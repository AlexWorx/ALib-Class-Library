#if ALIB_DEBUG_BOXING
#   include "ALib.Lang.CIFunctions.H"

namespace alib::boxing::debug {

void  DbgRegisterVTable( detail::VTable* vtable, detail::VTable::DbgFactoryType productionType ) {
    vtable->DbgProduction= productionType;
    DbgLockMaps(true);
        if( !vtable->IsArray() ) {
            #if ALIB_MONOMEM
                ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                DbgKnownVTables.InsertUnique( std::make_pair( &vtable->Type, vtable ) );
            #else
                if ( DbgKnownVTables.find( &vtable->Type ) != DbgKnownVTables.end() ) {
                    ALIB_ERROR( "BOXING", "Double instantiation of VTable of Type: \"{}\"",
                                          vtable->Type )
                    DbgLockMaps(false);
                    return;
                }

                DbgKnownVTables.insert(std::make_pair( &vtable->Type, vtable ) );
            #endif
        } else {
            #if ALIB_MONOMEM
                ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                DbgKnownVTablesArray.InsertUnique(std::make_pair( &vtable->ElementType, vtable ) );
            #else
                if ( DbgKnownVTablesArray.find( &vtable->ElementType ) != DbgKnownVTablesArray.end() ) {
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
AString&     RemoveNamespaces( AString& string, integer startIndex  ) {
    for( auto& search: RemovableNamespaces ) {
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

void  typeName( const detail::VTable* vtable, AString& result ) {
    auto startLength= result.Length();
    if( !vtable->IsArray() )
        result <<  vtable->Type;
    else
        result <<  vtable->ElementType << "[]";

    // MSC adds "class "
    if( result.StartsWith( A_CHAR("class ") ) )
        result.DeleteStart( 6 );

    RemoveNamespaces(result, startLength);
}
#endif // ALIB_STRINGS


} // namespace [alib::boxing::debug]

#   include "ALib.Lang.CIMethods.H"

#endif // ALIB_DEBUG_BOXING
