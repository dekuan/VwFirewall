// ParseCfgData.cpp: implementation of the CParseCfgData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "ParseCfgData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseCfgData::CParseCfgData()
{

}

CParseCfgData::~CParseCfgData()
{

}

BOOL CParseCfgData::LoadData( LPCTSTR lpcszFilename )
{
	BOOL bRet;
	STINISECTIONLINE stLine;
	STINISECTIONLINE * pstIniSecs;
	INT nSecsCount;
	INT nSecsCount2;
	INT nGroupIndex;
	INT i;
	STPARSECFGDATAGROUP stAclsDataGroup;
	
	if ( NULL == lpcszFilename || 0 == _tcslen( lpcszFilename ) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszFilename ) )
	{
		return FALSE;
	}

	//	...
	bRet		= FALSE;
	nGroupIndex	= 0;

	//
	//	...
	//
	m_vcAclsGroupList.clear();

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );
	nSecsCount = delib_ini_parse_section_lineex( lpcszFilename, "GroupList", NULL, TRUE );
	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	if ( nSecsCount > 0 )
	{
		pstIniSecs = new STINISECTIONLINE[ nSecsCount ];
		if ( pstIniSecs )
		{
			memset( pstIniSecs, 0, sizeof(STINISECTIONLINE)*nSecsCount );

			delib_drv_wow64_enablewow64_fs_redirection( FALSE );
			nSecsCount2 = delib_ini_parse_section_lineex( lpcszFilename, "GroupList", pstIniSecs, TRUE );
			delib_drv_wow64_enablewow64_fs_redirection( TRUE );

			if ( nSecsCount == nSecsCount2 )
			{
				//	...
				bRet = TRUE;

				for ( i = 0; i < nSecsCount; i ++ )
				{
					//
					if ( 0 == _tcsnicmp( ";", pstIniSecs[ i ].szLine, 1 ) )
					{
						continue;
					}

					memset( & stAclsDataGroup, 0, sizeof(stAclsDataGroup) );					
					_sntprintf( stAclsDataGroup.szGroup, sizeof(stAclsDataGroup.szGroup)/sizeof(TCHAR)-1, _T("%s"), pstIniSecs[ i ].szValue );
					_sntprintf( stAclsDataGroup.szGroupSet, sizeof(stAclsDataGroup.szGroupSet)/sizeof(TCHAR)-1, _T("%s.set"), pstIniSecs[ i ].szValue );
					CVwFirewallConfigFile::LoadConfigString( stAclsDataGroup.szGroup, "name", stAclsDataGroup.szGroupName, sizeof(stAclsDataGroup.szGroupName), lpcszFilename );
					if ( 0 == _tcslen( stAclsDataGroup.szGroupName ) )
					{
						_sntprintf( stAclsDataGroup.szGroupName, sizeof(stAclsDataGroup.szGroupName)/sizeof(TCHAR)-1, _T("%s°²È«"),  stAclsDataGroup.szGroup );
					}

					//	...
					stAclsDataGroup.vcAclsSetList.clear();
					LoadAclsSetList( lpcszFilename, stAclsDataGroup.szGroupSet, stAclsDataGroup.vcAclsSetList );

					//	...
					stAclsDataGroup.nIndex = nGroupIndex;
					m_vcAclsGroupList.push_back( stAclsDataGroup );

					//	...
					nGroupIndex ++;
				}
			}

			delete [] pstIniSecs;
			pstIniSecs = NULL;
		}
	}


	//
	//	TODO: parse acls set for all groups
	//


	return bRet;
}

BOOL CParseCfgData::LoadAclsSetList( LPCTSTR lpcszFilename, LPCTSTR lpcszGroupSet, vector<STPARSECFGDATASET> & vcAclsSetList )
{
	BOOL bRet;
	STINISECTIONLINE stLine;
	STINISECTIONLINE * pstIniSecs;
	INT nSecsCount;
	INT nSecsCount2;
	INT i;	
	STPARSECFGDATASET stAclsDataSet;

	if ( NULL == lpcszFilename || 0 == _tcslen( lpcszFilename ) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszFilename ) )
	{
		return FALSE;
	}
	if ( NULL == lpcszGroupSet || 0 == _tcslen( lpcszGroupSet ) )
	{
		return FALSE;
	}


	//	...
	bRet = FALSE;

	//	...
	vcAclsSetList.clear();

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );
	nSecsCount = delib_ini_parse_section_lineex( lpcszFilename, lpcszGroupSet, NULL, FALSE );
	delib_drv_wow64_enablewow64_fs_redirection( TRUE );
	
	if ( nSecsCount > 0 )
	{
		pstIniSecs = new STINISECTIONLINE[ nSecsCount ];
		if ( pstIniSecs )
		{
			memset( pstIniSecs, 0, sizeof(STINISECTIONLINE)*nSecsCount );
			
			delib_drv_wow64_enablewow64_fs_redirection( FALSE );
			nSecsCount2 = delib_ini_parse_section_lineex( lpcszFilename, lpcszGroupSet, pstIniSecs, FALSE );
			delib_drv_wow64_enablewow64_fs_redirection( TRUE );

			if ( nSecsCount == nSecsCount2 )
			{
				//	...
				bRet = TRUE;

				for ( i = 0; i < nSecsCount; i ++ )
				{
					//
					if ( 0 == _tcsnicmp( ";", pstIniSecs[ i ].szLine, 1 ) )
					{
						continue;
					}
					
					memset( & stAclsDataSet, 0, sizeof(stAclsDataSet) );

					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("type="), "|||", stAclsDataSet.szType, sizeof(stAclsDataSet.szType), FALSE );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("user="), "|||", stAclsDataSet.szUser, sizeof(stAclsDataSet.szUser), FALSE );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("chk="), "|||", stAclsDataSet.szChk, sizeof(stAclsDataSet.szChk), FALSE );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("cmd="), "|||", stAclsDataSet.szCmd, sizeof(stAclsDataSet.szCmd), FALSE );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("path="), "|||", stAclsDataSet.szPath, sizeof(stAclsDataSet.szPath), FALSE );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("param="), "|||", stAclsDataSet.szParam, sizeof(stAclsDataSet.szParam), FALSE );

					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("regroot="), "|||", stAclsDataSet.szRegRoot, sizeof(stAclsDataSet.szRegRoot), FALSE );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("regpath="), "|||", stAclsDataSet.szRegPath, sizeof(stAclsDataSet.szRegPath), FALSE );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("regvname="), "|||", stAclsDataSet.szRegVName, sizeof(stAclsDataSet.szRegVName), FALSE );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("regvtype="), "|||", stAclsDataSet.szRegVType, sizeof(stAclsDataSet.szRegVType), FALSE );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("regv="), "|||", stAclsDataSet.szRegV, sizeof(stAclsDataSet.szRegV), FALSE );

					delib_get_casecookie_value_ex( pstIniSecs[ i ].szLine, _T("remark="), "|||", stAclsDataSet.szRemark, sizeof(stAclsDataSet.szRemark), FALSE );

					//	...
					_tcsupr( stAclsDataSet.szChk );

					//	...
					delib_replace( stAclsDataSet.szCmd, "%VwScriptDir%", CVwFirewallConfigFile::m_szScriptDir );
					delib_replace( stAclsDataSet.szCmd, "%SystemRoot%", CVwFirewallConfigFile::mb_szWinDir );
					delib_replace( stAclsDataSet.szCmd, "%SystemDrive%", CVwFirewallConfigFile::mb_szSysDrive );

					delib_replace( stAclsDataSet.szPath, "%VwScriptDir%", CVwFirewallConfigFile::m_szScriptDir );
					delib_replace( stAclsDataSet.szPath, "%SystemRoot%", CVwFirewallConfigFile::mb_szWinDir );
					delib_replace( stAclsDataSet.szPath, "%SystemDrive%", CVwFirewallConfigFile::mb_szSysDrive );

					delib_replace( stAclsDataSet.szParam, "%VwScriptDir%", CVwFirewallConfigFile::m_szScriptDir );
					delib_replace( stAclsDataSet.szParam, "%SystemRoot%", CVwFirewallConfigFile::mb_szWinDir );
					delib_replace( stAclsDataSet.szParam, "%SystemDrive%", CVwFirewallConfigFile::mb_szSysDrive );

					delib_replace( stAclsDataSet.szCmd, "/", "\\" );
					delib_replace( stAclsDataSet.szCmd, "\\\\", "\\" );

					delib_replace( stAclsDataSet.szPath, "/", "\\" );
					delib_replace( stAclsDataSet.szPath, "\\\\", "\\" );
					StrTrim( stAclsDataSet.szPath, _T("\r\t \"\'") );

					delib_replace( stAclsDataSet.szParam, "\\/", "\\" );
					delib_replace( stAclsDataSet.szParam, "/\\", "\\" );
					delib_replace( stAclsDataSet.szParam, "\\\\", "\\" );

					stAclsDataSet.hkRoot = 0;
					GetHKeyRootFromString( stAclsDataSet.szRegRoot, & stAclsDataSet.hkRoot );
					stAclsDataSet.lnRegVType = GetRegDataTypeFromString( stAclsDataSet.szRegVType );

					//	...
					vcAclsSetList.push_back( stAclsDataSet );
				}
			}

			delete [] pstIniSecs;
			pstIniSecs = NULL;
		}
	}

	return bRet;
}

BOOL CParseCfgData::GetHKeyRootFromString( LPCTSTR lpcszRootString, HKEY * phkRoot )
{
	BOOL bRet;

	if ( NULL == lpcszRootString || 0 == _tcslen( lpcszRootString ) )
	{
		return FALSE;
	}
	if ( NULL == phkRoot )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	if ( 0 == _tcsicmp( "HKEY_CLASSES_ROOT", lpcszRootString ) )
	{
		bRet = TRUE;
		*phkRoot = HKEY_CLASSES_ROOT;
	}
	else if ( 0 == _tcsicmp( "HKEY_CURRENT_CONFIG", lpcszRootString ) )
	{
		bRet = TRUE;
		*phkRoot = HKEY_CURRENT_CONFIG;
	}
	else if ( 0 == _tcsicmp( "HKEY_CURRENT_USER", lpcszRootString ) )
	{
		bRet = TRUE;
		*phkRoot = HKEY_CURRENT_USER;
	}
	else if ( 0 == _tcsicmp( "HKEY_LOCAL_MACHINE", lpcszRootString ) )
	{
		bRet = TRUE;
		*phkRoot = HKEY_LOCAL_MACHINE;
	}
	else if ( 0 == _tcsicmp( "HKEY_PERFORMANCE_DATA", lpcszRootString ) )
	{
		bRet = TRUE;
		*phkRoot = HKEY_PERFORMANCE_DATA;
	}
	else if ( 0 == _tcsicmp( "HKEY_USERS", lpcszRootString ) )
	{
		bRet = TRUE;
		*phkRoot = HKEY_USERS;
	}

	return bRet;
}

LONG CParseCfgData::GetRegDataTypeFromString( LPCTSTR lpcszDataTypeString )
{
	LONG lnRet;

	if ( NULL == lpcszDataTypeString || 0 == _tcslen( lpcszDataTypeString ) )
	{
		return -1;
	}

	//	...
	lnRet = -1;

	//
	//	...
	//
	if ( 0 == _tcsicmp( "REG_BINARY", lpcszDataTypeString ) )
	{
		//	Binary data in any form.		
		lnRet =  REG_BINARY;
	}
	else if ( 0 == _tcsicmp( "REG_DWORD", lpcszDataTypeString ) )
	{
		//	32-bit number.		
		lnRet =  REG_DWORD;
	}
	else if ( 0 == _tcsicmp( "REG_QWORD", lpcszDataTypeString ) )
	{
		//	64-bit number.		
		lnRet =  REG_QWORD;
	}
	else if ( 0 == _tcsicmp( "REG_DWORD_LITTLE_ENDIAN", lpcszDataTypeString ) )
	{
		//	32-bit number in little-endian format. This is equivalent to REG_DWORD
		lnRet =  REG_DWORD_LITTLE_ENDIAN;
	}
	else if ( 0 == _tcsicmp( "REG_QWORD_LITTLE_ENDIAN", lpcszDataTypeString ) )
	{
		//	A 64-bit number in little-endian format. This is equivalent to REG_QWORD. 		
		lnRet =  REG_QWORD_LITTLE_ENDIAN;
	}
	else if ( 0 == _tcsicmp( "REG_DWORD_BIG_ENDIAN", lpcszDataTypeString ) )
	{
		//	32-bit number in big-endian format.		
		lnRet =  REG_DWORD_BIG_ENDIAN;
	}
	else if ( 0 == _tcsicmp( "REG_EXPAND_SZ", lpcszDataTypeString ) )
	{
		//	Null-terminated string that contains unexpanded references to environment variables (for example, "%PATH%").
		//	It will be a Unicode or ANSI string, depending on whether you use the Unicode or ANSI functions.		
		lnRet =  REG_EXPAND_SZ;
	}
	else if ( 0 == _tcsicmp( "REG_LINK", lpcszDataTypeString ) )
	{
		//	Unicode symbolic link.		
		lnRet =  REG_LINK;
	}
	else if ( 0 == _tcsicmp( "REG_MULTI_SZ", lpcszDataTypeString ) )
	{
		//	Array of null-terminated strings that are terminated by two null characters.		
		lnRet =  REG_MULTI_SZ;
	}
	else if ( 0 == _tcsicmp( "REG_NONE", lpcszDataTypeString ) )
	{
		//	No defined value type.		
		lnRet =  REG_NONE;
	}
	else if ( 0 == _tcsicmp( "REG_RESOURCE_LIST", lpcszDataTypeString ) )
	{
		//	Device-driver resource list.		
		lnRet =  REG_RESOURCE_LIST;
	}
	else if ( 0 == _tcsicmp( "REG_SZ", lpcszDataTypeString ) )
	{
		//	Null-terminated string. It will be a Unicode or ANSI string, depending on whether you use the Unicode or ANSI functions.		
		lnRet =  REG_SZ;
	}

	return lnRet;
}