/*
 ============================================================================
  This file is part of the bbLeanBar source code.

  bbLeanBar is a plugin for BlackBox for Windows
  Copyright © 2003-2009 grischka

  http://bb4win.sourceforge.net/bblean/

  bbLeanBar is free software, released under the GNU General Public License
  (GPL version 2). See for details:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

 ============================================================================
*/

#ifdef BBTINY
#define NO_DROP
#define NO_TIPS
#endif

#include <BBApi.h>
#include <win0x500.h>
#include <bblib.h>
#include <bbstyle.h>
#include <bbversion.h>

#include "bbPlugin.h"
#include "drawico.h"
#ifndef NO_TIPS
#include "tooltips.h"
#endif

#include <shellapi.h>
#include <shlobj.h>
#include <time.h>

#include "bbLeanBar.h"
#include "BuffBmp.h"
#include "TinyDropTarg.h"

#include "barinfo.h"
#include "leanbar.h"

#ifndef ASFW_ANY
#define ASFW_ANY ((DWORD)-1)
#endif

#ifndef MK_ALT
#define MK_ALT 32
#endif


#ifdef BBTINY
const char szVersion     [] = "bbLeanBar " BBLEAN_VERSION "t";
const char szInfoVersion [] = BBLEAN_VERSION "t";
#else
const char szVersion     [] = "bbLeanBar " BBLEAN_VERSION;
const char szInfoVersion [] = BBLEAN_VERSION;
#endif
const char szAppName     [] = "bbLeanBar";
const char szInfoAuthor  [] = "grischka";
const char szInfoRelDate [] = BBLEAN_RELDATE;
const char szInfoLink    [] = "http://bb4win.sourceforge.net/bblean";
const char szInfoEmail   [] = "grischka@users.sourceforge.net";
const char szCopyright   [] = "2003-2009";

//===========================================================================

extern "C" API_EXPORT
void bbDrawPix(HDC hDC, RECT *p_rect, COLORREF picColor, int style); 


//====================
int currentScreen{}; 

plugin_info* g_PI{};

extern HWND BBhwnd;
extern int styleBorderWidth;
extern int styleBevelWidth;


//====================
#define NIF_INFO 0x00000010
#define NIN_BALLOONSHOW (WM_USER + 2)
#define NIN_BALLOONHIDE (WM_USER + 3)
#define NIN_BALLOONTIMEOUT (WM_USER + 4)
#define NIN_BALLOONUSERCLICK (WM_USER + 5)

void EnumTasks(TASKENUMPROC lpEnumFunc, LPARAM lParam)
{
    const tasklist* tl;
    dolist (tl, GetTaskListPtr())
        if (FALSE == lpEnumFunc(tl, lParam))
            break;
} 

//===========================================================================

DLL_EXPORT int beginPluginEx(HINSTANCE hPluginInstance, HWND hSlit)
{
    plugin_info* p;
    barinfo* PI;
    int n_index;

    BBhwnd = GetBBWnd();
    for (p = g_PI, n_index = 0; p; p = p->next, ++n_index)
        ;
    PI = new barinfo();
    sprintf(PI->instance_key,
        n_index ? "%s.%d" : "%s",
        szAppName,
        n_index+1
        );

    PI->hSlit       = hSlit;
    PI->hInstance   = hPluginInstance;
    PI->class_name  = szAppName;
    PI->rc_key      = PI->instance_key;
    PI->broam_key   = PI->instance_key;
    PI->is_bar      = true;
    PI->n_index     = n_index;

    PI->make_cfg();

    if (0 == BBP_Init_Plugin(PI)) {
        delete(PI);
        return 1;
    }

    PI->old_place = PI->place;

    if (0 == n_index) {
#ifndef NO_DROP
        OleInitialize(NULL);
#endif
#ifndef NO_TIPS
        InitToolTips(hPluginInstance);
#endif
    }

#ifndef NO_DROP
    PI->m_TinyDropTarget = init_drop_targ(PI->hwnd);
#endif

    return 0;
}

DLL_EXPORT void endPlugin(HINSTANCE hPluginInstance)
{
    if (g_PI)
        delete g_PI;

    if (NULL == g_PI) {
#ifndef NO_TIPS
        exit_bb_balloon();
        ExitToolTips();
#endif
#ifndef NO_DROP
        OleUninitialize();
#endif
    }
}

DLL_EXPORT LPCSTR pluginInfo(int field)
{
    switch (field)
    {
        default:
        case 0: return szVersion;
        case 1: return szAppName;
        case 2: return szInfoVersion;
        case 3: return szInfoAuthor;
        case 4: return szInfoRelDate;
        case 5: return szInfoLink;
        case 6: return szInfoEmail;
    }
}

//===========================================================================
