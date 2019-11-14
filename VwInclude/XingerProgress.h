/* --------------------------------------------------------------------------------

	XingerProgress.h : header file
	作者：刘其星
	时间：2003年6月15日 于北京昌平

 --------------------------------------------------------------------------------*/



#ifndef __MY_PROGRESS_HEAD__
#define __MY_PROGRESS_HEAD__



#if _MSC_VER > 1000
#pragma once
#endif





class XingerProgress : public CProgressCtrl
{

public:
	XingerProgress();
	virtual ~XingerProgress();

public:
	BOOL m_bShowTitle;		//	是否显示标题
	BOOL m_bShowText;		//	是否显示文字
	BOOL m_bOwndrawBorder;		//	自画边框
	BOOL m_bDrawGlassStyle;		//	是否画玻璃效果
	INT  m_nLayerHeight;		//	每层的高度
	INT  m_nMaxSidestepCount;	//	总共画多少个台阶


	INT m_nyTopSpaceLines;	//	顶部空的行数
	INT m_nxSpace;

public:

	void InitToolTip();
	void SetTooltipText( LPCTSTR lpszText );
	void SetTopSpaceLines( int iyLines );
	void SetOwndrawBorder( BOOL bOwndrawBorder, COLORREF crBorder );
	void SetDrawGlassStyle( BOOL bDrawGlassStyle );
	void SetColorConfig( COLORREF crBakClr, COLORREF crShadowClr, COLORREF crFaceClr );

private:
	void DrawBorder( CDC * pDC );
	VOID DrawGlass( HDC hDC, CRect rect );
	VOID DrawGradient( HDC hDC, CRect rect, COLORREF StartColor, COLORREF EndColor, long Direction );

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	//{{AFX_MSG(XingerProgress)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()


private:
	COLORREF	m_crBorder;
	COLORREF	m_crBakColorx;		//	背景颜色
	COLORREF	m_crShadowColorx;	//	阴影颜色
	COLORREF	m_crFaceColorx;		//	亮文字颜色

	CToolTipCtrl	m_ToolTip;		//	Tooltip
};



#endif	//end of define __MY_PROGRESS_HEAD__
