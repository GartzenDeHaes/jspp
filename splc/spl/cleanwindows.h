/*
 *   This file is part of the Standard Portable Library (SPL).
 *
 *   SPL is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   SPL is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with SPL.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _cleanwindows_h
#define _cleanwindows_h

#ifdef _WINDOWS_
//#pragma message ( " " )
//#pragma message ( "You have included windows.h before cleanwindows.h" )
//#pragma message ( "All useless stuff from the windows headers won't be excluded !!!" )
//#pragma message ( " " )
#endif // _WINDOWS_

// Don't define the following because we need it
//#define NOVIRTUALKEYCODES 1 // VK_ESCAPE, VK_SPACE
//#define NOWINMESSAGES 1     // WM_QUIT, WM_INITDIALOG, LB_ADDSTRING
//#define NOWINSTYLES 1       // WS_OVERLAPPEDWINDOW, WS_SYSMENU, CS_OWNDC
//#define NOSHOWWINDOW 1      // SW_SHOWNORMAL, SW_HIDE
//#define NOCOLOR 1           // COLOR_GRAYTEXT
//#define NOCTLMGR 1          // GetDlgItem, CheckDlgButton, DialogBox, LB_RESETCONTENT, CBN_SELCHANGE
//#define NOGDI 1             // DEVMODE
//#define NOUSER 1            // MessageBox, GetCursorPos, PeekMessage, WM_CLOSE
//#define NOMB 1              // MessageBox, MB_OK
//#define NOMSG 1             // PeekMessage, MSG, PM_NOREMOVE
//#define NOWINOFFSETS 1      // SetWindowLong, GWL_STYLE


// Define the following to exclude some unused services from the windows headers.
// For information on what the following defenitions will exclude, look in the windows.h header file.
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOATOM
#define NOCLIPBOARD
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOCRYPT

// Define these for MFC projects
#define NOTAPE
#define NOIMAGE
#define NOPROXYSTUB
#define NORPC
#define NOIME

// Also define WIN32_LEAN_AND_MEAN to exclude rarely-used services from windows headers.
#define WIN32_LEAN_AND_MEAN

// Finally now we can include windows.h
#include <windows.h>

//#include <commdlg.h> // For GetSaveFileName, GetOpenFileName and OPENFILENAME
                     // commdlg.h is included in windows.h but doesn't get included with WIN32_LEAN_AND_MEAN
                     // We still use WIN32_LEAN_AND_MEAN because we can include commdlg.h here and WIN32_LEAN_AND_MEAN
                     // excludes much more than commdlg.h

#endif // _cleanwindows_h
 
