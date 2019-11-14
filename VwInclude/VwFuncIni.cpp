// VwFunc.cpp: implementation of the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "VwFuncIni.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )




/**
*	@ 私有函数
*	解析需要更新的文件列表
*/
BOOL _vwfunc_ini_parse_section_line( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, vector<STINISECTIONLINE> & vcSection, BOOL bParseValue /*=FALSE*/ )
{
	//
	//	lpcszIniFile	- [in] INI 文件路径
	//	lpcszSection	- [in] Section Name
	//	vcSection	- [out] 返回值
	//	bParseValue	- [in/opt] 是否要求解析 name=value 对成功
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpcszIniFile || ! PathFileExists( lpcszIniFile ) )
	{
		return FALSE;
	}
	if ( NULL == lpcszSection || 0 == _tcslen( lpcszSection ) )
	{
		return FALSE;
	}

	BOOL  bRet = FALSE;
	CMultiString mstr;
	TCHAR szSection[ 32767 ]		= {0};		// Win95 的大小限制
	INT   nSectionLen			= 0;
	INT   nNum				= 0;
	TCHAR ** ppList				= NULL;
	TCHAR ** p				= NULL;
	STINISECTIONLINE stLine;
	vector <STINISECTIONLINE>::iterator it;
	BOOL bExist				= FALSE;
	BOOL bParseValueSucc			= FALSE;
	TCHAR * pMov				= NULL;
	
	//	清理所有配置数据
	if ( vcSection.size() )
	{
		vcSection.clear();
	}

	nSectionLen = GetPrivateProfileSection( lpcszSection, szSection, sizeof(szSection), lpcszIniFile );
	if ( 0 == nSectionLen )
	{
		return FALSE;
	}
	
	ppList = mstr.MultiStringToStringList( szSection, nSectionLen+sizeof(TCHAR), &nNum );
	if ( ppList )
	{
		p = ppList;
		while( p && *p )
		{
			if ( 0 == _tcslen(*p) )
			{
				p ++;
				continue;
			}

			memset( & stLine, 0, sizeof( stLine ) );
			_sntprintf( stLine.szLine, sizeof(stLine.szLine)-sizeof(TCHAR), "%s", *p );

			//	解析 name=value 对
			bParseValueSucc = ( 2 == _stscanf( *p, "%[^=]=%s", stLine.szName, stLine.szValue ) ? TRUE : FALSE );

			bExist = FALSE;
			for ( it = vcSection.begin(); it != vcSection.end(); it ++ )
			{
				if ( 0 == _tcsicmp( stLine.szLine, (*it).szLine ) )
				{
					bExist = TRUE;
					break;
				}
			}

			if ( ! bExist )
			{
				if ( bParseValue )
				{
					//	调用者要求解析 name=value 对
					if ( bParseValueSucc )
					{
						//	将这个任务添加到队列
						vcSection.push_back( stLine );
					}
				}
				else
				{
					//	将这个任务添加到队列
					vcSection.push_back( stLine );
				}
			}

			//	..
			p ++;
		}
		free( ppList );
	}

	return TRUE;
}

/**
 *	根据 Acp 获取键值
 */
BOOL _vwfunc_ini_get_value_with_acpkey( LPCTSTR lpszIniFile, LPCTSTR lpszDomain, LPCTSTR lpszKey, LPTSTR lpszValue, DWORD dwSize )
{
	//
	//	lpszIniFile	- [in]  INI 文件路径
	//	lpszDomain	- [in]  Domain
	//	lpszKey		- [in]  Key
	//	lpszValue	- [out] 返回KEY值
	//	dwSize		- [in]  返回KEY值的缓冲区大小
	//	RETURN		- TRUE / FALSE
	//

	BOOL bRet	= FALSE;
	UINT uAcp	= GetACP();
	TCHAR szAcpKey[ MAX_PATH ] = {0};

	_sntprintf( szAcpKey, sizeof(szAcpKey)-sizeof(TCHAR), "%s_%d", lpszKey, uAcp );
	GetPrivateProfileString( lpszDomain, szAcpKey, "", lpszValue, dwSize, lpszIniFile );
	if ( _tcslen( lpszValue ) > 0 )
	{
		bRet = TRUE;
	}
	else
	{
		GetPrivateProfileString( lpszDomain, lpszKey, "", lpszValue, dwSize, lpszIniFile );
		if ( _tcslen( lpszValue ) > 0 )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

