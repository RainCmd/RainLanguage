#pragma once
#include <string>
using namespace std;
struct Args
{
	wstring name;
	wstring path;
	wstring entry;
	int errorLevel;
	int timestep;//ms
	bool debug;
	bool silent;
	Args(const wstring& name, const wstring& path, const wstring& entry, int errorLevel, int timestep, bool debug, bool silent) : name(name), path(path), entry(entry), errorLevel(errorLevel), timestep(timestep), debug(debug), silent(silent) {}
};
Args Parse(int cnt, char** args);