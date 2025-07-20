
#include "taskitemlist.h"

#include <bblib.h>

#include "taskentry.h"
#include "barinfo.h"
#include "bbLeanBar.h"

extern int styleBorderWidth;
extern int styleBevelWidth;

taskitemlist::taskitemlist(barinfo* bi)
    : baritemlist(M_TASKLIST, bi)
{}

bool taskitemlist::calc_sizes(void)
{
    bool f = false;
    int ts = m_bar->GetTaskListSizeEx();
    int n;

    if (ts != len)
    {
        clear();
        for (n = 0; n < ts; ++n)
            add(new taskentry(m_bar->reverseTasks ? ts-n-1 : n, m_bar));
        f = true;
        len = ts;
    }

    if (0 == ts)
        return f;

    int b = styleBevelWidth;
    int w = mr.right - mr.left + b;
    int h = mr.bottom - mr.top;
    int xpos = mr.left;
    //int is = m_bar->TASK_ICON_SIZE + b + 2;
    int is = h + b;
    bool icon_mode = 1 == m_bar->TaskStyle;
    int min_width = is / 2;
    int max_width = imax(w * m_bar->taskMaxWidth / 100, is);
    if (w / ts >= max_width)
        w = ts * max_width;

    itemlist* p;
    n = 0;
    dolist (p, items)
    {
        int left, right;
        if (icon_mode)
        {
            left = xpos + n * is;
            right = left + is - b;
        }
        else
        {
            left = xpos + w * n / ts;
            right = xpos + w * (n+1) / ts - b;
            if (right - left < min_width)
                right = left + min_width;
        }

        if (right > mr.right)
            break;

        f |= p->item->set_location(&left, mr.top, right - left, h, m_margin);
        ++n;
    }
    return f;
}

void taskitemlist::mouse_event(int mx, int my, int message, unsigned flags)
{
    if (flags & MK_ALT)
    {
        switch (message)
        {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK:
                if (++m_bar->TaskStyle==3)
                    m_bar->TaskStyle=0;

                m_bar->WriteRCSettings();
                m_bar->update(UPD_DRAW);
                break;

            case WM_RBUTTONDOWN:
            case WM_RBUTTONDBLCLK:
                m_bar->currentOnly = false==m_bar->currentOnly;
                m_bar->NewTasklist();

                m_bar->WriteRCSettings();
                m_bar->update(UPD_DRAW);
                break;
        }
        return;
    }
    baritemlist::mouse_event(mx, my, message, flags);
}

void taskitemlist::invalidate(int flag)
{
    baritem::invalidate(0);
}
