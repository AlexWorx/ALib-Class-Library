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
#include "alib/strings/strings.prepro.hpp"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Strings.Token;
    import   ALib.Characters.Functions;
    import   ALib.EnumOps;
#  if ALIB_RESOURCES
     import   ALib.Resources;
#  endif
#   if ALIB_ENUMRECORDS
     import   ALib.EnumRecords;
#  endif
#else
#   include "ALib.Strings.Token.H"
#   include "ALib.Characters.Functions.H"
#   include "ALib.EnumOps.H"
#   include "ALib.EnumRecords.H"
#   include "ALib.Resources.H"
#endif
// ======================================   Implementation   =======================================
// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif

namespace alib::strings::util  {

Token::Token(const String& pName, lang::Case sensitivity, int8_t minLength, const String& pExportName)
: definitionName (pName)
, exportName     (pExportName)
, format(Formats(   int8_t(Formats::Normal                                         )
                  + int8_t(sensitivity == lang::Case::Sensitive ? Formats(0) : ignoreCase) )  )
{
    minLengths[0]= minLength;
#if ALIB_DEBUG
    if( minLength < 0 || minLength > definitionName.Length() )
       format=  Formats(DbgDefinitionError::MinLenExceedsSegmentLength);

    if( minLength == 0 )
       format= Formats(DbgDefinitionError::ZeroMinLengthAndNotLastCamelHump);
#endif

}

Token::Token( const String& definitionSrc, lang::Case sensitivity,
              int8_t minLength1, int8_t minLength2, int8_t minLength3, int8_t minLength4, int8_t minLength5,
              int8_t minLength6, int8_t minLength7                                                   )
: definitionName(definitionSrc)
, minLengths { minLength1, minLength2, minLength3, minLength4, minLength5, minLength6, minLength7 }
{
    detectFormat();
    if( int(format) >= 0  &&  sensitivity == lang::Case::Ignore )
    format= Formats( int8_t(format) | int8_t(ignoreCase) );
}


void   Token::GetExportName(AString& target)                                                   const
{
    if( exportName.IsNotEmpty() )
    {
        target << exportName;
        return;
    }

    target << GetDefinitionName();

    // low the last character in if CamelCase and the last min length equals 0.
    if( GetFormat() == Token::Formats::CamelCase && Sensitivity() == lang::Case::Ignore )
    {
        for( int i= 0 ; i < 7 ; ++i )
        {
            auto minLen= GetMinLength( i );
            if( minLen == 0 )
            {
                target[target.Length()-1]= characters::ToLower(target[target.Length()-1]);
                break;
            }
            if( minLen == -1 )
                break;
        }
    }
}

#if ALIB_ENUMRECORDS
void Token::Define( const String& definitionSrc, character separator )
{
    minLengths[0]=  0;
    minLengths[1]= -1;
    format= ALIB_REL_DBG( Formats::Normal, Formats(DbgDefinitionError::EmptyName));

    Substring parser(definitionSrc);

    // name
    definitionName  = Substring( parser.ConsumeToken( separator ) ).Trim();
    if( definitionName.IsEmpty() )
        return;

    lang::Case letterCase= lang::Case::Sensitive;
    size_t qtyMinLengths= 0;
    if(parser.IsNotEmpty() )
    {
        // letter case sensitivity
        if( !enumrecords::Parse( parser, letterCase ) )
        {
            format= ALIB_REL_DBG( Formats::Normal, Formats(DbgDefinitionError::ErrorReadingSensitivity) );
            return;
        }

        // list of minimum length values
        while(parser.ConsumeChar( separator ) )
        {
            if( qtyMinLengths >= 7 )
            {
                format= ALIB_REL_DBG( Formats::Normal, Formats(DbgDefinitionError::TooManyMinLengthsGiven) );
                return;
            }

            if( !isdigit(parser.CharAtStart()) )
            {
                // optionally read export name once
                if( exportName.IsNotNull() )
                {
                    format= ALIB_REL_DBG( Formats::Normal, Formats(DbgDefinitionError::ErrorReadingMinLengths) );
                    return;
                }

                exportName= parser.ConsumeToken( separator, lang::Inclusion::Exclude );
                
                if( exportName.IsEmpty() )
                {
                    format= ALIB_REL_DBG( Formats::Normal, Formats(DbgDefinitionError::ErrorReadingMinLengths) );
                    return;
                }
                
                continue;
            }

            parser.ConsumeDecDigits( minLengths[qtyMinLengths++] );
        }
    }

    if( qtyMinLengths == 0 )
        minLengths[0]= int8_t( definitionName.Length() );

    if( qtyMinLengths > 0 && qtyMinLengths < 7 )
        minLengths[qtyMinLengths]= -1;

    #if ALIB_DEBUG
        if( parser.IsNotEmpty() )
        {
            format= Formats(DbgDefinitionError::DefinitionStringNotConsumed);
            return;
        }
    #endif

    detectFormat();

    #if ALIB_DEBUG
        if( int(format) < 0 )
            return;
    #endif

    if( letterCase == lang::Case::Ignore )
        format|=  ignoreCase;
}
#endif //ALIB_ENUMRECORDS

void    Token::detectFormat()
{
    // detect number of min length values
    int qtyMinLength= 1;
    while( qtyMinLength < 7 && minLengths[qtyMinLength] >= 0 )
        ++qtyMinLength;

    // just one length given? Keep format "normal"
    format= Formats::Normal;
    if( qtyMinLength > 1 )
    {
        // count hyphens, underscores, camel humps...
        bool hasLowerCases=  isalpha(definitionName[0]) && islower(definitionName[0]);
        int  qtyUpperCases=  0;
        int  qtyUnderscores= 0;
        int  qtyHyphens=     0;
        for( integer idx=  1; idx < definitionName.Length() ; ++idx )
        {
            character c= definitionName[idx];
                 if( c == '_' )      ++qtyUnderscores;
            else if( c == '-' )      ++qtyHyphens;
            else if( isalpha(c) )
            {
                if( islower(c) )
                    hasLowerCases= true;
                else
                    ++qtyUpperCases;
            }
            else
                hasLowerCases= true;
        }

        // Snake_Case?
        if( qtyUnderscores > 0 )
        {
            format= Formats::SnakeCase;
            #if ALIB_DEBUG
                if(    (qtyUnderscores >= 7  && qtyMinLength != 7 )
                    || (qtyUnderscores <  7  && qtyMinLength != qtyUnderscores  + 1 ) )
                    format= Formats(DbgDefinitionError::InconsistentMinLengths);
            #endif
        }

        // Kebab-Case?
        else if( qtyHyphens > 0 )
        {
            format= Formats::KebabCase;
            #if ALIB_DEBUG
                if(    (qtyHyphens >= 7  && qtyMinLength != 7 )
                    || (qtyHyphens <  7  && qtyMinLength != qtyHyphens  + 1 ) )
                    format= Formats(DbgDefinitionError::InconsistentMinLengths);
            #endif
        }

        // CamelCase
        else if( hasLowerCases && ( qtyUpperCases > 0 ) )
        {
            format= Formats::CamelCase;
            #if ALIB_DEBUG
                if(    (qtyUpperCases >= 7  && qtyMinLength != 7 )
                    || (qtyUpperCases <  7  && qtyMinLength != qtyUpperCases  + 1 ) )
                    format= Formats(DbgDefinitionError::InconsistentMinLengths);
            #endif
        }

        // normal
        #if ALIB_DEBUG
        else
            format= Formats(DbgDefinitionError::NoCaseSchemeFound);
        #endif
    }

    // check segment sizes against minLengths
    #if ALIB_DEBUG
        if( int(format) < 0 )
            return;

        if( GetFormat() == Formats::Normal )
        {
            if( minLengths[0] > definitionName.Length() )
            {
                format= Formats(DbgDefinitionError::MinLenExceedsSegmentLength);
                return;
            }
            if( minLengths[0] <= 0 )
            {
                format= Formats(DbgDefinitionError::ZeroMinLengthAndNotLastCamelHump);
                return;
            }
        }
        else
        {
            int     segmentNo    = 0;
            int     segmentLength= 0;
            integer charIdx      = 1;
            while( charIdx < definitionName.Length() )
            {
                ++segmentLength;
                character c= definitionName.CharAt( charIdx++ );
                bool segmentEnd=     c == '\0'
                                  || (format == Formats::SnakeCase && c == '_' )
                                  || (format == Formats::KebabCase && c == '-' )
                                  || (format == Formats::CamelCase && isalpha(c) && isupper(c) );

                if( segmentEnd )
                {
                    if( segmentNo < 7 && minLengths[segmentNo] > segmentLength )
                    {
                        format= Formats(DbgDefinitionError::MinLenExceedsSegmentLength);
                        return;
                    }

                    segmentLength=  (format == Formats::CamelCase ? 1 : 0);
                    ++segmentNo;
                }
            }

            for( int minLenIdx= 0 ; minLenIdx < 7 && minLengths[minLenIdx] >= 0 ; ++minLenIdx )
            {
                if(     minLengths[minLenIdx] == 0
                    &&  (   GetFormat() != Formats::CamelCase
                         || !( minLenIdx == 6 || minLengths[minLenIdx + 1] == -1 ) ) )
                {
                    format= Formats(DbgDefinitionError::ZeroMinLengthAndNotLastCamelHump);
                    return;
                }
            }
        }
    #endif


}

bool    Token::Match( const String& needle )
{
    ALIB_ASSERT_ERROR( needle.Length() > 0, "STRINGS/TOK",
                       "Empty search string in when matching function name." )
    lang::Case sensitivity= Sensitivity();

    Formats   caseType= GetFormat();
    bool      isNormal= (caseType == Formats::Normal    );
    bool      isCamel = (caseType == Formats::CamelCase );
    bool      isSnake = (caseType == Formats::SnakeCase );
    bool      isKebab = (caseType == Formats::KebabCase );

    int       segNo             = 0;
    int       segLen            = 0;
    bool      same              = false;
    integer   hIdx              = 0;
    integer   nIdx              = 0;
    integer   rollbackLen       = 0;
    bool      isSegOK           = false;
    int       segMinLen         = minLengths[0];
    while( hIdx < definitionName.Length() )
    {
        // read current haystack and needle
        ++segLen;
        character h= definitionName  .CharAt( hIdx++ );
        character n= needle.CharAt( nIdx++ );

        same= sensitivity == lang::Case::Ignore
              ?     characters::ToUpper(h)
                 == characters::ToUpper(n)
              :                                               h
                 ==                                           n;

        // special CamelCase treatment
        if( isCamel )
        {
            // end of needle and final, omitable segment?
            if( n == '\0' && segMinLen == 0)
                return true;

            // rollback
            if( !same )
            {
                if( segLen == 1 && rollbackLen > 0)
                {
                    nIdx-= 2;
                    --rollbackLen;
                    --hIdx;
                    --segLen;
                    continue;
                }

                --nIdx;
            }

            if( segLen == 1)
                rollbackLen= 0;

            else if( same && isSegOK )
                ++rollbackLen;
        }

        // end of haystack segment?
        bool isSegEnd=       hIdx == definitionName.Length()
                          || (isSnake && h == '_' )
                          || (isKebab && h == '-' )
                          || (isCamel && isalpha(definitionName.CharAt( hIdx ))
                                      && isupper(definitionName.CharAt( hIdx )) );

        // update segOK flag
        if( same )
        {
            isSegOK= (    ( segMinLen >= 0 && segLen >= segMinLen )
                       || ( segMinLen <  0 && isSegEnd            )  );
        }

        // result false, if not same and first of actual segment
        else if( segLen == 1 && segMinLen != 0 )
            return false;


        // end of segment and needle not empty?
        if( isSegEnd &&  n != '\0')
        {
            if( !isSegOK )
                return false;
        }

        // not same and either not end of segment or empty needle
        else if( !same )
        {
            if( !isSegOK )
                return false;

            // skip rest of segment
            while(    h != '\0'
                   && (     ( isCamel && (!isalpha(h) || !isupper(h) ) )
                        ||  ( isSnake && h != '_' )
                        ||  ( isKebab && h != '-' ) ) )
                h= definitionName.CharAt( hIdx++ );

            if( isCamel )
                --hIdx;
        }

        // start new segment
        if( !same || isSegEnd )
        {
            ++segNo;
            segLen= 0;
            segMinLen    = segNo < 7 ? minLengths[segNo] : -2;

            // oh,oh!
            if( n == '\0' && (!isCamel || h == '\0' || rollbackLen == 0) )
                return h == '\0' || isNormal || segMinLen == 0;
        }
    }

    return same && isSegOK && (nIdx == needle.Length());
}

} // namespace [alib::strings::util]

