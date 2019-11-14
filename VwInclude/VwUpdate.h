//	VwUpdate.h: interface for the CVwUpdate class.
//	Created @ 2007-09-24
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWUPDATE_HEADER__
#define __VWUPDATE_HEADER__

#include <vector>
using namespace std;

#include "VwConst.h"
#include "VwFunc.h"
#include "VwHttp.h"
#include "VwZip.h"
#include "MultiString.h"
#include "ProcSecurityDesc.h"

#include "shellapi.h"
#pragma comment( lib, "shell32.lib" )


/**
 *	更新文件所需要的宏
 */
#define VWUPDATE_UPDATE_FAILED		0	//	更新失败
#define VWUPDATE_UPDATE_OK		1	//	更新成功
#define VWUPDATE_UPDATE_RESTART		2	//	需要重启

#define VWUPDATE_MOVE_FILE_OK			0
#define VWUPDATE_MOVE_FILE_ERROR			1
#define VWUPDATE_MOVE_FILE_PENDING		2

#define VWUPDATE_DELETE_FILE_OK			0
#define VWUPDATE_DELETE_FILE_ERROR		1
#define VWUPDATE_DELETE_FILE_PENDING		2

#define VWUPDATE_REGKEY_RUNONCE			"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
#define VWUPDATE_WININIT_FILE_NAME		"wininit.ini"
#define VWUPDATE_WININIT_RENAME_SEC		"[rename]"


/**
 *	读取 INI 所需要的宏
 */
#define VWUPDATE_SECTION_ADD		"Add"
#define VWUPDATE_SECTION_UPDATE		"update"

#define VWUPDATE_KEY_URL		"Url"
#define VWUPDATE_KEY_REG		"Reg"
#define VWUPDATE_KEY_RUN		"Run"
#define VWUPDATE_KEY_EXEC		"Exec"
#define VWUPDATE_KEY_DETAIL		"Detail"
#define VWUPDATE_KEY_LASTWORK		"lastwork"

#define VWUPDATE_EXEC_FLAG_NONE		0	//	不运行
#define VWUPDATE_EXEC_FLAG_EXEC		1	//	执行一个 EXE 可执行文件
#define VWUPDATE_EXEC_FLAG_LOAD		2	//	针对 DLL 尝试 LoadLibrary，然后执行其导出函数 Exec

#define VWUPDATE_UPDATECFGINI		"UpdateCfg.ini"	//	本地更新配置文件


/**
 *	DLL Load 接口导出定义
 */
typedef BOOL(WINAPI * PFNLOAD)( LPCTSTR lpcszCmd );



/**
 *	更新配置数据
 */
typedef struct tagUpdateInfo
{
	tagUpdateInfo()
	{
		memset( this, 0, sizeof(tagUpdateInfo) );
		dwTimeout = 6*60*1000;
	}

	DWORD dwTimeout;
	HWND  hMainWnd;
	TCHAR szUpdateUrl[ MAX_PATH ];
	TCHAR szUpdateDir[ MAX_PATH ];
	TCHAR szUpdateCfgIniFile[ MAX_PATH ];
	TCHAR szUpdateIniFile[ MAX_PATH ];
	TCHAR szDstDir[ MAX_PATH ];

}STUPDATEINFO, *PSTUPDATEINFO;


/**
 *	下载列表
 */
typedef struct tagUpdateList
{
	tagUpdateList()
	{
		memset( this, 0, sizeof(tagUpdateList) );
	}
	
	TCHAR szUrl[ MAX_PATH ];
	TCHAR szVer[ 32 ];		//	INI 中指定该文件的最新版本号
	BOOL  bReg;
	UINT  uExec;			//	详细见上面的宏
	BOOL  bDownSucc;		//	记录是否下载成功

	//	..
	TCHAR szUrlFileName[ 64 ];		//	ZIP 压缩后的文件名
	TCHAR szFileName[ 64 ];		//	文件名
	TCHAR szDownFile[ MAX_PATH ];	//	下载临时存放的文件路径
	TCHAR szRealFile[ MAX_PATH ];	//	下载文件真实保存路径
	TCHAR szUnZipDir[ MAX_PATH ];	//	文件下载后，如果是压缩文件，这个是临时解压目录
	TCHAR szDetail[ MAX_PATH ];	//	详细
	
	//	..
	TCHAR szDstFile[ MAX_PATH ];	//	目标文件路径
	TCHAR szDstFileVer[ 32 ];	//	目标文件版本
	
}STUPDATELIST, *PSTUPDATELIST;




/**
 *	更新类
 */
class CVwUpdate :
	public CVwHttp
{
public:
	CVwUpdate();
	virtual ~CVwUpdate();

	//	设置更新配置信息
	BOOL SetUpdateInfo( STUPDATEINFO * pstUpInfo );

	//	开始更新
	BOOL StartUpdate();
	
	//	清理所有临时数据文件
	VOID CleanAllTempDataFile();

private:
	//	是否准备好
	BOOL IsReady();

	//	从服务器获取是否需要升级的信息
	BOOL GetUpdateIniFromServer();

	//	根据本地配置文件，判断当前是否开始向服务器发出升级信息
	BOOL IsWorkTime();

	//	保存最后工作时间
	VOID SaveLastWorkTime();

	//	分析升级信息
	BOOL CheckUpdateInfo();
	
	//	解析 INI 文件
	BOOL ParseIni( LPCTSTR lpszUpdateIni );
	
	//	下载所有服务器指定的模块
	BOOL DownloadAllModules();
	
	//	进行更新所有服务器指定的模块
	BOOL UpdateAllModules();

	//
	//	更新文件处理函数系列
	//
	UINT UpdateFile( LPCTSTR lpszSrc, LPCTSTR lpszDst, BOOL bForceUpdate, BOOL bReg, UINT nExecFlag );
	INT  MyMoveFile( LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bRestartMove );
	INT  MyDeleteFile( LPCTSTR lpFileName, BOOL bBootDelete );
	BOOL IsWindowsNT();
	BOOL GetWinInitFileName( LPTSTR lpszWininitFileName, DWORD dwSize );
	BOOL AddDelOrMoveToWinInit( LPCTSTR lpExistingFileNameA, LPCTSTR lpNewFileNameA );
	BOOL IsDllCanbeRun( LPCTSTR lpszFile );
	BOOL IsDllCanbeLoad( LPCTSTR lpszFile );
	BOOL RunDllLoad( LPCTSTR lpszFile, LPCTSTR lpcszCmd );

private:
	BOOL m_bIsReady;
	STUPDATEINFO m_stUpInfo;
	vector<STUPDATELIST>		m_vcUpdateList;
	vector<STUPDATELIST>::iterator	m_vcIt;

};

#endif	// __VWUPDATE_HEADER__
