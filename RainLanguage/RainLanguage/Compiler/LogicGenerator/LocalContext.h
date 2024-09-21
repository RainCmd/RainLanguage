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
struct AbstractClass;
struct DeclarationManager;
struct Context;
class LocalContext;
struct ClosureMemberVariable
{
	uint32 local;
	uint32 member;
	ClosureMemberVariable(uint32 local, uint32 member) :local(local), member(member) {}
};
class ClosureVariable
{
	LocalContext* localContent;
	DeclarationManager* manager;
	bool hold;
	uint32 id;
	uint32 localIndex;
	uint32 prevMember;
	Dictionary<uint32, List<uint32, true>> paths;// curr => prev
	CompilingClass* compiling;
	AbstractClass* abstract;
	void Init(Context& context);
	void MakeClosure(Context& context, const Local& local, uint32 deep, List<uint32, true>& path);
public:
	List<ClosureMemberVariable, true> variables;
	ClosureVariable* prevClosure;
	inline ClosureVariable(LocalContext* localContent, DeclarationManager* manager, uint32 id, ClosureVariable* prevClosure)
		:localContent(localContent), manager(manager), hold(false), id(id), localIndex(INVALID), prevMember(INVALID), paths(0), compiling(NULL), abstract(NULL), variables(0), prevClosure(prevClosure)
	{
	}
	inline bool Inited() const { return localIndex != INVALID; }
	inline bool Hold() const { return hold || paths.Count(); }
	inline uint32 ID() const { return id; }
	inline uint32 LocalIndex() const
	{
		ASSERT_DEBUG(Inited(), "未初始化");
		return localIndex;
	}
	inline CompilingClass* Compiling() const { return compiling; }
	inline AbstractClass* Abstract() const { return abstract; }
	inline List<uint32, true> GetPath(uint32 local) const
	{
		List<uint32, true> path(0);
		if(paths.TryGet(local, path)) return path;
		EXCEPTION("局部变量访问路径不存在");
	}
	void MakeClosure(Context& context, const Local& local, uint32 deep);
};
struct CaptureInfo
{
	uint32 closure;//存局部变量的那个闭包对象的局部变量id
	uint32 member;//存局部变量的那个成员字段索引
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
	List<ClosureVariable*, true> closures;// id => closure
public:
	Dictionary<uint32, CaptureInfo, true> captures;// localIndex => captureInfo
	inline LocalContext(DeclarationManager* manager, ClosureVariable* prevClosure) :manager(manager), localDeclarations(1), localAnchors(0), index(0), closureStack(0), closures(0), captures(0)
	{
		PushBlock(prevClosure);
	}
	void PushBlock(ClosureVariable* prevClosure);
	inline void PushBlock() { PushBlock(NULL); }
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
	inline ClosureVariable* CurrentClosure() { return closureStack.Peek(); }
	inline ClosureVariable* GetClosure(uint32 closureIndex) { return closures[closureIndex]; }
	inline bool TryGetCaptureInfo(uint32 localIndex, CaptureInfo& capture) const { return captures.TryGet(localIndex, capture); }
	CompilingDeclaration MakeClosure(Context& context, const Local& local, uint32 deep);

	void Reset(bool deleteClosureDeclaration);
	~LocalContext();
};

