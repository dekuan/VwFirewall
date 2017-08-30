#if !defined(AFX_TABUPDATE_H__9CEC8A10_D4D9_4E0B_9F1E_D261CE66DEE5__INCLUDED_)
#define AFX_TABUPDATE_H__9CEC8A10_D4D9_4E0B_9F1E_D261CE66DEE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabUpdate.h : header file
//




#include "TabBaseInfo.h"



/////////////////////////////////////////////////////////////////////////////
// CTabUpdate dialog

class CTabUpdate :
	public CDialog,
	public CTabBaseInfo
{
// Construction
public:
	CTabUpdate(CWnd* pParent = NULL);   // standard constructor

	CXingBalloon * m_pxballoon;

	CHyperLink m_LinkUpdate;

// Dialog Data
	//{{AFX_DATA(CTabUpdate)
	enum { IDD = IDD_TAB_UPDATE };
	CStatic	m_stcCurrVer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabUpdate)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabUpdate)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonCheckforupdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABUPDATE_H__9CEC8A10_D4D9_4E0B_9F1E_D261CE66DEE5__INCLUDED_)
