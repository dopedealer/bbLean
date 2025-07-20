/* ==========================================================================

  This file is part of the bbLean source code
  Copyright © 2001-2003 The Blackbox for Windows Development Team
  Copyright © 2004-2009 grischka

  http://bb4win.sourceforge.net/bblean
  http://developer.berlios.de/projects/bblean

  bbLean is free software, released under the GNU General Public License
  (GPL version 2). For details see:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  ========================================================================== */

/* BB.h - global defines aside from the plugin-api */

#ifndef _BB_H_
#define _BB_H_

// ==============================================================
/* optional defines */

/* menu icon support, sketchy */
//#define BBOPT_MENUICONS

// ==============================================================

#include <assert.h>

/* Always includes */
#include "BBApi.h"
#include "win0x500.h"       // some later windows APIs
#include "bblib.h"
#define NO_INTSHCUT_GUIDS // Don't include internet shortcut GUIDs
#define NO_SHDOCVW_GUIDS // Don't include 'FolderItem' with shlobj.h

// ==============================================================
/* compiler specifics */

#ifdef __BORLANDC__
  #define TRY __try
  #define EXCEPT(n) __except(n)
#endif

#ifdef _MSC_VER
  #define TRY __try
  #define EXCEPT(n) __except(n)
//  #pragma warning(disable: 4100) /* unreferenced function parameter */
#endif

#ifndef TRY
  #define TRY if (1)
  #define EXCEPT(n) if (0)
#endif

#define SIZEOFPART(s,e) (offsetof(s,e) + sizeof(((s*)0)->e))

#include "bbversion.h"

#ifdef BBTINY
    #define BBAPPNAME "bbTiny"
    #define BBAPPVERSION "bbLean " BBLEAN_VERSION " (Tiny)"
#else
    #define BBAPPNAME "bbLean"
    #define BBAPPVERSION "bbLean " BBLEAN_VERSION
#endif

#ifdef __BBCORE__

namespace bbcore { 

struct plugins;

extern HWND gBBhwnd;
extern bool gUnderExplorer;

// ==============================================================
/* global variables */ 
extern int VScreenX, VScreenY, VScreenWidth, VScreenHeight;

/* Menu */
bool Menu_IsA(HWND hwnd);


// ==============================================================
/* Blackbox.cpp */

void post_command(const char *cmd);
void post_command_fmt(const char *fmt, ...);
int get_workspace_number(const char *s);
void set_opaquemove(int);
void set_focus_model(const char *fm_string);

// ==============================================================
/* global variables */
// blackbox.cpp:
extern HINSTANCE hMainInstance;
extern DWORD BBThreadId;
extern unsigned WM_ShellHook;
extern bool usingNT, usingXP, usingVista, usingWin7;
extern bool bbactive;

extern BOOL (WINAPI* pSwitchToThisWindow)(HWND, int);

/* drawing */
void read_pix(void);
void reset_pix(void);
void unregister_fonts(void);
void register_fonts(void);
void arrow_bullet (HDC buf, int x, int y, int d); // missing ??
void draw_line_h(HDC hDC, int x1, int x2, int y, int w, COLORREF C);
char* replace_arg1(const char *fmt, const char *in);

/* other */
BOOL BBRegisterClass (const char *classname, WNDPROC wndproc, int flags);
#define BBCS_VISIBLE 1
#define BBCS_EXTRA 2
#define BBCS_DROPSHADOW 4
int EditBox(const char *caption, const char *message, const char *initvalue, char *buffer);

/* Logging */
void _log_printf(int flag, const char *fmt, ...);
#define LOG_PLUGINS 1
#define LOG_STARTUP 2
#define LOG_TRAY 4
#define LOG_SHUTDOWN 8
#define log_printf(args) _log_printf args

// ==============================================================
/* workspaces and tasks */

struct hwnd_list { struct hwnd_list *next; HWND hwnd; };
void get_window_icon(HWND hwnd, HICON *picon);
void get_window_text(HWND hwnd, char *buffer, int size);

// ==============================================================
/* definitions */ 

/* Blackbox window timers */
#define BB_RUNSTARTUP_TIMER     1
#define BB_ENDSTARTUP_TIMER     2
#define BB_TASKUPDATE_TIMER     3

/* SetDesktopMargin internal flags */
#define BB_DM_REFRESH -1
#define BB_DM_RESET -2

/* resource.rc */
#define IDI_BLACKBOX 101 // blackbox.ico
#define IDC_MOVEMENU 102 // MoveMenu.cur
#define IDC_EDITBOX 103 // MoveMenu.cur 

// ==============================================================
/* Some enumeration function, maybe candidates for the API */

typedef BOOL (*TASKENUMPROC)(tasklist *, LPARAM);
void EnumTasks (TASKENUMPROC lpEnumFunc, LPARAM lParam);

typedef BOOL (*DESKENUMPROC)(DesktopInfo *, LPARAM);
void EnumDesks (DESKENUMPROC lpEnumFunc, LPARAM lParam);

typedef BOOL (*TRAYENUMPROC)(systemTray *, LPARAM);
void EnumTray (TRAYENUMPROC lpEnumFunc, LPARAM lParam);

typedef BOOL (*PLUGINENUMPROC)(plugins*, LPARAM);
void EnumPlugins (PLUGINENUMPROC lpEnumFunc, LPARAM lParam);


} // !namespace bbcore 


// ==============================================================
#endif /*def __BBCORE__ */
#endif /*ndef _BB_H_ */
