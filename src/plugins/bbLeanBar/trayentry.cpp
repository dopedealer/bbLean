
#include "trayentry.h"
#include "barinfo.h"
#include "bbLeanBar.h"
#include "drawico.h"
#include "tooltips.h"

trayentry::trayentry(int index, barinfo* bi)
    : baritem(M_TRAY, bi)
{
    m_index = index;
}

void trayentry::invalidate(int flag)
{
    baritem::invalidate(0);
}

void trayentry::draw(void)
{
    systemTray* icon = m_bar->GetTrayIconEx(m_index);
    if (icon) {
        DrawIconSatnHue (m_bar->hdcPaint, mr.left+1, mr.top+1,
                icon->hIcon, m_bar->TRAY_ICON_SIZE, m_bar->TRAY_ICON_SIZE,
                0, NULL, DI_NORMAL,
                false == mouse_in, m_bar->saturationValue, m_bar->hueIntensity);
    }
}

void trayentry::settip(void)
{
#ifndef NO_TIPS
    systemTray* icon = m_bar->GetTrayIconEx(m_index);
    if (icon) {
        SetToolTip(m_bar->hwnd, &mr, icon->szTip);
        make_bb_balloon(m_bar, icon, &mr);
    }
#endif
}

void trayentry::mouse_event(int mx, int my, int message, unsigned flags)
{
    if (MK_SHIFT & flags) {
        if (WM_RBUTTONDOWN == message)
            return;
        if (WM_RBUTTONDBLCLK == message)
            return;
        if (WM_RBUTTONUP == message) {
            m_bar->trayShowIcon(m_bar->RealTrayIndex(m_index), -1);
            return;
        }
    }

    if ((WM_MOUSEMOVE == message && false == mouse_in)
            || WM_MOUSELEAVE == message
       )
        InvalidateRect(m_bar->hwnd, &mr, FALSE);

    systemTrayIconPos pos;
    pos.hwnd = m_bar->hwnd;
    pos.r = mr;
    ForwardTrayMessage(m_bar->RealTrayIndex(m_index), message, &pos);
}
