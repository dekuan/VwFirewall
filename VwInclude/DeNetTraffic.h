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

// VwNetTraffic.h: interface for the CDeNetTraffic class.
//
//////////////////////////////////////////////////////////////////////


#ifndef __DENETTRAFFIC_HEADER__
#define __DENETTRAFFIC_HEADER__


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <limits.h>

//#include <afxtempl.h>

#include <map>
using namespace std;


//////////////////////////////////////////////////////////////////////////
//
#define DEFAULT_BUFFER_SIZE 40960L


#define PLOTGRANULATRITY	1		//	两个线之间的间隔 Defines the width of the rectangle representing a bar in the diagram

#define NETUPDATESPEED		1000		// Value for the statistics timer
#define GRIDUPDATESPEED		50		// Value vor the grid timer
#define GRIDTIMER		1		// ID for the grid timer
#define NETTIMER		2		// ID for the statistics timer

#define GRIDXRESOLUTION		12		// Distance for grid in x direction
#define GRIDYRESOLUTION		30		// Distance for grid in y direction
#define GRIDSCROLLXSPEED	-1		// How fast grid scrolls in x direction
#define GRIDSCROLLYSPEED	0		// How fast grid scrolls in y direction



typedef struct _TRAFFIC_ENTRY_
{
	double value;
	BOOL connected;
	
} TRAFFICENTRY;

typedef struct _INTERFACE_NAME
{
	TCHAR szName[ MAX_PATH ];
	
}INTERFACE_NAME, *LPINTERFACE_NAME;



/**
 *	CDeNetTraffic class
 */
class CDeNetTraffic  
{
public:
	CDeNetTraffic();
	virtual ~CDeNetTraffic();

	enum TrafficType 
	{
		ENUM_TRAFFIC_TOTAL	= 388,
		ENUM_TRAFFIC_IN		= 264,
		ENUM_TRAFFIC_OUT	= 506
	};

	INT     GetTrafficType();
	VOID	SetTrafficType( INT nTrafficType );
	DWORD	GetInterfaceTotalTraffic( INT index );
	BOOL	GetNetworkInterfaceName( INT index, LPTSTR lpszInterfaceName, DWORD dwSize );
	INT	GetNetworkInterfacesCount();
	double	GetTraffic( INT interfaceNumber );
	double	GetTrafficProc( INT interfaceNumber );

	DWORD	GetInterfaceBandwidth( INT index );

private:
	BOOL	GetInterfaces();
	double	m_dbLasttraffic;

	//CStringList m_slInterfaces;
	//CList < DWORD, DWORD &>	m_lstBandwidths;
	//CList < DWORD, DWORD &>	m_lstTotalTraffics;

	map<INT,INTERFACE_NAME> m_mapInterfaces;
	map<DWORD,DWORD> m_mapBandwidths;
	map<DWORD,DWORD> m_mapTotalTraffics;

	int	m_nCurrentInterface;
	int	m_nCurrentTrafficType;

};


#endif	// __DENETTRAFFIC_HEADER__
