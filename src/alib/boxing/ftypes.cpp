// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################


#include "alib/alib.hpp"

#if ALIB_FEAT_BOXING_FTYPES

#include <algorithm>
#include <cmath>
#if !defined(_GLIBCXX_TYPEINDEX) && !defined(_TYPEINDEX_)
#   include <typeindex>
#endif


#if !defined(_GLIBCXX_CSTRING) && !defined(_CSTRING_)
    #include <cstring>
#endif

// #################################################################################################
// Singletons
// #################################################################################################

namespace aworx { namespace lib { namespace boxing {

/**
 *
 * <b>%ALib %Boxing</b> allows to map C++ types to boxed types in a non-bijective way. As a
 * consequence, there are different possibilities to implement such mapping and the mapping
 * that <b>%ALib %Boxing</b> uses by default for boxing C++ fundamental types is considered to
 * be just a proposal and a default behavior.
 *
 * To suppress the use of this built-in implementation of boxing C++ fundamental types,
 * compilation symbol \ref ALIB_FEAT_BOXING_FTYPES_OFF may be passed to the compiler.
 * This freedom of choice is \b limited when using the full distribution of \alib or other
 * \alibmod that includes more \alib functionality than just boxing. In these cases,
 * the default built-in implementation of boxing fundamental types is automatically chosen, because
 * other parts of \alib rely on the default behavior. Using \ref ALIB_FEAT_BOXING_FTYPES_OFF
 * with these distributions results in a compilation error.
 *
 * The mapping of C++ fundamental types to boxed types is done as follows:
 * - Character types \ref aworx::character, \ref aworx::complementChar and \ref aworx::strangeChar
 *   are mapped to identical boxed types.
 * - Arrays of character types are mapped to boxed arrays of the respective character type, e.g
 *   \c char[5] &rarr; \c char[] or \c wchar_t[10] &rarr; \c wchar_t[]. The array length is of-course
 *   stored in the box.
 * - Pointers to character types the same  mapped to arrays of the respective character type, e.g
 *   \c char* &rarr; \c char[] or \c wchar_t* &rarr; \c wchar_t[]. The array length is set to
 *   \c 0 if either the pointer is \c nullptr or the first character in the string is \c '\0'.
 *   Otherwise, the length is set to \c -1. Therefore, when unboxing a character array type
 *   or implementing such type in an \ref aworx::lib::boxing::Interface "Interface" implementation,
 *   the "true" length has to be evaluated if \c -1 is stored!
 * - Type \c bool is mapped to identical boxed type.
 * - All signed integer types (\c int8_t, \c int16_t, \c int32_t, \c int64_t and
 *   \ref aworx::lib::lang::intGap_t "intGap_t") are boxed as
 *   \ref aworx::lib::boxing::ftypes::boxed_int "boxed_int", which is an alias of \c int64_t.
 * - All unsigned integer types (\c uint8_t, \c uint16_t, \c uint32_t, \c uint64_t and
 *   \ref aworx::lib::lang::uintGap_t "uintGap_t") are boxed as
 *   \ref aworx::lib::boxing::ftypes::boxed_uint "boxed_uint", which is an alias of \c uint64_t.
 * - Type \c float and \c double are both boxed as \c double.
 * - Unboxing is implemented in a strict fashion: Only the "result" types are allowed to be
 *   unboxed, hence \c aworx::character, \c aworx::complementChar, \ref aworx::strangeChar,
 *   \c bool, \c %boxed_int, \c %boxed_uint and \c double.
 *
 * \see For more information about this default implementation of %boxing C++ fundamental types,
 *      see manual chapter \ref alib_namespace_boxing_types_ftypes "4.2 Boxing Fundamental Types".
 *
 * \attention
 *   In this namespace documentation, you will find specializations of templated namespace functions
 *   of namespace \c aworx::lib::boxing. This is of-course wrong in respect to the C++ language
 *   definition, which requires such specializations to be placed in the original namespace.<br>
 *   It was decided to 'trick' the [documentation parser](http://www.stack.nl/~dimitri/doxygen) to
 *   show these specialization in this namespace, to keep namespace \b %boxing
 *   clean and to have all specializations related to this namespace in one place.
 */
namespace ftypes {

// #################################################################################################
// IEquals_Tdouble
// #################################################################################################
bool IEquals_Tdouble::Invoke( const Box& box, const Box& comp )
{
    if( !comp.IsType<double>() )
        return false;
    double d1=   box.Unbox<double>();
    double d2= comp.Unbox<double>();

    // we can ignore this warning, because we fixed it with the second test
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wfloat-equal"
    #endif

    return      d1 == d2
            // take rounding errors into account.
            // We use the "float-epsilon" and double it to be even a little weaker!
            ||  std::fabs( d1 - d2 ) <= static_cast<double>( 2.0f * std::numeric_limits<float>::epsilon() );

    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif
}




// #################################################################################################
// IEquals for arrays of type nchar, wchar and strangeChar
// #################################################################################################
bool    IEquals_TncharArr::Invoke ( const Box& box, const Box& comp )
{
    if ( !comp.IsArrayOf<char>() )
        return false;

    const char* compBuf= reinterpret_cast<char*>( comp.PeekRawValue() );
    const char*  boxBuf= reinterpret_cast<char*>(  box.PeekRawValue() );
    if ( (boxBuf == nullptr) != (compBuf == nullptr) )
            return false;

    size_t compLen= comp.Length() >= 0 ? static_cast<size_t>( comp.Length() )
                                       : compBuf != nullptr ? strlen( compBuf )
                                       : 0;
    size_t  boxLen=  box.Length() >= 0 ? static_cast<size_t>( box.Length() )
                                       :  boxBuf != nullptr ? strlen( boxBuf )
                                       : 0;
    if ( boxLen != compLen )
        return false;


    return  0 == strncmp ( boxBuf, compBuf, static_cast<size_t>(boxLen) );
}

bool    IEquals_TwcharArr::Invoke ( const Box& box, const Box& comp )
{
    if ( !comp.IsArrayOf<wchar_t>() )
        return false;

    const wchar_t* compBuf= reinterpret_cast<wchar_t*>(comp.PeekRawValue());
    const wchar_t*  boxBuf= reinterpret_cast<wchar_t*>(box .PeekRawValue());
    if ( (boxBuf == nullptr) != (compBuf == nullptr) )
            return false;

    size_t compLen= comp.Length() >= 0 ? static_cast<size_t>( comp.Length() )
                                       : compBuf != nullptr ? wcslen( compBuf )
                                       : 0;
    size_t  boxLen=  box.Length() >= 0 ? static_cast<size_t>( box.Length() )
                                       :  boxBuf != nullptr ? wcslen(  boxBuf )
                                       : 0;
    if ( boxLen != compLen )
        return false;


    return  0 == wcsncmp ( boxBuf, compBuf, static_cast<size_t>(boxLen) );
}

#if ALIB_MODULE_STRINGS
bool    IEquals_TstrangeCharArr::Invoke ( const Box& box, const Box& comp )
{
    if ( !comp.IsArrayOf<strangeChar>() )
        return false;

    const strangeChar* compBuf= reinterpret_cast<strangeChar*>(comp.PeekRawValue());
    const strangeChar*  boxBuf= reinterpret_cast<strangeChar*>( box.PeekRawValue());
    if ( (boxBuf == nullptr) != (compBuf == nullptr) )
            return false;

    integer compLen= comp.Length() >= 0 ? comp.Length() : strings::CString<strangeChar>::Length( compBuf );
    integer boxLen=   box.Length() >= 0 ?  box.Length() : strings::CString<strangeChar>::Length(  boxBuf );
    if ( boxLen != compLen )
        return false;

    integer idx= 0;
    while( idx < boxLen )
    {
        if( boxBuf[idx] != compBuf[idx] )
            return false;
        idx++;
    }
    return  true;
}
#endif

bool    IIsLess_boxed_uint::Invoke ( const Box& lhs, const Box& rhs )
{
    if( lhs.IsSameType( rhs )   )     return                         lhs.PeekRawValue()  <                           rhs.PeekRawValue     ()  ;
    if( rhs.IsType<boxed_int>() )     return static_cast<boxed_int>( lhs.PeekRawValue()) <                           rhs.Unbox<boxed_int> ()  ;
    if( rhs.IsType<double>()    )     return                         lhs.PeekRawValue()  <  static_cast<boxed_uint>( rhs.Unbox<double    >() );
    return std::type_index( lhs.GetTypeInfo() ) < std::type_index( rhs.GetTypeInfo() );
}

bool    IIsLess_boxed_int ::Invoke ( const Box& lhs, const Box& rhs )
{
    if( lhs.IsSameType( rhs )    )    return                         lhs.Unbox<boxed_int>()  <                           rhs.Unbox<boxed_int >()  ;
    if( rhs.IsType<boxed_uint>() )    return                         lhs.Unbox<boxed_int>()  <  static_cast<boxed_int> ( rhs.Unbox<boxed_uint>() );
    if( rhs.IsType<double>()     )    return                         lhs.Unbox<boxed_int>()  <  static_cast<boxed_int> ( rhs.Unbox<double    >() );
    return std::type_index( lhs.GetTypeInfo() ) < std::type_index( rhs.GetTypeInfo() );
}

bool    IIsLess_double    ::Invoke ( const Box& lhs, const Box& rhs )
{
    if( lhs.IsSameType( rhs )    )    return                         lhs.Unbox<double>   ()  <                           rhs.Unbox<double    >()  ;
    if( rhs.IsType<boxed_uint>() )    return                         lhs.Unbox<double>   ()  <  static_cast<double>    ( rhs.Unbox<boxed_uint>() );
    if( rhs.IsType<boxed_int>()  )    return                         lhs.Unbox<double>   ()  <  static_cast<double>    ( rhs.Unbox<boxed_int >() );
    return std::type_index( lhs.GetTypeInfo() ) < std::type_index( rhs.GetTypeInfo() );
}



}}}}// namespace [aworx::lib::boxing::ftypes]

#endif // ALIB_FEAT_BOXING_FTYPES
