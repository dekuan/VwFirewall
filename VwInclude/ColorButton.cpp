// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

CColorButton::CColorButton()
{
//	SetBackColor(RGB(222,223,222));
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CMyBitmapButton, CBitmapButton)
//{{AFX_MSG_MAP(CMyBitmapButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

void CColorButton::SetBackColor(COLORREF BackColor)
{
	m_BackColor=BackColor;
	m_brush.CreateSolidBrush(m_BackColor);
}


HBRUSH CColorButton::CtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)m_brush;
}
