//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace strings {


/// Flags used with class \alib{strings;TNumberFormat}.
/// By default (with construction of an instance of \b TNumberFormat), all flags are unset,
/// except #ForceDecimalPoint.
enum class NumberFormatFlags : uint8_t
{
    /// If assigned, all flags are unset.
    NONE                                    = 0,

    /// Denotes if grouping characters are ignored when parsing numbers if they are given
    /// (not set to \c '\0').
    /// This applies to all number types.<br>
    /// Defaults to \c false. If set to \c true, grouping characters are just skipped when
    /// found while parsing numbers, no matter at which position they occur.
    ReadGroupChars                          = 1,

    /// Denotes if grouping characters are written if they are given (not set to \c '\0').
    /// This applies to all number types.
    WriteGroupChars                         = 2,

    /// If \c true, the decimal point of floating point values is written, even if the fractional
    /// part of the float value is zero. If \c false, in this case the decimal point is omitted.<br>
    /// Defaults to \c true.
    ForceDecimalPoint                       = 4,

    /// Determines if positive exponent values are prepended with an explicit '+' character when
    /// written using \alib{strings::detail;WriteFloat}.<br>
    /// Defaults to \c false, as some systems will not accept a plus sign on the exponent value.
    /// Note that field \alib{strings;TNumberFormat::PlusSign} is not applicable for exponent numbers.
    WriteExponentPlusSign                   = 8,

    /// If this flag ist set, then trailing \c '0' digits in the fractional part of a floating
    /// point value are not written, even if a \alib{strings;TNumberFormat::FractionalPartWidth}
    /// is set.
    OmitTrailingFractionalZeros             =16,


    /// If this flag ist set, then leading \c '0' digits and group characters are replaced with
    /// spaces. This applies to integral values and to the integral part of floating point values,
    /// in the case that \alib{strings;TNumberFormat::DecMinimumFieldWidth}, respectively
    /// \alib{strings;TNumberFormat::IntegralPartMinimumWidth}, specifies the output to be wider
    /// than the number printed.
    ReplaceLeadingZerosWithSpaces           =32,

    /// If \c true, scientific format is always used.<br>
    /// If \c false (the default), function \alib{strings::detail;WriteFloat} writes scientific
    /// format only if both fields, \alib{strings;TNumberFormat::IntegralPartMinimumWidth} and
    /// \alib{strings;TNumberFormat::FractionalPartWidth} are evaluating to \c -1 and only for
    /// numbers smaller than \c 10E-04 or larger than \c 10E+06.<br>
    ///
    /// If one of the fields \alib{strings;TNumberFormat::IntegralPartMinimumWidth} or
    /// \alib{strings;TNumberFormat::FractionalPartWidth} is set to a positive
    /// value, these limits get extended. Function \alib{strings::detail;WriteFloat} in this case
    /// keeps non-scientific notation established if possible.
    ForceScientific                         =64,

    /// If \c true, lower case letters \c 'a' - \c 'f' are written.
    /// Defaults to \c false, which writes upper case letters \c 'A' - \c 'F'.
    HexLowerCase                            =128,
}; // enum class NumberFormatFlags


//==================================================================================================
/// This struct defines flags and values that denote the format of conversion of integer and floating
/// point values to string representations, as well as the reverse operation, thus the
/// format expected when parsing numbers from strings.
///
/// In namespace #alib::strings::detail, corresponding functions that use an instance of this
/// type are implemented. However, those functions are not intended for common use.
/// Instead, the interface of classes
///    \alib{strings;TString;String},
///    \alib{strings;TSubstring;Substring},
///    \alib{strings;TAString;AString} or
///    \alib{format;Formatter}
/// are preferred to write and parse numbers. Also those accept an object of this type as parameters.
///
/// <b>Defined Singletons and User-Defined Instances:</b><br>
/// Two static singletons of this class, both initialized with function \alib{Bootstrap},
/// are defined which can be used wherever a number format object is needed as a parameter:
/// - #Global: Reflects locale-specific settings.
///
/// - #Computational:<br>
///   Intended to be used for writing and parsing numbers which are readable by software (not
///   humans). Its decimal point character is set to \c '.', the international standard.
///   Furthermore no group separators are set for decimal and decimal floating point as well as
///   for binary, hexadecimal and octal conversions.
///
/// Any user-defined object defaults to the computational setting after construction.
///
/// <b>Output Formats:</b><br>
/// The following conversion formats are supported:
///
/// - \b Decimal<br>
///   Supports optional minimum output width with field #DecMinimumFieldWidth,
///   and definable <em>thousands grouping character</em> with field #ThousandsGroupChar, which can
///   be activated with field #WriteGroupChars.<br>
///   Furthermore, the plus-sign can be controlled (#PlusSign) to be either omitted or be anything
///   defined. Of course, values <c>' '</c> and <c>'+'</c> are reasonable options.
///
/// - <b>Binary</b><br>
///   Binary output supports up to 64 digits and different group separators for nibbles, bytes,
///   16-bit words and 32 bit words. (See #BinNibbleGroupChar, #BinByteGroupChar,
///   #BinWordGroupChar and #BinWord32GroupChar ).<br>
///   When parsing integers, a customizable literal string defined in #BinLiteralPrefix might be used
///   to auto-detect binary values.
///
/// - <b>Hexadecimal</b><br>
///   Hexadecimal output supports up to 16 digits (64-bit) and different group separators
///   for bytes, 16-bit words and 32 bit words. (See #HexByteGroupChar, #HexWordGroupChar,
///   and #HexWord32GroupChar).
///   When parsing integers, a customizable literal string defined in #HexLiteralPrefix might be used
///   to auto-detect hexadecimal values.
///
/// - <b>Octal</b><br>
///   Octal output supports up to 22 digits (64-bit) and different a group separator for groups
///   of three digits defined with #OctGroupChar.
///   When parsing integers, a customizable literal string defined in #OctLiteralPrefix might be used
///   to auto-detect hexadecimal values.
///
/// - <b>Floating Point</b><br>
///   The width of the output is provided in two fields, #IntegralPartMinimumWidth and
///   #FractionalPartWidth. While the integral part is a minimum width (and nothing is ever cut),
///   the fractional part denotes a fixed width.
///   Values with higher fractional precision are rounded accordingly.
///   Note that the parameter of the interface functions that may override the width, in the floating
///   point case only affects the minimum width of the integral part.<br>
///   The integral and fractional part of float values are separated by decimalPointChar.
///   This field of course has to be different from group separator #ThousandsGroupChar, which can
///   be activated using field #WriteGroupChars.<br>
///   Other important fields used for writing and parsing floats are: #ExponentSeparator,
///   #INFLiteral, #NANLiteral, #WriteExponentPlusSign, and #ForceScientific.
///
/// <b>Notes on Writing and Parsing Values:</b><br>
/// For decimal output, the width (#DecMinimumFieldWidth) is a minimum width. This means, that
/// bigger numbers are written in a higher width.
///
/// \attention
///   This is \c not true for binary, hexadecimal and octal output. In these formats, the width
///   provided with fields #BinFieldWidth, #HexFieldWidth and #OctFieldWidth, denote an \b absolute
///   value. Higher digits of numbers are not written! The advantage of this design is that no
///   masking is needed when just the lower part of an integer number should be written.
///   However, if a width is set, values might of course change when cut and parsed back later!
///
/// All of the integral formats have in common that the output width given includes optional
/// grouping characters. For example if a width of \b 5 was given for decimal output, the value
/// \c 12 would be written \c "0,012", hence \b 4 digits plus the grouping character. If grouping
/// was disabled, the output became \c "00012", which uses one extra digit instead of the group
/// character.
/// In contrast to that, sign characters are \c not counted in the width.
///
/// When parsing values, grouping characters are ignored at any position within the digits,
/// except of the start. The same is true for whitespace characters as defined in
/// #Whitespaces. When this field is \e nulled or empty, then white spaces are \b not ignored.
/// This might be helpful in some cases where occurrence of white space characters should
/// indicate an error (or something else) when parsing.
/// Otherwise, the characters defined in this field are ignored at two places: at the beginning
/// of a parsing operation and after a sign character was read.
///
/// When parsing fails, a value of \c 0 (respectively \c 0.0) is returned by the functions of
/// namespace #alib::strings::detail which are using this class.
/// User-friendly classes that use the interface of this type will detect such failure through the
/// output parameter of the parsing functions, which indicates the index of the end of
/// the number found.
///
/// For each of the four integer formats, decimal, binary, hexadecimal and octal, dedicated
/// parsing functions exist. Those do not accept literal prefix identifiers as defined in
/// fields #BinLiteralPrefix, #HexLiteralPrefix and #OctLiteralPrefix. However, the prefixes \b are
/// identified by function \alib{strings::detail;ParseInt}, which aggregates the other four parsing
/// functions.<br>
/// There is no corresponding function defined that writes the literal prefix. When writing
/// binary, hexadecimal or octal values, such prefixes have to be prepended explicitly by a
/// user's code.
///
/// @tparam TChar The \ref alib_characters_chars "character type" that this class works on.
///                Alias names for specializations using the different character types are
///                provided in namespace #alib with type definitions
///                \alib{NumberFormat},
///                \alib{NNumberFormat},
///                \alib{WNumberFormat},
///                \alib{XNumberFormat},
///                \alib{ComplementNumberFormat}, and
///                \alib{StrangeNumberFormat}.
//==================================================================================================
template<typename TChar>
struct TNumberFormat
{
    /// The default static number format object that acts as the default settings of the currently
    /// running process.<br>
    /// Function \alib{Bootstrap} invokes #SetFromLocale() on this object and switches grouping
    /// to \e 'on'.
    ///
    /// Classes providing functionality based on this class, might use this as a default
    /// value for parameters of their interfaces.
    static              TNumberFormat   Global;

    /// A static number format object that may be used to write and parse numbers for 'computational'
    /// use, which means, that grouping is switched off and decimal point character
    /// is \c '.'.<br>
    /// Function \alib{Bootstrap} invokes #SetComputational on this object.
    ///
    /// Classes providing functionality based on this class, might use this as a default
    /// value for parameters of their interfaces.
    static              TNumberFormat   Computational;

    // ###############################   string members  ###################################
    /// Defines whitespace characters that are ignored when leading the number and after
    /// the sign-character. Applies to functions
    /// \alib{strings::detail;ParseInt} and
    /// \alib{strings::detail;ParseFloat}. In contrast, functions
    /// \alib{strings::detail;ParseDec},
    /// \alib{strings::detail;ParseBin},
    /// \alib{strings::detail;ParseHex}, and
    /// \alib{strings::detail;ParseOct} do not ignore any whitespace characters.
    TCString<TChar>     Whitespaces;

    /// Defines the decimal exponent symbol of string representations of floating point numbers
    /// when written or parsed in scientific notation by functions \alib{strings::detail;ParseFloat}
    /// and \alib{strings::detail;WriteFloat}.<br>
    /// Function \alib{strings::detail;ParseFloat} accepts characters 'e' and 'E' in addition to
    /// the character set in this field.<br>
    /// Defaults to \c 'E'.
    TCString<TChar>     ExponentSeparator;

    /// Defines what is written and parsed for infinite double values.
    TCString<TChar>     INFLiteral;

    /// Defines what is written and parsed for double values that represent "not a number".
    TCString<TChar>     NANLiteral;

    /// Used by function \alib{strings::detail;ParseInt} to detect binary format of integral values.
    /// If \e nulled, no binary format is detected.
    /// Functions provided with \alib are not writing the prefix. If this is desired, it has to
    /// be performed explicitly by the user code.<br>
    /// Defaults to \c "0b".
    TCString<TChar>     BinLiteralPrefix;

    /// Used by function \alib{strings::detail;ParseInt} to detect hexadecimal format of integer
    /// values. If \e nulled, no hexadecimal format is detected.
    /// Functions provided with \alib are not writing the prefix. If this is desired, it has to
    /// be performed explicitly by the user code.<br>
    /// Defaults to \c "0x".
    TCString<TChar>     HexLiteralPrefix;

    /// Used by function \alib{strings::detail;ParseInt} to detect octal format of integral values.
    /// If \e nulled, no octal format is detected.
    /// Functions provided with \alib are not writing the prefix. If this is desired, it has to
    /// be performed explicitly by the user code.<br>
    /// Defaults to \c "0o".
    TCString<TChar>     OctLiteralPrefix;

    // ###############################  character members  ###################################

    /// Defines the decimal point character when converting a floating point number to a string
    /// representation with function \alib{strings::detail;WriteFloat}. Also; function \alib{strings::detail;ParseFloat} uses
    /// the character provided in this field for parsing the character.<br>
    /// The field defaults to '.'. By invoking #SetFromLocale(), the current locale's setting is
    /// determined.
    TChar               DecimalPointChar;

    /// Determines if positive values are prepended with an explicit character (usually '+') when
    /// written using \alib{strings::detail;WriteFloat} or \alib{strings::detail;WriteDecSigned}.<br>
    /// Defaults to \c 0 which omits the writing. Usual other values are of course \c '+', but
    /// also  <c>' '</c> (space) which supports better horizontal alignment of numbers when written in
    /// columns. Note that this is not affecting exponent decimals of floating point values.
    /// For those, see #WriteExponentPlusSign
    TChar               PlusSign;

    /// Defines the separator character for thousands when converting a number to a string
    /// representation. In addition, this is used to identify grouping symbols when
    /// parsing decimal values. If set to \c '\0', no group separator is written and also when
    /// parsing, a group separator is not accepted.
    /// If set, still #WriteGroupChars controls if it is written.<br>
    /// Defaults to \c ','.
    /// By invoking #SetFromLocale(), the current locale's setting is determined.
    TChar               ThousandsGroupChar;

    /// This character is written instead of a grouping character in the case that a certain
    /// output width is requested but a grouping character would be the first character to write.
    /// Writing this character instead, ensures the field width to be as requested.
    /// Defaults to space (<c>' '</c>).
    TChar               LeadingGroupCharReplacement;


    /// Defines the separator character for nibbles (4 bits) of binary numbers.
    /// Defaults to \c '\0' what disables reading and writing of nibble group characters.
    TChar               BinNibbleGroupChar;

    /// Defines the separator character for bytes of binary numbers.
    /// Defaults to \c '\0' what chooses #BinNibbleGroupChar.
    TChar               BinByteGroupChar;

    /// Defines the separator character for 16-bit words  of binary numbers.
    /// Defaults to \c '\0' what chooses #BinByteGroupChar.
    TChar               BinWordGroupChar;

    /// Defines the separator character for 32-bit words  of binary numbers.
    /// Defaults to \c '\0' what chooses #BinWordGroupChar.
    TChar               BinWord32GroupChar;

    /// Defines the separator character for bytes of hexadecimal numbers. Defaults to \c 0,
    /// Defaults to \c '\0' what disables reading and writing of byte group characters.
    TChar               HexByteGroupChar;

    /// Defines the separator character for 16-bit words  of hexadecimal numbers.
    /// Defaults to \c '\0' what chooses #HexByteGroupChar.
    TChar               HexWordGroupChar;

    /// Defines the separator character for 32-bit words  of hexadecimal numbers.
    /// Defaults to \c '\0' what chooses #HexWordGroupChar.
    TChar               HexWord32GroupChar;

    /// Defines the separator character for bytes of hexadecimal numbers.
    /// Defaults to \c '\0' what disables reading and writing of byte group characters.
    TChar               OctGroupChar;

    /// The flag field.
    NumberFormatFlags   Flags;

    // ############################ width members ###############################
    /// Defines the minimum digits written for the integral part when converting a floating point
    /// value into a string.<br>
    /// If the integral part of the number provided has less digits
    /// then leading '0' digits are added.<br>
    /// The maximum value allowed is 15.<br>
    /// A value of 0 leads to omitting the '0' before the
    /// decimal separator in the case the value is below 1.0 and higher than -1.0 <br>
    /// The default value is -1, which writes a minimum of 1 digit.
    ///
    /// When either this field or field #FractionalPartWidth is set to a positive value,
    /// the limits to switch to scientific notation, which otherwise are fixed \c 10E-04 and
    /// \c 10E+06, get extended. Function \alib{strings::detail;WriteFloat} in this case keeps
    /// non-scientific notation established if possible.
    int8_t              IntegralPartMinimumWidth;

    /// Defines the number of digits written for the fractional part when converting a floating point
    /// value into a string. (For integer conversion, see #DecMinimumFieldWidth.)<br>
    /// If the fractional part of the number provided has less digits then trailing '0'
    /// digits are added.<br>
    /// If the fractional part of the number provided has more digits then the fractional part is
    /// rounded accordingly.<br>
    /// The maximum value allowed is 15.<br>
    /// The default value is -1, which writes as many digits as available in the provided float
    /// variable, with a minimum of 1 digit.
    ///
    /// When either this field or field #IntegralPartMinimumWidth is set to a positive value,
    /// the limits to switch to scientific notation, which otherwise are fixed \c 10E-04 and
    /// \c 10E+06, get extended. Function \alib{strings::detail;WriteFloat} in this case keeps non-scientific notation
    /// established if possible.
    int8_t              FractionalPartWidth;

    /// Defines the minimum digits and grouping symbols written when writing integers in decimal.
    /// format. If the value to write has less digits (and grouping symbols), then leading '0'
    /// digits (and eventually grouping symbols) are added.<br> If the value to write has more
    /// digits, then this field is ignored.
    /// A sign character is not calculated into the writing width. To have negative and positive
    /// numbers resulting in the same width, #PlusSign has to be set to a value unequal to \c '\0'
    /// (usually space character <c>' '</c> or \c '+').
    ///
    /// If this field is negative, it is ignored. Defaults to \c -1.
    int8_t              DecMinimumFieldWidth;

    /// Defines the digits written when writing binary values.
    /// If the value has less digits, then leading '0' digits are added. If it has more, than
    /// those digits are \b NOT written (!).<br>
    /// The default value and minimum value is \c -1, which writes as many bits as necessary.
    int8_t              BinFieldWidth;

    /// Defines the digits written when writing hexadecimal values.
    /// If the value has less digits, then leading '0' digits are added. If it has more, than
    /// those digits are \b NOT written (!).<br>
    /// The default value and minimum value is \c -1, which writes as many bits as necessary.
    int8_t              HexFieldWidth;


    /// Defines the digits written when writing hexadecimal values.
    /// If the value has less digits, then leading '0' digits are added. If it has more, than
    /// those digits are \b NOT written (!).<br>
    /// The default value and minimum value is \c -1, which writes as many bits as necessary.
    int8_t              OctFieldWidth;


    // #############################################################################################
    //  Interface
    // #############################################################################################

    //==============================================================================================
    /// Default constructor. Invokes #SetComputational to reset all fields to their default values.
    //==============================================================================================
    TNumberFormat()
    {
        SetComputational();
    }

    //==============================================================================================
    /// Copies all fields (settings) from the given object. If no object is provided, values of
    /// the static singleton found in field #Global are copied
    ///
    /// @param other  The \b %NumberFormat object to copy the values from.
    ///               Defaults to \c nullptr, which chooses the global singleton.
    //==============================================================================================
    void      Set( TNumberFormat* other =nullptr );

    //==============================================================================================
    /// Resets the object to its default values. This method is called in the constructor.
    ///
    /// Decimal point character and grouping characters are set as follows:
    /// <center>Field</center>         | <center>Value</center>
    /// - - - - - - - - - - - - - - - -| - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - -
    /// #DecimalPointChar              |  \c .
    /// #ThousandsGroupChar            |  \c ,
    /// #BinNibbleGroupChar            |  \c '
    /// #BinByteGroupChar              |  \c -
    /// #BinWordGroupChar              |  <c>' '</c> (space)
    /// #BinWord32GroupChar            |  \c #
    /// #HexWordGroupChar              |  \c '
    /// #HexWord32GroupChar            |  \c '
    /// #HexByteGroupChar              |  \c 0 (none)
    /// #OctGroupChar                  |  \c '
    ///
    /// The literal attributes are set as follows:
    /// <center>Field</center>         | <center>Value</center>
    /// - - - - - - - - - - - - - - - -| - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - -
    /// #ExponentSeparator             |  \c "E"
    /// #INFLiteral                    |  \c "INF"
    /// #NANLiteral                    |  \c "NAN"
    /// #BinLiteralPrefix              |  \c "0b"
    /// #HexLiteralPrefix              |  \c "0x"
    /// #OctLiteralPrefix              |  \c "0o"
    ///
    /// All width-attributes are reset to "automatic mode", \c -1. The attributes are
    /// #IntegralPartMinimumWidth,
    /// #FractionalPartWidth,
    /// #DecMinimumFieldWidth,
    /// #BinFieldWidth,
    /// #HexFieldWidth and
    /// #OctFieldWidth.
    ///
    /// Finally, the following further fields are reset to their default values:
    /// <center>Field</center>         | <center>Value</center>
    /// - - - - - - - - - - - - - - - -| - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - -
    /// #WriteGroupChars               |  \c false
    /// #ForceScientific               |  \c false
    /// #ForceDecimalPoint             |  \c true
    /// #PlusSign                      |  \c none (0)
    /// #WriteExponentPlusSign         |  \c false
    /// #OmitTrailingFractionalZeros   |  \c false
    /// #HexLowerCase                  |  \c false
    /// #Whitespaces                   |  #alib::DEFAULT_WHITESPACES
    ///
    ///
    /// \note
    ///   With static object
    ///   \ref alib::strings::TNumberFormat::Computational "TNumberFormat::Computational",
    ///   there is a global singleton existing which can be used but must not be changed.
    //==============================================================================================
    void     SetComputational();

    //==============================================================================================
    /// Sets the field #DecimalPointChar and #ThousandsGroupChar to reflect the current
    /// system locale setting. No other values are changed.
    ///
    /// \note
    ///   Static (global) object \ref alib::strings::TNumberFormat::Global "TNumberFormat::Global",
    ///   implements an instance which has the right locale set (provided that function
    ///   \alib{Bootstrap} was duly invoked by the process).
    ///   Otherwise, this method might be used to initialize a custom object with default values
    ///   to afterwards make some specific changes.
    //==============================================================================================
    void     SetFromLocale();
};

//! @cond NO_DOX

extern template   ALIB_DLL void   TNumberFormat<nchar>::Set             ( TNumberFormat* );
extern template   ALIB_DLL void   TNumberFormat<nchar>::SetFromLocale   ();


extern template   ALIB_DLL void   TNumberFormat<wchar>::Set             ( TNumberFormat* );
extern template   ALIB_DLL void   TNumberFormat<wchar>::SetFromLocale   ();


extern template   ALIB_DLL void   TNumberFormat<xchar>::Set             ( TNumberFormat* );
extern template   ALIB_DLL void   TNumberFormat<xchar>::SetFromLocale   ();


       template<> ALIB_DLL void   TNumberFormat<nchar>::SetComputational();
       template<> ALIB_DLL void   TNumberFormat<wchar>::SetComputational();
       template<> ALIB_DLL void   TNumberFormat<xchar>::SetComputational();

template<typename TChar> TNumberFormat<TChar>   TNumberFormat<TChar>::Global;
template<typename TChar> TNumberFormat<TChar>   TNumberFormat<TChar>::Computational;
//! @endcond

} // namespace alib::[strings]

/// Type alias in namespace \b alib.
using NumberFormatFlags= strings::NumberFormatFlags;

/// Type alias in namespace \b alib.
using  NumberFormat     =     strings::TNumberFormat<character>;

/// Type alias in namespace \b alib.
using  ComplementNumberFormat=strings::TNumberFormat<complementChar>;

/// Type alias in namespace \b alib.
using  StrangeNumberFormat=   strings::TNumberFormat<strangeChar>;

/// Type alias in namespace \b alib.
using  NNumberFormat    =     strings::TNumberFormat<nchar>;

/// Type alias in namespace \b alib.
using  WNumberFormat    =     strings::TNumberFormat<wchar>;

/// Type alias in namespace \b alib.
using  XNumberFormat    =     strings::TNumberFormat<xchar>;


} // namespace [alib]


ALIB_ENUMS_MAKE_BITWISE( alib::strings::NumberFormatFlags )
