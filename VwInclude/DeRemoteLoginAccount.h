// DeRemoteLoginAccount.h: interface for the CDeRemoteLoginAccount class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEREMOTELOGINACCOUNT_HEADER__
#define __DEREMOTELOGINACCOUNT_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
using namespace std;

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "ModuleInfoDeRemote.h"

#define DEREMOTELOGINACCOUNT_FLAG_ADDNEW	999999999

#define DEREMOTELOGINACCOUNT_DEFAULT_LANG	_T("en")
#define DEREMOTELOGINACCOUNT_DEFAULT_LANGNAME	_T("English")


//	PasswordKey = De79Xing09Pwd03
static TCHAR g_szDeRemoteLoginAccount_PasswordKey[] =
{
	-69, -102, -56, -58, -89, -106, -111, -104, -49, -58, -81, -120, -101, -49, -52, 0
};


/**
 *	帐号信息结构体
 */
typedef struct tagDeLoginAccountInfo
{
	tagDeLoginAccountInfo()
	{
		memset( this, 0, sizeof(tagDeLoginAccountInfo) );
	}
	DWORD dwStatus;
	TCHAR szUserName[ 64 ];
	TCHAR szPassword[ 64 ];		//	md5 后的密码
	TCHAR szPasswordNew[ 64 ];	//	用户设置的新密码，明文
	TCHAR szLang[ 32 ];
	TCHAR szLangName[ 64 ];

}STDELOGINACCOUNTINFO, *PSTDELOGINACCOUNTINFO;

/**
 *	语言配置信息
 */
typedef struct tagDeLangConfig
{
	tagDeLangConfig()
	{
		memset( this, 0, sizeof(tagDeLangConfig) );
	}
	TCHAR szLang[ 32 ];
	TCHAR szLangName[ 64 ];

}STDELANGCONFIG, *LPSTDELANGCONFIG;




/**
 *	class of CDeRemoteLoginAccount
 */
class CDeRemoteLoginAccount :
	public CModuleInfoDeRemote
{
public:
	CDeRemoteLoginAccount();
	virtual ~CDeRemoteLoginAccount();

	DWORD GetAccountCount();
	BOOL  LoadAll( vector<STDELOGINACCOUNTINFO> & pvcAccount );
	BOOL  SaveAccount( STDELOGINACCOUNTINFO * pstAccount );
	BOOL  DeleteAccount( STDELOGINACCOUNTINFO * pstAccount );
	BOOL  IsValidUsername( LPCTSTR lpcszUsername );
	BOOL  IsValidPassword( LPCTSTR lpcszPassword );
	BOOL  GetEncodePassword( LPCTSTR lpcszPassword, LPTSTR lpszPwdEncode, DWORD dwSize );

private:
	BOOL  LoadLangSupports();
	BOOL  GetLangName( LPCTSTR lpcszLang, LPTSTR lpszLangName, DWORD dwSize );

public:

	BOOL  m_bInitSucc;

	vector<STINISECTIONLINE> m_vcSecLine;
	vector<STDELOGINACCOUNTINFO> m_vcAllAccount;
	vector<STDELANGCONFIG> m_vcSupportedLang;

private:
	TCHAR m_szPasswordKey[ 64 ];
};


#endif // __DEREMOTELOGINACCOUNT_HEADER__
