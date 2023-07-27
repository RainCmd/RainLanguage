#include "Encoding.h"
#include <codecvt>
#include <iostream>

std::string UTF16To8(const std::wstring& src)
{
	try
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wcv;
		return wcv.to_bytes(src);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return std::string();
}

std::wstring UTF8To16(const std::string& src)
{
	try
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wcv;
		return wcv.from_bytes(src);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return std::wstring();
}

std::string UTF16ToANSI(const std::wstring& wstr)
{
	std::string ret;
	std::mbstate_t state = {};
	const wchar_t* src = wstr.data();
	size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len)
	{
		std::unique_ptr<char[]> buf(new char[len + 1]);
		len = std::wcsrtombs(buf.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) ret.assign(buf.get(), len);
	}
	return ret;
}

std::wstring ANSIToUTF16(const std::string& str)
{
	std::wstring ret;
	std::mbstate_t state = {};
	const char* src = str.data();
	size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len)
	{
		std::unique_ptr<wchar_t[]> buf(new wchar_t[len + 1]);
		len = std::mbsrtowcs(buf.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) ret.assign(buf.get(), len);
	}
	return ret;
}
