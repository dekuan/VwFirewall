/*******************************************

	MFTrafficButton

	Version:	1.0
	Date:		31.10.2001
	Author:		Michael Fatzi
	Mail:		Michael_Fatzi@hotmail.com
	Copyright 1996-1997, Keith Rule

	You may freely use or modify this code provided this
	Copyright is included in all derived versions.
	
	History: 10.2001 Startup

	Handy little button control to display current 
	nettraffic as graph in a button.

********************************************/

// CDeNetTraffic.cpp: implementation of the CDeNetTraffic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeNetTraffic.h"
#include "float.h"
#include "winperf.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeNetTraffic::CDeNetTraffic()
{
	m_dbLasttraffic		= 0.0;
	m_nCurrentInterface	= -1;
	m_nCurrentTrafficType	= ENUM_TRAFFIC_TOTAL;
	GetInterfaces();
}

CDeNetTraffic::~CDeNetTraffic()
{
	
}


//	Little helper functions
//	Found them on CodeGuru, but do not know who has written them originally

PERF_OBJECT_TYPE *FirstObject(PERF_DATA_BLOCK *dataBlock)
{
	return (PERF_OBJECT_TYPE *) ((BYTE *)dataBlock + dataBlock->HeaderLength);
}

PERF_OBJECT_TYPE *NextObject(PERF_OBJECT_TYPE *act)
{
	return (PERF_OBJECT_TYPE *) ((BYTE *)act + act->TotalByteLength);
}

PERF_COUNTER_DEFINITION *FirstCounter(PERF_OBJECT_TYPE *perfObject)
{
	return (PERF_COUNTER_DEFINITION *) ((BYTE *) perfObject + perfObject->HeaderLength);
}

PERF_COUNTER_DEFINITION *NextCounter(PERF_COUNTER_DEFINITION *perfCounter)
{
	return (PERF_COUNTER_DEFINITION *) ((BYTE *) perfCounter + perfCounter->ByteLength);
}

PERF_COUNTER_BLOCK *GetCounterBlock(PERF_INSTANCE_DEFINITION *pInstance)
{
	return (PERF_COUNTER_BLOCK *) ((BYTE *)pInstance + pInstance->ByteLength);
}

PERF_INSTANCE_DEFINITION *FirstInstance (PERF_OBJECT_TYPE *pObject)
{
	return (PERF_INSTANCE_DEFINITION *)  ((BYTE *) pObject + pObject->DefinitionLength);
}

PERF_INSTANCE_DEFINITION *NextInstance (PERF_INSTANCE_DEFINITION *pInstance)
{
	//	next instance is after
	//	this instance + this instances counter data

	PERF_COUNTER_BLOCK  *pCtrBlk = GetCounterBlock(pInstance);

	return (PERF_INSTANCE_DEFINITION *) ((BYTE *)pInstance + pInstance->ByteLength + pCtrBlk->ByteLength);
}

char *WideToMulti(wchar_t *source, char *dest, int size)
{
	WideCharToMultiByte(CP_ACP, 0, source, -1, dest, size, 0, 0);

	return dest;
}

/**
 *	returns the traffic of given interface
 */
double CDeNetTraffic::GetTraffic( INT interfaceNumber )
{
	double dbTraffic = GetTrafficProc( interfaceNumber );
	if ( dbTraffic > 1073741824 || dbTraffic < 0 )
	{
		//	1073741824 = 1024 * 1024 * 1024
		//	如果流量大于 1G，那么就人为是有问题的数据啦
		dbTraffic = 0;
	}
	return dbTraffic;
}
double CDeNetTraffic::GetTrafficProc( INT interfaceNumber )
{
	__try
	{
		map<INT,INTERFACE_NAME>::const_iterator it;
		map<DWORD,DWORD>::iterator itdw;
		INTERFACE_NAME stInterface;

		//
		//	查询是否存在
		//
		it = m_mapInterfaces.find( interfaceNumber );
		if ( it != m_mapInterfaces.end() )
		{
			stInterface = it->second;
		}
		else
		{
			return 0.0;
		}
		
		//	buffer for performance data
		unsigned char * data = new unsigned char[ DEFAULT_BUFFER_SIZE ];

		//	return value from RegQueryValueEx: ignored for this application
		DWORD type;
		
		//	Buffer size
		DWORD size = DEFAULT_BUFFER_SIZE;
		
		//	return value of RegQueryValueEx
		DWORD ret;

		//	request performance data from network object (index 510) 
		while ( ERROR_SUCCESS != ( ret = RegQueryValueEx( HKEY_PERFORMANCE_DATA, "510", 0, &type, data, &size ) ) )
		{
			if ( ERROR_MORE_DATA == ret )
			{
				//	buffer size was too small, increase allocation size
				size += DEFAULT_BUFFER_SIZE;

				delete [] data;
				data = NULL;

				data = new unsigned char[ size ];
				if ( NULL == data )
				{
					return 1;
				}
			}
			else 
			{
				// some unspecified error has occured
				return 1;
			}
		}


		PERF_DATA_BLOCK * dataBlockPtr = (PERF_DATA_BLOCK *)data;

		//	enumerate first object of list
		PERF_OBJECT_TYPE *objectPtr = FirstObject(dataBlockPtr);
		
		//	trespassing the list 
		for ( int a = 0 ; a < (int)dataBlockPtr->NumObjectTypes ; a++ ) 
		{
			char nameBuffer[255];

			//	did we receive a network object?
			if ( 510 == objectPtr->ObjectNameTitleIndex )
			{
				//	Calculate the offset
				DWORD processIdOffset = ULONG_MAX;
				
				//	find first counter 
				PERF_COUNTER_DEFINITION * counterPtr = FirstCounter(objectPtr);
				
				//	walking trough the list of objects
				for ( int b = 0 ; b < (int)objectPtr->NumCounters ; b++ ) 
				{
					//	Check if we received datatype wished
					if ( (int)counterPtr->CounterNameTitleIndex == m_nCurrentTrafficType )
					{
						processIdOffset = counterPtr->CounterOffset;
					}
					
					//	watch next counter
					counterPtr = NextCounter( counterPtr );
				}

				if ( ULONG_MAX == processIdOffset )
				{
					delete [] data;
					data = NULL;

					return 1;
				}

				//	Find first instance
				PERF_INSTANCE_DEFINITION * instancePtr = FirstInstance( objectPtr );

				DWORD fullTraffic;
				DWORD traffic;

				
				for ( b = 0 ; b < objectPtr->NumInstances; b++ )
				{
					//	evaluate pointer to name
					wchar_t *namePtr = (wchar_t *) ((BYTE *)instancePtr + instancePtr->NameOffset);

					//	get PERF_COUNTER_BLOCK of this instance
					PERF_COUNTER_BLOCK *counterBlockPtr = GetCounterBlock( instancePtr );

					//	now we have the interface name
					char * pName = WideToMulti( namePtr, nameBuffer, sizeof(nameBuffer) );
					//CString iName;
					//iName.Format( "%s", pName );
					TCHAR szIName[ MAX_PATH ];
					_sntprintf( szIName, sizeof(szIName)-sizeof(TCHAR), _T("%s"), pName );

					/*
					POSITION pos = m_lstTotalTraffics.FindIndex( b );
					if ( pos )
					{
						fullTraffic = *((DWORD *)( (BYTE*)counterBlockPtr + processIdOffset) );
						m_lstTotalTraffics.SetAt( pos, fullTraffic );
					}*/
					itdw = m_mapTotalTraffics.find( b );
					if ( itdw != m_mapTotalTraffics.end() )
					{
						fullTraffic = *((DWORD *)( (BYTE*)counterBlockPtr + processIdOffset) );
						itdw->second = fullTraffic;
					}


					//	If the interface the currently selected interface?
					if ( 0 == _tcsicmp( stInterface.szName, szIName ) )
					{
						traffic = *((DWORD *)( (BYTE *)counterBlockPtr + processIdOffset) );
						double acttraffic	= (double)traffic;
						double trafficdelta;

						//	Do we handle a new interface (e.g. due a change of the interface number
						if ( m_nCurrentInterface != interfaceNumber )
						{
							m_dbLasttraffic		= acttraffic;
							trafficdelta		= 0.0;
							m_nCurrentInterface	= interfaceNumber;
						}
						else
						{
							trafficdelta		= acttraffic - m_dbLasttraffic;
							m_dbLasttraffic		= acttraffic;
						}
						delete [] data;
						data = NULL;

						return trafficdelta;
					}

					//	next instance
					instancePtr = NextInstance(instancePtr);
				}
			}

			//	next object in list
			objectPtr = NextObject(objectPtr);
		}

		delete [] data;
		data = NULL;

		return 0;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return 0;
	}
}

/**
 *	Enumerate installed interfaces. 
 *	See comments above
 */
BOOL CDeNetTraffic::GetInterfaces()
{
	try
	{
		//
		//	清空记录
		//
		//m_slInterfaces.RemoveAll();
		m_mapInterfaces.clear();

		unsigned char *data = (unsigned char*)malloc(DEFAULT_BUFFER_SIZE);
		DWORD type;
		DWORD size = DEFAULT_BUFFER_SIZE;
		DWORD ret;

		char s_key[4096];
		sprintf( s_key , "%d" , 510 );

		while ( ( ret = RegQueryValueEx( HKEY_PERFORMANCE_DATA, s_key, 0, &type, data, &size ) ) != ERROR_SUCCESS )
		{
			while(ret == ERROR_MORE_DATA) 
			{
				size += DEFAULT_BUFFER_SIZE;
				data = (unsigned char*) realloc(data, size);
			} 
			if(ret != ERROR_SUCCESS)
			{
				return FALSE;
			}
		}

		PERF_DATA_BLOCK	 *dataBlockPtr = (PERF_DATA_BLOCK *)data;
		PERF_OBJECT_TYPE *objectPtr = FirstObject(dataBlockPtr);
		
		for ( int a = 0 ; a < (int)dataBlockPtr->NumObjectTypes; a++ ) 
		{
			char nameBuffer[255];
			if ( objectPtr->ObjectNameTitleIndex == 510 )
			{
				DWORD processIdOffset = ULONG_MAX;
				PERF_COUNTER_DEFINITION *counterPtr = FirstCounter(objectPtr);

				for ( int b = 0; b < (int)objectPtr->NumCounters; b++ ) 
				{
					if ( counterPtr->CounterNameTitleIndex == 520 )
					{
						processIdOffset = counterPtr->CounterOffset;
					}

					counterPtr = NextCounter(counterPtr);
				}

				if ( processIdOffset == ULONG_MAX )
				{
					free( data );
					return 1;
				}

				PERF_INSTANCE_DEFINITION * instancePtr = FirstInstance( objectPtr );

				for ( b = 0; b < objectPtr->NumInstances; b++ ) 
				{
					wchar_t *namePtr = (wchar_t *) ((BYTE*)instancePtr + instancePtr->NameOffset);
					PERF_COUNTER_BLOCK *counterBlockPtr = GetCounterBlock(instancePtr);
					char *pName = WideToMulti( namePtr, nameBuffer, sizeof(nameBuffer) );
					DWORD bandwith = *((DWORD *) ((BYTE *)counterBlockPtr + processIdOffset));				
					DWORD tottraff = 0;

					//
					//	添加到 interface 列表中
					//
					/*
					m_slInterfaces.AddTail( CString(pName) );
					*/
					INTERFACE_NAME stInterfaceName;
					_sntprintf( stInterfaceName.szName, sizeof(stInterfaceName.szName)-sizeof(TCHAR), "%s", pName );
					m_mapInterfaces.insert( map<INT,INTERFACE_NAME>::value_type( b, stInterfaceName ) );

					//m_lstBandwidths.AddTail( bandwith );
					m_mapBandwidths.insert( map<DWORD,DWORD>::value_type( b, bandwith ) );

					//m_lstTotalTraffics.AddTail( tottraff );  // initial 0, just for creating the list
					m_mapTotalTraffics.insert( map<DWORD,DWORD>::value_type( b, tottraff ) );

					instancePtr = NextInstance(instancePtr);
				}
			}
			objectPtr = NextObject(objectPtr);
		}
		free( data );

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
}

/**
 *	Returns the count of installed interfaces
 */
INT CDeNetTraffic::GetNetworkInterfacesCount()
{
	//return m_slInterfaces.GetCount();
	return m_mapInterfaces.size();
}

/**
 *	Returns the name of the given interface (-number)
 */
BOOL CDeNetTraffic::GetNetworkInterfaceName( INT index, LPTSTR lpszInterfaceName, DWORD dwSize )
{
	/*
	POSITION pos = m_slInterfaces.FindIndex(index);
	if ( pos )
	{
		_sntprintf( lpszInterfaceName, dwSize-sizeof(TCHAR), "%s", m_slInterfaces.GetAt( pos ) );
		return TRUE;
	}*/

	map<INT,INTERFACE_NAME>::const_iterator it;

	it = m_mapInterfaces.find( index );
	if ( it != m_mapInterfaces.end() )
	{
		_sntprintf( lpszInterfaceName, dwSize-sizeof(TCHAR), "%s", it->second.szName );
		return TRUE;
	}
	return FALSE;
}

/**
 *	Returns bandwith of interface e.g. 100000 for 100MBit
 */
DWORD CDeNetTraffic::GetInterfaceBandwidth( INT index )
{
	/*
	POSITION pos = m_lstBandwidths.FindIndex( index );
	if ( NULL == pos )
	{
		return 0;
	}
	else
	{
		return ( m_lstBandwidths.GetAt(pos) / 8 );
	}*/
	
	map<DWORD,DWORD>::const_iterator it;

	it = m_mapBandwidths.find( index );
	if ( it != m_mapBandwidths.end() )
	{
		return ( it->second / 8 );
	}
	return 0;
}

/**
 *	Sometime it is nice to know, how much traffic has a specific interface sent and received
 */
DWORD CDeNetTraffic::GetInterfaceTotalTraffic( INT index )
{
	/*DWORD		totaltraffic = 0;
	POSITION	pos;

	pos = m_lstTotalTraffics.FindIndex( index );
	if ( pos )
	{
		totaltraffic = m_lstTotalTraffics.GetAt( pos );
		if ( 0.0 == totaltraffic )
		{
			GetTraffic( index );
			pos = m_lstTotalTraffics.FindIndex( index );
			if ( pos )
			{
				totaltraffic = m_lstTotalTraffics.GetAt( pos );
			}
		}
	}

	return totaltraffic;
	*/

	DWORD totaltraffic = 0;
	map<DWORD,DWORD>::const_iterator it;

	it = m_mapTotalTraffics.find( index );
	if ( it != m_mapTotalTraffics.end() )
	{
		totaltraffic = it->second;
		if ( 0.0 == totaltraffic )
		{
			GetTraffic( index );
			it = m_mapTotalTraffics.find( index );
			if ( it != m_mapTotalTraffics.end() )
			{
				totaltraffic = it->second;
			}
		}
	}
	return totaltraffic;

}

/**
 *	To prevent direct manipulation of member variables....
 */
INT CDeNetTraffic::GetTrafficType()
{
	return m_nCurrentTrafficType;
}
VOID CDeNetTraffic::SetTrafficType( INT nTrafficType )
{
	m_nCurrentTrafficType = nTrafficType;
}
