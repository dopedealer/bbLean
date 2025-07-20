#ifndef _TINY_DROP_TARG_H
#define _TINY_DROP_TARG_H

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h> 

class TinyDropTarget : public IDropTarget
{
public:
    TinyDropTarget(HWND hwnd);
    virtual ~TinyDropTarget();
    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    // IDropTarget methods
    STDMETHOD(DragEnter)(LPDATAOBJECT pDataObject, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
    STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
    STDMETHOD(DragLeave)();
    STDMETHOD(Drop)(LPDATAOBJECT pDataObject, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
private:
    DWORD m_dwRef;
    bool valid_data;
public:
    HWND m_hwnd;
    HWND task_over;
    void handle_task_timer(void);
};

// call after the bar window is created with it's hwnd
TinyDropTarget *init_drop_targ(HWND hwnd);

// call before the bar window is destroyed
void exit_drop_targ (TinyDropTarget* m_TinyDropTarget);

// call on WM_TIMER / TASK_RISE_TIMER
void handle_task_timer(TinyDropTarget* m_TinyDropTarget);

#endif //!_TINY_DROP_TARG_H 
