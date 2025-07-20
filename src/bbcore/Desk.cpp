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

#include <shlobj.h>
#include <shellapi.h>

#include "BB.h"
#include "bbrc.h"
#include "Desk.h"
#include "Utils.h"
#include "Settings.h"
#include "Workspaces.h"
#include "MessageManager.h"
#include "Menu/MenuMaker.h"
#include "Blackbox.h"
#include "BBRootImage.h" 
#include "nls.h" 
#include "rcfile.h"

#ifndef MK_ALT
#define MK_ALT 0x20
#endif

#ifndef XBUTTON3
#define XBUTTON3 0x0004
#endif

namespace bbcore { 


HWND hDesktopWnd;

#ifndef BBTINY

static const char szDesktopName[] = "DesktopBackgroundClass";
static struct BBRootImageInfo {
    HBITMAP bmp;
    char command[MAX_PATH];
} gRoot;

static LRESULT CALLBACK Desk_WndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void set_bitmap(HBITMAP bmp);


static int get_drop_command(const char *filename, int flags);
unsigned get_modkeys(void);

static void (*pSetHooks)(HWND BlackboxWnd, int flags);
static const char deskhook_dll [] = "deskhook.dll";

static class DeskDropTarget *m_DeskDropTarget;
static void init_DeskDropTarget(HWND hwnd);
static void exit_DeskDropTarget(HWND hwnd);

// --------------------------------------------
void Desk_Init(void)
{
    if (Settings_disableDesk)
        ;
    else
    if (Settings_desktopHook)
    {
        if (load_imp(&pSetHooks, deskhook_dll, "SetHooks"))
            pSetHooks(gBBhwnd, gUnderExplorer);
        else
            bbMessageBox(MB_OK, NLS2("$Error_DesktopHook$",
                "Error: %s not found!"), deskhook_dll);
    }
    else
    {
        BBRegisterClass(szDesktopName, Desk_WndProc, BBCS_VISIBLE);
        CreateWindowEx(
            WS_EX_TOOLWINDOW,
            szDesktopName,
            NULL,
            WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
            0,0,0,0,
            GetDesktopWindow(),
            NULL,
            hMainInstance,
            NULL
            );
    }
    Desk_new_background(NULL);
}

// --------------------------------------------
void Desk_Exit(void)
{
    if (have_imp(pSetHooks)) {
        pSetHooks(NULL, 0);
        FreeLibrary(GetModuleHandle(deskhook_dll));
    }
    pSetHooks = NULL;

    if (hDesktopWnd) {
        DestroyWindow(hDesktopWnd);
        UnregisterClass(szDesktopName, hMainInstance);
        hDesktopWnd = NULL;
    }

    Desk_Reset(true);
}

//===========================================================================

static
void Desk_SetPosition(void)
{
    SetWindowPos(
        hDesktopWnd,
        HWND_BOTTOM,
        VScreenX, VScreenY, VScreenWidth, VScreenHeight,
        SWP_NOACTIVATE|SWP_NOSENDCHANGING
        );
}

static
void set_bitmap(HBITMAP bmp)
{
    if (gRoot.bmp)
        DeleteObject(gRoot.bmp);
    gRoot.bmp = bmp;
    if (hDesktopWnd)
        InvalidateRect(hDesktopWnd, NULL, FALSE);
}

void Desk_Reset(bool all)
{
    if (all)
        set_bitmap(NULL);
    gRoot.command [0] = 0;
}

HBITMAP Desk_getbmp(void)
{
    return gRoot.bmp;
}

//===========================================================================

void Desk_new_background(const char *p)
{
    bool makebmp;

    p = Desk_extended_rootCommand(p);
    if (p)
    {
        if (0 == stricmp(p, "none"))
            p = "";
        if (0 == stricmp(p, "style"))
            p = NULL;
    }
    if (false == Settings_enableBackground)
        p = "";
    else
    if (NULL == p)
        p = mStyle.rootCommand;

    makebmp = hDesktopWnd && Settings_smartWallpaper;
    if (0 == strcmp(gRoot.command, p) && *p && (gRoot.bmp || !makebmp))
        return;
    set_bitmap(load_desk_bitmap(p, makebmp));
    if (hDesktopWnd)
        Desk_SetPosition();
    strcpy(gRoot.command, p);
    if (usingVista && 0 == *p)
    {
        SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, NULL, SPIF_SENDCHANGE);
    }
}

//===========================================================================
// Desktop's window procedure

static
LRESULT CALLBACK Desk_WndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static const UINT msgs [] = { BB_DRAGTODESKTOP, BB_REDRAWGUI, 0 };
    static bool button_down, dblclk;
    int n;

    switch (uMsg)
    {
        //====================
        case WM_CREATE:
            hDesktopWnd = hwnd;
            makeSticky(hwnd);
            MessageManager_Register(hwnd, msgs, true);
            init_DeskDropTarget(hwnd);
            Desk_SetPosition();
            break;

        //====================
        case WM_DESTROY:
            exit_DeskDropTarget(hwnd);
            MessageManager_Register(hwnd, msgs, false);
            removeSticky(hwnd);
            break;

        case WM_NCPAINT:
            // debug_printf("ncpaint: %x %x %x %x", hwnd, uMsg, wParam, lParam);
            // keep the window on bottom
            Desk_SetPosition();
            break;

        case WM_SETTINGCHANGE:
            if (SPI_SETDESKWALLPAPER == wParam)
                InvalidateRect(hwnd, NULL, FALSE);
            break;

        //====================
        case WM_CLOSE:
            break;

        //====================
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            dblclk = false;
            button_down = true;
            if (uMsg == WM_LBUTTONDOWN) {
                n = 0;
                goto post_click_2;
            }
            break;

        case WM_MOUSEMOVE:
            break;

        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
            dblclk = true;
            button_down = true;
            break;

        case WM_LBUTTONUP: n = dblclk ? 7 : 1; goto post_click;
        case WM_RBUTTONUP: n = 2; goto post_click;
        case WM_MBUTTONUP: n = 3; goto post_click;
        case WM_XBUTTONUP:
            switch (HIWORD(wParam)) {
            case XBUTTON1: n = 4; goto post_click;
            case XBUTTON2: n = 5; goto post_click;
            case XBUTTON3: n = 6; goto post_click;
            } break;

        post_click:
            if (false == button_down)
                break;
            button_down = dblclk = false;

        post_click_2:
            wParam &= (MK_CONTROL|MK_SHIFT);
            if (0x8000 & GetAsyncKeyState(VK_MENU))
                wParam |= MK_ALT;

            PostMessage(gBBhwnd, BB_DESKCLICK, wParam, n);
            break;

        //====================

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc_scrn;
            HDC hdc_bmp;
            HGDIOBJ other;

            hdc_scrn = BeginPaint(hwnd, &ps);
            if (gRoot.bmp) {
                hdc_bmp = CreateCompatibleDC(hdc_scrn);
                other = SelectObject(hdc_bmp, gRoot.bmp);
                BitBltRect(hdc_scrn, hdc_bmp, &ps.rcPaint);
                SelectObject(hdc_bmp, other);
                DeleteDC(hdc_bmp);
            } else {
                PaintDesktop(hdc_scrn);
            }
            EndPaint(hwnd, &ps);
            break;
        }

        //====================
        case WM_ERASEBKGND:
            return TRUE;

        //====================
        case BB_DRAGTODESKTOP:
            return get_drop_command((const char *)lParam, wParam);

        case BB_REDRAWGUI:
            if (wParam & BBRG_DESK)
                Desk_new_background("style");
            break;

        //====================
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

//===========================================================================
// figure out whether we want the file (images and styles, that is)

static
int get_drop_command(const char *filename, int flags)
{
    const char *e;

    e = file_extension(filename);
    if (*e && stristr(".bmp.gif.png.jpg.jpeg", e)) {
        unsigned modkey = get_modkeys() & (MK_ALT|MK_SHIFT|MK_CONTROL);
        const char *mode;
        if (0 == modkey)
            mode = "full";
        else
        if (MK_SHIFT == modkey)
            mode = "center";
        else
        if (MK_CONTROL == modkey)
            mode = "tile";
        else
            return 0;
        if (0 == (flags & 1))
        {
            post_command_fmt("@BBCore.rootCommand bsetroot -%s \"%s\"", mode, filename);
        }
        return 1;
    }

    // whether its a style is checked only once by looking at
    // the file's contents on DragEnter
    if (flags & 2) {
        if (0 == (flags & 1))
        {
            post_command_fmt(MM_STYLE_BROAM, filename);
        }
        return 1;
    }

    return 0;
}

//===========================================================================
#endif //ndef BBTINY
//===========================================================================
// get/set/reset a custom rootcommand (e.g. with dropped images on desktop)

const char * Desk_extended_rootCommand(const char *p)
{
    const char rc_key [] = "blackbox.background.rootCommand";
    const char *extrc = bbExtensionsRcPath(NULL);
    if (p)
        write_string(extrc, rc_key, p);
    else
        p = read_string(extrc, rc_key, NULL);
    return p;
}

//===========================================================================
// read desktop mouse click events from extensions.rc and execute

static const unsigned char mk_mods[] =
{ MK_ALT, MK_SHIFT, MK_CONTROL, MK_LBUTTON, MK_RBUTTON, MK_MBUTTON };

static const char modkey_strings[][6] =
{ "Alt", "Shift", "Ctrl", "Left", "Right", "Mid"  };

static const unsigned short vk_codes[] =
{ VK_MENU, VK_SHIFT, VK_CONTROL, VK_LBUTTON, VK_RBUTTON, VK_MBUTTON };

static const char button_strings[][7] =
{ "Left", "Right", "Mid", "X1", "X2", "X3", "Double" };

unsigned get_modkeys(void)
{
    unsigned modkey = 0;
    int i = 0;
    do if (0x8000 & GetAsyncKeyState(vk_codes[i]))
        modkey |= mk_mods[i];
    while (++i < array_count(vk_codes));
    return modkey;
}

bool Desk_mousebutton_event(int button)
{
    char rc_key[100];
    unsigned modkey;
    int i;
    const char *broam;

    if (button < 1 || button > array_count(button_strings))
        return false;

    modkey = get_modkeys();
    if (button == 1 && 0 == modkey)
        return false;

    strcpy(rc_key, "blackbox.desktop.");
    for (i = 0; i < array_count(mk_mods); ++i)
        if (mk_mods[i] & modkey)
            strcat(rc_key, modkey_strings[i]);
    sprintf(strchr(rc_key, 0), "%sClick", button_strings[button-1]);

    broam = read_string(bbExtensionsRcPath(NULL), rc_key, NULL);
    // debug_printf("%s - %s", rc_key, broam);

    if (broam)
    {
        post_command(broam);
        return true;
    }

    if (2 == button && 0 == modkey) {
        PostMessage(gBBhwnd, BB_MENU, BB_MENU_ROOT, 0);
        return true;
    }
    if ((3 == button && 0 == modkey) || (2 == button && MK_SHIFT == modkey)) {
        PostMessage(gBBhwnd, BB_MENU, BB_MENU_TASKS, 0);
        return true;
    }
    return false;

}

//===========================================================================
// Show / Hide Explorer parts when running on top of the native shell

static hwnd_list* basebarlist;

static
void hidewnd (HWND hwnd)
{
    if (hwnd && ShowWindow(hwnd, SW_HIDE))
        cons_node(&basebarlist, new_node(hwnd));
}

static
BOOL CALLBACK HideBaseBars(HWND hwnd, LPARAM lParam)
{
    char temp[32];
    if (GetClassName(hwnd, temp, sizeof temp)
     && (0 == strcmp(temp, "BaseBar")
      || 0 == strcmp(temp, "Button")
        ))
        hidewnd(hwnd);
    return TRUE;
}

void HideExplorer(void)
{
    HWND hw;
    hw = FindWindow("Progman", "Program Manager");
    if (hw) {
        if (Settings_hideExplorer) {
            hidewnd(hw);
        } else {
            makeSticky(hw);
        }
    }

    hw = FindWindow("Shell_TrayWnd", NULL);
    if (hw) {
        if (Settings_hideExplorerTray) {
            hidewnd(hw);
            EnumWindows(HideBaseBars, 0);
        } else {
            makeSticky(hw);
        }
    }
}

void ShowExplorer(void)
{
    hwnd_list* p;
    dolist (p, basebarlist)
        ShowWindow(p->hwnd, SW_SHOW);
    freeall(&basebarlist);
}

//===========================================================================
// The COM object responsible for drag'n dropping on desktop

#ifndef BBTINY

class DeskDropTarget : public IDropTarget
{
public:
    DeskDropTarget();
    virtual ~DeskDropTarget();

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

private:
    // IDropTarget methods
    STDMETHOD(DragEnter)(LPDATAOBJECT pDataObject, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
    STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
    STDMETHOD(Drop)(LPDATAOBJECT pDataObject, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
    STDMETHOD(DragLeave)();

    DWORD drop_check(int flag);

    DWORD m_dwRef;
    char m_filename[MAX_PATH];
    int m_flags;
};

DeskDropTarget::DeskDropTarget()
{
    m_dwRef = 1;
}

DeskDropTarget::~DeskDropTarget()
{
}

COM_DECLSPEC_NOTHROW STDMETHODIMP DeskDropTarget::QueryInterface(REFIID iid, void** ppvObject)
{
    if (IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IDropTarget))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    *ppvObject = NULL;
    return E_NOINTERFACE;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) DeskDropTarget::AddRef()
{
    return ++m_dwRef;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) DeskDropTarget::Release()
{ 
    int r;
    if (0 == (r = --m_dwRef))
        delete this;
    return r; 
}

COM_DECLSPEC_NOTHROW STDMETHODIMP DeskDropTarget::DragEnter(LPDATAOBJECT pDataObject,
    DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
    AddRef();
    m_filename[0] = 0;
    if (pDataObject)
    {
        FORMATETC fmte;
        fmte.cfFormat   = CF_HDROP;
        fmte.ptd        = NULL;
        fmte.dwAspect   = DVASPECT_CONTENT;  
        fmte.lindex     = -1;
        fmte.tymed      = TYMED_HGLOBAL;

        STGMEDIUM medium;
        if (SUCCEEDED(pDataObject->GetData(&fmte, &medium)))
        {
            HDROP hDrop = (HDROP)medium.hGlobal;
            DragQueryFile(hDrop, 0, m_filename, sizeof(m_filename));
            DragFinish(hDrop);
            m_flags = is_stylefile(m_filename) ? 2 : 0;
        }
    }
    *pdwEffect = DROPEFFECT_NONE;
    return S_OK;
}

DWORD DeskDropTarget::drop_check(int flag)
{
    if (m_filename[0]
        && SendMessage(gBBhwnd, BB_DRAGTODESKTOP, m_flags|flag, (LPARAM)m_filename))
        return DROPEFFECT_LINK|DROPEFFECT_COPY;
    else
        return DROPEFFECT_NONE;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP DeskDropTarget::DragOver(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
    *pdwEffect = drop_check(1);
    return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP DeskDropTarget::Drop(LPDATAOBJECT pDataObject,
    DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
    *pdwEffect = drop_check(0);
    return DragLeave();
}

COM_DECLSPEC_NOTHROW STDMETHODIMP DeskDropTarget::DragLeave()
{
    Release();
    return S_OK;
}

//===========================================================================
static
void init_DeskDropTarget(HWND hwnd)
{
    m_DeskDropTarget = new DeskDropTarget();
    RegisterDragDrop(hwnd, m_DeskDropTarget);
}

static
void exit_DeskDropTarget (HWND hwnd)
{
    RevokeDragDrop(hwnd);
    m_DeskDropTarget->Release();
}

//===========================================================================
#endif //ndef BBTINY


} // !namespace bbcore
