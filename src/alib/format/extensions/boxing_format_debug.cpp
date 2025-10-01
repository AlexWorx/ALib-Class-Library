// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/containers/containers.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#if ALIB_DEBUG_BOXING
#   include <vector>
#   include <algorithm>
#endif
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Format;
#   if ALIB_DEBUG_BOXING
#      include "ALib.Lang.H"
#      include "ALib.Monomem.H"
#       if ALIB_DEBUG_CONTAINERS
#          include "ALib.Containers.HashTable.H"
#       endif
#   endif
#else
#   if ALIB_DEBUG_BOXING
#      include "ALib.Lang.H"
#      include "ALib.Monomem.H"
#      if ALIB_DEBUG_CONTAINERS
#         include "ALib.Containers.HashTable.H"
#      endif
#      include "ALib.Format.H"
#   endif
#endif

// ======================================   Implementation   =======================================
#if ALIB_DEBUG_BOXING
#   include "ALib.Lang.CIFunctions.H"
namespace alib::boxing::debug {

void  typeInfo( AString&                 target,
                const detail::VTable*    vtable,
                const String&            indent,
                bool                     srcIsPointer,
                bool                     srcIsStringType,
                bool                     srcIsUnboxableStringType,
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
                        ?  "Yes (Value fits in placeholder and is copy-constructible)"

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

                        :  srcIsUnboxableStringType
                        ?  "Yes (Unboxing from character array type)"

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
                        ?  "Not as pointer (Value fits in placeholder and is copy-constructible and trivially destructible)"

                        :  arrayBoxing
                        ?  "Arrays cannot be unboxed"

                        :  srcIsStringType
                        ?  "No (String type is not marked for implicit construction from character array type)"

                        : "INTERNAL ERROR IN DBG METHOd: CASE NOT MATCHED (E2)"
                     )
                 ) << NEW_LINE;
    } // not enum

    target << indent << "VTable Type:    " << (   vtable->DbgProduction == detail::VTable::DbgFactoryType::Unregistered
                                                ? "STATIC VTABLE NOT REGISTERED"
                                                : vtable->DbgProduction == detail::VTable::DbgFactoryType::Dynamic
                                                ? "Dynamic Singleton"
                                                : "Static Singleton (Specialized VTableOptimizationTraits)"
                                              )
                                           << NEW_LINE;

    target << indent << "Usage Counter:  " << vtable->DbgCntUsage  << NEW_LINE;

    ALIB_ASSERT_ERROR( target.IndexOf( A_CHAR("INTERNAL ERROR") ) < 0, "BOXING",
                       "An internal error occurred while describing type. Description follows:\n"
                       , target)

    ALIB_ASSERT_WARNING( target.IndexOf( A_CHAR("STATIC VTABLE NOT REGISTERED") ) < 0, "BOXING",
                         "An warning occurred while describing type. Description follows:\n"
                         , target)
    auto functions=  GetSpecificFunctionTypes(vtable);
    LocalAllocator8K la;
    DbgStringTable<uinteger> tmpStrings( la );
    String256 headline; headline << '\n' << indent << "Associated Specialized Functions:";
    String256 indent2;  indent2  << indent << indent;
    dumpFunctions( functions, target, headline, indent2, tmpStrings );
}


// #############################################################################################
// Dump Type Lists (conversion for type lists)
// #############################################################################################
AString DumpFunctions( const std::vector<std::pair<const std::type_info*,uinteger>>& input,
                       const String&                                                 headline,
                       const String&                                                 indent    )
{
    AString                            result;
    LocalAllocator8K                   la;
    DbgStringTable<uinteger>   tmpStrings( la );

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

void  dumpFunctions( const std::vector<std::pair<const std::type_info*,uinteger>>& input,
                     AString&                                                      output,
                     const String&                                                 headline,
                     const String&                                                 indent,
                     DbgStringTable<uinteger>&                                     tmpStrings )
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
    auto& args= formatter.Reset();
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

AString  DumpVTables( bool staticVtables, bool includeFunctions )
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
        DbgStringTable<const detail::VTable*>  vtableNames( allocator );
        dumpVTables( result, vtableNames, staticVtables , includeFunctions );
    }

    return result;
}


void dumpVTables( AString&                                result,
                  DbgStringTable<const detail::VTable*>&  vtableNames,
                  bool                                    staticVtables,
                  bool                                    includeFunctions )
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
    DbgLockMaps(true);
        for( int i= 0 ; i <  2; ++i )
            for( auto& it : ( i == 0 ? DbgKnownVTables : DbgKnownVTablesArray) )
            {
                if(   (it.second->DbgProduction == (staticVtables ? detail::VTable::DbgFactoryType::Static
                                                                  : detail::VTable::DbgFactoryType::Dynamic ))
                   ||  it.second->DbgProduction == detail::VTable::DbgFactoryType::Unregistered )
                {

                    temp.Reset();

                    temp << Field( String64("(") << it.second->DbgCntUsage<< ")  ", 6, lang::Alignment::Left );
                    typeName( it.second, temp );
                    if( it.second->DbgProduction == detail::VTable::DbgFactoryType::Unregistered )
                        temp << "  ATTENTION: Unregistered customized VTable!!! This is an Error";

                    vtableNames.Add( temp, it.second );
                }
            }
    DbgLockMaps(false);

    std::sort( vtableNames.begin(), vtableNames.end(),
               []   ( DbgStringTable<const detail::VTable*>::ElementType & a,
                      DbgStringTable<const detail::VTable*>::ElementType & b )
               {
                    // skip the prepended usage number
                    Substring lhs= std::get<0>(a); lhs.TrimStart().ConsumeToken(' '); lhs.TrimStart();
                    Substring rhs= std::get<0>(b); rhs.TrimStart().ConsumeToken(' '); rhs.TrimStart();
                    return lhs.CompareTo<CHK, lang::Case::Ignore>( rhs ) < 0;
               }
            );

    LocalAllocator8K la;
    DbgStringTable<uinteger>                                 tempStrings( la );
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


AString  DumpAll()
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
        DbgStringTable<const detail::VTable*>   vtableNames( la );
        DbgStringTable<uinteger>                tempStrings( la);

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

#if ALIB_DEBUG_CONTAINERS
void DumpCustomFunctionHashMapMetrics( AString& target, bool detailedBucketList ) {
    target << containers::DbgDumpDistribution( detail::customFunctionMap, detailedBucketList );
}
#endif
} // namespace [alib::boxing::debug]

#   include "ALib.Lang.CIMethods.H"
#endif // ALIB_DEBUG_BOXING
