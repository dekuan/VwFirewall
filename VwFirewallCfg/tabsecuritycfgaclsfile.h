#if !defined(AFX_TABSECURITYCFGACLS_H__C97C51F9_28C6_4A11_883C_2C18AD247659__INCLUDED_)
#define AFX_TABSECURITYCFGACLS_H__C97C51F9_28C6_4A11_883C_2C18AD247659__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSecurityCfgACLS.h : header file
//

#include "VwFirewallConfigFile.h"
#include "ParseCfgData.h"
#include "QueryAclsInfo.h"
#include "TabSecurityCfgBase.h"

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgACLSFile dialog

class CTabSecurityCfgACLSFile :
	public CDialog
{
// Construction
public:
	CTabSecurityCfgACLSFile(CWnd* pParent = NULL);   // standard constructor
	~CTabSecurityCfgACLSFile();

public:
	BOOL GetPermissionsDesc( ACCESS_MASK amMask, LPTSTR lpszPermissionsDesc, DWORD dwSize );
	BOOL TestGroup();

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
//	HIMAGELIST m_hImageListGroup;

	CParseCfgData m_CfgAclsFileData;
	CQueryAclsInfo m_cQueryAclsInfo;

public:
// Dialog Data
	//{{AFX_DATA(CTabSecurityCfgACLSFile)
	enum { IDD = IDD_TAB_SECURITYCFG_ACLS_FILE };
	CButtonST	m_btnRecover;
	CButtonST	m_btnDetect;
	CButtonST	m_btnFix;
	CProgressCtrl	m_proBar;
	CListCtrl	m_listGroup;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSecurityCfgACLSFile)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabSecurityCfgACLSFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDetect();
	afx_msg void OnButtonFix();
	afx_msg void OnDblclkListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuListDetail();
	afx_msg void OnButtonRecover();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSECURITYCFGACLS_H__C97C51F9_28C6_4A11_883C_2C18AD247659__INCLUDED_)
