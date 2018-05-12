// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_UTIL_STRINGMAP)
    #include "alib/strings/util/stringmap.hpp"
#endif

#include <algorithm>

namespace aworx { namespace lib {



/** ************************************************************************************************
\note
  It may be helpful to read this namespace documentation and understand the concepts - even if you
  are not planning to use configuration variables in your own code.
  The reason is that with the knowledge of the principals described here, it is possible to influence
  the behavior of 3rd party code (code of other team members or \alib enabled libraries
  like for example <b>%ALox Logging Library</b>).

# 1. Introduction #

This package provides tools to read and write configuration data using different mechanisms.
In short, the features are:
- Central class \alib{config,Configuration} used to retrieve and store external configuration data
  using arbitrary sources.
- Abstract plug-in interface \alib{config,ConfigurationPlugin} to read from (and optionally store into) custom external storage.
- Predefined plug-ins for command line parameters, environment variables, in-memory storage
  and simple INI files.
- The possibility to set default values 'in code' (if no external plug-in finds a value)
  and doing this "out of scope" which means independent from the code that finally uses a variable.
- The possibility to protect variables 'in code' from being modified (to block modification
  of variables exposed by 3rd party code).
- Automatic resolving of nested variables: Define a value once and pass it as a
  variable to other variables ("variable substitution").
- With the combination of variable substitution and protection of variable values, it is possible
  to hide, rename or even redefine the syntax and behavior of variables exposed by third party code
  (e.g. libraries).
- In the C++ version, variable declaration (defining the category, name, default value, comment and
  other fields) can be optionally externalized using a combination of concepts
  \alib{lang,Resources}, \alib{lang,Library} and \alib{lang,T_EnumMetaDataDecl,enum meta data}.<br>
  <b>%ALib</b>s internal variables itself are externalized and hence can be renamed by users
  of those libraries.


<b>Class %Configuration and Plug-ins:</b><br>

The plug-ins, derived from abstract class
\ref aworx::lib::config::ConfigurationPlugin "ConfigurationPlugin"
are plugged into an object of type
\ref aworx::lib::config::Configuration "Configuration" and are responsible for reading (and
optionally writing) variables from dedicated sources.

Class \b %Configuration offers an interface to retrieve and store configuration variables using
the plug-ins. The plug-ins are attached to the \b %Configuration instance along with a unique priority.
Values are retrieved and stored by looping through the plug-ins sorted by priority.
As soon as one plug-in confirms to have found (or stored) the variable, the loop ends.
This way, plug-ins may "overrule" each other in respect to retrieving configuration data.

Class \alib{lang,Library} creates an instance of this class with public access and this instance
is usually all that is needed.

<b>Naming %Configuration Variables</b>:<br>

%Configuration variables are addressed by two strings: The \b category and the \b name. In the case of
INI files, the category is translated to the section and the name to variables within the
section. Other plug-ins, for example
\ref aworx::lib::config::CLIArgs "CLIArgs" or
\ref aworx::lib::config::Environment  "Environment", are prepending the category
name to the variable name separated by an underscore character \c '_'.
For example, a variable in category \c "ALIB" named \c "WAIT_FOR_KEY_PRESS", in an
INI file would be stated like this:

        [ALIB]
        WAIT_FOR_KEY_PRESS= yes

To define the same variable and value on the command-line, the parameter needs to be:

        --ALIB_WAIT_FOR_KEY_PRESS=yes

The anonymous, empty category is allowed. For convenience, with variables of this category,
no underscore is prepended by the plug-ins. The little drawback of this is, that variables
in the anonymous category that contain an underscore character, for some plug-ins can get
ambiguous. For example, variable \c MY_VARIABLE in the anonymous category, on the command line
would be uniquely addressed by:

        --MY_VAR=my value

In an INI file, two ways of specifying the variable are possible (and therefore ambiguous). In the anonymous section:


        MY_VAR      =       my value

or, in section \c MY:

        [MY]
        VAR         =       my value

# 2. Command-Line parameters, Environment Variables and INI files #

Three different standard plug-ins that collect external configuration variables are provided
with \alib already:
- Class \ref aworx::lib::config::CLIArgs "CLIArgs":<br>
  Reads parameter values of the form -[-]CategoryName_VariableName=value from the command line.
- Class \ref aworx::lib::config::Environment "Environment":<br>
  Reads system defined environment variables
  (see [Wikipedia](https://en.wikipedia.org/wiki/Environment_variable) of the form
  <c>CategoryName_VariableName=value</c>.
- Class \ref aworx::lib::config::IniFile "IniFile":<br>
  Reads and writes quite standard INI files
  (see [Wikipedia](https://en.wikipedia.org/wiki/INI_file).
<p>

The proposed priorities to be used when attaching the plug-ins to the \b %Configuration object,
are enumerated in \alib{config,Priorities}, which is an
\alib{lang,T_EnumIsArithmetical,arithmetical enumeration}.

With these default priorities set, whatever is stored in an INI file,
can be overwritten by setting an environment variable. Both settings can in turn be
overwritten by specifying a corresponding command line parameter when launching the process!

# 3. Default Variables #

In addition to the three plug-ins described above, \alib implements a fourth one,
class \ref aworx::lib::config::InMemoryPlugin "InMemoryPlugin". As the name indicates, this
class keeps configuration variables in memory. The special thing about it is that it does not
read any external data source! The reason for having it, is twofold. The first use case
are <em>Default variables</em>:
An instance is plugged into class \b %Configuration with a (low) priority of
\ref aworx::lib::config::Priorities "Priorities::DefaultValues".
This plug-in serves as a storage for default values.
The only way to set these default values is \e programatically, which means "with program code".

The advantages of having such default variables are:
- Code that checks whether a variable is set can be omitted (there is always a value found).
- Code that sets default values if no variable is set can be omitted.
- All default settings can be made in one place (in resources or in a in custom bootstrap section
  of a process or library)


 * \anchor cpp_alib_config_protection
# 4. Protecting Variables #

Class \ref aworx::lib::config::InMemoryPlugin "InMemoryPlugin" is by default used a second
time by plugging an instance into class \b %Configuration with the highest possible priority
\alib{config,Priorities,Priorities::ProtectedValues}.

When setting a variable within this plug-in, no other plug-in can <em>'overrule'</em> this value.
This way, it is possible to protect values against external modification.

\note
    One might think: "If I do not want to allow external modification, I rather hard-code
    everything". This of-course is true for code under control. However, for 3rd party
    code using \alib, this offers an easy way to disallow users of your software (which
    incorporates that 3rd party code) to configure things that you do not want to be configurable.

# 5. Using class %Configuration #

In normal use cases, there is no need to create an instance of class Configuration, as a singleton
is provided with \alib{lang,Library::Config} for every library.
The command line parameters (optionally) provided with
\alib{lang,Library::Init} are passed to the command-line plug-in of this singleton. Dependent
libraries will receive the same singleton instance, unless they are explicitly initialized
prior to initializing the library that builds upon them. (For details see \alib{lang,Library}.)

Class %Configuration in the constructor by default sets up four plug-ins automatically:
\alib{config,CLIArgs} for parsing command-line arguments as variables,
\alib{config,Environment} for environment variables and two plug-ins of type
\alib{config,InMemoryPlugin} used for default and protected values.

If an application wants to suppress the use of one of the plug-ins, the plug-ins can be removed
using method \ref aworx::lib::config::Configuration::RemovePlugin "RemovePlugin".<br>
On the other hand, a plug-in of type \alib{config,IniFile} may be attached on bootstrap of a
process (or later) using \alib{config,Priorities::Standard}.

In addition (or alternatively), custom plug-ins may be written and installed using arbitrary
priorities.

Class \b %IniFile is designed for simplicity and smaller applications.
Instead of using it \b %IniFile, it is recommended to use application/platform
specific mechanisms for writing configuration data. In this case, write your own plug-in
to grant \alib and other libraries which rely on \alib, access to your applications's configuration
data. Again, \alib here follows its design principle to be non intrusive: The plug-in concept
allows users of \alib (more important:  users of \alib enabled libraries) to expose any external
configuration source to these libraries.
This way, the users can stick to his/her preferred way of implementation.

\anchor cpp_alib_namespace_config_substitution
# 6. %Variable Substitution #

Method \ref aworx::lib::config::Configuration::Load  "Configuration::Load" by default substitutes
references to other configuration variables found in the value of the requested variable.

For example, if two variables are defined as follows:

        MYCAT_RESULT= 42
        MYCAT_MYVARIABLE= The result is $MYCAT_RESULT

then, with the retrieval of variable \c MYCAT_MYVARIABLE, variable \c MYCAT_RESULT is read and the
substring "$MYCAT_RESULT" is substituted by "42".<br>

Substitutions are performed repeatedly until all variables are resolved. Therefore, nested
substitutions may be defined as well. To avoid an endless loop in case of circular dependencies,
a simple threshold applies: A maximum of 50 replacements are done.

The dollar sign \c '$' used to recognize variables is the default and can be modified. It is
also possible to specify a prefix and a suffix for the identification of substitutable variables
in other variables' values. For example, the syntax can be adjusted to

        MYCAT_MYVARIABLE= The result is %{MYCAT_RESULT}

See documentation of fields
\ref aworx::lib::config::Configuration::SubstitutionVariableStart        "SubstitutionVariableStart",
\ref aworx::lib::config::Configuration::SubstitutionVariableEnd          "SubstitutionVariableEnd" and
\ref aworx::lib::config::Configuration::SubstitutionVariableDelimiters   "SubstitutionVariableDelimiters"
for more information.

When parsing a variables' category and name, method
\ref aworx::lib::config::Configuration::Load  "Configuration::Load" searches for an underscore
character \c '_'. The first underscore found serves as a delimiter of category from the name.
If no underscore character is found, the category is left empty (anonymous category) and the name is set to what
is given as a variable name.
\note
    This implies that variables which have no category but have an underscore in their name, need
    to be specified with a leading underscore. For example:

            TEXT= Welcome to $_HOME_LOCATION
\note
    refers to a variable name \c HOME_LOCATION in the anonymous category \c "".<br>
    For clarification:
    - This is not needed (but also no fault) for variables that do not contain an underscore in their name
    - This must not be done if a variable belongs to a named category.


# 7. Loading and Storing %Variables #
Values of variables are loaded and received using instances of class Variable. Instances of this class
can be passed to the interface of class Configuration or directly to specific
\ref aworx::lib::config::ConfigurationPlugin "ConfigurationPlugin" objects.

Simple access methods allow to read or set the values of a variable.

By consequently using optional struct VariableDecl for declaring all variables of an application or library,
all attributes of variables can be maintained in one place. This includes categories, names, comments,
value delimiters and default values of variables.
Class \b %Variable accepts instances of \b %VariableDecl in the constructor as well as in overloaded
method \ref aworx::lib::config::Variable::Declare "Variable::Declare".

# 8. In- and Externalizing %Variable values #
%Variable values provided via command line parameters, environment variables or textual configuration
files need to be converted when loading and storing them. This has two reasons:
- Certain characters of variables have to be "escaped", e.g. \c '\\n' or \c '\\t' within strings
- Multiple values defined in variables need to be separated when storing  - and parsed back when loading
  a variable.

For this task, class
\ref aworx::lib::config::XTernalizer "XTernalizer" is used. Each
\ref aworx::lib::config::ConfigurationPlugin "ConfigurationPlugin" owns an instance of this
class (which is exchangeable). Conversion is done internally and there is no need to interface
with this class directly when using variables.
A huge benefit of this API design is that variable values look the same in an INI-file as
when passed as a command line parameter.

But it is important to understand that value parameters provided with interface methods to store
variables accept "externalized" strings only. In simple cases this is not relevant. But if a
variable contains multiple values or special characters, it has to be understood that
'internalization' of the value takes place.

Normal, internal string values need to be added to the variable in code prior to invoking
a store method. If an externalized string is provided with the store methods, then previously
added variable values are cleared!

# Reference Documentation #

***************************************************************************************************/
namespace config {



// #################################################################################################
// Configuration
// #################################################################################################

Configuration::Configuration( bool addDefaultPlugins )
{
    TrueValues= { ASTR("1"), ASTR("true"), ASTR("t"), ASTR("yes"), ASTR("y"), ASTR("on"), ASTR("ok") };

    if( addDefaultPlugins )
    {
        InsertPlugin( new InMemoryPlugin(CONFIG.Get(ASTR("CfgPlgDef"))), Priorities::DefaultValues   , Responsibility::Transfer);
        InsertPlugin( new Environment()                                , Priorities::Environment     , Responsibility::Transfer);
        InsertPlugin( new CLIArgs()                                    , Priorities::CLI             , Responsibility::Transfer);
        InsertPlugin( new InMemoryPlugin(CONFIG.Get(ASTR("CfgPlgPro"))), Priorities::ProtectedValues , Responsibility::Transfer);
    }
}

Configuration::~Configuration()
{
}

int Configuration::FetchFromDefault( ConfigurationPlugin& dest, const String& sectionName  )
{
    ALIB_LOCK
    InMemoryPlugin* defaultPlugin= GetPluginTypeSafe<InMemoryPlugin>( Priorities::DefaultValues );
    ALIB_ASSERT_ERROR( defaultPlugin,
                       ASTR("Utility method FetchFromDefault used without default plugin in place.") )

    int cntCopied= 0;
    Variable variable;
    for( size_t sNo= 0; sNo < defaultPlugin->Sections.size() ; sNo++ )
    {
        InMemoryPlugin::Section* section= defaultPlugin->Sections[sNo];
        if( sectionName.IsNotEmpty() && !sectionName.Equals( section->Name ) )
            continue;

        for( size_t vNo= 0; vNo < section->Entries.size() ; vNo++ )
        {
            InMemoryPlugin::Entry* entry= section->Entries[vNo];
            if( !dest.Load( variable.Declare( section->Name, entry->Name ), true ) )
            {
                defaultPlugin->Load  ( variable );
                dest.Store( variable );
                cntCopied++;
            }
        }
    }

    return  cntCopied;
}

bool Configuration::IsTrue( const String& value )
{
    for ( auto& it : TrueValues )
        if ( value.Equals<Case::Ignore>( it ) )
            return true;

    return false;
}



// #############################################################################################
// Load/Store
// #############################################################################################
Priorities Configuration::Load( Variable& variable )
{
    ALIB_LOCK

    variable.Config=    this;
    variable.Priority=  loadImpl( variable, true );

    if ( variable.Priority == Priorities::NONE && variable.DefaultValue.IsNotNull() )
        Store( variable, variable.DefaultValue );

    return variable.Priority;
}

Priorities Configuration::Store( Variable& variable, const String& externalizedValue )
{
    ALIB_LOCK

    // checks
    if( externalizedValue.IsNull() )
    {
        if ( variable.Name.IsEmpty())
        {
            ALIB_ERROR( ASTR("Trying to store an undefined variable.")  );
            return Priorities::NONE;
        }

        if ( variable.Size() > 1 && variable.Delim == '\0' )
        {
            ALIB_ERROR((String256() << ASTR("Trying to store variable \"") << variable.Fullname
                       << ASTR("\" which has multiple values set but no delimiter defined.")).ToCString()  );
            return Priorities::NONE;
        }
    }

    // store us as config
    variable.Config= this;

    // detect?
    bool detected= (variable.Priority < Priorities::NONE);
    if ( detected )
    {
        variable.Priority= Priorities::NONE;
        for ( auto& ppp : plugins )
            if ( ppp.plugin->Load( variable, true ) )
            {
                variable.Priority= ppp.priority;
                break;
            }
    }

    // new variables go to default
    if ( variable.Priority == Priorities::NONE )
        variable.Priority= Priorities::DefaultValues;

    // we do not store if detected priority is protected
    else if( detected && variable.Priority == Priorities::ProtectedValues )
        return (variable.Priority= Priorities::NONE);

    // store
    for ( auto& ppp : plugins )
        if (    ppp.priority <= variable.Priority
             && ppp.plugin->Store( variable, externalizedValue ) )
            {
                return (variable.Priority= ppp.priority);
            }

    return (variable.Priority= Priorities::NONE);
}


// #################################################################################################
 // convenience methods using Configuration::Default singleton
 // #################################################################################################
Priorities  Configuration::StoreDefault( Variable& variable, const String& externalizedValue )
{
    ALIB_LOCK
    ConfigurationPlugin* defaultPlugin= GetPlugin( Priorities::DefaultValues );
    ALIB_ASSERT_ERROR( defaultPlugin,
                       ASTR("Utility method StoreDefault used without default plugin in place.") )

    if ( externalizedValue.IsNotNull() )
        defaultPlugin->StringConverter->LoadFromString( variable, externalizedValue );

    if ( variable.Size() == 0 && variable.DefaultValue.IsNotNull() )
        defaultPlugin->StringConverter->LoadFromString( variable, variable.DefaultValue );

    variable.Priority= Priorities::DefaultValues;
    return Store( variable, NullString );
}

Priorities  Configuration::Protect( Variable& variable, const String& externalizedValue )
{
    ALIB_LOCK
    ConfigurationPlugin* protectedPlugin= GetPlugin( Priorities::ProtectedValues );

    if ( externalizedValue.IsNotNull() )
        protectedPlugin->StringConverter->LoadFromString( variable, externalizedValue );

    if ( variable.Size() == 0 && variable.DefaultValue.IsNotNull() )
        protectedPlugin->StringConverter->LoadFromString( variable, variable.DefaultValue );

    ALIB_ASSERT_ERROR( protectedPlugin,
                       ASTR("Utility method Protect used without default plugin in place.") )

    variable.Priority= Priorities::ProtectedValues;
    return Store( variable, NullString );
}

int  Configuration::LoadFromString( Variable& variable, const String& externalizedValue )
{
    ALIB_LOCK
    ConfigurationPlugin* defaultPlugin= GetPlugin( Priorities::DefaultValues );
    ALIB_ASSERT_ERROR( defaultPlugin,
                       ASTR("Utility method LoadFromString used without default plugin in place.") )
    defaultPlugin->StringConverter->LoadFromString( variable, externalizedValue );
    return  variable.Size();
}


// #############################################################################################
// internals
// #############################################################################################
Priorities  Configuration::loadImpl( Variable& variable, bool substitute )
{
    variable.ClearValues();
    if ( variable.Name.IsEmpty() )
    {
        ALIB_WARNING( ASTR("Empty variable name given") );
        return Priorities::NONE;
    }

    // search variable
    Priorities priority= Priorities::NONE;
    for ( auto& ppp : plugins )
        if ( ppp.plugin->Load( variable ) )
        {
            priority= ppp.priority;
            break;
        }

    // not found?
    if ( !substitute || priority == Priorities::NONE )
        return Priorities::NONE;

    // substitution in all values of variable
    for ( int valueNo= 0; valueNo < variable.Size(); valueNo++ )
    {
        integer searchStartIdx=  0;
        int maxReplacements = 50;
        do
        {
            AString& value= *variable.GetString( valueNo );

            // search start
            integer repStart= value.IndexOf( SubstitutionVariableStart, searchStartIdx );
            if ( repStart < 0 )
                break;
            searchStartIdx= repStart;
            integer varStart= repStart + SubstitutionVariableStart.Length();

            Variable replVar;
            integer repLen;
            integer varLen;

            // search end in two different ways depending on setting of public field "SubstitutionVariableEnd"
            if ( SubstitutionVariableEnd.IsEmpty() )
            {
                integer idx=   value.IndexOfAny<Inclusion::Include>( SubstitutionVariableDelimiters, varStart );
                if ( idx < 0 )
                    idx= value.Length();

                varLen= idx - varStart;
                repLen= idx - repStart;
            }
            else
            {
                integer idx=   value.IndexOf   ( SubstitutionVariableEnd, varStart );
                if (idx < 0 )
                {
                    ALIB_WARNING( (String256() << "End of substitution variable not found (while start was found). Variable name: "
                                  << variable.Fullname << " Value: \"" << variable.GetString() << "\".").ToCString()  );
                    break;
                }

                varLen= idx - varStart;
                repLen= idx + SubstitutionVariableEnd.Length() - repStart;
            }

            // get variable name string
            Substring    replVarCategory;
            Substring    replVarName= value.Substring( varStart, varLen );
            if ( replVarName.IsEmpty() )
            {
                searchStartIdx+=   SubstitutionVariableStart.Length()
                                 + SubstitutionVariableEnd.Length();
                continue;
            }

            // parse category from name
            integer catSeparatorIdx= replVarName.IndexOf( '_' );
            if (catSeparatorIdx >= 0 )
            {
                replVarCategory= replVarName.Substring<false>( 0, catSeparatorIdx );
                replVarName    = replVarName.Substring       ( catSeparatorIdx + 1);
            }

            // load replacement variable
            if ( replVarName.IsNotEmpty() )
            {
                replVar.Declare( replVarCategory, replVarName, variable.Delim );
                loadImpl( replVar, false );
            }
            else
                replVar.ClearValues();

            // do the replacement (even if no variable was found)
            if ( replVar.Size() == 1 )
                value.ReplaceSubstring( replVar.GetString(), repStart, repLen );

            else if ( replVar.Size() > 1 )
            {
                variable.ReplaceValue( valueNo, replVar );

                // repeat replacements in current value, as current value changed
                valueNo--;
                break;
            }

            else
                value.ReplaceSubstring( ASTR(""),            repStart, repLen );

        }
        while( --maxReplacements );

        // warn if max replacements
        if( maxReplacements <= 0 )
            ALIB_WARNING((String256() << "Too many substitutions in variable " << variable.Fullname
                         << ". Probably a recursive variable definition was made. ").ToCString() );
    }
    return priority;
}


// #################################################################################################
// Iterator
// #################################################################################################
//! @cond NO_DOX
namespace {

class IteratorImpl : public Configuration::Iterator
{
    Configuration&  config;
    String          sectionName;

    // This map is used to detect if a variable found in a plug-in was already found in a higher
    // prioritized plug-in.
    UnorderedStringMap<bool>  variablesFound;

    ConfigurationPlugin::Iterator*  pluginIt;

    size_t                          nextPlugin;

    public:

    IteratorImpl( Configuration& configuration, const String& pSectionName )
    : config(configuration)
    , sectionName(pSectionName)
    , pluginIt   (nullptr)
    , nextPlugin  (0)
    {

    }

    virtual ~IteratorImpl() override
    {
        if( pluginIt )
            delete pluginIt;
    }

    virtual bool            Next()                                                          override
    {
        for(;;)
        {
            // next plugin?
            while( pluginIt == nullptr )
            {
                // end of iteration?
                if( nextPlugin >= config.CountPlugins() )
                    return false;

                // It might be that a plug-in is not iteratable and returns nullptr.
                // Therefore the while loop
                pluginIt= config.GetPlugin( nextPlugin++ )->GetIterator( sectionName );
            }

            // continue in case of end of plugin vars
            if( !pluginIt->Next(Actual) )
            {
                delete pluginIt;
                pluginIt= nullptr;
                continue;
            }

            // check if variable already existed
            if( variablesFound.find( Actual.Name ) != variablesFound.end() )
                continue;
            variablesFound[Actual.Name]= true;

            // set the config field and return success!
            Actual.Config= &config;
            return true;
        }
    }
};

} // anonymous namespace

Configuration::Iterator*       Configuration::GetIterator( const String& sectionName )
{
    return new IteratorImpl( *this, sectionName );
}


//! @endcond
}}}// namespace [aworx::lib::config]

