//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/strings/strings.prepro.hpp"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Strings.Tokenizer;
#else
#   include "ALib.Strings.Tokenizer.H"
#endif
//========================================== Implementation ========================================
namespace alib {  namespace strings { namespace util  {

template<typename TChar>
TSubstring<TChar>& TTokenizer<TChar>::Next( lang::Whitespaces trimming, TChar newDelim ) {
    if ( Rest.IsNull() ) {
        Actual= nullptr;
        return Actual;
    }

    // change of delim?
    if ( newDelim != '\0' )
        delim= newDelim;

    do
    {
        integer nextDelimiter= Rest.IndexOf( delim );

        if ( nextDelimiter >= 0 ) {
            Actual=  Rest.template Substring<NC>( 0                , nextDelimiter                       );
            Rest  =  Rest.template Substring<NC>( nextDelimiter + 1, Rest.Length() - (nextDelimiter + 1) );
        } else {
            Actual= Rest;
            Rest  = nullptr;
        }

        // trim
        if ( trimming == lang::Whitespaces::Trim )
            Actual.Trim( TrimChars );
    }
    while( skipEmpty && Actual.IsEmpty() && Rest.IsNotNull() );

    return Actual;
}

template TSubstring<nchar>& TTokenizer<nchar>::Next( lang::Whitespaces trimming, nchar newDelim );
template TSubstring<wchar>& TTokenizer<wchar>::Next( lang::Whitespaces trimming, wchar newDelim );

}}} // namespace [alib::strings::util]
