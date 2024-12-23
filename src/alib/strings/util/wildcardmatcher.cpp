// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/strings/util/wildcardmatcher.hpp"
#   include "alib/strings/substring.hpp"
#endif // !DOXYGEN

namespace alib {  namespace strings { namespace util  {

#define    STRING    -1
#define    ASTERISK  -2

template<typename TChar>
void TWildcardMatcher<TChar>::Compile( const TString<TChar>& pattern )
{
    // Note: The following invariants will be true in the command vector:
    //       - A String-entry will be a non-empty string
    //       - No two * follow each other"
    //       - A ? will not follow an asterisk (combinations of ? and ' will be sorted to "?(n) *"

    commands.clear();

    TSubstring<TChar> parser= pattern;
    while( parser.IsNotEmpty() )
    {
        // *
        if(parser.CharAtStart() == '*' )
        {
            // add if empty or last is not * already.
             if(   commands.size() == 0
                || commands.back().first != ASTERISK ) commands.emplace_back( ASTERISK, nullptr  );

            parser.ConsumeChar();
            continue;
        }

        // ?
        int qtyQ= 0;
        while( parser.ConsumeChar('?') )
            ++qtyQ;

        if( qtyQ )
        {
            // previous is "*" ?
            if(     commands.size() > 1
                    &&  (commands.end()-1)->first == ASTERISK )
            {

                // before * we already have Q? -> add to the Q
                if(     commands.size() >= 2
                        &&  (commands.end()-2)->first > 0 )
                {
                    (commands.end()-2)->first+= qtyQ;
                    continue;
                }

                commands.emplace( commands.end() - 1, qtyQ, nullptr );
                continue;
            }

            // Just add it
            commands.emplace_back( qtyQ, nullptr );
            continue;
        }

        // strings
        integer idx= 1;
        while ( idx < parser.Length() && parser[idx] != '*' && parser[idx] != '?'  )
            ++idx;

        commands.emplace_back( STRING, TString<TChar>(parser.Buffer(), idx ) );
        parser.ConsumeChars( idx );
        continue;
    }

}

template<typename TChar>
bool TWildcardMatcher<TChar>::Match( const TString<TChar>& pHaystack, lang::Case sensitivity  )
{
    if( commands.size() == 0 )
        return true;

    if( pHaystack.IsNull() )
        return false;

    TSubstring<TChar> haystack= pHaystack;
    size_t  actCmd = 0;
    size_t  lastCmd= size_t( commands.size() - 1 );
    bool    lastWasAsterisk= false;

    for( actCmd= 0; actCmd <= lastCmd ; ++actCmd )
    {
        auto& cmd= commands[actCmd];

        // ?
        if( cmd.first > 0 )
        {
            if ( haystack.Length() < cmd.first )
                return false;
            haystack.template ConsumeChars<NC>( cmd.first );
            continue;
        }

        // *
        if( cmd.first == ASTERISK )
        {
            if ( actCmd == lastCmd )
                return true;

            lastWasAsterisk= true;
            continue;
        }

        // string
        if( cmd.second.Length() > haystack.Length() )
            return false;

        if( lastWasAsterisk )
        {
            integer idx= sensitivity==lang::Case::Sensitive  ?  haystack.template IndexOf<NC, lang::Case::Sensitive>( cmd.second, 0, haystack.Length() - cmd.second.Length() + 1 )
                                                             :  haystack.template IndexOf<NC, lang::Case::Ignore   >( cmd.second, 0, haystack.Length() - cmd.second.Length() + 1 );
            if( idx < 0 )
                return false;
            haystack.ConsumeChars( idx + cmd.second.Length() );
            lastWasAsterisk= false;
            continue;
        }

        if( sensitivity==lang::Case::Sensitive ? (!haystack.template StartsWith<NC, lang::Case::Sensitive>( cmd.second ) )
                                               : (!haystack.template StartsWith<NC, lang::Case::Ignore   >( cmd.second ) )  )
            return false;

        haystack.template ConsumeChars<NC>( cmd.second.Length() );
    }

    return haystack.IsEmpty() || lastWasAsterisk;
}

template void TWildcardMatcher<nchar>::Compile( const TString<nchar>& pattern );
template bool TWildcardMatcher<nchar>::Match  ( const TString<nchar>& haystack, lang::Case sensitivity );
template void TWildcardMatcher<wchar>::Compile( const TString<wchar>& pattern );
template bool TWildcardMatcher<wchar>::Match  ( const TString<wchar>& haystack, lang::Case sensitivity );

#undef   STRING
#undef   ASTERISK
}}} // namespace [alib::strings::util]

