//==================================================================================================
/// \file
/// This header-file is part of module \alib_app of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace app {

//==================================================================================================
/// This is a friend class of #"CommandLine" that exposes a collection of utility methods
/// useful for CLI applications.
///
/// The methods found here are static and receive the friend #"%CommandLine" object. They have been
/// gathered in this class to keep #"%CommandLine" tidy and more easily understandable as only
/// needed methods are found there.
///
/// \note
///   The nature of this class is to provide a basic, standard functionality. It was created while
///   building a CLI application based on this library and provided for convenience.
///
///   Methods found here, might be used as a jump start for own implementations.
///   Their documentation is considered only fundamental.
///   For details, please consult the source code.
//==================================================================================================
class CLIUtil {
  public:
    /// Searches and if found, retrieves the declaration of the option identified by
    /// \p{identString} which, if it contains a single character is compared to the
    /// #"OptionDecl::IdentifierChar;*". Otherwise, matching is done case-insensitive and
    /// with respecting #"OptionDecl::MinimumRecognitionLength;*".
    ///
    /// This method is useful to implement a help command or option, with an optional "topic"
    /// parameter.
    ///
    /// \note
    ///   If parsing of arguments should be (or has to be) customized, of course this method
    ///   can also be used for implementing such custom code. Otherwise, use method
    ///   #"CommandLine::ReadOptions;*", which parses and collects options in filed
    ///   #"CommandLine::Options;*".
    ///
    /// \see Methods #"GetCommandDecl", #".GetParameterDecl".
    ///
    /// @param cmdLine      The friend object we work on.
    /// @param identString The identifier string of the option to search. If this is a single
    ///                    character, the short identifier is searched.
    /// @return The object of type #"OptionDecl". \c nullptr if not found.
    static ALIB_DLL
    OptionDecl*     GetOptionDecl( CommandLine& cmdLine, const String& identString );

    /// Searches and if found, retrieves the declaration of the command identified by
    /// \p{identString}. Matching is done case-insensitive and with respecting
    /// #"CommandDecl::MinimumRecognitionLength;*".
    ///
    /// This method is useful to implement a help command or option, with an optional "topic"
    /// parameter.
    ///
    /// \note
    ///   If parsing of arguments should be (or has to be) customized, of course this method
    ///   can also be used for implementing such custom code. Otherwise, use
    ///   #"CommandLine::NextCommand;*" to retrieve command objects (instead of command
    ///   declarations).
    ///
    /// \see Methods #"GetOptionDecl", #".GetParameterDecl".
    ///
    /// @param cmdLine      The friend object we work on.
    /// @param identString The identifier of the command to search.
    /// @return The object of type #"CommandDecl". \c nullptr if not found.
    static ALIB_DLL
    CommandDecl*    GetCommandDecl( CommandLine& cmdLine, const String& identString );

    /// Searches and if found, retrieves the declaration of the parameter identified by
    /// \p{identString}. Matching is done case-insensitive and with respecting
    /// #"CommandDecl::MinimumRecognitionLength;*".
    ///
    /// This method is useful to implement a help command (or option), with an optional "topic"
    /// parameter.
    ///
    /// \see Methods #"GetOptionDecl", #"GetCommandDecl".
    ///
    /// @param cmdLine      The friend object we work on.
    /// @param identString The identifier of the command to search.
    /// @return The object of type #"CommandDecl". \c nullptr if not found.
    static ALIB_DLL
    ParameterDecl*  GetParameterDecl( CommandLine& cmdLine, const String& identString );


    /// Returns an AString providing a formatted help text on the defined command.
    /// @param cmdLine       The command-line instance.
    /// @param commandDecl  The declaration of the command to get help on.
    /// @return The help text.
    static ALIB_DLL
    AString         GetCommandUsageFormat( CommandLine& cmdLine, CommandDecl& commandDecl );

    /// Creates a help text from the resource strings.
    ///
    /// @param cmdLine       The command-line instance.
    /// @param topics        A comma-separated list of topics. If empty or nulled, general help is
    ///                      created.
    /// @param text          The target text.
    /// @return \c true on success. \c false if an argument was given that is not recognized or
    ///         if a topic list was found in the next argument where only some of the topics
    ///         could be identified.
    static ALIB_DLL
    bool GetHelp( CommandLine& cmdLine, const String& topics, Paragraphs& text );

    /// Creates a help text from the given help command.
    ///
    /// This method accepts a command-object that the command-line instance uses to process
    /// help requests.
    ///
    /// If no argument is set in \p{helpCmd}, the next argument is peeked and is checked to be a
    /// command, option, parameter, or a special help topic found in resource string
    /// "HlpAddnlTopics".
    ///
    /// If found, the argument is consumed and stored in \p{helpCmd}.
    /// If not, the general help text is generated.
    ///
    /// @param cmdLine   The command-line instance.
    /// @param helpCmd   The command to write the help text for.
    /// @param text      The target text.
    /// @return \c true on success. \c false if an argument was given that is not recognized or
    ///         if a topic list was found in the next argument where only some of the topics
    ///         could be identified.
    static ALIB_DLL
    bool GetHelp( CommandLine& cmdLine, Command* helpCmd, Paragraphs& text );

    /// Creates a help text from the given help option.
    ///
    /// This method accepts an option-object that the command-line instance uses to process
    /// help requests.
    ///
    /// If no argument is set in \p{helpOpt}, the next argument is peeked and is checked to be a
    /// command, option, parameter or special help topic found in resource string "HlpAddnlTopics".
    ///
    /// If found, the argument is consumed and stored in \p{helpOpt}.
    /// If not, the general help text is generated.
    ///
    /// @param cmdLine   The command-line instance.
    /// @param helpOpt   The option to write the help text for.
    /// @param text      The target text.
    /// @return \c true on success. \c false if an argument was given that is not recognized or
    ///         if a topic list was found in the next argument where only some of the topics
    ///         could be identified.
    static ALIB_DLL
    bool GetHelp( CommandLine& cmdLine, Option* helpOpt, Paragraphs& text );

    /// Dumps the configuration.
    /// Shows which commands, options, parameters and errors are set with enums and their
    /// meta info.
    /// Useful during development.
    ///
    /// @param cmdLine    The friend object we work on.
    /// @param text      The target text.
    /// @return An internal \c AString object containing the dump text. (Beware of concurrent
    ///         debugging threads :-)
    static ALIB_DLL
    AString&     DumpDeclarations( CommandLine& cmdLine, Paragraphs& text );

    /// Write in human-readable form, which commands and options have been read from the
    /// command-line.
    ///
    /// This is useful for debugging as well as to implement a "dry run" option of the
    /// CLI application, that offers the user a list of what is parsed with a given set of
    /// CLI arguments. In this case, method read #"CommandLine::ReadNextCommands;*" should
    /// be invoked after the provisions of the various definitions.
    ///
    /// Probably, depending on the command syntax, not all commands can be parsed prior
    /// to executing them. However, options can.
    ///
    /// @param cmdLine    The friend object we work on.
    /// @param text      The target text.
    /// @returns Returns an internal \c AString object containing the dump text.
    ///          (Beware of concurrent debugging threads :-)
    static ALIB_DLL
    AString&     DumpParseResults( CommandLine& cmdLine, Paragraphs& text );
};

} // namespace alib[::app]

/// Type alias in namespace #"%alib".
using     CLIUtil=           app::CLIUtil;


}  // namespace [alib]
