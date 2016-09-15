#pragma once
/*
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "ipffile.hpp"

extern int  libipf_read_header(const std::string &fname,std::ifstream &fin,ipf_file &ipf_h,ipf_table &ftable);
extern int  libipf_uncompress(std::ifstream &fin,ipf_element &ipf_info,ipf_data &buf);
extern void libipf_dump_fileinfo(ipf_file &ipf_h,ipf_table &ftable);

