// CUrlHttp.h: interface for the CVwCUrlHttp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURLHTTP_H__32E14BB7_C6C8_4CCC_9FE6_B1D617AC1428__INCLUDED_)
#define AFX_CURLHTTP_H__32E14BB7_C6C8_4CCC_9FE6_B1D617AC1428__INCLUDED_


#include <malloc.h>
#include <stdlib.h>

#include "curl/curl.h"
#pragma comment(lib, "curl/libcurl_imp.lib")


/**
 *	½á¹¹Ìå
 */
typedef struct tagCUrlMemory
{
	char * memory;
	size_t size;

}STCURLMEMORY, *PSTCURLMEMORY;

typedef struct tagCUrlOption
{
	tagCUrlOption()
	{
		memset( this, 0, sizeof(tagCUrlOption) );
	}
	DWORD   dwTimeout;
	LPCTSTR  lpcszCookie;
	LPCTSTR  lpcszReferer;
	LPCTSTR  lpcszUserAgent;
	LPCTSTR  lpcszPostData;

}STCURLOPTION, *PSTCURLOPTION;



class CVwCUrlHttp  
{
public:
	CVwCUrlHttp();
	virtual ~CVwCUrlHttp();

	VOID setTimeout( DWORD dwTimeout );
	VOID setCookie( LPCTSTR lpcszCookie );
	VOID setReferer( LPCTSTR lpcszReferer );
	VOID setUserAgent( LPCTSTR lpcszUserAgent );
	VOID setPostData( LPCTSTR lpcszPostData );

	BOOL getResponse( LPCTSTR lpcszUrl, STCURLMEMORY * chunk );
	BOOL postForm( LPCTSTR lpcszUrl, STCURLMEMORY * chunk );

	VOID freeMemory( STCURLMEMORY * chunk );

private:
	STCURLOPTION	m_stCUrlOpt;

};

#endif // !defined(AFX_CURLHTTP_H__32E14BB7_C6C8_4CCC_9FE6_B1D617AC1428__INCLUDED_)
