// VwInterfaceGroupBox.cpp : implementation file
//

#include "stdafx.h"
#include "VwInterfaceGroupBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVwInterfaceGroupBox

CVwInterfaceGroupBox::CVwInterfaceGroupBox()
{
	m_pFont		= NULL;
	m_strCaption	= _T("");

	m_crTextColor	= RGB( 0x00, 0x00, 0x00 );
	m_crBorderColor	= ::GetSysColor( COLOR_3DSHADOW );
	m_crBackColor	= RGB( 0xFF, 0xFF, 0xFF );
}
CVwInterfaceGroupBox::~CVwInterfaceGroupBox()
{
}


BEGIN_MESSAGE_MAP(CVwInterfaceGroupBox, CButton)
	//{{AFX_MSG_MAP(CVwInterfaceGroupBox)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVwInterfaceGroupBox message handlers

void CVwInterfaceGroupBox::PreSubclassWindow() 
{
	//ModifyStyle(0, BS_OWNERDRAW|BS_GROUPBOX);

	CButton::PreSubclassWindow();
}

VOID CVwInterfaceGroupBox::SetCapture( CString strCaption )
{
	m_strCaption = strCaption;

	Invalidate();
}

VOID CVwInterfaceGroupBox::SetTextColor( COLORREF crTextColor )
{
	m_crTextColor = crTextColor;	
}

VOID CVwInterfaceGroupBox::SetBorderColor( COLORREF crBorderColor )
{
	m_crBorderColor = crBorderColor;
}

VOID CVwInterfaceGroupBox::SetBackColor( COLORREF crBackColor )
{
	m_crBackColor = crBackColor;
}

VOID CVwInterfaceGroupBox::SetNewFont( LOGFONT * lf )
{
	m_Font.CreateFontIndirect(lf);
	m_pFont = &m_Font;
}

// Since groupboxes never get WM_DRAWITEM even if they are BS_OWNERDRAW,
// we must handle our painting in OnPaint.
// Fortunately they have no such things like focus rects or "pressed" states.
void CVwInterfaceGroupBox::OnPaint() 
{
	CRect rc;
	CRect rcText;
	CPoint ptStart, ptEnd;
	CSize seText;
	CString sText, sTemp;
	CPen pnFrmDark, pnFrmLight, *ppnOldPen;
	int iUpDist, nSavedDC;
	DWORD dwStyle, dwExStyle;


	CPaintDC dc(this); // device context for painting

	// save dc state
	nSavedDC = dc.SaveDC();

	// window rect
	GetWindowRect(&rc);
	ScreenToClient(&rc);

	if ( m_pFont == NULL )
	{
		m_pFont = GetParent()->GetFont();
	}	

	CFont * pFontOld = dc.SelectObject( m_pFont );

	/*
	if ( 0 )
	{
		HFONT hFont	= NULL;
		hFont = CreateFont(
			12, 12 / 2, 0,
			0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_ROMAN, _T("Ms sans serif") );
		HFONT hFontOld = (HFONT)SelectObject( dc.GetSafeHdc(), hFont );
	}
	*/

	


	// determine text length
	//GetWindowText(sTemp);
	sText.Format( _T(" %s "), m_strCaption ); // looks better with a blank on each side	
	seText = dc.GetTextExtent(sText);

	// distance from window top to group rect
	iUpDist = (seText.cy / 2);

	// calc rect and start/end points
	dwStyle = GetStyle();
	dwExStyle = GetExStyle();

	// handle the extended windows styles
	if (dwExStyle & WS_EX_DLGMODALFRAME)
	{
		rc.DeflateRect(2*GetSystemMetrics(SM_CXEDGE), 2*GetSystemMetrics(SM_CYEDGE));
		rc.left--;
	}
	if (dwExStyle & WS_EX_CLIENTEDGE)
	{
		rc.DeflateRect(GetSystemMetrics(SM_CXEDGE)+1, GetSystemMetrics(SM_CYEDGE)+1);
		rc.left--;
		if (dwExStyle & WS_EX_DLGMODALFRAME)
		{
			rc.right++;
		}
	}	
	if(dwExStyle & WS_EX_STATICEDGE)
	{
		rc.DeflateRect(2*GetSystemMetrics(SM_CXBORDER), 2*GetSystemMetrics(SM_CYBORDER));
		rc.left--;
	}

	// handle text alignment (Caution: BS_CENTER == BS_LEFT|BS_RIGHT!!!)
	ptStart.y = ptEnd.y = rc.top + iUpDist;
	if ( BS_RIGHT == ( BS_RIGHT & dwStyle ) )
	{
		//	right aligned
		ptEnd.x = rc.right - OFS_X;
		ptStart.x = ptEnd.x - seText.cx;
	}
	else if ( BS_CENTER == ( dwStyle & BS_CENTER ) )
	{
		//	text centered
		ptStart.x = (rc.Width() - seText.cx) / 2;
		ptEnd.x = ptStart.x + seText.cx;
	}
	else
	//else if ( ( ! ( dwStyle & BS_CENTER ) ) || ( ( dwStyle & BS_CENTER ) == BS_LEFT ) )// left aligned	/ default
	{
		ptStart.x = rc.left + OFS_X;
		ptEnd.x = ptStart.x + seText.cx;
	}

	if ( sText.GetLength() <= 2 )
	{
		ptEnd = ptStart;
	}

	//
	//	ÎÄ×ÖÇøÓò
	//
	rcText.top	= rc.top;
	rcText.bottom	= rc.top + seText.cy;
	rcText.left	= ptStart.x;
	rcText.right	= rc.right;

	//
	//	ÎÄ×ÖÇøÓò Ìî³ä±³¾°
	//			
	dc.FillSolidRect( rcText, m_crBackColor );


	if (dwStyle & BS_FLAT) // "flat" frame
	{
		VERIFY(pnFrmDark.CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
		VERIFY(pnFrmLight.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT)));

		ppnOldPen = dc.SelectObject(&pnFrmDark);

		dc.MoveTo(ptStart);
		dc.LineTo(rc.left, ptStart.y);
		dc.LineTo(rc.left, rc.bottom);
		dc.LineTo(rc.right, rc.bottom);
		dc.LineTo(rc.right, ptEnd.y);
		dc.LineTo(ptEnd);

		dc.SelectObject(&pnFrmLight);

		dc.MoveTo(ptStart.x, ptStart.y+1);
		dc.LineTo(rc.left+1, ptStart.y+1);
		dc.LineTo(rc.left+1, rc.bottom-1);
		dc.LineTo(rc.right-1, rc.bottom-1);
		dc.LineTo(rc.right-1, ptEnd.y+1);
		dc.LineTo(ptEnd.x, ptEnd.y+1);

	}
	else // 3D frame
	{
		VERIFY(pnFrmDark.CreatePen( PS_SOLID, 1, m_crBorderColor ));
		VERIFY(pnFrmLight.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT)));

		ppnOldPen = dc.SelectObject(&pnFrmDark);

		//	conner left-top
		dc.MoveTo( ptStart.x, ptStart.y );
		dc.LineTo( rc.left+2, ptStart.y );

		dc.MoveTo( rc.left+2, ptStart.y );
		dc.LineTo( rc.left+2, ptStart.y+2 );

		dc.MoveTo( rc.left+1, ptStart.y+1 );
		dc.LineTo( rc.left+1, ptStart.y+3 );

		//	top -> bottom
		dc.MoveTo( rc.left, ptStart.y+2 );
		dc.LineTo( rc.left, rc.bottom-1 );

		//	corner left-bottom
		dc.MoveTo( rc.left+1, rc.bottom-2 );
		dc.LineTo( rc.left+1, rc.bottom );

		dc.MoveTo( rc.left+2, rc.bottom-1 );
		dc.LineTo( rc.left+2, rc.bottom+1 );

		//	left -> right @ bottom
		dc.MoveTo( rc.left+3, rc.bottom );
		dc.LineTo( rc.right-2, rc.bottom );

		//	corner right-bottom
		dc.MoveTo( rc.right-2, rc.bottom-1 );
		dc.LineTo( rc.right-2, rc.bottom+1 );

		dc.MoveTo( rc.right-1, rc.bottom-2 );
		dc.LineTo( rc.right-1, rc.bottom );

		//	bottom -> top @ right
		dc.MoveTo( rc.right, rc.bottom-2 );
		dc.LineTo( rc.right, ptStart.y+1 );

		//	conner right-top
		dc.MoveTo( rc.right-1, ptStart.y+2 );
		dc.LineTo( rc.right-1, ptStart.y );

		dc.MoveTo( rc.right-2, ptStart.y+1 );
		dc.LineTo( rc.right-2, ptStart.y-1 );

		//	right -> left end
		dc.MoveTo( rc.right-2, ptStart.y );
		dc.LineTo( ptEnd.x, ptEnd.y );

	
//		dc.MoveTo( ptStart );
//		dc.LineTo( rc.left, ptStart.y );
//		dc.LineTo( rc.left, rc.bottom-1 );
//		dc.LineTo( rc.right-1, rc.bottom-1 );
//		dc.LineTo( rc.right-1, ptEnd.y );
//		dc.LineTo( ptEnd );

		/*
		dc.SelectObject(&pnFrmLight);

		dc.MoveTo(ptStart.x, ptStart.y+1);
		dc.LineTo(rc.left+1, ptStart.y+1);
		dc.LineTo(rc.left+1, rc.bottom-1);
		dc.MoveTo(rc.left, rc.bottom);
		dc.LineTo(rc.right, rc.bottom);
		dc.LineTo(rc.right, ptEnd.y-1);
		dc.MoveTo(rc.right-2, ptEnd.y+1);
		dc.LineTo(ptEnd.x, ptEnd.y+1);
		*/
	}

	// draw text (if any)
	if (!sText.IsEmpty() && !(dwExStyle & (BS_ICON|BS_BITMAP)))
	{
		if (!IsWindowEnabled())
		{
			ptStart.y -= iUpDist;
			dc.DrawState(ptStart, seText, sText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		}
		else
		{
			dc.SetBkMode(TRANSPARENT);
			dc.SetTextColor( m_crTextColor );
			
			dc.DrawText( sText, rcText, DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_NOCLIP);
		}
	}

	// cleanup
	dc.SelectObject( ppnOldPen );

	//	font
	dc.SelectObject( pFontOld );


	dc.RestoreDC(nSavedDC);
}

BOOL CVwInterfaceGroupBox::DrawCaption( CDC * pdc, CRect rcText, CString & str )
{
	CBitmap * poldbmp=m_memDC.SelectObject( &m_bmpBuffer );
	
	// TODO: Add your message handler code here
	m_memDC.FillSolidRect( 0, 0, rcText.Width(), rcText.Height(), m_crBackColor );
	m_memDC.SetBkColor( m_crBackColor );
	m_memDC.SetBkMode( TRANSPARENT );

	CFont * pOldFont;
	if ( m_pFont == NULL )
	{
		m_pFont = GetParent()->GetFont();
	}
	pOldFont=m_memDC.SelectObject(m_pFont);
	m_memDC.SetTextColor(m_crTextColor);
	m_memDC.DrawText(str,&rcText,DT_LEFT);
	m_memDC.SelectObject(pOldFont);
	
	pdc->BitBlt(0,0,rcText.Width(),rcText.Height(),&m_memDC,0,0,SRCCOPY);
	
	m_memDC.SelectObject(poldbmp);
	
	return TRUE;
}