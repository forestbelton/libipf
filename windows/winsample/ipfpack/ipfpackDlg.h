// ipfpackDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// ipfpackDlg ダイアログ
class ipfpackDlg : public CDialog
{
// コンストラクション
public:
	ipfpackDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_IPFPACK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	uint32_t m_base_rev;
	uint32_t m_revision;
	int m_comp_level;

	void PutVerbose(LPCTSTR lpszFormat, ...);
	void ClearVerbose();

	int createFilelist(CString dirname,ipf_table &ipffiles,const CString &arcname,CString prefname);

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedIpfpackDetailBtn();
	afx_msg void OnBnClickedIpfpackCompBtn();
	CString m_ipfdirname;
	CString m_ipffname;
	afx_msg void OnBnClickedIpfpackDirBtn();
	afx_msg void OnBnClickedIpfpackFileBtn();
	CEdit m_verbosemes;
};
