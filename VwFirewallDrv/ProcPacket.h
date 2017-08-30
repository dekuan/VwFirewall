// ProcPacket.h: interface for the ProcPacket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCPACKET_HEADER__
#define __PROCPACKET_HEADER__

#include "stdafx.h"
#include "NetHeaders.h"
#include "RdpStruct.h"
#include "DrvFunc.h"
#include "DrvThreadLogger.h"
#include "ProcConfig.h"
#include "ProcRdp.h"


#define PROCPACKET_HTTP_HOST_DLEN	6
#define PROCPACKET_HTTP_CRLF_DLEN	2


BOOLEAN procpacket_ip_is_allowed_packet( UCHAR * puszPacket, UINT uSize );
BOOLEAN procpacket_http_is_allowed_domain( UCHAR * lpuszHttp );

BOOLEAN procpacket_write_templog( BYTE * pbtPacket, UINT uPacketLength, WCHAR * lpwszType, BOOLEAN bBlocked );

BOOLEAN procpacket_swap_string( UCHAR * lpuszString, INT nLen, INT nLeftStrLen );
BOOLEAN	procpacket_fill_packet_checksum( UCHAR * puszPacket );
USHORT	procpacket_get_checksum( USHORT * puszBuffer, int nSize );


USHORT  procpacket_get_valid_tcpport( USHORT uPort );
USHORT	procpacket_ntohs( USHORT x );
USHORT	procpacket_htons( USHORT x );
BOOLEAN procpacket_inet_ntoa( ULONG ulAddr, PCHAR pszAddr, UINT uSize );



#endif // __PROCPACKET_HEADER__

