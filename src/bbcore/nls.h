
#ifndef _BBCORE_NLS_H
#define _BBCORE_NLS_H


// Native Language Support (see code in nls.cpp) 
#define BBOPT_SUPPORT_NLS

#ifdef BBOPT_SUPPORT_NLS
const char* nls1(const char *p);
const char* nls2a(const char *i, const char *p);
const char* nls2b(const char *p);
void free_nls(void);
# define NLS0(S) S
# define NLS1(S) nls1(S)
# define NLS2(I,S) nls2b(I S)
#else
# define free_nls()
# define NLS0(S) S
# define NLS1(S) (S)
# define NLS2(I,S) (S)
#endif







#endif //!_BBCORE_NLS_H
