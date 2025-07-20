#ifndef _BB_SPACER_H
#define _BB_SPACER_H

#include "baritem.h"

/// \brief Fill in a space or new line 
class spacer : public baritem
{
    public:
        spacer(int typ, barinfo* bi);
};

#endif //! _BB_SPACER_H

