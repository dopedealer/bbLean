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

#include "BB.h"
#include "Settings.h"
#include "Menu.h"
#include "BImage.h"
#include "nls.h"
#include "system.h"
#include "CommandItem.h"
#include "StringItem.h"
#include "IntegerItem.h"
#include "SpecialFolderItem.h"

namespace bbcore { 

//===========================================================================
// CommandItem
//===========================================================================

CommandItem::CommandItem(const char* pszCommand, const char* pszTitle, bool bChecked)
    : MenuItem(pszTitle)
{
    m_pszCommand = new_str(pszCommand);
    m_bChecked = bChecked;
    m_ItemID = MENUITEM_ID_CMD;
}


void CommandItem::Invoke(int button)
{
    LPCITEMIDLIST pidl = GetPidl();
    if (INVOKE_PROP & button)
    {
        show_props(pidl);
        return;
    }

    if (INVOKE_LEFT & button)
    {
        m_pMenu->hide_on_click();
        if (m_pszCommand) {
            if (strstr(m_pszCommand, "%b"))
                post_command_fmt(m_pszCommand, false == m_bChecked);
            else
                post_command(m_pszCommand);
        } else if (pidl)
            BBExecute_pidl(NULL, pidl);
        return;
    }

    if (INVOKE_RIGHT & button)
    {
        if (m_pszRightCommand)
            post_command(m_pszRightCommand);
        else
        if (m_pRightmenu)
            m_pRightmenu->incref(), ShowRightMenu(m_pRightmenu);
        else
            ShowContextMenu(m_pszCommand, pidl);
        return;
    }

    if (INVOKE_DRAG & button)
    {
        m_pMenu->start_drag(m_pszCommand, pidl);
        return;
    }
}

//===========================================================================
// helper for IntegerItem / StringItem

void CommandItem::next_item (WPARAM wParam)
{
    Menu *pm = m_pMenu->getParentMenu();
    if (pm) {
        pm->set_focus();
        Active(0);
        PostMessage(pm->getWindowHandle(), WM_KEYDOWN, wParam, 0);
    }
}

} // !namespace bbcore
