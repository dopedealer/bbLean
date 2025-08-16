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

#ifndef _BBDESK_H_
#define _BBDESK_H_

namespace bbcore { 

void Desk_Exit(void);
void Desk_Init(void);
void ShowExplorer(void);
void HideExplorer(void);

/// \brief Sets given image file as desktop background
/// Writes to correspondint extensions.rc field if arg present. In arg is null
/// tries to read value from extensions.rc file.
/// \param path Path to image file. Can be null.
void Desk_new_background(const char* path);

const char * Desk_extended_rootCommand(const char *p);
bool Desk_mousebutton_event(int button);
void Desk_Reset(bool all);
HBITMAP Desk_getbmp(void);

extern HWND hDesktopWnd;

} // !namespace bbcore

#endif /* _BBDESK_H_ */
