// VwLicence.cpp: implementation of the CVwLicence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwLicence.h"
#include "VwEnCodeString.h"
#include "HardwareInfo.h"

#include "md5.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "Shlwapi.h"
#pragma comment( lib, "Shlwapi.lib" )


#ifndef FD_SET
#include "Winsock2.h"
#endif
#pragma comment( lib, "Ws2_32.lib" )


/**
 *	加密字符串
 */
//	"%s-vwprt-%s-%s"
static TCHAR g_szCVwLicence_PrTypeFmt[] = { -38, -116, -46, -119, -120, -113, -115, -117, -46, -38, -116, -46, -38, -116, 0 };
//	"%s-vwprt-%s-%s-%s"
static TCHAR g_szCVwLicence_PrTypeFmtV4[] = { -38, -116, -46, -119, -120, -113, -115, -117, -46, -38, -116, -46, -38, -116, -46, -38, -116, 0 };


//	"comadv"
static TCHAR g_szCVwLicence_PrType_comadv[] = { -100, -112, -110, -98, -101, -119, 0 };

//	"comnormal"
static TCHAR g_szCVwLicence_PrType_comnormal[] = { -100, -112, -110, -111, -112, -115, -110, -98, -109, 0 };

//	"pslunlmt"
static TCHAR g_szCVwLicence_PrType_pslunlmt[] = { -113, -116, -109, -118, -111, -109, -110, -117, 0 };

//	"psladv"
static TCHAR g_szCVwLicence_PrType_psladv[] = { -113, -116, -109, -98, -101, -119, 0 };

//	"pslnormal"
static TCHAR g_szCVwLicence_PrType_pslnormal[] = { -113, -116, -109, -111, -112, -115, -110, -98, -109, 0 };



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwLicence::CVwLicence()
{
	memset( m_szPrNameVwInfoMonitor, 0, sizeof(m_szPrNameVwInfoMonitor) );
	memcpy( m_szPrNameVwInfoMonitor, g_szPrName_VwInfoMonitor, sizeof(g_szPrName_VwInfoMonitor) );
	delib_xorenc( m_szPrNameVwInfoMonitor );

	memset( m_szPrNameVwPanDomain, 0, sizeof(m_szPrNameVwPanDomain) );
	memcpy( m_szPrNameVwPanDomain, g_szPrName_VwPanDomain, sizeof(g_szPrName_VwPanDomain) );
	delib_xorenc( m_szPrNameVwPanDomain );

	memset( m_szPrNameVwCmAntiLeech, 0, sizeof(m_szPrNameVwCmAntiLeech) );
	memcpy( m_szPrNameVwCmAntiLeech, g_szPrName_VwCmAntiLeech, sizeof(g_szPrName_VwCmAntiLeech) );
	delib_xorenc( m_szPrNameVwCmAntiLeech );

	memset( m_szPrNameVwFirewall, 0, sizeof(m_szPrNameVwFirewall) );
	memcpy( m_szPrNameVwFirewall, g_szPrName_VwFirewall, sizeof(g_szPrName_VwFirewall) );
	delib_xorenc( m_szPrNameVwFirewall );

	memset( m_szPrNameRefererGuard, 0, sizeof(m_szPrNameRefererGuard) );
	memcpy( m_szPrNameRefererGuard, g_szPrName_RefererGuard, sizeof(g_szPrName_RefererGuard) );
	delib_xorenc( m_szPrNameRefererGuard );

	memset( m_szDecCVwLicence_PrTypeFmt, 0, sizeof(m_szDecCVwLicence_PrTypeFmt) );
	memcpy( m_szDecCVwLicence_PrTypeFmt, g_szCVwLicence_PrTypeFmt, sizeof(g_szCVwLicence_PrTypeFmt) );
	delib_xorenc( m_szDecCVwLicence_PrTypeFmt );

	memset( m_szDecCVwLicence_PrTypeFmtV4, 0, sizeof(m_szDecCVwLicence_PrTypeFmtV4) );
	memcpy( m_szDecCVwLicence_PrTypeFmtV4, g_szCVwLicence_PrTypeFmtV4, sizeof(g_szCVwLicence_PrTypeFmtV4) );
	delib_xorenc( m_szDecCVwLicence_PrTypeFmtV4 );

	//
	//	for prtype
	//
	//	"comadv"
	memset( m_szDecCVwLicence_PrType_comadv, 0, sizeof(m_szDecCVwLicence_PrType_comadv) );
	memcpy( m_szDecCVwLicence_PrType_comadv, g_szCVwLicence_PrType_comadv, sizeof(g_szCVwLicence_PrType_comadv) );
	delib_xorenc( m_szDecCVwLicence_PrType_comadv );

	memset( m_szDecCVwLicence_PrType_comnormal, 0, sizeof(m_szDecCVwLicence_PrType_comnormal) );
	memcpy( m_szDecCVwLicence_PrType_comnormal, g_szCVwLicence_PrType_comnormal, sizeof(g_szCVwLicence_PrType_comnormal) );
	delib_xorenc( m_szDecCVwLicence_PrType_comnormal );

	memset( m_szDecCVwLicence_PrType_pslunlmt, 0, sizeof(m_szDecCVwLicence_PrType_pslunlmt) );
	memcpy( m_szDecCVwLicence_PrType_pslunlmt, g_szCVwLicence_PrType_pslunlmt, sizeof(g_szCVwLicence_PrType_pslunlmt) );
	delib_xorenc( m_szDecCVwLicence_PrType_pslunlmt );

	memset( m_szDecCVwLicence_PrType_psladv, 0, sizeof(m_szDecCVwLicence_PrType_psladv) );
	memcpy( m_szDecCVwLicence_PrType_psladv, g_szCVwLicence_PrType_psladv, sizeof(g_szCVwLicence_PrType_psladv) );
	delib_xorenc( m_szDecCVwLicence_PrType_psladv );

	memset( m_szDecCVwLicence_PrType_pslnormal, 0, sizeof(m_szDecCVwLicence_PrType_pslnormal) );
	memcpy( m_szDecCVwLicence_PrType_pslnormal, g_szCVwLicence_PrType_pslnormal, sizeof(g_szCVwLicence_PrType_pslnormal) );
	delib_xorenc( m_szDecCVwLicence_PrType_pslnormal );

}
CVwLicence::~CVwLicence()
{
}

/**
 *	@ public
 *	检查用户的许可 KEY
 */
BOOL CVwLicence::IsValidLicense( LPCTSTR lpcszPrName, LPCTSTR lpcszRegIp, LPCTSTR lpcszRegKey )
{
	if ( NULL == lpcszPrName || NULL == lpcszRegIp || NULL == lpcszRegKey )
	{
		return FALSE;
	}
	if ( 0 == _tcslen( lpcszPrName ) || 0 == _tcslen( lpcszRegIp ) || 0 == _tcslen( lpcszRegKey ) )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;

	if ( IsLocalAddr( lpcszRegIp ) )
	{
		if ( CheckDllRegKey( lpcszPrName, lpcszRegIp, lpcszRegKey ) )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

/**
 *	@ public
 *	检查是否是有效的 PrType 的 checksum 值
 */
BOOL CVwLicence::IsValidPrTypeChecksum( LPCTSTR lpcszPrTypeChecksum, LPCTSTR lpcszRegPrType, LPCTSTR lpcszRegIp, LPCTSTR lpcszRegKey )
{
	if ( NULL == lpcszPrTypeChecksum || 0 == _tcslen(lpcszPrTypeChecksum) )
	{
		return FALSE;
	}
	if ( NULL == lpcszRegPrType || NULL == lpcszRegIp || NULL == lpcszRegKey )
	{
		return FALSE;
	}
	if ( 0 == _tcslen(lpcszRegPrType) || 0 == _tcslen(lpcszRegIp) || 0 == _tcslen(lpcszRegKey) )
	{
		return FALSE;
	}

	BOOL  bRet			= FALSE;
	TCHAR szInput[ MAX_PATH ]	= {0};
	TCHAR szMd5[ MAX_PATH ]		= {0};

	//	"%s-vwprt-%s-%s"
	_sntprintf( szInput, sizeof(szInput)-sizeof(TCHAR), m_szDecCVwLicence_PrTypeFmt, lpcszRegPrType, lpcszRegIp, lpcszRegKey );
	_tcslwr( szInput );
	GetStringMd5( szInput, szMd5, sizeof(szMd5) );

	if ( 0 == _tcsicmp( lpcszPrTypeChecksum, szMd5 ) )
	{
		bRet = TRUE;
	}

	return bRet;
}
BOOL CVwLicence::IsValidPrTypeChecksumV4( LPCTSTR lpcszPrTypeChecksum, LPCTSTR lpcszRegPrType, LPCTSTR lpcszRegIp, LPCTSTR lpcszRegKey, LPCTSTR lpcszExpireDate )
{
	if ( NULL == lpcszPrTypeChecksum || 0 == _tcslen(lpcszPrTypeChecksum) )
	{
		return FALSE;
	}
	if ( NULL == lpcszRegPrType || NULL == lpcszRegIp || NULL == lpcszRegKey )
	{
		return FALSE;
	}
	if ( 0 == _tcslen(lpcszRegPrType) || 0 == _tcslen(lpcszRegIp) || 0 == _tcslen(lpcszRegKey) )
	{
		return FALSE;
	}
	
	BOOL  bRet			= FALSE;
	TCHAR szInput[ MAX_PATH ]	= {0};
	TCHAR szMd5[ MAX_PATH ]		= {0};

	//	"%s-vwprt-%s-%s-%s"
	_sntprintf
	(
		szInput, sizeof(szInput)/sizeof(TCHAR)-1,
		m_szDecCVwLicence_PrTypeFmtV4,
		lpcszRegPrType,
		lpcszRegIp,
		lpcszRegKey,
		( lpcszExpireDate ? lpcszExpireDate : "" )
	);
	_tcslwr( szInput );
	GetStringMd5( szInput, szMd5, sizeof(szMd5) );

	if ( 0 == _tcsicmp( lpcszPrTypeChecksum, szMd5 ) )
	{
		bRet = TRUE;
	}
	
	return bRet;
}


BOOL CVwLicence::GetPrTypeNameByCode( LPCTSTR lpcszPrType, LPTSTR lpszPrTypeName, DWORD dwSize )
{
	//
	//	{ 0x0804, _T("Chinese-Simplified") },
	//
	//
	UINT i;
	LONG lnMachedIndex;
	LANGID lnLangID;

	if ( NULL == lpcszPrType || NULL == lpszPrTypeName || 0 == dwSize )
	{
		return FALSE;
	}

	lnMachedIndex = -1;
	for ( i = 0; i < g_nVwLicencePrTypeListCount; i ++ )
	{
		if ( 0 == _tcsicmp( g_ArrVwLicencePrTypeList[i].lpcszPrType, lpcszPrType ) )
		{
			lnMachedIndex = i;
			break;
		}
	}

	if ( -1 == lnMachedIndex )
	{
		//	未找到，则显示“免费评估版”
		lnMachedIndex = 0;
	}


	//UINT uAcp = GetACP();
	//lnLangID = GetSystemDefaultLangID();	//	GetUserDefaultLCID();
	lnLangID = GetUserDefaultUILanguage();
	if ( LANG_CHINESE == PRIMARYLANGID(lnLangID) && SUBLANG_CHINESE_SIMPLIFIED == SUBLANGID(lnLangID) )
	{
		//	chs
		_sntprintf( lpszPrTypeName, dwSize/sizeof(TCHAR)-1, _T("%s"), g_ArrVwLicencePrTypeList[ lnMachedIndex ].lpcszChs );
	}
	else if ( LANG_CHINESE == PRIMARYLANGID(lnLangID) && SUBLANG_CHINESE_TRADITIONAL == SUBLANGID(lnLangID) )
	{ 
		//	cht
		_sntprintf( lpszPrTypeName, dwSize/sizeof(TCHAR)-1, _T("%s"), g_ArrVwLicencePrTypeList[ lnMachedIndex ].lpcszEn );
	}
	else if ( LANG_ENGLISH == PRIMARYLANGID(lnLangID) )
	{
		//	eng
		_sntprintf( lpszPrTypeName, dwSize/sizeof(TCHAR)-1, _T("%s"), g_ArrVwLicencePrTypeList[ lnMachedIndex ].lpcszEn );
	}
	else
	{
		//	other
		_sntprintf( lpszPrTypeName, dwSize/sizeof(TCHAR)-1, _T("%s"), g_ArrVwLicencePrTypeList[ lnMachedIndex ].lpcszEn );
	}

/*
	if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMADV, lpcszPrType ) )
	{
		_sntprintf( lpszPrTypeName, dwSize-sizeof(TCHAR), "%s", "高级公司版" );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMNORMAL, lpcszPrType ) )
	{
		_sntprintf( lpszPrTypeName, dwSize-sizeof(TCHAR), "%s", "普通公司版" );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLUNLMT, lpcszPrType ) )
	{
		_sntprintf( lpszPrTypeName, dwSize-sizeof(TCHAR), "%s", "终身个人版" );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLADV, lpcszPrType ) )
	{
		_sntprintf( lpszPrTypeName, dwSize-sizeof(TCHAR), "%s", "高级个人版" );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLNORMAL, lpcszPrType ) )
	{
		_sntprintf( lpszPrTypeName, dwSize-sizeof(TCHAR), "%s", "普通个人版" );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLRENEW, lpcszPrType ) )
	{
		_sntprintf( lpszPrTypeName, dwSize-sizeof(TCHAR), "%s", "续费" );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_UPGRADE, lpcszPrType ) )
	{
		_sntprintf( lpszPrTypeName, dwSize-sizeof(TCHAR), "%s", "升级" );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_REMOTEHELP, lpcszPrType ) )
	{
		_sntprintf( lpszPrTypeName, dwSize-sizeof(TCHAR), "%s", "远程协助服务" );
	}
	else
	{
		_sntprintf( lpszPrTypeName, dwSize-sizeof(TCHAR), "%s", "免费评估版" );
	}
*/
	return TRUE;
}

/**
 *	@ public
 *	获取硬件 ID
 */
BOOL CVwLicence::GetComputerHardwareID( LPTSTR lpszHID, DWORD dwSize, BOOL bCreateWithDiskSN /*=TRUE*/ )
{
	//
	//	lpszHID	- [out] 返回加密因子，32 位字符 MD5
	//	dwSize	- [in]  返回缓冲区的大小，一般为一个大于 32 的缓冲区
	//	RETURN
	//		例如："genuineintel-6-afe9fbff000006d82c04307d00000000-e8309df3"
	//	算法：
	//		md5( m_szKey + NetworkCard Mac + DiskSerial )
	//

	CHardwareInfo cHardwareInfo;
	TCHAR szHDiskSN[ 64 ]	= {0};
	TCHAR szCPUID[ 64 ]	= {0};
	//TCHAR szSrc[ 128 ];


	//	获取 cpuid
	cHardwareInfo.GetCpuId( szCPUID, sizeof(szCPUID) );
	
	//	获取硬盘序列号
	if ( bCreateWithDiskSN )
	{
		cHardwareInfo.GetHDiskSerialNumber( szHDiskSN, sizeof(szHDiskSN) );
	}
	else
	{
		//	由于权限的问题，不能使用硬盘序列号作为参数来生成序列号
		memset( szHDiskSN, 0, sizeof(szHDiskSN) );
	}

	//	加密源文
	//	key + cpuid + HDiskSN
	_sntprintf( lpszHID, dwSize-sizeof(TCHAR), "%s-%s", szCPUID, szHDiskSN );
	_tcslwr( lpszHID );

	return TRUE;

	//	..
	//return GetStringMd5( szSrc, lpszHID, dwSize, 0 );
}


//////////////////////////////////////////////////////////////////////////
//	Private


/**
 *	检查注册码的正确性
 */
BOOL CVwLicence::CheckDllRegKey( LPCTSTR lpcszPrName, LPCTSTR lpcszRegIp, LPCTSTR lpcszRegKey )
{
	//
	//	lpcszRegKey	- [in]  注册码
	//	lpcszRegIp	- [in]  IP 地址，也有可能是一个 HOST
	//
	//	RETURN		- [out] 如果验证通过则返回 TRUE，否则返回 FALSE
	//

	if ( NULL == lpcszPrName || NULL == lpcszRegKey || NULL == lpcszRegIp )
	{
		return FALSE;
	}
	if ( 0 == _tcslen( lpcszPrName ) || 0 == _tcslen( lpcszRegKey ) || 0 == _tcslen(lpcszRegIp) )
	{
		return FALSE;
	}

	BOOL bRet				= FALSE;
	TCHAR szKeyHeader[ MAX_PATH ]		= {0};
	TCHAR szScanfCode[ MAX_PATH ]		= {0};
	unsigned char uszDeCode[ MAX_PATH ]	= {0};;
	LPTSTR lpszDecode			= NULL;

	//////////////////////////////////////////////////
	//	BB29559E - 8E1BA9E9 - 91FEB553 - 3A5E0253
	TCHAR szInput[ MAX_PATH ]		= {0};
	TCHAR szMd5[ MAX_PATH ]			= {0};

	if ( GetScanfCode( lpcszRegKey, szScanfCode, sizeof(szScanfCode) ) )
	{
		//
		//	检查注册码是否正确
		//
		memset( szKeyHeader, 0, sizeof(szKeyHeader) );
		if ( 0 == _tcsicmp( m_szPrNameVwPanDomain, lpcszPrName ) )
		{
			//	for "vwpandomain" 新泛域名解析器
			memcpy( szKeyHeader, g_szKeyHeader_VwPanDmNew, sizeof(g_szKeyHeader_VwPanDmNew) );
		}
		else if ( 0 == _tcsicmp( m_szPrNameVwCmAntiLeech, lpcszPrName ) )
		{
			//	for "vwcmantileech" 流媒体防盗链专家
			memcpy( szKeyHeader, g_szKeyHeader_VwCmAntiLeech, sizeof(g_szKeyHeader_VwCmAntiLeech) );
		}
		else if ( 0 == _tcsicmp( m_szPrNameVwFirewall, lpcszPrName ) )
		{
			//	for "vwfirewall" 域名防火墙
			memcpy( szKeyHeader, g_szKeyHeader_VwFirewall, sizeof(g_szKeyHeader_VwFirewall) );
		}
		else if ( 0 == _tcsicmp( m_szPrNameRefererGuard, lpcszPrName ) )
		{
			//	for "refererguard" RefererGuard
			memcpy( szKeyHeader, g_szKeyHeader_RefererGuard, sizeof(g_szKeyHeader_RefererGuard) );
		}
		else if ( 0 == _tcsicmp( m_szPrNameVwInfoMonitor, lpcszPrName ) )
		{
			//	for "vwinfomonitor" 非法信息拦截专家
			memcpy( szKeyHeader, g_szKeyHeader_VwInfoMonitor, sizeof(g_szKeyHeader_VwInfoMonitor) );
		}
		else
		{
			//	for "vwantileechs" 防盗链专家
			memcpy( szKeyHeader, g_szKeyHeader, sizeof(g_szKeyHeader) );
		}
		delib_xorenc( szKeyHeader );

		_sntprintf( szInput, sizeof(szInput)-sizeof(TCHAR), "%s-%s", szKeyHeader, lpcszRegIp );
		GetStringMd5( szInput, szMd5, sizeof(szMd5) );

		if ( 0 == _tcsicmp( szMd5, szScanfCode ) )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

/**
 *	转换用户输入的注册码到 MD5 字符串
 */
BOOL CVwLicence::GetScanfCode( LPCTSTR lpcszRegKey, LPTSTR lpszScanfCode, DWORD dwSize )
{
	//
	//	lpcszRegKey		- [in]  用户注册码
	//	lpszScanfCode		- [out] MD5 字符串
	//	dwSize			- [in]  lpszScanfCode 缓冲区大小
	//	RETURN			- TRUE / FALSE
	//
	//	输入	BB29559E-8E1BA9E9-91FEB553-3A5E0253
	//	输出	BB29559E8E1BA9E991FEB5533A5E0253
	//

	BOOL bRet = FALSE;

	if ( lpcszRegKey && lpszScanfCode &&
		35 == _tcslen(lpcszRegKey) && dwSize > 32 )
	{
		bRet = TRUE;

		_sntprintf( lpszScanfCode, 8, "%.8s", lpcszRegKey );
		_sntprintf( lpszScanfCode+8, 8, "%.8s", lpcszRegKey+8+1 );
		_sntprintf( lpszScanfCode+16, 8, "%.8s", lpcszRegKey+16+2 );
		_sntprintf( lpszScanfCode+24, 8, "%.8s", lpcszRegKey+24+3 );
	}
	
	return bRet;
}

/**
 *	判断指定的一个 IP 地址是否使本地某块网卡上的 IP 地址
 */
BOOL CVwLicence::IsLocalAddr( LPCTSTR lpcszIpAddr )
{
	if ( NULL == lpcszIpAddr || 0 == _tcslen(lpcszIpAddr) )
	{
		return FALSE;
	}

	BOOL  bRet		= FALSE;
	WSADATA WSData;
	TCHAR szBuffer[128]	= {0};
	HOSTENT * pst		= NULL;
	TCHAR ** pplist		= NULL;
	TCHAR * pTemp		= NULL;
	INT i			= 0;
	
	__try
	{
		if ( 0 == WSAStartup( MAKEWORD(CVWLICENCE_VERSION_MAJOR,CVWLICENCE_VERSION_MINOR), &WSData ) )
		{
			if ( 0 == gethostname( szBuffer, sizeof(szBuffer) ) )
			{
				pst = gethostbyname(szBuffer);
				if ( pst )
				{
					pplist = pst->h_addr_list;
					while( pplist[i] )
					{
						pTemp = inet_ntoa( *((LPIN_ADDR)pplist[i]) );
						if ( pTemp )
						{
							_sntprintf( szBuffer, sizeof(szBuffer)-sizeof(TCHAR), "%s", pTemp );
							if ( 0 == _tcsicmp( szBuffer, lpcszIpAddr ) )
							{
								bRet = TRUE;
								break;
							}
						}
						i++;
					}
				}
			}
			// ..
			WSACleanup();
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		// 线程异常错误退出
	}
	
	// ..
	return bRet;
}


/**
 *	获取一个字符串的 MD5 值(32位字符串)
 */
BOOL CVwLicence::GetStringMd5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen /* = 0 */ )
{
	//
	//	lpszString	- [in]  待处理字符串
	//	lpszMd5		- [out] 返回 MD5 值字符串缓冲区
	//	dwSize		- [in]  返回 MD5 值字符串长度
	//	dwSpecStringLen	- [in]  指定处理 lpszString 从头开始的多少个字节
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpszString || NULL == lpszMd5 )
	{
		return FALSE;
	}
	if ( dwSize < 32 )
	{
		return FALSE;
	}
	
	MD5_CTX	m_md5;
	unsigned char szEncrypt[16];
	unsigned char c;
	INT i = 0;

	memset( szEncrypt, 0, sizeof(szEncrypt) );
	if ( dwSpecStringLen > 0 )
	{
		m_md5.MD5Update( (unsigned char *)lpszString, dwSpecStringLen );
	}
	else
	{
		m_md5.MD5Update( (unsigned char *)lpszString, _tcslen(lpszString) );
	}
	m_md5.MD5Final( szEncrypt );

	//	...
	for ( i = 0; i < 16; i++ )
	{
		c = szEncrypt[i] / 16;
		lpszMd5[i*2]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
		c = szEncrypt[i] % 16;
		lpszMd5[i*2+1]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
	}

	//	..
	lpszMd5[ min( 32, dwSize-sizeof(TCHAR) ) ] = 0;
	
	return TRUE;
}