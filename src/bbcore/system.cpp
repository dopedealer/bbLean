
#include <windows.h> 
#include <psapi.h>
#include <BBApi.h>

#include <bbshell.h>

#include "system.h"
#include "BB.h"
#include "Settings.h"
#include "rcfile.h"
#include "Utils.h"
#include "nls.h"

namespace bbcore { 

// Structures
struct dt_margins
{
    dt_margins* next;
    HWND hwnd;
    HMONITOR hmon;
    int location;
    int margin;
}; 

struct _screen
{
    _screen* next;   /* this must come first */
    HMONITOR hMon;          /* this must be the second member */
    int index;
    RECT screen_rect;
    RECT work_rect;
    RECT new_rect;
    RECT custom_margins;
};

struct _screen_list
{
    _screen* pScr;
    _screen** ppScr;
    int index;
}; 

struct edges { int from1, from2, to1, to2, dmin, omin, d, o, def; };

struct snap_info
{
    edges* h;
    edges* v;
    bool sizing;
    bool same_level;
    int pad;
    HWND self;
    HWND parent;
}; 


// global variables
bool gMultimon;


bool bbGetMultimon(void)
{
    return gMultimon;
}

void bbSetMultimon(bool value)
{
    gMultimon = value;
}

void get_mon_rect(HMONITOR hMon, RECT *s, RECT *w)
{
    if (hMon)
    {
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (::GetMonitorInfoA(hMon, &mi))
        {
            if (w) *w = mi.rcWork;
            if (s) *s = mi.rcMonitor;
            return;
        }
    }
    if (w)
    {
        SystemParametersInfo(SPI_GETWORKAREA, 0, w, 0);
    }
    if (s)
    {
        s->top = s->left = 0;
        s->right = GetSystemMetrics(SM_CXSCREEN);
        s->bottom = GetSystemMetrics(SM_CYSCREEN);
    }
}

//Flags: 
//GETMON_XXX in BBApi.h 
HMONITOR getMonitorRect(void *from, RECT *r, int flags)
{
    HMONITOR hMon{};
    if (bbGetMultimon() && from)
    {
        switch (flags & ~GETMON_WORKAREA)
        {
            case GETMON_FROM_WINDOW:
                hMon = ::MonitorFromWindow((HWND)from, MONITOR_DEFAULTTONEAREST);
                break;
            case GETMON_FROM_POINT:
                hMon = ::MonitorFromPoint(*(POINT*)from, MONITOR_DEFAULTTONEAREST);
                break;
            case GETMON_FROM_MONITOR:
                hMon = (HMONITOR)from;
                break;
        }
    }

    if (flags & GETMON_WORKAREA)
    {
        get_mon_rect(hMon, NULL, r);
    }
    else
    {
        get_mon_rect(hMon, r, NULL);
    }

    return hMon;
}

/// \brief Wrapper for 'SetLayeredWindowAttributes', win9x compatible
bool setTransparency(HWND hwnd, BYTE alpha)
{
    LONG wStyle1, wStyle2;

    //dbg_window(hwnd, "alpha %d", alpha);

    wStyle1 = wStyle2 = ::GetWindowLong(hwnd, GWL_EXSTYLE);
    if (alpha < 255)
    {
        wStyle2 |= WS_EX_LAYERED;
    }
    else
    {
        wStyle2 &= ~WS_EX_LAYERED;
    }

    if (wStyle2 != wStyle1)
    {
        SetWindowLong(hwnd, GWL_EXSTYLE, wStyle2);
    }

    if (wStyle2 & WS_EX_LAYERED)
    {
        return 0 != ::SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
    }

    return true;
}

/// \brief Get application's name from window, returns lenght of string (0 on
///        failure)
int getAppByWindow(HWND hwnd, char* processName)
{
    DWORD pid{};
    HMODULE hMod{};
    HANDLE hPr;
    DWORD cbNeeded;

    processName[0] = 0;

    // determine the process id of the window handle
    GetWindowThreadProcessId(hwnd, &pid);

    hPr = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pid);
    if (hPr)
    {
        if(::EnumProcessModules(hPr, &hMod, sizeof(hMod), &cbNeeded))
        {
            ::GetModuleBaseNameA(hPr, hMod, processName, MAX_PATH);
        }
        CloseHandle(hPr);
    }

    // debug_printf("appname = %s\n", processName);
    return strlen(processName);
}

/// \brief Is this window considered as an application
/// Checks given hwnd to see if it's an app
/// This is used to populate the task list in case bb is started manually.
bool isAppWindow(HWND hwnd)
{
    HWND hParent;

    if (!IsWindow(hwnd))
    {
        return false;
    }
    
    // if it is a WS_CHILD or not WS_VISIBLE, fail it
    if ((GetWindowLong(hwnd, GWL_STYLE) & (WS_CHILD|WS_VISIBLE|WS_DISABLED)) != WS_VISIBLE)
    {
        return false;
    }

    // if the window is a WS_EX_TOOLWINDOW fail it
    if ((GetWindowLong(hwnd, GWL_EXSTYLE) & (WS_EX_TOOLWINDOW|WS_EX_APPWINDOW)) == WS_EX_TOOLWINDOW)
    {
        return false;
    }

    // If this has a parent, then only accept this window
    // if the parent is not accepted
    hParent = GetParent(hwnd);
    if (NULL == hParent)
    {
        hParent = GetWindow(hwnd, GW_OWNER);
    }

    if (hParent && isAppWindow(hParent))
    {
        return false;
    }

    if (0 == GetWindowTextLength(hwnd))
    {
        return false;
    }

    return true;
}

//static
BOOL CALLBACK fnEnumMonProc(HMONITOR hMon, HDC hdcOptional, RECT *prcLimit, LPARAM dwData)
{
    _screen* s;
    _screen_list* i;
    int screen;

    s = c_new(_screen);
    s->hMon = hMon;
    get_mon_rect(hMon, &s->screen_rect, &s->work_rect);
    s->new_rect = s->screen_rect;

    i = (_screen_list *)dwData;
    *i->ppScr = s;
    i->ppScr = &s->next;

    screen = s->index = i->index++;
    if (0 == screen)
    {
        s->custom_margins = Settings_desktopMargin;
    }
    else
    {
        getCustomMargin(&s->custom_margins, screen);
    }

    // debug_printf("EnumProc %x %d %d %d %d", hMon, s->screen_rect.left, s->screen_rect.top, s->screen_rect.right, s->screen_rect.bottom);
    return TRUE;
}

void updateScreenAreas(dt_margins* desktopMargins)
{
    _screen_list si;
    _screen* pS;

    si.pScr = NULL;
    si.ppScr = &si.pScr;
    si.index = 0;

    if (bbGetMultimon())
    {
        ::EnumDisplayMonitors(NULL, NULL, fnEnumMonProc, (LPARAM)&si);
    }
    else
    {
        fnEnumMonProc(NULL, NULL, NULL, (LPARAM)&si);
    }

    //debug_printf("list: %d %d", listlen(si.pScr), si.index);
    if (false == Settings_fullMaximization)
    {
        dt_margins* p;
        // first loop through the set margins from plugins
        dolist (p, desktopMargins) {
            pS = (_screen *)assoc(si.pScr, p->hmon); // get screen for this window
            if (pS) {
                RECT *n = &pS->new_rect;
                RECT *s = &pS->screen_rect;
                switch (p->location)
                {
                    case BB_DM_LEFT   : n->left     = imax(n->left  , s->left   + p->margin); break;
                    case BB_DM_TOP    : n->top      = imax(n->top   , s->top    + p->margin); break;
                    case BB_DM_RIGHT  : n->right    = imin(n->right , s->right  - p->margin); break;
                    case BB_DM_BOTTOM : n->bottom   = imin(n->bottom, s->bottom - p->margin); break;
                }
            }
        }
        // these may be overridden by fixed custom margins from extensions rc
        dolist (pS, si.pScr) {
            RECT *n = &pS->new_rect;
            RECT *s = &pS->screen_rect;
            RECT *m = &pS->custom_margins;
            if (-1 != m->left)     n->left     = s->left     + m->left    ;
            if (-1 != m->top)      n->top      = s->top      + m->top     ;
            if (-1 != m->right)    n->right    = s->right    - m->right   ;
            if (-1 != m->bottom)   n->bottom   = s->bottom   - m->bottom  ;
        }
    }

    // finally set the new margins, if changed
    dolist (pS, si.pScr) {
        RECT *n = &pS->new_rect;
        if (0 != memcmp(&pS->work_rect, n, sizeof(RECT))) {
            // debug_printf("WA = %d %d %d %d", n->left, n->top, n->right, n->bottom);
            SystemParametersInfo(SPI_SETWORKAREA, 0, (PVOID)n, 0);
        }
    }

    freeall(&si.pScr);
}

/// \brief Add a screen margin at the indicated location
/// Set a margin for e.g. toolbar, bbsystembar, etc
/// \param hwnd to associate the margin with, location, margin-width
void setDesktopMargin(HWND hwnd, int location, int margin)
{
    //debug_printf("SDTM: %08x %d %d (%x)", hwnd, location, margin, ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST));

    static dt_margins* margin_list;
    dt_margins* p;

    if (BB_DM_RESET == location || Settings_disableMargins)
    {
        // reset everything
        if (NULL == margin_list)
        {
            return;
        }
        freeall(&margin_list);
    }
    else if (BB_DM_REFRESH == location)
    {
        // re-validate margins
        for (p = margin_list; p;)
        {
            dt_margins* n = p->next;
            if (FALSE == IsWindow(p->hwnd))
            {
                remove_item(&margin_list, p);
            }
            p = n;
        }
    }
    else if (hwnd)
    {
        // search for hwnd:
        p = (dt_margins *)assoc(margin_list, hwnd);
        if (margin)
        {
            if (NULL == p)
            {
                // insert a _new structure
                p = c_new(dt_margins);
                cons_node (&margin_list, p);
                p->hwnd = hwnd;
            }
            p->location = location;
            p->margin = margin;
            if (bbGetMultimon())
            {
                p->hmon = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            }
        }
        else
        {
            if (p)
            {
                remove_item(&margin_list, p);
            }
        }
    }

    updateScreenAreas(margin_list);
}

/// \brief  Replace Environment Variables in string (like %USERNAME%)
/// Parses a given string and replaces all %VAR% with the environment
/// variable value if such a value exists
void replaceEnvVars(char* str)
{
    replace_environment_strings(str, MAX_LINE_LENGTH);
}

/// \brief Replace special folders in string (like APPDATA)
/// Replace shell folders in a string path, like DESKTOP\...
char* replaceShellFolders(char* str)
{
    return replace_shellfolders(str, str, false, bbDefaultrcPath(), isUsingUtf8Encoding());
}

/// \brief Copies the path of the Blackbox executable to the specified buffer.
/// Path where blackbox.exe is (including trailing backslash)
char* WINAPI getBlackboxPath(char* pszPath, int nMaxLen)
{
    GetModuleFileName(NULL, pszPath, nMaxLen);
    ::GetLongPathNameA(pszPath, pszPath, nMaxLen);
    *(char*)file_basename(pszPath) = 0;
    return pszPath;
}

#ifndef BBTINY

/// \brief A safe execute routine
/// Shell execute a command
BOOL bbExecute(
    HWND Owner,
    const char* szVerb,
    const char* szFile,
    const char* szArgs,
    const char* szDirectory,
    int nShowCmd,
    int flags)
{
    SHELLEXECUTEINFO sei;
    char workdir[MAX_PATH];

    if (NULL == szDirectory || 0 == szDirectory[0])
    {
        szDirectory = getBlackboxPath(workdir, sizeof workdir);
    }

    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.hwnd = Owner;
    sei.lpVerb = szVerb;
    sei.lpParameters = szArgs;
    sei.lpDirectory = szDirectory;
    sei.nShow = nShowCmd;

    if (flags & RUN_ISPIDL)
    {
        sei.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI;
        sei.lpIDList = (void*)szFile;
    }
    else
    {
        sei.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI;
        sei.lpFile = szFile;
        if (NULL == szFile || 0 == szFile[0])
            goto skip;
    }

    if (ShellExecuteEx(&sei))
    {
        return TRUE;
    }

skip:
    if (0 == (flags & RUN_NOERRORS))
    {
        char msg[200];
        bbMessageBox(MB_OK, NLS2("$Error_Execute$",
            "Error: Could not execute: %s\n(%s)"),
            szFile && szFile[0] ? szFile : NLS1("<empty>"),
            win_error(msg, sizeof msg));
    }

    return FALSE;
}
#endif

//static
void snap_to_edge(
    edges* h,
    edges* v,
    bool sizing,
    bool same_level,
    int pad)
{
    int o, d, n; edges* t;
    h->d = h->def; v->d = v->def;
    for (n = 2;;) // v- and h-edge
    {
        // see if there is any common edge,
        // i.e if the lower top is above the upper bottom.
        if ((v->to2 < v->from2 ? v->to2 : v->from2)
            >= (v->to1 > v->from1 ? v->to1 : v->from1))
        {
            if (same_level) // child to child
            {
                //snap to the opposite edge, with some padding between
                bool f = false;

                d = o = (h->to2 + pad) - h->from1;  // left edge
                if (d < 0) d = -d;
                if (d <= h->d)
                {
                    if (false == sizing)
                        if (d < h->d) h->d = d, h->o = o;
                    if (d < h->def) f = true;
                }

                d = o = h->to1 - (h->from2 + pad); // right edge
                if (d < 0) d = -d;
                if (d <= h->d)
                {
                    if (d < h->d) h->d = d, h->o = o;
                    if (d < h->def) f = true;
                }

                if (f)
                {
                    // if it's near, snap to the corner
                    if (false == sizing)
                    {
                        d = o = v->to1 - v->from1;  // top corner
                        if (d < 0) d = -d;
                        if (d < v->d) v->d = d, v->o = o;
                    }
                    d = o = v->to2 - v->from2;  // bottom corner
                    if (d < 0) d = -d;
                    if (d < v->d) v->d = d, v->o = o;
                }
            }
            else // child to frame
            {
                //snap to the same edge, with some bevel between
                if (false == sizing)
                {
                    d = o = h->to1 - (h->from1 - pad); // left edge
                    if (d < 0) d = -d;
                    if (d < h->d) h->d = d, h->o = o;
                }
                d = o = h->to2 - (h->from2 + pad); // right edge
                if (d < 0) d = -d;
                if (d < h->d) h->d = d, h->o = o;
            }
        }
        if (0 == --n) break;
        t = h; h = v, v = t;
    }

    if (false == sizing)// && false == same_level)
    {
        // snap to center
        for (n = 2;;) // v- and h-edge
        {
            if (v->d < v->dmin)
            {
                d = o = (h->to1 + h->to2)/2 - (h->from1 + h->from2)/2;
                if (d < 0) d = -d;
                if (d < h->d) h->d = d, h->o = o;
            }
            if (0 == --n) break;
            t = h; h = v, v = t;
        }
    }

    if (h->d < h->dmin) h->dmin = h->d, h->omin = h->o;
    if (v->d < v->dmin) v->dmin = v->d, v->omin = v->o;
}

/*
static
void snap_to_grid(edges* h, edges* v, bool sizing, int grid, int pad)
{
    for (edges* g = h;;g = v)
    {
        int o, d;
        if (sizing) o = g->from2 - g->from1 + pad; // relative to topleft
        else        o = g->from1 - pad; // absolute coords

        o = o % grid;
        if (o < 0) o += grid;

        if (o >= grid / 2)
            d = o = grid-o;
        else
            d = o, o = -o;

        if (d < g->dmin) g->dmin = d, g->omin = o;

        if (g == v) break;
    }
}
*/

//static
BOOL CALLBACK SnapEnumProc(HWND hwnd, LPARAM lParam)
{
    snap_info* si = (snap_info *)lParam;
    LONG style = GetWindowLong(hwnd, GWL_STYLE);

    if (hwnd != si->self && (style & WS_VISIBLE))
    {
        HWND pw = (style & WS_CHILD) ? GetParent(hwnd) : NULL;
        if (pw == si->parent && false == Menu_IsA(hwnd))
        {
            RECT r;

            GetWindowRect(hwnd, &r);
            r.right -= r.left;
            r.bottom -= r.top;

            if (pw)
                ScreenToClient(pw, (POINT*)&r.left);

            if (false == si->same_level)
            {
                r.left += si->h->from1;
                r.top  += si->v->from1;
            }
            si->h->to2 = (si->h->to1 = r.left) + r.right;
            si->v->to2 = (si->v->to1 = r.top)  + r.bottom;
            snap_to_edge(si->h, si->v, si->sizing, si->same_level, si->pad);
        }
    }
    return TRUE;
}   

void snapWindowToEdgeV(WINDOWPOS* wp, LPARAM nDist, UINT flags, va_list vlist)
{
    edges h;
    edges v;
    snap_info si;
    int snapdist, padding;
    bool snap_plugins, sizing, snap_workarea;
    HWND self, parent;
    RECT r;
    //int grid = 0;

    snapdist = Settings_snapThreshold;
    padding = Settings_snapPadding;
    snap_plugins = Settings_snapPlugins;
    sizing = 0 != (flags & SNAP_SIZING);
    snap_workarea = 0 == (flags & SNAP_FULLSCREEN);
    if (flags & SNAP_NOPLUGINS)
        snap_plugins = false;
    if (flags & SNAP_TRIGGER)
        snapdist = nDist;
    if (flags & SNAP_PADDING)
        padding = va_arg(vlist, int);
    if (snapdist < 1)
        return;

    self = wp->hwnd;
    parent = NULL;

    if (WS_CHILD & GetWindowLong(self, GWL_STYLE))
        parent = GetParent(self);

    // ------------------------------------------------------
    // well, why is this here? Because some plugins call this
    // even if they reposition themselves rather than being
    // moved by the user.
    {
        static bool capture;
        if (GetCapture() == self)
            capture = true;
        else if (capture)
            capture = false;
        else
            return;
    }

    // ------------------------------------------------------

    si.h = &h, si.v = &v, si.sizing = sizing, si.same_level = true,
    si.pad = padding, si.self = self, si.parent = parent;

    h.dmin = v.dmin = h.def = v.def = snapdist;
    h.from1 = wp->x;
    h.from2 = h.from1 + wp->cx;
    v.from1 = wp->y;
    v.from2 = v.from1 + wp->cy;

    // ------------------------------------------------------
    // snap to grid

    /*if (grid > 1 && (parent || sizing))
    {
        snap_to_grid(&h, &v, sizing, grid, padding);
    }*/
    //else
    {
        // -----------------------------------------
        if (parent) {

            // snap to siblings
            EnumChildWindows(parent, SnapEnumProc, (LPARAM)&si);

            if (0 == (flags & SNAP_NOPARENT)) {
                // snap to frame edges
                GetClientRect(parent, &r);
                h.to1 = r.left;
                h.to2 = r.right;
                v.to1 = r.top;
                v.to2 = r.bottom;
                snap_to_edge(&h, &v, sizing, false, padding);
            }
        } else {

            // snap to top level windows
            if (snap_plugins)
                EnumThreadWindows(GetCurrentThreadId(), SnapEnumProc, (LPARAM)&si);

            // snap to screen edges
            getMonitorRect(self, &r, snap_workarea ?
                GETMON_WORKAREA|GETMON_FROM_WINDOW : GETMON_FROM_WINDOW);
            h.to1 = r.left;
            h.to2 = r.right;
            v.to1 = r.top;
            v.to2 = r.bottom;
            snap_to_edge(&h, &v, sizing, false, 0);
        }

        // -----------------------------------------
        if (sizing) {
            if (flags & SNAP_CONTENT) { // snap to button icons
                SIZE * psize = va_arg(vlist, SIZE*);
                h.to2 = (h.to1 = h.from1) + psize->cx;
                v.to2 = (v.to1 = v.from1) + psize->cy;
                snap_to_edge(&h, &v, sizing, false, -2*padding);
            }

            if (0 == (flags & SNAP_NOCHILDS)) { // snap frame to childs
                si.same_level = false;
                si.pad = -padding;
                si.self = NULL;
                si.parent = self;
                EnumChildWindows(self, SnapEnumProc, (LPARAM)&si);
            }
        }
    }

    // -----------------------------------------
    // adjust the window-pos

    if (h.dmin < snapdist) {
        if (sizing)
            wp->cx += h.omin;
        else
            wp->x += h.omin;
    }

    if (v.dmin < snapdist) {
        if (sizing)
            wp->cy += v.omin;
        else
            wp->y += v.omin;
    } 
}

/// \brief Snaps a given windowpos at a specified distance
/// \param wp A WINDOWPOS recieved from WM_WINDOWPOSCHANGING
/// \param nDist Distance to snap to
/// \param flags Use screensize of workspace
void snapWindowToEdge(WINDOWPOS* wp, LPARAM nDist, UINT flags, ...)
{
    va_list va{}; 
    va_start(va, flags);
    snapWindowToEdgeV(wp, nDist, flags, va);
    va_end(va);
}


/// \brief Version string (as specified in resource.rc, e.g. "bbLean 1.17")
/// \returns The current version
const char* getBBVersion(void)
{
    return BBAPPVERSION;
}

BOOL BBExecute_string(const char* line, int flags)
{
    char workdir[MAX_PATH];
    char file[MAX_PATH];
    const char *cmd, *args;
    char* cmd_temp{};
    char* line_temp{};
    int n, ret;

    workdir[0] = 0;
    if (flags & RUN_WINDIR)
        GetWindowsDirectory(workdir, sizeof workdir);
    else
        getBlackboxPath(workdir, sizeof workdir);

    if (0 == (flags & RUN_NOSUBST))
        line = replace_environment_strings_alloc(&line_temp, line);

    if (flags & RUN_NOARGS)
    {
        cmd = line;
        args = NULL;
        strcpy(file, cmd);

    }
    else
    {
        for (args = line;; args += args[0] == ':')
        {
            cmd = args;
            NextToken(file, &args, NULL);
            if (file[0] != '-')
                break;

            // -hidden : run in a hidden window
            if (0 == strcmp(file+1, "hidden")) {
                flags |= RUN_HIDDEN;
                continue;
            }

            // -in <path> ; specify working directory
            if (0 == strcmp(file+1, "in")) {
                NextToken(file, &args, NULL);
                replace_shellfolders(workdir, file, true, bbDefaultrcPath(), isUsingUtf8Encoding());
                continue;
            }

            // -workspace1 : specify workspace
            if (-1 != (n = get_workspace_number(file+1))) {
                SendMessage(gBBhwnd, BB_SWITCHTON, 0, n);
                BBSleep(10);
                continue;
            }
            break;
        }
        if (0 == args[0])
            args = NULL;
    }

    if (0 == (flags & RUN_NOSUBST))
    {
        n = '\"' == file[0];
        cmd_temp = (char*)m_alloc((args ? strlen(args) : 0) + MAX_PATH + 10);
        strcpy(cmd_temp, replace_shellfolders(file, file, true, bbDefaultrcPath(), isUsingUtf8Encoding()));
        if (n)
            quote_path(cmd_temp);
        if (args)
            sprintf(strchr(cmd_temp, 0), " %s", args);
        cmd = cmd_temp;
    }

    ret = -1 != run_process(cmd, workdir, flags);
    if (ret)
    {
        // debug_printf("cmd (%d) <%s>", ret, cmd);
    }
    else
    {
        ret = bbExecute(NULL, NULL, file, args, workdir,
            flags & RUN_HIDDEN ? SW_HIDE : SW_SHOWNORMAL,
            flags & RUN_NOERRORS);
        // debug_printf("exec (%d) <%s> <%s>", ret, file, args);
    }

    free_str(&cmd_temp);
    free_str(&line_temp);
    return ret;
}

int BBExecute_pidl(const char* verb, const void *pidl)
{
    if (NULL == pidl)
        return FALSE;

    return bbExecute(NULL, verb, (const char*)pidl,
        NULL, NULL, SW_SHOWNORMAL, RUN_ISPIDL|RUN_NOERRORS);
}

} // !namespace bbcore
