

#include <Settings.h>

#include "ContextItem.h"
#include "Contextmenu.h"
#include "ShellContext.h"

namespace bbcore { 

ContextItem::ContextItem(Menu *m, char* pszTitle, int id, DWORD data, UINT type)
    : FolderItem(m, pszTitle)
{
    m_id   = id;
    m_data = data;
    m_type = type;
    m_bmp = NULL;
    if (NULL == m)
    {
        m_nSortPriority = M_SORT_NORMAL;
        m_ItemID = MENUITEM_ID_NORMAL;
    }
}

ContextItem::~ContextItem()
{
    if (m_bmp)
        DeleteObject(m_bmp);
}

void ContextItem::Invoke(int button)
{
    if (INVOKE_LEFT & button)
    {
        if (m_ItemID & MENUITEM_ID_FOLDER) {
            FolderItem::Invoke(button);
        } else {
            ShellContext *wc = ((ContextMenu*)m_pMenu)->wc;
            wc->addref();
            m_pMenu->hide_on_click();
            wc->Invoke(m_id);
            wc->decref();
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ContextItem::DrawItem(HDC buf, int w, int h, bool active)
{
    ContextMenu* Ctxt=(ContextMenu*)m_pMenu;

    DRAWITEMSTRUCT dis{};
    dis.CtlType     = ODT_MENU;
    dis.CtlID       = 0;
    dis.itemID      = m_id;
    dis.itemAction  = ODA_DRAWENTIRE;
    dis.itemState   = 0;
    dis.hwndItem    = Ctxt->getWindowHandle();
    dis.hDC         = buf;
    dis.rcItem.left     = 0;
    dis.rcItem.top      = 0;
    dis.rcItem.right    = w;
    dis.rcItem.bottom   = h;
    dis.itemData    = m_data;
    if (active)
        dis.itemState = ODS_SELECTED;
    Ctxt->wc->HandleMenuMsg(WM_DRAWITEM, 0, (LPARAM)&dis);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ContextItem::Measure(HDC hDC, SIZE *size)
{
    MEASUREITEMSTRUCT mai;
    int w, h;
    int x, y;
    HDC buf, hdc_s;
    HGDIOBJ other_bmp;
    HGDIOBJ otherfont;
    RECT r;
    HBRUSH hbr;
    ContextMenu* Ctxt=(ContextMenu*)m_pMenu;

    if (0==(m_type & MFT_OWNERDRAW))
    {
        MenuItem::Measure(hDC, size);
        return;
    }


    mai.CtlType     = ODT_MENU; // type of control
    mai.CtlID       = 0;        // combo box, list box, or button identifier
    mai.itemID      = m_id;     // menu item, variable-height list box, or combo box identifier
    mai.itemWidth   = 0;        // width of menu item, in pixels
    mai.itemHeight  = 0;        // height of single item in list box menu, in pixels
    mai.itemData    = m_data;   // application-defined 32-bit value

    Ctxt->wc->HandleMenuMsg(WM_MEASUREITEM, 0, (LPARAM)&mai);
    // the dumb measure method does not take an HDC,
    // and as such uses the system font as base instead of our's

    w = mai.itemWidth * 2;
    h = gMenuInfo.nItemHeight;

    if (m_bmp)
        DeleteObject(m_bmp);

    buf = CreateCompatibleDC(NULL);

    hdc_s = GetDC(NULL);
    m_bmp = CreateCompatibleBitmap(hdc_s, w, h);
    ReleaseDC(NULL, hdc_s);

    other_bmp = SelectObject(buf, m_bmp);

    hbr = CreateSolidBrush(GetSysColor(COLOR_MENU));
    r.left = r.top = 0, r.right = w, r.bottom = h;
    FillRect(buf, &r, hbr);
    DeleteObject(hbr);

    // get the background color for reference
    cr_back = GetPixel(buf, 0, 0);

    SetBkColor(buf, cr_back);
    SetBkMode(buf, TRANSPARENT);
    SetTextColor(buf, GetSysColor(COLOR_MENUTEXT));

    // let the item draw with our font
    otherfont = SelectObject(buf, gMenuInfo.hFrameFont);
    DrawItem(buf, w, h, false);
    SelectObject(buf, otherfont);

    // trick 17 (get the real width of the item)
    for (x = w; --x;)
        for (y = 0; y < h; y++)
            if (cr_back != GetPixel(buf, x, y)) {
                w = x+2;
                goto _break;
            }
_break:
    SelectObject(buf, other_bmp);
    DeleteDC(buf);
    // 29 = SendTo-Items
    m_icon_offset = Settings_contextMenuAdjust[m_id == 29];
    m_bmp_width = w;
    size->cx = w - m_icon_offset;
    size->cy = gMenuInfo.nItemHeight;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ContextItem::Paint(HDC hDC)
{
    RECT r;
    int w, h, m, tw, x, y;
    HDC buf;
    HGDIOBJ other_bmp;
    COLORREF crtxt_bb;

    if (m_ItemID & MENUITEM_ID_FOLDER)
        FolderItem::Paint(hDC);
    else
        MenuItem::Paint(hDC);

    if (0==(m_type & MFT_OWNERDRAW))
        return;

    GetTextRect(&r);
    w =  r.right  - r.left;
    h =  r.bottom - r.top;
    // the remaining margin
    m = imax(0, w - (m_bmp_width - m_icon_offset));
    // text width
    tw = w - m;

    buf = CreateCompatibleDC(NULL);
    other_bmp = SelectObject(buf, m_bmp);
#if 0
    BitBlt(hDC, r.left, r.top, tw, h, buf, m_icon_offset, 0, SRCCOPY);
#else
    // adjust offset according to justifications
    if (mStyle.MenuFrame.Justify == DT_CENTER)
        m /= 2;
    else
    if (mStyle.MenuFrame.Justify != DT_RIGHT)
        m = 0;

    // then plot points when they seem to have the textcolor
    // icons on the left are cut off
    crtxt_bb = m_bActive
        ? mStyle.MenuHilite.TextColor : mStyle.MenuFrame.TextColor;

    for (y = 0; y < h; y++)
        for (x = 0; x < tw; x++)
            if (cr_back != GetPixel(buf, x+m_icon_offset, y))
                SetPixel (hDC, r.left+m+x, r.top+y, crtxt_bb);
#endif

    SelectObject(buf, other_bmp);
    // this let's the handler know which command to invoke eventually
    if (m_bActive)
        DrawItem(buf, m_bmp_width, h, true);
    DeleteDC(buf);
}

} // !namespace bbcore
