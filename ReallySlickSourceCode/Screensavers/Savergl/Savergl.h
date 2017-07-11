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


/*
// OpenGL screensaver library

// Library for handling standard OpenGL screensaver functionality, such
// as opening windows, choosing pixel formats, setting video modes,
// handling user input, etc...

// This library is functionally similar to Microsoft's scrnsave.lib.  The
// most important extras are a routine for choosing an OpenGL video format
// and routines for choosing a video mode.  So if you've ever written a saver
// using scrnsave.lib and want to try writing an OpenGL saver, this library
// should be simple for you to use.

// The best resources I've found for learning this Windows screensaver stuff are:
// http://www.wischik.com/scr, which explains all the code in plain English, and
// the scrnsave.lib example code found in the MSDN Library.
// For the OpenGL part of this code, I can't name any good examples.  I
// usually just use GLUT or some other windowing library to take care
// of opening windows.
*/



#ifndef SAVERGL_H
#define SAVERGL_H



// String required in the .rc file.
// This will be the name displayed in the Screen Saver config window.
// This oesn't seem to work under some versions of Windows if filename
// has more than 8 characters.
#define IDS_DESCRIPTION 1
// Screensaver's "settings" dialog box
#define DLG_SCRNSAVECONFIGURE 7001
// icon resource id
#define ID_APP 7002


// The following globals are defined in Scrnsave.cpp
extern HINSTANCE mainInstance;
extern HWND mainWindow;
extern BOOL checkingPassword;  // A saver should check this and stop drawing if true
extern BOOL doingPreview;  // Previews take extra long to initialize because Windows
// has lots of screwy resource hogging problems.  It's good to check this so that you
// can simplify your preview.
extern LPCTSTR registryPath;  // Saver must define and use this registry path
// if you want all your video mode info in the same registry folder as
// all your other parameters.  If you don't redefine this path, it will
// default to the same path for every saver that you write, and that would
// be really annoying.
extern int theVideoMode;  // Video mode to use when saver starts up for real
// (not as a preview).  -1 = no change
extern int theVisualID;  // Advanced feature for those who want to specify a visual ID
// These variables can be useful information for saver programmer
extern BOOL pfd_swap_exchange;  // Is this bit set in the PIXELFORMATDESCRIPTOR?
extern BOOL pfd_swap_copy;  // Is this bit set in the PIXELFORMATDESCRIPTOR?


//----------------------------------------------------------------------------


// The following functions must be defined by the saver that uses this library.

// This should handle all messages to the saver
LRESULT ScreenSaverProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
// All idle processing is done here.  Typically, you would just draw 
// frames during this routine.
void IdleProc();
// Use this function to set up the "settings" dialog box.
BOOL ScreenSaverConfigureDialog (HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam);


//----------------------------------------------------------------------------


// The ScreenSaverProc function should call this function with any unhandled messages
LRESULT DefScreenSaverProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


//----------------------------------------------------------------------------


// The following are some other functions that could come in handy

// Initializes a combo box with a listing for every video mode that
// is available from the default video device.
void InitVideoModeComboBox(HWND dlg, int comboBoxID);

// Attempts to set the display mode to the parameters on the default
// display device indicated by theVideoMode
void AlterDisplayMode();

// Chooses the best pixel format available in the current graphics mode.
// It gives preference to modes that use OpenGL harware acceleration.
void SetBestPixelFormat(HDC hdc);



#endif