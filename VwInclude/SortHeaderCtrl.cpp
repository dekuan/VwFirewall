#include "stdafx.h"
#include "SortHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSortHeaderCtrl::CSortHeaderCtrl()
	: m_iSortColumn( -1 )
	, m_bSortAscending( TRUE )
{
}

CSortHeaderCtrl::~CSortHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CSortHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortHeaderCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl message handlers

void CSortHeaderCtrl::SetSortArrow( const int iSortColumn, const BOOL bSortAscending )
{
	m_iSortColumn = iSortColumn;
	m_bSortAscending = bSortAscending;

	// change the item to owner drawn.
	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;
	VERIFY( GetItem( iSortColumn, &hditem ) );
	hditem.fmt |= HDF_OWNERDRAW;
	VERIFY( SetItem( iSortColumn, &hditem ) );

	// invalidate the header control so it gets redrawn
	Invalidate();
}


void CSortHeaderCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	// attath to the device context.
	CDC dc;
	VERIFY( dc.Attach( lpDrawItemStruct->hDC ) );

	// save the device context.
	const int iSavedDC = dc.SaveDC();

	// get the column rect.
	CRect rc( lpDrawItemStruct->rcItem );

	// set the clipping region to limit drawing within the column.
	CRgn rgn;
	VERIFY( rgn.CreateRectRgnIndirect( &rc ) );
	(void)dc.SelectObject( &rgn );
	VERIFY( rgn.DeleteObject() );

	// draw the background,
	CBrush brush( GetSysColor( COLOR_3DFACE ) );
	dc.FillRect( rc, &brush );

	// get the column text and format.
	TCHAR szText[ 256 ];
	HD_ITEM hditem;

	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = szText;
	hditem.cchTextMax = 255;

	VERIFY( GetItem( lpDrawItemStruct->itemID, &hditem ) );

	// determine the format for drawing the column label.
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS ;

	if( hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if( hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;

	// adjust the rect if the mouse button is pressed on it.
	if( lpDrawItemStruct->itemState == ODS_SELECTED )
	{
		rc.left++;
		rc.top += 2;
		rc.right++;
	}

	CRect rcIcon( lpDrawItemStruct->rcItem );
	const int iOffset = ( rcIcon.bottom - rcIcon.top ) / 4;

	// adjust the rect further if the sort arrow is to be displayed.
	if( lpDrawItemStruct->itemID == (UINT)m_iSortColumn )
		rc.right -= 3 * iOffset;

	rc.left += iOffset;
	rc.right -= iOffset;

	// draw the column label.
	if( rc.left < rc.right )
		(void)dc.DrawText( szText, -1, rc, uFormat );

	// draw the sort arrow.
	if( lpDrawItemStruct->itemID == (UINT)m_iSortColumn )
	{
		// set up the pens to use for drawing the arrow.
		CPen penLight( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );
		CPen penShadow( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );
		CPen* pOldPen = dc.SelectObject( &penLight );

		if( m_bSortAscending )
		{
			// draw the arrow pointing upwards.
			dc.MoveTo( rcIcon.right - 2 * iOffset, iOffset);
			dc.LineTo( rcIcon.right - iOffset, rcIcon.bottom - iOffset - 1 );
			dc.LineTo( rcIcon.right - 3 * iOffset - 2, rcIcon.bottom - iOffset - 1 );
			(void)dc.SelectObject( &penShadow );
			dc.MoveTo( rcIcon.right - 3 * iOffset - 1, rcIcon.bottom - iOffset - 1 );
			dc.LineTo( rcIcon.right - 2 * iOffset, iOffset - 1);		
		}
		else
		{
			// draw the arrow pointing downwards.
			dc.MoveTo( rcIcon.right - iOffset - 1, iOffset );
			dc.LineTo( rcIcon.right - 2 * iOffset - 1, rcIcon.bottom - iOffset );
			(void)dc.SelectObject( &penShadow );
			dc.MoveTo( rcIcon.right - 2 * iOffset - 2, rcIcon.bottom - iOffset );
			dc.LineTo( rcIcon.right - 3 * iOffset - 1, iOffset );
			dc.LineTo( rcIcon.right - iOffset - 1, iOffset );		
		}

		// restore the pen.
		(void)dc.SelectObject( pOldPen );
	}

	// restore the previous device context.
	VERIFY( dc.RestoreDC( iSavedDC ) );

	// detach the device context before returning.
	(void)dc.Detach();
}


void CSortHeaderCtrl::Serialize( CArchive& ar )
{
	if( ar.IsStoring() )
	{
		const int iItemCount = GetItemCount();
		if( iItemCount != -1 )
		{
			ar << iItemCount;

			HD_ITEM hdItem = { 0 };
			hdItem.mask = HDI_WIDTH;

			for( int i = 0; i < iItemCount; i++ )
			{
				VERIFY( GetItem( i, &hdItem ) );
				ar << hdItem.cxy;
			}
		}
	}
	else
	{
		int iItemCount;
		ar >> iItemCount;
		
		if( GetItemCount() != iItemCount )
			TRACE0( _T("Different number of columns in registry.") );
		else
		{
			HD_ITEM hdItem = { 0 };
			hdItem.mask = HDI_WIDTH;

			for( int i = 0; i < iItemCount; i++ )
			{
				ar >> hdItem.cxy;
				VERIFY( SetItem( i, &hdItem ) );
			}
		}
	}
}
