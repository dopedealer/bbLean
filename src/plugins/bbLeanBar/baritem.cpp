
#include "baritem.h"
#include "barinfo.h"
#include "bbLeanBar.h"

extern int styleBorderWidth;
extern int styleBevelWidth;
extern HWND BBhwnd;

baritem::baritem(int type, barinfo* bi)
{
    m_bar = bi;
    m_type = type;
    m_active = false;
    m_margin = 0;
    mr.left = mr.right = 0;
    mouse_in = false;
}

baritem::~baritem(void)
{
    release_capture();
}

bool baritem::set_location(int *px, int y, int w, int h, int m)
{
    int x = *px;
    bool f = false;

    if (mr.left != x)
        mr.left = x, f = true;
    x += w;
    if (mr.right != x)
        mr.right = x, f = true;
    *px = x;

    mr.bottom = (mr.top = y) + h;
    m_margin = m;
    return f;
}

int baritem::mouse_over(int mx, int my)
{
    RECT r = mr;
#if 1
    // extend clickable area to screen edge
    int border = styleBorderWidth + styleBevelWidth;
    if (r.top - m_bar->mon_top <= border)
        r.top = m_bar->mon_top;

    if (m_bar->mon_bottom - r.bottom <= border)
        r.bottom = m_bar->mon_bottom;
#endif
    if (my < r.top || my >= r.bottom)
        return 0;
    if (mx < r.left || mx > r.right)
        return -1;
    return 1;
}

bool baritem::menuclick(int message, unsigned flags)
{
    if (flags & MK_CONTROL) {
        if (WM_RBUTTONDOWN == message)
            return true;
        if (WM_RBUTTONDBLCLK == message)
            return true;
        if (WM_RBUTTONUP == message) {
            m_bar->show_menu(true);
            return true;
        }
    }
    return false;
}

void baritem::mouse_event(int mx, int my, int message, unsigned flags)
{
    if (menuclick(message, flags))
        return;
    // default: show bb-menu
    if (WM_RBUTTONUP == message)
        PostMessage(BBhwnd, BB_MENU, 0, 0);
}

void baritem::draw()
{
}

void baritem::settip(void)
{
}

bool baritem::calc_sizes(void)
{
    return false;
};

void baritem::invalidate(int flag)
{
    InvalidateRect(m_bar->hwnd, &mr, FALSE);
}

void baritem::release_capture(void)
{
    if (m_bar->capture_item == this)
    {
        ReleaseCapture();
        m_bar->capture_item = NULL;
    }
}

bool baritem::check_capture(int mx, int my, int message)
{
    bool pa = m_active;
    bool ret = false;
    if (m_bar->capture_item == this)
    {
        int over = mouse_over(mx, my);
        m_active = ret = over > 0;
        if (over < 0 && m_type == M_TASK && m_bar->gesture_lock)
            ret = true;

        if (message != WM_MOUSEMOVE) {
            release_capture();
            m_active = false;
        }
    }
    else
        if (message == WM_LBUTTONDOWN
                || message == WM_RBUTTONDOWN
                || message == WM_LBUTTONDBLCLK
                || message == WM_RBUTTONDBLCLK
           )
        {
            SetCapture(m_bar->hwnd);
            m_bar->capture_item = this;
            m_active = ret = true;
        }

    if (m_active != pa)
        invalidate(0);

    return ret;
}
