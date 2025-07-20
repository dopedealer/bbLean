/*---------------------------------------------------------------------------*

  This file is part of the BBNote source code

  Copyright 2003-2009 grischka@users.sourceforge.net

  BBNote is free software, released under the GNU General Public License
  (GPL version 2). For details see:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

 *---------------------------------------------------------------------------*/
// bbstyle_bbnote.h

#define VERSION_STRING "BBNote 1.08"

#include <bbcore.h>
#include "BBApi.h"
#include "win0x500.h"
#include "bblib.h"
#include "BImage.h"
#include "Stylestruct.h"

//#define MAX_LINE_LENGTH 256

void MakeGradient_s (HDC hdc, RECT rw, StyleItem *si, int borderWidth);
extern bbcore::StyleStruct gMStyle;

// FolderItem bullet positions
#define FOLDER_RIGHT    0
#define FOLDER_LEFT     1

//=======================================
// A structure with precalculated menu metrics,
// as filled in by 'MenuMaker_Reconfigure()'.

extern bbcore::MenuInfo gMenuInfo;

/*----------------------------------------------------------------------------*/

#define SB_DIV 4000
#define FRM  4
extern int vscr, but1;

#define CMD_UNDO    1037
#define CMD_REDO    1038
#define CMD_ZOOM    1039
#define CMD_MENU_1  1040
#define CMD_MENU_2  1041
#define CMD_COLOR   1042
#define CMD_LIST    1043

int bb_menu(HWND hwnd, int);
int bb_msgbox(HWND hwnd, const char *s, int f);
int bb_search_dialog(HWND hwnd);
int bb_config_dialog(HWND hwnd);
void bb_close_dlg(void);
void bbnote_help(void);
void draw_line(HDC hDC, int x1, int x2, int y, int w, COLORREF C);
void draw_frame(HDC hdc, RECT *prect, int title_h, RECT *ptext);

#ifndef STRL
struct strl { struct strl *next; char str[1]; };
#define STRL
#endif

strl* newstr(const char *s);
void freelist(void *p);
void appendstr(strl** p, const char *s);

void bb_file_menu(HWND, int, strl*);
bool bb_getstyle(HWND hwnd);
void bb_reconfig(void);
void bb_sound(int f);
bool bb_register(HWND);
void bb_unregister(HWND);

int readstyle(const char *fname);
void makedlgfont(void);
void get_vscr_rect(RECT* rw);
char *set_my_path(char*, const char *);
void bb_rcreader_init(void);

int fileexist(char*);

extern strl* editfiles;
extern char defstyle[];

extern HFONT fnt1, fnt2;
extern int title_h;
extern int mousewheelfac;

extern int ewx0;
extern int ewy0;
extern int ewxl;
extern int ewyl;
extern char cfg_f;

/*----------------------------------------------------------------------------*/
