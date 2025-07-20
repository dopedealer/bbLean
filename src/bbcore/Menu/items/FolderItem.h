#ifndef _BBCORE_FOLDER_ITEM_H
#define _BBCORE_FOLDER_ITEM_H

#include "MenuItem.h"

namespace bbcore { 

// An menuitem that is a pointer to a sub menu, these folder items
// typically contain a |> icon at their right side.  
class FolderItem : public ::MenuItem
{
public:
    FolderItem(Menu* pSubMenu, const char* pszTitle);
    void Paint(HDC hDC);
    void Invoke(int button);
};

} // !namespace bbcore

#endif //!_BBCORE_FOLDER_ITEM_H
