//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
/// With supporting legacy or module builds, .mpp-files are either recognized by the build-system
/// as C++20 Module interface files, or are included by the
/// \ref alib_manual_modules_impludes "import/include headers".
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_STRINGS

ALIB_EXPORT namespace alib::strings {
// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
namespace APPENDABLES {
#endif

//##################################################################################################
// AppendableTraits<boxing::Enum>
//##################################################################################################
#if ALIB_ENUMRECORDS
/// Specialization of functor \alib{strings;AppendableTraits} for type \alib{boxing;Enum}.
/// @tparam TChar      The character type of the target \b %Astring.
/// @tparam TAllocator The allocator that the target \b %AString uses, as prototyped
///                    with \alib{lang;Allocator}.
template<typename TChar, typename TAllocator>
struct AppendableTraits<boxing::Enum, TChar,TAllocator>
{
    /// Writes the given boxed object. This is done by invoking box-function
    /// \alib{boxing;FAppend} on box \p{value}.
    ///
    /// @param target  The \b AString that \b Append was invoked on.
    /// @param value   The enum-box to its contents to \p{target}.
    void operator()( TAString<TChar,TAllocator>& target, const boxing::Enum& value ) {
        value.CastToBox().Call<boxing::FAppend<TChar,TAllocator>>( target );
    }
};
#endif // ALIB_ENUMRECORDS

//##################################################################################################
// AppendableTraits<Box>
//##################################################################################################

/// Specialization of template functor \alib{strings;AppendableTraits} for type \alib{boxing;Box}.
/// @tparam TChar      The character type of the target \b %Astring.
/// @tparam TAllocator The allocator that the target \b %AString uses, as prototyped with
///                    \alib{lang;Allocator}.
template<typename TChar, typename TAllocator>  struct AppendableTraits<boxing::Box, TChar, TAllocator>
{
    /// Writes the given boxed object. This is done by invoking box-function
    /// \alib{boxing;FAppend} on box \p{value}.
    ///
    /// @param target  The \b AString that \b Append was invoked on.
    /// @param box     The box to its contents to \p{target}.
    void operator()( TAString<TChar,TAllocator>& target, const boxing::Box& box )
    { box.Call<boxing::FAppend<TChar,TAllocator>>( target ); }
};

#if DOXYGEN
}    // namespace alib::strings[::appendables]
#endif
}  // namespace [alib::strings]
#endif // ALIB_STRINGS
