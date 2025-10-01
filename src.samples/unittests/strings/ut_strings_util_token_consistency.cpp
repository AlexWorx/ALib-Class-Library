// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_STRINGS || ALIB_UT_EXPRESSIONS

#if defined(QT_CORE_LIB) // needed here for unity builds to be included before boxing (!)
#   include "ALib.Compatibility.QTCharacters.H"
#endif

#include "ALib.ALox.H"
#include "ALib.Strings.Token.H"
#include "ALib.Format.Paragraphs.H"
#include "ALib.Format.Paragraphs.H"

#include "aworx_unittests.hpp"

using namespace std;
using namespace alib;


namespace ut_aworx {
#include "ALib.Lang.CIFunctions.H"
//--------------------------------------------------------------------------------------------------
//--- This test function can be used by unit tests to feed in all tokens which are used
//--- in a same context and may this way "collide".
//--- The function generates the minimum abbreviation of each token and tests this string
//--- against all other tokens. None of them should match!
//--------------------------------------------------------------------------------------------------
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

#include "ALib.Lang.CIMethods.H"

} // namespace ut_aworx

#include "aworx_unittests_end.hpp"

#endif // ALIB_UT_STRINGS
