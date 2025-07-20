
#include "barlabel.h"

#include <BBApi.h>
#include <bbstyle.h>

#include "barinfo.h"
#include "BuffBmp.h" 

extern int TBJustify; 

barlabel::barlabel(int type, barinfo* bi, char* text, int S)
    : baritem(type, bi)
{
    m_Style = S;
    m_text = text;
}

void barlabel::draw(void)
{
    StyleItem *pSI = (StyleItem*)GetSettingPtr(m_Style);
    m_bar->pBuff->MakeStyleGradient(m_bar->hdcPaint,  &mr, pSI, pSI->bordered);
    SetBkMode(m_bar->hdcPaint, TRANSPARENT);
    HGDIOBJ oldfont = SelectObject(m_bar->hdcPaint, m_bar->hFont);
    RECT r;
    int i = m_bar->labelIndent;
    r.left  = mr.left  + i;
    r.right = mr.right - i;
    r.top   = mr.top;
    r.bottom = mr.bottom;
    bbDrawText(m_bar->hdcPaint, m_text, &r, TBJustify, pSI->TextColor);
    SelectObject(m_bar->hdcPaint, oldfont);

}
