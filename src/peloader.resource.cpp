// Resource helper API.
#include "peloader.h"

#include <sdk/UniChar.h>

#include <sdk/NumericFormat.h>

// Generic finder routine.
PEFile::PEResourceItem* PEFile::PEResourceDir::FindItem( bool isIdentifierName, const peString <char16_t>& name, std::uint16_t identifier )
{
    if ( isIdentifierName )
    {
        auto *findIter = this->idChildren.Find( identifier );

        if ( findIter != nullptr )
        {
            return findIter->GetValue();
        }
    }
    else
    {
        auto *findIter = this->namedChildren.Find( name );

        if ( findIter != nullptr )
        {
            return findIter->GetValue();
        }
    }

    return nullptr;
}

peString <wchar_t> PEFile::PEResourceItem::GetName( void ) const
{
    if ( !this->hasIdentifierName )
    {
        return CharacterUtil::ConvertStrings <char16_t, wchar_t, PEGlobalStaticAllocator> ( this->name );
    }
    else
    {
        peString <wchar_t> charBuild( L"(ident:" );

        charBuild += eir::to_string <wchar_t, PEGlobalStaticAllocator> ( this->identifier );

        charBuild += L")";

        return charBuild;
    }
}

bool PEFile::PEResourceDir::AddItem( PEFile::PEResourceItem *theItem )
{
    if ( theItem->hasIdentifierName )
    {
        this->idChildren.Insert( theItem );
    }
    else
    {
        this->namedChildren.Insert( theItem );
    }

    return true;
}

bool PEFile::PEResourceDir::RemoveItem( const PEFile::PEResourceItem *theItem )
{
    if ( theItem->hasIdentifierName )
    {
        auto *findIter = this->idChildren.Find( theItem );

        if ( findIter != nullptr )
        {
            this->idChildren.RemoveNode( findIter );
            
            return true;
        }
    }
    else
    {
        auto *findIter = this->namedChildren.Find( theItem );

        if ( findIter != nullptr )
        {
            this->namedChildren.RemoveNode( findIter );

            return true;
        }
    }

    return false;
}