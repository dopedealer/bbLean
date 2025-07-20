
#include <cstdio>
#include <windows.h>
#include <bblib.h>
#include <bbshell.h>

#include "application.h"
#include "Settings.h"
#include "rcfile.h"

namespace bbcore {

// global variables
bool gUnderExplorer;
HWND gBBhwnd;

/// \brief Get some string about the OS
LPCSTR getOSInfo(void)
{
    static char osinfo_buf[32];
    const char *s;
    OSVERSIONINFO osInfo;

    memset(&osInfo, 0, sizeof(osInfo));
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    GetVersionEx(&osInfo);

    if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
        if (osInfo.dwMinorVersion >= 90)
        {
            s = "ME";
        }
        else if (osInfo.dwMinorVersion >= 10)
        {
            s = "98";
        }
        else
        {
            s = "95";
        }
    }
    else if (osInfo.dwMajorVersion == 5)
    {
        if (osInfo.dwMinorVersion >= 1)
        {
            s = "XP";
        }
        else
        {
            s = "2000";
        }
    }
    else
    {
        s = "NT";
    }
    sprintf(osinfo_buf, "Windows %s", s);
    return osinfo_buf;
}

/// \brief True if running on top of explorer shell:
bool getUnderExplorer(void)
{
    return gUnderExplorer;
}

/// \brief Get the main window
/// \returns The handle to the main Blackbox window 
HWND getBBWnd(void)
{
    if (gBBhwnd == NULL)
    {
        gBBhwnd = get_bbwindow();
    }
    return gBBhwnd;
}

/// \brief As configured in exensions.rc:
void getBlackboxEditor(char* editor)
{
    replace_shellfolders(editor, Settings_preferredEditor, true, bbDefaultrcPath(), isUsingUtf8Encoding());
}

} // !namespace bbcore
