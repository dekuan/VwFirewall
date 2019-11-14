// DeRemoteLicence.cpp: implementation of the CDeRemoteLicence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeRemoteLicence.h"

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

/**
 *	装载 License Info
 */
BOOL CDeRemoteLicence::ReLoadLicenseInfo()
{
	TCHAR szCkName[ 32 ]		= {0};
	TCHAR szFileBuffer[ 1024 ]	= {0};

	//	进入红灯区
	EnterCriticalSection( & m_oCriSecKeyFile );

	//	m_cDeFileEnc.EncryptFile( CModuleInfoDeRemote::m_szKeyFile, CModuleInfoDeRemote::m_szKeyDecFile );
	if ( m_cDeFileEnc.GetDecodedBufferFromFile( CModuleInfoDeRemote::m_szKeyFile, szFileBuffer, sizeof(szFileBuffer) ) )
	{
		//	这里不要读取 gene，因为这个是计算出来的

		//	regcode
		_sntprintf( szCkName, sizeof(szCkName)-sizeof(TCHAR), _T("%s="), m_szDecIniKeyRegCode );
		delib_get_cookie_value( szFileBuffer, szCkName, CDeLicence::mb_stLcInfo.szRegCode, sizeof(CDeLicence::mb_stLcInfo.szRegCode) );

		//	actcode
		_sntprintf( szCkName, sizeof(szCkName)-sizeof(TCHAR), _T("%s="), m_szDecIniKeyActCode );
		delib_get_cookie_value( szFileBuffer, szCkName, CDeLicence::mb_stLcInfo.szActCode, sizeof(CDeLicence::mb_stLcInfo.szActCode) );
	}


	//
	//	读取配置信息
	//
/*
	//mb_pfnGetPrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyGene, _T(""), CDeLicence::m_szGene, sizeof(CDeLicence::m_szGene), CModuleInfoDeRemote::m_szKeyDecFile );
	//_tcsupr( CDeLicence::m_szGene );

	mb_pfnGetPrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyRegCode, _T(""), CDeLicence::mb_stLcInfo.szRegCode, sizeof(CDeLicence::mb_stLcInfo.szRegCode), CModuleInfoDeRemote::m_szKeyDecFile );
	_tcsupr( CDeLicence::mb_stLcInfo.szRegCode );

	mb_pfnGetPrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyActCode, _T(""), CDeLicence::mb_stLcInfo.szActCode, sizeof(CDeLicence::mb_stLcInfo.szActCode), CModuleInfoDeRemote::m_szKeyDecFile );
	_tcsupr( CDeLicence::mb_stLcInfo.szActCode );
	
	//m_cDeFileEnc.EncryptFile( CModuleInfoDeRemote::m_szKeyDecFile, CModuleInfoDeRemote::m_szKeyFile );
	//_tunlink( CModuleInfoDeRemote::m_szKeyDecFile );
*/

	//	离开红灯区
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


	//	进入红灯区
	EnterCriticalSection( & m_oCriSecKeyFile );
	//m_cDeFileEnc.EncryptFile( CModuleInfoDeRemote::m_szKeyFile, CModuleInfoDeRemote::m_szKeyDecFile );

	//	默认使用配置文件中的内容
	memset( & stNewLInfo, 0, sizeof(stNewLInfo) );
	stNewLInfo	= CDeLicence::mb_stLcInfo;

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

/*
	//
	//	写入配置信息
	//

	//	写入 gene
	if ( bForceWrite || _tcslen(pstDeLicenseInfo->szGene) )
	{
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), _T("\"%s\""), pstDeLicenseInfo->szGene );
		mb_pfnWritePrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyGene, szTemp, CModuleInfoDeRemote::m_szKeyDecFile );
	}

	//	写入 regcode
	if ( bForceWrite || _tcslen(pstDeLicenseInfo->szRegCode) )
	{
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), _T("\"%s\""), pstDeLicenseInfo->szRegCode );
		mb_pfnWritePrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyRegCode, szTemp, CModuleInfoDeRemote::m_szKeyDecFile );
	}

	//	写入 actcode
	if ( _tcslen(pstDeLicenseInfo->szActCode) )
	{
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), _T("\"%s\""), pstDeLicenseInfo->szActCode );
		mb_pfnWritePrivateProfileString( m_szDecIniSecLicense, m_szDecIniKeyActCode, szTemp, CModuleInfoDeRemote::m_szKeyDecFile );
	}
*/

	//m_cDeFileEnc.EncryptFile( CModuleInfoDeRemote::m_szKeyDecFile, CModuleInfoDeRemote::m_szKeyFile );
	//_tunlink( CModuleInfoDeRemote::m_szKeyDecFile );
	
	//	离开红灯区	
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

