//==================================================================================================
/// \file
/// This header-file is part of module \alib_format of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib::format {

//==================================================================================================
/// Enumeration of exceptions thrown with classes found in sub-namespace #"alib::format;2"
/// of module \alib_format.
//==================================================================================================
enum class FMTExceptions {
    /// Argument index '0' not allowed.
    ArgumentIndexIs0                    =11,

    /// Argument index greater than number of arguments available.
    ArgumentIndexOutOfBounds            =12,

    /// Incompatible type code given argument type found.
    IncompatibleTypeCode                =13,

    // Formatter Python Style
    /// Thrown by #"FormatterPythonStyle":
    /// Closing bracket <c>'}'</c> not found.
    MissingClosingBracket               =101,

    /// Thrown by #"FormatterPythonStyle":
    /// Missing precision integral value after <c>'.'</c> character.
    MissingPrecisionValuePS             =102,

    /// Thrown by #"FormatterPythonStyle":
    /// Duplicate type code.
    DuplicateTypeCode                   =103,

    /// Thrown by #"FormatterPythonStyle":
    /// Unknown type code.
    UnknownTypeCode                     =104,

    /// Thrown by #"FormatterPythonStyle":
    /// Expected <c>'!'</c> not found.
    ExclamationMarkExpected             =105,

    /// Thrown by #"FormatterPythonStyle":
    /// Unknown conversion after <c>'!'</c>.
    UnknownConversionPS                 =106,

    /// Thrown by #"FormatterPythonStyle":
    /// Precision specification not allowed with integer types.
    PrecisionSpecificationWithInteger   =107,

    /// Thrown by #"FormatterPythonStyle":
    /// Missing replacement strings after conversion <c>!Replace</c>.
    MissingReplacementStrings           =108,



    // Formatter Java Style
    /// Thrown by #"FormatterJavaStyle":
    /// Formatting of negative values in brackets is not supported.
    NegativeValuesInBracketsNotSupported=201,

    /// Thrown by #"FormatterJavaStyle":
    /// Missing precision integral value after <c>'.'</c> character.
    MissingPrecisionValueJS             =202,

    /// Thrown by #"FormatterJavaStyle":
    /// Output of floating point values in hexadecimal format not supported.
    HexadecimalFloatFormatNotSupported  =203,

    /// Thrown by #"FormatterJavaStyle":
    /// The alternate form '#"'" is not supported with given conversion.
    NoAlternateFormOfConversion         =204,

    /// Thrown by #"FormatterJavaStyle":
    /// Precision specification is not supported with given conversion.
    NoPrecisionWithConversion           =205,

    /// Thrown by #"FormatterJavaStyle":
    /// Unknown conversion suffix with data/time conversion.
    UnknownDateTimeConversionSuffix     =206,

    /// Thrown by #"FormatterJavaStyle":
    /// Unknown conversion character.
    UnknownConversionJS                 =207,


    // PropertyFormatter and PropertyFormatters
    /// Thrown by constructor of #"PropertyFormatter"
    /// when a property identifier parsed from the format string has no corresponding entry in the
    /// #"PropertyFormatter::TCallbackTable" provided.
    UnknownPropertyInFormatString       =501,

    /// Exception entry added in method #"PropertyFormatter::Format;*"
    /// when the underlying formatter throws.
    ErrorInResultingFormatString        =502,

    /// Thrown by #"PropertyFormatters::Format;*" if a configuration variable
    /// cannot be loaded or is empty.
    ///
    /// \note To prevent this, software using this class should provide
    ///       #"GetResourcePool;resourced" default values for each formatter variable.
    MissingConfigurationVariable        =510,


    // class Paragraphs
    /// Thrown by #"Paragraphs::AddMarked(const BoxedObjects& ...);*"
    /// when an unknown marker token was found.
    UnknownMarker                       =601,

    /// Thrown by #"Paragraphs::AddMarked(const BoxedObjects& ...);*"
    /// when a property identifier parsed from the format string has no corresponding entry in the
    /// #"PropertyFormatter::TCallbackTable" provided.
    EndmarkerWithoutStart               =602,

};  // FMTExceptions

} // namespace [alib::format]


ALIB_ENUMS_ASSIGN_RECORD(   alib::format::FMTExceptions, alib::exceptions::ERException )
ALIB_BOXING_VTABLE_DECLARE( alib::format::FMTExceptions, vt_system_fmtexceptions )
