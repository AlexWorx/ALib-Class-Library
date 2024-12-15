//==================================================================================================
/// \file
/// This header file is part of the \aliblong.<br>
/// With the inclusion of this header compatibility features between \alib and the C++ standard
/// library are provided.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_COMPATIBILITY_STD_CHARACTERS
#define HPP_ALIB_COMPATIBILITY_STD_CHARACTERS 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

#if ALIB_CHARACTERS

#include "alib/characters/chartraits.hpp"

#include <string>
#include <string_view>
#include <vector>

#if ALIB_STRINGS
#   include "alib/strings/fwds.hpp"
#endif

namespace alib::characters {

#if DOXYGEN
/// This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
/// module \alib_characters_nl.<br>
/// The entities of those namespaces become available with the inclusion of optional "compatibility"
/// headers found in the folder \alibsrcdir{compatibility}.
///
namespace compatibility {

/// This namespace documents compatibility features of \alib_characters_nl and the
/// standard C++ class library found in namespace \c std.
namespace std {
#endif


// ####################################   std::string_view    ######################################
/// Specialization of TMP struct \alib{characters;T_CharArray} for type
/// <c>std::basic_string_view<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The type may be implicitly created from character array data.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types enabled by TMP helper-struct \alib{characters;TT_IsChar}.
template<typename TChar>
struct     T_CharArray<std::basic_string_view<TChar>, TChar, typename std::enable_if<
 TT_IsChar<TChar>::value                                                            >::type >
{
  #if !DOXYGEN
    static constexpr AccessType          Access       =       AccessType::Implicit;
    static constexpr ConstructionType    Construction = ConstructionType::Implicit;
    static constexpr const TChar*        Buffer   (std::basic_string_view<TChar> const & src) { return                       src.data  ()  ; }
    static constexpr integer             Length   (std::basic_string_view<TChar> const & src) { return static_cast<integer>( src.length() ); }
    static std::basic_string_view<TChar> Construct(const TChar* array, integer length       ) { return std::basic_string_view<TChar>( array, size_t(length) ); }
  #endif
};


/// Specialization of TMP struct \alib{characters;T_ZTCharArray} for type
/// <c>std::basic_string_view<TChar></c>:
/// - Zero-terminated string data is allowed to be explicitly accessed as usually data represented
///   by type \c std::string_view is not zero-terminated.
/// - The type may be implicitly created from zero-terminated character arrays.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types enabled by TMP helper-struct \alib{characters;TT_IsChar}.
template<typename TChar>
struct     T_ZTCharArray<std::basic_string_view<TChar>, TChar, typename std::enable_if<
 TT_IsChar<TChar>::value                                                            >::type >
{
  #if !DOXYGEN
    static constexpr AccessType          Access       =       AccessType::ExplicitOnly;
    static constexpr ConstructionType    Construction = ConstructionType::Implicit;
    static constexpr const TChar*        Buffer   (std::basic_string_view<TChar> const & src ) { return                       src.data  ()  ; }
    static constexpr integer             Length   (std::basic_string_view<TChar> const & src ) { return static_cast<integer>( src.length() ); }
    static std::basic_string_view<TChar> Construct(const TChar* array, integer length        ) { return std::basic_string_view<TChar>( array, size_t(length) ); }
  #endif
};


// ####################################   std::string         ######################################

/// Specialization of TMP struct \alib{characters;T_CharArray} for type
/// <c>std::basic_string<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from character arrays is defined to be allowed in explicit fashion only,
///   because \c std::string is a heavy-weight string type which will copy the data to an allocated
///   buffer.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types enabled by TMP helper-struct \alib{characters;TT_IsChar}.
template<typename TChar>
struct     T_CharArray<std::basic_string<TChar>, TChar, typename std::enable_if<
 TT_IsChar<TChar>::value                                                            >::type >
{
  #if !DOXYGEN
    static constexpr AccessType                 Access       =       AccessType::Implicit;
    static constexpr ConstructionType           Construction = ConstructionType::ExplicitOnly;
    static constexpr const TChar*               Buffer   ( std::basic_string<TChar> const &  src ) { return                       src.data  ()  ; }
    static constexpr integer                    Length   ( std::basic_string<TChar> const &  src ) { return static_cast<integer>( src.length() ); }
    static constexpr std::basic_string<TChar>   Construct( const TChar* array, integer length    ) { return std::basic_string<TChar>( array, size_t(length) ); }
  #endif
};


/// Specialization of TMP struct \alib{characters;T_ZTCharArray} for type
/// <c>std::basic_string<TChar></c>:
/// - Zero-terminated character string data is allowed to be implicitly accessed, because the
///   type's buffer access method \c data() returns zero-terminated strings and is defined \c const.
/// - The type may be created from character array data in an explicit fashion only, because it is a
///   is a heavy-weight string type which will copy the data to an allocated buffer.
///
/// \note
///   In combination with classes \alib{strings;TCString;CString} and \alib{strings;TAString;AString},
///   explicit creation is suppressed using TMP struct \alib{strings;T_SuppressAutoCast}, because
///   otherwise an ambiguity would occur due to their ability to implicitly cast to
///   <c>const char*</c>, which implicitly constructs \c std::string in turn.
///   This leads to the bad situation that an explicit construction like this:
///
///             std::string stdString( cString );
///
///   uses the implicit cast to <c>const char*</c> and with that constructs the \c std::string.
///   This is of course very inefficient, as the length of the string has to be determined
///   internally.
///
///   The most efficient way to create a \c std::string object from an object of type \b CString
///   or \b AString is to use the explicit constructor:
///
///             std::string stdString( String.Buffer(), String.Length() );
///
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types enabled by TMP helper-struct \alib{characters;TT_IsChar}.
template<typename TChar>
struct     T_ZTCharArray<std::basic_string<TChar>, TChar, typename std::enable_if<
 TT_IsChar<TChar>::value                                                            >::type >
{
  #if !DOXYGEN
    static constexpr AccessType               Access       =       AccessType::Implicit;
    static constexpr ConstructionType         Construction = ConstructionType::ExplicitOnly;
    static constexpr const TChar*             Buffer   ( std::basic_string<TChar> const &  src ) { return                       src.data  ()  ; }
    static constexpr integer                  Length   ( std::basic_string<TChar> const &  src ) { return static_cast<integer>( src.length() ); }
    static constexpr std::basic_string<TChar> Construct( const TChar* array, integer length    ) { return std::basic_string<TChar>( array, size_t(length) ); }
  #endif
};


// ####################################   std::vector<char>   ######################################
/// Specialization of TMP struct \alib{characters;T_CharArray} for type <c>std::vector<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from character arrays is defined to be allowed in explicit fashion only,
///   because \c std::vector is a heavy-weight type which will copy the data to an allocated
///   buffer.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types enabled by TMP helper-struct \alib{characters;TT_IsChar}.
template<typename TChar>
struct     T_CharArray<std::vector<TChar>, TChar, typename std::enable_if<
 TT_IsChar<TChar>::value                                                  >::type >
{
    #if !DOXYGEN
        static constexpr AccessType          Access       =       AccessType::Implicit;
        static constexpr ConstructionType    Construction = ConstructionType::ExplicitOnly;
        static constexpr const TChar*        Buffer   (std::vector<TChar> const & src) { return                       src.data()  ; }
        static constexpr integer             Length   (std::vector<TChar> const & src) { return static_cast<integer>( src.size() ); }
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        static std::vector<TChar>            Construct(const TChar* array, integer length )
        {
            std::vector<TChar> result;
            result.reserve( size_t(length) );
            const TChar* end= array + length;
            while( array < end )
                result.push_back( *array++ );
            return  result;
        }
        ALIB_WARNINGS_RESTORE
    #endif
};

/// Specialization of TMP struct \alib{characters;T_ZTCharArray} for type <c>std::vector<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from zero-terminated character arrays is defined to be allowed in explicit
///   fashion only, because \c std::vector is a heavy-weight type which will copy the data to an
///   allocated buffer.<br>
///   Note that the zero-termination character is not included in the vector when created from
///   a zero-terminated character array. The length of the vector will have the lengh of the
///   source string.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types enabled by TMP helper-struct \alib{characters;TT_IsChar}.
template<typename TChar>
struct     T_ZTCharArray<std::vector<TChar>, TChar, typename std::enable_if<
 TT_IsChar<TChar>::value                                                    >::type >
{
  #if !DOXYGEN
    static constexpr AccessType          Access       =       AccessType::Implicit;
    static constexpr ConstructionType    Construction = ConstructionType::ExplicitOnly;
    static constexpr const TChar*        Buffer   (std::vector<TChar> const & src) { return                       src.data()  ; }
    static constexpr integer             Length   (std::vector<TChar> const & src) { return static_cast<integer>( src.size() ); }
    static constexpr std::vector<TChar>  Construct(const TChar* array, integer length )
    {
        std::vector<TChar> result;
        result.reserve( size_t(length) );
        const TChar* end= array + length;
        while( array < end )
            result.push_back( *array++ );
        return  result;
    }
  #endif
};

#if DOXYGEN
}} // namespace alib::characters[::compatibility::std]
#endif

} // namespace [alib::characters]


// Suppress conversion of CString and AString to std::string. This rationale for this is
// documented with T_ZTCharArray<std::basic_string> above.
#if ALIB_STRINGS && !DOXYGEN

namespace alib {  namespace strings {
  template<typename TChar>                   struct T_SuppressAutoCast<TCString<TChar>       , characters::ConstructionType::ExplicitOnly, std::basic_string<TChar> > : public std::true_type {};
  template<typename TChar, typename TAlloc>  struct T_SuppressAutoCast<TAString<TChar,TAlloc>, characters::ConstructionType::ExplicitOnly, std::basic_string<TChar> > : public std::true_type {};
}}

#endif



#endif // ALIB_CHARACTERS
#endif // HPP_ALIB_COMPATIBILITY_STD_CHARACTERS

