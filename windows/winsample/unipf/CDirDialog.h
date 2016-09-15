/*
 *  CDirDialog
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once


class CDirDialog : public CFileDialog
{
//	DECLARE_DYNCREATE(CDirDialog)
public:
	CDirDialog(BOOL bOpenFileDialog, 
		LPCTSTR lpszDefExt = NULL, 
		LPCTSTR lpszFilename = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParent = NULL);

	bool	m_bSelectableFolder;
	CString	m_strOldFolder;

protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CDirDialog)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	virtual void OnFolderChange() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
