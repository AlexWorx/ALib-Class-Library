// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PARSER_IMPL)
#   include "alib/expressions/detail/parser_impl.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_COMPILER)
#   include "alib/expressions/compiler.hpp"
#endif


namespace aworx { namespace lib { namespace expressions { namespace detail {

// #################################################################################################
// Parser
// #################################################################################################

ParserImpl::ParserImpl( Compiler& pCompiler  )
: compiler           (pCompiler)
, internalizedStrings(128)
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
        ALIB_ASSERT_ERROR( unaryOperators.find(op) == unaryOperators.end(),
                           "Doubly defined unary operator symbol '{}'.", op )

        unaryOperators[op]= 0;
        for( auto it : op )
            operatorChars[static_cast<unsigned char>(it)]= true;
    }

    for( auto& op : compiler.AlphabeticUnaryOperatorAliases )
    {
        ALIB_ASSERT_ERROR( unaryOperators.find(op.first) == unaryOperators.end(),
                           "Doubly defined unary operator symbol '{}'.", op.first )

        unaryOperators[op.first]= 0;
        if( !isalpha( op.first.CharAtStart() ) )
            for( auto it : op.first )
                operatorChars[static_cast<unsigned char>(it)]= true;
    }


    for( auto op : compiler.BinaryOperators )
    {
        ALIB_ASSERT_ERROR( binaryOperators.find(op.first) == binaryOperators.end(),
                           "Doubly defined binary operator symbol '{}'.", op.first )
        if( op.first == A_CHAR("[]") )
        {
            syntaxTokens[static_cast<unsigned char>('[')]= true;
            syntaxTokens[static_cast<unsigned char>(']')]= true;
        }
        else
        {
            binaryOperators[op.first]= op.second;
            for( auto it : op.first )
                operatorChars[static_cast<unsigned char>(it)]= true;
        }
    }

    for( auto op : compiler.AlphabeticBinaryOperatorAliases )
    {
        ALIB_ASSERT_ERROR( binaryOperators.find(op.first) == binaryOperators.end(),
                           "Doubly defined binary operator symbol '{}'.", op.first )

        auto originalOp= compiler.BinaryOperators.find( op.second );
        ALIB_ASSERT_ERROR( originalOp != compiler.BinaryOperators.end(),
                           "Alias '{}' defined for unknown operator '{}'.",
                           op.first, op.second )

        binaryOperators[op.first]= originalOp->second;
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

    character first= scanner.CharAtStart<false>();

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
            operatorLength++;

            if( operatorChars[static_cast<unsigned char>(scanner.CharAtStart() ) ] )
            {
                scanner.ConsumeChar();
                operatorLength++;
            }
        }

        token= Tokens::Operator;
        tokString= String( expression.Buffer() + tokPosition, operatorLength );

        // special treatment for Elvis with spaces "? :"
        if(    tokString == A_CHAR("?")
            && compiler.BinaryOperators.find( A_CHAR("?:") ) != compiler.BinaryOperators.end()   )
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
        Substring lookAhead( scanner );
        lookAhead.ConsumeChar<false>();
        while( isalpha( lookAhead.CharAtStart() ) || lookAhead.CharAtStart() =='_' )
            lookAhead.ConsumeChar<false>();
        tokString= scanner.Substring<false>( 0, scanner.Length() - lookAhead.Length() );
        UnorderedStringMapIgnoreCase<int>::iterator it;
        if(   (   (it= unaryOperators .find( tokString )) != unaryOperators.end()
               && (    EnumContains(compiler.CfgCompilation, Compilation::AlphabeticOperatorsIgnoreCase)
                    || tokString.Equals( it->first ) ) )
           || (   (it= binaryOperators.find( tokString )) != binaryOperators.end()
               && (    EnumContains(compiler.CfgCompilation, Compilation::AlphabeticOperatorsIgnoreCase)
                    || tokString.Equals( it->first ) ) )
            )
        {
            scanner.ConsumeChars<false>( tokString.Length() );
            token= Tokens::Operator;
            return;
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
        tokString= String( expression.Buffer() + tokPosition, endOfIdent );
        scanner.ConsumeChars<false>( endOfIdent );
        return;
    }

    //------------------------------  numbers ------------------------------
    if( isdigit( first ) )
    {
        integer endOfDecPart= 0;
        character next= 0;
        while(      ++endOfDecPart < scanner.Length()
                && (   isdigit( next= scanner[endOfDecPart] )
                     || ( numberFormat->ReadGroupChars && next== numberFormat->ThousandsGroupChar ) )
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
        scanner.ConsumeChar<false>();
        character next;
        while( (next= scanner.ConsumeChar()) != '\0' )
        {
            if( next == '\\' )                  { lastWasSlash= true;   continue;   }
            if( next == '\"' && !lastWasSlash ) break;
            lastWasSlash= false;
        };

        if( next != '"' )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation,
                         EXPRESSIONS.GetResource("ExcExp4")                                   );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                         expression, expression.Length() - scanner.Length() );
            throw e;
        }

        String quoted( expression.Buffer() + tokPosition + 1,
                       expression.Length() - scanner.Length() - tokPosition -2 );


        AString internalizer;
        internalizer.SetBuffer( internalizedStrings.AllocArray<character>( quoted.Length() + 1 ),
                                quoted.Length() + 1,
                                0,
                                Responsibility::KeepWithSender );
        internalizer << quoted << Format::Escape( Switch::Off );
        token    = Tokens::LitString;
        tokString= internalizer;
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

detail::AST* ParserImpl::Parse( const String& expressionString, NumberFormat* nf  )
{
    if( expressionString.IsEmpty() )
        throw Exception( ALIB_CALLER, Exceptions::EmptyExpressionString );

    expression= expressionString;
    numberFormat= nf;
    internalizedStrings.Reset();

    // load first token
    scanner=    expression;
    NextToken();

//ALIB_DBG( lexer.DbgListTokens(); )

    AST* ast= nullptr;
    try
    {
        ast= Start();
    }
    catch(Exception&)
    {
        for( AST* undone : ASTs )
            delete undone;
        ASTs.clear();
        throw;
    }
    ALIB_ASSERT_ERROR( ASTs.size() == 0, "AST stack not empty. This must never happen." )


    // check if tokens remain
    if( token != Tokens::EOT )
    {
        delete ast;
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp5") );
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


    if(     token        == Tokens::Operator
        &&  tokString == A_CHAR("?")           )
    {
        NextToken();
        push( Start() ); // T

        // expect colon
        if(    token        != Tokens::Operator
            || tokString != A_CHAR(":")          )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp6") );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
            throw e;
        }
        integer colonPosition= tokPosition;

        NextToken();

        AST* F= Start();
        AST* T= pop();
        AST* Q= pop();
        return new ASTConditional( Q, T, F, qmPosition, colonPosition );
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
            replace( new ASTBinaryOp( binOp, top(), parseSimple(), position ) );
            position= tokPosition;
            continue;
        }
        break;
    }

   // check if tokens remain
    if( token == Tokens::EOT )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp7") );
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
        return new ASTBinaryOp( binOp, lhs, rhs, position );

    // insert binary at lhs of deepest equal-level binary found.
    // Its current lhs becomes its new lhs-child's rhs.
    parent->Lhs=  new ASTBinaryOp( binOp, lhs, parent->Lhs, position );
    return rhs;
}

AST* ParserImpl::parseSimple()
{
    //  '('  expr  ')'
    if( token == Tokens::BraceOpen )
    {
        NextToken();
        push( Start() );

        if( token != Tokens::BraceClose )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp1"));
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
            throw e;
        }
        NextToken();
        replace( parseSubscript( top() ) );
        return pop();
    }

    integer position= tokPosition;
    {
        String unOp= getUnaryOp();
        if( unOp.IsNotNull() )
        {
            push( new ASTUnaryOp( unOp, parseSimple(), position ) );
            replace( parseSubscript( top() ) );
            return pop();
        }
    }


    if( token == Tokens::LitInteger ) { push(new ASTLiteral( tokInteger, position, tokLiteralHint )); NextToken(); replace( parseSubscript(top()) ); return pop(); }
    if( token == Tokens::LitFloat   ) { push(new ASTLiteral( tokFloat  , position, tokLiteralHint )); NextToken(); replace( parseSubscript(top()) ); return pop(); }
    if( token == Tokens::LitString  ) { push(new ASTLiteral( tokString , position                 )); NextToken(); replace( parseSubscript(top()) ); return pop(); }

    if( token == Tokens::Identifier )
    {
        String name= tokString;
        NextToken();

        // function
        if( token == Tokens::BraceOpen )
        {
            ASTFunction* astFunction= new ASTFunction( name, position );
            push( astFunction );
            for(;;)
            {
                NextToken();
                if( token == Tokens::BraceClose )
                {
                    NextToken();
                    return pop();
                }
                astFunction->Arguments.emplace_back( Start() );

                if( token == Tokens::Comma )
                    continue;

                if( token != Tokens::BraceClose )
                {
                    Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp2") );
                    e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
                    throw e;
                }

                NextToken();
                replace( parseSubscript( astFunction ) );
                return pop();
            }
        }

        // identifier
        replace( parseSubscript( push(new ASTIdentifier( name, position ) ) ) );
        return pop();
    }

    if( token == Tokens::EOT )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp20") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    if( token == Tokens::BraceClose )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp21") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    if( token == Tokens::SubscriptOpen || token == Tokens::SubscriptClose )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp22") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    if( token == Tokens::Comma )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp23") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }


    ALIB_ERROR( "Internal parser Error. This should never happen");
    return nullptr;
}

AST* ParserImpl::parseSubscript( AST *function )
{
    if(    !EnumContains( compiler.CfgCompilation, Compilation::AllowSubscriptOperator )
        || token != Tokens::SubscriptOpen )
        return function;

    integer position= tokPosition;

    NextToken();

    push( Start() );

    if( token != Tokens::SubscriptClose )
    {
        Exception e( ALIB_CALLER_NULLED, Exceptions::SyntaxErrorExpectation, EXPRESSIONS.GetResource("ExcExp3") );
        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
        throw e;
    }

    // success
    NextToken();
    return new ASTBinaryOp( A_CHAR("[]"), function, pop(), position );
}


// #################################################################################################
// Helpers
// #################################################################################################


String ParserImpl::getUnaryOp()
{
    if( token != Tokens::Operator )
        return NullString();

    // unary ops may be nested. Hence, we find one by one from the actual token and consume the
    // token only if all is consumed.
    for( integer partialRead= 1 ; partialRead <= tokString.Length()    ; ++partialRead )
    {
        Substring key= Substring( tokString.Buffer(), partialRead );
        auto entry= unaryOperators.find( key );
        if( entry != unaryOperators.end() )
        {
            if( partialRead == tokString.Length() )
                NextToken();
            else
            {
                tokString= String( tokString.Buffer() + partialRead,
                                              tokString.Length() - partialRead );
                tokPosition+= partialRead;
            }
            return key;
        }
    }

    Exception e( ALIB_CALLER_NULLED, Exceptions::UnknownUnaryOperatorSymbol, tokString );
    e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
    throw e;
}

String ParserImpl::getBinaryOp()
{
    if( token != Tokens::Operator )
        return NullString();

    // ignore ternary
    if(    tokString == A_CHAR("?")
        || tokString == A_CHAR(":") )
        return NullString();

    // binary ops may be longer and concatenated with unaries. So we consume as much as possible
    // but are happy with less than available
    for( integer partialRead= tokString.Length() ; partialRead > 0    ; --partialRead )
    {
        Substring key= Substring( tokString.Buffer(), partialRead );
        auto entry= binaryOperators.find( key );
        if( entry != binaryOperators.end() )
        {
            if( partialRead == tokString.Length() )
                NextToken();
            else
            {
                tokString= String( tokString.Buffer() + partialRead,
                                   tokString.Length() - partialRead );
                tokPosition+= partialRead;
            }
            return key;
        }
    }

    Exception e( ALIB_CALLER_NULLED, Exceptions::UnknownBinaryOperatorSymbol, tokString );
    e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression, tokPosition );
    throw e;
}


#undef Start

}}}} // namespace [aworx::lib::expressions::detail]

