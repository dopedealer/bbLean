#ifndef _BBCORE_CONTEXT_ITEM_H
#define _BBCORE_CONTEXT_ITEM_H

#include "FolderItem.h"

namespace bbcore { 

class ContextItem : public FolderItem
{
public:
    ContextItem(Menu *m, char* pszTitle, int id, DWORD data, UINT type);
    ~ContextItem();
    void Paint(HDC hDC);
    void Measure(HDC hDC, SIZE *size);
    void Invoke(int button);
    void DrawItem(HDC hdc, int w, int h, bool active);
private:
    int   m_id;
    DWORD m_data;
    UINT  m_type;

    int m_icon_offset;
    HBITMAP m_bmp;
    int m_bmp_width;
    COLORREF cr_back;
};


} // !namespace bbcore

#endif //!_BBCORE_CONTEXT_ITEM_H
