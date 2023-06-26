#pragma once
#include "../Collections/List.h"
#include "../RainLanguage.h"
#include "../String.h"
#include "Message.h"
#include "CompilingLibrary.h"
#include "Anchor.h"
#include "Visibility.h"

class LineReader;
struct ParseParameter
{
	LineReader* reader;
	MessageCollector* messages;
	inline ParseParameter(LineReader* reader, MessageCollector* messages) :reader(reader), messages(messages) {}
};

struct FileReader
{
	String source;
	LineReader* lineReader;
	inline FileReader(const String& source, LineReader* lineReader) :source(source), lineReader(lineReader) {}
};

struct FileType
{
	List<Anchor> name;
	uint32 dimension;
	inline FileType() :name(0), dimension(INVALID) {}
	inline FileType(const List<Anchor> name, uint32 dimension) : name(name), dimension(dimension) {}
	Type GetType(DeclarationManager* manager, const List<CompilingDeclaration, true>& declarations);
};

struct FileParameter
{
	Anchor name;
	FileType type;
	inline FileParameter(const Anchor& name, const FileType& type) :name(name), type(type) {}
};
struct FileSpace;
struct FileDeclaration
{
	Anchor name;
	Visibility visibility;
	FileSpace* space;
	List<Anchor> attributes;
	inline FileDeclaration(const Anchor& name, Visibility visibility, FileSpace* space) :name(name), visibility(visibility), space(space), attributes(0) {}
};
struct FileVariable :FileDeclaration
{
	CompilingVariable* compiling;
	bool readonly;
	FileType type;
	Anchor expression;
	inline FileVariable(const Anchor& name, Visibility visibility, FileSpace* space, bool readonly, const FileType& type, Anchor expression) :FileDeclaration(name, visibility, space), compiling(NULL), readonly(readonly), type(type), expression(expression) {}
};
struct FileFunction :FileDeclaration
{
	CompilingFunction* compiling;
	List<FileParameter> parameters;
	List<FileType> returns;
	List<Line> body;
	inline FileFunction(const Anchor& name, Visibility visibility, FileSpace* space, const List<FileParameter>& parameters, const List<FileType>& returns) :FileDeclaration(name, visibility, space), compiling(NULL), parameters(parameters), returns(returns), body(0) {}
};
struct FileEnum :FileDeclaration
{
	struct Element
	{
		CompilingEnum::Element* compiling;
		Anchor name;
		Anchor expression;
		inline Element(const Anchor& name, const Anchor& expression) :compiling(NULL), name(name), expression(expression) {}
	};
	CompilingEnum* compiling;
	List<Element> elements;
	inline FileEnum(const Anchor& name, Visibility visibility, FileSpace* space) :FileDeclaration(name, visibility, space), compiling(NULL), elements(0) {}
};
struct FileStruct :FileDeclaration
{
	struct Variable
	{
		CompilingStruct::Variable* compiling;
		Anchor name;
		FileType type;
		List<Anchor> attributes;
		inline Variable(const Anchor& name, const FileType& type) :compiling(NULL), name(name), type(type), attributes(0) {}
	};
	CompilingStruct* compiling;
	List<Variable> variables;
	List<FileFunction> functions;
	inline FileStruct(const Anchor& name, Visibility visibility, FileSpace* space) :FileDeclaration(name, visibility, space), compiling(NULL), variables(0), functions(0) {}
};
struct FileClass :FileDeclaration
{
	struct Variable
	{
		CompilingClass::Variable* compiling;
		Anchor name;
		Visibility visibility;
		FileType type;
		Anchor expression;
		List<Anchor> attributes;
		inline Variable(const Anchor& name, Visibility visibility, const FileType& type, const Anchor& expression) :compiling(NULL), name(name), visibility(visibility), type(type), expression(expression), attributes(0) {}
	};
	struct Constructor
	{
		CompilingClass::Constructor* compiling;
		Anchor name;
		Visibility visibility;
		List<FileParameter> parameters;
		Anchor expression;
		List<Line> body;
		List<Anchor> attributes;
		inline Constructor(const Anchor name, Visibility visibility, const List<FileParameter>& parameters, const Anchor& expression) :compiling(NULL), name(name), visibility(visibility), parameters(parameters), expression(expression), body(0), attributes(0) {}
	};
	CompilingClass* compiling;
	List<Anchor> parent;
	List<FileType> inherits;
	List<Variable> variables;
	List<Constructor> constructors;
	List<FileFunction> functions;
	List<Line> destructor;
	inline FileClass(const Anchor& name, Visibility visibility, FileSpace* space) :FileDeclaration(name, visibility, space), compiling(NULL), parent(0), inherits(0), variables(0), constructors(0), functions(0), destructor(0) {}
};
struct FileInterface :FileDeclaration
{
	struct Function
	{
		CompilingFunctionDeclaration* compiling;
		Anchor name;
		List<FileParameter> parameters;
		List<FileType> returns;
		List<Anchor> attributes;
		inline Function(const Anchor& name, const List<FileParameter>& parameters, const List<FileType>& returns) :compiling(NULL), name(name), parameters(parameters), returns(returns), attributes(0) {}
	};
	CompilingInterface* compiling;
	List<FileType> inherits;
	List<Function> functions;
	inline FileInterface(const Anchor& name, Visibility visibility, FileSpace* space) :FileDeclaration(name, visibility, space), compiling(NULL), inherits(0), functions(0) {}
};
struct FileDelegate :FileDeclaration
{
	CompilingDelegate* compiling;
	List<FileParameter> parameters;
	List<FileType> returns;
	inline FileDelegate(const Anchor& name, Visibility visibility, FileSpace* space, const List<FileParameter>& parameters, const List<FileType>& returns) :FileDeclaration(name, visibility, space), compiling(NULL), parameters(parameters), returns(returns) {}
};
struct FileCoroutine :FileDeclaration
{
	CompilingCoroutine* compiling;
	List<FileType> returns;
	inline FileCoroutine(const Anchor& name, Visibility visibility, FileSpace* space, const List<FileType>& returns) :FileDeclaration(name, visibility, space), compiling(NULL), returns(returns) {}
};
struct FileNative :FileDeclaration
{
	CompilingNative* compiling;
	List<FileParameter> parameters;
	List<FileType> returns;
	inline FileNative(const Anchor& name, Visibility visibility, FileSpace* space, const List<FileParameter>& parameters, const List<FileType>& returns) :FileDeclaration(name, visibility, space), compiling(NULL), parameters(parameters), returns(returns) {}
};
struct DeclarationManager;
struct FileSpace
{
	CompilingSpace* compiling;
	List<Anchor> attributes;
	List<FileSpace>children;
	List<List<Anchor>> imports;

	List<FileVariable> variables;
	List<FileFunction> functions;
	List<FileEnum> enums;
	List<FileStruct> structs;
	List<FileClass> classes;
	List<FileInterface> interfaces;
	List<FileDelegate> delegates;
	List<FileCoroutine> coroutines;
	List<FileNative> natives;

	List<CompilingSpace*, true> relyCompilingSpaces;
	List<AbstractSpace*, true> relySpaces;
	FileSpace(CompilingSpace* compiling, uint32 parentIndent, ParseParameter* parameter);
	void ParseChild(const Line& line, List<Anchor>& attributes, uint32 index, ParseParameter* parameter);
	bool ParseDeclaration(const Line& line, List<Anchor>& attributes, ParseParameter* parameter);
	void ParseEnum(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributes, ParseParameter* parameter);
	void ParseStruct(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributes, ParseParameter* parameter);
	void ParseClass(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributes, ParseParameter* parameter);
	void ParseInterface(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributes, ParseParameter* parameter);

	void InitRelies(DeclarationManager* manager);
	void Tidy(DeclarationManager* manager);
	void Link(DeclarationManager* manager, List<List<AbstractSpace*, true>>* relySpaceCollector);
};
