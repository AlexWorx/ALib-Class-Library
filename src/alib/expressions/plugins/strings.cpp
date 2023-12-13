// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_STRINGS)
#      include "alib/expressions/plugins/strings.hpp"
#   endif

#   if !defined (HPP_ALIB_MONOMEM_MASTRING)
#      include "alib/monomem/mastring.hpp"
#   endif

#   if !defined (HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER)
#      include "alib/strings/util/wildcardmatcher.hpp"
#   endif

#   if !defined (HPP_ALIB_STRINGS_UTIL_REGEXMATCHER)
#      include "alib/strings/util/regexmatcher.hpp"
#   endif

#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#      include "alib/strings/util/tokenizer.hpp"
#   endif
#endif // !defined(ALIB_DOX)


//! @cond NO_DOX

#define ARG0           (*args)
#define ARG1           (*(args+1))
#define ARG2           (*(args+2))
#define BOL(box)       (box).Unbox<bool     >()
#define INT(box)       (box).Unbox<integer>()
#define FLT(box)       (box).Unbox<double   >()
#define STR(box)       (box).Unbox<String   >()
#define LEN(box)       (box).UnboxLength()
#define CPY(str,size)  MAString( scope.Allocator, STR(str), size)
#define WRT(box,size)  MAString( scope.Allocator, box, size)
#define EMP(size )     MAString( scope.Allocator, size)

#define FUNC( name,...) Box name( Scope& scope,                                                    \
                                  ArgIterator  args,                                               \
                                  ArgIterator  end    )                                            \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }
#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
#   define TOINT(arg)                      arg
#else
#   define TOINT(arg) static_cast<integer>(arg)
#endif


namespace aworx { namespace lib { namespace expressions { namespace plugins {

// #################################################################################################
// ### ToString (not in anonymous namespace)
// #################################################################################################
Box CBToString( Scope& scope, ArgIterator  args, ArgIterator end)
{
    String256 tmp;
    tmp.DbgDisableBufferReplacementWarning();
    while( args < end )
    {
        Box& arg= *args;
             if( arg.IsType<integer>() ) tmp << Format( INT(arg), &scope.Formatter->DefaultNumberFormat);
        else if( arg.IsType<double >() ) tmp << Format( FLT(arg), &scope.Formatter->DefaultNumberFormat);
        else                             tmp << arg;

        ++args;
    }

    return MAString( scope.Allocator, tmp, 0 );
}

// #################################################################################################
// ### Format  (not in anonymous namespace)
// #################################################################################################
Box CBFormat( Scope& scope, ArgIterator  args, ArgIterator end)
{
    String256 buf;
    buf.DbgDisableBufferReplacementWarning();

    Boxes& formatterArgs= scope.Formatter->Acquire(ALIB_CALLER_PRUNED);
    while( args != end)
        formatterArgs.Add( *args++ );

    try
    {
        scope.Formatter->FormatArgs( buf );
    }
    catch( Exception& )
    {
        scope.Formatter->Release();
        throw;
    }

    scope.Formatter->Release();

    return MAString( scope.Allocator, buf, 0 );
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

FUNC(trim       ,                              return Substring(STR(ARG0)).Trim     (  ); )
FUNC(trim2      ,   String256 ws(STR(ARG1));   return Substring(STR(ARG0)).Trim     (ws); )
FUNC(trimStart  ,                              return Substring(STR(ARG0)).TrimStart(  ); )
FUNC(trimStart2 ,   String256 ws(STR(ARG1));   return Substring(STR(ARG0)).TrimStart(ws); )
FUNC(trimEnd    ,                              return Substring(STR(ARG0)).TrimEnd  (  ); )
FUNC(trimEnd2   ,   String256 ws(STR(ARG1));   return Substring(STR(ARG0)).TrimEnd  (ws); )

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
                    return MAString(scope.Allocator, buf,0);                                       )
FUNC(oct        ,   String128 buf;
                    buf << Format::Oct( static_cast<uint64_t>(INT(ARG0)),
                                        args + 1 != end ? static_cast<int>(INT(ARG1)) : 0,
                                        &scope.Formatter->DefaultNumberFormat );
                    return MAString(scope.Allocator, buf,0);                                       )
FUNC(bin        ,   String128 buf;
                    buf << Format::Bin( static_cast<uint64_t>(INT(ARG0)),
                                        args + 1 != end ? static_cast<int>(INT(ARG1)) : 0,
                                        &scope.Formatter->DefaultNumberFormat );
                    return MAString(scope.Allocator, buf,0);                                       )


Box replace( Scope& scope, ArgIterator  args, ArgIterator )
{
    String src        = STR(ARG0);
    String needle     = STR(ARG1);
    String replacement= STR(ARG2);

    // replace char with char?
    if( needle.Length() == 1 && replacement.Length() == 1 )
    {
        MAString result(scope.Allocator, src, 0 );
        result.SearchAndReplace( needle[0], replacement[0], 0 );
        return result;
    }

    // replace string with char or string
    String256 buf;
    buf.DbgDisableBufferReplacementWarning();
    buf << src;
    buf.SearchAndReplace( needle, replacement, 0 );
    return MAString( scope.Allocator, buf, 0 );
}

Box repeat( Scope& scope, ArgIterator  args, ArgIterator )
{
    String src        = STR(ARG0);
    String256 buf;
    buf.DbgDisableBufferReplacementWarning();
    for( auto i= INT(ARG1) ; i > 0  ; --i )
        buf << src;
    return MAString( scope.Allocator, buf, 0 );
}

// #################################################################################################
// ### Strings - Unary operators
// #################################################################################################
FUNC(boolNot,   return LEN(ARG0) == 0;         )


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
    tmp.DbgDisableBufferReplacementWarning();
    tmp << ARG1;
    return CPY(ARG0, tmp.Length()  ) << tmp;
)

FUNC(add_XS,  return WRT( ARG0, ARG1.UnboxLength()  ) << ARG1; )


FUNC(     sm, return  STR(ARG0)   <   ( STR(ARG1) ); )
FUNC(   smEq, return  STR(ARG0)   <=  ( STR(ARG1) ); )
FUNC(     gt, return  STR(ARG0)   >   ( STR(ARG1) ); )
FUNC(   gtEq, return  STR(ARG0)   >=  ( STR(ARG1) ); )
FUNC(     eq, return  STR(ARG0).Equals( STR(ARG1) ); )
FUNC(    neq, return !STR(ARG0).Equals( STR(ARG1) ); )

FUNC(    arr, return  STR(ARG0).Substring( INT(ARG1), 1); )

FUNC( compSS, return TOINT(             STR(ARG0).CompareTo<true ALIB_COMMA Case::Sensitive>( STR(ARG1) ) ); )
FUNC(compSSB, return TOINT( BOL(ARG2) ? STR(ARG0).CompareTo<true ALIB_COMMA Case::Ignore   >( STR(ARG1) )
                                      : STR(ARG0).CompareTo<true ALIB_COMMA Case::Sensitive>( STR(ARG1) ) ); )

// #################################################################################################
// ### Strings - Wildcard matching
// #################################################################################################
DOX_MARKER([DOX_ALIB_EXPR_CTRES_1])
struct ScopeWildcardMatcher : public ScopeResource
{
    // the matcher object
    WildcardMatcher matcher;

    // virtual destructor, implicitly deletes the matcher.
    virtual ~ScopeWildcardMatcher()  override {}
};
DOX_MARKER([DOX_ALIB_EXPR_CTRES_1])

DOX_MARKER([DOX_ALIB_EXPR_CTRES_6])
Box wldcrd( Scope& scope, ArgIterator  args, ArgIterator end )
{
    String haystack= STR(ARG0);
    String pattern = STR(ARG1);
    Case   sensitivity= ( end-args > 2 && BOL(ARG2) ) ? Case::Ignore
                                                      : Case::Sensitive;

    if( !scope.IsCompileTime() )
    {
        // Search for resource named "_wc"+ pattern.
        NString128 keyString("_wc");
        keyString.DbgDisableBufferReplacementWarning();
        keyString << pattern;
        auto storedMatcher=  scope.CTScope->NamedResources.Find( keyString );
        if( storedMatcher != scope.CTScope->NamedResources.end() )
            return dynamic_cast<ScopeWildcardMatcher*>( storedMatcher.Mapped() )
                   ->matcher.Match( haystack, sensitivity );
    }
DOX_MARKER([DOX_ALIB_EXPR_CTRES_6])

DOX_MARKER([DOX_ALIB_EXPR_CTRES_7])
    // This is either compile-time or the pattern string is not constant
    {
        WildcardMatcher matcher( pattern );
        return matcher.Match( haystack, sensitivity );
    }
}
DOX_MARKER([DOX_ALIB_EXPR_CTRES_7])

// #################################################################################################
// ### Strings - Regex matching
// #################################################################################################
#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
struct ScopeRegexMatcher : public ScopeResource
{
    // the matcher object
    RegexMatcher matcher;

    // virtual destructor, implicitly deletes the matcher.
    virtual ~ScopeRegexMatcher()                                                        override  {}
};

Box regex( Scope& scope, ArgIterator  args, ArgIterator )
{
    String haystack= STR(ARG0);
    String pattern = STR(ARG1);

    if( !scope.IsCompileTime() )
    {
        // Search for resource named  "_wc"+ pattern.
        NString128 keyString( "_re" );
        keyString.DbgDisableBufferReplacementWarning();
        keyString << pattern;
        auto storedMatcher=  scope.CTScope->NamedResources.Find( keyString );
        if( storedMatcher != scope.CTScope->NamedResources.end() )
        {
            ScopeRegexMatcher* matcher= dynamic_cast<ScopeRegexMatcher*>( storedMatcher->second );
            return matcher->matcher.Match( haystack );
        }
    }

    // This is either compile-time or the pattern string is not constant
    {
        RegexMatcher matcher( pattern  );
        return matcher.Match( haystack );
    }
}
#endif

// #################################################################################################
// ### Strings - Tables
// #################################################################################################
ALIB_CPP14_CONSTEXPR Calculus::OperatorTableEntry  operatorTableStrings[] =
{
    // unary operators
    { A_CHAR("!") , Types::String , Types::Void   , CALCULUS_CALLBACK(boolNot), Types::Boolean, Calculus::CTI },
    { A_CHAR("+") , Types::String , Types::Void   , CALCULUS_CALLBACK(toUpper), Types::String , Calculus::CTI },
    { A_CHAR("-") , Types::String , Types::Void   , CALCULUS_CALLBACK(toLower), Types::String , Calculus::CTI },

    // binary operators
    { A_CHAR("+") , Types::String , Types::String , CALCULUS_CALLBACK( add_SS), Types::String , Calculus::CTI},
    { A_CHAR("+") , Types::String , Types::Integer, CALCULUS_CALLBACK( add_SI), Types::String , Calculus::CTI},
    { A_CHAR("+") , Types::String , Types::Float  , CALCULUS_CALLBACK( add_SF), Types::String , Calculus::CTI},
    { A_CHAR("+") , Types::Integer, Types::String , CALCULUS_CALLBACK( add_IS), Types::String , Calculus::CTI},
    { A_CHAR("+") , Types::Float  , Types::String , CALCULUS_CALLBACK( add_FS), Types::String , Calculus::CTI},
    { A_CHAR("<") , Types::String , Types::String , CALCULUS_CALLBACK(     sm), Types::Boolean, Calculus::CTI},
    { A_CHAR("<="), Types::String , Types::String , CALCULUS_CALLBACK(   smEq), Types::Boolean, Calculus::CTI},
    { A_CHAR(">") , Types::String , Types::String , CALCULUS_CALLBACK(     gt), Types::Boolean, Calculus::CTI},
    { A_CHAR(">="), Types::String , Types::String , CALCULUS_CALLBACK(   gtEq), Types::Boolean, Calculus::CTI},
    { A_CHAR("=="), Types::String , Types::String , CALCULUS_CALLBACK(     eq), Types::Boolean, Calculus::CTI},
    { A_CHAR("!="), Types::String , Types::String , CALCULUS_CALLBACK(    neq), Types::Boolean, Calculus::CTI},
    { A_CHAR("*") , Types::String , Types::String , CALCULUS_CALLBACK( wldcrd), Types::Boolean, Calculus::CTI},
  #if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
    { A_CHAR("%") , Types::String , Types::String , CALCULUS_CALLBACK( regex), Types::Boolean, Calculus::CTI},
  #endif
    { A_CHAR("[]"), Types::String , Types::Integer, CALCULUS_CALLBACK(arr   ), Types::String , Calculus::CTI},
};


} // anonymous namespace

// #################################################################################################
// ### Strings - Constructor. Creates the hash map
// #################################################################################################
Strings::Strings( Compiler& compiler )
: Calculus( "ALib Strings", compiler )
{
    constTAB=  A_CHAR("\t"); // Initialize constant static boxes. This must not be done
    constNL =  NewLine();    // in the C++ bootstrap code.

    AddOperators( operatorTableStrings );

    // load identifier/function names from resources
    #if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
        constexpr int tableSize= 25;
    #else
        constexpr int tableSize= 24;
    #endif

    Token functionNames[tableSize];
    Token::LoadResourcedTokens( EXPRESSIONS, "CPS", functionNames
                                ALIB_DBG(,tableSize)                                 );
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    Token* descriptor= functionNames;

    // Constant identifiers
    ConstantIdentifiers=
    {
        { *descriptor++, constNL   },
        { *descriptor++, constTAB  },
    };

    Functions=
    {
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Var ), CALCULUS_CALLBACK(CBToString ), &Types::String , CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(wldcrd     ), &Types::Boolean, CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SSB ), CALCULUS_CALLBACK(wldcrd     ), &Types::Boolean, CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SVar), CALCULUS_CALLBACK(CBFormat   ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::S   ), CALCULUS_CALLBACK(toUpper    ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::S   ), CALCULUS_CALLBACK(toLower    ), &Types::String , CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(compSS     ), &Types::Integer, CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SSB ), CALCULUS_CALLBACK(compSSB    ), &Types::Integer, CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(startsWith ), &Types::Boolean, CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SSB ), CALCULUS_CALLBACK(startsWithC), &Types::Boolean, CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(  endsWith ), &Types::Boolean, CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SSB ), CALCULUS_CALLBACK(  endsWithC), &Types::Boolean, CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::SI  ), CALCULUS_CALLBACK(substr     ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SII ), CALCULUS_CALLBACK(substr2    ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(idxof      ), &Types::Integer, CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(count      ), &Types::Integer, CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::S   ), CALCULUS_CALLBACK(trim       ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(trim2      ), &Types::String , CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::S   ), CALCULUS_CALLBACK(trimStart  ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(trimStart2 ), &Types::String , CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::S   ), CALCULUS_CALLBACK(trimEnd    ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(trimEnd2   ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::S   ), CALCULUS_CALLBACK(parsei     ), &Types::Integer, CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::S   ), CALCULUS_CALLBACK(parsef     ), &Types::Float  , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SSI ), CALCULUS_CALLBACK(token      ), &Types::String , CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(hex        ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::II  ), CALCULUS_CALLBACK(hex        ), &Types::String , CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(oct        ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::II  ), CALCULUS_CALLBACK(oct        ), &Types::String , CTI   },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(bin        ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::II  ), CALCULUS_CALLBACK(bin        ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SSS ), CALCULUS_CALLBACK(replace    ), &Types::String , CTI   },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SI  ), CALCULUS_CALLBACK(repeat     ), &Types::String , CTI   },

        #if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::SS  ), CALCULUS_CALLBACK(regex      ), &Types::Boolean, CTI   },
        #endif
    };

    ALIB_ASSERT_ERROR( descriptor - functionNames == tableSize, "EXPR",
                       "Descriptor table size mismatch: Consumed {} descriptors, {} available.",
                       descriptor - functionNames, tableSize                                     )
    ALIB_WARNINGS_RESTORE
}

namespace {
bool genericConcatenation( Type type )
{
    return !(
                type.IsType<integer>()
             || type.IsType<double   >()
             || type.IsType<String   >() );

}
}

DOX_MARKER([DOX_ALIB_EXPR_CTRES_2])
bool Strings::TryCompilation( CIFunction& ciFunction )
{
    // invoke parent
    if( !Calculus::TryCompilation( ciFunction ) )
        return false;
DOX_MARKER([DOX_ALIB_EXPR_CTRES_2])

#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
    if( ciFunction.Callback == regex && (ciFunction.ArgsBegin + 1)->UnboxLength() > 0)
    {
        String pattern= (ciFunction.ArgsBegin + 1)->Unbox<String>();
        NString128 keyString("_re");
        keyString.DbgDisableBufferReplacementWarning();
        keyString << pattern;
        auto storedMatcher=  ciFunction.CompileTimeScope.NamedResources.Find( keyString );
        if( storedMatcher == ciFunction.CompileTimeScope.NamedResources.end() )
        {
            ScopeRegexMatcher* matcher= new ScopeRegexMatcher();
            matcher->matcher.Compile( pattern );
            ciFunction.CompileTimeScope.NamedResources.EmplaceOrAssign(
                                 NMAString( ciFunction.CompileTimeScope.Allocator, keyString, 0 ),
                                 matcher                                                       );
        }
    }
#endif

DOX_MARKER([DOX_ALIB_EXPR_CTRES_3])
    if( ciFunction.Callback == wldcrd && (ciFunction.ArgsBegin + 1)->UnboxLength() > 0)
    {
DOX_MARKER([DOX_ALIB_EXPR_CTRES_3])
DOX_MARKER([DOX_ALIB_EXPR_CTRES_4])
        String pattern= (ciFunction.ArgsBegin + 1)->Unbox<String>();
        NString128 keyString(A_CHAR("_wc"));
        keyString.DbgDisableBufferReplacementWarning();
        keyString << pattern;
DOX_MARKER([DOX_ALIB_EXPR_CTRES_4])
DOX_MARKER([DOX_ALIB_EXPR_CTRES_5])
        auto hashCode     =  keyString.Hashcode();
        auto storedMatcher=  ciFunction.CompileTimeScope.NamedResources.Find( keyString, hashCode );
        if( storedMatcher == ciFunction.CompileTimeScope.NamedResources.end() )
        {
            ScopeWildcardMatcher* matcher= new ScopeWildcardMatcher();
            matcher->matcher.Compile( pattern );
            NString keyCopy= ciFunction.CompileTimeScope.Allocator.EmplaceString( keyString );
            ciFunction.CompileTimeScope.NamedResources.InsertUnique( std::make_pair(keyCopy, matcher),
                                                                     hashCode);
        }
    }
    return true;
DOX_MARKER([DOX_ALIB_EXPR_CTRES_5])
}


bool Strings::TryCompilation( CIBinaryOp& ciBinaryOp )
{
    Box& lhs= * ciBinaryOp.ArgsBegin;
    Box& rhs= *(ciBinaryOp.ArgsBegin + 1);

    // fetch string concatenation operator '+'
    if( ciBinaryOp.Operator == A_CHAR("+") )
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
      ALIB_DBG( ciBinaryOp.DbgCallbackName = "add_SX";      )
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
      ALIB_DBG( ciBinaryOp.DbgCallbackName = "add_XS";      )
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
#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
    if( ciBinaryOp.Operator == A_CHAR("%") && !ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst  )
    {
        String pattern= (ciBinaryOp.ArgsBegin + 1)->Unbox<String>();
        NString128 keyString(A_CHAR("_re"));
        keyString.DbgDisableBufferReplacementWarning();
        keyString << pattern;
        auto hashCode     =  keyString.Hashcode();

        auto storedMatcher=  ciBinaryOp.CompileTimeScope.NamedResources.Find( keyString, hashCode );
        if( storedMatcher == ciBinaryOp.CompileTimeScope.NamedResources.end() )
        {
            ScopeRegexMatcher* matcher= new ScopeRegexMatcher();
            matcher->matcher.Compile( pattern );
            ciBinaryOp.CompileTimeScope.NamedResources.InsertOrAssign(
                         NMAString( ciBinaryOp.CompileTimeScope.Allocator, keyString, 0 ),
                         matcher,
                         hashCode     );
        }
    }
#endif

    // check for wildcard match operator '*'
    if( ciBinaryOp.Operator == A_CHAR("*") && !ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst  )
    {
        String pattern= (ciBinaryOp.ArgsBegin + 1)->Unbox<String>();
        NString128 keyString(A_CHAR("_wc"));
        keyString.DbgDisableBufferReplacementWarning();
        keyString << pattern;
        auto hashCode     =  keyString.Hashcode();

        auto storedMatcher=  ciBinaryOp.CompileTimeScope.NamedResources.Find( keyString, hashCode );
        if( storedMatcher == ciBinaryOp.CompileTimeScope.NamedResources.end() )
        {
            ScopeWildcardMatcher* matcher= new ScopeWildcardMatcher();
            matcher->matcher.Compile( pattern );

            NString keyCopy= ciBinaryOp.CompileTimeScope.Allocator.EmplaceString( keyString );
            ciBinaryOp.CompileTimeScope.NamedResources.InsertUnique( std::make_pair(keyCopy, matcher),
                                                                     hashCode );
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
