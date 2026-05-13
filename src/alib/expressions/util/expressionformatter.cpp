/// Utility types of camp \alib_expressions.
namespace alib::expressions::util {

ExpressionFormatter::ExpressionFormatter( const String&  pFormatString,
                                          Compiler*      pCompiler,
                                          SPFormatter    formatter,
                                          character      separatorChar    )
: compiler            ( pCompiler )
, stdFormatter        ( formatter )
, originalFormatString( pFormatString ) {
    // use ALib standard formatter, if no dedicated instance was given.
    if(!formatter.Get())
        stdFormatter= Formatter::DEFAULT;

    String256 escapedFS; escapedFS.DbgDisableBufferReplacementWarning();
    StringEscaperStandard().Escape(pFormatString, escapedFS, EMPTY_STRING);

    // parse format string
    integer nonExprPortionStart= 0;
    integer parsePos           = 0;
    while(parsePos < escapedFS.Length() ) {
        // has a next parse position?
        // Note: if bracket is found at the end of string, we just ignore this here. An according
        // exception is thrown in formatter later.
        if(    ( parsePos= escapedFS.IndexOf( A_CHAR('{'), parsePos ) ) < 0
            ||   parsePos == escapedFS.Length() - 1 )
        {
            formatStringStripped << escapedFS.Substring( nonExprPortionStart );
            break;
        }

        // double Escape character? -> ignore
        ++parsePos;
        if( escapedFS[parsePos] == A_CHAR('{') ) {
            ++parsePos;
            continue;
        }

        // add the current portion to format string
        formatStringStripped << escapedFS.Substring( nonExprPortionStart, parsePos - nonExprPortionStart );
        formatSubstrings.push_back(formatStringStripped.Length() -1);
        
        // Either find separator character or closing bracket of placeholder
        integer endPos= parsePos;
        while(      endPos < escapedFS.Length() -1
               &&   escapedFS[endPos] != separatorChar
               &&   escapedFS[endPos] != A_CHAR('}')     )
            ++endPos;

        // extract expression string and set start of non-expression portion
        String expressionString= escapedFS.Substring( parsePos, endPos - parsePos );
        nonExprPortionStart= endPos;
        if( escapedFS[endPos] == separatorChar )
            ++nonExprPortionStart;

        // add expression
        try { expressions.emplace_back( compiler->Compile( expressionString ) ); }
        catch( Exception& e) {
             e.Add( ALIB_CALLER_NULLED, Exceptions::InExpressionFormatter,
                    expressions.size() + 1, escapedFS );
             throw;
    }   }
    formatSubstrings.push_back(formatStringStripped.Length());
}


void    ExpressionFormatter::Format( AString& target, expressions::Scope&  scope ) {
    // evaluate expressions and collect boxes
    try {
        // the first string is the start-substring.
        // Attn: must be done with formatter to unescape the string.
        stdFormatter->Format( target, formatStringStripped.Substring(0, formatSubstrings[0]));
        size_t idx= 1;

        for( size_t expressionNo= 0; expressionNo < expressions.size() ; ++expressionNo ) {
            Box exprResult=  expressions[expressionNo]->Evaluate( scope );

            try {
                String formatString= formatStringStripped.Substring( formatSubstrings[idx-1],
                                       formatSubstrings[idx] - formatSubstrings[idx-1] );
                stdFormatter->Format( target, formatString, exprResult );
                ++idx;
            }
            catch(Exception& e) {
                e.Add( ALIB_CALLER_NULLED,  format::FMTExceptions::ErrorInResultingFormatString,
                       originalFormatString );
                throw;
    }   }   }
    catch( Exception& e)
    {
        e.Add( ALIB_CALLER_NULLED, Exceptions::InExpressionFormatter,
               expressions.size() + 1, originalFormatString );
        throw;
    }

}

} // namespace [alib::expressions::util]
