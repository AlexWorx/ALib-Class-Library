// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_LANG_COMMONENUMS)
#       include "alib/lang/commonenums.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_DETAIL_ENUMRECORDMAP)
#      include "alib/enums/detail/enumrecordmap.hpp"
#   endif

#   if ALIB_STRINGS
#       if !defined(HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#           include "alib/enums/recordbootstrap.hpp"
#       endif
#       if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#          include "alib/strings/localstring.hpp"
#       endif
#   endif
#
#endif // !defined(ALIB_DOX)


namespace alib {  namespace enums {


// #################################################################################################
// Details
// #################################################################################################
namespace detail {

namespace
{
    #if ALIB_MONOMEM
        HashMap           < EnumRecordKey, const void*,
                            EnumRecordKey::Hash,
                            EnumRecordKey::EqualTo       >  EnumRecordMap( &monomem::GlobalAllocator,
                                                                           3.0, 6.0  );
    #else
        std::unordered_map< EnumRecordKey, const void*,
                            EnumRecordKey::Hash,
                            EnumRecordKey::EqualTo       >  EnumRecordMap;
    #endif
}

void  setEnumRecord( const std::type_info& rtti, integer elementValue, const void* record )
{
    #if ALIB_MONOMEM
        EnumRecordMap.EmplaceIfNotExistent( EnumRecordKey(rtti, elementValue ), record );
    #else
        EnumRecordMap.try_emplace( EnumRecordKey(rtti, elementValue ), record );
    #endif
}

const void* getEnumRecord( const std::type_info& rtti, integer elementValue )
{

#if ALIB_MONOMEM
    auto it= EnumRecordMap.Find( EnumRecordKey( rtti, elementValue ) );
#else
    auto it= EnumRecordMap.find( EnumRecordKey( rtti, elementValue ) );
#endif
    if ( it != EnumRecordMap.end() )
        return it->second;

    return nullptr;
}

#if ALIB_MONOMEM
const HashMap           < EnumRecordKey, const void*,
                          EnumRecordKey::Hash,
                          EnumRecordKey::EqualTo       >&  getInternalRecordMap()
#else
const std::unordered_map< EnumRecordKey, const void*,
                          EnumRecordKey::Hash,
                          EnumRecordKey::EqualTo       >&  getInternalRecordMap()
#endif
{
    return EnumRecordMap;
}



} // namespace alib::enums[::detail]

// #################################################################################################
// EnumRecordParser
// #################################################################################################
#if ALIB_STRINGS && !defined(ALIB_DOX)

namespace {
    void assembleMsgAndThrow [[noreturn]] ( const NString& error )
    {
        integer column=    EnumRecordParser::OriginalInput.Length()
                         - EnumRecordParser::Input        .Length();

        NAString msg;
        msg << "ERROR WHILE PARSING ENUMERATION RECORD STRING"             << NewLine()
            << "  Detail:  " << error                                      << NewLine()
            << "  Resrc :  "; if(EnumRecordParser::ResourceCategory.IsNotEmpty() )
                                    msg << '"'       << EnumRecordParser::ResourceCategory
                                        << "\" / \"" << EnumRecordParser::ResourceName << '"';
                                else
                                    msg << "(Not resourced)";
        msg << NewLine()
            << "  Column:   "   << column + 1                                << NewLine()
            << "  Input :   \"" << EnumRecordParser::OriginalInput << '"'    << NewLine()
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
        && Input.IndexOfAny<lang::Inclusion::Exclude>( DefaultWhitespaces() ) != 0 )
        assembleMsgAndThrow( NString256() << "Found whitespaces "
                                          << where );
}

void EnumRecordParser::assertNoTrailingWhitespaces(String& token)
{
    if( token.LastIndexOfAny<lang::Inclusion::Exclude>( DefaultWhitespaces() ) != token.Length() -1 )
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
        bigInt= static_cast<integer>(1LL << exp);
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
        Input.ConsumeChars<false>( Input.IndexOfOrLength( OuterDelimChar ), result );
    assertNoTrailingWhitespaces(result);
    if( isLastField )
        assertEndOfRecord();
}

void EnumRecordParser::Get( character& result, bool isLastField )
{
    assertNoWhitespaces( "before a character value" );
    result= Input.ConsumeChar<true, lang::Whitespaces::Keep>();
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
    assertNoWhitespaces( "prior to a delimiter");
    assertChar(InnerDelimChar,"expected inner delimiter" );
    assertNoWhitespaces( "after an inner delimiter");
}

void EnumRecordParser::OuterDelim()
{
    assertNoWhitespaces( "prior to an outer delimiter");
    assertChar(OuterDelimChar,"expected outer delimiter" );
    assertNoWhitespaces( "after an outer delimiter");
}


// #################################################################################################
// Implementation of parsing methods of built-in record types.
// #################################################################################################
void ERSerializable::Parse()
{
    EnumRecordParser::Get( EnumElementName     );
    EnumRecordParser::Get( MinimumRecognitionLength , true );
}

#endif // ALIB_STRINGS && defined(ALIB_DOX)


// #################################################################################################
// enums::Bootstrap()
// #################################################################################################
#if !ALIB_CAMP
void Bootstrap()
{
#if ALIB_STRINGS
DOX_MARKER([DOX_ALIB_ENUMS_MULTIPLE_RECORDS])
EnumRecords<lang::Bool>::Bootstrap(
{
    { lang::Bool::True , A_CHAR("False"), 1 },
    { lang::Bool::False, A_CHAR("True" ), 1 },
    { lang::Bool::True , A_CHAR("0"    ), 1 },
    { lang::Bool::False, A_CHAR("1"    ), 1 },
    { lang::Bool::True , A_CHAR("No"   ), 1 },
    { lang::Bool::False, A_CHAR("Yes"  ), 1 },
    { lang::Bool::True , A_CHAR("Off"  ), 2 },
    { lang::Bool::False, A_CHAR("On"   ), 2 },
    { lang::Bool::True , A_CHAR("-"    ), 1 },
    { lang::Bool::False, A_CHAR("Ok"   ), 2 }
} );
DOX_MARKER([DOX_ALIB_ENUMS_MULTIPLE_RECORDS])

EnumRecords<lang::Case>::Bootstrap(
{
    { lang::Case::Sensitive, A_CHAR("Sensitive"), 1 },
    { lang::Case::Ignore   , A_CHAR("Ignore"   ), 1 },
} );

DOX_MARKER([DOX_ALIB_ENUMS_MULTIPLE_RECORDS_2])
EnumRecords<lang::ContainerOp>::Bootstrap(
{
    { lang::ContainerOp::Insert   , A_CHAR("Insert"   ), 1 }, // integral 0
    { lang::ContainerOp::Remove   , A_CHAR("Remove"   ), 1 }, // integral 1
    { lang::ContainerOp::GetCreate, A_CHAR("GetCreate"), 4 }, // integral 3 <-- Switched order
    { lang::ContainerOp::Get      , A_CHAR("Get"      ), 1 }, // integral 2 <--
    { lang::ContainerOp::Create   , A_CHAR("Create"   ), 1 }, // integral 4
} );
DOX_MARKER([DOX_ALIB_ENUMS_MULTIPLE_RECORDS_2])

EnumRecords<lang::Switch>::Bootstrap(
{
    { lang::Switch::Off                    , A_CHAR("Off")           , 2 },
    { lang::Switch::On                     , A_CHAR("On" )           , 2 },
} );

EnumRecords<lang::Alignment>::Bootstrap(
{
    { lang::Alignment::Left                , A_CHAR("Left"  )        , 1 },
    { lang::Alignment::Right               , A_CHAR("Right" )        , 1 },
    { lang::Alignment::Center              , A_CHAR("Center")        , 1 },
} );

EnumRecords<lang::SortOrder>::Bootstrap(
{
    { lang::SortOrder::Ascending           , A_CHAR("Ascending" )    , 1 },
    { lang::SortOrder::Descending          , A_CHAR("Descending")    , 1 },
} );

EnumRecords<lang::Inclusion>::Bootstrap(
{
    { lang::Inclusion::Include             , A_CHAR("Include")       , 1 },
    { lang::Inclusion::Exclude             , A_CHAR("Exclude")       , 1 },
} );

EnumRecords<lang::Reach>::Bootstrap(
{
    { lang::Reach::Global                  , A_CHAR("Global")        , 1 },
    { lang::Reach::Local                   , A_CHAR("Local" )        , 1 },
} );

EnumRecords<lang::CurrentData>::Bootstrap(
{
    { lang::CurrentData::Keep              , A_CHAR("Keep" )         , 1 },
    { lang::CurrentData::Clear             , A_CHAR("Clear")         , 1 },
} );

EnumRecords<lang::SourceData>::Bootstrap(
{
    { lang::SourceData::Copy               , A_CHAR("Copy")          , 1 },
    { lang::SourceData::Move               , A_CHAR("Move")          , 1 },
} );


EnumRecords<lang::Safeness>::Bootstrap(
{
    { lang::Safeness::Safe                 , A_CHAR("Safe"  )        , 1 },
    { lang::Safeness::Unsafe               , A_CHAR("Unsafe")        , 1 },
} );

EnumRecords<lang::Responsibility>::Bootstrap(
{
    { lang::Responsibility::KeepWithSender , A_CHAR("KeepWithSender"), 1 },
    { lang::Responsibility::Transfer       , A_CHAR("Transfer"      ), 1 },
} );

EnumRecords<lang::Side>::Bootstrap(
{
    { lang::Side::Left                     , A_CHAR("Left" )         , 1 },
    { lang::Side::Right                    , A_CHAR("Right")         , 1 },
} );

EnumRecords<lang::Timezone>::Bootstrap(
{
    { lang::Timezone::Local                , A_CHAR("v"  )           , 1 },
    { lang::Timezone::UTC                  , A_CHAR("UTC")           , 1 },
} );

EnumRecords<lang::Whitespaces>::Bootstrap(
{
    { lang::Whitespaces::Trim              , A_CHAR("Trim")          , 1 },
    { lang::Whitespaces::Keep              , A_CHAR("Keep")          , 1 },
} );

EnumRecords<lang::Propagation>::Bootstrap(
{
    { lang::Propagation::Omit              , A_CHAR("Omit"         ) , 1 },
    { lang::Propagation::ToDescendants     , A_CHAR("ToDescendants") , 1 },
} );

EnumRecords<lang::Phase>::Bootstrap(
{
    { lang::Phase::Begin                   , A_CHAR("Begin")         , 1 },
    { lang::Phase::End                     , A_CHAR("End"  )         , 1 },
} );

EnumRecords<lang::Initialization>::Bootstrap(
{
    { lang::Initialization::Suppress         , A_CHAR("Suppress")    , 1 },
    { lang::Initialization::Perform          , A_CHAR("Perform" )    , 1 },
    { lang::Initialization::Suppress         , A_CHAR("NoInit"  )    , 1 },
    { lang::Initialization::Perform          , A_CHAR("Init"    )    , 1 },
    { lang::Initialization::Perform          , A_CHAR("Yes"     )    , 1 },
} );

EnumRecords<lang::Timing>::Bootstrap(
{
    { lang::Timing::Async                  , A_CHAR("Async"  )       , 1 },
    { lang::Timing::Sync                   , A_CHAR("Sync"   )       , 1 },
    { lang::Timing::Async                  , A_CHAR("Asynchronous")  , 1 },
    { lang::Timing::Sync                   , A_CHAR("Synchronous" )  , 1 },
    { lang::Timing::Sync                   , A_CHAR("Synchronized")  , 1 },
} );

EnumRecords<lang::Caching>::Bootstrap(
{
    { lang::Caching::Disabled              , A_CHAR("Disabled")      , 1 },
    { lang::Caching::Enabled               , A_CHAR("Enabled" )      , 1 },
    { lang::Caching::Auto                  , A_CHAR("Auto"    )      , 1 },
} );

#endif // ALIB_STRINGS


} // enums::Bootstrap()
#endif // !ALIB_CAMP




// #################################################################################################
// EnumRecordPrototype (Doxygen only)
// #################################################################################################

#if defined(ALIB_DOX)
/** ************************************************************************************************
 * This struct is \b not part of the library but only provided with the documentation of this
 * \alibmod_nl.
 *
 * The struct prototypes what module \alib_enums_nl \b expects from custom types that are
 * associated to enumerations as the type of  \ref alib_enums_records "ALib Enum Records".
 *
 * Usually, enum records are rather simple structs with fields of scalar or
 * \https{POD types,en.cppreference.com/w/cpp/types/is_pod} like, \c int, \c double or
 * \alib{strings,TString,String} and this way remain POD types themselves.
 *
 * When parsed or otherwise otherwise initialized, String members do not need to copy data to an
 * own buffer, because the input string for parsing
 * an instance, as well as the parameters of alternative constructors, are deemed to be static data.
 *
 * The life-cycle of instances of enumeration record is from bootstrapping an application
 * to its termination. After creation, the data can not be modified.
 **************************************************************************************************/
struct EnumRecordPrototype
{
    /**
     * Default constructor leaving the record undefined. For efficiency, this usually does
     * not initialize fields, as those will be overwritten by a subsequent invocation to #Parse.
     *
     * This constructor is only needed when method #Parse is given. Note that it is advised to
     * provide the parsing option and this way also this constructor.
     */
    EnumRecordPrototype()                                                       noexcept  = default;

    /**
     * Constructor accepting all necessary arguments to completely define the record.
     * This constructor is needed only in the case that records should be defined in a statical
     * way which is an alternative to implementing the definition by parsing an (externalized)
     * initialization strings. Static definition can be performed with method
     * \alib{enums,EnumRecords::Bootstrap(std::initializer_list<Initializer> definitions)}
     * but is not recommended and the definition from parsable stings is preferred.
     *
     * Note that the parameter's passed when this constructor is invoked, have to be of
     * "static nature". For example, the buffers and contents of passed string values are
     * deemed to survive the life-cycle of an application. Usually, C++ string literals are passed.
     *
     * @param myArg1   Assigned to the first custom field.
     * @param myArg2   Assigned to the second custom field.
     * @param ...      Further parameters, assigned to further fields.
     */
    EnumRecordPrototype(  const MyType1& myArg1, MyType2 myArg2, ... )                     noexcept;

    /**
     * Implementation has to parse the fields of this record from static interface struct
     * \alib{enums,EnumRecordParser}.
     *
     * For - usually simple - enum records, the process of parsing is limited to reading
     * values separated by delimiters. Convenient methods to do so are given by static type
     * \alib{enums,EnumRecordParser}. More complex parsing logic may be implemented by
     * using the "parser" substring found with \alib{enums,EnumRecordParser::Input} and further
     * of its entities.<br>
     * Please refer to the documentation of \alib{enums,EnumRecordParser} for all details.
     * A source code sample is given in chapter \ref alib_enums_records_resourced_parsing of the
     * Programmer's Manual of module \alib_enums_nl.
     *
     * The contents (buffer) of the string parsed is by contract of static nature. This means
     * that no copies of portions need to be allocated when used as a field value of string type.
     * This is in alignment with the static nature of \ref alib_enums_records "ALib Enum Records"
     * and their creation during bootstrap, either from C++ string literals or
     * \ref alib_basecamp_resources "ALib Externalized Resources", which comply to the same contract.
     *
     * On the same token, in case of an error, an implementation should raise an exception in
     * debug-compilations, as parsing is deemed to succeed on static data, even if externalized.
     */
    ALIB_API
    void Parse()                                                                           noexcept;
}; // EnumRecordPrototype
#endif // defined(ALIB_DOX)

}} // namespace [alib::enums]
