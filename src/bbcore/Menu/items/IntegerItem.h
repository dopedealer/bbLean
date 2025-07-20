#ifndef _BBCORE_INTEGER_ITEM_H
#define _BBCORE_INTEGER_ITEM_H

#include "CommandItem.h"


namespace bbcore { 

class IntegerItem : public CommandItem
{
public:
    IntegerItem(const char* pszCommand, int value, int minval, int maxval);

    void Mouse(HWND hwnd, UINT uMsg, DWORD wParam, DWORD lParam);
    void Invoke(int button);
    void ItemTimer(UINT nTimer);
    void Measure(HDC hDC, SIZE *size);
    void Key(UINT nMsg, WPARAM wParam);
    void set_next_value(void);

    int m_value;
    int m_min;
    int m_max;
    int m_count;
    int m_direction;
    int m_oldsize;
    int m_offvalue;
    const char *m_offstring;
}; 


} // !namespace bbcore

#endif //!_BBCORE_INTEGER_ITEM_H
