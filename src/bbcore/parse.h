#ifndef _BBCORE_PARSE_H
#define _BBCORE_PARSE_H

namespace bbcore { 

const char* tokenize(const char* string, char* buf, const char* delims);

int bbTokenize(
    const char* srcString,
    char **lpszBuffers,
    unsigned dwNumBuffers,
    char* szExtraParameters);

bool isInString(const char* inputString, const char* searchString);
char* strRemoveEncap(char* str);

} // !namespace bbcore



#endif //!_BBCORE_PARSE_H
