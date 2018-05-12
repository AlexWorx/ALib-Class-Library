// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// Include guard
#ifndef HPP_ALIB_STRINGS_STRING_BASE
#define HPP_ALIB_STRINGS_STRING_BASE 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION)
    #error "include 'alib/alib.hpp' instead of this header"
#endif


// conditional expression is constant for using our constant template parameters to select
// checking vs. non-checking method versions
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif

// #################################################################################################
// Macros
// #################################################################################################


/**
 * @addtogroup GrpALibCompilerSymbols
 * @{ \def  ALIB_DEBUG_STRINGS_ON
 *  This compiler symbol enables additional debug code within class AString. When provided,
 *  it defines \ref ALIB_DEBUG_STRINGS to \c true. This is useful when extending or specifically
 *  debugging class AString.
 * @}
 *
 * @addtogroup GrpALibCompilerSymbols
 * @{ \def  ALIB_DEBUG_STRINGS_OFF
 *  Disables certain debug code within class AString. See \ref ALIB_DEBUG_STRINGS_ON.
 *  This symbol represents the default behavior and is provided for completeness.
 * @}
 *
 * @addtogroup GrpALibCodeSelectorSymbols
 * @{ \def  ALIB_DEBUG_STRINGS
 *  Selects extra debug code within class AString. Gets defined by compiler symbol
 *  \ref ALIB_DEBUG_STRINGS_ON.
 * @}
 */

#if defined(DOX_PARSER)
    #define     ALIB_DEBUG_STRINGS
    #define     ALIB_DEBUG_STRINGS_ON
    #define     ALIB_DEBUG_STRINGS_OFF
#else
    #if defined(ALIB_DEBUG_STRINGS_OFF) && defined(ALIB_DEBUG_STRINGS_ON)
        #error "ALIB_DEBUG_STRINGS_OFF / ALIB_DEBUG_STRINGS_ON are both set"
    #endif

    #if defined( ALIB_DEBUG_STRINGS_ON )
        #define    ALIB_DEBUG_STRINGS 1
    #else
        #define    ALIB_DEBUG_STRINGS 0
    #endif
#endif //DOX_PARSER


/**
 * @addtogroup GrpALibStringsMacros
 * @{ \def  ALIB_STRING_DBG_CHK
 * Simple macro that just invokes method _dbgCheck(), which is defined for classes
 * \alib{strings,StringBase,String},
 * \alib{strings,TStringBase,TString} and
 * \alib{strings,AStringBase,AString}.
 * It is active only when compiler symbol \ref ALIB_DEBUG_STRINGS is \c true.
 * The macro is placed in almost every method.
 * @}
 */

// do not indent this, for the sake of doxygen formatting
#if ALIB_DEBUG_STRINGS
#define ALIB_STRING_DBG_CHK(instance)    \
{                                        \
    (instance)->_dbgCheck();             \
}
#else
#define  ALIB_STRING_DBG_CHK(instance)
#endif


/**
 * @addtogroup GrpALibStringsMacros
 * @{ \def  ALIB_STRING_CONSTRUCTOR_FIX
 * Defines template class
 * \ref aworx::lib::strings::T_String  "T_String" for the given type. This may be used to suppress
 * "false errors" on compilation for certain types. For example (at the time of writing this), the
 * use of \c std::tuple with elements of type \b %String might lead to errors in certain complex
 * usage scenarios.<br>
 * The macro defines methods \c %Buffer and \c %Length to return \c nullptr and \c 0. In effect
 * the methods are never called. The same as the specialization of the struct itself, the
 * methods need to exist only to avoid the failure of a static assertion, as evaluated by some
 * compilers in the constructor of class \alib{strings,StringBase,String}.
 *
 * The macro must be placed outside of any namespace.<br>
 * When passing template types with comma separated type lists, the commas must be given using
 * macro \ref ALIB_COMMA.
 * @}
 */
#define ALIB_STRING_CONSTRUCTOR_FIX(TYPE,TCHAR)                                                    \
namespace aworx { namespace lib { namespace strings {                                              \
    template<>   struct      T_String<TYPE,TCHAR>   : public std::true_type                        \
    {                                                                                              \
        static inline const char* Buffer( const TYPE&  )                                           \
        {                                                                                          \
            ALIB_ERROR( "Macro ALIB_STRING_CONSTRUCTOR_FIX set for type \""                        \
                        ALIB_STRINGIFY((TYPE))                                                     \
                        "\", which is used for string construction" );                             \
            return nullptr;                                                                        \
        }                                                                                          \
                                                                                                   \
        static inline integer     Length( const TYPE&  )  { return 0; }                            \
    };                                                                                             \
}}}



namespace aworx { namespace lib { namespace strings {

/// The maximum length of an \alib string.
constexpr static integer         MaxLen=            (std::numeric_limits<integer>::max)();



#if !defined(DOX_PARSER)
    ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING;
#endif

/** ********************************************************************************************
 * This is a specializable struct which defaults the question whether a given type
 * can be used to construct an \alib{strings,StringBase,String} to \c false.
 *
 * Specializations of this struct inherit from \c std::true_type, to define a type to be a usable
 * constructor parameter type of \b %String.
 *
 * Hence, this template struct supports converting 'external' <em>user defined</em> to
 * \alib{strings,StringBase,String}. It has a central role for using \alib in a
 * \ref aworx::lib "non intrusive" way, by allowing to pass external string types just as
 * the are when invoking \alib functions and methods that expect parameters of type
 * <c>const %String&</c>. Due to the implicit constructor invocation of C++
 * and the template meta programming in constructor of class \b %String, custom types
 * (that specialize this struct) may be passed "as is" to such methods.
 *
 * Methods #Buffer and #Length of partially specialized versions of this struct are invoked
 * by constructor \alib{strings::StringBase,StringBase(const TStringLike&)}
 * to receive a pointer to the character buffer and the length of the string.
 *
 * The following sample demonstrates the use of this technique:
 *
 * \snippet "DOX_ALIB_TOAS.cpp"     DOX_ALIB_TOAS
 *  The output will be:
 * \snippet "DOX_ALIB_TOAS.txt"     OUTPUT
 *
 * \note
 *   As shown in the sample, if a specialized version of this struct for a custom type exists, such
 *   type becomes automatically also an "applicable" type to class #AString, because method
 *   \b %AString::Apply checks for the existence of a specialization of this struct likewise
 *   the constructor of class \b %String does.
 *
 * A compile-time assertion will be raised when a String is constructed with a type that has
 * no specialization of this struct.
 *
 * @tparam TStringLike The type for which string construction compatibility is declared.
 * @tparam TChar       The character type of the strings that can be constructed with
 *                     \p{TStringLike}.
 * **********************************************************************************************/
template<typename TStringLike, typename TChar>
struct T_String     : public std::false_type
{
    /** ********************************************************************************************
     * In specialized versions, this method has to return a pointer to the buffer of the given
     * object of type \p{TStringLike}.
     *
     * This default implementation makes some static assertions to detect usage of an unsupported
     * type (a type that is not allowed to construct class \b %String with).
     *
     * @returns Has to return a pointer to the buffer of object of type T, respectively its length.
     **********************************************************************************************/
    static inline constexpr
    const TChar* Buffer( const TStringLike& )
    {
        // should never happen, because of static assert in string constructor
        return nullptr;
    }

    /** ********************************************************************************************
     * In specialized versions, this method has to return the length of the buffer of the given
     * object of type \p{TStringLike}.
     *
     * @returns Has to return the length of the string.
     **********************************************************************************************/
    static inline
    integer Length( const TStringLike& )
    {
        return (integer) 0;
    }
};



/** ************************************************************************************************
 * This TMP struct is similar to \ref aworx::lib::strings::T_String.
 * It is to be specialized for custom string types that allow to infer the string length already
 * from the \p{TLiteral} itself.
 *
 * The most obvious type are C++ string literals, hence character arrays \c char[TCapacity] of a
 * given length.
 *
 * See also class \alib{strings,StringLiteralBase,StringLiteral} which comes with a
 * (templated) specialization of this struct and can be used to declare C++ strings of fixed size
 * with variable contents.
 *
 * The benefit of specializing this struct when possible (in comparison to specializing
 * \ref aworx::lib::strings::T_String) is that method #Length is a \c constexpr with this struct.
 * This allows shorter template code and a small performance gain with some methods.
 *
 * @tparam TLiteral The literal type.
 * @tparam TChar    The character type of the string
 **********************************************************************************************/
template<typename TLiteral,typename TChar>
struct T_StringLiteral  : public std::false_type
{
    /**
     * Returns the buffer of the literal.
     * @return The buffer of the literal.
     */
    static inline constexpr TChar* Buffer(const TLiteral& )  { return nullptr; }

    /**
     * Returns the constant length (length is depending only on type \p{TLiteral})
     * @return The length.
     */
    static inline constexpr integer Length()      { return  1;      };

};

#if !defined(DOX_PARSER)
    ALIB_WARNINGS_RESTORE;
#endif

// #################################################################################################
// Forwards of detail functions
// #################################################################################################

/** ************************************************************************************************
 * This class is the base class of all \alib string classes. It represents a character string
 * whose data is allocated outside the scope of objects of this class.
 *
 * Once constructed, objects
 * of this class are immutable. This means, there is no interface to change their buffer or length
 * during their lifetime, which normally is rather volatile.
 *
 * The buffer represented might be zero-terminated or not. This is dependent on the data it was
 * constructed from. The buffer must not be changed and might even reside in read-only memory.
 *
 * <b>Templated Construction</b><br>
 * What makes this class very flexible, is the constructor variant
 * \alib{strings,StringBase::StringBase(const TStringLike&)}.
 * This template method internally uses <em>template meta programming</em> to detect known types
 * and, and to convert them to constant references of those. They are then passed to template
 * methods \alib{strings,T_String} that simply return a pointer to the external types' buffer
 * and length. This way, objects of this class can be implicitly constructed from just anything that
 * 'smells' like a string.<br>
 * For more information on how to make %String support to implicitly construct from user defined types,
 * see namespace template function
 * \ref aworx::lib::strings::T_String "T_String".
 *
 * This class provides compiler defined copy and move constructors and assignment operators.
 * Once and object is constructed, methods #Buffer and #Length allow read access to the contents.
 *
 * <b>Null-State</b><br> \anchor CPP_STRINGS_AS_NULLSTATE
 * Objects of this class can be \e nulled which means that it is a difference whether they are
 * representing an empty string or a \c nullptr. As objects are immutable, this is decided
 * on construction: once a %String is nulled, it will remain nulled.
 * In other words, it makes a difference if an %String is constructed using
 * - <em>%String()</em>, which creates a \e nulled object, method #IsNull will give \c true, or
 * - <em>%String("")</em>, which creates an empty object. Method #IsNull will give \c false while
 *   method #IsEmpty will give \c true
 *
 * <b>Non-checking Method Variants</b><br> \anchor CPP_STRINGS_AS_NC
 * Some of the provided methods are templated with boolean parameter <em>TCheck</em> which allow
 * to invoke a faster and shorter version of the same method. For more information see
 * \ref alib_strings_details_nonchecking in the namespace documentation.
 *
 * \note Almost all methods of this class are declared <em>inline</em>.
 *
 * <p>
 * \note The immutable nature of %String is lifted by derived types. While class
 *       \alib{strings,SubstringBase,Substring} allows to change the start and
 *       length of the string represented, class
 *       \alib{strings,AStringBase,AString} holds a copy of the data and allows to
 *       modify the contents. Field #buffer of this class is declared
 *       as an anonymous union of a <em>const char*</em> and a <em>char*</em> but the latter is
 *       not exposed. It might be exposed by derived classes (and is by class \p{%AString}).
 *
 * <p>
 * \note For an introduction into the \alib string classes see
 *       \ref aworx::lib::strings "namespace strings"
 *
 * @tparam TChar    The character type. Implementations for \c char and \c wchar_t are provided
 *                  with type definitions \ref aworx::NString and \ref aworx::WString.
 **************************************************************************************************/
template<typename TChar>
class StringBase
{
    // #############################################################################################
    // Debug warnings
    // #############################################################################################
    public:
        //! @cond NO_DOX
            ALIB_WARN_ONCE_PER_TYPE_DECL(ALIB_API, SetLengthLonger)

            #if ALIB_DEBUG_STRINGS
                void     _dbgCheck()   const;
            #endif
        //! @endcond

    // #############################################################################################
    // Private fields
    // #############################################################################################
    protected:
    #if !defined( DOX_PARSER )

         union
         {
             const TChar*   buffer;
                   TChar*  vbuffer;
         };

    #else
            /**
             * Pointer to an array of characters that holds the string we are representing.
             * Read access to this field is granted with method #Buffer.
             */
             const TChar*   buffer;

            /**
             * Non-constant version of field #buffer. This field is not used in this class, but
             * has to be declared here, because it constitutes an anonymous union with field
             * #buffer.
             * Derived classes might use and expose this field, like e.g. class
             * \alib{strings,AStringBase,AString} does.
             */
                   TChar*  vbuffer;
    #endif

        /**
         * The length of the string represented by us.
         * Read access to this field is granted with method #Length.
         */
        integer         length;

    public:

    /** ############################################################################################
     * @name Constructors
     ##@{ ########################################################################################*/
     public:

        /** ****************************************************************************************
         * Constructs a \e nulled %StringBase
         ******************************************************************************************/
        inline
        constexpr StringBase()
        : buffer(nullptr)
        , length(0)
        {}

        /** ****************************************************************************************
         * Constructs this object using the given external buffer and length of content.
         *
         *  \note The provided buffer does not need to be zero-terminated. However, there must not
         *        be any '\0' characters within the start and the given \p{contentLength}.
         *
         * @param buffer          The buffer to use.
         * @param contentLength   The length of the content in the given buffer.
         ******************************************************************************************/
        inline
        constexpr
        StringBase( const TChar* buffer, integer contentLength )
        : buffer(buffer)
        , length(contentLength )
        {}


        #if !defined(DOX_PARSER)
            ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING;
        #endif
        /** ****************************************************************************************
         * Templated constructor for different types. This constructor uses some template meta
         * programming to provide maximum flexibility to implicitly embed the data of any string
         * type in an object of type %String.
         *
         * This constructor accepts the following types:
         * - <em>nullptr</em> (creates a \e nulled %String).
         * - <em>[const] TChar*</em>
         * - Classes derived from %String.
         * - User defined (external) types. See documentation of
         *   \ref aworx::lib::strings::T_String "T_String" on how to add support for implicit
         *   construction of  \alib strings from custom string types.
         * - User defined literal types. See documentation of
         *   \ref aworx::lib::strings::T_StringLiteral "T_StringLiteral".
         *
         * When a non-supported type is passed, a compile-time error (\c static_assert) is raised.
         * In some rare cases, dependent on the compiler and platform, some false alarms may be
         * given for types that are not even passed to the constructor, but the compiler wrongly
         * "thinks" that such type is passed. For example, with advanced use of \c std::tuple template
         * class, such errors might occur. To suppress a "false" error, use macro
         * \ref ALIB_STRING_CONSTRUCTOR_FIX for the type in question.
         *
         * \note
         *   Other than the type of parameter \p{src} (<em>const TStringLike&</em>) may indicate,
         *   objects of the class types named above may be provided as pointer or reference.
         *   The TMP will detect \c nullptr and otherwise convert pointers to references.
         *
         * \see For more information, see
         * \ref aworx::lib::strings "namespace documentation" and template
         * function \ref aworx::lib::strings::T_String "T_String".
         *
         * @tparam TStringLike  Type that allows to construct Strings.
         * @param src           The source of template type T to take the buffer and length from.
         ******************************************************************************************/
        template <typename TStringLike>
        inline
        constexpr
        StringBase(const  TStringLike& src )
        : buffer(
            // nullptr ?
            std::is_same<decltype(nullptr), TStringLike>::value
              ? nullptr
            : std::is_pointer<TStringLike>::value && (*(TChar**)&src) == nullptr
              ? nullptr

            // String literal?
            :   T_StringLiteral<typename std::remove_cv<TStringLike                                    >::type,TChar>::value
              ? T_StringLiteral<typename std::remove_cv<TStringLike                                    >::type,TChar>::Buffer( (typename std::add_const<TStringLike>::type &) src )

            // String literal pointer?
            :   T_StringLiteral<typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::value
              ? T_StringLiteral<typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::Buffer( *(typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type *&)  src )

            // Custom string?
            :   T_String       <typename std::remove_cv<TStringLike                                    >::type,TChar>::value
              ? T_String       <typename std::remove_cv<TStringLike                                    >::type,TChar>::Buffer( (typename std::add_const<TStringLike>::type &) src )

            // Custom string pointer?
            :   T_String       <typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::value
              ? T_String       <typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::Buffer( *(typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type *&)  src )

            // Derived string type?
            :  std::is_base_of<StringBase<TChar>, TStringLike  >::value
              ?  ((StringBase<TChar>*) &src)->buffer

            // Pointer to derived string type?
            :  std::is_base_of<StringBase<TChar>, typename std::remove_pointer<TStringLike>::type  >::value
              ?  (*(StringBase<TChar>**) &src)->buffer

            : (const TChar*) -1   // this should never happen due to the static assert below
        )

        , length(
            // nullptr ?
            std::is_same<decltype(nullptr), TStringLike>::value
              ? 0
            : std::is_pointer<TStringLike>::value && (*(TChar**)&src) == nullptr
              ? 0

            // String literal?
            :   T_StringLiteral<typename std::remove_cv<TStringLike                                    >::type,TChar>::value
              ? T_StringLiteral<typename std::remove_cv<TStringLike                                    >::type,TChar>::Length()

            // String literal pointer?
            :   T_StringLiteral<typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::value
              ? T_StringLiteral<typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::Length()

            // Custom string?
            :   T_String       <typename std::remove_cv<TStringLike                                    >::type,TChar>::value
              ? T_String       <typename std::remove_cv<TStringLike                                    >::type,TChar>::Length( (typename std::add_const<TStringLike>::type &) src )

            // Custom string pointer?
            :   T_String       <typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::value
              ? T_String       <typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::Length( *(typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type *&)  src )

            // Derived string type?
            :  std::is_base_of<StringBase<TChar>, TStringLike  >::value
              ?  ((StringBase<TChar>*) &src)->length

            // Pointer to derived string type?
            :  std::is_base_of<StringBase<TChar>, typename std::remove_pointer<TStringLike>::type  >::value
              ?  (*(StringBase<TChar>**) &src)->length

            : -1  // this should never happen due to the static assert below
        )
        {
            static_assert
            (
                std::is_same   <decltype(nullptr),                                   TStringLike                    >::value
             || T_StringLiteral<typename std::remove_cv<                             TStringLike       >::type,TChar>::value
             || T_StringLiteral<typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::value
             || T_String       <typename std::remove_cv<                             TStringLike       >::type,TChar>::value
             || T_String       <typename std::remove_cv<typename std::remove_pointer<TStringLike>::type>::type,TChar>::value
             || std::is_base_of<StringBase<TChar>,                                TStringLike              >::value
             || std::is_base_of<StringBase<TChar>,   typename std::remove_pointer<TStringLike>::type       >::value

               , "aworx::StringBase<TChar> can not be constructed from type TStringLike."
            );
        }

        #if !defined(DOX_PARSER)
            ALIB_WARNINGS_RESTORE
        #endif

        /** ****************************************************************************************
         * Returns a sub-string of this object.
         *
         * @param  regionStart  The start of the region within this string.
         * @param  regionLength The length of the region to return.
         *                      Defaults to \b %MaxLen
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c false is given, no range check is performed.
         * @return A string representing a region of this string.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        StringBase<TChar>   Substring(integer regionStart, integer regionLength=  MaxLen )     const
        {
            ALIB_STRING_DBG_CHK(this)

            if ( TCheck )
            {
                AdjustRegion( regionStart, regionLength );
            }
            else
            {
                #if ALIB_DEBUG
                    integer rs= regionStart;
                    integer rl= regionLength;
                    AdjustRegion( rs, rl );
                    ALIB_ASSERT_ERROR( rs == regionStart && rl == regionLength,
                                       ASTR("Non checking and region out of range or empty") );
                #endif
            }

            return StringBase<TChar>( buffer + regionStart, regionLength );
        }


    /** ############################################################################################
     * @name Buffer Access, Length and State
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         *  Returns a pointer to the first character of the string we are representing.
         *  \note The string is not guaranteed to be zero terminated.
         *
         * @return The internal buffer array.
         ******************************************************************************************/
        inline
        const TChar*  Buffer()          const       { return buffer;    }

        /** ****************************************************************************************
         *  Returns the length of the string we are representing.
         *
         * @return The length of the string represented by this.
         ******************************************************************************************/
        inline
        integer       Length()          const       { return length;    }

        /** ****************************************************************************************
         * Returns the length of the string if represented as a wide character string.
         * If template parameter \p{TChar} equals \c wchar_t, then this is identical with #Length.
         *
         * Otherwise the calculation is done using
         * - <em>mbsnrtowcs()</em>
         *   (without providing a conversion buffer) on glibc platforms (e.g. Linux)
         * - <em>MultiByteToWideChar()</em>
         *   (without providing a conversion buffer) on the Windows platform.
         *
         * If the conversion fails, \c -1 is returned.
         *
         * \note
         *   On GNU/Linux and Mac OS, it might be necessary to invoke std c method
         *   <em>setlocale()</em> once, prior to using this method, to successfully calculate
         *   the length.
         *   This, by default, is done in \ref aworx::lib::ALib::init "ALib::init".
         *
         * @return  The length of string when it was converted to wide characters.
         *          If counting failed (what means that the conversion will fail) \c -1 is returned.
         ******************************************************************************************/
        integer       WStringLength()   const;


        /** ****************************************************************************************
         * Returns \c true if field #buffer equals nullptr, \c false otherwise.
         *
         * @return \c true if no buffer is allocated.
         ******************************************************************************************/
        inline
        bool         IsNull()           const       { return buffer == nullptr; }

        /** ****************************************************************************************
         * Returns \c true if field #buffer does not equal nullptr, \c false otherwise.
         *
         * @return \c true if no buffer is allocated.
         ******************************************************************************************/
        inline
        bool         IsNotNull()        const       { return buffer != nullptr; }

        /** ****************************************************************************************
         *  Returns \c true if this string is of zero length.
         * @return \c true if the actual length equals zero.
         ******************************************************************************************/
        inline
        bool         IsEmpty()          const       { return length == 0; }

        /** ****************************************************************************************
         *  Returns \c true if this string has a length of 1 or more.
         * @return \c true if the actual length does not equal zero.
         ******************************************************************************************/
        inline
        bool         IsNotEmpty()       const       { return length != 0; }


    /** ############################################################################################
     * @name Character access
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Retrieves the character at the given index. A range check is performed. If this fails,
         * '\0' is returned.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c false is given, no range check is performed.
         * @param idx      The index of the character to read.
         * @return The character at the given index, or '\0' if index out of range.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        TChar        CharAt( integer idx )          const
        {
            if (TCheck)
                return  ( idx >= 0 && idx < length ) ? *(buffer + idx )
                                                     : '\0' ;
            ALIB_ASSERT_ERROR( idx >= 0 && idx < length, ASTR("Non checking version: Index out of range") );

            return  *(buffer + idx );
        }

        /** ****************************************************************************************
         * Retrieves the first character. In case of an empty or \e nulled string, '\0' is returned.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c false is given, no check for an empty or \e nulled object is
         *                 performed.
         * @return The first character in the %String.
         *         If this instance's length is zero,  '\0' is returned.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        TChar        CharAtStart()          const
        {
            if (TCheck)
                return length > 0  ?  *(buffer)
                                   :  '\0';
            ALIB_ASSERT_ERROR(  length > 0, ASTR("Non checking invocation on empty string") );

            return  *(buffer);
        }


        /** ****************************************************************************************
         * Retrieves the last character. In case of an empty or \e nulled string, '\0' is returned.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c false is given, no check for an empty or \e nulled object is
         *                 performed.
         *
         * @return The last character in the %String.
         *         If this instance's length is zero,  '\0' is returned.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        TChar        CharAtEnd()          const
        {
            if (TCheck)
                return length > 0   ?  *(buffer + length - 1)
                                    : '\0';

            ALIB_ASSERT_ERROR( length > 0, ASTR("Non checking invocation on empty string") );

            return  *(buffer + length - 1);
        }

        /** ****************************************************************************************
         * Reads a character at a given index.
         *
         * \attention
         *   Unlike other operator methods in the family of of \alib string classes, which are
         *   performing parameter checks (in this case a range check), this operator does
         *   <em>not</em> do a check!<br>
         *   The rationale is that in derived class %AString, which overrides this operator
         *   returning, a reference to the character to provide write access, no reference
         *   to a character can be given if the index is out of range. This way, this method is
         *   equivalent to method #CharAt<\c false>.<br>
         *   For safe access to characters in the buffer use #CharAt (with template parameter
         *   \p{TCheck} being \c true).
         *
         * @param   idx    The index of the character within this objects' buffer.
         * @returns If the character contained at index \p{op}.
         ******************************************************************************************/
         inline
         TChar    operator[] (integer idx) const
         {
            ALIB_ASSERT_ERROR( idx >= 0  && idx < length, ASTR("Index out of bounds") );
            return buffer[idx];
         }

    /** ############################################################################################
     * @name Comparison
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Compares this with given %String.
         * \c true is returned if both are \e nulled or empty.
         * If only one is \e nulled, \c false is returned.
         *
         * @param needle        An %String that is compared to this.
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         *
         * @return    \c true, if contents of this and the given %String are equal.
         ******************************************************************************************/
        template<lang::Case TSensitivity =lang::Case::Sensitive>
        inline
        bool Equals( const StringBase& needle )                                               const
        {
            ALIB_STRING_DBG_CHK(this)
            if (    ( IsNull() !=  needle.IsNull() )
                    || ( length   !=  needle.length   )    )
                return false;

            if ( length == 0 )
                return true;

            return TSensitivity == lang::Case::Sensitive
                   ? CString<TChar>::Equal            ( buffer, needle.buffer, length )
                   : CString<TChar>::CompareIgnoreCase( buffer, needle.buffer, length ) == 0 ;
        }


        /** ****************************************************************************************
         * Returns \c true, if the given %String is found at the given position.
         *
         *  \note The following rules apply
         *        - If \p{pos} is out of range or \p{needle} is \e nulled, \c false is returned.
         *          (This check only done if \p{TCheck} equals \c true.)
         *        - Otherwise, if the length of \p{needle} is 0, \c true is returned.
         *
         * @param needle        The string to compare with. If is \e nulled or empty, \c true is
         *                      returned.
         * @param pos           The position to search for needle.
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c <false\> is given, no check on parameter
         *                      \p{pos} is performed and \p{needle} must not be \e nulled.
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         * @return \c true if \p{needle} is found at the given position. False otherwise. *
         ******************************************************************************************/
        template< bool        TCheck       = true,
                  lang::Case  TSensitivity = lang::Case::Sensitive >
        inline
        bool ContainsAt( const StringBase& needle, integer pos )                               const
        {
            integer needleLength= needle.length;
            ALIB_STRING_DBG_CHK(this)
            if ( TCheck )
            {
                if ( pos < 0 || pos + needleLength > length || needle.IsNull () )
                    return false;
                if ( needleLength == 0 )
                    return true;
            }
            else
            {
                ALIB_ASSERT_ERROR( pos >= 0 && pos + needleLength <= length && !needle.IsNull(),
                                   ASTR("Non checking and index out of range") );
                ALIB_ASSERT_ERROR( needleLength != 0,
                                   ASTR("Non checking and emtpy compare string") );
            }

            return TSensitivity == lang::Case::Sensitive
                   ? CString<TChar>::Equal            ( buffer + pos,  needle.buffer, needleLength )
                   : CString<TChar>::CompareIgnoreCase( buffer + pos,  needle.buffer, needleLength ) == 0 ;
        }

        /** ****************************************************************************************
         * Returns \c true, if this string starts with the string found in parameter \p{needle}.
         * If \p{needle} is \e nulled or empty, \c true is returned.
         *
         * @param needle        The string to compare the start of this string with.
         *                      If \e nulled or empty, \c true is returned.
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c <false\> is given, the given needle must not be empty
         *                      and must not be longer than this string!
         *                      \p{pos} is performed and \p{needle} must not be \e nulled.
         * @return \c true if \p{needle} is found at the start of this, \c false otherwise. *
         ******************************************************************************************/
        template<bool        TCheck      = true,
                 lang::Case  TSensitivity =lang::Case::Sensitive>
        inline
        bool StartsWith( const StringBase& needle )                                            const
        {
            if ( TCheck )
            {
                if ( needle.length > length )
                    return false;
                if ( needle.length == 0 )
                    return true;
            }
            else
            {
                ALIB_ASSERT_ERROR( needle.length <= length,
                                   ASTR("Non checking and needle longer than this string.") );
                ALIB_ASSERT_ERROR( needle.length != 0,
                                   ASTR("Non checking and emtpy needle given.") );
            }

            return TSensitivity == lang::Case::Sensitive
                   ? CString<TChar>::Equal            ( buffer,  needle.buffer, needle.length )
                   : CString<TChar>::CompareIgnoreCase( buffer,  needle.buffer, needle.length ) == 0;
        }

        /** ****************************************************************************************
         * Returns \c true, if this string ends with the string found in parameter \p{needle}.
         * If \p{needle} is \e nulled or empty, \c true is returned.
         *
         * @param needle        The string to compare the end of this string with.
         *                      If \e nulled or empty, \c true is returned.
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         * @return \c true if \p{needle} is found at the end of this, \c false otherwise. *
         ******************************************************************************************/
        template<bool        TCheck      = true,
                 lang::Case  TSensitivity =lang::Case::Sensitive>
        inline
        bool EndsWith( const StringBase& needle )                                              const
        {
            if ( TCheck )
            {
                if ( needle.length > length )
                    return false;
                if ( needle.length == 0 )
                    return true;
            }
            else
            {
                ALIB_ASSERT_ERROR( needle.length <= length,
                                   ASTR("Non checking and needle longer than this string.") );
                ALIB_ASSERT_ERROR( needle.length != 0,
                                   ASTR("Non checking and emtpy needle given.") );
            }

            return TSensitivity == lang::Case::Sensitive
                   ? CString<TChar>::Equal            ( buffer + length - needle.length,  needle.buffer, needle.length )
                   : CString<TChar>::CompareIgnoreCase( buffer + length - needle.length,  needle.buffer, needle.length ) == 0;
        }

        /** ****************************************************************************************
         * Compares this with another %StringBase.
         *
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c false is given, no check for a \e nulled object (this)
         *                      performed and this string must not be of zero length
         *                      (while \p{needle} might be of zero length).
         * @param  needle       The string to compare this string with.
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         *
         * @return
         *  -  0 if this and \p{needle} are \e nulled or if both have a length of 0 or if both
         *       share the same content
         *  - <0 if this is \e nulled and \p{needle} is not or if this is smaller than \p{needle}.
         *  - >0 if this is not \e nulled but \p{needle} is or if this is greater than \p{needle}.
         ******************************************************************************************/
        template < bool TCheck       = true,
                   Case TSensitivity = Case::Sensitive>
        inline
        int CompareTo( const StringBase&  needle )                                             const
        {
            // check \c nullptr arguments
            if (TCheck &&        IsNull() )  return  needle.IsNull() ? 0 : -1;
            if (TCheck && needle.IsNull() )  return  +1;

            // zero length ?
            if ( TCheck && length == 0 )  return  needle.length == 0 ? 0 : -1;
            if (     needle.length == 0 )  return +1;

            bool   iAmShorter= length < needle.length;
            integer shortLen=   iAmShorter ? length : needle.length;

            int cmpVal=  (TSensitivity == lang::Case::Sensitive)
                         ? CString<TChar>::Compare          ( buffer, needle.buffer, shortLen )
                         : CString<TChar>::CompareIgnoreCase( buffer, needle.buffer, shortLen );

            if ( cmpVal != 0 || length == needle.length )
                return cmpVal;
            return iAmShorter ? -1 : 1;
        }

        /** ****************************************************************************************
         * Compares this with a region of another %StringBase.
         *
         * @param needle             The string to compare this string with.
         * @param needleRegionStart  The start of the region in \p{needle} to compare this object
         *                           with.
         * @param needleRegionLength The length of the region in \p{needle} to compare this object
         *                           with.
         *                           Defaults to \b MaxLen;
         * @tparam TCheck            Defaults to \c true which is the normal invocation mode.
         *                           If \c false is given, no check for a \e nulled comparison
         *                           object is performed and this string must not be empty.
         *                           Furthermore, no check is performed whether the given region
         *                           fits to parameter \p{needle}. This also means that the default
         *                           value must not be used with <em>TCheck==\<\c false\></em>.
         * @tparam TSensitivity      Determines if comparison is case sensitive (the default) or not.
         *
         * @return
         *  -  0 if this and \p{needle} are \e nulled or if both have a length of 0 or if both
         *       share the same content
         *  - <0 if this is \e nulled and \p{needle} is not or if this is smaller than \p{needle}.
         *  - >0 if this is not \e nulled but \p{needle} is or if this is greater than \p{needle}.
         ******************************************************************************************/
        template < bool TCheck       = true,
                   Case TSensitivity = Case::Sensitive>
        inline
        int CompareTo(  const StringBase&  needle,
                        integer       needleRegionStart,
                        integer       needleRegionLength  = MaxLen  )                          const
        {
            if ( TCheck )
            {
                StringBase cmpSub( needle.buffer, 0);
                needle.AdjustRegion( needleRegionStart, needleRegionLength );
                cmpSub.buffer+=   needleRegionStart;
                cmpSub.length=    needleRegionLength;

                return CompareTo<true,  TSensitivity>( cmpSub );
            }
            else
                return CompareTo<false, TSensitivity>( StringBase( needle.buffer + needleRegionStart, needleRegionLength ) );
        }

        /** ****************************************************************************************
         * Compares a region of this object with a region of another %StringBase.
         *
         * @param needle          The string to compare this string with.
         * @param cmpRegionStart  The start of the region in \p{needle} to compare this object
         *                        with.
         * @param cmpRegionLength The length of the region in \p{needle} to compare this object
         *                        with.
         * @param regionStart     The start of the region in this object to compare with
         * @param regionLength   The length of the region in this object to compare with.
         *                        Defaults to \b MaxLen;
         * @tparam TCheck         Defaults to \c true which is the normal invocation mode.
         *                        If \c false is given, no check for a \e nulled comparison
         *                        object is performed and this string must not be empty.
         *                        Furthermore, no check is performed whether the given regions fit
         *                        to this object respectively the other region to the object given
         *                        with parameter \p{needle}.
         *                        This also means that the default value must not be used
         *                        with <em>TCheck==\<\c false\></em>.
         * @tparam TSensitivity   Determines if comparison is case sensitive (the default) or not.
         *
         * @return
         *  -  0 if this and \p{needle} are \e nulled or if both have a length of 0 or if both
         *       share the same content
         *  - <0 if this is \e nulled and \p{needle} is not or if this is smaller than \p{needle}.
         *  - >0 if this is not \e nulled but \p{needle} is or if this is greater than \p{needle}.
         ******************************************************************************************/
        template < bool TCheck       = true,
                   Case TSensitivity = Case::Sensitive>
        inline
        int CompareTo(  const StringBase&   needle,
                        integer         cmpRegionStart,
                        integer         cmpRegionLength,
                        integer         regionStart,
                        integer         regionLength    = MaxLen    )                          const
        {
            if ( TCheck )
            {
                StringBase cmpSub( needle.buffer, 0);
                needle.AdjustRegion( cmpRegionStart, cmpRegionLength );
                cmpSub.buffer+=   cmpRegionStart;
                cmpSub.length=    cmpRegionLength;

                AdjustRegion( regionStart, regionLength );
                return StringBase( buffer + regionStart, regionLength ).CompareTo<true, TSensitivity>( cmpSub );
            }
            else
                return StringBase( buffer + regionStart, regionLength )
                        .CompareTo<false, TSensitivity>( StringBase( needle.buffer + cmpRegionStart,
                                                                     cmpRegionLength )               );

        }

        /**
         * Uses method #CompareTo with parameter \p{op} to perform a lexical comparison.
         *
         * @param op The string to compare this string with.
         * @returns \c true if this is lexically smaller then \p{op}, \c false otherwise.
         */
        inline
        bool     operator<  (const StringBase& op) const { return CompareTo( op ) <  0 ;  }

        /**
         * Uses method #CompareTo with parameter \p{op} to perform a lexical comparison.
         *
         * @param op The string to compare this string with.
         * @returns \c true if this is lexically smaller or equal then \p{op}, \c false otherwise.
         */
        inline
        bool     operator<= (const StringBase& op) const { return CompareTo( op ) <=  0 ;  }

        /**
         * Uses method #CompareTo with parameter \p{op} to perform a lexical comparison.
         *
         * @param op The string to compare this string with.
         * @returns \c true if this is lexically greater then \p{op}, \c false otherwise.
         */
        inline
        bool     operator>  (const StringBase& op) const { return CompareTo( op ) >  0 ;  }

        /**
         * Uses method #CompareTo with parameter \p{op} to perform a lexical comparison.
         *
         * @param op The string to compare this string with.
         * @returns \c true if this is lexically greater or equal then \p{op}, \c false otherwise.
         */
        inline
        bool     operator>= (const StringBase& op) const { return CompareTo( op ) >=  0 ;  }

        /**
         * Uses method #Equals with parameter \p{op} to test on equality.
         *
         * @param op The string to compare this string with.
         * @returns \c true if the strings are equal, \c false otherwise.
         */
        inline
        bool     operator== (const StringBase& op) const { return Equals( op );  }

        /**
         * Uses method #Equals with parameter \p{op} to test on equality.
         *
         * @param op The string to compare this string with.
         * @returns \c true if the strings are equal, \c false otherwise.
         */
        inline
        bool     operator!= (const StringBase& op) const { return !Equals( op ) != 0 ;  }

    /** ############################################################################################
     * @name Search
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Searches a character starting from a given position.
         *
         * @tparam TCheck   Defaults to \c true which is the normal invocation mode.
         *                  If \c false is given, no range check is performed.
         * @param needle    The character to search for.
         * @param startIdx  The index in this to start searching the character.
         *                  Defaults to \c 0.
         *
         * @return  -1 if the character \p{needle} is not found.
         *          Otherwise the index of its first occurrence.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer  IndexOf( TChar needle, integer startIdx = 0 )                                 const
        {
            ALIB_STRING_DBG_CHK(this)

            if ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( startIdx <  0      )  startIdx= 0;
                else if ( startIdx >= length )  return -1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length,
                                   ASTR("Non checking and index out of range") );
            }

            const TChar* result=  CString<TChar>::Search( buffer + startIdx, length - startIdx, needle );

            return result != nullptr ? result  -  buffer
                                     : -1;
        }

        /** ****************************************************************************************
         * Searches a character within a region of this.
         *
         * @param needle       The character to search for.
         * @param regionStart  The start of the region to search the character in.
         * @param regionLength The length of the region to search the character in.
         * @tparam TCheck      Defaults to \c true which is the normal invocation mode.
         *                     If \c false is given, no range check is performed.
         * @return  -1 if the character \p{needle} is not found.
         *          Otherwise the index of its first occurrence.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer         IndexOf( TChar needle, integer regionStart, integer regionLength )    const
        {
            ALIB_STRING_DBG_CHK(this)

            if ( TCheck )
            {
                // adjust range, if empty return -1
                if ( AdjustRegion( regionStart, regionLength ) )
                    return -1;
            }
            else
            {
                #if ALIB_DEBUG
                    integer rs= regionStart;
                    integer rl= regionLength;
                    ALIB_ASSERT_ERROR( !AdjustRegion( rs, rl ) && rs == regionStart && rl == regionLength,
                                       ASTR("Non checking and region out of range or empty") );
                #endif
            }

            const TChar* result=  CString<TChar>::Search( buffer + regionStart, regionLength, needle );

            return result != nullptr ? result  -  buffer
                                     : -1;
        }

        /** ****************************************************************************************
         * Like \alib{strings::StringBase,IndexOf} but in case the character is not found, this
         * method returns the length of this string instead of \c -1.
         * Depending on the invocation context, the choice for the right version of this method may
         * lead to shorter and more efficient code.
         *
         * @param needle  The character to search for.
         * @return  This strings #Length if the character \p{needle} is not found.
         *          Otherwise the index of first occurrence.
         ******************************************************************************************/
        inline
        integer      IndexOfOrLength( TChar needle )                                           const
        {
            ALIB_STRING_DBG_CHK(this)
            const TChar* result=   CString<TChar>::Search( buffer, length, needle );

            return result != nullptr ? result  -  buffer
                                     : length;
        }

        /** ****************************************************************************************
         * Like \alib{strings::StringBase,IndexOf} but in case the character is not found, this
         * method returns the length of this string instead of \c -1.
         * Depending on the invocation context, the choice for the right version of this method may
         * lead to shorter and more efficient code.
         *
         * @param needle    The character to search for.
         * @param startIdx  The index in this to start searching the character.
         * @tparam TCheck   Defaults to \c true which is the normal invocation mode.
         *                  If \c false is given, no range check is performed.
         * @return  This strings #Length if the character \p{needle} is not found.
         *          Otherwise the index of first occurrence.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer      IndexOfOrLength( TChar needle, integer startIdx )                         const
        {
            ALIB_STRING_DBG_CHK(this)
            if ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( startIdx <  0      )  startIdx= 0;
                else if ( startIdx >= length )  return length;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length,
                                 ASTR("Non checking and index out of range") );
            }

            const TChar* result= CString<TChar>::Search( buffer + startIdx, length - startIdx, needle );
            return result != nullptr ? result  -  buffer
                                     : length;
        }

        /** ****************************************************************************************
         * Searches a character starting backwards from the end or a given start index.
         *
         * @tparam TCheck      Defaults to \c true which is the normal invocation mode.
         *                     If \c false is given, no range check is performed.
         *                     Also, in this case, parameter startIndex must be provided.
         *
         * @param needle       The character to search for.
         * @param startIndex   The index in this to start searching the character.
         *                     Defaults to \b MaxLen.
         *
         * @return  -1 if the character \p{needle} is not found.
         *          Otherwise the index of its last occurrence relative to the start index.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer    LastIndexOf( TChar needle, integer startIndex= MaxLen )                     const
        {
            ALIB_STRING_DBG_CHK(this)

            if ( TCheck )
            {
                // adjust range, if empty return -1
                if ( startIndex <  0      )   return -1;
                if ( startIndex >= length )   startIndex= length-1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIndex >= 0 && startIndex < length,
                                  ASTR("Non checking and index out of range")   );
            }

            while( startIndex >= 0 && buffer[ startIndex ] != needle )
                startIndex--;

            return startIndex;
        }

        /** ****************************************************************************************
         * Returns the index of the first character which is included, respectively <em>not</em>
         * included in a given set of characters.
         *
         * \note
         *   In derived class \b %TString, a faster version (using \e std::strpbrk() respectively
         *   \e std::strspn()) is available. So, if performance is important, it might be advisable
         *   to copy this \b %StringBase (and the needles) to a terminatable buffer.
         *
         * This method searches forwards. For backwards search, see #LastIndexOf.
         *
         * @param needles     Pointer to a zero terminated set of characters to be taken into
         *                    account.
         * @param startIdx    The index to start the search at. If the given value is less than 0,
         *                    it is set to 0. If it exceeds the length of the string, the length of
         *                    the string is returned.
         *                    Defaults to 0.
         * @tparam TInclusion Denotes whether the search returns the first index that holds a value
         *                    that is included or that is not excluded in the set of needle
         *                    characters.
         * @tparam TCheck     Defaults to \c true which is the normal invocation mode.
         *                    If \c <false\> is given, no parameter checks are performed.
         *
         * @return The index of the first character found which is included, respectively not
         *         included, in the given set of characters.
         *         If nothing is found, -1 is returned.
         ******************************************************************************************/
        template <lang::Inclusion   TInclusion,
                  bool              TCheck      = true>
        inline
        integer    IndexOfAny( const StringBase& needles, integer startIdx= 0 )                const
        {
            if (TCheck)
            {
                if ( startIdx < 0       ) startIdx= 0;
                if ( startIdx >= length ) return   -1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length && needles.Length() != 0,
                                   ASTR("Non checking and illegal parameters") );
            }


            integer idx= TInclusion == lang::Inclusion::Include
              ? CString<TChar>::IndexOfAnyIncluded( buffer + startIdx,  length - startIdx, needles.Buffer(), needles.Length() )
              : CString<TChar>::IndexOfAnyExcluded( buffer + startIdx,  length - startIdx, needles.Buffer(), needles.Length() );

            return idx == -1 ? -1 : startIdx + idx;
        }

        /** ****************************************************************************************
         * Returns the index of the last character which is included, respectively <em>not</em>
         * included in a given set of characters.
         *
         * This method searches backwards starting at the given index. For forwards search, see
         * #IndexOf.
         *
         * @param needles     Pointer to a zero terminated set of characters to be searched for.
         * @param startIdx    The index to start the search at. The value is cropped to be in
         *                    the bounds of 0 and the length of this object minus one.
         *                    Defaults to maximum integer value.
         * @tparam TInclusion Denotes whether the search returns the first index that holds a value
         *                    that is included or that is not excluded in the set of needle
         *                    characters.
         * @tparam TCheck     Defaults to \c true which is the normal invocation mode.
         *                    If \c <false\> is given, no parameter checks are performed.
         *
         * @return The index of the first character found which is included, respectively not
         *         included, in the given set of characters.
         *         If nothing is found, -1 is returned.
         ******************************************************************************************/
        template <lang::Inclusion   TInclusion,
                  bool              TCheck = true   >
        inline
        integer LastIndexOfAny( const StringBase& needles, integer startIdx= MaxLen ) const
        {
            if (TCheck)
            {
                if ( startIdx < 0       ) return -1;
                if ( startIdx >= length ) startIdx=  length - 1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length && needles.Length() != 0,
                                   ASTR("Non checking and illegal parameters") );
            }

            return   TInclusion == lang::Inclusion::Include
                       ? CString<TChar>::LastIndexOfAnyInclude(  Buffer(), startIdx, needles.Buffer(), needles.Length() )
                       : CString<TChar>::LastIndexOfAnyExclude(  Buffer(), startIdx, needles.Buffer(), needles.Length() );
        }

        /** ****************************************************************************************
         * Searches the given (unterminated) %StringBase in the Buffer.
         *
         * If this string and string \p{needle} \p{needle} are known to be zero-terminated,
         * it is advisable to use the faster implementation of this method,
         * \ref aworx::lib::strings::TStringBase::IndexOf "TString::IndexOf".<br>
         * This method is useful for example to search needles of type
         * \alib{strings,SubstringBase,Substring} (which are not terminatable).
         *
         * If \p{needle} is empty, \p{startIdx} is returned.
         *
         * @param  needle         The string to search for.
         * @param  startIdx       The index to start the search at. Optional and defaults to \c 0.
         * @tparam TSensitivity   Case sensitivity of the comparison.
         *                        Optional and defaults to \b Case::Sensitive.
         * @tparam TCheck         Defaults to \c true which is the normal invocation mode.
         *                        If \c false is given, parameter \p{startIdx} must be valid and
         *                        \p{needle} must not be empty.
         *
         * @return    -1 if the string is not found. Otherwise the index of first occurrence.
         ******************************************************************************************/
        template<bool           TCheck        = true,
                 lang::Case     TSensitivity  = lang::Case::Sensitive>
        inline
        integer  IndexOf( const StringBase&  needle,
                          integer            startIdx= 0    )                                  const
        {
            integer nLen=   needle.Length();
            if (TCheck)
            {
                if ( needle.IsNull()  )
                    return  -1;
                if ( startIdx < 0 )                startIdx= 0;
                if ( startIdx + nLen > length )    return -1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx <= length && needle.IsNotNull(),
                                   ASTR("Non checking and illegal parameters") );
            }


            return indexOfString<TSensitivity>(needle, startIdx);
        }

        /** ****************************************************************************************
         * Searches the first difference with given string.
         * If this string starts with \p{needle}, then the length of \p{needle} is returned.
         *
         * @tparam TCheck      Defaults to \c true which is the normal invocation mode.
         *                     If \c false is given, no range check is performed.
         * @param needle       The character to search for.
         * @param sensitivity  Case sensitivity of the comparison.
         *                     Optional and defaults to \b Case::Sensitive.
         * @param idx          The index in this to start comparing with \p{needle}.
         *                     Optional and defaults to \c 0.
         *
         * @return   The index of the first difference found in \p{needle} or the needle's length.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer  IndexOfFirstDifference( const StringBase&  needle,
                                         lang::Case     sensitivity = lang::Case::Sensitive,
                                         integer        idx         = 0                     )  const
        {
            ALIB_STRING_DBG_CHK(this)

            if ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( idx <  0      )  idx= 0;
                else if ( idx >= length )  return idx;
            }
            else
            {
                ALIB_ASSERT_ERROR( idx >= 0 && idx < length,
                                   ASTR("Non checking and index out of range") );
            }

            return   CString<TChar>::IndexOfFirstDifference(  buffer + idx,   length - idx,
                                                              needle.buffer,  needle.length,
                                                              sensitivity                         );
        }

        /** ****************************************************************************************
         * Counts all occurrences of \p{needle} from \p{startPos} to the end of the string.
         *
         * For empty strings \p{needle}, \c 0 is returned.
         *
         * @param needle    The string to search for.
         * @param startPos  The index to start the counting.
         *                  Optional and defaults to \c 0.
         * @tparam TCheck   Defaults to \c true which is the normal invocation mode.
         *                  If \c false is given, no range check is performed.
         *
         * @return  The index of the first difference in \p{needle}.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        int  CountChar( TChar           needle,
                        integer         startPos        = 0                                  ) const
        {
            ALIB_STRING_DBG_CHK(this)
            if ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( startPos <  0      )  startPos= 0;
                else if ( startPos >= length )  return 0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                 ASTR("Non checking and index out of range") );
            }


            int     result= 0;
            while( startPos < length && (startPos= IndexOf<false>( needle, startPos )) >= 0 )
            {
                startPos++;
                result++;
            }

            return result;
        }

        /** ****************************************************************************************
         * Counts all occurrences of \p{needle}, unless followed by \p{omit}, starting at
         * \p{startPos} to the end of the string.
         *
         * For empty strings \p{needle}, \c 0 is returned.
         * Also, for empty strings \p{omit}, \c 0 is returned.
         *
         * @param needle    The string to search for.
         * @param omit      Omit occurrence if the given character follows.
         * @param startPos  The index to start the counting.
         * @tparam TCheck   Defaults to \c true which is the normal invocation mode.
         *                  If \c false is given, no range check is performed.
         *
         * @return  The index of the first difference in \p{needle}.
         ******************************************************************************************/
        template <bool TCheck= true>
        int  CountChar( TChar           needle,
                        TChar           omit,
                        integer         startPos                                             ) const
        {
            ALIB_STRING_DBG_CHK(this)
            if ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( startPos <  0      )  startPos= 0;
                else if ( startPos >= length )  return 0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   ASTR("Non checking and index out of range") );
            }

            int     result= 0;
            while( startPos < length && (startPos= IndexOf<false>( needle, startPos )) >= 0 )
            {
                startPos++;
                if( startPos < Length()  &&  *(buffer + startPos) == omit  )
                    continue;

                result++;
            }

            return result;
        }

        /** ****************************************************************************************
         * Counts all occurrences of \p{needle} from \p{startPos} to the end of the string.
         *
         * For empty strings \p{needle}, \c 0 is returned.
         *
         * @param needle        The string to search for.
         * @param startPos      The index to start the counting.
         *                      Optional and defaults to \c 0.
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c false is given, parameter \p{startIdx} must be valid and
         *                      \p{needle} must not be empty.
         * @tparam TSensitivity Case sensitivity of the comparison.
         *                      Optional and defaults to \b Case::Sensitive.
         *
         * @return  The index of the first difference in \p{needle}.
         ******************************************************************************************/
        template< bool        TCheck       = true,
                  lang::Case  TSensitivity = lang::Case::Sensitive >
        int  Count( const StringBase&   needle,
                    integer         startPos        = 0                               )        const
        {
            ALIB_STRING_DBG_CHK(this)
            integer nLen= needle.Length();
            if( nLen == 0  )
                return 0;
            if (TCheck)
            {
                if ( startPos < 0 )                startPos= 0;
                if ( startPos + nLen > length )    return  0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   ASTR("Non checking and illegal parameters") );
            }

            int     result= 0;
            while( (startPos= IndexOf<false, TSensitivity>( needle, startPos )) >= 0 )
            {
                startPos+= needle.Length();
                result++;
            }

            return result;
        }

        /** ****************************************************************************************
         * Counts all occurrences of \p{needle}, unless followed by \p{omit}, starting at
         * \p{startPos} to the end of the string.
         *
         * For empty strings \p{needle}, \c 0 is returned.
         * Also, for empty strings \p{omit}, \c 0 is returned.
         *
         * @param needle        The string to search for.
         * @param omit          Omit occurrence if the given string follows.
         * @param startPos      The index to start the counting.
         *                      Optional and defaults to \c 0.
         * @tparam TSensitivity Case sensitivity of the comparison.
         *                      Optional and defaults to \b Case::Sensitive.
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c false is given, parameter \p{startIdx} must be valid and
         *                      \p{needle} must not be empty.
         *
         * @return  The index of the first difference in \p{needle}.
         ******************************************************************************************/
        template<bool           TCheck        = true,
                 lang::Case     TSensitivity  = lang::Case::Sensitive>
        int  Count( const StringBase&   needle,
                    const StringBase&   omit,
                    integer         startPos        = 0                                  )     const
        {
            ALIB_STRING_DBG_CHK(this)
            integer nLen= needle.Length();
            if ( nLen == 0  )
                return  0;
            if (TCheck)
            {
                if ( startPos < 0 )                startPos= 0;
                if ( startPos + nLen > length )    return  0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   ASTR("Non checking and illegal parameters") );
            }


            int     result= 0;
            while( (startPos= IndexOf<false, TSensitivity>( needle, startPos )) >= 0 )
            {
                startPos+= nLen;
                if(     startPos + omit.Length() <= Length()
                    &&  this->Substring<false>( startPos, omit.Length() ).template Equals<TSensitivity>( omit )
                  )
                    continue;

                result++;
            }

            return result;
        }



    /** ############################################################################################
     * @name Parsing Numbers
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Parses an integer value consisting of characters \c '0' to \c '9' from this string.
         * <br>Unlike with #ParseInt or #ParseDec, no sign, whitespaces or group characters are
         * accepted.
         *
         * @param startIdx     The start index from where the integer value is tried to be parsed.
         *                     Optional and defaults to \c 0.
         * @param[out] newIdx  Optional output variable that will point to the first character
         *                     in this string after the float number that was parsed.
         *                     If parsing fails, it will be set to the value of parameter startIdx.
         *                     Therefore, this parameter can be used to check if a value was found.
         *
         * @return  The parsed value. In addition, the parameter \p{newIdx} is set to point
         *          to the first character behind any found integer number.
         ******************************************************************************************/
        ALIB_API
        uint64_t  ParseDecDigits( integer startIdx =0, integer* newIdx= nullptr ) const;


        /** ****************************************************************************************
         * Parses an integer value in decimal, binary, hexadecimal or octal format from
         * the string by invoking method
         * \alib{strings,NumberFormatBase::ParseInt,NumberFormat::ParseInt}
         * on the given \p{numberFormat} instance.<br>
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}
         * which is configured to not using - and therefore also not parsing - grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on number conversion, see class
         * \alib{strings,NumberFormatBase,NumberFormat}. All of its interface methods
         * have a corresponding implementation within class \b %AString.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        ALIB_API
        int64_t  ParseInt( integer startIdx =0, NumberFormatBase<TChar>* numberFormat= nullptr,
                           integer* newIdx= nullptr ) const;

        /** ****************************************************************************************
         * Overloaded version of #ParseInt(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        int64_t  ParseInt( NumberFormatBase<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseInt( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of #ParseInt(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        int64_t  ParseInt( integer* newIdx ) const
        {
            return ParseInt( 0, nullptr, newIdx );
        }


        /** ****************************************************************************************
         * Overloaded version of #ParseInt(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        int64_t  ParseInt( integer startIdx, integer* newIdx ) const
        {
            return ParseInt( startIdx, nullptr, newIdx );
        }

        /** ****************************************************************************************
         * Reads an unsigned 64-bit integer in standard decimal format at the given position
         * from this %AString. This is done, by invoking \alib{strings,NumberFormatBase::ParseDec}
         * on the given \p{numberFormat} instance.<br>
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,NumberFormatBase::Computational}
         * which is configured to not using - and therefore also not parsing - grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail.
         * For reading signed integer values, see methods #ParseInt, for floating point numbers
         * #ParseFloat.
         *
         * For more information on number conversion, see class \alib{strings,NumberFormatBase}.
         * All of its interface methods have a corresponding implementation within class
         * \b %AString.
         *
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        ALIB_API
        uint64_t  ParseDec( integer startIdx =0, NumberFormatBase<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;

        /** ****************************************************************************************
         * Overloaded version of #ParseDec(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseDec( NumberFormatBase<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseDec( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of #ParseDec(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseDec( integer* newIdx ) const
        {
            return ParseDec( 0, nullptr, newIdx );
        }


        /** ****************************************************************************************
         * Overloaded version of #ParseDec(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseDec( integer startIdx, integer* newIdx ) const
        {
            return ParseDec( startIdx, nullptr, newIdx );
        }

        /** ****************************************************************************************
         * Reads an unsigned 64-bit integer in binary format at the given position
         * from this \b %AString. This is done, by invoking
         * \alib{strings,NumberFormatBase::ParseBin,NumberFormat::ParseBin}
         * on the given \p{numberFormat} instance.<br>
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}
         * which is configured to not using - and therefore also not parsing - grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on number conversion, see class
         * \alib{strings,NumberFormatBase,NumberFormat}. All of its interface methods
         * have a corresponding implementation within class \b %AString.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        ALIB_API
        uint64_t  ParseBin( integer startIdx =0, NumberFormatBase<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;


        /** ****************************************************************************************
         * Overloaded version of #ParseBin(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseBin( NumberFormatBase<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseBin( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of #ParseBin(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseBin( integer* newIdx ) const
        {
            return ParseBin( 0, nullptr, newIdx );
        }


        /** ****************************************************************************************
         * Overloaded version of #ParseBin(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseBin( integer startIdx, integer* newIdx ) const
        {
            return ParseBin( startIdx, nullptr, newIdx );
        }

        /** ****************************************************************************************
         * Reads an unsigned 64-bit integer in hexadecimal format at the given position
         * from this \b %AString. This is done, by invoking
         * \alib{strings,NumberFormatBase::ParseHex,NumberFormat::ParseHex}
         * on the given \p{numberFormat} instance.<br>
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}
         * which is configured to not using - and therefore also not parsing - grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on number conversion, see class
         * \alib{strings,NumberFormatBase,NumberFormat}. All of its interface methods
         * have a corresponding implementation within class \b %AString.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        ALIB_API
        uint64_t  ParseHex( integer startIdx =0, NumberFormatBase<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;


        /** ****************************************************************************************
         * Overloaded version of #ParseHex(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseHex( NumberFormatBase<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseHex( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of #ParseHex(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseHex( integer* newIdx ) const
        {
            return ParseHex( 0, nullptr, newIdx );
        }


        /** ****************************************************************************************
         * Overloaded version of #ParseHex(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseHex( integer startIdx, integer* newIdx ) const
        {
            return ParseHex( startIdx, nullptr, newIdx );
        }

        /** ****************************************************************************************
         * Reads an unsigned 64-bit integer in octal format at the given position
         * from this \b %AString. This is done, by invoking
         * \alib{strings,NumberFormatBase::ParseOct,NumberFormat::ParseOct}
         * on the given \p{numberFormat} instance.<br>
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}
         * which is configured to not using - and therefore also not parsing - grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on number conversion, see class
         * \alib{strings,NumberFormatBase,NumberFormat}. All of its interface methods
         * have a corresponding implementation within class \b %AString.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        ALIB_API
        uint64_t  ParseOct( integer startIdx =0, NumberFormatBase<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;


        /** ****************************************************************************************
         * Overloaded version of #ParseOct(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseOct( NumberFormatBase<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseOct( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of #ParseOct(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseOct( integer* newIdx ) const
        {
            return ParseOct( 0, nullptr, newIdx );
        }


        /** ****************************************************************************************
         * Overloaded version of #ParseOct(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseOct( integer startIdx, integer* newIdx ) const
        {
            return ParseOct( startIdx, nullptr, newIdx );
        }

        /** ****************************************************************************************
         * Reads a floating point number at the given position from this \b %AString.
         * This is done, by invoking
         * \alib{strings,NumberFormatBase::ParseFloat,NumberFormat::ParseFloat}
         * on the given \p{numberFormat} instance.<br>
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}
         * which is configured to 'international' settings (not using the locale) and therefore
         * also not parsing grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on parsing options for floating point numbers and number
         * conversion in general, see class
         * \alib{strings,NumberFormatBase,NumberFormat}. All of its interface methods
         * have a corresponding implementation within class \b %AString.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        ALIB_API
        double   ParseFloat( integer startIdx =0, NumberFormatBase<TChar>* numberFormat= nullptr,
                             integer* newIdx= nullptr ) const;

        /** ****************************************************************************************
         * Overloaded version of #ParseFloat(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        double  ParseFloat( NumberFormatBase<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseFloat( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of #ParseFloat(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        double  ParseFloat( integer* newIdx ) const
        {
            return ParseFloat( 0, nullptr, newIdx );
        }


        /** ****************************************************************************************
         * Overloaded version of #ParseFloat(int =,NumberFormatBase<TChar>* =,int* =) providing default values
         * for omitted parameters.
         *
         * @param startIdx     The start index for parsing.
         *                     Optional and defaults to \c 0.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \p{startIdx}.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        double  ParseFloat( integer startIdx, integer* newIdx ) const
        {
            return ParseFloat( startIdx, nullptr, newIdx );
        }

    /** ############################################################################################
     * @name Conversion
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Copies the strings contents into a given character buffer.
         *
         * @param dest   The destination buffer.
         * @return    The length of this string.
         ******************************************************************************************/
        inline
        integer CopyTo( TChar* dest ) const
        {
            CString<TChar>::Copy( buffer, length, dest );
            return length;
        }

    /** ############################################################################################
     * @name Helper methods
     ##@{ ########################################################################################*/
    public:
        /** ************************************************************************************
         * Adjusts a given region (in/out parameters) to fit to this object's range [0..length].
         *
         * @param[in,out] regionStart  The proposed region start which might get adjusted to fit to
         *                             range [0..length].
         * @param[in,out] regionLength The proposed region length which might get adjusted to fit to
         *                             range [0..length].
         *
         * @return   Returns \c true, if the adjusted region is empty.
         **************************************************************************************/
        inline
        bool AdjustRegion( integer& regionStart, integer& regionLength )                       const
        {
            // if start exceeds string, set to empty range at the end of the string and return true
            if (regionStart >= length)
            {
                regionStart=    length;
                regionLength=    0;
                return true; // indicate empty
            }

            // if negative start, cut it from the length
            if (regionStart < 0 )
            {
                regionLength+=    regionStart; // this adds a negative value, hence we do not exceed a given MAXINT
                regionStart=    0;
            }

            // adjust length
            integer maxLen=  length - regionStart;
            if ( regionLength > maxLen )
                 regionLength=  maxLen;
            if ( regionLength < 0 )
                regionLength= 0;

            // return true if adjusted region is empty
            return  regionLength == 0;
        }

    /** ############################################################################################
     * @name std::iterator
     ##@{ ########################################################################################*/
    public:
        /** ****************************************************************************************
         * Implementation of \c std::iterator for class \b %StringBase. This class exposes
         * #ConstIterator which uses <c>const TChar*</c> and <c>const TChar&</c> as
         * pointer and reference types. Descendant classes may expose a mutable version
         * (e.g. \alib{strings,AStringBase,AString}).
         *
         * As the name of the class indicates, this iterator satisfies the std library concept
         * [RandomAccessIterator](http://en.cppreference.com/w/cpp/concept/RandomAccessIterator).
         ******************************************************************************************/
        template<typename TPointer, typename TReference>
        class RandomAccessIteratorBase
            : public std::iterator< std::random_access_iterator_tag,  // iterator_category
                                    TChar,                            // value_type
                                    integer,                          // distance type
                                    TPointer,                         // pointer
                                    TReference                        // reference
                                  >
        {
            protected:
                /// The pointer into the buffer is all we store.
                TPointer p;

            public:
                /** Constructor.
                 *  @param _p Our initial value       */
                explicit RandomAccessIteratorBase( TPointer _p = nullptr ) : p(_p)
                {
                }

            //######################   To satisfy concept of  InputIterator   ######################

                /** Prefix increment operator.
                 *  @return A reference to ourselves. */
                RandomAccessIteratorBase& operator++()
                {
                    p++;
                    return *this;
                }

                /** Postfix increment operator.
                 *  @return A reference to ourselves. */
                RandomAccessIteratorBase operator++(int)
                {
                    return RandomAccessIteratorBase(p++);
                }

                /** Comparison operator.
                 *  @param other  The iterator to compare ourselves to.
                 *  @return \c true if this and given iterator are equal, \c false otherwise. */
                bool operator==(RandomAccessIteratorBase other)                                const
                {
                    return p == other.p;
                }

                /** Comparison operator.
                 *  @param other  The iterator to compare ourselves to.
                 *  @return \c true if this and given iterator are not equal, \c false otherwise. */
                bool operator!=(RandomAccessIteratorBase other)                                const
                {
                    return !(*this == other);
                }

                /** Retrieves the character that this iterator references.
                 * @return The character value.                               */
                TReference operator*()                                                         const
                {
                    return *p;
                }


            //##################   To satisfy concept of  BidirectionalIterator   ##################

                /** Prefix decrement operator.
                 *  @return A reference to ourselves. */
                RandomAccessIteratorBase& operator--()
                {
                    p--;
                    return *this;
                }


                /** Postfix decrement operator.
                 *  @return An iterator that with the old value. */
                RandomAccessIteratorBase operator--(int)
                {
                    return RandomAccessIteratorBase(p--);
                }


            //##################   To satisfy concept of  RandomAccessIterator   ###################

                /** Addition assignment.
                 *  @param n The value to subtract.
                 *  @return A reference to ourselves. */
                RandomAccessIteratorBase& operator+=(integer n)
                {
                    p+= n;
                    return *this;
                }

                /** Subtraction assignment.
                 *  @param n The value to subtract.
                 *  @return A reference to ourselves. */
                RandomAccessIteratorBase& operator-=(integer n)
                {
                    p-= n;
                    return *this;
                }

                /** Addition.
                 *  @param n The value to subtract.
                 *  @return A reference to the new iterator. */
                RandomAccessIteratorBase operator+(integer n)                                  const
                {
                    return RandomAccessIteratorBase( p + n );
                }

                /** Subtraction.
                 *  @param n The value to subtract.
                 *  @return A reference to the new iterator. */
                RandomAccessIteratorBase operator-(integer n)                                  const
                {
                    return RandomAccessIteratorBase( p - n );
                }

                /** Difference (distance) from this iterator to the given one.
                 *  @param other  The iterator to subtract
                 *  @return The iterator to subtract.    */
                integer operator-(RandomAccessIteratorBase other)                              const
                {
                    return p - other.p;
                }

                /** Subscript operator.
                 *  @param n  The iterator to subtract
                 *  @return <c>*( (*this) + n )</c>.          */
                TReference operator[]( integer n )                                             const
                {
                    return *( p + n );
                }

            //#### Comparison operators (also needed to satisfy concept of RandomAccessIterator) ###

                /** Compares this iterator with the given one.
                 *  @param other  The iterator to compare
                 *  @return \c true if this iterator is \e smaller than \p{other},
                 *          \c false otherwise. */
                bool operator<(RandomAccessIteratorBase other)                                 const
                {
                    return p < other.p;
                }

                /** Compares this iterator with the given one.
                 *  @param other  The iterator to compare
                 *  @return \c true if this iterator is \e smaller than or equal to \p{other},
                 *          \c false otherwise. */
                bool operator<=(RandomAccessIteratorBase other)                                const
                {
                    return p <= other.p;
                }


                /** Compares this iterator with the given one.
                 *  @param other  The iterator to compare
                 *  @return \c true if this iterator is \e greater than \p{other},
                 *          \c false otherwise. */
                bool operator>(RandomAccessIteratorBase other)                                 const
                {
                    return p > other.p;
                }

                /** Compares this iterator with the given one.
                 *  @param other  The iterator to compare
                 *  @return \c true if this iterator is \e greater than or equal to \p{other},
                 *          \c false otherwise. */
                bool operator>=(RandomAccessIteratorBase other)                                const
                {
                    return p >= other.p;
                }
        };

        /** The constant iterator exposed by this character container. A Mutable version is
         *  found only in descendant classes (e.g. \alib{strings,AStringBase,AString}).
         */
        using ConstIterator= RandomAccessIteratorBase<const TChar*, const TChar&>;


        /**
         * Returns an iterator pointing to the start of this string.
         * @return The start of this string.
         */
        ConstIterator begin()                                                                  const
        {
            return ConstIterator( Buffer() );
        }

        /**
         * Returns an iterator pointing to the first character behind this string.
         * @return The end of this string.
         */
        ConstIterator end()                                                                    const
        {
            return ConstIterator( Buffer() + length );
        }

        /** ****************************************************************************************
         * Constructs this string using start and end iterators.
         *
         * @param start An iterator referencing the start of the string.
         * @param end   An iterator referencing the end of the string.
         ******************************************************************************************/
        inline
        StringBase( ConstIterator& start, ConstIterator& end )
        : buffer( &*start)
        , length( end-start >= 0 ? end-start : 0 )
        {}

    protected:
        ALIB_WARNINGS_IGNORE_DOCS // needed due to a bug in current clang
        /** ****************************************************************************************
         * Implementation of substring search function.
         * @param  needle       The substring to search.
         * @param  startIdx     The start index of the search.
         * @tparam TSensitivity The letter case sensitivity of the search.
         * @return The index of the first occurrence of \p{needle}, respectively \c -1 if not found.
         ******************************************************************************************/
        template<lang::Case TSensitivity =lang::Case::Sensitive>
        ALIB_API integer
        indexOfString( const StringBase&  needle, integer startIdx )                          const;

        ALIB_WARNINGS_RESTORE
}; // class StringBase


//! @cond NO_DOX

// #################################################################################################
// NString specifics
// #################################################################################################

template<> struct T_String<nchar*, nchar> : public std::true_type
{
    static inline constexpr const nchar*   Buffer(nchar* const       & src) { return src;  }
    static inline integer                 Length(nchar* const       & src) { return static_cast<integer>( strlen(src) );  }
};

template<> struct T_String<nchar const*, nchar> : public std::true_type
{
    static inline constexpr const nchar*   Buffer(const nchar* const & src) { return src; }
    static inline integer                 Length(const nchar* const & src) { return static_cast<integer>( strlen(src) ); }
};

template<size_t TCapacity>
struct T_StringLiteral<nchar[TCapacity], nchar>  : public std::true_type
{
    static inline constexpr const nchar*   Buffer( nchar const (&src) [TCapacity]  ) { return src; }
    static inline constexpr integer       Length()                                  { return TCapacity -1; }
};

template<> ALIB_API integer StringBase<nchar>::WStringLength()                                 const;

#if ALIB_DEBUG && !defined(_MSC_VER)
    template<> bool    StringBase<nchar>::ALIB_OTW_SetLengthLonger;
#endif

#if ALIB_DEBUG_STRINGS
extern template ALIB_API void    StringBase<nchar>::_dbgCheck() const;
#endif

extern template ALIB_API integer  StringBase<nchar>::indexOfString<Case::Sensitive>(const StringBase<nchar   >&, integer)  const;
extern template ALIB_API integer  StringBase<nchar>::indexOfString<Case::Ignore   >(const StringBase<nchar   >&, integer)  const;
extern template ALIB_API uint64_t StringBase<nchar>::ParseDecDigits( integer, integer*                           ) const;
extern template ALIB_API  int64_t StringBase<nchar>::ParseInt      ( integer, NumberFormatBase<nchar>*, integer* ) const;
extern template ALIB_API uint64_t StringBase<nchar>::ParseDec      ( integer, NumberFormatBase<nchar>*, integer* ) const;
extern template ALIB_API uint64_t StringBase<nchar>::ParseBin      ( integer, NumberFormatBase<nchar>*, integer* ) const;
extern template ALIB_API uint64_t StringBase<nchar>::ParseHex      ( integer, NumberFormatBase<nchar>*, integer* ) const;
extern template ALIB_API uint64_t StringBase<nchar>::ParseOct      ( integer, NumberFormatBase<nchar>*, integer* ) const;
extern template ALIB_API double   StringBase<nchar>::ParseFloat    ( integer, NumberFormatBase<nchar>*, integer* ) const;


// #################################################################################################
// WString specifics
// #################################################################################################

template<> struct T_String<wchar_t*, wchar_t> : public std::true_type
{
    static inline constexpr const wchar_t*   Buffer(wchar_t* const & src)  { return src;  }
    static inline integer                    Length(wchar_t* const & src)  { return static_cast<integer>( wcslen(src) );  }
};

template<> struct T_String<wchar_t const*, wchar_t> : public std::true_type
{
    static inline constexpr const wchar_t*   Buffer(const wchar_t* const & src) { return src; }
    static inline integer                    Length(const wchar_t* const & src) { return static_cast<integer>( wcslen(src) ); }
};

template<size_t TCapacity>
struct T_StringLiteral<wchar_t[TCapacity], wchar_t>  : public std::true_type
{
    static inline constexpr const wchar_t*   Buffer( wchar_t  const (&src) [TCapacity]   ) { return src; }
    static inline constexpr integer          Length()                                      { return TCapacity -1; }
};

template<> inline   integer StringBase<wchar_t>::WStringLength()                               const
{
    return length;
}

extern template ALIB_API integer  StringBase<wchar>::indexOfString<Case::Sensitive>(const StringBase<wchar_t>&, integer)  const;
extern template ALIB_API integer  StringBase<wchar>::indexOfString<Case::Ignore   >(const StringBase<wchar_t>&, integer)  const;
extern template ALIB_API uint64_t StringBase<wchar>::ParseDecDigits( integer, integer*                           ) const;
extern template ALIB_API  int64_t StringBase<wchar>::ParseInt      ( integer, NumberFormatBase<wchar>*, integer* ) const;
extern template ALIB_API uint64_t StringBase<wchar>::ParseDec      ( integer, NumberFormatBase<wchar>*, integer* ) const;
extern template ALIB_API uint64_t StringBase<wchar>::ParseBin      ( integer, NumberFormatBase<wchar>*, integer* ) const;
extern template ALIB_API uint64_t StringBase<wchar>::ParseHex      ( integer, NumberFormatBase<wchar>*, integer* ) const;
extern template ALIB_API uint64_t StringBase<wchar>::ParseOct      ( integer, NumberFormatBase<wchar>*, integer* ) const;
extern template ALIB_API double   StringBase<wchar>::ParseFloat    ( integer, NumberFormatBase<wchar>*, integer* ) const;


#if ALIB_DEBUG && !defined(_MSC_VER)
    template<> bool    StringBase<wchar_t>::ALIB_OTW_SetLengthLonger;
#endif

#if ALIB_DEBUG_STRINGS
extern template ALIB_API void    StringBase<wchar_t>::_dbgCheck() const;
#endif

//! @endcond

}}} // namespace [aworx::lib::strings]

// #############    False assertion fixes. Dependent on compiler and library    #############
#if defined(__clang__)
    ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<      StringBase<nchar>   > , nchar )
    ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<      StringBase<nchar> & > , nchar )
    ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<const StringBase<nchar> & > , nchar )
    ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<      StringBase<nchar> &&> , nchar )

    ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<      StringBase<wchar>   > , wchar )
    ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<      StringBase<wchar> & > , wchar )
    ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<const StringBase<wchar> & > , wchar )
    ALIB_STRING_CONSTRUCTOR_FIX( std::tuple<      StringBase<wchar> &&> , wchar )
#endif



#if defined(_MSC_VER)
    #pragma warning( pop )
#endif
#endif // HPP_ALIB_STRINGS_STRING_BASE
