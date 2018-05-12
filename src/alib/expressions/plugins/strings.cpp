// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################


#include "alib/alib.hpp"
#include "alib/strings/substring.hpp"
#include "alib/strings/util/wildcardmatcher.hpp"
#include "alib/strings/util/regexmatcher.hpp"
#include "alib/strings/util/tokenizer.hpp"

#include "strings.hpp"
#include "scopestring.hpp"
#include "../compiler.hpp"


#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif



//! @cond NO_DOX

#define ARG0           (*args)
#define ARG1           (*(args+1))
#define ARG2           (*(args+2))
#define BOL(box)       (box).Unbox<bool     >()
#define INT(box)       (box).Unbox<boxed_int>()
#define FLT(box)       (box).Unbox<double   >()
#define STR(box)       (box).Unbox<String   >()
#define LEN(box)       (box).Length()
#define CPY(str,size)  ScopeString( scope, STR(str), size)
#define WRT(box,size)  ScopeString( scope, size, box)
#define EMP(size )     ScopeString( scope, size)

#define FUNC( name,...) Box name( Scope& scope,                                                    \
                                  ArgIterator  args,                                              \
                                  ArgIterator  end    )                                           \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }

namespace aworx { namespace lib { namespace expressions { namespace plugins {

// #################################################################################################
// ### ToString (not in anonymous namespace)
// #################################################################################################
Box CBToString( Scope& scope, ArgIterator  args, ArgIterator end)
{
    String256 tmp;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( tmp, ReplaceExternalBuffer )
    while( args < end )
    {
        Box& arg= *args;
             if( arg.IsType<boxed_int>() ) tmp << Format( INT(arg), &scope.Formatter->DefaultNumberFormat);
        else if( arg.IsType<double   >() ) tmp << Format( FLT(arg), &scope.Formatter->DefaultNumberFormat);
        else                               tmp << arg;

        args++;
    }

    return ScopeString( scope, tmp, 0 );
}

// #################################################################################################
// ### Format  (not in anonymous namespace)
// #################################################################################################
Box CBFormat( Scope& scope, ArgIterator  args, ArgIterator end)
{
    String256 buf;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( buf, ReplaceExternalBuffer )
    Boxes fmtArgs;
    fmtArgs.reserve( static_cast<size_t>( end - args ) );
    while( args != end)
        fmtArgs.Add( *args++ );
    scope.Formatter->Format( buf, fmtArgs );
    return ScopeString( scope, buf, 0 );
}


namespace {


// #################################################################################################
// ### String - constants
// #################################################################################################
Box constTAB;
Box constNL;

// #################################################################################################
// ### String functions
// #################################################################################################

// Parameter signatures
std::vector<Box>  SIG_S;
std::vector<Box>  SIG_I;
std::vector<Box>  SIG_II;
std::vector<Box>  SIG_V;
std::vector<Box>  SIG_SS;
std::vector<Box>  SIG_SV;
std::vector<Box>  SIG_SSB;
std::vector<Box>  SIG_SSI;
std::vector<Box>  SIG_SSS;
std::vector<Box>  SIG_SI;
std::vector<Box>  SIG_SII;

FUNC(toUpper    ,   return CPY(ARG0, 0).ToUpper(); )
FUNC(toLower    ,   return CPY(ARG0, 0).ToLower(); )
FUNC(startsWith ,   return             STR(ARG0).StartsWith<true,Case::Sensitive>( STR(ARG1) ); )
FUNC(startsWithC,   return BOL(ARG2) ? STR(ARG0).StartsWith<true,Case::Ignore   >( STR(ARG1) )
                                     : STR(ARG0).StartsWith<true,Case::Sensitive>( STR(ARG1) ); )
FUNC(endsWith   ,   return             STR(ARG0).  EndsWith<true,Case::Sensitive>( STR(ARG1) ); )
FUNC(endsWithC  ,   return BOL(ARG2) ? STR(ARG0).  EndsWith<true,Case::Ignore   >( STR(ARG1) )
                                     : STR(ARG0).  EndsWith<true,Case::Sensitive>( STR(ARG1) ); )
FUNC(substr     ,   return STR(ARG0).Substring( INT(ARG1)             ); )
FUNC(substr2    ,   return STR(ARG0).Substring( INT(ARG1), INT(ARG2)  ); )
FUNC(idxof      ,   String needle= STR(ARG1); return needle.Length() == 1 ? STR(ARG0).IndexOf         ( needle[0], 0 )
                                                                          : STR(ARG0).IndexOf( needle   , 0 ); )
FUNC(count      ,   String needle= STR(ARG1); return needle.Length() == 1 ? STR(ARG0).CountChar       ( needle[0]    )
                                                                          : STR(ARG0).Count           ( needle   , 0 ); )

FUNC(trim       ,                                               return Substring(STR(ARG0)).Trim     (  ); )
FUNC(trim2      ,   String256 ws(STR(ARG1)); ws.Terminate();    return Substring(STR(ARG0)).Trim     (ws); )
FUNC(trimStart  ,                                               return Substring(STR(ARG0)).TrimStart(  ); )
FUNC(trimStart2 ,   String256 ws(STR(ARG1)); ws.Terminate();    return Substring(STR(ARG0)).TrimStart(ws); )
FUNC(trimEnd    ,                                               return Substring(STR(ARG0)).TrimEnd  (  ); )
FUNC(trimEnd2   ,   String256 ws(STR(ARG1)); ws.Terminate();    return Substring(STR(ARG0)).TrimEnd  (ws); )

FUNC(parsei     ,   integer result; Substring(STR(ARG0)).ConsumeInt  (result, &scope.Formatter->DefaultNumberFormat); return result; )
FUNC(parsef     ,   double  result; Substring(STR(ARG0)).ConsumeFloat(result, &scope.Formatter->DefaultNumberFormat); return result; )

FUNC(token      ,   Tokenizer tknzr( STR(ARG0), STR(ARG1).CharAtStart() );
                    for( auto i= INT(ARG2) ; i >= 0  ; --i )
                        tknzr.Next( Whitespaces::Keep );
                    return tknzr.Actual;  )

FUNC(hex        ,   String128 buf;
                    buf << Format::Hex( static_cast<uint64_t>(INT(ARG0)),
                                        args + 1 != end ? static_cast<int>(INT(ARG1)) : 0,
                                        &scope.Formatter->DefaultNumberFormat );
                    return ScopeString(scope, buf,0);                                              )
FUNC(oct        ,   String128 buf;
                    buf << Format::Oct( static_cast<uint64_t>(INT(ARG0)),
                                        args + 1 != end ? static_cast<int>(INT(ARG1)) : 0,
                                        &scope.Formatter->DefaultNumberFormat );
                    return ScopeString(scope, buf,0);                                              )
FUNC(bin        ,   String128 buf;
                    buf << Format::Bin( static_cast<uint64_t>(INT(ARG0)),
                                        args + 1 != end ? static_cast<int>(INT(ARG1)) : 0,
                                        &scope.Formatter->DefaultNumberFormat );
                    return ScopeString(scope, buf,0);                                              )


Box replace( Scope& scope, ArgIterator  args, ArgIterator )
{
    String src        = STR(ARG0);
    String needle     = STR(ARG1);
    String replacement= STR(ARG2);

    // replace char with char?
    if( needle.Length() == 1 && replacement.Length() == 1 )
    {
        ScopeString result(scope, src, 0 );
        result.SearchAndReplace( needle[0], replacement[0], 0 );
        return result;
    }

    // replace string with char or string
    String128 tNeedle;
    String256 buf;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( tNeedle, ReplaceExternalBuffer )
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( buf    , ReplaceExternalBuffer )
    tNeedle << needle;
    buf << src;
    buf.SearchAndReplace( tNeedle, replacement, 0 );
    return ScopeString( scope, buf, 0 );
}

Box repeat( Scope& scope, ArgIterator  args, ArgIterator )
{
    String src        = STR(ARG0);
    String256 buf;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( buf    , ReplaceExternalBuffer )
    for( auto i= INT(ARG1) ; i > 0  ; --i )
        buf << src;
    return ScopeString( scope, buf, 0 );
}

// #################################################################################################
// ### Strings - Unary operators
// #################################################################################################
FUNC(boolNot,   return LEN(ARG0) == 0;         )


constexpr Calculus::UnaryOpTableEntry        unaryTable[] =
{
    { ASTR("!") , Types::String, CALCULUS_CALLBACK( boolNot ), Types::Boolean , Calculus::CTI },
    { ASTR("+") , Types::String, CALCULUS_CALLBACK( toUpper ), Types::String  , Calculus::CTI },
    { ASTR("-") , Types::String, CALCULUS_CALLBACK( toLower ), Types::String  , Calculus::CTI },
};


// #################################################################################################
// ### Strings - Binary operators
// #################################################################################################

FUNC(add_SI, return CPY(ARG0, 28      ) << Format( INT(ARG1), &scope.Formatter->DefaultNumberFormat );     )
FUNC(add_SF, return CPY(ARG0, 48      ) << Format( FLT(ARG1), &scope.Formatter->DefaultNumberFormat );     )


FUNC(add_IS, return EMP(LEN(ARG1) + 28      ) << Format( INT(ARG0), &scope.Formatter->DefaultNumberFormat) << STR(ARG1);    )
FUNC(add_FS, return EMP(LEN(ARG1) + 48      ) << Format( FLT(ARG0), &scope.Formatter->DefaultNumberFormat) << STR(ARG1);    )
FUNC(add_SS, return CPY(ARG0, LEN(ARG1)) << STR(ARG1); )
FUNC(add_SX,
    String256 tmp;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( tmp, ReplaceExternalBuffer )
    tmp << ARG1;
    return CPY(ARG0, tmp.Length()  ) << tmp;
)

FUNC(add_XS,  return WRT( ARG0, ARG1.Length()  ) << ARG1; )


FUNC(     sm, return  STR(ARG0)   <   ( STR(ARG1) ); )
FUNC(   smEq, return  STR(ARG0)   <=  ( STR(ARG1) ); )
FUNC(     gt, return  STR(ARG0)   >   ( STR(ARG1) ); )
FUNC(   gtEq, return  STR(ARG0)   >=  ( STR(ARG1) ); )
FUNC(     eq, return  STR(ARG0).Equals( STR(ARG1) ); )
FUNC(    neq, return !STR(ARG0).Equals( STR(ARG1) ); )

FUNC(    arr, return  STR(ARG0).Substring( INT(ARG1), 1); )

FUNC( compSS, return              STR(ARG0).CompareTo<true,Case::Sensitive>( STR(ARG1) ); )
FUNC(compSSB, return  BOL(ARG2) ? STR(ARG0).CompareTo<true,Case::Ignore   >( STR(ARG1) )
                                : STR(ARG0).CompareTo<true,Case::Sensitive>( STR(ARG1) ); )

// #################################################################################################
// ### Strings - Wildcard matching
// #################################################################################################
//! [DOX_ALIB_EXPR_CTRES_1]
struct ScopeWildcardMatcher : public ScopeResource
{
    // the matcher object
    WildcardMatcher matcher;

    // virtual destructor, implicitly deletes the matcher.
    virtual ~ScopeWildcardMatcher()  {}
};
//! [DOX_ALIB_EXPR_CTRES_1]

//! [DOX_ALIB_EXPR_CTRES_6]
Box wldcrd( Scope& scope, ArgIterator  args, ArgIterator end )
{
    String haystack= STR(ARG0);
    String pattern = STR(ARG1);
    Case   sensitivity= ( end-args > 2 && BOL(ARG2) ) ? Case::Ignore
                                                      : Case::Sensitive;

    if( !scope.IsCompileTime() )
    {
        // Search for resource named "_wc"+ pattern.
        String128 keyString(ASTR("_wc"));
        ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( keyString, ReplaceExternalBuffer )
        keyString << pattern;
        auto storedMatcher=  scope.CTScope->NamedResources.find( keyString );
        if( storedMatcher != scope.CTScope->NamedResources.end() )
        {
            ScopeWildcardMatcher* matcher= dynamic_cast<ScopeWildcardMatcher*>( storedMatcher->second );
            return matcher->matcher.Match( haystack, sensitivity );
        }
    }
//! [DOX_ALIB_EXPR_CTRES_6]

//! [DOX_ALIB_EXPR_CTRES_7]
    // This is either compile time or the pattern string is not constant
    {
        WildcardMatcher matcher( pattern );
        return matcher.Match( haystack, sensitivity );
    }
}
//! [DOX_ALIB_EXPR_CTRES_7]

// #################################################################################################
// ### Strings - Regex matching
// #################################################################################################
#if ALIB_FEAT_BOOST_REGEX
struct ScopeRegexMatcher : public ScopeResource
{
    // the matcher object
    RegexMatcher matcher;

    // virtual destructor, implicitly deletes the matcher.
    virtual ~ScopeRegexMatcher()  {}
};

Box regex( Scope& scope, ArgIterator  args, ArgIterator )
{
    String haystack= STR(ARG0);
    String pattern = STR(ARG1);

    if( !scope.IsCompileTime() )
    {
        // Search for resource named  "_wc"+ pattern.
        String128 keyString(ASTR("_re"));
        ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( keyString, ReplaceExternalBuffer )
        keyString << pattern;
        auto storedMatcher=  scope.CTScope->NamedResources.find( keyString );
        if( storedMatcher != scope.CTScope->NamedResources.end() )
        {
            ScopeRegexMatcher* matcher= dynamic_cast<ScopeRegexMatcher*>( storedMatcher->second );
            return matcher->matcher.Match( haystack );
        }
    }

    // This is either compile time or the pattern string is not constant
    {
        RegexMatcher matcher( pattern  );
        return matcher.Match( haystack );
    }
}
#endif

// #################################################################################################
// ### Strings - Tables
// #################################################################################################
constexpr Calculus::BinaryOpTableEntry  binaryOpTable[] =
{
    { ASTR("+") , Types::String , Types::String , CALCULUS_CALLBACK(add_SS), Types::String , Calculus::CTI},
    { ASTR("+") , Types::String , Types::Integer, CALCULUS_CALLBACK(add_SI), Types::String , Calculus::CTI},
    { ASTR("+") , Types::String , Types::Float  , CALCULUS_CALLBACK(add_SF), Types::String , Calculus::CTI},
    { ASTR("+") , Types::Integer, Types::String , CALCULUS_CALLBACK(add_IS), Types::String , Calculus::CTI},
    { ASTR("+") , Types::Float  , Types::String , CALCULUS_CALLBACK(add_FS), Types::String , Calculus::CTI},
    { ASTR("<") , Types::String , Types::String , CALCULUS_CALLBACK(    sm), Types::Boolean, Calculus::CTI},
    { ASTR("<="), Types::String , Types::String , CALCULUS_CALLBACK(  smEq), Types::Boolean, Calculus::CTI},
    { ASTR(">") , Types::String , Types::String , CALCULUS_CALLBACK(    gt), Types::Boolean, Calculus::CTI},
    { ASTR(">="), Types::String , Types::String , CALCULUS_CALLBACK(  gtEq), Types::Boolean, Calculus::CTI},
    { ASTR("=="), Types::String , Types::String , CALCULUS_CALLBACK(    eq), Types::Boolean, Calculus::CTI},
    { ASTR("!="), Types::String , Types::String , CALCULUS_CALLBACK(   neq), Types::Boolean, Calculus::CTI},
    { ASTR("*") , Types::String , Types::String , CALCULUS_CALLBACK(wldcrd), Types::Boolean, Calculus::CTI},
  #if ALIB_FEAT_BOOST_REGEX
    { ASTR("%") , Types::String , Types::String , CALCULUS_CALLBACK( regex), Types::Boolean, Calculus::CTI},
  #endif
    { ASTR("[]"), Types::String , Types::Integer, CALCULUS_CALLBACK(arr   ), Types::String , Calculus::CTI},
};


}; // anonymous namespace

// #################################################################################################
// ### Strings - Constructor. Creates the hash map
// #################################################################################################
Strings::Strings( Compiler& compiler )
: Calculus( "ALib Strings", compiler )
{
    constTAB=  ASTR("\t"); // Initialize constant static boxes. This must not be done
    constNL =  NewLine;    // in the C++ bootstrap code.
    SIG_S   = { Types::String                                  };
    SIG_I   = { Types::Integer                                 };
    SIG_II  = { Types::Integer, Types::Integer                 };
    SIG_V   = { nullptr                                        };
    SIG_SS  = { Types::String , Types::String                  };
    SIG_SV  = { Types::String , nullptr                        };
    SIG_SSB = { Types::String , Types::String , Types::Boolean };
    SIG_SSI = { Types::String , Types::String , Types::Integer };
    SIG_SI  = { Types::String , Types::Integer                 };
    SIG_SII = { Types::String , Types::Integer, Types::Integer };
    SIG_SSS = { Types::String , Types::String , Types::String  };


    AddUnaryOps ( unaryTable    );
    AddBinaryOps( binaryOpTable );

    // load identifier/function names from resources
    FunctionNameDescriptor functionNames[25];
    LoadResourcedFunctionDescriptors( lib::EXPRESSIONS, ASTR("Strings"), functionNames);
    FunctionNameDescriptor* descriptor= functionNames;


    // Constant identifiers
    ConstantIdentifiers=
    {
        { *descriptor++, constNL   },
        { *descriptor++, constTAB  },
    };


    Functions=
    {
        { *descriptor++, &SIG_V   , CALCULUS_CALLBACK(CBToString ), Types::String , CTI   },
        { *descriptor  , &SIG_SS  , CALCULUS_CALLBACK(wldcrd     ), Types::Boolean, CTI   },
        { *descriptor++, &SIG_SSB , CALCULUS_CALLBACK(wldcrd     ), Types::Boolean, CTI   },
#if ALIB_FEAT_BOOST_REGEX
        { *descriptor++, &SIG_SS  , CALCULUS_CALLBACK(regex      ), Types::Boolean, CTI   },
#endif
        { *descriptor++, &SIG_SV  , CALCULUS_CALLBACK(CBFormat   ), Types::String , CTI   },
        { *descriptor++, &SIG_S   , CALCULUS_CALLBACK(toUpper    ), Types::String , CTI   },
        { *descriptor++, &SIG_S   , CALCULUS_CALLBACK(toLower    ), Types::String , CTI   },
        { *descriptor  , &SIG_SS  , CALCULUS_CALLBACK(compSS     ), Types::Integer, CTI   },
        { *descriptor++, &SIG_SSB , CALCULUS_CALLBACK(compSSB    ), Types::Integer, CTI   },
        { *descriptor  , &SIG_SS  , CALCULUS_CALLBACK(startsWith ), Types::Boolean, CTI   },
        { *descriptor++, &SIG_SSB , CALCULUS_CALLBACK(startsWithC), Types::Boolean, CTI   },
        { *descriptor  , &SIG_SS  , CALCULUS_CALLBACK(  endsWith ), Types::Boolean, CTI   },
        { *descriptor++, &SIG_SSB , CALCULUS_CALLBACK(  endsWithC), Types::Boolean, CTI   },
        { *descriptor  , &SIG_SI  , CALCULUS_CALLBACK(substr     ), Types::String , CTI   },
        { *descriptor++, &SIG_SII , CALCULUS_CALLBACK(substr2    ), Types::String , CTI   },
        { *descriptor++, &SIG_SS  , CALCULUS_CALLBACK(idxof      ), Types::Integer, CTI   },
        { *descriptor++, &SIG_SS  , CALCULUS_CALLBACK(count      ), Types::Integer, CTI   },
        { *descriptor  , &SIG_S   , CALCULUS_CALLBACK(trim       ), Types::String , CTI   },
        { *descriptor++, &SIG_SS  , CALCULUS_CALLBACK(trim2      ), Types::String , CTI   },
        { *descriptor  , &SIG_S   , CALCULUS_CALLBACK(trimStart  ), Types::String , CTI   },
        { *descriptor++, &SIG_SS  , CALCULUS_CALLBACK(trimStart2 ), Types::String , CTI   },
        { *descriptor  , &SIG_S   , CALCULUS_CALLBACK(trimEnd    ), Types::String , CTI   },
        { *descriptor++, &SIG_SS  , CALCULUS_CALLBACK(trimEnd2   ), Types::String , CTI   },
        { *descriptor++, &SIG_S   , CALCULUS_CALLBACK(parsei     ), Types::Integer, CTI   },
        { *descriptor++, &SIG_S   , CALCULUS_CALLBACK(parsef     ), Types::Float  , CTI   },
        { *descriptor++, &SIG_SSI , CALCULUS_CALLBACK(token      ), Types::String , CTI   },
        { *descriptor  , &SIG_I   , CALCULUS_CALLBACK(hex        ), Types::String , CTI   },
        { *descriptor++, &SIG_II  , CALCULUS_CALLBACK(hex        ), Types::String , CTI   },
        { *descriptor  , &SIG_I   , CALCULUS_CALLBACK(oct        ), Types::String , CTI   },
        { *descriptor++, &SIG_II  , CALCULUS_CALLBACK(oct        ), Types::String , CTI   },
        { *descriptor  , &SIG_I   , CALCULUS_CALLBACK(bin        ), Types::String , CTI   },
        { *descriptor++, &SIG_II  , CALCULUS_CALLBACK(bin        ), Types::String , CTI   },
        { *descriptor++, &SIG_SSS , CALCULUS_CALLBACK(replace    ), Types::String , CTI   },
        { *descriptor++, &SIG_SI  , CALCULUS_CALLBACK(repeat     ), Types::String , CTI   },
    };
}

namespace {
bool genericConcatenation( Type type )
{
    return !(
                type.IsType<boxed_int>()
             || type.IsType<double   >()
             || type.IsType<String   >() );

}
}

//! [DOX_ALIB_EXPR_CTRES_2]
bool Strings::TryCompilation( CIFunction& ciFunction )
{
    // invoke parent
    if( !Calculus::TryCompilation( ciFunction ) )
        return false;
//! [DOX_ALIB_EXPR_CTRES_2]

#if ALIB_FEAT_BOOST_REGEX
    if( ciFunction.Callback == regex && (ciFunction.ArgsBegin + 1)->Length() > 0)
    {
        String pattern= (ciFunction.ArgsBegin + 1)->Unbox<String>();
        String128 keyString(ASTR("_re"));
        ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( keyString, ReplaceExternalBuffer )
        keyString << pattern;
        auto storedMatcher=  ciFunction.CompileTimeScope.NamedResources.find( keyString );
        if( storedMatcher == ciFunction.CompileTimeScope.NamedResources.end() )
        {
            ScopeRegexMatcher* matcher= new ScopeRegexMatcher();
            matcher->matcher.Compile( pattern );
            ciFunction.CompileTimeScope.NamedResources[ScopeString( ciFunction.CompileTimeScope, keyString, 0 )]= matcher;
        }
    }
#endif

//! [DOX_ALIB_EXPR_CTRES_3]
    if( ciFunction.Callback == wldcrd && (ciFunction.ArgsBegin + 1)->Length() > 0)
    {
//! [DOX_ALIB_EXPR_CTRES_3]
//! [DOX_ALIB_EXPR_CTRES_4]
        String pattern= (ciFunction.ArgsBegin + 1)->Unbox<String>();
        String128 keyString(ASTR("_wc"));
        ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( keyString, ReplaceExternalBuffer )
        keyString << pattern;
//! [DOX_ALIB_EXPR_CTRES_4]
//! [DOX_ALIB_EXPR_CTRES_5]
        auto storedMatcher=  ciFunction.CompileTimeScope.NamedResources.find( keyString );
        if( storedMatcher == ciFunction.CompileTimeScope.NamedResources.end() )
        {
            ScopeWildcardMatcher* matcher= new ScopeWildcardMatcher();
            matcher->matcher.Compile( pattern );
            ciFunction.CompileTimeScope.NamedResources[ScopeString( ciFunction.CompileTimeScope, keyString, 0 )]= matcher;
        }
    }
    return true;
//! [DOX_ALIB_EXPR_CTRES_5]
}


bool Strings::TryCompilation( CIBinaryOp& ciBinaryOp )
{
    Box& lhs= * ciBinaryOp.ArgsBegin;
    Box& rhs= *(ciBinaryOp.ArgsBegin + 1);

    // fetch string concatenation operator '+'
    if( ciBinaryOp.Operator == ASTR("+") )
    {
        bool argsAreConst= ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst;

        if( lhs.IsType<String>() && genericConcatenation(rhs) )
        {
            // optimize out?
            if( argsAreConst )
            {
                ciBinaryOp.TypeOrValue     = add_SX( ciBinaryOp.CompileTimeScope,
                                                     ciBinaryOp.ArgsBegin,
                                                     ciBinaryOp.ArgsEnd           );
      ALIB_DBG( ciBinaryOp.DbgCallBackName = "add_SX";      )
                return true;
            }

            ciBinaryOp.Callback            = add_SX;
            ciBinaryOp.TypeOrValue         = Types::String;
            return true;
        }

        if( genericConcatenation(lhs) && rhs.IsType<String>() )
        {
            if( argsAreConst )
            {
                ciBinaryOp.TypeOrValue     = add_XS( ciBinaryOp.CompileTimeScope,
                                                     ciBinaryOp.ArgsBegin,
                                                     ciBinaryOp.ArgsEnd           );
      ALIB_DBG( ciBinaryOp.DbgCallBackName = "add_XS";      )
                return true;
            }

            ciBinaryOp.Callback            = add_XS;
            ciBinaryOp.TypeOrValue         = Types::String;
            return true;
        }
    }

    // invoke parent
    if( !Calculus::TryCompilation( ciBinaryOp ) )
        return false;

    // Perform the same mechanics as with TryCompilation("regex") above:
    // check for regex match operator '*'
#if ALIB_FEAT_BOOST_REGEX
    if( ciBinaryOp.Operator == ASTR("%") && !ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst  )
    {
        String pattern= (ciBinaryOp.ArgsBegin + 1)->Unbox<String>();
        String128 keyString(ASTR("_re"));
        ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( keyString, ReplaceExternalBuffer )
        keyString << pattern;

        auto storedMatcher=  ciBinaryOp.CompileTimeScope.NamedResources.find( keyString );
        if( storedMatcher == ciBinaryOp.CompileTimeScope.NamedResources.end() )
        {
            ScopeRegexMatcher* matcher= new ScopeRegexMatcher();
            matcher->matcher.Compile( pattern );
            ciBinaryOp.CompileTimeScope.NamedResources[ScopeString( ciBinaryOp.CompileTimeScope, keyString, 0 )]= matcher;
        }
    }
#endif

    // check for wildcard match operator '*'
    if( ciBinaryOp.Operator == ASTR("*") && !ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst  )
    {
        String pattern= (ciBinaryOp.ArgsBegin + 1)->Unbox<String>();
        String128 keyString(ASTR("_wc"));
        ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( keyString, ReplaceExternalBuffer )
        keyString << pattern;

        auto storedMatcher=  ciBinaryOp.CompileTimeScope.NamedResources.find( keyString );
        if( storedMatcher == ciBinaryOp.CompileTimeScope.NamedResources.end() )
        {
            ScopeWildcardMatcher* matcher= new ScopeWildcardMatcher();
            matcher->matcher.Compile( pattern );
            ciBinaryOp.CompileTimeScope.NamedResources[ScopeString( ciBinaryOp.CompileTimeScope, keyString, 0 )]= matcher;
        }
    }
    return true;

}

}}}} // namespace [aworx::lib::expressions::detail]


#undef BOL
#undef INT
#undef FLT
#undef STR
#undef LEN
#undef CPY
#undef EMP
#undef FUNC_UNARY
#undef FUNC
#undef UN_MAP_ENTRY
#undef BIN_MAP_ENTRY
#undef BIN_ALIAS_ENTRY

//! @endcond
