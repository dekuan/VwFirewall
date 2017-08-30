// ProcProcess.cpp: implementation of the ProcProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcProcess.h"
#include "DrvFunc.h"
#include "Crc32.h"


/**
 *	...
 */
STLAUNCHEDPROCESS g_ArrLaunchedProcess[ PROCPROCESS_MAX_LAUNCHED_PROCESS_ARRSIZE ];


/**
 *	Init ...
 */
VOID procprocess_init()
{
	RtlZeroMemory( &g_ArrLaunchedProcess, sizeof(g_ArrLaunchedProcess) );
}

/**
 *	根据 PID 获取
 */
BOOLEAN procprocess_get_fullimagepath_by_pid( ULONG ulProcessId, WCHAR ** ppwszFullImagePath, USHORT * puFullImagePathLength, ULONG * pulCrc32U )
{
	BOOLEAN bRet;
	INT i;

	if ( 0 == ulProcessId )
	{
		return FALSE;
	}
	if ( NULL == ppwszFullImagePath || NULL == puFullImagePathLength || NULL == pulCrc32U )
	{
		return FALSE;
	}

	bRet = FALSE;

	__try
	{
		for ( i = 0; i < PROCPROCESS_MAX_LAUNCHED_PROCESS_ARRSIZE; i ++ )
		{
			if ( ulProcessId == g_ArrLaunchedProcess[ i ].ulProcessID )
			{
				*ppwszFullImagePath	= g_ArrLaunchedProcess[ i ].wszFullImagePath;
				*puFullImagePathLength	= g_ArrLaunchedProcess[ i ].uLengthInBytesW;
				*pulCrc32U		= g_ArrLaunchedProcess[ i ].ulCrc32U;

				bRet = TRUE;
				break;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procprocess_get_fullimagepath_by_pid\n" ));
	}

	return bRet;
}

//	http://msdn.microsoft.com/en-us/library/ff559957(VS.85).aspx
VOID procprocess_CreateProcessNotifyRoutine( IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN bCreate )
{
	INT i;
	ULONG ulProcessIdTmp;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return;
	}
	
	__try
	{
		if ( bCreate )
		{
			//	notified when system create a process
		}
		else
		{
			//	notify when system terminate a process
			ulProcessIdTmp = 0;
			RtlMoveMemory( &ulProcessIdTmp, &ProcessId, sizeof(ProcessId) );
			for ( i = 0; i < PROCPROCESS_MAX_LAUNCHED_PROCESS_ARRSIZE; i ++ )
			{
				if ( ulProcessIdTmp == g_ArrLaunchedProcess[ i ].ulProcessID )
				{
					g_ArrLaunchedProcess[ i ].ulProcessID = 0;
					break;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procprocess_CreateProcessNotifyRoutine\n" ));
	}
}

VOID procprocess_LoadImageNotifyRoutine( IN OPTIONAL PUNICODE_STRING pusFullImageName, IN HANDLE ProcessId, IN PIMAGE_INFO pstImageInfo )
{
	//
	//	1, The FullImageName parameter can be NULL in cases in which the operating system is unable to obtain the full name of the image at process creation time
	//	2, The ProcessId handle identifies the process in which the image has been mapped, but this handle is zero if the newly loaded image is a driver.
	//	3, pstImageInfo->SystemModeImage
	//	   Set either to one for newly loaded kernel-mode components, such as drivers, or to zero for images that are mapped into user space. 
	//
	INT i;
	BOOLEAN bAdded;
	NTSTATUS ntStatus;
	ULONG ulProcessIdTmp;

	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
	{
		return;
	}
	
	__try
	{
		ulProcessIdTmp = 0;
		RtlMoveMemory( &ulProcessIdTmp, &ProcessId, sizeof(ProcessId) );

		if ( pusFullImageName && ulProcessIdTmp > 0 )
		{
			//	0 == pstImageInfo->SystemModeImage

			if ( pusFullImageName->Length > 44 &&
				0 == _wcsnicmp( L"\\Device\\HarddiskVolume", pusFullImageName->Buffer, 22 )
			)
			{
				bAdded = FALSE;
				for ( i = 0; i < PROCPROCESS_MAX_LAUNCHED_PROCESS_ARRSIZE; i ++ )
				{
					if ( ulProcessIdTmp == g_ArrLaunchedProcess[ i ].ulProcessID )
					{
						bAdded = TRUE;
						break;
					}
				}
				if ( ! bAdded )
				{
					for ( i = 0; i < PROCPROCESS_MAX_LAUNCHED_PROCESS_ARRSIZE; i ++ )
					{
						if ( 0 == g_ArrLaunchedProcess[ i ].ulProcessID )
						{
							g_ArrLaunchedProcess[ i ].uLengthInBytesW	= min( pusFullImageName->Length, sizeof(g_ArrLaunchedProcess[ i ].wszFullImagePath) );
							RtlMoveMemory( g_ArrLaunchedProcess[ i ].wszFullImagePath, pusFullImageName->Buffer, g_ArrLaunchedProcess[ i ].uLengthInBytesW );

							//
							//	Convert WCHAR to UCHAR and computer crc32 by UCHAR
							//
							ntStatus = drvfunc_w2c_ex
								(
									g_ArrLaunchedProcess[ i ].wszFullImagePath,
									g_ArrLaunchedProcess[ i ].uLengthInBytesW,
									g_ArrLaunchedProcess[ i ].uszFullImagePath,
									sizeof(g_ArrLaunchedProcess[ i ].uszFullImagePath)
								);
							if ( NT_SUCCESS( ntStatus ) )
							{
								g_ArrLaunchedProcess[ i ].uLengthInBytesU	= (USHORT)strlen( g_ArrLaunchedProcess[ i ].uszFullImagePath );
								g_ArrLaunchedProcess[ i ].ulCrc32U		= crc32_autolwr_a( g_ArrLaunchedProcess[ i ].uszFullImagePath, g_ArrLaunchedProcess[ i ].uLengthInBytesU );
								g_ArrLaunchedProcess[ i ].ulProcessID		= ulProcessIdTmp;

								bAdded = TRUE;
								break;
							}
						}
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(( "EXCEPTION EXECUTE IN: procprocess_LoadImageNotifyRoutine\n" ));
	}
}
