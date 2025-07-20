/* ==========================================================================

  This file is part of the bbLean source code
  Copyright © 2001-2003 The Blackbox for Windows Development Team
  Copyright © 2004-2009 grischka

  http://bb4win.sourceforge.net/bblean
  http://developer.berlios.de/projects/bblean

  bbLean is free software, released under the GNU General Public License
  (GPL version 2). For details see:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  ========================================================================== */


#include "BB.h"
#include "Utils.h"
#include "Settings.h"
#include "bbrc.h"
#include "nls.h"

#include <shlobj.h>
#include <time.h>
#include <shellapi.h>
#include <malloc.h>
#include <windows.h>

#include "rcfile.h"

namespace bbcore {

typedef struct pixinfo {
    short size;
    char w, h, n, u;
    char vector[12];
    unsigned char bits[2];
} pixinfo;

// global variables
static pixinfo* gPixmap;


/// \brief Replace special folders in string (like APPDATA).
/// Standard BB-MessageBox
int bbMessageBoxV(int flg, const char *fmt, va_list args)
{
    const char *caption = BBAPPNAME;
    const char *message;
    char *p, *q;
    int r;

    message = p = m_formatv(fmt, args);
    if ('#' == p[0] &&  NULL != (q = strchr(p+1, p[0])))
        // "#Title#Message" is wanted
        *q = 0, caption = p+1, message = q+1;

#ifdef BBTINY
    r = MessageBox(NULL, message, caption, flg | MB_SYSTEMMODAL);
#else

    ::MessageBeep(0);
    MSGBOXPARAMSW mp;
    int lc = 1 + strlen(caption);
    int lm = 1 + strlen(message);
    WCHAR *wcaption = (WCHAR*)m_alloc(lc * sizeof (WCHAR));
    WCHAR *wmessage = (WCHAR*)m_alloc(lm * sizeof (WCHAR));
    bbMbyteToWideChar(caption, wcaption, lc);
    bbMbyteToWideChar(message, wmessage, lm);

    memset(&mp, 0, sizeof mp);
    mp.cbSize = sizeof mp;
    mp.hInstance = hMainInstance;
    //mp.hwndOwner = NULL;
    mp.lpszText = wmessage;
    mp.lpszCaption = wcaption;
    mp.dwStyle = flg | MB_USERICON | MB_SYSTEMMODAL;
    mp.lpszIcon = MAKEINTRESOURCEW(IDI_BLACKBOX);
    r = ::MessageBoxIndirectW(&mp);
    m_free(wcaption);
    m_free(wmessage); 
#endif

    m_free(p);

    return r;
}

int bbMessageBox(int flg, const char *fmt, ...)
{
    va_list args; 
    va_start(args, fmt);
    auto result = bbMessageBoxV(flg, fmt, args);
    va_end(args);

    return result;
}

/// \brief Draw a Pixmap for buttons, menu bullets, checkmarks ...
void bbDrawPixImpl(HDC hDC, RECT* rc, COLORREF color, int pic)
{
    int x0, y0, y, z, f, x1, x2;
    pixinfo *pmi{};
    unsigned char c, *pv;
    HGDIOBJ oldPen{};

    if (nullptr == gPixmap)
    {
        read_pix();
    }

    pmi = gPixmap;
    x0 = (rc->left + rc->right - pmi->h)/2;
    y0 = (rc->top + rc->bottom - pmi->w)/2;
    f = 1;

    if (pic < 0)
    {
        // draw mirrored
        pic = -pic;
        x0 += pmi->w - 1;
        f = -1;
    }

    if (pic == 1 && Settings_arrowUnix)
    {
        pic = 0;
    }
    if (0 == mStyle.bulletUnix)
    {
        pic += 6;
    } 
    if (pic >= pmi->n)
    {
        return;
    }

    pv = pmi->bits + pmi->vector[pic]; 
    if (!usingXP || usingVista)
    {
        oldPen = SelectObject(hDC, CreatePen(PS_SOLID, 1, color));
    }

    for (y = y0; 0 != (c = *pv); ++pv)
    {
        z = c >> 4;
        if (z == 0)
        {
            y += c;
            continue;
        }
        if (z & 8)
        {
            z &= 7;
            ++y;
        }

        x1 = x0 + f * (c & 15);
        x2 = x1 + f * z;

        if (oldPen)
        {
            // buggy on XP with dropshadows for menus enabled
            MoveToEx(hDC, x1, y, NULL);
            LineTo(hDC, x2, y);
        }
        else
        {
            // buggy on Vista with dropshadows for menus not enabled
            while (x1 != x2)
                SetPixel(hDC, x1, y, color), x1 += f;
       }
    }

    if (oldPen)
    {
        DeleteObject(SelectObject(hDC, oldPen));
    }
}

int drawTextUTF8(HDC hDC, const char *s, int nCount, RECT *p, unsigned format)
{
    WCHAR wstr[1000];
    SIZE size;
    int x, y, n, r;

    n = MultiByteToWideChar(CP_UTF8, 0, s, nCount, wstr, array_count(wstr));
    if (n) --n;

    if (usingNT)
        return DrawTextW(hDC, wstr, n, p, format);

    GetTextExtentPoint32W(hDC, wstr, n, &size);
    if (format & DT_CALCRECT) {
        p->right = p->left + size.cx;
        p->bottom = p->top + size.cy;
        return 1;
    }

    if (format & DT_RIGHT)
        x = imax(p->left, p->right - size.cx);
    else if (format & DT_CENTER)
        x = imax(p->left, (p->left + p->right - size.cx) / 2);
    else
        x = p->left;

    if (format & DT_BOTTOM)
        y = imax(p->top, p->bottom - size.cy);
    else if (format & DT_VCENTER)
        y = imax(p->top, (p->top + p->bottom - size.cy) / 2);
    else
        y = p->top;

    //SetTextAlign(hDC, TA_LEFT | TA_TOP);
    r = ExtTextOutW(hDC, x, y, ETO_CLIPPED, p, wstr, n, NULL);
    return r;
}

/// \brief Draw a textstring with color, possibly translating from UTF-8
void bbDrawTextImpl(HDC hDC, const char* text, RECT* p_rect, unsigned format, COLORREF c)
{
    if (0 == (format & DT_CALCRECT))
    {
        SetTextColor(hDC, c);
    }

    if (isUsingUtf8Encoding())
    {
        drawTextUTF8(hDC, text, -1, p_rect, format);
    }
    else
    {
        DrawText(hDC, text, -1, p_rect, format);
    }
}

/// \brief convert from Multibyte string to WCHAR
int bbMbyteToWideChar(const char* src, WCHAR* wstr, int len)
{
    int x, n;
    for (x = -1;;)
    {
        n = MultiByteToWideChar(isUsingUtf8Encoding() ? CP_UTF8 : CP_ACP,
                0, src, x, wstr, len);
        if (n)
        {
            return n;
        }
        if (x < 0)
        {
            x = len;
        }
        if (--x == 0)
        {
            break;
        }
    }
    wstr[0] = 0;
    return 0;
}

/// \brief convert from WCHAR to Multibyte string
int bbWideCharToMbyte(const WCHAR *src, char *str, int len)
{
    return wchar_to_mbyte(src, str, len, isUsingUtf8Encoding());
}

/// \brief Write to blackbox.log, not implemented in bblean.
/// Appends given line to Blackbox.log file
void log(const char* Title, const char* Line)
{
    log_printf((LOG_PLUGINS, "%s: %s", Title, Line));
}

/// \brief Complain about missing file.
/// Gives a message box proclaming missing file
int mboxErrorFile(const char* szFile)
{
    return bbMessageBox(MB_OK, NLS2("$Error_ReadFile$",
        "Error: Unable to open file \"%s\"."
        "\nPlease check location and try again."
        ), szFile);
}

/// \brief Complain about something
/// Gives a message box proclaming a given value
int mboxErrorValue(const char* szValue)
{
    return bbMessageBox(MB_OK, NLS2("$Error_MBox$", "Error: %s"), szValue);
}

COLORREF get_bg_color(StyleItem *pSI)
{
    if (B_SOLID == pSI->type) // && false == pSI->interlaced)
        return pSI->Color;
    return mixcolors(pSI->Color, pSI->ColorTo, 128);
}

COLORREF get_mixed_color(StyleItem *pSI)
{
    COLORREF b = get_bg_color(pSI);
    COLORREF t = pSI->TextColor;
    if (greyvalue(b) > greyvalue(t))
        return mixcolors(t, b, 96);
    else
        return mixcolors(t, b, 144);
}


int BBMessageBoxV(int flg, const char *fmt, va_list args)
{
    return bbMessageBoxV(flg, fmt, args);
} 

//===========================================================================
// Function: BBRegisterClass
// Purpose:  Register a window class, display error on failure
//===========================================================================

BOOL BBRegisterClass (const char *classname, WNDPROC wndproc, int flags)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.hInstance = hMainInstance;
    wc.lpszClassName = classname;
    wc.lpfnWndProc = wndproc;
    if (flags & BBCS_VISIBLE) {
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.style |= CS_DBLCLKS;
    }
    if ((flags & BBCS_DROPSHADOW) && usingXP)
        wc.style |= CS_DROPSHADOW;
    if (flags & BBCS_EXTRA)
        wc.cbWndExtra = sizeof(void*);
    if (RegisterClass(&wc))
        return 1;
    bbMessageBox(MB_OK, NLS2("$Error_RegisterClass$",
        "Error: Could not register \"%s\" window class."), classname);
    return 0;
}

void draw_line_h(HDC hDC, int x1, int x2, int y, int w, COLORREF C)
{
    HGDIOBJ oldPen;
    if (w <= 0)
        return;
    oldPen = SelectObject(hDC, CreatePen(PS_SOLID, 1, C));
    do {
        MoveToEx(hDC, x1, y, NULL);
        LineTo  (hDC, x2, y);
        ++y;
    } while (--w);
    DeleteObject(SelectObject(hDC, oldPen));
}


//===========================================================================
// replace %s and %1 in 'fmt' by 'arg' - allocate string for output

char* replace_arg1(const char *fmt, const char *arg)
{
    const char *va[] = { arg, arg, arg, arg };
    return m_formatv(fmt, (va_list)va);
}

//===========================================================================

void get_window_text(HWND hwnd, char *buffer, int size)
{
    if (usingNT)
    {
        WCHAR wbuf[1000];
        wbuf[0] = 0;
        GetWindowTextW(hwnd, wbuf, size);
        bbWideCharToMbyte(wbuf, buffer, size);
    } else {
        buffer[0] = 0;
        GetWindowTextA(hwnd, buffer, size);
    }
}

void get_window_icon(HWND hwnd, HICON *picon)
{
    HICON hIco = NULL;
    SendMessageTimeout(hwnd, WM_GETICON, ICON_SMALL,
        0, SMTO_ABORTIFHUNG|SMTO_NORMAL, 1000, (DWORD_PTR*)&hIco);
    if (NULL==hIco) {
    SendMessageTimeout(hwnd, WM_GETICON, ICON_BIG,
        0, SMTO_ABORTIFHUNG|SMTO_NORMAL, 1000, (DWORD_PTR*)&hIco);
    if (NULL==hIco) {
        hIco = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);
    if (NULL==hIco) {
        hIco = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    if (NULL==hIco) {
        return;
    }}}}
    if (*picon)
        DestroyIcon(*picon);
    *picon = CopyIcon(hIco);
}

//===========================================================================
// logging support
//===========================================================================

void _log_printf(int flag, const char *fmt, ...)
{
    static bool log_first;
    char log_path[MAX_PATH];
    FILE *fp;
    va_list arg;
    char date[32];
    char time[16];

    if (flag && 0 == (flag & Settings_LogFlag))
        return;

    fp = fopen(set_my_path(NULL, log_path, "blackbox.log"), "a");

    if (NULL == fp)
        return;

    if (false == log_first) {
        _strdate(date);
        _strtime(time);
        fprintf(fp, "\nStarting Log %s %s\n", date, time);
        log_first = true;
    }

    if ('\n' != *fmt) {
        _strtime(time);
        fprintf(fp, "%s  ", time);
    }

    va_start(arg, fmt);
    vfprintf(fp, fmt, arg);
    fprintf(fp, "\n");
    fclose(fp);
    va_end(arg);
} 

struct boxinfo {
    const char *c, *m, *i;
    char *b;
};

BOOL CALLBACK dlgproc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static char *buffer;
    boxinfo* b = (boxinfo *)lParam;
    switch( msg )
    {
    case WM_INITDIALOG:
        SetWindowText (hDlg, b->c);
        SetDlgItemText(hDlg, 401, b->m);
        SetDlgItemText(hDlg, 402, b->i);
        buffer = b->b;
        return 1;
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDOK:
            GetDlgItemText (hDlg, 402, buffer, 256);
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return 1;
        }
    default:
        break;
    }
    return 0;
}

int EditBox(
    const char *caption,
    const char *message,
    const char *initvalue,
    char *buffer)
{
    boxinfo b = { caption, message, initvalue, buffer };
    return DialogBoxParam(
        NULL, MAKEINTRESOURCE(IDC_EDITBOX), NULL, (DLGPROC)dlgproc, (LPARAM)&b);
}

//===========================================================================
int DrawTextUTF8(HDC hDC, const char *s, int nCount, RECT *p, unsigned format)
{
    return drawTextUTF8(hDC, s, nCount, p, format);
} 

//===========================================================================


void reset_pix(void);
void read_pix(void);
pixinfo * load_pix(const char *path);

static const unsigned char defpix[] = {
    0x82,0x00,
    11,11,12,0,
    0,5,12,22,29,39,
    48,55,64,78,87,101,
    0x04,0x24,0xC4,0xA4,0x00,
    0x03,0x14,0xA4,0xB4,0xA4,0x94,0x00,
    0x03,0x53,0x93,0x17,0x93,0x17,0x93,0x17,0xD3,0x00,
    0x03,0x15,0xB4,0xD3,0xB4,0x95,0x00,
    0x03,0x34,0x93,0x17,0x93,0x17,0x93,0x17,0xB4,0x00,
    0x02,0x18,0xA7,0xA6,0xA2,0x25,0xB3,0x94,0x00,
    0x03,0x23,0xC3,0xE3,0xC3,0xA3,0x00,
    0x02,0x14,0xA4,0xB4,0xC4,0xB4,0xA4,0x94,0x00,
    0x02,0x72,0x92,0x18,0x92,0x18,0x92,0x18,0x92,0x18,0x92,0x18,0xF2,0x00,
    0x02,0x15,0xB4,0xD3,0xF2,0xD3,0xB4,0x95,0x00,
    0x02,0x34,0x93,0x17,0x92,0x18,0x92,0x18,0x92,0x18,0x93,0x17,0xB4,0x00,
    0x02,0x18,0xA7,0x92,0x36,0xA2,0x35,0xD2,0xB3,0x94,0x00
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if 0
void write_pix(pixinfo *pmi)
{
    unsigned c;
    unsigned char *v;
    int n;
    char path[MAX_PATH];
    FILE *fp;

    if (NULL == pmi)
        return;

    findRCFile(path, "defpix.txt", NULL);
    fp = fopen(path, "wt");

    c = pmi->size;
    fprintf(fp, "\t0x%02X,0x%02X", c&255, c>>8);
    fprintf(fp, ",\n\t%d,%d,%d,0", pmi->w, pmi->h, pmi->n);
    for (n = 0; n < pmi->n; ++n) {
        c = pmi->vector[n];
        fprintf(fp, ",%s%d", n%6?"":"\n\t", c&255);
    }
    for (n = 0; n < pmi->n; ++n) {
        v = pmi->bits + pmi->vector[n];
        fprintf(fp, ",\n\t0x%02X", *v);
        while (*v)
            fprintf(fp, ",0x%02X", *++v);
    }
    fprintf(fp, "\n");
    fclose(fp);
}
#else
#define write_pix(n)
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void reset_pix(void)
{
    if (gPixmap)
        m_free(gPixmap), gPixmap = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void read_pix(void)
{
    char path[MAX_PATH];
    const char *p;
    pixinfo *pmi = NULL;

    reset_pix();

    p = read_string(bbExtensionsRcPath(NULL),
        "blackbox.appearance.bullet.bitmap:", NULL);

    if (p && findRCFile(path, p, NULL)) {
        pmi = load_pix(path);
        write_pix(pmi);
    }

    if (NULL == pmi) {
        pmi = (pixinfo*)defpix;
        pmi = (pixinfo *)memcpy(m_alloc(pmi->size), pmi, pmi->size);
    }
    gPixmap = pmi;
}

//===========================================================================

pixinfo *load_pix(const char *path)
{
    BITMAP bm; HDC hdc; HGDIOBJ other; HBITMAP hbmp;
    int y, x, px, py, x0, y0;
    unsigned char *v;
    pixinfo *pmi, *pmi2;

    int w, h;
    int bx, by;

    hbmp = (HBITMAP)LoadImage(
        NULL,
        path,
        IMAGE_BITMAP,
        0,
        0,
        LR_LOADFROMFILE
        );

    if (NULL == hbmp)
        return NULL;

    bx = 6, by = 1, w = h = 9;

    if (GetObject(hbmp, sizeof bm, &bm))
    {
        w = (bm.bmWidth-1)/bx-1;
        // the encoding below can handle 15x15 max.
        w = h = iminmax(w, 9, 15);
        by = (bm.bmHeight-1)/(h+1);
    }

    //debug_printf("bx:%d - by:%d - %d/%d", bx, by, w, h);

    hdc = CreateCompatibleDC(NULL);
    other = SelectObject(hdc, hbmp);

    pmi = (pixinfo *)c_alloc(sizeof(pixinfo) + bx*by * h * (w*3+1));
    pmi->w = (char)w;
    pmi->h = (char)h;
    pmi->n = (char)(bx*by);
    v = pmi->bits;

    for (py = 0; py < by; ++py) {
    for (px = 0; px < bx; ++px) {
        pmi->vector[px+py*bx] = (char)(v - pmi->bits);
        for (y0 = y = 0; y < h; ++y) {
        for (x0 = x = 0; x <= w; ++x) {
            int dx = x - x0;
            if (dx == 7) {
                --x;
            } else if (x < w) {
                if (0 == GetPixel(hdc, px*(w+1)+1+x, py*(h+1)+1+y))
                    continue;
            }
            if (dx) {
                int dy = y - y0;
                if (dy) {
                    if (dy == 1)
                        dx |= 8;
                    else
                        *v++ = (unsigned char)dy;
                    y0 = y;
                }
                *v++ = (unsigned char)((dx<<4)|x0);
            }
            x0 = x+1;
        }}
        *v++ = 0;
    }}

    pmi->size = (short)(sizeof(pixinfo) + (v - pmi->bits - 2));

    pmi2 = (pixinfo *)m_alloc(pmi->size);
    memcpy(pmi2, pmi, pmi->size);
    m_free(pmi);

    DeleteObject(SelectObject(hdc, other));
    DeleteDC(hdc);

    return pmi2;
} 

//===========================================================================

static string_node *bbFonts;

void register_fonts(void)
{
#if 1
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    char path[MAX_PATH], *end;
    const char * const *cp;
    int chg = 0;

    static const char * const font_extensions[] = {
        "fon", "ttf", "fnt", NULL
    };

    end = strchr(set_my_path(NULL, path, "fonts"), 0);
    for (cp = font_extensions; *cp; ++cp)
    {
        sprintf(end, "\\*.%s", *cp);
        hFind = FindFirstFile(path, &findData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                string_node *sn, **psn;
                strcpy(end+1, findData.cFileName);
                for (psn = &bbFonts; 0 != (sn = *psn); psn = &sn->next)
                    if (0 == strcmp(sn->str, path))
                        break;
                if (!sn) {
                    int r = AddFontResource(path);
                    // debug_printf("add font (%d) %s", r, path);
                    if (r)
                        ++chg, *psn = new_string_node(path);
                }
            } while (FindNextFile(hFind, &findData));
            FindClose(hFind);
        }
    }

    if (chg)
        PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
#endif
}

void unregister_fonts(void)
{
    string_node* sn = bbFonts;
    if (sn) {
        do {
            //int r =
            RemoveFontResource(sn->str);
            // debug_printf("remove font (%d) %s", r, sn->str);
        } while (0 != (sn = sn->next));
        freeall(&bbFonts);
        PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    }
}

//===========================================================================

}; //! namespace bbcore

