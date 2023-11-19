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
	std::wstring str;
	str.assign(value.value, value.length);
	std::wcout << str;// << L"\n";
}

void Clock(RainKernel&, CallerWrapper& caller)
{
	caller.SetReturnValue(0, (integer)clock());
}

void OpenFile(RainKernel&, CallerWrapper& caller)
{
	const RainString value = caller.GetStringParameter(0);
	std::wstring path;
	path.assign(value.value, value.length);
	std::wfstream* file = new std::wfstream(path);
	caller.SetEntityReturnValue(0, (uint64)file);
}
void FileReadLine(RainKernel&, CallerWrapper& caller)
{
	std::wfstream* file = (std::wfstream*)caller.GetEntityParameter(0);
	std::wstring line;
	bool end = !(bool)std::getline(*file, line);
	caller.SetReturnValue(0, end);
	caller.SetReturnValue(1, line.c_str());
}
void CloseFile(RainKernel&, CallerWrapper& caller)
{
	std::wfstream* file = (std::wfstream*)caller.GetEntityParameter(0);
	file->close();
	delete file;
}


OnCaller NativeLoader(RainKernel&, const RainString fullName, const RainType* parameters, uint32 parametersCount)
{
	std::wstring str; str.assign(fullName.value, fullName.length);
	//std::wcout << "\n\nNative Load:" << str << "\n\n";
	if (str == L"TestLib.Print") return Print;
	else if (str == L"TestLib.Clock") return Clock;
	else if (str == L"TestLib.OpenFile") return OpenFile;
	else if (str == L"TestLib.FileReadLine") return FileReadLine;
	else if (str == L"TestLib.CloseFile") return CloseFile;
	return nullptr;
}

std::map<long long, int> mmap;
int midx = 0;
void* ALLOC(uint32 size)
{
	void* result = malloc((size_t)size);
	midx++;
	mmap[(long long)result] = midx;
	//std::cout << "A" << midx << std::endl;
	return result;
}
void* REALLOC(void* pointer, uint32 size)
{
	//std::cout << "R" << -mmap[(long long)pointer] << std::endl;
	mmap[(long long)pointer] = -1;
	void* result = realloc(pointer, (size_t)size);
	midx++;
	mmap[(long long)result] = midx;
	//std::cout << "R" << midx << std::endl;
	return result;
}
void FREE(void* pointer)
{
	//std::cout << "F" << -mmap[(long long)pointer] << std::endl;
	mmap[(long long)pointer] = -1;
	free(pointer);
}

const RainLibrary* OnLibraryLoader(const RainString& libName)
{
	return nullptr;
}
void OnExce(RainKernel&, const RainStackFrame* stackFrames, uint32 stackFrameCount, const RainString message)
{
	std::wstring str;
	str.assign(message.value, message.length);
	std::wcout << L"异常信息:" << str << "\n";
	std::wcout << L"堆栈:\n";
	for (uint32 i = 0; i < stackFrameCount; i++)
	{
		str.assign(stackFrames[i].libraryName.value, stackFrames[i].libraryName.length);
		std::wcout << str << ":";
		str.assign(stackFrames[i].functionName.value, stackFrames[i].functionName.length);
		std::wcout << str << " [" << stackFrames[i].address << "]\n";
	}
}

void TestFunc()
{

	TestCodeLoader loader(L"E:\\Projects\\CPP\\RainLanguage\\Test\\TestScripts\\");
	//TestCodeLoader loader(L"E:\\Projects\\Unity\\RLDemo\\Assets\\Scripts\\Logic\\RainScripts\\");
	BuildParameter parameter(RainString::Create(L"TestLib"), false, &loader, OnLibraryLoader, ErrorLevel::WarringLevel4);
	RainProduct* product = Build(parameter);
	for (uint32 i = 0; i <= 8; i++)
	{
		ErrorLevel el = (ErrorLevel)i;
		uint32 c = product->GetLevelMessageCount(el);
		if (c) std::cout << "ERR_LVL:" << i << "\n";
		for (uint32 j = 0; j < c; j++)
		{
			RainErrorMessage msg = product->GetErrorMessage(el, j);
			std::wstring message;
			message.assign(msg.message.value, msg.message.length);
			std::wcout << message << "\n";
			std::wstring source;
			source.assign(msg.path.value, msg.path.length);
			std::wcout << source << " line:" << msg.line << "\tERR CODE:" << (uint32)msg.type << "\n";
		}
	}
	if (!product->GetLevelMessageCount(ErrorLevel::Error))
	{
		const RainLibrary* library = product->GetLibrary();
		StartupParameter parameter(&library, 1, 0, 0x10, 0xf, nullptr, nullptr, nullptr, NativeLoader, 0xff, 8, 8, 0xff, OnExce, nullptr);
		RainKernel* kernel = CreateKernel(parameter);
		RainFunction rf = kernel->FindFunction(L"Main", true);
		InvokerWrapper iw = rf.CreateInvoker();
		iw.Start(true, false);
		while (kernel->GetState().taskCount)
		{
			kernel->Update();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		delete kernel;
	}
	delete product;

}

int main()
{
	SetMemoryAllocator(ALLOC, FREE, REALLOC);
	std::wcout.imbue(std::locale("zh_CN"));

	TestFunc();

	ClearStaticCache();

	std::cout << "\n\n\n申请的内存总数：" << midx << "\n";
	std::cout << "未释放的内存索引列表:\n";
	for (auto it = mmap.begin(); it != mmap.end(); it++)
		if (it->second >= 0)
			std::cout << it->second << "\n";
}
