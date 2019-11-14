// MfcFunc.cpp: implementation of the CMfcFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MfcFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/**
 *	获取粗体字
 */
BOOL mfcfunc_get_bold_fontstyle( HWND hDlg, LPCTSTR lpcszFontName, CFont * pcFont )
{
	//
	//	hDlg		- [in]
	//	lpcszFontName	- [in]
	//	pcFont		- [out]
	//
	//	使用示例：
	//		CButton * pRadioClose = (CButton*)GetDlgItem(IDC_RADIO_MAIN_CLOSE);
	//		pRadioClose->SetFont( &m_font );
	//
	if ( NULL == hDlg || NULL == lpcszFontName || NULL == pcFont )
	{
		return FALSE;
	}

	// ..
	int DESIGNX  =  150;
	int DESIGNY  =  150;

	CRect rect;
	float m_xScale;
	float m_yScale;
	LOGFONT lf;

	rect.SetRect( 0, 0, 100, 100 );
	::MapDialogRect( hDlg, rect );

	m_xScale  = (float)((float)rect.right /  (float) DESIGNX);
	m_yScale  = (float)((float)rect.bottom / (float) DESIGNY);

	memset( &lf, 0, sizeof(LOGFONT));	// Clear out structure.
	lf.lfHeight	= (INT)(12*m_yScale);	// Request a 20-pixel-high font
	lf.lfCharSet	= GB2312_CHARSET;
	lf.lfWeight	= FW_BOLD;
	_tcscpy( lf.lfFaceName, lpcszFontName );
	pcFont->CreateFontIndirect( &lf );

	return TRUE;
}

