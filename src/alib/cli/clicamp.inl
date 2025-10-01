//==================================================================================================
/// \file
/// This header-file is part of module \alib_cli of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace  cli {

class CommandLine;

//==================================================================================================
/// The module class for module \alib_cli_nl.
///
/// This is a strict singleton class. The only instance is found with the namespace variable
/// \ref alib::CLI.
//==================================================================================================
class CliCamp : public camp::Camp
{
    public:
        //==========================================================================================
        /// Constructor.<br>
        /// While this is public, it must not be invoked as this is a strict singleton type.
        /// (See notes in \ref alib_camp_camp).
        //==========================================================================================
        CliCamp();

    protected:
        //==========================================================================================
        /// Initializes this camp.
        //==========================================================================================
        virtual void    Bootstrap()                                                        override;

        //==========================================================================================
        /// Terminates this camp. (Nothing to do.)
        /// @param phase  The shutdown phase to perform.
        //==========================================================================================
        virtual void    Shutdown( ShutdownPhases phase )                override   { (void) phase; }

}; // class CliCamp

//==================================================================================================
/// Exceptions of module #alib::cli.
/// As usual with class \alib{exceptions;Exception}, some of the exceptions are "inner exceptions" that
/// are caught internally and re-thrown with more information and a different exception code.
///
/// The in this respect "external" exceptions that have to be caught by users of the library, are:
///
/// - \b %NoCommandGiven,
/// - \b %UnknownCommand,
/// - \b %ParsingOptions and
/// - \b %ParsingCommand.
//==================================================================================================
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
};

//==================================================================================================
/// Dry run modes.
//==================================================================================================
enum class DryRunModes
{
    Off            , ///< No dry run
    CLIArgs        , ///< Displays the results of the command line argument parsing.
    CLIDeclarations, ///< Just displays the cli setup. This is more for debugging or interested users.
    Application    , ///< Dry run on application level: Given commands are invoked, nothing is
                     ///< executed, but probably log information is written. Application specific behavior.
                     ///< This is the default if no argument is given to the dry-run opton
};


} // namespace alib[::cli]

/// The singleton instance of \alibcamp class \alib{cli;CliCamp}.
extern ALIB_DLL cli::CliCamp CLI;

} // namespace [alib]

ALIB_ENUMS_ASSIGN_RECORD(   alib::cli::DryRunModes, alib::enumrecords::ERSerializable )
ALIB_BOXING_VTABLE_DECLARE( alib::cli::Exceptions , vt_cli_exceptions )
ALIB_ENUMS_ASSIGN_RECORD(   alib::cli::Exceptions , alib::exceptions::ERException  )
ALIB_RESOURCED_IN_MODULE(   alib::cli::Exceptions , alib::CLI, "E"     )
