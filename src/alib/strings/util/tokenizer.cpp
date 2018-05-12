// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #include "alib/strings/util/tokenizer.hpp"
#endif

#if !defined (_GLIBCXX_OSTREAM)
    #include <ostream>
#endif


// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif

namespace aworx { namespace lib { namespace strings { namespace util  {

Substring&    Tokenizer::Next( lang::Whitespaces trimming, character newDelim )
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
            Actual= Rest.Substring<false>( 0                , nextDelimiter                       );
            Rest  = Rest.Substring<false>( nextDelimiter + 1, Rest.Length() - (nextDelimiter + 1) );
        }
        else
        {
            Actual= Rest;
            Rest  = nullptr;
        }

        // trim
        if ( trimming == lang::Whitespaces::Trim )
            Actual.Trim( Whitespaces );
    }
    while( skipEmpty && Actual.IsEmpty() && Rest.IsNotNull() );

    return Actual;
}

}}}} // namespace [aworx::lib::strings::util]
