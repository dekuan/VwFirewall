// SSLCon.cpp: implementation of the CVwSslHttp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwSslHttp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwSslHttp::CVwSslHttp()
{
	m_hInternet		= NULL;
	m_hRequest		= NULL;
	m_certStoreType		= certStoreMY;
	m_hStore		= NULL;
	m_hSession		= NULL;
	m_pContext		= NULL;
	m_wPort			= INTERNET_DEFAULT_HTTPS_PORT;
	m_strVerb		= "GET";	//	GET / POST
	m_strOrganizationName	= "3-D Secure Compliance TestFacility";
	m_strAgentName		= "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)";
	m_secureFlags		= INTERNET_FLAG_RELOAD|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
	m_ReqID			= 0;
}

CVwSslHttp::~CVwSslHttp()
{
	ClearHandles();
}


bool CVwSslHttp::ConnectToHttpsServer()
{
	try
	{
		m_hInternet = InternetOpen( m_strAgentName.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( ! m_hInternet )
		{
			m_strLastError = "Cannot open internet";
			m_lastErrorCode = GetLastError();
			return false;
		}

		m_hSession = InternetConnect
		(
			m_hInternet, 
			m_strServerName.c_str(), 
			m_wPort,
			m_strUserName.c_str(), 
			m_strPassword.c_str(),
			INTERNET_SERVICE_HTTP,
			0,
			0
		);
		if ( ! m_hSession )
		{			
			m_strLastError = "Cannot connect to internet";
			m_lastErrorCode = GetLastError();
			ClearHandles();
			return false;
		}

		m_hRequest = HttpOpenRequest
		(
			m_hSession, 
			m_strVerb.c_str(),
			m_strUri.c_str(),
			NULL,
			"",
			NULL,
			m_secureFlags, 
			m_ReqID
		);
		if ( ! m_hRequest )
		{
			m_strLastError = "Cannot perform http request";
			m_lastErrorCode = GetLastError();
			ClearHandles();		
			return false;
		}
		
		m_ReqID++;
	}
	catch(...) 
	{
		m_strLastError = "Memory Exception occured";
		m_lastErrorCode = GetLastError();
		return false;
	}
	return true;
}

bool CVwSslHttp::SendHttpsRequest()
{
	const TCHAR * szContentType = "Content-Type: application/x-www-form-urlencoded\r\n";
	int result;
	int nPostDataLen;

	try
	{
		nPostDataLen = strlen( m_strPostData.c_str() );

		for ( int tries = 0; tries < 20; ++tries ) 
		{
			if ( nPostDataLen )
			{
				result = HttpSendRequest
				(
					m_hRequest,
					szContentType,
					strlen(szContentType),
					reinterpret_cast<LPVOID>( (LPVOID)m_strPostData.c_str() ),
					strlen( m_strPostData.c_str() )
				);
			}
			else
			{
				result = HttpSendRequest( m_hRequest, NULL, 0, NULL, 0 );
			}

			if ( result )
			{
				return true;
			}
			if ( FALSE == result && 12168 == ::GetLastError() )
			{
				return true;
			}

			int lastErr = GetLastError();
			if ( lastErr == ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED )
			{
				if ( ! SetClientCert() )
				{
					m_strLastError = "Cannot perform http request, client authentication needed but couldnt detect required client certificate";
					m_lastErrorCode = GetLastError();
					return false;
				}
			}
			else if ( lastErr == ERROR_INTERNET_INVALID_CA )
			{
				m_strLastError = "Cannot perform http request, client authentication needed, invalid client certificate is used";
				m_lastErrorCode = GetLastError();
				return false;
			}
			else
			{
				m_strLastError = "Cannot perform http request";
				m_lastErrorCode = GetLastError();
				return false;
			}
		}
	}
	catch(...)
	{
		m_strLastError = "Memory Exception occured";
		m_lastErrorCode = GetLastError();
		return false;
	}
	return false;
}

void CVwSslHttp::ClearHandles()
{
	if ( m_hInternet )
	{
		InternetCloseHandle( m_hInternet );
		m_hInternet = NULL;
	}

	if ( m_hSession )
	{
		InternetCloseHandle( m_hSession );
		m_hSession = NULL;
	}

	if ( m_pContext )
	{
		CertFreeCertificateContext( m_pContext );
		m_pContext = NULL;
	}
	if ( m_hStore ) 
	{
		CertCloseStore( m_hStore, CERT_CLOSE_STORE_FORCE_FLAG );
		m_hStore = NULL;
	}
}

bool CVwSslHttp::SetClientCert()
{
	char *lpszStoreName;
	switch (m_certStoreType)
	{
	case certStoreMY:
		lpszStoreName = "MY";
		break;
	case certStoreCA:
		lpszStoreName = "CA";
		break;
	case certStoreROOT:
		lpszStoreName = "ROOT";
		break;
	case certStoreSPC:
		lpszStoreName = "SPC";
		break;
	}

 	m_hStore = CertOpenSystemStore( NULL, lpszStoreName );
	if ( ! m_hStore ) 
	{
		m_strLastError = "Cannot open system store ";
		m_strLastError += lpszStoreName;
		m_lastErrorCode = GetLastError();
		ClearHandles();
		return false;
	}

	m_pContext = FindCertWithOUNITName();

	if ( ! m_pContext )
	{
		m_strLastError = "Cannot find the required certificate";
		m_lastErrorCode = GetLastError();
		ClearHandles();
		return false;
	}
	
	//	INTERNET_OPTION_CLIENT_CERT_CONTEXT is 84
	int res = InternetSetOption
		(
			m_hRequest, 
			INTERNET_OPTION_CLIENT_CERT_CONTEXT, 
			(void *) m_pContext, sizeof(CERT_CONTEXT)
		);
	if ( ! res )
	{
		m_strLastError = "Cannot set certificate context";
		m_lastErrorCode = GetLastError();
		ClearHandles();
		return false;
	}

	return true;
}

PCCERT_CONTEXT CVwSslHttp::FindCertWithOUNITName()
{
	//This function performs a certificate contex search
	//by the organizational unit name of the issuer
	//Take this function as a sample for your possible different search functions
	PCCERT_CONTEXT pCertContext = NULL;		
	CERT_RDN certRDN;

	certRDN.cRDNAttr		= 1;
	certRDN.rgRDNAttr		= new CERT_RDN_ATTR;
	certRDN.rgRDNAttr->pszObjId	= szOID_ORGANIZATIONAL_UNIT_NAME;
	certRDN.rgRDNAttr->dwValueType	= CERT_RDN_ANY_TYPE;
	certRDN.rgRDNAttr->Value.pbData	= (BYTE *) m_strOrganizationName.c_str();
	certRDN.rgRDNAttr->Value.cbData	= strlen(m_strOrganizationName.c_str());

	pCertContext = CertFindCertificateInStore
		(
			m_hStore, 
			X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 
			0,
			CERT_FIND_ISSUER_ATTR,
			&certRDN,
			NULL
		);

	delete certRDN.rgRDNAttr;
	return pCertContext;
}

/**
 *	@ public
 *	获取 http 头
 */
string CVwSslHttp::GetResponseHeader()
{
	DWORD dwNumberOfBytesRead;
	char  szHeader[ 1024 ];
	string strResult;

	if ( ConnectToHttpsServer() )
	{
		if ( SendHttpsRequest() )
		{
			//	读取 http header
			memset( szHeader, 0, sizeof(szHeader) );
			dwNumberOfBytesRead = sizeof(szHeader);
			HttpQueryInfo( m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, szHeader, &dwNumberOfBytesRead, NULL );
			szHeader[ dwNumberOfBytesRead ] = '\0';
			strResult = szHeader;
		}
	}
 
	return strResult;
}

/**
 *	@ public
 *	获取 header/body
 */
string CVwSslHttp::GetRequestResult( BOOL bGetHeader )
{
	DWORD dwNumberOfBytesRead;
	char  szHeader[ 1024 ];
	char sz[1024];
	string strResult;
	int result;

	if ( ConnectToHttpsServer() )
	{
		if ( SendHttpsRequest() )
		{
			if ( bGetHeader )
			{
				//	读取 http header
				memset( szHeader, 0, sizeof(szHeader) );
				dwNumberOfBytesRead = sizeof(szHeader);
				HttpQueryInfo( m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, szHeader, &dwNumberOfBytesRead, NULL );
				szHeader[ dwNumberOfBytesRead ] = '\0';
				strResult += szHeader;
			}
			
			do
			{
				dwNumberOfBytesRead = 0;
				result = InternetReadFile( m_hRequest, sz, 1023, &dwNumberOfBytesRead );
				sz[ dwNumberOfBytesRead ] = '\0';
				int x = strlen( sz );
				strResult += sz;
				memset( sz, 0, sizeof(sz) );
			}
			while( result && dwNumberOfBytesRead != 0 );
		}
	}

	return strResult;
}