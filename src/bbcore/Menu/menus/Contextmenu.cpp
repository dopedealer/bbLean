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


#include <BB.h>
#include <Settings.h>
#include "Contextmenu.h"
#include "Utils.h"
#include "Blackbox.h"
#include "rcfile.h"

#include <shlobj.h>
#include <shellapi.h>

#include "ContextItem.h"
#include "ShellContext.h"

namespace bbcore { 

/// \brief Context menu for filesystem items. One of path or pidl can be NULL
Menu* makeContextMenu(const char* path, const void* pidl)
{
    char buffer[MAX_PATH];
    ShellContext* shellCtx{};
    BOOL r{};
    Menu* menu{};
    LPITEMIDLIST path_pidl{};

    if (nullptr == pidl && path)
    {
        pidl = path_pidl = get_folder_pidl(path, bbDefaultrcPath());
    }

    shellCtx = new ShellContext(&r, (LPCITEMIDLIST)pidl);
    if (FALSE == r)
    {
        delete shellCtx; 
    }
    else if (Settings_shellContextMenu)
    {
        int nCmd = shellCtx->ShellMenu();
        if (nCmd)
        {
            shellCtx->Invoke(nCmd);
        }
        delete shellCtx;

    }
    else
    {
        sh_getdisplayname((LPCITEMIDLIST)pidl, buffer, isUsingUtf8Encoding());
        menu = new ContextMenu(buffer, shellCtx, shellCtx->menuHandle(), 1);
    }

    freeIDList(path_pidl);
    return menu;
}


#ifndef CMF_CANRENAME
#define CMF_CANRENAME 16
#endif
#ifndef CMF_EXTENDEDVERBS
#define CMF_EXTENDEDVERBS 0x00000100 // rarely used verbs
#endif


//=========================================================================== 
//                          class ContextMenu 
//===========================================================================
// This below is for the BB-style context menu

ContextMenu::ContextMenu(const char* title, ShellContext* w, HMENU hm, int m)
    : Menu (title)
{
    setMenuId(MENU_ID_SHCONTEXT);
    (wc=w)->addref();
    Copymenu(hm);
}

ContextMenu::~ContextMenu(void)
{
    wc->decref();
} 

void ContextMenu::Copymenu(HMENU hm)
{
    char text_string[128];
    int n, c, id;
    MENUITEMINFO MII;
    Menu *CM;
    MenuItem *CI;
    [[maybe_unused]] bool sep = false;

    static int (WINAPI *pGetMenuStringW)(HMENU,UINT,LPWSTR,int,UINT);

    for (c = GetMenuItemCount(hm),n = 0; n < c; n++)
    {
        memset(&MII, 0, sizeof MII);
        if (usingXP)
            MII.cbSize = sizeof MII;
        else
            MII.cbSize = MENUITEMINFO_SIZE_0400; // to make this work on win95

        MII.fMask  = MIIM_DATA|MIIM_ID|MIIM_SUBMENU|MIIM_TYPE;
        GetMenuItemInfo (hm, n, TRUE, &MII);
        id = MII.wID;

        text_string[0] = 0;
        if (0 == (MII.fType & MFT_OWNERDRAW))
        {
            if (usingNT && load_imp(&pGetMenuStringW, "user32.dll", "GetMenuStringW"))
            {
                WCHAR wstr[128];
                pGetMenuStringW(hm, n, wstr, array_count(wstr), MF_BYPOSITION);
                bbWideCharToMbyte(wstr, text_string, sizeof text_string);
            }
            else
            {
                GetMenuStringA(hm, n, text_string, sizeof text_string, MF_BYPOSITION);
            }
        }

        //char buffer[100]; sprintf(buffer, "%d <%s>", id, text_string); strcpy(text_string, buffer);

        CM = NULL;
        if (MII.hSubMenu)
        {
            wc->HandleMenuMsg(WM_INITMENUPOPUP, (WPARAM)MII.hSubMenu, MAKELPARAM(n, FALSE));
            if (usingWin7)
                BBSleep(10);
            CM = new ContextMenu(text_string, wc, MII.hSubMenu, 0);

        }
        else if (MII.fType & MFT_SEPARATOR)
        {
            sep = true;
            continue;
        }
#if 0
        if (sep)
            MakeMenuNOP(this, NULL), sep = false;
#endif
        CI = new ContextItem(CM, text_string, id, MII.dwItemData, MII.fType);
        AddMenuItem(CI);
    }
}

}; //!namespace bbcore
