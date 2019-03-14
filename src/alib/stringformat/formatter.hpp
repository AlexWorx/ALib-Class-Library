// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGFORMAT_FORMATTER
#define HPP_ALIB_STRINGFORMAT_FORMATTER 1

#if !defined(HPP_ALIB_STRINGFORMAT_STRINGFORMAT)
    #include "alib/stringformat/stringformat.hpp"
#endif

#if ALIB_MODULE_THREADS
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

#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

#if ALIB_MODULE_THREADS
    #if !defined (HPP_ALIB_THREADS_THREADLOCK)
        #include "alib/threads/threadlock.hpp"
    #endif
#else
    #if !defined (HPP_ALIB_OWNER)
        #include "alib/lib/owner.hpp"
    #endif
#endif

namespace aworx { namespace lib { namespace stringformat {

/** ************************************************************************************************
 * This is an abstract base class to implement an \alib string formatter. A string formatter
 * uses a "format string" to transform arguments into formatted text. The format string defines
 * how the arguments are transformed by offering a "placeholder syntax".
 *
 * With this information, it becomes clear that different formatter types (derived types that offer
 * a concrete placeholder syntax) all can have the same interface methods.
 * This class defines this abstract interface.
 *
 * Built-in formatters, provided by module \alibmod_stringformat and derived from this class, are
 * \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle} and
 * \alib{stringformat,FormatterJavaStyle,FormatterJavaStyle}.
 *
 * ## Friends ##
 * class \alib{stringformat,Stringformat}
 **************************************************************************************************/
class Formatter
#if ALIB_MODULE_THREADS
   : public threads::ThreadLock
#endif
{
    #if !ALIB_DOCUMENTATION_PARSER
        friend class aworx::lib::stringformat::Stringformat;
    #endif

    #if !ALIB_MODULE_THREADS
        int cntAcquirements = 0;
    #endif

    // #############################################################################################
    //  Internal fields
    // #############################################################################################
    protected:
        /** A list of boxes. This is reset with every new invocation of variadic template method
         *  #Format   */
        Boxes           boxes;

        /** A buffer used for conversion of the next argument, if it is not of string type. */
        AString         formatStringBuffer;

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
        NumberFormat    DefaultNumberFormat;

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
         *   - \alib{strings,TNumberFormat::DecimalPointChar,NumberFormat::DecimalPointChar}
         *   - \alib{strings,TNumberFormat::ThousandsGroupChar,NumberFormat::ThousandsGroupChar}
         *
         *   These are retrieved according to the current locale once in the constructor. To
         *   change the locale, these fields can be changed.
         *
         * - Lower case versions of floating point literals:
         *   - \alib{strings,TNumberFormat::ExponentSeparator,NumberFormat::ExponentSeparator}
         *   - \alib{strings,TNumberFormat::INFLiteral,NumberFormat::INFLiteral}
         *   - \alib{strings,TNumberFormat::NANLiteral,NumberFormat::NANLiteral}
         *
         * - Lower case versions of prefix literals that indicate the base of integer values:
         *   - \alib{strings,TNumberFormat::BinLiteralPrefix,BinLiteralPrefix}
         *   - \alib{strings,TNumberFormat::HexLiteralPrefix,HexLiteralPrefix}
         *   - \alib{strings,TNumberFormat::OctLiteralPrefix,OctLiteralPrefix}
         */
        NumberFormat    AlternativeNumberFormat;

        /**
         *  An optional, next formatter. If set, this formatter will be invoked for a format string
         *  that does not contain recognized placeholders.
         *  \attention
         *    This field is public and not further maintained by this class. Setting the field lies
         *    completely in the responsibility of the user. E.g. cyclic settings must be avoided.
         *    Also, object lifecycle management is completely up to the user.
         */
        std::shared_ptr<Formatter>  Next;

    // #############################################################################################
    //  Constructor/destructor/ThreadLock overrides
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Destructs an object of this class.
         * Note that concatenated formatters are not deleted automatically.
         ******************************************************************************************/
        virtual        ~Formatter()
        {}

        /** ****************************************************************************************
         * Overrides (non-virtual) \alib{threads,ThreadLock::Acquire}.
         * This method is inherently called by overloaded methods #Format and hence no explicit
         * invocation is needed.
         *
         * This method checks if this was the first acquisition and if so, invokes method
         * #reset. Therefore, a series of formatting calls may be performed without resetting
         * the internal state, by acquiring the object once prior to the series of invocations
         * and releasing it afterwards.
         *
         * Furthermore, this method invokes itself on potentially attached formatter #Next
         * (recursion).
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
         ALIB_API void      Acquire( const NCString& file, int line, const NCString& func );
         #else
         ALIB_API void      Acquire();
         #endif

        /** ****************************************************************************************
         * Overrides (non-virtual) \alib{threads,ThreadLock::Release}, which is called.
         * In addition, the method is called on potentially attached formatter #Next (recursion).
         ******************************************************************************************/
         ALIB_API void      Release();

    // #############################################################################################
    //  Interface
    // #############################################################################################

        /** ****************************************************************************************
         * Formats the given list of arguments using this and optionally the attached
         * sub-formatter(s).
         *
         * This method is to be chosen, if the list of arguments is created prior to invoking
         * this method.
         *
         * @param target     An AString that takes the result.
         * @param args       The arguments to be used with formatters.
         *******************************************************************************************/
        inline
        void                FormatArgs( AString& target, const Boxes&  args )
        {
            ALIB_LOCK
            formatLoop( target, args );
        }

        /** ****************************************************************************************
         * Variadic template method that accepts a target \b %AString and a list of arguments.
         *
         * @param target   An AString that takes the result.
         * @param args     The variadic list of arguments to be used with formatters.
         * @tparam Args    Variadic template type list.
         ******************************************************************************************/
        template <typename... Args>
        inline
        void                Format( AString& target, const Args&... args )
        {
            ALIB_LOCK

            // create argument objects using implicit constructor invocation
            boxes.clear();
            boxes.Add( Boxes ( args...) );

            // invoke format
            formatLoop( target, boxes );
        }

        /** ****************************************************************************************
         * Variadic template method that accepts a target \b %AString and a list of arguments.
         *
         * @param target   An AString that takes the result.
         * @param args     The variadic list of arguments to be used with formatters.
         * @tparam Args    Variadic template type list.
         ******************************************************************************************/
        template <typename... Args>
        inline
        void                Format( AString& target, Args&&... args )
        {
            ALIB_LOCK

            // create argument objects using implicit constructor invocation
            boxes.clear();
            boxes.Add( Boxes (std::forward<Args>( args )...) );

            // invoke format
            formatLoop( target, boxes );
        }

        /** ****************************************************************************************
         * Clones and returns a copy of this formatter.
         *
         * If a formatter is attached to field \alib{stringformat,Formatter::Next}, it is
         * cloned as well.
         *
         * @returns An object of the same (derived) type and the same custom settings.
         ******************************************************************************************/
        virtual Formatter*  Clone()                                                         = 0;

        /** ****************************************************************************************
         * Clones the settings from the given formatter.
         * @param reference  The formatter to copy settings from.
         ******************************************************************************************/
        ALIB_API
        virtual void        CloneSettings( Formatter& reference );

    // #############################################################################################
    //  Protected methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Virtual method which is invoked with each invocation of #Format.
         * The default implementation does nothing.
         ******************************************************************************************/
        virtual void        initializeFormat()                                                {}

        /** ****************************************************************************************
         * Virtual method to used to reset internal states. This method is invoked only with
         * the first \ref Acquire "acquisition" of a formatter. This way, reset of internal states
         * can be suppressed over a series of format calls by acquiring the formatter explicitly
         * prior to the series.
         *
         * As a sample, derived type
         * \alib{stringformat,FormatterPythonStyle,FormatterPythonStyle} clears its auto-tab
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
         * @param args          The objects to be used with formatters.
         * @param startArgument The first object in \p{args} to convert.
         *
         * @return The number of args consumed.
         ******************************************************************************************/
        virtual int         format( AString&         target,
                                    const String&    formatString,
                                    const Boxes&     args,
                                    int              startArgument )         = 0;

        /** ****************************************************************************************
         * The format loop implementation. Searches format strings in \p{args} and tests
         * if \c this or #Next is capable of processing it.
         *
         * @param target     An AString that takes the result.
         * @param args       The objects to be used with formatters.
         ******************************************************************************************/
        ALIB_API
        void                formatLoop( AString& target, const Boxes&  args );
};

}}} // namespace [aworx::lib::stringformat]


#endif // HPP_ALIB_STRINGFORMAT_FORMATTER
