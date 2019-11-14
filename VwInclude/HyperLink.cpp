#include "stdafx.h"
#include "HyperLink.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TOOLTIP_ID 1



/////////////////////////////////////////////////////////////////////////////
// CHyperLink

CHyperLink::CHyperLink()
{
	m_hMainWnd		= NULL;
	m_uCallBackMessage	= 0;
	m_wCallBackParam	= 0;
	m_lCallBackParam	= 0;

	m_bStaticOnly		= FALSE;		//	仅仅是一个静态的 static 控件
	m_bVisited		= FALSE;
	m_hLinkCursor		= NULL;                 // No cursor as yet
	m_crLinkColour		= RGB(  0,   0, 238);   // Blue
	m_crVisitedColour	= RGB( 85,  26, 139);   // Purple
	m_crHoverColour		= ::GetSysColor( COLOR_HIGHLIGHT );
	m_bOverControl		= FALSE;                // Cursor not yet over control
	m_bVisited		= FALSE;                // Hasn't been visited yet.
	m_bUnderline		= TRUE;                 // Underline the link?
	m_bAdjustToFit		= TRUE;                 // Resize the window to fit the text?
	m_strURL.Empty();

	memset( m_szHint, 0, sizeof(m_szHint) );
}

CHyperLink::~CHyperLink()
{
	m_Font.DeleteObject();

	if ( m_hLinkCursor )
	{
		DestroyCursor( m_hLinkCursor );
		m_hLinkCursor = NULL;
	}
}

BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
//{{AFX_MSG_MAP(CHyperLink)
ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
ON_WM_CTLCOLOR_REFLECT()
ON_WM_SETCURSOR()
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLink message handlers

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}

/**
 *	强制执行点击动作
 */
VOID CHyperLink::ExecuteClick()
{
	OnClicked();
}

void CHyperLink::OnClicked()
{
	if ( m_bStaticOnly )
	{
		return;
	}

	if ( m_hMainWnd && m_uCallBackMessage > 0 )
	{
		m_bVisited = ::PostMessage( m_hMainWnd, m_uCallBackMessage, m_wCallBackParam, m_lCallBackParam );
	}
	else
	{
		m_bVisited = LauchIEBrowser( m_strURL );	
	}
}

HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	ASSERT(nCtlColor == CTLCOLOR_STATIC);

	if ( pDC )
	{
		if ( m_bOverControl )
			pDC->SetTextColor(m_crHoverColour);
		else if ( m_bVisited )
			pDC->SetTextColor(m_crVisitedColour);
		else
			pDC->SetTextColor(m_crLinkColour);

		// transparent text.
		pDC->SetBkMode(TRANSPARENT);
	}
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void CHyperLink::OnMouseMove(UINT nFlags, CPoint point)
{
	CStatic::OnMouseMove( nFlags, point );

	if ( m_bOverControl )        // Cursor is currently over control
	{
		CRect rect;
		GetClientRect(rect);

		if (!rect.PtInRect(point) )
		{
			m_bOverControl = FALSE;
			ReleaseCapture();
			RedrawWindow();
			return;
		}
	}
	else                      // Cursor has just moved over control
	{
		m_bOverControl = TRUE;
		RedrawWindow();
		SetCapture();
	}
}

BOOL CHyperLink::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	if (m_hLinkCursor)
	{
		::SetCursor(m_hLinkCursor);
		return TRUE;
	}
	return FALSE;
}

void CHyperLink::PreSubclassWindow() 
{
	// We want to get mouse clicks via STN_CLICKED
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	// Set the URL as the window text
	if ( m_strURL.IsEmpty() )
	{
		GetWindowText( m_strURL );
	}

	// Check that the window text isn't empty. If it is, set it as the URL.
	CString strWndText;
	GetWindowText(strWndText);
	if (strWndText.IsEmpty())
	{
		ASSERT(!m_strURL.IsEmpty());    // Window and URL both NULL. DUH!
		SetWindowText(m_strURL);
	}

	// Create the font
	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	lf.lfUnderline = m_bUnderline;
	m_Font.CreateFontIndirect(&lf);
	SetFont(&m_Font);

	PositionWindow();        // Adjust size of window to fit URL if necessary
	SetDefaultCursor();      // Try and load up a "hand" cursor

	// Create the tooltip
	CRect rect;
	GetClientRect(rect);
	m_ToolTip.Create(this);
	if ( 0 == _tcslen( m_szHint ) )
	{
		m_ToolTip.AddTool( this, m_strURL, rect, TOOLTIP_ID );
	}
	else
	{
		m_ToolTip.AddTool( this, m_szHint, rect, TOOLTIP_ID );
	}

	CStatic::PreSubclassWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink operations
VOID CHyperLink::SetHint( TCHAR * pszHint )
{
	if ( NULL != pszHint && _tcslen(pszHint) > 0 )
	{
		_sntprintf( m_szHint, sizeof(m_szHint)/sizeof(TCHAR)-1, _T("%s"), pszHint );
		UpdateLinkTipText();
	}
}

/**
 *	设置回调消息
 */
BOOL CHyperLink::SetCallbackMessage( HWND hMainWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	if ( hMainWnd && uMessage )
	{
		m_hMainWnd		= hMainWnd;
		m_uCallBackMessage	= uMessage;
		m_wCallBackParam	= wParam;
		m_lCallBackParam	= lParam;

		return TRUE;
	}

	return FALSE;
}



VOID CHyperLink::SetURL(CString strURL)
{
	m_strURL = strURL;
	
	if ( ::IsWindow(GetSafeHwnd()) )
	{
		PositionWindow();

		UpdateLinkTipText();
		//m_ToolTip.UpdateTipText(strURL, this, TOOLTIP_ID);
	}
}

CString CHyperLink::GetURL() const
{ 
	return m_strURL;
}

VOID CHyperLink::SetStaticOnly( BOOL bStaticOnly )
{
	m_bStaticOnly = bStaticOnly;
}

void CHyperLink::SetColours(COLORREF crLinkColour, COLORREF crVisitedColour,
                            COLORREF crHoverColour /* = -1 */) 
{ 
	m_crLinkColour    = crLinkColour; 
	m_crVisitedColour = crVisitedColour;

	if (crHoverColour == -1)
		m_crHoverColour = ::GetSysColor(COLOR_HIGHLIGHT);
	else
		m_crHoverColour = crHoverColour;
	
	if (::IsWindow(m_hWnd))
		Invalidate(); 
}

COLORREF CHyperLink::GetLinkColour() const
{ 
	return m_crLinkColour; 
}

COLORREF CHyperLink::GetVisitedColour() const
{
	return m_crVisitedColour;
}

COLORREF CHyperLink::GetHoverColour() const
{
	return m_crHoverColour;
}

void CHyperLink::SetVisited( BOOL bVisited /* = TRUE */ )
{
	m_bVisited = bVisited;
	if ( ::IsWindow(GetSafeHwnd()) )
	{
		Invalidate();
	}
}

BOOL CHyperLink::GetVisited() const
{ 
	return m_bVisited; 
}

void CHyperLink::SetLinkCursor(HCURSOR hCursor)
{ 
	m_hLinkCursor = hCursor;
	if ( NULL == m_hLinkCursor )
	{
		SetDefaultCursor();
	}
}

HCURSOR CHyperLink::GetLinkCursor() const
{
	return m_hLinkCursor;
}


void CHyperLink::SetUnderline(BOOL bUnderline /* = TRUE */)
{
	m_bUnderline = bUnderline;
	
	if (::IsWindow(GetSafeHwnd()))
	{
		LOGFONT lf;
		GetFont()->GetLogFont(&lf);
		lf.lfUnderline = m_bUnderline;

		m_Font.DeleteObject();
		m_Font.CreateFontIndirect(&lf);
		SetFont(&m_Font);

		Invalidate();
	}
}

BOOL CHyperLink::GetUnderline() const
{ 
	return m_bUnderline; 
}


void CHyperLink::SetAutoSize(BOOL bAutoSize /* = TRUE */)
{
	m_bAdjustToFit = bAutoSize;
	
	if (::IsWindow(GetSafeHwnd()))
		PositionWindow();
}

BOOL CHyperLink::GetAutoSize() const
{ 
	return m_bAdjustToFit; 
}


// Move and resize the window so that the window is the same size
// as the hyperlink text. This stops the hyperlink cursor being active
// when it is not directly over the text. If the text is left justified
// then the window is merely shrunk, but if it is centred or right
// justified then the window will have to be moved as well.
//
void CHyperLink::PositionWindow()
{
	if (!::IsWindow(GetSafeHwnd()) || !m_bAdjustToFit) 
		return;
	
	// Get the current window position
	CRect rect;
	GetWindowRect(rect);
	
	CWnd* pParent = GetParent();
	if (pParent)
		pParent->ScreenToClient(rect);
	
	// Get the size of the window text
	CString strWndText;
	GetWindowText(strWndText);
	
	CDC * pDC = GetDC();
	CFont * pOldFont = pDC->SelectObject(&m_Font);
	CSize Extent = pDC->GetTextExtent(strWndText);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	// Get the text justification via the window style
	DWORD dwStyle = GetStyle();
	
	// Recalc the window size and position based on the text justification
	if ( dwStyle & SS_CENTERIMAGE )
		rect.DeflateRect(0, (rect.Height() - Extent.cy)/2);
	else
		rect.bottom = rect.top + Extent.cy;

	if ( dwStyle & SS_CENTER )
		rect.DeflateRect( (rect.Width() - Extent.cx)/2, 0 );
	else if ( dwStyle & SS_RIGHT )
		rect.left  = rect.right - Extent.cx;
	else // SS_LEFT = 0, so we can't test for it explicitly 
		rect.right = rect.left + Extent.cx;

	// Move the window
	SetWindowPos( NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER );

}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink implementation

// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
// It loads a "hand" cursor from the winhlp32.exe module
void CHyperLink::SetDefaultCursor()
{
	CString strWndDir;
	HMODULE hModule;
	HCURSOR hHandCursor;

	if ( m_hLinkCursor == NULL )                // No cursor handle - load our own
	{
		if ( m_bStaticOnly )
		{
			hHandCursor = ::LoadCursor( NULL, MAKEINTRESOURCE( IDC_APPSTARTING ) );
			if ( hHandCursor )
			{
				m_hLinkCursor = CopyCursor( hHandCursor );

				//	free it
				DestroyCursor( hHandCursor );
				hHandCursor = NULL;
			}
		}
		else
		{
			// Get the windows directory
			
			GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
			strWndDir.ReleaseBuffer();
			
			strWndDir += _T("\\winhlp32.exe");
			// This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
			hModule = LoadLibrary(strWndDir);
			if ( hModule )
			{
				hHandCursor = ::LoadCursor( hModule, MAKEINTRESOURCE(106) );
				if ( hHandCursor )
				{
					if ( m_hLinkCursor )
					{
						DestroyCursor( m_hLinkCursor );
						m_hLinkCursor = NULL;
					}
					m_hLinkCursor = CopyCursor( hHandCursor );
					
					//	free it
					DestroyCursor( hHandCursor );
					hHandCursor = NULL;
				}
				FreeLibrary( hModule );
				hModule = NULL;
			}
		}	
	}
}

BOOL CHyperLink::LauchIEBrowser( LPCTSTR lpcszUrl )
{
	__try
	{
		IWebBrowser2* pBrowserApp = NULL;
		WCHAR wszUrlPage[1024];
		VARIANT vtEmpty;

		CoInitialize( NULL );
		
		if( FAILED( CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_SERVER, IID_IWebBrowser2, (void**)&pBrowserApp) ) )
		{
			CoUninitialize();
			return FALSE;
		}
		
		if (pBrowserApp != NULL)
		{
			// show the browser.
			HWND hwnd = NULL;
			pBrowserApp->get_HWND((long *) &hwnd);
			if ( hwnd != NULL )
			{
				::SetForegroundWindow(hwnd); 
				//::ShowWindow(hwnd, SW_SHOWMAXIMIZED);
				::ShowWindow(hwnd, SW_SHOW);
			}

			// visit the url.
			if ( _tcslen(lpcszUrl) > 0 )
			{
				memset( wszUrlPage,0,sizeof(wszUrlPage) );
				#ifdef _UNICODE
					_sntprintf( wszUrlPage, sizeof(wszUrlPage)-sizeof(TCHAR), _T("%s"), lpcszUrl );
				#else
					MultiByteToWideChar( CP_ACP, 0, lpcszUrl, -1, wszUrlPage, (sizeof(wszUrlPage)/sizeof(WCHAR))-1 );
				#endif
				VARIANT varUrl;
				vtEmpty.vt = VT_EMPTY;
				varUrl.vt = VT_BSTR;
				varUrl.bstrVal = SysAllocString(wszUrlPage);
				HRESULT hr = pBrowserApp->Navigate2(&varUrl, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);
				SysFreeString( varUrl.bstrVal ); 
			}
			pBrowserApp->Release();
		}
		CoUninitialize();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		CoUninitialize();
		return FALSE;
	}
	return TRUE;
}

VOID CHyperLink::UpdateLinkTipText()
{
	if ( 0 == _tcslen(m_szHint) )
	{
		m_ToolTip.UpdateTipText( m_strURL, this, TOOLTIP_ID );
	}
	else
	{
		m_ToolTip.UpdateTipText( m_szHint, this, TOOLTIP_ID );
	}
}