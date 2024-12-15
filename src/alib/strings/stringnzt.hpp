//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_STRINGNZT
#define HPP_ALIB_STRINGS_STRINGNZT 1
#pragma once
#include "alib/strings/string.hpp"


namespace alib {  namespace strings {

//==================================================================================================
/// This is rather a helper-class than a genuine string class. While derived from class \b String,
/// no extension of its interface is offered. Instead, the templated constructor is limited to
/// accept only string-types that are not zero-terminated.
///
/// The class is designed to be used to create API interfaces (i.e., overloaded methods) that accept
/// zero-terminated strings of type \alib{strings;TCString;CString} independently from
/// non-zero-terminated strings: If two overloaded methods accepted type \c String and \c CString
/// alternatively, compilation would be ambiguous because the \b String version would accept
/// zero-terminated strings as well. Therefore, overloaded methods rather must accept this class
/// and \b CString.
///
/// \see
///   Details and a quick sample are given in chapter
///   \ref alib_strings_nzt "4. Non-Zero-Terminated String Detection" of the
///   \ref alib_mod_strings "Programmer's Manual" of module \alib_strings_nl.
///
/// @tparam TChar The \ref alib_characters_chars "character type" of this string.
///               Alias names of this class using different character types are provided
///               with in namespace #alib with type definitions
///               \alib{StringNZT},
///               \alib{NStringNZT},
///               \alib{WStringNZT}, and
///               \alib{XStringNZT}.
//==================================================================================================
template<typename TChar>
class TStringNZT  : public TString<TChar>
{
     public:

        //==========================================================================================
        /// Constructor accepting a pointer to a character array and a string length.
        ///
        /// @param pBuffer   Pointer to the start of the character string to represent.
        /// @param pLength   The length of the character string to represent.
        //==========================================================================================
        constexpr
        TStringNZT( const TChar* pBuffer, integer pLength )
        : TString<TChar>( pBuffer, pLength)
        {}


    #if DOXYGEN
        //==========================================================================================
        /// Templated constructor that accepts C++ string-types which are not zero-terminated.
        ///
        /// @tparam TCharArray  Type that comprises a non-zero-terminated character array.
        /// @param  src         The source object.
        //==========================================================================================
        template <typename TCharArray>
        inline
        constexpr
        TStringNZT(const  TCharArray& src );


    #else // doxygen end

        ATMP_SELECT_IF_1TP( typename T,     characters::T_CharArray  <ATMP_RCV(T),TChar>::Access == characters::AccessType::Implicit
                                        &&  characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access != characters::AccessType::Implicit )
        constexpr
        TStringNZT(const T& src)
        : TString<TChar>( characters::T_CharArray<ATMP_RCV(T),TChar>::Buffer( src ),
                          characters::T_CharArray<ATMP_RCV(T),TChar>::Length( src ) )
        {}

        ATMP_SELECT_IF_1TP( typename T,    characters::T_CharArray  <ATMP_RCV(T),TChar>::Access == characters::AccessType::Implicit
                                       &&  characters::T_ZTCharArray<ATMP_RCV(T),TChar>::Access != characters::AccessType::Implicit )
        constexpr
        TStringNZT(const T* src)
        : TString<TChar>( characters::T_CharArray<ATMP_RCV(T),TChar>::Buffer( *src ),
                          characters::T_CharArray<ATMP_RCV(T),TChar>::Length( *src ) )
        {}

        ATMP_SELECT_IF_1TP( typename T,    characters::T_CharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::ExplicitOnly )
        constexpr
        explicit
        TStringNZT(const T& src)
        : TString<TChar>( characters::T_CharArray<ATMP_RCV(T),TChar>::Buffer( src ),
                          characters::T_CharArray<ATMP_RCV(T),TChar>::Length( src ) )
        {}

        ATMP_SELECT_IF_1TP( typename T,    characters::T_CharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::ExplicitOnly )
        constexpr
        explicit
        TStringNZT(const T* src)
        : TString<TChar>( characters::T_CharArray<ATMP_RCV(T),TChar>::Buffer( *src ),
                          characters::T_CharArray<ATMP_RCV(T),TChar>::Length( *src ) )
        {}

        ATMP_SELECT_IF_1TP( typename T,    characters::T_CharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::MutableOnly && !std::is_const<T>::value )
        constexpr
        explicit
        TStringNZT(      T& src)
        : TString<TChar>( characters::T_CharArray<ATMP_RCV(T),TChar>::Buffer( const_cast<T&>( src ) ),
                          characters::T_CharArray<ATMP_RCV(T),TChar>::Length( const_cast<T&>( src ) ) )
        {}

        ATMP_SELECT_IF_1TP( typename T,    characters::T_CharArray<ATMP_RCV(T),TChar>::Access == characters::AccessType::MutableOnly && !std::is_const<T>::value )
        constexpr
        explicit
        TStringNZT(      T* src)
        : TString<TChar>( characters::T_CharArray<ATMP_RCV(T),TChar>::Buffer( const_cast<T&>( *src ) ),
                          characters::T_CharArray<ATMP_RCV(T),TChar>::Length( const_cast<T&>( *src ) ) )
        {}

    #endif // doxygen
}; // class TStringNZT


}} // namespace [alib::strings]


#endif // HPP_ALIB_STRINGS_STRINGNZT

