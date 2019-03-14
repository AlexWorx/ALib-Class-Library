// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

#if !defined(HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL)
    #include "alib/compatibility/std_strings_functional.hpp"
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

namespace aworx { namespace lib {


/** ************************************************************************************************
 * This \alibmod implements access, evaluation and writing of configuration variables which
 * might be defined through different sources, for example configuration files, command line
 * parameters, environment variables, or any custom storage type.
 *
 * Please consult \ref alib_mod_config "ALib Module Config - Programmer's Manual"
 * for further information.
 **************************************************************************************************/
namespace config {



// #################################################################################################
// Configuration
// #################################################################################################

Configuration::Configuration( bool addDefaultPlugins )
{
    TrueValues= { A_CHAR("1"), A_CHAR("true"), A_CHAR("t"), A_CHAR("yes"), A_CHAR("y"), A_CHAR("on"), A_CHAR("ok") };

    if( addDefaultPlugins )
    {
        InsertPlugin( new InMemoryPlugin(CONFIG.GetResource("CfgPlgDef")), Priorities::DefaultValues   , Responsibility::Transfer);
        InsertPlugin( new Environment   ()                               , Priorities::Environment     , Responsibility::Transfer);
        InsertPlugin( new CLIArgs       ()                               , Priorities::CLI             , Responsibility::Transfer);
        InsertPlugin( new InMemoryPlugin(CONFIG.GetResource("CfgPlgPro")), Priorities::ProtectedValues , Responsibility::Transfer);
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
                       "Utility method FetchFromDefault used without default plugin in place." )

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
            ALIB_ERROR( "Trying to store an undefined variable."  );
            return Priorities::NONE;
        }

        if ( variable.Size() > 1 && variable.Delim == '\0' )
        {
            ALIB_ERROR( "Trying to store variable {!Q} which has multiple values set but no delimiter defined.",
                        variable.Fullname );
            return Priorities::NONE;
        }
    }

    // set us as config
    variable.Config= this;

    // detect?
    if ( variable.Priority <= Priorities::NONE )
    {
        variable.Priority= Priorities::NONE;
        for ( auto& ppp : plugins )
            if ( ppp.plugin->Load( variable, true ) )
            {
                variable.Priority= ppp.priority;
                break;
            }

        if( variable.Priority == Priorities::ProtectedValues )
            return (variable.Priority= Priorities::NONE);
    }

    // new variables go to default
    if ( variable.Priority == Priorities::NONE )
        variable.Priority= Priorities::DefaultValues;

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
                       "Utility method StoreDefault used without default plugin in place." )

    if ( externalizedValue.IsNotNull() )
        defaultPlugin->StringConverter->LoadFromString( variable, externalizedValue );

    if ( variable.Size() == 0 && variable.DefaultValue.IsNotNull() )
        defaultPlugin->StringConverter->LoadFromString( variable, variable.DefaultValue );

    variable.Priority= Priorities::DefaultValues;
    return Store( variable, NullString() );
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
                       "Utility method Protect used without default plugin in place." )

    variable.Priority= Priorities::ProtectedValues;
    return Store( variable, NullString() );
}

int  Configuration::LoadFromString( Variable& variable, const String& externalizedValue )
{
    ALIB_LOCK
    ConfigurationPlugin* defaultPlugin= GetPlugin( Priorities::DefaultValues );
    ALIB_ASSERT_ERROR( defaultPlugin,
                       "Utility method LoadFromString used without default plugin in place." )
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
        ALIB_WARNING( "Empty variable name given" );
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
            AString& value= variable.GetString( valueNo );

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
                    ALIB_WARNING( "End of substitution variable not found (while start was found). "
                                  "Variable name: {} Value: {!Q}.",
                                  variable.Fullname, variable.GetString()  );
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
                value.ReplaceSubstring<false>( replVar.GetString(), repStart, repLen );

            else if ( replVar.Size() > 1 )
            {
                variable.ReplaceValue( valueNo, replVar );

                // repeat replacements in current value, as current value changed
                valueNo--;
                break;
            }

            else
                value.Delete<false>( repStart, repLen );

        }
        while( --maxReplacements );

        // warn if max replacements
        if( maxReplacements <= 0 )
        {
            ALIB_WARNING( "Too many substitutions in variable {!Q}. "
                          "Probably a recursive variable definition?", variable.Fullname );
        }
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

