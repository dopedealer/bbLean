#ifndef _BBCORE_CONFIG_MENU_H
#define _BBCORE_CONFIG_MENU_H

namespace bbcore {

struct cfgmenu
{
    const char* text;
    const char* command;
    const void* pvalue;
};

Menu* MakeConfigMenu(bool popup);
Menu *CfgMenuMaker(const char* title, const char* defbroam, const cfgmenu* pm, bool pop, char* menu_id);
int exec_cfg_command(const char* pszCommand);
const void *exec_internal_broam(const char* argument, const cfgmenu* menu_root, const cfgmenu** p_menu, const cfgmenu** p_item);


}; //!namespace bbcore

#endif //!_BBCORE_CONFIG_MENU_H
