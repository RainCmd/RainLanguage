#pragma once
#include <string>
#include <vector>
using namespace std;
struct Args
{
	wstring name;
	wstring path;
	vector<wstring> referencePath;
	wstring entry;
	wstring forcedReference;
	int errorLevel;
	int timestep;//ms
	bool debug;
	bool silent;
	wstring out;
	Args() : name(), path(), referencePath(), entry(), forcedReference(), errorLevel(), timestep(), debug(), silent(), out() {}
	Args(const wstring& name, const wstring& path, const wstring& entry, int errorLevel, int timestep, bool debug, bool silent, wstring out) : name(name), path(path), referencePath(), entry(entry), forcedReference(), errorLevel(errorLevel), timestep(timestep), debug(debug), silent(silent), out(out) {}
};
Args Parse(int cnt, char** args);