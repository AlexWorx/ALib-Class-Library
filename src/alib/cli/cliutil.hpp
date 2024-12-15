//==================================================================================================
/// \file
/// This header file is part of module \alib_cli of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_CLI_CLIUTIL
#define HPP_ALIB_CLI_CLIUTIL 1
#pragma once
#include "alib/cli/commandline.hpp"
#include "alib/lang/format/paragraphs.hpp"
#include "alib/lang/message/report.hpp"


namespace alib {  namespace cli {

//==================================================================================================
/// This is a friend class of \alib{cli;CommandLine} that exposes a collection of utility methods
/// useful for CLI applications.
///
/// The methods found here are static and receive the friend \b %CommandLine object. They have been
/// gathered in this class to keep \b %CommandLine tidy and more easily understandable as only
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
class CLIUtil
{
    public:
    //==============================================================================================
    /// Searches and if found, retrieves the declaration of the option identified by
    /// \p{identString} which, if it contains a single character is compared to the
    /// \alib{cli;OptionDecl::IdentifierChar}. Otherwise, matching is done case insensitive and
    /// with respecting \alib{cli;OptionDecl::MinimumRecognitionLength}.
    ///
    /// This method is useful to implement a help command or option, with an optional "topic"
    /// parameter.
    ///
    /// \note
    ///   If parsing of arguments should be (or has to be) customized, of course this method
    ///   can also be used for implementing such custom code. Otherwise, use method
    ///   \alib{cli;CommandLine::ReadOptions}, which parses and collects options in filed
    ///   \alib{cli;CommandLine::Options}.
    ///
    /// \see Methods #GetCommandDecl, #GetParameterDecl.
    ///
    /// @param cmdLine      The friend object we work on.
    /// @param identString The identifier string of the option to search. If this is a single
    ///                    character, the short identifier is searched.
    /// @return The object of type \alib{cli;OptionDecl}. \c nullptr if not found.
    //==============================================================================================
    static ALIB_API
    OptionDecl*     GetOptionDecl( CommandLine& cmdLine, const String& identString );

    //==============================================================================================
    /// Searches and if found, retrieves the declaration of the command identified by
    /// \p{identString}. Matching is done case insensitive and with respecting
    /// \alib{cli;CommandDecl::MinimumRecognitionLength}.
    ///
    /// This method is useful to implement a help command or option, with an optional "topic"
    /// parameter.
    ///
    /// \note
    ///   If parsing of arguments should be (or has to be) customized, of course this method
    ///   can also be used for implementing such custom code. Otherwise, use
    ///   \alib{cli;CommandLine::NextCommand} to retrieve command objects (instead of command
    ///   declarations).
    ///
    /// \see Methods #GetOptionDecl, #GetParameterDecl.
    ///
    /// @param cmdLine      The friend object we work on.
    /// @param identString The identifier of the command to search.
    /// @return The object of type \alib{cli;CommandDecl}. \c nullptr if not found.
    //==============================================================================================
    static ALIB_API
    CommandDecl*    GetCommandDecl( CommandLine& cmdLine, const String& identString );

    //==============================================================================================
    /// Searches and if found, retrieves the declaration of the parameter identified by
    /// \p{identString}. Matching is done case insensitive and with respecting
    /// \alib{cli;CommandDecl::MinimumRecognitionLength}.
    ///
    /// This method is useful to implement a help command (or option), with an optional "topic"
    /// parameter.
    ///
    /// \see Methods #GetOptionDecl, #GetCommandDecl.
    ///
    /// @param cmdLine      The friend object we work on.
    /// @param identString The identifier of the command to search.
    /// @return The object of type \alib{cli;CommandDecl}. \c nullptr if not found.
    //==============================================================================================
    static ALIB_API
    ParameterDecl*  GetParameterDecl( CommandLine& cmdLine, const String& identString );


    //==============================================================================================
    /// Returns an AString providing a formatted help text on the defined command.
    /// @param cmdLine       The command line instance.
    /// @param commandDecl  The declaration of the command to get help on.
    /// @return The help text.
    //==============================================================================================
    static ALIB_API
    AString         GetCommandUsageFormat( CommandLine& cmdLine, CommandDecl& commandDecl );

#include "alib/lang/callerinfo_functions.hpp"
    //==============================================================================================
    /// Translates exceptions thrown by the \alib_cli_nl library to exit codes defined with
    /// \alib{cli;CommandLine::DefineExitCodes}.
    ///
    /// If the code is not found, this indicates an error in the resource data, as an exit
    /// code corresponding to the \alib_cli_nl exceptions is obviously missing.
    /// In this case, \c -1 is returned. With debug-builds an \alib assertion is raised.
    ///
    /// @param cmdLine     The friend object we work on.
    /// @param exception  The cli exception caught.
    /// @return The exit code to return to the caller. \c -1, if not found.
    //==============================================================================================
    static ALIB_API
    integer       GetExitCode( CommandLine& cmdLine, Exception& exception )
    {
        auto element= exception.Type().Get<cli::Exceptions>();
        for( auto exitCodeDecl : cmdLine.ExitCodeDecls )
            if( exitCodeDecl.second->AssociatedCLIException() == element )
                return exitCodeDecl.first.Integral();
        ALIB_ERROR( "CLI", "No exit code associated with {}.", element )
        return -1;
    }
#include "alib/lang/callerinfo_methods.hpp"

    //==============================================================================================
    /// Creates a help text from the resource strings.
    ///
    /// This method accepts either a command object or an option object that the command line
    /// instance uses to process help requests. Only one of the objects has to be provided, the
    /// other has to be \c nullptr.
    ///
    /// If no argument is set in \p{helpCmd} (respectively \p{helpOpt}), the next argument is peeked
    /// and checked to be a command, option, parameter or special help topic found in resource
    /// string "HlpAddnlTopics".
    ///
    /// If found, the argument is consumed and stored in \p{helpCmd} (respectively \p{helpOpt}).
    /// If not, the general help text is generated.
    ///
    /// @param cmdLine   The command line instance.
    /// @param helpCmd   The command to write the help text for.
    /// @param helpOpt   The option to write the help text for.
    /// @param text      The target text.
    /// @return \c true on success. \c false if an argument was given that is not recognized or
    ///         if a topic list was found in the next argument where only some of the topics
    ///         could be identified.
    //==============================================================================================
    static ALIB_API
    bool GetHelp( CommandLine& cmdLine, Command* helpCmd, Option* helpOpt, Paragraphs& text );

    //==============================================================================================
    /// Reads a dry-run options and stores the result in \alib{cli;CommandLine::DryRun}.
    ///
    /// Option arguments as defined with records of enumeration \alib{cli;DryRunModes} are
    /// accepted. These records are resourced and default to:
    ///       \snippet "alib/cli/clicamp.cpp"  DOX_CLI_DRYRUN_RESOURCES
    ///
    /// If no argument is set in the given \p{dryOpt}, the next unread CLI-argument is checked
    /// for being parsable as an element of enum \b DryRunModes. If yes, the CLI-argument is
    /// consumed and duly stored in \p{dryOpt}.<br>
    /// In case no argument was set (or successfully peeked), \alib{cli;DryRunModes::Application} is
    /// chosen and stored.
    ///
    /// If one of the modes offered by enumeration \b DryRunModes should not be recognizable,
    /// three ways of implementing this exist:
    /// 1. Do not use this method but implement a custom version. In case that only
    ///    \alib{cli;DryRunModes::Application} should be accepted, instead calling this util method,
    ///    simply set field \alib{cli;CommandLine::DryRun} to this value.
    /// 2. Check for the "forbidden" argument type after the invocation of this method and
    ///    exit your cli app
    /// 3. Modify this module's resource string <b>"CLI/DRM"</b> to not contain the suppressed
    ///    argument's record. (With that, the defaulted argument names can also be modified).
    ///
    /// By modifying the resource string, it is also possible to add custom options. Remeber, that
    /// it is allowed in C++ to have an enum element evaluate to any integral, regardless
    /// whether it is defined in the C++ definition or not.
    ///
    /// @param cmdLine   The command line instance.
    /// @param dryOpt    The option object parsed.
    /// @return \c true on success. \c false if an argument was given that is not recognized.
    //==============================================================================================
    static ALIB_API
    bool GetDryOpt( CommandLine& cmdLine, Option& dryOpt );

    //==============================================================================================
    /// Dumps the configuration.
    /// Shows which commands, options, parameters and errors are set with enums and their
    /// meta info.
    /// Useful during development.
    ///
    /// @param cmdLine    The friend object we work on.
    /// @param text      The target text.
    /// @return An internal \c AString object containing the dump text. (Beware of concurrent
    ///         debugging threads :-)
    //==============================================================================================
    static ALIB_API
    AString&     DumpDeclarations( CommandLine& cmdLine, Paragraphs& text );

    //==============================================================================================
    /// Write in human-readable form, which commands and options have been read from the
    /// command line.
    ///
    /// This is useful for debugging as well as to implement a "dry run" option of the
    /// CLI application, that offers the user a list of what is parsed with a given set of
    /// CLI arguments. In this case, method read \alib{cli;CommandLine::ReadNextCommands} should
    /// be invoked after the provisions of the various definitions.
    ///
    /// Probably, depending on the command syntax, not all commands can be parsed prior
    /// to executing them. However, options can.
    ///
    /// @param cmdLine    The friend object we work on.
    /// @param text      The target text.
    /// @returns Returns an internal \c AString object containing the dump text.
    ///          (Beware of concurrent debugging threads :-)
    //==============================================================================================
    static ALIB_API
    AString&     DumpParseResults( CommandLine& cmdLine, Paragraphs& text );
};

} // namespace alib[::cli]

/// Type alias in namespace \b alib.
using     CLIUtil=           cli::CLIUtil;


}  // namespace [alib]



#endif // HPP_ALIB_CLI_CLIUTIL

