// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGFORMAT_STRINGFORMAT
#define HPP_ALIB_STRINGFORMAT_STRINGFORMAT 1

#if !defined(HPP_ALIB_STRINGFORMAT_FWDS)
    #include "alib/stringformat/fwds.hpp"
#endif

#if !defined(HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

#if !defined(HPP_ALIB_LIB_MODULE)
#   include "alib/lib/module.hpp"
#endif

#if ALIB_MODULE_TIME
#   if !defined(HPP_ALIB_TIME_TICKS)
#       include "alib/time/ticks.hpp"
#   endif
#   if !defined(HPP_ALIB_TIME_DATETIME)
#       include "alib/time/datetime.hpp"
#   endif
#endif




#if defined( __GNUC__ ) && !defined(_ERRNO_H)
    #include <errno.h>
#endif


namespace aworx { namespace lib { namespace stringformat {

DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum] )
/** ****************************************************************************************
 * Enumeration of exceptions thrown with classes found in module \alibmod_stringformat.
 ******************************************************************************************/
enum class Exceptions
{
    /** Argument index '0' not allowed. */
    ArgumentIndexIs0                    =11,

    /** Argument index greater than number of arguments available. */
    ArgumentIndexOutOfBounds            =12,

    /** Incompatible type code given argument type found. */
    IncompatibleTypeCode                =13,


    /** Thrown by \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle}:
     *  Closing bracket <c>'}'</c> not found.                       */
    MissingClosingBracket               =101,

    //...
    //...
    //...
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum] )

    /** Thrown by \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle}:
     *  Missing precision integer value after <c>'.'</c> character. */
    MissingPrecisionValuePS             =102,

    /** Thrown by \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle}:
     *  Duplicate type code.                                        */
    DuplicateTypeCode                   =103,

    /** Thrown by \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle}:
     *  Unknown type code.                                          */
    UnknownTypeCode                     =104,

    /** Thrown by \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle}:
     *  Expected <c>'!'</c> not found.                              */
    ExclamationMarkExpected             =105,

    /** Thrown by \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle}:
     *  Unknown conversion after <c>'!'</c>.                        */
    UnknownConversionPS                 =106,

    /** Thrown by \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle}:
     *  Precision specification not allowed with integer types.     */
    PrecisionSpecificationWithInteger   =107,

    /** Thrown by \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle}:
     *  Missing replacement strings after conversion <c>!Replace</c>.*/
    MissingReplacementStrings           =108,



    // Formatter Java Style
    /** Thrown by \alib{stringformat,FormatterJavaStyle,FormatterJavaStyle}:
     *  Formatting of negative values in brackets is not supported. */
    NegativeValuesInBracketsNotSupported=201,

    /** Thrown by \alib{stringformat,FormatterJavaStyle,FormatterJavaStyle}:
     *  Missing precision integer value after <c>'.'</c> character. */
    MissingPrecisionValueJS             =202,

    /** Thrown by \alib{stringformat,FormatterJavaStyle,FormatterJavaStyle}:
     *  Output of floating point values in hexadecimal format not supported.  */
    HexadecimalFloatFormatNotSupported  =203,

    /** Thrown by \alib{stringformat,FormatterJavaStyle,FormatterJavaStyle}:
     *  The alternate form '#' is not supported with given conversion.        */
    NoAlternateFormOfConversion         =204,

    /** Thrown by \alib{stringformat,FormatterJavaStyle,FormatterJavaStyle}:
     *  Precision specification is not supported with given conversion.       */
    NoPrecisionWithConversion           =205,

    /** Thrown by \alib{stringformat,FormatterJavaStyle,FormatterJavaStyle}:
     *  Unknown conversion suffix with with data/time conversion.             */
    UnknownDateTimeConversionSuffix     =206,

    /** Thrown by \alib{stringformat,FormatterJavaStyle,FormatterJavaStyle}:
     *  Unknown conversion character.                               */
    UnknownConversionJS                 =207,


    // PropertyFormatter and PropertyFormatters
    /**
     * Thrown by constructor of \alib{stringformat,PropertyFormatter}
     * when a property identifier parsed from the format string has no corresponding entry in the
     * \alib{stringformat,PropertyFormatter::TCallbackTable,TCallbackTable} provided.
     */
    UnknownPropertyInFormatString       =501,

    /**
     * Exception entry added in method \alib{stringformat,PropertyFormatter::Format}
     * when the underlying formatter throws.
     */
    ErrorInResultingFormatString        =502,

    /**
     * Thrown by \alib{stringformat,PropertyFormatters::Format} if a configuration variable
     * can not be loaded or is empty.
     *
     * \note To prevent this, software using this class should provide
     *       \aworx{lib,Module::Resources,resourced} default values for each formatter variable.
     */
    MissingConfigurationVariable        =510,


    // class Text
    /**
     * Thrown by \alib{stringformat,Text::AddMarked}
     * when an unknown marker token was found.
     */
    UnknownMarker                       =601,

    /**
     * Thrown by \alib{stringformat,Text::AddMarked}
     * when a property identifier parsed from the format string has no corresponding entry in the
     * \alib{stringformat,PropertyFormatter::TCallbackTable,TCallbackTable} provided.
     */
    EndmarkerWithoutStart               =602,

};

/** ************************************************************************************************
 * This is a strict singleton class representing module \alibmod_stringformat.
 *
 * The only instance is found with namespace variable \ref aworx::lib::STRINGFORMAT.
 **************************************************************************************************/
class Stringformat : public Module
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_modsingletons).
         ******************************************************************************************/
        Stringformat();


    protected:
        /**
         * A singleton instance (if this module is a singleton) which is received with
         * #GetDefaultFormatter.
         */
        SPFormatter                  defaultFormatter;


    // #############################################################################################
    // Interface methods
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Returns the default formatter object. With the module initialization (method #init),
         * an object of type \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle} is
         * created and a second one of type
         * \alib{stringformat,FormatterJavaStyle,FormatterJavaStyle} is appended. Those
         * will be deleted with #terminationCleanUp, respectively when a different default formatter
         * is set (see #ReplaceDefaultFormatter) and no other shared pointer of the original one
         * exists.
         *
         * \note
         *   This method has an inline shortcut defined with \ref aworx::GetDefaultFormatter that
         *   invokes this method on the singleton of this class.
         *
         * @return The default formatter.
         ******************************************************************************************/
        inline
        SPFormatter&        GetDefaultFormatter()
        {
            return defaultFormatter;
        }

        /** ****************************************************************************************
         * Replaces the formatter currently defined as the default formatter.
         *
         * @param newFormatter The replacement formatter.
         ******************************************************************************************/
        ALIB_API
        void                ReplaceDefaultFormatter( Formatter* newFormatter );

    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Initializes namespace \alib \ref aworx::lib::stringformat.
         *
         * @param level  The initialization level to reach.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void        init( InitLevels level,
                                  int argc, const char** argv, const wchar_t** wargv )     override;

        /** ****************************************************************************************
         * Frees resources of the \alib \ref aworx::lib::system.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

}; // class Stringformat


/** ************************************************************************************************
 * This interface class exposes interface method \c Invoke which writes the content of the
 * box to the given \b %AString object in accordance with \c formatSpec.
 *
 * \see
 *   For more information about this class see chapter
 *   \ref alib_stringformat_custom_types "3. Formatting Custom Types" of the
 *   \ref alib_mod_stringformat "Programmer's Manual" of module \alibmod_nolink_stringformat.
 **************************************************************************************************/
struct FFormat
{
    /**
     * Signature of the invokable function.<br>
     * Implementations write the content of \p{box} to the given \b %AString object \p{target} in
     * accordance to the type-specific format specification \p{formatSpec}.
     *
     * @param self       The box that the function was invoked on.
     * @param formatSpec The specification of the format (type specific)
     * @param target     The AString object receiving the formatted string.
     */
    using Signature = void (*) ( const Box& self, const String& formatSpec, AString& target );
};

#if ALIB_MODULE_SYSTEM && ALIB_MODULE_TIME
/** ************************************************************************************************
 * Implementation of \ref aworx::lib::stringformat::FFormat "FFormat" for boxable type
 * \alib{time,DateTime}.<br>
 * Writes the content of \p{box} (which is of type \b %DateTime) to the given \b %AString
 * object \p{target} using a local instance of class \alib{system,CalendarDateTime} and its method
 * \alib{system,CalendarDateTime::Format}.
 *
 * \note
 *   This interface implementation is only available if modules \alibmod_system and \alibmod_time
 *   are included in the library distribution.
 *
 * @param self       The box that the function was invoked on.
 * @param formatSpec The specification of the format (type specific)
 * @param target     The AString object receiving the formatted string.
 **************************************************************************************************/
ALIB_API void FFormat_DateTime( const Box& self, const String& formatSpec, AString& target );
#endif

} namespace strings {

#if ALIB_MODULE_TIME


#if ALIB_DOCUMENTATION_PARSER
namespace APPENDABLES { // Documentation fake namespace
#endif
/** Specialization of functor \alib{strings,T_Append} for type \c Format::Oct. */
template<typename TChar> struct T_Append<time::DateTime::Duration ,TChar>
{
    /** ********************************************************************************************
     * Appends a human readable string representation of objects of templated inner type
     * \alib{time,TimePointBase::Duration} of type \alib{time,DateTime}.
     *
     * \see
     *   For details of the conversion, see
     *   \ref anchor_T_ToString_TimePointBase_Duration "documentation of sibling method"
     *   for type \b %Ticks, which shares this method's implementation.
     *
     *
     * @param target    The \b AString that \b Append was invoked on.
     * @param duration  The duration to append.
     **********************************************************************************************/
    ALIB_API void operator()( TAString<TChar>& target,  const time::DateTime::Duration duration );
};

/** Specialization of functor \alib{strings,T_Append} for type \c Format::Oct. */
template<typename TChar> struct T_Append<time::Ticks::Duration ,TChar>
{
    /** ********************************************************************************************
     * \anchor anchor_T_ToString_TimePointBase_Duration
     * Appends a human readable string representation of objects of templated inner type
     * \alib{time,TimePointBase::Duration} of type \alib{time,Ticks}.
     *
     * Depending on the length of the duration, a different human readable time unit or combination
     * of it is used. The following rules are checked from top to bottom:
     * - If zero, resource string \b "TS_ZERO" is written.
     * - If negative, a minus sign <c>'-'</c> is written and the value is negated.
     * - If greater than 10 days, writes the number of days as floating point number.
     * - If between 1 and 10 days, writes the integral number of days and the additional hours as
     *   floating point number.
     * - If greater than an hour, writes the integral number of hours and integral minutes.
     * - If greater than a minute, writes the integral number of minutes and integral seconds.
     * - If greater than a second, writes the number of seconds as floating point number.
     * - If greater than a millisecond, writes the number of milliseconds as floating point number.
     * - If greater than a microsecond, writes the number of microseconds as floating point number.
     * - If greater than a nanosecond, writes the number of nanoseconds as floating point number.
     *
     * All floating point numbers are written with two digits fractional precision.<br>
     * The unit symbols are read from the \ref alib_mod_resources "resources" of module class
     * \alib{stringformat,Stringformat}. The resource keys used are
     * \b "TS_ZERO"
     * \b "DayP",     \b "DayS",
     * \b "HourP",    \b "HourS",
     * \b "MinP",     \b "MinS",
     * \b "SecP",     \b "SecS",
     * \b "MlSecP",   \b "MlSecS",
     * \b "McSecP",   \b "McSecS",
     * \b "NSecP" and \b "NSecS",
     *
     *
     * @param target    The \b AString that \b Append was invoked on.
     * @param duration  The duration to append.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target,  const time::Ticks::Duration duration );
};

extern template ALIB_API void T_Append<time::Ticks::Duration, nchar>::operator()( TAString<nchar>&, const time::Ticks::Duration );
extern template ALIB_API void T_Append<time::Ticks::Duration, wchar>::operator()( TAString<wchar>&, const time::Ticks::Duration );
extern template ALIB_API void T_Append<time::Ticks::Duration, xchar>::operator()( TAString<xchar>&, const time::Ticks::Duration );

#if ALIB_DOCUMENTATION_PARSER
} // APPENDABLES documentation fake namespace
#endif


#endif  // ALIB_MODULE_TIME


} // namespace aworx::lib[::strings]

/** The singleton of module-class \alib{stringformat,Stringformat}. */
extern ALIB_API stringformat::Stringformat STRINGFORMAT;

} // namespace aworx[::lib]

/** ************************************************************************************************
 * This inline function is is injected to this namespace as a shortcut to:
 *
 *      aworx::lib::stringformat::STRINGFORMAT.GetDefaultFormatter()
 *
 * @return A shared pointer to the default formatter of \alib.
 **************************************************************************************************/
inline SPFormatter GetDefaultFormatter()
{
    return lib::STRINGFORMAT.GetDefaultFormatter();
}

} // namespace [aworx]

ALIB_BOXING_VTABLE_DECLARE( aworx::lib::stringformat::Exceptions   , vt_stringformat_exceptions )

DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum2] )
ALIB_EXCEPTIONS(  aworx::lib::stringformat::Exceptions, aworx::lib::STRINGFORMAT, "FmtExceptions" )
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum2] )



#endif // HPP_ALIB_STRINGFORMAT_STRINGFORMAT

