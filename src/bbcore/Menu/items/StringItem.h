#ifndef _BBCORE_STRING_ITEM_H
#define _BBCORE_STRING_ITEM_H

#include "CommandItem.h"

namespace bbcore { 

class StringItem : public CommandItem
{
public:
    StringItem(const char* pszCommand, const char *init_string);
    ~StringItem();

    void Paint(HDC hDC);
    void Measure(HDC hDC, SIZE *size);
    void Invoke(int button);

    static LRESULT CALLBACK EditProc(HWND hText, UINT msg, WPARAM wParam, LPARAM lParam);
    HWND hText;
    WNDPROC wpEditProc;
    RECT m_textrect;
}; 

} // !namespace bbcore 

#endif //!_BBCORE_STRING_ITEM_H
