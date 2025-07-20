#ifndef _WINDOW_LABEL_H
#define _WINDOW_LABEL_H

#include "barlabel.h"

struct barinfo;

/// \brief Window-label
class window_label : public barlabel
{
public:
    window_label(barinfo* bi);
};

#endif //!_WINDOW_LABEL_H
