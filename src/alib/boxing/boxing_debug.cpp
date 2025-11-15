//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/boxing/boxing.prepro.hpp"
#if ALIB_DEBUG_BOXING
#   if !ALIB_MONOMEM
#       include <unordered_map>
#       include <unordered_set>
#   endif
#   include <vector>
#   include <algorithm>
#endif // ALIB_DEBUG_BOXING

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Boxing;
    import   ALib.Lang;
#  if ALIB_MONOMEM
    import   ALib.Monomem;
#  endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Monomem.H"
#   include "ALib.Boxing.H"
#endif
//========================================== Implementation ========================================
#if ALIB_DEBUG_BOXING
#   include "ALib.Lang.CIFunctions.H"

namespace alib::boxing::debug {

void  DbgRegisterVTable( detail::VTable* vtable, detail::VTable::DbgFactoryType productionType )
{
    vtable->DbgProduction= productionType;
    DbgLockMaps(true);
        if( !vtable->IsArray() )
        {
            #if ALIB_MONOMEM
                ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                DbgKnownVTables.InsertUnique( std::make_pair( &vtable->Type, vtable ) );
            #else
                if ( DbgKnownVTables.find( &vtable->Type ) != DbgKnownVTables.end() )
                {
                    ALIB_ERROR( "BOXING", "Double instantiation of VTable of Type: \"{}\"",
                                          vtable->Type )
                    DbgLockMaps(false);
                    return;
                }

                DbgKnownVTables.insert(std::make_pair( &vtable->Type, vtable ) );
            #endif
        }
        else
        {
            #if ALIB_MONOMEM
                ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                DbgKnownVTablesArray.InsertUnique(std::make_pair( &vtable->ElementType, vtable ) );
            #else
                if ( DbgKnownVTablesArray.find( &vtable->ElementType ) != DbgKnownVTablesArray.end() )
                {
                    ALIB_ERROR( "BOXING", "Double instantiation of VTable of Type: \"{}[]\"",
                                          vtable->ElementType )
                    DbgLockMaps(false);
                    return;
                }

                DbgKnownVTablesArray.insert(std::make_pair( &vtable->ElementType, vtable ) );
            #endif
        }
    DbgLockMaps(false);
}


#if ALIB_STRINGS
AString&     removeNamespaces( AString& string, integer startIndex  )
{
    for( auto& search: RemovableNamespaces )
    {
        integer idx;
        while( (idx= string.IndexOf(search, startIndex) ) >= 0 )
            string.Delete( idx, search.Length() );
    }
    return string;
}

std::vector<alib::String>   RemovableNamespaces
{
    A_CHAR( "alib::boxing::" ),
    A_CHAR( "alib::"         ),
};

void  typeName( const detail::VTable* vtable, AString& result )
{
    auto startLength= result.Length();
    if( !vtable->IsArray() )
        result <<  vtable->Type;
    else
        result <<  vtable->ElementType << "[]";

    // MSC adds "class "
    if( result.StartsWith( A_CHAR("class ") ) )
        result.DeleteStart( 6 );

    removeNamespaces(result, startLength);
}
#endif // ALIB_STRINGS


} // namespace [alib::boxing::debug]

#   include "ALib.Lang.CIMethods.H"

#endif // ALIB_DEBUG_BOXING
