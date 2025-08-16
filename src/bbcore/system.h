#ifndef _BBCORE_SYSTEM_H
#define _BBCORE_SYSTEM_H


namespace bbcore { 

/* Public flags for SnapWindowToEdge */
#define SNAP_FULLSCREEN 1  /* use full screen rather than workarea */
#define SNAP_NOPLUGINS  2 /* dont snap to other plugins */
#define SNAP_SIZING     4 /* window is resized (bottom-right sizing only) */

/* Private flags for SnapWindowToEdge */
#define SNAP_NOPARENT   8  /* dont snap to parent window edges */
#define SNAP_NOCHILDS  16  /* dont snap to child windows */
#define SNAP_TRIGGER   32  /* apply nTriggerDist instead of default */
#define SNAP_PADDING   64  /* Next arg points to the padding value */
#define SNAP_CONTENT  128  /* Next arg points to a SIZE struct */ 


bool bbGetMultimon(void);
void bbSetMultimon(bool);

HMONITOR getMonitorRect(void *from, RECT *r, int flags);
bool setTransparency(HWND hwnd, BYTE alpha);

int getAppByWindow(HWND hwnd, char* processName);
bool isAppWindow(HWND hwnd);

void setDesktopMargin(HWND hwnd, int location, int margin);

void replaceEnvVars(char* str);

char* replaceShellFolders(char* str);

const char* getBBVersion(void);
char* WINAPI getBlackboxPath(char* pszPath, int nMaxLen);

BOOL bbExecute(
    HWND Owner,
    const char* szVerb,
    const char* szFile,
    const char* szArgs,
    const char* szDirectory,
    int nShowCmd,
    int flags);

void snapWindowToEdge(WINDOWPOS* wp, LPARAM nDist, UINT flags, ...);
void snapWindowToEdgeV(WINDOWPOS* wp, LPARAM nDist, UINT flags, va_list vlist); 

// ==============================================================
/* BBApi.cpp - some (non api) utils */

/// \brief Executes application with given name and optional run parameters.
/// Possible optional run parameters:
/// -hidden : run in a hidden window
/// -in <path> ; specify working directory
/// -workspace1 : specify workspace
/// 
/// Possible flags (can be OR'ed):
/// RUN_SHOWERRORS
/// RUN_NOERRORS
/// RUN_WAIT
/// RUN_HIDDEN
/// RUN_NOARGS
/// RUN_NOSUBST
/// RUN_ISPIDL
/// RUN_WINDIR
BOOL BBExecute_string(const char *s, int flags);

int BBExecute_pidl(const char* verb, const void *pidl);


} // !namespace bbcore

#endif //!_BBCORE_SYSTEM_H
