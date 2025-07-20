#ifndef _BBLEAN_BAR_BUFFBMP_H
#define _BBLEAN_BAR_BUFFBMP_H

// Auto-buffered wrapper for MakeStyleGradient

// call instead:
// void BuffBmp::MakeStyleGradient(HDC hdc, RECT *rc, StyleItem *pSI, bool bordered);

// call after everything is drawn, and once at end of program to cleanup
// void BuffBmp::ClearBitmaps(void);

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>

struct StyleItem;

struct Bmp
{
    Bmp* next;

    /* 0.0.80 */
    int bevelstyle;
    int bevelposition;
    int type;
    bool parentRelative;
    bool interlaced;

    /* 0.0.90 */
    COLORREF Color;
    COLORREF ColorTo;

    int borderWidth;
    COLORREF borderColor;

    RECT r;
    HBITMAP bmp;
    bool in_use;
};


class BuffBmp
{

public:
    BuffBmp(void) = default; 
    ~BuffBmp(void);

    void MakeStyleGradient(HDC hdc, RECT* rc, StyleItem* pSI, bool withBorder); 
    void ClearBitmaps(bool force = false); 

private: 
    Bmp* buffers_{};
};






#endif //!_BBLEAN_BAR_BUFFBMP_H
