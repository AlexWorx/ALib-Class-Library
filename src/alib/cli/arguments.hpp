// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_CLI_ARGUMENTS
#define HPP_ALIB_CLI_ARGUMENTS 1

#if !defined (HPP_ALIB_CLI_CLI)
    #include "alib/cli/cli.hpp"
#endif


namespace aworx { namespace lib { namespace  cli {

class CLIApp;

// #################################################################################################
// Custom enum meta data type definitions
// #################################################################################################

// forward definition of declaration types. Needed to use meta data macro
struct ParameterDecl;
struct CommandDecl;
struct OptionDecl;
struct ExitCodeDecl;

}}} // namespace [aworx::lib:cli]

ALIB_ENUM_SPECIFICATION_DECL( aworx::lib::cli::CommandDecl  , String,int,String )
ALIB_ENUM_SPECIFICATION_DECL( aworx::lib::cli::OptionDecl   , String,int,String,String,size_t,bool,String )
ALIB_ENUM_SPECIFICATION_DECL( aworx::lib::cli::ParameterDecl, String,String,int,String,nchar,int,int )
ALIB_ENUM_SPECIFICATION_DECL( aworx::lib::cli::ExitCodeDecl , String,int )

namespace aworx { namespace lib { namespace  cli {

/**
 * Struct used as parent for types
 * - \alib{cli,CommandDecl},
 * - \alib{cli,OptionDecl},
 * - \alib{cli,ParameterDecl} and
 * - \alib{cli,ExitCodeDecl}.
 *
 * Stores run-time information of user-defined enum types.
 *
 * Construction is done by passing a custom enum element of an enum type equipped with
 * \alib{resources,T_EnumMetaDataDecl,enum meta data} of a distinct type.
 *
 * Such enums are announced to <b>%ALib CLI</b> using macros
 * - \ref ALIB_CLI_COMMANDS,
 * - \ref ALIB_CLI_OPTIONS,
 * - \ref ALIB_CLI_PARAMETERS and
 * - \ref ALIB_CLI_EXIT_CODES,
 *
 * When bootstrapping \alibmod_nolink_cli, method \alib{cli,CLIApp::DefineExitCodes} has to be
 * invoked for (each) enum type.
 *
 * @tparam TEMD The type of the enum meta data tuple.
 */
template<typename TEMD>
struct ArgumentDecl
{
    /** The meta data tuple type. */
    using TTuple = typename resources::T_EnumMetaDataSpecification<TEMD>::TTuple;

    Enum                  ID;                ///< The enum element boxed in class \b %Enum.
    TTuple&               Tuple;             ///< The resource tuple of the enum element.
    resources::Resources& Resources;         ///< The resources associated with the type \p{TEnum}.
                                             ///< Used to load dependent  resources.
    NString               ResourcesCategory; ///< The resource category within #Resources.
                                             ///< Used to load dependent  resources.

    /**
     * Templated constructor which takes an enum element of a custom enum type (enabled as
     * described in class documentation).
     *
     * Stores the enum code, the tuple and the resource module.
     *
     * @param element   The enum element
     * @tparam TEnum    C++ enum type enabled with \ref ALIB_CLI_EXIT_CODES.
     */
    template<typename TEnum>
    inline
    ArgumentDecl        ( TEnum element )
    : ID                ( element )
    , Tuple             ( *EnumMetaData<TEnum>::GetSingleton().Get( element ) )
    , Resources         ( *lib::resources::T_Resourced<TEnum>::Resource() )
    , ResourcesCategory (  lib::resources::T_Resourced<TEnum>::Category() )
    {}
};

/**
 * Struct used as parent for types
 * - \alib{cli,Command},
 * - \alib{cli,Option} and
 * - \alib{cli,Parameter}.
 *
 * Stores
 * - a pointer to the parent application,
 * - the position in \alib{cli,CLIApp::ArgNOriginal}, respectively \alib{cli,CLIApp::ArgWOriginal}
 *   where the object was found and
 * - number of arguments consumed when reading the object.
 *
 * \note
 *   For technical reasons, other members that are shared between the derived types named above,
 *   have to be declared (each three times) with the types themselves.
 */
struct ArgumentDef
{
    /** The cli application. */
    CLIApp*                      Parent;

    /** The index in \alib{cli,CLIApp::ArgNOriginal}, respectively \alib{cli,CLIApp::ArgWOriginal}
     *  that this instance was found. */
    size_t                       Position;

    /** The number of command line arguments that a command consumed. This includes the command name
        itself. Not essential, to whom it may concern...                                          */
    size_t                       QtyArgsConsumed;

    /**
     * Constructor
     * @param parent   The application.
     */
    inline
    ArgumentDef( CLIApp* parent )
    : Parent         (parent)
    , Position       ((std::numeric_limits<size_t>::max)())
    , QtyArgsConsumed(0)
    {}

};


// #################################################################################################
// Decl and Def versions of commands, options, parameters and ExitCodes
// #################################################################################################



/**
 * A parameter of a \alib{cli,CommandDecl}.
 *
 * Construction is done by passing a custom enum element of an enum type equipped with
 * \alib{resources,T_EnumMetaDataDecl,enum meta data} of a distinct type. Such enums are announced
 * to <b>%ALib CLI</b> using macro \ref ALIB_CLI_PARAMETERS.
 *
 * When bootstrapping \alibmod_nolink_cli, method \alib{cli,CLIApp::DefineParameters} has to be
 * invoked for (each) enum type.
 *
 *
 * The tuple elements have the following meaning:
 *
 * Index | Description
 * ------|-------------
 *   0   | The underlying integer value of the enum element
 *   1   | The name of the parameter (usually same as C++ enum element name).
 *   2   | The identifier of the parameter.
 *   3   | The minimum amount of characters to parse to accept the option.
 *   4   | An optional separator string (usually "=") that separates the parameter name from a value within the parameter itself.
 *   5   | A separator character for parsing multiple values.
 *   6   | The number of arguments to consume and store in \alib{cli,Parameter::Args}. if negative, parsing stops. If previous field, separator string is set and this value is equal or greater to \c 1, then a missing separator string leads to a parsing exception.
 *   7   | Denotes if this is an optional parameter
 *
 */
struct ParameterDecl : public ArgumentDecl<ParameterDecl>
{
    /**
     * Templated constructor which takes an enum element of a custom type that was enabled
     * using \ref ALIB_CLI_PARAMETERS.
     *
     * Stores the enum code, the tuple and the resource module.
     *
     * @param element   The enum element
     * @tparam TEnum    C++ enum type enabled with \ref ALIB_CLI_PARAMETERS.
     */
    template<typename TEnum>
    inline
    ParameterDecl( TEnum element )
    : ArgumentDecl<ParameterDecl>( element )
    {
        // fix separator character
        if( Separator() == 'C' )
            std::get<3>( Tuple )= ',';
    }
    /**
     * Returns the name of the parameter. This is not the identifier. The name is just for
     * help and configuration output.
     *
     * \see Method #Identifier.
     *
     * @return The name of the enum element.
     */
    inline String  Name()
    {
        return std::get<1>( Tuple );
    }

    /**
     * Returns the identifier of the parameter. If this is empty, the parameter is not optional
     * and hence has no identifier.
     *
     * @return The name of the enum element.
     */
    inline String  Identifier()
    {
        return std::get<2>( Tuple );
    }

    /**
     * Returns the minimum parse length of the identifier.
     * @return The minimum characters to satisfy the parameter.
     */
    inline int      MinimumParseLen()
    {
        return std::get<3>( Tuple );
    }

    /**
     * An optional separator string (usually "="), as described in table above.
     * @return The parameter identifier.
     */
    inline String   InArgSeparator()
    {
        return std::get<4>( Tuple );
    }



    /**
     * Returns the separator character (in case of multiple values).
     * @return The separator character.
     */
    inline nchar Separator()
    {
        return std::get<5>( Tuple ) != 'C' ? std::get<5>( Tuple ): ',';
    }

    /**
     * The number of CLI arguments to consume and store in \alib{cli,Option::Args} with method
     * \alib{cli,Parameter::Read}.
     *
     * @return The parameter identifier.
     */
    inline int   QtyExpectedArgsFollowing()
    {
        return std::get<6>( Tuple );
    }

    /**
     * Returns \c true if the parameter is optional. The information about this attribute is
     * used to create help messages and usage format strings only. It does not automatically
     * raise an exception if a parameter is not given. Such exception or other error treatment
     * is up to the user code.
     * @return \c true if the parameter is optional, \c false otherwise.
     */
    inline bool IsOptional()
    {
        return std::get<7>( Tuple );
    }

    /**
     * Returns the short help text.
     * Loads the string from #Resources using resource name \c "THelpParShtNN",
     * where \c NN is the enum element integer value.
     * @return The help text.
     */
    inline String  GetHelpTextShort()
    {
        return Resources.Get( ResourcesCategory, NString64("THlpParSht_" ) << Name() ALIB_DBG(, true));
    }

    /**
     * Returns the long help text.
     * Loads the string from #Resources using resource name \c "THelpParLngNN",
     * where \c NN is the enum element integer value.
     * @return The help text.
     */
    inline String  GetHelpTextLong()
    {
        return Resources.Get( ResourcesCategory, NString64("THlpParLng_" ) << Name() ALIB_DBG(, true));
    }
};

/**
 * A declaration for a \alib{cli::Parameter}.
 */
struct Parameter : public ArgumentDef
{
    /** Expose parent class's constructor. */
    using ArgumentDef::ArgumentDef;

    /** The underlying declaration. */
    ParameterDecl*      Declaration                                                       = nullptr;

    /** Arguments belonging to us. */
    std::vector<String>     Args;

    /**
     * Tries to reads the object from the front of \alib{cli,CLIApp::ArgsLeft}.
     *
     * The \c true on success.
     * If it could not be decided if the actual CLI argument contains this parameter \c false
     * is returned to indicate that parsing commands has to stop now.
     *
     * This is done in the following cases:
     * - When \alib{cli,ParameterDecl::Identifier} is empty and the parameter is
     *   \alib{cli,ParameterDecl::IsOptional} gives \c true.
     * - When it was successfully read, but  \alib{cli,ParameterDecl::QtyExpectedArgsFollowing}
     *   is defined \c -1.
     *
     * See \alib{cli,CLIApp,ReadNextCommands} for details
     *
     * @param decl   The declaration used for reading
     * @return The \c true on success, \c false indicates that parsing has to end here.
     */
    ALIB_API
    bool    Read( ParameterDecl& decl );
};


/**
 * A declaration for an \alib{cli::Option}.
 *
 * Construction is done by passing a custom enum element of an enum type equipped with
 * \alib{resources,T_EnumMetaDataDecl,enum meta data} of a distinct type. Such enums are announced
 * to <b>%ALib CLI</b> using macro \ref ALIB_CLI_OPTIONS.
 *
 * When bootstrapping \alibmod_nolink_cli, method \alib{cli,CLIApp::DefineOptions} has to be
 * invoked for (each) enum type.
 *
 * The tuple elements have the following meaning:
 *
 * Index | Description
 * ------|-------------
 *   0   | The underlying integer value of the enum element
 *   1   | The name of the option as parsed from command line if double hyphen <c>'--'</c> is used.
 *   2   | The minimum amount of characters to parse to accept the option.
 *   3   | The name of the option as parsed from command line if single hyphen <c>'-'</c> is used. Defined as string to be able to have empty strings, which disables single character options.
 *   4   | An optional separator string (usually "=") that separates the option name from a value within the first argument itself. If this is given, the number of arguments to consume should be \c 0.
 *   5   | The number of arguments to consume and store in \alib{cli,Option::Args}. If previous field, separator string is set and this value is equal or greater to \c 1, then a missing separator string leads to a parsing exception.
 *   6   | If \c true, a next occurrence overwrites previous. Overwritten options are stored in \alib{cli,CLIApp::OptionsOverwritten}. Otherwise, multiple occurrencies are stored.
 *   7   | If not empty, the argument string will be replaced by this and search for next options continue. Note: Shortcut options have to occur earlier in the enum resource table!
 *
 */
struct OptionDecl : public ArgumentDecl<OptionDecl>
{
    /** Expose parent class's constructor. */
    using ArgumentDecl<OptionDecl>::ArgumentDecl;


    /**
     * Returns the identifier of the option if double hyphen <c>'--'</c> is used.
     * @return The option identifier.
     */
    inline String   Identifier()
    {
        return std::get<1>( Tuple );
    }

    /**
     * Returns the minimum parse length of the identifier if double hyphen <c>'--'</c> is used.
     * @return The minimum characters to satisfy the option.
     */
    inline int      MinimumParseLen()
    {
        return std::get<2>( Tuple );
    }

    /**
     * Returns the identifier of the option if single hyphen <c>'-'</c> is used.
     * @return The option identifier.
     */
    inline character    IdentifierChar()
    {
        return std::get<3>( Tuple ).IsNotEmpty() ? std::get<3>( Tuple ).CharAtStart() : '\0';
    }

    /**
     * An optional separator string (usually "="), as described in table above.
     * @return The option identifier.
     */
    inline String   InArgSeparator()
    {
        return std::get<4>( Tuple );
    }

    /**
     * The number of CLI arguments to consume and store in \alib{cli,Option::Args} with method
     * \alib{cli,Option::Read}.
     * @return The option identifier.
     */
    inline size_t   QtyExpectedArgsFollowing()
    {
        return std::get<5>( Tuple );
    }

    /**
     * Denotes whether multiple occurrences of the option in the CLI argument list are collected
     * and stored in the vector of \alib{cli,CLIApp::OptionsFound}, or if a subsequent occurrence
     * replaces the previous one, which get moved to \alib{cli,CLIApp::OptionArgsIgnored}.
     *
     * @return \c true, if only the last option given is used..
     */
    bool            MultiIgnored()
    {
        return std::get<6>( Tuple );
    }

    /**
     * If an option is a shortcut to another one, this string replaces the argument given.
     * @return The option identifier.
     */
    inline String   ShortcutReplacementString()
    {
        return std::get<7>( Tuple );
    }


    /**
     * Returns a formal description of the usage.
     * Loads the string from #Resources using resource name \c "TOptUsgNN",
     * where \c NN is the enum element integer value.
     * @return The help text.
     */
    inline String  HelpUsageLine()
    {
        return Resources.Get( ResourcesCategory, NString64("TOptUsg_" ) << Identifier() ALIB_DBG(, true));
    }

    /**
     * Returns the help text.
     * Loads the string from #Resources using resource name \c "TOptHlpNN",
     * where \c NN is the enum element integer value.
     * @return The help text.
     */
    inline String  HelpText()
    {
        return Resources.Get( ResourcesCategory, NString64("TOptHlp_" ) << Identifier() ALIB_DBG(, true));
    }


    protected:
        /**
         * Called from the constructor. Parses tuple value denoting the parameters and
         * loads them.
         */
        ALIB_API
        void addParamDecls();
};

/**
 * An option of a cli application. Options are read "automatically" using their declaration
 * information defined with the resources accessed through the enum meta data of enum elements
 * of custom types.
 *
 * However, such automatic read is limited due to the fact, that the simple values and flags
 * defined with \alib{cli,OptionDecl}, can not provide the flexibility needed to perfectly
 * parse options with a complex syntax.
 *
 * In this case, the way out is to use custom code that invokes \alib{cli,CLIApp,ReadOptions}
 * and then processes all options that may have remaining arguments left in the list.
 * Using field #Position further arguments may be consumed from \alib{cli,CLIApp::ArgsLeft}.<br>
 * Note, "processing all options" may mean a nested loop. The outer is over the option types
 * of  \alib{cli,CLIApp,OptionsFound}, the inner is over the vector of options per type.
 *
 */
struct Option : public ArgumentDef
{
    /** Arguments belonging to this option. */
    std::vector<String>     Args;

    /** The declaration struct. */
    OptionDecl*             Declaration                                                   = nullptr;

    /** Expose parent class's constructor. */
    using ArgumentDef::ArgumentDef;

    /**
     * Tries to reads the object from the current CLI arg(s).
     * \note
     *   Unlike the read methods \alib{cli,Command::Read} and \alib{cli,Parameter::Read}, this
     *   method expects the argument to test not only by number with \p{argNo} but as well
     *   with string parameter \p{arg}.<br>
     *   This redundancy is needed is needed to easily implement shortcut options, that just
     *   replace a shortcut option read to another one, probably with a preset argument included.
     *
     * @param decl   The declaration used for reading.
     * @param arg    The argument string starting with one or two hyphens.
     * @param argNo  The position of reading.
     * @return The \c true on success, \c false otherwise.
     */
    ALIB_API
    bool  Read( OptionDecl& decl, String& arg, const size_t argNo );
};


/**
 * A declaration for a \alib{cli::Command}.
 *
 * Construction is done by passing a custom enum element of an enum type equipped with
 * \alib{resources,T_EnumMetaDataDecl,enum meta data} of a distinct type. Such enums are announced
 * to <b>%ALib CLI</b> using macro \ref ALIB_CLI_COMMANDS.
 *
 * When bootstrapping \alibmod_nolink_cli, method \alib{cli,CLIApp::DefineCommands} has to be
 * invoked for (each) enum type.
 *
 * The tuple elements have the following meaning:
 *
 * Index | Description
 * ------|-------------
 *   0   | The underlying integer value of the enum element
 *   1   | The name of the command as parsed from command line.
 *   2   | The minimum amount of characters to be parsed to accept the command.
 *   3   | List of parameters attached. Separated by <c>'/'</c>.
 *
 */
struct CommandDecl : public ArgumentDecl<CommandDecl>
{
    /** The cli application we are belong to */
    CLIApp&                         Parent;

    /** Command parameters. */
    std::vector<ParameterDecl*>     Parameters;


    /**
     * Templated constructor which takes an enum element of a custom type that was enabled
     * using \ref ALIB_CLI_COMMANDS.<br>
     * Stores the enum code, the tuple and the resource module.
     *
     * Furthermore, parameters are added, as specified in the resource table entry.
     *
     * @param element   The enum element
     * @param parent    The application object. Will be stored.
     * @tparam TEnum    C++ enum type enabled with \ref ALIB_CLI_COMMANDS.
     */
    template<typename TEnum>
    inline
    CommandDecl( TEnum element, CLIApp& parent )
    : ArgumentDecl<CommandDecl>( element )
    , Parent    (parent)

    {
        addParamDecls();
    }


    /**
     * Returns the identifier (name) of the command
     * @return The command identifier.
     */
    inline String  Identifier()
    {
        return std::get<1>( Tuple );
    }

    /**
     * Returns the minimum parse length of the identifier.
     * @return The minimum characters to satisfy the command to be parsed.
     */
    inline int      MinimumParseLen()
    {
        return std::get<2>( Tuple );
    }

    /**
     * Returns the short version of the help text.
     * Loads the string from #Resources using resource name \c "THlpCmdShtNN",
     * where \c NN is the enum element integer value.
     * @return The help text.
     */
    inline String  HelpTextShort()
    {
        return Resources.Get( ResourcesCategory, NString64("THlpCmdSht_" ) << Identifier() ALIB_DBG(, true));
    }

    /**
     * Returns the long version of the help text.
     * Loads the string from #Resources using resource name \c "THlpCmdLngNN",
     * where \c NN is the enum element integer value.
     * @return The help text.
     */
    inline String  HelpTextLong()
    {
        return Resources.Get( ResourcesCategory, NString64("THlpCmdLng_" ) << Identifier() ALIB_DBG(, true));
    }

    /**
     * Searches in #Parameters for the declaration of parameter \p{name}.
     * @param name   The declaration name of the parameter.
     * @return A pointer to the parameter's declaration, \c nullptr if parameter was not declared.
     */
    ALIB_API
    ParameterDecl* GetParameterDecl(const String& name );


    private:
        /**
         * Called from the constructor. Parses tuple value denoting the parameters and
         * loads them.
         */
        ALIB_API
        void addParamDecls();
};

/**
 * A command of a \alibmod_nolink_cli application.
 */
struct Command  : public ArgumentDef
{
    /** Mandatory parameters parsed. */
    std::vector<Parameter>      ParametersMandatory;

    /** Optional parameters parsed. */
    std::vector<Parameter>      ParametersOptional;

    /** The underlying declaration. */
    CommandDecl*                Declaration                                               = nullptr;

    /** Expose parent class's constructor. */
    using ArgumentDef::ArgumentDef;

    /**
     * Tries to read the object from the front of \alib{cli,CLIApp::ArgsLeft}.
     * @param decl   The declaration used for reading.
     * @return The \c true on success, \c false otherwise.
     */
    ALIB_API
    bool  Read( CommandDecl& decl );

    /**
     * Searches in #ParametersMandatory and #ParametersOptional for parameter \p{name}.
     * @param name   The declaration name of the parameter.
     * @return A pointer to the parameter, \c nullptr if parameter was not parsed.
     */
    ALIB_API
    Parameter* GetParsedParameter(const String& name );

    /**
     * Searches in #ParametersMandatory and #ParametersOptional for parameter \p{name} and returns
     * its (first) argument.
     * @param name   The declaration name of the parameter.
     * @return The argument string, \b NullString()  if parameter was not parsed if not given.
     */
    ALIB_API
    String GetParsedParameterArg( const String& name );
};


/**
 * An exit code of the cli application.
 *
 * Construction is done by passing a custom enum element of an enum type equipped with
 * \alib{resources,T_EnumMetaDataDecl,enum meta data} of a distinct type. Such enums are announced
 * to <b>%ALib CLI</b> using macro \ref ALIB_CLI_EXIT_CODES.
 *
 * When bootstrapping \alibmod_nolink_cli, method \alib{cli,CLIApp::DefineExitCodes} has to be
 * invoked for (each) enum type.
 *
 * The tuple elements have the following meaning:
 *
 * Index | Description
 * ------|-------------
 *   0   | The underlying integer value of the enum element
 *   1   | The name of the exit code (usually same as C++ enum element name.
 *   2   | The CLI module exception associated to this exit code.
 *
 * Announcing the main application's exit codes to the \alibmod_nolink_cli module has two reasons:
 * - The exit codes are included in the help output text utility methods provided by class
 *   \alib{cli,CLIApp}.
 * - \alibmod_nolink_cli module exit codes can be translated to valid exit codes using
 *   method \alib{cli,CLIUtil::GetExitCode}.
 */
struct ExitCodeDecl : public ArgumentDecl<ExitCodeDecl>
{
    /** Expose parent class's constructor. */
    using ArgumentDecl<ExitCodeDecl>::ArgumentDecl;

    /**
     * Returns the name of the enum element
     * @return The name of the enum element.
     */
    inline String  Name()
    {
        return std::get<1>( Tuple );
    }


    /**
     * Returns the format string associated with this exit code.
     * Loads the string from #Resources using resource name \c "TExitNN",
     * where \c NN is the enum element integer value.
     * @return The format string.
     */
    inline String  FormatString()
    {
        return Resources.Get( ResourcesCategory, NString64("TExit" ) << ID.Value() ALIB_DBG(, true));
    }

    /**
     * If an element of enum type \alib{cli,Exceptions} is associated with this exit code, it
     * is returned. Otherwise <c>cli::ExitCodes(-1)</c>.
     *
     * \see Method \alib{cli,CLIUtil::GetExitCode}.
     * @return The associated element of cli::Exceptions.
     */
    cli::Exceptions AssociatedCLIException()
    {
        return cli::Exceptions( std::get<2>( Tuple ) );
    }
};

}}} // namespace [aworx::lib::cli]


// #################################################################################################
// Preprocessor definitions to declare enums as to be used as CLI types
// #################################################################################################


#define ALIB_CLI_COMMANDS( TEnum, TModule, ResourceName )                                          \
ALIB_RESOURCED_IN_MODULE( TEnum, TModule, ResourceName)                                            \
ALIB_ENUM_SPECIFICATION( aworx::lib::cli::CommandDecl, TEnum )

#define ALIB_CLI_PARAMETERS( TEnum, TModule, ResourceName )                                        \
ALIB_RESOURCED_IN_MODULE( TEnum, TModule, ResourceName)                                            \
ALIB_ENUM_SPECIFICATION( aworx::lib::cli::ParameterDecl, TEnum )

#define ALIB_CLI_OPTIONS( TEnum, TModule, ResourceName )                                           \
ALIB_RESOURCED_IN_MODULE( TEnum, TModule, ResourceName)                                            \
ALIB_ENUM_SPECIFICATION( aworx::lib::cli::OptionDecl, TEnum )

#define ALIB_CLI_EXIT_CODES( TEnum, TModule, ResourceName )                                        \
ALIB_RESOURCED_IN_MODULE( TEnum, TModule, ResourceName)                                            \
ALIB_ENUM_SPECIFICATION( aworx::lib::cli::ExitCodeDecl, TEnum )



#endif // HPP_ALIB_CLI_ARGUMENTS
