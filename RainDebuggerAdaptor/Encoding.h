#pragma once
#include <string>
#include "Rain.h"

std::string UTF16To8(const std::wstring& src);
std::wstring UTF8To16(const std::string& src);

std::string UTF16ToANSI(const std::wstring& src);
std::wstring ANSIToUTF16(const std::string& src);