#ifndef _SETTINGS_BLACKBOX_H
#define _SETTINGS_BLACKBOX_H

#include "Settings.h"

namespace bbcore {

/// \brief Only for blackbox app. Operates on all possible settings
class SettingsBbox : public SettingsCommon
{
    public:
        SettingsBbox(void) = default;

        void readStyleSettings(void) override; 
        void readStylePersonal(const char *style, StyleStruct *pStyle) override;
        const char* checkGlobalFont(const char *p, const char *fullkey) override; 

        void readRCSettings(void) override;
        void writeRCSetting(const void *v) override; 
}; 

// TODO: remove global
extern SettingsBbox* gSettingBbox;

}; //!namespace bbcore

#endif //!_SETTINGS_BLACKBOX_H
