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
static int Zlib_Decompress(ipf_data &comp,ipf_data &dest)
{
	z_stream z;
	int status;
	
	// initialize zlib
	z.zalloc = Z_NULL;z.zfree = Z_NULL;z.opaque = Z_NULL;
	z.next_in = Z_NULL;
	z.avail_in = 0;
	
	// RAW data use. must -MAX_WBITS
	status = inflateInit2(&z,-MAX_WBITS);
	if(status != Z_OK) return IPF_ERROR_ZLIBINIT;

	// buffer setting
	z.next_in = &comp[0];
	z.avail_in = comp.size();
	z.next_out = &dest[0];
	z.avail_out = dest.size();
	
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
		return status;
	}
	// exit
	if(inflateEnd(&z) != Z_OK){
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

// CRC check
int ipf_element::checkCRC(ipf_data &data)
{
	uLong crc = crc32(0L, Z_NULL, 0);
		
	crc = crc32(crc, (const Bytef *)((char *) &data[0]), data.size());
	if((uint32_t)crc == getCRC()) return IPF_OK;
	return IPF_ERROR_CRCMISS;
}

// Output to the data to read the file decompression
int ipf_element::uncompress(ifstream &fin,ipf_data &data)
{
	fin.seekg (getDataOffset(), ios::beg);
	if(!fin) return IPF_ERROR_FSEEKDECOMP;
	
	if(!isCompress()){
		// As it is read if it is not compressed
		data.resize(getComreessLength());
		fin.read((char *) &data[0], getComreessLength());
		if(! fin)	return IPF_ERROR_FREADDECOMP;
	}else{
		// If that has been compressed to decompress and decode
		ipf_data comp;
		int zstatus;
		
		comp.resize(getComreessLength());	
		fin.read((char *) &comp[0], getComreessLength());
		if(! fin)	return IPF_ERROR_FREADDECOMP;
		
		// ipf decoding because it is encrypted
		ipf_decrypt(IPF_COMPLESS_PASSWD,comp);

		// uncompress zip
		data.resize(getUnCompressLength());
		zstatus = Zlib_Decompress(comp,data);
		if(zstatus != IPF_OK){
			return zstatus;
		}
	}
	return checkCRC(data);
}

// Output to the data to extract the src
int ipf_element::uncompress(ipf_data &src,ipf_data &data)
{
	if(!isCompress()){
		// Ahead to ensure the memory area
		data.reserve(src.size());
		std::copy(src.begin(), src.end(), std::back_inserter(data));
	}else{
		int zstatus;
		
		// ipf decoding because it is encrypted
		ipf_decrypt(IPF_COMPLESS_PASSWD,src);

		// uncompress zip
		data.resize(getUnCompressLength());
		zstatus = Zlib_Decompress(src,data);
		if(zstatus != IPF_OK){
			return zstatus;
		}
	}
	return checkCRC(data);
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


