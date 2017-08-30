// ProcRdp.c: implementation of the DrvThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcRdp.h"
#include "DrvFunc.h"
#include "DrvFuncReg.h"


/**
 *	全局变量定义
 */
HANDLE  g_procrdp_hThread		= NULL;
KEVENT  g_procrdp_hkEvent;
BOOLEAN g_procrdp_bThreadStart		= FALSE;	//	
USHORT	g_procrdp_ArrSrvPort[ 1024 ]	= {0};		//	RDP 端口号，应该是 USHORT 的
INT	g_procrdp_nSrvPortMaxSize	= 0;		//	RDP 端口号数组最大INDEX
INT	g_procrdp_nSrvPortCount		= 0;		//	RDP 端口号数组计数器




/**
 *	创建线程
 */
BOOLEAN procrdp_thread_readport_create()
{
	NTSTATUS ntStatus;

	//	初始化事件句柄
	KeInitializeEvent( &g_procrdp_hkEvent, NotificationEvent, FALSE );

	//	设置事件句柄为未激发状态，使得 KeWaitForSingleObject 阻塞等待
	KeResetEvent( &g_procrdp_hkEvent );

	//	是否开始
	g_procrdp_bThreadStart = FALSE;

	//	初始化端口存放数组
	memset( g_procrdp_ArrSrvPort, 0, sizeof(g_procrdp_ArrSrvPort) );
	g_procrdp_nSrvPortMaxSize	= sizeof(g_procrdp_ArrSrvPort) / sizeof(g_procrdp_ArrSrvPort[0]);
	g_procrdp_nSrvPortCount		= 0;

	//	创建线程
	ntStatus = PsCreateSystemThread( &g_procrdp_hThread, 0, NULL, NULL, NULL, procrdp_thread_readport_proc, NULL );

	//	...
	return NT_SUCCESS( ntStatus );
}

VOID procrdp_thread_readport_terminate()
{
	//	设置事件句柄为激发状态
	KeSetEvent( &g_procrdp_hkEvent, IO_NO_INCREMENT, FALSE );
}

VOID procrdp_thread_readport_start()
{
	g_procrdp_bThreadStart = TRUE;
}

VOID procrdp_thread_readport_stop()
{
	g_procrdp_bThreadStart = FALSE;
}

VOID procrdp_thread_readport_proc( IN PVOID pContext )
{
	NTSTATUS ntStatus;
	LARGE_INTEGER liTimeWait;
	UNICODE_STRING usRdpPortNumberRegPath;
	UNICODE_STRING usRdpPortNumberRegKeyName;
	ULONG ulRdpPortNumber;
	

	//
	//	间隔 3 秒 读取一次
	//	-30000000
	//
	liTimeWait.QuadPart = 3 * 10 * 1000 * 1000 * ( -1 );

	//
	//	"\REGISTRY\MACHINE\SYSTEM\ControlSet001\Services\VwFirewallDrv"
	//	"HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server\Wds\rdpwd\Tds\tcp"
	//	 "\REGISTRY\MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server\Wds\rdpwd\Tds\tcp"
	//
	ntStatus = drvfunc_a2u( PROCRDP_RDP_PORTNUMBER_REGPATH, &usRdpPortNumberRegPath );
	if ( ! NT_SUCCESS( ntStatus ) )
	{
		dprintf( "VwFirewallDrv.sys: procrdp_thread_readport_proc :: falied to init reg path string.\n" );
		return;
	}

	ntStatus = drvfunc_a2u( PROCRDP_RDP_PORTNUMBER_REGKEYNAME, &usRdpPortNumberRegKeyName );
	if ( ! NT_SUCCESS( ntStatus ) )
	{
		dprintf( "VwFirewallDrv.sys: procrdp_thread_readport_proc :: falied to init reg keyname string.\n" );
		return;
	}


	while ( TRUE )
	{
		//
		//	从注册表读取
		//
		if ( g_procrdp_bThreadStart )
		{
			dprintf( "VwFirewallDrv.sys: procrdp_thread_readport_proc :: read the rdp port.\n" );

			//	尝试从注册表读取 RDP 端口号
			ulRdpPortNumber = 0;
			if ( drvfuncreg_get_value_ulong( &usRdpPortNumberRegPath, &usRdpPortNumberRegKeyName, &ulRdpPortNumber ) )
			{
				//dprintf( ( "VwFirewallDrv.sys: procrdp_thread_readport_proc :: ### obtain rdp port=%d.\n", ulRdpPortNumber ) );
				if ( ulRdpPortNumber > 0 )
				{
					//	读取成功
					procrdp_thread_readport_pushnew( (USHORT)ulRdpPortNumber );
				}
			}
			else
			{
				dprintf( "VwFirewallDrv.sys: procrdp_thread_readport_proc :: falied to obtain rdp port @drvfuncreg_get_value_ulong.\n" );
			}	
		}
		else
		{
			dprintf( "VwFirewallDrv.sys: procrdp_thread_readport_proc :: thread not start @if g_procrdp_bThreadStart.\n" );
		}	

		//
		//	等待 g_procrdp_hkEvent 变为激发状态
		//
		//dprintf( ( "VwFirewallDrv.sys: procrdp_thread_readport_proc :: sleep %d start.\n", liTimeWait.QuadPart ) );
		ntStatus = KeWaitForSingleObject( &g_procrdp_hkEvent, Executive, KernelMode, FALSE, &liTimeWait );
		if ( NT_SUCCESS( ntStatus ) && STATUS_SUCCESS == ntStatus )
		{
			//	g_procrdp_hkEvent 变为激发状态，退出线程
			dprintf( "VwFirewallDrv.sys: procrdp_thread_readport_proc :: break and ready to exit thread.\n" );
			break;
		}

		//	设置事件句柄为未激发状态，使得 KeWaitForSingleObject 阻塞等待
		KeResetEvent( &g_procrdp_hkEvent );
	}

	//	...
	dprintf( "VwFirewallDrv.sys: procrdp_thread_readport_proc :: PsTerminateSystemThread.\n" );
	PsTerminateSystemThread( STATUS_SUCCESS );
}


/**
 *	解析 RDP 包: Client MCS Connect Initial PDU with GCC Conference Create Request
 *	此包是 CLIENT 发送给 SERVER，用来商议配置信息的第二个包
 */
BOOLEAN procrdp_parse_packet_mcscipdu( BYTE * pbtPacket, UINT uPacketLength, TS_UD_CS_CORE ** ppstTsUdCsCore )
{
	BOOLEAN bRet;
	BYTE * pbtCurrent;
	UINT uPacketMinimumSize;
	STTPKTHEADER * pstTpktHeader;
	STX224DATA * pstX224Data;

	//	for "BER: Type Length"
	STBERTYPELENGTHINFO stBERTypeLengthInfo;
	//STBERENCODEDINTEGERTYPEINFO stBEREncodedIntegerTypeInfo;
	STUSERDATAVALUELENGTHINFO stUserDataValueLengthInfo;
	BYTE btObjectLength;
	USHORT utValue;


	//	...
	*ppstTsUdCsCore		= NULL;

	if ( NULL == pbtPacket || 0 == uPacketLength || NULL == ppstTsUdCsCore )
	{
		dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 000.\n" );
		return FALSE;
	}

	//	该包可能的最小值
	uPacketMinimumSize = sizeof(STTPKTHEADER) + sizeof(STX224DATA) + sizeof(TS_UD_CS_CORE) + sizeof(TS_UD_CS_CLUSTER) + sizeof(TS_UD_CS_SEC) + 12 + 2 + 16;
	if ( uPacketLength < uPacketMinimumSize )
	{
		dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 001.\n" );
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		//
		//	parse STTPKTHEADER
		//	start  : 0
		//	length : 4
		//
		pstTpktHeader = (STTPKTHEADER*)pbtPacket;
		if ( 0x03 == pstTpktHeader->btVersion &&
			0x00 == pstTpktHeader->btReserved &&
			pstTpktHeader->uLength > uPacketMinimumSize )
		{
			//	parse STX224DATA
			//	start  : 4
			//	length : 3
			pstX224Data = (STX224DATA*)( pbtPacket + sizeof(STTPKTHEADER) );
			if ( 0x02 == pstX224Data->btLengthIndicator &&
				0xf0 == pstX224Data->btType &&
				0x80 == pstX224Data->btEOT )
			{
				//
				//	开始分析内容
				//
				//	to next	...
				pbtCurrent  = pbtPacket;
				pbtCurrent += ( sizeof(STTPKTHEADER) + sizeof(STX224DATA) );
				if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
				{
					dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 002.\n" );
					return FALSE;
				}

				//	BER: Application-Defined Type = APPLICATION 101 = Connect-Initial
				if ( 0x7f == pbtCurrent[ 0 ] && 0x65 == pbtCurrent[ 1 ] )
				{
					//	to next	...
					pbtCurrent += 2;
					if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
					{
						dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 003.\n" );
						return FALSE;
					}	

					//	BER: Type Length = 404 bytes
					//	82 01 94
					//	This is the BER encoded definite long variant of the ASN.1 length field. The long variant layout is constructed as follows:
					//
					//	  7 6 5 4 3 2 1 0     7 6 5 4 3 2 1 0         7 6 5 4 3 2 1 0
					//	+-----------------+ +-----------------+     +-----------------+
					//	| 1 (0 < n < 127) | | L L L L L L L L | ... | L L L L L L L L |
					//	+-----------------+ +-----------------+     +-----------------+
					//	         1                   2                     n + 1
					//
					//	Since the most significant bit of the first byte (0x82) is set,
					//	the low seven bits contain the number of length bytes, which means that the number of length bytes is 2.
					//	Hence 0x01 and 0x94 are length bytes,
					//	which indicates that the length is greater than 256 bytes and less than 65536 bytes, specifically 0x194 (404) bytes.
					//
					uPacketMinimumSize = sizeof(TS_UD_CS_CORE) + sizeof(TS_UD_CS_CLUSTER) + sizeof(TS_UD_CS_SEC) + 12 + 2 + 16;
					memset( &stBERTypeLengthInfo, 0, sizeof(stBERTypeLengthInfo) );
					if ( rdpstruct_get_ber_length( pbtCurrent, &stBERTypeLengthInfo ) &&
						stBERTypeLengthInfo.uByteNumber > 0 &&
						stBERTypeLengthInfo.ulValue > uPacketMinimumSize )
					{
						//	to next	...
						pbtCurrent += ( 1 + stBERTypeLengthInfo.uByteNumber );
						if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
						{
							dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 004.\n" );
							return FALSE;
						}

						//	3 bytes	- Connect-Initial::callingDomainSelector
						//	0x04 0x01 0x01
						//	The first byte (0x04) is the ASN.1 BER encoded OctetString type.
						//	The length of the data is given by the second byte (1 byte),
						//	which is encoded using the BER definite short variant of the ASN.1 length field.
						//	The third byte contains the value, which is 0x01.
						pbtCurrent += 3;
						if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
						{
							dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 005.\n" );
							return FALSE;
						}

						//	3 bytes	- Connect-Initial::calledDomainSelector
						//	0x04 0x01 0x01
						pbtCurrent += 3;
						if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
						{
							dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 006.\n" );
							return FALSE;
						}

						//	3 bytes	- Connect-Initial::upwardFlag = TRUE
						//	0x01 0x01 0xff
						//	The first byte (0x01) is the ASN.1 BER encoded Boolean type.
						//	The length of the data is given by the second byte (0x01, so the length is 1 byte).
						//	The third byte contains the value, which is 0xff (TRUE).
						pbtCurrent += 3;
						if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
						{
							dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 007.\n" );
							return FALSE;
						}

						//	Connect-Initial::targetParameters (25 bytes)
						//	2 bytes
						//	0x30 0x19
						//	The first byte (0x30) is the ASN.1 BER encoded SequenceOf type.
						//	The length of the sequence data is given by the second byte (0x19, so the length is 25 bytes).
						if ( 0x30 == pbtCurrent[ 0 ] )
						{
							pbtCurrent += (BYTE)( 2 + pbtCurrent[ 1 ] );
							if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
							{
								dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 008.\n" );
								return FALSE;
							}
						}

						//	Connect-Initial::minimumParameters (25 bytes)
						//	2 bytes
						//	0x30 0x19
						if ( 0x30 == pbtCurrent[ 0 ] )
						{
							pbtCurrent += (BYTE)( 2 + pbtCurrent[ 1 ] );
							if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
							{
								dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 009.\n" );
								return FALSE;
							}
						}
						
						//	Connect-Initial::maximumParameters (28 bytes)
						//	2 bytes
						//	0x30 0x1c
						if ( 0x30 == pbtCurrent[ 0 ] )
						{
							pbtCurrent += (BYTE)( 2 + pbtCurrent[ 1 ] );
							if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
							{
								dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 010.\n" );
								return FALSE;
							}
						}

						//	Connect-Initial::userData (307 bytes)
						//	04 82 01 33
						//	The first byte (0x04) is the ASN.1 OctetString type.
						//	The length is encoded using the BER definite long variant format.
						//	Hence, since the most significant bit of the second byte (0x82) is set,
						//	the low seven bits contain the number of length bytes, which means that the number of length bytes is 2.
						//	Hence 0x01 and 0x33 are length bytes, which indicates that the length is greater than 256 bytes and less than 65536 bytes, specifically 0x133 (307) bytes.
						if ( 0x04 == pbtCurrent[ 0 ] )
						{
							//	The first byte (0x04) is the ASN.1 OctetString type.
							pbtCurrent += 1;
							if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
							{
								dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 011.\n" );
								return FALSE;
							}

							//	BER: Type Length
							//	82 01 33
							uPacketMinimumSize = sizeof(TS_UD_CS_CORE) + sizeof(TS_UD_CS_CLUSTER) + sizeof(TS_UD_CS_SEC) + 12 + 2;
							memset( &stBERTypeLengthInfo, 0, sizeof(stBERTypeLengthInfo) );
							if ( rdpstruct_get_ber_length( pbtCurrent, &stBERTypeLengthInfo ) &&
								stBERTypeLengthInfo.uByteNumber > 0 &&
								stBERTypeLengthInfo.ulValue > uPacketMinimumSize )
							{
								//	to next	...
								pbtCurrent += ( 1 + stBERTypeLengthInfo.uByteNumber );
								if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
								{
									dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 012.\n" );
									return FALSE;
								}

								//
								//	下面就是 Connect-Initial::userData 的内容
								//	PER encoded (basic aligned variant) GCC Connection Data (ConnectData):
								//	00 05 00 14 7c 00 01 81 2a 00 08 00 10 00 01 c0
								//	00 44 75 63 61 81 1c

								//	Connect-Initial::userData config
								//	0x00 is mains:
								//	------------------------------------------------------------
								//	0 - CHOICE: From Key select object (0) of type OBJECT IDENTIFIER
								//	0 - padding
								//	0 - padding
								//	0 - padding
								//	0 - padding
								//	0 - padding
								//	0 - padding
								//	0 - padding
								pbtCurrent += 1;
								if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
								{
									dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 013.\n" );
									return FALSE;
								}

								//	object length = 5 bytes
								//	0x05
								btObjectLength = (BYTE)( pbtCurrent[ 0 ] );
								pbtCurrent += 1;
								if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
								{
									dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 014.\n" );
									return FALSE;
								}

								//	object
								//	00 14 7c 00 01
								//	The first byte gives the first two values in the sextuple (m and n), as it is encoded as
								//	40m + n. Hence, decoding the remaining data yields the correct results:
								//
								//	OID = { 0 0 20 124 0 1 } = {itu-t(0) recommendation(0) t(20) t124(124) version(0) 1}
								//	Description = v.1 of ITU-T Recommendation T.124 (Feb 1998): "Generic Conference Control"
								pbtCurrent += btObjectLength;
								if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
								{
									dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 015.\n" );
									return FALSE;
								}

								//	ConnectData::connectPDU length = 298 bytes
								//	81 2a
								//	Since the most significant bit of the first byte (0x81) is set to 1 
								//	and the following bit is set to 0,
								//	the length is given by the low six bits of the first byte and the second byte.
								//	Hence, the value is 0x12a, which is 298 bytes.
								//
								//	PER encoded (basic aligned variant) GCC Conference Create Request PDU:
								//	00 08 00 10 00 01 c0 00 44 75 63 61 81 1c
								//
								uPacketMinimumSize = sizeof(TS_UD_CS_CORE) + sizeof(TS_UD_CS_CLUSTER) + sizeof(TS_UD_CS_SEC) + 12;
								memset( &stUserDataValueLengthInfo, 0, sizeof(stUserDataValueLengthInfo) );
								if ( rdpstruct_get_userdata_value_length( pbtCurrent, &stUserDataValueLengthInfo ) &&
									stUserDataValueLengthInfo.utValue > uPacketMinimumSize )
								{
									pbtCurrent += 2;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 016.\n" );
										return FALSE;
									}

									//	...
									//	0x00:
									//	0 - extension bit (ConnectGCCPDU)
									//	0 - --\
									//	0 - | CHOICE: From ConnectGCCPDU select conferenceCreateRequest (0)
									//	0 - --/ of type ConferenceCreateRequest
									//	0 - extension bit (ConferenceCreateRequest)
									//	0 - ConferenceCreateRequest::convenerPassword present
									//	0 - ConferenceCreateRequest::password present
									//	0 - ConferenceCreateRequest::conductorPrivileges present
									pbtCurrent ++;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 017.\n" );
										return FALSE;
									}

									//	...
									//	0x08:
									//	0 - ConferenceCreateRequest::conductedPrivileges present
									//	0 - ConferenceCreateRequest::nonConductedPrivileges present
									//	0 - ConferenceCreateRequest::conferenceDescription present
									//	0 - ConferenceCreateRequest::callerIdentifier present
									//	1 - ConferenceCreateRequest::userData present
									//	0 - extension bit (ConferenceName)
									//	0 - ConferenceName::text present
									//	0 - padding
									pbtCurrent ++;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 018.\n" );
										return FALSE;
									}

									//	...
									//	0x00:
									//	0 - --\
									//	0 - |
									//	0 - |
									//	0 - | ConferenceName::numeric length = 0 + 1 = 1 character
									//	0 - | (minimum for SimpleNumericString is 1)
									//	0 - |
									//	0 - |
									//	0 - --/
									pbtCurrent ++;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 019.\n" );
										return FALSE;
									}

									//	...
									//	0x10:
									//	0 - --\
									//	0 - | ConferenceName::numeric = "1"
									//	0 - |
									//	1 - --/
									//	0 - ConferenceCreateRequest::lockedConference
									//	0 - ConferenceCreateRequest::listedConference
									//	0 - ConferenceCreateRequest::conducibleConference
									//	0 - extension bit (TerminationMethod)
									pbtCurrent ++;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 020.\n" );
										return FALSE;
									}

									//	...
									//	0x00:
									//	0 - TerminationMethod::automatic
									//	0 - padding
									//	0 - padding
									//	0 - padding
									//	0 - padding
									//	0 - padding
									//	0 - padding
									//	0 - padding
									pbtCurrent ++;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 021.\n" );
										return FALSE;
									}

									//	...
									//	0x01:
									//	0 - --\
									//	0 - |
									//	0 - |
									//	0 - | number of UserData sets = 1
									//	0 - |
									//	0 - |
									//	0 - |
									//	1 - --/
									pbtCurrent ++;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 022.\n" );
										return FALSE;
									}

									//	...
									//	0xc0:
									//	1 - UserData::value present
									//	1 - CHOICE: From Key select h221NonStandard (1) of type H221NonStandardIdentifier
									//	0 - padding
									//	0 - padding
									//	0 - padding
									//	0 - padding
									//	0 - padding
									//	0 - padding
									pbtCurrent ++;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 023.\n" );
										return FALSE;
									}

									//	...
									//	0x00:
									//	0 - --\
									//	0 - |
									//	0 - |
									//	0 - | h221NonStandard length = 0 + 4 = 4 octets
									//	0 - | (minimum for H221NonStandardIdentifier is 4)
									//	0 - |
									//	0 - |
									//	0 - --/
									//	...
									btObjectLength = (BYTE)( pbtCurrent[ 0 ] + 4 );
									pbtCurrent ++;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 024.\n" );
										return FALSE;
									}
									
									//	h221NonStandard (client-to-server H.221 key) = "Duca"
									//	44 75 63 61
									pbtCurrent += btObjectLength;
									if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 025.\n" );
										return FALSE;
									}

									//	UserData::value length = 284 bytes
									//	81 1c
									//	Since the most significant bit of the first byte (0x81) is set to 1 
									//	and the following bit is set to 0,
									//	the length is given by the low six bits of the first byte and the second byte.
									//	Hence, the value is 0x11c, which is 284 bytes.
									uPacketMinimumSize = sizeof(TS_UD_CS_CORE) + sizeof(TS_UD_CS_CLUSTER) + sizeof(TS_UD_CS_SEC);
									memset( &stUserDataValueLengthInfo, 0, sizeof(stUserDataValueLengthInfo) );
									if ( rdpstruct_get_userdata_value_length( pbtCurrent, &stUserDataValueLengthInfo ) &&
										stUserDataValueLengthInfo.utValue > uPacketMinimumSize )
									{
										pbtCurrent += 2;
										if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
										{
											dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 026.\n" );
											return FALSE;
										}

										//	TS_UD_HEADER::type = CS_CORE (0xc001), length = 216 bytes
										//	01 c0 d8 00
										utValue = MAKEWORD( pbtCurrent[0], pbtCurrent[1] );
										if ( CS_CORE == utValue )
										{
											//########################################
											//	CS_CORE
											*ppstTsUdCsCore = (TS_UD_CS_CORE*)pbtCurrent;

											utValue = MAKEWORD( pbtCurrent[2], pbtCurrent[3] );
											if ( utValue == sizeof(TS_UD_CS_CORE) || 
												utValue == ( sizeof(TS_UD_CS_CORE) - 4 ) )
											{
												//	next ...
												pbtCurrent += utValue;
												if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
												{
													dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 027.\n" );
													return FALSE;
												}											

												//	TS_UD_HEADER::type = CS_CLUSTER (0xc004), length = 12 bytes
												//	04 c0 0c 00
												utValue = MAKEWORD( pbtCurrent[0], pbtCurrent[1] );
												if ( CS_CLUSTER == utValue )
												{
													utValue = MAKEWORD( pbtCurrent[2], pbtCurrent[3] );
													if ( utValue == sizeof(TS_UD_CS_CLUSTER) )
													{
														//	next ...
														pbtCurrent += utValue;
														if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
														{
															dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 028.\n" );
															return FALSE;
														}

														//	TS_UD_HEADER::type = CS_SECURITY (0xc002), length = 12 bytes
														//	02 c0 0c 00
														utValue = MAKEWORD( pbtCurrent[0], pbtCurrent[1] );
														if ( CS_SECURITY == utValue )
														{
															utValue = MAKEWORD( pbtCurrent[2], pbtCurrent[3] );
															if ( utValue == sizeof(TS_UD_CS_SEC) )
															{
																//	next ...
																pbtCurrent += utValue;
																if ( (UINT)( pbtCurrent - pbtPacket ) >= uPacketLength )
																{
																	dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 029.\n" );
																	return FALSE;
																}

																//	TS_UD_HEADER::type = CS_NET (0xc003), length = 44 bytes
																utValue = MAKEWORD( pbtCurrent[0], pbtCurrent[1] );
																if ( CS_NET == utValue )
																{
																	//	get the length of CS_NET
																	utValue = MAKEWORD( pbtCurrent[2], pbtCurrent[3] );

																	//
																	//	数据包结束了
																	//
																	bRet = TRUE;
																}
																else
																{
																	dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 030.\n" );
																}	
															}
															else
															{
																dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 031.\n" );
															}	
														}
														else
														{
															dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 032.\n" );
														}	
													}
													else
													{
														dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 033.\n" );
													}	
												}
												else
												{
													dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 034.\n" );
												}	
											}
											else
											{
												dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 035.\n" );
											}	
										}
										else
										{
											dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 036.\n" );
										}	
									}
									else
									{
										dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 037.\n" );
									}	
								} // end of ConnectData::connectPDU length = 298 bytes
								else
								{
									dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 038.\n" );
								}	
							}
							else
							{
								dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 039.\n" );
							}
						}
						else
						{
							dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 040.\n" );
						}	


						//	DomainParameters::protocolVersion = 2
						//	0x02 0x01 0x02
						//memset( &stBEREncodedIntegerTypeInfo, 0, sizeof(stBEREncodedIntegerTypeInfo) );
						//if ( rdpstruct_get_ber_encoded_integer_type( pbtCurrent, &stBEREncodedIntegerTypeInfo ) &&
						//	stBEREncodedIntegerTypeInfo.uByteNumber > 0 &&
						//	stBEREncodedIntegerTypeInfo.ulValue > 0 )
						//{
						//	//	to next	...
						//	pbtCurrent += stBEREncodedIntegerTypeInfo.uByteNumber;
						//}
					}
					else
					{
						dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 041.\n" );
					}	
				}
				else
				{
					dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 042.\n" );
				}	
			}
			else
			{
				dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 043.\n" );
			}	
		}
		else
		{
			dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit 044.\n" );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		dprintf( "VwFirewallDrv.sys: procrdp_parse_packet_mcscipdu :: exit EXCEPTION_EXECUTE_HANDLER.\n" );
	}

	return bRet;
}


/**
 *	判断端口是否存在
 */
BOOLEAN procrdp_isexist_rdp_port( USHORT uSrvPort )
{
	BOOLEAN bRet;
	INT i;

	bRet = FALSE;

	__try
	{
		for ( i = 0; i < g_procrdp_nSrvPortCount; i ++ )
		{
			if ( uSrvPort == g_procrdp_ArrSrvPort[ i ] )
			{
				bRet = TRUE;
				break;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: procrdp_isexist_rdp_port"));
	}	

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//	Private


/**
 *	添加新端口
 */
VOID procrdp_thread_readport_pushnew( USHORT uSrvPort )
{
	if ( ! procrdp_isexist_rdp_port( uSrvPort ) )
	{
		if ( g_procrdp_nSrvPortCount < g_procrdp_nSrvPortMaxSize )
		{
			g_procrdp_ArrSrvPort[ g_procrdp_nSrvPortCount ] = uSrvPort;
			g_procrdp_nSrvPortCount ++;
		}
	}
}