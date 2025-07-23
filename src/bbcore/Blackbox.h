#ifndef _BLACKBOX_H
#define _BLACKBOX_H

#include <windows.h>

#include "application.h" 
#include "SettingsBbox.h"

namespace bbcore {

class Blackbox
{
    public:
        Blackbox(HINSTANCE hinst, LPSTR lpCmdLine);
        Blackbox(const Blackbox&) = delete;
        int run(void);

        int messageLoop(void);

        // exports

        // TODO: move all exports to subsystems
        /* Resource File API */
        bool isUsingUtf8Encoding(void);

        /* Read Settings */
        bool read_bool(const char* fileName, const char* szKey, bool bDefault);
        int read_int(const char* fileName, const char* szKey, int nDefault);
        COLORREF readColor(const char* fileName, const char* szKey, const char* defaultColor);
        const char* read_string(const char* fileName, const char* szKey, const char* defaultString);

        /* Read a rc-value as string. 'ptr' is optional, if present, at input indicates the line
           from where the search starts, at output is set to the line that follows the match. */
        const char* read_value(const char* path, const char* szKey, long* ptr);
        void write_value(const char* path, const char* szKey, const char* value);

        /* Note that pointers returned from 'ReadString' and 'ReadValue' are valid only
           until the next Read/Write call. For later usage, you need to copy the string
           into a place within your code. */ 
        /// \brief Returns 0=not found, 1=found exact value, 2=found matching wildcard 
        int found_last_value(void);

        /* Write Settings */
        void write_bool(const char* fileName, const char* szKey, bool value);
        void write_int(const char* fileName, const char* szKey, int value);
        void write_string(const char* fileName, const char* szKey, const char* value);
        void write_color(const char* fileName, const char* szKey, COLORREF value);

        /* Delete rc-entry - wildcards supported for keyword */
        bool deleteSetting(LPCSTR path, LPCSTR szKey);
        /* Rename Setting (or delete with new_keyword=NULL) */
        bool renameSetting(const char* path, const char* szKey, const char* new_keyword);

        /* Direct access to Settings variables / styleitems / colors
           See the "SN_XXX" constants above */
        void* getSettingPtr(int sn_index);

        /* Read an entire StyleItem. This is not meant for the standard style items,
           but for a plugin to read in custom style extensions. Returns nonzero if
           any of the requested properties were read */
        int readStyleItem(
                const char* fileName, /* style filename */
                const char* szKey,  /* keyword until the last dot, e.g. "bbpager.frame" */
                StyleItem* pStyleItemOut, /* pointer to receiving StyleItem struct */
                StyleItem* pStyleItemDefault /* pointer to a default StyleItem or NULL */
                );

        /* ------------------------------------ */
        /* Get paths */

        /* Path where blackbox.exe is (including trailing backslash) */
        char* getBlackboxPath(char* path, int maxLength);

        /*
           Get full Path for a given filename. The file is searched:
           1) If pluginInstance is not NULL: In the directory that contains the DLL
           2) In the blackbox directory.
           Returns true if the file was found.
           If not, 'pszOut' is set to the default location (plugin directory)
           */
        bool findRCFile(char* pszOut, const char* filename, HINSTANCE module);

        /* Get configuration filepaths */
        const char* bbRcPath(const char* bbrcFileName ISNULL);
        const char* bbExtensionsRcPath(const char* other);
        const char* bbMenuPath(const char* other);
        const char* bbPluginsRcPath(const char* other);
        const char* bbStylePath(const char* other);
        const char* bbDefaultrcPath(void);
        const char* bbConfigFileExists(const char* filename, const char* pluginDir);

        /* As configured in exensions.rc: */
        void getBlackboxEditor(char* editor);

        /* ------------------------------------ */
        /* File functions */

        /* Check if 'pszPath' exists as a regular file */
        bool file_exists(const char* szFileName);

        /* Open/Close a file for use with 'FileRead' or 'ReadNextCommand'
           (Do not use 'fopen/fclose' in combination with these) */
        FILE* fileOpen(const char* szPath);
        bool fileClose(FILE *fp);

        /* Read one line from file. Leading and trailing spaces are removed.
           Tabs are converted to spaces. */
        bool fileRead(FILE *fp, char* buffer);

        /* Similar, additionally skips comments and empty lines */
        bool readNextCommand(FILE *fp, char* szBuffer, unsigned dwLength);

        /* ------------------------------------ */
        /* Make a plugin sticky on all workspaces */

        void makeSticky(HWND hwnd);
        void removeSticky(HWND hwnd);
        // Note: Please make sure to call RemoveSticky before your plugin-window is destroyed 
        bool checkSticky(HWND hwnd);

        /* ------------------------------------ */
        /* Window utilities */

        // Flags: one of GETMON_XXX flags
        HMONITOR getMonitorRect(void *from, RECT *r, int flags);

        // Flags: one of SNAP_XXX flags
        void SnapWindowToEdge(WINDOWPOS* wp, LPARAM nDist, UINT flags, ...);
        void snapWindowToEdgeV(WINDOWPOS* wp, LPARAM nDist, UINT flags, va_list vlist);

        // Wrapper for 'SetLayeredWindowAttributes', win9x compatible 
        // alpha: 0-255, 255=transparency off 
        bool setTransparency(HWND hwnd, BYTE alpha);

        /* Get application's name from window, returns lenght of string (0 on failure) */
        int getAppByWindow(HWND hwnd, char* processName);

        /* Is this window considered as an application */
        bool isAppWindow(HWND hwnd);

        /* ------------------------------------ */
        /* Desktop margins: */

        /* Add a screen margin at the indicated location */
        // location is one of 'SetDesktopMargin_locations' values
        // Note: Make sure to remove the margin before your plugin-window
        // is destroyed: SetDesktopMargin(hwnd_plugin, 0, 0); 
        void setDesktopMargin(HWND hwnd, int location, int margin);

        /* ------------------------------------ */
        /* Info */

        /* Get the main window */
        HWND getBBWnd(void);
        /* Version string (as specified in resource.rc, e.g. "bbLean 1.17") */
        const char* getBBVersion(void);
        /* True if running on top of explorer shell: */
        bool getUnderExplorer(void);
        /* Get some string about the OS */
        LPCSTR getOSInfo(void);

        /* ------------------------------------ */
        /* String tokenizing etc. */

        /* Put first token from source into target. Returns: ptr to rest of source */
        const char* tokenize(const char* string, char* buf, const char* delims);

        /* Put first 'numTokens' from 'source' into 'targets', copy rest into
           'remaining', if it's not NULL. The 'targets' and 'remaining' buffers
           are zero-terminated always. Returns the number of actual tokens found */
        int bbTokenize(const char* srcString, char **lpszBuffers,
                unsigned dwNumBuffers, char* szExtraParameters);

        /* Parse a texture specification, e.g. 'raised vertical gradient' */
        void parse_item(const char* szItem, StyleItem* item);
        /* Is searchString part of inputString (letter case ignored) */
        bool isInString(const char* inputString, const char* searchString);
        /* Remove first and last character from string */
        char* strRemoveEncap(char* str);
        /* Replace Environment Variables in string (like %USERNAME%) */
        void replaceEnvVars(char* str);
        /* Replace special folders in string (like APPDATA) */
        char* replaceShellFolders(char* str);

        /* ------------------------------------ */
        /* Shell execute a command */
        BOOL bbExecute( HWND Owner, const char* szVerb,
                const char* szFile, const char* szArgs,
                const char* szDirectory, int nShowCmd, int flags);

        /* ------------------------------------ */
        /* Logging and error messages */

        /* printf-like message box, flag is MB_OK etc */
        int bbMessageBox(int flg, const char *fmt, ...);
        int bbMessageBoxV(int flg, const char* fmt, va_list args);
        /* Pass formatted message to 'OutputDebugString (for plugin developers)' */
        void dbg_printf(const char *fmt, ...);
        void debug_vprintf(const char* format, va_list vlist);
        /* Write to blackbox.log, not implemented in bblean */
        void log(const char* Title, const char* Line);
        /* Complain about missing file */
        int mboxErrorFile(const char* szFile);
        /* Complain about something */
        int mboxErrorValue(const char* szValue);

        /* ------------------------------------ */
        /* Painting */

        /* Generic Gradient Function */
        void makeGradient(
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
                int borderWidth);

        /* Draw a Gradient Rectangle from StyleItem, optional using the style border. */
        void makeStyleGradient(HDC hdc, RECT* rp, StyleItem* pSI, bool withBorder);
        /* Draw a Border */
        void createBorder(HDC hdc, RECT* rp, COLORREF borderColor, int borderWidth);
        /* Draw a Pixmap for buttons, menu bullets, checkmarks ... */
        void bbDrawPixImpl(HDC hDC, RECT *rc, COLORREF color, int pic);
        /* Create a font handle from styleitem, with parsing and substitution. */
        HFONT createStyleFont(StyleItem* pSI);

        /* ------------------------------------ */
        /* UTF-8 support */

        /* Draw a textstring with color, possibly translating from UTF-8 */
        void bbDrawTextImpl(HDC hDC, const char* text, RECT* p_rect, unsigned format, COLORREF c);
        /* convert from Multibyte string to WCHAR */
        int bbMbyteToWideChar(const char *src, WCHAR *wstr, int len);
        /* convert from WCHAR to Multibyte string */
        int bbWideCharToMbyte(const WCHAR *src, char *str, int len);

        /* ------------------------------------ */
        /* Plugin Menu API - See the SDK for application examples */

        /* creates a Menu or Submenu, Id must be unique, fshow indicates whether
           the menu should be shown (true) or redrawn (false) */
        Menu* makeNamedMenu(const char* HeaderText, const char* IDString, bool popup);

        //inserts an item to execute a command or to set a boolean value 
        MenuItem* makeMenuItem(Menu *PluginMenu, const char* Title, const char* Cmd, bool ShowIndicator);

        /* inserts an inactive item, optionally with text. 'Title' may be NULL. */
        MenuItem* makeMenuNOP(Menu *PluginMenu, const char* Title);

        /* inserts an item to adjust a numeric value */
        MenuItem* makeMenuItemInt(Menu* PluginMenu, const char* Title, const char* Cmd,
                int val, int minval, int maxval);

        /* inserts an item to edit a string value */
        MenuItem* makeMenuItemString(Menu* PluginMenu, const char* Title, const char* Cmd,
                const char* init_string);

        /* inserts an item, which opens a submenu */
        MenuItem* makeSubmenu(Menu* ParentMenu, Menu* ChildMenu, const char* Title);

        /* inserts an item, which opens a submenu from a system folder.
           'Cmd' optionally may be a Broam which then is sent on user click
           with "%s" in the broam string replaced by the selected filename */
        MenuItem* makeMenuItemPath(Menu* ParentMenu, const char* Title, const char* path, const char* Cmd);

        /* Context menu for filesystem items. One of path or pidl can be NULL */
        Menu* makeContextMenu(const char* path, const void* pidl);

        /* shows the menu */
        void showMenu(Menu* pluginMenu);

        /* checks whether a menu with ID starting with 'IDString_start', still exists */
        bool menuExists(const char* IDString_part);

        // set option for MenuItem  
        // option - one of BBMENUITEM_XXX values
        void menuItemOption(MenuItem *pItem, int option, ...);
        void menuItemOptionV(MenuItem* pItem, int option, va_list vl);
        // flags - one of BBMENU_XXX values
        void menuOption(Menu *pMenu, int flags, ...);
        void menuOptionV(Menu* pMenu, int flags, va_list vl);

        /* ------------------------------------ */
        /* obsolete: */
        Menu* makeMenu(const char* headerText);
        void delMenu(Menu* pluginMenu);

        /* ------------------------------------ */
        /* Tray icon access */

        /* A plugin can register BB_TRAYUPDATE to receive notification about
           changes with tray icons */ 

        int getTraySize(void);
        systemTray* getTrayIcon(int icon_index);
        int forwardTrayMessage(int icon_index, UINT message, systemTrayIconPos* pos);

        /* ------------------------------------ */
        /* Task items access */

        /* A plugin can register BB_TASKSUPDATE to receive notification about
           changes with tasks */

        /* get the size */
        int getTaskListSize(void);
        /* get a task's HWND by index [0..GetTaskListSize()] */
        HWND getTask(int index);
        /* get the index of the currently active task */
        int getActiveTask(void);
        /* Get the workspace number for a task */
        int getTaskWorkspace(HWND hwnd);
        /* For backwards compatibility only. (because of unclear implementation) */
        void setTaskWorkspace(HWND hwnd, int wkspc);

        /* Direct access: get the internal TaskList. */
        const struct tasklist* getTaskListPtr(void);

        /* ------------------------------------ */
        /* Workspace (aka Desktop) Information */

        /* A plugin can register BB_DESKTOPINFO to receive notification about
           workspace changes */ 

        /* Get current Desktop information: */
        void getDesktopInfo(DesktopInfo* deskInfo);

        /* ------------------------------------ */
        /* windows on workspace placement interface */ 

        /* get workspace and original position/size for window */
        bool getTaskLocation(HWND hwnd, struct taskinfo* t);

        // set workspace and/or position for window 
        // flags here is any combination of BBTI_XXX values
        bool setTaskLocation(HWND hwnd, struct taskinfo* t, UINT flags);

        /* get a pointer to core's static ToolbarInfo */
        ToolbarInfo* getToolbarInfo(void);

    private:
        struct {
            HINSTANCE hInstance;
            LPSTR lpCmdLine;
        } data{};

        SettingsBbox settings{}; 
}; 

}; //!namespace bbcore

#endif //!_BLACKBOX_H
