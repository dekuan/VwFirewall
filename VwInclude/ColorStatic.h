#if !defined(AFX_COLORSTATIC_H__57522FA8_09AB_470B_A4AB_0D85B93794EB__INCLUDED_)
#define AFX_COLORSTATIC_H__57522FA8_09AB_470B_A4AB_0D85B93794EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorStatic window

class CColorStatic : public CStatic
{
// Construction
public:
	CColorStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	VOID SetRoundRectRgn( BOOL bRoundRectRgn );
	VOID SetTextAlign( DWORD dwAlign = DT_LEFT );
	VOID SetTextPaddingTop( DWORD dwTextPaddingTop );

	void SetNewFont(LOGFONT* lf);
	BOOL DrawCaption( LPCTSTR lpcszStr );
	BOOL DrawCaption( CString & str );
	BOOL DrawCaption( CDC * ,CString & str );
	void SetTextColor(COLORREF TextColor);
	COLORREF m_TextColor;
	void SetBackColor(COLORREF BackColor);
	void SetCaption(CString strCaption);
	void Create(CString strCaption,COLORREF BackColor);

	void InitToolTip();
	void SetTooltipText(LPCTSTR lpszText);


	BOOL		m_bRoundRectRgn;
	DWORD		m_dwTextAlign;
	DWORD		m_dwTextPaddingTop;

	COLORREF	m_BackColor;
	CString		m_strCaption;
	CDC			m_memDC;
	CBitmap		m_bmpBuffer;
	CRect		m_rcClient;
	CFont		m_Font;
	CFont*		m_pFont;

	CToolTipCtrl	m_ToolTip;

	virtual ~CColorStatic();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorStatic)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSTATIC_H__57522FA8_09AB_470B_A4AB_0D85B93794EB__INCLUDED_)
