// ProcSecurityDesc.cpp: implementation of the CProcSecurityDesc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcSecurityDesc.h"

#include <stdio.h>
#include <stdlib.h>





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcSecurityDesc::CProcSecurityDesc()
{

}

CProcSecurityDesc::~CProcSecurityDesc()
{

}

/**
 *	@ Public
 *	设置某个目录或者文件的安全属性为 Everyone 可读
 */
BOOL CProcSecurityDesc::SetSecurityDescForEveryone( LPCTSTR lpcszPath, LPTSTR pszError )
{
	return CreateEveryoneSecurityDesc( lpcszPath, GENERIC_READ|GENERIC_EXECUTE, pszError );
}

/**
 *	@ Public
 *	设置某个目录或者文件的安全属性为 IIS 可控制
 */
BOOL CProcSecurityDesc::SetSecurityDescForFilter( LPCTSTR lpcszPath, LPTSTR pszError, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	return CreateIISSecurityDesc( lpcszPath, pszError, bPermissionsEveryoneWritable );
}

/**
 *	@ Public
 *	获取 Everyone 安全描述符数据
 */
BOOL CProcSecurityDesc::GetEveryoneSecurityAttributesData( LPSECURITY_ATTRIBUTES lpFileMappingAttributes, LPTSTR pszError )
{
	PSID pEveryoneSID				= NULL;
	PSID pAdminSID					= NULL;
	PACL pACL					= NULL;
	PSECURITY_DESCRIPTOR pSD			= NULL;
	EXPLICIT_ACCESS ea[ EXPLICIT_ENTRIES_NUM_EVERYONE ]	= {0};		// 安全对象数组
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld		= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT		= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal		= SECURITY_LOCAL_SID_AUTHORITY;
//	SECURITY_ATTRIBUTES sa;
	DWORD dwErrorCode				= 0;
	BOOL bIsSuccess					= FALSE;
	DWORD dwAccessPms				= GENERIC_READ|GENERIC_EXECUTE|GENERIC_WRITE;


	lpFileMappingAttributes	= NULL;

	__try
	{
		////////////////////////////////////////////////////////////
		// 初始化 安全描述符
		memset( &ea, 0, EXPLICIT_ENTRIES_NUM_EVERYONE * sizeof(EXPLICIT_ACCESS) );

		////////////////////////////////////////////////////////////
		// 1.1 - Everyone
		// Create a well-known SID for the Everyone group
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthWorld,		// 基础 SID 结构体地址
				1,			// 要初始化下面几个“子权利项”
				SECURITY_WORLD_RID,	// 这个就是上面指定的要初始化的“子权利项”，其下的 7 个将被忽略
				0, 0, 0, 0, 0, 0, 0,	// 这里是剩下的 7 个“子权利项”
				&pEveryoneSID
			);
		if( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建 Everyone 组 SID，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// 1.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow Everyone read access to the key.

		ea[0].grfAccessPermissions	= dwAccessPms;	//GENERIC_ALL;	// 给 Everyone 读写权限
		ea[0].grfAccessMode		= SET_ACCESS;
		ea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
		ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName		= (LPTSTR)pEveryoneSID;

		////////////////////////////////////////////////////////////
		// 2.1 - Administrators
		// Create a SID for the BUILTIN\Administrators group.
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthNT,			// 基础 SID 结构体地址
				2,				// 要初始化下面几个“子权利项”
				SECURITY_BUILTIN_DOMAIN_RID,	// 上面指定的要初始化的第 1 个“子权利项”
				DOMAIN_ALIAS_RID_ADMINS,	// 上面指定的要初始化的第 2 个“子权利项”
				0, 0, 0, 0, 0, 0,		// 这里是剩下的 6 个“子权利项”
				&pAdminSID
			);
		if ( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建 Administrators 组 SID，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// 2.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow the Administrators group full access to the key.

		ea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
		ea[1].grfAccessMode		= SET_ACCESS;
		ea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
		ea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
		ea[1].Trustee.ptstrName		= (LPTSTR)pAdminSID;


		////////////////////////////////////////////////////////////
		// Create a new ACL that contains the new ACEs.
		if ( ERROR_SUCCESS != SetEntriesInAcl( EXPLICIT_ENTRIES_NUM_EVERYONE, ea, NULL, &pACL ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建新 ACL，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security descriptor.  

		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH );
		if ( pSD == NULL )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法为安全描述符分配内存空间，错误：%d"), dwErrorCode );
			return FALSE;
		}

		if ( ! InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法初始化安全描述符，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// Add the ACL to the security descriptor.
		bIsSuccess = FALSE;
		bIsSuccess = SetSecurityDescriptorDacl
			(
				pSD,
				TRUE,		// fDaclPresent flag
				pACL,
				FALSE		// not a default DACL
			);
		if ( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法将 ACL 添加到安全描述符中，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security attributes structure.
		lpFileMappingAttributes->nLength		= sizeof(SECURITY_ATTRIBUTES);
		lpFileMappingAttributes->lpSecurityDescriptor	= pSD;
		lpFileMappingAttributes->bInheritHandle		= FALSE;
	}
	__finally
	{
		if ( pEveryoneSID )
			FreeSid( pEveryoneSID );

		if ( pAdminSID )
			FreeSid( pAdminSID );

		if ( pACL )
			LocalFree( pACL );

		if ( pSD )
			LocalFree(pSD);
	}


	return TRUE;
}

/**
 *	获得 Local System + Administrator 权限
 */
BOOL CProcSecurityDesc::GetLocalSystemSecurityAttributesData( LPSECURITY_ATTRIBUTES lpFileMappingAttributes, LPTSTR pszError )
{
	PSID pSystemSID					= NULL;
	PSID pAdminSID					= NULL;
	PACL pACL					= NULL;
	PSECURITY_DESCRIPTOR pSD			= NULL;
	EXPLICIT_ACCESS ea[ 2 ]				= {0};		// 安全对象数组
//	SID_IDENTIFIER_AUTHORITY SIDAuthWorld		= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT		= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal		= SECURITY_LOCAL_SID_AUTHORITY;
//	SECURITY_ATTRIBUTES sa;
	DWORD dwErrorCode				= 0;
	BOOL bIsSuccess					= FALSE;
	DWORD dwAccessPms				= GENERIC_READ|GENERIC_EXECUTE|GENERIC_WRITE;


	lpFileMappingAttributes	= NULL;

	__try
	{
		////////////////////////////////////////////////////////////
		// 初始化 安全描述符
		memset( &ea, 0, 2 * sizeof(EXPLICIT_ACCESS) );

		////////////////////////////////////////////////////////////
		// 1.1 - System
		bIsSuccess = FALSE;
		bIsSuccess &= AllocateAndInitializeSid
			(
				&SIDAuthNT,			//	This structure provides the top-level identifier authority value to set in the SID 基础 SID 结构体地址
				1,				//	要初始化下面几个“子权利项”
				SECURITY_LOCAL_SYSTEM_RID,	//	这个就是上面指定的要初始化的“子权利项”，其下的 7 个将被忽略
				0, 0, 0, 0, 0, 0, 0,		//	这里是剩下的 7 个“子权利项”
				&pSystemSID
			);
		if( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建 System 组 SID，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// 1.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow Everyone read access to the key.

	//	ea[0].grfAccessPermissions	= dwAccessPms;	//GENERIC_ALL;	// 给 Everyone 读写权限
	//	ea[0].grfAccessMode		= SET_ACCESS;
	//	ea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
	//	ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
	//	ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
	//	ea[0].Trustee.ptstrName		= (LPTSTR)pSystemSID;


		ea[0].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// GENERIC_ALL;	// 给 SYSTEM 读写权限
		ea[0].grfAccessMode		= SET_ACCESS;
		ea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
		ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName		= (LPTSTR)pSystemSID;


		////////////////////////////////////////////////////////////
		// 2.1 - Administrators
		// Create a SID for the BUILTIN\Administrators group.
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthNT,			// 基础 SID 结构体地址
				2,				// 要初始化下面几个“子权利项”
				SECURITY_BUILTIN_DOMAIN_RID,	// 上面指定的要初始化的第 1 个“子权利项”
				DOMAIN_ALIAS_RID_ADMINS,	// 上面指定的要初始化的第 2 个“子权利项”
				0, 0, 0, 0, 0, 0,		// 这里是剩下的 6 个“子权利项”
				&pAdminSID
			);
		if ( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建 Administrators 组 SID，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// 2.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow the Administrators group full access to the key.

		ea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
		ea[1].grfAccessMode		= SET_ACCESS;
		ea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
		ea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
		ea[1].Trustee.ptstrName		= (LPTSTR)pAdminSID;


		////////////////////////////////////////////////////////////
		// Create a new ACL that contains the new ACEs.
		if ( ERROR_SUCCESS != SetEntriesInAcl( EXPLICIT_ENTRIES_NUM_EVERYONE, ea, NULL, &pACL ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建新 ACL，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security descriptor.  

		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH );
		if ( pSD == NULL )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法为安全描述符分配内存空间，错误：%d"), dwErrorCode );
			return FALSE;
		}

		if ( ! InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法初始化安全描述符，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// Add the ACL to the security descriptor.
		bIsSuccess = FALSE;
		bIsSuccess = SetSecurityDescriptorDacl
			(
				pSD,
				TRUE,		// fDaclPresent flag
				pACL,
				FALSE		// not a default DACL
			);
		if ( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法将 ACL 添加到安全描述符中，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security attributes structure.
		lpFileMappingAttributes->nLength		= sizeof(SECURITY_ATTRIBUTES);
		lpFileMappingAttributes->lpSecurityDescriptor	= pSD;
		lpFileMappingAttributes->bInheritHandle		= FALSE;
	}
	__finally
	{
		if ( pSystemSID )
		{
			FreeSid( pSystemSID );
			pSystemSID = NULL;
		}

		if ( pAdminSID )
		{
			FreeSid( pAdminSID );
			pAdminSID = NULL;
		}

		if ( pACL )
		{
			LocalFree( pACL );
			pACL = NULL;
		}

		if ( pSD )
		{
			LocalFree( pSD );
			pSD = NULL;
		}
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
//
// 创建 NTFS 上有 EveryOne 属性的目录
//
BOOL CProcSecurityDesc::CreateEveryoneSecurityDesc( LPCTSTR lpszPath, DWORD dwAccessPms, LPTSTR pszError )
{
	////////////////////////////////////////
	// 给新建的目录加上安全属性
	PSID pEveryoneSID				= NULL;
	PSID pAdminSID					= NULL;
	PACL pACL					= NULL;
	PSECURITY_DESCRIPTOR pSD			= NULL;
	EXPLICIT_ACCESS ea[ EXPLICIT_ENTRIES_NUM_EVERYONE ]	= {0};		// 安全对象数组
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld		= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT		= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal		= SECURITY_LOCAL_SID_AUTHORITY;
	SECURITY_ATTRIBUTES sa;
	DWORD dwErrorCode				= 0;
	BOOL bIsSuccess					= FALSE;


	__try
	{

		////////////////////////////////////////////////////////////
		// 初始化 安全描述符
		memset( &ea, 0, EXPLICIT_ENTRIES_NUM_EVERYONE * sizeof(EXPLICIT_ACCESS) );


		////////////////////////////////////////////////////////////
		// 1.1 - Everyone
		// Create a well-known SID for the Everyone group
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthWorld,		// 基础 SID 结构体地址
				1,			// 要初始化下面几个“子权利项”
				SECURITY_WORLD_RID,	// 这个就是上面指定的要初始化的“子权利项”，其下的 7 个将被忽略
				0, 0, 0, 0, 0, 0, 0,	// 这里是剩下的 7 个“子权利项”
				&pEveryoneSID
			);
		if( FALSE == bIsSuccess )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建 Everyone 组 SID，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// 1.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow Everyone read access to the key.

		ea[0].grfAccessPermissions	= dwAccessPms;	//GENERIC_ALL;	// 给 Everyone 读写权限
		ea[0].grfAccessMode		= SET_ACCESS;
		ea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
		ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName		= (LPTSTR)pEveryoneSID;


		////////////////////////////////////////////////////////////
		// 2.1 - Administrators
		// Create a SID for the BUILTIN\Administrators group.
		bIsSuccess = FALSE;
		bIsSuccess = AllocateAndInitializeSid
			(
				&SIDAuthNT,			// 基础 SID 结构体地址
				2,				// 要初始化下面几个“子权利项”
				SECURITY_BUILTIN_DOMAIN_RID,	// 上面指定的要初始化的第 1 个“子权利项”
				DOMAIN_ALIAS_RID_ADMINS,	// 上面指定的要初始化的第 2 个“子权利项”
				0, 0, 0, 0, 0, 0,		// 这里是剩下的 6 个“子权利项”
				&pAdminSID
			);
		if ( FALSE == bIsSuccess ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建 Administrators 组 SID，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// 2.2
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow the Administrators group full access to the key.

		ea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
		ea[1].grfAccessMode		= SET_ACCESS;
		ea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
		ea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
		ea[1].Trustee.ptstrName		= (LPTSTR)pAdminSID;



		////////////////////////////////////////////////////////////
		// Create a new ACL that contains the new ACEs.
		if ( ERROR_SUCCESS != SetEntriesInAcl( EXPLICIT_ENTRIES_NUM_EVERYONE, ea, NULL, &pACL ) )
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建新 ACL，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security descriptor.  

		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH );
		if ( pSD == NULL ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法为安全描述符分配内存空间，错误：%d"), dwErrorCode );
			return FALSE;
		}

		if ( ! InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法初始化安全描述符，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// Add the ACL to the security descriptor.
		bIsSuccess = FALSE;
		bIsSuccess = SetSecurityDescriptorDacl(
				pSD,
				TRUE,		// fDaclPresent flag
				pACL,
				FALSE );	// not a default DACL
		if ( FALSE == bIsSuccess ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法将 ACL 添加到安全描述符中，错误：%d"), dwErrorCode );
			return FALSE;
		}

		// Initialize a security attributes structure.

		sa.nLength		= sizeof (SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor	= pSD;
		sa.bInheritHandle	= FALSE;


		dwErrorCode = 0;
		if ( FALSE == SetFileSecurity( lpszPath, DACL_SECURITY_INFORMATION, pSD ) ) {
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法将创建也无法修改“%s”的安全属性，错误：%d"), lpszPath, dwErrorCode );
			return FALSE;
		}

	}
	__finally
	{
		if ( pEveryoneSID )
			FreeSid( pEveryoneSID );

		if ( pAdminSID )
			FreeSid( pAdminSID );

		if ( pACL )
			LocalFree( pACL );

		if ( pSD )
			LocalFree(pSD);
	}


	return TRUE;

}


/**
*	Get Windows system type
*/
ENUMDEWINDOWSSYSTYPE CProcSecurityDesc::GetShellSysType()
{
	ENUMDEWINDOWSSYSTYPE ShellType;
	DWORD dwWinVer;
	//OSVERSIONINFO * osvi;
	OSVERSIONINFOEX stOsVerEx;
	TCHAR szTemp[ MAX_PATH ];
	
	ShellType = _DEOS_UNKNOWN;
	
	__try
	{
		ZeroMemory( & stOsVerEx, sizeof(stOsVerEx) );
		
		dwWinVer = GetVersion();
		if ( dwWinVer < 0x80000000 )
		{
			// NT
			ShellType = _DEOS_WINDOWS_NT3;
			stOsVerEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			if ( GetVersionEx( (OSVERSIONINFO*)&stOsVerEx ) )
			{
				_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1,
					_T("stOsVerEx.dwMajorVersion=%d, stOsVerEx.dwMinorVersion=%d"),
					stOsVerEx.dwMajorVersion,
					stOsVerEx.dwMinorVersion );
				MessageBox( NULL, szTemp, NULL, NULL );

				if ( 4L == stOsVerEx.dwMajorVersion )
				{
					ShellType = _DEOS_WINDOWS_NT4;
				}
				else if ( 5L == stOsVerEx.dwMajorVersion && 0L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_2000;
				}
				else if ( 5L == stOsVerEx.dwMajorVersion && 1L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_XP;
				}
				else if ( 5L == stOsVerEx.dwMajorVersion && 2L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_2003;
				}
				else if ( 6L == stOsVerEx.dwMajorVersion && 1L == stOsVerEx.dwMinorVersion )
				{
					if ( VER_NT_WORKSTATION == stOsVerEx.wProductType )
					{
						//	Windows 7
						//	OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
						ShellType = _DEOS_WINDOWS_7;
					}
					else
					{
						//	Windows Server 2008 R2
						//	OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
						ShellType = _DEOS_WINDOWS_2008_R2;
					}
				}
				else if ( 6L == stOsVerEx.dwMajorVersion && 0L == stOsVerEx.dwMinorVersion )
				{
					if ( VER_NT_WORKSTATION == stOsVerEx.wProductType )
					{
						//	Windows Vista
						//	OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
						ShellType = _DEOS_WINDOWS_VISTA;
					}
					else
					{
						//	Windows Server 2008
						//	OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
						ShellType = _DEOS_WINDOWS_2008;
					}
				}
			}
		}
		else if ( LOBYTE(LOWORD(dwWinVer)) < 4 )
		{
			ShellType = _DEOS_WINDOWS_32S;
		}
		else
		{
			ShellType = _DEOS_WINDOWS_95;
			stOsVerEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if ( GetVersionEx( (OSVERSIONINFO*) &stOsVerEx ) )
			{
				if ( 4L == stOsVerEx.dwMajorVersion && 10L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_98;
				}
				else if ( 4L == stOsVerEx.dwMajorVersion && 90L == stOsVerEx.dwMinorVersion )
				{
					ShellType = _DEOS_WINDOWS_ME;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return ShellType;
}
//////////////////////////////////////////////////////////////////////////
//
// 创建 NTFS 上有 IIS 控制属性的目录
//
/*
 * ------------------------------------------------------------
 *  下面做标记的地方：
 *  ACE 的继承属性中选项如下：
 * ------------------------------------------------------------
 *	OBJECT_INHERIT_ACE			- 该文件夹及文件
 *	CONTAINER_INHERIT_ACE			- 该文件夹及子文件夹
 *	NO_PROPAGATE_INHERIT_ACE		- 只有该文件夹
 *	INHERIT_ONLY_ACE			- 只有文件
 *	INHERITED_ACE
 *	VALID_INHERIT_FLAGS
 *	SUB_CONTAINERS_ONLY_INHERIT		- 只有子文件夹
 *	SUB_OBJECTS_ONLY_INHERIT		- 只有子文件
 *	SUB_CONTAINERS_AND_OBJECTS_INHERIT	- 该文件夹，子文件夹及文件
 */
BOOL CProcSecurityDesc::CreateIISSecurityDesc( LPCTSTR lpcszPath, LPTSTR pszError, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	//
	//	lpcszPath	- [in]  Pointer to a character null-termined string containing the file path
	//	dwAccessPms	- [in]  Specifies the access to file, for example : GENERIC_READ|GENERIC_EXECUTE|GENERIC_WRITE
	//	pszError	- [out] Pointer to a character null-termined string to receive the error information.
	//

	if ( NULL == lpcszPath || NULL == pszError )
	{
		return FALSE;
	}

	////////////////////////////////////////
	// 给新建的目录加上安全属性
	BOOL  bRet			= FALSE;
	ENUMDEWINDOWSSYSTYPE emOsType;
	PACL pNewACL			= NULL;
	PSECURITY_DESCRIPTOR pSD	= NULL;
	EXPLICIT_ACCESS ea[ EXPLICIT_ENTRIES_NUM_FILTER ]	= {0};		// 安全对象数组
	SECURITY_ATTRIBUTES sa;
	DWORD dwErrorCode		= 0;
	SECURITY_INFORMATION si		= OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION;


	// 为 IIS5 创建 EXPLICIT_ACCESS
	emOsType = delib_get_shellsystype();
	if ( _DEOS_WINDOWS_2003 == emOsType )
	{
		BuildEaFor_Iis60_2003( lpcszPath, ea, sizeof(ea)/sizeof(ea[0]), bPermissionsEveryoneWritable );
	}
	else if ( _DEOS_WINDOWS_2008 == emOsType || _DEOS_WINDOWS_2008_R2 == emOsType || _DEOS_WINDOWS_VISTA == emOsType || _DEOS_WINDOWS_7 == emOsType )
	{
		BuildEaFor_Iis70_2008( lpcszPath, ea, sizeof(ea)/sizeof(ea[0]), bPermissionsEveryoneWritable );
	}
	else
	{
		BuildEaFor_Iis50_2000( lpcszPath, ea, sizeof(ea)/sizeof(ea[0]), bPermissionsEveryoneWritable );
	}


	////////////////////////////////////////////////////////////
	// Create a new ACL that contains the new ACEs.
	// The SetEntriesInAcl function creates a new access control list (ACL) by merging new access control or audit control information into an existing ACL structure.			
	if ( ERROR_SUCCESS == SetEntriesInAcl( EXPLICIT_ENTRIES_NUM_FILTER, ea, NULL, &pNewACL ) )
	{
		// Initialize a security descriptor.
		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH );
		if ( pSD )
		{
			if ( InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) )
			{
				// Add the ACL to the security descriptor.
				if ( SetSecurityDescriptorDacl( pSD, TRUE, pNewACL, FALSE ) )
				{
					// Initialize a security attributes structure.
					sa.nLength		= sizeof(sa);
					sa.lpSecurityDescriptor	= pSD;
					sa.bInheritHandle	= FALSE;

					dwErrorCode = 0;
					if ( SetFileSecurity( lpcszPath, DACL_SECURITY_INFORMATION, pSD ) )
					{
						bRet = TRUE;
					}
					else
					{
						dwErrorCode = GetLastError();
						_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法将创建也无法修改“%s”的安全属性，错误：%d"), lpcszPath, dwErrorCode );
					}
				}
				else
				{
					dwErrorCode = GetLastError();
					_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法将 ACL 添加到安全描述符中，错误：%d"), dwErrorCode );
				}
			}
			else
			{
				dwErrorCode = GetLastError();
				_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法初始化安全描述符，错误：%d"), dwErrorCode );
			}

			// ..
			LocalFree(pSD);
		}
		else
		{
			dwErrorCode = GetLastError();
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法为安全描述符分配内存空间，错误：%d"), dwErrorCode );
		}

		// ..
		if ( pNewACL )
			LocalFree( pNewACL );
	}
	else
	{
		dwErrorCode = GetLastError();
		_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法创建新 ACL，错误：%d"), dwErrorCode );
	}

	// ..
	return bRet;
}



//////////////////////////////////////////////////////////////////////////
// 为 IIS 5.0/Win2000 创建 EXPLICIT_ACCESS
// 
BOOL CProcSecurityDesc::BuildEaFor_Iis50_2000( LPCTSTR lpcszPath, EXPLICIT_ACCESS * pea, DWORD dwEaCount, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	if ( NULL == pea || EXPLICIT_ENTRIES_NUM_FILTER != dwEaCount )
	{
		return FALSE;
	}

	BOOL bAllocateSucc	= TRUE;
	SID_IDENTIFIER_AUTHORITY SIDAuth	= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT	= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal	= SECURITY_LOCAL_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld	= SECURITY_WORLD_SID_AUTHORITY;
	PSID pSystemSID				= NULL;
	PSID pAdminSID				= NULL;
	PSID pPwUsrSID				= NULL;
	PSID pEveryoneSID			= NULL;


	memset( pea, 0, dwEaCount * sizeof(EXPLICIT_ACCESS) );

	////////////////////////////////////////////////////////////
	//	4.1 - SYSTEM
	//	Create a well-known SID for the SYSTEM
	bAllocateSucc &= AllocateAndInitializeSid
		(
			&SIDAuthNT,			// This structure provides the top-level identifier authority value to set in the SID 基础 SID 结构体地址
			1,				// 要初始化下面几个“子权利项”
			SECURITY_LOCAL_SYSTEM_RID,	// 这个就是上面指定的要初始化的“子权利项”，其下的 7 个将被忽略
			0, 0, 0, 0, 0, 0, 0,		// 这里是剩下的 7 个“子权利项”
			&pSystemSID
		);
	if ( bAllocateSucc )
	{
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow SYSTEM read access to the key.
		pea[0].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// GENERIC_ALL;	// 给 SYSTEM 读写权限
		pea[0].grfAccessMode		= SET_ACCESS;
		pea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
		pea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		pea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		pea[0].Trustee.ptstrName	= (LPTSTR)pSystemSID;

		////////////////////////////////////////////////////////////
		//	4.2 - Administrators
		//	Create a SID for the BUILTIN\Administrators group.
		bAllocateSucc &= AllocateAndInitializeSid
			(
				&SIDAuthNT,			// 基础 SID 结构体地址
				2,				// 要初始化下面几个“子权利项”
				SECURITY_BUILTIN_DOMAIN_RID,	// 上面指定的要初始化的第 1 个“子权利项”
				DOMAIN_ALIAS_RID_ADMINS,		// 上面指定的要初始化的第 2 个“子权利项”
				0, 0, 0, 0, 0, 0,		// 这里是剩下的 6 个“子权利项”
				&pAdminSID
			);
		if ( bAllocateSucc )
		{
			//	Initialize an EXPLICIT_ACCESS structure for an ACE.
			//	The ACE will allow the Administrators group full access to the key.
			pea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// dwAccessPms
			pea[1].grfAccessMode		= SET_ACCESS;
			pea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
			pea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
			pea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
			pea[1].Trustee.ptstrName	= (LPTSTR)pAdminSID;

			////////////////////////////////////////////////////////////
			//	4.3 - PowerUser
			//	Create a SID for the BUILTIN\Administrators group.
			bAllocateSucc &= AllocateAndInitializeSid
				(
					&SIDAuthNT,			// 基础 SID 结构体地址
					2,				// 要初始化下面几个“子权利项”
					SECURITY_BUILTIN_DOMAIN_RID,	// 上面指定的要初始化的第 1 个“子权利项”
					DOMAIN_ALIAS_RID_POWER_USERS,	// 上面指定的要初始化的第 2 个“子权利项”
					0, 0, 0, 0, 0, 0,		// 这里是剩下的 6 个“子权利项”
					&pPwUsrSID
				);
			if ( bAllocateSucc )
			{
				// Initialize an EXPLICIT_ACCESS structure for an ACE.
				// The ACE will allow the Administrators group full access to the key.
				pea[2].grfAccessPermissions	= STANDARD_RIGHTS_READ;	// dwAccessPms
				pea[2].grfAccessMode		= SET_ACCESS;
				pea[2].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
				pea[2].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
				pea[2].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
				pea[2].Trustee.ptstrName	= (LPTSTR)pPwUsrSID;

				// ..
				//FreeSid( pIisWpgSID );

				////////////////////////////////////////////////////////////
				//	4.3	Everyone
				//
				bAllocateSucc &= AllocateAndInitializeSid
					(
						&SIDAuthWorld,		// 基础 SID 结构体地址
						1,			// 要初始化下面几个“子权利项”
						SECURITY_WORLD_RID,	// 这个就是上面指定的要初始化的“子权利项”，其下的 7 个将被忽略
						0, 0, 0, 0, 0, 0, 0,	// 这里是剩下的 7 个“子权利项”
						&pEveryoneSID
					);
				if ( bAllocateSucc )
				{
					pea[3].grfAccessPermissions	= GENERIC_READ|GENERIC_EXECUTE;	// 给 Everyone 读写权限
					pea[3].grfAccessMode		= SET_ACCESS;
					pea[3].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
					pea[3].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
					pea[3].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
					pea[3].Trustee.ptstrName	= (LPTSTR)pEveryoneSID;
				}
			}
		}

		// ..
		//FreeSid( pSystemSID );
	}

	// ..
	return bAllocateSucc;
}

//////////////////////////////////////////////////////////////////////////
// 为 IIS 7.0/Win2008/2008R2 创建 EXPLICIT_ACCESS
//
BOOL CProcSecurityDesc::BuildEaFor_Iis70_2008( LPCTSTR lpcszPath, EXPLICIT_ACCESS * pea, DWORD dwEaCount, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	if ( NULL == pea || EXPLICIT_ENTRIES_NUM_FILTER != dwEaCount )
	{
		return FALSE;
	}

	BOOL bAllocateSucc	= TRUE;
	SID_IDENTIFIER_AUTHORITY SIDAuth	= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT	= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal	= SECURITY_LOCAL_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld	= SECURITY_WORLD_SID_AUTHORITY;
	PSID pSystemSID				= NULL;
	PSID pAdminSID				= NULL;
	PSID pPwUsrSID				= NULL;
	PSID pEveryoneSID			= NULL;


	memset( pea, 0, dwEaCount * sizeof(EXPLICIT_ACCESS) );

	////////////////////////////////////////////////////////////
	//	4.1 - SYSTEM
	//	Create a well-known SID for the SYSTEM
	bAllocateSucc &= AllocateAndInitializeSid
		(
			&SIDAuthNT,			// This structure provides the top-level identifier authority value to set in the SID 基础 SID 结构体地址
			1,				// 要初始化下面几个“子权利项”
			SECURITY_LOCAL_SYSTEM_RID,	// 这个就是上面指定的要初始化的“子权利项”，其下的 7 个将被忽略
			0, 0, 0, 0, 0, 0, 0,		// 这里是剩下的 7 个“子权利项”
			&pSystemSID
		);
	if ( bAllocateSucc )
	{
		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow SYSTEM read access to the key.
		pea[0].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// GENERIC_ALL;	// 给 SYSTEM 读写权限
		pea[0].grfAccessMode		= SET_ACCESS;
		pea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
		pea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		pea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		pea[0].Trustee.ptstrName	= (LPTSTR)pSystemSID;

		////////////////////////////////////////////////////////////
		//	4.2 - Administrators
		//	Create a SID for the BUILTIN\Administrators group.
		bAllocateSucc &= AllocateAndInitializeSid
			(
				&SIDAuthNT,			// 基础 SID 结构体地址
				2,				// 要初始化下面几个“子权利项”
				SECURITY_BUILTIN_DOMAIN_RID,	// 上面指定的要初始化的第 1 个“子权利项”
				DOMAIN_ALIAS_RID_ADMINS,		// 上面指定的要初始化的第 2 个“子权利项”
				0, 0, 0, 0, 0, 0,		// 这里是剩下的 6 个“子权利项”
				&pAdminSID
			);
		if ( bAllocateSucc )
		{
			//	Initialize an EXPLICIT_ACCESS structure for an ACE.
			//	The ACE will allow the Administrators group full access to the key.
			pea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// dwAccessPms
			pea[1].grfAccessMode		= SET_ACCESS;
			pea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
			pea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
			pea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
			pea[1].Trustee.ptstrName	= (LPTSTR)pAdminSID;

			////////////////////////////////////////////////////////////
			//	4.3 - PowerUser
			//	Create a SID for the BUILTIN\Administrators group.
			bAllocateSucc &= AllocateAndInitializeSid
				(
					&SIDAuthNT,			// 基础 SID 结构体地址
					2,				// 要初始化下面几个“子权利项”
					SECURITY_BUILTIN_DOMAIN_RID,	// 上面指定的要初始化的第 1 个“子权利项”
					DOMAIN_ALIAS_RID_POWER_USERS,	// 上面指定的要初始化的第 2 个“子权利项”
					0, 0, 0, 0, 0, 0,		// 这里是剩下的 6 个“子权利项”
					&pPwUsrSID
				);
			if ( bAllocateSucc )
			{
				// Initialize an EXPLICIT_ACCESS structure for an ACE.
				// The ACE will allow the Administrators group full access to the key.
				pea[2].grfAccessPermissions	= STANDARD_RIGHTS_READ;	// dwAccessPms
				pea[2].grfAccessMode		= SET_ACCESS;
				pea[2].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
				pea[2].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
				pea[2].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
				pea[2].Trustee.ptstrName	= (LPTSTR)pPwUsrSID;

				// ..
				//FreeSid( pIisWpgSID );

				////////////////////////////////////////////////////////////
				//	4.3	Everyone
				//
				bAllocateSucc &= AllocateAndInitializeSid
					(
						&SIDAuthWorld,		// 基础 SID 结构体地址
						1,			// 要初始化下面几个“子权利项”
						SECURITY_WORLD_RID,	// 这个就是上面指定的要初始化的“子权利项”，其下的 7 个将被忽略
						0, 0, 0, 0, 0, 0, 0,	// 这里是剩下的 7 个“子权利项”
						&pEveryoneSID
					);
				if ( bAllocateSucc )
				{
					if ( bPermissionsEveryoneWritable )
					{
						pea[3].grfAccessPermissions	= GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE;	// 给 Everyone 读写权限
					}
					else
					{
						pea[3].grfAccessPermissions	= GENERIC_READ|GENERIC_EXECUTE;		// 给 Everyone 读写权限
					}
					pea[3].grfAccessMode		= SET_ACCESS;
					pea[3].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
					pea[3].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
					pea[3].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
					pea[3].Trustee.ptstrName	= (LPTSTR)pEveryoneSID;
				}
			}
		}

		// ..
		//FreeSid( pSystemSID );
	}

	// ..
	return bAllocateSucc;
}

//////////////////////////////////////////////////////////////////////////
// 为 IIS 6.0/Win2003 创建 EXPLICIT_ACCESS
//
BOOL CProcSecurityDesc::BuildEaFor_Iis60_2003( LPCTSTR lpcszPath, EXPLICIT_ACCESS * pea, DWORD dwEaCount, BOOL bPermissionsEveryoneWritable /*= FALSE*/ )
{
	if ( NULL == pea || EXPLICIT_ENTRIES_NUM_FILTER != dwEaCount )
	{
		return FALSE;
	}

	BOOL bAllocateSucc			= TRUE;
	SID_IDENTIFIER_AUTHORITY SIDAuth	= SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT	= SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal	= SECURITY_LOCAL_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld	= SECURITY_WORLD_SID_AUTHORITY;
	PSID pSystemSID				= NULL;
	PSID pAdminSID				= NULL;
	PSID pIisWpgSID				= NULL;
	PSID pEveryoneSID			= NULL;

	memset( pea, 0, dwEaCount * sizeof(EXPLICIT_ACCESS) );
	
	////////////////////////////////////////////////////////////
	//	4.1 - SYSTEM
	//	Create a well-known SID for the SYSTEM
	bAllocateSucc &= AllocateAndInitializeSid
		(
			&SIDAuthNT,			// This structure provides the top-level identifier authority value to set in the SID 基础 SID 结构体地址
			1,				// 要初始化下面几个“子权利项”
			SECURITY_LOCAL_SYSTEM_RID,	// 这个就是上面指定的要初始化的“子权利项”，其下的 7 个将被忽略
			0, 0, 0, 0, 0, 0, 0,		// 这里是剩下的 7 个“子权利项”
			&pSystemSID
		);
	if ( bAllocateSucc )
	{
		//	Initialize an EXPLICIT_ACCESS structure for an ACE.
		//	The ACE will allow SYSTEM read access to the key.
		pea[0].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// GENERIC_ALL;	// 给 SYSTEM 读写权限
		pea[0].grfAccessMode		= SET_ACCESS;
		pea[0].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
		pea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		pea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		pea[0].Trustee.ptstrName	= (LPTSTR)pSystemSID;


		////////////////////////////////////////////////////////////
		//	4.2 - Administrators
		//	Create a SID for the BUILTIN\Administrators group.
		bAllocateSucc &= AllocateAndInitializeSid
			(
				&SIDAuthNT,			// 基础 SID 结构体地址
				2,				// 要初始化下面几个“子权利项”
				SECURITY_BUILTIN_DOMAIN_RID,	// 上面指定的要初始化的第 1 个“子权利项”
				DOMAIN_ALIAS_RID_ADMINS,	// 上面指定的要初始化的第 2 个“子权利项”
				0, 0, 0, 0, 0, 0,		// 这里是剩下的 6 个“子权利项”
				&pAdminSID
			);
		if ( bAllocateSucc )
		{
			//	Initialize an EXPLICIT_ACCESS structure for an ACE.
			//	The ACE will allow the Administrators group full access to the key.
			pea[1].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// dwAccessPms
			pea[1].grfAccessMode		= SET_ACCESS;
			pea[1].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
			pea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
			pea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
			pea[1].Trustee.ptstrName	= (LPTSTR)pAdminSID;


			////////////////////////////////////////////////////////////
			//	4.3 - IIS_WPG
			//	"S-1-5-x-1000"
			//	"S-1-5-21-3338309497-422766316-1218862070-1018"
			//	ConvertStringSidToSid( "S-1-5-32-1000", &pIisWpgSID )
			if ( SUCCEEDED( GetSIDFromName( "IIS_WPG", &pIisWpgSID ) ) )
			{
				//	Initialize an EXPLICIT_ACCESS structure for an ACE.
				//	The ACE will allow the Administrators group full access to the key.
				pea[2].grfAccessPermissions	= SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;	// dwAccessPms
				pea[2].grfAccessMode		= SET_ACCESS;
				pea[2].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
				pea[2].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
				pea[2].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
				pea[2].Trustee.ptstrName	= (LPTSTR)pIisWpgSID;
				//FreeSid( pIisWpgSID );

				////////////////////////////////////////////////////////////
				//	4.3	Everyone
				//
				bAllocateSucc &= AllocateAndInitializeSid
				(
					&SIDAuthWorld,		// 基础 SID 结构体地址
					1,			// 要初始化下面几个“子权利项”
					SECURITY_WORLD_RID,	// 这个就是上面指定的要初始化的“子权利项”，其下的 7 个将被忽略
					0, 0, 0, 0, 0, 0, 0,	// 这里是剩下的 7 个“子权利项”
					&pEveryoneSID
				);
				if ( bAllocateSucc )
				{
					pea[3].grfAccessPermissions	= GENERIC_READ|GENERIC_EXECUTE;	// 给 Everyone 读写权限
					pea[3].grfAccessMode		= SET_ACCESS;
					pea[3].grfInheritance		= SUB_CONTAINERS_AND_OBJECTS_INHERIT;	// <*** 标记 ACE
					pea[3].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
					pea[3].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
					pea[3].Trustee.ptstrName	= (LPTSTR)pEveryoneSID;
				}
			}
		}
		
		// ..
		//FreeSid( pSystemSID );
	}
	
	// ..
	return bAllocateSucc;
}



//////////////////////////////////////////////////////////////////////////
// 获取指定用户名的 SID
HRESULT CProcSecurityDesc::GetSIDFromName( LPCSTR pwszUserName, PSID * ppSid )
{
	// Translate the user name into a SID
	if( NULL == ppSid )
	{
		return( E_INVALIDARG );
	}
	
	HRESULT hr		= S_OK;
	DWORD dwRetVal		= 0;
	SID_NAME_USE SidNameUse;
	DWORD cbSid		= 0;
	DWORD cbDomainName	= 0;
	LPSTR wstrDomainName	= NULL;
	BOOL fRetVal		= FALSE;
	
	do
	{
		if ( ! LookupAccountName( NULL, pwszUserName, NULL, &cbSid, NULL, &cbDomainName, &SidNameUse ) )
		{
			dwRetVal = GetLastError();
			if ( ERROR_INSUFFICIENT_BUFFER != dwRetVal )
			{
				hr = HRESULT_FROM_WIN32( dwRetVal );
				break;
			}
		}
		
		*ppSid = (SID *)LocalAlloc( LPTR, cbSid );
		if ( NULL == *ppSid )
		{
			hr = E_OUTOFMEMORY;
			break;
		}
		
		wstrDomainName = new CHAR[ cbDomainName ];
		if ( NULL == wstrDomainName )
		{
			hr = E_OUTOFMEMORY;
			break;
		}
		
		fRetVal = LookupAccountName( NULL, pwszUserName, *ppSid, &cbSid, wstrDomainName, &cbDomainName, &SidNameUse );
		if ( ! fRetVal )
		{
			dwRetVal = GetLastError();
			hr = HRESULT_FROM_WIN32( dwRetVal );
			break;
		}
	}
	while( FALSE );
	
	if ( NULL != wstrDomainName )
	{
		delete [] wstrDomainName;
	}
	
	if ( FAILED( hr ) && ( NULL != *ppSid ) )
	{
		LocalFree( *ppSid );
		*ppSid = NULL;
	}
	
	return ( hr );
}