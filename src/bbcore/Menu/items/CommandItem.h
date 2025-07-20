#ifndef _BBCORE_COMMAND_ITEM_H
#define _BBCORE_COMMAND_ITEM_H

#include "MenuItem.h"

namespace bbcore { 

class CommandItem : public ::MenuItem
{
public:
    CommandItem(const char* pszCommand, const char* pszTitle, bool bChecked);
    void Invoke(int button);
    void next_item (WPARAM wParam);
};

} // !namespace bbcore

#endif //!_BBCORE_COMMAND_ITEM_H
