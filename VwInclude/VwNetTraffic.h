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

// VwNetTraffic.h: interface for the CVwNetTraffic class.
//
//////////////////////////////////////////////////////////////////////


#ifndef __VWNETTRAFFIC_HEADER__
#define __VWNETTRAFFIC_HEADER__



#include <afxtempl.h>


//////////////////////////////////////////////////////////////////////////
//
#define DEFAULT_BUFFER_SIZE 40960L

typedef struct _TRAFFIC_ENTRY_
{
	_TRAFFIC_ENTRY_()
	{
		memset( this, 0, sizeof(_TRAFFIC_ENTRY_) );
	}
	double value;
	BOOL connected;
	
} TRAFFICENTRY;


#define PLOTGRANULATRITY	1		//	两个线之间的间隔 Defines the width of the rectangle representing a bar in the diagram

#define NETUPDATESPEED		1000		// Value for the statistics timer
#define GRIDUPDATESPEED		50		// Value vor the grid timer
#define GRIDTIMER		1		// ID for the grid timer
#define NETTIMER		2		// ID for the statistics timer

#define GRIDXRESOLUTION		12		// Distance for grid in x direction
#define GRIDYRESOLUTION		30		// Distance for grid in y direction
#define GRIDSCROLLXSPEED	-1		// How fast grid scrolls in x direction
#define GRIDSCROLLYSPEED	0		// How fast grid scrolls in y direction







/**
 *	CVwNetTraffic class
 */
class CVwNetTraffic  
{
public:
	CVwNetTraffic();
	virtual ~CVwNetTraffic();

	enum TrafficType 
	{
		ENUM_ALLTRAFFIC		= 388,
		ENUM_INCOMINGTRAFFIC	= 264,
		ENUM_OUTGOINGTRAFFIC	= 506
	};

	VOID	SetTrafficType( INT nTrafficType );
	DWORD	GetInterfaceTotalTraffic( INT index );
	BOOL	GetNetworkInterfaceName( CString *InterfaceName, INT index );
	INT	GetNetworkInterfacesCount();
	DOUBLE	GetTraffic( INT interfaceNumber );

	DWORD	GetInterfaceBandwidth( INT index );

private:
	BOOL	GetInterfaces();
	double	m_dbLasttraffic;

	CStringList m_slInterfaces;
	CList < DWORD, DWORD &>	m_lstBandwidths;
	CList < DWORD, DWORD &>	m_lstTotalTraffics;

	int	m_nCurrentInterface;
	int	m_nCurrentTrafficType;

};


#endif	// __VWNETTRAFFIC_HEADER__
