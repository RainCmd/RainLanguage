#include "Args.h"
#include <Builder.h>
#include <VirtualMachine.h>
#include <Debugger.h>

#include <iostream>
#include <string>
#include <io.h>
#include <fstream>
#include <vector>
#include <thread>

static bool EndWidth(wstring src, wstring suffix)
{
	if(src.size() < suffix.size()) return false;
	auto sidx = src.size() - suffix.size();
	for(size_t i = 0; i < suffix.size(); i++)
		if(src.at(sidx + i) != suffix.at(i))
			return false;
	return true;
}
class CodeLoadHelper : public CodeLoader
{
	vector<wstring> files;
	wstring path;
	wstring rpath;
	wstring content;
	void LoadFiles(wstring dir)
	{
		_wfinddata_t data;
		auto handle = _wfindfirst(wstring(dir).append(L"*").c_str(), &data);
		if(handle != -1)
		{
			do
			{
				if(EndWidth(data.name, L".") || EndWidth(data.name, L"..")) continue;
				wstring path = wstring(dir).append(data.name);
				if(data.attrib & _A_SUBDIR)
					LoadFiles(path.append(L"\\"));
				else if(EndWidth(path, L".rain"))
					files.push_back(path);
			}
			while(_wfindnext(handle, &data) == 0);
		}
		_findclose(handle);
	}
public:
	wstring workspace;
	CodeLoadHelper(wstring dir)
	{
		wchar_t lastChar = dir[dir.size() - 1];
		if(lastChar != '\\' && lastChar != '/') dir = dir.append(L"\\");
		workspace = dir;
		LoadFiles(dir);
	}
	bool LoadNext()
	{
		if(files.empty()) return false;
		path = files.back();
		files.pop_back();
		return true;
	}
	const RainString CurrentPath()
	{
		rpath = path.substr(workspace.size());
		return RainString(rpath.c_str(), (uint32)rpath.length());
	}
	const RainString CurrentContent()
	{
		wfstream file(path);
		content.clear();
		wstring line;
		while(getline(file, line))
		{
			content.append(line);
			content.append(L"\n");
		}
		return RainString(content.c_str(), (uint32)content.length());
	}
};


static void Print(RainKernel&, CallerWrapper& caller)
{
	wcout << caller.GetStringParameter(0).value;
	wcout.flush();
}
static void NativeHelper(RainKernel&, CallerWrapper&) {}
static OnCaller NativeLoader(RainKernel& kernel, const RainString fullName, const RainType* parameterTypes, uint32 parameterCount)
{
	if(parameterCount == 1 && *parameterTypes == RainType::String)
	{
		uint32 s = fullName.length;
		while(--s > 0)
			if(fullName.value[s] == '.')
			{
				s++;
				break;
			}
		wstring fn(fullName.value + s, fullName.length - s);
		if(fn == L"Print") return Print;
	}

	wcout << "本地函数绑定失败：" << fullName.value << endl;
	return NativeHelper;
}

static void OnExceptionExitFunc(RainKernel&, const RainStackFrame* frames, uint32 frameCount, const RainString msg)
{
	wcout << L"异常信息:" << msg.value << L"\n";
	for(size_t i = 0; i < frameCount; i++)
		wcout << frames[i].libraryName.value << L"." << frames[i].functionName.value << "\t" << frames[i].address << endl;
}

static RainProduct* product;
static wstring name;
static bool IsName(const RainString& name)
{
	if(name.length != ::name.size()) return false;
	for(uint32 i = 0; i < name.length; i++)
		if(name.value[i] != ::name.c_str()[i])return false;
	return true;
}
static const RainLibrary* LibraryLoader(const RainString& name)
{
	if(IsName(name)) return product->GetLibrary();
	return nullptr;
}
static const RainProgramDatabase* ProgramDatabaseLoader(const RainString& name)
{
	if(IsName(name)) return product->GetRainProgramDatabase();
	return nullptr;
}
int main(int cnt, char** _args)
{
	wcout.imbue(locale("zh_CN.UTF-8"));
	Args args = Parse(cnt, _args);

	name = args.name;
	CodeLoadHelper helper(args.path);
	BuildParameter parameter(RainString(args.name.c_str(), (uint32)args.name.size()), args.debug, &helper, nullptr, nullptr, (ErrorLevel)args.errorLevel);
	if(!args.silent) wcout << L"开始编译" << endl;
	clock_t startTime = clock();
	product = Build(parameter);
	if(!args.silent) wcout << L"编译结束，用时 " << (clock() - startTime) / (CLOCKS_PER_SEC / 1000) << "ms" << endl;
	if(args.debug && product->GetLevelMessageCount(ErrorLevel::Error))
	{
		wcout << L"<compilation failure>";
		wcout.flush();
	}
	for(uint32 i = 0; i <= (uint32)ErrorLevel::LoggerLevel4; i++)
	{
		ErrorLevel lvl = (ErrorLevel)i;
		uint32 errCnt = product->GetLevelMessageCount(lvl);
		for(uint32 j = 0; j < errCnt; j++)
		{
			auto msg = product->GetErrorMessage(lvl, j);
			if(msg.message.length) wcout << wstring(msg.message.value, msg.message.length) << endl;
			wcout << "ERR CODE:" << (uint64)msg.type << endl;
			wcout << helper.workspace << msg.path.value << " line:" << msg.line << " [" << msg.start << ", " << msg.start + msg.length << "]" << endl;
		}
	}
	if(product->GetLevelMessageCount(ErrorLevel::Error))
	{
		if(!args.silent) wcout << L"编译失败" << endl;
	}
	else
	{
		if(args.debug)
		{
			wcout << L"<ready to connect debugger>";
			wcout.flush();
			char success;
			cin >> success;
			if((success | 0x20) != 'y')
			{
				wcout << "fail to debug" << endl;
				delete product;
				return 0;
			}
		}
		const RainLibrary* library = product->GetLibrary();
		StartupParameter parameter(&library, 1, nullptr, nullptr, LibraryLoader, nullptr, NativeLoader, OnExceptionExitFunc);
		RainKernel* kernel = CreateKernel(parameter, ProgramDatabaseLoader, nullptr);
		RainFunction entry = kernel->FindFunction(args.entry.c_str(), true);
		if(entry.IsValid())
		{
			InvokerWrapper invoker = entry.CreateInvoker();
			invoker.Start(true, false);
			while(kernel->GetState().taskCount)
			{
				this_thread::sleep_for(chrono::milliseconds(args.timestep));
				kernel->Update();
			}
		}
		else wcout << "entry:" << args.entry << " not found" << endl;
		Delete(kernel);
	}
	delete product;
	return 0;
}