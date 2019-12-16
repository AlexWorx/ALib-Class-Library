/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_text of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_TEXT_FORMATTER
#define HPP_ALIB_TEXT_FORMATTER 1

#if !defined(HPP_ALIB_TEXT_FWDS)
#   include "alib/text/fwds.hpp"
#endif

ALIB_ASSERT_MODULE(TEXT)

#if ALIB_THREADS
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

#if ALIB_THREADS
    #if !defined (HPP_ALIB_THREADS_THREADLOCK)
        #include "alib/threads/threadlock.hpp"
    #endif
#else
    #if !defined (HPP_ALIB_FS_OWNER_OWNER)
        #include "alib/lib/fs_owner/owner.hpp"
    #endif
#endif

namespace aworx { namespace lib { namespace text {

/** ************************************************************************************************
 * This is an abstract base class to implement an \alib string formatter. A string formatter
 * uses a "format string" to transform arguments into formatted text. The format string defines
 * how the arguments are transformed by offering a "placeholder syntax".
 *
 * With this information, it becomes clear that different formatter types (derived types that offer
 * a concrete placeholder syntax) all can have the same interface methods.
 * This class defines this abstract interface.
 *
 * Built-in formatters, provided by module \alib_text and derived from this class, are
 * \alib{text,FormatterPythonStyle,FormatterPythonStyle} and
 * \alib{text,FormatterJavaStyle,FormatterJavaStyle}.
 *
 * ## Friends ##
 * class \alib{text,Text}.
 **************************************************************************************************/
class Formatter
#if ALIB_THREADS
   : public threads::ThreadLock
#endif
{
    #if !defined(ALIB_DOX)
        friend class  aworx::lib::text::Text;
    #endif

    ALIB_IFN_THREADS( int cntAcquirements = 0; )

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
         * Stores default attributes for formatting numbers.
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
         * - Lower case versions of prefix literals that indicate the base of integral values:
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
         *    Also, object life-cycle management is completely up to the user.
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
         *
         * This method checks if this was the first acquisition and if so, invokes method
         * #reset. Therefore, a series of formatting calls may be performed without resetting
         * the internal state, by acquiring the object once prior to the series of invocations
         * and releasing it afterwards.
         *
         * Furthermore, this method invokes itself on potentially attached formatter #Next
         * (recursion).
         *
         * This method is inherently called by convenience method #Format that accepts variadic
         * arguments and hence no explicit invocation is needed in this case.
         *
         * Acquiring this formatter is mandatory with overloaded methods #FormatArgs.
         *
         * Multiple (nested) acquirements by the same execution thread are allowed.
         * A corresponding amount of invocations to #Release have to be made.
         *
         * \note
         *   In the debug-compilation of an application, this method accepts the parameters,
         *   providing information about the caller. In the release version these parameters do not
         *   exist. Therefore use macro #ALIB_CALLER_PRUNED to provide the parameters:
         *
         *          sample.Acquire( ALIB_CALLER_PRUNED );
         *
         * @param dbgFile  Caller information. Available only with debug builds.
         * @param dbgLine  Caller information. Available only with debug builds.
         * @param dbgFunc  Caller information. Available only with debug builds.
         * @return An internally allocated container of boxes that may be used to collect
         *         formatter arguments.
         ******************************************************************************************/
         #if ALIB_DEBUG
         ALIB_API Boxes&    Acquire( const NCString& dbgFile, int dbgLine, const NCString& dbgFunc );
         #else
         ALIB_API Boxes&    Acquire();
         #endif

        /** ****************************************************************************************
         * Overrides (non-virtual) \alib{threads,ThreadLock::Release}, which is called.
         * In addition, the method is called on potentially attached formatter #Next (recursion).
         ******************************************************************************************/
         ALIB_API void      Release();

#if !ALIB_THREADS || defined(ALIB_DOX)
        /** ****************************************************************************************
         * Returns the number of acquirements of this formatter.
         *
         * \note
         *   Usually this method is inherited from base clas \alib{threads,ThreadLock} and
         *   therefore not defined. However, in the absence of module \alib_threads in an
         *   \alibdist, this method is re-implemented with this type.
         *
         * @return The number of (recursive) acquirements.
         ******************************************************************************************/
        int           CountAcquirements()                                                      const
        {
            return cntAcquirements;
        }
#endif //!ALIB_THREADS

    // #############################################################################################
    //  Interface
    // #############################################################################################

        /** ****************************************************************************************
         * Variadic template method that accepts a target \b %AString and a list of arguments.
         *
         * This is a convenience method to allow single-line format invocations. No calls to
         * methods #Acquire and #Release are needed to be performed.
         *
         * \note
         *   This method uses <c>static_assert</c> to disallow the invocation with one
         *   variadic argument of type \alib{boxing,Boxes} or a derived type.
         *   This is to ensure that for these box containers, the more efficient method #FormatArgs
         *   is used.
         *
         * @tparam TArgs   Variadic template type list.
         * @param target   An AString that takes the result.
         * @param args     The variadic list of arguments to be used with formatters.
         * @return A reference to this formatter to allow concatenated operations.
         ******************************************************************************************/
        template <typename... TArgs>
        Formatter&          Format( AString& target, TArgs&&... args )
        {
            // assert that this method is not used with with Boxes containers.
            // Those are to be processed with FormatArgs
            constexpr  bool Argument_has_type_Boxes=
                  (sizeof...(TArgs) == 1)
               && ATMP_ISOF( ATMP_RCVR( std::tuple_element<0 ALIB_COMMA std::tuple<TArgs...>> ),
                             Boxes);
            static_assert( !Argument_has_type_Boxes,
                           "To pass a container of type Boxes to a formatter, use method FormatArgs." );
            ALIB_LOCK

            // create argument objects using implicit constructor invocation
            boxes.clear();
            boxes.Add( std::forward<TArgs>( args )... );

            // invoke format
            formatLoop( target, boxes );
            return *this;
        }

        /** ****************************************************************************************
         * Formats the internal list of arguments that iis returned when acquiring access to this
         * formatter with #Acquire.
         *
         * This method may be more efficient than using inlined variadic method \b Format
         * and should be preferred if:
         * - Format arguments can not be collected in a single invocation, for example if those
         *   are to be collected in a loop.
         * - Multiple format operations (invocations of this method) are to be performed in a row.
         *   In this case, only a single call to #Acquire and #Release is to be performed.
         *
         * @param target     An AString that takes the result.
         * @return A reference to this formatter to allow concatenated operations.
         *******************************************************************************************/
        ALIB_API
        Formatter&          FormatArgs( AString& target )
        {
            ALIB_IF_THREADS( ALIB_ASSERT_ERROR( CountAcquirements(), "Formatter not acquired." ) )
            return formatLoop( target, boxes );
        }


        /** ****************************************************************************************
         * Same as Format(AString&) but allows to specify an external list of arguments instead of
         * the internally allocated object, which is returned by method #Acquire.
         *
         * @param args       The arguments to be used with formatters.
         * @param target     An AString that takes the result.
         * @return A reference to this formatter to allow concatenated operations.
         *******************************************************************************************/
        Formatter&          FormatArgs( AString& target, const Boxes& args )
        {
            ALIB_IF_THREADS( ALIB_ASSERT_ERROR( CountAcquirements(), "Formatter not acquired." ) )
            return formatLoop( target, args );
        }


        /** ****************************************************************************************
         * Clones and returns a copy of this formatter.
         *
         * If a formatter is attached to field \alib{text,Formatter::Next}, it is
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
    // Static Interface methods (default formatter)
    // #############################################################################################
    protected:
        /**
         * A static singleton instance which is received with #GetDefault.
         */
        ALIB_API
        static SPFormatter     defaultFormatter;

    public:
        /** ****************************************************************************************
         * Returns the default formatter object. With the module initialization (method
         * \alib{text,Text::bootstrap}), an object of type
         * \alib{text,FormatterPythonStyle,FormatterPythonStyle} is created and a second
         *
         * one of type \alib{text,FormatterJavaStyle,FormatterJavaStyle} is appended.
         * Those formatters will be deleted with \alib{text,Text::shutdown},
         * respectively when a different default formatter is set (see #ReplaceDefault)
         * and no other shared pointer of the original one exists.
         *
         * @return A shared pointer containing the default formatter.
         ******************************************************************************************/
        static
        SPFormatter         GetDefault()
        {
            return defaultFormatter;
        }

        /** ****************************************************************************************
         * Same as #GetDefault, also acquires the formatter before it is returned.
         * the reference.
         *
         * @param dbgFile  Caller information. Available only with debug builds.
         * @param dbgLine  Caller information. Available only with debug builds.
         * @param dbgFunc  Caller information. Available only with debug builds.
         * @return A shared pointer containing the default formatter, already acquired.
         ******************************************************************************************/
        #if ALIB_DEBUG
            static
            SPFormatter     AcquireDefault( const NCString& dbgFile,
                                            int             dbgLine,
                                            const NCString& dbgFunc  )
            {
                defaultFormatter->Acquire( dbgFile, dbgLine, dbgFunc );
                return defaultFormatter;
            }
        #else
            static
            SPFormatter     AcquireDefault()
            {
                defaultFormatter->Acquire();
                return defaultFormatter;
            }
        #endif

        /** ****************************************************************************************
         * Replaces the formatter currently defined as the default formatter.
         *
         * \see
         *   Static methods #GetDefault and #AcquireDefault.
         * @param newFormatter The replacement formatter.
         ******************************************************************************************/
        ALIB_API
        void                ReplaceDefault( Formatter* newFormatter );

    // #############################################################################################
    //  Protected methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Virtual method which is invoked with each invocation of #Format.
         * The default implementation does nothing.
         ******************************************************************************************/
        virtual void        initializeFormat()                                                    {}

        /** ****************************************************************************************
         * Virtual method to used to reset internal states. This method is invoked only with
         * the first \ref Acquire "acquisition" of a formatter. This way, reset of internal states
         * can be suppressed over a series of format calls by acquiring the formatter explicitly
         * prior to the series.
         *
         * As a sample, derived type
         * \alib{text,FormatterPythonStyle,FormatterPythonStyle} clears its auto-tab
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
         * @return A reference to this formatter to allow concatenated operations.
         ******************************************************************************************/
        ALIB_API
            Formatter&      formatLoop( AString& target, const Boxes& args );
};

}} // namespace aworx[::lib::text]


} // namespace [aworx]


#endif // HPP_ALIB_TEXT_FORMATTER
