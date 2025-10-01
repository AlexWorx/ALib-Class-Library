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
#include "alib/enumrecords/enumrecords.prepro.hpp"
#include <stdexcept>
#if !ALIB_MONOMEM && ALIB_CONTAINERS
#    include <unordered_map>
#endif
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.EnumRecords.Bootstrap;
    import   ALib.Lang;
    import   ALib.EnumRecords;
#if ALIB_MONOMEM && ALIB_CONTAINERS
    import   ALib.Monomem;
    import   ALib.Containers.HashTable;
#endif
#if ALIB_FORMAT
    import   ALib.Format;
#endif
#else
#   include "ALib.Lang.H"
#   include "ALib.EnumRecords.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#if ALIB_MONOMEM && ALIB_CONTAINERS
#       include "ALib.Monomem.H"
#       include "ALib.Containers.HashTable.H"
#endif
#   include "ALib.Format.H"
#endif
// ======================================   Implementation   =======================================
namespace alib::enumrecords::bootstrap {

// #################################################################################################
// EnumRecordParser
// #################################################################################################
#if !DOXYGEN

namespace {
    void assembleMsgAndThrow [[noreturn]] ( const NString& error )
    {
        integer column=    EnumRecordParser::OriginalInput.Length()
                         - EnumRecordParser::Input        .Length();

        NAString msg;
        msg << "ERROR WHILE PARSING ENUMERATION RECORD STRING"             << NEW_LINE
            << "  Detail:  " << error                                      << NEW_LINE
            << "  Resrc :  "; if(EnumRecordParser::ResourceCategory.IsNotEmpty() )
                                    msg << '"'       << EnumRecordParser::ResourceCategory
                                        << "\" / \"" << EnumRecordParser::ResourceName << '"';
                                else
                                    msg << "(Not resourced)";
        msg << NEW_LINE
            << "  Column:   "   << column + 1                                << NEW_LINE
            << "  Input :   \"" << EnumRecordParser::OriginalInput << '"'    << NEW_LINE
            << "            ";
        for( integer i= column ; i >= 0 ; --i )
            msg << (i != 0 ? '-' : '>');
        msg << "^<--";

        throw std::runtime_error( msg.Terminate() );
    }
} // anonymous namespace

Substring   EnumRecordParser::Input;
character   EnumRecordParser::InnerDelimChar;
character   EnumRecordParser::OuterDelimChar;
String      EnumRecordParser::OriginalInput;
NString     EnumRecordParser::ResourceCategory;
NString     EnumRecordParser::ResourceName;

void EnumRecordParser::error [[noreturn]] (const NCString& what)
{
    assembleMsgAndThrow( NString256() <<  what << '.' );
}

void EnumRecordParser::assertNoWhitespaces(const NCString& where)
{
    if(    Input.IsNotEmpty()
        && Input.IndexOfAny<lang::Inclusion::Exclude>( DEFAULT_WHITESPACES ) != 0 )
        assembleMsgAndThrow( NString256() << "Found whitespaces "
                                          << where );
}

void EnumRecordParser::assertNoTrailingWhitespaces(String& token)
{
    if( token.LastIndexOfAny<lang::Inclusion::Exclude>( DEFAULT_WHITESPACES ) != token.Length() -1 )
        assembleMsgAndThrow( NString256() << "Found trailing whitespaces in string value \""
                                          << token << '"' );
}

void EnumRecordParser::assertNoUnnecessary(character specificChar, const NCString& where)
{
    if( Input.CharAtStart() == specificChar )
        assembleMsgAndThrow( NString256() << "Unnecessary character \""
                                          << specificChar
                                          << "\" found " << where );
}

void EnumRecordParser::assertChar(character specificChar, const NCString& where)
{
    if( !Input.ConsumeChar(specificChar) )
        assembleMsgAndThrow( NString256() <<  where << '\"' << specificChar << '\"');
}

void EnumRecordParser::assertEndOfRecord()
{
    if ( Input.IsEmpty() )
        return;
    assertNoWhitespaces( "after record" );
    if( Input.CharAtStart() != OuterDelimChar )
        assembleMsgAndThrow( NString256() << "Expected outer delimiter or end of input" );
}

void EnumRecordParser::assertEndOfInput()
{
    if ( !Input.IsEmpty() )
        assembleMsgAndThrow( NString256() << "Expected end of parsable input string" );
}

integer EnumRecordParser::getInteger( bool isLastField )
{
    integer bigInt;
    assertNoWhitespaces( "before integral value" );
    assertNoUnnecessary( A_CHAR('+'), "before integral value" );

         if( Input.ConsumeString<lang::Case::Ignore>( A_CHAR("max") ) )
           bigInt= (std::numeric_limits<integer>::max)();
    else if( Input.ConsumeString<lang::Case::Ignore>( A_CHAR("min") ) )   bigInt= (std::numeric_limits<integer>::min)();
    else if( Input.ConsumeChar( A_CHAR('^') ) )
    {
        int exp;
        if( !Input.ConsumeDec( exp ) )
            error("Power of 2 symbol '^' is not followed by a number" );
        bigInt= integer(1LL << exp);
    }
    else
    {
        if(    (!isLastField && ( Input.CharAtStart() == InnerDelimChar                   ) )
            || ( isLastField && ( Input.CharAtStart() == OuterDelimChar || Input.IsEmpty()) )  )
            bigInt= 0;
        else
            if( ! Input.ConsumeInt( bigInt ) )
                error( "Not an integral value" );
    }

    if( !isLastField )
        Delim();
    else
        assertEndOfRecord();
    return bigInt;
}


void EnumRecordParser::Get( String& result, bool isLastField )
{
    assertNoWhitespaces( "before string" );
    if( !isLastField )
        result= Input.ConsumeToken(InnerDelimChar);
    else
        Input.ConsumeChars<NC>( Input.IndexOfOrLength( OuterDelimChar ), result );
    assertNoTrailingWhitespaces(result);
    if( isLastField )
        assertEndOfRecord();
}

void EnumRecordParser::Get( character& result, bool isLastField )
{
    assertNoWhitespaces( "before a character value" );
    result= Input.ConsumeChar<CHK, lang::Whitespaces::Keep>();
    if(    (!isLastField && ( result == InnerDelimChar                   ) )
        || ( isLastField && ( result == OuterDelimChar || result == '\0' ) )  )
        result= '\0';
    else
    {
        if( result == '\0' )
            error("End of input when parsing a character." );
        assertNoWhitespaces( "after a character value" );
        if( !isLastField )
            Delim();
        else
            assertEndOfRecord();
    }
}

void EnumRecordParser::Get( double& result, bool isLastField )
{
    assertNoWhitespaces( "before a floating point value" );
    assertNoUnnecessary( A_CHAR('+'), "before floating point value" );
    if(    (!isLastField && ( Input.CharAtStart() == InnerDelimChar                   ) )
        || ( isLastField && ( Input.CharAtStart() == OuterDelimChar || Input.IsEmpty()) )  )
        result= 0.0;
    else
        if( !Input.ConsumeFloat( result ) )
            error("Not a floating point value" );

    if( !isLastField )
        Delim();
    else
        assertEndOfRecord();
}

void EnumRecordParser:: Delim()
{
    assertNoWhitespaces( "before a delimiter");
    assertChar(InnerDelimChar,"expected inner delimiter" );
    assertNoWhitespaces( "after an inner delimiter");
}

void EnumRecordParser::OuterDelim()
{
    assertNoWhitespaces( "before an outer delimiter");
    assertChar(OuterDelimChar,"expected outer delimiter" );
    assertNoWhitespaces( "after an outer delimiter");
}

#endif // #if !DOXYGEN


// #################################################################################################
// enumrecords::detail::bootstrap()
// #################################################################################################
} namespace alib::enumrecords::detail {

#   include "ALib.Lang.CIFunctions.H"
void shutdown()
{
    #if ALIB_MONOMEM && ALIB_CONTAINERS
    getInternalRecordMap().Reset();
    #endif
}

} // namespace [alib::enumrecords::detail]
#   include "ALib.Lang.CIMethods.H"


