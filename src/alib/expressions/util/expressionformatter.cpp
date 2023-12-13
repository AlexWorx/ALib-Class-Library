// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
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

ExpressionFormatter::ExpressionFormatter( const String   pFormatString,
                                          Compiler*      pCompiler,
                                          SPFormatter    formatter,
                                          character      separatorChar    )
: compiler            ( pCompiler )
, stdFormatter        ( formatter )
, originalFormatString( pFormatString )
{
    // use ALib standard formatter, if no dedicated instance was given.
    if(!formatter.get())
        stdFormatter= Formatter::GetDefault();

    // parse format string
    integer nonExprPortionStart= 0;
    integer parsePos           = 0;
    while(parsePos < originalFormatString.Length() )
    {
        // has next parse position?
        // Note: if bracket is found at the end of string, we just ignore this here. An according
        // exception is thrown in formatter later.
        if(    ( parsePos= originalFormatString.IndexOf( A_CHAR('{'), parsePos ) ) < 0
            ||   parsePos == originalFormatString.Length() - 1 )
        {
            formatStringStripped << originalFormatString.Substring( nonExprPortionStart );
            break;
        }

        // double Escape character? -> ignore
        ++parsePos;
        if( originalFormatString[parsePos] == A_CHAR('{') )
        {
            ++parsePos;
            continue;
        }

        // add current portion to format string
        formatStringStripped << originalFormatString.Substring( nonExprPortionStart, parsePos - nonExprPortionStart );

        // Either find separator character or closing bracket of placeholder
        integer endPos= parsePos;
        while(      endPos < originalFormatString.Length() -1
               &&   originalFormatString[endPos] != separatorChar
               &&   originalFormatString[endPos] != A_CHAR('}')     )
            ++endPos;

        // extract expression string and set start of non-expression portion
        String expressionString= originalFormatString.Substring( parsePos, endPos - parsePos );
        nonExprPortionStart= endPos;
        if( originalFormatString[endPos] == separatorChar )
            ++nonExprPortionStart;

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
    }
}


void    ExpressionFormatter::Format( AString& target, expressions::Scope&  scope )
{
    // evaluate expressions and collect boxes
    auto& results= stdFormatter->Acquire(ALIB_CALLER_PRUNED);
    results.Add( formatStringStripped );

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
