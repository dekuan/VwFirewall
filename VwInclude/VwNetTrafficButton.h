/*******************************************

	CVwNetTrafficButton

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

#ifndef __VWNETTRAFFICBUTTON_HEADER__
#define __VWNETTRAFFICBUTTON_HEADER__


#include "VwNetTraffic.h"	// Added by ClassView
#include "Windows.h"


#define STARTINDEX	2000
#define ENDINDEX	2999
#define SET_SCALING	3000



/////////////////////////////////////////////////////////////////////////////
// CVwNetTrafficButton window

//VOID CALLBACK InterfaceChanged(int newInterface);

//typedef FARPROC INTERFACECHANCEDPROC;
typedef VOID (CALLBACK* INTERFACECHANCEDPROC)(int);
typedef VOID (CALLBACK* TRAFFICCHANGEDPROC)( double dblData );



class CVwNetTrafficButton : public CButton
{
// Construction
public:
	CVwNetTrafficButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVwNetTrafficButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	void SetFigureWaveColor( COLORREF clrColor );
	void SetFigureTextColor( COLORREF clrColor );
	void SetFigureBgColor( COLORREF clrColor );
	void SetDrawGlassStyle( BOOL bDrawGlassStyle );

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
	void SelectTrafficChangedNotificationFunction( TRAFFICCHANGEDPROC callfunct );
	void SetUpdateSpeed( UINT netspeed, UINT gridspeed );
	void ReInit( int newInterface );
	void ReInit( RECT newSize );
	virtual ~CVwNetTrafficButton();

	VOID DrawGlass( HDC hDC, CRect rect );
	VOID DrawGradient( HDC hDC, CRect rect, COLORREF StartColor, COLORREF EndColor, long Direction );

	// Generated message map functions
protected:
	//{{AFX_MSG(CVwNetTrafficButton)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void InterfaceHasChanged();

	INTERFACECHANCEDPROC	m_callbackFunc_interface;
	TRAFFICCHANGEDPROC	m_callbackFunc_traffic;

	CVwNetTraffic		m_cTrafficClass;
	CVwNetTraffic		* m_pcTrafficClassAll;
	INT			m_nInterfaceCount;

	CFont			smallFont;
	CBrush			m_brColorBrush;
	CBrush			m_brWaveBrush;

	COLORREF		m_clrBlue;
	COLORREF		m_clrBlack;
	COLORREF		m_clrText;
	COLORREF		m_clrDarkBlue;

	COLORREF		m_clrLightgreen;
	COLORREF		m_clrDarkgreen;

	//	²¨µÄÑÕÉ«
	COLORREF		m_clrWave;
	//	±³¾°µÄÑÕÉ«
	COLORREF		m_clrBackGround;

	//	±³¾°Í¼Æ¬
	BOOL		m_bDrawGlassStyle;

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

#endif // __VWNETTRAFFICBUTTON_HEADER__
