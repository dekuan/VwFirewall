// HyperLink.h : header file
//
//
// HyperLink static control. Will open the default browser with the given URL
// when the user clicks on the link.
//
// Copyright Chris Maunder, 1997, 1998
// Feel free to use and distribute. May not be sold for profit. 

#ifndef __HYPERLINK_HEADER__
#define __HYPERLINK_HEADER__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



/////////////////////////////////////////////////////////////////////////////
// CHyperLink window
#include "ColorStatic.h"

class CHyperLink : public CColorStatic
{
public:
	CHyperLink();
	virtual ~CHyperLink();

public:


public:

	//	设置回调消息
	BOOL SetCallbackMessage( HWND hMainWnd, UINT uMessage, WPARAM wParam, LPARAM lParam );
	VOID ExecuteClick();

	VOID SetURL( CString strURL );
	CString GetURL() const;

	VOID SetStaticOnly( BOOL bStaticOnly );
	VOID SetColours( COLORREF crLinkColour, COLORREF crVisitedColour, COLORREF crHoverColour = -1 );
	COLORREF GetLinkColour() const;
	COLORREF GetVisitedColour() const;
	COLORREF GetHoverColour() const;

	VOID SetVisited(BOOL bVisited = TRUE);
	BOOL GetVisited() const;

	VOID SetLinkCursor(HCURSOR hCursor);
	HCURSOR GetLinkCursor() const;

	VOID SetUnderline(BOOL bUnderline = TRUE);
	BOOL GetUnderline() const;

	VOID SetAutoSize(BOOL bAutoSize = TRUE);
	BOOL GetAutoSize() const;

	VOID SetHint( TCHAR * pszHint );

	//	打开一个网页地址
	BOOL LauchIEBrowser( LPCTSTR lpcszUrl );

protected:

	void PositionWindow();
	void SetDefaultCursor();
	VOID UpdateLinkTipText();


// Protected attributes
protected:

	HWND	m_hMainWnd;
	UINT	m_uCallBackMessage;
	WPARAM	m_wCallBackParam;
	LPARAM	m_lCallBackParam;

	COLORREF m_crLinkColour, m_crVisitedColour;     // Hyperlink colours
	COLORREF m_crHoverColour;                       // Hover colour
	BOOL	 m_bStaticOnly;				// this is a static only
	BOOL     m_bOverControl;                        // cursor over control?
	BOOL	 m_bUnderline;                          // underline hyperlink?
	BOOL     m_bVisited;                            // Has it been visited?
	BOOL     m_bAdjustToFit;                        // Adjust window size to fit text?
	CString  m_strURL;                              // hyperlink URL
	TCHAR    m_szHint[ MAX_PATH ];			// hint
	CFont    m_Font;                                // Underline font if necessary
	HCURSOR  m_hLinkCursor;                         // Cursor for hyperlink
	CToolTipCtrl m_ToolTip;                         // The tooltip


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperLink)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CHyperLink)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnClicked();
	DECLARE_MESSAGE_MAP()
};






#endif // __HYPERLINK_HEADER__
