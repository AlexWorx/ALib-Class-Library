// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/cli/cliutil.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#   include "alib/enums/serialization.hpp"
#   include "alib/monomem/aliases/stdvector.hpp"
#endif // !DOXYGEN

#include <vector>
#include <algorithm>

namespace alib::cli {

OptionDecl*  CLIUtil::GetOptionDecl( CommandLine& cmdLine, const String& identString )
{
    for( auto* decl : cmdLine.OptionDecls )
        if (    (    identString.Length()              == 1
                  && identString.CharAtStart<NC>()  == decl->IdentifierChar() )
             || (    identString.Length()              >= decl->MinimumRecognitionLength()
                 &&  decl->Identifier().StartsWith<CHK,lang::Case::Ignore>( identString ) )  )
            return decl;
    return nullptr;
}


CommandDecl*  CLIUtil::GetCommandDecl( CommandLine& cmdLine, const String& identString )
{
    for( auto* decl : cmdLine.CommandDecls )
        if (     identString.Length() >=  decl->MinimumRecognitionLength()
             &&  decl->Identifier().StartsWith<CHK,lang::Case::Ignore>( identString ) )
            return decl;
    return nullptr;
}

ParameterDecl*  CLIUtil::GetParameterDecl(CommandLine &cmdLine, const String &identString)
{
    for( auto* decl : cmdLine.ParameterDecls )
        if ( decl->Name().StartsWith<CHK,lang::Case::Ignore>( identString ) )
            return decl;
    return nullptr;
}



AString CLIUtil::GetCommandUsageFormat( CommandLine& cmdLine, CommandDecl& cmd )
{
    AString result;
    result.EnsureRemainingCapacity(2048);
    result << cmdLine.GetResource( "HlpCLIAppName" ) << ' ' << cmd.Identifier();

    for( auto* param : cmd.Parameters )
    {
        result << ' ';
        if( param->IsOptional() ) result << '[';

        result << param->Name();
        if ( param->ValueListSeparator() != '\0' )
        {
            result << '[' << param->ValueListSeparator() << param->Name() << "...]";
        }

        if( param->IsOptional() ) result << ']';

    }
    return result;
}

bool CLIUtil::GetHelp( CommandLine& cmdLine, Command* helpCmd, Option* helpOpt, Paragraphs& text )
{
    text.AddMarked( cmdLine.AppInfo );

    String argList= NULL_STRING;
    bool   argWasPeeked= false;
    if( helpCmd )
    {
        if(  helpCmd->ParametersOptional.IsNotEmpty() )
            argList= helpCmd->ParametersOptional.Front()->Args.Front();
        else if( cmdLine.ArgCount() > helpCmd->Position + 1  )
        {
            argWasPeeked= true;
            argList= cmdLine.GetArg(helpCmd->Position + 1);
        }
    }
    else
    {
        if(  helpOpt->Args.IsNotEmpty() )
            argList= helpOpt->Args.Front();
        else if( cmdLine.ArgCount() > helpOpt->Position + 1 )
        {
            argWasPeeked= true;
            argList= cmdLine.GetArg(helpOpt->Position + 1);
        }
    }

    if( argList.IsNotEmpty() )
    {
        int cntArgsRecognized= 0;

        Tokenizer args( argList, ',' );
        int cntTokens= 0;
        while( args.HasNext() )
        {
            ++cntTokens;

            String arg= args.Next();
            int cntArgsRecognizedSoFar= cntArgsRecognized;

            // command
            {
                CommandDecl* cmdDecl= CLIUtil::GetCommandDecl( cmdLine, arg );
                if( cmdDecl )
                {
                    ++cntArgsRecognized;

                    if( cmdLine.DryRun == DryRunModes::Off )
                    {
                        text.Add( cmdLine.GetResource( "HlpHdlTopic" ), "command", cmdDecl->Identifier() )
                            .PushIndent( 2 )
                            .Add( cmdLine.GetResource( "HlpHdlUsage" ), " ", GetCommandUsageFormat( cmdLine, *cmdDecl ) )

                            .Add( NEW_LINE, cmdLine.GetResource( "HlpHdlDscr" ) )
                            .PushIndent( 2 )
                              .AddMarked( cmdDecl->HelpTextLong(), NEW_LINE )
                            .PopIndent()

                            .Add( NEW_LINE, cmdLine.GetResource( "HlpHdlPDscr" ) )
                            .PushIndent( 2 );
                            for( auto* param : cmdDecl->Parameters )
                            {
                                text.Add( "* ", param->Name()  )
                                    .PushIndent( 2 )
                                      .AddMarked( param->GetHelpTextShort() )
                                    .PopIndent()
                                   .Add( NEW_LINE );
                            }
                        text.PopIndent()
                            .PopIndent();
                    }
                }
            }

            // option?
            if( cntArgsRecognizedSoFar == cntArgsRecognized )
            {
                OptionDecl* optDecl= CLIUtil::GetOptionDecl( cmdLine, arg );
                if( optDecl )
                {
                    ++cntArgsRecognized;
                    if( cmdLine.DryRun == DryRunModes::Off )
                    {
                        text.Add( cmdLine.GetResource( "HlpHdlTopic" ), "option", optDecl->Identifier() )
                            .PushIndent( 2 )
                            .Add( cmdLine.GetResource( "HlpHdlUsage" ), "  ", optDecl->HelpUsageLine() )
                            .Add( NEW_LINE, cmdLine.GetResource( "HlpHdlDscr" ) )
                            .PushIndent( 2 )
                                .AddMarked( optDecl->HelpText(), NEW_LINE )
                            .PopIndent()

                            .PopIndent();
                    }
                }
            }

            // parameter?
            if( cntArgsRecognizedSoFar == cntArgsRecognized )
            {
                ParameterDecl* paramDecl= CLIUtil::GetParameterDecl( cmdLine, arg );
                if( paramDecl )
                {
                    ++cntArgsRecognized;
                    if( cmdLine.DryRun == DryRunModes::Off )
                    {
                        text.Add( cmdLine.GetResource( "HlpHdlTopic" ), "parameter", paramDecl->Name() )
                            .PushIndent( 2 )
                            .AddMarked( paramDecl->GetHelpTextLong() )
                            .PopIndent();
                    }
                }
            }

            // special help topic?
            if( cntArgsRecognizedSoFar == cntArgsRecognized )
            {
                auto& additionalHelpTopics= cmdLine.TryResource("HlpAddnlTopics");
                if( additionalHelpTopics.IsNotEmpty() )
                {
                    Tokenizer topics(additionalHelpTopics, ',');
                    while(topics.Next().IsNotEmpty())
                    {
                        if(topics.Actual.StartsWith<CHK,lang::Case::Ignore>( arg ) )
                        {
                            ++cntArgsRecognized;
                            if( cmdLine.DryRun == DryRunModes::Off )
                                text.AddMarked( cmdLine.GetResource( NString64("HlpAddnl")._( topics.Actual ) ) );

                            break;
                        }
                    }
                }
            }
        }


        // Add arg to command object if at least one token matched
        if( cntArgsRecognized > 0 && argWasPeeked )
        {
            if( helpCmd )
            {
                cmdLine.RemoveArg( helpCmd->Position + 1 );
                ++helpCmd->ConsumedArguments;
            }
            else
            {
                cmdLine.RemoveArg( helpOpt->Position + 1 );
                ++helpOpt->ConsumedArguments;
                helpOpt->Args.PushBack( argList );
            }
        }

        // not peeked means "--help=arg" was given. In this case, the argument has to be recognized.
        // Also in the case that more than one token was read but the recognized qty is smaller.
        if (    ( !argWasPeeked && cntArgsRecognized == 0 )
             || ( cntTokens > 1 &&  cntArgsRecognized < cntTokens ) )
           return false;

        // consumed arg?
        if( cntArgsRecognized > 0 )
            return true;
    }

    // general help
    text.AddMarked( cmdLine.GetResource( "HlpGeneral" ) );

    text.Add( cmdLine.GetResource( "HlpHdlExtCds") )
        .PushIndent( 2 );

    {
        // sort the exit codes by their number
        auto snapshot= cmdLine.allocator.TakeSnapshot();
        StdVectorMono<std::pair<Enum, ExitCodeDecl *>>
                sortedExitCodes(cmdLine.allocator);
        for( auto declIt : cmdLine.ExitCodeDecls )
            sortedExitCodes.emplace_back(declIt);
        std::sort( sortedExitCodes.begin(), sortedExitCodes.end(),
                []( std::pair<Enum, ExitCodeDecl *>& lhs,
                    std::pair<Enum, ExitCodeDecl *>& rhs)
                {
                     return lhs.first.Integral() < rhs.first.Integral();
                }
               );

        for( auto declIt : sortedExitCodes )
            text.Add( "  {:>3}: {}\n       {}", declIt.first.Integral(),
                                                declIt.second->Name(),
                                                declIt.second->FormatString() );
        cmdLine.allocator.Reset(snapshot);
    }
    text.PopIndent();

    text.Add( cmdLine.GetResource( "HlpHdlUsage" ) )
        .PushIndent( 2 )
        .Add( cmdLine.GetResource( "HlpUsage"    ) )
        .PopIndent()
        .Add( NEW_LINE, cmdLine.GetResource(       "HlpHdlOpts"  ) )
        .PushIndent( 2 );
    for( auto* decl : cmdLine.OptionDecls )
        text.Add( decl->HelpUsageLine() );

    text.PopIndent();

    text.Add( NEW_LINE, cmdLine.GetResource(       "HlpHdlCmds"  ) )
        .PushIndent( 2 );
    for( auto* decl : cmdLine.CommandDecls )
    {
        text.Add( "* ", GetCommandUsageFormat( cmdLine, *decl ), NEW_LINE )
            .PushIndent( 2 )
              .Add( decl->HelpTextShort(), NEW_LINE )
            .PopIndent();
    }
    text.PopIndent();


    return true;
}

bool CLIUtil::GetDryOpt( CommandLine& cmdLine, Option& dryOpt)
{
    cmdLine.DryRun= DryRunModes::Application;

    // try to fetch argument
    Substring arg= NULL_STRING;
    bool      argWasPeeked= false;
    if( dryOpt.Args.IsNotEmpty() )
        arg= dryOpt.Args.Front();
    else if( cmdLine.ArgCount() > dryOpt.Position )
    {
        argWasPeeked= true;
        arg= cmdLine.GetArg(dryOpt.Position + 1);
    }

    if( arg.IsNotEmpty() )
    {
        DryRunModes dryRunMode;
        if( enums::Parse( arg, dryRunMode ) && arg.IsEmpty() )
        {
            cmdLine.DryRun= dryRunMode;
            if( argWasPeeked )
            {
                cmdLine.RemoveArg( dryOpt.Position + 1 );
                ++dryOpt.ConsumedArguments;
                dryOpt.Args.PushBack( cmdLine.GetArg(dryOpt.Position + 1) );
            }
        }

        // not peeked means "--dryrun=arg" was given. In this case, the argument has to be recognized.
        else if ( !argWasPeeked )
           return false;
    }

    return true;
}


AString&  CLIUtil::DumpDeclarations( CommandLine& cmdLine, Paragraphs& dump )
{
    dump.Add( "COMMANDS:")
        .PushIndent( 2 );
    for( auto* decl : cmdLine.CommandDecls )
    {
        dump.Add( "- ({}) {}", decl->Element(), decl->Identifier())
            .PushIndent( 2 );
        String256 paramIDs;
        for( auto& param : decl->Parameters )
            paramIDs << param->Name() << ", ";
        if( paramIDs.IsEmpty() )
            paramIDs << "none";
        else
            paramIDs.DeleteEnd( 2 );
        dump.Add(  "Associated parameters: ", paramIDs )
            .Add( decl->HelpTextShort())
            .PopIndent()
            .Add( NEW_LINE );
    }
    dump.PopIndent()


        .Add( NEW_LINE )
        .Add( "OPTIONS:")
        .PushIndent( 2 );
    for( auto* decl : cmdLine.OptionDecls )
    {
        dump.Add( decl->HelpUsageLine() )
            .Add( decl->HelpText()      )
            .Add( NEW_LINE );
    }
    dump.PopIndent();



    dump.Add( NEW_LINE )
        .Add( "PARAMETERS:")
        .PushIndent( 2 );
    for( auto* decl : cmdLine.ParameterDecls )
    {
        dump.Add( "- ({}) {}  Optional: {}  Multi-Separator: {}" ,
                           decl->Element(),
                           decl->Name(),
                           decl->IsOptional(),
                          (decl->ValueListSeparator() ? Box(decl->ValueListSeparator()) : Box("-/-") ))
            .Add(  decl->GetHelpTextShort())
            .Add( NEW_LINE );
    }
    dump.PopIndent()

        .Add( NEW_LINE )
        .Add( "EXIT CODES:")
        .PushIndent( 2 );
    for( auto& declIt : cmdLine.ExitCodeDecls )
        dump.Add( "{:>5} : {}", declIt.first, declIt.second->FormatString() );

    dump.PopIndent();

    return dump.Buffer;
}


//! @cond NO_DOX
namespace
{
    void dumpParsedOptions( CommandLine& app, List<MonoAllocator, Option*>& optionsOriginal, Paragraphs& dump )
    {
        std::vector<Option*> options;
        std::vector<Option*> optionsOfActType;
        auto overallSize= optionsOriginal.Count();
        options       .reserve( size_t(overallSize) );
        optionsOfActType.reserve( size_t(overallSize) );
        for( auto* optionOrig : optionsOriginal )
            options.push_back( optionOrig );

        dump.PushIndent( 2 );
            while( options.size() )
            {
                // collect all options of the same type in  optionsOfActType
                auto* decl= options.front()->Declaration;
                optionsOfActType.clear();
                size_t actIdx= 0;
                while( actIdx < options.size() )
                {
                    if( options[actIdx]->Declaration == decl )
                    {
                        optionsOfActType.push_back( options[actIdx] );
                        options.erase( options.begin() + static_cast<integer>(actIdx) );
                    }
                    else
                        ++actIdx;
                }


                dump.Add( "- \"-{},--{}\" ({}x)",
                          decl->IdentifierChar(), decl->Identifier(), optionsOfActType.size() )
                    .PushIndent( 2 );

                for( actIdx= 0; actIdx < optionsOfActType.size() ; ++actIdx )
                {
                    Option* actOption= optionsOfActType[actIdx];
                    dump.Add( "{}/{}: ArgStrings{!Q[]}= {!Q}, #arguments parsed: {}",
                              actIdx + 1, optionsOfActType.size(),
                              actOption->Position,
                              app.GetArg(actOption->Position),
                              actOption->Args.Count()                          )
                        .PushIndent(5);

                    uinteger argNo= 0;
                    for( auto& arg : actOption->Args )
                        dump.Add( "Argument {}: {!Q}", ++argNo, arg );
                    dump.PopIndent();

                }
                dump.PopIndent()
                    .Add( NEW_LINE );
            }
        dump.PopIndent();
    }
} // anon namespace
//! @endcond

AString&  CLIUtil::DumpParseResults( CommandLine& cmdLine, Paragraphs& dump )
{
    dump.Add( NEW_LINE )
        .Add( "OPTIONS:");
    dumpParsedOptions( cmdLine, cmdLine.Options, dump );

    dump.Add( NEW_LINE )
        .Add( "OPTION ARGUMENTS IGNORED (Usable with other libs):")
        .PushIndent( 2 );
        int cnt= 0;
        for( auto& it : cmdLine.OptionArgsIgnored )
            dump.Add( "{}: {!Q}", cnt++ + 1, it );
        if (cnt == 0 )
            dump.Add( "None" );
    dump.PopIndent();

    dump.Add( NEW_LINE )
        .Add( "COMMANDS PARSED:")
        .PushIndent( 2 );
            cnt= 0;
            for( auto cmd : cmdLine.CommandsParsed )
            {
                ++cnt;
                dump.Add( "- {:8}with argument #{}", cmd->Declaration->Identifier(), cmd->Position )
                    .PushIndent( 2 );
                for( auto* param : cmd->ParametersMandatory )
                {
                    dump.Add( "Parameter: {}", param->Declaration->Name() )
                        .PushIndent( 2 );
                    uinteger argNo= 0;
                    for( auto& arg : param->Args )
                        dump.Add( "Parameter argument {}: {!Q}", ++argNo, arg );
                    dump.PopIndent();

                }
                for( auto* param : cmd->ParametersOptional )
                {
                    dump.Add( "Parameter: {}", param->Declaration->Name() )
                        .PushIndent( 2 );
                    uinteger argNo= 0;
                    for( auto& arg : param->Args )
                        dump.Add( "Parameter argument {}: {!Q}", ++argNo, arg );
                    dump.PopIndent();

                }
                dump.PopIndent()
                    .Add( NEW_LINE );
            }
            if (cnt == 0 )
                dump.Add( "None" );
        dump.PopIndent();



    dump.Add( NEW_LINE )
        .Add( "UNRECOGNIZED CLI ARGUMENTS:")
        .PushIndent( 2 );
    for( auto& it : cmdLine.ArgsLeft )
        dump.Add( "{}: {!Q}", it, cmdLine.GetArg(it));

    if (cmdLine.ArgsLeft.size() == 0 )
        dump.Add( "None" );
    dump.PopIndent();

    return dump.Buffer;
}

} // namespace alib::cli
