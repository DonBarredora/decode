#ifndef WINDOW_H
#define WINDOW_H

#include <string>

#ifndef UNICODE
#define UNICODE
#endif
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

class Window {
public:
	Window(const WNDCLASSEX&);
	virtual ~Window() = default;
	virtual HWND getHandle() const;
	
private:
	HWND m_handle;
	WNDCLASSEX m_class;
	WNDPROC procedure;
};

#endif // WINDOW_H