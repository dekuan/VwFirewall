// EnumProc.h: interface for the CVwEnumProc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ENUMPROC_HEADER__
#define __ENUMPROC_HEADER__

#pragma warning(disable : 4291)

#include "VwFunc.h"


/**
 *	define
 */
#define CONST_SPLIT_FLAG			_T("|^_^|")

//	排序功能号
#define CONST_SORT_BY_PROCESS_NAME		1
#define CONST_SORT_BY_PROCESS_SIZE		2
#define CONST_SORT_BY_PROCESS_COMPANY		3


//////////////////////////////////////////////////////////////////////////
//
template <class T>
class CSimpleArray
{
public:
	T* m_aT;
	int m_nSize;
	int m_nAllocSize;

// Construction/destruction
	CSimpleArray() : m_aT(NULL), m_nSize(0), m_nAllocSize(0)
	{ }

	~CSimpleArray()
	{
		RemoveAll();
	}

// Operations
	int GetSize() const
	{
		return m_nSize;
	}
	BOOL Add(T& t)
	{
		if(m_nSize == m_nAllocSize)
		{
			T* aT;
			int nNewAllocSize = (m_nAllocSize == 0) ? 1 : (m_nSize * 2);
			aT = (T*)realloc(m_aT, nNewAllocSize * sizeof(T));
			if(aT == NULL)
				return FALSE;
			m_nAllocSize = nNewAllocSize;
			m_aT = aT;
		}
		m_nSize++;
		SetAtIndex(m_nSize - 1, t);
		return TRUE;
	}
	BOOL Remove(T& t)
	{
		int nIndex = Find(t);
		if(nIndex == -1)
			return FALSE;
		return RemoveAt(nIndex);
	}
	BOOL RemoveAt(int nIndex)
	{
		if(nIndex != (m_nSize - 1))
		{
			m_aT[nIndex].~T();
			memmove((void*)&m_aT[nIndex], (void*)&m_aT[nIndex + 1], (m_nSize - (nIndex + 1)) * sizeof(T));
		}
		m_nSize--;
		return TRUE;
	}
	void RemoveAll()
	{
		if(m_aT != NULL)
		{
			for(int i = 0; i < m_nSize; i++)
				m_aT[i].~T();
			free(m_aT);
			m_aT = NULL;
		}
		m_nSize = 0;
		m_nAllocSize = 0;
	}
	T& operator[] (int nIndex) const
	{
//		assert(nIndex >= 0 && nIndex < m_nSize);
		return m_aT[nIndex];
	}
	T* GetData() const
	{
		return m_aT;
	}

// Implementation
	class Wrapper
	{
	public:
		Wrapper(T& _t) : t(_t)
		{
		}
		template <class _Ty>
		void *operator new(size_t, _Ty* p)
		{
			return p;
		}
		T t;
	};
	void SetAtIndex(int nIndex, T& t)
	{
//		ATLASSERT(nIndex >= 0 && nIndex < m_nSize);
		new(&m_aT[nIndex]) Wrapper(t);
	}
	int Find(T& t) const
	{
		for(int i = 0; i < m_nSize; i++)
		{
			if(m_aT[i] == t)
				return i;
		}
		return -1;  // not found
	}
};






//////////////////////////////////////////////////////////////////////////
//
#include "VwCompressedPath.h"

typedef struct tagProcessInfo
{
	tagProcessInfo()
	{
		memset( this, 0, sizeof(tagProcessInfo) );
	}
	DWORD dwProcID;
	TCHAR szWindowTitle[ MAX_PATH ];
	TCHAR szProcName[ MAX_PATH ];
	TCHAR szProcPath[ MAX_PATH ];
	TCHAR szCompressPath[ MAX_PATH ];
	TCHAR szFileMd5[ 64 ];
	DWORD dwMemoryUsed;
	DWORD dwFileSize;
	TCHAR szCompany[ MAX_PATH ];
	TCHAR szVersion[ MAX_PATH ];
	
} STPROCESSINFO;

typedef struct tagSpecModule
{
	tagSpecModule()
	{
		memset( this, 0, sizeof(tagSpecModule) );
	}
	TCHAR szFilePath[ MAX_PATH ];
	
}STSPECMODULE, * PSTSPECMODULE;



/**
 *	class of CVwEnumProc
 */
class CVwEnumProc : 
	public CCompressedPath
{
public:
	CVwEnumProc();
	virtual ~CVwEnumProc();

public:
	BOOL EnumProcess( TCHAR * pszError );
	BOOL FreeAllLinks();
	BOOL GetLinkSize();
	BOOL GetInfoByIndex( DWORD dwIndex, STPROCESSINFO & stProcInfo );
	BOOL GetStringInfoByIndex( int dwIndex_in, TCHAR * pszInfo_out, DWORD dwSize, TCHAR * pszError );
	BOOL SortLink( DWORD dwSortType );
	BOOL GetColSpecModules( LPTSTR pszFileList, DWORD dwSize );
	BOOL SetCollectProcessName( LPTSTR pszProcessName );

private:
	BOOL UpPrivilege( TCHAR * pszPrivilege, BOOL bEnable );
	BOOL CorrectSpecSysPath( TCHAR * pszPathSpec, TCHAR * pszPathOut );
	BOOL CollectSpecModule( LPTSTR pszFilePath );
	BOOL AddInfoToLinks( STPROCESSINFO * stInfo, TCHAR * pszError );

public:
	// 存放数据的数组
	CSimpleArray<STPROCESSINFO> m_aProcessInfo;
	CSimpleArray<STSPECMODULE> m_aSpecModule;

private:
	TCHAR m_szSpecProcessName[ MAX_PATH ];
};


#endif // __ENUMPROC_HEADER__




















