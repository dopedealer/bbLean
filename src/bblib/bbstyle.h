#ifndef _BBSTYLE_H
#define _BBSTYLE_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------ */
/* BImage definitions */

/* Gradient types */
#define B_HORIZONTAL 0
#define B_VERTICAL 1
#define B_DIAGONAL 2
#define B_CROSSDIAGONAL 3
#define B_PIPECROSS 4
#define B_ELLIPTIC 5
#define B_RECTANGLE 6
#define B_PYRAMID 7
#define B_SOLID 8

/* Bevelstyle */
#define BEVEL_FLAT 0
#define BEVEL_RAISED 1
#define BEVEL_SUNKEN 2

/* Bevelposition */
#define BEVEL1 1
#define BEVEL2 2

/* bullet styles for menus */
#define BS_EMPTY 0
#define BS_TRIANGLE 1
#define BS_SQUARE 2
#define BS_DIAMOND 3
#define BS_CIRCLE 4
#define BS_CHECK 5

/* =========================================================================== */
/* StyleItem */

typedef struct StyleItem
{
    /* 0.0.80 */
    int bevelstyle;
    int bevelposition;
    int type;
    bool parentRelative;
    bool interlaced;

    /* 0.0.90 */
    COLORREF Color;
    COLORREF ColorTo;
    COLORREF TextColor;
    int FontHeight;
    int FontWeight;
    int Justify;
    int validated;

    char Font[128];

    /* bbLean 1.16 */
    int nVersion;
    int marginWidth;
    int borderWidth;
    COLORREF borderColor;
    COLORREF foregroundColor;
    COLORREF disabledColor;
    bool bordered;
    bool FontShadow; /* xoblite */

    char reserved[102]; /* keep sizeof(StyleItem) = 300 */

} StyleItem;

typedef struct ShortStyleItem
{
    /* 0.0.80 */
    int bevelstyle;
    int bevelposition;
    int type;
    bool parentRelative;
    bool interlaced;
    /* 0.0.90 */
    COLORREF Color;
    COLORREF ColorTo;
    COLORREF TextColor;
    int FontHeight;
    int FontWeight;
    int Justify;
    int validated;
    char Font[16];
} ShortStyleItem;


#ifdef __cplusplus
}
#endif

#endif //!_BBSTYLE_H
