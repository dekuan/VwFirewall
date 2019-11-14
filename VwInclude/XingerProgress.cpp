/* --------------------------------------------------------------------------------

	XingerProgress.cpp : implementation file
	作者：刘其星
	时间：2003年6月15日 于北京昌平

 --------------------------------------------------------------------------------*/


#include "stdafx.h"

#include "XingerProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// XingerProgress

XingerProgress::XingerProgress()
{
	m_nyTopSpaceLines	= 6;		//	设置默认的头部空格为 3 行
	m_bShowTitle		= TRUE;		//	设置默认显示标题
	m_bShowText		= TRUE;		//	是否显示文字
	m_nLayerHeight		= 2;		//	每层的高度
	m_nMaxSidestepCount	= 10;		//	总共画多少个台阶
	m_bOwndrawBorder	= FALSE;	//	默认使用系统的边框
	m_bDrawGlassStyle	= FALSE;	//	是否画玻璃效果

	m_nxSpace		= 0;


	m_crBakColorx		= RGB(0,0,0);	// 默认背景颜色
	m_crShadowColorx	= RGB(0,128,0);	// 默认阴影颜色
	m_crFaceColorx		= RGB(0,255,0);	// 默认文字颜色是绿色
}


XingerProgress::~XingerProgress()
{
}


BEGIN_MESSAGE_MAP(XingerProgress, CProgressCtrl)
	//{{AFX_MSG_MAP(XingerProgress)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL XingerProgress::PreTranslateMessage(MSG* pMsg) 
{
	if ( IsWindowVisible() && m_ToolTip.m_hWnd )
	{
		m_ToolTip.RelayEvent( pMsg );
	}

//	if ( m_bFlat )
//	{
//		ModifyStyleEx( WS_EX_STATICEDGE, 0 );
//	}

	return CProgressCtrl::PreTranslateMessage( pMsg );
}

/////////////////////////////////////////////////////////////////////////////
// XingerProgress message handlers


void XingerProgress::OnPaint()
{
	COLORREF crShadow;
	COLORREF crFace;


	try
	{
		CPaintDC dc(this);	// 进度条的上下文环境句柄
		INT nPos = GetPos();

		if ( nPos < 0 )
			nPos = 0;
		else if ( nPos > 100 )
			nPos = 100;

		if ( nPos > 95 && nPos <= 100 )
		{
			crShadow	= RGB(0xFF,0x00,0x00);
			crFace		= RGB(0xFF,0x00,0x00);
		}
		else if ( nPos > 80 && nPos <= 95 )
		{
			crShadow	= RGB(0xFF,0x99,0x00);
			crFace		= RGB(0xFF,0x99,0x00);
		}
		else
		{
			crShadow	= m_crShadowColorx;
			crFace		= m_crFaceColorx;
		}

		CRect rect;
		INT cxClientWidth	= 0;
		INT ixSplit		= 0;
		INT iXstart1		= 0;
		INT iXend1		= 0;
		INT iXstart2		= 0;
		INT iXend2		= 0;
		INT nySidestepPositive	= 0;
		INT nySidestepReverse	= 0;

		HPEN hPen1		= NULL;
		HPEN hPen2		= NULL;
		HGDIOBJ hOriginal	= NULL;
		INT i			= 0;
		INT j			= 0;
		INT ixSpaceLeft		= 0;
		INT iyLines		= 0;
		INT nySidestep		= 0;	//	台阶数
		INT nYInnerHeight;

		hPen1		= CreatePen( PS_SOLID, 0, crShadow );	// 创建绘制阴影条的画笔
		hPen2		= CreatePen( PS_SOLID, 0, crFace );	// 创建绘制进度条的画笔

		GetClientRect( &rect );
		dc.FillSolidRect( &rect, m_crBakColorx );			// 填充矩形为黑色背景

		//
		//	画玻璃效果
		//
		if ( m_bDrawGlassStyle )
		{
			DrawGlass( dc.GetSafeHdc(), &rect );
		}


		cxClientWidth	= rect.Width();
		ixSplit		= cxClientWidth / 4;

		if ( 0 == m_nxSpace )
		{
			//
			//	左边图形起始于结束的 X 位置
			//
			iXstart1	= ixSplit;
			iXend1		= ixSplit * 2;

			//
			//	右边图形起始与结束的 X 位置
			//
			iXstart2	= ixSplit * 2 + 1;
			iXend2		= ixSplit * 3 + 1;
		}
		else
		{
			//
			//	左边图形起始于结束的 X 位置
			//
			iXstart1	= m_nxSpace;
			iXend1		= ixSplit * 2;
			
			//
			//	右边图形起始与结束的 X 位置
			//
			iXstart2	= ixSplit * 2 + 1;
			iXend2		= cxClientWidth - m_nxSpace;
		}	

		//
		//	例如 nPos = 66
		//	台阶应该是
		//	----------------------------------------
		//	100	10
		//	66	x
		//	x = 10 * 66 / 100
		//	  = 66 / 10
		//	  = 6
		//
		//nySidestepPositive = nPos / 10;		// 0，1，2，3，4，5，6，7，8，9
		nySidestepPositive = ( m_nMaxSidestepCount * nPos ) / 100;
		if ( 0 != nPos )
		{
			nySidestepPositive += 1;	// 如果是 0 % 则不显示任何进度，nPos>1 and nPos<=10 显示一格
		}
		nySidestepReverse	= m_nMaxSidestepCount - nySidestepPositive;	// 相反的值 0，1，2，3，4，5，6，7，8，9

		//
		//	根据最大台阶数，计算相反的台阶数
		//


		iyLines		= 0;
		nySidestep	= 0;
		nYInnerHeight	= m_nLayerHeight * m_nMaxSidestepCount + ( m_nMaxSidestepCount - 1 );
		for ( i = m_nyTopSpaceLines; i < m_nyTopSpaceLines + nYInnerHeight; i++ )
		{
			//	2*10+9=29 控制 y 坐标

			if ( iyLines == ( nySidestep * ( m_nLayerHeight + 1 ) + m_nLayerHeight ) )
			{
				//
				//	画完每一个台阶，让台阶数加 1
				//	一个台阶是 2 行
				//
				nySidestep ++;

				//	...
				i ++;
				iyLines ++;
			}

			if ( nySidestep >= nySidestepReverse )
			{
				//
				//	画亮的实线(每次一行)
				//	Saving the original object
				//
				hOriginal = SelectObject( dc, hPen2 );
				
				if ( 1 == m_nLayerHeight )
				{
					MoveToEx( dc, iXstart1, i, NULL );	// 画第一道线
					LineTo( dc, iXend1 - 0, i );
					MoveToEx( dc, iXstart2, i, NULL );	// 画第二道线
					LineTo( dc, iXend2 - 0, i );
				}
				else
				{
					MoveToEx( dc, iXstart1, i, NULL );	// 画第一道线
					LineTo( dc, iXend1, i );
					MoveToEx( dc, iXstart2, i, NULL );	// 画第二道线
					LineTo( dc, iXend2, i );
				}
				
				//	Restoring the original object
				SelectObject( dc, hOriginal );
			}
			else
			{
				//
				//	画虚线(每次一行)
				//
				if ( 1 == m_nLayerHeight || 0 == iyLines % 3 )
				{
					ixSpaceLeft = 0;
				}
				else
				{
					ixSpaceLeft = 1;
				}

				//	Saving the original object
				hOriginal = SelectObject( dc, hPen1 );

				for ( j = iXstart1 + ixSpaceLeft; j < iXend1; j+=2 )
				{
					//	画点横条
					MoveToEx( dc, j, i, NULL );		// 画第一道线
					LineTo( dc, j+1, i );
				}
				for ( j = iXstart2 + ixSpaceLeft; j<iXend2; j+=2 )
				{
					//	画点横条
					MoveToEx( dc, j, i, NULL );		// 画第二道线
					LineTo( dc, j+1, i );
				}

				//	Restoring the original object
				SelectObject( dc, hOriginal );
			}

			//
			//	纵行加 1
			//
			iyLines ++;
		}


		//	动态设置进度条标题，在进度条上输出文本
		if ( m_bShowText )
		{
			CRect rt;
			GetClientRect( &rt );					// 获得设备区域大小
			dc.SetBkMode( TRANSPARENT );				// 文本输出方式为透明
			dc.SetTextColor(crFace);				// 文本颜色，默认色为绿色

			TCHAR szBuffer[100]	= {0};		// 定义标题文字缓冲
			INT cxChar	= 0;			// 字体宽，标题长度
			INT iLength	= 0;
			TEXTMETRIC tm;				// 系统字体信息结构体

			GetTextMetrics( dc, &tm );		// 获取系统字体信息
			cxChar = tm.tmAveCharWidth;		// 获取系统字体的宽度

			iLength = _sntprintf( szBuffer, sizeof(szBuffer)-sizeof(TCHAR), _T("%d%%"), nPos );
			TextOut( dc,
				(rt.Width() - iLength * cxChar -6) / 2,
				m_nyTopSpaceLines + 30, szBuffer, iLength );
		}

		//	...
		if ( hPen1 )
		{
			DeleteObject( hPen1 );
			hPen1 = NULL;
		}
		if ( hPen2 )
		{
			DeleteObject( hPen2 );
			hPen2 = NULL;
		}

		//
		//	画边框
		//
		if ( m_bOwndrawBorder )
		{
			CDC* pWinDC = GetWindowDC();
			if ( pWinDC )
			{
				DrawBorder( pWinDC );
				ReleaseDC( pWinDC );
			}
		}
	}
	catch (...)
	{
	}
}



int XingerProgress::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CProgressCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void XingerProgress::SetOwndrawBorder( BOOL bOwndrawBorder, COLORREF crBorder )
{
	m_bOwndrawBorder = bOwndrawBorder;
	m_crBorder = crBorder;
}


void XingerProgress::SetTopSpaceLines(int iyLines=5)
{
	m_nyTopSpaceLines	= iyLines;
}


void XingerProgress::SetColorConfig(COLORREF crBakClr=RGB(0,0,0), COLORREF crShadowClr=RGB(0,255,0), COLORREF crFaceClr=RGB(0,128,0))
{
	m_crBakColorx		= crBakClr;
	m_crFaceColorx		= crFaceClr;
	m_crShadowColorx	= crShadowClr;
}

void XingerProgress::SetDrawGlassStyle( BOOL bDrawGlassStyle )
{
	m_bDrawGlassStyle = bDrawGlassStyle;
}

void XingerProgress::InitToolTip()
{
	if ( m_ToolTip.m_hWnd == NULL )
	{
		// Create ToolTip control
		m_ToolTip.Create(this);

		// Create inactive
		m_ToolTip.Activate(TRUE);

		// Enable multiline
		m_ToolTip.SendMessage( TTM_SETMAXTIPWIDTH, 0, 400 );
	}
}


void XingerProgress::SetTooltipText(LPCTSTR lpszText)
{
	// We cannot accept NULL pointer
	if ( lpszText == NULL )
		return;
	
	// Initialize ToolTip
	InitToolTip();

	//	If there is no tooltip defined then add it
	if ( m_ToolTip.GetToolCount() == 0 )
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool( this, lpszText, rectBtn, 1);
	}

	//	Set text for tooltip
	m_ToolTip.UpdateTipText( lpszText, this, 1 );
	m_ToolTip.Activate(TRUE);
}

void XingerProgress::DrawBorder(CDC *pDC)
{
	if (m_hWnd )
	{
		CBrush Brush( m_crBorder );
		CBrush* pOldBrush = pDC->SelectObject(&Brush);

		CRect rtWnd;
		GetWindowRect(&rtWnd); 
		
		CPoint point;

		//	填充顶部框架
		point.x = rtWnd.Width();
		point.y = 1;	//	GetSystemMetrics(SM_CYFRAME)+1;
		pDC->PatBlt(0, 0, point.x, point.y, PATCOPY);
		
		//填充左侧框架
		point.x = 1;	//	GetSystemMetrics(SM_CXFRAME);
		point.y = rtWnd.Height();
		pDC->PatBlt(0, 0, point.x, point.y, PATCOPY);
		
		//填充底部框架
		point.x = rtWnd.Width(); 
		point.y = 1;	//GetSystemMetrics(SM_CYFRAME) + 1;
		pDC->PatBlt(0, rtWnd.Height()-point.y, point.x, point.y, PATCOPY);
		
		//填充右侧框架
		point.x = 1;	//GetSystemMetrics(SM_CXFRAME);
		point.y = rtWnd.Height();
		pDC->PatBlt(rtWnd.Width()-point.x, 0, point.x, point.y, PATCOPY);
	}
}

VOID XingerProgress::DrawGlass( HDC hDC, CRect rect )
{
	CRect rectGradient;
	HPEN hPen1		= NULL;

	hPen1 = CreatePen( PS_SOLID, 0, RGB( 0xBE, 0xBE, 0xBE ) );

	//
	//	画顶部直线
	//
	SelectObject( hDC, hPen1 );

	MoveToEx( hDC, rect.left + 1, rect.top + 1, NULL );
	LineTo( hDC, rect.right-1, rect.top + 1 );

	if ( hPen1 )
	{
		DeleteObject( hPen1 );
		hPen1 = NULL;
	}

	//
	//	画左右渐变线
	//
	rectGradient = rect;
	rectGradient.top	+= 1;
	rectGradient.left	+= 1;
	rectGradient.right	= rectGradient.left + 1;
	rectGradient.bottom	/= 2;
	DrawGradient( hDC, &rectGradient, RGB( 0xBE, 0xBE, 0xBE ), RGB( 0x00, 0x00, 0x00 ), 1 );

	rectGradient = rect;
	rectGradient.top	+= 1;
	rectGradient.left	= rectGradient.right - 3;
	rectGradient.right	-= 1;
	rectGradient.bottom	/= 2;
	DrawGradient( hDC, &rectGradient, RGB( 0xBE, 0xBE, 0xBE ), RGB( 0x00, 0x00, 0x00 ), 1 );

	//
	//	画主渐变区域
	//
	rectGradient = rect;
	rectGradient.top	+= 2;
	rectGradient.left	+= 2;
	rectGradient.right	-= 2;
	rectGradient.bottom	/= 2;

	DrawGradient( hDC, &rectGradient, RGB( 0x80, 0x80, 0x80 ), RGB( 0x00, 0x00, 0x00 ), 1 );

}

/**
 *	@ Private
 *	绘制渐变窗口
 */
VOID XingerProgress::DrawGradient( HDC hDC, CRect rect, COLORREF StartColor, COLORREF EndColor, long Direction )
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
		
		if ( brush )
		{
			DeleteObject( brush );
			brush = NULL;
		}
	}
}