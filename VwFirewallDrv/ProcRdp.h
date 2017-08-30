// ProcRdp.h: interface for the DrvThread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCRDP_HEADER__
#define __PROCRDP_HEADER__

#include "RdpStruct.h"



/**
 *	宏定义
 */
#define PROCRDP_RDP_PORTNUMBER_REGPATH		"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Wds\\rdpwd\\Tds\\tcp"
#define PROCRDP_RDP_PORTNUMBER_REGKEYNAME	"PortNumber"


/**
 *	全局变量定义
 */
extern HANDLE  g_procrdp_hThread;
extern KEVENT  g_procrdp_hkEvent;
extern BOOLEAN g_procrdp_bThreadStart;
extern USHORT  g_procrdp_srvport;
extern USHORT  g_procrdp_ArrSrvPort[ 1024 ];
extern INT     g_procrdp_nSrvPortMaxSize;
extern INT     g_procrdp_nSrvPortCount;

/**
 *	Public
 */
BOOLEAN procrdp_thread_readport_create();
VOID    procrdp_thread_readport_terminate();
VOID    procrdp_thread_readport_start();
VOID    procrdp_thread_readport_stop();
VOID    procrdp_thread_readport_proc( IN PVOID pContext );

BOOLEAN procrdp_parse_packet_mcscipdu( BYTE * pbtPacket, UINT uPacketLength, TS_UD_CS_CORE ** ppstTsUdCsCore );
BOOLEAN procrdp_isexist_rdp_port( USHORT uSrvPort );

/**
 *	Private
 */
VOID    procrdp_thread_readport_pushnew( USHORT uSrvPort );











#endif // __PROCRDP_HEADER__
