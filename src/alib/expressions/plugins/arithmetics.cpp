// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

//! @cond NO_DOX

#include "alib/alib.hpp"
#include "alib/strings/substring.hpp"

#include "arithmetics.hpp"
#include "../compiler.hpp"
#include <math.h>

#define ARG0           (*args)
#define ARG1           (*(args+1))
#define BOL(box)       (box).Unbox<bool     >()
#define INT(box)       (box).Unbox<boxed_int>()
#define FLT(box)       (box).Unbox<double   >()
#define FUNC( name,...) Box name( Scope& scope,                                                  \
                                  ArgIterator  args,                                            \
                                  ArgIterator  end    )                                         \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }

namespace aworx { namespace lib { namespace expressions { namespace plugins {


FUNC( ToBoolean , return  args->Invoke<IIsTrue,bool>();  )

namespace {

// function signatures
std::vector<Box>  SIG_BOL;
std::vector<Box>  SIG_INT;
std::vector<Box>  SIG_FLT;
std::vector<Box>  SIG_VAR;

// #################################################################################################
// ### Arithmetics - constants
// #################################################################################################

Box constTrue;
Box constFalse;
Box identity;
Box bool_false;
Box bool_true;
Box int_0;
Box int_1;
Box float_0;
Box float_1;


// #################################################################################################
// ### Arithmetics - Functions
// #################################################################################################
FUNC( toInt_B  , return static_cast<int   >(BOL(ARG0));       )
FUNC( toInt_I  , return  ARG0;                                )
FUNC( toInt_F  , return static_cast<int   >(FLT(ARG0));       )
FUNC( toFloat_B, return static_cast<double>(BOL(ARG0));       )
FUNC( toFloat_I, return static_cast<double>(INT(ARG0));       )
FUNC( toFloat_F, return  ARG0;                                )

FUNC( arrLen,    return  ARG0.Length();                       )


// #################################################################################################
// ### Arithmetics - unary operations
// #################################################################################################

FUNC( pos      , return  ARG0;                                )
FUNC( pos_B    , return   static_cast<int>(BOL(ARG0));        )
FUNC( neg_B    , return  -static_cast<int>(BOL(ARG0));        )
FUNC( neg_I    , return  -INT(ARG0);                          )
FUNC( neg_F    , return  -FLT(ARG0);                          )
FUNC( bitNot   , return  ~INT(ARG0);                          )
FUNC( boolNot_B, return  !BOL(ARG0);                          )
FUNC( boolNot_I, return   INT(ARG0) == 0;                     )
FUNC( boolNot_F, return   FLT(ARG0)  == 0.0;                  )

constexpr Calculus::UnaryOpTableEntry  unaryTable[] =
{
    { ASTR("+"), Types::Integer, CALCULUS_CALLBACK(       pos ), Types::Integer  , Calculus::CTI },
    { ASTR("+"), Types::Float  , CALCULUS_CALLBACK(       pos ), Types::Float    , Calculus::CTI },
    { ASTR("+"), Types::Boolean, CALCULUS_CALLBACK(     pos_B ), Types::Integer  , Calculus::CTI },
    { ASTR("-"), Types::Integer, CALCULUS_CALLBACK(     neg_I ), Types::Integer  , Calculus::CTI },
    { ASTR("-"), Types::Float  , CALCULUS_CALLBACK(     neg_F ), Types::Float    , Calculus::CTI },
    { ASTR("-"), Types::Boolean, CALCULUS_CALLBACK(     neg_B ), Types::Integer  , Calculus::CTI },
    { ASTR("!"), Types::Boolean, CALCULUS_CALLBACK( boolNot_B ), Types::Boolean  , Calculus::CTI },
    { ASTR("!"), Types::Integer, CALCULUS_CALLBACK( boolNot_I ), Types::Boolean  , Calculus::CTI },
    { ASTR("!"), Types::Float  , CALCULUS_CALLBACK( boolNot_F ), Types::Boolean  , Calculus::CTI },
    { ASTR("~"), Types::Integer, CALCULUS_CALLBACK(    bitNot ), Types::Integer  , Calculus::CTI },
};



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
FUNC(     mul_IF, return  INT(ARG0)  *   FLT(ARG1) ; )
FUNC(     mul_FB, return  FLT(ARG0)  *   BOL(ARG1) ; )
FUNC(     mul_FI, return  FLT(ARG0)  *   INT(ARG1) ; )
FUNC(     mul_FF, return  FLT(ARG0)  *   FLT(ARG1) ; )
FUNC(     div_BB, return  BOL(ARG0)  /   BOL(ARG1) ; )
FUNC(     div_BI, return  BOL(ARG0)  /   INT(ARG1) ; )
FUNC(     div_BF, return  BOL(ARG0)  /   FLT(ARG1) ; )
FUNC(     div_IB, return  INT(ARG0)  /   BOL(ARG1) ; )
FUNC(     div_II, return  INT(ARG0)  /   INT(ARG1) ; )
FUNC(     div_IF, return  INT(ARG0)  /   FLT(ARG1) ; )
FUNC(     div_FB, return  FLT(ARG0)  /   BOL(ARG1) ; )
FUNC(     div_FI, return  FLT(ARG0)  /   INT(ARG1) ; )
FUNC(     div_FF, return  FLT(ARG0)  /   FLT(ARG1) ; )
FUNC(     mod_BB, return  BOL(ARG0)  %   BOL(ARG1) ; )
FUNC(     mod_BI, return  BOL(ARG0)  %   INT(ARG1) ; )
FUNC(     mod_IB, return  INT(ARG0)  %   BOL(ARG1) ; )
FUNC(     mod_II, return  INT(ARG0)  %   INT(ARG1) ; )
FUNC(     mod_BF, return fmod(BOL(ARG0), FLT(ARG1)); )
FUNC(     mod_FB, return fmod(FLT(ARG0), BOL(ARG1)); )
FUNC(     mod_FI, return fmod(FLT(ARG0), INT(ARG1)); )
FUNC(     mod_IF, return fmod(INT(ARG0), FLT(ARG1)); )
FUNC(     mod_FF, return fmod(FLT(ARG0), FLT(ARG1)); )

FUNC(     add_BB, return  BOL(ARG0)  +   BOL(ARG1) ; )
FUNC(     add_BI, return  BOL(ARG0)  +   INT(ARG1) ; )
FUNC(     add_BF, return  BOL(ARG0)  +   FLT(ARG1) ; )
FUNC(     add_IB, return  INT(ARG0)  +   BOL(ARG1) ; )
FUNC(     add_II, return  INT(ARG0)  +   INT(ARG1) ; )
FUNC(     add_IF, return  INT(ARG0)  +   FLT(ARG1) ; )
FUNC(     add_FB, return  FLT(ARG0)  +   BOL(ARG1) ; )
FUNC(     add_FI, return  FLT(ARG0)  +   INT(ARG1) ; )
FUNC(     add_FF, return  FLT(ARG0)  +   FLT(ARG1) ; )
FUNC(     sub_BB, return  BOL(ARG0)  -   BOL(ARG1) ; )
FUNC(     sub_BI, return  BOL(ARG0)  -   INT(ARG1) ; )
FUNC(     sub_BF, return  BOL(ARG0)  -   FLT(ARG1) ; )
FUNC(     sub_IB, return  INT(ARG0)  -   BOL(ARG1) ; )
FUNC(     sub_II, return  INT(ARG0)  -   INT(ARG1) ; )
FUNC(     sub_IF, return  INT(ARG0)  -   FLT(ARG1) ; )
FUNC(     sub_FB, return  FLT(ARG0)  -   BOL(ARG1) ; )
FUNC(     sub_FI, return  FLT(ARG0)  -   INT(ARG1) ; )
FUNC(     sub_FF, return  FLT(ARG0)  -   FLT(ARG1) ; )

FUNC(    shfL_BI, return  BOL(ARG0)  <<  INT(ARG1) ; )
FUNC(    shfL_IB, return  INT(ARG0)  <<  BOL(ARG1) ; )
FUNC(    shfL_II, return  INT(ARG0)  <<  INT(ARG1) ; )
FUNC(    shfR_BI, return  BOL(ARG0)  <<  INT(ARG1) ; )
FUNC(    shfR_IB, return  INT(ARG0)  <<  BOL(ARG1) ; )
FUNC(    shfR_II, return  INT(ARG0)  >>  INT(ARG1) ; )

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
FUNC(      eq_IF, return  std::fabs( INT(ARG0)  -   FLT(ARG1) ) <= std::numeric_limits<double>::epsilon(); )
FUNC(      eq_FB, return  std::fabs( FLT(ARG0)  -   BOL(ARG1) ) <= std::numeric_limits<double>::epsilon(); )
FUNC(      eq_FI, return  std::fabs( FLT(ARG0)  -   INT(ARG1) ) <= std::numeric_limits<double>::epsilon(); )
FUNC(      eq_FF, return  std::fabs( FLT(ARG0)  -   FLT(ARG1) ) <= std::numeric_limits<double>::epsilon(); )



FUNC(     neq_BB, return             BOL(ARG0)  !=  BOL(ARG1) ; )
FUNC(     neq_BI, return             BOL(ARG0)  !=  INT(ARG1) ; )
FUNC(     neq_BF, return  std::fabs( BOL(ARG0)  -   FLT(ARG1) ) >  std::numeric_limits<double>::epsilon(); )
FUNC(     neq_IB, return             INT(ARG0)  !=  BOL(ARG1) ; )
FUNC(     neq_II, return             INT(ARG0)  !=  INT(ARG1) ; )
FUNC(     neq_IF, return  std::fabs( INT(ARG0)  -   INT(ARG1) ) >  std::numeric_limits<double>::epsilon(); )
FUNC(     neq_FB, return  std::fabs( FLT(ARG0)  -   BOL(ARG1) ) >  std::numeric_limits<double>::epsilon(); )
FUNC(     neq_FI, return  std::fabs( FLT(ARG0)  -   INT(ARG1) ) >  std::numeric_limits<double>::epsilon(); )
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

constexpr Calculus::BinaryOpTableEntry  binaryOpTable[] =
{
    { ASTR("*" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  mul_BB  ), Types::Integer, Calculus::CTI },
    { ASTR("*" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  mul_BI  ), Types::Integer, Calculus::CTI },
    { ASTR("*" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  mul_BF  ), Types::Float  , Calculus::CTI },
    { ASTR("*" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  mul_IB  ), Types::Integer, Calculus::CTI },
    { ASTR("*" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  mul_II  ), Types::Integer, Calculus::CTI },
    { ASTR("*" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  mul_IF  ), Types::Float  , Calculus::CTI },
    { ASTR("*" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  mul_FB  ), Types::Float  , Calculus::CTI },
    { ASTR("*" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  mul_FI  ), Types::Float  , Calculus::CTI },
    { ASTR("*" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  mul_FF  ), Types::Float  , Calculus::CTI },
    { ASTR("/" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  div_BB  ), Types::Integer, Calculus::CTI },
    { ASTR("/" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  div_BI  ), Types::Integer, Calculus::CTI },
    { ASTR("/" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  div_BF  ), Types::Float  , Calculus::CTI },
    { ASTR("/" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  div_IB  ), Types::Integer, Calculus::CTI },
    { ASTR("/" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  div_II  ), Types::Integer, Calculus::CTI },
    { ASTR("/" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  div_IF  ), Types::Float  , Calculus::CTI },
    { ASTR("/" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  div_FB  ), Types::Float  , Calculus::CTI },
    { ASTR("/" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  div_FI  ), Types::Float  , Calculus::CTI },
    { ASTR("/" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  div_FF  ), Types::Float  , Calculus::CTI },
    { ASTR("%" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  mod_BB  ), Types::Integer, Calculus::CTI },
    { ASTR("%" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  mod_BI  ), Types::Integer, Calculus::CTI },
    { ASTR("%" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  mod_BF  ), Types::Float  , Calculus::CTI },
    { ASTR("%" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  mod_IB  ), Types::Integer, Calculus::CTI },
    { ASTR("%" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  mod_II  ), Types::Integer, Calculus::CTI },
    { ASTR("%" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  mod_IF  ), Types::Float  , Calculus::CTI },
    { ASTR("%" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  mod_FB  ), Types::Float  , Calculus::CTI },
    { ASTR("%" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  mod_FI  ), Types::Float  , Calculus::CTI },
    { ASTR("%" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  mod_FF  ), Types::Float  , Calculus::CTI },
    { ASTR("+" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  add_BB  ), Types::Integer, Calculus::CTI },
    { ASTR("+" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  add_BI  ), Types::Integer, Calculus::CTI },
    { ASTR("+" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  add_BF  ), Types::Float  , Calculus::CTI },
    { ASTR("+" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  add_IB  ), Types::Integer, Calculus::CTI },
    { ASTR("+" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  add_II  ), Types::Integer, Calculus::CTI },
    { ASTR("+" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  add_IF  ), Types::Float  , Calculus::CTI },
    { ASTR("+" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  add_FB  ), Types::Float  , Calculus::CTI },
    { ASTR("+" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  add_FI  ), Types::Float  , Calculus::CTI },
    { ASTR("+" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  add_FF  ), Types::Float  , Calculus::CTI },
    { ASTR("-" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  sub_BB  ), Types::Integer, Calculus::CTI },
    { ASTR("-" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  sub_BI  ), Types::Integer, Calculus::CTI },
    { ASTR("-" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  sub_BF  ), Types::Float  , Calculus::CTI },
    { ASTR("-" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  sub_IB  ), Types::Integer, Calculus::CTI },
    { ASTR("-" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  sub_II  ), Types::Integer, Calculus::CTI },
    { ASTR("-" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  sub_IF  ), Types::Float  , Calculus::CTI },
    { ASTR("-" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  sub_FB  ), Types::Float  , Calculus::CTI },
    { ASTR("-" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  sub_FI  ), Types::Float  , Calculus::CTI },
    { ASTR("-" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  sub_FF  ), Types::Float  , Calculus::CTI },
    { ASTR("<<"), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  shfL_BI ), Types::Integer, Calculus::CTI },
    { ASTR("<<"), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  shfL_IB ), Types::Integer, Calculus::CTI },
    { ASTR("<<"), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  shfL_II ), Types::Integer, Calculus::CTI },
    { ASTR(">>"), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  shfR_BI ), Types::Integer, Calculus::CTI },
    { ASTR(">>"), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  shfR_IB ), Types::Integer, Calculus::CTI },
    { ASTR(">>"), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  shfR_II ), Types::Integer, Calculus::CTI },
    { ASTR("<" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(    sm_BB ), Types::Boolean, Calculus::CTI },
    { ASTR("<" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(    sm_BI ), Types::Boolean, Calculus::CTI },
    { ASTR("<" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(    sm_BF ), Types::Boolean, Calculus::CTI },
    { ASTR("<" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(    sm_IB ), Types::Boolean, Calculus::CTI },
    { ASTR("<" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(    sm_II ), Types::Boolean, Calculus::CTI },
    { ASTR("<" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(    sm_IF ), Types::Boolean, Calculus::CTI },
    { ASTR("<" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(    sm_FB ), Types::Boolean, Calculus::CTI },
    { ASTR("<" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(    sm_FI ), Types::Boolean, Calculus::CTI },
    { ASTR("<" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(    sm_FF ), Types::Boolean, Calculus::CTI },
    { ASTR("<="), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  smeq_BB ), Types::Boolean, Calculus::CTI },
    { ASTR("<="), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  smeq_BI ), Types::Boolean, Calculus::CTI },
    { ASTR("<="), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  smeq_BF ), Types::Boolean, Calculus::CTI },
    { ASTR("<="), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  smeq_IB ), Types::Boolean, Calculus::CTI },
    { ASTR("<="), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  smeq_II ), Types::Boolean, Calculus::CTI },
    { ASTR("<="), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  smeq_IF ), Types::Boolean, Calculus::CTI },
    { ASTR("<="), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  smeq_FB ), Types::Boolean, Calculus::CTI },
    { ASTR("<="), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  smeq_FI ), Types::Boolean, Calculus::CTI },
    { ASTR("<="), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  smeq_FF ), Types::Boolean, Calculus::CTI },
    { ASTR(">" ), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(    gt_BB ), Types::Boolean, Calculus::CTI },
    { ASTR(">" ), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(    gt_BI ), Types::Boolean, Calculus::CTI },
    { ASTR(">" ), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(    gt_BF ), Types::Boolean, Calculus::CTI },
    { ASTR(">" ), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(    gt_IB ), Types::Boolean, Calculus::CTI },
    { ASTR(">" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(    gt_II ), Types::Boolean, Calculus::CTI },
    { ASTR(">" ), Types::Integer , Types::Float   , CALCULUS_CALLBACK(    gt_IF ), Types::Boolean, Calculus::CTI },
    { ASTR(">" ), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(    gt_FB ), Types::Boolean, Calculus::CTI },
    { ASTR(">" ), Types::Float   , Types::Integer , CALCULUS_CALLBACK(    gt_FI ), Types::Boolean, Calculus::CTI },
    { ASTR(">" ), Types::Float   , Types::Float   , CALCULUS_CALLBACK(    gt_FF ), Types::Boolean, Calculus::CTI },
    { ASTR(">="), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(  gteq_BB ), Types::Boolean, Calculus::CTI },
    { ASTR(">="), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(  gteq_BI ), Types::Boolean, Calculus::CTI },
    { ASTR(">="), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(  gteq_BF ), Types::Boolean, Calculus::CTI },
    { ASTR(">="), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(  gteq_IB ), Types::Boolean, Calculus::CTI },
    { ASTR(">="), Types::Integer , Types::Integer , CALCULUS_CALLBACK(  gteq_II ), Types::Boolean, Calculus::CTI },
    { ASTR(">="), Types::Integer , Types::Float   , CALCULUS_CALLBACK(  gteq_IF ), Types::Boolean, Calculus::CTI },
    { ASTR(">="), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(  gteq_FB ), Types::Boolean, Calculus::CTI },
    { ASTR(">="), Types::Float   , Types::Integer , CALCULUS_CALLBACK(  gteq_FI ), Types::Boolean, Calculus::CTI },
    { ASTR(">="), Types::Float   , Types::Float   , CALCULUS_CALLBACK(  gteq_FF ), Types::Boolean, Calculus::CTI },
    { ASTR("=="), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(    eq_BB ), Types::Boolean, Calculus::CTI },
    { ASTR("=="), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(    eq_BI ), Types::Boolean, Calculus::CTI },
    { ASTR("=="), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(    eq_BF ), Types::Boolean, Calculus::CTI },
    { ASTR("=="), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(    eq_IB ), Types::Boolean, Calculus::CTI },
    { ASTR("=="), Types::Integer , Types::Integer , CALCULUS_CALLBACK(    eq_II ), Types::Boolean, Calculus::CTI },
    { ASTR("=="), Types::Integer , Types::Float   , CALCULUS_CALLBACK(    eq_IF ), Types::Boolean, Calculus::CTI },
    { ASTR("=="), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(    eq_FB ), Types::Boolean, Calculus::CTI },
    { ASTR("=="), Types::Float   , Types::Integer , CALCULUS_CALLBACK(    eq_FI ), Types::Boolean, Calculus::CTI },
    { ASTR("=="), Types::Float   , Types::Float   , CALCULUS_CALLBACK(    eq_FF ), Types::Boolean, Calculus::CTI },
    { ASTR("!="), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(   neq_BB ), Types::Boolean, Calculus::CTI },
    { ASTR("!="), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(   neq_BI ), Types::Boolean, Calculus::CTI },
    { ASTR("!="), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(   neq_BF ), Types::Boolean, Calculus::CTI },
    { ASTR("!="), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(   neq_IB ), Types::Boolean, Calculus::CTI },
    { ASTR("!="), Types::Integer , Types::Integer , CALCULUS_CALLBACK(   neq_II ), Types::Boolean, Calculus::CTI },
    { ASTR("!="), Types::Integer , Types::Float   , CALCULUS_CALLBACK(   neq_IF ), Types::Boolean, Calculus::CTI },
    { ASTR("!="), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(   neq_FB ), Types::Boolean, Calculus::CTI },
    { ASTR("!="), Types::Float   , Types::Integer , CALCULUS_CALLBACK(   neq_FI ), Types::Boolean, Calculus::CTI },
    { ASTR("!="), Types::Float   , Types::Float   , CALCULUS_CALLBACK(   neq_FF ), Types::Boolean, Calculus::CTI },
    { ASTR("&" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(   bitAnd ), Types::Integer, Calculus::CTI },
    { ASTR("^" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(   bitXOr ), Types::Integer, Calculus::CTI },
    { ASTR("|" ), Types::Integer , Types::Integer , CALCULUS_CALLBACK(   bitOr  ), Types::Integer, Calculus::CTI },
    { ASTR("&&"), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK(boolAnd_BB), Types::Boolean, Calculus::CTI },
    { ASTR("&&"), Types::Boolean , Types::Integer , CALCULUS_CALLBACK(boolAnd_BI), Types::Boolean, Calculus::CTI },
    { ASTR("&&"), Types::Boolean , Types::Float   , CALCULUS_CALLBACK(boolAnd_BF), Types::Boolean, Calculus::CTI },
    { ASTR("&&"), Types::Integer , Types::Boolean , CALCULUS_CALLBACK(boolAnd_IB), Types::Boolean, Calculus::CTI },
    { ASTR("&&"), Types::Integer , Types::Integer , CALCULUS_CALLBACK(boolAnd_II), Types::Boolean, Calculus::CTI },
    { ASTR("&&"), Types::Integer , Types::Float   , CALCULUS_CALLBACK(boolAnd_IF), Types::Boolean, Calculus::CTI },
    { ASTR("&&"), Types::Float   , Types::Boolean , CALCULUS_CALLBACK(boolAnd_FB), Types::Boolean, Calculus::CTI },
    { ASTR("&&"), Types::Float   , Types::Integer , CALCULUS_CALLBACK(boolAnd_FI), Types::Boolean, Calculus::CTI },
    { ASTR("&&"), Types::Float   , Types::Float   , CALCULUS_CALLBACK(boolAnd_FF), Types::Boolean, Calculus::CTI },
    { ASTR("||"), Types::Boolean , Types::Boolean , CALCULUS_CALLBACK( boolOr_BB), Types::Boolean, Calculus::CTI },
    { ASTR("||"), Types::Boolean , Types::Integer , CALCULUS_CALLBACK( boolOr_BI), Types::Boolean, Calculus::CTI },
    { ASTR("||"), Types::Boolean , Types::Float   , CALCULUS_CALLBACK( boolOr_BF), Types::Boolean, Calculus::CTI },
    { ASTR("||"), Types::Integer , Types::Boolean , CALCULUS_CALLBACK( boolOr_IB), Types::Boolean, Calculus::CTI },
    { ASTR("||"), Types::Integer , Types::Integer , CALCULUS_CALLBACK( boolOr_II), Types::Boolean, Calculus::CTI },
    { ASTR("||"), Types::Integer , Types::Float   , CALCULUS_CALLBACK( boolOr_IF), Types::Boolean, Calculus::CTI },
    { ASTR("||"), Types::Float   , Types::Boolean , CALCULUS_CALLBACK( boolOr_FB), Types::Boolean, Calculus::CTI },
    { ASTR("||"), Types::Float   , Types::Integer , CALCULUS_CALLBACK( boolOr_FI), Types::Boolean, Calculus::CTI },
    { ASTR("||"), Types::Float   , Types::Float   , CALCULUS_CALLBACK( boolOr_FF), Types::Boolean, Calculus::CTI },
};

constexpr Calculus::BinaryOpAliasTableEntry  bitwiseOpsAliasBooleanOps[] =
{
    { ASTR("&"), Types::Boolean, Types::Boolean ,ASTR("&&") },
    { ASTR("&"), Types::Boolean, Types::Integer ,ASTR("&&") },
    { ASTR("&"), Types::Boolean, Types::Float   ,ASTR("&&") },
    { ASTR("&"), Types::Integer, Types::Boolean ,ASTR("&&") },
    { ASTR("&"), Types::Float  , Types::Boolean ,ASTR("&&") },
    { ASTR("|"), Types::Boolean, Types::Boolean ,ASTR("||") },
    { ASTR("|"), Types::Boolean, Types::Integer ,ASTR("||") },
    { ASTR("|"), Types::Boolean, Types::Float   ,ASTR("||") },
    { ASTR("|"), Types::Integer, Types::Boolean ,ASTR("||") },
    { ASTR("|"), Types::Float  , Types::Boolean ,ASTR("||") },
};

// optimization table with works with the same constant being on both sides, e.g.
//   a * 0 == 0 * a, however
//   a / 1 == a      must be used only on rhs!
constexpr Calculus::BinaryOpOptimizationsTableEntry  bothSides[] =
{
    { ASTR("||"), Types::Boolean, bool_true , bool_true  },
    { ASTR("||"), Types::Boolean, bool_false, identity   },
    { ASTR("&&"), Types::Boolean, bool_true , identity   },
    { ASTR("&&"), Types::Boolean, bool_false, bool_false },

    { ASTR("+") , Types::Integer, int_0     , identity   },
    { ASTR("+") , Types::Float  , float_0   , identity   },
    { ASTR("-") , Types::Integer, int_0     , identity   },
    { ASTR("-") , Types::Float  , float_0   , identity   },

    { ASTR("*") , Types::Integer, int_0     , int_0      },
    { ASTR("*") , Types::Integer, int_1     , identity   },
    { ASTR("*") , Types::Float  , float_0   , float_0    },
    { ASTR("*") , Types::Float  , float_1   , identity   },
};

constexpr Calculus::BinaryOpOptimizationsTableEntry  rhsSide[] =
{
    { ASTR("/") , Types::Integer,    int_1  , identity   },
    { ASTR("/") , Types::Float  ,    int_1  , identity   },
    { ASTR("/") , Types::Float  ,  float_1  , identity   },

    { ASTR("%") , Types::Integer,    int_1  , identity   },
    { ASTR("%") , Types::Float  ,    int_1  , identity   },
    { ASTR("%") , Types::Float  ,  float_1  , identity   },
};


} // anonymous namespace


// #################################################################################################
// ### Arithmetics - Constructor. Creates the hash map
// #################################################################################################
Arithmetics::Arithmetics( Compiler& compiler )
: Calculus( "ALib Arithmetics", compiler )
{
    constTrue = true;    // Initialize constant static boxes. This must not be done
    constFalse= false;   // in the C++ bootstrap code.
    identity  = nullptr;
    bool_false= false;
    bool_true = true;
    int_0     = 0;
    int_1     = 1;
    float_0   = 0.0;
    float_1   = 1.0;


    FunctionNameDescriptor functionNames[9];
    LoadResourcedFunctionDescriptors( lib::EXPRESSIONS, ASTR("Arithmethics"), functionNames);
    FunctionNameDescriptor* descriptor= functionNames;
    ConstantIdentifiers=
    {
        { *descriptor++, constTrue   },
        { *descriptor++, constFalse   },
        { *descriptor++, constTrue    },
        { *descriptor++, constFalse   },
        { *descriptor++, constTrue    },
        { *descriptor++, constFalse   },
    };

    SIG_BOL= { Types::Boolean   };
    SIG_INT= { Types::Integer   };
    SIG_FLT= { Types::Float     };
    SIG_VAR= { nullptr          };

    Functions=
    {
        { *descriptor++, &SIG_VAR, CALCULUS_CALLBACK(ToBoolean), Types::Boolean , CTI },
        { *descriptor  , &SIG_BOL, CALCULUS_CALLBACK(toInt_B  ), Types::Integer , CTI },
        { *descriptor  , &SIG_INT, CALCULUS_CALLBACK(toInt_I  ), Types::Integer , CTI },
        { *descriptor++, &SIG_FLT, CALCULUS_CALLBACK(toInt_F  ), Types::Integer , CTI },
        { *descriptor  , &SIG_BOL, CALCULUS_CALLBACK(toFloat_B), Types::Float   , CTI },
        { *descriptor  , &SIG_INT, CALCULUS_CALLBACK(toFloat_I), Types::Float   , CTI },
        { *descriptor++, &SIG_FLT, CALCULUS_CALLBACK(toFloat_F), Types::Float   , CTI },
    };


    AddUnaryOps ( unaryTable    );
    AddBinaryOps( binaryOpTable );

    if( EnumContains(compiler.CfgCompilation, Compilation::AllowBitwiseBooleanOperations ) )
    {
        AddUnaryOpAlias( ASTR("~"), Types::Boolean, ASTR("!") );
        AddBinaryOpAliases( bitwiseOpsAliasBooleanOps );
    }


    AddBinaryOpOptimizations( bothSides, false );
    AddBinaryOpOptimizations( bothSides, true );
    AddBinaryOpOptimizations( rhsSide  , true );
};

bool Arithmetics::TryCompilation( CIFunction& ciFunction )
{
    if( Calculus::TryCompilation( ciFunction ) )
        return true;

    if( ciFunction.QtyArgs() == 1 && ciFunction.ArgsBegin->IsArray() )
    {
        FunctionNameDescriptor functionNames[1];
        LoadResourcedFunctionDescriptors( lib::EXPRESSIONS, ASTR("Arithmethics2"), functionNames);

        if( MatchFunctionName( functionNames[0], ciFunction.Name ) )
        {
            ciFunction.Name._()._( functionNames[0].Name );
ALIB_DBG(   ciFunction.DbgCallBackName  = "arrLen";  )

            // for constants, the callback might b invoked right away (optimizing cal out)
            if( ciFunction.AllArgsAreConst )
            {
                ciFunction.TypeOrValue      = ciFunction.ArgsBegin->Length();
                return true;
            }

            ciFunction.Callback             = arrLen;
            ciFunction.TypeOrValue          = Types::Integer;

            return true;
        }
    }

    return false;
}



}}}} // namespace [aworx::lib::expressions::detail]

#undef BOL
#undef INT
#undef FLT
#undef FUNC
#undef FUNC
#undef UN_MAP_ENTRY
#undef BIN_MAP_ENTRY
#undef BIN_ALIAS_ENTRY


//! @endcond
