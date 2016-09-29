/*
 * 
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "ipffile.hpp"
#include "ipfcrypt.hpp"

using namespace std;

#define IPF_HEADER_FORMAT (0x06054B50)
static const string IPF_COMPLESS_PASSWD = "ofO1a0ueXA? [\xFFs h %?";

// Decompress using Zlib
static int Zlib_Decompress(ipf_data &comp,ipf_data &dest,uint32_t &total_out)
{
	z_stream z;
	int status;
	
	// initialize zlib param
	z.zalloc = Z_NULL;z.zfree = Z_NULL;z.opaque = Z_NULL;
	total_out = 0;
	// buffer setting
	z.next_in = &comp[0];
	z.avail_in = comp.size();
	z.next_out = &dest[0];
	z.avail_out = dest.size();

	// zlib INIT. RAW data use. must -MAX_WBITS
	status = inflateInit2(&z,-MAX_WBITS);
	if(status != Z_OK) return IPF_ERROR_ZLIBINIT;
	
	// uncompress
	status = inflate(&z, Z_FINISH);
	if((status != Z_OK) && (status != Z_STREAM_END)){
		switch(status){
		case Z_STREAM_ERROR:
		case Z_DATA_ERROR:
			status = IPF_ERROR_ZLIBDATA;
			break;
		case Z_MEM_ERROR:
		case Z_BUF_ERROR:
			status = IPF_ERROR_ZLIBMEM;
			break;
		default:
			status = IPF_ERROR_ZLIBUNKNOWN;
		}
		inflateEnd(&z);
		return status;
	}
	total_out = z.total_out;
	// exit
	if(inflateEnd(&z) != Z_OK){
		return IPF_ERROR_ZLIBEND;
	}
	return IPF_OK;
}

// Compress using Zlib
static int Zlib_Compress(ipf_data &data,ipf_data &dest,uint32_t &total_out)
{
	z_stream z;
	int status;
	
	// initialize zlib param
	z.zalloc = Z_NULL;z.zfree = Z_NULL;z.opaque = Z_NULL;
	total_out = 0;
	// buffer setting
	z.next_in = &data[0];
	z.avail_in = data.size();
	z.next_out = &dest[0];
	z.avail_out = dest.size();

	// zlib INIT. RAW data use. must -MAX_WBITS
	status = deflateInit2(&z,
		Z_DEFAULT_COMPRESSION,
		Z_DEFLATED,
		-MAX_WBITS,
		8,
		Z_DEFAULT_STRATEGY
	);
	
	if(status != Z_OK) return IPF_ERROR_ZLIBINIT;

	
	// uncompress
	status = deflate(&z, Z_FINISH);
	if((status != Z_OK) && (status != Z_STREAM_END)){
		switch(status){
		case Z_STREAM_ERROR:
		case Z_DATA_ERROR:
			status = IPF_ERROR_ZLIBDATA;
			break;
		case Z_MEM_ERROR:
		case Z_BUF_ERROR:
			status = IPF_ERROR_ZLIBMEM;
			break;
		default:
			status = IPF_ERROR_ZLIBUNKNOWN;
		}
		deflateEnd(&z);
		return status;
	}
	total_out = z.total_out;
	// exit
	if(deflateEnd(&z) != Z_OK){
		return IPF_ERROR_ZLIBEND;
	}
	return IPF_OK;
}



// ipf_element -----------------------------------------------------------------------------------------------

// clear information
void ipf_element::clear_info()
{
	memset(&m_info,0,sizeof(IPFInfo));
	m_FileName.clear();
	m_ArchiveName.clear();
	m_enable = false;
}

// read information from file stream
bool ipf_element::set_infomation_fromfile(ifstream &fin)
{
	char fname[PATH_MAX];
	char arcname[PATH_MAX];
	
	if(!fin) return false;
	fin.read((char *) &m_info, sizeof(IPFInfo));	
	if(!fin) return false;
	fin.read(arcname, getArcnameLength());
	if(!fin) return false;
	fin.read(fname, getFilenameLength());
	if(!fin) return false;
	
	arcname[getArcnameLength()] = '\0';
	fname[getFilenameLength()] = '\0';
	
	m_FileName = fname;
	m_ArchiveName = arcname;
	m_enable = true;
	return true;
}

// write information to file stream
bool ipf_element::write_infomation_tofile(ofstream &fout)
{
	// check param
	if(getFileName().empty()) return false;
	if(getArchiveName().empty()) return false;
	if(getFilenameLength() <= 0) return false;
	if(getArcnameLength() <= 0) return false;
	if(getDataOffset() <= 0) return false;
	
	if(!fout) return false;
	fout.write((char *) &m_info, sizeof(IPFInfo));	
	if(!fout) return false;
	fout.write(getArchiveName().c_str(), getArcnameLength()-1);
	if(!fout) return false;
	fout.write(getFileName().c_str(), getFilenameLength()-1);
	if(!fout) return false;
	
	return true;
}


// CRC32 calculation
uint32_t ipf_element::computeCRC(ipf_data &data)
{
	uLong crc = crc32(0L, Z_NULL, 0);
	return (uint32_t)crc32(crc, (const Bytef *)((char *) &data[0]), data.size());
}

// CRC32 check
int ipf_element::checkCRC(ipf_data &data)
{
	if(computeCRC(data) == getCRC()) return IPF_OK;
	return IPF_ERROR_CRCMISS;
}



// Output to the data to read the file decompression
int ipf_element::uncompress(ifstream &fin,ipf_data &data)
{
	if(getUnCompressLength() == 0){
		data.clear();
		return IPF_OK;
	}
	fin.seekg (getDataOffset(), ios::beg);
	if(!fin) return IPF_ERROR_FSEEKDECOMP;
	
	if(!isDeCompress()){
		// As it is read if it is not compressed
		data.resize(getCompressLength());
		fin.read((char *) &data[0], getCompressLength());
		if(! fin)	return IPF_ERROR_FREADDECOMP;
	}else{
		// If that has been compressed to decompress and decode
		ipf_data comp;
		int zstatus;
		uint32_t uncomp_len = 0;
		
		comp.resize(getCompressLength());	
		fin.read((char *) &comp[0], getCompressLength());
		if(! fin)	return IPF_ERROR_FREADDECOMP;
		
		// ipf decoding because it is encrypted
		ipf_decrypt(IPF_COMPLESS_PASSWD,comp);

		// uncompress zip
		data.resize(getUnCompressLength());
		zstatus = Zlib_Decompress(comp,data,uncomp_len);
		if(zstatus != IPF_OK){
			return zstatus;
		}
	}
	return checkCRC(data);
}

// Output to the data to extract the src
int ipf_element::uncompress(ipf_data &src,ipf_data &data)
{
	if(getUnCompressLength() == 0){
		data.clear();
		return IPF_OK;
	}
	if(!isDeCompress()){
		// Ahead to ensure the memory area
		data.reserve(src.size());
		std::copy(src.begin(), src.end(), std::back_inserter(data));
	}else{
		int zstatus;
		uint32_t uncomp_len = 0;
		
		// ipf decoding because it is encrypted
		ipf_decrypt(IPF_COMPLESS_PASSWD,src);

		// uncompress zip
		data.resize(getUnCompressLength());
		zstatus = Zlib_Decompress(src,data,uncomp_len);
		if(zstatus != IPF_OK){
			return zstatus;
		}
	}
	return checkCRC(data);
}

// Data output
int ipf_element::compress(ofstream &fout,ipf_data &data)
{
	m_info.uncomp_length = data.size();
	m_info.comp_length = 0;
	setDataOffset((uint32_t)fout.tellp());
	
	if(getUnCompressLength() == 0){
		char dmydata[10];
		// set dummy data
		m_info.comp_length = 2;
		m_info.crc = 0;
		fout.write(dmydata, getCompressLength());
		if(! fout)	return IPF_ERROR_FWRITECOMP;
		return IPF_OK;
	}
	
	if(!isCompress()){
		// it is no compression object
		m_info.comp_length = m_info.uncomp_length;
		// CRC32
		m_info.crc = computeCRC(data);
		// write file
		fout.write((const char *) &data[0], getCompressLength());
		if(! fout)	return IPF_ERROR_FWRITECOMP;
	}else{
		// If that has been compressed
		ipf_data comp;
		int zstatus;
		uint32_t comp_len = 0;
		
		comp.resize(getUnCompressLength() * 2);		// tenuki
		// compress zip
		zstatus = Zlib_Compress(data,comp,comp_len);
		if(zstatus != IPF_OK){
			return zstatus;
		}
		// resize compress data buffer
		m_info.comp_length = comp_len;
		comp.resize(getCompressLength());
		// CRC32
		m_info.crc = computeCRC(comp);
		
		// ipf encoding
		ipf_encrypt(IPF_COMPLESS_PASSWD,comp);
		fout.write((const char *) &comp[0], getCompressLength());
		if(! fout)	return IPF_ERROR_FWRITECOMP;
	}
	return IPF_OK;
}

// Output to the data to compress the src
int ipf_element::compress(ipf_data &src,ipf_data &data)
{
	m_info.uncomp_length = src.size();
	m_info.comp_length = 0;
	
	if(getUnCompressLength() == 0){
		// set dummy data
		m_info.comp_length = 2;
		m_info.crc = 0;
		data.resize(getCompressLength()); // empty data
		return IPF_OK;
	}
	
	if(!isCompress()){
		// Ahead to ensure the memory area
		m_info.comp_length = m_info.uncomp_length;
		data.reserve(src.size());
		std::copy(src.begin(), src.end(), std::back_inserter(data));
		// CRC32
		m_info.crc = computeCRC(data);		
	}else{
		int zstatus;
		uint32_t comp_len = 0;
		
		data.resize(getUnCompressLength() * 2);		// tenuki
		// compress zip
		zstatus = Zlib_Compress(src,data,comp_len);
		if(zstatus != IPF_OK){
			return zstatus;
		}
		// resize compress data buffer
		m_info.comp_length = comp_len;
		data.resize(getCompressLength());
		// CRC32
		m_info.crc = computeCRC(data);
		// ipf encoding
		ipf_encrypt(IPF_COMPLESS_PASSWD,data);
	}
	return IPF_OK;
}


// ipf_file -----------------------------------------------------------------------------------------------

// clear information
void ipf_file::clear_header()
{
	memset(&m_header,0,sizeof(IPFHeader));
	m_filename.clear();
}


// read ipf file informations from file stream
int ipf_file::set_fileinfo_fromfile(ifstream &fin,ipf_table &ftable)
{
	std::size_t i;
	
	// The location of the information structure has been written in the table offset
	fin.clear();
	fin.seekg (getFileTableOffset(), ios::beg);

	for(i=0;i<getFileCount();i++){
		ipf_element element;
		if(!element.set_infomation_fromfile(fin)){
			return IPF_ERROR_FREADINFO;
		}
		ftable.push_back(element);
	}
	return IPF_OK;
}

// read IPF file and put ftable
int ipf_file::read_header_fromfile(const string &fname,ifstream &fin,ipf_table &ftable)
{
	clear_header();
	m_filename = fname;
	
	// header is file last offset -24( [24] size of IPFHeader)
	fin.seekg (-sizeof(IPFHeader), ios::end);
	if(!fin)	return IPF_ERROR_FSEEKHEAD;

	// read header
	fin.read((char *) &m_header, sizeof(IPFHeader));
	if(!fin)	return IPF_ERROR_FREADHEAD;
	
	if(IPF_HEADER_FORMAT != getFormat()) return IPF_ERROR_HEADER_FORMAT;

	// read files information 
	return set_fileinfo_fromfile(fin,ftable);
}


// write ipf file informations to file stream
int ipf_file::write_tofile(ofstream &fout,ipf_table &ftable,uint32_t base_rev,uint32_t revision)
{
	std::size_t i;

	// initialize info
	m_header.filetable_offset = (uint32_t)fout.tellp();
	m_header.file_count = (uint16_t)ftable.size();
	m_header.unknown = 0;
	m_header.format = IPF_HEADER_FORMAT;
	m_header.base_revision = base_rev;
	m_header.revision = revision;

	for(i=0;i<getFileCount();i++){
		if(!ftable[i].write_infomation_tofile(fout)){
			return IPF_ERROR_FWRITEINFO;
		}
	}
	m_header.filefooter_offset = (uint32_t)fout.tellp();
	
	fout.write((char *) &m_header, sizeof(IPFHeader));
	if(!fout)	return IPF_ERROR_FWRITEHEAD;
	
	return IPF_OK;
}


