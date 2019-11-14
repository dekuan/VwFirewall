/*******************************************

	VwNetTrafficButton

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

// VwNetTrafficButton.cpp : implementation file
//

#include "stdafx.h"
#include "VwNetTrafficButton.h"
#include "MemDC.h"
#include <math.h>

#ifndef _WIN32_IE
	#define _WIN32_IE 0x0500
#endif
#include <commctrl.h>
#include <intshcut.h>
#include <wininet.h> 




/////////////////////////////////////////////////////////////////////////////
// VwNetTrafficButton

CVwNetTrafficButton::CVwNetTrafficButton()
{
	m_bBrushInitalized		= FALSE;

	m_callbackFunc_interface	= NULL;
	m_callbackFunc_traffic		= NULL;

	m_bUseAdaptiveScale		= FALSE;
	m_nGridXStartPos		= 0;
	m_nGridYStartPos		= 0;
	m_nGridXResolution		= GRIDXRESOLUTION;
	m_nGridYResolution		= GRIDYRESOLUTION;
	m_nGridScrollXSpeed		= GRIDSCROLLXSPEED;
	m_nGridScrollYSpeed		= GRIDSCROLLYSPEED;
	m_nPlotgranularity		= PLOTGRANULATRITY;
	m_nNetUpdateSpeed		= NETUPDATESPEED;
	m_nGridUpdateSpeed		= GRIDUPDATESPEED;
	m_strToggleStatusText		= "local maximum";

	//	背景图片
	m_bDrawGlassStyle		= FALSE;

	//
	//	初始化颜色
	//
	//m_clrText	= RGB( 0xFF, 0xFF, 0x00 );
	m_clrText	= RGB( 0xFF, 0xFF, 0xFF );
	m_clrWave	= RGB( 0x54, 0xA9, 0xFA );
	m_clrBackGround	= RGB( 0x00, 0x00, 0x00 );

	//m_clrBlue	= m_clrWave;
	m_clrDarkBlue	= RGB( 0, 0, 75 );

	m_clrBlack	= RGB( 0, 0, 0 );
	m_clrLightgreen	= RGB( 156, 255, 156 );
	m_clrDarkgreen	= RGB( 0x00, 0x80, 0x40 );


	//
	//	为所有网卡初始化类
	//
	m_pcTrafficClassAll	= NULL;
	m_nInterfaceCount	= m_cTrafficClass.GetNetworkInterfacesCount();
	try
	{
		if ( m_nInterfaceCount > 0 )
		{
			m_pcTrafficClassAll = new CVwNetTraffic[ m_nInterfaceCount ];
		}
	}
	catch ( ... )
	{
	}
}

CVwNetTrafficButton::~CVwNetTrafficButton()
{
	if ( m_pcTrafficClassAll )
	{
		delete [] m_pcTrafficClassAll;
	}
}


BEGIN_MESSAGE_MAP(CVwNetTrafficButton, CButton)
	//{{AFX_MSG_MAP(CVwNetTrafficButton)
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVwNetTrafficButton message handlers



void CVwNetTrafficButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CButton::PreSubclassWindow();

	// make sure we are an owner draw button
	ModifyStyle( 0 , BS_OWNERDRAW );

	this->GetWindowRect( &m_TrafficDrawRectangle );
	this->GetWindowRect( &m_TrafficDrawUpdateRectangle );
	ScreenToClient( &m_TrafficDrawUpdateRectangle );
	ScreenToClient( &m_TrafficDrawRectangle );


	m_TGSize.cx = m_TrafficDrawRectangle.right - m_TrafficDrawRectangle.left;
	m_TGSize.cy = m_TrafficDrawRectangle.bottom - m_TrafficDrawRectangle.top;

	m_bInitalized		= FALSE;
	m_dbMaxTrafficAmount	= 0.0;
	m_nSelectedInterface	= -1;

	//
	//	创建字体
	//
	smallFont.CreateFont
		(
			-10, 0, 0, 0,
			FW_THIN, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, VARIABLE_PITCH, _T("Verdana")
		);



	//
	//	创建笔刷
	//
	//m_bhGreenBrush.CreateSolidBrush( m_clrBlue );

	m_GridPen.CreatePen( PS_SOLID, 1, m_clrDarkgreen );
	m_WavePen.CreatePen( PS_SOLID, 1, m_clrWave );

	GetWindowRect( &m_TrafficDrawRectangle );
	ScreenToClient( &m_TrafficDrawRectangle );
	//	m_TrafficDrawRectangle.left += 2;
	//	m_TrafficDrawRectangle.right -= 3;
	//	m_TrafficDrawRectangle.bottom -= 4;
	
	m_TGSize.cx = m_TrafficDrawRectangle.right - m_TrafficDrawRectangle.left;
	m_TGSize.cy = m_TrafficDrawRectangle.bottom - m_TrafficDrawRectangle.top;
	
	
	m_dwTrafficEntries	= m_TGSize.cx / m_nPlotgranularity;
	m_pTrafficStats		= new TRAFFICENTRY[m_dwTrafficEntries+1];

	for ( DWORD x = 0; x < m_dwTrafficEntries; x++ )
	{
		m_pTrafficStats[x].connected	= TRUE;	//	FALSE;
		m_pTrafficStats[x].value	= 0.0;	//	(float)x * (float)m_dwTrafficEntries*0.05;//;10.0 + 10.0*(sin(5.0*(float)x*3.14/180.0));
	}

	m_dbMaxTrafficAmount	= 0.0;
	m_nSelectedInterface	= -1;


	CRgn rectRgn, ellRgn, finalRgn;
	rectRgn.CreateRectRgn( 0, 0, m_TGSize.cx, m_TGSize.cy );
	m_rgnShapeWNDRegion.CreateRectRgn( 0, 0, m_TGSize.cx, m_TGSize.cy );
	m_rgnShapeDCRegion.CreateRectRgn( 0, 0, m_TGSize.cx, m_TGSize.cy );

	int x1, x2, y1, y2, xe, ye, xs, ys;
	int xof, yof;
	int r;
	xs	= m_TGSize.cx;
	ys	= m_TGSize.cy;
	x1	= 0;
	y1	= 0;
	x2	= xs;
	y2	= ys;
	xe	= 0;		//	Radius of edge
	ye	= 0;		//	Radius of edge
	xof	= (int)( (float)xs*0.0 );
	yof	= (int)( (float)ys*0.0 );
	r	= ellRgn.CreateRoundRectRgn( x1, y1, x2, y2, xe, ye );
	r	= ellRgn.OffsetRgn( -xof, -yof );
	r	= m_rgnShapeWNDRegion.CombineRgn( &rectRgn, &ellRgn, RGN_AND );
	r	= m_rgnShapeDCRegion.CombineRgn( &rectRgn, &ellRgn, RGN_AND );

	this->SetWindowRgn( (HRGN)m_rgnShapeWNDRegion, TRUE );


	m_bInitalized	= TRUE;

	SetTimer( GRIDTIMER, m_nGridUpdateSpeed, 0 );
	SetTimer( NETTIMER, m_nNetUpdateSpeed, 0 );

}

VOID CVwNetTrafficButton::DrawGlass( HDC hDC, CRect rect )
{
	CRect rectGradient;
	HPEN hPen1		= NULL;


	//
	//	画边框
	//
	hPen1 = CreatePen( PS_SOLID, 0, GetSysColor(COLOR_3DSHADOW) );
	SelectObject( hDC, hPen1 );
	
	MoveToEx( hDC, rect.left, rect.top, NULL );
	LineTo( hDC, rect.right, rect.top );

	MoveToEx( hDC, rect.left, rect.top, NULL );
	LineTo( hDC, rect.left, rect.bottom );

	DeleteObject( hPen1 );



	if ( m_bDrawGlassStyle )
	{
		//
		//	画顶部直线
		//
		hPen1 = CreatePen( PS_SOLID, 0, RGB( 0xBE, 0xBE, 0xBE ) );
		SelectObject( hDC, hPen1 );

		MoveToEx( hDC, rect.left + 2, rect.top + 2, NULL );
		LineTo( hDC, rect.right-2, rect.top + 2 );

		DeleteObject( hPen1 );

		//
		//	画左右渐变线
		//
		rectGradient = rect;
		rectGradient.top	+= 2;
		rectGradient.left	+= 2;
		rectGradient.right	= rectGradient.left + 2;
		rectGradient.bottom	/= 2;
		DrawGradient( hDC, &rectGradient, RGB( 0xBE, 0xBE, 0xBE ), RGB( 0x00, 0x00, 0x00 ), 1 );

		rectGradient = rect;
		rectGradient.top	+= 2;
		rectGradient.left	= rectGradient.right - 4;
		rectGradient.right	-= 2;
		rectGradient.bottom	/= 2;
		DrawGradient( hDC, &rectGradient, RGB( 0xBE, 0xBE, 0xBE ), RGB( 0x00, 0x00, 0x00 ), 1 );

		//
		//	画主渐变区域
		//
		rectGradient = rect;
		rectGradient.top	+= 3;
		rectGradient.left	+= 3;
		rectGradient.right	-= 3;
		rectGradient.bottom	/= 2;

		DrawGradient( hDC, &rectGradient, RGB( 0x80, 0x80, 0x80 ), RGB( 0x00, 0x00, 0x00 ), 1 );
	}
}

/**
 *	@ Private
 *	绘制渐变窗口
 */
VOID CVwNetTrafficButton::DrawGradient( HDC hDC, CRect rect, COLORREF StartColor, COLORREF EndColor, long Direction )
{
	//	Direction=0 横向
	//	Direction=1 纵向

	//在给定的rect中绘制渐变色填充StartColor----EndColor
	CRect rectFill;			   // Rectangle for filling band
	float fStep;              // How wide is each band?
	HBRUSH brush;			// Brush to fill in the bar	
	rectFill=rect;
	int r, g, b;
	r = (GetRValue(EndColor) - GetRValue(StartColor));
	g = (GetGValue(EndColor) - GetGValue(StartColor));
	b =  (GetBValue(EndColor) - GetBValue(StartColor));
	
	int nSteps = max(abs(r), max(abs(g), abs(b)));
	if(Direction==0)
		nSteps=min(nSteps,rect.right-rect.left);
	else
		nSteps=min(nSteps,rect.bottom-rect.top);
	float rStep, gStep, bStep;
	if(Direction==0)
		fStep = (float)(rect.right - rect.left)/ (float)nSteps;
	else
		fStep = (float)(rect.bottom - rect.top)/ (float)nSteps;
	rStep = r/(float)nSteps;
	gStep = g/(float)nSteps;
	bStep = b/(float)nSteps;
	
	r = GetRValue(StartColor);
	g = GetGValue(StartColor);
	b = GetBValue(StartColor);
	
	
	for (int iOnBand = 0; iOnBand < nSteps; iOnBand++) 
	{
		if(Direction==0)
			::SetRect(&rectFill,
			rect.left+(int)(iOnBand * fStep),       // Upper left X
			rect.top,									 // Upper left Y
			rect.left+(int)((iOnBand+1) * fStep),          // Lower right X
			rect.bottom);			// Lower right Y
		else
			::SetRect(&rectFill,
			rect.left,       // Upper left X
			rect.top+(int)(iOnBand * fStep),			 // Upper left Y
			rect.right,          // Lower right X
			rect.top+(int)((iOnBand+1) * fStep));			// Lower right Y
		brush = ::CreateSolidBrush(RGB(r+rStep*iOnBand, g + gStep*iOnBand, b + bStep *iOnBand));
		::FillRect( hDC, &rectFill, brush);
		DeleteObject(brush);
	}
}

void CVwNetTrafficButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
//	CButton::OnDrawItem(nIDCtl, lpDrawItemStruct);

	ASSERT( lpDrawItemStruct != NULL );

	CBitmap * pOldBitmap;
	CSize cBmpsSize;

	CDC * pDC   = CDC::FromHandle( lpDrawItemStruct->hDC );
	int erg = pDC->SelectClipRgn( &m_rgnShapeDCRegion );

	CRect rect = lpDrawItemStruct->rcItem;
	UINT state = lpDrawItemStruct->itemState;
	UINT nStyle = GetStyle();

	int nSavedDC = pDC->SaveDC();


	// Create the brush for the color bar
	if ( ! m_bBrushInitalized )
	{
		CBitmap bmp;
		CMemDC *memDC = new CMemDC( pDC );

		RECT clipRect;
		memDC->GetClipBox( &clipRect );

		if ( clipRect.right - clipRect.left > 1 )
		{
			bmp.CreateCompatibleBitmap( memDC, m_nPlotgranularity, m_TGSize.cy );

			pOldBitmap = memDC->SelectObject( &bmp );
			cBmpsSize = bmp.GetBitmapDimension();

			//
			//	创建彩色画笔
			//

			//	Need for scaling the color to the size of button
			double factor = 255.0 / (float)m_TGSize.cy;
			BYTE r, g, b;
			for ( int x = 0; x < m_TGSize.cy; x++ )
			{
				g = (BYTE)( 255 - factor * x );
				r = (BYTE)( factor * x );
				b = (BYTE)64;

				//
				//	画波
				//
				memDC->SetPixelV( 0, x, m_clrWave );
				memDC->SetPixelV( 1, x, m_clrWave );
			}
			memDC->SelectObject( pOldBitmap );

			memDC->DrawEdge( &m_TrafficDrawRectangle, BDR_RAISEDINNER, BF_DIAGONAL );

			if ( m_brColorBrush.m_hObject )
			{
				m_brColorBrush.DeleteObject();
			}
			m_brColorBrush.CreatePatternBrush( &bmp );
			m_bBrushInitalized = TRUE;
		}

		//
		//	为控件加上边框
		//
		/*
		CRect rectFrm( rect.left, rect.top, rect.right-1, rect.bottom-1 );
		memDC->Draw3dRect( &rectFrm, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT) );
		rectFrm.InflateRect( -1, -1 );
		memDC->Draw3dRect( &rectFrm, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DFACE) );
		rectFrm.InflateRect( -1, -1 );
		*/
		
		delete memDC;
	}

	if ( m_bInitalized )
	{
		COLORREF backcolor = GetSysColor(COLOR_BTNFACE);

		CBrush brush;
		CMemDC *memDC = new CMemDC( pDC );

		RECT clipRect;
		memDC->GetClipBox(&clipRect);
		memDC->FillSolidRect( &clipRect, backcolor );

		CFont *oldFont;
		int xp, yp, xx, yy;
		m_ptOrgBrushOrigin = memDC->GetBrushOrg();

		oldFont = memDC->SelectObject(&smallFont);

		double scale = (double)m_TGSize.cy / (double)m_dbMaxTrafficAmount;

		yp = m_TrafficDrawRectangle.bottom;
		xp = m_TrafficDrawRectangle.left;

		RECT fillrect;
		CString tmp;


		//	Fill the background
		m_clrBackGround = memDC->GetBkColor();
		brush.CreateSolidBrush( m_clrBlack );	//back);
		memDC->FillRect( &m_TrafficDrawRectangle, &brush );

		//
		//	画玻璃效果
		//
		DrawGlass( memDC->GetSafeHdc(), &rect );

		//	draw the grid
		int xgridlines, ygridlines;

		xgridlines = m_TGSize.cx / m_nGridXResolution;
		ygridlines = m_TGSize.cy / m_nGridYResolution;
		CPen* oldPen = memDC->SelectObject( &m_GridPen );
		//	Create the vertical lines
/*		for ( int x = 0; x <= xgridlines; x++ )
		{
			memDC->MoveTo( x * m_nGridXResolution + m_nGridXStartPos, 0 );
			memDC->LineTo( x * m_nGridXResolution + m_nGridXStartPos, m_TGSize.cy );
		}
		// And the horizontal lines
		for ( int y = 0; y <= ygridlines; y++ )
		{
			memDC->MoveTo( 0, m_nGridYStartPos + m_TGSize.cy - y * m_nGridYResolution - 2 );
			memDC->LineTo( m_TGSize.cx, m_nGridYStartPos + m_TGSize.cy - y * m_nGridYResolution - 2);
		}
*/
		memDC->SelectObject( oldPen );


		//	Draw Wave ..
		m_nGridXStartPos += m_nGridScrollXSpeed;
		m_nGridYStartPos += m_nGridScrollYSpeed;
	
		if ( m_nGridXStartPos < 0 )
		{	
			m_nGridXStartPos = m_nGridXResolution;
		}	
		if ( m_nGridXStartPos > m_nGridXResolution )
		{	
			m_nGridXStartPos = 0;
		}	
		if ( m_nGridYStartPos < 0 )
		{	
			m_nGridYStartPos = m_nGridYResolution;
		}	
		if ( m_nGridYStartPos > m_nGridYResolution )
		{	
			m_nGridYStartPos = 0;
		}	

		//	..
		oldPen = memDC->SelectObject( &m_WavePen );

		for ( DWORD cnt = 0; cnt < m_dwTrafficEntries; cnt++ )
		{
			xx = xp + cnt * m_nPlotgranularity;
			double traffic = (double)m_pTrafficStats[ cnt ].value;
			yy = yp - (int)( (double)m_pTrafficStats[ cnt ].value * scale );

			if ( traffic > 1024 * 1000 )
			{
				continue;
			}

			int ylast = 0;
			if ( cnt > 0 )
			{	
				ylast = yp - (int)( (double)m_pTrafficStats[ cnt - 1 ].value * scale );
			}	
			else
			{	
				ylast = 0;
			}	

			//	Just paint if we are connected...
			if ( m_pTrafficStats[cnt].connected && ylast > 0 )
			{
				fillrect.bottom = yp - 4;
				fillrect.top	= yy + 4;
				fillrect.left	= xx - 4;
				fillrect.right	= xx + m_nPlotgranularity - 4;
				if ( fillrect.top > fillrect.bottom )
				{
					fillrect.top = fillrect.bottom - 1;
				}

				if ( fillrect.left <= 4 )
				{
					continue;
				}

				/*
				memDC->MoveTo( fillrect.left, ylast );
				memDC->LineTo( fillrect.right, fillrect.top );
				*/

				memDC->SetBrushOrg( xx, yp );
				if ( m_pTrafficStats[ cnt ].value > 0.0 ) 
				{
					memDC->FillRect( &fillrect, &m_brColorBrush );
					//memDC->SetPixelV( xx, yy, m_clrText );
				}
			}
		}

		memDC->SelectObject( oldPen );


		//	last print the textual statistic
		tmp.Format( _T("%8.1f"), m_pTrafficStats[ m_dwTrafficEntries - 1 ].value );
		COLORREF textcolor = memDC->GetTextColor();
		int bkmode = memDC->GetBkMode();
		memDC->SetBkMode( TRANSPARENT );
		memDC->SetTextColor( m_clrDarkBlue );	// darkblue
		memDC->TextOut( 6, 5, m_strAllTraffic );
		memDC->SetTextColor( m_clrText );	// cyan
		memDC->TextOut( 5, 5, m_strAllTraffic ); 
		memDC->SetTextColor( textcolor );
		memDC->SetBkMode( bkmode );

		memDC->SelectObject( oldFont );
		memDC->SetBrushOrg( m_ptOrgBrushOrigin.x, m_ptOrgBrushOrigin.y );


		//
		//	为控件加上边框
		//
		/*
		CRect rectFrm( rect.left, rect.top, rect.right-1, rect.bottom-1 );
		memDC->Draw3dRect( &rectFrm, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT) );
		rectFrm.InflateRect( -1, -1 );
		memDC->Draw3dRect( &rectFrm, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DFACE) );
		rectFrm.InflateRect( -1, -1 );
		*/

		delete memDC;
	}

	pDC->RestoreDC( nSavedDC );
}

// if you have the Microsoft platform SDK, uncomment the following statement
// and insert Wininet.lib in the linker section of the compiler
// #define _I_HAVE_PLATFORM_SDK_INSTALLED_

void CVwNetTrafficButton::OnTimer( UINT nIDEvent )
{
	// TODO: Add your message handler code here and/or call default
	if ( nIDEvent == NETTIMER )
	{
	
// If you have installed the Microsoft Platform SDK be happy, because you can use the feature
// added below..
#ifdef _I_HAVE_PLATFORM_SDK_INSTALLED_		

		DWORD flag, reserved;
		BOOL erg;
		flag = 0;//INTERNET_CONNECTION_OFFLINE ;
		reserved = 0;
		CHAR connectionname[1024];
		//	erg = InternetGetConnectedState(&flag, reserved);
		erg =  InternetGetConnectedStateEx
			(
				&flag,				//	OUT LPDWORD lpdwFlags,
				(LPSTR)&connectionname,	//	OUT LPTSTR lpszConnectionName,
				1024,				//	IN DWORD dwNameLen,
				0				//	IN DWORD dwReserved
			);

		m_bIsOnline = erg;
#else
		m_bIsOnline = TRUE;
#endif

		// Get current traffic
		INT	i		= 0;
		DWORD	x		= 0;
		double  traffic		= 0;
		double delta1		= 0.0;
		double delta2		= 0.0;
		double divisor		= 0.0;

		//DWORD  totaltraffic	= m_cTrafficClass.GetInterfaceTotalTraffic( m_nSelectedInterface );
	
		if ( m_nSelectedInterface >= 0 )
		{
			//
			//	获取指定网卡的流量 
			//
			traffic	= m_cTrafficClass.GetTraffic( m_nSelectedInterface );
		}
		else if ( m_nInterfaceCount > 0 )
		{
			//
			//	获取所有网卡的流量
			//
			for ( i = 0; i < m_nInterfaceCount; i ++ )
			{
				traffic += m_pcTrafficClassAll[ i ].GetTraffic( i );
			}
		}

		if ( traffic < 0 )
		{
			traffic = 0;
		}
		if ( traffic > 1024 * 100 )
		{
			//	为了图形不出问题，超过 200M 就不靠谱了
			traffic = 1024 * 100;
		}
	
		divisor		= ( 1000.0 / (double)NETUPDATESPEED );
		delta1		= (double)(traffic * divisor);
		//m_strCurrentTraffic.Format( "%.1f KB/sec", delta1 );

		
		// Should we recalculate the local maximum per session or per display?
		if ( m_bUseAdaptiveScale == TRUE )
		{
			m_dbMaxTrafficAmount = 0.0;
		}
		//
		//	Shift whole array 1 step to left and calculate local maximum
		//	将整个数组的数据向前挪动一个单位
		//
		for ( x = 0; x < m_dwTrafficEntries; x++ )
		{
			if ( ! IsBadReadPtr( (const void *)( & m_pTrafficStats[x+1].connected ), sizeof(BOOL) ) )
			{
				m_pTrafficStats[x].connected	= m_pTrafficStats[x+1].connected;
			}
			if ( ! IsBadReadPtr( (const void *)( & m_pTrafficStats[x+1].value ), sizeof(double) ) )
			{
				m_pTrafficStats[x].value	= m_pTrafficStats[x+1].value;
			}
			if ( m_pTrafficStats[x].value > m_dbMaxTrafficAmount )
			{
				m_dbMaxTrafficAmount = m_pTrafficStats[x].value;
			}
		}
		if ( m_bIsOnline )
		{
			m_pTrafficStats[ m_dwTrafficEntries ].connected	= TRUE;
			m_pTrafficStats[ m_dwTrafficEntries ].value	= traffic;
			if ( m_pTrafficStats[m_dwTrafficEntries].value > m_dbMaxTrafficAmount )
			{
				m_dbMaxTrafficAmount = m_pTrafficStats[m_dwTrafficEntries].value;
			}
		}
		else
		{
			m_pTrafficStats[m_dwTrafficEntries].connected	= FALSE;
			m_pTrafficStats[m_dwTrafficEntries].value	= traffic;
			if ( m_pTrafficStats[m_dwTrafficEntries].value > m_dbMaxTrafficAmount )
			{
				m_dbMaxTrafficAmount = m_pTrafficStats[m_dwTrafficEntries].value;
			}
		}
		
		divisor		= (1000.0/(double)NETUPDATESPEED);
		delta2		= (double)(m_dbMaxTrafficAmount * divisor);
		//m_strMaximalTraffic.Format( "%.1f KB/Sec", delta2 );
		//AllTraffic.Format("%.1f / %.1f KB/Sec", delta1, delta2 );
		if ( delta1 > 1024 )
		{
			m_strAllTraffic.Format( _T("%.1f mbps"), ( delta1/1024 ) );
		}
		else
		{
			m_strAllTraffic.Format( _T("%.1f kbps"), delta1 );
		}

		//
		//	trafic changed callback
		//
		if ( m_callbackFunc_traffic )
		{
			m_callbackFunc_traffic( delta1 );
		}
	}

	// Force a redraw
	Invalidate( FALSE );

	CButton::OnTimer(nIDEvent);
}

/*
	Create little popup
*/
void CVwNetTrafficButton::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CButton::OnRButtonDown( nFlags, point );
	return;



	HMENU hMenu , hPopup;
	long retId = 0;
	int InterfaceIndex = STARTINDEX;
	POINT pt;
	
	hMenu = CreatePopupMenu();			  //  Create a new popup menu
	if( hMenu )
	{	
		//  If the menu was created, add the item text
		CString tmp, tmp2;
		double tottraff = (double)m_cTrafficClass.GetInterfaceTotalTraffic(m_nSelectedInterface) / (1024.0*1024.0);
		m_cTrafficClass.GetNetworkInterfaceName( &tmp, m_nSelectedInterface );
		tmp2.Format( _T("%s : %.1f MB"), tmp, tottraff);
		
		AppendMenu( hMenu , MF_STRING , 1001 , tmp2 );
		AppendMenu( hMenu , MF_SEPARATOR , 1000 , NULL );
		// Submenu
		hPopup = CreatePopupMenu( );			  //  Create a new popup menu
		if( hPopup )
		{
			CString tmp;
			int cnt;
			for ( cnt = 0; cnt < m_cTrafficClass.GetNetworkInterfacesCount(); cnt++ )
			{
				CString tmp2;
				double tottraff = (double)m_cTrafficClass.GetInterfaceTotalTraffic(cnt) / (1024.0*1024.0);
				m_cTrafficClass.GetNetworkInterfaceName( &tmp, cnt );
				
				tmp2.Format( _T("%s : %.1f MB"), tmp, tottraff );
				AppendMenu( hPopup , MF_STRING , InterfaceIndex++, tmp2 );
			}
			AppendMenu( hMenu , MF_POPUP , ( UINT )hPopup , _T("Select Interface") );
			AppendMenu( hMenu , MF_SEPARATOR , 1000 , NULL );
			AppendMenu( hMenu , MF_STRING , SET_SCALING , _T("Toggle scaling to ") + m_strToggleStatusText );
		}
	}
	
	GetCursorPos( & pt );			 //  Get the cursor position and
	retId = TrackPopupMenu( hMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD,  pt.x, pt.y, 0, this->m_hWnd, NULL );  //  start the menu
	DestroyMenu( hMenu );				//  Kill the menu when done
	
	if ( 0 == retId )
	{
		return;
	}
	else
	{
		if ( retId == SET_SCALING )
		{
			if ( m_bUseAdaptiveScale )
			{
				m_bUseAdaptiveScale	= FALSE;
				m_strToggleStatusText	= "global maximum";
			}
			else
			{
				m_bUseAdaptiveScale	= TRUE;
				m_strToggleStatusText	= "local maximum";
			}
		}
		if ( ( retId >= STARTINDEX ) && ( retId < ENDINDEX ) )
		{
			m_nSelectedInterface	= retId - STARTINDEX;
			InterfaceHasChanged();
			ReInit( m_nSelectedInterface );
		}
	}

	CButton::OnRButtonDown( nFlags, point );
}

/*
	Button size has changed, we need an update of statistics.
	I am not sure if that works, because never used this...
*/
void CVwNetTrafficButton::ReInit( RECT newSize )
{
	KillTimer( GRIDTIMER );
	KillTimer( NETTIMER );

	if ( m_pTrafficStats )
	{
		delete [] m_pTrafficStats;
		m_pTrafficStats = NULL;
	}


	INT i		= 0;

	m_TGSize.cx		= newSize.right - newSize.left;
	m_TGSize.cy		= newSize.bottom - newSize.top;

	m_dwTrafficEntries	= m_TGSize.cx / m_nPlotgranularity;
	m_pTrafficStats		= new TRAFFICENTRY[m_dwTrafficEntries+1];
	
	m_dbMaxTrafficAmount	= 0.0;
	m_nSelectedInterface	= -1;

	for ( DWORD x = 0; x < m_dwTrafficEntries; x ++ )
	{
		m_pTrafficStats[x].connected	= TRUE;
		m_pTrafficStats[x].value	= 0.0;
	}


	if ( m_nSelectedInterface >= 0 )
	{
		//
		//	获取指定网卡的流量 
		//
		m_cTrafficClass.GetTraffic( m_nSelectedInterface );
		m_cTrafficClass.GetTraffic( m_nSelectedInterface );
	}
	else if ( m_nInterfaceCount > 0 )
	{
		//
		//	获取所有网卡的流量
		//
		for ( i = 0; i < m_nInterfaceCount; i ++ )
		{
			m_pcTrafficClassAll[ i ].GetTraffic( i );
			m_pcTrafficClassAll[ i ].GetTraffic( i );
		}
	}


	m_dbMaxTrafficAmount = 0.0;

	SetTimer( GRIDTIMER, m_nGridUpdateSpeed, 0 );
	SetTimer( NETTIMER,  m_nNetUpdateSpeed,	0 );
}

/**
 *	We want to monitor another interface
 */
void CVwNetTrafficButton::ReInit( int newInterface )
{
	KillTimer( GRIDTIMER );
	KillTimer( NETTIMER );

	INT i		= 0;
	DWORD x		= 0;


	m_nSelectedInterface = newInterface;

	for ( x = 0; x < m_dwTrafficEntries; x++ )
	{
		m_pTrafficStats[x].connected	= TRUE;
		m_pTrafficStats[x].value	= 0.0;
	}

	if ( m_nSelectedInterface >= 0 )
	{
		//
		//	获取指定网卡的流量 
		//
		m_cTrafficClass.GetTraffic( m_nSelectedInterface );
		m_cTrafficClass.GetTraffic( m_nSelectedInterface );
	}
	else if ( m_nInterfaceCount > 0 )
	{
		//
		//	获取所有网卡的流量
		//
		for ( i = 0; i < m_nInterfaceCount; i ++ )
		{
			m_pcTrafficClassAll[ i ].GetTraffic( i );
			m_pcTrafficClassAll[ i ].GetTraffic( i );
		}
	}


	m_dbMaxTrafficAmount = 0.0;

	SetTimer( GRIDTIMER, m_nGridUpdateSpeed, 0 );
	SetTimer( NETTIMER, m_nNetUpdateSpeed, 0 );
}

void CVwNetTrafficButton::SetFigureWaveColor( COLORREF clrColor )
{
	m_clrWave	= clrColor;

	m_bBrushInitalized = FALSE;

	Invalidate();
}

void CVwNetTrafficButton::SetFigureTextColor( COLORREF clrColor )
{
	m_clrText	= clrColor;
	Invalidate();
}

void CVwNetTrafficButton::SetDrawGlassStyle( BOOL bDrawGlassStyle )
{
	m_bDrawGlassStyle = bDrawGlassStyle;
}

void CVwNetTrafficButton::SetFigureBgColor( COLORREF clrColor )
{
	m_clrBackGround	= clrColor;
	Invalidate();
}

void CVwNetTrafficButton::SetUpdateSpeed(UINT netspeed, UINT gridspeed)
{
	m_nGridUpdateSpeed	= gridspeed;
	m_nNetUpdateSpeed	= netspeed;

	KillTimer( GRIDTIMER );
	KillTimer( NETTIMER );

	SetTimer( GRIDTIMER, m_nGridUpdateSpeed, 0 );
	SetTimer( NETTIMER, m_nNetUpdateSpeed, 0 );
}



/*
	React calling the callback function
*/
void CVwNetTrafficButton::InterfaceHasChanged()
{
	if ( m_callbackFunc_interface )
	{
		m_callbackFunc_interface( m_nSelectedInterface );
	}
}

/*
	Is someone wants to be informed, he has to tell us so
*/
void CVwNetTrafficButton::SetInterfaceNumberNotificationFunction(INTERFACECHANCEDPROC callfunct)
{
	m_callbackFunc_interface = callfunct;
}

void CVwNetTrafficButton::SelectTrafficChangedNotificationFunction( TRAFFICCHANGEDPROC callfunct )
{
	m_callbackFunc_traffic = callfunct;
}

/**
 *	Which kind of traffic do you want to monitor?
 */
void CVwNetTrafficButton::SelectTrafficType( int trafficType )
{
	INT i			= 0;
	INT nClsTrafficType	= CVwNetTraffic::ENUM_ALLTRAFFIC;

	switch( trafficType )
	{
	case ENUM_TRAFFIC_INCOMING:
		{
			nClsTrafficType	= CVwNetTraffic::ENUM_INCOMINGTRAFFIC;
		}
		break;

	case ENUM_TRAFFIC_OUTGOING:
		{
			nClsTrafficType	= CVwNetTraffic::ENUM_OUTGOINGTRAFFIC;
		}
		break;

	case ENUM_TRAFFIC_TOTAL:
		{
			nClsTrafficType	= CVwNetTraffic::ENUM_ALLTRAFFIC;
		}
		break;
	default:
		{
			nClsTrafficType	= CVwNetTraffic::ENUM_ALLTRAFFIC;
		}
	}

	//
	//	根据情况选择适当的 TrafficType
	//
	if ( m_nSelectedInterface >= 0 )
	{
		//	指定网卡的流量 
		m_cTrafficClass.SetTrafficType( nClsTrafficType );
	}
	else if ( m_nInterfaceCount > 0 )
	{
		//	所有网卡的流量
		for ( i = 0; i < m_nInterfaceCount; i ++ )
		{
			m_pcTrafficClassAll[ i ].SetTrafficType( nClsTrafficType );
		}
	}
}

/**
 *	Which interface do you want to monitor
 */
void CVwNetTrafficButton::SetInterfaceNumber( int interfacenumber )
{
	m_nSelectedInterface = interfacenumber;
	ReInit( m_nSelectedInterface );
}

/*
	Sound important, isnt it?
	Decides, wether to scale the graph using the total maximum traffic amount or the current maximum
*/
void CVwNetTrafficButton::SetAdaptiveScaling(BOOL adaptive)
{
	m_bUseAdaptiveScale = adaptive;
}

