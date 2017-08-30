#if !defined(AFX_TABABOUT_H__2B22C19B_EA37_4EAA_847C_9EEFA48E70CF__INCLUDED_)
#define AFX_TABABOUT_H__2B22C19B_EA37_4EAA_847C_9EEFA48E70CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabAbout.h : header file
//

#include "TabBaseInfo.h"



/////////////////////////////////////////////////////////////////////////////
// CTabAbout dialog

class CTabAbout :
	public CDialog,
	public CTabBaseInfo
{
// Construction
public:
	CTabAbout(CWnd* pParent = NULL);   // standard constructor


	BOOL LoadConfig();
	BOOL SaveConfig();


	CXingBalloon * m_pxballoon;

	CHyperLink m_LinkAboutMore;


	
// Dialog Data
	//{{AFX_DATA(CTabAbout)
	enum { IDD = IDD_TAB_ABOUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabAbout)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABABOUT_H__2B22C19B_EA37_4EAA_847C_9EEFA48E70CF__INCLUDED_)
