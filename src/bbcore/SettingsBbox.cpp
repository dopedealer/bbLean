
#include "BB.h"
#include "bbrc.h"
#include "Menu/MenuMaker.h"
#include "BImage.h"
#include "SettingsBbox.h"
#include "Blackbox.h"
#include "rcfile.h"

namespace bbcore {

// TODO: remove global
SettingsBbox* gSettingBbox{};


/// \brief Create a Font, possible substitutions have been already applied
/// Create a font handle from styleitem, with parsing and substitution.
HFONT createStyleFont(StyleItem *pSI)
{
    SettingsBbox settings;
    return settings.createStyleFont(pSI);
}

/// \brief whether using utf-8 encoding on char string
///        conversions (current OS code page in case of false)
bool isUsingUtf8Encoding(void)
{
    return bool(Settings_UTF8Encoding);
}

/// \brief Direct access to Settings variables / styleitems / colors
///        See the "SN_XXX" constants above
void* getSettingPtr(int sn_index)
{
    return StyleStructPtr(sn_index, &mStyle);
}

/// \brief Read an entire StyleItem.
/// This is not meant for the standard style items, but for a plugin to read in
/// custom style extensions. Returns nonzero if any of the requested properties
/// were read
int readStyleItem(
    const char* fileName,
    const char* szKey,
    StyleItem* pStyleItemOut,
    StyleItem* pStyleItemDefault
    )
{
    SettingsBbox settings;
    return settings.readStyleItem(fileName, szKey, pStyleItemOut, pStyleItemDefault);
}

void getCustomMargin(RECT *pcm, int screen)
{
    char key[40];
    int x, i;
    static const char * const edges[4] = { "Left", "Top", "Right", "Bottom" };

    if (0 == screen)
    {
        x = sprintf(key, "blackbox.desktop.margin");
    }
    else
    {
        x = sprintf(key, "blackbox.desktop.%d.margin", 1+screen);
    }

    i = 0;
    do {
        strcpy(key+x, edges[i]);
        (&pcm->left)[i] = read_int(bbExtensionsRcPath(NULL), key, -1);
    } while (++i < 4);
}


//===========================================================================

// A private settings code that used only by blackbox exe 

void SettingsBbox::readStylePersonal(const char *style, StyleStruct *pStyle)
{
    pStyle->toolbarAlpha = Settings_toolbar.alphaEnabled ? Settings_toolbar.alphaValue : 255;
    pStyle->menuAlpha = gSettingsMenu.alphaEnabled ? gSettingsMenu.alphaValue : 255;
} 

const char* SettingsBbox::checkGlobalFont(const char *p, const char *fullkey)
{
    if (Settings_globalFonts)
    {
        char globalkey[100];
        const char *p2;
        strcat(strcpy(globalkey, "blackbox.global."), fullkey);
        p2 = read_value(bbExtensionsRcPath(NULL), globalkey, NULL);
        //debug_printf("<%s> <%s>", globalkey, p2);
        if (p2 && p2[0]) return p2;
    }
    return p;
}

void SettingsBbox::readStyleSettings(void)
{
    //DWORD t = GetTickCount(); for (int i=0; i < 1000; ++i)
    readStyle(bbStylePath(NULL), &mStyle);
    //debug_printf("1000 styles read in %d ms", GetTickCount() - t);

    bimage_init(Settings_imageDither, mStyle.is_070);
}

void SettingsBbox::readRCSettings(void)
{
    const char *p, *extrc, *bbrc;

    bbResetDefaultrcPath();
    p = read_string(bbExtensionsRcPath(NULL), "blackbox.theme:", "");
    if (p[0] && 0 != stricmp(p, "default"))
    {
        findRCFile(bbDefaultrcBuffer(), p, NULL);
    }

    extrc = bbExtensionsRcPath(NULL);
    bbrc = bbRcPath(NULL);
    readSettings(bbrc, GetBboxRcConfig());
    readSettings(extrc, GetExtRcConfig());
    p = read_string(extrc, "blackbox.contextmenu.itemAdjust", "28/28");
    sscanf(p, "%d/%d", &Settings_contextMenuAdjust[0], &Settings_contextMenuAdjust[1]);

    bbMenuPath(read_string(bbrc, "session.menuFile", NULL));
    bbStylePath(read_string(bbrc, "session.styleFile", ""));
    bbPluginsRcPath(read_string(bbrc, "session.pluginFile", NULL));

    p = read_string(extrc, "blackbox.options.log", "");
    Settings_LogFlag = 0;
    if (*p)
    {
        if (stristr(p, "Shutdown"))
            Settings_LogFlag |= LOG_SHUTDOWN;
        if (stristr(p, "Startup"))
            Settings_LogFlag |= LOG_STARTUP;
        if (stristr(p, "Tray"))
            Settings_LogFlag |= LOG_TRAY;
        if (stristr(p, "Plugins"))
            Settings_LogFlag |= LOG_PLUGINS;
    }
}

void SettingsBbox::writeRCSetting(const void *v)
{
    writeSetting(bbRcPath(NULL), GetBboxRcConfig(), v)
    ||
    writeSetting(bbExtensionsRcPath(NULL), GetExtRcConfig(), v);
}


}; //!namespace bbcore
