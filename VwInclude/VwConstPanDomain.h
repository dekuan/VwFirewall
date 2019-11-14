#ifndef __VWCONSTPANDOMAIN_HEADER__
#define __VWCONSTPANDOMAIN_HEADER__




/**
 *	常量定义
 */
#define INI_DOMAIN_MAIN			"Main"
#define INI_DOMAIN_SYSDOMAINS		"SysDomains"
#define INI_DOMAIN_USRDOMAINS		"UsrDomains"

#define INI_KEY_MAIN_START		"Start"

#define CONST_DOMAINTYPE_PAN		1
#define CONST_DOMAINTYPE_NORMAL		0


/**
 *	结构体定义
 */
typedef struct tagPanDomain
{
	LONG  lnIdx;
	DWORD dwDmType;		//	CONST_DOMAINTYPE_PAN / CONST_DOMAINTYPE_NORMAL
	TCHAR szSrvAddr[ 32];
	DWORD dwSrvAddrValue;
	TCHAR szSrvPort[ 32];
	DWORD dwSrvPort;
	TCHAR szSrvHost[ 64 ];
	TCHAR szSrvPath[ MAX_PATH ];
	DWORD dwSrvPathLen;

}STPANDOMAIN, *PSTPANDOMAIN;



#endif	// __VWCONSTPANDOMAIN_HEADER__