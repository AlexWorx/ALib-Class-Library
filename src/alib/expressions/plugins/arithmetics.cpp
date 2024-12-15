// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#if !DOXYGEN

#include "alib/expressions/plugins/arithmetics.hpp"
#include <math.h>

#define ARG0           (*args)
#define ARG1           (*(args+1))
#define BOL(box)       (box).Unbox<bool     >()
#define INT(box)       (box).Unbox<integer>()
#define ITF(box)       static_cast<double>((box).Unbox<integer>() )
#define BTF(box)       static_cast<double>((box).Unbox<bool   >() )
#define FLT(box)       (box).Unbox<double   >()
#define FUNC( name,...) Box name( Scope& scope,                                                 \
                                  ArgIterator  args,                                            \
                                  ArgIterator  end    )                                         \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }

#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
#   define TOINT(arg)                      arg
#else
#   define TOINT(arg) static_cast<integer>(arg)
#endif


namespace alib {  namespace expressions { namespace plugins {


FUNC( ToBoolean , return  args->Call<FIsTrue>();  )

namespace {


// #################################################################################################
// ### Arithmetics - constants
// #################################################################################################

Box constTrue  = true;
Box constFalse = false;
Box identity   = nullptr;
Box bool_false = false;
Box bool_true  = true;
Box int_0      = static_cast<integer>( 0 );
Box int_1      = static_cast<integer>( 1 );
Box float_0    = 0.0;
Box float_1    = 1.0;


// #################################################################################################
// ### Arithmetics - Functions
// #################################################################################################
FUNC( toInt_B  , return static_cast<integer>(BOL(ARG0));   )
FUNC( toInt_I  , return ARG0;                              )
FUNC( toInt_F  , return static_cast<integer>(FLT(ARG0));   )
FUNC( toFloat_B, return static_cast<double >(BOL(ARG0));   )
FUNC( toFloat_I, return static_cast<double >(INT(ARG0));   )
FUNC( toFloat_F, return ARG0;                              )

FUNC( arrLen,    return  ARG0.UnboxLength();               )


// #################################################################################################
// ### Arithmetics - unary operations
// #################################################################################################

FUNC( pos      , return  ARG0;                                  )
FUNC( pos_B    , return   static_cast<integer>(BOL(ARG0));      )
FUNC( neg_B    , return  -static_cast<integer>(BOL(ARG0));      )
FUNC( neg_I    , return  -INT(ARG0);                            )
FUNC( neg_F    , return  -FLT(ARG0);                            )
FUNC( bitNot   , return  ~INT(ARG0);                            )
FUNC( boolNot_B, return  !BOL(ARG0);                            )
FUNC( boolNot_I, return   INT(ARG0) == static_cast<integer>(0); )
FUNC( boolNot_F, return   FLT(ARG0) == 0.0;                     )


// #################################################################################################
// ### Arithmetics - binary operations
// #################################################################################################

#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4804 ) // unsafe use of type bool
    #pragma warning( disable : 4805 ) // unsafe mix of type bool and int
#endif

FUNC(     mul_BB, return  BOL(ARG0)  *   BOL(ARG1) ; )
FUNC(     mul_BI, return  BOL(ARG0)  *   INT(ARG1) ; )
FUNC(     mul_BF, return  BOL(ARG0)  *   FLT(ARG1) ; )
FUNC(     mul_IB, return  INT(ARG0)  *   BOL(ARG1) ; )
FUNC(     mul_II, return  INT(ARG0)  *   INT(ARG1) ; )
FUNC(     mul_IF, return  ITF(ARG0)  *   FLT(ARG1) ; )
FUNC(     mul_FB, return  FLT(ARG0)  *   BTF(ARG1) ; )
FUNC(     mul_FI, return  FLT(ARG0)  *   ITF(ARG1) ; )
FUNC(     mul_FF, return  FLT(ARG0)  *   FLT(ARG1) ; )
FUNC(     div_BI, return  BOL(ARG0)  /   INT(ARG1) ; )
FUNC(     div_BF, return  BOL(ARG0)  /   FLT(ARG1) ; )
FUNC(     div_II, return  INT(ARG0)  /   INT(ARG1) ; )
FUNC(     div_IF, return  ITF(ARG0)  /   FLT(ARG1) ; )
FUNC(     div_FI, return  FLT(ARG0)  /   ITF(ARG1) ; )
FUNC(     div_FF, return  FLT(ARG0)  /   FLT(ARG1) ; )
FUNC(     mod_BI, return  BOL(ARG0)  %   INT(ARG1) ; )
FUNC(     mod_II, return  INT(ARG0)  %   INT(ARG1) ; )
FUNC(     mod_BF, return fmod(BOL(ARG0), FLT(ARG1)); )
FUNC(     mod_FI, return fmod(FLT(ARG0), INT(ARG1)); )
FUNC(     mod_IF, return fmod(INT(ARG0), FLT(ARG1)); )
FUNC(     mod_FF, return fmod(FLT(ARG0), FLT(ARG1)); )

FUNC(     add_BB, return  BOL(ARG0)  +   BOL(ARG1) ; )
FUNC(     add_BI, return  BOL(ARG0)  +   INT(ARG1) ; )
FUNC(     add_BF, return  BOL(ARG0)  +   FLT(ARG1) ; )
FUNC(     add_IB, return  INT(ARG0)  +   BOL(ARG1) ; )
FUNC(     add_II, return  INT(ARG0)  +   INT(ARG1) ; )
FUNC(     add_IF, return  ITF(ARG0)  +   FLT(ARG1) ; )
FUNC(     add_FB, return  FLT(ARG0)  +   BOL(ARG1) ; )
FUNC(     add_FI, return  FLT(ARG0)  +   ITF(ARG1) ; )
FUNC(     add_FF, return  FLT(ARG0)  +   FLT(ARG1) ; )
FUNC(     sub_BB, return  BOL(ARG0)  -   BOL(ARG1) ; )
FUNC(     sub_BI, return  BOL(ARG0)  -   INT(ARG1) ; )
FUNC(     sub_BF, return  BOL(ARG0)  -   FLT(ARG1) ; )
FUNC(     sub_IB, return  INT(ARG0)  -   BOL(ARG1) ; )
FUNC(     sub_II, return  INT(ARG0)  -   INT(ARG1) ; )
FUNC(     sub_IF, return  ITF(ARG0)  -   FLT(ARG1) ; )
FUNC(     sub_FB, return  FLT(ARG0)  -   BOL(ARG1) ; )
FUNC(     sub_FI, return  FLT(ARG0)  -   ITF(ARG1) ; )
FUNC(     sub_FF, return  FLT(ARG0)  -   FLT(ARG1) ; )

FUNC(    shfL_BI, return  TOINT( BOL(ARG0)  <<  INT(ARG1) ); )
FUNC(    shfL_IB, return         INT(ARG0)  <<  BOL(ARG1)  ; )
FUNC(    shfL_II, return         INT(ARG0)  <<  INT(ARG1)  ; )
FUNC(    shfR_BI, return  TOINT( BOL(ARG0)  <<  INT(ARG1) ); )
FUNC(    shfR_IB, return         INT(ARG0)  <<  BOL(ARG1)  ; )
FUNC(    shfR_II, return         INT(ARG0)  >>  INT(ARG1)  ; )

FUNC(      sm_BB, return  BOL(ARG0)  <   BOL(ARG1) ; )
FUNC(      sm_BI, return  BOL(ARG0)  <   INT(ARG1) ; )
FUNC(      sm_BF, return  isless(static_cast<double>(BOL(ARG0)),                     FLT(ARG1) ); )
FUNC(      sm_IB, return  INT(ARG0)  <   BOL(ARG1) ; )
FUNC(      sm_II, return  INT(ARG0)  <   INT(ARG1) ; )
FUNC(      sm_IF, return  isless(static_cast<double>(INT(ARG0)),                     FLT(ARG1) ); )
FUNC(      sm_FB, return  isless(                    FLT(ARG0) , static_cast<double>(BOL(ARG1))); )
FUNC(      sm_FI, return  isless(                    FLT(ARG0) , static_cast<double>(INT(ARG1))); )
FUNC(      sm_FF, return  isless(                    FLT(ARG0) ,                     FLT(ARG1) ); )
FUNC(    smeq_BB, return  BOL(ARG0)  <=  BOL(ARG1) ; )
FUNC(    smeq_BI, return  BOL(ARG0)  <=  INT(ARG1) ; )
FUNC(    smeq_BF, return  islessequal(static_cast<double>(BOL(ARG0)),                     FLT(ARG1) ); )
FUNC(    smeq_IB, return  INT(ARG0)  <=  BOL(ARG1) ; )
FUNC(    smeq_II, return  INT(ARG0)  <=  INT(ARG1) ; )
FUNC(    smeq_IF, return  islessequal(static_cast<double>(INT(ARG0)),                     FLT(ARG1) ); )
FUNC(    smeq_FB, return  islessequal(                    FLT(ARG0) , static_cast<double>(BOL(ARG1))); )
FUNC(    smeq_FI, return  islessequal(                    FLT(ARG0) , static_cast<double>(INT(ARG1))); )
FUNC(    smeq_FF, return  islessequal(                    FLT(ARG0) ,                     FLT(ARG1) ); )
FUNC(      gt_BB, return  BOL(ARG0)  >   BOL(ARG1) ; )
FUNC(      gt_BI, return  BOL(ARG0)  >   INT(ARG1) ; )
FUNC(      gt_BF, return  isgreater(static_cast<double>(BOL(ARG0)),                     FLT(ARG1) ); )
FUNC(      gt_IB, return  INT(ARG0)  >   BOL(ARG1) ; )
FUNC(      gt_II, return  INT(ARG0)  >   INT(ARG1) ; )
FUNC(      gt_IF, return  isgreater(static_cast<double>(INT(ARG0)),                     FLT(ARG1) ); )
FUNC(      gt_FB, return  isgreater(                    FLT(ARG0) , static_cast<double>(BOL(ARG1))); )
FUNC(      gt_FI, return  isgreater(                    FLT(ARG0) , static_cast<double>(INT(ARG1))); )
FUNC(      gt_FF, return  isgreater(                    FLT(ARG0) ,                     FLT(ARG1) ); )
FUNC(    gteq_BB, return  BOL(ARG0)  >=  BOL(ARG1) ; )
FUNC(    gteq_BI, return  BOL(ARG0)  >=  INT(ARG1) ; )
FUNC(    gteq_BF, return  isgreaterequal(static_cast<double>(BOL(ARG0)),                     FLT(ARG1) ); )
FUNC(    gteq_IB, return  INT(ARG0)  >=  BOL(ARG1) ; )
FUNC(    gteq_II, return  INT(ARG0)  >=  INT(ARG1) ; )
FUNC(    gteq_IF, return  isgreaterequal(static_cast<double>(INT(ARG0)),                     FLT(ARG1) ); )
FUNC(    gteq_FB, return  isgreaterequal(                    FLT(ARG0) , static_cast<double>(BOL(ARG1))); )
FUNC(    gteq_FI, return  isgreaterequal(                    FLT(ARG0) , static_cast<double>(INT(ARG1))); )
FUNC(    gteq_FF, return  isgreaterequal(                    FLT(ARG0) ,                     FLT(ARG1) ); )

FUNC(      eq_BB, return             BOL(ARG0)  ==  BOL(ARG1) ; )
FUNC(      eq_BI, return             BOL(ARG0)  ==  INT(ARG1) ; )
FUNC(      eq_BF, return  std::fabs( BOL(ARG0)  -   FLT(ARG1) ) <= std::numeric_limits<double>::epsilon(); )
FUNC(      eq_IB, return             INT(ARG0)  ==  BOL(ARG1) ; )
FUNC(      eq_II, return             INT(ARG0)  ==  INT(ARG1) ; )
FUNC(      eq_IF, return  std::fabs( ITF(ARG0)  -   FLT(ARG1) ) <= std::numeric_limits<double>::epsilon(); )
FUNC(      eq_FB, return  std::fabs( FLT(ARG0)  -   BOL(ARG1) ) <= std::numeric_limits<double>::epsilon(); )
FUNC(      eq_FI, return  std::fabs( FLT(ARG0)  -   ITF(ARG1) ) <= std::numeric_limits<double>::epsilon(); )
FUNC(      eq_FF, return  std::fabs( FLT(ARG0)  -   FLT(ARG1) ) <= std::numeric_limits<double>::epsilon(); )



FUNC(     neq_BB, return             BOL(ARG0)  !=  BOL(ARG1) ; )
FUNC(     neq_BI, return             BOL(ARG0)  !=  INT(ARG1) ; )
FUNC(     neq_BF, return  std::fabs( BOL(ARG0)  -   FLT(ARG1) ) >  std::numeric_limits<double>::epsilon(); )
FUNC(     neq_IB, return             INT(ARG0)  !=  BOL(ARG1) ; )
FUNC(     neq_II, return             INT(ARG0)  !=  INT(ARG1) ; )
FUNC(     neq_IF, return  std::fabs( INT(ARG0)  -   INT(ARG1) ) >  std::numeric_limits<double>::epsilon(); )
FUNC(     neq_FB, return  std::fabs( FLT(ARG0)  -   BOL(ARG1) ) >  std::numeric_limits<double>::epsilon(); )
FUNC(     neq_FI, return  std::fabs( FLT(ARG0)  -   ITF(ARG1) ) >  std::numeric_limits<double>::epsilon(); )
FUNC(     neq_FF, return  std::fabs( FLT(ARG0)  -   FLT(ARG1) ) >  std::numeric_limits<double>::epsilon(); )


FUNC(     bitAnd, return  INT(ARG0)  &  INT(ARG1) ; )
FUNC(     bitXOr, return  INT(ARG0)  ^  INT(ARG1) ; )
FUNC(     bitOr , return  INT(ARG0)  |  INT(ARG1) ; )
FUNC( boolAnd_BB, return  BOL(ARG0)             &&  BOL(ARG1)        ; )
FUNC( boolAnd_BI, return  BOL(ARG0)             &&  INT(ARG1) !=   0 ; )
FUNC( boolAnd_BF, return  BOL(ARG0)             &&  FLT(ARG1) != 0.0 ; )
FUNC( boolAnd_IB, return  INT(ARG0) !=   0      &&  BOL(ARG1)        ; )
FUNC( boolAnd_II, return  INT(ARG0) !=   0      &&  INT(ARG1) !=   0 ; )
FUNC( boolAnd_IF, return  INT(ARG0) !=   0      &&  FLT(ARG1) != 0.0 ; )
FUNC( boolAnd_FB, return  FLT(ARG0) != 0.0      &&  BOL(ARG1)        ; )
FUNC( boolAnd_FI, return  FLT(ARG0) != 0.0      &&  INT(ARG1) !=   0 ; )
FUNC( boolAnd_FF, return  FLT(ARG0) != 0.0      &&  FLT(ARG1) != 0.0 ; )
FUNC(  boolOr_BB, return  BOL(ARG0)             ||  BOL(ARG1)        ; )
FUNC(  boolOr_BI, return  BOL(ARG0)             ||  INT(ARG1) !=   0 ; )
FUNC(  boolOr_BF, return  BOL(ARG0)             ||  FLT(ARG1) != 0.0 ; )
FUNC(  boolOr_IB, return  INT(ARG0) !=   0      ||  BOL(ARG1)        ; )
FUNC(  boolOr_II, return  INT(ARG0) !=   0      ||  INT(ARG1) !=   0 ; )
FUNC(  boolOr_IF, return  INT(ARG0) !=   0      ||  FLT(ARG1) != 0.0 ; )
FUNC(  boolOr_FB, return  FLT(ARG0) != 0.0      ||  BOL(ARG1)        ; )
FUNC(  boolOr_FI, return  FLT(ARG0) != 0.0      ||  INT(ARG1) !=   0 ; )
FUNC(  boolOr_FF, return  FLT(ARG0) != 0.0      ||  FLT(ARG1) != 0.0 ; )

#if defined(_MSC_VER)
    #pragma warning( pop )
#endif

Calculus::OperatorTableEntry  OperatorTable[] =
{
    // unary operators
    { A_CHAR("+"), Types::Integer  , Types::Void    , CALCULUS_CALLBACK(      pos ), Types::Integer, Calculus::CTI },
    { A_CHAR("+"), Types::Float    , Types::Void    , CALCULUS_CALLBACK(      pos ), Types::Float  , Calculus::CTI },
    { A_CHAR("+"), Types::Boolean  , Types::Void    , CALCULUS_CALLBACK(    pos_B ), Types::Integer, Calculus::CTI },
    { A_CHAR("-"), Types::Integer  , Types::Void    , CALCULUS_CALLBACK(    neg_I ), Types::Integer, Calculus::CTI },
    { A_CHAR("-"), Types::Float    , Types::Void    , CALCULUS_CALLBACK(    neg_F ), Types::Float  , Calculus::CTI },
    { A_CHAR("-"), Types::Boolean  , Types::Void    , CALCULUS_CALLBACK(    neg_B ), Types::Integer, Calculus::CTI },
    { A_CHAR("!"), Types::Boolean  , Types::Void    , CALCULUS_CALLBACK(boolNot_B ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!"), Types::Integer  , Types::Void    , CALCULUS_CALLBACK(boolNot_I ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!"), Types::Float    , Types::Void    , CALCULUS_CALLBACK(boolNot_F ), Types::Boolean, Calculus::CTI },
    { A_CHAR("~"), Types::Integer  , Types::Void    , CALCULUS_CALLBACK(   bitNot ), Types::Integer, Calculus::CTI },

    // binary operators
    { A_CHAR("*" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  mul_BB  ), Types::Integer, Calculus::CTI },
    { A_CHAR("*" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  mul_BI  ), Types::Integer, Calculus::CTI },
    { A_CHAR("*" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  mul_BF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("*" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  mul_IB  ), Types::Integer, Calculus::CTI },
    { A_CHAR("*" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  mul_II  ), Types::Integer, Calculus::CTI },
    { A_CHAR("*" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  mul_IF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("*" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  mul_FB  ), Types::Float  , Calculus::CTI },
    { A_CHAR("*" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  mul_FI  ), Types::Float  , Calculus::CTI },
    { A_CHAR("*" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  mul_FF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("/" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  div_BI  ), Types::Integer, Calculus::CTI },
    { A_CHAR("/" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  div_BF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("/" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  div_II  ), Types::Integer, Calculus::CTI },
    { A_CHAR("/" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  div_IF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("/" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  div_FI  ), Types::Float  , Calculus::CTI },
    { A_CHAR("/" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  div_FF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("%" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  mod_BI  ), Types::Integer, Calculus::CTI },
    { A_CHAR("%" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  mod_BF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("%" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  mod_II  ), Types::Integer, Calculus::CTI },
    { A_CHAR("%" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  mod_IF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("%" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  mod_FI  ), Types::Float  , Calculus::CTI },
    { A_CHAR("%" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  mod_FF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("+" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  add_BB  ), Types::Integer, Calculus::CTI },
    { A_CHAR("+" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  add_BI  ), Types::Integer, Calculus::CTI },
    { A_CHAR("+" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  add_BF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("+" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  add_IB  ), Types::Integer, Calculus::CTI },
    { A_CHAR("+" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  add_II  ), Types::Integer, Calculus::CTI },
    { A_CHAR("+" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  add_IF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("+" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  add_FB  ), Types::Float  , Calculus::CTI },
    { A_CHAR("+" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  add_FI  ), Types::Float  , Calculus::CTI },
    { A_CHAR("+" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  add_FF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("-" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  sub_BB  ), Types::Integer, Calculus::CTI },
    { A_CHAR("-" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  sub_BI  ), Types::Integer, Calculus::CTI },
    { A_CHAR("-" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  sub_BF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("-" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  sub_IB  ), Types::Integer, Calculus::CTI },
    { A_CHAR("-" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  sub_II  ), Types::Integer, Calculus::CTI },
    { A_CHAR("-" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  sub_IF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("-" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  sub_FB  ), Types::Float  , Calculus::CTI },
    { A_CHAR("-" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  sub_FI  ), Types::Float  , Calculus::CTI },
    { A_CHAR("-" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  sub_FF  ), Types::Float  , Calculus::CTI },
    { A_CHAR("<<"), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  shfL_BI ), Types::Integer, Calculus::CTI },
    { A_CHAR("<<"), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  shfL_IB ), Types::Integer, Calculus::CTI },
    { A_CHAR("<<"), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  shfL_II ), Types::Integer, Calculus::CTI },
    { A_CHAR(">>"), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  shfR_BI ), Types::Integer, Calculus::CTI },
    { A_CHAR(">>"), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  shfR_IB ), Types::Integer, Calculus::CTI },
    { A_CHAR(">>"), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  shfR_II ), Types::Integer, Calculus::CTI },
    { A_CHAR("<" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(    sm_BB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(    sm_BI ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(    sm_BF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(    sm_IB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(    sm_II ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(    sm_IF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(    sm_FB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(    sm_FI ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(    sm_FF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<="), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  smeq_BB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<="), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  smeq_BI ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<="), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  smeq_BF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<="), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  smeq_IB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<="), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  smeq_II ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<="), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  smeq_IF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<="), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  smeq_FB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<="), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  smeq_FI ), Types::Boolean, Calculus::CTI },
    { A_CHAR("<="), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  smeq_FF ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(    gt_BB ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(    gt_BI ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(    gt_BF ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(    gt_IB ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(    gt_II ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(    gt_IF ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(    gt_FB ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(    gt_FI ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(    gt_FF ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">="), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  gteq_BB ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">="), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  gteq_BI ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">="), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  gteq_BF ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">="), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  gteq_IB ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">="), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  gteq_II ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">="), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  gteq_IF ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">="), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  gteq_FB ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">="), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  gteq_FI ), Types::Boolean, Calculus::CTI },
    { A_CHAR(">="), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  gteq_FF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("=="), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(    eq_BB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("=="), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(    eq_BI ), Types::Boolean, Calculus::CTI },
    { A_CHAR("=="), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(    eq_BF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("=="), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(    eq_IB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("=="), Types::Integer , Types::Integer , CALCULUS_CALLBACK(    eq_II ), Types::Boolean, Calculus::CTI },
    { A_CHAR("=="), Types::Integer , Types::Float   , CALCULUS_CALLBACK(    eq_IF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("=="), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(    eq_FB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("=="), Types::Float   , Types::Integer , CALCULUS_CALLBACK(    eq_FI ), Types::Boolean, Calculus::CTI },
    { A_CHAR("=="), Types::Float   , Types::Float   , CALCULUS_CALLBACK(    eq_FF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!="), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(   neq_BB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!="), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(   neq_BI ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!="), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(   neq_BF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!="), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(   neq_IB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!="), Types::Integer , Types::Integer , CALCULUS_CALLBACK(   neq_II ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!="), Types::Integer , Types::Float   , CALCULUS_CALLBACK(   neq_IF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!="), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(   neq_FB ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!="), Types::Float   , Types::Integer , CALCULUS_CALLBACK(   neq_FI ), Types::Boolean, Calculus::CTI },
    { A_CHAR("!="), Types::Float   , Types::Float   , CALCULUS_CALLBACK(   neq_FF ), Types::Boolean, Calculus::CTI },
    { A_CHAR("&" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(   bitAnd ), Types::Integer, Calculus::CTI },
    { A_CHAR("^" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(   bitXOr ), Types::Integer, Calculus::CTI },
    { A_CHAR("|" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(   bitOr  ), Types::Integer, Calculus::CTI },
    { A_CHAR("&&"), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(boolAnd_BB), Types::Boolean, Calculus::CTI },
    { A_CHAR("&&"), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(boolAnd_BI), Types::Boolean, Calculus::CTI },
    { A_CHAR("&&"), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(boolAnd_BF), Types::Boolean, Calculus::CTI },
    { A_CHAR("&&"), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(boolAnd_IB), Types::Boolean, Calculus::CTI },
    { A_CHAR("&&"), Types::Integer , Types::Integer , CALCULUS_CALLBACK(boolAnd_II), Types::Boolean, Calculus::CTI },
    { A_CHAR("&&"), Types::Integer , Types::Float   , CALCULUS_CALLBACK(boolAnd_IF), Types::Boolean, Calculus::CTI },
    { A_CHAR("&&"), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(boolAnd_FB), Types::Boolean, Calculus::CTI },
    { A_CHAR("&&"), Types::Float   , Types::Integer , CALCULUS_CALLBACK(boolAnd_FI), Types::Boolean, Calculus::CTI },
    { A_CHAR("&&"), Types::Float   , Types::Float   , CALCULUS_CALLBACK(boolAnd_FF), Types::Boolean, Calculus::CTI },
    { A_CHAR("||"), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK( boolOr_BB), Types::Boolean, Calculus::CTI },
    { A_CHAR("||"), Types::Boolean , Types::Integer , CALCULUS_CALLBACK( boolOr_BI), Types::Boolean, Calculus::CTI },
    { A_CHAR("||"), Types::Boolean , Types::Float   , CALCULUS_CALLBACK( boolOr_BF), Types::Boolean, Calculus::CTI },
    { A_CHAR("||"), Types::Integer , Types::Boolean , CALCULUS_CALLBACK( boolOr_IB), Types::Boolean, Calculus::CTI },
    { A_CHAR("||"), Types::Integer , Types::Integer , CALCULUS_CALLBACK( boolOr_II), Types::Boolean, Calculus::CTI },
    { A_CHAR("||"), Types::Integer , Types::Float   , CALCULUS_CALLBACK( boolOr_IF), Types::Boolean, Calculus::CTI },
    { A_CHAR("||"), Types::Float   , Types::Boolean , CALCULUS_CALLBACK( boolOr_FB), Types::Boolean, Calculus::CTI },
    { A_CHAR("||"), Types::Float   , Types::Integer , CALCULUS_CALLBACK( boolOr_FI), Types::Boolean, Calculus::CTI },
    { A_CHAR("||"), Types::Float   , Types::Float   , CALCULUS_CALLBACK( boolOr_FF), Types::Boolean, Calculus::CTI },
};

Calculus::OperatorAliasTableEntry  bitwiseOpsAliasBooleanOps[] =
{
    { A_CHAR("&"), Types::Boolean, Types::Boolean ,A_CHAR("&&") },
    { A_CHAR("&"), Types::Boolean, Types::Integer ,A_CHAR("&&") },
    { A_CHAR("&"), Types::Boolean, Types::Float   ,A_CHAR("&&") },
    { A_CHAR("&"), Types::Integer, Types::Boolean ,A_CHAR("&&") },
    { A_CHAR("&"), Types::Float  , Types::Boolean ,A_CHAR("&&") },
    { A_CHAR("|"), Types::Boolean, Types::Boolean ,A_CHAR("||") },
    { A_CHAR("|"), Types::Boolean, Types::Integer ,A_CHAR("||") },
    { A_CHAR("|"), Types::Boolean, Types::Float   ,A_CHAR("||") },
    { A_CHAR("|"), Types::Integer, Types::Boolean ,A_CHAR("||") },
    { A_CHAR("|"), Types::Float  , Types::Boolean ,A_CHAR("||") },
};

Calculus::BinaryOpOptimizationsTableEntry  binaryOperatorOptimizations[] =
{
    // optimizations with LEFT side constant value
    { A_CHAR("||"), lang::Side::Left , bool_true , Types::Boolean,    bool_true  },
    { A_CHAR("||"), lang::Side::Left , bool_false, Types::Boolean,    identity   },
    { A_CHAR("&&"), lang::Side::Left , bool_true , Types::Boolean,    identity   },
    { A_CHAR("&&"), lang::Side::Left , bool_false, Types::Boolean,    bool_false },

    { A_CHAR("+") , lang::Side::Left , int_0     , Types::Integer,    identity   },
    { A_CHAR("+") , lang::Side::Left , float_0   , Types::Float  ,    identity   },
    { A_CHAR("-") , lang::Side::Left , int_0     , Types::Integer,    identity   },
    { A_CHAR("-") , lang::Side::Left , float_0   , Types::Float  ,    identity   },

    { A_CHAR("*") , lang::Side::Left , int_0     , Types::Integer,    int_0      },
    { A_CHAR("*") , lang::Side::Left , int_1     , Types::Integer,    identity   },
    { A_CHAR("*") , lang::Side::Left , float_0   , Types::Float  ,    float_0    },
    { A_CHAR("*") , lang::Side::Left , float_1   , Types::Float  ,    identity   },


    // optimizations with RIGHT side constant value (repeat from above)
    { A_CHAR("||"), lang::Side::Right, bool_true , Types::Boolean,    bool_true  },
    { A_CHAR("||"), lang::Side::Right, bool_false, Types::Boolean,    identity   },
    { A_CHAR("&&"), lang::Side::Right, bool_true , Types::Boolean,    identity   },
    { A_CHAR("&&"), lang::Side::Right, bool_false, Types::Boolean,    bool_false },

    { A_CHAR("+") , lang::Side::Right, int_0     , Types::Integer,    identity   },
    { A_CHAR("+") , lang::Side::Right, float_0   , Types::Float  ,    identity   },
    { A_CHAR("-") , lang::Side::Right, int_0     , Types::Integer,    identity   },
    { A_CHAR("-") , lang::Side::Right, float_0   , Types::Float  ,    identity   },

    { A_CHAR("*") , lang::Side::Right, int_0     , Types::Integer,    int_0      },
    { A_CHAR("*") , lang::Side::Right, int_1     , Types::Integer,    identity   },
    { A_CHAR("*") , lang::Side::Right, float_0   , Types::Float  ,    float_0    },
    { A_CHAR("*") , lang::Side::Right, float_1   , Types::Float  ,    identity   },

    // further optimizations with RIGHT side constant value (not available for left-side)
    { A_CHAR("/") , lang::Side::Right,    int_1  , Types::Integer,    identity   },
    { A_CHAR("/") , lang::Side::Right,    int_1  , Types::Float  ,    identity   },
    { A_CHAR("/") , lang::Side::Right,  float_1  , Types::Float  ,    identity   },

    { A_CHAR("%") , lang::Side::Right,    int_1  , Types::Integer,    identity   },
    { A_CHAR("%") , lang::Side::Right,    int_1  , Types::Float  ,    identity   },
    { A_CHAR("%") , lang::Side::Right,  float_1  , Types::Float  ,    identity   },
};


} // anonymous namespace


// #################################################################################################
// ### Arithmetics - Constructor. Creates the hash map
// #################################################################################################
Arithmetics::Arithmetics( Compiler& compiler )
: Calculus( "ALib Arithmetics", compiler, CompilePriorities::Arithmetics )
{
    constexpr int tableSize= 9;
    Token functionNames[tableSize];
    Token::LoadResourcedTokens( EXPRESSIONS, "CPA", functionNames
                                ALIB_DBG(,tableSize)                                        );
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    Token* descriptor= functionNames;
    ConstantIdentifiers=
    {
        { *descriptor++, constTrue   },
        { *descriptor++, constFalse   },
        { *descriptor++, constTrue    },
        { *descriptor++, constFalse   },
        { *descriptor++, constTrue    },
        { *descriptor++, constFalse   },
    };

    Functions=
    {
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Var), CALCULUS_CALLBACK(ToBoolean), &Types::Boolean , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::B  ), CALCULUS_CALLBACK(toInt_B  ), &Types::Integer , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I  ), CALCULUS_CALLBACK(toInt_I  ), &Types::Integer , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F  ), CALCULUS_CALLBACK(toInt_F  ), &Types::Integer , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::B  ), CALCULUS_CALLBACK(toFloat_B), &Types::Float   , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I  ), CALCULUS_CALLBACK(toFloat_I), &Types::Float   , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F  ), CALCULUS_CALLBACK(toFloat_F), &Types::Float   , CTI },
    };

    AddOperators( OperatorTable );

    if( HasBits(compiler.CfgCompilation, Compilation::AllowBitwiseBooleanOperators ) )
    {
        AddOperatorAliases( bitwiseOpsAliasBooleanOps );
        AddOperatorAlias  ( A_CHAR("~"), Types::Boolean, Types::Void, A_CHAR("!") );
    }

    AddBinaryOpOptimizations( binaryOperatorOptimizations);

    ALIB_ASSERT_ERROR( descriptor - functionNames == tableSize, "EXPR",
                       "Descriptor table size mismatch: Consumed {} descriptors, {} available.",
                       descriptor - functionNames, tableSize)
    ALIB_WARNINGS_RESTORE
}

bool Arithmetics::TryCompilation( CIFunction& ciFunction )
{
    if( Calculus::TryCompilation( ciFunction ) )
        return true;

    if( ciFunction.QtyArgs() == 1 && ciFunction.ArgsBegin->IsArray() )
    {
        constexpr int tableSize= 1;
        Token functionNames[tableSize];

        Token::LoadResourcedTokens( EXPRESSIONS, "CPALen", functionNames
                                    ALIB_DBG(,tableSize)                                        );

        if( functionNames[0].Match( ciFunction.Name ) )
        {
            ciFunction.Name.Reset( functionNames[0] );
ALIB_DBG(   ciFunction.DbgCallbackName  = "arrLen";  )

            // for constants, the callback might b invoked right away (optimizing call out)
            if( ciFunction.AllArgsAreConst )
            {
                ciFunction.TypeOrValue      = ciFunction.ArgsBegin->UnboxLength();
                return true;
            }

            ciFunction.Callback             = arrLen;
            ciFunction.TypeOrValue          = Types::Integer;

            return true;
        }
    }

    return false;
}

}}} // namespace [alib::expressions::detail]

#undef BOL
#undef INT
#undef FLT
#undef FUNC
#undef FUNC
#undef UN_MAP_ENTRY
#undef BIN_MAP_ENTRY
#undef BIN_ALIAS_ENTRY


#endif //ALIB_DOX
