#if !defined(AFX_DLGSETRDPPORT_H__60608661_8B7E_4B9F_94A4_0BF3DF8984BD__INCLUDED_)
#define AFX_DLGSETRDPPORT_H__60608661_8B7E_4B9F_94A4_0BF3DF8984BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetRDPPort.h : header file
//

#include "ProcFunc.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgSetRDPPort dialog

class CDlgSetRDPPort :
	public CDialog,
	public CProcFunc	
{
// Construction
public:
	CDlgSetRDPPort(CWnd* pParent = NULL);   // standard constructor

	BOOL LoadConfig();
	BOOL SaveConfig();


	CString m_strCaption;
	CString m_strString;

// Dialog Data
	//{{AFX_DATA(CDlgSetRDPPort)
	enum { IDD = IDD_DLG_SETRDPPORT };
	CButton	m_btnSave;
	CButton	m_chkAddToFwException;
	CEdit	m_editNewPort;
	CEdit	m_editOrgPort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetRDPPort)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetRDPPort)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	afx_msg void OnChangeEditNewPort();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETRDPPORT_H__60608661_8B7E_4B9F_94A4_0BF3DF8984BD__INCLUDED_)
