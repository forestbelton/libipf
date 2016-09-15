/*
 * ipf/unipf sample
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <getopt.h>

#include "libipf.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

using namespace std;

// バージョン
static const string THIS_VERSION	= "0.0.1";


class cmdlineOption
{
	#define DIM_PARAM(TYPE,NAME) \
	protected: TYPE	NAME; \
	public: inline const TYPE get##NAME() {return NAME;} \
			inline void set##NAME(const TYPE &val) {NAME=val;}; 
	
	DIM_PARAM(bool,Extract)
	DIM_PARAM(bool,Create)
	DIM_PARAM(bool,Info)
	DIM_PARAM(bool,Verbose)
	
	DIM_PARAM(std::string,Fname)
	DIM_PARAM(std::string,Dirname)

	#undef DIM_PARAM

	public:
	cmdlineOption(){
		Extract 		= false;
		Create 			= false;
		Info		 	= false;
		Verbose			= false;
	}
	~cmdlineOption(){};
};

class mainApp
{
protected:
	cmdlineOption		opt;

	void usage(int argc, char *argv[]);
	bool parse_cmdline(int argc, char *argv[]);
	
	int doExtractIPF();
	int doCreateIPF();
	
	ipf_file	m_Ipf;

public:
	mainApp(){}
	~mainApp(){}

	int DoMain(int argc, char *argv[]);
};


// ディレクトリ作成
static int makedir(const string &dirname,mode_t mode)
{
	return mkdir(dirname.c_str(),mode);
}

static void makepath(const string &dirname)
{
	size_t found = 0,st=0;;
	found = dirname.find("/");
	while(found != string::npos){
		string dirn = dirname.substr(0,found);
		makedir(dirn,0755);
		st = found+1;
		found = dirname.find("/",st);
	}
}



// 使用方法の表示
void mainApp::usage(int argc, char *argv[])
{
	if(opt.getExtract()){
		fprintf(stdout,
		 "ipf extractor [%s]\n"
		 "Usage: %s [options] FileName dirname\n"
		 "\n"
		 "[Options]\n"
		 "-h | --help          Print this message\n"
		 "-i | --info          Print file information\n"
		 "-v | --verbose       Verbose\n"
		 "\n"
		 "[Example]\n"
		 "uncompress sampleipf to exp dir\n"
		 "unipf sample.ipf exp\n"
		 "\n"
		 "",
		 THIS_VERSION.c_str(),argv[0]
		);
	}else{
		fprintf(stdout,
		 "ipf creator [%s]\n"
		 "Usage: %s [options] ipffile dirname\n"
		 "\n"
		 "-h | --help          Print this message\n"
		 "-v | --verbose       Verbose\n"
		 "\n"
		 "[Example]\n"
		 "ipf sample.ipf sample \n"
		 "",
		 THIS_VERSION.c_str(),argv[0]
		);
	}
}

namespace CMDLINE_OPT
{
	enum {
		PUT_HELP	= 1,
		INFO,
		VERBOSE,
	};
};

static const struct option
long_options[] = {
	{ "info",		no_argument,		NULL, CMDLINE_OPT::INFO			},
	{ "help",		no_argument,		NULL, CMDLINE_OPT::PUT_HELP		},
	{ "verbose",	no_argument,		NULL, CMDLINE_OPT::VERBOSE		},
	{ 0, 0, 0, 0 }
};


// コマンドラインの解析
bool mainApp::parse_cmdline(int argc,char *argv[])
{
	while(1){
		int c = getopt_long_only(argc,argv,"",long_options,NULL);
		
		if(c == -1)		break;		// -1は解析終わり
		switch (c) {
		case 0:
			break;
		case CMDLINE_OPT::PUT_HELP:
			usage(argc, argv);
			return false;
			
		case CMDLINE_OPT::INFO:
			opt.setInfo(true);
			break;

		case CMDLINE_OPT::VERBOSE:
			opt.setVerbose(true);
	        break;

		default:
	        usage(argc, argv);
			return false;
		}
	}
	
	if (optind >= argc) {
		usage(argc, argv);
		return false;
	}
	opt.setFname(argv[optind++]);
//	printf("fname: %s\n",opt.getFname().c_str());
	if (optind >= argc) {
		if(opt.getExtract()) return true;
		
		usage(argc, argv);
		return false;
	}
	opt.setDirname(argv[optind++]);
//	printf("Dname: %s\n",opt.getDirname().c_str());
	
	return true;
}

// IPFの展開を行う
int mainApp::doExtractIPF()
{
	std::size_t i;
	
	ipf_table flist;
	ipf_data fdata;
	int status;
	
	ifstream fin;
	string wfilename;
	
	// 出力先が省略された場合はresult以下に書くようにする
	if(opt.getDirname().empty()){
		opt.setDirname("result");
	}
	printf("read IPF file %s...\n",opt.getFname().c_str());
	fin.open(opt.getFname().c_str(),ios::in | ios::binary);
	if(! fin.is_open()){
		printf("ERROR: open file miss!! %s \n",opt.getFname().c_str());
		return -1;
	}
	
	status = libipf_read_header(opt.getFname().c_str(),fin,m_Ipf,flist);
	if(status != IPF_OK){
		printf("ERROR: read header miss!! (%d)\n",status);
		return -1;
	}
	printf("IPF file in file %lu \n",flist.size());
	
	if(opt.getInfo()){
		libipf_dump_fileinfo(m_Ipf,flist);
	}
	
	// TESTEST -------------------------------------------------------------
	for(i=0;i<flist.size();i++){
		printf("uncompress data... %s \n",flist[i].getFileName().c_str());
		status = libipf_uncompress(fin,flist[i],fdata);
		if(status != IPF_OK){
			printf("ERROR: uncompress data miss!! (%d)\n",status);
			return -1;
		}
		wfilename = opt.getDirname() + "/" + flist[i].getArchiveName() + "/" + flist[i].getFileName();
		makepath(wfilename);
		ofstream fout;
		fout.open(wfilename.c_str(), ios::out|ios::binary|ios::trunc);
		
		if(! fout){
			printf("ERROR: file create error \n");
			return -1;
		}
		
		fout.write((char *) &fdata[0], fdata.size());
		if(fout.bad()){
			return -1;
		}
		fout.close();
	}
	printf("done!\n");
	return 0;
}

// IPFの作成を行う
int mainApp::doCreateIPF()
{
	printf("sorry.. undefined \n");
	return -1;
}

int mainApp::DoMain(int argc,char *argv[])
{
	if(strstr(argv[0],"unipf") != NULL)	opt.setExtract(true);
	else								opt.setCreate(true);
	
	if(argc == 1){
		usage(argc, argv);
		return -1;
	}
	// コマンドライン解析
	if(! parse_cmdline(argc,argv)){
		return -1;
	}
	
	if(opt.getExtract()){
		return doExtractIPF();
	}
	if(opt.getCreate()){
		return doCreateIPF();
	}
	
	return -1;
}

int main(int argc,char *argv[])
{
	mainApp app;
	return app.DoMain(argc,argv);
}


