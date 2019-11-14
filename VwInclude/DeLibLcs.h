#ifndef __DELIBLCS_HEADER__
#define __DELIBLCS_HEADER__



#ifndef __DELIBLCS_STRUCT__
#define __DELIBLCS_STRUCT__

/**
 *	结构体定义
 */
typedef struct tagDeLibLcs
{
	tagDeLibLcs()
	{
		memset( this, 0, sizeof(tagDeLibLcs) );
	}
	HWND  hParentDlg;
	BOOL  bReged;			//	是否正式版本
	BOOL  bCreateGeneWithDiskSN;	//	是否使用硬盘序列号作为生成 gene 的参数

	TCHAR szSoftName[ MAX_PATH ];	//	产品名称，例如："deremote"
	TCHAR szPrKey[ MAX_PATH ];	//	生成 gene 的钥匙
	TCHAR szHttpKey[ MAX_PATH ];	//	http 通讯钥匙
	
	TCHAR szWorkDir[ MAX_PATH ];	//	工作目录

//	TCHAR szGene[ 128 ];		//	32 位的特征码
//	TCHAR szRegCode[ 128 ];		//	注册码
//	TCHAR szActCode[ 128 ];		//	激活码
	
	TCHAR szBuyUrl[ MAX_PATH ];	//	直接购买的链接
	
}STDELIBLCS, *LPSTDELIBLCS;

#endif	//	__DELIBLCS_STRUCT__





/**
 *	导出函数定义
 */
HINSTANCE __stdcall delib_lcs_get_instance();
BOOL __stdcall delib_lcs_showdlg( STDELIBLCS * pstLcInfo );








#endif	//	__DELIBLCS_HEADER__