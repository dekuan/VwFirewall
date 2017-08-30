// ProcPacket.cpp: implementation of the ProcPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcPacket.h"





/**
 *	处理泛域名解析
 **/
BOOLEAN procpacket_ip_is_allowed_packet( UCHAR * puszPacket, UINT uSize )
{
	//
	//	puszPacket	- [in] IP 数据包
	//	uSize		- [in] IP 数据包大小
	//	RETURN		- TRUE 放过，FALSE 拦截
	//

	BOOLEAN bRet		= TRUE;
	UCHAR * lpuszTcpContent;
	UINT uTcpContentLen;
	UINT uIPHeaderLen;
	UINT uTcpHeaderLen;
	STIPHEADER * pstIPPacket;
	STTCPHEADER * pstTCPPacket;
	UCHAR * pszTemp;
	TS_UD_CS_CORE * pstTsUdCsCore;
	CHAR szClientName[ 32 ];
	CHAR szRemoteIpAddr[ 16 ];

	if ( NULL == puszPacket || 0 == uSize )
	{
		return TRUE;
	}

	//	尽可能放过，只有检测后再确定是否拦截
	bRet = TRUE;


	__try
	{
		pstIPPacket = (STIPHEADER*)puszPacket;
		if ( pstIPPacket && IPPROTO_TCP == pstIPPacket->ucProtocol )
		{
			//
			//	parse ip packet
			//
			uIPHeaderLen	= pstIPPacket->ucHeaderLength * 4;
			pstTCPPacket	= (STTCPHEADER*)( puszPacket + uIPHeaderLen );
			if ( pstTCPPacket )
			{
				//
				//	parse tcp packet
				//
				uTcpHeaderLen	= ( pstTCPPacket->offset ? pstTCPPacket->offset : pstTCPPacket->unused ) * 4;
				lpuszTcpContent	= (UCHAR*)( puszPacket + uIPHeaderLen + uTcpHeaderLen );
				uTcpContentLen	= uSize - ( uIPHeaderLen + uTcpHeaderLen );
				if ( lpuszTcpContent )
				{
					//
					//	域名白名单
					//
					if ( bRet &&
						g_stCfg.bStartProtectDomain )
					{
						if ( 0 == _strnicmp( lpuszTcpContent, g_szDecryptedString_GET, 5 ) ||
							0 == _strnicmp( lpuszTcpContent, g_szDecryptedString_POST, 6 ) )
						{
							if ( ! procpacket_http_is_allowed_domain( lpuszTcpContent ) )
							{
								//	拦截掉此包
								bRet = FALSE;

								//	统计拦截此次数
								procconfig_add_block_count( & g_stCfg.llProtectDomainBlockCount );
							}

							//procpacket_write_templog( (BYTE*)lpuszTcpContent, uTcpContentLen, L"DM", bRet );

							procpacket_inet_ntoa( pstIPPacket->ulSourceAddress, szRemoteIpAddr, sizeof(szRemoteIpAddr) );
							KdPrint(( "procpacket_ip_is_allowed_packet:[%s-%d]{%s} %s\n", "DM", ( bRet ? 1 : 0 ), szRemoteIpAddr, lpuszTcpContent ));
						}
					}


					//
					//	RDP 远程桌面安全
					//
					if ( bRet &&
						g_stCfg.bStartProtectRdp )
					{
						//	检查是否是发送向 RDP 服务端口的包
						if ( procrdp_isexist_rdp_port( procpacket_get_valid_tcpport( pstTCPPacket->destinationPort ) ) )
						{
							procpacket_inet_ntoa( pstIPPacket->ulSourceAddress, szRemoteIpAddr, sizeof(szRemoteIpAddr) );
							KdPrint( ( "procpacket_ip_is_allowed_packet:is rdp port [%s]{%s}\n", "RDP", szRemoteIpAddr ) );

							if ( procrdp_parse_packet_mcscipdu( (BYTE*)lpuszTcpContent, uTcpContentLen, &pstTsUdCsCore ) )
							{
								if ( ! procconfig_is_exist_rdp_clientname( pstTsUdCsCore->clientName, wcslen(pstTsUdCsCore->clientName)*sizeof(WCHAR) ) )
								{
									//	拦截掉此包
									bRet = FALSE;

									//	统计拦截此次数
									procconfig_add_block_count( & g_stCfg.llProtectRdpBlockCount );
								}

								//procpacket_write_templog( (BYTE*)lpuszTcpContent, uTcpContentLen, L"RDP", bRet );

								drvfunc_w2c( pstTsUdCsCore->clientName, szClientName, sizeof(szClientName) );
								KdPrint(( "procpacket_ip_is_allowed_packet:[%s-%d]{%s} %s\n", "RDP", ( bRet ? 1 : 0 ), szRemoteIpAddr, szClientName ));
							}
						}
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_ip_is_allowed_packet\n" ));
	}	

	return bRet;
}

BOOLEAN procpacket_write_templog( BYTE * pbtPacket, UINT uPacketLength, WCHAR * lpwszType, BOOLEAN bBlocked )
{
	BOOLEAN bRet;
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;
	WCHAR wszLogFile[ MAX_PATH ];
	STDRVTLLOG stLog;

	if ( NULL == pbtPacket || 0 == uPacketLength || NULL == lpwszType )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	KeQuerySystemTime( &lnSystemTime );
	ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
	RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );

	if ( g_stCfg.uLogDirLength > 4 )
	{
		memset( wszLogFile, 0, sizeof(wszLogFile) );
		_snwprintf
		(
			wszLogFile, sizeof(wszLogFile)/sizeof(WCHAR)-1,
			L"%s[%s-%d]%04d%02d%02d-%02d%02d%02d-%d.log",
			g_stCfg.wszLogDir,
			lpwszType,
			( bBlocked ? 1 : 0 ),
			tfTimeFields.Year, tfTimeFields.Month, tfTimeFields.Day,
			tfTimeFields.Hour, tfTimeFields.Minute, tfTimeFields.Second, tfTimeFields.Milliseconds
		);

		//bRet = drvfunc_save_filecontent( wszLogFile, (PVOID)pbtPacket, uPacketLength );

		stLog.lpcwszFilename	= wszLogFile;
		stLog.lpvContent	= (PVOID)pbtPacket;
		stLog.dwSize		= uPacketLength;

		if ( NT_SUCCESS( drvthreadlogger_execute_task( drvthreadlogger_worker, (PVOID)(&stLog) ) ) )
		{
			bRet = TRUE;
		}	
	}

	return bRet;
}

/**
 *	交换字符串处理
 */
BOOLEAN procpacket_swap_string( UCHAR * lpuszString, INT nLen, INT nLeftStrLen )
{
	//
	//	lpuszString	- [in] 输入源字符串
	//	uLen		- [in] 输入源字符串的长度
	//	uLeftStrLen	- [in] 左边需要移动的字符串的长度
	//	
	INT i, j, k;
	UCHAR cTemp;

	if ( NULL == lpuszString || 0 == nLen || nLeftStrLen >= nLen )
	{
		return FALSE;
	}	

	__try
	{
		for ( i = nLeftStrLen, k = 1;
			i < nLen;
			i++, k++ )
		{
			for ( j = i; j >= k ; j -- )
			{
				cTemp			= lpuszString[ j ];
				lpuszString[ j ]	= lpuszString[ j - 1 ];
				lpuszString[ j - 1 ]	= cTemp;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_swap_string\n" ));
	}	

	return TRUE;
}

/**
 *	从 HTTP 包里检查是否是允许访问的 域名
 */
BOOLEAN procpacket_http_is_allowed_domain( UCHAR * lpuszHttp )
{
	//
	//	lpuszHttp	- [in] http 包开始的位置
	//
	//	HTTP包数据格式如下
	//	------------------------------------------------------------
	//	GET http://runonce.msn.com/runonce3.aspx HTTP/1.1
	//	Accept: */*
	//	Accept-Language: zh-cn
	//	UA-CPU: x86
	//	Accept-Encoding: gzip, deflate
	//	User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727; .NET CLR 1.1.4322)
	//	Host: runonce.msn.com
	//	Connection: Keep-Alive
	//

	BOOLEAN bRet;
	UCHAR * lpuszHost;
	UCHAR * lpuszHostNew;
	UCHAR * lpuszTopDomain;
	UCHAR * lpuszUserAgent;
	UCHAR * lpuszTail;
	UCHAR * lpuszTailCrlf;
	UCHAR * lpuszTailColon;

	UINT	uHttpHostDLen;
	UINT	uHttpUserAgentDLen;
	UINT	uFullDomainLen;
	UINT	uTopDomainLen;
	UINT	uTopDomainOffset;

	UCHAR * lpuszSubDomain;
	UINT	uSubDomainLen;
	BOOLEAN bDomainMatched;

	if ( NULL == lpuszHttp )
	{
		return FALSE;
	}
	if ( 0 == g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uDomainSingleCount )
	{
		//	例外白名单为空，也即没有任何域名可以访问，所以拦截掉
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		uHttpHostDLen		= PROCPACKET_HTTP_HOST_DLEN;

		//	find "Host:"
		lpuszHost = strstr( lpuszHttp, g_szDecryptedString_Host1 );
		if ( NULL == lpuszHost )
		{
			uHttpHostDLen --;
			lpuszHost = strstr( lpuszHttp, g_szDecryptedString_Host2 );
		}

		if ( lpuszHost )
		{
			//
			//	要处理的配置文件中的域名长度
			//
			uTopDomainLen	= 0;
			lpuszTopDomain	= NULL;
			lpuszTailCrlf	= strstr( lpuszHost, "\r\n" );
			lpuszTailColon	= strstr( lpuszHost + uHttpHostDLen, ":" );
			if ( lpuszTailCrlf && lpuszTailColon )
			{
				lpuszTail = min( lpuszTailCrlf, lpuszTailColon );
			}
			else
			{
				lpuszTail = lpuszTailCrlf;
			}
			if ( lpuszTail )
			{
				uFullDomainLen	= (UINT)( lpuszTail - lpuszHost - uHttpHostDLen );
				if ( uFullDomainLen >= 4 && uFullDomainLen < 128 )
				{
					//	4 ~ 128 的长度认为是比较合适的域名长度
					bRet = procconfig_is_matched_domain( lpuszHost + uHttpHostDLen, uFullDomainLen );
				}
				else
				{
					//	放过 - 域名长度不正确
					bRet = TRUE;
				}	
			}
			else
			{
				//	放过 - 未找到 HOST 节的正确结尾
				bRet = TRUE;
			}	
		}
		else
		{
			//
			//	包里未找到 HOST 节
			//

			//	确认的确是 HTTP 包
			if ( strstr( lpuszHttp, " HTTP/1." ) || strstr( lpuszHttp, " http/1." ) )
			{
				//	find "User-Agent:"
				lpuszUserAgent = strstr( lpuszHttp, g_szDecryptedString_UserAgent1 );
				if ( NULL == lpuszUserAgent )
				{
					lpuszUserAgent = strstr( lpuszHttp, g_szDecryptedString_UserAgent2 );
				}

				if ( lpuszUserAgent )
				{
					//	包内含有 " HTTP/1." 和 "User-Agent: " 节，肯定是 HTTP
					//	这样的包要拦截
					bRet = FALSE;
				}
				else
				{
					//	未找到 "User-Agent: " 有点危险，放过
					bRet = TRUE;
				}	
			}
			else
			{
				//	未找到 " HTTP/1." 有点危险，放过
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_http_is_allowed_domain\n" ));
	}	

	return bRet;
}


/**
 *	负责制作 TCP/UDP 包校验和，并填充到包内
 */
BOOLEAN procpacket_fill_packet_checksum( UCHAR * puszPacket )
{
	//
	//	puszPacket	- [in] IP 数据包
	//	RETURN		成功计算并填写校验和 TRUE
	//			OR FALSE
	//

	BOOLEAN bRet				= FALSE;
	STIPHEADER * pstIPHeader		= NULL;		//	IP头指针
	UINT uIpHeaderLen;					//	IP头长度
	USHORT uIpDataLen			= 0;		//	IP数据部分整体长度

	STTCPHEADER * pstTCPHeader		= NULL;		//	TCP头指针
	STTCPPSDHEADER * pstTCPPsdHeader	= NULL;

	STIPHEADER * pstIPHeaderTemp;		//	临时存放 IP 头


	if ( NULL == puszPacket )
	{
		return FALSE;
	}

	__try
	{
		pstIPHeader = (STIPHEADER*)puszPacket;
		if ( pstIPHeader )
		{
			if ( IPPROTO_TCP == pstIPHeader->ucProtocol )
			{
				////////////////////////////////////////
				//	FOR TCP 包
				////////////////////////////////////////

				//	IP头长度
				uIpHeaderLen	= pstIPHeader->ucHeaderLength * 4;	//	( ( pstIPHeader->ucHeaderLength ) & 15 ) * 4;
				if ( uIpHeaderLen >= sizeof(STIPHEADER) )
				{
					pstTCPHeader	= (STTCPHEADER*)( puszPacket + uIpHeaderLen );

					//	传输层协议头以及附加数据的总长度
					//	TCP 头部 + TCP 数据整个长度
					uIpDataLen	= procpacket_ntohs( pstIPHeader->uTotalLength ) - uIpHeaderLen;

					//
					//	1, 备份 IP 头
					//	2, 填充 TCP 伪头部
					//	3, 计算 TCP 部分 checksum
					//	4, 还原 IP 头
					//	5, 计算 IP 头部的 checksum
					//
					pstIPHeaderTemp	= (STIPHEADER*)ExAllocatePool( NonPagedPool, uIpHeaderLen );
					if ( pstIPHeaderTemp )
					{
						memset( pstIPHeaderTemp, 0, uIpHeaderLen );

						//	备份 IP 头
						memcpy( pstIPHeaderTemp, pstIPHeader, uIpHeaderLen );

						//	填充伪 TCP 头
						//	需要拷贝的数据，距离 IP 头的偏移
						pstTCPPsdHeader = (STTCPPSDHEADER*)( (UCHAR*)pstIPHeader + ( uIpHeaderLen - sizeof(STTCPPSDHEADER) ) );
						memset( pstTCPPsdHeader, 0, sizeof(STTCPPSDHEADER) );

						pstTCPPsdHeader->destip		= pstIPHeaderTemp->ulDestinationAddress;
						pstTCPPsdHeader->sourceip	= pstIPHeaderTemp->ulSourceAddress;
						pstTCPPsdHeader->mbz		= 0;
						pstTCPPsdHeader->ptcl		= IPPROTO_TCP;
						pstTCPPsdHeader->tcpl		= procpacket_htons( uIpDataLen );

						//
						//	计算 TCP 校验和，并写入原始包内存
						//	TCP 首部 + TCP 数据部分整体计算校验和
						//
						pstTCPHeader->checksum	= 0;
						pstTCPHeader->checksum	= procpacket_get_checksum( (USHORT*)pstTCPPsdHeader, uIpDataLen + sizeof(STTCPPSDHEADER) );

						//	还原 IP 头
						memcpy( pstIPHeader, pstIPHeaderTemp, uIpHeaderLen );

						//
						//	计算ip头的校验和，并写入原始包内存
						//	IP 首部校验和是对 IP 头做校验和
						//
						pstIPHeader->uChecksum	= 0;
						pstIPHeader->uChecksum	= procpacket_get_checksum( (USHORT*)pstIPHeader, uIpHeaderLen );


						//	free memory
						ExFreePool( pstIPHeaderTemp );
						pstIPHeaderTemp = NULL;
					}
				}
			}
			else if ( IPPROTO_UDP == pstIPHeader->ucProtocol )
			{
				////////////////////////////////////////
				//	FOR UDP 包
				////////////////////////////////////////
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_fill_packet_checksum\n" ));
	}


	return bRet;
}


/**
 *	获取 TCP 的 checksum
 */
USHORT procpacket_get_checksum( USHORT * puszBuffer, int nSize )
{
	ULONG ulRet = 0;

	__try
	{
		while ( nSize > 1 )
		{
			ulRet += *puszBuffer++;
			nSize  -= sizeof(USHORT);
		}

		if ( nSize )
		{
			ulRet += *(UCHAR*)puszBuffer;
		}

		ulRet	= ( ulRet >> 16 ) + ( ulRet & 0xffff );
		ulRet	+= ( ulRet >>16 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procpacket_get_checksum\n" ));
	}	

	return (USHORT)( ~ulRet );
}


/**
 *	获取正确的 TCP 端口
 */
USHORT procpacket_get_valid_tcpport( USHORT uPort )
{
	//	方法，低 8 位与高 8 位对调
	return MAKEWORD( HIBYTE(uPort), LOBYTE(uPort) );
}

/**
 *	一些 NET 处理函数
 **/
USHORT procpacket_ntohs( USHORT x )
{
	#if BYTE_ORDER == LITTLE_ENDIAN
		UCHAR * s = (UCHAR*)&x;
		return (USHORT)( s[0] << 8 | s[1] );
	#else
		return x;
	#endif
}
USHORT procpacket_htons( USHORT x )
{
	#if BYTE_ORDER == LITTLE_ENDIAN
		UCHAR *s = (UCHAR *)&x;
		return (USHORT)( s[0] << 8 | s[1] );
	#else
		return x;
	#endif
}
BOOLEAN procpacket_inet_ntoa( ULONG ulAddr, PCHAR pszAddr, UINT uSize )
{
	CHAR * pTemp;

	if ( NULL == pszAddr || uSize < 16 )
	{
		return FALSE;
	}

	pTemp = (CHAR*)&ulAddr;

	_snprintf
	(
		pszAddr, uSize/sizeof(CHAR)-1,
		"%d.%d.%d.%d",
		(INT)( ulAddr & 0x000000FF ),
		(INT)( ( ulAddr & 0x0000FF00 ) >> 8 ),
		(INT)( ( ulAddr & 0x00FF0000 ) >> 16 ),
		(INT)( ( ulAddr & 0xFF000000 ) >> 24 )
	);

	return TRUE;
}