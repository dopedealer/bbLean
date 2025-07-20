
#ifndef _BAR_ITEM_LIST_H
#define _BAR_ITEM_LIST_H

#include "baritem.h"

struct itemlist
{
    itemlist* next;
    baritem* item;
};

/// \brief A list class, for tasks and tray-icons, also for the entire bar 
class baritemlist : public baritem
{
public:
    itemlist* items{};

    //-----------------------------

    baritemlist(int type, barinfo* bi); 
    virtual ~baritemlist(void); 

    void add(class baritem *entry); 
    void clear(void); 
    void draw(void) override;

    void mouse_event(int mx, int my, int message, unsigned flags) override; 
    void settip(void) override;
    void invalidate(int flag) override;
};

#endif //!_BAR_ITEM_LIST_H

