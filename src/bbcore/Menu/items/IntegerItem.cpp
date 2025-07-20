
#include <cstdio> 
#include <bblib.h>

#include "BB.h"
#include "IntegerItem.h"
#include "Settings.h"
#include "nls.h"

#include "Menu.h"

namespace bbcore { 

IntegerItem::IntegerItem(const char* pszCommand, int value, int minval, int maxval)
    : CommandItem(pszCommand, NULL, false)
{
    m_value = value;
    m_min = minval;
    m_max = maxval;
    m_oldsize = 0;
    m_direction = 0;
    m_count = 0;
    m_offvalue = 0x7FFFFFFF;
    m_offstring = NULL;
    m_ItemID = MENUITEM_ID_INT;
}

void IntegerItem::Measure(HDC hDC, SIZE *size)
{
    if (gSettingsMenu.showBroams)
    {
        MenuItem::Measure(hDC, size);
        m_Justify = MENUITEM_STANDARD_JUSTIFY;
    }
    else
    {
        char buf[100];
        char val[100];
        const char *s;
        if (m_offvalue == m_value && m_offstring)
            s = NLS1(m_offstring);
        else
            s = val, sprintf(val, "%d", m_value);

        sprintf(buf, "%c %s %c",
            m_value == m_min ? ' ': '-',
            s,
            m_value == m_max ? ' ': '+'
            );
        replace_str(&m_pszTitle, buf);

        MenuItem::Measure(hDC, size);
        if (size->cx > m_oldsize+5 || size->cx < m_oldsize-5)
            m_oldsize = size->cx;
        size->cx = m_oldsize + 5;
        m_Justify = DT_CENTER;
    }
}

//====================

void IntegerItem::Mouse(HWND hwnd, UINT uMsg, DWORD wParam, DWORD lParam)
{
    MenuItem::Mouse(hwnd, uMsg, wParam, lParam);
    if (false == gSettingsMenu.showBroams)
        switch(uMsg) {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK:
            {
                int xmouse = (short)LOWORD(lParam);
                int xwidth = m_pMenu->getWidth();
                m_direction = (xmouse < xwidth / 2) ? -1 : 1;
                ItemTimer(MENU_INTITEM_TIMER);
                break;
            }
            case WM_LBUTTONUP:
                break;
        }
}

void IntegerItem::set_next_value(void)
{
    int value, dir, mod, c, d;
    value = m_value;

    dir = m_direction;
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        dir *= 10;
    for (c = 15, d = 2; c < m_count; d = d==2?5:2, c += 10 + 4*d)
        dir *= d;

    mod = value % dir;
    if (mod > 0 && dir < 0) 
        mod+=dir;
    value -= mod;

    m_value = iminmax(value + dir, m_min, m_max);
    m_pMenu->Redraw(0);
    ++m_count;
}

void IntegerItem::ItemTimer(UINT nTimer)
{
    HWND hwnd;
    if (MENU_INTITEM_TIMER != nTimer)
    {
        MenuItem::ItemTimer(nTimer);
        return;
    }
    hwnd = m_pMenu->getWindowHandle();
    if (0 == m_direction || GetCapture() != hwnd)
    {
        KillTimer(hwnd, nTimer);
        return;
    }
    SetTimer(hwnd, MENU_INTITEM_TIMER, 0 == m_count ? 320 : 80, NULL);
    set_next_value();
}

void IntegerItem::Key(UINT nMsg, WPARAM wParam)
{
    Active(2);

    if (WM_KEYDOWN == nMsg)
    {
        if (VK_LEFT == wParam)
        {
            m_direction = -1;
            set_next_value();
        }
        else
        if (VK_RIGHT == wParam)
        {
            m_direction = 1;
            set_next_value();
        }
        else
        if (VK_UP == wParam || VK_DOWN==wParam || VK_TAB == wParam)
        {
            next_item(wParam);
        }
    }
    else
    if (WM_KEYUP == nMsg)
    {
        if (VK_LEFT == wParam || VK_RIGHT == wParam)
        {
            Invoke(INVOKE_LEFT);
        }
    }
    else
    if (WM_CHAR == nMsg && (VK_ESCAPE == wParam || VK_RETURN==wParam))
    {
        next_item(0);
    }

}

void IntegerItem::Invoke(int button)
{
    if (0 == m_direction)
        return;
    m_direction = 0;
    m_count = 0;
    if (INVOKE_LEFT & button) {
        if (strstr(m_pszCommand, "%d"))
            post_command_fmt(m_pszCommand, m_value);
        else
            post_command_fmt("%s %d", m_pszCommand, m_value);
    }
}

} // !namespace bbcore
