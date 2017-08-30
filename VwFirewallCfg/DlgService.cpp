// DlgService.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgService dialog


CDlgService::CDlgService(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgService::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgService)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_lpcszSvcName	= NULL;
	m_bSvcStart	= FALSE;
}
CDlgService::~CDlgService()
{
	m_cThSleepOpService.EndSleep();
	m_cThSleepOpService.EndThread();
}


void CDlgService::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgService)
	DDX_Control(pDX, IDC_STATIC_HINT, m_stcHint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgService, CDialog)
	//{{AFX_MSG_MAP(CDlgService)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgService message handlers

BOOL CDlgService::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	MakeAlphaDlg( m_hWnd, 90 );

	//
	//	开启修复线程
	//
	m_cThSleepOpService.m_hThread = (HANDLE)_beginthreadex
		(
			NULL,
			0,
			&_threadOpService,
			(void*)this,
			0,
			&m_cThSleepOpService.m_uThreadId
		);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


unsigned __stdcall CDlgService::_threadOpService( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CDlgService * pThis = (CDlgService*)arglist;
		if ( pThis )
		{
			pThis->OpServiceProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CDlgService::OpServiceProc()
{
	TCHAR szHint[ MAX_PATH ];

	if ( m_lpcszSvcName )
	{
		if ( m_bSvcStart )
		{
			//	启动服务
			_sntprintf( szHint, sizeof(szHint)/sizeof(TCHAR)-1, _T("正在启动服务 %s，请稍候..."), m_lpcszSvcName );
			m_stcHint.SetWindowText( szHint );

			if ( delib_drv_is_exist( m_lpcszSvcName ) )
			{
				delib_drv_start( m_lpcszSvcName );
			}
		}
		else
		{
			//	停止服务
			_sntprintf( szHint, sizeof(szHint)/sizeof(TCHAR)-1, _T("正在停止服务 %s，请稍候..."), m_lpcszSvcName );
			m_stcHint.SetWindowText( szHint );

			if ( delib_drv_is_exist( m_lpcszSvcName ) )
			{
				delib_drv_stop( m_lpcszSvcName );
			}
		}
	}

	//	...
	SetTimer( 1000, 3000, NULL );
}

void CDlgService::OnTimer( UINT nIDEvent ) 
{
	if ( 1000 == nIDEvent )
	{
		KillTimer( 1000 );
		OnOK();
	}	
	CDialog::OnTimer(nIDEvent);
}



/**
*	@ public
*	设置窗口透明度
*/
BOOL CDlgService::MakeAlphaDlg( HWND hWnd, INT nAlphaPercent )
{
	if ( NULL == hWnd )
	{
		return FALSE;
	}
	
	typedef BOOL (FAR PASCAL * FUNC1)
		(
			HWND hwnd,           // handle to the layered window
			COLORREF crKey,      // specifies the color key
			BYTE bAlpha,         // value for the blend function
			DWORD dwFlags        // action
		);
	
	//	第二步 实现代码
	HMODULE hModule = GetModuleHandle( _T("user32.dll") );
	
	if ( hModule )
	{
		FUNC1 SetLayeredWindowAttributes = (FUNC1)GetProcAddress( hModule, ("SetLayeredWindowAttributes") );
		
		if ( SetLayeredWindowAttributes )
		{
			// 设置分层扩展标记
			SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | 0x80000L );
			// 70% alpha
			SetLayeredWindowAttributes( hWnd, 0, ( 255 * nAlphaPercent ) / 100, 0x2 );
			
			// 第三步：如何除去透明选项？
			// 除去分层扩展标记
			//	SetWindowLong(hWnd,, GWL_EXSTYLE,
			//	GetWindowLong(hWnd, GWL_EXSTYLE) & ~ 0x80000L);
			// 重画窗口
			//	RedrawWindow();
		}
	}
	
	return TRUE;
}