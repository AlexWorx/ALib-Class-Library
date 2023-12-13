// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#   include "alib/strings/util/tokenizer.hpp"
#endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace strings { namespace util  {

template<typename TChar>
TSubstring<TChar>& TTokenizer<TChar>::Next( Whitespaces trimming, TChar newDelim )
{
    if ( Rest.IsNull() )
    {
        Actual= nullptr;
        return Actual;
    }

    // change of delim?
    if ( newDelim != '\0' )
        delim= newDelim;

    do
    {
        integer nextDelimiter= Rest.IndexOf( delim );

        if ( nextDelimiter >= 0 )
        {
            Actual=  Rest.template Substring<false>( 0                , nextDelimiter                       );
            Rest  =  Rest.template Substring<false>( nextDelimiter + 1, Rest.Length() - (nextDelimiter + 1) );
        }
        else
        {
            Actual= Rest;
            Rest  = nullptr;
        }

        // trim
        if ( trimming == Whitespaces::Trim )
            Actual.Trim( TrimChars );
    }
    while( skipEmpty && Actual.IsEmpty() && Rest.IsNotNull() );

    return Actual;
}

template TSubstring<nchar>& TTokenizer<nchar>::Next( Whitespaces trimming, nchar newDelim );
template TSubstring<wchar>& TTokenizer<wchar>::Next( Whitespaces trimming, wchar newDelim );

}}}} // namespace [aworx::lib::strings::util]
