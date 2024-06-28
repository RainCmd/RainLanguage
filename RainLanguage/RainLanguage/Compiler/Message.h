#pragma once
#include "../Language.h"
#include "BuilderDefinitions.h"
#include "Line.h"
#include "Anchor.h"

inline ErrorLevel GetLevel(MessageType type) { return (ErrorLevel)((uint32)type >> 24); }

const struct Message
{
	String source;
	MessageType type;
	uint32 line, start, length;
	String message;
	inline ErrorLevel GetLevel() { return ::GetLevel(type); }
	inline Message(const String& source, MessageType type, uint32 line, uint32 start, uint32 length, String message) :source(source), type(type), line(line), start(start), length(length), message(message) {}
};

class MessageCollector
{
private:
	StringAgency stringAgency;
	List<Message> messages[16];
	ErrorLevel level;
public:
	inline ErrorLevel GetLevel() const { return level; }
	inline MessageCollector(ErrorLevel level) :stringAgency(512), messages{ List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0), List<Message>(0) }, level(level) {}
	inline List<Message>* GetMessages(ErrorLevel errorLevel) { return messages + (uint32)errorLevel; }
	inline const List<Message>* GetMessages(ErrorLevel errorLevel) const { return messages + (uint32)errorLevel; }
	inline void Add(const String& source, MessageType type, uint32 line, uint32 start, uint32 length, const character* message)
	{
		if (type != MessageType::INVALID) new (GetMessages(::GetLevel(type))->Add())Message(stringAgency.Add(source), type, line, start, length, stringAgency.Add(message));
	}
	inline void Add(const String& source, MessageType type, uint32 line, uint32 start, uint32 length, const String& message)
	{
		if (type != MessageType::INVALID) new (GetMessages(::GetLevel(type))->Add())Message(stringAgency.Add(source), type, line, start, length, stringAgency.Add(message));
	}
	inline void Add(const String& source, MessageType type, uint32 line, uint32 start, uint32 length)
	{
		Add(source, type, line, start, length, String());
	}
	inline void Add(const Anchor& anchor, MessageType type, const character* message)
	{
		Add(anchor.source, type, anchor.line, anchor.position, anchor.content.GetLength(), message);
	}
	inline void Add(const Anchor& anchor, MessageType type, const String& message)
	{
		Add(anchor.source, type, anchor.line, anchor.position, anchor.content.GetLength(), message);
	}
	inline void Add(const List<Anchor>& anchors, MessageType type, const String& message)
	{
		if (anchors.Count())Add(anchors.Peek(), type, message);
	}
	inline void Add(const Anchor& anchor, MessageType type)
	{
		Add(anchor, type, String());
	}
	inline void Add(const List<Anchor>& anchors, MessageType type)
	{
		Add(anchors, type, String());
	}
	inline void Add(const Line& line, MessageType type, const character* message)
	{
		Add(line.source, type, line.number, 0, line.content.GetLength(), message);
	}
	inline void Add(const Line& line, MessageType type, const String& message)
	{
		Add(line.source, type, line.number, 0, line.content.GetLength(), message);
	}
	inline void Add(const Line& line, MessageType type)
	{
		Add(line, type, String());
	}
};

#define MESSAGE6(collector,source,type,line,start,length,message) {if (GetLevel(type) <= collector->GetLevel())collector->Add(source, type, line, start, length, message);}
#define MESSAGE5(collector,source,type,line,start,length) {if (GetLevel(type) <= collector->GetLevel())collector->Add(source, type, line, start, length);}
#define MESSAGE3(collector,anchor,type,message) {if (GetLevel(type) <= collector->GetLevel())collector->Add(anchor, type, message);}
#define MESSAGE2(collector,anchor,type) {if (GetLevel(type) <= collector->GetLevel())collector->Add(anchor, type);}