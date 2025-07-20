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

#ifndef _MENUMAKER_H_
#define _MENUMAKER_H_

// This file defines the public menu interface.

class Menu;
class MenuItem;

namespace bbcore {

struct menu_stats {
    int menu_count;
    int item_count;
};

bool MenuMaker_ShowMenu(int id, const char* param);

void Menu_Init(void);
void Menu_Exit(void);
void Menu_Reconfigure(void);
void Menu_Stats(menu_stats* st);
bool Menu_IsA(HWND);
bool Menu_Exists(bool pinned);
void Menu_ResetFonts(void);

void Menu_All_Redraw(int flags);
void Menu_All_Toggle(bool hidden);
void Menu_All_BringOnTop(void);
void Menu_All_Hide(void);
void Menu_All_Hide_But(Menu *p);
bool Menu_ToggleCheck(const char* menu_id);

void Menu_Tab_Next(Menu* p);

void Menu_Update(int id);
// id for Menu_Update:
#define MENU_UPD_ROOT    1
#define MENU_UPD_TASKS   2
#define MENU_UPD_CONFIG  3

char* Core_IDString(char* buffer, const char* menu_id);
Menu* MakeRootMenu(const char* menu_id, const char* path, const char* default_menu, bool pop); 

MenuItem* helper_menu(Menu *PluginMenu, const char* Title, int menuID, MenuItem *pItem);

//====================
// special broams

#define MM_STYLE_BROAM "@BBCore.style %s"
#define MM_THEME_BROAM "@BBCore.theme %s"
#define MM_ROOT_BROAM "@BBCore.rootCommand %s"

void register_menuclass(void);
void un_register_menuclass(void);



Menu* makeNamedMenu(const char* HeaderText, const char* IDString, bool popup);

}; //!namespace bbcore


//===========================================================================
#endif /* ndef _MENUMAKER_H_s */
