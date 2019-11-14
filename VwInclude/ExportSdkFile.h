// ExportSdkFile.h: interface for the CExportSdkFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __EXPORTSDKFILE_HEADER__
#define __EXPORTSDKFILE_HEADER__


typedef struct tagSdkFileConfig
{
	tagSdkFileConfig()
	{
		memset( this, 0, sizeof(tagSdkFileConfig) );
	}
	BOOL  bAutoSetCookie;
	TCHAR szPubkey[ MAX_PATH ];
	DWORD dwPubKeyTime;
	DWORD dwPubKeyType;
	DWORD dwTimeSpan;
	TCHAR szDomain[ MAX_PATH ];

}STSDKFILECONFIG, *PSTSDKFILECONFIG;

typedef struct tagReplaceTemplate
{
	TCHAR szFind[ MAX_PATH ];
	TCHAR szRepFmt[ MAX_PATH ];

}STREPLACETEMPLATE, *PSTREPLACETEMPLATE;


class CExportSdkFile
{
public:
	CExportSdkFile();
	virtual ~CExportSdkFile();

	BOOL SetConfig( STSDKFILECONFIG * pstCfg );
	BOOL ExportSdkFile( UINT uResId, LPCSTR lpcszFilename );
	BOOL MakeNewSdkFile( UINT uResId, LPCSTR lpcszFilename );
	
	BOOL BrowseForFolder( LPCTSTR lpcszTitle, LPTSTR lpszPath, DWORD dwSize );

	TCHAR  m_szOutFile[ MAX_PATH ];

private:
	STSDKFILECONFIG	m_stCfg;
};

#endif // __EXPORTSDKFILE_HEADER__




