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
#include <algorithm>
#include <functional>

#include <getopt.h>
#include "libipf.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>

using namespace std;

// バージョン
static const string THIS_VERSION	= "1.0.0";


class cmdlineOption
{
	#define DIM_PARAM(TYPE,NAME) \
	protected: TYPE	NAME; \
	public: inline const TYPE get##NAME() {return NAME;} \
			inline void set##NAME(const TYPE &val) {NAME=val;}; 
	
	DIM_PARAM(bool,Extract)
	DIM_PARAM(bool,Create)
	DIM_PARAM(bool,Fhead)
	DIM_PARAM(bool,Info)
	DIM_PARAM(bool,Dryrun)
	DIM_PARAM(bool,Verbose)
	
	DIM_PARAM(std::string,Fname)
	DIM_PARAM(std::string,Dirname)
	
	DIM_PARAM(int,Revision)
	DIM_PARAM(int,Baserev)
	DIM_PARAM(int,Complevel)
	

	#undef DIM_PARAM

	public:
	cmdlineOption(){
		Extract 		= false;
		Create 			= false;
		Info		 	= false;
		Fhead			= false;
		Dryrun			= false;
		Verbose			= false;
		Revision		= 0;
		Baserev			= 0;
		Complevel		= 1;		// -1はZ_DEFAULT_COMPRESSIONだけど、ファイルをみたらあまり圧縮してないので
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
		 "-f | --fhead         Print file header information\n"
		 "-i | --info          Print file information\n"
		 "-d | --dryrun        Dry run (not run uncompressing)\n"
		 "-v | --verbose       Verbose\n"
		 "\n"
		 "[Example]\n"
		 "uncompress sample.ipf to exp dir\n"
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
		 "-r | --revision num  write revision number\n"
		 "-b | --baserev  num  write BASE revision number\n"
		 "-c | --clevel num    compression level (0-9) 0 is nocompression \n"
		 "-i | --info          Print file information after compression\n"
		 "-h | --help          Print this message\n"
		 "-v | --verbose       Verbose\n"
		 "\n"
		 "[Example]\n"
		 "compress sample.ipf from exp dir (base rev 134515 rev 136528)\n"
		 "ipf -b 134515 -r 136528 sample.ipf sample \n"
		 "",
		 THIS_VERSION.c_str(),argv[0]
		);
	}
}

namespace CMDLINE_OPT
{
	enum {
		PUT_HELP	= 1,
		FHEAD,
		INFO,
		DRYRUN,
		BASEREV,
		REVISION,
		CLEVEL,
		VERBOSE,
	};
};

static const struct option
long_options[] = {
	{ "fhead",		no_argument,		NULL, CMDLINE_OPT::FHEAD		},
	{ "info",		no_argument,		NULL, CMDLINE_OPT::INFO			},
	{ "dryrun",		no_argument,		NULL, CMDLINE_OPT::DRYRUN		},
	{ "baserev",	required_argument,	NULL, CMDLINE_OPT::BASEREV		},
	{ "revision",	required_argument,	NULL, CMDLINE_OPT::REVISION		},
	{ "clevel",		required_argument,	NULL, CMDLINE_OPT::CLEVEL		},
	{ "help",		no_argument,		NULL, CMDLINE_OPT::PUT_HELP		},
	{ "verbose",	no_argument,		NULL, CMDLINE_OPT::VERBOSE		},
	{ 0, 0, 0, 0 }
};


// コマンドラインの解析
bool mainApp::parse_cmdline(int argc,char *argv[])
{
	int num;
	while(1){
		int c = getopt_long_only(argc,argv,"",long_options,NULL);
		
		if(c == -1)		break;		// -1は解析終わり
		switch (c) {
		case 0:
			break;
		case CMDLINE_OPT::PUT_HELP:
			usage(argc, argv);
			return false;
			
		case CMDLINE_OPT::FHEAD:
			opt.setFhead(true);
			break;
			
		case CMDLINE_OPT::INFO:
			opt.setInfo(true);
			break;

		case CMDLINE_OPT::DRYRUN:
			opt.setDryrun(true);
			break;

		case CMDLINE_OPT::BASEREV:
			num = atoi(optarg);
			opt.setBaserev(num);
			break;

		case CMDLINE_OPT::REVISION:
			num = atoi(optarg);
			opt.setRevision(num);
			break;

		case CMDLINE_OPT::CLEVEL:
			num = atoi(optarg);
			if((num < 0) || (num > 9)){
				num = -1;
			}
			opt.setComplevel(num);
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
	string dname;
	dname = argv[optind++];
	if(dname[dname.size()-1] == '/'){
		dname.erase(dname.size() - 1, 1);
	}
	opt.setDirname(dname);
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
		if(opt.getFhead()){
			libipf_dump_headerinfo(m_Ipf);
		}
		return -1;
	}
	printf("IPF file in file %lu \n",flist.size());
	
	if(opt.getFhead()){
		libipf_dump_headerinfo(m_Ipf);
	}
	if(opt.getInfo()){
		libipf_dump_fileinfo(flist);
	}
	if(opt.getDryrun()){
		return 0;
	}
	
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
		if(! fdata.empty()){
			fout.write((char *) &fdata[0], fdata.size());
			if(fout.bad()){
				return -1;
			}
		}
		fout.close();
	}
	printf("done!\n");
	return 0;
}

static int createFilelist(string dirname,ipf_table &ipffiles,const string &arcname,string prefname)
{
	struct	dirent *dp;
	struct stat fst;
	DIR	*dir;
	
	dirname += "/";
	if((dir = opendir(dirname.c_str()))==NULL){
		printf("ERROR: can not open directory %s \n",dirname.c_str());
		return -1;
	}
	while((dp = readdir(dir)) != NULL){
		string path;
		if(strcmp("..",dp->d_name) == 0) continue;
		if(strcmp(".",dp->d_name) == 0) continue;
		path = dirname + dp->d_name;
		if(stat(path.c_str(), &fst) == -1) continue;
		if(S_ISDIR(fst.st_mode)){
			string newpref = prefname + dp->d_name + "/";
			if(createFilelist(path,ipffiles,arcname,newpref) != 0){
				closedir(dir);
				return -1;
			}
		}else{
			ipf_element element;
			string fname = prefname + dp->d_name;
			
			element.setFullPath(path);
			element.setArchiveName(arcname);
			element.setFileName(fname);
			element.setUnCompressLength((uint32_t)fst.st_size);
			
			ipffiles.push_back(element);
		}
	}
	closedir(dir);	
	return 0;
}


// IPFファイルリストは大文字に変換した上で比較する必要があるため
bool compare_fname_nocase_upper(const ipf_element& left, const ipf_element& right)
{
	string upl,upr;
	
	upl.resize(left.getFullPath().size());
	std::transform(
		left.getFullPath().begin(),
		left.getFullPath().end(),
		upl.begin(),
		::toupper);
	
	upr.resize(right.getFullPath().size());
	std::transform(
		right.getFullPath().begin(),
		right.getFullPath().end(),
		upr.begin(),
		::toupper);
	
	return upl < upr;
}


// IPFの作成を行う
int mainApp::doCreateIPF()
{
	struct	dirent *dp;
	DIR	*dir;
	struct stat fst;
	ipf_table ipffiles;
	std::size_t i;
	string parent_path = opt.getDirname() + "/";
	int status;
	
	printf("reading file list....\n");

	// Archive/file の構造なので、まずはアーカイブ名のリストを作る
	if((dir = opendir(opt.getDirname().c_str()))==NULL){
		printf("ERROR: can not open directory %s \n",opt.getDirname().c_str());
		return -1;
	}
	while((dp = readdir(dir)) != NULL){
		string path;
		if(strcmp("..",dp->d_name) == 0) continue;
		if(strcmp(".",dp->d_name) == 0) continue;
		path = parent_path + dp->d_name;
		if(stat(path.c_str(), &fst) == -1) continue;
		if(! S_ISDIR(fst.st_mode)) continue;
		createFilelist(path,ipffiles,dp->d_name,"");
	}
	closedir(dir);
	// 昇順ソート(大文字小文字区別なし＆大文字ベース)
	std::sort(ipffiles.begin(),ipffiles.end(),compare_fname_nocase_upper);
	printf("done....\n");

	ofstream fout;
	printf("IPF file in file %lu \n",ipffiles.size());

	// 圧縮の開始
	fout.open(opt.getFname().c_str(),ios::out | ios::binary);
	if(! fout.is_open()){
		printf("ERROR: open file miss!! %s \n",opt.getFname().c_str());
		return -1;
	}
	for(i=0;i<ipffiles.size();i++){
		ifstream fin;
		ipf_data rdata;
		printf("compress data...%s %s \n",ipffiles[i].getArchiveName().c_str(),ipffiles[i].getFileName().c_str());
		
		fin.open(ipffiles[i].getFullPath().c_str(),ios::in | ios::binary);
		if(! fin.is_open()){
			printf("ERROR: open file miss!! %s \n",ipffiles[i].getFullPath().c_str());
			fin.close();
			return -1;
		}
		rdata.resize(ipffiles[i].getUnCompressLength());
		fin.read((char *) &rdata[0], ipffiles[i].getUnCompressLength());
		if(!fin){
			fin.close();
			return -1;
		}
		fin.close();
		
		status = libipf_compress(fout,ipffiles[i],rdata,opt.getComplevel());
		if(status != IPF_OK){
			printf("ERROR: compress data miss!! (%d)\n",status);
			return -1;
		}
	}
	status = libipf_write_header_info(
		fout,
		ipffiles,
		opt.getBaserev(),
		opt.getRevision()
	);
	fout.close();
	if(status != IPF_OK){
		printf("ERROR: header write miss!! (%d)\n",status);
		return -1;
	}
	printf("done! \n");
	
	if(opt.getInfo()){
		libipf_dump_fileinfo(ipffiles);
	}
	
	return 0;
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


