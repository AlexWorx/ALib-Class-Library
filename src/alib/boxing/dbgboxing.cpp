// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALIB_BOXING_BOXING)
#   include "alib/boxing/boxing.hpp"
#endif
#endif // !defined(ALIB_DOX)

#if ALIB_DEBUG_BOXING

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_BOXING_DBGBOXING)
#      include "alib/boxing/dbgboxing.hpp"
#   endif
#   if !defined(HPP_ALIB_COMPATIBILITY_STD_TYPEINFO)
#      include "alib/compatibility/std_typeinfo.hpp"
#   endif

#   if ALIB_THREADS && !defined(HPP_ALIB_THREADS_THREADLOCK)
#      include "alib/threads/threadlock.hpp"
#   endif

#   if ALIB_TEXT
#       if !defined (HPP_ALIB_STRINGS_FORMAT)
#          include "alib/strings/format.hpp"
#       endif
#   endif
#   if ALIB_MONOMEM
#      if !defined(HPP_ALIB_MONOMEM_HASHMAP)
#         include "alib/monomem/hashmap.hpp"
#      endif
#      if !defined(HPP_ALIB_MONOMEM_HASHSET)
#         include "alib/monomem/hashset.hpp"
#      endif
#   else
#      if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
#         include <unordered_map>
#      endif
#      if !defined(_GLIBCXX_SET) && !defined(_SET_)
#         include <set>
#      endif
#   endif

#endif // !defined(ALIB_DOX)

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

namespace aworx { namespace lib { namespace boxing { namespace detail {

#if !defined(ALIB_DOX)

#if ALIB_MONOMEM
    extern HashSet           <TypeFunctors::Key                 , TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
           HashSet           <TypeFunctors::Key                 , TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions(&monomem::GlobalAllocator);
    extern HashMap           <TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables;
           HashMap           <TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables(&monomem::GlobalAllocator);
    extern HashMap           <TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray;
           HashMap           <TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray(&monomem::GlobalAllocator);
#else
    extern std:set           <TypeFunctors::Key                 , TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
           std::set          <TypeFunctors::Key                 , TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
    extern std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables;
           std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables;
    extern std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray;
           std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray;
#endif


    ALIB_IF_THREADS( namespace { ThreadLock dbgLock; } )

    extern ALIB_API
    void DbgLockMaps( bool doLock );
    void DbgLockMaps( bool doLock )
    {
        #if !ALIB_THREADS
            (void) doLock;
            lib::DbgCheckSingleThreaded();
        #else
            if( doLock )
                dbgLock.Acquire( ALIB_CALLER_PRUNED );
            else
                dbgLock.Release();
        #endif
    }

#endif

void  DbgRegisterVTable( detail::VTable* vtable, detail::VTable::DbgFactoryType productionType )
{
    vtable->DbgProduction= productionType;
    DbgLockMaps(true);
        if( !vtable->IsArray() )
        {
            #if ALIB_MONOMEM
                ALIB_LOCK_WITH( aworx::lib::monomem::GlobalAllocatorLock )
                DbgKnownVTables.InsertUnique( std::make_pair( &vtable->Type, vtable ) );
            #else
                if ( DbgKnownVTables.find( vtable->Type ) != DbgKnownVTables.end() )
                {
                    ALIB_ERROR( "Double instantiation of VTable of Type: \"",
                                DbgTypeDemangler( vtable->Type ).Get(), "\"" )
                    DbgLockMaps(false);
                    return;
                }

                DbgKnownVTables.insert(std::make_pair( &vtable->Type, vtable ) );
            #endif
        }
        else
        {
            #if ALIB_MONOMEM
                ALIB_LOCK_WITH( aworx::lib::monomem::GlobalAllocatorLock )
                DbgKnownVTablesArray.InsertUnique(std::make_pair( &vtable->ElementType, vtable ) );
            #else
                if ( DbgKnownVTablesArray.find( vtable->ElementType ) != DbgKnownVTablesArray.end() )
                {
                    ALIB_ERROR( "Double instantiation of VTable of Type: \"",
                                DbgTypeDemangler( vtable->ElementType ).Get(), "[]\"" )
                    DbgLockMaps(false);
                    return;
                }

                DbgKnownVTablesArray.insert(std::make_pair( &vtable->ElementType, vtable ) );
            #endif
        }
    DbgLockMaps(false);
}

} // namespace aworx::lib::boxing[::detail]


#if ALIB_STRINGS

// #############################################################################################
// Type Name
// #############################################################################################
AString&     DbgBoxing::removeNamespaces( AString& string, integer startIndex  )
{
    for( auto& search: RemovableNamespaces )
    {
        integer idx;
        while( (idx= string.IndexOf(search, startIndex) ) >= 0 )
            string.Delete( idx, search.Length() );
    }
    return string;
}

std::vector<aworx::String>   DbgBoxing::RemovableNamespaces
{
    A_CHAR( "aworx::lib::boxing::" ),
    A_CHAR( "aworx::lib::"         ),
};

void  DbgBoxing::typeName( const detail::VTable* vtable, AString& result )
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

#if ALIB_TEXT

// #############################################################################################
// Type Info
// #############################################################################################

void  DbgBoxing::typeInfo( AString&                 target,
                           const detail::VTable*    vtable,
                           const String&            indent,
                           bool                     srcIsPointer,
                           bool                     isValueTypeCustomized,
                           bool                     isPointerTypeCustomized,
                           bool                     fitsToPlaceholder,
                           bool                     copyConstructible,
                           bool                     triviallyDestructible,
                           bool                     isUnboxable                )
{
    target <<indent << "Mapping:        " << (  vtable->Mapping == detail::VTable::MappingType::Pointer
                                              ? "Pointer"
                                              : vtable->Mapping == detail::VTable::MappingType::Value
                                              ? "Value"
                                              : vtable->Mapping == detail::VTable::MappingType::Enum
                                              ? "Enum"
                                              : "Array"
                                              ) << NewLine();
    if( vtable->Mapping == detail::VTable::MappingType::Enum )
    {
        target << indent << "Mapped Type:    "; typeName(vtable, target ); target << " (Enumeration)"  << NewLine();
        target << indent << "Customized:     Not customizable (always boxed as enum value type)"       << NewLine();
    }
    else
    {
        bool valueBoxing  =  vtable->Mapping == detail::VTable::MappingType::Value;
        bool pointerBoxing=  vtable->Mapping == detail::VTable::MappingType::Pointer;
        bool arrayBoxing  =  vtable->IsArray();
        bool srcIsValue   =  !srcIsPointer;

        target << indent << "Mapped Type:    "; typeName(vtable, target ); target << NewLine();

        target <<indent << "Customized T:   " << isValueTypeCustomized               << NewLine();
        target <<indent << "Customized T*:  " << isPointerTypeCustomized             << NewLine();
        target <<indent << "Is Unboxable:   "
               <<( isUnboxable
                   ? (     srcIsValue   && valueBoxing   && isValueTypeCustomized
                        ?  "Yes (By customization)"

                        :  srcIsValue   && valueBoxing   && !isValueTypeCustomized
                        ?  "Yes (Value fits in placeholder and is copy constructible)"

                        :  srcIsPointer && pointerBoxing && isPointerTypeCustomized
                        ?  "Yes (By customization)"

                        :  srcIsPointer && pointerBoxing && !isPointerTypeCustomized && (!copyConstructible || !triviallyDestructible)
                        ?  "Yes (Value would not be copy-constructible or trivially destructible)"

                        :  srcIsPointer && pointerBoxing && !isPointerTypeCustomized && !fitsToPlaceholder
                        ?  "Yes (Value would not fit to placeholder)"

                        :  srcIsValue   && arrayBoxing   && isValueTypeCustomized
                        ?  "Yes (Custom unboxing from array type)"

                        :  srcIsValue   && arrayBoxing   && isPointerTypeCustomized
                        ?  "Yes (Unboxing from array type, customized with pointer type)"

                        :  srcIsPointer && arrayBoxing   && isPointerTypeCustomized
                        ?  "Yes (Custom unboxing from array type)"

                        :  srcIsPointer && arrayBoxing   && isValueTypeCustomized
                        ?  "Yes (Unboxing from array type, customized with value type)"

                        : "INTERNAL ERROR IN DBG METHDO: CASE NOT MATCHED (E1)"
                     )
                   : (
                          srcIsValue    && isValueTypeCustomized
                        ? "Forbidden (By customization)"

                        :  srcIsValue   && pointerBoxing && isPointerTypeCustomized
                        ?  "Not as value (Pointer type is customized)"

                        :  srcIsValue   && pointerBoxing && !isPointerTypeCustomized  && (!copyConstructible || !triviallyDestructible)
                        ?  "Not as value (Not copy-constructible or trivially destructible)"

                        :  srcIsValue   && pointerBoxing && !isPointerTypeCustomized  && !fitsToPlaceholder
                        ?  "Not as value (Does not fit to placeholder)"

                        :  srcIsPointer && isPointerTypeCustomized
                        ? "Forbidden (By customization)"

                        :  srcIsPointer && valueBoxing && isValueTypeCustomized
                        ?  "Not as pointer (Value type is customized)"

                        :  srcIsPointer && valueBoxing && !isValueTypeCustomized
                        ?  "Not as pointer (Value fits in placeholder and is copy constructible and trivially destructible)"

                        : "INTERNAL ERROR IN DBG METHOd: CASE NOT MATCHED (E2)"
                     )
                 ) << NewLine();
    } // not enum

    target << indent << "VTable Type:    " << (   vtable->DbgProduction == detail::VTable::DbgFactoryType::Unregistered
                                                ? "INTERNAL ERROR IN DBG METHOD: STATIC VTABLE NOT REGISTERED"
                                                : vtable->DbgProduction == detail::VTable::DbgFactoryType::Dynamic
                                                ? "Dynamic Singleton"
                                                : "Static Singleton (Specialized T_VTableFactory)"
                                              )
                                           << NewLine();

    target << indent << "Usage Counter:  " << vtable->DbgCntUsage  << NewLine();

    ALIB_ASSERT_ERROR( target.IndexOf( A_CHAR("INTERNAL ERROR") ) < 0, "Error occurred describing type" )

    auto functions=  GetSpecificFunctionTypes(vtable);
    MonoAllocator allocator(8192);
    detail::DbgStringTable<uinteger> tmpStrings( allocator );
    String256 headline; headline << '\n' << indent << "Associated Specialized Functions:";
    String256 indent2;  indent2  << indent << indent;
    dumpFunctions( functions, target, headline, indent2, tmpStrings );
}


// #############################################################################################
// Dump Type Lists (conversion for type lists)
// #############################################################################################
AString DbgBoxing::DumpFunctions( const std::vector<std::pair<const std::type_info*,uinteger>>& input,
                                  const String&                                                 headline,
                                  const String&                                                 indent    )
{
    AString                            result;
    MonoAllocator                      allocator(8192);
    detail::DbgStringTable<uinteger>   tmpStrings( allocator );

    // repeat twice to get auto-tabs adjusted
    SPFormatter formatter= Formatter::AcquireDefault(ALIB_CALLER_PRUNED);
        for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
        {
            result.Reset();
            dumpFunctions( input, result, headline, indent, tmpStrings );
        }
    formatter->Release();

    return result;
}

void  DbgBoxing::dumpFunctions( const std::vector<std::pair<const std::type_info*,uinteger>>& input,
                                AString&                                                      output,
                                const String&                                                 headline,
                                const String&                                                 indent,
                                detail::DbgStringTable<uinteger>&                             tmpStrings )
{
    String512 buffer;
    tmpStrings.clear();
    for( auto& type : input )
        tmpStrings.Add( removeNamespaces(buffer.Reset() << *type.first, 0), type.second );

    std::sort( tmpStrings.begin(), tmpStrings.end(),
               [] (std::tuple<String, uinteger>& a,
                   std::tuple<String, uinteger>& b )
               {
                    return std::get<0>(a).template CompareTo<true, Case::Ignore>( std::get<0>(b) ) < 0;
               }
             );

    if ( headline.IsNotEmpty() )
        output << headline << NewLine();

    SPFormatter formatter= Formatter::GetDefault();
    Boxes& args= formatter->Acquire(ALIB_CALLER_PRUNED);
    args.Add( indent, "{}  {!ATab5}{:>2})\\n", nullptr, '(', nullptr );
    for( auto& nameAndUse : tmpStrings )
    {
        args[2]= std::get<0>(nameAndUse);
        args[4]= std::get<1>(nameAndUse) != (std::numeric_limits<uinteger>::max)()
                 ? Box( std::get<1>(nameAndUse) )
                 : Box( "No default implementation" );
        formatter->FormatArgs( output, args );
    }
    formatter->Release();
    output << NewLine();
}


// #############################################################################################
// Dump
// #############################################################################################

AString  DbgBoxing::DumpVTables( bool staticVtables, bool includeFunctions )
{
    AString                                        result;
    MonoAllocator                                  allocator(8192);

    // repeat twice to get auto-tabs adjusted
    SPFormatter formatter= Formatter::AcquireDefault(ALIB_CALLER_PRUNED);

        for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
        {
            result      .Reset();
            allocator   .Reset();
            detail::DbgStringTable<const detail::VTable*>  vtableNames( allocator );
            dumpVTables( result, vtableNames, staticVtables , includeFunctions );
        }

    formatter->Release();
    return result;
}


void DbgBoxing::dumpVTables( AString&                                        result,
                             detail::DbgStringTable<const detail::VTable*>&  vtableNames,
                             bool                                            staticVtables,
                             bool                                            includeFunctions )
{
    SPFormatter formatter= Formatter::AcquireDefault(ALIB_CALLER_PRUNED);

    // dump vtables and their interfaces
    result << ( staticVtables ? A_CHAR("Mapped types with static VTables")
                              : A_CHAR("Mapped types with dynamic VTables") );
    if( includeFunctions )
            result <<  A_CHAR(" and their associated specialized functions");

    (result << ':' << NewLine()) .InsertChars('-', 77) << NewLine();

    // Get vtables and add names to string array
    vtableNames.clear();
    String1K temp;
    detail::DbgLockMaps(true);
        for( int i= 0 ; i <  2; ++i )
            for( auto& it : ( i == 0 ? detail::DbgKnownVTables : detail::DbgKnownVTablesArray) )
            {
                if(   (it.second->DbgProduction == (staticVtables ? detail::VTable::DbgFactoryType::Static
                                                                  : detail::VTable::DbgFactoryType::Dynamic ))
                   ||  it.second->DbgProduction == detail::VTable::DbgFactoryType::Unregistered )
                {

                    temp.Reset();

                    temp << Format::Field( String64("(") << it.second->DbgCntUsage<< ")  ", 6, Alignment::Left );
                    typeName( it.second, temp );
                    if( it.second->DbgProduction == detail::VTable::DbgFactoryType::Unregistered )
                        temp << "  ATTENTION: Unregistered customized VTable!!! This is an Error";

                    vtableNames.Add( temp, it.second );
                }
            }
    detail::DbgLockMaps(false);

    std::sort( vtableNames.begin(), vtableNames.end(),
               []   ( detail::DbgStringTable<const detail::VTable*>::ElementType & a,
                      detail::DbgStringTable<const detail::VTable*>::ElementType & b )
               {
                    // skip the prepended usage number
                    Substring lhs= std::get<0>(a); lhs.TrimStart().ConsumeToken(' '); lhs.TrimStart();
                    Substring rhs= std::get<0>(b); rhs.TrimStart().ConsumeToken(' '); rhs.TrimStart();
                    return lhs.CompareTo<true, Case::Ignore>( rhs ) < 0;
               }
            );

    MonoAllocator allocator(8192);
    detail::DbgStringTable<uinteger>                         tempStrings( allocator );
    std::vector<std::pair<const std::type_info*,uinteger>>   tempFunctions;
    for( auto& vtable: vtableNames )
    {
        result << std::get<0>(vtable)  << NewLine();
        if( includeFunctions )
        {
            getFunctionTypes(std::get<1>(vtable)->Functions, tempFunctions );
            dumpFunctions( tempFunctions, result, NullString(),  A_CHAR(" "), tempStrings );
        }
    }

    formatter->Release();
}

AString  DbgBoxing::DumpAll()
{
    AString  result;

    // Get vtables and add names to string array
    MonoAllocator                                   allocator(8192);

    // repeat twice to get auto-tabs adjusted
    SPFormatter formatter= Formatter::AcquireDefault(ALIB_CALLER_PRUNED);

        for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
        {
            result   .Reset();
            allocator.Reset();
            detail::DbgStringTable<const detail::VTable*>   vtableNames( allocator );
            detail::DbgStringTable<uinteger>                tempStrings( allocator);
            dumpVTables( result, vtableNames, true , true );     result.NewLine();
            dumpVTables( result, vtableNames, false, true );     result.NewLine();

            auto knownFunctions= GetKnownFunctionTypes();
            (result << "Known Function Declarators And Usage Of Default Implementation:"
                    << NewLine()) .InsertChars('-', 77) << NewLine();
            dumpFunctions( knownFunctions, result, NullString(), A_CHAR("  "), tempStrings );

            #if ALIB_DEBUG_MONOMEM
                (result << NewLine() << "Metrics Of Custom Function Implementation HashMap: "
                        << NewLine()) .InsertChars('-', 77) << NewLine();
                DumpCustomFunctionHashMapMetrics( result, false );
                result << NewLine();
            #endif
        }

    formatter->Release();

    return result;
}


#endif // ALIB_TEXT

}}}// namespace [aworx::lib::boxing]

#endif // ALIB_DEBUG_BOXING


