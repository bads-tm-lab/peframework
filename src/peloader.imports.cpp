#include "peloader.h"

PEFile::PEImportDesc* PEFile::FindImportDescriptor( const char *moduleName )
{
    for ( PEImportDesc& impDesc : this->imports )
    {
        if ( stricmp( moduleName, impDesc.DLLName.c_str() ) == 0 )
        {
            return &impDesc;
        }
    }

    return NULL;
}

PEFile::PEImportDesc& PEFile::EstablishImportDescriptor( const char *moduleName )
{
    if ( PEImportDesc *impDesc = this->FindImportDescriptor( moduleName ) )
    {
        return *impDesc;
    }

    this->imports.emplace_back( moduleName );

    // Need new native array.
    this->importsAllocEntry = PESectionAllocation();
    
    return this->imports.back();
}

const PEFile::PEImportDesc::importFunc* PEFile::PEImportDesc::FindImportEntry( std::uint16_t ordinal_hint, const char *name, bool isOrdinalImport, std::uint32_t *indexOut ) const
{
    std::uint32_t index = 0;
    const PEImportDesc::importFunc *funcOut = NULL;

    for ( const PEImportDesc::importFunc& impFunc : this->funcs )
    {
        if ( isOrdinalImport )
        {
            if ( impFunc.isOrdinalImport == true )
            {
                if ( impFunc.ordinal_hint == ordinal_hint )
                {
                    funcOut = &impFunc;
                    break;
                }
            }
        }
        else
        {
            if ( impFunc.isOrdinalImport == false )
            {
                if ( impFunc.name == name )
                {
                    funcOut = &impFunc;
                    break;
                }
            }
        }

        index++;
    }

    if ( funcOut )
    {
        if ( indexOut )
        {
            *indexOut = index;
        }
    }

    return funcOut;
}