//	HashPool.h
//	LiuQiXing @ 2006-08-19
//
//////////////////////////////////////////////////////////////////////


#ifndef __HASHPOOL_HEADER__
#define __HASHPOOL_HEADER__


//#include <stdio.h>
//#include <stdlib.h>
#include "crc32.h"


/**
 *	const define
 */
const UINT  CONST_MAX_HASHPOOL_DEPTH	= 299993;		//	池最大上限，尽量选择素数
								//	999983 * 16 = 15.2 M
								//	299993 * 16 = 4.5 M

const UINT  CONST_FNV_32_PRIME		= 0x01000193UL;		//	哈希的种子值
const UINT  CONST_IPADDR_NULL		= 0;

static const CHAR * CONST_FILEMAP_VW_HASHPOOL_HEADER	= "FILEMAP_VW_HASHPOOL_HEADER";
static const CHAR * CONST_FILEMAP_VW_HASHPOOL		= "FILEMAP_VW_HASHPOOL";


//	error define
//const DWORD CONST_ERROR_PARAM_HASHPOOL_DEPTH	= 0x1000;


/**
 *	HashPoolList Header 结构体
 */
typedef struct tagHashPoolListHeader
{
	tagHashPoolListHeader()
	{
		memset( this, 0, sizeof(tagHashPoolListHeader) );
	}

	HANDLE hFileMapHeader;	//	头共享内存的文件句柄
	HANDLE hFileMap;	//	数据共享内存的文件句柄
	UINT uRefCount;		//	使用进程引用计数
	CRITICAL_SECTION oCriticalSectionModify;

}STHASHPOOLLISTHEADER, *PSTHASHPOOLLISTHEADER;


/**
 *	Pool List data struct
 */
typedef struct tagHashPoolList
{
	tagHashPoolList()
	{
		dwQueryTick	= 0;
		dwIpAddr	= CONST_IPADDR_NULL;
		usThreadCount	= 0;
		usVisitCount	= 0;
		dwWakeUpTime	= 0;
	}

	DWORD   dwQueryTick;		//	每次“检索某个 IP 上的线程计数”或“增加某个 IP 上的线程计数”都要记录下 TickCount，并根据此来限制超时
	DWORD	dwIpAddr;		//	Ip Address
	USHORT	usThreadCount;		//	下载计数
	USHORT	usVisitCount;		//	1 分钟内的访问次数
	DWORD	dwWakeUpTime;		//	如果被拒绝，这里保存让某个客户端醒来的时间点
	USHORT  usLmtDayIpLastDate;	//	限制日 IP 多少：最后记录时间。格式：月日，例如：1015 / 0101
	USHORT  usLmtDayIpTimes;	//	限制日 IP 多少：数值 0 ~ 65535

}STHASHPOOLLIST, *PSTHASHPOOLLIST;





/**
 *	Hash Pool Class
 */
class CHashPool
{
public:
	//	Construction function
	CHashPool( unsigned int lnPoolDepth = CONST_MAX_HASHPOOL_DEPTH*1 );

	//	..
	virtual ~CHashPool();

public:

	BOOL MallocShareMemory( DWORD dwSize, LPCTSTR lpcszMemName, LPVOID lpvBuffer, LPBOOL lpbCreate );

	//	Get IpList pool depth
	UINT  GetPoolDepth();

	//	是否初始化成功
	BOOL  IsInitSucc();


	//
	//	根据内容获取 Hash Index
	//
	LONG  GetHashPoolIndex( const DWORD dwIpAddr );

	//
	//	辅助函数，提供字符串 IP 地址到 DWORD 的转换
	//
	DWORD GetIpAddrFromString( LPCSTR lpcszIpAddr );


	////////////////////////////////////////////////////////////
	//	下载线程限制
	//

	//	获取计数
	LONG  GetCount( LONG lnIndex );

	//	保存数据并增加计数
	BOOL  IncCount( LONG lnIndex, const DWORD dwIpAddr );

	//	减少计数
	BOOL  DecCount( LONG lnIndex, const DWORD dwIpAddr );


	////////////////////////////////////////////////////////////
	//	日保护文件访问次数限制
	//

	//	获取计数
	LONG  LmtDayIpGetCount( LONG lnIndex );

	//	保存数据并增加计数
	BOOL  LmtDayIpIncCount( LONG lnIndex, const DWORD dwIpAddr );


	////////////////////////////////////////////////////////////
	//	防攻击
	//

	//	获取计数
	LONG  AntiAttackGetCount( LONG lnIndex );

	//	增加计数
	BOOL  AntiAttackIncCount( LONG lnIndex, const DWORD dwIpAddr );

	//	获取被限制客户端醒来的时间点
	LONG  AntiAttackGetWakeUpTime( LONG lnIndex );

	//	设置被限制客户端醒来的时间点
	VOID  AntiAttackSetWakeUpTime( LONG lnIndex, LONG lnWakeUpTime );

	//	防攻击后台工作线程
	static VOID _threadAntiAttackBgWorker( PVOID pvArg );
	VOID AntiAttackBgWorkerProc();


private:

	//	初始化节点
	VOID InitPoolNode( STHASHPOOLLIST * pstPoolNode );

	//
	//	检测 Index 是否在一级缓存中
	//
	BOOL  IsInIndexRange( UINT uIndex );


	//	32 version for zero terminated string
	UINT  GetStringHashValue32( const char * s, UINT lnInit = CONST_FNV_32_PRIME );

	//	32 version for buffer
	UINT  GetStringHashValue32ByBuffer( const char * buf, UINT len, UINT lnInit = CONST_FNV_32_PRIME );



	//	the depth of iplist pool, you'd better set it with a "prime number".
	UINT  m_uPoolDepth;

	//	标识是否初始化成功
	BOOL  m_bInitSucc;

	//	超时
	DWORD m_dwThreadRecordTimeout;

	//	IpList data
	STHASHPOOLLISTHEADER * m_pstPoolListHeader;
	STHASHPOOLLIST * m_pstPoolList;

	//	..
	//CRITICAL_SECTION m_CriticalSectionModify;
	//HANDLE  m_hFileMap;

};



#endif	// __HASHPOOL_HEADER__





















