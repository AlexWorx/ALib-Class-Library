// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_CLI_CLI
#define HPP_ALIB_CLI_CLI 1


#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

namespace aworx { namespace lib { namespace  cli {

class CLIApp;


/** ************************************************************************************************
 * The module class for module \alibmod_nolink_cli.
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
         * (See notes in \ref alib_manual_bootstrapping_class_modsingletons).
         ******************************************************************************************/
        Cli();

    protected:
        /** ****************************************************************************************
         * Initializes module module \alibmod_nolink_cli.
         *
         * @param level  The initialization level to reach.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        ALIB_API virtual
        void                    init( InitLevels level,
                                      int argc, const char** argv, const wchar_t** wargv ) override;

        /** ****************************************************************************************
         * Frees resources of the \alib \c string namespace.
         ******************************************************************************************/
        ALIB_API virtual
        void                    terminationCleanUp()                                       override;

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
    Off                 , ///< No dry run
    CLIArgs             , ///< Displays the results of the command line argument parsing.
    CLIDeclarations      , ///< Just displays the cli setup. This is more for debugging or interested users.
    Application         , ///< Dry run on application level: Given commands are invoked, nothing is
                          ///< executed. Application specific behaviour.
                          ///< Probably log information is written.
};


} // namespace aworx::lib[::cli]

/** The singleton of module-class \alib{cli,Cli}. */
extern ALIB_API cli::Cli CLI;

}} // namespace [aworx::lib]

ALIB_EXCEPTIONS(    aworx::lib::cli::Exceptions , aworx::lib::CLI, "Exceptions" )
ALIB_ENUM_PARSABLE( aworx::lib::cli::DryRunModes )
ALIB_RESOURCED_IN_MODULE(aworx::lib::cli::DryRunModes, aworx::lib::CLI, "DryRunModes")

ALIB_BOXING_VTABLE_DECLARE( aworx::lib::cli::Exceptions, vt_cli_exceptions )




#endif // HPP_ALIB_CLI_CLI
