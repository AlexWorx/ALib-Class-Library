//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_STRING
#define HPP_ALIB_STRINGS_STRING 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
#include "alib/strings/fwds.hpp"
#include "alib/characters/characters.hpp"
#include "alib/strings/chararraytraits.inl"
#include "alib/lang/tmp.hpp"
#include <iterator>


// conditional expression is constant for using our constant template parameters to select
// checking vs. non-checking method versions
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif

namespace alib { namespace strings {

/// The maximum length of an \alib string.
constexpr static integer  MAX_LEN = (std::numeric_limits<integer>::max)();

//==================================================================================================
/// This class is the base class of all \ref alib_strings_classes "ALib string classes".
/// Objects of this type represent character strings whose data is allocated outside their scope.
/// In particular, the class does not allocate a character array buffer to store and manipulate
/// string data.
///
/// Once constructed, objects of this class are immutable, except for the possibility to assign
/// a complete new object value.
/// This means there is no interface to change the single two data members #buffer and #length.
/// The immutable nature of this type is lifted by derived types. While class
/// \alib{strings;TSubstring;Substring}  allows  changing the start and
/// length of the string represented, class \alib{strings;TAString;AString} holds a copy of the
/// data and consequently allows modifying the string stored.<br>
///
/// \see
///   For an introduction into the \alib string classes see this module's
///   \ref alib_mod_strings "Programmer's Manual".
///
/// @tparam TChar The \ref alib_characters_chars "character type" of this string.
///         Alias names for specializations along the different character types
///         are provided in namespace #alib with type definitions
///         \alib{String},
///         \alib{NString},
///         \alib{WString},
///         \alib{XString},
///         \alib{ComplementString}, and
///         \alib{StrangeString}.
//==================================================================================================
template<typename TChar>
class TString
{

    public:
        /// Expose template parameter \p{TChar} to the outer world.
        using CharType= TChar;

    // #############################################################################################
    // Debug warnings
    // #############################################################################################
    public:
        //! @cond NO_DOX
            #if ALIB_DEBUG_STRINGS
                void     dbgCheck()   const;
            #endif
        //! @endcond

    // #############################################################################################
    // Private fields
    // #############################################################################################
    protected:
    #if !DOXYGEN

         union
         {
             const TChar*   buffer;
                   TChar*  vbuffer;
         };

    #else
            /// Pointer to an array of constant character values. This array holds the string that
            /// an instance of this type is representing.<br>
            /// Read access to this field is granted with method #Buffer.
            ///
            /// For technical reasons, this documentation unfortunaltely omits the important fact that
            /// this field is part of an anonymous union declared like this:
            ///
            ///      union
            ///      {
            ///          const TChar*   buffer;
            ///                TChar*  vbuffer;
            ///      };
            ///
            /// Hence, the field can also be interpreted as a pointer to an array of <b>non-constant</b>
            /// character values.
            /// Derived classes might use the sibling version \b vbuffer to modify the contents of
            /// the string if it is assured that such memory is writable.
            /// This is for example done extensively by the implementation of class
            /// \alib{strings;TAString;AString}.
            ///
            /// \note
            ///   It might not be considered pure/nice OO-design to prepare a feature of specialized
            ///   classes in a non-abstract base class, as done here. But the alternative would have
            ///   been to force derived classes to perform re-interpret casts or even worse tricks
            ///   to rightfully access a writeable buffer.
             const TChar*   buffer;

    #endif

        /// The length of the character string represented.
        /// Read access to this field is granted with method #Length.
        integer         length;

    //##############################################################################################
    // Constructors
    //##############################################################################################
     public:

        /// Defaulted default constructor
        constexpr TString()                                                      noexcept = default;

        /// Defaulted copy constructor
        constexpr TString(const TString&)                                        noexcept = default;

        /// Defaulted move constructor
        constexpr TString(     TString&&)                                        noexcept = default;

        /// Defaulted copy assignment operator.
        /// @return A reference to <c>this</c> instance.
                  TString& operator=(const TString&)                             noexcept = default;

        /// Defaulted move assignment operator.
        /// @return A reference to <c>this</c> instance.
                  TString& operator=(     TString&&)                             noexcept = default;


        //==========================================================================================
        /// Constructor accepting a pointer to a character array and a string length.
        ///
        /// @param pBuffer   Pointer to the start of the character string to represent.
        /// @param pLength   The length of the character string to represent.
        //==========================================================================================
        constexpr
        TString( const TChar* pBuffer, integer pLength )                                    noexcept
        : buffer(pBuffer)
        , length(pLength)
        {}

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
        /// \see
        ///   More information about string construction is provided with chapter
        ///   \ref alib_strings_cc_construction of the Programmer's Manual of module
        ///   \alib_strings.
        ///
        /// @tparam TCharArray  Type that comprises a character array.
        /// @param  src        The source object.
        //==========================================================================================
        template <typename TCharArray>
        inline
        constexpr
        TString(const  TCharArray& src );

        //==========================================================================================
        /// \b Implicit cast operator to objects of templated type \p{TCharArray}.<br>
        /// This operator is available for all custom types that have an accordingly specialized
        /// version of TMP struct \alib{characters;T_CharArray} defined.
        ///
        /// \see
        ///   More information about casting \alib string-types to built-in C++ types or custom
        ///   types is provided with chapter \ref alib_strings_cc_cast of the Programmer's
        ///   Manual of module \alib_strings.
        ///
        /// @tparam TCharArray The custom type to implicitly convert this object to.
        /// @return A value of custom string-type.
        //==========================================================================================
        template<typename TCharArray>
        inline
        operator TCharArray ()                                                                 const
        {}

        //==========================================================================================
        /// \b Explicit cast operator to objects of templated  type \p{TCharArray}.<br>
        /// This operator is available for all custom types that have an accordingly specialized
        /// version of TMP struct \alib{characters;T_CharArray} defined.
        ///
        /// \see
        ///   More information about casting \alib string-types to built-in C++ types or custom
        ///   types is provided with chapter \ref alib_strings_cc_cast of the Programmer's
        ///   Manual of module \alib_strings.
        ///
        /// @tparam TCharArray The custom type to explicitly convert this object to.
        /// @return A value of custom string-type.
        //==========================================================================================
        template<typename TCharArray>
        inline explicit
        operator TCharArray ()                                                                 const
        {}

        //==========================================================================================
        /// Allocates memory and copies the given string's contents.
        ///
        /// \see
        ///   Methods #Allocate and #Free for information how to use allocated string objects.
        ///
        /// @tparam TAllocator The type of the given \p{allocator}, as prototyped with
        ///                    \alib{lang;Allocator}. Deduced by the compiler.
        /// @param  allocator  The allocator to use.
        /// @param  copy       The string to copy to the new memory allocated.
        //==========================================================================================
        template<typename TAllocator, typename TEnableIf= typename TAllocator::ChainedAllocator>
        TString( TAllocator& allocator, const TString<TChar>& copy );

    #else // doxygen end

        ATMP_SELECT_IF_1TP( typename T,
            std::is_same<std::nullptr_t,T>::value )
        constexpr
        TString(const T&)                                                                   noexcept
        : buffer( nullptr )
        , length( 0       )
        {}

        template <typename T,
                  typename std::enable_if<(characters::T_CharArray<ATMP_RCV(T),TChar>::Access
                                        == characters::AccessType::Implicit), int>::type = 0 >
        constexpr
        TString(const T& src)                                                               noexcept
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
        operator T ()                                                                          const
        {
            return characters::T_CharArray<T,TChar>::Construct( buffer, length );
        }

        ATMP_SELECT_IF_1TP( typename T,
            characters::T_CharArray<T,TChar>::Construction == characters::ConstructionType::ExplicitOnly
        && !T_SuppressAutoCast<TString<TChar>,characters::ConstructionType::ExplicitOnly,ATMP_RCV(T)>::value)
        constexpr
        explicit
        operator T ()                                                                          const
        {
            return characters::T_CharArray<T,TChar>::Construct( buffer, length );
        }

        template<typename TAllocator>
        TString( TAllocator& allocator, const TString<TChar>& copy )
        {
            if( (length= copy.length) == 0 )
            {
                buffer= copy.buffer;
                return;
            }

            auto* newBuf=  allocator().template AllocArray<TChar>( copy.length);
            copy.CopyTo( newBuf );
            buffer= newBuf;
        }

    #endif // doxygen


    //##############################################################################################
    /// @name Buffer Access, Length and State
    //##############################################################################################

        //==========================================================================================
        /// Returns a pointer to the first character of the string we are representing.
        /// \note The string is not guaranteed to be zero-terminated.
        ///
        /// @return The start of the character array comprising the string represented by this
        ///         object.
        //==========================================================================================
        constexpr const TChar*  Buffer()            const                      { return buffer;    }

        //==========================================================================================
        /// Returns the length of the string that this object represents.
        ///
        /// @return The length of the string represented by this object.
        //==========================================================================================
        constexpr integer       Length()            const                      { return length;    }

        //==========================================================================================
        /// Returns the length of the string if represented as a wide character string.
        /// If template parameter \p{TChar} equals \c wchar, then this is identical with #Length.
        /// Otherwise the calculation is done using
        /// - <em>mbsnrtowcs()</em>
        ///   (without providing a conversion buffer) on glibc platforms (e.g., Linux)
        /// - <em>MultiByteToWideChar()</em>
        ///   (without providing a conversion buffer) on the Windows platform.
        ///
        /// If the conversion fails, \c -1 is returned.
        ///
        /// \note
        ///   On GNU/Linux and Mac OS, it might be necessary to invoke standard C method
        ///   <em>setlocale()</em> once, before using this method, to successfully calculate
        ///   the length.
        ///   This by default is done during \ref alib_manual_bootstrapping "library initialization",
        ///   if performed on class \alib{lang::basecamp;BaseCamp}.
        ///
        /// @return  The length of string when it was converted to wide characters.
        ///          If counting failed (what means that a corresponding conversion would also fail)
        ///          the #Length is returned.
        //==========================================================================================
        integer                 WStringLength()     const;


        //==========================================================================================
        /// Returns \c true if field #buffer equals \c nullptr, \c false otherwise.
        /// Note that a \e nulled string is also considered \ref IsEmpty "empty".
        ///
        /// \see
        ///   Details on the concept of \e nulled and \e empty strings are documented in chapter
        ///   \ref alib_strings_details_nulled_vsempty of this module's
        ///   \ref alib_mod_strings "Programmer's Manual".
        ///
        /// @return \c true if no buffer is allocated.
        //==========================================================================================
        constexpr bool          IsNull()            const              { return buffer == nullptr; }

        //==========================================================================================
        /// Returns \c true if field #buffer does not equal \c nullptr, \c false otherwise.
        ///
        /// @return The negated value of method #IsNull.
        //==========================================================================================
        constexpr bool          IsNotNull()         const              { return buffer != nullptr; }

        //==========================================================================================
        /// Returns \c true if this string is of zero length.
        /// Note that a \e nulled string is also considered empty.
        ///
        /// \see
        ///   Details on the concept of \e nulled and \e empty strings are documented in chapter
        ///   \ref alib_strings_details_nulled_vsempty of this module's
        ///   \ref alib_mod_strings "Programmer's Manual".
        /// @return \c true if the actual length equals zero.
        //==========================================================================================
        constexpr bool          IsEmpty()           const                    { return length == 0; }

        //==========================================================================================
        /// Returns \c true if this string has a length of \c 1 or more.
        /// @return \c true if the actual length does not equal zero.
        //==========================================================================================
        constexpr bool          IsNotEmpty()        const                    { return length != 0; }




        //==========================================================================================
        /// Returns a new string object representing a substring of the string that this object
        /// represents.
        ///
        /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                      If \c false is given, no range check is performed.
        /// @param  regionStart  The start of the substring within this string.
        /// @param  regionLength The length of the substring to return.
        ///                      Defaults to \alib{strings;MAX_LEN}.
        /// @return A string representing a region of this string.
        //==========================================================================================
        template <typename TCheck= CHK>
        TString<TChar>   Substring(integer regionStart, integer regionLength =MAX_LEN )        const
        {
            ALIB_STRING_DBG_CHK(this)

            if constexpr ( TCheck::value )
            {
                AdjustRegion( regionStart, regionLength );
            }
            else
            {
                #if ALIB_DEBUG
                    integer rs= regionStart;
                    integer rl= regionLength;
                    AdjustRegion( rs, rl );
                    ALIB_ASSERT_ERROR( rs == regionStart && rl == regionLength, "STRINGS",
                                       "Non checking and region out of range or empty" )
                #endif
            }

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            return TString<TChar>( buffer + regionStart, regionLength );
            ALIB_WARNINGS_RESTORE
        }

    //##############################################################################################
    /// @name Character Access
    //##############################################################################################

        //==========================================================================================
        /// Retrieves the character at the given index. A range check is performed. If this fails,
        /// \c '\0' is returned.
        ///
        /// @tparam TCheck  Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                 If \c false is given, no range check is performed.
        /// @param idx      The index of the character to read.
        /// @return The character at the given index, or '\0' if index out of range.
        //==========================================================================================
        template <typename TCheck= CHK>
        TChar        CharAt( integer idx )                                                     const
        {
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            if constexpr ( TCheck::value )
                return  ( idx >= 0 && idx < length ) ? *(buffer + idx )
                                                     : '\0' ;
            ALIB_ASSERT_ERROR( idx >= 0 && idx < length, "STRINGS",
                               "Non checking version: Index out of range" )

            return  *(buffer + idx );
            ALIB_WARNINGS_RESTORE
        }

        //==========================================================================================
        /// Retrieves the first character. In case of an empty or \e nulled string, '\0' is returned.
        ///
        /// @tparam TCheck  Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                 If \c false is given, no check for an empty string object is performed.
        /// @return The first character of the %String.
        ///         If this instance's length is zero,  '\0' is returned.
        //==========================================================================================
        template <typename TCheck= CHK>
        TChar        CharAtStart()                                                             const
        {
            if constexpr ( TCheck::value )
                return length > 0  ?  *(buffer)
                                   :  '\0';

            ALIB_ASSERT_ERROR( length > 0, "STRINGS", "Non checking invocation on empty string" )
            return  *(buffer);
        }


        //==========================================================================================
        /// Retrieves the last character. In case of an empty string, '\0' is returned.
        ///
        /// @tparam TCheck  Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                 If \c false is given, no check for an empty or \e nulled object is
        ///                 performed.
        ///
        /// @return The last character of the %String.
        ///         If this instance's length is zero,  '\0' is returned.
        //==========================================================================================
        template <typename TCheck= CHK>
        TChar        CharAtEnd()          const
        {
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                if constexpr ( TCheck::value )
                    return length > 0   ?  *(buffer + length - 1)
                                        : '\0';

                ALIB_ASSERT_ERROR( length > 0, "STRINGS", "Non checking invocation on empty string" )
                return  *(buffer + length - 1);
            ALIB_WARNINGS_RESTORE
        }

        //==========================================================================================
        /// Reads a character at a given index.
        ///
        /// \note
        ///   Unlike method #CharAt, this operator does <em>not</em> perform do range check on
        ///   parameter \p{idx}.
        ///   The rationale for this is that derived mutable types (e.g., class %AString),
        ///   may provide a mutable (non-<c>const</c>) version of this operator, returning a
        ///   a reference to the character to provide write access. Such reference
        ///   to a character could not be given if the index was out of range.
        ///   This way, a check in the derived type could likewise not be implemented.
        ///
        /// \note
        ///   As a result, this operator is equivalent to the non-checking version of method
        ///   #CharAt<\c false>. For safe access to characters in the buffer use #CharAt
        ///   (with template parameter \p{TCheck} being \alib{CHK}) which returns <c>'\0'</c> in the
        ///   case of that \p{idx} is out of bounds.
        ///
        /// \note
        ///   Still, in debug-compilations this operator raises an assertion if \p{idx} is out of
        ///   bounds.
        ///
        /// @param   idx    The index of the character within this object's buffer.
        /// @returns If the character contained at index \p{idx}.
        //==========================================================================================
         TChar    operator[] (integer idx)                                                     const
         {
             ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            ALIB_ASSERT_ERROR( idx >= 0  && idx < length, "STRINGS", "Index out of bounds" )
            return buffer[idx];
            ALIB_WARNINGS_RESTORE
         }

    //##############################################################################################
    /// @name Hashing
    //##############################################################################################
        //==========================================================================================
        /// Computes a hash number for the contained string.
        ///
        /// \note
        ///   If this library is compiled using C++ 17, internally this method is using
        ///   <c>std::hash<std::string_view<TChar>></c>. Otherwise a compatible hash function
        ///   is used.
        /// \see Alternative method #HashcodeIgnoreCase.
        ///
        /// @return  A hash number which is equal for two instances with the same content.
        //==========================================================================================
        std::size_t Hashcode()                                                                const;

        //==========================================================================================
        /// Computes a hash number for the contained string converted to upper case letters.
        ///
        /// \see Alternative method #Hashcode.
        ///
        /// @return  A hash number which is equal for two instances with have the same content
        ///          if converted to upper case letters.
        //==========================================================================================
        std::size_t HashcodeIgnoreCase()                                                      const;

    //##############################################################################################
    /// @name Comparison Methods
    //##############################################################################################
        //==========================================================================================
        /// Compares this string with a
        /// \ref alib_strings_cc_construction_string "string-like object".
        ///
        /// \c true is returned if this and the compared string are \e nulled or empty.
        /// If only one is \e nulled or empty, \c false is returned.
        ///
        /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                      If \c false is given, no check for a \e nulled is performed
        ///                      on this string as well as on \p{rhs}.
        /// @tparam TSensitivity Determines if comparison is case-sensitive (the default) or not.
        /// @param  rhs          The object to compare.
        ///
        /// @return  \c true, if contents of this string and the string representation of the
        ///          given \p{rhs} are equal.
        //==========================================================================================
        template <typename   TCheck       = CHK,
                  lang::Case TSensitivity = lang::Case::Sensitive>
        bool Equals( const TString<TChar>& rhs )                                               const
        {
            ALIB_STRING_DBG_CHK(this)

            if constexpr ( TCheck::value )
            {
                if ( IsNull() &&  rhs.IsNull() )
                    return true;

                if (  ( IsNull() !=  rhs.IsNull() )    )
                    return false;
            }
            #if ALIB_DEBUG
            else
            {   // we do not use IsNull() here, for not having a call noted in performance tools
                ALIB_ASSERT_ERROR(     buffer != nullptr , "STRINGS", "Non checking but this is nulled." )
                ALIB_ASSERT_ERROR( rhs.buffer != nullptr , "STRINGS", "Non checking but rhs is nulled." )
            }
            #endif

            if ( length != rhs.length )
                return false;

            if ( length == 0 )
                return true;

            if constexpr (TSensitivity == lang::Case::Sensitive )
                return  characters::Equal            ( buffer, rhs.buffer, length );
            else
                return  characters::CompareIgnoreCase( buffer, rhs.buffer, length ) == 0;
        }

        //==========================================================================================
        /// Compares this string with a
        /// \ref alib_strings_cc_construction_string "string-like object".
        ///
        /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                      If \c false is given, no check for a \e nulled object (this) is
        ///                      performed and this string must not be of zero length
        ///                      (while \p{rhs} might be of zero length).
        /// @tparam TSensitivity Determines if comparison is case-sensitive (the default) or not.
        /// @param  rhs          The object to compare.
        ///
        /// @return
        ///  -  0 if this and \p{rhs} are \e nulled or if both have a length of 0 or if both
        ///       share the same content
        ///  - <0 if this is \e nulled and \p{rhs} is not or if this is smaller than \p{rhs}.
        ///  - >0 if this is not \e nulled but \p{rhs} is or if this is greater than \p{rhs}.
        //==========================================================================================
        template <typename   TCheck       = CHK,
                  lang::Case TSensitivity = lang::Case::Sensitive>
        int CompareTo( const TString<TChar>& rhs )                                             const
        {
            ALIB_STRING_DBG_CHK(this)

            // check \c nullptr arguments
            if (TCheck::value &&     IsNull() )  return  rhs.IsNull() ? 0 : -1;
            if (TCheck::value && rhs.IsNull() )  return  +1;

            // zero length ?
            if ( TCheck::value && length == 0 )  return  rhs.length == 0 ? 0 : -1;
            if (              rhs.length == 0 )  return +1;

            bool    thisIsShorter=  ( length < rhs.length);
            integer shortLen  = thisIsShorter ? length : rhs.length;

            int cmpVal=  (TSensitivity == lang::Case::Sensitive)
                         ? characters::Compare          ( buffer, rhs.buffer, shortLen )
                         : characters::CompareIgnoreCase( buffer, rhs.buffer, shortLen );

            if ( cmpVal != 0 || length == rhs.length )
                return cmpVal;
            return thisIsShorter ? -1 : 1;
        }

        //==========================================================================================
        /// Compares this string with a region of another
        /// \ref alib_strings_cc_construction_string "string-like object".
        ///
        /// @tparam TCheck         Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                        If \c false is given, no check for a \e nulled comparison
        ///                        object is performed and this string must not be empty.
        ///                        Furthermore, no check is performed whether the given region
        ///                        fits to parameter \p{rhs}. This also means that the default
        ///                        value must not be used with <em>TCheck==\alib{NC}</em>.
        /// @tparam TSensitivity   Determines if comparison is case-sensitive (the default) or not.
        /// @param rhs             The string to compare this string with.
        /// @param rhsRegionStart  The start of the region in \p{rhs} to compare this object
        ///                        with.
        /// @param rhsRegionLength The length of the region in \p{rhs} to compare this object
        ///                        with.
        ///                        Defaults to \alib{strings;MAX_LEN}.
        ///
        /// @return
        ///  -  0 if this and \p{rhs} are \e nulled or if both have a length of 0 or if both
        ///       share the same content
        ///  - <0 if this is \e nulled and \p{rhs} is not or if this is smaller than \p{rhs}.
        ///  - >0 if this is not \e nulled but \p{rhs} is or if this is greater than \p{rhs}.
        //==========================================================================================
        template < typename   TCheck       = CHK,
                   lang::Case TSensitivity = lang::Case::Sensitive>
        int CompareTo(  const TString&  rhs,
                        integer         rhsRegionStart,
                        integer         rhsRegionLength   =MAX_LEN  )                          const
        {
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            if constexpr ( TCheck::value )
            {
                TString cmpSub( rhs.buffer, 0);
                rhs.AdjustRegion( rhsRegionStart, rhsRegionLength );
                cmpSub.buffer+=   rhsRegionStart;
                cmpSub.length=    rhsRegionLength;

                return CompareTo<CHK,  TSensitivity>( cmpSub );
            }
            else
                return CompareTo<NC, TSensitivity>( TString( rhs.buffer + rhsRegionStart,
                                                                rhsRegionLength ) );
            ALIB_WARNINGS_RESTORE
        }

        //==========================================================================================
        /// Compares a region of this object with a region of another
        /// \ref alib_strings_cc_construction_string "string-like object".
        ///
        /// @tparam TCheck         Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                        If \c false is given, no check for a \e nulled comparison
        ///                        object is performed and this string must not be empty.
        ///                        Furthermore, no check is performed whether the given regions fit
        ///                        to this object respectively the other region to the object given
        ///                        with parameter \p{rhs}.
        ///                        This also means that the default value of \p{regionLength} must
        ///                        not be used in this case.
        /// @tparam TSensitivity   Determines if comparison is case-sensitive (the default) or not.
        /// @param rhs             The string to compare this string with.
        /// @param rhsRegionStart  The start of the region in \p{rhs} to compare this object
        ///                        with.
        /// @param rhsRegionLength The length of the region in \p{rhs} to compare this object
        ///                        with.
        /// @param regionStart     The start of the region in this object to compare with
        /// @param regionLength    The length of the region in this object to compare with.
        ///                        Defaults to \alib{strings;MAX_LEN}.
        ///
        /// @return
        ///  -  0 if this and \p{rhs} are \e nulled or if both have a length of 0 or if both
        ///       share the same content
        ///  - <0 if this is \e nulled and \p{rhs} is not or if this is smaller than \p{rhs}.
        ///  - >0 if this is not \e nulled but \p{rhs} is or if this is greater than \p{rhs}.
        //==========================================================================================
        template < typename   TCheck       = CHK,
                   lang::Case TSensitivity = lang::Case::Sensitive>
        int CompareTo(  const TString&  rhs,
                        integer            rhsRegionStart,
                        integer            rhsRegionLength,
                        integer            regionStart,
                        integer            regionLength      =MAX_LEN  )                       const
        {
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            if constexpr ( TCheck::value )
            {
                TString cmpSub( rhs.buffer, 0);
                rhs.AdjustRegion( rhsRegionStart, rhsRegionLength );
                cmpSub.buffer+=   rhsRegionStart;
                cmpSub.length=    rhsRegionLength;

                AdjustRegion( regionStart, regionLength );
                return TString( buffer + regionStart, regionLength ).CompareTo<CHK, TSensitivity>( cmpSub );
            }

            return TString( buffer + regionStart, regionLength )
                    .CompareTo<NC, TSensitivity>( TString( rhs.buffer + rhsRegionStart,
                                                              rhsRegionLength )               );
            ALIB_WARNINGS_RESTORE
        }

        //==========================================================================================
        /// Returns \c true, if the contents of the given
        /// \ref alib_strings_cc_construction_string "string-like object" is found at the given
        /// position.
        ///
        /// \note
        ///   The following rules apply:
        ///   - If \p{pos} is out of range or \p{needle} is \e nulled, \c false is returned.
        ///     (This check only done if \p{TCheck} equals \alib{CHK}.)
        ///   - Otherwise, if the length of \p{needle} is 0, \c true is returned.
        ///
        /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                      If \c <false\> is given, no check on parameter
        ///                      \p{pos} is performed and \p{needle} must not be \e nulled.
        /// @tparam TSensitivity Determines if comparison is case-sensitive (the default) or not.
        /// @param needle        The string to compare with. If it is \ref IsEmpty "empty", \c true
        ///                      is returned.
        /// @param pos           The position to search for needle.
        /// @return \c true if \p{needle} is found at the given position. False otherwise.
        //==========================================================================================
        template< typename    TCheck       = CHK,
                  lang::Case  TSensitivity = lang::Case::Sensitive >
        bool ContainsAt( const TString& needle, integer pos )                                  const
        {
            integer needleLength= needle.length;
            ALIB_STRING_DBG_CHK(this)
            if constexpr ( TCheck::value )
            {
                if ( pos < 0 || pos + needleLength > length || needle.IsNull () )
                    return false;
                if ( needleLength == 0 )
                    return true;
            }
            else
            {
                ALIB_ASSERT_ERROR( pos >= 0 && pos + needleLength <= length && !needle.IsNull(),
                                   "STRINGS", "Non checking and index out of range" )
                ALIB_ASSERT_ERROR( needleLength != 0,
                                   "STRINGS", "Non checking and emtpy compare string" )
            }

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            return TSensitivity == lang::Case::Sensitive
                   ? characters::Equal            ( buffer + pos,  needle.buffer, needleLength )
                   : characters::CompareIgnoreCase( buffer + pos,  needle.buffer, needleLength ) == 0 ;
            ALIB_WARNINGS_RESTORE
        }

        //==========================================================================================
        /// Returns \c true, if this string starts with the contents of the
        /// \ref alib_strings_cc_construction_string "string-like object" given with parameter
        /// \p{needle}.
        /// In the special case that \p{needle} is \ref IsEmpty "empty", \c true is returned.
        ///
        /// @tparam TSensitivity Determines if comparison is case-sensitive (the default) or not.
        /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                      If \c <false\> is given, the given needle must not be empty
        ///                      and must not be longer than this string!
        ///                      \p{pos} is performed and \p{needle} must not be \e nulled.
        /// @param needle        The string to compare the start of this string with.
        ///                      If \e nulled or empty, \c true is returned.
        /// @return \c true if \p{needle} is found at the start of this string, \c false otherwise.
        //==========================================================================================
        template<typename    TCheck       = CHK,
                 lang::Case  TSensitivity =lang::Case::Sensitive>
        bool StartsWith( const TString& needle )                                               const
        {
            if constexpr ( TCheck::value )
            {
                if ( needle.length > length )
                    return false;
                if ( needle.length == 0 )
                    return true;
            }
            else
            {
                ALIB_ASSERT_ERROR( needle.length <= length, "STRINGS",
                                   "Non checking and needle longer than this string." )
                ALIB_ASSERT_ERROR( needle.length != 0     , "STRINGS",
                                   "Non checking and emtpy needle given." )
            }

            if constexpr ( TSensitivity == lang::Case::Sensitive )
                return  characters::Equal            ( buffer,  needle.buffer, needle.length );
            else
                return  characters::CompareIgnoreCase( buffer,  needle.buffer, needle.length ) == 0;
        }

        //==========================================================================================
        /// Returns \c true, if this string ends with the string found in parameter \p{needle}.
        /// If \p{needle} is \ref IsEmpty "empty", \c true is returned.
        ///
        /// @tparam TSensitivity Determines if comparison is case-sensitive (the default) or not.
        /// @param needle        The string to compare the end of this string with.
        ///                      If \e nulled or empty, \c true is returned.
        /// @return \c true if \p{needle} is found at the end of this, \c false otherwise.
        //==========================================================================================
        template<typename    TCheck       = CHK,
                 lang::Case  TSensitivity =lang::Case::Sensitive>
        bool EndsWith( const TString& needle )                                                 const
        {
            if constexpr ( TCheck::value )
            {
                if ( needle.length > length )
                    return false;
                if ( needle.length == 0 )
                    return true;
            }
            else
            {
                ALIB_ASSERT_ERROR( needle.length <= length, "STRINGS",
                                   "Non checking and needle longer than this string." )
                ALIB_ASSERT_ERROR( needle.length != 0     , "STRINGS",
                                   "Non checking and emtpy needle given." )
            }

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            if constexpr ( TSensitivity == lang::Case::Sensitive )
                return characters::Equal            ( buffer + length - needle.length,  needle.buffer, needle.length );
            else
                return characters::CompareIgnoreCase( buffer + length - needle.length,  needle.buffer, needle.length ) == 0;
            ALIB_WARNINGS_RESTORE
        }

    //##############################################################################################
    /// @name Search
    //##############################################################################################

        //==========================================================================================
        /// Searches a character starting from a given position.
        ///
        /// @tparam TCheck   Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                  If \c false is given, no range check is performed.
        /// @param needle    The character to search for.
        /// @param startIdx  The index in this to start searching the character.
        ///                  Defaults to \c 0.
        ///
        /// @return  \c -1 if the character \p{needle} is not found.
        ///          Otherwise the index of its first occurrence.
        //==========================================================================================
        template <typename TCheck= CHK>
        integer  IndexOf( TChar needle, integer startIdx = 0 )                                 const
        {
            ALIB_STRING_DBG_CHK(this)

            if constexpr ( TCheck::value )
            {
                // adjust range, if empty return -1
                     if ( startIdx <  0      )  startIdx= 0;
                else if ( startIdx >= length )  return -1;
            }
            else
            {
                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length, "STRINGS",
                                   "Non checking and index out of range" )
                ALIB_WARNINGS_RESTORE
            }

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            const TChar* result=  characters::Search( buffer + startIdx, length - startIdx, needle );
            ALIB_WARNINGS_RESTORE

            return result != nullptr ? result  -  buffer
                                     : -1;
        }

        //==========================================================================================
        /// Searches a character within a region of this.
        ///
        /// @tparam TCheck      Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                     If \c false is given, no range check is performed.
        /// @param needle       The character to search for.
        /// @param regionStart  The start of the region to search the character in.
        /// @param regionLength The length of the region to search the character in.
        /// @return  \c -1 if the character \p{needle} is not found.
        ///          Otherwise the index of its first occurrence.
        //==========================================================================================
        template <typename TCheck= CHK>
        integer         IndexOf( TChar needle, integer regionStart, integer regionLength )     const
        {
            ALIB_STRING_DBG_CHK(this)

            if constexpr ( TCheck::value )
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
                                       "STRINGS",  "Non checking and region out of range or empty" )
                #endif
            }

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            const TChar* result=  characters::Search( buffer + regionStart, regionLength, needle );
            ALIB_WARNINGS_RESTORE

            return result != nullptr ? result  -  buffer
                                     : -1;
        }

        //==========================================================================================
        /// Like \alib{strings::TString;IndexOf} but in case the character is not found, this
        /// method returns the length of this string instead of \c -1.
        /// Depending on the invocation context, the choice for the right version of this method may
        /// lead to shorter and more efficient code.
        ///
        /// @param needle  The character to search for.
        /// @return  This string's #Length if character \p{needle} is not found.
        ///          Otherwise the index of first occurrence.
        //==========================================================================================
        integer      IndexOfOrLength( TChar needle )                                           const
        {
            ALIB_STRING_DBG_CHK(this)
            const TChar* result=   characters::Search( buffer, length, needle );

            return result != nullptr ? result  -  buffer
                                     : length;
        }

        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        //==========================================================================================
        /// Like \alib{strings::TString;IndexOf} but in case the character is not found, this
        /// method returns the length of this string instead of \c -1.
        /// Depending on the invocation context, the choice for the right version of this method may
        /// lead to shorter and more efficient code.
        ///
        /// @tparam TCheck   Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                  If \c false is given, no range check is performed.
        /// @param needle    The character to search for.
        /// @param startIdx  The index in this to start searching the character.
        /// @return  This string's #Length if  character \p{needle} is not found.
        ///          Otherwise the index of first occurrence.
        //==========================================================================================
        template <typename TCheck= CHK>
        integer      IndexOfOrLength( TChar needle, integer startIdx )                         const
        {
            ALIB_STRING_DBG_CHK(this)
            if constexpr ( TCheck::value )
            {
                // adjust range, if empty return -1
                     if ( startIdx <  0      )  startIdx= 0;
                else if ( startIdx >= length )  return length;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length,
                                 "STRINGS", "Non checking and index out of range" )
            }

            const TChar* result= characters::Search( buffer + startIdx, length - startIdx, needle );
            return result != nullptr ? result  -  buffer
                                     : length;
        }
        ALIB_WARNINGS_RESTORE


        //==========================================================================================
        /// Searches a character starting backwards from the end or a given start index.
        ///
        /// @tparam TCheck      Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                     If \c false is given, no range check is performed.
        ///                     Consequently, in this case, optional parameter startIndex must be
        ///                     provided.
        ///
        /// @param needle       The character to search for.
        /// @param startIndex   The index within this string to start searching the character.
        ///                     Defaults to \alib{strings;MAX_LEN}.
        ///
        /// @return  \c -1 if the character \p{needle} is not found.
        ///          Otherwise, the index of its last occurrence.
        //==========================================================================================
        template <typename TCheck= CHK>
        integer    LastIndexOf( TChar needle, integer startIndex =MAX_LEN )                    const
        {
            ALIB_STRING_DBG_CHK(this)

            if constexpr ( TCheck::value )
            {
                // adjust range, if empty return -1
                if ( startIndex <  0      )   return -1;
                if ( startIndex >= length )   startIndex= length-1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIndex >= 0 && startIndex < length,
                                  "STRINGS", "Non checking and index out of range"   )
            }

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            while( startIndex >= 0 && buffer[ startIndex ] != needle )
                --startIndex;
            ALIB_WARNINGS_RESTORE

            return startIndex;
        }

        //==========================================================================================
        /// Returns the index of the first character which is included, respectively <em>not</em>
        /// included in a set of characters given as a
        /// \ref alib_strings_cc_construction_string "string-like object".
        ///
        /// \note
        ///   In derived class \b %CString, a faster version of this method (using \c std::strpbrk()
        ///   respectively \c std::strspn()) is available.
        ///   So, if performance is important and repetitive calls are performed, it might be
        ///   advisable to hold this string and the needles in a zero-terminated string buffer,
        ///   for example in an \b AString.
        ///
        /// This method searches forwards. For backwards search, see #LastIndexOf.
        ///
        /// @tparam TInclusion Denotes whether the search returns the first index that holds a value
        ///                    that is included or that is not excluded in the set of needle
        ///                    characters.
        /// @tparam TCheck     Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                    If \c <false\> is given, no parameter checks are performed.
        /// @param needles     Pointer to a zero-terminated set of characters to be taken into
        ///                    account.
        /// @param startIdx    The index to start the search at. If the given value is less than 0,
        ///                    it is set to 0. If it exceeds the length of the string, the length of
        ///                    the string is returned.
        ///                    Defaults to 0.
        ///
        /// @return The index of the first character found which is included, respectively not
        ///         included, in the given set of characters.
        ///         If nothing is found, -1 is returned.
        //==========================================================================================
        template <lang::Inclusion   TInclusion,
                  typename          TCheck      = CHK>
        integer    IndexOfAny( const TString& needles, integer startIdx= 0 )                   const
        {
            if constexpr ( TCheck::value )
            {
                if ( startIdx < 0       ) startIdx= 0;
                if ( startIdx >= length ) return   -1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length && needles.Length() != 0,
                                   "STRINGS", "Non checking and illegal parameters" )
            }


            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            integer idx= TInclusion == lang::Inclusion::Include
              ? characters::IndexOfAnyIncluded( buffer + startIdx,  length - startIdx, needles.Buffer(), needles.Length() )
              : characters::IndexOfAnyExcluded( buffer + startIdx,  length - startIdx, needles.Buffer(), needles.Length() );
            ALIB_WARNINGS_RESTORE

            return idx == -1 ? -1 : startIdx + idx;
        }

        //==========================================================================================
        /// Returns the index of the last character which is included, respectively <em>not</em>
        /// included in set of characters given as a
        /// \ref alib_strings_cc_construction_string "string-like object".
        ///
        /// This method searches backwards starting at the given index. For forwards search, see
        /// #IndexOfAny.
        ///
        /// @tparam TInclusion Denotes whether the search returns the first index that holds a value
        ///                    that is included or that is not excluded in the set of needle
        ///                    characters.
        /// @tparam TCheck     Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                    If \c <false\> is given, no parameter checks are performed.
        /// @param needles     Pointer to a zero-terminated set of characters to be searched for.
        /// @param startIdx    The index to start the search at. The value is cropped to be in
        ///                    the bounds of 0 and the length of this object minus one.
        ///                    Defaults to \alib{strings;MAX_LEN}.
        ///
        /// @return The index of the first character found which is included, respectively not
        ///         included, in the given set of characters.
        ///         If nothing is found, -1 is returned.
        //==========================================================================================
        template <lang::Inclusion TInclusion,
                  typename        TCheck = CHK>
        integer LastIndexOfAny( const TString& needles, integer startIdx =MAX_LEN )            const
        {
            if constexpr ( TCheck::value )
            {
                if ( startIdx < 0       ) return -1;
                if ( startIdx >= length ) startIdx=  length - 1;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length && needles.Length() != 0,
                                   "STRINGS", "Non checking and illegal parameters" )
            }

            if constexpr ( TInclusion == lang::Inclusion::Include )
                return characters::LastIndexOfAnyInclude(  Buffer(), startIdx, needles.Buffer(), needles.Length() );
            else
                return characters::LastIndexOfAnyExclude(  Buffer(), startIdx, needles.Buffer(), needles.Length() );
        }

        //==========================================================================================
        /// Searches the given \ref alib_strings_cc_construction_string "string-like object"
        /// in this string.
        ///
        /// If \p{needle} is empty, the adjusted value of \p{startIdx} is returned.
        ///
        /// @tparam TSensitivity   Case sensitivity of the comparison.
        ///                        Optional and defaults to \b Case::Sensitive.
        /// @tparam TCheck         Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                        If \c false is given, parameter \p{needle} must not be empty and
        ///                        \p{startIdx} must be in the range of [0 ... #Length() - needle.Length()].
        ///                        This also implies that this string must not be empty.
        /// @param needle          The string to search for.
        /// @param startIdx        The index to start the search at. Optional and defaults to \c 0.
        /// @param endIdx          The index where the search ends. Precisely, the index of the
        ///                        first character that is not found.
        ///                        Defaults to \alib{strings;MAX_LEN}.
        ///
        /// @return    If the checking of parameters failed or the string is not found, \c -1 is
        ///            returned. Otherwise the index of the first occurrence of \p{needle}.
        //==========================================================================================
        template<typename   TCheck        = CHK,
                 lang::Case TSensitivity  = lang::Case::Sensitive>
        integer  IndexOf( const TString&  needle,
                          integer         startIdx= 0,
                          integer         endIdx  = strings::MAX_LEN  )                        const
        {
            if constexpr ( TCheck::value )
            {
                if ( needle.IsNull()  )
                    return  -1;
                if ( startIdx < 0                        )    startIdx= 0;
                endIdx= (std::min) (endIdx, length - needle.Length() + 1 );
                if ( startIdx >= endIdx                  )    return -1;
            }
            else
            {
                // this default parameter is still corrected. Optimized out anyhow.
                if( endIdx  == strings::MAX_LEN)
                    endIdx= length - needle.Length() + 1;

                ALIB_ASSERT_ERROR(    startIdx >= 0 && startIdx <= length
                                   &&                    endIdx <= length
                                   && needle.IsNotNull(),
                                   "STRINGS", "Non checking and illegal parameters" )
            }


            return indexOfString<TSensitivity>(needle, startIdx, endIdx);
        }

        //==========================================================================================
        /// Searches the first difference of a substring of this string and a
        /// \ref alib_strings_cc_construction_string "string-like object" given with parameter
        /// \p{needle}.
        /// If no difference is found, then the index of the first character behind the substring
        /// is returned.
        ///
        /// @tparam TCheck      Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                     If \c false is given, no range check is performed.
        /// @param needle       The substring to search for.
        /// @param sensitivity  Letter case sensitivity of the comparison.
        ///                     Optional and defaults to \b Case::Sensitive.
        /// @param startIdx     The index in this string to start the comparison with \p{needle}.
        ///                     Optional and defaults to \c 0.
        ///
        /// @return The index of the first difference found or \p{idx} plus the length of \p{needle}.
        //==========================================================================================
        template <typename TCheck= CHK>
        integer  IndexOfFirstDifference( const TString&  needle,
                                         lang::Case      sensitivity = lang::Case::Sensitive,
                                         integer         startIdx    = 0                     ) const
        {
            ALIB_STRING_DBG_CHK(this)

            if constexpr ( TCheck::value )
            {
                // adjust range, if empty return -1
                     if ( startIdx <  0      )  startIdx= 0;
                else if ( startIdx >= length )  return startIdx;
            }
            else
            {
                ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx < length,
                                   "STRINGS", "Non checking and index out of range" )
            }

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            return   characters::IndexOfFirstDifference( buffer + startIdx,   length - startIdx,
                                                                           needle.buffer,  needle.length,
                                                                           sensitivity                       );
            ALIB_WARNINGS_RESTORE
        }

        //==========================================================================================
        /// The method searches the next matching \p{closer}-character while taking nested pairs of
        /// \p{opener} and \p{closer} characters into account.
        ///
        /// Before the invocation of this method, the initial \p{opener} has to be known already
        /// and the given \p{idx} has to point to the first character behind the opener, where the
        /// search for an according \p{closer} is to be started.
        ///
        /// This method is useful to scan a string for pairs of opening and closing brackets, while
        /// the found segment may contain nested pairs of the same brackets.
        ///
        /// @param opener The character that represents the opening bracket, e.g., <c>'{'</c>.
        /// @param closer The character that represents the closing bracket, e.g., <c>'}'</c>.
        /// @param idx    Index pointing to first character behind the (first) \p{opener}.
        ///
        /// @return   The index of the corresponding closing character. If none was found, a negative
        ///           value is returned.
        ///           In the latter case the negated (absolute) value is indicating the number of
        ///           still open (nested) brackets.
        //==========================================================================================
        integer  IndexOfSegmentEnd( TChar opener, TChar closer, integer idx )                 const;


        //==========================================================================================
        /// Counts all occurrences of character \p{needle} in the range from \p{startPos} to the end
        /// of the string.
        ///
        /// For empty strings \p{needle}, \c 0 is returned.
        ///
        /// @tparam TCheck   Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                  If \c false is given, no range check is performed.
        /// @param needle    The character to search for.
        /// @param startPos  The index to start the counting.
        ///                  Optional and defaults to \c 0.
        ///
        /// @return  The index of the first difference in \p{needle}.
        //==========================================================================================
        template <typename TCheck= CHK>
        integer  CountChar( TChar           needle,
                            integer         startPos        = 0   )                            const
        {
            ALIB_STRING_DBG_CHK(this)
            if constexpr ( TCheck::value )
            {
                // adjust range, if empty return -1
                     if ( startPos <  0      )  startPos= 0;
                else if ( startPos >= length )  return 0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   "STRINGS", "Non checking and index out of range" )
            }


            int     result= 0;
            while( startPos < length && (startPos= IndexOf<NC>( needle, startPos )) >= 0 )
            {
                ++startPos;
                ++result;
            }

            return result;
        }

        //==========================================================================================
        /// Counts all occurrences of character \p{needle}, unless followed by character \p{omit}
        /// in the range from \p{startPos} to the end of the string.
        ///
        /// For empty strings \p{needle}, \c 0 is returned.
        /// Also, for empty strings \p{omit}, \c 0 is returned.
        ///
        /// @tparam TCheck   Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                  If \c false is given, no range check is performed.
        /// @param needle    The character to search for.
        /// @param omit      Omit occurrence if the given character follows.
        /// @param startPos  The index to start the counting.
        ///
        /// @return  The index of the first difference in \p{needle}.
        //==========================================================================================
        template <typename TCheck= CHK>
        integer  CountChar( TChar           needle,
                            TChar           omit,
                            integer         startPos  )                                        const
        {
            ALIB_STRING_DBG_CHK(this)
            if constexpr ( TCheck::value )
            {
                // adjust range, if empty return -1
                     if ( startPos <  0      )  startPos= 0;
                else if ( startPos >= length )  return 0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   "STRINGS", "Non checking and index out of range" )
            }

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            int     result= 0;
            while( startPos < length && (startPos= IndexOf<NC>( needle, startPos )) >= 0 )
            {
                ++startPos;
                if( startPos < Length()  &&  *(buffer + startPos) == omit  )
                    continue;

                ++result;
            }
            ALIB_WARNINGS_RESTORE

            return result;
        }

        //==========================================================================================
        /// Counts all occurrences of \p{needle} from \p{startPos} to the end of the string.
        ///
        /// For empty strings \p{needle}, \c 0 is returned.
        ///
        /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                      If \c false is given, parameter \p{startIdx} must be valid and
        ///                      \p{needle} must not be empty.
        /// @tparam TSensitivity Case sensitivity of the comparison.
        ///                      Optional and defaults to \b Case::Sensitive.
        /// @param needle        The string to search for.
        /// @param startPos      The index to start the counting.
        ///                      Optional and defaults to \c 0.
        ///
        /// @return  The index of the first difference in \p{needle}.
        //==========================================================================================
        template< typename    TCheck       = CHK,
                  lang::Case  TSensitivity = lang::Case::Sensitive >
        integer Count( const TString&    needle,
                       integer           startPos   = 0 )                                      const
        {
            ALIB_STRING_DBG_CHK(this)
            integer nLen= needle.Length();
            if( nLen == 0  )
                return 0;
            if constexpr ( TCheck::value )
            {
                if ( startPos < 0 )                startPos= 0;
                if ( startPos + nLen > length )    return  0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   "STRINGS", "Non checking and illegal parameters" )
            }

            int     result= 0;
            while( (startPos= IndexOf<NC, TSensitivity>( needle, startPos )) >= 0 )
            {
                startPos+= needle.Length();
                ++result;
            }

            return result;
        }

        //==========================================================================================
        /// Counts all occurrences of \p{needle}, unless followed by \p{omit}, starting at
        /// \p{startPos} to the end of the string.
        ///
        /// For empty strings \p{needle}, \c 0 is returned.
        /// Also, for empty strings \p{omit}, \c 0 is returned.
        ///
        /// @tparam TSensitivity Case sensitivity of the comparison.
        ///                      Optional and defaults to \b Case::Sensitive.
        /// @tparam TCheck       Defaults to \alib{CHK}, which is the normal invocation mode.
        ///                      If \c false is given, parameter \p{startPos} must be valid and
        ///                      \p{needle} must not be empty.
        /// @param needle        The string to search for.
        /// @param omit          Omit occurrence if the given string follows.
        /// @param startPos      The index to start the counting.
        ///                      Optional and defaults to \c 0.
        ///
        /// @return  The index of the first difference in \p{needle}.
        //==========================================================================================
        template<typename    TCheck        = CHK,
                 lang::Case  TSensitivity  = lang::Case::Sensitive>
        integer  Count( const TString&   needle,
                        const TString&   omit,
                        integer          startPos        = 0         )                         const
        {
            ALIB_STRING_DBG_CHK(this)
            integer nLen= needle.Length();
            if ( nLen == 0  )
                return  0;
            if constexpr ( TCheck::value )
            {
                if ( startPos < 0 )                startPos= 0;
                if ( startPos + nLen > length )    return  0;
            }
            else
            {
                ALIB_ASSERT_ERROR( startPos >= 0 && startPos < length,
                                   "STRINGS", "Non checking and illegal parameters" )
            }


            int     result= 0;
            while( (startPos= IndexOf<NC , TSensitivity>( needle, startPos )) >= 0 )
            {
                startPos+= nLen;
                if(    startPos + omit.Length() <= Length()
                    && (    omit.IsEmpty()
                         || ContainsAt<NC>( omit, startPos ) )    )
                    continue;

                ++result;
            }

            return result;
        }

    //##############################################################################################
    /// @name Parsing Numbers
    //##############################################################################################

        //==========================================================================================
        /// Parses an integral value consisting of characters \c '0' to \c '9' from this string.
        /// <br>Unlike with #ParseInt or #ParseDec, no sign, whitespaces or group characters are
        /// accepted.
        ///
        /// @param startIdx     The start index from where the integral value is tried to be parsed.
        ///                     Optional and defaults to \c 0.
        /// @param[out] newIdx  Optional output variable that will point to the first character
        ///                     in this string after the float number that was parsed.
        ///                     If parsing fails, it will be set to the value of parameter startIdx.
        ///                     Therefore, this parameter can be used to check if a value was found.
        ///
        /// @return  The parsed value. In addition, the parameter \p{newIdx} is set to point
        ///          to the first character behind any found integer number.
        //==========================================================================================
        ALIB_API
        uint64_t  ParseDecDigits( integer startIdx =0, integer* newIdx= nullptr ) const;


        //==========================================================================================
        /// Parses an integral value in decimal, binary, hexadecimal or octal format from
        /// the string
        ///
        /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
        /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
        /// which is configured to not using - and therefore also not parsing - grouping characters.
        ///
        /// Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
        /// If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
        /// read.
        ///
        /// For more information on number conversion, see class
        /// \alib{strings;TNumberFormat;NumberFormat}. All of its interface methods
        /// have a corresponding implementation within class \b %AString.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        ALIB_API
        int64_t  ParseInt( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                           integer* newIdx= nullptr ) const;

        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseInt(integer,TNumberFormat<TChar>*,integer*)const;ParseInt}
        /// providing default values for omitted parameters.
        ///
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        int64_t  ParseInt( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseInt( 0, numberFormat, newIdx );
        }

        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseInt(integer,TNumberFormat<TChar>*,integer*)const;ParseInt}
        /// providing default values for omitted parameters.
        ///
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        int64_t  ParseInt( integer* newIdx ) const
        {
            return ParseInt( 0, nullptr, newIdx );
        }


        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseInt(integer,TNumberFormat<TChar>*,integer*)const;ParseInt}
        /// providing default values for omitted parameters.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        int64_t  ParseInt( integer startIdx, integer* newIdx ) const
        {
            return ParseInt( startIdx, nullptr, newIdx );
        }

        //==========================================================================================
        /// Reads an unsigned 64-bit integer in standard decimal format at the given position
        /// from this %AString.
        ///
        /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
        /// \alib{strings;TNumberFormat::Computational}
        /// which is configured to not using - and therefore also not parsing - grouping characters.
        ///
        /// Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
        /// If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
        /// read.
        ///
        /// Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail.
        /// For reading signed integral values, see methods #ParseInt, for floating point numbers
        /// #ParseFloat.
        ///
        /// For more information on number conversion, see class
        /// \alib{strings;TNumberFormat;NumberFormat}. All number-parsing interface methods
        /// have a corresponding implementation within this class.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        ALIB_API
        uint64_t  ParseDec( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;

        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseDec(integer,TNumberFormat<TChar>*,integer*)const;ParseDec}
        /// providing default values for omitted parameters.
        ///
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseDec( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseDec( 0, numberFormat, newIdx );
        }

        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseDec(integer,TNumberFormat<TChar>*,integer*)const;ParseDec}
        /// providing default values for omitted parameters.
        ///
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseDec( integer* newIdx ) const
        {
            return ParseDec( 0, nullptr, newIdx );
        }


        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseDec(integer,TNumberFormat<TChar>*,integer*)const;ParseDec}
        /// providing default values for omitted parameters.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseDec( integer startIdx, integer* newIdx ) const
        {
            return ParseDec( startIdx, nullptr, newIdx );
        }

        //==========================================================================================
        /// Reads an unsigned 64-bit integer in binary format at the given position
        /// from this string.
        ///
        /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
        /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
        /// which is configured to not using - and therefore also not parsing - grouping characters.
        ///
        /// Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
        /// If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
        /// read.
        ///
        /// For more information on number conversion, see class
        /// \alib{strings;TNumberFormat;NumberFormat}. All number-parsing interface methods
        /// have a corresponding implementation within this class.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        ALIB_API
        uint64_t  ParseBin( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;


        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseBin(integer,TNumberFormat<TChar>*,integer*)const;ParseBin}
        /// providing default values for omitted parameters.
        ///
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseBin( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseBin( 0, numberFormat, newIdx );
        }

        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseBin(integer,TNumberFormat<TChar>*,integer*)const;ParseBin}
        /// providing default values for omitted parameters.
        ///
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseBin( integer* newIdx ) const
        {
            return ParseBin( 0, nullptr, newIdx );
        }


        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseBin(integer,TNumberFormat<TChar>*,integer*)const;ParseBin}
        /// providing default values for omitted parameters.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseBin( integer startIdx, integer* newIdx ) const
        {
            return ParseBin( startIdx, nullptr, newIdx );
        }

        //==========================================================================================
        /// Reads an unsigned 64-bit integer in hexadecimal format at the given position
        /// from this string.
        ///
        /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
        /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
        /// which is configured to not using - and therefore also not parsing - grouping characters.
        ///
        /// Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
        /// If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
        /// read.
        ///
        /// For more information on number conversion, see class
        /// \alib{strings;TNumberFormat;NumberFormat}. All number-parsing interface methods
        /// have a corresponding implementation within this class.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        ALIB_API
        uint64_t  ParseHex( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;


        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseHex(integer,TNumberFormat<TChar>*,integer*)const;ParseHex}
        /// providing default values for omitted parameters.
        ///
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseHex( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseHex( 0, numberFormat, newIdx );
        }

        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseHex(integer,TNumberFormat<TChar>*,integer*)const;ParseHex}
        /// providing default values for omitted parameters.
        ///
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseHex( integer* newIdx ) const
        {
            return ParseHex( 0, nullptr, newIdx );
        }


        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseHex(integer,TNumberFormat<TChar>*,integer*)const;ParseHex}
        /// providing default values for omitted parameters.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseHex( integer startIdx, integer* newIdx ) const
        {
            return ParseHex( startIdx, nullptr, newIdx );
        }

        //==========================================================================================
        /// Reads an unsigned 64-bit integer in octal format at the given position
        /// from this string.
        ///
        /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
        /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
        /// which is configured to not using - and therefore also not parsing - grouping characters.
        ///
        /// Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
        /// If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
        /// read.
        ///
        /// For more information on number conversion, see class
        /// \alib{strings;TNumberFormat;NumberFormat}. All number-parsing interface methods
        /// have a corresponding implementation within this class.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        ALIB_API
        uint64_t  ParseOct( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                            integer* newIdx= nullptr ) const;


        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseOct(integer,TNumberFormat<TChar>*,integer*)const;ParseOct}
        /// providing default values for omitted parameters.
        ///
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseOct( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseOct( 0, numberFormat, newIdx );
        }

        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseOct(integer,TNumberFormat<TChar>*,integer*)const;ParseOct}
        /// providing default values for omitted parameters.
        ///
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseOct( integer* newIdx ) const
        {
            return ParseOct( 0, nullptr, newIdx );
        }


        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseOct(integer,TNumberFormat<TChar>*,integer*)const;ParseOct}
        /// providing default values for omitted parameters.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        uint64_t  ParseOct( integer startIdx, integer* newIdx ) const
        {
            return ParseOct( startIdx, nullptr, newIdx );
        }

        //==========================================================================================
        /// Reads a floating point number at the given position from this string.
        ///
        /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
        /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
        /// which is configured to 'international' settings (not using the locale) and therefore
        /// also not parsing grouping characters.
        ///
        /// Optional output parameter \p{newIdx} may be used to detect if parsing was successful.
        /// If not, it receives the value of \p{startIdx}, even if leading whitespaces had been
        /// read.
        ///
        /// For more information on number conversion, see class
        /// \alib{strings;TNumberFormat;NumberFormat}. All number-parsing interface methods
        /// have a corresponding implementation within this class.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        ALIB_API
        double   ParseFloat( integer startIdx =0, TNumberFormat<TChar>* numberFormat= nullptr,
                             integer* newIdx= nullptr ) const;

        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseFloat(integer,TNumberFormat<TChar>*,integer*)const;ParseFloat}
        /// providing default values for omitted parameters.
        ///
        /// @param numberFormat The format definition. Defaults to \c nullptr.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        double  ParseFloat( TNumberFormat<TChar>* numberFormat, integer* newIdx= nullptr ) const
        {
            return ParseFloat( 0, numberFormat, newIdx );
        }

        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseFloat(integer,TNumberFormat<TChar>*,integer*)const;ParseFloat}
        /// providing default values for omitted parameters.
        ///
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \c 0.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        double  ParseFloat( integer* newIdx ) const
        {
            return ParseFloat( 0, nullptr, newIdx );
        }


        //==========================================================================================
        /// Overloaded version of
        /// \alib{strings;TString::ParseFloat(integer,TNumberFormat<TChar>*,integer*)const;ParseFloat}
        /// providing default values for omitted parameters.
        ///
        /// @param startIdx     The start index for parsing.
        ///                     Optional and defaults to \c 0.
        /// @param[out] newIdx  Optional output variable that will point to the first
        ///                     character in this string after the number parsed.
        ///                     On failure, it will be set to the initial value \p{startIdx}.
        /// @return  The parsed value. In addition, the output parameter \b newIdx is set to
        ///          point to the first character behind the parsed number.
        //==========================================================================================
        double  ParseFloat( integer startIdx, integer* newIdx ) const
        {
            return ParseFloat( startIdx, nullptr, newIdx );
        }

    //##############################################################################################
    /// @name Conversion
    //##############################################################################################

        //==========================================================================================
        /// Copies this string's contents into a given character buffer.
        /// It is the caller's responsibility that \p{dest} is large enough, write-enabled, etc.
        ///
        /// @param dest   The destination buffer.
        /// @return    The length of this string.
        //==========================================================================================
        integer CopyTo( TChar* dest )                                                          const
        {
            characters::Copy( buffer, length, dest );
            return length;
        }

        //==========================================================================================
        /// Sets this String to a copy of the given string, allocated in given \p{allocator}.
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
        //==========================================================================================
        template<typename TAllocator>
        void  Allocate( TAllocator& allocator, const TString<TChar>& copy )
        {
            if( (length= copy.length) == 0 )
            {
                buffer= copy.buffer;
                return;
            }

            auto* newBuf=  allocator().template AllocArray<TChar>( copy.length );
            copy.CopyTo( newBuf );
            buffer= newBuf;
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
        void  Free( TAllocator& allocator )
        {
            if( length == 0 || buffer == nullptr )
                return;
            allocator().FreeArray( buffer, length );
        }

    //##############################################################################################
    /// @name Helper Methods
    //##############################################################################################
    public:
        //==========================================================================================
        /// Adjusts a region given as in/out parameters, to fit to this object's range [0..length].
        ///
        /// @param[in,out] regionStart  The proposed region start which might get adjusted to fit to
        ///                             range [0..length].
        /// @param[in,out] regionLength The proposed region length which might get adjusted to fit to
        ///                             range [0..length].
        ///
        /// @return   Returns \c true, if the adjusted region is empty.
        //==========================================================================================
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

    //##############################################################################################
    // std::iterator_traits
    //##############################################################################################
    public:
        //==========================================================================================
        /// Implementation of \c std::iterator_traits for class \b %TString and its descendents.
        /// Base class \b String exposes #ConstIterator which uses
        /// <c>const TChar*</c> and <c>const TChar&</c> for template types \p{TPointer} and
        /// \p{TReference}. Descendant classes may expose a mutable
        /// version (e.g., \alib{strings;TAString;AString}).
        ///
        /// As the name of the class indicates, this iterator satisfies the C++ standard library
        /// concept
        /// \https{RandomAccessIterator,en.cppreference.com/w/cpp/concept/RandomAccessIterator}.
        //==========================================================================================
        template<typename TCharConstOrMutable>
        class TRandomAccessIterator
        {
            public:
                using iterator_category = std::random_access_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
                using value_type        = TCharConstOrMutable;              ///< Implementation of <c>std::iterator_traits</c>.
                using difference_type   = integer;                          ///< Implementation of <c>std::iterator_traits</c>.
                using pointer           = TCharConstOrMutable*;             ///< Implementation of <c>std::iterator_traits</c>.
                using reference         = TCharConstOrMutable&;             ///< Implementation of <c>std::iterator_traits</c>.


            protected:
                /// The pointer into the buffer is all we store.
                TCharConstOrMutable* p;
            public:
                /// Constructor.
                /// @param start Pointer to the initial character.
                explicit TRandomAccessIterator( TCharConstOrMutable* start = nullptr ) : p(start)
                {}

            //######################   To satisfy concept of  InputIterator   ######################

                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                /// Prefix increment operator.
                /// @return A reference to this object.
                TRandomAccessIterator& operator++()
                {
                    ++p;
                    return *this;
                }

                 /// Postfix increment operator.
                /// @return An iterator value that is not increased, yet.
               TRandomAccessIterator operator++(int)
                {
                    return TRandomAccessIterator(p++);
                }

                /// Comparison operator.
                /// @param other  The iterator to compare ourselves to.
                /// @return \c true if this and the given iterator are pointing to the same character
                ///         in the same array, \c false otherwise.
                bool operator==(TRandomAccessIterator other)                                   const
                {
                    return p == other.p;
                }

                /// Comparison operator.
                /// @param other  The iterator to compare ourselves to.
                /// @return \c true if this and given iterator are not equal, \c false otherwise.
                bool operator!=(TRandomAccessIterator other)                                   const
                {
                    return !(*this == other);
                }

                /// Retrieves the character that this iterator references.
                /// @return The character value.
                TCharConstOrMutable& operator*()                                               const
                {
                    return *p;
                }

                /// Retrieves the character that this iterator references.
                /// @return The character value.
                TCharConstOrMutable& operator*()
                {
                    return *p;
                }


            //##################   To satisfy concept of  BidirectionalIterator   ##################

                /// Prefix decrement operator.
                /// @return A reference to this object.
                TRandomAccessIterator& operator--()
                {
                    --p;
                    return *this;
                }


                /// Postfix decrement operator.
                /// @return The iterator value prior the decrement operation.
                TRandomAccessIterator operator--(int)
                {
                    return TRandomAccessIterator(p--);
                }


            //##################   To satisfy concept of  RandomAccessIterator   ###################

                /// Addition assignment.
                /// @param n The value to subtract.
                /// @return A reference to this iterator.
                TRandomAccessIterator& operator+=(integer n)
                {
                    p+= n;
                    return *this;
                }

                /// Subtraction assignment.
                /// @param n The value to subtract.
                /// @return A reference to this iterator.
                TRandomAccessIterator& operator-=(integer n)
                {
                    p-= n;
                    return *this;
                }

                /// Addition.
                /// @param n The value to subtract.
                /// @return The resulting iterator value.
                TRandomAccessIterator operator+(integer n)                                     const
                {
                    return TRandomAccessIterator( p + n );
                }

                /// Subtraction.
                /// @param n The value to subtract.
                /// @return The resulting iterator value.
                TRandomAccessIterator operator-(integer n)                                     const
                {
                    return TRandomAccessIterator( p - n );
                }

                /// Difference (distance) from this iterator to the given one.
                /// @param other  The iterator to subtract
                /// @return The difference between (distance of) this and the given iterator.
                integer operator-(TRandomAccessIterator other)                                 const
                {
                    return p - other.p;
                }

                /// Subscript operator.
                /// @param n  The distance to add.
                /// @return Reference to the character referenced by this iterator plus the distance
                ///         given.
                TCharConstOrMutable& operator[]( integer n )                                   const
                {
                    return *( p + n );
                }

            //#### Comparison operators (also needed to satisfy concept of RandomAccessIterator) ###

                /// Compares this iterator with the given one.
                /// @param other  The iterator to compare
                /// @return \c true if this iterator is \e smaller than \p{other},
                ///         \c false otherwise.
                bool operator<(TRandomAccessIterator other)                                    const
                {
                    return p < other.p;
                }

                /// Compares this iterator with the given one.
                /// @param other  The iterator to compare
                /// @return \c true if this iterator is \e smaller than or equal to \p{other},
                ///         \c false otherwise.
                bool operator<=(TRandomAccessIterator other)                                   const
                {
                    return p <= other.p;
                }

                /// Compares this iterator with the given one.
                /// @param other  The iterator to compare
                /// @return \c true if this iterator is \e greater than \p{other},
                ///         \c false otherwise.
                bool operator>(TRandomAccessIterator other)                                    const
                {
                    return p > other.p;
                }

                /// Compares this iterator with the given one.
                /// @param other  The iterator to compare
                /// @return \c true if this iterator is \e greater than or equal to \p{other},
                ///         \c false otherwise.
                bool operator>=(TRandomAccessIterator other)                                   const
                {
                    return p >= other.p;
                }
                ALIB_WARNINGS_RESTORE
        };

        /// The constant iterator exposed by this character container. A Mutable version is
        /// found only in descendant classes (e.g., \alib{strings;TAString;AString}).
        using ConstIterator         = TRandomAccessIterator<const TChar>;

        /// The constant reverse iterator exposed by this character container. A Mutable version is
        /// found only in descendant classes (e.g., \alib{strings;TAString;AString}).
        using ConstReverseIterator  = std::reverse_iterator<ConstIterator>;


    //##############################################################################################
    /// @name std::iterator_traits
    /// @{
    //##############################################################################################
        /// Returns an iterator pointing to a constant character at the start of this string.
        /// @return The start of this string.
        ConstIterator          begin()  const           { return ConstIterator( buffer          ); }

        /// Returns an iterator pointing to a constant character at the start of this string.
        /// @return The start of this string.
        ConstIterator         cbegin()  const           { return ConstIterator( buffer          ); }

        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        /// Returns an iterator pointing behind this string.
        /// @return The end of this string.
        ConstIterator          end()    const           { return ConstIterator( buffer + length ); }

        /// Returns an iterator pointing behind this string.
        /// @return The end of this string.
        ConstIterator         cend()    const           { return ConstIterator( buffer + length ); }
        ALIB_WARNINGS_RESTORE

        /// Returns a reverse iterator pointing to a constant character at the end of this string.
        /// @return The last character of this string.
        ConstReverseIterator  rbegin()  const           { return ConstReverseIterator( end()   );  }

        /// Returns a reverse iterator pointing before the start of this string.
        /// @return The character before this string.
        ConstReverseIterator  rend()    const           { return ConstReverseIterator( begin() );  }

        /// Returns a reverse iterator pointing to a constant character at the end of this string.
        /// @return The last character of this string.
        ConstReverseIterator crbegin()  const           { return ConstReverseIterator( end()   );  }

        /// Returns a reverse iterator pointing before the start of this string.
        /// @return The character before this string.
        ConstReverseIterator crend()    const           { return ConstReverseIterator( begin() );  }

    //##############################################################################################
    /// @}
    //##############################################################################################
    
        //==========================================================================================
        /// Constructs this string using start and end iterators.
        ///
        /// @param start An iterator referencing the start of the string.
        /// @param end   An iterator referencing the end of the string.
        //==========================================================================================
        TString( ConstIterator& start, ConstIterator& end )
        : buffer( &*start)
        , length( end-start >= 0 ? end-start : 0 )
        {}

    protected:
        #if !DOXYGEN
            ALIB_WARNINGS_IGNORE_DOCS // needed due to a bug in current clang
        #endif

        //==========================================================================================
        /// Implementation of the substring search function.
        /// \attention This protected method expects valid (in range) parameters!
        ///            Parameter \p{endIdx} must be smaller or equal to the length of this string
        ///            minus the length of \p{needle} plus \c 1.
        /// @tparam TSensitivity The letter case sensitivity of the search.
        /// @param  needle       The substring to search.
        /// @param  startIdx     The start index of the search.
        /// @param  endIdx       The index where the search ends. Precisely, the index of the
        ///                      first character that is not found.
        /// @return The index of the first occurrence of \p{needle}, respectively \c -1 if not found.
        //==========================================================================================
        template<lang::Case TSensitivity =lang::Case::Sensitive>
        ALIB_API integer
        indexOfString( const TString&  needle, integer startIdx, integer endIdx )             const;

        #if !DOXYGEN
            ALIB_WARNINGS_RESTORE
        #endif
}; // class TString

//##############################################################################################
/// @name Comparison Operators
//##############################################################################################
#if DOXYGEN

//==================================================================================================
/// Comparison operators for \alib Strings and compatible types. Those are:
/// - <c>operator==</c>,
/// - <c>operator!=</c>,
/// - <c>operator<</c>,
/// - <c>operator<=</c>,
/// - <c>operator></c>, and
/// - <c>operator>=</c>.
///
/// The operators use methods
/// \alib{strings::TString;Equals;lhs.Equals<CHK, lang::Case::Sensitive>(rhs)}, and
/// \alib{strings::TString;CompareTo;lhs.CompareTo<CHK, lang::Case::Sensitive>(rhs)} for evaluation.
///
/// For performance-critical sections, the original methods may be used where template
/// parameter \p{TCheck} can be set to \alib{NC}.
///
/// \note For avoiding redundancy, only <c>operator==</c> is listed here in the documentation!
///
/// @param  lhs         The left-hand operand of string-like type.
/// @param  rhs         The right-hand operand of string-like type.
/// @returns \c true if the contents of the strings are equal, \c false otherwise.
//==================================================================================================
bool  operator== (const String& lhs, const String& rhs)  {}
#else

template<typename TChar, typename TCharArray>           bool                                           operator== (const TString<TChar>& lhs, const TCharArray&     rhs) { return  lhs. template Equals   <CHK, lang::Case::Sensitive>(rhs); }
template<typename TChar, typename TCharArray> ATMP_T_IF(bool, !ATMP_ISOF(TCharArray, TString<TChar>) ) operator== (const TCharArray&     lhs, const TString<TChar>& rhs) { return  rhs. template Equals   <CHK, lang::Case::Sensitive>(lhs); }
template<typename TChar, typename TCharArray>           bool                                           operator!= (const TString<TChar>& lhs, const TCharArray&     rhs) { return !lhs. template Equals   <CHK, lang::Case::Sensitive>(rhs); }
template<typename TChar, typename TCharArray> ATMP_T_IF(bool, !ATMP_ISOF(TCharArray, TString<TChar>) ) operator!= (const TCharArray&     lhs, const TString<TChar>& rhs) { return !rhs. template Equals   <CHK, lang::Case::Sensitive>(lhs); }

template<typename TChar, typename TCharArray>           bool                                           operator<  (const TString<TChar>& lhs, const TCharArray&     rhs) { return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs) <  0; }
template<typename TChar, typename TCharArray> ATMP_T_IF(bool, !ATMP_ISOF(TCharArray, TString<TChar>) ) operator<  (const TCharArray&     lhs, const TString<TChar>& rhs) { return  rhs. template CompareTo<CHK, lang::Case::Sensitive>(lhs) >  0; }
template<typename TChar, typename TCharArray>           bool                                           operator<= (const TString<TChar>& lhs, const TCharArray&     rhs) { return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs) <= 0; }
template<typename TChar, typename TCharArray> ATMP_T_IF(bool, !ATMP_ISOF(TCharArray, TString<TChar>) ) operator<= (const TCharArray&     lhs, const TString<TChar>& rhs) { return  rhs. template CompareTo<CHK, lang::Case::Sensitive>(lhs) >= 0; }
template<typename TChar, typename TCharArray>           bool                                           operator>  (const TString<TChar>& lhs, const TCharArray&     rhs) { return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs) >  0; }
template<typename TChar, typename TCharArray> ATMP_T_IF(bool, !ATMP_ISOF(TCharArray, TString<TChar>) ) operator>  (const TCharArray&     lhs, const TString<TChar>& rhs) { return  rhs. template CompareTo<CHK, lang::Case::Sensitive>(lhs) <  0; }
template<typename TChar, typename TCharArray>           bool                                           operator>= (const TString<TChar>& lhs, const TCharArray&     rhs) { return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs) >= 0; }
template<typename TChar, typename TCharArray> ATMP_T_IF(bool, !ATMP_ISOF(TCharArray, TString<TChar>) ) operator>= (const TCharArray&     lhs, const TString<TChar>& rhs) { return  rhs. template CompareTo<CHK, lang::Case::Sensitive>(lhs) <= 0; }

#endif



// #################################################################################################
// Template instantiation declarations
// #################################################################################################
#if !DOXYGEN

template<>      ALIB_API integer  TString<nchar>::WStringLength                       ()                                           const;
extern template ALIB_API integer  TString<nchar>::indexOfString<lang::Case::Sensitive>( const TString<nchar >&, integer, integer ) const;
extern template ALIB_API integer  TString<nchar>::indexOfString<lang::Case::Ignore   >( const TString<nchar >&, integer, integer ) const;
extern template ALIB_API integer  TString<nchar>::IndexOfSegmentEnd                   ( nchar, nchar, integer                    ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseDecDigits                      ( integer, integer*                        ) const;
extern template ALIB_API  int64_t TString<nchar>::ParseInt                            ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseDec                            ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseBin                            ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseHex                            ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<nchar>::ParseOct                            ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API double   TString<nchar>::ParseFloat                          ( integer, TNumberFormat<nchar>*, integer* ) const;
extern template ALIB_API size_t   TString<nchar>::Hashcode                            ()                                           const;
extern template ALIB_API size_t   TString<nchar>::HashcodeIgnoreCase                  ()                                           const;

template<> inline        integer  TString<wchar>::WStringLength                       () const  { return length; }
extern template ALIB_API integer  TString<wchar>::indexOfString<lang::Case::Sensitive>(const TString<wchar>&, integer, integer   ) const;
extern template ALIB_API integer  TString<wchar>::indexOfString<lang::Case::Ignore   >(const TString<wchar>&, integer, integer   ) const;
extern template ALIB_API integer  TString<wchar>::IndexOfSegmentEnd                   (wchar, wchar, integer                     ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseDecDigits                      ( integer, integer*                        ) const;
extern template ALIB_API  int64_t TString<wchar>::ParseInt                            ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseDec                            ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseBin                            ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseHex                            ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<wchar>::ParseOct                            ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API double   TString<wchar>::ParseFloat                          ( integer, TNumberFormat<wchar>*, integer* ) const;
extern template ALIB_API size_t   TString<wchar>::Hashcode                            ()                                           const;
extern template ALIB_API size_t   TString<wchar>::HashcodeIgnoreCase                  ()                                           const;

template<>      ALIB_API integer  TString<xchar>::WStringLength                       ()                                           const;
extern template ALIB_API integer  TString<xchar>::indexOfString<lang::Case::Sensitive>( const TString<xchar >&, integer, integer ) const;
extern template ALIB_API integer  TString<xchar>::indexOfString<lang::Case::Ignore   >( const TString<xchar >&, integer, integer ) const;
extern template ALIB_API integer  TString<xchar>::IndexOfSegmentEnd                   ( xchar, xchar, integer                    ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseDecDigits                      ( integer, integer*                        ) const;
extern template ALIB_API  int64_t TString<xchar>::ParseInt                            ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseDec                            ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseBin                            ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseHex                            ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API uint64_t TString<xchar>::ParseOct                            ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API double   TString<xchar>::ParseFloat                          ( integer, TNumberFormat<xchar>*, integer* ) const;
extern template ALIB_API size_t   TString<xchar>::Hashcode                            ()                                           const;
extern template ALIB_API size_t   TString<xchar>::HashcodeIgnoreCase                  ()                                           const;

// #################################################################################################
// debug members
// #################################################################################################
#if ALIB_DEBUG_STRINGS
    extern template ALIB_API void    TString<nchar>::dbgCheck() const;
    extern template ALIB_API void    TString<wchar>::dbgCheck() const;
    extern template ALIB_API void    TString<xchar>::dbgCheck() const;
#endif

#endif //!DOXYGEN


#if DOXYGEN
//==================================================================================================
/// This template class has three specializations for types \alib{characters;nchar},
/// \alib{characters;wchar}, and \alib{characters;xchar}, which each provides the static method
/// #EmptyString.
/// (The only non-zero-terminated string-constant needed/supported by \alib.)
///
/// The class is useful to implement methods that are templated with the character type they use
/// and that are in need of an empty string instance.
///
/// In non-templated code (that works with fixed or logical character sizes), the following
/// \c constexpr namespace constants are rather to be used:
/// \alib{EMPTY_STRING}, \alib{EMPTY_COMPLEMENT_STRING}, \alib{EMPTY_STRANGE_STRING},
/// \alib{EMPTY_NSTRING}, \alib{EMPTY_WSTRING}, and \alib{EMPTY_XSTRING}.
///
/// Nulled strings are not defined here, as they any time can be constructed using \c nullptr.
/// To improve readability, \c constexpr namespace variable are given with \alib{NULL_STRING},
/// \alib{NULL_COMPLEMENT_STRING}, \alib{NULL_STRANGE_STRING}, \alib{NULL_NSTRING},
/// \alib{NULL_WSTRING}, and \alib{NULL_XSTRING}.
///
/// @see
///   Zero-terminated constants are given with the inclusion of header
///   \alibheader{strings/cstring.hpp}.<br>
///   See also the manual chapter \ref alib_strings_details_constants.
///
/// @tparam TChar The \ref alib_characters_chars "character type".
//==================================================================================================
template<typename TChar> struct TT_StringConstants
{
    ///@return An empty string.
    constexpr static String<TChar>     EmptyString();
};


#else // DOXYGEN
template<typename TChar> struct TT_StringConstants {};
template<> struct TT_StringConstants<nchar> { constexpr static NString EmptyString() { return         "" ; } };
template<> struct TT_StringConstants<wchar> { constexpr static WString EmptyString() { return A_WCHAR(""); } };
template<> struct TT_StringConstants<xchar> { constexpr static XString EmptyString() { return A_XCHAR(""); } };
#endif  //!DOXYGEN

} // namespace alib[::strings]

/// An empty string of the default character type.
constexpr const String      EMPTY_STRING          = strings::TT_StringConstants<character     >::EmptyString();

/// An empty string of the complement character type.
constexpr ComplementString  EMPTY_COMPLEMENT_STRING= strings::TT_StringConstants<complementChar>::EmptyString();

/// An empty string of the strange character type.
constexpr StrangeString     EMPTY_STRANGE_STRING   = strings::TT_StringConstants<strangeChar   >::EmptyString();

/// An empty string of the narrow character type.
constexpr NString           EMPTY_NSTRING          = strings::TT_StringConstants<nchar         >::EmptyString();

/// An empty string of the wide character type.
constexpr WString           EMPTY_WSTRING          = strings::TT_StringConstants<wchar         >::EmptyString();

/// An empty string of the complement character type.
constexpr XString           EMPTY_XSTRING          = strings::TT_StringConstants<xchar         >::EmptyString();

// doxygen would consider the constructor call as a function call
#if DOXYGEN
/// A \e nulled string of the default character type.
constexpr  String           NULL_STRING;

/// A \e nulled string of the complement character type.
constexpr  ComplementString NULL_COMPLEMENT_STRING;

/// A \e nulled string of the strange character type.
constexpr  StrangeString    NULL_STRANGE_STRING;

/// A \e nulled string of the narrow character type.
constexpr NString           NULL_NSTRING;

/// A \e nulled string of the wide character type.
constexpr WString           NULL_WSTRING;

/// A \e nulled string of the complement character type.
constexpr XString           NULL_XSTRING;
#else
constexpr  String           NULL_STRING(nullptr);
constexpr  ComplementString NULL_COMPLEMENT_STRING(nullptr);
constexpr  StrangeString    NULL_STRANGE_STRING(nullptr);
constexpr NString           NULL_NSTRING(nullptr);
constexpr WString           NULL_WSTRING(nullptr);
constexpr XString           NULL_XSTRING(nullptr);
#endif

#if defined(_MSC_VER)
    #pragma warning( pop )
#endif

} // namespace [alib]

#endif // HPP_ALIB_STRINGS_STRING

