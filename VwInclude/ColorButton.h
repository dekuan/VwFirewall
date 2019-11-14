#if !defined(AFX_COLORBUTTON_H__3342913E_7E9C_480D_853A_DF4528875309__INCLUDED_)
#define AFX_COLORBUTTON_H__3342913E_7E9C_480D_853A_DF4528875309__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorButton window

class CColorButton : public CButton
{
// Construction
public:
	CColorButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	CBrush m_brush;
	void SetBackColor(COLORREF BackColor);

	virtual ~CColorButton();

	// Generated message map functions
protected:
	COLORREF m_BackColor;

	//{{AFX_MSG(CColorButton)
	afx_msg HBRUSH CtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};









/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CBitmapButton window

class CMyBitmapButton : public CBitmapButton
{
	// Construction
public:
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	
	// Generated message map functions
protected:
	
	//{{AFX_MSG(CColorButton)
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
		
		void DrawItem(LPDRAWITEMSTRUCT lpDIS)
	{
		ASSERT(lpDIS != NULL);
		// must have at least the first bitmap loaded before calling DrawItem
		ASSERT(m_bitmap.m_hObject != NULL);     // required
		
		// use the main bitmap for up, the selected bitmap for down
		CBitmap* pBitmap = &m_bitmap;
		UINT state = lpDIS->itemState;
		if ((state & ODS_SELECTED) && m_bitmapSel.m_hObject != NULL)
			pBitmap = &m_bitmapSel;
		else if ((state & ODS_FOCUS) && m_bitmapFocus.m_hObject != NULL)
			pBitmap = &m_bitmapFocus;   // third image for focused
		else if ((state & ODS_DISABLED) && m_bitmapDisabled.m_hObject != NULL)
			pBitmap = &m_bitmapDisabled;   // last image for disabled
		
		// draw the whole button
		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap* pOld = memDC.SelectObject(pBitmap);
		if (pOld == NULL)
			return;     // destructors will clean up
		
		BITMAP bm;
		pBitmap->GetBitmap(&bm);
		
		CRect rcClient;
		GetClientRect(&rcClient);

		CRect rect;
		rect.CopyRect(&lpDIS->rcItem);
		pDC->StretchBlt(rect.left, rect.top, rcClient.right, rcClient.bottom, 
			&memDC, 0, 0, bm.bmWidth,bm.bmHeight, SRCCOPY);
		memDC.SelectObject(pOld);
	}
};





#endif // !defined(AFX_COLORBUTTON_H__3342913E_7E9C_480D_853A_DF4528875309__INCLUDED_)
