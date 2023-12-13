/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_cli of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CLI_CLI
#define HPP_ALIB_CLI_CLI 1

#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

#if !defined (HPP_ALOX)
#   include "alib/alox.hpp"
#endif

#if !defined(HPP_ALIB_FS_MODULES_MODULE)
#   include "alib/lib/fs_modules/module.hpp"
#endif

namespace aworx { namespace lib { namespace  cli {

class CommandLine;


/** ************************************************************************************************
 * The module class for module \alib_cli_nl.
 *
 * This is a strict singleton class. The only instance is found with namespace variable
 * \ref aworx::lib::CLI.
 **************************************************************************************************/
class Cli : public Module
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_module_singletons).
         ******************************************************************************************/
        Cli();

    protected:
        /** ****************************************************************************************
         * Initializes module module \alib_cli_nl.
         *
         * @param phase  The initialization phase to perform.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void    bootstrap( BootstrapPhases phase,
                                   int argc, const char** argv, const wchar_t** wargv )    override;

        /** ****************************************************************************************
         * Terminates this module. (Nothing to do.)
         * @param phase  The shutdown phase to perform.
         ******************************************************************************************/
        virtual void    shutdown( ShutdownPhases phase )                                 override
        { (void) phase; }


}; // class CLILib

/** ************************************************************************************************
 * Exceptions of module #aworx::lib::cli.
 * As usual with class \alib{results,Exception}, some of the exceptions are "inner exceptions" that
 * are caught internally and re-thrown with more information and a different exception code.
 *
 * The in this respect "external" exceptions that have to be caught by users of the library, are:
 *
 * - \b %NoCommandGiven,
 * - \b %UnknownCommand,
 * - \b %ParsingOptions and
 * - \b %ParsingCommand.
 **************************************************************************************************/
enum class Exceptions
{
    // main exceptions  to be handled by the user
    NoCommandGiven                = 1, ///< Unknown command given.
    UnknownCommand                = 2, ///< Unknown command given.
    ParsingOptions                =10, ///< General option parse error. Adds option help text.
    ParsingCommand                =20, ///< General parameter parse error. Adds command help text.

    // inner exceptions
    MissingOptionValue            =11, ///< Missing argument when reading option.
                                       ///< (Will be generalized with ParsingOptions.)
    IllegalOptionNameContinuation =12, ///< An option was given in long name, but continued after
                                       ///< its name in an undefined way.
                                       ///< (Will be generalized with ParsingOptions.)

    MissingParameterValue         =21, ///< Missing argument when reading parameter.
                                       ///< (Will be generalized with ParameterError.)

    END_OF_ENUM
};

/** ************************************************************************************************
 * Dry run modes.
 **************************************************************************************************/
enum class DryRunModes
{
    Off            , ///< No dry run
    CLIArgs        , ///< Displays the results of the command line argument parsing.
    CLIDeclarations, ///< Just displays the cli setup. This is more for debugging or interested users.
    Application    , ///< Dry run on application level: Given commands are invoked, nothing is
                     ///< executed, but probably log information is written. Application specific behavior.
                     ///< This is the default if no argument is given to the dry-run opton
};


} // namespace aworx::lib[::cli]

/** The singleton of module-class \alib{cli,Cli}. */
extern ALIB_API cli::Cli CLI;

}} // namespace [aworx::lib]

ALIB_ENUMS_ASSIGN_RECORD(   aworx::lib::cli::DryRunModes, aworx::lib::enums::ERSerializable )
ALIB_BOXING_VTABLE_DECLARE( aworx::lib::cli::Exceptions , vt_cli_exceptions )
ALIB_ENUMS_ASSIGN_RECORD(   aworx::lib::cli::Exceptions , aworx::lib::results::ERException  )
ALIB_RESOURCED_IN_MODULE(   aworx::lib::cli::Exceptions , aworx::lib::CLI, "E"     )

#endif // HPP_ALIB_CLI_CLI
