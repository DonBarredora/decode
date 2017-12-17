#include <iostream>
#include "window.h"
#include "str_convert.h"

Window::Window(const WNDCLASSEX& clss) : m_handle(nullptr), m_class(clss), procedure(clss.lpfnWndProc)
{
	if (!RegisterClassEx(&m_class)) {
		std::wstring error_msg = std::wstring{L"Error when attempting to register class "} + m_class.lpszClassName;
		throw std::runtime_error(wstring_to_string(error_msg));
	}
	
	m_handle = CreateWindowEx(0, m_class.lpszClassName, L"Base 64 decode",
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
				
	if (!m_handle) {
		std::wstring error_msg = std::wstring{L"Error when attempting to create window of class "} + m_class.lpszClassName;
		throw std::runtime_error(wstring_to_string(error_msg));
	}
}

HWND Window::getHandle() const
{
	return m_handle;
}