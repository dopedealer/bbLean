/*
 ============================================================================

  This file is part of the bbStyleMaker source code
  Copyright 2003-2009 grischka@users.sourceforge.net

  http://bb4win.sourceforge.net/bblean
  http://developer.berlios.de/projects/bblean

  bbStyleMaker is free software, released under the GNU General Public
  License (GPL version 2). For details see:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

 ============================================================================
*/

#ifndef BBDIALOG_H
#define BBDIALOG_H

#include <windows.h>
#include <bbstyle.h>

#define UPDOWN_TIMER 2
#define UPDATE_TIMER 3
#define SLIDER_TIMER 4

// ----------------------
enum {
    BN_NULL,
    BN_RECT,
    BN_STR ,
    BN_BTN ,
    BN_CHK ,
    BN_ITM ,
    BN_SLD ,
    BN_EDT ,
    BN_UPDN,
    BN_COLOR
};

// TODO: remove global
extern const char *dlg_item_types[];

// ----------------------
#define BN_DIS 1
#define BN_ON  2
#define BN_ACT 4
#define BN_HID 8

#define BN_LFT 0x10
#define BN_CEN 0x20
#define BN_RHT 0x40
#define BN_RAD 0x80
#define BN_GRP 0x100
#define BN_EXT 0x200
#define BN_BUF 0x400
#define BN_16  0x800
#define BN_255 0x1000

// TODO: remove global
extern const char* dlg_item_flags[];

// ----------------------

enum {
    FIRST_ITEM = 0,
#define TOKEN(s) s,
#include "tokens.inl"
#undef TOKEN
    LAST_ITEM
};

// TODO: remove global
extern const char* dlg_item_strings[LAST_ITEM-FIRST_ITEM];

// ----------------------
// dialog types
#define D_DLG  1
#define D_BOX  2
#define D_MENU 3

// ----------------------
// flags for bb_msgbox
#define IDALWAYS    11
#define IDNEVER     12

#define B_OK        1
#define B_YES       2
#define B_NO        4
#define B_CANCEL    8
#define B_ALWAYS    16
#define B_NEVER     32

// ----------------------
#define BMP_NULL  0
#define BMP_TITLE 1
#define BMP_FRAME 2
#define BMP_SLD1  3
#define BMP_SLD2  4
#define BMP_ALL   5

struct dlg;

typedef LRESULT CALLBACK dlg_proc (dlg* dlg, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct button {
    const char *str;
    int typ, msg, x0, y0, w0, h0, f;

    button* next;
    dlg* dlg;
    DWORD_PTR data;
    int x, y, w, h;
};

struct dlg {
    button* bn_ptr;
    button* bn_act;
    int  tf, tx, ty;
    int  w, h;
    int  x, y;
    int typ;

// move/size -->>
    int sx, sy, sizing;
    int config;
// ------------<<

    int title_h, tab, ud;

    HWND  hwnd;
    dlg_proc * proc;
    int w_orig, h_orig;
    int dx, dy;

    HFONT fnt1, fnt2;
    int captionbar;
    int box_flags;

    HGDIOBJ my_bmps[BMP_ALL];
    char title[120];
};

#define SB_DIV 8192
#define EDT_ID 1234
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
int make_dlg_wnd(dlg* dlg, HWND parent_hwnd, int x, int y, const char *title, dlg_proc *proc);

void get_item_rect(button* b, RECT *r);
void get_slider_rect(button* b, RECT *r);
void set_slider(button* b, int my);
void invalidate_item(dlg* d, int id);
int inside_item(button* b, int mx, int my);
void fix_box(dlg* dlg);

void put_bitmap(dlg* d, HDC hdc, RECT *rc, StyleItem *pSI, int borderWidth, int index, RECT* rPaint);
void create_editline (button* bp);
void delete_bitmaps(dlg* d);
void paint_box(dlg* d);

int get_accel_msg(dlg* d, int key);
int dlg_mouse (dlg* d, UINT msg, WPARAM wParam, LPARAM lParam);
void dlg_timer(dlg* dlg, int n_timer);

void check_button (dlg* d, int msg, int f);
void check_radio (dlg* d, int msg);

void enable_button (dlg* d, int msg, int f);
void enable_section(dlg* d, int i1, int i2, int en);

void show_button (dlg* d, int msg, int f);
void show_section(dlg* d, int i1, int i2, int en);

void set_button_data (dlg* d, int msg, int f);
void set_button_text (dlg* d, int msg, const char *text);
button* getbutton (dlg* d, int msg);
button* mousebutton(dlg* dlg, int mx, int my);
void insert_first(dlg* dlg, button* cp);

WPARAM do_message_loop(void);
void fix_dlg(dlg* dlg);
void set_dlg_windowpos(dlg* dlg);
dlg* make_dlg (const button* bp0, int w, int h);
void invalidate_button(button* bp);
void delete_dlg(dlg* dlg);
void set_button_focus(dlg* dlg, int id);
int get_button_text(dlg* dlg, int id, char* text, int bufsize);

#endif //!BBDIALOG_H
