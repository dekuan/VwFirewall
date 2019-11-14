// MetaOpIIS.cpp: implementation of the CAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MetaOpIIS.h"





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMetaOpIIS::CMetaOpIIS()
{
	m_pIMeta	= NULL;
	m_pIAppAdmin	= NULL;

	m_wszBuf = new WCHAR[ METAOPIIS_BUFFER_SIZE ];
	ZeroMemory( m_wszBuf, METAOPIIS_BUFFER_SIZE*sizeof(WCHAR) );
}
CMetaOpIIS::CMetaOpIIS( STMETASITEINFO * pstSiteinfo )
{
//	memcpy( & m_stSiteInfo, pstSiteinfo, sizeof(STMETASITEINFO) );	
	m_pIMeta	= NULL;
	m_pIAppAdmin	= NULL;

	m_wszBuf = new WCHAR[ METAOPIIS_BUFFER_SIZE ];
	ZeroMemory( m_wszBuf, METAOPIIS_BUFFER_SIZE*sizeof(WCHAR) );
}
CMetaOpIIS & CMetaOpIIS::operator = ( STMETASITEINFO & stSiteinfo )
{
//	memcpy( & m_stSiteInfo, &stSiteinfo, sizeof(STMETASITEINFO) );	
	return *this;
}
CMetaOpIIS::~CMetaOpIIS(void)
{
	m_pIMeta	= NULL;
	m_pIAppAdmin	= NULL;

	if ( m_wszBuf )
	{
		delete [] m_wszBuf;
		m_wszBuf = NULL;
	}
}


/**
 *	@ Public
 *	初始化COM，并取得 IID_IDSAdminBase 接口
 */
BOOL CMetaOpIIS::initCom()
{
	HRESULT hRet = NULL;

	//	初始化类的借口
#ifndef __METAOPIIS_COINITIALIZE__
#define __METAOPIIS_COINITIALIZE__
	hRet = CoInitialize( NULL );
#endif

	if ( m_pIMeta == NULL )
	{
		hRet = CoCreateInstance( CLSID_MSAdminBase, NULL, CLSCTX_LOCAL_SERVER, IID_IMSAdminBase, (void **)&m_pIMeta );
		if ( FAILED(hRet) || m_pIMeta == NULL )
		{
			setLastErrorInfo( 0, "初始化IMSAdminBase接口失败" );
			return FALSE;
		}
	}

	if ( m_pIAppAdmin == NULL )
	{
		hRet = CoCreateInstance( CLSID_WamAdmin, NULL, CLSCTX_LOCAL_SERVER, IID_IWamAdmin2, (void **)&m_pIAppAdmin );
		if ( FAILED(hRet) || m_pIAppAdmin == NULL )
		{
			setLastErrorInfo( 0, "初始化IWamAdmin2接口失败" );
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	@ Public
 *	销毁 IID_IDSAdminBase 接口
 */
VOID CMetaOpIIS::uninitCom()
{
	if ( m_pIMeta != NULL )
	{
		m_pIMeta.Release();
	}
	if ( m_pIAppAdmin != NULL )
	{
		m_pIAppAdmin.Release();
	}
}

/**
 *	@ Public
 *	是否初始化就绪
 */
BOOL CMetaOpIIS::isInitComReady()
{
	return ( m_pIMeta && m_pIAppAdmin );
}

/**
 *	@ Public
 *	创建新站点
 */
BOOL CMetaOpIIS::createSite( STMETASITEINFO * pstSiteInfo )
{
	//
	//	dwKeyName	- [in] metabase 中站点的键名
	//	RETURN		- TRUE / FALSE
	//
	if ( NULL == pstSiteInfo )
	{
		return FALSE;
	}

	//	检查指向 MSAdminBase，IWamAdmin2 接口的灵敏指针
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "创建站点时，发现指向 MSAdminBase，IWamAdmin2 接口的灵敏指针未初始化" );
		return FALSE;
	}

	//	第一步：在/LM/W3SVC下，获取唯一站点健值
	DWORD dwKeyName = this->getNewSiteKey();
	if ( 0 == dwKeyName )
	{
		setLastErrorInfo( 0, "获取唯一站点健值失败" );
		return FALSE;
	}
	_snprintf( pstSiteInfo->szKeyName, sizeof(pstSiteInfo->szKeyName)-sizeof(TCHAR), "%d", dwKeyName );
	//_snwprintf

	
	//	第二步：使用第一步找出的数，建立一个主键
	WCHAR wszKeyNameOrg[ METADATA_MAX_NAME_LEN ]	= {0};
 	WCHAR wszBuf[ METAOPIIS_BUFFER_SIZE ]		= {0};
	STMETAOPERRECORD rec;
	INT nSBindingsLen;

	wcscpy( rec.wszKeyPath, L"/LM/W3SVC" );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szKeyName, strlen(pstSiteInfo->szKeyName)+1, rec.wszKeyName, METAOPIIS_KEYNAME_LEN );
	if ( ! mb_createKey( & rec ) )
	{
		setLastErrorInfo( 0, "新建站点主键失败" );
		return FALSE;
	}
	wcscpy( wszKeyNameOrg, rec.wszKeyName );


	//	第三步：设置主键下的各个属性
	//	设置 KEYTYPE	MD_KEY_TYPE
	_snwprintf( rec.wszKeyPath, sizeof(rec.wszKeyPath)-sizeof(WCHAR), L"/LM/W3SVC/%s", rec.wszKeyName );
	memset( wszBuf, 0, sizeof(wszBuf) );
	wcscpy( wszBuf, L"IIsWebServer" );
	if ( ! modifyRecordData( & rec, MD_KEY_TYPE, 0, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( 0, "设置主键下的各个属性失败" );
		return FALSE;
	}

	//	1001	设置	ServerComment
	if ( 0 == strlen(pstSiteInfo->szServerComment) )
	{
		setLastErrorInfo( 0, "szServerComment 数据无效" );
		return FALSE;
	}
	memset( wszBuf, 0, sizeof(wszBuf) );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szServerComment, strlen(pstSiteInfo->szServerComment)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
	if ( ! modifyRecordData( & rec, MD_SERVER_COMMENT, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( ERROR_MODIFY_SCOMMENT, "设置 ServerComment 失败" );
		return FALSE;		
	}

	//	1002	szAppPoolId
	if ( 0 == strlen(pstSiteInfo->szAppPoolId) )
	{
		setLastErrorInfo( 0, "szAppPoolId 数据无效" );
		return FALSE;
	}
	memset( wszBuf, 0, sizeof(wszBuf) );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szAppPoolId, strlen(pstSiteInfo->szAppPoolId)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
	if ( ! modifyRecordData( & rec, 9101, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( 0, "设置 szAppPoolId 失败" );
		return FALSE;
	}

	//	1003	设置 ServerState
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->dwServerState, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_SERVER_STATE, METADATA_VOLATILE, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_MODIFY_SSTATE, "设置 dwServerState 失败" );
	}

	//	1004	dwAllowKeepAlive
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &pstSiteInfo->dwAllowKeepAlive, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_ALLOW_KEEPALIVES, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 dwAllowKeepAlive 失败" );
		return FALSE;
	}

	//	1005	dwConnectionTimeout
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &pstSiteInfo->dwConnectionTimeout, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_CONNECTION_TIMEOUT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 dwConnectionTimeout 失败" );
		return FALSE;
	}

	//	1006 设置绑定信息
	if ( 0 == strlen(pstSiteInfo->szBindings) )
	{
		setLastErrorInfo( 0, "szBindings 数据无效" );
		return FALSE;
	}
	memset( wszBuf, 0, sizeof( wszBuf ) );
	if ( isValidSBindings( pstSiteInfo->szBindings ) )
	{
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szBindings, strlen(pstSiteInfo->szBindings)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		nSBindingsLen = formatSBindingsToMetabase( wszBuf );
		if ( ! modifyRecordData( & rec, MD_SERVER_BINDINGS, 0, IIS_MD_UT_SERVER, MULTISZ_METADATA, wszBuf, (nSBindingsLen)*2+2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_SBINDING, "设置绑定信息失败" );
			return FALSE;
		}
	}
	else
	{
		setLastErrorInfo( ERROR_INVALID_SB, "" );
	}

	//	1007	设置 MaxConnection
	if ( 0 != pstSiteInfo->lnMaxConnection )
	{
		//memset( wszBuf, 0xB0, sizeof(wszBuf) );
		memset( wszBuf, 0, sizeof(wszBuf) );
		memcpy( wszBuf, &pstSiteInfo->lnMaxConnection, sizeof(DWORD) );
		if ( ! modifyRecordData( & rec, MD_MAX_CONNECTIONS, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( ERROR_MODIFY_MAXCONNECTION, "设置 MaxConnection 失败" );
			return FALSE;
		}
	}

	//	1008	设定最大带宽限制
	if ( 0 != pstSiteInfo->lnMaxBindwidth )
	{
		//	不等于 -1 启用最大带宽限制
		//LONG lnBindWidth	= 1024 * pstSiteInfo->lnMaxBindwidth;
		LONG lnBindWidth	= pstSiteInfo->lnMaxBindwidth;
		//memset( wszBuf, 0xB0, sizeof(wszBuf) );
		memset( wszBuf, 0, sizeof(wszBuf) );
		memcpy( wszBuf, &lnBindWidth, sizeof(DWORD) );
		if ( ! modifyRecordData( & rec, MD_MAX_BANDWIDTH, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( ERROR_MODIFY_MAXCONNECTION, "设定最大带宽限制失败" );
			return FALSE;
		}
	}

	//	1009	设置是否启用日志
	//		启用日志记录项关闭 MD_LOG_TYPE
	if ( 0 != pstSiteInfo->lnLogType && 1 != pstSiteInfo->lnLogType )
	{
		pstSiteInfo->lnLogType = 0;
	}
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogType, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_LOG_TYPE, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_USE_LOGS, "设置是否启用日志失败" );
		return FALSE;		
	}

	//	1010	lnLogFileLocaltimeRollover
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogFileLocaltimeRollover, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, 4015, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 lnLogFileLocaltimeRollover 失败" );
		return FALSE;		
	}

	//	1011	lnLogFilePeriod
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogFilePeriod, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_LOGFILE_PERIOD, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 lnLogFilePeriod 失败" );
		return FALSE;		
	}

	//	1012	lnLogFileTruncateSize
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogFileTruncateSize, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_LOGFILE_TRUNCATE_SIZE, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 lnLogFileTruncateSize 失败" );
		return FALSE;		
	}

	//	1013	lnLogExtFileFlags
	memset( wszBuf, 0, sizeof( wszBuf ) );
	memcpy( wszBuf, &pstSiteInfo->lnLogExtFileFlags, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_LOGEXT_FIELD_MASK, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 lnLogExtFileFlags 失败" );
		return FALSE;		
	}

	//	1014	szLogFileDirectory
	if ( strlen( pstSiteInfo->szLogFileDirectory ) && PathIsDirectory( pstSiteInfo->szLogFileDirectory ) )
	{
		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szLogFileDirectory, strlen(pstSiteInfo->szLogFileDirectory)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_LOGFILE_DIRECTORY, METADATA_INHERIT, IIS_MD_UT_SERVER, EXPANDSZ_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( 0, "设置 szLogFileDirectory 失败" );
			return FALSE;		
		}
	}

	//	1015	szLogPluginClsid
	if ( strlen( pstSiteInfo->szLogPluginClsid ) )
	{
		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szLogPluginClsid, strlen(pstSiteInfo->szLogPluginClsid)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_LOG_PLUGIN_ORDER, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( 0, "设置 szLogPluginClsid 失败" );
			return FALSE;		
		}
	}

	//	1016	启用默认文档 szDefaultDoc
	if ( strlen( pstSiteInfo->szDefaultDoc ) )
	{
		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szDefaultDoc, strlen(pstSiteInfo->szDefaultDoc)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_DEFAULT_LOAD_FILE, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_DEFAULTDOC, "启用默认文档失败" );
			return FALSE;		
		}
	}



	//
	//	第四步：在该站点主键下建立一个Root键，假如键值为1则跳过
	//
	if ( 0 != wcscmp( rec.wszKeyName, L"1" ) )
	{
		//	拷贝新的值给 wszKeyName
		wcscpy( rec.wszKeyName, L"/Root" );
		if ( ! mb_createKey( & rec ) )
		{
			setLastErrorInfo( 0, "在该站点主键下建立一个 Root 键失败" );
			return FALSE;
		}
		//	还原 wszKeyName
		wcscpy( rec.wszKeyName, wszKeyNameOrg );
	}

	//	第五步：在Root键下设置各个属性
	//	设置 KEYTYPE / MD_KEY_TYPE
	_snwprintf( rec.wszKeyPath, sizeof(rec.wszKeyPath)-sizeof(WCHAR), L"/LM/W3SVC/%s/Root", rec.wszKeyName );
	memset( wszBuf, 0, sizeof(wszBuf) );
	wcscpy( wszBuf, L"IIsWebVirtualDir" );
	if ( ! modifyRecordData( & rec, MD_KEY_TYPE, 0, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( 0, "在 Root 键下设置各个属性失败" );
		return FALSE;		
	}

	//	1017	设置 ServerMDir
	if ( 0 == strlen(pstSiteInfo->szServerMDir) || ( ! PathIsDirectory( pstSiteInfo->szServerMDir ) ) )
	{
		setLastErrorInfo( 0, "szServerMDir 数据无效" );
		return FALSE;
	}
	memset( wszBuf, 0, sizeof( wszBuf ) );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szServerMDir, strlen(pstSiteInfo->szServerMDir)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
	if ( ! modifyRecordData( & rec, MD_VR_PATH, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
	{
		setLastErrorInfo( ERROR_MODIFY_MAINDIR, "设置 Path 失败" );
		return FALSE;		
	}

	//	1018	设置主目录访问属性
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->dwMDirAccessFlag, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_ACCESS_PERM, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_MODIFY_MDIRACCESS, "设置主目录访问属性失败" );
		return FALSE;		
	}

	//	1019	lnDirBrowseFlags
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->lnDirBrowseFlags, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_DIRECTORY_BROWSING, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 lnDirBrowseFlags 属性失败" );
		return FALSE;		
	}

	//	1020	dwDontLog
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->dwDontLog, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_DONT_LOG, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 dwDontLog 属性失败" );
		return FALSE;		
	}
	//	1021	dwContentIndexed
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->dwContentIndexed, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_IS_CONTENT_INDEXED, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 dwContentIndexed 属性失败" );
		return FALSE;		
	}

	//	1022	szHttpExpires
	if ( strlen( pstSiteInfo->szHttpExpires ) )
	{
		memset( wszBuf, 0, sizeof( wszBuf ) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szHttpExpires, strlen(pstSiteInfo->szHttpExpires)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_HTTP_EXPIRES, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
		{
			setLastErrorInfo( 0, "设置 szHttpExpires 失败" );
			return FALSE;		
		}
	}

	//	1023	lnEnableDocFooter
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, & pstSiteInfo->lnEnableDocFooter, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_FOOTER_ENABLED, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "设置 lnEnableDocFooter 属性失败" );
		return FALSE;		
	}

	//	1024	szDefaultDocFooter
	if ( strlen( pstSiteInfo->szDefaultDocFooter ) )
	{
		memset( wszBuf, 0, sizeof( wszBuf ) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szDefaultDocFooter, strlen(pstSiteInfo->szDefaultDocFooter)+1, wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_FOOTER_DOCUMENT, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
		{
			setLastErrorInfo( 0, "设置 szDefaultDocFooter 失败" );
			return FALSE;		
		}
	}


	//
	//	创建应用程序
	//
	memset( wszBuf, 0, sizeof(wszBuf) );
	wcscpy( wszBuf, rec.wszKeyPath );
	_wcsupr( wszBuf );
	if ( ! mb_createApp( pstSiteInfo, wszBuf, 2 ) )
	{
		setLastErrorInfo( ERROR_CREATE_APP, "创建应用程序失败" );
		return FALSE;
	}
	
	//	第六，使用 COMMAND 启动站点
	DWORD dwState	= MD_SERVER_COMMAND_START;
	switch( pstSiteInfo->dwServerState )
	{
	case MD_SERVER_COMMAND_START:
		{
			dwState = MD_SERVER_COMMAND_START;
		}
		break;
	case MD_SERVER_COMMAND_STOP:
		{
			dwState = MD_SERVER_COMMAND_STOP;
		}
		break;
	case MD_SERVER_COMMAND_PAUSE:
		{
			dwState = MD_SERVER_COMMAND_PAUSE;
		}
		break;
	}
	rec.wszKeyPath[ wcslen(rec.wszKeyPath) - 5 ] = 0x0000;	//	"/root"
	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &dwState, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_SERVER_COMMAND, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_STATE_COMMAND, "启动站点服务失败" );
		return FALSE;
	}

	return TRUE;
}

/**
 *	@ Public
 *	删除站点
 */
BOOL CMetaOpIIS::deleteSite( STMETASITEINFO * pstSiteInfo )
{
	if ( NULL == pstSiteInfo )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "删除站点时，发现指向 MSAdminBase，IWamAdmin2 接口的灵敏指针未初始化" );
		return FALSE;
	}

	//	直接删除主键
	STMETAOPERRECORD rec;
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC" );
	MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szKeyName, strlen(pstSiteInfo->szKeyName)+1, rec.wszKeyName, MAX_PATH );

	if ( ! mb_deleteKey( & rec ) )
	{
		setLastErrorInfo( 0, "直接删除主键 失败" );
		return FALSE;		
	}
	
	return TRUE;
}

/**
 *	@ Public
 *	修改站点信息
 */
BOOL CMetaOpIIS::modifySite( STMETASITEINFO * pstSiteInfo )
{
	if ( NULL == pstSiteInfo )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "修改站点信息时，发现指向 MSAdminBase，IWamAdmin2 接口的灵敏指针未初始化" );
		return FALSE;
	}

	WCHAR wszTemp[METADATA_MAX_NAME_LEN]	= {0};
	WCHAR wszBuf[METAOPIIS_BUFFER_SIZE]	= {0};
	STMETAOPERRECORD rec;
	unsigned int i = 0;
	INT nSBindingsLen;

	//	检查每个项修改非零的项数据
	if ( 0 == pstSiteInfo->dwOpFlag )
	{
		setLastErrorInfo( 0, "No Site Data need modify" );
		return FALSE;
	}

	//	检查KEYNAME
	if ( 0 == strlen(pstSiteInfo->szKeyName) )
	{
		setLastErrorInfo( 0, "Can not localize a site key" );
		return FALSE;
	}

	MultiByteToWideChar(CP_ACP, 0, pstSiteInfo->szKeyName, strlen(pstSiteInfo->szKeyName)+1,
		wszTemp, METAOPIIS_KEYNAME_LEN);

	//	检查SERVERCOMMENT
	if ( 0 != strlen(pstSiteInfo->szServerComment) )
	{
		//	修改该数据属性
		wcscpy(rec.wszKeyPath, L"/LM/W3SVC/");
		wcscat(rec.wszKeyPath, wszTemp);

		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar( CP_ACP, 0, pstSiteInfo->szServerComment, strlen(pstSiteInfo->szServerComment), wszBuf, METAOPIIS_BUFFER_SIZE );
		if ( ! modifyRecordData( & rec, MD_SERVER_COMMENT, 0, IIS_MD_UT_SERVER, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_SCOMMENT, "检查 SERVERCOMMENT 失败" );
			return FALSE;
		}
	}

	//	检查MaxConnection
	if ( 0 != pstSiteInfo->lnMaxConnection )
	{
		//	修改该数据属性
		wcscpy(rec.wszKeyPath, L"/LM/W3SVC/");
		wcscat(rec.wszKeyPath, wszTemp);

		memset( wszBuf, 0, sizeof(wszBuf) );
		memcpy(wszBuf, &pstSiteInfo->lnMaxConnection, sizeof(DWORD));
		if ( ! modifyRecordData( & rec, MD_MAX_CONNECTIONS, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( ERROR_MODIFY_MAXCONNECTION, "检查 MaxConnection 失败" );
			return FALSE;
		}
	}
	
	//	设定最大带宽限制
	LONG lnBindWidth = 0;
	if ( -1 != pstSiteInfo->lnMaxBindwidth )
	{
		//	不等于-1启用最大带宽限制
		lnBindWidth	= 1024*pstSiteInfo->lnMaxBindwidth;
	}
	else
	{
		lnBindWidth = -1;
	}

	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &lnBindWidth, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_MAX_BANDWIDTH, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_MODIFY_MAXCONNECTION, "设定最大带宽限制 失败" );
		return FALSE;
	}

	//	设置绑定信息
	memset( wszBuf, 0, sizeof(wszBuf) );
	if ( ! isValidSBindings( pstSiteInfo->szBindings ) )
	{
		setLastErrorInfo( ERROR_INVALID_SB, "" );
	}
	else
	{
		MultiByteToWideChar(CP_ACP, 0, pstSiteInfo->szBindings, strlen(pstSiteInfo->szBindings)+1, wszBuf, METAOPIIS_BUFFER_SIZE);
		nSBindingsLen = formatSBindingsToMetabase( wszBuf );
		if ( ! modifyRecordData( & rec, MD_SERVER_BINDINGS, 0, IIS_MD_UT_SERVER, MULTISZ_METADATA, wszBuf, (nSBindingsLen)*2+2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_SBINDING, "设置绑定信息 失败" );
			return FALSE;
		}
	}

	//	设置是否启用日志
	//	启用日志记录项关闭 MD_LOG_TYPE
	DWORD dwLogType	= 0;
	switch( pstSiteInfo->lnLogType )
	{
	case 0:				//	不启用日志
		dwLogType	= 0;
		break;
	case 1:				//	启用日志
		dwLogType	= 1;
		break;
	}

	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy(wszBuf, &dwLogType, sizeof(DWORD));
	if ( ! modifyRecordData( & rec, MD_LOG_TYPE, 0, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_USE_LOGS, "设置是否启用日志 失败" );
		return FALSE;		
	}

	//	获取ServerState站点状态
	DWORD dwServerState	= 4;
	memset( wszBuf, 0, sizeof(wszBuf) );
	if ( ! modifyRecordData( & rec, MD_SERVER_STATE, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, METAOPIIS_BUFFER_SIZE*sizeof(WCHAR) ) )
	{
		setLastErrorInfo( 0, "修改站点信息时，获取站点状态失败" );
	}
	else
	{
		dwServerState =  *(DWORD*)wszBuf;
	}

	wcscat(rec.wszKeyPath, L"/Root");

	//	检查AccessMDir.主目录访问属性
	if ( 0 != pstSiteInfo->dwMDirAccessFlag )
	{
		//	修改该数据属性
		memset( wszBuf, 0, sizeof(wszBuf) );
		memcpy( wszBuf, &pstSiteInfo->dwMDirAccessFlag, sizeof(DWORD) );
		if ( ! modifyRecordData( & rec, MD_ACCESS_PERM, 0, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( ERROR_MODIFY_MDIRACCESS, "" );
			return FALSE;
		}
	}
	
	//	启用默认文档 MD_DEFAULT_LOAD_FILE
	if ( 0 != strlen( pstSiteInfo->szDefaultDoc ) )
	{
		memset( wszBuf, 0, sizeof(wszBuf) );
		MultiByteToWideChar(CP_ACP, 0, pstSiteInfo->szDefaultDoc, strlen(pstSiteInfo->szDefaultDoc)+1, wszBuf, METAOPIIS_BUFFER_SIZE);
		if ( ! modifyRecordData( & rec, MD_DEFAULT_LOAD_FILE, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( ERROR_MODIFY_DEFAULTDOC, "启用默认文档 失败" );
			return FALSE;		
		}
	}


	//检查站点状态为过期和服务状态为停止则重定向到过期页面
/*
	if ( 2 == pstSiteInfo->nStatus ) {
		string strUrl(g_ConfigInfo.szExpUrl);	
		strUrl += ",EXACT_DESTINATION";

		MultiByteToWideChar(CP_ACP, 0, strUrl.c_str(), strUrl.length()+1, wszBuf, METAOPIIS_BUFFER_SIZE);
		if ( ! modifyRecordData( MD_HTTP_REDIRECT, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 ) )
		{
			CErrorOperation::s_dwErrorCode = GetLastError();
			CErrorOperation::WriteErrorLog();
			return false;
		}
	}else {		
		//如果存在重定向,重新设置主目录
		if ( DelHttpRedirect(rec) ) {
			//创建应用程序
			CreateApp(rec.szKeyPath, 2);
		}
	}
*/

	//	检查应用程序状态，如果没有
	//APPSTATUS_NOTDEFINED		No application is defined at the specified path. 
	//APPSTATUS_RUNNING			The application is running. 
	//APPSTATUS_STOPPED			The application is not running. 
	DWORD __RPC_FAR dwAppState	= 0;
	HRESULT	hRet		= NULL;
	hRet = m_pIAppAdmin->AppGetStatus( rec.wszKeyPath, &dwAppState );

	//	检查ServerState，是否卸载应用程序
	switch( pstSiteInfo->dwServerState )
	{
	case MD_SERVER_COMMAND_START:
		{
			//	开启
			if ( APPSTATUS_NOTDEFINED == dwAppState )
			{
				//	没有定义则重新开启
				hRet = mb_createApp( pstSiteInfo, rec.wszKeyPath, 2 );
			}
			else if ( APPSTATUS_STOPPED == dwAppState )
			{
				//	暂停则恢复
				if ( MD_SERVER_COMMAND_PAUSE == dwServerState )
				{
					hRet = m_pIAppAdmin->AppRecover( rec.wszKeyPath, true );
				}
			}
		}
		break;
	case MD_SERVER_COMMAND_STOP:
		{
			//	停止
			if ( APPSTATUS_NOTDEFINED != dwAppState ) {
				//卸载
				hRet = m_pIAppAdmin->AppUnLoad( rec.wszKeyPath, false );
			}
		}
		break;
	case MD_SERVER_COMMAND_PAUSE:
		{
			//	暂停
			if ( APPSTATUS_RUNNING == dwAppState )
			{
				//	卸载
				hRet = m_pIAppAdmin->AppDeleteRecoverable( rec.wszKeyPath, true );
			}
		}
		break;
	}
	

	//	"/Root"
	rec.wszKeyPath[ wcslen(rec.wszKeyPath)-5 ] = 0x0000;

	//	使用COMMAND启动站点
	DWORD dwState	= 0;
	switch( pstSiteInfo->dwServerState )
	{
	case MD_SERVER_COMMAND_START:
		{
			if ( 6 == dwServerState )
			{
				//	原来是暂停则现在就继续
				dwState = MD_SERVER_COMMAND_CONTINUE;
			}
			else
			{
				dwState = MD_SERVER_COMMAND_START;
			}
		}
		break;
	case MD_SERVER_COMMAND_STOP:
		{
			dwState = MD_SERVER_COMMAND_STOP;
		}
		break;
	case MD_SERVER_COMMAND_PAUSE:
		{
			dwState = MD_SERVER_COMMAND_PAUSE;
		}
		break;
	}

	memset( wszBuf, 0, sizeof(wszBuf) );
	memcpy( wszBuf, &dwState, sizeof(DWORD) );
	if ( ! modifyRecordData( & rec, MD_SERVER_COMMAND, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( ERROR_STATE_COMMAND, "启动站点服务失败" );
		return FALSE;
	}

	return TRUE;
}

/**
 *	@ Public
 *	从 Metabase 数据库中获取数据
 */
BOOL CMetaOpIIS::getAllDataFromMetabase( STMETAALLDATA * pstAllData )
{
	if ( NULL == pstAllData )
	{
		return FALSE;
	}

	BOOL bRet					= FALSE;
	HRESULT hRet					= NULL;
	METADATA_HANDLE hLocalMachine			= NULL;
	STMETAKEYNAME stKeyname;
	vector<STMETAKEYNAME> vcSiteKeyNames;
	vector<STMETAKEYNAME>::iterator itk;
	vector<STMETAKEYNAME> vcAppPoolKeyNames;
	vector<STMETAKEYNAME>::iterator itp;
	DWORD nIndex					= 0;
	STMETASITEINFO stSiteInfo;
	STMETAAPPPOOLINFO stAppPoolInfo;

	//	Enum Site Keyname
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, L"/LM", METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hLocalMachine );
	if ( SUCCEEDED(hRet) )
	{
		bRet	= TRUE;

		//	枚举站点 id
		nIndex	= 0;
		vcSiteKeyNames.clear();
		do
		{
			memset( & stKeyname, 0, sizeof(stKeyname) );
			hRet = m_pIMeta->EnumKeys( hLocalMachine, L"/W3SVC", stKeyname.wszKeyName, nIndex );
			if ( SUCCEEDED( hRet ) )
			{
				if ( wcslen( stKeyname.wszKeyName ) && (DWORD)_wtol( stKeyname.wszKeyName ) )
				{
					vcSiteKeyNames.push_back( stKeyname );
				}
			}
			nIndex++;
		}
		while ( SUCCEEDED(hRet) );

		//	枚举 AppPool 名称
		nIndex	= 0;
		vcAppPoolKeyNames.clear();
		do
		{
			memset( & stKeyname, 0, sizeof(stKeyname) );
			hRet = m_pIMeta->EnumKeys( hLocalMachine, L"/W3SVC/AppPools", stKeyname.wszKeyName, nIndex );
			if ( SUCCEEDED( hRet ) )
			{
				if ( wcslen( stKeyname.wszKeyName ) )
				{
					vcAppPoolKeyNames.push_back( stKeyname );
				}
			}
			nIndex++;
		}
		while ( SUCCEEDED(hRet) );

		m_pIMeta->CloseKey( hLocalMachine );
	}



	//
	//	循环所有站点，读取信息
	//
	for ( itk = vcSiteKeyNames.begin(); itk != vcSiteKeyNames.end(); itk ++ )
	{
		//	打开各站点获取所要的数据
		memset( & stSiteInfo, 0, sizeof(stSiteInfo) );
		if ( getSiteDataFromKey( (*itk).wszKeyName, & stSiteInfo ) )
		{
			//	保存站点信息
			pstAllData->vcSite.push_back( stSiteInfo );
		}
	}

	//
	//	循环所有 AppPool，读取信息
	//
	for ( itp = vcAppPoolKeyNames.begin(); itp != vcAppPoolKeyNames.end(); itp ++ )
	{
		//	打开各 AppPool 获取信息
		memset( & stAppPoolInfo, 0, sizeof(stAppPoolInfo) );
		if ( getAppPoolDataFromKey( (*itp).wszKeyName, & stAppPoolInfo ) )
		{
			//	保存 AppPool 信息
			pstAllData->vcAppPool.push_back( stAppPoolInfo );
		}
	}

	return bRet;
}

/**
 *	@ Public
 *	获取某个 AppPool 信息
 */
BOOL CMetaOpIIS::getAppPoolDataFromKey( LPCWSTR lpcwszKeyName, STMETAAPPPOOLINFO * pstAppPoolInfo )
{
	if ( NULL == lpcwszKeyName || 0 == wcslen(lpcwszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == pstAppPoolInfo )
	{
		return FALSE;
	}

	HRESULT hRet					= NULL;
	METADATA_HANDLE hMDAppPool			= NULL;
	METADATA_RECORD MyRecord;
	DWORD dwBufLen					= METAOPIIS_BUFFER_SIZE*sizeof(WCHAR);
	DWORD dwReqBufLen				= 0;
	WCHAR wszKeyPath[ METADATA_MAX_NAME_LEN ]	= {0};
	WCHAR wszBuffer[ METAOPIIS_BUFFER_SIZE ]	= {0};
	LPWSTR lpwszTemp				= NULL;

	//	写入 KEYNAME
	WideCharToMultiByte( CP_ACP, 0, lpcwszKeyName, wcslen(lpcwszKeyName), pstAppPoolInfo->szKeyName, sizeof(pstAppPoolInfo->szKeyName), NULL, NULL );

	//	健路径
	_snwprintf( wszKeyPath, sizeof(wszKeyPath)-sizeof(WCHAR), L"/LM/W3SVC/AppPools/%s", lpcwszKeyName );

	try
	{
		hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, wszKeyPath, METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hMDAppPool );
		if ( ! SUCCEEDED( hRet ) )
		{
			throw L"打开KEYNAME键失败";
		}

		//	2001	取得数据 dwAppPoolState
		//		2启动，4停止
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_STATE, METADATA_VOLATILE, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( ! SUCCEEDED( hRet ) && MD_ERROR_DATA_NOT_FOUND != hRet )
		{
			throw "获取 dwAppPoolState 状态失败";
		}
		else
		{
			pstAppPoolInfo->dwAppPoolState = *((DWORD*)wszBuffer);
		}

		//	2002	回收 >> 回收工作进程(分钟)
		//		dwPeriodicRestartTime
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTTIME, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPeriodicRestartTime = *((DWORD*)wszBuffer);
		}
	
		//	2003	回收 >> 回收工作进程(请求数目)
		//		dwPeriodicRestartRequests
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTTIME, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPeriodicRestartRequests = *((DWORD*)wszBuffer);
		}

		//	2004 回收 >> 在下列时间回收工作进程
		//		szPeriodicRestartSchedule
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTSCHEDULE, METADATA_INHERIT, IIS_MD_UT_SERVER, MULTISZ_METADATA, wszBuffer, sizeof(wszBuffer), & dwReqBufLen );
		if ( SUCCEEDED(hRet) )
		{
			//	...
			//	...
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstAppPoolInfo->szPeriodicRestartSchedule, sizeof(pstAppPoolInfo->szPeriodicRestartSchedule), NULL, NULL );
		}

		//	2005	回收 >> 内存回收 >> 最大虚拟内存(兆)
		//		dwPeriodicRestartMemory
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTMEMORY, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPeriodicRestartMemory = *((DWORD*)wszBuffer);
		}

		//	2006	回收 >> 内存回收 >> 最大使用的内存(兆)
		//		dwPeriodicRestartPrivateMemory
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PERIODICRESTARTPRIVATEMEMORY, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPeriodicRestartPrivateMemory = *((DWORD*)wszBuffer);
		}

		//	2007	性能 >> 空闲超时 >> 在空闲此段工作实践后关闭工作进程(分钟)
		//		dwIdleTimeout
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_IDLETIMEOUT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwIdleTimeout = *((DWORD*)wszBuffer);
		}

		//	2008	性能 >> 请求队列限制 >> 核心请求队列限制为(请求次数)
		//		dwAppPoolQueueLength
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_APPPOOLQUEUELENGTH, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwAppPoolQueueLength = *((DWORD*)wszBuffer);
		}

		//	2009	性能 >> 启用 CPU 监视 >> 最大 CPU 使用率(百分比)
		//		dwCPULimit
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_CPULIMIT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwCPULimit = *((DWORD*)wszBuffer);
		}

		//	2010	性能 >> 启用 CPU 监视 >> 刷新 CPU 使用率(分钟)
		//		dwCPUResetInterval
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_CPURESETINTERVAL, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwCPUResetInterval = *((DWORD*)wszBuffer);
		}

		//	2011	性能 >> 启用 CPU 监视 >> CPU 使用率超过最大使用率时执行的操作
		//		dwCPUAction
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_CPUACTION, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwCPUAction = *((DWORD*)wszBuffer);
		}

		//	2012	性能 >> Web园 >> 最大工作进程数
		//		dwMaxProcesses
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_MAXPROCESSES, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwMaxProcesses = *((DWORD*)wszBuffer);
		}

		//	2013	运行状况 >> 启用 Ping
		//		dwPingingEnabled
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PINGINGENABLED, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPingingEnabled = *((DWORD*)wszBuffer);
		}

		//	2014	运行状况 >> 启用 Ping >> 每隔下列时间 Ping 工作线程(秒)
		//		dwPingInterval
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_PINGINTERVAL, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwPingInterval = *((DWORD*)wszBuffer);
		}

		//	2015	运行状况 >> 启用快速失败保护
		//		dwRapidFailProtection
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_RAPIDFAILPROTECTION, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwRapidFailProtection = *((DWORD*)wszBuffer);
		}

		//	2016	运行状况 >> 启用快速失败保护 >> 失败数
		//		dwRapidFailProtectionMaxCrashes
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_RAPIDFAILPROTECTIONMAXCRASHES, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwRapidFailProtectionMaxCrashes = *((DWORD*)wszBuffer);
		}

		//	2017	运行状况 >> 启用快速失败保护 >> 时间段(分钟)
		//		dwRapidFailProtectionInterval
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_RAPIDFAILPROTECTIONINTERVAL, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwRapidFailProtectionInterval = *((DWORD*)wszBuffer);
		}

		//	2018	运行状况 >> 启动时间限制 >> 工作进程必须在下列时间内开始(秒)
		//		dwStartupTimeLimit
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_STARTUPTIMELIMIT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwStartupTimeLimit = *((DWORD*)wszBuffer);
		}

		//	2019	运行状况 >> 关闭时间限制 >> 工作进程必须在下列时间内关闭(秒)
		//		dwShutdownTimeLimit
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDAppPool, & MyRecord, MD_APPPOOL_SHUTDOWNTIMELIMIT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED(hRet)  )
		{
			pstAppPoolInfo->dwShutdownTimeLimit = *((DWORD*)wszBuffer);
		}

		//
		//	结束了
		//
		if ( hMDAppPool )
		{
			m_pIMeta->CloseKey( hMDAppPool );
		}
	}
	catch ( LPTSTR lpszError )
	{
		setLastErrorInfo( 0, lpszError );
	}

	return TRUE;
}

BOOL CMetaOpIIS::getSiteDataFromKey( LPCWSTR lpcwszKeyName, STMETASITEINFO * pstSiteInfo )
{
	HRESULT hRet					= NULL;
	METADATA_HANDLE hMDSite				= NULL;
	METADATA_HANDLE	hMDSiteRoot			= NULL;
	METADATA_RECORD MyRecord;
	DWORD dwBufLen					= METAOPIIS_BUFFER_SIZE*sizeof(WCHAR);
	DWORD dwReqBufLen				= 0;
	WCHAR wszKeyPath[ METADATA_MAX_NAME_LEN ]	= {0};
	WCHAR wszBuffer[ METAOPIIS_BUFFER_SIZE ]	= {0};
	LPWSTR lpwszTemp				= NULL;


	//	写入KEYNAME
	WideCharToMultiByte( CP_ACP, 0, lpcwszKeyName, wcslen(lpcwszKeyName), pstSiteInfo->szKeyName, sizeof(pstSiteInfo->szKeyName), NULL, NULL );

	//	在键名前加上"/"
	_snwprintf( wszKeyPath, sizeof(wszKeyPath)-sizeof(WCHAR), L"/LM/W3SVC/%s", lpcwszKeyName );

	try
	{
		hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, wszKeyPath, METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hMDSite );
		if ( ! SUCCEEDED( hRet ) )
		{
			throw L"打开KEYNAME键失败";
		}

		//	1001	取得数据 MD_SERVER_COMMENT
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, MD_SERVER_COMMENT, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuffer, sizeof(wszBuffer), & dwReqBufLen );
		if ( ! SUCCEEDED(hRet) && MD_ERROR_DATA_NOT_FOUND != hRet )
		{
			throw "获取站点名称失败";
		}
		else
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szServerComment, sizeof(pstSiteInfo->szServerComment), NULL, NULL );
		}

		//	1002	取得数据 AppPoolId
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, 9101, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuffer, sizeof(wszBuffer), & dwReqBufLen );
		if ( ! SUCCEEDED(hRet) && MD_ERROR_DATA_NOT_FOUND != hRet )
		{
			throw "获取站点 AppPoolId 失败";
		}
		else
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szAppPoolId, sizeof(pstSiteInfo->szAppPoolId), NULL, NULL );
		}

		//	1003	取得数据 MD_SERVER_STATE
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, MD_SERVER_STATE, METADATA_VOLATILE, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( ! SUCCEEDED( hRet ) && MD_ERROR_DATA_NOT_FOUND != hRet )
		{
			throw "获取站点状态失败";
		}
		else
		{
			pstSiteInfo->dwServerState = *((DWORD*)wszBuffer);
		}

		//	1004	获取“保持 Http 连接”
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwAllowKeepAlive	= 1;
		hRet = getRecordData( hMDSite, & MyRecord, MD_ALLOW_KEEPALIVES, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->dwAllowKeepAlive = *((DWORD*)wszBuffer);
		}

		//	1005	获取“连接超时”
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwConnectionTimeout	= 120;
		hRet = getRecordData( hMDSite, & MyRecord, MD_CONNECTION_TIMEOUT, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->dwConnectionTimeout = *((DWORD*)wszBuffer);
		}

		//	1006	取得数据 MD_SERVER_BINDINGS
		lpwszTemp = new WCHAR[ METAOPIIS_BUFFER_SIZE ];
		memset( lpwszTemp, 0, METAOPIIS_BUFFER_SIZE*sizeof(WCHAR) );
		MyRecord.dwMDIdentifier = MD_SERVER_BINDINGS;
		MyRecord.dwMDAttributes = 0;
		MyRecord.dwMDUserType   = IIS_MD_UT_SERVER;
		MyRecord.dwMDDataType   = MULTISZ_METADATA;
		MyRecord.dwMDDataLen    = METAOPIIS_BUFFER_SIZE*sizeof(WCHAR);    
		MyRecord.pbMDData       = (unsigned char *)lpwszTemp;
		MyRecord.dwMDDataTag    = 0; 

		hRet = m_pIMeta->GetData( hMDSite, NULL, &MyRecord, &dwReqBufLen );
		if ( RETURNCODETOHRESULT(ERROR_INSUFFICIENT_BUFFER) == hRet )
		{
			if ( lpwszTemp )
			{
				delete [] lpwszTemp;
				lpwszTemp = NULL;
			}
			lpwszTemp = new WCHAR[ dwReqBufLen ];
			memset( lpwszTemp, 0, dwReqBufLen*sizeof(WCHAR) );
			MyRecord.dwMDDataLen	= dwReqBufLen;
			MyRecord.pbMDData	= (unsigned char *)lpwszTemp;
			hRet = m_pIMeta->GetData( hMDSite, NULL, &MyRecord, &dwReqBufLen );
			if ( ! SUCCEEDED( hRet ) )
			{
				throw "获取站点绑定数据失败";
			}
			dwBufLen = dwReqBufLen;
		}
		dwReqBufLen = dwBufLen;
		if ( wcslen(lpwszTemp) )
		{
			WideCharToMultiByte( CP_ACP, 0, lpwszTemp, dwReqBufLen, pstSiteInfo->szBindings, sizeof(pstSiteInfo->szBindings), NULL, NULL );

			//	对绑定信息进行处理，将每个绑定头之间的“NULL”改为“|”，将最后一个也改为|
			formatSBindingsToString( pstSiteInfo->szBindings, sizeof(pstSiteInfo->szBindings) );
		}
		else
		{
			strcpy( pstSiteInfo->szBindings, ":80:|" );
		}

		if ( lpwszTemp )
		{
			delete [] lpwszTemp;
			lpwszTemp = NULL;
		}


		//	1007	取得数据 MD_MAX_CONNECTION
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnMaxConnection	= -1;	//	无限制连接
		hRet = getRecordData( hMDSite, & MyRecord, MD_MAX_CONNECTIONS, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnMaxConnection = *((LONG*)wszBuffer);
		}
		

		//	1008	获取最大带宽限制
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnMaxBindwidth	= -1;	//	无限制连接
		hRet = getRecordData( hMDSite, & MyRecord, MD_MAX_BANDWIDTH, 0, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnMaxBindwidth = *((LONG*)wszBuffer);
		}

		//
		//	获取日志相关属性
		//

		//	1009	获取是否启用日志 MD_LOG_TYPE
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogType = 0;	//	网站 >> 0-不启用 1-启用并使用W3C格式
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOG_TYPE, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogType = *((LONG*)wszBuffer);
		}

		//	1010	获取是否启用日志 lnLogFileLocaltimeRollover
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogFileLocaltimeRollover = 0;
		hRet = getRecordData( hMDSite, & MyRecord, 4015, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogFileLocaltimeRollover = *((LONG*)wszBuffer);
		}

		//	1011	获取 日志记录：lnLogFilePeriod
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogFilePeriod = 1;	//	网站 >> 日志 >> 1每天，4每小时，2每周，3每月，0不限制文件大小/当文件大小达到
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOGFILE_PERIOD, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogFilePeriod = *((LONG*)wszBuffer);
		}

		//	1012	获取 日志记录：lnLogFileTruncateSize
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogFileTruncateSize = 20971520;	//	网站 >> 日志纪录：文件大小
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOGFILE_TRUNCATE_SIZE, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogFileTruncateSize = *((LONG*)wszBuffer);
		}

		//	1013	获取 日志记录：lnLogExtFileFlags
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnLogExtFileFlags = 0;		//	网站 >> 日志纪录：高级选项
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOGEXT_FIELD_MASK, METADATA_INHERIT, IIS_MD_UT_SERVER, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnLogExtFileFlags = *((LONG*)wszBuffer);
		}

		//	1014	获取 日志记录：目录 szLogFileDirectory
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOGFILE_DIRECTORY, METADATA_INHERIT, IIS_MD_UT_SERVER, EXPANDSZ_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szLogFileDirectory, sizeof(pstSiteInfo->szLogFileDirectory), NULL, NULL );
		}
		else
		{
			throw "获取日志记录目录失败";
		}

		//	1015	获取 日志纪录：PluginClsid
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSite, & MyRecord, MD_LOG_PLUGIN_ORDER, METADATA_INHERIT, IIS_MD_UT_SERVER, STRING_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szLogPluginClsid, sizeof(pstSiteInfo->szLogPluginClsid), NULL, NULL );
		}

		//	1016	启用默认文档 MD_DEFAULT_LOAD_FILE
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		dwReqBufLen = 0;
		hRet = getRecordData( hMDSite, & MyRecord, MD_DEFAULT_LOAD_FILE, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szDefaultDoc, sizeof(pstSiteInfo->szDefaultDoc), NULL, NULL);
		}
		else
		{
			if ( MD_ERROR_DATA_NOT_FOUND == hRet )
			{
				_sntprintf( pstSiteInfo->szDefaultDoc, sizeof(pstSiteInfo->szDefaultDoc)-sizeof(TCHAR), "%s", METAOPIIS_DEFAULTDOC );
			}
			else
			{
				throw "获取默认文档失败";
			}
		}
		if ( 0 == strlen( pstSiteInfo->szDefaultDoc ) )
		{
			_sntprintf( pstSiteInfo->szDefaultDoc, sizeof(pstSiteInfo->szDefaultDoc)-sizeof(TCHAR), "%s", METAOPIIS_DEFAULTDOC );
		}

		//
		//	先打开/Root目录
		//	读取主目录属性
		//

		hRet = m_pIMeta->OpenKey( hMDSite, L"/Root", METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hMDSiteRoot );
		if ( ! SUCCEEDED( hRet ) )
		{
			throw "打开KEYNAME/Root键失败";
		}

		//	1017	读取Path属性
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_VR_PATH, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuffer, sizeof(wszBuffer), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szServerMDir, sizeof(pstSiteInfo->szServerMDir), NULL, NULL );
		}
		else
		{
			throw "获取主目录路径失败";
		}

		//	1018	获取主目录访问属性
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwMDirAccessFlag = 513;
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_ACCESS_PERM, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->dwMDirAccessFlag = *((DWORD*)wszBuffer);
		}

		//	1019	获取 主目录：目录浏览属性
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_DIRECTORY_BROWSING, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnDirBrowseFlags = *((LONG*)wszBuffer);
		}

		//	1020	获取 主目录：记录访问
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwDontLog = 1;
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_DONT_LOG, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			//	1 为不纪录，0为纪录
			pstSiteInfo->dwDontLog = *((LONG*)wszBuffer);
		}

		//	1021	获取 主目录：索引资源
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->dwContentIndexed = 0;
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_IS_CONTENT_INDEXED, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), & dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->dwContentIndexed = *((LONG*)wszBuffer);
		}


		//	1022	获取 http 头 >> 内容过期
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_HTTP_EXPIRES, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szHttpExpires, sizeof(pstSiteInfo->szHttpExpires), NULL, NULL);
		}

		//	获取 自定义错误


		//	1023	获取 文档 >> 启用文档页脚
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		pstSiteInfo->lnEnableDocFooter = 0;
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_FOOTER_ENABLED, METADATA_INHERIT, IIS_MD_UT_FILE, DWORD_METADATA, wszBuffer, sizeof(DWORD), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			pstSiteInfo->lnEnableDocFooter = *((LONG*)wszBuffer);
		}

		//	1024	获取 文档 >> 文档页脚文件路径
		memset( wszBuffer, 0, sizeof(wszBuffer) );
		hRet = getRecordData( hMDSiteRoot, & MyRecord, MD_FOOTER_DOCUMENT, METADATA_INHERIT, IIS_MD_UT_FILE, STRING_METADATA, wszBuffer, sizeof(wszBuffer), &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			WideCharToMultiByte( CP_ACP, 0, wszBuffer, wcslen(wszBuffer), pstSiteInfo->szDefaultDocFooter, sizeof(pstSiteInfo->szDefaultDocFooter), NULL, NULL);
		}


		//
		//	结束了
		//
		if ( hMDSiteRoot )
		{
			m_pIMeta->CloseKey( hMDSiteRoot );
		}
		if ( hMDSite )
		{
			m_pIMeta->CloseKey( hMDSite );
		}
	}
	catch ( LPTSTR lpszError )
	{
		setLastErrorInfo( 0, lpszError );
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////







/**
 *	@ Private
 *	获取一个不存在的 KeyName
 */
DWORD CMetaOpIIS::getNewSiteKey()
{
	DWORD dwRet		= 0;
	SYSTEMTIME st;
	HRESULT	hRet;
	METADATA_HANDLE hLocalMachine			= NULL;
	WCHAR wszKeyName[ METADATA_MAX_NAME_LEN ]	= {0};
	TCHAR szTemp[ MAX_PATH ]			= {0};
	DWORD dwIndex;
	BOOL bExist;
	vector<DWORD> vcKeyNum;
	vector<DWORD>::iterator it;

	//	取得本机数据的句柄
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, L"/LM", METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hLocalMachine );
	if ( SUCCEEDED(hRet) && hLocalMachine )
	{
		dwIndex = 0;
		while ( SUCCEEDED(hRet) )
		{
			//	枚举WWW服务站点
			hRet = m_pIMeta->EnumKeys( hLocalMachine, L"/W3SVC", wszKeyName, dwIndex );
			if ( SUCCEEDED( hRet ) )
			{
				//	过滤掉Info和Filter键
				if ( 0 != wcscmp( wszKeyName, L"" ) &&
					0 != wcscmp( wszKeyName, L"Info" ) && 
					0 != wcscmp( wszKeyName, L"Filters" ) && 
					0 != wcscmp( wszKeyName, L"AppPools" ) )
				{
					vcKeyNum.push_back( (DWORD)_wtol( wszKeyName ) );
				}
			}
			dwIndex ++;
		}
		m_pIMeta->CloseKey( hLocalMachine );
	}
	else
	{
		setLastErrorInfo( 0, "获取本机主键句柄失败" );
	}

	//	循环目前所有的 KeyName
	while( vcKeyNum.size() )
	{
		bExist = FALSE;

		//	创建一个新的 keyvalue
		GetLocalTime( & st );
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		dwRet = m_cCrc32.GetCrc32( (unsigned char *)szTemp, strlen(szTemp) );
		if ( dwRet > 999999999 )
		{
			dwRet = (DWORD)( dwRet / 100 );
			if ( dwRet > 999999999 )
			{
				dwRet = (DWORD)( dwRet / 100 );
			}
		}

		for ( it = vcKeyNum.begin(); it != vcKeyNum.end(); it ++ )
		{
			if ( dwRet == (*it) )
			{
				bExist = TRUE;
				break;
			}
		}

		if ( ! bExist )
		{
			break;
		}
	}

	return dwRet;
}


/**
 *	@ Priavte
 *	获取一个站点下的某个纪录的值
 */
HRESULT CMetaOpIIS::getRecordData( METADATA_HANDLE hKeyName, METADATA_RECORD * pstMRec, 
				DWORD dwMDIdentifier, DWORD dwMDAttributes, DWORD dwMDUserType, DWORD dwMDDataType, LPCWSTR lpcwszBuf, DWORD dwBufLen,
				DWORD * pdwMDRequiredDataLen )
{
	if ( NULL == hKeyName || NULL == pstMRec )
	{
		return -1;
	}

//	BOOL bRet	= FALSE;
//	HRESULT hRet	= NULL;

	pstMRec->dwMDIdentifier	= dwMDIdentifier;
	pstMRec->dwMDAttributes	= dwMDAttributes;
	pstMRec->dwMDUserType	= dwMDUserType;
	pstMRec->dwMDDataType	= dwMDDataType;
	pstMRec->dwMDDataLen	= dwBufLen;    
	pstMRec->pbMDData	= (unsigned char *)(lpcwszBuf);
	pstMRec->dwMDDataTag	= 0;

	return m_pIMeta->GetData( hKeyName, NULL, pstMRec, pdwMDRequiredDataLen );
}

/**
 *	@ Priavte
 *	修改一个站点下的某个纪录的值
 */
BOOL CMetaOpIIS::modifyRecordData( STMETAOPERRECORD * pstRec, DWORD dwMDIdentifier, DWORD dwMDAttributes, DWORD dwMDUserType, DWORD dwMDDataType, LPCWSTR lpcwszBuf, DWORD dwBufLen )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}

//	STMETAOPERRECORD rec;
//	memset( & rec, 0, sizeof(rec) );

	pstRec->metaRecord.dwMDIdentifier	= dwMDIdentifier;
	pstRec->metaRecord.dwMDAttributes	= dwMDAttributes;
	pstRec->metaRecord.dwMDUserType		= dwMDUserType;
	pstRec->metaRecord.dwMDDataType		= dwMDDataType;
	pstRec->metaRecord.dwMDDataLen		= dwBufLen;    
	pstRec->metaRecord.pbMDData		= (unsigned char *)(lpcwszBuf);
	pstRec->metaRecord.dwMDDataTag		= 0;

	return ModifyData( pstRec );
}

BOOL CMetaOpIIS::ModifyData( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "修改站点数据时，指向 MSAdminBase，IWamAdmin2 接口的灵敏指针未初始化" );
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	HRESULT hRet	= NULL;
	METADATA_HANDLE hLocalMachine = NULL;
	METADATA_HANDLE hCurrent      = NULL;
	
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( SUCCEEDED(hRet) && hCurrent )
	{
		//	设置一个键属性
		hRet = m_pIMeta->SetData( hCurrent, NULL, & pstRec->metaRecord );
		if ( SUCCEEDED(hRet) )
		{
			bRet = TRUE;
		}
		else
		{
			setLastErrorInfo( 0, "设置一个键属性失败" );
		}
		
		m_pIMeta->CloseKey( hCurrent );
	}
	else
	{
		setLastErrorInfo( 0, "" );
	}

	return bRet;
}




/**
 *	@ Private
 *	CreateApp
 */
BOOL CMetaOpIIS::mb_createApp( STMETASITEINFO * pstSiteInfo, LPWSTR lpszMDPath, DWORD dwAppMode )
{
	if ( NULL == pstSiteInfo )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "CreateApp，发现指向 MSAdminBase，IWamAdmin2 接口的灵敏指针未初始化" );
		return FALSE;
	}

	HRESULT	hRet	= NULL;
	hRet = m_pIAppAdmin->AppCreate2( lpszMDPath, dwAppMode );
	if ( FAILED(hRet) )
	{
		setLastErrorInfo( 0, "%s 站点建立应用程序失败", pstSiteInfo->szServerComment );
		return FALSE;
	}

	return TRUE;
}


/**
 *	@ Private
 *	创建一个键
 */
BOOL CMetaOpIIS::mb_createKey( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "创建键时，发现指向 MSAdminBase，IWamAdmin2 接口的灵敏指针未初始化" );
		return FALSE;
	}

	BOOL bRet	= FALSE;
	HRESULT hRet	= NULL;
	METADATA_HANDLE hCurrent      = NULL;
	
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( SUCCEEDED(hRet) && hCurrent )
	{
		//	添加一个子键
		hRet = m_pIMeta->AddKey( hCurrent, pstRec->wszKeyName );
		if ( SUCCEEDED( hRet ) )
		{
			bRet = TRUE;
		}
		else
		{
			setLastErrorInfo( 0, "" );
		}

		m_pIMeta->CloseKey( hCurrent );
	}
	else
	{
		setLastErrorInfo( 0, "" );
	}

	return bRet;
}

/**
 *	@ Private
 *	删除一个键
 */
BOOL CMetaOpIIS::mb_deleteKey( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "删除键时发现，指向 MSAdminBase，IWamAdmin2 接口的灵敏指针未初始化" );
		return FALSE;
	}

	HRESULT hRet = NULL;
	METADATA_HANDLE hLocalMachine = NULL;
	METADATA_HANDLE hCurrent      = NULL;
	
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( !SUCCEEDED(hRet) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}
	
	//删除一个子键
	hRet = m_pIMeta->DeleteKey( hCurrent, pstRec->wszKeyName );
	if ( !SUCCEEDED(hRet) )
	{
		m_pIMeta->CloseKey(hCurrent);

		setLastErrorInfo( 0, "删除一个子键失败" );
		return FALSE;
	}
	
	m_pIMeta->CloseKey(hCurrent);
	return TRUE;
}

/**
 *	@ Private
 *	重新命名一个键
 */
BOOL CMetaOpIIS::mb_renameKey( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}
	if ( ! isInitComReady() )
	{
		setLastErrorInfo( 0, "重命名一个键时，发现指向 MSAdminBase，IWamAdmin2 接口的灵敏指针未初始化" );
		return FALSE;
	}

	HRESULT hRet = NULL;
	METADATA_HANDLE hLocalMachine = NULL;
	METADATA_HANDLE hCurrent      = NULL;
	
	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( ! SUCCEEDED(hRet) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}

	//	重命名一个键
	hRet = m_pIMeta->RenameKey( hCurrent, NULL, pstRec->wszKeyName );
	if ( ! SUCCEEDED(hRet) )
	{
		m_pIMeta->CloseKey(hCurrent);

		setLastErrorInfo( 0, "重命名一个键失败" );
		return FALSE;
	}

	m_pIMeta->CloseKey(hCurrent);
	return TRUE;
}

/**
 *	@ Private
 *	获取一个键的值
 */
BOOL CMetaOpIIS::mb_getKeyData( STMETAOPERRECORD * pstRec )
{
	if ( NULL == pstRec )
	{
		return FALSE;
	}

	BOOL bRet			= FALSE;
	HRESULT hRet			= NULL;
	METADATA_HANDLE hLocalMachine	= NULL;
	METADATA_HANDLE hCurrent	= NULL;
	DWORD dwReqBufLen		= 0;

	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, pstRec->wszKeyPath, METADATA_PERMISSION_READ, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( SUCCEEDED( hRet ) )
	{
		//	获取一个键属性
		hRet = m_pIMeta->GetData( hCurrent, NULL, &pstRec->metaRecord, &dwReqBufLen );
		if ( SUCCEEDED( hRet ) )
		{
			bRet = TRUE;
		}
		else
		{
			setLastErrorInfo( 0, "获取一个键属性失败" );
		}
		
		m_pIMeta->CloseKey( hCurrent );
	}
	else
	{
		setLastErrorInfo( 0, "" );
	}

	return bRet;
}


BOOL CMetaOpIIS::DelHttpRedirect(STMETAOPERRECORD &rec)
{
	HRESULT hRet = NULL;
	METADATA_HANDLE hLocalMachine	= NULL;
	METADATA_HANDLE hCurrent	= NULL;
	DWORD dwReqBufLen		= 0;
	BOOL  bRet			= TRUE;

	hRet = m_pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE, rec.wszKeyPath, METADATA_PERMISSION_WRITE, METAOPIIS_MD_TIMEOUT, &hCurrent );
	if ( ! SUCCEEDED(hRet) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}
	
	hRet = m_pIMeta->DeleteData(hCurrent, L"", MD_HTTP_REDIRECT, STRING_METADATA);
	if ( !SUCCEEDED(hRet) )
	{
		if ( MD_ERROR_DATA_NOT_FOUND == hRet )
		{
			//没有重定向
			bRet = FALSE;
		}
		else
		{
			setLastErrorInfo( 0, "删除重定向失败" );
			bRet = FALSE;
		}
	}
	else
	{
		bRet = TRUE;
	}

	m_pIMeta->CloseKey(hCurrent);
	return bRet;
}






/**
 *	@ Private
 *	检查 Bindings 信息是否有效
 */
BOOL CMetaOpIIS::isValidSBindings( LPCTSTR lpcszBindings )
{
	if ( NULL == lpcszBindings )
	{
		return FALSE;
	}

	string strSBindings( lpcszBindings );

	//	获取绑定头的个数
	string::size_type idx1 = 0;
	string::size_type idx2 = 0;
	string strSub;

	do 
	{
		idx2 = strSBindings.find( '|', idx1 );			
		strSub = strSBindings.substr( idx1, idx2 - idx1 );
		if ( ! isValidSingleSBindings( strSub ) )
		{
			return FALSE;
		}
		if ( idx2 == strSBindings.length()-1 )
		{
			break;
		}
		
		idx1 = idx2+1;
	} while( idx2 != string::npos );
	
	return TRUE;
}
BOOL CMetaOpIIS::isValidSingleSBindings( string strSBindings )
{
	//	检查“:”的个数
	int iTimes = count( strSBindings.begin(), strSBindings.end(), ':' );
	if ( 2 != iTimes )
	{
		return FALSE;
	}

	string::size_type idx1 = 0;
	string::size_type idx2 = 0;

	//	检查IP是否有效
	string strIP;
	idx2 = strSBindings.find_first_of(':', (string::size_type)0);
	if ( idx2 != 0 )
	{
		strIP = strSBindings.substr(0, idx2);
		//	检查'.'的数目是否为3个
		iTimes = count(strIP.begin(), strIP.end(), '.');
		if ( 3 != iTimes )
		{
			return FALSE;
		}
		if ( INADDR_NONE == inet_addr( strIP.c_str() ) )
		{
			return FALSE;
		}
	}

	idx1 = idx2+1;
	//	检查PORT是否有效
	string strPort;
	idx2 = strSBindings.find_first_of(':', idx1);
	strPort = strSBindings.substr(idx1, idx2-idx1);
	if ( strPort.empty() )
	{
		return FALSE;
	}
	//	检查是否有除数字之外的其他字符
	if ( string::npos != strPort.find_first_not_of( "0123456789" ) )
	{
		return FALSE;
	}

	unsigned short iPort = (unsigned short)atoi(strPort.c_str());
	if ( iPort < 0 || iPort > 65535 )
	{
		return FALSE;
	}
	
	//	检查主机头是否有效, 英文，数字，'.', '-'
	return TRUE;
}



/**
 *	@ Private
 *	将数据库记录的格式(以 | 分割)转化为 Metabase 格式(以 NULL 分割)，同时返回其长度
 */
INT CMetaOpIIS::formatSBindingsToMetabase( LPWSTR pwszSBindings )
{
	//	数据库中的格式是：IP:PORT:HEADER|IP:PORT:HEADER|....|IP:PORT:HEADER|
	//	直接将 "|" 改成 NULL
	INT i;
	INT nLen = wcslen( pwszSBindings );
	for ( i = 0; i < nLen; i ++ )
	{
		if ( '|' == pwszSBindings[ i ] )
		{
			pwszSBindings[ i ] = 0x0;
		}
	}
	return nLen;
}

/**
 *	@ Private
 *	将 Metabase 格式(以 NULL 分割)转化为数据库记录的格式(以 | 分割)
 */
void CMetaOpIIS::formatSBindingsToString( LPSTR lpszBuffer, DWORD dwSize )
{
	INT i;

	//	将某个绑定条中间的NULL改为"|",再再最后加上一个"|"
	for( i = 0; i < dwSize; i++ )
	{
		if ( 0 == lpszBuffer[i] )
		{
			lpszBuffer[ i ] = '|';
			if ( 0 == lpszBuffer[ i + 1 ] )
			{
				break;
			}
		}
	}

	string sbindings(lpszBuffer);
	string::size_type idx = 0;		// start index
	string::size_type idxnew = 0;	// new index
	string::size_type idxsub = 0;	// sub string index
	
	while ( idx != string::npos )
	{
		//	以“|”截取字符串
		//string::size_type idxnew = sbindings.find( '|', idx );
		//	找到“:”的位置
		string::size_type idxsub = sbindings.find( ':', idx );
		
		//	将 IP 置换为空
		sbindings.replace( idx, (idxsub-idx), "|" );
		if ( '|' == sbindings[0] )
		{
			sbindings = sbindings.replace( 0, 1, "" );
		}

		//	将搜索位置向字符串后移动
		idx = sbindings.find( '|', idx+1 );
	}
	sbindings[ sbindings.length() ] = NULL;
	strcpy( lpszBuffer, sbindings.c_str() );
}

/*
bool CMetaOpIIS::GetBindingData(STMETAOPERRECORD &rec, WCHAR *pwszBuf, DWORD &dwBufLen)
{
	HRESULT hRet = NULL;
	METADATA_HANDLE hCurrent     = NULL;
	DWORD dwReqBufLen			 = 0;
	MultiByteToWideChar(CP_ACP, 0, m_SBinding.szSiteKeyName, strlen(m_SBinding.szSiteKeyName)+1,
		rec.szKeyName, METAOPIIS_KEYNAME_LEN);

	wcscpy(rec.szKeyPath, L"/LM/W3SVC/");
	wcscat(rec.szKeyPath, rec.szKeyName);

	hRet = m_pIMeta->OpenKey(METADATA_MASTER_ROOT_HANDLE, 
		rec.szKeyPath, METADATA_PERMISSION_READ,
		METAOPIIS_MD_TIMEOUT, &hCurrent);
	if ( !SUCCEEDED(hRet) )
	{
		CErrorOperation::s_dwErrorCode = GetLastError();
		CErrorOperation::WriteErrorLog();
		return false;
	}
	
	//获取该站点的ServerBindings数据
	rec.metaRecord.dwMDIdentifier	= MD_SERVER_BINDINGS;
	rec.metaRecord.dwMDAttributes	= 0;
	rec.metaRecord.dwMDUserType		= IIS_MD_UT_SERVER;
	rec.metaRecord.dwMDDataType		= MULTISZ_METADATA;
	rec.metaRecord.dwMDDataLen		= METAOPIIS_BUFFER_SIZE;
	rec.metaRecord.pbMDData			= (unsigned char *)pwszBuf;
	rec.metaRecord.dwMDDataTag		= 0;

	hRet = m_pIMeta->GetData(hCurrent, NULL, &rec.metaRecord, 
		&dwReqBufLen);
	if ( RETURNCODETOHRESULT(ERROR_INSUFFICIENT_BUFFER) == hRet )
	{
		delete [] pwszBuf;
		pwszBuf = new WCHAR[dwReqBufLen/sizeof(WCHAR)];
		memset(pwszBuf,0xff, dwReqBufLen/sizeof(WCHAR));
		rec.metaRecord.dwMDDataLen = dwReqBufLen;
		rec.metaRecord.pbMDData = (unsigned char *)pwszBuf;
		hRet = m_pIMeta->GetData(hCurrent, NULL, &rec.metaRecord, 
			&dwReqBufLen);
		if ( !SUCCEEDED(hRet) )		
		{
			m_pIMeta->CloseKey(hCurrent);
			CErrorOperation::s_dwErrorCode = GetLastError();
			CErrorOperation::WriteErrorLog();	
			return false;
		}
		dwBufLen = dwReqBufLen/sizeof(WCHAR);
	}

	m_pIMeta->CloseKey(hCurrent);
	return true;
}
*/


BOOL CMetaOpIIS::AddVDir()
{
	//	增加一个虚拟目录键
	STMETAOPERRECORD rec;
	WCHAR wszTemp[METAOPIIS_KEYNAME_LEN]		= {0};

	//	检查Site_KeyName是否为空
	if ( 0 == strlen(m_VDir.szSiteKeyName) )
	{
		setLastErrorInfo( 0, "站点键名为空" );
		return FALSE;
	}
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szSiteKeyName, strlen(m_VDir.szSiteKeyName)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC/" );
	wcscat( rec.wszKeyPath, wszTemp );
	wcscat( rec.wszKeyPath, L"/Root" );

	memset( wszTemp, 0, sizeof(wszTemp) );
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szKeyName_VirDir, strlen(m_VDir.szKeyName_VirDir)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy(rec.wszKeyName, wszTemp);	

	if ( ! mb_createKey( & rec ) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}

	//	修改虚拟目录路径
	WCHAR pwszBuf[MAX_PATH]		= {0};
	memset(pwszBuf, 0, MAX_PATH*2);
	
	MultiByteToWideChar(CP_ACP, 0, m_VDir.szPath_VirDir, strlen(m_VDir.szPath_VirDir)+1, pwszBuf, MAX_PATH);
	wcscat( rec.wszKeyPath, L"/" );
	wcscat( rec.wszKeyPath, wszTemp );
	if ( ! modifyRecordData( & rec, MD_VR_PATH, 0, IIS_MD_UT_FILE, STRING_METADATA, pwszBuf, (wcslen(pwszBuf)+1)*2 ) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}
	
	//	修改访问属性
	memset(pwszBuf, 0, MAX_PATH*2);
	memcpy(pwszBuf, &m_VDir.dwAccessFlag, sizeof(DWORD));
	if ( ! modifyRecordData( & rec, MD_ACCESS_PERM, 0, IIS_MD_UT_FILE, DWORD_METADATA, pwszBuf, sizeof(DWORD) ) )
	{
		setLastErrorInfo( 0, "" );
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMetaOpIIS::DeleteVDir()
{
	//	删除一个虚拟目录键
	STMETAOPERRECORD rec;
	WCHAR wszTemp[ METAOPIIS_KEYNAME_LEN ]	= {0};

	MultiByteToWideChar( CP_ACP, 0, m_VDir.szSiteKeyName, strlen(m_VDir.szSiteKeyName)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC/" );
	wcscat( rec.wszKeyPath, wszTemp );
	wcscat( rec.wszKeyPath, L"/Root" );
	
	memset( wszTemp, 0, sizeof(wszTemp) );
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szKeyName_VirDir, strlen(m_VDir.szKeyName_VirDir)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyName, wszTemp );	
	
	if ( ! mb_deleteKey( & rec ) )
	{
		setLastErrorInfo( 0, "" );
	}

	return TRUE;
}

BOOL CMetaOpIIS::ModifyVDir()
{
	//	设置KEYPATH
	STMETAOPERRECORD rec;

	WCHAR wszTemp[ METAOPIIS_KEYNAME_LEN ]	= {0};
	WCHAR wszBuf[ MAX_PATH ]		= {0};

	MultiByteToWideChar( CP_ACP, 0, m_VDir.szSiteKeyName, strlen(m_VDir.szSiteKeyName)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC" );
	wcscat( rec.wszKeyPath, wszTemp );
	wcscat( rec.wszKeyPath, L"/Root" );
	
	memset( wszTemp, 0, sizeof(wszTemp) );
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szKeyName_VirDir, strlen(m_VDir.szKeyName_VirDir)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyName, wszTemp );	

	//	修改虚拟目录路径
	if ( 0 != strlen(m_VDir.szPath_VirDir) )
	{
		MultiByteToWideChar(CP_ACP, 0, m_VDir.szPath_VirDir, strlen(m_VDir.szPath_VirDir)+1, wszBuf, MAX_PATH);
		wcscat( rec.wszKeyPath, wszTemp );
		if ( ! modifyRecordData( & rec, MD_VR_PATH, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, (wcslen(wszBuf)+1)*2 ) )
		{
			setLastErrorInfo( 0, "" );
			return FALSE;
		}
		
	}
	//	修改访问属性
	if ( 0 != m_VDir.dwAccessFlag ) 
	{
		memcpy( wszBuf, &m_VDir.dwAccessFlag, sizeof(DWORD) );
		if ( ! modifyRecordData( & rec, MD_ACCESS_PERM, 0, IIS_MD_UT_FILE, DWORD_METADATA, wszBuf, sizeof(DWORD) ) )
		{
			setLastErrorInfo( 0, "" );
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CMetaOpIIS::HttpRedirect()
{
	//	如果没有该虚拟目录子键，则建立一个
	BOOL bRet				= FALSE;
	STMETAOPERRECORD rec;
	WCHAR wszTemp[ METAOPIIS_KEYNAME_LEN ]	= {0};
	WCHAR wszBuf[ MAX_PATH ]		= {0};

	MultiByteToWideChar( CP_ACP, 0, m_VDir.szSiteKeyName, strlen(m_VDir.szSiteKeyName)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyPath, L"/LM/W3SVC/" );
	wcscat( rec.wszKeyPath, wszTemp );
	wcscat( rec.wszKeyPath, L"/Root" );

	memset( wszTemp, 0, sizeof(wszTemp) );
	MultiByteToWideChar( CP_ACP, 0, m_VDir.szKeyName_VirDir, strlen(m_VDir.szKeyName_VirDir)+1, wszTemp, METAOPIIS_KEYNAME_LEN );
	wcscpy( rec.wszKeyName, wszTemp );

	if ( mb_createKey( & rec ) )
	{
		//	修改http重定位路径
		MultiByteToWideChar( CP_ACP, 0, m_VDir.szPath_VirDir, strlen(m_VDir.szPath_VirDir)+1, wszBuf, MAX_PATH );
		wcscat( rec.wszKeyPath, L"/" );
		wcscat( rec.wszKeyPath, rec.wszKeyName );
		bRet = modifyRecordData( & rec, MD_HTTP_REDIRECT, 0, IIS_MD_UT_FILE, STRING_METADATA, wszBuf, wcslen(wszBuf)*2+2 );
	}

	return bRet;
}


/**
 *	设置最后错误信息
 */
VOID CMetaOpIIS::setLastErrorInfo( DWORD dwOperErrorCode, LPCTSTR lpszFmt, ... )
{
	INT nSize = 0;
	va_list args;

	m_dwLastSysErrorCode	= GetLastError();
	m_dwLastOperErrorCode	= dwOperErrorCode;

	memset( m_szLastErrorInfo, 0, sizeof(m_szLastErrorInfo) );
	va_start( args, lpszFmt );
	nSize = _vsnprintf( m_szLastErrorInfo, sizeof(m_szLastErrorInfo), lpszFmt, args );
	va_end( args );
}



