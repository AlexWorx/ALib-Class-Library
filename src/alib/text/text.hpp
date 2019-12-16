/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_text of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_TEXT_TEXT
#define HPP_ALIB_TEXT_TEXT 1

#if !defined(HPP_ALIB_TEXT_FWDS)
    #include "alib/text/fwds.hpp"
#endif

#if !defined(HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

#if !defined(HPP_ALIB_FS_MODULES_MODULE)
#   include "alib/lib/fs_modules/module.hpp"
#endif

#if ALIB_TIME
#   if !defined(HPP_ALIB_TIME_TICKS)
#       include "alib/time/ticks.hpp"
#   endif
#   if !defined(HPP_ALIB_TIME_DATETIME)
#       include "alib/time/datetime.hpp"
#   endif
#endif

namespace aworx { namespace lib { namespace text {

/** ****************************************************************************************
 * Enumeration of exceptions thrown with classes found in module \alib_text.
 ******************************************************************************************/
enum class Exceptions
{
    /** Argument index '0' not allowed. */
    ArgumentIndexIs0                    =11,

    /** Argument index greater than number of arguments available. */
    ArgumentIndexOutOfBounds            =12,

    /** Incompatible type code given argument type found. */
    IncompatibleTypeCode                =13,


    /** Thrown by \alib{text,FormatterPythonStyle,FormatterPythonStyle}:
     *  Closing bracket <c>'}'</c> not found.                       */
    MissingClosingBracket               =101,

    /** Thrown by \alib{text,FormatterPythonStyle,FormatterPythonStyle}:
     *  Missing precision integral value after <c>'.'</c> character. */
    MissingPrecisionValuePS             =102,

    /** Thrown by \alib{text,FormatterPythonStyle,FormatterPythonStyle}:
     *  Duplicate type code.                                        */
    DuplicateTypeCode                   =103,

    /** Thrown by \alib{text,FormatterPythonStyle,FormatterPythonStyle}:
     *  Unknown type code.                                          */
    UnknownTypeCode                     =104,

    /** Thrown by \alib{text,FormatterPythonStyle,FormatterPythonStyle}:
     *  Expected <c>'!'</c> not found.                              */
    ExclamationMarkExpected             =105,

    /** Thrown by \alib{text,FormatterPythonStyle,FormatterPythonStyle}:
     *  Unknown conversion after <c>'!'</c>.                        */
    UnknownConversionPS                 =106,

    /** Thrown by \alib{text,FormatterPythonStyle,FormatterPythonStyle}:
     *  Precision specification not allowed with integer types.     */
    PrecisionSpecificationWithInteger   =107,

    /** Thrown by \alib{text,FormatterPythonStyle,FormatterPythonStyle}:
     *  Missing replacement strings after conversion <c>!Replace</c>.*/
    MissingReplacementStrings           =108,



    // Formatter Java Style
    /** Thrown by \alib{text,FormatterJavaStyle,FormatterJavaStyle}:
     *  Formatting of negative values in brackets is not supported. */
    NegativeValuesInBracketsNotSupported=201,

    /** Thrown by \alib{text,FormatterJavaStyle,FormatterJavaStyle}:
     *  Missing precision integral value after <c>'.'</c> character. */
    MissingPrecisionValueJS             =202,

    /** Thrown by \alib{text,FormatterJavaStyle,FormatterJavaStyle}:
     *  Output of floating point values in hexadecimal format not supported.  */
    HexadecimalFloatFormatNotSupported  =203,

    /** Thrown by \alib{text,FormatterJavaStyle,FormatterJavaStyle}:
     *  The alternate form '#' is not supported with given conversion.        */
    NoAlternateFormOfConversion         =204,

    /** Thrown by \alib{text,FormatterJavaStyle,FormatterJavaStyle}:
     *  Precision specification is not supported with given conversion.       */
    NoPrecisionWithConversion           =205,

    /** Thrown by \alib{text,FormatterJavaStyle,FormatterJavaStyle}:
     *  Unknown conversion suffix with with data/time conversion.             */
    UnknownDateTimeConversionSuffix     =206,

    /** Thrown by \alib{text,FormatterJavaStyle,FormatterJavaStyle}:
     *  Unknown conversion character.                               */
    UnknownConversionJS                 =207,


    // PropertyFormatter and PropertyFormatters
    /**
     * Thrown by constructor of \alib{text,PropertyFormatter}
     * when a property identifier parsed from the format string has no corresponding entry in the
     * \alib{text,PropertyFormatter::TCallbackTable,TCallbackTable} provided.
     */
    UnknownPropertyInFormatString       =501,

    /**
     * Exception entry added in method \alib{text,PropertyFormatter::Format}
     * when the underlying formatter throws.
     */
    ErrorInResultingFormatString        =502,

    /**
     * Thrown by \alib{text,PropertyFormatters::Format} if a configuration variable
     * can not be loaded or is empty.
     *
     * \note To prevent this, software using this class should provide
     *       \aworx{lib,Module::GetResourcePool,resourced} default values for each formatter variable.
     */
    MissingConfigurationVariable        =510,


    // class Paragraphs
    /**
     * Thrown by \alib{text,Paragraphs::AddMarked}
     * when an unknown marker token was found.
     */
    UnknownMarker                       =601,

    /**
     * Thrown by \alib{text,Paragraphs::AddMarked}
     * when a property identifier parsed from the format string has no corresponding entry in the
     * \alib{text,PropertyFormatter::TCallbackTable,TCallbackTable} provided.
     */
    EndmarkerWithoutStart               =602,

};  // Exceptions

/** ************************************************************************************************
 * This is a strict singleton class representing module \alib_text.
 *
 * The only instance is found with namespace variable \ref aworx::lib::TEXT.
 **************************************************************************************************/
class Text : public Module
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_module_singletons).
         ******************************************************************************************/
        Text();

    protected:
        /** ****************************************************************************************
         * Initializes namespace \alib \ref aworx::lib::text.
         *
         * @param phase  The initialization phase to perform.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void bootstrap( BootstrapPhases phase,
                                int argc, const char **argv,
                                const wchar_t **wargv ) override;

        /** ****************************************************************************************
         * Terminates this module.
         * @param phase  The shutdown phase to perform.
         ******************************************************************************************/
        virtual void shutdown( ShutdownPhases phase ) override;

}; // class Text
} // namespace aworx::lib[::text]

/** The singleton of module-class \alib{text,Text}. */
extern ALIB_API text::Text TEXT;

}} // namespace [aworx::lib]

ALIB_BOXING_VTABLE_DECLARE( aworx::lib::text::Exceptions, vt_text_exceptions )


ALIB_ENUMS_ASSIGN_RECORD(   aworx::lib::text::Exceptions, aworx::lib::results::ERException )
ALIB_RESOURCED_IN_MODULE(   aworx::lib::text::Exceptions, aworx::lib::TEXT, "E" )

#endif // HPP_ALIB_TEXT_TEXT

