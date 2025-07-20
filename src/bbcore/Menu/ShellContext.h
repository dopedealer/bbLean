#ifndef _BBCORE_SHELLCTX_H
#define _BBCORE_SHELLCTX_H

#include <windows.h>
#include <Shlobj.h>

namespace bbcore { 

class ShellContext
{
public:
    ShellContext(BOOL* result, LPCITEMIDLIST pidl);
    virtual ~ShellContext(void);

    int ShellMenu(void);
    void Invoke(int nCmd);
    void decref(void); // when counter hits 0 deletes self
    void addref(void);
    HRESULT HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam); 
    HMENU menuHandle(void);

private:
    enum { MIN_SHELL_ID = 1, MAX_SHELL_ID = 0x7FFF };
    int refc{0L};

    LPSHELLFOLDER psfFolder{};
    LPITEMIDLIST  pidlItem{};
    LPITEMIDLIST  pidlFull{};
    LPCONTEXTMENU pContextMenu{};
    HMENU hMenu{};

    static WNDPROC         g_pOldWndProc;
    static LPCONTEXTMENU2  g_pIContext2;
    static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

}; //!namespace bbcore


#endif //!_BBCORE_SHELLCTX_H
