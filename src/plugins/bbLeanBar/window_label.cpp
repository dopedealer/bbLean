
#include "window_label.h"
#include "bbLeanBar.h"
#include "barinfo.h"

window_label::window_label(barinfo* bi)
    : barlabel(M_WINL, bi, bi->windowlabel, SN_TOOLBARWINDOWLABEL)
{
}
