#ifndef _BBCORE_MENU_ITEM_H
#define _BBCORE_MENU_ITEM_H

#include <windows.h>
#include <Shlobj.h>

class MenuItem;
class Menu; // don't include "Menu.h" to avoid circular dependency
struct pidl_node;

class MenuItem
{
public:
    MenuItem *next;

    MenuItem(const char* pszTitle);
    virtual ~MenuItem(void);

    virtual void Measure(HDC hDC, SIZE *size);
    virtual void Paint(HDC hDC);
    virtual void Invoke(int button);
    virtual void Mouse(HWND hw, UINT nMsg, DWORD, DWORD);
    virtual void Key(UINT nMsg, WPARAM wParam);
    virtual void ItemTimer(UINT nTimer);
    virtual void ShowSubmenu(void);

    void UnlinkSubmenu(void);
    void LinkSubmenu(Menu *pSubMenu);
    void Active(int bActive);
    void ShowRightMenu(Menu *pSub);
    void ShowContextMenu(const char *path, LPCITEMIDLIST pidl);

    void GetItemRect(RECT* r);
    void GetTextRect(RECT* r);
    const char* GetDisplayString(void);
    LPCITEMIDLIST GetPidl(void);

    MenuItem *get_real_item(void);

    // mouse over check
    inline bool isover(int y) { return y >= m_nTop && y < m_nTop + m_nHeight; }

    void DrawIcon(HDC hDC); // currently sketchy

    // ----------------------
    char *m_pszTitle;
    char *m_pszCommand;
    char *m_pszRightCommand;
    pidl_node* m_pidl_list;

    Menu* m_pMenu;          // the menu where the item is on
    Menu* m_pSubmenu;       // for folder items, also context menus
    Menu* m_pRightmenu;     // optional rightclick menu

    int m_nTop;             // metrics
    int m_nLeft;
    int m_nWidth;
    int m_nHeight;

    int m_Justify;          // alignment
    int m_ItemID;           // see below
    int m_nSortPriority;    // see below

    bool m_bActive;         // hilite
    bool m_bNOP;            // just text
    bool m_bDisabled;       // draw with disabledColor
    bool m_bChecked;        // draw check mark

    // when using BBOPT_MENUICONS:
    HICON m_hIcon;
    char* m_pszIcon;
};

//---------------------------------
// values/bitflags for m_ItemID

#define MENUITEM_ID_NORMAL  0
#define MENUITEM_ID_CMD     4
#define MENUITEM_ID_INT     (MENUITEM_ID_CMD|1)
#define MENUITEM_ID_STR     (MENUITEM_ID_CMD|2)
#define MENUITEM_ID_FOLDER  8
#define MENUITEM_ID_SF      (MENUITEM_ID_FOLDER|1)
#define MENUITEM_ID_INSSF   16
#define MENUITEM_UPDCHECK   128 // refresh checkmark everytime

// values for m_nSortPriority
#define M_SORT_NORMAL 0
#define M_SORT_NAME   1 // ignore extensions when sorting
#define M_SORT_FOLDER 2 // insert above other items

// button values for void Invoke(int button);
#define INVOKE_DBL      1
#define INVOKE_LEFT     2
#define INVOKE_RIGHT    4
#define INVOKE_MID      8
#define INVOKE_DRAG    16
#define INVOKE_PROP    32
#define INVOKE_RET     64

// FolderItem bullet positions
#define FOLDER_RIGHT    0
#define FOLDER_LEFT     1

// DrawText flags
#define DT_MENU_STANDARD (DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOCLIP | DT_EXPANDTABS)
#define DT_MENU_MEASURE_STANDARD (DT_LEFT | DT_EXPANDTABS | DT_CALCRECT | DT_EXPANDTABS)

#define MENUITEM_STANDARD_JUSTIFY -1
#define MENUITEM_CUSTOMTEXT -2








#endif //!_BBCORE_MENU_ITEM_H
