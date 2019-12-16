// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_CLI_CLIUTIL)
#   include "alib/cli/cliutil.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#   include "alib/strings/util/tokenizer.hpp"
#endif
#if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#   include "alib/enums/serialization.hpp"
#endif

#endif // !defined(ALIB_DOX)

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif


namespace aworx { namespace lib { namespace cli {


OptionDecl*  CLIUtil::GetOptionDecl( CLIApp& cliApp, const String& identString )
{
    for( auto* decl : cliApp.OptionDecls )
        if (    (    identString.Length()              == 1
                  && identString.CharAtStart<false>()  == decl->IdentifierChar() )
             || (    identString.Length()              >= decl->MinimumRecognitionLength()
                 &&  decl->Identifier().StartsWith<true,Case::Ignore>( identString ) )  )
            return decl;
    return nullptr;
}


CommandDecl*  CLIUtil::GetCommandDecl( CLIApp& cliApp, const String& identString )
{
    for( auto* decl : cliApp.CommandDecls )
        if (     identString.Length() >=  decl->MinimumRecognitionLength()
             &&  decl->Identifier().StartsWith<true,Case::Ignore>( identString ) )
            return decl;
    return nullptr;
}

ParameterDecl*  CLIUtil::GetParameterDecl(CLIApp &cliApp, const String &identString)
{
    for( auto* decl : cliApp.ParameterDecls )
        if ( decl->Name().StartsWith<true,Case::Ignore>( identString ) )
            return decl;
    return nullptr;
}



AString CLIUtil::GetCommandUsageFormat( CLIApp& cliApp, CommandDecl& cmd )
{
    AString result(2048);
    result << cliApp.ResModule->GetResource( "HlpCLIAppName" ) << ' ' << cmd.Identifier();

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

bool CLIUtil::GetHelp( CLIApp& cliApp, Command* helpCmd, Option* helpOpt, Paragraphs& text )
{
    Module& mod= *cliApp.ResModule;
    text.AddMarked( cliApp.AppInfo );

    String argList= NullString();
    bool   argWasPeeked= false;
    if( helpCmd )
    {
        if(  helpCmd->ParametersOptional.IsNotEmpty() )
            argList= helpCmd->ParametersOptional.Front()->Args.Front();
        else if( cliApp.ArgCount() > helpCmd->Position + 1  )
        {
            argWasPeeked= true;
            argList= cliApp.Arg(helpCmd->Position + 1);
        }
    }
    else
    {
        if(  helpOpt->Args.IsNotEmpty() )
            argList= helpOpt->Args.Front();
        else if( cliApp.ArgCount() > helpOpt->Position + 1 )
        {
            argWasPeeked= true;
            argList= cliApp.Arg(helpOpt->Position + 1);
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
                CommandDecl* cmdDecl= CLIUtil::GetCommandDecl( cliApp, arg );
                if( cmdDecl )
                {
                    ++cntArgsRecognized;

                    if( cliApp.DryRun == DryRunModes::Off )
                    {
                        text.Add( mod.GetResource( "HlpHdlTopic" ), "command", cmdDecl->Identifier() )
                            .PushIndent( 2 )
                            .Add( mod.GetResource( "HlpHdlUsage" ), " ", GetCommandUsageFormat( cliApp, *cmdDecl ) )

                            .Add( NewLine(), mod.GetResource( "HlpHdlDscr" ) )
                            .PushIndent( 2 )
                              .AddMarked( cmdDecl->HelpTextLong(), NewLine() )
                            .PopIndent()

                            .Add( NewLine(), mod.GetResource( "HlpHdlPDscr" ) )
                            .PushIndent( 2 );
                            for( auto* param : cmdDecl->Parameters )
                            {
                                text.Add( "* ", param->Name()  )
                                    .PushIndent( 2 )
                                      .AddMarked( param->GetHelpTextShort() )
                                    .PopIndent()
                                   .Add( NewLine() );
                            }
                        text.PopIndent()
                            .PopIndent();
                    }
                }
            }

            // option?
            if( cntArgsRecognizedSoFar == cntArgsRecognized )
            {
                OptionDecl* optDecl= CLIUtil::GetOptionDecl( cliApp, arg );
                if( optDecl )
                {
                    ++cntArgsRecognized;
                    if( cliApp.DryRun == DryRunModes::Off )
                    {
                        text.Add( mod.GetResource( "HlpHdlTopic" ), "option", optDecl->Identifier() )
                            .PushIndent( 2 )
                            .Add( mod.GetResource( "HlpHdlUsage" ), "  ", optDecl->HelpUsageLine() )
                            .Add( NewLine(), mod.GetResource( "HlpHdlDscr" ) )
                            .PushIndent( 2 )
                                .AddMarked( optDecl->HelpText(), NewLine() )
                            .PopIndent()

                            .PopIndent();
                    }
                }
            }

            // parameter?
            if( cntArgsRecognizedSoFar == cntArgsRecognized )
            {
                ParameterDecl* paramDecl= CLIUtil::GetParameterDecl( cliApp, arg );
                if( paramDecl )
                {
                    ++cntArgsRecognized;
                    if( cliApp.DryRun == DryRunModes::Off )
                    {
                        text.Add( mod.GetResource( "HlpHdlTopic" ), "parameter", paramDecl->Name() )
                            .PushIndent( 2 )
                            .AddMarked( paramDecl->GetHelpTextLong() )
                            .PopIndent();
                    }
                }
            }

            // special help topic?
            if( cntArgsRecognizedSoFar == cntArgsRecognized )
            {
                auto& additionalHelpTopics= mod.TryResource("HlpAddnlTopics");
                if( additionalHelpTopics.IsNotEmpty() )
                {
                    Tokenizer topics(additionalHelpTopics, ',');
                    while(topics.Next().IsNotEmpty())
                    {
                        if(topics.Actual.StartsWith<true,Case::Ignore>( arg ) )
                        {
                            ++cntArgsRecognized;
                            if( cliApp.DryRun == DryRunModes::Off )
                                text.AddMarked( mod.GetResource( NString64("HlpAddnl")._( topics.Actual ) ) );

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
                cliApp.RemoveArg( helpCmd->Position + 1 );
                ++helpCmd->QtyArgsConsumed;
            }
            else
            {
                cliApp.RemoveArg( helpOpt->Position + 1 );
                ++helpOpt->QtyArgsConsumed;
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
    text.AddMarked( cliApp.ResModule->GetResource( "HlpGeneral" ) );

    text.Add( mod.GetResource(                "HlpHdlExtCds") )
        .PushIndent( 2 );
    for( auto declIt : cliApp.ExitCodeDecls )
        text.Add( "  {:2}: {:>2}\\n      {}" , declIt.first.Integral(),
                                               declIt.first,
                                               declIt.second->FormatString() );
    text.PopIndent();

    text.Add( cliApp.ResModule->GetResource( "HlpHdlUsage" ) )
        .PushIndent( 2 )
        .Add( cliApp.ResModule->GetResource( "HlpUsage"    ) )
        .PopIndent()
        .Add( NewLine(), mod.GetResource(       "HlpHdlOpts"  ) )
        .PushIndent( 2 );
    for( auto* decl : cliApp.OptionDecls )
        text.Add( decl->HelpUsageLine() );

    text.PopIndent();

    text.Add( NewLine(), mod.GetResource(       "HlpHdlCmds"  ) )
        .PushIndent( 2 );
    for( auto* decl : cliApp.CommandDecls )
    {
        text.Add( "* ", GetCommandUsageFormat( cliApp, *decl ), NewLine() )
            .PushIndent( 2 )
              .Add( decl->HelpTextShort(), NewLine() )
            .PopIndent();
    }
    text.PopIndent();


    return true;
}

bool CLIUtil::GetDryOpt( CLIApp& cliApp, Option& dryOpt)
{
    cliApp.DryRun= DryRunModes::Application;

    // try to fetch argument
    Substring arg= NullString();
    bool      argWasPeeked= false;
    if( dryOpt.Args.IsNotEmpty() )
        arg= dryOpt.Args.Front();
    else if( cliApp.ArgCount() > dryOpt.Position )
    {
        argWasPeeked= true;
        arg= cliApp.Arg(dryOpt.Position + 1);
    }

    if( arg.IsNotEmpty() )
    {
        DryRunModes dryRunMode;
        if( enums::Parse( arg, dryRunMode ) && arg.IsEmpty() )
        {
            cliApp.DryRun= dryRunMode;
            if( argWasPeeked )
            {
                cliApp.RemoveArg( dryOpt.Position + 1 );
                ++dryOpt.QtyArgsConsumed;
                dryOpt.Args.PushBack( cliApp.Arg(dryOpt.Position + 1) );
            }
        }

        // not peeked means "--dryrun=arg" was given. In this case, the argument has to be recognized.
        else if ( !argWasPeeked )
           return false;
    }

    return true;
}


AString&  CLIUtil::DumpDeclarations( CLIApp& cliApp, Paragraphs& dump )
{
    dump.Add( "COMMANDS:")
        .PushIndent( 2 );
    for( auto* decl : cliApp.CommandDecls )
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
            .Add( NewLine() );
    }
    dump.PopIndent()


        .Add( NewLine() )
        .Add( "OPTIONS:")
        .PushIndent( 2 );
    for( auto* decl : cliApp.OptionDecls )
    {
        dump.Add( decl->HelpUsageLine() )
            .Add( decl->HelpText()      )
            .Add( NewLine() );
    }
    dump.PopIndent();



    dump.Add( NewLine() )
        .Add( "PARAMETERS:")
        .PushIndent( 2 );
    for( auto* decl : cliApp.ParameterDecls )
    {
        dump.Add( "- ({}) {}  Optional: {}  Multi-Separator: {}" ,
                           decl->Element(),
                           decl->Name(),
                           decl->IsOptional(),
                          (decl->ValueListSeparator() ? Box(decl->ValueListSeparator()) : Box("-/-") ))
            .Add(  decl->GetHelpTextShort())
            .Add( NewLine() );
    }
    dump.PopIndent()

        .Add( NewLine() )
        .Add( "EXIT CODES:")
        .PushIndent( 2 );
    for( auto& declIt : cliApp.ExitCodeDecls )
        dump.Add( "{:>5} : {}", declIt.first, declIt.second->FormatString() );

    dump.PopIndent();

    return dump.Buffer;
}


//! @cond NO_DOX
namespace
{
    void dumpParsedOptions( CLIApp& app, List<Option*>& optionsOriginal, Paragraphs& dump )
    {
        std::vector<Option*> options;
        std::vector<Option*> optionsOfActType;
        auto overallSize= optionsOriginal.Size();
        options       .reserve( static_cast<size_t>(overallSize) );
        optionsOfActType.reserve( static_cast<size_t>(overallSize) );
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
                              app.Arg(actOption->Position),
                              actOption->Args.Size()                          )
                        .PushIndent(5);

                    uinteger argNo= 0;
                    for( auto& arg : actOption->Args )
                        dump.Add( "Argument {}: {!Q}", ++argNo, arg );
                    dump.PopIndent();

                }
                dump.PopIndent()
                    .Add( NewLine() );
            }
        dump.PopIndent();
    }
} // anon namespace
//! @endcond

AString&  CLIUtil::DumpParseResults( CLIApp& cliApp, Paragraphs& dump )
{
    dump.Add( NewLine() )
        .Add( "OPTIONS:");
    dumpParsedOptions( cliApp, cliApp.Options, dump );

    dump.Add( NewLine() )
        .Add( "OPTION ARGUMENTS IGNORED (Usable with other libs):")
        .PushIndent( 2 );
        int cnt= 0;
        for( auto& it : cliApp.OptionArgsIgnored )
            dump.Add( "{}: {!Q}", cnt++ + 1, it );
        if (cnt == 0 )
            dump.Add( "None" );
    dump.PopIndent();

    dump.Add( NewLine() )
        .Add( "COMMANDS PARSED:")
        .PushIndent( 2 );
            cnt= 0;
            for( auto& cmd : cliApp.CommandsParsed )
            {
                ++cnt;
                dump.Add( "- {:8}with argument #{}", cmd.Declaration->Identifier(), cmd.Position )
                    .PushIndent( 2 );
                for( auto* param : cmd.ParametersMandatory )
                {
                    dump.Add( "Parameter: {}", param->Declaration->Name() )
                        .PushIndent( 2 );
                    uinteger argNo= 0;
                    for( auto& arg : param->Args )
                        dump.Add( "Parameter argument {}: {!Q}", ++argNo, arg );
                    dump.PopIndent();

                }
                for( auto* param : cmd.ParametersOptional )
                {
                    dump.Add( "Parameter: {}", param->Declaration->Name() )
                        .PushIndent( 2 );
                    uinteger argNo= 0;
                    for( auto& arg : param->Args )
                        dump.Add( "Parameter argument {}: {!Q}", ++argNo, arg );
                    dump.PopIndent();

                }
                dump.PopIndent()
                    .Add( NewLine() );
            }
            if (cnt == 0 )
                dump.Add( "None" );
        dump.PopIndent();



    dump.Add( NewLine() )
        .Add( "UNRECOGNIZED CLI ARGUMENTS:")
        .PushIndent( 2 );
    for( auto& it : cliApp.ArgsLeft )
        dump.Add( "{}: {!Q}", it, cliApp.Arg(it));

    if (cliApp.ArgsLeft.size() == 0 )
        dump.Add( "None" );
    dump.PopIndent();

    return dump.Buffer;
}

}}}// namespace aworx::lib::system

