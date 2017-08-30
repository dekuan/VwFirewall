// TabSecurityCfgBase.h: interface for the CTabSecurityCfgBase class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __TABSECURITYCFGBASE_HEADER__
#define __TABSECURITYCFGBASE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BtnSt.h"
#include "ParseCfgData.h"
#include "QueryAclsInfo.h"
#include "VwFirewallConfigFile.h"
#include "DeThreadSleep.h"


//
//	...
//
#define RECURSE_REMOVE_ALL		0
#define RECURSE_REMOVE_FILE		1
#define RECURSE_REMOVE_DIR		2


/**
 *	icon types
 */
enum
{
	CTABSECURITYCFG_ICONTYPE_INFO	= 0x1000,
	CTABSECURITYCFG_ICONTYPE_ALERT,
	CTABSECURITYCFG_ICONTYPE_OK,
	CTABSECURITYCFG_ICONTYPE_X,
};



/**
 *	struct ...
 */
typedef struct tagTSCBLocalUser
{
	TCHAR szUser[ 128 ];

}STTSCBLOCALUSER, *LPSTTSCBLOCALUSER;

typedef struct tagTSCBLocalGroup
{
	TCHAR szGroupName[ 128 ];

	vector<STTSCBLOCALUSER> vcUsers;
	vector<STTSCBLOCALUSER>::iterator it;

}STTSCBLOCALGROUP, *LPSTTSCBLOCALGROUP;


/**
 *	class of CTabSecurityCfgBase
 */
class CTabSecurityCfgBase
{
public:
	CTabSecurityCfgBase();
	virtual ~CTabSecurityCfgBase();

public:
	BOOL Regsvr32_Wshom_ocx( BOOL bReg );
	BOOL IsNeedToFix( STPARSECFGDATAGROUP * pstAclsDataGroup );

	VOID SetFixAclsProblemsStatus( BOOL bContinue );
	BOOL FixAclsProblemsInGroup( STPARSECFGDATAGROUP * pstAclsDataGroup );
	BOOL RecoverFixingInGroup( STPARSECFGDATAGROUP * pstAclsDataGroup );

	VOID SetFlashFixingIcon( CListCtrl * plistCtrl, LONG lnListRowIndex, LONG lnListColumnIndex );
	BOOL GetServiceStatusText( LPCTSTR lpcszSvcName, LPTSTR lpszSvcDesc, DWORD dwSize );

	BOOL InitLocalAccounts();
	BOOL IsLocalAccountExist( LPCTSTR lpcszUser );

	BOOL RecurseRemoveDirectory( LPCTSTR lpcszDir, INT nType = RECURSE_REMOVE_ALL );
	BOOL RecurseIsFileExists( LPCTSTR lpcszFilename );
	BOOL SpellRegisterFile( LPCTSTR lpcszDestFile, LPCTSTR lpcszSrcFile );

private:
	BOOL IsPeFile( LPCTSTR lpcszFilename );
	BOOL CreateNewTempFile( LPCTSTR lpcszFilename );
	BOOL IsRegExist( STPARSECFGDATASET* pstAclsDataSet );
	BOOL RegDelete( STPARSECFGDATASET * pstAclsDataSet );
	BOOL RegAdd( STPARSECFGDATASET * pstAclsDataSet );

	BOOL RenameFileOrDirectory( LPCTSTR lpcszFrom, LPCTSTR lpcszTo );
	

	static unsigned __stdcall _threadFlashFixingIcon( PVOID arglist );
	VOID FlashFixingIconProc();

	CDeThreadSleep m_cThSleepFlashFixingIcon;
	LONG m_lnListRowIndex;
	LONG m_lnListColumnIndex;
	CListCtrl * m_plistCtrl;


public:
	HIMAGELIST m_hImageListGroup;

private:
	CQueryAclsInfo m_cQueryAclsInfo;
	CVwFirewallConfigFile m_cVwFirewallConfigFile;
	
	//	for FixAclsProblemsInGroup()
	BOOL m_bFixAclsProblemsContinue;

	BOOL m_bInitLocalAccountsSucc;
	vector<STTSCBLOCALGROUP> m_vcLocalGroups;
	vector<STTSCBLOCALGROUP>::iterator m_itLG;

};



#endif // __TABSECURITYCFGBASE_HEADER__
