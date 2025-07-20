
#include "baritemlist.h"

#include <bblib.h>

#include "barinfo.h"

baritemlist::baritemlist(int type, barinfo *bi)
    : baritem(type, bi)
{
}

baritemlist::~baritemlist(void)
{
    clear();
}

void baritemlist::add(class baritem *entry)
{
    append_node(&items, new_node(entry));
}

void baritemlist::clear(void)
{
    itemlist* i;
    dolist (i, items) delete i->item;
    freeall(&items);
}

void baritemlist::draw()
{
    itemlist *p; RECT rtmp;
    dolist (p, items)
        if (IntersectRect(&rtmp, &p->item->mr, m_bar->p_rcPaint))
            p->item->draw();
} 

void baritemlist::mouse_event(int mx, int my, int message, unsigned flags)
{
    itemlist *p, *q;

    dolist (p, items)
        if (p->item->mouse_over(mx, my) > 0)
            break;

    dolist (q, items)
        if (q->item->mouse_in)
            break;

    if (q && q != p) {
        q->item->mouse_event(mx, my, WM_MOUSELEAVE, flags);
        q->item->mouse_in = false;
    }

    if (p) {
        p->item->mouse_event(mx, my, message, flags);
        p->item->mouse_in = true;
    } else {
        menuclick(message, flags);
    }
} 

void baritemlist::settip(void)
{
    itemlist* p;
    dolist (p, items) p->item->settip();
}

void baritemlist::invalidate(int flag)
{
    itemlist* p;
    dolist (p, this->items)
        if (flag == p->item->m_type)
            p->item->invalidate(0);
}
