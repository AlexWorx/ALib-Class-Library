// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_EXPRESSIONS_STANDARD_REPOSITORY)
#      include "alib/expressions/standardrepository.hpp"
#   endif
#   if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#      include "alib/config/configuration.hpp"
#   endif
#   if !defined (HPP_ALIB_CONFIG_VARIABLE)
#      include "alib/config/variable.hpp"
#   endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace expressions {

int StandardRepository::StoreLoadedExpressions( Compiler*  compiler, Priorities slot )
{
    int count= 0;
    for( auto& entry : VariablesLoaded )
        if( std::get<0>( entry ) == slot )
        {
            ++count;
            var.Declare( std::get<1>( entry ), std::get<2>( entry ) );
            var.SetPriority( slot );
            var.Add( compiler->GetNamed( std::get<3>( entry ))->GetNormalizedString() );
            config->Store( var );
        }

    return count;
}

bool StandardRepository::Get( const String& identifier, AString& target )
{
    if( config != nullptr )
    {
        // search in given default categories first.
        for( auto& category : DefaultCategories )
        {
            if(  config->Load( var.Declare( category, identifier ) ) != Priorities::NONE )
            {
                target << var.GetString();
                VariablesLoaded.emplace_back( var.Priority(), category, identifier, identifier );
                return true;
            }
        }

        // try to split identifier in "category_name"
        integer underscorePos=  identifier.IndexOf( '_', 1 );
        while( underscorePos > 0 )
        {
            Substring name;
            Substring category(identifier);
            category.Split( underscorePos, name, 1 );
            if(  config->Load( var.Declare( category, name ) ) != Priorities::NONE )
            {
                target << var.GetString();
                VariablesLoaded.emplace_back( var.Priority(), category, name, identifier );
                return true;
            }
            underscorePos=  identifier.IndexOf( '_', underscorePos + 1 );
        }
    }

    if( resources != nullptr )
    {
        auto& result= resources->Get( resourceCategory, identifier ALIB_DBG(, false) );
        if( result.IsNotNull() )
        {
            target << result;
            return true;
        }
    }


    // failed
    return false;
}



}}} // namespace [aworx::lib::expressions]

