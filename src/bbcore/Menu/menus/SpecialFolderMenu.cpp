
#include "SpecialFolderMenu.h"
#include "nls.h"

namespace bbcore { 

SpecialFolderMenu::SpecialFolderMenu(
    const char *pszTitle,
    const pidl_node* pidl_list,
    const char  *pszExtra)
    : Menu(pszTitle)
{
    setMenuId(MENU_ID_SF);
    setPidlList(copy_pidl_list(pidl_list));
    m_pszExtra = new_str(pszExtra);
    setIsDropTarget(true);
    // fill menu
    UpdateFolder();
}

SpecialFolderMenu::~SpecialFolderMenu(void)
{
    free_str(&m_pszExtra);
}

void SpecialFolderMenu::UpdateFolder(void)
{ 
    // delete_old items
    DeleteMenuItems(); 
    // load the folder contents
    int flag = AddFolderContents(getPidlList(), m_pszExtra);

    if (0 == (flag & 6))
    {
        if (flag & 1)
        {
            makeMenuNOP(this, NLS0("No Files"));
        }
        else
        {
            makeMenuNOP(this, NLS0("Invalid Path"));
        }
    }
}

} // !namespace bbcore
