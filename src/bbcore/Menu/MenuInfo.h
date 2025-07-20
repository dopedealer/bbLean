#ifndef _BBCORE_MENU_INFO_H
#define _BBCORE_MENU_INFO_H

#include <bbstyle.h>
#include <bbshell.h>

namespace bbcore {

//=======================================
// A structure with precalculated menu metrics,
// as filled in by 'Menu_Reconfigure()'.

struct MenuInfo
{
    HFONT hFrameFont; // fonts...
    HFONT hTitleFont;

    int nTitleHeight; // total height of title
    int nTitleIndent; // left/right text indent

    int nItemHeight; // height of normal item
    int nItemLeftIndent; // text indent
    int nItemRightIndent;

    int nFrameMargin; // outer margin of menu frame, including border
    int nTitleMargin; // frame margin around title (normally 0)

    int nSubmenuOverlap;
    int MaxWidth;     // as configured

    COLORREF separatorColor;
    int separatorWidth; // in pixel
    bool check_is_pr; // whether checkmarks cant use the hilite style
    bool openLeft;

    // presets for possible scrollbuttons
    int nScrollerSize;
    int nScrollerSideOffset;
    int nScrollerTopOffset;
    StyleItem Scroller;

    int nBulletPosition;
    int nBulletStyle;
    int nIconSize;
};

extern MenuInfo gMenuInfo;



}; //!namespace bbcore


#endif //!_BBCORE_MENU_INFO_H
