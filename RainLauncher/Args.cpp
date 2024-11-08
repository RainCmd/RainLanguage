#include "windows.h"
#include <stringapiset.h>
#include <sstream>
#include "Args.h"

static wstring S2WS(string src)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
	if(!len) return L"";
	wchar_t* pwc = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, pwc, len);
	wstring result = pwc;
	delete[] pwc;
	return result;
	return L"";
}
static bool CheckCmd(const char* left, const char* right)
{
	while((*left | 0x20) == (*right | 0x20))
	{
		if(!*left) return true;
		left++; right++;
	}
	return false;
}
static void SplitPath(vector<wstring>& paths, const wstring& str)
{
	wstringstream ss(str);
	wstring path;
	while(getline(ss, path, L';'))
		if(!path.empty())
			paths.push_back(path);
}
Args Parse(int cnt, char** args)
{
	Args result = Args(L"雨言测试工程", L".\\", L"Main", 4, 0, false, false, L"");
	for(size_t i = 0; i < cnt; i++)
	{
		char* arg = args[i];
		if(*arg != '-' && *arg != '/') continue;
		arg++;
		for(size_t j = 0; arg[j]; j++) arg[j] |= 0x20;
		if(CheckCmd(arg, "name"))
		{
			if(++i >= cnt) break;
			result.name = S2WS(args[i]);
		}
		else if(CheckCmd(arg, "path"))
		{
			if(++i >= cnt) break;
			result.path = S2WS(args[i]);
		}
		else if(CheckCmd(arg, "referencePath"))
		{
			if(++i >= cnt) break;
			SplitPath(result.referencePath, S2WS(args[i]));
		}
		else if(CheckCmd(arg, "entry"))
		{
			if(++i >= cnt) break;
			result.entry = S2WS(args[i]);
		}
		else if(CheckCmd(arg, "forcedReference"))
		{
			if(++i >= cnt) break;
			result.forcedReference = S2WS(args[i]);
		}
		else if(CheckCmd(arg, "errorlevel"))
		{
			if(++i >= cnt) break;
			result.errorLevel = atoi(args[i]);
			if(result.errorLevel < 0) result.errorLevel = 0;
			else if(result.errorLevel > 8) result.errorLevel = 8;
		}
		else if(CheckCmd(arg, "timestep"))
		{
			if(++i >= cnt) break;
			result.timestep = atoi(args[i]);
		}
		else if(CheckCmd(arg, "debug")) result.debug = true;
		else if(CheckCmd(arg, "silent")) result.silent = true;
		else if(CheckCmd(arg, "out"))
		{
			if(++i >= cnt) break;
			result.out = S2WS(args[i]);
		}
	}
	return result;
}