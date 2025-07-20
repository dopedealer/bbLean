
#include <bblib.h>
#include <bbrc.h>

#include "nls.h"
#include "rcfile.h"

#ifdef BBOPT_SUPPORT_NLS

struct nls {
    nls* next;
    unsigned hash;
    int k;
    char *translation;
    char key[1];
};

static nls* pNLS;
bool nls_loaded;

void free_nls(void)
{
    nls* t;
    dolist (t, pNLS)
        free_str(&t->translation);
    freeall(&pNLS);
    nls_loaded = false;
}

static int decode_escape(char* d, const char* s)
{
    char c, e, *d0 = d; do
    {
        c = *s++;
        if ('\\' == c)
        {
            e = *s++;
            if ('n' == e) c = '\n';
            else
            if ('r' == e) c = '\r';
            else
            if ('t' == e) c = '\t';
            else
            if ('\"' == e || '\'' == e || '\\' == e) c = e;
            else --s;
        }
        *d++ = c;
    } while(c);
    return d - d0 - 1;
}

// -------------------------------------------

static void load_nls(void)
{
    const char *lang_file;
    char full_path[MAX_PATH];
    char key[100], line[4000], new_text[4000], *np;
    FILE *fp;
    int nl;

    lang_file = read_string(bbcore::bbExtensionsRcPath(),
        "blackbox.options.translation", NULL);

    if (NULL == lang_file)
        return;

    bbcore::findRCFile(full_path, lang_file, NULL);
    fp = fopen(full_path, "rb");
    if (NULL == fp)
        return;

    key[0] = new_text[0] = 0;
    np = new_text;
    nl = 0;
    for (;;)
    {
        bool eof = false == read_next_line(fp, line, sizeof line);
        char *s = line, c = *s;
        if ('$' == c || eof)
        {
            if (key[0] && new_text[0])
            {
                nls* t = (nls *)c_alloc(sizeof *t + strlen(key));
                t->hash = calc_hash(t->key, key, &t->k, 0);
                t->translation = new_str(new_text);
                cons_node(&pNLS, t);
            }
            if (eof)
                break;
            if (' ' == s[1])
                s += 2;
            decode_escape(key, s);
            new_text[0] = 0;
            np = new_text;
            nl = 0;
            continue;
        }

        if ('#' == c || '!' == c)
            continue;

        if ('\0' != c)
        {
            while (nl--) *np++ = '\n';
            np += decode_escape(np, s);
            nl = 0;
        }

        nl ++;
    }
    fclose(fp);
    reverse_list(&pNLS);
}

// -------------------------------------------
const char* nls2a(const char* i, const char* p)
{
    if (false == nls_loaded)
    {
        load_nls();
        nls_loaded = true;
    }

    if (pNLS)
    {
        char buffer[100];
        int k;
        unsigned hash;
        hash = calc_hash(buffer, i, &k, 0);
        nls* t;
        dolist (t, pNLS)
            if (t->hash==hash && 0==memcmp(buffer, t->key, 1+k))
                return t->translation;
    }
    return p;
}

const char *nls2b(const char* p)
{
    char buffer[100];
    const char *e;
    if (NULL == p || *p != '$' || NULL == (e = strchr(p+1, *p)))
        return p;
    ++e;
    return nls2a(extract_string(buffer, p, e-p), e);
}

const char *nls1(const char* p)
{
    if (NULL == p)
        return p;
    return nls2a(p, p);
}

#endif //def BBOPT_SUPPORT_NLS
