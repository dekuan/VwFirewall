// VwOnlineLicenseCheck.cpp: implementation of the CVwOnlineLicenseCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwOnlineLicenseCheck.h"
#include "DeHttp.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



/**
 *	加密数据区
 */

//	"<result>0</result>"
static TCHAR g_szCVwOnlineLicenseCheck_result0[] =
{
	-61, -115, -102, -116, -118, -109, -117, -63, -49, -61, -48, -115, -102, -116, -118, -109, -117, -63, 0
};

//	"<result>1</result>"
static TCHAR g_szCVwOnlineLicenseCheck_result1[] =
{
	-61, -115, -102, -116, -118, -109, -117, -63, -50, -61, -48, -115, -102, -116, -118, -109, -117, -63, 0
};

//	"%s?pn=%s&ip=%s&ot=%s&dver=%s"
static TCHAR g_szCVwOnlineLicenseCheck_PostStartupInfoFmt[] =
{
	-38, -116, -64, -113, -111, -62, -38, -116, -39, -106, -113, -62, -38, -116, -39, -112, -117, -62, -38, -116, -39,
	-101, -119, -102, -115, -62, -38, -116, 0
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwOnlineLicenseCheck::CVwOnlineLicenseCheck()
{
	m_dwServerNumber	= 0;	//	默认是 服务器1
	m_dwTryTimes		= 3;	//	默认 3 次

	//
	//	解密关键数据
	//
	memset( m_szDecResult0, 0, sizeof(m_szDecResult0) );
	memcpy( m_szDecResult0, g_szCVwOnlineLicenseCheck_result0, min( sizeof(g_szCVwOnlineLicenseCheck_result0), sizeof(m_szDecResult0) ) );
	delib_xorenc( m_szDecResult0 );

	memset( m_szDecResult1, 0, sizeof(m_szDecResult1) );
	memcpy( m_szDecResult1, g_szCVwOnlineLicenseCheck_result1, min( sizeof(g_szCVwOnlineLicenseCheck_result1), sizeof(m_szDecResult1) ) );
	delib_xorenc( m_szDecResult1 );

	memset( m_szPostStartupInfoFmt, 0, sizeof(m_szPostStartupInfoFmt) );
	memcpy( m_szPostStartupInfoFmt, g_szCVwOnlineLicenseCheck_PostStartupInfoFmt, min( sizeof(g_szCVwOnlineLicenseCheck_PostStartupInfoFmt), sizeof(m_szPostStartupInfoFmt) ) );
	delib_xorenc( m_szPostStartupInfoFmt );
	
}
CVwOnlineLicenseCheck::~CVwOnlineLicenseCheck()
{
}

/**
 *	@ public
 *	设置服务器组
 */
VOID CVwOnlineLicenseCheck::SetServerNumber( DWORD dwServerNumber )
{
	if ( dwServerNumber >= 0 && dwServerNumber < g_dwArrVwOnlineLicenseCheckServerCount )
	{
		m_dwServerNumber = dwServerNumber;
	}
}

/**
 *	@ public
 *	设置连接服务器尝试次数
 */
VOID CVwOnlineLicenseCheck::SetTryTimes( DWORD dwTryTimes )
{
	m_dwTryTimes = dwTryTimes;
}

/**
 *	@ public
 *	向服务器查询更新信息
 */
BOOL CVwOnlineLicenseCheck::PostVwOnlineLicenseCheckWithServer( LPCTSTR lpcszParameter, BOOL * pbEnterpriseVer, BOOL *pbNeedUpdate )
{
	if ( NULL == lpcszParameter || NULL == pbEnterpriseVer )
	{
		return FALSE;
	}
	
	BOOL  bRet = FALSE;
	TCHAR szUrlOnlineLicenseCheck[ MAX_PATH ]	= {0};
	TCHAR szInfoUrl[ MAX_URL ]	= {0};
	TCHAR szResponse[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};
	UINT  i				= 0;
	CDeHttp cDeHttp;

	for ( i = 0; i < m_dwTryTimes; i ++ )
	{
		if ( IsRealVirtualWallServer() )
		{
			memcpy
			(
				szUrlOnlineLicenseCheck,
				g_ArrVwOnlineLicenseCheckServer[ m_dwServerNumber ].pszUrlOnlineLicenseCheckV3,
				min( g_ArrVwOnlineLicenseCheckServer[ m_dwServerNumber ].dwSizeOfUrlOnlineLicenseCheckV3, sizeof(szUrlOnlineLicenseCheck) )
			);
			delib_xorenc( szUrlOnlineLicenseCheck );
			
			_sntprintf( szInfoUrl, sizeof(szInfoUrl)-sizeof(TCHAR), "%s?%s", szUrlOnlineLicenseCheck, lpcszParameter );
			// ..
			memset( szResponse, 0, sizeof(szResponse) );
			if ( cDeHttp.GetResponse( szInfoUrl, szResponse, sizeof(szResponse), szError, 30*1000 ) )
			{
				if ( _tcsstr( szResponse, m_szDecResult1 ) )
				{
					bRet = TRUE;

					// 真正的企业版本
					*pbEnterpriseVer = TRUE;
				}
				else if ( _tcsstr( szResponse, m_szDecResult0 ) )
				{
					bRet = TRUE;

					// 不是企业版本，数据库内没有查询到
					*pbEnterpriseVer = FALSE;
				}
				else
				{
					// 如果未能正确返回结果，那么只能认为是 OK，因为怕是我们服务器或者网络问题耽误正版用户的使用
					*pbEnterpriseVer = TRUE;
				}

				//	成功从服务器获取信息
				break;				
			}
		}
		else
		{
			Sleep( 30*1000 );
		}
	}
	// ..
	return bRet;
}
BOOL CVwOnlineLicenseCheck::PostVwOnlineLicenseCheckWithServer( LPCTSTR lpcszParameter, STVWONLINELICENSECHECKDATA * lpstData )
{
	if ( NULL == lpcszParameter || NULL == lpstData )
	{
		return FALSE;
	}
	
	BOOL  bRet = FALSE;
	TCHAR szUrlOnlineLicenseCheck[ MAX_PATH ]	= {0};
	TCHAR szInfoUrl[ MAX_URL ]	= {0};
	TCHAR szResponse[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};
	UINT  i				= 0;
	CDeHttp cDeHttp;
	
	LPSTR lpszTmp			= NULL;
	LPSTR lpszVwPrType		= NULL;
	LPSTR lpszVwPrTypeCs		= NULL;
	LPSTR lpszVwCDatePos		= NULL;
	LPSTR lpszVwExpireTypePos	= NULL;
	LPSTR lpszVwLeftDaysPos		= NULL;
	LPSTR lpszVwExpirePos		= NULL;

	//	...
	lpstData->nResult = -1;

	for ( i = 0; i < m_dwTryTimes; i ++ )
	{
		if ( IsRealVirtualWallServer() )
		{
			memcpy
			(
				szUrlOnlineLicenseCheck,
				g_ArrVwOnlineLicenseCheckServer[ m_dwServerNumber ].pszUrlOnlineLicenseCheckV3,
				min( g_ArrVwOnlineLicenseCheckServer[ m_dwServerNumber ].dwSizeOfUrlOnlineLicenseCheckV3, sizeof(szUrlOnlineLicenseCheck) )
			);
			delib_xorenc( szUrlOnlineLicenseCheck );

			_sntprintf( szInfoUrl, sizeof(szInfoUrl)-sizeof(TCHAR), "%s?%s", szUrlOnlineLicenseCheck, lpcszParameter );
			// ..
			memset( szResponse, 0, sizeof(szResponse) );
			bRet = cDeHttp.GetResponse( szInfoUrl, szResponse, sizeof(szResponse), szError, 30*1000 );
			if ( bRet && _tcslen( szResponse ) )
			{
				try
				{
					if ( _tcsstr( szResponse, "<result>1</result>" ) )
					{
						lpstData->nResult = 1;
					}
					else if ( _tcsstr( szResponse, "<result>0</result>" ) )
					{
						lpstData->nResult = 0;
					}

					lpszVwPrType		= _tcsstr( szResponse, "<vwprtype>" );
					lpszVwPrTypeCs		= _tcsstr( szResponse, "<vwprtypecs>" );
					lpszVwCDatePos		= _tcsstr( szResponse, "<vwcdate>" );
					lpszVwExpireTypePos	= _tcsstr( szResponse, "<vwexpiretype>" );
					lpszVwLeftDaysPos	= _tcsstr( szResponse, "<vwleftdays>" );
					lpszVwExpirePos		= _tcsstr( szResponse, "<vwexpire>" );

					if ( lpszVwPrType )
					{
						sscanf( lpszVwPrType, "<vwprtype>%s</vwprtype>\r\n", lpstData->szVwPrType );
						lpszTmp = strstr( lpstData->szVwPrType, "</" );
						if ( lpszTmp )
						{
							*lpszTmp = 0;
						}
					}
					if ( lpszVwPrTypeCs )
					{
						//	<vwprtypecs>a76e024d907205c9199184d63763475a</vwprtypecs>
						sscanf( lpszVwPrTypeCs, "<vwprtypecs>%s</vwprtypecs>\r\n", lpstData->szVwPrTypeChecksum );
						lpszTmp = strstr( lpstData->szVwPrTypeChecksum, "</" );
						if ( lpszTmp )
						{
							*lpszTmp = 0;
						}
					}
					if ( lpszVwCDatePos )
					{
						sscanf( lpszVwCDatePos, "<vwcdate>%s</vwcdate>\r\n", lpstData->szVwCdate );
						lpszTmp = strstr( lpstData->szVwCdate, "</" );
						if ( lpszTmp )
						{
							*lpszTmp = 0;
						}
					}
					if ( lpszVwExpireTypePos )
					{
						sscanf( lpszVwExpireTypePos, "<vwexpiretype>%d</vwexpiretype>\r\n", & lpstData->nVwExpireType );
					}
					if ( lpszVwLeftDaysPos )
					{
						sscanf( lpszVwLeftDaysPos, "<vwleftdays>%d</vwleftdays>\r\n", & lpstData->nVwLeftDays );
					}
					if ( lpszVwExpirePos )
					{
						sscanf( lpszVwExpirePos, "<vwexpire>%s</vwexpire>\r\n", lpstData->szVwExpireDate );
						lpszTmp = strstr( lpstData->szVwExpireDate, "</" );
						if ( lpszTmp )
						{
							*lpszTmp = 0;
						}
					}
				}
				catch ( ... )
				{
				}
			}

			// ..
			break;
		}
		else
		{
			Sleep( 30*1000 );
		}
	}
	// ..
	return bRet;
}


/**
 *	@ public
 *	向服务器发送该用户计算机上的信息
 */
BOOL CVwOnlineLicenseCheck::PostVwStartupInfoToServer( LPTSTR lpszFilterFilePath )
{
	if ( NULL == lpszFilterFilePath )
		return FALSE;
	
	TCHAR szUrlGetfilterversion[ MAX_PATH ]	= {0};
	TCHAR szInfoUrl[ MAX_URL ]		= {0};
	TCHAR szResponse[ MAX_PATH ]		= {0};
	TCHAR szError[ MAX_PATH ]		= {0};
	//	TCHAR szTempDir[ MAX_PATH ]		= {0};
	//	TCHAR szTempFile[ MAX_PATH ]		= {0};
	TCHAR szComputerName[ 64 ]		= {0};
	TCHAR szIpAddr[ 32 ]			= {0};
	TCHAR szOsType[ 64 ]			= {0};
	TCHAR szDllVersion[ 64 ]		= {0};
	DWORD dwSize			= 0;
	HRESULT hRetDown		= S_FALSE;
	CDeHttp cDeHttp;
	
	dwSize = sizeof(szComputerName);
	GetComputerName( szComputerName, &dwSize );
	
	delib_get_local_ipaddr( szIpAddr, sizeof(szIpAddr) );
	delib_get_os_type_string( szOsType, sizeof(szOsType) );
	delib_get_file_version( lpszFilterFilePath, szDllVersion, sizeof(szDllVersion) );

	
	//	GetTempPath( sizeof(szTempDir), szTempDir );
	//	if ( PathIsDirectory(szTempDir) )
	//		_sntprintf( szTempFile, sizeof(szTempFile)-sizeof(TCHAR), "%s_vw_info.tmp", szTempDir );

	
	//	解密 URL
	memcpy( szUrlGetfilterversion, g_szUrlGetfilterversion, min( sizeof(g_szUrlGetfilterversion), sizeof(szUrlGetfilterversion) ) );
	delib_xorenc( szUrlGetfilterversion );
	

	_sntprintf
	(
		szInfoUrl, sizeof(szInfoUrl)-sizeof(TCHAR),
		m_szPostStartupInfoFmt,		//	"%s?pn=%s&ip=%s&ot=%s&dver=%s", 
		szUrlGetfilterversion,
		szComputerName,
		szIpAddr,
		szOsType,
		szDllVersion
	);

	// ..
	cDeHttp.GetResponse( szInfoUrl, szResponse, sizeof(szResponse), szError, 30*1000 );
	// ..
	return S_OK == hRetDown ? TRUE : FALSE;	
}



//////////////////////////////////////////////////////////////////////////
//	Private




/**
 *	@ Private
 *	对服务器进行认证
 */
BOOL CVwOnlineLicenseCheck::IsRealVirtualWallServer()
{
	BOOL  bRet = FALSE;
	BOOL  bGetResp	= FALSE;
	TCHAR szUrlHelloServer1[ MAX_PATH ]	= {0};
	TCHAR szUrlHelloServer2[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};
	TCHAR szInfoUrl[ MAX_URL ]	= {0};
	TCHAR szResponse[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};
	TCHAR szClientKey[64]		= {0};
	TCHAR szServerRetChk[128]	= {0};
	TCHAR szAuthString[MAX_PATH]	= {0};
	TCHAR szAuthStringMd5[64]	= {0};
	DWORD dwTickCount		= 0;
	LPTSTR lpszMove			= NULL;
	LPTSTR lpszHead			= NULL;
	CDeHttp cDeHttp;

	// ..
	dwTickCount = GetTickCount();
	memset( szResponse, 0, sizeof(szResponse) );
	
	//	解密 URL
	memcpy
	(
		szUrlHelloServer1,
		g_ArrVwOnlineLicenseCheckServer[ m_dwServerNumber ].pszUrlHelloServer1,
		min( g_ArrVwOnlineLicenseCheckServer[ m_dwServerNumber ].dwSizeOfUrlHelloServer1, sizeof(szUrlHelloServer1) )
	);
	delib_xorenc( szUrlHelloServer1 );

	memcpy
	(
		szUrlHelloServer2,
		g_ArrVwOnlineLicenseCheckServer[ m_dwServerNumber ].pszUrlHelloServer2,
		min( g_ArrVwOnlineLicenseCheckServer[ m_dwServerNumber ].dwSizeOfUrlHelloServer2, sizeof(szUrlHelloServer2) )
	);
	delib_xorenc( szUrlHelloServer2 );
	

	bGetResp = cDeHttp.GetResponse( szUrlHelloServer1, szResponse, sizeof(szResponse), szError, 30*1000 );

	if ( bGetResp && _tcslen(szResponse) > 0 )
	{
		lpszMove = _tcsstr( szResponse, "<!--" );
		if ( lpszMove )
		{
			lpszHead = lpszMove + _tcslen( "<!--" );
			if ( lpszHead )
			{
				_sntprintf( szClientKey, sizeof(szClientKey)-sizeof(TCHAR), "%s", lpszHead );
				lpszMove = _tcsstr( szClientKey, "-->" );
				if ( lpszMove )
				{
					*lpszMove = 0;
				}
			}
		}
	}
	if ( 32 == _tcslen(szClientKey) )
	{
		memset( szResponse, 0, sizeof(szResponse) );
		_sntprintf( szInfoUrl, sizeof(szInfoUrl)-sizeof(TCHAR), "%s?ck=%s&tc=%u", szUrlHelloServer2, szClientKey, dwTickCount );
		bGetResp = cDeHttp.GetResponse( szInfoUrl, szResponse, sizeof(szResponse), szError, 30*1000 );
		
		if ( bGetResp && _tcslen(szResponse) > 0 )
		{
			lpszMove = _tcsstr( szResponse, "<!--" );
			if ( lpszMove )
			{
				lpszHead = lpszMove + _tcslen( "<!--" );
				if ( lpszHead )
				{
					_sntprintf( szServerRetChk, sizeof(szServerRetChk)-sizeof(TCHAR), "%s", lpszHead );
					lpszMove = _tcsstr( szServerRetChk, "-->" );
					if ( lpszMove )
					{
						*lpszMove = 0;
					}
				}
			}
		}
	}
	if ( 64 == _tcslen(szServerRetChk) )
	{
		_sntprintf( szAuthString, sizeof(szAuthString)-sizeof(TCHAR), "%s%u", szClientKey, dwTickCount );
		delib_get_string_md5( szAuthString, szAuthStringMd5, sizeof(szAuthStringMd5) );
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%s%s", szClientKey, szAuthStringMd5 );
		// ..
		if ( 0 == _tcsicmp( szServerRetChk, szTemp ) )
		{
			bRet = TRUE;
		}
	}
	return bRet;
}