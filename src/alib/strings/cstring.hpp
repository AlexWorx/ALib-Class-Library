// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGS_CSTRING
#define HPP_ALIB_STRINGS_CSTRING 1

#if !defined (HPP_ALIB_STRINGS_STRING)
#   include "alib/strings/string.hpp"
#endif


// conditional expression is constant for using our constant template parameters to select
// checking vs. non-checking method versions
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif

namespace aworx { namespace lib { namespace strings {

/** ************************************************************************************************
 * This class specializes its base class \alib{strings,TString,String} in that respect that
 * the character strings represented are guaranteed to be zero-terminated.<br>
 * Zero-terminated strings are widely used by programming language \b C and are often called
 * "C-strings", what gave the class its name.
 *
 * \see
 *   For an introduction into the \alib string classes see this module's
 *   \ref alib_mod_strings "Programmer's Manual".
 *
 * @tparam TChar The character type.<br>
 *   Alias names for specializations of this class using character types
 *   \alib{characters,character}, \alib{characters,nchar}, \alib{characters,wchar},
 *   \alib{characters,xchar}, \alib{characters,complementChar} and \alib{characters,strangeChar}
 *   are provided in namespace #aworx with type definitions \aworx{CString}, \aworx{NCString},
 *   \aworx{WCString}, \aworx{XCString}, \aworx{ComplementCString} and \aworx{StrangeCString}.
 **************************************************************************************************/
template<typename TChar>
class TCString : public TString<TChar>
{
    /** ############################################################################################
     * @name Constructor And Assignment
     ##@{ ########################################################################################*/
    public:

        /** ****************************************************************************************
         * Default constructor creating a \ref alib_strings_details_nulled \e "nulled" c-string.
         ******************************************************************************************/
        constexpr  inline
        TCString()
        : TString<TChar>()
        {}

        /** ****************************************************************************************
         * Constructor accepting a pointer to a character array and a string length.
         *
         * \note
         *   It is a user's responsibility to ensure that the character array provided includes
         *   a terminating \c '\0' character.<br>
         *   In debug-compilations a run-time assertion is raised if the provided buffer is not
         *   zero-terminated.
         *
         *
         * @param buffer          The buffer to use.
         * @param contentLength   The length of the content in the given buffer.
         ******************************************************************************************/
#if ALIB_DOCUMENTATION_PARSER
        constexpr
#else
        ALIB_CPP14_REL_CONSTEXPR
#endif
        inline  explicit
        TCString( const TChar* buffer, integer contentLength )
        : TString<TChar>( buffer, contentLength )
        {
            ALIB_ASSERT_ERROR(   TString<TChar>::IsNull()
                              || TString<TChar>::buffer[TString<TChar>::length] == '\0'
                 ,"Error: Explicit construction of CString with unterminated string."     )
        }


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
         * In debug-compilations a run-time assertion is raised if the provided buffer is not
         * zero-terminated.
         *
         * \see
         *   More information about construction of this type is provided with chapter
         *   \ref alib_strings_cc_construction_cstring of the Programmer's Manual of module
         *   \alibmod_strings.
         *
         * @tparam TZTCharArray  Type that comprises a zero-terminated character array.
         * @param  src           The source object.
         ******************************************************************************************/
        template <typename TZTCharArray>
        inline
        constexpr
        TCString( const  TTerminatable& src );


        /** ****************************************************************************************
         * \b Implicit cast operator to objects of type \p{TZTCharArray}.<br>
         * This operator is available for all custom types that have an accordingly specialized
         * version of TMP struct \alib{characters,T_CharArray} and/or
         * \alib{characters,T_ZTCharArray} defined.
         *
         * \see
         *   More information about casting \alib string types to built-in C++ types or custom
         *   types is provided with chapter \ref alib_strings_cc_cast_cstring of the programmer's
         *   manual of module \alibmod_strings.
         *
         * @tparam TZTCharArray The custom type to implicitly convert this object to.
         * @return A value of custom string type.
         ******************************************************************************************/
        template<typename TZTCharArray>
        inline
        operator TZTCharArray () const
        {}

        /** ****************************************************************************************
         * \b Explicit cast operator to objects of type \p{TZTCharArray}.<br>
         * This operator is available for all custom types that have an accordingly specialized
         * version of TMP struct \alib{characters,T_CharArray} and/or
         * \alib{characters,T_ZTCharArray} defined.
         *
         * \see
         *   More information about casting \alib string types to built-in C++ types or custom
         *   types is provided with chapter \ref alib_strings_cc_cast_cstring of the programmer's
         *   manual of module \alibmod_strings.
         *
         * @tparam TZTCharArray The custom type to explicitly convert this object to.
         * @return A value of custom string type.
         ******************************************************************************************/
        template<typename TZTCharArray>
        inline explicit
        operator TZTCharArray () const
        {}

    #else // no doxygen now

        // ##################### Constructors selected with std::enable_if #########################

        ATMP_SELECT_IF_1TP( typename T, std::is_same<std::nullptr_t,T>::value )
        constexpr
        TCString(const  T&   )
        : TString<TChar>( nullptr, 0 )
        {}

        ATMP_SELECT_IF_1TP( typename T, characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::Implicit )
        ALIB_CPP14_REL_CONSTEXPR
        TCString(const T& src)
        : TString<TChar>(  characters::T_ZTCharArray<ATMP_RCV(T), TChar>::Buffer(  src ),
                           characters::T_ZTCharArray<ATMP_RCV(T), TChar>::Length(  src ) )
        {
            ALIB_ASSERT_ERROR(   TString<TChar>::IsNull()
                              || TString<TChar>::buffer[TString<TChar>::length] == '\0'
                 ,"Error: Implicit construction of CString with unterminated string."     )
        }

        ATMP_SELECT_IF_1TP( typename T, characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::Implicit )
        ALIB_CPP14_REL_CONSTEXPR
        TCString(const T* src)
        : TString<TChar>(  characters::T_ZTCharArray<ATMP_RCV(T), TChar>::Buffer(  *src ),
                           characters::T_ZTCharArray<ATMP_RCV(T), TChar>::Length(  *src ) )
        {
            ALIB_ASSERT_ERROR(   TString<TChar>::IsNull()
                              || TString<TChar>::buffer[TString<TChar>::length] == '\0'
                 ,"Error: Implicit construction of CString with unterminated string."     )
        }

        ATMP_SELECT_IF_1TP( typename T, characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::ExplicitOnly )
        ALIB_CPP14_REL_CONSTEXPR
        explicit
        TCString(      T& src)
        : TString<TChar>(  characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Buffer( src ),
                           characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Length( src ) )
        {
            ALIB_ASSERT_ERROR(   TString<TChar>::IsNull()
                              || TString<TChar>::buffer[TString<TChar>::length] == '\0'
                 ,"Error: Explicit construction of CString with unterminated string."     )
        }

        ATMP_SELECT_IF_1TP( typename T, characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::ExplicitOnly  )
        ALIB_CPP14_REL_CONSTEXPR
        explicit
        TCString(      T* src)
        : TString<TChar>(  characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Buffer( *src ),
                           characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Length( *src ) )
        {
            ALIB_ASSERT_ERROR(   TString<TChar>::IsNull()
                              || TString<TChar>::buffer[TString<TChar>::length] == '\0'
                 ,"Error: Explicit construction of CString with unterminated string."     )
        }

        ATMP_SELECT_IF_1TP( typename T, characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::MutableOnly && !std::is_const<T>::value )
        ALIB_CPP14_REL_CONSTEXPR
        explicit
        TCString(      T& src)
        : TString<TChar>(  characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Buffer( const_cast<T&>( src ) ),
                           characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Length( const_cast<T&>( src ) ) )
        {
            ALIB_ASSERT_ERROR(   TString<TChar>::IsNull()
                              || TString<TChar>::buffer[TString<TChar>::length] == '\0'
                 ,"Error: Construction of CString (from mutable object) with unterminated string." )
        }

        ATMP_SELECT_IF_1TP( typename T, characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::MutableOnly && !std::is_const<T>::value )
        ALIB_CPP14_REL_CONSTEXPR
        explicit
        TCString(      T* src)
        : TString<TChar>(  characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Buffer( *src ),
                           characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Length( *src ) )
        {
            ALIB_ASSERT_ERROR(   TString<TChar>::IsNull()
                              || TString<TChar>::buffer[TString<TChar>::length] == '\0'
                 ,"Error: Construction of CString (from mutable object) with unterminated string." )
        }

        // ##############################    casting  back    ######################################
        ATMP_SELECT_IF_1TP(typename T,    characters::T_CharArray<T,TChar>::Construction == characters::ConstructionType::Implicit
                                       && !T_SuppressAutoCast<TCString<TChar>,characters::ConstructionType::Implicit    ,ATMP_RCV(T)>::value)
        operator T () const
        {
            return  characters::T_CharArray<T,TChar>::Construct( TString<TChar>::buffer, TString<TChar>::length );
        }

        ATMP_SELECT_IF_1TP(typename T,    characters::T_CharArray<T,TChar>::Construction == characters::ConstructionType::ExplicitOnly
                                       && !T_SuppressAutoCast<TCString<TChar>,characters::ConstructionType::ExplicitOnly,ATMP_RCV(T)>::value)
        explicit
        operator T () const
        {
            return  characters::T_CharArray<T,TChar>::Construct( TString<TChar>::buffer, TString<TChar>::length );
        }


        ATMP_SELECT_IF_1TP(typename T,    characters::T_CharArray  <T,TChar>::Construction != characters::ConstructionType::Implicit
                                       && characters::T_ZTCharArray<T,TChar>::Construction == characters::ConstructionType::Implicit
                                       && !T_SuppressAutoCast<TCString<TChar>,characters::ConstructionType::Implicit    ,ATMP_RCV(T)>::value)
        operator T () const
        {
            return  characters::T_ZTCharArray<T,TChar>::Construct( TString<TChar>::buffer, TString<TChar>::length );
        }

        ATMP_SELECT_IF_1TP(typename T,    characters::T_CharArray  <T,TChar>::Construction != characters::ConstructionType::ExplicitOnly
                                       && characters::T_ZTCharArray<T,TChar>::Construction == characters::ConstructionType::ExplicitOnly
                                       && !T_SuppressAutoCast<TCString<TChar>,characters::ConstructionType::ExplicitOnly,ATMP_RCV(T)>::value)
        explicit
        operator T () const
        {
            return  characters::T_ZTCharArray<T,TChar>::Construct( TString<TChar>::buffer, TString<TChar>::length );
        }

    #endif // doxygen


    /** ############################################################################################
     * @name Interface
     ##@{ ########################################################################################*/
    public:
        /** ****************************************************************************************
         * Reads a character at a given index.<br> Overrides
         * \alib{strings,TString::operator[],String::operator[]} to change the debug assertion
         * to allow inclusion of the termination character.
         *
         * \attention
         *   No parameter check is performed (other than an assertions in debug-compilation of
         *   \alib). See \alib{strings,TString::operator[],String::operator[]} for details.
         *
         * @param   op    The index of the character within this object's buffer.
         * @returns If the character contained at index \p{op}.
         ******************************************************************************************/
         inline
         TChar    operator[] (integer  op) const
         {
            ALIB_ASSERT_ERROR( op >= 0  && op <= TString<TChar>::length, "Index out of bounds" )
            return TString<TChar>::buffer[op];
         }

        /** ****************************************************************************************
         * Returns the index of the first character which is included, respectively <em>not</em>
         * included in a given set of characters.
         *
         * This method searches forwards. For backwards search, see
         * \alib{strings,TString::LastIndexOfAny,String::LastIndexOfAny}.
         *
         * \note
         *   This method overrides method \alib{strings,TString::IndexOfAny,String::IndexOf}.
         *   This implementation however expects a \b %CString with parameter \p{needles}
         *   (beside the fact that it has to be invoked on a \b %CString itself).
         *   If no zero-terminated needle string is available, the parent's original method
         *   needs to be invoked. This has to be done by explicitly naming
         *   the parent class in the invocation, like for example in
         *
         *          myCString.TString::IndexOfAny<Inclusion::Include>( myString );
         *
         * \note
         *   On most platforms, this zero-terminated version should perform slightly faster than
         *   the original method in class \b %String.
         *
         * @param needles    Set of characters to be taken into account.
         * @param startIdx   The index to start the search at. If the given value is less than \c 0,
         *                   it is set to \c 0. If it exceeds the length of the string, the length of
         *                   the string is returned.
         *                   Defaults to \c 0.
         * @tparam TCheck    Defaults to \c true which is the normal invocation mode.
         *                   If \c <false\> is added to the method name, no parameter checks are
         *                   performed and the needles must not be empty.
         * @tparam inclusion Denotes whether the search returns the first index that holds a value
         *                   that is included or that is not excluded in the set of needle
         *                   characters.
         *
         * @return The index of the first character found which is included, respectively not
         *         included, in the given set of characters. If nothing is found, \c -1 is returned.
         ******************************************************************************************/
        template <Inclusion   inclusion,
                  bool        TCheck= true >
        inline
        integer  IndexOfAny( const TCString& needles, integer startIdx= 0 )
        const
        {
            if (TCheck)
            {
                if ( startIdx < 0                          ) startIdx= 0;
                if ( startIdx >= TString<TChar>::length ) return   -1;
            }
            else
            {
                ALIB_ASSERT_ERROR(    startIdx >= 0
                                   && startIdx < TString<TChar>::length
                                   && needles.Length() != 0,
                                   "Non checking and illegal parameters" )
            }


            if ( inclusion == Inclusion::Include )
            {
                integer idx=  characters::CharArray<TChar>
                              ::IndexOfAnyIncludedZT(TString<TChar>::buffer + startIdx, needles);
                return  idx >= 0 ? idx + startIdx : -1;
            }
            else
            {
                const TChar* haystack= TString<TChar>::buffer + startIdx;
                integer idx=  characters::CharArray<TChar>::IndexOfAnyExcludedZT( haystack, needles );
                return idx < 0 || *( haystack + idx ) == '\0' ? -1 : startIdx + idx;
            }
        }

}; // class TCString


/** ************************************************************************************************
 * This template class has three specializations for types \c nchar, \c wchar, and \c xchar, which
 * each provide static fields:
 * - \b EmptyString,
 * - \b NewLine and
 * - \b DefaultWhitespaces.
 *
 * The class is useful to implement methods that are templated with the character type they use.
 *
 * \note
 *   In non-templated code (that works with fixed or logical character sizes), it might lead to
 *   better readable code, if the following shortcut/alias functions of namespace #aworx are used:
 *   - \ref EmptyString, \ref EmptyComplementString, \ref EmptyStrangeString,
 *     \ref EmptyNString, \ref EmptyWString, \ref EmptyXString,
 *   - \ref NewLine, \ref ComplementNewLine, \ref StrangeNewLine,
 *     \ref NNewLine, \ref WNewLine, \ref XNewLine and
 *   - \ref DefaultWhitespaces, \ref ComplementDefaultWhitespaces, \ref StrangeDefaultWhitespaces,
 *     \ref NDefaultWhitespaces, \ref WDefaultWhitespaces, \ref XDefaultWhitespaces.
 *  <p>
 *
 * \note
 *   Constants for \e nulled strings are given with
 *   \ref NullString, \ref NullComplementString, \ref NullStrangeString,
 *   \ref NullNString, \ref NullWString and \ref NullXString. However, those are returning
 *   type \b String instead of \b CString. For \e nulled objects of type \b CString use
 *   keyword \c nullptr which performs implicit constexpr creation.
 *
 * \see
 *   See also manual chapter \ref alib_strings_details_constants.
 *
 * @tparam TChar The character type.
 **************************************************************************************************/
template<typename TChar> struct TT_StringConstants
{
    #if ALIB_DOCUMENTATION_PARSER
    /** @return A \e zero-terminated empty string. */
    constexpr inline static CString<TChar>     EmptyString();

    /**
     * On Windows OS, the returned string contains characters <c>'\\r'</c> and <c>'\\n'</c>, on
     * other platforms just character <c>'\\n'</c>.
     * @return A \e zero-terminated string containing platform dependent "newline" charcater(s).
     */
    constexpr inline static CString<TChar>     NewLine();

    /** @return A zero-terminated string containing default whitespace characters "space", "newline"
    *  "carriage return" and "tablulator", hence <c>" \n\r\t"</c>.  */
    constexpr inline static CString<TChar>     DefaultWhitespaces());
    #endif
};


#if !ALIB_DOCUMENTATION_PARSER

template<> struct TT_StringConstants<nchar>
{
    ALIB_CPP14_CONSTEXPR static inline NCString  EmptyString()        { return         ""        ; }

  #if defined(_WIN32)
    ALIB_CPP14_CONSTEXPR static inline NCString  NewLine()            { return         "\r\n"    ; }
  #else
    ALIB_CPP14_CONSTEXPR static inline NCString  NewLine()            { return         "\n"      ; }
  #endif

    ALIB_CPP14_CONSTEXPR static inline NCString  DefaultWhitespaces() { return         " \n\r\t" ; }
};

template<> struct TT_StringConstants<wchar>
{
    ALIB_CPP14_CONSTEXPR static inline WCString  EmptyString()        { return A_WCHAR(""       ); }

  #if defined(_WIN32)
    ALIB_CPP14_CONSTEXPR static inline WCString  NewLine()            { return A_WCHAR("\r\n"   ); }
  #else
    ALIB_CPP14_CONSTEXPR static inline WCString  NewLine()            { return A_WCHAR("\n"     ); }
  #endif

    ALIB_CPP14_CONSTEXPR static inline WCString  DefaultWhitespaces() { return A_WCHAR(" \n\r\t"); }
};

template<> struct TT_StringConstants<xchar>
{
    ALIB_CPP14_CONSTEXPR static inline XCString  EmptyString()        { return A_XCHAR(""       ); }

  #if defined(_WIN32)
    ALIB_CPP14_CONSTEXPR static inline XCString  NewLine()            { return A_XCHAR("\r\n"   ); }
  #else
    ALIB_CPP14_CONSTEXPR static inline XCString  NewLine()            { return A_XCHAR("\n"     ); }
  #endif

    ALIB_CPP14_CONSTEXPR static inline XCString  DefaultWhitespaces() { return A_XCHAR(" \n\r\t"); }
};

#endif  //!ALIB_DOCUMENTATION_PARSER

}} // namespace aworx[::lib::strings]


/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::EmptyString,TT_StringConstants<character>::EmptyString}.
/// @return A zero-terminated, empty string.
inline ALIB_CPP14_CONSTEXPR CString             EmptyString()          { return lib::strings::TT_StringConstants<character>::EmptyString();        }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::EmptyString,TT_StringConstants<complementChar>::EmptyString}.
/// @return A zero-terminated, empty string.
inline ALIB_CPP14_CONSTEXPR ComplementCString   EmptyComplementString(){ return lib::strings::TT_StringConstants<complementChar>::EmptyString();   }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::EmptyString,TT_StringConstants<strangeChar>::EmptyString}.
/// @return A zero-terminated, empty string.
inline ALIB_CPP14_CONSTEXPR StrangeCString      EmptyStrangeString()   { return lib::strings::TT_StringConstants<strangeChar>::EmptyString();      }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::EmptyString,TT_StringConstants<nchar>::EmptyString}.
/// @return A zero-terminated, empty string.
inline ALIB_CPP14_CONSTEXPR NCString            EmptyNString()         { return lib::strings::TT_StringConstants<nchar    >::EmptyString();        }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::EmptyString,TT_StringConstants<wchar>::EmptyString}.
/// @return A zero-terminated, empty string.
inline ALIB_CPP14_CONSTEXPR WCString            EmptyWString()         { return lib::strings::TT_StringConstants<wchar    >::EmptyString();        }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::DefaultWhitespaces,TT_StringConstants<xchar>::DefaultWhitespaces}.
/// @return A zero-terminated, empty string.
inline ALIB_CPP14_CONSTEXPR XCString            EmptyXString()         { return lib::strings::TT_StringConstants<xchar    >::DefaultWhitespaces(); }



/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::NewLine,TT_StringConstants<character>::NewLine}.
/// @return A zero-terminated string containing the new-line character sequence.
inline ALIB_CPP14_CONSTEXPR  CString            NewLine()             { return lib::strings::TT_StringConstants<character>::NewLine();            }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::NewLine,TT_StringConstants<complementChar>::NewLine}.
/// @return A zero-terminated string containing the new-line character sequence.
inline ALIB_CPP14_CONSTEXPR  ComplementCString  ComplementNewLine()   { return lib::strings::TT_StringConstants<complementChar>::NewLine();       }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::NewLine,TT_StringConstants<strangeChar>::NewLine}.
/// @return A zero-terminated string containing the new-line character sequence.
inline ALIB_CPP14_CONSTEXPR  StrangeCString     StrangeNewLine()      { return lib::strings::TT_StringConstants<strangeChar>::NewLine();          }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::NewLine,TT_StringConstants<nchar>::NewLine}.
/// @return A zero-terminated string containing the new-line character sequence.
inline ALIB_CPP14_CONSTEXPR NCString            NNewLine()             { return lib::strings::TT_StringConstants<nchar    >::NewLine();            }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::NewLine,TT_StringConstants<wchar>::NewLine}.
/// @return A zero-terminated string containing the new-line character sequence.
inline ALIB_CPP14_CONSTEXPR WCString            WNewLine()             { return lib::strings::TT_StringConstants<wchar    >::NewLine();            }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::NewLine,TT_StringConstants<xchar>::NewLine}.
/// @return A zero-terminated string containing the new-line character sequence.
inline ALIB_CPP14_CONSTEXPR XCString            XNewLine()             { return lib::strings::TT_StringConstants<xchar    >::NewLine();            }



/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::DefaultWhitespaces,TT_StringConstants<character>::DefaultWhitespaces}.
/// @return A zero-terminated string of default whitespace characters.
inline ALIB_CPP14_CONSTEXPR  CString            DefaultWhitespaces()  { return lib::strings::TT_StringConstants<character>::DefaultWhitespaces(); }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::DefaultWhitespaces,TT_StringConstants<complementChar>::DefaultWhitespaces}.
/// @return A zero-terminated string of default whitespace characters.
inline ALIB_CPP14_CONSTEXPR  ComplementCString  ComplementDefaultWhitespaces() { return lib::strings::TT_StringConstants<complementChar>::DefaultWhitespaces(); }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::DefaultWhitespaces,TT_StringConstants<strangeChar>::DefaultWhitespaces}.
/// @return A zero-terminated string of default whitespace characters.
inline ALIB_CPP14_CONSTEXPR  StrangeCString     StrangeDefaultWhitespaces() { return lib::strings::TT_StringConstants<strangeChar>::DefaultWhitespaces(); }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::DefaultWhitespaces,TT_StringConstants<nchar>::DefaultWhitespaces}.
/// @return A zero-terminated string of default whitespace characters.
inline ALIB_CPP14_CONSTEXPR NCString            NDefaultWhitespaces()  { return lib::strings::TT_StringConstants<nchar    >::DefaultWhitespaces(); }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::DefaultWhitespaces,TT_StringConstants<wchar>::DefaultWhitespaces}.
/// @return A zero-terminated string of default whitespace characters.
inline ALIB_CPP14_CONSTEXPR WCString            WDefaultWhitespaces()  { return lib::strings::TT_StringConstants<wchar    >::DefaultWhitespaces(); }

/// Inline shortcut to method \alib{strings,TT_StringConstants<TChar>::DefaultWhitespaces,TT_StringConstants<xchar>::DefaultWhitespaces}.
/// @return A zero-terminated string of default whitespace characters.
inline ALIB_CPP14_CONSTEXPR XCString            XDefaultWhitespaces()  { return lib::strings::TT_StringConstants<xchar    >::DefaultWhitespaces(); }

} // namespace [aworx::lib::strings]


#if defined(_MSC_VER)
    #pragma warning( pop )
#endif
#endif // HPP_ALIB_STRINGS_CSTRING
