// ProcProcess.h: interface for the ProcProcess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCPROCESS_HEADER__
#define __PROCPROCESS_HEADER__


/**
 *	...
 */
#define PROCPROCESS_MAX_LAUNCHED_PROCESS_ARRSIZE	2048	//	运行的进程信息数组大小



//
//	运行的进程列表
//
typedef struct tagLaunchedProcess
{
	ULONG  ulProcessID;
	WCHAR  wszFullImagePath[ MAX_PATH ];
	USHORT uLengthInBytesW;			//	Length of wszFullImagePath in bytes

	UCHAR  uszFullImagePath[ MAX_PATH ];
	USHORT uLengthInBytesU;			//	Length of uszFullImagePath in bytes
	ULONG  ulCrc32U;			//	Crc32 值

}STLAUNCHEDPROCESS, *LPSTLAUNCHEDPROCESS;



/**
 *	...
 */
extern STLAUNCHEDPROCESS g_ArrLaunchedProcess[ PROCPROCESS_MAX_LAUNCHED_PROCESS_ARRSIZE ];


/**
 *	...
 */
VOID    procprocess_init();
BOOLEAN procprocess_get_fullimagepath_by_pid( ULONG ulProcessId, WCHAR ** ppwszFullImagePath, USHORT * puFullImagePathLength, ULONG * pulCrc32U );
VOID    procprocess_CreateProcessNotifyRoutine( IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN bCreate );
VOID    procprocess_LoadImageNotifyRoutine( IN OPTIONAL PUNICODE_STRING pusFullImageName, IN HANDLE ProcessId, IN PIMAGE_INFO pstImageInfo );





#endif // __PROCPROCESS_HEADER__
