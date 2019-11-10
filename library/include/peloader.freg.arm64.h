// ARM64 implementation of function registry entries.

#ifndef _PELOADER_FUNCTIONS_REGISTRY_ARM64_
#define _PELOADER_FUNCTIONS_REGISTRY_ARM64_

#include "peloader.h"

namespace PEFileDetails
{

struct PERuntimeFunctionARM64
{
    PEFile::PESectionDataReference BeginAddress;
    union
    {
        struct
        {
            std::uint32_t flag : 2;
            std::uint32_t funcLen : 11;
            std::uint32_t regF : 3;
            std::uint32_t regI : 4;
            std::uint32_t H : 1;
            std::uint32_t CR : 2;
            std::uint32_t frameSize : 9;
        };
        std::uint32_t UnwindData;
    };
};

struct PEFunctionRegistryARM64 : public PEFile::PEDataDirectoryGeneric
{
    inline PEFunctionRegistryARM64( void ) = default;
    inline PEFunctionRegistryARM64( const PEFunctionRegistryARM64& ) = delete;
    inline PEFunctionRegistryARM64( PEFunctionRegistryARM64&& ) = default;

    void SerializeDataDirectory( PEFile::PESection *targetSect, std::uint64_t peImageBase ) override
    {
        const auto& exceptRFs = this->entries;

        std::uint32_t numExceptEntries = (std::uint32_t)exceptRFs.GetCount();

        const std::uint32_t exceptTableSize = ( sizeof(PEStructures::IMAGE_RUNTIME_FUNCTION_ENTRY_ARM64) * numExceptEntries );

        if ( numExceptEntries != 0 )
        {
            PEFile::PESectionAllocation exceptTableAlloc;
            targetSect->Allocate( exceptTableAlloc, exceptTableSize, sizeof(std::uint32_t) );

            for ( std::uint32_t n = 0; n < numExceptEntries; n++ )
            {
                const PEFileDetails::PERuntimeFunctionARM64& rfEntry = entries[ n ];

                PEStructures::IMAGE_RUNTIME_FUNCTION_ENTRY_ARM64 info;
                info.BeginAddress = rfEntry.BeginAddress.GetRVA();
                info.UnwindData = rfEntry.UnwindData;

                const std::uint32_t rfEntryOff = ( n * sizeof(PEStructures::IMAGE_RUNTIME_FUNCTION_ENTRY_ARM64) );

                exceptTableAlloc.WriteToSection( &info, sizeof(info), rfEntryOff );
            }

            this->allocEntry = std::move( exceptTableAlloc );
        }
    }

    peVector <PEFileDetails::PERuntimeFunctionARM64> entries;
};

};

#endif //_PELOADER_FUNCTIONS_REGISTRY_ARM64_