
#include "leanbar.h"

#include <bblib.h>

#include "barinfo.h"
#include "bbLeanBar.h"
#include "tooltips.h"
#include "bar_button.h"
#include "spacer.h"
#include "workspace_label.h"
#include "clock_displ.h"
#include "window_label.h"
#include "taskitemlist.h"
#include "trayitemlist.h"

extern int styleBorderWidth;
extern int styleBevelWidth;

LeanBar::LeanBar(barinfo* bi)
    : baritemlist(M_BARLIST, bi)
{ 
}

void LeanBar::settip(void)
{
#ifndef NO_TIPS
    baritemlist::settip();
    ClearToolTips(m_bar->hwnd);
#endif
}

void LeanBar::invalidate(int flag)
{
    if (M_TASKLIST == flag || M_TRAYLIST == flag || M_WINL == flag)
    {
        baritemlist::invalidate(flag); // redraw related items
        return;
    }

    if (UPD_NEW == flag)
        create_bar(); // rebuild items from scratch

    if (calc_sizes() || UPD_DRAW == flag)
    {
        baritem::invalidate(0); // redraw entire bar
        return;
    }

    baritemlist::invalidate(flag);
}

void LeanBar::mouse_event(int mx, int my, int message, unsigned flags)
{
    if (m_bar->hwnd == GetCapture())
    {
        // on capture, the captured item gets the message
        if (m_bar->capture_item)
            m_bar->capture_item->mouse_event(mx, my, message, flags);
        else
            ReleaseCapture(); // bar has been rebuilt in between
        return;

    }
    baritemlist::mouse_event(mx, my, message, flags);
}

void LeanBar::create_bar(void)
{
    char *item_ptr;

    clear();
    max_label_width = 0;
    max_clock_width = 0;

    for (item_ptr = m_bar->item_string; *item_ptr;  item_ptr++)
    {
        switch (*item_ptr)
        {
            case M_TDPB:
            case M_CUOB:
            case M_WSPB_L:
            case M_WSPB_R:
            case M_WINB_L:
            case M_WINB_R:
                add( new bar_button(*item_ptr, m_bar) );
                break;
            case M_NEWLINE:
            case M_SPAC:
                add( new spacer(*item_ptr, m_bar) );
                break;
            case M_WSPL:
                add( new workspace_label(m_bar) );
                break;
            case M_CLCK:
                add( new clock_displ(m_bar) );
                break;
            case M_WINL:
                add( new window_label(m_bar) );
                break;
            case M_TASK:
                add( new taskitemlist(m_bar) );
                break;
            case M_TRAY:
                add( new trayitemlist(m_bar) );
                break;

        }
    }
}

bool LeanBar::calc_sizes(void)
{
    itemlist* p;
    int s, line, top, xpos, height;
    bool f;

    xpos = 0;
    s = m_bar->TRAY_ICON_SIZE + 2;
    trayzone_width = imax(s/3, s*m_bar->GetTraySizeEx());
    f = this->set_location(&xpos, 0, m_bar->width, m_bar->height, 0);

    // loop though lines
    for (p = items, top = line = 0; p; ++line) {
        height = m_bar->bbLeanBarLineHeight[line];
        f |= calc_line_size(&p, top, top + height, height);
        top += height - styleBorderWidth;
    }
    return f;
}

bool LeanBar::calc_line_size(itemlist** p0, int top, int bottom, int height)
{
    int
        winlabel_width,
        taskzone_width,
        label_width,
        clock_width;

    clock_width =
        label_width =
        winlabel_width =
        taskzone_width = 0;

    bool f = false;

    int b = styleBevelWidth;
    int bo = styleBorderWidth + b;
    int button_padding = (height - m_bar->buttonH) / 2 - styleBorderWidth;
    int prev_margin = b;
    int xpos = styleBorderWidth;
    bool isbutton = false;

    int t0, t1, lr;
    t0 = lr = 0;

    itemlist* p;

    // --- 1st pass ----------------------------------
    dolist (p, *p0)
    {
        t1 = p->item->m_type;
        if (M_NEWLINE == t1)
            break;

        int pm = prev_margin;
        prev_margin = b;
        xpos += pm;
        bool ib = isbutton;
        isbutton = false;

        switch (t1)
        {
            case M_TDPB:
            case M_CUOB:
            case M_WSPB_L:
            case M_WSPB_R:
            case M_WINB_L:
            case M_WINB_R:
                xpos += imax(0, button_padding - pm) + m_bar->buttonH;
                if (ib && button_padding == 0)
                    xpos -= m_bar->buttonBorder;
                isbutton = true;
                prev_margin = button_padding;
                break;

            case M_WSPL:
                label_width = m_bar->labelWidth + 3+2*m_bar->labelIndent;
                lr |= 2 + (t0 == 0);
                break;

            case M_CLCK:
                clock_width = m_bar->clockWidth + 3+2*m_bar->labelIndent;
                lr |= 4 + (t0 == 0);
                break;

            case M_WINL:
                winlabel_width = 1;
                break;

            case M_SPAC:
                xpos += styleBevelWidth+1;
                break;

            case M_TASKLIST:
                taskzone_width = 1;
                break;

            case M_TRAYLIST:
                xpos += trayzone_width;
                break;
        }
        t0 = t1;
    }
    if (t0 == M_CLCK || t0 == M_WSPL) lr |= 8;

    xpos += prev_margin + styleBorderWidth;

    // --- label and clock balance ---------------------------------
    if ((lr & 2) && (max_label_width < label_width || max_label_width >= label_width + 12))
        max_label_width = label_width + 4;
    if ((lr & 4) && (max_clock_width < clock_width || max_clock_width >= clock_width + 12))
        max_clock_width = clock_width + 4;
    if (lr == 15)
        max_label_width = max_clock_width = imax(max_label_width, max_clock_width);
    if (lr & 2)
        xpos += max_label_width;
    if (lr & 4)
        xpos += max_clock_width;

    // --- assign variable widths ----------------------------------
    int rest_width = imax(0, mr.right - xpos);

    if (taskzone_width && winlabel_width)
    {
        if (1 == m_bar->TaskStyle) // icons only
        {
            taskzone_width = imin(rest_width,
                    (m_bar->TASK_ICON_SIZE + b + 2) * m_bar->GetTaskListSizeEx() - b
                    );
            winlabel_width = rest_width - taskzone_width;
        }
        else
        {
            winlabel_width = rest_width / 2;
            if (winlabel_width > 200) winlabel_width = 200;
            if (winlabel_width < 32)  winlabel_width = 0;
            taskzone_width = rest_width - winlabel_width;
        }
    }
    else if (winlabel_width)
    {
        winlabel_width = rest_width;
    }
    else if (taskzone_width)
    {
        taskzone_width = rest_width;
    }

    // --- 2nd pass ----------------------------------
    prev_margin = b;
    xpos = styleBorderWidth;

    dolist (p, *p0) {
        baritem *gi = p->item;

        if (M_NEWLINE == gi->m_type) {
            p = p->next;
            break;
        }

        int ypos = top + bo;
        int hs = height - bo - bo;
        int ws = 0;
        int pm = prev_margin;
        bool ib = isbutton;

        xpos += pm;
        prev_margin = b;
        isbutton = false;

        switch (gi->m_type) {
            case M_TDPB:
            case M_CUOB:
            case M_WSPB_L:
            case M_WSPB_R:
            case M_WINB_L:
            case M_WINB_R:
                xpos += imax(0, button_padding-pm);
                if (ib && button_padding == 0)
                    xpos -= m_bar->buttonBorder;
                isbutton = true;
                prev_margin = button_padding;
                ypos = top + (height - (hs = ws = m_bar->buttonH)) / 2;
                break;

            case M_SPAC:
                ws = b+1;
                break;

            case M_TRAYLIST:
                ws = trayzone_width;
                break;

            case M_WSPL:
                ws = max_label_width;
                goto label_ypos;
            case M_CLCK:
                ws = max_clock_width;
                goto label_ypos;
            case M_WINL:
                ws = winlabel_width;
                goto label_ypos;
            case M_TASKLIST:
                ws = taskzone_width;
                goto label_ypos;

label_ypos:
                ypos = top + (height - (hs = m_bar->labelH)) / 2;
                break;

        }

        f |= gi->set_location(&xpos, ypos, ws, hs, ypos - top);
        if (gi->m_type >= M_BARLIST)
            gi->calc_sizes();
    }
    *p0 = p;
    return f;
}
