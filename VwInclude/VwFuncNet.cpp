// VwFunc.cpp: implementation of the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "VwFuncNet.h"
#include <stdio.h>
#include <stdlib.h>

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )



/**
 *	将字符串的 IP 地址转换成数字
 */
DWORD _vwfunc_get_ipaddr_value_from_string( LPCSTR lpcszIpAddr )
{
	if ( NULL == lpcszIpAddr || 0 == lpcszIpAddr[ 0 ] )
	{
		return 0;
	}

	//	INADDR_NONE 如果不是一个合法的 IP 地址
	return inet_addr( lpcszIpAddr );
}


/**
 *	获取所有本地的 IP 地址
 */
BOOL _vwfunc_get_all_ipaddr( vector<STVWFIPADDR> & vcIpAddr )
{
	BOOL  bRet		= FALSE;
	WSADATA WSData;
	HOSTENT * pst		= NULL;
	TCHAR ** pplist		= NULL;
	TCHAR * pTemp		= NULL;
	INT i			= 0;
	DWORD dwIndex		= 0;
	TCHAR szBuffer[128]	= {0};
	STVWFIPADDR stIpAddr;
	
	try
	{
		if ( 0 == WSAStartup( MAKEWORD(VERSION_MAJOR,VERSION_MINOR), &WSData ) )
		{
			if ( 0 == gethostname( szBuffer, sizeof(szBuffer) ) )
			{
				pst = gethostbyname(szBuffer);
				if ( pst )
				{
					pplist = pst->h_addr_list;
					while( pplist[i] )
					{
						//	..
						pTemp = inet_ntoa( *((LPIN_ADDR)pplist[i]) );
						//	..
						if ( pTemp && _vwfunc_is_valid_ipaddr( pTemp ) )
						{
							bRet = TRUE;

							memset( &stIpAddr, 0, sizeof(stIpAddr) );
							_sntprintf( stIpAddr.szIpAddr, sizeof(stIpAddr.szIpAddr)-sizeof(TCHAR), "%s", pTemp );
							vcIpAddr.push_back( stIpAddr );

							dwIndex ++;
						}
						i++;
					}
				}
			}
			// ..
			WSACleanup();
		}
	}
	catch (...)
	{
	}
	
	// ..
	return bRet;
}

/**
 *	获取 MAC 地址
 */
BOOL _vwfunc_get_mac_address( LPCTSTR lpcszNetBiosName, LPTSTR lpszMacAddress, DWORD dwSize )
{
	/*
		lpcszNetBiosName	- [in]
		lpszMacAddress		- [out] 
		dwSize			- [in]
		RETURN			- TRUE / FALSE
	*/
	if ( NULL == lpcszNetBiosName || 0 == strlen( lpcszNetBiosName ) )
	{
		return FALSE;
	}
	if ( NULL == lpszMacAddress || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL bRet		= FALSE;
	TCHAR szBiosName[ 64 ]	= {0};
	STASTAT Adapter;
	NCB ncb;
	UCHAR uRetCode;

	//	copy bios name
	_sntprintf( szBiosName, sizeof(szBiosName)-sizeof(TCHAR), "%s", lpcszNetBiosName );

	memset( & ncb, 0, sizeof(ncb) );
	ncb.ncb_command		= NCBRESET;
	ncb.ncb_lana_num	= 0;

	uRetCode = Netbios( &ncb );
	if ( 0 == uRetCode )
	{
		memset( & ncb, 0, sizeof(ncb) );
		ncb.ncb_command		= NCBASTAT;
		ncb.ncb_lana_num	= 0;

		//	大写
		_tcsupr( szBiosName );

		FillMemory( ncb.ncb_callname, NCBNAMSZ - 1, 0x20 );
		_sntprintf( (LPTSTR)ncb.ncb_callname, sizeof(ncb.ncb_callname)-sizeof(TCHAR), "%s", szBiosName );

		ncb.ncb_callname[ strlen( szBiosName ) ] = 0x20;
		ncb.ncb_callname[ NCBNAMSZ ] = 0x0;

		ncb.ncb_buffer = (unsigned char *) &Adapter;
		ncb.ncb_length = sizeof(Adapter);

		uRetCode = Netbios( &ncb );
		if ( 0 == uRetCode )
		{
			bRet = TRUE;
			_sntprintf
			(
				lpszMacAddress,
				dwSize-sizeof(TCHAR),
				"%02x-%02x-%02x-%02x-%02x-%02x", 
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]
			);
		}
	}

	return bRet;
}


//	获取硬盘序列号
BOOL _vwfunc_get_hdisk_serialnumber( LPTSTR lpszSerialNumber, DWORD dwSize )
{
	BOOL  bRet	= FALSE;
	TCHAR szNameBuf[ 12 ];
	DWORD dwSerialNumber;
	DWORD dwMaxLen; 
	DWORD dwFileFlag;
	TCHAR szSysNameBuf[ 10 ];

	bRet = ::GetVolumeInformation
	(
		"c:\\",
		szNameBuf,
		sizeof(szNameBuf), 
		& dwSerialNumber, 
		& dwMaxLen,
		& dwFileFlag,
		szSysNameBuf,
		sizeof(szSysNameBuf)
	);
	if ( bRet )
	{
		_sntprintf( lpszSerialNumber, dwSize-sizeof(TCHAR), "%x", dwSerialNumber );
	}

	return bRet;
}