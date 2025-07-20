
#include <BB.h>
#include <bbshell.h>
#include <Settings.h>

#include "ShellContext.h"

namespace bbcore { 

ShellContext::ShellContext(BOOL* result, LPCITEMIDLIST pidl)
{
    *result = FALSE;
    if (sh_get_uiobject(pidl, &pidlFull, &pidlItem, &psfFolder, IID_IContextMenu, (void**)&pContextMenu))
    {
        HRESULT hr;
        hMenu = CreatePopupMenu();
        hr = pContextMenu->QueryContextMenu(
            hMenu, 
            0, MIN_SHELL_ID, MAX_SHELL_ID,
            CMF_EXPLORE|CMF_CANRENAME//|CMF_EXTENDEDVERBS
            );

        if (SUCCEEDED(hr))
        {
            *result = TRUE;
        }
    }
}

ShellContext::~ShellContext(void)
{
    if (hMenu) 
    {
        DestroyMenu(hMenu);
    }
    if (pContextMenu) 
    {
        pContextMenu->Release();
    }
    if (psfFolder) 
    {
        psfFolder->Release();
    }
    freeIDList(pidlItem);
    freeIDList(pidlFull);
}

void ShellContext::Invoke(int nCmd)
{
    HRESULT hr = S_OK;

    if (19 == nCmd) // rename
    {
        char oldName[100];
        char newName[100];
        WCHAR newNameW[100];
        sh_getnameof(psfFolder, pidlItem, SHGDN_NORMAL, oldName, isUsingUtf8Encoding());
        if (IDOK == EditBox(BBAPPNAME, "Enter new name:", oldName, newName))
        {
            MultiByteToWideChar (CP_ACP, 0, newName, -1, newNameW, array_count(newNameW));
            hr = psfFolder->SetNameOf(NULL, (LPCITEMIDLIST)pidlItem, newNameW, SHGDN_NORMAL, NULL);
        }
    }
    else if (nCmd >= MIN_SHELL_ID)
    {
        CMINVOKECOMMANDINFO ici{};
        ici.cbSize          = sizeof(ici);
        ici.fMask           = 0;//CMIC_MASK_FLAG_NO_UI;
        ici.hwnd            = NULL;
        ici.lpVerb          = (LPCSTR)(ULONG_PTR)(nCmd - MIN_SHELL_ID);
        ici.lpParameters    = NULL;
        ici.lpDirectory     = NULL;
        ici.nShow           = SW_SHOWNORMAL;
        ici.dwHotKey        = 0;
        ici.hIcon           = NULL;
        hr = pContextMenu->InvokeCommand(&ici);
    }

    if (0 == SUCCEEDED(hr))
    {
        ;//MessageBeep(MB_OK);
    } 
}

/// \brief Decrements counter. When coutner hits 0 deletes self
void ShellContext::decref(void)
{
    if (0 == --refc)
    {
        delete this;
    }
}

void ShellContext::addref(void)
{
    ++refc;
}

HRESULT ShellContext::HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LPCONTEXTMENU2 p = (LPCONTEXTMENU2)this->pContextMenu;
    return COMCALL3(p, HandleMenuMsg, uMsg, wParam, lParam);
}

HMENU ShellContext::menuHandle(void)
{
    return hMenu;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This below is for the normal windows context menu

WNDPROC         ShellContext::g_pOldWndProc; // regular window proc
LPCONTEXTMENU2  ShellContext::g_pIContext2;  // active shell context menu

int ShellContext::ShellMenu(void)
{
    HWND hwnd = gBBhwnd;
    POINT point;
    int nCmd;

    GetCursorPos(&point);

    g_pIContext2  = (LPCONTEXTMENU2)pContextMenu;
    g_pOldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)HookWndProc);

    nCmd = TrackPopupMenu (
        hMenu,
        TPM_LEFTALIGN
        | TPM_LEFTBUTTON
        | TPM_RIGHTBUTTON
        | TPM_RETURNCMD,
        point.x, point.y, 0, hwnd, NULL);

    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)g_pOldWndProc);
    return nCmd;
}

LRESULT CALLBACK ShellContext::HookWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg)
   { 
       case WM_DRAWITEM:
       case WM_MEASUREITEM:
            if (wParam) break; // not menu related
            g_pIContext2->HandleMenuMsg(msg, wParam, lParam);
            return TRUE; // handled

       case WM_INITMENUPOPUP: // this will fill the "open with" / "send to" menus
            g_pIContext2->HandleMenuMsg(msg, wParam, lParam);
            return 0;
   }
   return CallWindowProc(g_pOldWndProc, hWnd, msg, wParam, lParam);
}

}; //!namespace bbcore
