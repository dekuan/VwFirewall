// ShortcutLink.cpp: implementation of the CShortcutLink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "lashou.h"
#include "ShortcutLink.h"




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShortcutLink::CShortcutLink()
{
	CoInitialize(NULL);
}
CShortcutLink::~CShortcutLink()
{
	CoUninitialize();
}

BOOL CShortcutLink::Create( LPCSTR pszCmdLine, LPCSTR pszWorkingDirectory, LPCSTR pszFilePath, LPCSTR pszDesc, LPTSTR pszError )
{
	
	
	BOOL bRet = FALSE;
	HRESULT hres;
	IShellLink *psl;
	
	// Create an IShellLink object and get a pointer to the IShellLink 
	// interface (returned from CoCreateInstance).
	hres = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (void **)&psl);
	if ( SUCCEEDED (hres) )
	{
		IPersistFile *ppf;
		
		// Query IShellLink for the IPersistFile interface for 
		// saving the shortcut in persistent storage.
		hres = psl->QueryInterface (IID_IPersistFile, (void **)&ppf);
		if ( SUCCEEDED(hres) )
		{ 
			WORD wsz [MAX_PATH]; // buffer for Unicode string
			
			// Set the work directory
			if ( pszWorkingDirectory && _tcslen(pszWorkingDirectory) > 0 )
			{
				psl->SetWorkingDirectory( pszWorkingDirectory );
			}
			
			// Set the path to the shortcut target.
			hres = psl->SetPath(pszCmdLine);
			
			if ( SUCCEEDED (hres) )
			{
				// Set the description of the shortcut.
				hres = psl->SetDescription (pszDesc);
				
				if ( SUCCEEDED(hres) )
				{
					// Ensure that the string consists of ANSI characters.
					MultiByteToWideChar( CP_ACP, 0, pszFilePath, -1, wsz, MAX_PATH );
					
					// Save the shortcut via the IPersistFile::Save member function.
					hres = ppf->Save( wsz, TRUE );
					if ( SUCCEEDED(hres) )
					{
						bRet = TRUE;
					}
					else
					{
						_tcscpy( pszError, "Save failed!" );
					}
				}
				else
				{
					_tcscpy( pszError, "SetDescription failed!" );
				}
				
			}
			else
			{
				_tcscpy( pszError, "SetPath failed!" );
			}
			
			// Release the pointer to IPersistFile.
			ppf->Release ();
		}
		// Release the pointer to IShellLink.
		psl->Release ();
	}
	
	
	
	return bRet;
}
