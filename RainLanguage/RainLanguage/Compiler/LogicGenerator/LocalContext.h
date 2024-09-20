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
	uint32 id;
	uint32 local;
	uint32 prevMember;
	Dictionary<uint32, List<uint32, true>> paths;// curr => prev
	Dictionary<uint32, uint32, true> variables;// localIndex => memberIndex
	void GetClosureDeclaration(const Local& local, uint32 deep, List<uint32, true>& path);
public:
	CompilingClass* closure;
	ClosureVariable* prevClosure;
	bool hold;
	ClosureVariable(DeclarationManager* manager, Context& context, uint32 id, uint32 local, ClosureVariable* prevClosure);
	inline uint32 ID() const { return id; }
	inline uint32 LocalIndex() const { return local; }
	inline CompilingClass* Closure() const { return closure; }
	inline List<uint32, true> GetPath(uint32 localIndex) const
	{
		List<uint32, true> path(0);
		if(paths.TryGet(localIndex, path)) return path;
		EXCEPTION("局部变量访问路径不存在");
	}
	uint32 GetClosureDeclaration(const Local& local, uint32 deep);
};
struct CaptureInfo
{
	uint32 closure;
	uint32 member;
	CaptureInfo() :closure(INVALID), member(INVALID) {}
	CaptureInfo(uint32 closure, uint32 member) :closure(closure), member(member) {}
};
class MessageCollector;
class LocalContext
{
	DeclarationManager* manager;
	List<Dictionary<String, Local>*, true> localDeclarations;
	Dictionary<uint32, Anchor> localAnchors;
	uint32 index;
	List<ClosureVariable*, true> closureStack;
	List<ClosureVariable*, true> closures;
	Dictionary<uint32, CaptureInfo, true> captures;// localIndex => captureInfo
public:
	inline LocalContext(DeclarationManager* manager, Context& context) :manager(manager), localDeclarations(1), localAnchors(0), index(0), closureStack(0), closures(0), captures(0)
	{
		PushBlock(context);
	}
	inline void PushBlock(Context& context)
	{
		localDeclarations.Add(new Dictionary<String, Local>(0));
		ClosureVariable* prev = NULL;
		if(closureStack.Count()) prev = closureStack.Peek();
		ClosureVariable* closure = new ClosureVariable(manager, context, closures.Count(), index, prev);
		closureStack.Add(closure);
		closures.Add(closure);
		AddLocal(Anchor(), closure->Closure()->declaration.DefineType());
	}
	inline void PopBlock()
	{
		delete localDeclarations.Pop();
		closures.Pop();
	}
	Local AddLocal(const String& name, const Anchor& anchor, const Type& type);
	inline Local AddLocal(const Anchor& anchor, const Type& type)
	{
		return AddLocal(anchor.content, anchor, type);
	}
	bool TryGetLocal(const String& name, Local& local);
	Local GetLocal(uint32 index);
	inline const Dictionary<uint32, Anchor>* GetLocalAnchors() const { return &localAnchors; }

	inline uint32 CurrentDeep() const { return localDeclarations.Count(); }
	bool TryGetLocalAndDeep(const String& name, Local& local, uint32& deep);
	inline const ClosureVariable* GetClosure(uint32 closureIndex) const { return closures[closureIndex]; }
	inline ClosureVariable* GetClosure(uint32 closureIndex) { return closures[closureIndex]; }
	inline bool TryGetCaptureInfo(uint32 localIndex, CaptureInfo& capture) const { return captures.TryGet(localIndex, capture); }
	CompilingDeclaration MakeClosure(LocalContext* context, const Local& local, uint32 deep);

	void Reset(bool deleteClosureDeclaration);
	~LocalContext();
};

