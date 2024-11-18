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
// bbapi-small.cpp - rc-file reader for blackbox styles

#include "BBApi.h"
#include "BImage.h"
#include "bbrc.h"

static struct rcreader_init g_rc =
{
    NULL,   // struct fil_list *rc_files;
    NULL,   // void (*write_error)(const char *filename);
    true,   // char dos_eol;
    true,   // char translate_065;
    0,      // char found_last_value;
};

void bb_rcreader_init(void)
{
    init_rcreader(&g_rc);
} 
