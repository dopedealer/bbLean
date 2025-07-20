#ifndef _BB_LEAN_BAR_H
#define _BB_LEAN_BAR_H

#include "baritemlist.h"

struct barinfo;

/// \brief LeanBar - the main class 
class LeanBar : public baritemlist
{
public:
    int max_label_width;
    int max_clock_width;
    int trayzone_width;

    //-----------------------------

    LeanBar(barinfo* bi); 
    void settip(void); 
    void invalidate(int flag);

    /// \brief Check for capture, otherwise dispatch the mouse event
    void mouse_event(int mx, int my, int message, unsigned flags);

    /// \brief Build everything from scratch
    void create_bar(void); 
    bool calc_sizes(void);

    // Here sizes are calculated in two passes: The first pass
    // gets all fixed sizes. Then the remaining space is assigned
    // to the variable ones (windowlabel/taskzone). The second pass
    // assigns the actual x-coords.  
    bool calc_line_size(itemlist** p0, int top, int bottom, int height);
};

#endif //!_BB_LEAN_BAR_H
