#include "../RainLanguage.h"
#include "../KernelDeclarations.h"
#include "../Library.h"
#include "../Public/Builder.h"
#include "LineReader.h"
#include "Message.h"
#include "FileSpace.h"
#include "DeclarationManager.h"
#include "AbstractLibrary.h"
#include "DeclarationValidityCheck.h"
#include "LogicGenerator/Generator.h"

void CalculatedTupleInfo(DeclarationManager* manager, TupleInfo& tuple)
{
	uint8 alignment = 0;
	for (uint32 i = 0; i < tuple.Count(); i++)
	{
		uint32 size = manager->GetStackSize(tuple.GetType(i), alignment);
		MemoryAlignment(tuple.size, tuple.GetOffset(i), alignment, size);
	}
}
void CalculatedMemberFunctionParameterTupleInfo(DeclarationManager* manager, TupleInfo& tuple)
{
	uint8 alignment = 0;
	uint32 size = manager->GetStackSize(tuple.GetType(0), alignment);
	tuple.size += MemoryAlignment(size, MEMORY_ALIGNMENT_MAX);
	for (uint32 i = 1; i < tuple.Count(); i++)
	{
		size = manager->GetStackSize(tuple.GetType(i), alignment);
		MemoryAlignment(tuple.size, tuple.GetOffset(i), alignment, size);
	}
}
void CalculatedTupleInfo(DeclarationManager* manager)
{
	AbstractLibrary* library = manager->selfLibaray;
	List<AbstractStruct*, true> structs = List<AbstractStruct*, true>(4);
	for (uint32 x = 0; x < library->structs.Count(); x++)
	{
		uint32 size = 0; uint8 alignment = 0;
		AbstractStruct* abstractStruct = &library->structs[x];
		for (uint32 y = 0; y < abstractStruct->variables.Count(); y++)
		{
			AbstractVariable* member = &abstractStruct->variables[y];
			if (member->type.library == LIBRARY_SELF && member->type.code == TypeCode::Struct && !member->type.dimension && library->structs[member->type.index].size == INVALID)
				goto label_next_struct;
			else
			{
				uint8 memberAlignment;
				uint32 memberSize = manager->GetStackSize(member->type, memberAlignment);
				MemoryAlignment(size, member->address, memberAlignment, memberSize);
				if (memberAlignment > alignment)alignment = memberAlignment;
			}
		}
		abstractStruct->size = size;
		abstractStruct->alignment = alignment;
	label_next_struct:;
	}
	while (structs.Count())
	{
		uint32 count = structs.Count();
		for (uint32 x = 0; x < structs.Count(); x++)
		{
			uint32 size = 0; uint8 alignment = 0;
			AbstractStruct* abstractStruct = structs[x];
			for (uint32 y = 0; y < abstractStruct->variables.Count(); y++)
			{
				AbstractVariable* member = &abstractStruct->variables[y];
				if (member->type.library == LIBRARY_SELF && member->type.code == TypeCode::Struct && !member->type.dimension && library->structs[member->type.index].size == INVALID) goto label_next_struct2;
				else
				{
					uint8 memberAlignment;
					uint32 memberSize = manager->GetStackSize(member->type, memberAlignment);
					MemoryAlignment(size, member->address, memberAlignment, memberSize);
					if (memberAlignment > alignment)alignment = memberAlignment;
				}
			}
			abstractStruct->size = size;
			abstractStruct->alignment = alignment;
			structs.RemoveAtSwap(x--);
		label_next_struct2:;
		}
		ASSERT(structs.Count() < count, "应该是结构体类型循环包含了，前面的检查算法可能有误");
	}
	for (uint32 x = 0; x < library->classes.Count(); x++)
	{
		uint32 size = 0; uint8 alignment = 0;
		AbstractClass* abstractClass = &library->classes[x];
		for (uint32 y = 0; y < abstractClass->variables.Count(); y++)
		{
			AbstractVariable* member = &abstractClass->variables[y];
			uint8 memberAlignment;
			uint32 memberSize = manager->GetStackSize(member->type, memberAlignment);
			MemoryAlignment(size, member->address, memberAlignment, memberSize);
			if (memberAlignment > alignment)alignment = memberAlignment;
		}
		abstractClass->size = size;
		abstractClass->alignment = alignment;
	}

	uint32 address = 0;
	for (uint32 i = 0; i < library->variables.Count(); i++)
		if (library->variables[i].readonly)
		{
			uint8 memberAlignment;
			uint32 memberSize = manager->GetStackSize(library->variables[i].type, memberAlignment);
			MemoryAlignment(address, library->variables[i].address, memberAlignment, memberSize);
		}
	manager->compilingLibrary.constantSize = address;
	for (uint32 i = 0; i < library->variables.Count(); i++)
		if (!library->variables[i].readonly)
		{
			uint8 memberAlignment;
			uint32 memberSize = manager->GetStackSize(library->variables[i].type, memberAlignment);
			MemoryAlignment(address, library->variables[i].address, memberAlignment, memberSize);
		}
	manager->compilingLibrary.dataSize = address;

	for (uint32 x = 0; x < library->functions.Count(); x++)
	{
		AbstractFunction* function = &library->functions[x];
		if (function->declaration.category == DeclarationCategory::StructFunction || function->declaration.category == DeclarationCategory::Constructor || function->declaration.category == DeclarationCategory::ClassFunction)
			CalculatedMemberFunctionParameterTupleInfo(manager, function->parameters);
		else CalculatedTupleInfo(manager, function->parameters);
		CalculatedTupleInfo(manager, function->returns);
	}
	for (uint32 x = 0; x < library->interfaces.Count(); x++)
	{
		for (uint32 y = 0; y < library->interfaces[x].functions.Count(); y++)
		{
			CalculatedMemberFunctionParameterTupleInfo(manager, library->interfaces[x].functions[y].parameters);
			CalculatedTupleInfo(manager, library->interfaces[x].functions[y].returns);
		}
	}
	for (uint32 x = 0; x < library->delegates.Count(); x++)
	{
		CalculatedTupleInfo(manager, library->delegates[x].parameters);
		CalculatedTupleInfo(manager, library->delegates[x].returns);
	}
	for (uint32 x = 0; x < library->coroutines.Count(); x++)
		CalculatedTupleInfo(manager, library->coroutines[x].returns);
	for (uint32 x = 0; x < library->natives.Count(); x++)
	{
		CalculatedTupleInfo(manager, library->natives[x].parameters);
		CalculatedTupleInfo(manager, library->natives[x].returns);
	}
}

const class Product :public RainProduct
{
public:
	const MessageCollector* messageCollector;
	const Library* library;
	const ProgramDebugDatabase* programDebugDatabase;
	inline Product(MessageCollector* messageCollector) :messageCollector(messageCollector), library(NULL), programDebugDatabase(NULL) {}
	inline Product(MessageCollector* messageCollector, Library* library, ProgramDebugDatabase* programDebugDatabase) : messageCollector(messageCollector), library(library), programDebugDatabase(programDebugDatabase) {}
	inline ErrorLevel GetLevel() { return messageCollector->GetLevel(); }
	inline uint32 GetLevelMessageCount(ErrorLevel level) { return messageCollector->GetMessages(level)->Count(); }
	inline const ErrorMessage GetErrorMessage(ErrorLevel level, uint32 index)
	{
		const Message& message = (*messageCollector->GetMessages(level))[index];
		return ErrorMessage(message.source.GetPointer(), message.source.length, message.type, message.line, message.start, message.length, message.message.GetPointer(), message.message.length);
	}
	inline const RainLibrary* GetLibrary() { return library; }
	inline const ProgramDebugDatabase* GetProgramDebugDatabase() { return programDebugDatabase; }
	inline ~Product()
	{
		delete messageCollector;
		delete library;
		delete programDebugDatabase;
	}
};

#define COMPILE_TERMINATION_CHECK if (messages->GetMessages(ErrorLevel::Error)->Count())return new Product(messages);
RainProduct* Build(const BuildParameter& parameter)
{
	StringAgency stringAgency = StringAgency(0x1000);
	MessageCollector* messages = new MessageCollector(parameter.messageLevel);

	DeclarationManager manager = DeclarationManager(parameter.libraryLoader, &stringAgency, messages, stringAgency.Add(parameter.name));
	List<List<AbstractSpace*, true>> relySpaceCollector(0);
	{
		List<FileSpace>fileSpaces = List<FileSpace>(0);
		while (parameter.codeLoader->LoadNext())
		{
			LineReader lineReader(&stringAgency, stringAgency.Add(parameter.codeLoader->CurrentPath(), parameter.codeLoader->CurrentPathLength()), parameter.codeLoader->CurrentCode(), parameter.codeLoader->CurrentCodeLength());
			ParseParameter parseParameter = ParseParameter(&lineReader, messages);
			new (fileSpaces.Add())FileSpace(&manager.compilingLibrary, INVALID, &parseParameter);
		}
		COMPILE_TERMINATION_CHECK;

		for (uint32 i = 0; i < fileSpaces.Count(); i++)
			fileSpaces[i].Tidy(&manager);
		COMPILE_TERMINATION_CHECK;

		manager.selfLibaray = new AbstractLibrary(&manager.compilingLibrary, AbstractParameter(&stringAgency, &manager, messages));

		for (uint32 i = 0; i < fileSpaces.Count(); i++)
			fileSpaces[i].Link(&manager, &relySpaceCollector);
		COMPILE_TERMINATION_CHECK;
	}
	DeclarationValidityCheck(&manager);
	COMPILE_TERMINATION_CHECK;

	ImplementsCheck(&manager);
	COMPILE_TERMINATION_CHECK;

	CalculatedTupleInfo(&manager);
	COMPILE_TERMINATION_CHECK;

	Generator generator = Generator(&manager);
	GeneratorParameter generatorParameter = GeneratorParameter(&manager, &generator, parameter.debug ? new ProgramDebugDatabase() : NULL);
	generator.GeneratorFunction(generatorParameter);
	relySpaceCollector.Clear();
	if (messages->GetMessages(ErrorLevel::Error)->Count())
	{
		if (generatorParameter.debugDatabase) delete generatorParameter.debugDatabase;
		return new Product(messages);
	}

	return new Product(messages, generator.GeneratorLibrary(manager), generatorParameter.debugDatabase);
}
