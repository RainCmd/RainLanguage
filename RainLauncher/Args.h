#pragma once
#include <string>
using namespace std;
struct Args
{
	wstring name;
	wstring path;
	wstring entry;
	int fps;
	Args(const wstring& name, const wstring& path, const wstring& entry, int fps) : name(name), path(path), entry(entry), fps(fps) {}
};
Args Parse(int cnt, char** args);