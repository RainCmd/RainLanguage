#pragma once
#include "BuilderDefinitions.h"
#include "RainLibrary.h"

const struct RAINLANGUAGE CodeBuffer
{
	const character* source;
	uint32 sourceLength;
	CodeReader reader;
	inline CodeBuffer(const character* source, uint32 sourceLength, CodeReader reader) :source(source), sourceLength(sourceLength), reader(reader) {}
};
typedef CodeBuffer(*CodeLoader)();
const struct RAINLANGUAGE BuildParameter
{
	const character* name;
	bool debug;
	CodeLoader codeLoader;
	LibraryLoader libraryLoader;
	ErrorLevel messageLevel;

	BuildParameter(const character* name, bool debug, const CodeLoader& codeLoader, const LibraryLoader& libraryLoader, const ErrorLevel& messageLevel)
		: name(name), debug(debug), codeLoader(codeLoader), libraryLoader(libraryLoader), messageLevel(messageLevel) {}
};

class ProgramDebugDatabase
{
	//todo µ÷ÊÔÊý¾Ý
};

const class RAINLANGUAGE RainProduct
{
public:
	const struct RAINLANGUAGE ErrorMessage
	{
		const character* source;
		uint32 sourceLength;
		MessageType type;
		uint32 line, start, length;
		const character* message;
		uint32 messageLength;

		inline ErrorMessage(const character* source, const uint32& sourceLength, const MessageType& type, const uint32& line, const uint32& start, const uint32& length, const character* message, const uint32& messageLength)
			: source(source), sourceLength(sourceLength), type(type), line(line), start(start), length(length), message(message), messageLength(messageLength) {}
	};
	RainProduct() = default;
	virtual ~RainProduct() {}

	virtual ErrorLevel GetLevel() = 0;
	virtual uint32 GetLevelMessageCount(ErrorLevel level) = 0;
	virtual const ErrorMessage GetErrorMessage(ErrorLevel level, uint32 index) = 0;
	virtual const RainLibrary* GetLibrary() = 0;
	virtual const ProgramDebugDatabase* GetProgramDebugDatabase() = 0;
};

RAINLANGUAGE RainProduct* Build(const BuildParameter& parameter);