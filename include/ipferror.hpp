#pragma once

/*
 * Error define
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define IPF_OK					(0)			// No Error
#define IPF_ERROR_FOPEN			(-1)		// File open miss
#define IPF_ERROR_NOINFO		(-2)		// Info not found
#define IPF_ERROR_FREADINFO		(-3)		// Info read error
#define IPF_ERROR_FSEEKHEAD		(-4)		// failed seek head
#define IPF_ERROR_FREADHEAD		(-5)		// read header miss
#define IPF_ERROR_CRCMISS		(-6)		// data crc miss
#define IPF_ERROR_FSEEKDECOMP	(-7)		// file seek error(decompress)
#define IPF_ERROR_FREADDECOMP	(-8)		// file read error(decompress)
#define IPF_ERROR_ZLIBINIT		(-9)		// error Zlib init
#define IPF_ERROR_ZLIBEND		(-10)		// error Zlib exit
#define IPF_ERROR_ZLIBDATA		(-11)		// not zip file (broken file)
#define IPF_ERROR_ZLIBMEM		(-12)		// zip buffer empty (broken file)
#define IPF_ERROR_ZLIBUNKNOWN	(-13)		// unknown error Zlib
#define IPF_ERROR_HEADER_FORMAT (-14)		// not support format

