/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_RECORDPARSER
#define HPP_ALIB_ENUMS_RECORDPARSER 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)
ALIB_ASSERT_MODULE(STRINGS)


#if ALIB_STRINGS && !defined(HPP_ALIB_STRINGS_SUBSTRING)
#   include "alib/strings/substring.hpp"
#endif

namespace alib {  namespace enums {

/**
 * This is a pure static type used for parsing enum records from strings.
 * The static fields are initialized with each method of type \alib{enums,EnumRecords}
 * and therefore implementations of \alib{enums,EnumRecordPrototype::Parse} can rely on finding
 * these fields properly set.
 *
 * This concept makes use of the restriction, that the setup of
 * \ref alib_enums_records "ALib Enum Records" must exclusively be done during
 * \ref alib_manual_bootstrapping "bootstrapping" of a software process, where no multi-threading
 * is active, yet.
 *
 * If parse errors occur, a <c>std::runtime_error</c> is thrown.
 *
 * \note
 *   This is the only occasion where any \alib module throws an exception of type \c std.
 *   If (in presence of module \alib_basecamp) an \alib{lang,Exception} was thrown,
 *   this had to be resourced likewise.
 *   The rational for this design decision is that enum records usually are defined by
 *   potentially externalized resources. The maintenance of such resources must not lead to
 *   corrupted exceptions that are thrown when parsing those resources.
 *   Consequently, the exception messages are using english language and are not externalized.
 *   An end user will not be faced with these exception unless an errorneous "roll-out"
 *   of a software using malicious externalized resources was performed.
 *
 * The methods of this type perform strict checks about unnecessary whitespaces, <c>'+'</c>-signs,
 * etc, which also raise an exception. The rationale for this design decision
 * (to not allow unnecessary, but maybe unharmful characters in resources) besides saving memory is,
 * that the occurrence of unnecessary characters, indicates an errorneous maintenance of
 * externalized resources, and such should be detected as soon as possible.
 *
 * The main interface into this class is given with the overloaded #Get methods.
 * Enum Records are usually rather simple structs, consisting of strings, characters, integers
 * and floating point values. If so, a simple list of invocations of #Get for each
 * field of the struct has to be placed in implementations of method
 * \alib{enums,EnumRecordPrototype::Parse}. For the last field, optional parameter
 * \p{isLastField} has to be given as \c true4.
 * As an example, the following code shows the implementation of \alib{config,VariableDecl::Parse}:
 * \snippet "alib/config/variable.cpp"     DOX_ALIB_ENUMS_RECORD_PARSER
 * This parse method omits reading the value for inherited field
 * \alib{enums,ERSerializable::MinimumRecognitionLength}, and sets it to fixed \c 0 instead.
 * This is often an option when custom enumeration record types
 * are derived from \b ERSerializable but are not supposed to be deserialized or de-serialization
 * forcing the whole string to read is acceptable.
 * Note that it is still is possible to deserialize elements, but not with abbreviated names.
 *
 * An implementation for a custom record type might perform more complex parsing operations.
 * For this, some helper methods are publicly provided and finally, the direct use of the
 * substring #Input is likewise allowed.
 */
struct EnumRecordParser
{
    /** The remaining input string.                                */
    static ALIB_API Substring   Input;

    /** The delimiter of fields of a record.                       */
    static ALIB_API character   InnerDelimChar;

    /** The delimiter of records.                                  */
    static ALIB_API character   OuterDelimChar;

    /** A backup of the originally given string to parse.          */
    static ALIB_API String      OriginalInput;

    /** The resource category (if a resourced string was parsed).  */
    static ALIB_API NString     ResourceCategory;

    /** The resource name (if a resourced string was parsed).      */
    static ALIB_API NString     ResourceName;

    /**
     * Initializes this parser. This is done once prior to reading a resource (or static) string
     * with enum record definitions.
     *
     * @param input            The input string.
     * @param innerDelim       The delimiter of fields of a record.
     * @param outerDelim       The delimiter of records.
     * @param resourceCategory The delimiter of records.
     * @param resourceName     The delimiter of records.
     */
    ALIB_API static void Initialize( const String& input            ,
                                     character     innerDelim       , character      outerDelim,
                                     const NString& resourceCategory, const NString& resourceName )
    {
        // Test for double initialization call, what is not easily avoidable for resourced strings.
        // This protects the resource information, of the first call.
        if( input.Buffer() != Input.Buffer() || Input.IsNull() )
        {
            Input            =
            OriginalInput    = input;
            InnerDelimChar   = innerDelim;
            OuterDelimChar   = outerDelim;
            ResourceCategory = resourceCategory;
            ResourceName     = resourceName;
            if( input.IsNull () ) error( "Input string is nulled" );
            if( input.IsEmpty() ) error( "Input string is empty"  );
        }
    }

    // #############################################################################################
    // Helpers
    // #############################################################################################

    /**
     * Raises an error.
     * @param what     Textual description about what has bien tried to to.
     */
    ALIB_API static void    error[[noreturn]] (const NCString& what);

    /**
     * Asserts that no whitespaces follow in input.
     * @param where Textual description about what is currently done.
     */
    ALIB_API static void    assertNoWhitespaces(const NCString& where);

    /**
     * Asserts that no trailing whitespaces are in \p{token}.
     * @param token The string to test for trailing whitespaces.
     */
    ALIB_API static void    assertNoTrailingWhitespaces(String& token);


    /**
     * Asserts that a specific redundant character is not given, for example a leading
     * <c>'+'</c> sign for an integral value.
     * @param specificChar The character to check.
     * @param where        Textual description about what is currently done.
     */
    ALIB_API static void    assertNoUnnecessary(character specificChar, const NCString& where);

    /**
     * Asserts that either \p{specificChar} follows. The character will also be consumed.
     * @param specificChar The character to consume next.
     * @param where        Textual description about what is currently done.
     */
    ALIB_API static void    assertChar(character specificChar, const NCString& where);

    /**
     * Asserts that either #Input is empty, or an #OuterDelimChar follows.
     */
    ALIB_API static void    assertEndOfRecord();

    /**
     * Asserts that the #Input is empty.
     */
    ALIB_API static void    assertEndOfInput();

    /**
     * Used by #Get(TIntegral&, bool).
     * Reads special values \b min, \b max and \b ^N. The latter provides the power of two.
     * @param isLastField  Has to be given as \c true, if this is the last field to parse,
     *                     otherwise \c false.
     * @return The value parsed.
     */
    ALIB_API static integer getInteger( bool isLastField );


    // #############################################################################################
    // Main methods for parsing fields.
    // #############################################################################################
    /**
     * Parses a field of string type. Parsing ends with an #InnerDelimChar.
     * The string is checked for not containing leading or trailing whitespaces.<br>
     *
     * @param result       A reference to the record's field to be read.
     * @param isLastField  Has to be given as \c true, if this is the last field to parse.
     *                     Defaults to \c false.
     */
    ALIB_API static void     Get( String& result, bool isLastField= false );

    /**
     * Parses a field of character type.
     *
     * @param result       A reference to the record's field to be read.
     * @param isLastField  Has to be given as \c true, if this is the last field to parse.
     *                     Defaults to \c false.
     */
    ALIB_API static void     Get( character& result, bool isLastField= false );

#if defined(ALIB_DOX)
    /**
     * Parses an integral field of a record. Accepts <c>'-'</c> signs, while <c>'+'</c>
     * is considered an unnecessary token.
     * Furthermore asserts that at least one digit was parsed.
     *
     * Allows special values \b min, \b max and \b ^N. The latter provides the power of two.
     *
     * @tparam TIntegral    The integer type to parse.
     *                      Must be statically castable from \b uint64_t.
     *                      Deduced by the compiler.
     * @param  result       A reference to the record's field to be read.
     * @param  isLastField  Has to be given as \c true, if this is the last field to parse.
     *                      Defaults to \c false.
     */
    template<typename TIntegral>
    static inline
    void Get( TIntegral& result, bool isLastField= false );
#else
    template<typename TIntegral>
    static
    ATMP_VOID_IF( ATMP_IS_INT(TIntegral) )
    Get( TIntegral& result, bool isLastField= false )
    {
        integer  bigInt= getInteger( isLastField );
             if( bigInt == (std::numeric_limits<integer>::max)() )     result= (std::numeric_limits<TIntegral>::max)();
        else if( bigInt == (std::numeric_limits<integer>::min)() )     result= (std::numeric_limits<TIntegral>::min)();
        else                                                           result= static_cast<TIntegral>( bigInt );
    }
#endif // defined(ALIB_DOX)

#if defined(ALIB_DOX)
    /**
     * Parses an enumeration element value, which has to be given in the source string by its
     * underlying integral value. (It is not possible to parse "named" enumerations in
     * bootstrapping, yet.)
     *
     * \note
     *   This method uses #Get(TIntegral&, bool) to read the value and thus special values
     *   \b min, \b max and \b ^N are allowed
     *
     * @tparam TEnum        The enum type to parse an element from.
     * @param  result       A reference to the integer variable to parse.
     * @param  isLastField  Has to be given as \c true, if this is the last field to parse.
     *                      Defaults to \c false.
     */
    template<typename TEnum>
    static inline
    void Get( TEnum& result, bool isLastField= false );
#else
    template<typename TEnum>
    static
    ATMP_VOID_IF( std::is_enum<TEnum>::value )
    Get( TEnum& result, bool isLastField= false )
    {
        typename std::underlying_type<TEnum>::type resultIntegral;
        EnumRecordParser::Get( resultIntegral, isLastField );
        result= TEnum( resultIntegral );
    }
#endif // defined(ALIB_DOX)

    /**
     * Parses the next floating point field of a record. Accepts <c>'-'</c> signs, while <c>'+'</c>
     * is considered an unnecessary token.
     * Furthermore asserts that at least one digit was parsed.
     *
     * @param result       A reference to the record's field to be read.
     * @param isLastField  Has to be given as \c true, if this is the last field to parse.
     *                     Defaults to \c false.
     */
    ALIB_API static void     Get( double& result, bool isLastField= false );


    /**
     * Removes an #InnerDelimChar.
     * The #Input is checked for not containing whitespaces before or after the delimiter.
     */
    ALIB_API static void     Delim();

    /**
     * Removes an #OuterDelimChar.
     * The #Input is checked for not containing whitespaces before or after the delimiter.
     */
    ALIB_API static void     OuterDelim();

};

}} // namespace [alib::enums]

#endif // HPP_ALIB_ENUMS_RECORDPARSER
