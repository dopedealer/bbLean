

#include "bar_button.h" 

#include <BBApi.h>
#include <bbstyle.h>

#include "bbLeanBar.h"
#include "barinfo.h"
#include "tooltips.h"
#include "BuffBmp.h"

extern HWND BBhwnd; 

bar_button::bar_button(int m, barinfo* bi)
    : baritem(m, bi)
{
    dir = m==M_TDPB || m==M_CUOB ? 0 : m==M_WINB_L || m==M_WSPB_L ? -1 : 1;
}

void bar_button::draw(void)
{
    StyleItem* pSI = (StyleItem *)GetSettingPtr(
            m_active || (dir>0 && m_bar->force_button_pressed)
            ? SN_TOOLBARBUTTONP : SN_TOOLBARBUTTON
            );

    m_bar->pBuff->MakeStyleGradient(m_bar->hdcPaint, &mr, pSI, pSI->bordered);

    if (0 == dir)
    {
        HPEN Pen   = CreatePen(PS_SOLID, 1, pSI->picColor);
        HGDIOBJ other = SelectObject(m_bar->hdcPaint, Pen);
        int w = (mr.right - mr.left) / 2;
        int x = mr.left + w;
        int y = mr.top  + w;
        int z = 2;
        Arc(m_bar->hdcPaint, x-2, y-2, x+z, y+z, x,0,x,0);
        if (m_type == M_CUOB && false == m_bar->currentOnly)
        {
            z--;
            MoveToEx(m_bar->hdcPaint,   x-1,  y-z, NULL);
            LineTo(m_bar->hdcPaint,     x-1,  y+z);
            MoveToEx(m_bar->hdcPaint,   x,  y-z, NULL);
            LineTo(m_bar->hdcPaint,     x,  y+z);
            MoveToEx(m_bar->hdcPaint,   x+1,  y-z, NULL);
            LineTo(m_bar->hdcPaint,     x+1,  y+z);
        }
        DeleteObject(SelectObject(m_bar->hdcPaint, other));
    }
    else
    {
        bbDrawPix(m_bar->hdcPaint, &mr, pSI->picColor, dir > 0 ? BS_TRIANGLE : -BS_TRIANGLE);
    }

}

void bar_button::mouse_event(int mx, int my, int message, unsigned flags)
{
    if (false == check_capture(mx, my, message))
        return;

    if (message == WM_LBUTTONUP) {
        switch (m_type) {
            case M_CUOB:
                m_bar->currentOnly = false == m_bar->currentOnly;
                m_bar->NewTasklist();

                m_bar->WriteRCSettings();
                m_bar->update(UPD_DRAW);
                break;

            case M_TDPB:
                if (++m_bar->TaskStyle == 3)
                    m_bar->TaskStyle = 0;

                m_bar->WriteRCSettings();
                m_bar->update(UPD_DRAW);
                break;

            case M_WINB_L:
            case M_WINB_R:
                PostMessage(BBhwnd, BB_WORKSPACE,
                        (dir > 0) ^ m_bar->reverseTasks
                        ? BBWS_NEXTWINDOW
                        : BBWS_PREVWINDOW,
                        m_bar->currentOnly ^ (0 == (flags & MK_SHIFT))
                        );
                break;

            case M_WSPB_L:
            case M_WSPB_R:
                PostMessage(BBhwnd, BB_WORKSPACE, dir>0 ? BBWS_DESKRIGHT:BBWS_DESKLEFT, 0);
                break;
        }
    } else if (message == WM_RBUTTONUP) {
        if (m_type == M_CUOB)
            m_bar->trayToggleShowAll(-1);
    }
}

void bar_button::settip(void)
{
    if (hasCustomTip)
    {
        if (m_type == M_CUOB)
        {
            SetToolTip(m_bar->hwnd, &mr,
                    "left-click: toggle current-only taskmode"
                    "\nright-click: toggle hidden trayicons"
                    );
        }
    }
    else
    {
        baritem::settip();
    }
}
