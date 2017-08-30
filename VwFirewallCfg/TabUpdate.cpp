// TabUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabUpdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabUpdate dialog


CTabUpdate::CTabUpdate(CWnd* pParent /*=NULL*/)
	: CDialog(CTabUpdate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabUpdate)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTabUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabUpdate)
	DDX_Control(pDX, IDC_STATIC_CURRVER, m_stcCurrVer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabUpdate, CDialog)
	//{{AFX_MSG_MAP(CTabUpdate)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_CHECKFORUPDATE, OnButtonCheckforupdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabUpdate message handlers

BOOL CTabUpdate::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString strText = "";
	CString strTemp	= "";

	m_stcCurrVer.GetWindowText( strText );
	if ( strText.GetLength() )
	{
		if ( strlen( CVwFirewallConfigFile::m_szSysDriverVersion ) )
		{
			strTemp.Format( strText, CVwFirewallConfigFile::m_szSysDriverVersion );
		}
		else
		{
			strTemp.Format( strText, "unknown" );
		}
		m_stcCurrVer.SetWindowText( strTemp );
	}

	//
	//	链接
	//
	m_LinkUpdate.SubclassDlgItem( IDC_STATIC_DOWNLOAD, this );
	m_LinkUpdate.SetColours( RGB(0x00,0x00,0xFF), RGB(0x00,0x00,0xFF), RGB(0x00,0x00,0xFF) );
	m_LinkUpdate.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkUpdate.SetUnderline( TRUE );
	m_LinkUpdate.SetVisited( FALSE );
	m_LinkUpdate.SetURL( "http://rd.vidun.net/?id=4502" );

	return TRUE;
}

BOOL CTabUpdate::PreTranslateMessage(MSG* pMsg) 
{
	//	拦截用户按下的 ESC 键，强迫用户点 X 退出
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam )
		{
			return FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CTabUpdate::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//
	//	处理任何位置的拖拽效果
	//
	HWND hParent = ::GetParent( m_hWnd );
	if ( hParent )
	{
		::PostMessage( hParent, WM_NCLBUTTONDOWN, HTCAPTION,MAKELPARAM( point.x, point.y ) );
	}
	
	//	向系统发送 HTCAPTION 消息，让系统以为鼠标点在标题栏上
	CDialog::OnLButtonDown(nFlags, point);
}

void CTabUpdate::OnButtonCheckforupdate() 
{
	WinExec( "VwUpdate.exe", SW_SHOW );
}


