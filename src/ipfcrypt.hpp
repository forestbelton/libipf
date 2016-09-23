#pragma once
/*
 * 
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include "typedef.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <zlib.h>

class ipf_crypto
{
protected:
	const z_crc_t	*crc_tbl;			// CRC tabke
	uint32_t		m_key[3];			// key value
protected:

	inline uint32_t compute_crc32(uint32_t c,char b){
		return crc_tbl[((int)c ^ b) & 0xff] ^ (c >> 8);
	};
	
	inline uint8_t decrypt_byte_factor()
	{
		uint16_t v = (uint16_t)((m_key[2] & 0xFFFF) | 2);
		return (uint8_t)(((v * (v ^ 1)) >> 8) & 0xFF);
	};
	
	// 1-Byte decrypter
	inline uint8_t decrypt_byte(uint8_t src)
	{
		uint8_t dest = src ^ decrypt_byte_factor();
		update(dest);
		return dest;
	};

	inline uint8_t encrypt_byte_factor(uint32_t prekey2)
	{
		uint16_t v = (uint16_t)((prekey2 & 0xFFFF) | 2);
		return (uint8_t)(((v * (v ^ 1)) >> 8) & 0xFF);
	};
	
	// 1-Byte encrypter
	inline uint8_t encrypt_byte(uint8_t src)
	{
		uint32_t prekey2 = m_key[2];
		update(src);
		uint8_t dest = src ^ encrypt_byte_factor(prekey2);
		return dest;
	};
	
	
	void crc_init();
	void update(char bv);
public:
	ipf_crypto(){
		crc_init();
	};
	
	~ipf_crypto(){};
	
	void init(const std::string &password);	
	void decrypt(std::vector<uint8_t> &cdata);
	void decrypt(const std::vector<uint8_t> &cdata,std::vector<uint8_t> &ddata);

	void encrypt(std::vector<uint8_t> &cdata);
	void encrypt(const std::vector<uint8_t> &cdata,std::vector<uint8_t> &ddata);
	
};

extern void ipf_decrypt(const std::string &password,std::vector<uint8_t> &cdata);
extern void ipf_decrypt(const std::string &password,const std::vector<uint8_t> &cdata,std::vector<uint8_t> &ddata);

extern void ipf_encrypt(const std::string &password,std::vector<uint8_t> &cdata);
extern void ipf_encrypt(const std::string &password,const std::vector<uint8_t> &cdata,std::vector<uint8_t> &ddata);



