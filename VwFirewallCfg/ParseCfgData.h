// ParseCfgData.h: interface for the CParseCfgData class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PARSECFGDATA_HEADER__
#define __PARSECFGDATA_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwFirewallConfigFile.h"
#include <vector>
using namespace std;


typedef struct tagParseCfgDataHeader
{
	TCHAR szVersion[ 16 ];	//	version
	TCHAR szDate[ 32 ];	//	release date

}STPARSECFGDATAHEADER, *LPSTPARSECFGDATAHEADER;

typedef struct tagParseCfgDataSet
{
	TCHAR szType[ 16 ];
	TCHAR szUser[ 32 ];
	TCHAR szChk[ 16 ];
	TCHAR szCmd[ 512 ];
	TCHAR szPath[ MAX_PATH ];
	TCHAR szParam[ MAX_PATH ];

	HKEY  hkRoot;			//	convert from szRegRoot
	TCHAR szRegRoot[ 32 ];
	TCHAR szRegPath[ MAX_PATH ];
	TCHAR szRegVName[ 64 ];
	TCHAR szRegVType[ 16 ];
	LONG  lnRegVType;
	TCHAR szRegV[ MAX_PATH ];

	TCHAR szRemark[ 128 ];

}STPARSECFGDATASET, *LPSTPARSECFGDATASET;

typedef struct tagParseCfgDataGroup
{
	INT   nIndex;			//	±àºÅ
	TCHAR szGroup[ 64 ];		//	e.g.: "sethc.exe"
	TCHAR szGroupSet[ 64 ];		//	e.g.: "sethc.exe.set"
	TCHAR szGroupName[ 128 ];	//	e.g.: "ÃâÒß¡°Õ³ÖÃ¼ü¡±Â©¶´"

	vector<STPARSECFGDATASET> vcAclsSetList;
	vector<STPARSECFGDATASET>::iterator it;

}STPARSECFGDATAGROUP, *LPSTPARSECFGDATAGROUP;


/**
 *	class of CParseCfgData
 */
class CParseCfgData :
	public CVwFirewallConfigFile
{
public:
	CParseCfgData();
	virtual ~CParseCfgData();

public:
	BOOL LoadData( LPCTSTR lpcszFilename );

private:
	BOOL  LoadAclsSetList( LPCTSTR lpcszFilename, LPCTSTR lpcszGroupSet, vector<STPARSECFGDATASET> & vcAclsSetList );
	BOOL  GetHKeyRootFromString( LPCTSTR lpcszRootString, HKEY * phkRoot );
	LONG  GetRegDataTypeFromString( LPCTSTR lpcszDataTypeString );

public:
	vector<STPARSECFGDATAGROUP> m_vcAclsGroupList;
	vector<STPARSECFGDATAGROUP>::iterator m_it;


};




#endif // __PARSECFGDATA_HEADER__
