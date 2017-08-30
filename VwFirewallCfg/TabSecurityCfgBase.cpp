// TabSecurityCfgBase.cpp: implementation of the CTabSecurityCfgBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabSecurityCfgBase.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "Lmaccess.h"
#include "Lm.h"
#pragma comment( lib, "Netapi32.lib" )


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTabSecurityCfgBase::CTabSecurityCfgBase()
{
	//
	//	Build Group ImageList
	//	
	HICON hIcon;
	
	m_hImageListGroup = ImageList_Create( 16, 16, ILC_MASK | ILC_COLOR32, 0, 1 );
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_WAIT_S) );
	if ( hIcon )	// 0
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_OK) );
	if ( hIcon )	// 1
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_X) );
	if ( hIcon )	// 2
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_OKB) );
	if ( hIcon )	// 3
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_LOADING1) );
	if ( hIcon )	// 4
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_LOADING2) );
	if ( hIcon )	// 5
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_LOADING3) );
	if ( hIcon )	// 6
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_LOADING4) );
	if ( hIcon )	// 7
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_LOADING5) );
	if ( hIcon )	// 8
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}
	hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_LOADING6) );
	if ( hIcon )	// 9
	{
		ImageList_AddIcon( m_hImageListGroup, hIcon );
		DestroyIcon( hIcon );
	}

	//	...
	m_plistCtrl			= NULL;
	m_lnListRowIndex		= -1;
	m_lnListColumnIndex		= -1;

	//	...
	m_bInitLocalAccountsSucc	= InitLocalAccounts();

	//	...
	m_bFixAclsProblemsContinue	= TRUE;

	
	//
	//	开启修复线程
	//
	m_cThSleepFlashFixingIcon.m_hThread = (HANDLE)_beginthreadex
		(
			NULL,
			0,
			&_threadFlashFixingIcon,
			(void*)this,
			0,
			&m_cThSleepFlashFixingIcon.m_uThreadId
		);
}
CTabSecurityCfgBase::~CTabSecurityCfgBase()
{
	m_cThSleepFlashFixingIcon.EndSleep();
	m_cThSleepFlashFixingIcon.EndThread();
}

BOOL CTabSecurityCfgBase::Regsvr32_Wshom_ocx( BOOL bReg )
{
	TCHAR szCmdLine[ 1024 ];

	memset( szCmdLine, 0, sizeof(szCmdLine) );
	_sntprintf
	(
		szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1,
		_T("%s %s \"%s\""),
		m_cVwFirewallConfigFile.mb_szSysRegsvr32File,
		( bReg ? _T("/s") : _T("/s /u") ),
		m_cVwFirewallConfigFile.m_szSys_Wshom_ocx
	);
	return delib_run_block_process_ex( szCmdLine, FALSE );
}


BOOL CTabSecurityCfgBase::IsNeedToFix( STPARSECFGDATAGROUP * pstAclsDataGroup )
{
	BOOL bRet;
	BOOL bLocUserExist;
	ACCESS_MASK Mask;
	STPARSECFGDATASET * pstAclsDataSet;
	
	if ( NULL == pstAclsDataGroup )
	{
		return FALSE;
	}

	bRet = FALSE;
	
	for ( pstAclsDataGroup->it = pstAclsDataGroup->vcAclsSetList.begin();
		pstAclsDataGroup->vcAclsSetList.end() != pstAclsDataGroup->it;
		pstAclsDataGroup->it ++ )
	{
		pstAclsDataSet = pstAclsDataGroup->it;
		if ( pstAclsDataSet )
		{
			if ( 0 == _tcsicmp( "acls", pstAclsDataSet->szType ) &&
				_tcslen( pstAclsDataSet->szUser ) > 0 )
			{
				//
				//	...
				//
				if ( 0 == _tcsicmp( "TERMINAL SERVER USER", pstAclsDataSet->szUser ) )
				{
					bLocUserExist = IsLocalAccountExist( pstAclsDataSet->szUser );
				}
				else
				{
					bLocUserExist = TRUE;
				}

				//
				//	...
				//
				Mask = 0;
				if ( 0 == _tcsicmp( "D", pstAclsDataSet->szChk ) )
				{
					//	检测制定用的 DENIED 权限是否存在
					//	修复方法：如果不存在，添加之
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						if ( m_cQueryAclsInfo.GetSpecUserAclsInfo( pstAclsDataSet->szPath, pstAclsDataSet->szUser, ACCESS_DENIED_ACE_TYPE, &Mask ) )
						{
							m_cQueryAclsInfo.CheckPermissions( Mask );
							if ( ! m_cQueryAclsInfo.IsDenyPermissionsExist( Mask ) )
							{
								//	如果 DENY 权限不存在的话，添加之
								bRet = ( bLocUserExist && TRUE );
							}
						}
						else
						{
							//	为查询到指定用户的安全信息，所以要添加之
							bRet = ( bLocUserExist && TRUE );
						}
					}
				}
				else if ( 0 == _tcsicmp( "E", pstAclsDataSet->szChk ) )
				{
					//	检测制定用的 ALLOWED 权限是否存在
					//	修复方法：如果存在，删除之
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						if ( m_cQueryAclsInfo.GetSpecUserAclsInfo( pstAclsDataSet->szPath, pstAclsDataSet->szUser, ACCESS_ALLOWED_ACE_TYPE, &Mask ) ||
							m_cQueryAclsInfo.GetSpecUserAclsInfo( pstAclsDataSet->szPath, pstAclsDataSet->szUser, ACCESS_DENIED_ACE_TYPE, &Mask ) )
						{
							bRet = ( bLocUserExist && TRUE );
						}
					}
				}
				else if ( 0 == _tcsicmp( "N", pstAclsDataSet->szChk ) )
				{
					//	检测指定用户的 ALLOWED 权限是否存在
					//	修复方法：不存在，添加之
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						if ( m_cQueryAclsInfo.GetSpecUserAclsInfo( pstAclsDataSet->szPath, pstAclsDataSet->szUser, ACCESS_ALLOWED_ACE_TYPE, &Mask ) )
						{
							m_cQueryAclsInfo.CheckPermissions( Mask );
							if ( ! m_cQueryAclsInfo.IsAllowPermissionsExist( Mask ) )
							{
								bRet = ( bLocUserExist && TRUE );
							}
						}
						else
						{
							bRet = ( bLocUserExist && TRUE );
						}
					}
				}
			}
			else if ( 0 == _tcsicmp( "exec", pstAclsDataSet->szType ) )
			{
				if ( 0 == _tcsicmp( "EDIR", pstAclsDataSet->szChk ) ||
					0 == _tcsicmp( "EDIRRN", pstAclsDataSet->szChk ) )
				{
					//	检测指定目录是否存在
					//	修复方法：如果存在，删除之
					if ( PathIsDirectory( pstAclsDataSet->szPath ) )
					{
						bRet = TRUE;
					}
				}
				else if ( 0 == _tcsicmp( "EFILE", pstAclsDataSet->szChk ) )
				{
					//	检测指定文件是否存在
					//	修复方法：如果存在，删除之
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						bRet = TRUE;
					}
				}
				else if ( 0 == _tcsicmp( "PEBAKF", pstAclsDataSet->szChk ) )
				{
					//	检测指定文件否存在，如果存在并且是 PE 文件的话，则备份之
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						if ( delib_is_pe_file( pstAclsDataSet->szPath ) )
						{
							bRet = TRUE;
						}
					}
				}
				
			}
			else if ( 0 == _tcsicmp( "svc", pstAclsDataSet->szType ) )
			{
				
				//
				//	关于启动代码详解
				//	------------------------------------------------------------
				//
				//	SERVICE_BOOT_START	0x00000000
				//	A device driver started by the system loader.
				//	在系统启动时候，出现 Windows 正在启动的滚动条时候启动
				//
				//	SERVICE_SYSTEM_START	0x00000001
				//	稍后
				//
				//	SERVICE_AUTO_START	0x00000002
				//	在出现登陆框的时候启动
				//	一般用这个较多
				//
				//	SERVICE_DEMAND_START	0x00000003
				//	A service started by the service control manager when a process calls the StartService function.
				//
				//	SERVICE_DISABLED	0x00000004
				//
				if ( 0 == _tcsicmp( "DEMAND", pstAclsDataSet->szChk ) )
				{
					if ( delib_drv_is_exist( pstAclsDataSet->szPath ) )
					{
						if ( SERVICE_DEMAND_START != delib_drv_get_start( pstAclsDataSet->szPath ) )
						{
							bRet = TRUE;
						}
					}
				}
				else if ( 0 == _tcsicmp( "DISABLED", pstAclsDataSet->szChk ) )
				{
					if ( delib_drv_is_exist( pstAclsDataSet->szPath ) )
					{
						if ( SERVICE_DISABLED != delib_drv_get_start( pstAclsDataSet->szPath ) ||
							SERVICE_STOPPED != delib_drv_get_status( pstAclsDataSet->szPath ) )
						{
							bRet = TRUE;
						}
					}
				}
			}
			else if ( 0 == _tcsicmp( "reg", pstAclsDataSet->szType ) )
			{
				if ( 0 == _tcsicmp( "DELETE", pstAclsDataSet->szChk ) )
				{
					if ( IsRegExist( pstAclsDataSet ) )
					{
						bRet = TRUE;
					}
				}
				else if ( 0 == _tcsicmp( "ADD", pstAclsDataSet->szChk ) )
				{
					if ( ! IsRegExist( pstAclsDataSet ) )
					{
						bRet = TRUE;
					}
				}
			}
			else if ( 0 == _tcsicmp( "regsvr32", pstAclsDataSet->szType ) )
			{
				if ( 0 == _tcsicmp( "INSTALL", pstAclsDataSet->szChk ) )
				{
					if ( ! IsRegExist( pstAclsDataSet ) )
					{
						bRet = TRUE;
					}
				}
				else if ( 0 == _tcsicmp( "UNINSTALL", pstAclsDataSet->szChk ) )
				{
					if ( IsRegExist( pstAclsDataSet ) )
					{
						bRet = TRUE;
					}
				}
			}
		}

		if ( bRet )
		{
			break;
		}
	}
	
	return bRet;
}

VOID CTabSecurityCfgBase::SetFixAclsProblemsStatus( BOOL bContinue )
{
	m_bFixAclsProblemsContinue = bContinue;
}
BOOL CTabSecurityCfgBase::FixAclsProblemsInGroup( STPARSECFGDATAGROUP * pstAclsDataGroup )
{
	BOOL bRet;
	ACCESS_MASK Mask;
	STPARSECFGDATASET * pstAclsDataSet;
	SYSTEMTIME st;
	TCHAR szTemp[ MAX_PATH ];
	TCHAR szCmdLine[ 1024 ];


	if ( NULL == pstAclsDataGroup )
	{
		return FALSE;
	}

	//	..
	bRet = FALSE;

	for ( pstAclsDataGroup->it = pstAclsDataGroup->vcAclsSetList.begin();
		pstAclsDataGroup->vcAclsSetList.end() != pstAclsDataGroup->it;
		pstAclsDataGroup->it ++ )
	{
		if ( ! m_bFixAclsProblemsContinue )
		{
			break;
		}
		
		pstAclsDataSet = pstAclsDataGroup->it;
		if ( pstAclsDataSet )
		{
			if ( 0 == _tcsicmp( "acls", pstAclsDataSet->szType ) &&
				_tcslen( pstAclsDataSet->szUser ) > 0 )
			{
				if ( 0 == _tcsicmp( "D", pstAclsDataSet->szChk ) )
				{
					//
					//	修复 DENIED 不存在的话，添加之
					//
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						_sntprintf( szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s \"%s\" %s", pstAclsDataSet->szCmd, pstAclsDataSet->szPath, pstAclsDataSet->szParam );
						
						Mask = 0;
						if ( m_cQueryAclsInfo.GetSpecUserAclsInfo( pstAclsDataSet->szPath, pstAclsDataSet->szUser, ACCESS_DENIED_ACE_TYPE, &Mask ) )
						{
							if ( ! m_cQueryAclsInfo.IsDenyPermissionsExist( Mask ) )
							{
								delib_run_block_process_ex( szCmdLine, FALSE );
							}
						}
						else
						{
							delib_run_block_process_ex( szCmdLine, FALSE );
						}
					}
				}
				else if ( 0 == _tcsicmp( "E", pstAclsDataSet->szChk ) )
				{
					//
					//	修复 ALLOWED 或者 DENIED 存在的话，删除
					//
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						_sntprintf( szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s \"%s\" %s", pstAclsDataSet->szCmd, pstAclsDataSet->szPath, pstAclsDataSet->szParam );

						Mask = 0;
						if ( m_cQueryAclsInfo.GetSpecUserAclsInfo( pstAclsDataSet->szPath, pstAclsDataSet->szUser, ACCESS_ALLOWED_ACE_TYPE, &Mask ) ||
							m_cQueryAclsInfo.GetSpecUserAclsInfo( pstAclsDataSet->szPath, pstAclsDataSet->szUser, ACCESS_DENIED_ACE_TYPE, &Mask ) )
						{
							//MessageBox( NULL, szCmdLine, "", MB_ICONWARNING );
							delib_run_block_process_ex( szCmdLine, FALSE );
						}
					}
				}
				else if ( 0 == _tcsicmp( "N", pstAclsDataSet->szChk ) )
				{
					//
					//	修复 ALLOWED 如果不存在，添加之
					//
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						_sntprintf( szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s \"%s\" %s", pstAclsDataSet->szCmd, pstAclsDataSet->szPath, pstAclsDataSet->szParam );
						
						if ( m_cQueryAclsInfo.GetSpecUserAclsInfo( pstAclsDataSet->szPath, pstAclsDataSet->szUser, ACCESS_ALLOWED_ACE_TYPE, &Mask ) )
						{
							if ( ! m_cQueryAclsInfo.IsAllowPermissionsExist( Mask ) )
							{
								delib_run_block_process_ex( szCmdLine, FALSE );
							}
						}
						else
						{
							delib_run_block_process_ex( szCmdLine, FALSE );
						}
					}
				}
			}
			else if ( 0 == _tcsicmp( "exec", pstAclsDataSet->szType ) )
			{
				_sntprintf( szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s \"%s\" %s", pstAclsDataSet->szCmd, pstAclsDataSet->szPath, pstAclsDataSet->szParam );
				
				if ( 0 == _tcsicmp( "EDIR", pstAclsDataSet->szChk ) )
				{
					//
					//	修复 如果目录存在的话，删除之
					//
					if ( PathIsDirectory( pstAclsDataSet->szPath ) )
					{
						RecurseRemoveDirectory( pstAclsDataSet->szPath );
						//RemoveDirectory( pstAclsDataSet->szPath );
						//delib_run_block_process_ex( szCmdLine, FALSE );
					}
				}
				else if ( 0 == _tcsicmp( "EDIRRN", pstAclsDataSet->szChk ) )
				{
					//
					//	修复 如果目录存在的话，删除之
					//
					if ( '\\' == pstAclsDataSet->szPath[ _tcslen(pstAclsDataSet->szPath) - 1 ] ||
						'/' == pstAclsDataSet->szPath[ _tcslen(pstAclsDataSet->szPath) - 1 ] )
					{
						pstAclsDataSet->szPath[ _tcslen(pstAclsDataSet->szPath) - 1 ] = 0;
					}
					GetLocalTime( &st );
					_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("%s_vwbak[%04d-%02d-%02d %02d,%02d,%02d]"),
						pstAclsDataSet->szPath,
						st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
					RenameFileOrDirectory( pstAclsDataSet->szPath, szTemp );
				}
				else if ( 0 == _tcsicmp( "EFILE", pstAclsDataSet->szChk ) )
				{
					//	检测指定文件是否存在
					//	修复方法：如果存在，删除之
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						DeleteFile( pstAclsDataSet->szPath );
					}
				}
				else if ( 0 == _tcsicmp( "PEBAKF", pstAclsDataSet->szChk ) )
				{
					//	检测指定“文件”否存在，如果存在并且是 PE 文件的话，则备份之
					if ( RecurseIsFileExists( pstAclsDataSet->szPath ) )
					{
						if ( delib_is_pe_file( pstAclsDataSet->szPath ) )
						{
							//_sntprintf( szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s %s", pstAclsDataSet->szCmd, pstAclsDataSet->szParam );
							//delib_run_block_process_ex( szCmdLine, TRUE );

							//	...
							CopyFile( pstAclsDataSet->szPath, pstAclsDataSet->szParam, FALSE );
						}
					}
				}
				else if ( _tcsstr( pstAclsDataSet->szCmd, "echo CREATE_NEW_TEMP_TEXTFILE" ) )
				{
					CreateNewTempFile( pstAclsDataSet->szPath );
				}
				else
				{
					delib_run_block_process_ex( szCmdLine, FALSE );
				}
			}
			else if ( 0 == _tcsicmp( "svc", pstAclsDataSet->szType ) )
			{
				//
				//	关于启动代码详解
				//	------------------------------------------------------------
				//
				//	SERVICE_BOOT_START	0x00000000
				//	A device driver started by the system loader.
				//	在系统启动时候，出现 Windows 正在启动的滚动条时候启动
				//
				//	SERVICE_SYSTEM_START	0x00000001
				//	稍后
				//
				//	SERVICE_AUTO_START	0x00000002
				//	在出现登陆框的时候启动
				//	一般用这个较多
				//
				//	SERVICE_DEMAND_START	0x00000003
				//	A service started by the service control manager when a process calls the StartService function.
				//
				//	SERVICE_DISABLED	0x00000004
				//
				if ( 0 == _tcsicmp( "DEMAND", pstAclsDataSet->szChk ) )
				{
					if ( delib_drv_is_exist( pstAclsDataSet->szPath ) )
					{
						if ( SERVICE_DEMAND_START != delib_drv_get_start( pstAclsDataSet->szPath ) )
						{
							delib_drv_set_start( pstAclsDataSet->szPath, SERVICE_DEMAND_START );
							delib_drv_stop( pstAclsDataSet->szPath );
						}
					}
				}
				else if ( 0 == _tcsicmp( "DISABLED", pstAclsDataSet->szChk ) )
				{
					if ( delib_drv_is_exist( pstAclsDataSet->szPath ) )
					{
						if ( SERVICE_DISABLED != delib_drv_get_start( pstAclsDataSet->szPath ) ||
							SERVICE_STOPPED != delib_drv_get_status( pstAclsDataSet->szPath ) )
						{
							delib_drv_set_start( pstAclsDataSet->szPath, SERVICE_DISABLED );
							delib_drv_stop( pstAclsDataSet->szPath );
						}
					}
				}
			}
			else if ( 0 == _tcsicmp( "reg", pstAclsDataSet->szType ) )
			{
				//
				//	fix ...
				//
				if ( 0 == _tcsicmp( "DELETE", pstAclsDataSet->szChk ) )
				{
					RegDelete( pstAclsDataSet );
				}
				else if ( 0 == _tcsicmp( "ADD", pstAclsDataSet->szChk ) )
				{
					RegAdd( pstAclsDataSet );
				}
			}
			else if ( 0 == _tcsicmp( "regsvr32", pstAclsDataSet->szType ) )
			{
				//
				//	fix ...
				//
				_sntprintf( szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s \"%s\" %s", pstAclsDataSet->szCmd, pstAclsDataSet->szPath, pstAclsDataSet->szParam );

				if ( 0 == _tcsicmp( "INSTALL", pstAclsDataSet->szChk ) )
				{
					delib_run_block_process_ex( szCmdLine, FALSE );
				}
				else if ( 0 == _tcsicmp( "UNINSTALL", pstAclsDataSet->szChk ) )
				{
					delib_run_block_process_ex( szCmdLine, FALSE );
				}
			}
		}
	}

	return TRUE;
}
BOOL CTabSecurityCfgBase::RecoverFixingInGroup( STPARSECFGDATAGROUP * pstAclsDataGroup )
{
	BOOL bRet;
//	ACCESS_MASK Mask;
	STPARSECFGDATASET * pstAclsDataSet;
	TCHAR szCmdLine[ 1024 ];


	if ( NULL == pstAclsDataGroup )
	{
		return FALSE;
	}

	//	..
	bRet = FALSE;

	for ( pstAclsDataGroup->it = pstAclsDataGroup->vcAclsSetList.begin();
		pstAclsDataGroup->vcAclsSetList.end() != pstAclsDataGroup->it;
		pstAclsDataGroup->it ++ )
	{
		if ( ! m_bFixAclsProblemsContinue )
		{
			break;
		}
		
		pstAclsDataSet = pstAclsDataGroup->it;
		if ( pstAclsDataSet )
		{
			if ( 0 == _tcsicmp( "svc", pstAclsDataSet->szType ) )
			{
				//
				//	关于启动代码详解
				//	------------------------------------------------------------
				//
				//	SERVICE_BOOT_START	0x00000000
				//	A device driver started by the system loader.
				//	在系统启动时候，出现 Windows 正在启动的滚动条时候启动
				//
				//	SERVICE_SYSTEM_START	0x00000001
				//	稍后
				//
				//	SERVICE_AUTO_START	0x00000002
				//	在出现登陆框的时候启动
				//	一般用这个较多
				//
				//	SERVICE_DEMAND_START	0x00000003
				//	A service started by the service control manager when a process calls the StartService function.
				//
				//	SERVICE_DISABLED	0x00000004
				//
				if ( 0 == _tcsicmp( "DEMAND", pstAclsDataSet->szChk ) )
				{
					delib_drv_set_start( pstAclsDataSet->szPath, SERVICE_AUTO_START );
					delib_drv_start( pstAclsDataSet->szPath );
				}
				else if ( 0 == _tcsicmp( "DISABLED", pstAclsDataSet->szChk ) )
				{
					delib_drv_set_start( pstAclsDataSet->szPath, SERVICE_AUTO_START );
					delib_drv_start( pstAclsDataSet->szPath );
				}
			}
			else if ( 0 == _tcsicmp( "reg", pstAclsDataSet->szType ) )
			{
				//	c:\Windows\regedit.exe /e 1.reg HKEY_CLASSES_ROOT\WScript.Shell
				if ( 0 == _tcsicmp( "ADD", pstAclsDataSet->szChk ) )
				{
					//	删除 添加的注册表项
					RegDelete( pstAclsDataSet );
				}
			}
			else if ( 0 == _tcsicmp( "regsvr32", pstAclsDataSet->szType ) )
			{
				//
				//	recover ...
				//
				if ( 0 == _tcsicmp( "INSTALL", pstAclsDataSet->szChk ) )
				{
					_sntprintf( szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s \"%s\" %s", pstAclsDataSet->szCmd, pstAclsDataSet->szPath, "/s /u" );
					delib_run_block_process_ex( szCmdLine, FALSE );
				}
				else if ( 0 == _tcsicmp( "UNINSTALL", pstAclsDataSet->szChk ) )
				{
					_sntprintf( szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s \"%s\" %s", pstAclsDataSet->szCmd, pstAclsDataSet->szPath, "/s" );
					delib_run_block_process_ex( szCmdLine, FALSE );
				}
			}
		}
	}

	return TRUE;
}


VOID CTabSecurityCfgBase::SetFlashFixingIcon( CListCtrl * plistCtrl, LONG lnListRowIndex, LONG lnListColumnIndex )
{
	if ( NULL == plistCtrl )
	{
		return;
	}

	m_plistCtrl		= plistCtrl;
	m_lnListRowIndex	= lnListRowIndex;
	m_lnListColumnIndex	= lnListColumnIndex;
}

BOOL CTabSecurityCfgBase::GetServiceStatusText( LPCTSTR lpcszSvcName, LPTSTR lpszSvcDesc, DWORD dwSize )
{
	BOOL bRet;
	LONG lnSvcStatus;

	if ( NULL == lpcszSvcName || NULL == lpszSvcDesc || 0 == dwSize )
	{
		return FALSE;
	}
	if ( 0 == _tcslen( lpcszSvcName ) )
	{
		return FALSE;
	}

	bRet = FALSE;

	lnSvcStatus = delib_drv_get_status( lpcszSvcName );
	if ( SERVICE_RUNNING == lnSvcStatus )
	{
		bRet = TRUE;
		_sntprintf( lpszSvcDesc, dwSize/sizeof(TCHAR)-1, _T("已启动") );
	}
	else if ( SERVICE_STOPPED == lnSvcStatus )
	{
		bRet = TRUE;
		_sntprintf( lpszSvcDesc, dwSize/sizeof(TCHAR)-1, _T("已停止") );
	}
	else if ( SERVICE_PAUSED == lnSvcStatus )
	{
		bRet = TRUE;
		_sntprintf( lpszSvcDesc, dwSize/sizeof(TCHAR)-1, _T("暂停") );
	}
	else
	{
		bRet = TRUE;
		_sntprintf( lpszSvcDesc, dwSize/sizeof(TCHAR)-1, _T("未知") );
	}

	return bRet;
}

/**
 *	@ Public
 *	获取本地所有帐号（包括组名）
 */
BOOL CTabSecurityCfgBase::InitLocalAccounts()
{
	BOOL bRet;
	LPBYTE lpbtBufferGroups;
	UINT uPrefMaxLenGroups;
	DWORD dwEntriesReadGroups;
	DWORD dwTotalEntriesGroups;
	LOCALGROUP_INFO_1 * pstGroupInfo1;
	
	LPBYTE lpbtBufferMembers;
	UINT uPrefMaxLenMembers;
	DWORD dwEntriesReadMembers;
	DWORD dwTotalEntriesMembers;
	LOCALGROUP_MEMBERS_INFO_1 * pstMemberInfo1;
	
	UINT i;
	UINT j;

	STTSCBLOCALGROUP stTSCBGroup;
	STTSCBLOCALUSER stTSCBUser;


	//	...
	m_vcLocalGroups.clear();

	//	...
	bRet = FALSE;

	try
	{
		//	...
		lpbtBufferGroups	= NULL;
		uPrefMaxLenGroups	= 0xFFFFFFFF;
		dwEntriesReadGroups	= 0;
		dwTotalEntriesGroups	= 0;

		if ( NERR_Success == 
			NetLocalGroupEnum( NULL, 1, &lpbtBufferGroups, uPrefMaxLenGroups, &dwEntriesReadGroups, &dwTotalEntriesGroups, NULL ) )
		{
			for ( i = 0; i < dwTotalEntriesGroups; i ++ )
			{
				pstGroupInfo1 = (LOCALGROUP_INFO_1*)( lpbtBufferGroups + sizeof(LOCALGROUP_INFO_1) * i );
				if ( pstGroupInfo1 )
				{
					memset( &stTSCBGroup, 0, sizeof(stTSCBGroup) );
					stTSCBGroup.vcUsers.clear();

					WideCharToMultiByte( CP_ACP, 0, pstGroupInfo1->lgrpi1_name, (int)wcslen(pstGroupInfo1->lgrpi1_name), stTSCBGroup.szGroupName, sizeof(stTSCBGroup.szGroupName), NULL, NULL );

					//	...
					lpbtBufferMembers	= NULL;
					uPrefMaxLenMembers	= 0xFFFFFFFF;
					dwEntriesReadMembers	= 0;
					dwTotalEntriesMembers	= 0;

					if ( NERR_Success ==
						NetLocalGroupGetMembers( NULL, pstGroupInfo1->lgrpi1_name, 1, &lpbtBufferMembers, uPrefMaxLenMembers, &dwEntriesReadMembers, &dwTotalEntriesMembers, NULL ) )
					{
						for ( j = 0; j < dwTotalEntriesMembers; j ++)
						{
							pstMemberInfo1 = (LOCALGROUP_MEMBERS_INFO_1*)( lpbtBufferMembers + sizeof(LOCALGROUP_MEMBERS_INFO_1) * j );
							if ( pstMemberInfo1 )
							{
								bRet = TRUE;

								//	..
								memset( &stTSCBUser, 0, sizeof(stTSCBUser) );
								WideCharToMultiByte( CP_ACP, 0, pstMemberInfo1->lgrmi1_name, (int)wcslen(pstMemberInfo1->lgrmi1_name), stTSCBUser.szUser, sizeof(stTSCBUser.szUser), NULL, NULL );

								//	push it into ...
								stTSCBGroup.vcUsers.push_back( stTSCBUser );
							}
						}

						NetApiBufferFree( lpbtBufferMembers );
						lpbtBufferMembers = NULL;
					}

					//	push it into ...
					m_vcLocalGroups.push_back( stTSCBGroup );
				}
			}
			
			NetApiBufferFree( lpbtBufferGroups );
			lpbtBufferGroups = NULL;
		}
	}
	catch ( ... )
	{
	}

	//	...
	return bRet;
}

/**
 *	@ Public
 *	检测指定用户名是否存在
 */
BOOL CTabSecurityCfgBase::IsLocalAccountExist( LPCTSTR lpcszUser )
{
	BOOL bRet;
	STTSCBLOCALGROUP * pstTSCBGroup;
	STTSCBLOCALUSER * pstTSCBUser;

	if ( NULL == lpcszUser || 0 == lpcszUser[0] )
	{
		return FALSE;
	}
	if ( ! m_bInitLocalAccountsSucc )
	{
		return FALSE;
	}
	if ( 0 == m_vcLocalGroups.size() )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		for ( m_itLG = m_vcLocalGroups.begin(); m_vcLocalGroups.end() != m_itLG; m_itLG ++ )
		{
			pstTSCBGroup = m_itLG;
			if ( pstTSCBGroup )
			{
				for ( pstTSCBGroup->it = pstTSCBGroup->vcUsers.begin(); pstTSCBGroup->vcUsers.end() != pstTSCBGroup->it; pstTSCBGroup->it ++ )
				{
					pstTSCBUser = pstTSCBGroup->it;
					if ( pstTSCBUser )
					{
						if ( 0 == _tcsicmp( pstTSCBUser->szUser, lpcszUser ) )
						{
							bRet = TRUE;
							break;
						}
					}
				}
			}

			if ( bRet )
			{
				break;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	//	...
	return bRet;
}

/**
 *	@ Public
 *	检测文件是否存在，支持 *.* 检测
 */
BOOL CTabSecurityCfgBase::RecurseIsFileExists( LPCTSTR lpcszFilename )
{
	BOOL bRet;
	INT  nLen;
	HANDLE hFindFile;
	WIN32_FIND_DATA stFindFileData;

	if ( NULL == lpcszFilename || 0 == lpcszFilename[ 0 ] )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		nLen = _tcslen( lpcszFilename );
		if ( _tcsstr( lpcszFilename, _T("*.*") ) &&
			0 == _tcsnicmp( lpcszFilename + nLen - 3, _T("*.*"), 3 ) )
		{
			hFindFile = FindFirstFile( lpcszFilename, &stFindFileData );
			if ( INVALID_HANDLE_VALUE != hFindFile )
			{
				do
				{
					if ( 0 == _tcsicmp( stFindFileData.cFileName, "." ) || 0 == _tcsicmp( stFindFileData.cFileName, ".." ) )
					{
						continue;
					}
					
					if ( FILE_ATTRIBUTE_DIRECTORY == ( FILE_ATTRIBUTE_DIRECTORY & stFindFileData.dwFileAttributes ) )
					{
					}
					else
					{
						//	有文件存在
						bRet = TRUE;
						break;
					}

				}while( FindNextFile( hFindFile, &stFindFileData ) );

				FindClose( hFindFile );
				hFindFile = NULL;
			}
		}
		else
		{
			bRet = PathFileExists( lpcszFilename );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}


/**
 *	@ Public
 *	递归删除目录
 *	lpcszDir 必须以'\\'结尾，如"C:\\windows\\"
 */
BOOL CTabSecurityCfgBase::RecurseRemoveDirectory( LPCTSTR lpcszDir, int nType /* = RECURSE_REMOVE_ALL */)
{
	BOOL bReturn;
	HANDLE hFindFile;
	WIN32_FIND_DATA stFindFileData;
	TCHAR szToFind[ MAX_PATH ];
	TCHAR szNewSource[MAX_PATH];
	TCHAR szFile[MAX_PATH];

	if ( NULL == lpcszDir || 0 == lpcszDir[0] )
	{
		return FALSE;
	}

	//	...
	bReturn = TRUE;


	memset( szToFind, 0, sizeof(szToFind) );
	if ( '\\' != lpcszDir[ _tcslen(lpcszDir) - 1 ] )
	{
		_sntprintf( szToFind, sizeof(szToFind)/sizeof(TCHAR)-1, "%s\\%s", lpcszDir, "*.*" );
	}
	else
	{
		_sntprintf( szToFind, sizeof(szToFind)/sizeof(TCHAR)-1, "%s%s", lpcszDir, "*.*" );
	}

	__try
	{
		hFindFile = FindFirstFile( szToFind, &stFindFileData );
		if ( INVALID_HANDLE_VALUE != hFindFile )
		{
			//bReturn = TRUE;

			do
			{
				if ( ! strcmp( stFindFileData.cFileName, "." ) )
				{
					continue;
				}
				if ( ! strcmp( stFindFileData.cFileName, ".." ) )
				{
					continue;
				}
				if ( FILE_ATTRIBUTE_DIRECTORY == ( FILE_ATTRIBUTE_DIRECTORY & stFindFileData.dwFileAttributes ) )
				{
					memset( szNewSource, 0, sizeof(szNewSource) );
					_sntprintf( szNewSource, sizeof(szNewSource)/sizeof(TCHAR)-1, _T("%s%s\\"), lpcszDir, stFindFileData.cFileName );
					bReturn = RecurseRemoveDirectory( szNewSource, nType );
				}
				else
				{
					memset( szFile, 0, sizeof(szFile) );
					_sntprintf( szFile, sizeof(szFile)/sizeof(TCHAR)-1, "%s%s", lpcszDir, stFindFileData.cFileName );
//	#pragma warning(disable:4800)
					if ( RECURSE_REMOVE_ALL == nType || RECURSE_REMOVE_FILE == nType )
					{
						bReturn = DeleteFile( szFile );
					}
//	#pragma warning(default:4800)
				}

			}while( FindNextFile( hFindFile, &stFindFileData ) );

			FindClose( hFindFile );
			hFindFile = NULL;
		}
		
//#pragma warning(disable:4800)
		if ( RECURSE_REMOVE_ALL == nType || RECURSE_REMOVE_DIR == nType )
		{
			bReturn = RemoveDirectory( lpcszDir );
		}
//#pragma warning(default:4800)
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bReturn = FALSE;
		if ( INVALID_HANDLE_VALUE != hFindFile )
		{
			FindClose(hFindFile);
		}
		//assert(false);
	}

	return bReturn;
}


/**
 *	@ Private
 *	拼接文件
 */
BOOL CTabSecurityCfgBase::SpellRegisterFile( LPCTSTR lpcszDestFile, LPCTSTR lpcszSrcFile )
{
	BOOL bRet;
	WCHAR wszLine[ 4096 ];
	FILE * fpSrc;
	FILE * fpDest;

	if ( NULL == lpcszDestFile || NULL == lpcszSrcFile )
	{
		return FALSE;
	}
	if ( 0 == lpcszDestFile[0] || 0 == lpcszSrcFile[0] )
	{
		return FALSE;
	}

	bRet = FALSE;

	fpDest = fopen( lpcszDestFile, "ab" );
	if ( fpDest )
	{
		fpSrc = fopen( lpcszSrcFile, "rb" );
		if ( fpSrc )
		{
			while( ! feof( fpSrc ) )
			{
				memset( wszLine, 0, sizeof(wszLine) );
				fgetws( wszLine, sizeof(wszLine)/sizeof(TCHAR), fpSrc );

				fputws( wszLine, fpDest );
			}

			fclose( fpSrc );
			fpSrc = NULL;
		}

		fclose( fpDest );
		fpDest = NULL;
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//	Private




unsigned __stdcall CTabSecurityCfgBase::_threadFlashFixingIcon( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CTabSecurityCfgBase * pThis = (CTabSecurityCfgBase*)arglist;
		if ( pThis )
		{
			pThis->FlashFixingIconProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CTabSecurityCfgBase::FlashFixingIconProc()
{
	LONG lnCount;
	LONG lnImgIndex;
	
	lnCount = 0;
	while( m_cThSleepFlashFixingIcon.IsContinue() )
	{
		if ( m_plistCtrl &&
			-1 != m_lnListRowIndex &&
			-1 != m_lnListColumnIndex )
		{
			lnImgIndex = lnCount % 6;
			m_plistCtrl->SetItem( m_lnListRowIndex, m_lnListColumnIndex, LVIF_IMAGE, NULL, 4 + lnImgIndex, 0, 0, 0 );

			lnCount ++;
			if ( lnCount < 0 )
			{
				lnCount = 0;
			}
		}
		
		m_cThSleepFlashFixingIcon.EventSleep( 100 );
	}
}

//判断是否是PE文件结构
BOOL CTabSecurityCfgBase::IsPeFile( LPCTSTR lpcszFilename )
{
	BOOL bRet;
	HANDLE hFile;
	HANDLE hFileMap;
	LPVOID lpMemory;
	DWORD dwFilesize;
	PIMAGE_DOS_HEADER  pstDosHeader;
	PIMAGE_NT_HEADERS  pstNtHeader;

	if ( NULL == lpcszFilename || 0 == lpcszFilename[0] || ! RecurseIsFileExists( lpcszFilename ) )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		//	...
		hFile = CreateFile
			(
				lpcszFilename,		//	open  pe file 
				GENERIC_READ,		//	open for reading 
				FILE_SHARE_READ,	//	share for reading 
				NULL,			//	no security 
				OPEN_EXISTING,		//	existing file only 
				FILE_ATTRIBUTE_NORMAL,	//	normal file 
				NULL			//	no attr. template 
			);
		if ( INVALID_HANDLE_VALUE != hFile )
		{
			dwFilesize = GetFileSize( hFile, NULL );
			if ( dwFilesize > sizeof(IMAGE_DOS_HEADER) )
			{
				hFileMap = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
				if ( hFileMap )
				{
					lpMemory = MapViewOfFile( hFileMap, FILE_MAP_READ, NULL, NULL, NULL );
					if ( lpMemory )
					{
						//
						//	check if PE file
						//
						pstDosHeader = (PIMAGE_DOS_HEADER)lpMemory;
						if ( IMAGE_DOS_SIGNATURE == pstDosHeader->e_magic )
						{
							pstNtHeader = (PIMAGE_NT_HEADERS32)( (DWORD)pstDosHeader + pstDosHeader->e_lfanew );
							if ( IMAGE_NT_SIGNATURE == pstNtHeader->Signature )
							{
								bRet = TRUE;
							}
						}

						UnmapViewOfFile( lpMemory );
						lpMemory = NULL;
					}

					CloseHandle( hFileMap );
					hFileMap = NULL;
				}
			}

			CloseHandle( hFile );
			hFile = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

BOOL CTabSecurityCfgBase::CreateNewTempFile( LPCTSTR lpcszFilename )
{
	BOOL bRet;
	FILE * fp;

	if ( NULL == lpcszFilename || 0 == lpcszFilename[0] )
	{
		return FALSE;
	}

	bRet = FALSE;

	if ( RecurseIsFileExists( lpcszFilename ) )
	{
		DeleteFile( lpcszFilename );
	}

	fp = fopen( lpcszFilename, "wb" );
	if ( fp )
	{
		bRet = TRUE;

		fputs( "THIS IS A TEMP FILE, CREATED BY VWFIREWALL.   WWW.VIDUN.COM", fp );
		fclose( fp );
		fp = NULL;
	}

	return bRet;
}

BOOL CTabSecurityCfgBase::IsRegExist( STPARSECFGDATASET * pstAclsDataSet )
{
	BOOL bRet;
	DWORD dwType;
	TCHAR szValue[ MAX_PATH ];
	DWORD dwValue;
	DWORD dwRegV;
	DWORD dwValueSize;


	if ( NULL == pstAclsDataSet )
	{
		return FALSE;
	}
	if ( 0 == pstAclsDataSet->hkRoot )
	{
		return FALSE;
	}

	bRet = FALSE;
	HKEY hkResult;

	if ( _tcslen( pstAclsDataSet->szRegVName ) > 0 )
	{
		if ( _tcslen( pstAclsDataSet->szRegVType ) && _tcslen( pstAclsDataSet->szRegV ) )
		{
			if ( REG_DWORD == pstAclsDataSet->lnRegVType )
			{
				dwRegV		= StrToInt( pstAclsDataSet->szRegV );
				dwType		= pstAclsDataSet->lnRegVType;
				dwValue		= 0;
				dwValueSize	= sizeof(dwValue);
				if ( ERROR_SUCCESS ==
					SHGetValue( pstAclsDataSet->hkRoot, pstAclsDataSet->szRegPath, pstAclsDataSet->szRegVName, &dwType, &dwValue, &dwValueSize ) )
				{
					if ( dwRegV == dwValue )
					{
						bRet = TRUE;
					}
				}
			}
			else if ( REG_SZ == pstAclsDataSet->lnRegVType )
			{
				memset( szValue, 0, sizeof(szValue) );
				dwType		= pstAclsDataSet->lnRegVType;
				dwValueSize	= sizeof(szValue);
				if ( ERROR_SUCCESS ==
					SHGetValue( pstAclsDataSet->hkRoot, pstAclsDataSet->szRegPath, pstAclsDataSet->szRegVName, &dwType, szValue, &dwValueSize ) )
				{
					if ( 0 == _tcsicmp( szValue, pstAclsDataSet->szRegV ) )
					{
						bRet = TRUE;
					}
				}
			}
			else
			{
				dwType = pstAclsDataSet->lnRegVType;
				if ( ERROR_SUCCESS ==
					SHGetValue( pstAclsDataSet->hkRoot, pstAclsDataSet->szRegPath, pstAclsDataSet->szRegVName, &dwType, NULL, NULL ) )
				{
					bRet = TRUE;
				}
			}
		}
		else
		{
			dwType = pstAclsDataSet->lnRegVType;
			if ( ERROR_SUCCESS ==
				SHGetValue( pstAclsDataSet->hkRoot, pstAclsDataSet->szRegPath, pstAclsDataSet->szRegVName, &dwType, NULL, NULL ) )
			{
				bRet = TRUE;
			}
		}
	}
	else
	{
		//
		//	检查 REG PATH 是否存在即可
		//		
		if ( ERROR_SUCCESS == RegOpenKey( pstAclsDataSet->hkRoot, pstAclsDataSet->szRegPath, &hkResult ) )
		{
			bRet = TRUE;
			RegCloseKey( hkResult );
		}
	}


	return bRet;
}

BOOL CTabSecurityCfgBase::RegDelete( STPARSECFGDATASET * pstAclsDataSet )
{
	BOOL bRet;
	
	if ( NULL == pstAclsDataSet )
	{
		return FALSE;
	}
	if ( 0 == pstAclsDataSet->hkRoot )
	{
		return FALSE;
	}

	bRet = FALSE;

	if ( _tcslen( pstAclsDataSet->szRegVName ) > 0 )
	{
		//	Deletes a named value from the specified registry key.		
		if ( ERROR_SUCCESS ==
			SHDeleteValue( pstAclsDataSet->hkRoot, pstAclsDataSet->szRegPath, pstAclsDataSet->szRegVName ) )
		{
			bRet = TRUE;
		}
	}
	else
	{
		//	Deletes a subkey and all its descendants. The function will remove the key and all of the key's values from the registry.
		if ( ERROR_SUCCESS ==
			SHDeleteKey( pstAclsDataSet->hkRoot, pstAclsDataSet->szRegPath ) )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CTabSecurityCfgBase::RegAdd( STPARSECFGDATASET * pstAclsDataSet )
{
	BOOL  bRet;
	DWORD dwValue;
	
	if ( NULL == pstAclsDataSet )
	{
		return FALSE;
	}
	if ( 0 == pstAclsDataSet->hkRoot )
	{
		return FALSE;
	}
	
	bRet = FALSE;


	if ( _tcslen( pstAclsDataSet->szRegVName ) > 0 &&
		-1 != pstAclsDataSet->lnRegVType )
	{
		if ( REG_DWORD == pstAclsDataSet->lnRegVType )
		{
			dwValue = StrToInt( pstAclsDataSet->szRegV );
			if ( ERROR_SUCCESS == SHSetValue
				(
					pstAclsDataSet->hkRoot,
					pstAclsDataSet->szRegPath,
					pstAclsDataSet->szRegVName,
					pstAclsDataSet->lnRegVType,
					(LPCVOID)(&dwValue),
					sizeof(dwValue)
				)
			)
			{
				bRet = TRUE;
			}
			
		}
		else if ( REG_SZ == pstAclsDataSet->lnRegVType )
		{
			if ( ERROR_SUCCESS == SHSetValue
				(
					pstAclsDataSet->hkRoot,
					pstAclsDataSet->szRegPath,
					pstAclsDataSet->szRegVName,
					pstAclsDataSet->lnRegVType,
					(LPCVOID)pstAclsDataSet->szRegV,
					_tcslen(pstAclsDataSet->szRegV)*sizeof(TCHAR)
				)
			)
			{
				bRet = TRUE;
			}
		}
		else if ( REG_MULTI_SZ == pstAclsDataSet->lnRegVType )
		{
			if ( ERROR_SUCCESS == SHSetValue
				(
					pstAclsDataSet->hkRoot,
					pstAclsDataSet->szRegPath,
					pstAclsDataSet->szRegVName,
					pstAclsDataSet->lnRegVType,
					(LPCVOID)pstAclsDataSet->szRegV,
					_tcslen(pstAclsDataSet->szRegV)*sizeof(TCHAR)
				)
			)
			{
				bRet = TRUE;
			}
		}
	}
	
	return bRet;
}

/**
 *	@ Private
 *	重命名文件或文件夹
 */
BOOL CTabSecurityCfgBase::RenameFileOrDirectory( LPCTSTR lpcszFrom, LPCTSTR lpcszTo )
{
	SHFILEOPSTRUCT stFileOp = {0};

	if ( NULL == lpcszFrom || NULL == lpcszTo )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszFrom ) || ! PathIsDirectory( lpcszFrom ) )
	{
		return FALSE;
	}

	stFileOp.fFlags	= FOF_NOCONFIRMATION;	//	不出现确认对话框 
	stFileOp.pFrom	= lpcszFrom;
	stFileOp.pTo	= lpcszTo;
	stFileOp.wFunc	= FO_RENAME;

	return ( 0 == SHFileOperation( & stFileOp ) );
}