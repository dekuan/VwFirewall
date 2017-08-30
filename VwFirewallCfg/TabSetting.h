#if !defined(AFX_TABSETTING_H__CE615BEE_30C5_4E76_8A74_4A01DDD49CA2__INCLUDED_)
#define AFX_TABSETTING_H__CE615BEE_30C5_4E76_8A74_4A01DDD49CA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSetting.h : header file
//

#include "TabBaseInfo.h"
#include "hyperlink.h"

#include "TabFile.h"
#include "TabDomain.h"
#include "TabRdp.h"


typedef struct tagStatusIcon
{
	//	win icon
	HICON hIconO;
	HICON hIconX;

	//	ctrl buttons
	HICON hIconStatusDomainOk;
	HICON hIconStatusDomainX;
	HICON hIconStatusRdpOk;
	HICON hIconStatusRdpX;
	HICON hIconStatusFileOk;
	HICON hIconStatusFileX;

	//	systray icon
	HICON hIconTrayRunning;
	HICON hIconTrayRun;
	HICON hIconTrayStopped;


}STSTATUSICON, *LPSTSTATUSICON;

typedef struct tagStatusCache
{
	BOOL  bCoreFwStartCached;
	BOOL  bCoreFwStart;

	BOOL  bStartProtectCached;
	TCHAR szProtectChecksum[ MAX_PATH ];

	BOOL  bCountInfoCached;
	TCHAR szCountInfoChecksum[ MAX_PATH ];

}STSTATUSCACHE, *LPSTSTATUSCACHE;


/////////////////////////////////////////////////////////////////////////////
// CTabSetting dialog

class CTabSetting :
	public CDialog,
	public CTabBaseInfo
{
// Construction
public:
	CTabSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabSetting();


	VOID StartFirewall();
	VOID StopFirewall();


	static unsigned __stdcall _threadDisplayCoreStatus( PVOID arglist );
	VOID DisplayCoreStatusProc( BOOL bWarningIsapi, BOOL bFlashIcon = TRUE );
	VOID DisplayCoreStatusSub( BOOL bFlashIcon );
	VOID DisplayCtrlButtonsStatus();
	VOID DisplayCoreCountInfo();

	BOOL IsFwStart();
	BOOL IsCoreRunning();
	BOOL IsCoreStart();

	LONGLONG GetCoreBlockCount( DWORD dwCountType );
	UINT GetCoreCfgCount( DWORD dwCountType );
	BOOL LoadConfig();
	BOOL SaveConfig();

	VOID FreeAllIconsResources();


	CHyperLink	m_LinkAboutMore;

	CHyperLink	m_CtlLinkFirewall;
	CHyperLink	m_CtlLinkFile;
	CHyperLink	m_CtlLinkFileSet;
	CHyperLink	m_CtlLinkDomain;
	CHyperLink	m_CtlLinkDomainSet;
	CHyperLink	m_CtlLinkRdp;
	CHyperLink	m_CtlLinkRdpSet;

	
	STVWFIREWALLCONFIG m_stConfig;
	STSTATUSCACHE m_stStatusCache;


	TCHAR m_szAppName[ MAX_PATH ];
	TCHAR m_szCaption[ MAX_PATH ];
	TCHAR m_szString[ MAX_PATH ];

	HWND m_hParentWind;
	CXingBalloon * m_pxballoon;

	STSTATUSICON m_stStatusIcon;

	CDeThreadSleep	m_cThSleepDisplayCoreStatus;

// Dialog Data
	//{{AFX_DATA(CTabSetting)
	enum { IDD = IDD_TAB_SETTING };
	CStatic	m_stcCtlLinkRdpSp;
	CStatic	m_stcCtlLinkFileSp;
	CStatic	m_stcCtlLinkDomainSp;
	CStatic	m_stcPFileBlockCount;
	CStatic	m_stcStatusFile;
	CStatic	m_stcStatusFileTxt;
	CStatic	m_stcPRdpBlockCount;
	CStatic	m_stcStatusRdpTxt;
	CStatic	m_stcStatusRdp;
	CStatic	m_stcPDomainBlockCount;
	CStatic	m_stcStatusDomainTxt;
	CStatic	m_stcStatusDomain;
	CButton	m_btnStop;
	CButton	m_btnStart;
	CStatic	m_stcFirewallStatus;
	CStatic	m_stcStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSetting)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabSetting)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual LRESULT OnCtrlButton( WPARAM wParam, LPARAM lParam );
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSETTING_H__CE615BEE_30C5_4E76_8A74_4A01DDD49CA2__INCLUDED_)
