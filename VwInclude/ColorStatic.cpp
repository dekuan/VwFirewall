// ColorStatic.cpp : implementation file
//

#include "stdafx.h"
#include "ColorStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatic

CColorStatic::CColorStatic()
{
	m_BackColor	= 0;	//	±³¾°ÑÕÉ«

	m_TextColor	= RGB(0,0,0);
	m_pFont		= NULL;

	m_bRoundRectRgn		= FALSE;
	m_dwTextAlign		= DT_LEFT;
	m_dwTextPaddingTop	= 0;
}

CColorStatic::~CColorStatic()
{
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatic message handlers

BOOL CColorStatic::PreTranslateMessage(MSG* pMsg) 
{
	if ( IsWindowVisible() && m_ToolTip.m_hWnd )
	{
		m_ToolTip.RelayEvent( pMsg );
	}
	
	//	if ( m_bFlat )
	//	{
	//		ModifyStyleEx( WS_EX_STATICEDGE, 0 );
	//	}

	return CStatic::PreTranslateMessage( pMsg );
}

int CColorStatic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CColorStatic::OnPaint() 
{
	CPaintDC dc(this);
	if(m_memDC.m_hDC==NULL)
	{
		CDC * pdc=&dc;
		m_memDC.CreateCompatibleDC(pdc);

		GetClientRect(&m_rcClient);

		m_bmpBuffer.CreateCompatibleBitmap(pdc,m_rcClient.Width(),m_rcClient.Height());
	}
	
	DrawCaption(&dc,m_strCaption);


	// Do not call CStatic::OnPaint() for painting messages
}

void CColorStatic::Create(CString strCaption, COLORREF BackColor)
{
}

VOID CColorStatic::SetRoundRectRgn( BOOL bRoundRectRgn )
{
	m_bRoundRectRgn = bRoundRectRgn;
}

VOID CColorStatic::SetTextAlign( DWORD dwAlign /* = DT_LEFT */ )
{
	m_dwTextAlign = dwAlign;
}

VOID CColorStatic::SetTextPaddingTop( DWORD dwTextPaddingTop )
{
	m_dwTextPaddingTop = dwTextPaddingTop;
}


void CColorStatic::SetCaption(CString strCaption)
{
	m_strCaption=strCaption;
}

void CColorStatic::SetBackColor(COLORREF BackColor)
{
	m_BackColor=BackColor;
}

void CColorStatic::SetTextColor(COLORREF TextColor)
{
	m_TextColor=TextColor;
}

BOOL CColorStatic::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	
	//return CStatic::OnEraseBkgnd(pDC);
}

BOOL CColorStatic::DrawCaption(CDC *pdc,CString &str)
{
	CBitmap * poldbmp=m_memDC.SelectObject(&m_bmpBuffer);

	// TODO: Add your message handler code here
	m_memDC.FillSolidRect( 0, 0, m_rcClient.Width(), m_rcClient.Height(), m_BackColor );
	m_memDC.SetBkColor( m_BackColor );
	m_memDC.SetBkMode( TRANSPARENT );

	CFont *pOldFont;
	CRect rcText;
	if ( m_pFont == NULL )
	{
		m_pFont =GetParent()->GetFont();
	}

	pOldFont = m_memDC.SelectObject(m_pFont);

	rcText = m_rcClient;
	rcText.top += m_dwTextPaddingTop;

	m_memDC.SetTextColor(m_TextColor);
	m_memDC.DrawText( str, &rcText, m_dwTextAlign );
	m_memDC.SelectObject(pOldFont);

	if ( m_bRoundRectRgn )
	{
		pdc->BitBlt( 0, 1, 1, m_rcClient.Height()-2, &m_memDC, 0, 0, SRCCOPY );
		pdc->BitBlt( 1, 0, m_rcClient.Width()-2, m_rcClient.Height(), &m_memDC, 0, 0, SRCCOPY );
		pdc->BitBlt( m_rcClient.Width()-1, 1, 1, m_rcClient.Height()-2, &m_memDC, 0, 0, SRCCOPY );
	}
	else
	{
		pdc->BitBlt( 0, 0, m_rcClient.Width(), m_rcClient.Height(), &m_memDC, 0, 0, SRCCOPY );
	}	


	m_memDC.SelectObject(poldbmp);

	return TRUE;
}

BOOL CColorStatic::DrawCaption(CString &str)
{
	CDC * pdc=GetDC();
	DrawCaption(pdc,str);
	ReleaseDC(pdc);

	return TRUE;
}

BOOL CColorStatic::DrawCaption( LPCTSTR lpcszStr )
{
	CString strTemp;
	CDC * pdc = GetDC();

	strTemp = lpcszStr;
	DrawCaption( pdc, strTemp );
	ReleaseDC( pdc );

	return TRUE;
}

void CColorStatic::SetNewFont(LOGFONT* lf)
{
	m_Font.CreateFontIndirect(lf);
	m_pFont = &m_Font;
}


void CColorStatic::InitToolTip()
{
	if ( m_ToolTip.m_hWnd == NULL )
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		
		// Create inactive
		m_ToolTip.Activate(TRUE);
		
		// Enable multiline
		m_ToolTip.SendMessage( TTM_SETMAXTIPWIDTH, 0, 400 );
	}
}

void CColorStatic::SetTooltipText(LPCTSTR lpszText)
{
	// We cannot accept NULL pointer
	if ( lpszText == NULL )
		return;
	
	// Initialize ToolTip
	InitToolTip();
	
	//	If there is no tooltip defined then add it
	if ( m_ToolTip.GetToolCount() == 0 )
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool( this, lpszText, rectBtn, 1);
	}
	
	//	Set text for tooltip
	m_ToolTip.UpdateTipText( lpszText, this, 1 );
	m_ToolTip.Activate(TRUE);
}