// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/expressions/util/expressionformatter.hpp"
#   include "alib/lang/format/fmtexceptions.hpp"
#   include "alib/expressions/compiler.hpp"
#endif // !DOXYGEN

/// Utility types of camp \alib_expressions.
namespace alib::expressions::util {

ExpressionFormatter::ExpressionFormatter( const String   pFormatString,
                                          Compiler*      pCompiler,
                                          SPFormatter    formatter,
                                          character      separatorChar    )
: compiler            ( pCompiler )
, stdFormatter        ( formatter )
, originalFormatString( pFormatString )
{
    // use ALib standard formatter, if no dedicated instance was given.
    if(!formatter.Get())
        stdFormatter= Formatter::Default;

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
    auto& results= stdFormatter->GetArgContainer();
    results.Add( formatStringStripped );

    try
    {
        for( size_t expressionNo= 0; expressionNo < expressions.size() ; ++expressionNo )
            results.Add( expressions[expressionNo]->Evaluate( scope ) );
    }
    catch( Exception& e)
    {
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
        e.Add( ALIB_CALLER_NULLED,  lang::format::FMTExceptions::ErrorInResultingFormatString,
               originalFormatString );
        throw;
    }
}

} // namespace [alib::expressions::util]

