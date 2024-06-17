#include "FileSpace.h"
#include "LineReader.h"
#include "Lexical.h"
#include "../KeyWords.h"

#define DISCARD_ATTRIBUTE \
	for (uint32 i = 0; i < attributeCollector.Count(); i++)\
		MESSAGE2(parameter->messages, attributeCollector[i], MessageType::LOGGER_LEVEL1_DISCARD_ATTRIBUTE);\
	attributeCollector.Clear();

#define CHECK_VISIABLE(line,defaultVisiable)\
	if (visibility == Visibility::None)\
	{\
		MESSAGE2(parameter->messages, line, MessageType::LOGGER_LEVEL3_MISSING_VISIBILITY);\
		visibility = Visibility::defaultVisiable;\
	}

bool TryGetNames(const Line& line, uint32& index, Lexical& lexical, List<Anchor>* space, MessageCollector* messages)
{
	if(TryExtractName(line, lexical.anchor.GetEnd(), index, space, messages)) return true;
	else MESSAGE2(messages, line, MessageType::ERROR_MISSING_NAME);
	return false;
}
bool TryGetNextLexical(const Line& line, uint32 startIndex, LexicalType type, MessageType message, Lexical& lexical, MessageCollector* messages)
{
	if(TryAnalysis(line, startIndex, lexical, messages))
	{
		if(lexical.type == type) return true;
		else MESSAGE2(messages, line, MessageType::ERROR_UNEXPECTED_LEXCAL);
	}
	else if(message != MessageType::INVALID) MESSAGE2(messages, line, message);
	return false;
}
void CheckLineEnd(const Line& line, uint32 startIndex, MessageCollector* messages)
{
	Lexical lexical;
	if(TryAnalysis(line, startIndex, lexical, messages))
		MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
}
bool CheckIndent(const Line& line, uint32& indent, uint32 parentIndent)
{
	if(indent == INVALID)
	{
		indent = line.indent;
		if(indent <= parentIndent) return false;
	}
	else if(line.indent < indent) return false;
	return true;
}

bool TryParseAttributes(const Line& line, List<Anchor>& attributeCollector, MessageCollector* messages)
{
	Lexical lexical;
	if(TryAnalysis(line, 0, lexical, messages) && lexical.type == LexicalType::BracketLeft1)
	{
		while(TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages) && lexical.type == LexicalType::ConstString)
		{
			attributeCollector.Add(lexical.anchor);
			if(TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
			{
				if(lexical.type == LexicalType::BracketRight1)
				{
					if(TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
						MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
				}
				else if(lexical.type == LexicalType::Comma || lexical.type == LexicalType::Semicolon) continue;
				else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
			}
			else MESSAGE2(messages, line, MessageType::ERROR_INPUT_COMMA_OR_SEMICOLON);
			return true;
		}
		MESSAGE2(messages, line, MessageType::ERROR_INPUT_STRINGL);
		return true;
	}
	return false;
}

void ParseImport(const Line& line, Lexical& lexical, List<Anchor>* space, MessageCollector* messages)
{
	uint32 index = lexical.anchor.GetEnd();
	if(TryGetNames(line, index, lexical, space, messages)) CheckLineEnd(line, index, messages);
	else MESSAGE2(messages, line, MessageType::ERROR_MISSING_NAME);
}

Visibility ConvertVisibility(const Anchor& anchor)
{
	if(anchor == KeyWord_public()) return Visibility::Public;
	else if(anchor == KeyWord_internal()) return Visibility::Internal;
	else if(anchor == KeyWord_space()) return Visibility::Space;
	else if(anchor == KeyWord_protected()) return Visibility::Protected;
	else if(anchor == KeyWord_private()) return Visibility::Private;
	return Visibility::None;
}

Visibility ParseVisibility(const Line& line, uint32& index, MessageCollector* messages)
{
	Visibility result = Visibility::None;
	Lexical lexical;
	index = 0;
	while(TryAnalysis(line, index, lexical, messages) && lexical.type == LexicalType::Word)
	{
		Visibility visibility = ConvertVisibility(lexical.anchor);
		if(visibility == Visibility::None) break;
		else
		{
			if(visibility == result) MESSAGE2(messages, lexical.anchor, MessageType::WARRING_LEVEL1_REPEATED_VISIBILITY)
			else if(result == Visibility::None) result = visibility;
			else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_INVALID_VISIBILITY);
			index = lexical.anchor.GetEnd();
		}
	}
	return result;
}
void ParseBlock(uint32 parentIndent, List<Line>& lines, ParseParameter* parameter)
{
	uint32 indent = INVALID;
	Lexical lexical;
	while(parameter->reader->ReadLine())
	{
		Line line = parameter->reader->CurrentLine();
		if(TryAnalysis(line, 0, lexical, parameter->messages) && lexical.type != LexicalType::Annotation)
			if(CheckIndent(line, indent, parentIndent)) lines.Add(line);
			else break;
	}
}
bool TryParseParameters(const Line& line, uint32& index, List<FileParameter>& parameters, MessageCollector* messages)
{
	Lexical lexical;
	if(!TryGetNextLexical(line, index, LexicalType::BracketLeft0, MessageType::ERROR_UNEXPECTED_LINE_END, lexical, messages)) return false;

	index = lexical.anchor.GetEnd();
	if(TryAnalysis(line, index, lexical, messages))
	{
		if(lexical.type == LexicalType::BracketRight0)
		{
			index = lexical.anchor.GetEnd();
			return true;
		}
		else
		{
			List<Anchor> names = List<Anchor>(0);
		label_parse_parameter:
			names.Clear();
			if(TryExtractName(line, index, index, &names, messages))
			{
				FileType type = FileType(names, ExtractDimension(line, index));
				if(TryAnalysis(line, index, lexical, messages))
				{
					Anchor name;
					if(lexical.type == LexicalType::Word)
					{
						name = lexical.anchor;
						if(!TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
						{
							MESSAGE2(messages, line, MessageType::ERROR_UNEXPECTED_LINE_END);
							return false;
						}
					}
					new (parameters.Add()) FileParameter(name, type);
					if(lexical.type == LexicalType::Comma || lexical.type == LexicalType::Semicolon)
					{
						index = lexical.anchor.GetEnd();
						goto label_parse_parameter;
					}
					else if(lexical.type == LexicalType::BracketRight0)
					{
						index = lexical.anchor.GetEnd();
						return true;
					}
					else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
				}
				else MESSAGE2(messages, line, MessageType::ERROR_UNEXPECTED_LINE_END);
			}
			else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
		}
	}
	else MESSAGE2(messages, line, MessageType::ERROR_UNEXPECTED_LINE_END);
	return false;
}
bool TryParseTuple(const Line& line, uint32 index, Anchor& name, bool operatorReloadable, List<FileType>& types, MessageCollector* messages)
{
	List<Anchor> names = List<Anchor>(0);
	bool segmented = false;
	Lexical lexical;
label_parse_type:
	names.Clear();
	if(TryExtractName(line, index, index, &names, messages))
	{
		uint32 dimesnion = ExtractDimension(line, index);
		if(TryAnalysis(line, index, lexical, messages))
		{
			if(lexical.type == LexicalType::Word || (operatorReloadable && IsReloadable(lexical.type)))
			{
				new (types.Add())FileType(names, dimesnion);
				name = lexical.anchor;
				return true;
			}
			else if(lexical.type == LexicalType::Comma || lexical.type == LexicalType::Semicolon)
			{
				new (types.Add())FileType(names, dimesnion);
				segmented = true;
				index = lexical.anchor.GetEnd();
				goto label_parse_type;
			}
			else if(names.Count() == 1 && !segmented)
			{
				name = names[0];
				return true;
			}
			else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
		}
		else if(names.Count() == 1 && !segmented)
		{
			name = names[0];
			return true;
		}
		else MESSAGE2(messages, line, MessageType::ERROR_MISSING_NAME);
	}
	else if(!segmented && operatorReloadable && TryAnalysis(line, index, lexical, messages) && IsReloadable(lexical.type))
	{
		name = lexical.anchor;
		return true;
	}
	else MESSAGE2(messages, line, MessageType::ERROR_MISSING_NAME);
	return false;
}

bool TryParseVariable(const Line& line, uint32 index, Anchor& name, FileType& type, Anchor& expression, MessageCollector* messages)
{
	List<Anchor> typeName = List<Anchor>(0);
	if(TryExtractName(line, index, index, &typeName, messages))
	{
		type = FileType(typeName, ExtractDimension(line, index));
		Lexical lexical;
		if(TryAnalysis(line, index, lexical, messages))
		{
			if(lexical.type == LexicalType::Word)
			{
				name = lexical.anchor;
				if(TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
				{
					if(lexical.type == LexicalType::Assignment)
					{
						if(TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
						{
							expression = Anchor(line.source, line.content.Sub(lexical.anchor.position), line.number, lexical.anchor.position);
							return true;
						}
						else MESSAGE2(messages, line, MessageType::ERROR_MISSING_ASSIGNMENT_EXPRESSION);
					}
				}
				else
				{
					expression = Anchor();
					return true;
				}
			}
		}
	}
	return false;
}

bool ParseFunctionDeclaration(const Line& line, uint32 index, Anchor& name, bool operatorReloadable, List<FileParameter>& parameters, List<FileType>& returns, MessageCollector* messages)
{
	if(TryParseTuple(line, index, name, operatorReloadable, returns, messages))
	{
		index = name.GetEnd();
		if(TryParseParameters(line, index, parameters, messages))
		{
			CheckLineEnd(line, index, messages);
			return true;
		}
	}
	return false;
}

void ParseGlobalFunction(FileSpace* space, const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributeCollector, ParseParameter* parameter)
{
	Anchor name; List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
	ParseFunctionDeclaration(line, index, name, true, parameters, returns, parameter->messages);
	FileFunction* function = new (space->functions.Add())FileFunction(name, visibility, space, parameters, returns);
	function->attributes.Add(attributeCollector);
	attributeCollector.Clear();
	ParseBlock(line.indent, function->body, parameter);
}

FileSpace::FileSpace(CompilingSpace* compiling, uint32 parentIndent, ParseParameter* parameter) :source(parameter->reader->Source()), compiling(compiling), attributes(0), children(0), imports(0),
variables(0), functions(0), enums(0), structs(0), classes(0), interfaces(0), delegates(0), tasks(0), natives(0), relyCompilingSpaces(0), relySpaces(0)
{
	uint32 indent = INVALID;
	List<Anchor> attributeCollector = List<Anchor>(0);
	while(parameter->reader->ReadLine())
	{
	label_parse:
		Line line = parameter->reader->CurrentLine();

		if(TryParseAttributes(line, attributeCollector, parameter->messages)) continue;

		Lexical lexical;
		if(TryAnalysis(line, 0, lexical, parameter->messages))
		{
			if(indent == INVALID)
			{
				indent = line.indent;
				if(parentIndent != INVALID && indent <= parentIndent)break;
			}
			else if(line.indent > indent) MESSAGE2(parameter->messages, line, MessageType::ERROR_INDENT)
			else if(line.indent < indent)
			{
				if(parentIndent == INVALID) MESSAGE2(parameter->messages, line, MessageType::ERROR_INDENT)
				else
				{
					this->attributes.Add(attributeCollector.GetPointer(), attributeCollector.Count());
					break;
				}
			}
			if(lexical.type == LexicalType::Word)
			{
				if(lexical.anchor == KeyWord_import())
				{
					if(attributeCollector.Count()) MESSAGE2(parameter->messages, lexical.anchor, MessageType::ERROR_ATTRIBUTE_INVALID);
					ParseImport(line, lexical, new (imports.Add())List<Anchor>(0), parameter->messages);
					attributeCollector.Clear();
				}
				else if(lexical.anchor == KeyWord_namespace())
				{
					bool readline = ParseChild(line, attributeCollector, lexical.anchor.GetEnd(), parameter);
					attributeCollector.Clear();
					if(readline) goto label_parse;
				}
				else if(ParseDeclaration(line, attributeCollector, parameter)) goto label_parse;
			}
			else if(IsReloadable(lexical.type))
			{
				ParseGlobalFunction(this, line, 0, Visibility::Space, attributeCollector, parameter);
				goto label_parse;
			}
			else MESSAGE2(parameter->messages, line, MessageType::ERROR_UNEXPECTED_LEXCAL);
		}
	}
	this->attributes.Add(attributeCollector.GetPointer(), attributeCollector.Count());
}

bool FileSpace::ParseChild(const Line& line, List<Anchor>& attributeCollector, uint32 index, ParseParameter* parameter)
{
	List<Anchor> name = List<Anchor>(0);
	if(TryExtractName(line, index, index, &name, parameter->messages))
	{
		CheckLineEnd(line, index, parameter->messages);
		CompilingSpace* space = compiling;
		for(uint32 i = 0; i < name.Count(); i++)
			space = space->GetChild(name[i].content);
		FileSpace* fileSpace = new FileSpace(space, line.indent, parameter);
		children.Add(fileSpace);
		fileSpace->attributes.Add(attributeCollector.GetPointer(), attributeCollector.Count());
		return true;
	}
	else MESSAGE2(parameter->messages, line, MessageType::ERROR_MISSING_NAME);
	return false;
}

//如果预读了下一行代码则返回true
bool FileSpace::ParseDeclaration(const Line& line, List<Anchor>& attributeCollector, ParseParameter* parameter)
{
	uint32 index;
	Visibility visibility = ParseVisibility(line, index, parameter->messages);
	Lexical lexical;
	if(!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_UNEXPECTED_LINE_END, lexical, parameter->messages)) return false;

	CHECK_VISIABLE(line, Space);
	Anchor name, expression; FileType type;
	if(lexical.anchor == KeyWord_const())
	{
		if(TryParseVariable(line, lexical.anchor.GetEnd(), name, type, expression, parameter->messages))
		{
			if(expression.content.IsEmpty())MESSAGE2(parameter->messages, name, MessageType::ERROR_CONSTANT_NOT_ASSIGNMENT)
			else (new (variables.Add())FileVariable(name, visibility, this, true, type, expression))->attributes.Add(attributeCollector);
		}
		else MESSAGE2(parameter->messages, line, MessageType::ERROR_NOT_VARIABLE_DECLARATION);
		attributeCollector.Clear();
	}
	else if(lexical.anchor == KeyWord_enum())
	{
		bool readline = ParseEnum(line, lexical.anchor.GetEnd(), visibility, attributeCollector, parameter);
		attributeCollector.Clear();
		return readline;
	}
	else if(lexical.anchor == KeyWord_struct())
	{
		bool readline = ParseStruct(line, lexical.anchor.GetEnd(), visibility, attributeCollector, parameter);
		attributeCollector.Clear();
		return readline;
	}
	else if(lexical.anchor == KeyWord_class())
	{
		bool readline = ParseClass(line, lexical.anchor.GetEnd(), visibility, attributeCollector, parameter);
		attributeCollector.Clear();
		return readline;
	}
	else if(lexical.anchor == KeyWord_interface())
	{
		bool readline = ParseInterface(line, lexical.anchor.GetEnd(), visibility, attributeCollector, parameter);
		attributeCollector.Clear();
		return readline;
	}
	else if(lexical.anchor == KeyWord_delegate())
	{
		List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
		ParseFunctionDeclaration(line, lexical.anchor.GetEnd(), name, false, parameters, returns, parameter->messages);
		(new (delegates.Add())FileDelegate(name, visibility, this, parameters, returns))->attributes.Add(attributeCollector);
		attributeCollector.Clear();
	}
	else if(lexical.anchor == KeyWord_task())
	{
		List<FileType> returns = List<FileType>(0);
		if(TryParseTuple(line, lexical.anchor.GetEnd(), name, false, returns, parameter->messages))
		{
			CheckLineEnd(line, name.GetEnd(), parameter->messages);
			(new (tasks.Add())FileTask(name, visibility, this, returns))->attributes.Add(attributeCollector);
		}
		attributeCollector.Clear();
	}
	else if(lexical.anchor == KeyWord_native())
	{
		List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
		ParseFunctionDeclaration(line, lexical.anchor.GetEnd(), name, false, parameters, returns, parameter->messages);
		(new (natives.Add())FileNative(name, visibility, this, parameters, returns))->attributes.Add(attributeCollector);
		attributeCollector.Clear();
	}
	else if(TryParseVariable(line, index, name, type, expression, parameter->messages))
	{
		(new (variables.Add())FileVariable(name, visibility, this, false, type, expression))->attributes.Add(attributeCollector);
		attributeCollector.Clear();
	}
	else
	{
		ParseGlobalFunction(this, line, index, visibility, attributeCollector, parameter);
		return true;
	}
	return false;
}

bool FileSpace::ParseEnum(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributeCollector, ParseParameter* parameter)
{
	Lexical lexical;
	if(!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages)) return false;
	CheckLineEnd(line, lexical.anchor.GetEnd(), parameter->messages);

	FileEnum* fileEnum = new (enums.Add())FileEnum(lexical.anchor, visibility, this);
	fileEnum->attributes.Add(attributeCollector);
	attributeCollector.Clear();
	uint32 indent = INVALID;
	while(parameter->reader->ReadLine())
	{
		Line current = parameter->reader->CurrentLine();
		if(!TryAnalysis(current, 0, lexical, parameter->messages)) continue;
		if(!CheckIndent(current, indent, line.indent)) break;

		if(TryGetNextLexical(current, 0, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages))
		{
			Anchor name = lexical.anchor;
			if(TryAnalysis(current, lexical.anchor.GetEnd(), lexical, parameter->messages))
			{
				if(lexical.type == LexicalType::Assignment)
				{
					if(TryAnalysis(current, lexical.anchor.GetEnd(), lexical, parameter->messages))
						new (fileEnum->elements.Add())FileEnum::Element(name, Anchor(current.source, current.content.Sub(lexical.anchor.position), current.number, lexical.anchor.position));
					else MESSAGE2(parameter->messages, name, MessageType::ERROR_MISSING_ASSIGNMENT_EXPRESSION);
				}
				else MESSAGE2(parameter->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
			}
			else new (fileEnum->elements.Add())FileEnum::Element(name, Anchor());
		}
	}
	DISCARD_ATTRIBUTE;
	return true;
}

bool FileSpace::ParseStruct(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributeCollector, ParseParameter* parameter)
{
	Lexical lexical;
	if(!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages)) return false;
	CheckLineEnd(line, lexical.anchor.GetEnd(), parameter->messages);

	FileStruct* fileStruct = new (structs.Add())FileStruct(lexical.anchor, visibility, this);
	fileStruct->attributes.Add(attributeCollector);
	attributeCollector.Clear();
	uint32 indent = INVALID;
	while(parameter->reader->ReadLine())
	{
	label_parse:
		Line current = parameter->reader->CurrentLine();
		if(!TryAnalysis(current, 0, lexical, parameter->messages)) continue;
		if(!CheckIndent(current, indent, line.indent))break;

		if(TryParseAttributes(current, attributeCollector, parameter->messages)) continue;
		Anchor name, expression; FileType type;
		visibility = ParseVisibility(current, index, parameter->messages);
		if(TryParseVariable(current, index, name, type, expression, parameter->messages))
		{
			if(visibility != Visibility::None) MESSAGE2(parameter->messages, current, MessageType::ERROR_INVALID_VISIBILITY);
			if(!expression.content.IsEmpty()) MESSAGE2(parameter->messages, expression, MessageType::ERROR_INVALID_INITIALIZER);
			(new (fileStruct->variables.Add()) FileStruct::Variable(name, type))->attributes.Add(attributeCollector);
			attributeCollector.Clear();
		}
		else
		{
			CHECK_VISIABLE(current, Private);
			List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
			ParseFunctionDeclaration(current, index, name, false, parameters, returns, parameter->messages);
			FileFunction* function = new (fileStruct->functions.Add()) FileFunction(name, visibility, this, parameters, returns);
			function->attributes.Add(attributeCollector);
			attributeCollector.Clear();
			ParseBlock(indent, function->body, parameter);
			goto label_parse;
		}
	}
	DISCARD_ATTRIBUTE;
	return true;
}

bool FileSpace::ParseClass(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributeCollector, ParseParameter* parameter)
{
	Lexical lexical;
	if(!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages)) return false;
	FileClass* fileClass = new (classes.Add())FileClass(lexical.anchor, visibility, this);
	fileClass->attributes.Add(attributeCollector);
	attributeCollector.Clear();

	index = lexical.anchor.GetEnd();
	if(TryExtractName(line, index, index, &fileClass->parent, parameter->messages))
	{
		List<Anchor> names = List<Anchor>(0);
	lable_parse_inherits:
		names.Clear();
		if(TryExtractName(line, index, index, &names, parameter->messages))
		{
			new (fileClass->inherits.Add())FileType(names, ExtractDimension(line, index));
			goto lable_parse_inherits;
		}
		else CheckLineEnd(line, index, parameter->messages);
	}
	else CheckLineEnd(line, index, parameter->messages);


	uint32 indent = INVALID;
	while(parameter->reader->ReadLine())
	{
	label_parse:
		Line current = parameter->reader->CurrentLine();
		if(!TryAnalysis(current, 0, lexical, parameter->messages)) continue;
		if(!CheckIndent(current, indent, line.indent))break;

		if(TryParseAttributes(current, attributeCollector, parameter->messages))continue;
		visibility = ParseVisibility(current, index, parameter->messages);
		Anchor name, expression; FileType type;
		if(TryParseVariable(current, index, name, type, expression, parameter->messages))
		{
			CHECK_VISIABLE(current, Private);
			(new (fileClass->variables.Add())FileClass::Variable(name, visibility, type, expression))->attributes.Add(attributeCollector);
			attributeCollector.Clear();
		}
		else if(TryAnalysis(current, index, lexical, parameter->messages))
		{
			if(lexical.type == LexicalType::Negate)//析构函数
			{
				if(visibility != Visibility::None) MESSAGE2(parameter->messages, current, MessageType::ERROR_INVALID_VISIBILITY);
				if(attributeCollector.Count())
				{
					MESSAGE2(parameter->messages, lexical.anchor, MessageType::WARRING_LEVEL1_DESTRUCTOR_ATTRIBUTES);
					DISCARD_ATTRIBUTE;
				}
				if(visibility != Visibility::None)
					MESSAGE2(parameter->messages, lexical.anchor, MessageType::WARRING_LEVEL1_DESTRUCTOR_VISIBILITY);
				ParseBlock(current.indent, fileClass->destructor, parameter);
				goto label_parse;
			}
			else
			{
				CHECK_VISIABLE(current, Private);
				List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
				if(TryParseTuple(current, index, name, false, returns, parameter->messages))
				{
					index = name.GetEnd();
					if(TryParseParameters(current, index, parameters, parameter->messages))
					{
						if(name.content == fileClass->name.content)
						{
							if(returns.Count()) MESSAGE3(parameter->messages, name, MessageType::ERROR, TEXT("构造函数不能有返回值"));
							expression = Anchor();
							if(TryAnalysis(current, index, lexical, parameter->messages))
							{
								if(lexical.type == LexicalType::Word && (lexical.anchor == KeyWord_base() || lexical.anchor == KeyWord_this()))
									expression = Anchor(current.source, current.content.Sub(lexical.anchor.position), current.number, lexical.anchor.position);
								else MESSAGE2(parameter->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
							}
							FileClass::Constructor* constuctor = new (fileClass->constructors.Add())FileClass::Constructor(name, visibility, parameters, expression);
							constuctor->attributes.Add(attributeCollector);
							attributeCollector.Clear();
							ParseBlock(current.indent, constuctor->body, parameter);
						}
						else
						{
							CheckLineEnd(current, index, parameter->messages);
							FileFunction* function = new (fileClass->functions.Add())FileFunction(name, visibility, this, parameters, returns);
							function->attributes.Add(attributeCollector);
							attributeCollector.Clear();
							ParseBlock(indent, function->body, parameter);
						}
						goto label_parse;
					}
				}
			}
		}
	}
	DISCARD_ATTRIBUTE;
	return true;
}

bool FileSpace::ParseInterface(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributeCollector, ParseParameter* parameter)
{
	Lexical lexical;
	if(!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages)) return false;
	FileInterface* fileInterface = new (interfaces.Add())FileInterface(lexical.anchor, visibility, this);
	fileInterface->attributes.Add(attributeCollector);
	attributeCollector.Clear();

	List<Anchor> names = List<Anchor>(0);
	index = lexical.anchor.GetEnd();
lable_parse_inherits:
	names.Clear();
	if(TryExtractName(line, index, index, &names, parameter->messages))
	{
		new (fileInterface->inherits.Add())FileType(names, ExtractDimension(line, index));
		goto lable_parse_inherits;
	}
	else CheckLineEnd(line, index, parameter->messages);

	uint32 indent = INVALID;
	while(parameter->reader->ReadLine())
	{
		Line current = parameter->reader->CurrentLine();
		if(!TryAnalysis(current, 0, lexical, parameter->messages)) continue;
		if(!CheckIndent(current, indent, line.indent)) break;

		if(TryParseAttributes(current, attributeCollector, parameter->messages)) continue;
		visibility = ParseVisibility(current, index, parameter->messages);
		if(visibility != Visibility::None) MESSAGE2(parameter->messages, current, MessageType::ERROR_INVALID_VISIBILITY);
		Anchor name; List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
		ParseFunctionDeclaration(current, index, name, false, parameters, returns, parameter->messages);
		(new (fileInterface->functions.Add()) FileInterface::Function(name, parameters, returns))->attributes.Add(attributeCollector);
		attributeCollector.Clear();
	}
	DISCARD_ATTRIBUTE;
	return true;
}
