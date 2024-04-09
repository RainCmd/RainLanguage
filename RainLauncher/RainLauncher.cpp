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
	CodeLoadHelper(wstring dir) { LoadFiles(dir.append(L"\\")); }
	bool LoadNext()
	{
		if(files.empty()) return false;
		path = files.back();
		files.pop_back();
		return true;
	}
	const RainString CurrentPath() { return RainString(path.c_str(), (uint32)path.length()); }
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
	uint32 s = fullName.length;
	while(--s > 0)
		if(fullName.value[s] == '.')
		{
			s++;
			break;
		}
	wstring fn(fullName.value + s, fullName.length - s);
	if(fn == L"Print") return Print;

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
int main(int cnt, char** _args)
{
	Args args = Parse(cnt, _args);
	CodeLoadHelper helper(args.path);
	BuildParameter parameter(RainString(args.name.c_str(), (uint32)args.name.size()), args.debug, &helper, nullptr, nullptr, ErrorLevel::LoggerLevel4);
	product = Build(parameter);
	for(uint32 i = 0; i <= (uint32)ErrorLevel::LoggerLevel4; i++)
	{
		ErrorLevel lvl = (ErrorLevel)i;
		uint32 errCnt = product->GetLevelMessageCount(lvl);
		for(uint32 j = 0; j < errCnt; j++)
		{
			auto msg = product->GetErrorMessage(lvl, j);
			if(msg.message.length)
				wcout << wstring(msg.message.value, msg.message.length) << "\n";
			wcout << msg.path.value << " line:" << msg.line << " [" << msg.start << ", " << msg.start + msg.length << "]\nERR CODE:" << (uint32)msg.type << endl;
		}
	}
	if(product->GetLevelMessageCount(ErrorLevel::Error)) wcout << L"编译失败！" << endl;
	else
	{
		const RainLibrary* library = product->GetLibrary();
		StartupParameter parameter(&library, 1, nullptr, nullptr, [](const RainString& name) { return product->GetLibrary(); }, nullptr, NativeLoader, OnExceptionExitFunc);
		RainKernel* kernel = CreateKernel(parameter);
		RainFunction entry = kernel->FindFunction(args.entry.c_str(), true);
		if(entry.IsValid())
		{
			InvokerWrapper invoker = entry.CreateInvoker();
			if(args.debug)
			{
				RegistDebugger(kernel, [](const RainString& name) { return product->GetRainProgramDatabase(); }, nullptr);
				char success;
				cin >> success;
				if((success | 0x20) != 'y')
				{
					wcout << "fail to debug" << endl;
					delete product;
					return 0;
				}
			}
			invoker.Start(true, false);
			while(kernel->GetState().taskCount)
			{
				this_thread::sleep_for(chrono::milliseconds(args.timestep));
				kernel->Update();
			}
		}
		else wcout << "entry:" << args.entry << " not found" << endl;
	}
	delete product;
	return 0;
}