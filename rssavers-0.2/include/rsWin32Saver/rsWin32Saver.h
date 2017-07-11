/*
 * Copyright (C) 1999-2010  Terence M. Welsh
 *
 * This file is part of rsWin32Saver.
 *
 * rsWin32Saver is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * rsWin32Saver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/*
 * OpenGL screensaver library
 *
 * Library for handling standard OpenGL screensaver functionality, such
 * as opening windows, choosing pixel formats, setting video modes,
 * handling user input, etc...
 *
 * This library is functionally similar to Microsoft's scrnsave.lib.  The
 * most important extras are a routine for choosing an OpenGL video format
 * and routines for choosing a video mode.  So if you've ever written a saver
 * using scrnsave.lib and want to try writing an OpenGL saver, this library
 * should be simple for you to use.
 *
 * The best resources I've found for learning this Windows screensaver stuff are:
 * http://www.wischik.com/scr, which explains all the code in plain English, and
 * the scrnsave.lib example code found in the MSDN Library.
 * For the OpenGL part of this code, I can't name any good examples.
 */



#ifndef RSWIN32SAVER_H
#define RSWIN32SAVER_H



#include <util/rsTimer.h>



// String required in the .rc file.
// This will be the name displayed in the Screen Saver config window.
// This doesn't seem to work under some versions of Windows if filename
// has more than 8 characters.
#define IDS_DESCRIPTION 1
// Screensaver's "settings" dialog box
#define DLG_SCRNSAVECONFIGURE 7001
// icon resource id
#define ID_APP 7002


// The following globals are defined in rsWin32Saver.cpp
extern HINSTANCE mainInstance;
extern HWND mainWindow;
extern int checkingPassword;  // A saver should check this and stop drawing if true
extern int isSuspended;  // power save mode
extern int doingPreview;  // Previews take extra long to initialize because Windows
// has lots of screwy resource hogging problems.  It's good to check this so that you
// can simplify your preview initialization.  For example, if you need to initialize
// data structures, keep them as small as possible.
extern LPCTSTR registryPath;  // Saver must define and use this registry path
// if you want all your video mode info in the same registry folder as
// all your other parameters.  If you don't redefine this path, it will
// default to the same path for every saver that you write, and that would
// be really annoying.

// These variables can be useful information for saver programmer
extern int pfd_swap_exchange;  // Is this bit set in the PIXELFORMATDESCRIPTOR?
extern int pfd_swap_copy;  // Is this bit set in the PIXELFORMATDESCRIPTOR?
// used to limit frame rate of saver (0 = no limit)
extern unsigned int dFrameRateLimit;
// Keyboard toggle for displaying statistics, such as frames per second.
// Application must implement the displaying of statistics if kStatistics is true.
extern int kStatistics;


//----------------------------------------------------------------------------


// The following functions must be defined by the saver that uses this library.

// This should handle all messages to the saver
LRESULT screenSaverProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
// All idle processing is done here.  Typically, you would just draw 
// frames during this routine.
void idleProc();
// Use this function to set up the "settings" dialog box.
BOOL screenSaverConfigureDialog (HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam);


//----------------------------------------------------------------------------


// The ScreenSaverProc function should call this function with any unhandled messages
LRESULT defScreenSaverProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


//----------------------------------------------------------------------------


// Chooses the best pixel format available in the current graphics mode.
// It gives preference to modes that use OpenGL harware acceleration.
void setBestPixelFormat(HDC hdc);


//----------------------------------------------------------------------------

// Functions for using frame rate limiter

void initFrameRateLimitSlider(HWND hdlg, int sliderID, int textID);
void updateFrameRateLimitSlider(HWND hdlg, int sliderID, int textID);
void readFrameRateLimitFromRegistry();




#endif
