
#include <bblib.h>
#include <bbrc.h>
#include <bbcore.h>
#include <BBApi.h>

//===========================================================================
// API: ReadBool
//===========================================================================

bool ReadBool(const char* fileName, const char* szKey, bool bDefault)
{
    return read_bool(fileName, szKey, bDefault);
}

//===========================================================================
// API: ReadInt
//===========================================================================

int ReadInt(const char* fileName, const char* szKey, int nDefault)
{
    return read_int(fileName, szKey, nDefault);
}

//===========================================================================
// API: ReadColor
//===========================================================================

COLORREF ReadColor(const char* fileName, const char* szKey, const char* defaultColor)
{
    return bbcore::readColor(fileName, szKey, defaultColor);
}

//===========================================================================
// API: ReadString
//===========================================================================

const char* ReadString(const char* fileName, const char* szKey, const char* szDefault)
{
    return read_string(fileName, szKey, szDefault);
} 

//===========================================================================
// API: ReadValue
//===========================================================================

const char* ReadValue(const char* path, const char* szKey, long *ptr)
{
    return read_value(path, szKey, ptr);
}

//===========================================================================
// API: WriteValue
//===========================================================================
void WriteValue(const char* path, const char* szKey, const char* value)
{
    write_value(path, szKey, value);
} 

//===========================================================================
// API: FoundLastValue
//===========================================================================
int FoundLastValue(void)
{
    return found_last_value();
}

//===========================================================================
// API: WriteBool
//===========================================================================

void WriteBool(const char* fileName, const char* szKey, bool value)
{
    write_bool(fileName, szKey, value);
}

//===========================================================================
// API: WriteInt
//===========================================================================

void WriteInt(const char* fileName, const char* szKey, int value)
{
    return write_int(fileName, szKey, value);
}

//===========================================================================
// API: WriteString
//===========================================================================

void WriteString(const char* fileName, const char* szKey, const char* value)
{
    return write_string(fileName, szKey, value);
}

//===========================================================================
// API: WriteColor
//===========================================================================

void WriteColor(const char* fileName, const char* szKey, COLORREF value)
{
    return write_color(fileName, szKey, value);
}

//===========================================================================
// API: DeleteSetting
//===========================================================================

bool DeleteSetting(LPCSTR path, LPCSTR szKey)
{
    return bbcore::deleteSetting(path, szKey);
} 

//===========================================================================
// API: RenameSetting
//===========================================================================

bool RenameSetting(const char* path, const char* szKey, const char* new_keyword)
{
    return bbcore::renameSetting(path, szKey, new_keyword);
} 


//===========================================================================
// API: FindRCFile
// Purpose: Lookup a configuration file
// In:  pszOut = the location where to put the result
// In:  const char* = filename to look for
// In:  HINSTANCE = plugin module handle or NULL
// Out: bool = true of found, FALSE otherwise
//===========================================================================

bool FindRCFile(char* pszOut, const char* filename, HINSTANCE module)
{
    return bbcore::findRCFile(pszOut, filename, module);
}

//===========================================================================
// API: ConfigFileExists
//===========================================================================

EXTERN_C 
const char* ConfigFileExists(const char* filename, const char* pluginDir)
{
    return bbcore::bbConfigFileExists(filename, pluginDir);
}

//===========================================================================
// API: FileExists
// Purpose: Checks for a files existance
//===========================================================================

bool FileExists(const char* szFileName)
{
    return file_exists(szFileName);
}

//===========================================================================
// API: FileOpen
// Purpose: Opens file for parsing
//===========================================================================

FILE* FileOpen(const char* szPath)
{
    return bbcore::fileOpen(szPath);
}

//===========================================================================
// API: FileClose
// Purpose: Close selected file
//===========================================================================

bool FileClose(FILE *fp)
{
    return bbcore::fileClose(fp);
}

//===========================================================================
// API: FileRead
// Purpose: Read's a line from given FILE and returns boolean on status
//===========================================================================

bool FileRead(FILE *fp, char* buffer)
{
    return bbcore::fileRead(fp, buffer);
}

//===========================================================================
// API: ReadNextCommand
// Purpose: Reads the next line of the file
//===========================================================================

bool ReadNextCommand(FILE *fp, char* szBuffer, unsigned dwLength)
{
    return bbcore::readNextCommand(fp, szBuffer, dwLength);
}

//===========================================================================
// API: GetMonitorRect
//===========================================================================

/* Flags: */
#define GETMON_FROM_WINDOW 1    /* 'from' is HWND */
#define GETMON_FROM_POINT 2     /* 'from' is POINT* */
#define GETMON_FROM_MONITOR 4   /* 'from' is HMONITOR */
#define GETMON_WORKAREA 16      /* get working area rather than full screen */

HMONITOR GetMonitorRect(void *from, RECT *r, int flags)
{
    return bbcore::getMonitorRect(from, r, flags);
}

//===========================================================================
// API: SnapWindowToEdge
// Purpose:Snaps a given windowpos at a specified distance
// In: WINDOWPOS* = WINDOWPOS recieved from WM_WINDOWPOSCHANGING
// In: int = distance to snap to
// In: bool = use screensize of workspace
// it's seems to be that ellipsis used only for interpreting '4th' arg based on
// function logic and flags
//===========================================================================

//[> Public flags for SnapWindowToEdge <]
//#define SNAP_FULLSCREEN 1  [> use full screen rather than workarea <]
//#define SNAP_NOPLUGINS  2 [> dont snap to other plugins <]
//#define SNAP_SIZING     4 [> window is resized (bottom-right sizing only) <]

//[> Private flags for SnapWindowToEdge <]
//#define SNAP_NOPARENT   8  [> dont snap to parent window edges <]
//#define SNAP_NOCHILDS  16  [> dont snap to child windows <]
//#define SNAP_TRIGGER   32  [> apply nTriggerDist instead of default <]
//#define SNAP_PADDING   64  [> Next arg points to the padding value <]
//#define SNAP_CONTENT  128  [> Next arg points to a SIZE struct <]

void SnapWindowToEdge(WINDOWPOS* wp, LPARAM nDist, UINT flags, ...)
{
    va_list va{}; 
    va_start(va, flags);
    bbcore::snapWindowToEdgeV(wp, nDist, flags, va);
    va_end(va);
}

//===========================================================================
// API: SetTransparency
// Purpose: Wrapper
//=========================================================================== 

bool SetTransparency(HWND hwnd, BYTE alpha)
{
    return bbcore::setTransparency(hwnd, alpha);
}

//===========================================================================
// API: GetAppByWindow
// Purpose:
// In:
// Out:
//===========================================================================

int GetAppByWindow(HWND hwnd, char* processName)
{
    return bbcore::getAppByWindow(hwnd, processName);
}

//===========================================================================
// API: IsAppWindow
// Purpose: checks given hwnd to see if it's an app
// This is used to populate the task list in case bb is started manually.
//===========================================================================

bool IsAppWindow(HWND hwnd)
{
    return bbcore::isAppWindow(hwnd);
}

//===========================================================================
// API: SetDesktopMargin
// Purpose:  Set a margin for e.g. toolbar, bbsystembar, etc
// In:       hwnd to associate the margin with, location, margin-width
//=========================================================================== 

void SetDesktopMargin(HWND hwnd, int location, int margin)
{
    return bbcore::setDesktopMargin(hwnd, location, margin);
}

//===========================================================================
// API: Tokenize
// Purpose: Put first token of 'string' (seperated by one of delims)
// Returns: pointer to the rest into 'string'
//===========================================================================

const char* Tokenize(const char* string, char* buf, const char* delims)
{
    return bbcore::tokenize(string, buf, delims);
}

//===========================================================================
// API: BBTokenize
// Purpose: Assigns a specified number of string variables and outputs the
//      remaining to a variable
//===========================================================================

int BBTokenize(
    const char* srcString,
    char **lpszBuffers,
    unsigned dwNumBuffers,
    char* szExtraParameters)
{
    return bbcore::bbTokenize(srcString, lpszBuffers, dwNumBuffers, szExtraParameters);
}

//===========================================================================
// API: ParseItem
// Purpose: parses a given string and assigns settings to a StyleItem class
//===========================================================================

void ParseItem(const char* szItem, StyleItem *item)
{
    parse_item(szItem, item);
} 

//===========================================================================
// API: IsInString
// Purpose: Checks a given string to an occurance of the second string
//===========================================================================

bool IsInString(const char* inputString, const char* searchString)
{
    return bbcore::isInString(inputString, searchString);
}

//===========================================================================
// API: StrRemoveEncap
// Purpose: Removes the first and last characters of a string
//===========================================================================

char* StrRemoveEncap(char* str)
{
    return bbcore::strRemoveEncap(str);
}

//===========================================================================
// API: ReplaceEnvVars
// Purpose: parses a given string and replaces all %VAR% with the environment
//          variable value if such a value exists
//===========================================================================

void ReplaceEnvVars(char* str)
{
    return bbcore::replaceEnvVars(str);
}

//===========================================================================
// API: ReplaceShellFolders
// Purpose: replace shell folders in a string path, like DESKTOP\...
//===========================================================================

char* ReplaceShellFolders(char* str)
{
    return bbcore::replaceShellFolders(str);
}

//===========================================================================
// API: BBExecute
// Purpose: A safe execute routine
// In: HWND = owner
// In: const char* = operation (eg. "open")
// In: const char* = command to run
// In: const char* = arguments
// In: const char* = directory to run from
// In: int = show status
// In: bool = suppress error messages
// Out: BOOL TRUE on success
//===========================================================================

#ifndef BBTINY
BOOL BBExecute(
    HWND Owner,
    const char* szVerb,
    const char* szFile,
    const char* szArgs,
    const char* szDirectory,
    int nShowCmd,
    int flags)
{
    return bbcore::bbExecute(Owner, szVerb, szFile, szArgs, szDirectory, nShowCmd, flags);
}
#endif

//===========================================================================
// API: Log
// Purpose: Appends given line to Blackbox.log file
//===========================================================================

void Log(const char* Title, const char* Line)
{
    return bbcore::log(Title, Line);
}

//===========================================================================
// API: MBoxErrorFile
// Purpose: Gives a message box proclaming missing file
//===========================================================================

int MBoxErrorFile(const char* szFile)
{
    return bbcore::mboxErrorFile(szFile);
}

//===========================================================================
// API: MBoxErrorValue
// Purpose: Gives a message box proclaming a given value
//===========================================================================

int MBoxErrorValue(const char* szValue)
{
    return bbcore::mboxErrorValue(szValue);
} 

//===========================================================================
// API: IsUsingUtf8Encoding - whether using utf-8 encoding on char string
// conversions
//===========================================================================
bool IsUsingUtf8Encoding(void)
{
    return bbcore::isUsingUtf8Encoding();
}

//===========================================================================
// API: GetSettingPtr - retrieve a pointer to a setting var/struct
//===========================================================================

void* GetSettingPtr(int sn_index)
{
    return bbcore::getSettingPtr(sn_index);
} 

//===========================================================================
// API: ReadStyleItem
//===========================================================================

int ReadStyleItem(
    const char* fileName,
    const char* szKey,
    StyleItem* pStyleItemOut,
    StyleItem* pStyleItemDefault
    )
{
    return bbcore::readStyleItem(fileName, szKey, pStyleItemOut, pStyleItemDefault);
} 

//===========================================================================
// API: MakeGradient
// Purpose: creates a gradient and fills it with the specified options
// In: HDC = handle of the device context to fill
// In: RECT = rect you wish to fill
// In: int = type of gradient (eg. B_HORIZONTAL)
// In: COLORREF = first color to use
// In: COLORREF = second color to use
// In: bool = whether to interlace (darken every other line)
// In: int = style of bevel (eg. BEVEL_FLAT)
// In: int = position of bevel (eg. BEVEL1)
// In: int = not used
// In: COLORREF = border color
// In: int = width of border around bitmap
//===========================================================================

void MakeGradient(
    HDC hdc,
    RECT rect,
    int type,
    COLORREF Color,
    COLORREF ColorTo,
    bool interlaced,
    int bevelstyle,
    int bevelposition,
    int bevelWidth,
    COLORREF borderColor,
    int borderWidth)
{
    return bbcore::makeGradient(hdc, rect, type, Color, 
            ColorTo, interlaced, bevelstyle, bevelposition, 
            bevelWidth, borderColor, borderWidth);
} 


//===========================================================================
// API: MakeStyleGradient
// Purpose:  Make a gradient from style Item
//===========================================================================

void MakeStyleGradient(HDC hdc, RECT* rp, StyleItem* pSI, bool withBorder)
{
    return bbcore::makeStyleGradient(hdc, rp, pSI, withBorder);
}

//===========================================================================
// API: CreateBorder
//===========================================================================

void CreateBorder(HDC hdc, RECT* rp, COLORREF borderColor, int borderWidth)
{
    return bbcore::createBorder(hdc, rp, borderColor, borderWidth);
}

//===========================================================================
// API: GetBlackboxPath
// Purpose: Copies the path of the Blackbox executable to the specified buffer
//===========================================================================

char* WINAPI GetBlackboxPath(char* pszPath, int nMaxLen)
{
    return bbcore::getBlackboxPath(pszPath, nMaxLen);
}

//===========================================================================
// API: bbrcPath
//===========================================================================

const char* bbrcPath(const char* other)
{
    return bbcore::bbRcPath(other);
}

//===========================================================================
// API: extensionsrcPath
// Purpose: Returns the handle to the extensionsrc file that is being used
//===========================================================================

const char* extensionsrcPath(const char* other)
{
    return bbcore::bbExtensionsRcPath(other);
}

//===========================================================================
// API: menuPath
// Purpose: Returns the handle to the menu file that is being used
//===========================================================================

const char* menuPath(const char* other)
{
    return bbcore::bbMenuPath(other);
}

//===========================================================================
// API: plugrcPath
// Purpose: Returns the handle to the plugins rc file that is being used
//===========================================================================

const char* plugrcPath(const char* other)
{
    return bbcore::bbPluginsRcPath(other);
}

//===========================================================================
// API: stylePath
// Purpose: Returns the handle to the style file that is being used
//===========================================================================

const char* stylePath(const char* other)
{
    return bbcore::bbStylePath(other);
}

//===========================================================================
// API: defaultrcPath
//===========================================================================
const char* defaultrcPath(void)
{
    return bbcore::bbDefaultrcPath();
}

//===========================================================================
// API: GetBlackboxEditor
//===========================================================================

void GetBlackboxEditor(char* editor)
{
    return bbcore::getBlackboxEditor(editor);
}

//===========================================================================
// API: GetBBWnd
// Purpose: Returns the handle to the main Blackbox window
//===========================================================================

HWND GetBBWnd(void)
{
    return bbcore::getBBWnd();
}

//===========================================================================
// API: GetBBVersion
// Purpose: Returns the current version
// In: None
// Out: const char* = Formatted Version String
//===========================================================================

const char* GetBBVersion(void)
{
    return bbcore::getBBVersion();
}

//===========================================================================
// API: GetUnderExplorer
//===========================================================================

bool GetUnderExplorer(void)
{
    return bbcore::getUnderExplorer();
}

//===========================================================================
// API: GetOSInfo
//===========================================================================

LPCSTR GetOSInfo(void)
{
    return bbcore::getOSInfo();
} 

//===========================================================================
// API: MakeContextMenu
//===========================================================================

Menu* MakeContextMenu(const char* path, const void* pidl)
{
    return bbcore::makeContextMenu(path, pidl);
}

//===========================================================================
// API: MakeNamedMenu
// Purpose:         Create or refresh a Menu
// In: HeaderText:  the menu title
// In: IDString:    An unique string that identifies the menu window
// In: popup        true: menu is to be shown, false: menu is to be refreshed
// Out: Menu *:     A pointer to a Menu structure (opaque for the client)
// Note:            A menu once it has been created must be passed to
//                  either 'MakeSubMenu' or 'ShowMenu'.
//===========================================================================

Menu* MakeNamedMenu(const char* HeaderText, const char* IDString, bool popup)
{
    return bbcore::makeNamedMenu(HeaderText, IDString, popup);
}

//===========================================================================
// API: MakeMenuItem
//===========================================================================

MenuItem* MakeMenuItem(Menu *PluginMenu, const char* Title, const char* Cmd, bool ShowIndicator)
{
    return bbcore::makeMenuItem(PluginMenu, Title, Cmd, ShowIndicator);
}

//===========================================================================
// API: MakeMenuNOP
//===========================================================================

MenuItem* MakeMenuNOP(Menu *PluginMenu, const char* Title)
{
    return bbcore::makeMenuNOP(PluginMenu, Title);
}

//===========================================================================
// API: MakeMenuItemInt
//===========================================================================

MenuItem* MakeMenuItemInt(Menu* PluginMenu, const char* Title, const char* Cmd,
    int val, int minval, int maxval)
{
    return bbcore::makeMenuItemInt(PluginMenu, Title, Cmd, val, minval, maxval);
}

//===========================================================================
// API: MakeMenuItemString
//===========================================================================

MenuItem* MakeMenuItemString(Menu* PluginMenu, const char* Title, const char* Cmd,
    const char* init_string)
{
    return bbcore::makeMenuItemString(PluginMenu, Title, Cmd, init_string);
}

//===========================================================================
// API: MakeSubmenu
//===========================================================================

MenuItem* MakeSubmenu(Menu* ParentMenu, Menu* ChildMenu, const char* Title)
{
    return bbcore::makeSubmenu(ParentMenu, ChildMenu, Title);
} 

//===========================================================================
// API: MakeMenuItemPath
//===========================================================================

MenuItem* MakeMenuItemPath(Menu* ParentMenu, const char* Title, const char* path, const char* Cmd)
{
    return bbcore::makeMenuItemPath(ParentMenu, Title, path, Cmd);
}

//===========================================================================
// API: ShowMenu
// Purpose: Finalizes creation or refresh for the menu and its submenus
// IN: PluginMenu - pointer to the toplevel menu
//===========================================================================

void ShowMenu(Menu* pluginMenu)
{
    return bbcore::showMenu(pluginMenu);
} 

//===========================================================================
// API: MenuExists
//===========================================================================

bool MenuExists(const char* IDString_part)
{
    return bbcore::menuExists(IDString_part);
} 

//===========================================================================
// API: MenuItemOption - set some options for a menuitem
//===========================================================================

void MenuItemOption(MenuItem *pItem, int option, ...)
{
    if (nullptr == pItem)
    {
        return;
    }

    va_list vl{};
    va_start(vl, option);
    bbcore::menuItemOptionV(pItem, option, vl);
    va_end(vl);
} 

//===========================================================================
// API: MenuOption - set some special features for a individual menu
//===========================================================================

void MenuOption(Menu *pMenu, int flags, ...)
{
    if (NULL == pMenu)
    {
        return;
    }

    va_list vl{}; 
    va_start(vl, flags);
    bbcore::menuOptionV(pMenu, flags, vl); 
    va_end(vl);
}

//===========================================================================
// API: MakeMenu
// Purpose: as above, for menus that dont need refreshing
//===========================================================================
Menu* MakeMenu(const char* headerText)
{
    return bbcore::makeMenu(headerText);
} 

//===========================================================================
// API: DelMenu. Not implemented
// Purpose: obsolete
//===========================================================================

void DelMenu(Menu* pluginMenu)
{
    return bbcore::delMenu(pluginMenu);
} 

//===========================================================================
// API: CreateStyleFont
// Purpose: Create a Font, possible substitutions have been already applied.
//===========================================================================

HFONT CreateStyleFont(StyleItem* pSI)
{
    return bbcore::createStyleFont(pSI);
}

//===========================================================================
// API: GetToolbarInfo
//===========================================================================

ToolbarInfo* GetToolbarInfo(void)
{
    return bbcore::getToolbarInfo();
}

//===========================================================================
// API: GetTraySize
//===========================================================================

int GetTraySize(void)
{
    return bbcore::getTraySize();
}

//===========================================================================
// API: GetTrayIcon
//=========================================================================== 

systemTray* GetTrayIcon(int icon_index)
{
    return bbcore::getTrayIcon(icon_index);
}

//===========================================================================
// API: ForwardTrayMessage
// Reroute the mouse message to the tray icon's host window...
//=========================================================================== 

int ForwardTrayMessage(int icon_index, UINT message, systemTrayIconPos *pos)
{
    return bbcore::forwardTrayMessage(icon_index, message, pos);
}

//===========================================================================
// API: BBMessageBox
// Purpose:  standard BB-MessageBox
//===========================================================================

int BBMessageBox(int flg, const char *fmt, ...)
{
    va_list args; 
    va_start(args, fmt);
    auto result = bbcore::bbMessageBoxV(flg, fmt, args);
    va_end(args);

    return result;
}

//===========================================================================
// API: dbg_printf
//===========================================================================

void dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ::debug_vprintf(fmt, args);
    va_end(args);
}

//===========================================================================
// API: bbDrawPix
//===========================================================================

void bbDrawPix(HDC hDC, RECT *rc, COLORREF color, int pic)
{
    return bbcore::bbDrawPixImpl(hDC, rc, color, pic);
}

//===========================================================================
// API: bbDrawText
//===========================================================================

void bbDrawText(HDC hDC, const char* text, RECT* p_rect, unsigned format, COLORREF c)
{
    return bbcore::bbDrawTextImpl(hDC, text, p_rect, format, c);
}

//===========================================================================
// API: bbMB2WC
//===========================================================================

int bbMB2WC(const char *src, WCHAR *wstr, int len)
{
    return bbcore::bbMbyteToWideChar(src, wstr, len);
}

//===========================================================================
// API: bbWC2MB
//===========================================================================

int bbWC2MB(const WCHAR *src, char *str, int len)
{
    return bbcore::bbWideCharToMbyte(src, str, len);
}

//===========================================================================
// API: SetTaskLocation - move a window and it's popups to another desktop and/or position
//===========================================================================

bool SetTaskLocation(HWND hwnd, struct taskinfo* t, UINT flags)
{
    return bbcore::setTaskLocation(hwnd, t, flags);
}

//===========================================================================
// API: MakeSticky
// API: RemoveSticky
// API: CheckSticky
// Purpose: make a plugin/app window appear on all workspaces
//===========================================================================

// This is now one API for both plugins and application windows,
// still internally uses different methods

//===========================================================================
// API: MakeSticky
//===========================================================================
void MakeSticky(HWND hwnd)
{
    return bbcore::makeSticky(hwnd);
}

//===========================================================================
// API
//===========================================================================
void RemoveSticky(HWND hwnd)
{
    return bbcore::removeSticky(hwnd);
}

//===========================================================================
// API
//===========================================================================
bool CheckSticky(HWND hwnd)
{
    return bbcore::checkSticky(hwnd);
}

//===========================================================================
// API: GetTaskListSize - returns the number of currently registered tasks
//===========================================================================

int GetTaskListSize(void)
{
    return bbcore::getTaskListSize();
}

//===========================================================================
// API: GetTask - returns the HWND of the task by index
//===========================================================================

HWND GetTask(int index)
{
    return bbcore::getTask(index);
}

//===========================================================================
// API: GetActiveTask - returns index of current active task or -1, if none or BB
//===========================================================================

int GetActiveTask(void)
{
    return bbcore::getActiveTask();
}

//===========================================================================
// API: GetTaskWorkspace - returns the workspace of the task by HWND
//===========================================================================

int GetTaskWorkspace(HWND hwnd)
{
    return bbcore::getTaskWorkspace(hwnd);
}

//===========================================================================
// API: SetTaskWorkspace - set the workspace of the task by HWND
//===========================================================================

void SetTaskWorkspace(HWND hwnd, int wkspc)
{
    return bbcore::setTaskWorkspace(hwnd, wkspc);
}

//===========================================================================
// API: GetTaskListPtr - returns the raw task-list
//===========================================================================

const struct tasklist* GetTaskListPtr(void)
{
    return bbcore::getTaskListPtr();
} 

//===========================================================================
// API: GetDesktopInfo
//===========================================================================

void GetDesktopInfo(DesktopInfo* deskInfo)
{
    return bbcore::getDesktopInfo(deskInfo);
}

//===========================================================================
// API: GetTaskLocation - retrieve the desktop and the original coords for a window
//===========================================================================

bool GetTaskLocation(HWND hwnd, struct taskinfo* t)
{
    return bbcore::getTaskLocation(hwnd, t);
}

