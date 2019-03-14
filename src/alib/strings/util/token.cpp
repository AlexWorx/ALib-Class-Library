// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/strings/util/token.hpp"

#if ALIB_FILESET_MODULES && ALIB_MODULE_RESOURCES && !defined(HPP_ALIB_LIB_COMMONENUMS_RESOURCED)
#   include "alib/lib/commonenumsresourced.hpp"
    ALIB_ENUM_IS_BITWISE( aworx::lib::strings::util::Token::Formats )
#endif


// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif


namespace aworx { namespace lib { namespace strings { namespace util  {

Token::Token(const String& pName, Case sensitivity, int8_t minLength)
: name  (pName)
, format(Formats(   int8_t(Formats::Normal                                         )
                  + int8_t(sensitivity == Case::Sensitive ? Formats(0) : ignoreCase) )  )
, minLengths { minLength, 0,0,0,0,0,0 }
{
#if ALIB_DEBUG
    if( minLength < 0 || minLength > name.Length() )
       format=  Formats(DbgDefinitionError::MinLenExceedsSegmentLength);

    if( minLength == 0 )
       format= Formats(DbgDefinitionError::ZeroMinLengthAndNotLastCamelHump);
#endif

}

Token::Token( const String& pName, Case sensitivity,
              int8_t minLength1, int8_t minLength2, int8_t minLength3, int8_t minLength4, int8_t minLength5,
              int8_t minLength6, int8_t minLength7                                                   )
: name       (pName)
, minLengths { minLength1, minLength2, minLength3, minLength4, minLength5, minLength6, minLength7 }
{
    detectFormat();
    if( int(format) >= 0  &&  sensitivity == Case::Ignore )
    format= Formats( int8_t(format) | int8_t(ignoreCase) );
}

#if ALIB_FILESET_MODULES && ALIB_MODULE_RESOURCES
void Token::Define( const String& definition, character separator )
{
    minLengths[0]=  0;
    minLengths[1]= -1;
    format= ALIB_REL_DBG( Formats::Normal, Formats(DbgDefinitionError::EmptyName));

    Substring parser(definition);

    // name
    name  = Substring( parser.ConsumeToken( separator ) ).Trim();
    if( name.IsEmpty() )
        return;

    Case letterCase= Case::Sensitive;
    size_t qtyMinLenghts= 0;
    if(parser.IsNotEmpty() )
    {
        // letter case sensitivity
        if( !parser.ConsumeEnum( letterCase ) )
        {
            format= ALIB_REL_DBG( Formats::Normal, Formats(DbgDefinitionError::ErrorReadingSensitivity) );
            return;
        }

        // list of minimum length values
        while(parser.ConsumeChar( separator ) )
        {
            if( qtyMinLenghts >= 7 )
            {
                format= ALIB_REL_DBG( Formats::Normal, Formats(DbgDefinitionError::TooManyMinLengthsGiven) );
                return;
            }

            if( !isdigit(parser.CharAtStart()) )
            {
                format= ALIB_REL_DBG( Formats::Normal, Formats(DbgDefinitionError::ErrorReadingMinLengths) );
                return;
            }

            parser.ConsumeDecDigits( minLengths[qtyMinLenghts++] );
        }
    }
    if( qtyMinLenghts == 0 )
        minLengths[0]= static_cast<int8_t>( name.Length() );
    if( qtyMinLenghts> 0 && qtyMinLenghts < 7 )
        minLengths[qtyMinLenghts]= -1;

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

    if( letterCase == Case::Ignore )
        format|=  ignoreCase;
}
#endif

void    Token::detectFormat()
{
    // detect number of min length values
    int qtyMinLength= 1;
    while( qtyMinLength < 7 && minLengths[qtyMinLength] >= 0 )
        qtyMinLength++;

    // just one length given? Keep format "normal"
    format= Formats::Normal;
    if( qtyMinLength > 1 )
    {
        // count hyphens, underscores, camel humps...
        bool hasLowerCases=  isalpha(name[0]) && islower(name[0]);
        int  qtyUpperCases=  0;
        int  qtyUnderscores= 0;
        int  qtyHyphens=     0;
        for( integer idx=  1; idx < name.Length() ; ++idx )
        {
            character c= name[idx];
                 if( c == '_' )      qtyUnderscores++;
            else if( c == '-' )      qtyHyphens++;
            else if( isalpha(c) )
            {
                if( islower(c) )
                    hasLowerCases= true;
                else
                    qtyUpperCases++;
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
            if( minLengths[0] > name.Length() )
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
            while( charIdx < name.Length() )
            {
                segmentLength++;
                character c= name.CharAt( charIdx++ );
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
                    segmentNo++;
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
    ALIB_ASSERT_ERROR( needle.Length() > 0, "Empty search string in when matching function name." )
    Case sensitivity= Sensitivity();

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
    while( hIdx < name.Length() )
    {
        // read current haystack and needle
        segLen++;
        character h= name  .CharAt( hIdx++ );
        character n= needle.CharAt( nIdx  ++ );

        same= sensitivity == Case::Ignore
              ? toupper(h) == toupper(n)
              :         h  ==         n;

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
                    rollbackLen--;
                    hIdx--;
                    segLen--;
                    continue;
                }

                nIdx--;
            }

            if( segLen == 1)
                rollbackLen= 0;

            else if( same && isSegOK )
                rollbackLen++;
        }

        // end of haystack segment?
        bool isSegEnd=       hIdx == name.Length()
                          || (isSnake && h == '_' )
                          || (isKebab && h == '-' )
                          || (isCamel && isalpha(name.CharAt( hIdx ))
                                      && isupper(name.CharAt( hIdx )) );

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
                h= name.CharAt( hIdx++ );

            if( isCamel )
                hIdx--;
        }

        // start new segment
        if( !same || isSegEnd )
        {
            segNo++;
            segLen= 0;
            segMinLen    = segNo < 7 ? minLengths[segNo] : -2;

            // oh,oh!
            if( n == '\0' && (!isCamel || h == '\0' || rollbackLen == 0) )
                return h == '\0' || isNormal || segMinLen == 0;
        }
    }

    return same && isSegOK && (nIdx == needle.Length());
}

#if ALIB_FILESET_MODULES && ALIB_MODULE_RESOURCES && !ALIB_DOCUMENTATION_PARSER

void Token::LoadResourcedTokens( Module& module, const NString& resourceName,
                                 Token *token,
                                 ALIB_DBG(int dbgSizeVerifier, )
                                 character outerSeparator,
                                 character innerSeparator                      )
{
    ALIB_DBG( int tableSize= 0; )
    Substring parser= module.GetResource( resourceName );
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
                    errorMessage= "Zero minimum length provided for segment which is not the last\\n"
                                  "of a camel case token.";
                    break;
            }

            if( errorMessage.IsNotEmpty() )
            {
                ALIB_ERROR( errorMessage, NString512() <<
                    "\n(While reading token table.)\n"
                    "    Resource category (module name):  \"" <<  module.ResourceCategory << "\"\n"
                    "    Resource name:                    \"" <<  resourceName            << "\"\n"
                    "    Token value parsed:               \"" <<  actValue                << "\""  )


            }

        #endif


        token++;
        ALIB_DBG( tableSize++; )
    }

    ALIB_ASSERT_ERROR( dbgSizeVerifier == tableSize, NString512() <<
        "Size mismatch in resourced token table:\\n"
        "    Resource category (module name):  \"" << module.ResourceCategory   << "\"\n"
        "    Resource name:                    \"" << resourceName              << "\"\n"
        "    Resourced table size:             ["  << tableSize                 << "]\\n"
        "    Expecteded table size:            ["  << dbgSizeVerifier           << "]"     )

}
#endif

}}}} // namespace [aworx::lib::strings::util]

//! @cond NO_DOX
void aworx::lib::strings::T_Append<aworx::lib::strings::util::Token,aworx::character>
::operator()( TAString<aworx::character>& target, const aworx::lib::strings::util::Token& src)
{
    target << src.GetRawName();

    // low the last character in if CamelCase and the last min length equals 0.
    if( src.GetFormat() == Token::Formats::CamelCase && src.Sensitivity() == Case::Ignore )
    {
        for( int i= 0 ; i < 7 ; ++i )
        {
            auto minLen= src.GetMinLength( i );
            if( minLen == 0 )
            {
                target[target.Length()-1]= characters::CharArray<character>::ToLower(target[target.Length()-1]);
                break;
            }
            if( minLen == -1 )
                break;
        }
    }
}
//! @endcond
