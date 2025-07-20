#ifndef _BARITEM_H
#define _BARITEM_H

#include <windows.h>

struct barinfo;

/// \brief The base class for all items on the bar: 
class baritem
{
public:
    barinfo* m_bar;     // the pointer to the plugin_info structure
    int m_type;         // the item's M_XXX ID
    RECT mr;            // the rectangle of the item
    bool m_active;      // pressed state
    int m_margin;       //
    bool mouse_in;

    //-----------------------------

    baritem(int type, barinfo* bi); 
    virtual ~baritem(void);

    /// \brief Asign the rectangle, advance the x-pointer, returns true if changed 
    bool set_location(int *px, int y, int w, int h, int m);

    // check the item for mouse-over
    int mouse_over (int mx, int my); 
    bool menuclick(int message, unsigned flags);

    virtual void mouse_event(int mx, int my, int message, unsigned flags); 
    virtual void draw(void); 
    virtual void settip(void); 
    // calculate metrics, return true on changes
    virtual bool calc_sizes(void); 
    virtual void invalidate(int flag); 
    void release_capture(void);

    bool check_capture(int mx, int my, int message); 
};





#endif //!_BARITEM_H
