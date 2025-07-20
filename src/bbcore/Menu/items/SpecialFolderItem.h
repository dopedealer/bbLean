#ifndef _BBCORE_SPEC_FOLDERITEM_H
#define _BBCORE_SPEC_FOLDERITEM_H

#include "FolderItem.h"

namespace bbcore { 

// an item that opens a SpecialFolder - unlike with
// other submenus these are not built until needed
class SpecialFolderItem : public FolderItem
{
public:
    SpecialFolderItem(const char* pszTitle, const char *path, pidl_node* pidl_list, const char *pszExtra);
    ~SpecialFolderItem();
    void ShowSubmenu(void);
    void Invoke(int button);
    friend class SpecialFolder;
private:
    char* m_pszExtra{};
};


int LoadFolder(MenuItem **, LPCITEMIDLIST pIDFolder, const char  *pszExtra, int options);

enum { LF_join = 1, LF_norecurse = 2 };
void show_props(LPCITEMIDLIST pidl);

::Menu* MakeFolderMenu(const char *title, const char* path, const char *cmd);


} // !namespace bbcore



#endif //!_BBCORE_SPEC_FOLDERITEM_H
