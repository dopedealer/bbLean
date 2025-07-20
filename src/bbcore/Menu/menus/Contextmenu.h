#ifndef _BBCORE_CONTEXT_MENU_H
#define _BBCORE_CONTEXT_MENU_H

#include "Menu.h"

namespace bbcore { 

class ContextItem;

class ContextMenu : public ::Menu
{
public:
    ContextMenu(const char* title, ShellContext* w, HMENU hm, int m);
    ~ContextMenu(void);
private:
    void Copymenu(HMENU hm);

    ShellContext* wc{};
    friend class ContextItem;
};

} // !namespace bbcore

#endif //!_BBCORE_CONTEXT_MENU_H
