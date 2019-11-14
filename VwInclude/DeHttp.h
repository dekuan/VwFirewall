//////////////////////////////////////////////////////////////////////////
//
//	Version 1.0.4.1004
//	History:
//		2008-11-01 Add GetResponseByProxy
//


#ifndef __DEHTTP_HEADER__
#define __DEHTTP_HEADER__


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>


#include <vector>
using namespace std;

#include "winInet.h"
#pragma comment( lib, "winInet.lib" )




typedef unsigned (WINAPI *PBEGINTHREADEX_THREADFUNC)(LPVOID lpThreadParameter);
typedef unsigned *PBEGINTHREADEX_THREADID;





//////////////////////////////////////////////////////////////////////////
typedef struct tagHttpCookie
{
	tagHttpCookie()
	{
		memset( this, 0, sizeof(tagHttpCookie) );
	}
	
	TCHAR	szName[ 64 ];
	TCHAR	szData[ 4096 ];

}STHTTPCOOKIE, *LPSTHTTPCOOKIE;

typedef struct tagHttpInfo
{
	tagHttpInfo()
	{
		memset( this, 0, sizeof(tagHttpInfo) );
	}
	DWORD dwHttpFileLen;
	DWORD dwHttpStatusCode;
	DWORD dwHttpQueryStart;
	DWORD dwHttpQueryComplete;
	TCHAR szHttpLastModified[ MAX_PATH ];
	TCHAR szHttpServerInfo[ MAX_PATH ];
	
} STHTTPINFO, *PSTHTTPINFO;

typedef struct tagHttpFormData
{
	tagHttpFormData()
	{
		memset( this, 0, sizeof(tagHttpFormData) );
	}
	BOOL  bGZipOpen;
	TCHAR szUserFileInput[ MAX_PATH ];
	TCHAR szUserFileValue[ MAX_PATH ];
	TCHAR szFilesInput[ MAX_PATH ];
	TCHAR szFilesValue[ MAX_PATH ];
	TCHAR szExtraInput[ MAX_PATH ];
	TCHAR szExtraValue[ MAX_PATH ];

} STHTTPFORMDATA;

typedef struct tagHttpHeaderVar
{
	TCHAR szHeadLine[ 4096 ];

} STHTTPHEADERVAR, *PSTHTTPHEADERVAR;


//////////////////////////////////////////////////////////////////////////
//
#define HTTP_THREAD_SUCC			0
#define HTTP_THREAD_FAIL			1

#define GETSTATUSCODE_ERROR			0
#define DEFAULT_AGENT_NAME			_T("DeHttp Agent")
#define DEFAULT_PORT				80
#define DEFAULT_BOUNDRY_TAG			_T("---------------------------7d42613120976")
#define DEFAULT_BUF_SIZE			1024
#define DEFAULT_FORM_INPUT_USERFILE		_T("userfile")
#define DEFAULT_FORM_INPUT_FILES		_T("files")

#define HTTP_ACCEPT_TYPE			_T("*/*")
#define HTTP_ACCEPT				_T("Accept: */*\r\n")
#define HTTP_VERB_GET				_T("GET")
#define HTTP_VERB_POST				_T("POST")

// 防止 URL 被 Cache，为 URL 加上一个随机数字
#define CHECK_NOISE_FLAG			_T("?")
#define URL_NOISE_1				_T("?f=")
#define URL_NOISE_2				_T("&f=")
#define DOWNLOAD_TIME_FORCEFAIL			60*1000		// 进度无变化，超时处理，强制超时时间

#define UM_HTTP_PREGRESS			WM_USER + 0x6000
#define UM_HTTP_COMPLETE			WM_USER + 0x6001





//////////////////////////////////////////////////////////////////////////
//
typedef struct tagDeHttpDeParseUrl
{
	tagDeHttpDeParseUrl()
	{
		memset( this, 0, sizeof(tagDeHttpDeParseUrl) );
	}
	
	CHAR szScheme[ 16 ];		//	e.g. http
	CHAR szUser[ 32 ];
	CHAR szPass[ 32 ];
	CHAR szHost[ 128 ];
	UINT  uPort;
	CHAR szPath[ 1024 ];
	CHAR szQuery[ 1024 ];	//	after the question mark ?
	CHAR szFragment[ MAX_PATH ];	//	after the hashmark #	
	
} STDEHTTPDEPARSEURL, *LPSTDEHTTPDEPARSEURL;
typedef struct tagDeHttpDeParseUrlW
{
	tagDeHttpDeParseUrlW()
	{
		memset( this, 0, sizeof(tagDeHttpDeParseUrlW) );
	}
	
	WCHAR szScheme[ 16 ];		//	e.g. http
	WCHAR szUser[ 32 ];
	WCHAR szPass[ 32 ];
	WCHAR szHost[ 128 ];
	UINT  uPort;
	WCHAR szPath[ 1024 ];
	WCHAR szQuery[ 1024 ];		//	after the question mark ?
	WCHAR szFragment[ MAX_PATH ];	//	after the hashmark #	
	
} STDEHTTPDEPARSEURLW, *LPSTDEHTTPDEPARSEURLW;


typedef struct tagHttpThread
{
	tagHttpThread()
	{
		memset( this, 0, sizeof(tagHttpThread) );
	}
	const TCHAR *	pszUrl;
	const TCHAR *	pszDestFile;
	TCHAR *		pszPostData;
	const TCHAR *	pszAgent;			//	for V1.0.0.2
	vector<STHTTPHEADERVAR> * pvcHttpHeaderVar;	//	for v1.0.3.1003
	TCHAR *		pszHeader;			//	for V1.0.0.2 / HttpGetHeader

	LPCTSTR		lpcszCookie;			// [in] cookie
	vector<STHTTPCOOKIE>	* pvcCookie;		// [in] cookie

	LPCTSTR		lpcszProxy;			//	for v1.0.4.1004 / GetResponseByProxy
	BOOL		bTextFile;			//	GetFile 标记是否是文本文件
	TCHAR *		pszResponse;
	TCHAR *		pszIpAddress;		// 根据 Host Name 返回 IP 地址
	DWORD		dwSize;
	TCHAR *		pszError;

	BOOL		bAddNoise;		// 防止 URL 被 Catch，为 URL 人为地增加一个随机数字
	DWORD		dwTimeout;		// 超时控制

	STHTTPINFO *	stHttpInfo;		// HttpGetInfo 返回内容
	STHTTPFORMDATA * stFormData;		// HttpPostFile 数据
	
	BOOL		bRet;			// 线程返回值
	DWORD		dwRet;			// 线程返回值
	UINT		uThreadID;
	HANDLE		hThread;		// 线程句柄
	HWND		hMainWnd;		// 父窗口句柄，用来给发进度

	LPVOID		pvThisCls;		// This 类指针

}STHTTPTHREAD, *PSTHTTPTHREAD;


/**
 *	class of CDeHttp
 */
class CDeHttp
{
public:
	BOOL HaveInternetConnection();

	BOOL GetHttpInfo( LPCTSTR lpcszUrl, STHTTPINFO & stHttp, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI GetHttpInfoProc( LPVOID p );

	BOOL IsUrlConnectOk( LPCTSTR lpcszUrl, LPTSTR lpszError, DWORD dwTimeout );

	DWORD GetStatusCode( LPCTSTR lpcszUrl, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI GetStatusCodeProc( LPVOID p );

	DWORD GetStatusCodeEx( LPCTSTR lpcszUrl, LPCTSTR lpszAgent, vector<STHTTPCOOKIE> * pvcCookie, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI CDeHttp::GetStatusCodeExProc( LPVOID p );

	BOOL GetHttpHeader( LPCTSTR lpcszUrl, LPTSTR lpszHeader, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI GetHttpHeaderProc( LPVOID p );

	BOOL GetHttpHeaderEx( LPCTSTR lpcszUrl, vector<STHTTPHEADERVAR> & vcHttpHeaderVar, LPTSTR lpszHeader, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI GetHttpHeaderExProc( LPVOID p );

	BOOL GetResponse( LPCTSTR lpcszUrl, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI GetResponseProc( LPVOID p );

	BOOL GetResponseByProxy( LPCTSTR lpcszUrl, LPCTSTR lpcszProxy, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI GetResponseByProxyProc( LPVOID p );

	BOOL GetResponseEx( LPCTSTR lpcszUrl, vector<STHTTPHEADERVAR> & vcHttpHeaderVar, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI GetResponseExProc( LPVOID p );

	BOOL PostForm( LPCTSTR lpcszUrl, LPTSTR lpszPostData, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout );
	BOOL PostFormEx( LPCTSTR lpcszUrl, LPTSTR lpszPostData, LPTSTR lpszAgent, LPTSTR lpszHeader, 
			    LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI PostFormProc( LPVOID p );

	BOOL PostFile( LPCTSTR lpcszUrl, STHTTPFORMDATA & stFormData, const HWND hMainWnd,
			  LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI PostFileProc( LPVOID p );

	BOOL GetFile( LPCTSTR lpcszUrl, LPTSTR lpszDestFile, BOOL bTextFile, LPTSTR lpszError, DWORD dwTimeout );
	BOOL GetFileEx( LPCTSTR lpcszUrl, LPTSTR lpszAgent, LPTSTR lpszHeader, LPCTSTR lpcszDestFile, BOOL bTextFile, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI GetFileProc( LPVOID p );

	BOOL DownloadFile( LPCTSTR lpcszUrl, LPTSTR lpszDestFile, const HWND hMainWnd, BOOL bAddNoise, LPTSTR lpszError, DWORD dwTimeout );
	static DWORD WINAPI DownloadFileProc( LPVOID p );

	INT UrlEncode( LPCTSTR lpcszInput, INT nInputLen, LPTSTR lpszOutBuf, INT nOutBufLen );


	BOOL  dehttp_delib_parse_urlA( LPCSTR lpcszUrl, DWORD dwUrlLen, STDEHTTPDEPARSEURL * pstParseUrl );
	BOOL  dehttp_delib_parse_urlW( LPCWSTR lpcwszUrl, DWORD dwUrlLen, STDEHTTPDEPARSEURLW * pstParseUrlW );
#ifdef UNICODE
#define dehttp_delib_parse_url  dehttp_delib_parse_urlW
#else
	BOOL  dehttp_delib_parse_url( LPCSTR lpcszUrl, DWORD dwUrlLen, STDEHTTPDEPARSEURL * pstParseUrl );
#endif
	TCHAR * dehttp_delibfunc_replace_controlchars( LPSTR lpszStr, UINT uLen );
	BOOL dehttp_delibfunc_memcpy( LPCTSTR lpcszSrc, DWORD dwSrcLen, LPTSTR lpszDst, DWORD dwDstSize, BOOL bRepCtlChr = FALSE );


private:
	// ..
	BOOL  ParseUrl( const TCHAR * pszUrl, TCHAR * pszServer, TCHAR * pszPath );
	BOOL  UserHttpSendReqEx( HINTERNET hRequest, STHTTPFORMDATA & stFormData, HWND hMainWnd, TCHAR * pszError );
	BOOL  ReadInternetFileBuffer( HINTERNET hRequest, TCHAR * pszBuffer, DWORD dwSize );

};





#endif	// __DEHTTP_HEADER__




