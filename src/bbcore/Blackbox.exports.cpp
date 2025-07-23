
#include <stdexcept> 
#include <windows.h>
#include <mmsystem.h>
#include <process.h>
#include <tlhelp32.h>
#include <locale.h>

#include "Blackbox.h"
#include "BB.h"
#include "MessageManager.h"
#include "PluginManager.h"
#include "Workspaces.h"
#include "Desk.h"
#include "Tray.h"
#include "Toolbar.h"
#include "Menu.h"
#include "Menu/MenuMaker.h"
#include "BBSendData.h"
#include "BImage.h"
#include "bbshell.h"
#include "bbrc.h"
#include "nls.h"
#include "Utils.h"
#include "system.h"
#include "rcfile.h"
#include "commands.h" 
#include "ConfigMenu.h"
#include "DesktopMenu.h"
#include "parse.h"

namespace bbcore { 

// Exports:

bool Blackbox::isUsingUtf8Encoding(void)
{
    return bbcore::isUsingUtf8Encoding();
}

bool Blackbox::read_bool(const char* fileName, const char* szKey, bool bDefault)
{
    return ::read_bool(fileName, szKey, bDefault);
}

int Blackbox::read_int(const char* fileName, const char* szKey, int nDefault)
{
    return ::read_int(fileName, szKey, nDefault);
}

COLORREF Blackbox::readColor(const char* fileName, const char* szKey, const char* defaultColor)
{
    return bbcore::readColor(fileName, szKey, defaultColor);
}

const char* Blackbox::read_string(const char* fileName, const char* szKey, const char* szDefault)
{
    return ::read_string(fileName, szKey, szDefault);
} 

const char* Blackbox::read_value(const char* path, const char* szKey, long* ptr)
{
    return ::read_value(path, szKey, ptr);
}

void Blackbox::write_value(const char* path, const char* szKey, const char* value)
{
    return ::write_value(path, szKey, value);
} 

int Blackbox::found_last_value(void)
{
    return ::found_last_value();
}

void Blackbox::write_bool(const char* fileName, const char* szKey, bool value)
{
    return ::write_bool(fileName, szKey, value);
}

void Blackbox::write_int(const char* fileName, const char* szKey, int value)
{
    return ::write_int(fileName, szKey, value);
}

void Blackbox::write_string(const char* fileName, const char* szKey, const char* value)
{
    return ::write_string(fileName, szKey, value);
}

void Blackbox::write_color(const char* fileName, const char* szKey, COLORREF value)
{
    return ::write_color(fileName, szKey, value);
}

bool Blackbox::deleteSetting(LPCSTR path, LPCSTR szKey)
{
    return bbcore::deleteSetting(path, szKey);
} 

bool Blackbox::renameSetting(const char* path, const char* szKey, const char* new_keyword)
{
    return bbcore::renameSetting(path, szKey, new_keyword);
} 

void* Blackbox::getSettingPtr(int sn_index)
{
    return bbcore::getSettingPtr(sn_index);
} 

int Blackbox::readStyleItem( const char* fileName, const char* szKey,
    StyleItem* pStyleItemOut, StyleItem* pStyleItemDefault)
{
    return bbcore::readStyleItem(fileName, szKey, pStyleItemOut, pStyleItemDefault);
} 

char* Blackbox::getBlackboxPath(char* pszPath, int nMaxLen)
{
    return bbcore::getBlackboxPath(pszPath, nMaxLen);
}

bool Blackbox::findRCFile(char* pszOut, const char* filename, HINSTANCE module)
{
    return bbcore::findRCFile(pszOut, filename, module);
}

const char* Blackbox::bbRcPath(const char* other)
{
    return bbcore::bbRcPath(other);
}

const char* Blackbox::bbExtensionsRcPath(const char* other)
{
    return bbcore::bbExtensionsRcPath(other);
}

const char* Blackbox::bbMenuPath(const char* other)
{
    return bbcore::bbMenuPath(other);
}

const char* Blackbox::bbPluginsRcPath(const char* other)
{
    return bbcore::bbPluginsRcPath(other);
}

const char* Blackbox::bbStylePath(const char* other)
{
    return bbcore::bbStylePath(other);
}

const char* Blackbox::bbDefaultrcPath(void)
{
    return bbcore::bbDefaultrcPath();
}

const char* Blackbox::bbConfigFileExists(const char* filename, const char* pluginDir)
{
    return bbcore::bbConfigFileExists(filename, pluginDir);
}

void Blackbox::getBlackboxEditor(char* editor)
{
    return bbcore::getBlackboxEditor(editor);
}

bool Blackbox::file_exists(const char* szFileName)
{
    return ::file_exists(szFileName);
}

FILE* Blackbox::fileOpen(const char* szPath)
{
    return bbcore::fileOpen(szPath);
}

bool Blackbox::fileClose(FILE *fp)
{
    return bbcore::fileClose(fp);
}

bool Blackbox::fileRead(FILE *fp, char* buffer)
{
    return bbcore::fileRead(fp, buffer);
}

bool Blackbox::readNextCommand(FILE *fp, char* szBuffer, unsigned dwLength)
{
    return bbcore::readNextCommand(fp, szBuffer, dwLength);
}

void Blackbox::makeSticky(HWND hwnd)
{
    return bbcore::makeSticky(hwnd);
}

void Blackbox::removeSticky(HWND hwnd)
{
    return bbcore::removeSticky(hwnd);
}

bool Blackbox::checkSticky(HWND hwnd)
{
    return bbcore::checkSticky(hwnd);
}

HMONITOR Blackbox::getMonitorRect(void *from, RECT *r, int flags)
{
    return bbcore::getMonitorRect(from, r, flags);
}

void Blackbox::SnapWindowToEdge(WINDOWPOS* wp, LPARAM nDist, UINT flags, ...)
{
    va_list vlist{}; 
    va_start(vlist, flags);
    this->snapWindowToEdgeV(wp, nDist, flags, vlist);
    va_end(vlist);
}

void Blackbox::snapWindowToEdgeV(WINDOWPOS* wp, LPARAM nDist, UINT flags, va_list vlist)
{
    bbcore::snapWindowToEdgeV(wp, nDist, flags, vlist);
}

bool Blackbox::setTransparency(HWND hwnd, BYTE alpha)
{
    return bbcore::setTransparency(hwnd, alpha);
}

int Blackbox::getAppByWindow(HWND hwnd, char* processName)
{
    return bbcore::getAppByWindow(hwnd, processName);
}

bool Blackbox::isAppWindow(HWND hwnd)
{
    return bbcore::isAppWindow(hwnd);
}

void Blackbox::setDesktopMargin(HWND hwnd, int location, int margin)
{
    return bbcore::setDesktopMargin(hwnd, location, margin);
}

HWND Blackbox::getBBWnd(void)
{
    return bbcore::getBBWnd();
}

const char* Blackbox::getBBVersion(void)
{
    return bbcore::getBBVersion();
}

bool Blackbox::getUnderExplorer(void)
{
    return bbcore::getUnderExplorer();
}

LPCSTR Blackbox::getOSInfo(void)
{
    return bbcore::getOSInfo();
} 

const char* Blackbox::tokenize(const char* string, char* buf, const char* delims)
{
    return bbcore::tokenize(string, buf, delims);
}

int Blackbox::bbTokenize(const char* srcString, char **lpszBuffers,
    unsigned dwNumBuffers, char* szExtraParameters)
{
    return bbcore::bbTokenize(srcString, lpszBuffers, dwNumBuffers, szExtraParameters);
}

void Blackbox::parse_item(const char* szItem, StyleItem* item)
{
    ::parse_item(szItem, item);
} 

bool Blackbox::isInString(const char* inputString, const char* searchString)
{
    return bbcore::isInString(inputString, searchString);
}

char* Blackbox::strRemoveEncap(char* str)
{
    return bbcore::strRemoveEncap(str);
}

void Blackbox::replaceEnvVars(char* str)
{
    return bbcore::replaceEnvVars(str);
}

char* Blackbox::replaceShellFolders(char* str)
{
    return bbcore::replaceShellFolders(str);
}

BOOL Blackbox::bbExecute( HWND Owner, const char* szVerb,
    const char* szFile, const char* szArgs,
    const char* szDirectory, int nShowCmd, int flags)
{
    return bbcore::bbExecute(Owner, szVerb, szFile, szArgs, szDirectory, nShowCmd, flags);
}

int Blackbox::bbMessageBox(int flg, const char *fmt, ...)
{
    va_list args; 
    va_start(args, fmt);
    auto result = this->bbMessageBoxV(flg, fmt, args);
    va_end(args);

    return result;
}

int Blackbox::bbMessageBoxV(int flg, const char* fmt, va_list args)
{
    return bbcore::bbMessageBoxV(flg, fmt, args); 
}

void Blackbox::dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    this->debug_vprintf(fmt, args);
    va_end(args);
}

void Blackbox::debug_vprintf(const char* format, va_list vlist)
{
    return ::debug_vprintf(format, vlist);
} 

void Blackbox::log(const char* Title, const char* Line)
{
    return bbcore::log(Title, Line);
}

int Blackbox::mboxErrorFile(const char* szFile)
{
    return bbcore::mboxErrorFile(szFile);
}

int Blackbox::mboxErrorValue(const char* szValue)
{
    return bbcore::mboxErrorValue(szValue);
} 

void Blackbox::makeGradient(
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

void Blackbox::makeStyleGradient(HDC hdc, RECT* rp, StyleItem* pSI, bool withBorder)
{
    return bbcore::makeStyleGradient(hdc, rp, pSI, withBorder);
}

void Blackbox::createBorder(HDC hdc, RECT* rp, COLORREF borderColor, int borderWidth)
{
    return bbcore::createBorder(hdc, rp, borderColor, borderWidth);
}

void Blackbox::bbDrawPixImpl(HDC hDC, RECT *rc, COLORREF color, int pic)
{
    return bbcore::bbDrawPixImpl(hDC, rc, color, pic);
}

HFONT Blackbox::createStyleFont(StyleItem* pSI)
{
    return bbcore::createStyleFont(pSI);
}

void Blackbox::bbDrawTextImpl(HDC hDC, const char* text, RECT* p_rect, unsigned format, COLORREF c)
{
    return bbcore::bbDrawTextImpl(hDC, text, p_rect, format, c);
}

int Blackbox::bbMbyteToWideChar(const char *src, WCHAR *wstr, int len)
{
    return bbcore::bbMbyteToWideChar(src, wstr, len);
}

int Blackbox::bbWideCharToMbyte(const WCHAR *src, char *str, int len)
{
    return bbcore::bbWideCharToMbyte(src, str, len);
}

Menu* Blackbox::makeNamedMenu(const char* HeaderText, const char* IDString, bool popup)
{
    return bbcore::makeNamedMenu(HeaderText, IDString, popup);
}

MenuItem* Blackbox::makeMenuItem(Menu *PluginMenu, const char* Title, const char* Cmd, bool ShowIndicator)
{
    return bbcore::makeMenuItem(PluginMenu, Title, Cmd, ShowIndicator);
}

MenuItem* Blackbox::makeMenuNOP(Menu *PluginMenu, const char* Title)
{
    return bbcore::makeMenuNOP(PluginMenu, Title);
}

MenuItem* Blackbox::makeMenuItemInt(Menu* PluginMenu, const char* Title, const char* Cmd,
    int val, int minval, int maxval)
{
    return bbcore::makeMenuItemInt(PluginMenu, Title, Cmd, val, minval, maxval);
}

MenuItem* Blackbox::makeMenuItemString(Menu* PluginMenu, const char* Title, const char* Cmd,
    const char* init_string)
{
    return bbcore::makeMenuItemString(PluginMenu, Title, Cmd, init_string);
}

MenuItem* Blackbox::makeSubmenu(Menu* ParentMenu, Menu* ChildMenu, const char* Title)
{
    return bbcore::makeSubmenu(ParentMenu, ChildMenu, Title);
} 

MenuItem* Blackbox::makeMenuItemPath(Menu* ParentMenu, const char* Title, const char* path, const char* Cmd)
{
    return bbcore::makeMenuItemPath(ParentMenu, Title, path, Cmd);
}

Menu* Blackbox::makeContextMenu(const char* path, const void* pidl)
{
    return bbcore::makeContextMenu(path, pidl);
}

void Blackbox::showMenu(Menu* pluginMenu)
{
    return bbcore::showMenu(pluginMenu);
} 

bool Blackbox::menuExists(const char* IDString_part)
{
    return bbcore::menuExists(IDString_part);
} 

void Blackbox::menuItemOption(MenuItem *pItem, int option, ...)
{
    if (nullptr == pItem)
    {
        return;
    }

    va_list vl{};
    va_start(vl, option);
    this->menuItemOptionV(pItem, option, vl);
    va_end(vl);
} 

void Blackbox::menuItemOptionV(MenuItem* pItem, int option, va_list vl)
{
    return bbcore::menuItemOptionV(pItem, option, vl); 
}

void Blackbox::menuOption(Menu *pMenu, int flags, ...)
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

void Blackbox::menuOptionV(Menu* pMenu, int flags, va_list vl)
{
    return bbcore::menuOptionV(pMenu, flags, vl); 
}

Menu* Blackbox::makeMenu(const char* headerText)
{
    return bbcore::makeMenu(headerText);
} 

void Blackbox::delMenu(Menu* pluginMenu)
{
    return bbcore::delMenu(pluginMenu);
} 

int Blackbox::getTraySize(void)
{
    return bbcore::getTraySize();
}

systemTray* Blackbox::getTrayIcon(int icon_index)
{
    return bbcore::getTrayIcon(icon_index);
}

int Blackbox::forwardTrayMessage(int icon_index, UINT message, systemTrayIconPos* pos)
{
    return bbcore::forwardTrayMessage(icon_index, message, pos);
}

int Blackbox::getTaskListSize(void)
{
    return bbcore::getTaskListSize();
}

HWND Blackbox::getTask(int index)
{
    return bbcore::getTask(index);
}

int Blackbox::getActiveTask(void)
{
    return bbcore::getActiveTask();
}

int Blackbox::getTaskWorkspace(HWND hwnd)
{
    return bbcore::getTaskWorkspace(hwnd);
}

void Blackbox::setTaskWorkspace(HWND hwnd, int wkspc)
{
    return bbcore::setTaskWorkspace(hwnd, wkspc);
}

const struct tasklist* Blackbox::getTaskListPtr(void)
{
    return bbcore::getTaskListPtr();
} 

void Blackbox::getDesktopInfo(DesktopInfo* deskInfo)
{
    return bbcore::getDesktopInfo(deskInfo);
}

bool Blackbox::getTaskLocation(HWND hwnd, struct taskinfo* t)
{
    return bbcore::getTaskLocation(hwnd, t);
}

bool Blackbox::setTaskLocation(HWND hwnd, struct taskinfo* t, UINT flags)
{
    return bbcore::setTaskLocation(hwnd, t, flags);
}

ToolbarInfo* Blackbox::getToolbarInfo(void)
{
    return bbcore::getToolbarInfo();
}


} // !namespace bbcore
