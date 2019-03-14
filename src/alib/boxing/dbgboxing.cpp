// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"


#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

#if ALIB_MODULE_STRINGFORMAT


#if !defined(HPP_ALIB_BOXING_DBGBOXING)
#   include "alib/boxing/dbgboxing.hpp"
#endif

#if ALIB_DEBUG



#if !defined(HPP_ALIB_STRINGS_UTIL_STRINGTUPLES)
#   include "alib/strings/util/stringtuples.hpp"
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif


namespace aworx { namespace lib { namespace boxing {

// #############################################################################################
// Type Name
// #############################################################################################
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

                        :  srcIsPointer && pointerBoxing && !isPointerTypeCustomized && !copyConstructible
                        ?  "Yes (Value would not fit to placeholder)"

                        :  srcIsPointer && pointerBoxing && !isPointerTypeCustomized && !fitsToPlaceholder
                        ?  "Yes (Value would not be copy-constructible)"

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

                        :  srcIsValue   && pointerBoxing && !isPointerTypeCustomized  && !copyConstructible
                        ?  "Not as value (Not copy-constructible)"

                        :  srcIsValue   && pointerBoxing && !isPointerTypeCustomized  && !fitsToPlaceholder
                        ?  "Not as value (Does not fit to placeholder)"

                        :  srcIsPointer && isPointerTypeCustomized
                        ? "Forbidden (By customization)"

                        :  srcIsPointer && valueBoxing && isValueTypeCustomized
                        ?  "Not as pointer (Value type is customized)"

                        :  srcIsPointer && valueBoxing && !isValueTypeCustomized
                        ?  "Not as pointer (Value fits in placeholder and is copy constructible)"

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
    StringTable<uinteger> tmpStrings;
    String256 headline; headline << '\n' << indent << "Associated Specialized Functions:";
    String256 indent2;  indent2  << indent << indent;
    dumpFunctions( functions, target, headline, indent2, tmpStrings );
}



// #############################################################################################
// Function Lists
// #############################################################################################
std::vector<std::pair<const std::type_info*,uinteger>>  DbgBoxing::GetKnownFunctionTypes()
{
    std::vector<std::pair<const std::type_info*,uinteger>> result;
    result.emplace_back( &typeid( FHashcode         ), detail::DEFAULT_FUNCTIONS.fHashcode  ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFHashcode  : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FClone            ), detail::DEFAULT_FUNCTIONS.fClone     ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFClone     : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FIsNotNull        ), detail::DEFAULT_FUNCTIONS.fIsNotNull ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFIsNotNull : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FEquals           ), detail::DEFAULT_FUNCTIONS.fEquals    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFEquals    : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FIsLess           ), detail::DEFAULT_FUNCTIONS.fIsLess    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFIsLess    : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FIsTrue           ), detail::DEFAULT_FUNCTIONS.fIsTrue    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFIsTrue    : (std::numeric_limits<uinteger>::max)() );
    result.emplace_back( &typeid( FAppend<character>), detail::DEFAULT_FUNCTIONS.fAppend    ? detail::DEFAULT_FUNCTIONS.DbgCntInvocationsFAppend    : (std::numeric_limits<uinteger>::max)() );

    detail::DbgLockMaps(true);
        for (auto& it : detail::DBG_KNOWN_FUNCTIONS )
        {
            auto& fTypeId= it.get();

            // search corresponding default implementation.
            auto usage= (std::numeric_limits<uinteger>::max)();
            detail::FunctionTable::Entry* entry= detail::DEFAULT_FUNCTIONS.Custom;
            while( entry )
            {
                if( entry->Type == fTypeId )
                {
                    usage= entry->DbgCntInvocations;
                    break;
                }
                entry= entry->Next;
            }
            result.emplace_back( &fTypeId, usage );
        }
    detail::DbgLockMaps(false);

    return result;
}

void DbgBoxing::getFunctionTypes( const detail::FunctionTable&                             input,
                                  std::vector<std::pair<const std::type_info*,uinteger>>&  output  )
{
    output.clear();
    if(input.fHashcode ) output.emplace_back( &typeid( FHashcode         ), input.DbgCntInvocationsFHashcode  );
    if(input.fClone    ) output.emplace_back( &typeid( FClone            ), input.DbgCntInvocationsFClone     );
    if(input.fIsNotNull) output.emplace_back( &typeid( FIsNotNull        ), input.DbgCntInvocationsFIsNotNull );
    if(input.fEquals   ) output.emplace_back( &typeid( FEquals           ), input.DbgCntInvocationsFEquals    );
    if(input.fIsLess   ) output.emplace_back( &typeid( FIsLess           ), input.DbgCntInvocationsFIsLess    );
    if(input.fIsTrue   ) output.emplace_back( &typeid( FIsTrue           ), input.DbgCntInvocationsFIsTrue    );
    if(input.fAppend   ) output.emplace_back( &typeid( FAppend<character>), input.DbgCntInvocationsFAppend    );

    detail::FunctionTable::Entry* entry= input.Custom;
    while( entry )
    {
        output.emplace_back( &entry->Type , entry->DbgCntInvocations );
        entry= entry->Next;
    }
}


// #############################################################################################
// Dump Type Lists (conversion for type lists)
// #############################################################################################
AString DbgBoxing::DumpFunctions( const std::vector<std::pair<const std::type_info*,uinteger>>& input,
                                  const String&                                                 headline,
                                  const String&                                                 indent    )
{
    AString                 result;
    StringTable<uinteger>   tmpStrings;

    // repeat twice to get auto-tabs adjusted
    SPFormatter formatter= GetDefaultFormatter();
    formatter->Acquire(ALIB_CALLER_PRUNED);
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
                                strings::util::StringTable<uinteger>&                         tmpStrings )
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

    SPFormatter formatter= GetDefaultFormatter();
    formatter->Acquire(ALIB_CALLER_PRUNED);
    for( auto& nameAndUse : tmpStrings )
        formatter->Format( output, indent, "{}  {!ATab5}{:>2})\\n",
                           std::get<0>(nameAndUse), '(',
                           std::get<1>(nameAndUse) != (std::numeric_limits<uinteger>::max)()
                            ? Box( std::get<1>(nameAndUse) )
                            : Box( "No default implementation" )
                            );
    formatter->Release();
    output << NewLine();
}


// #############################################################################################
// Dump
// #############################################################################################

AString  DbgBoxing::DumpVTables( bool staticVtables, bool includeFunctions )
{
    AString                             result;
    StringTable<const detail::VTable*>  vtableNames;

    // repeat twice to get auto-tabs adjusted
    SPFormatter formatter= GetDefaultFormatter();
    formatter->Acquire(ALIB_CALLER_PRUNED);

        for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
        {
            result.Reset();
            dumpVTables( result, vtableNames, staticVtables , includeFunctions );
        }

    formatter->Release();
    return result;
}


void DbgBoxing::dumpVTables( AString&                                result,
                             StringTable<const detail::VTable*>&     vtableNames,
                             bool                                    staticVtables,
                             bool                                    includeFunctions )
{
    SPFormatter formatter= GetDefaultFormatter();
    formatter->Acquire(ALIB_CALLER_PRUNED);

    // dump vtables and their interfaces
    result << ( staticVtables ? A_CHAR("Mapped types with static VTables")
                              : A_CHAR("Mapped types with dynamic VTables") );
    if( includeFunctions )
            result <<  A_CHAR(" and their associated specialized functions");

    (result << ':' << NewLine()) .InsertChars('-', 77) << NewLine();

    // Get vtables and add names to string array
    vtableNames.Clear();
    String1K temp;
    detail::DbgLockMaps(true);
        for( int i= 0 ; i <  2; ++i )
            for( auto& it : detail::DBG_KNOWN_VTABLES[i] )
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
               []   ( StringTable<const detail::VTable*>::ElementType & a,
                      StringTable<const detail::VTable*>::ElementType & b )
               {
                    // skip the prepended usage number
                    Substring lhs= std::get<0>(a); lhs.TrimStart().ConsumeToken(' '); lhs.TrimStart();
                    Substring rhs= std::get<0>(b); rhs.TrimStart().ConsumeToken(' '); rhs.TrimStart();
                    return lhs.CompareTo<true, Case::Ignore>( rhs ) < 0;
               }
            );

    StringTable<uinteger>                                    tempStrings;
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
    StringTable<const detail::VTable*>    vtableNames;
    // repeat twice to get auto-tabs adjusted
    SPFormatter formatter= GetDefaultFormatter();
    formatter->Acquire(ALIB_CALLER_PRUNED);

        for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
        {
            result.Reset();
            dumpVTables( result, vtableNames, true , true );     result.NewLine();
            dumpVTables( result, vtableNames, false, true );     result.NewLine();


            StringTable<uinteger>   tempStrings;
            auto knownFunctions= GetKnownFunctionTypes();
            (result << "Known Function Declarators And Usage Of Default Implementation:"
                    << NewLine()) .InsertChars('-', 77) << NewLine();
            dumpFunctions( knownFunctions, result, NullString(), A_CHAR("  "), tempStrings );
        }

    formatter->Release();

    return result;
}

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


}}}// namespace [aworx::lib::boxing]


#endif // ALIB_DEBUG
#endif // ALIB_MODULE_STRINGFORMAT


