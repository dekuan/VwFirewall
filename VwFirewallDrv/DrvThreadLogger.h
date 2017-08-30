// DrvThreadLogger.h: interface for the DrvThreadLogger class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DRVTHREADLOGGER_HEADER__
#define __DRVTHREADLOGGER_HEADER__


typedef void (*PFN_DRVTLTHREAD_WORKER)( PVOID pvContext );

typedef struct tagDrvTLThread
{
	LIST_ENTRY ListEntryNode;	//	Request list in out thread
	HANDLE hThread;		//	Thread id.
	KEVENT hEvent;		//	An event to inform the thread to process a request.
	KSPIN_LOCK spLock;	//	A lock used by the list

}STDRVTLTHREAD, *LPSTDRVTLTHREAD;

typedef struct tagDrvTLNode
{
	LIST_ENTRY ListEntry;
	PVOID pvContent;
	KEVENT hEvent;
	PFN_DRVTLTHREAD_WORKER pfnWorker;

}STDRVTLNODE, *LPSTDRVTLNODE;

typedef struct tagDrvTLLog
{
	UINT uType;
	LPCWSTR lpcwszFilename;
	PVOID lpvContent;
	DWORD dwSize;

}STDRVTLLOG, *LPSTDRVTLLOG;


/**
 *	...
 */
extern BOOLEAN g_bDrvTLIsInit;
extern STDRVTLTHREAD g_stDrvTLThread;



NTSTATUS drvthreadlogger_create_thread();
VOID     drvthreadlogger_threadproc( IN PVOID pvContext );
NTSTATUS drvthreadlogger_execute_task( IN PFN_DRVTLTHREAD_WORKER pfnWorker, IN OUT PVOID pvContent );
BOOLEAN  drvthreadlogger_is_my_thread();
VOID     drvthreadlogger_worker( IN PVOID pvWorkerContext );



#endif // __DRVTHREADLOGGER_HEADER__
