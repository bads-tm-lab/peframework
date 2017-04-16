// Helper API for managing exports.
#include "peloader.h"

#include "peexcept.h"

std::uint32_t PEFile::PEExportDir::AddExport( func&& entry )
{
    size_t currentIndex = this->functions.size();

    this->functions.push_back( std::move( entry ) );

    // We need to rewrite stuff.
    this->allocEntry = PESectionAllocation();
    this->funcAddressAllocEntry = PESectionAllocation();

    return (std::uint32_t)currentIndex;
}

void PEFile::PEExportDir::MapName( std::uint32_t ordinal, const char *name )
{
    mappedName newNameMap;
    newNameMap.name = name;

    this->funcNameMap.insert( std::make_pair( std::move( newNameMap ), ordinal ) );

    // Need to recommit memory.
    this->allocEntry = PESectionAllocation();
    this->funcNamesAllocEntry = PESectionAllocation();
}

void PEFile::PEExportDir::RemoveExport( std::uint32_t ordinal )
{
    size_t curNumFunctions = this->functions.size();

    if ( ordinal >= curNumFunctions )
    {
        throw peframework_exception( ePEExceptCode::RUNTIME_ERROR, "ordinal out of bounds for removing export" );
    }

    // Simply reset the function field.
    {
        func& expEntry = this->functions[ ordinal ];
        expEntry.isForwarder = false;
        expEntry.expRef = PESectionDataReference();
        expEntry.forwarder.clear();
    }

    // Remove all name mappings of this ordinal.
    {
        auto iter = this->funcNameMap.begin();

        while ( iter != this->funcNameMap.end() )
        {
            if ( iter->second == ordinal )
            {
                iter = this->funcNameMap.erase( iter );
            }
        }
    }
}