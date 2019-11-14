// VwOnlineLicenseCheck.h: interface for the CVwOnlineLicenseCheck class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWONLINELICENSECHECK_HEADER__
#define __VWONLINELICENSECHECK_HEADER__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include "VwConstCommon.h"
#include "VwEnCodeString.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 *	服务器返回数据格式
 */
typedef struct tagVwOnlineLicenseCheckData
{
	tagVwOnlineLicenseCheckData()
	{
		memset( this, 0, sizeof(tagVwOnlineLicenseCheckData) );
	}

	INT	nResult;			//	检测结果，1、0 <result>1</result>
	TCHAR	szVwPrType[ 32 ];		//	产品类型：<vwprtype>pslnormal</vwprtype>
	TCHAR   szVwPrTypeChecksum[ 64 ];	//	产品类型校验值： <vwprtypecs>a76e024d907205c9199184d63763475a</vwprtypecs>
	TCHAR	szVwCdate[ 32 ];		//	注册日期，<vwcdate>2005-9-1</vwcdate>
	INT	nVwExpireType;			//	失效类型，<vwexpiretype>1</vwexpiretype>
	INT	nVwLeftDays;			//	剩余天数，<vwleftdays>340</vwleftdays>
	TCHAR	szVwExpireDate[ 32 ];		//	过期时间，<vwexpire>2009-12-8</vwexpire>

}STVWONLINELICENSECHECKDATA, *LPSTVWONLINELICENSECHECKDATA;

typedef struct tagVwOnlineLicenseCheckServer
{
	TCHAR * pszUrlHelloServer1;
	DWORD   dwSizeOfUrlHelloServer1;

	TCHAR * pszUrlHelloServer2;
	DWORD   dwSizeOfUrlHelloServer2;

	TCHAR * pszUrlOnlineLicenseCheckV3;
	DWORD   dwSizeOfUrlOnlineLicenseCheckV3;

}STVWONLINELICENSECHECKSERVER, *LPSTVWONLINELICENSECHECKSERVER;

//	Array of servers
static STVWONLINELICENSECHECKSERVER g_ArrVwOnlineLicenseCheckServer[] =
{
	{
		g_szUrlHelloServer1, sizeof(g_szUrlHelloServer1),
		g_szUrlHelloServer2, sizeof(g_szUrlHelloServer2),
		g_szUrlOnlineLicenseCheckV3, sizeof(g_szUrlOnlineLicenseCheckV3)
	},
	{
		g_szUrlHelloServer1_check2, sizeof(g_szUrlHelloServer1_check2),
		g_szUrlHelloServer2_check2, sizeof(g_szUrlHelloServer2_check2),
		g_szUrlOnlineLicenseCheckV3_check2, sizeof(g_szUrlOnlineLicenseCheckV3_check2)
	},
	{
		g_szUrlHelloServer1_check3, sizeof(g_szUrlHelloServer1_check3),
		g_szUrlHelloServer2_check3, sizeof(g_szUrlHelloServer2_check3),
		g_szUrlOnlineLicenseCheckV4_check3, sizeof(g_szUrlOnlineLicenseCheckV4_check3)
	}
};
static CONST DWORD g_dwArrVwOnlineLicenseCheckServerCount = sizeof(g_ArrVwOnlineLicenseCheckServer)/sizeof(g_ArrVwOnlineLicenseCheckServer[0]);



/**
 *	class of 
 */
class CVwOnlineLicenseCheck
{
public:
	CVwOnlineLicenseCheck();
	virtual ~CVwOnlineLicenseCheck();

	VOID SetServerNumber( DWORD dwServerNumber );
	VOID SetTryTimes( DWORD dwTryTimes );

	BOOL PostVwOnlineLicenseCheckWithServer( LPCTSTR lpcszParameter, BOOL * pbEnterpriseVer, BOOL *pbNeedUpdate );
	BOOL PostVwOnlineLicenseCheckWithServer( LPCTSTR lpcszParameter, STVWONLINELICENSECHECKDATA * lpstData );
	BOOL PostVwStartupInfoToServer( LPTSTR lpszFilterFilePath );


private:
	BOOL IsRealVirtualWallServer();

	DWORD m_dwServerNumber;
	DWORD m_dwTryTimes;

	TCHAR m_szDecResult0[ MAX_PATH ];
	TCHAR m_szDecResult1[ MAX_PATH ];
	TCHAR m_szPostStartupInfoFmt[ MAX_PATH ];

};

#endif // __VWONLINELICENSECHECK_HEADER__
