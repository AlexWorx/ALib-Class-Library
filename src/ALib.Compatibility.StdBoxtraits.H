//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_BOXING_STDBOXTRAITS
#define H_ALIB_BOXING_STDBOXTRAITS
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/boxing/boxing.prepro.hpp"

#if ALIB_BOXING

#include "ALib.Lang.H"
#include "ALib.Boxing.H"
#include "ALib.Strings.H"
#include "ALib.Compatibility.StdStrings.H"

// ==========================================   Exports   ==========================================
namespace alib::boxing {

#if DOXYGEN
/// This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
/// module \alib_boxing_nl.<br>
/// The entities of those namespaces become available with the inclusion of specific headers
/// that import a certain C++20 Module or inject the functionality into a namespace in a
/// traditional fashion, for example, header \implude{Boxing.StdFunctors}.
namespace compatibility {

/// This namespace documents compatibility features of \alib_boxing_nl and the
/// standard C++ class library found in namespace \c std.
namespace std {
#endif

/// Specialization of struct \b %BoxTraits for template type <c>std::array<T, Size>></c>
/// Instead of boxing a pointer to the array object, a boxed array is stored, hence a pointer
/// to the first element contents and the array's length.
///
/// To enable this behavior, header-file \implude{Compatibility.StdBoxtraits} needs to be included in
/// the corresponding compilation unit.
///
/// Excluded from the specialization are character arrays.
/// Boxing of \c std::array instances of those types is customized by the specialization of
/// \alib{characters;ArrayTraits}, as described in manual chapter
/// \ref alib_boxing_strings "10. Boxing Character Strings".
///
/// \note As mandatory, the specialization is defined in \ref alib::boxing.
///       To keep the reference documentation of that namespace clean, it is documented here.
///
/// @tparam TElement The element type of the array.
/// @tparam N        The size of the array.
template<typename TElement, size_t N>
requires ( !characters::IsCharacter<TElement> )
struct BoxTraits<std::array<TElement, N> >
{
    /// Mapped type is \b TElement[].
    using                   Mapping=  TElement;

    /// Mapped as array-type
    static constexpr bool   IsArray=  true;

    /// Implementation of custom boxing for template class std::array
    /// @param box    The placeholder of the box box.
    /// @param value     The object to box.
    static void Write( Placeholder& box, const std::array<TElement, N>& value)
    {
        box.Write( value.data(), integer( N ) );
    }

    /// Forbid unboxing by declaring Read as void.
    /// @param box Ignored.
    static void Read( const Placeholder& box);
};

/// Specialization of struct \b %BoxTraits for template type <c>std::vector<T, std::allocator<T>></c>
/// Instead of boxing a pointer to the vector object, a boxed array is stored, hence a pointer
/// to the first element contents and the array length.
///
/// To enable this behavior, the header-file \implude{Compatibility.StdBoxtraits} needs to be
/// included in the corresponding compilation unit.
///
/// Excluded from the specialization are character arrays.
/// Boxing of \c std::vector instances of those types is customized by the specialization of
/// \alib{characters;ArrayTraits}, as described in manual chapter
/// \ref alib_boxing_strings "10. Boxing Character Strings".
///
/// \note As mandatory, the specialization is defined in \ref alib::boxing.
///       To keep the reference documentation of that namespace clean, it is documented here.
///
/// @tparam TElement The element type of the vector.
DOX_MARKER([DOX_BOXING_CUSTOM_VECTOR])
template<typename TElement>
requires (!characters::IsCharacter<TElement>)
struct BoxTraits<std::vector<TElement>>
{
    /// Mapped type is \c TElement[].
    using                   Mapping=  TElement;

    /// Mapped as array-type
    static constexpr bool   IsArray=  true;


    /// Implementation of custom boxing for template <c>class std::vector</c>.
    /// @param box    The placeholder of the box box.
    /// @param value  The object to box.
    static void Write( Placeholder& box, const std::vector<TElement>& value) {
        box.Write( value.data(), integer( value.size() ) );
    }

    /// Forbid unboxing by declaring Read as void.
    /// @param box Ignored.
    static void        Read( const Placeholder& box);
};
DOX_MARKER([DOX_BOXING_CUSTOM_VECTOR])


#if DOXYGEN
}} // namespace alib::boxing[::compatibility::std]
#endif

// #################################################################################################
// #############    Utility methods in namespace alib::compatibility::std     ################
// #################################################################################################
namespace compatibility { namespace std {

/// Creates a deep copy of a boxed C++ array type by appending its contents to a given
/// \c std::vector of corresponding element type.<br>
/// Note that no type checks are performed on the given box.
///
/// @tparam TElement  The element type.
/// @param  target    The target vector to fill.
/// @param  box       The source box of type \p{TElement[]}.
DOX_MARKER([DOX_BOXING_SAMPLE_ARR_UNBOX_VECTOR_IMPLEMENTATION])
template<typename TElement>
inline void CopyToVector( ::std::vector<TElement>& target, const Box& box )
{
    target.reserve( target.size() + size_t( box.UnboxLength() ) );
    for( integer i= 0 ; i < box.UnboxLength() ; ++i )
        target.emplace_back( box.UnboxElement<TElement>( i ) );
}
DOX_MARKER([DOX_BOXING_SAMPLE_ARR_UNBOX_VECTOR_IMPLEMENTATION])



void BootstrapStdStringBoxing();

/// Initializes \alib_boxing_nl in respect to <c>std::string</c>-types.
///
/// This method is \b not automatically invoked with function \alib{Bootstrap}, because support
/// for boxing <c>std::string</c>-types is optional and provided with the inclusion of header
/// \implude{Compatibility.StdBoxtraits}.
///
/// In general, boxing of <c>std::string</c>-types works well without the one-time invocation of
/// this function at the bootstrap of a process.
/// This method registers box-function \alib{boxing;FAppend} for <c>std::string</c>-types
/// types when \ref alib_boxing_customizing_identity "custom boxing is bypassed" by wrapping the
/// types in \c std::reference_wrapper<T>.
/// The function is implemented with the help of \alib{boxing;FAppend::WrappedAppendable}
/// for wrapped <c>std::string</c>-types, each for character types \b nchar and \b wchar.
///
/// \note
///   If invoked \b after bootstrap and modules \alib_threads_nl and \alib_monomem_nl are included in
///   the \alibbuild,  mutex \alib{monomem;GLOBAL_ALLOCATOR_LOCK} has to be locked before an
///   invocation.
///   Bootstrapping may look as follows:
///     \snippet "gtest_main.cpp"     DOX_COMPATIBILITY_BOOTSTRAP
///
/// \note
///   (Note, that the curly brackets create a compound that releases the automatic owner instance
///   after the call.) <br>
///   Alternatively, bootstrapping can be performed until \alib{BootstrapPhases::PrepareConfig}
///   and then this function can be invoked. In this case, no locking is necessary.
///
inline void BootstrapStdStringBoxing()
{
    #if ALIB_STRINGS && ALIB_BOXING

        alib::boxing::BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::string   >>( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<::std::string   >);
        alib::boxing::BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::string   >>( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<::std::string   >);
        alib::boxing::BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::wstring  >>( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<::std::wstring  >);
        alib::boxing::BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::wstring  >>( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<::std::wstring  >);
        alib::boxing::BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::u8string >>( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<::std::u8string >);
        alib::boxing::BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::u8string >>( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<::std::u8string >);
        #if ALIB_SIZEOF_WCHAR_T == 4
        alib::boxing::BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::u16string>>( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<::std::u16string>);
        alib::boxing::BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::u16string>>( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<::std::u16string>);
        #else
        alib::boxing::BootstrapRegister<FAppend<nchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::u32string>>( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<::std::u32string>);
        alib::boxing::BootstrapRegister<FAppend<wchar, lang::HeapAllocator>, ::std::reference_wrapper<::std::u32string>>( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<::std::u32string>);
        #endif

    #endif
}

}}} // namespace [alib::boxing::custom::std]#endif

#endif // ALIB_BOXING
#endif // H_ALIB_BOXING_STDBOXTRAITS


