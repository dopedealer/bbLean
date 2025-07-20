/*---------------------------------------------------------------------------*

  This file is part of the BBNote source code

  Copyright 2003-2009 grischka@users.sourceforge.net

  BBNote is free software, released under the GNU General Public License
  (GPL version 2). For details see:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

 *---------------------------------------------------------------------------*/
// readstyle.cpp - read blackbox style

#include "bbstyle_bbnote.h"
#include "Settings.h"

bbcore::StyleStruct gMStyle{};
bbcore::menu_setting gSettingsMenuBbnote{};
bool Settings_menusBroamMode = false;
int Settings_menuMaxWidth = 200;
bbcore::MenuInfo gMenuInfo{};

extern bbcore::SettingsCommon* gSettings; // todo: remove global

struct BBNoteFolderItem
{
    static int m_nBulletStyle;
    static int m_nBulletPosition;
};

int BBNoteFolderItem::m_nBulletStyle;
int BBNoteFolderItem::m_nBulletPosition;

LPCSTR localStylePath(LPCSTR styleFileName)
{
    static char path[MAX_PATH];
    if (styleFileName)
    {
        strcpy(path, styleFileName); // TODO: fix buffer overflow possibility
    }
    return path;
}

COLORREF get_bg_color(StyleItem *pSI)
{
    if (B_SOLID == pSI->type) // && false == pSI->interlaced)
        return pSI->Color;
    return mixcolors(pSI->Color, pSI->ColorTo, 128);
}

COLORREF get_mixed_color(StyleItem *pSI)
{
    COLORREF b = get_bg_color(pSI);
    COLORREF t = pSI->TextColor;
    if (greyvalue(b) > greyvalue(t))
        return mixcolors(t, b, 96);
    else
        return mixcolors(t, b, 144);
}

//===========================================================================
// Function: get_fontheight
//===========================================================================

int get_menu_bullet (const char *tmp)
{
    static const char * const bullet_strings[] = {
        "triangle" ,
        "square"   ,
        "diamond"  ,
        "circle"   ,
        "empty"    ,
        NULL
        };

    static const char bullet_styles[] = {
        BS_TRIANGLE  ,
        BS_SQUARE    ,
        BS_DIAMOND   ,
        BS_CIRCLE    ,
        BS_EMPTY     ,
        };

    int i = get_string_index(tmp, bullet_strings);
    if (-1 != i)
        return bullet_styles[i];
    return BS_TRIANGLE;
}

void MenuMaker_Clear(void)
{
    if (gMenuInfo.hTitleFont) DeleteObject(gMenuInfo.hTitleFont);
    if (gMenuInfo.hFrameFont) DeleteObject(gMenuInfo.hFrameFont);
    gMenuInfo.hTitleFont = gMenuInfo.hFrameFont = NULL;
}

void Menu_Reconfigure();

void GetStyle (const char *styleFile)
{
    if (styleFile)
    {
        localStylePath(styleFile); 
        gSettings->readStyle(styleFile, &gMStyle);
    }

    bbcore::bimage_init(true, gMStyle.is_070);

    Menu_Reconfigure();
    gMStyle.borderWidth = gMStyle.MenuFrame.borderWidth;
}

//===========================================================================

void Menu_Clear(void)
{
    if (gMenuInfo.hTitleFont)
        DeleteObject(gMenuInfo.hTitleFont);
    if (gMenuInfo.hFrameFont)
        DeleteObject(gMenuInfo.hFrameFont);
    gMenuInfo.hTitleFont =
    gMenuInfo.hFrameFont = NULL;
}

//===========================================================================

void Menu_Reconfigure(void)
{
    // clear fonts
    Menu_Clear();

    StyleItem *MTitle = &gMStyle.MenuTitle;
    StyleItem *MFrame = &gMStyle.MenuFrame;
    StyleItem *MHilite = &gMStyle.MenuHilite;
    StyleItem *pSI;

    // create fonts
    gMenuInfo.hTitleFont = gSettings->createStyleFont(MTitle);
    gMenuInfo.hFrameFont = gSettings->createStyleFont(MFrame);

    // set bullet position & style
    gMenuInfo.nBulletPosition =
        stristr(gMStyle.menuBulletPosition, "left") ? FOLDER_LEFT : FOLDER_RIGHT;

    gMenuInfo.nBulletStyle =
        get_menu_bullet(gMStyle.menuBullet);

    if (0 == stricmp(gSettingsMenuBbnote.openDirection, "bullet"))
        gMenuInfo.openLeft = gMenuInfo.nBulletPosition == FOLDER_LEFT;
    else
        gMenuInfo.openLeft = 0 == stricmp(gSettingsMenuBbnote.openDirection, "left");

    // --------------------------------------------------------------
    // calulate metrics:

    gMenuInfo.nFrameMargin = MFrame->marginWidth + MFrame->borderWidth;
    gMenuInfo.nSubmenuOverlap = gMenuInfo.nFrameMargin + MHilite->borderWidth;
    gMenuInfo.nTitleMargin = 0;

    if (gMStyle.menuTitleLabel)
        gMenuInfo.nTitleMargin = MFrame->marginWidth;

    // --------------------------------------
    // title height, indent, margin

    int tfh = get_fontheight(gMenuInfo.hTitleFont);
    int titleHeight = 2*MTitle->marginWidth + tfh;

    // xxx old behaviour xxx
    if (false == gMStyle.is_070 && 0 == (MTitle->validated & V_MAR))
        titleHeight = MTitle->FontHeight + 2*gMStyle.bevelWidth;
    //xxxxxxxxxxxxxxxxxxxxxx

    pSI = MTitle->parentRelative ? MFrame : MTitle;
    gMenuInfo.nTitleHeight = titleHeight + MTitle->borderWidth + MFrame->borderWidth;
    gMenuInfo.nTitleIndent = imax(imax(2 + pSI->bevelposition, pSI->marginWidth), (titleHeight-tfh)/2);

    if (gMStyle.menuTitleLabel) {
        gMenuInfo.nTitleHeight += MTitle->borderWidth + MFrame->marginWidth;
        gMenuInfo.nTitleIndent += MTitle->borderWidth;
    }

    // --------------------------------------
    // item height, indent

    int ffh = get_fontheight(gMenuInfo.hFrameFont);
    int itemHeight = MHilite->marginWidth + ffh;

    // xxx old behaviour xxx
    if (false == gMStyle.is_070 && 0 == (MHilite->validated & V_MAR))
        itemHeight = MFrame->FontHeight + (gMStyle.bevelWidth+1)/2;
    //xxxxxxxxxxxxxxxxxxxxxx

#ifdef BBOPT_MENUICONS
    itemHeight = imax(14, itemHeight);
    gMenuInfo.nItemHeight =
    gMenuInfo.nItemLeftIndent =
    gMenuInfo.nItemRightIndent = itemHeight;
    gMenuInfo.nIconSize = imin(itemHeight - 2, 16);
    if (DT_LEFT == MFrame->Justify)
        gMenuInfo.nItemLeftIndent += 1;
#else
    gMenuInfo.nItemHeight = itemHeight;
    gMenuInfo.nItemLeftIndent =
    gMenuInfo.nItemRightIndent = imax(11, itemHeight);
#endif

#ifdef BBXMENU
    if (DT_CENTER != MFrame->Justify) {
        int n = imax(3 + MHilite->borderWidth, (itemHeight-ffh)/2);
        if (gMenuInfo.nBulletPosition == FOLDER_RIGHT)
            gMenuInfo.nItemLeftIndent = n;
        else
            gMenuInfo.nItemRightIndent = n;
    }
#endif

    // --------------------------------------
    // from where on does it need a scroll button:
    gMenuInfo.MaxWidth = gSettingsMenuBbnote.showBroams
        ? iminmax(gSettingsMenuBbnote.maxWidth*2, 320, 640)
        : gSettingsMenuBbnote.maxWidth;

    // --------------------------------------
    // setup a StyleItem for the scroll rectangle
    StyleItem *pScrl = &gMenuInfo.Scroller;
    if (false == MTitle->parentRelative)
    {
        *pScrl = *MTitle;
        if (false == gMStyle.menuTitleLabel) {
            pScrl->borderColor = MFrame->borderColor;
            pScrl->borderWidth = imin(MFrame->borderWidth, MTitle->borderWidth);
        }

    } else {
        *pScrl = *MHilite;
        if (pScrl->parentRelative) {
            if (MFrame->borderWidth) {
                pScrl->borderColor = MFrame->borderColor;
                pScrl->borderWidth = MFrame->borderWidth;
            } else {
                pScrl->borderColor = MFrame->TextColor;
                pScrl->borderWidth = 1;
            }
        }
    }

    pScrl->bordered = 0 != pScrl->borderWidth;

    gMenuInfo.nScrollerSize =
        imin(itemHeight + imin(MFrame->borderWidth, pScrl->borderWidth),
            titleHeight + 2*pScrl->borderWidth
            );

    if (gMStyle.menuTitleLabel) {
        gMenuInfo.nScrollerTopOffset = 0;
        gMenuInfo.nScrollerSideOffset = gMenuInfo.nFrameMargin;
    } else {
        // merge the slider's border into the frame/title border
        if (MTitle->parentRelative)
            gMenuInfo.nScrollerTopOffset = 0;
        else
            gMenuInfo.nScrollerTopOffset =
                - (MFrame->marginWidth + imin(MTitle->borderWidth, pScrl->borderWidth));
        gMenuInfo.nScrollerSideOffset = imax(0, MFrame->borderWidth - pScrl->borderWidth);
    }

    // Menu separator line
    gMenuInfo.separatorColor = get_mixed_color(MFrame);
    gMenuInfo.separatorWidth = gSettingsMenuBbnote.drawSeparators ? imax(1, MFrame->borderWidth) : 0;
    gMenuInfo.check_is_pr = MHilite->parentRelative
        || iabs(greyvalue(get_bg_color(MFrame))
                - greyvalue(get_bg_color(MHilite))) < 24;
}

//===========================================================================

