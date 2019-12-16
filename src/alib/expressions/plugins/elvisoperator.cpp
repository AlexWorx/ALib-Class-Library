// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_ELVIS)
#   include "alib/expressions/plugins/elvisoperator.hpp"
#endif
#endif // !defined(ALIB_DOX)


//! @cond NO_DOX

namespace aworx { namespace lib { namespace expressions { namespace plugins {

ElvisOperator::ElvisOperator( Compiler& compiler )
: CompilerPlugin("ALib Elvis", compiler )
{}

namespace {

Box elvis( Scope&, ArgIterator  arg, ArgIterator )
{
    return (*arg).Call<FIsTrue>() ? *arg : *(arg + 1);
}

} // anonymous namespace

bool ElvisOperator::TryCompilation( CIBinaryOp&   ciBinaryOp    )
{
    Box& lhs= * ciBinaryOp.ArgsBegin;
    Box& rhs= *(ciBinaryOp.ArgsBegin + 1);

    // not Elvis operator  A_CHAR("A ?: B") ?
    if( ciBinaryOp.Operator != A_CHAR("?:") || !lhs.IsSameType(rhs) )
        return false;

    ALIB_DBG( ciBinaryOp.DbgCallbackName = "elvis";      )

    // constant A?
    if( ciBinaryOp.LhsIsConst )
    {
        if( lhs.Call<FIsTrue>() )
            ciBinaryOp.TypeOrValue= lhs;
        else
        {
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

}}}} // namespace [aworx::lib::expressions::detail]

//! @endcond
