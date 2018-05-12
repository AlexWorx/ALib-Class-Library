// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_CLI_LIB)
    #include "alib/cli/clilib.hpp"
#endif

#if !defined (HPP_ALIB_CLI_CLIUTIL)
    #include "alib/cli/cliutil.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_FORMAT_SIMPLETEXT)
#   include "alib/strings/format/simpletext.hpp"
#endif

#if !defined (HPP_ALIB_SYSTEM_PROCESSINFO)
    #include "alib/system/process.hpp"
#endif

#include <algorithm>


using namespace std;

namespace aworx { namespace lib { namespace cli {





// #################################################################################################
// CLIApp Constructor
// #################################################################################################

CLIApp::CLIApp( Library& resLibrary, int argc, wchar_t *argvW[], char *argvN[] )
: ResLibrary ( resLibrary )
{
    ArgcOriginal=   argc;
    ArgWOriginal=  argvW;
    ArgNOriginal=  argvN;

    ArgStrings.reserve( static_cast<size_t>( argc ) );
    ArgsLeft  .reserve( static_cast<size_t>( argc ) );

#if ALIB_NARROW_STRINGS
    if( argvN )
    {
        for ( int i= 1; i < argc ; i++ )
        {
            ArgStrings.emplace_back( argvN[i] );
            ArgsLeft  .emplace_back( i -1      );
        }
    }
    else
    {
        // convert wide to narrow strings
        convertedArgStrings.reserve( static_cast<size_t>( argc ) );
        for ( int i= 1; i < argc ; i++ )
        {
            convertedArgStrings.emplace_back( argvW[i]                   );
            ArgStrings         .emplace_back( convertedArgStrings.back() );
            ArgsLeft           .emplace_back( i -1                       );
        }
    }
#else
    if( argvW )
    {
        for ( int i= 1; i < argc ; i++ )
        {
            ArgStrings.emplace_back( argvW[i] );
            ArgsLeft  .emplace_back( i -1      );
        }
    }
    else
    {
        // convert narrow to wide strings
        convertedArgStrings.reserve( static_cast<size_t>( argc ) );
        for ( int i= 1; i < argc ; i++ )
        {
            convertedArgStrings.emplace_back( argvN[i]                  );
            ArgStrings         .emplace_back( convertedArgStrings.back() );
            ArgsLeft           .emplace_back( i -1                       );
        }
    }
#endif


}

CLIApp::~CLIApp()
{
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

            auto& instanceList=  OptionsFound[decl.EnumBox];

            // move previous?
            if( decl.MultiIgnored() && instanceList.size() > 0 )
            {
                OptionsOverwritten[decl.EnumBox].emplace_back( instanceList.back() );
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
                       CLIUtil::GetCommandUsageFormat( CommandDecls[loopDeclNo] ),
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
        return NullString;

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

