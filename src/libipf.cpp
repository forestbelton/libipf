/*
 * 
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "libipf.hpp"

using namespace std;

// decompress -------------------------------------------------
// read header and information from file
int  libipf_read_header(const string &fname,ifstream &fin,ipf_file &ipf_h,ipf_table &ftable)
{
	return ipf_h.read_header_fromfile(fname,fin,ftable);
}

// And extract of the specified file to ipf_info, return the thaw after the data in buf
int libipf_uncompress(std::ifstream &fin,ipf_element &ipf_info,ipf_data &buf)
{
	int ret=IPF_OK;
	if(! ipf_info.isEnable())	return IPF_ERROR_NOINFO;
	ret = ipf_info.uncompress(fin,buf);
	return ret;
}


// compress -------------------------------------------------
// write compress data from buf (and set ipf_info)
int libipf_compress(std::ofstream &fout,ipf_element &ipf_info,ipf_data &buf)
{
	int ret=IPF_OK;
	ret = ipf_info.compress(fout,buf);
	return ret;
}

// write file to IPF header and IPF infomation
int  libipf_write_header_info(ofstream &fout,ipf_file &ipf_h,ipf_table &ftable,uint32_t base_rev,uint32_t revision)
{
	return ipf_h.write_tofile(fout,ftable,base_rev,revision);
}


// dump -------------------------------------------------
void libipf_dump_headerinfo(ipf_file &ipf_h)
{
	printf("IPF file %s\n"			,ipf_h.getFileName().c_str());
	printf("IPF header info ---------------------------------\n");
	printf("file count %d\n"		,ipf_h.getFileCount());
	printf("table offset %08X\n"	,ipf_h.getFileTableOffset());
	printf("unknown %08X\n"			,ipf_h.getUnknown());
	printf("archive header %08X\n"	,ipf_h.getFileFooterOffset());
	printf("format %08X\n"			,ipf_h.getFormat());
	printf("base revision 0x%08X (%d)\n",ipf_h.getBaseRevision(),ipf_h.getBaseRevision());
	printf("revision 0x%08X (%d)\n"		,ipf_h.getRevision(),ipf_h.getRevision());
	printf("---------------------------------\n");
}

void libipf_dump_fileinfo(ipf_table &ftable)
{
	std::size_t i;
	printf("IPF file info ---------------------------------\n");
	printf(" Fnl  Anl      CRC         Comp       Uncomp   offset arcname filename\n");
	for(i=0;i<ftable.size();i++){
		printf("%4d %4d %08X %12d %12d %08X %s %s\n"
			,ftable[i].getFilenameLength()
			,ftable[i].getArcnameLength()
			,ftable[i].getCRC()
			,ftable[i].getCompressLength()
			,ftable[i].getUnCompressLength()
			,ftable[i].getDataOffset()
			,ftable[i].getArchiveName().c_str()
			,ftable[i].getFileName().c_str()
		);
	}
	printf("---------------------------------\n");
}


