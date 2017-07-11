/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Savergl is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Savergl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <windows.h>
#include <stdio.h>
#include "savergl.h"




// These variables are externed in Savergl.h so you can use them in your saver
HINSTANCE mainInstance = 0;
HWND mainWindow = 0;
BOOL childPreview = FALSE;
BOOL reallyClose = FALSE;
BOOL checkingPassword = FALSE;
BOOL doingPreview = 0;
int theVideoMode = 0;
int theVisualID = 0;
BOOL pfd_swap_exchange = FALSE;
BOOL pfd_swap_copy = FALSE;

POINT mousePoint;
BOOL closing = FALSE;
int wakeThreshold = 4;  // must move 4 pixels to wake up
typedef BOOL (WINAPI* VERIFYPASSWORDPROC) (HWND hwnd);
typedef VOID (WINAPI* PASSWORDCHANGEPROC) (LPCSTR regKeyName, HWND hwnd, DWORD dwd, LPVOID lpv);

static int startScreenSaver(HWND parent);
static int openConfigBox(HWND parent);
static int startSaverPreview(LPCTSTR str);



//----------------------------------------------------------------------------
// Password functions (only needed on Win95 and Win98)
// I think this code still has trouble actually drawing the password checking box
// Maybe I should install Win95 and test it.  Ha!


BOOL checkPassword(HWND hwnd){
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize=sizeof(osvi);
	GetVersionEx(&osvi);
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS){  // this is Win95 or Win98
		HINSTANCE passwordcpl = LoadLibrary("PASSWORD.CPL");
		if(passwordcpl == NULL)
			return TRUE;
		VERIFYPASSWORDPROC verifyPwdProc;
		verifyPwdProc = (VERIFYPASSWORDPROC)GetProcAddress(passwordcpl, "VerifyScreenSavePwd");
		if(verifyPwdProc==NULL){
			FreeLibrary(passwordcpl);
			return TRUE;
		}
		bool junk;
		SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, &junk, 0);  // disable ctrl-alt-delete
		checkingPassword = TRUE;
		BOOL verified = verifyPwdProc(hwnd);
		checkingPassword = FALSE;
		SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, &junk, 0);
		FreeLibrary(passwordcpl);
		return verified;
	}

	return TRUE;
}


void changePassword(HWND hwnd){
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize=sizeof(osvi);
	GetVersionEx(&osvi);
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS){  // this is Win95 or Win98
		HINSTANCE mprdll = LoadLibrary("MPR.DLL");
		if(mprdll == NULL)
			return;
		PASSWORDCHANGEPROC PwdChangePassword
			= (PASSWORDCHANGEPROC)GetProcAddress(mprdll, "PwdChangePasswordA");
		if(PwdChangePassword == NULL){
			FreeLibrary(mprdll);
			return;
		}
		PwdChangePassword("SCRSAVE", hwnd, 0, 0);
		FreeLibrary(mprdll);
	}
}


//----------------------------------------------------------------------------


LRESULT DefScreenSaverProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	static int firstMouseMove = 0;  // Used for 3dfx driver fix

	if(!childPreview && !closing){
		switch(uMsg){
		case WM_CLOSE:
			// We need to explicitly tell this program to close.
			// NT will sometimes send a WM_CLOSE on its own.
			if(!reallyClose)
				return(0);
			// Check password on Win95 or Win98 (NT takes care of this on its own)
			if(!checkPassword(hWnd)){
				GetCursorPos(&mousePoint);  // reset mouse position if not quitting
				return(0);
			}
			// If video mode is other than default display resolution
			// we need to set the display back to the default
			if(theVideoMode > 0){
				DestroyWindow(hWnd);
				ChangeDisplaySettings(NULL, 0);
			}
			break;
		default:{
			POINT movePoint, checkPoint;
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
				// This prevents 3dfx drivers from killing saver because
				// 3dfx drivers send extraneous WM_MOUSEMOVE message.
				if(!firstMouseMove)
					firstMouseMove = 1;
				else
					reallyClose = TRUE;
				// check for real movements
				GetCursorPos(&checkPoint);
				movePoint.x = mousePoint.x - checkPoint.x;
				if(movePoint.x < 0)
					movePoint.x *= -1;
				movePoint.y = mousePoint.y - checkPoint.y;
				if(movePoint.y < 0)
					movePoint.y *= -1;
				if((movePoint.x + movePoint.y) > wakeThreshold)
					PostMessage(hWnd, WM_CLOSE, 0, 0l);
				else
					mousePoint = checkPoint;
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


LRESULT WINAPI RealScreenSaverProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_CREATE:
		// get the mouse position at startup
		GetCursorPos(&mousePoint);
		if(!childPreview)
			SetCursor(NULL);
		break;
	case WM_SYSCOMMAND:
		if(!childPreview){
			switch(wParam){
			// ignore these messages
			case SC_NEXTWINDOW:       // alt-tabs
			case SC_PREVWINDOW:       // shift-alt-tabs
			case SC_SCREENSAVE:       // other screensavers
				return FALSE;
			}
		}
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if(closing)
			return DefWindowProc(wnd, msg, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return ScreenSaverProc(wnd, msg, wParam, lParam);
}


//-----------------------------------------------------------------------
// Functions for doing display mode stuff

// Initializes a combo box in the settings dialog of your saver by
// enumerating all possible video modes and making text strings out
// of them.
void InitVideoModeComboBox(HWND dlg, int comboBoxID){
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
		SendDlgItemMessage(dlg, comboBoxID, CB_DELETESTRING, WPARAM(i), 0);
	// Insert "default" video mode string
	SendDlgItemMessage(dlg, comboBoxID, CB_ADDSTRING, 0, LPARAM("no change"));
	// Insert all possible modes
	for(i=0; i<nummodes; i++){
		EnumDisplaySettings(NULL, i, &mode);
		sprintf(modestring, "%dx%d %dbits %dHz", mode.dmPelsWidth, mode.dmPelsHeight, mode.dmBitsPerPel, mode.dmDisplayFrequency);
		SendDlgItemMessage(dlg, comboBoxID, CB_ADDSTRING, 0, LPARAM(modestring));
	}
	// Set the cursor position in the combo box
	SendDlgItemMessage(dlg, comboBoxID, CB_SETCURSEL, WPARAM(theVideoMode), 0);
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
void AlterDisplayMode(){
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
void SetBestPixelFormat(HDC hdc){
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
			24,                                     // Size of depth buffer
			0,                                      // Not used to select mode
			0,                                      // Not used to select mode
			PFD_MAIN_PLANE,                         // Draw in main plane
			0,                                      // Not used to select mode
			0,0,0 };                                // Not used to select mode
		bestPixelFormat = ChoosePixelFormat(hdc, &defaultPfd);
		// ChoosePixelFormat is poorly documented and I don't trust it.
		// That's the main reason for this whole function.
		SetPixelFormat(hdc, bestPixelFormat, &defaultPfd);
		pfd_swap_exchange = defaultPfd.dwFlags & PFD_SWAP_EXCHANGE;  // Is this bit set?
		pfd_swap_copy = defaultPfd.dwFlags & PFD_SWAP_COPY;  // Is this bit set?
	}
}


//-----------------------------------------------------------------


static int startScreenSaver(HWND parent){
	LPCTSTR windowClass = TEXT("WindowsScreenSaverClass");
	LPCTSTR windowTitle;
	WNDCLASS wclass;
	MSG msg;
	UINT style;
	UINT exStyle;
	int width, height;

	wclass.style          = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wclass.lpfnWndProc    = RealScreenSaverProc;
	wclass.cbClsExtra     = 0;
	wclass.cbWndExtra     = 0;
	wclass.hInstance      = mainInstance;
	wclass.hIcon          = LoadIcon(mainInstance, MAKEINTATOM(ID_APP));
	wclass.hCursor        = NULL;
	wclass.hbrBackground  = HBRUSH(GetStockObject(BLACK_BRUSH));
	wclass.lpszMenuName   = NULL;
	wclass.lpszClassName  = windowClass;

	if(parent){
		RECT parentRect;
		GetClientRect(parent, &parentRect);
		width = parentRect.right;
		height = parentRect.bottom;
		style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
		exStyle = 0;
		childPreview = TRUE;
		windowTitle = TEXT("Preview");  // must differ from full screen window name
	}
	else{
		// change display mode if necessary
		ReadVideoModeFromRegistry();
		AlterDisplayMode();

		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
		style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exStyle = WS_EX_TOPMOST;

		// if there is another normal screen saver instance, switch to it
		windowTitle = TEXT("Screensaver");  // must differ from preview window name
		HWND hOther = FindWindow(windowClass, windowTitle);

		if(hOther && IsWindow(hOther)){
			SetForegroundWindow(hOther);
			return 0;
		}
	}

	if(RegisterClass(&wclass))
		mainWindow = CreateWindowEx(exStyle, windowClass, windowTitle, style,
			0, 0, width, height, parent, (HMENU)NULL, mainInstance, (LPVOID)NULL);

	if(mainWindow){
		if(!childPreview)
			SetForegroundWindow(mainWindow);
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
			else
				return msg.wParam;
		}
	}

	return msg.wParam;
}


// convert text to unsigned int
static UINT atoui(LPCTSTR str){
	UINT returnval = 0;
	while((*str >= TEXT('0')) && (*str <= TEXT('9')))
		returnval = ((returnval * 10) + (*str++ - TEXT('0')));
	return returnval;
}


static int startSaverPreview(LPCTSTR str){
	doingPreview = TRUE;

	// get parent handle from string
	HWND hParent = (HWND)(atoui(str));

	// only preview on a valid parent window (NOT full screen)
	return((hParent && IsWindow(hParent)) ? startScreenSaver(hParent) : -1);
}


static int openConfigBox(HWND parent){
	return DialogBox(mainInstance, MAKEINTRESOURCE(DLG_SCRNSAVECONFIGURE),
		parent, (DLGPROC)ScreenSaverConfigureDialog);
}


//-----------------------------------------------------------------------
// main function

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prevInst, LPSTR cmdLine, int cmdShow){
	LPCTSTR commandLine = cmdLine;
	HWND parent = 0;

	mainInstance = inst;

	while(1){
		switch(*commandLine){
		case TEXT(' '):  // skip over whitespace
		case TEXT('-'):
		case TEXT('/'):
			commandLine++;
			break;
		case TEXT('a'):  // change password on Win95
		case TEXT('A'):
			changePassword(parent);
			return -1;
		case TEXT('c'):  // open "settings" dialog box
		case TEXT('C'):
			return openConfigBox(GetForegroundWindow());
		case TEXT('\0'):
			return openConfigBox(NULL);
		case TEXT('p'):  // run the preview
		case TEXT('P'):
			do
				commandLine++;
			while(*commandLine == TEXT(' '));  // skip over whitespace (I'm not sure why)
			return startSaverPreview(commandLine);
		case TEXT('s'):  // run the saver
		case TEXT('S'):
			return startScreenSaver(NULL);
		default:
			return -1;
		}
	}

	return -1;
}