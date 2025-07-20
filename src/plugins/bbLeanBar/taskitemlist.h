#ifndef _BB_TASK_ITEM_LIST_H
#define _BB_TASK_ITEM_LIST_H

#include "baritemlist.h"

struct barinfo;

// task zone 
class taskitemlist : public baritemlist
{
    int len{};

public:
    taskitemlist(barinfo* bi);

    // This one assigns the individual locations and sizes for
    // the items in the task-list 
    bool calc_sizes(void); 
    void mouse_event(int mx, int my, int message, unsigned flags); 
    void invalidate (int flag);

/*
    void draw()
    {
        if (TaskStyle == 1)
        {
            m_bar->pBuff->MakeStyleGradient(hdcPaint,  &mr, TaskStyle_L, false);
        }
        baritemlist::draw();
    }
*/
};

#endif //!_BB_TASK_ITEM_LIST_H
