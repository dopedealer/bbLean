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

#ifndef _BBWORKSPACES_H_
#define _BBWORKSPACES_H_

namespace bbcore {

bool setTaskLocation(HWND hwnd, taskinfo* t, UINT flags);
void makeSticky(HWND hwnd);
void removeSticky(HWND hwnd);
bool checkSticky(HWND hwnd);
int getTaskListSize(void);
HWND getTask(int index);
int getActiveTask(void);
int getTaskWorkspace(HWND hwnd);
void setTaskWorkspace(HWND hwnd, int wkspc);
const tasklist* getTaskListPtr(void);
void getDesktopInfo(DesktopInfo* deskInfo);
bool getTaskLocation(HWND hwnd, taskinfo* t);


//===========================================================================
void Workspaces_Init(int nostartup);
void Workspaces_Exit(void);
void Workspaces_Reconfigure(void);
LRESULT Workspaces_Command(UINT msg, WPARAM wParam, LPARAM lParam);
void Workspaces_GatherWindows(void);
bool Workspaces_GetScreenMetrics(void);
void Workspaces_DeskSwitch(int);
void Workspaces_GetCaptions();
void CleanTasks(void);

void Workspaces_TaskProc(WPARAM, HWND);
void Workspaces_handletimer(void);

bool focus_top_window(void);
void SwitchToWindow(HWND hwnd);
void SwitchToBBWnd(void);
HWND GetActiveTaskWindow(void);
void ToggleWindowVisibility(HWND);

// total number of desktops
extern int nScreens;

// current desktop
extern int currentScreen;
extern int lastScreen;

// from workspaces.cpp
bool check_sticky_name(HWND hwnd);
bool check_sticky_plugin(HWND hwnd);
void workspaces_set_desk(void);
void send_desk_refresh(void);
void send_task_refresh(void);

} // !namespace bbcore


#endif //ndef _BBWORKSPACES_H_
