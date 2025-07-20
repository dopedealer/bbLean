#ifndef _BB_APPLICATION_H
#define _BB_APPLICATION_H

// Intended to contain runtime-state app logic

namespace bbcore {

LPCSTR getOSInfo(void);
bool getUnderExplorer(void);
HWND getBBWnd(void);
void getBlackboxEditor(char* editor);

} // !namespace bbcore



#endif //!_BB_APPLICATION_H
