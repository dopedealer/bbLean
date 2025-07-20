#ifndef _BARINFO_H
#define _BARINFO_H 

#include <bbstyle.h>

#include "bbPlugin.h"

//===========================================================================
// configuration and menu items

enum cfg_types {
    R_BOL = 0,
    R_INT,
    R_STR
};

#define CFG_255 1
#define CFG_INT 2
#define CFG_TASK 4
#define CFG_STR 8
#define CFG_INT2 16
#define CFG_SUB 32
#define CFG_WIN 33
#define CFG_MAIN 64

struct config
{
    const char* str;
    int mode;
    const void* def;
    const void *ptr;
};

struct pmenu
{
    const char* displ;
    const char* msg;
    int f;
    const void* ptr;
};

struct barinfo : plugin_info
{
    public:
        #define BARINFO_FIRSTITEM cfg_list 
        config* cfg_list;
        pmenu* cfg_menu;
        class BuffBmp   *pBuff;
        class LeanBar   *pLeanBar;
        class TinyDropTarget *m_TinyDropTarget;
        tasklist* taskList;
        ToolbarInfo* TBInfo;

        // configuration
        int  widthPercent           ;
        bool reverseTasks           ;
        int  saturationValue        ;
        int  hueIntensity           ;
        bool smallIcons             ;
        char strftimeFormat[60]     ;
        bool taskSysmenu            ;
        int TaskStyle               ;
        int taskMaxWidth            ;
        bool currentOnly            ;
        bool task_with_border       ;
        bool autoFullscreenHide     ;
        bool setDesktopMargin       ;
        bool sendToSwitchTo         ;
        bool sendToGesture          ;

        // painting
        HFONT hFont;
        HDC hdcPaint;
        RECT *p_rcPaint;

        // metrics
        int buttonH;
        int labelH;
        int bbLeanBarLineHeight[8];
        int BarLines;
        int TASK_ICON_SIZE;
        int TRAY_ICON_SIZE;
        int buttonBorder;
        int labelIndent;
        int labelBorder;

        // runtime temp variables
        int old_place;
        char clockTime[80];
        char windowlabel[256];
        int labelWidth;
        int clockWidth;
        bool force_button_pressed;
        bool ShowingExternalLabel;
        bool near_top;
        bool on_multimon;
        bool has_tray;
        bool has_tasks;
        bool show_kbd_layout;
        bool gesture_lock;
        bool full_screen_hidden ;
        int mon_top, mon_bottom;

        HWND task_over_hwnd;
        HWND task_lock;

        char instance_key[40];
        int n_index;

        // The string, which describes the outlay of the bar
        char item_string[32];
        // an item has captured the mouse
        class baritem* capture_item; 

        void about_box(void);

        // class ctor & dtor 
        barinfo(void); 
        ~barinfo(void);

        // --------------------------------------------------------------
        // class methods

        void process_broam(const char *temp, int f);
        LRESULT wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *ret);
        void show_menu(bool);
        void desktop_margin_fn (void);
        void pos_changed(void);
        void set_tbinfo(void);
        void reset_tbinfo(void);

        void set_screen_info(void);
        void set_clock_string (void);
        void update_windowlabel(void);
        int get_text_width(const char *cp);

        void update(int);

        void post_trayicon_message(UINT wParam);
        bool check_fullscreen_window(void);
        void GetRCSettings();
        void WriteRCSettings();
        void GetStyleSettings();
        void make_cfg(void);

        // --------------------------------------------------------------
        // tasklist support - wrapper for the GetTask... functions
        // to implement the 'current tasks only' feature

        const tasklist* GetTaskListPtrEx(void) { return taskList; } 
        static BOOL task_enum_func(const tasklist* tl, LPARAM lParam);

        void DelTasklist(void); 
        void NewTasklist(void); 
        tasklist* GetTaskPtrEx(int pos); 
        int GetTaskListSizeEx(void);

        HWND GetTaskWindowEx(int i);

        // --------------------------------------------------------------
        // traylist support - wrapper for the SystemTray functions
        // to implement the 'hide icons' feature

#define CLASS_NAME_MAX 80

        // there are two lists that hold the additional information

        // one list corresponding to all icons from the core.
        // hWnd and uId are to identify the icon for lookup, index is
        // its orignal index in the core's list, true for hidden means the
        // icon is not shown unless trayShowAll is true.
        struct trayNode
        {
            trayNode* next;
            bool hidden;
            bool mark;
            bool tip_checked;
            HWND hWnd;
            unsigned uID;
            int index;
            char class_name[CLASS_NAME_MAX];
        };

        // another list for the info as loaded from/saved to bbleanbar.rc
        struct trayHidden {
            trayHidden* next;
            unsigned uID;
            char class_name[CLASS_NAME_MAX];
        };

        trayNode *trayList;
        trayHidden *trayHiddenList;
        int trayVisiblesCount;
        bool trayShowAll;

        /// \brief Clean up everything with the tray wrapper
        void DelTraylist(void);

        // these three are the wrappers that replace the
        // previous plain implementation 
        int GetTraySizeEx(void); 
        systemTray* GetTrayIconEx(int i); 

        /// \brief Get icon node from index, counting only visibles
        trayNode* GetTrayNode(int vis_index);

        /// \brief Get real index from visibles index
        int RealTrayIndex(int vis_index);

        /// \brief Build / refresh secondary trayList
        void NewTraylist(void);

        /// \brief change hidden state:  1:show,  0:hide, -1:toggle
        void trayShowIcon(int index, int state);

        /// \brief Change show all state:  1:on,  0:off, -1:toggle
        void trayToggleShowAll(int state);

        /// \brief Save info about hidden icons to rc (by uID and window classname)
        void traySaveHidden(void);

        /// \brief Load hidden icons' info from rc (uID and window classname)
        trayHidden* trayLoadHidden(void);

        // some icons dont have a tip until mouse hoover.
        // So try to get the needed tips for the menu with some faked mouse moves
        void forceTips(void);

        /// \brief Show menu with tray icons
        void trayMenu(bool pop);

        // end of tray wrapper functionality
        // --------------------------------------------------------------

    protected:
        static void get_window_text(HWND hwnd, char *buffer, int size);
};

extern bool nobool;
extern char gScreenName[]; 
extern StyleItem NTaskStyle;
extern StyleItem ATaskStyle;

#endif //!_BARINFO_H
