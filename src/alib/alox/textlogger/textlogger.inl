//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace lox {

//==================================================================================================
/// This namespaces defines class \b TextLogger and its helpers.
//==================================================================================================
namespace textlogger {

//==================================================================================================
/// This abstract class represents a plug-in for the TextLogger class which converts the list
/// of logables into a textual representation.
/// \see StandardConverter for further information.
//==================================================================================================
class ObjectConverter
{
    public:
        //==========================================================================================
        /// Destructs an object of this class.
        //==========================================================================================
        virtual            ~ObjectConverter() {}

        //==========================================================================================
        /// The conversion method.
        /// @param target     An AString that takes the result.
        /// @param logables   The objects to convert.
        //==========================================================================================
        virtual void        ConvertObjects( AString& target, BoxesMA& logables )                 =0;

        //==========================================================================================
        /// If this converter uses an \alib{strings::util;AutoSizes} object, this method passes
        /// an external object to use.
        /// @param autoSizes   The instance to use.
        //==========================================================================================
        ALIB_DLL
        virtual void        SetAutoSizes( AutoSizes* autoSizes )                                 =0;

        //==========================================================================================
        /// If this converter uses an \alib{strings::util;AutoSizes} object, this method returns
        /// such object.
        /// @return The auto sizes used, \c nullptr if not applicable.
        //==========================================================================================
        virtual AutoSizes*  GetAutoSizes()                                                       =0;

        //==========================================================================================
        /// If this converter uses an \alib{strings::util;AutoSizes} object, values of this field
        /// are reset.
        //==========================================================================================
        virtual void        ResetAutoSizes()                                                     =0;
};

//==================================================================================================
/// Implements the interface
/// \ref alib::lox::textlogger::ObjectConverter "ObjectConverter". Class
/// \ref alib::lox::textlogger::TextLogger "TextLogger" creates an instance of this type in
/// the moment no other (custom) type was set before the first log statement.
///
/// This implementation uses
/// two specialisations of class
/// \alib{format;Formatter} to format the given logables to a textual
/// representation. The formatters (and their sequence!) are:
///
/// 1. \alib{format;FormatterPythonStyle;FormatterPythonStyle}
/// 2. \alib{format;FormatterJavaStyle;FormatterJavaStyle}
///
/// This way, standard text logging supports format strings in Python style as well as in Java style.
//==================================================================================================
class StandardConverter : public ObjectConverter
{
    public:
        /// A list of formatters used to "convert" logables to strings.
        /// By default, each entry contains a concatenated pair of formatters of types
        /// \alib{format;FormatterPythonStyle;FormatterPythonStyle} and
        /// \alib{format;FormatterJavaStyle;FormatterJavaStyle} are added in the
        /// constructor of this class.
        ///
        /// A vector of formatters is needed to support recursive log calls.
        /// If recursion occurs during logging (aka the conversion of a logable triggers another
        /// logging operation), necessary formatters are created on the fly, respectively re-used
        /// from previous recursions.
        /// Their settings are cloned to those of the main formatters
        /// using \alib{format;Formatter::CloneSettings}.
        ///
        /// To use different formatters, it is recommended to implement a different converter
        /// type, instead of "patching" the linked and recursive formatters found in this vector.
        std::vector<Formatter*>             Formatters;

    protected:
        /// A counter to detect recursive calls.
        int                                 cntRecursion;

    public:

        //==========================================================================================
        /// Constructor.
        //==========================================================================================
        ALIB_DLL
                            StandardConverter();

        //==========================================================================================
        /// Virtual destructor.
        //==========================================================================================
        ALIB_DLL
        virtual            ~StandardConverter()                                            override;

        //==========================================================================================
        /// The conversion method.
        /// Passes \p{target} and \p{logables} to the #Formatters.
        /// @param target     An AString that takes the result.
        /// @param logables   The objects to convert.
        //==========================================================================================
        ALIB_DLL
        virtual void        ConvertObjects( AString& target, BoxesMA& logables )           override;

        //==========================================================================================
        /// Checks if the first formatter in #Formatters is of type
        /// \alib{format;FormatterPythonStyle}. If so, its \b AutoSizes member is set to
        /// the given external instance. Otherwise the call is ignored.
        /// @param autoSizes The instance to use.
        //==========================================================================================
        ALIB_DLL
        virtual void        SetAutoSizes( AutoSizes* autoSizes )                           override;

        //==========================================================================================
        /// Checks if the first formatter in #Formatters is of type
        /// \alib{format;FormatterPythonStyle}. If so, its \b AutoSizes member is returned.
        /// If not, the method returns \c nullptr.
        /// @return The auto sizes object of the main formatter.
        //==========================================================================================
        ALIB_DLL
        virtual AutoSizes*  GetAutoSizes()                                                 override;

        //==========================================================================================
        /// Resets automatically widened tab stops and field widths of this converter.
        //==========================================================================================
        ALIB_DLL
        virtual void        ResetAutoSizes()                                               override;
}; //class StandardConverter


//==================================================================================================
///  This class is a still abstract implementation of class Logger which is used as a base
///  for all textual Logger implementations within \alox, e.g., \b %ConsoleLogger.
///
///  One main purpose of the class is to generate the textual representation of the meta-information
///  of a log call.  The final log message is then passed to the abstract method #logText.
///  Hence, types that inherited from this class instead of directly from class
///  \alib{lox;detail::Logger},  need to implement #logText instead of implementing #Log.
///
///  Class \b %TextLogger supports multi line log outputs. Such multi line log outputs can be
///  configured to be logged in different ways. See struct \alib{lox::textlogger;FormatMultiLine} for more
///  information.
//==================================================================================================
class TextLogger : public Logger
{

  // ###############################################################################################
  // Internal fields
  // ###############################################################################################
  protected:

    /// The internal log Buffer.
    AString     logBuf;

    /// The buffers for converting the logables.
    AString     msgBuf;

    /// Denotes whether this logger writes to the <em>standard output streams</em>. If so,
    /// \alib{threads;STD_IOSTREAMS_LOCK} is acquired with writing.
    bool        usesStdStreams;

    /// Variable of type \alib{lox::textlogger;FormatMetaInfo} residing in the
    /// \alib{variables;Configuration} of camp \alib_alox.
    Variable    varFormatMetaInfo;

    /// Variable of type \alib{lox::textlogger;FormatDateTime} residing in the
    /// \alib{variables;Configuration} of camp \alib_alox.
    Variable    varFormatDateTime;

    /// Variable of type \alib{lox::textlogger;FormatTimeDiff} residing in the
    /// \alib{variables;Configuration} of camp \alib_alox.
    Variable    varFormatTimeDiff;

    /// Variable of type \alib{lox::textlogger;FormatMultiLine} residing in the
    /// \alib{variables;Configuration} of camp \alib_alox.
    Variable    varFormatMultiLine;

    /// Variable of type \alib{lox::textlogger;FormatOther} residing in the
    /// \alib{variables;Configuration} of camp \alib_alox.
    Variable    varFormatOther;

    /// Variable of type \alib{lox::textlogger;FormatAutoSizes}, which inherits class
    /// \alib{strings::util;AutoSizes}.The sizes are used for auto tab positions and field sizes.
    /// For each requested value, a corresponding array field is created on the fly.
    /// If the format string get's changed and different (new) auto values should be used, then
    /// this field should be reset after setting the new format string.
    /// The other way round, it is also possible to preset set minimum values for tabs and field
    /// sizes and hence avoid the columns growing during the lifetime of the Logger.
    ///
    /// The second member of type \b AutoSizes is attached to field #Converter
    /// when this \b %TextLogger is attached to a \b %Lox.
    ///
    /// This field will be read from configuration variable \ref alxcvALOX_LOGGERNAME_AUTO_SIZES
    Variable    varFormatAutoSizes;

    /// A list of pairs of strings. Within each log message, the first string of a pair is
    ///  searched and replaced by the second string. Very simple, but useful in some cases.
    Variable    varReplacements;


    /// A singleton calendar time object shared between different format variables during one
    ///  invocation.
    strings::util::CalendarDateTime callerDateTime;


  // ###############################################################################################
  // Public fields
  // ###############################################################################################
  public:
    /// A helper object to get textual representation of logable objects.
    /// If no converter is set when this logger is attached to a lox, a converter of type
    /// \alib{lox::textlogger;StandardConverter} is created and used.
    /// Custom loggers might create their own, custom converter objects here.
    /// In the destructor of this class, the current object converter will be deleted.
    ///
    /// To extend class \b %TextLogger to support logging custom objects, custom converters can
    /// set. The preferred alternative is however, to make custom types be formattable
    /// by formatter classes used with \b %StandardConverter.
    ObjectConverter*            Converter                                                 = nullptr;

    /// This field is used to convert the steady and monotonic clock values provided with
    /// \alox{detail,ScopeInfo::GetTimeStamp} into human-readable, calendrical values in the case
    /// that field #varFormatMetaInfo contains tokens <c>%%TD</c> and/or <c>%%TT</c>.
    ///
    /// This may become problematic and ambiguous if the system clock is changed during a
    /// software run. Especially for long-running background software (daemons, servers, etc.),
    /// the software that uses \alox needs to provide a strategy of synchronizing this converter
    /// with the system clock.
    ///
    /// A simple strategy is to just periodically invoke \alib{time;TickConverter::SyncClocks},
    /// for example once a second.
    ///
    /// For some explanation of the problem see details of namespace #alib::time.
    TickConverter               DateConverter;

    ///  If \c false, an one-time warning (using \ref ALIB_WARNING) will be issued if the format
    ///  string is illegal. With each warning, the flag is set to \c true to omit further
    ///  warnings.
    bool                        FormatWarningOnce= false;

   /// Provides access to the value of the internal configuration variable #varFormatMetaInfo.<br>
   /// This variable is declared only after the logger was added to a \b %Lox.
   /// @return The struct containing formatting information.
   FormatMetaInfo&  GetFormatMetaInfo()          { return varFormatMetaInfo.Get<FormatMetaInfo>(); }

   /// Provides access to the value of the internal configuration variable #varFormatDateTime.<br>
   /// This variable is declared only after the logger was added to a \b %Lox.
   /// @return The struct containing formatting information.
   FormatDateTime&  GetFormatDate()              { return varFormatDateTime.Get<FormatDateTime>(); }

   /// Provides access to the value of the internal configuration variable #varFormatTimeDiff.<br>
   /// This variable is declared only after the logger was added to a \b %Lox.
   /// @return The struct containing formatting information.
   FormatTimeDiff&  GetFormatTimeDiff()          { return varFormatTimeDiff.Get<FormatTimeDiff>(); }

   /// Provides access to the value of the internal configuration variable #varFormatMultiLine.<br>
   /// This variable is declared only after the logger was added to a \b %Lox.
   /// @return The struct containing formatting information.
   FormatMultiLine& GetFormatMultiLine()       { return varFormatMultiLine.Get<FormatMultiLine>(); }

   /// Provides access to the value of the internal configuration variable #varFormatOther.<br>
   /// This variable is declared only after the logger was added to a \b %Lox.
   /// @return The struct containing formatting information.
   FormatOther&     GetFormatOther()                   { return varFormatOther.Get<FormatOther>(); }

   /// Provides access to the value of the internal configuration variable #varFormatAutoSizes.<br>
   /// This variable is declared only after the logger was added to a \b %Lox.
   /// @return The struct containing the \alib{strings::util;AutoSizes} instances for the
   ///        meta-information of the log message and for the log message itself.
   FormatAutoSizes& GetAutoSizes()             { return varFormatAutoSizes.Get<FormatAutoSizes>(); }

   /// Provides access to the value of the internal configuration variable #varReplacements.<br>
   /// This variable is declared only after the logger was added to a \b %Lox.
   /// @return The \alib{lox::textlogger;Replacements} instance for the logger in question.
   Replacements&    GetReplacements()                { return varReplacements.Get<Replacements>(); }


  // ###############################################################################################
  // protected Constructor/ public destructor
  // ###############################################################################################
  protected:
    //==============================================================================================
    /// Constructs a TextLogger.
    /// @param pName            The name of the \e Logger.
    /// @param typeName         The type of the \e Logger.
    /// @param pUsesStdStreams  Denotes whether this logger writes to the
    ///                        <em>standard output streams</em>.
    //==============================================================================================
    ALIB_DLL explicit TextLogger( const NString& pName, const NString& typeName,
                                  bool  pUsesStdStreams );

  // ###############################################################################################
  // protected methods
  // ###############################################################################################
    //==============================================================================================
    ///  Parses the format string in the field #varFormatMetaInfo and logs meta-information into
    ///  the log buffer.
    ///  For each variable found, the method #processVariable is invoked.
    ///  Hence, to add new variables, the latter method can be overwritten by descendants.
    ///  Overwriting this method is recommended for formatter classes that do not rely on format
    ///  strings.
    /// @param buffer    The buffer to write meta-information into.
    /// @param domain    The <em>Log Domain</em>.
    /// @param verbosity The verbosity.
    /// @param scope     Information about the scope of the <em>Log Statement</em>..
    //==============================================================================================
    ALIB_DLL
    virtual void writeMetaInfo( AString&           buffer,
                                detail::Domain&    domain,
                                Verbosity          verbosity,
                                detail::ScopeInfo& scope     );

    //==============================================================================================
    /// Processes the next command found in the format string, by writing formatted information
    /// into the given buffer.
    /// The given
    /// \alib{strings;TSubstring;Substring} holds the next command.
    /// When the method returns, the command is cut from the front.
    ///
    /// @param domainPath   The <em>Log Domain</em> full path.
    /// @param verbosity    The verbosity. This has been checked to be active already on this
    ///                     stage and is provided to be able to be logged out only.
    /// @param scope        Information about the scope of the <em>Log Statement</em>..
    /// @param dest         The buffer to write meta-information into.
    /// @param variable     The variable to read (may have more characters appended)
    //==============================================================================================
    ALIB_DLL
    virtual void processVariable( const NString&     domainPath,
                                  Verbosity          verbosity,
                                  detail::ScopeInfo& scope,
                                  AString&           dest,
                                  Substring&         variable      );

    //==============================================================================================
    ///  Helper function that logs a time given difference into the given buffer in a human-readable
    ///  format. Works from nanoseconds seconds to days.
    ///
    /// @param buffer       The buffer to write the time difference representation into.
    /// @param diffNanos    The time difference to write in nanoseconds.
    //==============================================================================================
    ALIB_DLL
    virtual void writeTimeDiff  ( AString& buffer, int64_t diffNanos );


  public:
    //==============================================================================================
    ///  Destructs a TextLogger.
    //==============================================================================================
    ALIB_DLL virtual ~TextLogger()                                                         override;

  // ###############################################################################################
  // Overriding parent's virtual, empty method AcknowledgeLox()
  // ###############################################################################################

    //==============================================================================================
    /// Configuration variables are read within this method and created with
    /// default values, in the case they do not exist, yet. The variables read are:
    /// - \b alxcvALOX_LOGGERNAME_AUTO_SIZES
    /// - \b alxcvALOX_LOGGERNAME_FORMAT
    /// - \b alxcvALOX_LOGGERNAME_FORMAT_DATE_TIME
    /// - \b alxcvALOX_LOGGERNAME_FORMAT_TIME_DIFF
    /// - \b alxcvALOX_LOGGERNAME_FORMAT_MULTILINE
    /// - \b alxcvALOX_LOGGERNAME_FORMAT_OTHER
    /// - \b alxcvALOX_LOGGERNAME_REPLACEMENTS
    ///
    /// Configuration variables are \ref alib_alox_cfgvars "documented here".
    ///
    /// @param lox     The \b %Lox to acknowledge insertion or removal
    /// @param op      The operation. Either \b ContainerOp::Insert or \b ContainerOp::Remove.
    //==============================================================================================
    ALIB_DLL
    virtual void AcknowledgeLox( detail::LoxImpl* lox, lang::ContainerOp op )              override;


  // ###############################################################################################
  // Abstract methods introduced
  // ###############################################################################################
  protected:
    //==============================================================================================
    ///  This abstract method introduced by this class "replaces" the abstract method #Log
    ///  of parent class Logger which this class implements. In other words, descendants of this
    ///  class need to override this method instead of \b %Log. This class %TextLogger is
    ///  responsible for generating meta-information, doing text replacements, handle multi-line
    ///  messages, etc. and provides the textual representation of the whole log contents
    ///  to descendants using this method.
    ///
    /// @param domain     The <em>Log Domain</em>.
    /// @param verbosity  The verbosity. This has been checked to be active already on this
    ///                   stage and is provided to be able to be logged out only.
    /// @param msg        The log message.
    /// @param scope      Information about the scope of the <em>Log Statement</em>.
    /// @param lineNumber The line number of a multi-line message, starting with 0.
    ///                   For single line messages this is -1.
    //==============================================================================================
    virtual void logText(  detail::Domain&     domain,
                           Verbosity           verbosity,
                           AString&            msg,
                           detail::ScopeInfo&  scope,
                           int                 lineNumber )                                  =0;

    //==============================================================================================
    /// Abstract method to be implemented by descendants. This message is called only when
    /// multi-line messages are logged. It is called exactly once before a series of #logText
    /// calls of a multi-line message and exactly once after such series.<br>
    /// This is useful if the writing of text includes the acquisition of system resources
    /// (e.g., opening a file).
    ///
    /// @param phase  Indicates the beginning or end of a multi-line operation.
    //==============================================================================================
    virtual void notifyMultiLineOp( lang::Phase phase )                                      =0;

  // ###############################################################################################
  // Abstract method implementations
  // ###############################################################################################
  public:
    //==============================================================================================
    /// This is the implementation of the abstract method inherited from class Logger
    /// that executes a log.<br>
    /// This class implements this method and but exposes the new abstract method #logText.
    /// This mechanism allows this class to do various things that are standard to Loggers
    /// of type TextLogger. For example, meta-information of the log invocation is formatted
    /// and string replacements are performed. This way, descendants of this class will consume
    /// a ready to use log buffer with all meta-information and contents of all objects to be
    /// included and their primary obligation is to copy the content into a corresponding
    /// output stream.
    ///
    /// @param domain    The <em>Log Domain</em>.
    /// @param verbosity The verbosity.
    /// @param logables  The list of objects to log.
    /// @param scope     Information about the scope of the <em>Log Statement</em>..
    //==============================================================================================
    ALIB_DLL
    virtual void Log( detail::Domain& domain, Verbosity verbosity, BoxesMA& logables,
                      detail::ScopeInfo& scope)                                            override;

  // ###############################################################################################
  // Public interface
  // ###############################################################################################
  public:
    //==============================================================================================
    /// Adds the given pair of replacement strings. If searched string already exists, the
    /// current replacement string gets replaced. If the replacement string is \c nullptr,
    /// nothing is set and a previously set replacement definition becomes unset.
    /// @param searched    The string to be searched.
    /// @param replacement The replacement string. If this equals 'nullptr' a previously set
    ///                    replacement will be unset.
    //==============================================================================================
    ALIB_DLL
    virtual void  SetReplacement( const String& searched, const String& replacement );

    //==============================================================================================
    ///  Removes all pairs of searched strings and their replacement value.
    //==============================================================================================
    ALIB_DLL
    virtual void  ClearReplacements();

    //==============================================================================================
    /// Resets automatically widened tab stops and field widths of this logger by calling
    /// \alib{lox::textlogger::StandardConverter;ResetAutoSizes} on field #Converter.
    ///
    /// \note The sizes affected are the ones used to format the custom log output, not
    ///       the ones uses for the meta-information. To reset the auto-sizes of the meta
    ///       information, invoke \alib{strings::util;AutoSizes::Reset} on field #AutoSizes.
    //==============================================================================================
    ALIB_DLL
    virtual void    ResetAutoSizes();
}; // class TextLogger

}} // namespace alib[::lox::textlogger]

/// Type alias in namespace \b alib.
using     TextLogger=       lox::textlogger::TextLogger;

}  // namespace [alib]

