//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/expressions/expressions.prepro.hpp"

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Expressions;
    import   ALib.Expressions.Impl;
    import   ALib.Lang;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
#else
#   include "ALib.Lang.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.H"
#   include "ALib.Expressions.H"
#   include "ALib.Expressions.Impl.H"
#endif
//========================================== Implementation ========================================
namespace alib {  namespace expressions {


ExpressionVal::ExpressionVal( MonoAllocator&  ma,
                              const String&   sourceString,
                              Scope*          pCTScope           )
: allocator     ( ma )
, ctScope       ( pCTScope )
, name          ( nullptr )
, program       ( nullptr )
, originalString( ma, sourceString )          { normalizedString.SetBuffer(sourceString.Length()); }

ExpressionVal::~ExpressionVal() {
    allocator.DbgLock(false);
    if(program)
       delete static_cast<detail::Program*>(program);
    lang::Destruct(*ctScope);
}

String   ExpressionVal::Name() {
    if( name.IsNull() )
        name= EXPRESSIONS.GetResource("ANON_EXPR_NAME");
    return name;
}

alib::Box  ExpressionVal::ResultType()
{
    ALIB_ASSERT_ERROR( program, "EXPR", "Internal error: Expression without program" )
        return static_cast<detail::Program*>(program)->ResultType();
}

alib::Box  ExpressionVal::Evaluate( Scope& scope ) {
    ALIB_ASSERT_ERROR( program, "EXPR","Internal error: Expression without program" )
    ALIB_DBG( Ticks startTime; )

        Box result= static_cast<detail::Program*>(program)->Run( scope );


    ALIB_DBG( DbgLastEvaluationTime= startTime.Age(); )

    return result;
}


String     ExpressionVal::GetOptimizedString() {
    if( optimizedString.IsNull() )
        static_cast<detail::Program*>(program)->compiler.getOptimizedExpressionString(*this);
    return optimizedString;
}

integer   ExpressionVal::GetProgramLength()
{ return static_cast<detail::Program*>(program)->Length(); }

int       ExpressionVal::CtdOptimizations()
{ return static_cast<detail::Program*>(program)->CtdOptimizations(); }

#if ALIB_DEBUG
AString  DbgList(Expression expression)
{ return detail::VirtualMachine::DbgList(*static_cast<detail::Program*>(expression->GetProgram()));}
#endif

}} // namespace [alib::expressions]


#undef EOS
