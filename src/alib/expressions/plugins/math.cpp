// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_MATH)
#   include "alib/expressions/plugins/math.hpp"
#endif
#if !defined (_GLIBCXX_CMATH) && !defined (_CMATH_)
#   include <cmath>
#endif
#if defined(_MSC_VER)
#   define _USE_MATH_DEFINES
#   include <math.h>
#endif
#endif // !defined(ALIB_DOX)

//! @cond NO_DOX

#define INT(box)       (box).Unbox<integer>()
#define FLT(box)       (box).Unbox<double   >()
#define FUNC( name,...) Box name( Scope& scope,                                                   \
                                  ArgIterator  args,                                              \
                                  ArgIterator  end    )                                           \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }

namespace aworx { namespace lib { namespace expressions { namespace plugins {

namespace {

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
FUNC(   rint     , return  static_cast<integer>(::llrint(FLT(*args))); )
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
#if defined(_MSC_VER) || defined(__APPLE__)
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
#if defined(_MSC_VER) || defined(__APPLE__)
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
    constexpr int tableSize= 34;
    Token functionNames[tableSize];
    Token::LoadResourcedTokens( EXPRESSIONS, "CPM", functionNames
                                ALIB_DBG(,tableSize)                               );
    Token* descriptor= functionNames;

    // Constant identifiers
    ConstantIdentifiers=
    {
        { *descriptor++, constPI  },
        { *descriptor++, constE   },
    };

    Functions=
    {
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I ), CALCULUS_CALLBACK(abs_I    ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(abs      ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(ceil     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(floor    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(trunc    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(round    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(rint     ), &Types::Integer  , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::II), CALCULUS_CALLBACK(max_I    ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::FF), CALCULUS_CALLBACK(max_F    ), &Types::Float    , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::II), CALCULUS_CALLBACK(min_I    ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::FF), CALCULUS_CALLBACK(min_F    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::FF), CALCULUS_CALLBACK(remainder), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(nullptr       ), CALCULUS_CALLBACK(rand     ), &Types::Float    , ETI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(sin      ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(asin     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(cos      ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(acos     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(tan      ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(atan     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(sinh     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(asinh    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(cosh     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(acosh    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(tanh     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(atanh    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(exp      ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(exp2     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(exp10    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(log      ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(log2     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(log10    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::FF), CALCULUS_CALLBACK(pow      ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(pow10    ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(sqrt     ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F ), CALCULUS_CALLBACK(cbrt     ), &Types::Float    , CTI },
    };

    ALIB_ASSERT_ERROR( descriptor - functionNames == tableSize,
                       "Descriptor table size mismatch: Consumed {} descriptors, {} available.",
                       descriptor - functionNames, tableSize                                     )
}


}}}} // namespace [aworx::lib::expressions::detail]

#undef INT
#undef FLT
#undef FUNC


//! @endcond
