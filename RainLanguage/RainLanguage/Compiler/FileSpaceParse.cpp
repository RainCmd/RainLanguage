#include "FileSpace.h"
#include "LineReader.h"
#include "Lexical.h"
#include "../KeyWords.h"

#define DISCARD_ATTRIBUTE \
	for (uint32 i = 0; i < attributes.Count(); i++)\
		MESSAGE2(parameter->messages, attributes[i], MessageType::LOGGER_LEVEL1_DISCARD_ATTRIBUTE);\
	attributes.Clear();

#define CHECK_VISIABLE(line,defaultVisiable)\
	if (visibility == Visibility::None)\
	{\
		MESSAGE2(parameter->messages, line, MessageType::LOGGER_LEVEL4_MISSING_VISIBILITY);\
		visibility = Visibility::defaultVisiable;\
	}

bool TryGetNames(const Line& line, uint32& index, Lexical& lexical, List<Anchor>* space, MessageCollector* messages)
{
	if (TryExtractName(line, lexical.anchor.GetEnd(), index, space, messages)) return true;
	else MESSAGE2(messages, line, MessageType::ERROR_MISSING_NAME);
	return false;
}
bool TryGetNextLexical(const Line& line, uint32 startIndex, LexicalType type, MessageType message, Lexical& lexical, MessageCollector* messages)
{
	if (TryAnalysis(line, startIndex, lexical, messages))
	{
		if (lexical.type == type) return true;
		else MESSAGE2(messages, line, MessageType::ERROR_UNEXPECTED_LEXCAL);
	}
	else if (message != MessageType::INVALID) MESSAGE2(messages, line, message);
	return false;
}
void CheckLineEnd(const Line& line, uint32 startIndex, MessageCollector* messages)
{
	Lexical lexical;
	if (TryAnalysis(line, startIndex, lexical, messages))
		MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
}
bool CheckIndent(const Line& line, uint32& indent, uint32 parentIndent)
{
	if (indent == INVALID)
	{
		indent = line.indent;
		if (indent <= parentIndent)return false;
	}
	else if (line.indent < indent)return false;
	return true;
}

bool TryParseAttributes(const Line& line, List<Anchor>& attributes, MessageCollector* messages)
{
	Lexical lexical;
	if (TryAnalysis(line, 0, lexical, messages) && lexical.type == LexicalType::BracketLeft1)
	{
		while (TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
		{
			if (lexical.type == LexicalType::ConstString)attributes.Add(lexical.anchor);
			else
			{
				MESSAGE2(messages, lexical.anchor, MessageType::ERROR_INPUT_STRINGL);
				return false;
			}
			if (TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
			{
				if (lexical.type == LexicalType::BracketRight1)
				{
					if (TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
						MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
				}
				else if (lexical.type == LexicalType::Comma || lexical.type == LexicalType::Semicolon)continue;
				else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
			}
			else MESSAGE2(messages, line, MessageType::ERROR_INPUT_COMMA_OR_SEMICOLON);
			return false;
		}
		MESSAGE2(messages, line, MessageType::ERROR_INPUT_STRINGL);
		return true;
	}
	return false;
}

void ParseImport(const Line& line, Lexical& lexical, List<Anchor>* space, MessageCollector* messages)
{
	uint32 index = lexical.anchor.GetEnd();
	if (TryGetNames(line, index, lexical, space, messages)) CheckLineEnd(line, index, messages);
	else MESSAGE2(messages, line, MessageType::ERROR_MISSING_NAME);
}

Visibility ParseVisibility(const Line& line, uint32& index, MessageCollector* messages)
{
	Visibility visibility = Visibility::None;
	Lexical lexical;
	index = 0;
	while (TryAnalysis(line, index, lexical, messages) && lexical.type == LexicalType::Word)
	{
		index = lexical.anchor.GetEnd();
		if (lexical.anchor == KeyWord_public())
		{
			if (IsClash(visibility, Visibility::Public))MESSAGE2(messages, lexical.anchor, MessageType::WARRING_LEVEL1_REPEATED_VISIBILITY);
			visibility |= Visibility::Public;
		}
		else if (lexical.anchor == KeyWord_internal())
		{
			if (IsClash(visibility, Visibility::Internal))MESSAGE2(messages, lexical.anchor, MessageType::WARRING_LEVEL1_REPEATED_VISIBILITY);
			visibility |= Visibility::Internal;
		}
		else if (lexical.anchor == KeyWord_space())
		{
			if (IsClash(visibility, Visibility::Space))MESSAGE2(messages, lexical.anchor, MessageType::WARRING_LEVEL1_REPEATED_VISIBILITY);
			visibility |= Visibility::Space;
		}
		else if (lexical.anchor == KeyWord_protected())
		{
			if (IsClash(visibility, Visibility::Protected))MESSAGE2(messages, lexical.anchor, MessageType::WARRING_LEVEL1_REPEATED_VISIBILITY);
			visibility |= Visibility::Protected;
		}
		else if (lexical.anchor == KeyWord_private())
		{
			if (IsClash(visibility, Visibility::Private))MESSAGE2(messages, lexical.anchor, MessageType::WARRING_LEVEL1_REPEATED_VISIBILITY);
			visibility |= Visibility::Private;
		}
		else
		{
			index = lexical.anchor.position;
			break;
		}
	}
	return visibility;
}
void ParseBlock(uint32 parentIndent, List<Line>& lines, ParseParameter* parameter)
{
	uint32 indent = INVALID;
	while (parameter->reader->ReadLine())
	{
		Line line = parameter->reader->CurrentLine();
		if (CheckIndent(line, indent, parentIndent))lines.Add(line);
		else break;
	}
}
bool TryParseParameters(const Line& line, uint32& index, List<FileParameter>& parameters, MessageCollector* messages)
{
	Lexical lexical;
	if (!TryGetNextLexical(line, index, LexicalType::BracketLeft0, MessageType::ERROR_UNEXPECTED_LINE_END, lexical, messages)) return false;

	index = lexical.anchor.GetEnd();
	if (TryAnalysis(line, index, lexical, messages))
	{
		if (lexical.type == LexicalType::BracketRight0)
		{
			index = lexical.anchor.GetEnd();
			return true;
		}
		else
		{
			List<Anchor> names = List<Anchor>(0);
		label_parse_parameter:
			names.Clear();
			if (TryExtractName(line, index, index, &names, messages))
			{
				FileType type = FileType(names, ExtractDimension(line, index));
				if (TryAnalysis(line, index, lexical, messages))
				{
					Anchor name;
					if (lexical.type == LexicalType::Word)
					{
						name = lexical.anchor;
						if (!TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
						{
							MESSAGE2(messages, line, MessageType::ERROR_UNEXPECTED_LINE_END);
							return false;
						}
					}
					new (parameters.Add())FileParameter(name, type);
					if (lexical.type == LexicalType::Comma || lexical.type == LexicalType::Semicolon)
					{
						index = lexical.anchor.GetEnd();
						goto label_parse_parameter;
					}
					else if (lexical.type == LexicalType::BracketRight0)
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
	if (TryExtractName(line, index, index, &names, messages))
	{
		if (TryAnalysis(line, index, lexical, messages))
		{
			if (lexical.type == LexicalType::Word || (operatorReloadable && IsReloadable(lexical.type)))
			{
				new (types.Add())FileType(names, 0);
				name = lexical.anchor;
				return true;
			}
			else if (lexical.type == LexicalType::Comma || lexical.type == LexicalType::Semicolon)
			{
				new (types.Add())FileType(names, 0);
				segmented = true;
				index = lexical.anchor.GetEnd();
				goto label_parse_type;
			}
			else if (names.Count() == 1 && !segmented)
			{
				name = names[0];
				return true;
			}
			else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
		}
		else if (names.Count() == 1 && !segmented)
		{
			name = names[0];
			return true;
		}
		else MESSAGE2(messages, line, MessageType::ERROR_MISSING_NAME);
	}
	else if (!segmented && operatorReloadable && TryAnalysis(line, index, lexical, messages) && IsReloadable(lexical.type))
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
	if (TryExtractName(line, index, index, &typeName, messages))
	{
		type = FileType(typeName, ExtractDimension(line, index));
		Lexical lexical;
		if (TryAnalysis(line, index, lexical, messages))
		{
			if (lexical.type == LexicalType::Word)
			{
				name = lexical.anchor;
				if (TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
				{
					if (lexical.type == LexicalType::Assignment)
					{
						if (TryAnalysis(line, lexical.anchor.GetEnd(), lexical, messages))
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

void ParseFunctionDeclaration(const Line& line, uint32 index, Anchor& name, bool operatorReloadable, List<FileParameter>& parameters, List<FileType>& returns, MessageCollector* messages)
{
	if (TryParseTuple(line, index, name, operatorReloadable, returns, messages))
	{
		index = name.GetEnd();
		if (TryParseParameters(line, index, parameters, messages))
			CheckLineEnd(line, index, messages);
	}
}

FileSpace::FileSpace(CompilingSpace* compiling, uint32 parentIndent, ParseParameter* parameter) :compiling(compiling), attributes(0), children(0), imports(0),
variables(0), functions(0), enums(0), structs(0), classes(0), interfaces(0), delegates(0), coroutines(0), natives(0), relyCompilingSpaces(0), relySpaces(0)
{
	uint32 indent = INVALID;
	List<Anchor> attributes = List<Anchor>(0);
	while (parameter->reader->ReadLine())
	{
	label_parse:
		Line line = parameter->reader->CurrentLine();
		if (line.content.IsEmpty())return;
		if (TryParseAttributes(line, attributes, parameter->messages)) continue;

		Lexical lexical;
		if (TryGetNextLexical(line, 0, LexicalType::Word, MessageType::INVALID, lexical, parameter->messages))
		{
			if (indent == INVALID)
			{
				indent = line.indent;
				if (parentIndent != INVALID && indent <= parentIndent)return;
			}
			else if (line.indent > indent)MESSAGE2(parameter->messages, line, MessageType::ERROR_INDENT)
			else if (line.indent < indent)
			{
				if (parentIndent == INVALID)MESSAGE2(parameter->messages, line, MessageType::ERROR_INDENT)
				else
				{
					this->attributes.Add(attributes.GetPointer(), attributes.Count());
					return;
				}
			}
			if (lexical.anchor == KeyWord_import())
			{
				if (attributes.Count()) MESSAGE2(parameter->messages, lexical.anchor, MessageType::ERROR_ATTRIBUTE_INVALID);
				ParseImport(line, lexical, new (imports.Add())List<Anchor>(0), parameter->messages);
				attributes.Clear();
			}
			else if (lexical.anchor == KeyWord_namespace())
			{
				ParseChild(line, attributes, lexical.anchor.GetEnd(), parameter);
				attributes.Clear();
				goto label_parse;
			}
			else if (ParseDeclaration(line, attributes, parameter)) goto label_parse;
		}
	}
	this->attributes.Add(attributes.GetPointer(), attributes.Count());
}

void FileSpace::ParseChild(const Line& line, List<Anchor>& attributes, uint32 index, ParseParameter* parameter)
{
	List<Anchor> name = List<Anchor>(0);
	if (TryExtractName(line, index, index, &name, parameter->messages))
	{
		CheckLineEnd(line, index, parameter->messages);
		CompilingSpace* space = compiling;
		for (uint32 i = 0; i < name.Count(); i++)
			space = space->GetChild(name[i].content);
		(new (children.Add())FileSpace(space, line.indent, parameter))->attributes.Add(attributes.GetPointer(), attributes.Count());
	}
	else MESSAGE2(parameter->messages, line, MessageType::ERROR_MISSING_NAME);
}

//如果预读了下一行代码则返回true
bool FileSpace::ParseDeclaration(const Line& line, List<Anchor>& attributes, ParseParameter* parameter)
{
	uint32 index;
	Visibility visibility = ParseVisibility(line, index, parameter->messages);
	Lexical lexical;
	if (!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_UNEXPECTED_LINE_END, lexical, parameter->messages))return false;

	CHECK_VISIABLE(line, Space);
	Anchor name, expression; FileType type;
	if (lexical.anchor == KeyWord_const())
	{
		if (TryParseVariable(line, lexical.anchor.GetEnd(), name, type, expression, parameter->messages))
		{
			if (expression.content.IsEmpty())MESSAGE2(parameter->messages, name, MessageType::ERROR_CONSTANT_NOT_ASSIGNMENT)
			else (new (variables.Add())FileVariable(name, visibility, this, true, type, expression))->attributes.Add(attributes);
		}
		else MESSAGE2(parameter->messages, line, MessageType::ERROR_NOT_VARIABLE_DECLARATION);
		attributes.Clear();
	}
	else if (lexical.anchor == KeyWord_enum())
	{
		ParseEnum(line, lexical.anchor.GetEnd(), visibility, attributes, parameter);
		attributes.Clear();
		return true;
	}
	else if (lexical.anchor == KeyWord_struct())
	{
		ParseStruct(line, lexical.anchor.GetEnd(), visibility, attributes, parameter);
		attributes.Clear();
		return true;
	}
	else if (lexical.anchor == KeyWord_class())
	{
		ParseClass(line, lexical.anchor.GetEnd(), visibility, attributes, parameter);
		attributes.Clear();
		return true;
	}
	else if (lexical.anchor == KeyWord_interface())
	{
		ParseInterface(line, lexical.anchor.GetEnd(), visibility, attributes, parameter);
		attributes.Clear();
		return true;
	}
	else if (lexical.anchor == KeyWord_delegate())
	{
		List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
		ParseFunctionDeclaration(line, lexical.anchor.GetEnd(), name, false, parameters, returns, parameter->messages);
		(new (delegates.Add())FileDelegate(name, visibility, this, parameters, returns))->attributes.Add(attributes);
		attributes.Clear();
	}
	else if (lexical.anchor == KeyWord_coroutine())
	{
		List<FileType> returns = List<FileType>(0);
		if (TryParseTuple(line, lexical.anchor.GetEnd(), name, false, returns, parameter->messages))
		{
			CheckLineEnd(line, name.GetEnd(), parameter->messages);
			(new (coroutines.Add())FileCoroutine(name, visibility, this, returns))->attributes.Add(attributes);
		}
		attributes.Clear();
	}
	else if (lexical.anchor == KeyWord_native())
	{
		List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
		ParseFunctionDeclaration(line, lexical.anchor.GetEnd(), name, false, parameters, returns, parameter->messages);
		(new (natives.Add())FileNative(name, visibility, this, parameters, returns))->attributes.Add(attributes);
		attributes.Clear();
	}
	else if (TryParseVariable(line, index, name, type, expression, parameter->messages))
	{
		(new (variables.Add())FileVariable(name, visibility, this, false, type, expression))->attributes.Add(attributes);
		attributes.Clear();
	}
	else
	{
		List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
		ParseFunctionDeclaration(line, index, name, true, parameters, returns, parameter->messages);
		FileFunction* function = new (functions.Add())FileFunction(name, visibility, this, parameters, returns);
		function->attributes.Add(attributes);
		attributes.Clear();
		ParseBlock(line.indent, function->body, parameter);
		return true;
	}
	return false;
}

void FileSpace::ParseEnum(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributes, ParseParameter* parameter)
{
	Lexical lexical;
	if (!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages))return;
	CheckLineEnd(line, lexical.anchor.GetEnd(), parameter->messages);

	FileEnum* fileEnum = new (enums.Add())FileEnum(lexical.anchor, visibility, this);
	fileEnum->attributes.Add(attributes);
	attributes.Clear();
	uint32 indent = INVALID;
	while (parameter->reader->ReadLine())
	{
		Line current = parameter->reader->CurrentLine();
		if (!CheckIndent(current, indent, line.indent)) break;

		if (TryGetNextLexical(current, 0, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages))
		{
			Anchor name = lexical.anchor;
			if (TryAnalysis(current, lexical.anchor.GetEnd(), lexical, parameter->messages))
			{
				if (lexical.type == LexicalType::Assignment)
				{
					if (TryAnalysis(current, lexical.anchor.GetEnd(), lexical, parameter->messages))
						new (fileEnum->elements.Add())FileEnum::Element(name, Anchor(current.source, current.content.Sub(lexical.anchor.position), current.number, lexical.anchor.position));
					else MESSAGE2(parameter->messages, name, MessageType::ERROR_MISSING_ASSIGNMENT_EXPRESSION);
				}
				else MESSAGE2(parameter->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
			}
			else new (fileEnum->elements.Add())FileEnum::Element(name, Anchor());
		}
	}
	DISCARD_ATTRIBUTE;
}

void FileSpace::ParseStruct(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributes, ParseParameter* parameter)
{
	Lexical lexical;
	if (!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages))return;
	CheckLineEnd(line, lexical.anchor.GetEnd(), parameter->messages);

	FileStruct* fileStruct = new (structs.Add())FileStruct(lexical.anchor, visibility, this);
	fileStruct->attributes.Add(attributes);
	attributes.Clear();
	uint32 indent = INVALID;
	while (parameter->reader->ReadLine())
	{
	label_parse:
		Line current = parameter->reader->CurrentLine();
		if (!CheckIndent(current, indent, line.indent))break;

		if (TryParseAttributes(current, attributes, parameter->messages))continue;
		Anchor name, expression; FileType type;
		if (TryParseVariable(current, 0, name, type, expression, parameter->messages))
		{
			if (!expression.content.IsEmpty())MESSAGE2(parameter->messages, expression, MessageType::ERROR_INVALID_INITIALIZER);
			(new (fileStruct->variables.Add())FileStruct::Variable(name, type))->attributes.Add(attributes);
			attributes.Clear();
		}
		else
		{
			visibility = ParseVisibility(current, index, parameter->messages);
			List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
			ParseFunctionDeclaration(current, index, name, false, parameters, returns, parameter->messages);
			FileFunction* function = new (fileStruct->functions.Add())FileFunction(name, visibility, this, parameters, returns);
			function->attributes.Add(attributes);
			attributes.Clear();
			ParseBlock(indent, function->body, parameter);
			goto label_parse;
		}
	}
	DISCARD_ATTRIBUTE;
}

void FileSpace::ParseClass(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributes, ParseParameter* parameter)
{
	Lexical lexical;
	if (!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages))return;
	FileClass* fileClass = new (classes.Add())FileClass(lexical.anchor, visibility, this);
	fileClass->attributes.Add(attributes);
	attributes.Clear();

	index = lexical.anchor.GetEnd();
	if (TryExtractName(line, index, index, &fileClass->parent, parameter->messages))
	{
		List<Anchor> names = List<Anchor>(0);
	lable_parse_inherits:
		names.Clear();
		if (TryExtractName(line, index, index, &names, parameter->messages))
		{
			new (fileClass->inherits.Add())FileType(names, ExtractDimension(line, index));
			goto lable_parse_inherits;
		}
		else CheckLineEnd(line, index, parameter->messages);
	}
	else CheckLineEnd(line, index, parameter->messages);


	uint32 indent = INVALID;
	while (parameter->reader->ReadLine())
	{
	label_parse:
		Line current = parameter->reader->CurrentLine();
		if (!CheckIndent(current, indent, line.indent))break;

		if (TryParseAttributes(current, attributes, parameter->messages))continue;
		visibility = ParseVisibility(current, index, parameter->messages);
		Anchor name, expression; FileType type;
		if (TryParseVariable(current, index, name, type, expression, parameter->messages))
		{
			CHECK_VISIABLE(current, Private);
			(new (fileClass->variables.Add())FileClass::Variable(name, visibility, type, expression))->attributes.Add(attributes);
			attributes.Clear();
		}
		else if (TryAnalysis(current, index, lexical, parameter->messages))
		{
			if (lexical.type == LexicalType::Negate)//析构函数
			{
				if (attributes.Count())
				{
					MESSAGE2(parameter->messages, lexical.anchor, MessageType::WARRING_LEVEL1_DESTRUCTOR_ATTRIBUTES);
					DISCARD_ATTRIBUTE;
				}
				if (visibility != Visibility::None)
					MESSAGE2(parameter->messages, lexical.anchor, MessageType::WARRING_LEVEL1_DESTRUCTOR_VISIBILITY);
				ParseBlock(current.indent, fileClass->destructor, parameter);
				goto label_parse;
			}
			else
			{
				CHECK_VISIABLE(current, Private);
				List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
				if (TryParseTuple(current, index, name, false, returns, parameter->messages))
				{
					index = name.GetEnd();
					if (TryParseParameters(current, index, parameters, parameter->messages))
					{
						if (name.content == fileClass->name.content)
						{
							if (returns.Count())MESSAGE3(parameter->messages, name, MessageType::ERROR, TEXT("构造函数不能有返回值"));
							expression = Anchor();
							if (TryAnalysis(current, index, lexical, parameter->messages))
							{
								if (lexical.type == LexicalType::Word && (lexical.anchor == KeyWord_base() || lexical.anchor == KeyWord_this()))
									expression = Anchor(current.source, current.content.Sub(lexical.anchor.position), current.number, lexical.anchor.position);
								else MESSAGE2(parameter->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
							}
							FileClass::Constructor* constuctor = new (fileClass->constructors.Add())FileClass::Constructor(name, visibility, parameters, expression);
							constuctor->attributes.Add(attributes);
							attributes.Clear();
							ParseBlock(current.indent, constuctor->body, parameter);
						}
						else
						{
							CheckLineEnd(current, index, parameter->messages);
							FileFunction* function = new (fileClass->functions.Add())FileFunction(name, visibility, this, parameters, returns);
							function->attributes.Add(attributes);
							attributes.Clear();
							ParseBlock(indent, function->body, parameter);
						}
						goto label_parse;
					}
				}
			}
		}
	}
	DISCARD_ATTRIBUTE;
}

void FileSpace::ParseInterface(const Line& line, uint32 index, Visibility visibility, List<Anchor>& attributes, ParseParameter* parameter)
{
	Lexical lexical;
	if (!TryGetNextLexical(line, index, LexicalType::Word, MessageType::ERROR_MISSING_NAME, lexical, parameter->messages))return;
	FileInterface* fileInterface = new (interfaces.Add())FileInterface(lexical.anchor, visibility, this);
	fileInterface->attributes.Add(attributes);
	attributes.Clear();

	List<Anchor> names = List<Anchor>(0);
	index = lexical.anchor.GetEnd();
lable_parse_inherits:
	names.Clear();
	if (TryExtractName(line, index, index, &names, parameter->messages))
	{
		new (fileInterface->inherits.Add())FileType(names, ExtractDimension(line, index));
		goto lable_parse_inherits;
	}
	else CheckLineEnd(line, index, parameter->messages);

	uint32 indent = INVALID;
	while (parameter->reader->ReadLine())
	{
		Line current = parameter->reader->CurrentLine();
		if (!CheckIndent(current, indent, line.indent))break;

		if (TryParseAttributes(current, attributes, parameter->messages))continue;
		Anchor name; List<FileParameter> parameters = List<FileParameter>(0); List<FileType> returns = List<FileType>(0);
		ParseFunctionDeclaration(current, 0, name, false, parameters, returns, parameter->messages);
		(new (fileInterface->functions.Add())FileInterface::Function(name, parameters, returns))->attributes.Add(attributes);
		attributes.Clear();
	}
	DISCARD_ATTRIBUTE;
}
