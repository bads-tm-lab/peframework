// ARM/ARM64 implementation of function registry entries.

#ifndef _PELOADER_FUNCTIONS_REGISTRY_ARM_
#define _PELOADER_FUNCTIONS_REGISTRY_ARM_

#include "peloader.h"

namespace PEFileDetails
{

struct PERuntimeFunctionARM
{
    PEFile::PESectionDataReference beginAddr;
    std::uint32_t prologLength : 8;
    std::uint32_t functionLength : 22;
    std::uint32_t is32Bit : 1;
    std::uint32_t hasExceptionHandler : 1;
};

}

#endif //_PELOADER_FUNCTIONS_REGISTRY_ARM_