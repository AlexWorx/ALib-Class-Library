// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/expressions/detail/parser_impl.hpp"
#   include "alib/expressions/compiler.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#   include "alib/monomem/aliases/stdvector.hpp"
#endif // !DOXYGEN


namespace alib {  namespace expressions { namespace detail {

// #################################################################################################
// Parser
// #################################################################################################

ParserImpl::ParserImpl( Compiler& pCompiler, MonoAllocator& allocator )
: compileTimeAllocator(allocator)
, compiler            (pCompiler)
, unaryOperators      (allocator)
, binaryOperators     (allocator)
{
    // characters to be known
    syntaxTokens [static_cast<unsigned char>('(')]= true;
    syntaxTokens [static_cast<unsigned char>(')')]= true;
    syntaxTokens [static_cast<unsigned char>(',')]= true;

    operatorChars[static_cast<unsigned char>('?')]= true;
    operatorChars[static_cast<unsigned char>(':')]= true;

    // define unary ops
    for( auto& op : compiler.UnaryOperators )
    {
        ALIB_ASSERT_ERROR( !unaryOperators.Contains(op), "EXPR",
                           "Doubly defined unary operator symbol {!Q'}.", op )

        unaryOperators.EmplaceUnique(op);
        for( auto it : op )
            operatorChars[static_cast<unsigned char>(it)]= true;
    }

    for( auto& op : compiler.AlphabeticUnaryOperatorAliases )
    {
        ALIB_ASSERT_ERROR( !unaryOperators.Contains(op.first), "EXPR",
                           "Doubly defined unary operator symbol {!Q'}.", op.first )

        unaryOperators.EmplaceUnique(op.first);
        if( !isalpha( op.first.CharAtStart() ) )
            for( auto it : op.first )
                operatorChars[static_cast<unsigned char>(it)]= true;
    }


    for( auto op : compiler.BinaryOperators )
    {
        ALIB_ASSERT_ERROR( !binaryOperators.Contains(op.first), "EXPR",
                           "Doubly defined binary operator symbol {!Q'}.", op.first )
        if( op.first == A_CHAR("[]") )
        {
            syntaxTokens[static_cast<unsigned char>('[')]= true;
            syntaxTokens[static_cast<unsigned char>(']')]= true;
        }
        else
        {
            binaryOperators.EmplaceUnique(op.first);
            for( auto it : op.first )
                operatorChars[static_cast<unsigned char>(it)]= true;
        }
    }

    for( auto op : compiler.AlphabeticBinaryOperatorAliases )
    {
        ALIB_ASSERT_ERROR( !binaryOperators.Contains(op.first), "EXPR",
                           "Doubly defined binary operator symbol {!Q'}.", op.first )

        ALIB_DBG( auto originalOp= )
        compiler.BinaryOperators.Find( op.second );
        ALIB_ASSERT_ERROR( originalOp != compiler.BinaryOperators.end(), "EXPR",
                           "Alias {!Q'} defined for unknown operator {!Q'}.",
                           op.first, op.second )

        binaryOperators.EmplaceUnique(op.first);
        if( !isalpha( op.first.CharAtStart() ) )
            for( auto it : op.first )
                operatorChars[static_cast<unsigned char>(it)]= true;
    }
}

// #################################################################################################
//  Lexer
// #################################################################################################
void ParserImpl::NextToken()
{
    scanner.TrimStart();
    tokPosition= expression.Length() - scanner.Length();

    if( scanner.IsEmpty() )
    {
        token= Tokens::EOT;
        return;
    }

    character first= scanner.CharAtStart<NC>();

    //------------------------------  Syntax Tokens ------------------------------
    if( syntaxTokens[static_cast<unsigned char>(first)]  )
    {
        token= Tokens(first);
        scanner.ConsumeChar();
        return;
    }

    //------------------------------  Symbolic operators ------------------------------
    // read up to 3 operator characters
    if( operatorChars[static_cast<unsigned char>(first)] )
    {
        integer operatorLength= 1;
        scanner.ConsumeChar();
        if( operatorChars[static_cast<unsigned char>(scanner.CharAtStart() ) ] )
        {
            scanner.ConsumeChar();
            ++operatorLength;

            if( operatorChars[static_cast<unsigned char>(scanner.CharAtStart() ) ] )
            {
                scanner.ConsumeChar();
                ++operatorLength;
            }
        }

        token= Tokens::SymbolicOp;
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        tokString= String( expression.Buffer() + tokPosition, operatorLength );
        ALIB_WARNINGS_RESTORE

        // special treatment for Elvis with spaces "? :"
        if(    tokString == A_CHAR("?") && compiler.BinaryOperators.Contains( A_CHAR("?:") )   )
        {
            // patch existing token and return
            Substring backup= scanner;
            if( scanner.TrimStart().CharAtStart() == ':' )
            {
                tokString= A_CHAR("?:");
                scanner.ConsumeChar();
            }
            else
                scanner= backup;
        }
        return;
    }

    //------------------------------  alphabetic operators ------------------------------
    if( isalpha( first ) )
    {
        integer len= 1;
        while( len < scanner.Length() && ( isalpha( scanner[len] ) || scanner[len] == '_' ) )
            ++len;
        tokString= scanner.Substring<NC>( 0, len );
        auto hashCode=  tokString.HashcodeIgnoreCase();

        // unary
        {
            decltype(unaryOperators)::Iterator it;
            if(    (it= unaryOperators .Find( tokString, hashCode )) != unaryOperators.end()
                && (    HasBits(compiler.CfgCompilation, Compilation::AlphabeticOperatorsIgnoreCase)
                     || tokString.Equals<NC>( it.Value() ) ) )
            {
                scanner.ConsumeChars<NC>( tokString.Length() );
                token= Tokens::AlphaUnOp;
                return;
            }
        }

        // binary
        {
            decltype(binaryOperators)::Iterator it;
            if(    (it= binaryOperators .Find( tokString, hashCode )) != binaryOperators.end()
                && (    HasBits(compiler.CfgCompilation, Compilation::AlphabeticOperatorsIgnoreCase)
                     || tokString.Equals<NC>( it.Value() ) ) )
            {
                scanner.ConsumeChars<NC>( tokString.Length() );
                token= Tokens::AlphaBinOp;
                return;
            }
        }

    }

    //------------------------------  Identifiers ------------------------------
    if( isalpha( first ) || first == '_' )
    {
        integer endOfIdent= 0;
        character next= 0;
        while(      ++endOfIdent < scanner.Length()
                && (    isalnum( next= scanner[endOfIdent] )
                     || next == '_'                         ) );

        token= Tokens::Identifier;
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        tokString= String( expression.Buffer() + tokPosition, endOfIdent );
        ALIB_WARNINGS_RESTORE
        scanner.ConsumeChars<NC>( endOfIdent );
        return;
    }

    //------------------------------  numbers ------------------------------
    if( isdigit( first ) )
    {
        integer endOfDecPart= 0;
        character next= 0;
        while(      ++endOfDecPart < scanner.Length()
                && (   isdigit( next= scanner[endOfDecPart] )
                     || ( HasBits(numberFormat->Flags, NumberFormatFlags::ReadGroupChars) && next== numberFormat->ThousandsGroupChar ) )
             );


        // float number
        if(    next == numberFormat->DecimalPointChar
            || next == 'e'
            || next == 'E'
            || scanner.Substring( endOfDecPart ).StartsWith( numberFormat->ExponentSeparator ) )

        {
            auto oldStart= scanner.Buffer();
            double value;
            scanner.ConsumeFloat( value, numberFormat );
            token   = Tokens::LitFloat;
            tokFloat= value;

            String numberParsed( oldStart, scanner.Buffer() - oldStart );
            tokLiteralHint=        numberParsed.IndexOf('e') > 0
                                || numberParsed.IndexOf('E') > 0
                                || numberParsed.IndexOf( numberFormat->ExponentSeparator ) > 0
                            ? ASTLiteral::NFHint::Scientific
                            : ASTLiteral::NFHint::NONE;
        }

        // integer number
        else
        {
            tokLiteralHint= ASTLiteral::NFHint::NONE;
            if(         numberFormat->HexLiteralPrefix.IsNotEmpty()
                && scanner.StartsWith( numberFormat->HexLiteralPrefix ) )   tokLiteralHint= ASTLiteral::NFHint::Hexadecimal;
            else if(    numberFormat->OctLiteralPrefix.IsNotEmpty()
                && scanner.StartsWith( numberFormat->OctLiteralPrefix ) )   tokLiteralHint= ASTLiteral::NFHint::Octal;
            else if(    numberFormat->BinLiteralPrefix.IsNotEmpty()
                && scanner.StartsWith( numberFormat->BinLiteralPrefix ) )   tokLiteralHint= ASTLiteral::NFHint::Binary;

            integer value;
            scanner.ConsumeInt( value, numberFormat );
            token= Tokens::LitInteger;
            tokInteger= value;
        }

        return;
    }

    //------------------------------  Strings ------------------------------
    if( first == '"' )
    {
        bool lastWasSlash= false;
        scanner.ConsumeChar<NC>();
        character next;
        while( (next= scanner.ConsumeChar()) != '\0' )
        {
            if( next == '\\' )                  { lastWasSlash= true;   continue;   }
            if( next == '\"' && !lastWasSlash ) break;
            lastWasSlash= false;
        }

        if( next != '"' )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation,
                         EXPRESSIONS.GetResource("EE4")                                   );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                         expression, expression.Length() - scanner.Length() );
            throw e;
        }

        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        String quoted( expression.Buffer() + tokPosition + 1,
                       expression.Length() - scanner.Length() - tokPosition -2 );
        ALIB_WARNINGS_RESTORE
        token    = Tokens::LitString;
        tokString.Allocate(compileTimeAllocator, String1K(quoted) << Format::Escape( lang::Switch::Off ) );
        return;
    }

    // -------- unrecognized token ---------
    Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxError );
    e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, expression.Length() - scanner.Length() );
    throw e;
}



// #################################################################################################
// Parser
// #################################################################################################
#define Start               parseConditional

detail::AST* ParserImpl::Parse( const String& exprString, NumberFormat* nf )
{
    if( exprString.IsEmpty() )
        throw Exception( ALIB_CALLER, Exceptions::EmptyExpressionString );

    expression  = exprString;
    numberFormat= nf;
    ASTs        = compileTimeAllocator().New<StdVectorMono<AST*>>( compileTimeAllocator );
    ASTs->reserve(20);

    // load first token
    scanner=    expression;
    NextToken();

//ALIB_DBG( lexer.DbgListTokens(); )

    AST* ast= Start();


    // if tokens remain, an "operator" would be expected
    if( token != Tokens::EOT )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE5") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    return ast;
}


AST* ParserImpl::parseConditional()
{
    // parse lhs as simple
    push( parseBinary() ); // Q

    integer qmPosition= tokPosition;


    if( token == Tokens::SymbolicOp && tokString == A_CHAR("?") )
    {
        NextToken();
        push( Start() ); // T

        // expect colon
        if( token != Tokens::SymbolicOp || tokString != A_CHAR(":") )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE6") );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
            throw e;
        }
        integer colonPosition= tokPosition;

        NextToken();

        AST* F= Start();
        AST* T= pop();
        AST* Q= pop();
        return compileTimeAllocator().New<ASTConditional>(Q, T, F, qmPosition, colonPosition );
    }

    // was no conditional
    return pop();
}

AST* ParserImpl::parseBinary()
{
    // parse lhs as simple
    push( parseSimple() );

    // parse
    integer position= tokPosition;
    String binOp;
    for( ;; )
    {
        binOp= getBinaryOp();
        if( binOp.IsNull() )
            return pop();

        // rhs is braced? -> lhs becomes <lhs op rhs> and we start over
        if( token == Tokens::BraceOpen )
        {
            replace( compileTimeAllocator().New<ASTBinaryOp>(binOp, top(), parseSimple(), position ) );
            position= tokPosition;
            continue;
        }
        break;
    }

    // check if tokens remain
    if( token == Tokens::EOT )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE7") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    AST* lhs= top();
    AST* rhs= push( parseBinary() );

    int             binOpPrecedence= compiler.GetBinaryOperatorPrecedence( binOp );
    AST*            replace        = rhs;
    ASTBinaryOp*    parent         = nullptr;
    while(    replace->NodeType == AST::Types::BinaryOp
           && compiler.GetBinaryOperatorPrecedence(dynamic_cast<ASTBinaryOp*>(replace)->Operator) <= binOpPrecedence )
    {
        parent = dynamic_cast<ASTBinaryOp*>(replace);
        replace= parent->Lhs;
    }

    pop();
    pop();
    if( parent == nullptr )
        return compileTimeAllocator().New<ASTBinaryOp>( binOp, lhs, rhs, position );

    // insert binary at lhs of deepest equal-level binary found.
    // Its current lhs becomes its new lhs-child's rhs.
    parent->Lhs=  compileTimeAllocator().New<ASTBinaryOp>( binOp, lhs, parent->Lhs, position );
    return rhs;
}

AST* ParserImpl::parseSimple()
{
    //  '('  expr  ')'   (brackets)
    if( token == Tokens::BraceOpen )
    {
        NextToken();
        push( Start() );

        if( token != Tokens::BraceClose )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE1"));
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
            throw e;
        }
        NextToken();
        replace( parseSubscript( top() ) );
        return pop();
    }

    //  unary operator
    integer position= tokPosition;
    {
        String unOp= getUnaryOp();
        if( unOp.IsNotNull() )
        {
            push( compileTimeAllocator().New<ASTUnaryOp>(unOp, parseSimple(), position ) );
            replace( parseSubscript( top() ) );
            return pop();
        }
    }

    // terminals
    if( token == Tokens::LitInteger ) { push(compileTimeAllocator().New<ASTLiteral>(tokInteger, position, tokLiteralHint )             ); NextToken(); replace( parseSubscript(top()) ); return pop(); }
    if( token == Tokens::LitFloat   ) { push(compileTimeAllocator().New<ASTLiteral>(tokFloat  , position, tokLiteralHint )             ); NextToken(); replace( parseSubscript(top()) ); return pop(); }
    if( token == Tokens::LitString  ) { push(compileTimeAllocator().New<ASTLiteral>(String(compileTimeAllocator, tokString), position )); NextToken(); replace( parseSubscript(top()) ); return pop(); }
    if( token == Tokens::Identifier || token == Tokens::AlphaBinOp )   // allow bin op's names here! This is tricky but right!
    {
        String name= tokString;
        NextToken();

        // function
        if( token == Tokens::BraceOpen )
        {
            ASTFunction* astFunction= compileTimeAllocator().New<ASTFunction>( name, position, compileTimeAllocator );
            push( astFunction );
            for(;;)
            {
                NextToken();
                if( token == Tokens::BraceClose )
                {
                    NextToken();
                    return pop();
                }
                astFunction->Arguments.EmplaceBack( Start() );

                if( token == Tokens::Comma )
                    continue;

                if( token != Tokens::BraceClose )
                {
                    Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE2") );
                    e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
                    throw e;
                }

                NextToken();
                replace( parseSubscript( astFunction ) );
                return pop();
            }
        }

        // identifier
        replace( parseSubscript( push(compileTimeAllocator().New<ASTIdentifier>( String(compileTimeAllocator, name), position ) ) ) );
        return pop();
    }

    // ----------------------------------------   ERRORS   -----------------------------------------
    if( token == Tokens::EOT )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE20") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    if( token == Tokens::BraceClose )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE21") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    if( token == Tokens::SubscriptOpen || token == Tokens::SubscriptClose )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE22") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    if( token == Tokens::Comma )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE23") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    ALIB_ERROR( "EXPR", "Internal error. This should never happen.")
    return nullptr;
}

AST* ParserImpl::parseSubscript( AST *function )
{
    if(    !HasBits( compiler.CfgCompilation, Compilation::AllowSubscriptOperator )
        || token != Tokens::SubscriptOpen )
        return function;

    integer position= tokPosition;

    NextToken();

    push( Start() );

    if( token != Tokens::SubscriptClose )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("EE3") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    // success
    NextToken();
    return compileTimeAllocator().New<ASTBinaryOp>( A_CHAR("[]"), function, pop(), position );
}


// #################################################################################################
// Helpers
// #################################################################################################


String ParserImpl::getUnaryOp()
{
    if( token == Tokens::SymbolicOp )
    {
        // symbolic unary ops may be nested. Hence, we find one by one from the actual token and consume the
        // token only if all is consumed.
        for( integer partialRead= 1 ; partialRead <= tokString.Length()    ; ++partialRead )
        {
            Substring key= Substring( tokString.Buffer(), partialRead );
            if( unaryOperators.Contains( key ) )
            {
                if( partialRead == tokString.Length() )
                    NextToken();
                else
                {
                    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                    tokString= String( tokString.Buffer() + partialRead,
                                       tokString.Length() - partialRead );
                    tokPosition+= partialRead;
                    ALIB_WARNINGS_RESTORE
                }
                return key;
            }
        }
        Exception e( ALIB_CALLER_NULLED, Exceptions::UnknownUnaryOperatorSymbol, tokString );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }
    else if ( token == Tokens::AlphaUnOp )
    {
        String alphabeticOperator= tokString;
        NextToken();
        return alphabeticOperator;
    }

    return NULL_STRING;
}

String ParserImpl::getBinaryOp()
{
    if ( token == Tokens::SymbolicOp )
    {
        // ignore ternary
        if ( tokString == A_CHAR( "?" ) || tokString == A_CHAR( ":" ) )
            return NULL_STRING;

        // binary ops may be longer and concatenated with unaries. So we consume as much as possible
        // but are happy with less than available
        for ( integer partialRead = tokString.Length(); partialRead > 0; --partialRead )
        {
            Substring key = Substring( tokString.Buffer(), partialRead );
            if ( binaryOperators.Contains( key ) )
            {
                if ( partialRead == tokString.Length() )
                    NextToken();
                else
                {
                    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                    tokString = String( tokString.Buffer() + partialRead,
                                        tokString.Length() - partialRead );
                    tokPosition += partialRead;
                    ALIB_WARNINGS_RESTORE
                }
                return key;
            }
        }

        Exception e( ALIB_CALLER_NULLED, Exceptions::UnknownBinaryOperatorSymbol, tokString );
        e.Add( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }
    else if ( token == Tokens::AlphaBinOp )
    {
        String alphabeticOperator= tokString;
        NextToken();
        return alphabeticOperator;
    }

    return NULL_STRING;
}


#undef Start

}}} // namespace [alib::expressions::detail]
