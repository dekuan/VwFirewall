//////////////////////////////////////////////////////////////////////////
//
// Intro:
//	------------------------------------------------------------
//	This is a cool class, it can help you to create TrayIcon & balloon
//
// History:
//	------------------------------------------------------------
//	2004-09-25	- Create By LiuQixing
//
//
//
//
//////////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "XingBalloon.h"



//////////////////////////////////////////////////////////////////////////
CXingBalloon* CXingBalloon::m_pThis = NULL;



//////////////////////////////////////////////////////////////////////////
CXingBalloon::CXingBalloon()
{
	Initialise();
}

CXingBalloon::~CXingBalloon()
{
	RemoveTrayIcon();
	if ( m_hWnd )
	{
		DestroyWindow( m_hWnd );
	}
}


VOID CXingBalloon::Initialise()
{
	m_pThis			= this;

	m_bEnabled		= FALSE;
	m_bRemoved		= FALSE;
	m_bHidden		= FALSE;
	m_bShowIconPending	= FALSE;

	_sntprintf( m_szTrayiconClassName, sizeof(m_szTrayiconClassName)-sizeof(TCHAR), _T("%s"), TRAYICON_CLASS );
}


ATOM CXingBalloon::RegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style		= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)BalloonProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
//	wcex.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON_BEAR_GREEN) );
	wcex.hCursor		= LoadIcon( hInstance, MAKEINTRESOURCE(IDC_ARROW) );
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= m_szTrayiconClassName;	//TRAYICON_CLASS;
	wcex.hIconSm		= 0;

	return RegisterClassEx(&wcex);
}

/**
 *	设置 TrayIcon 类名称
 */
VOID CXingBalloon::SetTrayiconClassName( LPCTSTR lpcszClassName )
{
	if ( lpcszClassName && _tcslen( lpcszClassName ) )
	{
		_sntprintf( m_szTrayiconClassName, sizeof(m_szTrayiconClassName)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s"), lpcszClassName );
	}
}

BOOL CXingBalloon::CreateTrayIcon( HINSTANCE hInst, HWND hParentWnd,
		UINT uCallbackMessage, LPCTSTR pszToolTip, HICON hIcon, UINT uMenuID, BOOL bHidden )
{
	if ( NULL == hInst || NULL == hParentWnd || NULL == pszToolTip || NULL == hIcon )
	{
		_tcscpy( m_szError, _T("输入参数有问题") );
		return FALSE;
	}
	if ( uCallbackMessage < WM_APP )
	{
		_sntprintf( m_szError, sizeof(m_szError)-sizeof(TCHAR),
			_T("CallbackMessage 已经大于 WM_APP(0X%X) 的标准"), WM_APP );
		return FALSE;
	}

	BOOL bRet	= FALSE;

	m_bEnabled	= TRUE;
	m_hInstance	= hInst;
	RegisterClass( hInst );

	// Create an invisible window
	m_hWnd = ::CreateWindow
		(
			m_szTrayiconClassName,
			m_szTrayiconClassName,	//_T(""),
			WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, 0,
			hInst, 0
		);

	// load up the NOTIFYICONDATA structure
	m_tnd.cbSize		= sizeof(NOTIFYICONDATA);
	m_tnd.hWnd		= hParentWnd;
	m_tnd.uID		= uMenuID;
	m_tnd.hIcon		= hIcon;
	m_tnd.uFlags		= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage	= uCallbackMessage;
	_sntprintf( m_tnd.szTip, sizeof(m_tnd.szTip)-sizeof(TCHAR), _T("%s"), pszToolTip );

	m_bHidden		= bHidden;
	m_hWnd			= hParentWnd;
	m_uCreationFlags	= m_tnd.uFlags;

	if ( FALSE == m_bHidden )
	{
		bRet			= Shell_NotifyIcon( NIM_ADD, &m_tnd );
		m_bShowIconPending	= m_bHidden = m_bRemoved = !bRet;
	}

	return bRet;
}




BOOL CXingBalloon::AddTrayIcon()
{
	if ( FALSE == m_bRemoved )
	{
		RemoveTrayIcon();
	}

	if ( m_bEnabled )
	{
		m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		if ( !Shell_NotifyIcon(NIM_ADD, &m_tnd) )
			m_bShowIconPending = TRUE;
		else
			m_bRemoved = m_bHidden = FALSE;
	}
	return ( FALSE == m_bRemoved );
}

BOOL CXingBalloon::RemoveTrayIcon()
{
	m_tnd.uFlags	= 0;
	if ( Shell_NotifyIcon(NIM_DELETE, &m_tnd) )
	{
		m_bRemoved = TRUE;
	}
	return ( TRUE == m_bRemoved );
}

BOOL CXingBalloon::ModifyTrayIcon( HICON hIcon, TCHAR * pszTip )
{
	if ( FALSE == m_bEnabled )
		return FALSE;


	if ( pszTip )
	{
		// 要求修改 Tips
		m_tnd.uFlags	= NIF_ICON | NIF_TIP;
		m_tnd.hIcon	= hIcon;
		_sntprintf( m_tnd.szTip, sizeof(m_tnd.szTip)-sizeof(TCHAR), _T("%s"), pszTip );
	}
	else
	{
		// 仅仅修改 Icon
		m_tnd.uFlags	= NIF_ICON;
		m_tnd.hIcon	= hIcon;
	}

	if ( m_bHidden )
		return TRUE;
	else
		return Shell_NotifyIcon( NIM_MODIFY, &m_tnd );
}

BOOL CXingBalloon::HideTrayIcon()
{
	if ( !m_bEnabled || m_bRemoved || m_bHidden )
		return TRUE;

	m_tnd.uFlags		= NIF_STATE;
	m_tnd.dwState		= NIS_HIDDEN;
	m_tnd.dwStateMask	= NIS_HIDDEN;

        m_bHidden = Shell_NotifyIcon( NIM_MODIFY, &m_tnd );
	RemoveTrayIcon();

	return ( TRUE == m_bHidden );
}

BOOL CXingBalloon::ShowTrayIcon()
{
	if ( m_bRemoved )
		return AddTrayIcon();
	
	if ( !m_bHidden )
		return TRUE;

	m_tnd.uFlags		= NIF_STATE;
	m_tnd.dwState		= 0;
	m_tnd.dwStateMask	= NIS_HIDDEN;
	Shell_NotifyIcon ( NIM_MODIFY, &m_tnd );
	AddTrayIcon();

	return ( FALSE == m_bHidden );
}

BOOL CXingBalloon::ShowBalloon( LPCTSTR pszText, LPCTSTR pszTitle, DWORD dwIcon, UINT uTimeout )
{
	// dwBalloonIcon must be valid.
	if ( NIIF_NONE != dwIcon && NIIF_INFO != dwIcon &&
		NIIF_WARNING != dwIcon && NIIF_ERROR != dwIcon )
	{
		_tcscpy( m_szError, _T("参数不符合气球的规格") );
		return FALSE;
	}
	if ( uTimeout < 10*1000 || uTimeout > 30*1000 )
	{
		_tcscpy( m_szError, _T("气球浮动时间最长 30 秒，最短 10 秒，请输入正确的值") );
		return FALSE;
	}

	BOOL bRet		= FALSE;

	m_tnd.uFlags		= NIF_INFO;
	m_tnd.dwInfoFlags	= dwIcon;
	m_tnd.uTimeout		= uTimeout;   // convert time to ms
	_sntprintf( m_tnd.szInfo, sizeof(m_tnd.szInfo)-sizeof(TCHAR), _T("%s"), pszText );
	if ( pszTitle )
		_sntprintf( m_tnd.szInfoTitle, sizeof(m_tnd.szInfoTitle)-sizeof(TCHAR), _T("%s"), pszTitle );
	else
		memset( m_tnd.szInfoTitle, 0, sizeof(m_tnd.szInfoTitle) );

	bRet = Shell_NotifyIcon( NIM_MODIFY, &m_tnd );

	// Clean the balloon text string, so it won't be redisplaied;
	memset( m_tnd.szInfo, 0, sizeof(m_tnd.szInfo) );
	
	return bRet;
}



UINT CXingBalloon::GetCallbackMessage() const
{
	return m_tnd.uCallbackMessage;
}

HWND CXingBalloon::GetSafeHwnd() const
{
	return (this) ? m_hWnd : NULL;
}

LRESULT __stdcall CXingBalloon::BalloonProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CXingBalloon * pTrayIcon = m_pThis;
	if ( pTrayIcon->GetSafeHwnd() != hWnd )
		return ::DefWindowProc( hWnd, message, wParam, lParam );


	return ::DefWindowProc( hWnd, message, wParam, lParam );
}



