// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_COMPATIBILITY_STD_BOXING
#define HPP_ALIB_COMPATIBILITY_STD_BOXING

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)

#if !defined(HPP_ALIB_COMPATIBILITY_STD_CHARACTERS)
#   include "alib/compatibility/std_characters.hpp"
#endif

#if !defined(HPP_ALIB_BOXING_BOXING)
#   include "alib/boxing/boxing.hpp"
#endif

#if !defined(_GLIBCXX_FUNCTIONAL) && !defined(_FUNCTIONAL_)
    #include <functional>
#endif


namespace aworx { namespace lib { namespace boxing {


#if ALIB_DOCUMENTATION_PARSER
/**
 * This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
 * module \alibmod_nolink_boxing.<br>
 * The entities of those namespaces become available with the inclusion of optional "compatibility"
 * headers found in folder \b "alib/compatibility".
 */
namespace compatibility {

/**
 * This namespace documents compatibility features of \alibmod_nolink_boxing and the
 * standard C++ class library found in namespace \c std.
 */
namespace std {
#endif

/**
 * Specialization of struct \b %T_Boxer for template type <c>std::array<T, Size>></c>
 * Instead of boxing a pointer to the array object, a boxed array is stored, hence a pointer
 * to the first element contents and the array's length.
 *
 * To enable this behaviour, header file
 *          "alib/compatibility/std_boxing.hpp"
 *
 * needs to be included in the corresponding compilation unit.
 *
 * Excluded from the specialization are character arrays.
 * Boxing of \c std::array instances of those types is customized by the specialization of
 * \alib{characters,T_CharArray}, as described in manual chapter
 * \ref alib_boxing_strings "10. Boxing Character Strings".
 *
 * \note As mandatory, the specialization is defined in \ref aworx::lib::boxing.
 *       To keep the reference documentation of that namespace clean, it is documented here.
 *
 * @tparam TElement The element type of the array.
 * @tparam N        The size of the array.
 */
template<typename TElement, size_t N>
struct T_Boxer<std::array<TElement, N>, ATMP_VOID_IF(!characters::TT_IsChar<TElement>::value) >
{
    /** Mapped type is \c TElement[]. */
    using  Mapping=    TMappedToArrayOf<TElement>;

    /**
     * Implementation of custom boxing for template class std::array
     * @param target    The placeholder of the target box.
     * @param value     The object to box.
     */
    static inline void Write( Placeholder& target, const std::array<TElement, N>& value)
    {
        target.Write( value.data(), static_cast<integer>( N ) );
    }

    /**
     * Forbid unboxing by declaring Read as void.
     * @param src Ignored.
     */
    static void Read( const Placeholder& src);
};


/**
 * Specialization of struct \b %T_Boxer for template type <c>std::vector<T, std::allocator<T>></c>
 * Instead of boxing a pointer to the vector object, a boxed array is stored, hence a pointer
 * to the first element contents and the array length.
 *
 * To enable this behaviour, header file
 *          "alib/compatibility/std_boxing.hpp"
 *
 * needs to be included in the corresponding compilation unit.
 *
 * Excluded from the specialization are character arrays.
 * Boxing of \c std::vector instances of those types is customized by the specialization of
 * \alib{characters,T_CharArray}, as described in manual chapter
 * \ref alib_boxing_strings "10. Boxing Character Strings".
 *
 * \note As mandatory, the specialization is defined in \ref aworx::lib::boxing.
 *       To keep the reference documentation of that namespace clean, it is documented here.
 *
 * @tparam TElement The element type of the vector.
 */
DOX_MARKER([DOX_ALIB_BOXING_CUSTOM_VECTOR])
template<typename TElement>
struct T_Boxer<std::vector<TElement>, ATMP_VOID_IF(!characters::TT_IsChar<TElement>::value) >
{
    /** Mapped type is \c TElement[]. */
    using  Mapping=    TMappedToArrayOf<TElement>;

    /**
     * Implementation of custom boxing for template class std::vector
     * @param target    The placeholder of the target box.
     * @param value     The object to box.
     */
    static inline void Write( Placeholder& target, const std::vector<TElement>& value)
    {
        target.Write( value.data(), static_cast<integer>( value.size() ) );
    }

    /**
     * Forbid unboxing by declaring Read as void.
     * @param src Ignored.
     */
    static void        Read( const Placeholder& src);
};
DOX_MARKER([DOX_ALIB_BOXING_CUSTOM_VECTOR])


#if ALIB_DOCUMENTATION_PARSER
}} // namespace aworx::lib::boxing[::compatibility::std]
#endif



// #################################################################################################
// #############    Utility methods in namespace aworx::lib::compatibility::std     ################
// #################################################################################################


namespace compatibility { namespace std {

/**
 * Creates a deep copy of a boxed C++ array type by appending its contents to a given
 * \c std::vector of corresponding element type.<br>
 * Note that no type checks are performed on the given box.
 *
 * @param  target    The target vector to fill.
 * @param  box       The source box of type \p{TElement[]}.
 * @tparam TElement  The element type.
 */
DOX_MARKER([DOX_ALIB_BOXING_SAMPLE_ARR_UNBOX_VECTOR_IMPLEMENTATION])
template<typename TElement>
inline void CopyToVector( ::std::vector<TElement>& target, const Box& box )
{
    target.reserve( target.size() + static_cast<size_t>( box.UnboxLength() ) );
    for( integer i= 0 ; i < box.UnboxLength() ; ++i )
        target.emplace_back( box.UnboxElement<TElement>( i ) );
}
DOX_MARKER([DOX_ALIB_BOXING_SAMPLE_ARR_UNBOX_VECTOR_IMPLEMENTATION])



void InitStdStringBoxing();

/**
 * Initializes \alibmod_nolink_boxing in respect to <c>std::string</c>-types.
 *
 * This method is \b not automatically invoked with \alib{ALibModules::Init}, because support
 * for boxing <c>std::string</c>-types is optional and provided with the inclusion of header
 * \ref alib/compatibility/std_boxing.hpp.
 *
 * In general boxing of <c>std::string</c>-types works well without the one-time invocation of
 * this function at the bootstrap of a process.
 * This method registers box-function \alib{boxing,FAppend} for <c>std::string</c>-types
 * types when \ref alib_boxing_customizing_identity "custom boxing is bypassed" by wrapping the
 * types in \c std::reference_wrapper<T>.
 * The function is implemented with the help of \alib{boxing,FAppend::WrappedAppendable}
 * for wrapped <c>std::string</c>-types, each for character types \b nchar and \b wchar.
 *
 * \note
 *   Although the method must be invoked only once, it is defined inline to avoid the need of
 *   including a corresponding compilation unit.
 */
inline void InitStdStringBoxing()
{
    #if ALIB_MODULE_STRINGS && ALIB_MODULE_BOXING

DOX_MARKER( [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_4] )
aworx::lib::boxing::Register<FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<::std::string>>>( boxing::FAppend<nchar>::WrappedAppendable<::std::string> );
aworx::lib::boxing::Register<FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<::std::string>>>( boxing::FAppend<wchar>::WrappedAppendable<::std::string> );
DOX_MARKER( [DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_4] )

        aworx::lib::boxing::Register<FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<::std::wstring  >>>( boxing::FAppend<nchar>::WrappedAppendable<::std::wstring  >);
        aworx::lib::boxing::Register<FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<::std::wstring  >>>( boxing::FAppend<wchar>::WrappedAppendable<::std::wstring  >);
        #if ALIB_SIZEOF_WCHAR_T == 4
        aworx::lib::boxing::Register<FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<::std::u16string>>>( boxing::FAppend<nchar>::WrappedAppendable<::std::u16string>);
        aworx::lib::boxing::Register<FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<::std::u16string>>>( boxing::FAppend<wchar>::WrappedAppendable<::std::u16string>);
        #else
        aworx::lib::boxing::Register<FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<::std::u32string>>>( boxing::FAppend<nchar>::WrappedAppendable<::std::u32string>);
        aworx::lib::boxing::Register<FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<::std::u32string>>>( boxing::FAppend<wchar>::WrappedAppendable<::std::u32string>);
        #endif

    #endif
}


}}}}} // namespace [aworx::lib::boxing::custom::std]


#endif // HPP_ALIB_COMPATIBILITY_STD_BOXING
