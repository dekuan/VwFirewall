#if !defined(AFX_DLGPROTECTEDDIRADD_H__FB4869D7_56E0_4FA1_8A16_6E9ADD300C88__INCLUDED_)
#define AFX_DLGPROTECTEDDIRADD_H__FB4869D7_56E0_4FA1_8A16_6E9ADD300C88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProtectedDirAdd.h : header file
//
#include "ProcFunc.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgProtectedDirAdd dialog

class CDlgProtectedDirAdd : public CDialog
{
// Construction
public:
	CDlgProtectedDirAdd(CWnd* pParent = NULL);   // standard constructor

	BOOL  m_bUse;
	TCHAR m_szDir[ MAX_PATH ];
	BOOL  m_bTree;
	TCHAR m_szEcpExt[ 1024 ];	//	max length = 768

// Dialog Data
	//{{AFX_DATA(CDlgProtectedDirAdd)
	enum { IDD = IDD_DLG_PROTECTEDDIR_ADD };
	CButton	m_chkTree;
	CButton	m_chkUse;
	CEdit	m_editEcpExt;
	CEdit	m_editDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProtectedDirAdd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgProtectedDirAdd)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBrowserDir();
	afx_msg void OnButtonSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROTECTEDDIRADD_H__FB4869D7_56E0_4FA1_8A16_6E9ADD300C88__INCLUDED_)
