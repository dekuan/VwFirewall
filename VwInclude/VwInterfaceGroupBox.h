//////////////////////////////////////////////////////////////////////////
//	VwInterfaceGroupBox.h
//

#ifndef __VWINTERFACEGROUPBOX_HEADER__
#define __VWINTERFACEGROUPBOX_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TGroupBox.h : header file
//

#define OFS_X	10 // distance from left/right side to beginning/end of text

/////////////////////////////////////////////////////////////////////////////
// CVwInterfaceGroupBox window

class CVwInterfaceGroupBox : public CButton
{
// Construction
public:
	CVwInterfaceGroupBox();

	VOID SetCapture( CString strCaption );
	VOID SetTextColor( COLORREF crTextColor );
	VOID SetBorderColor( COLORREF crBorderColor );
	VOID SetBackColor( COLORREF crBackColor );
	VOID SetNewFont( LOGFONT * lf );

private:

	BOOL DrawCaption( CDC * pdc, CRect rcText, CString & str );


// Attributes
public:

private:
	CDC m_memDC;
	CBitmap m_bmpBuffer;
	CFont * m_pFont;
	CFont m_Font;

	CString  m_strCaption;
	COLORREF m_crTextColor;
	COLORREF m_crBorderColor;
	COLORREF m_crBackColor;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVwInterfaceGroupBox)
	public:
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVwInterfaceGroupBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVwInterfaceGroupBox)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __VWINTERFACEGROUPBOX_HEADER__
