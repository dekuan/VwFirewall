// DeEnumProcess.h: interface for the CDeEnumProcess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEENUMPROCESS_HEADER__
#define __DEENUMPROCESS_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <atlbase.h>
#include "CompressedPath.h"


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


typedef struct tagSpecModule
{
	tagSpecModule()
	{
		memset( this, 0, sizeof(tagSpecModule) );
	}
	TCHAR szFilePath[ MAX_PATH ];
	
}STSPECMODULE, * PSTSPECMODULE;



class CDeEnumProcess :
	public CCompressedPath
{
public:
	CDeEnumProcess( HINSTANCE hCallerInstance );
	virtual ~CDeEnumProcess();
	
public:
	BOOL EnumProcess( CSimpleArray<STPROCESSINFO> * pArrProcess );
	BOOL FreeProcessArray();
	BOOL GetItemByIndex( INT dwIndex, LPTSTR lpszJsonString, DWORD dwSize, DWORD * pdwDataLen );

	BOOL ReadLinksInfoByIndex( int dwIndex_in, TCHAR * pszInfo_out, DWORD dwSize, LPTSTR lpszError );
	BOOL SortProcessArray( DWORD dwSortType );
	BOOL GetColSpecModules( LPTSTR pszFileList, DWORD dwSize );
	BOOL SetCollectProcessName( LPCTSTR lpcszProcessName );

private:
	BOOL UpPrivilege( TCHAR * pszPrivilege, BOOL bEnable );
	BOOL DoProcessEnum( LPTSTR lpszError );
	BOOL CorrectSpecSysPath( TCHAR * pszPathSpec, TCHAR * pszPathOut );
	BOOL CollectSpecModule( LPTSTR pszFilePath );
	BOOL SaveProcessInfo( STPROCESSINFO * stInfo, LPTSTR lpszError );

private:
	//	存放进程信息的数组
	CSimpleArray<STPROCESSINFO> m_aProcessInfo;

	//	存放特殊模块的文件地址
	CSimpleArray<STSPECMODULE> m_aSpecModule;
	

	TCHAR m_szSpecProcessName[ MAX_PATH ];

};


#endif // __DEENUMPROCESS_HEADER__
