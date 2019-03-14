// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM)
#   include "alib/expressions/detail/program.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN)
#   include "alib/expressions/compilerplugin.hpp"
#endif


namespace aworx { namespace lib { namespace expressions { namespace detail {

//! @cond NO_DOX
using Command= VirtualMachine::Command;
//! @endcond

// #################################################################################################
// Program
// #################################################################################################

#if ALIB_DEBUG
#define DBG_CALLBACK_INFO(cmd)                                                                  \
     cmd.DbgInfo.Callback= cInfo.DbgCallbackName;                                                  \
     cmd.DbgInfo.Plugin  = ppp.plugin;
#else
#define DBG_CALLBACK_INFO(cmd)
#endif

Program::~Program()
{
    if( ctScope )
        delete ctScope;
    if( compileStorage )
        delete compileStorage;
}

const Box& Program::ResultType() const
{
    return back()     .ResultType;
}


// #################################################################################################
// Assemble commands
// #################################################################################################
#define TypeStr(box)            compiler.TypeName(box)
#define AddCommand(...)         emplace_back(__VA_ARGS__)
#define RemoveLastCommand       pop_back()
#define LastCommandNo           Length() -1
#define LastCommand             back()
#define PushResult              compileStorage->resultStack.emplace_back( LastCommandNo )
#define PopResult               compileStorage->resultStack.pop_back()
#define ResultPos               compileStorage->resultStack.back()
#define LHSResultPos            (*(compileStorage->resultStack.end() - 2))
#define RHSResultPos            ResultPos
#define LHSCodeStart            (compileStorage->resultStack.size() == 2 ? 0  :  *(compileStorage->resultStack.end() - 3 ) + 1 )
//#define RHSCodeStart          (LHSResultPos + 1 )

#define ASSERT_ASSEMBLE                                                                            \
    if( compileStorage == nullptr )  return;                                                       \
    ALIB_ASSERT_ERROR(    compileStorage->resultStack.empty()                                      \
                       || ResultPos == LastCommandNo                                               \
                       || back().IsConditionalJump()                   ,                           \
                       "Internal error: Last in result stack is not last command." )               \

bool Program::collectArgs( integer qty )
{
    ALIB_ASSERT_ERROR( compileStorage->resultStack.size() >= static_cast<size_t>( qty < 0 ? 0 : qty ),
                       "Not enough arguments on the stack. This should never happen (internal error)." )

    ctScope->Stack.clear();

    bool allAreConst= true;
    for( integer i= qty; i > 0 ; --i )
    {
        Command& cmd=  At(*(compileStorage->resultStack.end() - i));
        bool     isConstant=  cmd.IsConstant();
        ctScope->Stack.emplace_back( isConstant ? cmd.Operation.Value
                                                : cmd.ResultType      );
        allAreConst&= isConstant;
    }

    return allAreConst && !EnumContains(compiler.CfgCompilation, Compilation::NoOptimization );
}


void Program::AssembleConstant( Box& value, integer idxInOriginal, integer idxInNormalized )
{
    ASSERT_ASSEMBLE

    AddCommand( VM::Command(  value.IsType<String>() ?  Box( ctScope->Memory.Clone(value.Unbox<String>()) )
                                                     :  value
                              , false, idxInOriginal, idxInNormalized  )  );
    PushResult;
}


void Program::AssembleFunction( AString& functionName , integer qtyArgsOrNoParentheses,
                                integer  idxInOriginal, integer idxInNormalized  )
{
    ASSERT_ASSEMBLE

    // Nested expressions
    if(    compiler.CfgNestedExpressionFunction.GetRawName().IsNotEmpty()
        && compiler.CfgNestedExpressionFunction.Match(functionName )       )
    {
        functionName.Reset( compiler.CfgNestedExpressionFunction );

        if(    qtyArgsOrNoParentheses < (EnumContains( compiler.CfgCompilation, Compilation::AllowCompileTimeNestedExpressions )
                                         ? 1 : 2 )
            || !At(*(compileStorage->resultStack.end() - ( qtyArgsOrNoParentheses == 3 ? 2 : qtyArgsOrNoParentheses) ) )
                .ResultType.IsType<String>() )
        {
            throw Exception( ALIB_CALLER_NULLED, Exceptions::NestedExpressionCallArgumentMismatch,
                             compiler.CfgNestedExpressionFunction );
        }


        // single argument? -> we have to get the expression now
        if( qtyArgsOrNoParentheses == 1 )
        {
            if( !At(compileStorage->resultStack.back()).IsConstant() ) // must not use bool allAreConstant here!
            {
                Exception e( ALIB_CALLER_NULLED, Exceptions::NamedExpressionNotConstant );
                e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                             expression.GetOriginalString(), idxInOriginal );
                throw e;
            }

            SPExpression nested;
            String nestedExpressionName= At(compileStorage->resultStack.back()).ResultType.Unbox<String>();
            try
            {
                nested= compiler.GetNamed( nestedExpressionName );
            }
            catch( Exception& e )
            {
                if( e.Type().Value() == EnumValue( Exceptions::NamedExpressionNotFound ) )
                {
                    e.Add( ALIB_CALLER_NULLED, Exceptions::NestedExpressionNotFoundCT ,
                           nestedExpressionName );
                }
                else
                {
                    ALIB_ERROR( "Unknown exception {!Q}.", e.Type() );
                }
                throw;
            }

            ctNestedExpressions.emplace_back(nested);

            LastCommand= VM::Command( dynamic_cast<Program*>( nested->GetProgram() ),
                                      compiler.CfgNestedExpressionFunction.GetRawName(),
                                      idxInOriginal, idxInNormalized                       );
            return;
        }

        // If two arguments, we send nullptr to indicate that 2nd argument is replacement
        if( qtyArgsOrNoParentheses == 2 )
        {
            AddCommand( static_cast<Program*>( nullptr ),
                        compiler.CfgNestedExpressionFunction.GetRawName(),
                        idxInOriginal, idxInNormalized                     );
            back().ResultType= (end()-2)->ResultType;
        }

        // 3rd argument given (throw): we send "this" which indicates to throw in expression is not found.
        else
            AddCommand( dynamic_cast<Program*>( this ),
                        compiler.CfgNestedExpressionFunction.GetRawName(),
                        idxInOriginal, idxInNormalized                     );

        PopResult;
        ResultPos= LastCommandNo;
        return;
    }


    // collect arguments
    bool allAreConstant= collectArgs(qtyArgsOrNoParentheses);

    compileStorage->functionsWithNonMatchingArguments.clear();
    CompilerPlugin::CIFunction cInfo( *ctScope,
                                      functionName,
                                      (qtyArgsOrNoParentheses < 0), //no parentheses?
                                      allAreConstant,
                                      compileStorage->functionsWithNonMatchingArguments );

    try
    {
        for( auto& ppp : compileStorage->plugins )
        {
            if( !ppp.plugin->TryCompilation( cInfo ) )
                continue;

            // constant?
            if( cInfo.Callback == nullptr )
            {
                if( qtyArgsOrNoParentheses > 0 )
                    qtyOptimizations++;

                // remove constant vm commands, add new and update result stack
                if( ctScope->Stack.size() == 0 )
                {
                    AddCommand( VM::Command( cInfo.TypeOrValue, true, idxInOriginal, idxInNormalized ) );
                    PushResult;
                }
                else
                {
                    for( auto i= ctScope->Stack.size() - 1; i > 0 ; --i )
                    {
                        RemoveLastCommand;
                        PopResult;
                    }
                    ResultPos= LastCommandNo;
                    LastCommand= VM::Command( cInfo.TypeOrValue, true, idxInOriginal, idxInNormalized );
                }

                #if ALIB_DEBUG
                    DBG_CALLBACK_INFO(back())
                #endif

                return;
            }

            // function
            AddCommand( cInfo.Callback, qtyArgsOrNoParentheses, cInfo.TypeOrValue,
                        ctScope->Memory.Clone( functionName ), false,
                        idxInOriginal, idxInNormalized                                  );

            // update result type stack
            if( ctScope->Stack.size()== 0 )
                PushResult;
            else
            {
                for( auto i= ctScope->Stack.size() - 1; i > 0 ; --i )
                    PopResult;
                ResultPos= LastCommandNo;
            }


            #if ALIB_DEBUG
                DBG_CALLBACK_INFO(back())
            #endif
            return;
        }
    }
    catch( Exception& e )
    {
        if(    !EnumContains(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough)
            && !e.Type().IsEnumType<Exceptions>()                                                    )
            e.Add( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name() );

        e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal  );
        throw;
    }
    catch( std::exception& stdException )
    {
        if( !EnumContains(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough) )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name()   );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                         expression.GetOriginalString(), idxInOriginal );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::StdExceptionInfo,   stdException.what() );
            throw e;
        }
        throw;
    }


    // create identifier exception
    if( qtyArgsOrNoParentheses < 0 )
        throw Exception( ALIB_CALLER_NULLED, Exceptions::UnknownIdentifier, functionName );

    // create function exception
    String128 arguments;
    arguments.DbgDisableBufferReplacementWarning();
    compiler.WriteFunctionSignature( ctScope->Stack.begin(), ctScope->Stack.end(), arguments );

    Exception e( ALIB_CALLER_NULLED, Exceptions::UnknownFunction, functionName, arguments );

    for( auto& notMatchedName : cInfo.FunctionsWithNonMatchingArguments )
        e.Add( ALIB_CALLER_NULLED, Exceptions::FunctionHint, notMatchedName );

    throw e;
}

void Program::AssembleUnaryOp( String& op, integer idxInOriginal, integer idxInNormalized )
{
    ASSERT_ASSEMBLE

    // If we have a global operator replacement, this will be used. However, in this case changes
    // of it must not be passed back to the caller as long certain normalization flags are set.
    String  opReference= op;
    bool    aliased= false;
    auto globalAliasIt=  compiler.AlphabeticUnaryOperatorAliases.find(op);
    if( globalAliasIt != compiler.AlphabeticUnaryOperatorAliases.end() )
    {
        aliased= true;
        opReference= globalAliasIt->second;
    }

    bool isConstant= collectArgs(1);

    // Nested expressions
    if(    EnumContains( compiler.CfgCompilation, Compilation::AllowCompileTimeNestedExpressions )
        && opReference == compiler.CfgNestedExpressionOperator
        && ctScope->Stack.back().IsType<String>() )
    {
        if( !At(compileStorage->resultStack.back()).IsConstant() ) // must not use bool allAreConstant here!
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::NamedExpressionNotConstant );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                         expression.GetOriginalString(), idxInOriginal );
            throw e;
        }

        String expressionName= ctScope->Stack.back().Unbox<String>();
        SPExpression nested;
        try
        {
            nested= compiler.GetNamed( expressionName );
        }
        catch( Exception& e )
        {
            if( e.Type().Value() == EnumValue( Exceptions::NamedExpressionNotFound ) )
                e.Add( ALIB_CALLER_NULLED, Exceptions::NestedExpressionNotFoundCT,
                       expressionName );
            else
            {
                ALIB_ERROR( "Unknown exception {!Q}.", e.Type() );
            }
            throw;
        }

        if( !aliased || EnumContains(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToSymbolic ) )
            op= opReference;
        else
            if( EnumContains(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToDefinedLetterCase ) )
                op= globalAliasIt->first;

        ctNestedExpressions.emplace_back(nested);
        LastCommand= VM::Command( dynamic_cast<Program*>( nested->GetProgram() ), op,
                                  idxInOriginal, idxInNormalized  );
        return;
    }


    try
    {
        for( int pass= 0; pass < 2 ; ++pass )
        {
            isConstant= collectArgs(1);
            CompilerPlugin::CIUnaryOp cInfo( *ctScope, opReference, isConstant );

            // search plug-ins
            for( auto& ppp : compileStorage->plugins )
            {
                if( !ppp.plugin->TryCompilation( cInfo ) )
                    continue;

                if( !aliased || EnumContains(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToSymbolic ) )
                    op= opReference;
                else
                    if( EnumContains(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToDefinedLetterCase ) )
                        op= globalAliasIt->first;

                // constant?
                if( !cInfo.Callback )
                {
                    qtyOptimizations++;

                    // replace last command (unary op arg is always the last)
                    back()= VM::Command( cInfo.TypeOrValue, true, idxInOriginal, idxInNormalized );

                    DBG_CALLBACK_INFO(back())

                    return;
                }

                // callback
                AddCommand( cInfo.Callback, 1, cInfo.TypeOrValue, op, true, idxInOriginal, idxInNormalized );
                ResultPos++;

                DBG_CALLBACK_INFO(back())
                return;
            }

            // did we try auto cast already?
            if( pass )
                break;

            // try auto cast
            CompilerPlugin::CIAutoCast ciAutoCast( *ctScope, op, At( ResultPos ).IsConstant(), false );
            for( auto& pppAutoCast : compileStorage->plugins )
            {
                if( !pppAutoCast.plugin->TryCompilation( ciAutoCast ) )
                    continue;

                // cast found?
                if( !ciAutoCast.TypeOrValue.IsType<void>() )
                {
                    // const?
                    if( ciAutoCast.Callback == nullptr )
                    {
                        // change constant value conversion
                        VM::Command& cmdToPatch= *( begin() + ResultPos );
                        cmdToPatch.Operation.Value= ciAutoCast.TypeOrValue;
                        ALIB_DBG( cmdToPatch.DbgInfo.Plugin= pppAutoCast.plugin; )
                    }

                    // non-const
                    else
                    {
                        // insert conversion
                        ALIB_DBG( auto insertIt= )
                        emplace( begin() + ResultPos + 1,
                                 ciAutoCast.Callback, 1,
                                 ciAutoCast.TypeOrValue,
                                 ciAutoCast.ReverseCastFunctionName, false,
                                 idxInOriginal, idxInNormalized                      );

                        (ResultPos) ++;
                        ALIB_DBG( insertIt->DbgInfo.Callback= ciAutoCast.DbgCallbackName;
                                  insertIt->DbgInfo.Plugin= pppAutoCast.plugin;            )
                    }
                }

                break;

            } // auto cast plug-in loop
        } // pass loop

    }
    catch( Exception& e )
    {
        if(    !EnumContains(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough)
            && !e.Type().IsEnumType<Exceptions>()                                                    )
            e.Add( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name() );
        e.Add    ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal  );
        throw;
    }
    catch( std::exception& stdException )
    {
        if( !EnumContains(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough) )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name()   );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,     expression.GetOriginalString(), idxInOriginal );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::StdExceptionInfo,   stdException.what() );
            throw e;
        }
        throw;
    }


    // not found
    Exception e( ALIB_CALLER_NULLED, Exceptions::UnaryOperatorNotDefined, op, TypeStr( ctScope->Stack[0] )  );
    e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal );
    throw e;
}


void Program::AssembleBinaryOp( String& op, integer idxInOriginal, integer idxInNormalized )
{
    ASSERT_ASSEMBLE

    // If we have a global operator replacement, this will be used. However, in this case changes
    // of it must not be passed back to the caller as long certain normalization flags are set.
    String  opReference= op;
    bool    aliased= false;
    auto globalAliasIt= compiler.AlphabeticBinaryOperatorAliases.find(op);
    if( globalAliasIt != compiler.AlphabeticBinaryOperatorAliases.end() )
    {
        aliased= true;
        opReference= globalAliasIt->second;
    }

    bool foundOperator=   false; // die Variable kann wohl raus. Stattdessen dort wo sie gesetzt wird ein "return" machen.
    bool triedToAutoCast= false;

    Box lhsOrigType= At(LHSResultPos).ResultType;
    Box rhsOrigType= At(RHSResultPos).ResultType;


    for(;;)
    {
        collectArgs(2);
        bool lhsIsConstant= At(LHSResultPos).IsConstant() && !EnumContains(compiler.CfgCompilation, Compilation::NoOptimization );
        bool rhsIsConstant= At(RHSResultPos).IsConstant() && !EnumContains(compiler.CfgCompilation, Compilation::NoOptimization );

        CompilerPlugin::CIBinaryOp cInfo( *ctScope, opReference, lhsIsConstant, rhsIsConstant );

        try
        {
            for( auto& ppp : compileStorage->plugins )
            {
                if( !ppp.plugin->TryCompilation( cInfo ) )
                    continue;

                if( !aliased || EnumContains(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToSymbolic ) )
                    op= opReference;
                else
                    if( EnumContains(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToDefinedLetterCase ) )
                        op= globalAliasIt->first;


                // --- identity? (like "a * 1" or "x && true")  ---
                if( cInfo.NonConstArgIsResult )
                {
                    qtyOptimizations++;

                    // lhs or rhs to remove?
                    if( lhsIsConstant )
                        erase( begin() + LHSCodeStart );
                    else
                        RemoveLastCommand;

                    PopResult;
                    ResultPos= LastCommandNo;

                    foundOperator= true;
                    break;
                }

                // --- constant? ---
                if( cInfo.Callback == nullptr )
                {
                    qtyOptimizations++;

                    // remove lhs, rhs and correct result stack
                    erase( begin() + LHSCodeStart, begin() + RHSResultPos     );

                    PopResult;
                    ResultPos= LastCommandNo;
                    LastCommand= VM::Command( cInfo.TypeOrValue, true, idxInOriginal, idxInNormalized );

                    foundOperator= true;
                    break;
                }

                //--- Callback ---

                // found correct result type stack
                PopResult;
                AddCommand( cInfo.Callback, 2, cInfo.TypeOrValue, op, true,
                            idxInOriginal, idxInNormalized  );
                ResultPos= LastCommandNo;

                DBG_CALLBACK_INFO(back())

                // done!
                foundOperator= true;
                break;
            }

            // success
            if( foundOperator )
                return;

            if( !foundOperator && triedToAutoCast )
            {
                Exception e( ALIB_CALLER_NULLED, Exceptions::BinaryOperatorNotDefined,
                             op,
                             TypeStr( lhsOrigType ),
                             TypeStr( rhsOrigType )           );

                throw e;
            }

            // try auto cast (we do this even if types are equal )
            triedToAutoCast= true;
            CompilerPlugin::CIAutoCast ciAutoCast( *ctScope, op,
                                                   At( LHSResultPos ).IsConstant(),
                                                   At( RHSResultPos ).IsConstant()   );

            for( auto& pppAutoCast : compileStorage->plugins )
            {
                if( !pppAutoCast.plugin->TryCompilation( ciAutoCast ) )
                    continue;

                // cast for lhs?
                if( !ciAutoCast.TypeOrValue.IsType<void>() )
                {
                    // const?
                    if( ciAutoCast.Callback == nullptr )
                    {
                        // change constant value conversion
                        VM::Command& cmdToPatch= *( begin() + LHSResultPos );
                        cmdToPatch.Operation.Value= ciAutoCast.TypeOrValue;
                        ALIB_DBG( cmdToPatch.DbgInfo.Plugin= pppAutoCast.plugin;                 )
                    }

                    // cast function upgrade for lhs?
                    else
                    {
                        // insert conversion
                        ALIB_DBG( auto insertIt= )
                        emplace( begin() + LHSResultPos + 1,
                                 ciAutoCast.Callback, 1,
                                 ciAutoCast.TypeOrValue,
                                 ciAutoCast.ReverseCastFunctionName, false,
                                 idxInOriginal, idxInNormalized                      );

                        (LHSResultPos) ++;
                        (RHSResultPos) ++;
                        ALIB_DBG( insertIt->DbgInfo.Callback= ciAutoCast.DbgCallbackName;
                                  insertIt->DbgInfo.Plugin=   pppAutoCast.plugin;               )
                    }
                }

                // cast for rhs?
                if( !ciAutoCast.TypeOrValueRhs.IsType<void>() )
                {
                    // const?
                    if( ciAutoCast.CallbackRhs == nullptr )
                    {
                        // change constant value conversion
                        back().Operation.Value= ciAutoCast.TypeOrValueRhs;
                        ALIB_DBG( back().DbgInfo.Plugin= pppAutoCast.plugin;                 )
                    }

                    // cast function upgrade for rhs?
                    else
                    {
                        // insert conversion
                        ALIB_DBG( auto insertIt= )
                        emplace( begin() + RHSResultPos + 1,
                                 ciAutoCast.CallbackRhs, 1,
                                 ciAutoCast.TypeOrValueRhs,
                                 ciAutoCast.ReverseCastFunctionNameRhs, false,
                                 idxInOriginal, idxInNormalized                     );
                        (RHSResultPos)++;

                        ALIB_DBG( insertIt->DbgInfo.Callback= ciAutoCast.DbgCallbackNameRhs;
                                  insertIt->DbgInfo.Plugin= pppAutoCast.plugin;               )
                    }
                }
                break;

            } // auto cast plug-in loop
        }
        catch( Exception& e )
        {
            if(    !EnumContains(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough)
                && !e.Type().IsEnumType<Exceptions>()                                                    )
                e.Add( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name() );
            e.Add    ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,    expression.GetOriginalString(), idxInOriginal  );
            throw;
        }
        catch( std::exception& stdException )
        {
            if( !EnumContains(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough) )
            {
                Exception e( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name()   );
                e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo   , expression.GetOriginalString(), idxInOriginal );
                e.Add      ( ALIB_CALLER_NULLED, Exceptions::StdExceptionInfo , stdException.what() );
                throw e;
            }
            throw;
        }

    } // formally endless loop (max 2)
}

// flags to access values on the info-stack used to compile conditionals
#define QJumpPos    std::get<0>( compileStorage->conditionalStack.top() )
#define TJumpPos    std::get<1>( compileStorage->conditionalStack.top() )
#define QConstFlags std::get<2>( compileStorage->conditionalStack.top() )


void   Program::AssembleCondFinalize_Q( integer idxInOriginal, integer idxInNormalized )
{
    ASSERT_ASSEMBLE
    // Note:
    // The "conditional stack" of tuples stores for each nested condition, three values:
    // 1. The position of the lhs result,
    // 2. The position of the jump command between T and F
    // 3. An integer with two bits: bit 1 tells us whether Q was constant and bit 0 which value
    //    the constant Q had. "Had" because it is removed right away.
    // But even if Q ist constant, both

    // Q constant?
    int constQ= 0;
    if( back().IsConstant() && !EnumContains(compiler.CfgCompilation, Compilation::NoOptimization ) )
    {
        qtyOptimizations++;

        Box& condition= back().Operation.Value;
        constQ= 2 + ( condition.Call<FIsTrue>()  ?  1 :  0 );
        pop_back(); // remove constant Q
    }

    // insert Q-Jump
    AddCommand( Command::JumpType::Conditional, idxInOriginal, idxInNormalized );
    compileStorage->conditionalStack.emplace( LastCommandNo, 0, constQ );
}


void    Program::AssembleCondFinalize_T( integer idxInOriginal, integer idxInNormalized )
{
    ASSERT_ASSEMBLE

    // insert T-Jump
    AddCommand( Command::JumpType::Unconditional, idxInOriginal, idxInNormalized );
    ResultPos++; // for the time being this points to the jump command.
                 // Otherwise upcoming F optimizations don't know where to find the start of F!

    // patch Q-Jump to command after T-Jump
    At(QJumpPos).Operation.Distance= Length() - QJumpPos;

    // store T-Jump address on conditional stack
    TJumpPos= LastCommandNo;
}

void    Program::AssembleCondFinalize_F( integer idxInOriginal, integer idxInNormalized )
{
    ASSERT_ASSEMBLE

    // patch result stack position of T one back (in Finalize_T we had increased it by one to
    // point to the jump command, to protect it from being deleted with an lhs-delete
    LHSResultPos--;

    At( TJumpPos ).Operation.Distance= Length() - TJumpPos;


    // needs type alignment?
    if( !At( LHSResultPos ).ResultType.IsSameType( At( RHSResultPos ).ResultType )    )
    {
        collectArgs(2);
        String condOp= A_CHAR("Q?T:F");
        CompilerPlugin::CIAutoCast ciAutoCast( *ctScope, condOp,
                                                At( LHSResultPos ).IsConstant(),
                                                At( RHSResultPos ).IsConstant()   );
        bool found= false;
        try
        {
            for( auto& ppp : compileStorage->plugins )
                if( ppp.plugin->TryCompilation( ciAutoCast ) )
                {
                    if( !ciAutoCast.TypeOrValue.IsType<void>() )
                    {
                        // const cast upgrade for T?
                        if(  ciAutoCast.Callback == nullptr )
                        {
                            // change constant value conversion and patch type in jump command
                            At(LHSResultPos).Operation.Value=
                            At(LHSResultPos).ResultType     = ciAutoCast.TypeOrValue;

                            ALIB_DBG( At(LHSResultPos).DbgInfo.Plugin= ppp.plugin;    )
                        }

                        // upgrade function for T?
                        else if( ciAutoCast.Callback )
                        {
                            // jump one more (the other as well)
                            At(QJumpPos).Operation.Distance++;
                            At(TJumpPos).Operation.Distance++;

                            // insert conversion
                            ALIB_DBG( auto insertIt= )
                            emplace( begin() + TJumpPos++,
                                     ciAutoCast.Callback, 1,
                                     ciAutoCast.TypeOrValue,
                                     ciAutoCast.ReverseCastFunctionName, false,
                                     idxInOriginal, idxInNormalized                     );
                            ALIB_DBG( insertIt->DbgInfo.Callback= ciAutoCast.DbgCallbackName;
                                      insertIt->DbgInfo.Plugin= ppp.plugin;               )
                            LHSResultPos++;
                        }
                    }

                    // const cast upgrade for F?
                    if( !ciAutoCast.TypeOrValueRhs.IsType<void>() )
                    {
                        if( ciAutoCast.Callback == nullptr )
                        {
                            // change constant value
                            back().Operation.Value=
                            back().ResultType     =  ciAutoCast.TypeOrValueRhs;

                            ALIB_DBG( back().DbgInfo.Callback= ciAutoCast.DbgCallbackNameRhs;
                                      back().DbgInfo.Plugin= ppp.plugin;                    )
                        }

                        // upgrade function for T?
                        else
                        {
                            // insert conversion
                            AddCommand( ciAutoCast.CallbackRhs, 1,
                                        ciAutoCast.TypeOrValueRhs,
                                        ciAutoCast.ReverseCastFunctionNameRhs, false,
                                        idxInOriginal, idxInNormalized                    );
                            RHSResultPos++;
                            At(TJumpPos).Operation.Distance++;

                            ALIB_DBG( back().DbgInfo.Callback= ciAutoCast.DbgCallbackNameRhs;
                                      back().DbgInfo.Plugin= ppp.plugin;                    )
                        }
                    }

                   found= true;
                   break;
                }
        }
        catch( Exception& e )
        {
            if(    !EnumContains(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough)
                && !e.Type().IsEnumType<Exceptions>()                                                    )
                e.Add( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name() );
            e.Add    ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal  );
            throw;
        }
        catch( std::exception& stdException )
        {
            if( !EnumContains(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough) )
            {
                Exception e( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name()   );
                e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,     expression.GetOriginalString(), idxInOriginal );
                e.Add      ( ALIB_CALLER_NULLED, Exceptions::StdExceptionInfo,   stdException.what() );
                throw e;
            }
            throw;
        }


        if(!found)
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::IncompatibleTypesInConditional,
                         TypeStr( * ciAutoCast.ArgsBegin   ),
                         TypeStr( *(ciAutoCast.ArgsBegin+1))            );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal  );

            throw e;
        }
    }

    // was this a constant conditional to be optimized out?
    if( QConstFlags  )
    {
        // eliminate T?
        if( (QConstFlags & 1) == 0  )
        {
            erase( begin() + QJumpPos,  begin() + TJumpPos + 1 );
        }

        // eliminate F?
        else
        {
            erase( begin() + TJumpPos,  end()  );
            erase( begin() + QJumpPos );
        }
    }
    else
        // mark last command as part of conditional. Otherwise constant F-terms become optimized
        back().SetEndOfConditionalFlag();


    // clean the conditional stack
    compileStorage->conditionalStack.pop();

    // remove results Q [? T : F]
    PopResult;
    PopResult;
    ResultPos= LastCommandNo;
}


void Program::AssembleFinalize()
{
    ASSERT_ASSEMBLE

    ALIB_ASSERT_ERROR( compileStorage->conditionalStack.size() == 0,
                       "Finalizing program, while conditional stack is of size {}.",
                       compileStorage->conditionalStack.size()                             )
    ALIB_ASSERT_ERROR( compileStorage->resultStack.size() == 1,
                       "Finalizing program, while result stack is of size {}.",
                       compileStorage->resultStack.size()                                  )
    if( compileStorage )
    {
        delete compileStorage;
        compileStorage= nullptr;
    }
}


#undef TypeStr
#undef AddCommand
#undef RemoveLastCommand
#undef LastCommandNo
#undef LastCommand
#undef PushResult
#undef PopResult
#undef ResultPos
#undef LHSResultPos
#undef RHSResultPos
#undef LHSCodeStart
//#undef RHSCodeStart

}}}} // namespace [aworx::lib::expressions::detail]

