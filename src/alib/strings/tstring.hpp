// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include guard
#ifndef HPP_ALIB_STRINGS_TSTRING
#define HPP_ALIB_STRINGS_TSTRING 1

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
 *
 * @addtogroup GrpALibStringsMacros
 * @{ \def  ALIB_STRING_DBG_UNTERMINATE
 * This macro serves for debugging the development (and potentially the use) of
 * derived classes, eg. class \alib{strings,AStringBase,AString}.
 * It is active only when compiler symbol \ref ALIB_DEBUG_STRINGS is \c true. When active,
 * whenever the contents of an %AString instance is modified, the buffer is explicitly
 * "unterminated" by writing a value of '\\1' at the termination position and the state is
 * stored in field
 * \ref aworx::lib::strings::TStringBase::debugIsTerminated "debugIsTerminated"
 * \ref aworx::lib::strings::TStringBase::_dbgCheck "_dbgCheck".<br>
 * Doing this, will also hint to buffers that reside in non-writable memory, hence
 * it will hint to buffers that may not be used with this class.
 *
 * The macro is active when conditional compilation symbol \ref ALIB_DEBUG_STRINGS_ON is set.
 * In case \b ALIB_DEBUG_STRINGS_ON it is not set, but symbol \ref ALIB_AVOID_ANALYZER_WARNINGS_ON
 * is, still the string is un-terminated, but no checks are are performed.
 *
 * @param tchar   The template character type name of \p{string}.
 * @param string  The string to un-terminate.
 * @param offset  The offset of the un-termination character from the current length of the string.
 *
 * @}
 *
 */

// do not indent this, for the sake of doxygen formatting
//--- debug mode ---
#if ALIB_DEBUG_STRINGS

    #define  ALIB_STRING_DBG_UNTERMINATE( tchar,string, offset )         \
    {                                                                    \
        if ( (string).Buffer() != nullptr )                              \
            (string).VBuffer()[ (string).Length()  + offset ]= '\1';     \
        static_cast<TStringBase<tchar>&>(string).debugIsTerminated= 0;  \
    }

//--- suppress analyzer warnings by writing the value ---
#elif  ALIB_AVOID_ANALYZER_WARNINGS
//TODO: fix this
    #define  ALIB_STRING_DBG_UNTERMINATE( tchar, string, offset )        \
    {                                                                    \
        if ( (string).Buffer() != nullptr )                              \
{ALIB_ASSERT( (string).Capacity() >=  (string).Length()  + offset )                              \
            (string).VBuffer()[ (string).Length()  + offset ]= '\2';     }\
    }

#else

//--- normal mode: do nothing ---
#define  ALIB_STRING_DBG_UNTERMINATE(tchar,astring, offset)

#endif


namespace aworx { namespace lib { namespace strings {

/** ************************************************************************************************
 * Template (meta programming) struct to determine if an arbitrary string type is terminatable or
 * not.
 * The 'default implementation' inherits from std::false_type
 *
 * For all user defined string types (from an \alib perspective external types), which
 * - implement template struct
 *   \ref aworx::lib::strings::T_String "T_String"
 *   to be able to serve as a string argument for implicit construction of class
 *   \alib{strings,StringBase,String}
 * - and which do not provide a terminated buffer in that <em>ToString</em> method
 * - and which should in addition be suitable to serve as string argument to implicit constructor
 *   of class  \alib{strings,TStringBase,TString}
 *
 * a specialization of this struct (that simply is derived from <em>std::true_type</em>)
 * has to be provided.
 *
 * When this is done, the capacity for the termination character '\0' has to be available
 * in the buffer that is passed in method
 * \ref aworx::lib::strings::T_String::Buffer "T_String::Buffer".
 * In other words, their buffer needs to be writable and at least one character longer than
 * their content length.
 *
 * \note
 *  If (user defined) objects are terminated already when passed to the constructor of
 *  TString, then this template is <b>not</b> needed to be defined.<br>
 *  For example, the implementation of method <em>ToString</em> for type <em>std::string</em>
 *  returns <em>std::string::c_str()</em>. This way, the string returned is well terminated and
 *  there is no need to partially specialize this struct for type <em>std::string</em>.
 **************************************************************************************************/
template<typename Type> struct  T_IsTerminatable : public std::false_type { };


/** ************************************************************************************************
 * This class specializes class
 * \alib{strings,StringBase,String} to represent zero terminated or more precisely
 * zero \e terminatable strings.
 * Zero terminated strings are especially needed when string data has to be passed to system
 * functions. Also, some efficient algorithms (platform dependent and mostly written in assembly
 * language) exist.  Hence, various functions and methods of \alib require a const reference
 * to an object of this type \b %TString instead to an object of class \b %String.
 *
 * <b>Implicit construction</b><p>
 * This class hides its parents' constructors and re-implements the template meta
 * programming based "all-for-one" constructor
 * \alib{strings::StringBase::StringBase(const TStringLike&),StringBase(const TStringLike&)} by introducing
 * \alib{strings::TStringBase::TStringBase(const TTerminatable&),TString(const TTerminatable&)}.
 *
 * As it is not possible to implement compile time assertions for the save provision of
 * terminatable types (without taking serious restrictions into account), this constructor
 * performs a run time check (in debug compilations of \alib).<br>
 * It is checked whether
 * - the string type passed is already terminated or
 * - can be terminated because a) the buffer reserved capacity for the termination value and b)
 *   the buffer is writable.
 *
 * The second check is done only if the first did not apply and is performed using partially
 * implemented template struct
 * \ref aworx::lib::strings::T_IsTerminatable "T_IsTerminatable". Consult this struct for information
 * about how to enable external (user defined) string types to be used as parameters to implicitly
 * construct objects of this class.
 *
 *  \note
 *   While the good news is that C++ string literals, any zero terminated <em>char*</em> and
 *   objects of type <em>std::strings</em> or \alib's \b %AString,
 *   are well suited to implicitly construct respectively directly serve as objects of this class,
 *   objects of type
 *   \alib{strings,StringBase,String} itself and especially of type
 *   \alib{strings,SubstringBase,Substring} are not.
 *   Therefore the latter are accepted only if they are terminated already.
 *   But obviously, this is especially unlikely for objects of type
 *   \alib{strings,SubstringBase,Substring}.
 *
 *  \note
 *   Therefore, for example to pass a <em>Substring</em>
 *   as a parameter to functions accepting only terminatable strings, it is possible to
 *   wrap them into temporary objects of type
 *   \alib{strings,AStringBase,AString} or, if maximum size is known at compile time,
 *   of type
 *   \alib{strings,PreallocatedStringBase,PreallocatedString}.
 *   Of-course, a performance drawback (for creating the temporary copy of the contents) has to
 *   be taken into account.
 *
 *  \note
 *   Alternatively, class \b %Substring and class \b %String provide some useful
 *   implementations of slightly slower versions of some of the methods introduced this class.
 *   It has to be considered case by case which option is the best in respect to performance and
 *   memory use.
 *
 * <b>Constant Nature</b><p>
 * Same as parent class
 * \alib{strings,StringBase,String}, this class is not copying the data of the provided
 * source. It is designed preliminary to offer a fast but convenient (by implicit conversion)
 * type for function parameters that require terminated cstring buffers. As with parent class \b %String,
 * the life-time of objects of this class is considerably short.<br>
 * Only derived class \alib{strings,AStringBase,AString} and its different
 * further specializations will copy the buffer and always include space for the termination
 * character in their buffers' capacity.
 *
 * <b>Parsing Numbers</b><p>
 * This class provides methods for parsing integer and floating point data.
 * For implementation performance reasons, those are <em>not</em> provided by parent class \b %String
 * which does not guarantee zero terminated strings. If parsing has to be done on non-zero
 * terminated string data, consider using a slightly slower variant of the parsing methods which
 * are found in class \alib{strings,SubstringBase,Substring}.<br>
 * It is rather more efficient, to instantiate a local Substring object that just wraps an
 * unterminated string of type \b %String and use its parsing interface, than copying the
 * unterminated data into a terminatable string variant (e.g. \b %AString) and to use the
 * interface provided herein.
 *
 * @tparam TChar    The character type. Implementations for \c char and \c wchar_t are provided
 *                  with type definitions \ref aworx::TString and \ref aworx::TString.
 **************************************************************************************************/
template<typename TChar>
class TStringBase : public StringBase<TChar>
{
    /** ############################################################################################
     * @name Debug methods
     ##@{ ########################################################################################*/
    public:
    #if ALIB_DEBUG_STRINGS
        /** ****************************************************************************************
         * Validates this instance. This method is available only if \ref ALIB_DEBUG_STRINGS
         * is \c true. Invocations to this method should be performed using macro
         * \ref ALIB_STRING_DBG_CHK.
         ******************************************************************************************/
         ALIB_API
         void            _dbgCheck()   const;

        /**
         * This field serves for debugging the development (and potentially the use) of
         * derived classes, eg. class \alib{strings,AStringBase,AString}.
         * It is available only when compiler symbol \ref ALIB_DEBUG_STRINGS is \c true. When active,
         * whenever the contents of an \b %AString instance is modified, the buffer is explicitly
         * "unterminated" by writing a value of '\\1' at the termination position and the state is
         * stored in this field .<br>
         * To keep the constructor <em>constexpr</em>, it is -1 (undetermined) after construction.
         * \see Macro \ref ALIB_STRING_DBG_UNTERMINATE and method #Terminate
         *
         */
        int              debugIsTerminated                                                      =-1;
    #endif


    /** ############################################################################################
     * @name Constructor and Assignment
     ##@{ ########################################################################################*/
    protected:

        /** ****************************************************************************************
         * Constructs this object using the given external buffer and length of content.
         *
         * \note
         *   This constructor is protected to avoid the creation of objects with non-terminatable
         *   types.
         *
         * @param buffer          The buffer to use.
         * @param contentLength   The length of the content in the given buffer.
         ******************************************************************************************/
        constexpr
        inline
        TStringBase( const TChar* buffer, integer contentLength )
        : StringBase<TChar>( buffer, contentLength )
        {}


    public:

        /** ****************************************************************************************
         * Default constructor creating a \e nulled \b %TString.
         ******************************************************************************************/
        constexpr
        inline
        TStringBase()
        : StringBase<TChar>()
        {}

        /** ****************************************************************************************
         * Constructor used with string literals (const character arrays).
         *
         * @param src  The string to represent by this object.
         ******************************************************************************************/
        template <size_t TCapacity>
        inline constexpr
        TStringBase( const TChar (&src) [TCapacity] )
        : StringBase<TChar>(src, TCapacity - 1)
        {}

        /** ****************************************************************************************
         * This constructor overloads the powerful templated constructor
         * \alib{strings::StringBase,StringBase(const TStringLike&)}.
         * In addition to invoking that, it is asserted that the given value is terminated or
         * type \p{TTerminatable} is terminatable. For more information see this classes' general
         * description.
         *
         * @param src  The string to represent by this object.
         ******************************************************************************************/
        template <typename TTerminatable>
        inline
        TStringBase(const  TTerminatable& src )
        : StringBase<TChar>(src)
        {
            ALIB_ASSERT_ERROR(    T_IsTerminatable  < typename std::remove_cv
                                                    < typename std::remove_pointer<TTerminatable>::type>::type>::value
                              ||  StringBase<TChar>::IsNull()
                              ||  StringBase<TChar>::buffer[StringBase<TChar>::length] == '\0'
                              ,ASTR("Error unterminated and non-terminatable string buffer given.") );
        }

    /** ############################################################################################
     * @name Interface
     ##@{ ########################################################################################*/
    public:
        /** ****************************************************************************************
         * Reads a character at a given index.<br> Overrides
         * \alib{strings,StringBase::operator[],String::operator[]} to change the debug assertion
         * to allow inclusion of the termination character.
         *
         * \attention
         *   No parameter check is performed (other than an assertions in debug-compilation of \alib).
         *   See \alib{strings,StringBase::operator[],String::operator[]} for details.
         *
         * @param   op    The index of the character within this objects' buffer.
         * @returns If the character contained at index \p{op}.
         ******************************************************************************************/
         inline
         TChar    operator[] (integer  op) const
         {
            ALIB_ASSERT_ERROR( op >= 0  && op <= StringBase<TChar>::length, ASTR("Index out of bounds") );
            return StringBase<TChar>::buffer[op];
         }

        /** ****************************************************************************************
         * Checks if this objects' buffer is terminated and - if not - terminates it by writing
         * '\0' into the first character of the buffer after the represented string
         * (precisely into <em>buffer[ length ]</em>).
         *
         * \note
         *   When compiler symbol \ref ALIB_DEBUG_STRINGS is \c true, the state of debug field
         *   #debugIsTerminated gets set to '1'.
         *
         *  \see #ToCString and #operator const TChar*().
         ******************************************************************************************/
        inline
        void Terminate()    const
        {
            ALIB_ASSERT_ERROR(  StringBase<TChar>::buffer != nullptr ,ASTR("Can't terminated nulled object.") );

            // Note:
            // The following read may cause memory tools to detect access to uninitialized memory.
            // This is OK and should be ignored/suppressed.
            // For the 'valgrind' tool, add a suppression rule as follows:
            //   {
            //      <TString_terminate>
            //      Memcheck:Cond
            //      fun:*TStXring*Terminate*
            //   }
            //
            // (Tested with valgrind version 3.11)
            // See project folder "tools" for a complete valgrind suppression file for \alib.

            if (StringBase<TChar>::buffer[ StringBase<TChar>::length ] != '\0' )
            {
                StringBase<TChar>::vbuffer[ StringBase<TChar>::length ]= '\0';

                #if ALIB_DEBUG_STRINGS
                    // cast to non-const...hey its for good 8-)
                    const_cast<TStringBase*>(this)->debugIsTerminated= 1;
                #endif
            }
        }

        /** ****************************************************************************************
         * Returns the index of the first character which is included, respectively <em>not</em>
         * included in a given set of characters.
         *
         * This method searches forwards. For backwards search, see
         * \alib{strings,StringBase::LastIndexOfAny,String::LastIndexOfAny}.
         *
         * \note This method reimplements method
         *       \alib{strings,StringBase::IndexOf,String::IndexOf} of parent class.
         *       This implementation however needs a zero-terminatable string for the needles
         *       (beside the fact that this is a zero terminatable string itself).
         *       If no zero-terminatable needles are available, parent method accepting non-zero
         *       terminated needles needs to be invoked. This has to be done by explicitly naming
         *       the parent class in the invocation, e.g. <c>mystring.String::IndexOfAny(...)</c>.<br>
         *       On most platforms, this zero-terminated version should perform faster than
         *       the original method in class \b %String.
         *
         * @param needles    Pointer to a zero terminated set of characters to be taken into
         *                   account.
         * @param startIdx   The index to start the search at. If the given value is less than 0,
         *                   it is set to 0. If it exceeds the length of the string, the length of
         *                   the string is returned.
         *                   Defaults to 0.
         * @tparam TCheck    Defaults to \c true which is the normal invocation mode.
         *                   If \c <false\> is added to the method name, no parameter checks are
         *                   performed and the needles must not be empty.
         * @tparam inclusion Denotes whether the search returns the first index that holds a value
         *                   that is included or that is not excluded in the set of needle
         *                   characters.
         *
         * @return The index of the first character found which is included, respectively not
         *         included, in the given set of characters.
         *         If nothing is found, -1 is returned.
         ******************************************************************************************/
        template <lang::Inclusion   inclusion,
                  bool              TCheck= true >
        inline
        integer  IndexOfAny( const TStringBase& needles, integer startIdx= 0 )
        const
        {
            if (TCheck)
            {
                if ( startIdx < 0                          ) startIdx= 0;
                if ( startIdx >= StringBase<TChar>::length ) return   -1;
            }
            else
            {
                ALIB_ASSERT_ERROR(    startIdx >= 0
                                   && startIdx < StringBase<TChar>::length
                                   && needles.Length() != 0,
                                   ASTR("Non checking and illegal parameters") );
            }


            if ( inclusion == lang::Inclusion::Include )
            {
                integer idx=  CString<TChar>::IndexOfAnyIncludedZT( ToCString() + startIdx, needles.ToCString() );
                return  idx >= 0 ? idx + startIdx : -1;
            }
            else
            {
                const TChar* haystack= ToCString() + startIdx;
                integer idx=  CString<TChar>::IndexOfAnyExcludedZT( haystack, needles.ToCString() ) ;
                return *( haystack + idx ) == '\0' ? -1 : startIdx + idx;
            }
        }

        /** ****************************************************************************************
         * Cast operator to <em>const TChar*</em>. Invokes and returns result of #ToCString.
         *
         * \note Casting an instance with <em>(const TChar*)</em> is the same as invoking
         *       method #ToCString on that instance. The operator is provided as an alias
         *       and it is a matter of taste which of them to use.
         * <p>
         * \note This operator is marked explicit. Although implicit conversion would be quite
         *       convenient, the need for adding the cast explicitly should be helpful in
         *       understanding the structure of the code and leads to more predictable
         *       compiler warnings and invocations of overloaded methods.
         ******************************************************************************************/
        explicit
        inline
        operator const TChar*()
        {
            return ToCString();
        }

        /** ****************************************************************************************
         * This instances' buffer will be terminated (if not already) and returned.
         * If this object is \e nulled, a pointer to an empty cstring is returned.
         * This allows to omit explicit checks for \e nulled objects, in the frequent case that
         * no difference should be made in the processing of a \e nulled or a not \e nulled, but
         * empty object.
         *
         * \note
         *   If used within loops, it is advisable to use a combination of #Terminate (before
         *   the loop) and #Buffer (inside the loop) to avoid the overhead for a repeated
         *   termination check.
         *
         * @return The internal, now zero terminated buffer.
         ******************************************************************************************/
        inline
        const TChar* ToCString()         const
        {
            if ( StringBase<TChar>::buffer == nullptr )
                return  std::is_same<TChar,char>::value ? reinterpret_cast<const TChar*>(  "" )
                                                        : reinterpret_cast<const TChar*>( L"" );
            Terminate();
            return StringBase<TChar>::buffer;
        }

}; // class TStringBase

#if ALIB_DEBUG_STRINGS
extern template ALIB_API void    TStringBase<nchar>::_dbgCheck() const;
extern template ALIB_API void    TStringBase<wchar>::_dbgCheck() const;
#endif


}}} // namespace [aworx::lib::strings]


#if defined(_MSC_VER)
    #pragma warning( pop )
#endif
#endif // HPP_ALIB_STRINGS_TSTRING
