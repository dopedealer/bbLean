#ifndef _BB_CLOCK_DISPL_H
#define _BB_CLOCK_DISPL_H

#include "barlabel.h"

struct barinfo;

/// \brief Clock-label 
class clock_displ : public barlabel
{
public:
    clock_displ(barinfo *bi); 
    void mouse_event(int mx, int my, int message, unsigned flags);
};

#endif //!_BB_CLOCK_DISPL_H
