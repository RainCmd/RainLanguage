#include "ProgramDatabase.h"
#include "Serialization.h"

ProgramDatabase::ProgramDatabase(const String& name) : agency(new StringAgency(0xFF)), functions(0), statements(0), files(0)
{
	this->name = agency->Add(name);
}

const uint32 ProgramDatabase::GetStatement(const RainString& file, uint32 line) const
{
	String fileName = agency->Add(file.value, file.length);
	DebugFile* debugFile; uint32 result = 0;
	if(files.TryGet(fileName, debugFile) && debugFile->statements.TryGet(line, result)) return result;
	return INVALID;
}

uint32 ProgramDatabase::GetStatement(uint32 instructAddress) const
{
	if(!statements.Count() || instructAddress < statements[0].pointer) return INVALID;
	uint32 start = 0, end = statements.Count(), result = 0;
	while(start + 1 < end)
	{
		uint32 middle = (start + end) >> 1;
		if(instructAddress <= statements[middle].pointer) end = middle;
		else result = start = middle;
	}
	return result;
}

const RainString ProgramDatabase::GetPosition(uint32 instructAddress, uint32& line) const
{
	const DebugStatement& statement = statements[GetStatement(instructAddress)];
	line = statement.line;
	const DebugFunction* function = functions[statement.function];
	return RainString(function->file.GetPointer(), function->file.GetLength());
}

ProgramDatabase::~ProgramDatabase()
{
	for(uint32 i = 0; i < functions.Count(); i++) delete functions[i];
	Dictionary<String, DebugFile*>::Iterator iterator = files.GetIterator();
	while(iterator.Next()) delete iterator.CurrentValue();
	files.Clear();
	delete agency; agency = NULL;

}

void SerializeMembers(Serializer* serializer, List<DebugMemberIndex>& members)
{
	serializer->Serialize(members.Count());
	for(uint32 i = 0; i < members.Count(); i++)
	{
		serializer->Serialize(members[i].declaration);
		serializer->Serialize(members[i].member.index);
		serializer->Serialize(members[i].index);
	}
}

RAINLANGUAGE RainBuffer<uint8>* Serialize(const RainProgramDatabase& database) //这里直接序列化String.index会导致反序列化时Agency中的字符串引用计数翻倍，但是不影响正常使用和内存释放
{
	ProgramDatabase* source = (ProgramDatabase*)&database;
	Serializer* serializer = new Serializer(0x100);
	serializer->SerializeStringAgency(source->agency);
	serializer->Serialize(source->name.index);
	serializer->Serialize(source->functions.Count());
	for(uint32 x = 0; x < source->functions.Count(); x++)
	{
		DebugFunction* function = source->functions[x];
		serializer->Serialize(function->file.index);
		serializer->Serialize(function->locals.Count());
		for(uint32 y = 0; y < function->locals.Count(); y++)
		{
			DebugLocal& local = function->locals[y];
			serializer->Serialize(local.name.index);
			serializer->Serialize(local.address);
			serializer->Serialize(local.type);
			serializer->Serialize(local.start);
			serializer->Serialize(local.end);
		}
		serializer->Serialize(function->localAnchors.Count());
		Dictionary<DebugAnchor, uint32, true>::Iterator localIterator = function->localAnchors.GetIterator();
		while(localIterator.Next())
		{
			serializer->Serialize(localIterator.CurrentKey());
			serializer->Serialize(localIterator.CurrentValue());
		}
		serializer->Serialize(function->members.Count());
		Dictionary<DebugAnchor, DebugMember>::Iterator memberIterator = function->members.GetIterator();
		while(memberIterator.Next())
		{
			serializer->Serialize(memberIterator.CurrentKey());
			serializer->Serialize(memberIterator.CurrentValue().local);
			SerializeMembers(serializer, memberIterator.CurrentValue().members);
		}
	}
	serializer->SerializeList(source->statements);
	serializer->Serialize(source->files.Count());
	Dictionary<String, DebugFile*>::Iterator fileIterator = source->files.GetIterator();
	while(fileIterator.Next())
	{
		serializer->Serialize(fileIterator.CurrentKey().index);
		DebugFile* file = fileIterator.CurrentValue();
		serializer->SerializeList(file->functions);
		serializer->Serialize(file->globalAnchors.Count());
		Dictionary<DebugAnchor, DebugGlobal>::Iterator globalIterator = file->globalAnchors.GetIterator();
		while(globalIterator.Next())
		{
			serializer->Serialize(globalIterator.CurrentKey());
			serializer->Serialize(globalIterator.CurrentValue().library);
			serializer->Serialize(globalIterator.CurrentValue().index);
			SerializeMembers(serializer, globalIterator.CurrentValue().members);
		}
		serializer->Serialize(file->statements.Count());
		Dictionary<uint32, uint32, true>::Iterator statementIterator = file->statements.GetIterator();
		while(statementIterator.Next())
		{
			serializer->Serialize(statementIterator.CurrentKey());
			serializer->Serialize(statementIterator.CurrentValue());
		}
	}
	return serializer;
}

void DeserializeMembers(Deserializer& deserializer, List<DebugMemberIndex>& members, StringAgency* agency)
{
	uint32 count = deserializer.Deserialize<uint32>();
	while(count--)
	{
		Declaration declaration = deserializer.Deserialize<Declaration>();
		String member = agency->Get(deserializer.Deserialize<string>());
		DebugAnchor index = deserializer.Deserialize<DebugAnchor>();
		new (members.Add())DebugMemberIndex(declaration, member, index);
	}
}

RAINLANGUAGE RainProgramDatabase* DeserializeDatabase(const uint8* data, uint32 size)
{
	Deserializer deserializer(data, size);
	StringAgency* agency = deserializer.DeserializeStringAgency();
	ProgramDatabase* result = new ProgramDatabase(agency);
	result->name = agency->Get(deserializer.Deserialize<string>());
	uint32 functionCount = deserializer.Deserialize<uint32>();
	while(functionCount--)
	{
		DebugFunction* function = new DebugFunction();
		function->file = agency->Get(deserializer.Deserialize<string>());
		uint32 localCount = deserializer.Deserialize<uint32>();
		while(localCount--)
		{
			DebugLocal* local = new (function->locals.Add())DebugLocal();
			local->name = agency->Get(deserializer.Deserialize<string>());
			local->address = deserializer.Deserialize<uint32>();
			local->type = deserializer.Deserialize<Type>();
			local->start = deserializer.Deserialize<uint32>();
			local->end = deserializer.Deserialize<uint32>();
		}
		uint32 localAnchorCount = deserializer.Deserialize<uint32>();
		while(localAnchorCount--)
		{
			DebugAnchor anchor = deserializer.Deserialize<DebugAnchor>();
			function->localAnchors.Set(anchor, deserializer.Deserialize<uint32>());
		}
		uint32 memberCount = deserializer.Deserialize<uint32>();
		while(memberCount--)
		{
			DebugAnchor anchor = deserializer.Deserialize<DebugAnchor>();
			DebugMember member;
			member.local = deserializer.Deserialize<uint32>();
			DeserializeMembers(deserializer, member.members, agency);
			function->members.Set(anchor, member);
		}
		result->functions.Add(function);
	}
	deserializer.Deserialize(result->statements);
	uint32 fileCount = deserializer.Deserialize<uint32>();
	while(fileCount--)
	{
		String fileName = agency->Get(deserializer.Deserialize<string>());
		DebugFile* file = new DebugFile();
		result->files.Set(fileName, file);
		deserializer.Deserialize(file->functions);
		uint32 globalCount = deserializer.Deserialize<uint32>();
		while(globalCount--)
		{
			DebugAnchor anchor = deserializer.Deserialize<DebugAnchor>();
			DebugGlobal global;
			global.library = deserializer.Deserialize<uint32>();
			global.index = deserializer.Deserialize<uint32>();
			DeserializeMembers(deserializer, global.members, agency);
			file->globalAnchors.Set(anchor, global);
		}
		uint32 statementCount = deserializer.Deserialize<uint32>();
		while(statementCount--)
		{
			uint32 line = deserializer.Deserialize<uint32>();
			uint32 statement = deserializer.Deserialize<uint32>();
			file->statements.Set(line, statement);
		}
	}
	return result;
}

void Delete(RainProgramDatabase*& database)
{
	delete database; database = NULL;
}