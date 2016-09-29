/*
 *  unipfDlg
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "stdafx.h"
#include "unipf.h"
#include "libipf.hpp"
#include "unipfDlg.h"
#include "CDirDialog.h"
#include "CDirectory.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

#include <stdarg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()



// フォルダ選択ダイアログを表示する共通関数
static bool DoModalFolderDlg(CString &rhdir,LPCTSTR ftype)
{
	CString tmpdir;
	CDirDialog dlg(TRUE, "", "", OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, ftype);

	dlg.m_bSelectableFolder=TRUE;
	if(! rhdir.IsEmpty()){
		dlg.m_ofn.lpstrInitialDir = rhdir;
	}
	if(dlg.DoModal() != IDOK) return false;

	CDirectory tmppath = CDirectory(dlg.GetPathName());

	if(tmppath.IsPathStrict() == FALSE){
		rhdir = tmppath.GetFilePath();
	}else{
		rhdir = tmppath;
	}

	if(rhdir.IsEmpty()){
		return false;
	}
	return true;
}




// unipfDlg ダイアログ

unipfDlg::unipfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(unipfDlg::IDD, pParent)
	, m_ipffname(_T(""))
	, m_ipfdirname(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void unipfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UNIPF_FILENAME, m_ipffname);
	DDX_Text(pDX, IDC_UNIPF_DIRNAME, m_ipfdirname);
	DDX_Control(pDX, IDC_UNIPF_VERBOSE, m_verbosemes);
}

BEGIN_MESSAGE_MAP(unipfDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UNIPF_FILE_BTN, &unipfDlg::OnBnClickedUnipfFileBtn)
	ON_BN_CLICKED(IDC_UNIPF_DIR_BTN, &unipfDlg::OnBnClickedUnipfDirBtn)
	ON_BN_CLICKED(IDC_UNIPF_INFO_BTN, &unipfDlg::OnBnClickedUnipfInfoBtn)
	ON_BN_CLICKED(IDC_UNIPF_UNCOMP_BTN, &unipfDlg::OnBnClickedUnipfUncompBtn)
END_MESSAGE_MAP()


// unipfDlg メッセージ ハンドラ

BOOL unipfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// CEditの限界をできる限り取る
	m_verbosemes.SetLimitText(0);	

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void unipfDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void unipfDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR unipfDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ファイル選択ボタン
void unipfDlg::OnBnClickedUnipfFileBtn()
{
	CFileDialog dlg(TRUE, _T("ipf"), _T("*.ipf"), OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_ENABLESIZING,
		_T("IPFファイル(*.ipf)|*.ipf|すべてのファイル(*.*)|*.*|"), NULL);

	dlg.m_ofn.lpstrInitialDir = m_ipffname;

	if(dlg.DoModal() == IDCANCEL){
		return;
	}
	CString fname = dlg.GetPathName();
	if(fname.IsEmpty()){
		return;
	}
	UpdateData(TRUE);
	m_ipffname = fname;
	UpdateData(FALSE);
}

// ディレクトリ選択
void unipfDlg::OnBnClickedUnipfDirBtn()
{
	CString dirname=m_ipfdirname;

	if(! DoModalFolderDlg(dirname,"all (*.*)|*.*||")) return; 

	UpdateData(TRUE);
	m_ipfdirname = dirname;
	UpdateData(FALSE);
}

void unipfDlg::PutVerbose(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[1200];

	nBuf = _vsnprintf(szBuffer, 1024, lpszFormat, args);

	int xlen = m_verbosemes.GetWindowTextLength();
	m_verbosemes.SetFocus();
	m_verbosemes.SetSel(xlen, xlen);
	m_verbosemes.ReplaceSel(szBuffer);

	va_end(args);
}


void unipfDlg::ClearVerbose()
{
	m_verbosemes.SetFocus();
	m_verbosemes.SetSel(0, -1);
	m_verbosemes.Clear();
}


bool unipfDlg::OpenIPFFile(ifstream &fin)
{
	int status;
	m_IpfInfo.clear();

	PutVerbose(_T("read IPF file %s...\r\n"),m_ipffname);

	fin.open(m_ipffname,ios::in | ios::binary);
	if(! fin.is_open()){
		PutVerbose(_T("ERROR: open file miss!! %s \r\n"),m_ipffname);
		return false;
	}
	status = libipf_read_header((LPCTSTR)m_ipffname,fin,m_Ipffile,m_IpfInfo);
	if(status != IPF_OK){
		PutVerbose(_T("ERROR: read header miss!! (%d)\r\n"),status);
		return false;
	}
	PutVerbose(_T("IPF file in file %lu \r\n\r\n"),m_IpfInfo.size());
	return true;
}


// 情報の表示ボタン
void unipfDlg::OnBnClickedUnipfInfoBtn()
{
	ifstream fin;
	ClearVerbose();

	if(m_ipffname.IsEmpty()){
		AfxMessageBox("IPFファイル名を指定してください");
		return;
	}

	if(! OpenIPFFile(fin)) return;

	std::size_t i;
	
	PutVerbose("IPF ファイル %s\r\n"				,m_Ipffile.getFileName().c_str());
	PutVerbose("IPF ヘッダ情報  ---------------------------------\r\n");
	PutVerbose("IPF内ファイル数 %d\r\n"				,m_Ipffile.getFileCount());
	PutVerbose("テーブルオフセット %08X\r\n"		,m_Ipffile.getFileTableOffset());
	PutVerbose("不明 %08X\r\n"						,m_Ipffile.getUnknown());
	PutVerbose("アーカイブヘッダオフセット %08X\r\n",m_Ipffile.getFileFooterOffset());
	PutVerbose("フォーマット %08X\r\n"				,m_Ipffile.getFormat());
	PutVerbose("ベースリビジョン %08X\r\n"			,m_Ipffile.getBaseRevision());
	PutVerbose("リビジョン %08X\r\n"				,m_Ipffile.getRevision());
	PutVerbose("---------------------------------\r\n");

	PutVerbose("IPF ファイルテーブル ---------------------------------\r\n");
	PutVerbose(" Fnl  Anl      CRC         Comp       Uncomp   offset arcname filename\r\n");
	for(i=0;i<m_IpfInfo.size();i++){
		PutVerbose(_T("%4d %4d %08X %12d %12d %08X %s %s\r\n")
			,m_IpfInfo[i].getFilenameLength()
			,m_IpfInfo[i].getArcnameLength()
			,m_IpfInfo[i].getCRC()
			,m_IpfInfo[i].getCompressLength()
			,m_IpfInfo[i].getUnCompressLength()
			,m_IpfInfo[i].getDataOffset()
			,m_IpfInfo[i].getArchiveName().c_str()
			,m_IpfInfo[i].getFileName().c_str()
		);
	}
	PutVerbose("---------------------------------\n");

}

// 解凍ボタン
void unipfDlg::OnBnClickedUnipfUncompBtn()
{
	ifstream fin;
	ClearVerbose();

	if(m_ipffname.IsEmpty()){
		AfxMessageBox("IPFファイル名を指定してください");
		return;
	}
	if(m_ipfdirname.IsEmpty()){
		AfxMessageBox("解凍先フォルダを指定してください");
		return;
	}

	if(! OpenIPFFile(fin)) return;

	std::size_t i;
	ipf_data fdata;
	int status;
	int scnt=0,ecnt=0;

	for(i=0;i<m_IpfInfo.size();i++){
		PutVerbose("ファイル解凍中... %s/%s \r\n",m_IpfInfo[i].getArchiveName().c_str(),m_IpfInfo[i].getFileName().c_str());
		// 解凍
		status = libipf_uncompress(fin,m_IpfInfo[i],fdata);
		if(status != IPF_OK){
			PutVerbose("ERROR: ファイルの解凍に失敗！ (%d)\r\n",status);
			ecnt++;
			continue;
		}
		// Directoryの作成。Filenameにパスが含まれていて/形式なのでWindowsの場合はややこしい処理が必要
		CDirectory dirname(m_ipfdirname);
		CDirectory wfname(m_IpfInfo[i].getFileName().c_str());

		dirname.Append(m_IpfInfo[i].getArchiveName().c_str());
		wfname.ReplaceLinuxPathtoWindowsPath();
		dirname.Append(wfname);
		wfname = dirname;
		dirname.RemoveFileSpec();
		// フォルダ無ければ作成
		if(! dirname.MakeDirectory()){
			PutVerbose("ERROR: フォルダ作成失敗 \r\n");
			ecnt++;
			continue;
		}
		// ファイル書き込み
		ofstream fout;
		fout.open(wfname, ios::out|ios::binary|ios::trunc);
		if(! fout){
			PutVerbose("ERROR: ファイル作成失敗 \r\n");
			ecnt++;
			continue;
		}
		// 展開後サイズが空の場合があるので一応チェックしてから書く
		if(! fdata.empty()){
			fout.write((char *) &fdata[0], (std::streamsize)fdata.size());
			if(fout.bad()){
				PutVerbose("ERROR: ファイル書き込み失敗 \r\n");
				ecnt++;
				fout.close();
				continue;
			}
		}
		fout.close();
		scnt++;
	}
	PutVerbose("ファイル解凍を終了しました (トータル %d/正常 %d/異常 %d)\r\n",m_IpfInfo.size(),scnt,ecnt);
}
