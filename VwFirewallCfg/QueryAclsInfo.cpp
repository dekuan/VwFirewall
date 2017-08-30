// QueryAclsInfo.cpp: implementation of the CQueryAclsInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "QueryAclsInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryAclsInfo::CQueryAclsInfo()
{
}
CQueryAclsInfo::~CQueryAclsInfo()
{
}

BOOL CQueryAclsInfo::GetSpecUserAclsInfo( LPCTSTR lpcszFilename, LPCTSTR lpcszUser, BYTE btAceType, ACCESS_MASK * pMask )
{
	//
	//	lpcszFilename	- [in]
	//	lpcszUser	- [in]
	//	btAceType	- [in] ACCESS_ALLOWED_ACE_TYPE / ACCESS_DENIED_ACE_TYPE
	//	pMask		- [out]
	//	RETURN		-
	//

	BOOL bRet;
	BOOL bGetFileSec;
	DWORD dwSize;
	PSECURITY_DESCRIPTOR pstSD;
	SECURITY_INFORMATION si;
	STQUERYACLSINFOACEINFO stAceInfo;
	TCHAR szNewFilename[ MAX_PATH ];

	if ( NULL == lpcszFilename || 0 == _tcslen( lpcszFilename ) || ! PathFileExists( lpcszFilename ) )
	{
		return FALSE;
	}
	if ( NULL == lpcszUser || 0 == _tcslen( lpcszUser ) )
	{
		return FALSE;
	}
	if ( NULL == pMask )
	{
		return FALSE;
	}


	//	..
	bRet		= FALSE;
	bGetFileSec	= FALSE;
	dwSize		= 0;
	pstSD		= NULL;
	si		= OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION;
	//si		= DACL_SECURITY_INFORMATION;


	__try
	{
		_sntprintf( szNewFilename, sizeof(szNewFilename)/sizeof(TCHAR)-1, _T("%s"), lpcszFilename );
		StrTrim( szNewFilename, _T("\r\n\t \"\'") );

		//
		//	enable the privilege
		//
		MyPrivilege( SE_SECURITY_NAME, TRUE );	

		//
		//	get the size
		//
		if ( GetFileSecurity( szNewFilename, si, pstSD, dwSize, &dwSize) )
		{
			bGetFileSec = TRUE;
		}
		else if ( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
		{
			pstSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, dwSize );			
			if ( GetFileSecurity( szNewFilename, si, pstSD, dwSize, &dwSize ) )
			{
				bGetFileSec = TRUE;
			}
			else
			{
				// DisplayError(GetLastError(), TEXT("GetFileSecurity"));
			}
		}
		
		// ..
		if ( bGetFileSec )
		{
			memset( &stAceInfo, 0, sizeof(stAceInfo) );
			if ( DumpSDInfo( lpcszUser, btAceType, pstSD, &stAceInfo ) )
			{
				//	...
				*pMask = stAceInfo.Mask;
				bRet = TRUE;
			}
		}
		if ( pstSD )
		{
			LocalFree( pstSD );
			pstSD = NULL;
		}

		//
		//	disable the privilege
		//
		MyPrivilege( SE_SECURITY_NAME, FALSE );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	// ..
	return bRet;
}

VOID CQueryAclsInfo::CheckPermissions( ACCESS_MASK amMask )
{
	INT nTemp;

	nTemp = 0;
	////////////////////////////////////////////////////////////
	// Generic
	if ( GENERIC_READ == (amMask & GENERIC_READ) )
	{
		nTemp = GENERIC_READ;
	}
	if ( GENERIC_WRITE == (amMask & GENERIC_WRITE) )
	{
		nTemp = GENERIC_WRITE;
	}
	if ( GENERIC_EXECUTE == (amMask & GENERIC_EXECUTE) )
	{
		nTemp = GENERIC_EXECUTE;
	}
	if ( GENERIC_ALL == (amMask & GENERIC_ALL) )
	{
		nTemp = GENERIC_ALL;
	}
	////////////////////////////////////////////////////////////
	// Standard
	if ( MAXIMUM_ALLOWED == (amMask & MAXIMUM_ALLOWED) )
	{
		nTemp = MAXIMUM_ALLOWED;
	}
	if ( STANDARD_RIGHTS_ALL == (amMask & STANDARD_RIGHTS_ALL) )
	{
		nTemp = STANDARD_RIGHTS_ALL;
	}
	if ( STANDARD_RIGHTS_EXECUTE == (amMask & STANDARD_RIGHTS_EXECUTE) )
	{
		nTemp = STANDARD_RIGHTS_EXECUTE;
	}
	if ( STANDARD_RIGHTS_READ == (amMask & STANDARD_RIGHTS_READ) )
	{
		nTemp = STANDARD_RIGHTS_READ;
	}
	if ( STANDARD_RIGHTS_REQUIRED == (amMask & STANDARD_RIGHTS_REQUIRED) )
	{
		nTemp = STANDARD_RIGHTS_REQUIRED;
	}
	if ( STANDARD_RIGHTS_WRITE == (amMask & STANDARD_RIGHTS_WRITE) )
	{
		nTemp = STANDARD_RIGHTS_WRITE;
	}	
	if ( SPECIFIC_RIGHTS_ALL == (amMask & SPECIFIC_RIGHTS_ALL) )
	{
		nTemp = SPECIFIC_RIGHTS_ALL;
	}
	
	////////////////////////////////////////////////////////////
	// Standard - 2
	if ( DELETE == (amMask & DELETE) )
	{
		nTemp = DELETE;
	}
	if ( READ_CONTROL == (amMask & READ_CONTROL) )
	{
		nTemp = READ_CONTROL;
	}
	if ( WRITE_DAC == (amMask & WRITE_DAC) )
	{
		nTemp = WRITE_DAC;
	}
	if ( WRITE_OWNER == (amMask & WRITE_OWNER) )
	{
		nTemp = WRITE_OWNER;
	}
	if ( SYNCHRONIZE == (amMask & SYNCHRONIZE) )
	{
		nTemp = SYNCHRONIZE;
	}
}

BOOL CQueryAclsInfo::IsDenyPermissionsExist( ACCESS_MASK Mask )
{
	BOOL bRet = FALSE;

	//
	//	Deny Permissions
	//	检测所有的 Deny 权限都存在
	//
	if ( STANDARD_RIGHTS_EXECUTE == ( Mask & STANDARD_RIGHTS_EXECUTE ) &&
		STANDARD_RIGHTS_READ == ( Mask & STANDARD_RIGHTS_READ ) &&
		STANDARD_RIGHTS_WRITE == ( Mask & STANDARD_RIGHTS_WRITE ) &&
		DELETE == ( Mask & DELETE ) &&
		READ_CONTROL == ( Mask & READ_CONTROL )
	)
	{
		bRet = TRUE;
	}
	if ( SYNCHRONIZE == ( Mask & SYNCHRONIZE ) )
	{
		if ( 1048577 == Mask )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}
BOOL CQueryAclsInfo::IsAllowPermissionsExist( ACCESS_MASK Mask )
{
	BOOL bRet = FALSE;
	
	//
	//	Remove Permissions
	//	检测只要有一个 Allow 权限存在就认为存在，接下来是删除该用户的权限
	//
	if ( GENERIC_READ == ( Mask & GENERIC_READ ) ||
		GENERIC_WRITE == ( Mask & GENERIC_WRITE ) ||
		GENERIC_EXECUTE == ( Mask & GENERIC_EXECUTE ) ||

		STANDARD_RIGHTS_READ == ( Mask & STANDARD_RIGHTS_READ ) ||
		STANDARD_RIGHTS_REQUIRED == ( Mask & STANDARD_RIGHTS_REQUIRED ) ||
		STANDARD_RIGHTS_EXECUTE == ( Mask & STANDARD_RIGHTS_EXECUTE ) ||
		STANDARD_RIGHTS_WRITE == ( Mask & STANDARD_RIGHTS_WRITE ) ||

		DELETE == ( Mask & DELETE ) ||
		READ_CONTROL == ( Mask & READ_CONTROL ) ||

		WRITE_DAC == ( Mask & WRITE_DAC ) ||
		WRITE_OWNER == ( Mask & WRITE_OWNER )
	)
	{
		bRet = TRUE;
	}
	if ( SYNCHRONIZE == ( Mask & SYNCHRONIZE ) )
	{
		if ( 1048614 == Mask )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}




//////////////////////////////////////////////////////////////////////////
//	Private
//

BOOL CQueryAclsInfo::MyPrivilege( LPCTSTR lpszSeName, BOOL fEnable )
{
	// Enabling the debug privilege allows the application to see
	// information about service applications
	
	HANDLE hToken;
	BOOL fOK;

	__try
	{
		// Try to open this process's access token
		if ( OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) )
		{
			// Attempt to modify the "Debug" privilege
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			LookupPrivilegeValue(NULL, lpszSeName, &tp.Privileges[0].Luid);
			tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
			fOK = (ERROR_SUCCESS == GetLastError());
			CloseHandle(hToken);
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return fOK;
}

BOOL CQueryAclsInfo::DumpSDInfo( LPCTSTR lpcszUser, BYTE btAceType, PSECURITY_DESCRIPTOR pstSD, LPSTQUERYACLSINFOACEINFO pstAceInfo )
{
	//
	//	lpcszUser	- [in] 
	//	btAceType	- [in]
	//	pstSD		- [in]
	//	pstAceInfo	- [out]
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpcszUser || NULL == lpcszUser[0] )
	{
		return FALSE;
	}
	if ( NULL == pstSD )
	{
		return FALSE;
	}

	BOOL bRet;
	DWORD dwSize;
	BOOL bDaclPresent;
	BOOL bDaclDefaulted;
	PACL pDacl;

	ACL_SIZE_INFORMATION inf;
	DWORD dwNumAces;
	DWORD dwCnt;
	ACCESS_ALLOWED_ACE * pAce;
	//ACCESS_DENIED_ACE * pDeniedAce;

	//	...
	bRet	= FALSE;
	
	__try
	{
		dwSize	= GetSecurityDescriptorLength( pstSD );

		if ( GetSecurityDescriptorDacl( pstSD, &bDaclPresent, &pDacl, &bDaclDefaulted )
			&& bDaclPresent )
		{
			//	Dump the aces
			if ( GetAclInformation( pDacl, &inf, sizeof(ACL_SIZE_INFORMATION), AclSizeInformation ) )
			{
				dwNumAces = inf.AceCount;
				//	printf( "\nThe DACL has %d ACEs", dwNumAces );
				for ( dwCnt = 0; dwCnt < dwNumAces; dwCnt ++ )
				{
					pAce = NULL;
					if ( GetAce( pDacl, dwCnt, (LPVOID*)&pAce ) )
					{
						if ( pAce->Mask > 0 )
						{
							//	compare the AceType
							if ( btAceType == pAce->Header.AceType )
							{
								memset( pstAceInfo, 0, sizeof(STQUERYACLSINFOACEINFO) );

								//	Copy Mask information
								pstAceInfo->Mask = pAce->Mask;

								if ( DumpAceInfo( pAce, pstAceInfo ) )
								{
									if ( 0 == _tcsicmp( pstAceInfo->szUserName, lpcszUser ) )
									{
										bRet = TRUE;
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	// ..
	return bRet;
}

/**
 *	@ Private
 *	Dumps information in an ACE.
 *	Note that this is simple cook book code.
 *	There are many available wrappers to do this sort of thing.
 */
BOOL CQueryAclsInfo::DumpAceInfo( ACCESS_ALLOWED_ACE * pAce, LPSTQUERYACLSINFOACEINFO pstAceInfo )
{
	// printf("\n Ace, type=0x%x, flags=0x%x, mask=0x%x",
	//	pAce->Header.AceType, pAce->Header.AceFlags, pAce->Mask);

	BOOL bRet;
	PSID pSid;
	SID_NAME_USE emSIDUse;
	TCHAR szUserName[ 64 ];
	TCHAR szDomain[ 64 ];
	WCHAR wszUser[ 64 ];
	WCHAR wszDomain[ 64 ];
	DWORD dwNameSize;
	DWORD dwDomainSize;

	bRet	= FALSE;
	pSid	= 0;
	memset( szUserName, 0, sizeof(szUserName) );
	memset( szDomain, 0, sizeof(szDomain) );
	dwNameSize	= sizeof(wszUser) / sizeof(WCHAR);
	dwDomainSize	= sizeof(wszDomain) / sizeof(WCHAR);

	__try
	{
		//	...
		pSid = &pAce->SidStart;
		if ( LookupAccountSidW(
			NULL,			//	name of local or remote computer
			pSid,			//	security identifier
			wszUser,		//	account name buffer
			&dwNameSize,		//	size of account name buffer
			wszDomain,		//	domain name
			&dwDomainSize,		//	size of domain name buffer
			&emSIDUse )
		)
		{
			WideCharToMultiByte( CP_ACP, 0, wszUser, wcslen(wszUser), szUserName, sizeof(szUserName), NULL, NULL );
			WideCharToMultiByte( CP_ACP, 0, wszDomain, wcslen(wszDomain), szDomain, sizeof(szDomain), NULL, NULL );
			if ( pstAceInfo )
			{
				//	...
				bRet = TRUE;

				_sntprintf( pstAceInfo->szUserName, sizeof(pstAceInfo->szUserName)-sizeof(TCHAR), "%s", szUserName );
				_sntprintf( pstAceInfo->szDomain, sizeof(pstAceInfo->szDomain)-sizeof(TCHAR), "%s", szDomain );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
/*
	// 获取操作权限
	pstAceInfo->amPermissionsMask = 0;
	memset( pstAceInfo->szPermissions, 0, sizeof(pstAceInfo->szPermissions) );
	// ..
	switch( pAce->Header.AceType )
	{
	case ACCESS_ALLOWED_ACE_TYPE:
		{
			pstAceInfo->amPermissionsMask = pAce->Mask;
			GetPermissionsDesc( pAce->Mask, pstAceInfo->szPermissions, sizeof(pstAceInfo->szPermissions) );
		}
		break;
	case ACCESS_DENIED_ACE_TYPE:
		{
			pstAceInfo->amPermissionsMask = pAce->Mask;
			GetPermissionsDesc( pAce->Mask, pstAceInfo->szPermissions, sizeof(pstAceInfo->szPermissions) );
		}
		break;
	default:
		{
			// wprintf(L"Unknown ACE Type");
		}
	}
*/
	//	...
	return bRet;
}



