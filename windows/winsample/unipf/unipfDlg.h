/*
 *  unipfDlg
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once
#include "libipf.hpp"
#include "afxwin.h"


class unipfDlg : public CDialog
{
// コンストラクション
public:
	unipfDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_UNIPF_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;
	ipf_file	m_Ipffile;
	ipf_table	m_IpfInfo;

	bool OpenIPFFile(std::ifstream &fin);
	void PutVerbose(LPCTSTR lpszFormat, ...);
	void ClearVerbose();


	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUnipfFileBtn();
	CString m_ipffname;
	CString m_ipfdirname;
	afx_msg void OnBnClickedUnipfDirBtn();
	afx_msg void OnBnClickedUnipfInfoBtn();
	afx_msg void OnBnClickedUnipfUncompBtn();
	CEdit m_verbosemes;
};
