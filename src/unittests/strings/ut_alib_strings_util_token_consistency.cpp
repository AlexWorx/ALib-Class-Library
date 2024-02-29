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

#if !defined (HPP_ALIB_LANG_FORMAT_PARAGRAPHS)
    #include "alib/lang/format/paragraphs.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER)
    #include "alib/strings/util/wildcardmatcher.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_UTIL_SUBSEARCH)
    #include "alib/strings/util/subsearch.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKEN)
    #include "alib/strings/util/token.hpp"
#endif


#include "unittests/aworx_unittests.hpp"


using namespace std;
using namespace alib;

namespace ut_aworx {

//--------------------------------------------------------------------------------------------------
//--- This test function can be used by unit tests to feed in all tokens which are used
//--- in a same context and may this way "collide".
//--- The function generates the minimum abbreviation of each token and tests this string
//--- against all other tokens. None of them should match!
//--------------------------------------------------------------------------------------------------
ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
void TestTokenConsistency(AWorxUnitTesting& ut, Token* tokens, int qtyTokens )
{
    String256 abbreviation;
    for( int i= 0 ; i < qtyTokens ; ++i )
    {
        Token& lhs     = tokens[i];
        auto&  name    = lhs.GetRawName();
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
                    UT_PRINT( "  Internal error: token does not match against its abbreviation {!AW} of token {!Q}",
                              String256("\"")._(abbreviation)._('"'), rhs.GetRawName() )
                    UT_TRUE(false)
                }
                continue;
            }

            if( rhs.Match( abbreviation ) )
            {
                UT_PRINT( "  Warning: Abbreviation {!AW!Q} of token {!AW!Q} matches against token {!AW!Q}",
                          abbreviation, name, rhs.GetRawName() )
            }

        }
    }
}
ALIB_WARNINGS_RESTORE


} //namespace

#include "unittests/aworx_unittests_end.hpp"

#endif // ALIB_UT_STRINGS
