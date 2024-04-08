#pragma once
#include <string>
using namespace std;
struct Args
{
	wstring name;
	wstring path;
	wstring entry;
	int timestep;//ms
	Args(const wstring& name, const wstring& path, const wstring& entry, int timestep) : name(name), path(path), entry(entry), timestep(timestep) {}
};
Args Parse(int cnt, char** args);