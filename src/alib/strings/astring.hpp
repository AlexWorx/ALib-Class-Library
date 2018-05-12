// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

// needed for Doxygen include of the typedefs at the end of the file
///@file

// Include guard
#ifndef HPP_ALIB_STRINGS_ASTRING
#define HPP_ALIB_STRINGS_ASTRING 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION)
    #error "include 'alib/alib.hpp' instead of this header"
#endif


#if !defined (_STDLIB_H) && !defined(_INC_STDLIB)
    #include <stdlib.h>
#endif


// conditional expression is constant for using our constant template parameters to select
// checking vs. non-checking method versions
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif


namespace aworx { namespace lib { namespace strings
{
// #################################################################################################
// forward declarations
// #################################################################################################

template<typename TChar>
class AStringBase;


//! @cond NO_DOX
ALIB_DBG( ALIB_API void appendErrorToAString( AStringBase<nchar>& target ); )
ALIB_DBG( ALIB_API void appendErrorToAString( AStringBase<wchar>& target ); )
//! @endcond


// #################################################################################################
// template struct T_Apply
// #################################################################################################

/** ********************************************************************************************
 * This is a template struct which answers the question
 * "Can an object (value) of type \p{TApplicable} be passed to method \ref AString::Apply?
 * By default, the answer is no. In particular, the default implementation of the struct inherits
 * \c std::false_type.
 *
 * To make a custom type "applicable", this struct needs to be specialized for the custom type
 * with inheriting from \c std::true_type and with implementing method #Apply.
 *
 * This concept of specializing this struct "once" and then being able to pass custom types
 * to objects of class
 * \alib{strings,AStringBase,AString} has a central role for using \alib in a
 * \ref aworx::lib "non intrusive" way: Methods of \alib classes (or third-party code that uses
 * \alib) may offer APIs that can be fed with custom types without the prior need of conversion.
 *
 * A list of built-in specializations for standard C++ and \alib types is given and documented
 * \ref alib_namespace_strings_astring_application_vs_interface "here".
 *
 * To restrict the need for specializations of this method to a single variant of the external type,
 * template type \p{TApplicable} to specialize will always be a the plain custom type. No pointer,
 * const, etc. versions need to be implemented.
 *
 * The implementation of method #Apply may do 'whatever' is wanted with the %AString provided in
 * parameter \p{target}. For string types, simple append operations are predefined. More
 * complex operations format the string, convert integer and floating point numbers, etc.
 *
 * For basic user defined string types that get adopted to \alib string system using a
 * specialization of struct
 * \ref aworx::lib::strings::T_String "T_String", no specialization of this struct is needed,
 * because method \alib{strings,AStringBase::Apply,AString::Apply} will detect that and
 * append the custom string.
 *
 * The second template parameter \p{TEnableIf} may be used to perform templated specializations
 * based on a condition evaluated with \c std::enable_if (or similar tmp mechanics).
 *
 * Built-in specializations of this function for certain types exist, as listed in reference
 * documentation of class \alib{strings,AStringBase,AString}.
 * Some types exists only for the reason to be "applicable" to <b>%AString</b>!
 * As a sample, class
 * \alib{strings,FormatBase::Field,Format::Field} allows to align strings
 * when they are applied. The invocation is simple
 * and nicely readable:
 * \snippet "DOX_ALIB_APPLYTO.cpp"     DOX_ALIB_APPLYTO_FIELD
 * which produces:
 * \snippet "DOX_ALIB_APPLYTO_FIELD.txt"     OUTPUT
 *
 * The next sample implements this function for \alib class \alib{time,DateTime} to print out a
 * formatted date:
 * \snippet "DOX_ALIB_APPLYTO.cpp"     DOX_ALIB_APPLYTO_DEFINITION
 * With this definition included, user code might now do this:
 * \snippet "DOX_ALIB_APPLYTO.cpp"     DOX_ALIB_APPLYTO_USE
 *
 *  The output will be something like this:
 *
 * \snippet "DOX_ALIB_APPLYTO.txt"     OUTPUT
 *
 * # Reference Documentation #
 * @tparam  TApplicable The type that should be made compatible with method \ref AString::Apply.
 * @tparam  TEnableIf   Optional TMP parameter to allow selective templated specializations.
 **********************************************************************************************/
template<typename TApplicable, typename TChar, typename TEnableIf= void>
struct T_Apply   : public std::false_type
{
    ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING
    /** ********************************************************************************************
     * Default implementation of Apply. This method must never be invoked and therefore
     * throws an assertion if it is.
     * Prior to do so, a \c static_assert will prevent code to apply types to an
     * object of class \b %AString that are not applicable. However, it might happen that this
     * compile-time assertion fails to be activated in some tricky situations.
     * In this case, the runtime assert will step in.
     *
     * Implementations for specialized versions of this struct might do whatever they are supposed
     * to with \p{target}. Actions are not restricted to appending something.
     *
     * \note
     *   Some more complex applicable types might want to return extended information by setting
     *   own members during the application process. To achieve this, the corresponding
     *   implementations of this method have to cast the given const reference to a
     *   non-const reference. This is considered a good practice, as the const specifier
     *   most probably was added by the TMP operations of method
     *   \alib{strings,AStringBase::Apply,AString::Apply} anyhow - only for the reason
     *   to have one single template specialization per type.
     *
     * @param   target The object of type %AString that will have object \p{src} applied.
     * @param   src    The source object of arbitrary type will get applied to %AString \p{target}.
     * @return  The number of characters that got appended should be returned
     *          ('should' here means that at least a non zero, positive value has to be returned)
     *          and -1 if the object \p{src}  which is to be applied represented a \e nulled object.
     *          Returning -1 causes the AString to be \e nulled if the apply operation was invoked through
     *          \alib{strings,AStringBase::operator=,AString assignment operator}.<br>
     *          If nothing is was appended, this method must return 0.
     **********************************************************************************************/
    static
    integer Apply( AStringBase<TChar>& target, const TApplicable& src )
    {
        (void) target;
        (void) src;

        ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING

        // prevent invoking us with unknown types
        using TPlain= typename std::remove_const    <
                      typename std::remove_pointer  <
                      typename std::remove_reference<
                      typename std::remove_const    < TApplicable >::type>::type>::type>::type;
        using TAElem= typename std::remove_extent<TPlain>::type;

        static_assert(
                   std::is_arithmetic <                     TPlain>::value
            ||     std::is_base_of    <StringBase<nchar>  , TPlain>::value
            ||     std::is_base_of    <StringBase<wchar>  , TPlain>::value
            ||     std::is_same       <decltype(nullptr)  , TPlain>::value

            || (   std::is_array <TPlain> ::value
                && (    std::is_same       <character     , TAElem>::value
                     || std::is_same       <complementChar, TAElem>::value
                     || std::is_same       <strangeChar   , TAElem>::value
                   )
               )
            ||     T_String           <TPlain, TChar  >::value
            ||     T_Apply            <TPlain, TChar  >::value
        , "ALib: T is not a known type to append to AString. Implement struct T_Apply<T> to enable conversion.");

        //ALIB_TMP_SHOW_TYPE_IN_DEBUGGER(TPlain)
        //assert(0);

        ALIB_DBG( appendErrorToAString( target ) );

        ALIB_WARNING(ASTR("Unknown type for T_ApplyTo()"));

        return 0;

        ALIB_WARNINGS_RESTORE
    }
    ALIB_WARNINGS_RESTORE
};



#define ALIB_STRINGS_SPECIALIZE_T_APPLY(TYPE)                                                      \
namespace aworx { namespace lib { namespace strings                                                \
{                                                                                                  \
    template<> struct       T_Apply<TYPE, aworx::character> : public std::true_type                \
    {                                                                                              \
        ALIB_API static integer Apply( AStringBase<aworx::character>& target, const TYPE src );    \
    };                                                                                             \
}}}                                                                                                \

#define ALIB_STRINGS_SPECIALIZE_T_APPLY_N(TYPE)                                                    \
namespace aworx { namespace lib { namespace strings                                                \
{                                                                                                  \
    template<> struct       T_Apply<TYPE, aworx::nchar> : public std::true_type                \
    {                                                                                              \
        ALIB_API static integer Apply( AStringBase<aworx::nchar>& target, const TYPE src );    \
    };                                                                                             \
}}}                                                                                                \

#define ALIB_STRINGS_SPECIALIZE_T_APPLY_W(TYPE)                                                    \
namespace aworx { namespace lib { namespace strings                                                \
{                                                                                                  \
    template<> struct       T_Apply<TYPE, aworx::wchar> : public std::true_type                \
    {                                                                                              \
        ALIB_API static integer Apply( AStringBase<aworx::wchar>& target, const TYPE src );    \
    };                                                                                             \
}}}                                                                                                \


#define ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF(TYPE, IMPL)                                            \
aworx::lib::lang::integer aworx::lib::strings::T_Apply<TYPE,aworx::character>::Apply(AStringBase<aworx::character>& target, const TYPE src) \
{                                                                                                  \
    IMPL                                                                                           \
}                                                                                                  \

#define ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF_N(TYPE, IMPL)                                          \
aworx::lib::lang::integer aworx::lib::strings::T_Apply<TYPE,aworx::nchar>::Apply(AStringBase<aworx::nchar>& target, const TYPE src) \
{                                                                                                  \
    IMPL                                                                                           \
}                                                                                                  \

#define ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF_W(TYPE, IMPL)                                          \
aworx::lib::lang::integer aworx::lib::strings::T_Apply<TYPE,aworx::wchar>::Apply(AStringBase<aworx::wchar>& target, const TYPE src) \
{                                                                                                  \
    IMPL                                                                                           \
}                                                                                                  \

#define ALIB_STRINGS_SPECIALIZE_T_APPLY_INLINE(TYPE, IMPL)                                         \
namespace aworx { namespace lib { namespace strings                                                \
{                                                                                                  \
    template<> struct       T_Apply<TYPE,aworx::character> : public std::true_type                 \
    {                                                                                              \
        static inline integer Apply( AStringBase<aworx::character>& target, const TYPE src ){ IMPL}\
    };                                                                                             \
}}}                                                                                                \

#define ALIB_STRINGS_SPECIALIZE_T_APPLY_INLINE_N(TYPE, IMPL)                                       \
namespace aworx { namespace lib { namespace strings                                                \
{                                                                                                  \
    template<> struct       T_Apply<TYPE,aworx::nchar> : public std::true_type                 \
    {                                                                                              \
        static inline integer Apply( AStringBase<aworx::nchar>& target, const TYPE src ){ IMPL}\
    };                                                                                             \
}}}                                                                                                \

#define ALIB_STRINGS_SPECIALIZE_T_APPLY_INLINE_W(TYPE, IMPL)                                       \
namespace aworx { namespace lib { namespace strings                                                \
{                                                                                                  \
    template<> struct       T_Apply<TYPE,aworx::wchar> : public std::true_type                 \
    {                                                                                              \
        static inline integer Apply( AStringBase<aworx::wchar>& target, const TYPE src ){ IMPL}\
    };                                                                                             \
}}}                                                                                                \



/** ************************************************************************************************
 * Specializes class
 * \alib{strings,TStringBase,TString} to implement mutable, non-constant
 * strings using writable buffer memory.
 *
 * There are two possible types of buffers:
 * - <b>Internal buffers</b><br>
 *   This is the standard case and implements a buffer that is allocated from
 *   <em>free memory</em> (aka 'the heap') that eventually grows over time and never shrinks, unless
 *   explicitly demanded by the user of the class. Those buffers are deleted when objects
 *   of this type are deleted.
 * - <b>External buffers</b><br>
 *   Set with method #SetBuffer. These buffers don't get deleted. However, if their capacity
 *   is exceeded, they will get automatically replaced by an internal buffer.
 *   (See field #ReplaceExternalBuffer to control warnings on that in debug compilation).
 *
 *<b>Construction/Destruction</b><p>
 * The following ways of construction are provided:
 * - <b>Default construction</b><br>
 *   Does not allocate a character buffer. The allocation is this way deferred to
 *   the first operation that writes data into the object. Alternatively, an external buffer
 *   can be set before such operation is invoked.
 * - <b>Construction with initial capacity</b><br>
 *   With constructor #AStringBase(size_t), an initial length can be provided. This construction
 *   variant should be used, if the maximum length the object that will be reached during its
 *   lifetime is predictable, because it avoids iterative, increasing buffer allocations that
 *   otherwise would be performed until that size is reached once.
 *   Furthermore there are constructors
 * - <b>Construction with external buffers</b><br>
 *   With constructor #AStringBase(TChar*, integer), an external buffer is set. This constructor is
 *   protected and therefore accessible by derived classes' constructors only.
 *   This is to avoid an otherwise likely common misunderstanding  that this constructor would
 *   copy the contents of a provided cstring. For users of this class, the alternative to this
 *   constructor is using a combination of the default constructor and method #SetBuffer which
 *   has exactly the same effect.
 * - <b>Construction with strings</b><br>
 *   Constructors #AStringBase(const TApplicable&), AStringBase(const String&, index, integer=) and
 *   copy constructor #AStringBase(const AStringBase&) will allocate an internal buffer of the
 *   capacity needed and copy the contents of the given string type.
 *
 * Destruction will free internally allocated buffers.
 * \note Method #SetBuffer provides a boolean parameter that also allows to let an object of this
 *       class take control on the provided buffer, namely delete it in the destructor. In this
 *       case, a buffer set with this method is considered an internal buffer, rather than an
 *       external one.
 *
 * \anchor alib_namespace_strings_astring_move
 *<b>Move constructor and move assignment</b><p>
 * This class provides a (noexcept) move constructor which grabs the buffer of a given movable
 * \b %AString, as long as this given object does not use an external buffer. In the latter case, the
 * contents is copied like in the copy constructor.<br>
 * This class does not provide a move assignment operator. The rational for this is that normally
 * a "more temporary" AString would be assigned to a "less temporary" one. In this case, it would
 * not be helpful to replace the allocated storage, each time.
 *
 * \note
 *   In general, while assignment of other objects of type \b %AString is possible through
 *   #operator=(const AStringBase&), the advised code style is to use a combination of #Clear and
 *   append \ref _ "_()" (e.g. <em>myString._()._(otherString)</em>) to do the assignment instead.
 *   This leads to exactly the same result as long as the \b %AString that is assigned is not
 *   \e nulled.<br>
 * \note
 *   In the case that it is \e nulled, using the assignment operator will set this object also
 *   to \e nulled state, hence dispose any currently allocated buffer. This, for most cases is not
 *   what is wanted.
 *
 * \anchor alib_namespace_strings_astring_write_access
 * <b>Write Access to the Buffer</b><p>
 * Parent class \alib{strings,StringBase,String} holds its protected field
 * \alib{strings,StringBase::buffer,buffer} in an anonymous C++ union of two pointers,
 * one typed <em>const char*</em> and the other <em>char*</em>.
 * This class exposes the non-constant buffer pointer of that union with method #VBuffer.
 * This allows users of this class to <em>freely</em> operate on the buffer with any <em>C</em>
 * or <em>C++</em> algorithm available. Of-course, it is up to the programmer doing this to
 * keep invariants that this class naturally constitutes intact.
 *
 * In addition to this, a bunch of methods allow the modification of characters.
 * #operator[] is returning a reference to a char in contrast to the same operator defined
 * in class \b %String. This allows assignments of values when using an object with this operator as
 * an lvalue.
 *
 * \anchor  alib_namespace_strings_astring_nullable
 * <b>Nullable AStrings</b><p>
 * As discussed in the namespace documentation (\ref alib_strings_classes_String_nullable)
 * parent class \alib{strings,StringBase,String} provides an interface to detect \e nulled objects.
 * This concept is duly implemented in this class. An object of type #AString is \e nulled
 * when no internal buffer is allocated or external buffer is set.
 *
 * If default constructed, constructed  with zero size, \c nullptr or any other string type
 * object which is \e nulled, no buffer is created and hence the new object is in \e nulled
 * state and inherited method
 * aworx::lib::strings::String::IsNull "IsNull" will return \c true for that object.
 * Consequently, it makes a difference if an \b %AString is constructed using <em>AString()</em> or
 * <em>AString(\"\")</em>.
 * This allows to differentiate between \e nulled AStrings and empty \b %AStrings, which is quite handy
 * in certain situations. An object that was filled already can be reset (nulled) by
 * either assigning a \c nullptr, by invoking \ref SetBuffer "SetBuffer(0)" or by invoking
 * #SetNull on the instance. (See also methods #IsNull, #IsNotNull and #Capacity.)
 * The methods #Equals, #CompareTo and the overloaded comparison #operator== and
 * #operator!= allow nullptr comparisons. e.g. a \e nulled \b %AString equals to another
 * \e nulled \b %AString but not to an empty but not \e nulled AString.
 *
 * To make this more clear, note the following <b>sample code</b> which does not throw an assertion:
 * \snippet "DOX_ALIB_ASTRING.cpp"    DOX_ALIB_ASTRING_NULLED
 *
 * \note
 *   - As mentioned above, to avoid loosing an allocated buffer, use a combination of
 *     #Clear and append \ref _ "_()" to perform assignments with other string types instead of
 *     using one of the assignment operators (#operator=). No check for \e nulled string
 *     is needed when appending them to an object of this type.
 *
 *   - Even if an \b %AString object is \e nulled, inherited method #ToCString will return a
 *     (zero terminated) valid empty <em>char*</em>.
 *     This has the advantage that in many situations the nulled state is not
 *     needed to be handled (for those cases where the difference between a \e nulled and an empty
 *     string is irrelevant).
 *
 * \anchor  alib_namespace_strings_astring_applyto
 * <b>Applying Objects to AStrings</b><p>
 * The concept of appending things to a string is generalized in the C++
 * implementation of this class and named <em>applying</em> instead. While applying string
 * types is the same as appending string types in other language versions of \alib, the
 * '<em>application</em>' of non-string types allows flexible and well readable code design.
 *
 * The central mechanism used to apply arbitrary types is similar to what is used by class
 * \alib{strings,StringBase,String} and the partially specialized template struct
 * \ref aworx::lib::strings::T_String "T_String".
 * By specializing template struct
 * \ref aworx::lib::strings::T_Apply  "T_Apply"
 * it is possible to apply objects of 'external', user defined types to objects of this class.
 *
 * These partly specialized templates are used by this classes' method #Apply. Because this
 * method, although declared <em>public</em>, has a more internal character, in standard
 * situations its invocation is performed indirectly through methods:
 * - Constructor \ref AStringBase(const TApplicable&) "AString(const TApplicable&)"
 * - Operator operator <<(const T&)
 * - Method #_(const T&) (which is a synonym for <em>operator << </em> but allowing non-checked invocations)
 * - Operator operator=(const T&)
 *
 * For more information refer to documentation of partially specialized template struct
 * \alib{strings,T_Apply} and
 *
 * \anchor alib_namespace_strings_astring_application_vs_interface
 * <b>Application Instead of Explicit Interface Methods</b><p>
 * Various methods that exist in the implementations of class \b %AString in different programming
 * languages versions of \alib, are missing in this C++ implementation.
 * Here, they have been replaced by  the concept of <em>applying</em> types.
 * The following provides a list of predefined applicable types found in \alib:
 *
 * - Character types \ref aworx::character, \ref aworx::complementChar and \ref aworx::strangeChar,
 *   and strings of those (pointers to the types or arrays of them) will be appended.<br>
 *   Narrow/wide character conversion will be performed according to the current locale. (See
 *   \ref aworx::lib::ALib::Init "ALib::Init" for more information about setting locale.
 * - C++ fundamental arithmetic types (e.g. \c float, \c int, <c>signed char</c>, etc.) will be
 *   converted to string representations using static object
 *   \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational} and appended.
 * - Type \c bool appends the string literal \c "true" respectively \c "false".
 * - Class \alib{strings,FormatBase,Format}, with public inner classes
 *   - \alib{strings,FormatBase::Field,Format::Field}
 *   - \alib{strings,FormatBase::Tab,Format::Tab}
 *   - \alib{strings,FormatBase::Bin,Format::Bin}
 *   - \alib{strings,FormatBase::Hex,Format::Hex}
 *   - \alib{strings,FormatBase::Oct,Format::Oct}
 * - Class \ref aworx::lib::boxing::Box "Box" by invoking boxing interface
 *   \ref aworx::lib::strings::boxing::IApply "IApply".
 * - Some support is given for third party types. See \ref aworx::lib::strings::thirdparty
 *   for details.
 *
 * <b>Insert, Replace and Delete</b><p>
 * The following insert, delete and replace operations are provided:
 * - #InsertAt To insert a string at an arbitrary position.
 * - #InsertChars To insert a quantity of characters at an arbitrary position.
 * - \ref ReplaceSubstring(const StringBase<TChar>&,integer,integer) "ReplaceSubstring" To replace a substring with a new string.
 * - \ref ReplaceRegion(TChar,integer,integer) "ReplaceRegion" To replace a substring with a given character.
 * - #SearchAndReplace to search all occurrences of a substring and replace by a different string.
 * - #Delete To delete an arbitrary  substring.
 * - #DeleteStart To delete n characters from the start.
 * - #DeleteEnd To delete n characters from the end.
 *
 * @tparam TChar    The character type. Implementations for \c char and \c wchar_t are provided
 *                  with type definitions \ref aworx::NAString and \ref aworx::WAString.
 **************************************************************************************************/
template<typename TChar>
class AStringBase : public TStringBase<TChar>
{

    /**  This friendship is needed to allow class PreallocatedString to steal our buffer in its
         move constructor. */
    friend class PreallocatedString;

    /** ############################################################################################
     * @name Debug Features
     ##@{ ########################################################################################*/


        #if ALIB_DEBUG_STRINGS
        protected:
            /**
             * Used to check if previous grow request was exactly what is now the length.<br>
             * This is available only if \ref ALIB_DEBUG_STRINGS is \c true.
             */
            integer         debugLastAllocRequest                                                =0;

            /**
             * Used to check if previous grow request was exactly what is now the length.<br>
             * This is available only if \ref ALIB_DEBUG_STRINGS is \c true.
             */
            bool            debugBufferWithMagicBytePadding                                  =false;

            /**
             *  Checks this objects' state. This method is internally invoked with almost
             *  every other method of this class, but only if compilation symbol
             *  \ref ALIB_DEBUG_STRINGS is \c true.
             */
        public:
            void    _dbgCheck()   const;

        #endif

        /**
         *  If \c true, an one-time warning (using
         *  \ref aworx::lib::lang::Report::DoReport "Report::DoReport")
         *  will be issued when an external buffer, whose life-cycle is not controlled by this
         *  instance gets replaced by a new allocation. This normally shall not
         *  happen, but still might be wanted or at least taken into account.
         *
         *  E.g. for \alib{strings,PreallocatedStringBase,PreallocatedString} objects, that
         *  sometimes may grow more than average, this warning may be disabled after creation
         *  of an instance.
         *
         *  The warning will be issued only once per instance unless the warning gets re-enabled
         *
         *  \note This field can not be accessed directly (For technical reasons, it is
         *        differently declared than stated here in the source documentation).<br>
         *        To change the field use macros
         *        - \ref ALIB_WARN_ONCE_PER_INSTANCE_ENABLE(myinstance, ReplaceExternalBuffer) and
         *        - \ref ALIB_WARN_ONCE_PER_INSTANCE_DISABLE(myinstance, ReplaceExternalBuffer)
         *
         *        where \p{myinstance} is a reference to the object in question).
         */
        #if defined(DOX_PARSER)
            bool                              ReplaceExternalBuffer= true;
        #else
            ALIB_WARN_ONCE_PER_INSTANCE_DECL( ReplaceExternalBuffer, true )
        #endif


    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /**
         *  The current size of the buffer excluding the trailing '\0'. If no buffer is allocated,
         *  this is is 0. If an external Buffer is used and if this buffer is not under our control
         *  (we must not delete it), then the size of such buffer is stored as a negative value.
         */
        integer            capacity;

    protected:
        /** ****************************************************************************************
         * Constructs an \b %AString with the given external buffer.
         * The given buffers' life-cycle is considered to be managed externally.<br>
         * This constructor is protected and provided for derived classes that dispose about
         * their own buffer.
         *
         * \note
         *   Protected access was given to this method also to avoid misunderstandings that this
         *   constructor is not for providing copyable string data. If the functionality of this
         *   constructor is needed, it can simply be imitated by
         *   - default construction and
         *   - immediate invocation of #SetBuffer(TChar*, integer, integer, lang::Responsibility).
         *
         * @param extBuffer       The external buffer to use.
         * @param extBufferSize   The capacity of the given buffer.
         ******************************************************************************************/
        constexpr
        inline
        explicit AStringBase( TChar* extBuffer, integer extBufferSize )
        : TStringBase<TChar>( extBuffer, 0)
        #if ALIB_DEBUG_STRINGS
        ,debugLastAllocRequest(extBufferSize-1)
        #endif
        , capacity  (- (extBufferSize - 1))

        {}

    /** ############################################################################################
     * @name Constructors, Destructor and Assignment
     ##@{ ########################################################################################*/
    public:
        /** ****************************************************************************************
         *   Constructs an empty, \e nulled \b %AString (does not allocate a buffer).
         ******************************************************************************************/
        explicit
        constexpr
        inline
        AStringBase()
        : TStringBase<TChar>()
        , capacity  (0)
        {}

        /** ****************************************************************************************
         * Constructor allocating a specific given buffer size
         * (plus one for a potential termination character).
         *
         *  \note This constructor is useful for \b %AString objects whose minimum future string length
         *        is predictable to avoid recurring incremental allocations up to the known
         *        minimum size.
         *
         * @param initialCapacity  The size of the buffer that is allocated.
         ******************************************************************************************/
        explicit
        inline
        AStringBase( int64_t initialCapacity )
        : TStringBase<TChar>()
        , capacity(0)
        {
            SetBuffer( static_cast<integer>( initialCapacity ) );
        }

        /** ****************************************************************************************
         * Overloaded version of #AStringBase(int64_t).
         *
         * @param initialCapacity  The size of the buffer that is allocated.
         ******************************************************************************************/
        explicit
        inline
        AStringBase( int32_t initialCapacity )
        : TStringBase<TChar>()
        , capacity(0)
        {
            SetBuffer( static_cast<integer>( initialCapacity ) );
        }

        /** ****************************************************************************************
         * Overloaded version of #AStringBase(int64_t).
         *
         * @param initialCapacity  The size of the buffer that is allocated.
         ******************************************************************************************/
        explicit
        inline
        AStringBase( size_t initialCapacity )
        : TStringBase<TChar>()
        , capacity(0)
        {
            SetBuffer( static_cast<integer>( initialCapacity ) );
        }

        /** ****************************************************************************************
         * Copy constructor that allocates memory and copies the contents of the given object.
         * @param copy The object to copy.
         ******************************************************************************************/
        explicit
        inline
        AStringBase( const AStringBase& copy)
        : TStringBase<TChar>()
        , capacity  (0)
        {
            Apply( copy );
        }

        /** ****************************************************************************************
         * Move constructor.
         * See \ref alib_namespace_strings_astring_move "Move Constructor and Move Assignment"
         * for details.
         * @param move The object to move.
         ******************************************************************************************/
        inline
        AStringBase(AStringBase&& move) noexcept
        : TStringBase<TChar>()
        {
            // given move object has external buffer: we have to copy
            if ( !move.HasInternalBuffer() )
            {
                capacity=   0;
                Apply( move );
                return;
            }

            // copy values
            StringBase<TChar>::buffer=         move.buffer;
            StringBase<TChar>::length=         move.length;
            capacity=       move.capacity;

            // clean moved object (buffer does not need to be nulled)
            move.capacity=
            move.length=     0;

            // in debug mode, more copying and more destructor prevention is needed
            #if ALIB_DEBUG_STRINGS
                debugLastAllocRequest=                  move.debugLastAllocRequest;
                TStringBase<TChar>::debugIsTerminated=  move.debugIsTerminated;
                debugBufferWithMagicBytePadding=        move.debugBufferWithMagicBytePadding;

                move.buffer=  nullptr;
            #endif
        }

        /** ****************************************************************************************
         *  Constructs the object and uses <em>Apply</em> to append objects of
         *  \ref Apply "applicable type".
         *
         * @tparam TApplicable   The type of parameter \p{source}.
         * @param  src           The source of type T to append.
         ******************************************************************************************/
        template <class TApplicable>
        inline
        explicit
        AStringBase (const  TApplicable& src )
        : TStringBase<TChar>()
        , capacity  (0)
        {
            Apply( src );
        }

        /** ****************************************************************************************
         * Constructor copying a region of a \alib{strings,StringBase,String}.
         * @param src          The reference to the \b %String to copy from.
         * @param regionStart  The start of the region in \p{src} to append.
         * @param regionLength The length of the region in \p{src} to append.
         *                     Defaults to \b %MaxLen.
         ******************************************************************************************/
        explicit
        inline
        AStringBase(const StringBase<TChar>& src, integer regionStart, integer regionLength= MaxLen )
        : TStringBase<TChar>()
        , capacity  (0)
        {
            _<true>( src, regionStart, regionLength );
        }

        /** ****************************************************************************************
         *  Destructs an \b %AString object
         ******************************************************************************************/
        ~AStringBase() noexcept
        {
            ALIB_STRING_DBG_CHK(this)
            if ( HasInternalBuffer() )
                #if !ALIB_DEBUG_STRINGS
                    delete[] StringBase<TChar>::buffer;
                #else
                    delete[] (StringBase<TChar>::buffer - (debugBufferWithMagicBytePadding ? 16 : 0) );
                #endif
        }

        /** ****************************************************************************************
         * The <em>default</em> C++ assign operator (ClassT & operator=(const ClassT &) ).
         * Copies the contents of the given object \p{copy}.
         * \note There is no move assignment operator defined.
         *       In general, it might be advisable to use a combination of #Clear and #_(const T&)
         *       instead of using assignments.
         *       See \ref alib_namespace_strings_astring_move "Move Constructor and Move Assignment"
         *       for an explanation.
         *
         * @param  copy  The object to copy the contents from.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        AStringBase& operator= (const AStringBase&  copy)
        {
            if ( copy.IsNull() )
            {
                SetNull();
                return *this;
            }

            Clear().Apply( copy );
            return  *this;
        }

        /** ****************************************************************************************
         * Clears this object and uses <em>Apply</em> to append objects of arbitrary type.
         * See \ref Apply for more information.
         *
         * \note In general, it might be advisable to use a combination of #Clear and #_(const T&)
         *       instead of using assignments.
         *       See \ref alib_namespace_strings_astring_move "Move Constructor and Move Assignment"
         *       for an explanation.
         *
         * @tparam T      The type of parameter \p{source}.
         * @param  arg    The source of type T to append.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template<typename T>
        inline
        AStringBase& operator= (const  T& arg )
        {
            Clear();
            if ( Apply<true>( arg ) < 0 )
                SetNull();
            return *this;
        }


    /** ############################################################################################
     * @name Memory allocation and buffer access
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Resizes the internal buffer to meet exactly the given size.
         *
         * The following rules apply:
         * - The string represented by this instance is copied to the new buffer.
         *   If this is larger than the new buffer size, the string is cut at the end to fit.
         * - If the desired new size is 0, then the currently allocated buffer will be released
         *   and the object's state is \e nulled.
         * - If the current buffers' life-cycle is managed externally (e.g. was set using
         *   #SetBuffer(TChar*,integer,integer, lang::Responsibility)
         *   with parameter \p{responsibility} being \c Responsibility::KeepWithSender), this method
         *   will replace the buffer by a new one, even if the new requested size is the same as
         *   the external buffers' size. In other words, the only case when this method does not
         *   replace the current buffer is when the current buffers' life-cycle is (already)
         *   internally managed and it has the same size than what is requested.
         * - In the C++ version of \alib, the \c true allocation size is + 1 of what is given
         *   in parameter \p{newCapacity}. This is to duly apply to a constraint given by parent
         *   #TString.
         *
         * \note Any methods of this class that extend the length of the string represented, will
         *       invoke this method if the current buffer size is not sufficient.
         *       If a future string length of an \b %AString is predictable, then it is advisable
         *       to allocate such size prior to start appending the data to avoid recurring
         *       allocations (performance optimization).
         * <p>
         *
         * \note
         *   If currently an external buffer is set (this means a buffer whose life-cycle
         *   is not controlled by this instance), an one-time warning per instance is
         *   issued.
         *   To enable/disable this warning use macro
         *   - \ref ALIB_WARN_ONCE_PER_INSTANCE_ENABLE(theInstanceInQuestion, ReplaceExternalBuffer) or
         *   - \ref ALIB_WARN_ONCE_PER_INSTANCE_DISABLE(theInstanceInQuestion, ReplaceExternalBuffer).

         *
         * @param newCapacity   The new capacity of the internal buffer.
         ******************************************************************************************/
        ALIB_API  void    SetBuffer( integer newCapacity );



        /** ****************************************************************************************
         * This methods replaces the current buffer with the one provided.
         *
         * The following rules apply:
         * - If a nullptr is provided still, the current buffer is released.
         * - If provided buffer is not nullptr, its size provided with parameter \p{extBufferSize}
         *   has to be at least 1 for
         *   providing the space for a termination character.
         * - After the operation, #Capacity will report \p{extBufferSize} \c -1.
         * - Optional parameter \p{responsibility} can be used to pass the responsibility for the
         *   deletion of the buffer to this object.
         * - The length of the content provided with parameter \p{extLength} must not exceed
         *   the value of parameter \p{extBufferSize} \c -1.
         * - In no event any data of an existing buffer is copied into the new one. The rationale
         *   here is that in most use cases, this is not needed. Should this be desired,
         *   then the contents has to be copied manually, e.g. by invoking
         *   <code>strcpy( extBuffer, target.ToCString() )</code> prior to invoking this method.
         *
         * @param extBuffer       The external buffer to use.
         * @param extBufferSize   The size of the given buffer.
         * @param extLength       The length of any content located in the given buffer that should
         *                        be used.
         *                        Has to be smaller or equal to extBufferSize -1 to preserve
         *                        space for a trailing '\0'.
         * @param responsibility  If \c Responsibility::Transfer, the given buffer will be deleted
         *                        by this object when a new buffer is set or it is deleted itself.
         *                        Defaults to \c Responsibility::KeepWithSender which denotes that
         *                        the life-cycle of the given external buffer is managed elsewhere.
         ******************************************************************************************/
        ALIB_API
        void     SetBuffer( TChar*               extBuffer,
                            integer              extBufferSize,
                            integer              extLength          = 0,
                            lang::Responsibility responsibility     = lang::Responsibility::KeepWithSender );



        /** ****************************************************************************************
         * Sets a new length for this string.
         *
         * The <em>checking version</em> (\p{TCheck}=\c true) of this method must be used to shorten
         * the string only. If the new length is requested to be higher than the current length,
         * a one time warning is issued. The warning occurs only once for the first
         * occurrence of an invocation of this method with such wrong parameter.
         * To enable/disable this warning use macros
         * - \ref ALIB_WARN_ONCE_PER_TYPE_ENABLE(String, SetLengthLonger) or
         * - \ref ALIB_WARN_ONCE_PER_TYPE_DISABLE(String, SetLengthLonger).
         *
         * The <em>non-checking</em> version may be used to increase the length. Here, an
         * assertion is only raised, when the length is negative or greater than the
         * current #Capacity. Therefore, the non-checking version <em>has to</em> be used when
         * external code changes the size of the string represented in an \c %AString buffer.
         * Specializations of function
         * \alib{strings,T_Apply} regularly do that.
         *
         * Furthermore, if \ref ALIB_DEBUG_STRINGS is \c true, the non-checking version applies
         * \ref  ALIB_STRING_DBG_UNTERMINATE to this object, after the new length was
         * set and before \ref ALIB_STRING_DBG_CHK is executed.
         *
         * @tparam TCheck    Defaults to \c true which is the normal invocation mode.
         *                   If \c \<false\> is added to the method name, no parameter checks are
         *                   performed.
         * @param newLength  The new length of the \b %AString. Must be between 0 and the current
         *                   length.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        void    SetLength( integer newLength )
        {
            if ( TCheck )
            {
                ALIB_STRING_DBG_CHK(this)
                #if ALIB_DEBUG
                    if( StringBase<TChar>::ALIB_OTW_SetLengthLonger && !(newLength <= StringBase<TChar>::length) )
                    {
                        StringBase<TChar>::ALIB_OTW_SetLengthLonger= false;
                        ALIB_WARNING( ASTR("Increase requested") );
                    }
                #endif




                ALIB_ASSERT_ERROR(     newLength >= 0,      ASTR("Negative length") );
                if ( newLength >= 0 && newLength < StringBase<TChar>::length )
                    StringBase<TChar>::length= newLength;
                ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            }
            else
            {
                // in the non-checking version, the debug check must be done only after setting the
                // length.
                ALIB_ASSERT_ERROR( newLength >= 0 && newLength <= Capacity(),
                   (AString(ASTR("NC: Length out of range. capacity=")) << capacity
                        <<  ASTR(", newLength=")  << newLength).ToCString()    )
                StringBase<TChar>::length= newLength;
                ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
                ALIB_STRING_DBG_CHK(this)
            }
        }

        /** ****************************************************************************************
         * Ensures that the capacity of the internal buffer meets or exceeds the actual length
         * plus the given growth value.
         *
         * @param spaceNeeded  The desired growth of the length of the string represented by this.
         ******************************************************************************************/
        inline
        void     EnsureRemainingCapacity( integer spaceNeeded )
        {
            #if !ALIB_DEBUG_STRINGS
                if ( Capacity() < StringBase<TChar>::length + spaceNeeded )
                    GrowBufferAtLeastBy( spaceNeeded );
            #else
                ALIB_ASSERT_ERROR( StringBase<TChar>::length <= debugLastAllocRequest,
                    ASTR("Previous allocation request was too short") );
                debugLastAllocRequest= StringBase<TChar>::length + spaceNeeded;

                if ( Capacity() < StringBase<TChar>::length + spaceNeeded )
                    GrowBufferAtLeastBy( spaceNeeded );

                ALIB_STRING_DBG_UNTERMINATE(TChar,*this, spaceNeeded)
            #endif
        }

        /** ****************************************************************************************
         * Increases the allocation size at least by the value provided.
         *
         * @param minimumGrowth    The desired minimum growth of length.
         ******************************************************************************************/
        void     GrowBufferAtLeastBy( integer minimumGrowth )
        {
            integer actCapacity= Capacity();

            // big enough?
            ALIB_ASSERT_WARNING ( actCapacity < StringBase<TChar>::length + minimumGrowth,
                                  ASTR("Unnecessary invocation of Grow()") );

            // first allocation? Go with given growth as size
            if (actCapacity == 0 )
            {
                SetBuffer( minimumGrowth > 16 ? minimumGrowth : 16 );
                return;
            }

            // calc new size: in general grow by 50%
            integer newCapacity= actCapacity + (actCapacity / 2);
            if ( newCapacity < StringBase<TChar>::length + minimumGrowth )
                newCapacity+= minimumGrowth;

            if ( newCapacity < 16 )
                newCapacity= 16;

            SetBuffer( newCapacity );
        }

        /** ****************************************************************************************
         * The current size of the internal buffer (this is excluding the trailing '\0'character)
         * which is reserved to terminate the represented character array if needed.
         * In other words, the internal memory allocated is the size returned here plus one.
         *
         * @return The size of the allocated buffer.
         ******************************************************************************************/
        inline
        integer Capacity()  const
        {
            return  capacity >= 0   ?  capacity
                                    : -capacity;
        }

        /** ****************************************************************************************
         * Returns \c true, if the buffer was allocated by this class itself. If the buffer was
         * set using #SetBuffer(TChar*,integer,integer,lang::Responsibility) with parameter \p{responsibility}
         * given as \c Responsibility::KeepWithSender (and not automatically replaced, yet,
         * because it became too small) then \c false is returned.
         * \note Derived class
         *       \alib{strings,PreallocatedStringBase,PreallocatedString} will report \c false here.
         *       This sounds wrong on the first sight, as the buffer is allocated by an 'internal'
         *       member. But from an AString's perspective, class <em>PreallocatedString</em> works on
         *       an 'external' buffer.
         *
         * @return \c true if the buffer is internally allocated and will be deleted with the
         *         deletion of this object. False otherwise.
         ******************************************************************************************/
        inline
        bool HasInternalBuffer()
        {
            return  capacity > 0;
        }

        /** ****************************************************************************************
         * Invokes \ref SetBuffer "SetBuffer(0)".
         ******************************************************************************************/
        inline
        void           SetNull()
        {
            SetBuffer( 0 );
        }

    /** ############################################################################################
     * @name Writable Buffer Access
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * The internal buffer character array provided as non constant character pointer.
         * \see Chapter
         * \ref alib_namespace_strings_astring_write_access "Write Access to the Buffer"
         * of the reference documentation of this class.
         *
         * @return The internal buffer array.
         ******************************************************************************************/
        inline
        TChar*          VBuffer()         const       { return StringBase<TChar>::vbuffer;    }

        /** ****************************************************************************************
         * Sets the character at the given index. A range check is performed. If this fails,
         * nothing is done.
         *
         * \note
         *   The C++ language would allow to declare this method const, as it does not
         *   manipulate the data of the class itself but a character in the buffer pointer.<br>
         *   In exclamation cases, to manipulate the contents of <em>const %AString</em>, use
         *   method VBuffer() like in the following sample:
         *   \snippet "DOX_ALIB_ASTRING.cpp" DOX_ALIB_ASTRING_MODIFY_CONST_BUFFER
         *   <p>
         *
         * @tparam TCheck Performs a range check on the given index and a check for illegal setting
         *                of termination character '\0' anywhere else but at idx==length.
         * @param idx     The index of the character to write.
         * @param c       The character to write.
         ******************************************************************************************/
        template<bool TCheck =true>
        inline
        void        SetCharAt( integer idx, TChar c )
        {
            ALIB_ASSERT_ERROR(  c != '\0' || idx==StringBase<TChar>::length, ASTR("Can't write character '\0'") );
            if (TCheck)
            {
                if  ( (idx >= 0 && idx < StringBase<TChar>::length) || ( c =='\0' && idx == StringBase<TChar>::length ) )
                    *(StringBase<TChar>::vbuffer + idx )= c;
            }
            else
            {
                ALIB_ASSERT_ERROR(  idx >= 0 && idx < StringBase<TChar>::length, ASTR("NC: Index out of range") );
                *(StringBase<TChar>::vbuffer + idx )= c;
            }
        }

        /** ****************************************************************************************
         * Provides read/write access to single characters.
         * Overrides
         * \alib{strings,StringBase::operator[],String::operator[]}
         * returning a reference to a TChar which allows assignments of values when using an
         * object with this operator as lvalue.
         *
         * \attention
         *   Unlike other operator methods in the family of string classes of \alib,  which are doing
         *   parameter checks (in this case a range check), this operator does <em>not</em> do a
         *   check!
         *   The rationale is that, no reference to a character can be given if \p{op} is out
         *   of range.<br>
         *   This way, this method is equivalent to methods #SetCharAt<false> and #CharAt<false>.
         *   For safe access to characters in the buffer use #SetCharAt and #CharAt
         *   (with template parameter \p{TCheck} being \c true).
         *
         * @param op The index of the character within this objects' buffer.
         * @returns If the character contained (or, if lvalue the one to set).
         ******************************************************************************************/
        inline
        TChar&    operator[] (integer  op)
        {
            ALIB_ASSERT_ERROR( op >= 0  && op < StringBase<TChar>::length , ASTR("Index out of bounds") );
            return StringBase<TChar>::vbuffer[op];
        }


    /** ############################################################################################
     * @name Insert and Delete
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Clear the Buffer. Same as \ref Delete "Delete(0, Length())" (but of-course, without
         * internal region checks).
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        AStringBase&    Clear()
        {
            ALIB_STRING_DBG_CHK(this)

            StringBase<TChar>::length=            0;
            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            return *this;
        }

        /** ****************************************************************************************
         * Clear the Buffer. Same as #Clear(), really just a synonym to allow short code in
         * alignment with the various "Apply" methods named <em>_(type src)</em>
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        AStringBase&    _()
        {
            ALIB_STRING_DBG_CHK(this)

            StringBase<TChar>::length=            0;
            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            return *this;
        }

        /** ****************************************************************************************
         * Inserts the given strings at given position.
         * If the position is not between 0 and the length of the target, nothing is inserted.
         *
         * \note
         *   To insert a string with replacing a different one at the same time, use
         *   \ref ReplaceSubstring(const StringBase<TChar>&,integer,integer) "ReplaceSubstring(src, pos, regionLength)".
         *
         * @tparam TCheck   Chooses checking or non-checking implementation. Defaults to true.
         * @param  src      The \b %String to insert.
         * @param  pos      The position to insert \p{src}.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        AStringBase&   InsertAt( const StringBase<TChar>& src, integer pos )
        {
            ALIB_STRING_DBG_CHK(this)
            integer srcLength= src.Length();
            if ( TCheck )
            {
                if ( srcLength == 0 || pos < 0 || pos > StringBase<TChar>::length )
                    return *this;
            }
            else
                ALIB_ASSERT_ERROR( srcLength > 0 && pos >=0 && pos <= StringBase<TChar>::length,
                                   ASTR("NC: Illegal parameters") );

            EnsureRemainingCapacity( srcLength );

            // move content and fill new region
            CString<TChar>::Move( StringBase<TChar>::vbuffer + pos,
                                           StringBase<TChar>::length -  pos,
                                           StringBase<TChar>::vbuffer + pos + srcLength    );
            StringBase<TChar>::length+= src.CopyTo( StringBase<TChar>::vbuffer + pos );

            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            return *this;
        }

        /** ****************************************************************************************
         * Inserts the given character n-times at a given position.
         * If the given position is out of range, nothing is inserted.
         *
         * The non-checking version does not check the position. However the default value
         * which is appending at the end) is still verified.
         *
         * @param c     The character to insert \p{qty} times.
         * @param qty   The quantity of characters to insert.
         * @param pos   The index in this object where \p{c} is inserted \p{qty} times.
         *              Defaults to \b %MaxLen.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        AStringBase&   InsertChars( TChar c, integer qty, integer pos= MaxLen )
        {
            if (pos == MaxLen )
                pos= StringBase<TChar>::length;

            if( TCheck )
            {
                if ( qty <= 0 || pos < 0 ||  pos > StringBase<TChar>::length )
                    return *this;
            }
            #if ALIB_DEBUG
            else
            {
                ALIB_ASSERT_ERROR( qty >= 0,
                                   ASTR("NC: Illegal quantity given") )
                ALIB_ASSERT_ERROR( pos >= 0 && pos <= StringBase<TChar>::length,
                                   ASTR("NC: Illegal position given") )
            }
            #endif

            EnsureRemainingCapacity( qty );

            // move content ?
            if ( pos != StringBase<TChar>::length )
            {
                CString<TChar>::Move( StringBase<TChar>::vbuffer + pos,
                                      StringBase<TChar>::length  - pos,
                                      StringBase<TChar>::vbuffer + pos + qty         );
            }

            // set
            CString<TChar>::Fill( StringBase<TChar>::vbuffer + pos, c, qty );
            SetLength<false>( StringBase<TChar>::length +  qty );
            return *this;
        }

        /** ****************************************************************************************
         * Replaces a region in this object by a given string.
         * If the region does not fit to this object, then nothing is done.
         *
         * The non-checking version does not adjust the region.
         *
         * @tparam TCheck   Chooses checking or non-checking implementation. Defaults to true.
         * @param src             The replacement string.
         * @param regionStart     The start of the region.
         * @param regionLength    The length of the region.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        AStringBase&   ReplaceSubstring( const StringBase<TChar>& src, integer regionStart, integer regionLength )
        {
            ALIB_STRING_DBG_CHK(this)
            integer srcLength= src.Length();

            if( TCheck )
            {
                if (    regionStart  < 0
                    ||  regionStart + regionLength > StringBase<TChar>::length
                    ||  regionLength < 0
                    ||  srcLength    < 0)    // TOOD ???
                        return *this;
            }
            else
            {
                ALIB_ASSERT_ERROR( src.IsNotNull(), ASTR("NC: Source string is nulled")  );
                #if ALIB_DEBUG
                    integer rs=  regionStart;
                    integer rl=  regionLength;
                    StringBase<TChar>::AdjustRegion( rs, rl );
                    ALIB_ASSERT_ERROR(  rs == regionStart && rl == regionLength, ASTR("NC: Invalid region given") )
                #endif
            }

            integer lenDiff= srcLength - regionLength;

            // check buffer size
            if ( lenDiff > 0 )
                EnsureRemainingCapacity( lenDiff );

            // move content
            if ( lenDiff != 0 )
                CString<TChar>::Move( StringBase<TChar>::vbuffer + regionStart + regionLength,
                                               StringBase<TChar>::length  - (regionStart + regionLength),
                                               StringBase<TChar>::vbuffer + regionStart + srcLength       );

            // copy the source
            src.CopyTo( StringBase<TChar>::vbuffer + regionStart );
            StringBase<TChar>::length+= lenDiff;

            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            return *this;
        }

        /** ****************************************************************************************
         * Replaces a region in the string with the given character.
         * If the region does not fit to this object, then nothing is done.
         *
         * The non-checking version does not adjust the region.
         *
         * @tparam TCheck   Chooses checking or non-checking implementation. Defaults to true.
         * @param regionStart     The start of the region
         * @param regionLength    The length of the region
         * @param c               The character to set in the region.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        AStringBase&   ReplaceRegion( TChar c, integer regionStart, integer regionLength )
        {
            if( TCheck )
            {
                if ( StringBase<TChar>::AdjustRegion( regionStart, regionLength ) )
                    return *this;
            }
            else
            {
                #if ALIB_DEBUG
                    integer rs=  regionStart;
                    integer rl=  regionLength;
                    StringBase<TChar>::AdjustRegion( rs, rl );
                    ALIB_ASSERT_ERROR( rs == regionStart && rl == regionLength, ASTR("NC: Invalid region given") )
                #endif
            }
            CString<TChar>::Fill( StringBase<TChar>::vbuffer + regionStart, c, regionLength );
            return *this;
        }


        /** ****************************************************************************************
         * Deletes a region from the Buffer. A range check is performed and the region is cut
         * to fit to the string.
         *
         * The non-checking version (\p{TCheck} = \c false) still allows to leave parameter
         * \p{regionLength} as default (respectively allows the sum of parameters
         * \p{regionStart} and \p{regionLength} to be longer than the length of this \b %AString).
         *  In this case, this string is cut starting from index \p{regionStart}.
         *
         * \note See also methods #Clear, #DeleteStart and #DeleteEnd.
         *
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c \<false\> is added to the method name, no parameter checks are
         *                      performed.
         * @param regionStart   The start of the region to delete.
         * @param regionLength  The length of the region to delete. Defaults to \b %MaxLen.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true >
        inline
        AStringBase&    Delete( integer regionStart, integer regionLength= MaxLen  )
        {
            ALIB_STRING_DBG_CHK(this)

            integer regionEnd;

            if ( TCheck )
            {
                if ( StringBase<TChar>::AdjustRegion( regionStart, regionLength ) )
                {
                    ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
                    return *this;
                }

                // delete over the end?
                if ( (regionEnd= regionStart + regionLength) >= StringBase<TChar>::length )
                {
                    StringBase<TChar>::length= regionStart;
                    ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
                    return *this;
                }
            }
            else
            {
                ALIB_ASSERT_ERROR( regionStart  >= 0
                                   &&  regionStart  <= StringBase<TChar>::length
                                   &&  regionLength >= 0,
                                   ASTR("NC: Illegal arguments") );

                // delete over the end?
                if ( (regionEnd= regionStart + regionLength) >= StringBase<TChar>::length )
                {
                    StringBase<TChar>::length= regionStart;
                    ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
                    return *this;
                }
            }

            // both versions
            CString<TChar>::Move( StringBase<TChar>::vbuffer + regionEnd,
                                           StringBase<TChar>::length  - regionEnd + 1,
                                            StringBase<TChar>::vbuffer + regionStart   );
            StringBase<TChar>::length-= regionLength;
            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            return *this;
        }

        /** ****************************************************************************************
         * Deletes the given number of characters from the start of the string.
         *
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c \<false\> is added to the method name, no parameter checks are
         *                      performed.
         *
         * @param regionLength  The length of the region at the start to delete.
         *
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true >
        inline
        AStringBase&                 DeleteStart( integer regionLength )
        {
            ALIB_STRING_DBG_CHK(this)

            if ( TCheck )
            {
                if ( regionLength <= 0 )
                {
                    ALIB_STRING_DBG_CHK(this)
                    ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
                    return *this;
                }
                if ( regionLength >= StringBase<TChar>::length )
                    return Clear();
            }

            ALIB_ASSERT_ERROR(  regionLength >=0 && regionLength <= StringBase<TChar>::length,
                                    ASTR("NC: Region length out of range") );

            CString<TChar>::Move( StringBase<TChar>::buffer + regionLength,
                                           StringBase<TChar>::length - regionLength + 1,
                                           StringBase<TChar>::vbuffer                     );
            StringBase<TChar>::length-= regionLength;
            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            return *this;
        }

        /** ****************************************************************************************
         * Deletes the given number of characters from the end of the string.
         *
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c \<false\> is added to the method name, no parameter checks are
         *                      performed.
         * @param regionLength  The length of the region at the end to delete.
         *
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true >
        inline
        AStringBase&                DeleteEnd( integer regionLength  )
        {
            ALIB_STRING_DBG_CHK(this)

            if (TCheck)
            {
                if ( regionLength > 0 )
                {
                    if ( regionLength >= StringBase<TChar>::length )
                        StringBase<TChar>::length= 0;
                    else
                        StringBase<TChar>::length-= regionLength;
                }
            }
            else
            {
                ALIB_ASSERT_ERROR(  regionLength >=0 && regionLength <= StringBase<TChar>::length,
                                    ASTR("NC: Region length out of range") );
                StringBase<TChar>::length-= regionLength;
            }

            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            return *this;
        }

        /** ****************************************************************************************
         * All characters defined in given set are removed at the beginning and at the end of this
         * string.
         *
         * \see Method #TrimAt to remove whitespaces at arbitrary places in the string.
         *
         * @param trimChars   Pointer to a zero terminated set of characters to be omitted.
         *                    Defaults to \ref DefaultWhitespaces.
         * @return  \c *this to allow concatenated calls.
         ******************************************************************************************/
        ALIB_API
        AStringBase& Trim( const TStringBase<TChar>& trimChars = StringConstants<TChar>::DefaultWhitespaces );

        /** ****************************************************************************************
         * All characters defined in given set at, left of and right of the given index
         * are removed from the string.<br>
         * The method returns index of the first character of those characters that were behind the
         * trimmed region. With legal \p{idx} given, this value can only be smaller or equal to
         * \p{idx}. If \p{idx} is out of bounds, the length of the string is returned.
         *
         * @param idx         The index to perform the trim operation at. Has to be between zero
         *                    and <em>Length() -1</em>.
         * @param trimChars   Pointer to a zero terminated set of characters to be omitted.
         *                    Defaults to \ref DefaultWhitespaces.
         * @return  The index of the first character of those characters that were behind the
         *          trimmed region.
         ******************************************************************************************/
        ALIB_API
        integer  TrimAt( integer idx,
                         const TStringBase<TChar>& trimChars = StringConstants<TChar>::DefaultWhitespaces );

        /** ****************************************************************************************
         * All characters defined in given set are removed at the beginning of this string.
         *
         * \see Method #TrimAt to remove whitespaces at arbitrary places in the string.
         *
         * @param trimChars   Pointer to a zero terminated set of characters to be omitted.
         *                    Defaults to \ref DefaultWhitespaces.
         * @return  \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        AStringBase& TrimStart( const TStringBase<TChar>& trimChars = StringConstants<TChar>::DefaultWhitespaces )
        {
            if (StringBase<TChar>::length == 0 || trimChars.IsEmpty() )
                return *this;

            integer idx= StringBase<TChar>::template IndexOfAny<Inclusion::Exclude, false>( trimChars );
            if ( idx > 0 )
                Delete<false>( 0, idx );
            else if ( idx < 0 )
                StringBase<TChar>::length= 0;

            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            return *this;
        }

        /** ****************************************************************************************
         * All characters defined in given set are removed at the end of this string.
         *
         * \see Method #TrimAt to remove whitespaces at arbitrary places in the string.
         *
         * @param trimChars   Pointer to a zero terminated set of characters to be omitted.
         *                    Defaults to \ref DefaultWhitespaces.
         * @return  \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        AStringBase& TrimEnd( const TStringBase<TChar>& trimChars = StringConstants<TChar>::DefaultWhitespaces )
        {
            if( StringBase<TChar>::length > 0 &&  trimChars.IsNotEmpty() )
            {
                StringBase<TChar>::length= StringBase<TChar>::template LastIndexOfAny<Inclusion::Exclude, false>( trimChars, StringBase<TChar>::length - 1 ) + 1;
                ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
            }
            return *this;
        }

    /** ############################################################################################
     * @name Append and apply
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Appends an array of one byte characters.
         *
         * @param  src         A pointer to the start of the array to append.
         * @param  srcLength   The length of the string.
         * @tparam TCheck      Defaults to \c true which is the normal invocation mode.
         *                     If \c \<false\>, no nullptr check is done on parameter \p{src}.
         *                     Also, this object would not loose a \e nulled state when the given
         *                     cstring portion is empty.
         *
         * @return    \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true >
        AStringBase& Append( const nchar* src, integer srcLength );

        /** ****************************************************************************************
         * Appends an array of wide characters.
         *
         * @param  src         A pointer to the start of the array to append.
         * @param  srcLength   The length of the string.
         * @tparam TCheck      Defaults to \c true which is the normal invocation mode.
         *                     If \c \<false\>, no nullptr check is done on parameter \p{src}.
         *                     Also, this object would not loose a \e nulled state when the given
         *                     cstring portion is empty.
         *
         * @return    \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true >
        AStringBase& Append( const wchar* src, integer srcLength );


        /** ****************************************************************************************
         * Appends an array of characters of type \c strangeChar.
         *
         * @param  src         A pointer to the start of the array to append.
         * @param  srcLength   The length of the string.
         * @tparam TCheck      Defaults to \c true which is the normal invocation mode.
         *                     If \c \<false\>, no nullptr check is done on parameter \p{src}.
         *                     Also, this object would not loose a \e nulled state when the given
         *                     cstring portion is empty.
         *
         * @return    \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true >
        AStringBase& Append( const strangeChar* src, integer srcLength );

        /** ****************************************************************************************
         * This generic method allows to apply an object of custom type \p{TApplicable}.
         * For most types, 'apply' means to append a string representation of the given object.<br>
         * In detail, the implementation of this method proceeds as follows:
         * - If a pointer type is detected and value is nullptr, nothing is done and -1 is returned.
         *   This allows the caller to react on nullptr values (e.g. the assignment operator,
         *   would set this object to \e nulled state if -1 was returned).
         * - If a character type (\ref aworx::character, \ref aworx::complementChar or
         *   \ref aworx::strangeChar) is passed, it is appended.
         * - If a reference or pointer to a type representing a string value is detected, the
         *   string is appended. %String types detected are:
         *   - \c character*, \c character[]
         *   - \c complementChar*, \c complementChar[]
         *   - \c strangeChar*, \c strangeChar[]
         *   - \alib{strings,StringBase,String},
         *     \ref String, \alib{strings,StringLiteralBase,StringLiteral},
         *     \alib{strings,TStringBase,TString},
         *     \alib{strings,SubstringBase,Substring}
         *   - Custom types that have a specialization of template struct \ref T_String.
         * - If above does not match, it is checked for a specialization of template struct
         *   \ref T_Apply for the given type \p{TApplicable}. For this, unnecessary qualifiers
         *   are removed and if \p{TApplicable} is a pointer type, even this is ignored.
         *   (Hence pointers can be passed to this method without de-referencing them and with only
         *   one specialization of struct \b %TApply.)
         * - If a matching (partially implemented) template struct is found, its method
         *   \b T_Apply<TApplicable>::Apply
         *   is invoked and this way the custom specialization defines what 'applying' of an
         *   object of this type means.
         *
         * See \ref T_Apply for information on how to implement a supporting template struct to
         * allow the application of user defined types.
         *
         * \note
         *   A static assertion tries to detect unsupported types (types that are missing
         *   a specialization of \b %T_Apply) at compile time. However, for technical reasons,
         *   there might be a few types not fetched by this static assert.
         *   In this case an ALIB_WARNING is raised at runtime and 0 is returned.
         *
         * <b>Usage</b><p>
         *  This method not directly used by standard user code. However, indirectly it is used
         *  through:
         *  - Constructor AString(const T& src),
         *  - assignment \ref operator=(const T& src)
         *  - "apply-operator" #operator<<(const T& src) and
         *  - "apply-method"   #_(const T& src).
         *
         *  which each provides (therefore) huge flexibility in respect to their operands.
         *
         * <b>Built-in Applicable Types</b><p>
         * For a list of built-in types that are applicable using this method, respectively the
         * methods listed in the previous section, see
         * \ref alib_namespace_strings_astring_application_vs_interface "Application Instead of Explicit Interface Methods".
         *
         * <b>Non-checking version</b><p>
         * If template parameter \p{TCheck} is provided as \c false on invocation, two things are
         * omitted:
         * - There is no check for nullptr objects (attention!)
         * - If this is a \e nulled \b %AString object, and the length of the given parameter \p{src}
         *   to append is zero (e.g. an empty string), this object would not loose
         *   its \e nulled state. In other words, \e nulled strings keep being \e nulled if empty objects
         *   are appended when invoking this method with \p{TCheck}= \c false.<p>
         * \note The one and only motivation for using the non-checking version of this method is
         *       to improve performance in critical code sections.
         *
         * \note This method's description may sound (and the code may look) like having quite
         *       some runtime overhead. However, due to TMP, there is no overhead involved with
         *       this mechanism, in respect to the alternative of having overloaded apply methods
         *       implemented.
         *
         * <b>Sample</b><p>
         * Besides the implementations of <em>T_Apply</em> for various types like integer or double,
         * \alib provides class \alib{strings,FormatBase,Format} having public
         * inner classes with simple constructors that are useful to pass to the methods
         * listed above like in the following sample:
         *
         * \snippet "DOX_ALIB_ASTRING.cpp"     DOX_ALIB_ASTRING_FORMAT
         *
         * The output will be:
         *
         * \snippet "DOX_ALIB_ASTRING_FORMAT.txt"     OUTPUT
         *
         * @tparam TCheck      Defaults to \c true which is the normal invocation mode.
         *                     If \c \<false\> is added to the method name, checks are omitted
         *                     as documented above.
         * @tparam TApplicable The template type of parameter \p{source}.
         * @param  src         The source of type \p{TApplicable} to append.
         *
         * @return -1 if the given object represents a \e nulled string. Otherwise 0 or positive
         *         value is returned (indicating the number of characters that were appended,
         *         but not necessarily correct). Depends on the implementation of the template
         *         method \ref T_Apply which is invoked in turn.
         ******************************************************************************************/
        template <bool TCheck= true, typename TApplicable>
        inline
        integer Apply(const  TApplicable& src )
        {
            ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING;
            ALIB_STRING_DBG_CHK(this);

            // nullptr ?
            if( std::is_same<TApplicable, decltype(nullptr)>::value )
                return -1;

            //---------- single character? ----------
            using TRemovedCV= typename std::remove_cv<TApplicable>::type;
            if(    std::is_same<TChar, TRemovedCV> ::value )
            {
                TChar c= *(TChar*) &src;
                if ( TCheck && c == '\0' )
                    return -1;

                EnsureRemainingCapacity( 1 );
                StringBase<TChar>::vbuffer[ StringBase<TChar>::length++ ]= c;
                ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0);
                return 1;
            }

            //---------- incompatible character? ----------
            if(    std::is_same<TRemovedCV, complementChar> ::value
                || std::is_same<TRemovedCV, strangeChar   > ::value  )
            {
                // incompatible char on  narrow string
                if(    std::is_same<TChar, nchar> ::value )
                {
                    wchar wc;
                         if(  std::is_same<TRemovedCV, wchar> ::value ) wc= (wchar) *(wchar*      ) &src;
                    else                                                wc= (wchar) *(strangeChar*) &src;

                    if ( TCheck && wc == L'\0' )
                        return -1;

                    int mbLength;
                    #if defined(_MSC_VER)
                        //does not work need utf8 encoding
                        //mblength= wctomb_s( &mbLength, vbuffer + length, 16, wc);
                        //mbLength= wctomb( nullptr, wc );
                        //mbLength= wctomb( vbuffer + length, wc );
                        EnsureRemainingCapacity( MB_LEN_MAX * 2);
                        mbLength= WideCharToMultiByte( CP_UTF8, NULL, &wc, 1,  ((char*) StringBase<TChar>::vbuffer) + StringBase<TChar>::length, MB_LEN_MAX * 2, NULL, NULL );
                        if ( mbLength <= 0 )
                        {
                            ALIB_DBG( DWORD error= GetLastError(); )
                            ALIB_WARNING( ASTR("AString: Cannot convert wide character string to UTF-8. Error: "),
                                           (   error == ERROR_INSUFFICIENT_BUFFER    ? ASTR("ERROR_INSUFFICIENT_BUFFER"   )
                                            :  error == ERROR_INVALID_FLAGS          ? ASTR("ERROR_INVALID_FLAGS."        )
                                            :  error == ERROR_INVALID_PARAMETER      ? ASTR("ERROR_INVALID_PARAMETER"     )
                                            :  error == ERROR_NO_UNICODE_TRANSLATION ? ASTR("ERROR_NO_UNICODE_TRANSLATION")
                                                                                     : (WString64()._( error )).ToCString() ) )
                        }
                    #else
                        EnsureRemainingCapacity( static_cast<integer>(MB_CUR_MAX) + 1);
                        mbLength= wctomb( ((char*) StringBase<TChar>::vbuffer) + StringBase<TChar>::length, wc );
                    #endif

                    if ( mbLength <= 0 )
                    {
                        ALIB_WARNING( ASTR("Cannot convert WC to MBC.") );
                        return 0;
                    }

                    StringBase<TChar>::length+= mbLength;
                    ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0);
                    return mbLength;
                }

                // incompatible char on wide string
                else
                {
                    EnsureRemainingCapacity( 1 );
                    TChar wc;
                         if(  std::is_same<TRemovedCV, nchar>::value ) wc= (TChar) *(nchar*      ) &src;
                    else                                               wc= (TChar) *(strangeChar*) &src;


                    if ( TCheck && wc == '\0' )
                        return -1;
                    StringBase<TChar>::vbuffer[ StringBase<TChar>::length++ ]= static_cast<TChar>(wc);
                    ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0);
                    return 1;
                }
            }


            // Define result and fundamental type
            integer result= -1;
            using TPlain= typename std::remove_cv     <
                          typename std::remove_pointer<
                          typename std::remove_const  < TApplicable >::type>::type>::type;
            //ALIB_TMP_SHOW_TYPE_IN_DEBUGGER(TPlain)

            using TCompChar= typename T_ComplementChar<TChar>::type;

            // string type with fixed length ( e.g. TChar[n] or class StringLiteral )? ----------
            if ( T_StringLiteral<TPlain,TChar>::value )
            {
                constexpr integer TLength= T_StringLiteral<TPlain,TChar>::Length();
                result= TLength;
                if ( TLength > 0 )
                {
                    EnsureRemainingCapacity( TLength );

                    if( std::is_pointer<TApplicable>::value )
                        StringLiteralBase<TChar, (size_t)TLength>::Copy(  StringBase<TChar>::vbuffer + StringBase<TChar>::length,
                                                                          T_StringLiteral<TPlain,TChar>::Buffer( (TPlain&) **(int**) &src ) );
                    else
                        StringLiteralBase<TChar, (size_t)TLength>::Copy(  StringBase<TChar>::vbuffer + StringBase<TChar>::length,
                                                                          T_StringLiteral<TPlain,TChar>::Buffer( (TPlain&) *(int*) &src ) );
                    // adjust length
                    StringBase<TChar>::length+= TLength;
                }
            }

            // TCompChar[] (e.g string literal)
            else if (  std::is_array<TPlain>::value &&  std::is_same<typename std::remove_extent<TPlain>::type, TCompChar>::value )
            {
                constexpr integer TLength= static_cast<integer>( std::extent<TPlain>::value - 1 );
                if ( TLength > 0 )
                    Append<false>((TCompChar*)  &src, TLength );
                result= TLength;
            }

            // strangeChar[] (e.g string literal u"xyz" or U"xyz")
            else if (  std::is_array<TPlain>::value &&  std::is_same<typename std::remove_extent<TPlain>::type, strangeChar>::value )
            {
                constexpr integer TLength= static_cast<integer>( std::extent<TPlain>::value - 1 );
                if ( TLength > 0 )
                    Append<false>((strangeChar*)  &src, TLength );
                result= TLength;
            }

            //---------- String-constructor type? ----------
            else if ( T_String<TPlain,TChar>::value )
            {
                const TChar* sBuffer;
                integer    sLength;
                if ( std::is_pointer<TApplicable>::value )
                {
                    sBuffer= T_String<TPlain,TChar>::Buffer( (TPlain&) **(int**) &src );
                    sLength= T_String<TPlain,TChar>::Length( (TPlain&) **(int**) &src );
                }
                else
                {
                    sBuffer= T_String<TPlain,TChar>::Buffer( (TPlain&) *(int*) &src );
                    sLength= T_String<TPlain,TChar>::Length( (TPlain&) *(int*) &src );
                }
                if( sBuffer )
                    Append<false>( sBuffer, result= sLength );
                else
                    result= -1;
            }

            //---------- String-constructor type of complement character type? ----------
            else if ( T_String<TPlain,TCompChar>::value )
            {
                const TCompChar* sBuffer;
                integer    sLength;
                if ( std::is_pointer<TApplicable>::value )
                {
                    sBuffer= T_String<TPlain,TCompChar>::Buffer( (TPlain&) **(int**) &src );
                    sLength= T_String<TPlain,TCompChar>::Length( (TPlain&) **(int**) &src );
                }
                else
                {
                    sBuffer= T_String<TPlain,TCompChar>::Buffer( (TPlain&) *(int*) &src );
                    sLength= T_String<TPlain,TCompChar>::Length( (TPlain&) *(int*) &src );
                }
                if( sBuffer )
                    Append<false>( sBuffer, result= sLength );
                else
                    result= -1;
            }

            //---------- pointer type? ----------
            else if ( std::is_pointer<TApplicable>::value )
            {
                // nullptr?
                if( TCheck && (*(TChar**)&src) == nullptr )
                    return -1;

                // pointers to fundamental?
                if ( std::is_arithmetic<TPlain>::value )
                {
                    //ALIB::dbgTMPShowTypeInDebugger<TPlain>();

                    // provide character types as pointer
                    if (    std::is_same<TPlain, character     >::value
                         || std::is_same<TPlain, complementChar>::value
                         || std::is_same<TPlain, strangeChar   >::value )
                        result=  T_Apply<TPlain*, TChar>::Apply( *this, *(TPlain**)   &src );

                    // provide character * as pointer
                    else if (    std::is_same<TPlain,       character     *>::value
                              || std::is_same<TPlain, const character     *>::value
                              || std::is_same<TPlain,       complementChar*>::value
                              || std::is_same<TPlain, const complementChar*>::value
                              || std::is_same<TPlain,       strangeChar   *>::value
                              || std::is_same<TPlain, const strangeChar   *>::value
                            )
                        result=  T_Apply<TPlain*, TChar>::Apply( *this, **(TPlain***) &src );

                    // other types as value
                    else
                        result= T_Apply<TPlain,   TChar>::Apply( *this, (TPlain&) **(int**) &src );
                }

                // pointer to sibling string type
                else if ( std::is_base_of<StringBase<TChar>, typename std::remove_pointer<TApplicable>::type>::value )
                {
                    if ( TCheck && (*(StringBase<TChar>**) &src)->IsNull() )
                        return -1;
                    Append<false>(          (*(StringBase<TChar>**) &src)->Buffer(),
                                    result= (*(StringBase<TChar>**) &src)->Length()   );
                }

                // pointer to complementary string type type
                else if ( std::is_base_of<StringBase<TCompChar>, typename std::remove_pointer<TApplicable>::type>::value )
                {
                    if ( TCheck && (*(StringBase<TCompChar>**) &src)->IsNull() )
                        return -1;
                    Append<false>(          (*(StringBase<TCompChar>**) &src)->Buffer(),
                                    result= (*(StringBase<TCompChar>**) &src)->Length()   );
                }

                // pointers to class types?
                else
                    result= T_Apply<TPlain, TChar>::Apply   ( *this, *(TPlain *&)  src );
            }

            //---------- reference type ----------
            else
            {
                // references to fundamental types? -> by reference
                if ( std::is_arithmetic<TPlain>::value )
                    result= T_Apply<TPlain, TChar>::Apply   ( *this, (TPlain&) *(int*) &src );

                // reference to sibling string type
                else if ( std::is_base_of<StringBase<TChar>, typename std::remove_reference<TApplicable>::type>::value )
                {
                    if ( TCheck && ((StringBase<TChar>&) *(int*) &src).IsNull() )
                        return -1;
                    Append<false>(         ((StringBase<TChar>&) *(int*) &src).Buffer(),
                                   result= ((StringBase<TChar>&) *(int*) &src).Length() );
                }

                // reference to complementary string type type
                else if ( std::is_base_of<StringBase<TCompChar>, typename std::remove_reference<TApplicable>::type>::value )
                {
                    if ( TCheck && ((StringBase<TCompChar>&) *(int*) &src).IsNull()  )
                        return -1;
                    if ( TCheck && ((StringBase<TCompChar>&) *(int*) &src).IsEmpty() )   //TODO: move this up (3x)???
                        return 0;
                    Append<false>(         ((StringBase<TCompChar>&) *(int*) &src).Buffer(),
                                   result= ((StringBase<TCompChar>&) *(int*) &src).Length() );
                }

                // references to class types? -> by const reference
                else
                    result= T_Apply<TPlain, TChar>::Apply   ( *this, (TPlain&) *(int*) &src );
            }

            //---------- conclude ----------

            // was empty?
            if ( TCheck && result == 0 && StringBase<TChar>::IsNull() )
            {
                // special treatment if currently nothing is allocated and a blank string ("") is added:
                // we allocate, which means, we are not a nulled object anymore!
                // (...also, in this case we check the src parameter)
                SetBuffer( 15 );
            }

            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0);
            return result;
        }



        /** ****************************************************************************************
         * Wrapper method around #Apply that returns \c *this to allow concatenated calls.
         *
         * With template parameter TCheck equal to \c true, this method is equivalent to
         * invoking #operator<<(const T&).
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c \<false\> is added to the method name, checks are omitted.
         * @tparam T       The type of parameter \p{source}.
         * @param  src     The source of type \p{T} to append.
         *
         * @return   \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true, class T >
        inline
        AStringBase& _(const  T& src )
        {
            Apply<TCheck>( src );
            return *this;
        }

        /** ****************************************************************************************
         * Invokes #Append on a region of a reference of \alib{strings,StringBase,String}.
         * Accepts various types for \p{src}, due to the flexibility of class \b %String to implicitly
         * construct.
         *
         * \note When using the non-checking version, parameter \p{regionLength} must be set
         *       explicitly to the correct value (instead of using the default value).
         *
         * @tparam TCheck        Chooses checking or non-checking implementation. Defaults to true.
         * @param  src           The \b %String to append.
         * @param  regionStart   The start of the region in src to append.
         * @param  regionLength  The maximum length of the region in src to append.
         *                       Defaults to \b %MaxLen
         *
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        AStringBase& _( const StringBase<TChar>& src, integer regionStart, integer regionLength =MaxLen )
        {
            if (TCheck)
            {
                if ( src.IsNull() )
                    return *this;
                if ( src.StringBase<TChar>::AdjustRegion( regionStart, regionLength ) )
                {
                    // special treatment if currently nothing is allocated and a blank string ("") is added:
                    // we allocate, which means, we are not a nulled object anymore!
                    // (...also, in this case we check the src parameter)
                    if ( StringBase<TChar>::IsNull() )
                        SetBuffer( 15 );
                    return *this;
                }
            }
            else
            {
                //---- non-checking version ----
                ALIB_ASSERT_ERROR(    regionStart >= 0 && regionLength >= 0
                                   && regionLength != MaxLen // has to be separately checked!
                                   && regionStart +  regionLength  <= src.Length(),
                                   ASTR("NC: Invalid region given") )
            }

            // both versions
            return Append<false>( src.Buffer() + regionStart, regionLength );
        }

        /** ****************************************************************************************
         * Invokes #Apply to append string-type objects and to generically 'apply' objects of
         * arbitrary type.
         * This template method wraps an invocation of #Apply, ignoring its return value
         * and instead returns \c *this to allow concatenated calls.
         * This operator is equivalent to invoking #_<true>(const T&).
         * \see Method \ref Apply for more information.
         *
         * @tparam T   The type of parameter \p{source}.
         * @param  op  The object of type T to append.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <class T>
        inline
        AStringBase& operator<< (const  T& op )
        {
            Apply<true>(op);
            return *this;
        }

        /** ****************************************************************************************
         * Appends platform specific new line character(s) by appending literal string
         * \ref aworx::NewLine "NewLine".
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        AStringBase&          NewLine()
        {
            return _<false>( aworx::NewLine );
        }

    /** ############################################################################################
     * @name Replace
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Replaces one or more occurrences of one character by another character.
         *
         * @param needle           The terminatable string to be replaced.
         * @param replacement      The replacement string (does not need to be zero terminatable).
         * @param startIdx         The index where the search starts. Optional and defaults 0.
         *
         * @return The number of replacements that where performed.
         ******************************************************************************************/
        ALIB_API
        integer SearchAndReplace( TChar           needle,
                                  TChar           replacement,
                                  integer        startIdx            = 0  );

        /** ****************************************************************************************
         * Replaces one or more occurrences of a terminatable string (
         * \alib{strings,TStringBase,TString}) by a replacement string.
         *
         * @param needle           The terminatable string to be replaced.
         * @param replacement      The replacement string (does not need to be zero terminatable).
         * @param startIdx         The index where the search starts. Optional and defaults 0.
         * @param maxReplacements  The maximum number of replacements to perform.
         *                         Optional and defaults to \b %MaxLen.
         * @param sensitivity      Case sensitivity of the comparison.
         *                         Optional and defaults to Case::Sensitive.
         *
         * @return The number of replacements that where performed.
         ******************************************************************************************/
        ALIB_API
        integer SearchAndReplace( const TStringBase<TChar>& needle,
                                  const StringBase<TChar>&  replacement,
                                  integer        startIdx            = 0,
                                  integer        maxReplacements     = MaxLen,
                                  lang::Case     sensitivity         = lang::Case::Sensitive );

        /** ****************************************************************************************
         * Converts all or a region of characters in the Buffer to upper case.
         *
         * @param regionStart     Start of the region to be converted. Defaults to 0
         * @param regionLength    Length of the region to be converted. Defaults to \b %MaxLen.
         *
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        AStringBase& ToUpper( integer regionStart= 0, integer regionLength= MaxLen )
        {
            if( TCheck )
            {
                if ( StringBase<TChar>::AdjustRegion( regionStart, regionLength ) )
                    return *this;
            }
            else
            {
                #if ALIB_DEBUG
                    integer rs=  regionStart;
                    integer rl=  regionLength;
                    StringBase<TChar>::AdjustRegion( rs, rl );
                    ALIB_ASSERT_ERROR( rs == regionStart && rl == regionLength,
                                       ASTR("NC: Invalid region given") )
                #endif
            }

            // convert
            TChar* buf= StringBase<TChar>::vbuffer + regionStart;
            TChar* end= buf     + regionLength;
            while( buf < end )
            {
                *buf=  static_cast<TChar>(toupper( *buf ));
                buf++;
            }
            return *this;
        }


        /** ****************************************************************************************
         * Converts all or a region of characters in the Buffer to lower case.
         *
         * @param regionStart     Start of the region to be converted. Defaults to 0
         * @param regionLength    Length of the region to be converted. Defaults to \b %MaxLen.
         *
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        AStringBase& ToLower( integer regionStart= 0, integer regionLength= MaxLen )
        {
            if( TCheck )
            {
                if ( StringBase<TChar>::AdjustRegion( regionStart, regionLength ) )
                    return *this;
            }
            else
            {
                #if ALIB_DEBUG
                    integer rs=  regionStart;
                    integer rl=  regionLength;
                    StringBase<TChar>::AdjustRegion( rs, rl );
                    ALIB_ASSERT_ERROR(  rs == regionStart && rl == regionLength, ASTR("NC: Invalid region given") )
                #endif
            }

            // convert
            TChar* buf= StringBase<TChar>::vbuffer + regionStart;
            TChar* end= buf     + regionLength;
            while( buf < end )
            {
                *buf=  static_cast<TChar>(tolower( *buf ));
                buf++;
            }
            return *this;
        }

    /** ############################################################################################
     * @name std::iterator
     ##@{ ########################################################################################*/
    public:
        /**
         * While parent class \b %String provides a constant iterator, we are exposing the same
         * thing in a mutable fashion.
         */
        using Iterator= typename StringBase<TChar>:: template RandomAccessIteratorBase<TChar*, TChar&>;

        /**
         * Returns an iterator pointing to the start of this string.
         * @return The start of this string.
         */
        Iterator begin()
        {
            return Iterator( VBuffer() );
        }

        /**
         * Returns an iterator pointing to the first character behind this string.
         * @return The end of this string.
         */
        Iterator end()
        {
            return Iterator( VBuffer() + StringBase<TChar>::length );
        }

}; // class AStringBase

//! @cond NO_DOX

// #################################################################################################
// NAString implementations
// #################################################################################################
#if ALIB_DEBUG_STRINGS
extern template ALIB_API void    AStringBase<nchar>::_dbgCheck() const;
#endif

// instantiations
extern  template ALIB_API void                AStringBase<nchar>::SetBuffer       ( integer);
extern  template ALIB_API void                AStringBase<nchar>::SetBuffer       ( nchar*,integer,integer,lang::Responsibility);
extern  template ALIB_API integer             AStringBase<nchar>::SearchAndReplace( nchar,nchar,integer );
extern  template ALIB_API integer             AStringBase<nchar>::SearchAndReplace( const TStringBase<nchar>&,const StringBase<nchar>&,integer,integer,lang::Case );
extern  template ALIB_API AStringBase<nchar>& AStringBase<nchar>::Trim            ( const TStringBase<nchar>& );
extern  template ALIB_API integer             AStringBase<nchar>::TrimAt          ( integer,const TStringBase<nchar>& );

// specializations
template<> template<>  ALIB_API AStringBase<nchar>& AStringBase<nchar>::Append<true >( const wchar*      , integer);
template<> template<>  ALIB_API AStringBase<nchar>& AStringBase<nchar>::Append<false>( const wchar*      , integer);
template<> template<>  ALIB_API AStringBase<nchar>& AStringBase<nchar>::Append<true >( const strangeChar*, integer);
template<> template<>  ALIB_API AStringBase<nchar>& AStringBase<nchar>::Append<false>( const strangeChar*, integer);

// inline specializations
template <>
template <bool TCheck> inline
AStringBase<nchar>& AStringBase<nchar>::Append( const nchar* src, integer srcLength )
{
    ALIB_STRING_DBG_CHK(this)

    if( TCheck )
    {
        if (!src)
            return *this;

        // check empty
        if ( srcLength <= 0 )
        {
            // set "un-nulled"
            if ( StringBase<nchar>::IsNull() )
                SetBuffer( 15 );

            return *this;
        }
    }
    else
    {
        ALIB_STRING_DBG_CHK(this)
        ALIB_ASSERT_ERROR( src || srcLength == 0,  ASTR("NC: nullptr passed") )
    }

    EnsureRemainingCapacity( srcLength );
    CString<nchar>::Copy( src, srcLength, StringBase<nchar>::vbuffer + StringBase<nchar>::length );
    StringBase<nchar>::length+= srcLength;
    ALIB_STRING_DBG_UNTERMINATE(nchar,*this, 0)

    return *this;
}


// #################################################################################################
// WAString implementations
// #################################################################################################
#if ALIB_DEBUG_STRINGS
extern template ALIB_API void    AStringBase<wchar>::_dbgCheck() const;
#endif

// instantiations
extern  template ALIB_API void                  AStringBase<wchar>::SetBuffer       ( integer);
extern  template ALIB_API void                  AStringBase<wchar>::SetBuffer       ( wchar*,integer,integer,lang::Responsibility);
extern  template ALIB_API integer               AStringBase<wchar>::SearchAndReplace( wchar,wchar,integer );
extern  template ALIB_API integer               AStringBase<wchar>::SearchAndReplace( const TStringBase<wchar>&,const StringBase<wchar>&,integer,integer,lang::Case );
extern  template ALIB_API AStringBase<wchar>&   AStringBase<wchar>::Trim            ( const TStringBase<wchar>& );
extern  template ALIB_API integer               AStringBase<wchar>::TrimAt          ( integer,const TStringBase<wchar>& );

// specializations
template<>  template<>  ALIB_API AStringBase<wchar>& AStringBase<wchar>::Append<true >( const nchar*      , integer );
template<>  template<>  ALIB_API AStringBase<wchar>& AStringBase<wchar>::Append<false>( const nchar*      , integer );
template<>  template<>  ALIB_API AStringBase<wchar>& AStringBase<wchar>::Append<true >( const strangeChar*, integer );
template<>  template<>  ALIB_API AStringBase<wchar>& AStringBase<wchar>::Append<false>( const strangeChar*, integer );

// inline specializations
template <>
template <bool TCheck> inline
AStringBase<wchar>& AStringBase<wchar>::Append( const wchar* src, integer srcLength )
{
    ALIB_STRING_DBG_CHK(this)

    if( TCheck )
    {
        if (!src)
            return *this;

        // check empty
        if ( srcLength <= 0 )
        {
            // set "un-nulled"
            if ( StringBase<wchar>::IsNull() )
                SetBuffer( 15 );

            return *this;
        }
    }
    else
    {
        ALIB_STRING_DBG_CHK(this)
        ALIB_ASSERT_ERROR( src || srcLength == 0,  ASTR("NC: nullptr passed") )
    }

    EnsureRemainingCapacity( srcLength );
    CString<wchar>::Copy( src, srcLength, StringBase<wchar>::vbuffer + StringBase<wchar>::length );
    StringBase<wchar>::length+= srcLength;
    ALIB_STRING_DBG_UNTERMINATE(wchar,*this, 0)

    return *this;
}

//! @endcond

ALIB_WARNINGS_RESTORE


// #################################################################################################
// Namespace types and functions
// #################################################################################################
    /** ********************************************************************************************
     * Partial specialization of struct <em>T_IsTerminatable</em> which denotes allowance
     * to write a termination character '\0' into the buffer at position \b %length.
     * (Specialization inherits from \c std::true_type.).
     **********************************************************************************************/
    template<typename TChar>
    struct  T_IsTerminatable<AStringBase<TChar   >>   : public std::true_type { };

// #################################################################################################
// Debug methods
// #################################################################################################
    #if ALIB_DEBUG
        } // aworx::lib[::strings]
        namespace debug {

        /** ****************************************************************************************
         * Simple method that removes known namespace of %ALib from a given \b %AString.
         * If parameter \p{remove} is false, nothing is done.
         *
         * \note This method is available only in debug compilations of %ALib.
         *
         * @param target   The string to process.
         * @param remove   If \c false, nothing is done, if \c true method does what it says.
         * @return The given string to allow concatenated calls.
         ******************************************************************************************/
        strings::AStringBase<character>&  RemoveALibNamespaces ( strings::AStringBase<character>& target, bool remove );

        #if ALIB_FEAT_SINGLETON_MAPPED

             /** ***********************************************************************************
             * This debug function writes all type names and addresses of each currently defined
             * instance of class
             * \ref aworx::lib::lang::Singleton "Singleton"
             * into the given AString.<br>
             *
             * \note
             *  This method is available only
             *  - with \ref ALIB_DEBUG "debug compilations" of the software,
             *  - if module \ref aworx::lib::strings "ALib Strings" is included in the \alib
             *    \ref aworx::lib "distribution module" and
             *  - if code selction symbol \ref ALIB_FEAT_SINGLETON_MAPPED is \c true.
             *
             *  In case <b>%ALib Strings</b> are not included in the distribution module, then
             *  alternative method
             *  \ref aworx::lib::debug::GetSingletons "GetSingletons" can be used which
             *  returns a list of \c std::type_info structs together with (void) pointers to the
             *  singletons.
             *
             * @param target The target string to write the list of singletons to.
             *
             * @return The number of singletons written.
             **************************************************************************************/
            ALIB_API
            int GetSingletons( NAString& target );
        #endif

        } // aworx::lib[::debug]
        namespace strings {
    #endif


}}} // namespace [aworx::lib::strings]


#if defined(_MSC_VER)
    #pragma warning( pop )
#endif
#endif // HPP_ALIB_STRINGS_ASTRING
