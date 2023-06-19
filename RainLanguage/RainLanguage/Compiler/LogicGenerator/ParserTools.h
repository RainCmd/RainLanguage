#pragma once
#include "../../RainLanguage.h"
#include "../../Type.h"
class TupleExpression;
class Expression;
struct DeclarationManager;

TupleExpression* Combine(const List<Expression*, true>& expressions);
bool TryConvert(DeclarationManager* manager, const Type& source, const Type& target, bool& convert, uint32& measure);
bool TryConvert(DeclarationManager* manager, const Span<Type, true>& source, const Span<Type, true>& target, uint32& measure);
bool CheckEquals(DeclarationManager* manager, const Type& left, const Type& right);