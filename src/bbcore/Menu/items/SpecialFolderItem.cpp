/* ==========================================================================

  This file is part of the bbLean source code
  Copyright © 2001-2003 The Blackbox for Windows Development Team
  Copyright © 2004-2009 grischka

  http://bb4win.sourceforge.net/bblean
  http://developer.berlios.de/projects/bblean

  bbLean is free software, released under the GNU General Public License
  (GPL version 2). For details see:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  ========================================================================== */

#include "bbrc.h"
#include "BB.h"
#include "Settings.h"
#include "Menu.h"
#include "Blackbox.h"
#include <shellapi.h>
#include <shlobj.h>

#include "rcfile.h"
#include "nls.h"
#include "system.h"
#include "SpecialFolderItem.h"
#include "CommandItem.h"
#include "SFInsertItem.h"
#include "SpecialFolderMenu.h"


namespace bbcore { 

static void exec_folder_click(LPCITEMIDLIST pidl);


//===========================================================================
// SpecialFolderItem
//===========================================================================

SpecialFolderItem::SpecialFolderItem(
    const char* pszTitle,
    const char *path,
    pidl_node* pidl_list,
    const char  *pszExtra
    ) : FolderItem(NULL, pszTitle)
{
    m_ItemID = MENUITEM_ID_SF;
    m_pidl_list = path ? get_folder_pidl_list(path, bbDefaultrcPath()) : pidl_list;
    // command to apply to files, optional
    m_pszExtra = new_str(pszExtra);
    if (0 == pszTitle[0]) {
        char disp_name[MAX_PATH];
        replace_str(&m_pszTitle, m_pidl_list
            ? sh_getdisplayname(first_pidl(m_pidl_list), disp_name, isUsingUtf8Encoding())
            : NLS0("Invalid Path"));
    }
}

SpecialFolderItem::~SpecialFolderItem()
{
    free_str(&m_pszExtra);
}


//================================================

void SpecialFolderItem::ShowSubmenu(void)
{
    if (NULL == m_pSubmenu)
    {
        Menu *m = Menu::find_special_folder(m_pidl_list);
        if (m)
            m->incref();
        else
            m = new SpecialFolderMenu(m_pszTitle, m_pidl_list, m_pszExtra);
        LinkSubmenu(m);
    }
    MenuItem::ShowSubmenu();
}

//================================================

void SpecialFolderItem::Invoke(int button)
{
    LPCITEMIDLIST pidl;

    pidl = GetPidl();

    if (INVOKE_PROP & button)
    {
        show_props(pidl);
        return;
    }

    if ((INVOKE_DBL|INVOKE_LEFT) == button)
    {
        m_pMenu->hide_on_click();
        exec_folder_click(pidl);
        return;
    }

    if (INVOKE_RIGHT & button)
    {
        ShowContextMenu(NULL, pidl);
        return;
    }

    if (INVOKE_DRAG & button)
    {
        m_pMenu->start_drag(NULL, pidl);
        return;
    }

    if (INVOKE_LEFT & button)
    {
        if (m_pSubmenu && (MENU_ID_SF != m_pSubmenu->getMenuId()))
            m_pMenu->HideChild(); // hide contextmenu
    }

    FolderItem::Invoke(button);
}

//===========================================================================

Menu* MakeFolderMenu(const char* title, const char* path, const char* cmd)
{
    char disp_name[MAX_PATH];
    pidl_node* pidl_list{};
    Menu* m{};

    pidl_list = get_folder_pidl_list(path, bbDefaultrcPath());
    if (pidl_list)
    {
        m = Menu::find_special_folder(pidl_list);
    }

    if (m)
    {
        m->incref();
        m->SaveState();
    }
    else
    {
        if (nullptr == title)
        {
            if (pidl_list)
            {
                sh_getdisplayname(first_pidl(pidl_list), disp_name, isUsingUtf8Encoding());
                title = disp_name;
            }
            else
            {
                title = file_basename(unquote(strcpy_max(disp_name, path, sizeof disp_name)));
            }
        }
        m = new SpecialFolderMenu(title, pidl_list, cmd);
    }
    delete_pidl_list(&pidl_list);
    return m;
}

//===========================================================================



/* ----------------------------------------------------------------------- */
// Function:    LoadFolder
// Purpose:     enumerate items in a folder, sort them, and insert them
//              into a menu
/* ----------------------------------------------------------------------- */

#ifndef BBTINY

static int is_controls(LPCITEMIDLIST pIDFolder)
{
     LPITEMIDLIST pIDCtrl;
     int ret = 0;
     if (NOERROR == SHGetSpecialFolderLocation(NULL, CSIDL_CONTROLS, &pIDCtrl)) {
         ret = isEqualPIDL(pIDCtrl, pIDFolder);
         SHMalloc_Free(pIDCtrl);
     }
     return ret;
}

int LoadFolder(
    MenuItem **ppItems,
    LPCITEMIDLIST pIDFolder,
    const char *pszExtra,
    int options
    )
{
    enum_files* ef;

    char szDispName[MAX_PATH];
    char szFullPath[MAX_PATH];
    MenuItem* pItem;
    pidl_node* pidl_list;
    int attr, r;

    if (0 == ef_open(pIDFolder, &ef))
        return 0;

    if (usingVista && is_controls(pIDFolder))
        options |= LF_norecurse;

    r = 1; // folder exists
    while (ef_next(ef))
    {
        ef_getattr(ef, &attr);
        if ((attr & ef_hidden) && false == gSettingsMenu.showHiddenFiles)
            continue;

        ef_getname(ef, szDispName, isUsingUtf8Encoding());
        ef_getpidl(ef, &pidl_list);

        if (pszExtra) {
            char *p = (char*)file_extension(szDispName);
            if (0 == stricmp(p, ".style"))
                *p = 0; // cut off .style file-extension
        }

        if (options & LF_join) {
            dolist (pItem, *ppItems) {
                if (0 == stricmp(pItem->m_pszTitle, szDispName)) {
                    //debug_printf("join: %s %d", szDispName, 0 != (attr & ef_folder));
                    if (attr & ef_folder)
                        append_node(&pItem->m_pidl_list, pidl_list);
                    else
                        delete_pidl_list(&pidl_list);
                    break;
                }
            }
            if (pItem)
                continue;
        }

        if ((attr & ef_folder) && !(options & LF_norecurse))
        {
            r |= 4; // contents include a folder
            pItem = new SpecialFolderItem(
                szDispName,
                NULL,
                pidl_list,
                pszExtra
                );

        }
        else if (pszExtra)
        {
            r |= 2; // contents include an item
            pItem = new CommandItem(
                NULL,
                szDispName,
                false
                );

            ef_getpath(ef, szFullPath, isUsingUtf8Encoding());
            pItem->m_pszCommand = replace_arg1(pszExtra, szFullPath);
            pItem->m_ItemID |= MENUITEM_UPDCHECK;
            pItem->m_nSortPriority = M_SORT_NAME;
            pItem->m_pidl_list = pidl_list;

        }
        else
        {
            r |= 2; // contents include an item
            pItem = new CommandItem(
                NULL,
                szDispName,
                false
                );

            if (attr & ef_link)
                pItem->m_nSortPriority = M_SORT_NAME;
            pItem->m_pidl_list = pidl_list;
        }

        // add item to the list
        pItem->next = *ppItems, *ppItems = pItem;
    }
    ef_close(ef);
    return r;
}

//===========================================================================
#endif //ndef BBTINY
//===========================================================================

static void exec_folder_click(LPCITEMIDLIST pidl)
{
    char path[MAX_PATH], *tmp;
    const char *cmd = read_string(bbExtensionsRcPath(NULL), "blackbox.options.openFolderCommand", NULL);
    if (cmd)
    {
        if (sh_getnameof(NULL, pidl, SHGDN_FORPARSING, path, isUsingUtf8Encoding()))
        {
            post_command(tmp = replace_arg1(cmd, path));
            m_free(tmp);
        }
    }
    else
    {
        BBExecute_pidl("explore", pidl);
    }
}

void show_props(LPCITEMIDLIST pidl)
{
    BBExecute_pidl("properties", pidl);
}



} // !namespace bbcore
