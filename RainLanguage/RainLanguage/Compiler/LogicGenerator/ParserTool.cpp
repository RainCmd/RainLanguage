#include "ParserTools.h"
#include "Attribute.h"
#include "Expression.h"
#include "Expressions/TupleExpression.h"

TupleExpression* Combine(const List<Expression*, true>& expressions)
{
	if (expressions.Count())
	{
		Attribute attribute = Attribute::Assignable;
		uint32 returnCount = 0;
		for (uint32 i = 0; i < expressions.Count(); i++)
			returnCount += expressions[i]->returns.Count();
		List<Type, true> returns(returnCount);
		for (uint32 i = 0; i < expressions.Count(); i++)
		{
			returns.Add(expressions[i]->returns);
			attribute &= expressions[i]->attribute;
		}
		TupleExpression* result = new TupleExpression(expressions[0]->anchor, returns, expressions);
		result->attribute = attribute | Attribute::Tuple;
		return result;
	}
	else return GetEmptyTupleExpression();
}

bool TryConvert(DeclarationManager* manager, const Type& source, const Type& target, bool& convert, uint32& measure)
{
	if (source == TYPE_Blurry || target == TYPE_Null) return false;
	else if (source == TYPE_Null)
	{
		convert = false;
		measure = 0;
		return target == TYPE_Entity || IsHandleType(target);
	}
	else if (target == TYPE_Blurry)
	{
		convert = false;
		measure = 0;
		return source != TYPE_Null;
	}
	else if (source == target)
	{
		convert = false;
		measure = 0;
		return true;
	}
	else if (target == TYPE_Char)
	{
		if (source == TYPE_Byte)
		{
			convert = true;
			measure = 0xff;
			return true;
		}
	}
	else if (target == TYPE_Integer)
	{
		if (source == TYPE_Byte || source == TYPE_Char)
		{
			convert = true;
			measure = 0xff;
			return true;
		}
		else if (!source.dimension && source.code == TypeCode::Enum)
		{
			convert = false;
			measure = 0xfff;
			return true;
		}
	}
	else if (target == TYPE_Real)
	{
		if (source == TYPE_Byte || source == TYPE_Char || source == TYPE_Integer)
		{
			convert = true;
			measure = 0xff;
			return true;
		}
	}
	else if (target == TYPE_Real2)
	{
		if (source == TYPE_Real3)
		{
			convert = true;
			measure = 0xff;
			return true;
		}
		else if (source == TYPE_Real4)
		{
			convert = true;
			measure = 0xfff;
			return true;
		}
	}
	else if (target == TYPE_Real3)
	{
		if (source == TYPE_Real2)
		{
			convert = true;
			measure = 0xffff;
			return true;
		}
		else if (source == TYPE_Real4)
		{
			convert = true;
			measure = 0xff;
			return true;
		}
	}
	else if (target == TYPE_Real4)
	{
		if (source == TYPE_Real2)
		{
			convert = true;
			measure = 0xfffff;
			return true;
		}
		else if (source == TYPE_Real3)
		{
			convert = true;
			measure = 0xffff;
			return true;
		}
	}
	else if (manager->TryGetInherit(target, source, measure))
	{
		convert = false;
		return true;
	}
	else if (target == TYPE_Handle)
	{
		convert = true;
		measure = 0xffffff;
		return true;
	}
	return false;
}
bool TryConvert(DeclarationManager* manager, const Span<Type, true>& source, const Span<Type, true>& target, uint32& measure)
{
	if (source.Count() != target.Count())return false;
	bool convert;
	uint32 indexMeasure;
	measure = 0;
	for (uint32 i = 0; i < source.Count(); i++)
		if (TryConvert(manager, source[i], target[i], convert, indexMeasure)) measure += indexMeasure;
		else return false;
	return true;
}

bool CheckEquals(DeclarationManager* manager, const Type& left, const Type& right)
{
	if (left == right) return true;
	else if (left == TYPE_Byte || left == TYPE_Char || left == TYPE_Integer || left == TYPE_Real) return right == TYPE_Byte || right == TYPE_Char || right == TYPE_Integer || right == TYPE_Real;
	else if (left == TYPE_Real2 || left == TYPE_Real3 || left == TYPE_Real4) return right == TYPE_Real2 || right == TYPE_Real3 || right == TYPE_Real4;
	else if (IsHandleType(left) && IsHandleType(right))
	{
		uint32 measure; bool convert;
		return TryConvert(manager, left, right, convert, measure) || TryConvert(manager, right, left, convert, measure);
	}
	return false;
}
