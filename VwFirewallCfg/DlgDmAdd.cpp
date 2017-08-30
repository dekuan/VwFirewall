// DlgDmAdd.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgDmAdd.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDmAdd dialog


CDlgDmAdd::CDlgDmAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDmAdd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDmAdd)
	//}}AFX_DATA_INIT

	m_bUse	= FALSE;
	memset( m_szPanDomain, 0, sizeof(m_szPanDomain) );
	memset( m_szToDomain, 0, sizeof(m_szToDomain) );
}


void CDlgDmAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDmAdd)
	DDX_Control(pDX, IDC_CHECK_USE, m_chkUse);
	DDX_Control(pDX, IDC_EDIT_PANDOMAIN, m_editPanDomain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDmAdd, CDialog)
	//{{AFX_MSG_MAP(CDlgDmAdd)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_MESSAGE( UM_OPENHTMLHELP, OnOpenHtmlHelp )
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDmAdd message handlers

BOOL CDlgDmAdd::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_chkUse.SetCheck( m_bUse );
	m_editPanDomain.SetWindowText( m_szPanDomain );

	SetTimer( 1000, 1, NULL );

	return TRUE;
}

LRESULT CDlgDmAdd::OnOpenHtmlHelp( WPARAM wParam, LPARAM lParam )
{
	CProcFunc::OpenChmHelpPage( wParam );
	return S_OK;
}

void CDlgDmAdd::OnButtonSave() 
{
	CString strCap;
	CString strTemp;

	m_bUse	= m_chkUse.GetCheck();
	
	memset( m_szPanDomain, 0, sizeof(m_szPanDomain) );
	m_editPanDomain.GetWindowText( m_szPanDomain, sizeof(m_szPanDomain) );
	StrTrim( m_szPanDomain, _T(" \r\n\t") );

	if ( IsValidPanDomain( m_szPanDomain ) || IsValidDomain( m_szPanDomain ) )
	{
		//	ªÒ»° m_szToDomain
		_tcslwr( m_szPanDomain );
	}
	else
	{
		strCap.Format( IDS_MSG_CAPTION_WARNING );
		strTemp.Format( IDS_DLGDMADD_ERR_DOMAIN );
		MessageBox( strTemp, strCap, MB_ICONWARNING );
		m_editPanDomain.SetFocus();
		return;
	}

	OnOK();
}


void CDlgDmAdd::OnTimer(UINT nIDEvent) 
{
	if ( 1000 == nIDEvent )
	{
		KillTimer( 1000 );
		m_editPanDomain.SetFocus();
	}

	CDialog::OnTimer(nIDEvent);
}
