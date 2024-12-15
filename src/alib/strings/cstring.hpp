//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_CSTRING
#define HPP_ALIB_STRINGS_CSTRING 1
#pragma once
#include "alib/strings/string.hpp"

// conditional expression is constant for using our constant template parameters to select
// checking vs. non-checking method versions
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif

namespace alib {  namespace strings {

//==================================================================================================
/// This class specializes its base class \alib{strings;TString;String} in that respect that
/// the character strings represented are guaranteed to be zero-terminated.<br>
/// Zero-terminated strings are widely used by programming language \b C and are often called
/// "C-strings", what gave the class its name.
///
/// \see
///   For an introduction into the \alib string classes see this module's
///   \ref alib_mod_strings "Programmer's Manual".
///
/// @tparam TChar The \ref alib_characters_chars "character type" of this string.
//==================================================================================================
template<typename TChar>
class TCString : public TString<TChar>
{
protected:
    /// Shortcut to the base type.
    using base= TString<TChar>;

public:

    //==============================================================================================
    /// Default constructor creating a \ref alib_strings_details_nulled \e "nulled" c-string.
    //==============================================================================================
    constexpr
    TCString()
    : base()
    {}

    //==============================================================================================
    /// Constructor accepting a pointer to a character array and a string length.
    ///
    /// \note
    ///   It is a user's responsibility to ensure that the character array provided includes
    ///   a terminating \c '\0' character.<br>
    ///   In debug-compilations a run-time assertion is raised if the provided buffer is not
    ///   zero-terminated.
    ///
    ///
    /// @param pBuffer         The buffer to use.
    /// @param contentLength   The length of the content in the given buffer.
    //==============================================================================================
    constexpr
    explicit
    TCString( const TChar* pBuffer, integer contentLength )
    : base( pBuffer, contentLength )
    {
        #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        ALIB_ASSERT_ERROR(   base::IsNull()
                          || base::buffer[base::length] == '\0', "STRINGS",
             "Error: Explicit construction of CString with unterminated string."     )
                ALIB_WARNINGS_RESTORE
        #endif
    }


    #if DOXYGEN
        //==========================================================================================
        /// This templated constructor accepts various different kinds of source data.
        /// Unlike this documentation suggests, this constructor is internally implemented by a
        /// series of different constructors which are selected using template meta programming
        /// (i.e., \c std::enable_if).
        ///
        /// Together, the set of constructors provide maximum flexibility by allowing implicit
        /// construction with (and assignment of) any built-in or third-party character array type.
        /// Some of the constructors are defined using keyword \c explict.
        ///
        /// In debug-compilations a run-time assertion is raised if the provided buffer is not
        /// zero-terminated.
        ///
        /// \see
        ///   More information about construction of this type is provided with chapter
        ///   \ref alib_strings_cc_construction_cstring of the Programmer's Manual of module
        ///   \alib_strings.
        ///
        /// @tparam TZTCharArray  Type that comprises a zero-terminated character array.
        /// @param  src           The source object.
        //==========================================================================================
        template <typename TZTCharArray>
        inline
        constexpr
        TCString( const  TTerminatable& src );


        //==========================================================================================
        /// \b Implicit cast operator to objects of type \p{TZTCharArray}.<br>
        /// This operator is available for all custom types that have an accordingly specialized
        /// version of TMP struct \alib{characters;T_CharArray} and/or
        /// \alib{characters;T_ZTCharArray} defined.
        ///
        /// \see
        ///   More information about casting \alib string-types to built-in C++ types or custom
        ///   types is provided with chapter \ref alib_strings_cc_cast_cstring of the Programmer's
        ///   Manual of module \alib_strings.
        ///
        /// @tparam TZTCharArray The custom type to implicitly convert this object to.
        /// @return A value of custom string-type.
        //==========================================================================================
        template<typename TZTCharArray>
        inline
        operator TZTCharArray () const
        {}

        //==========================================================================================
        /// \b Explicit cast operator to objects of type \p{TZTCharArray}.<br>
        /// This operator is available for all custom types that have an accordingly specialized
        /// version of TMP struct \alib{characters;T_CharArray} and/or
        /// \alib{characters;T_ZTCharArray} defined.
        ///
        /// \see
        ///   More information about casting \alib string-types to built-in C++ types or custom
        ///   types is provided with chapter \ref alib_strings_cc_cast_cstring of the Programmer's
        ///   Manual of module \alib_strings.
        ///
        /// @tparam TZTCharArray The custom type to explicitly convert this object to.
        /// @return A value of custom string-type.
        //==========================================================================================
        template<typename TZTCharArray>
        inline explicit
        operator TZTCharArray () const
        {}

    #else // no doxygen now

        // ##################### Constructors selected with std::enable_if #########################
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE

        ATMP_SELECT_IF_1TP( typename T, std::is_same<std::nullptr_t,T>::value )
        constexpr
        TCString(const  T&   )
        : base( nullptr, 0 )
        {}

        template <typename T,
                  typename std::enable_if<    characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access
                                           == characters::AccessType::Implicit, int>::type = 0 >
        constexpr
        TCString(const T& src)
        : base( characters::T_ZTCharArray<ATMP_RCV(T), TChar>::Buffer(  src ),
                characters::T_ZTCharArray<ATMP_RCV(T), TChar>::Length(  src ) )
        {
            #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
                    ALIB_ASSERT_ERROR(   base::IsNull()
                                      || base::buffer[base::length] == '\0', "STRINGS",
                    "Error: Implicit construction of CString with unterminated string."     )
            #endif
        }

        ATMP_SELECT_IF_1TP( typename T,    characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access
                                        == characters::AccessType::Implicit )
        constexpr
        TCString(const T* src)
        : base( characters::T_ZTCharArray<ATMP_RCV(T), TChar>::Buffer(  *src ),
                characters::T_ZTCharArray<ATMP_RCV(T), TChar>::Length(  *src ) )
        {
            #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
            ALIB_ASSERT_ERROR(   base::IsNull()
                              || base::buffer[base::length] == '\0', "STRINGS",
                 "Error: Implicit construction of CString with unterminated string."     )
            #endif
        }

        ATMP_SELECT_IF_1TP( typename T,    characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access
                                        == characters::AccessType::ExplicitOnly )
        constexpr
        explicit
        TCString(      T& src)
        : base( characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Buffer( src ),
                characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Length( src ) )
        {
            #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
            ALIB_ASSERT_ERROR(   base::IsNull()
                              || base::buffer[base::length] == '\0', "STRINGS",
                 "Error: Explicit construction of CString with unterminated string."     )
            #endif
        }

        ATMP_SELECT_IF_1TP( typename T,    characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access
                                        == characters::AccessType::ExplicitOnly  )
        constexpr
        explicit
        TCString(      T* src)
        : base( characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Buffer( *src ),
                characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Length( *src ) )
        {
            #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
            ALIB_ASSERT_ERROR(   base::IsNull()
                              || base::buffer[base::length] == '\0', "STRINGS",
                 "Error: Explicit construction of CString with unterminated string."     )
            #endif
        }

        ATMP_SELECT_IF_1TP( typename T,    characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access
                                        == characters::AccessType::MutableOnly && !std::is_const<T>::value )
        constexpr
        explicit
        TCString(      T& src)
        : base( characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Buffer( const_cast<T&>( src ) ),
                characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Length( const_cast<T&>( src ) ) )
        {
            #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
            ALIB_ASSERT_ERROR(   base::IsNull()
                              || base::buffer[base::length] == '\0', "STRINGS",
                 "Error: Construction of CString (from mutable object) with unterminated string." )
            #endif
        }

        ATMP_SELECT_IF_1TP( typename T,    characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access
                                        == characters::AccessType::MutableOnly && !std::is_const<T>::value )
        constexpr
        explicit
        TCString(      T* src)
        : base( characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Buffer( *src ),
                characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Length( *src ) )
        {
            #if ALIB_DEBUG // needed to avoid empty {} in constexpr constructor
            ALIB_ASSERT_ERROR(   base::IsNull()
                              || base::buffer[base::length] == '\0', "STRINGS",
                 "Error: Construction of CString (from mutable object) with unterminated string." )
            #endif
        }

        // ##############################    casting  back    ######################################
        ATMP_SELECT_IF_1TP(typename T,       characters::T_CharArray<T,TChar>::Construction
                                          == characters::ConstructionType::Implicit
                                       && !T_SuppressAutoCast<TCString<TChar>,characters::ConstructionType::Implicit    ,ATMP_RCV(T)>::value)
        operator T () const
        {
            return  characters::T_CharArray<T,TChar>::Construct( base::buffer, base::length );
        }

        ATMP_SELECT_IF_1TP(typename T,       characters::T_CharArray<T,TChar>::Construction
                                          == characters::ConstructionType::ExplicitOnly
                                       && !T_SuppressAutoCast<TCString<TChar>,characters::ConstructionType::ExplicitOnly,ATMP_RCV(T)>::value)
        explicit
        operator T () const
        {
            return  characters::T_CharArray<T,TChar>::Construct( base::buffer, base::length );
        }


        ATMP_SELECT_IF_1TP(typename T,       characters::T_CharArray  <T,TChar>::Construction
                                          != characters::ConstructionType::Implicit
                                       &&    characters::T_ZTCharArray<T,TChar>::Construction
                                          == characters::ConstructionType::Implicit
                                       && !T_SuppressAutoCast<TCString<TChar>,characters::ConstructionType::Implicit    ,ATMP_RCV(T)>::value)
        operator T () const
        {
            return  characters::T_ZTCharArray<T,TChar>::Construct( base::buffer, base::length );
        }

        ATMP_SELECT_IF_1TP(typename T,       characters::T_CharArray  <T,TChar>::Construction
                                          != characters::ConstructionType::ExplicitOnly
                                       &&    characters::T_ZTCharArray<T,TChar>::Construction
                                          == characters::ConstructionType::ExplicitOnly
                                       && !T_SuppressAutoCast<TCString<TChar>,characters::ConstructionType::ExplicitOnly,ATMP_RCV(T)>::value)
        explicit
        operator T () const
        {
            return  characters::T_ZTCharArray<T,TChar>::Construct( base::buffer, base::length );
        }

    ALIB_WARNINGS_RESTORE
    #endif // doxygen

    //==============================================================================================
    /// Allocates memory including a termination character, copies the given string's contents
    /// and zero-terminates this string.
    ///
    /// \see
    ///   Methods #Allocate and #Free for information how to use allocated string objects.
    ///
    /// @tparam TAllocator The type of the given \p{allocator}, as prototyped with
    ///                    \alib{lang;Allocator}. Deduced by the compiler.
    /// @param  allocator  The allocator to use.
    /// @param  copy       The string to copy to the new memory allocated.
    //==============================================================================================
    template<typename TAllocator, typename TEnableIf= typename TAllocator::ChainedAllocator>
    TCString( TAllocator& allocator, const TString<TChar>& copy )
    {
        if( (base::length= copy.Length()) == 0 )
        {
            base::buffer= copy.Buffer();
            return;
        }

        auto* newBuf=  allocator().template AllocArray<TChar>( copy.Length() + 1);
        copy.CopyTo( newBuf );
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        newBuf[base::length]= '\0';
        ALIB_WARNINGS_RESTORE
        base::buffer= newBuf;
    }

    //==============================================================================================
    /// Reads a character at a given index.<br> Overrides
    /// \alib{strings;TString::operator[];String::operator[]} to change the debug assertion
    /// to allow inclusion of the termination character.
    ///
    /// \attention
    ///   No parameter check is performed (other than an assertions in debug-compilation of
    ///   \alib). See \alib{strings;TString::operator[];String::operator[]} for details.
    ///
    /// @param   op    The index of the character within this object's buffer.
    /// @returns If the character contained at index \p{op}.
    //==============================================================================================
     TChar    operator[] (integer  op) const
     {
        ALIB_ASSERT_ERROR( op >= 0  && op <= base::length, "STRINGS",
                           "Index out of bounds" )
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        return base::buffer[op];
        ALIB_WARNINGS_RESTORE
     }

    //==============================================================================================
    /// Returns the index of the first character which is included, respectively <em>not</em>
    /// included in a given set of characters.
    ///
    /// This method searches forwards. For backwards search, see
    /// \alib{strings;TString::LastIndexOfAny;String::LastIndexOfAny}.
    ///
    /// \note
    ///   This method overrides method \alib{strings;TString::IndexOfAny;String::IndexOf}.
    ///   This implementation however expects a \b %CString with parameter \p{needles}
    ///   (beside the fact that it has to be invoked on a \b %CString itself).
    ///   If no zero-terminated needle string is available, the parent's original method
    ///   needs to be invoked. This has to be done by explicitly naming
    ///   the parent class in the invocation, like for example in
    ///
    ///          myCString.TString::IndexOfAny<Inclusion::Include>( myString );
    ///
    /// \note
    ///   On most platforms, this zero-terminated version should perform slightly faster than
    ///   the original method in class \b %String.
    ///
    /// @tparam TCheck     Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                    If \c <false\> is added to the method name, no parameter checks are
    ///                    performed and the needles must not be empty.
    /// @tparam TInclusion Denotes whether the search returns the first index that holds a value
    ///                    that is included or that is not excluded in the set of needle
    ///                    characters.
    /// @param needles     Set of characters to be taken into account.
    /// @param startIdx    The index to start the search at. If the given value is less than \c 0,
    ///                    it is set to \c 0. If it exceeds the length of the string, the length of
    ///                    the string is returned.
    ///                    Defaults to \c 0.
    ///
    /// @return The index of the first character found which is included, respectively not
    ///         included, in the given set of characters. If nothing is found, \c -1 is returned.
    //==============================================================================================
    template <lang::Inclusion  TInclusion,
              typename         TCheck= CHK >
    integer  IndexOfAny( const TCString& needles, integer startIdx= 0 )
    const
    {
        if (TCheck::value)
        {
            if ( startIdx < 0                          ) startIdx= 0;
            if ( startIdx >= base::length ) return   -1;
        }
        else
        {
            ALIB_ASSERT_ERROR(    startIdx >= 0
                               && startIdx < base::length
                               && needles.Length() != 0, "STRINGS",
                               "Non checking and illegal parameters" )
        }


        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        if constexpr ( TInclusion == lang::Inclusion::Include )
        {
            integer idx=  characters::IndexOfAnyIncludedZT<TChar>(base::buffer + startIdx, needles);
            return  idx >= 0 ? idx + startIdx : -1;
        }
        else
        {
            const TChar* haystack= base::buffer + startIdx;
            integer idx=  characters::IndexOfAnyExcludedZT<TChar>( haystack, needles );
            return idx < 0 || *( haystack + idx ) == '\0' ? -1 : startIdx + idx;
        }
        ALIB_WARNINGS_RESTORE
    }

    //==============================================================================================
    /// Sets this object to a zero-terminated copy of the given string, allocated in
    /// given \p{allocator}.
    ///
    /// \note
    ///   In case given \p{copy} is empty or nulled, no allocation is performed and this string
    ///   is set to empty. Still the pointer to the buffer is copied. Thus, this string
    ///   behaves in respect to method #IsNull the same as given string \p{copy}.
    ///
    /// @tparam TAllocator The type of the given \p{allocator}, as prototyped with
    ///                    \alib{lang;Allocator}. Deduced by the compiler.
    /// @param  allocator  The allocator to use.
    /// @param  copy       The string to copy to the new memory allocated.
    //==============================================================================================
    template<typename TAllocator>
    void  Allocate( TAllocator& allocator, const TString<TChar>& copy )
    {
        if( (base::length= copy.Length()) == 0 )
        {
            base::buffer= copy.Buffer();
            return;
        }

        auto* newBuf=  allocator().template AllocArray<TChar>( base::length + 1);
        copy.CopyTo( newBuf );
        newBuf[base::length]= '\0';
        base::buffer= newBuf;
    }

    /// Deallocates this String's memory in \p{allocator} and sets this instance to \e nulled.
    /// \note
    ///   In case this instance is empty or nulled, no de-allocation is performed.
    ///   This should not happen with due usage. Due usage is:
    ///   - A string is allocated using method #Allocate on another instance.
    ///   - For the allocated string, this method is to be called, passing the same allocator.
    ///
    /// \note
    ///   Nevertheless, because of the fact that method #Allocate likewise does not allocate
    ///   in case an empty string was given, it is still good use to allocate and deallocate
    ///   empty strings.
    /// @tparam TAllocator The type of the given \p{allocator}, as prototyped with
    ///                    \alib{lang;Allocator}. Deduced by the compiler.
    /// @param  allocator  The allocator to use.
    template<typename TAllocator>
    void  Free( TAllocator& allocator )                                                    const
    {
        if( base::length == 0 || base::buffer == nullptr )
            return;
        allocator().FreeArray( base::buffer, base::length + 1 );
    }

}; // class TCString


//==================================================================================================
/// This template class has three specializations for types \alib{characters;nchar},
/// \alib{characters;wchar}, and \alib{characters;xchar}, which each provide the following
/// static \c constexpr methods:
/// - #EmptyString,
/// - #NewLine, and
/// - #DefaultWhitespaces.
///
/// The class is useful to implement methods that are templated with the character type they use.
///
/// \note
///   In non-templated code (that works with fixed or logical character sizes), it might lead to
///   better readability if the following shortcut/alias functions of namespace #alib are used:
///   - \ref EMPTY_CSTRING, \ref EMPTY_COMPLEMENT_CSTRING, \ref EMPTY_STRANGE_CSTRING,
///     \ref EMPTY_NCSTRING, \ref EMPTY_WCSTRING, \ref EMPTY_XCSTRING,
///   - \ref NEW_LINE, \ref COMPLEMENT_NEW_LINE, \ref STRANGE_NEW_LINE,
///     \ref NNEW_LINE, \ref WNEW_LINE, \ref XNEW_LINE and
///   - \ref DEFAULT_WHITESPACES, \ref COMPLEMENT_DEFAULT_WHITESPACES,
///     \ref STRANGE_DEFAULT_WHITESPACES, \ref NDEFAULT_WHITESPACES, \ref WDEFAULT_WHITESPACES,
///     \ref XDEFAULT_WHITESPACES.
///  <p>
///
/// \note
///   Constants for \e nulled strings are given with
///   \ref NULL_STRING, \ref NULL_COMPLEMENT_STRING, \ref NULL_STRANGE_STRING,
///   \ref NULL_NSTRING, \ref NULL_WSTRING and \ref NULL_XSTRING. However, those are returning
///   type \b String instead of \b CString. For \e nulled objects of type \b CString use the
///   keyword \c nullptr which performs implicit constexpr creation.
///
/// \see
///   See also manual chapter \ref alib_strings_details_constants.
///
/// @tparam TChar The \ref alib_characters_chars "character type".
//==================================================================================================
template<typename TChar> struct TT_CStringConstants
{
    #if DOXYGEN
    /// @return A \e zero-terminated empty string.
    constexpr inline static CString<TChar>     EmptyString();

    /// On Windows OS, the returned string contains characters <c>'\\r'</c> and <c>'\\n'</c>, on
    /// other platforms just character <c>'\\n'</c>.
    /// @return A \e zero-terminated string containing platform-dependent "newline" charcater(s).
    constexpr inline static CString<TChar>     NewLine;

    /// @return A zero-terminated string containing default whitespace characters "space", "newline"
   /// "carriage return" and "tabulator", hence <c>" \n\r\t"</c>.
    constexpr inline static CString<TChar>     DefaultWhitespaces());
    #endif
};


#if !DOXYGEN

template<> struct TT_CStringConstants<nchar>
{
    constexpr static NCString  EmptyString()        { return         ""        ; }

  #if defined(_WIN32)
    constexpr static NCString  NewLine()            { return         "\r\n"    ; }
  #else
    constexpr static NCString  NewLine()            { return         "\n"      ; }
  #endif

    constexpr static NCString  DefaultWhitespaces() { return         " \n\r\t" ; }
};

template<> struct TT_CStringConstants<wchar>
{
    constexpr static WCString  EmptyString()        { return A_WCHAR(""       ); }

  #if defined(_WIN32)
    constexpr static WCString  NewLine()            { return A_WCHAR("\r\n"   ); }
  #else
    constexpr static WCString  NewLine()            { return A_WCHAR("\n"     ); }
  #endif

    constexpr static WCString  DefaultWhitespaces() { return A_WCHAR(" \n\r\t"); }
};

template<> struct TT_CStringConstants<xchar>
{
    constexpr static XCString  EmptyString()        { return A_XCHAR(""       ); }

  #if defined(_WIN32)
    constexpr static XCString  NewLine()            { return A_XCHAR("\r\n"   ); }
  #else
    constexpr static XCString  NewLine()            { return A_XCHAR("\n"     ); }
  #endif

    constexpr static XCString  DefaultWhitespaces() { return A_XCHAR(" \n\r\t"); }
};

#endif  //!DOXYGEN

} // namespace alib[::strings]

/// A zero-terminated, empty string.
constexpr CString             EMPTY_CSTRING          = strings::TT_CStringConstants<character>::EmptyString();

/// A zero-terminated, empty string.
constexpr ComplementCString   EMPTY_COMPLEMENT_CSTRING= strings::TT_CStringConstants<complementChar>::EmptyString();

/// A zero-terminated, empty string.
constexpr StrangeCString      EMPTY_STRANGE_CSTRING   = strings::TT_CStringConstants<strangeChar>::EmptyString();

/// A zero-terminated, empty string.
constexpr NCString            EMPTY_NCSTRING         = strings::TT_CStringConstants<nchar    >::EmptyString();

/// A zero-terminated, empty string.
constexpr WCString            EMPTY_WCSTRING         = strings::TT_CStringConstants<wchar    >::EmptyString();

/// A zero-terminated, empty string.
constexpr XCString            EMPTY_XCSTRING         = strings::TT_CStringConstants<xchar    >::EmptyString();



/// A zero-terminated string containing the new-line character sequence.
constexpr  CString            NEW_LINE           =  strings::TT_CStringConstants<character     >::NewLine();

/// A zero-terminated string containing the new-line character sequence.
constexpr  ComplementCString  COMPLEMENT_NEW_LINE=  strings::TT_CStringConstants<complementChar>::NewLine();

/// A zero-terminated string containing the new-line character sequence.
constexpr  StrangeCString     STRANGE_NEW_LINE   =  strings::TT_CStringConstants<strangeChar   >::NewLine();

/// A zero-terminated string containing the new-line character sequence.
constexpr NCString            NNEW_LINE          =  strings::TT_CStringConstants<nchar         >::NewLine();

/// A zero-terminated string containing the new-line character sequence.
constexpr WCString            WNEW_LINE          =  strings::TT_CStringConstants<wchar         >::NewLine();

/// A zero-terminated string containing the new-line character sequence.
constexpr XCString            XNEW_LINE          =  strings::TT_CStringConstants<xchar         >::NewLine();



/// A zero-terminated string of default whitespace characters.
constexpr  CString            DEFAULT_WHITESPACES           =  strings::TT_CStringConstants<character     >::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
constexpr  ComplementCString  COMPLEMENT_DEFAULT_WHITESPACES =  strings::TT_CStringConstants<complementChar>::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
constexpr  StrangeCString     STRANGE_DEFAULT_WHITESPACES    =  strings::TT_CStringConstants<strangeChar   >::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
constexpr NCString            NDEFAULT_WHITESPACES          =  strings::TT_CStringConstants<nchar         >::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
constexpr WCString            WDEFAULT_WHITESPACES          =  strings::TT_CStringConstants<wchar         >::DefaultWhitespaces();

/// A zero-terminated string of default whitespace characters.
constexpr XCString            XDEFAULT_WHITESPACES          =  strings::TT_CStringConstants<xchar         >::DefaultWhitespaces();


} // namespace [alib]

#if defined(_MSC_VER)
    #pragma warning( pop )
#endif
#endif // HPP_ALIB_STRINGS_CSTRING

