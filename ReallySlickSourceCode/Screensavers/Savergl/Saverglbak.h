/*
// OpenGL screensaver library

// Library for handling standard OpenGL screensaver functionality, such
// as opening windows, choosing pixel formats, setting video modes,
// handling user input, etc...

// This library is functionally similar to MicroSoft's scrnsave.lib.  The
// most important extras are a routine for choosing an OpenGL video format
// and routines for choosing a video mode.
*/



#ifndef SAVERGL_H
#define SAVERGL_H



// Screensaver's configuration dialog box
#define DLG_SCRNSAVECONFIGURE   7001

// String required in the .rc file.
// This will be the name displayed in the Screen Saver config window.
// Doesn't work under some versions of Windows if filename has more than 8 characters.
#define IDS_DESCRIPTION      1

// icon resource id
#define ID_APP      7002


// The following globals are defined in Scrnsave.cpp
extern HINSTANCE hMainInstance;
extern HWND hMainWindow;
//extern BOOL childPreview;
//extern BOOL reallyClose;
extern BOOL checkingPassword;
extern BOOL doingPreview;
extern LPCTSTR registryPath;  // Must redefine and use this registry path
// if you want all your video mode info in the same registry folder as
// all your other parameters.  If you don't redefine this path, it will
// default to the same path for every saver that you write, and that would
// really suck.
extern int theVideoMode;  // Video mode to use when saver starts up for real
// (not as a preview).  -1 = no change
extern int theVisualID;  // Advanced feature for those who want to specify a visual ID
extern BOOL pfd_swap_exchange;  // Is this bit set in the PIXELFORMATDESCRIPTOR?
extern BOOL pfd_swap_copy;  // Is this bit set in the PIXELFORMATDESCRIPTOR?
// Can be useful information for saver programmer :)


/* This function is the Window Procedure for the screen saver.  It is
 * up to the programmer to handle any of the messages that wish to be
 * interpretted.  Any unused messages are then passed back to
 * DefScreenSaverProc if desired which will take default action on any
 * unprocessed message...
 */
/*#ifdef UNICODE
LRESULT WINAPI ScreenSaverProcW (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#   define  ScreenSaverProc ScreenSaverProcW
#else*/
LRESULT WINAPI ScreenSaverProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//#endif

/* This function performs default message processing.  Currently handles
 * the following messages:
 */
LRESULT WINAPI DefScreenSaverProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/* All idle processing is done here.  Typically, you would just draw 
 * frames during this routine.
 */
void WINAPI IdleProc();

/* A function is also needed for configuring the screen saver.  The function
 * should be exactly like it is below and must be exported such that the
 * program can use MAKEPROCINSTANCE on it and call up a dialog box. Further-
 * more, the template used for the dialog must be called
 * ScreenSaverConfigure to allow the main function to access it...
 */
BOOL WINAPI ScreenSaverConfigureDialog (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

/* To allow the programmer the ability to register child control windows, this
 * function is called prior to the creation of the dialog box.  Any
 * registering that is required should be done here, or return TRUE if none
 * is needed...
 */
//BOOL WINAPI RegisterDialogClasses (HANDLE hInst);



//----------------------------------------------------------------------------

/* The following functions are called by DefScreenSaverProc and must
 * be exported by all screensavers using this model.
 */


/* OPTIONAL - Win95 Only */

/*
 * This message is sent to the main screen saver window when password
 * protection is enabled and the user is trying to close the screen saver.  You
 * can process this message and provide your own validation technology.  If you
 * process this message, you should also support the ScreenSaverChangePassword
 * function, described below.  Return zero from this message if the password
 * check failed.  Return nonzero for success.  If you run out of memory or
 * encounter a similar class of error, return non-zero so the user isn't left
 * out in the cold.  The default action is to call the Windows Master
 * Password Router to validate the user's password.
 */
//#define SCRM_VERIFYPW   WM_APP

/*
 * You supply this if you provide your own authentication.  Windows will call
 * it when the user wants to change the password.  An implementation of this
 * function should present password change UI to the user.
 * You should only supply this function if you also hook the SCRM_VERIFYPW
 * message to validate passwords.
 * The default action is to call the Windows Master Password Router.
 */
//void WINAPI ScreenSaverChangePassword( HWND hParent );

//----------------------------------------------------------------------------

/*
 * Initializes a combo box with a listing for every video mode that
 * is available from the default video device.
 */
void WINAPI InitVideoModeComboBox(HWND hdlg, int comboBoxID);

/*
 * Attempts to set the display mode to the parameters on the default
 * display device indicated by theVideoMode
 */
void WINAPI AlterDisplayMode();

/*
 * Chooses the best pixel format available in the current graphics mode.
 * It gives preference to modes that use OpenGL harware acceleration.
 */
void WINAPI SetBestPixelFormat(HDC hdc);




#endif  // SAVERGL_H
