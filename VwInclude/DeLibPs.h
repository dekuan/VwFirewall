#ifndef __DELIBPS_HEADER__
#define __DELIBPS_HEADER__



#include <atlbase.h>	// for CSimpleArray


//////////////////////////////////////////////////////////////////////////
//	start define

#ifndef _DEENUMPROCESS_DEFINES_
#define _DEENUMPROCESS_DEFINES_


//	排序功能号
#define DEENUMPROCESS_SORT_BY_PROCESS_NAME		1
#define DEENUMPROCESS_SORT_BY_PROCESS_SIZE		2
#define DEENUMPROCESS_SORT_BY_PROCESS_COMPANY		3


typedef struct tagProcessInfo
{
	tagProcessInfo()
	{
		memset( this, 0, sizeof(tagProcessInfo) );
	}
	DWORD dwProcID;
	TCHAR szWindowTitle[ MAX_PATH ];
	TCHAR szProcName[ MAX_PATH ];
	TCHAR szProcPath[ MAX_PATH ];
	TCHAR szCompressPath[ MAX_PATH ];
	TCHAR szFileMd5[ 64 ];
	DWORD dwMemoryUsed;
	DWORD dwFileSize;
	TCHAR szCompany[ MAX_PATH ];
	TCHAR szVersion[ MAX_PATH ];

} STPROCESSINFO;

#endif	// _DEENUMPROCESS_DEFINES_

//	end define
//////////////////////////////////////////////////////////////////////////



/**
 *	导出函数定义
 */
INT   __stdcall delib_enum_process( STPROCESSINFO * pstProcessList );
BOOL  __stdcall delib_kill_process( DWORD dwProcessID, DWORD dwTimeout, LPTSTR lpszError );



#endif