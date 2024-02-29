/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong.<br>
 * With the inclusion of this header compatibility features between \alib and the C++ standard
 * library are provided.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_COMPATIBILITY_STD_BOXING
#define HPP_ALIB_COMPATIBILITY_STD_BOXING

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
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


namespace alib {  namespace boxing {


#if defined(ALIB_DOX)
/**
 * This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
 * module \alib_boxing_nl.<br>
 * The entities of those namespaces become available with the inclusion of optional "compatibility"
 * headers found in folder \alibsrcdir{compatibility}.
 */
namespace compatibility {

/**
 * This namespace documents compatibility features of \alib_boxing_nl and the
 * standard C++ class library found in namespace \c std.
 */
namespace std {
#endif

/**
 * Specialization of struct \b %T_Boxer for template type <c>std::array<T, Size>></c>
 * Instead of boxing a pointer to the array object, a boxed array is stored, hence a pointer
 * to the first element contents and the array's length.
 *
 * To enable this behavior, header file
 *          "alib/compatibility/std_boxing.hpp"
 *
 * needs to be included in the corresponding compilation unit.
 *
 * Excluded from the specialization are character arrays.
 * Boxing of \c std::array instances of those types is customized by the specialization of
 * \alib{characters,T_CharArray}, as described in manual chapter
 * \ref alib_boxing_strings "10. Boxing Character Strings".
 *
 * \note As mandatory, the specialization is defined in \ref alib::boxing.
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
    static void Write( Placeholder& target, const std::array<TElement, N>& value)
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
 * To enable this behavior, header file
 *          "alib/compatibility/std_boxing.hpp"
 *
 * needs to be included in the corresponding compilation unit.
 *
 * Excluded from the specialization are character arrays.
 * Boxing of \c std::vector instances of those types is customized by the specialization of
 * \alib{characters,T_CharArray}, as described in manual chapter
 * \ref alib_boxing_strings "10. Boxing Character Strings".
 *
 * \note As mandatory, the specialization is defined in \ref alib::boxing.
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
    static void Write( Placeholder& target, const std::vector<TElement>& value)
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


#if defined(ALIB_DOX)
}} // namespace alib::boxing[::compatibility::std]
#endif



// #################################################################################################
// #############    Utility methods in namespace alib::compatibility::std     ################
// #################################################################################################


namespace compatibility { namespace std {

/**
 * Creates a deep copy of a boxed C++ array type by appending its contents to a given
 * \c std::vector of corresponding element type.<br>
 * Note that no type checks are performed on the given box.
 *
 * @tparam TElement  The element type.
 * @param  target    The target vector to fill.
 * @param  box       The source box of type \p{TElement[]}.
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



void BootstrapStdStringBoxing();

/**
 * Initializes \alib_boxing_nl in respect to <c>std::string</c>-types.
 *
 * This method is \b not automatically invoked with function \alib{Bootstrap}, because support
 * for boxing <c>std::string</c>-types is optional and provided with the inclusion of header
 * \alibheader{compatibility/std_boxing.hpp}.
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
 *   If invoked after bootstrap and modules \alib_threads_nl and \alib_monomem_nl are included in
 *   the \alibdist,  mutex \alib{monomem,GlobalAllocatorLock} has to be locked prior to an
 *   invocation.
 *   Bootstrapping may look as follows:
 *     \snippet "gtest_main.cpp"     DOX_ALIB_COMPATIBILITY_BOOTSTRAP
 *
 * \note
 *   Alternatively, bootstrapping can be performed until \alib{BootstrapPhases::PrepareConfig}
 *   and then this function can be invoked. In this case, no locking is necessary.
 *
 */
inline void BootstrapStdStringBoxing()
{
    #if ALIB_STRINGS && ALIB_BOXING

        alib::boxing::BootstrapRegister<FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<::std::string>>>( boxing::FAppend<nchar>::WrappedAppendable<::std::string> );
        alib::boxing::BootstrapRegister<FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<::std::string>>>( boxing::FAppend<wchar>::WrappedAppendable<::std::string> );

        alib::boxing::BootstrapRegister<FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<::std::wstring  >>>( boxing::FAppend<nchar>::WrappedAppendable<::std::wstring  >);
        alib::boxing::BootstrapRegister<FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<::std::wstring  >>>( boxing::FAppend<wchar>::WrappedAppendable<::std::wstring  >);
        #if ALIB_SIZEOF_WCHAR_T == 4
        alib::boxing::BootstrapRegister<FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<::std::u16string>>>( boxing::FAppend<nchar>::WrappedAppendable<::std::u16string>);
        alib::boxing::BootstrapRegister<FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<::std::u16string>>>( boxing::FAppend<wchar>::WrappedAppendable<::std::u16string>);
        #else
        alib::boxing::BootstrapRegister<FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<::std::u32string>>>( boxing::FAppend<nchar>::WrappedAppendable<::std::u32string>);
        alib::boxing::BootstrapRegister<FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<::std::u32string>>>( boxing::FAppend<wchar>::WrappedAppendable<::std::u32string>);
        #endif

    #endif
}


}}}} // namespace [alib::boxing::custom::std]


#endif // HPP_ALIB_COMPATIBILITY_STD_BOXING
