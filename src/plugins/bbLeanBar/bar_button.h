#ifndef _BB_BAR_BUTTON_H
#define _BB_BAR_BUTTON_H

#include "baritem.h"

class bar_button : public baritem
{
    public:
        int dir;

        //-----------------------------

        bar_button(int m, barinfo* bi); 
        void draw(void) override;

        /// \brief For the buttons, the mouse is captured on button-down
        void mouse_event(int mx, int my, int message, unsigned flags) override;
        void settip(void) override;

    private:
        bool hasCustomTip{};
};


#endif //!_BB_BAR_BUTTON_H
