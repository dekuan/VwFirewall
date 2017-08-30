// DrvThreadLogger.c
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrvThreadLogger.h"
#include "DrvFunc.h"


/**
 *	变量定义
 */
BOOLEAN g_bDrvTLIsInit		= FALSE;
STDRVTLTHREAD g_stDrvTLThread;




/**
 *	创建线程
 */
NTSTATUS drvthreadlogger_create_thread()
{
	NTSTATUS ntStatus;

	InitializeListHead( &g_stDrvTLThread.ListEntryNode );
	KeInitializeSpinLock( &g_stDrvTLThread.spLock );
	KeInitializeEvent( &g_stDrvTLThread.hEvent, SynchronizationEvent, FALSE );

	ntStatus = PsCreateSystemThread
		(
			&g_stDrvTLThread.hThread,
			0,
			NULL,
			NULL,
			NULL,
			drvthreadlogger_threadproc,
			(PVOID)(&g_stDrvTLThread)
		);
	if ( NT_SUCCESS( ntStatus ) )
	{
		g_bDrvTLIsInit = TRUE;
	}

	return ntStatus;
}

VOID drvthreadlogger_threadproc( IN PVOID pvContext )
{
	LPSTDRVTLTHREAD pstThread;
	LPSTDRVTLNODE pstNode;

	pstThread = (LPSTDRVTLTHREAD)pvContext;
	if ( pstThread )
	{
		while ( TRUE )
		{
			KeWaitForSingleObject
				(
					&pstThread->hEvent,
					Executive,
					KernelMode,
					FALSE,
					NULL
				);
			while ( TRUE )
			{
				pstNode = (LPSTDRVTLNODE)ExInterlockedRemoveHeadList( &pstThread->ListEntryNode, &pstThread->spLock );
				if ( pstNode )
				{
					pstNode->pfnWorker( pstNode->pvContent );
					KeSetEvent( &pstNode->hEvent, IO_NO_INCREMENT, FALSE );
				}
				else
				{
					break;
				}
			}
		}
	}
}

NTSTATUS drvthreadlogger_execute_task( IN PFN_DRVTLTHREAD_WORKER pfnWorker, IN OUT PVOID pvContent )
{
	NTSTATUS ntStatus;
	LPSTDRVTLNODE pstNode;

	if ( ! g_bDrvTLIsInit )
	{
		return STATUS_UNSUCCESSFUL;
	}

	ntStatus = STATUS_UNSUCCESSFUL;

	//pstNode = ExAllocatePoolWithTag( NonPagedPool, sizeof(STDRVTLNODE), WIT_TAG );
	pstNode = ExAllocatePool( NonPagedPool, sizeof(STDRVTLNODE) );
	if ( pstNode )
	{
		pstNode->pvContent	= pvContent;
		pstNode->pfnWorker	= pfnWorker;
		KeInitializeEvent( &pstNode->hEvent, SynchronizationEvent, FALSE );
		ExInterlockedInsertTailList
			(
				&g_stDrvTLThread.ListEntryNode,
				&pstNode->ListEntry,
				&g_stDrvTLThread.spLock
			);
		KeSetEvent( &g_stDrvTLThread.hEvent, (KPRIORITY)0, FALSE );
		//KeWaitForSingleObject( &pstNode->hEvent, Executive, KernelMode, FALSE, NULL );

		ExFreePool( pstNode );
		pstNode = NULL;

		ntStatus = STATUS_SUCCESS;
	}
	else
	{
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;
	}

	return ntStatus;
}

BOOLEAN drvthreadlogger_is_my_thread()
{
	if ( ! g_bDrvTLIsInit )
	{
		return FALSE;
	}

	return ( PsGetCurrentThreadId() == g_stDrvTLThread.hThread );
}

VOID drvthreadlogger_worker( IN PVOID pvWorkerContext )
{
	LPSTDRVTLLOG pstLog;

	if ( NULL == pvWorkerContext )
	{
		return;
	}

	pstLog = (LPSTDRVTLLOG)pvWorkerContext;
	if ( pstLog )
	{
		drvfunc_save_filecontent( pstLog->lpcwszFilename, pstLog->lpvContent, pstLog->dwSize, TRUE );
	}
}