// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/expressions/standardrepository.hpp"
#   include "alib/config/configuration.hpp"
#endif // !DOXYGEN

namespace alib {  namespace expressions {

int StandardRepository::StoreLoadedExpressions( Compiler*  compiler, Priority slot )
{
    int count= 0;
    for( auto& entry : VariablesLoaded )
        if( std::get<0>( entry ) == slot )
        {
            ++count;
            Variable var(*config, std::get<1>( entry ), A_CHAR("S") );
            var= compiler->GetNamed( std::get<2>( entry ))->GetNormalizedString();
        }

    return count;
}

bool StandardRepository::Get( const String& identifier, AString& target )
{
    if( config != nullptr )
    {
        if( ConfigPaths.empty() )
            ConfigPaths.emplace_back( "" );

        // search in given default categories first.
        Variable var(*config);
        for( auto& path : ConfigPaths )
        {
            String256 name(path); name << '/' <<  identifier;
            if( var.Try(name, A_CHAR("S") ) && var.IsDefined() )
            {
                target << var.GetString();
                VariablesLoaded.emplace_back( var.GetPriority(), name, identifier );
                return true;
            }
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

}} // namespace [alib::expressions]
