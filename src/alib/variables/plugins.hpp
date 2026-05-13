//==================================================================================================
/// \file
/// This header-file is part of module \alib_variables of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib::variables {

//==================================================================================================
/// Specialization of abstract interface class #"ConfigurationPlugin", which reads command-line
/// parameters from namespace globals #"alib::ARG_C;2" and #"alib::ARG_VN;2" / #"ARG_VW;2" on
/// request.
/// Its priority value usually is #"Priority::CLI", which is higher
/// than all other default plug-ins provided.
///
/// To recognize variables, the separation character <c>'/'</c> of the configuration tree is
/// converted to underscore character <c>'_'</c>. For example, the \alib locale variable with path
///         ALIB/LOCALE
/// is recognized as
///         ALIB_LOCALE
///
/// Variable names are insensitive in respect to character case.
///
/// Command line variables may be passed with either one hyphen ('-') or two ('--').
/// Both are accepted.
///
/// An application can specify one or more "default categories" by adding their string names to
/// public field #".DefaultCategories". Variables of these categories are recognized by the plug-in
/// also when given without the name prefix of category name and underscore \c '_'.
//==================================================================================================
class CLIVariablesPlugin : public ConfigurationPlugin {
  public:
    /// If any value is added to this vector, its values are used as the source of command-line
    /// arguments instead of using \alib namespace variables #"ARG_C;2" and
    /// #"ARG_VN;2" / #"ARG_VW;2".<br>
    /// This mechanic provides an alternative method to set the command-line argument list.
    ///
    /// Applications that have a dedicated (more sophisticated) CLI interface which performs
    /// more complex processing of CLI arguments, may collect any unrecognized
    /// CLI argument here to be duly recognized as a configuration variable instead
    StringVectorMA      AlternativeArgs;

    /// An application can specify one or more "default categories" by adding a prefix of the
    /// variable path here. Variables of these "categories" are recognized by the plug-in also
    /// when given without this prefix.<br>
    /// Please note, that if \alib and \alox variables should be abbreviatable, for example, that
    /// the locale can be given with
    ///             --locale=de_DE.UTF-8
    /// instead of
    ///             --alib_locale=de_DE.UTF-8
    /// this vector has to be populated during bootstrap phase #"BootstrapPhases::PrepareConfig".
    /// See chapter #"alib_mod_bs_customize" for more information on how to
    /// customize \alib bootstrapping.
    StringVectorMA      DefaultCategories;

    /// Determines whether zero, one or two introducing hyphen characters <c>'-'</c> are mandatory.
    /// An command-line argument is ignored if the number of leading hyphens is smaller than
    /// the value set here.<br>
    /// Defaults to \c 0.
    /// @see Sibling option #"QtyOptionalHyphens".
    uint8_t                 QtyMandatoryHyphens                                                 = 0;

    /// Determines whether zero, one or two optional hyphen characters <c>'-'</c> might be given.
    /// An command-line argument is ignored if the number of leading hyphens is greater than
    /// the value set here.<br>
    /// Defaults to \c 2.
    /// @see Sibling option #"QtyMandatoryHyphens".
    uint8_t                 QtyOptionalHyphens                                                  = 2;

    /// Constructor.
    /// @param ma         The monotonic allocator to use. This usually is the one of the
    ///                   configuration instance.
    /// @param pPriority  The priority that this plug-in uses. Defaults to #"Priority::CLI".
    ALIB_DLL
    CLIVariablesPlugin( MonoAllocator& ma, Priority pPriority= Priority::CLI );

    /// Virtual Destructor.
    virtual ~CLIVariablesPlugin()                                                        override {}

    /// @return The plug-in name, in this case, we read resource variable "CFGPlgCLI".
    ALIB_DLL virtual String      Name()                                              const override;

    /// Searches the variable in the command-line parameters.
    /// @param      name    The name of the variable to retrieve.
    /// @param[out] target  A reference to the buffer to write the variable's exported value to.
    /// @return \c true if variable was found, \c false if not.
    ALIB_DLL virtual bool        Get( const String& name, AString& target )                override;
};

//==================================================================================================
/// Specialization of abstract interface class #"ConfigurationPlugin", retrieves configuration
/// data from the system environment.
///
/// The priority value of this plug-in usually is #"Priority::Environment",
/// which is higher than #"var Priority::Standard" but lower than #"Priority::CLI".
///
/// To recognize variables, the separation character <c>'/'</c> of the configuration tree is
/// converted to underscore character <c>'_'</c>. For example, the \alib locale variable with path
///         ALIB/LOCALE
/// is recognized as
///         ALIB_LOCALE
///
/// Category and Variable names are insensitive in respect to character case.
//==================================================================================================
class EnvironmentVariablesPlugin : public ConfigurationPlugin {
  public:
    /// An application can specify one or more "default categories" by adding a prefix of the
    /// variable path here. Variables of these "categories" are recognized by the plug-in also
    /// when given without this prefix.<br>
    /// Please note, that if \alib and \alox variables should be abbreviatable, for example, that
    /// the locale can be given with
    ///             --locale=de_DE.UTF-8
    /// instead of
    ///             --alib_locale=de_DE.UTF-8
    /// this vector has to be populated during bootstrap phase #"BootstrapPhases::PrepareConfig".
    /// See chapter #"alib_mod_bs_customize" for more information on how to
    /// customize \alib bootstrapping.
    StringVectorMA          DefaultCategories;

    /// Constructor.
    /// @param ma         The monotonic allocator to use. This usually is the one of the
    ///                   configuration instance.
    /// @param pPriority  The priority that this plug-in uses. Defaults to #"Priority::Environment".
    ALIB_DLL EnvironmentVariablesPlugin( MonoAllocator& ma,
                                         Priority      pPriority      = Priority::Environment );

    /// Virtual Destructor.
    virtual                 ~EnvironmentVariablesPlugin()                                override {}

    /// @return The name of the plug-in, in this case, the value of the resource variable
    ///         \b "CFGPlgEnv".
    ALIB_DLL virtual String Name()                                                   const override;

    /// Searches the variable in the environment.
    /// @param      name    The name of the variable to retrieve.
    /// @param[out] target  A reference to the buffer to write the variable's exported value to.
    /// @return \c true if variable was found, \c false if not.
    ALIB_DLL virtual bool   Get( const String& name, AString& target )                     override;
};

} // namespace [alib::variables]
