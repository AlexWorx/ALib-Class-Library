/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_cli of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CLI_ARGUMENTS
#define HPP_ALIB_CLI_ARGUMENTS 1

#if !defined (HPP_ALIB_CLI_CLI)
    #include "alib/cli/cli.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_LIST)
#   include "alib/monomem/list.hpp"
#endif


namespace aworx { namespace lib { namespace  cli {


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
struct Parsed
{
    /** The cli application. */
    CLIApp*     Parent;

    /** The index in \alib{cli,CLIApp::ArgNOriginal}, respectively \alib{cli,CLIApp::ArgWOriginal}
     *  that this instance was found. */
    integer     Position;

    /** The number of command line arguments that a command consumed. This includes the command name
        itself. Not essential, to whom it may concern...                                          */
    integer     QtyArgsConsumed;

    /**
     * Constructor
     * @param parent   The application.
     */
    Parsed( CLIApp* parent )
    : Parent         (parent)
    , Position       ((std::numeric_limits<integer>::max)())
    , QtyArgsConsumed(0)
    {}
};


// #################################################################################################
// Decl and Def versions of commands, options, parameters and ExitCodes
// #################################################################################################


/**
 * \ref alib_enums_records "ALib Enum Record" type used by class \alib{cli,ParameterDecl}.
 */
 struct ERParameterDecl : public enums::ERSerializable
 {
    /** The identifier of the parameter.    */
    String      identifier;

    /** An optional separator string (usually "=") that separates the parameter name from a
     *  value given within the parameter itself.                                                */
    String      valueSeparator;

    /** A separator character for parsing multiple values.
     *  If set to <c>'C'</c>, method \alib{cli,ParameterDecl::ValueListSeparator} will return
     *  <c>','</c> instead.                                                                     */
    nchar       valueListSeparator;

    /** The number of arguments to consume and store in \alib{cli,Parameter::Args}.
     *  If negative, parsing stops. If previous field, separator string is set and this
     *  value is equal or greater to \c 1, then a missing separator string leads to
     *  a parsing exception.                                                                    */
    int         qtyExpectedArgsFollowing;

    /** Denotes if this is an optional parameter. */
    bool        isOptional;

     /** Default constructor leaving the record undefined. (Implementation required as documented
      *  \alib{enums,EnumRecordPrototype::EnumRecordPrototype(),here}.)                           */
     ERParameterDecl()                                                          noexcept  = default;

    /** Implementation of \alib{enums,EnumRecordPrototype::Parse}.      */
    ALIB_API
    void Parse();
 };

/**
 * A parameter of a \alib{cli,CommandDecl}.
 *
 * Construction is done by passing a custom enum element of an enum type equipped with
 * \ref alib_enums_records "ALib Enum Records" of type \alib{cli,ERParameterDecl}.
 *
 * When bootstrapping \alib_cli_nl, method \alib{cli,CLIApp::DefineParameters} has to be
 * invoked for (each) enum type.
 */
class ParameterDecl
{
    protected:
        /** The enumeration element given with construction. */
        Enum            declElement;

        /** A copy (!) of the enum record. */
        ERParameterDecl record;

        /** The resource information of the enumeration type given with construction. */
        ResourceInfo    resourceInfo;

    public:
        /**
         * Templated constructor which takes an enum element of a custom type equipped with
         * \ref alib_enums_records "ALib Enum Records" of type \alib{cli,ERParameterDecl}.
         *
         * @tparam TEnum    C++ enum type equipped with corresponding \alib Enum Records.
         * @param element   The enum element
         */
        template<typename TEnum>
        ParameterDecl( TEnum element )
        : declElement( element )
        , resourceInfo(element)
        {
            // make a copy of the resourced record
            record= enums::GetRecord(element);

            // fix separator character
            if( record.valueListSeparator == 'C' )
                record.valueListSeparator= ',';
        }

       /**
        * Returns the type and integral value of the enumeration element used with construction.
        * @return The enumeration element used with construction.
        */
        const Enum&     Element()                                                              const
        {
            return declElement;
        }

        /**
         * Returns the name of the parameter. This is not the identifier. The name is just for
         * help and configuration output.
         *
         * \see Method #Identifier.
         *
         * @return The name of the enum element.
         */
        const String&   Name()
        {
            return record.EnumElementName;
        }

        /**
         * Returns the identifier of the parameter. If this is empty, the parameter is not optional
         * and hence has no identifier.
         *
         * @return The name of the enum element.
         */
        const String&   Identifier()
        {
            return record.identifier;
        }

        /**
         * Returns the minimum parse length of the identifier.
         * @return The minimum characters to satisfy the parameter.
         */
        int             MinimumRecognitionLength()
        {
            return record.MinimumRecognitionLength;
        }

        /**
         * An optional separator string (usually "="), that separates the parameter name from a
         * parameter value.
         * @return The separator string.
         */
        const String&   ValueSeparator()
        {
            return record.valueSeparator;
        }

        /**
         * A separator character for parsing multiple values.
         * @return The separator character.
         */
        nchar           ValueListSeparator()
        {
            return record.valueListSeparator != 'C' ? record.valueListSeparator : ',';
        }

        /**
         * The number of CLI arguments to consume and store in \alib{cli,Option::Args} with method
         * \alib{cli,Parameter::Read}.
         *
         * @return The parameter identifier.
         */
        int             QtyExpectedArgsFollowing()
        {
            return record.qtyExpectedArgsFollowing;
        }

        /**
         * Returns \c true if the parameter is optional. The information about this attribute is
         * used to create help messages and usage format strings only. It does not automatically
         * raise an exception if a parameter is not given. Such exception or other error treatment
         * is up to the user code.
         * @return \c true if the parameter is optional, \c false otherwise.
         */
        bool IsOptional()
        {
            return record.isOptional;
        }

        /**
         * Returns the short help text.
         * Loads the string from #resourceInfo using resource name \c "THelpParShtNN",
         * where \c NN is the enum element's integral value.
         * @return The help text.
         */
        const String&   GetHelpTextShort()
        {
            return resourceInfo.Get( NString64("THlpParSht_" ) << Name()  ALIB_DBG(, true) );
        }

        /**
         * Returns the long help text.
         * Loads the string from #resourceInfo using resource name \c "THelpParLngNN",
         * where \c NN is the enum element's integral value.
         * @return The help text.
         */
        const String&   GetHelpTextLong()
        {
            return resourceInfo.Get( String64("THlpParLng_" ) << Name() ALIB_DBG(, true));
        }
}; // ParameterDecl

/**
 * A declaration for a \alib{cli::Parameter}.
 */
struct Parameter : public Parsed
{
    /** The underlying declaration. */
    ParameterDecl*                  Declaration                                           = nullptr;

    /** Arguments belonging to us. */
    List<String, Recycling::Shared> Args;

    /**
     * Constructor
     * @param parent   The application.
     */
    inline
    Parameter( CLIApp* parent );

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
 * \ref alib_enums_records "ALib Enum Record" type used by class \alib{cli,OptionDecl}.
 */
struct EROptionDecl : public enums::ERSerializable
{
    /** The name of the option as parsed from command line if single hyphen <c>'-'</c> is used.
     *  Defined as string to be able to have empty strings, which disables single character
     *  options.                                                                                 */
    String  identifierChar;

    /** An optional separator string (usually "=") that separates the option name from a value
     *  within the first argument itself.
     *  If this is not given, field #qtyExpectedArgsFollowing has to be \c 0.                                                                          */
    String  valueSeparator;

    /** The number of arguments to consume and store in \alib{cli,Option::Args}.
     *  If field #qtyExpectedArgsFollowing is set and this value is not \c 0, then a missing
     *  separator string leads to a parsing exception.                                       */
    integer qtyExpectedArgsFollowing;

    /** If \c true, a next occurrence overwrites previous. This flag is not used by the
     *  default implementations given by the types of this module. Instead, when processing
     *  options, an application either uses \alib{cli,CLIApp::GetOption} to retrieve the last
     *  occurrence, or walks list \alib{cli,CLIApp::Options} by itself to fetch all occurrences.*/
    bool    multiIgnored;

    /** If not empty, the argument string will be replaced by this and the search for next options
     *  continues.
     *  Note: Shortcut options have to occur earlier in the enumeration's resource definition.  */
    String  shortcutReplacementString;

    /** Defaulted constructor leaving the record undefined.
     *  (Implementation required as documented
     *  \alib{enums,EnumRecordPrototype::EnumRecordPrototype(),here}.) */
    EROptionDecl()                                                              noexcept  = default;

    /** Implementation of \alib{enums,EnumRecordPrototype::Parse}.      */
    ALIB_API
    void Parse();
};

/**
 * A declaration for an \alib{cli::Option}.
 *
 * Construction is done by passing a custom enum element of an enum type equipped with
 * \ref alib_enums_records "ALib Enum Records" of type \alib{cli,EROptionDecl}.
 *
 * When bootstrapping \alib_cli_nl, method \alib{cli,CLIApp::DefineOptions} has to be
 * invoked for (each) enum type.
 *
 */
class OptionDecl
{
    protected:
        /** The enumeration element given with construction. */
        Enum            declElement;

        /** A copy (!) of the enum record. */
        EROptionDecl    record;

        /** The resource information of the enumeration type given with construction. */
        ResourceInfo    resourceInfo;


    public:
        /**
         * Templated constructor which takes an enum element of a custom type equipped with
         * \ref alib_enums_records "ALib Enum Records" of type \alib{cli,EROptionDecl}.
         *
         * @tparam TEnum    C++ enum type equipped with corresponding \alib Enum Records.
         * @param element   The enum element
         */
        template<typename TEnum>
        OptionDecl( TEnum element )
        : declElement( element )
        , resourceInfo(element)
        {
            // make a copy of the resourced record
            record= enums::GetRecord(element);
        }

       /**
        * Returns the type and integral value of the enumeration element used with construction.
        * @return The enumeration element used with construction.
        */
        const Enum&     Element()                                                              const
        {
            return declElement;
        }

        /**
         * Returns the identifier of the option if double hyphen <c>'--'</c> is used.
         * @return The option identifier.
         */
        const String&   Identifier()
        {
            return record.EnumElementName;
        }

        /**
         * Returns the minimum parse length of the identifier if double hyphen <c>'--'</c> is used.
         * @return The minimum characters to satisfy the option.
         */
        int             MinimumRecognitionLength()
        {
            return record.MinimumRecognitionLength;
        }

        /**
         * Returns the identifier of the option if single hyphen <c>'-'</c> is used.
         * @return The option identifier.
         */
        character       IdentifierChar()
        {
            return record.identifierChar.IsNotEmpty() ? record.identifierChar.CharAtStart()
                                                      : '\0';
        }

        /**
         * An optional separator string (usually "="), that separates the parameter name from a
         * parameter value.
         * @return The separator string.
         */
        const String&   ValueSeparator()
        {
            return record.valueSeparator;
        }

        /**
         * The number of CLI arguments to consume and store in \alib{cli,Option::Args} with method
         * \alib{cli,Option::Read}.
         * @return The option identifier.
         */
        integer         QtyExpectedArgsFollowing()
        {
            return record.qtyExpectedArgsFollowing;
        }

        /**
         * Denotes whether multiple occurrences of the option are evaluated or if only the last
         * occurrence is relevant.
         *
         * This flag is not used by the default implementation given by the types of this module.
         * Instead, when processing options, an application either uses \alib{cli,CLIApp::GetOption}
         * to retrieve the last occurrence, or walks list \alib{cli,CLIApp::Options} by itself to
         * fetch all occurrences.
         *
         * @return \c true, if only the last option given is used..
         */
        bool            MultiIgnored()
        {
            return record.multiIgnored;
        }

        /**
         * If an option is a shortcut to another one, this string replaces the argument given.
         * @return The option identifier.
         */
        const String&   ShortcutReplacementString()
        {
            return record.shortcutReplacementString;
        }


        /**
         * Returns a formal description of the usage.
         * Loads the string from #resourceInfo using resource name \c "TOptUsgNN",
         * where \c NN is the enum element's integral value.
         * @return The help text.
         */
        const String&   HelpUsageLine()
        {
            return resourceInfo.Get( NString64("TOptUsg_" ) << Identifier()   ALIB_DBG(, true) );
        }

        /**
         * Returns the help text.
         * Loads the string from #resourceInfo using resource name \c "TOptHlpNN",
         * where \c NN is the enum element's integral value.
         * @return The help text.
         */
        const String&   HelpText()
        {
            return resourceInfo.Get( NString64("TOptHlp_" ) << Identifier()   ALIB_DBG(, true) );
        }
}; // OptionDecl

/**
 * An option of a cli application. Options are read "automatically" using their declaration
 * information defined with externalized strings (resources) accessed through
 * \ref alib_enums_records "ALib Enum Records" associated with enum elements of enum custom types.
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
struct Option : public Parsed
{
    /** The declaration struct. */
    OptionDecl*                         Declaration                                       = nullptr;

    /** Arguments belonging to this option. */
    List<String, Recycling::Shared>     Args;

    /**
     * Constructor
     * @param parent   The application.
     */
    inline
    Option( CLIApp* parent );

    /**
     * Tries to read the object from the current CLI arg(s).
     * \note
     *   Unlike the read methods \alib{cli,Command::Read} and \alib{cli,Parameter::Read}, this
     *   method expects the argument to test not only by number with \p{argNo} but as well
     *   with string parameter \p{arg}.<br>
     *   This redundancy is needed to easily implement shortcut options, that just
     *   replace a shortcut option read to another one, probably with a preset argument included.
     *
     * @param decl   The declaration used for reading.
     * @param arg    The argument string starting with one or two hyphens.
     * @param argNo  The position of reading.
     * @return The \c true on success, \c false otherwise.
     */
    ALIB_API
    bool  Read( OptionDecl& decl, String& arg, const integer argNo );
};

/**
 * \ref alib_enums_records "ALib Enum Record" type used by class \alib{cli,CommandDecl}.
 */
 struct ERCommandDecl : public enums::ERSerializable
 {
    /** List of parameters attached. Separated by <c>'/'</c>.                   */
    String      parameters;

     /** Default constructor leaving the record undefined.
      *  (Implementation required as documented
      *  \alib{enums,EnumRecordPrototype::EnumRecordPrototype(),here}.) */
     ERCommandDecl()                                                            noexcept  = default;

    /** Implementation of \alib{enums,EnumRecordPrototype::Parse}.      */
    ALIB_API
    void Parse();
 };

/**
 * A declaration for a \alib{cli::Command}.
 *
 * Construction is done by passing a custom enum element of an enum type equipped with
 * \ref alib_enums_records "ALib Enum Records" of type \alib{cli,ERCommandDecl}.
 *
 * When bootstrapping \alib_cli_nl, method \alib{cli,CLIApp::DefineCommands} has to be
 * invoked for (each) enum type.
 *
 */
class CommandDecl
{
    protected:
        /** The enumeration element given with construction. */
        Enum                    declElement;

        /** A copy (!) of the enum record. */
        ERCommandDecl           record;

        /** The resource information of the enumeration type given with construction. */
        ResourceInfo            resourceInfo;

    public :
        /** The cli application we are belong to */
        CLIApp&                 Parent;

        /** Command parameters. */
        List<ParameterDecl*>    Parameters;

        /**
         * Templated constructor which takes an enum element of a custom type equipped with
         * \ref alib_enums_records "ALib Enum Records" of type \alib{cli,ERCommandDecl}.
         *
         * Field #Parameters is filled as specified in the enum record.
         *
         * @tparam TEnum    C++ enum type equipped with corresponding \alib Enum Records.
         * @param element   The enum element
         * @param parent    The application object. Will be stored.
         */
        template<typename TEnum>
        inline
        CommandDecl( TEnum element, CLIApp& parent );

       /**
        * Returns the type and integral value of the enumeration element used with construction.
        * @return The enumeration element used with construction.
        */
        const Enum&     Element()                                                                  const
        {
            return declElement;
        }

        /**
         * Returns the identifier (name) of the command
         * @return The command identifier.
         */
        const String&   Identifier()
        {
            return record.EnumElementName;
        }

        /**
         * Returns the minimum parse length of the identifier.
         * @return The minimum characters to satisfy the command to be parsed.
         */
        int             MinimumRecognitionLength()
        {
            return record.MinimumRecognitionLength;
        }

        /**
         * Returns the short version of the help text.
         * Loads the string from #resourceInfo using resource name \c "THlpCmdShtNN",
         * where \c NN is the enum element's integral value.
         * @return The help text.
         */
        const String&   HelpTextShort()
        {
            return resourceInfo.Get( NString64("THlpCmdSht_" ) << Identifier()  ALIB_DBG(, true) );
        }

        /**
         * Returns the long version of the help text.
         * Loads the string from #resourceInfo using resource name \c "THlpCmdLngNN",
         * where \c NN is the enum element's integral value.
         * @return The help text.
         */
        const String&   HelpTextLong()
        {
            return resourceInfo.Get( NString64("THlpCmdLng_" ) << Identifier()  ALIB_DBG(, true) );
        }

        /**
         * Searches in #Parameters for the declaration of parameter \p{name}.
         * @param name   The declaration name of the parameter.
         * @return A pointer to the parameter's declaration, \c nullptr if parameter was not
         *         declared.
         */
        ALIB_API
        ParameterDecl* GetParameterDecl(const String& name );

    protected:
        /**
         * Called from the constructor. Parses field \alib{cli,ERCommandDecl::parameters} of the
         * enum record, and loads the corresponding parameters.
         */
        ALIB_API
        void addParamDecls();
};

/**
 * A command of a \alib_cli_nl application.
 */
struct Command  : public Parsed
{
    /** The underlying declaration. */
    CommandDecl*                            Declaration                                   = nullptr;

    /** Mandatory parameters parsed. */
    List<Parameter*, Recycling::Shared>     ParametersMandatory;

    /** Optional parameters parsed. */
    List<Parameter*, Recycling::Shared>     ParametersOptional;

    /**
     * Constructor
     * @param parent   The application.
     */
    inline
    Command( CLIApp* parent );

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
 * \ref alib_enums_records "ALib Enum Record" type used by class \alib{cli,ExitCodeDecl}.
 * \note Field \alib{enums,ERSerializable::MinimumRecognitionLength} is not read from the string,
 *       but set to fixed value \c 0.
 */
struct ERExitCodeDecl : public enums::ERSerializable
{
    /** The CLI module exception associated to this exit code.             */
    int         associatedCLIException;

    /** Default constructor leaving the record undefined.
     *  (Implementation required as documented
     *  \alib{enums,EnumRecordPrototype::EnumRecordPrototype(),here}.) */
    ERExitCodeDecl()                                                            noexcept
    : ERSerializable()
    {}

    /** Implementation of \alib{enums,EnumRecordPrototype::Parse}. */
    ALIB_API
    void Parse();
};

/**
 * An exit code of the cli application.
 *
 * Construction is done by passing a custom enum element of an enum type equipped with
 * \ref alib_enums_records "ALib Enum Records" of type \alib{cli,ERExitCodeDecl}.
 *
 * When bootstrapping \alib_cli_nl, method \alib{cli,CLIApp::DefineExitCodes} has to be
 * invoked for (each) enum type.
 *
 * Announcing the main application's exit codes to the \alib_cli_nl module has two reasons:
 * - The exit codes are included in the help output text utility methods provided by class
 *   \alib{cli,CLIApp}.
 * - \alib_cli_nl module exit codes can be translated to valid exit codes using
 *   method \alib{cli,CLIUtil::GetExitCode}.
 */
class ExitCodeDecl
{
    protected:
        /** The enumeration element given with construction. */
        Enum            declElement;

        /** A copy (!) of the enum record. */
        ERExitCodeDecl  record;

        /** The resource information of the enumeration type given with construction. */
        ResourceInfo    resourceInfo;


    public:
        /**
         * Templated constructor which takes an enum element of a custom type equipped with
         * \ref alib_enums_records "ALib Enum Records" of type \alib{cli,ERExitCodeDecl}.
         *
         * @tparam TEnum    C++ enum type equipped with corresponding \alib Enum Records.
         * @param element   The enum element.
         */
        template<typename TEnum>
        ExitCodeDecl( TEnum element )
        : declElement( element )
        , resourceInfo(element)
        {
            // make a copy of the resourced record
            record= enums::GetRecord(element);
        }

        /**
         * Returns the name of the enum element
         * @return The name of the enum element.
         */
        const String&   Name()
        {
            return record.EnumElementName;
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
            return cli::Exceptions( record.associatedCLIException );
        }

        /**
         * Returns the format string associated with this exit code.
         * Loads the string from #resourceInfo using resource name \c "TExitNN",
         * where \c NN is the enum element's integral value.
         * @return The format string.
         */
        const String&   FormatString()
        {
            return resourceInfo.Get( NString64("TExit" ) << declElement.Integral()  ALIB_DBG(, true) );
        }
};

}}} // namespace [aworx::lib::cli]


#endif // HPP_ALIB_CLI_ARGUMENTS
