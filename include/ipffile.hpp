#pragma once

/*
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "typedef.hpp"
#include "ipferror.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <limits.h>
#include <zlib.h>

// IPF file header
#pragma pack (push, 1)
typedef struct {
    uint16_t file_count;				// file in object count
    uint32_t filetable_offset;			// IPFInfo table address
    uint16_t unknown;					// unk
    uint32_t filefooter_offset;			// ????
    uint32_t format;					// IPF format now 0x06054B50
    uint32_t base_revision;				// IPF base revision
    uint32_t revision;					// IPF sub revision
} IPFHeader;
#pragma pack (pop)

// IPF files information
#pragma pack (push, 1)
typedef struct {
    uint16_t fname_length;				// file and path name length
    uint32_t crc;						// CRC
    uint32_t comp_length;				// compresed length
    uint32_t uncomp_length;				// uncompressed length
    uint32_t data_offset;				// file data offset
	uint16_t arcname_length;			// archive name length
} IPFInfo;
#pragma pack (pop)


typedef std::vector<uint8_t> ipf_data;

class ipf_element
{
protected:
	IPFInfo			m_info;
	
	std::string		m_FileName;				// file and path name (addon.ipf\aaa\bbb.lua)
	std::string		m_ArchiveName;			// archive name (xx.ipf)
	std::string		m_FullPath;				// file and folder full path
	
	bool			m_enable;

protected:
	void clear_info();
	
	uint32_t computeCRC(ipf_data &data);
	int checkCRC(ipf_data &data);

public:
	ipf_element(){
		clear_info();
	};
	
	~ipf_element(){};

	// uncompress --------
	bool set_infomation_fromfile(std::ifstream &fin);
	
	int uncompress(std::ifstream &fin,ipf_data &data);
	int uncompress(ipf_data &src,ipf_data &data);

	// compress --------
	bool write_infomation_tofile(std::ofstream &fout);
	
	int compress(std::ofstream &fout,ipf_data &data,int complevel);
	int compress(ipf_data &src,ipf_data &data,int complevel);
	
	
	bool write_data_tofile();

	inline bool 	isEnable()				{return m_enable;};
	
	inline uint16_t getFilenameLength()		{return m_info.fname_length;}
	inline uint32_t getCRC()				{return m_info.crc;}
	inline uint32_t getCompressLength()		{return m_info.comp_length;}
	inline uint32_t getUnCompressLength()	{return m_info.uncomp_length;}
	inline uint32_t getDataOffset()			{return m_info.data_offset;}
	inline uint16_t getArcnameLength()		{return m_info.arcname_length;}

	inline const std::string& getFileName()		{return m_FileName;};
	inline const std::string& getArchiveName()	{return m_ArchiveName;};
	inline const std::string& getFullPath()	const	{return m_FullPath;};
	
	inline void setDataOffset(uint32_t offset) {m_info.data_offset = offset;};
	inline void setFileName(const std::string &s) {
		m_FileName = s;
		m_info.fname_length = (uint16_t)m_FileName.size();
	};
	inline void setArchiveName(const std::string &s) {
		m_ArchiveName = s;
		m_info.arcname_length = (uint16_t)m_ArchiveName.size();
	};
	inline void setFullPath(const std::string &s) {
		m_FullPath = s;
	};
	inline void setUnCompressLength(uint32_t val) {
		m_info.uncomp_length = val;
	}

	inline bool isDeCompress()
	{
		return isCompress();
	};
	inline bool isCompress()
	{
		if(m_FileName.rfind(".jpg") != std::string::npos)	return false;
		if(m_FileName.rfind(".JPG") != std::string::npos)	return false;
		if(m_FileName.rfind(".fsb") != std::string::npos)	return false;
		if(m_FileName.rfind(".mp3") != std::string::npos)	return false;
		return true;
	};
	
};

typedef std::vector<ipf_element> ipf_table;


class ipf_file
{
protected:
	IPFHeader 		m_header;
	std::string		m_filename;

protected:
	void clear_header();
	int set_fileinfo_fromfile(std::ifstream &fin,ipf_table &ftable);
	
public:
	ipf_file()
	{
		clear_header();
	};
	~ipf_file(){};

	int read_header_fromfile(const std::string& fname,std::ifstream &fin,ipf_table &ftable);
	
	int write_tofile(std::ofstream &fout,ipf_table &ftable,uint32_t base_rev,uint32_t revision);

	inline uint16_t getFileCount()			{return m_header.file_count;}
	inline uint32_t getFileTableOffset()	{return m_header.filetable_offset;}
	inline uint16_t getUnknown()			{return m_header.unknown;}
	inline uint32_t getFileFooterOffset()	{return m_header.filefooter_offset;}
	inline uint32_t getFormat()				{return m_header.format;}
	inline uint32_t getBaseRevision()		{return m_header.base_revision;}
	inline uint32_t getRevision()			{return m_header.revision;}

	inline const std::string& getFileName()		{return m_filename;};
	
};

