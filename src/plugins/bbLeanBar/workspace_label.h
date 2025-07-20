#ifndef _BB_WORKSPACE_LABEL_H
#define _BB_WORKSPACE_LABEL_H

#include "barlabel.h"

struct barinfo;

/// \brief Workspace-label
class workspace_label : public barlabel
{
public:
    workspace_label(barinfo* bi); 
    void mouse_event(int mx, int my, int message, unsigned flags); 
};

#endif //!_BB_WORKSPACE_LABEL_H
