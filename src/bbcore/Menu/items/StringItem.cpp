
#include "StringItem.h"
#include "BB.h"
#include "Menu.h"
#include "BImage.h"
#include "Settings.h"

namespace bbcore { 

StringItem::StringItem(const char* pszCommand, const char *init_string)
    : CommandItem(pszCommand, NULL, false)
{
    if (init_string)
        replace_str(&m_pszTitle, init_string);
    hText = NULL;
    m_ItemID = MENUITEM_ID_STR;
}

StringItem::~StringItem()
{
    if (hText)
        DestroyWindow(hText);
}

void StringItem::Paint(HDC hDC)
{
    RECT r;
    HFONT hFont;
    int x, y, w, h, padd;
    if (gSettingsMenu.showBroams)
    {
        if (hText)
            DestroyWindow(hText), hText = NULL;
        m_Justify = MENUITEM_STANDARD_JUSTIFY;
        MenuItem::Paint(hDC);
        return;
    }

    m_Justify = MENUITEM_CUSTOMTEXT;
    MenuItem::Paint(hDC);

    GetTextRect(&r);
    if (EqualRect(&m_textrect, &r))
        return;

    m_textrect = r;

    if (NULL == hText)
    {
        hText = CreateWindow(
            TEXT("EDIT"),
            m_pszTitle,
            WS_CHILD
            | WS_VISIBLE
            | ES_AUTOHSCROLL
            | ES_MULTILINE,
            0, 0, 0, 0,
            m_pMenu->getWindowHandle(),
            (HMENU)1234,
            hMainInstance,
            NULL
            );

        SetWindowLongPtr(hText, GWLP_USERDATA, (LONG_PTR)this);
        wpEditProc = (WNDPROC)SetWindowLongPtr(hText, GWLP_WNDPROC, (LONG_PTR)EditProc);
#if 0
        int n = GetWindowTextLength(hText);
        SendMessage(hText, EM_SETSEL, 0, n);
        SendMessage(hText, EM_SCROLLCARET, 0, 0);
#endif
        m_pMenu->setChildEditWindowHandle(hText);
        if (GetFocus() == m_pMenu->getWindowHandle())
            SetFocus(hText);
    }

    hFont = gMenuInfo.hFrameFont;
    SendMessage(hText, WM_SETFONT, (WPARAM)hFont, 0);

    x = r.left-1;
    y = r.top+2;
    h = r.bottom - r.top - 4;
    w = r.right - r.left + 2;

    SetWindowPos(hText, NULL, x, y, w, h, SWP_NOZORDER);

    padd = imax(0, (h - get_fontheight(hFont)) / 2);
    r.left  = padd+2;
    r.right = w - (padd+2);
    r.top   = padd;
    r.bottom = h - padd;
    SendMessage(hText, EM_SETRECT, 0, (LPARAM)&r);
}

void StringItem::Measure(HDC hDC, SIZE *size)
{
    MenuItem::Measure(hDC, size);
    if (false == gSettingsMenu.showBroams)
    {
        size->cx = imax(size->cx + 20, 120);
        size->cy += 6;
    }
    SetRectEmpty(&m_textrect);
}

void StringItem::Invoke(int button)
{
    char *buff;
    int len;
    if (gSettingsMenu.showBroams)
        return;
    len = 1 + GetWindowTextLength(hText);
    buff = (char*)m_alloc(len);
    GetWindowText(hText, buff, len);
    replace_str(&m_pszTitle, buff);
    if (SendMessage(hText, EM_GETMODIFY, 0, 0) || (button & INVOKE_RET)) {
        SendMessage(hText, EM_SETMODIFY, FALSE, 0);
        if (strstr(m_pszCommand, "%s") || strstr(m_pszCommand, "%q"))
            post_command_fmt(m_pszCommand, buff);
        else
            post_command_fmt("%s %s", m_pszCommand, buff);
    }
    m_free(buff);
}

//===========================================================================

LRESULT CALLBACK StringItem::EditProc(HWND hText, UINT msg, WPARAM wParam, LPARAM lParam)
{
    StringItem *pItem = (StringItem*)GetWindowLongPtr(hText, GWLP_USERDATA);
    LRESULT r = 0;
    Menu *pMenu = pItem->m_pMenu;

    pMenu->incref();
    switch(msg)
    {
        // --------------------------------------------------------
        // Send Result

        case WM_MOUSEMOVE:
            PostMessage(pMenu->getWindowHandle(), WM_MOUSEMOVE, wParam, MAKELPARAM(10, pItem->m_nTop+2));
            break;

        // --------------------------------------------------------
        // Key Intercept

        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_DOWN:
                case VK_UP:
                case VK_TAB:
                    pItem->Invoke(0);
                    pItem->next_item(wParam);
                    goto leave;

                case VK_RETURN:
                    pItem->Invoke(INVOKE_RET);
                    pItem->next_item(0);
                    goto leave;

                case VK_ESCAPE:
                    SetWindowText(hText, pItem->m_pszTitle);
                    pItem->next_item(0);
                    goto leave;
            }
            break;

        case WM_CHAR:
            switch (wParam)
            {
                case 'A' - 0x40: // ctrl-A: select all
                    SendMessage(hText, EM_SETSEL, 0, GetWindowTextLength(hText));
                case 13:
                case 27:
                    goto leave;
            }
            break;

        // --------------------------------------------------------
        // Paint

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc;
            RECT r;
            StyleItem *pSI;

            hdc = BeginPaint(hText, &ps);
            GetClientRect(hText, &r);
            pSI = &mStyle.MenuFrame;
            makeGradient(hdc, r,
                pSI->type, pSI->Color, pSI->ColorTo,
                pSI->interlaced, BEVEL_SUNKEN, BEVEL1, 0, 0, 0);
            CallWindowProc(pItem->wpEditProc, hText, msg, (WPARAM)hdc, lParam);
            EndPaint(hText, &ps);
            goto leave;
        }

        case WM_ERASEBKGND:
            r = TRUE;
            goto leave;

        case WM_DESTROY:
            pItem->hText = NULL;
            pMenu->setChildEditWindowHandle(NULL);
            break;

        case WM_SETFOCUS:
            break;

        case WM_KILLFOCUS:
            pItem->Invoke(0);
            break;
    }
    r = CallWindowProc(pItem->wpEditProc, hText, msg, wParam, lParam);
leave:
    pMenu->decref();
    return r;
}

} // !namespace bbcore
