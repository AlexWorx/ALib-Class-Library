// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

//! @cond NO_DOX

#include "alib/alib.hpp"
#include "alib/strings/substring.hpp"

#include "math.hpp"
#include "../compiler.hpp"


#if defined(_MSC_VER)
#   define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <cmath>

#define INT(box)       (box).Unbox<boxed_int>()
#define FLT(box)       (box).Unbox<double   >()
#define FUNC( name,...) Box name( Scope& scope,                                                   \
                                  ArgIterator  args,                                              \
                                  ArgIterator  end    )                                           \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }

namespace aworx { namespace lib { namespace expressions { namespace plugins {

namespace {

// function signatures
std::vector<Box>  SIG_NONE;
std::vector<Box>  SIG_INT;
std::vector<Box>  SIG_INT_INT;
std::vector<Box>  SIG_FLT;
std::vector<Box>  SIG_FLT_FLT;

// #################################################################################################
// ### Math - constants
// #################################################################################################

Box constPI;
Box constE;

// #################################################################################################
// ### Math - functions
// #################################################################################################
FUNC(   max_I    , return  (std::max) (INT(*args),INT(*(args+1))); )
FUNC(   max_F    , return  (::fmax)   (FLT(*args),FLT(*(args+1))); )
FUNC(   min_I    , return  (std::min) (INT(*args),INT(*(args+1))); )
FUNC(   min_F    , return  (::fmin)   (FLT(*args),FLT(*(args+1))); )
FUNC(   abs_I    , return  std::abs   (INT(*args)); )
FUNC(   abs      , return  ::fabs     (FLT(*args)); )
FUNC(   ceil     , return  ::ceil     (FLT(*args)); )
FUNC(   floor    , return  ::floor    (FLT(*args)); )
FUNC(   trunc    , return  ::round    (FLT(*args)); )
FUNC(   round    , return  ::round    (FLT(*args)); )
FUNC(   rint     , return  ::llrint   (FLT(*args)); )
FUNC(   remainder, return  ::remainder(FLT(*args),FLT(*(args+1))); )

FUNC(   rand     , return  static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX); )

FUNC(   sin      , return  ::sin      (FLT(*args)); )
FUNC(  asin      , return  ::asin     (FLT(*args)); )
FUNC(   cos      , return  ::cos      (FLT(*args)); )
FUNC(  acos      , return  ::acos     (FLT(*args)); )
FUNC(   tan      , return  ::tan      (FLT(*args)); )
FUNC(  atan      , return  ::atan     (FLT(*args)); )

FUNC(   sinh     , return  ::sinh     (FLT(*args)); )
FUNC(  asinh     , return  ::asinh    (FLT(*args)); )
FUNC(   cosh     , return  ::cosh     (FLT(*args)); )
FUNC(  acosh     , return  ::acosh    (FLT(*args)); )
FUNC(   tanh     , return  ::tanh     (FLT(*args)); )
FUNC(  atanh     , return  ::atanh    (FLT(*args)); )

FUNC(   exp      , return  ::exp      (FLT(*args)); )
FUNC(   exp2     , return  ::exp2     (FLT(*args)); )
#if defined(_MSC_VER)
   FUNC(exp10    , return  ::pow      (10.0 ,FLT(*args)); )
#else
   FUNC(exp10    , return  ::exp10    (FLT(*args)); )
#endif


FUNC(   log      , return  ::log      (FLT(*args)); )
FUNC(   log2     , return  ::log2     (FLT(*args)); )
#if defined(_MSC_VER)
  FUNC( log10    , return  ::log      (FLT(*args)) / M_LN10; )
#else
  FUNC( log10    , return  ::log10    (FLT(*args)); )
#endif

FUNC(   pow      , return  ::pow      (FLT(*args++),FLT(*args)); )
#if defined(_MSC_VER)
   FUNC(pow10    , return  ::pow      (10.0        ,FLT(*args)); )
#else
   FUNC(pow10    , return  ::exp10    (FLT(*args)); )
#endif
FUNC(   sqrt     , return  ::sqrt     (FLT(*args)); )
FUNC(   cbrt     , return  ::cbrt     (FLT(*args)); )



} // anonymous namespace


// #################################################################################################
// ### Math - Constructor. Creates the hash map
// #################################################################################################
Math::Math( Compiler& compiler )
: Calculus( "ALib Math", compiler )
{
    constPI= M_PI; // Initialize constant static boxes. This must not be done
    constE = M_E;  // in the C++ bootstrap code.

    // load identifier/function names from resources
    FunctionNameDescriptor functionNames[34];
    LoadResourcedFunctionDescriptors( lib::EXPRESSIONS, ASTR("Math"), functionNames);
    FunctionNameDescriptor* descriptor= functionNames;

    // Constant identifiers
    ConstantIdentifiers=
    {
        { *descriptor++, constPI  },
        { *descriptor++, constE   },
    };

    // Identifiers and functions
    SIG_INT       = { Types::Integer                };
    SIG_INT_INT   = { Types::Integer,Types::Integer };
    SIG_FLT       = { Types::Float                  };
    SIG_FLT_FLT   = { Types::Float  ,Types::Float   };

    Functions=
    {
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(abs_I    ), Types::Integer  , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(abs      ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(ceil     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(floor    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(trunc    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(round    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(rint     ), Types::Integer  , CTI },
        { *descriptor  , &SIG_INT_INT, CALCULUS_CALLBACK(max_I    ), Types::Integer  , CTI },
        { *descriptor++, &SIG_FLT_FLT, CALCULUS_CALLBACK(max_F    ), Types::Float    , CTI },
        { *descriptor  , &SIG_INT_INT, CALCULUS_CALLBACK(min_I    ), Types::Integer  , CTI },
        { *descriptor++, &SIG_FLT_FLT, CALCULUS_CALLBACK(min_F    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT_FLT, CALCULUS_CALLBACK(remainder), Types::Float    , CTI },
        { *descriptor++, &SIG_NONE   , CALCULUS_CALLBACK(rand     ), Types::Float    , ETI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(sin      ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(asin     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(cos      ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(acos     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(tan      ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(atan     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(sinh     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(asinh    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(cosh     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(acosh    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(tanh     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(atanh    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(exp      ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(exp2     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(exp10    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(log      ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(log2     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(log10    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT_FLT, CALCULUS_CALLBACK(pow      ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(pow10    ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(sqrt     ), Types::Float    , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(cbrt     ), Types::Float    , CTI },
    };
};


}}}} // namespace [aworx::lib::expressions::detail]

#undef INT
#undef FLT
#undef FUNC


//! @endcond
