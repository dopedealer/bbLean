
#include <bblib.h>

#include "parse.h"
#include "system.h"

namespace bbcore { 

/// \brief Put first token from source into target. Returns: ptr to rest of
///        source. Put first token of 'string' (seperated by one of delims)
/// \returns Pointer to the rest into 'string'
const char* tokenize(const char* string, char* buf, const char* delims)
{
    ::NextToken(buf, &string, delims);
    return string;
}

/// \brief Put first 'numTokens' from 'source' into 'targets', copy rest into
///        'remaining', if it's not NULL. The 'targets' and 'remaining' buffers
///        are zero-terminated always. Returns the number of actual tokens
///        found.
/// Assigns a specified number of string variables and outputs the
/// remaining to a variable
int bbTokenize(
    const char* srcString,
    char **lpszBuffers,
    unsigned dwNumBuffers,
    char* szExtraParameters)
{
    const char *s = srcString;
    int stored = 0;
    unsigned c;

    //debug_printf("BBTokenize [%d] <%s>", dwNumBuffers, srcString);
    for (c = 0; c < dwNumBuffers; ++c)
    {
        const char *a; int n; char *out;
        n = nexttoken(&a, &s, NULL);
        if (n)
        {
            if (('\''  == a[0] || '\"' == a[0]) && n >= 2 && a[n-1] == a[0])
                ++a, n -= 2; /* remove quotes */
            ++stored;
        }
        out = lpszBuffers[c];
        extract_string(out, a, imin(n, MAX_PATH-1));
    }
    if (szExtraParameters)
    {
        strcpy_max(szExtraParameters, s, MAX_PATH);
    }
    return stored;
}

/// \brief Is searchString part of inputString (letter case ignored)
/// Checks a given string to an occurance of the second string
bool isInString(const char* inputString, const char* searchString)
{
    // xoblite-flavour plugins bad version test workaround
#ifdef __BBCORE__
    if (0 == strcmp(searchString, "bb") && 0 == strcmp(inputString, getBBVersion()))
    {
        return false;
    }
#endif
    return NULL != stristr(inputString, searchString);
}

/// \brief Remove first and last character from string.
/// Literally copies next N symbols to start with memcpy(!!) and writes 'new' ending zero
char* strRemoveEncap(char* str)
{
    int l = strlen(str);
    if (l >= 2)
    {
        extract_string(str, str+1, l-2);
    }
    return str;
} 

} // !namespace bbcore
