// ProcCfg.cpp: implementation of the CProcCfg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcCfg.h"
#include "VwSpiderMatch.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


#ifndef RUN_ENV_SERVICE
	#ifndef RUN_ENV_VWUPETC
		#include "../VwAntiLeechs/VwCfgNew/VwCfgNew.h"
	#endif
#endif


#pragma warning( disable : 4509 )


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcCfg::CProcCfg( BOOL bShowMessageBox /* = TRUE */ )
{

#ifdef __VWCFGNEW_HEADER__

	//
	//	仅仅是在 VwCfgNew 配置工具运行的时候
	//

	CVwCfgNewApp *pApp=(CVwCfgNewApp *)AfxGetApp();
	if ( pApp )
	{
		CProcDb::SetShowMessageBox( bShowMessageBox );
		CProcDb::ConnDatabase( pApp->m_szCfgDbFile );

		//
		//	更新为最新版本
		//
		UpdateDatabase();
	}
	
#endif

}

CProcCfg::~CProcCfg()
{

#ifdef __VWCFGNEW_HEADER__

	//
	//	关闭数据库连接
	//
	CProcDb::CloseDatabase();

#endif

}


/**
 *	更新数据库
 */
BOOL CProcCfg::UpdateDatabase()
{
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}

	//
	//	for 3.0.6.1045
	//
	if ( ! CProcDb::IsExistColumn( "iis_table", "iis_pubvsid_type" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_table ADD COLUMN iis_pubvsid_type INTEGER DEFAULT 0", FALSE );
	}

	//
	//	2007-01-22 增加防攻击功能
	//
	if ( ! CProcDb::IsExistColumn( "iis_security_table", "isc_enable_antatt" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_security_table ADD COLUMN isc_enable_antatt INTEGER DEFAULT 0", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_security_table SET isc_enable_antatt=0", FALSE );
	}
	if ( ! CProcDb::IsExistColumn( "iis_security_table", "isc_antatt_maxvisit" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_security_table ADD COLUMN isc_antatt_maxvisit INTEGER DEFAULT 1000", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_security_table SET isc_antatt_maxvisit=1000", FALSE );
	}

	if ( ! CProcDb::IsExistColumn( "iis_security_table", "isc_antatt_sleep" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_security_table ADD COLUMN isc_antatt_sleep INTEGER DEFAULT 1", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_security_table SET isc_antatt_sleep=1", FALSE );
	}

	if ( ! CProcDb::IsExistColumn( "iis_security_table", "isc_antatt_disabled_proxy" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_security_table ADD COLUMN isc_antatt_disabled_proxy INTEGER DEFAULT 0", FALSE );
	}

	if ( ! CProcDb::IsExistColumn( "iis_security_table", "isc_antatt_bwlist" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_security_table ADD COLUMN isc_antatt_bwlist TEXT DEFAULT \"\"", FALSE );
	}

	if ( ! CProcDb::IsExistTable( "iis_bwlist_table" ) )
	{
		//	CREATE TABLE iis_bwlist_table( bw_id INTEGER(11) NOT NULL PRIMARY KEY AUTOINCREMENT, iis_siteid VARCHAR(32), bw_type INTEGER, bw_ipaddr VARCHAR(16), bw_reason INTEGER );
		CProcDb::ExectueSql( "CREATE TABLE iis_bwlist_table( bw_id AUTOINCREMENT NOT NULL PRIMARY KEY, iis_siteid VARCHAR(32), bw_type INTEGER DEFAULT 0, bw_ipaddr VARCHAR(16) NOT NULL, bw_reason INTEGER DEFAULT 0 );", FALSE );
	}
//	if ( ! CProcDb::IsExistColumn( "iis_bwlist_table", "iis_siteid" ) )
//	{
//	}

	//
	//	2007-01-27 增加 VSID 设置 COOKIE 页面字段
	//
	if ( ! CProcDb::IsExistColumn( "iis_table", "iis_pubvsid_cookiepage" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_table ADD COLUMN iis_pubvsid_cookiepage VARCHAR(255) DEFAULT \"\"", FALSE );
	}

	//
	//	2007-02-1 增加定时开关功能字段
	//
	if ( ! CProcDb::IsExistColumn( "iis_table", "iis_start_time_switch" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_table ADD COLUMN iis_start_time_switch INTEGER DEFAULT 0", FALSE );
	}

	//
	//	2007-02-20 增加正式版本验证标记
	//
	if ( ! CProcDb::IsExistColumn( "main_table", "m_is_reg_user" ) )
	{
		//	m_is_reg_user INTEGER 是否是正式版本用户
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_is_reg_user INTEGER DEFAULT 0", FALSE );
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_reg_check_date DATETIME DEFAULT 0", FALSE );
	}

	//
	//	2007-10-08 增加：用户必须使用客户端插件的标记
	//
	if ( ! CProcDb::IsExistColumn( "iis_table", "iis_pubvsid_usebrowserin" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_table ADD COLUMN iis_pubvsid_usebrowserin INTEGER DEFAULT 0", FALSE );
	}

	//
	//	2007-10-22 增加：日 IP 下载次数限制
	//
	if ( ! CProcDb::IsExistColumn( "iis_security_table", "isc_enable_lmt_dayip" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_security_table ADD COLUMN isc_enable_lmt_dayip INTEGER DEFAULT 0", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_security_table SET isc_enable_lmt_dayip=0", FALSE );
	}
	if ( ! CProcDb::IsExistColumn( "iis_security_table", "isc_lmt_dayip" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_security_table ADD COLUMN isc_lmt_dayip INTEGER DEFAULT 0", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_security_table SET isc_lmt_dayip=0", FALSE );
	}

	//
	//	2007-12-22 增加：注册信息保存几个字段
	//
	if ( ! CProcDb::IsExistColumn( "main_table", "m_reg_expire_type" ) )
	{
		//	m_is_reg_user INTEGER 是否是正式版本用户
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_reg_expire_type INTEGER DEFAULT 0", FALSE );
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_reg_create_date DATETIME", FALSE );
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_reg_expire_date DATETIME", FALSE );
	}

	//
	//	2008-10-03 增加： host_table > hst_use
	//
	if ( ! CProcDb::IsExistColumn( "host_table", "hst_use" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE host_table ADD COLUMN hst_use INTEGER DEFAULT 0", FALSE );
	}
	if ( ! CProcDb::IsExistColumn( "iis_advanced_table", "iav_wm_http_status" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_advanced_table ADD COLUMN iav_wm_http_status INTEGER DEFAULT 200", FALSE );
	}
	
	//
	//	2008-10-30 Add :: main_table > m_max_friend_hosts, m_max_allow_dirs
	//
	if ( ! CProcDb::IsExistColumn( "main_table", "m_max_allow_dirs" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_max_allow_dirs INTEGER DEFAULT 32", FALSE );
		CProcDb::ExectueSql( "UPDATE main_table SET m_max_allow_dirs=32", FALSE );
	}
	if ( ! CProcDb::IsExistColumn( "main_table", "m_max_friend_hosts" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_max_friend_hosts INTEGER DEFAULT 32", FALSE );
		CProcDb::ExectueSql( "UPDATE main_table SET m_max_friend_hosts=32", FALSE );
	}

	//
	//	2009-01-16 Add :: iis_friendhosts_table > ifh_type
	//
	if ( ! CProcDb::IsExistColumn( "iis_friendhosts_table", "ifh_type" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_friendhosts_table ADD COLUMN ifh_type INTEGER DEFAULT 0", FALSE );
		
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "UPDATE iis_friendhosts_table SET ifh_type=%d", CPROCCFG_IFH_TYPE_WEBSITE );
		CProcDb::ExectueSql( m_szSql, FALSE );
	}

	//
	//	2009-01-20 Add :: iis_table > iis_root_path
	//
	if ( ! CProcDb::IsExistColumn( "iis_table", "iis_root_path" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_table ADD COLUMN iis_root_path TEXT DEFAULT \"\"", FALSE );
	}

	//
	//	2009-05-21 Add :: main_table > m_reg_prtype, m_reg_prtypecs
	//
	if ( ! CProcDb::IsExistColumn( "main_table", "m_reg_prtype" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_reg_prtype VARCHAR(32) DEFAULT \"\"", FALSE );
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_reg_prtypecs VARCHAR(64) DEFAULT \"\"", FALSE );
	}

	//
	//	2009-05-30 Add :: iis_table > iis_apppoolid
	//
	if ( ! CProcDb::IsExistColumn( "iis_table", "iis_apppoolid" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_table ADD COLUMN iis_apppoolid VARCHAR(128) DEFAULT \"\"", FALSE );
	}

	//
	//	2009-07-11 Add :: iis_advanced_table > iav_wm_sitelogo_url
	//
	if ( ! CProcDb::IsExistColumn( "iis_advanced_table", "iav_wm_sitelogo_url" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_advanced_table ADD COLUMN iav_wm_sitelogo_url VARCHAR(255) DEFAULT \"\"", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_advanced_table SET iav_wm_sitelogo_url='http://'", FALSE );
	}

	//
	//	2009-07-31 Create Table [iis_blockhosts_table] & [iis_blockip_table]
	//
	if ( ! CProcDb::IsExistTable( "iis_blockhosts_table" ) )
	{
		CProcDb::ExectueSql( "CREATE TABLE iis_blockhosts_table( \
			ibh_id AUTOINCREMENT NOT NULL PRIMARY KEY, \
			iis_siteid VARCHAR(32), \
			ibh_use INTEGER DEFAULT 0, \
			ibh_type INTEGER DEFAULT 0, \
			ibh_name VARCHAR(255) NOT NULL );", FALSE );
	}
	if ( ! CProcDb::IsExistTable( "iis_blockip_table" ) )
	{
		CProcDb::ExectueSql( "CREATE TABLE iis_blockip_table( \
			ibip_id AUTOINCREMENT NOT NULL PRIMARY KEY, \
			iis_siteid VARCHAR(32), \
			ibip_use INTEGER DEFAULT 0, \
			ibip_type INTEGER DEFAULT 0, \
			ibip_name VARCHAR(32) NOT NULL );", FALSE );
	}


	//
	//	2009-10-20 Add :: iis_table > iis_pubvsid_use_proxy and iis_pubvsid_proxy_variable
	//
	if ( ! CProcDb::IsExistColumn( "main_table", "m_use_proxy" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_use_proxy INTEGER DEFAULT 0", FALSE );
		CProcDb::ExectueSql( "UPDATE main_table SET m_use_proxy=0", FALSE );

		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_proxy_variable VARCHAR(128) DEFAULT \"\"", FALSE );
		CProcDb::ExectueSql( "UPDATE main_table SET m_proxy_variable=''", FALSE );
	}

	//
	//	2010-01-06 :: iis_friendhosts_table > ifh_except_config
	//
	if ( ! CProcDb::IsExistColumn( "iis_friendhosts_table", "ifh_except_config" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_friendhosts_table ADD COLUMN ifh_except_config INTEGER DEFAULT -1", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_friendhosts_table SET ifh_except_config=-1", FALSE );
	}
	if ( ! CProcDb::IsExistColumn( "iis_allowdirs_table", "iad_except_config" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_allowdirs_table ADD COLUMN iad_except_config INTEGER DEFAULT -1", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_allowdirs_table SET iad_except_config=-1", FALSE );
	}

	//
	//	2010-07-22 增加：登录密码
	//
	if ( ! CProcDb::IsExistColumn( "main_table", "m_login_password" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_login_algorithm INTEGER DEFAULT 0", FALSE );
		CProcDb::ExectueSql( "ALTER TABLE main_table ADD COLUMN m_login_password VARCHAR(64) DEFAULT \"\"", FALSE );
	}

	//
	//	2010-08-25 增加：iis_pubvsid_auto_expired
	//
	if ( ! CProcDb::IsExistColumn( "iis_table", "iis_pubvsid_auto_expired" ) )
	{
		CProcDb::ExectueSql( "ALTER TABLE iis_table ADD COLUMN iis_pubvsid_auto_expired INTEGER DEFAULT 0", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_table SET iis_pubvsid_auto_expired=1", FALSE );
		CProcDb::ExectueSql( "UPDATE iis_table SET iis_pubvsid_auto_expired=0", FALSE );
	}

	return TRUE;
}



BOOL CProcCfg::GetMainTableInfoById( UINT m_id, PSTMAINTABLE pstMainTable )
{
	if ( NULL == pstMainTable )
		return FALSE;
	if ( ! CProcDb::IsConnDatabase() )
		return FALSE;

	BOOL bRet	= FALSE;

/*	UINT	m_id;			// 自增字段
	TCHAR	m_key[32];		// 注册码
	TCHAR	m_vw_ver[32];		// 软件版本
	TCHAR	m_vw_system[32];
	TCHAR	m_reg_ipaddr[16];
	TCHAR	m_reg_host[64];
*/

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT TOP 1 * FROM main_table WHERE m_id=%d", m_id );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		if ( ! m_rs->adoEOF )
		{
			bRet = TRUE;

			pstMainTable->m_id = QueryIntValue( m_rs, "m_id" );
			QueryStringValue( m_rs, "m_key", pstMainTable->m_key, sizeof(pstMainTable->m_key) );
			QueryStringValue( m_rs, "m_vw_ver", pstMainTable->m_vw_ver, sizeof(pstMainTable->m_vw_ver) );
			QueryStringValue( m_rs, "m_vw_system", pstMainTable->m_vw_system, sizeof(pstMainTable->m_vw_system) );
			QueryStringValue( m_rs, "m_reg_ipaddr", pstMainTable->m_reg_ipaddr, sizeof(pstMainTable->m_reg_ipaddr) );
			QueryStringValue( m_rs, "m_reg_host", pstMainTable->m_reg_host, sizeof(pstMainTable->m_reg_host) );

			pstMainTable->m_is_reg_user	= QueryIntValue( m_rs, "m_is_reg_user" );
			pstMainTable->m_reg_expire_type	= QueryIntValue( m_rs, "m_reg_expire_type" );

			//	2009-05-21
			//	main_table > m_reg_prtype, m_reg_prtypecs
			QueryStringValue( m_rs, "m_reg_prtype", pstMainTable->m_reg_prtype, sizeof(pstMainTable->m_reg_prtype) );
			QueryStringValue( m_rs, "m_reg_prtypecs", pstMainTable->m_reg_prtypecs, sizeof(pstMainTable->m_reg_prtypecs) );

			QueryStringValue( m_rs, "m_reg_check_date", pstMainTable->m_reg_check_date, sizeof(pstMainTable->m_reg_check_date) );
			QueryStringValue( m_rs, "m_reg_create_date", pstMainTable->m_reg_create_date, sizeof(pstMainTable->m_reg_create_date) );
			QueryStringValue( m_rs, "m_reg_expire_date", pstMainTable->m_reg_expire_date, sizeof(pstMainTable->m_reg_expire_date) );

			pstMainTable->m_max_allow_dirs		= QueryIntValue( m_rs, "m_max_allow_dirs" );
			pstMainTable->m_max_friend_hosts	= QueryIntValue( m_rs, "m_max_friend_hosts" );
			
			pstMainTable->m_use_proxy		= QueryIntValue( m_rs, "m_use_proxy" );
			QueryStringValue( m_rs, "m_proxy_variable", pstMainTable->m_proxy_variable, sizeof(pstMainTable->m_proxy_variable) );

			pstMainTable->m_login_algorithm		= QueryIntValue( m_rs, "m_login_algorithm" );
			QueryStringValue( m_rs, "m_login_password", pstMainTable->m_login_password, sizeof(pstMainTable->m_login_password) );
		}
		m_rs->Close();
	}

	return bRet;
}
BOOL CProcCfg::SaveMainTableInfoById( UINT m_id, LPSTR lpszField, LPSTR lpszValue )
{
	//
	//	UINT m_id		- m_id
	//	LPCSTR lpcszField	- 需要更新的字段名
	//	LPCSTR lpcszValue	- 对应字段的值
	//

	if ( NULL == lpszField || NULL == lpszValue )
		return FALSE;
	if ( 0 == strlen(lpszField) )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar(lpszField);
	CProcDb::KillBadChar(lpszValue);

	if ( strlen(lpszField) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE main_table SET %s='%s' WHERE m_id=%d",
			lpszField, lpszValue, m_id );

		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	// ..
	return bRet;
}
BOOL CProcCfg::SaveMainTableInfoById( UINT m_id, LPSTR lpszField, LONG lnValue )
{
	//
	//	UINT m_id		- m_id
	//	LPCSTR lpcszField	- 需要更新的字段名
	//	LPCSTR lpcszValue	- 对应字段的值
	//

	if ( NULL == lpszField )
		return FALSE;
	if ( 0 == strlen(lpszField) )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar(lpszField);

	if ( strlen(lpszField) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE main_table SET %s=%d WHERE m_id=%d",
			lpszField, lnValue, m_id );

		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	// ..
	return bRet;
}




/**
 *	for [host_table]
 *	获取所有 HOST 信息
 */
BOOL CProcCfg::GetAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll )
{
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}


	BOOL bRet	= FALSE;
	STHOSTINFO stHost;

	//
	//	hst_id		-
	//	hst_siteid	-
	//	hst_name	-
	//	hst_port	-
	//	hst_ipaddr	-
	//

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT TOP %d * FROM host_table", MAX_HOSTCOUNT );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		while ( ! m_rs->adoEOF )
		{
			bRet = TRUE;
			memset( &stHost, 0, sizeof(stHost) );

			stHost.lnUse = QueryIntValue( m_rs, "hst_use" );
			stHost.uPort = QueryIntValue( m_rs, "hst_port" );
			QueryStringValue( m_rs, "hst_siteid", stHost.szSiteId, sizeof(stHost.szSiteId) );
			QueryStringValue( m_rs, "hst_name", stHost.szHostName, sizeof(stHost.szHostName) );
			QueryStringValue( m_rs, "hst_ipaddr", stHost.szIpAddr, sizeof(stHost.szIpAddr) );

			if ( strlen( stHost.szIpAddr ) > 6 && delib_is_valid_ipaddr( stHost.szIpAddr ) )
			{
				//	将字符串的 IP 地址转换成 DWORD 的数字
				stHost.dwIpAddrValue = delib_get_ipaddr_value_from_string( stHost.szIpAddr );
			}

			//	..
			vcHostInfoAll.push_back( stHost );

			//	..
			m_rs->MoveNext();
		}
		m_rs->Close();
	}

	return bRet;
}
/**
 *	[host_table]
 *	Get All Hosts from Metabase and than
 *	Save them to cfg.mdb
 */
BOOL CProcCfg::SaveAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll )
{
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	vector<STHOSTINFO>::iterator it;
	STHOSTINFO stHost;
	UINT uId = 0;

	strcpy( m_szSql, "DELETE FROM host_table" );
	if ( CProcDb::ExectueSql( m_szSql ) )
	{
		bRet	= TRUE;
		uId	= 0;
		for ( it = vcHostInfoAll.begin(); it != vcHostInfoAll.end(); it ++ )
		{
			memset( &stHost, 0, sizeof(stHost) );
			stHost = (*it);

			CProcDb::KillBadChar( stHost.szHostName );
			CProcDb::KillBadChar( stHost.szIpAddr );
			CProcDb::KillBadChar( stHost.szSiteId );

			_sntprintf
			(
				m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
				"INSERT INTO host_table " \
				"( hst_id, hst_siteid, hst_use, hst_name, hst_port, hst_ipaddr ) VALUES " \
				"( %d, '%s', %d, '%s', %d, '%s' )",
				uId, stHost.szSiteId, stHost.lnUse, stHost.szHostName, stHost.uPort, stHost.szIpAddr
			);
			bRet &= CProcDb::ExectueSql( m_szSql );

			uId ++;
		}
	}

	return bRet;
}


/**
 *	Try to Create a new config(default) for the website of specifis id
 */
BOOL CProcCfg::TryToCreateNewSiteCfg( LPCSTR lpcszSiteId )
{
	/*
		lpcszSiteId	- [in]  Pointer to a character null-terminated string containing the SiteId
	*/

	if ( NULL == lpcszSiteId )
		return FALSE;
	if ( 0 == strlen(lpcszSiteId) )
		return FALSE;


	BOOL bRet	= FALSE;
	STIISTABLE stIis;
	STIISBLOCKEXTTABLE stBlcokExt;
	STIISALLOWDIRSTABLE stAllowDir;
	STIISFRIENDHOSTSTABLE stFriendHost;
	STIISSECURITYTABLE stSecurity;
	STIISLOGABLE stLog;
	STIISADVANCEDTABLE stAdvanced;


	//	如果站点的配置信息不存在才创建之
	if ( ! GetIisTableInfoBySiteId( lpcszSiteId, & stIis ) )
	{
		memset( &stIis, 0, sizeof(stIis) );

		_sntprintf( stIis.iis_siteid, sizeof(stIis.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
		stIis.iis_start			= 1;
		stIis.iis_enable_protects	= 1;
		stIis.iis_enable_exceptions	= 1;
		stIis.iis_enable_limits		= 1;
		stIis.iis_enable_security	= 1;
		stIis.iis_enable_logs		= 1;
		stIis.iis_type			= BLOCK_TYPE_SUPER;
		stIis.iis_pubvsid_usebrowserin	= DEF_PUBVSIDUSEBROWSERIN;
		stIis.iis_pubvsid_usecookie	= DEF_PUBVSIDUSECOOKIE;
		_sntprintf( stIis.iis_pubvsid_key, sizeof(stIis.iis_pubvsid_key)-sizeof(TCHAR), "%s", DEF_PUBVSIDKEY );
		stIis.iis_pubvsid_keytime	= DEF_PUBVSIDKEYTIME;

		if ( SaveIisTableInfoBySiteId( lpcszSiteId, & stIis ) )
		{
			bRet = TRUE;

			//	[1]
			//	更新为启用
			UpdateIisTableUseBySiteId( lpcszSiteId, 1 );

			//	[2]
			//	add default block ext ".htw|.bat|.log|.ida|.mdb|.exe|.rar|.zip"
			memset( &stBlcokExt, 0, sizeof(stBlcokExt) );
			_sntprintf( stBlcokExt.iis_siteid, sizeof(stBlcokExt.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stBlcokExt.ibe_name, sizeof(stBlcokExt.ibe_name)-sizeof(TCHAR), "%s", ".htw" );
			stBlcokExt.ibe_id		= INSERT_NEW_ITEM_FIRST + 1;
			stBlcokExt.ibe_use		= 1;
			stBlcokExt.ibe_config		= CONFIG_DENYALLHTTP;
			stBlcokExt.ibe_action		= ACTION_SENDMESSAGES;
			SaveIisBlockExtTableInfoById( stBlcokExt.ibe_id, & stBlcokExt );

			memset( &stBlcokExt, 0, sizeof(stBlcokExt) );
			_sntprintf( stBlcokExt.iis_siteid, sizeof(stBlcokExt.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stBlcokExt.ibe_name, sizeof(stBlcokExt.ibe_name)-sizeof(TCHAR), "%s", ".bat" );
			stBlcokExt.ibe_id		= INSERT_NEW_ITEM_FIRST + 1;
			stBlcokExt.ibe_use		= 1;
			stBlcokExt.ibe_config		= CONFIG_DENYALLHTTP;
			stBlcokExt.ibe_action		= ACTION_SENDMESSAGES;
			SaveIisBlockExtTableInfoById( stBlcokExt.ibe_id, & stBlcokExt );

			memset( &stBlcokExt, 0, sizeof(stBlcokExt) );
			_sntprintf( stBlcokExt.iis_siteid, sizeof(stBlcokExt.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stBlcokExt.ibe_name, sizeof(stBlcokExt.ibe_name)-sizeof(TCHAR), "%s", ".log" );
			stBlcokExt.ibe_id		= INSERT_NEW_ITEM_FIRST + 1;
			stBlcokExt.ibe_use		= 1;
			stBlcokExt.ibe_config		= CONFIG_DENYALLHTTP;
			stBlcokExt.ibe_action		= ACTION_SENDMESSAGES;
			SaveIisBlockExtTableInfoById( stBlcokExt.ibe_id, & stBlcokExt );

			memset( &stBlcokExt, 0, sizeof(stBlcokExt) );
			_sntprintf( stBlcokExt.iis_siteid, sizeof(stBlcokExt.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stBlcokExt.ibe_name, sizeof(stBlcokExt.ibe_name)-sizeof(TCHAR), "%s", ".ida" );
			stBlcokExt.ibe_id		= INSERT_NEW_ITEM_FIRST + 1;
			stBlcokExt.ibe_use		= 1;
			stBlcokExt.ibe_config		= CONFIG_DENYALLHTTP;
			stBlcokExt.ibe_action		= ACTION_SENDMESSAGES;
			SaveIisBlockExtTableInfoById( stBlcokExt.ibe_id, & stBlcokExt );

			memset( &stBlcokExt, 0, sizeof(stBlcokExt) );
			_sntprintf( stBlcokExt.iis_siteid, sizeof(stBlcokExt.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stBlcokExt.ibe_name, sizeof(stBlcokExt.ibe_name)-sizeof(TCHAR), "%s", ".mdb" );
			stBlcokExt.ibe_id		= INSERT_NEW_ITEM_FIRST + 1;
			stBlcokExt.ibe_use		= 1;
			stBlcokExt.ibe_config		= CONFIG_DENYALLHTTP;
			stBlcokExt.ibe_action		= ACTION_SENDMESSAGES;
			SaveIisBlockExtTableInfoById( stBlcokExt.ibe_id, & stBlcokExt );

			memset( &stBlcokExt, 0, sizeof(stBlcokExt) );
			_sntprintf( stBlcokExt.iis_siteid, sizeof(stBlcokExt.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stBlcokExt.ibe_name, sizeof(stBlcokExt.ibe_name)-sizeof(TCHAR), "%s", ".exe" );
			stBlcokExt.ibe_id		= INSERT_NEW_ITEM_FIRST + 1;
			stBlcokExt.ibe_use		= 1;
			stBlcokExt.ibe_config		= CONFIG_DENYALLHTTP;
			stBlcokExt.ibe_action		= ACTION_SENDMESSAGES;
			SaveIisBlockExtTableInfoById( stBlcokExt.ibe_id, & stBlcokExt );

			memset( &stBlcokExt, 0, sizeof(stBlcokExt) );
			_sntprintf( stBlcokExt.iis_siteid, sizeof(stBlcokExt.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stBlcokExt.ibe_name, sizeof(stBlcokExt.ibe_name)-sizeof(TCHAR), "%s", ".rar" );
			stBlcokExt.ibe_id		= INSERT_NEW_ITEM_FIRST + 1;
			stBlcokExt.ibe_use		= 1;
			stBlcokExt.ibe_config		= CONFIG_ANTILEECHS;
			stBlcokExt.ibe_action		= ACTION_SENDMESSAGES;
			SaveIisBlockExtTableInfoById( stBlcokExt.ibe_id, & stBlcokExt );

			memset( &stBlcokExt, 0, sizeof(stBlcokExt) );
			_sntprintf( stBlcokExt.iis_siteid, sizeof(stBlcokExt.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stBlcokExt.ibe_name, sizeof(stBlcokExt.ibe_name)-sizeof(TCHAR), "%s", ".zip" );
			stBlcokExt.ibe_id		= INSERT_NEW_ITEM_FIRST + 1;
			stBlcokExt.ibe_use		= 1;
			stBlcokExt.ibe_config		= CONFIG_ANTILEECHS;
			stBlcokExt.ibe_action		= ACTION_SENDMESSAGES;
			SaveIisBlockExtTableInfoById( stBlcokExt.ibe_id, & stBlcokExt );

			//	[3]
			//	add default allow dir "/images"
			memset( &stAllowDir, 0, sizeof(stAllowDir) );
			_sntprintf( stAllowDir.iis_siteid, sizeof(stAllowDir.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stAllowDir.iad_name, sizeof(stAllowDir.iad_name)-sizeof(TCHAR), "%s", "/images/" );
			stAllowDir.iad_id		= INSERT_NEW_ITEM_FIRST + 1;
			stAllowDir.iad_use		= 1;
			SaveIisAllowDirTableInfoById( stAllowDir.iad_id, & stAllowDir );


			//
			//	[4]
			//	添加默认搜索引擎蜘蛛例外
			/*
			memset( &stFriendHost, 0, sizeof(stFriendHost) );
			_sntprintf( stFriendHost.iis_siteid, sizeof(stFriendHost.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stFriendHost.ifh_name, sizeof(stFriendHost.ifh_name)-sizeof(TCHAR), "%d", VWSPIDERMATCH_IDX_YAHOO );
			stFriendHost.ifh_id		= INSERT_NEW_ITEM_FIRST + 1;
			stFriendHost.ifh_use		= 1;
			stFriendHost.ifh_type		= CPROCCFG_IFH_TYPE_SPIDER;
			SaveIisFriendHostsTableInfoById( stFriendHost.ifh_id, &stFriendHost );

			memset( &stFriendHost, 0, sizeof(stFriendHost) );
			_sntprintf( stFriendHost.iis_siteid, sizeof(stFriendHost.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stFriendHost.ifh_name, sizeof(stFriendHost.ifh_name)-sizeof(TCHAR), "%d", VWSPIDERMATCH_IDX_BAIDU );
			stFriendHost.ifh_id		= INSERT_NEW_ITEM_FIRST + 1;
			stFriendHost.ifh_use		= 1;
			stFriendHost.ifh_type		= CPROCCFG_IFH_TYPE_SPIDER;
			SaveIisFriendHostsTableInfoById( stFriendHost.ifh_id, &stFriendHost );

			memset( &stFriendHost, 0, sizeof(stFriendHost) );
			_sntprintf( stFriendHost.iis_siteid, sizeof(stFriendHost.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stFriendHost.ifh_name, sizeof(stFriendHost.ifh_name)-sizeof(TCHAR), "%d", VWSPIDERMATCH_IDX_GOOGLE );
			stFriendHost.ifh_id		= INSERT_NEW_ITEM_FIRST + 1;
			stFriendHost.ifh_use		= 1;
			stFriendHost.ifh_type		= CPROCCFG_IFH_TYPE_SPIDER;
			SaveIisFriendHostsTableInfoById( stFriendHost.ifh_id, &stFriendHost );
			*/


			//	[4]
			//	add default to secturity
			memset( &stSecurity, 0, sizeof(stSecurity) );
			_sntprintf( stSecurity.iis_siteid, sizeof(stSecurity.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			_sntprintf( stSecurity.isc_lmt_urlchars_cfg, sizeof(stSecurity.isc_lmt_urlchars_cfg)-sizeof(TCHAR), "%s", DEF_ISC_LMT_URLCHARS_CFG );
			stSecurity.isc_id			= INSERT_NEW_ITEM_FIRST + 1;
			stSecurity.isc_iis_disguise_cfg		= ISC_DISGUISE_AS_APACHE;
			stSecurity.isc_enable_antatt		= DEF_ISC_ENABLE_ANTATT;		//	是否开启防攻击
			stSecurity.isc_antatt_maxvisit		= DEF_ISC_ANTATT_MAXVISIT;		//	每分钟最大访问量
			stSecurity.isc_antatt_sleep		= DEF_ISC_ANTATT_SLEEP;			//	发现攻击者，拒绝服务的时间
			stSecurity.isc_antatt_disabled_proxy	= DEF_ISC_ANTATT_DISABLED_PROXY;	//	是否禁止代理访问
			SaveIisSecurityTableInfoById( stSecurity.isc_id, & stSecurity );

			//	[5]
			//	add default to log
			memset( &stLog, 0, sizeof(stLog) );
			_sntprintf( stLog.iis_siteid, sizeof(stLog.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			stLog.ilg_id			= INSERT_NEW_ITEM_FIRST + 1;
			stLog.ilg_logtimeperiod		= ILG_LOGTIMEPERIOD_DAILY;
			SaveIisLogTableInfoById( stLog.ilg_id, & stLog );

			//	[6]
			//	add default to advanced
			memset( &stAdvanced, 0, sizeof(stAdvanced) );
			_sntprintf( stAdvanced.iis_siteid, sizeof(stAdvanced.iis_siteid)-sizeof(TCHAR), "%s", lpcszSiteId );
			stAdvanced.iav_id		= INSERT_NEW_ITEM_FIRST + 1;
			stAdvanced.iav_wm_show_vwtitle	= 1;
			stAdvanced.iav_wm_model		= IAV_WM_MODEL_NORMAL;
			stAdvanced.iav_wm_delay_time	= DEF_IAV_WM_DELAY_TIME;
			stAdvanced.iav_wm_http_status	= 404;
			_sntprintf( stAdvanced.iav_wm_redirect_url, sizeof(stAdvanced.iav_wm_redirect_url)-sizeof(TCHAR), "%s", DEF_IAV_WM_REDIRECT_URL );
			_sntprintf( stAdvanced.iav_wm_sitename, sizeof(stAdvanced.iav_wm_sitename)-sizeof(TCHAR), "%s", DEF_IAV_WM_SITENAME );
			SaveIisAdvancedTableInfoById( stAdvanced.iav_id, & stAdvanced );
		}
	}

	return bRet;
}



/**
 *	for [iis_table]
 */
BOOL CProcCfg::GetAllIisTableInfo( vector<STIISTABLE> & vcIis )
{
	if ( ! CProcDb::IsConnDatabase() )
		return FALSE;

	BOOL bRet	= FALSE;
	STIISTABLE stIis;

	/*
		LONG	iis_id;
		TCHAR	iis_siteid[32];
		LONG	iis_use;			//	是否被添加到保护队列，只有被添加到保护队列下面的配置才有效。iis_use 辖管着 iis_start
		LONG	iis_start;			//	是否开启: 0为关闭，1为开启
		LONG	iis_enable_protects;		//	是否启用:保护的配置
		LONG	iis_enable_exceptions;		//	是否启用:例外的配置
		LONG	iis_enable_limits;		//	是否启用:限制的配置
		LONG	iis_enable_security;		//	是否启用:安全的配置
		LONG	iis_enable_logs;		//	是否启用:日志的配置
		LONG	iis_enable_vh;			//	是否开启 VirtualHtml 优化
		LONG	iis_type;			//	防盗类型：0为普通防盗，1为超强防盗
		TCHAR	iis_pubvsid_key[MAX_PATH];	//	VSID 信息
		LONG	iis_pubvsid_keytime;		//	VSID 的有效时间
		LONG	iis_pubvsid_usebrowserin;	//	用户必须使用客户端插件
		LONG	iis_pubvsid_usecookie;		//	是否使用 COOKIE 来传递 VSID 信息
	*/

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT TOP 1000 * FROM iis_table" );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		while ( ! m_rs->adoEOF )
		{
			bRet = TRUE;
			memset( &stIis, 0, sizeof(stIis) );

			stIis.iis_id			= QueryIntValue( m_rs, "iis_id" );
			QueryStringValue( m_rs, "iis_siteid", stIis.iis_siteid, sizeof(stIis.iis_siteid) );
			stIis.iis_use			= QueryIntValue( m_rs, "iis_use" );
			stIis.iis_start			= QueryIntValue( m_rs, "iis_start" );
			stIis.iis_start_time_switch	= QueryIntValue( m_rs, "iis_start_time_switch" );

			stIis.iis_enable_protects	= QueryIntValue( m_rs, "iis_enable_protects" );
			stIis.iis_enable_exceptions	= QueryIntValue( m_rs, "iis_enable_exceptions" );
			stIis.iis_enable_limits		= QueryIntValue( m_rs, "iis_enable_limits" );
			stIis.iis_enable_security	= QueryIntValue( m_rs, "iis_enable_security" );
			stIis.iis_enable_logs		= QueryIntValue( m_rs, "iis_enable_logs" );

			stIis.iis_enable_vh		= QueryIntValue( m_rs, "iis_enable_vh" );
			stIis.iis_type			= QueryIntValue( m_rs, "iis_type" );
			QueryStringValue( m_rs, "iis_pubvsid_key", stIis.iis_pubvsid_key, sizeof(stIis.iis_pubvsid_key) );
			stIis.iis_pubvsid_keytime	= QueryIntValue( m_rs, "iis_pubvsid_keytime" );
			stIis.iis_pubvsid_usebrowserin	= QueryIntValue( m_rs, "iis_pubvsid_usebrowserin" );
			stIis.iis_pubvsid_usecookie	= QueryIntValue( m_rs, "iis_pubvsid_usecookie" );
			stIis.iis_pubvsid_type		= QueryIntValue( m_rs, "iis_pubvsid_type" );
			stIis.iis_pubvsid_auto_expired	= QueryIntValue( m_rs, "iis_pubvsid_auto_expired" );
			QueryStringValue( m_rs, "iis_pubvsid_cookiepage", stIis.iis_pubvsid_cookiepage, sizeof(stIis.iis_pubvsid_cookiepage) );
			stIis.iis_pubvsid_cookiepage_len = strlen( stIis.iis_pubvsid_cookiepage );

			QueryStringValue( m_rs, "iis_apppoolid", stIis.iis_apppoolid, sizeof(stIis.iis_apppoolid) );

			//	..
			vcIis.push_back( stIis );

			//	..
			m_rs->MoveNext();
		}
		m_rs->Close();
	}

	return bRet;
}
/*
	for [iis_table]
*/
BOOL CProcCfg::GetIisTableInfoBySiteId( LPCSTR lpcszSiteId, PSTIISTABLE pstSiteInfo )
{
	if ( NULL == lpcszSiteId || 0 == _tcslen(lpcszSiteId) )
	{
		return FALSE;
	}
	if ( NULL == pstSiteInfo )
	{
		return FALSE;
	}
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;

	//
	//	LONG	iis_id;
	//	TCHAR	iis_siteid[32];
	//	LONG	iis_use;			//	是否被添加到保护队列，只有被添加到保护队列下面的配置才有效。iis_use 辖管着 iis_start
	//	LONG	iis_start;			//	是否开启: 0为关闭，1为开启
	//	LONG	iis_enable_protects;		//	是否启用:保护的配置
	//	LONG	iis_enable_exceptions;		//	是否启用:例外的配置
	//	LONG	iis_enable_limits;		//	是否启用:限制的配置
	//	LONG	iis_enable_security;		//	是否启用:安全的配置
	//	LONG	iis_enable_logs;		//	是否启用:日志的配置
	//	LONG	iis_enable_vh;			//	是否开启 VirtualHtml 优化
	//	LONG	iis_type;			//	防盗类型：0为普通防盗，1为超强防盗
	//	TCHAR	iis_pubvsid_key[MAX_PATH];	//	VSID 信息
	//	LONG	iis_pubvsid_keytime;		//	VSID 的有效时间
	//	LONG	iis_pubvsid_usebrowserin;	//	用户必须使用客户端插件
	//	LONG	iis_pubvsid_usecookie;		//	是否使用 COOKIE 来传递 VSID 信息
	//	LONG	iis_pubvsid_type;		//	计算方法：0普通，1随机变化
	//	LONG	iis_pubvsid_use_proxy;			//	服务器被防火墙保护，需要从 HTTP_X_FORWARDED_FOR 节或者用户指定的节中获取远程用户的 IP 地址
	//	TCHAR	iis_pubvsid_proxy_variable[ 64 ];	//	在 HTTP 头中，用户指定的获取远程 IP 地址的节的变量名字
	//

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM iis_table WHERE iis_siteid='%s'", lpcszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		if ( ! m_rs->adoEOF )
		{
			bRet = TRUE;

			pstSiteInfo->iis_id			= QueryIntValue( m_rs, "iis_id" );
			QueryStringValue( m_rs, "iis_siteid", pstSiteInfo->iis_siteid, sizeof(pstSiteInfo->iis_siteid) );
			pstSiteInfo->iis_use			= QueryIntValue( m_rs, "iis_use" );
			pstSiteInfo->iis_start			= QueryIntValue( m_rs, "iis_start" );
			pstSiteInfo->iis_start_time_switch	= QueryIntValue( m_rs, "iis_start_time_switch" );

			pstSiteInfo->iis_enable_protects	= QueryIntValue( m_rs, "iis_enable_protects" );
			pstSiteInfo->iis_enable_exceptions	= QueryIntValue( m_rs, "iis_enable_exceptions" );
			pstSiteInfo->iis_enable_limits		= QueryIntValue( m_rs, "iis_enable_limits" );
			pstSiteInfo->iis_enable_security	= QueryIntValue( m_rs, "iis_enable_security" );
			pstSiteInfo->iis_enable_logs		= QueryIntValue( m_rs, "iis_enable_logs" );

			pstSiteInfo->iis_enable_vh		= QueryIntValue( m_rs, "iis_enable_vh" );
			pstSiteInfo->iis_type			= QueryIntValue( m_rs, "iis_type" );
			QueryStringValue( m_rs, "iis_pubvsid_key", pstSiteInfo->iis_pubvsid_key, sizeof(pstSiteInfo->iis_pubvsid_key) );
			pstSiteInfo->iis_pubvsid_keytime	= QueryIntValue( m_rs, "iis_pubvsid_keytime" );
			pstSiteInfo->iis_pubvsid_usebrowserin	= QueryIntValue( m_rs, "iis_pubvsid_usebrowserin" );
			pstSiteInfo->iis_pubvsid_usecookie	= QueryIntValue( m_rs, "iis_pubvsid_usecookie" );
			pstSiteInfo->iis_pubvsid_type		= QueryIntValue( m_rs, "iis_pubvsid_type" );
			pstSiteInfo->iis_pubvsid_auto_expired	= QueryIntValue( m_rs, "iis_pubvsid_auto_expired" );
			QueryStringValue( m_rs, "iis_pubvsid_cookiepage", pstSiteInfo->iis_pubvsid_cookiepage, sizeof(pstSiteInfo->iis_pubvsid_cookiepage) );
			pstSiteInfo->iis_pubvsid_cookiepage_len = strlen( pstSiteInfo->iis_pubvsid_cookiepage );

			QueryStringValue( m_rs, "iis_apppoolid", pstSiteInfo->iis_apppoolid, sizeof(pstSiteInfo->iis_apppoolid) );
		}
		m_rs->Close();
	}

	return bRet;
}
/*
	for [iis_table]
*/
BOOL CProcCfg::SaveIisTableInfoBySiteId( LPCSTR lpcszSiteId, PSTIISTABLE pstSiteInfo )
{
	//
	//	LONG	iis_id;
	//	TCHAR	iis_siteid[32];
	//	LONG	iis_use;			//	是否被添加到保护队列，只有被添加到保护队列下面的配置才有效。iis_use 辖管着 iis_start
	//	LONG	iis_start;			//	是否开启: 0为关闭，1为开启
	//	LONG	iis_enable_protects;		//	是否启用:保护的配置
	//	LONG	iis_enable_exceptions;		//	是否启用:例外的配置
	//	LONG	iis_enable_limits;		//	是否启用:限制的配置
	//	LONG	iis_enable_security;		//	是否启用:安全的配置
	//	LONG	iis_enable_logs;		//	是否启用:日志的配置
	//	LONG	iis_enable_vh;			//	是否开启 VirtualHtml 优化
	//	LONG	iis_type;			//	防盗类型：0为普通防盗，1为超强防盗
	//	TCHAR	iis_pubvsid_key[MAX_PATH];	//	VSID 信息
	//	LONG	iis_pubvsid_keytime;		//	VSID 的有效时间
	//	LONG	iis_pubvsid_usebrowserin;	//	用户必须使用客户端插件
	//	LONG	iis_pubvsid_usecookie;		//	是否使用 COOKIE 来传递 VSID 信息
	//	LONG	iis_pubvsid_type;		//	计算方法：0普通，1随机变化
	//	LONG	iis_pubvsid_use_proxy;			//	服务器被防火墙保护，需要从 HTTP_X_FORWARDED_FOR 节或者用户指定的节中获取远程用户的 IP 地址
	//	TCHAR	iis_pubvsid_proxy_variable[ 64 ];	//	在 HTTP 头中，用户指定的获取远程 IP 地址的节的变量名字
	//

	if ( NULL == lpcszSiteId || NULL == pstSiteInfo )
		return FALSE;
	if ( 0 == strlen(lpcszSiteId) )
		return FALSE;

	STIISTABLE stIisTable;

	CProcDb::KillBadChar( pstSiteInfo->iis_siteid );
	CProcDb::KillBadChar( pstSiteInfo->iis_pubvsid_key );
	CProcDb::KillBadChar( pstSiteInfo->iis_pubvsid_cookiepage );


	if ( GetIisTableInfoBySiteId( lpcszSiteId, & stIisTable ) )
	{
		//	Exist site, we update it...
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE iis_table SET iis_start=%d, iis_start_time_switch=%d, \
			iis_enable_protects=%d, iis_enable_exceptions=%d, iis_enable_limits=%d, iis_enable_security=%d, iis_enable_logs=%d, \
			iis_enable_vh=%d, \
			iis_type=%d, \
			iis_pubvsid_key='%s', iis_pubvsid_keytime=%d, \
			iis_pubvsid_usebrowserin=%d, iis_pubvsid_usecookie=%d, \
			iis_pubvsid_type=%d, iis_pubvsid_auto_expired=%d, iis_pubvsid_cookiepage='%s', \
			iis_apppoolid='%s' \
			WHERE iis_siteid='%s'",
			pstSiteInfo->iis_start,
			pstSiteInfo->iis_start_time_switch,

			pstSiteInfo->iis_enable_protects,
			pstSiteInfo->iis_enable_exceptions, 
			pstSiteInfo->iis_enable_limits,
			pstSiteInfo->iis_enable_security,
			pstSiteInfo->iis_enable_logs, 
			pstSiteInfo->iis_enable_vh, 
			pstSiteInfo->iis_type,
			
			pstSiteInfo->iis_pubvsid_key, 
			pstSiteInfo->iis_pubvsid_keytime,
			pstSiteInfo->iis_pubvsid_usebrowserin,
			pstSiteInfo->iis_pubvsid_usecookie, 
			pstSiteInfo->iis_pubvsid_type,
			pstSiteInfo->iis_pubvsid_auto_expired,
			pstSiteInfo->iis_pubvsid_cookiepage,

			pstSiteInfo->iis_apppoolid,

			pstSiteInfo->iis_siteid
		);
	}
	else
	{
		//	New site, we insert new ...
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"INSERT INTO iis_table ( iis_siteid, iis_start, iis_start_time_switch, \
			iis_enable_protects, iis_enable_exceptions, iis_enable_limits, iis_enable_security, iis_enable_logs, \
			iis_enable_vh, \
			iis_type, iis_pubvsid_key, iis_pubvsid_keytime, \
			iis_pubvsid_usebrowserin, iis_pubvsid_usecookie, \
			iis_pubvsid_type, iis_pubvsid_auto_expired, iis_pubvsid_cookiepage, \
			iis_apppoolid ) \
			VALUES ( '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, '%s', '%s' )",
			pstSiteInfo->iis_siteid,
			pstSiteInfo->iis_start,
			pstSiteInfo->iis_start_time_switch,

			pstSiteInfo->iis_enable_protects,
			pstSiteInfo->iis_enable_exceptions,
			pstSiteInfo->iis_enable_limits,
			pstSiteInfo->iis_enable_security,
			pstSiteInfo->iis_enable_logs,
			pstSiteInfo->iis_enable_vh,
			pstSiteInfo->iis_type,

			pstSiteInfo->iis_pubvsid_key,
			pstSiteInfo->iis_pubvsid_keytime,
			pstSiteInfo->iis_pubvsid_usebrowserin,
			pstSiteInfo->iis_pubvsid_usecookie,
			pstSiteInfo->iis_pubvsid_type,
			pstSiteInfo->iis_pubvsid_auto_expired,
			pstSiteInfo->iis_pubvsid_cookiepage,

			pstSiteInfo->iis_apppoolid
		);
	}

	//	..
	return CProcDb::ExectueSql( m_szSql );
}
/*
	for [iis_table]
*/
BOOL CProcCfg::UpdateIisTableUseById( LONG iis_id, LONG iis_use )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "UPDATE iis_table SET iis_use=%d WHERE iis_id=%d",  iis_use, iis_id );
	return CProcDb::ExectueSql( m_szSql );
}
/*
	for [iis_table]
*/
BOOL CProcCfg::UpdateIisTableUseBySiteId( LPCSTR lpcszSiteId, LONG iis_use )
{
	if ( NULL == lpcszSiteId || 0 == _tcslen(lpcszSiteId) )
	{
		return FALSE;
	}

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "UPDATE iis_table SET iis_use=%d WHERE iis_siteid='%s'",  iis_use, lpcszSiteId );
	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::UpdateIisTableRootPathBySiteId( LPCSTR lpcszSiteId, LPCSTR lpcszRootPath )
{
	if ( NULL == lpcszSiteId || 0 == _tcslen(lpcszSiteId) )
	{
		return FALSE;
	}
	if ( NULL == lpcszRootPath )
	{
		return FALSE;
	}

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
		"UPDATE iis_table SET iis_root_path='%s' WHERE iis_siteid='%s'",  lpcszRootPath, lpcszSiteId );

	//	..
	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::UpdateIisTableAppPoolIdBySiteId( LPCSTR lpcszSiteId, LPCSTR lpcszAppPoolId )
{
	if ( NULL == lpcszSiteId || 0 == _tcslen(lpcszSiteId) )
	{
		return FALSE;
	}
	if ( NULL == lpcszAppPoolId )
	{
		return FALSE;
	}

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
		"UPDATE iis_table SET iis_apppoolid='%s' WHERE iis_siteid='%s'",  lpcszAppPoolId, lpcszSiteId );

	//	..
	return CProcDb::ExectueSql( m_szSql );
}



/*
	for [iis_table]
	拷贝一条记录覆盖指定 SITEID 的站点
*/
BOOL CProcCfg::CopyGeneralSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo )
{
	/*
		lpcszSiteId		- 被拷贝的站点
		lpszSiteIdCopyTo	- 要覆盖的目标站点
	*/

	if ( NULL == lpszSiteId || NULL == lpszSiteIdCopyTo )
		return FALSE;
	if ( 0 == strlen( lpszSiteId ) || 0 == strlen( lpszSiteIdCopyTo ) )
		return FALSE;

	BOOL bRet	= FALSE;
	STIISTABLE stIisTable;

	if ( GetIisTableInfoBySiteId( lpszSiteId, & stIisTable ) )
	{
		_sntprintf( stIisTable.iis_siteid, sizeof(stIisTable.iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
		bRet = SaveIisTableInfoBySiteId( lpszSiteIdCopyTo, & stIisTable );
	}

	return bRet;
}










//////////////////////////////////////////////////////////////////////////////////////////////
//	[iis_blockext_table]
//////////////////////////////////////////////////////////////////////////////////////////////


/*
	for [iis_blockext_table]
*/
UINT CProcCfg::GetIisBlockExtTableCountBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return 0;

	UINT uRet		= 0;
	_variant_t vIndex;
	if ( ! CProcDb::IsConnDatabase() )
		return FALSE;


	CProcDb::KillBadChar( lpszSiteId );

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT COUNT(*) FROM iis_blockext_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		if ( ! m_rs->adoEOF )
		{
			vIndex = (long)0;
			m_varValue = m_rs->GetCollect( vIndex );
			uRet = long( m_varValue );
		}
		m_rs->Close();
	}

	return uRet;
}

/*
	for [iis_blockext_table]
*/
BOOL CProcCfg::GetAllIisBlockExtTableInfoBySiteIdEx( LPSTR lpszSiteId, vector<STIISBLOCKEXTTABLE> & vcBlockExts )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
	{
		return FALSE;
	}
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	STIISBLOCKEXTTABLE stBlockExts;

	//
	//	LONG	ibe_id;
	//	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	//	LONG	ibe_use;			//	是否使用：0为不使用，1为使用
	//	TCHAR	ibe_name[ 32 ];			//	扩展名
	//	LONG	ibe_config;			//	是否开启：1-Deny all HTTP requests, 2-anti-leechs, 4-limit playonly
	//	LONG	ibe_lmt_thread;			//	限制线程
	//	LONG	ibe_lmt_speed;			//	限制速度		
	//	LONG	ibe_action;			//	处理动作：1-Send warning messages表示发送警告信息, 2-表示使用文件替换
	//	TCHAR	ibe_lmt_player[8];		//	该扩展名限制使用哪个播放器播放。MP 表示 Window Media Player；RP 表示 Real Player。
	//	TCHAR	ibe_replacefile[ MAX_PATH ];	//	该扩展名的警告替换文件路径	
	//

	CProcDb::KillBadChar( lpszSiteId );

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM iis_blockext_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		bRet = TRUE;

		while ( ! m_rs->adoEOF )
		{
			memset( & stBlockExts, 0, sizeof(stBlockExts) );
			if ( QueryValueForIisBlockExtTable( m_rs, & stBlockExts ) )
			{
				vcBlockExts.push_back( stBlockExts );
			}
			m_rs->MoveNext();
		}
		m_rs->Close();
	}

	return bRet;
}

/*
	for [iis_blockext_table]
*/
BOOL CProcCfg::DeleteAllIisBlockExtTableInfoBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar(lpszSiteId);
	

	if ( strlen(lpszSiteId) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"DELETE FROM iis_blockext_table WHERE iis_siteid='%s'", lpszSiteId );
		
		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	return bRet;
}

/*
	for [iis_blockext_table]
*/
BOOL CProcCfg::DeleteIisBlockExtTableInfoById( LONG ibe_id )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
		"DELETE FROM iis_blockext_table WHERE ibe_id=%d", ibe_id );
	// ..
	return CProcDb::ExectueSql( m_szSql );
}


BOOL CProcCfg::QueryValueForIisBlockExtTable( _RecordsetPtr rs, STIISBLOCKEXTTABLE * pstIisBlockExtTable )
{
	BOOL bRet	= FALSE;

	if ( ! rs->adoEOF && pstIisBlockExtTable )
	{
		bRet = TRUE;

		pstIisBlockExtTable->ibe_id = QueryIntValue( rs, "ibe_id" );
		QueryStringValue( rs, "iis_siteid", pstIisBlockExtTable->iis_siteid, sizeof(pstIisBlockExtTable->iis_siteid) );
		pstIisBlockExtTable->ibe_use = QueryIntValue( rs, "ibe_use" );
		QueryStringValue( rs, "ibe_name", pstIisBlockExtTable->ibe_name, sizeof(pstIisBlockExtTable->ibe_name) );
		pstIisBlockExtTable->ibe_config = QueryIntValue( rs, "ibe_config" );
		pstIisBlockExtTable->ibe_lmt_thread = QueryIntValue( rs, "ibe_lmt_thread" );
		pstIisBlockExtTable->ibe_lmt_speed = QueryIntValue( rs, "ibe_lmt_speed" );
		pstIisBlockExtTable->ibe_action = QueryIntValue( rs, "ibe_action" );
		QueryStringValue( rs, "ibe_lmt_player", pstIisBlockExtTable->ibe_lmt_player, sizeof(pstIisBlockExtTable->ibe_lmt_player) );
		QueryStringValue( rs, "ibe_replacefile", pstIisBlockExtTable->ibe_replacefile, sizeof(pstIisBlockExtTable->ibe_replacefile) );
	}

	return bRet;
}

/*
	for [iis_blockext_table]
*/
BOOL CProcCfg::GetIisBlockExtTableInfoById( LONG ibe_id, STIISBLOCKEXTTABLE * pstIisBlockExtTable )
{
	if ( NULL == pstIisBlockExtTable )
		return FALSE;
	if ( ! CProcDb::IsConnDatabase() )
		return FALSE;

	BOOL bRet	= FALSE;

	/*
		LONG	ibe_id;
		TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
		LONG	ibe_use;			//	是否使用：0为不使用，1为使用
		TCHAR	ibe_name[ 32 ];			//	扩展名
		LONG	ibe_config;			//	是否开启：1-Deny all HTTP requests, 2-anti-leechs, 4-limit playonly
		LONG	ibe_action;			//	处理动作：1-Send warning messages表示发送警告信息, 2-表示使用文件替换
		TCHAR	ibe_lmt_player[8];		//	该扩展名限制使用哪个播放器播放。MP 表示 Window Media Player；RP 表示 Real Player。
		TCHAR	ibe_replacefile[ MAX_PATH ];	//	该扩展名的警告替换文件路径	
	*/

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM iis_blockext_table WHERE ibe_id=%d", ibe_id );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		if ( ! m_rs->adoEOF )
		{
			bRet = TRUE;
			QueryValueForIisBlockExtTable( m_rs, pstIisBlockExtTable );
		}
		m_rs->Close();
	}

	return bRet;
}
/*
	for [iis_blockext_table]
*/
BOOL CProcCfg::SaveIisBlockExtTableInfoById( LONG ibe_id, STIISBLOCKEXTTABLE * pstIisBlockExtTable )
{
	/* //////////////////////////////////////////////////
		UINT m_id		- m_id
		LPCSTR lpcszField	- 需要更新的字段名
		LPCSTR lpcszValue	- 对应字段的值
	*/

	if ( NULL == pstIisBlockExtTable )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar( pstIisBlockExtTable->ibe_name );
	CProcDb::KillBadChar( pstIisBlockExtTable->ibe_lmt_player );
	CProcDb::KillBadChar( pstIisBlockExtTable->ibe_replacefile );


	if ( ibe_id >= INSERT_NEW_ITEM_FIRST )
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"INSERT INTO iis_blockext_table ( \
			iis_siteid, ibe_name, ibe_use, ibe_config, ibe_lmt_thread, ibe_lmt_speed, ibe_action, ibe_lmt_player, ibe_replacefile ) \
			VALUES ( '%s', '%s', %d, %d, %d, %d, %d, '%s', '%s' )",
			pstIisBlockExtTable->iis_siteid,
			pstIisBlockExtTable->ibe_name,
			pstIisBlockExtTable->ibe_use,
			pstIisBlockExtTable->ibe_config,
			pstIisBlockExtTable->ibe_lmt_thread,
			pstIisBlockExtTable->ibe_lmt_speed,
			pstIisBlockExtTable->ibe_action,
			pstIisBlockExtTable->ibe_lmt_player,
			pstIisBlockExtTable->ibe_replacefile
		);
	}
	else
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE iis_blockext_table SET \
			ibe_name='%s', ibe_use=%d, ibe_config=%d, ibe_lmt_thread=%d, ibe_lmt_speed=%d, ibe_action=%d, ibe_lmt_player='%s', ibe_replacefile='%s' \
			WHERE ibe_id=%d",
			pstIisBlockExtTable->ibe_name,
			pstIisBlockExtTable->ibe_use,
			pstIisBlockExtTable->ibe_config,
			pstIisBlockExtTable->ibe_lmt_thread,
			pstIisBlockExtTable->ibe_lmt_speed,
			pstIisBlockExtTable->ibe_action,
			pstIisBlockExtTable->ibe_lmt_player,
			pstIisBlockExtTable->ibe_replacefile,
			ibe_id
		);
	}

	bRet = CProcDb::ExectueSql( m_szSql );

	// ..
	return bRet;
}


/*
	for [iis_blockdirs_table]
*/
BOOL CProcCfg::GetAllIisBlockDirTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISBLOCKDIRSTABLE> & vcBlockDirs )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
	{
		return FALSE;
	}
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	STIISBLOCKDIRSTABLE stBlockDirs;

	//
	//	LONG	ibd_id;
	//	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	//	LONG	ibd_use;			//	是否使用：0为不使用，1为使用
	//	TCHAR	ibd_name[ 64 ];			//	目录名称	
	//	LONG	ibd_config;			//	是否开启：1-limit thread, 2-limit speed
	//	LONG	ibd_lmt_thread;			//	限制线程
	//	LONG	ibd_lmt_speed;			//	限制速度		
	//

	CProcDb::KillBadChar( lpszSiteId );

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM iis_blockdirs_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		bRet = TRUE;

		while ( ! m_rs->adoEOF )
		{
			memset( & stBlockDirs, 0, sizeof(stBlockDirs) );

			stBlockDirs.ibd_id = QueryIntValue( m_rs, "ibd_id" );
			QueryStringValue( m_rs, "iis_siteid", stBlockDirs.iis_siteid, sizeof(stBlockDirs.iis_siteid) );
			stBlockDirs.ibd_use = QueryIntValue( m_rs, "ibd_use" );
			QueryStringValue( m_rs, "ibd_name", stBlockDirs.ibd_name, sizeof(stBlockDirs.ibd_name) );
			stBlockDirs.ibd_config = QueryIntValue( m_rs, "ibd_config" );
			stBlockDirs.ibd_lmt_thread = QueryIntValue( m_rs, "ibd_lmt_thread" );
			stBlockDirs.ibd_lmt_speed = QueryIntValue( m_rs, "ibd_lmt_speed" );

			vcBlockDirs.push_back( stBlockDirs );

			m_rs->MoveNext();
		}
		m_rs->Close();
	}

	return bRet;
}
/*
	for [iis_blockdirs_table]
*/
BOOL CProcCfg::SaveIisBlockDirTableInfoById( LONG ibd_id, STIISBLOCKDIRSTABLE * pstIisBlockDirTable )
{
	if ( NULL == pstIisBlockDirTable )
		return FALSE;

	BOOL bRet	= FALSE;


	CProcDb::KillBadChar( pstIisBlockDirTable->ibd_name );
	CProcDb::KillBadChar( pstIisBlockDirTable->iis_siteid );

	if ( ibd_id >= INSERT_NEW_ITEM_FIRST )
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"INSERT INTO iis_blockdirs_table ( iis_siteid, ibd_use, ibd_name, ibd_config, ibd_lmt_thread, ibd_lmt_speed ) VALUES ( '%s', %d, '%s', %d, %d, %d )",
			pstIisBlockDirTable->iis_siteid,
			pstIisBlockDirTable->ibd_use,
			pstIisBlockDirTable->ibd_name,
			pstIisBlockDirTable->ibd_config,
			pstIisBlockDirTable->ibd_lmt_thread,
			pstIisBlockDirTable->ibd_lmt_speed
		);
	}
	else
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE iis_blockdirs_table SET ibd_use=%d, ibd_name='%s', ibd_config=%d, ibd_lmt_thread=%d, ibd_lmt_speed=%d WHERE ibd_id=%d",
			pstIisBlockDirTable->ibd_use,
			pstIisBlockDirTable->ibd_name,
			pstIisBlockDirTable->ibd_config,
			pstIisBlockDirTable->ibd_lmt_thread,
			pstIisBlockDirTable->ibd_lmt_speed,
			ibd_id
		);
	}

	bRet = CProcDb::ExectueSql( m_szSql );

	// ..
	return bRet;
}
/*
	for [iis_blockdirs_table]
*/
BOOL CProcCfg::DeleteIisBlockDirTableInfoById( LONG ibd_id )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "DELETE FROM iis_blockdirs_table WHERE ibd_id=%d", ibd_id );
	return CProcDb::ExectueSql( m_szSql );
}
/*
	for [iis_blockdirs_table]
*/
BOOL CProcCfg::DeleteAllIisBlockDirTableInfoBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar(lpszSiteId);
	

	if ( strlen(lpszSiteId) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"DELETE FROM iis_blockdirs_table WHERE iis_siteid='%s'", lpszSiteId );

		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	return bRet;
}



/*
	for [iis_blockext_table]
	拷贝一条记录覆盖指定 SITEID 的站点
*/
BOOL CProcCfg::CopyProtectedSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo )
{
	//
	//	lpcszSiteId		- 被拷贝的站点
	//	lpszSiteIdCopyTo	- 要覆盖的目标站点
	//
	if ( NULL == lpszSiteId || NULL == lpszSiteIdCopyTo )
	{
		return FALSE;
	}
	if ( 0 == strlen( lpszSiteId ) || 0 == strlen( lpszSiteIdCopyTo ) )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	vector<STIISBLOCKEXTTABLE> vcBlockExts;
	vector<STIISBLOCKEXTTABLE>::iterator itbext;


	//	拷贝扩展名
	vcBlockExts.clear();
	if ( GetAllIisBlockExtTableInfoBySiteIdEx( lpszSiteId, vcBlockExts ) )
	{
		//	先删除目标的所有项目
		DeleteAllIisBlockExtTableInfoBySiteId( lpszSiteIdCopyTo );

		if ( vcBlockExts.size() )
		{
			//	拷贝
			for ( itbext = vcBlockExts.begin(); itbext != vcBlockExts.end(); itbext ++ )
			{
				_sntprintf( (*itbext).iis_siteid, sizeof((*itbext).iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
				(*itbext).ibe_id = INSERT_NEW_ITEM_FIRST + 1;
				if ( SaveIisBlockExtTableInfoById( (*itbext).ibe_id, itbext ) )
				{
					bRet = TRUE;
				}
			}
		}
	}

	vector<STIISBLOCKDIRSTABLE> vcBlockDirs;
	vector<STIISBLOCKDIRSTABLE>::iterator itbdir;

	//	拷贝保护目录
	vcBlockDirs.clear();
	if ( GetAllIisBlockDirTableInfoBySiteId( lpszSiteId, vcBlockDirs ) )
	{
		//	先删除目标的所有项目
		DeleteAllIisBlockDirTableInfoBySiteId( lpszSiteIdCopyTo );

		if ( vcBlockDirs.size() )
		{
			//	拷贝之
			for ( itbdir = vcBlockDirs.begin(); itbdir != vcBlockDirs.end(); itbdir ++ )
			{
				_sntprintf( (*itbdir).iis_siteid, sizeof((*itbdir).iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
				(*itbdir).ibd_id = INSERT_NEW_ITEM_FIRST + 1;
				if ( SaveIisBlockDirTableInfoById( (*itbdir).ibd_id, itbdir ) )
				{
					bRet = TRUE;
				}
			}
		}
	}

	//	..
	return bRet;
}










/*
	for [iis_allowdirs_table]
*/
BOOL CProcCfg::GetAllIisAllowDirTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISALLOWDIRSTABLE> & vcAllowDirs )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
	{
		return FALSE;
	}
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	STIISALLOWDIRSTABLE stAllowDirs;

	//
	//	LONG	iad_id;
	//	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	//	LONG	iad_use;			//	是否使用：0为不使用，1为使用
	//	TCHAR	iad_name[ 64 ];			//	目录名称	
	//

	CProcDb::KillBadChar( lpszSiteId );

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM iis_allowdirs_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		bRet = TRUE;

		while ( ! m_rs->adoEOF )
		{
			memset( & stAllowDirs, 0, sizeof(stAllowDirs) );

			stAllowDirs.iad_id		= QueryIntValue( m_rs, "iad_id" );
			QueryStringValue( m_rs, "iis_siteid", stAllowDirs.iis_siteid, sizeof(stAllowDirs.iis_siteid) );
			stAllowDirs.iad_use		= QueryIntValue( m_rs, "iad_use" );
			stAllowDirs.iad_except_config	= QueryIntValue( m_rs, "iad_except_config" );
			QueryStringValue( m_rs, "iad_name", stAllowDirs.iad_name, sizeof(stAllowDirs.iad_name) );	

			vcAllowDirs.push_back( stAllowDirs );
			m_rs->MoveNext();
		}
		m_rs->Close();
	}

	return bRet;
}
BOOL CProcCfg::SaveIisAllowDirTableInfoById( LONG iad_id, STIISALLOWDIRSTABLE * pstIisAllowDirTable )
{
	if ( NULL == pstIisAllowDirTable )
		return FALSE;

	CProcDb::KillBadChar( pstIisAllowDirTable->iad_name );
	CProcDb::KillBadChar( pstIisAllowDirTable->iis_siteid );

	if ( iad_id >= INSERT_NEW_ITEM_FIRST )
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"INSERT INTO iis_allowdirs_table ( iis_siteid, iad_name, iad_use, iad_except_config ) VALUES ( '%s', '%s', %d, %d )",
			pstIisAllowDirTable->iis_siteid,
			pstIisAllowDirTable->iad_name,
			pstIisAllowDirTable->iad_use,
			pstIisAllowDirTable->iad_except_config
		);
	}
	else
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE iis_allowdirs_table SET iad_name='%s', iad_use=%d, iad_except_config=%d WHERE iad_id=%d",
			pstIisAllowDirTable->iad_name,
			pstIisAllowDirTable->iad_use,
			pstIisAllowDirTable->iad_except_config,
			iad_id
		);
	}

	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::DeleteIisAllowDirTableInfoById( LONG iad_id )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
		"DELETE FROM iis_allowdirs_table WHERE iad_id=%d", iad_id );

	// ..
	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::DeleteAllIisAllowDirTableInfoBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar(lpszSiteId);

	if ( strlen(lpszSiteId) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"DELETE FROM iis_allowdirs_table WHERE iis_siteid='%s'", lpszSiteId );
		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


/*
	for [iis_friendhosts_table]
*/
BOOL CProcCfg::GetAllIisFriendHostsTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISFRIENDHOSTSTABLE> & vcFriendHosts )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
	{
		return FALSE;
	}
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	STIISFRIENDHOSTSTABLE stFriendHosts;

	/*
		LONG	ifh_id;
		TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
		LONG	ifh_use;			//	是否使用：0为不使用，1为使用
		LONG	ifh_type;			//	类型：0表示普通站点，1表示搜索引擎的蜘蛛
		TCHAR	ifh_name[ 255 ];		//	域名或者搜索引擎的蜘蛛的索引
	*/

	CProcDb::KillBadChar( lpszSiteId );

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM iis_friendhosts_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		bRet = TRUE;

		while ( ! m_rs->adoEOF )
		{
			memset( & stFriendHosts, 0, sizeof(stFriendHosts) );

			stFriendHosts.ifh_id		= QueryIntValue( m_rs, "ifh_id" );
			QueryStringValue( m_rs, "iis_siteid", stFriendHosts.iis_siteid, sizeof(stFriendHosts.iis_siteid) );
			stFriendHosts.ifh_use		= QueryIntValue( m_rs, "ifh_use" );
			stFriendHosts.ifh_type		= QueryIntValue( m_rs, "ifh_type" );
			stFriendHosts.ifh_except_config	= QueryIntValue( m_rs, "ifh_except_config" );
			QueryStringValue( m_rs, "ifh_name", stFriendHosts.ifh_name, sizeof(stFriendHosts.ifh_name) );	

			vcFriendHosts.push_back( stFriendHosts );

			m_rs->MoveNext();
		}

		m_rs->Close();
	}

	return bRet;
}
BOOL CProcCfg::SaveIisFriendHostsTableInfoById( LONG ifh_id, STIISFRIENDHOSTSTABLE * pstIisFriendHostsTable )
{
	if ( NULL == pstIisFriendHostsTable )
		return FALSE;


	CProcDb::KillBadChar( pstIisFriendHostsTable->ifh_name );
	CProcDb::KillBadChar( pstIisFriendHostsTable->iis_siteid );

	if ( ifh_id >= INSERT_NEW_ITEM_FIRST )
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"INSERT INTO iis_friendhosts_table ( iis_siteid, ifh_type, ifh_except_config, ifh_name, ifh_use ) VALUES ( '%s', %d, %d, '%s', %d )",
			pstIisFriendHostsTable->iis_siteid,
			pstIisFriendHostsTable->ifh_type,
			pstIisFriendHostsTable->ifh_except_config,
			pstIisFriendHostsTable->ifh_name,
			pstIisFriendHostsTable->ifh_use
		);
	}
	else
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE iis_friendhosts_table SET ifh_type=%d, ifh_except_config=%d, ifh_name='%s', ifh_use=%d WHERE ifh_id=%d",
			pstIisFriendHostsTable->ifh_type,
			pstIisFriendHostsTable->ifh_except_config,
			pstIisFriendHostsTable->ifh_name,
			pstIisFriendHostsTable->ifh_use,
			ifh_id
		);
	}

	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::DeleteIisFriendHostsTableInfoById( LONG ifh_id )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
		"DELETE FROM iis_friendhosts_table WHERE ifh_id=%d", ifh_id );

	// ..
	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::DeleteAllIisFriendHostsTableInfoBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar(lpszSiteId);

	if ( strlen(lpszSiteId) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"DELETE FROM iis_friendhosts_table WHERE iis_siteid='%s'", lpszSiteId );
		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	return bRet;
}


/*
	for [iis_allowdirs_table] [iis_friendhosts_table]
	拷贝一条记录覆盖指定 SITEID 的站点
*/
BOOL CProcCfg::CopyExceptionsSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo )
{
	//
	//	lpcszSiteId		- 被拷贝的站点
	//	lpszSiteIdCopyTo	- 要覆盖的目标站点
	//

	if ( NULL == lpszSiteId || NULL == lpszSiteIdCopyTo )
	{
		return FALSE;
	}
	if ( 0 == strlen( lpszSiteId ) || 0 == strlen( lpszSiteIdCopyTo ) )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	vector<STIISALLOWDIRSTABLE> vcAllowDirs;
	vector<STIISALLOWDIRSTABLE>::iterator itad;


	//	拷贝允许的目录
	vcAllowDirs.clear();
	if ( GetAllIisAllowDirTableInfoBySiteId( lpszSiteId, vcAllowDirs ) )
	{
		//	先删除目标的所有项目
		DeleteAllIisAllowDirTableInfoBySiteId( lpszSiteIdCopyTo );

		if ( vcAllowDirs.size() )
		{
			//	拷贝
			for ( itad = vcAllowDirs.begin(); itad != vcAllowDirs.end(); itad ++ )
			{
				_sntprintf( (*itad).iis_siteid, sizeof((*itad).iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
				(*itad).iad_id = INSERT_NEW_ITEM_FIRST + 1;
				if ( SaveIisAllowDirTableInfoById( (*itad).iad_id, itad ) )
				{
					bRet = TRUE;
				}
			}
		}
	}


	vector<STIISFRIENDHOSTSTABLE> vcFriendHosts;
	vector<STIISFRIENDHOSTSTABLE>::iterator itfh;

	//	拷贝友情站点
	vcFriendHosts.clear();
	if ( GetAllIisFriendHostsTableInfoBySiteId( lpszSiteId, vcFriendHosts ) )
	{
		//	先删除目标的所有项目
		DeleteAllIisFriendHostsTableInfoBySiteId( lpszSiteIdCopyTo );
		
		if ( vcFriendHosts.size() )
		{
			//	拷贝之
			for ( itfh = vcFriendHosts.begin(); itfh != vcFriendHosts.end(); itfh ++ )
			{
				_sntprintf( (*itfh).iis_siteid, sizeof((*itfh).iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
				(*itfh).ifh_id = INSERT_NEW_ITEM_FIRST + 1;
				if ( SaveIisFriendHostsTableInfoById( (*itfh).ifh_id, itfh ) )
				{
					bRet = TRUE;
				}
			}
		}
	}

	//	..
	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


/**
 *	@ public
 *	[iis_blockhosts_table]
 */
BOOL CProcCfg::GetAllIisBlockHostsTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISBLOCKHOSTSTABLE> & vcBlockHosts )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
	{
		return FALSE;
	}
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	STIISBLOCKHOSTSTABLE stBlockHosts;

	//
	//	LONG	ibh_id;
	//	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	//	LONG	ibh_use;			//	是否使用：0为不使用，1为使用
	//	LONG	ibh_type;			//	类型：0表示普通站点，1表示搜索引擎的蜘蛛
	//	TCHAR	ibh_name[ 260 ];		//	URL
	//

	CProcDb::KillBadChar( lpszSiteId );

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM iis_blockhosts_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		bRet = TRUE;

		while ( ! m_rs->adoEOF )
		{
			memset( & stBlockHosts, 0, sizeof(stBlockHosts) );

			stBlockHosts.ibh_id = QueryIntValue( m_rs, "ibh_id" );
			QueryStringValue( m_rs, "iis_siteid", stBlockHosts.iis_siteid, sizeof(stBlockHosts.iis_siteid) );
			stBlockHosts.ibh_use = QueryIntValue( m_rs, "ibh_use" );
			stBlockHosts.ibh_type = QueryIntValue( m_rs, "ibh_type" );
			QueryStringValue( m_rs, "ibh_name", stBlockHosts.ibh_name, sizeof(stBlockHosts.ibh_name) );	

			vcBlockHosts.push_back( stBlockHosts );

			m_rs->MoveNext();
		}

		m_rs->Close();
	}

	return bRet;
}
BOOL CProcCfg::SaveIisBlockHostsTableInfoById( LONG ibh_id, STIISBLOCKHOSTSTABLE * pstIisBlockHostsTable )
{
	if ( NULL == pstIisBlockHostsTable )
	{	
		return FALSE;
	}
	
	CProcDb::KillBadChar( pstIisBlockHostsTable->ibh_name );
	CProcDb::KillBadChar( pstIisBlockHostsTable->iis_siteid );
	
	if ( ibh_id >= INSERT_NEW_ITEM_FIRST )
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"INSERT INTO iis_blockhosts_table ( iis_siteid, ibh_type, ibh_name, ibh_use ) VALUES ( '%s', %d, '%s', %d )",
			pstIisBlockHostsTable->iis_siteid,
			pstIisBlockHostsTable->ibh_type,
			pstIisBlockHostsTable->ibh_name,
			pstIisBlockHostsTable->ibh_use
		);
	}
	else
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE iis_blockhosts_table SET ibh_type=%d, ibh_name='%s', ibh_use=%d WHERE ibh_id=%d",
			pstIisBlockHostsTable->ibh_type,
			pstIisBlockHostsTable->ibh_name,
			pstIisBlockHostsTable->ibh_use,
			ibh_id
		);
	}

	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::DeleteIisBlockHostsTableInfoById( LONG ibh_id )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
		"DELETE FROM iis_blockhosts_table WHERE ibh_id=%d", ibh_id );

	// ..
	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::DeleteAllIisBlockHostsTableInfoBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
	{	
		return FALSE;
	}	

	BOOL bRet	= FALSE;
	
	CProcDb::KillBadChar(lpszSiteId);

	if ( strlen(lpszSiteId) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"DELETE FROM iis_blockhosts_table WHERE iis_siteid='%s'", lpszSiteId );
		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	return bRet;
}


/**
 *	@ public
 *	for [iis_blockip_table]
 */
BOOL CProcCfg::GetAllIisBlockIPTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISBLOCKIPTABLE> & vcBlockIP )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
	{
		return FALSE;
	}
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	STIISBLOCKIPTABLE stBlockIP;
	
	//
	//	LONG	ibip_id;
	//	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	//	LONG	ibip_use;			//	是否使用：0为不使用，1为使用
	//	LONG	ibip_type;			//	类型：0表示普通站点，1表示搜索引擎的蜘蛛
	//	TCHAR	ibip_name[ 32 ];		//	IP 地址
	//

	CProcDb::KillBadChar( lpszSiteId );
	
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM iis_blockip_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		bRet = TRUE;
		
		while ( ! m_rs->adoEOF )
		{
			memset( & stBlockIP, 0, sizeof(stBlockIP) );
			
			stBlockIP.ibip_id = QueryIntValue( m_rs, "ibip_id" );
			QueryStringValue( m_rs, "iis_siteid", stBlockIP.iis_siteid, sizeof(stBlockIP.iis_siteid) );
			stBlockIP.ibip_use = QueryIntValue( m_rs, "ibip_use" );
			stBlockIP.ibip_type = QueryIntValue( m_rs, "ibip_type" );
			QueryStringValue( m_rs, "ibip_name", stBlockIP.ibip_name, sizeof(stBlockIP.ibip_name) );	
			
			vcBlockIP.push_back( stBlockIP );
			
			m_rs->MoveNext();
		}
		
		m_rs->Close();
	}
	
	return bRet;
}
BOOL CProcCfg::SaveIisBlockIPTableInfoById( LONG ibip_id, STIISBLOCKIPTABLE * pstIisBlockIPTable )
{
	if ( NULL == pstIisBlockIPTable )
	{	
		return FALSE;
	}
	
	CProcDb::KillBadChar( pstIisBlockIPTable->ibip_name );
	CProcDb::KillBadChar( pstIisBlockIPTable->iis_siteid );
	
	if ( ibip_id >= INSERT_NEW_ITEM_FIRST )
	{
		_sntprintf
			(
				m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
				"INSERT INTO iis_blockip_table ( iis_siteid, ibip_type, ibip_name, ibip_use ) VALUES ( '%s', %d, '%s', %d )",
				pstIisBlockIPTable->iis_siteid,
				pstIisBlockIPTable->ibip_type,
				pstIisBlockIPTable->ibip_name,
				pstIisBlockIPTable->ibip_use
			);
	}
	else
	{
		_sntprintf
			(
				m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
				"UPDATE iis_blockip_table SET ibip_type=%d, ibip_name='%s', ibip_use=%d WHERE ibip_id=%d",
				pstIisBlockIPTable->ibip_type,
				pstIisBlockIPTable->ibip_name,
				pstIisBlockIPTable->ibip_use,
				ibip_id
			);
	}

	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::DeleteIisBlockIPTableInfoById( LONG ibip_id )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
		"DELETE FROM iis_blockip_table WHERE ibip_id=%d", ibip_id );
	
	// ..
	return CProcDb::ExectueSql( m_szSql );
}
BOOL CProcCfg::DeleteAllIisBlockIPTableInfoBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
	{	
		return FALSE;
	}

	BOOL bRet	= FALSE;
	
	CProcDb::KillBadChar(lpszSiteId);
	
	if ( strlen(lpszSiteId) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"DELETE FROM iis_blockip_table WHERE iis_siteid='%s'", lpszSiteId );
		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	return bRet;
}

BOOL CProcCfg::CopyLimitSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo )
{
	//
	//	lpcszSiteId		- 被拷贝的站点
	//	lpszSiteIdCopyTo	- 要覆盖的目标站点
	//
	
	if ( NULL == lpszSiteId || NULL == lpszSiteIdCopyTo )
	{
		return FALSE;
	}
	if ( 0 == strlen( lpszSiteId ) || 0 == strlen( lpszSiteIdCopyTo ) )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	vector<STIISBLOCKHOSTSTABLE> vcBlockHosts;
	vector<STIISBLOCKHOSTSTABLE>::iterator itbh;
	

	//	拷贝限制的站点
	vcBlockHosts.clear();
	if ( GetAllIisBlockHostsTableInfoBySiteId( lpszSiteId, vcBlockHosts ) )
	{
		//	先删除目标的所有项目
		DeleteAllIisBlockHostsTableInfoBySiteId( lpszSiteIdCopyTo );
		
		if ( vcBlockHosts.size() )
		{
			//	拷贝
			for ( itbh = vcBlockHosts.begin(); itbh != vcBlockHosts.end(); itbh ++ )
			{
				_sntprintf( (*itbh).iis_siteid, sizeof((*itbh).iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
				(*itbh).ibh_id = INSERT_NEW_ITEM_FIRST + 1;
				if ( SaveIisBlockHostsTableInfoById( (*itbh).ibh_id, itbh ) )
				{
					bRet = TRUE;
				}
			}
		}
	}

	
	vector<STIISBLOCKIPTABLE> vcBlockIP;
	vector<STIISBLOCKIPTABLE>::iterator itbip;

	//	拷贝限制的 IP 地址
	vcBlockIP.clear();
	if ( GetAllIisBlockIPTableInfoBySiteId( lpszSiteId, vcBlockIP ) )
	{
		//	先删除目标的所有项目
		DeleteAllIisBlockIPTableInfoBySiteId( lpszSiteIdCopyTo );

		if ( vcBlockIP.size() )
		{
			//	拷贝之
			for ( itbip = vcBlockIP.begin(); itbip != vcBlockIP.end(); itbip ++ )
			{
				_sntprintf( (*itbip).iis_siteid, sizeof((*itbip).iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
				(*itbip).ibip_id = INSERT_NEW_ITEM_FIRST + 1;
				if ( SaveIisBlockIPTableInfoById( (*itbip).ibip_id, itbip ) )
				{
					bRet = TRUE;
				}
			}
		}
	}

	//	..
	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



/*
	for [iis_security_table]
*/
BOOL CProcCfg::GetIisSecurityTableInfoBySiteId( LPSTR lpszSiteId, STIISSECURITYTABLE * pstIisSecurity )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return FALSE;
	if ( ! CProcDb::IsConnDatabase() )
		return FALSE;

	BOOL bRet	= FALSE;

	/*
		LONG	isc_id;
		TCHAR	iis_siteid[32];				//	关联 iis_table 的字段
		LONG	isc_enable_iis_disguise;		//	是否开启 IIS 伪装功能
		LONG	isc_iis_disguise_cfg;			//	IIS 伪装的配置
		LONG	isc_enable_antatt;			//	是否开启防 CC 攻击功能
		LONG	isc_antatt_maxvisit;			//	防 CC 攻击：一分钟多少个请求就拒绝请求
		LONG	isc_antatt_sleep;			//	防 CC 攻击：遇到攻击停歇多久，单位分钟
		LONG	isc_antatt_disabled_proxy;		//	防 CC 攻击：proxy 禁止功能
		//	防 CC 攻击：白名单
		//	防 CC 攻击：黑名单
		LONG	isc_enable_url_filter;			//	是否开启 URL 过滤
		LONG	isc_enable_content_filter;		//	是否开始内容过滤
		LONG	isc_enable_lmt_urllen;			//	是否限制 URL 的长度
		LONG	isc_lmt_urllen_cfg;			//	限制 URL 长度的数值（单位是字符数字）
		LONG	isc_enable_lmt_urlchars;		//	是否限制 URL 中不允许有特殊字符出现
		TCHAR	isc_lmt_urlchars_cfg[ MAX_PATH ];	//	不允许出现的特殊字符配置
		LONG	isc_enable_lmt_contentlen;		//	是否开启限制内容长度
		LONG	isc_lmt_contentlen_sendbyserver;	//	限制服务器发送出的大小
		LONG	isc_lmt_contentlen_postbyclient;	//	限制从客户端 POST 给服务器的大小
		LONG    isc_lmt_dayip;				//	日 IP 下载次数限制
		#ifdef RUN_ENV_SERVICE
		TCHAR	szUrlCharsCfg[32][MAX_SINGLEURLCHARSLEN];	//	isc_lmt_urlchars_cfg 的数组展开
		DWORD	dwUrlCharsCfgCount;				//	UrlCharsCfg 有效个数
		#endif
	*/

	CProcDb::KillBadChar( lpszSiteId );

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT TOP 1 * FROM iis_security_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		if ( ! m_rs->adoEOF )
		{
			pstIisSecurity->isc_id	= CProcDb::QueryIntValue( m_rs, "isc_id" );
			CProcDb::QueryStringValue( m_rs, "iis_siteid", pstIisSecurity->iis_siteid, sizeof(pstIisSecurity->iis_siteid) );
			pstIisSecurity->isc_enable_iis_disguise		= CProcDb::QueryIntValue( m_rs, "isc_enable_iis_disguise" );
			pstIisSecurity->isc_iis_disguise_cfg		= CProcDb::QueryIntValue( m_rs, "isc_iis_disguise_cfg" );

			//	防攻击配置信息
			pstIisSecurity->isc_enable_antatt		= CProcDb::QueryIntValue( m_rs, "isc_enable_antatt" );
			pstIisSecurity->isc_antatt_maxvisit		= CProcDb::QueryIntValue( m_rs, "isc_antatt_maxvisit" );
			pstIisSecurity->isc_antatt_sleep		= CProcDb::QueryIntValue( m_rs, "isc_antatt_sleep" );
			pstIisSecurity->isc_antatt_disabled_proxy	= CProcDb::QueryIntValue( m_rs, "isc_antatt_disabled_proxy" );

			pstIisSecurity->isc_enable_url_filter		= CProcDb::QueryIntValue( m_rs, "isc_enable_url_filter" );
			pstIisSecurity->isc_enable_content_filter	= CProcDb::QueryIntValue( m_rs, "isc_enable_content_filter" );
			pstIisSecurity->isc_enable_lmt_urllen		= CProcDb::QueryIntValue( m_rs, "isc_enable_lmt_urllen" );
			pstIisSecurity->isc_lmt_urllen_cfg		= CProcDb::QueryIntValue( m_rs, "isc_lmt_urllen_cfg" );
			pstIisSecurity->isc_enable_lmt_urlchars		= CProcDb::QueryIntValue( m_rs, "isc_enable_lmt_urlchars" );
			CProcDb::QueryStringValue( m_rs, "isc_lmt_urlchars_cfg", pstIisSecurity->isc_lmt_urlchars_cfg, sizeof(pstIisSecurity->isc_lmt_urlchars_cfg) );
			pstIisSecurity->isc_enable_lmt_contentlen	= CProcDb::QueryIntValue( m_rs, "isc_enable_lmt_contentlen" );
			pstIisSecurity->isc_lmt_contentlen_sendbyserver	= CProcDb::QueryIntValue( m_rs, "isc_lmt_contentlen_sendbyserver" );
			pstIisSecurity->isc_lmt_contentlen_postbyclient	= CProcDb::QueryIntValue( m_rs, "isc_lmt_contentlen_postbyclient" );

			pstIisSecurity->isc_enable_lmt_dayip		= CProcDb::QueryIntValue( m_rs, "isc_enable_lmt_dayip" );
			pstIisSecurity->isc_lmt_dayip			= CProcDb::QueryIntValue( m_rs, "isc_lmt_dayip" );

			CProcDb::DecodeBadChar( pstIisSecurity->isc_lmt_urlchars_cfg );

			bRet = TRUE;
		}
		m_rs->Close();
	}

	return bRet;
}
/*
	for [iis_security_table]
*/
BOOL CProcCfg::SaveIisSecurityTableInfoById( LONG isc_id, STIISSECURITYTABLE * pstIisSecurity )
{
	if ( NULL == pstIisSecurity )
		return FALSE;


	CProcDb::KillBadChar( pstIisSecurity->isc_lmt_urlchars_cfg );
	CProcDb::KillBadChar( pstIisSecurity->iis_siteid );

	if ( isc_id >= INSERT_NEW_ITEM_FIRST )
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"INSERT INTO iis_security_table ( iis_siteid, isc_enable_iis_disguise, isc_iis_disguise_cfg, isc_enable_antatt, isc_antatt_maxvisit, isc_antatt_sleep, isc_antatt_disabled_proxy, isc_enable_url_filter, isc_enable_content_filter, isc_enable_lmt_urllen, isc_lmt_urllen_cfg, isc_enable_lmt_urlchars, isc_lmt_urlchars_cfg, isc_enable_lmt_contentlen, isc_lmt_contentlen_sendbyserver, isc_lmt_contentlen_postbyclient, isc_enable_lmt_dayip, isc_lmt_dayip ) \
			VALUES ( '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d )",
			pstIisSecurity->iis_siteid,
			pstIisSecurity->isc_enable_iis_disguise,
			pstIisSecurity->isc_iis_disguise_cfg,

			pstIisSecurity->isc_enable_antatt,
			pstIisSecurity->isc_antatt_maxvisit,
			pstIisSecurity->isc_antatt_sleep,
			pstIisSecurity->isc_antatt_disabled_proxy,

			pstIisSecurity->isc_enable_url_filter,
			pstIisSecurity->isc_enable_content_filter,
			pstIisSecurity->isc_enable_lmt_urllen,
			pstIisSecurity->isc_lmt_urllen_cfg,
			pstIisSecurity->isc_enable_lmt_urlchars,
			pstIisSecurity->isc_lmt_urlchars_cfg,
			pstIisSecurity->isc_enable_lmt_contentlen,
			pstIisSecurity->isc_lmt_contentlen_sendbyserver,
			pstIisSecurity->isc_lmt_contentlen_postbyclient,
			pstIisSecurity->isc_enable_lmt_dayip,
			pstIisSecurity->isc_lmt_dayip
		);
	}
	else
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE iis_security_table SET isc_enable_iis_disguise=%d, isc_iis_disguise_cfg=%d, \
			isc_enable_antatt=%d, isc_antatt_maxvisit=%d, isc_antatt_sleep=%d, isc_antatt_disabled_proxy=%d, \
			isc_enable_url_filter=%d, isc_enable_content_filter=%d, isc_enable_lmt_urllen=%d, isc_lmt_urllen_cfg=%d, \
			isc_enable_lmt_urlchars=%d, isc_lmt_urlchars_cfg='%s', isc_enable_lmt_contentlen=%d, \
			isc_lmt_contentlen_sendbyserver=%d, isc_lmt_contentlen_postbyclient=%d, isc_enable_lmt_dayip=%d, isc_lmt_dayip=%d WHERE isc_id=%d",
			pstIisSecurity->isc_enable_iis_disguise,
			pstIisSecurity->isc_iis_disguise_cfg,

			pstIisSecurity->isc_enable_antatt,
			pstIisSecurity->isc_antatt_maxvisit,
			pstIisSecurity->isc_antatt_sleep,
			pstIisSecurity->isc_antatt_disabled_proxy,

			pstIisSecurity->isc_enable_url_filter,
			pstIisSecurity->isc_enable_content_filter,
			pstIisSecurity->isc_enable_lmt_urllen,
			pstIisSecurity->isc_lmt_urllen_cfg,
			pstIisSecurity->isc_enable_lmt_urlchars,
			pstIisSecurity->isc_lmt_urlchars_cfg,
			pstIisSecurity->isc_enable_lmt_contentlen,
			pstIisSecurity->isc_lmt_contentlen_sendbyserver,
			pstIisSecurity->isc_lmt_contentlen_postbyclient,
			pstIisSecurity->isc_enable_lmt_dayip,
			pstIisSecurity->isc_lmt_dayip,
			isc_id
		);
	}

	CProcDb::DecodeBadChar( pstIisSecurity->isc_lmt_urlchars_cfg );

	return CProcDb::ExectueSql( m_szSql );
}
/*
	for [iis_security_table]
*/
BOOL CProcCfg::DeleteIisSecurityTableInfoById( LONG isc_id )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "DELETE FROM iis_security_table WHERE isc_id=%d", isc_id );

	// ..
	return CProcDb::ExectueSql( m_szSql );
}
/*
	for [iis_security_table]
*/
BOOL CProcCfg::DeleteAllIisSecurityTableInfoBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar(lpszSiteId);

	if ( strlen(lpszSiteId) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"DELETE FROM iis_security_table WHERE iis_siteid='%s'", lpszSiteId );

		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	return bRet;
}
/*
	for [iis_security_table]
	拷贝一条记录覆盖指定 SITEID 的站点
*/
BOOL CProcCfg::CopySecuritySettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo )
{
	/*
		lpszSiteId		- 被拷贝的站点
		lpszSiteIdCopyTo	- 要覆盖的目标站点
	*/

	if ( NULL == lpszSiteId || NULL == lpszSiteIdCopyTo )
		return FALSE;
	if ( 0 == strlen( lpszSiteId ) || 0 == strlen( lpszSiteIdCopyTo ) )
		return FALSE;

	BOOL bRet	= FALSE;
	STIISSECURITYTABLE stIisSecurity;

	if ( GetIisSecurityTableInfoBySiteId( lpszSiteId, & stIisSecurity ) )
	{
		//	删除原来的记录
		DeleteAllIisSecurityTableInfoBySiteId( lpszSiteIdCopyTo );

		//	拷贝之
		stIisSecurity.isc_id = INSERT_NEW_ITEM_FIRST + 1;
		_sntprintf( stIisSecurity.iis_siteid, sizeof(stIisSecurity.iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
		bRet = SaveIisSecurityTableInfoById( stIisSecurity.isc_id, & stIisSecurity );
	}

	return bRet;
}






/*
	for [iis_log_table]
*/
BOOL CProcCfg::GetIisLogTableInfoBySiteId( LPSTR lpszSiteId, STIISLOGABLE * pstIisLog )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return FALSE;
	if ( ! CProcDb::IsConnDatabase() )
		return FALSE;

	BOOL bRet	= FALSE;

	/*
		LONG	ilg_id;
		TCHAR	iis_siteid[32];				//	关联 iis_table 的字段
		LONG	ilg_logtimeperiod;			//	日志格式：0 每小时，1每天，2每周，3每月
		LONG	ilg_logsize;				//	日志文件大小：单位 MB	
	*/

	CProcDb::KillBadChar( lpszSiteId );

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT TOP 1 * FROM iis_log_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		if ( ! m_rs->adoEOF )
		{
			pstIisLog->ilg_id = CProcDb::QueryIntValue( m_rs, "ilg_id" );
			CProcDb::QueryStringValue( m_rs, "iis_siteid", pstIisLog->iis_siteid, sizeof(pstIisLog->iis_siteid) );
			pstIisLog->ilg_logtimeperiod = CProcDb::QueryIntValue( m_rs, "ilg_logtimeperiod" );
			pstIisLog->ilg_logsize = CProcDb::QueryIntValue( m_rs, "ilg_logsize" );

			bRet = TRUE;
		}
		m_rs->Close();
	}

	return bRet;
}
/*
	for [iis_log_table]
*/
BOOL CProcCfg::SaveIisLogTableInfoById( LONG ilg_id, STIISLOGABLE * pstIisLog )
{
	if ( NULL == pstIisLog )
		return FALSE;
	

	CProcDb::KillBadChar( pstIisLog->iis_siteid );
	
	if ( ilg_id >= INSERT_NEW_ITEM_FIRST )
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"INSERT INTO iis_log_table ( iis_siteid, ilg_logtimeperiod, ilg_logsize ) VALUES ( '%s', %d, %d )",
			pstIisLog->iis_siteid,
			pstIisLog->ilg_logtimeperiod,
			pstIisLog->ilg_logsize
		);
	}
	else
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE iis_log_table SET ilg_logtimeperiod=%d, ilg_logsize=%d WHERE ilg_id=%d",
			pstIisLog->ilg_logtimeperiod,
			pstIisLog->ilg_logsize,
			ilg_id
		);
	}

	return CProcDb::ExectueSql( m_szSql );
}
/*
	for [iis_log_table]
*/
BOOL CProcCfg::DeleteIisLogTableInfoById( LONG ilg_id )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "DELETE FROM iis_log_table WHERE ilg_id=%d", ilg_id );
	
	// ..
	return CProcDb::ExectueSql( m_szSql );
}
/*
	for [iis_log_table]
*/
BOOL CProcCfg::DeleteAllIisLogTableInfoBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar(lpszSiteId);

	if ( strlen(lpszSiteId) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"DELETE FROM iis_log_table WHERE iis_siteid='%s'", lpszSiteId );
		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	return bRet;
}
/*
	for [iis_security_table]
	拷贝一条记录覆盖指定 SITEID 的站点
*/
BOOL CProcCfg::CopyLogSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo )
{
	/*
		lpszSiteId		- 被拷贝的站点
		lpszSiteIdCopyTo	- 要覆盖的目标站点
	*/

	if ( NULL == lpszSiteId || NULL == lpszSiteIdCopyTo )
		return FALSE;
	if ( 0 == strlen( lpszSiteId ) || 0 == strlen( lpszSiteIdCopyTo ) )
		return FALSE;

	BOOL bRet	= FALSE;
	STIISLOGABLE stIisLog;

	if ( GetIisLogTableInfoBySiteId( lpszSiteId, & stIisLog ) )
	{
		//	删除原来的记录
		DeleteAllIisLogTableInfoBySiteId( lpszSiteIdCopyTo );

		//	拷贝之
		stIisLog.ilg_id = INSERT_NEW_ITEM_FIRST + 1;
		_sntprintf( stIisLog.iis_siteid, sizeof(stIisLog.iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
		bRet = SaveIisLogTableInfoById( stIisLog.ilg_id, & stIisLog );
	}

	return bRet;
}








/*
	for [iis_advanced_table]
*/
BOOL CProcCfg::GetIisAdvancedTableInfoBySiteId( LPSTR lpszSiteId, STIISADVANCEDTABLE * pstIisAdvanced )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) || NULL == pstIisAdvanced )
		return FALSE;
	if ( ! CProcDb::IsConnDatabase() )
		return FALSE;

	BOOL bRet	= FALSE;

	//
	//	LONG	iav_id;
	//	TCHAR	iis_siteid[32];				//	关联 iis_table 的字段
	//	LONG	iav_wm_show_vwtitle;			//	警告消息：是否显示VW标题
	//	LONG	iav_wm_model;				//	警告消息：模式，1-normal, 2-expert, 3-htmlfile
	//	LONG	iav_wm_http_status;			//	警告消息：http status
	//	LONG	iav_wm_delay_time;			//	警告消息：停留时间
	//	TCHAR	iav_wm_redirect_url[ MAX_PATH ];	//	警告消息：转向 URL
	//	TCHAR	iav_wm_sitename[ MAX_PATH ];		//	警告消息：网站名称
	//	TCHAR	iav_wm_info[ 1024 ];			//	警告消息：用户自定义的警告信息
	//	TCHAR	iav_wm_htmlfile[ MAX_PATH ];		//	警告消息：用户自定义的 HTML 文件	
	//

	CProcDb::KillBadChar( lpszSiteId );

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT TOP 1 * FROM iis_advanced_table WHERE iis_siteid='%s'", lpszSiteId );
	if ( S_OK == m_rs->Open( _variant_t( m_szSql ), _variant_t( (IDispatch *)m_conn, true ), adOpenStatic, adLockReadOnly, adCmdText ) )
	{
		if ( ! m_rs->adoEOF )
		{
			pstIisAdvanced->iav_id = CProcDb::QueryIntValue( m_rs, "iav_id" );
			CProcDb::QueryStringValue( m_rs, "iis_siteid", pstIisAdvanced->iis_siteid, sizeof(pstIisAdvanced->iis_siteid) );

			pstIisAdvanced->iav_wm_show_vwtitle	= CProcDb::QueryIntValue( m_rs, "iav_wm_show_vwtitle" );
			pstIisAdvanced->iav_wm_model		= CProcDb::QueryIntValue( m_rs, "iav_wm_model" );
			pstIisAdvanced->iav_wm_http_status	= CProcDb::QueryIntValue( m_rs, "iav_wm_http_status" );
			pstIisAdvanced->iav_wm_delay_time	= CProcDb::QueryIntValue( m_rs, "iav_wm_delay_time" );

			CProcDb::QueryStringValue( m_rs, "iav_wm_redirect_url", pstIisAdvanced->iav_wm_redirect_url, sizeof(pstIisAdvanced->iav_wm_redirect_url) );
			CProcDb::QueryStringValue( m_rs, "iav_wm_sitename", pstIisAdvanced->iav_wm_sitename, sizeof(pstIisAdvanced->iav_wm_sitename) );
			CProcDb::QueryStringValue( m_rs, "iav_wm_sitelogo_url", pstIisAdvanced->iav_wm_sitelogo_url, sizeof(pstIisAdvanced->iav_wm_sitelogo_url) );
			CProcDb::QueryStringValue( m_rs, "iav_wm_info", pstIisAdvanced->iav_wm_info, sizeof(pstIisAdvanced->iav_wm_info) );
			CProcDb::QueryStringValue( m_rs, "iav_wm_htmlfile", pstIisAdvanced->iav_wm_htmlfile, sizeof(pstIisAdvanced->iav_wm_htmlfile) );

			bRet = TRUE;
		}
		m_rs->Close();
	}

	return bRet;
}

/*
	for [iis_advanced_table]
*/
BOOL CProcCfg::SaveIisAdvancedTableInfoById( LONG iav_id, STIISADVANCEDTABLE * pstIisAdvanced )
{
	if ( NULL == pstIisAdvanced )
		return FALSE;


	CProcDb::KillBadChar( pstIisAdvanced->iis_siteid );
	CProcDb::KillBadChar( pstIisAdvanced->iav_wm_redirect_url );
	CProcDb::KillBadChar( pstIisAdvanced->iav_wm_sitename );
	CProcDb::KillBadChar( pstIisAdvanced->iav_wm_info );
	CProcDb::KillBadChar( pstIisAdvanced->iav_wm_htmlfile );


	if ( iav_id >= INSERT_NEW_ITEM_FIRST )
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"INSERT INTO iis_advanced_table " \
			"( iis_siteid, iav_wm_show_vwtitle, iav_wm_model, iav_wm_http_status, iav_wm_delay_time, iav_wm_redirect_url, iav_wm_sitename, iav_wm_sitelogo_url, iav_wm_info, iav_wm_htmlfile ) " \
			"VALUES ( '%s', %d, %d, %d, %d, '%s', '%s', '%s', '%s', '%s' )",
			pstIisAdvanced->iis_siteid,
			pstIisAdvanced->iav_wm_show_vwtitle,
			pstIisAdvanced->iav_wm_model,
			pstIisAdvanced->iav_wm_http_status,
			pstIisAdvanced->iav_wm_delay_time,
			pstIisAdvanced->iav_wm_redirect_url,
			pstIisAdvanced->iav_wm_sitename,
			pstIisAdvanced->iav_wm_sitelogo_url,
			pstIisAdvanced->iav_wm_info,
			pstIisAdvanced->iav_wm_htmlfile
		);
	}
	else
	{
		_sntprintf
		(
			m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"UPDATE iis_advanced_table SET " \
			"iav_wm_show_vwtitle=%d, iav_wm_model=%d, iav_wm_http_status=%d, iav_wm_delay_time=%d, iav_wm_redirect_url='%s', iav_wm_sitename='%s', iav_wm_sitelogo_url='%s', iav_wm_info='%s', iav_wm_htmlfile='%s' " \
			"WHERE iav_id=%d",
			pstIisAdvanced->iav_wm_show_vwtitle,
			pstIisAdvanced->iav_wm_model,
			pstIisAdvanced->iav_wm_http_status,
			pstIisAdvanced->iav_wm_delay_time,
			pstIisAdvanced->iav_wm_redirect_url,
			pstIisAdvanced->iav_wm_sitename,
			pstIisAdvanced->iav_wm_sitelogo_url,
			pstIisAdvanced->iav_wm_info,
			pstIisAdvanced->iav_wm_htmlfile,
			iav_id
		);
	}

	return CProcDb::ExectueSql( m_szSql );
}
/*
	for [iis_advanced_table]
*/
BOOL CProcCfg::DeleteIisAdvancedTableInfoById( LONG iav_id )
{
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "DELETE FROM iis_advanced_table WHERE iav_id=%d", iav_id );

	// ..
	return CProcDb::ExectueSql( m_szSql );
}
/*
	for [iis_advanced_table]
*/
BOOL CProcCfg::DeleteAllIisAdvancedTableInfoBySiteId( LPSTR lpszSiteId )
{
	if ( NULL == lpszSiteId || 0 == _tcslen(lpszSiteId) )
		return FALSE;

	BOOL bRet	= FALSE;

	CProcDb::KillBadChar(lpszSiteId);

	if ( strlen(lpszSiteId) > 0 )
	{
		_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR),
			"DELETE FROM iis_advanced_table WHERE iis_siteid='%s'", lpszSiteId );
		// ..
		bRet = CProcDb::ExectueSql( m_szSql );
	}

	return bRet;
}
/*
	for [iis_advanced_table]
	拷贝一条记录覆盖指定 SITEID 的站点
*/
BOOL CProcCfg::CopyAdvancedSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo )
{
	/*
		lpszSiteId		- 被拷贝的站点
		lpszSiteIdCopyTo	- 要覆盖的目标站点
	*/

	if ( NULL == lpszSiteId || NULL == lpszSiteIdCopyTo )
		return FALSE;
	if ( 0 == strlen( lpszSiteId ) || 0 == strlen( lpszSiteIdCopyTo ) )
		return FALSE;

	BOOL bRet	= FALSE;
	STIISADVANCEDTABLE stIisAdvanced;

	if ( GetIisAdvancedTableInfoBySiteId( lpszSiteId, & stIisAdvanced ) )
	{
		//	删除原来的记录
		DeleteAllIisAdvancedTableInfoBySiteId( lpszSiteIdCopyTo );

		//	拷贝之
		stIisAdvanced.iav_id = INSERT_NEW_ITEM_FIRST + 1;
		_sntprintf( stIisAdvanced.iis_siteid, sizeof(stIisAdvanced.iis_siteid)-sizeof(TCHAR), "%s", lpszSiteIdCopyTo );
		bRet = SaveIisAdvancedTableInfoById( stIisAdvanced.iav_id, & stIisAdvanced );
	}

	return bRet;
}