
#include "barinfo.h"

#include <string>
#include <time.h>

#include <bblib.h>
#include <BB.h>

#include "bbLeanBar.h"
#include "TinyDropTarg.h"

#include "tooltips.h"
#include "Droptarget.h"
#include "leanbar.h"
#include "BuffBmp.h" 

// global variables
bool nobool{};
char gScreenName[80];
int styleBorderWidth{};
int styleBevelWidth{};
HWND BBhwnd{};

StyleItem NTaskStyle{};
StyleItem ATaskStyle{};

int TBJustify;
//static int styleBevelWidth;
COLORREF styleBorderColor{};

barinfo::barinfo(void)
{
    BBP_clear(this, BARINFO_FIRSTITEM);
    this->next = g_PI;
    g_PI = this;
    memset(&gScreenName, 0x0, std::size(gScreenName));
}

void barinfo::about_box(void)
{
    BBP_messagebox(this, MB_OK, "%s - © %s %s\n", szVersion, szCopyright, szInfoEmail);
}

barinfo::~barinfo(void)
{
    DelTasklist();
    DelTraylist();
#ifndef NO_TIPS
    ClearToolTips(hwnd);
#endif
#ifndef NO_DROP
    exit_drop_targ(m_TinyDropTarget);
#endif
    BBP_Exit_Plugin(this);
    delete [] (char*)cfg_list;
    delete [] (char*)cfg_menu;
    for (plugin_info** pp = &g_PI; *pp; pp = &(*pp)->next) {
        if (this == *pp) {
            *pp = this->next;
            break;
        }
    }
}

BOOL barinfo::task_enum_func(const tasklist* tl, LPARAM lParam)
{
    barinfo* PI = (barinfo *)lParam;

    if (PI->currentOnly)
    {
        if (PI->on_multimon)
        {
            // TODO: howto detect when the user drags a window to
            // the other monitor ?

            if (PI->hMon != GetMonitorRect(tl->hwnd, NULL, GETMON_FROM_WINDOW))
                return TRUE;
        }

        if (currentScreen != tl->wkspc)
            return TRUE;
    }

    tasklist* item = new tasklist;
    *item = *tl;
    cons_node(&PI->taskList, item);
    return TRUE;
}

void barinfo::DelTasklist(void)
{
    freeall(&taskList);
}

void barinfo::NewTasklist(void)
{
    DelTasklist();
    EnumTasks(task_enum_func, (LPARAM)this);
    reverse_list(&taskList);
}

tasklist* barinfo::GetTaskPtrEx(int pos)
{
    tasklist* tl = taskList; int i = 0;
    while (tl) { if (pos == i) break; i++, tl = tl->next; }
    return tl;
}

int barinfo::GetTaskListSizeEx(void)
{
    tasklist* tl = taskList; int i = 0;
    while (tl) { i++, tl = tl->next; }
    return i;
}

HWND barinfo::GetTaskWindowEx(int i)
{
    tasklist* tl = GetTaskPtrEx(i);
    return tl ? tl->hwnd : NULL;
}

void barinfo::DelTraylist(void)
{
    freeall(&trayList);
    freeall(&trayHiddenList);
}

int barinfo::GetTraySizeEx(void)
{
    return trayVisiblesCount;
}

systemTray* barinfo::GetTrayIconEx(int i)
{
    return GetTrayIcon(RealTrayIndex(i));
}

barinfo::trayNode* barinfo::GetTrayNode(int vis_index)
{
    trayNode *tn; int n = vis_index;
    dolist (tn, trayList)
        if (false == tn->hidden || trayShowAll)
            if (n-- == 0)
                break;
    return tn;
}

int barinfo::RealTrayIndex(int vis_index)
{
    trayNode *tn = GetTrayNode(vis_index);
    return tn ? tn->index : -1;
}

void barinfo::NewTraylist(void)
{
    trayNode *tn, **ptn;
    trayHidden* sn;
    int n, ts;

    if (NULL == trayHiddenList)
        trayHiddenList = trayLoadHidden();

    if (false == this->has_tray)
        goto skip;

    // loop through icons, add new ones as needed, mark all as present
    for (n = 0, ts = GetTraySize(); n < ts; ++n) {
        systemTray *icon = GetTrayIcon(n);
        if (NULL == icon)
            continue;

        dolist (tn, trayList)
            if (tn->hWnd == icon->hWnd && tn->uID == icon->uID)
                goto found; // already there

        tn = new trayNode;
        tn->hWnd = icon->hWnd;
        tn->uID = icon->uID;
        tn->hidden = false;
        tn->tip_checked = false;
        GetClassName(tn->hWnd, tn->class_name, sizeof tn->class_name);

        // add a new one
        cons_node(&trayList, tn);

        // check through saved list whether this should start hidden
        dolist (sn, trayHiddenList) {
            if (0 == strcmp(sn->class_name, tn->class_name)) {
                trayNode* tn1 = NULL;
                if (sn->uID != tn->uID) {
                    dolist (tn1, trayList)
                        if (tn1 != tn && 0 == strcmp(tn1->class_name, tn->class_name))
                            break;
                }
                if (!tn1) {
                    tn->hidden = true;
                    break;
                }
            }
        }

found:
        tn->mark = true; // do not remove this below
        tn->index = n; // the index for 'GetTrayIcon(int index);'
    }

skip:
    // remove icons that were not marked above, otherwise clear mark
    for (ptn = &trayList; NULL != (tn = *ptn); ) {
        if (tn->mark) {
            tn->mark = false;
            ptn = &tn->next;
        } else {
            *ptn = tn->next;
            delete tn;
        }
    }

    trayVisiblesCount = 0;
    dolist (tn, trayList)
        if (false == tn->hidden || trayShowAll)
            ++ trayVisiblesCount;
}

void barinfo::trayShowIcon(int index, int state)
{
    trayNode *tn;
    dolist (tn, trayList)
        if (tn->index == index)
            tn->hidden = state < 0 ? false == tn->hidden : state == 0;

    traySaveHidden();

    trayShowAll = false;
    NewTraylist();
    trayMenu(false);
    this->update(UPD_DRAW);
}

void barinfo::trayToggleShowAll(int state)
{
    trayShowAll = state < 0 ? false == trayShowAll : state == 0;
    NewTraylist();
    trayMenu(false);
    this->update(UPD_DRAW);
}

void barinfo::traySaveHidden (void)
{
    trayHidden *sn, *sn0 = NULL;
    trayNode* tn;
    int n;

    // build new list
    dolist (tn, trayList) {
        if (false == tn->hidden)
            continue;
        sn = new trayHidden;
        sn->uID = tn->uID;
        GetClassName(tn->hWnd, sn->class_name, sizeof sn->class_name);
        cons_node(&sn0, sn);
    }

    // save list to rc
    BBP_write_int(this, "tray.hidden.count", listlen(sn0));
    for (n = 0, sn = sn0;;) {
        char val[240], key[40];
        sprintf(key, "tray.hidden.%d", ++n);
        if (sn) {
            sprintf(val, "\"%s\",%d", sn->class_name, sn->uID);
            sn = sn->next;
            BBP_write_string(this, key, val);
        } else {
            if (NULL == BBP_read_string(this, NULL, key, NULL))
                break;
            // delete redundant entry:
            BBP_write_string(this, key, NULL);
        }
    }

    freeall(&trayHiddenList);
    trayHiddenList = sn0;
}

barinfo::trayHidden* barinfo::trayLoadHidden(void)
{
    trayHidden *sn, *sn0 = NULL;
    int n, count;
    char key[40];
    const char *val;
    count = BBP_read_int(this, "tray.hidden.count", 0);
    for (n = 0; n < count; ++n) {
        sprintf(key, "tray.hidden.%d", n+1);
        val = BBP_read_string(this, NULL, key, NULL);
        if (val) {
            sn = new trayHidden;
            sscanf(val, "\"%[^\"]\",%u", sn->class_name, &sn->uID);
            cons_node(&sn0, sn);
        }
    }
    return sn0;
}

void barinfo::forceTips(void)
{
    trayNode* tn;
    systemTray *icon;
    int f = 0;
    dolist (tn, trayList) {
        if (tn->tip_checked)
            continue;
        icon = GetTrayIcon(tn->index);
        if (NULL == icon)
            continue;
        if (0 == icon->szTip[0]) {
            //dbg_printf("fakemouse \"%s\" : %d", tn->class_name, tn->uID);
            ForwardTrayMessage(tn->index, WM_MOUSEMOVE, NULL);
            ++f;
        }
        tn->tip_checked = true;
    }

    if (f)
    {
        BBSleep(20); // wait for them to send back the tip
    }
}

void barinfo::trayMenu(bool pop)
{
    trayNode* tn;
    systemTray *icon;
    char broam[100];

    if (pop)
        forceTips();
    n_menu *m = n_makemenu("Tray Icons");
    n_menuitem_bol(m, "Show All", "tray.showall", trayShowAll);
    n_menuitem_nop(m, NULL);
    dolist (tn, trayList) {
        icon = GetTrayIcon(tn->index);
        if (NULL == icon)
            continue;
        sprintf(broam, "tray.showicon %d", tn->index + 1);
        n_menuitem_bol(m, icon->szTip, broam, false == tn->hidden);
    }
    n_showmenu(this, m, pop, 0);
}

// configuration and menu items
void barinfo::make_cfg(void)
{
    config c[] = {
        {  "widthPercent" ,     R_INT, (void*)72,             &widthPercent },
        {  "tasks.style" ,      R_INT, (void*)2,              &TaskStyle },
        {  "tasks.reverse" ,    R_BOL, (void*)false,          &reverseTasks },
        {  "tasks.current" ,    R_BOL, (void*)false,          &currentOnly },
        {  "tasks.sysmenu" ,    R_BOL, (void*)false,          &taskSysmenu },
        {  "tasks.drawBorder" , R_BOL, (void*)false,          &task_with_border },
        {  "tasks.maxWidth" ,   R_INT, (void*)100,            &taskMaxWidth },

        {  "icon.saturation" ,  R_INT, (void*)0,              &saturationValue },
        {  "icon.hue" ,         R_INT, (void*)60,             &hueIntensity },

        {  "strftimeFormat" ,   R_STR, (void*)"%a %d %H:%M",  &strftimeFormat },

        {  "smallIcons" ,       R_BOL, (void*)true,           &smallIcons },
        {  "autoFullscreenHide",R_BOL, (void*)true,           &autoFullscreenHide },
        {  "setDesktopMargin"  ,R_BOL, (void*)true,           &setDesktopMargin },
        {  "sendToSwitchTo"    ,R_BOL, (void*)true,           &sendToSwitchTo },
        {  "sendToGesture"     ,R_BOL, (void*)true,           &sendToGesture },
        { NULL,0,NULL,NULL }
    };

    cfg_list = new config[array_count(c)], memcpy(cfg_list, c, sizeof c);

    pmenu m[] = {
        { "Width Percent",          "widthPercent",     CFG_INT, &widthPercent  },
        { "",                       "",                 CFG_WIN, NULL },
        { "",                       "",                 0, &nobool  },
        { "Tasks",                  "",                 CFG_SUB, NULL },
        //--------
        { "Text only",              "tbStyle 0",        CFG_INT2|CFG_TASK|0<<8, &TaskStyle },
        { "Icons only",             "tbStyle 1",        CFG_INT2|CFG_TASK|1<<8, &TaskStyle },
        { "Text and Icons",         "tbStyle 2",        CFG_INT2|CFG_TASK|2<<8, &TaskStyle },
        { "",                       "",                 CFG_TASK, &nobool  },
        { "Max. Width (%)",         "taskMaxWidth",     CFG_TASK|CFG_INT, &taskMaxWidth },
        { "Reversed",               "reversedTasks",    CFG_TASK, &reverseTasks  },
        { "Current Only",           "currentOnly",      CFG_TASK, &currentOnly },
        { "System Menu",            "sysMenu",          CFG_TASK, &taskSysmenu },
        { "Force Border",           "drawBorder",       CFG_TASK, &task_with_border },

        { "Icons",                  "",                 CFG_SUB, NULL },
        //--------
        { "Small Size",             "smallIcons",       0, &smallIcons  },
        { "Saturation",             "iconSaturation",   CFG_INT|CFG_255, &saturationValue  },
        { "Hue",                    "iconHue",          CFG_INT|CFG_255, &hueIntensity  },

        { "Clock Format",           "clockFormat",      CFG_MAIN|CFG_STR, &strftimeFormat  },

        //--------
        { "Special Options",        "",                 CFG_SUB, NULL },
        //--------
        { "Detect Fullscreen App",  "autoFullscreenHide",   0, &autoFullscreenHide  },
        { "Set Desktop Margin",   "setDesktopMargin",     0, &setDesktopMargin  },
        { "SendTo Switches Workspace", "sendToSwitchTo",       0, &sendToSwitchTo  },
        { "Mouse Gesture Moves Task", "sendToGesture",       0, &sendToGesture  },

        { NULL,NULL,0,NULL }
    };

    cfg_menu = new pmenu[array_count(m)], memcpy(cfg_menu, m, sizeof m);
}

int barinfo::get_text_width(const char *cp)
{
    HDC hdc = GetDC(hwnd);
    RECT s = {0,0,0,0};
    HGDIOBJ oldfont = SelectObject(hdc, hFont);
    bbDrawText(hdc, cp, &s, DT_CALCRECT|DT_NOPREFIX, 0);
    SelectObject(hdc, oldfont);
    ReleaseDC(hwnd, hdc);
    return s.right;
}

void barinfo::set_screen_info(void)
{
    DesktopInfo DI;
    GetDesktopInfo(&DI);
    currentScreen = DI.number;
    strcpy(gScreenName, DI.name);
    labelWidth = 0;
    string_node* p;
    dolist (p, DI.deskNames)
        labelWidth = imax(get_text_width(p->str), labelWidth);
}

void barinfo::set_clock_string (void)
{
    time_t systemTime;
    time(&systemTime);
    tm* ltp = localtime(&systemTime);

    char fmt_buffer[200];
    const char *fmt = strftimeFormat;
    const char *p = strstr(fmt, "%K");
    bool seconds = strstr(fmt, "%S") || strstr(fmt, "%#S");

    show_kbd_layout = false;

    // check for keyboard layout indicator %K -->
    if (p)
    {
        char lang_id[16];
        int lang_code = LOWORD(GetKeyboardLayout(
            GetWindowThreadProcessId(GetForegroundWindow(), NULL)
            ));
        if (GetLocaleInfo(lang_code, LOCALE_SABBREVLANGNAME, lang_id, sizeof lang_id))
        {
            strcpy(fmt_buffer, fmt);
            memcpy(fmt_buffer + (p - fmt), strupr(lang_id), 2);
            fmt = fmt_buffer;
            seconds = show_kbd_layout = true;
        }
    }
    // <--------------------------------------

    strftime(clockTime, array_count(clockTime), fmt, ltp);
    clockWidth = get_text_width(clockTime);

    SYSTEMTIME lt;
    GetLocalTime(&lt);
    SetTimer(this->hwnd, CLOCK_TIMER, seconds
        ? 1100 - lt.wMilliseconds
        : 61000 - lt.wSecond * 1000,
        NULL);
}

void barinfo::desktop_margin_fn (void)
{
    int margin = 0, pos = 0;
    if (false == this->inSlit
        && this->setDesktopMargin
        && false == this->autoHide
        && (this->is_visible || this->full_screen_hidden)
        )
        switch (this->place) {
        case POS_Top            :
        case POS_TopLeft        :
        case POS_TopRight       :
        case POS_TopCenter      : pos = BB_DM_TOP; margin = height;
            break;
        case POS_Bottom         :
        case POS_BottomCenter   :
        case POS_BottomLeft     :
        case POS_BottomRight    : pos = BB_DM_BOTTOM; margin = height;
            break;
        }
    SetDesktopMargin(this->hwnd, pos, margin);
}

void barinfo::update(int flag)
{
    this->pLeanBar->invalidate(flag);
}

void barinfo::get_window_text(HWND hwnd, char *buffer, int size)
{
    if (0 == (GetVersion() & 0x80000000)) {
        WCHAR wbuf[1000];
        wbuf[0] = 0;
        GetWindowTextW(hwnd, wbuf, size);
        bbWC2MB(wbuf, buffer, size);
    } else {
        buffer[0] = 0;
        GetWindowTextA(hwnd, buffer, size);
    }
}

void barinfo::update_windowlabel(void)
{
    HWND hwnd = GetForegroundWindow();
    if (NULL == hwnd || is_bbwindow(hwnd)) {
        if (GetCapture())
            return;
        hwnd = BBhwnd;
    }
    get_window_text(hwnd, windowlabel, sizeof windowlabel);
}

bool barinfo::check_fullscreen_window(void)
{
    HWND hwnd;
    RECT r, s;
    bool covers;

    char class_name[80];
    DWORD wstyle;

    hwnd = GetForegroundWindow();
    if (NULL == hwnd)
        return false;

    wstyle = GetWindowLong(hwnd, GWL_STYLE);
    /* consider captionless windows only */
    if (WS_CAPTION == (wstyle & WS_CAPTION))
        return false;

    if (0 == (WS_VISIBLE & wstyle))
        return false;

    /* exclude bbLean's own desktop window */
    if (GetWindowThreadProcessId(hwnd, NULL) == GetCurrentThreadId())
        return false;

    /* exclude explorer desktop window */
    GetClassName(hwnd, class_name, sizeof class_name);
    if (0 == strcmp(class_name, "Progman"))
        return false;

    /* consider windows on the bar's monitor only */
    if (this->hMon != GetMonitorRect(hwnd, &s, GETMON_FROM_WINDOW))
        return false;

    /* see wether the window covers the entire screen */
    GetWindowRect(hwnd, &r);

    covers = r.left <= s.left
     && r.right >= s.right
     && r.top <= s.top
     && r.bottom >= s.bottom
     ;

    return covers;
}

LRESULT barinfo::wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *ret)
{
    static int msgs[] =
    {
        BB_TASKSUPDATE,
        BB_TRAYUPDATE,
        BB_DESKTOPINFO,
        BB_SETTOOLBARLABEL,
        BB_REDRAWGUI,
        BB_TOGGLETRAY,
        0
    };

    //dbg_printf("hwnd %04x  msg %5d  wp %08x  lp %08x", hwnd, message, wParam, lParam);
    if (ret)
    {
        if (WM_TIMER == message && AUTOHIDE_TIMER == wParam && TBInfo && TBInfo->hwnd == hwnd && TBInfo->bbsb_hwnd)
        {
            if (check_mouse(TBInfo->bbsb_hwnd))
                return TRUE;
        }

        return *ret;
    }

    switch (message)
    {       
        //=============================================
        case WM_ERASEBKGND:
            return TRUE;

        //=============================================
        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hwnd, &ps);
            HDC buf = CreateCompatibleDC(NULL);
            HGDIOBJ other = SelectObject(buf, CreateCompatibleBitmap(hdc, this->width, this->height));

            RECT r = {0, 0, this->width, 0};
#if 0
            dbg_printf("WM_PAINT: x=%-3d y=%-3d w=%-3d h=%-3d",
                ps.rcPaint.left,
                ps.rcPaint.top,
                ps.rcPaint.right-ps.rcPaint.left,
                ps.rcPaint.bottom-ps.rcPaint.top
                );
#endif
            for (int i = 0; i < BarLines; i++)
            {
                int h = bbLeanBarLineHeight[i];
                r.bottom = r.top + h;
                StyleItem *T = (StyleItem *)GetSettingPtr(SN_TOOLBAR);
                this->pBuff->MakeStyleGradient(buf, &r, T, T->bordered);
                r.top += h - styleBorderWidth;
            }

            hdcPaint = buf;
            p_rcPaint = &ps.rcPaint;
            this->pLeanBar->draw();
            this->pLeanBar->settip();
            this->pBuff->ClearBitmaps();

            BitBltRect(hdc, buf, &ps.rcPaint);
            DeleteObject(SelectObject(buf, other));
            DeleteDC(buf);
            EndPaint(hwnd, &ps);
            return 0;
        }

        //=============================================
        case WM_CREATE:
            SendMessage(BBhwnd, BB_REGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
            this->pBuff = new BuffBmp;
            this->pLeanBar = new LeanBar(this);

            GetRCSettings();

            this->set_tbinfo();
            GetStyleSettings();
            set_screen_info();
            this->NewTasklist();
            this->NewTraylist();
            this->update_windowlabel();
            this->update(UPD_NEW);

            break;

        //=============================================
        case WM_DESTROY:
            SendMessage(BBhwnd, BB_UNREGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
            SetDesktopMargin(hwnd, 0, 0);
            delete this->pLeanBar;
            delete this->pBuff;
            if (hFont) DeleteObject(hFont), hFont = NULL;
            this->reset_tbinfo();
            break;

        //=============================================
        case BB_TOGGLETRAY:
            this->toggled_hidden = false == this->toggled_hidden;
            BBP_set_window_modes(this);
            break;

        //=============================================
        case BB_REDRAWGUI:
            if (wParam & BBRG_TOOLBAR)
            {
                force_button_pressed = 0 != (wParam & BBRG_PRESSED);
                GetStyleSettings();
                this->set_clock_string();
                this->update(UPD_DRAW);
            }
            break;

        //=============================================
        // If Blackbox sends a reconfigure message, load the new settings and update window...

        case BB_RECONFIGURE:
            GetRCSettings();
            GetStyleSettings();
            set_screen_info();
            this->NewTasklist();
            this->NewTraylist();
            this->update_windowlabel();
            this->set_tbinfo();
            this->update(UPD_NEW);
            break;

        //=============================================
        // Desktop or Desktop Name changed

        case BB_DESKTOPINFO:
            if (((DesktopInfo *)lParam)->isCurrent)
            {
                set_screen_info();
                this->NewTasklist();
                update(UPD_DRAW);
            }
            break;

        //=============================================
        // Task was added/removed/activated/has changed text

        case BB_TASKSUPDATE:
            if (TASKITEM_LANGUAGE == lParam)
            {
                /* this message comes here on Win9x only */
                SetTimer(hwnd, LANGUAGE_TIMER, 20, NULL);
                break;
            }

            this->NewTasklist();
            if (false == ShowingExternalLabel)
            {
                this->update_windowlabel();
                this->update(M_WINL);
            }
            if (TASKITEM_MODIFIED == lParam)
            {
                if (this->has_tasks)
                    this->update(M_TASKLIST);
            } else {
                this->update(UPD_DRAW);
            }

            if (TASKITEM_ACTIVATED == lParam)
            {
                if (this->show_kbd_layout)
                    this->set_clock_string();

                if (false == this->toggled_hidden)
                {
                    if (this->autoFullscreenHide && false == this->autoHide) {
                        SetTimer(hwnd, CHECK_FULLSCREEN_TIMER, 10, NULL);
                    } else {
                        this->full_screen_hidden = false;
                        BBP_set_visible(this, false == this->full_screen_hidden);
                    }
                }
            }
            break;

        //=============================================
        // Tray Icon was added/removed/has changed tooltip

        case BB_TRAYUPDATE:
            if (TRAYICON_ADDED == lParam || TRAYICON_REMOVED == lParam)
                this->NewTraylist();

            if (false == this->has_tray)
                break;

            //dbg_printf("BB_TRAYUPDATE %x %d", wParam, lParam);
            if (TRAYICON_MODIFIED == lParam)
            {
                unsigned uChanged = HIWORD(wParam);
                if (NIF_TIP == uChanged || NIF_INFO == uChanged) {
                    this->pLeanBar->settip();
                } else {
                    this->update(M_TRAYLIST);
                }
                break;
            }

            this->update(UPD_DRAW);
            break;

        //=============================================
        case WM_TIMER:
            if (CLOCK_TIMER == wParam)
            {
                this->set_clock_string();
                this->update(M_CLCK);
                break;
            }

#ifndef NO_DROP
            if (TASK_RISE_TIMER == wParam)
            {
                handle_task_timer(m_TinyDropTarget);
                break;
            }
#endif
            KillTimer(hwnd, wParam);

            if (LABEL_TIMER == wParam)
            {
                ShowingExternalLabel = false;
                update_windowlabel();
                this->update(M_WINL);
                break;
            }

            if (TASKLOCK_TIMER == wParam)
            {
                this->task_lock = NULL;
                this->update(M_TASKLIST);
                break;
            }

            if (GESTURE_TIMER == wParam)
            {
                this->gesture_lock = false;
                break;
            }

            if (LANGUAGE_TIMER == wParam)
            {
                this->set_clock_string();
                this->update(M_CLCK);
                break;
            }

            if (CHECK_FULLSCREEN_TIMER == wParam)
            {
                this->full_screen_hidden = check_fullscreen_window();
                BBP_set_visible(this, false == this->full_screen_hidden);
                break;
            }

            break;

        //=============================================
        case BB_DRAGOVER:
            task_over_hwnd = NULL;
            if (this->auto_hidden)
                BBP_set_autoHide(this, true);
            else
                this->pLeanBar->mouse_event((short)LOWORD(lParam), (short)HIWORD(lParam), message, wParam);
            return (LRESULT)task_over_hwnd;

        //=============================================
        case BB_SETTOOLBARLABEL:
            if (NULL == strchr(item_string, M_WINL))
                break;

            SetTimer(hwnd, LABEL_TIMER, 2000, NULL);
            ShowingExternalLabel = true;
            strcpy_max(windowlabel, (LPCSTR)lParam, sizeof windowlabel);
            this->update(M_WINL);
            break;

        case WM_LBUTTONDBLCLK:
            if (wParam & MK_CONTROL) {
                BBP_set_place(this, old_place);
                break;
            }
            goto mouse;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
        case WM_MOUSELEAVE:
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        mouse:
            if (0x8000 & GetAsyncKeyState(VK_MENU))
                wParam |= MK_ALT;
            this->pLeanBar->mouse_event((short)LOWORD(lParam), (short)HIWORD(lParam), message, wParam);
            break;

        case WM_MOUSEACTIVATE:
            return HIWORD(lParam) == WM_MOUSEMOVE ? MA_NOACTIVATE : MA_ACTIVATE;

        //=============================================

        /*case NIN_BALLOONSHOW:
            ((msg_balloon*)lParam)->show();
            break;*/

        //=============================================
        default:
            return DefWindowProc(hwnd,message,wParam,lParam);

    //====================
    }
    return 0;
}

//===========================================================================
// Function: GetRCSettings
//===========================================================================
void barinfo::GetRCSettings()
{
    this->place = POS_BottomCenter; /* default placement */
    BBP_read_window_modes(this, szAppName);

    config* p = cfg_list;
    do switch (p->mode) {
        case R_BOL:
            *(bool*)p->ptr = BBP_read_bool(this, p->str, 0 != (int)(DWORD_PTR)p->def);
            break;

        case R_INT:
            *(int*)p->ptr  = BBP_read_int(this, p->str, (int)(DWORD_PTR)p->def);
            break;

        case R_STR:
            BBP_read_string(this, (char*)p->ptr, p->str, (char*)p->def);
            break;

    } while ((++p)->str);

    // -----------------------------------------------------------------
    // read the bbleanbar.items

    char *ip = item_string;
    LONG pos = 0;

    for (;;)
    {
        int i;
        const char *c;

        c = BBP_read_value(this, "item", &pos);
        if (NULL == c)
            break;

        //dbg_printf("c=<%s> pos=%d", c, pos);

        static const char * const baritemstrings[] = {
            "NewLine",

            "Tasks"    ,
            "Tray"     ,
            "WorkspaceLabel"   ,
            "Clock"    ,
            "WindowLabel"   ,
            "Space",
            "CurrentOnlyButton" ,
            "TaskStyleButton" ,
            "WorkspaceButtonL"  ,
            "WorkspaceButtonR"  ,
            "WindowButtonL"  ,
            "WindowButtonR"  ,
            NULL
        };

        i = 1 + get_string_index(c, baritemstrings);
        if (i)
            *ip++ = (char)i;
    }

    *ip = 0;

    if (0 == item_string[0])
    {
        static char const default_items[] =
        {
            M_WSPL,
            M_WSPB_L,
            M_WSPB_R,
            M_TASK,
            M_CUOB,
            M_TRAY,
            M_WINB_L,
            M_WINB_R,
            M_CLCK,
            0
        };
        strcpy(ip, default_items);
    }

    has_tasks = NULL != strchr(item_string, M_TASK);
    has_tray = NULL != strchr(item_string, M_TRAY);
}

//===========================================================================
// Function: WriteRCSettings
//===========================================================================
void barinfo::WriteRCSettings()
{
    config* p = cfg_list;
    do switch (p->mode) {
        case R_BOL:
            BBP_write_bool(this, p->str, *(bool*)p->ptr);
            break;

        case R_INT:
            BBP_write_int(this, p->str, *(int*)p->ptr);
            break;

        case R_STR:
            BBP_write_string(this, p->str, (char*)p->ptr);
            break;

    } while ((++p)->str);
}

//===========================================================================
// Function: GetStyleSettings
//=========================================================================== 
void barinfo::GetStyleSettings()
{

    StyleItem * T = (StyleItem *)GetSettingPtr(SN_TOOLBAR);
    StyleItem * W = (StyleItem *)GetSettingPtr(SN_TOOLBARWINDOWLABEL);
    StyleItem * L = (StyleItem *)GetSettingPtr(SN_TOOLBARLABEL);
    StyleItem * C = (StyleItem *)GetSettingPtr(SN_TOOLBARCLOCK);
    StyleItem * B = (StyleItem *)GetSettingPtr(SN_TOOLBARBUTTON);

    NTaskStyle = *T;
    if (false == W->parentRelative || W->bordered)
        ATaskStyle = *W;
    else
    if (false == L->parentRelative)
        ATaskStyle = *L;
    else
    if (false == C->parentRelative)
        ATaskStyle = *C;
    else
        ATaskStyle = *W;

    TBJustify = T->Justify | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_NOPREFIX;

    styleBevelWidth = T->marginWidth;
    styleBorderWidth = T->borderWidth;
    styleBorderColor = T->borderColor;

    if (0 == styleBorderWidth)
        styleBorderColor = T->Color;

    if (hFont)
        DeleteObject(hFont), hFont = NULL;
    hFont = CreateStyleFont(T);
    int fontH = get_fontheight(hFont);

#if 1
    {
#   define VALID_MARGIN 0x200
    // xxx old behaviour xxx
    void *p = GetSettingPtr(SN_ISSTYLE070);
    bool is_style070 = HIWORD(p) ? *(bool*)p : NULL!=p;
    if (false == is_style070 && 0 == (L->validated & VALID_MARGIN))
        fontH = T->FontHeight-(T->nVersion < 3 ? 0 : 2);
    //xxxxxxxxxxxxxxxxxxxxxx
    }
#endif

    labelBorder = imax(L->borderWidth, imax(C->borderWidth, W->borderWidth));
    labelH  = (fontH|1) + 2*(L->marginWidth/*+labelBorder*/);

    buttonBorder = B->borderWidth;
    labelIndent = imax(2+labelBorder, (labelH-fontH)/2);

    int iconMargin = imax(2, 2*W->borderWidth);
    int margin = 2*T->marginWidth;

    int lbl_max = labelH - iconMargin;

    if (false == smallIcons || lbl_max >= 16) {
        // with icons>=16 we give up button symmetrie
        // for the sake of icon symmetrie
        lbl_max = imax(lbl_max & ~1, 16);
    }

    TASK_ICON_SIZE = imin(lbl_max, 16);

    lbl_max += iconMargin;;
    labelH = lbl_max;
    buttonH = labelH + 2*(B->marginWidth-L->marginWidth);
    lbl_max += margin;

    int bar_max = imax(labelH, buttonH) + margin;
    int tray_max = bar_max - margin;

    TRAY_ICON_SIZE = imin(tray_max, 16);

    // calculate the bar height(s)
    BarLines = 0;
    this->height = 0;

    char c, *ip = item_string;
    for (;;)
    {
        // check, if a line has an icon
        int h = bar_max;
        for (;0 != (c = *ip++) && M_NEWLINE != c;) {
            if (M_TASK == c) {
                 h = imax(h, lbl_max);
            }
            else if (M_TRAY == c) {
                 h = imax(h, tray_max);
            }
        }

        h += 2 * T->borderWidth;
        this->height += h;
        bbLeanBarLineHeight[BarLines++] = h;
        if (0 == c)
            break;

        this->height -= T->borderWidth;
    }

    // set location on the screen
    this->width = (this->mon_rect.right - this->mon_rect.left) * widthPercent / 100;
    BBP_reconfigure(this);

    on_multimon = GetSystemMetrics(SM_CMONITORS) > 1;
    //dbg_printf("Monitors: %d %d", GetSystemMetrics(SM_CMONITORS), on_multimon);
}

void barinfo::process_broam(const char *temp, int f)
{
    if (f)
    {
        show_menu(false);
        PostMessage(hwnd, BB_RECONFIGURE, 0, 0);
        return;
    }

    //-----------------------------------
    // check config menu messages

    pmenu* cp;
    for (cp = cfg_menu; cp->displ; cp++)
    {
        int n = strlen(cp->msg);
        if (n && 0 == memicmp(temp, cp->msg, n))
        {
            if (cp->ptr)
            {
                temp += n + (0 != temp[n]);
                if (cp->f & CFG_INT)
                    *(int*)cp->ptr = atoi(temp);
                else
                if (cp->f & CFG_INT2)
                    *(int*)cp->ptr = atoi(temp-1);
                else
                if (cp->f & CFG_STR)
                    strcpy((char*)cp->ptr, temp);
                else
                if (0 == stricmp(temp, "true"))
                    *(bool*)cp->ptr = true;
                else
                if (0 == stricmp(temp, "false"))
                    *(bool*)cp->ptr = false;
                else
                    *(bool*)cp->ptr = false == *(bool*)cp->ptr;
            }
            WriteRCSettings();
            show_menu(false);
            PostMessage(hwnd, BB_RECONFIGURE, 0, 0);
            return;
        }
    }

    if (exec_sysmenu_command(temp, sendToSwitchTo))
        return;

    int index;
    if (1 == sscanf(temp, "tray.showicon %d", &index))
    {
        trayShowIcon(index-1, -1);
        return;
    }

    if (0 == strcmp(temp, "tray.showall"))
    {
        trayToggleShowAll(-1);
        return;
    }
}


//===========================================================================
// Function: show_menu
// Purpose: ...
// In: void
// Out: void
//===========================================================================
void barinfo::show_menu(bool pop)
{
    n_menu *myMenu, *cSub, *wSub, *m;
    pmenu* cp = cfg_menu;

    myMenu = n_makemenu(this->instance_key);
    if (false == this->inSlit)
        BBP_n_placementmenu(this, myMenu);

    m = cSub = n_submenu(myMenu, "Configuration");
    do {
        if (cp->f & CFG_MAIN)
            m = cSub;

        if (cp->f == CFG_SUB)
            m = n_submenu(cSub, cp->displ);
        else
        if (cp->f == CFG_WIN)
            BBP_n_insertmenu(this, m);
        else
        if (cp->f & CFG_TASK && false == has_tasks)
            ;
        else
        if (cp->f & CFG_INT)
            n_menuitem_int(m, cp->displ, cp->msg, *(int*)cp->ptr, 0, cp->f & CFG_255 ? 255 : 100);
        else
        if (cp->f & CFG_INT2)
            n_menuitem_bol(m, cp->displ, cp->msg, *(int*)cp->ptr == (cp->f >> 8));
        else
        if (cp->f & CFG_STR)
            n_menuitem_str(m, cp->displ, cp->msg, (char*)cp->ptr);
        else
        if (0 == cp->displ[0])
            n_menuitem_nop(m, NULL);
        else
            n_menuitem_bol(m, cp->displ, cp->msg, cp->ptr ? *(bool*)cp->ptr : false);
    } while ((++cp)->displ);

    wSub = n_submenu(myMenu, "Window");
    n_menuitem_cmd(wSub, "Minimize All", "@BBCore.MinimizeAll");
    n_menuitem_cmd(wSub, "Restore All", "@BBCore.RestoreAll");
    n_menuitem_cmd(wSub, "Cascade", "@BBCore.Cascade");
    n_menuitem_cmd(wSub, "Tile Horizontal", "@BBCore.TileHorizontal");
    n_menuitem_cmd(wSub, "Tile Vertical", "@BBCore.TileVertical");
    n_menuitem_nop(myMenu, NULL);
    n_menuitem_cmd(myMenu, "Edit Workspace Names", "@BBCore.EditWorkspaceNames");
    n_menuitem_cmd(myMenu, "Edit Settings", "EditRc");
    n_menuitem_cmd(myMenu, "View Readme", "Readme");
    n_menuitem_cmd(myMenu, "About", "About");
    n_showmenu(this, myMenu, pop, 0);
}

void barinfo::pos_changed(void)
{
    POINT pt;
    pt.x = mon_rect.left;
    pt.y = mon_rect.top;
    ScreenToClient(hwnd, &pt);

    mon_top = pt.y;
    mon_bottom = pt.y + mon_rect.bottom - mon_rect.top;

    if (this->place != POS_User && this->place < POS_CenterLeft)
        this->old_place = this->place;

    desktop_margin_fn();
    //dbg_printf("pos_changed");

    if (TBInfo)
    {
        TBInfo->placement        = PLACEMENT_TOP_LEFT + (place - POS_TopLeft);
        TBInfo->widthPercent     = widthPercent;
        TBInfo->onTop            = alwaysOnTop;
        TBInfo->autoHide         = autoHide;
        TBInfo->pluginToggle     = pluginToggle;
        TBInfo->disabled         = false;

        TBInfo->alphaValue       = (BYTE)alphaValue;
        TBInfo->transparency     = alphaValue < 255;

        TBInfo->autohidden       = auto_hidden;
        TBInfo->hidden           = false == is_visible;

        TBInfo->xpos             = xpos;
        TBInfo->ypos             = ypos;
        TBInfo->width            = width;
        TBInfo->height           = height;
        TBInfo->hwnd             = hwnd;

        if (TBInfo->bbsb_hwnd)
            PostMessage(TBInfo->bbsb_hwnd, BB_TOOLBARUPDATE, 0, 0);
    }
}

void barinfo::set_tbinfo(void)
{
    if (this->inSlit)
    {
        reset_tbinfo();
        return;
    }

    ToolbarInfo *TBInfo = GetToolbarInfo();
    if (TBInfo && NULL == TBInfo->hwnd)
        (this->TBInfo = TBInfo)->hwnd = this->hwnd;
}

void barinfo::reset_tbinfo(void)
{
    if (TBInfo)
    {
        TBInfo->hwnd = NULL;
        TBInfo->disabled = TBInfo->hidden = true;
        if (TBInfo->bbsb_hwnd)
            PostMessage(TBInfo->bbsb_hwnd, BB_TOOLBARUPDATE, 0, 0);
        TBInfo = NULL;
    }
}
