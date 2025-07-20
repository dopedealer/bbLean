#ifndef _BBCORE_SEPARATOR_ITEM_H
#define _BBCORE_SEPARATOR_ITEM_H

#include "MenuItem.h"

namespace bbcore { 

class SeparatorItem : public ::MenuItem
{
public:
    SeparatorItem() : MenuItem("") {}
    void Measure(HDC hDC, SIZE *size);
    void Paint(HDC hDC);
};


} // !namespace bbcore


#endif //!_BBCORE_SEPARATOR_ITEM_H
