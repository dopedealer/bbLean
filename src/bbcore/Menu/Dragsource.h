#ifndef _BBCORE_DRAGSRC_H
#define _BBCORE_DRAGSRC_H

namespace bbcore {

class CImpIDropSource: public IDropSource
{
public:
    CImpIDropSource();
    virtual ~CImpIDropSource();
    //IUnknown members
    STDMETHOD(QueryInterface)(REFIID,  void **);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);
private:
    //IDataObject members
    STDMETHOD(GiveFeedback)(DWORD dwEffect);
    STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed,DWORD grfKeyState);
    long m_dwRef;
};

void drag_pidl(LPCITEMIDLIST pidl);

}; //!namespace bbcore

#endif //!_BBCORE_DRAGSRC_H
