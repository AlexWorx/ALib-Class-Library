// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_AST)
#      include "alib/expressions/detail/ast.hpp"
#   endif

#   if !defined (HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN)
#      include "alib/expressions/compilerplugin.hpp"
#   endif

#   if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM)
#      include "alib/expressions/detail/program.hpp"
#   endif

#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif

#endif // !defined(ALIB_DOX)


namespace aworx { namespace lib { namespace expressions { namespace detail {



// #################################################################################################
// Anonymous helpers
// #################################################################################################
//! @cond NO_DOX
namespace {

const String normSpace(A_CHAR(" "));
const String normBracketOpen[4]  {A_CHAR("("), A_CHAR("( "), A_CHAR(" ("), A_CHAR(" ( ")};
const String normBracketClose[4] {A_CHAR(")"), A_CHAR(" )"), A_CHAR(") "), A_CHAR(" ) ")};

#define      SPACE(flag)          ( HasBits(format, Normalization::flag ) ? normSpace : EmptyString() )
#define COND_SPACE(flag, force) if( HasBits(format, Normalization::flag ) || force ) normalized << ' '

void checkForbiddenStrings( Compiler& compiler, AString& normalized, integer positionToCheck, integer spaceInsertionPos )
{
    for( auto& it : compiler.CfgNormalizationDisallowed )
        if( it.Length() > spaceInsertionPos && normalized.ContainsAt( it, positionToCheck ) )
        {
            normalized.InsertAt( A_CHAR(" "), positionToCheck + spaceInsertionPos );
            return;
        }
}
} // anonymous namespace
//! @endcond

// #################################################################################################
// Optimize implementations
// #################################################################################################
AST* ASTLiteral   ::Optimize( Normalization               )   { return this; }
AST* ASTIdentifier::Optimize( Normalization               )   { return this; }
AST* ASTFunction  ::Optimize( Normalization normalization )
{
    for( auto& arg : Arguments )
        arg= arg->Optimize( normalization );
    return this;
}

AST* ASTBinaryOp::Optimize( Normalization normalization )
{
    Lhs= Lhs->Optimize( normalization );
    Rhs= Rhs->Optimize( normalization );
    return this;
}

AST* ASTConditional::Optimize( Normalization normalization )
{
    Q= Q->Optimize( normalization );
    T= T->Optimize( normalization );
    F= F->Optimize( normalization );
    return this;
}



AST* ASTUnaryOp::Optimize( Normalization normalization )
{
    Argument= Argument->Optimize( normalization );

    if( HasBits( normalization, Normalization::RemoveRedundantUnaryOpsOnNumberLiterals ) )
    {
        if( Argument->NodeType == Types::Literal && (Operator == A_CHAR("+") || Operator == A_CHAR("-")) )
        {
            ASTLiteral* astLiteral= dynamic_cast<ASTLiteral*>( Argument );

            if( astLiteral->Value.IsType<integer>() )
            {
                if( Operator.CharAtStart<false>() == '-'  )
                    astLiteral->Value= - astLiteral->Value.Unbox<integer>();
                Argument= nullptr;
                return astLiteral;
            }

            if( astLiteral->Value.IsType<double>() )
            {
                if( Operator.CharAtStart<false>() == '-'  )
                    astLiteral->Value= - astLiteral->Value.Unbox<double>();
                Argument= nullptr;
                return astLiteral;
            }
        }
    }

    return this;
}



// #################################################################################################
// Assemble implementations
// #################################################################################################

void ASTLiteral::Assemble( Program& program, MonoAllocator&, AString & normalized )
{
    integer idxInNormalized= normalized.Length();

    auto* func= Value.GetFunction<FToLiteral>( Reach::Local );
    if( func )
    {
        Value.CallDirect <FToLiteral>( func, normalized );
    }
    else if( Value.IsType<String>() )
    {
        normalized << '"';
        integer startExternalization= normalized.Length();
        normalized << Value;
        normalized << Format::Escape( Switch::On, startExternalization );
        normalized << '"';
    }
    else if( Value.IsType<double>() )
    {
        NumberFormat* nf= &program.compiler.CfgFormatter->DefaultNumberFormat;
        bool oldState= nf->ForceScientific;
        nf->ForceScientific=     nf->ForceScientific
                             ||(   Format == NFHint::Scientific
                                && HasBits(program.compiler.CfgNormalization, Normalization::KeepScientificFormat ) );

        normalized <<   aworx::Format( Value.Unbox<double>(), &program.compiler.CfgFormatter->DefaultNumberFormat);
        nf->ForceScientific=     oldState;
    }
    else if( Value.IsType<integer>() )
    {
        NFHint format= Format;
             if( HasBits(program.compiler.CfgNormalization, Normalization::ForceHexadecimal ) )
            format= NFHint::Hexadecimal;
        else if( HasBits(program.compiler.CfgNormalization, Normalization::ForceOctal       ) )
            format= NFHint::Octal;
        else if( HasBits(program.compiler.CfgNormalization, Normalization::ForceBinary      ) )
            format= NFHint::Binary;

        NumberFormat* nf   = &program.compiler.CfgFormatter->DefaultNumberFormat;
        integer     value= Value.Unbox<integer>();
             if( format == NFHint::Hexadecimal )  normalized << nf->HexLiteralPrefix << aworx::Format::Hex(static_cast<uint64_t>(value), 0, nf );
        else if( format == NFHint::Octal       )  normalized << nf->OctLiteralPrefix << aworx::Format::Oct(static_cast<uint64_t>(value), 0, nf );
        else if( format == NFHint::Binary )       normalized << nf->BinLiteralPrefix << aworx::Format::Bin(static_cast<uint64_t>(value), 0, nf );
        else                                      normalized                         << aworx::Format(value, 0, nf);
    }
    else
        normalized << Value;

    program.AssembleConstant( Value, Position, idxInNormalized );
}

void ASTIdentifier::Assemble( Program& program, MonoAllocator&, AString & normalized )
{
    auto format= program.compiler.CfgNormalization;

    String64 identifier;
    identifier.DbgDisableBufferReplacementWarning();
    identifier << Name;

    // -1 indicates that not even parentheses were given
    program.AssembleFunction( identifier, -1, Position, normalized.Length() );

    normalized << (HasBits( format, Normalization::ReplaceFunctionNames ) ? identifier : Name);
}


void ASTFunction::Assemble( Program& program, MonoAllocator& allocator, AString& normalized )
{
    auto format= program.compiler.CfgNormalization;
    String64 functionName;
    functionName.DbgDisableBufferReplacementWarning();

    bool completeIdentifierNames= HasBits( format, Normalization::ReplaceFunctionNames );
    functionName << Name;

    integer namePos= normalized.Length();
    normalized  << functionName;
    integer nameLen= normalized.Length() - namePos;
    normalized  << SPACE(FunctionSpaceBeforeOpeningBracket);

    auto qtyArgs= Arguments.Size();

    // the function used for nested expressions?
    bool replacedNestedExpressionIdentifierByLiteral= false;
    bool thirdArgumentIsThrowIdentifier= false;
    if( program.compiler.CfgNestedExpressionFunction.Match( Name ) )
    {
        if( qtyArgs < 1 ||  qtyArgs > 3 )
        {
            throw Exception( ALIB_CALLER_NULLED, Exceptions::NestedExpressionCallArgumentMismatch,
                             program.compiler.CfgNestedExpressionFunction );
        }

        // if an identifier is given for the first argument, we optionally convert the identifier
        // to a string value.
        if(     HasBits( program.compiler.CfgCompilation, Compilation::AllowIdentifiersForNestedExpressions)
            &&  (*Arguments.begin())->NodeType == Types::Identifier                                    )
        {
            ASTIdentifier* astIdentifier= dynamic_cast<ASTIdentifier*>( *Arguments.begin() );
            ASTLiteral*    astLiteral   = allocator.Emplace<ASTLiteral>( astIdentifier->Name, astIdentifier->Position );
            *Arguments.begin()= astLiteral;
            replacedNestedExpressionIdentifierByLiteral= true;
        }

        // if third parameter given it must be an identifier and equal to "throw"
        if( qtyArgs == 3 )
        {
            auto argIt= Arguments.begin();
            ++argIt;
            ++argIt;
            if(    (*argIt)->NodeType != Types::Identifier
                || !dynamic_cast<ASTIdentifier*>( (*argIt) )->Name.Equals<Case::Ignore>(
                                             program.compiler.CfgNestedExpressionThrowIdentifier) )
            {
                throw Exception( ALIB_CALLER_NULLED, Exceptions::NestedExpressionCallArgumentMismatch,
                                 program.compiler.CfgNestedExpressionFunction );
            }
            thirdArgumentIsThrowIdentifier= true;
        }
    }


    if( qtyArgs > 0 )
    {
        normalized  << '(' << SPACE(FunctionInnerBracketSpace);
        int no= -1;
        for( auto* argAst : Arguments )
        {
            ++no;
            if( no==0 )
            {
                // optionally remove quotes if we previously converted an identifier to string type
                if(    replacedNestedExpressionIdentifierByLiteral
                    && !HasBits(program.compiler.CfgNormalization, Normalization::QuoteUnaryNestedExpressionOperatorArgument) )
                {
                    integer lenBeforeArgument= normalized.Length();
                    argAst->Assemble( program, allocator, normalized );
                    normalized.ShortenTo( lenBeforeArgument );
                    normalized << dynamic_cast<ASTLiteral*>(argAst)->Value.Unbox<String>();
                    continue;
                }
            }
            else
                normalized << SPACE(FunctionSpaceBeforeComma) << ',' << SPACE(FunctionSpaceAfterComma);

            if( no != 2 || !thirdArgumentIsThrowIdentifier )
                argAst->Assemble( program, allocator, normalized );
            else
                normalized << program.compiler.CfgNestedExpressionThrowIdentifier;
        }
        normalized  << SPACE(FunctionInnerBracketSpace) << ')';
    }
    else
        normalized  << (HasBits(format, Normalization::FunctionInnerBracketSpaceIfNoArguments )
                        ? A_CHAR("()") : A_CHAR("( )") );


    program.AssembleFunction( functionName, static_cast<int>(qtyArgs), Position, namePos );
    if( completeIdentifierNames )
        normalized.ReplaceSubstring<false>( functionName, namePos, nameLen );
}


void ASTUnaryOp::Assemble( Program& program, MonoAllocator& allocator, AString & normalized )
{
    auto format= program.compiler.CfgNormalization;
    String op= Operator;

    bool isVerbalOp= isalpha(op.CharAtStart());


    // if a) this is the unary operator used for nested expressions
    // and b) an identifier terminal follows, we optionally convert the identifier to a string
    // value.
    bool replacedNestedExpressionIdentifierByLiteral= false;
    if( HasBits( program.compiler.CfgCompilation, Compilation::AllowIdentifiersForNestedExpressions) )
    {
        String nonVerbalOp;
        if( !isVerbalOp )
            nonVerbalOp= op;
        else
        {
            auto it=   program.compiler.AlphabeticUnaryOperatorAliases.Find(op);
            if(  it != program.compiler.AlphabeticUnaryOperatorAliases.end() )
                nonVerbalOp= it.Mapped();
            else
                nonVerbalOp= EmptyString();
        }

        if( program.compiler.CfgNestedExpressionOperator == nonVerbalOp
            &&  Argument->NodeType == Types::Identifier )
        {
            ASTIdentifier* astIdentifier= dynamic_cast<ASTIdentifier*>( Argument );
            ASTLiteral*    astLiteral   = allocator.Emplace<ASTLiteral>( astIdentifier->Name, astIdentifier->Position );
            Argument= astLiteral;
            replacedNestedExpressionIdentifierByLiteral= true;
        }
    }

    //--------- normal unary operators -------
    auto opIdx= normalized.Length();
    normalized << op;
    auto opLen= normalized.Length() - opIdx;

    // args in brackets if its a binary or ternary expression (-> if it has lower precedence)
    bool brackets=       HasBits(format, Normalization::RedundantUnaryOpBrackets)
                    || ( Argument->NodeType == Types::UnaryOp
                         && HasBits(format, Normalization::RedundantBracketsBetweenTwoUnaryOps)  )
                    ||   Argument->NodeType == Types::BinaryOp
                    ||   Argument->NodeType == Types::TernaryOp;

    bool opSpaceIfNotVerbal=    !brackets
                             && ( Argument->NodeType == Types::UnaryOp   ? HasBits(format, Normalization::UnaryOpSpaceIfUnaryFollows)
                                                                         : HasBits(format, Normalization::UnaryOpSpace              ) );
    normalized << ( brackets
                    ? normBracketOpen[   HasBits(format, Normalization::UnaryOpInnerBracketSpace    )
                                       + HasBits(format, Normalization::UnaryOpSpaceIfBracketFollows) * 2]
                    : String( (opSpaceIfNotVerbal || isVerbalOp ) ? A_CHAR(" ") : A_CHAR("") )
                  );

    // recursion
    integer lenBeforeArgument= normalized.Length();
    Argument->Assemble( program, allocator, normalized );

    // optionally remove quotes if we previously converted an identifier to string type
    if(    replacedNestedExpressionIdentifierByLiteral
        && !HasBits(program.compiler.CfgNormalization, Normalization::QuoteUnaryNestedExpressionOperatorArgument) )
    {
        normalized.ShortenTo( lenBeforeArgument );
        normalized << dynamic_cast<ASTLiteral*>( Argument )->Value.Unbox<String>();
    }

    if( brackets ) normalized << normBracketClose[HasBits(format, Normalization::UnaryOpInnerBracketSpace )];

    // check plugins
    program.AssembleUnaryOp( op, Position, opIdx );

    // did the compiler plug-in replace the operator (was given an alias operator)?
    if( op != Operator || isVerbalOp )
    {
        if( isVerbalOp )
        {
            // replace in any case: class program would have changed the op only if the corresponding
            // flags had been set:
            normalized.ReplaceSubstring<false>( op, opIdx, opLen );
            opLen=  op.Length();

            // we have to check only two of four format flags here, the rest was is (was) done in program
            if( ( program.compiler.CfgNormalization & (    Normalization::ReplaceVerbalOperatorsToLowerCase
                                                        +  Normalization::ReplaceVerbalOperatorsToUpperCase  ) )
                != Normalization::NONE)
                for( integer i= opIdx ; i < opIdx + opLen ; ++i )
                    normalized[i] = HasBits(program.compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToLowerCase)
                                    ? characters::CharArray<character>::ToLower( normalized[i] )
                                    : characters::CharArray<character>::ToUpper( normalized[i] );

            // remove space that was inserted for non-verbal op, if op is now symbolic
            if( !opSpaceIfNotVerbal && !isalpha(op.CharAtStart() ) )
                normalized.Delete( opIdx + opLen, 1 );

        }

        else if( HasBits(format, Normalization::ReplaceAliasOperators ) )
        {
            normalized.ReplaceSubstring<false>( op, opIdx, opLen );
            opLen=  op.Length();
        }
    }


    // check if a forbidden string occurred due to writing operator with no spaces/brackets and
    // the subsequent writing of probably nested unary operators, negative literals, etc.
    checkForbiddenStrings( program.compiler, normalized, opIdx, opLen );
}


void ASTBinaryOp::Assemble( Program& program, MonoAllocator& allocator, AString & normalized )
{
    auto format= program.compiler.CfgNormalization;
    String op=    Operator;

    //----------- Special treatment for subscript operator (needs different normalization) ---------
    if( op == A_CHAR("[]") )
    {
        // LHS recursion
        Lhs->Assemble( program, allocator, normalized );

        COND_SPACE(SubscriptSpaceBeforeBrackets, false);
        normalized << '[';


        // RHS recursion
        COND_SPACE(SubscriptInnerBracketSpace, false );
        auto opIdx= normalized.Length();
        Rhs->Assemble( program, allocator, normalized );
        COND_SPACE(SubscriptInnerBracketSpace, false );
        normalized << ']';

        // check plugins
        program.AssembleBinaryOp( op, Position, opIdx );

        return;
    }

    // Add brackets for LHS, if one of the two is true for it:
    // - it is a ternary op (always has lower precedence)
    // - it is a binary op with lower precedence
    // In the case that operators precedences are equal, we do not need brackets, as the
    // left-hand side is parsed first.
    int precedence= program.compiler.GetBinaryOperatorPrecedence( Operator );
    int lhsBinaryPrecedence= Lhs->NodeType == Types::BinaryOp
                             ? program.compiler.GetBinaryOperatorPrecedence(dynamic_cast<ASTBinaryOp*>( Lhs )->Operator)
                             : 0;
    int rhsBinaryPrecedence= Rhs->NodeType == Types::BinaryOp
                             ? program.compiler.GetBinaryOperatorPrecedence(dynamic_cast<ASTBinaryOp*>( Rhs )->Operator)
                             : 0;


    bool lhsBrackets=         Lhs->NodeType == Types::TernaryOp
                       || (   lhsBinaryPrecedence
                           && (    HasBits(format, Normalization::RedundantBinaryOpBrackets )
                                || precedence > lhsBinaryPrecedence
                                ||  (    HasBits(format, Normalization::RedundantBracketsIfLhsAndRhsAreBinaryOps )
                                      && rhsBinaryPrecedence )
                              )
                          );

    int bracketStringIdx=  (HasBits(format, Normalization::InnerBracketSpace   ) +
                            HasBits(format, Normalization::OuterBracketSpace   ) *2 );

    // LHS recursion
    if( lhsBrackets ) normalized << normBracketOpen[bracketStringIdx];
    Lhs->Assemble( program, allocator, normalized );
    if( lhsBrackets ) normalized << normBracketClose[bracketStringIdx];

    bool isVerbalOp= isalpha(op.CharAtStart());

    COND_SPACE( BinaryOpSpaces, isVerbalOp );
    auto opIdx= normalized.Length();
    normalized << op;
    auto opLen= normalized.Length() - opIdx;
    COND_SPACE( BinaryOpSpaces, isVerbalOp );

    // Add brackets for RHS, if one of the two is true for it:
    // - it is a ternary op (always has lower precedence)
    // - it is a binary op with lower or equal precedence
    // In fact, there are more situations where brackets can be removed, for example in:
    //         1 + (2 + 3)
    // but this is kept. The reason, why we don't remove, if operators are equal is:
    //         1 - (2 - 3)
    // Here, we must not remove the brackets. This generally means, we do not have enough
    // information about the algebraic rules of our operators to remove the brackets in all
    // cases!

    bool rhsBrackets=     Rhs->NodeType == Types::TernaryOp
                       || (    rhsBinaryPrecedence
                            && (    HasBits(format, Normalization::RedundantBinaryOpBrackets )
                                 || ( precedence >= rhsBinaryPrecedence )
                                 || (    HasBits(format, Normalization::RedundantRhsBracketsIfRhsIsStrongerBinaryOp )
                                      && precedence < rhsBinaryPrecedence )
                                 || (    HasBits(format, Normalization::RedundantBracketsIfLhsAndRhsAreBinaryOps )
                                      && lhsBinaryPrecedence )
                               )
                          );


    // RHS recursion
    if( rhsBrackets )  normalized << normBracketOpen[bracketStringIdx];
    Rhs->Assemble( program, allocator, normalized );
    if( rhsBrackets )  normalized << normBracketClose[bracketStringIdx];

    // check plugins
    program.AssembleBinaryOp( op, Position, opIdx );

    // did the compiler plug-in replace the operator (was given an alias operator)?
    if( op != Operator || isVerbalOp )
    {
        if( isVerbalOp )
        {
            // replace in any case: class program would have changed the op only if the corresponding
            // flags had been set:
            normalized.ReplaceSubstring<false>( op, opIdx, opLen );
            opLen=  op.Length();

            // we have to check only two of four format flags here, the rest was is (was) done in program
            if( ( program.compiler.CfgNormalization & (    Normalization::ReplaceVerbalOperatorsToLowerCase
                                                        +  Normalization::ReplaceVerbalOperatorsToUpperCase  ) )
                != Normalization::NONE)
                for( integer i= opIdx ; i < opIdx + opLen ; ++i )
                    normalized[i] = HasBits(program.compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToLowerCase)
                                    ? characters::CharArray<character>::ToLower( normalized[i] )
                                    : characters::CharArray<character>::ToUpper( normalized[i] );

            // remove operator spaces if they were inserted for non-verbal op, if op is now symbolic
            if( !HasBits(format, Normalization::BinaryOpSpaces) && !isalpha(op.CharAtStart() ) )
            {
                normalized.Delete( opIdx + opLen, 1 );
                normalized.Delete( opIdx - 1    , 1 );
            }

        }

        else if( HasBits(format, Normalization::ReplaceAliasOperators ) )
        {
            normalized.ReplaceSubstring<false>( op, opIdx, opLen );
            opLen=  op.Length();
        }
    }

    // check if a forbidden string occurred due to writing operator with no spaces/brackets and
    // the subsequent writing of probably nested unary operators, negative literals, etc.
    checkForbiddenStrings( program.compiler, normalized, opIdx, opLen );
}


void ASTConditional::Assemble( Program& program, MonoAllocator& allocator, AString & normalized )
{
    auto format= program.compiler.CfgNormalization;

    int bracketStringIdx=  (HasBits(format,  Normalization::InnerBracketSpace   ) +
                            HasBits(format,  Normalization::OuterBracketSpace   ) *2 );


    // Q
    Q->Assemble( program, allocator, normalized );
    normalized << SPACE(ConditionalOpSpaceBeforeQM);
    program.AssembleCondFinalize_Q( Position, normalized.Length() );
    normalized << A_CHAR("?") << SPACE(ConditionalOpSpaceAfterQM);

    // T
    integer idxInNormalized= normalized.Length();

    bool brackets=    T->NodeType == Types::TernaryOp
                   && HasBits( format, Normalization::RedundantConditionalOpBrackets);

    if( brackets ) normalized << normBracketOpen[bracketStringIdx];
        T->Assemble( program, allocator, normalized );
    if( brackets ) normalized << normBracketClose[bracketStringIdx];

    // :
    normalized << SPACE(ConditionalOpSpaceBeforeColon);

    program.AssembleCondFinalize_T( ColonPosition, normalized.Length() );

    normalized << ":" << SPACE(ConditionalOpSpaceAfterColon);


    // F
    brackets=         F->NodeType == Types::TernaryOp
                   && HasBits( format, Normalization::RedundantConditionalOpBrackets);

    if( brackets ) normalized << normBracketOpen[bracketStringIdx];
        F->Assemble( program, allocator, normalized );
    if( brackets ) normalized << normBracketClose[bracketStringIdx];

    program.AssembleCondFinalize_F( Position, idxInNormalized );
}





}}}} // namespace [aworx::lib::expressions::detail]





