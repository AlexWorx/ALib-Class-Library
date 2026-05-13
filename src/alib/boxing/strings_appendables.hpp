//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
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
/// Specialization of functor #"AppendableTraits" for type #"boxing::Enum".
/// @tparam TChar      The character type of the target #"^AString".
/// @tparam TAllocator The allocator that the target #"%AString" uses, as prototyped with class
///                    #"lang::Allocator".
template<typename TChar, typename TAllocator>
struct AppendableTraits<boxing::Enum, TChar,TAllocator>
{
    /// Writes the given boxed object. This is done by invoking box-function
    /// #"FAppend" on box \p{value}.
    ///
    /// @param target  The #"%AString" that #"%Append(const TAppendable&)" was invoked on.
    /// @param value   The enum-box to its contents to \p{target}.
    void operator()( TAString<TChar,TAllocator>& target, const boxing::Enum& value ) {
        value.CastToBox().Call<boxing::FAppend<TChar,TAllocator>>( target );
    }
};
#endif // ALIB_ENUMRECORDS

//##################################################################################################
// AppendableTraits<Box>
//##################################################################################################

/// Specialization of template functor #"AppendableTraits" for type #"Box".
/// @tparam TChar      The character type of the target #"^AString".
/// @tparam TAllocator The allocator that the target #"%AString" uses, as prototyped with class
///                    #"lang::Allocator".
template<typename TChar, typename TAllocator>
struct AppendableTraits<boxing::Box, TChar, TAllocator>
{
    /// Writes the given boxed object. This is done by invoking box-function
    /// #"FAppend" on box \p{value}.
    ///
    /// @param target  The #"%AString" that #"%Append(const TAppendable&)" was invoked on.
    /// @param box     The box to its contents to \p{target}.
    void operator()( TAString<TChar,TAllocator>& target, const boxing::Box& box )
    { box.Call<boxing::FAppend<TChar,TAllocator>>( target ); }
};

#if DOXYGEN
}    // namespace alib::strings[::appendables]
#endif
}  // namespace [alib::strings]
#endif // ALIB_STRINGS
