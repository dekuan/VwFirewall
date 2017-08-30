// Driver.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Driver.h"


/**
 *	全局变量
 */
STSYSINFO g_stSysInfo;
PDRIVER_OBJECT g_pstDriverObject		= NULL;		//	我们自身驱动对象
PDEVICE_OBJECT g_pstControlDeviceObject		= NULL;		//	我们自身设备对象

HANDLE g_ipfirewall_hThread			= NULL;
KEVENT g_ipfirewall_hkEvent;
BOOLEAN g_ipfirewall_bThreadStart		= FALSE;


/**
 *	DriverEntry
 */
NTSTATUS DriverEntry( IN PDRIVER_OBJECT pstDriverObject, IN PUNICODE_STRING pusRegistryPath )
{
	NTSTATUS ntStatus;
	UNICODE_STRING usdeviceName;
	UNICODE_STRING usDeviceLink;
	
	dprintf( "VwFirewallDrv.sys: Loading Driver....\n" );
	
	
	//
	//	保存全局数据
	//
	g_pstDriverObject = pstDriverObject;
	
	
	//
	//	Create the device
	//
	RtlInitUnicodeString( &usdeviceName, NT_DEVICE_NAME );
	
	ntStatus = IoCreateDevice( pstDriverObject, 0, &usdeviceName, VWFIREWALLDRV_FILE_DEVICE, 0, FALSE, &g_pstControlDeviceObject );
	if ( NT_SUCCESS( ntStatus ) )
	{
		//
		//	设置 IOCTL 读写方式
		//
		g_pstControlDeviceObject->Flags |= DO_BUFFERED_IO;
		
		//	Create symbolic link
		RtlInitUnicodeString( &usDeviceLink, DOS_DEVICE_NAME );
		
		ntStatus = IoCreateSymbolicLink( &usDeviceLink, &usdeviceName );
		if ( NT_SUCCESS( ntStatus ) )
		{
			//
			//	初始化驱动数据
			//
			DrvDataInit( pstDriverObject );
		}
		else
		{
			dprintf( "VwFirewallDrv.sys: Error in IoCreateSymbolicLink.\n" );
			IoDeleteDevice( pstDriverObject->DeviceObject );
		}

		//
		//	Define driver's control functions
		//
		pstDriverObject->MajorFunction[ IRP_MJ_CREATE ]		= DrvDispatch;
		pstDriverObject->MajorFunction[ IRP_MJ_CLOSE ]		= DrvDispatch;
		pstDriverObject->MajorFunction[ IRP_MJ_DEVICE_CONTROL ]	= DrvDispatch;
		pstDriverObject->DriverUnload				= DrvUnload;


		//
		//	how-to-find-process-full-image-name
		//	http://84.45.57.224/how-to-find-process-full-image-name_topic9419_post40525.html
		//
		//	retrieving full process image path name in kernel mode
		//	http://rootkit.com/board.php?thread=5660&did=proj5&disp=5660&closed=1
		//	
		//	PsSetLoadImageNotifyRoutine
		//	http://msdn.microsoft.com/en-us/library/ff559957(VS.85).aspx
		//	http://hi.baidu.com/%B7%FA%CD%DE/blog/item/f223b18951054dbb0e24442f.html
		//
		//	PsSetCreateProcessNotifyRoutine
		//	http://msdn.microsoft.com/en-us/library/ff559951(v=VS.85).aspx
		//
		PsSetCreateProcessNotifyRoutine( procprocess_CreateProcessNotifyRoutine, FALSE );
		PsSetLoadImageNotifyRoutine( procprocess_LoadImageNotifyRoutine );
	}
	
	if ( ! NT_SUCCESS( ntStatus ) )
	{
		dprintf( "Error Intitializing. Unloading driver..." );
		DrvUnload( pstDriverObject );
	}
	
	return ntStatus;
}

/**
 *	Unload the driver. Free all resources.
 */
VOID DrvUnload( IN PDRIVER_OBJECT pstDriverObject )
{
	//
	//	pstDriverObject - Pointer to driver object.
	//

	UNICODE_STRING usDeviceLink;

	dprintf( "VwFirewallDrv.sys: Unloading driver...\n" );

	//
	//	Uninstall Filesystem Api Hooks
	//
	fsapihook_hook_uninstall();

	//
	//	结束 RDP 端口读取线程
	//
	procrdp_thread_readport_terminate();


	//
	//	卸载防火墙回调函数
	//
	if ( g_stSysInfo.bFirewallLoaded )
	{
		if ( NT_SUCCESS( SetFilterFunction( callbackFilterFunction, FALSE ) ) )
		{
			g_stSysInfo.bFirewallLoaded = FALSE;
		}
	}

	//
	//	卸载配置信息
	//
	procconfig_unload_config();



	//
	//	Remove CreateProcessNotifyRoutine & LoadImageNotifyRoutine
	//
	PsSetCreateProcessNotifyRoutine( procprocess_CreateProcessNotifyRoutine, TRUE );
	PsRemoveLoadImageNotifyRoutine( procprocess_LoadImageNotifyRoutine );


	//
	//	Remove symbolic link
	//
	RtlInitUnicodeString( &usDeviceLink, DOS_DEVICE_NAME );
	IoDeleteSymbolicLink( &usDeviceLink );

	//
	//	Remove the device
	//
	IoDeleteDevice( pstDriverObject->DeviceObject );
}

/**
 *	Process all IRPs the driver receive
 */
NTSTATUS DrvDispatch( IN PDEVICE_OBJECT pstDeviceObject, IN PIRP pstIrp )
{
	//
	//	pstDeviceObject - Pointer to device object
	//	pstIrp		- IRP
	//

	PIO_STACK_LOCATION pstIrpStack;
	PVOID pvIoBuffer;
	ULONG ulInputBufferLength;
	ULONG ulOutputBufferLength;
	ULONG ulIoControlCode;
	ULONG ulStart;
	NTSTATUS ntStatus;
	INT   i;

	//	...
	pstIrp->IoStatus.Status		= STATUS_SUCCESS;
	pstIrp->IoStatus.Information	= 0;

	//	Get pointer to current stack location
	pstIrpStack = IoGetCurrentIrpStackLocation( pstIrp );

	//	Get pointer to buffer information
	pvIoBuffer		= pstIrp->AssociatedIrp.SystemBuffer;
	ulInputBufferLength	= pstIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	ulOutputBufferLength	= pstIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	switch ( pstIrpStack->MajorFunction )
	{
	case IRP_MJ_CREATE:
		{
			dprintf( "VwFirewallDrv.sys: IRP_MJ_CREATE\n" );
		}
		break;

	case IRP_MJ_CLOSE:
		{
			dprintf( "VwFirewallDrv.sys: IRP_MJ_CLOSE\n" );
		}
		break;

	case IRP_MJ_DEVICE_CONTROL:
		{
			dprintf( "DrvFltIp.SYS: IRP_MJ_DEVICE_CONTROL\n" );
			
			ulIoControlCode = pstIrpStack->Parameters.DeviceIoControl.IoControlCode;
			switch ( ulIoControlCode )
			{
			case VWFIREWALLDRV_IOCTL_START:
				{
					if ( g_stSysInfo.bFirewallLoaded )
					{
						procconfig_start();
					}
				}
				break;

			case VWFIREWALLDRV_IOCTL_STOP:
				{
					//	IOCTL to delete filter function.
					if ( g_stSysInfo.bFirewallLoaded )
					{
						procconfig_stop();
					}
				}
				break;

			case VWFIREWALLDRV_IOCTL_ISSTART:
				{
					if ( MmIsAddressValid( pvIoBuffer ) && sizeof(ULONG) == ulOutputBufferLength )
					{
						ulStart = g_stSysInfo.bFirewallLoaded ? 1 : 0;
						RtlCopyMemory( pvIoBuffer, &ulStart, sizeof(ULONG) ); 
						pstIrp->IoStatus.Information = sizeof(ULONG);
					}
				}
				break;

			case VWFIREWALLDRV_IOCTL_PFILE_BLOCKCOUNT:
				{
					//
					//	获取拦截次数: 文件锁
					//	LONGLONG
					//
					if ( MmIsAddressValid( pvIoBuffer ) && sizeof(LONGLONG) == ulOutputBufferLength )
					{
						RtlCopyMemory( pvIoBuffer, &g_stCfg.llProtectFileBlockCount, sizeof(LONGLONG) ); 
						pstIrp->IoStatus.Information = sizeof(LONGLONG);
					}
				}
				break;

			case VWFIREWALLDRV_IOCTL_PDOMAIN_BLOCKCOUNT:
				{
					//
					//	获取拦截次数: 域名白名单
					//	LONGLONG
					//
					if ( MmIsAddressValid( pvIoBuffer ) && sizeof(LONGLONG) == ulOutputBufferLength )
					{
						RtlCopyMemory( pvIoBuffer, &g_stCfg.llProtectDomainBlockCount, sizeof(LONGLONG) ); 
						pstIrp->IoStatus.Information = sizeof(LONGLONG);
					}
				}
				break;

			case VWFIREWALLDRV_IOCTL_PRDP_BLOCKCOUNT:
				{
					//
					//	获取拦截次数: 远程桌面
					//	LONGLONG
					//
					if ( MmIsAddressValid( pvIoBuffer ) && sizeof(LONGLONG) == ulOutputBufferLength )
					{
						RtlCopyMemory( pvIoBuffer, &g_stCfg.llProtectRdpBlockCount, sizeof(LONGLONG) ); 
						pstIrp->IoStatus.Information = sizeof(LONGLONG);
					}
				}
				break;

			case VWFIREWALLDRV_IOCTL_PFILE_CFGCOUNT:
				{
					//
					//	获取当前配置多少项: 文件锁
					//	LONGLONG
					//
					if ( MmIsAddressValid( pvIoBuffer ) && sizeof(UINT) == ulOutputBufferLength )
					{
						g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uProtedDirTotalCount = g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uProtedDirTreeCount + g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uProtedDirSingleCount;
						RtlCopyMemory( pvIoBuffer, &( g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uProtedDirTotalCount ), sizeof(UINT) ); 
						pstIrp->IoStatus.Information = sizeof(UINT);
					}
				}
				break;

			case VWFIREWALLDRV_IOCTL_PDOMAIN_CFGCOUNT:
				{
					//
					//	获取当前配置多少项: 域名白名单
					//	LONGLONG
					//
					if ( MmIsAddressValid( pvIoBuffer ) && sizeof(UINT) == ulOutputBufferLength )
					{
						RtlCopyMemory( pvIoBuffer, &(g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uDomainSingleCount), sizeof(UINT) ); 
						pstIrp->IoStatus.Information = sizeof(UINT);
					}
				}
				break;
	
			case VWFIREWALLDRV_IOCTL_PRDP_CFGCOUNT:
				{
					//
					//	获取当前配置多少项: 远程桌面
					//	LONGLONG
					//
					if ( MmIsAddressValid( pvIoBuffer ) && sizeof(UINT) == ulOutputBufferLength )
					{
						RtlCopyMemory( pvIoBuffer, &(g_stCfg.stCfgData[ g_stCfg.uCurrIndex ].uRdpClientNameSingleCount), sizeof(UINT) ); 
						pstIrp->IoStatus.Information = sizeof(UINT);
					}
				}
				break;

			case VWFIREWALLDRV_IOCTL_LOADCFG:
				{
					//	通知下，要装在配置了，从配置文件装载吧
					if ( g_stSysInfo.bInitSucc )
					{
						//
						//	确定是否是正式版本
						//
						g_stCfg.bReged = FALSE;

						//if ( ulInputBufferLength == 32 )
						//{
						//	if ( 0 == _stricmp( "p1x", (CHAR*)pvIoBuffer ) )
						//	{
						//		//	正式版本
						//		g_stCfg.bReged = TRUE;
						//	}
						//}

						//	预装载配置信息，为 procconfig_is_valid_key 提供数据
						procconfig_load_config( g_stSysInfo.wszConfigFilename );

						//	验证是否是正式版本
						g_stCfg.bReged		= procconfig_is_valid_key( g_stSysInfo.wszKeyFilename );
						g_stCfg.bValidPrType	= procconfig_is_valid_prtype_checksum( g_stCfg.szRegPrTypeChecksum );

						//	再次，真正装载配置文件
						procconfig_load_config( g_stSysInfo.wszConfigFilename );
					}
					else
					{
						dprintf( ("VwFirewallDrv.sys: system init failed, failed to load config.\n") );
					}
				}
				break;

			default:
				{
					pstIrp->IoStatus.Status = STATUS_INVALID_PARAMETER;
					dprintf( ("VwFirewallDrv.sys: Unknown IOCTL.\n") );
				}
				break;
			}
		}
		break;
	}


	//
	//	We can't return pstIrp-IoStatus directly because after IoCompleteRequest
	//	we aren't the owners of the IRP.
	//
	ntStatus = pstIrp->IoStatus.Status;
	IoCompleteRequest( pstIrp, IO_NO_INCREMENT );

	return ntStatus;
}



/**
 *	初始化驱动数据
 */
VOID DrvDataInit( IN PDRIVER_OBJECT pDriverObject )
{
	INT i;

	__try
	{
		g_stSysInfo.bInitSucc		= FALSE;
		g_stSysInfo.bInitLoadConfigSucc	= FALSE;
		g_stSysInfo.bFirewallLoaded	= FALSE;

		//
		//	C:\WINDOWS\system32\drivers
		//	get windows dir
		//
		if ( drvfunc_get_windowsdir( pDriverObject, g_stSysInfo.wszWindowsDir, sizeof(g_stSysInfo.wszWindowsDir) ) )
		{
			g_stSysInfo.bInitSucc	= TRUE;

			_snwprintf( g_stSysInfo.wszSystem32Dir, sizeof(g_stSysInfo.wszSystem32Dir)/sizeof(WCHAR)-1,
				L"%ssystem32\\", g_stSysInfo.wszWindowsDir );
			
			_snwprintf( g_stSysInfo.wszDriversDir, sizeof(g_stSysInfo.wszDriversDir)/sizeof(WCHAR)-1,
				L"%sdrivers\\", g_stSysInfo.wszSystem32Dir );
			
			_snwprintf( g_stSysInfo.wszDriverFilename, sizeof(g_stSysInfo.wszDriverFilename)/sizeof(WCHAR)-1,
				L"%s%s.sys", g_stSysInfo.wszDriversDir, DEVIE_NAMEW );

			_snwprintf( g_stSysInfo.wszConfigFilename, sizeof(g_stSysInfo.wszConfigFilename)/sizeof(WCHAR)-1,
				L"%s%sConfig.ini", g_stSysInfo.wszDriversDir, DEVIE_NAMEW );

			_snwprintf( g_stSysInfo.wszKeyFilename, sizeof(g_stSysInfo.wszKeyFilename)/sizeof(WCHAR)-1,
				L"%s%s.dll", g_stSysInfo.wszDriversDir, DEVIE_NAMEW );
		}

		if ( g_stSysInfo.bInitSucc )
		{
			//
			//	初始化配置信息
			//
			if ( procconfig_init_config() )
			{
				//
				//	预装载配置信息，为 procconfig_is_valid_key 提供数据
				//
				if ( procconfig_load_config( g_stSysInfo.wszConfigFilename ) )
				{
					//
					//	验证是否是正式版本
					//
					g_stCfg.bReged		= procconfig_is_valid_key( g_stSysInfo.wszKeyFilename );
					g_stCfg.bValidPrType	= procconfig_is_valid_prtype_checksum( g_stCfg.szRegPrTypeChecksum );

					//
					//	再次，真正装载配置文件
					//
					if ( procconfig_load_config( g_stSysInfo.wszConfigFilename ) )
					{
						g_stSysInfo.bInitLoadConfigSucc = TRUE;

						//
						//	安装驱动程序相关的其他东西
						//
						DrvInstallFunc( g_pstControlDeviceObject );
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: DrvDataInit\n" ));
	}	
}



/**
*	安装驱动程序相关的其他东西
*/
VOID DrvInstallFunc( IN PDEVICE_OBJECT pDeviceObject )
{
	INT i;

	//if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	//{
	//	return;
	//}
	if ( NULL == pDeviceObject )
	{
		return;
	}
	
	__try
	{
		
		//
		//	创建并开启 ipfirewall 安装线程
		//
		if ( ipfirewall_create_install_thread() )
		{
			ipfirewall_start_install_thread();
		}
		
		//
		//	开始 RDP 端口读取线程
		//
		if ( procrdp_thread_readport_create() )
		{
			procrdp_thread_readport_start();
		}
		
		//
		//	创建 LOGGER 线程
		//
		//drvthreadlogger_create_thread();


		//
		//	Install FsApiHook
		//
		fsapihook_set_config
		(
			FSAPIHOOKFS_CFG_ZWCREATEFILE |
			FSAPIHOOKFS_CFG_ZWWRITEFILE |
			FSAPIHOOKFS_CFG_ZWSETINFORMATIONFILE |
			FSAPIHOOKFS_CFG_ZWCREATEDIRECTORYOBJECT
		);
		fsapihook_hook_install();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: DrvInstallFunc\n" ));
	}
}



/**
 *	Install/Uninstall the filter function
 */
NTSTATUS SetFilterFunction( IPPacketFirewallPtr pfnFilterFunction, BOOLEAN bLoad )
{
	//
	//	pfnFilterFunction	- [in] Pointer to the filter function.
	//	loa			- [in] If TRUE, the function is added. Else, the function will be removed.
	//	RETURN			- STATUS_SUCCESS If success,
	//

	NTSTATUS ntStatus		= STATUS_SUCCESS;
	NTSTATUS ntWaitStatus		= STATUS_SUCCESS;
	UNICODE_STRING usFilterName;
	PDEVICE_OBJECT pstIpDeviceObject	= NULL;
	PFILE_OBJECT pstIpFileObject	= NULL;
	IP_SET_FIREWALL_HOOK_INFO stFilterData;

	KEVENT oEvent;
	IO_STATUS_BLOCK stIoStatus;
	PIRP pstIrp;

	__try
	{
		//	Get pointer to Ip device
		RtlInitUnicodeString( &usFilterName, DD_IP_DEVICE_NAME );
		ntStatus = IoGetDeviceObjectPointer( &usFilterName, STANDARD_RIGHTS_ALL, &pstIpFileObject, &pstIpDeviceObject );
		if ( NT_SUCCESS( ntStatus ) )
		{
			//	Init firewall hook structure
			memset( &stFilterData, 0, sizeof(stFilterData) );
			stFilterData.FirewallPtr	= pfnFilterFunction;
			stFilterData.Priority		= 1;
			stFilterData.Add		= bLoad;

			KeInitializeEvent( &oEvent, NotificationEvent, FALSE );

			//	Build pstIrp to establish filter function
			pstIrp = IoBuildDeviceIoControlRequest
				(
					IOCTL_IP_SET_FIREWALL_HOOK,
			  		pstIpDeviceObject,
					(PVOID)&stFilterData,
					sizeof(IP_SET_FIREWALL_HOOK_INFO),
					NULL,
					0,
					FALSE,
					&oEvent,
					&stIoStatus
				);
			if ( pstIrp )
			{
				//	Send the pstIrp and wait for its completion
				ntStatus = IoCallDriver( pstIpDeviceObject, pstIrp );
				if ( STATUS_PENDING == ntStatus ) 
				{
					ntWaitStatus = KeWaitForSingleObject( &oEvent, Executive, KernelMode, FALSE, NULL);
					if ( STATUS_SUCCESS != ntWaitStatus )
					{
						dprintf( "VwFirewallDrv.sys: Error waiting for Ip Driver." );
					}
				}
				ntStatus = stIoStatus.Status;
				if ( ! NT_SUCCESS( ntStatus ) )
				{
					dprintf( "VwFirewallDrv.sys: E/S error with Ip Driver\n" );
				}
			}
			else
			{
				ntStatus = STATUS_INSUFFICIENT_RESOURCES;
				dprintf( "VwFirewallDrv.sys: Error creating the IRP\n" );
			}

			//	Free resources
			if ( pstIpFileObject )
			{
				ObDereferenceObject( pstIpFileObject );
			}

			pstIpFileObject	= NULL;
			pstIpDeviceObject	= NULL;
		}
		else
		{
			dprintf( "VwFirewallDrv.sys: Error getting pointer to Ip driver.\n" );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: SetFilterFunction\n" ));
	}

	return ntStatus;
}


/**
 *	防火墙回调函数
 *	Firewall Hook filter function.
 */
FORWARD_ACTION callbackFilterFunction(
	VOID **		pvData,
	UINT		uRecvInterfaceIndex,
	UINT *		puSendInterfaceIndex,
	UCHAR *		puszDestinationType,
	VOID *		pvContext,
	UINT		uContextLength,
	struct IPRcvBuf ** pRcvBuf )
{
	//
	//	RETURN		- FORWARD = 0, DROP = 1
	//

	struct IPRcvBuf * pstBuffer		= NULL;
	PFIREWALL_CONTEXT_T pstFwContext	= NULL;
	FORWARD_ACTION faRet;

	UCHAR *puszFullPacket;
	UINT   uFullPacketSize;
	UINT   uOffset;


	//	...
	faRet = FORWARD;

	__try
	{
		if ( procconfig_is_start() )
		{
			//
			//	...
			//
			pstBuffer	= (struct IPRcvBuf *)*pvData;
			pstFwContext	= (PFIREWALL_CONTEXT_T)pvContext;

			if ( pstFwContext && IP_RECEIVE == pstFwContext->Direction &&
				pstBuffer && pstBuffer->ipr_buffer && pstBuffer->ipr_size > 100 )
			{
				if ( 0 && pstBuffer->ipr_next )
				{
					//////////////////////////////////////////////////
					//	这里故意废弃不用，怕性能太差
					//////////////////////////////////////////////////

					//	计算缓冲区的总尺寸
					uFullPacketSize = pstBuffer->ipr_size;
					while ( pstBuffer->ipr_next )
					{
						pstBuffer	= pstBuffer->ipr_next;
						uFullPacketSize += pstBuffer->ipr_size;
					}

					//
					//	Reserve memory for the complete packet.
					//
					puszFullPacket = (CHAR*)ExAllocatePool( NonPagedPool, uFullPacketSize );
					if ( puszFullPacket )
					{
						//dprintf( "VwFirewallDrv.sys: callbackFilterFunction :: uFullPacketSize=%d.\n", uFullPacketSize );

						//
						//	将所有数据拷贝到 
						//
						uOffset		= 0;
						pstBuffer	= (struct IPRcvBuf *)*pvData;
						memcpy( puszFullPacket, pstBuffer->ipr_buffer, pstBuffer->ipr_size );
						while ( pstBuffer->ipr_next )
						{
							uOffset		+= pstBuffer->ipr_size;
							pstBuffer	= pstBuffer->ipr_next;
							memcpy( puszFullPacket + uOffset, pstBuffer->ipr_buffer, pstBuffer->ipr_size );
						}

						//
						//	分析每个 RECEIVE 的 IP 包
						//
						if ( ! procpacket_ip_is_allowed_packet( puszFullPacket, uFullPacketSize ) )
						{
							//	拦截掉
							faRet = DROP;
						}

						//	free
						ExFreePool( puszFullPacket );
						puszFullPacket = NULL;
					}
				}
				else
				{
					puszFullPacket	= pstBuffer->ipr_buffer;
					uFullPacketSize	= pstBuffer->ipr_size;

					//
					//	分析每个 RECEIVE 的 IP 包
					//
					if ( ! procpacket_ip_is_allowed_packet( puszFullPacket, uFullPacketSize ) )
					{
						//	拦截掉
						faRet = DROP;
					}
				}	
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: callbackFilterFunction"));
	}	

	return faRet;
}




/**
 *	创建线程
 */
BOOLEAN ipfirewall_create_install_thread()
{
	NTSTATUS ntStatus;

	__try
	{
		//	初始化事件句柄
		KeInitializeEvent( &g_ipfirewall_hkEvent, NotificationEvent, FALSE );

		//	设置事件句柄为未激发状态，使得 KeWaitForSingleObject 阻塞等待
		KeResetEvent( &g_ipfirewall_hkEvent );

		//	是否开始
		g_ipfirewall_bThreadStart = FALSE;

		//	创建线程
		ntStatus = PsCreateSystemThread( &g_ipfirewall_hThread, 0, NULL, NULL, NULL, ipfirewall_install_thread_proc, NULL );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: ipfirewall_create_install_thread"));
	}

	//	...
	return NT_SUCCESS( ntStatus );
}
VOID ipfirewall_start_install_thread()
{
	g_ipfirewall_bThreadStart = TRUE;
}
VOID ipfirewall_stop_install_thread()
{
	g_ipfirewall_bThreadStart = FALSE;
}
VOID ipfirewall_terminate_install_thread()
{
	__try
	{
		//	设置事件句柄为激发状态
		KeSetEvent( &g_ipfirewall_hkEvent, IO_NO_INCREMENT, FALSE );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: ipfirewall_terminate_install_thread"));
	}
}
VOID ipfirewall_install_thread_proc( IN PVOID pContext )
{
	NTSTATUS ntStatus;
	LARGE_INTEGER liTimeWait;	

	__try
	{
		//
		//	间隔 3 秒
		//	-30000000
		//
		liTimeWait.QuadPart = 3 * 10 * 1000 * 1000 * ( -1 );

		while ( TRUE )
		{
			//
			//	尝试安装防火墙过滤器
			//
			if ( g_ipfirewall_bThreadStart )
			{
				dprintf( "VwFirewallDrv.sys: ipfirewall_install_thread_proc :: try to install ipfirewall filter.\n" );

				if ( ! g_stSysInfo.bFirewallLoaded )
				{
					if ( NT_SUCCESS( SetFilterFunction( callbackFilterFunction, TRUE ) ) )
					{
						g_stSysInfo.bFirewallLoaded = TRUE;
						break;
					}
					else
					{
						dprintf( "VwFirewallDrv.sys: ipfirewall_install_thread_proc :: falied to install ipfirewall filter.\n" );
					}
				}
			}
			else
			{
				dprintf( "VwFirewallDrv.sys: ipfirewall_install_thread_proc :: thread stopped.\n" );
			}

			//
			//	等待 g_ipfirewall_hkEvent 变为激发状态
			//
			//dprintf( ( "VwFirewallDrv.sys: ipfirewall_install_thread_proc :: sleep %d start.\n", liTimeWait.QuadPart ) );
			ntStatus = KeWaitForSingleObject( &g_ipfirewall_hkEvent, Executive, KernelMode, FALSE, &liTimeWait );
			if ( NT_SUCCESS( ntStatus ) && STATUS_SUCCESS == ntStatus )
			{
				//	g_ipfirewall_hkEvent 变为激发状态，退出线程
				dprintf( "VwFirewallDrv.sys: ipfirewall_install_thread_proc :: break and ready to exit thread.\n" );
				break;
			}

			//	设置事件句柄为未激发状态，使得 KeWaitForSingleObject 阻塞等待
			KeResetEvent( &g_ipfirewall_hkEvent );
		}

		//	...
		dprintf( "VwFirewallDrv.sys: ipfirewall_install_thread_proc :: PsTerminateSystemThread.\n" );
		PsTerminateSystemThread( STATUS_SUCCESS );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: ipfirewall_install_thread_proc"));
	}
}