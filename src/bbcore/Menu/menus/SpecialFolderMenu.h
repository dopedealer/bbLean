#ifndef _BBCORE_SPEC_FOLDER_MENU_H
#define _BBCORE_SPEC_FOLDER_MENU_H

#include "Menu.h"

namespace bbcore { 

class SpecialFolderItem;

// a menu containing items from a folder
class SpecialFolderMenu : public ::Menu
{
public:
    SpecialFolderMenu(const char* pszTitle, const pidl_node* pidl_list, const char* pszExtra);
    ~SpecialFolderMenu(void);
    void UpdateFolder(void);
    friend class SpecialFolderItem;
private:
    char* m_pszExtra{};
}; 

} // !namespace bbcore 

#endif //!_BBCORE_SPEC_FOLDER_MENU_H
