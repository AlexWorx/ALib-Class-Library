// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/cli/commandline.hpp"
#   include "alib/cli/cliutil.hpp"
#   include "alib/lang/basecamp/basecamp.hpp"
#endif // !DOXYGEN


namespace alib::cli {

// #################################################################################################
// CommandLine Constructor
// #################################################################################################

void   CommandLine::Init( ResourcePool* resourcePool, NCString resCategory )
{
    Resources       =  resourcePool;
    ResourceCategory=  resCategory;

    ArgStrings.reserve( size_t(ARG_C) );
    ArgsLeft  .reserve( size_t(ARG_C) );

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE

    #if !ALIB_CHARACTERS_WIDE
        if( ARG_VN )
        {
            for ( int i= 1; i < ARG_C ; ++i )
            {
                ArgStrings.emplace_back( ARG_VN[i] );
                ArgsLeft  .emplace_back( i -1      );
            }
        }
        else
        {
            // convert wide to narrow strings
            NString1K converter;
            converter.DbgDisableBufferReplacementWarning();

            for ( int i= 1; i < ARG_C ; ++i )
            {
                converter.Reset() << ARG_VW[i];
                ArgStrings.emplace_back( String( allocator, converter) );
                ArgsLeft  .emplace_back( i - 1 );
            }
        }
    #else
        #if ALIB_CHARACTERS_NATIVE_WCHAR // use original strings only if alib::wchar == wchar_t
        if( ARG_VW )
        {
            for ( int i= 1; i < ARG_C ; ++i )
            {
                ArgStrings.emplace_back( ARG_VW[i] );
                ArgsLeft  .emplace_back( i - 1           );
            }
        }
        else
        #endif
        {
            // convert narrow to wide strings (or "wrong" wide width to width)
            String1K converter;
            converter.DbgDisableBufferReplacementWarning();

            for ( int i= 1; i < ARG_C ; ++i )
            {
                converter.Reset() << ARG_VN[i];
                ArgStrings.emplace_back( String(allocator, converter) );
                ArgsLeft  .emplace_back( i -1                      );
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
        auto   argNo=  ArgsLeft[size_t(argIdx)];
        String arg  =  GetArg(argNo);

        SHORTCUT_JUMP:

        // ignore non-option args
        if( arg.CharAtStart() != '-' )
        {
            ++argIdx;
            continue;
        }

        // create an option object and search decl with the actual argument
        {
            Option* option= allocator().New<Option>(this);

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
            if( option->ConsumedArguments > 0 )
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
                                ArgsLeft.begin() + argIdx +  option->ConsumedArguments );

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
        Command* command= allocator().New<Command>(this);
        ALIB_ASSERT_ERROR( CommandDecls.Count() > 0,  "CLI", "No commands declared." )
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

            if( command->ConsumedArguments > 0 )
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
        return NULL_STRING;

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



} // namespace alib::cli
