// DlgExpire.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgExpire.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExpire dialog


CDlgExpire::CDlgExpire(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExpire::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExpire)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgExpire::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExpire)
	DDX_Control(pDX, IDC_STATIC_INFO, m_stcInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExpire, CDialog)
	//{{AFX_MSG_MAP(CDlgExpire)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExpire message handlers

BOOL CDlgExpire::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	How to Renew ?
	m_LinkHowToRenew.SubclassDlgItem( IDC_STATIC_HOWTORENEW, this );
	m_LinkHowToRenew.SetColours( RGB(0x00,0x00,0xFF), RGB(0x80,0x00,0x80), RGB(0x00,0x00,0xFF) );
	m_LinkHowToRenew.SetURL( "http://rd.vidun.net/?id=4501" );
	
	//
	//	谁知信息
	//
	CString strFmt;
	CString strTemp;
	
	m_stcInfo.GetWindowText( strFmt );
	strTemp.Format( strFmt, m_szRegExpireDate );
	m_stcInfo.SetWindowText( strTemp );
	
	
	//
	//	铛的一声响
	//
	MessageBeep( MB_ICONEXCLAMATION );
	
	
	//
	//	设置到最高层窗口
	//
	SetTopWindow();
	//::SetWindowPos( m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE );
	
	
	return TRUE;
}


/**
*	设置窗口到最顶层
*/
BOOL CDlgExpire::SetTopWindow()
{
	// Set Top Window
	HWND hDeskTopWin;
	RECT rectFrame, rectThisWin;
	INT nWndX, nWndY;
	
	hDeskTopWin = ::GetDesktopWindow();
	if ( hDeskTopWin )
	{
		if ( ::GetWindowRect( hDeskTopWin, &rectFrame ) &&
			::GetWindowRect( m_hWnd, &rectThisWin ) )
		{
			nWndX	= rectFrame.left + ( rectFrame.right - rectFrame.left ) / 2  - ( rectThisWin.right - rectThisWin.left ) / 2;
			nWndY	= rectFrame.top + ( rectFrame.bottom - rectFrame.top ) / 2 - ( rectThisWin.bottom - rectThisWin.top ) / 2;
			
			::SetWindowPos( m_hWnd, HWND_TOPMOST, nWndX, nWndY, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE );
		}
	}
	
	return TRUE;
}
