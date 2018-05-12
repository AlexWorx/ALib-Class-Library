// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#include "alib/alib.hpp"
#include "alib/time/timepointbase.hpp"
#include "alib/strings/format/formatterpythonstyle.hpp"
#include "alib/strings/boxing/debug.hpp"
#include "alib/compatibility/std_vector.hpp"
#include "alib/compatibility/std_string.hpp"
#include "alib/compatibility/std_iostream.hpp"

#include "scope.hpp"
#include "compiler.hpp"
#include "alib/expressions/detail/parser.hpp"
#include "detail/ast.hpp"
#include "alib/expressions/detail/program.hpp"
#include "alib/expressions/plugins/elvisoperator.hpp"
#include "alib/expressions/plugins/autocast.hpp"
#include "alib/expressions/plugins/arithmetics.hpp"
#include "alib/expressions/plugins/math.hpp"
#include "alib/expressions/plugins/strings.hpp"
#include "alib/expressions/plugins/dateandtime.hpp"

namespace aworx { namespace lib { namespace expressions {

using namespace detail;

// #################################################################################################
// Scope constructor, Clear() (avoid an own compilation unit for now)
// #################################################################################################
Scope::Scope( SPFormatter& formatter )
: CTScope  (nullptr   )
, Memory   (256       )
, Formatter(formatter )
{}

void    Scope::Clear()
{
    Memory.Clear();
    for( auto resource : Resources )
        delete resource;
    Resources.clear();

    for( auto resource : NamedResources )
        delete resource.second;
    NamedResources.clear();
}


// #################################################################################################
// Constructor & Setup
// #################################################################################################
Compiler::Compiler()
{
    // create a clone of the default formatter.
    CfgFormatter.reset( lib::STRINGS.GetDefaultFormatter()->Clone() );

//! [DOX_ALIB_EXPRESSIONS_COMPILER_REGISTERING_TYPES]
AddType( nullptr         , "NONE"      );
AddType( Types::Boolean  , "Boolean"   );
AddType( Types::Integer  , "Integer"   );
AddType( Types::Float    , "Float"     );
AddType( Types::String   , "String"    );
AddType( Types::DateTime , "DateTime" );
AddType( Types::Duration , "Duration"  );
//! [DOX_ALIB_EXPRESSIONS_COMPILER_REGISTERING_TYPES]

    // load nested expression function descriptor
    LoadResourcedFunctionDescriptors( lib::EXPRESSIONS, ASTR("EF"), &CfgNestedExpressionFunction);
    CfgNestedExpressionThrowIdentifier= EXPRESSIONS.Get( ASTR("EFT") );
}

Compiler::~Compiler()
{
    namedExpressions.clear();
    if (parser)
        delete parser;
}

Scope* Compiler::getCompileTimeScope()
{
    return new Scope( CfgFormatter );
}

void Compiler::SetupDefaults()
{
    //------------- add default unary ops ----------
    if( EnumContains( CfgCompilation, Compilation::DefaultUnaryOperators ) )
    {
        auto& metaInfoUnaryOps= *EnumMetaData<expressions::DefaultUnaryOperators>::GetSingleton();

        for( size_t i= 1; i < 6 ; i++ )
            AddUnaryOperator( std::get<1>(*metaInfoUnaryOps.Get(DefaultUnaryOperators(i))));

        // default unary op aliases
        if( EnumContains( CfgCompilation, Compilation::DefaultAlphabeticOperatorAliases ) )
        {
            auto& metaInfoUnaryOpAliases= *EnumMetaData<expressions::DefaultAlphabeticUnaryOperatorAliases>::GetSingleton();

            // Not -> !
            auto mi= metaInfoUnaryOpAliases.Get( DefaultAlphabeticUnaryOperatorAliases::Not );
            if( std::get<1>( *mi ).IsNotEmpty() )
                AlphabeticUnaryOperatorAliases [std::get<1>( *mi )]= std::get<2>( *mi );
        }
    }

    //------------- add default binary ops ----------
    if( EnumContains( CfgCompilation, Compilation::DefaultBinaryOperators ) )
    {
        auto& metaInfoBinaryOps= *EnumMetaData<expressions::DefaultBinaryOperators>::GetSingleton();

        for( size_t i= EnumContains( CfgCompilation, Compilation::AllowSubscriptOperator) ? 1 : 2;
             i < 22 ; i++ )
        {
            auto metaData= metaInfoBinaryOps.Get(DefaultBinaryOperators(i));
            AddBinaryOperator( std::get<1>(*metaData),

                               // patch assign operator
                               std::get<2>( i != EnumValue( DefaultBinaryOperators::Assign )
                                            ? *metaData
                                            : EnumContains( CfgCompilation, Compilation::AliasEqualsOperatorWithAssignOperator )
                                               ? *metaInfoBinaryOps.Get(DefaultBinaryOperators::Equal )
                                               : *metaInfoBinaryOps.Get(DefaultBinaryOperators::Assign)  )
                              );
        }

        // default binary op aliases
        if( EnumContains( CfgCompilation, Compilation::DefaultAlphabeticOperatorAliases ) )
        {
            auto& metaInfoUnaryOpAliases= *EnumMetaData<expressions::DefaultAlphabeticBinaryOperatorAliases>::GetSingleton();
            for( int i= 0 ; i < 14 ; ++i )
            {
                auto mi= metaInfoUnaryOpAliases.Get( DefaultAlphabeticBinaryOperatorAliases(i) );
                if( std::get<1>( *mi ).IsNotEmpty() )
                    AlphabeticBinaryOperatorAliases [std::get<1>( *mi )]= std::get<2>( *mi );
            }
        }
    }


    //------------- add default plug-ins ----------
    CfgNormalizationDisallowed.emplace_back( "--" );
    CfgNormalizationDisallowed.emplace_back( "++" );

    if( EnumContains( CfgBuiltInPlugins, BuiltInPlugins::ElvisOperator     ) )
        InsertPlugin( new plugins::ElvisOperator(*this),
                      CompilePriorities::ElvisOperator,
                      Responsibility::Transfer );

    if( EnumContains( CfgBuiltInPlugins, BuiltInPlugins::AutoCast          ) )
        InsertPlugin( new plugins::AutoCast(*this),
                      CompilePriorities::AutoCast,
                      Responsibility::Transfer );

    if( EnumContains( CfgBuiltInPlugins, BuiltInPlugins::Arithmetics       ) )
        InsertPlugin( new plugins::Arithmetics(*this),
                      CompilePriorities::Arithmetics,
                      Responsibility::Transfer );

    if( EnumContains( CfgBuiltInPlugins, BuiltInPlugins::Math              ) )
        InsertPlugin( new plugins::Math(*this),
                      CompilePriorities::Math,
                      Responsibility::Transfer );

    if( EnumContains( CfgBuiltInPlugins, BuiltInPlugins::Strings           ) )
        InsertPlugin( new plugins::Strings(*this),
                      CompilePriorities::Strings,
                      Responsibility::Transfer );

    if( EnumContains( CfgBuiltInPlugins, BuiltInPlugins::DateAndTime       ) )
        InsertPlugin( new plugins::DateAndTime(*this),
                      CompilePriorities::DateAndTime,
                      Responsibility::Transfer );
}

// #############################################################################################
// Parse and compile
// #############################################################################################
SPExpression   Compiler::Compile( const String& expressionString   )
{
    // checks
    ALIB_ASSERT_ERROR( HasPlugins(), "No plug-ins attached. Invoke SetupDefaults() on compiler instance." );
    ALIB_ASSERT_ERROR( expressionString.IsNotNull(), "Nulled expression string." );
    if( expressionString.IsEmpty() )
        throw Exception( ALIB_CALLER, Exceptions::EmptyExpressionString );


    Expression* expression=  new Expression( expressionString );

    // parse
    if (!parser)
        parser= Parser::Create( *this );

    AST*                 ast = nullptr;
    detail::Program* program = nullptr;
    try
    {
        ALIB_DBG( Ticks startTime; )
            ast= parser->Parse( expressionString, &CfgFormatter->DefaultNumberFormat );
        ALIB_DBG( expression->DbgParseTime= startTime.Age(); )

        ALIB_DBG( startTime= Ticks::Now(); )

        // optimize on AST level
        ast= ast->Optimize( CfgNormalization );

        // create program
        program            = new detail::Program( *this, plugins,  *expression, getCompileTimeScope());
        expression->program= program;

        // assemble
        ast->Assemble( *program, expression->normalizedString );
        expression->normalizedString.TrimEnd();
        delete ast;
        program->AssembleFinalize();
        ALIB_DBG( expression->DbgAssemblyTime= startTime.Age(); )

        // checks
        ALIB_ASSERT_ERROR( program->ResultType().IsNotNull(),
                           "No exception when parsing expression, but result type is nulled!" )

        ALIB_ASSERT_ERROR( expression->program->ResultType().IsNotNull(),
                           "No exception when parsing expression, but result type is nulled." )
    }
    catch( Exception& )
    {
        if( program && !expression->program )
            delete program;
        delete ast;
        delete expression;
        throw;
    }
    catch( std::exception& )
    {
        if( program && !expression->program )
            delete program;
        delete ast;
        delete expression;
        throw;
    }
    return SPExpression(expression);
}

void      Compiler::getOptimizedExpressionString( Expression* expression )
{
    detail::AST* ast= nullptr;

    try
    {
        ast = detail::VirtualMachine::Decompile( *dynamic_cast<Program*>(expression->program) );
        detail::Program* program= new detail::Program( *this, plugins,  *expression, nullptr );
        ast->Assemble( *program, expression->optimizedString );
        program->AssembleFinalize();
        delete program;
        delete ast;
    }
    catch(Exception& e)
    {
        delete ast;
        throw e;
    }
}


// #############################################################################################
// Manage named expressions
// #############################################################################################

bool           Compiler::AddNamed( const String& identifier, const String& expressionString )
{
    String128 key;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( key, ReplaceExternalBuffer )
    key << identifier;
    if( !EnumContains(CfgCompilation, Compilation::CaseSensitiveNamedExpressions ) )
        key.ToUpper();


    auto it=        namedExpressions.find( key );
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
    compiledExpression->name= identifier;
    ALIB_ASSERT( compiledExpression )
    if( existed )
        it->second= compiledExpression;
    else
        namedExpressions.emplace( key, compiledExpression );
    return existed;
}

SPExpression   Compiler::GetNamed( const String& identifier )
{
    // search
    String128 key;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( key, ReplaceExternalBuffer )
    key << identifier;
    if( !EnumContains(CfgCompilation, Compilation::CaseSensitiveNamedExpressions ) )
        key.ToUpper();

    auto it= namedExpressions.find( key );
    if( it != namedExpressions.end() )
        return it->second;

    // not found! -> use protected, virtual method to get the string from somewhere
    AString expressionString;
    if (!getExpressionString( identifier, expressionString ) )
        throw Exception( ALIB_CALLER_NULLED, Exceptions::NamedExpressionNotFound,
                         identifier );

    // Got an expression string! -> Compile
    SPExpression  parsedExpression= Compile( expressionString );

    parsedExpression->name= identifier;

    SPExpression sharedExpression( parsedExpression );
    namedExpressions.insert( std::make_pair( key, sharedExpression) );

    return sharedExpression;
}

#if ALIB_MODULE_CONFIGURATION
int Compiler::StoreLoadedExpressions( Priorities slot )
{
    int count= 0;
    for( auto& entry : VariablesLoaded )
        if( std::get<0>( entry ) == slot )
        {
            count++;
            var.Declare( std::get<1>( entry ), std::get<2>( entry ) );
            var.Priority= slot;
            var.Add( GetNamed( std::get<3>( entry ))->GetNormalizedString() );
            Config->Store( var );
        }

    return count;
}
#endif // ALIB_MODULE_CONFIGURATION

bool Compiler::getExpressionString( const String& identifier, AString& target )
{
    #if ALIB_MODULE_CONFIGURATION

        if( Config == nullptr )
            return false;

        // search in given default categories first.
        for( auto& category : DefaultCategories )
        {
            if(  Config->Load( var.Declare( category, identifier ) ) != Priorities::NONE )
            {
                target << var.GetString();
                VariablesLoaded.emplace_back( var.Priority, category, identifier, identifier );
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
                VariablesLoaded.emplace_back( var.Priority, category, name, identifier );
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
    #endif // ALIB_MODULE_CONFIGURATION
}


// #################################################################################################
// Helpers
// #################################################################################################

void Compiler::AddType( Type sample, const NString& name )
{
    ALIB_ASSERT_ERROR( types.find( sample.GetTypeInfo<0>() ) == types.end(),
                       "Type already registered with compiler"                      );
    types[ sample.GetTypeInfo<0>() ]= name;
}

void Compiler::WriteFunctionSignature( ArgIterator begin, ArgIterator end, AString& target )
{
    bool variadic= begin < end && (*(end-1)).IsNull();
    if( variadic )
        end--;

    target<< '(';
    bool first= true;
    while( begin != end )
    {
        if(!first)
            target<< ", ";
        first= false;
        target << TypeName( *begin );
        ++begin;
    }
    if(variadic)
    {
        if(!first)
            target<< ", ";
        target<< "...";
    }

    target<< ')';

}


        }}} // namespace [aworx::lib::expressions]

