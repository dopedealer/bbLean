#ifndef _BBCORE_DESKTOP_MENU_H
#define _BBCORE_DESKTOP_MENU_H

class Menu;


namespace bbcore {

Menu* MakeDesktopMenu(int mode, bool popup);
Menu* MakeRecoverMenu(bool popup);
Menu* MakeTaskFolder(int workspace, bool popup);
void ShowRecoverMenu(void);

}; //!namespace bbcore

#endif //!_BBCORE_DESKTOP_MENU_H
