#ifndef _BBCORE_TITLE_ITEM_H
#define _BBCORE_TITLE_ITEM_H

#include "MenuItem.h"

namespace bbcore { 

class TitleItem : public ::MenuItem
{
public:
    TitleItem(const char* pszTitle) : MenuItem(pszTitle) {}
    void Paint(HDC hDC);
    void Mouse(HWND hw, UINT nMsg, DWORD wP, DWORD lP);
};

} // !namespace bbcore 

#endif //!_BBCORE_TITLE_ITEM_H
