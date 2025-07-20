
#include <BB.h>
#include <Settings.h>

#include "SeparatorItem.h" 
#include "MenuInfo.h" 

namespace bbcore { 

void SeparatorItem::Measure(HDC hDC, SIZE *size)
{
    if (gSettingsMenu.drawSeparators)
        //size->cy = gMenuInfo.separatorWidth + imax(gMenuInfo.nItemHeight*2/5 & ~1, 2*mStyle.MenuFrame.marginWidth);
        size->cy = gMenuInfo.separatorWidth + 2*mStyle.MenuFrame.marginWidth;
    else
        size->cy = gMenuInfo.nItemHeight*3/5;

    size->cx = 0;
}

void SeparatorItem::Paint(HDC hDC)
{
    RECT rect; int y, d;
    if (false == gSettingsMenu.drawSeparators)
        return;
    GetTextRect(&rect);
    d = gMenuInfo.separatorWidth;
    y = (rect.top + rect.bottom - d) / 2;
    draw_line_h(hDC, rect.left, rect.right, y, d, gMenuInfo.separatorColor);
}

} // !namespace bbcore
