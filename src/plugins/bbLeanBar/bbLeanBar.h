#ifndef _BBLEAN_BAR_H
#define _BBLEAN_BAR_H

#define CLOCK_TIMER 2
#define LANGUAGE_TIMER 3
#define LABEL_TIMER 4
#define TASK_RISE_TIMER 5
#define TASKLOCK_TIMER 6
#define GESTURE_TIMER 7
#define CHECK_FULLSCREEN_TIMER 8

// flags for invalidation
#define UPD_DRAW 0x180
#define UPD_NEW  0x181 

#define MY_BROAM "@bbLeanBar"

// forward declarations
struct plugin_info;
struct tasklist;

// possible bar items
enum BARITEMS
{
    M_EOS = 0,
    M_NEWLINE,

    M_TASK,
    M_TRAY,

    M_WSPL,
    M_CLCK,
    M_WINL,
    M_SPAC,
    M_CUOB,     // CurrentOnlyButton
    M_TDPB,     // TaskDisplayButton
    M_WSPB_L,
    M_WSPB_R,

    M_WINB_L,
    M_WINB_R,

    // list classes
    M_BARLIST = 256,
    M_TASKLIST,
    M_TRAYLIST
};

#ifdef BBTINY
extern const char szVersion[];
extern const char szInfoVersion[];
#else
extern const char szVersion[];
extern const char szInfoVersion[];
#endif
extern const char szAppName[];
extern const char szInfoAuthor[];
extern const char szInfoRelDate[];
extern const char szInfoLink[];
extern const char szInfoEmail[];
extern const char szCopyright[];

extern plugin_info* g_PI;

extern int currentScreen;

typedef BOOL (*TASKENUMPROC)(const tasklist*, LPARAM);

// TODO: maybe create api in BB.h and replace this?
void EnumTasks(TASKENUMPROC lpEnumFunc, LPARAM lParam);

// TODO: maybe make prototype definition in sysmenu.h, paired to sysmenu.cpp ?
bool ShowSysmenu(HWND TaskWnd, HWND BarWnd, RECT *pRect, const char *plugin_broam);
bool sysmenu_exists();
bool exec_sysmenu_command(const char *temp, bool sendToSwitchTo);

#endif //!_BBLEAN_BAR_H
