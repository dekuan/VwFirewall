// VwFirewallCfgDlg.h : header file
//

#if !defined(AFX_DEINFOMFILTERCFGDLG_H__ABEF054F_7127_4C12_A093_C19BA6CA6FEB__INCLUDED_)
#define AFX_DEINFOMFILTERCFGDLG_H__ABEF054F_7127_4C12_A093_C19BA6CA6FEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HyperLink.h"
#include "XingBalloon.h"
#include "DeCaptionStatic.h"
#include "DeThreadSleep.h"

#include "ModuleInfo.h"

#include "BtnSt.h"
#include "TabAbout.h"
#include "TabSecurityCfg.h"
#include "TabLicense.h"
#include "TabUpdate.h"
#include "TabSetting.h"
//#include "TabFile.h"
//#include "TabDomain.h"
//#include "TabRdp.h"



//	Timer
#define CVWFIREWALLCFGDLG_TIMER_AUTOHIDDEN	1000

#define CVWFIREWALLCFGDLG_BUTTON_COUNT		4

typedef struct tagVwFirewallCfgTree
{
	LPTSTR  lpszText;
	LPTSTR  lpszCaption;
	INT     nImage;
	LPARAM  lParam;
	INT     nDlgId;

}STVWFIREWALLCFGTREE, *LPSTVWFIREWALLCFGTREE;


static STVWFIREWALLCFGTREE g_stMenuTree[] =
{
	{ "VirtualWall", "VirtualWall", 0, 0, IDD_TAB_ABOUT },
	{ "许可", "许可", 1, 1, IDD_TAB_LICENSE },
	{ "升级", "升级", 4, 2, IDD_TAB_UPDATE },
	{ "设置", "设置", 2, 3, IDD_TAB_SETTING },
	{ "域名白名单", "设置 > 域名白名单", 7, 100, IDD_TAB_DOMAIN },
	{ "远程桌面安全", "设置 > 远程桌面安全", 8, 101, IDD_TAB_RDP },
};
static g_nMenuTreeCount	= sizeof(g_stMenuTree)/sizeof(g_stMenuTree[0]);




/////////////////////////////////////////////////////////////////////////////
// CVwFirewallCfgDlg dialog

class CVwFirewallCfgDlg :
	public CDialog
{
// Construction
public:
	CVwFirewallCfgDlg(CWnd* pParent = NULL);	// standard constructor
	~CVwFirewallCfgDlg();

	CHyperLink m_LinkVw;
	CHyperLink m_LinkDiscuss;
	CHyperLink m_LinkHelpDoc;
	CHyperLink m_LinkAd;
	CHyperLink m_LinkSplit1;
	CHyperLink m_LinkSplit2;

	CDeThreadSleep	m_cThreadAd;

// Dialog Data
	//{{AFX_DATA(CVwFirewallCfgDlg)
	enum { IDD = IDD_VWFIREWALLCFG_DIALOG };
	CStatic	m_stcTitImage;
	CButtonST	m_btnUpdate;
	CButtonST	m_btnLicense;
	CButtonST	m_btnStatus;
	CButtonST	m_btnSecurityCfg;
	CTreeCtrl	m_treeMenu;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVwFirewallCfgDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

private:

	VOID InitAllButtons();
	VOID OnNavButtonClick( DWORD dwButton, BOOL bInitPos = FALSE );
	VOID OnMyPaint();

	static unsigned __stdcall _threadVwStaticAd( PVOID arglist );
	VOID VwStaticAdProc();

	VOID ShowTabWindow( UINT uIndex );
	BOOL IsStartAuto( BOOL bCheckExist = FALSE );
	VOID SetStartAuto( BOOL bStartAuto );

	TCHAR m_szAppName[ MAX_PATH ];
	TCHAR m_szAppCaption[ MAX_PATH ];
	TCHAR m_szStartupInfo[ MAX_PATH ];
	TCHAR m_szVersion[ 64 ];

	DWORD		m_dwDlgAutoHideCount;
	CRect		m_rcTableRect;
	DWORD		m_dwBottomAdLineHeight;
	DWORD		m_dwSelButton;
	DWORD		m_dwButtonTopOffset;
	DWORD		m_dwButtonWidth;
	DWORD		m_dwButtonHeight;
	CButtonST     * m_pArrButtons[ CVWFIREWALLCFGDLG_BUTTON_COUNT ];
	DWORD           m_dwArrButtonsID[ CVWFIREWALLCFGDLG_BUTTON_COUNT ];

	COLORREF	m_clrBtnText;
	COLORREF	m_clrGray;
	COLORREF	m_clrBack;
	COLORREF	m_clrHover;
	COLORREF	m_clrDarkRed;

	DWORD		m_dwLastButtonFace;
	//CDeCaptionStatic m_stcCap;
	CString		m_csCapText;

	CImageList m_imgListTree;
	HTREEITEM m_hTreeSetting;

	//	气球
	CXingBalloon	m_xballoon;
	CModuleInfo	m_cModuleInfo;

	//	tables
	CTabAbout	m_cTabAbout;
	CTabSecurityCfg	m_cTabSecurityCfg;
	CTabLicense	m_cTabLicense;
	CTabUpdate	m_cTabUpdate;
	CTabSetting	m_cTabSetting;
	//CTabFile	m_cTabFile;
	//CTabDomain	m_cTabDomain;
	//CTabRdp		m_cTabRdp;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVwFirewallCfgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMyClose( WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);	//系统托盘工作函数
	virtual LRESULT OnOpenHtmlHelp( WPARAM wParam, LPARAM lParam );
	afx_msg void OnMenuSystrayExit();
	afx_msg void OnMenuSystrayDehomepage();
	afx_msg void OnMenuSystrayOpen();
	afx_msg void OnMenuSystrayOpenworkingdir();
	afx_msg void OnMenuSystrayStartauto();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMenuSystrayOpenhelpdoc();
	afx_msg void OnButtonStatus();
	afx_msg void OnButtonLicense();
	afx_msg void OnButtonUpdate();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnButtonSecuritycfg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEINFOMFILTERCFGDLG_H__ABEF054F_7127_4C12_A093_C19BA6CA6FEB__INCLUDED_)
