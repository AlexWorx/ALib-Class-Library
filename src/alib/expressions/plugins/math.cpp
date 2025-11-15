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
#   include <cmath>

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Expressions.Impl;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
#else
#   include "ALib.Expressions.Impl.H"
#endif
//========================================== Implementation ========================================

#if !defined(M_PI)
#   define M_PI         3.14159265358979323846
#endif
#if !defined(M_E)
#   define M_E          2.7182818284590452354
#endif
#if !defined(M_LN10)
#   define M_LN10       2.30258509299404568402
#endif


//! @cond NO_DOX

#define INT(box)       (box).Unbox<integer>()
#define FLT(box)       (box).Unbox<double   >()
#define FUNC( name,...) Box name( Scope& scope,                                                   \
                                  ArgIterator  args,                                              \
                                  ArgIterator  end    )                                           \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }

namespace alib {  namespace expressions { namespace plugins {

namespace {

//##################################################################################################
// ### Math - constants
//##################################################################################################

Box constPI;
Box constE;

//##################################################################################################
// ### Math - functions
//##################################################################################################
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
FUNC(   rint     , return  integer(::llrint(FLT(*args))); )
FUNC(   remainder, return  ::remainder(FLT(*args),FLT(*(args+1))); )

FUNC(   rand     , return  double(std::rand()) / double(RAND_MAX); )

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
#if defined(_WIN32) || defined(__APPLE__)  || defined(__ANDROID_NDK__)
   FUNC(exp10    , return  ::pow      (10.0 ,FLT(*args)); )
#else
   FUNC(exp10    , return  ::exp10    (FLT(*args)); )
#endif


FUNC(   log      , return  ::log      (FLT(*args)); )
FUNC(   log2     , return  ::log2     (FLT(*args)); )
#if defined(_WIN32)
  FUNC( log10    , return  ::log      (FLT(*args)) / M_LN10; )
#else
  FUNC( log10    , return  ::log10    (FLT(*args)); )
#endif

FUNC(   pow      , return  ::pow      (FLT(*args++),FLT(*args)); )
#if defined(_WIN32) || defined(__APPLE__)   || defined(__ANDROID_NDK__)
   FUNC(pow10    , return  ::pow      (10.0        ,FLT(*args)); )
#else
   FUNC(pow10    , return  ::exp10    (FLT(*args)); )
#endif
FUNC(   sqrt     , return  ::sqrt     (FLT(*args)); )
FUNC(   cbrt     , return  ::cbrt     (FLT(*args)); )



} // anonymous namespace


//##################################################################################################
// ### Math - Constructor. Creates the hash map
//##################################################################################################
Math::Math( Compiler& compiler )
: Calculus( "ALib Math", compiler, CompilePriorities::Math ) {
    constPI= M_PI; // Initialize constant static boxes. This must not be done
    constE = M_E;  // in the C++ bootstrap code.

    // load identifier/function names from resources
    constexpr int tableSize= 34;
    Token functionNames[tableSize];
    strings::util::LoadResourcedTokens( EXPRESSIONS, "CPM", functionNames
                                        ALIB_DBG(,tableSize)                  );

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

    ALIB_ASSERT_ERROR( descriptor - functionNames == tableSize, "EXPR",
                       "Descriptor table size mismatch: Consumed {} descriptors, {} available.",
                       descriptor - functionNames, tableSize                                     )
}


}}} // namespace [alib::expressions::detail]

#undef INT
#undef FLT
#undef FUNC


//! @endcond
