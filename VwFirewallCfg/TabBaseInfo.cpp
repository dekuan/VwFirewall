// TabBaseInfo.cpp: implementation of the CTabBaseInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabBaseInfo.h"
#include "VwEnCodeString.h"
#include "ProcSecurityDesc.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTabBaseInfo::CTabBaseInfo()
{
	//
	//	初始化许可信息
	//
	memset( & m_stCfg, 0, sizeof(m_stCfg) );
	CVwFirewallConfigFile::LoadConfig( & m_stCfg );

	//	检测注册码是否正确
	memset( m_szPrNameVwFirewall, 0, sizeof(m_szPrNameVwFirewall) );
	memcpy( m_szPrNameVwFirewall, g_szPrName_VwFirewall, min( sizeof(g_szPrName_VwFirewall), sizeof(m_szPrNameVwFirewall) ) );
	delib_xorenc( m_szPrNameVwFirewall );
	m_bReged	= CVwLicence::IsValidLicense( m_szPrNameVwFirewall, m_stCfg.szRegIp, m_stCfg.szRegKey );
	m_bValidPrType	= CVwLicence::IsValidPrTypeChecksum( m_stCfg.szRegPrTypeChecksum, m_stCfg.szRegPrType, m_stCfg.szRegIp, m_stCfg.szRegKey );

	//	免费版本最多词数限制
	m_dwFreeVerDomainLimit = (INT)( m_bReged ? 1 : 0 );

	//
	memset( m_szHintFmt, 0, sizeof(m_szHintFmt) );
}

CTabBaseInfo::~CTabBaseInfo()
{
}


/**
 *	@ public
 *	设置 HINT 文字，带词个数
 */
BOOL CTabBaseInfo::GetHintWithWordCount( LPCTSTR lpcszHintFmt, DWORD dwWordCount, LPTSTR lpszBuffer, DWORD dwSize )
{
	if ( NULL == lpcszHintFmt || 0 == _tcslen( lpcszHintFmt ) )
	{
		return FALSE;
	}
	if ( NULL == lpszBuffer || 0 == dwSize )
	{
		return FALSE;
	}

	_sntprintf( lpszBuffer, dwSize-sizeof(TCHAR), lpcszHintFmt, dwWordCount );
	return TRUE;
}


/**
 *	检测 LISTVIEW 的 CHECKBOX 的状态是否发生变化
 */
BOOL CTabBaseInfo::IsCheckboxChanged( NM_LISTVIEW * pNMListView )
{
	//
	//	pNMListView	- [in] ...
	//	RETURN		- TRUE / FALSE
	//
	//	下面的代码演示如何设置 CHECKBOX 的状态
	//	void SetLVCheck (WPARAM ItemIndex, BOOL bCheck)
	//	{
	//		ListView_SetItemState( m_lvTestList.m_hWnd, ItemIndex, UINT((int(bCheck) + 1) << 12), LVIS_STATEIMAGEMASK);
	//	}
	//

	if ( NULL == pNMListView )
	{
		return FALSE;
	}
	
	BOOL bPrevState	= FALSE;
	BOOL bChecked	= FALSE;
	
	
	//
	//	First check ...
	//
	if ( pNMListView->uOldState == 0 && pNMListView->uNewState == 0 )
	{
		//	No change
		return FALSE;
	}
	
	//
	//	Get Old check box state
	//
	bPrevState	= (BOOL)( ( ( pNMListView->uOldState & LVIS_STATEIMAGEMASK ) >> 12 ) - 1 );
	if ( bPrevState < 0 )
	{
		//	On startup there's no previous state
		//	so assign as false (unchecked)
		bPrevState = 0;
	}
	
	//
	//	Get New check box state
	//
	bChecked	= (BOOL)( ( ( pNMListView->uNewState & LVIS_STATEIMAGEMASK ) >> 12 ) - 1 );
	if ( bChecked < 0 )
	{
		//	On non-checkbox notifications assume false
		bChecked = 0;
	}
	
	if ( bPrevState == bChecked )
	{
		//	No change in check box
		return FALSE;
	}
	else
	{
		//
		//	Hi, it changed !
		//
		return TRUE;
	}
}

BOOL CTabBaseInfo::NotifyDriverToLoadNewConfig()
{
	BOOL bRet	= FALSE;
	TCHAR szIOBuffer[ 32 ];

	if ( delib_drv_is_exist( DEVIE_NAMEA ) )
	{
		if ( SERVICE_RUNNING == delib_drv_get_status( DEVIE_NAMEA ) )
		{
			memset( szIOBuffer, 0, sizeof(szIOBuffer) );
			_sntprintf( szIOBuffer, sizeof(szIOBuffer)-sizeof(TCHAR), "%s", ( m_bReged ? "P1X" : "P0X" ) ); 

			if ( DELIBDRV_ERROR_SUCC == delib_drv_write_io( DEVIE_NAMEA, VWFIREWALLDRV_IOCTL_LOADCFG, szIOBuffer, sizeof(szIOBuffer) ) )
			{
				bRet = TRUE;
			}
		}
	}

	//
	//	制作 key 文件
	//
	MakeKeyFile();

	return bRet;
}

BOOL CTabBaseInfo::MakeKeyFile()
{
	BOOL bRet	= FALSE;
	TCHAR szIOBuffer[ 10240 ];
	TCHAR szSource[ 260 ];
	TCHAR szMd5[ 34 ];
	DWORD dwOffset;
	DWORD dwSubOffset;
	BOOL  bIOBufferMaked;
	HANDLE hFile;
	DWORD dwDummy;
	STVWFIREWALLREGDATE stRegDate;
	COleDateTime cOldTmRegExpireDate;


	bIOBufferMaked	= FALSE;

	//
	//	重新装载配置信息
	//
	CVwFirewallConfigFile::LoadConfig( & m_stCfg );

	//
	//	从驱动文件中读取 10240 个字节
	//
	hFile = CreateFile( CVwFirewallConfigFile::m_szSysDriverFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile && INVALID_HANDLE_VALUE != hFile )
	{
		dwDummy = 0;
		memset( szIOBuffer, 0, sizeof(szIOBuffer) );
		if ( ReadFile( hFile, &szIOBuffer, sizeof(szIOBuffer), &dwDummy, NULL) && dwDummy == sizeof(szIOBuffer) )
		{
			//
			//	继续加工
			//
			memset( szSource, 0, sizeof(szSource) );
			memset( szMd5, 0, sizeof(szMd5) );

			_sntprintf( szSource, sizeof(szSource)-sizeof(TCHAR), "%s,%s,%d,%s", m_stCfg.szRegPrType, m_stCfg.szRegIp, ( m_bReged ? 1 : 0 ), m_stCfg.szRegKey );
			_tcslwr( szSource );
			delib_get_string_md5( szSource, szMd5, sizeof(szMd5) );
			_tcslwr( szMd5 );

			//
			//	制作过期时间
			//
			memset( &stRegDate, 0, sizeof(stRegDate) );
			if ( _tcslen( m_stCfg.szRegExpireDate ) && 1 == m_stCfg.dwRegExpireType )
			{
				//
				//	有限期用户
				//	m_stCfg.szRegExpireDate 格式如下 "2016-1-1"
				//	m_stCfg.dwRegExpireType = 1 表示有限期用户
				//
				cOldTmRegExpireDate.ParseDateTime( m_stCfg.szRegExpireDate );
				if ( COleDateTime::valid == cOldTmRegExpireDate.GetStatus() )
				{
					stRegDate.nExpireYear	= cOldTmRegExpireDate.GetYear();
					stRegDate.nExpireMonth	= cOldTmRegExpireDate.GetMonth();
					stRegDate.nExpireDay	= cOldTmRegExpireDate.GetDay();
				}
			}

			//	拷贝到文件内容的内存中
			dwSubOffset = atoi(m_stCfg.szRegIp);
			if ( dwSubOffset > 255 )
			{
				dwSubOffset = 255;
			}
			dwOffset = 1024 + dwSubOffset;
			if ( dwOffset + 8 < sizeof(szIOBuffer) )
			{
				//	拷贝验证码
				memcpy( szIOBuffer + dwOffset, szMd5, 8 );

				//	拷贝过期时间
				memcpy( szIOBuffer + dwOffset + 8, &stRegDate, sizeof(stRegDate) );

				//
				//	IOBuffer 制作成功
				//
				bIOBufferMaked = TRUE;
			}
		}

		CloseHandle( hFile );
	}

	//
	//	先删除文件
	//
	DeleteFile( CVwFirewallConfigFile::m_szKeyFile );

	//
	//	输出内容到 KEY 文件
	//
	if ( bIOBufferMaked )
	{
		hFile = CreateFile( CVwFirewallConfigFile::m_szKeyFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( INVALID_HANDLE_VALUE != hFile)
		{
			dwDummy = 0;
			if ( WriteFile( hFile, szIOBuffer, sizeof(szIOBuffer), &dwDummy, NULL ) && dwDummy == sizeof(szIOBuffer) )
			{
				bRet = TRUE;
			}

			CloseHandle( hFile );
		}
	}

	return bRet;
}


/*
	输出调试信息
*/
BOOL CTabBaseInfo::OutputDebugStringFormat( LPCTSTR lpszFmt, ... )
{
	//
	//	lpszFmt		- [in] Format-control string
	//	argument	- [in] Optional arguments		
	//

	INT   nSize			= 0;
	TCHAR szMessage[ MAX_PATH ];
	va_list args;
	va_start( args, lpszFmt );
	szMessage[ 0 ] = 0;
	nSize = _vsnprintf( szMessage, sizeof(szMessage), lpszFmt, args );
	va_end( args );

	//	..
	OutputDebugString( szMessage );
	
	return FALSE;
}