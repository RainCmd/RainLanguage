#include "ProgramDatabase.h"
#include "Serialization.h"

DebugFile::~DebugFile()
{
	Dictionary<uint32, List<uint32, true>*, true>::Iterator iterator = statements.GetIterator();
	while (iterator.Next()) delete iterator.CurrentValue();
	statements.Clear();
}

const uint32* ProgramDatabase::GetStatements(const RainString& file, uint32 line, uint32& count) const
{
	String fileName = agency->Add(file.value, file.length);
	DebugFile* debugFile; List<uint32, true>* fileStatements;
	if (files.TryGet(fileName, debugFile) && debugFile->statements.TryGet(line, fileStatements))
	{
		count = fileStatements->Count();
		return fileStatements->GetPointer();
	}
	return nullptr;
}

uint32 ProgramDatabase::GetStatement(uint32 instructAddress) const
{
	uint32 start = 0, end = statements.Count(), result = INVALID;
	while (start < end)
	{
		uint32 middle = (start + end) >> 1;
		if (instructAddress < statements[middle].pointer) end = middle;
		else result = start = middle;
	}
	return result;
}

ProgramDatabase::~ProgramDatabase()
{
	Dictionary<String, DebugFile*>::Iterator iterator = files.GetIterator();
	while (iterator.Next()) delete iterator.CurrentValue();
	files.Clear();
	delete agency; agency = NULL;

}

RAINLANGUAGE const RainBuffer<uint8>* Serialize(const RainProgramDatabase* database) //这里直接序列化String.index会导致反序列化时Agency中的字符串引用计数翻倍，但是不影响正常使用和内存释放
{
	ASSERT(database, "database为空");
	ProgramDatabase* source = (ProgramDatabase*)database;
	Serializer* serializer = new Serializer(0x100);
	serializer->SerializeStringAgency(source->agency);
	serializer->Serialize(source->name.index);
	serializer->Serialize(source->functions.Count());
	for (uint32 x = 0; x < source->functions.Count(); x++)
	{
		DebugFunction& function = source->functions[x];
		serializer->Serialize(function.file.index);
		serializer->Serialize(function.entry);
		serializer->Serialize(function.locals.Count());
		for (uint32 y = 0; y < function.locals.Count(); y++)
		{
			DebugLocal& local = function.locals[y];
			serializer->Serialize(local.name.index);
			serializer->Serialize(local.address);
			serializer->Serialize(local.type);
		}
		serializer->Serialize(function.localAnchors.Count());
		Dictionary<DebugAnchor, uint32, true>::Iterator localIterator = function.localAnchors.GetIterator();
		while (localIterator.Next())
		{
			serializer->Serialize(localIterator.CurrentKey());
			serializer->Serialize(localIterator.CurrentValue());
		}
	}
	serializer->SerializeList(source->statements);
	serializer->Serialize(source->files.Count());
	Dictionary<String, DebugFile*>::Iterator fileIterator = source->files.GetIterator();
	while (fileIterator.Next())
	{
		serializer->Serialize(fileIterator.CurrentKey().index);
		DebugFile* file = fileIterator.CurrentValue();
		serializer->SerializeList(file->functions);
		serializer->Serialize(file->globalAnchors.Count());
		Dictionary<DebugAnchor, DebugGlobal, true>::Iterator globalIterator = file->globalAnchors.GetIterator();
		while (globalIterator.Next())
		{
			serializer->Serialize(globalIterator.CurrentKey());
			serializer->Serialize(globalIterator.CurrentValue());
		}
		Dictionary<uint32, List<uint32, true>*, true>::Iterator statementIterator = file->statements.GetIterator();
		while (statementIterator.Next())
		{
			serializer->Serialize(statementIterator.CurrentKey());
			serializer->SerializeList(*statementIterator.CurrentValue());
		}
	}
	return serializer;
}

RAINLANGUAGE const RainProgramDatabase* DeserializeDataBase(const uint8* data, uint32 size)
{
	Deserializer deserializer(data, size);
	StringAgency* agency = deserializer.DeserializeStringAgency();
	ProgramDatabase* result = new ProgramDatabase(agency);
	result->name = agency->Get(deserializer.Deserialize<string>());
	uint32 functionCount = deserializer.Deserialize<uint32>();
	while (functionCount--)
	{
		DebugFunction* function = new (result->functions.Add())DebugFunction();
		function->file = agency->Get(deserializer.Deserialize<string>());
		function->entry = deserializer.Deserialize<uint32>();
		uint32 localCount = deserializer.Deserialize<uint32>();
		while (localCount--)
		{
			DebugLocal* local = new (function->locals.Add())DebugLocal();
			local->name = agency->Get(deserializer.Deserialize<string>());
			local->address = deserializer.Deserialize<uint32>();
			local->type = deserializer.Deserialize<Type>();
		}
		uint32 statementCount = deserializer.Deserialize<uint32>();
		while (statementCount--)
		{
			DebugAnchor anchor = deserializer.Deserialize<DebugAnchor>();
			function->localAnchors.Set(anchor, deserializer.Deserialize<uint32>());
		}
	}
	deserializer.Deserialize(result->statements);
	uint32 fileCount = deserializer.Deserialize<uint32>();
	while (fileCount--)
	{
		String fileName = agency->Get(deserializer.Deserialize<string>());
		DebugFile* file = new DebugFile();
		result->files.Set(fileName, file);
		deserializer.Deserialize(file->functions);
		uint32 globalCount = deserializer.Deserialize<uint32>();
		while (globalCount--)
		{
			DebugAnchor anchor = deserializer.Deserialize<DebugAnchor>();
			file->globalAnchors.Set(anchor, deserializer.Deserialize<DebugGlobal>());
		}
		uint32 statementCount = deserializer.Deserialize<uint32>();
		while (statementCount--)
		{
			uint32 line = deserializer.Deserialize<uint32>();
			List<uint32, true>* statements = new List<uint32, true>(0);
			deserializer.Deserialize(*statements);
			file->statements.Set(line, statements);
		}
	}
	return result;
}
