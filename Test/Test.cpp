#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <io.h>
#include <Builder.h>
#include <VirtualMachine.h>
#include <MemoryAllocator.h>
#include <map>
#include <vector>
#include <thread>
#include <time.h>
#include <fstream>

using namespace std;

class TestCodeLoader : public CodeLoader
{
	vector<wstring> files;
	wstring path;
	wstring content;
	bool EndWith(wstring src, wstring suffix)
	{
		if (src.size() < suffix.size()) return false;
		size_t d = src.size() - suffix.size();
		for (size_t i = 0; i < suffix.size(); i++)
			if (src.at(i + d) != suffix.at(i))
				return false;
		return true;
	}
	void LoadFiles(wstring dir) {
		_wfinddata_t data;
		wstring tmp;
		intptr_t handle = _wfindfirst(tmp.assign(dir).append(L"*").c_str(), &data);
		if (handle != -1)
		{
			do {
				wstring path = data.name;
				if (EndWith(path, L".")) continue;
				path = tmp.assign(dir).append(path);
				if (data.attrib & _A_SUBDIR)
					LoadFiles(path.append(L"\\"));
				else if (EndWith(path, L".rain"))
					files.push_back(path);
			} while (_wfindnext(handle, &data) == 0);
		}
		_findclose(handle);
	}
public:
	TestCodeLoader(wstring dir) {
		LoadFiles(dir);
	}
	bool LoadNext() {
		if (files.empty()) return false;
		path = files.back();
		files.pop_back();
		return true;
	}
	const RainString CurrentPath() {
		return RainString(path.c_str(), path.length());
	}
	const RainString CurrentContent() {
		wfstream file(path);
		content.clear();
		wstring line;
		while (getline(file, line))
		{
			content.append(line);
			content.append(1, L'\n');
		}
		return RainString(content.c_str(), content.length());
	}
};

void Print(RainKernel&, CallerWrapper& caller)
{
	const RainString value = caller.GetStringParameter(0);
	wstring str;
	str.assign(value.value, value.length);
	wcout << str;// << L"\n";
}

void Clock(RainKernel&, CallerWrapper& caller)
{
	caller.SetReturnValue(0, (integer)clock());
}

void OpenFile(RainKernel&, CallerWrapper& caller)
{
	const RainString value = caller.GetStringParameter(0);
	wstring path;
	path.assign(value.value, value.length);
	wfstream* file = new wfstream(path);
	caller.SetEntityReturnValue(0, (uint64)file);
}
void FileReadLine(RainKernel&, CallerWrapper& caller)
{
	wfstream* file = (wfstream*)caller.GetEntityParameter(0);
	wstring line;
	bool end = !(bool)getline(*file, line);
	caller.SetReturnValue(0, end);
	caller.SetReturnValue(1, line.c_str());
}
void CloseFile(RainKernel&, CallerWrapper& caller)
{
	wfstream* file = (wfstream*)caller.GetEntityParameter(0);
	file->close();
	delete file;
}


OnCaller NativeLoader(RainKernel&, const RainString fullName, const RainType* parameters, uint32 parametersCount)
{
	wstring str; str.assign(fullName.value, fullName.length);
	//wcout << "\n\nNative Load:" << str << "\n\n";
	if (str == L"TestLib.Print") return Print;
	else if (str == L"TestLib.Clock") return Clock;
	else if (str == L"TestLib.OpenFile") return OpenFile;
	else if (str == L"TestLib.FileReadLine") return FileReadLine;
	else if (str == L"TestLib.CloseFile") return CloseFile;
	return nullptr;
}


map<long long, int> mmap;
int midx = 0;
void OnAlloc(long long key, int idx)
{
	auto it = mmap.find(key);
	if (it != mmap.end())
	{
		if (it->second >= 0)
			cout << "realloc" << endl;
	}
}
void OnFree(int idx)
{
	if (idx < 0)
	{
		cout << "refree" << endl;
	}
}
void* ALLOC(uint32 size)
{
	void* result = malloc((size_t)size);
	midx++;
	OnAlloc((long long)result, midx);
	mmap[(long long)result] = midx;
	return result;
}
void* REALLOC(void* pointer, uint32 size)
{
	OnFree(mmap[(long long)pointer]);
	mmap[(long long)pointer] = -1;
	void* result = realloc(pointer, (size_t)size);
	midx++;
	OnAlloc((long long)result, midx);
	mmap[(long long)result] = midx;
	return result;
}
void FREE(void* pointer)
{
	OnFree(mmap[(long long)pointer]);
	mmap[(long long)pointer] = -1;
	free(pointer);
}

void OnExce(RainKernel&, const RainStackFrame* stackFrames, uint32 stackFrameCount, const RainString message)
{
	wstring str;
	str.assign(message.value, message.length);
	wcout << L"异常信息:" << str << "\n";
	wcout << L"堆栈:\n";
	for (uint32 i = 0; i < stackFrameCount; i++)
	{
		str.assign(stackFrames[i].libraryName.value, stackFrames[i].libraryName.length);
		wcout << str << ":";
		str.assign(stackFrames[i].functionName.value, stackFrames[i].functionName.length);
		wcout << str << " [" << stackFrames[i].address << "]\n";
	}
}

const RainLibrary* test2Lib;
const RainLibrary* OnLibraryLoader(const RainString& libName)
{
	return test2Lib;
}
void TestFunc()
{
	/*TestCodeLoader loader2(L".\\RainTest2\\");
	BuildParameter parameter2(RainString::Create(L"TestLib2"), false, &loader2, nullptr, ErrorLevel::WarringLevel4);
	RainProduct* product2 = Build(parameter2);
	const RainLibrary* lib2 = product2->GetLibrary();
	const RainBuffer<uint8>* buf2 = Serialize(*lib2);
	test2Lib = DeserializeLibrary(buf2->Data(), buf2->Count());
	delete product2;
	delete buf2;*/
	test2Lib = nullptr;

	TestCodeLoader loader(L".\\RainScripts\\");
	//TestCodeLoader loader(L"E:\\Projects\\Unity\\RLDemo\\Assets\\Scripts\\Logic\\RainScripts\\");
	BuildParameter parameter(RainString::Create(L"TestLib"), false, &loader, OnLibraryLoader, ErrorLevel::WarringLevel4);
	RainProduct* product = Build(parameter);
	for (uint32 i = 0; i <= 8; i++)
	{
		ErrorLevel el = (ErrorLevel)i;
		uint32 c = product->GetLevelMessageCount(el);
		if (c) cout << "ERR_LVL:" << i << "\n";
		for (uint32 j = 0; j < c; j++)
		{
			RainErrorMessage msg = product->GetErrorMessage(el, j);
			wstring message;
			message.assign(msg.message.value, msg.message.length);
			wcout << message << "\n";
			wstring source;
			source.assign(msg.path.value, msg.path.length);
			wcout << source << " line:" << msg.line << "\tERR CODE:" << (uint32)msg.type << "\n";
		}
	}
	if (!product->GetLevelMessageCount(ErrorLevel::Error))
	{
		const RainLibrary* library = product->GetLibrary();
		StartupParameter parameter(&library, 1, 0, 0x10, 0xf, nullptr, nullptr, OnLibraryLoader, NativeLoader, 0xff, 8, 8, 0xff, OnExce, nullptr);
		RainKernel* kernel = CreateKernel(parameter);
		RainFunction rf = kernel->FindFunction(L"Main", true);
		if (rf.IsValid())
		{
			InvokerWrapper iw = rf.CreateInvoker();
			iw.Start(true, false);
			while (kernel->GetState().taskCount)
			{
				kernel->Update();
				this_thread::sleep_for(chrono::milliseconds(300));
			}
		}
		else
		{
			wcout << "无效的RainFunction\n";
		}
		delete kernel;
	}
	delete product;
	delete test2Lib;

}

int main()
{
	SetMemoryAllocator(ALLOC, FREE, REALLOC);
	wcout.imbue(locale("zh_CN.UTF-8"));
	TestFunc();

	ClearStaticCache();

	cout << "\n\n\n申请的内存总数：" << midx << "\n";
	cout << "未释放的内存索引列表:\n";
	for (auto it = mmap.begin(); it != mmap.end(); it++)
		if (it->second >= 0)
			cout << it->second << "\n";
}
