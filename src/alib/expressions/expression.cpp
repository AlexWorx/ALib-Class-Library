// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/expressions/expression.hpp"
#   include "alib/expressions/detail/program.hpp"
#   include "alib/expressions/scope.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#endif // !DOXYGEN


namespace alib {  namespace expressions {


ExpressionVal::ExpressionVal( MonoAllocator&  ma,
                        const String&   sourceString,
                        Scope*          pCTScope           )
: allocator     ( ma )
, ctScope       ( pCTScope )
, name          ( nullptr )
, program       ( nullptr )
, originalString( ma, sourceString )
{
    normalizedString.SetBuffer(sourceString.Length());
}

ExpressionVal::~ExpressionVal()
{
    allocator.DbgLock(false);
    if(program)
       delete program;
    lang::Destruct(*ctScope);
}

String   ExpressionVal::Name()
{
    if( name.IsNull() )
        name= EXPRESSIONS.GetResource("ANON_EXPR_NAME");
    return name;
}

alib::Box  ExpressionVal::ResultType()
{
    ALIB_ASSERT_ERROR( program, "EXPR", "Internal error: Expression without program" )
        return program->ResultType();
}

alib::Box  ExpressionVal::Evaluate( Scope& scope )
{
    ALIB_ASSERT_ERROR( program, "EXPR","Internal error: Expression without program" )
    #if ALIB_TIME && ALIB_DEBUG
        Ticks startTime;
    #endif

        Box result= program->Run( scope );

    #if ALIB_TIME && ALIB_DEBUG
        DbgLastEvaluationTime= startTime.Age();
    #endif

    return result;
}


String     ExpressionVal::GetOptimizedString()
{
    if( optimizedString.IsNull() )
        dynamic_cast<detail::Program*>( program )->compiler.getOptimizedExpressionString(*this);
    return optimizedString;
}


}} // namespace [alib::expressions]


#undef EOS
