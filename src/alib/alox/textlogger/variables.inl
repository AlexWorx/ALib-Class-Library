//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::lox::textlogger {

//==================================================================================================
/// Definition of the main log-line format string for class \alib{lox;textlogger::TextLogger}
/// and its derivates.
/// This struct is used to define an external \ref alib_mod_variables "configuration variable"
/// named \ref alxcvALOX_LOGGERNAME_FORMAT.
/// This variable can be accessed programatically
/// with \alib{lox::textlogger;TextLogger::GetFormatMetaInfo} or by accessing the variable's value
/// through the configuration object found in camp singleton #alib::ALOX.
//==================================================================================================
struct FormatMetaInfo
{
    /// The line format specifies the (automated) log output that is prepended to each log line before
    /// the log message itself. This format string can be changed after the creation
    /// of a \ref TextLogger.
    ///
    /// The string supports replacement variables that begin with a <c>\%</c> character:
    /// - \%SP: The full path of the source file
    /// - \%Sp: The trimmed path of the source file
    /// - \%SF: The caller's source file name
    /// - \%Sf: The caller's source file name without extension
    /// - \%SL: The line number in the source file
    /// - \%SM: The method name
    ///
    /// - \%TD: The date the log call was invoked
    /// - \%TT: Time of day the log call was invoked
    /// - \%TC: Time elapsed since the Logger was created or its timer was reset
    /// - \%TL: Time elapsed since the last log call
    ///
    /// - \%tN: Thread name
    /// - \%tI: Thread ID
    /// - \%V:  The verbosity. This is replaced by the corresponding strings found in fields
    ///        #VerbosityError, #VerbosityWarning, #VerbosityInfo and #VerbosityVerbose.
    /// - \%D:  Log domain
    /// - \%#:  The log call counter (like a line counter, but counting multi lines as one)
    /// - \%An: An auto-adjusted tabulator. This grows whenever it needs, but never shrinks. The
    ///        optional integer number n specifies how much extra space is added when tab is adjusted.
    ///        Setting this to a higher value avoids too many adjustments at the beginning of a log session.
    /// - \%LG: The name of the \e Logger. This might be useful if multiple loggers write to the same
    ///        output stream (e.g., Console).
    /// - \%LX: The name of the \e Lox.
    /// - \%P:  The name of the process / application.
    ///
    /// Resourced default is
    /// \code "%Sp/%SF(%SL):%A5%SM() %A5[%TC +%TL][%tN]%V[%D]%A1(%#): " \endcode
    ///
    /// If debug logging as well as release logging has scope information disabled
    /// (see \ref ALOX_DBG_LOG_CI and \ref ALOX_REL_LOG_CI), then it defaults to:
    /// <c>"[%TC +%TL][%tN]%V[%D]%A1(%#): "</c>
    ///
    /// \note
    ///   The use of <c>%%TD</c> and <c>%%TT</c> may become problematic and ambiguous if the
    ///   system clock is changed during software run. Especially for long-running background
    ///   software (daemons, servers, etc.), the software that uses \alox needs to provide
    ///   a strategy of synchronizing the clock with field
    ///   \alib{lox::textlogger;TextLogger::DateConverter}.<br>
    ///   For some explanation of the problem see details of namespace #alib::time.
    AStringPA  Format;

    /// The replacement for variable \c %%V in field #Format if \e Verbosity is \c Error.
    /// Resourced as '[ERR]'.
    AStringPA  VerbosityError;

    /// The replacement for variable \c %%V in field #Format if \e Verbosity is \c Warning.
    /// Resourced as '[WRN]'.
    AStringPA  VerbosityWarning;

    /// The replacement for variable \c %%V in field #Format if \e Verbosity is \c  Info.
    /// Resourced as '     '.
    AStringPA  VerbosityInfo;

    /// The replacement for variable \c %%V in field #Format if \e Verbosity is \c Verbose.
    /// Resourced as to '[***]'.
    AStringPA  VerbosityVerbose;

    /// Characters written after each <em>Log Statement</em>.
    /// This may be used for example to reset colors and styles.
    /// Note, that with multi-line <em>Log Statements</em>, the contents of this field is \b not
    /// written at the end of each line, but only at the end of the last line.
    /// To define characters that are written after each line of a multi-line
    /// <em>Log Statement</em>, set field #MsgSuffix.
    ///
    /// Defaults to empty string.
    AStringPA   MsgSuffix;


    /// Constructor taking an object pool which is passed to each string member.
    /// @param pool The pool object of the \alib{variables;Configuration}.
    FormatMetaInfo(PoolAllocator& pool)
    : Format           (pool)
    , VerbosityError   (pool)
    , VerbosityWarning (pool)
    , VerbosityInfo    (pool)
    , VerbosityVerbose (pool)
    , MsgSuffix (pool)                                                                     {}
}; // struct FormatMetaInfo

//==================================================================================================
/// Definition of date and time formats for class \alib{lox;textlogger::TextLogger}
/// and its derivates.
/// This struct is used to define an external \ref alib_mod_variables "configuration variable"
/// named \ref alxcvALOX_LOGGERNAME_FORMAT_DATE_TIME.
/// This variable can be accessed programatically
/// with \alib{lox::textlogger;TextLogger::GetFormatDate} or by accessing the variable's value
/// through the configuration object found in camp singleton #alib::ALOX.
//==================================================================================================
struct FormatDateTime
{
    /// Format string for the output of the log date. For more information, see
    /// "Standard Date and Time Format Strings" in .NET StringBuilder.AppendFormat().
    AStringPA    Date;

    /// Format string for the output of the time of day. For more information, see
    /// "Standard Date and Time Format Strings" in .NET StringBuilder.AppendFormat().
    AStringPA   TimeOfDay;

    /// The word "Days" the out put of time elapsed (if longer than a day).
    AStringPA   ElapsedDays;

    /// Constructor taking an object pool which is passed to each string member.
    /// @param pool The pool object of the \alib{variables;Configuration}.
    FormatDateTime(PoolAllocator& pool)
    : Date       (pool)
    , TimeOfDay  (pool)
    , ElapsedDays(pool)                                                                           {}
}; // struct DateFormat

//==================================================================================================
/// Definition of the formats of time differences for class
/// \alib{lox;textlogger::TextLogger} and its derivates.
/// This struct is used to define an external \ref alib_mod_variables "configuration variable"
/// named \ref alxcvALOX_LOGGERNAME_FORMAT_TIME_DIFF.
/// This variable can be accessed programatically
/// with \alib{lox::textlogger;TextLogger::GetFormatTimeDiff} or by accessing the variable's value
/// through the configuration object found in camp singleton #alib::ALOX.
//==================================================================================================
struct FormatTimeDiff
{
    /// Minimum time difference to log in nanoseconds.  Below that #None is written.
    int64_t    Minimum;

    /// Output for time difference if below reasonable (measurable) minimum defined in #Minimum.
    AStringPA   None;

    /// Entity nanoseconds for time difference outputs below 1000 microsecond.
    AStringPA   Nanos;

    /// Entity microseconds for time difference outputs below 1000 microseconds.
    AStringPA   Micros;

    /// Entity milliseconds for time difference outputs below 1000 milliseconds.
    AStringPA   Millis;

    /// Format for time difference outputs between 10s and 99.9s.
    AStringPA   Secs;

    /// Format for time difference outputs between 100s and 60 min.
    AStringPA   Mins;

    /// Format for time difference outputs between 1h and 24h.
    AStringPA   Hours;

    /// Format for time difference outputs of more than a day.
    AStringPA   Days;

    /// Constructor taking an object pool which is passed to each string member.
    /// @param pool The pool object of the \alib{variables;Configuration}.
    FormatTimeDiff(PoolAllocator& pool)
    : None  (pool)
    , Nanos (pool)
    , Micros(pool)
    , Millis(pool)
    , Secs  (pool)
    , Mins  (pool)
    , Hours (pool)
    , Days  (pool)                                                                                {}
};

//==================================================================================================
/// Definition of the multi-line message mode for class \alib{lox;textlogger::TextLogger}
/// and its derivates.
/// This struct is used to define an external \ref alib_mod_variables "configuration variable"
/// named \ref alxcvALOX_LOGGERNAME_FORMAT_MULTILINE.
/// This variable can be accessed programatically
/// with \alib{lox::textlogger;TextLogger::GetFormatMultiLine} or by accessing the variable's value
/// through the configuration object found in camp singleton #alib::ALOX.
//==================================================================================================
struct FormatMultiLine
{
    /// Determines if multi line messages should be split into different log lines. Possible
    /// values are:
    ///
    /// - 0: No line split is performed, delimiters can be replaced by readable delimiters
    ///   (depending on setting of #Delimiter and #DelimiterReplacement).
    ///
    /// - 1: Each log line contains all meta-information
    ///
    /// - 2: Starting with second log line, meta-information is replaced by blanks  (default)
    ///
    /// - 3: The headline #Headline is logged and all lines of the multi line
    ///      text are logged at position zero (without further meta-information)
    ///
    /// - 4: Just the multi line text is logged, starting at column zero (no meta-information
    ///   is logged)
    ///
    /// \note In modes 3 and 4, no ESC::EOMETA is included at the beginning of the message.
    ///       Loggers that need such info, have to implement method
    ///        \alib{lox::textlogger;TextLogger::notifyMultiLineOp}.
    int               Mode;

    /// This is the string interpreted as line delimiter within log messages. If \e nulled, then
    /// <c>'\\n'</c>, <c>'\\r'</c> or <c>'\\r\\n'</c> is recognized.<br>
    /// Important: Can be set to an empty string, to stop any multi line processing of
    /// \b %TextLogger, even the replacements of the delimiter character.
    AStringPA        Delimiter;

    /// Headline for multi line messages (depending on #Mode)  .
    /// Defaults to "ALox: Multi line message follows: "
    AStringPA        Headline;

    /// Prefix for multi line messages. This is also used if multi line messages logging is
    /// switched off (MultiLineMsgMode == 0) but replacing of a set MultiLineDelimiter takes
    /// place.
    /// Defaults to ">> ".
    AStringPA        Prefix;

    /// Suffix for multi line messages. This is also used if multi line messages logging is
    /// switched off (MultiLineMsgMode == 0) and replacing of a set #Delimiter
    /// takes place.<br>
    /// Note that at the end of the last line, in addition #Suffix is added.<br>
    /// Defaults to "".
    AStringPA        Suffix;

    /// This is the readable (!) separator string, for logging out multi line messages into a
    /// single line (#Mode==0).
    /// Defaults to "\\r".
    AStringPA        DelimiterReplacement;

    /// Constructor taking an object pool which is passed to each string member.
    /// @param pool The pool object of the \alib{variables;Configuration}.
    FormatMultiLine(PoolAllocator& pool)
    : Delimiter    (pool)
    , Headline     (pool)
    , Prefix       (pool)
    , Suffix       (pool)
    , DelimiterReplacement(pool)                                                                         {}

};

//==================================================================================================
/// Definition of further format options for class \alib{lox;textlogger::TextLogger}
/// and its derivates.
/// This struct is used to define an external \ref alib_mod_variables "configuration variable"
/// named \ref alxcvALOX_LOGGERNAME_FORMAT_OTHER.
/// This variable can be accessed programatically
/// with \alib{lox::textlogger;TextLogger::GetFormatOther} or by accessing the variable's value
/// through the configuration object found in camp singleton #alib::ALOX.
//==================================================================================================
struct FormatOther
{
    /// Replacement string if no source info is available.
    NAStringPA  NoSourceFileInfo;

    /// Replacement string if no source info is available.
    NAStringPA  NoMethodInfo;

    /// The minimum digits to write for the log number (if used in format string).
    int         LogNumberMinDigits;

    /// Constructor taking an object pool which is passed to each string member.
    /// @param pool The pool object of the \alib{variables;Configuration}.
    FormatOther(PoolAllocator& pool)
    : NoSourceFileInfo(pool)
    , NoMethodInfo    (pool)                                                                      {}
}; // struct FormatOther


//==================================================================================================
/// Automatically detected field width and tab-stop positions for the meta-information part
/// as well as the log message part, collected during the run of software.
/// This struct is used to define an external \ref alib_mod_variables "configuration variable"
/// named \ref alxcvALOX_LOGGERNAME_AUTO_SIZES.
///
/// This variable can be accessed programatically either with method
/// \alib{lox::textlogger;TextLogger::GetAutoSizes} or by accessing the variable's value
/// through the configuration object found in camp singleton #alib::ALOX.
///
/// For almost all use cases, field #Main will work as expected, because the meta-information
/// of log lines usually do not change. In other words, if they get changed, it is a good idea
/// to invoke \alib{strings::util;AutoSizes::Reset} on that field.
///
/// Field #LogMessage with built-in text loggers, is passed to field
/// \alib{format;FormatterPythonStyle::Sizes} and thus is used to organize tab-stops and
/// field widths of the log messages itself.<br>
/// The field may be reset manually after (or before) a coherent set of log messages have
/// been (respectively are to be) written. Such "coherent set" is for example the output of
/// tabular data. If this is not done, a next table containing different data, would not be
/// printed with correct field sizes in case the automatic tab-stops.
///
/// Also, it might be reasonable to invoke \alib{strings::util::AutoSizes;Consolidate} one both
/// objects from time to time, or in certain situations.
///
/// @see %Variable \ref alxcvALOX_LOGGERNAME_AUTO_SIZES.
//==================================================================================================
struct FormatAutoSizes
{
    /// The instance used with the meta info format string.
    AutoSizes   Main;

    /// An \b AutoSizes object which is used to format the log message itself.
    /// \note With the use of class \alib{lox::textlogger;StandardConverter}, this
    ///       instance is passed to the internal instance(s) of the
    ///       \alib{format;FormatterPythonStyle} used.
    AutoSizes   LogMessage;
}; // struct FormatAutoSizes

//==================================================================================================
/// A simple list of pairs of strings to search in log messages and their replacement value.
/// This variable can be accessed programatically with
/// \alib{lox::textlogger;TextLogger::GetReplacements} or by accessing the variable's value
/// through the configuration object found in camp singleton #alib::ALOX.
//==================================================================================================
struct Replacements
{
    /// The list of pairs of replacement strings
    std::vector<AStringPA, lang::StdContainerAllocator<AStringPA, PoolAllocator>>   Pairs;

    /// Constructor taking an object pool which is passed to the string vector.
    /// @param pool The pool object of the \alib{variables;Configuration}.
    Replacements(PoolAllocator& pool)
    : Pairs(pool)                                                                                 {}
};

/// Parameters specific to colorful loggers. As of today, this simply has one attribute.
struct ColorfulLoggerParameters
{
    /// Denotes the use of light vs dark colors. Used with colorful descendants of class
    /// \alib{lox::textlogger;TextLogger}.
    enum class LightColorUsage
    {
        Auto,            ///< Automatic
        Never,           ///< Never use light colors
        Foreground,      ///< Use light colors for foreground
        Background       ///< Use light colors for background
    };

    LightColorUsage      LCU= LightColorUsage::Auto; ///< Denotes use of light or dark colors.
                                                     ///< Defaults to \e Auto.
};


} // namespace [alib::lox::textlogger]

DOX_MARKER([DOX_VARIABLES_DEFINETYPE1])
ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::lox::textlogger::, FormatMetaInfo           ,A_CHAR("ALOXFMI") )
DOX_MARKER([DOX_VARIABLES_DEFINETYPE1])
ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::lox::textlogger::, FormatDateTime           ,A_CHAR("ALOXFDT") )
ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::lox::textlogger::, FormatTimeDiff           ,A_CHAR("ALOXFTD") )
ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::lox::textlogger::, FormatMultiLine          ,A_CHAR("ALOXFML") )
ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::lox::textlogger::, FormatOther              ,A_CHAR("ALOXFO" ) )
ALIB_VARIABLES_DEFINE_TYPE                      ( alib::lox::textlogger::, FormatAutoSizes          ,A_CHAR("ALOXAS" ) )
ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::lox::textlogger::, Replacements             ,A_CHAR("ALOXSR" ) )
ALIB_VARIABLES_DEFINE_TYPE                      ( alib::lox::textlogger::, ColorfulLoggerParameters ,A_CHAR("ALOXCLP") )


ALIB_ENUMS_ASSIGN_RECORD( alib::lox::textlogger::ColorfulLoggerParameters::LightColorUsage, alib::enumrecords::ERSerializable )

