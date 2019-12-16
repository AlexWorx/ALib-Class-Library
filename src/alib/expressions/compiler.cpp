// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_EXPRESSIONS_COMPILER)
#   include "alib/expressions/compiler.hpp"
#endif
#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PARSER)
#   include "alib/expressions/detail/parser.hpp"
#endif
#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_AST)
#   include "alib/expressions/detail/ast.hpp"
#endif
#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM)
#   include "alib/expressions/detail/program.hpp"
#endif
#if !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_ELVIS)
#   include "alib/expressions/plugins/elvisoperator.hpp"
#endif
#if !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_AUTOCAST)
#   include "alib/expressions/plugins/autocast.hpp"
#endif
#if !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_ARITHMETICS)
#   include "alib/expressions/plugins/arithmetics.hpp"
#endif
#if !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_MATH)
#   include "alib/expressions/plugins/math.hpp"
#endif
#if !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_STRINGS)
#   include "alib/expressions/plugins/strings.hpp"
#endif
#if ALIB_SYSTEM && !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_DATEANDTIME)
#   include "alib/expressions/plugins/dateandtime.hpp"
#endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace expressions {

using namespace detail;

// #################################################################################################
// Scope constructor, Clear() (avoid an own compilation unit for now)
// #################################################################################################
Scope::Scope( SPFormatter& formatter )
: Allocator        (  4 * 1024  )
, CTScope          (  nullptr   )
, NestedExpressions(  Allocator )
, Resources        (  Allocator )
, NamedResources   ( &Allocator )
, Formatter        (  formatter )
{
    #if ALIB_DEBUG_MONOMEM
        Allocator.LogDomain= "MA/EXPR/SCP";
    #endif
}

void    Scope::Reset()
{
    Stack.clear();
    auto nestedExpressionsSize= NestedExpressions.size();
    auto         ResourcesSize=         Resources.size();
    auto    NamedResourcesSize=    NamedResources.Size();


    for( auto* resource : Resources )
        delete resource;

    for( auto resource : NamedResources )
        delete resource.second;

    NamedResources.Reset();

    Allocator.Reset();
    new (&NestedExpressions) std::vector<Expression*, StdContMA<Expression*>>( StdContMA<Expression*>(Allocator) );
    new (&Resources        ) std::vector<Expression*, StdContMA<Expression*>>( StdContMA<Expression*>(Allocator) );

    // reserve storage of previous size for the next run.
    NestedExpressions.reserve(nestedExpressionsSize);
            Resources.reserve(        ResourcesSize);
       NamedResources.Reserve(   NamedResourcesSize);
}


// #################################################################################################
// Constructor & Setup
// #################################################################################################
Compiler::Compiler()
: allocator                      ( 4 * 1024   )
, typeMap                        ( &allocator, 2.0, 5.0) // we don't care about speed here, just for log output
, namedExpressions               ( &allocator )
, UnaryOperators                 ( &allocator )
, AlphabeticUnaryOperatorAliases ( &allocator )
, AlphabeticBinaryOperatorAliases( &allocator )
, BinaryOperators                ( &allocator )
{
    #if ALIB_DEBUG_MONOMEM
        allocator.LogDomain= "MA/EXPR/CMPLR";
    #endif

    // create a clone of the default formatter.
    CfgFormatter.reset( Formatter::GetDefault()->Clone() );

    // register compiler types
    ALIB_CPP14_CONSTEXPR std::pair<Box&,NString> typeKeys[]=
    {
       { Types::Void     , "T_VOID"  },
       { Types::Boolean  , "T_BOOL"  },
       { Types::Integer  , "T_INT"   },
       { Types::Float    , "T_FLOAT" },
       { Types::String   , "T_STR"   },
     ALIB_IF_SYSTEM(
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
    Token::LoadResourcedTokens( EXPRESSIONS, "EF", &CfgNestedExpressionFunction ALIB_DBG(,1));
    CfgNestedExpressionThrowIdentifier= EXPRESSIONS.GetResource( "EFT" );
}

Compiler::~Compiler()
{
    ALIB_ASSERT_ERROR( CfgFormatter.get()->CountAcquirements() == 0,
                       "The formatter of this compiler was not released properly" )
}

Scope* Compiler::getCompileTimeScope()
{
    auto* result= new Scope( CfgFormatter );
    #if ALIB_DEBUG_MONOMEM
        result->Allocator.LogDomain= "MA/EXPR/CTSCP";
    #endif
    return result;
}

void Compiler::SetupDefaults()
{
    //------------- add default unary ops ----------
    if( HasBits( CfgCompilation, Compilation::DefaultUnaryOperators ) )
    {
        auto enumRecordIt= EnumRecords<DefaultUnaryOperators>::begin();
        ALIB_ASSERT_ERROR( enumRecordIt.Enum() == DefaultUnaryOperators::NONE,
                           "Expected none-operator as first enum record" )
        while( ++enumRecordIt != EnumRecords<DefaultUnaryOperators>::end() )
            AddUnaryOperator( enumRecordIt->EnumElementName );

        // default unary op aliases
        if( HasBits( CfgCompilation, Compilation::DefaultAlphabeticOperatorAliases ) )
        {
            // Not -> !
            auto& record= enums::GetRecord( DefaultAlphabeticUnaryOperatorAliases::Not );
            if( record.Symbol.IsNotEmpty() )
                AlphabeticUnaryOperatorAliases.EmplaceOrAssign( record.Symbol,
                                                                record.Replacement );
        }
    }

    //------------- add default binary ops ----------
    if( HasBits( CfgCompilation, Compilation::DefaultBinaryOperators ) )
    {
        auto enumRecordIt= EnumRecords<DefaultBinaryOperators>::begin();
        ALIB_ASSERT_ERROR( enumRecordIt.Enum() == DefaultBinaryOperators::NONE,
                           "Expected none-operator as first enum record" )
        while( ++enumRecordIt != EnumRecords<DefaultBinaryOperators>::end() )
        {
            // get symbol
            if( enumRecordIt->Symbol.Equals( A_CHAR( "[]") )
                && !HasBits( CfgCompilation, Compilation::AllowSubscriptOperator) )
                continue;

            // use equal operator's precedence for assign operator, if aliased.
            auto precedence = ( enumRecordIt.Enum() == DefaultBinaryOperators::Assign
                                 && HasBits(CfgCompilation, Compilation::AliasEqualsOperatorWithAssignOperator) )
                               ? enums::GetRecord(DefaultBinaryOperators::Equal).Precedence
                               : enumRecordIt->Precedence;

            AddBinaryOperator( enumRecordIt->Symbol, precedence );
        }

        // default binary op aliases
        if( HasBits( CfgCompilation, Compilation::DefaultAlphabeticOperatorAliases ) )
            for( auto& enumEntry : EnumRecords<DefaultAlphabeticBinaryOperatorAliases>() )
                AlphabeticBinaryOperatorAliases.EmplaceOrAssign( enumEntry.Symbol,
                                                                 enumEntry.Replacement );
        }


    //------------- add default plug-ins ----------
    CfgNormalizationDisallowed.emplace_back( "--" );
    CfgNormalizationDisallowed.emplace_back( "++" );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::ElvisOperator     ) )
        InsertPlugin( new plugins::ElvisOperator(*this),
                      CompilePriorities::ElvisOperator,
                      Responsibility::Transfer );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::AutoCast          ) )
        InsertPlugin( new plugins::AutoCast(*this),
                      CompilePriorities::AutoCast,
                      Responsibility::Transfer );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::Arithmetics       ) )
        InsertPlugin( new plugins::Arithmetics(*this),
                      CompilePriorities::Arithmetics,
                      Responsibility::Transfer );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::Math              ) )
        InsertPlugin( new plugins::Math(*this),
                      CompilePriorities::Math,
                      Responsibility::Transfer );

    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::Strings           ) )
        InsertPlugin( new plugins::Strings(*this),
                      CompilePriorities::Strings,
                      Responsibility::Transfer );

ALIB_IF_SYSTEM(
    if( HasBits( CfgBuiltInPlugins, BuiltInPlugins::DateAndTime       ) )
        InsertPlugin( new plugins::DateAndTime(*this),
                      CompilePriorities::DateAndTime,
                      Responsibility::Transfer );                              )
}

// #############################################################################################
// Parse and compile
// #############################################################################################
SPExpression   Compiler::Compile( const String& expressionString   )
{
    // checks
    ALIB_ASSERT_ERROR( HasPlugins(), "No plug-ins attached. Invoke SetupDefaults() on compiler instance." )
    ALIB_ASSERT_ERROR( expressionString.IsNotNull(), "Nulled expression string." )
    if( expressionString.IsEmpty() )
        throw Exception( ALIB_CALLER, Exceptions::EmptyExpressionString );


    Expression* expression=  new Expression( expressionString, getCompileTimeScope() );

    // parser
    if (parser == nullptr)
        parser= Parser::Create( *this );

    AST*                 ast = nullptr;

    // prevent cleaning memory with recursive compilation (may happen with nested expressions)
    static integer recursionCounter= 0;
    MonoAllocator::Snapshot startOfCompilation;
    if( recursionCounter++ == 0 )
        startOfCompilation= allocator.TakeSnapshot();

    try
    {
        #if ALIB_TIME && ALIB_DEBUG
            Ticks startTime;
        #endif


        // parse
        ast= parser->Parse( expressionString, &CfgFormatter->DefaultNumberFormat, &allocator );

        #if ALIB_TIME && ALIB_DEBUG
            expression->DbgParseTime= startTime.Age();
            startTime= Ticks::Now();
        #endif

        // optimize on AST level
        ast= ast->Optimize( CfgNormalization );

        // create program
        expression->program= new detail::Program( *this, *expression, &allocator );

        // assemble
        ast->Assemble( *expression->program, allocator, expression->normalizedString );
        expression->normalizedString.TrimEnd();

        expression->program->AssembleFinalize();
        #if ALIB_TIME && ALIB_DEBUG
            expression->DbgAssemblyTime= startTime.Age();
        #endif

        if(--recursionCounter == 0)
            allocator.Reset( startOfCompilation );

        // checks
        ALIB_ASSERT_ERROR( !expression->program->ResultType().IsType<void>(),
                           "No exception when parsing expression, but result type is void!" )

        ALIB_ASSERT_ERROR( !expression->program->ResultType().IsType<void>(),
                           "No exception when parsing expression, but result type is void." )
    }
    catch( Exception& )
    {
        delete expression;
        if(--recursionCounter == 0)
            allocator.Reset( startOfCompilation );
        throw;
    }
    catch( std::exception& )
    {
        delete expression;
        if(--recursionCounter == 0)
            allocator.Reset( startOfCompilation );
        throw;
    }
    return SPExpression(expression);
}

void      Compiler::getOptimizedExpressionString( Expression* expression )
{
    detail::AST* ast= nullptr;
    auto startOfDecompilation= allocator.TakeSnapshot();
    try
    {
        ast = detail::VirtualMachine::Decompile( *dynamic_cast<Program*>(expression->program), allocator );
        detail::Program* program= allocator.Emplace<detail::Program>( *this, *expression, nullptr );
        ast->Assemble( *program, allocator, expression->optimizedString );
        program->AssembleFinalize();
        monomem::Destruct(program);
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
            namedExpressions.Erase( it );
            return true;
        }
        return false;
    }

    auto compiledExpression= Compile( expressionString );
    compiledExpression->name= compiledExpression->ctScope->Allocator.EmplaceString( name );
    ALIB_ASSERT( compiledExpression )
    if( existed )
        it.Mapped()= compiledExpression;
    else
        namedExpressions.EmplaceUnique( key, compiledExpression );
    return existed;
}

SPExpression   Compiler::GetNamed( const String& name )
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
    if (!getExpressionString( name, expressionString ) )
        throw Exception( ALIB_CALLER_NULLED, Exceptions::NamedExpressionNotFound, name );

    // Got an expression string! -> Compile
    SPExpression  parsedExpression= Compile( expressionString );

    parsedExpression->name= parsedExpression->ctScope->Allocator.EmplaceString( name );

    SPExpression sharedExpression( parsedExpression );
    namedExpressions.EmplaceUnique( key, sharedExpression );

    return sharedExpression;
}

#if ALIB_CONFIGURATION
int Compiler::StoreLoadedExpressions( Priorities slot )
{
    int count= 0;
    for( auto& entry : VariablesLoaded )
        if( std::get<0>( entry ) == slot )
        {
            ++count;
            var.Declare( std::get<1>( entry ), std::get<2>( entry ) );
            var.SetPriority( slot );
            var.Add( GetNamed( std::get<3>( entry ))->GetNormalizedString() );
            Config->Store( var );
        }

    return count;
}
#endif // ALIB_CONFIGURATION

bool Compiler::getExpressionString( const String& identifier, AString& target )
{
    #if ALIB_CONFIGURATION

        if( Config == nullptr )
            return false;

        // search in given default categories first.
        for( auto& category : DefaultCategories )
        {
            if(  Config->Load( var.Declare( category, identifier ) ) != Priorities::NONE )
            {
                target << var.GetString();
                VariablesLoaded.emplace_back( var.Priority(), category, identifier, identifier );
                return true;
            }
        }

        // try to split identifier in "category_name"
        integer underscorePos=  identifier.IndexOf( '_', 1 );
        while( underscorePos > 0 )
        {
            Substring name;
            Substring category(identifier);
            category.Split( underscorePos, name, 1 );
            if(  Config->Load( var.Declare( category, name ) ) != Priorities::NONE )
            {
                target << var.GetString();
                VariablesLoaded.emplace_back( var.Priority(), category, name, identifier );
                return true;
            }
            underscorePos=  identifier.IndexOf( '_', underscorePos + 1 );
        }

        // failed
        return false;
    #else
        (void) identifier;
        (void) target;
        return false;
    #endif // ALIB_CONFIGURATION
}


// #################################################################################################
// Helpers
// #################################################################################################

void    Compiler::AddType( Type sample, const NString& name )
{
    ALIB_DBG( auto it= )
    typeMap.EmplaceIfNotExistent( &sample.TypeID(), name );
    ALIB_ASSERT_ERROR( it.second  == true, // is insert
                       "Type already registered with compiler."  )
}

NString Compiler::TypeName(Type box)
{
    if( box.IsType<void>() )
        return "NONE";

    auto entry= typeMap.Find( &box.TypeID() );
    ALIB_ASSERT_WARNING( entry != typeMap.end(),
                         "Custom type {!Q} not registered. Please use Compiler::AddType to do so.",
                         box.TypeID() )
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
    buf.reserve( static_cast<size_t>(end - begin) );
    while( begin != end )
        buf.emplace_back( &*begin++ );
    WriteFunctionSignature( buf.data(), buf.size(), target );
}



}}} // namespace [aworx::lib::expressions]

