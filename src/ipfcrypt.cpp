/*
 * 
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "ipfcrypt.hpp"

using namespace std;

// Encrypted form of IPF is a method of basically the ZIP-passwd have been some changes

// It initializes the CRC table. Use in practice as it is of zlib is
void ipf_crypto::crc_init()
{
	crc_tbl = get_crc_table();
}

void ipf_crypto::init(const string &password)
{
	std::size_t i;
	m_key[0] = 0x12345678;
	m_key[1] = 0x23456789;
	m_key[2] = 0x34567890;
	
	for(i=0;i<password.size();i++){
		update(password[i]);
	}
}

void ipf_crypto::update(char bv)
{
	// Same as ZIP
	m_key[0] = compute_crc32(m_key[0],bv);
	m_key[1] += (m_key[0] & 0xff);
    m_key[1] = m_key[1] * 134775813L + 1;
	m_key[2] = compute_crc32(m_key[2],m_key[1] >> 24);
}

// Performs decoding processing of cdata, to override the results to the cdata
void ipf_crypto::decrypt(vector<uint8_t> &cdata)
{
	std::size_t i;
	
	// Unlike the case of Zip, 2Byte eyes have been using the original value
	// [Crypt][raw][Crypt][raw].....
	for(i=0;i<cdata.size();i+=2){
		cdata[i] = decrypt_byte(cdata[i]);
	}
}

// Performs decoding processing of cdata, put the result in ddata
void ipf_crypto::decrypt(const vector<uint8_t> &cdata, vector<uint8_t> &ddata)
{
	std::size_t i;
	
	ddata.resize(cdata.size());
	for(i=0;i<cdata.size();i+=2){
		ddata[i] = decrypt_byte(cdata[i]);
	}
}


void ipf_decrypt(const string &password,vector<uint8_t> &cdata)
{
	ipf_crypto ipfc;
	ipfc.init(password);
	ipfc.decrypt(cdata);
}

void ipf_decrypt(const string &password,const vector<uint8_t> &cdata,vector<uint8_t> &ddata)
{
	ipf_crypto ipfc;
	ipfc.init(password);
	ipfc.decrypt(cdata,ddata);
}


