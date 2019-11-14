// CyclePool.h: interface for the CCyclePool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CYCLEPOOL_HEADER__
#define __CYCLEPOOL_HEADER__

#include "VwStruct.h"
#include "DeThreadSleep.h"

/**
 *	const define
 */
const UINT  CONST_MAX_CYCLEPOOL_DEPTH	= 2000;	//	池最大上限，尽量选择素数
							//	2344 * 3000  = 6.7 MB
							//	2344 * 3989 = 8.9 MB

static const CHAR * CONST_FILEMAP_VW_CYCLEPOOL_HEADER	= "FILEMAP_VW_CYCLEPOOL_HEADER";
static const CHAR * CONST_FILEMAP_VW_CYCLEPOOL		= "FILEMAP_VW_CYCLEPOOL";


/**
 *	CyclePool Header 结构体
 */
typedef struct tagCyclePoolHeader
{
	tagCyclePoolHeader()
	{
		memset( this, 0, sizeof(tagCyclePoolHeader) );
	}

	HANDLE hFileMapHeader;	//	头共享内存的文件句柄
	HANDLE hFileMap;	//	数据共享内存的文件句柄
	LONG lnRefCount;		//	使用进程引用计数
	UINT uFreePos;		//	FreePos index of pool
	CRITICAL_SECTION oCriticalSectionModify;

}STCYCLEPOOLHEADER, *PSTCYCLEPOOLHEADER;

/**
 *	CyclePool Data 结构体
 *	[结构体 STCYCLEPOOL 说明]
 *	1，必须有一个成员用来记录当前自己的索引
 *	2，必须有一个成员用来记录当前自己的类型：在循环池中还是迫不得已临时 new 出来的
 */
typedef STMYHTTPFILTERCONTEXT	STCYCLEPOOL;

/**
 *	最近访问者，该结构体数组默认是 50 个长度
 */
typedef struct tagVisitRecently
{
	tagVisitRecently()
	{
		memset( this, 0, sizeof(tagVisitRecently) );
	}

	LONG		lnTickCount;
	SYSTEMTIME	stStart;			//	来访时间
	TCHAR		szAgent[ 128 ];			//	Agent
	TCHAR		szUrl[ MAX_PATH ];		//	Url
	TCHAR		szRemoteAddr[ 16 ];		//	来访者 IP 地址
	TCHAR		szReferer[ MAX_PATH ];		//	从那里点击过来的

}STVISITRECENTLY, *PSTVISITRECENTLY;



/**
 *	class of CCyclePool
 */
class CCyclePool
{
public:
	CCyclePool( UINT lnPoolDepth = CONST_MAX_CYCLEPOOL_DEPTH );
	virtual ~CCyclePool();

	//	Get IpList pool depth
	UINT GetPoolDepth();

	//	是否初始化成功
	BOOL IsInitSucc();

	//	获取一个节点
	BOOL GetNode( STCYCLEPOOL ** ppstNode );

	//	释放一个节点
	VOID FreeNode( STCYCLEPOOL * pstCyclePool );

	//	后台工作线程
	static unsigned __stdcall _threadBgWorker( PVOID arglist );
	VOID BgWorkerProc();

	//	获取最近 100 个访问者列表的 JSON 缓冲区大概大小
	DWORD GetVisitRecentlyJsonStringLength();

	//	获取最近 100 个访问者列表
	BOOL GetVisitRecentlyJsonString( LPTSTR lpszVRListJson, DWORD dwSize );


private:
	VOID InitPoolNode( STCYCLEPOOL * pstPoolNode, LONG lnFlag = CONST_CYCLEPOOLFLAG_FREE );

public:
	//	..
	STCYCLEPOOLHEADER * m_pstPoolListHeader;
	STCYCLEPOOL * m_pstPoolList;

	//	最近来访，由于暂时不使用，所以修改成 3
	STVISITRECENTLY	m_stVisitRecently[ 1 ];

	//	处理线程等待
	CDeThreadSleep	m_cThSleepBgWorker;

private:

	//	循环池的长度
	UINT	m_uPoolDepth;

	//	标识是否初始化成功
	BOOL	m_bInitSucc;
	
	//	红绿灯
	//CRITICAL_SECTION m_CriticalSectionModify;
	//HANDLE  m_hFileMap;

	//	指向当前节点
	//UINT	m_uCurrPos;

	//	指向一个 FREE 节点
	//UINT	m_uFreePos;

	//	最近来访 池的当前指针
	UINT	m_uVisitRecentlyCurrPos;

	//	最近来访 池的长度
	UINT	m_uVisitRecentlyPoolDepth;

};





#endif // __CYCLEPOOL_HEADER__
