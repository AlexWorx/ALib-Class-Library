//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_DEBUG_BOXING

ALIB_EXPORT namespace alib::boxing {

//==================================================================================================
/// This namespace provides low- and high-level debug function for \alib_boxing_nl.<br>
///
/// To shorten custom type names, custom namespaces might be added to vector
/// \alib{boxing;debug::RemovableNamespaces} before invoking a function.
///
/// \note
///   Some of the functions are named with a non-capital letter which in \alib-types usually
//    indicates that they are \c protected or \c private.
//    These functions are "inner" functions used by the similar named capitalized functions.
///   Their use is recommended, when writing custom "composite" debug-functions, as the various
///   output parameters might be reused with subsequent invocations.
///
/// \par Availability
///  This namespace is only available if the compiler-symbol \ref ALIB_DEBUG_BOXING is set.
///
/// \par
///   The following functions are available independent of the \alibbuild.
///   - #GetKnownVTables
///   - #GetKnownFunctionTypes
///   - #GetSpecificFunctionTypes
///   - #getFunctionTypes
/// \par
///   The following functions become available if module \alib_strings is included in the \alibbuild:
///   - #TypeName
///   - #typeName
///   - #removeNamespaces
///
/// \par
///   All other functions become available if the module \alib_format is included in the \alibbuild.
///   If in addition symbol \ref ALIB_DEBUG_CONTAINERS is given, then a final function becomes
///   available:
///   - #DumpCustomFunctionHashMapMetrics

/// \see
///   Chapter \ref alib_boxing_more_debug of the
///   \ref alib_mod_boxing "Programmer's Manual" of module \alib_boxing_nl.
//==================================================================================================
namespace debug {

/// Registers a virtual table for debug purposes.
/// This function is invoked internally, when a dynamic \e vtable is created and
/// when a static \e vtable is registered.
///
/// Statically created \e vtables have to be registered during bootstrap in debug-compilations.
/// For this, macro \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER is available, which is empty
/// in release compilations.
///
/// \see
///   Macros ALIB_BOXING_VTABLE_DECLARE and \ref alib_mod_boxing "Programmer's Manual" chapter
///   \ref alib_boxing_more_opt_staticvt.
///
/// @param vtable         The vtable to register.
/// @param productionType Denotes whether the \p{vtable} is a static constexpr object or
///                       dynamically created from template type \b VTableUnoptimized.
ALIB_DLL
void DbgRegisterVTable( detail::VTable* vtable, detail::VTable::DbgFactoryType productionType );

/// Returns all \e vtable singletons that have been created during program execution so far.
/// One \e vtable is created for each mapped type used.
///
/// \see
///   The result can be conveniently passed to #DumpVTables.
/// @return A vector of pointers to objects of type \alib{boxing;detail::VTable}.
ALIB_DLL
std::vector<detail::VTable*>
GetKnownVTables();

/// Returns a vector of \c std::type_info objects, representing all function types that either
/// a default or a type specific implementation has been registered for.<br>
/// A second value in the vector provides the number of invocations of a default implementation.
/// If such is not available, this number is set to <c>std::numeric_limits<uinteger>::max()</c>.
///
/// The list includes the built-in function types.
///
/// \see
///   The result can be conveniently passed to #DumpFunctions.
/// @return A vector of pairs of type information structs and usage numbers.
ALIB_DLL
std::vector<std::pair<const std::type_info*,uinteger>>
GetKnownFunctionTypes();

/// Collects all function declarator types of the given box-function table.
///
/// \see
///   The result can be conveniently passed to #DumpFunctions.
///
/// @param input  The function table to use.
/// @param output The result vector to fill.
ALIB_DLL
void getFunctionTypes( const detail::FunctionTable&                             input,
                       std::vector<std::pair<const std::type_info*,uinteger>>&  output  );

/// Collects all function declarator types, with type-specific implementations.
/// Parameter \p{vtable} might for example be retrieved from a box instance with
/// \alib{boxing;Box::DbgGetVTable}.
///
/// \see
///   The result can be conveniently passed to #DumpFunctions.
/// @param vtable The vtable to get all function implementations for.
/// @return A vector of type information structs.
inline
std::vector<std::pair<const std::type_info*,uinteger>>
GetSpecificFunctionTypes( const detail::VTable* vtable )
{
    std::vector<std::pair<const std::type_info*,uinteger>> result;
    getFunctionTypes( vtable->Functions, result );
    return result;
}

/// Convenience function: invokes #GetSpecificFunctionTypes(const detail::VTable*) with
/// the \e vtable of the box created or passed with the invocation.
///
/// \see
///   The result can be conveniently passed to #DumpFunctions.
///
/// @param box  A box that the vtable is used of.
/// @return A vector of type information structs.
inline
std::vector<std::pair<const std::type_info*,uinteger>>
GetSpecificFunctionTypes( const Box& box )
{ return  GetSpecificFunctionTypes( box.DbgGetVTable() ); }

#if ALIB_STRINGS
/// Implementation of #TypeName.
///
/// \par Availability
///   This function is included only if module \alib_strings is included in the \alibbuild.
///
/// @param      vtable The vtable.
/// @param[out] result The target string to write the type information to.
ALIB_DLL void                 typeName( const detail::VTable* vtable, AString& result );

/// Writes the (demangled) mapped type that the given \p{vtable} represents.
///
/// \par Availability
///   This function is included only if module \alib_strings is included in the \alibbuild.
///
/// @param  vtable           The \e vtable to get the mapped type name for.
/// @return A string containing the requested information.
inline AString                 TypeName  ( const detail::VTable* vtable )
{
    AString result;
    typeName( vtable, result );
    return result;
}

/// Convenience function: invokes #TypeName(const detail::VTable*) with
/// the \e vtable of the box created or passed with the invocation.
///
/// \par Availability
///   This function is included only if module \alib_strings is included in the \alibbuild.
///
/// @param  box   The box to get the mapped type name for.
/// @return A string containing the requested information.
inline AString                 TypeName( const Box& box )
{
    AString result;
    typeName( box.DbgGetVTable(), result );
    return result;
}

/// Removes namespaces in the given \p{string}. The function is used with all debug-functions that
/// create string values containing type names.
/// Note that custom namespaces might be added to namespace variable
/// \alib{boxing;debug::RemovableNamespaces} before invoking any function.
///
/// @param string      The string to remove the namespace from.
/// @param startIndex  The index within the string to start searching for removable namespaces.
/// @return A string containing the requested information.
ALIB_DLL AString&             removeNamespaces( AString& string, integer startIndex  );

/// See function #removeNamespaces. Pre-initialized with <b>"alib::"</b>.
ALIB_DLL extern std::vector<String>  RemovableNamespaces;

#endif // ALIB_STRINGS

}} // namespace [alib::boxing::debug]

#endif // ALIB_DEBUG_BOXING

