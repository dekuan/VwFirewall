#if !defined(AFX_DLGIMPORT_H__54B8EE79_20C8_4B22_B6E2_8074D12129A9__INCLUDED_)
#define AFX_DLGIMPORT_H__54B8EE79_20C8_4B22_B6E2_8074D12129A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImport.h : header file
//

#include "ProcFunc.h"



/////////////////////////////////////////////////////////////////////////////
// CDlgImport dialog

class CDlgImport :
	public CDialog,
	public CProcFunc
{
// Construction
public:
	CDlgImport(CWnd* pParent = NULL);   // standard constructor

	BOOL ReadAllIISHostToList();
public:
	CListCtrl * m_pListParent;

private:
	CString m_strCaption;
	CString m_strString;

public:
// Dialog Data
	//{{AFX_DATA(CDlgImport)
	enum { IDD = IDD_DLG_IMPORT };
	CButton	m_btnImport;
	CListCtrl	m_listDomain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImport)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonImport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORT_H__54B8EE79_20C8_4B22_B6E2_8074D12129A9__INCLUDED_)
