#ifndef __HTTP_HEADER__
#define __HTTP_HEADER__


#include "process.h"
#include "winInet.h"
#pragma comment( lib, "winInet.lib" )

#include "vector"
using namespace std;


//////////////////////////////////////////////////////////////////////////
// 默认消息（如果用户不指定的话）
#define UM_DEFFUP_PREGRESS	WM_USER + 0x6000	// 默认消息

// wParam 参数传递消息类型，lParam 传递消息内容
#define WP_HTTP_START		0x1100		// 上传开始
#define WP_HTTP_PREGRESS	0x1101		// 上传中...
#define WP_HTTP_COMPLETE	0x1102		// 上传完成

 

//////////////////////////////////////////////////////////////////////////
typedef struct tagHttpFormData
{
	tagHttpFormData()
	{
		memset( this, 0, sizeof(tagHttpFormData) );
	}
	BOOL  bGZipOpen;
	TCHAR szUserFileInput[ 32 ];		// userfile 字段
	TCHAR szUserFileValue[ MAX_PATH ];
	TCHAR szFilesInput[ 32 ];		// 简单文件列表
	TCHAR szFilesValue[ MAX_PATH ];
	TCHAR szExtraInput[ 32 ];		// 附加字段
	TCHAR szExtraValue[ MAX_PATH ];

} STHTTPFORMDATA;


//////////////////////////////////////////////////////////////////////////
//
#define GETSTATUSCODE_ERROR			0

#define HTTP_THREAD_SUCC			0
#define HTTP_THREAD_FAIL			1

#define DEFAULT_AGENT_NAME			"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322)"
#define DEFAULT_PORT				80
#define DEFAULT_BOUNDRY_TAG			_T("---------------------------7d42613120976")
#define DEFAULT_BUF_SIZE			1024
#define DEFAULT_FORM_INPUT_USERFILE		_T("userfile")
#define DEFAULT_FORM_INPUT_FILES		_T("files")

#define HTTP_ACCEPT_TYPE			_T("*/*")
#define HTTP_ACCEPT				_T("Accept: */*\r\n")
#define HTTP_VERB_GET				_T("GET")
#define HTTP_VERB_POST				_T("POST")





//////////////////////////////////////////////////////////////////////////
//
typedef struct tagHttpCookie
{
	tagHttpCookie()
	{
		memset( this, 0, sizeof(tagHttpCookie) );
	}

	TCHAR	szName[ 64 ];
	TCHAR	szData[ MAX_PATH ];

}STHTTPCOOKIE, *PSTHTTPCOOKIE;

typedef struct tagHttpThread
{
	tagHttpThread()
	{
		memset( this, 0, sizeof(tagHttpThread) );
	}
	const TCHAR	* pszUrl;
	TCHAR		* pszPostData;
	const TCHAR	* pszAgent;		// for V1.0.0.2
	TCHAR		* pszHeader;		// for V1.0.0.2 / HttpGetHeader
	LPCTSTR		lpcszCookie;		// [in] cookie
	vector<STHTTPCOOKIE>	* pvcCookie;	// [in] cookie
	TCHAR		* pszResponse;
	DWORD		dwSize;
	TCHAR		* pszError;

	DWORD		dwTimeout;		// 超时控制

	STHTTPFORMDATA	* stFormData;		// HttpPostFile 数据
	
	BOOL		bRet;			// 线程返回值
	DWORD		dwRet;			// 线程返回值
	UINT		uThreadID;
	HANDLE		hThread;		// 线程句柄
	HWND		hMainWnd;		// 父窗口句柄，用来给发进度
	UINT		uFupMessage;		// 用户自定义的进度消息

}STHTTPTHREAD;



//////////////////////////////////////////////////////////////////////////
// 导出功能函数
BOOL HttpHaveInternetConnection();

DWORD HttpGetStatusCode( LPCTSTR lpcszUrl, LPCTSTR pszAgent, vector<STHTTPCOOKIE> * pvcCookie, LPSTR pszError, DWORD dwTimeout );
static unsigned int __stdcall HttpGetStatusCodeProc( VOID * p );

BOOL HttpGetResponse( LPCTSTR pszUrl, LPCTSTR pszAgent, LPTSTR pszResponse, DWORD dwSize, LPTSTR pszError, DWORD dwTimeout );
static unsigned int __stdcall HttpGetResponseProc( VOID * p );

BOOL HttpPostForm( LPCTSTR pszUrl, LPTSTR pszPostData, LPCTSTR pszAgent, 
		LPTSTR pszResponse, DWORD dwSize, LPTSTR pszError, DWORD dwTimeout );
static unsigned int __stdcall HttpPostFormProc( VOID * p );

BOOL HttpPostFile( LPCTSTR pszUrl, STHTTPFORMDATA & stFormData, LPCTSTR pszAgent, const HWND hMainWnd, UINT uFupMessage, 
		LPTSTR pszResponse, DWORD dwSize, LPTSTR pszError, DWORD dwTimeout );
static unsigned int __stdcall HttpPostFileProc( VOID * p );

INT UrlEncode( const TCHAR * pszInput, INT nInputLen, TCHAR * pszOutBuf, INT nOutBufLen );




//////////////////////////////////////////////////////////////////////////
// 非导出函数
BOOL  ParseUrl( const TCHAR * pszUrl, TCHAR * pszServer, TCHAR * pszPath, ULONG * puPort );
BOOL  UserHttpSendReqEx( HINTERNET hRequest, STHTTPFORMDATA & stFormData, HWND hMainWnd, UINT uFupMessage, TCHAR * pszError );








#endif	// __HTTP_HEADER__




