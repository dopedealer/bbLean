
#include <windows.h> 

#include <bbshell.h>
#include <bbrc.h> 

#include "rcfile.h"
#include "Settings.h"
#include "nls.h"
#include "Utils.h"

namespace bbcore {

char defaultrc_path[MAX_PATH];
char gPluginrcPath[MAX_PATH]; 
static char blackboxrc_path[MAX_PATH];
static char extensionsrc_path[MAX_PATH];
static char menurc_path[MAX_PATH];
static char stylerc_path[MAX_PATH]; 

void fn_write_error(const char *filename);

static rcreader_init g_rc =
{
    NULL,               // struct fil_list *rc_files;
    fn_write_error,     // void (*write_error)(const char *filename);
    true,               // char dos_eol;
    true,               // char translate_065;
    0                   // char found_last_value;
};

COLORREF readColor(const char* fileName, const char* szKey, const char* defaultColor)
{
    const char* szValue = szKey[0] ? read_value(fileName, szKey, NULL) : NULL;
    return ReadColorFromString(szValue ? szValue : defaultColor);
} 

/// \brief Delete rc-entry - wildcards supported for keyword
bool deleteSetting(LPCSTR path, LPCSTR szKey)
{
    return 0 != delete_setting(path, szKey);
}

/// \brief Rename Setting (or delete with new_keyword=NULL)
bool renameSetting(const char* path, const char* szKey, const char* new_keyword)
{
    return 0 != rename_setting(path, szKey, new_keyword);
}

[[deprecated]]  // must be refactored and removed! Writing to unknown size buffer is bad
char* bbDefaultrcBuffer(void)
{
    return  defaultrc_path;
}

/// \brief If contains non-empty string then returns it. Otherwise returns nullptr
const char* bbDefaultrcPath(void)
{
    return defaultrc_path[0] ? defaultrc_path : nullptr;
}

void bbResetDefaultrcPath(void)
{
    defaultrc_path[0] = 0;
} 

//static
bool find_resource_file(char* pszOut, const char* filename, const char* basedir)
{
    char temp[MAX_PATH];

    if (bbDefaultrcPath()) {
        join_path(pszOut,  bbDefaultrcPath(), file_basename(filename));
        if (file_exists(pszOut))
        {
            return true;
        }
    }
#ifndef BBTINY
    sprintf(temp, "APPDATA\\blackbox\\%s", file_basename(filename));
    if (file_exists(replace_shellfolders(pszOut, temp, false, bbDefaultrcPath(), isUsingUtf8Encoding())))
    {
        return true;
    }
#endif
    if (is_absolute_path(filename))
        return file_exists(strcpy(pszOut, filename));

    if (basedir) {
        join_path(pszOut, basedir, filename);
        // save as default for below
        strcpy(temp, pszOut);
        if (file_exists(pszOut))
            return true;
    }
    replace_shellfolders(pszOut, filename, false, bbDefaultrcPath(), isUsingUtf8Encoding());
    if (file_exists(pszOut))
        return true;
    if (basedir)
        strcpy(pszOut, temp);
    return false;
}

/// \brief Lookup a configuration file
/// Get full Path for a given filename. The file is searched:
///    1) If pluginInstance is not NULL: In the directory that contains the DLL
///    2) In the blackbox directory.
///    Returns true if the file was found.
///    If not, 'pszOut' is set to the default location (plugin directory)
/// \param  pszOut The location where to put the result
/// \param  filename Name of file to look for
/// \param  module Handle or NULL
/// \return bool = true of found, FALSE otherwise
bool findRCFile(char* pszOut, const char* filename, HINSTANCE module)
{
    char basedir[MAX_PATH];
    char file_rc[MAX_PATH];
    char temp[MAX_PATH];
    char *e;
    bool ret;

    if (NULL == module || 0 == GetModuleFileName(module, temp, MAX_PATH))
    {
        ret = find_resource_file(pszOut, filename, NULL); 
    }
    else
    {
        ::GetLongPathNameA(temp, temp, MAX_PATH);
        file_directory(basedir, temp);
        strcpy(file_rc, filename);

        e = (char*)file_extension(file_rc);
        if (*e)
        {
            // file has extension
            ret = find_resource_file(pszOut, file_rc, basedir);
        }
        else
        {
            strcpy(e, ".rc"); // try file.rc
            ret = find_resource_file(pszOut, file_rc, basedir);
            if (!ret)
            {
                strcpy(temp, pszOut);
                strcpy(e, "rc"); // try filerc
                ret = find_resource_file(pszOut, file_rc, basedir);
                if (!ret)
                {
                    strcpy(pszOut, temp); // use file.rc as default
                }
            }
        }
    }
    //debug_printf("FindRCFile (%d) %s -> %s", ret, filename, pszOut);
    return ret;
}

const char* bbConfigFileExists(const char* filename, const char* pluginDir)
{
    static char tempBuf[MAX_PATH];
    if (false == find_resource_file(tempBuf, filename, pluginDir))
    {
        tempBuf[0] = 0;
    }
    return tempBuf;
}

char* bbPluginrcBuffer(void)
{
    return gPluginrcPath;
}

void bbResetPluginRcPath(void)
{
    gPluginrcPath[0] = 0;
}

/// \brief Opens file for parsing
/// Open/Close a file for use with 'FileRead' or 'ReadNextCommand'
/// (Do not use 'fopen/fclose' in combination with these)
FILE* fileOpen(const char* szPath)
{
#ifdef __BBCORE__
    // hack to prevent BBSlit from loading plugins, since they are
    // loaded by the built-in PluginManager.
    if (0 == strcmp(szPath, bbPluginrcBuffer()))
    {
        return nullptr;
    }
#endif
    return fopen(szPath, "rt");
}

/// \brief Close selected file
bool fileClose(FILE* fp)
{
    return fp && 0 == fclose(fp);
}

/// \brief Read's a line from given FILE and returns boolean on status
bool fileRead(FILE* fp, char* buffer)
{
    return 0 != read_next_line(fp, buffer, MAX_LINE_LENGTH);
}

/// \brief Reads the next line of the file
/// Additionally skips comments and empty lines
bool readNextCommand(FILE *fp, char* szBuffer, unsigned dwLength)
{
    while (read_next_line(fp, szBuffer, dwLength))
    {
        char c = szBuffer[0];
        if (c && '#' != c && '!' != c)
        {
            return true;
        }
    }
    return false;
} 

//static
void fn_write_error(const char *filename)
{
    bbMessageBox(MB_OK, NLS2("$Error_WriteFile$",
        "Error: Could not open \"%s\" for writing."), filename);
}

void bb_rcreader_init(void)
{
    init_rcreader(&g_rc);
} 

char* bbBlackboxrcBuffer(void)
{
    return blackboxrc_path;
}

void bbResetBlackboxrcPath(void)
{
    blackboxrc_path[0] = 0;
}

char* bbExtensionRcBuffer(void)
{
    return extensionsrc_path;
}

void bbResetExtensionRcPath(void)
{
    blackboxrc_path[0] = 0;
}

char* bbMenuRcBuffer(void)
{
    return menurc_path;
}

void bbResetMenuRcPath(void)
{
    menurc_path[0] = 0;
}

char* bbStyleRcBuffer(void)
{
    return stylerc_path;
}

void bbResetStyleRcPath(void)
{
    stylerc_path[0] = 0;
}

} // !namespace bbcore 
