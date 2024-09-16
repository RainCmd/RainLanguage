#include "_LambdaClosure.h"
#include "ExpressionParser.h"

CompilingClass* LambdaClosure::GetClosure()
{
	if (!closure)
	{
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_SELF, Visibility::Space, DeclarationCategory::Class, environment->manager->compilingLibrary.classes.Count(), 0);
		environment->manager->compilingLibrary.classes.Add(closure = new CompilingClass(Anchor(), declaration, List<Anchor>(0), environment->context.compilingSpace, 0, List<CompilingClass::Constructor*, true>(0), List<CompilingClass::Variable*, true>(0), List<uint32, true>(0), List<Line>(0)));
		closure->parent = TYPE_Handle;
		closure->relies = environment->context.relies;
		environment->manager->selfLibaray->classes.Add(new AbstractClass(closure->name.content, declaration, List<String>(0), environment->context.compilingSpace->abstract, TYPE_Handle, List<Type, true>(0), List<uint32, true>(0), List<AbstractVariable*, true>(0), List<uint32, true>(1), (uint32)0, (uint8)0));
	}
	return closure;
}

CompilingDeclaration LambdaClosure::Convert(const Anchor& name, const CompilingDeclaration& declaration)
{
	CompilingDeclaration result;
	if (map.TryGet(declaration, result)) return result;
	CompilingClass* compilingClosure = GetClosure();
	AbstractClass* abstractClosure = environment->manager->selfLibaray->classes[compilingClosure->declaration.index];
	result = CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LambdaClosureValue, compilingClosure->variables.Count(), compilingClosure->declaration.index);
	CompilingClass::Variable* variable = new CompilingClass::Variable(name, result, List<Anchor>(0), Anchor());
	compilingClosure->variables.Add(variable);
	variable->type = environment->GetVariableType(declaration);
	uint8 alignment;
	uint32 size = environment->manager->GetStackSize(variable->type, alignment);
	abstractClosure->size = MemoryAlignment(abstractClosure->size, alignment);
	abstractClosure->variables.Add(new AbstractVariable(name.content, result, List<String>(0), compilingClosure->space->abstract, false, variable->type, abstractClosure->size));
	abstractClosure->size += size;
	if (alignment > abstractClosure->alignment)abstractClosure->alignment = alignment;
	map.Set(declaration, result);
	return result;
}

List<CompilingDeclaration, true> LambdaClosure::GetClosureVariables(uint32 functionIndex)
{
	closure->functions.Add(functionIndex);
	List<CompilingDeclaration, true> sourceVariables(closure->variables.Count());
	sourceVariables.SetCount(closure->variables.Count());
	Dictionary<CompilingDeclaration, CompilingDeclaration, true>::Iterator iterator = map.GetIterator();
	while (iterator.Next()) sourceVariables[iterator.CurrentValue().index] = iterator.CurrentKey();
	return sourceVariables;
}

Type LambdaClosure::GetVariableType(const CompilingDeclaration& declaration)
{
	ASSERT_DEBUG(declaration.definition == closure->declaration.index, "不是该闭包的定义");
	return closure->variables[declaration.index]->type;
}

bool LambdaClosure::TryFindDeclaration(const Anchor& name, List<CompilingDeclaration, true>& result)
{
	if (environment->TryFindDeclaration(name, result))
	{
		if (result.Count() == 1 && (result.Peek().category == DeclarationCategory::LocalVariable || result.Peek().category == DeclarationCategory::LambdaClosureValue))
			result.Peek() = Convert(name, result.Peek());
		return true;
	}
	return false;
}

bool LambdaClosure::TryGetThisValueDeclaration(CompilingDeclaration& declaration)
{
	if (environment->TryGetThisValueDeclaration(declaration))
	{
		declaration = Convert(Anchor(), declaration);
		return true;
	}
	return false;
}
