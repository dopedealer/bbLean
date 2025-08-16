#ifndef _BBCORE_RCFILE_H
#define _BBCORE_RCFILE_H


namespace bbcore {

COLORREF readColor(const char* fileName, const char* szKey, const char* defaultColor);
bool deleteSetting(LPCSTR path, LPCSTR szKey);
bool renameSetting(const char* path, const char* szKey, const char* new_keyword);

const char* bbDefaultrcPath(void);
void bbResetDefaultrcPath(void);

//[[deprecated]]  // must be refactored and removed! Writing to unknown size buffer is bad
char* bbBlackboxrcBuffer(void);
void bbResetBlackboxrcPath(void);

void bb_rcreader_init(void);

//[[deprecated]]  // must be refactored and removed! Writing to unknown size buffer is bad
char* bbDefaultrcBuffer(void);

bool findRCFile(char* pszOut, const char* filename, HINSTANCE module);
const char* bbConfigFileExists(const char* filename, const char* pluginDir);

//[[deprecated]]  // must be refactored and removed! Writing to unknown size buffer is bad
char* bbPluginrcBuffer(void);
void bbResetPluginRcPath(void);

//[[deprecated]]  // must be refactored and removed! Writing to unknown size buffer is bad
char* bbExtensionRcBuffer(void);
void bbResetExtensionRcPath(void);

//[[deprecated]]  // must be refactored and removed! Writing to unknown size buffer is bad
char* bbMenuRcBuffer(void);
void bbResetMenuRcPath(void);

//[[deprecated]]  // must be refactored and removed! Writing to unknown size buffer is bad
char* bbStyleRcBuffer(void);
void bbResetStyleRcPath(void);

/// \brief Opens file for parsing
/// Open/Close a file for use with 'FileRead' or 'ReadNextCommand'
/// (Do not use 'fopen/fclose' in combination with these)
FILE* fileOpen(const char* szPath);

/// \brief Close selected file
bool fileClose(FILE* fp);

/// \brief Read's a line from given FILE and returns boolean on status
bool fileRead(FILE* fp, char* buffer); 

/// \brief Reads the next line of the file
/// Additionally skips comments and empty lines
bool readNextCommand(FILE *fp, char* szBuffer, unsigned dwLength); 

const char* bbStylePath(const char* other);
const char* bbPluginsRcPath(const char* other);
const char* bbMenuPath(const char* other);
const char* bbExtensionsRcPath(const char* other = nullptr);
const char* bbRcPath(const char* other);

} // !namespace bbcore


#endif //!_BBCORE_RCFILE_H
