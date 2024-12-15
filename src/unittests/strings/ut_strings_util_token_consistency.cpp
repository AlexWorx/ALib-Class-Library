// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_STRINGS || ALIB_UT_EXPRESSIONS

#if defined(QT_CORE_LIB) // needed here for unity builds to be included before boxing (!)
#   include "alib/compatibility/qt_characters.hpp"
#endif

#include "alib/alox.hpp"

#include "alib/lang/format/paragraphs.hpp"
#include "alib/strings/util/wildcardmatcher.hpp"
#include "alib/strings/util/subsearch.hpp"
#include "alib/strings/util/token.hpp"

#include "unittests/aworx_unittests.hpp"

using namespace std;
using namespace alib;


namespace ut_aworx {
#include "alib/lang/callerinfo_functions.hpp"
//--------------------------------------------------------------------------------------------------
//--- This test function can be used by unit tests to feed in all tokens which are used
//--- in a same context and may this way "collide".
//--- The function generates the minimum abbreviation of each token and tests this string
//--- against all other tokens. None of them should match!
//--------------------------------------------------------------------------------------------------
ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
int TestTokenConsistency(AWorxUnitTesting& ut, Token* tokens, int qtyTokens )
{
    int qtyWarnings= 0;
    String256 abbreviation;
    for( int i= 0 ; i < qtyTokens ; ++i )
    {
        Token& lhs     = tokens[i];
        auto&  name    = lhs.GetDefinitionName();
        auto   format  = lhs.GetFormat();
        abbreviation.Reset();
        int       segmentNo      = 0;
        int       segmentLength  = 0;
        int       segMinLen      = lhs.GetMinLength(0);
        for( int cIdx= 0 ; cIdx < name.Length() ; ++cIdx )
        {
            auto c= name.CharAt( cIdx );

            if( segmentLength < segMinLen )
                abbreviation << c;

            ++segmentLength;

            // segment end?
            if(     cIdx!=0
                && segmentLength != 1
                &&  (    (format == Token::Formats::SnakeCase && c == '_' )
                      || (format == Token::Formats::KebabCase && c == '-' )
                      || (format == Token::Formats::CamelCase && isalpha(c) && isupper(c) ) ) )
            {
                if( format == Token::Formats::CamelCase )
                    --cIdx;

                if( c=='_' || c=='-' )
                    abbreviation << c;

                ++segmentNo;
                segmentLength= 0;
                segMinLen    = segmentNo < 7 ? lhs.GetMinLength(segmentNo) : 1000;
                continue;
            }

        }

        //UT_PRINT( "Testing all tokens against abbreviation {!AW} of token {!Q}",
        //          String256("\"")._(abbreviation)._('"'), name                     );
        for( int j= 0 ; j < qtyTokens ; ++j )
        {
            Token& rhs= tokens[j];
            if ( i == j )
            {
                if( !rhs.Match( abbreviation ) )
                {
                    ++qtyWarnings;
                    UT_PRINT( "  Internal error: token does not match against its abbreviation {!AW} of token {!Q}",
                              String256("\"")._(abbreviation)._('"'), rhs.GetDefinitionName() )
                    UT_TRUE(false)
                }
                continue;
            }

            if( rhs.Match( abbreviation ) )
            {
                ++qtyWarnings;
                UT_PRINT( "  Warning: Abbreviation {!AW!Q} of token {!AW!Q} matches against token {!AW!Q}",
                          abbreviation, name, rhs.GetDefinitionName() )
            }

        }
        
    }

    return qtyWarnings;
}
ALIB_WARNINGS_RESTORE

#include "alib/lang/callerinfo_methods.hpp"

} // namespace ut_aworx

#include "unittests/aworx_unittests_end.hpp"

#endif // ALIB_UT_STRINGS
