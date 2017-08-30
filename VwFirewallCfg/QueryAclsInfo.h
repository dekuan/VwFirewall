// QueryAclsInfo.h: interface for the CQueryAclsInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __QUERYACLSINFO_HEADER__
#define __QUERYACLSINFO_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



typedef struct tagQueryAclsInfoAceInfo
{
	TCHAR szUserName[ 64 ];
	TCHAR szDomain[ 64 ];
	ACCESS_MASK Mask;	// Permissions Mask
	
}STQUERYACLSINFOACEINFO, *LPSTQUERYACLSINFOACEINFO;



/**
 *	class of ...
 */
class CQueryAclsInfo
{
public:
	CQueryAclsInfo();
	virtual ~CQueryAclsInfo();

public:
	BOOL GetSpecUserAclsInfo( LPCTSTR lpcszFilename, LPCTSTR lpcszUser, BYTE btAceType, ACCESS_MASK * pMask );
	VOID CheckPermissions( ACCESS_MASK Mask );
	BOOL IsDenyPermissionsExist( ACCESS_MASK Mask );
	BOOL IsAllowPermissionsExist( ACCESS_MASK Mask );

private:
	BOOL MyPrivilege( LPCTSTR lpszSeName, BOOL fEnable );
	BOOL DumpSDInfo( LPCTSTR lpcszUser, BYTE btAceType, PSECURITY_DESCRIPTOR pstSD, LPSTQUERYACLSINFOACEINFO pstAceInfo );
	BOOL DumpAceInfo( ACCESS_ALLOWED_ACE * pAce, LPSTQUERYACLSINFOACEINFO pstAceInfo );

};


#endif // __QUERYACLSINFO_HEADER__
