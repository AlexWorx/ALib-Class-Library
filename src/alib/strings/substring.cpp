// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif

#if !defined (_GLIBCXX_OSTREAM)
    #include <ostream>
#endif


// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif

namespace aworx { namespace lib { namespace strings {

template<typename TChar>
bool   SubstringBase<TChar>::consumeDecDigitsImpl( uint64_t& result )
{
    integer idx= 0;
    result=  NumberFormatBase<TChar>::ParseDecDigits( *this, idx );
    if( idx > 0 )
    {
        ConsumeChars<false>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   SubstringBase<TChar>::consumeIntImpl( int64_t& result, NumberFormatBase<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &NumberFormatBase<TChar>::Computational;

    integer idx= 0;
    result=  numberFormat->ParseInt( *this, idx );
    if( idx > 0 )
    {
        ConsumeChars<false>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   SubstringBase<TChar>::consumeDecImpl( uint64_t& result, NumberFormatBase<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &NumberFormatBase<TChar>::Computational;

    integer idx= 0;
    result=  numberFormat->ParseDec( *this, idx );
    if( idx > 0 )
    {
        ConsumeChars<false>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   SubstringBase<TChar>::consumeBinImpl( uint64_t& result, NumberFormatBase<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &NumberFormatBase<TChar>::Computational;

    integer idx= 0;
    result=  numberFormat->ParseBin( *this, idx );
    if( idx > 0 )
    {
        ConsumeChars<false>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   SubstringBase<TChar>::consumeHexImpl( uint64_t& result, NumberFormatBase<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &NumberFormatBase<TChar>::Computational;

    integer idx= 0;
    result=  numberFormat->ParseHex( *this, idx );
    if( idx > 0 )
    {
        ConsumeChars<false>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   SubstringBase<TChar>::consumeOctImpl( uint64_t& result, NumberFormatBase<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &NumberFormatBase<TChar>::Computational;

    integer idx= 0;
    result=  numberFormat->ParseOct( *this, idx );
    if( idx > 0 )
    {
        ConsumeChars<false>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   SubstringBase<TChar>::ConsumeFloat( double&          result,
                                NumberFormatBase<TChar>*    numberFormat   )
{
    if ( numberFormat == nullptr )
        numberFormat= &NumberFormatBase<TChar>::Computational;

    integer idx= 0;
    result=  numberFormat->ParseFloat( *this, idx );
    if( idx > 0 )
    {
        ConsumeChars<false>( idx );
        return true;
    }
    return false;
}



template bool  SubstringBase<nchar>::ConsumeFloat        ( double&   , NumberFormatBase<nchar>* );
template bool  SubstringBase<nchar>::consumeDecDigitsImpl( uint64_t&                            );
template bool  SubstringBase<nchar>::consumeIntImpl      (  int64_t& , NumberFormatBase<nchar>* );
template bool  SubstringBase<nchar>::consumeDecImpl      ( uint64_t& , NumberFormatBase<nchar>* );
template bool  SubstringBase<nchar>::consumeBinImpl      ( uint64_t& , NumberFormatBase<nchar>* );
template bool  SubstringBase<nchar>::consumeHexImpl      ( uint64_t& , NumberFormatBase<nchar>* );
template bool  SubstringBase<nchar>::consumeOctImpl      ( uint64_t& , NumberFormatBase<nchar>* );

template bool  SubstringBase<wchar>::ConsumeFloat        ( double&   , NumberFormatBase<wchar>* );
template bool  SubstringBase<wchar>::consumeDecDigitsImpl( uint64_t&                            );
template bool  SubstringBase<wchar>::consumeIntImpl      (  int64_t& , NumberFormatBase<wchar>* );
template bool  SubstringBase<wchar>::consumeDecImpl      ( uint64_t& , NumberFormatBase<wchar>* );
template bool  SubstringBase<wchar>::consumeBinImpl      ( uint64_t& , NumberFormatBase<wchar>* );
template bool  SubstringBase<wchar>::consumeHexImpl      ( uint64_t& , NumberFormatBase<wchar>* );
template bool  SubstringBase<wchar>::consumeOctImpl      ( uint64_t& , NumberFormatBase<wchar>* );

}}}// namespace [aworx::lib::strings]
