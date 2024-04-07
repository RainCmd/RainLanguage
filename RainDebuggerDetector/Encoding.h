#pragma once
#include <string>

std::string UTF_16To8(const std::wstring src);
std::wstring UTF_8To16(const std::string src);