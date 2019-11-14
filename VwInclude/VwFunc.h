// VwFunc.h: interface for the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWFUNC_HEADER__
#define __VWFUNC_HEADER__

#pragma warning(disable : 4018)
#pragma warning(disable : 4530)

#include <winsvc.h>
#pragma comment( lib, "Version.lib" )

#include "VwConst.h"
#include "md5.h"

//#include <nb30.h>
//#pragma comment( lib, "netapi32.lib" )

//	func base
#include "VwConstAntileechs.h"
#include "VwFuncBase.h"
#include "VwFuncNet.h"
#include "VwFuncServices.h"
//#include "VwFuncIni.h"


//	创建 cfg map
typedef BOOL (WINAPI * PFNMAKECFGMAP)();

//	生成 CfgMap 文件	
BOOL _vwfunc_make_cfg_map_file_byrundll( LPCTSTR lpcszVwDllFile );
BOOL _vwfunc_make_cfg_map_file_bystdcall( LPCTSTR lpcszVwDllFile );

//	判断 VwCfgNew 是否正在运行
BOOL _vwfunc_is_vwcfgnew_running();

//BOOL _vwfunc_close_vwcfgnew_window();


#endif // __VWFUNC_HEADER__
