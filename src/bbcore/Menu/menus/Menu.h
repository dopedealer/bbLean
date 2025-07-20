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

// This file is the common header for the files that make up
// the menu implementation.

#ifndef _MENU_H_
#define _MENU_H_

#include <bbshell.h>

#include "MenuMaker.h"
#include "MenuInfo.h"

// forward declaration of two public exported classes
class Menu;
class MenuItem;

namespace bbcore { 

class ShellContext;

MenuItem* makeMenuItem(Menu *PluginMenu, const char* Title, const char* Cmd, bool ShowIndicator);
MenuItem* makeMenuNOP(Menu *PluginMenu, const char* Title); 
MenuItem* makeSubmenu(Menu* ParentMenu, Menu* ChildMenu, const char* Title = nullptr); 
MenuItem* makeMenuItemPath(Menu* ParentMenu, const char* Title, const char* path, const char* Cmd);
Menu* makeContextMenu(const char* path, const void* pidl);
Menu* makeMenu(const char* headerText);
void delMenu(Menu* pluginMenu);
MenuItem* makeMenuItemString(Menu* PluginMenu, const char* Title, const char* Cmd, const char* init_string);
MenuItem* makeMenuItemInt(Menu* PluginMenu, const char* Title, const char* Cmd, int val, int minval, int maxval);

bool menuExists(const char* IDString_start);
void showMenu(Menu* pluginMenu);
void menuItemOption(MenuItem *pItem, int option, ...); 
void menuItemOptionV(MenuItem *pItem, int option, va_list vl);
void menuOptionV(Menu* pMenu, int flags, va_list vl);
void menuOption(Menu *pMenu, int flags, ...);

class CDropTarget;

struct MenuList { MenuList* next; class Menu *m; }; 
typedef bool (*MENUENUMPROC)(Menu *m, void *ud); 

// global vars 
extern int g_menu_item_count; 

} // !namespace bbcore

//=======================================
class Menu
{
public:
    virtual ~Menu(void);
    virtual void UpdateFolder(void);

    int decref(void);
    int incref(void);

    void Redraw(int mode);
    bool isPopup(void) const; 

protected:
    Menu(const char *pszTitle);

public:
    MenuItem *AddMenuItem(MenuItem* m);
    void DeleteMenuItems(void);

    void LinkToParentItem(MenuItem *pItem);

    void Hide(void);
    void HideNow(void);
    void HideChild(void);
    void UnHilite(void);

    void SetPinned(bool bPinned);
    void SetZPos(void);
    void Validate(void);
    void Show(int xpos, int ypos, bool fShow);
    void Paint(void);

    void MenuTimer(UINT);
    bool Handle_Key(UINT msg, UINT wParam);
    void Handle_Mouse(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void register_droptarget(bool set);

    // window callback
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // popup/close delay
    void set_timer(bool set);
    void set_capture(int flg);

    // init / exit
    void make_menu_window(void);
    void destroy_menu_window(bool force);
    void post_autohide();

    // scrolling
    int get_y_range(int *py0, int *ps);
    void get_vscroller_rect(RECT* rw);
    int calc_topindex (bool adjust);
    void set_vscroller(int ymouse);
    void scroll_assign_items(int n);
    void scroll_menu(int n);

    // keyboard
    MenuItem * kbd_get_next_shortcut(const char *d);
    void kbd_hilite(MenuItem *pItem);

    // retrieve specific things
    Menu *menu_root (void);
    int get_item_index(MenuItem *item);
    bool has_focus_in_chain(void);
    bool has_hwnd_in_chain(HWND hwnd);
    MenuItem * nth_item(int a);

    // mouse
    void mouse_over(bool indrag);
    void mouse_leave(void);
    LPCITEMIDLIST dragover(POINT* ppt);
    void start_drag(const char *path, LPCITEMIDLIST pidl);

    // other
    void write_menu_pos(void);
    void insert_at_last (void);
    void set_focus(void);
    void bring_ontop(bool force_active);
    void menu_set_pos(HWND after, UINT flags);
    void on_killfocus(HWND newfocus);
    void on_setfocus(HWND oldFocus);
    void hide_on_click(void);

    void RedrawGUI(int flags);
    void RestoreState(void);
    void SaveState(void);
    void ShowMenu(void);

    // candidates to refactor:
    static const char* className(void);
    static int get_menu_bullet(const char* tmp);
    static int menuCount(void);

    // for inserting SpecialFolderItems
    int AddFolderContents(const pidl_node* pidl_list, const char* extra);

    // getters/setters
    HWND getWindowHandle(void) const;
    void setWindowHandle(HWND);

    void setMenuId(int);
    int getMenuId(void) const;

    void setLastActiveItem(MenuItem*);
    MenuItem* getLastActiveItem(void) const;

    void setLastItem(MenuItem*);
    MenuItem* getLastItem(void) const;

    MenuItem* getMenuItems(void);
    void setMenuItems(MenuItem*);

    int getTotalItemsCount(void) const;
    void setTotalItemsCount(int);
    void incTotalItemsCount(void);
    void decTotalItemsCount(void);

    pidl_node* getPidlList(void);
    void setPidlList(pidl_node*);

    void setIsDropTarget(bool);
    bool getIsDropTarget(void) const; 

    HWND getChildEditWindowHandle(void);
    void setChildEditWindowHandle(HWND); 

    int getWidth(void) const;
    void setWidth(int value);

    int getHeight(void) const;
    void setHeight(int value);

    Menu* getParentMenu(void);
    void setParentMenu(Menu* menu);

    bool getIsOnTop(void) const;
    void setIsOnTop(bool);

    const RECT& getMenuRectOnMonitor(void) const;
    void setMenuRectOnMonitor(const RECT&);

    int getMaxHeight(void) const;
    void setMaxHeight(int);

    int getMaxWidth(void) const;
    void setMaxWidth(int value);

    int getFlags(void) const;
    int& rflags(void);
    void setFlags(int);

    MenuItem* getParentMenuItem(void);
    void setParentMenuItem(MenuItem* item);

    bool getIsDoubleClicked(void) const;
    void setIsDoubleClicked(bool); 

    int getMouseCaptureFlag(void) const;
    void setMouseCaptureFlag(int);

    Menu* getChildMenu(void);
    void setChildMenu(Menu* menu);

    bool getIsPinned(void) const;
    void setIsPinned(bool);

    // -------------------------------------
    // overall menu functions
    static void ignore_mouse_msgs(void);
    static Menu *last_active_menu_root(void);
    static Menu *find_named_menu(const char * IDString, bool fuzzy = false);
    static void Sort(MenuItem **, int(*cmp_fn)(MenuItem **, MenuItem**));

    static bool del_menu(Menu *m, void *ud);
    static bool toggle_menu(Menu *m, void *ud);
    static bool redraw_menu(Menu *m, void *ud);
    static bool hide_menu(Menu *m, void *ud);

    static Menu *find_special_folder(pidl_node* p1);
    //file extension priority sort
    static int folder_compare(MenuItem** pm1, MenuItem** pm2);
    

    // -------------------------------------
    // friend functions

    // PluginMenu API friends
    friend Menu* bbcore::makeNamedMenu(const char* HeaderText, const char* IDString, bool popup);
    friend MenuItem* bbcore::makeSubmenu(Menu* ParentMenu, Menu* ChildMenu, const char* Title);
    friend void bbcore::menuOptionV(Menu* pMenu, int flags, va_list vl);

    friend MenuItem* bbcore::helper_menu(Menu *PluginMenu, const char* Title, int menuID, MenuItem *pItem);

    //friend Menu* bbcore::MenuEnum(bbcore::MENUENUMPROC fn, void *ud);
    friend void bbcore::Menu_All_BringOnTop(void);
    //friend void bbcore::Menu_All_Hide_But(Menu*);
    friend bool bbcore::Menu_Exists(bool pinned);
    friend void bbcore::Menu_Tab_Next(Menu* start);
    friend bool bbcore::Menu_ToggleCheck(const char* menu_id);

    // ----------------------
    // global variables
    static void g_incref(void);
    static void g_decref(void); 

    // ----------------------
    // global variables 
    static bbcore::MenuList* g_MenuWindowList;  // all menus with a window
    static bbcore::MenuList* g_MenuStructList;  // all menus
    static bbcore::MenuList* g_MenuRefList;  // menus for deletion

private:

private:
    struct {
        int         m_refc;         // menu reference count (must come first)
        Menu*       m_pParent;      // parent menu, if onscreen
        Menu*       m_pChild;       // child menu, if onscreen

        MenuItem*   m_pMenuItems;   // items, first is title (always present)
        MenuItem*   m_pParentItem;  // parentmenu's folderitem linked to this
        MenuItem*   m_pActiveItem;  // currently hilited item
        MenuItem*   m_pLastItem;    // tail pointer while adding items

        int         m_MenuID;       // see below
        char*       m_IDString;     // unique ID for plugin menus

        int         m_itemcount;    // total items
        int         m_topindex;     // top item index
        int         m_pagesize;     // visible items
        int         m_firstitem_top;// in pixel
        int         m_kbditempos;   // item hilited by the keyboard

        int         m_scrollpos;    // scroll button location (pixel from trackstart)
        int         m_captureflg;   // when the mouse is captured, see below

        Menu*       m_OldChild;     // in update: remember child

        int         m_OldPos;       // in update: remember active item
        bool        m_saved;
        bool        m_bOnTop;       // z-order
        bool        m_bPinned;      // pinned
        bool        m_bNoTitle;     // dont draw title

        bool        m_kbdpos;  // save position to blackbox.rc on changes
        bool        m_bIsDropTarg;  // window should be registered as DT
        bool        m_bMouseOver;   // current states:
        bool        m_bHasFocus;
        bool        m_dblClicked;
        bool        m_bMoving;      // moved by the user
        bool        m_bInDrag;      // in drag&drop operation
        bool        m_bPopup;       // for plugin menus, false when updating

        unsigned char m_alpha;      // transparency

        int         m_sortmode;     // folder sort mode
        int         m_sortrev;      // folder sort order

        int         m_xpos;         // window position and sizes
        int         m_ypos;

        int         m_width;
        int         m_height;

        HBITMAP     m_hBitMap;      // background bitmap, only while onscreen
        HWND        hwnd;           // window handle, only while onscreen
        HWND        m_hwndChild;    // edit control of StringItems
        HWND        m_hwndRef;      // hwnd to send notifications to */

        int         m_maxwidth;
        int         m_maxheight;

        RECT        m_mon;          // monitor rect where the menu is on
        RECT        m_pos;          // initial display position setting
        int         m_flags_;        // initial display (and other) flags

        pidl_node* m_pidl_list;
        bbcore::CDropTarget* m_droptarget;
        UINT m_notify;
    } data{}; 
};

//---------------------------------
#define MENU_POPUP_TIMER        1
#define MENU_TRACKMOUSE_TIMER   2
#define MENU_INTITEM_TIMER      3

//---------------------------------
// values for m_MenuID

#define MENU_ID_NORMAL      0
#define MENU_ID_SF          1   // SpecialFolder
#define MENU_ID_SHCONTEXT   2   // ContextMenu
#define MENU_ID_INT         4   // IntegerItem's ParentMenu
#define MENU_ID_STRING      8   // StringItem's ParentMenu
#define MENU_ID_RMENU      16   // Custom Rightclick Menu

//---------------------------------
// flags for set_capture(int flg)

#define MENU_CAPT_SCROLL 1
#define MENU_CAPT_ITEM 2



#endif /*ndef _MENU_H_ */
