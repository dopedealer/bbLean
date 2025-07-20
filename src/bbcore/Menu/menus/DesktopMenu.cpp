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

#include "BB.h"
#include "Settings.h"
#include "Workspaces.h"
#include "Menu/menus/Menu.h"
#include "Menu/MenuMaker.h"
#include "nls.h"
#include "system.h"
#include "Utils.h"
#include "DesktopMenu.h"

namespace bbcore {

//===========================================================================
struct en { Menu *m; int desk; HWND hwndTop; int flags, i, n; };
enum e_flags { e_alltasks = 1 };

static BOOL task_enum_func(tasklist* tl, LPARAM lParam)
{
    en* entry = (en*)lParam;
    bool is_top = tl->hwnd == entry->hwndTop;
    bool iconic = !is_top && IsIconic(tl->hwnd);
    entry->i++;
    if ((entry->desk == -1 && iconic) // iconic tasks
     || (entry->desk == tl->wkspc // tasks for one workspace
        && ((entry->flags & e_alltasks) || false == iconic)))
    {
        char buf[100];
        MenuItem *mi;

        sprintf(buf, "@BBCore.ActivateWindow %d", entry->i);
        mi = makeMenuItem(entry->m, tl->caption, buf, is_top);
#ifdef BBOPT_MENUICONS
        if (tl->icon)
            menuItemOption(mi, BBMENUITEM_SETHICON, tl->icon);
#endif
        if (entry->desk != -1) {
            sprintf(buf, "@BBCore.MinimizeWindow %d", entry->i);
            menuItemOption(mi, BBMENUITEM_RCOMMAND, buf);
            if (iconic)
                menuItemOption(mi, BBMENUITEM_DISABLED);
        }
        entry->n ++;
    }
    return TRUE;
}

static int fill_task_folder(Menu *m, int desk, int flags)
{
    en en;
    en.m = m, en.desk = desk, en.hwndTop = GetActiveTaskWindow(), en.flags = flags, en.i = en.n = 0;
    EnumTasks(task_enum_func, (LPARAM)&en);
    return en.n;
}

static Menu * build_task_folder(int desk, const char *title, bool popup)
{
    Menu *m;
    char buf[100];

    sprintf(buf, (-1 == desk)
        ? "Core_tasks_icons" : "Core_tasks_workspace%d", desk+1);
    m = makeNamedMenu(title, buf, popup);
    if (m) fill_task_folder(m, desk, e_alltasks);
    return m;
}

//===========================================================================
Menu * MakeTaskFolder(int n, bool popup)
{
    DesktopInfo DI;
    string_node* sn;

    getDesktopInfo(&DI);
    if (n < 0 || n >= DI.ScreensX)
        return NULL;

    sn = (string_node *)nth_node(DI.deskNames, n);
    return build_task_folder(n, sn->str, popup);
}

//===========================================================================
Menu* MakeDesktopMenu(int mode, bool popup)
{
    DesktopInfo DI;
    string_node *sl;
    int n, d;
    Menu *m, *s, *i = NULL;

    if (mode == 2) {
        m = makeNamedMenu(NLS0("Tasks"), "Core_tasks_menu", popup);
    } else {
        i = build_task_folder(-1, NLS0("Icons"), popup);
        if (mode == 1)
            return i;
        m = makeNamedMenu(NLS0("Workspaces"), "Core_tasks_workspaces", popup);
    }
    getDesktopInfo(&DI);
    for (n = 0, d = DI.ScreensX, sl = DI.deskNames; n < d; ++n, sl = sl->next) {
        if (mode == 0) {
            char buf[100];
            MenuItem *fi;

            fi = makeSubmenu(m, build_task_folder(n, sl->str, popup), NULL);
            sprintf(buf, "@BBCore.SwitchToWorkspace %d", n+1);
            menuItemOption(fi, BBMENUITEM_LCOMMAND, buf);
            if (n == DI.number)
                menuItemOption(fi, BBMENUITEM_CHECKED);
        } else {
            fill_task_folder(m, n, e_alltasks);
            if (n == d-1) return m;
            makeMenuNOP(m, NULL);
        }
    }
    makeMenuNOP(m, NULL);
    makeSubmenu(m, i, NULL);
    s = makeNamedMenu(NLS0("New/Remove"), "Core_workspaces_setup", popup);
    makeMenuItem(s, NLS0("New Workspace"), "@BBCore.AddWorkspace", false);
    if (d > 1) makeMenuItem(s, NLS0("Remove Last"), "@BBCore.DelWorkspace", false);
    makeMenuItemString(s, NLS0("Workspace Names"), "@BBCore.SetWorkspaceNames", Settings_workspaceNames);
    makeSubmenu(m, s, NULL);
    return m;
}

//===========================================================================
static BOOL CALLBACK recoverwindow_enum_proc(HWND hwnd, LPARAM lParam)
{
    char windowtext[100];
    char classname[100];
    char appname[100];
    char broam[100];
    char text[400];
    RECT r;
    MenuItem *mi;

    if (IsWindowEnabled(hwnd) && (IsIconic(hwnd)
        || (GetClientRect(hwnd, &r) && r.right && r.bottom))
        && GetWindowText(hwnd, windowtext, sizeof text))
    {
        classname[0] = 0;
        //GetClassName(hwnd, classname, sizeof classname);
        getAppByWindow(hwnd, appname);

        sprintf (broam, "@BBCore.RecoverWindow %p", hwnd);
        sprintf (text, "%s : \"%s\"", appname, windowtext);
        mi = makeMenuItem((Menu*)lParam, text, broam, FALSE != IsWindowVisible(hwnd));
        menuItemOption(mi, BBMENUITEM_JUSTIFY, DT_LEFT);
    }
    return TRUE;
}

Menu *MakeRecoverMenu(bool popup)
{
    Menu *m = makeNamedMenu(NLS0("Toggle Window Visibilty"), "Core_tasks_recoverwindows", popup);
    EnumWindows(recoverwindow_enum_proc, (LPARAM)m);
    menuOption(m, BBMENU_MAXWIDTH|BBMENU_SORT|BBMENU_CENTER|BBMENU_PINNED|BBMENU_ONTOP, 400);
    return m;
}

void ShowRecoverMenu(void)
{
    Workspaces_GatherWindows();
    if (Settings_altMethod) {
        showMenu(MakeRecoverMenu(true));
    } else {
        bbMessageBox(MB_OK, "Windows gathered in current workspace.");
    }
}

}; //!namespace bbcore
