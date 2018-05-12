// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include guard
#ifndef HPP_ALIB_STRINGS_LITERAL
#define HPP_ALIB_STRINGS_LITERAL 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION)
    #error "include 'alib/alib.hpp' instead of this header"
#endif

// #################################################################################################
// includes
// #################################################################################################


// conditional expression is constant for using our constant template parameters to select
// checking vs. non-checking method versions
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
    #pragma warning( disable : 4307 ) // also: integral constant overflow, because of TMP
#endif

namespace aworx { namespace lib { namespace strings
{

/** ************************************************************************************************
 * This templated class is a specialization of class #TString which has a fixed length.
 * Functions that accept such objects as a parameter can rely
 * at compile time on the constant length of the string and optimize their performance
 * accordingly. <br>
 * For example, template method
 * \alib{strings,AStringBase::operator<<(const T&),AString::operator<<(const T&)}
 * which internally makes use of template method
 * \alib{strings,AStringBase::Apply,AString::Apply} will append objects of this type
 * without doing a copy loop (up to a certain amount of characters).
 *
 * The class is useful to assign string literals to objects, including their lengths and pass
 * them around, hence the name.<br>
 *
 * It might also used to provide strings with variable contents. For this, the character
 * array given in the constructor needs to be managed and manipulated outside of this class.
 *
 * \note
 *   This class \b %StringLiteral, when used in source code as well as in documentations, is
 *   often referred to using the synonym '<b>%SLiteral</b>'. For more information about
 *   the synonymous names of \alib string classes, refer to
 *   \ref alib_strings_templated "3. Templated String Types" and
 *   \ref CPP_AWORX_NS_SHORTCUTS "Type Shortcuts of Namespace aworx".
 *
 * @tparam TLength The length of the represented string.
***************************************************************************************************/
template<typename TChar, size_t TLength>
class StringLiteralBase : public TStringBase<TChar>
{
    public:
    /** ********************************************************************************************
     * The only available constructor taking a string literal of exactly the size we have plus one
     * for the termination character.
     * @param src  The string literal to represent.
     **********************************************************************************************/
    constexpr StringLiteralBase( const TChar (&src)[TLength + 1]  )
    : TStringBase<TChar>( src, TLength )
    {}

    /** ********************************************************************************************
     * Copies the contents of this string (excluding the terminating zero) to the given
     * destination using static method #Copy(TChar*, const TChar*).
     *
     * @param dest The destination buffer.
     **********************************************************************************************/
    inline
    void Copy( TChar* dest)  { Copy( dest, StringBase<TChar>::buffer ); }

    /** ********************************************************************************************
     * Static method that copies TLength characters from the given the source array to the
     * given destination array.
     * If template parameter \p{TLength} equals or is greater than zero, memcpy() is used. If it is
     * below, the right amount of single character copy operations (no loops, no ifs) is  inserted
     * into the code.
     *
     * @param src  The source buffer to copy from.
     * @param dest The destination buffer to copy to..
     **********************************************************************************************/
    static
    inline
    void Copy( TChar* dest, const TChar* src)
    {
        if (TLength >= 10 )
        {
            CString<TChar>::Copy( src, TLength, dest );
            return;
        }

        if (TLength == 9 ) goto copy9;
        if (TLength == 8 ) goto copy8;
        if (TLength == 7 ) goto copy7;
        if (TLength == 6 ) goto copy6;
        if (TLength == 5 ) goto copy5;
        if (TLength == 4 ) goto copy4;
        if (TLength == 3 ) goto copy3;
        if (TLength == 2 ) goto copy2;
        if (TLength == 1 ) goto copy1;
        return;

        copy9: *dest++=   *src++;
        copy8: *dest++=   *src++;
        copy7: *dest++=   *src++;
        copy6: *dest++=   *src++;
        copy5: *dest++=   *src++;
        copy4: *dest++=   *src++;
        copy3: *dest++=   *src++;
        copy2: *dest++=   *src++;
        copy1: *dest  =   *src;
    }
};


/** ************************************************************************************************
 * Specialization of template struct \alib{strings,T_StringLiteral} for template class
 * \alib{strings,StringLiteralBase,StringLiteral} itself.
 *
 * @tparam TLength The length of the literal.
 **************************************************************************************************/
template<typename TChar, size_t TLength>
struct T_StringLiteral<StringLiteralBase<TChar, TLength>, TChar> : public std::true_type
{
    /**
     * Returns the buffer of the literal.
     * @param src The pointer to the source string literal
     * @return The buffer of the literal.
     */
    static inline constexpr const TChar* Buffer(const StringLiteralBase<TChar, TLength>& src) { return src.Buffer(); }

    /**
     * Returns the constant \p{TLength} of type <b>StringLiteral<TLength></b>.
     * @return The length.
     */
    static inline constexpr integer    Length()        { return TLength; }
};



}}} // namespace [aworx::lib::strings]


#if defined(_MSC_VER)
    #pragma warning( pop )
#endif

#endif // HPP_ALIB_STRINGS_LITERAL
