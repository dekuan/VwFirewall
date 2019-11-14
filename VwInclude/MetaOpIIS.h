// MetaOpIIS.h: interface for the CAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __METAOPIIS_HEADER__
#define __METAOPIIS_HEADER__

//	记得这个必须放在最前面，否则编译会出错
#include <objbase.h>
#include <initguid.h>

//	加入对ATL的支持
#include <atlbase.h>

#include <winsock2.h>
#pragma comment(lib, "ws2_32")

#include "Crc32.h"
#include "MetaOpIISHeader.h"








/**
 *	用于操作IIS，增加、删除、修改站点属性
 */
class CMetaOpIIS
{
public:
	CMetaOpIIS();
	CMetaOpIIS( STMETASITEINFO * pSiteinfo );
	CMetaOpIIS( STMETAVIRTUALDIR * pVDir );

	CMetaOpIIS & operator = ( STMETASITEINFO & pSiteinfo );
	CMetaOpIIS & operator = ( STMETAVIRTUALDIR & pVDir );

	virtual ~CMetaOpIIS(void);

private:	
//	STMETASITEINFO	m_stSiteInfo;
	CCrc32		m_cCrc32;

	//PWCHAR m_wszBuf;			//
	DWORD  m_dwLastSysErrorCode;
	DWORD  m_dwLastOperErrorCode;
	TCHAR  m_szLastErrorInfo[ MAX_PATH ];
	PWCHAR m_wszBuf;

	STMETAVIRTUALDIR	m_VDir;		//	虚拟目录
	CComPtr <IMSAdminBase>	m_pIMeta;	//	指向MSAdminBase接口的灵敏指针
	CComPtr <IWamAdmin2>	m_pIAppAdmin;	//	指向IWamAdmin接口地灵敏指针

public:
	BOOL initCom();				//	初始化COM，打开METABASE
	VOID uninitCom();			//	销毁COM，关闭 METABASE
	BOOL isInitComReady();

	//
	//	站点操作
	//
	BOOL createSite( STMETASITEINFO * pstSiteInfo );	//	建立一个站点，返回KEYNAME
	BOOL deleteSite( STMETASITEINFO * pstSiteInfo );	//	删除一个站点
	BOOL modifySite( STMETASITEINFO * pstSiteInfo );	//	修改一个站点属性

	BOOL getAllDataFromMetabase( STMETAALLDATA * pstAllData );					//	获取所有站点信息，包括进程池的信息
	BOOL getSiteDataFromKey( LPCWSTR lpcwszKeyName, STMETASITEINFO * pstSiteInfo );			//	获取单个站点的信息
	BOOL getAppPoolDataFromKey( LPCWSTR lpcwszKeyName, STMETAAPPPOOLINFO * pstAppPoolInfo );	//	获取某个 AppPool 的信息

	//
	//	虚拟目录的操作
	//
	BOOL AddVDir();
	BOOL DeleteVDir();
	BOOL ModifyVDir();

	BOOL HttpRedirect();				//	虚拟目录HTTP重定位
//	INT  GetNewKeyName( char * szNewKeyName );	//	获取新建立的站点主键
	BOOL ReadAllData();				//	读取所有站点数据

	//	设置最后错误信息
	VOID setLastErrorInfo( DWORD dwOperErrorCode, LPCTSTR lpszFmt, ... );

private:

	DWORD getNewSiteKey();

	//	获取一个站点下的某个纪录的值
	HRESULT getRecordData( METADATA_HANDLE hKeyName, METADATA_RECORD * pstMRec, 
			DWORD dwMDIdentifier, DWORD dwMDAttributes, DWORD dwMDUserType, DWORD dwMDDataType, LPCWSTR lpcwszBuf, DWORD dwBufLen,
			DWORD * pdwMDRequiredDataLen );

	//	修改一个站点下的某个纪录的值
	BOOL modifyRecordData( STMETAOPERRECORD * pstRec, DWORD dwMDIdentifier, DWORD dwMDAttributes, DWORD dwMDUserType, DWORD dwMDDataType, LPCWSTR lpcwszBuf, DWORD dwBufLen );

	BOOL mb_createApp( STMETASITEINFO * pstSiteInfo, LPWSTR lpszMDPath, DWORD dwAppMode );		//	为一个站点建立APP
	BOOL mb_createKey( STMETAOPERRECORD * pstRec );				//	建立一个子键
	BOOL mb_renameKey( STMETAOPERRECORD * pstRec );				//	修改一个键名
	BOOL mb_deleteKey( STMETAOPERRECORD * pstRec );				//	删除一个子键
	BOOL mb_getKeyData( STMETAOPERRECORD * pstRec );			//	获取数据值

	BOOL ModifyData( STMETAOPERRECORD * pstRec );				//	修改数据，即一个键的属性
	BOOL isValidSBindings( LPCTSTR lpcszBindings );				//	检查绑定信息是否有效
	BOOL isValidSingleSBindings( string strSBindings );			//	检查单个绑定信息的有效性
	INT  formatSBindingsToMetabase( LPWSTR pwszSBindings );			//	将字符串格式(以 | 分割) 处理成 Metabase 格式(以 NULL 分割)
	VOID formatSBindingsToString( LPSTR lpszBuffer, DWORD dwSize );		//	将 Metabase 的 Binding(以 NULL 分割)处理成数据库格式(以 | 分割)

	BOOL DelHttpRedirect( STMETAOPERRECORD &operRecord );		//	某站点是否被重定向


	//bool GetBindingData( STMETAOPERRECORD &operRecord, WCHAR *pwszBuf, DWORD &dwBufLen );	//	获取绑定数据
};




#endif	// __METAOPIIS_HEADER__