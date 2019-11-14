// SslCon: interface for the CSslConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINETSEC_H__91AD1B9B_5B03_457E_A6B6_D66BB03147B7__INCLUDED_)
#define AFX_WINETSEC_H__91AD1B9B_5B03_457E_A6B6_D66BB03147B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <iostream>
using namespace std;

#include "winInet.h"
#pragma comment( lib, "winInet.lib" )
#pragma comment( lib, "crypt32.lib" )

#include <wincrypt.h>

#pragma warning(disable:4786)

#include <string>
using namespace std;

enum CertStoreType { certStoreMY, certStoreCA, certStoreROOT, certStoreSPC };

class CVwSslHttp  
{
public:
	CVwSslHttp();
	virtual ~CVwSslHttp();	
public:
	bool ConnectToHttpsServer();
	bool SendHttpsRequest();

	string GetResponseHeader();
	string GetRequestResult( BOOL bGetHeader );

public: //accessors
	void SetAgentName( string &strAgentName ) { m_strAgentName = strAgentName; }
	void SetCertStoreType( CertStoreType storeID ) { m_certStoreType = storeID; }
	void SetVerb( string &strVerb ) { m_strVerb = strVerb; }
	void SetServerName( string &strServerName ) { m_strServerName = strServerName; }
	void SetUri( string &strUri ) { m_strUri = strUri; }
	void SetPort( INTERNET_PORT wPort = INTERNET_DEFAULT_HTTPS_PORT ) { m_wPort = wPort; }

	//	assign a unique number for each internet connection
	void SetRequestID( int reqID ) { m_ReqID = reqID; }

	void SetSecurityFlags( int flags ) { m_secureFlags = flags; }
	void SetPostData( string &strPostData ) { m_strVerb = "POST"; m_strPostData = strPostData; }

	//	Search indicators
	//	You may choose any field of a certificate to perform a context search, 
	//	i just implemented the OU field of the Issuer here	
	void SetOrganizationName( string &strOName ) { m_strOrganizationName = strOName; }

	string GetLastErrorString() { return m_strLastError; }
	int GetLastErrorCode() { return m_lastErrorCode; }

private:
	// examine the following function in order to perform different certificate 
	// property searchs in stores. It detects the desired certificate with the organization name
	PCCERT_CONTEXT FindCertWithOUNITName();	
	/////////////////////////////////////
	bool SetClientCert();
	void ClearHandles();
private:
	HINTERNET m_hInternet;
	HINTERNET m_hRequest;
	HINTERNET m_hSession;
	
	string m_strVerb;
	string m_strServerName;
	string m_strUri;
	INTERNET_PORT m_wPort;
	int m_secureFlags;
	string m_strPostData;

	HCERTSTORE m_hStore;
	PCCERT_CONTEXT m_pContext;
	CertStoreType m_certStoreType;
	string m_strUserName;
	string m_strPassword;
	string m_strAgentName;
	string m_strOrganizationName;
	string m_strLastError;
	int m_lastErrorCode;
	int m_ReqID;
};

#endif // !defined(AFX_WINETSEC_H__91AD1B9B_5B03_457E_A6B6_D66BB03147B7__INCLUDED_)

