// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_VIRTUAL_MACHINE)
#   include "alib/expressions/detail/virtualmachine.hpp"
#endif

#if !defined (HPP_ALIB_STRINGFORMAT_TEXT)
#   include "alib/stringformat/text.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN)
#   include "alib/expressions/compilerplugin.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM)
#   include "alib/expressions/detail/program.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_AST)
#   include "alib/expressions/detail/ast.hpp"
#endif

namespace aworx { namespace lib { namespace expressions { namespace detail {


VirtualMachine::Command::Command( Program* program, const String& functionOrOp,
                                  integer idxOriginal, integer idxNormalized          )
: opcode          ( OpCodes::Subroutine )
, Operation       ( program )
, ResultType      ( program ? program->ResultType() : Box( nullptr ) )
, ExpressionPositions( (static_cast<uinteger>(idxNormalized) << (sizeof(integer)/2*8) )
                      + static_cast<uinteger>(idxOriginal  )  )
, DecompileType   ( DecompileInfoType::Subroutine )
, DecompileSymbol ( functionOrOp )
{}


#define POS_IN_EXPR_STR      (cmd.ExpressionPositions & ( (1UL << (sizeof(integer) / 2 * 8 - 1) ) -1 )  )
#if ALIB_DEBUG
#   define NORMPOS_IN_EXPR_STR  (cmd.ExpressionPositions >> (sizeof(integer)/2*8 )    )
#endif

// #################################################################################################
// Run()
// #################################################################################################
Box  VirtualMachine::Run( Program& program, Scope& scope )
{
    ALIB_ASSERT_ERROR( program.compileStorage == nullptr,
         "Internal error in program: AssembleFinalize not called?"  )

    // If the stack is empty, this indicates, that this is an 'external' call and not a subroutine.
    // In this case, we clean the scope and such
    std::vector<aworx::Box>&    stack=     scope.Stack;
    stack.clear();
    scope.Clear();
    scope.nestedExpressionStack.clear();

    // attach the compile-time scope to the evaluation scope.
    scope.CTScope= program.ctScope;

    // run
    run( program, scope, stack, scope.nestedExpressionStack );


    // unset ctscope
    scope.CTScope=  nullptr;

    // Remove top element from the stack and return its result.
    Box result= stack.back();
    stack.pop_back();
    return result;

}


void  VirtualMachine::run( Program& program, Scope& scope, std::vector<Box>& stack,
                           std::vector<Expression*>& nestedCalls )
{

    ALIB_DBG( using DCT= Command::DecompileInfoType; )
    ALIB_DBG( auto initialStackSize= stack.size(); )

    // check circular calls
    for( auto it: nestedCalls )
        if( it == &program.expression )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::CircularNestedExpressions );
            for( auto it2= nestedCalls.begin() ; it2!=  nestedCalls.end(); ++it2 )
                e.Add  ( ALIB_CALLER_NULLED, Exceptions::CircularNestedExpressionsInfo,
                          (*it2)->Name(),
                          it2 + 1 != nestedCalls.end()
                              ? (*(it2+1))->Name()
                              : it->Name()                   );
            throw e;
        }

    nestedCalls.emplace_back( &program.expression );



    for( integer programCounter= 0; programCounter < program.Length() ; ++ programCounter )
    {
        const Command& cmd= program.AtConst(programCounter);
        ALIB_WARNINGS_ALLOW_BITWISE_SWITCH
        switch( cmd.Which() )
        {
            case Command::OpCodes::Constant:
                stack.emplace_back( cmd.Operation.Value );

            break;

            case Command::OpCodes::Function:
            {
                try
                {
                    // with no args, we need a new stack object
                    if(  cmd.QtyFunctionArgs <= 0 )
                    {
                        stack.emplace_back( cmd.Operation.Callback( scope,
                                                                    stack.end() ,
                                                                    stack.end()  ) );

                        ALIB_ASSERT_ERROR( cmd.ResultType.IsSameType(stack.back()),
                               "Result type mismatch during command execution:\\n"
                               "       In expression: {!Q} {{{}}}\\n"
                               "              Plugin: {}\\n"
                               "          Identifier: {} ({})\\n"
                               "       Expected type: <{}> (aka {})\\n"
                               "         Result type: <{}> (aka {})\\n"
                               "        Result value: {}\\n",
                               program.expression.Name(),
                               program.expression.GetNormalizedString(),
                               cmd.DbgInfo.Plugin->Name,
                               cmd.DecompileSymbol, cmd.DbgInfo.Callback,
                               program.compiler.TypeName( cmd.ResultType ), cmd.ResultType.TypeID(),
                               program.compiler.TypeName( stack.back()   ), stack.back()  .TypeID(),
                               stack.back()                                 )
                    }

                    // otherwise, we assign the value the position of the first arg in the stack and
                    // delete the other args after the call
                    else
                    {
                        ALIB_DBG( Box arg1Saved= *(stack.end() - static_cast<ptrdiff_t >( cmd.QtyFunctionArgs ) ); )

                        *(stack.end() - static_cast<ptrdiff_t>( cmd.QtyFunctionArgs ) )=
                            cmd.Operation.Callback( scope,
                                                    stack.end()  - static_cast<ptrdiff_t>( cmd.QtyFunctionArgs ),
                                                    stack.end()  );

                        #if ALIB_DEBUG
                            if( !cmd.ResultType.IsSameType(*(stack.end() - static_cast<ptrdiff_t>( cmd.QtyFunctionArgs ) )) )
                            {
                                String128 description;
                                switch( cmd.DecompileType )
                                {
                                    case DCT::UnaryOp:
                                        description << "Unary operator '"  << cmd.DecompileSymbol  << '\'';
                                        break;
                                    case DCT::BinaryOp:
                                        description << "Binary operator '" << cmd.DecompileSymbol  << '\'';
                                        break;
                                    case DCT::Function:
                                        if( cmd.QtyFunctionArgs < 0 )
                                            description << "Identifier \"" << cmd.DecompileSymbol << '\"';
                                        else if( cmd.QtyFunctionArgs == 0 )
                                            description << "Function \"" << cmd.DecompileSymbol << "()\"";
                                        else
                                            description << "Function \"" << cmd.DecompileSymbol << "(#"
                                                                         << cmd.QtyFunctionArgs << ")\"";
                                        break;
                                    case DCT::AutoCast:
                                        description << "Auto-cast"       << cmd.DecompileSymbol  << '\'';
                                        break;
                                    case DCT::LiteralConstant:      ALIB_FALLTHROUGH
                                    case DCT::OptimizationConstant:
                                        ALIB_ERROR("Must not be set with function calls");
                                }

                                String512 msg;
                                auto fmt= GetDefaultFormatter();
                                fmt->Format( msg, "Result type mismatch during command execution:\\n"
                                                 "       In expression: {!Q} {{{}}}\\n"
                                                 "              Plugin: {}\\n"
                                                 "                Info: {}\\n"
                                                 "            Callback: {}\\n"
                                                 "       Expected type: <{}> (aka {})\\n"
                                                 "         Result type: <{}> (aka {})\\n"
                                                 "        Result value: {}\\n"
                                                 "    Parameter values: ",
                                                 program.expression.Name(),
                                                 program.expression.GetNormalizedString(),
                                                 cmd.DbgInfo.Plugin->Name,
                                                 description,
                                                 cmd.DbgInfo.Callback,
                                                 program.compiler.TypeName( cmd.ResultType ), cmd.ResultType.TypeID(),
                                                 program.compiler.TypeName( stack.back()   ), stack.back()  .TypeID(),
                                                 stack.back()                                 );
                                fmt->Format( msg, "('{}' <{}>",
                                                 arg1Saved,
                                                 program.compiler.TypeName( arg1Saved )        );
                                for( integer i= cmd.QtyFunctionArgs - 1; i > 0 ; --i )
                                    fmt->Format( msg, ", '{}' <{}>",
                                                     *(stack.end()  - i),
                                                     program.compiler.TypeName( *(stack.end()-i))   );
                                msg << ')';
                                ALIB_ERROR( msg )
                            }
                        #endif

                        for(integer i= 1; i < cmd.QtyFunctionArgs; ++i )
                            stack.pop_back();
                    }
                }
                catch( Exception& e )
                {
                    if( !EnumContains(program.compiler.CfgCompilation, Compilation::CallbackExceptionFallThrough) )
                    {
                        e.Add( ALIB_CALLER_NULLED, Exceptions::ExceptionInCallback,
                               program.expression.Name()         );
                        e.Add( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                               program.expression.GetOriginalString(), POS_IN_EXPR_STR );
                    }
                    throw;
                }
                catch( std::exception& stdException )
                {
                    if( !EnumContains(program.compiler.CfgCompilation, Compilation::CallbackExceptionFallThrough) )
                    {
                        Exception e( ALIB_CALLER_NULLED, Exceptions::ExceptionInCallback,
                                     program.expression.Name()         );
                        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                                     program.expression.GetOriginalString(), POS_IN_EXPR_STR );
                        e.Add      ( ALIB_CALLER_NULLED, Exceptions::StdExceptionInfo,
                                     stdException.what()      );
                        throw e;
                    }
                    throw;
                }
            }
            break;

            case Command::OpCodes::JumpIfFalse:
            {
                if( !stack.back().Call<FIsTrue>() )
                    programCounter+= cmd.Operation.Distance -1; //-1 due to loop increase
                stack.pop_back();
            }
            break;

            case Command::OpCodes::Jump:
                programCounter+= cmd.Operation.Distance - 1;    //-1 due to loop increase
                break;


            case Command::OpCodes::Subroutine:
                // evaluation time defined nested expression
                if(    cmd.Operation.NestedProgram == nullptr
                    || cmd.Operation.NestedProgram == &program)
                {
                    SPExpression nested;
                    String nestedExpressionName= (stack.end()-2)->Unbox<String>();
                    try
                    {
                        nested= program.compiler.GetNamed( nestedExpressionName );
                    }
                    catch( Exception& e )
                    {
                        // 3rd parameter "throw" was not given
                        if( cmd.Operation.NestedProgram == nullptr )
                        {
                            // not found? -> remove name parameter from stack and use result of
                            // second parameter.
                            if( e.Type().Value() == EnumValue( Exceptions::NamedExpressionNotFound ) )
                            {
                                stack.erase( stack.end() - 2 );
                                break;
                            }

                            e.Add( ALIB_CALLER_NULLED, Exceptions::WhenEvaluatingNestedExpression,
                                   nestedExpressionName         );
                            e.Add( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                                   program.expression.GetOriginalString(), POS_IN_EXPR_STR );

                            throw;
                        }

                        // 3rd parameter "throw" was given
                        if( e.Type().Value() == EnumValue( Exceptions::NamedExpressionNotFound ) )
                            e.Add( ALIB_CALLER_NULLED, Exceptions::NestedExpressionNotFoundET,
                                   nestedExpressionName         );
                        else
                            e.Add( ALIB_CALLER_NULLED, Exceptions::WhenEvaluatingNestedExpression,
                                   nestedExpressionName         );
                        throw;
                    }

                    run( * dynamic_cast<Program*>(nested.get()->GetProgram()), scope, stack, nestedCalls );

                    if( !(stack.end()-2)->IsSameType(stack.back()) )
                    {
                        Exception e( ALIB_CALLER_NULLED, Exceptions::NestedExpressionResultTypeError,
                                     nestedExpressionName,
                                     program.compiler.TypeName( *(stack.end()-2) ),
                                     program.compiler.TypeName( stack.back() )          );
                        e.Add        ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                                       program.expression.GetOriginalString(), POS_IN_EXPR_STR );
                        throw e;
                    }

                    stack.erase( stack.end()-3, stack.end() -1 );
                }

                // compile-time defined nested expression: just call it
                else
                    run( *cmd.Operation.NestedProgram, scope, stack, nestedCalls );

                break;
        }
        ALIB_WARNINGS_RESTORE

    } // command loop

    nestedCalls.pop_back();

    // This assertion should never happen. It indicates rather a library error than an
    // erroneous plug-in.
    ALIB_ASSERT_ERROR( stack.size() == initialStackSize + 1,
                       "Internal error: Stack increased by {} (instead of 1) after run of expression program.",
                        stack.size() - initialStackSize      )

    // Usually a function did not return what it is defined to return.
    ALIB_ASSERT_ERROR( program.ResultType().IsSameType(stack.back()),
                       "Wrong result type of program execution:\\n"
                       "   Expected Type: <{}> (aka {})\\n"
                       "     Result Type: <{}> (aka {})\\n"
                       "    Result value: {}\\n"
                       "   In expression: {!Q}"
                       ,
                       program.compiler.TypeName( program.ResultType() ), program.ResultType().TypeID(),
                       program.compiler.TypeName( stack.back()         ), stack.back()        .TypeID(),
                       stack.back(),
                       program.expression.Name()
                     )
}

// #################################################################################################
// Decompile()
// #################################################################################################
AST* VirtualMachine::Decompile( Program& program )
{
    using DCT= Command::DecompileInfoType;

    #define PushNode(node)        nodeStack.emplace_back( node )
    #define PopNode               nodeStack.pop_back()
    #define Lhs                   (*(nodeStack.end() - 2) )
    #define Rhs                   nodeStack.back()
    #define Arg                   nodeStack.back()

    std::vector<AST*>   nodeStack;

    std::stack <PC>     conditionalStack;  // Stores the target of jump commands behind the T of
                                           // conditional term "Q : T : F". In other words, the
                                           // end of 'F'

    for( integer pc= 0 ; pc < program.Length() ; ++pc )
    {
        const Command& cmd= program.AtConst(pc);
        integer positionInExpression= POS_IN_EXPR_STR;

        ALIB_WARNINGS_ALLOW_BITWISE_SWITCH
        switch( cmd.Which() )
        {
            case Command::OpCodes::Subroutine:
            {
                // Function "Expression(name, type, boolean)"
                if(    cmd.Operation.NestedProgram == nullptr
                    || cmd.Operation.NestedProgram == & program )
                {
                    ASTFunction* node= new ASTFunction( cmd.DecompileSymbol, positionInExpression );
                    for( integer i= 0 ; i< 2 ; ++i )
                    {
                        node->Arguments.insert( node->Arguments.begin(), Arg );
                        PopNode;
                    }

                    // if nullptr, third parameter "throw"
                    if( cmd.Operation.NestedProgram != nullptr )
                        node->Arguments.emplace_back( new ASTIdentifier( program.compiler.CfgNestedExpressionThrowIdentifier, positionInExpression ) );

                    PushNode(node);
                    break;
                }

                // constant call (program was given)
                ASTIdentifier* name  = new ASTIdentifier( cmd.Operation.NestedProgram->expression.Name(), positionInExpression );
                ASTUnaryOp*    nested= new ASTUnaryOp( program.compiler.CfgNestedExpressionOperator, name, positionInExpression );
                PushNode( nested );
            }
            break;

            case Command::OpCodes::Constant:
            {
                ASTLiteral* node= new ASTLiteral( static_cast<integer>(0), positionInExpression );
                node->Value= cmd.Operation.Value;
                PushNode(node);
            }
            break;

            case Command::OpCodes::Function:
            {
                if( cmd.DecompileType == DCT::UnaryOp )
                {
                    ASTUnaryOp* node= new ASTUnaryOp( cmd.DecompileSymbol, Arg, positionInExpression );
                    PopNode;
                    PushNode(node);
                    break;
                }

                if( cmd.DecompileType == DCT::BinaryOp )
                {
                    ASTBinaryOp* node= new ASTBinaryOp( cmd.DecompileSymbol, Lhs, Rhs, positionInExpression );
                    PopNode;
                    PopNode;
                    PushNode(node);
                    break;
                }

                if( cmd.QtyFunctionArgs < 0 )
                {
                    ASTIdentifier* node= new ASTIdentifier( cmd.DecompileSymbol, positionInExpression );
                    PushNode(node);
                    break;
                }

                ASTFunction* node= new ASTFunction( cmd.DecompileSymbol, positionInExpression );
                for( integer i= 0 ; i< cmd.QtyFunctionArgs ; ++i )
                {
                    node->Arguments.insert( node->Arguments.begin(), Arg );
                    PopNode;
                }

                PushNode(node);
            }
            break;

            case Command::OpCodes::JumpIfFalse: // '?'
            {
            }
            break;

            case Command::OpCodes::Jump:        // ':'
            {
                conditionalStack.emplace( pc + cmd.Operation.Distance -1 );
            }
            break;

        }
        ALIB_WARNINGS_RESTORE

        while( !conditionalStack.empty() && conditionalStack.top() == pc )
        {
            AST* F= Arg; PopNode;
            AST* T= Arg; PopNode;               // F-Pos -2 is a little vague. But we don't care!
            Arg= new ASTConditional( Arg, T, F, positionInExpression, F->Position - 2  );
            conditionalStack.pop();
        }


    } // command loop

    #undef PushNode
    #undef PopNode
    #undef Lhs
    #undef Rhs
    #undef Arg

    ALIB_ASSERT_ERROR( nodeStack.size() == 1,
        "VM AST generation error: Node stack must contain one element. Elements: {}", nodeStack.size())
    ALIB_ASSERT_ERROR( conditionalStack.size() == 0,
        "VM Program List error: Conditional stack after listing not 0 but {}", conditionalStack.size())

    return nodeStack.back();
}



// #################################################################################################
// DbgList()
// #################################################################################################
#if ALIB_DEBUG

//! @cond NO_DOX
namespace{
void writeArgPositions(AString& target, std::vector<VirtualMachine::PC>& resultStack, integer qtyArgs )
{
    for( integer argNo= qtyArgs ; argNo > 0 ; -- argNo )
    {
        target << (argNo == qtyArgs ? "" : ", ")
               << (qtyArgs-argNo) << "{"
               << ( static_cast<integer>(resultStack.size()) == argNo ? 0
                                                                      : *(resultStack.end() - argNo - 1) + 1 )
               << ".."  << *(resultStack.end() - argNo )
               << "}";
    }
}
}
//! @endcond


AString VirtualMachine::DbgList( Program& program )
{
    using DCT= Command::DecompileInfoType;

    String fmtLine=   EXPRESSIONS.GetResource( "ProgListLine"  );
    String fmtHeader= EXPRESSIONS.GetResource( "ProgListHeader");

    Text text;
    text.LineWidth=0;
    text.Formatter= program.ctScope->Formatter;
    text.Formatter->Acquire( ALIB_CALLER_PRUNED ); // get formatter once to keep autosizes!

    // repeat the whole output until its size is stable and all auto-tabs are set
    integer  lastLineWidth  = 0;
    while(lastLineWidth == 0 || lastLineWidth != text.DetectedMaxLineWidth)
    {
        lastLineWidth= text.DetectedMaxLineWidth;
        text.Buffer.Reset();

        // write headline
        text.LineWidth= text.DetectedMaxLineWidth;
            text.AddMarked( fmtHeader,
                          program.expression.Name(),
                          program.expression.GetNormalizedString()    );


        text.LineWidth= 0;
        NString hdlKey= "ProgListHdl";
        Boxes hdlArgs( fmtLine );
        NString64 hdlKeyNumbered(hdlKey);
        for( int i= 0 ; i < 7 ; ++i )
        {
            hdlArgs.Add( EXPRESSIONS.GetResource(hdlKeyNumbered << i ) );
            hdlKeyNumbered.ShortenTo( hdlKey.Length() );
        }

        hdlArgs.Add( 1 );
        hdlArgs.Add( program.expression.GetNormalizedString() );
        text.AddMarked( hdlArgs );
        text.LineWidth= text.DetectedMaxLineWidth;
            text.AddMarked("@HL-");
        text.LineWidth= 0;


        #define FMT(qtyArgs)                                                                       \
        { if( cmd.DbgInfo.Plugin ) description << ", CP=\"" << cmd.DbgInfo.Plugin->Name<< '\"';    \
            String256 argpos; writeArgPositions(argpos,resultStack,qtyArgs);                       \
          text.Add( fmtLine,                                                                         \
                  pc,                                                                              \
                  program.compiler.TypeName( cmd.ResultType ),                                     \
                  cmd.Which(),                                                                     \
                  operation,                                                                       \
                  stackSize,                                                                       \
                  description,                                                                     \
                  argpos,                                                                          \
                  NORMPOS_IN_EXPR_STR, "_^_"                                );                    }


        #define PushResult              resultStack.emplace_back( pc )
        #define PopResult               resultStack.pop_back()
        #define ResultPos               resultStack.back()

        std::vector<PC>     resultStack;       // The last command of the current results on the stack
        std::stack <PC>     conditionalStack;  // Stores the target of jump commands behind the T of
                                               // conditional term "Q : T : F". In other words, the
                                               // end of 'F'

        PC stackSize    = 0;
        for( integer pc= 0 ; pc < program.Length() ; ++pc )
        {
            const Command& cmd= program.AtConst(pc);

            String128 operation;
            String128 description;
            ALIB_WARNINGS_ALLOW_BITWISE_SWITCH
            switch( cmd.Which() )
            {
                case Command::OpCodes::Subroutine:
                {
                    if(    cmd.Operation.NestedProgram == nullptr
                        || cmd.Operation.NestedProgram == &program )
                    {
                        stackSize--;
                        operation   << ( cmd.Operation.NestedProgram == nullptr ? "Expr(name, type)"
                                                                                : "Expr(name, type, throw)" );
                        description << "Nested expr. searched at evaluation-time";
                        PopResult;
                    }
                    else
                    {
                        stackSize++;
                        operation   << cmd.DecompileSymbol
                                    << "\"" << cmd.Operation.NestedProgram->expression.Name() << '\"';
                        description << "Nested expr. searched at compile-time";
                        PushResult;
                    }
                    FMT(0)
                }
                break;

                case Command::OpCodes::Constant:
                {
                    stackSize++;
                    char uetzelchen= cmd.Operation.Value.IsType<String>() ? '\"' : '\'';
                    operation   << uetzelchen << cmd.Operation.Value << uetzelchen;
                    description << (cmd.DecompileType == DCT::LiteralConstant ? "Literal constant"
                                                                              : "Optimization constant" );
                    PushResult;
                    FMT(0)
                }
                break;

                case Command::OpCodes::Function:
                {
                    operation   << cmd.DbgInfo.Callback
                                << "(#" << (cmd.QtyFunctionArgs < 0 ? 0 : cmd.QtyFunctionArgs) << ')';

                    NString descr;
                    char   decSym= '\0';
                    switch( cmd.DecompileType )
                    {
                        case DCT::UnaryOp:
                            descr = "Unary operator";
                            decSym= '\'';
                            break;
                        case DCT::BinaryOp:
                            descr = "Binary operator";
                            decSym= '\'';
                            break;
                        case DCT::Function:
                            if( cmd.QtyFunctionArgs < 0 )
                            {
                                descr = "Identifier";
                                decSym= '"';
                                break;
                            }
                            description << "Function \"";
                            if( cmd.QtyFunctionArgs == 0 )
                                description << cmd.DecompileSymbol << "()\"";
                            else
                                description << cmd.DecompileSymbol << "(#"
                                            << cmd.QtyFunctionArgs << ")\"";
                            break;
                        case DCT::AutoCast:
                            descr = "Auto-cast";
                            decSym= '\'';
                            break;
                        case DCT::LiteralConstant:      ALIB_FALLTHROUGH
                        case DCT::OptimizationConstant:
                            ALIB_ERROR("Must not be set with function calls");
                    }
                    if( descr.IsNotNull() )
                        description << descr << ' ' << decSym << cmd.DecompileSymbol << decSym;

                    stackSize+= 1 - (cmd.QtyFunctionArgs > 0 ? cmd.QtyFunctionArgs : 0 );
                    FMT(cmd.QtyFunctionArgs < 0 ? 0 : cmd.QtyFunctionArgs)
                    for( integer i= 0 ; i< cmd.QtyFunctionArgs ; ++i )
                        PopResult;
                    PushResult;
                }
                break;

                case Command::OpCodes::JumpIfFalse:  // '?'
                {
                    operation   << ( pc + cmd.Operation.Distance ) << " (absolute)"  ;
                    description << "'?'";
                    FMT( 1 )     // first print  Q
                    ResultPos++; // then have jump be included in T
                }
                break;

                case Command::OpCodes::Jump:        // ':'
                {
                    conditionalStack.emplace( pc + cmd.Operation.Distance -1 );
                    operation   << ( pc + cmd.Operation.Distance ) << " (absolute)"  ;
                    description << "':'";
                    FMT( 1 )     // first print  T
                    ResultPos++; // then have jump be included in T
                }
                break;

            }
            ALIB_WARNINGS_RESTORE


            while( !conditionalStack.empty() && conditionalStack.top() == pc )
            {
                 PopResult;
                 PopResult;
                 ResultPos= pc;
                 conditionalStack.pop();
                 stackSize-= 2;
            }

        } // command loop


        #undef PushResult
        #undef PopResult
        #undef ResultPos

        ALIB_ASSERT_ERROR( lastLineWidth!= 0 || stackSize == 1,
            "VM Program List error: Stack size after listing not 1 but {}", stackSize )

        ALIB_ASSERT_ERROR( lastLineWidth!= 0 || resultStack.size() == 1,
            "VM Program List error: Resultstack after listing not 1 but {}", resultStack.size())

        ALIB_ASSERT_ERROR( lastLineWidth!= 0 || conditionalStack.size() == 0,
            "VM Program List error: Conditional stack after listing not 0 but {}", conditionalStack.size())
    }; // main loop

    text.Formatter->Release();

    return std::move(text.Buffer);
}
#endif


#undef POS_IN_EXPR_STR
#undef PushResult
#undef PopResult
#undef ResultPos
#undef FMT
#undef PushNode
#undef PopNode
#undef Lhs
#undef Rhs
#undef Arg

}}}} // namespace [aworx::lib::expressions::detail]


