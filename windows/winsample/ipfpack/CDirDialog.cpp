/*
 *  CDirDialog
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "stdafx.h"
#include "CDirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CDirDialog::CDirDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFilename, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParent)
#if _MSC_VER < 1400
	: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFilename, dwFlags, lpszFilter, pParent)
#else
	: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFilename, dwFlags|OFN_ENABLESIZING, lpszFilter, pParent)
#endif
{
#if _MSC_VER < 1400
#else
	m_ofn.FlagsEx |= OFN_EX_NOPLACESBAR;
#endif

	m_bSelectableFolder = false;
	m_strOldFolder.Empty();
}

BEGIN_MESSAGE_MAP(CDirDialog, CFileDialog)
	//{{AFX_MSG_MAP(CDirDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH CDirDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return CFileDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CDirDialog::OnEraseBkgnd(CDC* pDC) 
{
	BOOL ret = CFileDialog::OnEraseBkgnd(pDC);
	return ret;
}


BOOL CDirDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();

	// [?]を消す
	CWnd *Dialog = GetParent();
	if(Dialog){
		Dialog->ModifyStyleEx(WS_EX_CONTEXTHELP, 0);
	}

	// フォルダ選択のみのファイル選択ダイアログの場合
	if(m_bSelectableFolder){
		strcpy_s(m_ofn.lpstrFile, m_ofn.nMaxFile, m_strOldFolder);

		CWnd	*p_parent, *pwnd;

		p_parent = GetParent();
		if (p_parent != NULL){
			// タイトル
			p_parent->SetWindowText("フォルダを開く");

			// ファイル名
			pwnd = p_parent->GetDlgItem(0x442);
			if (pwnd != NULL)	pwnd->ShowWindow(SW_HIDE);

			// ファイル名（コンボ・ボックス）
#if _MSC_VER < 1400
			pwnd = p_parent->GetDlgItem(0x480);
#else
			pwnd = p_parent->GetDlgItem(0x47c);
#endif
			if (pwnd != NULL)	pwnd->ShowWindow(SW_HIDE);

			// ファイルの種類
			pwnd = p_parent->GetDlgItem(0x441);
			if (pwnd != NULL)	pwnd->ShowWindow(SW_HIDE);

			// ファイルの種類（コンボ・ボックス）
			pwnd = p_parent->GetDlgItem(0x470);
			if (pwnd != NULL)	pwnd->ShowWindow(SW_HIDE);
		}
	}

	return TRUE;
}

// フォルダが変更されるたびに発生するイベント
void CDirDialog::OnFolderChange()
{
	if(m_bSelectableFolder){
		const CString strFolder = GetFolderPath();
		if (strFolder != m_strOldFolder) {
			m_strOldFolder = strFolder;
			return;
		}

		CPoint ptCursor;
		VERIFY(::GetCursorPos(&ptCursor));

		CRect rcOK;
		CWnd *const pWndOK = GetParent()->GetDlgItem(IDOK);
		pWndOK->GetWindowRect(rcOK);

		if (!::PtInRect(rcOK, ptCursor)) {
			return;
		}

		const int iResult = strcpy_s(m_ofn.lpstrFile, m_ofn.nMaxFile, strFolder);
		DBG_UNREFERENCED_LOCAL_VARIABLE(iResult);
		ASSERT(iResult == 0);

		if (OnFileNameOK() != FALSE) {
			return;
		}

		CDialog *const pParent = (CDialog *)GetParent();
		if (pParent != NULL) {
			pParent->EndDialog(IDOK);
		}
	}
	__super::OnFolderChange();
}
