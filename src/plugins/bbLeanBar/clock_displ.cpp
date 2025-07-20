
#include <win0x500.h>

#include "clock_displ.h"
#include "barinfo.h"
#include "bbLeanBar.h"

extern HWND BBhwnd; 

clock_displ::clock_displ(barinfo* bi)
    : barlabel(M_CLCK, bi, bi->clockTime, SN_TOOLBARCLOCK)
{
    m_bar->set_clock_string();
}

void clock_displ::mouse_event(int mx, int my, int message, unsigned flags)
{
    if (menuclick(message, flags))
        return;

    int n;
    switch (message)
    {
        case WM_LBUTTONDBLCLK: //n = 7; goto post_click;
            SendMessage(BBhwnd, BB_EXECUTEASYNC, 0, (LPARAM) "control.exe timedate.cpl");
            break;
        case WM_LBUTTONUP: n = 1; goto post_click;
        case WM_RBUTTONUP: n = 2; goto post_click;
        case WM_MBUTTONUP: n = 3; goto post_click;
        case WM_XBUTTONUP:
            switch (HIWORD(flags)) {
            case XBUTTON1: n = 4; goto post_click;
            case XBUTTON2: n = 5; goto post_click;
            case XBUTTON3: n = 6; goto post_click;
            } break;

        post_click:
            flags &= (MK_CONTROL|MK_SHIFT|MK_ALT);
            PostMessage(BBhwnd, BB_DESKCLICK, flags, n);
            break;
    }
}
