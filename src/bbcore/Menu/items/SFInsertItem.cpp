
#include "SFInsertItem.h"
#include "Menu.h"
#include "rcfile.h"

namespace bbcore { 

//===========================================================================
// SFInsert - An invisible item that expands into a file listing
//  when the menu is about to be shown (in Menu::Validate)
//===========================================================================

SFInsert::SFInsert(const char *path, const char  *pszExtra)
    : MenuItem("")
{
    m_ItemID = MENUITEM_ID_INSSF;
    m_pidl_list = get_folder_pidl_list(path, bbDefaultrcPath());
    m_pszExtra = new_str(pszExtra);
    m_pLast = NULL;
    m_bNOP = true;
}

SFInsert::~SFInsert()
{
    free_str(&m_pszExtra);
}

void SFInsert::Measure(HDC hDC, SIZE *size)
{
    MenuItem *pNext, *p;
    size->cx = size->cy = 0;
    if (m_pLast)
        return;

    pNext = this->next;
    this->next = NULL;
    m_pMenu->setLastItem(this);
    m_pMenu->AddFolderContents(m_pidl_list, m_pszExtra);
    for (p = this->next; p; p = p->next)
        m_pMenu->incTotalItemsCount();
    m_pLast = m_pMenu->getLastItem();
    m_pLast->next = pNext;
    if (NULL == m_pMenu->getPidlList())
    {
        m_pMenu->setPidlList(copy_pidl_list(m_pidl_list));
        m_pMenu->setIsDropTarget(true);
    }
}

void SFInsert::RemoveStuff(void)
{
    MenuItem *mi, *next = NULL;
    if (NULL == m_pLast)
        return;
    for (mi = this->next; mi; ) {
        next = mi->next;
        delete mi;
        m_pMenu->decTotalItemsCount();
        if (mi == m_pLast)
            break;
        mi = next;
    }
    this->next = next;
    m_pLast = NULL;
    m_pMenu->setLastActiveItem(NULL);
}

void SFInsert::Paint(HDC hDC)
{
}

} // !namespace bbcore
