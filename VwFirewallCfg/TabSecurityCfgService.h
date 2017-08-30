#if !defined(AFX_TABSECURITYCFGSERVICE_H__A1DF7661_370D_4E7C_B193_7660E3D02F71__INCLUDED_)
#define AFX_TABSECURITYCFGSERVICE_H__A1DF7661_370D_4E7C_B193_7660E3D02F71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSecurityCfgService.h : header file
//
#include "TabSecurityCfgBase.h"



/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgService dialog

class CTabSecurityCfgService : public CDialog
{
// Construction
public:
	CTabSecurityCfgService(CWnd* pParent = NULL);   // standard constructor
	~CTabSecurityCfgService();
	
public:
	VOID LoadDataFromCfgFile();
	VOID InitList();
	VOID DetectAclsProblems();

	VOID BackupServicesInRegister();
	VOID FixAclsProblems();

	VOID RecoverFixing();

private:
	static unsigned __stdcall _threadFixAclsProblems( PVOID arglist );
	VOID FixAclsProblemsProc();
	CDeThreadSleep m_cThSleepFixAclsProblems;
	BOOL m_bDoingFix;
	BOOL m_bFixInProgress;

	static unsigned __stdcall _threadRecoverFixing( PVOID arglist );
	VOID RecoverFixingProc();
	CDeThreadSleep m_cThSleepRecoverFixing;
	BOOL m_bDoingRecover;
	BOOL m_bRecoverInProgress;

	//	static unsigned __stdcall _threadFlashFixingIcon( PVOID arglist );
	//	VOID FlashFixingIconProc();
	//	CDeThreadSleep m_cThSleepFlashFixingIcon;
	//	LONG m_lnListRowIndex;

public:
	TCHAR m_szRecoverRegCmdLine[ 1024 ];
	TCHAR m_szInfo[ MAX_PATH ];
	DWORD m_dwInfoType;

private:
	CTabSecurityCfgBase m_cTabSecurityCfgBase;
	CVwFirewallConfigFile m_cVwFirewallConfigFile;
	
	CParseCfgData m_CfgServiceData;
	CQueryAclsInfo m_cQueryAclsInfo;
	
public:
// Dialog Data
	//{{AFX_DATA(CTabSecurityCfgService)
	enum { IDD = IDD_TAB_SECURITYCFG_SERVICE };
	CButtonST	m_btnRecover;
	CButtonST	m_btnDetect;
	CButtonST	m_btnFix;	
	CProgressCtrl	m_proBar;
	CListCtrl	m_listGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSecurityCfgService)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabSecurityCfgService)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDetect();
	afx_msg void OnButtonFix();
	afx_msg void OnDblclkListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuListSvcStart();
	afx_msg void OnMenuListSvcStop();
	afx_msg void OnButtonRecover();
	virtual LRESULT OnDoNext( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSECURITYCFGSERVICE_H__A1DF7661_370D_4E7C_B193_7660E3D02F71__INCLUDED_)
