//==================================================================================================
/// \file
/// This header-file is part of module \alib_format of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::format {

//==================================================================================================
/// Enumeration of exceptions thrown with classes found in sub-namespace #alib::format
/// of module \alib_format.
//==================================================================================================
enum class FMTExceptions
{
    /// Argument index '0' not allowed.
    ArgumentIndexIs0                    =11,

    /// Argument index greater than number of arguments available.
    ArgumentIndexOutOfBounds            =12,

    /// Incompatible type code given argument type found.
    IncompatibleTypeCode                =13,

    // Formatter Python Style
    /// Thrown by \alib{format;FormatterPythonStyle;FormatterPythonStyle}:
    /// Closing bracket <c>'}'</c> not found.
    MissingClosingBracket               =101,

    /// Thrown by \alib{format;FormatterPythonStyle;FormatterPythonStyle}:
    /// Missing precision integral value after <c>'.'</c> character.
    MissingPrecisionValuePS             =102,

    /// Thrown by \alib{format;FormatterPythonStyle;FormatterPythonStyle}:
    /// Duplicate type code.
    DuplicateTypeCode                   =103,

    /// Thrown by \alib{format;FormatterPythonStyle;FormatterPythonStyle}:
    /// Unknown type code.
    UnknownTypeCode                     =104,

    /// Thrown by \alib{format;FormatterPythonStyle;FormatterPythonStyle}:
    /// Expected <c>'!'</c> not found.
    ExclamationMarkExpected             =105,

    /// Thrown by \alib{format;FormatterPythonStyle;FormatterPythonStyle}:
    /// Unknown conversion after <c>'!'</c>.
    UnknownConversionPS                 =106,

    /// Thrown by \alib{format;FormatterPythonStyle;FormatterPythonStyle}:
    /// Precision specification not allowed with integer types.
    PrecisionSpecificationWithInteger   =107,

    /// Thrown by \alib{format;FormatterPythonStyle;FormatterPythonStyle}:
    /// Missing replacement strings after conversion <c>!Replace</c>.
    MissingReplacementStrings           =108,



    // Formatter Java Style
    /// Thrown by \alib{format;FormatterJavaStyle;FormatterJavaStyle}:
    /// Formatting of negative values in brackets is not supported.
    NegativeValuesInBracketsNotSupported=201,

    /// Thrown by \alib{format;FormatterJavaStyle;FormatterJavaStyle}:
    /// Missing precision integral value after <c>'.'</c> character.
    MissingPrecisionValueJS             =202,

    /// Thrown by \alib{format;FormatterJavaStyle;FormatterJavaStyle}:
    /// Output of floating point values in hexadecimal format not supported.
    HexadecimalFloatFormatNotSupported  =203,

    /// Thrown by \alib{format;FormatterJavaStyle;FormatterJavaStyle}:
    /// The alternate form '#' is not supported with given conversion.
    NoAlternateFormOfConversion         =204,

    /// Thrown by \alib{format;FormatterJavaStyle;FormatterJavaStyle}:
    /// Precision specification is not supported with given conversion.
    NoPrecisionWithConversion           =205,

    /// Thrown by \alib{format;FormatterJavaStyle;FormatterJavaStyle}:
    /// Unknown conversion suffix with data/time conversion.
    UnknownDateTimeConversionSuffix     =206,

    /// Thrown by \alib{format;FormatterJavaStyle;FormatterJavaStyle}:
    /// Unknown conversion character.
    UnknownConversionJS                 =207,


    // PropertyFormatter and PropertyFormatters
    /// Thrown by constructor of \alib{format;PropertyFormatter}
    /// when a property identifier parsed from the format string has no corresponding entry in the
    /// \alib{format;PropertyFormatter::TCallbackTable;TCallbackTable} provided.
    UnknownPropertyInFormatString       =501,

    /// Exception entry added in method \alib{format;PropertyFormatter::Format}
    /// when the underlying formatter throws.
    ErrorInResultingFormatString        =502,

    /// Thrown by \alib{format;PropertyFormatters::Format} if a configuration variable
    /// cannot be loaded or is empty.
    ///
    /// \note To prevent this, software using this class should provide
    ///       \alib{camp::Camp;GetResourcePool;resourced} default values for each formatter variable.
    MissingConfigurationVariable        =510,


    // class Paragraphs
    /// Thrown by \alib{format;Paragraphs::AddMarked}
    /// when an unknown marker token was found.
    UnknownMarker                       =601,

    /// Thrown by \alib{format;Paragraphs::AddMarked}
    /// when a property identifier parsed from the format string has no corresponding entry in the
    /// \alib{format;PropertyFormatter::TCallbackTable;TCallbackTable} provided.
    EndmarkerWithoutStart               =602,

};  // FMTExceptions

} // namespace [alib::format]


ALIB_ENUMS_ASSIGN_RECORD(   alib::format::FMTExceptions, alib::exceptions::ERException )
ALIB_BOXING_VTABLE_DECLARE( alib::format::FMTExceptions, vt_system_fmtexceptions )

