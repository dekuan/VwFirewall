// ExportSdkFile.cpp: implementation of the CExportSdkFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExportSdkFile.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExportSdkFile::CExportSdkFile()
{
	memset( m_szOutFile, 0, sizeof(m_szOutFile) );
}

CExportSdkFile::~CExportSdkFile()
{

}

BOOL CExportSdkFile::SetConfig( STSDKFILECONFIG * pstCfg )
{
	if ( NULL == pstCfg )
	{
		return FALSE;
	}

	m_stCfg = *pstCfg;
	return TRUE;
}

BOOL CExportSdkFile::ExportSdkFile( UINT uResId, LPCSTR lpcszFilename )
{
	if ( NULL == lpcszFilename || 0 == strlen( lpcszFilename ) )
	{
		return FALSE;
	}

	return MakeNewSdkFile( uResId, lpcszFilename );
}

BOOL CExportSdkFile::MakeNewSdkFile( UINT uResId, LPCSTR lpcszFilename )
{
	/*
		Private Const CONST_PUBKEY	= "vsid"	' Vsid 的公共钥匙（请与远程服务器相同）
		Private Const CONST_PUBKEYTYPE	= 0		' Vsid 计算类型：0为普通，1为随机变化，2为文件名附加模式
		Private Const CONST_PUBKEYTIME	= 0		' Vsid 的有效时间（请与远程服务器相同）
		Private Const CONST_TIMESPAN	= 0		' 该值是本台服务器比远程服务器时间上慢的秒数
		Private Const CONST_DOMAIN	= "xxx.com"	' [仅在使用COOKIE时设置]这里设置您网站的域名，例如：www.fangdaolian.com，您应该设置：fangdaolian.com；详细请见下面“使用示例 [1]”
	*/
	if ( 0 == strlen( m_stCfg.szPubkey ) )
	{
		return FALSE;
	}

	TCHAR  szOutPath[ MAX_PATH ]	= {0};

	if ( ! BrowseForFolder( "", szOutPath, sizeof(szOutPath) ) )
	{
		return FALSE;
	}
	if ( ! PathIsDirectory( szOutPath ) )
	{
		return FALSE;
	}

	//	导出文件
	_sntprintf( m_szOutFile, sizeof(m_szOutFile)-sizeof(TCHAR), "%s\\%s", szOutPath, lpcszFilename );


	LPSTR  lpszTmpFile	= "sdk.tmp";
	FILE   * fp;
	FILE   * fpout;
	TCHAR  szLine[ 40960 ]	= {0};
	STREPLACETEMPLATE * pstRepTpl;

	//	asp
	STREPLACETEMPLATE stRepTplAsp[] = 
	{
		{ "Private Const CONST_PUBKEY	",	"Private Const CONST_PUBKEY	= \"%s\"	' Vsid 的公共钥匙（请与远程服务器相同）\r\n" },
		{ "Private Const CONST_PUBKEYTYPE",	"Private Const CONST_PUBKEYTYPE	= %d		' Vsid 计算类型：0为普通，1为随机变化，2为文件名附加模式\r\n" },
		{ "Private Const CONST_PUBKEYTIME",	"Private Const CONST_PUBKEYTIME	= %d		' Vsid 的有效时间（请与远程服务器相同）\r\n" },
		{ "Private Const CONST_DOMAIN",		"Private Const CONST_DOMAIN	= \"%s\"	' [仅在使用COOKIE时设置]这里设置您网站的域名，例如：www.vidun.com，您应该设置：vidun.com；详细请见下面“使用示例 [1]”\r\n" },
		{ "'[AUTO-PROCESS]",		"'[AUTO-PROCESS]\r\nCall AutoProcess()\r\n" },
	};

	//	php
	STREPLACETEMPLATE stRepTplPhp[] = 
	{
		{ "define( 'CONST_PUBKEY',",		"define( 'CONST_PUBKEY',	'%s');		// Vsid 的公共钥匙（请与远程服务器相同）\r\n" },
		{ "define( 'CONST_PUBKEYTYPE',",	"define( 'CONST_PUBKEYTYPE',	%d );		// Vsid 计算类型：0为普通，1为随机变化，2为文件名附加模式\r\n" },
		{ "define( 'CONST_PUBKEYTIME',",	"define( 'CONST_PUBKEYTIME',	%d );		// Vsid 的有效时间（请与远程服务器相同）\r\n" },
		{ "define( 'CONST_DOMAIN',",		"define( 'CONST_DOMAIN',	'%s' );		// [仅在使用COOKIE时设置]这里设置您网站的域名，例如：www.vidun.com，您应该设置：vidun.com；详细请见下面“使用示例 [1]”\r\n" },
		{ "//[AUTO-PROCESS]",		"//[AUTO-PROCESS]\r\nAutoProcess();\r\n" },
	};

	//	js of asp
	STREPLACETEMPLATE stRepTplJs[] = 
	{
		{ "var CONST_PUBKEY",		"var CONST_PUBKEY	= \"%s\";		// Vsid 的公共钥匙（请与远程服务器相同）\r\n" },
		{ "var CONST_PUBKEYTYPE",	"var CONST_PUBKEYTYPE	= %d;			// Vsid 计算类型：0为普通，1为随机变化，2为文件名附加模式\r\n" },
		{ "var CONST_PUBKEYTIME",	"var CONST_PUBKEYTIME	= %d;			// Vsid 的有效时间（请与远程服务器相同）\r\n" },
		{ "var CONST_DOMAIN",		"var CONST_DOMAIN	= \"%s\";		// [仅在使用COOKIE时设置]这里设置您网站的域名，\r\n" },
		{ "//[AUTO-PROCESS]",		"//[AUTO-PROCESS]\r\nAutoProcess();\r\n" },
	};

	DeleteFile( lpszTmpFile );
	//DeleteFile( "aaa.asp" );

	if ( 0 == stricmp( "vsidasp.asp", lpcszFilename ) )
	{
		//uResId = IDR_SDK_ASP;
		pstRepTpl = stRepTplAsp;
	}
	else if ( 0 == stricmp( "vsidphp.php", lpcszFilename ) )
	{
		//uResId = IDR_SDK_PHP;
		pstRepTpl = stRepTplPhp;
	}
	else if ( 0 == stricmp( "vsidjs.asp", lpcszFilename ) )
	{
		//uResId = IDR_SDK_JS;
		pstRepTpl = stRepTplJs;
	}

	if ( ! delib_extract_file_from_resource( AfxGetInstanceHandle(), "SDK", uResId, lpszTmpFile ) )
	{
		return FALSE;
	}

	//	...
	fp	= fopen( lpszTmpFile, "r" );
	fpout	= fopen( m_szOutFile, "w" );
	if ( fp && fpout )
	{
		while( ! feof( fp ) )
		{
			memset( szLine, 0, sizeof(szLine) );
			if ( fgets( szLine, sizeof(szLine), fp ) )
			{
				//	szPubkey
				if ( 0 == strnicmp( szLine, pstRepTpl[0].szFind, strlen( pstRepTpl[0].szFind ) ) )
				{
					_sntprintf( szLine, sizeof(szLine)-sizeof(TCHAR), pstRepTpl[0].szRepFmt, m_stCfg.szPubkey );
				}

				//	dwPubKeyType
				if ( 0 == strnicmp( szLine, pstRepTpl[1].szFind, strlen( pstRepTpl[1].szFind ) ) )
				{
					_sntprintf( szLine, sizeof(szLine)-sizeof(TCHAR), pstRepTpl[1].szRepFmt, m_stCfg.dwPubKeyType );
				}

				//	dwPubKeyTime
				if ( 0 == strnicmp( szLine, pstRepTpl[2].szFind, strlen( pstRepTpl[2].szFind ) ) )
				{
					_sntprintf( szLine, sizeof(szLine)-sizeof(TCHAR), pstRepTpl[2].szRepFmt, m_stCfg.dwPubKeyTime );
				}

				//	szDomain
				if ( _tcslen( m_stCfg.szDomain ) > 0 )
				{
					if ( 0 == strnicmp( szLine, pstRepTpl[3].szFind, strlen( pstRepTpl[3].szFind ) ) )
					{
						_sntprintf( szLine, sizeof(szLine)-sizeof(TCHAR), pstRepTpl[3].szRepFmt, m_stCfg.szDomain );
					}
				}

				//	AutoSetCookie
				if ( m_stCfg.bAutoSetCookie )
				{
					if ( 0 == strnicmp( szLine, pstRepTpl[4].szFind, strlen( pstRepTpl[4].szFind ) ) )
					{
						_sntprintf( szLine, sizeof(szLine)-sizeof(TCHAR), pstRepTpl[4].szRepFmt );
					}
				}

				//	输出到文件
				fputs( szLine, fpout );
			}
		}
	}

	if ( fp )
	{
		fclose( fp );
	}
	if ( fpout )
	{
		fclose( fpout );
	}

	DeleteFile( lpszTmpFile );


	return TRUE;
}

BOOL CExportSdkFile::BrowseForFolder( LPCTSTR lpcszTitle, LPTSTR lpszPath, DWORD dwSize )
{
	if ( NULL == lpszPath || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	CString sFolder;
	LPMALLOC pMalloc;

	//	Gets the Shell's default allocator
	if ( ::SHGetMalloc(&pMalloc) == NOERROR )
	{
		BROWSEINFO bi;
		char pszBuffer[MAX_PATH];
		LPITEMIDLIST pidl;

		bi.hwndOwner		= AfxGetMainWnd()->GetSafeHwnd();
		bi.pidlRoot		= NULL;
		bi.pszDisplayName	= pszBuffer;
		bi.lpszTitle		= lpcszTitle;
		bi.ulFlags		= BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn			= NULL;
		bi.lParam		= 0;

		//	This next call issues the dialog box.
		if ( ( pidl = ::SHBrowseForFolder(&bi)) != NULL )
		{
			if ( ::SHGetPathFromIDList( pidl, pszBuffer ) )
			{ 	
				//	At this point pszBuffer contains the selected path
				sFolder = pszBuffer;
			}
			//	Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free( pidl );

			//	read the file list again
			_sntprintf( lpszPath, dwSize-sizeof(TCHAR), "%s", sFolder.GetBuffer(0) );

			bRet = TRUE;
		}
		//	Release the shell's allocator.
		pMalloc->Release();
	}
	return bRet;
}