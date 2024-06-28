#include "ClearStaticCache.h"
#include "Rain.h"

void ClearStaticCache()
{
	ClearKernelLibrary();
	ClearKernelLibraryInfo();
	ClearKeyWorlds();
}