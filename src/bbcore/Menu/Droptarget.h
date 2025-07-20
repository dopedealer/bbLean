#ifndef _BBCORE_DROPTARGET_H
#define _BBCORE_DROPTARGET_H

#include <shlobj.h>

namespace bbcore { 

class CDropTarget;

CDropTarget* init_drop_target(HWND hwnd, LPCITEMIDLIST pidl);
void exit_drop_target(CDropTarget* dt);
bool in_drop(CDropTarget* dt);

class CDropTarget : public IDropTarget
{
public:
    CDropTarget(HWND hwnd, LPCITEMIDLIST pidl);
    virtual ~CDropTarget();
    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

private:
    // IDropTarget methods
    STDMETHOD(DragEnter)(LPDATAOBJECT pDataObject, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
    STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
    STDMETHOD(DragLeave)();
    STDMETHOD(Drop)(LPDATAOBJECT pDataObject, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);

    DWORD m_dwRef; // "COM" object reference counter

    HWND m_hwnd;
    LPITEMIDLIST m_pidl;
    LPDROPTARGET m_pDropTarget; // the actual shell folder that will receive dropped items
    LPDATAOBJECT m_pDataObject;
    friend class CDropTarget *init_drop_target(HWND hwnd, LPCITEMIDLIST pidl);
    friend void exit_drop_target (class CDropTarget *dt);
    friend bool in_drop(class CDropTarget *dt);
};


} // !namespace bbcore

#endif //!_BBCORE_DROPTARGET_H
