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
    module ALib.Expressions.Impl;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
#else
#   include "ALib.Characters.Functions.H"
#   include "ALib.Strings.H"
#   include "ALib.Expressions.Impl.H"
#endif
//========================================== Implementation ========================================
//! @cond NO_DOX
namespace alib {  namespace expressions { namespace plugins {

ElvisOperator::ElvisOperator( Compiler& compiler )
: CompilerPlugin("ALib Elvis", compiler, CompilePriorities::ElvisOperator )                       {}

namespace {

Box elvis( Scope&, ArgIterator  arg, ArgIterator )
{ return (*arg).Call<FIsTrue>() ? *arg : *(arg + 1); }

} // anonymous namespace

bool ElvisOperator::TryCompilation( CIBinaryOp&   ciBinaryOp    ) {
    Box& lhs= * ciBinaryOp.ArgsBegin;
    Box& rhs= *(ciBinaryOp.ArgsBegin + 1);

    // not Elvis operator  A_CHAR("A ?: B") ?
    if( ciBinaryOp.Operator != A_CHAR("?:") || !lhs.IsSameType(rhs) )
        return false;

    ALIB_DBG( ciBinaryOp.DbgCallbackName = "elvis";      )

    // constant A?
    if( ciBinaryOp.LhsIsConst ) {
        if( lhs.Call<FIsTrue>() )
            ciBinaryOp.TypeOrValue= lhs;
        else {
            // constant B?
            if( ciBinaryOp.RhsIsConst )
                ciBinaryOp.TypeOrValue= rhs;
            else
                ciBinaryOp.NonConstArgIsResult= true;
        }
        return true;
    }

    // non constant A
    ciBinaryOp.Callback   = elvis;
    ciBinaryOp.TypeOrValue= rhs;

    return true;
}

}}} // namespace [alib::expressions::detail]
//! @endcond
