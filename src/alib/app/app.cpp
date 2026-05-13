
namespace alib::app {

App* APP_SINGLETON= nullptr;

App::App() {
    stopWatch.Reset();

    ALIB_ASSERT_ERROR(!APP_SINGLETON, "ALIB/APP",
        "A second instance of the App-singleton was created, which is forbidden.")
    APP_SINGLETON= this;

    machine.Program.reserve( 20 + 10 ); // allow 10 custom states without resizing
    
    machine.Program.insert( machine.Program.end(),  {
         { States::SetCamps              , &App::onBsSetCamps              },
         { States::PrepareResources      , &App::onBsPrepareResources      },
         { States::SetNameVersionAndInfo , &App::onBsSetNameVersionAndInfo },

         { States::PrepareConfig         , &App::onBsPrepareConfig         },
         { States::PreloadVariables      , &App::onBsPreloadVariables      },
         { States::CLIDefine             , &App::onBsCLIDefine             },
         { States::CLIReadOptions        , &App::onBsCLIReadOptions        },
         { States::ImportConfig          , &App::onBsImportConfig          },
         { States::ConfigureCLI          , &App::onBsConfigureCLI          },

         { States::FinalizeBootstrap     , &App::onBsFinalizeBootstrap     },
         { States::ReadDryRunOption      , &App::onBsReadDryRunOption      },
         { States::SetupALox             , &App::onBsSetupALox             },

         { States::RunStart              , &App::onRunStart                },
         { States::Run                   , &App::onRun                     },
         { States::RunEnd                , &App::onRunEnd                  },
        
         { States::AnnounceShutdown      , &App::onSdAnnounceShutdown      },
         { States::CleanALox             , &App::onSdCleanALox             },
         { States::ExportConfig          , &App::onSdExportConfig          },
         { States::Output                , &App::onSdOutput                },
         { States::FinalizeShutdown      , &App::onSdFinalizeShutdown      },
    });
}

App::~App()                                                                                       {}

int App::Main( int argc, const char** argv, const wchar_t** argvw ) {
    ARG_C=   argc;
    ARG_VN=  argv;
    ARG_VW=  argvw;
    
    // sort commands by state
    std::sort(machine.Program.begin(), machine.Program.end(),
              [](const StateMachine::Command &a, const StateMachine::Command &b) {
                  return a.State.Integral<int>() < b.State.Integral<int>();
              });
    #if ALIB_DEBUG
    {
        int lastID= 0;
        for (StateMachine::Command& cmd : machine.Program ) {
            if (cmd.State.Integral<int>() == lastID)
                ALIB_ERROR("APP", "Duplicate command ID {} found in program",
                                  cmd.State.Integral<int>() )
            lastID= cmd.State.Integral<int>();
    }   }
    #endif


    for (StateMachine::Command& cmd : machine.Program )
        try {
            // transition the state machine "State"
            machine.State= cmd.State;

            // built-in step?
            if ( cmd.State.IsEnumType<App::States>() ) {
                // transition the state machine "BuiltInState"
                machine.BuiltInState= cmd.State.Get<States>();
                (this->*cmd.Method.BuiltIn)();
            } else {
                (*cmd.Method.Custom)(*this);
            }

            // with emergencies, we stop processing
            if ( machine.EmergencyStop )
                break;

        // exceptions are converted to exit codes and displayed. A custom conversion implementation
        // may take the current app-state into account to determine the right exit code.
        } catch( Exception& e ) {
            if (e.Type() != Exceptions::ControlledEarlyExit)
                machine.SetExitCode( exceptionToExitCode(e) );
            #if ALIB_DEBUG
                else {
                    ALIB_ASSERT_ERROR(  int(machine.BuiltInState) >= int(States::RunStart)
                                     && int(machine.BuiltInState) <= int(States::RunEnd  ), "APP",
                      "Exceptions::ControlledEarlyExit thrown while not in States::RunStart/Run/RunEnd")
                    ALIB_ASSERT_ERROR(machine.GetExitCode().Integral() != 0, "APP",
                      "Exceptions::ControlledEarlyExit thrown without having exit code set")
                }
            #endif
            exceptionDisplay(e, cErr->Buffer);
        }

        // System exceptions should be handled by the descendant. (prior to ending here)
        catch( std::runtime_error& e ) {
            cErr->Add("Unhandled system exception: ", e.what());
            machine.SetExitCode( App::ExitCodes::ErrUnknown );
            #if ALIB_DEBUG
                onSdOutput();
                ALIB_WARNING("APP", "System exception not handled. "
                                    "This should be done by the user of class ALib App.")
            #endif
        }

        if (machine.EmergencyStop) {
            // finish bootstrapping of ALib, to be able to duly shutdown.
            if ( machine.BuiltInState < States::FinalizeBootstrap )
                Bootstrap();

            // shutdown ALib
            if ( machine.BuiltInState < States::FinalizeShutdown)
                Shutdown();
        }

    return machine.GetExitCode().Integral<int>();
}


//##################################################################################################
// Helpers
//##################################################################################################

alib::Enum  App::exceptionToExitCode( alib::Exception& exception ) {
    // Exceptions from ALib module Variables -> either read or write of INI-files failed.
    if ( exception.Type().IsEnumType<variables::Exceptions>() ) {
        auto ee= exception.Type().Get<variables::Exceptions>();

        if (machine.BuiltInState==States::ImportConfig  ) {
            if (ee==variables::Exceptions::ErrorOpeningFile ) return ExitCodes::ErrConfigFileNotFound;
            if (ee==variables::Exceptions::ErrorWritingFile ) return ExitCodes::ErrConfigFileNotWritable;
    }   }

    // handle cli-exceptions
    if ( exception.Type().IsEnumType<CLIExceptions>() ) {
        ExitCodes exitCode;
             if( exception.Type().Get<CLIExceptions>() == CLIExceptions::NoCommandGiven) exitCode= ExitCodes::ErrNoCmdGiven;
        else if( exception.Type().Get<CLIExceptions>() == CLIExceptions::UnknownCommand) exitCode= ExitCodes::ErrUnknownCommand;
        else if( exception.Type().Get<CLIExceptions>() == CLIExceptions::ParsingOptions) exitCode= ExitCodes::ErrParsingOption;
        else { ALIB_ASSERT_ERROR(     exception.Type().Get<CLIExceptions>()
                                   == CLIExceptions::ParsingCommand,
                                   "APP", "Unknown exception")                           exitCode= ExitCodes::ErrParsingCommand; }

        return exitCode;
    }

    // We do not know. Overridden methods should know
    return Enum(ExitCodes::ErrUnknown);
}

void  App::exceptionDisplay( alib::Exception& exception, AString& target ) {
    if (exception.Type()== Exceptions::ControlledEarlyExit) {
        #if ALIB_DEBUG
            alib::assert::Raise( exception.Back().CI, 1, "APP",
                                "<- Exceptions::ControlledEarlyExit was trown here!" );
            machine.DbgDumpFurtherExitCodes(target);
        #endif
        return;
    }

    // print unhandled exceptions
    String512 cmdLine(ProcessInfo::Current().ExecFileName);
    for( int i= 0; i < alib::ARG_C ; ++i )
        cmdLine << ' ' << alib::ARG_VN[i];
    ALIB_LOCK_RECURSIVE_WITH(alib::format::Formatter::DEFAULT_LOCK)
    cErr->Add( "Exception occurred when executing command:\n"
               "    {}\n"
               "Exception details ({} item{}):",
               cmdLine, exception.Size(), (exception.Size() > 1 ? "s" : "") )
         .PushIndent(A_CHAR("  "))
         .Add( exception.Format() )
         .PopIndent();

    #if ALIB_DEBUG
      cErr->Add( "Application exit-code will be: ", machine.GetExitCode() );
    #endif

    machine.DbgDumpFurtherExitCodes(target);
}

void App::printConfigFileInfo(Paragraphs& p) {
    LocalAllocator2K cfgFilePathAllocator;
    StdVectorMA<ConfigFileDescriptor> files(cfgFilePathAllocator);
    getConfigFilePaths(files);

    if ( !files.size() ) {
        p.Add("None");
        return;
    }

    for (ConfigFileDescriptor& fileInfo : files) {
        p.Add( "Configuration file:   {!ATab}:1\n", fileInfo.Pathname );
        p.Add( "  Existed: {!ATab}\n", fileInfo.WasEmpty ? "no" : "yes"  );

        if ( fileInfo.Exports.IsNotEmpty() ) {
            Tokenizer tok(fileInfo.Exports, ',');
            while ( tok.HasNext() )
                p.Add("  Exports: {!ATab}\n" , tok.Next() );
}   }   }


void App::getConfigFilePathsFromResources(StdVectorMA<ConfigFileDescriptor>& files) {
    size_t cnt= 0;

    // read file-names from resource, only if not set, yet.
    if ( !files.size() )
        for (;;) {
            NString128 resourceName("CFGF_NAME_"); resourceName << (cnt+1);
            String name= APP.TryResource( resourceName );
            if (name.IsEmpty())
                break;
            files.emplace_back();
            files.at(cnt).Pathname= name;
            ++cnt;
        }

    // read comments from resources
    cnt= 0;
    for (ConfigFileDescriptor& fInfo : files ) {
        if ( fInfo.Comment.IsEmpty() ) {
            NString128 resourceName("CFGF_CMT_"); resourceName << (cnt+1);
            fInfo.Comment= APP.TryResource( resourceName );
        }
        ++cnt;
    }

    // read exports
    cnt= 0;
    for (ConfigFileDescriptor& fInfo : files ) {
        NString128 resourceName("CFGF_EXP_"); resourceName << (cnt+1);
        fInfo.Exports= APP.TryResource( resourceName );
        ++cnt;
    }

    // read additional exports: if more export values in resources exist than
    // the number of created config-files, the last ones are added to the last file.
    String1K additionalExports;
    for (;;) {
        NString128 resourceName("CFGF_EXP_"); resourceName << (cnt+1);
        String exports= APP.TryResource( resourceName );
        if (exports.IsNull() )
            break;
        additionalExports << ',' << exports;
        ++cnt;
    }

    if ( additionalExports.IsNotEmpty() ) {
        ALIB_ASSERT_ERROR(!files.empty(), "APP",
            "Config-exports {!Q} found, but no config-file is created by the application",
            additionalExports)
        ConfigFileDescriptor& last= files.back();
        if ( last.Exports.IsNotEmpty() )  additionalExports.InsertAt(last.Exports, 0);
        else                              additionalExports.DeleteStart(1);

        last.Exports.Allocate(files.get_allocator().GetAllocator(), additionalExports );

    }

    return;
}

void App::getConfigFilePathsMakeAbsolutePaths(StdVectorMA<ConfigFileDescriptor>& files) {
    for (ConfigFileDescriptor& fileInfo : files)  {
        if (fileInfo.Pathname.IsEmpty() || fileInfo.Pathname.IsAbsolute())
            continue;

        Path backup(fileInfo.Pathname);
        // some relative addressing?
        if (   fileInfo.Pathname.StartsWith(A_PATH("./"  ))
            || fileInfo.Pathname.StartsWith(A_PATH("../" ))
            || fileInfo.Pathname.StartsWith(A_PATH(".\\" ))
            || fileInfo.Pathname.StartsWith(A_PATH("..\\")) )
            fileInfo.Pathname.Change( SystemFolders::Current, backup );
        else {
                fileInfo.Pathname.Change( SystemFolders::HomeConfig );
                fileInfo.Pathname._<NC>(DIRECTORY_SEPARATOR)._(backup);
}   }   }

void App::getConfigFilePathsFromCLIParam(StdVectorMA<ConfigFileDescriptor>& files) {

    auto* optionConfig= cli.GetOption(Options::Config);
    if( optionConfig == nullptr )
        return;

    Substring names=  optionConfig->Args.front();
    size_t cnt= 0;
    while (names.IsNotEmpty()) {
        Substring name= names.ConsumeToken(',');
        name.Trim();
        if (files.size() <= cnt)
            files.emplace_back();
        Path& path= files.at(cnt).Pathname;
        if(    !name.IsEmpty()
            && !String(A_CHAR("Default")).StartsWith<CHK,lang::Case::Ignore>( name ) )
            path.Reset(name);
        ++cnt;
    }
    return;
}

void App::getConfigFilePaths(StdVectorMA<ConfigFileDescriptor>& files) {
    getConfigFilePathsFromCLIParam     (files); if (machine.EmergencyStop) return;
    getConfigFilePathsFromResources    (files); if (machine.EmergencyStop) return;
    getConfigFilePathsMakeAbsolutePaths(files); if (machine.EmergencyStop) return;
    while (files.size() && files.back().Pathname.IsEmpty())
        files.pop_back();
}



void App::onBsSetCamps()                        { if ( CAMPS.empty() ) BootstrapAddDefaultCamps(); }

void App::onBsPrepareResources() {
    #if ALIB_DEBUG_RESOURCES
        alib::resources::LocalResourcePool::DbgResourceLoadObserver= &std::cout;
    #endif

    Bootstrap( BootstrapPhases::PrepareResources );

    cOut= monomem::GLOBAL_ALLOCATOR().New<Paragraphs>();
    cErr= monomem::GLOBAL_ALLOCATOR().New<Paragraphs>();

    if( cOut->LineWidth == 0) {
        cOut->LineWidth=
        cErr->LineWidth= Console::GetWidth(false, 120);
}   }

void App::onBsSetNameVersionAndInfo() {
    if (appName.IsEmpty())
        appName= APP.TryResource( "AppName" );

    if (appVersion.IsEmpty())
        appVersion= APP.TryResource( "AppVersion" );

    if (appInfo.IsEmpty()) {
        String infoFmtString= APP.TryResource("AppInfo");
        if ( infoFmtString.IsNotEmpty() ) {
            StringLengthResetter resetter(cOut->Buffer);
            cOut->AddMarked(infoFmtString, appName, appVersion, CalendarDateTime(DateTime()).Year);
            appInfo= String( monomem::GLOBAL_ALLOCATOR,
                             cOut->Buffer.Substring(resetter.OriginalLength()) );
}   }   }

void App::onBsPrepareConfig()                        { Bootstrap(BootstrapPhases::PrepareConfig ); }

void App::onBsPreloadVariables()            { APP.GetConfig()->PreloadVariables<lox::Variables>(); }

void App::onBsCLIDefine() {
    cli.Init( &APP );
    cli.DefineExitCodes <ExitCodes >();
    cli.DefineParameters<Parameters>();
    cli.DefineCommands  <Commands  >();
    cli.DefineOptions   <Options   >();
}

void App::onBsCLIReadOptions()                                                { cli.ReadOptions(); }

void App::onBsConfigureCLI() {
    // set remaining cli args to configuration and allow shortcuts for cli variables
    variables::CLIVariablesPlugin* cliParameterPlugin=
                          APP.GetConfig()->GetPluginTypeSafe<variables::CLIVariablesPlugin>();
    for( auto& arg : cli.OptionArgsIgnored )
        cliParameterPlugin->AlternativeArgs.emplace_back( arg );

    cliParameterPlugin->DefaultCategories.emplace_back(A_CHAR("ALIB"));
    cliParameterPlugin->DefaultCategories.emplace_back(A_CHAR("ALOX"));
}


void App::onBsImportConfig() {
    LocalAllocator2K cfgFilePathAllocator;
    StdVectorMA<ConfigFileDescriptor> files(cfgFilePathAllocator);
    getConfigFilePaths(files);

    // loop over cfg files (if opening fails, nothing is imported)
    for ( size_t fileNo= 0; fileNo < files.size() ; ++fileNo ) {
        ConfigFileDescriptor& fileInfo= files.at(fileNo);
        if (fileInfo.Pathname.IsEmpty())
            continue;

        // import variables
        IniFileFeeder iniFileFeeder(*APP.GetConfig());
        try { iniFileFeeder.ImportStart( fileInfo.Pathname ); }
        catch (Exception& e) {
            if (e.Type() != variables::Exceptions::ErrorOpeningFile)
                throw;
            fileInfo.WasEmpty= true;
            continue;
        }
        iniFileFeeder.ImportAll();
        iniFileFeeder.ImportEnd();
}   }

void App::onBsFinalizeBootstrap()                                                   { Bootstrap(); }

void App::onBsReadDryRunOption() {
    auto* dryOpt= cli.GetOption( Options::DryRun );
    if( dryOpt != nullptr ) {
        dryrun= true;
        if( dryOpt->Args.IsNotEmpty() ) {
            lang::Bool val= lang::Bool::False;
            Substring arg= dryOpt->Args.front();
            enumrecords::Parse( arg, val );
            dryrun= (val == lang::Bool::True);
}   }   }



void App::onBsSetupALox() {
    ALIB_ASSERT_ERROR( alib::lox::Log::DEBUG_LOGGER == nullptr, "APP",
                       "DEBUG_LOGGER already created. Obviously logging was used "
                       "prior to the invocation of bs30SetupALox. This can cause issues" )

    // Create release lox
    if ( HasBits(flags, Flags::CreateReleaseLox ) ) {
        ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
        Lox_Prune( releaseLox   = monomem::GLOBAL_ALLOCATOR().New<alib::Lox>(releaseLoxName) );
        Lox_Prune( releaseLogger= alib::lox::Lox::CreateConsoleLogger(releaseLoggerName); )
        Lox_SetVerbosity( releaseLogger, Verbosity::Info   , "/"  )

        // Set the debug-lox to our release lox
        if (HasBits(flags, Flags::UseReleaseLoggerForDebugLogging ) ) {
            Log_Prune( Log::DEBUG_LOGGER= releaseLogger; )
            Log_Prune( Log::SetALibAssertionPlugin(releaseLox); )
            Log_Prune( alib::lox::DEBUG_LOX= releaseLox; )
    }   }
}

//==================================================================================================
// Shutdown
//==================================================================================================
void App::onSdAnnounceShutdown()                     { alib::Shutdown( ShutdownPhases::Announce ); }


void App::onSdCleanALox() {
    #if ALOX_DBG_LOG
        if ( HasBits(flags, Flags::ALoxVerbosityExportAllAndWriteBackDbgLogger ) )
            Log_SetVerbosityExport( "DEBUG_LOGGER", true )
    #endif

    // set ExportAll and writeback with ALox verbosity-variables which were not in the
    // INI-file before. We do this regardless if there is a INI-file or not. It might benefit
    // for custom configurations systems likewise.
    if (    HasBits(flags, Flags::ALoxVerbosityExportAllAndWriteBackRelLogger)
         && releaseLogger ) {
        Lox_SetVerbosityExport(releaseLogger, true )
        Log_Prune( Log::SetALibAssertionPlugin(nullptr); )
        Log_Prune( Log::DEBUG_LOGGER= nullptr; )
        Lox_RemoveLogger( releaseLogger )
        Log_Prune( alib::lox::DEBUG_LOX= nullptr; )
        Lox_Prune(delete  releaseLogger );
}   }

void App::onSdExportConfig() {
    LocalAllocator2K cfgFilePathAllocator;
    StdVectorMA<ConfigFileDescriptor> files(cfgFilePathAllocator);
    getConfigFilePaths(files);

    // loop over two (possible) files
    for ( ConfigFileDescriptor& fileInfo : files ) {
        if (fileInfo.Pathname.IsEmpty())
            continue;

        // Open INI-file
        IniFileFeeder iniFileFeeder(*APP.GetConfig());
        iniFileFeeder.ExportStart( fileInfo.Pathname );

        // export variables that are not existing in the INI-file yet
        int cntChanges= 0;
        Tokenizer tknzr(fileInfo.Exports, ',');
        while (tknzr.HasNext()) {
            auto& tok= tknzr.Next();                     // for root, just the direct children
            int changes= iniFileFeeder.ExportSubTree( tok, tok.Equals(A_CHAR("/")) );
            if ( changes > 0)
                cntChanges+= changes;
        }

        auto& iniFile= iniFileFeeder.GetIniFile();
        // If INI-file was empty, set comment
        if(     iniFileFeeder.DidNotExistOrWasEmpty
            && fileInfo.Comment.IsNotEmpty() ) {
                String4K buf;
                Paragraphs p(buf);
                p.LineWidth= iniFileFeeder.LineWidth;
                p.AddMarked( fileInfo.Comment )
                 .RemoveLastNewLine();
                iniFile.FileComments.Allocate(iniFile.Allocator, buf);
                cntChanges++;
        }

        // add section comments (if not existing)
        cntChanges+= iniFileFeeder.AddResourcedSectionComments(BASECAMP , "CFG_SECT_CMT_" );
        cntChanges+= iniFileFeeder.AddResourcedSectionComments(ALOX     , "CFG_SECT_CMT_" );
        cntChanges+= iniFileFeeder.AddResourcedSectionComments(APP      , "CFG_SECT_CMT_" );

        // write INI-file, if changed.
        if( cntChanges > 0 ) iniFileFeeder.ExportEnd( fileInfo.Pathname );
        else                 iniFileFeeder.ExportEnd();

    } // loop over 2 files
}

void App::onSdOutput() {
    machine.DbgDumpFurtherExitCodes(cOut->Buffer);
    if ( cOut->Buffer.IsNotEmpty() ) {
        std::cout << cOut->Buffer;
        if (!cOut->Buffer.EndsWith(NEW_LINE))
            std::cout << std::endl;
        cOut->Buffer.Reset();
    }
    std::cout.flush();

    if ( cErr->Buffer.IsNotEmpty() ) {
        std::cerr << cErr->Buffer;
        if (!cErr->Buffer.EndsWith(NEW_LINE))
            std::cerr << std::endl;
        cErr->Buffer.Reset();
    }
    std::cerr.flush();
}


void App::onSdFinalizeShutdown() {
    // Dump resources
    #if ALIB_DEBUG_RESOURCES
         std::cout << std::endl;
         std::cout << "---------------- Resource Pool Dump ----------------" << std::endl;
             auto resourceList= APP.GetResourcePool().DbgGetList();
             std::cout <<  resources::DbgDump( resourceList ) << std::endl;
         std::cout << "---------------- Resource Pool Dump (end) ----------" << std::endl;
    #endif

    // Dump Boxing Info
    #if ALIB_DEBUG_BOXING
        std::cout << std::endl;
        std::cout << "---------------- Debug Boxing ----------------" << std::endl;
            std::cout << alib::boxing::debug::DumpVTables(false);
            std::cout << alib::boxing::debug::DumpFunctions( alib::boxing::debug::GetKnownFunctionTypes() );
            std::cout << alib::boxing::debug::DumpAll();
        std::cout << "---------------- Debug Boxing (end) ----------" << std::endl;
    #endif

    if (cOut) cOut->~Paragraphs();
    if (cErr) cErr->~Paragraphs();

    // finalize ALib shutdown
    alib::Shutdown();
}


//==================================================================================================
// Run
//==================================================================================================
void App::onRunStart() {
    if ( cliStop )
        return;

    String resource= APP.TryResource( "AppStart" );
    if ( resource.IsNotEmpty() )
        cOut->AddMarked( resource, GetName(), GetVersion() );

    // if 'dryrun' was recognized, output resource "DryRunStart"
    if( dryrun ) {
        resource= APP.TryResource( "DryRunStart" );
        if ( resource.IsNotEmpty() )
            cOut->AddMarked( resource, GetName() );
    }

    //------- check for option 'help' -------
    Option* option= cli.GetOption( Options::Help);
    if( option ) {
        if( !CLIUtil::GetHelp( cli, option, *cOut ) )
            exitWithHelpOutput( ExitCodes::ErrBadParamValue,
                                option->Declaration->Identifier(),
                                option->Args.front(),
                                option->Declaration->Identifier() );
        cliStop= true;
        return;
    }

    //------- check for option 'version' -------
    if( cli.GetOption( Options::Version ) ) {
        cOut->Add(GetVersion());
        cliStop= true;
}   }

void App::onRun() {
    if( cli.ArgsLeft.size() == 0) {
        if (!processCLICmd(nullptr) )
            exitWithHelpOutput(ExitCodes::ErrNoCmdGiven);
        return;
    }

    while(!cliStop && cli.ArgsLeft.size() > 0) {
        cli.ReadNextCommands();
        //------- No command recognized? This is allowed, calls processCLICmd with nullptr -------
        if( cli.NextCommandIt == cli.CommandsParsed.end() ) {
            // No command. If derived does not process no-commands, then throw.
            if (!processCLICmd(nullptr) )
                exitWithHelpOutput(ExitCodes::ErrNoCmdGiven);
            else
                continue;
        }

        //------- Command loop -------
        if ( Command* actCmd= cli.NextCommand(); actCmd != nullptr ) {
            ALIB_DBG( bool processed= )
            processCLICmd(actCmd);
            ALIB_ASSERT_ERROR(processed, "APP",
                "Command \"{}\" recognized but not processed by the App.",
                actCmd->Declaration->Identifier() )
            continue;
        }
        break;
}   }

void App::onRunEnd() {
    if ( cliStop )
        return;

    // if 'dryrun' was recognized, output resource "DryRunEnd"
    if( dryrun ) {
        String dryRunEnd= APP.TryResource( "DryRunEnd" );
        if ( dryRunEnd.IsNotEmpty() )
            cOut->AddMarked( dryRunEnd );
}   }


void App::exitWithHelpOutput( Enum exitCode, const String& helpTopic, Box fp1, Box fp2 ) {
    (void) fp1;
    (void) fp2;
    onSdOutput();
    CLIUtil::GetHelp( cli, helpTopic, *cOut );
    machine.SetExitCode(exitCode);
    throw Exception(ALIB_CALLER_NULLED, App::Exceptions::ControlledEarlyExit);
}

bool App::processCLICmd(Command* cmd) {
    if ( cmd == nullptr )
        return false;

    if( cmd->Declaration->Element() == Commands::Help )  {
        if( !CLIUtil::GetHelp( cli, cmd, *cOut ) ) {
            // Assert that one argument was consumed as the help topic.
            // No other circumstance could make GetHelp fail.
            ALIB_ASSERT(cmd->ConsumedArguments== 2, "APP") // 2 means the command and the argument
            exitWithHelpOutput( ExitCodes::ErrBadParamValue, cmd->Declaration->Identifier(),
                                ARG_VN[cmd->Position + 2], cmd->Declaration->Identifier()  );
        }
        cliStop= true;
        return true;
    }

    if( cmd->Declaration->Element() == Commands::Version )  {
        cOut->Add(GetVersion());
        return true;
    }

    if( cmd->Declaration->Element() == Commands::Info )  {
        if ( GetInfo().IsNotEmpty() )
            cOut->Add(GetInfo());
        cOut->Add("Configuration files:\n");
        printConfigFileInfo(*cOut);
        return true;
    }

    return false;
}



} // namespace [alib::app]
