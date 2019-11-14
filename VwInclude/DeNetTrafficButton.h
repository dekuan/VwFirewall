/*******************************************

	CDeNetTrafficButton

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

#ifndef __DENETTRAFFICBUTTON_HEADER__
#define __DENETTRAFFICBUTTON_HEADER__


#include "DeNetTraffic.h"	// Added by ClassView
#include "Windows.h"


#define STARTINDEX	2000
#define ENDINDEX	2999
#define SET_SCALING	3000



/////////////////////////////////////////////////////////////////////////////
// CDeNetTrafficButton window

//VOID CALLBACK InterfaceChanged(int newInterface);

//typedef FARPROC INTERFACECHANCEDPROC;
typedef VOID (CALLBACK* INTERFACECHANCEDPROC)(int);

class CDeNetTrafficButton : public CButton
{
// Construction
public:
	CDeNetTrafficButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeNetTrafficButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	void SetAdaptiveScaling( BOOL adaptive );
	void SetInterfaceNumber( int interfacenumber );
	enum traffictype
	{
		ENUM_TRAFFIC_TOTAL	= 0,
		ENUM_TRAFFIC_INCOMING	= 1,
		ENUM_TRAFFIC_OUTGOING	= 2
	};

	void SelectTrafficType( int trafficType );
	void SetInterfaceNumberNotificationFunction( INTERFACECHANCEDPROC callfunct );
	void SetUpdateSpeed( UINT netspeed, UINT gridspeed );
	void ReInit( int newInterface );
	void ReInit( RECT newSize );
	virtual ~CDeNetTrafficButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDeNetTrafficButton)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void InterfaceHasChanged();

	INTERFACECHANCEDPROC	m_callbackFunc_interface;

	CDeNetTraffic		m_cTrafficClass;
	CDeNetTraffic		* m_pcTrafficClassAll;
	INT			m_nInterfaceCount;

	CFont			smallFont;
	CBrush			m_brColorBrush;

	COLORREF		m_clrBlue;
	COLORREF		m_clrBlack;
	COLORREF		m_clrText;
	COLORREF		m_clrDarkBlue;

	COLORREF		m_clrLightgreen;
	COLORREF		m_clrDarkgreen;

	//	波的颜色
	COLORREF		m_clrWave;
	//	背景的颜色
	COLORREF		m_clrBackGround;

	CBitmap		m_clrBrushBmp;
	CPen		m_GridPen;
	CPen		m_WavePen;
	CSize		m_TGSize;

	CBrush		m_bhGreenBrush;
	CPoint		m_ptOrgBrushOrigin; 

	RECT		m_TrafficDrawRectangle;
	RECT		m_TrafficDrawUpdateRectangle;

	CString		m_strCurrentTraffic;
	CString		m_strMaximalTraffic;
	CString		m_strAllTraffic;

	DWORD		m_dwTrafficEntries;

	BOOL		m_bInitalized;
	BOOL		m_bIsOnline;
	BOOL		m_bBrushInitalized;

	CRgn		m_rgnShapeWNDRegion;
	CRgn		m_rgnShapeDCRegion;

	DOUBLE		m_dbMaxTrafficAmount;
	CString		m_strToggleStatusText;

	INT		m_nSelectedInterface;

	BOOL		m_bUseAdaptiveScale;

	TRAFFICENTRY *	m_pTrafficStats;

	INT		m_nGridXStartPos;
	INT		m_nGridYStartPos;
	INT		m_nPlotgranularity;

	// Public modification variables
public:
	INT		m_nGridXResolution;		// The size of grid raster
	INT		m_nGridYResolution;
	INT		m_nGridScrollXSpeed;		// Scroll speed of the grid
	INT		m_nGridScrollYSpeed; 
	INT		m_nNetUpdateSpeed;		// Should be set via SetUpdateSpeet method
	INT		m_nGridUpdateSpeed;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __DENETTRAFFICBUTTON_HEADER__
