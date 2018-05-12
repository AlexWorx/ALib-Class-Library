// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_CLI_CLIUTIL)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_CLI_CLIUTIL
//! @cond NO_DOX
#define HPP_ALIB_CLI_CLIUTIL 1
//! @endcond

#if !defined (HPP_ALIB_CLI_CLIAPP)
#   include "alib/cli/cliapp.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_FORMAT_SIMPLETEXT)
#   include "alib/strings/format/simpletext.hpp"
#endif

namespace aworx {
namespace lox { class Lox; }

namespace lib { namespace cli {

/** ************************************************************************************************
 * This is a friend class of \alib{cli,CLIApp} that exposes a collection of utility methods
 * useful for CLI applications.
 *
 * The methods found here are static and receive the friend \b %CLIApp object. They have been
 * gathered in this class to keep \b %CLIApp tidy and more easily understandable as only
 * needed methods are found there.
 *
 * \note
 *   The nature of this class is to provide a basic, standard functionality. It was created while
 *   building a CLI application based on this library and provided for convenience.
 *
 *   Methods found here, might be used as a jump start for own implementations.
 *   Their documentation is considered only fundamental.
 *   For details, please consult the source code.
 **************************************************************************************************/
class CLIUtil
{
    public:
    /** ********************************************************************************************
     * Searches and if found, retrieves the declaration of the option identified by
     * \p{identString} which, if it contains a single character is compared to the
     * \alib{cli,OptionDecl::IdentifierChar}. Otherwise, matching is done case insensitive and
     * with respecting \alib{cli,OptionDecl::MinimumParseLen}.
     *
     * This method is useful to implement a help command or option, with an optional "topic"
     * parameter.
     *
     * \note
     *   If parsing of arguments should be (or has to be) customized, of-course this method
     *   can also be used for implementing such custom code. Otherwise, use
     *   \alib{cli,CLIApp::ReadOptions} to parse and collect options in
     *   \alib{cli,CLIApp::OptionsFound}.
     *
     * \see Methods #GetCommandDecl, #GetParameterDecl.
     *
     * @param cliApp      The friend object we work on.
     * @param identString The identifier string of the option to search. If this is a single
     *                    character, the short identifier is searched.
     * @return The object of type \alib{cli,OptionDecl}. \c nullptr if not found.
     **********************************************************************************************/
    static ALIB_API
    OptionDecl*     GetOptionDecl( CLIApp& cliApp, const String& identString );

    /** ********************************************************************************************
     * Searches and if found, retrieves the declaration of the command identified by
     * \p{identString}. Matching is done case insensitive and with respecting
     * \alib{cli,CommandDecl::MinimumParseLen}.
     *
     * This method is useful to implement a help command or option, with an optional "topic"
     * parameter.
     *
     * \note
     *   If parsing of arguments should be (or has to be) customized, of-course this method
     *   can also be used for implementing such custom code. Otherwise, use
     *   \alib{cli,CLIApp::NextCommand} to retrieve command objects (instead of command
     *   declarations).
     *
     * \see Methods #GetOptionDecl, #GetParameterDecl.
     *
     * @param cliApp      The friend object we work on.
     * @param identString The identifier of the command to search.
     * @return The object of type \alib{cli,CommandDecl}. \c nullptr if not found.
     **********************************************************************************************/
    static ALIB_API
    CommandDecl*    GetCommandDecl( CLIApp& cliApp, const String& identString );

    /** ********************************************************************************************
     * Searches and if found, retrieves the declaration of the parameter identified by
     * \p{identString}. Matching is done case insensitive and with respecting
     * \alib{cli,CommandDecl::MinimumParseLen}.
     *
     * This method is useful to implement a help command (or option), with an optional "topic"
     * parameter.
     *
     * \see Methods #GetOptionDecl, #GetCommandDecl.
     *
     * @param cliApp      The friend object we work on.
     * @param identString The identifier of the command to search.
     * @return The object of type \alib{cli,CommandDecl}. \c nullptr if not found.
     **********************************************************************************************/
    static ALIB_API
    ParameterDecl*  GetParameterDecl( CLIApp& cliApp, const String& identString );


    /** ********************************************************************************************
     * Returns an AString providing a formatted help text on the defined command.
     * @param commandDecl  The declaration of the command to get help on.
     * @return The help text.
     **********************************************************************************************/
    static ALIB_API
    AString         GetCommandUsageFormat( CommandDecl& commandDecl );


    /** ********************************************************************************************
     * Translates exceptions thrown by the \alibmod_nolink_cli library to exit codes defined with
     * \alib{cli,CLIApp::DefineExitCodes}.
     *
     * If the code is not found, this indicates an error in the resource data, as an exit
     * code corresponding to the \alibmod_nolink_cli exceptions is obviously missing.
     * In this case, \c -1 is returned. In debug compilations an \alib assertion is raised.
     *
     * @param cliApp    The friend object we work on.
     * @param exception  The cli exception caught.
     * @return The exit code to return to the caller. \c -1, if not found.
     **********************************************************************************************/
    static ALIB_API
    int             GetExitCode( CLIApp& cliApp, Exception& exception )
    {
        auto element= exception.Code().Get<lib::cli::Exceptions>();
        for( auto& exitCode : cliApp.ExitCodeDecls )
            if( exitCode.second.AssociatedCLIException() == element )
                return exitCode.second.EnumBox.Value();
        ALIB_ERROR( "No exit code associated with {}.", element );
        return -1;
    }

    /** ********************************************************************************************
     * Creates a help text from the resource strings.
     * If no argument is set in \p{helpOpt}, the next argument is peeked and checked to be a command,
     * option, parameter or special help topic found in resource string "HlpAddnlTopics".
     *
     * If found, the argument is consumed and stored in \p{dryOpt}.
     * If not, the general help is performed.
     *
     * @param cliApp    The application object.
     * @param helpOpt   The option to write the help text for.
     * @param text      The target text.
     * @param lox       A \alox object to log information about processing the help command.
     * @return \c true on success. \c false if an argument was given that is not recognized or
     *         if a topic list was found in the next argument where only some of the topics
     *         could be identified.
     **********************************************************************************************/
    static ALIB_API
    bool GetHelp( CLIApp& cliApp, Option& helpOpt, SimpleText& text, lox::Lox& lox );

    /** ********************************************************************************************
     * Read dry-run options. This utility method accepts options as given with enumeration
     * \alib{cli,DryRunModes}.
     *
     * If no argument is set in \p{dryOpt}, the next argument is checked to
     * be an option. If yes, the argument is consumed and stored in \p{dryOpt}.
     * If no argument is set or successfully peeked, \alib{cli,DryRunModes::Application} is chosen.
     *
     * @param cliApp    The application object.
     * @param dryOpt    The option object parsed.
     * @return \c true on success. \c false if an argument was given that is not recognized.
     **********************************************************************************************/
    static ALIB_API
    bool GetDryOpt( CLIApp& cliApp, Option& dryOpt );

    /** ********************************************************************************************
     * Dumps the configuration.
     * Shows which commands, options, parameters and errors are set with enums and their
     * meta info.
     * Useful during development.
     *
     * @param cliApp    The friend object we work on.
     * @param text      The target text.
     * @return An internal \c AString object containing the dump text. (Beware of concurrent
     *         debugging threads :-)
     **************************************************************************************************/
    static ALIB_API
    AString&     DumpDeclarations( CLIApp& cliApp, SimpleText& text );

    /** ********************************************************************************************
     * Write in human readable form, which commands and options have been read from the
     * command line.
     *
     * This is useful for debugging as well as to implement a "dry run" option of the
     * CLI application, that offers the user a list of what is parsed with a given set of
     * CLI arguments. In this case, method read \alib{cli,CLIApp::ReadNextCommands} should
     * be invoked after the provisions of the various definitions.
     *
     * Probably, depending on the command syntax, not all commands can be parsed prior
     * to executing them. However, options can.
     *
     * @param cliApp    The friend object we work on.
     * @param text      The target text.
     * @returns Returns an internal \c AString object containing the dump text.
     *          (Beware of concurrent debugging threads :-)
     **************************************************************************************************/
    static ALIB_API
    AString&     DumpParseResults( CLIApp& cliApp, SimpleText& text );
};

}} // namespace lib::system

/// Type alias in namespace #aworx.
using     CLIUtil=           aworx::lib::cli::CLIUtil;


}  // namespace aworx



#endif // HPP_ALIB_CLI_CLIUTIL
