
#include "exports.h" 

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    bbcore::Blackbox blackbox(hInstance, lpCmdLine);
    gApp = &blackbox;
    return blackbox.run();
}
