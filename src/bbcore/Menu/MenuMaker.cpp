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
#include "Settings.h"
#include "bbshell.h"
#include "MenuMaker.h"
#include "Menu.h"
#include "Blackbox.h"
#include "rcfile.h"
#include "nls.h"
#include "system.h"
#include "SpecialFolderItem.h"
#include "Workspaces.h"
#include "ConfigMenu.h"
#include "DesktopMenu.h"


namespace bbcore {

Menu* MenuEnum(MENUENUMPROC fn, void* ud);

/// \brief Create or refresh a Menu
/// Creates a Menu or Submenu, Id must be unique, fshow indicates whether
/// the menu should be shown (true) or redrawn (false).
/// \param HeaderText  the menu title
/// \param IDString    An unique string that identifies the menu window
/// \param popup       true: menu is to be shown, false: menu is to be refreshed
/// \return A pointer to a Menu structure (opaque for the client)
Menu* makeNamedMenu(const char* HeaderText, const char* IDString, bool popup)
{
    Menu *pMenu{};

    if (IDString != nullptr)
    {
        pMenu = Menu::find_named_menu(IDString);
    }

    if (pMenu)
    {
        pMenu->incref();
        pMenu->SaveState();
        pMenu->DeleteMenuItems();
        if (HeaderText)
        {
            replace_str(&pMenu->getMenuItems()->m_pszTitle, NLS1(HeaderText));
        }
    } else
    {
        pMenu = new Menu(NLS1(HeaderText));
        pMenu->data.m_IDString = new_str(IDString);
    }
    pMenu->data.m_bPopup = popup;
    //debug_printf("makeNamedMenu (%d) %x %s <%s>", popup, pMenu, HeaderText, IDString);
    return pMenu;
}


// when no menu.rc file is found, use this default menu
static const char default_root_menu[] =
    "[begin]"
    "\n[path](Programs){PROGRAMS|COMMON_PROGRAMS}"
    "\n[path](Desktop){DESKTOP}"
    "\n[submenu](Blackbox)"
    "\n" "[config](Configuration)"
    "\n" "[stylesmenu](Styles){styles}"
    "\n" "[submenu](Edit)"
    "\n"    "[editstyle](current style)"
    "\n"    "[editmenu](menu.rc)"
    "\n"    "[editplugins](plugins.rc)"
    "\n"    "[editblackbox](blackbox.rc)"
    "\n"    "[editextensions](extensions.rc)"
    "\n"    "[end]"
    "\n" "[about](About)"
    "\n" "[reconfig](Reconfigure)"
    "\n" "[restart](Restart)"
    "\n" "[exit](Quit)"
    "\n" "[end]"
    "\n[submenu](Goodbye)"
    "\n" "[logoff](Log Off)"
    "\n" "[reboot](Reboot)"
    "\n" "[shutdown](Shutdown)"
    "\n" "[end]"
    "\n[end]"
    ;

// Special menu commands
static const char * const menu_cmds[] = {

    // ------------------
    "begin"             ,
    "end"               ,
    "submenu"           ,
    "include"           ,
    "nop"               ,
    "sep"               ,
    "separator"         ,
    // ------------------
    "path"              ,
    "insertpath"        ,
    "stylesmenu"        ,
    "stylesdir"         ,

    "themesmenu"        ,
    // ------------------
    "workspaces"        ,
    "icons"             ,
    "tasks"             ,
    "config"            ,
    "exec"              ,
    // ------------------
    NULL
};

enum menu_cmd_tokens
{
    // ------------------
    e_begin             ,
    e_end               ,
    e_submenu           ,
    e_include           ,
    e_nop               ,
    e_sep1              ,
    e_sep2              ,
    // ------------------
    e_path              ,
    e_insertpath        ,
    e_stylesmenu        ,
    e_stylesdir         ,

    e_themesmenu        ,
    // ------------------
    e_workspaces        ,
    e_icons             ,
    e_tasks             ,
    e_config            ,
    e_exec              ,
    // ------------------
    e_no_end            ,
    e_other             = -1
};


// menu include file handling
#define MAXINCLUDELEVEL 10

struct menu_src {
    int level;
    const char *default_menu;
    bool popup;
    FILE *fp[MAXINCLUDELEVEL];
    char path[MAXINCLUDELEVEL][MAX_PATH];
};

static bool add_inc_level(menu_src* src, const char *path)
{
    FILE *fp;
    if (src->level >= MAXINCLUDELEVEL)
    {
        return false;
    }

    fp = fileOpen(path);
    if (NULL == fp)
    {
        return false;
    }

    src->fp[src->level] = fp;
    strcpy(src->path[src->level], path);
    ++src->level;
    return true;
}

static void dec_inc_level(menu_src* src)
{
    fileClose(src->fp[--src->level]);
}

// recursive parsing of menu file
static Menu* ParseMenu(menu_src* src, const char *title, const char *IDString)
{
    char line[4000];
    char data[4000];
    char buffer[4000];
    char command[40];
    char label[MAX_PATH];

    Menu *pMenu, *pSub;
    MenuItem *pItem;
    int f, e_cmd;
    const char *p_label, *p_data, *cp, *p_cmd;

    pMenu = NULL;
    for(;;)
    {
        p_label = NULL;
        p_data = data;

        if (0 == src->level)
        {
            // read default menu from string
            NextToken(line, &src->default_menu, "\n");
        }
        else
        {
            f = readNextCommand(src->fp[src->level-1], line, sizeof(line));
            if (!f) {
                if (src->level > 1) {
                    dec_inc_level(src);
                    continue; // continue from included file
                }
                e_cmd = e_no_end;
                goto skip;
            }
        }

        cp = replace_environment_strings(line, sizeof line);

        //debug_printf("Menu %08x line:%s", pMenu, line);

        // get the command
        if (false == get_string_within(command, sizeof command, &cp, "[]"))
            continue;
        // search the command
        e_cmd = get_string_index(command, menu_cmds);

        if (get_string_within(label, sizeof label, &cp, "()"))
            p_label = label;

        if (false == get_string_within(data, sizeof data, &cp, "{}"))
            p_data = label;

skip:
        if (NULL == pMenu)
        {
            if (e_begin == e_cmd)
            {
                // If the line contains [begin] we create the menu
                // If no menu title has been defined, display Blackbox version...
#ifdef BBXMENU
                if (src->default_menu)
                    strcpy(label, "bbXMenu");
                else
#endif
                if (0 == label[0] && src->default_menu)
                {
                    p_label = getBBVersion();
                }
                pMenu = makeNamedMenu(p_label, IDString, src->popup);
                continue;
            }

            if (NULL == title)
            {
                title = NLS0("missing [begin]");
            }

            pMenu = makeNamedMenu(title, IDString, src->popup);
        }

        pItem = NULL;

        switch (e_cmd) {

        //====================
        // [begin] is like [submenu] when within the menu
        case e_begin:
        case e_submenu:
            sprintf(buffer, "%s_%s", IDString, label);
            strlwr(buffer + strlen(IDString));
            pSub = ParseMenu(src, p_data, buffer);
            if (pSub)
                pItem = makeSubmenu(pMenu, pSub, label);
            else
                pItem = makeMenuNOP(pMenu, label);
            break;

        //====================
        case e_no_end:
            makeMenuNOP(pMenu, NLS0("missing [end]"));
        case e_end:
            menuOption(pMenu, BBMENU_ISDROPTARGET);
            return pMenu;

        //====================
        case e_include:
        {
            char dir[MAX_PATH];
            file_directory(dir, src->path[src->level-1]);
            replace_shellfolders_from_base(buffer, p_data, false, dir, bbDefaultrcPath(), isUsingUtf8Encoding());
            if (false == add_inc_level(src, buffer)) {
                replace_shellfolders(buffer, p_data, false, bbDefaultrcPath(), isUsingUtf8Encoding());
                if (false == add_inc_level(src, buffer))
                    makeMenuNOP(pMenu, NLS0("[include] failed"));
            }
            continue;
        }

        //====================
        // a [nop] item will insert an inactive item with optional text
        case e_nop:
            pItem = makeMenuNOP(pMenu, label);
            break;

        // a insert separator, we treat [sep] like [nop] with no label
        case e_sep1:
        case e_sep2:
            pItem = makeMenuNOP(pMenu, NULL);
            break;

        //====================
        // a [path] item is pointing to a dynamic folder...
        case e_path:
            p_cmd = get_special_command(&p_data, buffer, sizeof buffer);
            pItem = makeMenuItemPath(pMenu, label, p_data, p_cmd);
            break;

        // a [insertpath] item will insert items from a folder...
        case e_insertpath:
            p_cmd = get_special_command(&p_data, buffer, sizeof buffer);
            pItem = makeMenuItemPath(pMenu, NULL, p_data, p_cmd);
            break;

        // a [stylemenu] item is pointing to a dynamic style folder...
        case e_stylesmenu:
            pItem = makeMenuItemPath(pMenu, label, p_data, MM_STYLE_BROAM);
            break;

        // a [styledir] item will insert styles from a folder...
        case e_stylesdir:
            pItem = makeMenuItemPath(pMenu, NULL, p_data, MM_STYLE_BROAM);
            break;

        case e_themesmenu:
            pItem = makeMenuItemPath(pMenu, label, p_data, MM_THEME_BROAM);
            break;

        //====================
        // special items...
        case e_workspaces:
            pItem = makeSubmenu(pMenu, MakeDesktopMenu(0, src->popup), p_label);
            break;
        case e_icons:
            pItem = makeSubmenu(pMenu, MakeDesktopMenu(1, src->popup), p_label);
            break;
        case e_tasks:
            pItem = makeSubmenu(pMenu, MakeDesktopMenu(2, src->popup), p_label);
            break;
        case e_config:
            pItem = makeSubmenu(pMenu, MakeConfigMenu(src->popup), p_label);
            break;

        //====================
        case e_exec:
            if ('@' == data[0]) {
                pItem = makeMenuItem(pMenu, label, data, false);
                menuItemOption(pItem, BBMENUITEM_UPDCHECK);
            } else {
                goto core_broam;
            }
            break;

        //====================
        case e_other:
            f = get_workspace_number(command); // check for 'workspace1..'
            if (-1 != f) {
                pItem = makeSubmenu(pMenu, MakeTaskFolder(f, src->popup), p_label);
            } else {
                p_data = data;
                goto core_broam;
            }
            break;

        //====================
        // everything else is converted to a '@BBCore.xxx' broam
        core_broam:
            f = sprintf(buffer, "@bbCore.%s", command);
            strlwr(buffer+8);
            if (p_data[0])
                sprintf(buffer + f, " %s", p_data);
            pItem = makeMenuItem(pMenu, label[0]?label:command, buffer, false);
            break;
        }

#ifdef BBOPT_MENUICONS
        if (pItem && get_string_within(label,  sizeof label, &cp, "<>"))
            menuItemOption(pItem, BBMENUITEM_SETICON, label);
#endif
    }
}

// generate MenuID for core menus (root, config, workspaces)
char *Core_IDString(char *buffer, const char *menu_id)
{
    const char id[] = "Core_";
    memcpy(buffer, id, sizeof id-1);
    strlwr(strcpy(buffer + sizeof id-1, menu_id));
    return buffer;
}

// toplevel entry for menu parser
Menu * MakeRootMenu(const char *menu_id, const char *path, const char *default_menu, bool pop)
{
    Menu *m = NULL;
    char IDString[MAX_PATH];
    menu_src src;

    src.level = 0;
    src.default_menu = default_menu;
    src.popup = pop;

    if (false == add_inc_level(&src, path)) {
        if (NULL == default_menu)
            return m;
    }

    m = ParseMenu(&src, NULL, Core_IDString(IDString, menu_id));

    while (src.level)
        dec_inc_level(&src);
    return m;
}

// show one of the root menus
bool MenuMaker_ShowMenu(int id, const char* param)
{
    char buffer[MAX_PATH];
    Menu *m;
    int x, y, n, flags, toggle;

    static const char * const menu_string_ids[] = {
        "",
        "root",
        "workspaces",
        "icons",
        "tasks",
        "configuration",
        NULL
    };

    enum {
        e_lastmenu,
        e_root,
        e_workspaces,
        e_icons,
        e_tasks,
        e_configuration,
    };

    x = y = flags = n = toggle = 0;


    switch (id)
    {
        case BB_MENU_BROAM: // @showMenu ...
            while (param[0] == '-') {
                const char *p = NextToken(buffer, &param, NULL);
                if (0 == strcmp(p, "-at")) {
                    for (;;++param) {
                        if (*param == 'r')
                            flags |= BBMENU_XRIGHT;
                        else if (*param == 'b')
                            flags |= BBMENU_YBOTTOM;
                        else
                            break;
                    }
                    x = atoi(NextToken(buffer, &param, " ,"));
                    param += ',' == *param;
                    y = atoi(NextToken(buffer, &param, NULL));
                    flags |= BBMENU_XY;
                } else if (0 == strcmp(p, "-key")) {
                    flags |= BBMENU_KBD;
                } else if (0 == strcmp(p, "-toggle")) {
                    toggle = 1;
                } else if (0 == strcmp(p, "-pinned")) {
                    flags |= BBMENU_PINNED;
                } else if (0 == strcmp(p, "-ontop")) {
                    flags |= BBMENU_ONTOP;
                } else if (0 == strcmp(p, "-notitle")) {
                    flags |= BBMENU_NOTITLE;
                }
            }
            break;
        case BB_MENU_ROOT: // Main menu
            param = "root";
            break;
        case BB_MENU_TASKS: // Workspaces menu
            param = "workspaces";
            break;
        case BB_MENU_ICONS: // Iconized tasks menu
            param = "icons";
            break;
        case BB_MENU_UPDATE:
            Menu_Update(MENU_UPD_ROOT);
            Menu_All_Redraw(0);
            return false;
        case BB_MENU_SIGNAL: // just to signal e.g. BBSoundFX
            return true;
        default:
            return false;
    }

    // If invoked by kbd and the menu currently has focus,
    // hide it and return
    if (((flags & BBMENU_KBD) || toggle) && Menu_ToggleCheck(param))
        return false;

    //DWORD t1 = GetTickCount();

    switch (get_string_index(param, menu_string_ids))
    {
        case e_root:
        case e_lastmenu:
            m = MakeRootMenu("root", bbMenuPath(NULL), default_root_menu, true);
            break;

        case e_workspaces:
            m = MakeDesktopMenu(0, true);
            break;

        case e_icons:
            m = MakeDesktopMenu(1, true);
            break;

        case e_tasks:
            m = MakeDesktopMenu(2, true);
            break;

        case e_configuration:
            m = MakeConfigMenu(true);
            break;

        default:
            n = get_workspace_number(param); // e.g. 'workspace1'
            if (-1 != n)
            {
                m = MakeTaskFolder(n, true);
            }
            else if (findRCFile(buffer, param, NULL))
            {
                m = MakeRootMenu(param, buffer, NULL, true);
            }
            else
            {
                const char *cmd = get_special_command(&param, buffer, sizeof buffer);
                m = MakeFolderMenu(NULL, param, cmd);
            }
            break;
    }

    if (NULL == m)
        return false;

    menuOption(m, flags, x, y);
    showMenu(m);

    //debug_printf("showmenu time %d", GetTickCount() - t1);
    return true;
}

// update one of the core menus
void Menu_Update(int id)
{
    switch (id)
    {
        case MENU_UPD_ROOT:
            // right click menu or any of its submenus
            if (menuExists("Core_root"))
            {
                showMenu(MakeRootMenu("root", bbMenuPath(NULL),
                    default_root_menu, false));
                break;
            }
            // fall though
        case MENU_UPD_CONFIG:
            // the core config menu
            if (menuExists("Core_configuration"))
                showMenu(MakeConfigMenu(false));
            break;

        case MENU_UPD_TASKS:
            // desktop workspaces menu etc.
            if (menuExists("Core_tasks"))
            {
                if (menuExists("Core_tasks_workspace"))
                    showMenu(MakeDesktopMenu(0, false));
                else
                if (menuExists("Core_tasks_icons"))
                    showMenu(MakeDesktopMenu(1, false));

                if (menuExists("Core_tasks_menu"))
                    showMenu(MakeDesktopMenu(2, false));

                if (menuExists("Core_tasks_recoverwindows"))
                    showMenu(MakeRecoverMenu(false));
            }
            break;
    }
}


//===========================================================================
// The menu window class

void register_menuclass(void)
{
    int style = BBCS_VISIBLE|BBCS_EXTRA;
    if (gSettingsMenu.dropShadows)
        style |= BBCS_DROPSHADOW;
    BBRegisterClass(Menu::className(), Menu::WindowProc, style);
}

void un_register_menuclass(void)
{
    UnregisterClass(Menu::className(), hMainInstance);
}

//===========================================================================
//
//  Global Menu Interface
//
//===========================================================================
// Below are the global functions to do something with menus. No other menu
// functions should be called from outside

void Menu_Init(void)
{
    register_menuclass();
    Menu_Reconfigure();
}

void Menu_ResetFonts(void)
{
    if (gMenuInfo.hTitleFont)
        DeleteObject(gMenuInfo.hTitleFont);
    if (gMenuInfo.hFrameFont)
        DeleteObject(gMenuInfo.hFrameFont);
    gMenuInfo.hTitleFont =
    gMenuInfo.hFrameFont = NULL;
}

void Menu_Exit(void)
{
    MenuEnum(Menu::del_menu, NULL);
    Menu_ResetFonts();
    un_register_menuclass();
}


//===========================================================================


void Menu_Reconfigure(void)
{
    StyleItem *MTitle, *MFrame, *MHilite, *pSI, *pScrl;

    int tfh, titleHeight;
    int ffh, itemHeight;

    MTitle = &mStyle.MenuTitle;
    MFrame = &mStyle.MenuFrame;
    MHilite = &mStyle.MenuHilite;

    // create fonts
    Menu_ResetFonts();
    gMenuInfo.hTitleFont = createStyleFont(MTitle);
    gMenuInfo.hFrameFont = createStyleFont(MFrame);

    // set bullet position & style
    gMenuInfo.nBulletPosition = stristr(mStyle.menuBulletPosition, "left")
        ? FOLDER_LEFT : FOLDER_RIGHT;

    gMenuInfo.nBulletStyle = Menu::get_menu_bullet(mStyle.menuBullet);

    gMenuInfo.openLeft = 0 == stricmp(gSettingsMenu.openDirection, "bullet")
        ? gMenuInfo.nBulletPosition == FOLDER_LEFT
        : 0 == stricmp(gSettingsMenu.openDirection, "left")
        ;

    // --------------------------------------------------------------
    // calulate metrics:

    gMenuInfo.nFrameMargin = MFrame->marginWidth + MFrame->borderWidth;
    gMenuInfo.nSubmenuOverlap = gMenuInfo.nFrameMargin + MHilite->borderWidth;
    gMenuInfo.nTitleMargin = 0;

    if (mStyle.menuTitleLabel)
        gMenuInfo.nTitleMargin = MFrame->marginWidth;

    // --------------------------------------
    // title height, indent, margin

    tfh = get_fontheight(gMenuInfo.hTitleFont);
    titleHeight = 2*MTitle->marginWidth + tfh;

    // xxx old behaviour xxx
    if (false == mStyle.is_070 && 0 == (MTitle->validated & V_MAR))
        titleHeight = MTitle->FontHeight + 2*mStyle.bevelWidth;
    //xxxxxxxxxxxxxxxxxxxxxx

    pSI = MTitle->parentRelative ? MFrame : MTitle;
    gMenuInfo.nTitleHeight = titleHeight + MTitle->borderWidth + MFrame->borderWidth;
    gMenuInfo.nTitleIndent = imax(imax(2 + pSI->bevelposition, pSI->marginWidth), (titleHeight-tfh)/2);

    if (mStyle.menuTitleLabel) {
        gMenuInfo.nTitleHeight += MTitle->borderWidth + MFrame->marginWidth;
        gMenuInfo.nTitleIndent += MTitle->borderWidth;
    }

    // --------------------------------------
    // item height, indent

    ffh = get_fontheight(gMenuInfo.hFrameFont);
    itemHeight = MHilite->marginWidth + ffh + 2*MHilite->borderWidth;

    // xxx old behaviour xxx
    if (false == mStyle.is_070 && 0 == (MHilite->validated & V_MAR))
        itemHeight = MFrame->FontHeight + (mStyle.bevelWidth+1)/2;
    //xxxxxxxxxxxxxxxxxxxxxx

#ifdef BBOPT_MENUICONS
    itemHeight = imax(14, itemHeight);
    gMenuInfo.nItemHeight =
    gMenuInfo.nItemLeftIndent =
    gMenuInfo.nItemRightIndent = itemHeight;
    gMenuInfo.nIconSize = imin(itemHeight - 2, 16);
    if (DT_LEFT == MFrame->Justify)
        gMenuInfo.nItemLeftIndent += 2;
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
    gMenuInfo.MaxWidth = gSettingsMenu.showBroams
        ? iminmax(gSettingsMenu.maxWidth*2, 320, 640)
        : gSettingsMenu.maxWidth;

    // --------------------------------------
    // setup a StyleItem for the scroll rectangle
    pScrl = &gMenuInfo.Scroller;
    if (false == MTitle->parentRelative)
    {
        *pScrl = *MTitle;
        if (false == mStyle.menuTitleLabel) {
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

    if (mStyle.menuTitleLabel) {
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
    gMenuInfo.separatorWidth = gSettingsMenu.drawSeparators ? imax(1, MFrame->borderWidth) : 0;
    gMenuInfo.check_is_pr = MHilite->parentRelative
        || iabs(greyvalue(get_bg_color(MFrame))
                - greyvalue(get_bg_color(MHilite))) < 24;
}

void Menu_Stats(menu_stats* st)
{
    st->menu_count = Menu::menuCount();
    st->item_count = g_menu_item_count;
}

// this is used in snapWindowToEdge to avoid plugins snap to menus.
bool Menu_IsA(HWND hwnd)
{
    return (LONG_PTR)Menu::WindowProc == GetWindowLongPtr(hwnd, GWLP_WNDPROC);
}

bool Menu_Exists(bool pinned)
{
    MenuList *ml;
    dolist (ml, Menu::g_MenuWindowList)
        if (pinned == ml->m->getIsPinned())
            return true;
    return false;
}

void Menu_All_Redraw(int flags)
{
    MenuEnum(Menu::redraw_menu, &flags);
}

void Menu_All_Toggle(bool hidden)
{
    MenuEnum(Menu::toggle_menu, &hidden);
}

// bring all menus on top, restoring the previous z-order
void Menu_All_BringOnTop(void)
{
    MenuList *ml;
    Menu *m;
    dolist (ml, Menu::g_MenuWindowList)
        SetOnTop(ml->m->getWindowHandle());
    m = Menu::last_active_menu_root();
    if (m) 
        m->set_focus();
}

// Hide all menus, which are not pinned, except p, unless NULL
void Menu_All_Hide_But(Menu *p)
{
    MenuEnum(Menu::hide_menu, p);
} 

void Menu_All_Hide(void)
{
    Menu_All_Hide_But(NULL);
} 

// for invocation from the keyboard:
// menu is present:
//      - has focus: hide it and switch to application
//      - does not have focus: set focus to the menu
// is not present: return false -> show the menu.  
bool Menu_ToggleCheck(const char *menu_id)
{
    Menu *m;
    char IDString[MAX_PATH];

    if (menu_id[0])
        m = Menu::find_named_menu(Core_IDString(IDString, menu_id));
    else
        m = Menu::last_active_menu_root();

    if (NULL == m || NULL == m->getWindowHandle())
        return false;

    if (m->has_focus_in_chain()) {
        Menu_All_Hide();
        focus_top_window();
        return true;
    }

    if (m->getIsPinned()) {
        m->bring_ontop(true);
        return true;
    }

    return false;
}

//--------------------------------------
void Menu_Tab_Next(Menu* p)
{
    MenuList *ml;
    Menu *m = NULL;
    //bool backwards = 0x8000 & GetAsyncKeyState(VK_SHIFT);
    dolist (ml, Menu::g_MenuWindowList)
        if (NULL == (m = ml->m)->getParentMenu() && p != m)
            break;
    if (NULL == m)
        m = p;

    if (m->data.m_kbditempos == -1)
        m->data.m_kbditempos = m->data.m_topindex;
    m->bring_ontop(true);
}

//===========================================================================
// misc menu helpers

// Create the one item submenu for makeMenuItemInt and MakeMenuItemString
MenuItem* helper_menu(Menu *PluginMenu, const char* Title, int menuID, MenuItem *pItem)
{
    Menu *sub;
    if (PluginMenu->data.m_IDString) {
        char IDString[200];

        // build an idstring from the parent menu's and the title
        sprintf (IDString, "%s:%s", PluginMenu->data.m_IDString, Title);
        sub = Menu::find_named_menu(IDString);
        if (sub) {
            // re-use existing menu&item to avoid changes in the
            // edit control while the user is working on it
            sub->incref();
            sub->data.m_OldPos = -1;
            delete pItem;
            return makeSubmenu(PluginMenu, sub);
        }
        sub = makeNamedMenu(Title, IDString, PluginMenu->data.m_bPopup);
    } else {
        sub = makeMenu(Title);
    }
    sub->AddMenuItem(pItem);
    sub->setMenuId(menuID);
    return makeSubmenu(PluginMenu, sub);
} 

// Operate on all currently visible menus
Menu* MenuEnum(MENUENUMPROC fn, void* ud)
{
    MenuList *ml, *ml_copy = (MenuList*)copy_list(Menu::g_MenuWindowList);
    Menu *m = NULL;
    Menu::g_incref();
    dolist (ml, ml_copy) {
        m = ml->m;
        if (false == fn(m, ud))
            break;
    }
    Menu::g_decref();
    freeall(&ml_copy);
    return m;
}

}; //!namespace bbcore
