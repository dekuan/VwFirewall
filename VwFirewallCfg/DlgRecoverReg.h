#if !defined(AFX_DLGRECOVERREG_H__E0674598_98AF_4E9F_94E9_35CF1DCE444D__INCLUDED_)
#define AFX_DLGRECOVERREG_H__E0674598_98AF_4E9F_94E9_35CF1DCE444D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRecoverReg.h : header file
//
#include "VwFirewallConfigFile.h"

//	...
#define CDLGRECOVERREG_TYPE_OBJECT	0x001
#define CDLGRECOVERREG_TYPE_SERVICE	0x002



/////////////////////////////////////////////////////////////////////////////
// CDlgRecoverReg dialog

class CDlgRecoverReg : public CDialog
{
// Construction
public:
	CDlgRecoverReg(CWnd* pParent = NULL);   // standard constructor


	VOID InitList();

	//	...
	DWORD   m_dwType;
	HWND    m_hParentWnd;
	TCHAR * m_pszRecoverRegCmdLine;
	DWORD   m_dwRecoverRegCmdLineSize;

private:
	TCHAR * m_pszBackupDir;
	CVwFirewallConfigFile m_cVwFirewallConfigFile;

// Dialog Data
	//{{AFX_DATA(CDlgRecoverReg)
	enum { IDD = IDD_DLG_RECOVER_REG };
	CStatic	m_stcHint;
	CButton	m_btnRecover;
	CListCtrl	m_listFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRecoverReg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRecoverReg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRecover();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRECOVERREG_H__E0674598_98AF_4E9F_94E9_35CF1DCE444D__INCLUDED_)
