//==================================================================================================
/// \file
/// This header file is part of module \alib_config of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_CONFIG_PLUGINS
#define HPP_ALIB_CONFIG_PLUGINS 1

#include "configuration.hpp"
#include "alib/config/priority.hpp"
#include "alib/strings/util/escaper.hpp"

namespace alib::config {

// forwards
class Configuration;
class Variable;

//==================================================================================================
/// Abstract class that defines the plug-in interface for class \alib{config;Configuration}.
/// Plug-ins provide configuration data (variables) from external configuration data sources
/// at the moment those are requested.
///
/// The rationale for this concept is that some external configuration sources may provide a lot of
/// data that is not related to an application. This is for example true for for environment
/// variables, the windows registry or the Gnome variable systems <em>gconf/dconf</em>.
/// In these cases, variables have to be read into the \alib configuration system only at the moment
/// they are declared.
///
/// With other sources, like dedicated INI-files, a different technique is usually preferred:
/// Here, all data is read and imported into the configuration system with an application's
/// bootstrap and variables (respectively whole subtrees of variables) may be exported when an
/// application exits, just to populate empty INI-files with default values or to add new variables
/// which occur with a new software version.
///
/// Category and Variable names are character case insensitive for the plug-ins predefined
/// with \alib. It is up to a custom implementation to decide to ignore character case in custom
/// specializations of this class as well.
//==================================================================================================
class ConfigurationPlugin  : public lang::Plugin<Configuration, Priority>
{
  protected:
    /// A default string escaper instance. This is used with all plugins provided with \alib.
    /// (Namely \alib{config;CLIVariablesPlugin} and \alib{config;EnvironmentVariablesPlugin}.)
    StringEscaperStandard   stringEscaper;

    /// Constructor which is protected, as this is an abstract class.
    /// @param pPriority      The priority that this plug-in uses.
    ConfigurationPlugin( Priority pPriority )
    : Plugin(pPriority)                                                                           {}

  public:
    /// Virtual Destructor.
    virtual ~ConfigurationPlugin() {}

    /// Derived types may return a different, customized implementation specific to their needs.
    /// This default implementation returns field #stringEscaper.
    /// @return An escaper used to convert string values from and to escaped sequences as provided
    ///         by a configuration source that is based on serialization of variable values to and
    ///         from ASCII/unicode strings which are human-readable and placeable in text files,
    ///         command line parameters, etc.
    virtual const StringEscaper&   GetEscaper()                     const  { return stringEscaper; }

    /// Abstract method. Descendents need to return a plug-in name. The name may be used in human
    /// readable output, e.g., log-files or exception messages to tell a user for example, which
    /// plug-in loaded a variable containing a syntax error.
    /// @return The name of the plug-in.
    virtual String                  Name()                                               const   =0;

    //==============================================================================================
    /// Abstract method that has to be overwritten by descendants.
    /// Searches and retrieves the value of a configuration variable.
    /// @param      name    The name of the variable to retrieve.
    /// @param[out] target  A reference to the buffer to write the variable's exported value to.
    /// @return \c true if variable was found within this configuration source, \c false if not.
    //==============================================================================================
    ALIB_API
    virtual bool  Get( const String& name, AString& target )                                     =0;
};


//==================================================================================================
/// Specialization of abstract interface class #ConfigurationPlugin, which reads command line
/// parameters from namespace globals #alib::ARG_C and #alib::ARG_VN / \alib{ARG_VW} on request.
/// Its priority value usually is \alib{config,Priority,Priority::CLI}, which is higher
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
/// public field #DefaultCategories. Variables of these categories are recognized by the plug-in
/// also when given without the name prefix of category name and underscore \c '_'.
///
/// ## Friends ##
/// class \alib{config,detail::nextCLIArg} (Used for implementation of iterator.)
//==================================================================================================
class CLIVariablesPlugin : public ConfigurationPlugin
{
  public:
    /// If any value is added to this vector, its values are used as the source of command line
    /// arguments instead of using \alib namespace variables \alib{ARG_C} and
    /// \alib{ARG_VN}/\alib{ARG_VW}.<br>
    /// This mechanic provides an alternative method to set the command line argument list.
    ///
    /// Applications that have a dedicated (more sophisticated) CLI interface which performs
    /// more complex processing of CLI arguments, may collect any unrecognized
    /// CLI argument here to be duly recognized as a configuration variable instead
    StringVectorMA      AlternativeArgs;

    /// An application can specify one or more "default categories" by adding a prefix of the
    /// variable path here. Variables of these "categories" are recognized by the plug-in also
    /// when given without this prefix.<br>
    /// Please note, that if \alib and \alox variables should be abbreviatable, for example that
    /// the locale can be given with
    ///             --locale=de_DE.UTF-8
    /// instead of
    ///             --alib_locale=de_DE.UTF-8
    /// this vector has to be populated during bootstrap phase \alib{BootstrapPhases;PrepareConfig}.
    /// See chapter \ref alib_manual_bootstrapping_customize for more information on how to
    /// customize \alib bootstrapping.
    StringVectorMA      DefaultCategories;

    /// Determines whether zero, one or two introducing hyphen characters <c>'-'</c> are mandatory.
    /// An command line argument is ignored if the number of leading hyphens is smaller than
    /// the value set here.<br>
    /// Defaults to \c 0.
    /// @see Sibling option #QtyOptionalHyphens.
    uint8_t                 QtyMandatoryHyphens                                                 = 0;

    /// Determines whether zero, one or two optional hyphen characters <c>'-'</c> might be given.
    /// An command line argument is ignored if the number of leading hyphens is greater than
    /// the value set here.<br>
    /// Defaults to \c 2.
    /// @see Sibling option #QtyMandatoryHyphens.
    uint8_t                 QtyOptionalHyphens                                                  = 2;

    /// Constructor.
    /// @param ma         The monotonic allocator to use. This usually is the one of the
    ///                   configuration instance.
    /// @param pPriority  The priority that this plug-in uses. Defaults to
    ///                   \alib{config;Priority;CLI}.
    ALIB_API
    CLIVariablesPlugin( MonoAllocator& ma, Priority pPriority= Priority::CLI );

    /// Virtual Destructor.
    virtual ~CLIVariablesPlugin()                                                       override  {}

    /// @return The plug-in name, in this case, we read resource variable "CfgPlgCLI".
    ALIB_API virtual String      Name()                                              const override;

    /// Searches the variable in the command line parameters.
    /// @param      name    The name of the variable to retrieve.
    /// @param[out] target  A reference to the buffer to write the variable's exported value to.
    /// @return \c true if variable was found, \c false if not.
    ALIB_API virtual bool        Get( const String& name, AString& target )                override;
};

//==================================================================================================
/// Specialization of abstract interface class #ConfigurationPlugin, retrieves configuration
/// data from the system environment.
///
/// The priority value of this plug-in usually is \alib{config;Priority;Priority::Environment},
/// which is higher than \alib{config;Priority;Standard} but lower than \alib{config;Priority;CLI}.
///
/// To recognize variables, the separation character <c>'/'</c> of the configuration tree is
/// converted to underscore character <c>'_'</c>. For example, the \alib locale variable with path
///         ALIB/LOCALE
/// is recognized as
///         ALIB_LOCALE
///
/// Category and Variable names are insensitive in respect to character case.
//==================================================================================================
class EnvironmentVariablesPlugin : public ConfigurationPlugin
{
  public:
    /// An application can specify one or more "default categories" by adding a prefix of the
    /// variable path here. Variables of these "categories" are recognized by the plug-in also
    /// when given without this prefix.<br>
    /// Please note, that if \alib and \alox variables should be abbreviatable, for example that
    /// the locale can be given with
    ///             --locale=de_DE.UTF-8
    /// instead of
    ///             --alib_locale=de_DE.UTF-8
    /// this vector has to be populated during bootstrap phase \alib{BootstrapPhases;PrepareConfig}.
    /// See chapter \ref alib_manual_bootstrapping_customize for more information on how to
    /// customize \alib bootstrapping.
    StringVectorMA          DefaultCategories;

    /// Constructor.
    /// @param ma         The monotonic allocator to use. This usually is the one of the
    ///                   configuration instance.
    /// @param pPriority     The priority that this plug-in uses. Defaults to
    ///                      \alib{config;Priority;Environment}.
    ALIB_API EnvironmentVariablesPlugin( MonoAllocator& ma,
                                         Priority      pPriority      = Priority::Environment );

    /// Virtual Destructor.
    virtual                 ~EnvironmentVariablesPlugin()                                override {}

    /// @return The name of the plug-in, in this case, the value of resource variable "CfgPlgEnv".
    ALIB_API virtual String Name()                                                   const override;

    /// Searches the variable in the environment.
    /// @param      name    The name of the variable to retrieve.
    /// @param[out] target  A reference to the buffer to write the variable's exported value to.
    /// @return \c true if variable was found, \c false if not.
    ALIB_API virtual bool   Get( const String& name, AString& target )                     override;
};

} // namespace [alib::config]

#endif // HPP_ALIB_CONFIG_PLUGINS

