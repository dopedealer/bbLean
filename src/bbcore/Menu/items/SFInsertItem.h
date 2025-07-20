#ifndef _SF_INSERT_ITEM_H
#define _SF_INSERT_ITEM_H

#include "MenuItem.h"

namespace bbcore { 

// an invisble item that expands into a folder listing when validated
class SFInsert : public ::MenuItem
{
public:
    SFInsert(const char *pszPath, const char *pszExtra);
    ~SFInsert();
    void Paint(HDC hDC);
    void Measure(HDC hDC, SIZE *size);
    void RemoveStuff(void);
private:
    char* m_pszExtra{};
    MenuItem* m_pLast{};
};

} // !namespace bbcore

#endif //!_SF_INSERT_ITEM_H
