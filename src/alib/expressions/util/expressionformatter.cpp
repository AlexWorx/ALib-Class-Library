// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_EXPRESSIONS_UTIL_EXPRESSION_FORMATTER)
#      include "alib/expressions/util/expressionformatter.hpp"
#   endif
#   if !defined(HPP_ALIB_TEXT_TEXT)
#      include "alib/text/text.hpp"
#   endif

#   if !defined (HPP_ALIB_EXPRESSIONS_COMPILER)
#       include "alib/expressions/compiler.hpp"
#   endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace expressions { namespace util {

ExpressionFormatter::ExpressionFormatter( const String   customFormatString,
                                          Compiler*      pCompiler,
                                          SPFormatter    formatter,
                                          character      ESCCharacter,
                                          character      ESCOpeningBracket,
                                          character      ESCClosingBracket     )
: compiler            ( pCompiler )
, stdFormatter        ( formatter )
, originalFormatString( customFormatString )
, formatString        ( customFormatString )
{
    if(!formatter.get())
        stdFormatter= Formatter::GetDefault();

    integer parsePos= 0;
    while(parsePos < formatString.Length() )
    {
        // has next parse position?
        integer parsePosCopy= parsePos;
        if( ( parsePos= formatString.IndexOf( ESCCharacter, parsePosCopy ) ) < 0 )
            break;

        String expressionString;
        integer endPos= parsePos+ 1;
        if( endPos < formatString.Length() )
        {
            // double Escape character? -> replace to single!
            if( formatString[endPos] == ESCCharacter )
            {
                formatString.Delete( endPos, 1 );
                ++parsePos;
                continue;
            }

            // find end of expression string
            if( formatString[endPos] == ESCOpeningBracket )
            {
                endPos= formatString.IndexOfSegmentEnd( ESCOpeningBracket, ESCClosingBracket, endPos + 1 );
                expressionString= formatString.Substring( parsePos + 2, endPos - parsePos - 2 );
                ++endPos;
            }
            else
            {
                while( endPos < formatString.Length()  && isalpha( formatString[endPos] ) )
                    ++endPos;
                expressionString= formatString.Substring( parsePos + 1, endPos - parsePos - 1 );
            }
        }

        // add expression
        try
        {
            expressions.emplace_back( compiler->Compile( expressionString ) );

        }
        catch( Exception& e)
        {
             e.Add( ALIB_CALLER_NULLED, Exceptions::InExpressionFormatter,
                    expressions.size() + 1, originalFormatString );
             throw;
        }

        // delete expression from format string
        formatString.Delete( parsePos, endPos - parsePos
                                       + (    endPos < formatString.Length()
                                           && formatString[endPos] == ESCCharacter ? 1 : 0 )  );

        ++parsePos;
    }
}


void    ExpressionFormatter::Format( AString& target, expressions::Scope&  scope )
{
    // evaluate expressions and collect boxes
    auto& results= stdFormatter->Acquire(ALIB_CALLER_PRUNED);
    results.Add( formatString );

    try
    {
        for( size_t expressionNo= 0; expressionNo < expressions.size() ; ++expressionNo )
            results.Add( expressions[expressionNo]->Evaluate( scope ) );
    }
    catch( Exception& e)
    {
        stdFormatter->Release();
        e.Add( ALIB_CALLER_NULLED, Exceptions::InExpressionFormatter,
               expressions.size() + 1, originalFormatString );
        throw;
    }

    try
    {
        stdFormatter->FormatArgs( target );
    }
    catch(Exception& e)
    {
        stdFormatter->Release();
        e.Add( ALIB_CALLER_NULLED, text::Exceptions::ErrorInResultingFormatString,
               originalFormatString );
        throw;
    }

    stdFormatter->Release();
}


}}}} // namespace [aworx::lib::expressions::util]
