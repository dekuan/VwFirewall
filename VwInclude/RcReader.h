// RcReader.h: interface for the CExecImageVersion class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __RC_READER_HEADER__
#define __RC_READER_HEADER__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma comment (lib, "version.lib")



struct LANGANDCODEPAGE {
	WORD wLanguage;
	WORD wCodePage;
};



class CRcReader
{
public:
	CRcReader();
	virtual ~CRcReader();

public:
	////////////////////////////////////////
	BOOL	InitInfo( TCHAR * pszError );
	VOID	FreeInfo();

	BOOL	GetSpecialBuild( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetPrivateBuild( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetLegalTrademarks( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetInternalName( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetFileVersion( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetFileDescription( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetComments( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetCopyright( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetCompanyName( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetProductVersion( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );
	BOOL	GetProductName( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError );




public:
	TCHAR * m_pszFilePath;


private:
	LANGANDCODEPAGE * m_lpTranslate;
	UINT m_cbTranslate;

	BOOL	m_bIsSucc;
	DWORD	m_dwHandle;
	DWORD	m_dwSize;
	LPVOID	m_lpBuffer;
	LPVOID	m_lpData;
	UINT	m_uiDataSize;
};







#endif // __RC_READER_HEADER__
