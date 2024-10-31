
#include <Blackbox.h>

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    Blackbox blackbox(hInstance, lpCmdLine);
    return blackbox.run();
}
