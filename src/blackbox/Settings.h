/* ==========================================================================

  This file is part of the bbLean source code
  Copyright © 2001-2003 The Blackbox for Windows Development Team
  Copyright © 2004-2009 grischka

  http://bb4win.sourceforge.net/bblean
  http://developer.berlios.de/projects/bblean

  bbLean is free software, released under the GNU General Public License
  (GPL version 2). For details see:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  ========================================================================== */

#ifndef _BBSETTINGS_H_
#define _BBSETTINGS_H_

#include <bbstyle.h>

#include "Stylestruct.h"

//===========================================================================
// Settings.cpp functions

void Settings_ReadRCSettings(void);
void Settings_ReadStyleSettings(void);
void Settings_WriteRCSetting(const void *);
int Settings_ItemSize(int w);
COLORREF get_bg_color(StyleItem *pSI);
COLORREF get_mixed_color(StyleItem *pSI);
int checkfont (const char *face);

//=====================================================
// Style definitions

extern StyleStruct mStyle;

//====================
// Toolbar Config

struct toolbar_setting
{
    char placement[20];
    int  widthPercent;
    char strftimeFormat[40];
    bool onTop;
    bool autoHide;
    bool pluginToggle;
    bool enabled;
    bool alphaEnabled;
    int  alphaValue;
};

extern toolbar_setting  Settings_toolbar;

//====================
// Menu Config
struct menu_setting
{
    struct
    {
        int x;
        int  y;
    } pos;
    int  popupDelay;
    int  mouseWheelFactor;
    int  maxWidth;
    char openDirection[20];
    bool onTop;
    bool sticky;
    bool pluginToggle;
    bool showBroams;
    bool showHiddenFiles;
    bool sortByExtension;
    bool drawSeparators;
    bool snapWindow;
    bool dropShadows;
    bool alphaEnabled;
    int  alphaValue;
};

extern struct menu_setting  Settings_menu;

//====================
// workspaces
extern bool Settings_styleXPFix;
extern bool Settings_followActive;
extern bool Settings_altMethod;
extern int  Settings_workspaces;
extern char Settings_workspaceNames[200];

//====================
// Plugin Snap
extern int Settings_snapThreshold;
extern int Settings_snapPadding;
extern bool Settings_snapPlugins;

//====================
// Desktop

// Margins
extern RECT Settings_desktopMargin;
extern bool Settings_fullMaximization;

// Background
extern bool Settings_enableBackground;
extern bool Settings_smartWallpaper;

// Options
extern bool Settings_desktopHook;
extern bool Settings_hideExplorer;
extern bool Settings_hideExplorerTray;

//====================
// Other

// window behaviour
extern bool Settings_opaqueMove;
extern char Settings_focusModel[40];
extern int  Settings_autoRaiseDelay;

// misc
extern char Settings_preferredEditor[MAX_PATH];
extern bool Settings_useDefCursor;
extern bool Settings_arrowUnix;
extern bool Settings_globalFonts;
extern bool Settings_imageDither;
extern bool Settings_shellContextMenu;
extern bool Settings_UTF8Encoding;
extern bool Settings_OldTray;
extern int Settings_contextMenuAdjust[2];
extern int Settings_LogFlag;

// feature select
extern bool Settings_disableTray;
extern bool Settings_disableDesk;
extern bool Settings_disableDDE;
extern bool Settings_disableVWM;
extern bool Settings_disableMargins;

//====================
// --- unused *nix settings ---
//extern bool Settings_focusLastWindow;
//extern bool Settings_focusNewWindows;
//extern char Settings_windowPlacement[40];
//extern char Settings_colPlacementDirection[40];
//extern char Settings_rowPlacementDirection[40];
//extern bool Settings_desktopWheel;

//===========================================================================
// Settings.cpp internal definitions

#define V_MAR 0x0200
void ReadStyle(const char *style, StyleStruct *pStyle);

#ifdef BBSETTINGS_INTERNAL

#define V_TEX 0x0001
#define V_CO1 0x0002
#define V_CO2 0x0004
#define V_TXT 0x0008
#define V_PIC 0x0010
#define V_FON 0x0020
#define V_FHE 0x0040
#define V_FWE 0x0080
#define V_JUS 0x0100
#define V_MAR 0x0200
#define V_BOW 0x0400
#define V_BOC 0x0800
#define V_DIS 0x1000

struct items {
    short type;
    short sn;
    const char *rc_string;
    int sn_def;
    unsigned flags;
};

const struct items *GetStyleItems(void);
void* StyleStructPtr(int sn_index, StyleStruct *pStyle);

#define A_TEX (V_TEX|V_CO1|V_CO2|V_BOW|V_BOC)
#define A_FNT (V_FON|V_FHE|V_FWE|V_JUS)
#define I_DEF 0x10000
#define I_ACT 0x20000
#define I_BUL 0x40000

enum style_init_types
{
    C_INT = 1,
    C_BOL,
    C_STR,
    C_COL,
    C_STY,

    C_TEX,
    C_CO1,
    C_CO2,
    C_TXT,
    C_PIC,
    C_DIS,

    C_FON,
    C_FHE,
    C_FWE,
    C_JUS,

    C_MAR,
    C_BOC,
    C_BOW,

    C_SHT,
    C_SHP,
    C_SHE
};
#endif

//===========================================================================
#endif // _BBSETTINGS_H_
