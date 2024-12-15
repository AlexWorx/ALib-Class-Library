// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/boxing/boxing.hpp"
#endif // !DOXYGEN

#if ALIB_DEBUG_BOXING

#if !DOXYGEN
#   include "alib/boxing/dbgboxing.hpp"
#   include "alib/compatibility/std_typeinfo.hpp"
#   if ALIB_THREADS
#      include "alib/threads/recursivelock.hpp"
#   endif
#   if ALIB_CAMP
#       include "alib/strings/astring.hpp"
#       include "alib/monomem/localallocator.hpp"
#   endif
#   if ALIB_MONOMEM
#       include "alib/containers/hashtable.hpp"
#       include "alib/monomem/globalallocator.hpp"
#   else
#       include <unordered_map>
#       include <unordered_set>
#   endif
#endif // !DOXYGEN

#include <algorithm>

#include "alib/lang/callerinfo_functions.hpp"

namespace alib {  namespace boxing { namespace detail {

#if !DOXYGEN

#if ALIB_MONOMEM
    extern HashSet           <MonoAllocator, TypeFunctors::Key                 , TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
           HashSet           <MonoAllocator, TypeFunctors::Key                 , TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions(monomem::GLOBAL_ALLOCATOR);
    extern HashMap           <MonoAllocator, TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables;
           HashMap           <MonoAllocator, TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables(monomem::GLOBAL_ALLOCATOR);
    extern HashMap           <MonoAllocator, TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray;
           HashMap           <MonoAllocator, TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray(monomem::GLOBAL_ALLOCATOR);
#else
    extern std::unordered_set<TypeFunctors::Key                 , TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
           std::unordered_set<TypeFunctors::Key                 , TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
    extern std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables;
           std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTables;
    extern std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray;
           std::unordered_map<TypeFunctors::Key, detail::VTable*, TypeFunctors::Hash, TypeFunctors::EqualTo>  DbgKnownVTablesArray;
#endif


    IF_ALIB_THREADS( namespace { RecursiveLock dbgLock; } )

    extern ALIB_API
    void DbgLockMaps( bool doLock );
    void DbgLockMaps( bool doLock )
    {
        #if !ALIB_THREADS
            (void) doLock;
            DbgAssertSingleThreaded();
        #else
            ALIB_DBG(dbgLock.Dbg.Name= "DbgBoxing";)
            if( doLock )
                dbgLock.AcquireRecursive( ALIB_CALLER_PRUNED );
            else
                dbgLock.ReleaseRecursive( ALIB_CALLER_PRUNED );
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
                ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                DbgKnownVTables.InsertUnique( std::make_pair( &vtable->Type, vtable ) );
            #else
                if ( DbgKnownVTables.find( &vtable->Type ) != DbgKnownVTables.end() )
                {
                    ALIB_ERROR( "BOXING", "Double instantiation of VTable of Type: \"",
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
                ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                DbgKnownVTablesArray.InsertUnique(std::make_pair( &vtable->ElementType, vtable ) );
            #else
                if ( DbgKnownVTablesArray.find( &vtable->ElementType ) != DbgKnownVTablesArray.end() )
                {
                    ALIB_ERROR( "BOXING", "Double instantiation of VTable of Type: \"",
                                DbgTypeDemangler( vtable->ElementType ).Get(), "[]\"" )
                    DbgLockMaps(false);
                    return;
                }

                DbgKnownVTablesArray.insert(std::make_pair( &vtable->ElementType, vtable ) );
            #endif
        }
    DbgLockMaps(false);
}

} // namespace alib::boxing[::detail]


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

std::vector<alib::String>   DbgBoxing::RemovableNamespaces
{
    A_CHAR( "alib::boxing::" ),
    A_CHAR( "alib::"         ),
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

#if ALIB_CAMP

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
                                              ) << NEW_LINE;
    if( vtable->Mapping == detail::VTable::MappingType::Enum )
    {
        target << indent << "Mapped Type:    "; typeName(vtable, target ); target << " (Enumeration)"  << NEW_LINE;
        target << indent << "Customized:     Not customizable (always boxed as enum value type)"       << NEW_LINE;
    }
    else
    {
        bool valueBoxing  =  vtable->Mapping == detail::VTable::MappingType::Value;
        bool pointerBoxing=  vtable->Mapping == detail::VTable::MappingType::Pointer;
        bool arrayBoxing  =  vtable->IsArray();
        bool srcIsValue   =  !srcIsPointer;

        target << indent << "Mapped Type:    "; typeName(vtable, target ); target << NEW_LINE;

        target <<indent << "Customized T:   " << isValueTypeCustomized            << NEW_LINE;
        target <<indent << "Customized T*:  " << isPointerTypeCustomized          << NEW_LINE;
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
                 ) << NEW_LINE;
    } // not enum

    target << indent << "VTable Type:    " << (   vtable->DbgProduction == detail::VTable::DbgFactoryType::Unregistered
                                                ? "INTERNAL ERROR IN DBG METHOD: STATIC VTABLE NOT REGISTERED"
                                                : vtable->DbgProduction == detail::VTable::DbgFactoryType::Dynamic
                                                ? "Dynamic Singleton"
                                                : "Static Singleton (Specialized T_VTableFactory)"
                                              )
                                           << NEW_LINE;

    target << indent << "Usage Counter:  " << vtable->DbgCntUsage  << NEW_LINE;

    ALIB_ASSERT_ERROR( target.IndexOf( A_CHAR("INTERNAL ERROR") ) < 0,
                       "BOXING", "Error occurred describing type" )

    auto functions=  GetSpecificFunctionTypes(vtable);
    LocalAllocator8K la;
    detail::DbgStringTable<uinteger> tmpStrings( la );
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
    LocalAllocator8K                   la;
    detail::DbgStringTable<uinteger>   tmpStrings( la );

    // repeat twice to get auto-tabs adjusted
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter& formatter= *Formatter::Default;
    formatter.Reset();
    for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
    {
        result.Reset();
        dumpFunctions( input, result, headline, indent, tmpStrings );
    }

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
                    return std::get<0>(a).template CompareTo<CHK, lang::Case::Ignore>( std::get<0>(b) ) < 0;
               }
             );

    if ( headline.IsNotEmpty() )
        output << headline << NEW_LINE;

    Formatter& formatter= *Formatter::Default;
    BoxesMA& args= formatter.Reset();
    args.Add( indent, "{}  {!ATab5}{:>2})\n", nullptr, '(', nullptr );
    for( auto& nameAndUse : tmpStrings )
    {
        args[2]= std::get<0>(nameAndUse);
        args[4]= std::get<1>(nameAndUse) != (std::numeric_limits<uinteger>::max)()
                 ? Box( std::get<1>(nameAndUse) )
                 : Box( "No default implementation" );
        formatter.FormatArgs( output, args );
    }
    output << NEW_LINE;
}


// #############################################################################################
// Dump
// #############################################################################################

AString  DbgBoxing::DumpVTables( bool staticVtables, bool includeFunctions )
{
    AString             result;
    LocalAllocator8K    allocator;

    // repeat twice to get auto-tabs adjusted
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter& formatter= *Formatter::Default;
    formatter.Reset();

    for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
    {
        result      .Reset();
        allocator   .Reset();
        detail::DbgStringTable<const detail::VTable*>  vtableNames( allocator );
        dumpVTables( result, vtableNames, staticVtables , includeFunctions );
    }

    return result;
}


void DbgBoxing::dumpVTables( AString&                                        result,
                             detail::DbgStringTable<const detail::VTable*>&  vtableNames,
                             bool                                            staticVtables,
                             bool                                            includeFunctions )
{
    // dump vtables and their interfaces
    result << ( staticVtables ? A_CHAR("Mapped types with static VTables")
                              : A_CHAR("Mapped types with dynamic VTables") );
    if( includeFunctions )
            result <<  A_CHAR(" and their associated specialized functions");

    (result << ':' << NEW_LINE) .InsertChars('-', 77) << NEW_LINE;

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

                    temp << Format::Field( String64("(") << it.second->DbgCntUsage<< ")  ", 6, lang::Alignment::Left );
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
                    return lhs.CompareTo<CHK, lang::Case::Ignore>( rhs ) < 0;
               }
            );

    LocalAllocator8K la;
    detail::DbgStringTable<uinteger>                         tempStrings( la );
    std::vector<std::pair<const std::type_info*,uinteger>>   tempFunctions;
    for( auto& vtable: vtableNames )
    {
        result << std::get<0>(vtable)  << NEW_LINE;
        if( includeFunctions )
        {
            getFunctionTypes(std::get<1>(vtable)->Functions, tempFunctions );
            dumpFunctions( tempFunctions, result, NULL_STRING,  A_CHAR(" "), tempStrings );
        }
    }
}

AString  DbgBoxing::DumpAll()
{
    AString  result;

    // Get vtables and add names to string array
    LocalAllocator8K  la;

    // repeat twice to get auto-tabs adjusted
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter::Default->Reset();
    for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
    {
        result   .Reset();
        la.Reset();
        detail::DbgStringTable<const detail::VTable*>   vtableNames( la );
        detail::DbgStringTable<uinteger>                tempStrings( la);

        dumpVTables( result, vtableNames, true , true );     result.NewLine();
        dumpVTables( result, vtableNames, false, true );     result.NewLine();

        auto knownFunctions= GetKnownFunctionTypes();
        (result << "Known Function Declarators And Usage Of Default Implementation:"
                << NEW_LINE) .InsertChars('-', 77) << NEW_LINE;
        dumpFunctions( knownFunctions, result, NULL_STRING, A_CHAR("  "), tempStrings );

        #if ALIB_DEBUG_CONTAINERS
            (result << NEW_LINE << "Metrics Of Custom Function Implementation HashMap: "
                    << NEW_LINE) .InsertChars('-', 77) << NEW_LINE;
            DumpCustomFunctionHashMapMetrics( result, false );
            result << NEW_LINE;
        #endif
    }

    return result;
}


#endif // ALIB_CAMP

}} // namespace [alib::boxing]

#include "alib/lang/callerinfo_methods.hpp"

#endif // ALIB_DEBUG_BOXING
