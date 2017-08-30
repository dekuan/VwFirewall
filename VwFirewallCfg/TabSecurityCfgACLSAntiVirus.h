#if !defined(AFX_TABSECURITYCFGACLSANTIVIRUS_H__FEE1B732_7AF2_4034_8D00_DED6AE276D3B__INCLUDED_)
#define AFX_TABSECURITYCFGACLSANTIVIRUS_H__FEE1B732_7AF2_4034_8D00_DED6AE276D3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSecurityCfgACLSAntiVirus.h : header file
//

#include "VwFirewallConfigFile.h"
#include "ParseCfgData.h"
#include "QueryAclsInfo.h"
#include "TabSecurityCfgBase.h"



/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgACLSAntiVirus dialog

class CTabSecurityCfgACLSAntiVirus : public CDialog
{
// Construction
public:
	CTabSecurityCfgACLSAntiVirus(CWnd* pParent = NULL);   // standard constructor
	~CTabSecurityCfgACLSAntiVirus();
	
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

public:
	TCHAR m_szInfo[ MAX_PATH ];
	DWORD m_dwInfoType;

private:
	CTabSecurityCfgBase m_cTabSecurityCfgBase;
	CVwFirewallConfigFile m_cVwFirewallConfigFile;

	CParseCfgData m_CfgAclsAntiVirusData;
	CQueryAclsInfo m_cQueryAclsInfo;

// Dialog Data
	//{{AFX_DATA(CTabSecurityCfgACLSAntiVirus)
	enum { IDD = IDD_TAB_SECURITYCFG_ACLS_ANTIVIRUS };
	CButtonST	m_btnRecover;
	CButtonST	m_btnDetect;
	CButtonST	m_btnFix;	
	CProgressCtrl	m_proBar;
	CListCtrl	m_listGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSecurityCfgACLSAntiVirus)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabSecurityCfgACLSAntiVirus)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuListDetail();
	afx_msg void OnButtonDetect();
	afx_msg void OnButtonFix();
	afx_msg void OnButtonRecover();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSECURITYCFGACLSANTIVIRUS_H__FEE1B732_7AF2_4034_8D00_DED6AE276D3B__INCLUDED_)
