#ifndef _BBOX_EXPORTS_H
#define _BBOX_EXPORTS_H

#include <Blackbox.h>

using BlackboxAppType = bbcore::Blackbox;

// singletone. For exported APIs only. Inited manually in main.
extern BlackboxAppType* gApp;

#endif //!_BBOX_EXPORTS_H
