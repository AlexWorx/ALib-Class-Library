// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_CLI_CLIAPP)
#   include "alib/cli/cliapp.hpp"
#endif

#if !defined (HPP_ALIB_CLI_CLIUTIL)
#   include "alib/cli/cliutil.hpp"
#endif

#if !defined (HPP_ALIB_LIB_ALIBMODULES)
#   include "alib/lib/alibmodules.hpp"
#endif

namespace aworx { namespace lib { namespace cli {

// #################################################################################################
// CLIApp Constructor
// #################################################################################################

void   CLIApp::Init( Module* resModule )
{
    ResModule   =  resModule;
    ArgcOriginal=  ALIB.ArgC;
    ArgWOriginal=  ALIB.ArgVW;
    ArgNOriginal=  ALIB.ArgVN;

    ArgStrings.reserve( static_cast<size_t>( ArgcOriginal ) );
    ArgsLeft  .reserve( static_cast<size_t>( ArgcOriginal ) );

#if ALIB_CHARACTERS_ARE_NARROW
    if( ArgNOriginal )
    {
        for ( int i= 1; i < ArgcOriginal ; i++ )
        {
            ArgStrings.emplace_back( ArgNOriginal[i] );
            ArgsLeft  .emplace_back( i -1      );
        }
    }
    else
    {
        // convert wide to narrow strings
        convertedArgStrings.reserve( static_cast<size_t>( ArgcOriginal ) );
        for ( int i= 1; i < ArgcOriginal ; i++ )
        {
            convertedArgStrings.emplace_back( ArgWOriginal[i]                   );
            ArgStrings         .emplace_back( convertedArgStrings.back() );
            ArgsLeft           .emplace_back( i -1                       );
        }
    }
#else
    #if ALIB_CHARACTERS_NATIVE_WCHAR // use original strings only if aworx::wchar == wchar_t
    if( ArgWOriginal )
    {
        for ( int i= 1; i < ArgcOriginal ; i++ )
        {
            ArgStrings.emplace_back( ArgWOriginal[i] );
            ArgsLeft  .emplace_back( i - 1           );
        }
    }
    else
    #endif
    {
        // convert narrow to wide strings (or "wrong" wide width to width)
        convertedArgStrings.reserve( static_cast<size_t>( ArgcOriginal ) );
        for ( int i= 1; i < ArgcOriginal ; i++ )
        {
            convertedArgStrings.emplace_back( ArgNOriginal[i]            );
            ArgStrings         .emplace_back( convertedArgStrings.back() );
            ArgsLeft           .emplace_back( i -1                       );
        }
    }
#endif


}

// #################################################################################################
// Interface
// #################################################################################################

void CLIApp::ReadOptions()
{
    // loop over all arg indices in ArgsLeft
    size_t argIdx= 0;
    while( argIdx < ArgsLeft.size() )
    {
        // get arg number and string once
        size_t argNo=  ArgsLeft  [argIdx];
        String arg=    ArgStrings[argNo ];

        SHORTCUT_JUMP:

        // ignore non-option args
        if( arg.CharAtStart() != '-' )
        {
            argIdx++;
            continue;
        }

        // create an option object and search decl with actual argument
        Option option(this);
        size_t loopDeclNo;
        try
        {
            for( loopDeclNo= 0 ; loopDeclNo < OptionDecls.size() ; ++loopDeclNo )
                if( option.Read( OptionDecls[loopDeclNo], arg, argNo ) )
                    break;
        }
        catch ( Exception& e )
        {
            e.Add( ALIB_CALLER_NULLED, cli::Exceptions::ParsingOptions,
                   OptionDecls[loopDeclNo].HelpUsageLine()                    );
            throw;
        }

        // found?
        if( option.QtyArgsConsumed > 0 )
        {
            OptionDecl& decl= *option.Declaration;
            if( decl.ShortcutReplacementString().IsNotEmpty() )
            {
                arg= decl.ShortcutReplacementString();
                goto SHORTCUT_JUMP;
            }

            auto& instanceList=  OptionsFound[decl.ID];

            // move previous?
            if( decl.MultiIgnored() && instanceList.size() > 0 )
            {
                OptionsOverwritten[decl.ID].emplace_back( instanceList.back() );
                instanceList.pop_back();
            }

            // add current
            instanceList.emplace_back( option );

            // delete args and continue
            ArgsLeft.erase( ArgsLeft.begin() + static_cast<int>( argIdx ),
                            ArgsLeft.begin() + static_cast<int>( argIdx ) + static_cast<int>( option.QtyArgsConsumed ) );
            continue;
        }


        // erase args that start with '-' and put them into field OptionsIgnored.
        if( ArgsLeft.size() > 0 )
        {
            OptionArgsIgnored.emplace_back( ArgStrings[argNo] );
            ArgsLeft.erase( ArgsLeft.begin() + static_cast<int>( argIdx ) );
        }
    }
}


void CLIApp::ReadNextCommands()
{
    // loop over all arg indices in ArgsLeft
    bool   lastCommandFullyParsed= true;
    while( lastCommandFullyParsed &&  ArgsLeft.size() > 0 )
    {
        // create a command object and search decl with actual argument
        Command command(this);
        for( size_t loopDeclNo= 0 ; loopDeclNo < CommandDecls.size() ; ++loopDeclNo )
        {
            try
            {
                lastCommandFullyParsed= command.Read( CommandDecls[loopDeclNo] );
            }
            catch ( Exception& e )
            {
                e.Add( ALIB_CALLER_NULLED, cli::Exceptions::ParsingCommand,
                       CLIUtil::GetCommandUsageFormat( *this, CommandDecls[loopDeclNo] ),
                       CommandDecls[loopDeclNo].HelpTextShort()                    );
                throw;
            }

            if( command.QtyArgsConsumed > 0 )
            {
                CommandsParsed.emplace_back( command );
                break;
            }
        }
    }
}

Command* CLIApp::NextCommand()
{
    if( NextCommandNo == CommandsParsed.size() )
        ReadNextCommands();
    if( NextCommandNo == CommandsParsed.size() )
    {
        // check for arguments left which got not recognized
        if( ArgsLeft.size() > 0 )
            throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::UnknownCommand,
                             ArgsLeft[0], PeekArg()   );

        // check for no command
        if ( CommandsParsed.size() == 0 )
            throw Exception(ALIB_CALLER_NULLED,  cli::Exceptions::NoCommandGiven );

        return nullptr;
    }

    return &CommandsParsed[ NextCommandNo++ ];
}


String  CLIApp::PopArg()
{
    if( ArgsLeft.size() == 0)
        return NullString();

    String result= ArgStrings[ArgsLeft[0]];
    ArgsLeft.erase( ArgsLeft.begin() );
    return result;
}

void  CLIApp::RemoveArg( size_t argNo )
{
    for( auto it= ArgsLeft.begin() ; it != ArgsLeft.end() ; ++it )
        if( *it == argNo )
        {
            ArgsLeft.erase( it );
            return;
        }
    ALIB_ERROR( "Argument number {} already removed.", argNo )
}



}}}// namespace aworx::lib::system

