//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_STRINGS_STDSTRINGS
#define H_ALIB_STRINGS_STDSTRINGS
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_STRINGS
#include <string>
#include <string_view>
#if __has_include(<format>)
#   include <format>
#else
#   include <fmt/format.h>
#endif

#include <span>
#include <array>
#include <vector>
#include "ALib.Lang.H"
#include "ALib.Strings.H"



//##################################################################################################
//###################################### ArrayTraits<std::xyz> #####################################
//##################################################################################################

namespace alib::characters {
#if DOXYGEN
/// This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
/// module \alib_characters_nl.<br>
/// The entities of those namespaces become available with the inclusion of specific headers
/// that import a certain C++20 Module or inject the functionality into a namespace in a
/// traditional fashion, for example, header \implude{Compatibility.StdStrings}.
namespace compatibility {

/// This namespace documents compatibility features of \alib_characters_nl and the
/// standard C++ class library found in namespace \c std.
namespace std {
#endif

//######################################### std::string_view #######################################
/// Specialization of the type trait \alib{characters;ArrayTraits} for type
/// <c>std::basic_string_view<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The type may be implicitly created from character array data.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar>
requires IsCharacter<TChar>
struct     ArrayTraits<std::basic_string_view<TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                               = Policy::Implicit;
    static constexpr Policy              Construction                                         = Policy::Implicit;
    static constexpr const TChar*        Buffer   (std::basic_string_view<TChar> const & src) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::basic_string_view<TChar> const & src) { return integer( src.length() ); }
    constexpr static std::basic_string_view<TChar> Construct(const TChar* array, integer length       ) { return std::basic_string_view<TChar>( array, size_t(length) ); }
  #endif
};

/// Specialization of the type trait \alib{characters;ArrayTraits} for type
/// <c>std::basic_string_view<char8_t></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The type may be implicitly created from character array data.
template<>
struct     ArrayTraits<std::basic_string_view<char8_t>, nchar>
{
  #if !DOXYGEN
    static constexpr Policy                Access                                               = Policy::Implicit;
    static constexpr Policy                Construction                                         = Policy::Implicit;
    static           const nchar*          Buffer   (std::basic_string_view<char8_t> const & src) { return reinterpret_cast<const nchar*>(src.data  ())  ; }
    static constexpr integer               Length   (std::basic_string_view<char8_t> const & src) { return integer( src.length() ); }
    static std::basic_string_view<char8_t> Construct(const nchar* array, integer length       )
    { return std::basic_string_view<char8_t>( reinterpret_cast<const char8_t*>(array), size_t(length) ); }
  #endif
};

/// Specialization of the type trait \alib{characters;ZTArrayTraits} for type
/// <c>std::basic_string_view<TChar></c>:
/// - Zero-terminated string data is allowed to be explicitly accessed as usually data represented
///   by type \c std::string_view is not zero-terminated.
/// - The type may be implicitly created from zero-terminated character arrays.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar>
requires IsCharacter<TChar>
struct   ZTArrayTraits<std::basic_string_view<TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                                = Policy::ExplicitOnly;
    static constexpr Policy              Construction                                          = Policy::Implicit;
    static constexpr const TChar*        Buffer   (std::basic_string_view<TChar> const & src ) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::basic_string_view<TChar> const & src ) { return integer( src.length() ); }
    static constexpr std::basic_string_view<TChar> Construct(const TChar* array, integer length  ) { return std::basic_string_view<TChar>( array, size_t(length) ); }
  #endif
};

/// Specialization of the type trait \alib{characters;ZTArrayTraits} for type
/// <c>std::basic_string_view<char8_t></c>:
/// - Zero-terminated string data is allowed to be explicitly accessed as usually data represented
///   by type \c std::string_view is not zero-terminated.
/// - The type may be implicitly created from zero-terminated character arrays.
template<>
struct   ZTArrayTraits<std::basic_string_view<char8_t>, nchar>
{
  #if !DOXYGEN
    static constexpr Policy                Access                                                = Policy::ExplicitOnly;
    static constexpr Policy                Construction                                          = Policy::Implicit;
    static           const nchar*          Buffer   (std::basic_string_view<char8_t> const & src ) { return reinterpret_cast<const nchar*>(src.data  ())  ; }
    static constexpr integer               Length   (std::basic_string_view<char8_t> const & src ) { return integer( src.length() ); }
    static std::basic_string_view<char8_t> Construct(const nchar* array, integer length  )
    { return std::basic_string_view<char8_t>( reinterpret_cast<const char8_t*>(array), size_t(length) ); }
  #endif
};

//########################################### std::string ##########################################

/// Specialization of the type trait \alib{characters;ArrayTraits} for type
/// <c>std::basic_string<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from character arrays is defined to be allowed in explicit fashion only,
///   because \c std::string is a heavy-weight string type that will copy the data to an allocated
///   buffer.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar>
requires IsCharacter<TChar>
struct     ArrayTraits<std::basic_string<TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy                     Access                                             = Policy::Implicit;
    static constexpr Policy                     Construction                                       = Policy::ExplicitOnly;
    static constexpr const TChar*               Buffer   ( std::basic_string<TChar> const &  src ) { return          src.data  ()  ; }
    static constexpr integer                    Length   ( std::basic_string<TChar> const &  src ) { return integer( src.length() ); }
    static constexpr std::basic_string<TChar>   Construct( const TChar* array, integer length    ) { return std::basic_string<TChar>( array, size_t(length) ); }
  #endif
};

/// Specialization of the type trait \alib{characters;ArrayTraits} for type
/// <c>std::basic_string<char8_t></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from character arrays is defined to be allowed in explicit fashion only,
///   because \c std::string is a heavy-weight string type that will copy the data to an allocated
///   buffer.
template<>
struct     ArrayTraits<std::basic_string<char8_t>, nchar>
{
  #if !DOXYGEN
    static constexpr Policy             Access                                             = Policy::Implicit;
    static constexpr Policy             Construction                                       = Policy::ExplicitOnly;
    static           const nchar*       Buffer   ( std::basic_string<char8_t> const &  src ) { return reinterpret_cast<const nchar*>(src.data()); }
    static constexpr integer            Length   ( std::basic_string<char8_t> const &  src ) { return integer( src.length() ); }
    static std::basic_string<char8_t>   Construct( const nchar* array, integer length    )
    { return std::basic_string<char8_t>( reinterpret_cast<const char8_t*>(array), size_t(length) ); }
  #endif
};



/// Specialization of the type trait \alib{characters;ZTArrayTraits} for type
/// <c>std::basic_string<TChar></c>:
/// - Zero-terminated character string data is allowed to be implicitly accessed, because the
///   type's buffer access method \c data() returns zero-terminated strings and is defined \c const.
/// - The type may be created from character array data in an explicit fashion only, because it is a
///   heavy-weight string type that will copy the data to an allocated buffer.
///
/// \note
///   In combination with classes \alib{strings;TCString;CString} and \alib{strings;TAString;AString},
///   explicit creation is suppressed using the type trait \alib{strings;NoAutoCastTraits}, because
///   otherwise an ambiguity would occur due to their ability to implicitly cast to
///   <c>const char*</c>, which implicitly constructs \c std::string in turn.
///   This leads to the bad situation that an explicit construction like this:
///
///             std::string stdString( cString );
///
///   uses the implicit cast to <c>const char*</c> and with that constructs the \c std::string.
///   This would be inefficient, as the length of the string has to be determined internally.
///
///   The most efficient way to create a \c std::string object from an object of type \b CString
///   or \b AString is to use the explicit constructor:
///
///             std::string stdString( String.Buffer(), String.Length() );
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar>
requires IsCharacter<TChar>
struct     ZTArrayTraits<std::basic_string<TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy                   Access                                             = Policy::Implicit;
    static constexpr Policy                   Construction                                       = Policy::ExplicitOnly;
    static constexpr const TChar*             Buffer   ( std::basic_string<TChar> const &  src ) { return          src.data  ()  ; }
    static constexpr integer                  Length   ( std::basic_string<TChar> const &  src ) { return integer( src.length() ); }
    static constexpr std::basic_string<TChar> Construct( const TChar* array, integer length    ) { return std::basic_string<TChar>( array, size_t(length) ); }
  #endif
};


//######################################## std::vector<char> #######################################
/// Specialization of the type trait \alib{characters;ArrayTraits} for type <c>std::vector<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from character arrays is defined to be allowed in explicit fashion only,
///   because \c std::vector is a heavy-weight type which will copy the data to an allocated
///   buffer.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar>
requires IsCharacter<TChar>
struct     ArrayTraits<std::vector<TChar>, TChar>
{
    #if !DOXYGEN
        static constexpr Policy              Access                                    = Policy::Implicit;
        static constexpr Policy              Construction                              = Policy::ExplicitOnly;
    static constexpr const TChar*        Buffer   (std::vector<TChar> const & src) { return          src.data()  ; }
    static constexpr integer             Length   (std::vector<TChar> const & src) { return integer( src.size() ); }
    static std::vector<TChar>            Construct(const TChar* array, integer length ) {
        std::vector<TChar> result;
        result.reserve( size_t(length) );
        const TChar* end= array + length;
        while( array < end )
            result.push_back( *array++ );
        return  result;
    }
    #endif
};

/// Specialization of the type trait \alib{characters;ZTArrayTraits} for type
/// <c>std::vector<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The construction from zero-terminated character arrays is defined to be allowed in explicit
///   fashion only, because \c std::vector is a heavy-weight type which will copy the data to an
///   allocated buffer.<br>
///   Note that the zero-termination character is not included in the vector when created from
///   a zero-terminated character array. The length of the vector will have the lengh of the
///   source string.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar>
requires IsCharacter<TChar>
struct ZTArrayTraits<std::vector<TChar>, TChar >
{
  #if !DOXYGEN
    static constexpr Policy              Access                                    = Policy::Implicit;
    static constexpr Policy              Construction                              = Policy::ExplicitOnly;
    static constexpr const TChar*        Buffer   (std::vector<TChar> const & src) { return          src.data()  ; }
    static constexpr integer             Length   (std::vector<TChar> const & src) { return integer( src.size() ); }
    static constexpr std::vector<TChar>  Construct(const TChar* array, integer length ) {
        std::vector<TChar> result;
        result.reserve( size_t(length) );
        const TChar* end= array + length;
        while( array < end )
            result.push_back( *array++ );
        return  result;
    }
  #endif
};

//############################################ std::array ##########################################
/// Specialization of the type trait \alib{characters;ArrayTraits} for type
/// <c>std::array<TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The type may be implicitly created from character array data.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar, size_t TLength>
requires IsCharacter<TChar>
struct     ArrayTraits<std::array<TChar,TLength>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                            = Policy::Implicit;
    static constexpr Policy              Construction                                      = Policy::NONE;
    static constexpr const TChar*        Buffer   (std::array<TChar, TLength> const & src) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::array<TChar, TLength> const & src) { return integer( src.size() ); }
  #endif
};


/// Specialization of the type trait \alib{characters;ZTArrayTraits} for type
/// <c>std::array<TChar></c>:
/// - Zero-terminated string data is allowed to be explicitly accessed as usually data represented
///   by type \c std::string_view is not zero-terminated.
/// - The type may be implicitly created from zero-terminated character arrays.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar, size_t TLength>
requires IsCharacter<TChar>
struct   ZTArrayTraits<std::array<TChar, TLength>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                             = Policy::ExplicitOnly;
    static constexpr Policy              Construction                                       = Policy::NONE;
    static constexpr const TChar*        Buffer   (std::array<TChar, TLength> const & src ) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::array<TChar, TLength> const & src ) { return integer( src.size() ); }
  #endif
};

//############################################ std::span ###########################################
/// Specialization of the type trait \alib{characters;ArrayTraits} for type
/// <c>std::span<const TChar></c>:
/// - Character array data (string data) is allowed to be implicitly accessed.
/// - The type may be implicitly created from character array data.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar>
requires IsCharacter<TChar>
struct     ArrayTraits<std::span<const TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                         = Policy::Implicit;
    static constexpr Policy              Construction                                   = Policy::Implicit;
    static constexpr const TChar*        Buffer   (std::span<const TChar> const & src)  { return          src.data  ()  ; }
    static constexpr integer             Length   (std::span<const TChar> const & src)  { return integer( src.size() ); }
    static std::span<const TChar>        Construct(const TChar* array, integer length ) { return std::span<const TChar>( array, size_t(length) ); }
  #endif
};


/// Specialization of the type trait \alib{characters;ZTArrayTraits} for type
/// <c>std::span<const TChar></c>:
/// - Zero-terminated string data is allowed to be explicitly accessed as usually data represented
///   by type \c std::span is not zero-terminated.
/// - The type may be implicitly created from zero-terminated character arrays.
///
/// @tparam TChar Template parameter providing the underlying character type.
///               Restricted to types that satisfy concept \alib{characters;IsCharacter}.
template<typename TChar>
requires IsCharacter<TChar>
struct   ZTArrayTraits<std::span<const TChar>, TChar>
{
  #if !DOXYGEN
    static constexpr Policy              Access                                         = Policy::ExplicitOnly;
    static constexpr Policy              Construction                                   = Policy::Implicit;
    static constexpr const TChar*        Buffer   (std::span<const TChar> const & src ) { return          src.data  ()  ; }
    static constexpr integer             Length   (std::span<const TChar> const & src ) { return integer( src.size() ); }
    static std::span<const TChar> Construct(const TChar* array, integer length        ) { return std::span<const TChar>( array, size_t(length) ); }
  #endif
};


#if DOXYGEN
}} // namespace alib::characters[::compatibility::std]
#endif

} // namespace [alib::characters]


// Suppress conversion of CString and AString to std::string. This rationale for this is
// documented with ZTArrayTraits<std::basic_string> above.
#if !DOXYGEN
namespace alib {  namespace strings {
template<typename TChar>                   struct NoAutoCastTraits<TCString<TChar>       , characters::Policy::ExplicitOnly, std::basic_string<TChar> > : std::true_type {};
template<typename TChar, typename TAlloc>  struct NoAutoCastTraits<TAString<TChar,TAlloc>, characters::Policy::ExplicitOnly, std::basic_string<TChar> > : std::true_type {};
}}
#endif



//##################################################################################################
//################################ std::formatter<alib::strings::xyz> ##############################
//##################################################################################################

#if DOXYGEN
namespace alib::strings::compatibility::std {
#else
#   if __has_include(<format>)
namespace std {
#   else
namespace fmt {
#   endif
#endif


/// Standard formatter specialization for \alib{strings;TString} for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter \ref alib_strings_stdformat of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar>
struct formatter<alib::strings::TString<TChar>, TChar>
  : formatter<std::basic_string_view<TChar>, TChar> {
// No need to override parse() since the base class does the work

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TString<TChar>& str, TContext& ctx)                     const {
        // Delegate formatting to the std::basic_string_view<TChar> formatter
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

/// Standard formatter specialization for \alib{strings;TCString} for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter \ref alib_strings_stdformat of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar>
struct formatter<alib::strings::TCString<TChar>, TChar>
  : formatter<std::basic_string_view<TChar>, TChar> {
// No need to override parse() since the base class does the work

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TCString<TChar>& str, TContext& ctx)                    const {
        // Delegate formatting to the std::basic_string_view<TChar> formatter
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

/// Standard formatter specialization for \alib{strings;TAString} for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter \ref alib_strings_stdformat of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar, typename TAllocator>
struct formatter<alib::strings::TAString<TChar, TAllocator>, TChar>
  : formatter<std::basic_string_view<TChar>, TChar> {

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TAString<TChar, TAllocator>& str, TContext& ctx)        const {
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

/// Standard formatter specialization for \alib{strings;TSubstring} for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter \ref alib_strings_stdformat of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar>
struct formatter<alib::strings::TSubstring<TChar>, TChar>
  : formatter<std::basic_string_view<TChar>, TChar> {
// No need to override parse() since the base class does the work

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TSubstring<TChar>& str, TContext& ctx)                  const {
        // Delegate formatting to the std::basic_string_view<TChar> formatter
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

/// Standard formatter specialization for \alib{strings;TLocalString} for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter \ref alib_strings_stdformat of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar, alib::integer TCapacity, typename TAllocator>
struct formatter<alib::strings::TLocalString<TChar,TCapacity,TAllocator>, TChar>
  : formatter<std::basic_string_view<TChar>, TChar> {
// No need to override parse() since the base class does the work

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TLocalString<TChar,TCapacity,TAllocator>& str,
                TContext&                                                      ctx )         const {
        // Delegate formatting to the std::basic_string_view<TChar> formatter
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

} // namespace [std]

//##################################################################################################
//################################# std::formatter<alib::Appendable> ###############################
//##################################################################################################

#if DOXYGEN
namespace alib::strings::APPENDABLES {
#else
namespace alib::strings {
#endif

/// For this simple templated wrapper struct, a generic specialization of type traits
/// \c std::formatter exists if template type \p{TAppendable} has a specialization of
/// \alib{strings;AppendableTraits}.
/// In other words, all types which have been \ref alib_strings_assembly_ttostring "made appendable"
/// to class \alib{strings;TAString}, can be wrapped in this type and then used as
/// arguments of function \c std::format.
///
/// \note
///   Unfortunately, it is not possible with all compilers to generically specialize
///   \c std::formatter for all appendable types. With some compilers, this leads to an
///   unresolvable ambiguity for types which have both, a specialization for \c std::formatter
///   and for \b %AppendableTraits. Therefore, this wrapper has to be used.
///
/// @see Chapter \ref alib_strings_stdformat of the Programmer's Manual of the module
///      \alib_strings_nl.
///
/// @tparam TAppendable  The type of the wrapped appendable.
template<typename TAppendable>
struct Appendable {
    const TAppendable&    appendable; ///< A reference to the wrapped appendable.

    /// Constructor.
    /// @param pAppendable The wrapped appendable.
    Appendable(const TAppendable& pAppendable)                         : appendable{pAppendable}  {}
};

/// C++17 Deduction Guide to construct the type \alib{strings::APPENDABLES;Appendable}.
/// @tparam TAppendable  The type of the wrapped appendable.
template<typename TAppendable>
Appendable(const TAppendable& ) -> Appendable<TAppendable>                                        ;


} // namespace [alib::strings[::APPENDABLES]]

#if DOXYGEN
namespace alib::strings::compatibility::std {
#else
# if __has_include(<format>)
namespace std {
# else
namespace fmt {
# endif
#endif


/// This specialization of type traits \c std::formatter enables to format wrapper type
/// \alib{strings::APPENDABLES;Appendable}, for all types that have a specialization of
/// \alib{strings;AppendableTraits}.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
///
/// @see
///   - Wrapper type \alib{strings::APPENDABLES;Appendable}
///   - Chapter \ref alib_strings_stdformat of the Programmer's Manual of the module
//      \alib_strings_nl.
/// @tparam TAppendable The type of the value wrapped in type \b %Appendable.
/// @tparam TChar       The character type used with the formatting operation.
template <typename TAppendable, typename TChar>
requires alib::strings::IsAppendable<TAppendable, TChar, alib::lang::HeapAllocator>
struct formatter<alib::strings::Appendable<TAppendable>, TChar>
 : formatter<std::basic_string_view<TChar>, TChar> {
    /// Creates an intermediate local string (avoiding dynamic allocation in most cases),
    /// appends the value wrapped in \p{wrapper} to it, and then delegates formatting to the
    /// \c std::basic_string_view<TChar> formatter.
    /// @param wrapper The wrapper containing the appendable to append.
    /// @param ctx     The formatting context.
    /// @return An end iterator of the output range.
    template <typename FormatContext>
    auto format(const alib::strings::Appendable<TAppendable>&  wrapper,
                FormatContext&                                 ctx)                          const {
        alib::strings::TLocalStringNoWarning<TChar, 256, alib::lang::HeapAllocator> buf;
        buf.Append(wrapper.appendable);

        return formatter<std::basic_string_view<TChar>, TChar>::format(
                         static_cast<std::basic_string_view<TChar>>(buf), ctx);
    }
};

} // namespace [std], for doxygen it is [alib::strings::compatibility::std]

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
namespace alib::strings::APPENDABLES {
#else
namespace alib::strings {
#endif

/// This struct enables the use of C++20 function \c std::format - and with that any adaption
/// of custom types - to be directly appendable to class \alib{strings;TAString}, without the
/// need of creating an intermediate temporary \c std::string.
///
/// For that, this struct stores the format string and a tuple of decayed formatting arguments.
/// With a corresponding specialization of struct \alib{strings;AppendableTraits}, the formatted
/// output is produced using C++20's \c std::vformat_to and a <c>std::back_insert_iterator</c>
/// that appends characters directly to an \b %AString.
///
/// \note
///   C++20 supports formatting only on character types \c char and \c wchar_t.
///   With that, the specializations of \alib{strings;AppendableTraits} are likewise
///   only defined for these concrete character types. If you compile \alib to switch
///   the default character type, for example, by using compiler symbol
///   \ref ALIB_CHARACTERS_SIZEOF_WCHAR to change the default size
///   (which is defined by the compiler !), then appending this type \c TStdFormat might
///   not be available.
///
/// \note
///   For the same reason, the alias definitions of this struct, namely
///   - alib::StdFormat,
///   - alib::NStdFormat, and
///   - alib::WStdFormat
///
/// \note
///   use these concrete character types, instead of the "logical" types \ref alib::nchar and
///   \ref alib::wchar.
///
/// @see Chapter \ref alib_strings_stdformat of the Programmer's Manual of the module
///      \alib_strings_nl.
///
/// @tparam TChar  The character type of the \b %TAString and the format string.
/// @tparam TArgs  Variadic template parameters representing the types of the formatting arguments.
template<typename TChar, typename... TArgs>
struct TStdFormat {
    /// The format string given construction. Will be passed to <c>std::format</c>.
    TString<TChar> format;

    /// The variadic arguments given construction. Will be passed to <c>std::format</c>.
    std::tuple<std::decay_t<TArgs>...> arguments;

    /// Constructor. Uses perfect forwarding and stores decayed copies of the arguments.
    /// @param formatString Stored in #format.
    /// @param args         Stored in #arguments.
    TStdFormat(const TString<TChar>& formatString, TArgs&&... args)
    : format(formatString)
    , arguments(std::make_tuple(std::forward<TArgs>(args)...))                                    {}
};


/// C++17 Deduction Guide to construct the type \alib{strings::APPENDABLES;TStdFormat}.
/// @tparam TChar The character type of the target \c std::string.
/// @tparam TArgs The types of the variadic formatter arguments.
template<typename TChar, typename... TArgs>
TStdFormat(TString<TChar>, TArgs&&...) -> TStdFormat<TChar, TArgs...>;

/// Specialization of the functor \alib{strings;AppendableTraits} for the type
/// \alib{strings::APPENDABLES;TStdFormat;TStdFormat<char>}.
template<typename TAllocator, typename... Args>
struct  AppendableTraits<TStdFormat<char, Args...> ,char,TAllocator>
{
    /// The functor operator.
    /// @param target     The target string-buffer.
    /// @param fmtpackage The packaged argumentds to \c std::format.
    void operator()( TAString<char,TAllocator>&         target,
                     const TStdFormat<char, Args...>&   fmtpackage ) {
        std::back_insert_iterator<TAString<char, TAllocator>> it(target);

        #if __has_include(<format>)
            namespace f_temp=std;
        #else
            namespace f_temp=fmt;
        #endif

        // Use std::apply to expand the tuple and pass each argument to make_format_args.
        std::apply( [&](auto&&... a) {
                        f_temp::vformat_to(it, fmtpackage.format,
                        f_temp::make_format_args(static_cast<const decltype(a)&>(a)...));
                    },
                    fmtpackage.arguments  );
    }
};

/// Specialization of the functor \alib{strings;AppendableTraits} for the type
/// \alib{strings::APPENDABLES;TStdFormat;TStdFormat<wchar_t>}.
template<typename TAllocator, typename... Args>
struct  AppendableTraits<TStdFormat<wchar_t, Args...> ,wchar_t,TAllocator>
{
    /// The functor operator.
    /// @param target     The target string-buffer.
    /// @param fmtpackage The packaged argumentds to \c std::format.
    void operator()( TAString<wchar_t,TAllocator>&       target,
                     const TStdFormat<wchar_t, Args...>& fmtpackage ) {
        std::back_insert_iterator<TAString<wchar_t, TAllocator>> it(target);

        // Use std::apply to expand the tuple and pass each argument to make_format_args.
        std::apply(
            [&](auto&&... a) {
                // Cast each argument to a const lvalue reference.
                vformat_to(it, fmtpackage.format,
                           make_wformat_args(static_cast<const decltype(a)&>(a)...));
            },
            fmtpackage.arguments
        );
    }
};

} // namespace [alib::strings]

namespace alib {

    /// Type alias in namespace \b alib.
    template<typename... Args>
    using StdFormat= strings::TStdFormat<character, Args...>;

    /// Type alias in namespace \b alib.
    template<typename... Args>
    using NStdFormat= strings::TStdFormat<char, Args...>;

    /// Type alias in namespace \b alib.
    template<typename... Args>
    using WStdFormat= strings::TStdFormat<wchar_t, Args...>;

    /// Type alias in namespace \b alib.
    template<typename TAppendable>
    using Appendable=  strings::Appendable<TAppendable>;
}


#endif // ALIB_STRINGS
#endif // H_ALIB_STRINGS_STDSTRINGS
