#ifndef _BLACKBOX_H
#define _BLACKBOX_H

#include <windows.h>

#include "application.h" 
#include "SettingsBbox.h"

namespace bbcore {

class Blackbox
{
    public:
        Blackbox(HINSTANCE hinst, LPSTR lpCmdLine);
        Blackbox(const Blackbox&) = delete;
        int run(void);

        int messageLoop(void);

    private:
        struct {
            HINSTANCE hInstance;
            LPSTR lpCmdLine;
        } data{};

        SettingsBbox settings{};
}; 

}; //!namespace bbcore

#endif //!_BLACKBOX_H
