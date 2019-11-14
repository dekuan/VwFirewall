#ifndef __XINGBALLOON_HEADER__
#define __XINGBALLOON_HEADER__


//////////////////////////////////////////////////////////////////////////
#define TRAYICON_CLASS _T("TrayIconClass")

#define UM_SHOWTASK		WM_USER + 0x8000
#define UM_SHOWBALLOON		WM_USER + 0x8001
#define UM_HIDEBALLOON		WM_USER + 0x8002



class CXingBalloon  
{
public:
	CXingBalloon();
	virtual ~CXingBalloon();


public:
	static LRESULT __stdcall BalloonProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	static CXingBalloon * m_pThis;
	UINT GetCallbackMessage() const;
	HWND GetSafeHwnd() const;

public:
	VOID SetTrayiconClassName( LPCTSTR lpcszClassName );
	BOOL CreateTrayIcon( HINSTANCE hInst, HWND hParentWnd,
			UINT uCallbackMessage, LPCTSTR pszToolTip, HICON hIcon, UINT uMenuID, BOOL bHidden );
	
	BOOL AddTrayIcon();
	BOOL RemoveTrayIcon();
	BOOL ModifyTrayIcon( HICON hIcon, TCHAR * pszTip );
	BOOL HideTrayIcon();
	BOOL ShowTrayIcon();

	BOOL ShowBalloon( LPCTSTR pszText, LPCTSTR pszTitle, DWORD dwIcon, UINT uTimeout );


private:
	VOID Initialise();
	ATOM RegisterClass( HINSTANCE hInstance );


public:
	TCHAR m_szError[ MAX_PATH ];


protected:
	NOTIFYICONDATA		m_tnd;
	HINSTANCE		m_hInstance;
	HWND			m_hWnd;
	TCHAR			m_szTrayiconClassName[ 64 ];
	
	BOOL			m_bEnabled;
	BOOL			m_bRemoved;
	BOOL			m_bHidden;
	BOOL			m_bShowIconPending;
	UINT			m_uCreationFlags;
};





#endif // __XINGBALLOON_HEADER__
