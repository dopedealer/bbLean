
#include "commands.h"

#include <windows.h>
#include <cstdio>

#include "BBApi.h"
#include "MenuMaker.h"
#include "rcfile.h"
#include "Desk.h"
#include "MessageManager.h"

namespace bbcore {

/* ----------------------------------------------------------------------- */
/* This is for the menu checkmarks in the styles and backgrounds folders */

bool get_opt_command(char* opt_cmd, const char* cmd)
{
    //debug_printf("optcmd %s", cmd);
    if (0 == opt_cmd[0])
    {
        if (0 == memicmp(cmd, "@BBCore.", 8))
        {
            // internals, currently for style and rootcommand
#define CHECK_BROAM(broam) 0 == memicmp(cmd, s = broam, sizeof broam - 3)
            const char *s;
            if (CHECK_BROAM(MM_STYLE_BROAM))
            {
                sprintf(opt_cmd, s, bbStylePath(NULL));
            }
            else if (CHECK_BROAM(MM_THEME_BROAM))
            {
                sprintf(opt_cmd, s, bbDefaultrcPath() ? bbDefaultrcPath() : "default");
            }
            else if (CHECK_BROAM(MM_ROOT_BROAM))
            {
                sprintf(opt_cmd, s, Desk_extended_rootCommand(NULL));
            }
            else
            {
                return false;
            }
#undef CHECK_BROAM
        }
        else if (0 == MessageManager_Send(BB_GETBOOL, (WPARAM)opt_cmd, (LPARAM)cmd))
        {
            return false; // nothing found
        }
        else if (opt_cmd[0] == 1)
        {
            opt_cmd[0] = 0; // recheck next time;
            return true;
        }
    }
    return opt_cmd[0] && 0 == stricmp(opt_cmd, cmd);
}


}; //!namespace bbcore
