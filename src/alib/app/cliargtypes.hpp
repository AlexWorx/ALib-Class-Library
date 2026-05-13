//==================================================================================================
/// \file
/// This header-file is part of module \alib_app of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib::app {

class CommandLine;

//==================================================================================================
/// Exceptions of module #"alib::app;2".
/// As usual with class #"exc Exception", some of the exceptions are "inner exceptions"
/// that are caught internally and re-thrown with more information and a different exception code.
///
/// The in this respect "external" exceptions that have to be caught by users of the library, are:
///
/// - #"%NoCommandGiven",
/// - #"%UnknownCommand",
/// - #"%ParsingOptions" and
/// - #"%ParsingCommand".
//==================================================================================================
enum class CLIExceptions {
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

/// Struct used as parent for types
/// - #"app::Command",
/// - #"app::Option", and
/// - #"app::Parameter".
///
/// Stores
/// - a pointer to the #"CommandLine" object,
/// - the position in #"ARG_VN;2", respectively #"ARG_VW;2" where the object was found, and
/// - the number of arguments consumed when reading the object.
///
/// \note
///   For technical reasons, other members that are shared between the derived types named above,
///   have to be declared (each three times) with the types themselves.
struct Parsed {
    /// The cli command-line.
    CommandLine* CmdLine;

    /// The index in #"ARG_VN", respectively #"ARG_VW;2", that this instance (derived option
    /// or parameter) was found at.
    integer      Position;

    /// The number of command-line arguments that a command consumed. This includes the command name
    /// itself. If the method \b Read of derived types leaves this to <c>0</c>, then the option or
    /// parameter was not found.
    integer      ConsumedArguments;

    /// Constructor
    /// @param cmdLine   The command-line instance.
    Parsed( CommandLine* cmdLine )
    : CmdLine          (cmdLine)
    , Position         ((std::numeric_limits<integer>::max)())
    , ConsumedArguments(0)                                                                        {}
};


//##################################################################################################
// Decl and Def versions of commands, options, parameters and ExitCodes
//##################################################################################################


/// #"alib_enums_records;ALib Enum Record" type used by class #"ParameterDecl".
struct ERParameterDecl : public enumrecords::ERSerializable {
    /// The identifier of the parameter.
    String      identifier;

    /// An optional separator string (usually "=") that separates the parameter name from a
    /// value given within the parameter itself.
    String      valueSeparator;

    /// A separator character for parsing multiple values.
    /// If set to <c>'C'</c>, method #"ParameterDecl::ValueListSeparator;*" will return
    /// <c>','</c> instead.
    nchar       valueListSeparator;

    /// The number of arguments to consume and store in #"Parameter::Args;*".
    /// If negative, parsing stops. If previous field, separator string is set and this
    /// value is equal or greater to \c 1, then a missing separator string leads to
    /// a parsing exception.
    int         RequiredArguments;

    /// Denotes if this is an optional parameter.
    bool        isOptional;

    /// Default constructor leaving the record undefined. (Implementation required as documented
    /// #"EnumRecordPrototype();here".)
    ERParameterDecl()                                                             noexcept =default;

    /// Implementation of #"EnumRecordPrototype::Parse;*".
    ALIB_DLL
    void Parse();
 };

/// A parameter of a #"CommandDecl".
///
/// Construction is done by passing a custom enum element of an enum type equipped with
/// #"alib_enums_records;ALib Enum Records" of type #"ERParameterDecl".
///
/// When bootstrapping, the method #"CommandLine::DefineParameters;*" has to be invoked for (each)
/// enum type.
class ParameterDecl {
  protected:
    /// The enumeration element given with construction.
    Enum            declElement;

    /// A copy (!) of the enum record.
    ERParameterDecl record;

    /// The resource information of the enumeration type given with construction.
    ResourceInfo    resourceInfo;

  public:
    /// Templated constructor which takes an enum element of a custom type equipped with
    /// #"alib_enums_records;ALib Enum Records" of type #"ERParameterDecl".
    ///
    /// @tparam TEnum    C++ enum type equipped with corresponding \alib Enum Records.
    /// @param element   The enum element
    template<typename TEnum>
    ParameterDecl( TEnum element )
    : declElement( element )
    , resourceInfo(element) {
        // make a copy of the resourced record
        record= enumrecords::GetRecord(element);

        // fix separator character
        if( record.valueListSeparator == 'C' )
            record.valueListSeparator= ',';
    }

    /// Returns the type and integral value of the enumeration element used with construction.
    /// @return The enumeration element used with construction.
    const Enum&     Element()                                          const { return declElement; }

    /// Returns the name of the parameter. This is not the identifier. The name is just for
    /// help and configuration output.
    ///
    /// \see Method #".Identifier".
    ///
    /// @return The name of the enum element.
    const String&   Name()                                        { return record.EnumElementName; }

    /// Returns the identifier of the parameter. If this is empty, the parameter is not optional
    /// and hence has no identifier.
    ///
    /// @return The name of the enum element.
    const String&   Identifier()                                       { return record.identifier; }

    /// Returns the minimum parse length of the identifier.
    /// @return The minimum characters to satisfy the parameter.
    int             MinimumRecognitionLength()           { return record.MinimumRecognitionLength; }

    /// An optional separator string (usually "="), that separates the parameter name from a
    /// parameter value.
    /// @return The separator string.
    const String&   ValueSeparator()                               { return record.valueSeparator; }

    /// A separator character for parsing multiple values.
    /// @return The separator character.
    nchar           ValueListSeparator()
    { return record.valueListSeparator != 'C' ? record.valueListSeparator : ','; }

    /// The number of CLI arguments to consume and store in #"Option::Args;*" with method
    /// #"Parameter::Read;*".
    ///
    /// @return The parameter identifier.
    int             QtyExpectedArgsFollowing()                  { return record.RequiredArguments; }

    /// Returns \c true if the parameter is optional. The information about this attribute is
    /// used to create help messages and usage format strings only. It does not automatically
    /// raise an exception if a parameter is not given. Such exception or other error treatment
    /// is up to the user code.
    /// @return \c true if the parameter is optional, \c false otherwise.
    bool IsOptional()                                                  { return record.isOptional; }

    /// Returns the short help text.
    /// Loads the string from #".resourceInfo" using resource name \c "THelpParShtNN",
    /// where \c NN is the enum element's integral value.
    /// @return The help text.
    const String&   GetHelpTextShort()
    { return resourceInfo.Get( NString64("THlpParSht_" ) << Name()  ALIB_DBG(, true) ); }

    /// Returns the long help text.
    /// Loads the string from #".resourceInfo" using resource name \c "THelpParLngNN",
    /// where \c NN is the enum element's integral value.
    /// @return The help text.
    const String&   GetHelpTextLong()
    { return resourceInfo.Get( String64("THlpParLng_" ) << Name() ALIB_DBG(, true)); }
}; // ParameterDecl

/// A declaration for a #"app::Parameter".
struct Parameter : public Parsed {
    /// The underlying declaration.
    ParameterDecl*                                  Declaration                           = nullptr;

    /// Arguments belonging to us.
    ListMA<String, Recycling::Shared>  Args;

    /// Constructor
    /// @param cmdLine   The command-line instance.
    inline
    Parameter( CommandLine* cmdLine );

    /// Tries to read the object from the front of #"CommandLine::ArgsLeft;*".
    /// Success is indicated by setting inherited fields #"Parsed::Position;*" and
    /// #"Parsed::ConsumedArguments;*" to values greater than \c 0.
    ///
    /// If it could not be decided if the actual CLI argument contains this parameter, \c false
    /// is returned to indicate that parsing commands has to stop now.
    ///
    /// This is done in the following cases:
    /// - When #"ParameterDecl::Identifier;*" is empty and the parameter is
    ///   #"ParameterDecl::IsOptional;*" gives \c true.
    /// - When it was successfully read, but #"ParameterDecl::QtyExpectedArgsFollowing;*"
    ///   is defined \c -1.
    ///
    /// See #"CommandLine;ReadNextCommands" for details
    ///
    /// @param decl   The declaration used for reading
    /// @return The \c true on success, \c false indicates that parsing has to end here.
    ALIB_DLL
    bool    Read( ParameterDecl& decl );
};

/// #"alib_enums_records;ALib Enum Record" type used by class #"OptionDecl".
struct EROptionDecl : public enumrecords::ERSerializable {
    /// The name of the option as parsed from command-line if single hyphen <c>'-'</c> is used.
    /// Defined as string to be able to have empty strings, which disables single character
    /// options.
    String  identifierChar;

    /// An optional separator string (usually "=") that separates the option name from a value
    /// within the first argument itself.
    /// If this is not given, field #".RequiredArguments" has to be \c 0.
    String  valueSeparator;

    /// The number of arguments to consume and store in #"Option::Args;*".
    /// If this field is set and this value is not \c 0, then a missing separator string leads
    /// to a parsing exception.
    integer RequiredArguments;

    /// If not empty, the argument string will be replaced by this and the search for next options
    /// continues.
    /// Note: Shortcut options have to occur earlier in the enumeration's resource definition.
    String  shortcutReplacementString;

    /// Defaulted constructor leaving the record undefined.
    /// (Implementation required as documented
    /// #"EnumRecordPrototype::EnumRecordPrototype();here".)
    EROptionDecl()                                                                noexcept =default;

    /// Implementation of #"EnumRecordPrototype::Parse;*".
    ALIB_DLL
    void Parse();
};

/// A declaration for an #"app::Option".
///
/// Construction is done by passing a custom enum element of an enum type equipped with
/// #"alib_enums_records;ALib Enum Records" of type #"EROptionDecl".
///
/// When bootstrapping, the method #"CommandLine::DefineOptions;*" has to be invoked for (each)
/// enum type.
class OptionDecl {
  protected:
    /// The enumeration element given with construction.
    Enum            declElement;

    /// A copy (!) of the enum record.
    EROptionDecl    record;

    /// The resource information of the enumeration type given with construction.
    ResourceInfo    resourceInfo;


  public:
    /// Templated constructor which takes an enum element of a custom type equipped with
    /// #"alib_enums_records;ALib Enum Records" of type #"EROptionDecl".
    ///
    /// @tparam TEnum    C++ enum type equipped with corresponding \alib Enum Records.
    /// @param element   The enum element
    template<typename TEnum>
    OptionDecl( TEnum element )
    : declElement( element )
    , resourceInfo(element)
    {
        // make a copy of the resourced record
        record= enumrecords::GetRecord(element);
    }

    /// Returns the type and integral value of the enumeration element used with construction.
    /// @return The enumeration element used with construction.
    const Enum&     Element()                                          const { return declElement; }

    /// Returns the identifier of the option if double hyphen <c>'--'</c> is used.
    /// @return The option identifier.
    const String&   Identifier()                                  { return record.EnumElementName; }

    /// Returns the minimum parse length of the identifier if double hyphen <c>'--'</c> is used.
    /// @return The minimum characters to satisfy the option.
    int             MinimumRecognitionLength()           { return record.MinimumRecognitionLength; }

    /// Returns the identifier of the option if single hyphen <c>'-'</c> is used.
    /// @return The option identifier.
    character       IdentifierChar()
    {
        return record.identifierChar.IsNotEmpty() ? record.identifierChar.CharAtStart()
                                                  : '\0';
    }

    /// An optional separator string (usually "="), that separates the parameter name from a
    /// parameter value.
    /// @return The separator string.
    const String&   ValueSeparator()                               { return record.valueSeparator; }

    /// The number of CLI arguments to consume and store in #"Option::Args;*" with method
    /// #"Option::Read;*".
    /// @return The option identifier.
    integer         QtyExpectedArgsFollowing()                  { return record.RequiredArguments; }

    /// If an option is a shortcut to another one, this string replaces the argument given.
    /// @return The option identifier.
    const String&   ShortcutReplacementString()         { return record.shortcutReplacementString; }


    /// Returns a formal description of the usage.
    /// Loads the string from #".resourceInfo" using resource name \c "TOptUsgNN",
    /// where \c NN is the enum element's integral value.
    /// @return The help text.
    const String&   HelpUsageLine()
    { return resourceInfo.Get( NString64("TOptUsg_" ) << Identifier()   ALIB_DBG(, true) ); }

    /// Returns the help text.
    /// Loads the string from #".resourceInfo" using resource name \c "TOptHlpNN",
    /// where \c NN is the enum element's integral value.
    /// @return The help text.
    const String&   HelpText()
    { return resourceInfo.Get( NString64("TOptHlp_" ) << Identifier()   ALIB_DBG(, true) ); }
}; // OptionDecl

/// An option of a command-line. Options are read "automatically" using their declaration
/// information defined with externalized strings (resources) accessed through
/// #"alib_enums_records;ALib Enum Records" associated with enum elements of enum custom types.
///
/// However, such automatic read is limited due to the fact, that the simple values and flags
/// defined with #"OptionDecl", cannot provide the flexibility needed to perfectly
/// parse options with a complex syntax.
///
/// In this case, the way out is to use custom code that invokes #"ReadOptions"
/// and then processes all options that may have remaining arguments left in the list.
/// By using the inherited field #"Parsed::Position", further arguments may be consumed from
/// #"CommandLine::ArgsLeft".<br>
/// Note that the term "processing all options" may mean a nested loop.
/// The outer is over the option types in the field #"CommandLine::Options", the inner is over
/// the vector of options per type.
struct Option : public Parsed {
    /// The declaration struct.
    OptionDecl*                                     Declaration                           = nullptr;

    /// Arguments belonging to this option.
    ListMA<String, Recycling::Shared>  Args;

    /// Constructor
    /// @param cmdLine   The command-line main object.
    inline
    Option( CommandLine* cmdLine );

    /// Tries to read the object from the current CLI arg(s).
    /// \note
    ///   Unlike the read methods #"Command::Read;*" and #"Parameter::Read;*", this
    ///   method expects the argument to test not only by number with \p{argNo} but as well
    ///   with string parameter \p{arg}.<br>
    ///   This redundancy is needed to easily implement shortcut options, that just
    ///   replace a shortcut option read to another one, probably with a preset argument included.
    ///
    /// @param decl   The declaration used for reading.
    /// @param arg    The argument string starting with one or two hyphens.
    /// @param argNo  The position of reading.
    /// @return The \c true on success, \c false otherwise.
    ALIB_DLL
    bool  Read( OptionDecl& decl, String& arg, const integer argNo );
};

/// #"alib_enums_records;ALib Enum Record" type used by class #"CommandDecl".
struct ERCommandDecl : public enumrecords::ERSerializable {
    /// List of parameters attached. Separated by <c>'/'</c>.
    String      parameters;

    /// Default constructor leaving the record undefined.
    /// (Implementation required as documented
    /// #"EnumRecordPrototype();here".)
    ERCommandDecl()                                                               noexcept =default;

    /// Implementation of #"EnumRecordPrototype::Parse;*".
    ALIB_DLL
    void Parse();
};

/// A declaration for a #"app::Command".
///
/// Construction is done by passing a custom enum element of an enum type equipped with
/// #"alib_enums_records;ALib Enum Records" of type #"ERCommandDecl".
///
/// When bootstrapping, method #"CommandLine::DefineCommands;*" has to be invoked for (each)
/// enum type.
class CommandDecl {
  protected:
    /// The enumeration element given with construction.
    Enum                                declElement;

    /// A copy (!) of the enum record.
    ERCommandDecl                       record;

    /// The resource information of the enumeration type given with construction.
    ResourceInfo                        resourceInfo;

  public :
    /// The command-line instance we belong to.
    CommandLine&                        CmdLine;

    /// Command parameters.
    ListMA<ParameterDecl*> Parameters;

    /// Templated constructor which takes an enum element of a custom type equipped with
    /// #"alib_enums_records;ALib Enum Records" of type #"ERCommandDecl".
    ///
    /// Field #".Parameters" is filled as specified in the enum record.
    ///
    /// @tparam TEnum    C++ enum type equipped with corresponding \alib Enum Records.
    /// @param element   The enum element
    /// @param cmdLine   The command-line  object. Will be stored.
    template<typename TEnum>
    inline
    CommandDecl( TEnum element, CommandLine& cmdLine );

    /// Returns the type and integral value of the enumeration element used with construction.
    /// @return The enumeration element used with construction.
    const Enum&     Element()                                          const { return declElement; }

    /// Returns the identifier (name) of the command
    /// @return The command identifier.
    const String&   Identifier()                                  { return record.EnumElementName; }

    /// Returns the minimum parse length of the identifier.
    /// @return The minimum characters to satisfy the command to be parsed.
    int             MinimumRecognitionLength()           { return record.MinimumRecognitionLength; }

    /// Returns the short version of the help text.
    /// Loads the string from #".resourceInfo" using resource name \c "THlpCmdShtNN",
    /// where \c NN is the enum element's integral value.
    /// @return The help text.
    const String&   HelpTextShort()
    { return resourceInfo.Get( NString64("THlpCmdSht_" ) << Identifier()  ALIB_DBG(, true) ); }

    /// Returns the long version of the help text.
    /// Loads the string from #".resourceInfo" using resource name \c "THlpCmdLngNN",
    /// where \c NN is the enum element's integral value.
    /// @return The help text.
    const String&   HelpTextLong()
    { return resourceInfo.Get( NString64("THlpCmdLng_" ) << Identifier()  ALIB_DBG(, true) ); }

    /// Searches in #".Parameters" for the declaration of parameter \p{name}.
    /// @param name   The declaration name of the parameter.
    /// @return A pointer to the parameter's declaration, \c nullptr if parameter was not
    ///         declared.
    ALIB_DLL
    ParameterDecl* GetParameterDecl(const String& name );

  protected:
    /// Called from the constructor. Parses field #"ERCommandDecl::parameters;*" of the
    /// enum record, and loads the corresponding parameters.
    ALIB_DLL
    void addParamDecls();
};

/// A command argument of the command-line.
struct Command  : public Parsed {
    /// The underlying declaration.
    CommandDecl*                                        Declaration                       = nullptr;

    /// Mandatory parameters parsed.
    ListMA<Parameter*, Recycling::Shared>  ParametersMandatory;

    /// Optional parameters parsed.
    ListMA<Parameter*, Recycling::Shared>  ParametersOptional;

    /// Constructor
    /// @param cmdLine   The command-line instance.
    inline
    Command( CommandLine* cmdLine );

    /// Tries to read the object from the front of #"CommandLine::ArgsLeft;*".
    /// @param decl   The declaration used for reading.
    /// @return The \c true on success, \c false otherwise.
    ALIB_DLL
    bool  Read( CommandDecl& decl );

    /// Searches in #".ParametersMandatory" and #".ParametersOptional" for parameter \p{name}.
    /// @param name   The declaration name of the parameter.
    /// @return A pointer to the parameter, \c nullptr if parameter was not parsed.
    ALIB_DLL
    Parameter* GetParsedParameter(const String& name );

    /// Searches in #".ParametersMandatory" and #".ParametersOptional" for parameter \p{name} and returns
    /// its (first) argument.
    /// @param name   The declaration name of the parameter.
    /// @return The argument string, #"%NULL_STRING" if parameter was not parsed if not given.
    ALIB_DLL
    String GetParsedParameterArg( const String& name );
};

/// #"alib_enums_records;ALib Enum Record" type used by class #"ExitCodeDecl".
/// \note Field #"ERSerializable::MinimumRecognitionLength;*" is not read from the string,
///       but set to fixed value \c 0.
struct ERExitCodeDecl : public enumrecords::ERSerializable {
    /// Default constructor leaving the record undefined.
    /// (Implementation required as documented
    /// #"EnumRecordPrototype();here".)
    ERExitCodeDecl()                                                  noexcept : ERSerializable() {}

    /// Implementation of #"EnumRecordPrototype::Parse;*". This implementation omits parsing
    /// the inherited field #"ERSerializable::MinimumRecognitionLength;2".
    ALIB_DLL
    void Parse();
};

/// An exit-code of a cli application.
///
/// Construction is done by passing a custom enum element of an enum type equipped with
/// #"alib_enums_records;ALib Enum Records" of type #"ERExitCodeDecl".
///
/// When bootstrapping, the method #"CommandLine::DefineExitCodes;*" has to be invoked for
/// (each) enum type.
///
/// With the announcment of exit codes to the CLI, these codes can be listed with help output
/// generated by class #"CLIUtil".
class ExitCodeDecl {
  protected:
    /// The enumeration element given with construction.
    Enum            declElement;

    /// A copy (!) of the enum record.
    ERExitCodeDecl  record;

    /// The resource information of the enumeration type given with construction.
    ResourceInfo    resourceInfo;

  public:
    /// Templated constructor which takes an enum element of a custom type equipped with
    /// #"alib_enums_records;ALib Enum Records" of type #"ERExitCodeDecl".
    ///
    /// @tparam TEnum    C++ enum type equipped with corresponding \alib Enum Records.
    /// @param element   The enum element.
    template<typename TEnum>
    ExitCodeDecl( TEnum element )
    : declElement( element )
    , resourceInfo(element)
    {
        // make a copy of the resourced record
        record= enumrecords::GetRecord(element);
    }

    /// Returns the name of the enum element
    /// @return The name of the enum element.
    const String&   Name()                                        { return record.EnumElementName; }

    /// Returns the format string associated with this exit-code.
    /// Loads the string from #".resourceInfo" using resource name \c "TExitNN",
    /// where \c NN is the enum element's integral value.
    /// @return The format string.
    const String&   FormatString()
    { return resourceInfo.Get( NString64("TExit" ) << declElement.Integral() ALIB_DBG(,true)); }
};

} // namespace [alib::app]
