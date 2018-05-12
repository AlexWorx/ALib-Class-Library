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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_CLI_LIB)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_CLI_LIB
//! @cond NO_DOX
#define HPP_ALIB_CLI_LIB 1
//! @endcond

namespace aworx { namespace lib { namespace  cli {

class CLIApp;


/** ************************************************************************************************
 * The library class for module \alibmod_nolink_cli.
 *
 * If compilation symbol \ref ALIB_MODULE_CLI is set, library \ref aworx::lib::ALIB will
 * create a dependency to this library's singleton \ref aworx::lib::CLI.
 **************************************************************************************************/
class CLILib  : public lib::lang::Library
{
    public:
        /** ****************************************************************************************
         * Constructor.
         ******************************************************************************************/
        ALIB_API                CLILib();

    protected:
        /** ****************************************************************************************
         * Initializes library module \alibmod_nolink_cli.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        ALIB_API virtual void   init( Phases phase )                                           override;

        /** ****************************************************************************************
         * Frees resources of the \alib \c string namespace.
         ******************************************************************************************/
        ALIB_API virtual void   terminationCleanUp()                                           override;

}; // class CLILib

} // namespace aworx::lib[::cli]

/** ************************************************************************************************
 * The static library singleton of \alibmod_nolink_cli.
 **************************************************************************************************/
ALIB_API
extern cli::CLILib CLI;

namespace cli {

/** ************************************************************************************************
 * Exceptions of library #aworx::lib::cli.
 * As usual with class \alib{lang,Exception}, some of the exceptions are "inner exceptions" that
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

}}} // namespace [aworx::lib::cli]

ALIB_LANG_EXCEPTIONS(    aworx::lib::cli::Exceptions , aworx::lib::CLI, ASTR("Exceptions") )
ALIB_LANG_ENUM_PARSABLE( aworx::lib::cli::DryRunModes )
ALIB_LANG_RESOURCED(     aworx::lib::cli::DryRunModes, aworx::lib::CLI, ASTR("DryRunModes"))





#endif // HPP_ALIB_CLI_LIB
