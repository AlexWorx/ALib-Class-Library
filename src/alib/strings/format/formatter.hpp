// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include guard
#ifndef HPP_ALIB_STRINGS_FORMAT_FORMATTER
#define HPP_ALIB_STRINGS_FORMAT_FORMATTER 1

// to preserve the right order, we check if string is already included
#if !defined(HPP_ALIB_STRINGS_STRING)
    #error "include 'alib/alib.hpp' before including this header. Also make sure to include ALib Module Strings"
#endif


#if !ALIB_MODULE_BOXING
    #error "class Formatter of library ALib can not be used without the use of %ALib %Boxing."
#endif

#if !ALIB_FEAT_BOXING_FTYPES
    #error "class Formatter relies on the default implementation of boxing fundamental C++ types."
#endif

// #################################################################################################
// includes
// #################################################################################################
#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif

#if ALIB_MODULE_CORE
    #if !defined (HPP_ALIB_THREADS_THREADLOCK)
        #include "alib/threads/threadlock.hpp"
    #endif
#endif

#if !defined(HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif

#if !defined (HPP_ALIB_LANG_EXCEPTION)
    #include "alib/lang/exception.hpp"
#endif

#if ALIB_MODULE_CORE || ALIB_MODULE_CONFIGURATION
    #if !defined (HPP_ALIB_THREADS_THREADLOCK)
        #include "alib/threads/threadlock.hpp"
    #endif
#else
    #if !defined (HPP_ALIB_LANG_OWNER)
        #include "alib/lang/owner.hpp"
    #endif
    #define ALIB_LOCK
#endif

namespace aworx { namespace lib { namespace strings {

/**
 * This sub-namespace  provides some classes used for formatting data in string buffers of
 * type \alib{strings,AStringBase,AString}.
 *
 * \note
 *   This namespace is available only in <b>%ALib Module</b> distributions that includes
 *   modules <b>%ALib String </b> and <b>%ALib %Boxing</b>.
 */
namespace format {


//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum]
/** ****************************************************************************************
 * Enumeration of exceptions thrown with classes found in namespace
 * #aworx::lib::strings::format.
 ******************************************************************************************/
enum class Exceptions
{
    // general formatter errors
    /// Argument index '0' not allowed.
    ArgumentIndexIs0                    =11,

    /// Argument index greater than number of arguments available.
    ArgumentIndexOutOfBounds            =12,

    /// Incompatible type code given argument type found.
    IncompatibleTypeCode                =13,


    // Formatter Python Style
    /** Thrown by \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}:
     *  Closing bracket <c>'}'</c> not found.                       */
    MissingClosingBracket               =101,

    /** Thrown by \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}:
     *  Missing precision integer value after <c>'.'</c> character. */
    MissingPrecisionValuePS             =102,

    /** Thrown by \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}:
     *  Duplicate type code.                                        */
    DuplicateTypeCode                   =103,

    /** Thrown by \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}:
     *  Unknown type code.                                          */
    UnknownTypeCode                     =104,

    /** Thrown by \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}:
     *  Expected <c>'!'</c> not found.                              */
    ExclamationMarkExpected             =105,

    /** Thrown by \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}:
     *  Unknown conversion after <c>'!'</c>.                        */
    UnknownConversionPS                 =106,

    /** Thrown by \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}:
     *  Precision specification not allowed with integer types.     */
    PrecisionSpecificationWithInteger   =107,

    /** Thrown by \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}:
     *  Missing replacement strings after conversion <c>!Replace</c>.*/
    MissingReplacementStrings           =108,



    // Formatter Java Style
    /** Thrown by \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}:
     *  Formatting of negative values in brackets is not supported. */
    NegativeValuesInBracketsNotSupported=201,

    /** Thrown by \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}:
     *  Missing precision integer value after <c>'.'</c> character. */
    MissingPrecisionValueJS             =202,

    /** Thrown by \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}:
     *  Output of floating point values in hexadecimal format not supported.  */
    HexadecimalFloatFormatNotSupported  =203,

    /** Thrown by \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}:
     *  The alternate form '#' is not supported with given conversion.        */
    NoAlternateFormOfConversion         =204,

    /** Thrown by \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}:
     *  Precision specification is not supported with given conversion.       */
    NoPrecisionWithConversion           =205,

    /** Thrown by \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}:
     *  Unknown conversion suffix with with data/time conversion.             */
    UnknownDateTimeConversionSuffix     =206,

    /** Thrown by \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}:
     *  Unknown conversion character.                               */
    UnknownConversionJS                 =207,


    // PropertyFormatter and PropertyFormatters
    /**
     * Thrown by constructor of \alib{strings::format,PropertyFormatter}
     * when a property identifier parsed from the format string has no corresponding entry in the
     * \alib{strings::format,PropertyFormatter::TCallbackTable,TCallbackTable} provided.
     */
    UnknownPropertyInFormatString       =501,

    /**
     * Exception entry added in method \alib{strings::format,PropertyFormatter::Format}
     * when the underlying formatter throws.
     */
    ErrorInResultingFormatString        =502,

    /**
     * Thrown by \alib{strings::format,PropertyFormatters::Format} if a configuration variable
     * can not be loaded or is empty.
     *
     * \note To prevent this, software using this class should provide
     *       \alib{lang,Library::Res,resourced} default values for each formatter variable.
     */
    MissingConfigurationVariable        =510,


    // SimpleText
    /**
     * Thrown by \alib{strings::format,SimpleTextBase::AddMarked,SimpleText::AddMarked}
     * when an unknown marker token was found.
     */
    UnknownMarker                       =601,

    /**
     * Thrown by \alib{strings::format,SimpleTextBase::AddMarked,SimpleText::AddMarked}
     * when a property identifier parsed from the format string has no corresponding entry in the
     * \alib{strings::format,PropertyFormatter::TCallbackTable,TCallbackTable} provided.
     */
    EndmarkerWithoutStart               =602,

};
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum]


/** ************************************************************************************************
 * Writes formatted text into an \alib{strings,AStringBase,AString}.
 *
 * \note
 *   The C++ version of this class relies heavily on classes found in module (namespace)
 *   \ref aworx::lib::boxing "ALib Boxing". In short, <b>%ALib %Boxing</b> allows to create methods
 *   that accept any type of argument as parameter, without the need of type conversion,
 *   wrapper classes, etc. Furthermore <b>%ALib %Boxing</b> allows to add virtual interfaces
 *   to C++ types, independent from the original implementation of the type itself. This means
 *   that any 3rd-party type can be adopted to support "boxing" and hence to be formatted with this
 *   class - without changing their implementation and with no access to their source code!<br>
 *   Knowledge about <b>%ALib %Boxing</b> is not needed to use this class with standard C++ types.
 *   Only if a string representation of custom types should be generated by this class with
 *   passing such custom types "as is", then support for <b>%ALib %Boxing</b> in respect to such
 *   custom types have to be added.
 *
 * \note
 *   The  \ref aworx::lib::boxing "detailed documentation" of <b>%ALib %Boxing</b> describes how
 *   to do this. The "boxing interfaces" that are needed to be implemented for custom types
 *   to work with this formatters are
 *   \ref aworx::lib::strings::boxing::IApply  "IApply" and optionally
 *   \ref aworx::lib::strings::boxing::IFormat "IFormat". The latter is used with
 *   \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}, if a custom format
 *   specification is given for a replacement field.
 *
 * ##Different Supported Formatting Syntax##
 *
 * While this class is abstract, only derived classes that support a specific format syntax can
 * be instantiated and used. Built-in formatters provided with \alib are:
 * - \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}, which mimics
 *   the [Python formatting syntax](https://docs.python.org/3.5/library/string.html#format-string-syntax)
 *   and
 * - \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}, which mimics
 *   the formatting syntax of corresponding
 *   [Java class "Formatter"](http://docs.oracle.com/javase/8/docs/api/java/util/Formatter.html)
 *
 * One of the design goals of \alib is to provide a library for (currently) three programming
 * languages \b C++, \b Java and \b C#, which allows to reach a certain level of source code
 * compatibility if used in cross-platform/language projects. Now, why do we provide
 * <b>Python Style</b> instead of to mimic what is coming with \b .Net languages like \b C#?
 *
 * The answer is that we considered <b>Python Style</b> to be more powerful. It is also more powerful
 * than the <b>Java Style</b> formatting syntax. But the latter is in its basics similar to
 * good old \c printf format string style and hence for \alib a must have.
 * The good news is that in its very basics, <b>Python Style</b> is similar to \b .Net formatting.
 * This way, there is some "familiar basic syntax" available for everybody.<br>
 * Finally, it is worth to mention that the \alib implementation of <b>Python Style</b> formatting
 * is extending the original standard even a little.
 *
 *
 * ##Concatenated Formatters And Concatenated Format Operations##
 *
 * In comparison to "usual", similar classes that are found in other libraries/languages, there
 * are two main differences with this class:
 * 1. This class supports the concatenation of formatters by passing an instance of
 *    a different derived type in the constructor. A formatter that has a (list of) concatenated
 *    other formatter(s) attached, allows to detect the provided format syntax by passing the format
 *    operation recursively to the concatenated formatter(s). Of-course this approach is limited to
 *    such syntax definitions that are not ambiguous. (...It works well with formatters
 *    \b %FormatterPythonStyle and \b %FormatterJavaStyle!)
 *
 * 2. Usually (in other libraries) the interface of formatting methods accept a dedicated
 *    format string and list of arguments to format. Method #Format of this class however
 *    expects just an argument list. The processing then is done as follows:
 *    The first argument in the list is tested to be a format string.  If it is, method #format
 *    will report how many arguments form the list have been "consumed" and formatting will continue
 *    with the first argument after the last consumed one. If the actual argument is not identified
 *    as a format string (tested with each concatenated formatter), the string is just appended to
 *    the target and formatting continues with the next argument in the list.<br>
 *    If at the end of this processing loop a single argument is left, then this is not checked
 *    for being a format string. Instead, it will be just appended to the target as is.
 *    An argument that potentially could be a format string is simply skipped (ignored) if it
 *    evaluates to \c nullptr.
 *
 * This behaviour has the following consequences:
 * - No format string needs to be passed. In this case, the arguments are appended to the target
 *   \b %AString in their sequence order
 * - More than one format string (with according) arguments might be passed.
 * - The different format strings might even follow a different syntax.
 *
 * The biggest \b advantage of this implementation is that format strings and arguments might be
 * collected prior to invoking the formatter. Collection might happen at different places and times
 * in a process with the only restriction that not more or less arguments might be added to the
 * overall list than referred to in the format string. In other words, format strings have to
 * be well set in respect to the arguments they consume.
 *
 * The biggest <b>potential issue</b> of this approach is that a string argument that is not meant
 * to be a format string might be accidentally interpreted as one if it happens to contain one or
 * more placeholder characters that are matched by one of the concatenated formatters. (In this case it
 * it is likely that the format is invalid and an error is written. In debug mode, even an assertion
 * might be raised). To avoid such ambiguity, the following considerations have to be taken into
 * account when passing arguments:
 * - Ambiguities might only happen with string-type arguments. Other types that are converted to
 *   string representations are never considered a potential format string.
 * - Strings that contain a newline-character (ascii \b 0x0a) are not considered to be a format string.
 *   (Newline characters in a format string have to be escaped using a formatter-specific notation,
 *   e.g. \c "\n").
 * - A string-type argument that is the last in the list (or the only one) is safe
 *   (no ambiguity can occur) because the last argument is never checked for being a format string.
 * - %String-type arguments that do not contain placeholder characters defined by one of the formatters
 *   are safe.
 *
 * \attention
 *   <b>The rule that results from this is:</b><br>
 *   If none of the conditions mentioned above can be guaranteed, then the argument in question
 *   should be passed together with a preceding simple formatting string, for example,
 *   \c "{}" for \b %FormatterPythonStyle or
 *   \c "%s" in case of \b %FormatterJavaStyle.
 *
 * ##Choosing The %Formatter##
 * As elaborated above, there are two built-in formatter types and those can be concatenated.
 * Finally, there is the option to implement an own, custom formatter. For the undecided, the
 * recommendation is to use <b>Java Style</b> for simple, quick <c>printf-</c> like formatting
 * and to choose <b>Python Style</b> in the moment formatting requirements get more complex.
 * A special case and good sample for the use of  \alib formatting features is found in the
 * [ALox Logging Library](https://github.com/AlexWorx/ALox-Logging-Library). Here the special
 * situation is that \alox itself is a library and hence exposes its formatters used to
 * users. Therefore \alox (by default) uses a <b>Python Style</b> formatter and a concatenated
 * <b>Java Style</b> version. This way, simple samples will work from scratch for users of all
 * three languages!
 *
 * ##Usage##
 * The class is abstract, hence only derived types can be instantiated.
 * Those types are constructed with optionally providing a concatenated instance of another
 * derived type. The optional concatenated formatter object has a lower priority in respect to
 * detecting a format string.
 *
 * Two overloaded methods \b Format exists:
 * - The first method accepts a target \b AString and a variadic list of arguments. This method
 *   can be used if the format string(s) and all arguments are available/known at the point of
 *   invocation.
 * - The second method accepts a target \b AString and a list of arguments. This method
 *   can be used if format string(s) and arguments are collected prior to the final invocation
 *   of the formatter.
 *
 * \note
 *   The use of this class is not "thread safe".
 *
 * ##Implementing A Custom %Formatter##
 * To implement a custom formatter, the abstract methods found in this class have to be implemented.
 * With class
 * \ref aworx::lib::strings::format::FormatterStdImpl "FormatterStdImpl" a - still abstract -
 * descendant class is provided. This class implements method #format but introduces a set of
 * new abstract methods to overwrite. Although, those are more methods, these methods are probably
 * more simple to implement and therefore deriving from this class might be considered for new
 * custom types.
 *
 * The two built-in <b>Formatter</b>s provided with \alib,
 * \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle} and
 * \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle} derive from
 * \b %FormatterStdImpl and copying the source code of one of them might be a good start for
 * implementing a custom type.
 *
 * @tparam TChar    The character type. Implementations for \c char and \c wchar_t are provided
 *                  with type definitions \ref aworx::NFormatter and \ref aworx::WFormatter.
 **************************************************************************************************/
template<typename TChar>
class FormatterBase
#if ALIB_MODULE_CORE || ALIB_MODULE_CONFIGURATION
   : public threads::ThreadLock
#endif
{
    #if !defined(DOX_PARSER)
        friend class aworx::lib::strings::Strings;
    #endif

    #if !(ALIB_MODULE_CORE || ALIB_MODULE_CONFIGURATION)
        int cntAcquirements = 0;
    #endif

    // #############################################################################################
    //  Internal fields
    // #############################################################################################
    protected:
        /** A list of boxes. This is reset with every new invocation of variadic template method
         *  #Format   */
        Boxes                   boxes;

        /// A buffer for conversion of multi-byte format strings to char-strings.
        AStringBase<TChar>      mbsFormatString;

        ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL

    // #############################################################################################
    //  public fields
    // #############################################################################################
    public:
        /**
         * This number format is used to store default attributes for formatting numbers.
         * Likewise #AlternativeNumberFormat, it usually is not used directly for formatting by
         * descendants.
         * Instead, at the beginning of parsing a next placeholder field, values are copied to
         * a local copy. During the parsing process, values are then modified only in this local
         * copy, probably taken from #AlternativeNumberFormat.
         *
         * This object is to be initialized in the constructor (of descendants) once to meet the
         * formatting defaults of the corresponding specification.
         * If after construction, attribute values of this object are changed, such changes are
         * applied to all number formatting.
         */
        NumberFormatBase<TChar>     DefaultNumberFormat;

        /**
         * This number format is used to store alternative attributes. Likewise #DefaultNumberFormat,
         * it is never used directly for formatting.
         * Instead when processing the placeholder syntax, alternatives get copied from either this
         * object or from #DefaultNumberFormat.
         *
         * This object is initialized in the constructor (of descendants) once to meet the
         * formatting defaults of the corresponding specification.
         *
         * With the implementations of this class provided with \alib, not all fields in this object
         * are used. The fields used are:
         * <p>
         * - Locale-specific versions of floating point separators:
         *   - \alib{strings,NumberFormatBase::DecimalPointChar,NumberFormat::DecimalPointChar}
         *   - \alib{strings,NumberFormatBase::ThousandsGroupChar,NumberFormat::ThousandsGroupChar}
         *
         *   These are retrieved according to the current locale once in the constructor. To
         *   change the locale, these fields can be changed.
         *
         * - Lower case versions of floating point literals:
         *   - \alib{strings,NumberFormatBase::ExponentSeparator,NumberFormat::ExponentSeparator}
         *   - \alib{strings,NumberFormatBase::INFLiteral,NumberFormat::INFLiteral}
         *   - \alib{strings,NumberFormatBase::NANLiteral,NumberFormat::NANLiteral}
         *
         * - Lower case versions of prefix literals that indicate the base of integer values:
         *   - \alib{strings,NumberFormatBase::BinLiteralPrefix,BinLiteralPrefix}
         *   - \alib{strings,NumberFormatBase::HexLiteralPrefix,HexLiteralPrefix}
         *   - \alib{strings,NumberFormatBase::OctLiteralPrefix,OctLiteralPrefix}
         */
        NumberFormatBase<TChar>     AlternativeNumberFormat;

        /**
         *  An optional, next formatter. If set, this formatter will be invoked for a format string
         *  that does not contain recognized placeholders.
         *  \attention
         *    This field is public and not further maintained by this class. Setting the field lies
         *    completely in the responsibility of the user. E.g. cyclic settings must be avoided.
         *    Also, object lifecycle management is completely up to the user.
         */
        FormatterBase<TChar>*       Next                                                  = nullptr;

    // #############################################################################################
    //  Constructor/destructor/ThreadLock overrides
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Destructs an object of this class.
         * Note that concatenated formatters are not deleted automatically.
         ******************************************************************************************/
        virtual        ~FormatterBase()
        {}

        /** ****************************************************************************************
         * Overrides (non-virtual) \alib{threads,ThreadLock::Acquire}.
         * This method is inherently called by overloaded methods #Format and hence no explicit
         * invocation is needed.
         *
         * However, this method checks if this was the first acquisition and if so, invokes method
         * #reset. Therefore, a series of formatting calls may be performed without resetting
         * the internal state, by acquiring the object once prior to to the series of invocations
         * and releasing it afterwards.
         *
         * \note
         *   In the debug-compilation of an application, this method accepts the parameters,
         *   providing information about the caller. In the release version these parameters do not
         *   exist. Therefore use macro #ALIB_CALLER_PRUNED to provide the parameters:
         *
         *          sample.Acquire( ALIB_CALLER_PRUNED );
         *
         * @param file  Caller information. Available only in debug compilations.
         * @param line  Caller information. Available only in debug compilations.
         * @param func  Caller information. Available only in debug compilations.
         ******************************************************************************************/
         #if ALIB_DEBUG
         ALIB_API void      Acquire( const NTString& file, int line, const NTString& func );
         #else
         ALIB_API void      Acquire();
         #endif


    // #############################################################################################
    //  Interface
    // #############################################################################################

        /** ****************************************************************************************
         * Applies the given list of arguments to the target using this and optionally the attached
         * sub-formatter(s).
         *
         * @param target     An AString that takes the result.
         * @param args       The objects to apply.
         *******************************************************************************************/
        inline
        void                Format( AStringBase<TChar>& target, const Boxes&  args )
        {
            ALIB_LOCK
            formatLoop( target, args );
        }

        /** ****************************************************************************************
         * Variadic template method that accepts a target \b %AString and a list of applicable
         * arguments.
         *
         * @param target   An AString that takes the result.
         * @param args     The variadic list of arguments to be used with formatters.
         * @tparam BoxedObjects Variadic template type list.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        inline
        void                Format( AStringBase<TChar>& target, const BoxedObjects&... args )
        {
            ALIB_LOCK

            // create argument objects using implicit constructor invocation
            boxes.clear();
            boxes.Add( args... );

            // invoke format
            formatLoop( target, boxes );
        }

        /** ****************************************************************************************
         * Clones and returns a copy of this formatter.
         *
         * If a formatter is attached to field \alib{strings::format,FormatterBase::Next}, it is
         * cloned as well.
         *
         * @returns An object of the same (derived) type and the same custom settings.
         ******************************************************************************************/
        virtual FormatterBase*  Clone()                                                         = 0;

        /** ****************************************************************************************
         * Clones the settings from the given formatter.
         * @param reference  The formatter to copy settings from.
         ******************************************************************************************/
        ALIB_API
        virtual void            CloneSettings( FormatterBase& reference );

    // #############################################################################################
    //  Protected methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Virtual method which is invoked with each invocation of #Format.
         * The default implementation does nothing.
         ******************************************************************************************/
        virtual void            initializeFormat()                                                {}

        /** ****************************************************************************************
         * Virtual method to used to reset internal states. This method is invoked only with
         * the first \ref Acquire "acquisition" of a formatter. This way, reset of internal states
         * can be suppressed over a series of format calls by acquiring the formatter explicitly
         * prior to the series.
         *
         * As a sample, derived type \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle} clears its auto-tab
         * and auto-width positions.
         *
         *
         * The default implementation does nothing.
         ******************************************************************************************/
        virtual void        reset()                                                               {}


        /** ****************************************************************************************
         * The abstract format method that needs to be implemented by descendants.
         * Note that parameter \p{startIdx} and the demanded return value together comprise the
         * possibility to use more than one formatter in parallel and to perform multiple format
         * operations on suitable argument lists. This demands the implementation of this method to
         * \b not copy the format string to the \p{target} in the case that no 'escape sequence'
         * was found. For further information, see the general documentation of this class.
         *
         * @param target        The %AString that takes the result.
         * @param formatString  The format string.
         * @param args          The objects to apply.
         * @param startArgument The first object in \p{args} to convert.
         *
         * @return The number of args consumed.
         ******************************************************************************************/
        virtual int         format( AStringBase<TChar>&        target,
                                    const StringBase<TChar>&   formatString,
                                    const Boxes&               args,
                                    int                        startArgument )                  = 0;

        /** ****************************************************************************************
         * The format loop implementation. Searches format strings in \p{args} and tests
         * if \c this or #Next is capable of processing it.
         *
         * @param target     An AString that takes the result.
         * @param args       The objects to apply.
         ******************************************************************************************/
        ALIB_API
        void                formatLoop( AStringBase<TChar>& target, const Boxes&  args );
};

//! @cond NO_DOX

extern template ALIB_API void FormatterBase<char   >::Acquire( ALIB_DBG(const NTString&, int, const NTString&));
extern template ALIB_API void FormatterBase<char   >::CloneSettings(FormatterBase<char>&);
extern template          void FormatterBase<char   >::initializeFormat();
extern template          void FormatterBase<char   >::reset();
extern template ALIB_API void FormatterBase<char   >::formatLoop(NAString&,const Boxes&);

extern template ALIB_API void FormatterBase<wchar_t>::Acquire( ALIB_DBG(const NTString&, int, const NTString&));
extern template ALIB_API void FormatterBase<wchar_t>::CloneSettings(FormatterBase<wchar_t>&);
extern template          void FormatterBase<wchar_t>::initializeFormat();
extern template          void FormatterBase<wchar_t>::reset();
extern template ALIB_API void FormatterBase<wchar_t>::formatLoop(WAString&,const Boxes&);

//! @endcond

}}}} // namespace [aworx::lib::strings::format]


//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum2]
ALIB_LANG_EXCEPTIONS(  aworx::lib::strings::format::Exceptions, aworx::lib::STRINGS, ASTR("FmtExceptions") )
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum2]

#endif // HPP_ALIB_STRINGS_FORMAT_FORMATTER
