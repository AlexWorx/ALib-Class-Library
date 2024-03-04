/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #alib::lang::format of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_FORMAT_EXCEPTIONS
#define HPP_ALIB_LANG_FORMAT_EXCEPTIONS 1

#if !defined(HPP_ALIB_LANG_BASECAMP)
    #include "alib/lang/basecamp/basecamp.hpp"
#endif

namespace alib::lang::format {

/** ****************************************************************************************
 * Enumeration of exceptions thrown with classes found in sub-namespace #alib::lang::format
 * of module \alib_basecamp.
 ******************************************************************************************/
enum class FMTExceptions
{
    /** Argument index '0' not allowed. */
    ArgumentIndexIs0                    =11,

    /** Argument index greater than number of arguments available. */
    ArgumentIndexOutOfBounds            =12,

    /** Incompatible type code given argument type found. */
    IncompatibleTypeCode                =13,


    /** Thrown by \alib{lang::format,FormatterPythonStyle,FormatterPythonStyle}:
     *  Closing bracket <c>'}'</c> not found.                       */
    MissingClosingBracket               =101,

    /** Thrown by \alib{lang::format,FormatterPythonStyle,FormatterPythonStyle}:
     *  Missing precision integral value after <c>'.'</c> character. */
    MissingPrecisionValuePS             =102,

    /** Thrown by \alib{lang::format,FormatterPythonStyle,FormatterPythonStyle}:
     *  Duplicate type code.                                        */
    DuplicateTypeCode                   =103,

    /** Thrown by \alib{lang::format,FormatterPythonStyle,FormatterPythonStyle}:
     *  Unknown type code.                                          */
    UnknownTypeCode                     =104,

    /** Thrown by \alib{lang::format,FormatterPythonStyle,FormatterPythonStyle}:
     *  Expected <c>'!'</c> not found.                              */
    ExclamationMarkExpected             =105,

    /** Thrown by \alib{lang::format,FormatterPythonStyle,FormatterPythonStyle}:
     *  Unknown conversion after <c>'!'</c>.                        */
    UnknownConversionPS                 =106,

    /** Thrown by \alib{lang::format,FormatterPythonStyle,FormatterPythonStyle}:
     *  Precision specification not allowed with integer types.     */
    PrecisionSpecificationWithInteger   =107,

    /** Thrown by \alib{lang::format,FormatterPythonStyle,FormatterPythonStyle}:
     *  Missing replacement strings after conversion <c>!Replace</c>.*/
    MissingReplacementStrings           =108,



    // Formatter Java Style
    /** Thrown by \alib{lang::format,FormatterJavaStyle,FormatterJavaStyle}:
     *  Formatting of negative values in brackets is not supported. */
    NegativeValuesInBracketsNotSupported=201,

    /** Thrown by \alib{lang::format,FormatterJavaStyle,FormatterJavaStyle}:
     *  Missing precision integral value after <c>'.'</c> character. */
    MissingPrecisionValueJS             =202,

    /** Thrown by \alib{lang::format,FormatterJavaStyle,FormatterJavaStyle}:
     *  Output of floating point values in hexadecimal format not supported.  */
    HexadecimalFloatFormatNotSupported  =203,

    /** Thrown by \alib{lang::format,FormatterJavaStyle,FormatterJavaStyle}:
     *  The alternate form '#' is not supported with given conversion.        */
    NoAlternateFormOfConversion         =204,

    /** Thrown by \alib{lang::format,FormatterJavaStyle,FormatterJavaStyle}:
     *  Precision specification is not supported with given conversion.       */
    NoPrecisionWithConversion           =205,

    /** Thrown by \alib{lang::format,FormatterJavaStyle,FormatterJavaStyle}:
     *  Unknown conversion suffix with data/time conversion.             */
    UnknownDateTimeConversionSuffix     =206,

    /** Thrown by \alib{lang::format,FormatterJavaStyle,FormatterJavaStyle}:
     *  Unknown conversion character.                               */
    UnknownConversionJS                 =207,


    // PropertyFormatter and PropertyFormatters
    /**
     * Thrown by constructor of \alib{lang::format,PropertyFormatter}
     * when a property identifier parsed from the format string has no corresponding entry in the
     * \alib{lang::format,PropertyFormatter::TCallbackTable,TCallbackTable} provided.
     */
    UnknownPropertyInFormatString       =501,

    /**
     * Exception entry added in method \alib{lang::format,PropertyFormatter::Format}
     * when the underlying formatter throws.
     */
    ErrorInResultingFormatString        =502,

    /**
     * Thrown by \alib{lang::format,PropertyFormatters::Format} if a configuration variable
     * can not be loaded or is empty.
     *
     * \note To prevent this, software using this class should provide
     *       \alib{lang::Camp,GetResourcePool,resourced} default values for each formatter variable.
     */
    MissingConfigurationVariable        =510,


    // class Paragraphs
    /**
     * Thrown by \alib{lang::format,Paragraphs::AddMarked}
     * when an unknown marker token was found.
     */
    UnknownMarker                       =601,

    /**
     * Thrown by \alib{lang::format,Paragraphs::AddMarked}
     * when a property identifier parsed from the format string has no corresponding entry in the
     * \alib{lang::format,PropertyFormatter::TCallbackTable,TCallbackTable} provided.
     */
    EndmarkerWithoutStart               =602,

};  // FMTExceptions

 } // namespace [alib::lang::format]

ALIB_BOXING_VTABLE_DECLARE( alib::lang::format::FMTExceptions, vt_system_fmtexceptions )

ALIB_ENUMS_ASSIGN_RECORD(   alib::lang::format::FMTExceptions, alib::lang::ERException )
ALIB_RESOURCED_IN_MODULE(   alib::lang::format::FMTExceptions, alib::BASECAMP, "FE" )

#endif // HPP_ALIB_LANG_FORMAT_EXCEPTIONS
