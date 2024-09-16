#pragma once
#include "../Anchor.h"
#include "../../Collections/Dictionary.h"
#include "../../Collections/List.h"
#include "../../Type.h"
#include "../CompilingDeclaration.h"

struct Local
{
	Anchor anchor;
	uint32 index;
	Type type;
	inline Local() :anchor(), index(INVALID), type() {}
	inline Local(const Anchor& anchor, uint32 index, const Type& type) : anchor(anchor), index(index), type(type) {}
	inline CompilingDeclaration GetDeclaration()
	{
		return CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LocalVariable, index, NULL);
	}
};
struct CompilingClass;
struct DeclarationManager;
struct Context;
class ClosureVariable
{
	DeclarationManager* manager;
	uint32 index;
	CompilingClass* closure;
	Dictionary<uint32, uint32, true> variables;// LocalIndex => MemberIndex
public:
	ClosureVariable(DeclarationManager* manager, Context& context, uint32 index);
	inline uint32 LocalIndex() const { return index; }
	inline CompilingClass* Closure() const { return closure; }
	inline bool IsClosureVariable(uint32 localIndex) const { return variables.Contains(localIndex); }
	CompilingDeclaration GetClosureDeclaration(const Local& local);
};
class MessageCollector;
class LocalContext
{
	MessageCollector* messages;
	List<Dictionary<String, Local>*, true> localDeclarations;
	Dictionary<uint32, Anchor> localAnchors;
	uint32 index;
	ClosureVariable* closure;
public:
	inline LocalContext(MessageCollector* messages) :messages(messages), localDeclarations(1), localAnchors(0), index(0), closure(NULL)
	{
		localDeclarations.Add(new Dictionary<String, Local>(0));
	}
	void PushBlock();
	void PopBlock();
	Local AddLocal(const String& name, const Anchor& anchor, const Type& type);
	inline Local AddLocal(const Anchor& anchor, const Type& type)
	{
		return AddLocal(anchor.content, anchor, type);
	}
	bool TryGetLocal(const String& name, Local& local);
	Local GetLocal(uint32 index);
	inline const Dictionary<uint32, Anchor>* GetLocalAnchors() const { return &localAnchors; }

	inline const ClosureVariable* GetClosure() const { return closure; }
	inline bool IsClosureVariable(uint32 localIndex) const { return closure && closure->IsClosureVariable(localIndex); }
	CompilingDeclaration MakeClosure(DeclarationManager* manager, Context& context, uint32 localIndex);

	void Reset();
	~LocalContext();
};

