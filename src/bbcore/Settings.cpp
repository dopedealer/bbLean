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

#include "BB.h"
#include "bbrc.h" 
#include "Menu/MenuMaker.h"
#include "BImage.h" 
#include "Settings.h"


namespace bbcore {

/* *** Do not change this line. *** */
static_assert(sizeof(StyleItem) == 300, "The size of StyleItem struct must be strictly equal to 300 bytes!");

/* If you get an error here that means that BBApi.h was changed
   in an incompatible way */

// to be used in multimonitor setups (in the future, maybe ...)
int gScreenNumber = 0; 

static const rccfg extrc_cfg[] =
{ 
    { "blackbox.appearance.bullet.unix",       C_BOL, (void*)true,     &mStyle.bulletUnix },
    { "blackbox.appearance.arrow.unix",        C_BOL, (void*)false,    &Settings_arrowUnix },
    { "blackbox.appearance.cursor.usedefault", C_BOL, (void*)false,    &Settings_useDefCursor },

    { "blackbox.desktop.marginLeft",           C_INT, (void*)-1,       &Settings_desktopMargin.left },
    { "blackbox.desktop.marginRight",          C_INT, (void*)-1,       &Settings_desktopMargin.right },
    { "blackbox.desktop.marginTop",            C_INT, (void*)-1,       &Settings_desktopMargin.top },
    { "blackbox.desktop.marginBottom",         C_INT, (void*)-1,       &Settings_desktopMargin.bottom },

    { "blackbox.snap.toPlugins",               C_BOL, (void*)true,     &Settings_snapPlugins },
    { "blackbox.snap.padding",                 C_INT, (void*)2,        &Settings_snapPadding },
    { "blackbox.snap.threshold",               C_INT, (void*)7,        &Settings_snapThreshold },

    { "blackbox.background.enabled",           C_BOL, (void*)true,     &Settings_enableBackground  },
    { "blackbox.background.smartWallpaper",    C_BOL, (void*)true,     &Settings_smartWallpaper },

    { "blackbox.workspaces.followActive",      C_BOL, (void*)true,     &Settings_followActive },
    { "blackbox.workspaces.altMethod",         C_BOL, (void*)true,    &Settings_altMethod },
    { "blackbox.workspaces.styleXPFix",        C_BOL, (void*)false,    &Settings_styleXPFix },

    { "blackbox.options.disableTray",          C_BOL, (void*)false,    &Settings_disableTray },
    { "blackbox.options.disableDesk",          C_BOL, (void*)false,    &Settings_disableDesk },
    { "blackbox.options.disableMargins",       C_BOL, (void*)false,    &Settings_disableMargins },
    { "blackbox.options.disableVWM",           C_BOL, (void*)false,    &Settings_disableVWM },
    { "blackbox.options.disableDDE",           C_BOL, (void*)false,    &Settings_disableDDE },

    { "blackbox.options.desktopHook",          C_BOL, (void*)false,    &Settings_desktopHook },
    { "blackbox.options.hideExplorer",         C_BOL, (void*)true,     &Settings_hideExplorer  },
    { "blackbox.options.hideTaskbar",          C_BOL, (void*)true,     &Settings_hideExplorerTray },

    { "blackbox.options.shellContextMenu",     C_BOL, (void*)false,    &Settings_shellContextMenu },
    { "blackbox.options.UTF8Encoding",         C_BOL, (void*)false,    &Settings_UTF8Encoding },
    { "blackbox.options.OldTray",              C_BOL, (void*)false,    &Settings_OldTray },

    { "blackbox.global.fonts.enabled",         C_BOL, (void*)false,    &Settings_globalFonts },
    { "blackbox.editor",                       C_STR, (void*)"notepad.exe", Settings_preferredEditor },

    // --------------------------------

    { NULL, 0, NULL, NULL }
};

//===========================================================================
static const rccfg bbrc_cfg[] = {
    { "#toolbar.enabled",          C_BOL, (void*)true,     &Settings_toolbar.enabled },
    { "#toolbar.placement",        C_STR, (void*)"TopCenter", Settings_toolbar.placement },
    { "#toolbar.widthPercent",     C_INT, (void*)66,       &Settings_toolbar.widthPercent },
    { "#toolbar.onTop",            C_BOL, (void*)false,    &Settings_toolbar.onTop },
    { "#toolbar.autoHide",         C_BOL, (void*)false,    &Settings_toolbar.autoHide },
    { "#toolbar.pluginToggle",     C_BOL, (void*)true ,    &Settings_toolbar.pluginToggle },
    { "#toolbar.alpha.enabled",    C_BOL, (void*)false,    &Settings_toolbar.alphaEnabled },
    { "#toolbar.alpha.value",      C_INT, (void*)255,      &Settings_toolbar.alphaValue },

    { ".menu.position.x",          C_INT, (void*)100,      &gSettingsMenu.pos.x },
    { ".menu.position.y",          C_INT, (void*)100,      &gSettingsMenu.pos.y },
    { ".menu.maxWidth",            C_INT, (void*)240,      &gSettingsMenu.maxWidth },
    { ".menu.popupDelay",          C_INT, (void*)80,       &gSettingsMenu.popupDelay },
    { ".menu.mouseWheelFactor",    C_INT, (void*)3,        &gSettingsMenu.mouseWheelFactor },
    { ".menu.alpha.enabled",       C_BOL, (void*)false,    &gSettingsMenu.alphaEnabled },
    { ".menu.alpha.value",         C_INT, (void*)255,      &gSettingsMenu.alphaValue },
    { ".menu.onTop",               C_BOL, (void*)false,    &gSettingsMenu.onTop },
    { ".menu.sticky",              C_BOL, (void*)true,     &gSettingsMenu.sticky },
    { ".menu.snapWindow",          C_BOL, (void*)true,     &gSettingsMenu.snapWindow },
    { ".menu.pluginToggle",        C_BOL, (void*)true,     &gSettingsMenu.pluginToggle },
    { ".menu.openDirection",       C_STR, (void*)"right",  &gSettingsMenu.openDirection },
    { ".menu.sortbyExtension",     C_BOL, (void*)false,    &gSettingsMenu.sortByExtension },
    { ".menu.showHiddenFiles",     C_BOL, (void*)false,    &gSettingsMenu.showHiddenFiles },
    { ".menu.drawSeparators",      C_BOL, (void*)true,     &gSettingsMenu.drawSeparators },
    { ".menu.dropShadows",         C_BOL, (void*)false,    &gSettingsMenu.dropShadows },

    { "#workspaces",               C_INT, (void*)4,        &Settings_workspaces },
    { "#workspaceNames",           C_STR, (void*)"alpha,beta,gamma,delta", &Settings_workspaceNames },
    { "#strftimeFormat",           C_STR, (void*)"%I:%M %p", Settings_toolbar.strftimeFormat },
    { "#fullMaximization",         C_BOL, (void*)false,    &Settings_fullMaximization },
    { "#focusModel",               C_STR, (void*)"ClickToFocus", Settings_focusModel },

    { ".imageDither",              C_BOL, (void*)true,     &Settings_imageDither },
    { ".opaqueMove",               C_BOL, (void*)true,     &Settings_opaqueMove },
    { ".autoRaiseDelay",           C_INT, (void*)250,      &Settings_autoRaiseDelay },


    /* *nix settings, not used here
    // ----------------------------------
    { ".changeWorkspaceWithMouseWheel", C_BOL, (void*)false,    &Settings_desktopWheel   },
    { "#edgeSnapThreshold",        C_INT, (void*)7,        &Settings_snapThreshold },

    { "#focusLastWindow",          C_BOL, (void*)false,    &Settings_focusLastWindow },
    { "#focusNewWindows",          C_BOL, (void*)false,    &Settings_focusNewWindows },
    { "#windowPlacement",          C_STR, (void*)"RowSmartPlacement", &Settings_windowPlacement },
    { "#colPlacementDirection",    C_STR, (void*)"TopToBottom", &Settings_colPlacementDirection },
    { "#rowPlacementDirection",    C_STR, (void*)"LeftToRight", &Settings_rowPlacementDirection },

    { ".colorsPerChannel",         C_INT, (void*)4,        &Settings_colorsPerChannel },
    { ".doubleClickInterval",      C_INT, (void*)250,      &Settings_dblClickInterval },
    { ".cacheLife",                C_INT, (void*)5,        &Settings_cacheLife },
    { ".cacheMax",                 C_INT, (void*)200,      &Settings_cacheMax },
    // ---------------------------------- */

    { NULL,0,NULL,NULL }
};

//=====================================================
// Style definitions

StyleStruct mStyle;
toolbar_setting  Settings_toolbar;
menu_setting  gSettingsMenu;

//====================
// workspaces
bool Settings_styleXPFix;
bool Settings_followActive;
bool Settings_altMethod;
int  Settings_workspaces;
char Settings_workspaceNames[200];

//====================
// Plugin Snap
int Settings_snapThreshold;
int Settings_snapPadding;
bool Settings_snapPlugins;

//====================
// Desktop

// Margins
RECT Settings_desktopMargin;
bool Settings_fullMaximization;

// Background
bool Settings_enableBackground;
bool Settings_smartWallpaper;

// Options
bool Settings_desktopHook;
bool Settings_hideExplorer;
bool Settings_hideExplorerTray;

//====================
// Other

// window behaviour
bool Settings_opaqueMove;
char Settings_focusModel[40];
int  Settings_autoRaiseDelay;

// misc
char Settings_preferredEditor[MAX_PATH];
bool Settings_useDefCursor;
bool Settings_arrowUnix;
bool Settings_globalFonts;
bool Settings_imageDither;
bool Settings_shellContextMenu;
bool Settings_UTF8Encoding;
bool Settings_OldTray;
int Settings_contextMenuAdjust[2];
int Settings_LogFlag;

// feature select
bool Settings_disableTray;
bool Settings_disableDesk;
bool Settings_disableDDE;
bool Settings_disableVWM;
bool Settings_disableMargins;

//====================
// --- unused *nix settings ---
//bool Settings_focusLastWindow;
//bool Settings_focusNewWindows;
//char Settings_windowPlacement[40];
//char Settings_colPlacementDirection[40];
//char Settings_rowPlacementDirection[40];
//bool Settings_desktopWheel;

//===========================================================================

static const ShortStyleItem DefStyle_1 = {
    BEVEL_RAISED, BEVEL1, B_DIAGONAL, false, false,
    0xEEEEEE, 0xCCCCCC, 0x555555, 12, FW_NORMAL, DT_LEFT, 0, ""
};

static const ShortStyleItem DefStyle_2 = {
    BEVEL_RAISED, BEVEL1, B_VERTICAL, false, false,
    0xCCCCCC, 0xAAAAAA, 0x333333, 12, FW_NORMAL, DT_LEFT, 0, ""
};

static const void *other_items [] =
{
    &DefStyle_1,
    &DefStyle_2,
    "",
    "right",
    "triangle"
}; 

//===========================================================================
// check a font if it is available on the system

static int CALLBACK EnumFontFamProc(
    ENUMLOGFONT FAR *lpelf,     // pointer to logical-font data
    NEWTEXTMETRIC FAR *lpntm,   // pointer to physical-font data
    int FontType,               // type of font
    LPARAM lParam               // address of application-defined data
   )
{
    (*(int*)lParam)++;
    return 0;
}

int checkfont(const char *face)
{
    int data = 0;
    HDC hdc = CreateCompatibleDC(NULL);
    EnumFontFamilies(hdc, face, (FONTENUMPROC)EnumFontFamProc, (LPARAM)&data);
    DeleteDC(hdc);
    return data;
}

//===========================================================================
// convert weight into windows FW_XXX value
int getweight(const char *fontWeightName)
{
    static const char * const fontweightstrings[] =
    {
        "thin", "extralight", "light", "normal",
        "medium", "demibold", "bold", "extrabold",
        "heavy", "regular", "semibold", NULL
    };

    int i = 1 + get_string_index(fontWeightName, fontweightstrings);
    if (i>=10)
    {
        if (i==10) i = 4; // regular -> normal
        if (i==11) i = 6; // semibold -> demibold
    }
    return i*100;
}

static int tokenize_fontstring(char *buffer, char **pp, const char *src, int n, const char *delims)
{
    const char *a, *s = src; int i = 0, r = 0, l;
    while (i < n) {
        l = nexttoken(&a, &s, delims);
        *pp++ = extract_string(buffer, a, l);
        buffer += l+1, i++, r += 0 != l;
    }
    return r;
} 

//===========================================================================
void* StyleStructPtr(int sn_index, StyleStruct *pStyle)
{
    switch (sn_index) {
    case SN_STYLESTRUCT             : return pStyle;

    case SN_TOOLBAR                 : return &pStyle->Toolbar                ;
    case SN_TOOLBARBUTTON           : return &pStyle->ToolbarButton          ;
    case SN_TOOLBARBUTTONP          : return &pStyle->ToolbarButtonPressed   ;
    case SN_TOOLBARLABEL            : return &pStyle->ToolbarLabel           ;
    case SN_TOOLBARWINDOWLABEL      : return &pStyle->ToolbarWindowLabel     ;
    case SN_TOOLBARCLOCK            : return &pStyle->ToolbarClock           ;

    case SN_MENUTITLE               : return &pStyle->MenuTitle              ;
    case SN_MENUFRAME               : return &pStyle->MenuFrame              ;
    case SN_MENUHILITE              : return &pStyle->MenuHilite             ;

    case SN_MENUBULLET              : return &pStyle->menuBullet             ;
    case SN_MENUBULLETPOS           : return &pStyle->menuBulletPosition     ;

    case SN_BORDERWIDTH             : return &pStyle->borderWidth            ;
    case SN_BORDERCOLOR             : return &pStyle->borderColor            ;
    case SN_BEVELWIDTH              : return &pStyle->bevelWidth             ;
    case SN_FRAMEWIDTH              : return &pStyle->frameWidth             ;
    case SN_HANDLEHEIGHT            : return &pStyle->handleHeight           ;
    case SN_ROOTCOMMAND             : return &pStyle->rootCommand            ;
    case SN_MENUALPHA               : return &pStyle->menuAlpha              ;
    case SN_TOOLBARALPHA            : return &pStyle->toolbarAlpha           ;
    case SN_METRICSUNIX             : return &pStyle->metricsUnix            ;
    case SN_BULLETUNIX              : return &pStyle->bulletUnix             ;

    case SN_WINFOCUS_TITLE          : return &pStyle->windowTitleFocus       ;
    case SN_WINFOCUS_LABEL          : return &pStyle->windowLabelFocus       ;
    case SN_WINFOCUS_HANDLE         : return &pStyle->windowHandleFocus      ;
    case SN_WINFOCUS_GRIP           : return &pStyle->windowGripFocus        ;
    case SN_WINFOCUS_BUTTON         : return &pStyle->windowButtonFocus      ;
    case SN_WINFOCUS_BUTTONP        : return &pStyle->windowButtonPressed    ;
    case SN_WINUNFOCUS_TITLE        : return &pStyle->windowTitleUnfocus     ;
    case SN_WINUNFOCUS_LABEL        : return &pStyle->windowLabelUnfocus     ;
    case SN_WINUNFOCUS_HANDLE       : return &pStyle->windowHandleUnfocus    ;
    case SN_WINUNFOCUS_GRIP         : return &pStyle->windowGripUnfocus      ;
    case SN_WINUNFOCUS_BUTTON       : return &pStyle->windowButtonUnfocus    ;

    case SN_WINFOCUS_FRAME_COLOR    : return &pStyle->windowFrameFocusColor  ;
    case SN_WINUNFOCUS_FRAME_COLOR  : return &pStyle->windowFrameUnfocusColor;

    case SN_ISSTYLE070              : return &pStyle->is_070             ;
    case SN_SLIT                    : return &pStyle->Slit;

    default                         : return NULL;
    }
}

//===========================================================================
int Settings_ItemSize(int sn_index)
{
    switch (sn_index) {
    case SN_STYLESTRUCT             : return sizeof (StyleStruct);

    case SN_TOOLBAR                 :
    case SN_TOOLBARBUTTON           :
    case SN_TOOLBARBUTTONP          :
    case SN_TOOLBARLABEL            :
    case SN_TOOLBARWINDOWLABEL      :
    case SN_TOOLBARCLOCK            :

    case SN_MENUTITLE               :
    case SN_MENUFRAME               :
    case SN_MENUHILITE              : return sizeof (StyleItem);

    case SN_MENUBULLET              :
    case SN_MENUBULLETPOS           : return -1; // string, have to take strlen

    case SN_BORDERWIDTH             : return sizeof (int);
    case SN_BORDERCOLOR             : return sizeof (COLORREF);
    case SN_BEVELWIDTH              : return sizeof (int);
    case SN_FRAMEWIDTH              : return sizeof (int);
    case SN_HANDLEHEIGHT            : return sizeof (int);
    case SN_ROOTCOMMAND             : return -1; // string, have to take strlen

    case SN_MENUALPHA               : return sizeof (((StyleStruct*)0)->menuAlpha);
    case SN_TOOLBARALPHA            : return sizeof (((StyleStruct*)0)->toolbarAlpha);
    case SN_METRICSUNIX             :
    case SN_BULLETUNIX              : return sizeof (bool);

    case SN_WINFOCUS_TITLE          :
    case SN_WINFOCUS_LABEL          :
    case SN_WINFOCUS_HANDLE         :
    case SN_WINFOCUS_GRIP           :
    case SN_WINFOCUS_BUTTON         :
    case SN_WINFOCUS_BUTTONP        :
    case SN_WINUNFOCUS_TITLE        :
    case SN_WINUNFOCUS_LABEL        :
    case SN_WINUNFOCUS_HANDLE       :
    case SN_WINUNFOCUS_GRIP         :
    case SN_WINUNFOCUS_BUTTON       : return sizeof (StyleItem);

    case SN_WINFOCUS_FRAME_COLOR    :
    case SN_WINUNFOCUS_FRAME_COLOR  : return sizeof (COLORREF);

    case SN_ISSTYLE070              : return sizeof (bool);
    case SN_SLIT                    : return sizeof (StyleItem);

    default                         : return 0;
    }
}

//===========================================================================

/// \brief Parses string key to text format flag
/// \return If string contains 'center' or 'right' then returns related flag.
///         Otherwise returns 'left' flag
int ParseJustify(const char *buff)
{
    if (stristr(buff, "center"))
        return DT_CENTER;
    if (stristr(buff, "right"))
        return DT_RIGHT;
    return DT_LEFT;
} 

//=========================================================================== 


const rccfg* GetBboxRcConfig(void)
{
    return (const rccfg*)&bbrc_cfg;
}

const rccfg* GetExtRcConfig(void)
{
    return (const rccfg*)&extrc_cfg;
}

//===========================================================================


const char* SettingsCommon::checkGlobalFont(const char *p, const char *fullkey)
{
    return p;
}

SettingsCommon::SettingsCommon(const SettingsPars & pars)
    : initPars(pars)
{}

int SettingsCommon::readStyleItemEx(const char *style, StyleStruct *pStyle,
        const char *key, StyleItem *si, StyleItem *si_def,
        int sn, int f, bool is_070)
{
    static const struct s_prop
    {
        const char *k; const char *o; short mode; short f;
    }
    sPropValues[] =
    {
        // texture type
        { ".appearance",      ""                , C_TEX , V_TEX },
        // colors, from, to, text, pics
        { ".color1",          ".color"          , C_CO1 , V_CO1 },
        { ".color2",          ".colorTo"        , C_CO2 , V_CO2 },
        { ".textColor",       NULL              , C_TXT , V_TXT },
        { ".foregroundColor", ".picColor"       , C_PIC , V_PIC },
        { ".disabledColor",   ".disableColor"   , C_DIS , V_DIS },
        // font settings
        { ".font",            NULL              , C_FON , V_FON },
        { ".fontHeight",      NULL              , C_FHE , V_FHE },
        { ".fontWeight",      NULL              , C_FWE , V_FWE },
        { ".alignment",       ".justify"        , C_JUS , V_JUS },
        // borders & margins - _new in BBNix 0.70
        { ".borderWidth",     NULL              , C_BOW , V_BOW },
        { ".borderColor",     NULL              , C_BOC , V_BOC },
        { ".marginWidth",     NULL              , C_MAR , V_MAR },
    };

    const s_prop* cp = sPropValues;
    COLORREF cr;
    int w, trans;
    const char *p;
    char fullkey[100];
    char *lastword;

    trans = set_translate_065(false);

restart:
    w = strlen(key);
    lastword = w + (char*)memcpy(fullkey, key, w);
    si->nVersion = 4;
    do
    {
        if (cp->f & f)
        {
            if (C_CO1 == cp->mode
             && si->type == B_SOLID
             && false == si->interlaced)
                strcpy(lastword, ".backgroundColor");
            else
                strcpy(lastword, cp->k);

            p = read_value(style, fullkey, NULL);
            if (NULL == p && cp->o) {
                // try 0.65 key
                strcpy(lastword, cp->o);
                p = read_value(style, fullkey, NULL);
            }

            switch (cp->mode) {
            // --- textture ---
            case C_TEX:
                if (p)
                {
                    parse_item(p, si);
                    si->bordered = NULL != stristr(p, "border");
                    if (sn == SN_MENUTITLE)
                    {
                        if (false == si->parentRelative && stristr(p, "label"))
                            pStyle->menuTitleLabel = true;
                        if (stristr(p, "hidden"))
                            si->parentRelative = pStyle->menuNoTitle = true;
                    }
                }
                else
                {
                    if (f & I_ACT && false == is_070)
                    {
                        key = "menu.hilite";
                        f &= ~I_ACT;
                        goto restart;
                    }
                    memcpy(si, si_def, offsetof(StyleItem, Color));
                }
                break;

            // --- colors ---
            case C_CO1:
                cr = ReadColorFromString(p);
                if (CLR_INVALID == cr)
                    cr = si_def->Color;
                si->Color = cr;
                break;

            case C_CO2:
                cr = ReadColorFromString(p);
                if (CLR_INVALID == cr)
                    cr = si_def->ColorTo;
                si->ColorTo = cr;
                break;

            case C_TXT:
                cr = ReadColorFromString(p);
                if (CLR_INVALID == cr)
                    cr = si_def ->TextColor;
                si->TextColor = cr;
                break;

            case C_DIS:
                cr = ReadColorFromString(p);
                if (CLR_INVALID == cr)
                    cr = get_mixed_color(si);
                si->disabledColor = cr;
                break;

            case C_PIC:
                if (NULL == p && false == is_070 && (f & I_BUL)) {
                    strcpy(lastword, ".bulletColor"); // xoblite menu bullets
                    p = read_value(style, fullkey, NULL);
                }

                cr = ReadColorFromString(p);
                if (f & V_TXT) {
                    if (CLR_INVALID == cr)
                        cr = si->picColor;
                    si->foregroundColor = cr;
                } else {
                    if (CLR_INVALID == cr)
                        cr = si_def->picColor;
                    si->picColor = cr;
                }
                break;

            // --- Border ---
            case C_BOC:
                cr = ReadColorFromString(p);
                if (CLR_INVALID == cr)
                    cr = pStyle->borderColor;
                si->borderColor = cr;
                break;

            case C_BOW:
                // for backwards compatibility:
                if (false == is_070 && (f & I_DEF) && (p || pStyle->borderWidth))
                    si->bordered = true;

                // otherwise borderWidth is only valid with 'border' in texture
                if (false == si->bordered)
                    continue;

                si->borderWidth = p ? atoi(p) : pStyle->borderWidth;
                break;

            // --- Font ---
            case C_FON: // fontFace
                p = checkGlobalFont(p, fullkey);
                strcpy(si->Font, p ? p : si_def->Font);
                break;

            case C_FHE: // fontHeight
                p = checkGlobalFont(p, fullkey);
                si->FontHeight = p ? atoi(p) : si_def->FontHeight;
                break;

            case C_FWE: // fontWeight
                p = checkGlobalFont(p, fullkey);
                si->FontWeight = p && 0 != (w = getweight(p)) ? w : FW_NORMAL;
                if (!initPars.parsefontAfter && !initPars.bbsm)
                {
                    parseFontNameToStyleItem(si, si->Font);
                } 
                break;

            // --- Alignment ---
            case C_JUS:
                si->Justify = p ? ParseJustify(p) : si_def->Justify;
                break;

            // --- Margins ---
            case C_MAR:
                if (p) {
                    if (sn != SN_MENUHILITE || found_last_value() == 1) {
                        si->marginWidth = atoi(p);
                        break;
                    }}

                // --- default margins, a sensible issue ---
                switch (sn) {
                case SN_MENUTITLE:
                    if (is_070)
                        si->marginWidth = 2;
                    else
                        si->marginWidth = pStyle->bevelWidth + 1;
                    break;
                case SN_MENUFRAME:
                    if (is_070)
                        si->marginWidth = 1;
                    else
                    if (BEVEL_SUNKEN == si->bevelstyle
                        || BEVEL2 == si->bevelposition)
                        si->marginWidth = si->bevelposition;
                    else
                    if (pStyle->MenuHilite.borderWidth)
                        si->marginWidth = 1;
                    else
                        si->marginWidth = 0;
                    break;
                case SN_MENUHILITE:
                    if (is_070)
                        si->marginWidth = 2;
                    else
                        si->marginWidth = pStyle->bevelWidth+2;
                    break;
                case SN_TOOLBARBUTTON:
                    si->marginWidth = imax(0,
                        pStyle->ToolbarLabel.marginWidth - (false == is_070));
                    break;
                case SN_WINFOCUS_BUTTON:
                    si->marginWidth = imax(0,
                        pStyle->windowLabelFocus.marginWidth - (false == is_070));
                    break;
                default:
                    if (is_070 || 0 == (f & I_DEF))
                        si->marginWidth = 2;
                    else
                        si->marginWidth = pStyle->bevelWidth;
                    break;
                }
                break;
            }
            if (p)
                si->validated |= cp->f;
        }
    }
    while ((char*)++cp < (char*)sPropValues + sizeof sPropValues);

    set_translate_065(trans);
    return 0 != (si->validated & f);
}

void SettingsCommon::readStyle(const char *style, StyleStruct *pStyle)
{
    const items* s;
    StyleItem *T, *F;
    bool bu = pStyle->bulletUnix;
    bool valid_bevelWidth;
    bool valid_borderWidth;
    bool valid_borderColor;

    valid_bevelWidth =
    valid_borderWidth =
    valid_borderColor = false;

    memset(pStyle, 0, sizeof *pStyle);
    pStyle->bulletUnix = bu;
    pStyle->metricsUnix = true;
    pStyle->is_070 = 0 != get_070(style);

    readStylePersonal(style, pStyle);

    s = getStyleItems();
    do {
        const void *p_default;
        const char *p;
        COLORREF cr;
        void *v;
        int sn, dn, maxlen, type, trans;

        sn = s->sn;
        dn = s->sn_def;
        type = s->type;
        v = StyleStructPtr(sn, pStyle);

        if (dn == 0)
            p_default = &s->flags;
        else if (dn < SN_LAST)
            p_default = StyleStructPtr(dn, pStyle);
        else
            p_default = other_items[dn - SN_LAST];

        if (C_STY == type)
        {
            readStyleItemEx(
                style,
                pStyle,
                s->rc_string,
                (StyleItem*)v,
                (StyleItem*)p_default,
                sn,
                s->flags,
                pStyle->is_070
                );
            continue;
        }

        trans = set_translate_065(false);
        p = read_value(style, s->rc_string, NULL);
        set_translate_065(trans);

        if (p) switch (sn)
        {
            case SN_BORDERWIDTH:
                valid_borderWidth = true;
                break;
            case SN_BEVELWIDTH:
                valid_bevelWidth = true;
                break;
            case SN_BORDERCOLOR:
                valid_borderColor = true;
                break;
        }

        switch (type)
        {
            case C_INT:
                *(int*)v = p ? atoi(p) : *(int*)p_default;
                break;

            case C_STR:
                maxlen = s->flags;
                strcpy_max((char*)v, p ? p : (const char *)p_default, maxlen);
                break;

            case C_COL:
                cr = ReadColorFromString(p);
                if (CLR_INVALID == cr)
                    cr = *(COLORREF*)p_default;
                *(COLORREF*)v = cr;
                break;
        }

    } while ((++s)->sn);

    // ----------------------------------------------------
    // set some defaults for missing style settings
    if (pStyle->Toolbar.validated & V_TXT)
    {
        if (0==(pStyle->ToolbarLabel.validated & V_TXT))
            pStyle->ToolbarLabel.TextColor = pStyle->Toolbar.TextColor;

        if (0==(pStyle->ToolbarClock.validated & V_TXT))
            pStyle->ToolbarClock.TextColor = pStyle->Toolbar.TextColor;

        if (0==(pStyle->ToolbarWindowLabel.validated & V_TXT))
            pStyle->ToolbarWindowLabel.TextColor = pStyle->Toolbar.TextColor;
    }
    else
    {
        if (pStyle->ToolbarLabel.parentRelative)
            pStyle->Toolbar.TextColor = get_mixed_color(&pStyle->ToolbarLabel);
        else
        if (pStyle->ToolbarClock.parentRelative)
            pStyle->Toolbar.TextColor = get_mixed_color(&pStyle->ToolbarClock);
        else
        if (pStyle->ToolbarWindowLabel.parentRelative)
            pStyle->Toolbar.TextColor = get_mixed_color(&pStyle->ToolbarWindowLabel);
        else
            pStyle->Toolbar.TextColor = get_mixed_color(&pStyle->ToolbarLabel);
    }

    if (0==(pStyle->ToolbarButtonPressed.validated & V_PIC))
        pStyle->ToolbarButtonPressed.picColor = pStyle->ToolbarButton.picColor;

    if (0==(pStyle->Toolbar.validated & V_JUS))
        pStyle->Toolbar.Justify = DT_CENTER;

    if (0==(pStyle->ToolbarWindowLabel.validated & V_TEX))
        if (pStyle->ToolbarLabel.validated & V_TEX)
            pStyle->ToolbarWindowLabel = pStyle->ToolbarLabel;

    // Set a light grey background for no style at all
    if (0 == (pStyle->MenuFrame.validated & V_TEX))
    {
        if (0 == pStyle->rootCommand[0])
            strcpy(pStyle->rootCommand, "bsetroot -mod 4 4 -fg grey55 -bg grey60");
    }

    // menuTitle font defaults to menuFrame
    T = &pStyle->MenuTitle;
    F = &pStyle->MenuFrame;
    if (0 == (T->validated & V_FON))
    {
        strcpy(T->Font, F->Font);
        if (0 == (T->validated & V_FHE))
            T->FontHeight = F->FontHeight;
        if (0 == (T->validated & V_FWE))
            T->FontWeight = F->FontWeight;
    }

    if (pStyle->is_070)
    {
        StyleItem *si;
        for (si = &pStyle->windowTitleFocus; si <= &pStyle->windowButtonUnfocus; ++si)
        {
            if (0 == (si->validated & V_BOC))
            {
                if (si >= &pStyle->windowTitleUnfocus)
                    si->borderColor = pStyle->windowFrameUnfocusColor;
                else
                    si->borderColor = pStyle->windowFrameFocusColor;
            }
        }

        // setup some default border/bevel to satisfy old plugins from new styles
        if (false == valid_bevelWidth)
            pStyle->bevelWidth = pStyle->Toolbar.marginWidth;
        if (false == valid_borderWidth)
            pStyle->borderWidth = pStyle->Toolbar.borderWidth;
        if (false == valid_borderColor)
            pStyle->borderColor = pStyle->Toolbar.borderColor;
    }
}

int SettingsCommon::readStyleItem(const char* fileName, const char* szKey,
        StyleItem* pStyleItemOut, StyleItem* pStyleItemDefault)
{
    int ret;
    int f = A_TEX | A_FNT|V_TXT|V_DIS | V_PIC | V_MAR;

    if (nullptr == pStyleItemDefault)
    {
        pStyleItemDefault = (StyleItem*)&DefStyle_1;
    }

    memset(pStyleItemOut, 0, sizeof *pStyleItemOut);
    ret = readStyleItemEx(
        fileName,
        &mStyle,
        szKey,
        pStyleItemOut,
        pStyleItemDefault,
        0,
        f,
        0 != get_070(fileName)
        );
    return ret;
}

const char * SettingsCommon::makeKey(char *buff, const rccfg* cp)
{
    const char *k = cp->key;
    if (k[0]=='.')
        sprintf(buff, "session%s", k);
    else
    if (k[0]=='#')
        sprintf(buff, "session.screen%d.%s", gScreenNumber, k+1);
    else
        return k;
    return buff;
}

void SettingsCommon::readSettings(const char *bbrc, const rccfg* cp)
{
    do {
        char keystr[100];
        const char *key = makeKey(keystr, cp);
        switch (cp->mode)
        {
            case C_INT:
                *(int*)cp->ptr = read_int(bbrc, key, (int)(DWORD_PTR)cp->p_default);
                break;
            case C_BOL:
                *(bool*)cp->ptr = read_bool(bbrc, key, 0 != (int)(DWORD_PTR)cp->p_default);
                break;
            case C_STR:
                strcpy((char*)cp->ptr, read_string(bbrc, key, (char*)cp->p_default));
                break;
        }
    } while ((++cp)->key);
}

bool SettingsCommon::writeSetting(const char *bbrc, const rccfg* cp, const void *v)
{
    do if (NULL == v || cp->ptr == v)
    {
        char keystr[100];
        const char *key = makeKey(keystr, cp);
        switch (cp->mode)
        {
            case C_INT:
                write_int(bbrc, key, *(int*) cp->ptr);
                break;
            case C_BOL:
                write_bool(bbrc, key, *(bool*) cp->ptr);
                break;
            case C_STR:
                write_string(bbrc, key, (char*) cp->ptr);
                break;
        }
        if (v) return true;
    } while ((++cp)->key);
    return false;
}

HFONT SettingsCommon::createStyleFont(StyleItem *pSI)
{
    if (initPars.parsefontAfter)
    {
        StyleItem SI = *pSI;
        parseFontNameToStyleItem(&SI, pSI->Font);
        pSI = &SI;
    }
    return CreateFont(
        pSI->FontHeight,
        0, 0, 0,
        pSI->FontWeight,
        false, false, false,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH|FF_DONTCARE,
        pSI->Font
        );
}

void SettingsCommon::parseFontNameToStyleItem(StyleItem *si, const char *font)
{
    static const char * const substlist[] = {
        "lucidatypewriter"  ,   "edges"         ,
        "fixed"             ,   "lucida console",
        "lucida"            ,   "lucida sans"   ,
        "helvetica"         ,   "tahoma"        ,
        "calisto mt"        ,   "verdana"       ,
        "8x8 system font"   ,   "edges"         ,
        NULL
    };

    char fontstring[100];
    char *p[16];
    char *b;
    const char * const *r;
    int i, w;
    bool subst = false;
    bool dblcheck = true;

    if ('-' == font[0])
    {
        /* --------------------------------------------------------
            parse linux font spec:
                -foundry-family-weight-slant-setwidth-addstyle
                -pixel-point-resx-resy-spacing-width-charset-encoding

            slant: "-r-", "-i-", "-o-", "-ri-", "-ro-"
           --------------------------------------------------------
        */
        enum {
            f_foundry, f_family, f_weight, f_slant, f_setwidth, f_addstyle,
            f_pixel, f_point, f_resx, f_resy, f_spacing, f_width, f_charset,
            f_encoding, f_last
        };

        tokenize_fontstring(fontstring, p, font+1, f_last, "-");

        if (*(b = p[f_family]) && *b != '*') {
            strcpy(si->Font, b);
            subst = true;
        }

        if (*(b = p[f_weight])) {
            si->FontWeight = 0 != (w = getweight(b)) ? w : FW_NORMAL;
        }

        if (is_digit(*(b = p[f_pixel]))) { // 'pixel'
            si->FontHeight = atoi(b);
        }
        else
        if (is_digit(*(b = p[f_point]))) {// 'point'
            si->FontHeight = atoi(b) / 10;
        }
    } else if (checkfont(font)) {
        // if the font exists, we take it as is
        strcpy(si->Font, font);
        dblcheck = false;
    } else {
        tokenize_fontstring(fontstring, p, font, 3,
            strchr(font, '/') ? "/" :
            strchr(font, ',') ? "," : "-");
        for (i = 0; i < 3 && *(b = p[i]); ++i) {
            if (0 == i) {
                strcpy(si->Font, b);
                subst = true;
            } else if (is_digit(*b)) {
                si->FontHeight = atoi(b);
            } else if (0 != (w = getweight(b))) {
                si->FontWeight = w;
            } else if (stristr(b,"bold")) {
                si->FontWeight = FW_BOLD;
            }
        }
    }

    if (subst)
        for (r = substlist; r[0]; r+=2)
            if (0 == stricmp(si->Font, r[0])) {
                if (stristr(si->Font, "bold"))
                    si->FontWeight = FW_BOLD;
                strcpy(si->Font, r[1]);
                break;
            }

    //debug_printf("%s -> <%s> %d %d", font, si->Font, si->FontHeight, si->FontWeight);
    if (dblcheck && 0 == checkfont(si->Font)) {
        LOGFONT logFont;
        SystemParametersInfo(SPI_GETICONTITLELOGFONT, 0, &logFont, 0);
        strcpy(si->Font, logFont.lfFaceName);
        //si->FontHeight = logFont.lfHeight;
        {
            HFONT hf = CreateFontIndirect(&logFont);
            si->FontHeight = get_fontheight(hf);
            DeleteObject(hf);
        }
    }
}

const items* SettingsCommon::getStyleItems(void)
{
    // TODO: it is an ugly solution and need to be refactored. It is done to
    // make possible further changes with retaining current behaviour and not
    // changing existing logic. The related bool flags appeared as a result of
    // migrating from very dirty macros usage

    // NOMENU, NOWINDOW
    // {0, 0}
    // {0, 1}
    // {1, 0}
    // {1, 1} 

    // {0, 0}
    static const items allStyleItems[] =
    {
        COMMON_STYLE_PROPS,
        MENU_STYLE_PROPS,
        WINDOW_STYLE_PROPS,
        SLIT_STYLE_PROPS,
        ENDING_STYLE_PROPS
    }; 
    if (initPars.bbsettingNomenu == false && initPars.bbsettingNowindow == false)
    {
        return allStyleItems;
    }

    // {0, 1}
    static const items styleItemsNoWindow[] =
    {
        COMMON_STYLE_PROPS,
        MENU_STYLE_PROPS,
        SLIT_STYLE_PROPS,
        ENDING_STYLE_PROPS
    };
    if (initPars.bbsettingNomenu == false && initPars.bbsettingNowindow == true)
    {
        return styleItemsNoWindow;
    }

    // {1, 0}
    static const items styleItemsNoMenu[] =
    {
        COMMON_STYLE_PROPS,
        WINDOW_STYLE_PROPS,
        SLIT_STYLE_PROPS,
        ENDING_STYLE_PROPS
    }; 
    if (initPars.bbsettingNomenu == true && initPars.bbsettingNowindow == false)
    {
        return styleItemsNoMenu;
    }

    // {1, 1}
    static const items styleItemsNoMenuNoWindow[] =
    {
        COMMON_STYLE_PROPS,
        SLIT_STYLE_PROPS,
        ENDING_STYLE_PROPS
    }; 
    if (initPars.bbsettingNomenu == true && initPars.bbsettingNowindow == true)
    {
        return styleItemsNoMenuNoWindow;
    }

    return {};
}


}; //!namespace bbcore
