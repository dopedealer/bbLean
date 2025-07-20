#ifndef _BBCORE_UTILS_H
#define _BBCORE_UTILS_H

namespace bbcore {
int bbMessageBox(int flg, const char *fmt, ...);
int bbMessageBoxV(int flg, const char *fmt, va_list args);
int mboxErrorFile(const char* szFile);
int mboxErrorValue(const char* szValue);

void log(const char* Title, const char* Line);

void bbDrawPixImpl(HDC hDC, RECT* rc, COLORREF color, int pic);
void bbDrawTextImpl(HDC hDC, const char* text, RECT* p_rect, unsigned format, COLORREF c);
int bbMbyteToWideChar(const char* src, WCHAR* wstr, int len);
int bbWideCharToMbyte(const WCHAR *src, char *str, int len);

} // !namespace bbcore


#endif //!_BBCORE_UTILS_H
