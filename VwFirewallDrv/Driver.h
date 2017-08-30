//	VwFirewallDrv.h
//////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <ntddk.h>
#include <ndis.h>
#include <ipFirewall.h>

#include "VwFirewallDrvIOCtl.h"
#include "ProcPacket.h"
#include "FsApiHook.h"



/**
 *	structure to define system info
 */
typedef struct tagSysInfo
{
	BOOL  bInitSucc;
	BOOL  bInitLoadConfigSucc;
	BOOL  bFirewallLoaded;
	WCHAR wszWindowsDir[ MAX_PATH ];
	WCHAR wszSystem32Dir[ MAX_PATH ];
	WCHAR wszDriversDir[ MAX_PATH ];
	WCHAR wszDriverFilename[ MAX_PATH ];
	WCHAR wszConfigFilename[ MAX_PATH ];
	WCHAR wszKeyFilename[ MAX_PATH ];

}STSYSINFO, *PSTSYSINFO;





/**
 *	全局变量
 */
extern STSYSINFO g_stSysInfo;
extern PDRIVER_OBJECT g_pstDriverObject;		//	我们自身驱动对象
extern PDEVICE_OBJECT g_pstControlDeviceObject;		//	我们自身设备对象

extern HANDLE g_ipfirewall_hThread;
extern KEVENT g_ipfirewall_hkEvent;
extern BOOLEAN g_ipfirewall_bThreadStart;



/**
 *	Function declarations
 */
NTSTATUS DriverEntry( IN PDRIVER_OBJECT pstDriverObject, IN PUNICODE_STRING pusRegistryPath );
NTSTATUS DrvDispatch( IN PDEVICE_OBJECT pstDeviceObject, IN PIRP pstIrp );
VOID     DrvUnload( IN PDRIVER_OBJECT pstDriverObject );

VOID     DrvDataInit( IN PDRIVER_OBJECT pDriverObject );
VOID	 DrvInstallFunc( IN PDEVICE_OBJECT pDeviceObject );
NTSTATUS SetFilterFunction( IPPacketFirewallPtr pfnFilterFunction, BOOLEAN bLoad );


//	防火墙回调函数
FORWARD_ACTION callbackFilterFunction(
	VOID **		pData,
	UINT		RecvInterfaceIndex,
	UINT *		pSendInterfaceIndex,
	UCHAR *		pDestinationType,
	VOID *		pContext,
	UINT		ContextLength,
	struct IPRcvBuf ** pRcvBuf
	);

BOOLEAN ipfirewall_create_install_thread();
VOID ipfirewall_start_install_thread();
VOID ipfirewall_stop_install_thread();
VOID ipfirewall_terminate_install_thread();
VOID ipfirewall_install_thread_proc( IN PVOID pContext );

//////////////////////////////////////////////////////////////////////////
//
	
	


