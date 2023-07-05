#include "ClearStaticCache.h"
#include "Public\Rain.h"

void ClearStaticCache()
{
	ClearKernelLibrary();
	ClearKernelLibraryInfo();
	ClearKeyWorlds();
}