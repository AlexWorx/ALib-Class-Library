// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/strings/strings.prepro.hpp"
#include "alib/expressions/expressions.prepro.hpp"
#include "ALib.Monomem.StdContainers.H"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Expressions;
    import   ALib.Expressions.Impl;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
#else
#   include "ALib.Expressions.Impl.H"
#endif
// ======================================   Implementation   =======================================
namespace alib {  namespace expressions {

using namespace detail;

// #################################################################################################
// Scope constructor, Clear()
// #################################################################################################
Scope::Scope( SPFormatter& formatter)  // evaluation scope constructor using an own allocator
: EvalScopeAllocator(MonoAllocator::Create(ALIB_DBG("ExpressionScope",) 1, 200))
, Allocator         (*EvalScopeAllocator)
, Stack             (Allocator().New<StdVectorMono<Box>>(Allocator))
, Formatter         (formatter)
, NamedResources    (nullptr)
, EvalScopeVMMembers(Allocator().New<VMMembers>(Allocator))
#if ALIB_DEBUG_CRITICAL_SECTIONS
, DCS               ("ExpressionEvalScope")
#endif
{}

Scope::Scope( MonoAllocator& allocator, SPFormatter& formatter )
: EvalScopeAllocator(nullptr) // compile-time scope constructor using the allocator of the expression
, Allocator         (allocator)
, Stack             (Allocator().New<StdVectorMono<Box>>(Allocator))
, Formatter         (formatter)
, NamedResources    (Allocator().New< HashMap< MonoAllocator,
                                               NString,
                                               ScopeResource* >  >(Allocator) )
, EvalScopeVMMembers(nullptr)
#if ALIB_DEBUG_CRITICAL_SECTIONS
, DCS               ("ExpressionCTScope")
#endif
{}

Scope::~Scope()
{
    // Destruct members in vectors and tables.
    freeResources();
    lang::Destruct(*Stack);
    if ( EvalScopeAllocator )
    {
        lang::Destruct(*EvalScopeVMMembers);
        lang::Destruct(*EvalScopeAllocator);
    }
}


void    Scope::freeResources()
{
    Stack->clear();

    if ( NamedResources )
        for( auto& resource : *NamedResources )
            lang::Destruct(*resource.second);
}

void    Scope::Reset()
{
    ALIB_ASSERT( !IsCompileTime(), "EXPR" )
    
    // save sizes
    auto             stackSize=                       Stack->size();
    auto nestedExpressionsSize= EvalScopeVMMembers->NestedExpressions.size();

    // free and destruct
    freeResources();
    lang::Destruct(*EvalScopeVMMembers);
    lang::Destruct(*Stack);
    Allocator.Reset(sizeof(MonoAllocator), alignof(MonoAllocator));

    // create new
    Stack             = Allocator().New<StdVectorMono<Box>>(Allocator);
    EvalScopeVMMembers= Allocator().New<VMMembers>(Allocator);

    // reserve storage of the previous sizes for the next run.
                                   Stack->reserve(            stackSize );
    EvalScopeVMMembers->NestedExpressions.reserve(nestedExpressionsSize );
}


// #################################################################################################
// Constructor & Setup
// #################################################################################################
Compiler::Compiler()
: allocator                      (ALIB_DBG("ExpressionCompiler",) 4)
, typeMap                        (allocator, 2.0, 5.0) // we don't care about speed here, just for log output
, namedExpressions               (allocator)
, UnaryOperators                 (allocator)
, AlphabeticUnaryOperatorAliases (allocator)
, AlphabeticBinaryOperatorAliases(allocator)
, BinaryOperators                (allocator)
, CfgNormalizationDisallowed     (allocator)
{
    // create a clone of the default formatter.
    CfgFormatter= Formatter::Default->Clone();

    // register compiler types
    constexpr std::pair<Box&,NString> typeKeys[]=
    {
       { Types::Void     , "T_VOID"  },
       { Types::Boolean  , "T_BOOL"  },
       { Types::Integer  , "T_INT"   },
       { Types::Float    , "T_FLOAT" },
       { Types::String   , "T_STR"   },
     IF_ALIB_CAMP(
       { Types::DateTime , "T_DATE"  },
       { Types::Duration , "T_DUR"   }, )
    };

    size_t idx= sizeof( typeKeys ) / sizeof( std::pair<Box&, NString> );
    do
    {
        --idx;
        auto& name= EXPRESSIONS.GetResource( typeKeys[idx].second  );
        ALIB_STRINGS_TO_NARROW( name, nameNarrow, 128 )
        AddType( typeKeys[idx].first,  nameNarrow );
    }
    while( idx != 0 );

    // load nested expression function descriptor
    strings::util::LoadResourcedTokens( EXPRESSIONS, "EF", &CfgNestedExpressionFunction ALIB_DBG(,1));
    CfgNestedExpressionThrowIdentifier= EXPRESSIONS.GetResource( "EFT" );
}

Compiler::~Compiler()
{
    if( Repository != nullptr )
        delete Repository;
}

Scope* Compiler::createCompileTimeScope(MonoAllocator& ctAllocator)
{
    auto* result= ctAllocator().New<Scope>( ctAllocator, CfgFormatter );
    return result;
}
    
void Compiler::SetupDefaults()
{
    //------------- add default unary ops ----------
    if( HasBits( CfgCompilation, Compilation::DefaultUnaryOperators ) )
    {
        auto enumRecordIt= EnumRecords<DefaultUnaryOperators>::begin();
        ALIB_ASSERT_ERROR( enumRecordIt.Enum() == DefaultUnaryOperators::NONE, "EXPR",
                           "Expected none-operator as first enum record" )
        while( ++enumRecordIt != EnumRecords<DefaultUnaryOperators>::end() )
            AddUnaryOperator( enumRecordIt->EnumElementName );

        // default unary op aliases
        if( HasBits( CfgCompilation, Compilation::DefaultAlphabeticOperatorAliases ) )
        {
            // Not -> !
            auto& record= enumrecords::GetRecord( DefaultAlphabeticUnaryOperatorAliases::Not );
            if( record.Symbol.IsNotEmpty() )
                AlphabeticUnaryOperatorAliases.EmplaceOrAssign( record.Symbol,
                                                                record.Replacement );
        }
    }

    //------------- add default binary ops ----------
    if( HasBits( CfgCompilation, Compilation::DefaultBinaryOperators ) )
    {
        auto enumRecordIt= EnumRecords<DefaultBinaryOperators>::begin();
        ALIB_ASSERT_ERROR( enumRecordIt.Enum() == DefaultBinaryOperators::NONE, "EXPR",
                           "Expected none-operator as first enum record" )
        while( ++enumRecordIt != EnumRecords<DefaultBinaryOperators>::end() ) {
            // get symbol
            if (    enumRecordIt->Symbol.Equals<NC>(A_CHAR("[]"))
                 && !HasBits(CfgCompilation, Compilation::AllowSubscriptOperator))
                continue;

            // use equal operator's precedence for assign operator, if aliased.
            auto precedence = (    enumRecordIt.Enum() == DefaultBinaryOperators::Assign
                                && HasBits(CfgCompilation,
                                          Compilation::AliasEqualsOperatorWithAssignOperator))
                                  ? enumrecords::GetRecord(DefaultBinaryOperators::Equal).Precedence
                                  : enumRecordIt->Precedence;

            AddBinaryOperator(enumRecordIt->Symbol, precedence);
        }

        // default binary op aliases
        if( HasBits( CfgCompilation, Compilation::DefaultAlphabeticOperatorAliases ) )
            for( auto& enumEntry : EnumRecords<DefaultAlphabeticBinaryOperatorAliases>() )
                AlphabeticBinaryOperatorAliases.EmplaceOrAssign( enumEntry.Symbol,
                                                                 enumEntry.Replacement );
    }


    //------------- add default plug-ins ----------
    CfgNormalizationDisallowed.emplace_back( A_CHAR("--") );
    CfgNormalizationDisallowed.emplace_back( A_CHAR("++") );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::ElvisOperator     ) )
        InsertPlugin( new plugins::ElvisOperator(*this),  lang::Responsibility::Transfer );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::AutoCast          ) )
        InsertPlugin( new plugins::AutoCast(*this)     , lang::Responsibility::Transfer );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::Arithmetics       ) )
        InsertPlugin( new plugins::Arithmetics(*this) , lang::Responsibility::Transfer );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::Math              ) )
        InsertPlugin( new plugins::Math(*this)        , lang::Responsibility::Transfer );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::Strings           ) )
        InsertPlugin( new plugins::Strings(*this)     , lang::Responsibility::Transfer );

IF_ALIB_CAMP(
    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::DateAndTime       ) )
        InsertPlugin( new plugins::DateAndTime(*this) , lang::Responsibility::Transfer );        )
}

// #############################################################################################
// Parse and compile
// #############################################################################################
Expression   Compiler::Compile( const String& expressionString )
{
    // checks
    ALIB_ASSERT_ERROR( HasPlugins(), "EXPR",
                       "No plug-ins attached. Invoke SetupDefaults() on compiler instance." )
    ALIB_ASSERT_ERROR( expressionString.IsNotNull(), "EXPR", "Nulled expression string." )
    if( expressionString.IsEmpty() )
        throw Exception( ALIB_CALLER, Exceptions::EmptyExpressionString );


    Expression expression( 1, 100 );
    expression.ConstructT( expression.GetAllocator(),
                           expressionString,
                           createCompileTimeScope(expression.GetAllocator()) );

    // parser
    if (parser == nullptr)
        parser= Parser::Create( *this );

    AST*                 ast = nullptr;

    // prevent cleaning memory with recursive compilation (may happen with nested expressions)
    static integer recursionCounter= 0;
    monomem::Snapshot startOfCompilation;
    if( recursionCounter++ == 0 )
        startOfCompilation= allocator.TakeSnapshot();

    try
    {
        ALIB_DBG( Ticks startTime; )

        // parse
        ast= static_cast<AST*>(parser->Parse( expressionString, &CfgFormatter->DefaultNumberFormat ));

        ALIB_DBG(
            expression->DbgParseTime= startTime.Age();
            startTime= Ticks::Now();
        )

        // optimize on AST level
        ast= ast->Optimize( CfgNormalization );

        // create program
        expression->program= new detail::Program( *this, *expression, &allocator );

        // assemble
        ast->Assemble( *static_cast<detail::Program*>(expression->program), allocator, expression->normalizedString );
        expression->normalizedString.TrimEnd();

        static_cast<detail::Program*>(expression->program)->AssembleFinalize();
        ALIB_DBG( expression->DbgAssemblyTime= startTime.Age(); )

        if(--recursionCounter == 0)
            allocator.Reset( startOfCompilation );

        // checks
        ALIB_ASSERT_ERROR(
            !static_cast<detail::Program*>(expression->program)->ResultType().IsType<void>(),
            "EXPR", "No exception when parsing expression, but result type is void!" )

        ALIB_ASSERT_ERROR(
            !static_cast<detail::Program*>(expression->program)->ResultType().IsType<void>(),
            "EXPR", "No exception when parsing expression, but result type is void." )
    }
    catch( Exception& )
    {
        expression= nullptr;
        if(--recursionCounter == 0)
            allocator.Reset( startOfCompilation );
        throw;
    }
    catch( std::exception& )
    {
        expression= nullptr;
        if(--recursionCounter == 0)
            allocator.Reset( startOfCompilation );
        throw;
    }

    // Assert that nobody touches this allocator from now on. With evaluations,
    // exclusively the evaluation scope is to be used.
    expression->ctScope->Allocator.DbgLock(true);

    return Expression(expression);
}

void      Compiler::getOptimizedExpressionString( ExpressionVal& expression )
{
    detail::AST* ast= nullptr;
    auto startOfDecompilation= allocator.TakeSnapshot();
    try
    {
	    expression.allocator.DbgLock(false);
        ast                     = detail::VirtualMachine::Decompile( *static_cast<detail::Program*>(expression.program),
                                                                     allocator );
        detail::Program* program= allocator().New<detail::Program>( *this, expression, nullptr );
        ast->Assemble( *program, allocator, expression.optimizedString );
        program->AssembleFinalize();
        lang::Destruct(*program);
	    expression.allocator.DbgLock(true);
    }
    catch(Exception& )
    {
        allocator.Reset( startOfDecompilation );
        throw;
    }
    allocator.Reset( startOfDecompilation );
}


// #############################################################################################
// Manage named expressions
// #############################################################################################

bool           Compiler::AddNamed( const String& name, const String& expressionString )
{
    String128 key;
    key.DbgDisableBufferReplacementWarning();
    key << name;
    if( !HasBits(CfgCompilation, Compilation::CaseSensitiveNamedExpressions ) )
        key.ToUpper();

    auto it=        namedExpressions.Find( key );
    bool existed=   it != namedExpressions.end();

    // removal requested?
    if( expressionString.IsNull() )
    {
        if ( existed )
        {
            namedExpressions.erase( it );
            return true;
        }
        return false;
    }

    auto compiledExpression= Compile( expressionString );
    compiledExpression->allocator.DbgLock(false);
    compiledExpression->name.Allocate( compiledExpression->allocator, name);
    compiledExpression->allocator.DbgLock(true);
    ALIB_ASSERT( compiledExpression, "EXPR" )
    if( existed )
        it.Mapped()= compiledExpression;
    else
        namedExpressions.EmplaceUnique( key, compiledExpression );
    return existed;
}

Expression   Compiler::GetNamed( const String& name )
{
    // search
    String128 key;
    key.DbgDisableBufferReplacementWarning();
    key << name;
    if( !HasBits(CfgCompilation, Compilation::CaseSensitiveNamedExpressions ) )
        key.ToUpper();

    auto it= namedExpressions.Find( key );
    if( it != namedExpressions.end() )
        return it.Mapped();

    // not found! -> use protected, virtual method to get the string from somewhere
    AString expressionString;
    if (Repository == nullptr || !Repository->Get( name, expressionString ) )
        throw Exception( ALIB_CALLER_NULLED, Exceptions::NamedExpressionNotFound, name );

    // Got an expression string! -> Compile
    Expression  parsedExpression= Compile( expressionString );

    parsedExpression->allocator.DbgLock(false);
    parsedExpression->name.Allocate( parsedExpression->allocator, name );
    parsedExpression->allocator.DbgLock(true);

    Expression sharedExpression( parsedExpression );
    namedExpressions.EmplaceUnique( key, sharedExpression );

    return sharedExpression;
}

// #################################################################################################
// Helpers
// #################################################################################################
void    Compiler::AddType( Type sample, const NString& name )
{
    ALIB_DBG( auto it= )
    typeMap.EmplaceIfNotExistent( &sample.TypeID(), name );
    ALIB_ASSERT_ERROR( it.second == true, // is insert
                       "EXPR",  "Type already registered with compiler."  )
}

NString Compiler::TypeName(Type box)
{
    if( box.IsType<void>() )
        return "NONE";

    auto entry= typeMap.Find( &box.TypeID() );
    ALIB_ASSERT_WARNING( entry != typeMap.end(), "EXPR",
       "Custom type \"{}\" not registered. Please use Compiler::AddType to do so.", &box.TypeID() )
    if( entry == typeMap.end() )
        return "Unknown Type";

    return entry.Mapped();
}

void Compiler::WriteFunctionSignature( Box** boxArray,  size_t qty,  AString& target )
{
    bool variadic= qty && (*(boxArray + qty -1)) == nullptr;
    if( variadic )
        --qty;

    target<< '(';
    bool isFirst= true;
    for( size_t i= 0 ; i <  qty ; ++i )
    {
        if(!isFirst)
            target<< ", ";
        isFirst= false;
        target << '<' << TypeName( **boxArray++ ) << '>';
    }
    if(variadic)
    {
        if(!isFirst)
            target<< ", ";
        target<< "...";
    }

    target<< ')';
}

void Compiler::WriteFunctionSignature( ArgIterator  begin,
                                       ArgIterator  end,
                                       AString&     target  )
{
    std::vector<Box*> buf;
    buf.reserve( size_t(end - begin) );
    while( begin != end )
        buf.emplace_back( &*begin++ );
    WriteFunctionSignature( buf.data(), buf.size(), target );
}



}} // namespace [alib::expressions]
