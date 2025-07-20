#ifndef _BB_TASK_ENTRY_H
#define _BB_TASK_ENTRY_H

#include "baritem.h"

struct barinfo;
struct StyleItem;
struct tasklist;

/// \brief One task entry 
class taskentry : public baritem
{
public:
    int m_index;
    bool m_showtip;
    bool m_dblclk;
    bool a1, a2;
    int press_x;

    //-----------------------------

    taskentry(int index, barinfo* bi); 
    void draw(void);

    // Icon only mode
    void draw_icons(tasklist* tl, bool lit, StyleItem* pSI); 
    // Text (with icon) mode
    void draw_text(tasklist* tl, bool lit, StyleItem* pSI); 
    void settip(void); 
    void mouse_event(int mx, int my, int message, unsigned flags);
};

#endif //!_BB_TASK_ENTRY_H
