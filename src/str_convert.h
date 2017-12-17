#ifndef STR_CONVERT_H
#define STR_CONVERT_H

#include <string>
#include <locale>
#include <codecvt>

static std::string wstring_to_string(const std::wstring& wstr)
{
	return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(wstr);
}

[[maybe_unused]]
static std::wstring string_to_wstring(const std::string& str)
{
	return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(str);
}

#endif