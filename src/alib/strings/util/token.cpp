// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/strings/util/token.hpp"
#   if ALIB_ENUMS
#       include "alib/enums/serialization.hpp"
#       include "alib/lang/commonenums.hpp"
#   endif
#endif // !DOXYGEN

// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif

#if ALIB_BOXING
    ALIB_BOXING_VTABLE_DEFINE( alib::strings::util::Token*, vt_alib_strings_token )
#endif

namespace alib {  namespace strings { namespace util  {

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

#if ALIB_ENUMS
void Token::Define( const String& definitionSrc, character separator )
{
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    minLengths[0]=  0;
    minLengths[1]= -1;
    ALIB_WARNINGS_RESTORE
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
        if( !enums::Parse( parser, letterCase ) )
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

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            parser.ConsumeDecDigits( minLengths[qtyMinLengths++] );
            ALIB_WARNINGS_RESTORE
        }
    }

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    if( qtyMinLengths == 0 )
        minLengths[0]= static_cast<int8_t>( definitionName.Length() );

    if( qtyMinLengths > 0 && qtyMinLengths < 7 )
        minLengths[qtyMinLengths]= -1;
    ALIB_WARNINGS_RESTORE

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
#endif

void    Token::detectFormat()
{
    // detect number of min length values
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    int qtyMinLength= 1;
    while( qtyMinLength < 7 && minLengths[qtyMinLength] >= 0 )
        ++qtyMinLength;
    ALIB_WARNINGS_RESTORE

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
                    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                    if( segmentNo < 7 && minLengths[segmentNo] > segmentLength )
                    {
                        format= Formats(DbgDefinitionError::MinLenExceedsSegmentLength);
                        return;
                    }
                    ALIB_WARNINGS_RESTORE

                    segmentLength=  (format == Formats::CamelCase ? 1 : 0);
                    ++segmentNo;
                }
            }

            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
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
            ALIB_WARNINGS_RESTORE
        }
    #endif


}

bool    Token::Match( const String& needle )
{
    ALIB_ASSERT_ERROR( needle.Length() > 0,
                       "STRINGS/TOK", "Empty search string in when matching function name." )
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
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            segMinLen    = segNo < 7 ? minLengths[segNo] : -2;
            ALIB_WARNINGS_RESTORE

            // oh,oh!
            if( n == '\0' && (!isCamel || h == '\0' || rollbackLen == 0) )
                return h == '\0' || isNormal || segMinLen == 0;
        }
    }

    return same && isSegOK && (nIdx == needle.Length());
}

#if ALIB_CAMP && !DOXYGEN

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
#include "alib/lang/callerinfo_functions.hpp"

void Token::LoadResourcedTokens(  ResourcePool&         resourcePool,
                                  const NString&        resourceCategory,
                                  const NString&        resourceName,
                                  strings::util::Token* token,
                      ALIB_DBG(   int                   dbgSizeVerifier, )
                                  character             outerSeparator,
                                  character             innerSeparator        )
{
ALIB_WARNINGS_RESTORE
    ALIB_DBG( int tableSize= 0; )
    int resourceNo= -1; // disable number parsing

    Substring parser= resourcePool.Get( resourceCategory, resourceName ALIB_DBG(, false ) );
    if( parser.IsNull() )
        resourceNo= 0; // enable number parsing

    for( ;; )
    {
        if (resourceNo >= 0)
            parser= resourcePool.Get( resourceCategory, NString256() << resourceName << resourceNo++
                                      ALIB_DBG(, false ) );

        ALIB_ASSERT_ERROR( resourceNo != 1 || parser.IsNotNull(),  "STRINGS/TOK",
          NString256() <<  "Resource string(s) \"" << resourceCategory
                       <<  "/"                     << resourceName
                       << "(nn)\"  not found when parsing token."  )

        if( parser.IsEmpty() )
            break;

        while( parser.IsNotEmpty() )
        {
            String actValue= parser.ConsumeToken( outerSeparator );
            token->Define( actValue, innerSeparator );

            #if ALIB_DEBUG
                NCString errorMessage;
                switch( token->DbgGetError() )
                {
                    case Token::DbgDefinitionError::OK:
                        break;
                    case Token::DbgDefinitionError::EmptyName:
                        errorMessage= "No token name found.";
                        break;
                    case Token::DbgDefinitionError::ErrorReadingSensitivity:
                        errorMessage= "Sensitivity value not found.";
                        break;
                    case Token::DbgDefinitionError::ErrorReadingMinLengths:
                        errorMessage= "Error parsing the list of minimum lengths.";
                        break;
                    case Token::DbgDefinitionError::TooManyMinLengthsGiven:
                        errorMessage= " A maximum of 7 minimum length values was exceeded.";
                        break;
                    case Token::DbgDefinitionError::InconsistentMinLengths:
                        errorMessage= "The number of given minimum length values is greater than 1 "
                                      "but does not match the number of segments in the identifier.";
                        break;
                    case Token::DbgDefinitionError::NoCaseSchemeFound:
                        errorMessage= "More than one minimum length value was given but no "
                                      "segmentation scheme could be detected." ;
                        break;
                    case Token::DbgDefinitionError::MinLenExceedsSegmentLength:
                        errorMessage= "A minimum length is specified to be higher than the token "
                                      "name, respectively the according segment name.";
                        break;
                    case Token::DbgDefinitionError::DefinitionStringNotConsumed:
                        errorMessage= "The definition string was not completely consumed.";
                        break;
                    case Token::DbgDefinitionError::ZeroMinLengthAndNotLastCamelHump:
                        errorMessage= "Zero minimum length provided for segment which is not the last\n"
                                      "of a camel case token.";
                        break;

                    default: ALIB_ERROR("Illegal switch state.") break;
                }

                if( errorMessage.IsNotEmpty() )
                {
                    ALIB_ERROR( "STRINGS", errorMessage, NString512() <<
                        "\n(While reading token table.)\n"
                        "    Resource category (module name):  \"" <<  resourceCategory << "\"\n"
                        "    Resource name:                    \"" <<  resourceName     << "\"\n"
                        "    Token value parsed:               \"" <<  actValue         << "\""  )
                }

            #endif


            ++token;
            ALIB_DBG( tableSize++; )
        }
    }

    // check if there are more coming (a gap in numbered definition)
    #if ALIB_DEBUG
    if( resourceNo > 1 )
        for( int i= 0 ; i < 35 ; ++i )
        {
            if( resourcePool.Get( resourceCategory, NString256() << resourceName << (resourceNo + i)
                                  ALIB_DBG(, false ) ).IsNotNull() )
            {
                ALIB_ERROR( "STRINGS", NString128()
                   << "Detected a \"gap\" in numbering of resource strings while parsing "
                      "resource token table: "
                      "From index " << resourceNo - 1 << " to " << resourceNo + i - 1 << ".\n"
                      "Resource category/name: " << resourceCategory << '/' << resourceName << "." )
            }
        }
    #endif


    ALIB_ASSERT_ERROR( dbgSizeVerifier == tableSize, "STRINGS/TOK", NString512() <<
        "Size mismatch in resourced token table:\n"
        "    Resource category (module name):  \"" << resourceCategory   << "\"\n"
        "    Resource name:                    \"" << resourceName       << "\"\n"
        "    Resourced table size:             ["  << tableSize          << "]\n"
        "    Expected table size:              ["  << dbgSizeVerifier    << "]"     )

}
#include "alib/lang/callerinfo_methods.hpp"

#endif //ALIB_CAMP && !DOXYGEN

}}} // namespace [alib::strings::util]


