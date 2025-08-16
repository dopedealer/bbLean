/* ------------------------------------------------------------------------- */
/*
  This file is part of the bbLean source code
  Copyright © 2004-2009 grischka

  http://bb4win.sourceforge.net/bblean
  http://developer.berlios.de/projects/bblean

  bbLean is free software, released under the GNU General Public License
  (GPL version 2). For details see:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.
*/
/* ------------------------------------------------------------------------- */
/* read7write rc files with cache */

#ifndef _BBRC_H_
#define _BBRC_H_

#include "bblib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RCFILE_HTS 40 // hash table size

/// \brief A descriptor that describes a read line in a corresponding single linked list
struct lin_list
{
    struct lin_list* next;
    struct lin_list* hnext;
    struct lin_list* wnext;
    unsigned hash; ///< a hash for the 'key' part inside a string if calculated (key is part before ":")
    unsigned k, o;
    int i;
    char is_wild;
    char dirty;
    char flags;
    char str[3];
};

/// \brief Opened file descriptor
struct fil_list
{
    struct fil_list* next;
    struct lin_list* lines;
    struct lin_list* wild;
    struct lin_list* ht[RCFILE_HTS];
    unsigned hash;      ///< hash of file path

    char dirty;
    char newfile;
    char tabify;
    char write_error;
    char is_style;
    char is_070;

    int k;              ///< [k]ount bytes in path to file
    char path[1];       ///< Path to file
};

struct rcreader_init
{
    struct fil_list* rc_files;
    void (*write_error)(const char* filename);
    char dos_eol;
    char translate_065;
    char found_last_value;
    char used;
    char timer_set;
};

BBLIB_EXPORT void init_rcreader(struct rcreader_init *init);
BBLIB_EXPORT void reset_rcreader(void);

BBLIB_EXPORT int set_translate_065(int f);
BBLIB_EXPORT int get_070(const char* path);
BBLIB_EXPORT void check_070(struct fil_list *fl);

/// \brief Tells whether it is a style file.
/// Currently it reads all file to a memory (up to 10000 bytes) and tries to
/// find a "menu.frame" inside. 
/// \returns 0 - not style file. 1 - it is.
BBLIB_EXPORT int is_stylefile(const char *path);

BBLIB_EXPORT FILE *create_rcfile(const char *path);

/// \brief Gets size of file, allocates size+1 bytes with malloc, reads file content, adds
///        trailing zero
/// \param path Path to file
/// \param max_len Maximum bytes to read. 0 if no limit present.
/// \returns Pointer to allocated string in case of success. Null otherwise. 
BBLIB_EXPORT char* read_file_into_buffer(const char* path, int max_len);

/// \brief Scan one line in a buffer, advance read pointer, set start pointer
///        and length for the caller.
/// Replaces tabs to spaces in base buffer. Adds null bytes in base buffer to
/// found separate lines, so they can be examined with strlen. Skips front
/// spaces and trailing spaces.
/// \param pp Pointer to pointer to input string buffer. Modified.
/// \param ll Pointer to length value of recognized string value
/// \returns Non-empty char of line in case of success. Null on end of string reached and when no more calling needed.
BBLIB_EXPORT char scan_line(char **pp, char **ss, int *ll);

BBLIB_EXPORT int read_next_line(FILE *fp, char* szBuffer, unsigned dwLength);

//Note that pointers returned from 'read_string' and 'read_value' are valid only
//until the next Read/Write call. For later usage, you need to copy the string
//into a place within your code. 
// [~] Due to single linked list realisation the cost of each such 'read value' operation is MxN (i.e N^2)
// where M - is number of opened files in list and N - number of cached strings
// in list assigned to this opened file.

/// \brief Searches the given file for the supplied keyword and returns a
///        pointer to the value - string
/// \param path String containing the name of the file to be opened
/// \param szKey String containing the keyword to be looked for
/// \param ptr [opt] An index into the file to start search. If present, at input indicates the line
///                  from where the search starts, at output is set to the line that follows the match.
/// \returns Pointer to the value string of the keyword 
BBLIB_EXPORT const char* read_value(const char* path, const char* szKey, long *ptr);

/// \brief Searches the given file for the supplied keyword as true/false
///        string and returns whether it was true or false or bDefault in case of fail
/// \param path  String containing the name of the file to be opened
/// \param szKey String containing the keyword to be looked for
/// \param defaultValue  Default value that will be returned in case of fail recognition
/// \returns Result of recognition
BBLIB_EXPORT bool read_bool(const char* fileName, const char* szKey, bool defaultValue);

/// \brief Searches the given file for the supplied keyword as int
///        returns it if it was recoghized. In case of failure returns default value
/// \param path  String containing the name of the file to be opened
/// \param szKey String containing the keyword to be looked for
/// \param defaultValue  Default value that will be returned in case of fail recognition
/// \returns Result of recognition
BBLIB_EXPORT int read_int(const char* fileName, const char* szKey, int defaultValue);

/// \brief Searches the given file for the supplied keyword as string
///        and returns it if it was recoghized. In case of failure returns default value
/// \param path  String containing the name of the file to be opened
/// \param szKey String containing the keyword to be looked for
/// \param defaultValue  Default value that will be returned in case of fail recognition
/// \returns Result of recognition
BBLIB_EXPORT const char* read_string(const char* fileName, const char* szKey, const char* szDefault);

BBLIB_EXPORT int found_last_value(void);

/// \brief Searches the given key in readed strings for provided file path and
///        writes value to cached info. If file not opened and not present in
///        cache - opens it and reads to cache before it.
/// \param path Path to file
/// \param szKey Key that is searched
/// \param value String value that will be written to cache
BBLIB_EXPORT void write_value(const char* path, const char* szKey, const char* value);

// all write_XXX functions works the same way as write_value
BBLIB_EXPORT void write_bool(const char* fileName, const char* szKey, bool value);
BBLIB_EXPORT void write_int(const char* fileName, const char* szKey, int value);
BBLIB_EXPORT void write_string(const char* fileName, const char* szKey, const char* value);
BBLIB_EXPORT void write_color(const char* fileName, const char* szKey, COLORREF value);

BBLIB_EXPORT int rename_setting(const char* path, const char* szKey, const char* new_keyword);
BBLIB_EXPORT int delete_setting(LPCSTR path, LPCSTR szKey);

/* ------------------------------------------------------------------------- */
/* parse a StyleItem */

struct StyleItem;

BBLIB_EXPORT void parse_item(LPCSTR szItem, struct StyleItem *item);
BBLIB_EXPORT int findtex(const char *p, int prop);
BBLIB_EXPORT struct styleprop{ const char *key; int val; };
BBLIB_EXPORT const struct styleprop *get_styleprop(int prop);

/* ------------------------------------------------------------------------- */
/* only used in bbstylemaker */

BBLIB_EXPORT int scan_component(const char **p);
BBLIB_EXPORT int xrm_match (const char *key, const char *pat);

/// \brief Tries to read rc format file and store it's content in single linked list
///        one-by-one and caches opened file in global opened files info cache
///        (also in single linked list)
BBLIB_EXPORT struct fil_list* read_file(const char* filename);

BBLIB_EXPORT struct lin_list *make_line(struct fil_list *fl, const char *key, const char *val);
BBLIB_EXPORT void free_line(struct fil_list *fl, struct lin_list *tl);
BBLIB_EXPORT struct lin_list **get_simkey(struct lin_list **slp, const char *key);
BBLIB_EXPORT void make_style070(struct fil_list *fl);
BBLIB_EXPORT void make_style065(struct fil_list *fl);

/* ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif

#endif
