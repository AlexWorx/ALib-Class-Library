// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/strings/substring.hpp"
#   include "alib/strings/detail/numberconversion.hpp"
#endif // !DOXYGEN

// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif

namespace alib {  namespace strings {

template<typename TChar>
bool   TSubstring<TChar>::consumeDecDigitsImpl( uint64_t& result )
{
    integer idx= 0;
    result=  detail::ParseDecDigits( *this, idx );
    if( idx > 0 )
    {
        ConsumeChars<NC>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   TSubstring<TChar>::consumeIntImpl( int64_t& result, TNumberFormat<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &TNumberFormat<TChar>::Computational;

    integer idx= 0;
    result=  detail::ParseInt( *this, idx, *numberFormat );
    if( idx > 0 )
    {
        ConsumeChars<NC>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   TSubstring<TChar>::consumeDecImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &TNumberFormat<TChar>::Computational;

    integer idx= 0;
    result=  detail::ParseDec( *this, idx, *numberFormat );
    if( idx > 0 )
    {
        ConsumeChars<NC>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   TSubstring<TChar>::consumeBinImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &TNumberFormat<TChar>::Computational;

    integer idx= 0;
    result=  detail::ParseBin( *this, idx, *numberFormat );
    if( idx > 0 )
    {
        ConsumeChars<NC>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   TSubstring<TChar>::consumeHexImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &TNumberFormat<TChar>::Computational;

    integer idx= 0;
    result=  detail::ParseHex( *this, idx, *numberFormat );
    if( idx > 0 )
    {
        ConsumeChars<NC>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   TSubstring<TChar>::consumeOctImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat )
{
    if ( numberFormat == nullptr )
        numberFormat= &TNumberFormat<TChar>::Computational;

    integer idx= 0;
    result=  detail::ParseOct( *this, idx, *numberFormat );
    if( idx > 0 )
    {
        ConsumeChars<NC>( idx );
        return true;
    }
    return false;
}

template<typename TChar>
bool   TSubstring<TChar>::ConsumeFloat( double&                result,
                                        TNumberFormat<TChar>*  numberFormat   )
{
    if ( numberFormat == nullptr )
        numberFormat= &TNumberFormat<TChar>::Computational;

    integer idx= 0;
    result=  detail::ParseFloat( *this, idx, *numberFormat );
    if( idx > 0 )
    {
        ConsumeChars<NC>( idx );
        return true;
    }
    return false;
}


template bool  TSubstring<nchar>::ConsumeFloat        ( double&   , TNumberFormat<nchar>* );
template bool  TSubstring<nchar>::consumeDecDigitsImpl( uint64_t&                         );
template bool  TSubstring<nchar>::consumeIntImpl      (  int64_t& , TNumberFormat<nchar>* );
template bool  TSubstring<nchar>::consumeDecImpl      ( uint64_t& , TNumberFormat<nchar>* );
template bool  TSubstring<nchar>::consumeBinImpl      ( uint64_t& , TNumberFormat<nchar>* );
template bool  TSubstring<nchar>::consumeHexImpl      ( uint64_t& , TNumberFormat<nchar>* );
template bool  TSubstring<nchar>::consumeOctImpl      ( uint64_t& , TNumberFormat<nchar>* );

template bool  TSubstring<wchar>::ConsumeFloat        ( double&   , TNumberFormat<wchar>* );
template bool  TSubstring<wchar>::consumeDecDigitsImpl( uint64_t&                         );
template bool  TSubstring<wchar>::consumeIntImpl      (  int64_t& , TNumberFormat<wchar>* );
template bool  TSubstring<wchar>::consumeDecImpl      ( uint64_t& , TNumberFormat<wchar>* );
template bool  TSubstring<wchar>::consumeBinImpl      ( uint64_t& , TNumberFormat<wchar>* );
template bool  TSubstring<wchar>::consumeHexImpl      ( uint64_t& , TNumberFormat<wchar>* );
template bool  TSubstring<wchar>::consumeOctImpl      ( uint64_t& , TNumberFormat<wchar>* );

template bool  TSubstring<xchar>::ConsumeFloat        ( double&   , TNumberFormat<xchar>* );
template bool  TSubstring<xchar>::consumeDecDigitsImpl( uint64_t&                         );
template bool  TSubstring<xchar>::consumeIntImpl      (  int64_t& , TNumberFormat<xchar>* );
template bool  TSubstring<xchar>::consumeDecImpl      ( uint64_t& , TNumberFormat<xchar>* );
template bool  TSubstring<xchar>::consumeBinImpl      ( uint64_t& , TNumberFormat<xchar>* );
template bool  TSubstring<xchar>::consumeHexImpl      ( uint64_t& , TNumberFormat<xchar>* );
template bool  TSubstring<xchar>::consumeOctImpl      ( uint64_t& , TNumberFormat<xchar>* );

}} // namespace [alib::strings]

