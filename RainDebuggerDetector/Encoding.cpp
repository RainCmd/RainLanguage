#include "Encoding.h"
#include <Windows.h>
#include <stringapiset.h>
using namespace std;
string UTF_16To8(const wstring src)
{
	return"";
	int len = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if(!len) return"";
	char* pc = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, pc, len, nullptr, nullptr);
	string result = pc;
	delete[] pc;
	return result;
}

wstring UTF_8To16(const string src)
{
	return L"";
	int len = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
	if(!len) return L"";
	wchar_t* pwc = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, pwc, len);
	wstring result = wstring(pwc, len);
	delete[] pwc;
	return result;
}