#ifndef _BLACKBOX_H
#define _BLACKBOX_H

#include <windows.h>

class Blackbox
{
    public:
        Blackbox(HINSTANCE hinst, LPSTR lpCmdLine);
        Blackbox(const Blackbox&) = delete;
        int run(void);

    private:
        struct {
            HINSTANCE hInstance;
            LPSTR lpCmdLine;
        } data{};
}; 


#endif //!_BLACKBOX_H
