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
#include "Menu.h"
#include "nls.h"
#include "system.h"
#include "BImage.h"

#include "MenuItem.h"
#include "SeparatorItem.h"
#include "TitleItem.h"
#include "IntegerItem.h"
#include "StringItem.h"
#include "SpecialFolderItem.h"
#include "SFInsertItem.h"
#include "Dragsource.h"
#include "Droptarget.h" 
#include "MenuMaker.h"
#include "commands.h"
#include "MenuMaker.h"

namespace bbcore {


/// \brief inserts an item to execute a command or to set a boolean value
MenuItem* makeMenuItem(Menu* PluginMenu, const char* Title, const char* Cmd, bool ShowIndicator)
{
    //debug_printf("MakeMenuItem %x %s", PluginMenu, Title);
    return PluginMenu->AddMenuItem(new CommandItem(Cmd, NLS1(Title), ShowIndicator));
}

/// \brief Inserts an inactive item, optionally with text. 'Title' may be NULL.
MenuItem* makeMenuNOP(Menu* PluginMenu, const char* Title)
{
    MenuItem* pItem{};
    if (Title && Title[0])
    {
        pItem = new MenuItem(NLS1(Title));
    }
    else
    {
        pItem = new SeparatorItem();
    }
    pItem->m_bNOP = true;
    return PluginMenu->AddMenuItem(pItem);
}

/// \brief inserts an item to adjust a numeric value
MenuItem* makeMenuItemInt(Menu* PluginMenu, const char* Title, const char* Cmd,
    int val, int minval, int maxval)
{
    auto item = new IntegerItem(Cmd, val, minval, maxval);
    return bbcore::helper_menu(PluginMenu, Title, MENU_ID_INT, item);
}

/// \brief inserts an item to edit a string value
MenuItem* makeMenuItemString(Menu* PluginMenu, const char* Title, const char* Cmd,
    const char* init_string)
{
    auto item = new StringItem(Cmd, init_string);
    return bbcore::helper_menu(PluginMenu, Title, MENU_ID_STRING, item);
}

/// \brief inserts an item, which opens a submenu
MenuItem* makeSubmenu(Menu* ParentMenu, Menu* ChildMenu, const char* Title)
{
    //debug_printf("bbcore::makeSubmenu %x %s - %x %s", ParentMenu, ParentMenu->getMenuItems()->m_pszTitle, ChildMenu, Title);
    if (Title)
    {
        Title = NLS1(Title);
    }
    else
    {
        Title = ChildMenu->getMenuItems()->m_pszTitle;
    }
    return ParentMenu->AddMenuItem(new FolderItem(ChildMenu, Title));
}

/// \brief Inserts an item, which opens a submenu from a system folder.
//  'Cmd' optionally may be a Broam which then is sent on user click
//  with "%s" in the broam string replaced by the selected filename
MenuItem* makeMenuItemPath(Menu* ParentMenu, const char* Title, const char* path, const char* Cmd)
{
    MenuItem* pItem{};
    if (Title)
    {
        pItem = new bbcore::SpecialFolderItem(NLS1(Title), path, NULL, Cmd);
    }
    else
    {
        pItem = new SFInsert(path, Cmd);
    }
    return ParentMenu->AddMenuItem(pItem);
}

/// \brief Shows the menu
void showMenu(Menu* pluginMenu)
{
    if (nullptr == pluginMenu)
    {
    }
    // debug_printf("ShowMenu(%d) %x %s", pluginMenu->m_bPopup, pluginMenu, pluginMenu->getMenuItems()->m_pszTitle);

    if (pluginMenu->isPopup())
    {
        // just to signal e.g. BBSoundFX
#ifndef BBXMENU
        PostMessage(bbcore::gBBhwnd, BB_MENU, BB_MENU_SIGNAL, 0);
#endif
        pluginMenu->ShowMenu();
    }
    else
    {
        pluginMenu->Redraw(1);
        pluginMenu->decref();
    }
}

/// \brief Checks whether a menu with ID starting with 'IDString_start', still
///        exists
bool menuExists(const char* IDString_part)
{
    return nullptr != Menu::find_named_menu(IDString_part, true);
}

/// \brief Set option for MenuItem
void menuItemOptionV(MenuItem* pItem, int option, va_list vl)
{
    if (NULL == pItem)
    {
        return;
    }

    switch (option)
    { 
        // disabled text style
        case BBMENUITEM_DISABLED:
            pItem->m_bDisabled = true;
            if (NULL == pItem->m_pszRightCommand) /* hack for the taskmenus */
                pItem->m_bNOP = true;
            break;

        // set checkmark
        case BBMENUITEM_CHECKED:
            pItem->m_bChecked = true;
            break;

        // set a command for left click
        case BBMENUITEM_LCOMMAND:
            replace_str(&pItem->m_pszCommand, va_arg(vl, const char*));
            break;

        // set a command for right click
        case BBMENUITEM_RCOMMAND:
            replace_str(&pItem->m_pszRightCommand, va_arg(vl, const char*));
            break;

        // set a command for right click
        case BBMENUITEM_RMENU:
        {
            Menu *pSub = pItem->m_pRightmenu = va_arg(vl, Menu*);
            pSub->setMenuId(MENU_ID_RMENU);
            break;
        }

        // set a special value and display text for the integer items
        case BBMENUITEM_OFFVAL:
        {
            IntegerItem* IntItem = (IntegerItem*)pItem->get_real_item();
            if (IntItem && IntItem->m_ItemID == MENUITEM_ID_INT) {
                const char *p;
                int n;
                if (NULL == IntItem)
                    break;
                n = va_arg(vl, int);
                p = va_arg(vl, const char*);
                IntItem->m_offvalue = n;
                IntItem->m_offstring = p ? p : NLS0("off");
            }
            break;
        }

        // set a flag that the checkmarks are updated each time
        case BBMENUITEM_UPDCHECK:
            pItem->m_ItemID |= MENUITEM_UPDCHECK;
            break;

        // set a special justify mode (DT_LEFT/DT_CENTER/DT_RIGHT)
        case BBMENUITEM_JUSTIFY:
            pItem->m_Justify = va_arg(vl, int);
            break;

#ifdef BBOPT_MENUICONS
        // set an icon for this item by "path\to\icon[,#iconid]"
        case BBMENUITEM_SETICON:
            replace_str(&pItem->m_pszIcon, va_arg(vl, const char*));
            break;

        // set an icon for this item by HICON
        case BBMENUITEM_SETHICON:
            pItem->m_hIcon = CopyIcon(va_arg(vl, HICON));
            break;
#endif
    } 
}

void menuItemOption(MenuItem *pItem, int option, ...)
{
    if (nullptr == pItem)
    {
        return;
    }

    va_list vl{};
    va_start(vl, option);
    bbcore::menuItemOptionV(pItem, option, vl);
    va_end(vl);
} 

static int item_compare(MenuItem** m1, MenuItem** m2)
{
    return stricmp((*m1)->m_pszTitle, (*m2)->m_pszTitle);
}

/// \brief Set some special features for a individual menu
void menuOptionV(Menu* pMenu, int flags, va_list vl)
{
    if (nullptr == pMenu)
    {
        return;
    }

    int pos = flags & BBMENU_POSMASK;
    pMenu->rflags() |= (flags & ~BBMENU_POSMASK) | pos;

    if (pos == BBMENU_XY)
    {
        pMenu->data.m_pos.left = va_arg(vl, int),
        pMenu->data.m_pos.top = va_arg(vl, int);
    }
    else if (pos == BBMENU_RECT)
    {
        pMenu->data.m_pos = *va_arg(vl, RECT*);
    }

    if (flags & BBMENU_MAXWIDTH)
    {
        pMenu->setMaxWidth(va_arg(vl, int));
    }

    if (flags & BBMENU_HWND)
    {
        pMenu->data.m_hwndRef = va_arg(vl, HWND);
    }

    if (flags & BBMENU_ISDROPTARGET)
    {
        pMenu->setIsDropTarget(true);
    }

    if (flags & BBMENU_SORT)
    {
        Menu::Sort(&pMenu->getMenuItems()->next, item_compare); 
    }
}

void menuOption(Menu *pMenu, int flags, ...)
{
    if (NULL == pMenu)
    {
        return;
    }

    va_list vl{}; 
    va_start(vl, flags);
    bbcore::menuOptionV(pMenu, flags, vl); 
    va_end(vl);
}

/// \brief Create or refresh a Menu. For menus that dont need refreshing
Menu* makeMenu(const char* headerText)
{
    return bbcore::makeNamedMenu(headerText, nullptr, true);
}

void delMenu(Menu* pluginMenu)
{
    // Nothing here. We just dont know wether 'pluginMenu' still
    // exists. The pointer may be invalid or even belong to
    // a totally different memory object.
}

int g_menu_item_count{};

MenuInfo gMenuInfo{};

}; //!namespace bbcore

// #define CHECKFOCUS

static int g_menu_ref;
int g_menu_count;
// When the menu is invoked with a keystroke, some mouse moves are
// discarded to avoid unwanted submenu popups.
static int g_ignore_mouse_msgs;

// these are used during sorting
static int g_sortmode;
static int g_sortrev;
static IShellFolder* g_psf;

bbcore::MenuList* Menu::g_MenuStructList;   // all instances of 'class Menu'
bbcore::MenuList* Menu::g_MenuWindowList;   // all actually visible menus
bbcore::MenuList* Menu::g_MenuRefList;      // menus for deletion

//===========================================================================
// Menu constructor

Menu::Menu(const char *pszTitle)
{ 
    setIsOnTop(bbcore::gSettingsMenu.onTop);
    setMaxWidth(bbcore::gMenuInfo.MaxWidth);
    data.m_bPopup = true;

    // Add TitleItem. This will remain until the menu is
    // destroyed, while other items may change on updating.
    setLastItem(new bbcore::TitleItem(pszTitle));
    setMenuItems(getLastItem());
    getLastItem()->m_pMenu = this;

    // add to the global menu list. It is used to see
    // if a specific menu exists (with 'find_named_menu')
    cons_node(&g_MenuStructList, new_node(this));
    ++g_menu_count;

    // start with a refcount of 1, assuming that
    // the menu is either shown or linked as submenu.
    data.m_refc = 1;
}

//==============================================
// Menu destructor

Menu::~Menu(void)
{
    DeleteMenuItems();
    delete getMenuItems(); // TitleItem
    setMenuItems(nullptr);
    free_str(&data.m_IDString);
    delete_pidl_list(&data.m_pidl_list);

    // remove from the list
    remove_assoc(&g_MenuStructList, this);
    --g_menu_count;
}

//==============================================
// Menu destruction is controlled by reference counter.
// References are:
// - having a Window
// - being a submenu, i.e. bound to m_pSubmenu of an item

int Menu::decref(void)
{
    //debug_printf("decref %d %s", m_refc-1, getMenuItems()->m_pszTitle);
    int n = data.m_refc - 1;
    if (n)
        return data.m_refc = n;
    if (g_menu_ref) {
        // delay deletion
        cons_node(&g_MenuRefList, new_node(this));
        return n+1;
    }
    delete this;
    return n;
}

int Menu::incref(void)
{
    //debug_printf("incref %d %s", m_refc, getMenuItems()->m_pszTitle);
    return ++data.m_refc;
}

//==============================================
// Global reference count - controls access of ANY menu through either:
// - the menu window_proc or
// - one of the global menu functions, i.e. bbcore::Menu_All_Hide()

void Menu::g_incref()
{
    ++g_menu_ref;
}

void Menu::g_decref()
{
    bbcore::MenuList *ml;
    if (--g_menu_ref || NULL == g_MenuRefList)
        return;
    // debug_printf("MenuDelList %d", listlen(g_MenuRefList));
    dolist (ml, g_MenuRefList)
        ml->m->decref();
    freeall(&g_MenuRefList);
}

//===========================================================================
// Give a window to the menu, when it's about to be shown

void Menu::make_menu_window(void)
{
    if (getWindowHandle())
        return;

    data.m_bMouseOver = false;
    data.m_kbditempos = -1;
    data.m_alpha = 255;
    incref();

    auto hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        className(),      // window class name
        NULL,               // window title
        WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0, 0, 0,
        NULL,               // parent window
        NULL,               // no menu
        bbcore::hMainInstance,      // hInstance
        this                // creation data - link to the object
        );
    setWindowHandle(hwnd);

    register_droptarget(true);
    insert_at_last();
}

const char* Menu::className(void)
{ 
    static const char MenuClassName[] = "BBMenu";
    return MenuClassName;
}

//===========================================================================
// Remove the window, when the menu is hidden

void Menu::destroy_menu_window(bool force)
{
    HWND hwnd = getWindowHandle();
    if (NULL == hwnd)
        return;

    post_autohide();

    if (getChildMenu())
        getChildMenu()->LinkToParentItem(NULL);

    if (getParentMenu()) {
        if (getMenuId() & (MENU_ID_SF|MENU_ID_SHCONTEXT|MENU_ID_RMENU)
            && 0 == (getParentMenu()->getMenuId() & MENU_ID_SHCONTEXT)) {
            // unlink folders and context menus since these can
            // be remade any time
            getParentMenuItem()->UnlinkSubmenu();
        } else {
            LinkToParentItem(NULL);
        }
    }

    // the OS does not seem to like that a window is destroyed while
    // it is dragsource (may drop just somewhere) or droptarget
    // (wont be released)
    if (false == force && (data.m_bInDrag || in_drop(data.m_droptarget)))
        return;

    register_droptarget(false);
    remove_assoc(&g_MenuWindowList, this);
    setWindowHandle(NULL);
    data.m_hwndRef = NULL;
    DestroyWindow(hwnd);
    if (data.m_hBitMap)
        DeleteObject(data.m_hBitMap), data.m_hBitMap = NULL;
    decref();
}

//===========================================================================
void Menu::post_autohide()
{
    if (getFlags() & BBMENU_HWND) {
        PostMessage(data.m_hwndRef, BB_AUTOHIDE, 0, 1);
        rflags() &= ~BBMENU_HWND;
    }
}

//===========================================================================
LRESULT CALLBACK Menu::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Menu *p = (Menu*)GetWindowLongPtr(hwnd, 0);
    if (p)
    {
        LRESULT result;
        p->incref(), g_incref();
        result = p->wnd_proc(hwnd, uMsg, wParam, lParam);
        g_decref(), p->decref();
        return result;
    }

    if (WM_NCCREATE == uMsg)
    {
        // bind window to the c++ structure
        SetWindowLongPtr(hwnd, 0,
            (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//===========================================================================
void Menu::register_droptarget(bool set)
{
    if (set) {
        if (getIsDropTarget()) {
            LPCITEMIDLIST pidl = getPidlList() ? first_pidl(getPidlList()) : NULL;
            data.m_droptarget = bbcore::init_drop_target(getWindowHandle(), pidl);
            if (pidl)
                data.m_notify = add_change_notify_entry(getWindowHandle(), pidl);
        }

    } else {
        if (data.m_notify)
            remove_change_notify_entry(data.m_notify), data.m_notify = 0;
        if (data.m_droptarget)
            exit_drop_target(data.m_droptarget), data.m_droptarget = NULL;
    }
}

//==============================================
// When a submenu becomes visible, the m_pParent(pItem)
// pointers are set, also the m_pChild pointer of the parent
// When it becomes invisible (pItem = NULL), they are reset.

void Menu::LinkToParentItem(MenuItem *pItem)
{
    if (getParentMenuItem())
    {
        getParentMenu()->setChildMenu(NULL);
        setParentMenu(NULL);
    }
    setParentMenuItem(pItem);
    if (getParentMenuItem())
    {
        setParentMenu(getParentMenuItem()->m_pMenu);
        getParentMenu()->setChildMenu(this);
    }
}

//==============================================
// Append item to the list

MenuItem *Menu::AddMenuItem(MenuItem* pItem)
{
    // link to the list
    getLastItem()->next = pItem;
    setLastItem(pItem);

    // set references
    pItem->m_pMenu = this;
    return pItem;
}

//==============================================
// clear all Items - except the TitleItem

void Menu::DeleteMenuItems()
{   
    MenuItem *pItem, *thisItem;

    setLastItem(getMenuItems());
    thisItem = getLastItem();
    pItem = thisItem->next;
    thisItem->next = NULL;
    thisItem->m_bActive = false;
    setLastActiveItem(NULL);
    while (pItem)
        pItem=(thisItem=pItem)->next, delete thisItem;
}

//==============================================
// find a menu with IDString, i.e. for onscreen updating

Menu *Menu::find_named_menu(const char *IDString, bool fuzzy)
{
    int l = strlen(IDString);
    bbcore::MenuList *ml;
    Menu *m;
    dolist (ml, g_MenuStructList)
        if ((m = ml->m)->data.m_IDString
            && 0 == memcmp(m->data.m_IDString, IDString, l)
            && (0 == m->data.m_IDString[l] || fuzzy))
            return ml->m;
    return NULL;
}

//===========================================================================
// some utilities

// When a menu get's the focus, it is inserted at the last position of the
// list, so that it is set to the top of the z-order with "Menu_All_BringOnTop()"
void Menu::insert_at_last (void)
{
    bbcore::MenuList **mp, *ml, *mn = NULL;
    for (mp = &g_MenuWindowList; NULL != (ml = *mp); ) {
        if (this == ml->m)
            *mp = (mn = ml)->next;
        else
            mp = &ml->next;
    }
    if (NULL == mn)
        mn = (bbcore::MenuList*)new_node(this);
    (*mp = mn)->next = NULL;
}

// get the list-index for item or -2, if not found
int Menu::get_item_index (MenuItem *item)
{
    int c = 0;
    MenuItem *pItem;
    if (item)
        dolist (pItem, getMenuItems()->next) {
            if (item == pItem)
                return c;
            c++;
        }
    return -1;
}

// get the item from idex
MenuItem * Menu::nth_item(int a)
{
    int c = 0;
    MenuItem *pItem = NULL;
    if (a >= 0)
        dolist (pItem, getMenuItems()->next) {
            if (c == a)
                break;
            c++;
        }
    return pItem;
}

// get the root in a menu chain
Menu *Menu::menu_root (void)
{
    Menu *p, *m = this;
    while (NULL != (p = m->getParentMenu()))
        m = p;
    return m;
}

bool Menu::has_focus_in_chain(void)
{
    Menu *p;
    for (p = menu_root(); p; p = p->getChildMenu())
        if (p->data.m_bHasFocus)
            return true;
    return false;
}

bool Menu::has_hwnd_in_chain(HWND hwnd)
{
    Menu *p;
    for (p = menu_root(); p; p = p->getChildMenu())
        if (p->getWindowHandle() == hwnd)
            return true;
    return false;
}

// Get the root of the last active menu
Menu *Menu::last_active_menu_root(void)
{
    bbcore::MenuList *ml;
    dolist (ml, g_MenuWindowList)
        if (NULL == ml->next)
            return ml->m->menu_root();
    return NULL;
}

void Menu::set_capture(int flg)
{
    setMouseCaptureFlag(flg);
    if (flg)
        SetCapture(getWindowHandle());
    else
        ReleaseCapture();
}

void Menu::set_focus(void)
{
    if (data.m_bHasFocus)
        return;
    SetFocus(getWindowHandle());
}

// --------------------------------------
// these are called from WM_SET/KILLFOCUS

void Menu::on_setfocus(HWND oldFocus)
{
    MenuItem *pItem;

    data.m_bHasFocus = true;
    insert_at_last();

    pItem = nth_item(data.m_kbditempos);
    if (pItem)
        pItem->Active(2);

    // if there is a submenu, possibly overlapping,
    // then place this behind it again
    if (getChildMenu())
        menu_set_pos(NULL, SWP_NOMOVE|SWP_NOSIZE);
}

void Menu::on_killfocus(HWND newfocus)
{
    if (newfocus 
        && (newfocus == getWindowHandle() 
         || newfocus == getChildEditWindowHandle()))
        return;
    data.m_bHasFocus = false;
    if (false == has_hwnd_in_chain(newfocus))
        menu_root()->Hide();
}

//===========================================================================
// Timer to do the submenu open and close delay.
void Menu::set_timer(bool set)
{
    int d;
    if (false == set) {
        KillTimer(getWindowHandle(), MENU_POPUP_TIMER);
        return;
    }
    d = bbcore::gSettingsMenu.popupDelay;
    if (false == data.m_bHasFocus && d < 100)
        d = 100;
    if (0 == d) {
        UpdateWindow(getWindowHandle()); // update hilite bar first
        PostMessage(getWindowHandle(), WM_TIMER, MENU_POPUP_TIMER, 0);
    } else {
        SetTimer(getWindowHandle(), MENU_POPUP_TIMER, d, NULL);
    }
}

//===========================================================================
// scroll helpers

// helper: get the scroll range in pixel (menuheight-title-scrollbutton)
int Menu::get_y_range(int *py0, int *ps)
{
    int y0, s, d, t;
    d = data.m_bNoTitle ? bbcore::gMenuInfo.nFrameMargin : bbcore::gMenuInfo.nScrollerSideOffset;
    t = data.m_bNoTitle ? 0 : bbcore::gMenuInfo.nScrollerTopOffset;
    *py0 = y0 = data.m_firstitem_top + t;
    *ps = s = bbcore::gMenuInfo.nScrollerSize;
    return getHeight() - y0 - s - d;
}

// get the scroll button square from scrollindex
void Menu::get_vscroller_rect(RECT* rw)
{
    int y0, s, d;
    d = data.m_bNoTitle ? bbcore::gMenuInfo.nFrameMargin : bbcore::gMenuInfo.nScrollerSideOffset;
    get_y_range(&y0, &s);

    rw->top = data.m_scrollpos + y0;
    rw->bottom = rw->top + s;
    if (bbcore::gMenuInfo.nBulletPosition == FOLDER_RIGHT)
        rw->right = (rw->left = d) + s;
    else
        rw->left = (rw->right = getWidth() - d) - s;
}

// helper:  get the topitem-index from scrollindex
int Menu::calc_topindex (bool adjust)
{
    int y0, s, k, d, n;
    k = get_y_range(&y0, &s);
    d = getTotalItemsCount() - data.m_pagesize;
    if (d == 0) {
        data.m_scrollpos = n = 0;
    } else {
        s = imax(d, data.m_pagesize);
        n = imin(d, (s * data.m_scrollpos + k/2) / k);
        data.m_scrollpos = imin(data.m_scrollpos, d * k / s);
        if (adjust && n != data.m_topindex)
            data.m_scrollpos = data.m_topindex * k / s;
    }
    return n;
}

// set the scroll button according to mouse position, scroll menu if needed
void Menu::set_vscroller(int ymouse)
{
    int y0, s, k, i;
    RECT scroller1, scroller2;

    k = get_y_range(&y0, &s);
    i = iminmax(ymouse - s/2 - y0, 0, k);

    get_vscroller_rect(&scroller1);
    data.m_scrollpos = i;
    scroll_assign_items(calc_topindex(false));
    get_vscroller_rect(&scroller2);
    if (scroller1.top != scroller2.top)
    {
        InvalidateRect(getWindowHandle(), &scroller1, FALSE);
        InvalidateRect(getWindowHandle(), &scroller2, FALSE);
    }
}

// set the scroll button according to changed topindex, itemcount, pagesize
void Menu::scroll_menu(int n)
{
    scroll_assign_items(n + data.m_topindex);
    calc_topindex(true);
}

// assign y coordinates to items according to _new topindex
void Menu::scroll_assign_items(int new_top)
{
    int c0, c1, c, y;
    MenuItem *pItem;

    new_top = imax(0, imin(new_top, getTotalItemsCount() - data.m_pagesize));
    if (data.m_hBitMap)
    {
        if (data.m_topindex == new_top)
            return;
        InvalidateRect(getWindowHandle(), NULL, FALSE);
    }

    c0  = data.m_topindex = new_top;
    c1  = c0 + data.m_pagesize;
    c   = 0;
    y   = data.m_firstitem_top;
    pItem = getMenuItems();
    while (NULL != (pItem=pItem->next)) // skip TitleItem
    {
        if (c<c0 || c>=c1)
            pItem->m_nTop = -1000;
        else
            pItem->m_nTop = y, y += pItem->m_nHeight;
        c++;
    }
}

//===========================================================================

void Menu::ignore_mouse_msgs(void)
{
    MSG msg;
    while (PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
    g_ignore_mouse_msgs = 2;
}

// Hide menu on item clicked
void Menu::hide_on_click(void)
{
    Menu *root = menu_root();
    ignore_mouse_msgs();
    if (false == root->getIsPinned())
        root->Hide();
    else
    if (getMenuId() & MENU_ID_SHCONTEXT)
        Hide();
    else
        HideChild();
}

// save menu fixed keyboard position
void Menu::write_menu_pos(void)
{
    if (this->data.m_kbdpos) {
        bbcore::gSettingsMenu.pos.x = this->data.m_xpos;
        bbcore::gSettingsMenu.pos.y = this->data.m_ypos;

        bbcore::gSettingBbox->writeRCSetting(&bbcore::gSettingsMenu.pos.x);
        bbcore::gSettingBbox->writeRCSetting(&bbcore::gSettingsMenu.pos.y);
    }
}

void Menu::menu_set_pos(HWND after, UINT flags)
{
    if (getIsOnTop() || false == getIsPinned()) {
        if (getChildMenu()) // keep it behind the child menu anyway
            after = getChildMenu()->getWindowHandle();
        else if (getIsOnTop() || bbcore::bbactive || NULL == getParentMenu())
            after = HWND_TOPMOST;
    }
    SetWindowPos(getWindowHandle(), after, data.m_xpos, data.m_ypos, getWidth(), getHeight(),
        flags|SWP_NOACTIVATE|SWP_NOSENDCHANGING);
}

void Menu::SetZPos(void)
{
    menu_set_pos(HWND_NOTOPMOST, SWP_NOMOVE|SWP_NOSIZE);
}


// set menu on top of the z-order
void Menu::bring_ontop(bool force_active)
{
    if (bbcore::bbactive) {
        set_focus();
    } else if (force_active) {
        ForceForegroundWindow(getWindowHandle());
    } else {
        menu_set_pos(HWND_TOP, SWP_NOMOVE|SWP_NOSIZE);
        insert_at_last();
    }
    HideChild();
    bbcore::Menu_All_Hide_But(this);
}

void Menu::Paint(void)
{
    PAINTSTRUCT ps;
    HDC hdc, hdc_screen, back;
    RECT r;
    HGDIOBJ S0, F0, B0;
    int y1,y2,c1,c2,c;
    int bw;
    MenuItem *pItem;
    StyleItem *pSI;

    hdc_screen = hdc = BeginPaint(getWindowHandle(), &ps);
    back = CreateCompatibleDC(hdc_screen);
    B0 = NULL;

    y1 = ps.rcPaint.top;
    y2 = ps.rcPaint.bottom;

    if (y1 == 0)
    {
        // If the entrire window must be drawn, we use a double buffer
        // to avoid flicker. Otherwise, for speed, we draw directly.
        hdc = CreateCompatibleDC(hdc_screen);
        B0 = SelectObject(hdc,
                CreateCompatibleBitmap(hdc_screen, getWidth(), getHeight()));
    }

    pItem = getMenuItems();

    if (NULL == data.m_hBitMap) // create the background
    {
        data.m_hBitMap = CreateCompatibleBitmap(hdc_screen, getWidth(), getHeight());
        S0 = SelectObject(back, data.m_hBitMap);
        pSI = &bbcore::mStyle.MenuFrame;

        bw = pSI->borderWidth;
        r.left = r.top = 0;
        r.right = getWidth();
        r.bottom = getHeight();

        if (bw)
            bbcore::createBorder(back, &r, pSI->borderColor, bw);

        r.left = r.top = bw;
        r.right -= bw;
        r.bottom -= bw;

        if (false == data.m_bNoTitle
         && false == bbcore::mStyle.MenuTitle.parentRelative
         && false == bbcore::mStyle.menuTitleLabel)
            r.top = bbcore::gMenuInfo.nTitleHeight;

        if (r.bottom > r.top)
            bbcore::makeStyleGradient(back, &r, pSI, false);

        if (false == data.m_bNoTitle)
        {
            // Menu Title
            F0 = SelectObject(back, bbcore::gMenuInfo.hTitleFont);
            SetBkMode(back, TRANSPARENT);
            pItem->Paint(back);
            SelectObject(back, F0);
        }
    }
    else
    {
        S0 = SelectObject(back, data.m_hBitMap);
    }

    // copy (invalid part of) the background
    BitBltRect(hdc, back, &ps.rcPaint);
    SelectObject(back, S0);
    DeleteDC(back);

    SetBkMode(hdc, TRANSPARENT);
    F0 = SelectObject(hdc, bbcore::gMenuInfo.hFrameFont);

    c = -1, c1 = data.m_topindex, c2 = c1 + data.m_pagesize;

    // skip items scrolled out on top
    while (c < c1 && pItem)
        pItem = pItem->next, ++c;

    while (c < c2 && pItem)
    {
        int y = pItem->m_nTop;
        if (y >= y2)
            break;
        if (y + pItem->m_nHeight > y1)
            pItem->Paint(hdc);
        pItem = pItem->next, ++c;
    }
    SelectObject(hdc, F0);

    if (data.m_pagesize < getTotalItemsCount())
    {
        // draw the scroll button
        get_vscroller_rect(&r);
        pSI = &bbcore::gMenuInfo.Scroller;
        bbcore::makeStyleGradient(hdc, &r, pSI, pSI->bordered);
    }

    if (hdc != hdc_screen) // if using double buffer
    {
        // put buffer on screen
        BitBltRect(hdc_screen, hdc, &ps.rcPaint);
        DeleteObject(SelectObject(hdc, B0));
        DeleteDC(hdc);
    }

#ifdef CHECKFOCUS
    {
        bool focus = GetRootWindow(GetFocus()) == m_hwnd;
        RECT r; GetClientRect(m_hwnd, &r);
        COLORREF c = 0;
        if (focus) c |= 0x0000FF;
        if (m_bHasFocus) c |= 0x00cc00;
        if (0 == c) c = 0xCC0000;
        bbcore::createBorder(hdc_screen, &r, c, 2);
    }
#endif

    EndPaint(getWindowHandle(), &ps);
}

//==============================================
// Calculate sizes of the menu-window

void Menu::Validate()
{
    int margin  = bbcore::gMenuInfo.nFrameMargin;
    int border  = bbcore::mStyle.MenuFrame.borderWidth;
    int tborder = bbcore::mStyle.MenuTitle.borderWidth;
    int tm      = bbcore::gMenuInfo.nTitleMargin;
    int w1, w2, c0, c1, h, hmax, x; SIZE size;

    MenuItem *pItem;
    HDC hDC;
    HGDIOBJ other_font;
    char opt_cmd[1000];

    w1 = w2 = c0 = c1 = h = 0;
    opt_cmd[0] = 0;

    data.m_bNoTitle = (getFlags() & BBMENU_NOTITLE) || bbcore::mStyle.menuNoTitle;

    // ---------------------------------------------------
    // measure text-sizes
    pItem = getMenuItems();
    hDC = CreateCompatibleDC(NULL);
    other_font = SelectObject(hDC, bbcore::gMenuInfo.hTitleFont);

    // title item
    if (false == data.m_bNoTitle) {
        pItem->Measure(hDC, &size);
        w1 = size.cx;
    }

    // now for frame items
    SelectObject(hDC, bbcore::gMenuInfo.hFrameFont);

    while (NULL != (pItem = pItem->next))
    {
        pItem->Measure(hDC, &size);
        pItem->m_nHeight = size.cy;
        if (size.cx > w2)
            w2 = size.cx;
        if (pItem->m_ItemID & MENUITEM_UPDCHECK) {
            // update checkmark (for stylemenu folder etc.)
            pItem->m_bChecked = bbcore::get_opt_command(opt_cmd, pItem->m_pszCommand);
        }
    }

    SelectObject(hDC, other_font);
    DeleteDC(hDC);

    // ---------------------------------------------------
    // make sure that the menu is not wider than something

    int width = imin(getMaxWidth(),
        imax(w1 + 2*(bbcore::gMenuInfo.nTitleIndent + bbcore::gMenuInfo.nTitleMargin + border),
             w2 + 2*margin + bbcore::gMenuInfo.nItemLeftIndent + bbcore::gMenuInfo.nItemRightIndent
             ));
    setWidth(width);

    // ---------------------------------------------------
    // assign item positions

    // the title item
    pItem = getMenuItems();
    pItem->m_nLeft = border + tm;
    pItem->m_nWidth = getWidth() - 2*(border + tm);
    pItem->m_nHeight = bbcore::gMenuInfo.nTitleHeight - tborder - border;
    if (bbcore::mStyle.menuTitleLabel)
        pItem->m_nHeight += tborder - bbcore::mStyle.MenuFrame.marginWidth;

    if (data.m_bNoTitle) {
        pItem->m_nTop = -1000;
        data.m_firstitem_top = margin;
    } else {
        pItem->m_nTop = tm + border;
        data.m_firstitem_top = bbcore::gMenuInfo.nTitleHeight + bbcore::mStyle.MenuFrame.marginWidth;
    }

    hmax = getMaxHeight() - data.m_firstitem_top;

    // assign xy-coords and width
    w2 = getWidth() - 2*margin;
    while (NULL != (pItem = pItem->next))
    {
        pItem->m_nLeft = margin;
        pItem->m_nWidth = w2;
        ++c0;
        if ((x = h + pItem->m_nHeight) < hmax)
            h = x, c1 = c0;
    }

    setTotalItemsCount(c0);
    data.m_pagesize = c1;
    setHeight(data.m_firstitem_top + h + margin);

    // adjust height for empty menus
    if (0 == c0)
    {
        if (data.m_bNoTitle) {
            setHeight(getHeight() + 4);
        } else if (bbcore::mStyle.menuTitleLabel || bbcore::mStyle.MenuTitle.parentRelative) {
            setHeight(bbcore::gMenuInfo.nTitleHeight + margin);
        } else {
            setHeight(bbcore::gMenuInfo.nTitleHeight - tborder + border);
        }
    }

    // need a _new background
    if (data.m_hBitMap)
    {
        DeleteObject(data.m_hBitMap), data.m_hBitMap = NULL;
        InvalidateRect(getWindowHandle(), NULL, FALSE);
    }

    // assign y-coords
    scroll_menu(0);
}

//==============================================

void Menu::Show(int xpos, int ypos, bool fShow)
{
    Menu *p = getParentMenu();

    if (p) // a submenu
    {
        int overlap = bbcore::gMenuInfo.nSubmenuOverlap;
        int xleft = p->data.m_xpos - this->getWidth() + overlap;
        int xright = p->data.m_xpos + p->getWidth() - overlap;
        bool at_left = bbcore::gMenuInfo.openLeft;

        // pop to the left or to the right ?
        if (xright + getWidth() > getMenuRectOnMonitor().right)
            at_left = true;
        else
        if (xleft < getMenuRectOnMonitor().left)
            at_left = false;
        else
        if (p->getParentMenu())
            at_left = p->getParentMenu()->data.m_xpos > p->data.m_xpos;

        xpos = at_left ? xleft : xright;
        ypos = p->data.m_ypos;

        if (getParentMenuItem() != getParentMenu()->getMenuItems()) // not the TitleItem
            ypos += getParentMenuItem()->m_nTop - data.m_firstitem_top;
    }

    // make sure the popup is on the screen
    data.m_xpos = iminmax(xpos, getMenuRectOnMonitor().left,
        getMenuRectOnMonitor().right - getWidth());
    data.m_ypos = iminmax(ypos, getMenuRectOnMonitor().top,
        getMenuRectOnMonitor().bottom - (getIsPinned() ? bbcore::gMenuInfo.nTitleHeight : getHeight()));

    // create the menu window, if it doesn't have one already
    make_menu_window();

    if (data.m_alpha != bbcore::mStyle.menuAlpha)
    {
        bbcore::setTransparency(getWindowHandle(), data.m_alpha = bbcore::mStyle.menuAlpha);
    }

    if (fShow)
        menu_set_pos(HWND_TOP, SWP_SHOWWINDOW);
    else
        menu_set_pos(NULL, SWP_NOZORDER);
}

//==============================================
// On Timer: Show/Hide submenu

void Menu::MenuTimer(UINT nTimer)
{
    if (MENU_POPUP_TIMER == nTimer)
    {
        set_timer(false);
        if (NULL == getLastActiveItem()) {
            HideChild();
            return;
        }
        if (NULL == getParentMenu() && NULL == getChildMenu())
            bring_ontop(false);
    }

    if (getLastActiveItem())
        getLastActiveItem()->ItemTimer(nTimer);
}

void Menu::UnHilite()
{
    if (getLastActiveItem())
        getLastActiveItem()->Active(0);
}

//==============================================
// Hide menus  016f:004109d4

void Menu::Hide(void)
{
    UnHilite();
    HideChild();
    if (getIsPinned())
        return;

    if (getParentMenu())
        getParentMenu()->UnHilite();
    if (data.m_bHasFocus) {
        if (getParentMenu())
            getParentMenu()->set_focus();
        else if (data.m_hwndRef)
            SetFocus(data.m_hwndRef);
    }
    destroy_menu_window(false);
}

void Menu::HideNow(void)
{
    SetPinned(false); // force
    Hide();
}

void Menu::HideChild(void)
{
    if (getChildMenu())
        getChildMenu()->Hide(); // close submenu
}

//==============================================
// detach menu from it's parent

void Menu::SetPinned(bool bPinned)
{
    Menu *p{};
    if (getFlags() & BBMENU_NOTITLE)
        bPinned = false;
    setIsPinned(bPinned);
    if (false == bPinned)
        return;
    p = menu_root();
    if (p != this) {
        LinkToParentItem(NULL);
        p->Hide();
    }
    post_autohide();
}

//==============================================
// Virtual: Implemented by SpecialFolder to update contents on folder change

void Menu::UpdateFolder(void)
{
    MenuItem *mi;
    dolist (mi, getMenuItems())
        if (mi->m_ItemID == MENUITEM_ID_INSSF)
            ((bbcore::SFInsert*)mi)->RemoveStuff();
}

//===========================================================================
void Menu::mouse_over(bool indrag)
{
    if (data.m_bMouseOver)
        return;

    // hide grandchilds
    if (getChildMenu()) {
        getChildMenu()->HideChild();
        getChildMenu()->UnHilite();
    }

    //hilite my parentItem
    if (getParentMenu()) {
        getParentMenu()->set_timer(false);
        getParentMenuItem()->Active(2);
    }

    //set focus to this one immediately
    if (has_focus_in_chain())
        set_focus();

    data.m_bMouseOver = true;

    if (false == indrag && NULL == getChildEditWindowHandle())
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize  = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = getWindowHandle();
        tme.dwHoverTime = HOVER_DEFAULT;
        ::TrackMouseEvent(&tme);
    }
    else
    {
        // TrackMouseEvent does not seem to work in drag
        // operation (and on win95)
        SetTimer(getWindowHandle(),
            MENU_TRACKMOUSE_TIMER,
            iminmax(bbcore::gSettingsMenu.popupDelay-10, 20, 100), NULL);
    }
}

//==============================================

void Menu::mouse_leave(void)
{
    if (NULL == getChildMenu() || false == getChildMenu()->data.m_bMouseOver)
        UnHilite();
    if (getChildMenu())
        set_timer(true); // to close submenu

}

//==============================================

void Menu::Handle_Mouse(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    MenuItem* item;

    pt.x = (short)LOWORD(lParam);
    pt.y = (short)HIWORD(lParam);

    data.m_kbditempos = -1;

    if (WM_MOUSEWHEEL == uMsg)
    {
        int d = (short)HIWORD(wParam) > 0 ? -1 : 1;
        scroll_menu(d * bbcore::gSettingsMenu.mouseWheelFactor);
        ScreenToClient(hwnd, &pt);
    }

    if (getMouseCaptureFlag())
    {
        // ------------------------------------------------
        if (getMouseCaptureFlag() == MENU_CAPT_SCROLL) // the scroll button
        {
            if (WM_MOUSEMOVE == uMsg)
                set_vscroller(pt.y);
        }
        else
        if (getMouseCaptureFlag() == MENU_CAPT_ITEM) // mouse down on item
        {
            if (getLastActiveItem())
            {
                getLastActiveItem()->Mouse(hwnd, uMsg, wParam, lParam);
            }
        }

        if (WM_MOUSEMOVE == uMsg)
            return;

        // ------------------------------------------------
    }
    else // not captured
    {
        // Set Focus, track mouse, etc...
        mouse_over(false);

        // ------------------------------------------------
        // check scrollbutton
        if (data.m_pagesize < getTotalItemsCount())
        {
            RECT scroller;
            get_vscroller_rect(&scroller);

            if (pt.x >= scroller.left && pt.x < scroller.right
                && (data.m_bNoTitle || pt.y >= data.m_firstitem_top + bbcore::gMenuInfo.nScrollerTopOffset))
            {
                if (WM_LBUTTONDOWN == uMsg)
                {
                    set_vscroller(pt.y);
                    set_capture(MENU_CAPT_SCROLL);
                }
                else
                if (getChildMenu() && getChildMenu()->getParentMenuItem()->isover(pt.y))
                {
                    // ignore scroller, if an folder-item is at the same position
                    bool submenu_right = getChildMenu()->data.m_xpos > data.m_xpos;
                    bool scroller_right = FOLDER_RIGHT != bbcore::gMenuInfo.nBulletPosition;
                    if (scroller_right == submenu_right)
                        return;
                }
                mouse_leave();
                return;
            }
        }
    }

    // ------------------------------------------------
    // check through items, where the mouse is over

    item = getMenuItems(); // the TitleItem

    // everything above the first item belongs to title
    if (pt.y < data.m_firstitem_top
        // also, allow moving of 'title-less' menus with control held down
      || (data.m_bNoTitle && (wParam & MK_CONTROL))) {

        item->Mouse(hwnd, uMsg, wParam, lParam);
        return;
    }

    // check normal items
    while (NULL != (item = item->next)) {
        if (item->isover(pt.y)) {
            item->Mouse(hwnd, uMsg, wParam, lParam);
            return;
        }
    }
}

//==============================================
// simulate mouse movement in drag operation

#ifndef BBTINY
LPCITEMIDLIST Menu::dragover(POINT* ppt)
{
    MenuItem* pItem;
    POINT pt = *ppt;

    ScreenToClient(getWindowHandle(), &pt);
    mouse_over(true);

    dolist(pItem, getMenuItems())
        if (pItem->isover(pt.y)) {
            pItem->Active(1);
            return pItem->GetPidl();
        }
    return NULL;
}

void Menu::start_drag(const char *arg_path, LPCITEMIDLIST arg_pidl)
{
    LPITEMIDLIST pidl;
    if (data.m_bInDrag)
        return;
    if (arg_pidl)
        pidl = duplicateIDList(arg_pidl);
    else
    if (arg_path)
        pidl = sh_getpidl(NULL, arg_path);
    else
        return;
    if (NULL == pidl)
        return;

    incref();
    data.m_bInDrag = true;
    bbcore::drag_pidl(pidl);
    data.m_bInDrag = false;
    decref();
    freeIDList(pidl);
}
#endif

//===========================================================================
// Menu Keystroke handling
//===========================================================================

void Menu::kbd_hilite(MenuItem *pItem)
{
    int a;

    if (NULL == pItem) {
        UnHilite();
        return;
    }

    HideChild();
    pItem->Active(2);
    data.m_kbditempos = get_item_index(pItem);

    a = data.m_kbditempos - data.m_topindex;
    if (a < 0) {
        scroll_menu(a);
    }
    else
    {
        a -= data.m_pagesize - 1;
        if (a > 0)
            scroll_menu(a);
    }

    // immediately show submenu with int & string items
    if (pItem->m_pSubmenu
        && (pItem->m_pSubmenu->getMenuId() & (MENU_ID_INT|MENU_ID_STRING)))
        pItem->ShowSubmenu();
} 

//===========================================================================
MenuItem * Menu::kbd_get_next_shortcut(const char *d)
{
    MenuItem *pItem;
    unsigned n, dl;

    dl = strlen(d);
    for (n = 0, pItem = getLastActiveItem();; pItem = getMenuItems())
    {
        if (pItem)
            while (NULL != (pItem = pItem->next)) {
                const char *s, *t;
                for (t = s = pItem->m_pszTitle;; ++s)
                    if (0 == *s) {
                        s = t;
                        break;
                    } else if (*s == '&' && *++s != '&') {
                        break;
                    }
                if (strlen(s) < dl)
                    continue;
                if (2 == CompareString(
                        LOCALE_USER_DEFAULT,
                        NORM_IGNORECASE|NORM_IGNOREWIDTH|NORM_IGNOREKANATYPE,
                        d, dl, s, dl))
                    break;
            }
        if (pItem || 2 == ++n)
            return pItem;
    }
}

//--------------------------------------
bool Menu::Handle_Key(UINT msg, UINT wParam)
{
    int n, a, c, d, e, i, x, y;
    MenuItem *pItem; bool ctrl; const int c_steps = 1;

    ignore_mouse_msgs();

    if (VK_CONTROL == wParam || VK_SHIFT == wParam)
        return false;

    //--------------------------------------
    if (MENU_ID_INT & getMenuId())
    {
        getMenuItems()->next->Key(msg, wParam);
        return true;
    }

    //--------------------------------------
    if (WM_CHAR == msg)
    {
        char chars[16];
        MSG msg;

        i = 0;
        chars[i++] = (char)wParam;
        while (PeekMessage(&msg, getWindowHandle(), WM_CHAR, WM_CHAR, PM_REMOVE)) {
            chars[i++] = (char)msg.wParam;
            if (i >= (int)sizeof chars - 1)
                break;
        }
        chars[i] = 0;

        if (!IS_SPC(chars[0])) {
            // Shortcuts
            pItem = kbd_get_next_shortcut(chars);
            if (pItem) {
                kbd_hilite(pItem);
                if (pItem == kbd_get_next_shortcut(chars)) {
                    // (there is only one choice for that accelerator...)

                    if (pItem->m_ItemID & MENUITEM_ID_FOLDER) {
                        // popup folder
                        goto k_right;
                    }

                    if (getFlags() & BBMENU_SYSMENU) {
                        // for the window sysmenus (bbLeanSkin/Bar)
                        goto k_invoke;
                    }
                }
            }
        }
        return true;
    }

    //--------------------------------------
    if (WM_KEYDOWN == msg)
    {
        a = data.m_kbditempos;
        if (a == -1)
            a = get_item_index(getLastActiveItem());

        e = getTotalItemsCount() - 1;
        d = data.m_pagesize - 1;
        n = 0;
        ctrl = 0 != (0x8000 & GetKeyState(VK_CONTROL));
one_more:
        i = 1;
        switch(wParam)
        {
            set:
                pItem = nth_item(a);
                if (pItem && pItem->m_bNOP) {
                    if (n++ < e)
                        goto one_more;
                    pItem = NULL;
                }
                kbd_hilite(pItem);
                break;

            //--------------------------------------
            case VK_DOWN:
                if (ctrl)
                {
                    i = imin(c_steps, e - d - data.m_topindex);
                    if (i >= 0)
                        scroll_menu(i);
                    else
                        break;
                }
                if (a < 0)
                    a = data.m_topindex;
                else
                if (a >= e)
                    a = (d < e) ? e : 0;
                else
                    a = imin(a+i, e);
                goto set;

            //--------------------------------------
            case VK_UP:
                if (ctrl)
                {
                    i = imin(c_steps, data.m_topindex);
                    if (i >= 0)
                        scroll_menu(-i);
                    else
                        break;
                }
                if (a < 0)
                    a = e;
                else
                if (0 == a)
                    a = (d < e) ? 0 : e;
                else
                    a = imax(a-i, 0);
                goto set;

            //--------------------------------------
            case VK_NEXT:
                if (a < 0)
                    a = data.m_topindex;
                c = imin (e - d - data.m_topindex, d);
                if (c <= 0)
                    a = e;
                else
                    a += c;
                scroll_menu(c);
                wParam = VK_DOWN;
                goto set;

            //--------------------------------------
            case VK_PRIOR:
                if (a < 0)
                    a = data.m_topindex;
                c = imin(data.m_topindex, d);
                if (c <= 0)
                    a = 0;
                else
                    a -= c;
                scroll_menu(-c);
                wParam = VK_UP;
                goto set;

            //--------------------------------------
            case VK_HOME:
                a = 0;
                goto set;

            //--------------------------------------
            case VK_END:
                a = e;
                goto set;

            //--------------------------------------
            case VK_RIGHT:
            k_right:
                if (getLastActiveItem()) {
                    getLastActiveItem()->ShowSubmenu();
                }

            hilite_child:
                if (getChildMenu()) {
                    getChildMenu()->set_focus();
                    getChildMenu()->kbd_hilite(getChildMenu()->getMenuItems()->next);
                }
                break;

            //--------------------------------------
            case VK_LEFT:
                if (getParentMenu())
                    getParentMenu()->kbd_hilite(getParentMenuItem());
                break;

            //--------------------------------------
            case VK_RETURN: // invoke command
            case VK_SPACE:
            k_invoke:
                if (getLastActiveItem()) {
                    getLastActiveItem()->Invoke(INVOKE_LEFT);
                    goto hilite_child;
                }
                break;

            //--------------------------------------
            case VK_APPS: // context menu
                if (getLastActiveItem())
                {
                    getLastActiveItem()->Invoke(INVOKE_RIGHT);
                    goto hilite_child;
                }
                break;

            //--------------------------------------
            case VK_F5: // update folder
                PostMessage(getWindowHandle(), BB_FOLDERCHANGED, 0, 0);
                break;

            //--------------------------------------
            case VK_TAB: // cycle through menues
                bbcore::Menu_Tab_Next(this);
                break;

            //--------------------------------------
            case VK_INSERT:
                if (false == getIsPinned())
                {
                    SetPinned(true);
                }
                else
                {
                    setIsOnTop(false == getIsOnTop()); // inversion ?
                }

                SetZPos();
                break;

            //--------------------------------------
            case VK_ESCAPE:
                if (0 == (menu_root()->getFlags() & BBMENU_HWND))
                {
                    bbcore::Menu_All_Hide();
                    bbcore::focus_top_window();
                }
                else
                {
                    menu_root()->Hide();
                }
                break;
            case VK_DELETE:
                HideNow();
                break;

            //--------------------------------------
            //sortmode: extension, name, size, time
#ifndef BBTINY
            case 'E': case 'N': case 'S': case 'T':
                if (ctrl)
                {
                    static const char sm[] = ".ENS.T";
                    i = strchr(sm, wParam) - sm;
                    if (i != data.m_sortmode)
                        data.m_sortrev = 5==(data.m_sortmode = i);
                    else
                        data.m_sortrev ^= 1;
                    HideChild();
                    Redraw(2);
                }
                break;
#endif
            //--------------------------------------
            default:
                return false;
        }
        return true;
    }

    //--------------------------------------
    if (WM_SYSKEYDOWN == msg)
    {
        d = 12, x = data.m_xpos, y = data.m_ypos;
        switch(wParam)
        {
            case VK_LEFT:  x -= d; break;
            case VK_RIGHT: x += d; break;
            case VK_UP:    y -= d; break;
            case VK_DOWN:  y += d; break;

            case VK_RETURN:
            case VK_SPACE:
                if (getLastActiveItem())
                {
                    getLastActiveItem()->Invoke(INVOKE_PROP);
                }
                return true;

            default:
                return false;
        }
        Show(x, y, false);
        write_menu_pos();
        return true;
    }

    return false;
}

//===========================================================================
// Window proc for the popup menues

LRESULT Menu::wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        //====================
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);

        //====================
        case WM_CREATE:
            if (bbcore::gSettingsMenu.sticky)
            {
                bbcore::makeSticky(hwnd);
            }
            break;

        case WM_DESTROY:
            bbcore::removeSticky(hwnd);
            break;

        //====================
        case WM_SETFOCUS:
#ifdef CHECKFOCUS
            debug_printf("WM_SETFOCUS %s %x <- %x", getMenuItems()->m_pszTitle, hwnd, wParam);
            InvalidateRect(hwnd, NULL, FALSE);
#endif
            on_setfocus((HWND)wParam);
            break;

        //====================
        case WM_KILLFOCUS:
#ifdef CHECKFOCUS
            debug_printf("WM_KILLFOCUS %s %x -> %x", getMenuItems()->m_pszTitle, hwnd, wParam);
            InvalidateRect(hwnd, NULL, FALSE);
#endif
            on_killfocus((HWND)wParam);
            break;

        //====================
        case WM_CLOSE:
            HideNow();
            break;

        case BB_FOLDERCHANGED:
            if (NULL == getChildMenu() || getChildMenu()->getMenuId() != MENU_ID_SHCONTEXT)
                RedrawGUI(BBRG_FOLDER);
            break;

        case BB_DRAGOVER:
            return (LRESULT)dragover((POINT *)lParam);

        //====================
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
            if (false == Handle_Key(uMsg, wParam))
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            break;

        //====================
        case WM_MOUSELEAVE:
            //debug_printf("mouseleave %x", hwnd);
            if (data.m_bMouseOver)
            {
                data.m_bMouseOver = false;
                mouse_leave();
            }
            break;

        //====================
        case WM_MOUSEWHEEL:
            Handle_Mouse(hwnd, uMsg, wParam, lParam);
            break;

        //====================
        case WM_MOUSEMOVE:
            if (g_ignore_mouse_msgs)
                -- g_ignore_mouse_msgs;
            else
                Handle_Mouse(hwnd, uMsg, wParam, lParam);
            break;

        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
            setIsDoubleClicked(true);
            Handle_Mouse(hwnd, uMsg, wParam, lParam);
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            setIsDoubleClicked(false);
            Handle_Mouse(hwnd, uMsg, wParam, lParam);
            break;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            set_capture(0);
            Handle_Mouse(hwnd, uMsg, wParam, lParam);
            break;

        case WM_CAPTURECHANGED:
            setMouseCaptureFlag(0);
            break;

        //====================
        case WM_TIMER:
            if (MENU_TRACKMOUSE_TIMER == wParam)
            {
                if (hwnd != window_under_mouse())
                {
                    PostMessage(hwnd, WM_MOUSELEAVE, 0, 0);
                    KillTimer(hwnd, wParam);
                }
                break;
            }
            //debug_printf("Timer %x %d", hwnd, wParam);
            MenuTimer(wParam);
            break;

        //====================
        case WM_WINDOWPOSCHANGING:
            if (data.m_bMoving && bbcore::gSettingsMenu.snapWindow)
            {
                bbcore::snapWindowToEdge((WINDOWPOS*)lParam, 0, SNAP_NOPLUGINS);
            }
            break;

        case WM_WINDOWPOSCHANGED:
            if (0 == (((LPWINDOWPOS)lParam)->flags & SWP_NOMOVE))
            {
                data.m_xpos = ((LPWINDOWPOS)lParam)->x;
                data.m_ypos = ((LPWINDOWPOS)lParam)->y;
            }
            break;

        //====================
        case WM_ENTERSIZEMOVE:
            data.m_bMoving = true;
            break;

        //====================
        case WM_EXITSIZEMOVE:
            {
                data.m_bMoving = false;
                RECT rect{};
                bbcore::getMonitorRect(hwnd, &rect, GETMON_FROM_WINDOW);
                setMenuRectOnMonitor(rect);
                write_menu_pos();
            }
            break;

        //====================
        case WM_PAINT:
            Paint();
            break;

        //====================
        case WM_ERASEBKGND:
            return TRUE;

        //===============================================
        // String pItem - parent window notifications
        case WM_COMMAND:
            if (1234 == LOWORD(wParam))
                if (EN_UPDATE == HIWORD(wParam)
                ||  EN_HSCROLL == HIWORD(wParam))
                {
                    InvalidateRect((HWND)lParam, NULL, FALSE);
                }
            break;

        case WM_CTLCOLOREDIT:
            SetTextColor((HDC)wParam, bbcore::mStyle.MenuFrame.TextColor);
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (LRESULT)GetStockObject(NULL_BRUSH);

        //===============================================
    }
    return 0;
}


void Menu::Sort(MenuItem **ppItems, int(*cmp_fn)(MenuItem **, MenuItem**))
{
    MenuItem **a, **b, *i; int n = 0;
    dolist(i, *ppItems) n++;  // get size
    if (n < 2) return; // nothing to do
    b = a = (MenuItem**)m_alloc(n * sizeof *a); // make array
    dolist(i, *ppItems) *b = i, b++; // store pointers
    qsort(a, n, sizeof *a, (int(*)(const void*,const void*))cmp_fn);
    do a[--n]->next = i, i = a[n]; while (n); // make list
    m_free(a); // free array
    *ppItems = i;
} 

//===========================================================================
void Menu::RedrawGUI(int flags)
{
    if (flags & BBRG_FOLDER) {
        Redraw(2);
    } else {
        // sent from bbstylemaker
        if ((flags & BBRG_MENU) && (flags & BBRG_PRESSED) && NULL == getLastActiveItem()) {
            MenuItem *pItem = getMenuItems();
            while (NULL != (pItem = pItem->next))
                if (false == pItem->m_bNOP) {
                    pItem -> Active(2); // hilite some item
                    break;
                }
        }
        Redraw(0);
    }
}

//===========================================================================
// For menu updating. Since all items are deleted and rebuilt, this
// tries to make that happen without the user would notice.

void Menu::SaveState()
{
    if (NULL == getWindowHandle() || data.m_saved)
        return;
    // store position of hilited item
    data.m_OldPos = get_item_index(getLastActiveItem());
    // store childmenu
    data.m_OldChild = getChildMenu();
    data.m_saved = true;
    // debug_printf("savee %s", getMenuItems()->m_pszTitle);
}

void Menu::RestoreState(void)
{
    MenuItem *pItem;
    // debug_printf("restored %s", getMenuItems()->m_pszTitle);
    if (data.m_OldChild) {
        // The menu had an open submenu when it was updated, so
        // this if possible tries to link it to an FolderItem, again.
        dolist (pItem, getMenuItems()) {
            Menu *pSub = pItem->m_pSubmenu;
            if (NULL == pSub) {
                if (pItem->m_ItemID == MENUITEM_ID_SF)
                    pSub = Menu::find_special_folder(pItem->m_pidl_list);
                else
                    pSub = pItem->m_pRightmenu;
            }
            if (pSub == data.m_OldChild) {
                pSub->incref(), pItem->LinkSubmenu(pSub);
                pItem->m_pSubmenu->LinkToParentItem(pItem);
                break;
            }
        }
        if (NULL == pItem) {
            // Lost child - the parent menu or item doesn't exist anymore.
            data.m_OldChild->Hide();
            data.m_OldPos = -1;
        }
        data.m_OldChild = NULL;
    }

    // lookup the item that was previously hilited
    pItem = nth_item(data.m_OldPos);
    if (pItem)
        pItem->Active(1);
    data.m_saved = false;
}

//===========================================================================

//===========================================================================
// update and relink all visible menus within an updated menu structure

void Menu::Redraw(int mode)
{
    MenuItem *pItem;
    if (getWindowHandle())
    {
        if (mode != 0) {
            SaveState();
            if (mode == 2)
                UpdateFolder();
        }
        // recalculate sizes
        Validate();
        if (mode != 0) {
            RestoreState();
        }
        // update window position/size
        Show(data.m_xpos, data.m_ypos, false);
        if (mode == 2)
            return;
    }

    // Redraw submenus recursively
    dolist (pItem, getMenuItems())
        if (pItem->m_pSubmenu)
            pItem->m_pSubmenu->Redraw(mode);
        else
        if (pItem->m_pRightmenu)
            pItem->m_pRightmenu->Redraw(mode);
}

bool Menu::del_menu(Menu *m, void *ud)
{
    m->destroy_menu_window(true);
    return true;
} 

// Toggle visibility state
bool Menu::toggle_menu(Menu *m, void *ud)
{
    ShowWindow(m->getWindowHandle(), *(bool*)ud ? SW_HIDE : SW_SHOWNA);
    return true;
}


// update menus after style changed
bool Menu::redraw_menu(Menu *m, void *ud)
{
    if (NULL == m->getParentMenu())
        m->RedrawGUI(*(int*)ud);
    return true;
} 

bool Menu::hide_menu(Menu *m, void *ud)
{
    if (NULL == m->getParentMenu() && m != (Menu*)ud)
        m->Hide();
    return true;
} 

int Menu::get_menu_bullet(const char* tmp)
{
    static const char * const bullet_strings[] = {
        "triangle" ,
        "square"   ,
        "diamond"  ,
        "circle"   ,
        "empty"    ,
        NULL
        };

    static const char bullet_styles[] = {
        BS_TRIANGLE  ,
        BS_SQUARE    ,
        BS_DIAMOND   ,
        BS_CIRCLE    ,
        BS_EMPTY     ,
        };

    int i = get_string_index(tmp, bullet_strings);
    if (-1 != i)
        return bullet_styles[i];
    return BS_TRIANGLE;
}


//===========================================================================

void Menu::ShowMenu()
{
    POINT pt;
    int w, h, flags, pos;

    flags = getFlags();
    rflags() &= BBMENU_HWND|BBMENU_SYSMENU|BBMENU_NOTITLE;
    pos = flags & BBMENU_POSMASK;

    GetCursorPos(&pt);

    // check display position options
    switch (pos) {
    case BBMENU_XY:
        pt.x = data.m_pos.left;
        pt.y = data.m_pos.top;
        break;
    case BBMENU_RECT:
        if (BBMENU_KBD & flags)
            pt.x = data.m_pos.left;
        pt.y = data.m_pos.bottom;
        break;
    case BBMENU_CENTER:
        break;
    case BBMENU_CORNER:
    default:
        if (BBMENU_KBD & flags) {
            pt.x = bbcore::gSettingsMenu.pos.x;
            pt.y = bbcore::gSettingsMenu.pos.y;
        }
        break;
    }

    // get monitor rectangle and set max-menu-height
    RECT rect{};
    bbcore::getMonitorRect(&pt, &rect, GETMON_FROM_POINT);
    setMenuRectOnMonitor(rect);
    setMaxHeight((getMenuRectOnMonitor().bottom - getMenuRectOnMonitor().top) * 80 / 100);

    if (pos == BBMENU_RECT) {
        int h1 = data.m_pos.top - getMenuRectOnMonitor().top;
        int h2 = getMenuRectOnMonitor().bottom - data.m_pos.bottom;
        setMaxHeight(imax(h1, h2));
    }

    Redraw(1);

    if (getWindowHandle()) {
        // when its a submenu currently:
        LinkToParentItem(NULL);
        UnHilite();
        if (false == (BBMENU_KBD & flags))
            data.m_kbditempos = -1;
    } else {
        // calculate frame sizes
        Validate();
    }

    // now adjust menu position according to menu-width/height
    w = getWidth();
    h = getHeight();

    switch (pos) {
    case BBMENU_XY:
        // explicit xy-position was given, handle right/bottom alignment
        if (flags & BBMENU_XRIGHT)
            pt.x -= w;
        if (flags & BBMENU_YBOTTOM)
            pt.y -= h;
        break;
    case BBMENU_RECT:
        // auto show below or above a rectangle (taskbar etc.)
        pt.x = imax(imin(pt.x + w/2, data.m_pos.right) - w, data.m_pos.left);
        if (pt.x + w >= getMenuRectOnMonitor().right)
            pt.x = data.m_pos.right - w;
        if (pt.y + h >= getMenuRectOnMonitor().bottom)
            pt.y = data.m_pos.top - h;
        break;
    case BBMENU_CENTER:
        // center menu on screen and set on top
        pt.x = (getMenuRectOnMonitor().left + getMenuRectOnMonitor().right - w) / 2;
        pt.y = (getMenuRectOnMonitor().top + getMenuRectOnMonitor().bottom - h) / 2;
        break;
    case BBMENU_CORNER:
        if (pt.x + w >= getMenuRectOnMonitor().right)
            pt.x -= w;
        if (pt.y + h >= getMenuRectOnMonitor().bottom)
            pt.y -= h;
        break;
    default:
        if (BBMENU_KBD & flags)
            break;
        // show centered at cursor;
        pt.x -= w/2;
        pt.y -= bbcore::gMenuInfo.nTitleHeight/2 + bbcore::gMenuInfo.nTitleMargin;
        break;
    }

    if (BBMENU_KBD & flags)
        data.m_kbdpos = true; // flag to write new position on user move

    if (BBMENU_PINNED & flags)
        SetPinned(true);
    else if (0 == (BBMENU_KBD & flags))
        SetPinned(false);

    if (BBMENU_ONTOP & flags)
        setIsOnTop(true);

    if (NULL == data.m_hwndRef && NULL == getWindowHandle())
        data.m_hwndRef = GetFocus();

    Show(pt.x, pt.y, true);

    if (0 == (flags & BBMENU_NOFOCUS))
        bring_ontop(true);

    decref();
    ignore_mouse_msgs();
}

/// \brief for plugin menus, false when updating
bool Menu::isPopup(void) const
{
    return data.m_bPopup;
} 

HWND Menu::getWindowHandle(void) const
{
    return data.hwnd;
}

void Menu::setWindowHandle(HWND hwnd)
{
    data.hwnd = hwnd;
}


/// \brief Id is one of MENU_ID_XXX values
void Menu::setMenuId(int id)
{
    data.m_MenuID = id;
}

int Menu::getMenuId(void) const
{
    return data.m_MenuID;
}

void Menu::setLastActiveItem(MenuItem* item)
{
    data.m_pActiveItem = item;
}

MenuItem* Menu::getLastActiveItem(void) const
{
    return data.m_pActiveItem;
}

int Menu::getTotalItemsCount(void) const
{
    return data.m_itemcount;
}

void Menu::setTotalItemsCount(int count)
{
    data.m_itemcount = count;
}

void Menu::incTotalItemsCount(void)
{
    ++data.m_itemcount;
}

void Menu::decTotalItemsCount(void)
{
    --data.m_itemcount;
}

/// \brief Last tail pointer while adding items
void Menu::setLastItem(MenuItem* item)
{
    data.m_pLastItem = item;
}

MenuItem* Menu::getLastItem(void) const
{
    return data.m_pLastItem;
}

pidl_node* Menu::getPidlList(void)
{
    return data.m_pidl_list;
}

void Menu::setPidlList(pidl_node* pidlList)
{
    data.m_pidl_list = pidlList;
}

void Menu::setIsDropTarget(bool value)
{
    data.m_bIsDropTarg = value;
}

bool Menu::getIsDropTarget(void) const
{
    return data.m_bIsDropTarg;
}

/// \brief Get edit control of StringItems
HWND Menu::getChildEditWindowHandle(void)
{ 
    return data.m_hwndChild;    // edit control of StringItems
}

void Menu::setChildEditWindowHandle(HWND hwnd)
{
    data.m_hwndChild = hwnd;
}

int Menu::getWidth(void) const
{
    return data.m_width;
}

void Menu::setWidth(int value)
{
    data.m_width = value;
}

int Menu::getHeight(void) const
{
    return data.m_height;
}

void Menu::setHeight(int value)
{
    data.m_height = value;
}

Menu* Menu::getParentMenu(void)
{
    return data.m_pParent;
}

void Menu::setParentMenu(Menu* menu)
{
    data.m_pParent = menu;
}

/// \brief Whether on top of z-order
bool Menu::getIsOnTop(void) const
{
    return data.m_bOnTop;
}

/// \brief Set whether on top of z-order
void Menu::setIsOnTop(bool value)
{
    data.m_bOnTop = value;
}

const RECT& Menu::getMenuRectOnMonitor(void) const
{
    return data.m_mon;
}

void Menu::setMenuRectOnMonitor(const RECT& rect)
{
    data.m_mon = rect;
} 

int Menu::getMaxHeight(void) const
{
    return data.m_maxheight;
}

void Menu::setMaxHeight(int value)
{
    data.m_maxheight = value;
}

int Menu::getMaxWidth(void) const
{
    return data.m_maxwidth;
}

void Menu::setMaxWidth(int value)
{
    data.m_maxwidth = value;
}

int Menu::getFlags(void) const
{
    return data.m_flags_;
}

int& Menu::rflags(void)
{
    return data.m_flags_;
}

void Menu::setFlags(int newFlags)
{
    data.m_flags_ = newFlags;
} 

MenuItem* Menu::getParentMenuItem(void)
{
    return data.m_pParentItem;
}

void Menu::setParentMenuItem(MenuItem* item)
{
    data.m_pParentItem = item;
}

bool Menu::getIsDoubleClicked(void) const
{
    return data.m_dblClicked;
}

void Menu::setIsDoubleClicked(bool value)
{
    data.m_dblClicked = value;
}

int Menu::getMouseCaptureFlag(void) const
{
    return data.m_captureflg;
}

void Menu::setMouseCaptureFlag(int flag)
{
    data.m_captureflg = flag;
}

Menu* Menu::getChildMenu(void)
{
    return data.m_pChild;
}

void Menu::setChildMenu(Menu* menu)
{
    data.m_pChild = menu;
}

bool Menu::getIsPinned(void) const
{
    return data.m_bPinned;
}

void Menu::setIsPinned(bool value)
{
    data.m_bPinned = value;
}

MenuItem* Menu::getMenuItems(void)
{
    return data.m_pMenuItems;
}

void Menu::setMenuItems(MenuItem* item)
{
    data.m_pMenuItems = item;
} 

// This looks up whether a folder is already on screen 
Menu *Menu::find_special_folder(pidl_node* p1)
{
    bbcore::MenuList *ml;
    pidl_node* p2;
    if (p1) dolist (ml, Menu::g_MenuWindowList) {
        Menu *m = ml->m;
        if (m->getMenuId() == MENU_ID_SF) {
            p2 = m->getPidlList();
            if (p2 && equal_pidl_list(p1, p2))
                return m;
        }
    }
    return NULL;
}

//file extension priority sort
int Menu::folder_compare(MenuItem** pm1, MenuItem** pm2)
{
    int f, x, y, z;
    MenuItem *m1, *m2;
    const char *a1, *b1, *a2, *b2;
    int sortmode = g_sortmode;
    HRESULT hr;

    if (g_sortrev)
        m2 = *pm1, m1 = *pm2;
    else
        m1 = *pm1, m2 = *pm2;

#ifndef BBTINY
    if (sortmode >= 2 && g_psf)
    {
        // use shell column-sort function
        LPCITEMIDLIST p1, p2, n1, n2;
        p1 = m1->GetPidl();
        p2 = m2->GetPidl();
        if (p1 && p2) {
            // get relative pidl
            while (cbID(n1=NextID(p1))) p1 = n1;
            while (cbID(n2=NextID(p2))) p2 = n2;
            hr = COMCALL3(g_psf, CompareIDs, sortmode-2, p1, p2);
            return (int)(short)HRESULT_CODE(hr);
        }}
#endif

    if (0 != (z = m2->m_nSortPriority - (f = m1->m_nSortPriority)))
        return z;

    a1 = m1->m_pszTitle;
    b1 = m2->m_pszTitle;
    if (1 != sortmode || f == M_SORT_NAME || f == M_SORT_FOLDER)
        return stricmp(a1, b1); // sort by name

    a2 = strrchr(a1,'.');
    b2 = strrchr(b1,'.');
    if (a2 == NULL)
        return (b2 == NULL) ? stricmp(a1, b1) : -1;
    else
    if (b2 == NULL)
        return 1;

    if (0 != (z = stricmp(a2,b2))) // sort by extension
        return z;

    x = a2-a1;
    y = b2-b1;
    if (0 != (z = memicmp(a1,b1,x<y?x:y)))
        return z;
    if (0 != (z = x-y))
        return z;
    return (int)((BYTE*)m2 - (BYTE*)m1);
}

int Menu::AddFolderContents(const pidl_node* pidl_list, const char *extra)
{
    const pidl_node* p = pidl_list;
    MenuItem *pItems = NULL;
    int flag = 0;
    int options = 0;

    if (extra && 0 == strcmp(extra, MM_THEME_BROAM))
    {
        MenuItem *pItem = bbcore::makeMenuItem(this, "default", "@BBCore.theme default", false);
        bbcore::menuItemOption(pItem, BBMENUITEM_UPDCHECK);
        bbcore::makeMenuNOP(this, NULL);
        options |= bbcore::LF_norecurse;
    }

    if (p != nullptr)
    {
        for (;;)
        {
            flag |= bbcore::LoadFolder(&pItems, first_pidl(p), extra, options);
            p = p->next;
            if (NULL == p)
                break;
            options |= bbcore::LF_join;
        }
    }

    if (NULL == pItems)
        return flag;

    g_sortmode = data.m_sortmode ? data.m_sortmode : bbcore::gSettingsMenu.sortByExtension;
    g_sortrev = data.m_sortrev;
    g_psf = NULL;

#ifndef BBTINY
    if (g_sortmode >= 2 && !(options & bbcore::LF_join))
    {
        g_psf = sh_get_folder_interface(first_pidl(pidl_list));
    }
#endif

    Sort(&pItems, folder_compare);

    if (g_psf)
        COMCALL0(g_psf, Release), g_psf = NULL;

    if (pItems) for (;;) {
        MenuItem *next = pItems->next;
        AddMenuItem(pItems);
        if (!next)
            break;
#if 0
        if (pItems->m_nSortPriority != next->m_nSortPriority)
            bbcore::makeMenuNOP(this, NULL);
#endif
        pItems = next;
    }

    return flag;
}

int Menu::menuCount(void)
{
    return g_menu_count;
}

