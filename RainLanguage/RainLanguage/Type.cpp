#include "Type.h"

bool IsEquals(const List<Type, true>& sources, uint32 sourceStartIndex, const List<Type, true>& targets, uint32 targetStartIndex)
{
	if (sources.Count() - sourceStartIndex != targets.Count() - targetStartIndex)return false;
	const Type* sourcePointer = sources.GetPointer() + sourceStartIndex, * targetPointer = targets.GetPointer() + targetStartIndex;
	for (uint32 i = 0, count = sources.Count() - sourceStartIndex; i < count; i++)
		if (sourcePointer[i] != targetPointer[i])
			return false;
	return true;
}