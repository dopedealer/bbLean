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

#include <BBApi.h>
#include <bbstyle.h>

#include "Stylestruct.h"

namespace bbcore {

// forward definitions
struct items;

bool isUsingUtf8Encoding(void);
void* getSettingPtr(int sn_index);
int readStyleItem(
    const char* fileName,
    const char* szKey,
    StyleItem* pStyleItemOut,
    StyleItem* pStyleItemDefault
    );

HFONT createStyleFont(StyleItem* pSI);

void getCustomMargin(RECT *pcm, int screen);


//===========================================================================
// Settings.cpp functions

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

struct rccfg
{
    const char *key; char mode; const void *p_default; const void *ptr; 
};

class ISettings
{
    public:
        virtual ~ISettings(){};
        virtual void readStyleSettings(void) = 0;
        virtual void readStylePersonal(const char *style, StyleStruct *pStyle) = 0;
        virtual const char* checkGlobalFont(const char *p, const char *fullkey) = 0;

        virtual int readStyleItem(const char* fileName, const char* szKey,
                StyleItem* pStyleItemOut, StyleItem* pStyleItemDefault) = 0;

        virtual int readStyleItemEx(const char *style, StyleStruct *pStyle,
                const char *key, StyleItem *si, StyleItem *si_def,
                int sn, int f, bool is_070) = 0; 

        virtual void readStyle(const char *style, StyleStruct *pStyle) = 0; 

        virtual const char * makeKey(char *buff, const rccfg* cp) = 0;

        virtual void readSettings(const char *bbrc, const rccfg* cp) = 0;
        virtual void readRCSettings(void) = 0; 

        virtual bool writeSetting(const char *bbrc, const rccfg* cp, const void *v) = 0; 
        virtual void writeRCSetting(const void *v) = 0;

        virtual HFONT createStyleFont(StyleItem *pSI) = 0;
        virtual void parseFontNameToStyleItem(StyleItem *si, const char *font) = 0; 

        virtual const items* getStyleItems(void) = 0;

        // TODO: possibly all Settings_XXX functions to methods here
};

struct SettingsPars
{
    bool parsefontAfter;
    bool bbsm;              // bbstylemaker
    bool bbsettingNomenu;
    bool bbsettingNowindow;
}; 

/// \brief Lightweight one. Operates only on common props
class SettingsCommon : public ISettings
{
    public:
        SettingsCommon(void) = default;
        SettingsCommon(const SettingsPars &);

        void readStyleSettings(void) override {};
        void readStylePersonal(const char *style, StyleStruct *pStyle) override {}
        const char* checkGlobalFont(const char *p, const char *fullkey) override; 

        int readStyleItem(const char* fileName, const char* szKey,
                StyleItem* pStyleItemOut, StyleItem* pStyleItemDefault) override;
        int readStyleItemEx(const char *style, StyleStruct *pStyle,
                const char *key, StyleItem *si, StyleItem *si_def,
                int sn, int f, bool is_070) override;

        void readStyle(const char *style, StyleStruct *pStyle) override;

        const char * makeKey(char *buff, const rccfg* cp) override;
        void readSettings(const char *bbrc, const rccfg* cp) override;
        void readRCSettings(void) override {}

        bool writeSetting(const char *bbrc, const rccfg* cp, const void *v) override;
        void writeRCSetting(const void *v) override {};

        HFONT createStyleFont(StyleItem *pSI) override;
        void parseFontNameToStyleItem(StyleItem *si, const char *font) override;

        const items* getStyleItems(void) override;

    protected:
        SettingsPars initPars{};
};


extern menu_setting gSettingsMenu;

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
// Settings.cpp internal declarations 

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

enum other_defaults
{
    SN_DEFITEM_1 = SN_LAST,
    SN_DEFITEM_2,
    STR_EMPTY,
    STR_RIGHT,
    STR_TRIANGLE,
};

struct items
{
    short type;
    short sn;
    const char *rc_string;
    int sn_def;
    unsigned flags;
}; 

#define COMMON_STYLE_PROPS                                                                                          \
    /* bb4nix 065 style props --->> */                                                                              \
    { C_INT, SN_BORDERWIDTH         , "borderWidth",           0, 1 },                                              \
    { C_COL, SN_BORDERCOLOR         , "borderColor",           0, 0 },                                              \
    { C_INT, SN_BEVELWIDTH          , "bevelWidth",            0, 1 },                                              \
    { C_INT, SN_HANDLEHEIGHT        , "handleWidth",           0, 5 },                                              \
    /* -------------------------->> */                                                                              \
    /* The window frame from 065 is ignored since they are mostly set to random values in bb4win styles. */         \
    /*{ C_INT, SN_FRAMEWIDTH            , "frameWidth",                SN_BORDERWIDTH, 0 },*/                       \
    /*{ C_COL, SN_WINFOCUS_FRAME_COLOR  , "window.frame.focusColor",   SN_BORDERCOLOR, 0 },*/                       \
    /*{ C_COL, SN_WINUNFOCUS_FRAME_COLOR, "window.frame.unfocusColor", SN_BORDERCOLOR, 0 },*/                       \
    /*--------------------------<<\*/                                                                               \
    { C_STR, SN_ROOTCOMMAND         , "rootCommand",            STR_EMPTY       , sizeof mStyle.rootCommand },      \
    { C_STY, SN_TOOLBAR             , "toolbar",                SN_DEFITEM_1    , A_TEX|V_MAR|V_TXT|A_FNT|I_DEF },  \
    { C_STY, SN_TOOLBARLABEL        , "toolbar.label",          SN_DEFITEM_1    , A_TEX|V_MAR|V_TXT },              \
    { C_STY, SN_TOOLBARWINDOWLABEL  , "toolbar.windowLabel",    SN_DEFITEM_2    , A_TEX|V_TXT },                    \
    { C_STY, SN_TOOLBARCLOCK        , "toolbar.clock",          SN_DEFITEM_1    , A_TEX|V_TXT },                    \
    { C_STY, SN_TOOLBARBUTTON       , "toolbar.button",         SN_DEFITEM_2    , A_TEX|V_PIC|V_MAR },              \
    { C_STY, SN_TOOLBARBUTTONP      , "toolbar.button.pressed", SN_DEFITEM_1    , A_TEX|V_PIC }

#define MENU_STYLE_PROPS                                                                                                                \
    { C_STY, SN_MENUTITLE           , "menu.title",             SN_DEFITEM_2    , A_TEX|V_MAR|V_TXT|A_FNT|I_DEF },                      \
    { C_STY, SN_MENUFRAME           , "menu.frame",             SN_DEFITEM_1    , A_TEX|V_MAR|V_TXT|V_PIC|A_FNT|V_DIS|I_DEF|I_BUL },    \
    { C_STY, SN_MENUHILITE          , "menu.active",            SN_DEFITEM_2    , A_TEX|V_TXT|V_PIC|V_MAR|I_ACT|I_BUL },                \
    { C_STR, SN_MENUBULLET          , "menu.bullet",            STR_TRIANGLE    , sizeof mStyle.menuBullet  },                          \
    { C_STR, SN_MENUBULLETPOS       , "menu.bullet.position",   STR_RIGHT       , sizeof mStyle.menuBulletPosition  }

#define WINDOW_STYLE_PROPS                                                                              \
    { C_STY, SN_WINFOCUS_TITLE      , "window.title.focus",     SN_TOOLBAR      , A_TEX|I_DEF },        \
    { C_STY, SN_WINFOCUS_LABEL      , "window.label.focus",     SN_TOOLBARWINDOWLABEL, A_TEX|V_TXT },   \
    { C_STY, SN_WINFOCUS_HANDLE     , "window.handle.focus",    SN_TOOLBAR      , A_TEX|I_DEF },        \
    { C_STY, SN_WINFOCUS_GRIP       , "window.grip.focus",      SN_TOOLBARWINDOWLABEL, A_TEX|I_DEF },   \
    { C_STY, SN_WINFOCUS_BUTTON     , "window.button.focus",    SN_TOOLBARBUTTON, A_TEX|V_PIC },        \
    { C_STY, SN_WINFOCUS_BUTTONP    , "window.button.pressed",  SN_TOOLBARBUTTONP, A_TEX|V_PIC },       \
                                                                                                        \
    { C_STY, SN_WINUNFOCUS_TITLE    , "window.title.unfocus",   SN_TOOLBAR      , A_TEX|I_DEF },        \
    { C_STY, SN_WINUNFOCUS_LABEL    , "window.label.unfocus",   SN_TOOLBAR      , A_TEX|V_TXT },        \
    { C_STY, SN_WINUNFOCUS_HANDLE   , "window.handle.unfocus",  SN_TOOLBAR      , A_TEX|I_DEF },        \
    { C_STY, SN_WINUNFOCUS_GRIP     , "window.grip.unfocus",    SN_TOOLBARLABEL , A_TEX|I_DEF },        \
    { C_STY, SN_WINUNFOCUS_BUTTON   , "window.button.unfocus",  SN_TOOLBARBUTTON, A_TEX|V_PIC },        \
    /*-------------------------->>*/                                                                    \
    /*new bb4nix 070 style props*/                                                                      \
    { C_STY, SN_WINFOCUS_TITLE      , "window.title",           SN_TOOLBAR      , V_MAR|I_DEF },        \
    { C_STY, SN_WINFOCUS_LABEL      , "window.label",           SN_TOOLBARLABEL , V_MAR },              \
    { C_STY, SN_WINFOCUS_BUTTON     , "window.button",          SN_TOOLBARBUTTON, V_MAR },              \
                                                                                                        \
    { C_COL, SN_WINFOCUS_FRAME_COLOR, "window.frame.focus.borderColor",     SN_BORDERCOLOR },           \
    { C_COL, SN_WINUNFOCUS_FRAME_COLOR, "window.frame.unfocus.borderColor", SN_BORDERCOLOR },           \
    { C_INT, SN_FRAMEWIDTH          , "window.frame.borderWidth",           SN_BORDERWIDTH },           \
    { C_INT, SN_HANDLEHEIGHT        , "window.handleHeight",                SN_HANDLEHEIGHT, 0 },       \
    /*--------------------------<<*/                                                                    \
    /*window.font:*/                                                                                    \
    { C_STY, SN_WINFOCUS_LABEL      , "window",                 SN_TOOLBAR      , A_FNT } 

#define SLIT_STYLE_PROPS                                                                                \
    { C_STY, SN_SLIT                , "slit",                   SN_TOOLBAR      , A_TEX|V_MAR|I_DEF }

#define ENDING_STYLE_PROPS                                                                              \
    { 0,0,NULL,0,0 }

void* StyleStructPtr(int sn_index, StyleStruct *pStyle);

const rccfg* GetBboxRcConfig(void);
const rccfg* GetExtRcConfig(void);
int getweight(const char *fontWeightName);
int ParseJustify(const char *buff);



}; //!namespace bbcore

//===========================================================================
#endif // _BBSETTINGS_H_
