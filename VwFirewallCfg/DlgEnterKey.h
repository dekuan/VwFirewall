#if !defined(AFX_DLGENTERKEY_H__0C24A2E9_2AD3_468B_A69E_C257C270D3C4__INCLUDED_)
#define AFX_DLGENTERKEY_H__0C24A2E9_2AD3_468B_A69E_C257C270D3C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEnterKey.h : header file
//

#include "TabBaseInfo.h"



/////////////////////////////////////////////////////////////////////////////
// CDlgEnterKey dialog

class CDlgEnterKey :
	public CDialog,
	public CTabBaseInfo
{
// Construction
public:
	CDlgEnterKey(CWnd* pParent = NULL);   // standard constructor

	VOID InitIpAddrComb( LPCTSTR lpcszIpAddr );




// Dialog Data
	//{{AFX_DATA(CDlgEnterKey)
	enum { IDD = IDD_DLG_ENTERKEY };
	CEdit	m_editRegKey;
	CComboBox	m_combRegIp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEnterKey)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEnterKey)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonCopy();
	afx_msg void OnButtonSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGENTERKEY_H__0C24A2E9_2AD3_468B_A69E_C257C270D3C4__INCLUDED_)
