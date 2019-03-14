// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_CLI_CLIUTIL)
#   include "alib/cli/cliutil.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#   include "alib/strings/util/tokenizer.hpp"
#endif


namespace aworx { namespace lib { namespace cli {


OptionDecl*  CLIUtil::GetOptionDecl( CLIApp& cliApp, const String& identString )
{
    for( auto& decl : cliApp.OptionDecls )
        if (    (    identString.Length() == 1
                  && identString.CharAtStart<false>()  == decl.IdentifierChar() )
             || (    identString.Length() >=  decl.MinimumParseLen()
                 &&  decl.Identifier().StartsWith<true,Case::Ignore>( identString ) )            )
            return &decl;
    return nullptr;
}


CommandDecl*  CLIUtil::GetCommandDecl( CLIApp& cliApp, const String& identString )
{
    for( auto& decl : cliApp.CommandDecls )
        if (     identString.Length() >=  decl.MinimumParseLen()
             &&  decl.Identifier().StartsWith<true,Case::Ignore>( identString ) )
            return &decl;
    return nullptr;
}

ParameterDecl*  CLIUtil::GetParameterDecl(CLIApp &cliApp, const String &identString)
{
    for( auto& decl : cliApp.ParameterDecls )
        if ( decl.Name().StartsWith<true,Case::Ignore>( identString ) )
            return &decl;
    return nullptr;
}



AString CLIUtil::GetCommandUsageFormat( CLIApp& cliApp, CommandDecl& cmd )
{
    AString result(2048);
    result << cliApp.ResModule->GetResource( "HlpCLIAppName" ) << ' ' << cmd.Identifier();

    for( auto param : cmd.Parameters )
    {
        result << ' ';
        if( param->IsOptional() ) result << '[';

        result << param->Name();
        if ( param->Separator() != '\0' )
        {
            result << '[' << param->Separator() << param->Name() << "...]";
        }

        if( param->IsOptional() ) result << ']';

    }
    return result;
}

bool CLIUtil::GetHelp( CLIApp& cliApp, Command* helpCmd, Option* helpOpt, Text& text )
{
    Module& mod= *cliApp.ResModule;
    text.AddMarked( mod.GetResource( "AppInfo" ) );

    String argList= NullString();
    bool   argWasPeeked= false;
    if( helpCmd )
    {
        if(  helpCmd->ParametersOptional.size() )
            argList= helpCmd->ParametersOptional[0].Args[0];
        else if( cliApp.ArgStrings.size() > helpCmd->Position )
        {
            argWasPeeked= true;
            argList= cliApp.ArgStrings[helpCmd->Position + 1];
        }
    }
    else
    {
        if(  helpOpt->Args.size() )
            argList= helpOpt->Args[0];
        else if( cliApp.ArgStrings.size() > helpOpt->Position )
        {
            argWasPeeked= true;
            argList= cliApp.ArgStrings[helpOpt->Position + 1];
        }
    }

    if( argList.IsNotEmpty() )
    {
        int cntArgsRecognized= 0;

        Tokenizer args( argList, ',' );
        int cntTokens= 0;
        while( args.HasNext() )
        {
            cntTokens++;
            String arg= args.Next();
            int cntArgsRecognizedSoFar= cntArgsRecognized;

            // command
            {
                CommandDecl* cmdDecl= CLIUtil::GetCommandDecl( cliApp, arg );
                if( cmdDecl )
                {
                    cntArgsRecognized++;

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
                            for( auto param : cmdDecl->Parameters )
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
                    cntArgsRecognized++;
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
                    cntArgsRecognized++;
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
                String additionalHelpTopics= mod.TryResource("HlpAddnlTopics");
                if( additionalHelpTopics.IsNotEmpty() )
                {
                    Tokenizer topics(additionalHelpTopics, ',');
                    while(topics.Next().IsNotEmpty())
                    {
                        if(topics.Actual.StartsWith<true,Case::Ignore>( arg ) )
                        {
                            cntArgsRecognized++;
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
                helpCmd->QtyArgsConsumed++;
            }
            else
            {
                cliApp.RemoveArg( helpOpt->Position + 1 );
                helpOpt->QtyArgsConsumed++;
                helpOpt->Args.emplace_back( argList );
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
    for( auto& it : cliApp.ExitCodeDecls )
    {
        text.Add( "  {:2}: {:>2}\\n      {}" , it.first.Value(),  it.first,
                                               it.second.FormatString() );
    }
    text.PopIndent();

    text.Add( cliApp.ResModule->GetResource( "HlpHdlUsage" ) )
        .PushIndent( 2 )
        .Add( cliApp.ResModule->GetResource( "HlpUsage"    ) )
        .PopIndent()
        .Add( NewLine(), mod.GetResource(       "HlpHdlOpts"  ) )
        .PushIndent( 2 );
    for( auto& it : cliApp.OptionDecls )
        text.Add( it.HelpUsageLine() );

    text.PopIndent();

    text.Add( NewLine(), mod.GetResource(       "HlpHdlCmds"  ) )
        .PushIndent( 2 );
    for( auto& it : cliApp.CommandDecls )
    {
        text.Add( "* ", GetCommandUsageFormat( cliApp, it ), NewLine() )
            .PushIndent( 2 )
              .Add( it.HelpTextShort(), NewLine() )
            .PopIndent();
    }
    text.PopIndent();


    return true;
}

bool CLIUtil::GetDryOpt( CLIApp& cliApp, Option& dryOpt)
{
    Substring arg= NullString();
    bool      argWasPeeked= false;
    if( dryOpt.Args.size() )
        arg= dryOpt.Args[0];
    else if( cliApp.ArgStrings.size() > dryOpt.Position )
    {
        argWasPeeked= true;
        arg= cliApp.ArgStrings[dryOpt.Position + 1];
    }

    cliApp.DryRun= DryRunModes::Application;

    if( arg.IsNotEmpty() )
    {
        DryRunModes dryRunMode;
        if( arg.ConsumeEnum( dryRunMode ) && arg.IsEmpty() )
        {
            cliApp.DryRun= dryRunMode;
            if( argWasPeeked )
            {
                cliApp.RemoveArg( dryOpt.Position + 1 );
                dryOpt.QtyArgsConsumed++;
                dryOpt.Args.emplace_back( cliApp.ArgStrings[dryOpt.Position + 1] );
            }
        }

        // not peeked means "--dryrun=arg" was given. In this case, the argument has to be recognized.
        else if ( !argWasPeeked )
           return false;
    }

    return true;
}


AString&  CLIUtil::DumpDeclarations( CLIApp& cliApp, Text& dump )
{
    dump.Add( "COMMANDS:")
        .PushIndent( 2 );
    for( auto& it : cliApp.CommandDecls )
    {
        dump.Add( "- ({}) {}", it.ID, it.Identifier())
            .PushIndent( 2 );
        String256 paramIDs;
        for( auto& param : it.Parameters )
            paramIDs << param->Name() << ", ";
        if( paramIDs.IsEmpty() )
            paramIDs << "none";
        else
            paramIDs.DeleteEnd( 2 );
        dump.Add(  "Associated parameters: ", paramIDs )
            .Add(  it.HelpTextShort())
            .PopIndent()
            .Add( NewLine() );
    }
    dump.PopIndent()


        .Add( NewLine() )
        .Add( "OPTIONS:")
        .PushIndent( 2 );
    for( auto& it : cliApp.OptionDecls )
    {
        dump.Add( it.HelpUsageLine() )
            .Add( it.HelpText()      )
            .Add( NewLine() );
    }
    dump.PopIndent();



    dump.Add( NewLine() )
        .Add( "PARAMETERS:")
        .PushIndent( 2 );
    for( auto& it : cliApp.ParameterDecls )
    {
        dump.Add( "- ({}) {}  Optional: {}  Multi-Separator: {}" ,
                           it.ID,
                           it.Name(),
                           it.IsOptional(),
                           (it.Separator() ? Box(it.Separator()) : Box("-/-") ))
            .Add(  it.GetHelpTextShort())
            .Add( NewLine() );
    }
    dump.PopIndent()


        .Add( NewLine() )
        .Add( "EXIT CODES:")
        .PushIndent( 2 );
    for( auto& it : cliApp.ExitCodeDecls )
        dump.Add( "{:>5} : {}", it.first,  it.second.FormatString() );

    dump.PopIndent();

    return dump.Buffer;
}


//! @cond NO_DOX
namespace
{
    void dumpParsedOptions( CLIApp& app, std::map<Enum, std::vector<Option>>& optionMap, Text& dump  )
    {
        dump.PushIndent( 2 );
            int cnt= 0;
            for( auto& it : optionMap )
            {
                int qtyRepeats= static_cast<int>( it.second.size() );
                if( qtyRepeats == 0 )
                    continue;
                cnt++;

                OptionDecl* decl= it.second.back().Declaration;
                dump.Add( "- \"-{},--{}\" ({}x)",
                          decl->IdentifierChar(), decl->Identifier(), qtyRepeats )
                    .PushIndent( 2 );

                int cntInstances= 1;
                for( auto& instance : it.second )
                {
                    dump.Add( "{}/{}: ArgStrings[{}]= {!Q}, #arguments parsed: {}",
                                       cntInstances++, qtyRepeats,
                                       instance.Position, app.ArgStrings[instance.Position], instance.Args.size() )
                        .PushIndent(5);
                    for( size_t argNo= 0; argNo < instance.Args.size() ; ++argNo )
                        dump.Add( "Argument {}: {!Q}", argNo+1, instance.Args[argNo] );
                    dump.PopIndent();

                }
                dump.PopIndent()
                    .Add( NewLine() );
            }
            if (cnt == 0 )
                dump.Add( "None" );
        dump.PopIndent();
    }
} // anon namespace
//! @endcond

AString&  CLIUtil::DumpParseResults( CLIApp& cliApp, Text& dump )
{
    dump.Add( NewLine() )
        .Add( "OPTIONS:");
    dumpParsedOptions( cliApp, cliApp.OptionsFound, dump );

    dump.Add( NewLine() )
        .Add( "OPTIONS OVERWRITTEN:");
    dumpParsedOptions( cliApp, cliApp.OptionsOverwritten, dump );

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
            for( auto&& cmd : cliApp.CommandsParsed )
            {
                cnt++;
                dump.Add( "- {:8}with argument #{}", cmd.Declaration->Identifier(), cmd.Position )
                    .PushIndent( 2 );
                for( auto& param : cmd.ParametersMandatory )
                {
                    dump.Add( "Parameter: {}", param.Declaration->Name() )
                        .PushIndent( 2 );
                    for( size_t argNo= 0; argNo < param.Args.size() ; ++argNo )
                        dump.Add( "Parameter argument {}: {!Q}", argNo + 1, param.Args[argNo] );
                    dump.PopIndent();

                }
                for( auto& param : cmd.ParametersOptional )
                {
                    dump.Add( "Parameter: {}", param.Declaration->Name() )
                        .PushIndent( 2 );
                    for( size_t argNo= 0; argNo < param.Args.size() ; ++argNo )
                        dump.Add( "Parameter argument {}: {!Q}", argNo + 1, param.Args[argNo] );
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
        dump.Add( "{}: {!Q}", it, cliApp.ArgStrings[it]);

    if (cliApp.ArgsLeft.size() == 0 )
        dump.Add( "None" );
    dump.PopIndent();

    return dump.Buffer;
}

}}}// namespace aworx::lib::system

