
#include "spacer.h"

spacer::spacer(int typ, barinfo *bi)
    : baritem(typ, bi)
{
    ::ZeroMemory(&mr, sizeof(RECT));
}
