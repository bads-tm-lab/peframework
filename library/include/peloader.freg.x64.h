// X64 and Itanium implementation of runtime functions registry.

#ifndef _PELOADER_FUNCTIONS_REGISTRY_X64_
#define _PELOADER_FUNCTIONS_REGISTRY_X64_

#include "peloader.h"

namespace PEFileDetails
{

struct PERuntimeFunctionX64
{
    PEFile::PESectionDataReference beginAddrRef;
    PEFile::PESectionDataReference endAddrRef;
    PEFile::PESectionDataReference unwindInfoRef;
};

}

#endif //_PELOADER_FUNCTIONS_REGISTRY_X64_