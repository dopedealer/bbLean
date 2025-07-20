

#include "taskentry.h"

#include <drawico.h>

#include "barinfo.h"
#include "bbLeanBar.h"
#include "tooltips.h"
#include "BuffBmp.h"

extern HWND BBhwnd; 
extern int TBJustify; 

taskentry::taskentry(int index, barinfo* bi)
    : baritem(M_TASK, bi)
{
    m_index = index;
    m_dblclk = false;
    m_showtip = false;
    a1 = a2 = false;
}

void taskentry::draw()
{
    m_showtip = false;

    tasklist* tl = m_bar->GetTaskPtrEx(m_index);
    if (NULL==tl) return;

    bool lit = tl->active
        || tl->flashing
        || tl->hwnd == m_bar->task_lock
        || m_active
        ;

    a1 = tl->active;
    StyleItem *pSI;

    if (lit) pSI = &ATaskStyle;
    else pSI = &NTaskStyle;

    if (m_bar->TaskStyle == 1)
        draw_icons(tl, lit, pSI);
    else
        draw_text(tl, lit, pSI);

}

void taskentry::draw_icons(tasklist* tl, bool lit, StyleItem* pSI)
{
    if (lit)
    {
        bool bordered = pSI->bordered || pSI->parentRelative;
        m_bar->pBuff->MakeStyleGradient(m_bar->hdcPaint, &mr, pSI, bordered);
    }

    HICON icon = tl->icon;
    if (NULL == icon)
        icon = LoadIcon(NULL, IDI_APPLICATION);

    int o = (mr.bottom-mr.top-m_bar->TASK_ICON_SIZE)/2;

    DrawIconSatnHue (m_bar->hdcPaint, mr.left+o, mr.top+o,
            icon, m_bar->TASK_ICON_SIZE, m_bar->TASK_ICON_SIZE,
            0, NULL, DI_NORMAL,
            false == lit, m_bar->saturationValue, m_bar->hueIntensity);

    m_showtip = true;
}

void taskentry::draw_text(tasklist* tl, bool lit, StyleItem *pSI)
{
    bool bordered;
    if (m_bar->task_with_border || (lit && pSI->parentRelative))
        bordered = true;
    else
        if (false == m_bar->task_with_border && false == lit)
            bordered = false;
        else
            bordered = pSI->bordered;

    m_bar->pBuff->MakeStyleGradient(m_bar->hdcPaint, &mr, pSI, bordered);

    HGDIOBJ oldfont = SelectObject(m_bar->hdcPaint, m_bar->hFont);
    SetBkMode(m_bar->hdcPaint, TRANSPARENT);

    RECT ThisWin = mr;
    RECT s1 = {0,0,0,0};
    RECT s2 = {0,0,0,0};
    char buf[8];
    strncpy(buf, tl->caption, 8);
    buf[7] = 0;
    bbDrawText(m_bar->hdcPaint, buf, &s1, DT_CALCRECT|DT_NOPREFIX, 0);
    bbDrawText(m_bar->hdcPaint, tl->caption, &s2, DT_CALCRECT|DT_NOPREFIX, 0);

    int o, f, i;
    o = f = 0;
    if ((m_bar->TaskStyle&2) && NULL != tl->icon)
    {
        o = (mr.bottom-mr.top-m_bar->TASK_ICON_SIZE)/2;
        f = m_bar->TASK_ICON_SIZE + o - m_bar->labelBorder;
    }

    i = m_bar->labelIndent;
    ThisWin.left    += i+f;
    ThisWin.right   -= i;
    int s = ThisWin.right - ThisWin.left;

    bbDrawText(m_bar->hdcPaint, tl->caption, &ThisWin,
            (s > s1.right ? TBJustify : DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_NOPREFIX),
            pSI->TextColor
            );

    SelectObject(m_bar->hdcPaint, oldfont);

    if (s<s2.right)
        m_showtip = true;

    if (f)
    {
        DrawIconSatnHue (m_bar->hdcPaint, mr.left+o, mr.top+o,
                tl->icon, m_bar->TASK_ICON_SIZE, m_bar->TASK_ICON_SIZE,
                0, NULL, DI_NORMAL,
                false == lit, m_bar->saturationValue, m_bar->hueIntensity);
    }
}

void taskentry::settip()
{
#ifndef NO_TIPS
    if (m_showtip)
    {
        tasklist* tl = m_bar->GetTaskPtrEx(m_index);
        if (tl) SetToolTip(m_bar->hwnd, &mr, tl->caption);
    }
#endif
}

void taskentry::mouse_event(int mx, int my, int message, unsigned flags)
{
    HWND Window;
    tasklist* tl;
    bool shift_down, iconic;
    int gesture, d, w;

    if (menuclick(message, flags))
        return;
    if (false == check_capture(mx, my, message) && message != BB_DRAGOVER)
        return;
    tl = m_bar->GetTaskPtrEx(m_index);
    if (NULL == tl)
        return;

    Window = tl->hwnd;
    shift_down = 0 != (flags & MK_SHIFT);

    switch (message)
    {
        //====================
        // Restore and focus window
        case WM_LBUTTONUP:
            gesture = 0;
            d = mx - press_x;
            w = (m_bar->mon_rect.right - m_bar->mon_rect.left) / 30;
            if (d < -w)
                gesture = -1;
            if (d > w)
                gesture = 1;

            iconic = FALSE != IsIconic(Window);
            if (m_bar->taskSysmenu
                    && false == iconic
                    && false == gesture
                    && false == sysmenu_exists()
                    && tl->wkspc == currentScreen
                    && a2
                    && (WS_MINIMIZEBOX & GetWindowLong(Window, GWL_STYLE)))
                goto minimize;

            if (gesture && m_bar->gesture_lock) {
                if (tl->wkspc != currentScreen) {
                    PostMessage(BBhwnd, BB_BRINGTOFRONT, BBBTF_CURRENT, (LPARAM)Window);
                } else {
                    if (gesture < 0)
                        PostMessage(BBhwnd, BB_WORKSPACE, BBWS_MOVEWINDOWLEFT, (LPARAM)Window);
                    else
                        PostMessage(BBhwnd, BB_WORKSPACE, BBWS_MOVEWINDOWRIGHT, (LPARAM)Window);
                    PostMessage(BBhwnd, BB_BRINGTOFRONT, BBBTF_CURRENT, (LPARAM)Window);
                }
            }
            else if (shift_down)
                PostMessage(BBhwnd, BB_BRINGTOFRONT, BBBTF_CURRENT, (LPARAM)Window);
            else
                PostMessage(BBhwnd, BB_BRINGTOFRONT, 0,  (LPARAM)Window);

            // Avoid flicker between when the mouse is released and
            // the window becomes active
            m_bar->task_lock = Window;
            SetTimer(m_bar->hwnd, TASKLOCK_TIMER, 400, NULL);
            break;

            //====================

        case WM_RBUTTONUP:
            if (shift_down)
            {
                PostMessage(BBhwnd, BB_WINDOWCLOSE, 0, (LPARAM)Window);
                break;
            }

            if (m_bar->taskSysmenu)
            {
                RECT r = mr;
                int b = m_margin - NTaskStyle.borderWidth;
                r.top -= b;
                r.bottom += b;
                ClientToScreen(m_bar->hwnd, (POINT*)&r.left);
                ClientToScreen(m_bar->hwnd, (POINT*)&r.right);
                ShowSysmenu(Window, m_bar->hwnd, &r, MY_BROAM);
                break;
            }

minimize:
            PostMessage(BBhwnd, BB_WINDOWMINIMIZE, 0, (LPARAM)Window);
            //PostMessage(Window, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            break;

            //====================
            // Move window to the next/previous workspace

        case WM_MBUTTONUP:
            if (tl->wkspc != currentScreen)
                PostMessage(BBhwnd, BB_BRINGTOFRONT, BBBTF_CURRENT, (LPARAM)Window);
            else
                if (shift_down)
                    PostMessage(BBhwnd, BB_WORKSPACE, BBWS_MOVEWINDOWLEFT, (LPARAM)Window);
                else
                    PostMessage(BBhwnd, BB_WORKSPACE, BBWS_MOVEWINDOWRIGHT, (LPARAM)Window);
            break;

            //====================

        case WM_LBUTTONDBLCLK:
            a2 = a1;
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
            m_dblclk = m_active;
            break;

        case WM_LBUTTONDOWN:
            press_x = mx;
            if (m_bar->sendToGesture) {
                m_bar->gesture_lock = true;
                SetTimer(m_bar->hwnd, GESTURE_TIMER, 500, NULL);
            }

            a2 = a1;
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            m_dblclk = false;
            break;

            //====================
        case BB_DRAGOVER:
            m_bar->task_over_hwnd = Window;
            break;

            //====================
    }
}
