#pragma once

/*
 * typedef header
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include <cstddef>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <windef.h>
#define PATH_MAX MAX_PATH 
#endif
