// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM)
#   include "alib/expressions/detail/program.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN)
#   include "alib/expressions/compilerplugin.hpp"
#endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace expressions { namespace detail {

//! @cond NO_DOX
using Command= VirtualMachine::Command;
//! @endcond

// #################################################################################################
// Program
// #################################################################################################

#if ALIB_DEBUG
#   define DBG_SET_CALLBACK_INFO                                                                   \
        prg.act().DbgInfo.Callback= cInfo.DbgCallbackName;                                             \
        prg.act().DbgInfo.Plugin  = ppp.plugin;
#else
#   define DBG_SET_CALLBACK_INFO
#endif

// Use temporary allocator for program vector construction.
// In method AssembleFinalize, this will be re-allocated into ctScope
Program::Program( Compiler& pCompiler, Expression&  pExpression, MonoAllocator* compileTimeAlloc)
: compiler           ( pCompiler   )
, expression         ( pExpression )
, ctNestedExpressions( pExpression.ctScope->Allocator )
, qtyOptimizations   ( HasBits( compiler.CfgCompilation, Compilation::NoOptimization )
                       ? -1 : 0 )
, compileStorage     ( compileTimeAlloc ? compileTimeAlloc->Emplace<CompileStorage>( *compileTimeAlloc )
                                        : nullptr )
{}

Program::~Program()
{
    if( compileStorage )
        monomem::Destruct( compileStorage );
}

const Box& Program::ResultType() const
{
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    return commands[commandsCount-1].ResultType;
    ALIB_WARNINGS_RESTORE
}

// #################################################################################################
// Helpers used during compilation
// #################################################################################################
#if !defined(ALIB_DOX)
namespace {
struct Assembly
{
    using VM= VirtualMachine;

    std::vector<VirtualMachine::Command*, StdContMA<VirtualMachine::Command*>>& assembly;
    std::vector<VM::PC                  , StdContMA<VM::PC                  >>& resultStack;

    ALIB_FORCE_INLINE
    Assembly( std::vector<VirtualMachine::Command*, StdContMA<VirtualMachine::Command*>>& pAssembly,
              std::vector<VM::PC                  , StdContMA<VM::PC                  >>& pResultStack )
    : assembly   ( pAssembly )
    , resultStack( pResultStack)
    {}

    /**
     * Returns the current last command.
     * @return A reference to the last command.
     */
    ALIB_FORCE_INLINE
    integer length()
    {
        return static_cast<integer>(assembly.size());
    }

    /**
     * Returns the command at the given program counter \p{pc}.
     * @param  pc  The program counter of the command to get.
     * @return A reference to the requested command.
     */
    ALIB_FORCE_INLINE
    VM::Command& at( VM::PC pc )
    {
        return *assembly[static_cast<size_t>(pc)];
    }

    /**
     * Returns the current last command.
     * @return A reference to the last command.
     */
    ALIB_FORCE_INLINE
    VM::Command& act()
    {
        return *assembly.back();
    }

    /**
     * Returns the current last command.
     * @return A reference to the last command.
     */
    ALIB_FORCE_INLINE
    VM::Command& prev()
    {
        return **( assembly.end() - 2 );
    }

    /**
     * Returns the number of the last command.
     * @return The current size of the program minus \c 1.
     */
    ALIB_FORCE_INLINE
    VM::PC actPC()
    {
        return static_cast<VM::PC>( assembly.size() - 1);
    }

    /**
     * Removes the last command.
     */
    ALIB_FORCE_INLINE
    void eraseLast()
    {
        assembly.pop_back();
    }

    /**
     * Removes a command.
     * @param pc The command to remove.
     */
    ALIB_FORCE_INLINE
    void erase( VM::PC pc )
    {
        assembly.erase( assembly.begin() + pc );

    }

    /**
     * Removes range of commands.
     * @param begin The first command to remove.
     * @param end   The first command that is not removed.
     */
    ALIB_FORCE_INLINE
    void erase( VM::PC begin, VM::PC end )
    {
        assembly.erase( assembly.begin() + begin,
                        assembly.begin() + end );
    }


    /**
     * Inserts a command at the given position.
     * @tparam TArgs   Types of the variadic arguments \p{args}.
     * @param  pc      The position of insertion.
     * @param  args    Variadic arguments forwarded to the command's constructor.
     * @return The command inserted.
     */
    template<typename... TArgs>
    ALIB_FORCE_INLINE
    VM::Command&  insertAt( VM::PC pc, TArgs && ... args )
    {
        auto* cmd= assembly.get_allocator().allocator.Emplace<VM::Command>( std::forward<TArgs>( args )... );
        assembly.emplace( assembly.begin() + pc,  cmd );
        return *cmd;
    }

    /**
     * Inserts a command at the end of the program.
     * @tparam TArgs   Types of the variadic arguments \p{args}.
     * @param  args    Variadic arguments forwarded to the command's constructor.
     * @return The command inserted.
     */
    template<typename... TArgs>
    ALIB_FORCE_INLINE
    VM::Command& add( TArgs && ... args )
    {
        assembly.emplace_back( assembly.get_allocator().allocator.Emplace<VM::Command>(std::forward<TArgs>( args )... ) );
        return *assembly.back();
    }


    /** Pushes the current PC to the result stack.  */
    ALIB_FORCE_INLINE
    void pushResultPC()
    {
        resultStack.push_back( actPC() );
    }

    /** Pops one from the result stack.  */
    ALIB_FORCE_INLINE
    void popResultPC()
    {
        resultStack.pop_back();
    }

    /** Returns a mutable reference to the top of the stack of result positions.  */
    ALIB_FORCE_INLINE
    VM::PC & resultPC()
    {
        return resultStack.back();
    }

    /** Returns a mutable reference to the 2nd.-top of the stack of result positions.  */
    ALIB_FORCE_INLINE
    VM::PC & lhsResultPC()
    {
        return resultStack[resultStack.size() - 2];
    }

    /** Returns the program counter that identifies the start of the range that results in the
     *  current LHS value.  */
    ALIB_FORCE_INLINE
    VM::PC lhsResultStartPC()
    {
        auto qtyResults = resultStack.size();
        return qtyResults == 2 ? 0
                               : resultStack[qtyResults - 3] + 1; // one after the previous
    }
};
} // anonymous namespace
#endif // ALIB_DOX

// #################################################################################################
// Assemble commands
// #################################################################################################

#define ASSERT_ASSEMBLE                                                                            \
    ALIB_ASSERT_ERROR(    prg.resultStack.empty()                                                  \
                       || prg.resultPC() == prg.actPC()                                            \
                       || prg.act().IsConditionalJump() ,                                          \
                       "EXPR", "Internal error: Last in result stack is not last command." )

bool Program::collectArgs( integer qty )
{
    ALIB_ASSERT_ERROR( compileStorage->ResultStack.size() >= static_cast<size_t>( qty < 0 ? 0 : qty ),
            "EXPR",    "Not enough arguments on the stack. This should never happen (internal error)." )

    expression.ctScope->Stack.clear();
    if( qty > 0 )
        expression.ctScope->Stack.reserve( static_cast<size_t>( qty ));

    bool allAreConst= true;
    for( integer i= qty; i > 0 ; --i )
    {
        Command& cmd=  *compileStorage->Assembly[static_cast<size_t>(*(compileStorage->ResultStack.end() - i))];
        bool     isConstant=  cmd.IsConstant();
        expression.ctScope->Stack.emplace_back( isConstant ? cmd.Operation.Value
                                                : cmd.ResultType      );
        allAreConst&= isConstant;
    }

    return allAreConst && !HasBits(compiler.CfgCompilation, Compilation::NoOptimization );
}


void Program::AssembleConstant( Box& value, integer idxInOriginal, integer idxInNormalized )
{
    if( compileStorage == nullptr )
        return;
    Assembly prg( compileStorage->Assembly, compileStorage->ResultStack);
    ASSERT_ASSEMBLE

    prg.add( VM::Command( value.IsType<String>() ?  Box( expression.ctScope->Allocator.EmplaceString(value.Unbox<String>()) )
                                                 :  value,
                          false, idxInOriginal, idxInNormalized  )  );
    prg.pushResultPC();
}


void Program::AssembleFunction( AString& functionName , integer qtyArgsOrNoParentheses,
                                integer  idxInOriginal, integer idxInNormalized  )
{
    if( compileStorage == nullptr )
        return;
    Assembly prg( compileStorage->Assembly, compileStorage->ResultStack);
    ASSERT_ASSEMBLE

    // Nested expressions
    if(    compiler.CfgNestedExpressionFunction.GetRawName().IsNotEmpty()
        && compiler.CfgNestedExpressionFunction.Match(functionName )       )
    {
        functionName.Reset( compiler.CfgNestedExpressionFunction );

        if(    qtyArgsOrNoParentheses < (HasBits( compiler.CfgCompilation, Compilation::AllowCompileTimeNestedExpressions )
                                         ? 1 : 2 )
            || !prg.at( *(prg.resultStack.end() - ( qtyArgsOrNoParentheses == 3 ? 2 : qtyArgsOrNoParentheses) ) )
                .ResultType.IsType<String>() )
        {
            throw Exception( ALIB_CALLER_NULLED, Exceptions::NestedExpressionCallArgumentMismatch,
                             compiler.CfgNestedExpressionFunction );
        }


        // single argument? -> we have to get the expression now
        if( qtyArgsOrNoParentheses == 1 )
        {
            if( !prg.at(compileStorage->ResultStack.back()).IsConstant() ) // must not use bool allAreConstant here!
            {
                Exception e( ALIB_CALLER_NULLED, Exceptions::NamedExpressionNotConstant );
                e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                             expression.GetOriginalString(), idxInOriginal );
                throw e;
            }

            SPExpression nested;
            String nestedExpressionName= prg.at(prg.resultStack.back()).ResultType.Unbox<String>();
            try
            {
                nested= compiler.GetNamed( nestedExpressionName );
            }
            catch( Exception& e )
            {
                if( e.Type().Integral() == UnderlyingIntegral( Exceptions::NamedExpressionNotFound ) )
                {
                    e.Add( ALIB_CALLER_NULLED, Exceptions::NestedExpressionNotFoundCT ,
                           nestedExpressionName );
                }
                else
                {
                    ALIB_ERROR( "EXPR", "Unknown exception {!Q}.", e.Type() )
                }
                throw;
            }

            ctNestedExpressions.emplace_back(nested);

            prg.act()= VM::Command( nested->GetProgram(),
                                    nested->ResultType(),
                                    compiler.CfgNestedExpressionFunction.GetRawName(),
                                    idxInOriginal, idxInNormalized                       );
            return;
        }

        // If two arguments, we send nullptr to indicate that 2nd argument is replacement
        if( qtyArgsOrNoParentheses == 2 )
        {
            prg.add( static_cast<Program*>( nullptr ),  Box( nullptr ),
                     compiler.CfgNestedExpressionFunction.GetRawName(),
                     idxInOriginal, idxInNormalized                     );
            prg.act().ResultType= prg.prev().ResultType;
        }

        // 3rd argument given (throw): we send "this" which indicates to throw if an
        // expression is not found.
        else
            prg.add( dynamic_cast<Program*>( this ),
                     prg.act().ResultType,
                     compiler.CfgNestedExpressionFunction.GetRawName(),
                     idxInOriginal, idxInNormalized                     );

        prg.popResultPC();
        prg.resultPC()= prg.actPC();
        return;
    }


    // collect arguments
    bool allAreConstant= collectArgs(qtyArgsOrNoParentheses);

    compileStorage->FunctionsWithNonMatchingArguments.Clear();
    CompilerPlugin::CIFunction cInfo( *expression.ctScope,
                                      compileStorage->ConditionalStack.get_allocator().allocator,
                                      functionName,
                                      (qtyArgsOrNoParentheses < 0), //no parentheses?
                                      allAreConstant,
                                      compileStorage->FunctionsWithNonMatchingArguments );

    try
    {
        for( auto& ppp : compiler.plugins )
        {
            if( !ppp.plugin->TryCompilation( cInfo ) )
                continue;

            // constant?
            if( cInfo.Callback == nullptr )
            {
                if( qtyArgsOrNoParentheses > 0 )
                    ++qtyOptimizations;

                // remove constant vm commands, add new and update result stack
                if( expression.ctScope->Stack.size() == 0 )
                {
                    prg.add( VM::Command( cInfo.TypeOrValue, true, idxInOriginal, idxInNormalized ) );
                    prg.pushResultPC();
                }
                else
                {
                    for( auto i= expression.ctScope->Stack.size() - 1; i > 0 ; --i )
                    {
                        prg.eraseLast();
                        prg.popResultPC();
                    }
                    prg.resultPC()= prg.actPC();
                    prg.act()= VM::Command( cInfo.TypeOrValue, true, idxInOriginal, idxInNormalized );
                }

                DBG_SET_CALLBACK_INFO

                return;
            }

            // function
            prg.add( cInfo.Callback, qtyArgsOrNoParentheses, cInfo.TypeOrValue,
                     expression.ctScope->Allocator.EmplaceString( functionName ), false,
                     idxInOriginal, idxInNormalized                                  );

            // update result type stack
            if( expression.ctScope->Stack.size()== 0 )
                prg.pushResultPC();
            else
            {
                for( auto i= expression.ctScope->Stack.size() - 1; i > 0 ; --i )
                    prg.popResultPC();
                prg.resultPC()= prg.actPC();
            }

            DBG_SET_CALLBACK_INFO
            return;
        }
    }
    catch( Exception& e )
    {
        if(    !HasBits(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough)
            && !e.Type().IsEnumType<Exceptions>()                                                 )
            e.Add( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name() );

        e.Add    ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal  );
        throw;
    }
    catch( std::exception& stdException )
    {
        if( !HasBits(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough) )
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
    compiler.WriteFunctionSignature( expression.ctScope->Stack.begin(), expression.ctScope->Stack.end(), arguments );

    Exception e( ALIB_CALLER_NULLED, Exceptions::UnknownFunction, functionName, arguments );

    for( auto& notMatchedName : cInfo.FunctionsWithNonMatchingArguments )
        e.Add( ALIB_CALLER_NULLED, Exceptions::FunctionHint, notMatchedName );

    throw e;
}

void Program::AssembleUnaryOp( String& op, integer idxInOriginal, integer idxInNormalized )
{
    if( compileStorage == nullptr )
        return;
    Assembly prg( compileStorage->Assembly, compileStorage->ResultStack);
    ASSERT_ASSEMBLE

    // If we have a global operator replacement, this will be used. However, in this case changes
    // of it must not be passed back to the caller as long certain normalization flags are set.
    String  opReference= op;
    bool    aliased= false;
    auto globalAliasIt=  compiler.AlphabeticUnaryOperatorAliases.Find(op);
    if( globalAliasIt != compiler.AlphabeticUnaryOperatorAliases.end() )
    {
        aliased= true;
        opReference= globalAliasIt.Mapped();
    }

    bool isConstant= collectArgs(1);

    // Nested expressions
    if(    HasBits( compiler.CfgCompilation, Compilation::AllowCompileTimeNestedExpressions )
        && opReference == compiler.CfgNestedExpressionOperator
        && expression.ctScope->Stack.back().IsType<String>() )
    {
        if( !prg.at(compileStorage->ResultStack.back()).IsConstant() ) // must not use bool allAreConstant here!
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::NamedExpressionNotConstant );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,
                         expression.GetOriginalString(), idxInOriginal );
            throw e;
        }

        String expressionName= expression.ctScope->Stack.back().Unbox<String>();
        SPExpression nested;
        try
        {
            nested= compiler.GetNamed( expressionName );
        }
        catch( Exception& e )
        {
            if( e.Type().Integral() == UnderlyingIntegral( Exceptions::NamedExpressionNotFound ) )
                e.Add( ALIB_CALLER_NULLED, Exceptions::NestedExpressionNotFoundCT,
                       expressionName );
            else
            {
                ALIB_ERROR( "EXPR", "Unknown exception {!Q}.", e.Type() )
            }
            throw;
        }

        if( !aliased || HasBits(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToSymbolic ) )
            op= opReference;
        else
            if( HasBits(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToDefinedLetterCase ) )
                op= globalAliasIt->first;

        ctNestedExpressions.emplace_back(nested);
        prg.act()= VM::Command( nested->GetProgram(),
                               nested->ResultType(),
                               op,
                               idxInOriginal, idxInNormalized  );
        return;
    }


    try
    {
        for( int pass= 0; pass < 2 ; ++pass )
        {
            isConstant= collectArgs(1);
            CompilerPlugin::CIUnaryOp cInfo( *expression.ctScope,
                                             compileStorage->ConditionalStack.get_allocator().allocator,
                                             opReference, isConstant );

            // search plug-ins
            for( auto& ppp : compiler.plugins )
            {
                if( !ppp.plugin->TryCompilation( cInfo ) )
                    continue;

                if( !aliased || HasBits(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToSymbolic ) )
                    op= opReference;
                else
                    if( HasBits(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToDefinedLetterCase ) )
                        op= globalAliasIt->first;

                // constant?
                if( !cInfo.Callback )
                {
                    ++qtyOptimizations;

                    // replace last command (unary op arg is always the last)
                    prg.act()= VM::Command( cInfo.TypeOrValue, true, idxInOriginal, idxInNormalized );

                    DBG_SET_CALLBACK_INFO

                    return;
                }

                // callback
                prg.add( cInfo.Callback, 1, cInfo.TypeOrValue, op, true, idxInOriginal, idxInNormalized );
                ++prg.resultPC();

                DBG_SET_CALLBACK_INFO
                return;
            }

            // did we try auto cast already?
            if( pass )
                break;

            // try auto cast
            CompilerPlugin::CIAutoCast ciAutoCast( *expression.ctScope,
                                                   compileStorage->ConditionalStack.get_allocator().allocator,
                                                   op,
                                                   prg.at( prg.resultPC() ).IsConstant(), false );
            for( auto& pppAutoCast : compiler.plugins )
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
                        VM::Command& cmdToPatch= prg.at(prg.resultPC());
                        cmdToPatch.Operation.Value= ciAutoCast.TypeOrValue;
                        ALIB_DBG( cmdToPatch.DbgInfo.Plugin= pppAutoCast.plugin; )
                    }

                    // non-const
                    else
                    {
                        // insert conversion
                        ALIB_DBG( auto& newCmd= )
                        prg.insertAt( prg.resultPC() + 1, ciAutoCast.Callback, 1,
                                                          ciAutoCast.TypeOrValue,
                                                          ciAutoCast.ReverseCastFunctionName, false,
                                                          idxInOriginal, idxInNormalized                   );

                        ++prg.resultPC();
                        ALIB_DBG( newCmd.DbgInfo.Callback= ciAutoCast.DbgCallbackName;
                                  newCmd.DbgInfo.Plugin  = pppAutoCast.plugin;            )
                    }
                }

                break;

            } // auto cast plug-in loop
        } // pass loop

    }
    catch( Exception& e )
    {
        if(    !HasBits(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough)
            && !e.Type().IsEnumType<Exceptions>()                                                    )
            e.Add( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name() );
        e.Add    ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal  );
        throw;
    }
    catch( std::exception& stdException )
    {
        if( !HasBits(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough) )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name()   );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,     expression.GetOriginalString(), idxInOriginal );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::StdExceptionInfo,   stdException.what() );
            throw e;
        }
        throw;
    }


    // not found
    Exception e( ALIB_CALLER_NULLED, Exceptions::UnaryOperatorNotDefined, op, compiler.TypeName( expression.ctScope->Stack[0] )  );
    e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal );
    throw e;
}


void Program::AssembleBinaryOp( String& op, integer idxInOriginal, integer idxInNormalized )
{
    if( compileStorage == nullptr )
        return;
    Assembly prg( compileStorage->Assembly, compileStorage->ResultStack);
    ASSERT_ASSEMBLE

    // If we have a global operator replacement, this will be used. However, in this case changes
    // of it must not be passed back to the caller as long certain normalization flags are set.
    String  opReference= op;
    bool    aliased= false;
    auto globalAliasIt=  compiler.AlphabeticBinaryOperatorAliases.Find(op);
    if( globalAliasIt != compiler.AlphabeticBinaryOperatorAliases.end() )
    {
        aliased= true;
        opReference= globalAliasIt.Mapped();
    }

    bool foundOperator=   false; // die Variable kann wohl raus. Stattdessen dort wo sie gesetzt wird ein "return" machen.
    bool triedToAutoCast= false;

    Box lhsOrigType= prg.at(prg.lhsResultPC()).ResultType;
    Box rhsOrigType= prg.at(prg.   resultPC()).ResultType;


    for(;;)
    {
        collectArgs(2);
        bool lhsIsConstant= prg.at(prg.lhsResultPC()).IsConstant() && !HasBits(compiler.CfgCompilation, Compilation::NoOptimization );
        bool rhsIsConstant= prg.at(prg.   resultPC()).IsConstant() && !HasBits(compiler.CfgCompilation, Compilation::NoOptimization );

        CompilerPlugin::CIBinaryOp cInfo( *expression.ctScope,
                                           compileStorage->ConditionalStack.get_allocator().allocator,
                                           opReference, lhsIsConstant, rhsIsConstant );

        try
        {
            for( auto& ppp : compiler.plugins )
            {
                if( !ppp.plugin->TryCompilation( cInfo ) )
                    continue;

                if( !aliased || HasBits(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToSymbolic ) )
                    op= opReference;
                else
                    if( HasBits(compiler.CfgNormalization, Normalization::ReplaceVerbalOperatorsToDefinedLetterCase ) )
                        op= globalAliasIt->first;


                // --- identity? (like "a * 1" or "x && true")  ---
                if( cInfo.NonConstArgIsResult )
                {
                    ++qtyOptimizations;

                    // lhs or rhs to remove?
                    if( lhsIsConstant )
                        prg.erase( prg.lhsResultStartPC() );
                    else
                        prg.eraseLast();

                    prg.popResultPC();
                    prg.resultPC()= prg.actPC();

                    foundOperator= true;
                    break;
                }

                // --- constant? ---
                if( cInfo.Callback == nullptr )
                {
                    ++qtyOptimizations;

                    // remove lhs, rhs and correct result stack
                    prg.erase( prg.lhsResultStartPC(), prg.resultPC()     );

                    prg.popResultPC();
                    prg.resultPC()= prg.actPC();
                    prg.act()= VM::Command( cInfo.TypeOrValue, true, idxInOriginal, idxInNormalized );

                    foundOperator= true;
                    break;
                }

                //--- Callback ---

                // found correct result type stack
                prg.popResultPC();
                prg.add( cInfo.Callback, 2, cInfo.TypeOrValue, op, true, idxInOriginal, idxInNormalized );
                prg.resultPC()= prg.actPC();

                DBG_SET_CALLBACK_INFO

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
                             compiler.TypeName( lhsOrigType ),
                             compiler.TypeName( rhsOrigType )           );

                throw e;
            }

            // try auto cast (we do this even if types are equal )
            triedToAutoCast= true;
            CompilerPlugin::CIAutoCast ciAutoCast( *expression.ctScope,
                                                   compileStorage->ConditionalStack.get_allocator().allocator,
                                                   op,
                                                   prg.at( prg.lhsResultPC() ).IsConstant(),
                                                   prg.at( prg.   resultPC() ).IsConstant()   );

            for( auto& pppAutoCast : compiler.plugins )
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
                        VM::Command& cmdToPatch= prg.at( prg.lhsResultPC() );
                        cmdToPatch.Operation.Value= ciAutoCast.TypeOrValue;
                        ALIB_DBG( cmdToPatch.DbgInfo.Plugin= pppAutoCast.plugin;                 )
                    }

                    // cast function upgrade for lhs?
                    else
                    {
                        // insert conversion
                        ALIB_DBG( auto& newCmd= )
                        prg.insertAt( prg.lhsResultPC() + 1, ciAutoCast.Callback, 1,
                                                             ciAutoCast.TypeOrValue,
                                                             ciAutoCast.ReverseCastFunctionName, false,
                                                             idxInOriginal, idxInNormalized                );

                        ++prg.lhsResultPC();
                        ++prg.resultPC();
                        ALIB_DBG( newCmd.DbgInfo.Callback= ciAutoCast.DbgCallbackName;
                                  newCmd.DbgInfo.Plugin=   pppAutoCast.plugin;               )
                    }
                }

                // cast for rhs?
                if( !ciAutoCast.TypeOrValueRhs.IsType<void>() )
                {
                    // const?
                    if( ciAutoCast.CallbackRhs == nullptr )
                    {
                        // change constant value conversion
                        prg.act().Operation.Value= ciAutoCast.TypeOrValueRhs;
                        ALIB_DBG( prg.act().DbgInfo.Plugin= pppAutoCast.plugin;                 )
                    }

                    // cast function upgrade for rhs?
                    else
                    {
                        // insert conversion
                        ALIB_DBG( auto& newCmd= )
                        prg.insertAt( prg.resultPC() + 1, ciAutoCast.CallbackRhs, 1,
                                                          ciAutoCast.TypeOrValueRhs,
                                                          ciAutoCast.ReverseCastFunctionNameRhs, false,
                                                          idxInOriginal, idxInNormalized                );
                        ++prg.resultPC();

                        ALIB_DBG( newCmd.DbgInfo.Callback= ciAutoCast.DbgCallbackNameRhs;
                                  newCmd.DbgInfo.Plugin= pppAutoCast.plugin;               )
                    }
                }
                break;

            } // auto cast plug-in loop
        }
        catch( Exception& e )
        {
            if(    !HasBits(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough)
                && !e.Type().IsEnumType<Exceptions>()                                                    )
                e.Add( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name() );
            e.Add    ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo,    expression.GetOriginalString(), idxInOriginal  );
            throw;
        }
        catch( std::exception& stdException )
        {
            if( !HasBits(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough) )
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


void   Program::AssembleCondFinalize_Q( integer idxInOriginal, integer idxInNormalized )
{
    if( compileStorage == nullptr )
        return;
    Assembly prg( compileStorage->Assembly, compileStorage->ResultStack);
    ASSERT_ASSEMBLE

    // Note:
    // The "conditional stack" of tuples stores for each nested condition, three values:
    // 1. The position of the lhs result,
    // 2. The position of the jump command between T and F
    // 3. An integer with two bits: bit 1 tells us whether Q was constant and bit 0 which value
    //    the constant Q had. "Had" because it is removed right away.

    // Q constant?
    int constQ= 0;
    if( prg.act().IsConstant() && !HasBits(compiler.CfgCompilation, Compilation::NoOptimization ) )
    {
        ++qtyOptimizations;

        Box& condition= prg.act().Operation.Value;
        constQ= 2 + ( condition.Call<FIsTrue>()  ?  1 :  0 );
        prg.eraseLast(); // remove constant Q
    }

    // insert Q-Jump
    prg.add( Command::JumpType::Conditional, idxInOriginal, idxInNormalized );
    compileStorage->ConditionalStack.emplace_back( prg.actPC(), 0, constQ );
}


void    Program::AssembleCondFinalize_T( integer idxInOriginal, integer idxInNormalized )
{
    if( compileStorage == nullptr )
        return;

    Assembly prg( compileStorage->Assembly, compileStorage->ResultStack);
    ASSERT_ASSEMBLE

    // insert T-Jump
    prg.add( Command::JumpType::Unconditional, idxInOriginal, idxInNormalized );
    ++prg.resultPC(); // for the time being this points to the jump command.
                      // Otherwise upcoming F optimizations don't know where to find the start of F!

    auto& actCond= compileStorage->ConditionalStack.back();

    // patch Q-Jump to command after T-Jump
    prg.at(actCond.QJumpPos).Operation.Distance= prg.length() - actCond.QJumpPos;

    // store T-Jump address on conditional stack
    actCond.TJumpPos= prg.actPC();
}

void    Program::AssembleCondFinalize_F( integer idxInOriginal, integer idxInNormalized )
{
    if( compileStorage == nullptr )
        return;

    Assembly prg( compileStorage->Assembly, compileStorage->ResultStack);
    ASSERT_ASSEMBLE

    auto& actCond= compileStorage->ConditionalStack.back();

    // patch result stack position of T one back (in Finalize_T we had increased it by one to
    // point to the jump command, to protect it from being deleted with an lhs-delete
    --prg.lhsResultPC();

    prg.at( actCond.TJumpPos ).Operation.Distance= prg.length() - actCond.TJumpPos;


    // needs type alignment?
    if( !prg.at( prg.lhsResultPC() ).ResultType.IsSameType( prg.at( prg.resultPC() ).ResultType )    )
    {
        collectArgs(2);
        String condOp= A_CHAR("Q?T:F");
        CompilerPlugin::CIAutoCast ciAutoCast( *expression.ctScope,
                                               compileStorage->ConditionalStack.get_allocator().allocator,
                                               condOp,
                                               prg.at( prg.lhsResultPC() ).IsConstant(),
                                               prg.at( prg.   resultPC() ).IsConstant()   );
        bool found= false;
        try
        {
            for( auto& ppp : compiler.plugins )
                if( ppp.plugin->TryCompilation( ciAutoCast ) )
                {
                    if( !ciAutoCast.TypeOrValue.IsType<void>() )
                    {
                        // const cast upgrade for T?
                        if(  ciAutoCast.Callback == nullptr )
                        {
                            // change constant value conversion and patch type in jump command
                            prg.at(prg.lhsResultPC()).Operation.Value=
                            prg.at(prg.lhsResultPC()).ResultType     = ciAutoCast.TypeOrValue;

                            ALIB_DBG( prg.at(prg.lhsResultPC()).DbgInfo.Plugin= ppp.plugin;    )
                        }

                        // upgrade function for T?
                        else if( ciAutoCast.Callback )
                        {
                            // jump one more (the other as well)
                            ++prg.at(actCond.QJumpPos).Operation.Distance;
                            ++prg.at(actCond.TJumpPos).Operation.Distance;

                            // insert conversion
                            ALIB_DBG( auto& newCmd= )
                            prg.insertAt( actCond.TJumpPos++, ciAutoCast.Callback, 1,
                                                              ciAutoCast.TypeOrValue,
                                                              ciAutoCast.ReverseCastFunctionName,
                                                              false,
                                                              idxInOriginal, idxInNormalized      );
                            ALIB_DBG( newCmd.DbgInfo.Callback= ciAutoCast.DbgCallbackName;
                                      newCmd.DbgInfo.Plugin= ppp.plugin;               )
                            ++prg.lhsResultPC();
                        }
                    }

                    // const cast upgrade for F?
                    if( !ciAutoCast.TypeOrValueRhs.IsType<void>() )
                    {
                        if( ciAutoCast.Callback == nullptr )
                        {
                            // change constant value
                            prg.act().Operation.Value=
                            prg.act().ResultType     =  ciAutoCast.TypeOrValueRhs;

                            ALIB_DBG( prg.act().DbgInfo.Callback= ciAutoCast.DbgCallbackNameRhs;
                                      prg.act().DbgInfo.Plugin= ppp.plugin;                    )
                        }

                        // upgrade function for T?
                        else
                        {
                            // insert conversion
                            prg.add( ciAutoCast.CallbackRhs, 1,
                                     ciAutoCast.TypeOrValueRhs,
                                     ciAutoCast.ReverseCastFunctionNameRhs, false,
                                     idxInOriginal, idxInNormalized                    );
                            ++prg.resultPC();
                            ++prg.at(actCond.TJumpPos).Operation.Distance;

                            ALIB_DBG( prg.act().DbgInfo.Callback= ciAutoCast.DbgCallbackNameRhs;
                                      prg.act().DbgInfo.Plugin= ppp.plugin;                    )
                        }
                    }

                   found= true;
                   break;
                }
        }
        catch( Exception& e )
        {
            if(    !HasBits(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough)
                && !e.Type().IsEnumType<Exceptions>()                                                    )
                e.Add( ALIB_CALLER_NULLED, Exceptions::ExceptionInPlugin, expression.Name() );
            e.Add    ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal  );
            throw;
        }
        catch( std::exception& stdException )
        {
            if( !HasBits(compiler.CfgCompilation, Compilation::PluginExceptionFallThrough) )
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
                         compiler.TypeName( * ciAutoCast.ArgsBegin   ),
                         compiler.TypeName( *(ciAutoCast.ArgsBegin+1))            );
            e.Add      ( ALIB_CALLER_NULLED, Exceptions::ExpressionInfo, expression.GetOriginalString(), idxInOriginal  );

            throw e;
        }
    }

    // was this a constant conditional to be optimized out?
    if( actCond.ConstFlags  )
    {
        // eliminate T?
        if( (actCond.ConstFlags & 1) == 0  )
        {
            prg.erase( actCond.QJumpPos,  actCond.TJumpPos + 1 );
        }

        // eliminate F?
        else
        {
            prg.erase( actCond.TJumpPos,  prg.actPC() + 1 );
            prg.erase( actCond.QJumpPos );
        }
    }
    else
        // mark last command as part of conditional. Otherwise constant F-terms become optimized
        prg.act().SetEndOfConditionalFlag();


    // clean the conditional stack
    compileStorage->ConditionalStack.pop_back();

    // remove results Q [? T : F]
    prg.popResultPC();
    prg.popResultPC();
    prg.resultPC()= prg.actPC();
}


void Program::AssembleFinalize()
{
    if( compileStorage == nullptr )
        return;

    ALIB_DBG(Assembly prg( compileStorage->Assembly, compileStorage->ResultStack); )
    ASSERT_ASSEMBLE

    ALIB_ASSERT_ERROR( compileStorage->ConditionalStack.size() == 0, "EXPR",
                       "Finalizing program, while conditional stack is of size {}.",
                       compileStorage->ConditionalStack.size()                             )
    ALIB_ASSERT_ERROR( compileStorage->ResultStack.size() == 1, "EXPR",
                       "Finalizing program, while result stack is of size {}.",
                       compileStorage->ResultStack.size()                                  )


    // copy the program from the temporary vector to a simple array, allocated with the
    // compile-time scope's allocator.
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    commandsCount= static_cast<integer>( compileStorage->Assembly.size() );
    commands= expression.ctScope->Allocator.AllocArray<VM::Command>( compileStorage->Assembly.size() );
    auto* cmd= commands;
    for( auto* it : compileStorage->Assembly )
        new ( cmd++ ) VM::Command(*it);

    compileStorage= nullptr;
    ALIB_WARNINGS_RESTORE
}

#undef ASSERT_ASSEMBLE
#undef DBG_SET_CALLBACK_INFO

}}}} // namespace [aworx::lib::expressions::detail]

