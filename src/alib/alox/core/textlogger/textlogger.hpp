// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include ALox main header first...
#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

// then, set include guard
#ifndef HPP_ALOX_CORE_TEXTLOGGER_TEXTLOGGER
#define HPP_ALOX_CORE_TEXTLOGGER_TEXTLOGGER 1

// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALOX_CORE_LOGGER)
    #include "alib/alox/core/logger.hpp"
#endif

#if !defined (HPP_ALIB_TIME_CALENDAR)
    #include "alib/time/calendar.hpp"
#endif

#if !defined (HPP_ALIB_TIME_TICKSCONVERTER)
    #include "alib/time/ticksconverter.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_JAVASTYLE)
    #include "alib/strings/format/formatterjavastyle.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_PYTHONSTYLE)
    #include "alib/strings/format/formatterpythonstyle.hpp"
#endif

// #################################################################################################
// forward declarations
// #################################################################################################

namespace aworx { namespace lib { namespace lox { namespace core {
/** ************************************************************************************************
 * This namespaces defines class \b TextLogger and its helpers.
 **************************************************************************************************/
namespace textlogger {

/** ************************************************************************************************
 * This abstract class represents a plug-in for the TextLogger class which converts the list
 * of logables into a textual representation.
 * \see StandardConverter for further information.
 **************************************************************************************************/
class ObjectConverter
{
    public:
        /** ****************************************************************************************
         * Destructs an object of this class.
         ******************************************************************************************/
        virtual        ~ObjectConverter() {}

        /** ****************************************************************************************
         * The conversion method.
         * @param target     An AString that takes the result.
         * @param logables   The objects to convert.
         ******************************************************************************************/
        virtual void    ConvertObjects( AString& target, Boxes& logables )                       =0;

};

/** ************************************************************************************************
 * Implements the interface
 * \ref aworx::lib::lox::core::textlogger::ObjectConverter "ObjectConverter". Class
 * \ref aworx::lib::lox::core::textlogger::TextLogger "TextLogger" creates an instance of this type in
 * the moment no other (custom) type was set prior to the first log statement.
 *
 * This implementation uses
 * two specialisations of class
 * \alib{strings::format,FormatterBase,Formatter} to format the given logables to a textual
 * representation. The formatters (and their sequence!) are:
 *
 * 1. \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}
 * 2. \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}
 *
 * This way, standard text logging supports format strings in Python style as well as in Java style.
 **************************************************************************************************/
class StandardConverter : public ObjectConverter
{
    public:
        /// Formatter to process python style format strings. Used as the first (main) formatter.
        FormatterPythonStyle                  FormatterPS;

        /** Formatter to process Java style format strings. Attached to #FormatterPS as second
         *  format option.*/
        FormatterJavaStyle                    FormatterJS;

    protected:
        /// A counter to detect recursive calls.
        int                                   cntRecursion;

        /**
         * Formatters used with recursive calls log calls. If recursion occurs, the formatters
         * are created, respectively re-used from last time and their settings are reset to
         * those of the main formatters.
         */
        std::vector<FormatterPythonStyle*>    recursionFormatters;

    public:

        /** ****************************************************************************************
         * Constructor.
         ******************************************************************************************/
        ALIB_API
                        StandardConverter();

        /** ****************************************************************************************
         * Virtual destructor.
         ******************************************************************************************/
        ALIB_API
        virtual        ~StandardConverter();

        /** ****************************************************************************************
         * The conversion method.
         * Passes \p{target} and \p{logables} to #FormatterPS.
         * @param target     An AString that takes the result.
         * @param logables   The objects to convert.
         ******************************************************************************************/
        ALIB_API
        virtual void    ConvertObjects( AString& target, Boxes& logables );
};


/** ************************************************************************************************
 * This class is a sort of plug-in for the TextLogger class. Its purpose is to assemble the meta
 * information of each log line (things like timestamps, thread information, verbosity and domain, etc.).
 *
 * To manipulate the meta information log output, three options exist:
 * - by just changing the #Format string (at runtime).
 * - by creating a derived class that introduces a new version of method #processVariable() to handle
 *   one or more format variable differently or to introduce new variables.
 * - by creating a derived class that introduces a new version of method #Write() to
 *   completely control the meta information output.
 **************************************************************************************************/
class MetaInfo
{
    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         * The line format specifies the (automated) log output that is prepended to each log line before
         * the log message itself. This format string can be changed after the creation
         * of a \ref TextLogger.
         *
         * The string supports replacement variables that begin with a % sign
         * - %SP: The full path of the source file
         * - %Sp: The trimmed path of the source file
         * - %SF: The callers' source file name
         * - %Sf: The callers' source file name without extension
         * - %SL: The line number in the source file
         * - %SM: The method name
         *
         * - %TD: The date the log call was invoked
         * - %TT: Time of day the log call was invoked
         * - %TC: Time elapsed since the Logger was created or its timer was reset
         * - %TL: Time elapsed since the last log call
         *
         * - %tN: Thread name
         * - %tI: Thread ID
         * - %V:  The verbosity. This is replaced by the corresponding strings found in fields
         *        #VerbosityError, #VerbosityWarning, #VerbosityInfo and #VerbosityVerbose.
         * - %D:  Log domain
         * - %#:  The log call counter (like a line counter, but counting multi lines as one)
         * - %An: An auto-adjusted tabulator. This grows whenever it needs, but never shrinks. The
         *        optional integer number n specifies how much extra space is added when tab is adjusted.
         *        Setting this to a higher value avoids too many adjustments at the beginning of a log session.
         * - %LG: The name of the \e Logger. This might be useful if multiple loggers write to the same
         *        output stream (e.g. Console).
         * - %LX: The name of the \e Lox.
         * - %P:  The name of the process / application.
         *
         * Defaults to
         * \code "%Sp/%SF(%SL):%A5%SM() %A5[%TC +%TL][%tN]%V[%D]%A1(%#): " \endcode
         *
         * If debug logging as well as release logging has scope information disabled
         * (see \ref ALOX_DBG_LOG_CI_OFF and \ref ALOX_REL_LOG_CI_ON ), then it defaults to:
         * \code "[%TC +%TL][%tN]%V[%D]%A1(%#): " \endcode
         *
         * \note
         *   The use of <c>%%TD</c> and <c>%%TT</c> may become problematic and ambiguous if the
         *   system clock is changed during a software run. Especially for long-running background
         *   software (daemons, servers, etc.), the software that uses \alox needs to provide
         *   a strategy of synchronizing the clock with field #DateConverter.<br>
         *   For some explanation of the problem see details of namespace #aworx::lib::time.
         */
        #if ALOX_DBG_LOG_CI || ALOX_REL_LOG_CI
            AString Format { ASTR("%SF:%SL:%A3%SM %A3[%TC +%TL][%tN]%V[%D]%A1#%#: ")};
        #else
            AString Format { ASTR("[%TC +%TL][%tN]%V[%D]%A1#%#: ")};
        #endif

        /**
         *  If \c true, an one-time warning (using
         *  \ref aworx::lib::lang::Report::DoReport "Report::DoReport")
         *  will be issued if the format string is illegal.
         *
         *  \note This field can not be accessed directly (For technical reasons, it is
         *        differently declared than stated here in the source documentation).<br>
         *        To change the field use macros
         *        - \ref ALIB_WARN_ONCE_PER_INSTANCE_ENABLE(myinstance, FormatWarning) and
         *        - \ref ALIB_WARN_ONCE_PER_INSTANCE_DISABLE(myinstance, FormatWarning)
         *
         *        where \p{myinstance} is a reference to the object in question).
         */
        #if defined(DOX_PARSER)
            bool                              FormatWarning= true;
        #else
            ALIB_WARN_ONCE_PER_INSTANCE_DECL( FormatWarning, true )
        #endif


        /// The replacement for variable \c %%V in field #Format if \e Verbosity is \c Error
        String16  VerbosityError            = ASTR("[ERR]");

        /// The replacement for variable \c %%V in field #Format if \e Verbosity is \c Warning
        String16  VerbosityWarning          = ASTR("[WRN]");

        /// The replacement for variable \c %%V in field #Format if \e Verbosity is \c  Info
        String16  VerbosityInfo             = ASTR("     ");

        /// The replacement for variable \c %%V in field #Format if \e Verbosity is \c Verbose
        String16  VerbosityVerbose          = ASTR("[***]");

        /** Format string for the output of the log date. For more information, see
             "Standard Date and Time Format Strings" in .NET StringBuilder.AppendFormat() */
        String32    DateFormat              = ASTR("yyyy-MM-dd");

        /** Format string for the output of the time of day. For more information, see
             "Standard Date and Time Format Strings" in .NET StringBuilder.AppendFormat() */
        String16   TimeOfDayFormat          = ASTR("HH:mm:ss");

        /// The word "Days" the out put of time elapsed (if longer than a day).
        String16   TimeElapsedDays          = ASTR(" Days ");

        /// Minimum time difference to log in nanoseconds.  Below that #TimeDiffNone is written.
        int64_t    TimeDiffMinimum          =1000L;

        /// Output for time difference if below reasonable (measurable) minimum defined in #TimeDiffMinimum.
        String16   TimeDiffNone             = ASTR("---   ");

        /// Entity nanoseconds for time difference outputs below 1000 microsecond.
        String16   TimeDiffNanos            = ASTR(" ns");

        /// Entity microseconds for time difference outputs below 1000 microseconds.
        String16   TimeDiffMicros           = ASTR(" \u00B5s"); // greek 'm' letter

        /// Entity milliseconds for time difference outputs below 1000 milliseconds.
        String16   TimeDiffMillis           = ASTR(" ms");

        /// Format for time difference outputs between 10s and 99.9s.
        String16   TimeDiffSecs             = ASTR(" s");

        /// Format for time difference outputs between 100s and 60 min.
        String16   TimeDiffMins             = ASTR(" m");

        /// Format for time difference outputs between 1h and 24h.
        String16   TimeDiffHours            = ASTR(" h");

        /// Format for time difference outputs of more than a day.
        String16   TimeDiffDays             = ASTR(" days");

        /// Replacement string if no source info is available.
        NString16  NoSourceFileInfo         = "---";

        /// Replacement string if no source info is available.
        NString16  NoMethodInfo             = "---";

        /// The minimum digits to write for the log number (if used in format string).
        int        LogNumberMinDigits       = 3;

        /**
         * The maximum time elapsed. Used to determine the width of the output when writing
         * the elapsed time.
         *
         * This field will be read from the
         * configuration variable [ALOX_LOGGERNAME_MAX_ELAPSED_TIME](../group__GrpALoxConfigVars.html)
         * when the \b %TextLogger that this object belongs to is attached to a \b %Lox
         * and written back on removal.
         */
        Ticks::Duration MaxElapsedTime;


        /**
         * This field is used to convert the steady and monotonic clock values provided with
         * \alox{core,ScopeInfo::GetTimeStamp} into human readable, calendrical values in the
         * moment, field #Format contains tokens <c>%%TD</c> and/or <c>%%TT</c>.
         *
         * This may become problematic and ambiguous if the system clock is changed during a
         * software run. Especially for long-running background software (daemons, servers, etc.),
         * the software that uses \alox needs to provide a strategy of synchronizing this converter
         * with the system clock.
         *
         * A simple strategy is to just periodically invoke \alib{time,TicksConverter::SyncClocks},
         * for example once a second.
         *
         * For some explanation of the problem see details of namespace #aworx::lib::time.
         */
        TicksConverter  DateConverter;

    // #############################################################################################
    // Internal fields
    // #############################################################################################
    protected:

        /**  A singleton calendar time object shared between different format variables during one
         *   invocation. */
        lib::time::CalendarDateTime callerDateTime;

    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructor.
         ******************************************************************************************/
                 MetaInfo() : MaxElapsedTime()      {}

        /** ****************************************************************************************
         * Virtual destructor.
         ******************************************************************************************/
        virtual ~MetaInfo()                         {}

    // #############################################################################################
    // Public interface
    // #############################################################################################
    public:

        /** ****************************************************************************************
         *  Parses the #Format string and logs meta information into the log buffer. For each
         *  variable found, method #processVariable is invoked. Hence, to add new variables,
         *  the latter method can be overwritten by descendants. Overwriting this method is
         *  recommended for formatter classes that do not rely on format strings.
         * @param logger    The logger that we are embedded in.
         * @param buffer    The buffer to write meta information into.
         * @param domain    The <em>Log Domain</em>.
         * @param verbosity The verbosity.
         * @param scope     Information about the scope of the <em>Log Statement</em>..
         *
         * @return The number of tab sequences that were written (by adding ESC::TAB to the buffer).
         ******************************************************************************************/
        ALIB_API
        virtual int Write( TextLogger&        logger,
                           AString&           buffer,
                           core::Domain&      domain,
                           Verbosity          verbosity,
                           ScopeInfo&         scope     );

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:

        /** ****************************************************************************************
         * Processes the next command found in the format string, by writing formatted information
         * into the given buffer.
         * The given
         * \alib{strings,SubstringBase,Substring} holds the next command.
         * When method the returns, the command is cut from the front.
         *
         * @param logger       The logger that we are embedded in.
         * @param domain       The <em>Log Domain</em>.
         * @param verbosity    The verbosity. This has been checked to be active already on this
         *                     stage and is provided to be able to be logged out only.
         * @param scope        Information about the scope of the <em>Log Statement</em>..
         * @param dest         The buffer to write meta information into.
         * @param variable     The variable to read (may have more characters appended)
         *
         * @return The number of tab sequences that were written (by adding ESC::TAB to the buffer).
         ******************************************************************************************/
        ALIB_API
        virtual int processVariable( TextLogger&      logger,
                                     core::Domain&    domain,
                                     Verbosity        verbosity,
                                     ScopeInfo&       scope,
                                     AString&         dest,
                                     Substring&       variable      );

        /** ****************************************************************************************
         *  Helper function that logs a time given difference into the given buffer in a human readable
         *  format. Works from nanoseconds seconds to days.
         *
         * @param buffer       The buffer to write the time difference representation into.
         * @param diffNanos    The time difference to write in nanoseconds.
         ******************************************************************************************/
        ALIB_API
        virtual void writeTimeDiff( AString& buffer, int64_t diffNanos );
}; // class



/** ************************************************************************************************
 *  This class is a still abstract implementation of class Logger which is used as the super class
 *  for all textual Logger implementations within \alox, e.g. ConsoleLogger.
 *
 *  The class uses a helper class to generate the textual representation of the meta information
 *  of a log call.
 *  This helper can be extended and replaced in #MetaInfo to modify the behavior of TextLogger.
 *
 *  The final log message is then passed to the abstract method #logText.
 *  Hence, custom Logger classes that inherited from this class instead of directly from class
 *  #Logger, need to implement #logText instead of implementing #Log.
 *
 *  Class TextLogger supports multi line log outputs. Such multi line log outputs can be configured to
 *  be logged in different ways. See #MultiLineMsgMode for more information.
 **************************************************************************************************/
class TextLogger : public Logger
{

    // #############################################################################################
    // Internal fields
    // #############################################################################################
    protected:

        /// The internal log Buffer.
        AString                 logBuf;

        /// The buffers for converting the logables.
        AString                 msgBuf;

        /// Denotes whether this logger writes to the <em>standard output streams</em>. If so,
        /// \alib{ALib,StdOutputStreamsLock} is acquired on logging.
        bool                    usesStdStreams;

        /** Used to avoid to repeatedly register with \alib <em>standard output stream</em> lockers
            when attached to multiple instances of class \b Lox.   */
        int                     stdStreamLockRegistrationCounter                                 =0;

        /**
         * A list of pairs of strings. Within each log message, the first string of a pair is
         *  searched and replaced by the second string. Very simple, but useful in some cases.
         */
        std::vector<AString>    replacements;

    public: // public only to allow being resourced
        /// Denotes the use of light vs dark colors. Used with descendant classes only.
        enum class LightColorUsage
        {
            Auto,            ///< Automatic
            Never,           ///< Never use light colors
            Foreground,      ///< Use light colors for foreground
            Background       ///< Use light colors for background
        };

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         * A helper object to get textual representation of logable objects.
         * If no converter is set when this logger is used, a converter of type
         * \ref aworx::lib::lox::core::textlogger::StandardConverter "StandardConverter" is created and used.
         * In the destructor of this class, the current object converter will be deleted.
         *
         * To extend class \b %TextLogger to support logging custom objects, custom converters can
         * set. The preferred alternative is however, to make custom types be formattable
         * by formatter classes used with \b %StandardConverter.
         */
        ObjectConverter*                Converter                                        = nullptr;

        /**
         * A helper object to format log objects into textual representations. This class incorporates
         * a format string that defines the meta information in the log output. Furthermore, to extend
         * TextLogger, this object can be replaced by custom implementations of it.
         */
        textlogger::MetaInfo*           MetaInfo;

        /**
         * Characters written after each <em>Log Statement</em>.
         * This may be used for example to reset colors and styles.
         * Note, that with multi-line <em>Log Statements</em>, the contents of this field is \b not
         * written at the end of each line, but only at the end of the last line.
         * To define characters that are written after each line of a multi-line
         * <em>Log Statement</em>, field #FmtMultiLineSuffix.
         *
         * Defaults to empty string.
         */
        String16                        FmtMsgSuffix;

        /**
         * Holds a list of values for auto tab positions and field sizes.
         * For each requested value, a corresponding array field is created on the fly.
         * If the format string get's changed and different (new) auto values should be used, then
         * this field should be reset after setting the new format string.
         * The other way round, it is also possible to preset set minimum values for tabs and field
         * sizes and hence avoid the columns growing during the lifetime of the Logger.
         *
         * This field will be read from the
         * configuration variable [ALOX_LOGGERNAME_AUTO_SIZES](../group__GrpALoxConfigVars.html)
         * when the \b %TextLogger that we belong to is attached to a \b %Lox and written back
         * on removal.
         */
        lib::strings::util::AutoSizes   AutoSizes;

        /**
         * Determines if multi line messages should be split into different log lines. Possible
         * values are:
         *
         * - 0: No line split is performed, delimiters can be replaced by readable delimiters
         *   (depending on setting of #MultiLineDelimiter and # MultiLineDelimiterRepl).
         *
         * - 1: Each log line contains all meta information
         *
         * - 2: Starting with second log line, meta information is replaced by blanks  (default)
         *
         * - 3: The headline #FmtMultiLineMsgHeadline is logged and all lines of the multi line
         *      text are logged at position zero (without further meta information)
         *
         * - 4: Just the multi line text is logged, starting at column zero (no meta information
         *   is logged)
         *
         * \note In modes 3 and 4, no ESC::EOMETA is included at the beginning of the message.
         *       Loggers that need such info, have to implement method #notifyMultiLineOp.
         */
        int                             MultiLineMsgMode                                         =2;

        /**
         * This is the string interpreted as line delimiter within log messages. If \e nulled, then
         * <c>'\\n'</c>, <c>'\\r'</c> or <c>'\\r\\n'</c> is recognized.<br>
         * Important: Can be set to an empty string, to stop any multi line processing of
         * \b %TextLogger, even the replacements of the delimiter character.
         */
        AString                         MultiLineDelimiter;

        /**
         * This is the readable (!) separator string, for logging out multi line messages into a
         * single line (#MultiLineMsgMode==0).
         * Defaults to "\\r".
         */
        String16                        MultiLineDelimiterRepl                        = ASTR("\\r");

        /**
         * Headline for multi line messages (depending on #MultiLineMsgMode)  .
         * Defaults to "ALox: Multi line message follows: "
         */
        String64                        FmtMultiLineMsgHeadline= ASTR("ALox: Multi line message follows: ");

        /**
         * Prefix for multi line messages. This is also used if multi line messages logging is
         * switched off (MultiLineMsgMode == 0) but replacing of a set MultiLineDelimiter takes
         * place.
         * Defaults to ">> ".
         */
        String16                        FmtMultiLinePrefix                            = ASTR(">> ");

        /**
         * Suffix for multi line messages. This is also used if multi line messages logging is
         * switched off (MultiLineMsgMode == 0) and replacing of a set #MultiLineDelimiter
         * takes place.<br>
         * Note that at the end of the last line, in addition #FmtMsgSuffix is added.<br>
         * Defaults to "".
         */
        String16                        FmtMultiLineSuffix                               = ASTR("");

    // #############################################################################################
    // protected Constructor/ public destructor
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Constructs a TextLogger.
         * Reads the format variable for the meta information from the configuration. The
         * variable name is created from the \e Logger name and the suffix <c>'_FORMAT'</c>
         * @param pName            The name of the \e Logger.
         * @param typeName        The type of the \e Logger.
         * @param pUsesStdStreams  Denotes whether this logger writes to the
         *                        <em>standard output streams</em>.
         ******************************************************************************************/
        ALIB_API explicit TextLogger( const NString& pName, const NString& typeName,
                                      bool  pUsesStdStreams );

        /** ****************************************************************************************
         *  Destructs a TextLogger.
         ******************************************************************************************/
    public:
        ALIB_API virtual ~TextLogger()                                                     override;

    // #############################################################################################
    // Reimplementing interface of grand-parent class SmartLock
    // #############################################################################################

        /** ****************************************************************************************
         * Invokes grand-parent's method and in addition, if field #usesStdStreams
         * is set, registers with
         * \ref aworx::lib::ALib::StdOutputStreamsLock "ALib::StdOutputStreamsLock".
         *
         * Furthermore, configuration variables are read within this method and created with
         * default values, in the case they do not exist, yet. The variables read and optionally
         * created are:
         * - \b ALox::AUTO_SIZES
         * - \b ALox::FORMAT
         * - \b ALox::FORMAT_DATE_TIME
         * - \b ALox::FORMAT_MULTILINE
         * - \b ALox::FORMAT_TIME_DIFF
         * - \b ALox::MAX_ELAPSED_TIME
         * - \b ALox::REPLACEMENTS
         *
         * Configuration variables are
         * [documented here](../group__GrpALoxConfigVars.html).
         *
         * @param newAcquirer The acquirer to add.
         * @return The new number of \e acquirers set.
         ******************************************************************************************/
        ALIB_API
        virtual int   AddAcquirer( ThreadLock* newAcquirer )                               override;

        /** ****************************************************************************************
         * Invokes grand-parent's method and in addition, de-registers with
         * \ref aworx::lib::ALib::StdOutputStreamsLock "ALib::StdOutputStreamsLock".
         * @param acquirer The acquirer to remove.
         * @return The new number of \e acquirers set.
         ******************************************************************************************/
        ALIB_API
        virtual int   RemoveAcquirer( ThreadLock* acquirer )                               override;


    // #############################################################################################
    // Abstract methods introduced
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         *  This abstract method introduced by this class "replaces" the abstract method #Log
         *  of parent class Logger which this class implements. In other words, descendants of this
         *  class need to override this method instead of \b %Log. This class %TextLogger is
         *  responsible for generating meta information, doing text replacements, handle multi-line
         *  messages, etc. and provides the textual representation of the whole log contents
         *  to descendants using this method.
         *
         * @param domain     The <em>Log Domain</em>.
         * @param verbosity  The verbosity. This has been checked to be active already on this
         *                   stage and is provided to be able to be logged out only.
         * @param msg        The log message.
         * @param scope      Information about the scope of the <em>Log Statement</em>.
         * @param lineNumber The line number of a multi-line message, starting with 0.
         *                   For single line messages this is -1.
         ******************************************************************************************/
        virtual void logText(  core::Domain&      domain,
                               Verbosity          verbosity,
                               AString&           msg,
                               core::ScopeInfo&   scope,
                               int                lineNumber)                                    =0;

        /** ****************************************************************************************
         * Abstract method to be implemented by descendants. This message is called only when
         * multi-line messages are logged. It is called exactly once before a series of #logText
         * calls of a multi-line message and exactly once after such series.<br>
         * This is useful if the writing of text includes the acquisition of system resources
         * (e.g. opening a file).
         *
         * @param phase  Indicates the beginning or end of a multi-line operation.
         ******************************************************************************************/
        virtual void notifyMultiLineOp (lib::lang::Phase phase)                                  =0;

    // #############################################################################################
    // Abstract method implementations
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * This is the implementation of the abstract method inherited from class Logger
         * that executes a log.<br>
         * This class implements this method and but exposes the new abstract method #logText.
         * This mechanism allows this class to do various things that are standard to Loggers
         * of type TextLogger. For example, meta information of the log invocation is formatted
         * and string replacements are performed. This way, descendants of this class will consume
         * a ready to use log buffer with all meta information and contents of all objects to be
         * included and their primary obligation is to copy the content into a corresponding
         * output stream.
         *
         * @param domain    The <em>Log Domain</em>.
         * @param verbosity The verbosity.
         * @param logables  The list of objects to log.
         * @param scope     Information about the scope of the <em>Log Statement</em>..
         ******************************************************************************************/
        ALIB_API
        virtual void Log( Domain& domain, Verbosity verbosity, Boxes& logables,
                          ScopeInfo& scope)                                                override;

    // #############################################################################################
    // Public interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Adds the given pair of replacement strings. If searched string already exists, the
         * current replacement string gets replaced. If the replacement string is \c nullptr,
         * nothing is set and a previously set replacement definition becomes unset.
         * @param searched    The string to be searched.
         * @param replacement The replacement string. If this equals 'nullptr' a previously set
         *                    replacement will be unset.
        ******************************************************************************************/
        ALIB_API
        virtual void  SetReplacement( const String& searched, const String& replacement );

        /** ****************************************************************************************
         *  Removes all pairs of searched strings and their replacement value.
         ******************************************************************************************/
        ALIB_API
        virtual void  ClearReplacements();

}; // class TextLogger

}}}}// namespace aworx[::lib::lox::core::textlogger]

/// Type alias in namespace #aworx.
using     TextLogger=       aworx::lib::lox::core::textlogger::TextLogger;

}  // namespace aworx

ALIB_LANG_ENUM_PARSABLE( aworx::TextLogger::LightColorUsage )
ALIB_LANG_RESOURCED(     aworx::TextLogger::LightColorUsage, aworx::lib::ALOX, ASTR("LightColorUsage") )

#endif // HPP_ALOX_CORE_TEXTLOGGER_TEXTLOGGER
