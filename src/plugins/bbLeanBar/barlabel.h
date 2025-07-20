#ifndef _BB_BAR_LABEL_H
#define _BB_BAR_LABEL_H

#include "baritem.h"

struct barinfo;

/// \brief Common base class for clock, workspace-label, window-label 
class barlabel : public baritem
{
public:
    int m_Style;
    const char *m_text;

    //-----------------------------

    barlabel(int type, barinfo* bi, char* text, int S); 
    void draw(void);
};

#endif //!_BB_BAR_LABEL_H
