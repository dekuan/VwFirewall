#ifndef SORTHEADERCTRL_H
#define SORTHEADERCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSortHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CSortHeaderCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortHeaderCtrl)
	public:
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortHeaderCtrl();

	void SetSortArrow( const int iColumn, const BOOL bAscending );

	// Generated message map functions
protected:
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

	int m_iSortColumn;
	BOOL m_bSortAscending;

	//{{AFX_MSG(CSortHeaderCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // SORTHEADERCTRL_H
