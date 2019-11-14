/*****************************************************************************
 *
 * Autor:			Joachim Raidl
 * Adresse:			Joachim.Raidl@iname.com
 * Klasse:			XPaneInfo, XStatusBar
 * JRRC:			%J%
 * Inlines:			SetFont(), SetFontSize(), SetFontName(), Increment(), 
 * (XPaneInfo)		Decrement(), GetFgColor(), GetBkColor(), GetText(), 
 *					GetBitmap(), GetNumber(), GetProgressCtrl(), 
 *					GetFont(), GetFontName(), GetFontSize(), GetMode(), 
 *					IsHScroll(), IsVScroll(), SetRange(), SetPos(), 
 *					OffsetPos(), SetStep(), StepIt(), 
 * Inlines:			GetProgressCtrl(), SetFgColor(), SetBkColor(), 
 * (XStatusBar)		SetBitmap(), SetText(), SetNumber(), SetFont(), 
 *					SetFontSize(), SetFontName(), SetMode(), SetRange(), 
 *					EnablePane(), CheckKey(), Increment(), Decrement(), 
 *					SetPos(), OffsetPos(), SetStep(), StepIt(), 
 *					GetFgColor(), GetBkColor(), GetBitmap(), GetText(), 
 *					GetNumber(), GetFont(), GetFontName(), GetFontSize(), 
 *					GetMode(), IsHScroll(), IsVScroll(), _XPI(), 
 *
 ****************************************************************************/

#ifndef __XSTATUSBAR__
#define __XSTATUSBAR__

#include <afxtempl.h>

#define XSB_TOP				DT_TOP		// Styles werden aus DrawText übernommen.
#define XSB_LEFT			DT_LEFT		// Theoretisch können auch alle anderen
#define XSB_CENTER			DT_CENTER	// Format-Styles aus Draw-Text benutzt
#define XSB_RIGHT			DT_RIGHT	// werden. Diese werden direkt an die
#define XSB_VCENTER			DT_VCENTER	// jeweiligen Panes übergeben. Für Bitmaps
#define XSB_BOTTOM			DT_BOTTOM	// werden jedoch nur diese Styles benutzt.

#define XSB_LEER			0x00000000
#define XSB_TEXT			0x00100000	// Darstellung eines Textes im Pane
#define XSB_NUMBER			0x00200000	// Darstellung eines Zahlenwertes im Pane
#define XSB_BITMAP			0x00400000	// Darstellung eines Bitmaps im Pane
#define XSB_PROGRESS		0x00800000	// Darstellung eines Progress-Bars im Pane
#define XSB_HSCROLL			0x01000000	// Horizontales Scrollen des Textes bzw. Bitmaps
#define XSB_VSCROLL			0x02000000	// Vertikales Scrollen des Textes bzw. Bitmaps
#define XSB_DSCROLL			0x03000000	// Diagonales Scrollen des Textes bzw. Bitmaps
#define XSB_REPEAT			0x10000000	// Mehrfaches hinter- bzw. untereinandersetzen
#define XSB_STRETCH			0x20000000	// Bitmap an die Panegröße anpassen
#define XSB_SMOOTH			0x40000000	// Progress-Bar smooth darstellen

#define XSB_ALIGN			0x000fffff
#define XSB_MODE			0x00f00000
#define XSB_SCROLL			0x0f000000
#define XSB_MISC			0xf0000000

class XPaneInfo : public CObject
{
protected:				// 0 = aus, 1 = ein
	COLORREF fgColor[2];		// Textfarbe
	COLORREF bkColor[2];		// Hintergrundfarbe
	CString	 string[2];		// Text, Nummer oder Bitmapname
	CString  strUrl;		// url
	CProgressCtrl * progress;	// Fortschrittskontrolle

	LOGFONT font;			// Schriftart
	int mode;			// Darstellungsart
	int hScrollPos;			// Position für horizontales Scrollen
	int vScrollPos;			// Position für vertikales Scrollen

// Funktionen
public:
	XPaneInfo();
	XPaneInfo( const XPaneInfo & paneInfo );
	~XPaneInfo();
	XPaneInfo operator = ( const XPaneInfo & paneInfo );
	void SetDefault();

	void HScroll(CRect& rect, int maxWidth, int nullValue);
	void VScroll(CRect& rect, int maxHeight, int nullValue);

// Zugriffsfunktionen
public:
	void SetFgColor(COLORREF newOnColor, COLORREF newOffColor = -1);
	void SetBkColor(COLORREF newOnColor, COLORREF newOffColor = -1);
	void SetBitmap(LPCSTR newOnBitmap, LPCSTR newOffBitmap = "");
	void SetText(LPCSTR onText, LPCSTR offText = "");
	void SetNumber(int onValue, int offValue = 0);
	void SetMode(int newMode);
	void SetFont(LOGFONT& newFont)
	{
		font	= newFont;
	}
	void SetFont(CFont& newFont)
	{
		newFont.GetLogFont(&font);
	}
	void SetFont(CFont *newFont)
	{
		newFont->GetLogFont(&font);
	}
	void SetFontSize(int size)
	{
		font.lfHeight = size;
	}
	void SetFontName(LPCSTR name)
	{
		strcpy(font.lfFaceName, name);
	}
	void SetFont(LPCSTR name, int size);
	
	void SetUrl( LPCSTR lpcszUrl );
	CString GetUrl();

	int Increment(bool on = true)
	{
		int n = atoi(string[on ? 1 : 0]); 
		string[on ? 1 : 0].Format("%d", ++n);
		return n;
	}
	int Decrement(bool on = true)
	{
		int n = atoi(string[on ? 1 : 0]);
		string[on ? 1 : 0].Format("%d", --n);
		return n;
	}
	COLORREF GetFgColor(bool on = true)
	{
		return fgColor[on ? 1 : 0];
	}
	COLORREF GetBkColor(bool on = true)
	{
		return bkColor[on ? 1 : 0];
	}
	CString GetText(bool on = true)
	{
		return string[ on ? 1 : 0 ];
	}
	CString GetBitmap(bool on = true)
	{
		return string[on ? 1 : 0];
	}
	int GetNumber(bool on = true)
	{
		return atoi(string[on ? 1 : 0]);
	}

	CProgressCtrl * GetProgressCtrl()
	{
		return progress;
	}

	LOGFONT & GetFont()
	{
		return font;
	}
	CString GetFontName()
	{
		return font.lfFaceName;
	}
	int GetFontSize()
	{
		return font.lfHeight;
	}
	int GetMode()
	{
		return mode;
	}
	bool IsHScroll()
	{
		return ((mode & XSB_HSCROLL) != 0);
	}
	bool IsVScroll()
	{
		return ((mode & XSB_VSCROLL) != 0);
	}

	void SetRange(int nLower, int nUpper)
	{
		if (progress)
			progress->SetRange(nLower, nUpper);
	}
	int SetPos(int nPos)
	{
		return (progress ? progress->SetPos(nPos) : -1);
	}
	int OffsetPos(int nPos)
	{
		return (progress ? progress->OffsetPos(nPos) : -1);
	}
	int SetStep(int nStep)
	{
		return (progress ? progress->SetStep(nStep) : -1);
	}
	int StepIt()
	{
		return (progress ? progress->StepIt() : -1);
	}

#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
	void AssertValid() const;
	friend CDumpContext& AFXAPI operator<<(CDumpContext& dc, XPaneInfo& b);
#endif //_DEBUG
};

//****************************************************************************
//****************************************************************************
//****************************************************************************

class XStatusBar : public CStatusBar
{
public:
	static XStatusBar	*aktBar;

// Konstruktion
public:
	XStatusBar();
	bool CreateStatusBar(CWnd *pParent, const UINT* lpIDArray, int nIDCount, UINT pane0Style = SBPS_STRETCH);

	XPaneInfo & GetXPaneInfo(int ix);
	CProgressCtrl * GetProgressCtrl(int ix)
	{
		return GetXPaneInfo(ix).GetProgressCtrl();
	}
	void SavePane(int ix);
	void RestorePane(int ix);
	int GetPaneAtPosition(CPoint& point);

// Durchgriff auf die XPaneInfo-Members
public:
	void SetFgColor( int ix, COLORREF on, COLORREF off = -1 )
	{
		_XPI(ix).SetFgColor(on, off);
		Invalidate(FALSE);
	}
	void SetBkColor( int ix, COLORREF on, COLORREF off = -1 )
	{
		_XPI(ix).SetBkColor(on, off);
		Invalidate(FALSE);
	}
	void SetBitmap( int ix, LPCSTR on, LPCSTR off = "" )
	{
		_XPI(ix).SetBitmap(on, off);
		Invalidate(FALSE);
	}
	void SetText(int ix, LPCSTR on, LPCSTR off = "" )
	{
		_XPI(ix).SetText( on, off );
		Invalidate( FALSE );
	}
	void SetNumber( int ix, int on, int off = 0 )
	{
		_XPI(ix).SetNumber(on, off);
		Invalidate(FALSE);
	}
	void SetFont(int ix, LOGFONT& newFont)
	{
		_XPI(ix).SetFont(newFont);
		Invalidate(FALSE);
	}
	void SetFont(int ix, CFont& newFont)
	{
		_XPI(ix).SetFont(newFont);
		Invalidate(FALSE);
	}
	void SetFont( int ix, CFont *newFont )
	{
		_XPI(ix).SetFont(newFont);
		Invalidate(FALSE);
	}
	void SetFont( int ix, LPCSTR name, int size )
	{
		_XPI(ix).SetFont(name, size);
		Invalidate(FALSE);
	}
	void SetFontSize(int ix, int size)
	{
		_XPI(ix).SetFontSize(size);
		Invalidate(FALSE);
	}
	void SetFontName(int ix, LPCSTR name)
	{
		_XPI(ix).SetFontName(name);
		Invalidate(FALSE);
	}
	void SetUrl(int ix, LPCSTR lpcszUrl)
	{
		_XPI(ix).SetUrl(lpcszUrl);
		Invalidate(FALSE);
	}
	void SetMode(int ix, int newMode)
	{
		_XPI(ix).SetMode(newMode);
		Invalidate(FALSE);
	}
	void SetRange(int ix, int nLow, int nUp)
	{
		_XPI(ix).SetRange(nLow, nUp);
		Invalidate(FALSE);
	}

	void EnablePane(int ix, bool enable = true)
	{
		SetPaneStyle(ix, enable ? 0 : SBPS_DISABLED);
	}
	void CheckKey(int ix, int nVirtKey)
	{
		EnablePane(ix, ::GetKeyState(nVirtKey) & 1);
	}

	int  Increment(int ix, bool on = true)
	{
		int ret = _XPI(ix).Increment(on);
		Invalidate(FALSE);
		return ret;
	}
	int  Decrement(int ix, bool on = true)
	{
		int ret = _XPI(ix).Decrement(on);
		Invalidate(FALSE);
		return ret;
	}
	int SetPos(int ix, int nPos)
	{
		return _XPI(ix).SetPos(nPos);
	}
	int OffsetPos(int ix, int nPos)
	{
		return _XPI(ix).OffsetPos(nPos);
	}
	int SetStep(int ix, int nStep)
	{
		return _XPI(ix).SetStep(nStep);
	}
	int StepIt(int ix)
	{
		return _XPI(ix).StepIt();
	}
	UINT SetStyle(int ix, UINT style);
	int  SetWidth(int ix, int width);

	COLORREF GetFgColor(int ix, bool on = true)
	{
		return _XPI(ix).GetFgColor(on);
	}
	COLORREF GetBkColor(int ix, bool on = true)
	{
		return _XPI(ix).GetBkColor(on);
	}
	CString	 GetBitmap(int ix, bool on = true)
	{
		return _XPI(ix).GetBitmap(on);
	}
	CString	GetText(int ix, bool on = true)
	{
		return _XPI(ix).GetText(on);
	}
	CString GetUrl( int ix )
	{
		return _XPI(ix).GetUrl();
	}
	int GetNumber(int ix, bool on = true)
	{
		return _XPI(ix).GetNumber(on);
	}

	LOGFONT & GetFont(int ix)
	{
		return _XPI(ix).GetFont();
	}
	CString GetFontName(int ix)
	{
		return _XPI(ix).GetFontName();
	}
	int GetFontSize(int ix)
	{
		return _XPI(ix).GetFontSize();
	}
	int GetMode(int ix)
	{
		return _XPI(ix).GetMode();
	}
	bool IsHScroll(int ix)
	{
		return _XPI(ix).IsHScroll();
	}
	bool IsVScroll(int ix)
	{
		return _XPI(ix).IsVScroll();
	}
	UINT GetStyle(int ix);
	int GetWidth(int ix);
	UINT GetID(int ix);

// Hilfsfunktionen
protected:
	XPaneInfo & _XPI( int ix )
	{
		return GetXPaneInfo( ix );
	}
	void DrawSizing(CDC *pDC);
	void DrawTextPane(CDC *pDC, int ix, CRect& rect, XPaneInfo& aktPane);
	void DrawBitmapPane(CDC *pDC, int ix, CRect& rect, XPaneInfo& aktPane);
	void DrawProgressPane(CDC *pDC, int ix, CRect& rect, XPaneInfo& aktPane);

// Attribute
public:
	CArray< XPaneInfo, XPaneInfo & > paneInfo;	// Liste mit den Zusatzinformationen
	CMap< int, int, XPaneInfo, XPaneInfo & > buffer;		// Zwischenpuffer für jeweils ein Pane
	UINT timerID;	// ID des Refresh-Timers
	bool on;			// Aktueller Pane aktiv oder inaktiv?
	CWnd * pParent;	// Vaterfenster der Statusbar

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(XStatusBar)
	virtual BOOL SetIndicators(const UINT* lpIDArray, int nIDCount);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~XStatusBar();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(XStatusBar)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // __XSTATUSBAR__
