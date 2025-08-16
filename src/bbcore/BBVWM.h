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

#ifndef _BBVWM_H_
#define _BBVWM_H_

namespace bbcore {

struct winlist;

//=========================================================
// Init/exit

/// \brief Gets configuration options
void vwm_init(void);

void vwm_reconfig(bool force);
void vwm_exit(void);

//=========================================================
// update the list

/// \brief Updates cached info about existing windows
void vwm_update_winlist(void);

/// \brief Adds info about window to info cache if not present
/// \returns A pointer to found or newly created cached info entry
winlist* vwm_add_window(HWND hwnd);

//=========================================================
// set workspace

void vwm_switch(int newdesk);
void vwm_gather(void);

//=========================================================
// Set window properties

/// \brief Sets desktop for given window
/// \param hwnd The provided window
/// \param desk Desktop number
/// \param switchto Whether is need to switch to given desktop
/// \return Whether desktop is set for window
bool vwm_set_desk(HWND hwnd, int desk, bool switchto);

/// \brief Sets desktop location for window and moves it
bool vwm_set_location(HWND hwnd, taskinfo* t, unsigned flags);

/// \brief Sets in cached info about given window the new 'sticky' value.
///        If cache info not found it created
bool vwm_set_sticky(HWND hwnd, bool value);

bool vwm_lower_window(HWND hwnd);

// Workaround for BBPager:
bool vwm_set_workspace(HWND hwnd, int desk);

//=========================================================
// status infos about windows

/// \brief Tries to get the number of desktop that given window is associated.
///        If no association found returns number of current screen.
int vwm_get_desk(HWND hwnd);

bool vwm_get_location(HWND hwnd, taskinfo* t);

bool vwm_get_status(HWND hwnd, int what);
// values for "what":
#define VWM_MOVED 1
#define VWM_HIDDEN 2
#define VWM_STICKY 3
#define VWM_ICONIC 4


}; //!namespace bbcore

//=========================================================
#endif //def _BBVWM_H_
