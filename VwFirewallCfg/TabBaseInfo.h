// TabBaseInfo.h: interface for the CTabBaseInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABBASEINFO_H__72D2837F_BD30_4874_8408_81DFC1842DA7__INCLUDED_)
#define AFX_TABBASEINFO_H__72D2837F_BD30_4874_8408_81DFC1842DA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ModuleInfo.h"
#include "VwFirewallConfigFile.h"

#include "BtnSt.h"
#include "HyperLink.h"
#include "XingBalloon.h"
#include "VwLicence.h"

#include "Winsvc.h"
#include "winioctl.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "DeLibDrv.h"
#pragma comment( lib, "DeLibDrv.lib" )


/**
 *	class of CTabBaseInfo
 */
class CTabBaseInfo :
	public CVwFirewallConfigFile,
	public CVwLicence
{
public:
	CTabBaseInfo();
	virtual ~CTabBaseInfo();

	BOOL GetHintWithWordCount( LPCTSTR lpcszHintFmt, DWORD dwWordCount, LPTSTR lpszBuffer, DWORD dwSize );
	BOOL IsCheckboxChanged( NM_LISTVIEW * pNMListView );
	BOOL NotifyDriverToLoadNewConfig();
	BOOL MakeKeyFile();

	BOOL OutputDebugStringFormat( LPCTSTR lpszFmt, ... );

public:
	STVWFIREWALLCONFIG m_stCfg;
	TCHAR m_szPrNameVwFirewall[ MAX_PATH ];

	BOOL m_bReged;
	BOOL m_bValidPrType;

	DWORD m_dwFreeVerDomainLimit;
	TCHAR m_szHintFmt[ MAX_PATH ];
};

#endif // !defined(AFX_TABBASEINFO_H__72D2837F_BD30_4874_8408_81DFC1842DA7__INCLUDED_)
