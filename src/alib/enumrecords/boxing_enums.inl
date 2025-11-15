//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumrecords of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_BOXING
ALIB_EXPORT namespace alib::boxing {

#include "ALib.Lang.CIFunctions.H"

/// Returns the \ref alib_enums_records "ALib Enum Record" associated with the given instance of
/// class \alib{boxing;Enum}, which is defined in module \alib_boxing.
///
/// In debug-compilations an \alib_assertion is raised if no enum record was defined for
/// the enumeration element represented by this instance.
///
/// \see
/// - Namespace function \alib{enumrecords::GetRecord} of the module \alib_enumrecords, which
///   provides the standard way of accessing enum records for enum elements known at compile-time.
/// - Sibling method #TryRecord.
///
/// @tparam TRecord  The enumeration record type associated with the enum type.
///                  This has to be explicitly provided.
///                  It is the caller's obligation to ensure that the requested type equals
///                  the one associated. Otherwise, this method produces undefined behavior.
/// @param  e        The enumeration element, boxed in class \b %Enum.
/// @return The record that is associated with this enumeration element.
template<typename TRecord>
const TRecord&  GetRecord(Enum e) {
    const void* result= enumrecords::detail::getEnumRecord( e.TypeID(), e.Integral() );
    ALIB_ASSERT_ERROR( result != nullptr, "BOXING", "Enum Record for type <{}>({}) not found.",
        &e.TypeID(), e.Integral()  )

    return *reinterpret_cast<const TRecord*>( result );
}

/// Returns a pointer to the \ref alib_enums_records "ALib Enum Record" associated with this
/// enumeration element represented by this instance.
/// If no enum record was is defined, \c nullptr is returned.
///
/// \see
/// - Namespace function \alib{enumrecords::TryRecord} of module \alib_enumrecords, which provides the
///   standard way of accessing enum records for enum elements known at compile-time.
/// - Sibling method #GetRecord.
///
/// @tparam TRecord  The enumeration record type associated with the enum type.
///                  This has to be explicitly provided.
///                  It is the caller's obligation to ensure that the requested type equals
///                  the one associated. Otherwise this method produces undefined behavior.
/// @param  e        The enumeration element, boxed in class \b %Enum.
///
/// @return A pointer to the record that is associated with this enumeration element,
///         respectively \c nullptr if no record was found.
template<typename TRecord>
const TRecord*  TryRecord(Enum e){
    return reinterpret_cast<const TRecord*>( enumrecords::detail::getEnumRecord( e.TypeID(),
                                                                                 e.Integral() ) );
}

#include "ALib.Lang.CIMethods.H"

} // namespace [alib::boxing]
#endif //ALIB_BOXING
