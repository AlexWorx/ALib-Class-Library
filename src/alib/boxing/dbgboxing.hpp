// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_BOXING_DBGBOXING
#define HPP_ALIB_BOXING_DBGBOXING 1

#if ALIB_MODULE_STRINGFORMAT

#if !defined (HPP_ALIB_STRINGS_ASTRING)
    #include "alib/strings/astring.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)
ALIB_ASSERT_MODULE(STRINGFORMAT)


#if !defined(HPP_ALIB_STRINGS_)
#   include "alib/stringformat/formatter.hpp"
#endif


#if ALIB_DEBUG

namespace aworx { namespace lib { namespace boxing {


/** ************************************************************************************************
 * Provides low- and high-level debug methods for \alibmod_nolink_boxing.<br>
 * This struct is only available in debug compilations of \alib and furthermore only if an
 * \alibdist is used that includes module \alibmod_stringformat.
 *
 * This type is not constructible, as all method are static.
 *
 * To shorten custom type names, custom namespaces might be added to static vector
 * #RemovableNamespaces prior to invoking a method.
 *
 * \note
 *   While some of the methods are written with a non-capital letter (which usually indicates
 *   that they are \c protected or \c private), those are still declared \c public. These methods
 *   are "inner" methods used by the similar named capitalized methods.
 *   Their use is recommended, when writing custom "composite" debug-functions, as the various
 *   output parameters might be reused with subsequent invocations.
 *
 * \see
 *   Chapter \ref alib_boxing_more_debug of the
 *   \ref alib_mod_boxing "Programmer's Manual" of module \alibmod_nolink_boxing.
 **************************************************************************************************/
struct DbgBoxing
{
    /** ********************************************************************************************
     * Not constructible
     **********************************************************************************************/
    DbgBoxing() = delete;

    // #############################################################################################
    // Type Name
    // #############################################################################################
    /** ********************************************************************************************
     * Writes the (demangled) mapped type that the given \p{vtable} represents.
     *
     * @param  vtable           The \e vtable to get the mapped type name for.
     * @return A string containing the requested information.
     **********************************************************************************************/
    inline static
    AString      TypeName  ( const detail::VTable* vtable )
    {
        AString result;
        typeName( vtable, result );
        return result;
    }

    /** ********************************************************************************************
     * Convenience function: invokes #TypeName(const detail::VTable*) with
     * the \e vtable of the box created or passed with the invocation.
     *
     * @param  box              The box to get the mapped type name for.
     * @return A string containing the requested information.
     **********************************************************************************************/
    inline static
    AString      TypeName  ( const Box& box )
    {
        AString result;
        typeName( box.DbgGetVTable(), result );
        return result;
    }


    /** ********************************************************************************************
     * Implementation of #TypeName.
     *
     * @param      vtable           The vtable.
     * @param[out] result           The target string to write the type information to.
     **********************************************************************************************/
    ALIB_API static
    void         typeName  ( const detail::VTable* vtable, AString& result );


    // #############################################################################################
    // Type Info
    // #############################################################################################
    /** ********************************************************************************************
     * Returns human readable information about the type \p{TBoxable} in respect to how boxing
     * of objects of this type is performed.
     *
     * The implementation of this method creates a default value of \p{TBoxable}, boxes this
     * and returns the result of #TypeInfo(const Box&,const String&,const String&).
     * Hence, for types that are not default constructible, inner function \b %TypeInfo
     * has to be used with an accordingly boxed "sample" of type \p{TBoxable}
     *
     * \note
     *   Does not work for type \c void*, which is not customizable anyhow
     *   (always boxed as \c void*).
     *
     * @tparam TBoxable The boxable type to investigate in.
     * @param  headline The headline to write.
     * @param  indent   Spaces to write prior to each entry.
     * @return A string containing the requested information.
     **********************************************************************************************/
    template<typename TBoxable>
    static
    AString  TypeInfo( const String& headline        = A_CHAR( "Boxing Information For Boxable Type: ") ,
                       const String& indent          = A_CHAR( "  "                                   )   )

    {
        return DbgBoxing::TypeInfo<TBoxable>( TBoxable(), headline, indent );
    }

    /** ********************************************************************************************
     * Collects necessary information from template type \p{TBoxable} and invokes
     * #typeInfo.<br>
     * See method #TypeInfo.
     *
     * @tparam TBoxable The boxable type to investigate in.
     * @param  box      A box containing a "sample" of \p{TBoxable.}. If a different box is given
     *                  this method has undefined behaviour
     * @param  headline The headline to write.
     * @param  indent   Spaces to write prior to each entry.
     * @return A string containing the requested information.
     **********************************************************************************************/
    template<typename TBoxable>
    static
    AString  TypeInfo( const Box& box,
                       const String& headline= A_CHAR("Boxing Information For Boxable Type: "),
                       const String& indent  = A_CHAR("  "                                   ) )
    {
        AString target;
        using TSrc  = ATMP_RCV(TBoxable);
        using TPlain= ATMP_RCVP(TBoxable);

        DbgTypeDemangler srcRTTIDmgld( typeid(TSrc) );

        // repeat twice to get auto-tabs adjusted
        SPFormatter formatter= GetDefaultFormatter();
        formatter->Acquire(ALIB_CALLER_PRUNED);
        for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
        {
            target.Reset() << headline << srcRTTIDmgld.Get() << NewLine();
            removeNamespaces( target, headline.Length() );

            if( ATMP_EQ(TSrc, std::nullptr_t ) )
            {
                target  << indent << "'std::nullptr_t' sets a box to void state" << NewLine();
                return target;
            }

            typeInfo( target,
                      box.DbgGetVTable(),
                      indent,
                      ATMP_IS_PTR(TSrc),
                      lib::boxing::TT_IsCustomized<TPlain >::value,
                      lib::boxing::TT_IsCustomized<TPlain*>::value,
                      sizeof(lib::boxing::Placeholder) <= sizeof(ATMP_RCVP(TPlain)),
                      std::is_copy_constructible<ATMP_RCVP(TPlain)>::value,
                      lib::boxing::TT_IsUnboxable <TSrc   >::value                             );

        }
        formatter->Release();
        return target;
    }

    /** ********************************************************************************************
     * See method #TypeInfo, which invokes this method using template parameter \p{TBoxable}
     * @param[out] target                   The target string.
     * @param      vtable                   The vtable of the mapped type.
     * @param      indent                   Empty or a string of spaces that are alwys preprended
     *                                      prior to each entry.
     * @param      srcIsPointer             Denotes if \p{TBoxable} is a pointer type.
     * @param      isValueTypeCustomized    Denotes if the value type of \p{TBoxable} is customized.
     * @param      isPointerTypeCustomized  Denotes if the pointer type of \p{TBoxable} is
     *                                      customized.
     * @param      fitsToPlaceholder        Denotes if the value type of \p{TBoxable} would fit to
     *                                      placeholder.
     * @param      copyConstructible        Denotes if the value type of \p{TBoxable} is
     *                                      copy constructible.
     * @param      isUnboxable              Denotes if \p{TBoxable} is unboxable.
     **********************************************************************************************/
    ALIB_API static
    void typeInfo( AString&                 target,
                   const detail::VTable*    vtable,
                   const String&            indent,
                   bool                     srcIsPointer,
                   bool                     isValueTypeCustomized,
                   bool                     isPointerTypeCustomized,
                   bool                     fitsToPlaceholder,
                   bool                     copyConstructible,
                   bool                     isUnboxable                         );


    // #############################################################################################
    // Function Lists
    // #############################################################################################
    /** ********************************************************************************************
     * Returns a vector of \c std::type_info objects, representing all function types that either
     * a default or a type specific implementation has been registered for.<br>
     * A second value in the vector provides the number of invocations of a default implementation.
     * If such is not available, this number is set to <c>std::numeric_limits<uinteger>::max()</c>.
     *
     * The list includes the built-in function types.
     *
     * \see
     *   The result can be conveniently passed to #DumpFunctions.
     * @return A vector of pairs of type information structs and usage numbers.
     **********************************************************************************************/
    ALIB_API static
    std::vector<std::pair<const std::type_info*,uinteger>>
    GetKnownFunctionTypes();

    /** ********************************************************************************************
     * Collects all function declarator types of the given box-function table.
     *
     * \see
     *   The result can be conveniently passed to #DumpFunctions.
     *
     * @param input  The function table to use.
     * @param output The result vector to fill.
     **********************************************************************************************/
    ALIB_API static
    void getFunctionTypes( const detail::FunctionTable&                             input,
                           std::vector<std::pair<const std::type_info*,uinteger>>&  output  );

    /** ********************************************************************************************
     * Collects all function declarator types, with type-specific implementations.
     * Parameter \p{vtable} might for example be retrieved from a box instance with
     * \alib{boxing,Box::DbgGetVTable}.
     *
     * \see
     *   The result can be conveniently passed to #DumpFunctions.
     * @param vtable The vtable to get all function implementations for.
     * @return A vector of type information structs.
     **********************************************************************************************/
    inline static
    std::vector<std::pair<const std::type_info*,uinteger>>
    GetSpecificFunctionTypes( const detail::VTable* vtable )
    {
        std::vector<std::pair<const std::type_info*,uinteger>> result;
        getFunctionTypes( vtable->Functions, result );
        return result;
    }

    /** ********************************************************************************************
     * Convenience function: invokes #GetSpecificFunctionTypes(const detail::VTable*) with
     * the \e vtable of the box created or passed with the invocation.
     *
     * \see
     *   The result can be conveniently passed to #DumpFunctions.
     *
     * @param box  A box that the vtable is used of.
     * @return A vector of type information structs.
     **********************************************************************************************/
    inline static
    std::vector<std::pair<const std::type_info*,uinteger>>
    GetSpecificFunctionTypes( const Box& box )
    {
        return  GetSpecificFunctionTypes( box.DbgGetVTable() );
    }


    // #############################################################################################
    // Dump
    // #############################################################################################
    /** ********************************************************************************************
     * Takes a vector of pairs of \c std::type_info pointers and a usage counter, and returns an
     * \b AString with a sorted list of type names including counter information.
     *
     * @param input             A list of types.
     * @param headline          The headline to write.
     * @param indent            Spaces to write prior to each entry.
     * @return A list of demangled type names.
     **********************************************************************************************/
    ALIB_API static
    aworx::AString   DumpFunctions( const std::vector<std::pair<const std::type_info*,uinteger>>& input,
                                    const String&                                                 headline = EmptyString(),
                                    const String&                                                 indent   = EmptyString());

    /** ********************************************************************************************
     * Helper for (bigger part of) #DumpFunctions.
     *
     * @param      input            A list of types and usage numbers.
     * @param[out] output           A string to write to.
     * @param      headline         The headline to write.
     * @param      indent           Spaces to write prior to each entry.
     * @param      tmpStrings       A buffer needed for internal use.
     **********************************************************************************************/
    ALIB_API static
    void  dumpFunctions ( const std::vector<std::pair<const std::type_info*,uinteger>>& input,
                          AString&                                                      output,
                          const String&                                                 headline,
                          const String&                                                 indent,
                          strings::util::StringTable<uinteger>&                         tmpStrings  );


    /** ********************************************************************************************
     * Lists all mapped types with either static or dynamic \e vtables.
     * Along with each type, its default function implementations are given.
     *
     * @param staticVtables    If \c true, only types with static vtables are listed.
     *                         Otherwise only those with dynamic vtables.
     * @param includeFunctions If \c true for each type, the list of specialized functions are
     *                         listed.
     * @return A string containing the dump.
     **********************************************************************************************/
    ALIB_API static
    AString        DumpVTables( bool staticVtables,
                                bool includeFunctions= false  );

    /** ********************************************************************************************
     * Internally used by overloaded #DumpVTables methods.
     *
     * @param[out] target           The target string to write to.
     * @param      vtableNames      Output parameter that receives the strings.
     * @param      staticVtables    If \c true, only types with static vtables are listed.
     *                              Otherwise only those with dynamic vtables.
     * @param      includeFunctions If \c true, For each \e vtable the list of specialized
     *                              function is written.
     **********************************************************************************************/
    ALIB_API static
    void           dumpVTables( AString&                                           target,
                                strings::util::StringTable<const detail::VTable*>& vtableNames,
                                bool                                               staticVtables,
                                bool                                               includeFunctions );
    /** ********************************************************************************************
     * First, lists all mapped types with static, then those with dynamic \e vtables.
     * Along with each type, its default function implementations are given.
     *
     * @return A string containing the dump.
     **********************************************************************************************/
    ALIB_API static
    AString        DumpAll   ();

    // #############################################################################################
    // Tools
    // #############################################################################################
    /** ********************************************************************************************
     * Removes namespaces in the given \p{string}. The function is used with all methods that
     * create string values containing type names.
     * Note that custom namespaces might be added to static field #RemovableNamespaces
     * prior to invoking any method.
     *
     * @param string      The string to remove the namespace from.
     * @param startIndex  The index within the string to start searching for removable namespaces.
     * @return A string containing the requested information.
     **********************************************************************************************/
    ALIB_API static
    AString&     removeNamespaces( AString& string, integer startIndex  );

    /**
     * See method #removeNamespaces. Pre-initialized with <b>"aworx::lib::"</b>.
     */
    ALIB_API static
    std::vector<aworx::String>   RemovableNamespaces;

}; // class DbgBoxingDump


}} // namespace aworx[::lib::boxing]

/// Type alias in namespace #aworx.
using     DbgBoxing=  aworx::lib::boxing::DbgBoxing;

}// namespace [aworx]

#endif // ALIB_DEBUG

#endif // ALIB_MODULE_STRINGFORMAT

#endif // HPP_ALIB_BOXING_DBGBOXING
