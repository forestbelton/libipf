// ipfdetailDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ipfpack.h"
#include "ipfdetailDlg.h"


// ipfdetailDlg ダイアログ

IMPLEMENT_DYNAMIC(ipfdetailDlg, CDialog)

ipfdetailDlg::ipfdetailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ipfdetailDlg::IDD, pParent)
	, m_revision(0)
	, m_baserevision(0)
{
	m_complv = 1;

}

ipfdetailDlg::~ipfdetailDlg()
{
}

void ipfdetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPFDETAIL_COMPLV, m_complv_combo);
	DDX_Text(pDX, IDC_IPFDETAIL_REV, m_revision);
	DDX_Text(pDX, IDC_IPFDETAIL_SUBREV, m_baserevision);
}


BEGIN_MESSAGE_MAP(ipfdetailDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_IPFDETAIL_COMPLV, &ipfdetailDlg::OnCbnSelchangeIpfdetailComplv)
	ON_BN_CLICKED(IDOK, &ipfdetailDlg::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL ipfdetailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_complv_combo.SetCurSel(m_complv);

	return TRUE;
}

// ipfdetailDlg メッセージ ハンドラ

void ipfdetailDlg::OnCbnSelchangeIpfdetailComplv()
{
}

void ipfdetailDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_complv = m_complv_combo.GetCurSel();
	UpdateData(FALSE);
	OnOK();
}
