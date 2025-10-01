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
#include "alib/expressions/expressions.prepro.hpp"
#include <vector>
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Expressions.Impl;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
#else
#   include "ALib.Characters.Functions.H"
#   include "ALib.Strings.H"
#   include "ALib.Expressions.Impl.H"
#endif
// ======================================   Implementation   =======================================
ALIB_WARNINGS_IGNORE_UNUSED_MACRO

namespace alib {  namespace expressions {  namespace plugins {

// #################################################################################################
// Operator setup helpers (unary and binary)
// #################################################################################################
void Calculus::AddOperator     ( const String&      op,
                                 Type               lhsType,
                                 Type               rhsType,
                                 CallbackDecl       callback,
                                 #if ALIB_DEBUG
                                    const char*     dbgCallbackName,
                                 #endif
                                 Type               resultType,
                                 CTInvokable        cti                )
{
    #if ALIB_DEBUG
        auto result=
        Operators.EmplaceIfNotExistent(
                    OperatorKey { op, lhsType.TypeID(), rhsType.TypeID() },
                    std::make_tuple( callback, resultType, cti  ALIB_DBG(, dbgCallbackName) ) );

        ALIB_ASSERT_ERROR( result.second == true, "EXPR", // assert this was an insert!
                           "Binary operator '{}' already defined for types <{}> (aka {})\n"
                           "                                           and <{}> (aka {}).",
                           op, Cmplr.TypeName( lhsType ), &lhsType.TypeID(),
                               Cmplr.TypeName( rhsType ), &rhsType.TypeID()  )
    #else
        Operators.EmplaceUnique(
                    OperatorKey { op, lhsType.TypeID(), rhsType.TypeID() },
                    std::make_tuple( callback, resultType, cti  ALIB_DBG(, dbgCallbackName) ) );
    #endif
}

void Calculus::AddOperators( OperatorTableEntry* table, size_t length )
{
    Operators.Reserve( integer(length), lang::ValueReference::Relative );
    ALIB_DBG( auto actBucketCount= Operators.BucketCount(); )

    #define OP         std::get<0>( *(table + i) )
    #define LHS_TYPE   std::get<1>( *(table + i) ).TypeID()
    #define RHS_TYPE   std::get<2>( *(table + i) ).TypeID()
    #define CBFUNC     std::get<3>( *(table + i) )
    #define RESULTTYPE std::get<ALIB_REL_DBG(4,5)>( *(table + i) )
    #define CTINVOKE   std::get<ALIB_REL_DBG(5,6)>( *(table + i) )

    for( size_t i= 0 ; i < length ; ++i )
    {
        #if ALIB_DEBUG
            auto result=
            Operators.EmplaceIfNotExistent(
                        OperatorKey { OP, LHS_TYPE, RHS_TYPE },
                        std::make_tuple( CBFUNC, RESULTTYPE, CTINVOKE
                                           ALIB_DBG(, std::get<4>( *(table + i) ))   ) );

            ALIB_ASSERT_ERROR( result.second == true, "EXPR",// assert this was an insert!
                       "Binary operator '{}' already defined for types <{}> (aka {})\n"
                       "                                           and <{}> (aka {}).",
                           OP, Cmplr.TypeName(std::get<1>( *(table+i))), &LHS_TYPE,
                               Cmplr.TypeName(std::get<2>( *(table+i))), &RHS_TYPE      )
        #else
            Operators.EmplaceUnique(
                        OperatorKey { OP, LHS_TYPE, RHS_TYPE },
                        std::make_tuple( CBFUNC, RESULTTYPE, CTINVOKE
                                           ALIB_DBG(, std::get<4>( *(table + i) ))   ) );
        #endif
    }

    ALIB_ASSERT_ERROR( actBucketCount == Operators.BucketCount(), "EXPR",
                       "This is rather an internal error of HashTable: The number of buckets "
                       "of hash map 'Operators' increased, although it was reserved above." )

    #undef OP
    #undef LHS_TYPE
    #undef RHS_TYPE
    #undef CBFUNC
    #undef RESULTTYPE
    #undef CTINVOKE
}

void Calculus::AddOperatorAlias( const String& alias, Type lhs, Type rhs, const String& op )
{
    #if ALIB_DEBUG
        auto result=
        OperatorAliases.EmplaceIfNotExistent( OperatorKey { alias, lhs.TypeID(), rhs.TypeID() },
                                              op );

        ALIB_ASSERT_ERROR( result.second == true, "EXPR",// assert this was an insert!
                           "Binary operator alias '{}' already defined for types <{}> (aka {})\n"
                                                                            "and <{}> (aka {}).",
                           alias, Cmplr.TypeName( lhs ), &lhs.TypeID(),
                                  Cmplr.TypeName( rhs ), &rhs.TypeID()                              )
    #else
        OperatorAliases.EmplaceUnique( OperatorKey { alias, lhs.TypeID(), rhs.TypeID() },
                                       op );
    #endif
}

void Calculus::AddOperatorAliases( OperatorAliasTableEntry* table, size_t length )
{
    OperatorAliases.Reserve( integer(length), lang::ValueReference::Relative );

    #define ALIAS      std::get<0>( *(table + i) )
    #define LHS_TYPE   std::get<1>( *(table + i) ).TypeID()
    #define RHS_TYPE   std::get<2>( *(table + i) ).TypeID()
    #define OP         std::get<3>( *(table + i) )

    for( size_t i= 0 ; i < length ; ++i )
    {
        #if ALIB_DEBUG
            auto result=
            OperatorAliases.EmplaceIfNotExistent( OperatorKey { ALIAS, LHS_TYPE, RHS_TYPE },  OP );

            ALIB_ASSERT_ERROR( result.second == true, "EXPR",// assert this was an insert!
                           "Binary operator alias '{}' already defined for types <{}> (aka {})\n"
                                                                            "and <{}> (aka {}).",
                           ALIAS,   Cmplr.TypeName( std::get<1>( *(table + i) ) ), &LHS_TYPE,
                                    Cmplr.TypeName( std::get<2>( *(table + i) ) ), &RHS_TYPE       )
        #else
            OperatorAliases.EmplaceUnique( OperatorKey { ALIAS, LHS_TYPE, RHS_TYPE },
                                           OP );
        #endif
    }

    #undef ALIAS
    #undef LHS_TYPE
    #undef RHS_TYPE
    #undef OP
}

// #################################################################################################
// Unary operators
// #################################################################################################
bool Calculus::TryCompilation( CIUnaryOp&  ciUnaryOp )
{
    Box& arg= ciUnaryOp.CompileTimeScope.Stack->at(0);
    OperatorKey key     =  { ciUnaryOp.Operator, arg.TypeID(), typeid(void) };
    auto        hashCode=  OperatorKey::Hash()( key );

    // search alias first
    {
        auto aliasIt= OperatorAliases.Find( key, hashCode );
        if( aliasIt != OperatorAliases.end() )
            ciUnaryOp.Operator= aliasIt.Mapped();
    }


    // search callback
    auto opIt= Operators.Find( key, hashCode );
    if( opIt == Operators.end() )
        return false;

    auto& op= opIt.Mapped();

    // for constants, the callback might b invoked right away (optimizing cal out)
    if( ciUnaryOp.ArgIsConst && std::get<2>(op) )
    {
        // calculate constant value
        ciUnaryOp.TypeOrValue=  std::get<0>(op)( ciUnaryOp.CompileTimeScope,
                                                          ciUnaryOp.ArgsBegin,
                                                          ciUnaryOp.ArgsEnd           );
ALIB_DBG(ciUnaryOp.DbgCallbackName= std::get<3>(op);)
        ALIB_ASSERT_ERROR(ciUnaryOp.TypeOrValue.IsSameType(std::get<1>(op)), "EXPR",
            "Type mismatch in definition of unary operator \"{}\" ({}) in plugin \"{}\".\n"
            "                    Type specified: <{}> (aka {})\n"
            "         Type returned by callback: <{}> (aka {})",
            ciUnaryOp.Operator, ciUnaryOp.DbgCallbackName, CompilerPlugin::Name,
            CompilerPlugin::Cmplr.TypeName(std::get<1>(op)),
            &std::get<1>(op).TypeID(),
            CompilerPlugin::Cmplr.TypeName(ciUnaryOp.TypeOrValue),
            &ciUnaryOp.TypeOrValue.TypeID()                                   )
        return true;
    }
    ciUnaryOp.Callback       = std::get<0>(op);
    ciUnaryOp.TypeOrValue    = std::get<1>(op);
ALIB_DBG(ciUnaryOp.DbgCallbackName= std::get<3>(op);)

    return true;
}

// #################################################################################################
// Binary operators
// #################################################################################################
void Calculus::AddBinaryOpOptimizations( BinaryOpOptimizationsTableEntry* table, size_t length )
{
    BinaryOperatorOptimizations.Reserve(  integer(length), lang::ValueReference::Relative );

    #define OP         std::get<0>( *(table + i) )
    #define SIDE       std::get<1>( *(table + i) )
    #define CONSTVAL   std::get<2>( *(table + i) )
    #define CONSTTYPE  std::get<2>( *(table + i) ).TypeID()
    #define OTHERBOX   std::get<3>( *(table + i) )
    #define OTHERTYPE  std::get<3>( *(table + i) ).TypeID()
    #define RESULT     std::get<4>( *(table + i) )

    for( size_t i= 0 ; i < length ; ++i )
    {
        #if ALIB_DEBUG
            auto result=
                BinaryOperatorOptimizations.EmplaceIfNotExistent( BinOpOptKey { OP, SIDE, CONSTVAL, OTHERTYPE },  RESULT );

            ALIB_ASSERT_ERROR( result.second == true, "EXPR", // assert this was an insert!
               "Optimization already defined for operator \"{}\" with {!Lower}-hand "
               "constant value \"{}\" of type <{}> (aka {}) and with "
               "{!L}-hand type <{}> (aka {}).",
               OP, SIDE, CONSTVAL,                            Cmplr.TypeName(CONSTVAL), &CONSTTYPE,
               SIDE == lang::Side::Left ? lang::Side::Right
                                        : lang::Side::Left,   Cmplr.TypeName(OTHERBOX), &OTHERTYPE  )
        #else
            BinaryOperatorOptimizations.EmplaceUnique( BinOpOptKey { OP, SIDE, CONSTVAL, OTHERTYPE },
                                                       RESULT );
        #endif
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

    OperatorKey key     =  { ciBinaryOp.Operator, lhs.TypeID(), rhs.TypeID() };
    auto        hashCode=  OperatorKey::Hash()( key );

    // search alias first
    if(    ciBinaryOp.Operator == A_CHAR("=")
        && HasBits( Cmplr.CfgCompilation, Compilation::AliasEqualsOperatorWithAssignOperator ) )
    {
        ciBinaryOp.Operator= A_CHAR("==");
    }
    else
    {
        auto aliasIt =  OperatorAliases.Find( key, hashCode );
        if(  aliasIt != OperatorAliases.end() )
            ciBinaryOp.Operator= aliasIt.Mapped();
    }

    #define     CBFUNC        std::get<0>(op)
    #define     RESULTTYPE    std::get<1>(op)
    #define     CT_INVOKABLE  std::get<2>(op)
    #if ALIB_DEBUG
    #   define  DBG_CB_NAME   std::get<3>(op)
    #endif

// search callback
    auto opIt =  Operators.Find( key, hashCode );
    if(  opIt == Operators.end() )
        return false;

    auto& op= opIt.Mapped();

    // if both are constant, the callback might be invoked right away (optimizing the call out)
    if( ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst )
    {
        if( CT_INVOKABLE )
        {
            // calculate constant value
            ciBinaryOp.TypeOrValue=  CBFUNC  ( ciBinaryOp.CompileTimeScope,
                                               ciBinaryOp.ArgsBegin,
                                               ciBinaryOp.ArgsEnd           );
ALIB_DBG(   ciBinaryOp.DbgCallbackName= DBG_CB_NAME;                                    )
            ALIB_ASSERT_ERROR(ciBinaryOp.TypeOrValue.IsSameType(RESULTTYPE), "EXPR",
                "Type mismatch in definition of binary operator \"{}\" ({}) of plugin \"{}\".\n"
                "                    Type specified: <{}> (aka {})\n"
                "         Type returned by callback: <{}> (aka {})",
                ciBinaryOp.Operator, ciBinaryOp.DbgCallbackName, CompilerPlugin::Name,
                CompilerPlugin::Cmplr.TypeName(RESULTTYPE            ),
                                              &RESULTTYPE            .TypeID(),
                CompilerPlugin::Cmplr.TypeName(ciBinaryOp.TypeOrValue),
                                              &ciBinaryOp.TypeOrValue.TypeID() )
            return true;
        }
    }


    // if one is constant, we may find an entry in BinaryOpConsL/RHSOptimizations
    else if( ciBinaryOp.LhsIsConst || ciBinaryOp.RhsIsConst )
    {
        auto& nonConstType= (ciBinaryOp.LhsIsConst ?  *(ciBinaryOp.ArgsBegin + 1 )
                                                   :  *(ciBinaryOp.ArgsBegin     ) ).TypeID();
        auto& constValue=    ciBinaryOp.LhsIsConst ?  *(ciBinaryOp.ArgsBegin     )
                                                   :  *(ciBinaryOp.ArgsBegin + 1 );

        auto entryIt= BinaryOperatorOptimizations.Find( { ciBinaryOp.Operator,
                                                          ciBinaryOp.RhsIsConst ? lang::Side::Right : lang::Side::Left,
                                                          constValue,
                                                          nonConstType
                                                         } );
        if( entryIt  != BinaryOperatorOptimizations.end() )
        {
            // found! If it is an unset box, this tells us, that the result is the other side
            // (identity operation). Otherwise it is a constant.
            if( entryIt.Mapped().IsType<void>() )
                ciBinaryOp.NonConstArgIsResult= true;
            else
                ciBinaryOp.TypeOrValue        = entryIt.Mapped();
            return true;
        }
    }


    ciBinaryOp.Callback       = CBFUNC;
    ciBinaryOp.TypeOrValue    = RESULTTYPE;
ALIB_DBG(
    ciBinaryOp.DbgCallbackName= DBG_CB_NAME; )
    return true;


    #undef CBFUNC
    #undef RESULTTYPE
    #undef CT_INVOKABLE
    #if ALIB_DEBUG
    #undef DBG_CB_NAME
    #endif
}


// #################################################################################################
// Functions
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
                if(     ciFunction.IsIdentifier
                    && !HasBits(Cmplr.CfgCompilation, Compilation::AllowOmittingParenthesesOfParameterlessFunctions ) )
                    throw Exception( ALIB_CALLER_NULLED, Exceptions::MissingFunctionParentheses,
                                     entry.Descriptor );

                if(    !ciFunction.IsIdentifier
                    && !HasBits(Cmplr.CfgCompilation, Compilation::AllowEmptyParenthesesForIdentifierFunctions ) )
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
                --qtyRequired;
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
                String256 buffer( entry.Descriptor );
                if( qtyRequired )
                    Cmplr.WriteFunctionSignature( entry.Signature,
                                                  entry.SignatureLength,
                                                  buffer );
                ciFunction.AddFunctionsWithNonMatchingArguments( buffer );

                // search next
                continue;
            }

            // check for wrong parentheses
            if(     ciFunction.IsIdentifier
                &&  entry.Signature != nullptr
                && !HasBits(Cmplr.CfgCompilation, Compilation::AllowOmittingParenthesesOfParameterlessFunctions ) )
                throw Exception( ALIB_CALLER_NULLED, Exceptions::MissingFunctionParentheses,
                                 entry.Descriptor );

            if(    !ciFunction.IsIdentifier
                &&  entry.Signature == nullptr
                && !HasBits(Cmplr.CfgCompilation, Compilation::AllowEmptyParenthesesForIdentifierFunctions ) )
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
                ALIB_ASSERT_ERROR(ciFunction.TypeOrValue.IsSameType(*entry.ResultType), "EXPR",
                    "Type mismatch in definition of function \"{}\" ({}) in plugin \"{}\".\n"
                    "                    Type specified: <{}> (aka {})\n"
                    "         Type returned by callback: <{}> (aka {})",
                    entry.Descriptor, entry.DbgCallbackName, CompilerPlugin::Name,
                    CompilerPlugin::Cmplr.TypeName(*entry.ResultType),
                                                   &entry.ResultType->TypeID(),
                    CompilerPlugin::Cmplr.TypeName(ciFunction.TypeOrValue),
                                                  &ciFunction.TypeOrValue.TypeID() )
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
                    if( op.Equals<NC>( ciAutoCast.Operator ) )
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
                    if( op.Equals<NC>( ciAutoCast.Operator ) )
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
        return argsBegin->Data().Integrals.Array[0];
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

}}} // namespace [alib::expressions::plugin]

ALIB_WARNINGS_RESTORE
