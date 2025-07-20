#ifndef _BB_TRAY_ENTRY_H
#define _BB_TRAY_ENTRY_H

#include "baritem.h"

struct barinfo;

// one tray-icon 
class trayentry : public baritem
{
public:
    int m_index;

    //-----------------------------

    trayentry(int index, barinfo* bi); 

    void invalidate (int flag); 
    void draw(void); 
    void settip(void); 
    void mouse_event(int mx, int my, int message, unsigned flags);
};

#endif //!_BB_TRAY_ENTRY_H
