
#include "trayitemlist.h"

#include <bblib.h>

#include "barinfo.h"
#include "bbLeanBar.h"
#include "trayentry.h"

trayitemlist::trayitemlist(barinfo* bi)
    : baritemlist(M_TRAYLIST, bi)
{
}

bool trayitemlist::calc_sizes(void)
{
    [[maybe_unused]] int w;
    int h, ts, n, s, xpos;
    itemlist* p{};
    bool f;

    h = mr.bottom - mr.top;
    w = mr.right - mr.left;

    f = false;
    ts = m_bar->GetTraySizeEx();

    if (ts != len)
    {
        clear();
        for (n = 0; n < ts; ++n)
            add(new trayentry(n, m_bar));
        f = true;
        len = ts;
    }

    if (0 == ts)
        return f;

    s = (h - m_bar->TRAY_ICON_SIZE)/2;
    xpos = mr.left;

    n = 0;
    dolist (p, items)
    {
        int right = xpos + m_bar->TRAY_ICON_SIZE+2;
        if (right > mr.right)
            break;

        f |= p->item->set_location(&xpos, mr.top + s - 1, right-xpos, s + m_bar->TRAY_ICON_SIZE + 1, m_margin);
        ++n;
    };

    return f;
}

void trayitemlist::invalidate(int flag)
{
    if (flag == M_TRAYLIST)
        baritemlist::invalidate(M_TRAY);
    else
        baritem::invalidate(0);
}

void trayitemlist::mouse_event(int mx, int my, int message, unsigned flags)
{
    if ((MK_ALT|MK_CONTROL) & flags)
    {
        if (WM_RBUTTONDOWN == message)
            return;
        if (WM_RBUTTONDBLCLK == message)
            return;
        if (WM_RBUTTONUP == message) {
            if (MK_ALT & flags)
                m_bar->trayToggleShowAll(-1);
            else
                m_bar->trayMenu(true);
            return;
        }
    }
    baritemlist::mouse_event(mx, my, message, flags);
}
