// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#      include "alib/config/configuration.hpp"
#   endif
#   if !defined(HPP_ALIB_CONFIG_CONFIG)
#      include "alib/config/config.hpp"
#   endif

#   if !defined(HPP_ALIB_RESULTS_REPORT)
#      include "alib/results/report.hpp"
#   endif

#   if !defined(HPP_ALIB_MONOMEM_HASHSET)
#      include "alib/monomem/hashset.hpp"
#   endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib {


/** ************************************************************************************************
 * This is the namespace of\alibmod, which implements access, evaluation and writing of
 * configuration variables. Variable data can be load from different sources, for example
 * configuration files, command line parameters, environment variables, or any custom storage type.
 *
 * Please consult \ref alib_mod_config "ALib Module Config - Programmer's Manual"
 * for further information.
 **************************************************************************************************/
namespace config {



// #################################################################################################
// Configuration
// #################################################################################################

Configuration::Configuration( CreateDefaults addPlugins )
{
    TrueValues= { A_CHAR("1"), A_CHAR("true"), A_CHAR("t"), A_CHAR("yes"), A_CHAR("y"), A_CHAR("on"), A_CHAR("ok") };

    if( addPlugins == CreateDefaults::Yes )
    {
        InsertPlugin( new InMemoryPlugin(CONFIG.GetResource("CfgPlgDef") ), Priorities::DefaultValues   , Responsibility::Transfer);
        InsertPlugin( new Environment   (                                ), Priorities::Environment     , Responsibility::Transfer);
        InsertPlugin( new CLIArgs       (                                ), Priorities::CLI             , Responsibility::Transfer);
        InsertPlugin( new InMemoryPlugin(CONFIG.GetResource("CfgPlgPro") ), Priorities::ProtectedValues , Responsibility::Transfer);
    }
}


int Configuration::FetchFromDefault( ConfigurationPlugin& dest, const String& sectionName  )
{
    InMemoryPlugin* defaultPlugin= GetPluginTypeSafe<InMemoryPlugin>( Priorities::DefaultValues );
    ALIB_ASSERT_ERROR( defaultPlugin,
                       "Utility method FetchFromDefault used without default plugin in place." )

    int cntCopied= 0;
    Variable variable;
    for( auto& section : defaultPlugin->sections )
    {
        if( sectionName.IsNotEmpty() && !sectionName.Equals( section.Name() ) )
            continue;

        for( auto entryIt= section.entries.begin() ; entryIt != section.entries.end() ; ++entryIt )
        {
            if( !dest.Load( variable.Declare( section.name, entryIt->Name() ), true ) )
            {
                defaultPlugin->Load  ( variable );
                dest.Store( variable );
                ++cntCopied;
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
    variable.SetConfig  (this);
    variable.SetPriority( loadImpl( variable, true ) );

    if (    variable.Priority()    == Priorities::NONE
         && variable.DefaultValue().IsNotNull()         )
        Store( variable, variable.DefaultValue() );

    return variable.Priority();
}

Priorities Configuration::Store( Variable& variable, const String& externalizedValue )
{
    // checks
    if( externalizedValue.IsNull() )
    {
        if ( variable.Name().IsEmpty())
        {
            ALIB_ERROR( "Trying to store an undefined variable."  )
            return Priorities::NONE;
        }

        if ( variable.Size() > 1 && variable.Delim() == '\0' )
        {
            ALIB_ERROR( "Trying to store variable {!Q} which has multiple values set but no delimiter defined.",
                        variable.Fullname() )
            return Priorities::NONE;
        }
    }

    // set us as config
    variable.SetConfig( this );

    // detect?
    if ( variable.Priority() <= Priorities::NONE )
    {
        variable.SetPriority( Priorities::NONE );
        for ( auto& ppp : plugins )
            if ( ppp.plugin->Load( variable, true ) )
            {
                variable.SetPriority( ppp.priority );
                break;
            }

        if( variable.Priority() == Priorities::ProtectedValues )
        {
            variable.SetPriority( Priorities::NONE);
            return Priorities::NONE;
        }
    }

    // new variables go to default
    if ( variable.Priority() == Priorities::NONE )
        variable.SetPriority( Priorities::DefaultValues);

    // store
    for ( auto& ppp : plugins )
        if (    ppp.priority <= variable.Priority()
             && ppp.plugin->Store( variable, externalizedValue ) )
            {
                variable.SetPriority(ppp.priority);
                return ppp.priority;
            }

    variable.SetPriority( Priorities::NONE );
    return Priorities::NONE;
}


// #################################################################################################
 // convenience methods using Configuration::Default singleton
 // #################################################################################################
Priorities  Configuration::StoreDefault( Variable& variable, const String& externalizedValue )
{
    ConfigurationPlugin* defaultPlugin= GetPlugin( Priorities::DefaultValues );
    ALIB_ASSERT_ERROR( defaultPlugin,
                       "Utility method StoreDefault used without default plugin in place." )

    if ( externalizedValue.IsNotNull() )
        defaultPlugin->StringConverter->LoadFromString( variable, externalizedValue );

    if ( variable.Size() == 0 && variable.DefaultValue().IsNotNull() )
        defaultPlugin->StringConverter->LoadFromString( variable, variable.DefaultValue() );

    variable.SetPriority( Priorities::DefaultValues );
    return Store( variable, NullString() );
}

Priorities  Configuration::Protect( Variable& variable, const String& externalizedValue )
{
    ConfigurationPlugin* protectedPlugin= GetPlugin( Priorities::ProtectedValues );

    if ( externalizedValue.IsNotNull() )
        protectedPlugin->StringConverter->LoadFromString( variable, externalizedValue );

    if ( variable.Size() == 0 && variable.DefaultValue().IsNotNull() )
        protectedPlugin->StringConverter->LoadFromString( variable, variable.DefaultValue() );

    ALIB_ASSERT_ERROR( protectedPlugin,
                       "Utility method Protect used without default plugin in place." )

    variable.SetPriority( Priorities::ProtectedValues );
    return Store( variable, NullString() );
}

integer  Configuration::LoadFromString( Variable& variable, const String& externalizedValue )
{
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
    if ( variable.Name().IsEmpty() )
    {
        ALIB_WARNING( "Empty variable name given" )
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
    String512 valBuffer; valBuffer.DbgDisableBufferReplacementWarning();
    for ( int valueNo= 0; valueNo < variable.Size(); ++valueNo )
    {
        integer searchStartIdx  =  0;
        int     maxReplacements = 50;
        do
        {
            const String& value= variable.GetString( valueNo );

            // search start
            integer repStart= value.IndexOf( SubstitutionVariableStart, searchStartIdx );
            if ( repStart < 0 )
                break;
            searchStartIdx= repStart;
            integer varStart= repStart + SubstitutionVariableStart.Length();

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
                                  variable.Fullname(), variable.GetString()  )
                    break;
                }

                varLen= idx - varStart;
                repLen= idx + SubstitutionVariableEnd.Length() - repStart;
            }

            // get variable name string
            Substring    replVarCategory(nullptr);
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
                tempVar.Declare( replVarCategory, replVarName, variable.Delim() );
                loadImpl( tempVar, false );
            }
            else
                tempVar.Reset();

            // do the replacement (even if no variable was found)
            if ( tempVar.Size() > 1 )
            {
                variable.ReplaceValue( valueNo, tempVar );

                // repeat replacements in current value, as current value changed
                --valueNo;
                break;
            }
            else
            {
                valBuffer.Reset(value);
                if ( tempVar.Size() == 1 )
                    valBuffer.ReplaceSubstring<false>( tempVar.GetString(), repStart, repLen );
                else
                    valBuffer.Delete<false>( repStart, repLen );
                variable.ReplaceValue( valueNo, valBuffer );
            }

        }
        while( --maxReplacements );

        // warn if max replacements
        if( maxReplacements <= 0 )
        {
            ALIB_WARNING( "Too many substitutions in variable {!Q}. "
                          "Probably a recursive variable definition?", variable.Fullname() )
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
    Configuration&                  config;
    String                          sectionName;

    MonoAllocator                   allocator;
    HashSet<String>                 variablesFound;

    ConfigurationPlugin::Iterator*  pluginIt;

    integer                         nextPlugin;

    public:

    IteratorImpl( Configuration& configuration, const String& pSectionName )
    : config        (configuration)
    , sectionName   (pSectionName)
    , allocator     (4096)
    , variablesFound(&allocator)
    , pluginIt      (nullptr)
    , nextPlugin    (0)
    {}

    virtual ~IteratorImpl()                                                                 override
    {
        if( pluginIt )
            delete pluginIt;
    }

    void ResetToSection( const String& pSectionName )                                       override
    {
        sectionName= pSectionName;
        variablesFound.Reset();
        allocator.Reset();
        if( pluginIt )
        {
            delete pluginIt;
            pluginIt= nullptr;
        }
        nextPlugin= 0;

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
            if( variablesFound.InsertIfNotExistent( Actual.Name() ).second == false )
                continue;

            // set the config field and return success!
            Actual.SetConfig( &config );
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

