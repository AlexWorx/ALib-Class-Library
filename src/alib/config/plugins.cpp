// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/config/plugins.hpp"
#   include "alib/config/configcamp.hpp"
#   include "alib/config/configuration.hpp"
#   include "alib/lang/system/environment.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#endif // !DOXYGEN

namespace alib {  namespace config {


// #################################################################################################
// CLIVariablesPlugin
// #################################################################################################
CLIVariablesPlugin::CLIVariablesPlugin( MonoAllocator& ma, Priority pPriority )
: ConfigurationPlugin( pPriority)
, AlternativeArgs  (ma)
, DefaultCategories(ma)
{}

String CLIVariablesPlugin::Name()                                                              const
{
   return alib::CONFIG.GetResource( "CfgPlgCLI" );
}

bool  CLIVariablesPlugin::Get( const String& pName, AString& target )
{
    int           argC    = alib::ARG_C;
    bool          isWide;
    const void**  argV    = (isWide= (alib::ARG_VN == nullptr)) ? reinterpret_cast<const void**>(alib::ARG_VW)
                                                               : reinterpret_cast<const void**>(alib::ARG_VN);

    String256 name(pName); name.SearchAndReplace( '/', '_');
    Substring varNameWithoutCategory= nullptr;

    // check if a default category hits
    for (auto& defaultCategory : DefaultCategories )
        if( name.StartsWith( defaultCategory ) )
        {
            varNameWithoutCategory= name.Substring(defaultCategory.Length());
            varNameWithoutCategory.ConsumeChar('_');
            break;
        }
    String256   stringConverter;
    stringConverter.DbgDisableBufferReplacementWarning();


    auto argsIt= AlternativeArgs.begin();
    if( !AlternativeArgs.empty() )
    {
        argC  = int(AlternativeArgs.size());
    }

    for ( int i= !AlternativeArgs.empty() ? 0 : 1 ; i < argC ; ++i )
    {
        // create substring on actual variable (trim if somebody would work with quotation marks...)
        Substring cliArg;

        if( !AlternativeArgs.empty() )
        {
            cliArg= *argsIt;
            ++argsIt;
        }
        else
        {
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            if (!isWide)
            {
                if constexpr (!ATMP_EQ(character, char))
                {
                    stringConverter.Reset( reinterpret_cast<const char**>(argV)[i] );
                    cliArg= stringConverter;
                }
                else
                    cliArg= reinterpret_cast<const character**>(argV)[i];
            }
            else
            {
                if constexpr (!ATMP_EQ(character, wchar_t))
                {
                    stringConverter.Reset( reinterpret_cast<const wchar_t**>(argV)[i] );
                    cliArg= stringConverter;
                }
                else
                    cliArg= reinterpret_cast<const character**>(argV)[i];
            }
            ALIB_WARNINGS_RESTORE
        }

        cliArg.Trim();

        // consume and count hyphens, and check with our recognition settings.
        int qtyHyphens= 0;
        while( cliArg.ConsumeChar('-') )
            ++qtyHyphens;
        if(     qtyHyphens < QtyMandatoryHyphens
            ||  qtyHyphens > QtyOptionalHyphens  )
            continue;

        // try names
        if (    !                                         cliArg.ConsumeString<lang::Case::Ignore>( name )
             && !( varNameWithoutCategory.IsNotEmpty() && cliArg.ConsumeString<lang::Case::Ignore>( varNameWithoutCategory )  )
           )
            continue; // next arg

        if ( cliArg.Trim().IsEmpty()  ) // we return "yes, found!" even when no value is set.
            return true;
        if ( cliArg.ConsumeChar<CHK, lang::Whitespaces::Keep>() == '='  )
        {
            target.Reset(cliArg.Trim());
            return true;
        }
    }

    return false;
}

// #################################################################################################
// EnvironmentVariablesPlugin
// #################################################################################################
EnvironmentVariablesPlugin::EnvironmentVariablesPlugin( MonoAllocator&  ma,
                                                        Priority        pPriority   )
: ConfigurationPlugin( pPriority)
, DefaultCategories( ma )
{}

String  EnvironmentVariablesPlugin::Name()                                                     const
{
   return alib::CONFIG.GetResource( "CfgPlgEnv" );
}

bool  EnvironmentVariablesPlugin::Get( const String& pName,  AString& target )
{
    String256 result;
    result.DbgDisableBufferReplacementWarning();

    String256 varName(pName);
    varName.SearchAndReplace( '/', '_');
    Substring varNameWithoutCategory= nullptr;


    // check for full name
    EnvironmentVariables::Get( varName, result, lang::CurrentData::Keep );
    if ( result.IsNotEmpty() )
    {
        target.Reset(result);
        return true;
    }

    // check if a default category hits
    for (auto& defaultCategory : DefaultCategories )
        if( varName.StartsWith( defaultCategory ) )
        {
            varNameWithoutCategory= varName.Substring(defaultCategory.Length());
            varNameWithoutCategory.ConsumeChar('_');
            EnvironmentVariables::Get( String256(varNameWithoutCategory), result, lang::CurrentData::Keep );
            if ( result.IsNotEmpty() )
            {
                target.Reset(result);
                return true;
            }
        }
    
    // nothing found
    return false;
}

}} // namespace [alib::config]
