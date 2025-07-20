
#include "workspace_label.h"
#include "bbLeanBar.h"
#include "barinfo.h"

extern HWND BBhwnd;


workspace_label::workspace_label(barinfo* bi)
    : barlabel(M_WSPL, bi, gScreenName, SN_TOOLBARLABEL)
{
}

void workspace_label::mouse_event(int mx, int my, int message, unsigned flags)
{
    if (menuclick(message, flags))
        return;

    if (message == WM_LBUTTONUP)
    {
        PostMessage(BBhwnd, BB_WORKSPACE, BBWS_DESKRIGHT, 0);
    }
    else if (message == WM_RBUTTONUP)
    {
        PostMessage(BBhwnd, BB_WORKSPACE, BBWS_DESKLEFT, 0);
    }
    else if (message == WM_MBUTTONUP)
    {
        PostMessage(BBhwnd, BB_MENU, 1, 0);
    }
}
