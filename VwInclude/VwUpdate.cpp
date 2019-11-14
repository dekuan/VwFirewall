// VwUpdate.cpp: implementation of the CVwUpdate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwUpdate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwUpdate::CVwUpdate()
{
	m_bIsReady	= FALSE;
	memset( & m_stUpInfo, 0, sizeof( m_stUpInfo ) );
}

CVwUpdate::~CVwUpdate()
{
}

/**
 *	设置基本的更新配置信息
 */
BOOL CVwUpdate::SetUpdateInfo( STUPDATEINFO * pstUpInfo )
{
	if ( pstUpInfo )
	{
		m_stUpInfo = *pstUpInfo;

		if ( PathIsDirectory( m_stUpInfo.szUpdateDir ) )
		{
			//	本地更新配置文件，用来记录本地更新情况
			_sntprintf
			(
				m_stUpInfo.szUpdateCfgIniFile,
				sizeof(m_stUpInfo.szUpdateCfgIniFile)-sizeof(TCHAR),
				"%s\\%s", m_stUpInfo.szUpdateDir, VWUPDATE_UPDATECFGINI
			);
		}

		m_bIsReady = TRUE;

		return TRUE;
	}
	return FALSE;
}

/**
 *	@ Public
 *	开始更新
 */
BOOL CVwUpdate::StartUpdate()
{
	BOOL bRet	= FALSE;

	if ( ! IsReady() )
	{
		return FALSE;
	}

	//
	//	创建临时更新目录
	//
	CreateDirectory( m_stUpInfo.szUpdateDir, NULL );

	//
	//	从服务器获取是否需要升级的信息
	//
	if ( GetUpdateIniFromServer() )
	{
		//
		//	分析是否有需要升级的任务
		//
		if ( CheckUpdateInfo() )
		{
			//
			//	下载所有模块
			//
			if ( DownloadAllModules() )
			{
				//
				//	更新所有模块
				//
				if ( UpdateAllModules() )
				{
					bRet = TRUE;
				}
			}
		}

		//	只要从服务器获取更新配置文件成功，就记录一下最后工作时间，减少服务器压力
		//	一般设置为 24 小时一次更新
		SaveLastWorkTime();
	}

	return bRet;
}

/**
 *	@ Public
 *	清理所有临时数据文件
 *	### 暂时这个函数不能使用，原因是：需要多进程通讯来确认是否有其他进程在使用这些 ini 文件
 */
VOID CVwUpdate::CleanAllTempDataFile()
{
	if ( PathFileExists( m_stUpInfo.szUpdateIniFile ) )
	{
		DeleteFile( m_stUpInfo.szUpdateIniFile );
	}
}


/**
 *	@ 私有函数
 *	判断是否已经准备好
 */
BOOL CVwUpdate::IsReady()
{
	return m_bIsReady;
}


/**
 *	@ 私有函数
 *	从服务器获取是否需要升级的信息
 */
BOOL CVwUpdate::GetUpdateIniFromServer()
{
	if ( ! IsWorkTime() )
	{
		return FALSE;
	}

	TCHAR szError[ MAX_PATH ]	= {0};

	//	先删除原来的文件
	DeleteFile( m_stUpInfo.szUpdateIniFile );

	//	开始下载新的文件
	return CVwHttp::DownloadFile
	(
		m_stUpInfo.szUpdateUrl,
		m_stUpInfo.szUpdateIniFile,
		m_stUpInfo.hMainWnd,
		TRUE,
		szError,
		m_stUpInfo.dwTimeout
	);
}

/**
 *	@ 私有函数
 *	根据本地配置文件，判断当前是否开始向服务器发出升级信息
 */
BOOL CVwUpdate::IsWorkTime()
{
	if ( ! PathFileExists( m_stUpInfo.szUpdateCfgIniFile ) )
	{
		//	配置文件不存在，是工作时间了
		return TRUE;
	}

	BOOL bRet		= FALSE;
	SYSTEMTIME st;
	DWORD dwNowDate		= 0;
	DWORD dwRecDate		= 0;

	//	获取 当前月日
	GetLocalTime( &st );
	dwNowDate = ( st.wMonth * 100 + st.wDay );

	//	读取上次更新时间
	dwRecDate = (DWORD)GetPrivateProfileInt( VWUPDATE_SECTION_UPDATE, VWUPDATE_KEY_LASTWORK, 0, m_stUpInfo.szUpdateCfgIniFile );

	//	如果是今天，就不用工作了
	return ( dwRecDate == dwNowDate ) ? FALSE : TRUE;	
}

/**
 *	@ 私有函数
 *	保存最后工作时间
 */
VOID CVwUpdate::SaveLastWorkTime()
{
	SYSTEMTIME st;
	DWORD dwNowDate			= 0;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	
	//	当前月日
	GetLocalTime( &st );
	dwNowDate = ( st.wMonth * 100 + st.wDay );

	_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%d", dwNowDate );
	WritePrivateProfileString( VWUPDATE_SECTION_UPDATE, VWUPDATE_KEY_LASTWORK, szTemp, m_stUpInfo.szUpdateCfgIniFile );
}


/**
 *	@ 私有函数
 *	分析是否有必要的升级信息
 */
BOOL CVwUpdate::CheckUpdateInfo()
{
	BOOL  bRet				= FALSE;
	BOOL  nUpdateNow			= FALSE;

	if ( PathFileExists( m_stUpInfo.szUpdateIniFile ) )
	{
		nUpdateNow = (BOOL)GetPrivateProfileInt( "config", "update", 0, m_stUpInfo.szUpdateIniFile );
		if ( nUpdateNow )
		{
			//	清理更新列表
			if ( m_vcUpdateList.size() )
			{
				m_vcUpdateList.clear();
			}

			//
			//	解析 INI 信息
			//
			if ( ParseIni( m_stUpInfo.szUpdateIniFile ) )
			{
				bRet = ( m_vcUpdateList.size() ? TRUE : FALSE );
			}
		}
	}

	return bRet;
}


/**
 *	@ 私有函数
 *	解析需要更新的文件列表
 */
BOOL CVwUpdate::ParseIni( LPCTSTR lpszUpdateIni )
{
	//
	//	lpszUpdateIni		- [in] INI 文件路径
	//	RETURN			- TRUE / FALSE
	//	Remark			- 需要更新的文件列表将保存在 m_vcIniList 中
	//

	TCHAR szFileVersion[MAX_PATH]		= {0};		// xxx.dll 等
	TCHAR pszSection[ 32767 ]		= {0};		// Win95 的大小限制
	INT   nSectionLen			= 0;
	STUPDATELIST stUpdateList;

	TCHAR szError[ MAX_PATH ]		= {0};
	CMultiString mstr;
	INT   nNum				= 0;
	TCHAR ** ppList				= NULL;
	TCHAR ** p				= NULL;
	TCHAR szTargetFile[ MAX_PATH ]		= {0};
	TCHAR szFileName[ MAX_PATH ]		= {0};
	TCHAR szUrl[ MAX_PATH ]			= {0};
	STPARSEURL stParseUrl;
	TCHAR szUrlExt[ 32 ]			= {0};
	TCHAR szUrlFile[ MAX_PATH ]		= {0};
	BOOL  bGetUrlFile			= FALSE;


	//	清理更新队列
	if ( m_vcUpdateList.size() )
	{
		m_vcUpdateList.clear();
	}

	//	开始解析 INI
	nSectionLen = GetPrivateProfileSection( VWUPDATE_SECTION_ADD, pszSection, sizeof(pszSection), lpszUpdateIni );
	if ( nSectionLen <= 0 )
		return FALSE;

	ppList = mstr.MultiStringToStringList( pszSection, nSectionLen + 1, &nNum );
	if ( NULL == ppList )
	{
		return FALSE;
	}

	p = ppList;
	while( p && *p )
	{
		nNum = _stscanf( *p, "%[^=]=%s", szFileName, szFileVersion );
		if ( 2 == nNum )
		{
			memset( & stUpdateList, 0, sizeof(stUpdateList) );
			_vwfunc_replace( szFileVersion, "\"", "" );

			//	获取某个文件的信息
			GetPrivateProfileString( szFileName, VWUPDATE_KEY_URL, "", stUpdateList.szUrl, sizeof(stUpdateList.szUrl), lpszUpdateIni );
			stUpdateList.bReg = (BOOL)GetPrivateProfileInt( szFileName, VWUPDATE_KEY_REG, 0, m_stUpInfo.szUpdateIniFile );
			stUpdateList.uExec = (UINT)GetPrivateProfileInt( szFileName, VWUPDATE_KEY_EXEC, 0, m_stUpInfo.szUpdateIniFile );

			if ( 0 == strlen( stUpdateList.szUrl ) )
			{
				continue;
			}

			//	标记已下载成功 = FALSE
			stUpdateList.bDownSucc	= FALSE;

			bGetUrlFile	= FALSE;
			memset( szUrlFile, 0, sizeof( szUrlFile ) );
			if ( _vwfunc_parse_url( stUpdateList.szUrl, strlen( stUpdateList.szUrl ), & stParseUrl ) )
			{
				//	判断一个 URL 指向的是否是一个文件下载
				if ( _vwfunc_get_fileinfo_from_url( stParseUrl.szPath, szUrlExt, sizeof(szUrlExt), szUrlFile, sizeof(szUrlFile) ) )
				{
					bGetUrlFile	= TRUE;
				}
			}

			if ( ! bGetUrlFile || 0 == strlen( szUrlFile ) )
			{
				continue;
			}

			//	下载文件名
			_sntprintf( stUpdateList.szUrlFileName, sizeof(stUpdateList.szUrlFileName)-sizeof(TCHAR), "%s", szUrlFile );

			//	真正下载文件名
			_sntprintf( stUpdateList.szFileName, sizeof(stUpdateList.szFileName)-sizeof(TCHAR), "%s", szFileName );

			//	文件下载临时保存路径
			_sntprintf( stUpdateList.szDownFile, sizeof(stUpdateList.szDownFile)-sizeof(TCHAR), "%s\\%s", m_stUpInfo.szUpdateDir, szUrlFile );

			//	文件解压后的真实文件路径
			_sntprintf( stUpdateList.szRealFile, sizeof(stUpdateList.szRealFile)-sizeof(TCHAR), "%s\\%s", m_stUpInfo.szUpdateDir, szFileName );

			//	目标文件，这个是待升级的目标
			_sntprintf( stUpdateList.szDstFile, sizeof(stUpdateList.szDstFile)-sizeof(TCHAR), "%s\\%s", m_stUpInfo.szDstDir, szFileName );

			//	INI 中指定该文件的最新版本号
			_sntprintf( stUpdateList.szVer, sizeof(stUpdateList.szVer)-sizeof(TCHAR), "%s", szFileVersion );

			//	详细描述信息
			_vwfunc_ini_get_value_with_acpkey
			(
				m_stUpInfo.szUpdateIniFile,
				szFileName,
				VWUPDATE_KEY_DETAIL,
				stUpdateList.szDetail,
				sizeof(stUpdateList.szDetail)
			);

			//	获取本地文件 当前版本号
			_vwfunc_get_file_version( stUpdateList.szDstFile, stUpdateList.szDstFileVer, sizeof( stUpdateList.szDstFileVer ) );

			//	先比较本地文件的版本号是否需要升级
			//	如果需要升级的话才升级，避免做不必要的下载
			if ( stricmp( stUpdateList.szVer, stUpdateList.szDstFileVer ) > 0 )
			{
				//
				//	将此更新推送到待更新队列
				//
				m_vcUpdateList.push_back( stUpdateList );
			}
		}
		p++;
	}

	if ( ppList )
	{
		free( ppList );
	}	

	return TRUE;
}




/**
 *	@ 私有函数
 *	下载所有服务器指定的模块
 */
BOOL CVwUpdate::DownloadAllModules()
{
	BOOL  bDownload			= TRUE;
	TCHAR szError[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};
	TCHAR szFileExt[ MAX_PATH ]	= {0};
	STUPDATELIST * pstUpdateList;
	CVwZip vwzip;
	LONG  lnStep;
	LONG  lnCount;


	//	标记
	bDownload	= TRUE;
	lnStep		= 1;
	lnCount		= m_vcUpdateList.size();

	//
	//	下载列表中所有待下载文件
	//
	for ( m_vcIt = m_vcUpdateList.begin(); m_vcIt != m_vcUpdateList.end(); m_vcIt ++, lnStep ++ )
	{
		pstUpdateList = m_vcIt;

		//	给用户发送提示信息
		//	...

		//	如果文件存在则先删除之
		if ( PathFileExists( pstUpdateList->szDownFile ) )
		{
			DeleteFile( pstUpdateList->szDownFile );
		}

		//	下载文件
		pstUpdateList->bDownSucc = CVwHttp::DownloadFile
					(
						pstUpdateList->szUrl,
						pstUpdateList->szDownFile,
						m_stUpInfo.hMainWnd,
						TRUE,
						szError,
						m_stUpInfo.dwTimeout
					);
		if ( pstUpdateList->bDownSucc )
		{
			//
			//	[ 重要 ]
			//	我们约定一个 ZIP 包里面只能有一个文件，文件名保存在 pstUpdateList->szRealFile
			//

			//	先标记下载成功 = FALSE
			pstUpdateList->bDownSucc = FALSE;

			_sntprintf( szFileExt, sizeof(szFileExt)-sizeof(TCHAR), "%s", PathFindExtension( pstUpdateList->szDownFile ) );
			if ( 0 == stricmp( ".zip", szFileExt ) )
			{
				//	解压文件到临时下载文件保存的目录
				if ( vwzip.UnZipFiles( pstUpdateList->szDownFile, m_stUpInfo.szUpdateDir, TRUE ) )
				{
					//	解压成功后，标记下载成功 = TRUE
					pstUpdateList->bDownSucc = TRUE;

					//	删除 ZIP 包
					DeleteFile( pstUpdateList->szDownFile );
				}
			}
		}

		//	记录下载情况，必须所有模块都下载成功
		bDownload &= pstUpdateList->bDownSucc;
	}

	return bDownload;
}

/**
 *	@ 私有函数
 *	进行更新所有服务器指定的模块
 */
BOOL CVwUpdate::UpdateAllModules()
{
	BOOL bRet			= FALSE;
	CProcSecurityDesc procSdesc;
	TCHAR szError[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};
	STUPDATELIST * pstUpdateList;
	INT  nUpdateFile		= VWUPDATE_UPDATE_FAILED;
	BOOL bNeedReboot		= FALSE;	//	是否需要重启计算机


	//
	//	更新所有文件
	//
	for ( m_vcIt = m_vcUpdateList.begin(); m_vcIt != m_vcUpdateList.end(); m_vcIt ++ )
	{
		pstUpdateList	= m_vcIt;

		//	开始更新一个文件
		nUpdateFile = CVwUpdate::UpdateFile
			(
				pstUpdateList->szRealFile,
				pstUpdateList->szDstFile,
				FALSE,
				pstUpdateList->bReg,
				pstUpdateList->uExec
			);
		if ( VWUPDATE_UPDATE_OK == nUpdateFile )
		{
			//	更新成功

			bRet	= TRUE;

			//	设置安全描述符
			procSdesc.SetSecurityDescForFilter( pstUpdateList->szDstFile, szError );
		}
		else if ( VWUPDATE_UPDATE_RESTART == nUpdateFile )
		{
			//	需要重启
			
			bRet		= TRUE;
			bNeedReboot	= TRUE;

			//	设置安全描述符
			procSdesc.SetSecurityDescForFilter( pstUpdateList->szDstFile, szError );
		}
		else if ( VWUPDATE_UPDATE_FAILED == nUpdateFile )
		{
			//	更新失败

			bRet	= FALSE;

			//	由于是后台更新，所以就忍了，继续下一个
			//break;
		}
	}

	return bRet;
}


/**
 *	执行一个文件更新
 */
UINT CVwUpdate::UpdateFile( LPCTSTR lpszSrc, LPCTSTR lpszDst, BOOL bForceUpdate, BOOL bReg, UINT uExec )
{
	//
	//	lpszSrc			- [in] 源文件
	//	lpszDst			- [in] 目标文件
	//	bForceUpdate		- [in] 是否强制更新
	//	bReg			- [in] 是否需要注册
	//	bRun			- [in] 是否需要运行
	//
	//	RETURN			- 返回是否成功更新状态
	//	REMARK			- 根据文件版本号来更新文件，可以重启注册
	//
/*
#define VWUPDATE_EXEC_FLAG_NONE		0	//	不运行
#define VWUPDATE_EXEC_FLAG_EXEC		1	//	执行一个 EXE 可执行文件
#define VWUPDATE_EXEC_FLAG_LOAD		2	//	是个 DLL 尝试 LoadLibrary，然后执行其导出函数 Run
*/	

	assert( lpszSrc && lpszDst );

	if ( NULL == lpszSrc || NULL == lpszDst )
		return VWUPDATE_UPDATE_FAILED;
	if ( 0 == strlen( lpszSrc ) || 0 == strlen( lpszDst ) )
		return VWUPDATE_UPDATE_FAILED;
	if ( ! PathFileExists( lpszSrc ) )
		return VWUPDATE_UPDATE_FAILED;


	BOOL bIsExe			= FALSE;
	BOOL bUpdate			= FALSE;
	INT  nUpdateStatus		= VWUPDATE_UPDATE_OK;
	TCHAR szSrcVer[MAX_PATH]	= {0};
	TCHAR szDstVer[MAX_PATH]	= {0};

	LPCTSTR pdest = lpszSrc + strlen(lpszSrc) - 4*sizeof(TCHAR);
	if ( _tcsicmp( pdest, ".exe" ) == 0 )
		bIsExe = TRUE;

	if ( bForceUpdate )
	{
		bUpdate = TRUE;
	}
	else
	{
		//	不是强制更新的话要比较版本号
		if ( PathFileExists( lpszDst ) )
		{
			_vwfunc_get_file_version( lpszSrc, szSrcVer, sizeof(szSrcVer) );
			_vwfunc_get_file_version( lpszDst, szDstVer, sizeof(szDstVer) );
			if ( _tcslen(szSrcVer) > 0 && _tcslen(szDstVer) > 0 )	// 如果两个文件都有版本信息
			{
				//	当源文件的版本号大于目标文件时候，需要更新
				if ( _tcscmp( szSrcVer, szDstVer ) > 0 )
				{
					bUpdate = TRUE;
				}
			}
		}
		else
		{
			bUpdate = TRUE;
		}
	}
	
	if ( bUpdate )
	{
		switch( MyMoveFile( lpszSrc, lpszDst, FALSE ) )
		{
		case VWUPDATE_MOVE_FILE_ERROR:
			{
				nUpdateStatus = VWUPDATE_UPDATE_FAILED;
			}
			break;
			
		case VWUPDATE_MOVE_FILE_PENDING:
			{
				nUpdateStatus = VWUPDATE_UPDATE_RESTART;
				if ( bReg )
				{
					TCHAR szTemp[ MAX_PATH * 2 ]	= {0};
					_snprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "regsvr32 /s %s", lpszDst );
					SHSetValue( HKEY_LOCAL_MACHINE, VWUPDATE_REGKEY_RUNONCE, lpszDst, REG_SZ, szTemp, _tcslen(szTemp) );
				}
			}
			break;
			
		default:
			{
				if ( bReg )
				{
					TCHAR szParameter[MAX_PATH]	= {0};
					_sntprintf( szParameter, sizeof(szParameter)-sizeof(TCHAR), "/s %s", lpszDst );
					ShellExecute( GetDesktopWindow(), "open", "regsvr32.exe", szParameter, NULL, SW_SHOW );
				}
				switch( uExec )
				{
				case VWUPDATE_EXEC_FLAG_EXEC:
					{
						if ( bIsExe )
						{
							// WinExec( lpszDst, SW_SHOW );
							_vwfunc_run_block_process( lpszDst );
						}
						else
						{
							if ( IsDllCanbeRun( lpszDst ) )
							{
								TCHAR szCmdLine[ MAX_PATH ] = {0};
								_sntprintf( szCmdLine, sizeof(szCmdLine)-sizeof(TCHAR), "rundll32 %s,Rundll32", lpszDst );
								// WinExec( szCmdLine, SW_SHOW );
								_vwfunc_run_block_process( szCmdLine );
							}
						}
					}
					break;
				case VWUPDATE_EXEC_FLAG_LOAD:
					{
						//	针对 DLL 尝试 LoadLibrary，然后执行其导出函数 Load
						//	...
						if ( IsDllCanbeLoad( lpszDst ) )
						{
							//	运行之 ...
							RunDllLoad( lpszDst, "" );
						}
					}
					break;
				}
			}
			break;
		}
	}
	
	return nUpdateStatus;
}

/**
 *	bRestartMove强制重新启动的时候移动文件
 *	文件路径名必须是短路径名
 */
INT CVwUpdate::MyMoveFile( LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bRestartMove )
{
	BOOL bRet;
	if ( !bRestartMove )
	{
		bRet = CopyFile( lpExistingFileName, lpNewFileName, FALSE );
		if ( TRUE == bRet )
		{
			MyDeleteFile( lpExistingFileName, FALSE ); 
			return VWUPDATE_MOVE_FILE_OK;
		}
	}
	
	//	WriteCnsini( lpExistingFileName, lpNewFileName );
	
	if ( IsWindowsNT() )
	{
		if ( MoveFileEx( lpExistingFileName, lpNewFileName, MOVEFILE_REPLACE_EXISTING | MOVEFILE_DELAY_UNTIL_REBOOT ) )
		{
			return VWUPDATE_MOVE_FILE_PENDING;
		}
	}
	else
	{
		if ( AddDelOrMoveToWinInit(lpExistingFileName, lpNewFileName) )
		{
			return VWUPDATE_MOVE_FILE_PENDING;
		}
	}
	return VWUPDATE_MOVE_FILE_ERROR;
}

/**
 *	重新启动的时候才删除
 */
INT CVwUpdate::MyDeleteFile( LPCTSTR lpFileName, BOOL bBootDelete )
{
	/*
		lpFileName	- [in] 文件全路径
		bBootDelete	- [in] 是否重启计算机后删除
	*/
	BOOL bRet;
	INT  nRet;
	if ( !bBootDelete )
	{
		bRet = DeleteFile( lpFileName );
		if ( TRUE == bRet )
			return VWUPDATE_DELETE_FILE_OK;
		
		nRet = GetLastError();
	}
	else
	{
		nRet = ERROR_ACCESS_DENIED;
	}
	
	if ( ERROR_ACCESS_DENIED == nRet || ERROR_SHARING_VIOLATION == nRet )
	{
		if ( IsWindowsNT() )
		{
			MoveFileEx( lpFileName, NULL, MOVEFILE_DELAY_UNTIL_REBOOT );
			return VWUPDATE_DELETE_FILE_PENDING;
		}
		else
		{
			if ( AddDelOrMoveToWinInit( lpFileName, NULL ) )
				return VWUPDATE_DELETE_FILE_PENDING;
			return VWUPDATE_DELETE_FILE_ERROR;
		}
	}
	else if ( ERROR_FILE_NOT_FOUND == nRet )
		return TRUE;
	
	return TRUE;
}

BOOL CVwUpdate::IsWindowsNT()
{
	OSVERSIONINFO osif;
	osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osif);
	
	if ( osif.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CVwUpdate::GetWinInitFileName( LPTSTR lpszWininitFileName, DWORD dwSize )
{
	TCHAR szTemp[MAX_PATH]	= {0};
	INT   nRet		= 0;
	
	nRet = GetWindowsDirectory( szTemp, sizeof(szTemp) );
	if( 0 == nRet )
		return FALSE;
	if ( szTemp[strlen(szTemp)-sizeof(TCHAR)] != '\\' )
	{
		_tcscat( szTemp, "\\" );
	}
	
	_sntprintf( lpszWininitFileName, dwSize-sizeof(TCHAR), "%s%s", szTemp, VWUPDATE_WININIT_FILE_NAME );
	lpszWininitFileName[ dwSize-sizeof(TCHAR)] = 0;
	return TRUE;
}

BOOL CVwUpdate::AddDelOrMoveToWinInit( LPCTSTR lpExistingFileNameA, LPCTSTR lpNewFileNameA )
{
	TCHAR strWininitFileName[MAX_PATH] = {0};
	if ( FALSE == GetWinInitFileName( strWininitFileName,sizeof(strWininitFileName)) )
		return FALSE;
	
	FILE* pWininitFile;
	pWininitFile=fopen(strWininitFileName,"a+");
	if(NULL == pWininitFile)
		return FALSE;
	
	CHAR lpExistingFileName[MAX_PATH];
	CHAR lpNewFileName[MAX_PATH];
	GetShortPathName( lpExistingFileNameA, lpExistingFileName, sizeof(lpExistingFileName) );
	if( lpNewFileNameA )
	{
		int nRet = GetShortPathName( lpNewFileNameA, lpNewFileName, sizeof(lpNewFileName) );
		if( nRet == 0 || nRet > sizeof(lpNewFileName) )
		{
			memset( lpNewFileName,0,sizeof(lpNewFileName) );
			_snprintf( lpNewFileName,sizeof(lpNewFileName)-1, "%s", lpNewFileNameA );
		}
	}
	char strLineBuf[MAX_PATH*2+10];
	bool bFindRenameSection;
	bFindRenameSection=false;
	rewind(pWininitFile);
	while(NULL != (fgets(strLineBuf,sizeof(strLineBuf),pWininitFile)))
	{
		strLineBuf[sizeof(strLineBuf) -1] = '\0';
		
		if ( ! _strnicmp( VWUPDATE_WININIT_RENAME_SEC, strLineBuf, sizeof(VWUPDATE_WININIT_RENAME_SEC)-1 ) )
		{
			bFindRenameSection = true;
			break;
		}
	}
	fseek(pWininitFile,0,SEEK_END);
	if(!bFindRenameSection)
		fprintf(pWininitFile,"%s\n",VWUPDATE_WININIT_RENAME_SEC);
	if( lpNewFileNameA )
		fprintf(pWininitFile,"%s=%s\n", lpNewFileName, lpExistingFileName);
	else
	{
		int nLen = strlen(lpExistingFileName);
		if( lpExistingFileName[nLen-1] == '\\' )
		{
			lpExistingFileName[nLen-1] = '\0';
			fprintf(pWininitFile,"DIRNUL=%s\n",lpExistingFileName);
		}
		else
			fprintf(pWininitFile,"NUL=%s\n",lpExistingFileName);
	}
	
	fclose(pWininitFile);
	return TRUE;
}

/**
 *	判断是否是可以运行的 DLL 文件
 */
BOOL CVwUpdate::IsDllCanbeRun( LPCTSTR lpszFile )
{
	//
	//	lpszFile	- [in] 文件路径
	//	RETURN		- TRUE / FALSE

	if ( NULL == lpszFile )
		return FALSE;
	
	typedef VOID (CALLBACK* PRundll32)( HWND hMain, HINSTANCE hinst, LPTSTR lpCmdLine, int nCmdShow );

	assert( lpszFile );
	BOOL bRet = FALSE;
	HMODULE hDll = NULL;
	hDll = LoadLibrary( lpszFile );
	if( hDll )
	{
		PRundll32 Run = (PRundll32)GetProcAddress( hDll, "Rundll32" );
		if( Run )
			bRet = TRUE;
		FreeLibrary( hDll );
	}
	return bRet;
}

/**
 *	判断是否是可以运行的 DLL 文件
 */
BOOL CVwUpdate::IsDllCanbeLoad( LPCTSTR lpszFile )
{
	//
	//	lpszFile	- [in] 文件路径
	//	RETURN		- TRUE / FALSE

	assert( lpszFile );

	if ( NULL == lpszFile )
		return FALSE;

	BOOL bRet	= FALSE;
	HMODULE hDll	= NULL;
	PFNLOAD pfnLoad	= NULL;

	hDll = LoadLibrary( lpszFile );
	if ( hDll )
	{
		pfnLoad = (PFNLOAD)GetProcAddress( hDll, "Load" );
		if( pfnLoad )
		{
			bRet = TRUE;
		}
		FreeLibrary( hDll );
	}
	return bRet;
}

/**
 *	判断是否是可以运行的 DLL 文件
 */
BOOL CVwUpdate::RunDllLoad( LPCTSTR lpszFile, LPCTSTR lpcszCmd )
{
	//
	//	lpszFile	- [in] 文件路径
	//	RETURN		- TRUE / FALSE

	assert( lpszFile );

	if ( NULL == lpszFile )
		return FALSE;

	BOOL bRet	= FALSE;
	HMODULE hDll	= NULL;
	PFNLOAD pfnLoad	= NULL;

	hDll = LoadLibrary( lpszFile );
	if ( hDll )
	{
		pfnLoad = (PFNLOAD)GetProcAddress( hDll, "Load" );
		if( pfnLoad )
		{
			bRet = pfnLoad( lpcszCmd );
		}
		FreeLibrary( hDll );
	}
	return bRet;
}