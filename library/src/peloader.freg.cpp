#include "peframework.h"

#include "peloader.serialize.h"

#include "peloader.datadirs.hxx"

// Include all possible implementations.
#include "peloader.freg.x64.h"
#include "peloader.freg.arm.h"
#include "peloader.freg.mips32.h"

struct PEFunctionRegistryX64 : public PEFile::PEDataDirectoryGeneric
{
    inline PEFunctionRegistryX64( void ) = default;
    inline PEFunctionRegistryX64( const PEFunctionRegistryX64& ) = delete;
    inline PEFunctionRegistryX64( PEFunctionRegistryX64&& ) = default;

    void SerializeDataDirectory( PEFile::PESection *targetSect ) override
    {
        // TODO: remember that exception data is machine dependent.
        // revisit this if we need multi-architecture support.
        // (currently we specialize on x86/AMD64)

        const auto& exceptRFs = this->entries;

        std::uint32_t numExceptEntries = (std::uint32_t)exceptRFs.GetCount();

        const std::uint32_t exceptTableSize = ( sizeof(PEStructures::IMAGE_RUNTIME_FUNCTION_ENTRY_X64) * numExceptEntries );

        if ( numExceptEntries != 0 )
        {
            PEFile::PESectionAllocation exceptTableAlloc;
            targetSect->Allocate( exceptTableAlloc, exceptTableSize, sizeof(std::uint32_t) );

            // Now write all entries.
            // TODO: documentation says that these entries should be address sorted.
            for ( std::uint32_t n = 0; n < numExceptEntries; n++ )
            {
                const PEFileDetails::PERuntimeFunctionX64& rfEntry = exceptRFs[ n ];

                PEStructures::IMAGE_RUNTIME_FUNCTION_ENTRY_X64 funcInfo;
                funcInfo.BeginAddress = rfEntry.beginAddrRef.GetRVA();
                funcInfo.EndAddress = rfEntry.endAddrRef.GetRVA();
                funcInfo.UnwindInfoAddress = rfEntry.unwindInfoRef.GetRVA();

                const std::uint32_t rfEntryOff = ( n * sizeof(PEStructures::IMAGE_RUNTIME_FUNCTION_ENTRY_X64) );

                exceptTableAlloc.WriteToSection( &funcInfo, sizeof(funcInfo), rfEntryOff );
            }

            // Remember this valid exception table.
            this->allocEntry = std::move( exceptTableAlloc );
        }
    }

    void* GetDirectoryData( void ) override
    {
        return this->entries.GetData();
    }

    size_t GetDirectoryEntrySize( void ) const override
    {
        return sizeof(PEFileDetails::PERuntimeFunctionX64);
    }

    size_t GetDirectoryEntryCount( void ) const override
    {
        return this->entries.GetCount();
    }

    peVector <PEFileDetails::PERuntimeFunctionX64> entries;
};

struct PEFunctionRegistryDataDirectoryParser : public PEFile::PEDataDirectoryParser
{
    PEFile::PEDataDirectoryGeneric* DeserializeData( std::uint16_t machine_id, PEFile::PESectionMan& sections, PEFile::PEDataStream stream, std::uint32_t va, std::uint32_t vsize ) const override
    {
        if ( machine_id == PEL_IMAGE_FILE_MACHINE_ARM || machine_id == PEL_IMAGE_FILE_MACHINE_ARMNT ||
             machine_id == PEL_IMAGE_FILE_MACHINE_POWERPC ||
             machine_id == PEL_IMAGE_FILE_MACHINE_SH3 || machine_id == PEL_IMAGE_FILE_MACHINE_SH3DSP || machine_id == PEL_IMAGE_FILE_MACHINE_SH3E ||
             machine_id == PEL_IMAGE_FILE_MACHINE_SH4 )
        {
            
        }
        else if ( machine_id == PEL_IMAGE_FILE_MACHINE_AMD64 || machine_id == PEL_IMAGE_FILE_MACHINE_IA64 )
        {
            PEFunctionRegistryX64 exceptRFs;

            const std::uint32_t numFuncs = ( vsize / sizeof( PEStructures::IMAGE_RUNTIME_FUNCTION_ENTRY_X64 ) );

            for ( size_t n = 0; n < numFuncs; n++ )
            {
                PEStructures::IMAGE_RUNTIME_FUNCTION_ENTRY_X64 func;
                stream.Read( &func, sizeof(func) );

                // Since the runtime function entry stores RVAs, we want to remember them
                // relocation independent.
                PEFile::PESection *beginAddrSect = nullptr;
                std::uint32_t beginAddrSectOff = 0;

                if ( std::uint32_t BeginAddress = func.BeginAddress )
                {
                    bool gotLocation = sections.GetPEDataLocation( BeginAddress, &beginAddrSectOff, &beginAddrSect );

                    if ( !gotLocation )
                    {
                        throw peframework_exception(
                            ePEExceptCode::CORRUPT_PE_STRUCTURE,
                            "invalid PE runtime function begin address"
                        );
                    }
                }
                PEFile::PESection *endAddrSect = nullptr;
                std::uint32_t endAddrSectOff = 0;

                if ( std::uint32_t EndAddress = func.EndAddress )
                {
                    bool gotLocation = sections.GetPEDataLocation( EndAddress, &endAddrSectOff, &endAddrSect );

                    if ( !gotLocation )
                    {
                        throw peframework_exception(
                            ePEExceptCode::CORRUPT_PE_STRUCTURE,
                            "invalid PE runtime function end address"
                        );
                    }
                }
                PEFile::PESection *unwindInfoSect = nullptr;
                std::uint32_t unwindInfoSectOff = 0;

                if ( std::uint32_t UnwindInfoAddress = func.UnwindInfoAddress )
                {
                    bool gotLocation = sections.GetPEDataLocation( UnwindInfoAddress, &unwindInfoSectOff, &unwindInfoSect );

                    if ( !gotLocation )
                    {
                        throw peframework_exception(
                            ePEExceptCode::CORRUPT_PE_STRUCTURE,
                            "invalid PE runtime function unwind info address"
                        );
                    }
                }

                PEFileDetails::PERuntimeFunctionX64 funcInfo;
                funcInfo.beginAddrRef = PEFile::PESectionDataReference( beginAddrSect, beginAddrSectOff );
                funcInfo.endAddrRef = PEFile::PESectionDataReference( endAddrSect, endAddrSectOff );
                funcInfo.unwindInfoRef = PEFile::PESectionDataReference( unwindInfoSect, unwindInfoSectOff );

                exceptRFs.entries.AddToBack( std::move( funcInfo ) );
            }

            return eir::static_new_struct <PEFunctionRegistryX64, PEGlobalStaticAllocator> ( nullptr, std::move( exceptRFs ) );
        }
        else if ( machine_id == PEL_IMAGE_FILE_MACHINE_MIPSFPU )
        {

        }

        // Unknown machine type.
        return nullptr;
    }
};

static optional_struct_space <PEFunctionRegistryDataDirectoryParser> parser;

// We need one static component called the data directory extension registry that manages all such extensions as this and
// stores their handlers inside.

void registerRuntimeFunctionParser( void )
{
    parser.Construct();

    registerDataDirectoryParser( PEL_IMAGE_DIRECTORY_ENTRY_EXCEPTION, &parser.get() );
}

void unregisterRuntimeFunctionParser( void )
{
    unregisterDataDirectoryParser( PEL_IMAGE_DIRECTORY_ENTRY_EXCEPTION );

    parser.Destroy();
}