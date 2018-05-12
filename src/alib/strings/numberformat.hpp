// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include guard
#ifndef HPP_ALIB_STRINGS_NUMBERFORMAT
#define HPP_ALIB_STRINGS_NUMBERFORMAT 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(HPP_ALIB_STRINGS_STRING)
    #error "include 'alib/alib.hpp' instead of this header"
#endif

namespace aworx { namespace lib { namespace strings {
/** ***********************************************************************************************
 * Defines various values needed to convert integer and floating point values
 * to string representations and vice versa.
 * In addition, this class provides methods that do such conversions on character array
 * buffers.
 *
 * \attention
 *   The methods of this class are not intended for the common use. Instead, use interface of
 *   classes
 *      \alib{strings,AStringBase,AString},
 *      \alib{strings,SubstringBase,Substring} or
 *      \alib{strings::format,FormatterBase,Formatter}
 *   to write and parse numbers, which accept an object of this type as parameters.
 *
 * <b>Defined Singletons and User-Defined Instances:</b><br>
 * Two static singletons of this class are defined which can be used wherever a number format object
 * is needed as a parameter:
 * - #Global<br>
 *   This is initialized with method
 *   \ref aworx::lib::ALib::Init "ALib::Init" and reflects local specific settings.
 *
 * - #Computational<br>
 *   This is also initialized with method
 *   \ref aworx::lib::ALib::Init "ALib::Init" and should be used for writing and parsing
 *   numbers which are intended to be readable by software. Its' decimal point character
 *   is set to \c '.', the international standard. Furthermore no group separators are set for
 *   decimal and decimal floating point as well as for binary, hexadecimal and octal conversion.
 *
 * User defined objects default to the 'computational' setting after construction.
 *
 * <b>Output Formats:</b><br>
 * The following conversion formats are supported
 *
 * - \b Decimal<br>
 *   Supports optional minimum output width with field #DecMinimumFieldWidth,
 *   and definable thousands grouping character with field #ThousandsGroupChar, which can be
 *   activated with field #WriteGroupChars.<br>
 *   Furthermore, the plus-sign can be controlled (#PlusSign) to be either left out or be anything
 *   defined. Of-course, values \b ' ' and \b '+' are reasonable options.
 *
 * - <b>Binary</b><br>
 *   Binary output supports up to 64 digits and different group separators for nibbles, bytes,
 *   16-bit words and 32 bit words. (See #BinNibbleGroupChar, #BinByteGroupChar,
 *   #BinWordGroupChar and #BinWord32GroupChar ).<br>
 *   When parsing integers, a customizable literal string defined in #BinLiteralPrefix might be used
 *   to auto-detect binary values.
 *
 * - <b>Hexadecimal</b><br>
 *   Hexadecimal output supports up to 16 digits (64-bit) and different group separators
 *   for bytes, 16-bit words and 32 bit words. (See #HexByteGroupChar, #HexWordGroupChar,
 *   and #HexWord32GroupChar).
 *   When parsing integers, a customizable literal string defined in #HexLiteralPrefix might be used
 *   to auto-detect hexadecimal values.
 *
 * - <b>Octal</b><br>
 *   Octal output supports up to 22 digits (64-bit) and different a group separator for groups
 *   of three digits defined with #OctGroupChar.
 *   When parsing integers, a customizable literal string defined in #OctLiteralPrefix might be used
 *   to auto-detect hexadecimal values.
 *
 * - <b>Floating Point</b><br>
 *   The width of the output is provided in two fields,
 *   #IntegralPartMinimumWidth and #FractionalPartWidth. While the integral part is
 *   a minimum width (and nothing is ever cut), the fractional part is fixed width.
 *   Values with higher fractional precision are rounded accordingly.
 *   Note that the parameter of the interface methods that may override the width, in the floating
 *   point case only affects the minimum width of the integral part.<br>
 *   The integral and fractional part of float values are separated by decimalPointChar.
 *   This field of-course has to be different from group separator #ThousandsGroupChar, which can
 *   be activated using field #WriteGroupChars.<br>
 *   Other important fields for writing and parsing floats are: #ExponentSeparator,
 *   #INFLiteral, #NANLiteral, #WriteExponentPlusSign, and #ForceScientific.
 *
 * <b>Notes on Writing and Parsing Values:</b><br>
 * \attention
 *   For decimal output, the width (#DecMinimumFieldWidth) is a minimum width. This mean, that
 *   bigger numbers are written in a higher width.
 *   This is \c not true for binary, hexadecimal and octal output. In these formats, the width
 *   is an absolute value. Higher digits of numbers are just not written. The advantage of this
 *   is that no masking is needed when just the lower part of an integer number should be written.
 *   However, numbers might of-course change when cut and parsed back later!
 *
 * All of the integer formats have in common that the output width given is including optional
 * grouping characters. For example if a width of \b 5 was given for decimal output, the value
 * \c 12 would be written \c "0,012", hence \b 4 digits plus the grouping character. If grouping
 * was disabled, the output became \c "00012", hence one digit more but the same width.
 * In contrast to that, any sign that is written is \c not counted in the width.
 *
 * When parsing values, grouping characters are ignored at any position within the digits,
 * except of the start. The same is true for whitespace characters as defined in
 * #Whitespaces. When this field is nulled or empty, then white spaces are \b not ignored.
 * This might be helpful in some cases where occurrence of white space characters should
 * indicate an error (or something else) when parsing.
 * Otherwise, the characters defined in this field are ignored at two places: at the beginning
 * of a parsing operation and after a sign character was read.
 *
 * When parsing fails, a value of \c 0 (respectively \c 0.0) is returned.
 * User-friendly classes that use the interface of this type will detect such failure
 * through the output parameter of the parsing methods, which indicates the end of the number
 * found.
 *
 * For each of the four integer formats, decimal, binary, hexadecimal and octal, dedicated
 * parsing methods exist. Those do not accept literal prefix identifiers as defined in
 * fields #BinLiteralPrefix, #HexLiteralPrefix and #OctLiteralPrefix. Those are
 * identified by method #ParseInt, which aggregates the other four parsing methods.<br>
 * There is no corresponding method defined that writes the literal prefix. When writing
 * binary, hexadecimal or octal values, such prefixes have to be prepended 'manually'.
 *
 * @tparam TChar    The character type. Implementations for \c char and \c wchar_t are provided
 *                  with type definitions \ref aworx::NNumberFormat and \ref aworx::WNumberFormat.
 **************************************************************************************************/
template<typename TChar>
class NumberFormatBase
{
    // #############################################################################################
    //  public Fields
    // #############################################################################################
    public:

    /**
     * The default static number format object that acts as the
     * default settings of the currently running process.<br>
     * Method
     * \ref aworx::lib::ALib::Init "ALib::Init" invokes #SetFromLocale() on this object and
     * switches grouping on.
     *
     * Classes providing functionality based on this class, might use this as a default
     * value for parameters of their interface methods.
     */
    static              NumberFormatBase   Global;

    /**
     * A static number format object that may be used to write and parse numbers for 'computational'
     * use, which means, that grouping is switched off and decimal point character
     * is \c '.'.<br>
     * Method
     * \ref aworx::lib::ALib::Init "ALib::Init" invokes #SetComputational on this object.
     * Note that using code that use this field without having invoked <b>ALIB.init</b> may
     * behave wrongly.
     *
     * Classes providing functionality based on this class, might use this as a default
     * value for parameters of their interface methods.
     */
    static              NumberFormatBase   Computational;

    // ###############################   Parsing  ###################################
    /**
     * Defines whitespace characters that are ignored when leading the number and after
     * the sign-character. Applies to methods #ParseInt and #ParseFloat. In contrast, methods
     * #ParseDec, ParseBin, ParseHex and ParseOct do not ignore any whitespace characters.
     */
    TStringBase<TChar>  Whitespaces;

    /**
     * Denotes if grouping characters are ignored when parsing numbers if they are given
     * (not set to \c '\0').
     * This applies to all number types.<br>
     * Defaults to \c false. If set to \c true, grouping characters are just skipped when
     * found while parsing numbers, no matter at which position they occur.
     */
    bool                ReadGroupChars;

    // ###############################   Writing  ###################################
    /**
     * Denotes if grouping characters are written if they are given (not set to \c '\0').
     * This applies to all number types.<br>
     * Defaults to \c false.
     */
    bool                WriteGroupChars;

    // ############################ Floating point  numbers ###############################
    /**
     * Defines the decimal point character when converting a floating point number to a string
     * representation with method #WriteFloat. Also, method #ParseFloat uses the
     * character provided in this field for parsing the character.<br>
     * The field defaults to '.'. By invoking #SetFromLocale(), the current locale's setting is
     * determined.
     */
    TChar               DecimalPointChar;

    /**
     * Defines the decimal exponent symbol of string representations of floating point numbers
     * when written or parsed in scientific notation by methods #ParseFloat and
     * #WriteFloat.<br>
     * Method #ParseFloat accepts characters 'e' and 'E' in addition to the character
     * set in this field.<br>
     * Defaults to \c 'E'.
     */
    TStringBase<TChar>  ExponentSeparator;

    /// Defines what is written and parsed for infinite double values.
    TStringBase<TChar>  INFLiteral;

    /// Defines what is written and parsed for double values that represent "not a number".
    TStringBase<TChar>  NANLiteral;

    /**
     * If \c true, the decimal point of floating point values is written, even if the fractional
     * part of the float value is zero. If \c false, in this case the decimal point is omitted.<br>
     * Defaults to \c true.
     */
        bool            ForceDecimalPoint;

    /**
     * Determines if positive exponent values are prepended with an explicit '+' character when
     * written using #WriteFloat.<br>
     * Defaults to \c false, as some systems will not accept a plus sign on the exponent value.
     * Note that field #PlusSign is not applicable for exponent numbers.
     */
    bool                WriteExponentPlusSign;

    /**
     * Defines the minimum digits written for the integral part when converting a floating point
     * value into a string.<br>
     * If the integral part of the number provided has less digits
     * then leading '0' digits are added.<br>
     * The maximum value allowed is 15.<br>
     * A value of 0 leads to omitting the '0' before the
     * decimal separator in the case the value is below 1.0 and higher than -1.0 <br>
     * The default value is -1, which writes a minimum of 1 digit.
     *
     * When either this field or field #FractionalPartWidth is set to a positive value,
     * the limits to switch to scientific notation, which otherwise are fixed \c 10E-04 and
     * \c 10E+06, get extended. Method #WriteFloat in this case keeps non-scientific notation
     * established if possible.
     */
    int                 IntegralPartMinimumWidth;

    /**
     * Defines the number of digits written for the fractional part when converting a floating point
     * value into a string. (For integer conversion, see #DecMinimumFieldWidth.)<br>
     * If the fractional part of the number provided has less digits then trailing '0'
     * digits are added.<br>
     * If the fractional part of the number provided has more digits then the fractional part is
     * rounded accordingly.<br>
     * The maximum value allowed is 15.<br>
     * The default value is -1, which writes as many digits as available in the provided float
     * variable, with a minimum of 1 digit.
     *
     * When either this field or field #IntegralPartMinimumWidth is set to a positive value,
     * the limits to switch to scientific notation, which otherwise are fixed \c 10E-04 and
     * \c 10E+06, get extended. Method #WriteFloat in this case keeps non-scientific notation
     * established if possible.
     */
    int                 FractionalPartWidth;

    /**
     * If this field is \c true, then trailing \c '0' digits in the fractional part of a floating
     * point value are not written, even if a #FractionalPartWidth is set.
     * Defaults to \c false.
     */
    bool                OmitTrailingFractionalZeros;

    /**
     * If \c true, scientific format is always used.<br>
     * If \c false (the default), method #WriteFloat writes scientific format only if both fields,
     * #IntegralPartMinimumWidth and #FractionalPartWidth are evaluating to -1 and only for
     * numbers smaller than \c 10E-04 or larger than \c 10E+06.<br>
     *
     * If one of the fields #IntegralPartMinimumWidth or #FractionalPartWidth is set to a positive
     * value, these limits get extended. Method #WriteFloat in this case keeps non-scientific
     * notation established if possible.
     */
    bool                ForceScientific;

    // ############################ Float and integer numbers ###############################
    /**
     * Determines if positive values are prepended with an explicit character (usually '+') when
     * written using #WriteFloat or #WriteDecSigned.<br>
     * Defaults to \c 0 which omits the writing. Usual other values are of-course \c '+', but
     * also  <c>' '</c> (space) which supports better horizontal alignment of numbers when written in
     * columns. Note that this is not affecting exponent decimals of floating point values.
     * For those, see #WriteExponentPlusSign
     */
    TChar               PlusSign;

    /**
     * Defines the separator character for thousands when converting a number to a string
     * representation. In addition, this is used to identify thousand group symbols when
     * parsing decimal values. If set to \c '\0', no group separator is written and also when
     * parsing, a group separator is not accepted.
     * If set, still #WriteGroupChars controls if it is written.<br>
     * Defaults to \c ','.
     * By invoking #SetFromLocale(), the current locale's setting is determined.
     *
     */
    TChar               ThousandsGroupChar;

    /**
     * This character is written instead of a grouping character in the case that a certain
     * output width is requested but a grouping character would be the first character to write.
     * Writing this character instead, assures the field width to be as requested.
     * Defaults to space (<c>' '</c>).
     */
    TChar               LeadingGroupCharReplacement;


    // ################################  Base10 Integer numbers ####################################
    /**
     * Defines the minimum digits and grouping symbols written when writing integers in decimal.
     * format. If the value to write has less digits (and grouping symbols), then leading '0'
     * digits (and eventually grouping symbols) are added.<br> If the value to write has more
     * digits, then this field is ignored.
     * A sign character is not calculated into the writing width. To have negative and positive
     * numbers resulting in the same width, #PlusSign has to be set to a value unequal to \c '\0'
     * (usually space character <c>' '</c> or \c '+').
     *
     * If this field is negative, it is ignored. Defaults to \c -1.
     */
    int                 DecMinimumFieldWidth;

    // ###################################  Binary numbers #########################################
    /**
     * Defines the digits written when writing binary values.
     * If the value has less digits, then leading '0' digits are added. If it has more, than
     * those digits are \b NOT written (!).<br>
     * The default value and minimum value is \c -1, which writes as many bits as necessary.
     */
    int                 BinFieldWidth;

    /** Defines the separator character for nibbles (4 bits) of binary numbers.
     *  Defaults to \c '\0' what disables reading and writing of nibble group characters.  */
    TChar               BinNibbleGroupChar;

    /** Defines the separator character for bytes of binary numbers.
     *  Defaults to \c '\0' what chooses #BinNibbleGroupChar.  */
    TChar               BinByteGroupChar;

    /** Defines the separator character for 16-bit words  of binary numbers.
     *  Defaults to \c '\0' what chooses #BinByteGroupChar.  */
    TChar               BinWordGroupChar;

    /** Defines the separator character for 32-bit words  of binary numbers.
     *  Defaults to \c '\0' what chooses #BinWordGroupChar.  */
    TChar               BinWord32GroupChar;

    /** Used by method #ParseInt to detect binary format of integer values. If nulled, no binary
     *  format is detected.
     *  Methods of this class are not writing the prefix. If desired, this has performed
     *  explicitly ("manually") prior to invoking a writing method of this class. <br>
     *  Defaults to \c "0b". */
    TStringBase<TChar>  BinLiteralPrefix;

    // ###################################  Hex numbers #########################################
    /**
     * Defines the digits written when writing hexadecimal values.
     * If the value has less digits, then leading '0' digits are added. If it has more, than
     * those digits are \b NOT written (!).<br>
     * The default value and minimum value is \c -1, which writes as many bits as necessary.
     */
    int                 HexFieldWidth;

    /**
     * If \c true, lower case letters \c 'a' - \c 'f' are written.
     * Defaults to \c false, which writes upper case letters \c 'A' - \c 'F'.
     */
    bool                HexLowerCase;

    /** Defines the separator character for bytes of hexadecimal numbers. Defaults to \c 0,
     *  Defaults to \c '\0' what disables reading and writing of byte group characters.  */
    TChar               HexByteGroupChar;

    /** Defines the separator character for 16-bit words  of hexadecimal numbers.
     *  Defaults to \c '\0' what chooses #HexByteGroupChar.  */
    TChar               HexWordGroupChar;

    /** Defines the separator character for 32-bit words  of hexadecimal numbers.
     *  Defaults to \c '\0' what chooses #HexWordGroupChar.  */
    TChar               HexWord32GroupChar;

    /** Used by method #ParseInt to detect hexadecimal format of integer values. If nulled, no
     *  hexadecimal format is detected.
     *  Methods of this class are not writing the prefix. If desired, this has performed
     *  explicitly ("manually") prior to invoking a writing method of this class. <br>
     *  Defaults to \c "0x". */
    TStringBase<TChar>  HexLiteralPrefix;


    // ###################################  Octal numbers #########################################
    /**
     * Defines the digits written when writing hexadecimal values.
     * If the value has less digits, then leading '0' digits are added. If it has more, than
     * those digits are \b NOT written (!).<br>
     * The default value and minimum value is \c -1, which writes as many bits as necessary.
     */
    int                 OctFieldWidth;

    /** Defines the separator character for bytes of hexadecimal numbers.
     *  Defaults to \c '\0' what disables reading and writing of byte group characters.  */
    TChar               OctGroupChar;

    /** Used by method #ParseInt to detect octal format of integer values. If nulled, no octal
     *  format is detected.
     *  Methods of this class are not writing the prefix. If desired, this has performed
     *  explicitly ("manually") prior to invoking a writing method of this class.<br>
     *  Defaults to \c "0o". */
    TStringBase<TChar>  OctLiteralPrefix;

    // #############################################################################################
    //  Interface
    // #############################################################################################

    /** ********************************************************************************************
     * Constructor. Invokes #SetComputational to reset all fields to their default values.
     **********************************************************************************************/
    inline   NumberFormatBase() { SetComputational(); }

    /** ********************************************************************************************
     * Resets the object to its default values. This method is called in the constructor.
     *
     * Decimal point character and grouping characters are set as follows:
     * <center>Field</center>         | <center>Value</center>
     * - - - - - - - - - - - - - - - -| - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - -
     * #DecimalPointChar              |  \c .
     * #ThousandsGroupChar            |  \c ,
     * #BinNibbleGroupChar            |  \c '
     * #BinByteGroupChar              |  \c -
     * #BinWordGroupChar              |  <c>' '</c> (space)
     * #BinWord32GroupChar            |  \c #
     * #HexWordGroupChar              |  \c '
     * #HexWord32GroupChar            |  \c '
     * #HexByteGroupChar              |  \c 0 (none)
     * #OctGroupChar                  |  \c '
     *
     * The literal attributes are set as follows:
     * <center>Field</center>         | <center>Value</center>
     * - - - - - - - - - - - - - - - -| - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - -
     * #ExponentSeparator             |  \c "E"
     * #INFLiteral                    |  \c "INF"
     * #NANLiteral                    |  \c "NAN"
     * #BinLiteralPrefix              |  \c "0b"
     * #HexLiteralPrefix              |  \c "0x"
     * #OctLiteralPrefix              |  \c "0o"
     *
     * All width-attributes are reset to "automatic mode", \c -1. The attributes are
     * #IntegralPartMinimumWidth,
     * #FractionalPartWidth,
     * #DecMinimumFieldWidth,
     * #BinFieldWidth,
     * #HexFieldWidth and
     * #OctFieldWidth.
     *
     * Finally, the following further fields are reset to their default values:
     * <center>Field</center>         | <center>Value</center>
     * - - - - - - - - - - - - - - - -| - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - -
     * #WriteGroupChars               |  \c false
     * #ForceScientific               |  \c false
     * #ForceDecimalPoint             |  \c true
     * #PlusSign                      |  \c none (0)
     * #WriteExponentPlusSign         |  \c false
     * #OmitTrailingFractionalZeros   |  \c false
     * #HexLowerCase                  |  \c false
     * #Whitespaces                   |  #aworx::DefaultWhitespaces
     *
     *
     * \note
     *   With static object
     *   \ref aworx::lib::strings::NumberFormatBase::Computational "NumberFormatBase::Computational",
     *   there is a global singleton existing which can be used but must not be changed.
     **********************************************************************************************/
    ALIB_API   void    SetComputational();

    /** ********************************************************************************************
     * Copies all fields (settings) from the given object. If no object is provided, values of
     * the static singleton found in field #Global are copied
     *
     * @param other  The \b %NumberFormat object to copy the values from.
     *               Defaults to \c nullptr, which chooses the global singleton.
     **********************************************************************************************/
    ALIB_API  void    Set( NumberFormatBase* other =nullptr );

    /** ********************************************************************************************
     * Sets the field #DecimalPointChar and #ThousandsGroupChar to reflect the current
     * system locale setting. No other values are changed.
     *
     * \note
     *   Static (global) object
     *   \ref aworx::lib::strings::NumberFormatBase::Global "NumberFormatBase::Global",
     *   implements an instance which has the right locale set (provided that
     *   \ref aworx::lib::ALib::Init "ALib::Init"
     *   was duly invoked by the process).
     *   Otherwise, this method might be used to initialize a custom object with default values
     *   to afterwards make some specific changes.
     **********************************************************************************************/
    ALIB_API   void    SetFromLocale();


    // #############################################################################################
    //  Integer conversion
    // #############################################################################################

    /** ********************************************************************************************
     * Static method to read digits \c '0' to \c '9' into a positive integer value.
     *
     * @param         src   The string to read the value from.
     * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
     *                      behind the last character consumed. If unchanged, this indicates
     *                      that no parsable number was found.
     * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
     *         first character behind the parsed number.
     **********************************************************************************************/
    ALIB_API
    static
    uint64_t  ParseDecDigits( const StringBase<TChar>& src, integer& idx );

    /** ********************************************************************************************
     * Parses signed integer numbers, optionally in binary, hexadecimal or octal format.
     *
     * Leading characters defined in #Whitespaces are ignored.
     * An optional sign character \c '+' or \c '-' is parsed. If found, again whitespace characters
     * may follow behind such sign and are ignored.
     *
     * Then, the method detects any literal prefixes as defined in fields #BinLiteralPrefix,
     * #HexLiteralPrefix and #OctLiteralPrefix (usually \c 0b, \c 0x and \c 0o) and
     * invokes one of the methods #ParseDec, #ParseBin, #ParseHex or #ParseOct.
     *
     * @param         src   The string to read the value from.
     * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
     *                      behind the last character consumed. If unchanged, this indicates
     *                      that no parsable number was found.
     * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
     *         first character behind the parsed number.
     **********************************************************************************************/
    ALIB_API
    int64_t   ParseInt( const StringBase<TChar>& src, integer& idx )                          const;

    /** ********************************************************************************************
     * Reads an unsigned integer value in \b decimal format from the given character
     * array at the given position.<br>
     * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
     * Whitespace and grouping characters, as defined in fields #Whitespaces and #ThousandsGroupChar
     * will be ignored, regardless on their position between digits. To suppress the parsing
     * of group characters, set field #ThousandsGroupChar to <c>'\\0'</c>. To suppress whitespace
     * consumption before reading the value, set field #Whitespaces to nulled or empty string.
     *
     *
     * @param         src   The string to read the value from.
     * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
     *                      behind the last character consumed. If unchanged, this indicates
     *                      that no parsable number was found.
     * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
     *         first character behind the parsed number.
     **********************************************************************************************/
    ALIB_API
    uint64_t  ParseDec( const StringBase<TChar>& src, integer& idx )                          const;

    /** ********************************************************************************************
     * Reads an unsigned integer value in \b binary format from the given character
     * array at the given position.<br>
     * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
     * Whitespace and grouping characters, as defined in fields #Whitespaces,
     * #BinNibbleGroupChar, #BinByteGroupChar, #BinWordGroupChar and #BinWord32GroupChar
     * will be ignored, regardless on their position between digits. To suppress the parsing
     * of group characters, set the fields to <c>'\\0'</c>. To suppress whitespace consumption,
     * set field #Whitespaces to nulled or empty string.<br>
     *
     * @param         src   The string to read the value from.
     * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
     *                      behind the last character consumed. If unchanged, this indicates
     *                      that no parsable number was found.
     * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
     *         first character behind the parsed number.
     **********************************************************************************************/
    ALIB_API
    uint64_t  ParseBin( const StringBase<TChar>& src, integer& idx )                          const;

    /** ********************************************************************************************
     * Reads an unsigned integer value in \b hexadecimal format from the given character
     * array at the given position.<br>
     * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
     * Whitespace and grouping characters, as defined in fields #Whitespaces,
     * #HexByteGroupChar, #HexWordGroupChar and #HexWord32GroupChar
     * will be ignored, regardless on their position between digits. To suppress the parsing
     * of group characters, set the fields to <c>'\\0'</c>. To suppress whitespace consumption,
     * set field #Whitespaces to nulled or empty string.<br>
     *
     * Letters 'a' to 'f' are parsed ignoring their case, independent from the setting of field
     * #HexLowerCase.
     *
     * @param         src   The string to read the value from.
     * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
     *                      behind the last character consumed. If unchanged, this indicates
     *                      that no parsable number was found.
     * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
     *         first character behind the parsed number.
     **********************************************************************************************/
    ALIB_API
    uint64_t  ParseHex( const StringBase<TChar>& src, integer& idx )                          const;

    /** ********************************************************************************************
     * Reads an unsigned integer value in \b binary format from the given character
     * array at the given position.<br>
     * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
     * Whitespace and grouping characters, as defined in fields #Whitespaces and #OctGroupChar
     * will be ignored, regardless on their position between digits. To suppress the parsing
     * of group characters, set the field to <c>'\\0'</c>. To suppress whitespace consumption,
     * set field #Whitespaces to nulled or empty string.<br>
     *
     * @param         src   The string to read the value from.
     * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
     *                      behind the last character consumed. If unchanged, this indicates
     *                      that no parsable number was found.
     * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
     *         first character behind the parsed number.
     **********************************************************************************************/
    ALIB_API
    uint64_t  ParseOct( const StringBase<TChar>& src, integer& idx )                          const;

    /** ********************************************************************************************
     * Reads a floating point value from the given character array at the given position.<br>
     * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
     * and parsing will fail if found.<br>
     *
     * If the strings defined in fields #NANLiteral and #INFLiteral are found, corresponding
     * double constants (not a number, positive/negative infinity) will be returned.
     *
     * @param         src   The string to read the value from.
     * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
     *                      behind the last character consumed. If unchanged, this indicates
     *                      that no parsable number was found.
     * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
     *         first character behind the parsed number.
     **********************************************************************************************/
    ALIB_API
    double ParseFloat( const StringBase<TChar>& src, integer& idx )                           const;

    /** ********************************************************************************************
     * Converts the given long value to a string representation into a signed decimal format.<br>
     * For negative numbers, \c '-' is written, the sign of positive numbers (if any) depends
     * on field #PlusSign. After that, the value is converted to positive and
     * #WriteDecUnsigned is invoked.
     *
     * @param value         The value to write.
     * @param buffer        The character array to write the value to. Needs to be long enough
     *                      (after \p{idx}) to carry the string written.
     * @param idx           The index within \p{buffer} to start writing.
     * @param overrideWidth May be used to override value of field #DecMinimumFieldWidth.
     *                      Defaults to \c 0, which denotes to  use the fields' value.
     * @return  The index pointing to behind the last character written in \b buffer.
     **********************************************************************************************/
    ALIB_API
    integer  WriteDecSigned( int64_t value, TChar* buffer, integer idx, int overrideWidth  )  const;

    /** ********************************************************************************************
     * Converts the given long value to a string representation in decimal format.<br>
     * Negative numbers have to be converted to positive values when invoking this method.
     * the digits and optional grouping characters. The method does not check any overflow within
     * the given character buffer.
     *
     * The maximum number of digits written is 20. In addition, grouping characters may be written
     * according the settings of fields #WriteGroupChars, #ThousandsGroupChar and
     * #LeadingGroupCharReplacement.
     *
     * The minimum width of the output is taken from field #DecMinimumFieldWidth unless overwritten
     * by parameter \p{overrideWidth}. If the width is greater than the sum of digits and
     * grouping characters found in \p{value}, then \c '0' digits are prepended between the sign and
     * the number.
     *
     * @param value         The value to write.
     * @param buffer        The character array to write the value to. Needs to be long enough
     *                      (after \p{idx}) to carry the string written.
     * @param idx           The index within \p{buffer} to start writing.
     * @param overrideWidth May be used to override value of field #DecMinimumFieldWidth.
     *                      Defaults to \c 0, which denotes to  use the fields' value.
     * @return  The index pointing to behind the last character written in \b buffer.
     **********************************************************************************************/
    inline
    integer  WriteDecUnsigned( uint64_t value, TChar* buffer, integer idx, int overrideWidth ) const
    {
        int width= overrideWidth != 0 ? overrideWidth
                                      : DecMinimumFieldWidth;
        return writeDecUnsigned( value, buffer, idx, width );
    }

    /** ********************************************************************************************
     * Converts the given long value to a string representation in binary format.<br>
     * Negative numbers have to be converted to positive values when invoking this method.
     * the digits and optional grouping characters. The method does not check any overflow within
     * the given character buffer.
     *
     * The maximum number of digits written is 64. In addition, grouping characters may be written
     * according the settings of fields #WriteGroupChars, #BinNibbleGroupChar, #BinByteGroupChar,
     * #BinWordGroupChar, #BinWord32GroupChar and #LeadingGroupCharReplacement.
     *
     * The minimum width of the output is taken from field #BinFieldWidth unless overwritten
     * by parameter \p{overrideWidth}. If the width is greater than digits found in \p{value},
     * \c '0' digits are prepended. The width is taking group characters into account.
     *
     * \attention
     *  If the value is greater than can be represented by the output width, these greater
     *  digits are cut. This is true for this method as well as #WriteHex and WriteOct. The rational
     *  behind this is that this way, larger numbers do not need to be masked before writing.
     *  (In other words: it is assumed that there is a reason for providing the width).
     *
     * The literal prefix found in field #BinLiteralPrefix is \b not written. The field is
     * only used for detecting formats with method #ParseInt.
     *
     * @param value         The value to write.
     * @param buffer        The character array to write the value to. Needs to be long enough
     *                      (after \p{idx}) to carry the string written.
     * @param idx           The index within \p{buffer} to start writing.
     * @param overrideWidth May be used to override value of field #BinFieldWidth.
     *                      Defaults to \c 0, which denotes to  use the fields' value.
     * @return  The index pointing to behind the last character written in \b buffer.
     **********************************************************************************************/
    ALIB_API
    integer  WriteBin( uint64_t value, TChar* buffer, integer idx, int overrideWidth  )       const;


    /** ********************************************************************************************
     * Converts the given long value to a string representation in hexadecimal format.<br>
     * Negative numbers have to be converted to positive values when invoking this method.
     * the digits and optional grouping characters. The method does not check any overflow within
     * the given character buffer.
     *
     * The maximum number of digits written is 16. In addition, grouping characters may be written
     * according the settings of fields #WriteGroupChars, #HexByteGroupChar, #HexWordGroupChar,
     * #HexWord32GroupChar and #LeadingGroupCharReplacement.
     *
     * The minimum width of the output is taken from field #HexFieldWidth unless overwritten
     * by parameter \p{overrideWidth}. If the width is greater than digits found in \p{value},
     * \c '0' digits are prepended. The width is taking group characters into account.
     *
     * \attention
     *  If the value is greater than can be represented by the output width, these greater
     *  digits are cut. This is true for this method as well as #WriteBin and writeOct. The rational
     *  behind this is that this way, larger numbers do not need to be masked before writing.
     *  (In other words: it is assumed that there is a reason for providing the width).
     *
     * The literal prefix found in field #HexLiteralPrefix is \b not written. The field is
     * only used for detecting formats with method #ParseInt.
     *
     * @param value         The value to write.
     * @param buffer        The character array to write the value to. Needs to be long enough
     *                      (after \p{idx}) to carry the string written.
     * @param idx           The index within \p{buffer} to start writing.
     * @param overrideWidth May be used to override value of field #HexFieldWidth.
     *                      Defaults to \c 0, which denotes to  use the fields' value.
     * @return  The index pointing to behind the last character written in \b buffer.
     **********************************************************************************************/
    ALIB_API
    integer  WriteHex( uint64_t value, TChar* buffer, integer idx, int overrideWidth  )       const;

    /** ********************************************************************************************
     * Converts the given long value to a string representation in octal format.<br>
     * Negative numbers have to be converted to positive values when invoking this method.
     * the digits and optional grouping characters. The method does not check any overflow within
     * the given character buffer.
     *
     * The maximum number of digits written is 64. In addition, grouping characters may be written
     * according the settings of fields #WriteGroupChars, #OctGroupChar and
     * #LeadingGroupCharReplacement.
     *
     * The minimum width of the output is taken from field #OctFieldWidth unless overwritten
     * by parameter \p{overrideWidth}. If the width is greater than digits found in \p{value},
     * \c '0' digits are prepended. The width is taking group characters into account.
     *
     * \attention
     *  If the value is greater than can be represented by the output width, these greater
     *  digits are cut. This is true for this method as well as #WriteBin and writeHex. The rational
     *  behind this is that this way, larger numbers do not need to be masked before writing.
     *  (In other words: it is assumed that there is a reason for providing the width).
     *
     * The literal prefix found in field #OctLiteralPrefix is \b not written. The field is
     * only used for detecting formats with method #ParseInt.
     *
     * @param value         The value to write.
     * @param buffer        The character array to write the value to. Needs to be long enough
     *                      (after \p{idx}) to carry the string written.
     * @param idx           The index within \p{buffer} to start writing.
     * @param overrideWidth May be used to override value of field #OctFieldWidth.
     *                      Defaults to \c 0, which denotes to  use the fields' value.
     * @return  The index pointing to behind the last character written in \b buffer.
     **********************************************************************************************/
    ALIB_API
    integer  WriteOct( uint64_t value, TChar* buffer, integer idx, int overrideWidth  )       const;

    /** ********************************************************************************************
     * Writes the given double value as string representation.
     *
     * Negative numbers have to be converted to positive values when invoking this method.
     * the digits and optional grouping characters. The method does not check any overflow within
     * the given character buffer.
     *
     * Grouping characters are written according the settings of fields #WriteGroupChars,
     * #ThousandsGroupChar and #LeadingGroupCharReplacement.
     *
     * The minimum width of the the integral part of the output is taken from field
     * #IntegralPartMinimumWidth unless overwritten by parameter \p{overrideWidth}.
     * If the width is greater than integral digits found in \p{value},
     * \c '0' digits are prepended. The width is taking group characters into account.
     *
     * If field #FractionalPartWidth as well as the width of the integral part ( provided or set)
     * equals \c -1 the method may choose scientific notation. This is done, when for
     * numbers smaller than <c>10E-4</c> or larger than <c>10E+6</c>.<br>
     *
     * If the given value is not a number, #NANLiteral is written. If infinite, #INFLiteral.
     *
     * The output format is dependent on various further settings provided in the fields of
     * this class. Those are #DecimalPointChar, #ExponentSeparator,
     * #ForceDecimalPoint, #WriteExponentPlusSign and #ForceScientific
     *
     * @param value         The value to write.
     * @param buffer        The character array to write the value to. Needs to be long enough
     *                      (after \p{idx}) to carry the string written.
     * @param idx           The index within \p{buffer} to start writing.
     * @param overrideWidth May be used to override value of field #IntegralPartMinimumWidth.
     *                      Defaults to \c 0, which denotes to  use the fields' value.
     * @return  The index pointing to behind the last character written in \b buffer.
     **********************************************************************************************/
    ALIB_API
    integer  WriteFloat( double value, TChar*  buffer, integer idx, int overrideWidth  )      const;

    // #############################################################################################
    //  Protected methods
    // #############################################################################################
    /** ********************************************************************************************
     * Implementation used by #WriteDecSigned and WriteDecUnsigned.
     *
     * @param value     The value to write.
     * @param buffer    The character array to write the value to. Needs to be long enough
     *                  (after \p{idx}) to carry the string written.
     * @param idx       The index within \p{buffer} to start writing.
     * @param width     The overall minimum width to write.
     * @return  The index pointing to behind the last character written in \b buffer.
     **********************************************************************************************/
    ALIB_API
    integer  writeDecUnsigned( uint64_t value, TChar* buffer, integer idx, int width  )       const;
};

//! @cond NO_DOX
// static variables (need extra definition, what the heck! :)
template<typename TChar> NumberFormatBase<TChar>   NumberFormatBase<TChar>::Global;
template<typename TChar> NumberFormatBase<TChar>   NumberFormatBase<TChar>::Computational;

template<>               void     NumberFormatBase<char   >::SetComputational();

extern template ALIB_API void     NumberFormatBase<char   >::Set             ( NumberFormatBase*);
extern template ALIB_API void     NumberFormatBase<char   >::SetFromLocale   ( );
extern template ALIB_API uint64_t NumberFormatBase<char   >::ParseDecDigits  ( const StringBase<char>&, integer& );
extern template ALIB_API  int64_t NumberFormatBase<char   >::ParseInt        ( const StringBase<char>&, integer& ) const;
extern template ALIB_API uint64_t NumberFormatBase<char   >::ParseDec        ( const StringBase<char>&, integer& ) const;
extern template ALIB_API uint64_t NumberFormatBase<char   >::ParseBin        ( const StringBase<char>&, integer& ) const;
extern template ALIB_API uint64_t NumberFormatBase<char   >::ParseHex        ( const StringBase<char>&, integer& ) const;
extern template ALIB_API uint64_t NumberFormatBase<char   >::ParseOct        ( const StringBase<char>&, integer& ) const;
extern template ALIB_API double   NumberFormatBase<char   >::ParseFloat      ( const StringBase<char>&, integer& ) const;
extern template ALIB_API integer  NumberFormatBase<char   >::WriteDecSigned  ( int64_t , char*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<char   >::WriteBin        ( uint64_t, char*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<char   >::WriteHex        ( uint64_t, char*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<char   >::WriteOct        ( uint64_t, char*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<char   >::WriteFloat      ( double  , char*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<char   >::writeDecUnsigned( uint64_t, char*, integer, int     ) const;


template<>               void     NumberFormatBase<wchar_t>::SetComputational();

extern template ALIB_API void     NumberFormatBase<wchar_t>::Set             ( NumberFormatBase*);
extern template ALIB_API void     NumberFormatBase<wchar_t>::SetFromLocale   ( );
extern template ALIB_API uint64_t NumberFormatBase<wchar_t>::ParseDecDigits  ( const StringBase<wchar_t>&, integer& );
extern template ALIB_API  int64_t NumberFormatBase<wchar_t>::ParseInt        ( const StringBase<wchar_t>&, integer& ) const;
extern template ALIB_API uint64_t NumberFormatBase<wchar_t>::ParseDec        ( const StringBase<wchar_t>&, integer& ) const;
extern template ALIB_API uint64_t NumberFormatBase<wchar_t>::ParseBin        ( const StringBase<wchar_t>&, integer& ) const;
extern template ALIB_API uint64_t NumberFormatBase<wchar_t>::ParseHex        ( const StringBase<wchar_t>&, integer& ) const;
extern template ALIB_API uint64_t NumberFormatBase<wchar_t>::ParseOct        ( const StringBase<wchar_t>&, integer& ) const;
extern template ALIB_API double   NumberFormatBase<wchar_t>::ParseFloat      ( const StringBase<wchar_t>&, integer& ) const;
extern template ALIB_API integer  NumberFormatBase<wchar_t>::WriteDecSigned  ( int64_t , wchar_t*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<wchar_t>::WriteBin        ( uint64_t, wchar_t*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<wchar_t>::WriteHex        ( uint64_t, wchar_t*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<wchar_t>::WriteOct        ( uint64_t, wchar_t*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<wchar_t>::WriteFloat      ( double  , wchar_t*, integer, int     ) const;
extern template ALIB_API integer  NumberFormatBase<wchar_t>::writeDecUnsigned( uint64_t, wchar_t*, integer, int     ) const;

//! @endcond


}} // namespace aworx::[lib::strings]

}  // namespace aworx

#endif // HPP_ALIB_STRINGS_NUMBERFORMAT
