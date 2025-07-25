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

// generic Menu Items

#include <BB.h>
#include <Settings.h>

#include "Menu.h"
#include "MenuItem.h"
#include "Utils.h"
#include "BImage.h"

#include "parse.h"
#include "drawico.h"

MenuItem::MenuItem(const char* pszTitle)
{
    memset(&next, 0, sizeof *this - sizeof(void*));

    m_Justify   = MENUITEM_STANDARD_JUSTIFY;
    //m_ItemID    = MENUITEM_ID_NORMAL;
    //m_nSortPriority = M_SORT_NORMAL;
    m_pszTitle  = new_str(pszTitle);

    ++bbcore::g_menu_item_count;
}

MenuItem::~MenuItem()
{
    UnlinkSubmenu();
    if (m_pRightmenu)
        m_pRightmenu->decref();
    free_str(&m_pszTitle);
    free_str(&m_pszCommand);
    free_str(&m_pszRightCommand);
    delete_pidl_list(&m_pidl_list);

#ifdef BBOPT_MENUICONS
    free_str(&m_pszIcon);
    if (m_hIcon)
        DestroyIcon(m_hIcon);
#endif

    --bbcore::g_menu_item_count;
}

void MenuItem::LinkSubmenu(Menu *pSubMenu)
{
    UnlinkSubmenu();
    m_pSubmenu = pSubMenu;
}

void MenuItem::UnlinkSubmenu(void)
{
    if (m_pSubmenu)
    {
        Menu *sub = m_pSubmenu;
        m_pSubmenu = NULL;

        if (this == sub->getParentMenuItem())
            // i.e. the submenu is currently visible
            sub->LinkToParentItem(NULL);

        sub->decref();
    }
}

void MenuItem::ShowRightMenu(Menu *pSub)
{
    if (pSub)
    {
        m_pMenu->HideChild();
        LinkSubmenu(pSub);
        ShowSubmenu();
    }
}

#ifndef BBTINY
void MenuItem::ShowContextMenu(const char *path, LPCITEMIDLIST pidl)
{
    ShowRightMenu(bbcore::makeContextMenu(path, pidl));
}
#endif

LPCITEMIDLIST MenuItem::GetPidl(void)
{
    return m_pidl_list ? first_pidl(m_pidl_list) : NULL;
}

//===========================================================================
// Execute the command that is associated with the menu item
void MenuItem::Invoke(int button)
{
    // default implementation is a nop
}

//====================
void MenuItem::Key(UINT nMsg, WPARAM wParam)
{
    // default implementation is a nop
}

//====================
void MenuItem::ItemTimer(UINT nTimer)
{
    if (MENU_POPUP_TIMER == nTimer)
    {
        ShowSubmenu();
    }
}

//====================
// Mouse commands:

void MenuItem::Mouse(HWND hwnd, UINT uMsg, DWORD wParam, DWORD lParam)
{
    int i = 0;
    switch(uMsg)
    {
        case WM_MBUTTONUP:
            i = INVOKE_MID;
            break;

        case WM_RBUTTONUP:
            i = INVOKE_RIGHT;
            break;

        case WM_LBUTTONUP:
            i = INVOKE_LEFT;
            if (m_pMenu->getIsDoubleClicked())
                i |= INVOKE_DBL;

            if (0x8000 & GetAsyncKeyState(VK_MENU))
            {
                if (i & INVOKE_DBL)
                    i = INVOKE_PROP;
                else
                    i = 0;
            }
            break;

        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            m_pMenu->set_capture(MENU_CAPT_ITEM);
            Active(1);
            break;

        case WM_MOUSEMOVE:
            if (m_pMenu->getMouseCaptureFlag() && hwnd != window_under_mouse())
            {
                // start drag operation on mouseleave
                m_pMenu->set_capture(0);
                i = INVOKE_DRAG;
                break;
            }
            Active(1);
            break;

        case WM_MOUSEWHEEL:
            m_pMenu->HideChild();
            Active(2);
            break;
    }

    if (i && m_bActive && false == m_bNOP)
        Invoke(i);
}

//====================

void MenuItem::Active(int active)
{
    bool new_active;
    RECT r;

    new_active = active > 0;
    if (m_bActive == new_active)
        return;
    m_bActive = new_active;

    GetItemRect(&r);
    InvalidateRect(m_pMenu->getWindowHandle(), &r, false);

    if (false == new_active) {
        m_pMenu->setLastActiveItem(NULL);
        return;
    }

    m_pMenu->UnHilite();
    m_pMenu->setLastActiveItem(this);

    if (active == 1)
        m_pMenu->set_timer(true);

    if (m_pMenu->getMenuId() == MENU_ID_STRING && bbcore::bbactive)
    {
        if (m_ItemID == MENUITEM_ID_STR)
            SetFocus(m_pMenu->getChildEditWindowHandle());
        else
            SetFocus(m_pMenu->getWindowHandle());
    }
}

//====================

void MenuItem::ShowSubmenu()
{
    if (m_pMenu->getChildMenu())
    {
        if (this == m_pMenu->getChildMenu()->getParentMenuItem())
            return;
        m_pMenu->HideChild();
    }

    if (NULL == m_pSubmenu
     || m_pSubmenu->getIsPinned()
     || m_bDisabled)
        return;

    m_pSubmenu->Hide();

    // inherit some values from parent menu
    m_pSubmenu->setIsOnTop(m_pMenu->getIsOnTop());
    m_pSubmenu->rflags() |= m_pMenu->getFlags() & BBMENU_NOTITLE;
    m_pSubmenu->setMaxHeight(m_pMenu->getMaxHeight());
    m_pSubmenu->setMenuRectOnMonitor(m_pMenu->getMenuRectOnMonitor());

    m_pSubmenu->Validate();
    m_pSubmenu->LinkToParentItem(this);
    m_pSubmenu->Show(0, 0, true);
    PostMessage(bbcore::gBBhwnd, BB_SUBMENU, 0, 0); //for bbsoundfx
}

//===========================================================================

//===========================================================================

void MenuItem::GetItemRect(RECT* r)
{
    r->top    = m_nTop;
    r->bottom = m_nTop + m_nHeight;
    r->left   = m_nLeft;
    r->right  = m_nLeft + m_nWidth;
}

void MenuItem::GetTextRect(RECT* r)
{
    r->top    = m_nTop;
    r->bottom = m_nTop  + m_nHeight;
    r->left   = m_nLeft + bbcore::gMenuInfo.nItemLeftIndent;
    r->right  = m_nLeft + m_nWidth - bbcore::gMenuInfo.nItemRightIndent;
}

const char* MenuItem::GetDisplayString(void)
{
    if (bbcore::gSettingsMenu.showBroams)
    {
        if ((MENUITEM_ID_CMD & m_ItemID)
            && m_pszCommand
            && m_pszCommand[0] == '@')
            return m_pszCommand;
#if 0
        if (m_pMenu->m_pMenuItems == this && m_pMenu->m_IDString)
            return m_pMenu->m_IDString;
#endif
    }
    return m_pszTitle;
}

//===========================================================================

void MenuItem::Measure(HDC hDC, SIZE *size)
{
    const char *title = GetDisplayString();
    RECT r = { 0, 0, 0, 0 };
    bbcore::bbDrawTextImpl(hDC, title, &r, DT_MENU_MEASURE_STANDARD, 0);
    size->cx = r.right;
    size->cy = bbcore::gMenuInfo.nItemHeight;
#ifdef BBOPT_MENUICONS
    if (m_hIcon)
        size->cy = imax(bbcore::gMenuInfo.nIconSize+2, size->cy);
#endif
}

//===========================================================================

void MenuItem::Paint(HDC hDC)
{
    RECT rc, rhi;
    StyleItem *pSI;
    COLORREF TC, BC;
    int j;

    GetTextRect(&rc);
    pSI = &bbcore::mStyle.MenuFrame;

    if (m_bActive && false == m_bNOP) {
        // draw hilite bar
        GetItemRect(&rhi);
        pSI = &bbcore::mStyle.MenuHilite;
        bbcore::makeStyleGradient(hDC, &rhi, pSI, pSI->bordered);
        TC = pSI->TextColor;
        BC = pSI->foregroundColor;
    } else if (m_bDisabled) {
        BC = TC = pSI->disabledColor;
    } else {
        TC = pSI->TextColor;
        BC = pSI->foregroundColor;
    }

    j = m_Justify;
    if (MENUITEM_CUSTOMTEXT != j) {
        if (MENUITEM_STANDARD_JUSTIFY == j)
            j = bbcore::mStyle.MenuFrame.Justify;
        // draw menu item text
        bbcore::bbDrawTextImpl(hDC, GetDisplayString(), &rc, j | DT_MENU_STANDARD, TC);
    }

#ifdef BBOPT_MENUICONS
    this->DrawIcon(hDC);
#endif

    if (m_bChecked) // draw check-mark
    {
        int d, atright;

#ifdef BBOPT_MENUICONS
        if (m_ItemID & MENUITEM_ID_FOLDER)
            atright = bbcore::gMenuInfo.nBulletPosition == FOLDER_LEFT;
        else
            atright = true;
#else
        if (bbcore::gMenuInfo.nItemLeftIndent != bbcore::gMenuInfo.nItemRightIndent)
            atright = bbcore::gMenuInfo.nBulletPosition != FOLDER_LEFT;
        else
        if (m_ItemID & MENUITEM_ID_FOLDER)
            atright = bbcore::gMenuInfo.nBulletPosition == FOLDER_LEFT;
        else
            atright = j != DT_LEFT;
#endif
        rc.bottom = (rc.top = m_nTop) + m_nHeight + 1;
        if (atright) {
            d = bbcore::gMenuInfo.nItemRightIndent + bbcore::mStyle.MenuHilite.borderWidth;
            rc.left = (rc.right = m_nLeft + m_nWidth) - d + 1;
        } else {
            d = bbcore::gMenuInfo.nItemLeftIndent + bbcore::mStyle.MenuHilite.borderWidth;
            rc.right = (rc.left = m_nLeft) + d;
        }

#if 1
        bbcore::bbDrawPixImpl(hDC, &rc, BC, BS_CHECK);
#else
        {
            bool pr, lit;
            const int w = 6;
            pSI = &bbcore::mStyle.MenuHilite;
            pr = pSI->parentRelative;
            lit = m_bActive && false == m_bNOP;
            if (lit != pr)
                pSI = &bbcore::mStyle.MenuFrame;
            rc.left   = (rc.left + rc.right - w)/2;
            rc.right  = rc.left + w;
            rc.top    = (rc.top + rc.bottom - w)/2;
            rc.bottom = rc.top + w;
            if (pr)
                MakeGradient(hDC, rc, B_SOLID, pSI->TextColor, 0, false, BEVEL_FLAT, 0, 0, 0, 0);
            else
                bbcore::makeStyleGradient(hDC, &rc, pSI, false);
        }
#endif
    }
} 

//===========================================================================

void MenuItem::DrawIcon(HDC hDC)
{
    int size, px, py, d;
/*
    if (m_ItemID & MENUITEM_ID_FOLDER)
        return;
*/
    if (NULL == m_hIcon)
    { 
        // if no icon yet, try to load it, either from explicit string
        // or from pidl_list 
        if (m_pszIcon)
        {
            char path[MAX_PATH];
            const char *p;
            int index;

            p = bbcore::tokenize(m_pszIcon, path, ",");
            index = 0;
            if (p)
            {
                index = atoi(p);
                if (index)
                {
                    --index;
                }
            }
            unquote(path);
            ExtractIconEx(path, index, NULL, &m_hIcon, 1);

        }
        else if (m_pidl_list)
        {
            m_hIcon = sh_geticon(first_pidl(m_pidl_list), 16);

        }

        if (NULL == m_hIcon)
        {
            return;
        }
    }

    size = bbcore::gMenuInfo.nIconSize;
    d = (m_nHeight - size) / 2;
    px = m_nLeft + d;
    py = m_nTop + d;
/*
    DrawIconEx(hDC,
        px, py, m_hIcon,
        size, size, 0,
        NULL, DI_NORMAL
        );
*/
    DrawIconSatnHue(hDC,
        px, py, m_hIcon,
        size, size, 0,
        NULL, DI_NORMAL,
        false == m_bActive, 40, 0
        );
}

// used with 'bbcore::makeMenuItemInt/String'-made items
MenuItem* MenuItem::get_real_item(void)
{
    if (this->m_pSubmenu
     && (this->m_pSubmenu->getMenuId() & (MENU_ID_STRING|MENU_ID_INT)))
        return this->m_pSubmenu->getMenuItems()->next;
    return NULL;
}


//===========================================================================
