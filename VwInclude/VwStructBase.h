#ifndef __VWSTRUCTBASE_HEADER__
#define __VWSTRUCTBASE_HEADER__



/**
 *	Core Info 核心模块信息
 */
typedef struct tagVwCoreInfo
{
	//	R	- 表示非核心模块以外的进程对这个对象的操作权限
	//	W	- 标识可写
	tagVwCoreInfo()
	{
		memset( this, 0, sizeof(tagVwCoreInfo) );
	}

	BOOL  bEnterpriseVer;	//	[R]   是否是正式版本
	BOOL  bCfgChanged;	//	[R/W] 配置文件是否改变
	BOOL  bContinueWork;	//	[R]   核心模块是否正在工作
	DWORD dwCoreTick;	//	[R]   核心模块的 TickCount
	DWORD dwLoadCfgTick;	//	[R/W] 装载配置信息的 TickCount

}STVWCOREINFO, *PSTVWCOREINFO;





#endif	// __VWSTRUCTBASE_HEADER__