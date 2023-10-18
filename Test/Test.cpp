// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <io.h>
#include <Builder.h>
#include <VirtualMachine.h>
#include <MemoryAllocator.h>
#include <map>
#include <thread>

class TestCodeLoader :public CodeLoader
{
	std::wstring dir;
	std::wstring path;
	std::wstring content;
	char* buffer;
	uint32 count, size;
	intptr_t hFind;
	_wfinddata_t data;
	bool first;
	void Read()
	{
		path.assign(dir);
		path.append(data.name);
		if (size < data.size)
		{
			size = data.size;
			buffer = buffer ? (char*)realloc(buffer, size) : (char*)malloc(size);
		}
		FILE* file = 0;
		_wfopen_s(&file, path.c_str(), L"r");
		if (file)
		{
			count = fread(buffer, 1, data.size, file);
			buffer[count] = '\0';
			std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
			content = cvt.from_bytes(buffer);
			fclose(file);
		}
	}
public:
	TestCodeLoader(const character* path)
	{
		dir.assign(path);
		//std::wcout.imbue(std::locale("", LC_CTYPE));
		std::wstring p; p.assign(dir); p.append(L"\\*.rain");
		this->path = std::wstring();
		buffer = nullptr;
		count = size = 0;
		hFind = _wfindfirst(p.c_str(), &data);
		first = true;
	}
	bool LoadNext()
	{
		if (first)
		{
			if (hFind != -1)
			{
				Read();
				first = false;
				return true;
			}
		}
		else if (!_wfindnext(hFind, &data))
		{
			Read();
			return true;
		}
		return false;
	}
	const RainString CurrentPath()
	{
		return RainString(path.c_str(), path.length());
	}
	const RainString CurrentContent()
	{
		return RainString(content.c_str(), content.length());
	}
	~TestCodeLoader()
	{
		_findclose(hFind);
		if (buffer != nullptr)
		{
			free(buffer);
			buffer = nullptr;
		}
	}
};

void Print(RainKernel*, const CallerWrapper* caller)
{
	const RainString value = caller->GetStringParameter(0);
	std::wstring str;
	str.assign(value.value, value.length);
	std::wcout << str << L"\n";
}

OnCaller NativeLoader(RainKernel*, const RainString fullName, const RainType* parameters, uint32 parametersCount)
{
	std::wstring str; str.assign(fullName.value, fullName.length);
	std::wcout << "Native Load:" << str << "\n";
	return Print;
}

std::map<long long, uint32> mmap;
uint32 midx = 0;
void* ALLOC(uint32 size)
{
	void* result = malloc((size_t)size);
	midx++;
	mmap[(long long)result] = midx;
	return result;
}
void* REALLOC(void* pointer, uint32 size)
{
	mmap[(long long)pointer] = -1;
	void* result = realloc(pointer, (size_t)size);
	midx++;
	mmap[(long long)result] = midx;
	return result;
}
void FREE(void* pointer)
{
	mmap[(long long)pointer] = -1;
	free(pointer);
}

void OnExce(RainKernel*, const RainStackFrame* stackFrames, uint32 stackFrameCount, const RainString message)
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

	TestCodeLoader loader(L"D:\\Projects\\CPP\\RainLanguage\\Test\\RainScripts\\");
	BuildParameter parameter(RainString::Create(L"TestLib"), false, &loader, nullptr, ErrorLevel::WarringLevel4);
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
		StartupParameter parameter(product->GetLibrary(), 1, 0, 0x10, 0xf, nullptr, nullptr, nullptr, NativeLoader, 0xff, 8, 8, 0xff, OnExce, nullptr);
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
	std::wcout.imbue(std::locale(""));

	TestFunc();

	ClearStaticCache();

	std::cout << "申请的内存总数：" << midx << "\n";
	std::cout << "未释放的内存索引列表:\n";
	for (auto it = mmap.begin(); it != mmap.end(); it++)
		if (it->second < 0)
			std::cout << it->second << "\n";
}
