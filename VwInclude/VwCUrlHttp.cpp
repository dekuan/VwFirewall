// CUrlHttp.cpp: implementation of the CVwCUrlHttp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwCUrlHttp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//	There might be a realloc() out there that doesn't like reallocing  NULL pointers, so we take care of it here
static void * _vwcurlfunc_myrealloc( void * ptr, size_t size )
{
	if ( ptr )
	{
		return realloc( ptr, size );
	}
	else
	{
		return malloc( size );
	}
}
static size_t _vwcurlfunc_write_memory_callback( void * ptr, size_t size, size_t nmemb, void * data )
{
	size_t realsize = size * nmemb;
	STCURLMEMORY *mem = (STCURLMEMORY *)data;
	//	assert( mem->memory!=NULL || mem->size==0);

	mem->memory = (char *)realloc( mem->memory, mem->size + realsize + sizeof(char) );
	if ( mem->memory )
	{
		memcpy( &( mem->memory[ mem->size ] ), ptr, realsize );
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

CVwCUrlHttp::CVwCUrlHttp()
{
	memset( & m_stCUrlOpt, 0, sizeof(m_stCUrlOpt) );
	m_stCUrlOpt.dwTimeout	= 120;
}
CVwCUrlHttp::~CVwCUrlHttp()
{
}

/**
 *	参数设置函数
 */
VOID CVwCUrlHttp::setTimeout( DWORD dwTimeout )
{
	//
	//	dwTimeout	- [in] 单位是：秒
	//	RETURN		- VOID
	//
	m_stCUrlOpt.dwTimeout = dwTimeout;
}
VOID CVwCUrlHttp::setCookie( LPCTSTR lpcszCookie )
{
	//
	//	lpcszCookie	- [in] cookie 值，例如："a=1;b=123;xing=a=1212"
	//	RETURN		- VOID
	//
	m_stCUrlOpt.lpcszCookie = lpcszCookie;
}
VOID CVwCUrlHttp::setReferer( LPCTSTR lpcszReferer )
{
	//
	//	lpcszReferer	- [in] 引用站点地址
	//	RETURN		- VOID
	//
	m_stCUrlOpt.lpcszReferer = lpcszReferer;
}
VOID CVwCUrlHttp::setUserAgent( LPCTSTR lpcszUserAgent )
{
	//
	//	lpcszUserAgent	- [in] UserAgent 信息
	//	RETURN		- VOID
	//
	m_stCUrlOpt.lpcszUserAgent = lpcszUserAgent;
}
VOID CVwCUrlHttp::setPostData( LPCTSTR lpcszPostData )
{
	//
	//	lpszcPostData	- [in] post data 信息
	//	RETURN		- VOID
	//
	m_stCUrlOpt.lpcszPostData = lpcszPostData;
}


/**
 *	获取 http 结果
 */
BOOL CVwCUrlHttp::getResponse( LPCTSTR lpcszUrl, STCURLMEMORY * chunk )
{
	//
	//	lpcszUrl	- [in]  目标 url
	//	chunk		- [out] 指向返回数据的缓冲区地址
	//	RETURN		- TRUE / FALSE
	//
	if ( NULL == lpcszUrl || NULL == chunk )
	{
		return FALSE;
	}

	CURL * curl_handle;
	INT nErrcode	= 0;

	chunk->memory	= NULL;			//	we expect realloc(NULL, size) to work
	chunk->size	= 0;			//	no data at this point

	curl_global_init( CURL_GLOBAL_ALL );

	//	init the curl session
	curl_handle = curl_easy_init();
	curl_easy_setopt( curl_handle, CURLOPT_URL, lpcszUrl );

	//	send all data to this function
	curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, _vwcurlfunc_write_memory_callback );
	
	//	we pass our 'chunk' struct to the callback function
	curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, (void*)chunk );

	//	常规设置
	curl_easy_setopt( curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 360 );
	curl_easy_setopt( curl_handle, CURLOPT_FOLLOWLOCATION, 1 );
	curl_easy_setopt( curl_handle, CURLOPT_MAXREDIRS, 3 );
	curl_easy_setopt( curl_handle, CURLOPT_CONNECTTIMEOUT, 60 );

	//	可以配置的区域
	curl_easy_setopt( curl_handle, CURLOPT_TIMEOUT, m_stCUrlOpt.dwTimeout );
	if ( m_stCUrlOpt.lpcszCookie )
	{
		curl_easy_setopt( curl_handle, CURLOPT_COOKIE, m_stCUrlOpt.lpcszCookie );
	}
	if ( m_stCUrlOpt.lpcszReferer )
	{
		curl_easy_setopt( curl_handle, CURLOPT_REFERER, m_stCUrlOpt.lpcszReferer );
	}
	if ( m_stCUrlOpt.lpcszUserAgent )
	{
		curl_easy_setopt( curl_handle, CURLOPT_USERAGENT, m_stCUrlOpt.lpcszUserAgent );
	}
	else
	{
		curl_easy_setopt( curl_handle, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)" );
	}
	if ( m_stCUrlOpt.lpcszPostData )
	{
		curl_easy_setopt( curl_handle, CURLOPT_POST, 1 );
		curl_easy_setopt( curl_handle, CURLOPT_POSTFIELDS, m_stCUrlOpt.lpcszPostData );
	}

	//curl_easy_setopt( curl_handle, CURLOPT_COOKIEJAR,		NULL );
	//curl_easy_setopt( curl_handle, CURLOPT_COOKIEFILE, NULL );

	//	开始执行
	nErrcode = curl_easy_perform( curl_handle ); 

	//	cleanup curl stuff
	curl_easy_cleanup( curl_handle );

	return ( 0 == nErrcode ? TRUE : FALSE );
}
/**
 *	post form
 */
BOOL CVwCUrlHttp::postForm( LPCTSTR lpcszUrl, STCURLMEMORY * chunk )
{
	//
	//	lpcszUrl	- [in]  目标 url
	//	chunk		- [out] 指向返回数据的缓冲区地址
	//	RETURN		- TRUE / FALSE
	//	特别注意：
	//		在调用本函数前，
	//		一定要先调用 .setPostData( LPCTSTR lpcszPostData ) 方法，来设置 post 数据
	//
	return getResponse( lpcszUrl, chunk );
}

VOID CVwCUrlHttp::freeMemory( STCURLMEMORY * chunk )
{
	if ( chunk && chunk->memory )
	{
		free( chunk->memory );
		chunk->memory = NULL;
		chunk->size	= 0;
	}
}