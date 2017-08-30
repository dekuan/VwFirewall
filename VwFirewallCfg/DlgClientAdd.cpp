// DlgClientAdd.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgClientAdd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgClientAdd dialog


CDlgClientAdd::CDlgClientAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgClientAdd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgClientAdd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	memset( m_szClientName, 0, sizeof(m_szClientName) );
}


void CDlgClientAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgClientAdd)
	DDX_Control(pDX, IDC_EDIT_CLIENTNAME, m_editClientName);
	DDX_Control(pDX, IDC_CHECK_USE, m_chkUse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgClientAdd, CDialog)
	//{{AFX_MSG_MAP(CDlgClientAdd)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgClientAdd message handlers

BOOL CDlgClientAdd::OnInitDialog() 
{
	CDialog::OnInitDialog();


	m_chkUse.SetCheck( m_bUse );
	m_editClientName.SetWindowText( m_szClientName );

	
	SetTimer( 1000, 1, NULL );

	return TRUE;
}

void CDlgClientAdd::OnButtonSave() 
{
	CString strCap;
	CString strTemp;

	m_bUse	= m_chkUse.GetCheck();
	
	memset( m_szClientName, 0, sizeof(m_szClientName) );
	m_editClientName.GetWindowText( m_szClientName, sizeof(m_szClientName) );
	StrTrim( m_szClientName, _T(" \r\n\t") );

	if ( IsValidComputerName( m_szClientName ) )
	{
		_tcslwr( m_szClientName );
	}
	else
	{
		strCap.Format( IDS_MSG_CAPTION_WARNING );
		strTemp.Format( IDS_DLGCLIENTADD_ERR_PCNAME );
		MessageBox( strTemp, strCap, MB_ICONWARNING );
		m_editClientName.SetFocus();
		return;
	}

	OnOK();
	
}

void CDlgClientAdd::OnTimer(UINT nIDEvent) 
{
	if ( 1000 == nIDEvent )
	{
		KillTimer( 1000 );
		m_editClientName.SetFocus();
	}
	
	CDialog::OnTimer(nIDEvent);
}
