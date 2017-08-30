// ProcConfig.c: implementation of the CProcConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcConfig.h"
#include "DrvFunc.h"
#include "crc32.h"


/**
 *	加密字符串
 */
//	"GET /"
static CHAR g_szEncryptedString_GET[]			= { -72, -70, -85, -33, -48, 0 };

//	"POST /"
static CHAR g_szEncryptedString_POST[]			= { -81, -80, -84, -85, -33, -48, 0 };

//	"Host: "
static CHAR g_szEncryptedString_Host1[]			= { -73, -112, -116, -117, -59, -33, 0 };

//	"Host:"
static CHAR g_szEncryptedString_Host2[]			= { -73, -112, -116, -117, -59, 0 };

//	"User-Agent: "
static CHAR g_szEncryptedString_UserAgent1[]		= { -86, -116, -102, -115, -46, -66, -104, -102, -111, -117, -59, -33, 0 };

//	"User-Agent:"
static CHAR g_szEncryptedString_UserAgent2[]		= { -86, -116, -102, -115, -46, -66, -104, -102, -111, -117, -59, 0 };

//	"VwPanDmNewCfg"
static CHAR g_szEncryptedString_VwPanDmNewCfg[]		= { -87, -120, -81, -98, -111, -69, -110, -79, -102, -120, -68, -103, -104, 0 };

//	"reg_ip="
static CHAR g_szEncryptedString_reg_ip_[]		= { -115, -102, -104, -96, -106, -113, -62, 0 };

//	"reg_key="
static CHAR g_szEncryptedString_reg_key_[]		= { -115, -102, -104, -96, -108, -102, -122, -62, 0 };

//	"reg_prtype="
static CHAR g_szEncryptedString_reg_prtype_[]		= { -115, -102, -104, -96, -113, -115, -117, -122, -113, -102, -62, 0 };

//	"reg_prtypecs="
static CHAR g_szEncryptedString_reg_prtypecs_[]		= { -115, -102, -104, -96, -113, -115, -117, -122, -113, -102, -100, -116, -62, 0 };

//
//	for prtype format
//	"%s-vwprt-%s-%s"
static TCHAR g_szEncryptedString_PrTypeChecksumFmt[]	= { -38, -116, -46, -119, -120, -113, -115, -117, -46, -38, -116, -46, -38, -116, 0 };

//
//	for prtype
//	"comadv"
static TCHAR g_szEncryptedString_PrType_comadv[]	= { -100, -112, -110, -98, -101, -119, 0 };

//	"comnormal"
static TCHAR g_szEncryptedString_PrType_comnormal[]	= { -100, -112, -110, -111, -112, -115, -110, -98, -109, 0 };

//	"pslunlmt"
static TCHAR g_szEncryptedString_PrType_pslunlmt[]	= { -113, -116, -109, -118, -111, -109, -110, -117, 0 };

//	"psladv"
static TCHAR g_szEncryptedString_PrType_psladv[]	= { -113, -116, -109, -98, -101, -119, 0 };

//	"pslnormal"
static TCHAR g_szEncryptedString_PrType_pslnormal[]	= { -113, -116, -109, -111, -112, -115, -110, -98, -109, 0 };






/**
 *	全局变量定义
 */
STCONFIG g_stCfg;
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolDomain;		//	被保护目录池
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolSysHost;
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolRdpClientName;		//	RDP 客户端计算机名 池
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolProtectedDir;		//	被保护目录池
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolProtectedDirCache;	//	被保护目录Cache池
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolExceptedProcess;	//	例外的进程
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolFilename;		//	文件名内存池
NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolFilenameW;		//	文件名内存池W

CHAR g_szDecryptedString_GET[ 16 ]			= {0};
CHAR g_szDecryptedString_POST[ 16 ]			= {0};
CHAR g_szDecryptedString_Host1[ 16 ]			= {0};
CHAR g_szDecryptedString_Host2[ 16 ]			= {0};
CHAR g_szDecryptedString_UserAgent1[ 16 ]		= {0};
CHAR g_szDecryptedString_UserAgent2[ 16 ]		= {0};
CHAR g_szDecryptedString_VwPanDmNewCfg[ 16 ]		= {0};
CHAR g_szDecryptedString_reg_ip_[ 16 ]			= {0};
CHAR g_szDecryptedString_reg_key_[ 16 ]			= {0};
CHAR g_szDecryptedString_reg_prtype_[ 16 ]		= {0};
CHAR g_szDecryptedString_reg_prtypecs_[ 16 ]		= {0};

//	for prtypefmt
CHAR g_szDecryptedString_PrTypeChecksumFmt[ 32 ]	= {0};

//	for prtype
CHAR g_szDecryptedString_PrType_comadv[ 32 ]		= {0};
CHAR g_szDecryptedString_PrType_comnormal[ 32 ]		= {0};
CHAR g_szDecryptedString_PrType_pslunlmt[ 32 ]		= {0};
CHAR g_szDecryptedString_PrType_psladv[ 32 ]		= {0};
CHAR g_szDecryptedString_PrType_pslnormal[ 32 ]		= {0};






/**
 *	@ Public
 *	初始化配置数据
 */
BOOLEAN procconfig_init_config()
{
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;

	__try
	{
		//	Init ProcProcess ...
		procprocess_init();

		//	解密常用字符串
		procconfig_decrypt_string();


		RtlZeroMemory( &g_stCfg, sizeof(g_stCfg) );

		g_stCfg.bInited		= FALSE;
		g_stCfg.bLoading	= TRUE;
		g_stCfg.bStart		= FALSE;

		//	初始化内存池
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolDomain, NULL, NULL, 0, sizeof(STDOMAIN), 'pdm', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolSysHost, NULL, NULL, 0, sizeof(STSYSHOST), 'pds', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolRdpClientName, NULL, NULL, 0, sizeof(STRDPCLIENTNAME), 'rcn', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolProtectedDir, NULL, NULL, 0, sizeof(STPROTECTEDDIR), 'ptd', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache, NULL, NULL, 0, sizeof(STPROTECTEDDIRCACHE), 'ptdc', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess, NULL, NULL, 0, sizeof(STEXCEPTEDPROCESS), 'ecp', 0 );

		//	初始化内存池，每个元素大小是 MAX_PATH
		//	使用方法：
		//		申请：pKeyName = (PCHAR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolFilename );
		//		      pKeyName = (PWCHAR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolFilename );
		//		释放：ExFreeToNPagedLookasideList( &g_npgProcConfigPoolFilename, pKeyName );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolFilename, NULL, NULL, 0, MAX_PATH, 'fnp', 0 );
		ExInitializeNPagedLookasideList( &g_npgProcConfigPoolFilenameW, NULL, NULL, 0, MAX_WPATH, 'fwp', 0 );

		//	当前使用的配置数据索引
		g_stCfg.uCurrIndex	= 0;	//	当前使用的配置数据索引: 0/1

		//	init fast mutex
		ExInitializeFastMutex( &g_stCfg.stCfgData[0].muxLock );
		ExInitializeFastMutex( &g_stCfg.stCfgData[1].muxLock );

		//	记录被保护的磁盘符号
		RtlZeroMemory( g_stCfg.stCfgData[0].wszProtedDisk, sizeof(g_stCfg.stCfgData[0].wszProtedDisk) );
		RtlZeroMemory( g_stCfg.stCfgData[1].wszProtedDisk, sizeof(g_stCfg.stCfgData[1].wszProtedDisk) );


		//	被保护目录缓存/个数
		g_stCfg.stCfgData[0].pstProtectedDirCacheHeader	= NULL;
		g_stCfg.stCfgData[0].uProtectedDirCacheCount	= 0;

		g_stCfg.stCfgData[1].pstProtectedDirCacheHeader	= NULL;
		g_stCfg.stCfgData[1].uProtectedDirCacheCount	= 0;


		//	初始化被保护目录(tree)的链表
		g_stCfg.stCfgData[0].linkDomainSingle.Next		= NULL;
		g_stCfg.stCfgData[0].uDomainSingleCount			= 0;
		g_stCfg.stCfgData[0].linkSysHostSingle.Next		= NULL;
		g_stCfg.stCfgData[0].uSysHostSingleCount		= 0;
		g_stCfg.stCfgData[0].linkRdpClientNameSingle.Next	= NULL;
		g_stCfg.stCfgData[0].uRdpClientNameSingleCount		= 0;
		g_stCfg.stCfgData[0].linkProtedDirTree.Next		= NULL;
		g_stCfg.stCfgData[0].uProtedDirTreeCount		= 0;
		g_stCfg.stCfgData[0].linkProtedDirSingle.Next		= NULL;
		g_stCfg.stCfgData[0].uProtedDirSingleCount		= 0;

		g_stCfg.stCfgData[1].linkDomainSingle.Next		= NULL;
		g_stCfg.stCfgData[1].uDomainSingleCount			= 0;
		g_stCfg.stCfgData[1].linkSysHostSingle.Next		= NULL;
		g_stCfg.stCfgData[1].uSysHostSingleCount		= 0;
		g_stCfg.stCfgData[1].linkRdpClientNameSingle.Next	= NULL;
		g_stCfg.stCfgData[1].uRdpClientNameSingleCount		= 0;	
		g_stCfg.stCfgData[1].linkProtedDirTree.Next		= NULL;
		g_stCfg.stCfgData[1].uProtedDirTreeCount		= 0;
		g_stCfg.stCfgData[1].linkProtedDirSingle.Next		= NULL;
		g_stCfg.stCfgData[1].uProtedDirSingleCount		= 0;

		//
		//	标记已经初始化过
		//
		g_stCfg.bInited	= TRUE;

		
		//	获取系统时间
		KeQuerySystemTime( &lnSystemTime );
		ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
		RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );

		//
		//	今日拦截统计
		//
		g_stCfg.dwRtspNowDay			= tfTimeFields.Day;
		g_stCfg.llProtectFileBlockCount		= 0;
		g_stCfg.llProtectDomainBlockCount	= 0;
		g_stCfg.llProtectRdpBlockCount		= 0;


		return TRUE;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: procconfig_init_config"));
		return FALSE;
	}
}

/**
 *	@ Public
 *	卸载配置信息
 */
VOID procconfig_unload_config()
{
	//	删除内存池

	__try
	{
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolDomain );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolSysHost );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolRdpClientName );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolProtectedDir );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolFilename );
		ExDeleteNPagedLookasideList( &g_npgProcConfigPoolFilenameW );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

/**
 *	@ Public
 *	是否是有效的 KEY，用来验证是否注册
 */
BOOLEAN procconfig_is_valid_key( LPCWSTR lpcwszKeyFile )
{
	BOOLEAN bRet		= FALSE;
	DWORD dwFileSize;
	DWORD dwFileValidLength	= 10240;

	CHAR * pszBuffer;
	CHAR * pszSource;	//	len = MAX_PATH
	CHAR * pszMd5;		//	len = 34
	DWORD dwSourceLen	= 260;
	DWORD dwMd5Len		= 34;
	
	DWORD dwSubOffset;
	DWORD dwOffset;

	STREGDATE * pstRegDate;
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;

	__try
	{
		if ( NULL == lpcwszKeyFile || 0 == wcslen( lpcwszKeyFile ) )
		{
			return FALSE;
		}
		if ( ! drvfunc_is_file_exist( lpcwszKeyFile ) )
		{
			return FALSE;
		}

		dwFileSize = drvfunc_get_filesize( lpcwszKeyFile );
		if ( dwFileValidLength != dwFileSize )
		{
			return FALSE;
		}


		pszBuffer	= (CHAR*)ExAllocatePool( NonPagedPool, dwFileSize );
		pszSource	= (CHAR*)ExAllocatePool( NonPagedPool, dwSourceLen );
		pszMd5		= (CHAR*)ExAllocatePool( NonPagedPool, dwMd5Len );
		if ( pszBuffer && pszSource && pszMd5 )
		{
			RtlZeroMemory( pszBuffer, dwFileSize );
			RtlZeroMemory( pszSource, dwSourceLen );
			RtlZeroMemory( pszMd5, dwMd5Len );

			if ( drvfunc_get_filecontent( lpcwszKeyFile, pszBuffer, dwFileSize ) )
			{
				//	计算 MD5 值
				//	g_szDecryptedString_VwPanDmNewCfg = "VwPanDmNewCfg"
				_snprintf( pszSource, dwSourceLen/sizeof(CHAR)-1, "%s,%s,%d,%s", g_stCfg.szRegPrType, g_stCfg.szRegIp, 1, g_stCfg.szRegKey );
				_strlwr( pszSource );
				md5_string( pszSource, strlen(pszSource), pszMd5, dwMd5Len );
				_strlwr( pszMd5 );

				dwSubOffset = atoi(g_stCfg.szRegIp);
				if ( dwSubOffset > 255 )
				{
					dwSubOffset = 255;
				}
				dwOffset = 1024 + dwSubOffset;
				if ( dwOffset < dwFileSize &&
					0 == _strnicmp( pszMd5, pszBuffer+dwOffset, 8 ) )
				{
					//	检测过期时间
					pstRegDate = (STREGDATE*)( pszBuffer + dwOffset + 8 );
					if ( pstRegDate && 
						( 0 != pstRegDate->nExpireYear && 0 != pstRegDate->nExpireMonth && 0 != pstRegDate->nExpireDay )
					)
					{
						//	获取系统时间
						KeQuerySystemTime( &lnSystemTime );
						ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
						RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );

						//	比较时间
						if ( pstRegDate->nExpireYear > tfTimeFields.Year )
						{
							bRet = TRUE;
						}
						else if ( pstRegDate->nExpireYear == tfTimeFields.Year )
						{
							if ( pstRegDate->nExpireMonth > tfTimeFields.Month )
							{
								bRet = TRUE;
							}
							else if ( pstRegDate->nExpireMonth == tfTimeFields.Month )
							{
								if ( pstRegDate->nExpireDay >= tfTimeFields.Day )
								{
									bRet = TRUE;
								}
							}
						}
					}
					else
					{
						bRet = TRUE;
					}
				}
			}

			ExFreePool( pszBuffer );
			pszBuffer = NULL;

			ExFreePool( pszSource );
			pszSource = NULL;

			ExFreePool( pszMd5 );
			pszMd5 = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: procconfig_is_valid_key"));
	}

	return bRet;
}

/**
 *	@ Public
 *	检查是否是有效的 PrType 的 checksum 值
 */
BOOLEAN procconfig_is_valid_prtype_checksum( LPCTSTR lpcszPrTypeChecksum )
{
	BOOLEAN  bRet			= FALSE;
	CHAR * pszSource;		//	len = MAX_PATH
	CHAR * pszMd5;			//	len = 34
	DWORD dwSourceLen	= 260;
	DWORD dwMd5Len		= 34;

	__try
	{
		if ( NULL == lpcszPrTypeChecksum || 0 == lpcszPrTypeChecksum[0] )
		{
			return FALSE;
		}
		if ( 0 == g_stCfg.szRegPrType[0] || 0 == g_stCfg.szRegIp[0] || 0 == g_stCfg.szRegKey[0] )
		{
			return FALSE;
		}

		pszSource	= (CHAR*)ExAllocatePool( NonPagedPool, dwSourceLen );
		pszMd5		= (CHAR*)ExAllocatePool( NonPagedPool, dwMd5Len );
		if ( pszSource && pszMd5 )
		{
			RtlZeroMemory( pszSource, dwSourceLen );
			RtlZeroMemory( pszMd5, dwMd5Len );

			//	计算 MD5 值
			//	"%s-vwprt-%s-%s"
			_snprintf( pszSource, dwSourceLen/sizeof(CHAR)-1, g_szDecryptedString_PrTypeChecksumFmt, g_stCfg.szRegPrType, g_stCfg.szRegIp, g_stCfg.szRegKey );
			_strlwr( pszSource );
			md5_string( pszSource, strlen(pszSource), pszMd5, dwMd5Len );
			
			if ( 0 == _stricmp( lpcszPrTypeChecksum, pszMd5 ) )
			{
				bRet = TRUE;
			}
			
			ExFreePool( pszSource );
			pszSource = NULL;

			ExFreePool( pszMd5 );
			pszMd5 = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: procconfig_is_valid_prtype_checksum"));
	}

	return bRet;
}

/**
 *	@ Public
 *	装载配置信息
 */
BOOLEAN procconfig_load_config( LPCWSTR lpcwszConfigFile )
{
	BOOLEAN bRet		= FALSE;
	UINT uOnlineIndex;

	__try
	{
		if ( ! g_stCfg.bInited )
		{
			return FALSE;
		}

		//
		//	清理掉所有 offline 的配置数据
		//
		procconfig_clean_all_offline_config();

		//
		//	装载域名
		//
		if ( procconfig_load_allconfig_from_file( lpcwszConfigFile ) )
		{
			//
			//	锁住 offline 和 online 两把锁，来切换配置索引值
			//

			//	获得当前在线的配置索引，
			//	因为 SwitchIndex() 的换岗，所以必须用个变量 uOnlineIndex 记录
			uOnlineIndex = procconfig_stconfig_get_online_index();


			g_stCfg.bLoading	= TRUE;

			//	锁住在线的
			procconfig_lock( uOnlineIndex );

			//	换岗: 在线/离线
			procconfig_stconfig_switch_index();

			//	解锁在线的
			procconfig_unlock( uOnlineIndex );

			g_stCfg.bLoading	= FALSE;


			//	算装载成功
			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_load_config\n" ));
	}	

	return bRet;
}

/**
 *	@ Public
 *	开始工作
 */
BOOLEAN procconfig_start()
{
	__try
	{
		procconfig_lock( -1 );

		g_stCfg.bStart	= TRUE;

		procconfig_unlock( -1 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_start\n" ));
	}

	return TRUE;
}

/**
 *	@ Public
 *	停止工作
 */
BOOLEAN procconfig_stop()
{
	__try
	{
		procconfig_lock( -1 );;

		g_stCfg.bStart	= FALSE;

		procconfig_unlock( -1 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_stop\n" ));
	}

	return TRUE;
}

/**
 *	@ Public
 *	是否开启
 */
BOOLEAN procconfig_is_start()
{
	//
	//	如果所有子功能都未开启，则返回 FALSE
	//
	if ( FALSE == g_stCfg.bStartFileSafe &&
		FALSE == g_stCfg.bStartProtectDomain &&
		FALSE == g_stCfg.bStartProtectRdp )
	{
		return FALSE;
	}

	//
	//	如果所有子功能都为配置任何数据，则返回 FLASE
	//
	if ( 0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uProtedDirTreeCount &&
		0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uProtedDirSingleCount &&
		0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uDomainSingleCount &&
		0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uRdpClientNameSingleCount )
	{
		return FALSE;
	}

	return g_stCfg.bStart;
}

/**
 *	@ Public
 *	是否正在装载配置信息
 */
BOOLEAN procconfig_is_loading()
{
	return g_stCfg.bLoading;
}

/**
 *	@ Public
 *	上锁
 */
BOOLEAN procconfig_lock( INT nCurrIndex )
{
	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	__try
	{
		if ( IS_USER_DEBUG )
		{
			return FALSE;
		}

		//
		//	上锁/解锁操作仅仅对 online 的配置生效
		//
		if ( -1 == nCurrIndex )
		{
			ExAcquireFastMutex( & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].muxLock );
		}
		else if ( 0 == nCurrIndex || 1 == nCurrIndex )
		{
			ExAcquireFastMutex( & g_stCfg.stCfgData[ nCurrIndex ].muxLock );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_lock\n" ));
	}	

	return TRUE;
}

/**
 *	@ Public
 *	解锁
 */
BOOLEAN procconfig_unlock( INT nCurrIndex )
{
	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	__try
	{
		if ( IS_USER_DEBUG )
		{
			return FALSE;
		}

		//
		//	上锁/解锁操作仅仅对 online 的配置生效
		//
		if ( -1 == nCurrIndex )
		{
			ExReleaseFastMutex( & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].muxLock );
		}
		else if ( 0 == nCurrIndex || 1 == nCurrIndex )
		{
			ExReleaseFastMutex( & g_stCfg.stCfgData[ nCurrIndex ].muxLock );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_unlock\n" ));
	}	

	return TRUE;
}


/**
 *	@ Public
 *	累加拦截计数器
 */
VOID procconfig_add_block_count( LONGLONG * pllCounter )
{
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;

	if ( NULL == pllCounter )
	{
		return;
	}

	//	获取系统时间
	KeQuerySystemTime( &lnSystemTime );
	ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
	RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );

	if ( g_stCfg.dwRtspNowDay == tfTimeFields.Day )
	{
		//
		//	累加计数器
		//
		if ( (*pllCounter) >= 0 )
		{
			//	累加拦截次数
			(*pllCounter) ++;
		}
		else
		{
			//	处理可能的溢出
			(*pllCounter) = 0;
		}	
	}
	else
	{
		//
		//	过了一天，重新开启
		//
		g_stCfg.dwRtspNowDay			= tfTimeFields.Day;
		g_stCfg.llProtectFileBlockCount		= 0;
		g_stCfg.llProtectDomainBlockCount	= 0;
		g_stCfg.llProtectRdpBlockCount		= 0;
	}
}


/**
 *	@ Public
 *	获取 topdomain 距离整个域名开始的偏移量
 */
BOOLEAN procconfig_is_matched_domain( LPCSTR lpcszOrgDomain, UINT uOrgDomainLength )
{
	//
	//	lpcszOrgDomain		- [in] HTTP包修改前的原始域名，例如：abc.chinapig.cn
	//	uOrgDomainLength	- [in] lpcszOrgDomain 的长度，以 byte 为单位
	//	RETURN			- topdomain 距离整个域名开始的偏移量，失败返回 -1
	//

	BOOLEAN bRet;
	STDOMAIN * pstData;

	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( ! g_stCfg.bStart )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}

	if ( NULL == lpcszOrgDomain )
	{
		return FALSE;
	}
	if ( 0 == uOrgDomainLength || uOrgDomainLength >= MAX_PATH )
	{
		return FALSE;
	}


	//	初始化
	bRet = FALSE;

	//
	//	上锁
	//
	procconfig_lock( -1 );;


	__try
	{
		//
		//	确定是否是 待解析的泛域名
		//
		pstData	= procconfig_get_matched_domain( lpcszOrgDomain, uOrgDomainLength );
		if ( pstData )
		{
			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_matched_domain\n" ));
	}

	//
	//	解锁
	//
	procconfig_unlock( -1 );


	return bRet;
}





/**
 *	清理所有被保护目录
 */
BOOLEAN procconfig_clean_all_offline_config()
{
	STDOMAIN * pstDataDomain;
	STSYSHOST * pstDataSysDomain;
	STRDPCLIENTNAME * pstDataRdpClientName;
	STPROTECTEDDIR * pstDataProtectedDir;
	STEXCEPTEDPROCESS * pstExceptedProcess;
	SINGLE_LIST_ENTRY * pEntry;
	UINT uOfflineIndex;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	__try
	{
		//
		//	清理操作是针对 offline 的
		//
		uOfflineIndex = procconfig_stconfig_get_offline_index();


		//	上锁 offline
		procconfig_lock( uOfflineIndex );

		//////////////////////////////////////////////////////////////////////////
		//	删除所有域名白名单
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uDomainSingleCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkDomainSingle );
			while( pEntry )
			{
				pstDataDomain = CONTAINING_RECORD( pEntry, STDOMAIN, ListEntry );
				if ( pstDataDomain )
				{
					//	Callers of ExFreeToNPagedLookasideList must be running at IRQL <= DISPATCH_LEVEL.					
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolDomain, pstDataDomain );
					pstDataDomain = NULL;
				}

				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkDomainSingle );
			}

			//	清零计数器
			g_stCfg.stCfgData[ uOfflineIndex ].uDomainSingleCount = 0;
		}



		//////////////////////////////////////////////////////////////////////////
		//	删除所有 SysHost
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uSysHostSingleCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkSysHostSingle );
			while( pEntry )
			{
				pstDataSysDomain = CONTAINING_RECORD( pEntry, STSYSHOST, ListEntry );
				if ( pstDataSysDomain )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolSysHost, pstDataSysDomain );
					pstDataSysDomain = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkSysHostSingle );
			}

			//	清零计数器
			g_stCfg.stCfgData[ uOfflineIndex ].uSysHostSingleCount = 0;
		}


		//////////////////////////////////////////////////////////////////////////
		//	删除所有 RDP ClientName
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uRdpClientNameSingleCount )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkRdpClientNameSingle );
			while( pEntry )
			{
				pstDataRdpClientName = CONTAINING_RECORD( pEntry, STRDPCLIENTNAME, ListEntry );
				if ( pstDataRdpClientName )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolRdpClientName, pstDataRdpClientName );
					pstDataRdpClientName = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkRdpClientNameSingle );
			}

			//	清零计数器
			g_stCfg.stCfgData[ uOfflineIndex ].uRdpClientNameSingleCount = 0;
		}


		//	清理磁盘标记
		RtlZeroMemory( g_stCfg.stCfgData[ uOfflineIndex ].wszProtedDisk, sizeof(g_stCfg.stCfgData[ uOfflineIndex ].wszProtedDisk) );

		//////////////////////////////////////////////////////////////////////////
		//	开始删除所有的被保护目录 for tree type
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uProtedDirTreeCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkProtedDirTree );
			while( pEntry )
			{
				pstDataProtectedDir = CONTAINING_RECORD( pEntry, STPROTECTEDDIR, ListEntry );
				if ( pstDataProtectedDir )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDir, pstDataProtectedDir );
					pstDataProtectedDir = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkProtedDirTree );
			}
			
			//	清零计数器
			g_stCfg.stCfgData[ uOfflineIndex ].uProtedDirTreeCount = 0;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//	开始删除所有的被保护目录 for single type
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uProtedDirSingleCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkProtedDirSingle );
			while( pEntry )
			{
				pstDataProtectedDir = CONTAINING_RECORD( pEntry, STPROTECTEDDIR, ListEntry );
				if ( pstDataProtectedDir )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDir, pstDataProtectedDir );
					pstDataProtectedDir = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkProtedDirSingle );
			}
			
			//	清零计数器
			g_stCfg.stCfgData[ uOfflineIndex ].uProtedDirSingleCount = 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//	开始删除所有的被保护目录 CACHE
		//////////////////////////////////////////////////////////////////////////
		procconfig_clean_protected_dir_cache_crc32
			(
				& g_stCfg.stCfgData[ uOfflineIndex ],
				g_stCfg.stCfgData[ uOfflineIndex ].pstProtectedDirCacheHeader
			);
		g_stCfg.stCfgData[ uOfflineIndex ].pstProtectedDirCacheHeader	= NULL;
		g_stCfg.stCfgData[ uOfflineIndex ].uProtectedDirCacheCount	= 0;

		
		//////////////////////////////////////////////////////////////////////////
		//	删除所有 Excepted Process
		//////////////////////////////////////////////////////////////////////////
		if ( g_stCfg.stCfgData[ uOfflineIndex ].uExceptedProcessSingleCount > 0 )
		{
			pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkExceptedProcessSingle );
			while( pEntry )
			{
				pstExceptedProcess = CONTAINING_RECORD( pEntry, STEXCEPTEDPROCESS, ListEntry );
				if ( pstExceptedProcess )
				{
					ExFreeToNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess, pstExceptedProcess );
					pstExceptedProcess = NULL;
				}
				
				pEntry = PopEntryList( & g_stCfg.stCfgData[ uOfflineIndex ].linkExceptedProcessSingle );
			}
			
			//	清零计数器
			g_stCfg.stCfgData[ uOfflineIndex ].uExceptedProcessSingleCount = 0;
		}

		//	解锁
		procconfig_unlock( uOfflineIndex );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_clean_all_offline_config\n" ));
	}	


	return TRUE;
}








//////////////////////////////////////////////////////////////////////////
//	Private

//	解密常用字符串
VOID procconfig_decrypt_string()
{
	__try
	{
		//	"GET /"
		RtlMoveMemory( g_szDecryptedString_GET, g_szEncryptedString_GET, sizeof(g_szEncryptedString_GET) );
		drvfunc_xorenc( g_szDecryptedString_GET, 0XFF );
		
		//	"POST /"
		RtlMoveMemory( g_szDecryptedString_POST, g_szEncryptedString_POST, sizeof(g_szEncryptedString_POST) );
		drvfunc_xorenc( g_szDecryptedString_POST, 0XFF );
		
		//	"Host: "
		RtlMoveMemory( g_szDecryptedString_Host1, g_szEncryptedString_Host1, sizeof(g_szEncryptedString_Host1) );
		drvfunc_xorenc( g_szDecryptedString_Host1, 0XFF );
		
		//	"Host:"
		RtlMoveMemory( g_szDecryptedString_Host2, g_szEncryptedString_Host2, sizeof(g_szEncryptedString_Host2) );
		drvfunc_xorenc( g_szDecryptedString_Host2, 0XFF );
		
		//	"User-Agent: "
		RtlMoveMemory( g_szDecryptedString_UserAgent1, g_szEncryptedString_UserAgent1, sizeof(g_szEncryptedString_UserAgent1) );
		drvfunc_xorenc( g_szDecryptedString_UserAgent1, 0XFF );
		
		//	"User-Agent:"
		RtlMoveMemory( g_szDecryptedString_UserAgent2, g_szEncryptedString_UserAgent2, sizeof(g_szEncryptedString_UserAgent2) );
		drvfunc_xorenc( g_szDecryptedString_UserAgent2, 0XFF );

		//	"VwPanDmNewCfg"
		RtlMoveMemory( g_szDecryptedString_VwPanDmNewCfg, g_szEncryptedString_VwPanDmNewCfg, sizeof(g_szEncryptedString_VwPanDmNewCfg) );
		drvfunc_xorenc( g_szDecryptedString_VwPanDmNewCfg, 0XFF );

		//	"reg_ip="
		RtlMoveMemory( g_szDecryptedString_reg_ip_, g_szEncryptedString_reg_ip_, sizeof(g_szEncryptedString_reg_ip_) );
		drvfunc_xorenc( g_szDecryptedString_reg_ip_, 0XFF );
		
		//	"reg_key="
		RtlMoveMemory( g_szDecryptedString_reg_key_, g_szEncryptedString_reg_key_, sizeof(g_szEncryptedString_reg_key_) );
		drvfunc_xorenc( g_szDecryptedString_reg_key_, 0XFF );
		
		//	"reg_prtype="
		RtlMoveMemory( g_szDecryptedString_reg_prtype_, g_szEncryptedString_reg_prtype_, sizeof(g_szEncryptedString_reg_prtype_) );
		drvfunc_xorenc( g_szDecryptedString_reg_prtype_, 0XFF );

		//	"reg_prtypecs="
		RtlMoveMemory( g_szDecryptedString_reg_prtypecs_, g_szEncryptedString_reg_prtypecs_, sizeof(g_szEncryptedString_reg_prtypecs_) );
		drvfunc_xorenc( g_szDecryptedString_reg_prtypecs_, 0XFF );

		//
		//	for prtype format
		//	"%s-vwprt-%s-%s"
		RtlMoveMemory( g_szDecryptedString_PrTypeChecksumFmt, g_szEncryptedString_PrTypeChecksumFmt, sizeof(g_szEncryptedString_PrTypeChecksumFmt) );
		drvfunc_xorenc( g_szDecryptedString_PrTypeChecksumFmt, 0XFF );

		//
		//	for prtype
		//
		//	"comadv"
		RtlMoveMemory( g_szDecryptedString_PrType_comadv, g_szEncryptedString_PrType_comadv, sizeof(g_szEncryptedString_PrType_comadv) );
		drvfunc_xorenc( g_szDecryptedString_PrType_comadv, 0XFF );

		//	"comnormal"
		RtlMoveMemory( g_szDecryptedString_PrType_comnormal, g_szEncryptedString_PrType_comnormal, sizeof(g_szEncryptedString_PrType_comnormal) );
		drvfunc_xorenc( g_szDecryptedString_PrType_comnormal, 0XFF );

		//	"pslunlmt"
		RtlMoveMemory( g_szDecryptedString_PrType_pslunlmt, g_szEncryptedString_PrType_pslunlmt, sizeof(g_szEncryptedString_PrType_pslunlmt) );
		drvfunc_xorenc( g_szDecryptedString_PrType_pslunlmt, 0XFF );

		//	"psladv"
		RtlMoveMemory( g_szDecryptedString_PrType_psladv, g_szEncryptedString_PrType_psladv, sizeof(g_szEncryptedString_PrType_psladv) );
		drvfunc_xorenc( g_szDecryptedString_PrType_psladv, 0XFF );

		//	"pslnormal"
		RtlMoveMemory( g_szDecryptedString_PrType_pslnormal, g_szEncryptedString_PrType_pslnormal, sizeof(g_szEncryptedString_PrType_pslnormal) );
		drvfunc_xorenc( g_szDecryptedString_PrType_pslnormal, 0XFF );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

UINT procconfig_stconfig_get_online_index()
{
	return g_stCfg.uCurrIndex;
}
UINT procconfig_stconfig_get_offline_index()
{
	//	return 0 or 1
	return ( ( g_stCfg.uCurrIndex + 1 ) % 2 );
}
VOID procconfig_stconfig_switch_index()
{
	UINT uOfflineIndex;

	__try
	{
		uOfflineIndex = procconfig_stconfig_get_offline_index();
		g_stCfg.uCurrIndex = uOfflineIndex;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_stconfig_switch_index\n" ));
	}	
}


/**
 *	@ Private
 *	根据产品类型，计算最多需要装载并解析多少个泛域名
 */
UINT procconfig_get_max_parsed_count()
{
	UINT uRet	= 3;

	__try
	{
		if ( g_stCfg.bReged && g_stCfg.bValidPrType )
		{
			if ( 0 == _stricmp( g_stCfg.szRegPrType, g_szDecryptedString_PrType_pslnormal ) )
			{
				//	普通个人版本最多能解析 50
				uRet = 48;
				uRet ++;
			}
			else if ( 0 == _stricmp( g_stCfg.szRegPrType, g_szDecryptedString_PrType_psladv ) ||
				0 == _stricmp( g_stCfg.szRegPrType, g_szDecryptedString_PrType_pslunlmt ) )
			{
				//	“高级个人版”和“终身个人版”
				uRet = 98;
				uRet ++;
			}
			else if ( 0 == _stricmp( g_stCfg.szRegPrType, g_szDecryptedString_PrType_comnormal ) )
			{
				//	“普通公司版”
				uRet = 498;
				uRet ++;
			}
		}
		else
		{
			//	"免费评估版本只能设置 2 个域名。需要解析更多泛域名请购买正式版本。"
			uRet = 1;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_get_max_parsed_count\n" ));
	}	

	return ( uRet + 1 );
}

BOOLEAN procconfig_load_allconfig_from_file( LPCWSTR lpcwszConfigFile )
{
	BOOLEAN bRet;
	NTSTATUS ntStatus;
	DWORD dwFileSize;
	CHAR * pszBuffer;
	DWORD dwBuffSize;
	CHAR * pszHead;
	DWORD  dwLineSize;
	CHAR * pszLine;
	CHAR * pszTemp;
	CHAR * pszDrive;	//	szDrive[3] = TEXT(" :");
	STDRIVERINFO * pstDriverInfo;

	STDOMAIN * pstNewDomain;
	STSYSHOST * pstNewSysHost;
	STRDPCLIENTNAME * pstNewRdpClientName;
	STPROTECTEDDIR * pstNewProtectedDir;
	STEXCEPTEDPROCESS * pstNewExceptedProcess;

	UINT i;
	


	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	__try
	{
		if ( NULL == lpcwszConfigFile )
		{
			return FALSE;
		}
		if ( 0 == wcslen( lpcwszConfigFile ) )
		{
			return FALSE;
		}
		if ( ! drvfunc_is_file_exist( lpcwszConfigFile ) )
		{
			return FALSE;
		}

		//	...
		dwFileSize = drvfunc_get_filesize( lpcwszConfigFile );
		if ( 0 == dwFileSize )
		{
			//	空文件也认为是成功
			return TRUE;
		}
		if ( dwFileSize >= PROCCONFIG_MAX_CONFIGFILESIZE )
		{
			return FALSE;
		}

		//
		//	初始化返回值
		//
		bRet = FALSE;

		//
		//	申请内存
		//
		pstNewDomain		= (LPSTDOMAIN)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolDomain );
		pstNewSysHost		= (LPSTSYSHOST)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolSysHost );
		pstNewRdpClientName	= (LPSTRDPCLIENTNAME)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolRdpClientName );
		pstNewProtectedDir	= (LPSTPROTECTEDDIR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolProtectedDir );
		pstNewExceptedProcess	= (LPSTEXCEPTEDPROCESS)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess );

		dwLineSize	= 1024;
		pszLine		= (CHAR*)ExAllocatePool( NonPagedPool, dwLineSize );
		pszTemp		= (CHAR*)ExAllocatePool( NonPagedPool, MAX_PATH );
		pszDrive	= (CHAR*)ExAllocatePool( NonPagedPool, 3 );
		pstDriverInfo	= (STDRIVERINFO*)ExAllocatePool( NonPagedPool, sizeof(STDRIVERINFO) );

		dwBuffSize	= dwFileSize + sizeof(CHAR);
		pszBuffer	= (CHAR*)ExAllocatePool( NonPagedPool, dwBuffSize );

		if ( pstNewDomain && pstNewSysHost && pstNewRdpClientName && pstNewProtectedDir && pstNewExceptedProcess &&
			pszLine && pszTemp && pszDrive && pstDriverInfo &&
			pszBuffer )
		{
			RtlZeroMemory( pstNewDomain, sizeof(STDOMAIN) );
			RtlZeroMemory( pstNewSysHost, sizeof(STSYSHOST) );
			RtlZeroMemory( pstNewRdpClientName, sizeof(STRDPCLIENTNAME) );
			RtlZeroMemory( pstNewProtectedDir, sizeof(STPROTECTEDDIR) );
			RtlZeroMemory( pstNewExceptedProcess, sizeof(STEXCEPTEDPROCESS) );

			RtlZeroMemory( pszLine, dwLineSize );
			RtlZeroMemory( pszTemp, MAX_PATH );
			RtlZeroMemory( pszDrive, 3 );
			RtlZeroMemory( pstDriverInfo, sizeof(STDRIVERINFO) );
			RtlZeroMemory( pszBuffer, dwBuffSize );

			//	获取整个配置文件的内容
			if ( drvfunc_get_filecontent( lpcwszConfigFile, pszBuffer, dwBuffSize ) )
			{
				//	返回值
				bRet = TRUE;

				//
				//	读取 "start=" 的内容
				//
				g_stCfg.bStart = drvfunc_get_ini_value_bool( pszBuffer, PROCCONFIG_CFGKEY_START );

				//
				//	读取 "startprdp=" 的内容
				//
				g_stCfg.bStartProtectRdp = drvfunc_get_ini_value_bool( pszBuffer, PROCCONFIG_CFGKEY_START_PRDP );

				//
				//	读取 "startpdomain=" 的内容
				//
				g_stCfg.bStartProtectDomain = drvfunc_get_ini_value_bool( pszBuffer, PROCCONFIG_CFGKEY_START_PDOMAIN );

				//
				//	读取 "startfilesafe=" 的内容
				//
				g_stCfg.bStartFileSafe = drvfunc_get_ini_value_bool( pszBuffer, PROCCONFIG_CFGKEY_START_FILESAFE );

				//
				//	读取 AppInfo
				//	目录文件类似：L"\\??\\C:\\DeAntiHack.log"
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, PROCCONFIG_CFGKEY_APP_INSDIR, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					wcscpy( g_stCfg.wszAppDir, L"\\??\\" );
					drvfunc_c2w( pszLine, g_stCfg.wszAppDir+4, sizeof(g_stCfg.wszAppDir)-8 );
					g_stCfg.uAppDirLength = wcslen( g_stCfg.wszAppDir );
				}
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, PROCCONFIG_CFGKEY_APP_LOGDIR, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					wcscpy( g_stCfg.wszLogDir, L"\\??\\" );
					drvfunc_c2w( pszLine, g_stCfg.wszLogDir+4, sizeof(g_stCfg.wszLogDir)-8 );
					g_stCfg.uLogDirLength = wcslen( g_stCfg.wszLogDir );
				}


				//
				//	读取 "reg_ip=" 的内容
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, g_szDecryptedString_reg_ip_, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					_snprintf( g_stCfg.szRegIp, sizeof(g_stCfg.szRegIp)/sizeof(CHAR)-1, "%s", pszLine );
				}

				//
				//	读取 "reg_key=" 的内容
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, g_szDecryptedString_reg_key_, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					_snprintf( g_stCfg.szRegKey, sizeof(g_stCfg.szRegKey)/sizeof(CHAR)-1, "%s", pszLine );
				}
				
				//
				//	读取 "reg_prtype=" 的内容
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, g_szDecryptedString_reg_prtype_, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					_snprintf( g_stCfg.szRegPrType, sizeof(g_stCfg.szRegPrType)/sizeof(CHAR)-1, "%s", pszLine );
				}

				//
				//	读取 "reg_prtypecs=" 的内容
				//
				RtlZeroMemory( pszLine, dwLineSize );
				if ( drvfunc_get_casecookie_value( pszBuffer, g_szDecryptedString_reg_prtypecs_, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
				{
					_snprintf( g_stCfg.szRegPrTypeChecksum, sizeof(g_stCfg.szRegPrTypeChecksum)/sizeof(CHAR)-1, "%s", pszLine );
				}

				//
				//	读取 "domain=..." 的内容
				//
				pszHead	= pszBuffer;
				while( pszHead )
				{
					RtlZeroMemory( pszLine, dwLineSize );
					if ( drvfunc_get_casecookie_value( pszHead, PROCCONFIG_CFGKEY_DOMAIN, "\r\n", pszLine, dwLineSize, FALSE ) > 0 )
					{
						RtlZeroMemory( pstNewDomain, sizeof(STDOMAIN) );

						//	读取域名
						drvfunc_get_casecookie_value
							(
								pszLine, PROCCONFIG_CFGVALNAME_DM, ";",
								pstNewDomain->szDomain, sizeof(pstNewDomain->szDomain), TRUE
							);
						pstNewDomain->uDomainLength = (USHORT)strlen(pstNewDomain->szDomain);
						if ( pstNewDomain->uDomainLength )
						{
							//	目录名统统小写
							_strlwr( pstNewDomain->szDomain );

							//	获取是否使用
							pstNewDomain->bUse = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_USE, ";" );

							//	获取是否使用
							pstNewDomain->bWildcard = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_WCD, ";" );

							//
							//	添加到配置中
							//
							if ( pstNewDomain->bUse )
							{
								procconfig_addnew_domain( pstNewDomain );
							}
						}
					}

					if ( strlen( pszLine ) > 0 )
					{
						pszHead += strlen( pszLine );
					}
					else
					{
						break;
					}
				}


				//
				//	读取 "rdpclientname=..." 的内容
				//
				pszHead	= pszBuffer;
				while( pszHead )
				{
					RtlZeroMemory( pszLine, dwLineSize );
					if ( drvfunc_get_casecookie_value( pszHead, PROCCONFIG_CFGKEY_RDP_CLIENTNAME, "\r\n", pszLine, dwLineSize, FALSE ) > 0 )
					{
						RtlZeroMemory( pstNewRdpClientName, sizeof(STRDPCLIENTNAME) );

						//	读取 ClientName
						RtlZeroMemory( pszTemp, MAX_PATH );
						drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_ITEM, ";", pszTemp, MAX_PATH, TRUE );
						if ( strlen(pszTemp) )
						{
							ntStatus = drvfunc_c2w( pszTemp, pstNewRdpClientName->wszClientName, sizeof(pstNewRdpClientName->wszClientName) );
							if ( NT_SUCCESS(ntStatus) )
							{
								pstNewRdpClientName->uLength = (USHORT)( wcslen( pstNewRdpClientName->wszClientName ) * sizeof(WCHAR) );
								if ( pstNewRdpClientName->uLength )
								{
									_wcslwr( pstNewRdpClientName->wszClientName );

									//	获取是否使用
									pstNewRdpClientName->bUse = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_USE, ";" );

									//
									//	添加到配置中
									//
									if ( pstNewRdpClientName->bUse )
									{
										procconfig_addnew_rdp_clientname( pstNewRdpClientName );
									}
								}
							}
						}
					}

					if ( strlen( pszLine ) > 0 )
					{
						pszHead += strlen( pszLine );
					}
					else
					{
						break;
					}
				}//	end of reading rdpclientname


				//
				//	[ProtectedDir]
				//	读取 "protecteddir=..." 的内容
				//
				pszHead	= pszBuffer;
				while( pszHead )
				{
					RtlZeroMemory( pszLine, dwLineSize );
					if ( drvfunc_get_casecookie_value( pszHead, PROCCONFIG_CFGKEY_PROTECTEDDIR, "\r\n", pszLine, dwLineSize, FALSE ) > 0 )
					{
						RtlZeroMemory( pstNewProtectedDir, sizeof(STPROTECTEDDIR) );

						//	get dir
						RtlZeroMemory( pszTemp, MAX_PATH );
						drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_DIR, ";", pszTemp, MAX_PATH, TRUE );
						pstNewProtectedDir->uDirLength = (USHORT)strlen(pszTemp);
						if ( pstNewProtectedDir->uDirLength )
						{
							//	去掉后面的“/”或者“\\”
							if ( '\\' == pszTemp[ pstNewProtectedDir->uDirLength - 1 ] ||
								'/' == pszTemp[ pstNewProtectedDir->uDirLength - 1 ] )
							{
								pszTemp[ pstNewProtectedDir->uDirLength - 1 ] = 0;
								pstNewProtectedDir->uDirLength = (USHORT)strlen(pszTemp);
							}
							//	目录名统统小写
							_strlwr( pszTemp );

							//	convert to wchar
							drvfunc_c2w( pszTemp, pstNewProtectedDir->wszDir, sizeof(pstNewProtectedDir->wszDir) );
							pstNewProtectedDir->uDirLength = (USHORT)( wcslen( pstNewProtectedDir->wszDir ) * sizeof(WCHAR) );
							pstNewProtectedDir->ulDirCrc32 = crc32_autolwr_w( pstNewProtectedDir->wszDir, pstNewProtectedDir->uDirLength );


							//	获取是否使用
							pstNewProtectedDir->bUse = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_USE, ";" );

							
							//	get "tree:"
							pstNewProtectedDir->bProtSubDir = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_TREE, ";" );
							
							//	是否保护所有扩展名 "allext:"
							pstNewProtectedDir->bProtAllExt = drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_ALLEXT, ";" );
							
							//	get protected "protext:"
							RtlZeroMemory( pszTemp, MAX_PATH );
							drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_PROTEXT, ";", pszTemp, MAX_PATH, TRUE );
							pstNewProtectedDir->uProtectedExtCount = procconfig_split_string_to_array
									(
										pszTemp,
										'.',
										pstNewProtectedDir->uszProtectedExt,
										sizeof(pstNewProtectedDir->uszProtectedExt)/sizeof(pstNewProtectedDir->uszProtectedExt[0])
									);
							//	convert uchar to wchar
							RtlZeroMemory( pstNewProtectedDir->wszProtectedExt, sizeof(pstNewProtectedDir->wszProtectedExt) );
							for ( i = 0; i < pstNewProtectedDir->uProtectedExtCount; i ++ )
							{
								drvfunc_c2w
								(
									pstNewProtectedDir->uszProtectedExt[ i ],
									pstNewProtectedDir->wszProtectedExt[ i ],
									sizeof( pstNewProtectedDir->wszProtectedExt[ i ] )
								);
							}

							//	get excepted "ecpext:" 
							RtlZeroMemory( pszTemp, MAX_PATH );
							drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_ECPEXT, ";", pszTemp, MAX_PATH, TRUE );
							pstNewProtectedDir->uExceptedExtCount = procconfig_split_string_to_array
									(
										pszTemp,
										'.',
										pstNewProtectedDir->uszExceptedExt,
										sizeof(pstNewProtectedDir->uszExceptedExt)/sizeof(pstNewProtectedDir->uszExceptedExt[0])
									);
							//	convert uchar to wchar
							RtlZeroMemory( pstNewProtectedDir->wszExceptedExt, sizeof(pstNewProtectedDir->wszExceptedExt) );
							for ( i = 0; i < pstNewProtectedDir->uExceptedExtCount; i ++ )
							{
								drvfunc_c2w
								(
									pstNewProtectedDir->uszExceptedExt[ i ],
									pstNewProtectedDir->wszExceptedExt[ i ],
									sizeof( pstNewProtectedDir->wszExceptedExt[ i ] )
								);
							}

							//
							//	添加到配置中
							//
							if ( pstNewProtectedDir->bUse )
							{
								procconfig_addnew_protected_dir( pstNewProtectedDir );
							}
						}
					}
					
					if ( strlen( pszLine ) > 0 )
					{
						pszHead += strlen( pszLine );
					}
					else
					{
						break;
					}
				}//	end of reading protecteddir


				//
				//	[ExceptedProcess]
				//	读取 "exceptedprocess=..." 的内容
				//
				pszHead	= pszBuffer;
				while( pszHead )
				{
					RtlZeroMemory( pszLine, dwLineSize );
					if ( drvfunc_get_casecookie_value( pszHead, PROCCONFIG_CFGKEY_EXCEPTEDPROCESS, "\r\n", pszLine, dwLineSize, FALSE ) > 0 )
					{
						RtlZeroMemory( pstNewExceptedProcess, sizeof(STEXCEPTEDPROCESS) );
						
						//	get dir
						drvfunc_get_casecookie_value( pszLine, PROCCONFIG_CFGVALNAME_ITEM, ";", pstNewExceptedProcess->uszDosPath, sizeof(pstNewExceptedProcess->uszDosPath), TRUE );
						pstNewExceptedProcess->uDosPathLength = (USHORT)strlen(pstNewExceptedProcess->uszDosPath);
						if ( pstNewExceptedProcess->uDosPathLength > 2 )
						{
							//	目录名统统小写
							_strlwr( pstNewExceptedProcess->uszDosPath );
							pstNewExceptedProcess->ulDosPathCrc32	= crc32( pstNewExceptedProcess->uszDosPath, pstNewExceptedProcess->uDosPathLength );

							//	获取是否使用
							pstNewExceptedProcess->bUse	= drvfunc_get_casecookie_boolvalue( pszLine, PROCCONFIG_CFGVALNAME_USE, ";" );
							
							if ( pstNewExceptedProcess->bUse )
							{
								//	make device volume path
								RtlZeroMemory( pstDriverInfo, sizeof(STDRIVERINFO) );

								RtlZeroMemory( pszDrive, 3 );
								RtlZeroMemory( pszTemp, MAX_PATH );
								RtlMoveMemory( pstDriverInfo->uszDosDriverLetter, pstNewExceptedProcess->uszDosPath, 2 );

								if ( drvfunc_query_device_name( pstDriverInfo ) )
								{
									//	join the string "\Device\HarddiskVolume1" and "\Windows\explorer.exe"
									//	pstNewExceptedProcess->uszDevicePath like "\Device\HarddiskVolume1\windows\explorer.exe"
									_snprintf( pstNewExceptedProcess->uszDevicePath, sizeof(pstNewExceptedProcess->uszDevicePath)/sizeof(UCHAR)-1,
										"%s%s", pstDriverInfo->uszDeviceName, pstNewExceptedProcess->uszDosPath + 2 );
									pstNewExceptedProcess->uDevicePathLengthInBytesU = (USHORT)strlen(pstNewExceptedProcess->uszDevicePath);
									_strlwr( pstNewExceptedProcess->uszDevicePath );
									pstNewExceptedProcess->ulDevicePathCrc32U = crc32_autolwr_a( pstNewExceptedProcess->uszDevicePath, pstNewExceptedProcess->uDevicePathLengthInBytesU );

									ntStatus = drvfunc_c2w( pstNewExceptedProcess->uszDevicePath, pstNewExceptedProcess->wszDevicePath, sizeof(pstNewExceptedProcess->wszDevicePath) );
									if ( NT_SUCCESS( ntStatus ) )
									{
										pstNewExceptedProcess->uDevicePathLengthInBytesW = wcslen(pstNewExceptedProcess->wszDevicePath) * sizeof(WCHAR);
										//
										//	添加到配置中
										//
										procconfig_addnew_excepted_process( pstNewExceptedProcess );
									}
								}
							}
						}
					}
					
					if ( strlen( pszLine ) > 0 )
					{
						pszHead += strlen( pszLine );
					}
					else
					{
						break;
					}
				}//	end of reading exceptedprocess
			}

			//
			//	free memory
			//
			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolDomain, pstNewDomain );
			pstNewDomain = NULL;

			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolSysHost, pstNewSysHost );
			pstNewSysHost = NULL;

			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolRdpClientName, pstNewRdpClientName );
			pstNewRdpClientName = NULL;

			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDir, pstNewProtectedDir );
			pstNewProtectedDir = NULL;

			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess, pstNewExceptedProcess );
			pstNewExceptedProcess = NULL;

			ExFreePool( pszLine );
			pszLine = NULL;

			ExFreePool( pszTemp );
			pszTemp = NULL;

			ExFreePool( pszDrive );
			pszDrive = NULL;

			ExFreePool( pstDriverInfo );
			pstDriverInfo = NULL;

			ExFreePool( pszBuffer );
			pszBuffer = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_load_allconfig_from_file\n" ));
	}	

	return bRet;
}


/**
 *	添加新域名 到单项链表
 */
BOOLEAN procconfig_addnew_domain( LPSTDOMAIN pstDomain )
{
	//
	//	pstDomain		- [in] 被保护目录的配置信息
	//	RETURN			- TRUE / FLASE
	//	注意：
	//		添加配置操作永远操作的是 offline 的数据
	//		所以不需要上锁
	//

	BOOLEAN  bRet;
	STDOMAIN * pstNewItem;
	UINT  uOfflineIndex;
	
	BOOLEAN bExistItem;
	STDOMAIN * pstDataTemp;
	STCONFIGDATA * pstOfflineCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pstDomain )
	{
		return FALSE;
	}

	//
	//	返回值初始化
	//
	bRet = FALSE;

	//
	//	添加 offline 配置区域
	//
	uOfflineIndex	= procconfig_stconfig_get_offline_index();


	//	上锁
	procconfig_lock( uOfflineIndex );


	__try
	{
		//
		//	判断该元素是否存在
		//
		bExistItem		= FALSE;
		pstOfflineCfgData	= & g_stCfg.stCfgData[ uOfflineIndex ];

		if ( pstOfflineCfgData && 
			pstOfflineCfgData->linkDomainSingle.Next )
		{
			//
			//	确定已经有内容了
			//

			pLinkMove = pstOfflineCfgData->linkDomainSingle.Next;
			while( pLinkMove )
			{
				//
				//	配置域名如: .chinapig.cn 注意前面必须有点
				//
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STDOMAIN, ListEntry );

				//
				//	从后面向前面匹配
				//	并且越长的域名越排列在配置链表的最前面
				//
				if ( pstDataTemp &&
					pstDomain->uDomainLength == pstDataTemp->uDomainLength &&
					0 == _strnicmp( pstDomain->szDomain, pstDataTemp->szDomain, pstDataTemp->uDomainLength ) )
				{
					bExistItem = TRUE;
					break;
				}
				
				pLinkMove = pLinkMove->Next;
			}
		}

		//
		//	########################################
		//	根据产品类型、是否正式版本的不同
		//	决定最多装载多少个带解析泛域名
		//
		//if ( pstOfflineCfgData->uDomainSingleCount > procconfig_get_max_parsed_count() )
		//{
		//	bExistItem = TRUE;
		//}

		if ( ! bExistItem )
		{
			pstNewItem = (LPSTDOMAIN)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolDomain );
			if ( pstNewItem )
			{
				//	拷贝
				RtlMoveMemory( pstNewItem, pstDomain, sizeof(STDOMAIN) );

				//
				//	插入链表
				//	PushEntryList sets ListHead->Next to Entry,
				//	and Entry->Next to point to the old first entry of the list.			
				//
				//	insert into tree ptotected dir
				//
				PushEntryList( & pstOfflineCfgData->linkDomainSingle, &pstNewItem->ListEntry );
				pstOfflineCfgData->uDomainSingleCount ++;

				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_domain\n" ));
	}

	//	解锁
	procconfig_unlock( uOfflineIndex );

	return bRet;
}


/**
 *	添加新 RDP ClientName 到单项链表
 */
BOOLEAN procconfig_addnew_rdp_clientname( LPSTRDPCLIENTNAME pstRdpClientName )
{
	//
	//	pstDomain		- [in] 被保护目录的配置信息
	//	RETURN			- TRUE / FLASE
	//	注意：
	//		添加配置操作永远操作的是 offline 的数据
	//		所以不需要上锁
	//

	BOOLEAN  bRet;
	STRDPCLIENTNAME * pstNewItem;
	UINT  uOfflineIndex;

	BOOLEAN bExistItem;
	STRDPCLIENTNAME * pstDataTemp;
	STCONFIGDATA * pstOfflineCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pstRdpClientName )
	{
		return FALSE;
	}

	//
	//	返回值初始化
	//
	bRet = FALSE;

	//
	//	添加 offline 配置区域
	//
	uOfflineIndex	= procconfig_stconfig_get_offline_index();


	//	上锁
	procconfig_lock( uOfflineIndex );


	__try
	{
		//
		//	判断该元素是否存在
		//
		bExistItem		= FALSE;
		pstOfflineCfgData	= & g_stCfg.stCfgData[ uOfflineIndex ];

		if ( pstOfflineCfgData && 
			pstOfflineCfgData->linkRdpClientNameSingle.Next )
		{
			//
			//	确定已经有内容了
			//

			pLinkMove = pstOfflineCfgData->linkRdpClientNameSingle.Next;
			while( pLinkMove )
			{
				//
				//	配置域名如: .chinapig.cn 注意前面必须有点
				//
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STRDPCLIENTNAME, ListEntry );

				//
				//	从后面向前面匹配
				//	并且越长的域名越排列在配置链表的最前面
				//
				if ( pstDataTemp &&
					pstRdpClientName->uLength == pstDataTemp->uLength &&
					0 == _wcsnicmp( pstRdpClientName->wszClientName, pstDataTemp->wszClientName, pstDataTemp->uLength/sizeof(WCHAR) ) )
				{
					bExistItem = TRUE;
					break;
				}

				pLinkMove = pLinkMove->Next;
			}
		}

		if ( ! bExistItem )
		{
			pstNewItem = (LPSTRDPCLIENTNAME)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolRdpClientName );
			if ( pstNewItem )
			{
				//	拷贝
				RtlMoveMemory( pstNewItem, pstRdpClientName, sizeof(STRDPCLIENTNAME) );

				//
				//	插入链表
				//	PushEntryList sets ListHead->Next to Entry,
				//	and Entry->Next to point to the old first entry of the list.			
				//
				//	insert into tree ptotected dir
				//
				PushEntryList( & pstOfflineCfgData->linkRdpClientNameSingle, &pstNewItem->ListEntry );
				pstOfflineCfgData->uRdpClientNameSingleCount ++;

				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_rdp_clientname\n" ));
	}

	//	解锁
	procconfig_unlock( uOfflineIndex );

	return bRet;
}


/**
 *	@ Private
 *	是否包含在待处理域名列表中
 */
LPSTDOMAIN procconfig_get_matched_domain( LPCSTR lpcszOrgDomain, UINT uOrgDomainLength )
{
	//
	//	lpcszOrgDomain		- [in] HTTP 包修改前，完整的原始域名，例如：abc.chinapig.cn
	//	uOrgDomainLength	- [in] 原始域名的长度
	//	RETURN			- 指向全局配置信息的匹配上的节点指针 或者 NULL
	//
	STDOMAIN * pstRet;

	STDOMAIN * pstDataTemp;
	STCONFIGDATA * pstCurrCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;


	if ( NULL == lpcszOrgDomain )
	{
		return NULL;
	}
	if ( 0 == uOrgDomainLength || uOrgDomainLength >= MAX_PATH )
	{
		return NULL;
	}


	//	返回值
	pstRet	= NULL;


	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData && 
			pstCurrCfgData->linkDomainSingle.Next )
		{
			//
			//	查询域名是否在列表中
			//
			pLinkMove = pstCurrCfgData->linkDomainSingle.Next;
			while( pLinkMove )
			{
				//
				//	输入：abc.chinapig.cn
				//	配置：   .chinapig.cn
				//
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STDOMAIN, ListEntry );

				//
				//	从后面向前面匹配
				//	并且越长的域名越排列在配置链表的最前面
				//
				if ( pstDataTemp &&
					uOrgDomainLength >= pstDataTemp->uDomainLength &&
					0 == _strnicmp( lpcszOrgDomain + ( uOrgDomainLength - pstDataTemp->uDomainLength ),
							pstDataTemp->szDomain,
							pstDataTemp->uDomainLength )
				)
				{
					pstRet = pstDataTemp;
					break;
				}

				pLinkMove = pLinkMove->Next;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_get_matched_domain\n" ));
	}	

	return pstRet;
}

/**
 *	@ Private
 *	检测 RDP clientname 是否存在于白名单
 */
BOOLEAN procconfig_is_exist_rdp_clientname( LPCWSTR lpcwszClientName, UINT uLength )
{
	//
	//	lpcwszClientName	- [in] ClientName
	//	uLength			- [in] 原始域名的长度
	//	RETURN			- TRUE / FALSE
	//
	BOOLEAN bRet;
	STRDPCLIENTNAME * pstDataTemp;
	STCONFIGDATA * pstCurrCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;


	if ( NULL == lpcwszClientName )
	{
		return FALSE;
	}
	if ( 0 == uLength || uLength >= 16 )
	{
		return FALSE;
	}


	//	返回值
	bRet = FALSE;

	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData )
		{
			if ( pstCurrCfgData->uRdpClientNameSingleCount > 0 )
			{
				if ( pstCurrCfgData->linkRdpClientNameSingle.Next )
				{
					//
					//	查询域名是否在列表中
					//
					pLinkMove = pstCurrCfgData->linkRdpClientNameSingle.Next;
					while( pLinkMove )
					{
						pstDataTemp = CONTAINING_RECORD( pLinkMove, STRDPCLIENTNAME, ListEntry );

						//
						//	从后面向前面匹配
						//	并且越长的域名越排列在配置链表的最前面
						//
						if ( pstDataTemp &&
							uLength == pstDataTemp->uLength &&
							0 == _wcsnicmp( lpcwszClientName, pstDataTemp->wszClientName, pstDataTemp->uLength/sizeof(WCHAR) ) )
						{
							bRet = TRUE;
							break;
						}

						pLinkMove = pLinkMove->Next;
					}
				}
				else
				{
					//	可能是 用户未添加任何计算机名，放过
					bRet = TRUE;
				}
			}
			else
			{
				//	用户未添加任何计算机名，放过
				bRet = TRUE;
			}
		}
		else
		{
			//	配置数据库错误，放过
			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_exist_rdp_clientname\n" ));
	}	

	return bRet;
}


/**
 *	将以某种字符分隔开的字符串，以此分开到数组中
 */
UINT procconfig_split_string_to_array( IN LPCSTR lpcszStringList, IN UCHAR uChrSpliter, OUT BYTE pbtArray[][PROCCONFIG_MAX_EXT_LENGTH], IN DWORD dwArrayCount )
{
	//
	//	lpcszStringList	- [in]  扩展名列表，例如：".gif.jpg.bmp."，要求 lpcszStringList 必须以 '.' 结尾
	//	pbtArray	- [out] 要创建的数组的内存地址，这里是一个指针数组指向那个 2 维的数组
	//	dwArrayCount	- [in]  数组 1 维的个数
	//	RETURN		- 成功分割到数组中的项的个数
	//

	UINT   uRet;
	LPCSTR lpcszHead;
	LPCSTR lpcszMove;
	UINT   uPosStart;
	UINT   uPosEnd;
	INT    nExtLength;
	UINT   i;
	UCHAR  uChar;

	if ( NULL == lpcszStringList || NULL == pbtArray || 0 == dwArrayCount )
	{
		return 0;
	}
	if ( 0 == lpcszStringList[0] )
	{
		return 0;
	}

	//
	//	初始化
	//
	uRet = 0;

	//
	//	要求 lpcszStringList 必须以 '.' 结尾
	//
	if ( uChrSpliter == lpcszStringList[ strlen(lpcszStringList) -sizeof(CHAR) ] )
	{
		//	Callers of RtlZeroMemory can be running at any IRQL
		RtlZeroMemory( pbtArray, sizeof(pbtArray[0])*dwArrayCount );
		
		lpcszHead = lpcszStringList;
		while( TRUE )
		{
			lpcszHead = strchr( lpcszHead, uChrSpliter );
			if ( lpcszHead )
			{
				lpcszHead += sizeof(CHAR);
				if ( lpcszHead )
				{
					lpcszMove = strchr( lpcszHead, uChrSpliter );
					if ( lpcszMove )
					{
						//
						//
						//
						uPosStart	= (UINT)( lpcszHead - lpcszStringList );
						uPosEnd		= (UINT)( lpcszMove - lpcszStringList );
						nExtLength	= (INT)( uPosEnd - uPosStart );
						
						if ( nExtLength > 0 && nExtLength < PROCCONFIG_MAX_EXT_LENGTH )
						{
							//
							//	增加计数器，返回处理的扩展名个数
							//
							if ( uRet < dwArrayCount )
							{
								_snprintf( pbtArray[ uRet ], nExtLength, "%s", lpcszStringList + uPosStart );
								uRet ++;
							}
							else
							{
								break;
							}
						}
						
						lpcszHead = lpcszMove;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	return uRet;
}


/**
 *	添加新的被保护目录
 */
BOOLEAN procconfig_addnew_protected_dir( LPSTPROTECTEDDIR pstProtectedDir )
{
	//
	//	pstProtectedDir		- [in] 被保护目录的配置信息
	//	RETURN			- TRUE / FLASE
	//	注意：
	//		添加配置操作永远操作的是 offline 的数据
	//		所以不需要上锁
	//

	BOOL  bRet;
	STCONFIGDATA * pstOfflineCfgData;
	STPROTECTEDDIR * pstNewItem;
	STPROTECTEDDIR * pstDataTemp;
	SINGLE_LIST_ENTRY * plinkTarget;
	SINGLE_LIST_ENTRY * pLinkMove;
	UINT  * puProtedDirCount;

	UINT  uOfflineIndex;
	BOOL  bAddThis;
	BOOL  bExistDirInDisk;
	BOOL  bExistItem;
	PWCHAR pwszNewDir	= NULL;
	WCHAR  wcDiskLwr;
	NTSTATUS ntStatus;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pstProtectedDir )
	{
		return FALSE;
	}

	//
	//	返回值初始化
	//
	bRet = FALSE;


	//
	//	添加 offline 配置区域
	//
	uOfflineIndex	= procconfig_stconfig_get_offline_index();
	
	
	//	上锁
	procconfig_lock( uOfflineIndex );


	__try
	{
		//
		//	检查在物理磁盘上，该目录是否存在
		//
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			//	函数 drvfunc_is_dir_exist_w 必须运行在 PASSIVE_LEVEL 级别
			bExistDirInDisk = drvfunc_is_dir_exist_w( pstProtectedDir->wszDir );
		}
		else
		{
			//	如果不是在 PASSIVE_LEVEL 级别的话，
			//	就认为目录存在吧，实在没有什么办法
			bExistDirInDisk = TRUE;
		}

		/*
		bExistDirInDisk	= FALSE;
		//	Callers of ExAllocatePool must be executing at IRQL <= DISPATCH_LEVEL
		pwszNewDir = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_WPATH );
		if ( pwszNewDir )
		{
			RtlZeroMemory( pwszNewDir, MAX_WPATH );

			ntStatus = drvfunc_c2w( pstProtectedDir->uszDir, pwszNewDir, MAX_WPATH );
			if ( NT_SUCCESS(ntStatus) )
			{
				bExistDirInDisk = drvfunc_is_dir_exist_w( pwszNewDir );
			}

			ExFreePool( pwszNewDir );
			pwszNewDir = NULL;
		}
		*/

		if ( bExistDirInDisk )
		{
			//
			//	判断该元素是否存在
			//
			pstOfflineCfgData = & g_stCfg.stCfgData[ uOfflineIndex ];
			
			//
			//	标记一下该磁盘被保护了
			//
			wcDiskLwr = pstProtectedDir->wszDir[ 0 ];
			if ( L'A' <= wcDiskLwr && wcDiskLwr <= L'Z' )
			{
				wcDiskLwr += ( L'a' - L'A' );
			}
			pstOfflineCfgData->wszProtedDisk[ wcDiskLwr ]	= 1;

			//
			//	...
			//
			if ( pstProtectedDir->bProtSubDir )
			{
				//	保护目录及其子目录下的文件
				plinkTarget = & ( pstOfflineCfgData->linkProtedDirTree );
				puProtedDirCount = & ( pstOfflineCfgData->uProtedDirTreeCount );
			}
			else
			{
				//	仅仅保护目录下文件
				plinkTarget = & ( pstOfflineCfgData->linkProtedDirSingle );
				puProtedDirCount = & ( pstOfflineCfgData->uProtedDirSingleCount );
			}
			if ( plinkTarget && puProtedDirCount )
			{
				bExistItem = FALSE;

				//	...
				pLinkMove = (*plinkTarget).Next;
				while( pLinkMove )
				{
					//
					//	被保护目录
					//
					pstDataTemp = CONTAINING_RECORD( pLinkMove, STPROTECTEDDIR, ListEntry );
					
					//
					//	从后面向前面匹配
					//	并且越长的域名越排列在配置链表的最前面
					//
					if ( pstDataTemp &&
						pstProtectedDir->uDirLength == pstDataTemp->uDirLength &&
						0 == _wcsnicmp( pstProtectedDir->wszDir, pstDataTemp->wszDir, pstDataTemp->uDirLength/sizeof(WCHAR) ) )
					{
						bExistItem = TRUE;
						break;
					}
					pLinkMove = pLinkMove->Next;
				}

				if ( ! bExistItem )
				{
					pstNewItem = (LPSTPROTECTEDDIR)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolProtectedDir );
					if ( pstNewItem )
					{
						//	拷贝
						RtlMoveMemory( pstNewItem, pstProtectedDir, sizeof(STPROTECTEDDIR) );

						//
						//	插入链表
						//	PushEntryList sets ListHead->Next to Entry,
						//	and Entry->Next to point to the old first entry of the list.			
						//
						//	insert into tree ptotected dir
						//
						PushEntryList( plinkTarget, &pstNewItem->ListEntry );
						(*puProtedDirCount) ++;

						bRet = TRUE;
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_protected_dir\n" ));
	}

	//	解锁
	procconfig_unlock( uOfflineIndex );

	//	...
	return bRet;
}

/**
 *	是否是被保护的磁盘
 */
BOOLEAN	procconfig_is_protected_disk( WCHAR wszDisk )
{
	BOOLEAN  bRet;
	STCONFIGDATA * pstCurrCfgData;
	WCHAR wcDiskLwr;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( ! procconfig_is_start() )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}
	if ( ! g_stCfg.bStartFileSafe )
	{
		return FALSE;
	}

	//	初始化
	bRet = FALSE;

	//
	//	上锁
	//
	procconfig_lock( -1 );


	__try
	{
		//
		//	标记一下该磁盘被保护了
		//
		wcDiskLwr = wszDisk;
		if ( L'A' <= wcDiskLwr && wcDiskLwr <= L'Z' )
		{
			wcDiskLwr += ( L'a' - L'A' );
		}

		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData )
		{
			if ( 1 == pstCurrCfgData->wszProtedDisk[ wcDiskLwr ] )
			{
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_protected_disk\n" ));
	}

	//
	//	解锁
	//
	procconfig_unlock( -1 );

	return bRet;
}

/**
 *	是否是被保护目录
 */
BOOLEAN procconfig_is_protected_dir( IN WCHAR * lpwszFilePath, IN UINT uFilePathLength, IN BOOLEAN bCheckAllParentDirs, IN HANDLE hFileHandle )
{
	//
	//	lpuszFilePath		- [in] 输入文件名，要保证是正常的 DOS 格式，非设备格式，即，不能有 "\??\" 开头
	//	uFilePathLength		- [in] lpuszFilePath 的长度，以 byte 为单位
	//	bCheckAllParentDirs	- [in] 是否也包含其所有级父目录的检测，默认是 FLASE，只有在目录重命名判断时用到
	//	RETURN			- TRUE / FALSE
	//

	BOOLEAN  bRet;
	STPROTECTEDDIR * pstDataProtectedDir;
	ULONG ulFilePathCrc32;
	ULONG ulDirCrc32;
	STCONFIGDATA * pstCurrCfgData;
	SINGLE_LIST_ENTRY * pLinkMove;
	INT   nDirLen;
	INT   i;
	BOOLEAN bPathIsDir;
	//UCHAR uChar;

	NTSTATUS ntStatus;
	IO_STATUS_BLOCK stIoStatus;
	FILE_STANDARD_INFORMATION stStandardInfo;


	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( ! procconfig_is_start() )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}
	if ( ! g_stCfg.bStartFileSafe )
	{
		return FALSE;
	}
	if ( NULL == lpwszFilePath )
	{
		return FALSE;
	}
	if ( 0 == uFilePathLength || uFilePathLength >= MAX_PATH )
	{
		return FALSE;
	}

	//	初始化
	bRet = FALSE;
	bPathIsDir = FALSE;

	//
	//	上锁
	//
	procconfig_lock( -1 );


	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData &&
			( pstCurrCfgData->uProtedDirTreeCount > 0 || pstCurrCfgData->uProtedDirSingleCount > 0 ) )
		{
			bPathIsDir = FALSE;
			if ( hFileHandle )
			{
				//
				ntStatus = ZwQueryInformationFile( hFileHandle, &stIoStatus, &stStandardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );
				if ( NT_SUCCESS( ntStatus ) )
				{
					bPathIsDir = stStandardInfo.Directory;
				}
			}

			//
			//	找到文件全路径中，目录的结束点
			//	nDirLen = bytes
			//
			if ( bPathIsDir )
			{
				nDirLen = uFilePathLength;
			}
			else
			{
				nDirLen = drvfunc_get_dir_length_w( lpwszFilePath, uFilePathLength );

				//	用长度的方法，检查“待检测路径”是否是目录
				bPathIsDir = ( nDirLen == uFilePathLength ? TRUE : FALSE );
			}
			if ( nDirLen > 0 )
			{
				//
				//	获取自动取小写后的字符串的 Crc32 的值
				//
				ulDirCrc32 = crc32_autolwr_w( lpwszFilePath, nDirLen );
				if ( bPathIsDir )
				{
					ulFilePathCrc32 = ulDirCrc32;
				}
				else
				{
					ulFilePathCrc32 = crc32_autolwr_w( lpwszFilePath, uFilePathLength );
				}

				if ( procconfig_is_protected_dir_by_cache_crc32( pstCurrCfgData, ulFilePathCrc32 ) )
				{
					//
					//	在 cache 中找到了 该路径 是被保护目录
					//
					bRet = TRUE;
				}
				else
				{
					//
					//	包含子目录的 优先处理
					//
					pLinkMove = pstCurrCfgData->linkProtedDirTree.Next;
					while( pLinkMove )
					{
						pstDataProtectedDir = CONTAINING_RECORD( pLinkMove, STPROTECTEDDIR, ListEntry );
						if ( pstDataProtectedDir )
						{
							if ( nDirLen >= pstDataProtectedDir->uDirLength )
							{
								//
								//	情况大概如下：
								//	lpuszFilePath=
								//	"C:\Inetpub\wwwroot\images"
								//	"C:\Inetpub\wwwroot\"
								//	"C:\Inetpub\wwwroot"
								//
								//	pstDataProtectedDir->uszDir=
								//	"C:\Inetpub\wwwroot"	(配置路径后面的“\”被强制去掉)
								//

								//
								//	配置数据里 可能 是待检测目录或者其子目录
								//
								if ( 0 == _wcsnicmp( lpwszFilePath, pstDataProtectedDir->wszDir, pstDataProtectedDir->uDirLength/sizeof(WCHAR) ) )
								{
									if ( bPathIsDir )
									{
										//	待检测路径是目录，没有必要再检测其扩展名
										//	则直接返回 TRUE
										bRet = TRUE;
										break;
									}
									else
									{
										//	待检测路径是文件，需要继续判断其扩展名...
										if ( procconfig_is_protected_ext( pstDataProtectedDir, lpwszFilePath, uFilePathLength ) )
										{
											//	是否是例外
											if ( ! procconfig_is_excepted_ext( pstDataProtectedDir, lpwszFilePath, uFilePathLength ) )
											{
												bRet = TRUE;
												break;
											}
										}
									}
								}
							}
							else
							{
								//
								//	情况大概如下：
								//	lpuszFilePath=
								//	"C:\Inetpub\"
								//	"C:\Inetpub"
								//
								//	pstDataProtectedDir->uszDir=
								//	"C:\Inetpub\wwwroot"	(配置路径后面的“\”被强制去掉)
								//

								//
								//	待检测目录比配置数据的目录短
								//
								if ( bCheckAllParentDirs && bPathIsDir )
								{
									//
									//	主要防止父级别目录被重命名
									//
									//	如果指定要检测待检测目录的所有级父目录的话，
									//	
									//
									if ( 0 == _wcsnicmp( pstDataProtectedDir->wszDir, lpwszFilePath, nDirLen/sizeof(WCHAR) ) )
									{
										if ( '\\' == lpwszFilePath[ nDirLen/sizeof(WCHAR) - 1 ] || '/' == lpwszFilePath[ nDirLen/sizeof(WCHAR) - 1 ] )
										{
											//	lpuszFilePath="C:\Inetpub\"
											bRet = TRUE;
											break;
										}
										else if ( '\\' == pstDataProtectedDir->wszDir[ nDirLen/sizeof(WCHAR) ] || '/' == pstDataProtectedDir->wszDir[ nDirLen/sizeof(WCHAR) ] )
										{
											//	lpuszFilePath="C:\Inetpub"
											bRet = TRUE;
											break;
										}
									}
								}
							}
						}

						pLinkMove = pLinkMove->Next;
					}
					
					//
					//	在查找，不包含子目录的
					//
					if ( ! bRet )
					{
						pLinkMove = pstCurrCfgData->linkProtedDirSingle.Next;
						while( pLinkMove )
						{
							pstDataProtectedDir = CONTAINING_RECORD( pLinkMove, STPROTECTEDDIR, ListEntry );
							if ( pstDataProtectedDir )
							{
								if ( nDirLen == pstDataProtectedDir->uDirLength )
								{
									//
									//	情况大概如下：
									//	lpuszFilePath=
									//	"C:\Inetpub\wwwroot\"
									//	"C:\Inetpub\wwwroot"
									//
									//	pstDataProtectedDir->uszDir=
									//	"C:\Inetpub\wwwroot"	(配置路径后面的“\”被强制去掉)
									//
									
									//
									//	配置数据里 可能 是待检测目录
									//
									if ( ulDirCrc32 == pstDataProtectedDir->ulDirCrc32 || 
										0 == _wcsnicmp( pstDataProtectedDir->wszDir, lpwszFilePath, pstDataProtectedDir->uDirLength/sizeof(WCHAR) ) )
									{
										if ( bPathIsDir )
										{
											//	待检测路径是目录，没有必要再检测其扩展名
											//	则直接返回 TRUE
											bRet = TRUE;
											break;
										}
										else
										{
											//	待检测路径是文件，需要继续判断其扩展名...
											if ( procconfig_is_protected_ext( pstDataProtectedDir, lpwszFilePath, uFilePathLength ) )
											{
												//	是否是例外
												if ( ! procconfig_is_excepted_ext( pstDataProtectedDir, lpwszFilePath, uFilePathLength ) )
												{
													bRet = TRUE;
													break;
												}
											}
										}
									}
								}
								else if ( nDirLen < pstDataProtectedDir->uDirLength )
								{
									//
									//	情况大概如下：
									//	lpuszFilePath=
									//	"C:\Inetpub\"
									//	"C:\Inetpub"
									//
									//	pstDataProtectedDir->uszDir=
									//	"C:\Inetpub\wwwroot"	(配置路径后面的“\”被强制去掉)
									//
									
									//
									//	待检测目录比配置数据的目录短
									//
									if ( bCheckAllParentDirs && bPathIsDir )
									{
										//
										//	主要防止父级别目录被重命名
										//
										//	如果指定要检测待检测目录的所有级父目录的话，
										//	
										//
										if ( 0 == _wcsnicmp( pstDataProtectedDir->wszDir, lpwszFilePath, nDirLen/sizeof(WCHAR) ) )
										{
											if ( '\\' == lpwszFilePath[ nDirLen/sizeof(WCHAR) - 1 ] || '/' == lpwszFilePath[ nDirLen/sizeof(WCHAR) - 1 ] )
											{
												//	lpuszFilePath="C:\Inetpub\"
												bRet = TRUE;
												break;
											}
											else if ( '\\' == pstDataProtectedDir->wszDir[ nDirLen/sizeof(WCHAR) ] || '/' == pstDataProtectedDir->wszDir[ nDirLen/sizeof(WCHAR) ] )
											{
												//	lpuszFilePath="C:\Inetpub"
												bRet = TRUE;
												break;
											}
										}
									}
								}
							}

							pLinkMove = pLinkMove->Next;
						}
					}

					//
					//	将 ulFilePathCrc32 添加到缓存中
					//
					procconfig_addnew_protected_dir_cache_crc32( pstCurrCfgData, ulFilePathCrc32, bRet );
				}

			} // end of if ( nDirLen > 0 )
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_protected_dir\n" ));
	}


	//
	//	解锁
	//
	procconfig_unlock( -1 );


	return bRet;
}


/**
 *	是否是被保护扩展名
 */
BOOLEAN	procconfig_is_protected_ext( STPROTECTEDDIR * pstProtectedDir, LPCWSTR lpcwszFilePath, UINT uFilePathLength )
{
	//
	//	pstProtectedDir		- protected dir
	//	lpcwszFilePath		- wide chars
	//	uFilePathLength		- in bytes
	//
	BOOL  bRet;
	INT   nExtPos;
	INT   nExtCount;
	UINT  i;
	//UCHAR uChar;

	if ( NULL == pstProtectedDir || NULL == lpcwszFilePath || 0 == uFilePathLength )
	{
		return FALSE;
	}

	if ( pstProtectedDir->bProtAllExt )
	{
		//	如果保护所有扩展名，那么就不用判断了
		
		//	直接返回 TRUE
		return TRUE;
	}
	else
	{
		//	保护部分指定扩展名
		
		if ( 0 == pstProtectedDir->uProtectedExtCount )
		{
			//
			//	满足两个条件：
			//	1，未设置保护所有扩展名标记
			//	2，指定被保护扩展名个数也是0
			//	那么直接返回 FALSE
			//
			return FALSE;
		}
	}

	bRet		= FALSE;
	nExtCount	= 0;

	for ( nExtPos = ( uFilePathLength/sizeof(WCHAR) - 1 ); nExtPos >= 0; nExtPos -- )
	{
		if ( '.' == lpcwszFilePath[ nExtPos ] )
		{
			//	放弃 "." 的位置，向后挪一个位置
			//	例如：“exe”、“gif”
			nExtPos ++;

			//	计算扩展名的长度 nExtCount in chars
			nExtCount = uFilePathLength/sizeof(WCHAR) - nExtPos;

			//	结束
			break;
		}
	}

	if ( nExtCount > 0 && nExtCount < PROCCONFIG_MAX_EXT_LENGTH )
	{
		for ( i = 0; i < pstProtectedDir->uProtectedExtCount; i ++ )
		{
			if ( 0 == _wcsnicmp( pstProtectedDir->wszProtectedExt[ i ], lpcwszFilePath + nExtPos, nExtCount ) )
			{
				//	到此发现的确是用户设置被保护的扩展名！
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}



/**
 *	是否是例外的扩展名
 */
BOOLEAN	procconfig_is_excepted_ext( STPROTECTEDDIR * pstProtectedDir, LPCWSTR lpcwszFilePath, UINT uFilePathLength )
{
	BOOL  bRet;
	INT   nExtPos;
	INT   nExtCount;
	UINT  i;
	//UCHAR uChar;

	if ( NULL == pstProtectedDir || NULL == lpcwszFilePath || 0 == uFilePathLength )
	{
		return FALSE;
	}
	if ( 0 == pstProtectedDir->uExceptedExtCount )
	{
		return FALSE;
	}

	bRet		= FALSE;
	nExtCount	= 0;

	for ( nExtPos = ( uFilePathLength/sizeof(WCHAR) - 1 ); nExtPos >= 0; nExtPos -- )
	{
		if ( '.' == lpcwszFilePath[ nExtPos ] )
		{
			//	放弃 "." 的位置，向后挪一个位置
			//	例如：“exe”、“gif”
			nExtPos ++;
			
			//	计算扩展名的长度
			nExtCount = uFilePathLength/sizeof(WCHAR) - nExtPos;
			
			//	结束
			break;
		}
	}

	if ( nExtCount > 1 && nExtCount < PROCCONFIG_MAX_EXT_LENGTH )
	{
		for ( i = 0; i < pstProtectedDir->uExceptedExtCount; i ++ )
		{
			if ( 0 == _wcsnicmp( pstProtectedDir->wszExceptedExt[ i ], lpcwszFilePath + nExtPos, nExtCount ) )
			{
				//	到此发现的确是例外了哦！
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}




/**
 *	@ Private
 *	二叉树缓存(被保护目录的 Crc32 值做的树)
 */
BOOLEAN procconfig_addnew_protected_dir_cache_crc32( STCONFIGDATA * pstCurrCfgData, ULONG ulCrc32, BOOLEAN bProtectedDir )
{
	BOOLEAN bRet;
	STPROTECTEDDIRCACHE * pstMove;
	STPROTECTEDDIRCACHE * pstLast;
	STPROTECTEDDIRCACHE * pstNewItem;
	BOOLEAN bFoundItem;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}
	if ( NULL == pstCurrCfgData )
	{
		return FALSE;
	}
	if ( pstCurrCfgData->uProtectedDirCacheCount > PROCCONFIG_MAX_PROTECTEDDIR_CACHE_SIZE )
	{
		//	超过二叉树最大元素个数限制
		return FALSE;
	}

	//	初始化
	bRet = FALSE;
	
	
	__try
	{
		pstMove		= NULL;
		pstLast		= NULL;
		bFoundItem	= FALSE;

		pstMove = pstCurrCfgData->pstProtectedDirCacheHeader;
		while ( pstMove )
		{
			if ( ulCrc32 > pstMove->ulCrc32 )
			{
				pstLast = pstMove;
				pstMove = pstLast->pstLeft;
			}
			else if ( ulCrc32 < pstMove->ulCrc32 )
			{
				pstLast = pstMove;
				pstMove = pstLast->pstRight;
			}
			else if ( ulCrc32 == pstMove->ulCrc32 )
			{
				bFoundItem = TRUE;
				break;
			}
		}

		if ( bFoundItem )
		{
			//	如果已经存在，则认为添加成功
			bRet = TRUE;
		}
		else
		{
			pstNewItem = (STPROTECTEDDIRCACHE*)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache );
			if ( pstNewItem )
			{
				RtlZeroMemory( pstNewItem, sizeof(STPROTECTEDDIRCACHE) );
				
				pstNewItem->ulCrc32		= ulCrc32;
				pstNewItem->bProtectedDir	= bProtectedDir;
				pstNewItem->pstLeft		= NULL;
				pstNewItem->pstRight		= NULL;

				//	将新申请来的空间给树
				if ( NULL == pstCurrCfgData->pstProtectedDirCacheHeader )
				{
					// 创建第一个结点
					pstCurrCfgData->pstProtectedDirCacheHeader = pstNewItem;
				}
				else
				{
					//	创建后续结点
					if ( ulCrc32 > pstLast->ulCrc32 )
					{
						pstLast->pstLeft = pstNewItem;
					}
					else if ( ulCrc32 < pstLast->ulCrc32 )
					{
						pstLast->pstRight = pstNewItem;
					}
				}

				//
				//	Use following code to free the memony used by cache item
				//	--------------------------------------------------------------------------------
				//	ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache, pstNewItem );
				//	pstNewItem = NULL;
				//

				//	增加计数
				pstCurrCfgData->uProtectedDirCacheCount ++;

				//
				//	添加成功
				//
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_protected_dir_cache_crc32\n" ));
	}

	return bRet;
}

/**
 *	@ Private
 *	从二叉树缓存中检索待检测对象是否是被保护对象(被保护目录的 Crc32 值做的树)
 */
BOOLEAN procconfig_is_protected_dir_by_cache_crc32( STCONFIGDATA * pstCurrCfgData, ULONG ulCrc32 )
{
	BOOLEAN bRet;
	STPROTECTEDDIRCACHE * pstMove;
	STPROTECTEDDIRCACHE * pstLast;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	if ( procconfig_is_loading() )
	{
		return FALSE;
	}
	if ( NULL == pstCurrCfgData )
	{
		return FALSE;
	}
	
	//	初始化
	bRet = FALSE;

	__try
	{
		pstMove		= NULL;
		pstLast		= NULL;

		pstMove = pstCurrCfgData->pstProtectedDirCacheHeader;
		while ( pstMove )
		{
			if ( ulCrc32 > pstMove->ulCrc32 )
			{
				pstLast = pstMove;
				pstMove = pstLast->pstLeft;
			}
			else if ( ulCrc32 < pstMove->ulCrc32 )
			{
				pstLast = pstMove;
				pstMove = pstLast->pstRight;
			}
			else if ( ulCrc32 == pstMove->ulCrc32 )
			{
				//	是否是被保护对象
				bRet = pstMove->bProtectedDir;
				break;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_protected_dir_by_cache_crc32\n" ));
	}

	return bRet;
}
BOOLEAN procconfig_clean_protected_dir_cache_crc32( STCONFIGDATA * pstCurrCfgData, STPROTECTEDDIRCACHE * pstProtectedDirCache )
{
	BOOLEAN bRet;
	
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return FALSE;
	//}
	if ( ! g_stCfg.bInited )
	{
		return FALSE;
	}
	//if ( procconfig_is_loading() )
	//{
	//	return FALSE;
	//}
	if ( NULL == pstCurrCfgData )
	{
		return FALSE;
	}

	//	初始化
	bRet = FALSE;

	__try
	{
		if ( pstProtectedDirCache )
		{
			//	遍历 清理左右孩子
			procconfig_clean_protected_dir_cache_crc32( pstCurrCfgData, pstProtectedDirCache->pstLeft );
			procconfig_clean_protected_dir_cache_crc32( pstCurrCfgData, pstProtectedDirCache->pstRight );
			
			//	减少计数
			pstCurrCfgData->uProtectedDirCacheCount --;

			//	释放内存
			ExFreeToNPagedLookasideList( &g_npgProcConfigPoolProtectedDirCache, pstProtectedDirCache );
			pstProtectedDirCache = NULL;

			//	...
			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_clean_protected_dir_cache_crc32\n" ));
	}
	
	return bRet;
}





/**
 *	添加新的例外进程
 */
BOOLEAN procconfig_addnew_excepted_process( LPSTEXCEPTEDPROCESS pstExceptedProcess )
{
	//
	//	pstExceptedProcess	- [in] 例外进程信息
	//	RETURN			- TRUE / FLASE
	//	注意：
	//		添加配置操作永远操作的是 offline 的数据
	//		所以不需要上锁
	//

	BOOL  bRet;
	STCONFIGDATA * pstOfflineCfgData;
	STEXCEPTEDPROCESS * pstNewItem;
	STEXCEPTEDPROCESS * pstDataTemp;
	//SINGLE_LIST_ENTRY * plinkTarget;
	SINGLE_LIST_ENTRY * pLinkMove;

	BOOL  bExistDirInDisk;

	UINT  uOfflineIndex;
	//BOOL  bAddThis;
	BOOL  bExistItem;
	//PWCHAR pwszNewDir	= NULL;
	//NTSTATUS ntStatus;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pstExceptedProcess )
	{
		return FALSE;
	}

	//
	//	返回值初始化
	//
	bRet = FALSE;


	//
	//	添加 offline 配置区域
	//
	uOfflineIndex	= procconfig_stconfig_get_offline_index();
	
	
	//	上锁
	procconfig_lock( uOfflineIndex );


	__try
	{
		//
		//	检查在物理磁盘上，该目录是否存在
		//
		if ( PASSIVE_LEVEL == KeGetCurrentIrql() )
		{
			//	函数 drvfunc_is_file_exist_a 必须运行在 PASSIVE_LEVEL 级别
			bExistDirInDisk = drvfunc_is_file_exist_a( pstExceptedProcess->uszDosPath );
		}
		else
		{
			//	如果不是在 PASSIVE_LEVEL 级别的话，
			//	就认为目录存在吧，实在没有什么办法
			bExistDirInDisk = TRUE;
		}

		if ( bExistDirInDisk )
		{
			//
			//	判断该元素是否存在
			//
			bExistItem		= FALSE;
			pstOfflineCfgData	= & g_stCfg.stCfgData[ uOfflineIndex ];

			if ( pstOfflineCfgData && 
				pstOfflineCfgData->linkExceptedProcessSingle.Next )
			{
				//
				//	确定已经有内容了
				//
				
				pLinkMove = pstOfflineCfgData->linkExceptedProcessSingle.Next;
				while( pLinkMove )
				{
					pstDataTemp = CONTAINING_RECORD( pLinkMove, STEXCEPTEDPROCESS, ListEntry );

					//
					//	从后面向前面匹配
					//
					if ( pstDataTemp &&
						pstExceptedProcess->uDosPathLength == pstDataTemp->uDosPathLength &&
						0 == _strnicmp( pstExceptedProcess->uszDosPath, pstDataTemp->uszDosPath, pstDataTemp->uDosPathLength ) )
					{
						bExistItem = TRUE;
						break;
					}
					
					pLinkMove = pLinkMove->Next;
				}
			}

			if ( ! bExistItem )
			{
				pstNewItem = (LPSTEXCEPTEDPROCESS)ExAllocateFromNPagedLookasideList( &g_npgProcConfigPoolExceptedProcess );
				if ( pstNewItem )
				{
					//	拷贝
					RtlMoveMemory( pstNewItem, pstExceptedProcess, sizeof(STEXCEPTEDPROCESS) );
					
					//
					//	插入链表
					//	PushEntryList sets ListHead->Next to Entry,
					//	and Entry->Next to point to the old first entry of the list.
					//
					PushEntryList( & pstOfflineCfgData->linkExceptedProcessSingle, &pstNewItem->ListEntry );
					pstOfflineCfgData->uExceptedProcessSingleCount ++;
					
					bRet = TRUE;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_addnew_excepted_process\n" ));
	}

	//	解锁
	procconfig_unlock( uOfflineIndex );

	//	...
	return bRet;
}

/**
 *	@ Public for <FsApiHook Action Checking>
 *	check if excepted process for accessing the files
 */
BOOLEAN procconfig_is_excepted_process()
{
	BOOLEAN bRet;
	HANDLE ProcessId;
	ULONG ulProcessIdTmp;
	WCHAR * pwszFullImagePath;
	USHORT uFullImagePathLength;
	ULONG ulFullImagePathCrc32U;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		ProcessId	= PsGetCurrentProcessId();
		ulProcessIdTmp	= 0;
		RtlMoveMemory( &ulProcessIdTmp, &ProcessId, sizeof(ProcessId) );

		if ( procprocess_get_fullimagepath_by_pid( ulProcessIdTmp, &pwszFullImagePath, &uFullImagePathLength, &ulFullImagePathCrc32U ) )
		{
			if ( procconfig_is_excepted_process_by_devicepath( pwszFullImagePath, uFullImagePathLength ) )
			{
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_excepted_process\n" ));
	}

	return bRet;
}
BOOLEAN procconfig_is_excepted_process_by_crc32u( ULONG ulFullImagePathCrc32U )
{
	//
	//	ulFullImagePathCrc32U	- [in] crc32 value of uszFullImagePath in UCHAR
	//	RETURN			- TRUE / FALSE
	//

	BOOLEAN bRet;
	STCONFIGDATA * pstCurrCfgData;
	STEXCEPTEDPROCESS * pstDataTemp;
	SINGLE_LIST_ENTRY * pLinkMove;
	
	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;
	
	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData && 
			pstCurrCfgData->linkExceptedProcessSingle.Next )
		{
			//
			//	查询域名是否在列表中
			//
			pLinkMove = pstCurrCfgData->linkExceptedProcessSingle.Next;
			while( pLinkMove )
			{
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STEXCEPTEDPROCESS, ListEntry );
				
				//
				//	从后面向前面匹配
				//
				if ( pstDataTemp &&
					ulFullImagePathCrc32U == pstDataTemp->ulDevicePathCrc32U )
				{
					bRet = TRUE;
					break;
				}
				
				pLinkMove = pLinkMove->Next;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_excepted_process_by_crc32u\n" ));
	}
	
	return bRet;
}
BOOLEAN procconfig_is_excepted_process_by_devicepath( PWSTR pwszDevicePath, USHORT uLengthInBytes )
{
	//
	//	pwszDevicePath	- [in] like "\Device\HarddiskVolume1\windows\explorer.exe"
	//	uLengthInBytes	- [in] length of the string pointed to by pwszDevicePath, in bytes.
	//	RETURN		- TRUE / FALSE
	//

	BOOLEAN bRet;
	STCONFIGDATA * pstCurrCfgData;
	STEXCEPTEDPROCESS * pstDataTemp;
	SINGLE_LIST_ENTRY * pLinkMove;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return FALSE;
	}
	if ( NULL == pwszDevicePath || 0 == uLengthInBytes )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		pstCurrCfgData = & g_stCfg.stCfgData[ g_stCfg.uCurrIndex ];
		if ( pstCurrCfgData && 
			pstCurrCfgData->linkExceptedProcessSingle.Next )
		{
			//
			//	查询域名是否在列表中
			//
			pLinkMove = pstCurrCfgData->linkExceptedProcessSingle.Next;
			while( pLinkMove )
			{
				pstDataTemp = CONTAINING_RECORD( pLinkMove, STEXCEPTEDPROCESS, ListEntry );
				
				//
				//	从后面向前面匹配
				//
				if ( pstDataTemp &&
					uLengthInBytes == pstDataTemp->uDevicePathLengthInBytesW &&
					0 == _wcsnicmp( pwszDevicePath, pstDataTemp->wszDevicePath, pstDataTemp->uDevicePathLengthInBytesW/sizeof(WCHAR) ) )
				{
					bRet = TRUE;
					break;
				}
				
				pLinkMove = pLinkMove->Next;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procconfig_is_excepted_process_by_devicepath\n" ));
	}
	
	return bRet;
}
