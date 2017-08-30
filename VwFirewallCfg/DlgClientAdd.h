#if !defined(AFX_DLGCLIENTADD_H__C648D03A_6BBE_4107_804A_5D854D9EF7A0__INCLUDED_)
#define AFX_DLGCLIENTADD_H__C648D03A_6BBE_4107_804A_5D854D9EF7A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgClientAdd.h : header file
//
#include "ProcFunc.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgClientAdd dialog

class CDlgClientAdd :
	public CDialog,
	public CProcFunc
{
// Construction
public:
	CDlgClientAdd(CWnd* pParent = NULL);   // standard constructor

	BOOL  m_bUse;
	TCHAR m_szClientName[ MAX_PATH ];

// Dialog Data
	//{{AFX_DATA(CDlgClientAdd)
	enum { IDD = IDD_DLG_CLIENTADD };
	CEdit	m_editClientName;
	CButton	m_chkUse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgClientAdd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgClientAdd)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCLIENTADD_H__C648D03A_6BBE_4107_804A_5D854D9EF7A0__INCLUDED_)
