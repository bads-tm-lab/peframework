// Resource helper API.
#include "peloader.h"

#include <sdk/UniChar.h>

// Generic finder routine.
PEFile::PEResourceItem* PEFile::PEResourceDir::FindItem( bool isIdentifierName, const std::u16string& name, std::uint16_t identifier )
{
    for ( PEResourceItem *resItem : this->children )
    {
        if ( resItem->hasIdentifierName != isIdentifierName )
            continue;

        if ( !isIdentifierName )
        {
            if ( resItem->name == name )
                return resItem;
        }
        else
        {
            if ( resItem->identifier == identifier )
                return resItem;
        }
    }

    return NULL;
}

std::wstring PEFile::PEResourceItem::GetName( void ) const
{
    if ( !this->hasIdentifierName )
    {
        return CharacterUtil::ConvertStrings <char16_t, wchar_t> ( this->name );
    }
    else
    {
        std::wstring charBuild( L"(ident:" );

        charBuild += std::to_wstring( this->identifier );

        charBuild += L")";

        return charBuild;
    }
}

bool PEFile::PEResourceDir::RemoveItem( const PEFile::PEResourceItem *theItem )
{
    auto findIter = std::find( this->children.begin(), this->children.end(), theItem );

    if ( findIter == this->children.end() )
        return false;

    this->children.erase( findIter );

    return true;
}