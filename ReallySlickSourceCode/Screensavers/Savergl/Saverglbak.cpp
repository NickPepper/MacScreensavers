// OpenGL screensaver skeleton


#include <windows.h>
#include <stdio.h>
#include <regstr.h>
#include "savergl.h"



const TCHAR szScreenSaverKey[] = REGSTR_PATH_SCREENSAVE;
TCHAR szPasswordActiveValue[] = REGSTR_VALUE_USESCRPASSWORD;
TCHAR szPwdDLL[] = TEXT("PASSWORD.CPL");
CHAR szFnName[] = "VerifyScreenSavePwd";
TCHAR szImmDLL[] = TEXT("IMM32.DLL");
CHAR szImmFnc[] = "ImmAssociateContext";

typedef BOOL (FAR PASCAL * VERIFYPWDPROC) (HWND);
typedef BOOL (FAR PASCAL * IMMASSOCPROC) (HWND,HIMC);


//----------------------------------------------------------------------------
// variables declared in Savergl.h
HINSTANCE hMainInstance = 0;
HWND hMainWindow = 0;
BOOL childPreview = FALSE;
BOOL reallyClose = FALSE;
BOOL doingPreview = 0;
int theVideoMode = 0;
int theVisualID = 0;
BOOL pfd_swap_exchange = FALSE;
BOOL pfd_swap_copy = FALSE;


//----------------------------------------------------------------------------
// other globals
POINT ptMouse;
BOOL closing = FALSE;
BOOL checkingPassword = FALSE;
HINSTANCE hInstPwdDLL = NULL;
VERIFYPWDPROC VerifyPassword = NULL;
HINSTANCE hInstImm = NULL;
IMMASSOCPROC ImmFnc = NULL;
HIMC hPrevImc = (HIMC)0L;


static BOOL onWin95 = FALSE;  // TRUE if on Chicago, FALSE if on Cairo

//----------------------------------------------------------------------------
DWORD dwWakeThreshold = 4;  // default to slight movement
DWORD dwPasswordDelay = 1;
DWORD dwBlankTime = 10000;  // ten second delay

//----------------------------------------------------------------------------
// forward declarations of internal fns
static int DoScreenSaver( HWND hParent );
static int DoConfigBox( HWND hParent );
static int DoSaverPreview( LPCTSTR szUINTHandle );
static int DoChangePw( LPCTSTR szUINTHandle );
static BOOL DoPasswordCheck( HWND hParent );
VOID LoadPwdDLL(VOID);
VOID UnloadPwdDLL(VOID);


// convert text to unsigned int
static UINT atoui(LPCTSTR szUINT){
	UINT uValue = 0;

	while((*szUINT >= TEXT('0')) && (*szUINT <= TEXT('9')))
		uValue = ((uValue * 10) + (*szUINT++ - TEXT('0')));

	return uValue;
}


//----------------------------------------------------------------------------
int WINAPI WinMainN(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR szCmdLine, int nCmdShow){
	LPCTSTR commandLine = szCmdLine;
	HWND hParent = 0;
	OSVERSIONINFO osvi;
	hMainInstance = hInst;

	osvi.dwOSVersionInfoSize = sizeof(osvi);
	onWin95 = (GetVersionEx(&osvi) && osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);

	while(1){
		switch(*commandLine){
		case TEXT(' '):
		case TEXT('-'):
		case TEXT('/'):
			commandLine++;   // skip spaces and common switch prefixes
			break;
		case TEXT('s'):
		case TEXT('S'):
			return DoScreenSaver(NULL);
		case TEXT('p'):
		case TEXT('P'):
			do commandLine++;
			while(*commandLine == TEXT(' '));  // skip to the good stuff
			return(DoSaverPreview(commandLine));
		case TEXT('a'):
		case TEXT('A'):
			if(!onWin95)
				return -1;
			do{
				commandLine++;
			}
			while(*commandLine == TEXT(' '));  // skip to the good stuff
			return(DoChangePw(commandLine));
		case TEXT('c'):
		case TEXT('C'):
			return(DoConfigBox(GetForegroundWindow()));
		case TEXT('\0'):
			return(DoConfigBox(NULL));
		default:
			return -1;
		}
	}

	return -1;
}


//----------------------------------------------------------------------------
// default screen-saver proc, declared in savergl.h
// intended to be called by the consumer's ScreenSaverProc where
// DefWindowProc would normally be called
LRESULT WINAPI DefScreenSaverProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	static int firstMouseMove = 0;  // Used for 3dfx driver fix

	if(!childPreview && !closing){
		switch(uMsg){
		case WM_CLOSE:
			// We need to explicitly tell this program to close.
			// NT will sometimes send a WM_CLOSE on its own.
			if(!reallyClose)
				return(0);
			//
			// Only do password check if on Windows 95.  WinNT (Cairo) has
			// the password check built into the security desktop for
			// C2 compliance.
			//
			if(onWin95){
				if(!DoPasswordCheck(hWnd)){
					GetCursorPos( &ptMouse );  // re-establish
					return FALSE;
				}
			}

			// If video mode is other than default display resolution
			// we need to set the display back to that default
			if(theVideoMode > 0){
				DestroyWindow(hWnd);
				ChangeDisplaySettings(NULL, 0);
			}
			break;

		/*case SCRM_VERIFYPW:
			if(onWin95)
				return (VerifyPassword ? (LRESULT)VerifyPassword(hWnd) : 1L);
			break;*/

		default:{
			POINT ptMove, ptCheck;

			if(checkingPassword)
				break;

			switch(uMsg){
			case WM_SHOWWINDOW:
				if(BOOL(wParam))
					SetCursor(NULL);
				break;

			case WM_SETCURSOR:
				SetCursor(NULL);
				return TRUE;

			case WM_MOUSEMOVE:
				// Capture first mouse movement and do nothing.
				// This prevents 3dfx driver from killing saver because
				// 3dfx driver sends extraneous WM_MOUSEMOVE message.
				if(!firstMouseMove)
					firstMouseMove = 1;
				else
					reallyClose = TRUE;
				GetCursorPos( &ptCheck );
				if((ptMove.x = ptCheck.x - ptMouse.x) && (ptMove.x < 0))
					ptMove.x *= -1;
				if((ptMove.y = ptCheck.y - ptMouse.y) && (ptMove.y < 0))
					ptMove.y *= -1;
				if(((DWORD)ptMove.x + (DWORD)ptMove.y) > dwWakeThreshold){
					PostMessage(hWnd, WM_CLOSE, 0, 0l);
					ptMouse = ptCheck;
				}
				break;

			case WM_ACTIVATEAPP:
				if(wParam)
					break;
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				reallyClose = TRUE;
				PostMessage(hWnd, WM_CLOSE, 0, 0l);
				break;
            }
         }
      }
   }

   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//----------------------------------------------------------------------------
// This window procedure takes care of important stuff before calling the
// consumer's ScreenSaverProc. 
LRESULT WINAPI RealScreenSaverProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
	case WM_CREATE:
		// screen saver does not need the IME
		if((hInstImm = GetModuleHandle(szImmDLL)) &&
			(ImmFnc = (IMMASSOCPROC)GetProcAddress(hInstImm,szImmFnc)))
			hPrevImc = ImmFnc(hWnd, (HIMC)NULL);

		// establish the mouse position
		GetCursorPos(&ptMouse);

		if(!childPreview)
			SetCursor(NULL);

		break;

	case WM_DESTROY:
		// screen saver does not need the IME
		if(hInstImm && ImmFnc && hPrevImc)
			ImmFnc(hWnd, hPrevImc);

		PostQuitMessage(0);
		break;

	case WM_SETTEXT:
		// we need to be able to use FindWindow() to find running instances
		// of full-screen windows screen savers
		// NOTE: USER slams our title in during WM_NCCREATE by calling the
		// defproc for WM_SETTEXT directly, so the initial title will get
		// there.  If this ever changes, we can simply set a bypass flag
		// during WM_NCCREATE processing.
		return FALSE;

	case WM_SYSCOMMAND:
		if(!childPreview){
			switch(wParam){
			case SC_NEXTWINDOW:       // no Alt-tabs
			case SC_PREVWINDOW:       // no shift-alt-tabs
			case SC_SCREENSAVE:       // no more screensavers
				return FALSE;
			}
		}
		break;

	case WM_HELP:
	case WM_CONTEXTMENU:
		if(childPreview){
			// if we're in preview mode, pump the help stuff to our owner
			HWND hParent = GetParent(hWnd);

			if(hParent && IsWindow(hParent))
				PostMessage(hParent, uMsg, (WPARAM)hParent, lParam);
			return TRUE;
		}
		break;

	case WM_TIMER:
		if(closing)
			return FALSE;
		Sleep(0);
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if(closing)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;

	case WM_PAINT:
		if(closing)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		if(!childPreview)
			SetCursor(NULL);
		break;
	}

	return ScreenSaverProc(hWnd, uMsg, wParam, lParam);
}


//----------------------------------------------------------------------------
// Functions for doing display mode stuff

// Initializes a combo box in the settings dialog of your saver by
// enumerating all possible video modes and making text strings out
// of them.
void WINAPI InitVideoModeComboBox(HWND hdlg, int comboBoxID){
	int i, done;
	int nummodes;
	char modestring[64];
	DEVMODE mode;

	// Count the number of modes
	done = 1;
	nummodes = 0;
	while(done){
		done = EnumDisplaySettings(NULL, nummodes, &mode);
		if(done)
			nummodes++;
	}
	
	// Clear combo box before trying to set it up
	for(i=nummodes; i>=0; i--)
		SendDlgItemMessage(hdlg, comboBoxID, CB_DELETESTRING, WPARAM(i), 0);
	// Insert "default" video mode string
	SendDlgItemMessage(hdlg, comboBoxID, CB_ADDSTRING, 0, LPARAM("no change"));
	// Insert all possible modes
	for(i=0; i<nummodes; i++){
		EnumDisplaySettings(NULL, i, &mode);
		sprintf(modestring, "%dx%d %dbits %dHz", mode.dmPelsWidth, mode.dmPelsHeight, mode.dmBitsPerPel, mode.dmDisplayFrequency);
		SendDlgItemMessage(hdlg, comboBoxID, CB_ADDSTRING, 0, LPARAM(modestring));
	}
	// Set the cursor position in the combo box
	SendDlgItemMessage(hdlg, comboBoxID, CB_SETCURSEL, WPARAM(theVideoMode), 0);
}


// Read video mode value from registry.
// This function is necessary because when the screensaver starts, the
// registry reading in the application comes too late for the video mode
// change.
void ReadVideoModeFromRegistry(){
	LONG result;
	HKEY skey;
	DWORD valtype, valsize;
	int val;

	theVideoMode = 0;
	theVisualID = 0;

	result = RegOpenKeyEx(HKEY_CURRENT_USER, registryPath, 0, KEY_READ, &skey);
	if(result != ERROR_SUCCESS)
		return;
	valsize=sizeof(val);
	result = RegQueryValueEx(skey, "theVideoMode", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		theVideoMode = val;
	result = RegQueryValueEx(skey, "theVisualID", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		theVisualID = val;

	RegCloseKey(skey);
}


// Change the video mode if a new video mode is specified.
void WINAPI AlterDisplayMode(){
	DEVMODE mode;

	// Change modes if "no change" is not selected
	// Remember, video modes are indexed from 0, not 1.
	// This means we have to subtract 1 from theVideoMode
	if(theVideoMode > 0){
		EnumDisplaySettings(NULL, theVideoMode-1, &mode);
		ChangeDisplaySettings(&mode, 0);
	}
}


// Choose the best pixel format possible, giving preference to harware
// accelerated modes.
void WINAPI SetBestPixelFormat(HDC hdc){
	int moreFormats, score = 0, nPixelFormat = 1, bestPixelFormat = 0, temp;
	PIXELFORMATDESCRIPTOR pfd;

	// Try to find the best pixel format
	moreFormats = DescribePixelFormat(hdc, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	while(moreFormats){
		// Absolutely must have these 4 attributes
		if((pfd.dwFlags & PFD_SUPPORT_OPENGL)
			&& (pfd.dwFlags & PFD_DRAW_TO_WINDOW)
			&& (pfd.dwFlags & PFD_DOUBLEBUFFER)
			&& (pfd.iPixelType == PFD_TYPE_RGBA)){
			// If this pixel format is good, see if it's the best...
			temp = 0;
			// color depth and z depth?
			temp += pfd.cColorBits + 2 * pfd.cDepthBits;
			if(pfd.cColorBits > 16)
				temp += (16 - pfd.cColorBits) / 2;
			// hardware accelerated?
			if(pfd.dwFlags & PFD_GENERIC_FORMAT){
				if(pfd.dwFlags & PFD_GENERIC_ACCELERATED)
					temp += 1000;
			}
			else
				temp += 2000;
			// Compare score
			if(temp > score){
				score = temp;
				bestPixelFormat = nPixelFormat;
			}
		}
		// Try the next pixel format
		nPixelFormat++;
		moreFormats = DescribePixelFormat(hdc, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	}

	// Let the user define their own pixel format in the registry if they want
	if(theVisualID)
		bestPixelFormat = theVisualID;
	// Set the pixel format for the device context
	if(bestPixelFormat){
		DescribePixelFormat(hdc, bestPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		SetPixelFormat(hdc, bestPixelFormat, &pfd);
		pfd_swap_exchange = pfd.dwFlags & PFD_SWAP_EXCHANGE;  // Is this bit set?
		pfd_swap_copy = pfd.dwFlags & PFD_SWAP_COPY;  // Is this bit set?
	}
	else{  // Just in case a best pixel format wasn't found
		PIXELFORMATDESCRIPTOR defaultPfd = {
			sizeof(PIXELFORMATDESCRIPTOR),          // Size of this structure
			1,                                      // Version of this structure    
			PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
			PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
			PFD_DOUBLEBUFFER,                       // Double buffered
			PFD_TYPE_RGBA,                          // RGBA Color mode
			24,                                     // Want 24bit color 
			0,0,0,0,0,0,                            // Not used to select mode
			0,0,                                    // Not used to select mode
			0,0,0,0,0,                              // Not used to select mode
			16,                                     // Size of depth buffer
			0,                                      // Not used to select mode
			0,                                      // Not used to select mode
			PFD_MAIN_PLANE,                         // Draw in main plane
			0,                                      // Not used to select mode
			0,0,0 };                                // Not used to select mode
		bestPixelFormat = ChoosePixelFormat(hdc, &defaultPfd);
		// SetPixelFormat is poorly documented and I don't trust it.
		// That's the main reason for this whole function.
		SetPixelFormat(hdc, bestPixelFormat, &defaultPfd);
		pfd_swap_exchange = defaultPfd.dwFlags & PFD_SWAP_EXCHANGE;  // Is this bit set?
		pfd_swap_copy = defaultPfd.dwFlags & PFD_SWAP_COPY;  // Is this bit set?
	}
}


//----------------------------------------------------------------------------

static int DoScreenSaver(HWND hParent){
	LPCTSTR windowClass = TEXT("WindowsScreenSaverClass");
	LPCTSTR windowTitle;

	WNDCLASS cls;
	MSG      msg;
	UINT     uStyle;
	UINT     uExStyle;
	int      nCx, nCy;

	cls.style          = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	cls.lpfnWndProc    = RealScreenSaverProc;
	cls.cbClsExtra     = 0;
	cls.cbWndExtra     = 0;
	cls.hInstance      = hMainInstance;
	cls.hIcon          = LoadIcon(hMainInstance, MAKEINTATOM(ID_APP));
	cls.hCursor        = NULL;
	cls.hbrBackground  = HBRUSH(GetStockObject(BLACK_BRUSH));
	cls.lpszMenuName   = NULL;
	cls.lpszClassName  = windowClass;

	if(hParent){
		RECT parentRect;
		GetClientRect(hParent, &parentRect);
		nCx = parentRect.right;
		nCy = parentRect.bottom;
		uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
		uExStyle = 0;

		childPreview = TRUE;
		windowTitle = TEXT("Preview");      // MUST differ from full screen
	}
	else{
		ReadVideoModeFromRegistry();
		AlterDisplayMode();

		HWND hOther;

		nCx = GetSystemMetrics(SM_CXSCREEN);
		nCy = GetSystemMetrics(SM_CYSCREEN);

		uStyle = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		uExStyle = WS_EX_TOPMOST;

		windowTitle = TEXT("Screensaver"); // MUST differ from preview

		// if there is another NORMAL screen save instance, switch to it
		hOther = FindWindow(windowClass, windowTitle);

		if(hOther && IsWindow(hOther)){
			SetForegroundWindow(hOther);
			return 0;
		}

		LoadPwdDLL();
	}

	if(RegisterClass(&cls)){
		hMainWindow = CreateWindowEx(uExStyle, windowClass, windowTitle,
			uStyle, 0, 0, nCx, nCy, hParent, (HMENU)NULL,
			hMainInstance, (LPVOID)NULL);
	}

	if(hMainWindow){
		if(!childPreview)
			SetForegroundWindow(hMainWindow);

		/*while(GetMessage(&msg, NULL, 0, 0)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}*/
		while (1){
		// Message loop
			while(!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
				// do idle processing (i.e. draw frames)
				IdleProc();
				Sleep(0);
			}
			// drop down to here once a message is found in the queue
			if(GetMessage(&msg, NULL, 0, 0)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else{
				UnloadPwdDLL();
				return msg.wParam;
			}
		}
	}

	// free password-handling DLL if loaded
	UnloadPwdDLL();

	return msg.wParam;
}


//----------------------------------------------------------------------------

static int DoSaverPreview(LPCTSTR szUINTHandle){
	doingPreview = TRUE;

	// get parent handle from string
	HWND hParent = (HWND)(atoui(szUINTHandle));

	// only preview on a valid parent window (NOT full screen)
	return ((hParent && IsWindow(hParent))? DoScreenSaver(hParent) : -1);
}


static int DoConfigBox(HWND hParent){
	//if(!RegisterDialogClasses(hMainInstance))
	//	return FALSE;
	return DialogBox(hMainInstance, MAKEINTRESOURCE(DLG_SCRNSAVECONFIGURE),
		hParent, (DLGPROC)ScreenSaverConfigureDialog);
}


static int DoChangePw(LPCTSTR szUINTHandle){
	// get parent handle from string
	HWND hParent = (HWND)(atoui(szUINTHandle));

	if(!hParent || !IsWindow(hParent))
		hParent = GetForegroundWindow();
	// allow the library to be hooked
	//ScreenSaverChangePassword(hParent);
	return 0;
}

static const TCHAR szMprDll[] = TEXT("MPR.DLL");       // not to be localized
static const TCHAR szProviderName[] = TEXT("SCRSAVE"); // not to be localized

//#ifdef UNICODE
//static const CHAR szPwdChangePW[] = "PwdChangePasswordW"; // not to be localized
//#else
static const CHAR szPwdChangePW[] = "PwdChangePasswordA"; // not to be localized
//#endif

// bogus prototype
typedef DWORD (FAR PASCAL *PWCHGPROC)(LPCTSTR, HWND, DWORD, LPVOID);

/*void WINAPI ScreenSaverChangePassword(HWND hParent){
	HINSTANCE mpr = LoadLibrary(szMprDll);

	if(mpr){
		PWCHGPROC pwd = (PWCHGPROC)GetProcAddress(mpr, szPwdChangePW);
		if(pwd)
			pwd(szProviderName, hParent, 0, NULL);
		FreeLibrary(mpr);
	}
}*/


//----------------------------------------------------------------------------

static BOOL DoPasswordCheck(HWND hParent){
	// don't reenter and don't check when we've already decided
	if(checkingPassword || closing)
		return FALSE;

	if(VerifyPassword){
		static DWORD lastcheck = (DWORD)-1;
		DWORD curtime = GetTickCount();
		MSG msg;

		DWORD elapsedtime = (curtime >= dwBlankTime) ? (curtime - dwBlankTime) : (1 + curtime + (((DWORD)-1) - dwBlankTime));
		if (dwPasswordDelay && (elapsedtime < dwPasswordDelay)){
			closing = TRUE;
			goto _didcheck;
		}

		// do the check
		checkingPassword = TRUE;

#ifdef GL_SCRNSAVE
		// Put ss in idle mode during password dialog processing
		SendMessage(hParent, SS_WM_IDLE, SS_IDLE_ON, 0L);
#endif

		// flush WM_TIMER messages before putting up the dialog
		PeekMessage(&msg, hParent, WM_TIMER, WM_TIMER, PM_REMOVE | PM_NOYIELD);
		PeekMessage(&msg, hParent, WM_TIMER, WM_TIMER, PM_REMOVE | PM_NOYIELD);

		// call the password verify proc
//		closing = (BOOL)SendMessage(hParent, SCRM_VERIFYPW, 0, 0L);

		checkingPassword = FALSE;

#ifdef GL_SCRNSAVE
		// Restore normal display mode
		SendMessage(hParent, SS_WM_IDLE, SS_IDLE_OFF, 0L);
#endif

		if(!closing)
			SetCursor(NULL);

		// curtime may be outdated by now
		lastcheck = GetTickCount();
	}
	else{
		// passwords disabled or unable to load handler DLL, always allow exit
		closing = TRUE;
	}

_didcheck:
	return closing;
}

//----------------------------------------------------------------------------

int _stdcall DummyEntry(void){
	int i;
	STARTUPINFO si;
	LPTSTR pszCmdLine = GetCommandLine();

	if(*pszCmdLine == TEXT('\"')){
		// Scan, and skip over, subsequent characters until
		// another double-quote or a null is encountered.
		while (*(pszCmdLine = CharNext(pszCmdLine)) &&
			(*pszCmdLine != TEXT('\"')));
		// If we stopped on a double-quote (usual case), skip
		// over it.
		if(*pszCmdLine == TEXT('\"'))
			pszCmdLine++;
	}
	else{
		while((UINT)*pszCmdLine > (UINT)TEXT(' '))
			pszCmdLine = CharNext(pszCmdLine);
	}

	// Skip past any white space preceeding the second token.
	while(*pszCmdLine && ((UINT)*pszCmdLine <= (UINT)TEXT(' ')))
		pszCmdLine = CharNext(pszCmdLine);

	si.dwFlags = 0;
	GetStartupInfo(&si);

	i = WinMainN(GetModuleHandle(NULL), NULL, pszCmdLine,
		si.dwFlags & STARTF_USESHOWWINDOW ? si.wShowWindow : SW_SHOWDEFAULT);

	ExitProcess(i);
	return i;   // We never come here.
}

//----------------------------------------------------------------------------
// main() entry point to satisfy old NT screen savers
void _cdecl main(int argc, char *argv[]){
	DummyEntry();
}

//----------------------------------------------------------------------------
// WinMain() entry point to satisfy old NT screen savers
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow) {
	DummyEntry();
	return 0;

	// reference unreferenced parameters
	(void)hInst;
	(void)hPrev;
	(void)szCmdLine;
	(void)nCmdShow;
}


VOID LoadPwdDLL(VOID){
	HKEY hKey;

	if(!onWin95)
		return;

	if(hInstPwdDLL)
		UnloadPwdDLL();

	// look in registry to see if password turned on, otherwise don't
	// bother to load password handler DLL
	if(RegOpenKey(HKEY_CURRENT_USER, szScreenSaverKey, &hKey) == ERROR_SUCCESS){
		DWORD dwVal,dwSize=sizeof(dwVal);

		if((RegQueryValueEx(hKey,szPasswordActiveValue, NULL, NULL, (BYTE *)&dwVal, &dwSize) == ERROR_SUCCESS) && dwVal){

			// load the password dll
			hInstPwdDLL = LoadLibrary(szPwdDLL);
			if(hInstPwdDLL){
				VerifyPassword = (VERIFYPWDPROC)GetProcAddress(hInstPwdDLL, szFnName);

				if(!VerifyPassword)
					UnloadPwdDLL();
			}
		}

		RegCloseKey(hKey);
	}

}

VOID UnloadPwdDLL(VOID){
	if(!onWin95)
		return;

	if(hInstPwdDLL){
		FreeLibrary(hInstPwdDLL);
		hInstPwdDLL = NULL;

		if(VerifyPassword)
			VerifyPassword = NULL;
	}
}