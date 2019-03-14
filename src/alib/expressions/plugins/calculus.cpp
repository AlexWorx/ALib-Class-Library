// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
#   include "alib/expressions/plugins/calculus.hpp"
#endif


namespace aworx { namespace lib { namespace expressions {  namespace plugins {

constexpr Calculus::CTInvokable Calculus::CTI;
constexpr Calculus::CTInvokable Calculus::ETI;

// #################################################################################################
// Identifiers operators
// #################################################################################################
bool Calculus::TryCompilation( CIFunction& ciFunction )
{
    String& name= ciFunction.Name;

    // search in constant identifiers
    if( ciFunction.QtyArgs() == 0 )
    {
        for( auto& entry : ConstantIdentifiers )
        {
            if ( entry.Descriptor.Match( name ) )
            {
                // check for wrong parentheses
                if(     ciFunction.NoParentheses
                    && !EnumContains(Cmplr.CfgCompilation, Compilation::AllowOmittingParenthesesOfParameterlessFunctions ) )
                    throw Exception( ALIB_CALLER_NULLED, Exceptions::MissingFunctionParentheses,
                                     entry.Descriptor );

                if(    !ciFunction.NoParentheses
                    && !EnumContains(Cmplr.CfgCompilation, Compilation::AllowEmptyParenthesesForIdentifierFunctions ) )
                    throw Exception( ALIB_CALLER_NULLED, Exceptions::IdentifierWithFunctionParentheses,
                                     entry.Descriptor );


                // accept
                ciFunction.Name.Reset( entry.Descriptor );
                ciFunction.TypeOrValue= entry.Result;
                return true;
            }
        }
    }

    // search in functions
    for( auto& entry : Functions )
    {
        if( entry.Descriptor.Match( name ) )
        {
            // collect information about given and requested parameters
            size_t qtyGiven      =  ciFunction.QtyArgs();
            size_t qtyRequired   =  entry.SignatureLength;
            bool   isVariadic    =  false;
            if( entry.SignatureLength > 0 &&  (   entry.Signature[entry.SignatureLength - 1] == nullptr
                                               || entry.Signature[entry.SignatureLength - 1]->IsType<void>() ) )
            {
                isVariadic= true;
                qtyRequired--;
            }

            size_t qtyShared     = (std::min)( qtyGiven, qtyRequired );
            bool    sharedAreSameType = true;
            for( size_t i= 0;  i != qtyShared ; ++i )
                sharedAreSameType&=   ciFunction.Arg(i).IsSameType( *entry.Signature[i] );

            // check if given parameter don't match
            if(    !sharedAreSameType
                || ( isVariadic ? qtyGiven <  qtyRequired
                                : qtyGiven != qtyRequired )         )
            {
                ciFunction.FunctionsWithNonMatchingArguments.emplace_back( entry.Descriptor );
                if( qtyRequired )
                    Cmplr.WriteFunctionSignature( entry.Signature,
                                                  entry.SignatureLength,
                                                  ciFunction.FunctionsWithNonMatchingArguments.back() );
                // search next
                continue;
            }

            // check for wrong parentheses
            if(     ciFunction.NoParentheses
                &&  entry.Signature != nullptr
                && !EnumContains(Cmplr.CfgCompilation, Compilation::AllowOmittingParenthesesOfParameterlessFunctions ) )
                throw Exception( ALIB_CALLER_NULLED, Exceptions::MissingFunctionParentheses,
                                 entry.Descriptor );

            if(    !ciFunction.NoParentheses
                &&  entry.Signature == nullptr
                && !EnumContains(Cmplr.CfgCompilation, Compilation::AllowEmptyParenthesesForIdentifierFunctions ) )
                throw Exception( ALIB_CALLER_NULLED, Exceptions::IdentifierWithFunctionParentheses,
                                 entry.Descriptor );



            // accept
            ciFunction.Name.Reset( entry.Descriptor );

            if( !entry.Callback )
            {
                ciFunction.TypeOrValue      = *entry.ResultType;
    ALIB_DBG(   ciFunction.DbgCallbackName  =  entry.DbgCallbackName; )
                return true;
            }

            // for constants, the callback might b invoked right away (optimizing cal out)
            if( ciFunction.AllArgsAreConst && entry.IsCTInvokable )
            {
                // calculate constant value
                ciFunction.TypeOrValue      = entry.Callback( ciFunction.CompileTimeScope,
                                                              ciFunction.ArgsBegin,
                                                              ciFunction.ArgsEnd           );
                ALIB_ASSERT_ERROR(ciFunction.TypeOrValue.IsSameType(*entry.ResultType),
                                  "Type mismatch in definition of function {!Q} ({}) in plugin {!Q}.\\n"
                                  "                    Type specified: <{}> (aka {})\\n"
                                  "         Type returned by callback: <{}> (aka {})",
                                  entry.Descriptor, entry.DbgCallbackName, CompilerPlugin::Name,
                                  CompilerPlugin::Cmplr.TypeName(*entry.ResultType),
                                                                  entry.ResultType->TypeID(),
                                  CompilerPlugin::Cmplr.TypeName(ciFunction.TypeOrValue),
                                                                 ciFunction.TypeOrValue.TypeID() )
    ALIB_DBG(   ciFunction.DbgCallbackName  = entry.DbgCallbackName;  )
                return true;
            }

            ciFunction.Callback             =  entry.Callback;
            ciFunction.TypeOrValue          = *entry.ResultType;
 ALIB_DBG(  ciFunction.DbgCallbackName      =  entry.DbgCallbackName; )
            return true;
        }
    }

    return false;
}



// #################################################################################################
// Unary operators
// #################################################################################################
void Calculus::AddUnaryOp  ( const String&      op,
                             Type               argType,
                             CallbackDecl       callback,
                             #if ALIB_DEBUG
                                const char*     dbgCallbackName,
                             #endif
                             Type               resultType,
                             CTInvokable        cti              )
{
    auto& map=  UnaryOpMap[op];
    ALIB_ASSERT_ERROR( map.find( argType.TypeID() ) == map.end(),
                       "Unary operator '{}' already defined for type <{}> (aka {}).",
                       op, Cmplr.TypeName( argType ), argType.TypeID()    )

    map[argType.TypeID()]=  std::make_tuple( callback, resultType, cti  ALIB_DBG(, dbgCallbackName) );
}

void Calculus::AddUnaryOps( UnaryOpTableEntry* table, size_t length )
{

    #define OP         std::get<0>( *(table + i) )
    #define TYPE       std::get<1>( *(table + i) ).TypeID()
    #define CBFUNC     std::get<2>( *(table + i) )
    #define RESULTTYPE std::get<ALIB_REL_DBG(3,4)>( *(table + i) )
    #define CTINVOKE   std::get<ALIB_REL_DBG(4,5)>( *(table + i) )
    for( size_t i= 0 ; i < length ; ++i )
    {
        auto& map=  UnaryOpMap[OP];
        ALIB_ASSERT_ERROR( map.find( TYPE ) == map.end(),
                           "Unary operator '{}' already defined for type <{}> (aka {}).",
                           OP, Cmplr.TypeName( std::get<1>( *(table + i) ) ),
                                               std::get<1>( *(table + i) ).TypeID()    )

        map[TYPE] =  std::make_tuple( CBFUNC,
                                      RESULTTYPE,
                                      CTINVOKE
                           ALIB_DBG(, std::get<3>(*(table + i)))            );

    }
    #undef OP
    #undef TYPE
    #undef CBFUNC
    #undef RESULTTYPE
    #undef CTINVOKE
}

void Calculus::AddUnaryOpAlias  ( const String& alias, Type argType, const String& op )
{
    auto& map=  UnaryOpAliases[alias];
    ALIB_ASSERT_ERROR( map.find( argType.TypeID() ) == map.end(),
                       "Unary operator alias '{}' already defined for type <{}> (aka {}).",
                       alias, Cmplr.TypeName(argType), argType.TypeID()    );
    map[argType.TypeID()]= op;
}

void Calculus::AddUnaryOpAliases( UnaryOpAliasTableEntry* table, size_t length )
{
    if( length == 0 )
        return;

    #define ALIAS       std::get<0>( *(table + i) )
    #define ARG_TYPE    std::get<1>( *(table + i) ).TypeID()
    #define OP          std::get<2>( *(table + i) )

    size_t i= 0;
    do
    {
        const String& alias= ALIAS;
        auto& map=  UnaryOpAliases[alias];
        do
        {
            ALIB_ASSERT_ERROR( map.find( ARG_TYPE ) == map.end(),
                               "Unary operator alias '{}' already defined for type <{}> (aka {}).",
                               alias, Cmplr.TypeName(std::get<1>( *(table + i) )), ARG_TYPE   )
            map[ARG_TYPE]= OP ;
        }
        while( ++i < length && ALIAS == alias );
    }
    while( i < length );

    #undef OP
    #undef ARG_TYPE
    #undef ALIAS

}

bool Calculus::TryCompilation( CIUnaryOp&  ciUnaryOp )
{
    Box& arg= ciUnaryOp.CompileTimeScope.Stack[0];

    // search alias first
    {
        auto it1= UnaryOpAliases.find( ciUnaryOp.Operator );
        if( it1 != UnaryOpAliases.end() )
        {
            auto it2= it1->second.find( arg.TypeID() );
            if( it2 != it1->second.end() )
            {
                ciUnaryOp.Operator= it2->second;
            }
        }
    }

    // search callback
    auto it1= UnaryOpMap.find( ciUnaryOp.Operator );
    if( it1 != UnaryOpMap.end() )
    {
        auto it2= it1->second.find( arg.TypeID() );
        if( it2 != it1->second.end() )
        {
            // for constants, the callback might b invoked right away (optimizing cal out)
            if( ciUnaryOp.ArgIsConst && std::get<2>(it2->second) )
            {
                // calculate constant value
                ciUnaryOp.TypeOrValue=  std::get<0>(it2->second)( ciUnaryOp.CompileTimeScope,
                                                                  ciUnaryOp.ArgsBegin,
                                                                  ciUnaryOp.ArgsEnd           );
       ALIB_DBG(ciUnaryOp.DbgCallbackName= std::get<3>(it2->second);)
                ALIB_ASSERT_ERROR(ciUnaryOp.TypeOrValue.IsSameType(std::get<1>(it2->second)),
                                  "Type mismatch in definition of unary operator {!Q} ({}) in plugin {!Q}.\\n"
                                  "                    Type specified: <{}> (aka {})\\n"
                                  "         Type returned by callback: <{}> (aka {})",
                                  ciUnaryOp.Operator, ciUnaryOp.DbgCallbackName, CompilerPlugin::Name,
                                  CompilerPlugin::Cmplr.TypeName(std::get<1>(it2->second)),
                                  std::get<1>(it2->second).TypeID(),
                                  CompilerPlugin::Cmplr.TypeName(ciUnaryOp.TypeOrValue),
                                  ciUnaryOp.TypeOrValue.TypeID()                                   )
                return true;
            }
            ciUnaryOp.Callback       = std::get<0>(it2->second);
            ciUnaryOp.TypeOrValue    = std::get<1>(it2->second);
   ALIB_DBG(ciUnaryOp.DbgCallbackName= std::get<3>(it2->second);)
            return true;
        }
    }

    // not found
    return false;
}

// #################################################################################################
// Binary operators
// #################################################################################################
void Calculus::AddBinaryOp     ( const String&      op,
                                 Type               lhsType,
                                 Type               rhsType,
                                 CallbackDecl       callback,
                                 #if ALIB_DEBUG
                                    const char*     dbgCallbackName,
                                 #endif
                                 Type               resultType,
                                 CTInvokable        cti                )
{
    auto& map= BinaryOpMap[op][lhsType.TypeID()];
    ALIB_ASSERT_ERROR( map.find( rhsType.TypeID() ) == map.end(),
                       "Binary operator '{}' already defined for types <{}> (aka {})\\n"
                       "                                           and <{}> (aka {}).",
                       op, Cmplr.TypeName( lhsType ), lhsType.TypeID(),
                           Cmplr.TypeName( rhsType ), rhsType.TypeID()  );
    map[rhsType.TypeID()]= std::make_tuple( callback, resultType, cti  ALIB_DBG(, dbgCallbackName) );

}

void Calculus::AddBinaryOps( BinaryOpTableEntry* table, size_t length )
{
    #define OP         std::get<0>( *(table + i) )
    #define LHS_TYPE   std::get<1>( *(table + i) ).TypeID()
    #define RHS_TYPE   std::get<2>( *(table + i) ).TypeID()
    #define CBFUNC     std::get<3>( *(table + i) )
    #define RESULTTYPE std::get<ALIB_REL_DBG(4,5)>( *(table + i) )
    #define CTINVOKE   std::get<ALIB_REL_DBG(5,6)>( *(table + i) )

    for( size_t i= 0 ; i < length ; ++i )
    {
        auto& map= BinaryOpMap[OP][LHS_TYPE];
        ALIB_ASSERT_ERROR( map.find( RHS_TYPE ) == map.end(),
                       "Binary operator '{}' already defined for types <{}> (aka {})\\n"
                       "                                           and <{}> (aka {}).",
                           OP, Cmplr.TypeName(std::get<1>( *(table+i))), LHS_TYPE,
                               Cmplr.TypeName(std::get<2>( *(table+i))), RHS_TYPE      )
        map[RHS_TYPE]= std::make_tuple( CBFUNC,
                                        RESULTTYPE,
                                        CTINVOKE
                             ALIB_DBG(, std::get<4>( *(table + i) )) );
    }

    #undef OP
    #undef LHS_TYPE
    #undef RHS_TYPE
    #undef CBFUNC
    #undef RESULTTYPE
    #undef CTINVOKE
}

void Calculus::AddBinaryOpAlias( const String& alias, Type lhs, Type rhs, const String& op )
{
    auto& map= BinaryOpAliases[alias][lhs.TypeID()];
    ALIB_ASSERT_ERROR( map.find( rhs.TypeID() ) == map.end(),
                       "Binary operator alias '{}' already defined for types <{}> (aka {})\\n"
                                                                        "and <{}> (aka {}).",
                       alias, Cmplr.TypeName( lhs ), lhs.TypeID(),
                              Cmplr.TypeName( rhs ), rhs.TypeID()                              )
    map[rhs.TypeID()]= op;
}

void Calculus::AddBinaryOpAliases( BinaryOpAliasTableEntry* table, size_t length )
{
    if( length == 0 )
        return;

    #define ALIAS      std::get<0>( *(table + i) )
    #define LHS_TYPE   std::get<1>( *(table + i) ).TypeID()
    #define RHS_TYPE   std::get<2>( *(table + i) ).TypeID()
    #define OP         std::get<3>( *(table + i) )

    size_t i= 0;
    do
    {
        const String& alias= ALIAS;
        auto& map1=  BinaryOpAliases[alias];
        do
        {
            auto& lhsType= LHS_TYPE;
            auto& map2=  map1[lhsType];
            do
            {
                ALIB_ASSERT_ERROR( map2.find( RHS_TYPE ) == map2.end(),
                                   "Binary operator alias '{}' already defined for types <{}> (aka {})\\n"
                                                                                    "and <{}> (aka {}).",
                                   alias,   Cmplr.TypeName( std::get<1>( *(table + i) ) ), LHS_TYPE,
                                            Cmplr.TypeName( std::get<2>( *(table + i) ) ), RHS_TYPE       )
                map2[RHS_TYPE]= OP;
            }
            while( ++i != length && LHS_TYPE == lhsType );
        }
        while( i < length && ALIAS == alias );
    }
    while( i < length );

    #undef ALIAS
    #undef LHS_TYPE
    #undef RHS_TYPE
    #undef OP
}

void Calculus::AddBinaryOpOptimizations( BinaryOpOptimizationsTableEntry* table, size_t length,
                                         bool lhsOrRhs )
{
    #define OP         std::get<0>( *(table + i) )
    #define TYPE       std::get<1>( *(table + i) ).TypeID()
    #define CONSTVAL   std::get<2>( *(table + i) )
    #define RESULT     std::get<3>( *(table + i) )

    auto& map= !lhsOrRhs ? BinaryOpConstLHSOptimizations
                         : BinaryOpConstRHSOptimizations;

    for( size_t i= 0 ; i < length ; ++i )
    {
        auto& map2= map[OP][TYPE];
        ALIB_ASSERT_ERROR( map2.find( CONSTVAL ) == map2.end(),
                           "Optimization already defined for operator {}, {}-hand side type <{}> (aka {}) and {}-hand constant {}.",
                           OP,
                           ( lhsOrRhs ? "left" : "right"),
                           Cmplr.TypeName( std::get<1>( *(table + i) ) ), TYPE,
                           ( lhsOrRhs ? "right" : "left"),
                           CONSTVAL                                                                )
        map2[CONSTVAL]= RESULT;
    }

    #undef OP
    #undef TYPE
    #undef CONSTVAL
    #undef RESULT
}


bool Calculus::TryCompilation( CIBinaryOp& ciBinaryOp )
{
    Box& lhs= * ciBinaryOp.ArgsBegin;
    Box& rhs= *(ciBinaryOp.ArgsBegin + 1);

    // search alias first
    if(    ciBinaryOp.Operator == A_CHAR("=")
        && EnumContains( Cmplr.CfgCompilation, Compilation::AliasEqualsOperatorWithAssignOperator ) )
    {
        ciBinaryOp.Operator= A_CHAR("==");
    }
    else
    {
        auto it1= BinaryOpAliases.find( ciBinaryOp.Operator );
        if( it1 != BinaryOpAliases.end() )
        {
            auto it2= it1->second.find( lhs.TypeID() );
            if( it2 != it1->second.end() )
            {
                auto it3= it2->second.find( rhs.TypeID() );
                if( it3 != it2->second.end() )
                {
                    ciBinaryOp.Operator= it3->second;
                }
            }
        }
    }

    #define  CBFUNC        std::get<0>(it3->second)
    #define  RESULTTYPE    std::get<1>(it3->second)
    #define  CT_INVOKABLE  std::get<2>(it3->second)
    #if ALIB_DEBUG
    #define  DBG_CB_NAME   std::get<3>(it3->second)
    #endif
    // search callback
    auto it1= BinaryOpMap.find( ciBinaryOp.Operator );
    if( it1 != BinaryOpMap.end() )
    {
        auto it2= it1->second.find( lhs.TypeID() );
        if( it2 != it1->second.end() )
        {
            auto it3= it2->second.find( rhs.TypeID() );
            if( it3 != it2->second.end() )
            {
                // if both are constant, the callback might be invoked right away (optimizing the call out)
                if( ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst )
                {
                    if( CT_INVOKABLE )
                    {
                        // calculate constant value
                        ciBinaryOp.TypeOrValue=  CBFUNC  ( ciBinaryOp.CompileTimeScope,
                                                           ciBinaryOp.ArgsBegin,
                                                           ciBinaryOp.ArgsEnd           );
           ALIB_DBG(    ciBinaryOp.DbgCallbackName= DBG_CB_NAME;)
                        ALIB_ASSERT_ERROR(ciBinaryOp.TypeOrValue.IsSameType(RESULTTYPE),
                                          "Type mismatch in definition of binary operator {!Q} ({}) of plugin {!Q}.\\n"
                                          "                    Type specified: <{}> (aka {})\\n"
                                          "         Type returned by callback: <{}> (aka {})",
                                          ciBinaryOp.Operator, ciBinaryOp.DbgCallbackName, CompilerPlugin::Name,
                                          CompilerPlugin::Cmplr.TypeName(RESULTTYPE            ),
                                                                         RESULTTYPE            .TypeID(),
                                          CompilerPlugin::Cmplr.TypeName(ciBinaryOp.TypeOrValue),
                                                                         ciBinaryOp.TypeOrValue.TypeID() )
                        return true;
                    }
                }


                // if one is constant, we may find an entry in BinaryOpConsL/RHSOptimizations
                else if( ciBinaryOp.LhsIsConst || ciBinaryOp.RhsIsConst )
                {
                    auto& map=           ciBinaryOp.LhsIsConst ?  BinaryOpConstLHSOptimizations
                                                               :  BinaryOpConstRHSOptimizations;
                    auto& nonConstType= (ciBinaryOp.LhsIsConst ?  *(ciBinaryOp.ArgsBegin + 1 )
                                                               :  *(ciBinaryOp.ArgsBegin     ) ).TypeID();
                    auto& constValue=    ciBinaryOp.LhsIsConst ?  *(ciBinaryOp.ArgsBegin     )
                                                               :  *(ciBinaryOp.ArgsBegin + 1 );
                    auto& map2= map[ciBinaryOp.Operator][nonConstType];
                    auto entryIt= map2.find( constValue );

                    if( entryIt  != map2.end() )
                    {
                        // found! If it is an unset box, this tells us, that the result is the other side
                        // (identity operation). Otherwise it is a constant.
                        if( entryIt->second.IsType<void>() )
                            ciBinaryOp.NonConstArgIsResult= true;
                        else
                            ciBinaryOp.TypeOrValue        = entryIt->second;
                        return true;
                    }
                }


                ciBinaryOp.Callback       = CBFUNC;
                ciBinaryOp.TypeOrValue    = RESULTTYPE;
       ALIB_DBG(ciBinaryOp.DbgCallbackName= DBG_CB_NAME; )
                return true;
            }
        }
    }

    return false;

    #undef CBFUNC
    #undef RESULTTYPE
    #undef CT_INVOKABLE
    #if ALIB_DEBUG
    #undef DBG_CB_NAME
    #endif
}


// #################################################################################################
// Auto-Casts
// #################################################################################################
//! @cond NO_DOX
namespace {
    Calculus::AutoCastEntry* findAutoCastEntry( std::vector<Calculus::AutoCastEntry>& table,
                                                Calculus::CIAutoCast&                 ciAutoCast,
                                                int                                   argNo       )
    {
        Box& valueToCast= *(ciAutoCast.ArgsBegin + argNo);

        // main loop over all table entries
        for( auto& entry : table )
        {
            // first check for source type
            if( !entry.Type.IsSameType( valueToCast ) )
                continue;

            // operator included in list of accepted (if list given)?
            bool operatorIsIn= true;
            if(    entry.OperatorsAccepted != nullptr
                && entry.OperatorsAccepted->size() > 0   )
            {
                operatorIsIn= false;
                for( auto& op : *entry.OperatorsAccepted )
                    if( op.Equals( ciAutoCast.Op ) )
                    {
                        operatorIsIn= true;
                        break;
                    }
            }

            // operator included in decline list?
            if(    operatorIsIn
                && entry.OperatorsDeclined != nullptr
                && entry.OperatorsDeclined->size() > 0    )
            {
                for( auto& op : *entry.OperatorsDeclined )
                    if( op.Equals( ciAutoCast.Op ) )
                    {
                        operatorIsIn= false;
                        break;
                    }
            }

            // found?
            if( operatorIsIn )
                return &entry;
        }

        return nullptr;
    }

    Box any2Int ( expressions::Scope&, ArgIterator argsBegin, ArgIterator )
    {
        return argsBegin->Data().GetInteger(0);
    }

} //anonymous namespace
//! @endcond


bool Calculus::TryCompilation( CIAutoCast& ciAutoCast )
{
    bool result= false;

    //-------- cast first arg -------
    AutoCastEntry* entry= findAutoCastEntry( AutoCasts, ciAutoCast, 0 );
    if( entry != nullptr )
    {
        result= true;
        ciAutoCast.ReverseCastFunctionName= entry->ReverseCastFunctionName;

        CallbackDecl callback;
        if( entry->Callback == nullptr )
        {
           callback                  =  any2Int;
ALIB_DBG(  ciAutoCast.DbgCallbackName=  "any2Int"; )
           ciAutoCast.TypeOrValue    =  Types::Integer;
        }
        else
        {
           callback                  =  entry->Callback;
ALIB_DBG(  ciAutoCast.DbgCallbackName=  entry->DbgCallbackName; )
           ciAutoCast.TypeOrValue    =  entry->ResultType;
        }

        if( ciAutoCast.IsConst )
            ciAutoCast.TypeOrValue= callback( ciAutoCast.CompileTimeScope,
                                              ciAutoCast.ArgsBegin,
                                              ciAutoCast.ArgsEnd           );
        else
            ciAutoCast.Callback    = callback;
    }

    // no RHS given?
    if(     ciAutoCast.ArgsBegin + 1 >= ciAutoCast.ArgsEnd )
        return result;

    //-------- cast second arg (rhs) -------
    entry= findAutoCastEntry( AutoCasts, ciAutoCast, 1 );
    if( entry != nullptr )
    {
        //
        result= true;
        ciAutoCast.ReverseCastFunctionNameRhs= entry->ReverseCastFunctionName;

        CallbackDecl callback;
        if( entry->Callback == nullptr )
        {
           callback                     =  any2Int;
ALIB_DBG(  ciAutoCast.DbgCallbackNameRhs=  "any2Int"; )
           ciAutoCast.TypeOrValueRhs    =  Types::Integer;
        }
        else
        {
           callback                     =  entry->Callback;
ALIB_DBG(  ciAutoCast.DbgCallbackNameRhs=  entry->DbgCallbackName; )
           ciAutoCast.TypeOrValueRhs    =  entry->ResultType;
        }

        if( ciAutoCast.RhsIsConst )
            ciAutoCast.TypeOrValueRhs= callback( ciAutoCast.CompileTimeScope,
                                                 ciAutoCast.ArgsBegin + 1,
                                                 ciAutoCast.ArgsEnd           );
        else
            ciAutoCast.CallbackRhs    = callback;
    }


    return result;
}

}}}} // namespace [aworx::lib::expressions::plugin]



