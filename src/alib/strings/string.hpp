// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGS_STRING
#define HPP_ALIB_STRINGS_STRING 1

#if !defined (HPP_ALIB_LIB_ASSERT)
#   include "alib/lib/assert.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_FWDS)
#   include "alib/strings/fwds.hpp"
#endif

#define HPP_ALIB_STRING_PROPPERINCLUDE
#   include "alib/strings/chararraytraits.inl"
#undef  HPP_ALIB_STRING_PROPPERINCLUDE

#if !defined (_GLIBCXX_ITERATOR) && !defined (_ITERATOR_)
#   include <iterator>
#endif


// conditional expression is constant for using our constant template parameters to select
// checking vs. non-checking method versions
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif


namespace aworx { namespace lib { namespace strings {

/** The maximum length of an \alib string. */
constexpr static integer  MAX_LEN = (std::numeric_limits<integer>::max)();

/** ************************************************************************************************
 * This class is the base class of all \ref alib_strings_classes "ALib string classes".
 * Objects of this type represent character strings whose data is allocated outside their scope.
 * In particular, the class does not allocate a character array buffer to store and manipulate
 * string data.
 *
 * Once constructed, objects of this class are immutable, except for the possibility to assign
 * a complete new object value.
 * This means, there is no interface to change the single two data members #buffer and #length.
 * The immutable nature of this type is lifted by derived types. While class
 * \alib{strings,TSubstring,Substring} allows to change the start and
 * length of the string represented, class \alib{strings,TAString,AString} holds a copy of the
 * data and consequently allows to modify the string stored.<br>
 *
 * \see
 *   For an introduction into the \alib string classes see this module's
 *   \ref alib_mod_strings "Programmer's Manual".
 *
 * @tparam TChar The character type.<br>
 *   Alias names for specializations of this class using character types
 *   \alib{characters,character}, \alib{characters,nchar}, \alib{characters,wchar},
 *   \alib{characters,xchar}, \alib{characters,complementChar} and \alib{characters,strangeChar}
 *   are provided in namespace #aworx with type definitions \aworx{String}, \aworx{NString},
 *   \aworx{WString}, \aworx{XString}, \aworx{ComplementString} and \aworx{StrangeString}.
 **************************************************************************************************/
template<typename TChar>
class TString
{
    // #############################################################################################
    // Debug warnings
    // #############################################################################################
    public:
        //! @cond NO_DOX
            #if ALIB_STRINGS_DEBUG
                void     dbgCheck()   const;
            #endif
        //! @endcond

    // #############################################################################################
    // Private fields
    // #############################################################################################
    protected:
    #if !ALIB_DOCUMENTATION_PARSER

         union
         {
             const TChar*   buffer;
                   TChar*  vbuffer;
         };

    #else
            /**
             * Pointer to an array of constant character values. This array holds the string that
             * an instance of this type is representing.<br>
             * Read access to this field is granted with method #Buffer.
             *
             * For technical reasons, this documentation unfortunaltely omits the important fact that
             * this field is part of an anonymous union declared like this:
             *
             *      union
             *      {
             *          const TChar*   buffer;
             *                TChar*  vbuffer;
             *      };
             *
             * Hence, the field can also be interpreted as a pointer to an array of <b>non-constant</b>
             * character values.
             * Derived classes might use the sibling version \b vbuffer to modify the contents of
             * the string if it is assured that such memory is writable.
             * This is for example done extensively by the implementation of class
             * \alib{strings,TAString,AString}.
             *
             * \note
             *   It might not be considered pure/nice OO-design to prepare a feature of specialized
             *   classes in a non-abstract base class, as done here. But the alternative would have
             *   been to force derived classes to perform re-interpret casts or even worse tricks
             *   to rightfully access a writeable buffer.
             */
             const TChar*   buffer;

    #endif

        /**
         * The length of the character string represented.
         * Read access to this field is granted with method #Length.
         */
        integer         length;

    /** ############################################################################################
     * @name Constructors
     ##@{ ########################################################################################*/
     public:

        /** ****************************************************************************************
         * Default constructor creating a \ref alib_strings_details_nulled \e "nulled" string.
         ******************************************************************************************/
        inline
        constexpr TString()
        : buffer(nullptr)
        , length(0)
        {}

        /** ****************************************************************************************
         * Constructor accepting a pointer to a character array and a string length.
         *
         * @param pBuffer   Pointer to the start of the character string to represent.
         * @param pLength   The length of the character string to represent.
         ******************************************************************************************/
        inline
        constexpr
        TString( const TChar* pBuffer, integer pLength )
        : buffer(pBuffer)
        , length(pLength)
        {}

    #if ALIB_DOCUMENTATION_PARSER
        /** ****************************************************************************************
         * This templated constructor accepts various different kinds of source data.
         * Unlike this documentation suggests, this constructor is internally implemented by a
         * series of different constructors which are selected using template meta programming
         * (i.e. \c std::enable_if).
         *
         * Together, the set of constructors provide maximum flexibility by allowing implicit
         * construction with (and assignment of) any built-in or third-party character array type.
         * Some of the constructors are defined using keyword \c explict.
         *
         * \see
         *   More information about string construction is provided with chapter
         *   \ref alib_strings_cc_construction of the Programmer's Manual of module
         *   \alibmod_strings.
         *
         * @tparam TCharArray  Type that comprises a character array.
         * @param  src        The source object.
         ******************************************************************************************/
        template <typename TCharArray>
        inline
        constexpr
        TString(const  TCharArray& src );

        /** ****************************************************************************************
         * \b Implicit cast operator to objects of templated type \p{TCharArray}.<br>
         * This operator is available for all custom types that have an accordingly specialized
         * version of TMP struct \alib{characters,T_CharArray} defined.
         *
         * \see
         *   More information about casting \alib string types to built-in C++ types or custom
         *   types is provided with chapter \ref alib_strings_cc_cast of the programmer's
         *   manual of module \alibmod_strings.
         *
         * @tparam TCharArray The custom type to implicitly convert this object to.
         * @return A value of custom string type.
         ******************************************************************************************/
        template<typename TCharArray>
        inline
        operator TCharArray () const
        {}

        /** ****************************************************************************************
         * \b Explicit cast operator to objects of templated  type \p{TCharArray}.<br>
         * This operator is available for all custom types that have an accordingly specialized
         * version of TMP struct \alib{characters,T_CharArray} defined.
         *
         * \see
         *   More information about casting \alib string types to built-in C++ types or custom
         *   types is provided with chapter \ref alib_strings_cc_cast of the programmer's
         *   manual of module \alibmod_strings.
         *
         * @tparam TCharArray The custom type to explicitly convert this object to.
         * @return A value of custom string type.
         ******************************************************************************************/
        template<typename TCharArray>
        inline explicit
        operator TCharArray () const
        {}

    #else // doxygen end

        ATMP_SELECT_IF_1TP( typename T,
            std::is_same<std::nullptr_t,T>::value )
        constexpr
        TString(const T&)
        : buffer( nullptr )
        , length( 0       )
        {}

        ATMP_SELECT_IF_1TP( typename T,
            characters::T_CharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::Implicit )
        constexpr
        TString(const T& src)
        : buffer( characters::T_CharArray<ATMP_RCV(T),TChar>::Buffer( src ) )
        , length( characters::T_CharArray<ATMP_RCV(T),TChar>::Length( src ) )
        {}

        ATMP_SELECT_IF_1TP( typename T,
            characters::T_CharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::ExplicitOnly )
        constexpr
        explicit
        TString(const T& src)
        : buffer( characters::T_CharArray<ATMP_RCV(T),TChar>::Buffer( src ) )
        , length( characters::T_CharArray<ATMP_RCV(T),TChar>::Length( src ) )
        {}

        ATMP_SELECT_IF_1TP( typename T,
            characters::T_CharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::MutableOnly && !std::is_const<T>::value )
        constexpr
        explicit
        TString(      T& src)
        : buffer( characters::T_CharArray<ATMP_RCV(T),TChar>::Buffer( const_cast<T&>( src ) ) )
        , length( characters::T_CharArray<ATMP_RCV(T),TChar>::Length( const_cast<T&>( src ) ) )
        {}

        // ##############################    casting  back    ######################################
        ATMP_SELECT_IF_1TP( typename T,
            characters::T_CharArray<T,TChar>::Construction == characters::ConstructionType::Implicit
        && !T_SuppressAutoCast<TString<TChar>,characters::ConstructionType::Implicit    ,ATMP_RCV(T)>::value)
        constexpr
        operator T () const
        {
            return characters::T_CharArray<T,TChar>::Construct( buffer, length );
        }

        ATMP_SELECT_IF_1TP( typename T,
            characters::T_CharArray<T,TChar>::Construction == characters::ConstructionType::ExplicitOnly
        && !T_SuppressAutoCast<TString<TChar>,characters::ConstructionType::ExplicitOnly,ATMP_RCV(T)>::value)
        constexpr
        explicit
        operator T () const
        {
            return characters::T_CharArray<T,TChar>::Construct( buffer, length );
        }

    #endif // doxygen

ALIB_WARNINGS_IGNORE_IF_CONSTEXPR

        /** ****************************************************************************************
         * Returns a new string object representing a sub-string of the string that this object
         * represents.
         *
         * @param  regionStart  The start of the sub-string within this string.
         * @param  regionLength The length of the sub-string to return.
         *                      Defaults to \alib{strings,MAX_LEN}.
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c false is given, no range check is performed.
         * @return A string representing a region of this string.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        TString<TChar>   Substring(integer regionStart, integer regionLength =MAX_LEN )        const
        {
            ALIB_STRING_DBG_CHK(this)

            if ALIB_CPP17_CONSTEXPR ( TCheck )
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
                                       "Non checking and region out of range or empty" )
                #endif
            }

            return TString<TChar>( buffer + regionStart, regionLength );
        }


    /** ############################################################################################
     * @name Buffer Access, Length and State
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Returns a pointer to the first character of the string we are representing.
         * \note The string is not guaranteed to be zero terminated.
         *
         * @return The start of the character array comprising the string represented by this
         *         object.
         ******************************************************************************************/
        inline
        constexpr const TChar*  Buffer()          const       { return buffer;    }

        /** ****************************************************************************************
         * Returns the length of the string that this object represents.
         *
         * @return The length of the string represented by this object.
         ******************************************************************************************/
        inline
        constexpr integer       Length()          const       { return length;    }

        /** ****************************************************************************************
         * Returns the length of the string if represented as a wide character string.
         * If template parameter \p{TChar} equals \c wchar, then this is identical with #Length.
         * Otherwise the calculation is done using
         * - <em>mbsnrtowcs()</em>
         *   (without providing a conversion buffer) on glibc platforms (e.g. Linux)
         * - <em>MultiByteToWideChar()</em>
         *   (without providing a conversion buffer) on the Windows platform.
         *
         * If the conversion fails, \c -1 is returned.
         *
         * \note
         *   On GNU/Linux and Mac OS, it might be necessary to invoke standard C method
         *   <em>setlocale()</em> once, prior to using this method, to successfully calculate
         *   the length.
         *   This by default is done during \ref alib_manual_bootstrapping "library initialization",
         *   if performed on class \aworx{lib,ALibModules}.
         *
         * @return  The length of string when it was converted to wide characters.
         *          If counting failed (what means that the conversion will fail) \c -1 is returned.
         ******************************************************************************************/
        integer       WStringLength()   const;


        /** ****************************************************************************************
         * Returns \c true if field #buffer equals \c nullptr, \c false otherwise.
         * Note that a \e nulled string is also considered \ref IsEmpty "empty".
         *
         * \see
         *   Details on the concept of \e nulled and \e empty strings are documented in chapter
         *   \ref alib_strings_details_nulled_vsempty of this module's
         *   \ref alib_mod_strings "Programmer's Manual".
         *
         * @return \c true if no buffer is allocated.
         ******************************************************************************************/
        inline
        bool         IsNull()           const       { return buffer == nullptr; }

        /** ****************************************************************************************
         * Returns \c true if field #buffer does not equal \c nullptr, \c false otherwise.
         *
         * @return The negated value of method #IsNull.
         ******************************************************************************************/
        inline
        bool         IsNotNull()        const       { return buffer != nullptr; }

        /** ****************************************************************************************
         * Returns \c true if this string is of zero length.
         * Note that a \e nulled string is also considered empty.
         *
         * \see
         *   Details on the concept of \e nulled and \e empty strings are documented in chapter
         *   \ref alib_strings_details_nulled_vsempty of this module's
         *   \ref alib_mod_strings "Programmer's Manual".
         * @return \c true if the actual length equals zero.
         ******************************************************************************************/
        inline
        bool         IsEmpty()          const       { return length == 0; }

        /** ****************************************************************************************
         * Returns \c true if this string has a length of \c 1 or more.
         * @return \c true if the actual length does not equal zero.
         ******************************************************************************************/
        inline
        bool         IsNotEmpty()       const       { return length != 0; }


    /** ############################################################################################
     * @name Character access
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Retrieves the character at the given index. A range check is performed. If this fails,
         * \c '\0' is returned.
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
            if ALIB_CPP17_CONSTEXPR (TCheck)
                return  ( idx >= 0 && idx < length ) ? *(buffer + idx )
                                                     : '\0' ;
            ALIB_ASSERT_ERROR( idx >= 0 && idx < length, "Non checking version: Index out of range" )
            return  *(buffer + idx );
        }

        /** ****************************************************************************************
         * Retrieves the first character. In case of an empty or \e nulled string, '\0' is returned.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c false is given, no check for an empty string object is performed.
         * @return The first character of the %String.
         *         If this instance's length is zero,  '\0' is returned.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        TChar        CharAtStart()          const
        {
            if ALIB_CPP17_CONSTEXPR (TCheck)
                return length > 0  ?  *(buffer)
                                   :  '\0';

            ALIB_ASSERT_ERROR(  length > 0, "Non checking invocation on empty string" )
            return  *(buffer);
        }


        /** ****************************************************************************************
         * Retrieves the last character. In case of an empty string, '\0' is returned.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c false is given, no check for an empty or \e nulled object is
         *                 performed.
         *
         * @return The last character of the %String.
         *         If this instance's length is zero,  '\0' is returned.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        TChar        CharAtEnd()          const
        {
            if ALIB_CPP17_CONSTEXPR (TCheck)
                return length > 0   ?  *(buffer + length - 1)
                                    : '\0';

            ALIB_ASSERT_ERROR( length > 0, "Non checking invocation on empty string" )
            return  *(buffer + length - 1);
        }

        /** ****************************************************************************************
         * Reads a character at a given index.
         *
         * \attention
         *   Unlike method #CharAt, this operator does <em>not</em> perform do range check on
         *   parameter \p{idx}.
         *   The rationale for this is that in derived class %AString, which overrides this operator
         *   but returning a reference to the character (that provides write access), such reference
         *   to a character could not be given if the index was out of range.
         *   This way, a check in the derived type could not be implemented.<br>
         *   Consequently, this operator is equivalent to the non-checking version of method
         *   #CharAt<\c false>. For safe access to characters in the buffer use #CharAt
         *   (with template parameter \p{TCheck} being \c true) which returns <c>'\0'</c> in the
         *   case of that \p{idx} is out of bounds.<br>
         *   In debug-compilations this operator raises an assertion if \p{idx} is out of bounds.
         *
         * @param   idx    The index of the character within this object's buffer.
         * @returns If the character contained at index \p{idx}.
         ******************************************************************************************/
         inline
         TChar    operator[] (integer idx) const
         {
            ALIB_ASSERT_ERROR( idx >= 0  && idx < length, "Index out of bounds" )
            return buffer[idx];
         }

    /** ############################################################################################
     * @name Comparison
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Shortcut to method \ref #Equals "Equals<Case::Sensitive>( rhs )".
         *
         * @tparam TCharArray  A type that
         *                     \ref alib_strings_cc "constructs String objects implicitly".
         * @param rhs  The object to compare this string with.
         * @returns The result of the call to #Equals.
         ******************************************************************************************/
        template<typename TCharArray> inline  bool  operator== (const TCharArray& rhs)         const
        {
            return  Equals<Case::Sensitive>( rhs );
        }

        /** ****************************************************************************************
         * Shortcut to method \ref #Equals "Equals<Case::Sensitive>( rhs )".
         *
         * @tparam TCharArray  A type that
         *                     \ref alib_strings_cc "constructs String objects implicitly".
         * @param rhs  The object to compare this string with.
         * @returns The negated result of the call to #Equals.
         ******************************************************************************************/
        template<typename TCharArray> inline  bool  operator!= (const TCharArray& rhs)         const
        {
            return !Equals<Case::Sensitive>( rhs );
        }

        /** ****************************************************************************************
         * Invokes  \ref #CompareTo "CompareTo<true, Case::Sensitive>( rhs )" and tests the result
         * to be negative.
         *
         * @tparam TCharArray  A type that
         *                    \ref alib_strings_cc "constructs String objects implicitly".
         * @param rhs  The object to compare this string with.
         * @returns \c true if the call to #CompareTo returned a negative result, \c false
         *          otherwise.
         ******************************************************************************************/
        template<typename TCharArray> inline  bool  operator<  (const TCharArray& rhs)         const
        {
            return CompareTo<true, Case::Sensitive>( rhs ) <  0;
        }

        /** ****************************************************************************************
         * Invokes  \ref #CompareTo "CompareTo<true, Case::Sensitive>( rhs )" and tests the result
         * to be negative or zero.
         *
         * @tparam TCharArray  A type that
         *                    \ref alib_strings_cc "constructs String objects implicitly".
         * @param rhs  The object to compare this string with.
         * @returns \c true if the call to #CompareTo returned a negative or zero result, \c false
         *          otherwise.
         ******************************************************************************************/
        template<typename TCharArray> inline  bool  operator<= (const TCharArray& rhs)         const
        {
            return CompareTo<true, Case::Sensitive>( rhs ) <= 0;
        }

        /** ****************************************************************************************
         * Invokes  \ref #CompareTo "CompareTo<true, Case::Sensitive>( rhs )" and tests the result
         * to be positive excluding zero.
         *
         * @tparam TCharArray  A type that
         *                    \ref alib_strings_cc "constructs String objects implicitly".
         * @param rhs  The object to compare this string with.
         * @returns \c true if the call to #CompareTo returned a positive but non-zero result,
         *          \c false otherwise.
         ******************************************************************************************/
        template<typename TCharArray> inline  bool  operator>  (const TCharArray& rhs)         const
        {
            return CompareTo<true, Case::Sensitive>( rhs ) >  0;
        }

        /** ****************************************************************************************
         * Invokes  \ref #CompareTo "CompareTo<true, Case::Sensitive>( rhs )" and tests the result
         * to be positive (including zero).
         *
         * @tparam TCharArray  A type that
         *                    \ref alib_strings_cc "constructs String objects implicitly".
         * @param rhs  The object to compare this string with.
         * @returns \c true if the call to #CompareTo returned a positive result, \c false
         *          otherwise.
         ******************************************************************************************/
        template<typename TCharArray> inline  bool  operator>= (const TCharArray& rhs)         const
        {
            return CompareTo<true, Case::Sensitive>( rhs ) >=  0;
        }

        /** ****************************************************************************************
         * Compares this string with a
         * \ref alib_strings_cc_construction_string "string-like object".
         *
         * \c true is returned if this and the compared string are \e nulled or empty.
         * If only one is \e nulled or empty, \c false is returned.
         *
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         * @param  rhs          The object to compare.
         *
         * @return  \c true, if contents of this string and the string representation of the
         *          given \p{rhs} are equal.
         ******************************************************************************************/
        template <Case TSensitivity =Case::Sensitive>
        inline
        bool Equals( const TString<TChar>& rhs )                                            const
        {
            ALIB_STRING_DBG_CHK(this)
            if (    ( IsNull() !=  rhs.IsNull() )
                 || ( length   !=  rhs.length   )    )
                return false;

            if ( length == 0 )
                return true;

            if ALIB_CPP17_CONSTEXPR (TSensitivity == Case::Sensitive )
                return  characters::CharArray<TChar>::Equal            ( buffer, rhs.buffer, length );
            else
                return  characters::CharArray<TChar>::CompareIgnoreCase( buffer, rhs.buffer, length ) == 0;
        }

        /** ****************************************************************************************
         * Compares this string with a
         * \ref alib_strings_cc_construction_string "string-like object".
         *
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c false is given, no check for a \e nulled object (this) is
         *                      performed and this string must not be of zero length
         *                      (while \p{rhs} might be of zero length).
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         * @param  rhs          The object to compare.
         *
         * @return
         *  -  0 if this and \p{rhs} are \e nulled or if both have a length of 0 or if both
         *       share the same content
         *  - <0 if this is \e nulled and \p{rhs} is not or if this is smaller than \p{rhs}.
         *  - >0 if this is not \e nulled but \p{rhs} is or if this is greater than \p{rhs}.
         ******************************************************************************************/
        template <bool TCheck       = true,
                  Case TSensitivity = Case::Sensitive>
        inline
        int CompareTo( const TString<TChar>& rhs )                                          const
        {
            ALIB_STRING_DBG_CHK(this)

            // check \c nullptr arguments
            if (TCheck &&     IsNull() )  return  rhs.IsNull() ? 0 : -1;
            if (TCheck && rhs.IsNull() )  return  +1;

            // zero length ?
            if ( TCheck && length == 0 )  return  rhs.length == 0 ? 0 : -1;
            if (       rhs.length == 0 )  return +1;

            bool    iAmShorter=  (length < rhs.length);
            integer shortLen  =  iAmShorter ?  length  :  rhs.length;

            int cmpVal=  (TSensitivity == Case::Sensitive)
                         ? characters::CharArray<TChar>::Compare          ( buffer, rhs.buffer, shortLen )
                         : characters::CharArray<TChar>::CompareIgnoreCase( buffer, rhs.buffer, shortLen );

            if ( cmpVal != 0 || length == rhs.length )
                return cmpVal;
            return iAmShorter ? -1 : 1;
        }

        /** ****************************************************************************************
         * Compares this string with a region of another
         * \ref alib_strings_cc_construction_string "string-like object".
         *
         * @param rhs             The string to compare this string with.
         * @param rhsRegionStart  The start of the region in \p{rhs} to compare this object
         *                        with.
         * @param rhsRegionLength The length of the region in \p{rhs} to compare this object
         *                        with.
         *                        Defaults to \alib{strings,MAX_LEN}.
         * @tparam TCheck         Defaults to \c true which is the normal invocation mode.
         *                        If \c false is given, no check for a \e nulled comparison
         *                        object is performed and this string must not be empty.
         *                        Furthermore, no check is performed whether the given region
         *                        fits to parameter \p{rhs}. This also means that the default
         *                        value must not be used with <em>TCheck==\<\c false\></em>.
         * @tparam TSensitivity   Determines if comparison is case sensitive (the default) or not.
         *
         * @return
         *  -  0 if this and \p{rhs} are \e nulled or if both have a length of 0 or if both
         *       share the same content
         *  - <0 if this is \e nulled and \p{rhs} is not or if this is smaller than \p{rhs}.
         *  - >0 if this is not \e nulled but \p{rhs} is or if this is greater than \p{rhs}.
         ******************************************************************************************/
        template < bool TCheck       = true,
                   Case TSensitivity = Case::Sensitive>
        inline
        int CompareTo(  const TString&  rhs,
                        integer            rhsRegionStart,
                        integer            rhsRegionLength   =MAX_LEN  )                       const
        {
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                TString cmpSub( rhs.buffer, 0);
                rhs.AdjustRegion( rhsRegionStart, rhsRegionLength );
                cmpSub.buffer+=   rhsRegionStart;
                cmpSub.length=    rhsRegionLength;

                return CompareTo<true,  TSensitivity>( cmpSub );
            }
            else
                return CompareTo<false, TSensitivity>( TString( rhs.buffer + rhsRegionStart,
                                                                   rhsRegionLength ) );
        }

        /** ****************************************************************************************
         * Compares a region of this object with a region of another
         * \ref alib_strings_cc_construction_string "string-like object".
         *
         * @param rhs             The string to compare this string with.
         * @param rhsRegionStart  The start of the region in \p{rhs} to compare this object
         *                        with.
         * @param rhsRegionLength The length of the region in \p{rhs} to compare this object
         *                        with.
         * @param regionStart     The start of the region in this object to compare with
         * @param regionLength    The length of the region in this object to compare with.
         *                        Defaults to \alib{strings,MAX_LEN}.
         * @tparam TCheck         Defaults to \c true which is the normal invocation mode.
         *                        If \c false is given, no check for a \e nulled comparison
         *                        object is performed and this string must not be empty.
         *                        Furthermore, no check is performed whether the given regions fit
         *                        to this object respectively the other region to the object given
         *                        with parameter \p{rhs}.
         *                        This also means that the default value of \p{regionLength} must
         *                        not be used in this case.
         * @tparam TSensitivity   Determines if comparison is case sensitive (the default) or not.
         *
         * @return
         *  -  0 if this and \p{rhs} are \e nulled or if both have a length of 0 or if both
         *       share the same content
         *  - <0 if this is \e nulled and \p{rhs} is not or if this is smaller than \p{rhs}.
         *  - >0 if this is not \e nulled but \p{rhs} is or if this is greater than \p{rhs}.
         ******************************************************************************************/
        template < bool TCheck       = true,
                   Case TSensitivity = Case::Sensitive>
        inline
        int CompareTo(  const TString&  rhs,
                        integer            rhsRegionStart,
                        integer            rhsRegionLength,
                        integer            regionStart,
                        integer            regionLength      =MAX_LEN  )                       const
        {
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                TString cmpSub( rhs.buffer, 0);
                rhs.AdjustRegion( rhsRegionStart, rhsRegionLength );
                cmpSub.buffer+=   rhsRegionStart;
                cmpSub.length=    rhsRegionLength;

                AdjustRegion( regionStart, regionLength );
                return TString( buffer + regionStart, regionLength ).CompareTo<true, TSensitivity>( cmpSub );
            }

            return TString( buffer + regionStart, regionLength )
                    .CompareTo<false, TSensitivity>( TString( rhs.buffer + rhsRegionStart,
                                                              rhsRegionLength )               );
        }

        /** ****************************************************************************************
         * Returns \c true, if the contents of the given
         * \ref alib_strings_cc_construction_string "string-like object" is found at the given
         * position.
         *
         * \note
         *   The following rules apply:
         *   - If \p{pos} is out of range or \p{needle} is \e nulled, \c false is returned.
         *     (This check only done if \p{TCheck} equals \c true.)
         *   - Otherwise, if the length of \p{needle} is 0, \c true is returned.
         *
         * @param needle        The string to compare with. If it is \ref IsEmpty "empty", \c true
         *                      is returned.
         * @param pos           The position to search for needle.
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c <false\> is given, no check on parameter
         *                      \p{pos} is performed and \p{needle} must not be \e nulled.
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         * @return \c true if \p{needle} is found at the given position. False otherwise.
         ******************************************************************************************/
        template< bool  TCheck       = true,
                  Case  TSensitivity = Case::Sensitive >
        inline
        bool ContainsAt( const TString& needle, integer pos )                               const
        {
            integer needleLength= needle.length;
            ALIB_STRING_DBG_CHK(this)
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                if ( pos < 0 || pos + needleLength > length || needle.IsNull () )
                    return false;
                if ( needleLength == 0 )
                    return true;
            }
            else
            {
                ALIB_ASSERT_ERROR( pos >= 0 && pos + needleLength <= length && !needle.IsNull(),
                                   "Non checking and index out of range" )
                ALIB_ASSERT_ERROR( needleLength != 0,
                                   "Non checking and emtpy compare string" )
            }

            return TSensitivity == Case::Sensitive
                   ? characters::CharArray<TChar>::Equal            ( buffer + pos,  needle.buffer, needleLength )
                   : characters::CharArray<TChar>::CompareIgnoreCase( buffer + pos,  needle.buffer, needleLength ) == 0 ;
        }

        /** ****************************************************************************************
         * Returns \c true, if this string starts with the contents of the
         * \ref alib_strings_cc_construction_string "string-like object" given with parameter
         * \p{needle}.
         * In the special case that \p{needle} is \ref IsEmpty "empty", \c true is returned.
         *
         * @param needle        The string to compare the start of this string with.
         *                      If \e nulled or empty, \c true is returned.
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         * @tparam TCheck       Defaults to \c true which is the normal invocation mode.
         *                      If \c <false\> is given, the given needle must not be empty
         *                      and must not be longer than this string!
         *                      \p{pos} is performed and \p{needle} must not be \e nulled.
         * @return \c true if \p{needle} is found at the start of this string, \c false otherwise.
         ******************************************************************************************/
        template<bool  TCheck      = true,
                 Case  TSensitivity =Case::Sensitive>
        inline
        bool StartsWith( const TString& needle )                                            const
        {
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                if ( needle.length > length )
                    return false;
                if ( needle.length == 0 )
                    return true;
            }
            else
            {
                ALIB_ASSERT_ERROR( needle.length <= length,
                                   "Non checking and needle longer than this string." )
                ALIB_ASSERT_ERROR( needle.length != 0,
                                   "Non checking and emtpy needle given." )
            }

            if ALIB_CPP17_CONSTEXPR ( TSensitivity == Case::Sensitive )
                return  characters::CharArray<TChar>::Equal            ( buffer,  needle.buffer, needle.length );
            else
                return  characters::CharArray<TChar>::CompareIgnoreCase( buffer,  needle.buffer, needle.length ) == 0;
        }

        /** ****************************************************************************************
         * Returns \c true, if this string ends with the string found in parameter \p{needle}.
         * If \p{needle} is \ref IsEmpty "empty", \c true is returned.
         *
         * @param needle        The string to compare the end of this string with.
         *                      If \e nulled or empty, \c true is returned.
         * @tparam TSensitivity Determines if comparison is case sensitive (the default) or not.
         * @return \c true if \p{needle} is found at the end of this, \c false otherwise.
         ******************************************************************************************/
        template<bool        TCheck      = true,
                 Case  TSensitivity =Case::Sensitive>
        inline
        bool EndsWith( const TString& needle )                                              const
        {
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                if ( needle.length > length )
                    return false;
                if ( needle.length == 0 )
                    return true;
            }
            else
            {
                ALIB_ASSERT_ERROR( needle.length <= length,
                                   "Non checking and needle longer than this string." )
                ALIB_ASSERT_ERROR( needle.length != 0,
                                   "Non checking and emtpy needle given." )
            }

            if ALIB_CPP17_CONSTEXPR ( TSensitivity == Case::Sensitive )
                return characters::CharArray<TChar>::Equal            ( buffer + length - needle.length,  needle.buffer, needle.length );
            else
                return characters::CharArray<TChar>::CompareIgnoreCase( buffer + length - needle.length,  needle.buffer, needle.length ) == 0;
        }

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
         * @return  \c -1 if the character \p{needle} is not found.
         *          Otherwise the index of its first occurrence.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer  IndexOf( TChar needle, integer startIdx = 0 )                                 const
        {
            ALIB_STRING_DBG_CHK(this)

            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( startIdx <  0      )  startIdx= 0;
                else if ( startIdx >= length )  return -1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length,
                                   "Non checking and index out of range" )
            }

            const TChar* result=  characters::CharArray<TChar>::Search( buffer + startIdx, length - startIdx, needle );

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
         * @return  \c -1 if the character \p{needle} is not found.
         *          Otherwise the index of its first occurrence.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer         IndexOf( TChar needle, integer regionStart, integer regionLength )    const
        {
            ALIB_STRING_DBG_CHK(this)

            if ALIB_CPP17_CONSTEXPR ( TCheck )
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
                                       "Non checking and region out of range or empty" )
                #endif
            }

            const TChar* result=  characters::CharArray<TChar>::Search( buffer + regionStart, regionLength, needle );

            return result != nullptr ? result  -  buffer
                                     : -1;
        }

        /** ****************************************************************************************
         * Like \alib{strings::TString,IndexOf} but in case the character is not found, this
         * method returns the length of this string instead of \c -1.
         * Depending on the invocation context, the choice for the right version of this method may
         * lead to shorter and more efficient code.
         *
         * @param needle  The character to search for.
         * @return  This string's #Length if character \p{needle} is not found.
         *          Otherwise the index of first occurrence.
         ******************************************************************************************/
        inline
        integer      IndexOfOrLength( TChar needle )                                           const
        {
            ALIB_STRING_DBG_CHK(this)
            const TChar* result=   characters::CharArray<TChar>::Search( buffer, length, needle );

            return result != nullptr ? result  -  buffer
                                     : length;
        }

        /** ****************************************************************************************
         * Like \alib{strings::TString,IndexOf} but in case the character is not found, this
         * method returns the length of this string instead of \c -1.
         * Depending on the invocation context, the choice for the right version of this method may
         * lead to shorter and more efficient code.
         *
         * @param needle    The character to search for.
         * @param startIdx  The index in this to start searching the character.
         * @tparam TCheck   Defaults to \c true which is the normal invocation mode.
         *                  If \c false is given, no range check is performed.
         * @return  This string's #Length if  character \p{needle} is not found.
         *          Otherwise the index of first occurrence.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer      IndexOfOrLength( TChar needle, integer startIdx )                         const
        {
            ALIB_STRING_DBG_CHK(this)
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( startIdx <  0      )  startIdx= 0;
                else if ( startIdx >= length )  return length;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length,
                                 "Non checking and index out of range" )
            }

            const TChar* result= characters::CharArray<TChar>::Search( buffer + startIdx, length - startIdx, needle );
            return result != nullptr ? result  -  buffer
                                     : length;
        }

        /** ****************************************************************************************
         * Searches a character starting backwards from the end or a given start index.
         *
         * @tparam TCheck      Defaults to \c true which is the normal invocation mode.
         *                     If \c false is given, no range check is performed.
         *                     Consequently, in this case, optional parameter startIndex must be
         *                     provided.
         *
         * @param needle       The character to search for.
         * @param startIndex   The index within this string to start searching the character.
         *                     Defaults to \alib{strings,MAX_LEN}.
         *
         * @return  \c -1 if the character \p{needle} is not found.
         *          Otherwise the index of its last occurrence.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer    LastIndexOf( TChar needle, integer startIndex =MAX_LEN )                    const
        {
            ALIB_STRING_DBG_CHK(this)

            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                // adjust range, if empty return -1
                if ( startIndex <  0      )   return -1;
                if ( startIndex >= length )   startIndex= length-1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIndex >= 0 && startIndex < length,
                                  "Non checking and index out of range"   )
            }

            while( startIndex >= 0 && buffer[ startIndex ] != needle )
                startIndex--;

            return startIndex;
        }

        /** ****************************************************************************************
         * Returns the index of the first character which is included, respectively <em>not</em>
         * included in a set of characters given as a
         * \ref alib_strings_cc_construction_string "string-like object".
         *
         * \note
         *   In derived class \b %CString, a faster version of this method (using \c std::strpbrk()
         *   respectively \c std::strspn()) is available.
         *   So, if performance is important and repetitive calls are performed, it might be
         *   advisable to hold this string and the needles in a zero-terminated string buffer,
         *   for example in an \b AString.
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
        template <Inclusion   TInclusion,
                  bool        TCheck      = true>
        inline
        integer    IndexOfAny( const TString& needles, integer startIdx= 0 )                const
        {
            if ALIB_CPP17_CONSTEXPR (TCheck)
            {
                if ( startIdx < 0       ) startIdx= 0;
                if ( startIdx >= length ) return   -1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length && needles.Length() != 0,
                                   "Non checking and illegal parameters" )
            }


            integer idx= TInclusion == Inclusion::Include
              ? characters::CharArray<TChar>::IndexOfAnyIncluded( buffer + startIdx,  length - startIdx, needles.Buffer(), needles.Length() )
              : characters::CharArray<TChar>::IndexOfAnyExcluded( buffer + startIdx,  length - startIdx, needles.Buffer(), needles.Length() );

            return idx == -1 ? -1 : startIdx + idx;
        }

        /** ****************************************************************************************
         * Returns the index of the last character which is included, respectively <em>not</em>
         * included in set of characters given as a
         * \ref alib_strings_cc_construction_string "string-like object".
         *
         * This method searches backwards starting at the given index. For forwards search, see
         * #IndexOfAny.
         *
         * @param needles     Pointer to a zero terminated set of characters to be searched for.
         * @param startIdx    The index to start the search at. The value is cropped to be in
         *                    the bounds of 0 and the length of this object minus one.
         *                    Defaults to \alib{strings,MAX_LEN}.
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
        template <Inclusion   TInclusion,
                  bool        TCheck = true>
        inline
        integer LastIndexOfAny( const TString& needles, integer startIdx =MAX_LEN )            const
        {
            if ALIB_CPP17_CONSTEXPR (TCheck)
            {
                if ( startIdx < 0       ) return -1;
                if ( startIdx >= length ) startIdx=  length - 1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length && needles.Length() != 0,
                                   "Non checking and illegal parameters" )
            }

            if ALIB_CPP17_CONSTEXPR ( TInclusion == Inclusion::Include )
                return characters::CharArray<TChar>::LastIndexOfAnyInclude(  Buffer(), startIdx, needles.Buffer(), needles.Length() );
            else
                return characters::CharArray<TChar>::LastIndexOfAnyExclude(  Buffer(), startIdx, needles.Buffer(), needles.Length() );
        }

        /** ****************************************************************************************
         * Searches the given \ref alib_strings_cc_construction_string "string-like object"
         * in this string.
         *
         * If \p{needle} is empty, the adjusted value of \p{startIdx} is returned.
         *
         * @param  needle         The string to search for.
         * @param  startIdx       The index to start the search at. Optional and defaults to \c 0.
         * @tparam TSensitivity   Case sensitivity of the comparison.
         *                        Optional and defaults to \b Case::Sensitive.
         * @tparam TCheck         Defaults to \c true which is the normal invocation mode.
         *                        If \c false is given, parameter \p{needle} must not be empty and
         *                        \p{startIdx} must be in the range of [0 ... #Length() - needle.Length()].
         *                        This also implies that this string must not be empty.
         *
         *
         * @return    If the string is not found or the checking of parameters failed, \c -1 is
         *            returned. Otherwise the index of the first occurrence of \p{needle}.
         ******************************************************************************************/
        template<bool     TCheck        = true,
                 Case     TSensitivity  = Case::Sensitive>
        inline
        integer  IndexOf( const TString&  needle,
                          integer         startIdx= 0  )                                       const
        {
            if ALIB_CPP17_CONSTEXPR (TCheck)
            {
                if ( needle.IsNull()  )
                    return  -1;
                if ( startIdx < 0                        )    startIdx= 0;
                if ( startIdx + needle.Length() > length )    return -1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx <= length && needle.IsNotNull(),
                                   "Non checking and illegal parameters" )
            }


            return indexOfString<TSensitivity>(needle, startIdx);
        }

        /** ****************************************************************************************
         * Searches the first difference of a sub-string of this string and a
         * \ref alib_strings_cc_construction_string "string-like object" given with parameter
         * \p{needle}.
         * If no difference is found, then the index of the first character behind the sub-string
         * is returned.
         *
         * @tparam TCheck      Defaults to \c true which is the normal invocation mode.
         *                     If \c false is given, no range check is performed.
         * @param needle       The sub-string to search for.
         * @param sensitivity  Letter case sensitivity of the comparison.
         *                     Optional and defaults to \b Case::Sensitive.
         * @param idx          The index in this string to start the comparison with \p{needle}.
         *                     Optional and defaults to \c 0.
         *
         * @return The index of the first difference found or \p{idx} plus the length of \p{needle}.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer  IndexOfFirstDifference( const TString&  needle,
                                         Case     sensitivity = Case::Sensitive,
                                         integer  idx         = 0                     )  const
        {
            ALIB_STRING_DBG_CHK(this)

            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( idx <  0      )  idx= 0;
                else if ( idx >= length )  return idx;
            }
            else
            {
                ALIB_ASSERT_ERROR( idx >= 0 && idx < length,
                                   "Non checking and index out of range" )
            }

            return   characters::CharArray<TChar>::IndexOfFirstDifference( buffer + idx,   length - idx,
                                                                           needle.buffer,  needle.length,
                                                                           sensitivity                       );
        }

        /** ****************************************************************************************
         * The method searches the next matching \p{closer}-character while taking nested pairs of
         * \p{opener} and \p{closer} characters into account.
         *
         * Prior to the invocation of this method, the initial \p{opener} has to be known already
         * and the given \p{idx} has to point to the first character behind the opener, where the
         * search for an according \p{closer} is to be started.
         *
         * This method is useful to scan a string for pairs of opening and closing brackets, while
         * the found segment may contain nested pairs of the same brackets.
         *
         * @param opener The character that represents the opening bracket, e.g. <c>'{'</c>.
         * @param closer The character that represents the closing bracket, e.g. <c>'}'</c>.
         * @param idx    Index pointing to first character behind the (first) \p{opener}.
         *
         * @return   The index of the corresponding closing character. If none was found, a negative
         *           value is returned.
         *           In the latter case the negated (absolute) value is indicating the number of
         *           still open (nested) brackets.
         ******************************************************************************************/
        integer  IndexOfSegmentEnd( TChar opener, TChar closer, integer idx )                 const;


        /** ****************************************************************************************
         * Counts all occurrences of character \p{needle} in the range from \p{startPos} to the end
         * of the string.
         *
         * For empty strings \p{needle}, \c 0 is returned.
         *
         * @param needle    The character to search for.
         * @param startPos  The index to start the counting.
         *                  Optional and defaults to \c 0.
         * @tparam TCheck   Defaults to \c true which is the normal invocation mode.
         *                  If \c false is given, no range check is performed.
         *
         * @return  The index of the first difference in \p{needle}.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer  CountChar( TChar           needle,
                            integer         startPos        = 0   )                            const
        {
            ALIB_STRING_DBG_CHK(this)
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( startPos <  0      )  startPos= 0;
                else if ( startPos >= length )  return 0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   "Non checking and index out of range" )
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
         * Counts all occurrences of character \p{needle}, unless followed by character \p{omit}
         * in the range from \p{startPos} to the end of the string.
         *
         * For empty strings \p{needle}, \c 0 is returned.
         * Also, for empty strings \p{omit}, \c 0 is returned.
         *
         * @param needle    The character to search for.
         * @param omit      Omit occurrence if the given character follows.
         * @param startPos  The index to start the counting.
         * @tparam TCheck   Defaults to \c true which is the normal invocation mode.
         *                  If \c false is given, no range check is performed.
         *
         * @return  The index of the first difference in \p{needle}.
         ******************************************************************************************/
        template <bool TCheck= true>
        integer  CountChar( TChar           needle,
                            TChar           omit,
                            integer         startPos  )                                        const
        {
            ALIB_STRING_DBG_CHK(this)
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                // adjust range, if empty return -1
                     if ( startPos <  0      )  startPos= 0;
                else if ( startPos >= length )  return 0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   "Non checking and index out of range" )
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
                  Case  TSensitivity = Case::Sensitive >
        integer Count( const TString& needle,
                       integer           startPos   = 0 )                                      const
        {
            ALIB_STRING_DBG_CHK(this)
            integer nLen= needle.Length();
            if( nLen == 0  )
                return 0;
            if ALIB_CPP17_CONSTEXPR (TCheck)
            {
                if ( startPos < 0 )                startPos= 0;
                if ( startPos + nLen > length )    return  0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   "Non checking and illegal parameters" )
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
         *                      If \c false is given, parameter \p{startPos} must be valid and
         *                      \p{needle} must not be empty.
         *
         * @return  The index of the first difference in \p{needle}.
         ******************************************************************************************/
        template<bool     TCheck        = true,
                 Case     TSensitivity  = Case::Sensitive>
        integer  Count( const TString&   needle,
                        const TString&   omit,
                        integer         startPos        = 0         )                          const
        {
            ALIB_STRING_DBG_CHK(this)
            integer nLen= needle.Length();
            if ( nLen == 0  )
                return  0;
            if ALIB_CPP17_CONSTEXPR (TCheck)
            {
                if ( startPos < 0 )                startPos= 0;
                if ( startPos + nLen > length )    return  0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   "Non checking and illegal parameters" )
            }


            int     result= 0;
            while( (startPos= IndexOf<false, TSensitivity>( needle, startPos )) >= 0 )
            {
                startPos+= nLen;
                if(    startPos + omit.Length() <= Length()
                    && (    omit.IsEmpty()
                         || ContainsAt<false>( omit, startPos ) )    )
                    continue;

                result++;
            }

            return result;
        }

ALIB_WARNINGS_RESTORE // ALIB_WARNINGS_IGNORE_IF_CONSTEXPR



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
         * the string
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to not using - and therefore also not parsing - grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}. All of its interface methods
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
        int64_t  ParseInt( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                           integer* newIdx= nullptr ) const;

        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseInt(integer\,TNumberFormat<TChar>*\,integer*)const,ParseInt}
         * providing default values for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        int64_t  ParseInt( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseInt( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseInt(integer\,TNumberFormat<TChar>*\,integer*)const,ParseInt}
         * providing default values for omitted parameters.
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
         * Overloaded version of
         * \alib{strings,TString::ParseInt(integer\,TNumberFormat<TChar>*\,integer*)const,ParseInt}
         * providing default values for omitted parameters.
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
         * from this %AString.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational}
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
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}. All number-parsing interface methods
         * have a corresponding implementation within this class.
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
        uint64_t  ParseDec( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;

        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseDec(integer\,TNumberFormat<TChar>*\,integer*)const,ParseDec}
         * providing default values for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseDec( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseDec( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseDec(integer\,TNumberFormat<TChar>*\,integer*)const,ParseDec}
         * providing default values for omitted parameters.
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
         * Overloaded version of
         * \alib{strings,TString::ParseDec(integer\,TNumberFormat<TChar>*\,integer*)const,ParseDec}
         * providing default values for omitted parameters.
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
         * from this string.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to not using - and therefore also not parsing - grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}. All number-parsing interface methods
         * have a corresponding implementation within this class.
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
        uint64_t  ParseBin( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;


        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseBin(integer\,TNumberFormat<TChar>*\,integer*)const,ParseBin}
         * providing default values for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseBin( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseBin( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseBin(integer\,TNumberFormat<TChar>*\,integer*)const,ParseBin}
         * providing default values for omitted parameters.
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
         * Overloaded version of
         * \alib{strings,TString::ParseBin(integer\,TNumberFormat<TChar>*\,integer*)const,ParseBin}
         * providing default values for omitted parameters.
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
         * from this string.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to not using - and therefore also not parsing - grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}. All number-parsing interface methods
         * have a corresponding implementation within this class.
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
        uint64_t  ParseHex( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;


        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseHex(integer\,TNumberFormat<TChar>*\,integer*)const,ParseHex}
         * providing default values for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseHex( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseHex( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseHex(integer\,TNumberFormat<TChar>*\,integer*)const,ParseHex}
         * providing default values for omitted parameters.
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
         * Overloaded version of
         * \alib{strings,TString::ParseHex(integer\,TNumberFormat<TChar>*\,integer*)const,ParseHex}
         * providing default values for omitted parameters.
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
         * from this string.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to not using - and therefore also not parsing - grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}. All number-parsing interface methods
         * have a corresponding implementation within this class.
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
        uint64_t  ParseOct( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;


        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseOct(integer\,TNumberFormat<TChar>*\,integer*)const,ParseOct}
         * providing default values for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        uint64_t  ParseOct( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseOct( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseOct(integer\,TNumberFormat<TChar>*\,integer*)const,ParseOct}
         * providing default values for omitted parameters.
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
         * Overloaded version of
         * \alib{strings,TString::ParseOct(integer\,TNumberFormat<TChar>*\,integer*)const,ParseOct}
         * providing default values for omitted parameters.
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
         * Reads a floating point number at the given position from this string.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to 'international' settings (not using the locale) and therefore
         * also not parsing grouping characters.
         *
         * Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
         * If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
         * read.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}. All number-parsing interface methods
         * have a corresponding implementation within this class.
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
        double   ParseFloat( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                             integer* newIdx= nullptr ) const;

        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseFloat(integer\,TNumberFormat<TChar>*\,integer*)const,ParseFloat}
         * providing default values for omitted parameters.
         *
         * @param numberFormat The format definition. Defaults to \c nullptr.
         * @param[out] newIdx  Optional output variable that will point to the first
         *                     character in this string after the number parsed.
         *                     On failure, it will be set to the initial value \c 0.
         * @return  The parsed value. In addition, the output parameter \b newIdx is set to
         *          point to the first character behind the parsed number.
         ******************************************************************************************/
        inline
        double  ParseFloat( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseFloat( 0, numberFormat, newIdx );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \alib{strings,TString::ParseFloat(integer\,TNumberFormat<TChar>*\,integer*)const,ParseFloat}
         * providing default values for omitted parameters.
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
         * Overloaded version of
         * \alib{strings,TString::ParseFloat(integer\,TNumberFormat<TChar>*\,integer*)const,ParseFloat}
         * providing default values for omitted parameters.
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
         * Copies this string's contents into a given character buffer.
         * It is the caller's responsibility that \p{dest} is large enough, write-enabled, etc.
         *
         * @param dest   The destination buffer.
         * @return    The length of this string.
         ******************************************************************************************/
        inline
        integer CopyTo( TChar* dest ) const
        {
            characters::CharArray<TChar>::Copy( buffer, length, dest );
            return length;
        }

    /** ############################################################################################
     * @name Helper methods
     ##@{ ########################################################################################*/
    public:
        /** ************************************************************************************
         * Adjusts a region given as in/out parameters, to fit to this object's range [0..length].
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
                regionLength+=  regionStart;
                regionStart=    0;
            }

            // adjust length
            if ( regionLength < 0 )
            {
                regionLength= 0;
                return true;
            }

            integer maxRegionLength=  length - regionStart;
            if ( regionLength > maxRegionLength )
                 regionLength=  maxRegionLength;

            // return true if adjusted region is empty
            return  regionLength == 0;
        }

    /** ############################################################################################
     * @name std::iterator
     ##@{ ########################################################################################*/
    public:
        /** ****************************************************************************************
         * Implementation of \c std::iterator for class \b %TString and its descendents.
         * Base class \b String exposes #ConstIterator which uses
         * <c>const TChar*</c> and <c>const TChar&</c> for template types \p{TPointer} and
         * \p{TReference}. Descendant classes may expose a mutable
         * version (e.g. \alib{strings,TAString,AString}).
         *
         * As the name of the class indicates, this iterator satisfies the std library concept
         * [RandomAccessIterator](http://en.cppreference.com/w/cpp/concept/RandomAccessIterator).
         ******************************************************************************************/
        template<typename TPointer, typename TReference>
        class TRandomAccessIterator
            : public std::iterator< std::random_access_iterator_tag,  // iterator_category
                                    TChar,                            // value_type
                                    integer,                          // distance type
                                    TPointer,                         // pointer
                                    TReference                        // reference
                                  >
        {
            protected:
                /** The pointer into the buffer is all we store. */
                TPointer p;

            public:
                /** Constructor.
                 *  @param start Pointer to the initial character.   */
                explicit TRandomAccessIterator( TPointer start = nullptr ) : p(start)
                {
                }

            //######################   To satisfy concept of  InputIterator   ######################

                /** Prefix increment operator.
                 *  @return A reference to this object. */
                TRandomAccessIterator& operator++()
                {
                    p++;
                    return *this;
                }

                 /** Postfix increment operator.
                 *  @return An iterator value that is not increased, yet. */
               TRandomAccessIterator operator++(int)
                {
                    return TRandomAccessIterator(p++);
                }

                /** Comparison operator.
                 *  @param other  The iterator to compare ourselves to.
                 *  @return \c true if this and the given iterator are pointing to the same character
                 *          in the same array, \c false otherwise. */
                bool operator==(TRandomAccessIterator other)                                const
                {
                    return p == other.p;
                }

                /** Comparison operator.
                 *  @param other  The iterator to compare ourselves to.
                 *  @return \c true if this and given iterator are not equal, \c false otherwise. */
                bool operator!=(TRandomAccessIterator other)                                const
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
                 *  @return A reference to this object. */
                TRandomAccessIterator& operator--()
                {
                    p--;
                    return *this;
                }


                /** Postfix decrement operator.
                 *  @return The iterator value prior the decrement operation. */
                TRandomAccessIterator operator--(int)
                {
                    return TRandomAccessIterator(p--);
                }


            //##################   To satisfy concept of  RandomAccessIterator   ###################

                /** Addition assignment.
                 *  @param n The value to subtract.
                 *  @return A reference to this iterator. */
                TRandomAccessIterator& operator+=(integer n)
                {
                    p+= n;
                    return *this;
                }

                /** Subtraction assignment.
                 *  @param n The value to subtract.
                 *  @return A reference to this iterator. */
                TRandomAccessIterator& operator-=(integer n)
                {
                    p-= n;
                    return *this;
                }

                /** Addition.
                 *  @param n The value to subtract.
                 *  @return The resulting iterator value. */
                TRandomAccessIterator operator+(integer n)                                  const
                {
                    return TRandomAccessIterator( p + n );
                }

                /** Subtraction.
                 *  @param n The value to subtract.
                 *  @return The resulting iterator value. */
                TRandomAccessIterator operator-(integer n)                                  const
                {
                    return TRandomAccessIterator( p - n );
                }

                /** Difference (distance) from this iterator to the given one.
                 *  @param other  The iterator to subtract
                 *  @return The difference between (distance of) this and the given iterator.    */
                integer operator-(TRandomAccessIterator other)                              const
                {
                    return p - other.p;
                }

                /** Subscript operator.
                 *  @param n  The distance to add.
                 *  @return Reference to the character referenced by this iterator plus the distance
                 *          given.
                 */
                TReference operator[]( integer n )                                             const
                {
                    return *( p + n );
                }

            //#### Comparison operators (also needed to satisfy concept of RandomAccessIterator) ###

                /** Compares this iterator with the given one.
                 *  @param other  The iterator to compare
                 *  @return \c true if this iterator is \e smaller than \p{other},
                 *          \c false otherwise. */
                bool operator<(TRandomAccessIterator other)                                 const
                {
                    return p < other.p;
                }

                /** Compares this iterator with the given one.
                 *  @param other  The iterator to compare
                 *  @return \c true if this iterator is \e smaller than or equal to \p{other},
                 *          \c false otherwise. */
                bool operator<=(TRandomAccessIterator other)                                const
                {
                    return p <= other.p;
                }


                /** Compares this iterator with the given one.
                 *  @param other  The iterator to compare
                 *  @return \c true if this iterator is \e greater than \p{other},
                 *          \c false otherwise. */
                bool operator>(TRandomAccessIterator other)                                 const
                {
                    return p > other.p;
                }

                /** Compares this iterator with the given one.
                 *  @param other  The iterator to compare
                 *  @return \c true if this iterator is \e greater than or equal to \p{other},
                 *          \c false otherwise. */
                bool operator>=(TRandomAccessIterator other)                                const
                {
                    return p >= other.p;
                }
        };

        /**
         * The constant iterator exposed by this character container. A Mutable version is
         * found only in descendant classes (e.g. \alib{strings,TAString,AString}).
         */
        using ConstIterator= TRandomAccessIterator<const TChar*, const TChar&>;


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
        TString( ConstIterator& start, ConstIterator& end )
        : buffer( &*start)
        , length( end-start >= 0 ? end-start : 0 )
        {}

    protected:
        #if !ALIB_DOCUMENTATION_PARSER
            ALIB_WARNINGS_IGNORE_DOCS // needed due to a bug in current clang
        #endif

        /** ****************************************************************************************
         * Implementation of the sub-string search function.
         * @param  needle       The sub-string to search.
         * @param  startIdx     The start index of the search.
         * @tparam TSensitivity The letter case sensitivity of the search.
         * @return The index of the first occurrence of \p{needle}, respectively \c -1 if not found.
         ******************************************************************************************/
        template<Case TSensitivity =Case::Sensitive>
        ALIB_API integer
        indexOfString( const TString&  needle, integer startIdx )                          const;

        #if !ALIB_DOCUMENTATION_PARSER
            ALIB_WARNINGS_RESTORE
        #endif
}; // class TString


// #################################################################################################
// Template instantiation declarations
// #################################################################################################
#if !ALIB_DOCUMENTATION_PARSER

template<>      ALIB_API integer  TString<nchar>::WStringLength                 ()                                           const;
extern template ALIB_API integer  TString<nchar>::indexOfString<Case::Sensitive>( const TString<nchar   >&, integer        ) const;
extern template ALIB_API integer  TString<nchar>::indexOfString<Case::Ignore   >( const TString<nchar   >&, integer        ) const;
extern template ALIB_API integer  TString<nchar>::IndexOfSegmentEnd             ( nchar, nchar, integer                    ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseDecDigits                ( integer, integer*                        ) const;
extern template ALIB_API  int64_t TString<nchar>::ParseInt                      ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseDec                      ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseBin                      ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseHex                      ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseOct                      ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API double   TString<nchar>::ParseFloat                    ( integer, TNumberFormat<nchar>*, integer* ) const;

template<> inline        integer  TString<wchar>::WStringLength() const  { return length; }
extern template ALIB_API integer  TString<wchar>::indexOfString<Case::Sensitive>(const TString<wchar>&, integer            ) const;
extern template ALIB_API integer  TString<wchar>::indexOfString<Case::Ignore   >(const TString<wchar>&, integer            ) const;
extern template ALIB_API integer  TString<wchar>::IndexOfSegmentEnd             (wchar, wchar, integer                     ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseDecDigits                ( integer, integer*                        ) const;
extern template ALIB_API  int64_t TString<wchar>::ParseInt                      ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseDec                      ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseBin                      ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseHex                      ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseOct                      ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API double   TString<wchar>::ParseFloat                    ( integer, TNumberFormat<wchar>*, integer* ) const;

template<>      ALIB_API integer  TString<xchar>::WStringLength                 ()                                           const;
extern template ALIB_API integer  TString<xchar>::indexOfString<Case::Sensitive>( const TString<xchar   >&, integer        ) const;
extern template ALIB_API integer  TString<xchar>::indexOfString<Case::Ignore   >( const TString<xchar   >&, integer        ) const;
extern template ALIB_API integer  TString<xchar>::IndexOfSegmentEnd             ( xchar, xchar, integer                    ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseDecDigits                ( integer, integer*                        ) const;
extern template ALIB_API  int64_t TString<xchar>::ParseInt                      ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseDec                      ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseBin                      ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseHex                      ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseOct                      ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API double   TString<xchar>::ParseFloat                    ( integer, TNumberFormat<xchar>*, integer* ) const;

// #################################################################################################
// debug members
// #################################################################################################
#if ALIB_STRINGS_DEBUG
    extern template ALIB_API void    TString<nchar>::dbgCheck() const;
    extern template ALIB_API void    TString<wchar>::dbgCheck() const;
    extern template ALIB_API void    TString<xchar>::dbgCheck() const;
#endif

#endif //!ALIB_DOCUMENTATION_PARSER

}} // namespace aworx[::lib::strings]

#if defined(_MSC_VER)
    #pragma warning( pop )
#endif

/// Inline shortcut method to create a constexpr \e nulled string of standard character size.
/// @return A \e nulled string.
inline constexpr  String            NullString()            { return String(nullptr);  }

/// Inline shortcut method to create a constexpr \e nulled string of complementary character size.
/// @return A \e nulled string.
inline constexpr  ComplementString  NullComplementString()  { return ComplementString(nullptr);}

/// Inline shortcut method to create a constexpr \e nulled string of strange character size.
/// @return A \e nulled string.
inline constexpr  StrangeString     NullStrangeString()     { return StrangeString(nullptr);  }

/// Inline shortcut method to create a constexpr \e nulled string of narrow character size.
/// @return A \e nulled string.
inline constexpr NString            NullNString()           { return NString(nullptr);  }

/// Inline shortcut method to create a constexpr \e nulled string of wide character size.
/// @return A \e nulled string.
inline constexpr WString            NullWString()           { return WString(nullptr);  }

/// Inline shortcut method to create a constexpr \e nulled string of strange character size.
/// @return A \e nulled string.
inline constexpr XString            NullXString()           { return XString(nullptr);  }


} // namespace [aworx]



#endif // HPP_ALIB_STRINGS_STRING
