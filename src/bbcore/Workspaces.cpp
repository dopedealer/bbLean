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
#include "SettingsBbox.h"
#include "Workspaces.h"
#include "Desk.h"
#include "BBVWM.h"
#include "MessageManager.h" 
#include "rcfile.h"
#include "nls.h"
#include "system.h"

namespace bbcore {

//====================
// public variables

int VScreenX, VScreenY, VScreenWidth, VScreenHeight;
int currentScreen;
int lastScreen;
// total number of desktops
int nScreens;

//====================
// private variables

struct toptask {
    toptask* next;
    tasklist* task;
}; 

// workspace names
static string_node* deskNames;

// application listed in StickyWindows.ini
static string_node* stickyNamesList;

// the list of tasks, in order as they were added
static tasklist* gTaskList;

// the list of tasks, in order as they were recently active
static toptask* pTopTask;

// the current active taskwindow or NULL
static HWND activeTaskWindow;

// minimized windows by 'MinimizeAllWindows'
static list_node *toggled_windows; 

//


struct sticky_list
{
    sticky_list* next;
    HWND hwnd;
};

//====================
// private variables

// Sticky plugins & apps
static sticky_list* gStickylist;


DWORD_PTR send_bbls_command(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    DWORD_PTR result = 0;
    SendMessageTimeout(hwnd,
        RegisterWindowMessage(BBLEANSKIN_MSG), wParam, lParam,
        SMTO_ABORTIFHUNG|SMTO_NORMAL, 1000, (DWORD_PTR*)&result);
    return result;
}


void setDesktop(HWND hwnd, int n, bool switchto)
{
    vwm_set_desk(hwnd, n, switchto);
}

/// \brief Make a plugin/app window appear on all workspaces
///        This is now one API for both plugins and application windows,
///        still internally uses different methods
void makeSticky(HWND hwnd)
{
    sticky_list* p{};

    if (FALSE == IsWindow(hwnd))
    {
        return;
    }

    if (is_bbwindow(hwnd))
    {

        if (assoc(gStickylist, hwnd))
        {
            return;
        }
        p = (sticky_list*)m_alloc(sizeof(sticky_list));
        cons_node(&gStickylist, p);
        p->hwnd = hwnd;
        //dbg_window(hwnd, "[+%d]", listlen(gStickylist));

    }
    else
    { 
        if (vwm_get_desk(hwnd) != currentScreen)
        {
            setDesktop(hwnd, currentScreen, false);
        }
        vwm_set_sticky(hwnd, true);
        send_bbls_command(hwnd, BBLS_SETSTICKY, 1);
        //dbg_window(hwnd, "[+app]");
    }
} 

void removeSticky(HWND hwnd)
{
    sticky_list **pp, *p;

    pp = (sticky_list**)assoc_ptr(&gStickylist, hwnd);
    if (pp)
    { 
        *pp = (p = *pp)->next;
        m_free(p);
        //dbg_window(hwnd, "[-%d]", listlen(gStickylist));

    }
    else if (vwm_set_sticky(hwnd, false))
    {
        send_bbls_command(hwnd, BBLS_SETSTICKY, 0);
        //dbg_window(hwnd, "[-app]");
    }
}

bool checkSticky(HWND hwnd)
{
    return check_sticky_plugin(hwnd) || vwm_get_status(hwnd, VWM_STICKY);
}

/// \brief get the size
/// \returns The number of currently registered tasks
int getTaskListSize(void)
{
    return listlen(gTaskList);
}

/// \brief get a task's HWND by index [0..getTaskListSize()]
/// \returns The HWND of the task by index
HWND getTask(int index)
{
    tasklist* tl = (tasklist *)nth_node(gTaskList, index);
    return tl ? tl->hwnd : NULL;
}

/// \brief get the index of the currently active task
/// \returns Index of current active task or -1, if none or BB
int getActiveTask(void)
{
    tasklist* tl;
    int i = 0;
    dolist (tl, gTaskList) {
        if (tl->active)
            return i;
        i++;
    }
    return -1;
}

/// \brief Get the workspace number for a task
/// \returns The workspace of the task by HWND
int getTaskWorkspace(HWND hwnd)
{
    return vwm_get_desk(hwnd);
}

/// \brief Set the workspace of the task by HWND
/// For backwards compatibility only. (because of unclear
/// implementation)
void setTaskWorkspace(HWND hwnd, int wkspc)
{
    vwm_set_workspace(hwnd, wkspc);
}

/// \brief Direct access: get the internal TaskList.
/// \returns The raw task-list
const tasklist* getTaskListPtr(void)
{
    return gTaskList;
}

void get_desktop_info(DesktopInfo* deskInfo, int i)
{
    string_node *sp;
    deskInfo->isCurrent = i == currentScreen;
    deskInfo->number = i;
    deskInfo->name[0] = 0;
    deskInfo->ScreensX = nScreens;
    deskInfo->deskNames = deskNames;

    sp = (string_node*)nth_node(deskNames, i);
    if (sp)
    {
        strcpy(deskInfo->name, sp->str);
    }
}

/// \brief Get current Desktop information
void getDesktopInfo(DesktopInfo* deskInfo)
{
    get_desktop_info(deskInfo, currentScreen);
}

/// \brief Get workspace and original position/size for window.
/// Retrieve the desktop and the original coords for a window
bool getTaskLocation(HWND hwnd, taskinfo* t)
{
    return vwm_get_location(hwnd, t);
}

/// \brief Set workspace and/or position for window.
/// Move a window and it's popups to another desktop and/or position
bool setTaskLocation(HWND hwnd, taskinfo* t, UINT flags)
{
    bool is_top = hwnd == activeTaskWindow;
    if (false == vwm_set_location(hwnd, t, flags))
        return false;
    if (is_top && vwm_get_desk(hwnd) != currentScreen)
        focus_top_window();
    return true;
}



//====================
// local functions

static void WS_LoadStickyNamesList(void);

static int next_desk (int d);
static void MoveWindowToWkspc(HWND, int desk, bool switchto);
static void NextWindow(bool, int);

static void switchToDesktop(int desk);

static void SetWorkspaceNames(const char *names);
static void AddDesktop(int);
static void SetNames(void);

static void exit_tasks(void);
static void init_tasks(void);

static int is_valid_task(HWND hwnd);
static int FindTask(HWND hwnd);
static void SetTopTask(tasklist*, int f);
static HWND get_top_window(int scrn);

static void WS_ShadeWindow(HWND hwnd);
static void WS_GrowWindowHeight(HWND hwnd);
static void WS_GrowWindowWidth(HWND hwnd);
static void WS_LowerWindow(HWND hwnd);
static void WS_RaiseWindow(HWND hwnd);
static void WS_MaximizeWindow(HWND hwnd);
static void WS_MinimizeWindow(HWND hwnd);
static void WS_CloseWindow(HWND hwnd);
static void WS_RestoreWindow(HWND hwnd);
static void WS_BringToFront(HWND hwnd, bool to_current);

static void MinimizeAllWindows(void);
static void RestoreAllWindows(void);

void send_desk_refresh(void);
void send_task_refresh(void);

//===========================================================================

void Workspaces_Init(int nostartup)
{
    currentScreen   = 0;
    lastScreen      = 0;
    deskNames       = NULL;
    stickyNamesList = NULL;

    SetNames();
    WS_LoadStickyNamesList();
    Workspaces_GetScreenMetrics();
    vwm_init();
    if (nostartup)
        init_tasks();
}

void Workspaces_Exit(void)
{
    exit_tasks();
    vwm_exit();
    freeall(&deskNames);
    freeall(&stickyNamesList);
    // not neccesary if all plugins properly call 'removeSticky':
    freeall(&gStickylist);
}

void Workspaces_Reconfigure(void)
{
    bool changed;
    SetNames();
    WS_LoadStickyNamesList();
    // force reorder on resolution changes
    changed = Workspaces_GetScreenMetrics();
    vwm_reconfig(changed);
}

bool Workspaces_GetScreenMetrics(void)
{
    int x, y, w, h;
    bool changed;
    if (bbGetMultimon())
    {
        x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
    else
    {
        x = y = 0;
        w = GetSystemMetrics(SM_CXSCREEN);
        h = GetSystemMetrics(SM_CYSCREEN);
    }
    changed = w != VScreenWidth || h != VScreenHeight;
    VScreenWidth = w;
    VScreenHeight = h;
    VScreenX = x;
    VScreenY = y;
    nScreens = Settings_disableVWM ? 1 : Settings_workspaces;
    //debug_printf("Screen: %d/%d %d/%d", x, y, w, h);
    return changed;
}

//===========================================================================

static
void SetWorkspaceNames(const char *names)
{
    if (names)
    {
        strcpy(Settings_workspaceNames, names);
    }
    else if (IDOK != EditBox(
        BBAPPNAME,
        NLS2("$Workspace_EditNames$", "Workspace Names:"),
        Settings_workspaceNames,
        Settings_workspaceNames
        ))
    {
        return;
    }

    gSettingBbox->writeRCSetting(&Settings_workspaceNames);
    SetNames();
    send_desk_refresh();
}

static
void SetNames(void)
{
    const char *names; int i;

    freeall(&deskNames);
    names = Settings_workspaceNames;
    for (i = 0; i < Settings_workspaces; ++i)
    {
        char wkspc_name[MAX_PATH];
        if (0 == *NextToken(wkspc_name, &names, ","))
            sprintf(wkspc_name,
                NLS2("$Workspace_DefaultName$", "Workspace %d"), i+1);
        append_string_node(&deskNames, wkspc_name);
    }
}

//===========================================================================
static
HWND get_default_window(HWND hwnd)
{
    if (NULL == hwnd) {
        hwnd = GetForegroundWindow();
        if (NULL == hwnd || is_bbwindow(hwnd)) {
            hwnd = get_top_window(currentScreen);
        }
    }
    return hwnd;
}

static
LRESULT send_syscommand(HWND hwnd, WPARAM SC_XXX)
{
    DWORD_PTR dwResult = 0;
    SendMessageTimeout(hwnd, WM_SYSCOMMAND, SC_XXX, 0, SMTO_ABORTIFHUNG|SMTO_NORMAL, 1000, &dwResult);
    return dwResult;
} 

static
BOOL list_desktops_func(DesktopInfo *DI, LPARAM lParam)
{
    SendMessage((HWND)lParam, BB_DESKTOPINFO, 0, (LPARAM)DI);
    return TRUE;
}

/*
static
void post_message_if_needed(UINT message, WPARAM wParam, LPARAM lParam)
{
    MSG msg;
    if (!PeekMessage(&msg, gBBhwnd, message, message, PM_NOREMOVE))
        PostMessage(gBBhwnd, message, wParam, lParam);
}
*/

static
void send_task_message(HWND hwnd, UINT msg)
{
    SendMessage(gBBhwnd, BB_TASKSUPDATE, (WPARAM)hwnd, msg);
}

void send_task_refresh(void)
{
    send_task_message(NULL, TASKITEM_REFRESH);
}

void send_desk_refresh(void)
{
    DesktopInfo DI;
    get_desktop_info(&DI, currentScreen);
    SendMessage(gBBhwnd, BB_DESKTOPINFO, 0, (LPARAM)&DI);
}

//===========================================================================
LRESULT Workspaces_Command(UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND hwnd = (HWND)lParam;
    LONG style;

    switch (msg)
    {
        case BB_SWITCHTON:
            Workspaces_DeskSwitch((int)lParam);
            break;

        case BB_LISTDESKTOPS:
            if (wParam)
                EnumDesks(list_desktops_func, wParam);
            break;

        case BB_MOVEWINDOWTON:
        case BB_SENDWINDOWTON:
            MoveWindowToWkspc(get_default_window(hwnd), (int)wParam, msg == BB_MOVEWINDOWTON);
            break;

        //====================
        case BB_BRINGTOFRONT:
            if (hwnd)
                WS_BringToFront(hwnd, 0 != (wParam & BBBTF_CURRENT));
            else
                focus_top_window();
            break;

        //====================
        case BB_WORKSPACE:
            switch (wParam)
            {
                // ---------------------------------
                case BBWS_DESKLEFT:
                    Workspaces_DeskSwitch(next_desk(-1));
                    break;
                case BBWS_DESKRIGHT:
                    Workspaces_DeskSwitch(next_desk(1));
                    break;

                // ---------------------------------
                case BBWS_ADDDESKTOP:
                    AddDesktop(1);
                    break;
                case BBWS_DELDESKTOP:
                    AddDesktop(-1);
                    break;

                // ---------------------------------
                case BBWS_SWITCHTODESK:
                    Workspaces_DeskSwitch((int)lParam);
                    break;

                case BBWS_LASTDESK:
                    Workspaces_DeskSwitch(lastScreen);
                    break;

                case BBWS_GATHERWINDOWS:
                    Workspaces_GatherWindows();
                    break;

                // ---------------------------------
                case BBWS_MOVEWINDOWLEFT:
                    MoveWindowToWkspc(get_default_window(hwnd), next_desk(-1), true);
                    break;

                case BBWS_MOVEWINDOWRIGHT:
                    MoveWindowToWkspc(get_default_window(hwnd), next_desk(1), true);
                    break;

                // ---------------------------------
                case BBWS_PREVWINDOW:
                    NextWindow(0 != lParam /*true for all workspaces*/, -1);
                    break;

                case BBWS_NEXTWINDOW:
                    NextWindow(0 != lParam /*true for all workspaces*/, 1);
                    break;

                // ---------------------------------
                case BBWS_MAKESTICKY:
                    hwnd = get_default_window(hwnd);
                    if (NULL == hwnd)
                        break;
                    makeSticky(hwnd);
                    break;

                case BBWS_CLEARSTICKY:
                    hwnd = get_default_window(hwnd);
                    if (NULL == hwnd)
                        break;
                    removeSticky(hwnd);
                    break;

                case BBWS_TOGGLESTICKY:
                    hwnd = get_default_window(hwnd);
                    if (NULL == hwnd)
                        break;
                    if (checkSticky(hwnd))
                        removeSticky(hwnd);
                    else
                        makeSticky(hwnd);
                    break;

                case BBWS_TOGGLEONTOP:
                    hwnd = get_default_window(hwnd);
                    if (NULL == hwnd)
                        break;
                    SetWindowPos(hwnd,
                        (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
                        ? HWND_NOTOPMOST : HWND_TOPMOST,
                        0, 0, 0, 0,
                        SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

                    send_bbls_command(hwnd, BBLS_REDRAW, 0);
                    break;

                case BBWS_ISSTICKY:
                    return checkSticky(hwnd);

                case BBWS_GETTOPWINDOW:
                    return (LRESULT)get_default_window(hwnd);

                // ---------------------------------
                case BBWS_EDITNAME:
                    SetWorkspaceNames((const char *)lParam);
                    break;

                case BBWS_MINIMIZEALL:
                    MinimizeAllWindows();
                    break;

                case BBWS_RESTOREALL:
                    RestoreAllWindows();
                    break;

                case BBWS_CASCADE:
                    CascadeWindows(NULL, 0, NULL, 0, NULL);
                    break;

                case BBWS_TILEVERTICAL:
                    TileWindows(NULL, MDITILE_VERTICAL, NULL, 0, NULL);
                    break;

                case BBWS_TILEHORIZONTAL:
                    TileWindows(NULL, MDITILE_HORIZONTAL, NULL, 0, NULL);
                    break;

                default:
                    break;
            }
            break;

        //====================
        default:
            hwnd = get_default_window(hwnd);
            if (NULL == hwnd)
                break;

            if (currentScreen != vwm_get_desk(hwnd)) {
                if (BB_WINDOWCLOSE == msg)
                    WS_BringToFront(hwnd, true);
                else
                    break;
            }

            style = GetWindowLong(hwnd, GWL_STYLE);
            switch (msg)
            {
                case BB_WINDOWCLOSE:
                    WS_CloseWindow(hwnd);
                    break;

                case BB_WINDOWMINIMIZE:
                    if (0 == (WS_MINIMIZEBOX & style))
                        break;
                    WS_MinimizeWindow(hwnd);
                    break;

                case BB_WINDOWRESTORE:
                    WS_RestoreWindow(hwnd);
                    break;

                case BB_WINDOWMAXIMIZE:
                    if (0 == (WS_MAXIMIZEBOX & style))
                        break;
                    WS_MaximizeWindow(hwnd);
                    break;

                case BB_WINDOWGROWHEIGHT:
                    if (0 == (WS_MAXIMIZEBOX & style))
                        break;
                    WS_GrowWindowHeight(hwnd);
                    break;

                case BB_WINDOWGROWWIDTH:
                    if (0 == (WS_MAXIMIZEBOX & style))
                        break;
                    WS_GrowWindowWidth(hwnd);
                    break;

                case BB_WINDOWLOWER:
                    WS_LowerWindow(hwnd);
                    break;

                case BB_WINDOWRAISE:
                    WS_RaiseWindow(hwnd);
                    break;

                case BB_WINDOWSHADE:
                    if (0 == (WS_SIZEBOX & style))
                        break;
                    WS_ShadeWindow(hwnd);
                    break;

                case BB_WINDOWSIZE:
                    if (0 == (WS_SIZEBOX & style))
                        break;
                    send_syscommand(hwnd, SC_SIZE);
                    break;

                case BB_WINDOWMOVE:
                    send_syscommand(hwnd, SC_MOVE);
                    break;
            }
    }
    return -1;
}

//===========================================================================

static
void WS_BringToFront(HWND hwnd, bool to_current)
{
    int windesk;

    CleanTasks();

    windesk = vwm_get_desk(hwnd);
    if (windesk != currentScreen)
    {
        if (false == to_current)
        {
            switchToDesktop(windesk);
        }
        else
        {
            setDesktop(hwnd, currentScreen, false);
        }
    }
    SwitchToWindow(hwnd);
}

//===========================================================================

void SwitchToWindow (HWND hwnd_app)
{
    HWND hwnd = GetLastActivePopup(GetRootWindow(hwnd_app));
    if (have_imp(pSwitchToThisWindow))
    {
        // this one also restores the window, if it's iconic:
        pSwitchToThisWindow(hwnd, 1);
    }
    else
    {
        SetForegroundWindow(hwnd);
        if (IsIconic(hwnd))
            send_syscommand(hwnd, SC_RESTORE);
    }
}

void SwitchToBBWnd (void)
{
    ForceForegroundWindow(gBBhwnd);
    // sometimes the shell notification doesnt seem to work correctly:
    PostMessage(gBBhwnd, WM_ShellHook, HSHELL_WINDOWACTIVATED, 0);
}

//===========================================================================

static
void get_rect(HWND hwnd, RECT *rp)
{
    GetWindowRect(hwnd, rp);
    if (WS_CHILD & GetWindowLong(hwnd, GWL_STYLE))
    {
        HWND pw = GetParent(hwnd);
        ScreenToClient(pw, (LPPOINT)&rp->left);
        ScreenToClient(pw, (LPPOINT)&rp->right);
    }
}

static
void window_set_pos(HWND hwnd, RECT rc)
{
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    SetWindowPos(hwnd, NULL,
        rc.left, rc.top, width, height,
        SWP_NOZORDER|SWP_NOACTIVATE);
}

static
int get_shade_height(HWND hwnd)
{
    int shade, border, caption;

    shade = (int)send_bbls_command(hwnd, BBLS_GETSHADEHEIGHT, 0);
    //debug_printf("BBLS_GETSHADEHEIGHT: %d", shade);
    if (shade)
        return shade;

    border = GetSystemMetrics(
        (WS_SIZEBOX & GetWindowLong(hwnd, GWL_STYLE))
        ? SM_CYFRAME
        : SM_CYFIXEDFRAME);

    caption = GetSystemMetrics(
        (WS_EX_TOOLWINDOW & GetWindowLong(hwnd, GWL_EXSTYLE))
        ? SM_CYSMCAPTION
        : SM_CYCAPTION);

    //debug_printf("caption %d  border %d", caption, border);
    return 2*border + caption;
}

static
void WS_ShadeWindow(HWND hwnd)
{
    RECT rc;
    int h1, h2, height;
    HANDLE prop;

    get_rect(hwnd, &rc);
    height = rc.bottom - rc.top;
    prop = GetProp(hwnd, BBSHADE_PROP);

    h1 = LOWORD(prop);
    h2 = HIWORD(prop);
    if (IsZoomed(hwnd)) {
        if (h2) height = h2, h2 = 0;
        else h2 = height, height = get_shade_height(hwnd);
    } else {
        if (h1) height = h1, h1 = 0;
        else h1 = height, height = get_shade_height(hwnd);
        h2 = 0;
    }

    prop = (HANDLE)MAKELPARAM(h1, h2);
    if (0 == prop) RemoveProp(hwnd, BBSHADE_PROP);
    else SetProp(hwnd, BBSHADE_PROP, prop);

    rc.bottom = rc.top + height;
    window_set_pos(hwnd, rc);
}

//===========================================================================
static
bool check_for_restore(HWND hwnd)
{
    WINDOWPLACEMENT wp;

    if (FALSE == IsZoomed(hwnd))
        return false;
    send_syscommand(hwnd, SC_RESTORE);

    // restore the default maxPos (necessary when it was V-max'd or H-max'd)
    wp.length = sizeof wp;
    GetWindowPlacement(hwnd, &wp);
    wp.ptMaxPosition.x =
    wp.ptMaxPosition.y = -1;
    SetWindowPlacement(hwnd, &wp);
    return true;
}

static
void grow_window(HWND hwnd, bool v)
{
    RECT r1, r2;

    if (check_for_restore(hwnd))
        return;
    get_rect(hwnd, &r1);
    LockWindowUpdate(hwnd);
    send_syscommand(hwnd, SC_MAXIMIZE);
    get_rect(hwnd, &r2);
    if (v)
        r1.top = r2.top, r1.bottom = r2.bottom;
    else
        r1.left = r2.left, r1.right = r2.right;
    window_set_pos(hwnd, r1);
    LockWindowUpdate(NULL);
}

static
void WS_GrowWindowHeight(HWND hwnd)
{
    grow_window(hwnd, true);
}

static
void WS_GrowWindowWidth(HWND hwnd)
{
    grow_window(hwnd, false);
}

static
void WS_MaximizeWindow(HWND hwnd)
{
    if (check_for_restore(hwnd))
        return;
    send_syscommand(hwnd, SC_MAXIMIZE);
}

static
void WS_RestoreWindow(HWND hwnd)
{
    if (check_for_restore(hwnd))
        return;
    send_syscommand(hwnd, SC_RESTORE);
}

static
void WS_MinimizeWindow(HWND hwnd)
{
    ::AllowSetForegroundWindow(ASFW_ANY);
    send_syscommand(hwnd, SC_MINIMIZE);
}

static
void WS_CloseWindow(HWND hwnd)
{
    send_syscommand(hwnd, SC_CLOSE);
    PostMessage(gBBhwnd, BB_BRINGTOFRONT, 0, 0);
}

static
void WS_RaiseWindow(HWND hwnd_notused)
{
    tasklist* tl = NULL;
    toptask* lp;
    dolist (lp, pTopTask)
        if (currentScreen == lp->task->wkspc)
            tl = lp->task;
    if (tl)
        WS_BringToFront(tl->hwnd, false);
}

static
void WS_LowerWindow(HWND hwnd)
{
    tasklist* tl;
    SwitchToBBWnd();
    if (pTopTask)
    {
        tl = pTopTask->task;
        SetTopTask(tl, 2); // append
        if (tl != pTopTask->task)
        {
            focus_top_window();
        }
    }
    vwm_lower_window(hwnd);
}


// export to BBVWM.cpp
bool check_sticky_plugin(HWND hwnd)
{
    if (assoc(gStickylist, hwnd))
        return true;
    // bbPager is the only known plugin that still uses that sticky method:
    if (GetWindowLongPtr(hwnd, GWLP_USERDATA) == 0x49474541 /*magicDWord*/)
        return true;
    return false;
}

//===========================================================================
static
void WS_LoadStickyNamesList(void)
{
    char path[MAX_PATH];
    char buffer[MAX_PATH];
    FILE* fp;

    freeall(&stickyNamesList);
    findRCFile(path, "StickyWindows.ini", NULL);
    fp = fileOpen(path);
    if (fp) {
        while (readNextCommand(fp, buffer, sizeof (buffer)))
            append_string_node(&stickyNamesList, strlwr(buffer));
        fileClose(fp);
    }
}

// export to BBVWM.cpp
bool check_sticky_name(HWND hwnd)
{
    string_node* sl;
    char appName[MAX_PATH];
    if (NULL == stickyNamesList || 0 == getAppByWindow(hwnd, appName))
        return false;
    strlwr(appName);
    dolist (sl, stickyNamesList)
        if (0==strcmp(appName, sl->str))
            return true;
    return false;
}

//===========================================================================
// Functions: Minimize/Restore All Windows
//===========================================================================
struct mr_info { list_node *p; int cmd; bool iconic; };

static
bool mr_checktask(HWND hwnd)
{
    tasklist* tl = (tasklist *)assoc(gTaskList, hwnd);
    return tl && tl->wkspc == currentScreen;
}

static
BOOL CALLBACK mr_enumproc(HWND hwnd, LPARAM lParam)
{
    if (mr_checktask(hwnd)) {
        mr_info* mr = (mr_info*)lParam;
        if (mr->iconic == (FALSE != IsIconic(hwnd)))
            cons_node (&mr->p, new_node(hwnd));
    }
    return TRUE;
}

static
void min_rest_helper(int cmd)
{
    mr_info mri, *mr = &mri;
    list_node **pp, *p;

    mr->p = NULL;
    mr->iconic = SC_RESTORE == cmd;
    EnumWindows(mr_enumproc, (LPARAM)mr);

    if (SC_RESTORE == cmd)
        reverse_list(&mr->p);

    if (0 == cmd) {
        cmd = SC_MINIMIZE;
        if (NULL == mr->p) {
            for (pp = &toggled_windows; NULL != (p = *pp); ) {
                if (mr_checktask((HWND)p->v))
                    *pp = p->next, cons_node(&mr->p, p);
                else
                    pp = &p->next;
            }
            cmd = SC_RESTORE;
        }
    }

    mr->cmd = cmd;
    freeall(&toggled_windows);
    dolist (p, mr->p) {
        HWND hwnd = (HWND)p->v;
        if (SC_MINIMIZE == mr->cmd)
            cons_node (&toggled_windows, new_node(hwnd));
        send_syscommand(hwnd, mr->cmd);
    }
    freeall(&mr->p);
}

static
void MinimizeAllWindows(void)
{
    min_rest_helper(0); //SC_MINIMIZE);
}

static
void RestoreAllWindows(void)
{
    min_rest_helper(SC_RESTORE);
}

//================================================================
// get the top window in the z-order of the current workspace

static
HWND get_top_window(int scrn)
{
    toptask* lp;
    dolist (lp, pTopTask)
        if (scrn == lp->task->wkspc) {
            HWND hwnd = lp->task->hwnd;
            if (FALSE == IsIconic(hwnd))
                return hwnd;
        }
    return NULL;
}

//================================================================
// activate the topwindow in the z-order of the current workspace

bool focus_top_window(void)
{
    HWND hw = get_top_window(currentScreen);
    if (hw) {
        SwitchToWindow(hw);
        return true;
    }
    SwitchToBBWnd();
    return false;
}

//================================================================
// is this window a valid task

static
int is_valid_task(HWND hwnd)
{
    if (FALSE == IsWindow(hwnd))
        return 0;

    if (IsWindowVisible(hwnd))
        return 1;

    if (Settings_altMethod && vwm_get_status(hwnd, VWM_MOVED))
        return 2;

    return 0;
}

//===========================================================================
// gather windows in current WS
void Workspaces_GatherWindows(void)
{   
    vwm_gather();
}

//===========================================================================
// the internal switchToDesktop
static
void switchToDesktop(int n)
{
    // steel focus and wait for apps to close their menus, because that
    // could leave defunct dropshadows on the screen otherwise
    SwitchToBBWnd();
    BBSleep(10);
    vwm_switch(n);
} 

//===========================================================================
static
int next_desk (int d)
{
    int n = currentScreen + d;
    int m = nScreens - 1;
    if (n > m) return 0;
    if (n < 0) return m;
    return n;
}

static
void AddDesktop(int d)
{
    Settings_workspaces = imax(1, Settings_workspaces + d);

    gSettingBbox->writeRCSetting(&Settings_workspaces);

    Workspaces_Reconfigure();
    send_desk_refresh();
}

//====================
void Workspaces_DeskSwitch(int i)
{   
    HWND hwnd;

    //debug_printf("DeskSwitch %d -> %d", currentScreen, i);

    if (i == currentScreen || i < 0 || i >= nScreens)
        return;

    if (activeTaskWindow && vwm_get_status(activeTaskWindow, VWM_STICKY))
        hwnd = activeTaskWindow;
    else
        hwnd = get_top_window(i);

    switchToDesktop(i);
    if (hwnd)
        SwitchToWindow(hwnd);
    else
        SwitchToBBWnd();
}

//====================

static
void MoveWindowToWkspc(HWND hwnd, int desk, bool switchto)
{
    if (NULL == hwnd)
    {
        return;
    }

    removeSticky(hwnd);

    if (switchto)
    {
        SwitchToWindow(hwnd);
        setDesktop(hwnd, desk, true);

    } else
    {
        SwitchToBBWnd();
        setDesktop(hwnd, desk, false);
        focus_top_window();
    }
}

//====================

static
void NextWindow(bool allDesktops, int dir)
{
    int s,i,j;
    tasklist* tl;
    s = getTaskListSize();
    if (0==s) return;
    i = FindTask(pTopTask->task->hwnd);
    if (-1==i) i=0;
    j = i;
    do {
        if (dir>0) {
            i++;
            if (s==i) i=0;
        } else {
            if (0==i) i=s;
            i--;
        }
        tl = (tasklist *)nth_node(gTaskList, i);
        if (tl && (allDesktops || currentScreen == tl->wkspc)
            && FALSE == IsIconic(tl->hwnd)) {
            PostMessage(gBBhwnd, BB_BRINGTOFRONT, 0, (LPARAM)tl->hwnd);
            return;
        }
    } while (j!=i);
}

//===========================================================================
void ToggleWindowVisibility(HWND hwnd)
{
    if (IsWindow(hwnd)) {
        ShowWindowAsync(hwnd, IsWindowVisible(hwnd) ? SW_HIDE : SW_SHOWNA);
        send_task_refresh();
    }
}

//===========================================================================

//===========================================================================
// Task - Support

static
void del_from_toptasks(tasklist *tl)
{
    toptask **lpp, *lp;
    lpp = (toptask**)assoc_ptr(&pTopTask, tl);
    if (lpp)
        *lpp=(lp=*lpp)->next, m_free(lp);
}

static
void SetTopTask(tasklist* tl, int set_where)
{
    if (NULL==tl)
        return;
    del_from_toptasks(tl);
    if (0==set_where)
        return; // delete_only
    if (1==set_where) // push at front
        cons_node(&pTopTask, new_node(tl));
    if (2==set_where) // push at end
        append_node(&pTopTask, new_node(tl));
}

static
void get_caption(tasklist* tl, int force)
{
    if (force || 0 == tl->caption[0])
        get_window_text(tl->hwnd, tl->caption, sizeof tl->caption);
    if (force || NULL == tl->icon)
        get_window_icon(tl->hwnd, &tl->icon);
}

HWND GetActiveTaskWindow(void)
{
    return activeTaskWindow;
}

void Workspaces_GetCaptions()
{
    tasklist* tl;
    dolist (tl, gTaskList)
        get_caption(tl, 1);
}

//==================================

static
tasklist* AddTask(HWND hwnd)
{
    tasklist* tl = c_new(tasklist);
    tl->hwnd = hwnd;
    tl->wkspc = currentScreen;
    append_node(&gTaskList, tl);
    get_caption(tl, 1);
    send_task_message(hwnd, TASKITEM_ADDED);
    return tl;
}

static
void RemoveTask(tasklist* tl)
{
    HWND hwnd;
    if (tl->icon)
        DestroyIcon(tl->icon);
    del_from_toptasks(tl);
    hwnd = tl->hwnd;
    remove_item(&gTaskList, tl);
    send_task_message(hwnd, TASKITEM_REMOVED);
}

static
int FindTask(HWND hwnd)
{
    tasklist* tl; int i = 0;
    dolist(tl, gTaskList) {
        if (tl->hwnd == hwnd)
            return i;
        i++;
    }
    return -1;
}

// run through the tasklist and remove invalid tasks
void CleanTasks(void)
{
    tasklist** tl = &gTaskList;
    while (*tl)
        if (is_valid_task((*tl)->hwnd))
            tl = &(*tl)->next;
        else
            RemoveTask(*tl);
}

//==================================
static
BOOL CALLBACK TaskProc(HWND hwnd, LPARAM lParam)
{
    if (isAppWindow(hwnd))
    {
        SetTopTask(AddTask(hwnd), 2);
    }
    return TRUE;
}

static
void init_tasks(void)
{
    EnumWindows(TaskProc, 0);
}

static
void exit_tasks(void)
{
    while (gTaskList)
        RemoveTask(gTaskList);
    freeall(&toggled_windows);
}

//==================================
/* Set workspace number from vwm */
#if 0
void workspaces_set_desk(void)
{
    tasklist* tl;
    dolist (tl, taskList)
        tl->wkspc = vwm_get_desk(tl->hwnd);
}
#else
/* Set workspace number from vwm and reorder the tasklist such that
   tasks on higher workspace come after tasks on lower ones */
void workspaces_set_desk(void)
{
    tasklist *tl, *tn, **tpp, *tr = NULL;
    for (tl = gTaskList; tl; tl = tn) {
        tl->wkspc = vwm_get_desk(tl->hwnd);
        for (tpp = &tr; *tpp && (*tpp)->wkspc <= tl->wkspc;)
            tpp = &(*tpp)->next;
        tn = tl->next, tl->next = *tpp, *tpp = tl;
    }
    gTaskList = tr;
}
#endif

//===========================================================================
#if 0
static
void debug_tasks(WPARAM wParam, HWND hwnd, int is_task)
{
    static const char * const actions[] = {
        "null", "add", "remove", "activateshell",
        "activate", "minmax", "redraw", "taskman",
        "language", "sysmenu", "endtask", NULL,
        NULL, "replaced", "replacing"
    };

    int n = wParam & 0x7FFF;
    char buffer[MAX_PATH];
    const char *msg = n < array_count(actions) ? actions[n] : NULL;
    if (!msg) msg = "xxx";
    getAppByWindow(hwnd, buffer);
    debug_printf("msg %d [%s] hwnd=%x task=%d app=%s desk=%d",
        wParam, msg, hwnd, is_task, buffer, vwm_get_desk(hwnd));
}
#else
#define debug_tasks(a,b,c)
#endif

//===========================================================================
// called from the main windowproc on the registered WM_ShellHook message

void Workspaces_TaskProc(WPARAM wParam, HWND hwnd)
{
    tasklist* tl{};
    static HWND hwnd_replacing;

    if (hwnd) {
        tl = (tasklist *)assoc(gTaskList, hwnd);
        if (tl)
            tl->flashing = false;
    } else {
        tl = NULL;
    }

    debug_tasks(wParam, hwnd, NULL != tl);

    switch (wParam & 0x7FFF) {

    //====================
    case HSHELL_WINDOWREPLACING:
        hwnd_replacing = hwnd;
        goto hshell_windowdestroyed;

    case HSHELL_WINDOWREPLACED:
        if (NULL == hwnd_replacing)
            goto hshell_windowdestroyed;
        hwnd = hwnd_replacing;
        hwnd_replacing = NULL;
        if (NULL == tl)
            break;
        tl->hwnd = hwnd;
        get_caption(tl, 1);
        if (activeTaskWindow == hwnd)
            goto hshell_windowactivated;
        send_task_message(hwnd, TASKITEM_MODIFIED);
        break;

    //====================
    case HSHELL_WINDOWCREATED: // 1
        // windows reshown by the vwm also trigger the HSHELL_WINDOWCREATED
        if (hwnd && NULL == tl)
        {
            AddTask(hwnd);
            workspaces_set_desk();
        }
        break;

    //====================
    case HSHELL_WINDOWDESTROYED: // 2
    hshell_windowdestroyed:
        // windows hidden by the vwm also trigger the HSHELL_WINDOWDESTROYED
        if (tl && is_valid_task(hwnd) != 2)
        {
            RemoveTask(tl);
        }
        break;

    //====================
    case HSHELL_WINDOWACTIVATED: // 4
    hshell_windowactivated:
    {
        tasklist *tl2;
        HWND prev_fg_window;

        //if (wParam & 0x8000) ...;
        // true if a fullscreen window is present
        // (which is not necessarily this 'hwnd')

        prev_fg_window = activeTaskWindow;
        activeTaskWindow = hwnd;
        dolist (tl2, gTaskList)
            tl2->active = false;

        if (tl)
        {
            int windesk = vwm_get_desk(hwnd);
            tl->active = true;
            if (currentScreen == windesk)
            {
                // in case the app has windows on other workspaces, this
                // gathers them in the current
                setDesktop(hwnd, currentScreen, false);
            }
            else
            if (Settings_followActive
                && -1 != FindTask(prev_fg_window)
                && FALSE == IsIconic(prev_fg_window))
            {
                // we switch only if there is a previously active window
                // and that window neither was closed nor minimized
                setDesktop(hwnd, windesk, true);
            }
            else
            if (currentScreen == vwm_get_desk(GetLastActivePopup(hwnd)))
            {
                // the app is on other workspaces but has popup'd something
                // (supposedly spontaneously) in the current workspace.
                // We put the popup on the other WS and switch to.
                setDesktop(hwnd, windesk, true);
            }
            else
            {
                // else we prevent the window from being activated
                // and set blackbox as foreground task. We dont
                // want people typing in windows on other workspaces
                SwitchToBBWnd();
                setDesktop(hwnd, windesk, false);
                break;
            }

            // ----------------------------------------
            // insert at head of the 'activated windows' list

            SetTopTask(tl, 1);

            // ----------------------------------------
            // try to grab title & icon, if still missing

            get_caption(tl, 0);
        }

        send_task_message(hwnd, TASKITEM_ACTIVATED);
        break;
    }

    //====================
    case HSHELL_REDRAW: // 6
        if (tl)
        {
            UINT msg = TASKITEM_MODIFIED;
            get_window_text(tl->hwnd, tl->caption, sizeof tl->caption);
            get_window_icon(tl->hwnd, &tl->icon); // disable for foobar delay issue ?
            if (wParam & 0x8000)
            {
                msg = TASKITEM_FLASHED;
                tl->flashing = true;
            }
            if (false == tl->active && GetForegroundWindow() == GetLastActivePopup(hwnd))
                goto hshell_windowactivated;

            send_task_message(hwnd, msg);
            // post_message_if_needed(BB_TASKSUPDATE, (WPARAM)hwnd, msg);
        }
        break;

    //====================
    case HSHELL_LANGUAGE: // 8 - win9x/me only
        send_task_message(hwnd, TASKITEM_LANGUAGE);
        break;
/*
    //====================
    case HSHELL_TASKMAN: // 7 - win9x/me only
        break;

    case HSHELL_ACTIVATESHELLWINDOW: // 3, never seen
        break;

    case HSHELL_GETMINRECT:          // 5, never seen
        break;

    case HSHELL_ENDTASK: // 10
        break;
*/
    }
} 


//===========================================================================
// API(?candidate): EnumTasks
//===========================================================================

void EnumTasks (TASKENUMPROC lpEnumFunc, LPARAM lParam)
{
    tasklist* tl;
    dolist (tl, gTaskList)
        if (FALSE == lpEnumFunc(tl, lParam))
            break;
}

//===========================================================================
// API(?candidate): EnumDesks
//===========================================================================

void EnumDesks(DESKENUMPROC lpEnumFunc, LPARAM lParam)
{
    int n;
    for (n = 0; n < nScreens; n++) {
        DesktopInfo DI;
        get_desktop_info(&DI, n);
        if (FALSE == lpEnumFunc(&DI, lParam))
            break;
    }
}

} // !namespace bbcore

