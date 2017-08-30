// DlgSplash.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgSplash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSplash dialog


CDlgSplash::CDlgSplash(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSplash::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSplash)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSplash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSplash)
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_stcImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSplash, CDialog)
	//{{AFX_MSG_MAP(CDlgSplash)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSplash message handlers

BOOL CDlgSplash::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//
	//	ÔÚTaskBarÒþ²Ø
	//
	ModifyStyleEx( WS_EX_APPWINDOW, WS_EX_TOOLWINDOW );


	CRect rect;
	CRect rectImage;
	//CRect rectImageNew;

	GetClientRect( &rect );
	::GetClientRect( m_stcImage.m_hWnd, &rectImage );
	//rectImageNew.left	= 0;
	//rectImageNew.top	= 0;
	//rectImageNew.right	= rect.right;
	//rectImageNew.bottom	= ( ( rectImage.Height() * rect.Width() ) / rectImage.Width() );
	::SetWindowPos( m_hWnd, NULL, 0, 0, rectImage.Width(), rectImage.Height(), SWP_NOMOVE );
	CenterWindow();


	SetTimer( 1000, 3000, NULL );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSplash::OnTimer(UINT nIDEvent) 
{
	if ( 1000 == nIDEvent )
	{
		KillTimer( 1000 );
		OnOK();
	}	
	CDialog::OnTimer(nIDEvent);
}
