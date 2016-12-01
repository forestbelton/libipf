// ipfpack.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// ipfpackApp:
// このクラスの実装については、ipfpack.cpp を参照してください。
//

class ipfpackApp : public CWinApp
{
public:
	ipfpackApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern ipfpackApp theApp;