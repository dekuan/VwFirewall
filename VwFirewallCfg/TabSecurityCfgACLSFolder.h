#if !defined(AFX_TABSECURITYCFGACLSFOLDER_H__5BBD44A1_3515_4219_9B22_026B34F2A088__INCLUDED_)
#define AFX_TABSECURITYCFGACLSFOLDER_H__5BBD44A1_3515_4219_9B22_026B34F2A088__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSecurityCfgACLSFolder.h : header file
//
#include "TabSecurityCfgBase.h"


/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgACLSFolder dialog

class CTabSecurityCfgACLSFolder : public CDialog
{
// Construction
public:
	CTabSecurityCfgACLSFolder(CWnd* pParent = NULL);   // standard constructor
	~CTabSecurityCfgACLSFolder();
	
public:
	VOID LoadDataFromCfgFile();
	VOID InitList();
	VOID DetectAclsProblems();
	VOID FixAclsProblems();

private:
	static unsigned __stdcall _threadFixAclsProblems( PVOID arglist );
	VOID FixAclsProblemsProc();
	CDeThreadSleep m_cThSleepFixAclsProblems;
	BOOL m_bDoingFix;
	BOOL m_bFixInProgress;

//	static unsigned __stdcall _threadFlashFixingIcon( PVOID arglist );
//	VOID FlashFixingIconProc();
//	CDeThreadSleep m_cThSleepFlashFixingIcon;
//	LONG m_lnListRowIndex;

public:
	TCHAR m_szInfo[ MAX_PATH ];
	DWORD m_dwInfoType;

private:
	CTabSecurityCfgBase m_cTabSecurityCfgBase;
	CVwFirewallConfigFile m_cVwFirewallConfigFile;

	CParseCfgData m_CfgAclsFolderData;
	CQueryAclsInfo m_cQueryAclsInfo;
	
public:
// Dialog Data
	//{{AFX_DATA(CTabSecurityCfgACLSFolder)
	enum { IDD = IDD_TAB_SECURITYCFG_ACLS_FOLDER };
	CButtonST	m_btnRecover;
	CButtonST	m_btnDetect;
	CButtonST	m_btnFix;	
	CProgressCtrl	m_proBar;
	CListCtrl	m_listGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSecurityCfgACLSFolder)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabSecurityCfgACLSFolder)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDetect();
	afx_msg void OnButtonFix();
	afx_msg void OnMenuListDetail();
	afx_msg void OnRclickListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRecover();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSECURITYCFGACLSFOLDER_H__5BBD44A1_3515_4219_9B22_026B34F2A088__INCLUDED_)
