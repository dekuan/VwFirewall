// DeRemoteLicence.cpp: implementation of the CDeRemoteLicence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeRemoteLicence.h"


#include "DeHttp.h"
#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeRemoteLicence::CDeRemoteLicence()
{
	m_hDllInstance	= NULL;
	m_bInitSucc	= FALSE;
	memset( m_szPrKey, 0, sizeof(m_szPrKey) );

	memset( m_szDecIniSecLicense, 0, sizeof(m_szDecIniSecLicense) );
	memset( m_szDecIniKeyGene, 0, sizeof(m_szDecIniKeyGene) );
	memset( m_szDecIniKeyRegCode, 0, sizeof(m_szDecIniKeyRegCode) );
	memset( m_szDecIniKeyActCode, 0, sizeof(m_szDecIniKeyActCode) );

	//
	//	解密 ini 的 sec/key 信息
	//
	memcpy( m_szDecIniSecLicense, g_szDeRemoteLicence_inisec_License, min( sizeof(g_szDeRemoteLicence_inisec_License), sizeof(m_szDecIniSecLicense) ) );
	delib_xorenc( m_szDecIniSecLicense );

	memcpy( m_szDecIniKeyGene, g_szDeRemoteLicence_inikey_Gene, min( sizeof(g_szDeRemoteLicence_inikey_Gene), sizeof(m_szDecIniKeyGene) ) );
	delib_xorenc( m_szDecIniKeyGene );

	memcpy( m_szDecIniKeyRegCode, g_szDeRemoteLicence_inikey_RegCode, min( sizeof(g_szDeRemoteLicence_inikey_RegCode), sizeof(m_szDecIniKeyRegCode) ) );
	delib_xorenc( m_szDecIniKeyRegCode );

	memcpy( m_szDecIniKeyActCode, g_szDeRemoteLicence_inikey_ActCode, min( sizeof(g_szDeRemoteLicence_inikey_ActCode), sizeof(m_szDecIniKeyActCode) ) );
	delib_xorenc( m_szDecIniKeyActCode );

	memcpy( m_szDecPhpQueryActCodeKey, g_szDeRemoteLicence_PhpQueryActCodeKey, min( sizeof(g_szDeRemoteLicence_PhpQueryActCodeKey), sizeof(m_szDecPhpQueryActCodeKey) ) );
	delib_xorenc( m_szDecPhpQueryActCodeKey );
	

	//	...
	InitializeCriticalSection( & m_oCriSecKeyFile );


	//	获取本 DLL 的 Instance 和 DLL 的全路径
	delib_get_modulefile_path( &__cderemotelicence_global_voidfunc, &m_hDllInstance, szDllFilePath, sizeof(szDllFilePath) );

	//	创建本产品的许可加密 Key
	BuildKey();
	
	//	初始化 Licence 信息
	CDeLicence::InitData( m_hDllInstance, m_szPrKey );

}
CDeRemoteLicence::~CDeRemoteLicence()
{
	DeleteCriticalSection( & m_oCriSecKeyFile );
}

//	验证是否是正确的注册码
BOOL CDeRemoteLicence::IsValidDeRemoteLicence( LPCTSTR lpcszActCode, BOOL bReLoadConfig /* = FALSE */ )
{
	if ( bReLoadConfig )
	{
		ReLoadLicenseInfo();
	}

	return CDeLicence::IsValidLicence( lpcszActCode );
}



//	在线激活
LONG CDeRemoteLicence::QueryActCodeFromServer( LPCTSTR lpcszRegCode, LPTSTR lpszActCode, DWORD dwSize )
{
	//
	//	lpcszRegCode	- [in] regcode
	//	lpszActCode	- [out] 返回从服务器获取的 actcode
	//	dwSize		- [in] sizeof lpszActCode
	//	RETURN		- TRUE / FALSE
	//

	LONG lnRet		= MCH_GET_ACTCODE_ERR_UNKNOWN;

	CDeHttp cHttp;
	TCHAR szUrl[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};

	TCHAR szSrcString[ MAX_PATH ]	= {0};
	TCHAR szMd5Str[ MAX_PATH ]	= {0};

	TCHAR szResponse[ 1024 ]	= {0};
	TCHAR szResult[ 32 ]		= {0};
	TCHAR szActCode[ 64 ]		= {0};
	TCHAR szResMd5Str[ MAX_PATH ]	= {0};
	TCHAR * lpszRespCnt		= NULL;


	//	首先获取通讯密码
	if ( GetQueryActCodeMd5info( CDeLicence::mb_stLcInfo.szGene, szMd5Str, sizeof(szMd5Str) ) )
	{
		_sntprintf
		(
			szUrl,
			sizeof(szUrl)-sizeof(TCHAR),
			_T("http://rd.desafe.com/?id=2001&softname=%s&softgene=%s&regcode=%s&md5str=%s"),
			_T("deremote"), CDeLicence::mb_stLcInfo.szGene, lpcszRegCode, szMd5Str
		);

		cHttp.GetResponse( szUrl, szResponse, sizeof(szResponse), szError, 60*1000 );
		if ( _tcslen(szResponse) )
		{
			lpszRespCnt = szResponse;
			if ( -17 == szResponse[0] && -69 == szResponse[1] && -65 == szResponse[2] )
			{
				//	去掉 UTF-8 编码的头部 3 个字节(EF BB BF)
				lpszRespCnt = szResponse + sizeof(TCHAR) * 3;
			}
			if ( lpszRespCnt && _tcslen(lpszRespCnt) )
			{
				StrTrim( lpszRespCnt, _T("\r\n ") );

				GetCookieValue( lpszRespCnt, _T("result="), szResult, sizeof(szResult) );
				GetCookieValue( lpszRespCnt, _T("actcode="), szActCode, sizeof(szActCode) );
				GetCookieValue( lpszRespCnt, _T("md5str="), szResMd5Str, sizeof(szResMd5Str) );

				//	将结果转换成 LONG
				lnRet = atol( szResult );

				//	如果获取成功，则将服务器返回的 actcode 拷贝到返回缓冲区
				if ( 0 == _tcsicmp( _T("0"), szResult ) && _tcslen(szActCode) )
				{
					//	返回密码的算法
					_sntprintf( szSrcString, sizeof(szSrcString)-sizeof(TCHAR), _T("%s%s"), szResult, szActCode );
					if ( IsValidQueryActCodeData( szResMd5Str, szSrcString ) )
					{
						//	...
						_sntprintf( lpszActCode, dwSize-sizeof(TCHAR), _T("%s"), szActCode );
					}
				}
			}
			else
			{
				lnRet = MCH_GET_ACTCODE_ERR_NETWORK;
			}
		}
		else
		{
			lnRet = MCH_GET_ACTCODE_ERR_NETWORK;
		}
	}

	return lnRet;
}

/**
 *	get_actcode 通讯密码
 *	此函数在 ActivateOnline 向服务器发送请求时候负责计算通讯密码
 */
BOOL CDeRemoteLicence::GetQueryActCodeMd5info( LPCTSTR lpcszStr, LPTSTR lpszMd5, DWORD dwSize )
{
	if ( NULL == lpcszStr || NULL == lpszMd5 || dwSize <= 32 )
	{
		return FALSE;
	}
	
	TCHAR szString[ MAX_PATH ]	= {0};
	TCHAR szSrc[ MAX_PATH ]		= {0};
	
	_sntprintf( szString, sizeof(szString)-sizeof(TCHAR), _T("%s"), lpcszStr );
	_tcsupr( szString );
	_sntprintf( szSrc, sizeof(szSrc)-sizeof(TCHAR), _T("%s%s"), szString, m_szDecPhpQueryActCodeKey );
	
	GetStringMd5( szSrc, lpszMd5, dwSize );
	_tcsupr( lpszMd5 );
	
	return TRUE;
}

/**
 *	检测在函数 ActivateOnline 中客户端和服务器端通讯的密码是否正确
 */
BOOL CDeRemoteLicence::IsValidQueryActCodeData( LPCTSTR lpcszMd5, LPCTSTR lpcszStr )
{
	//
	//	$gmd5str	- [in] 校验 MD5 值
	//	$str		- [in] 原始产生校验核的字符串，例如软件的特征码等
	//	RETURN		- true / false
	//
	BOOL  bRet		= FALSE;
	TCHAR szMd5Str[ 64 ]	= {0};
	
	if ( GetQueryActCodeMd5info( lpcszStr, szMd5Str, sizeof(szMd5Str) ) )
	{
		if ( 0 == _tcsicmp( lpcszMd5, szMd5Str ) )
		{
			bRet = TRUE;
		}
	}
	
	return bRet;
}

/**
 *	装载 License Info
 */
BOOL CDeRemoteLicence::ReLoadLicenseInfo()
{
	TCHAR szCkName[ 32 ]		= {0};
	TCHAR szFileBuffer[ 1024 ]	= {0};

	//
	//	进入红灯区
	//
	EnterCriticalSection( & m_oCriSecKeyFile );


	if ( m_cDeFileEnc.GetDecodedBufferFromFile( CModuleInfoDeRemote::m_szKeyFile, szFileBuffer, sizeof(szFileBuffer) ) )
	{
		//	这里不要读取 gene，因为这个是计算出来的

		//	regcode
		_sntprintf( szCkName, sizeof(szCkName)-sizeof(TCHAR), _T("%s="), m_szDecIniKeyRegCode );
		GetCookieValue( szFileBuffer, szCkName, CDeLicence::mb_stLcInfo.szRegCode, sizeof(CDeLicence::mb_stLcInfo.szRegCode) );

		//	actcode
		_sntprintf( szCkName, sizeof(szCkName)-sizeof(TCHAR), _T("%s="), m_szDecIniKeyActCode );
		GetCookieValue( szFileBuffer, szCkName, CDeLicence::mb_stLcInfo.szActCode, sizeof(CDeLicence::mb_stLcInfo.szActCode) );
	}


	//
	//	离开红灯区
	//
	LeaveCriticalSection( & m_oCriSecKeyFile );

	return TRUE;
}

/**
 *	保存 License Info
 */
BOOL CDeRemoteLicence::SaveLicenseInfo( STDELICENSEINFO * pstDeLicenseInfo, BOOL bForceWrite /* = FALSE */ )
{
	if ( NULL == pstDeLicenseInfo )
	{
		return FALSE;
	}

	TCHAR szBuffer[ 1024 ]	= {0};
	STDELICENSEINFO stNewLInfo;	//	要保存的内容先挑选到这里


	//	先装载一次配置
	ReLoadLicenseInfo();

	//	默认使用配置文件中的内容
	memset( & stNewLInfo, 0, sizeof(stNewLInfo) );
	stNewLInfo	= CDeLicence::mb_stLcInfo;


	//
	//	进入红灯区
	//
	EnterCriticalSection( & m_oCriSecKeyFile );


	//	gene
	if ( bForceWrite || _tcslen(pstDeLicenseInfo->szGene) )
	{
		//	使用调用者给出的
		_sntprintf( stNewLInfo.szGene, sizeof(stNewLInfo.szGene)-sizeof(TCHAR), _T("%s"), pstDeLicenseInfo->szGene );
	}

	//	regcode
	if ( bForceWrite || _tcslen(pstDeLicenseInfo->szRegCode) )
	{
		//	使用调用者给出的
		_sntprintf( stNewLInfo.szRegCode, sizeof(stNewLInfo.szRegCode)-sizeof(TCHAR), _T("%s"), pstDeLicenseInfo->szRegCode );
	}

	//	actcode
	if ( _tcslen(pstDeLicenseInfo->szActCode) )
	{
		//	使用调用者给出的
		_sntprintf( stNewLInfo.szActCode, sizeof(stNewLInfo.szActCode)-sizeof(TCHAR), _T("%s"), pstDeLicenseInfo->szActCode );
	}


	//	组成数据格式串
	_sntprintf
	(
		szBuffer,
		sizeof(szBuffer)-sizeof(TCHAR),
		_T("%s=%s;%s=%s;%s=%s;"),
		m_szDecIniKeyGene, stNewLInfo.szGene,
		m_szDecIniKeyRegCode, stNewLInfo.szRegCode,
		m_szDecIniKeyActCode, stNewLInfo.szActCode
	);

	m_cDeFileEnc.SaveEncryptedBufferToFile( szBuffer, CModuleInfoDeRemote::m_szKeyFile );


	//
	//	离开红灯区
	//
	LeaveCriticalSection( & m_oCriSecKeyFile );

	return TRUE;
}









//////////////////////////////////////////////////////////////////////////
//	Private



BOOL CDeRemoteLicence::BuildKey()
{
	_sntprintf( m_szPrKey, sizeof(m_szPrKey)-sizeof(TCHAR), _T("%s"), _T("DeRemote") );
	memset( m_szPrKey, 0, sizeof(m_szPrKey) );

	memcpy( m_szPrKey, g_szDeRemoteLicence_PrKey, min( sizeof(g_szDeRemoteLicence_PrKey), sizeof(m_szPrKey) ) );
	delib_xorenc( m_szPrKey );

	return TRUE;
}

