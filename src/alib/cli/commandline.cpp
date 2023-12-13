// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_CLI_COMMANDLINE)
#   include "alib/cli/commandline.hpp"
#endif

#if !defined (HPP_ALIB_CLI_CLIUTIL)
#   include "alib/cli/cliutil.hpp"
#endif

#if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#   include "alib/lib/fs_modules/distribution.hpp"
#endif
#endif // !defined(ALIB_DOX)


namespace aworx { namespace lib { namespace cli {

// #################################################################################################
// CommandLine Constructor
// #################################################################################################

void   CommandLine::Init( resources::ResourcePool* resourcePool, NCString resCategory )
{
    Resources       =  resourcePool;
    ResourceCategory=  resCategory;
    ArgcOriginal=  ALIB.ArgC;
    ArgWOriginal=  ALIB.ArgVW;
    ArgNOriginal=  ALIB.ArgVN;

    ArgStrings.reserve( static_cast<size_t>(ArgcOriginal) );
    ArgsLeft  .reserve( static_cast<size_t>(ArgcOriginal) );

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE

    #if !ALIB_CHARACTERS_WIDE
        if( ArgNOriginal )
        {
            for ( int i= 1; i < ArgcOriginal ; ++i )
            {
                ArgStrings.emplace_back( ArgNOriginal[i] );
                ArgsLeft  .emplace_back( i -1      );
            }
        }
        else
        {
            // convert wide to narrow strings
            NString1K converter;
            converter.DbgDisableBufferReplacementWarning();

            for ( int i= 1; i < ArgcOriginal ; ++i )
            {
                converter.Reset() << ArgWOriginal[i];
                ArgStrings.emplace_back( allocator.EmplaceString(converter) );
                ArgsLeft  .emplace_back( i - 1                              );
            }
        }
    #else
        #if ALIB_CHARACTERS_NATIVE_WCHAR // use original strings only if aworx::wchar == wchar_t
        if( ArgWOriginal )
        {
            for ( int i= 1; i < ArgcOriginal ; ++i )
            {
                ArgStrings.emplace_back( ArgWOriginal[i] );
                ArgsLeft  .emplace_back( i - 1           );
            }
        }
        else
        #endif
        {
            // convert narrow to wide strings (or "wrong" wide width to width)
            String1K converter;
            converter.DbgDisableBufferReplacementWarning();

            for ( int i= 1; i < ArgcOriginal ; ++i )
            {
                converter.Reset() << ArgNOriginal[i];
                ArgStrings.emplace_back( allocator.EmplaceString(converter) );
                ArgsLeft  .emplace_back( i -1                       );
            }
        }
    #endif
    ALIB_WARNINGS_RESTORE
}

// #################################################################################################
// Interface
// #################################################################################################

void CommandLine::ReadOptions()
{
    // loop over all arg indices in ArgsLeft
    integer argIdx= 0;
    while( argIdx < static_cast<integer>(ArgsLeft.size()) )
    {
        // get arg number and string once
        auto   argNo=  ArgsLeft[static_cast<size_t>(argIdx)];
        String arg  =  GetArg(argNo);

        SHORTCUT_JUMP:

        // ignore non-option args
        if( arg.CharAtStart() != '-' )
        {
            ++argIdx;
            continue;
        }

        // create an option object and search decl with actual argument
        {
            Option* option= allocator.Emplace<Option>(this);

            auto optionDeclIt= OptionDecls.begin();
            try
            {
                while( optionDeclIt != OptionDecls.end() )
                {
                    if( option->Read( **optionDeclIt, arg, argNo ) )
                        break;
                    ++optionDeclIt;
                }
            }
            catch ( Exception& e )
            {
                e.Add( ALIB_CALLER_NULLED, cli::Exceptions::ParsingOptions,
                       (*optionDeclIt)->HelpUsageLine()                    );
                throw;
            }

            // found a declaration?
            if( option->QtyArgsConsumed > 0 )
            {
                // shortcut to another option?
                OptionDecl& decl= *option->Declaration;
                if( decl.ShortcutReplacementString().IsNotEmpty() )
                {
                    arg= decl.ShortcutReplacementString();
                    goto SHORTCUT_JUMP;
                }

                // delete args and continue
                ArgsLeft.erase( ArgsLeft.begin() + argIdx,
                                ArgsLeft.begin() + argIdx +  option->QtyArgsConsumed );

                // move local option into the monotonic memory add to the list for this option type.
                Options.PushBack( option );
                continue;
            }


            // erase args that start with '-' and put them into field OptionsIgnored.
            if( ArgsLeft.size() > 0 )
            {
                OptionArgsIgnored.PushBack( GetArg(argNo) );
                ArgsLeft.erase( ArgsLeft.begin() + argIdx );
            }
        }
    }
}

Option*  CommandLine::GetOption( Enum element )
{
    for( auto optionIt= Options.rbegin() ; optionIt != Options.rend() ; optionIt ++ )
        if( (*optionIt)->Declaration->Element() == element )
            return *optionIt;
    return nullptr;
}


void CommandLine::ReadNextCommands()
{
    // loop over all arg indices in ArgsLeft
    bool   lastCommandFullyParsed= true;
    while( lastCommandFullyParsed &&  ArgsLeft.size() > 0 )
    {
        // create a command object and search decl with actual argument
        Command* command= allocator.Emplace<Command>(this);
        ALIB_ASSERT_ERROR( CommandDecls.Size() > 0,  "CLI", "No commands declared." )
        for( auto* commandDecl : CommandDecls )
        {
            try
            {
                lastCommandFullyParsed= command->Read( *commandDecl );
            }
            catch ( Exception& e )
            {
                e.Add( ALIB_CALLER_NULLED, cli::Exceptions::ParsingCommand,
                       CLIUtil::GetCommandUsageFormat( *this, *commandDecl ),
                       commandDecl->HelpTextShort()                    );
                throw;
            }

            if( command->QtyArgsConsumed > 0 )
            {
                CommandsParsed.PushBack( command );
                if( NextCommandIt == CommandsParsed.end() )
                    --NextCommandIt;
                break;
            }
        }
    }
}

Command* CommandLine::NextCommand()
{
    if( NextCommandIt == CommandsParsed.end() )
        ReadNextCommands();
    if( NextCommandIt == CommandsParsed.end() )
    {
        // check for arguments left which got not recognized
        if( ArgsLeft.size() > 0 )
            throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::UnknownCommand,
                             ArgsLeft[0], PeekArg()   );

        // check for no command
        if ( CommandsParsed.IsEmpty() )
            throw Exception(ALIB_CALLER_NULLED,  cli::Exceptions::NoCommandGiven );

        return nullptr;
    }

    auto* result= *NextCommandIt;
    ++NextCommandIt;
    return result;
}


String  CommandLine::PopArg()
{
    if( ArgsLeft.size() == 0)
        return NullString();

    String result= GetArg(ArgsLeft[0]);
    ArgsLeft.erase( ArgsLeft.begin() );
    return result;
}

void  CommandLine::RemoveArg( integer argNo )
{
    for( auto it= ArgsLeft.begin() ; it != ArgsLeft.end() ; ++it )
        if( *it == argNo )
        {
            ArgsLeft.erase( it );
            return;
        }
    ALIB_ERROR( "CLI", "Argument number {} already removed.", argNo )
}



}}}// namespace aworx::lib::system
