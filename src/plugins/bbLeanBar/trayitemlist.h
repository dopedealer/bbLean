#ifndef _BB_TRAY_ITEM_LIST_H
#define _BB_TRAY_ITEM_LIST_H

#include "baritemlist.h"

struct barinfo;

/// \brief Tray zone 
class trayitemlist : public baritemlist
{
    int len{};

public:
    trayitemlist(barinfo* bi);

    /// \brief This one assigns the individual locations and sizes for
    ///        the items in the tray-icon-list
    bool calc_sizes(void); 
    void invalidate (int flag); 
    void mouse_event(int mx, int my, int message, unsigned flags);

/*
    void draw()
    {
        m_bar->pBuff->MakeStyleGradient(hdcPaint,  &mr, TaskStyle_L, false);
        baritemlist::draw();
    }
*/
};

#endif //!_BB_TRAY_ITEM_LIST_H
