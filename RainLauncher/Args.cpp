#include "windows.h"
#include <stringapiset.h>
#include "Args.h"

static wstring S2WS(string src)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
	if(!len) return L"";
	wchar_t* pwc = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, pwc, len);
	wstring result = wstring(pwc, len);
	delete[] pwc;
	return result;
	return L"";
}
static bool CheckCmd(const char* left, const char* right)
{
	if(!*left) return false;
	if(*left != '-' && *left != '/') return false;
	left++;
	while((*left | 0x20) == *right)
	{
		if(!*left) return true;
		left++; right++;
	}
	return false;
}
Args Parse(int cnt, char** args)
{
	Args result = Args(L"”Í—‘≤‚ ‘π§≥Ã", L".\\", L"Main", 10);
	for(size_t i = 0; i < cnt; i++)
	{
		char* arg = args[i];
		if(*arg != '-') continue;
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
		else if(CheckCmd(arg, "entry"))
		{
			if(++i >= cnt) break;
			result.entry = S2WS(args[i]);
		}
		else if(CheckCmd(arg, "timestep"))
		{
			if(++i >= cnt) break;
			result.timestep = atoi(args[i]);
		}
	}
	return result;
}