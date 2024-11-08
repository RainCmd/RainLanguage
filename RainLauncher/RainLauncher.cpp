#include "Args.h"
#include "Builder.h"
#include "VirtualMachine.h"
#include "Debugger.h"

#include <iostream>
#include <string>
#include <io.h>
#include <fstream>
#include <vector>
#include <thread>
#include <codecvt>

static void Save(RainBuffer<uint8>* buffer, wstring path)
{
	fstream file(path, ios::out | ios::binary);
	file.write((char*)buffer->Data(), buffer->Count());
	file.close();
}

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
		auto handle = _wfindfirst((dir + L"*").c_str(), &data);
		if(handle != -1)
		{
			do
			{
				if(EndWidth(data.name, L".") || EndWidth(data.name, L"..")) continue;
				wstring path = dir + data.name;
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
		for(uint32 i = 0; i < rpath.length(); i++)
			if(rpath[i] == '\\')
				rpath[i] = '/';
		return RainString(rpath.c_str(), (uint32)rpath.length());
	}
	const RainString CurrentContent()
	{
		content.clear();
		ifstream file(path, ios::binary);
		if(file.is_open())
		{
			string buffer = string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
			file.close();

			if(buffer.substr(0, 3) == "\xEF\xBB\xBF")//utf-8-sig
			{
				buffer.erase(0, 3);
				wstring_convert<codecvt_utf8<wchar_t>> converter;
				content = converter.from_bytes(buffer);
			}
			else if(buffer.substr(0, 2) == "\xFF\xFE")//utf-16-le
			{
				buffer.erase(0, 2);
				for(size_t i = 0, size = buffer.size() / 2; i < size; i++)
				{
					uint16& value = *(uint16*)&buffer[i << 1];
					value = (value << 8) | (value >> 8);
				}
				wstring_convert<codecvt_utf16<wchar_t>> converter;
				content = converter.from_bytes(buffer);
			}
			else if(buffer.substr(0, 2) == "\xFE\xFF")//utf-16-be
			{
				buffer.erase(0, 2);
				wstring_convert<codecvt_utf16<wchar_t>> converter;
				content = converter.from_bytes(buffer);
			}
			else//默认当做utf8处理
			{
				wstring_convert<codecvt_utf8<wchar_t>> converter;
				content = converter.from_bytes(buffer);
			}
		}
		else wcout << "File opening failure: " << path << endl;
		return RainString(content.c_str(), (uint32)content.length());
	}
};

static void Print(RainKernel&, CallerWrapper& caller)
{
	const character* value = caller.GetStringParameter(0).value;
	if(value)
	{
		wcout << value;
		wcout.flush();
	}
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

	wcout << "native function binding failure: " << fullName.value << endl;
	return NativeHelper;
}

static RainProduct* product;
static Args args;
static void OnExceptionExitFunc(RainKernel&, const RainStackFrame* frames, uint32 frameCount, const RainString msg)
{
	RainProgramDatabase* pdb = product->GetRainProgramDatabase();
	wcout << L"exception:" << (msg.value ? msg.value : L"") << L"\n";
	for(size_t i = 0; i < frameCount; i++)
	{
		wstring libName = wstring(frames[i].libraryName.value, frames[i].libraryName.length);
		if(args.name == libName)
		{
			uint32 line;
			RainString file = pdb->GetPosition(frames[i].address, line);
			wcout << file.value << L"\t" << frames[i].functionName.value << L"\tline: " << line << endl;
		}
		else
		{
			wcout << L"file not found\t" << frames[i].functionName.value << L"\taddress: " << frames[i].address << endl;
		}
	}
}

static wstring NormalPath(wstring path)
{
	if(path[0] == L'.') path = args.path + L"/" + path;
	wchar_t last = path[path.size() - 1];
	if(last != '/' && last != '\\') path += '/';
	return path;
}
static vector<uint8>* GetData(wstring path, const wstring& name)
{
	path = NormalPath(path) + name;

	ifstream file(path, ios::binary);
	vector<uint8>* result = nullptr;
	if(file.good() && file.is_open())
	{
		streamsize size = file.tellg();
		file.seekg(0, ios::beg);

		result = new vector<uint8>(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
		file.close();
	}
	return result;
}
static vector<uint8>* GetData(wstring name)
{
	vector<uint8>* result = nullptr;
	for(uint32 i = 0; i < args.referencePath.size(); i++)
	{
		result = GetData(args.referencePath[i], name);
		if(result) return result;
	}
	return nullptr;
}
static bool IsName(const RainString& name)
{
	if(name.length != args.name.size()) return false;
	for(uint32 i = 0; i < name.length; i++)
		if(name.value[i] != args.name.c_str()[i])return false;
	return true;
}
static RainLibrary* LibraryLoader(const RainString& name)
{
	if(IsName(name)) return product->GetLibrary();
	vector<uint8>* data = GetData(wstring(name.value, name.length).append(L".rdll"));
	if(data)
	{
		RainLibrary* result = DeserializeLibrary(data->data(), data->size());
		delete data; data = nullptr;
		return result;
	}
	return nullptr;
}
static void LibraryUnloader(RainLibrary* library)
{
	Delete(library);
}
static RainProgramDatabase* ProgramDatabaseLoader(const RainString& name)
{
	if(IsName(name)) return product->GetRainProgramDatabase();
	vector<uint8>* data = GetData(wstring(name.value, name.length).append(L".rpdb"));
	if(data)
	{
		RainProgramDatabase* result = DeserializeDatabase(data->data(), data->size());
		delete data; data = nullptr;
		return result;
	}
	return nullptr;
}
static void ProgramDatabaseUnloader(RainProgramDatabase* database)
{
	Delete(database);
}
int main(int cnt, char** _args)
{
	wcout.imbue(locale("zh_CN"));
	args = Parse(cnt, _args);

	CodeLoadHelper helper(args.path);
	BuildParameter parameter(RainString(args.name.c_str(), (uint32)args.name.size()), args.debug, &helper, LibraryLoader, LibraryUnloader, (ErrorLevel)args.errorLevel);
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
		if(args.out.size())
		{
			args.out = NormalPath(args.out);
			if(!_wmkdir(args.out.c_str()) || errno == EEXIST)
			{
				RainBuffer<uint8>* buffer = Serialize(*product->GetLibrary());
				Save(buffer, args.out + L"\\" + args.name + L".rdll");
				Delete(buffer);
				buffer = Serialize(*product->GetRainProgramDatabase());
				Save(buffer, args.out + L"\\" + args.name + L".rpdb");
				Delete(buffer);
			}
		}
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
		StartupParameter parameter(library, nullptr, nullptr, LibraryLoader, LibraryUnloader, NativeLoader, OnExceptionExitFunc);
		RainKernel* kernel = CreateKernel(parameter, ProgramDatabaseLoader, ProgramDatabaseUnloader);
		RainFunction entry = kernel->FindFunction(args.entry.c_str(), true);
		if(entry.IsValid())
		{
			InvokerWrapper invoker = entry.CreateInvoker();
			invoker.Start(true, false);
			while(kernel->GetState().taskCount)
			{
				if(args.timestep > 0) this_thread::sleep_for(chrono::milliseconds(args.timestep));
				kernel->Update();
			}
		}
		else wcout << "entry:" << args.entry << " not found" << endl;
		Delete(kernel);
	}
	delete product;
	return 0;
}