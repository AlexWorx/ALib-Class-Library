// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined(HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif
#if !defined(HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif

#if !defined(_GLIBCXX_TYPEINDEX) && !defined(_TYPEINDEX_)
#   include <typeindex>
#endif

#if ALIB_DEBUG
    #if !defined (HPP_ALIB_STRINGS_UTIL_STRINGTUPLES)
    #   include "alib/strings/util/stringtuples.hpp"
    #endif
    #if !defined (HPP_ALIB_STRINGS_BOXING_DEBUG)
    #   include "alib/strings/boxing/debug.hpp"
    #endif

    #if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
    #   include <algorithm>
    #endif
#endif


namespace aworx { namespace lib { namespace strings {


/**
 * This namespace provides a customization of <b>%ALib %Boxing</b> for <b>%ALib Strings</b>.
 * This means, on the one hand that code in this namespace specifies how objects found in parent
 * namespace #aworx::lib::strings are boxed and on the other hand introduces box-interfaces
 * related to strings.
 *
 * \attention
 *   In this documentation, you will find specializations of templated namespace functions of
 *   namespace \c aworx::lib::boxing. This is of-course wrong in respect to the C++ language
 *   definition, which requires such specializations to be placed in the original namespace.<br>
 *   It was decided to 'trick' the [documentation parser](http://www.stack.nl/~dimitri/doxygen) to
 *   show these specialization in this namespace, to keep namespace \b %boxing
 *   clean and to have all specializations related to this namespace in one place.
 */
namespace boxing {

//! @cond NO_DOX
bool IIsLess_Tchar_arr::Invoke( const Box& lhs, const Box& rhs )
{
    if( rhs.IsType<NString>() )
        return lhs.Unbox<NString>() < rhs.Unbox<NString>();

    return std::type_index( lhs.GetTypeInfo() ) < std::type_index(rhs.GetTypeInfo() );
}

bool IIsLess_Twchar_arr::Invoke( const Box& lhs, const Box& rhs )
{
    if( rhs.IsType<WString>() )
        return lhs.Unbox<WString>() < rhs.Unbox<WString>();

    return std::type_index( lhs.GetTypeInfo() ) < std::type_index(rhs.GetTypeInfo() );
}


template<>
void IApply<char>::Invoke( const Box& box, NAString& target)
{
    target._( ALIB_REL_DBG( "Unknown", lib::debug::Boxing().GetType(box) ) )
          ._('(')._( NNumberFormat::Computational.HexLiteralPrefix)._( NFormat::Hex(static_cast<uint64_t >( box.PeekRawValue())) )._(')');
}


template<>
void IApply<wchar_t>::Invoke( const Box& box, WAString& target)
{
    target._( WAString( ALIB_REL_DBG( L"Unknown", lib::debug::Boxing().GetType(box) )).ToCString()  )
          ._(L'(')._( WNumberFormat::Computational.HexLiteralPrefix)._( WFormat::Hex(static_cast<uint64_t >( box.PeekRawValue())) )._(')');
}

//! @endcond



}}}}// namespace [aworx::lib::strings::boxing]

