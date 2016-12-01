// ipfpackDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ipfpack.h"
#include "libipf.hpp"
#include "ipfpackDlg.h"
#include "ipfdetailDlg.h"
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
#include <algorithm>
#include <functional>

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



// ipfpackDlg ダイアログ

ipfpackDlg::ipfpackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ipfpackDlg::IDD, pParent)
	, m_ipfdirname(_T(""))
	, m_ipffname(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ipfpackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IPFPACK_DIRNAME, m_ipfdirname);
	DDX_Text(pDX, IDC_IPFPACK_FILENAME, m_ipffname);
	DDX_Control(pDX, IDC_IPFPACK_VERBOSE, m_verbosemes);
}

BEGIN_MESSAGE_MAP(ipfpackDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_IPFPACK_DETAIL_BTN, &ipfpackDlg::OnBnClickedIpfpackDetailBtn)
	ON_BN_CLICKED(IDC_IPFPACK_COMP_BTN, &ipfpackDlg::OnBnClickedIpfpackCompBtn)
	ON_BN_CLICKED(IDC_IPFPACK_DIR_BTN, &ipfpackDlg::OnBnClickedIpfpackDirBtn)
	ON_BN_CLICKED(IDC_IPFPACK_FILE_BTN, &ipfpackDlg::OnBnClickedIpfpackFileBtn)
END_MESSAGE_MAP()


// ipfpackDlg メッセージ ハンドラ

BOOL ipfpackDlg::OnInitDialog()
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

	// TODO: 初期化をここに追加します。
	m_base_rev = 0;
	m_revision = 0;
	m_comp_level = 1;

	// CEditの限界をできる限り取る
	m_verbosemes.SetLimitText(0);	

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void ipfpackDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void ipfpackDlg::OnPaint()
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
HCURSOR ipfpackDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ディレクトリ選択
void ipfpackDlg::OnBnClickedIpfpackDirBtn()
{
	CString dirname=m_ipfdirname;

	if(! DoModalFolderDlg(dirname,"all (*.*)|*.*||")) return; 

	UpdateData(TRUE);
	m_ipfdirname = dirname;
	UpdateData(FALSE);
}

// ファイル選択ボタン
void ipfpackDlg::OnBnClickedIpfpackFileBtn()
{
	CFileDialog dlg(FALSE, _T("ipf"), _T("*.ipf"), OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_ENABLESIZING,
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

void ipfpackDlg::PutVerbose(LPCTSTR lpszFormat, ...)
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
	TRACE(szBuffer);

	va_end(args);
}

void ipfpackDlg::ClearVerbose()
{
	m_verbosemes.SetFocus();
	m_verbosemes.SetSel(0, -1);
	m_verbosemes.Clear();
}


// 詳細設定ボタン
void ipfpackDlg::OnBnClickedIpfpackDetailBtn()
{
	ipfdetailDlg dlg;

	dlg.m_baserevision = m_base_rev;
	dlg.m_revision = m_revision ;
	dlg.m_complv = m_comp_level;

	if(dlg.DoModal() != IDOK) return;

	m_base_rev = dlg.m_baserevision;
	m_revision = dlg.m_revision;
	m_comp_level = dlg.m_complv;

}

int ipfpackDlg::createFilelist(CString dirname,ipf_table &ipffiles,const CString &arcname,CString prefname)
{
	CString findname = dirname + "\\*.*";
	CFileFind arcfinder;

	BOOL b_arkfind = arcfinder.FindFile(findname);
	if(b_arkfind == FALSE){
		return -1;
	}

	while(b_arkfind){
		b_arkfind = arcfinder.FindNextFile();
		if(arcfinder.IsDots())	continue;

		CString path = arcfinder.GetFilePath();

		if(arcfinder.IsDirectory()){
			// ディレクトリだった
			CString newpref = prefname + arcfinder.GetFileName() + "/";
			if(createFilelist(path,ipffiles,arcname,newpref) != 0){
				return -1;
			}
		}else{
			// ファイルだった
			ipf_element element;
			CString fname = prefname + arcfinder.GetFileName();

			element.setFullPath((LPCTSTR) path);
			element.setArchiveName((LPCTSTR) arcname);
			element.setFileName((LPCTSTR) fname);
			element.setUnCompressLength((uint32_t) arcfinder.GetLength());

			ipffiles.push_back(element);
		}
	}
	return 0;
}

// IPFファイルリストは大文字に変換した上で比較する必要があるため
bool compare_fname_nocase_upper(const ipf_element& left,const ipf_element& right)
{
	string upl,upr;
	upl.resize(left.getFullPath().size());
	std::transform(
		left.getFullPath().begin(),
		left.getFullPath().end(),
		upl.begin(),
		::toupper
	);
	upr.resize(right.getFullPath().size());
	std::transform(
		right.getFullPath().begin(),
		right.getFullPath().end(),
		upr.begin(),
		::toupper
	);
	return upl < upr;
}

// 圧縮ボタン
void ipfpackDlg::OnBnClickedIpfpackCompBtn()
{
	ClearVerbose();

	if(m_ipffname.IsEmpty()){
		AfxMessageBox("IPFファイル名を指定してください");
		return;
	}
	if(m_ipfdirname.IsEmpty()){
		AfxMessageBox("圧縮元フォルダを指定してください");
		return;
	}

	// アーカイブリストを作成
	CString findname = m_ipfdirname + "\\*.*";
	CFileFind arcfinder;
	ipf_table ipffiles;
	int status;
	int i;

	BOOL b_arkfind = arcfinder.FindFile(findname);

	PutVerbose("フォルダにあるファイルのリストを作成しています... \r\n");

	if(b_arkfind == FALSE){
		PutVerbose("ERROR: 圧縮元フォルダを開くのに失敗 \r\n");
		return;
	}

	while(b_arkfind){
		b_arkfind = arcfinder.FindNextFile();
		if(arcfinder.IsDots())	continue;	// .の時はそのまま

		if(! arcfinder.IsDirectory()) continue;
		// ディレクトリだった
		createFilelist(arcfinder.GetFilePath(),
			ipffiles,
			arcfinder.GetFileName(),
			""
		);
	}

	// 昇順ソート(大文字小文字区別なし＆大文字ベース)
	std::sort(ipffiles.begin(),ipffiles.end(),compare_fname_nocase_upper);
	PutVerbose("ファイル総数 %lu \r\n",ipffiles.size());

	ofstream fout;
	fout.open((LPCTSTR)m_ipffname,ios::out | ios::binary);

	if(! fout.is_open()){
		PutVerbose("ERROR: ipfファイルの作成に失敗\r\n");
		return;
	}
	for(i=0;i<ipffiles.size();i++){
		ifstream fin;
		ipf_data rdata;

		PutVerbose("ファイル圧縮中... %s/%s \r\n",ipffiles[i].getArchiveName().c_str(),ipffiles[i].getFileName().c_str());

		fin.open(ipffiles[i].getFullPath().c_str(), ios::in | ios::binary);
		if(! fin.is_open()){
			PutVerbose("ERROR: 圧縮対象ファイルを開くのに失敗　%s \r\n",ipffiles[i].getFullPath().c_str());
			return;
		}
		rdata.resize(ipffiles[i].getUnCompressLength());
		fin.read((char *) &rdata[0],ipffiles[i].getUnCompressLength());
		if(!fin){
			PutVerbose("ERROR: 圧縮対象ファイルの読み込みに失敗　%s \r\n",ipffiles[i].getFullPath().c_str());
			return;
		}
		fin.close();

		status = libipf_compress(fout,ipffiles[i],rdata,m_comp_level);
		if(status != IPF_OK){
			PutVerbose("ERROR: 圧縮対象ファイルの圧縮に失敗　%d \r\n",status);
			return;
		}
	}
	status = libipf_write_header_info(
		fout,
		ipffiles,
		m_base_rev,
		m_revision
	);
	fout.close();
	if(status != IPF_OK){
		PutVerbose("ERROR: IPFファイルのヘッダ書き込みに失敗　%d \r\n",status);
		return;
	}
	PutVerbose("IPFファイルを作成しました\r\n");

#if 0
	PutVerbose("IPF ファイルテーブル ---------------------------------\r\n");
	PutVerbose(" Fnl  Anl      CRC         Comp       Uncomp   offset arcname filename\r\n");
	for(i=0;i<ipffiles.size();i++){
		PutVerbose(_T("%4d %4d %08X %12d %12d %08X %s %s\r\n")
			,ipffiles[i].getFilenameLength()
			,ipffiles[i].getArcnameLength()
			,ipffiles[i].getCRC()
			,ipffiles[i].getCompressLength()
			,ipffiles[i].getUnCompressLength()
			,ipffiles[i].getDataOffset()
			,ipffiles[i].getArchiveName().c_str()
			,ipffiles[i].getFileName().c_str()
		);
	}
	PutVerbose("---------------------------------\n");
#endif

}

