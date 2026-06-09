// #################################################################################################
//  Documentation - ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_app    ALib Module App - Programmer's Manual
                             
\tableofcontents

\I{################################################################################################}
# 1\. Introduction # {#alib_app_intro}
   
The \aliblong offers several modules that directly support building complete applications:

- \alib_bootstrap: Handles all fundamental initialization and shutdown tasks.  
- \alib_resources: Manages externalized data such as strings, texts, and other resources.  
- \alib_variables: Provides runtime variables that can be imported from command-line arguments
                   and the system environment and imported from or exported to configuration sources.  
- \alib_camp: Integrates the three modules above.  

Together, these four modules already form a coherent foundation for building applications. 
This fifth top-level module presented here brings all of them together under a unified umbrella, 
which offers several key benefits:

- Correct and consistent use of the underlying modules is enforced by this module’s structure.
- Application code becomes much smaller because many repetitive and error‑prone tasks are handled
  centrally.
- New applications can be set up quickly by copying and adapting a minimal sample application.
- Configuration and logging behavior can be standardized across tools, improving maintainability.
- Cross-cutting aspects (like error handling, diagnostics, and environment‑dependent behavior) 
  can be configured once and reused.

As a user of \alib, the most important effect is that you save substantial time when getting started
and when maintaining your applications over the long term.  

The central type of this module is class #"App" that implements a linear state machine with 
pre-implemented helper facilities. For the CLI-processing, class #"CommandLine" is 
provided. While its basic initialization and use is nicely abstracted in class #"%App", 
the class can also be used stand-alone. Thus, this class and its helpers are documented 
in the later chapter #"alib_app_cli".

\I{################################################################################################}
# 2\. Class App # {#alib_app_app}

While most \alib modules are primarily optimized for orthogonality and performance, this module
pursues a slightly different goal: it aims to make the application’s startup, run, and shutdown flow
explicit and easy to follow. In other words, the code itself is structured as a step-by-step guide
to what an application does when it runs.

To achieve this, the central class #"App;3" orchestrates a simple, linear state machine that
drives your application through all relevant phases: bootstrap, run, and shutdown. The philosophy is
to provide a clear, overridable sequence of small methods that you can extend or replace without
having to re-implement infrastructure.

As common with other frameworks, class #"%App" is designed to be a base class for your application
class and its method #"App::Main;2" is the entry point:

\snippet "App/main.cpp"        DOX_APP_MINIMAL_APP


## 2.1 A Simple Linear State Machine ## {#alib_app_app_statemachine}

At the heart of class #"App" is inner class #"App::StateMachine" with its internal "program" 
represented by the field #"App::StateMachine::Program;3". 
The fields' type #"App::StateMachine::CommandList;2" is a vector of commands, each tying a "state" 
to the method that should execute at that state.<br>
The built-in states are listed in enumeration #"App::States;2" and roughly follow this pattern:

- Bootstrap resource preparation (e.g., camps, resource pools)
- Bootstrap configuration data (runtime variables)
- Bootstrap finalization
- Run phase (start → run → end)
- Shutdown (announce → clean up → export configuration → output → finalize)

The actual execution order is defined by the underlying integral value of the field 
#"App::StateMachine::Command::State;3".

Custom applications may define their own enumeration type of custom states and add them,
along with a custom method to be executed, to the program vector. Custom methods can be a regular 
member functions of your derived type and do not need to be virtual.
Adding states has to be done before calling #"App::Main;2". The constructor of the 
derived application class is a good place to do this. 
Finally, adding a custom state can be conveniently done with 
#"App::StateMachine::CommandList::Add;3" (which is using some C++20 template magic).

The following sample demonstrates this:

\snippet "src.samples/App/main.cpp"        DOX_APP_MINIMAL_APP_WITH_CUSTOM_METHOD

## 2.2 Everything is virtual ## {#alib_app_app_virtual}

All built-in steps are implemented as virtual methods. You can override any of them to customize
behavior or keep defaults where appropriate. Typical hooks include:

Phase     | Methods                           | Description
----------|-----------------------------------|---------------
Bootstrap | #"App::onBsSetCamps"              | Calls #"BootstrapAddDefaultCamps;2" (see \alib_camp).
   &nbsp; | #"App::onBsPrepareResources"      | Initialize resource pools and output buffers.
   &nbsp; | #"App::onBsSetNameVersionAndInfo" | Collects name/version/info strings(optionally via resources).                                             
   &nbsp; | #"App::onBsPrepareConfig"         | Calls #"alib::Bootstrap(BootstrapPhases);2" passing #"BootstrapPhases::PrepareConfig;2".
   &nbsp; | #"App::onBsPreloadVariables"      | Allows preloading variables which should later be written to a configuration file, even if they have not been used during the execution of the application.
   &nbsp; | #"App::onBsCLIDefine"             | Defines command-line processing arguments.                                         
   &nbsp; | #"App::onBsCLIReadOptions"        | Read command-line options. This is done early to allow influencing the configuration sources in the next state.                                         
   &nbsp; | #"App::onBsImportConfig"          | Load configuration data.
   &nbsp; | #"App::onBsConfigureCLI"          | Finalizes CLI after configuration import.                                         
   &nbsp; | #"App::onBsFinalizeBootstrap"     | Finalize common bootstrap.                                
   &nbsp; | #"App::onBsReadDryRunOption"      | Read dry-run option and set field #"App::dryrun".                                         
   &nbsp; | #"App::onBsSetupALox"             | Set up logging. (The default behaviour is manipulateable with #"App::Flags".)                                
Run       | #"App::onRunStart"                | Optional pre-step around an application's main logic.
   &nbsp; | #"App::onRun"                     | Provides the default CLI processing loop. May be overridden if an application omits CLI processing.                 
   &nbsp; | #"App::onRunEnd"                  | Optional post-step around an application's main logic.
Shutdown  | #"App::onSdAnnounceShutdown"      | Calls #"alib::Shutdown(ShutdownPhases)" passing #"ShutdownPhases::Announce;2".
  &nbsp;  | #"App::onSdCleanALox"             | Cleans the logging subsystem as established in #"App::onBsSetupALox".
  &nbsp;  | #"App::onSdExportConfig"          | Exports the configuration data (if wanted).
  &nbsp;  | #"App::onSdOutput"                | Write collected application output.
  &nbsp;  | #"App::onSdFinalizeShutdown"      | Calls #"alib::Shutdown(ShutdownPhases)" passing #"ShutdownPhases::Destruct;2".


Because these methods are virtual, you can keep the structure but replace the implementation of any
step, or even insert/remove steps by editing the program vector as described above.

## 2.3 Lifecycle Conveniences And Cross-Cutting Aspects ## {#alib_app_app_lifecycle}

- Protected Access<br>
  - All fields and methods of class #"App" are protected. Derived classes may publish functionality
    via getter-methods.
  - Only rather small applications should rely on the derived application class to be the 
    main provider of the application logic. If an application gets bigger, the derived app class
    should be seen as only the holder of those features that are built-in, like resource management,
    configuration data, etc. Thus, in their #"onRun" method, the application should call 
    entry methods of other custom types.  
       
- Camps and resources<br>
  - An application does not need to create an own, dedicated #"Camp" class. It is absolutely OK
    to feed own resources and configuration data into the predefined singleton #"alib::APP;2". 
  - An application might replace those resources that are placed in the singleton as defaults,
    for example, to perform language translations. 
    As with other \alibcamps, with configuration macro #"ALIB_CAMP_OMIT_DEFAULT_RESOURCES", the 
    predefined resources can also be excluded from the compilation 
    
- Configuration files<br>
  The default flow 
  - lets #"getConfigFilePathsFromCLIParam" adjust file paths from  CLI input such as `--config`, 
  - then derives missing entries from resources via #"getConfigFilePathsFromResources",
  - makes them absolute with #"getConfigFilePathsMakeAbsolutePaths", and finally
  - imports/exports variables using #"IniFileFeeder". 
   
  On shutdown, only new values or variables with #"IsWriteBack;writeback-flag" are 
  exported, honoring per-file export lists.

- Logging<br>
  If enabled with #"App::Flags::CreateReleaseLox;2", a release \b Lox and a release logger
  are created (names configurable via fields documented in the class). Flag
  #"App::Flags::UseReleaseLoggerForDebugLogging" attaches the release logger to the debug
  \b Lox as well.

- Buffered output and early flush<br>
  During execution, user-facing text is collected in #"App::cOut;2" and error text in
  #"App::cErr;2". The shutdown step #"App::onSdOutput" writes both to
  `std::cout`/`std::cerr`, flushes, and clears the buffers.<br> 
  Besides this automatic invocation, you may call this method at any time
  (e.g., after milestones or before/after long operations) to emit partial output early.

- Exceptions and exit codes<br>
  The method #"App::exceptionToExitCode" maps exceptions to exit codes and can be overridden.
  Implementations should call the base first and extend only if the base returns a nulled enum;
  this preserves standard \alib mappings while allowing application-specific codes.

- Entry point and singleton<br>
  Use #"App::Main;2" from your `main()` to pass CLI arguments and run the program.
  The application instance is available via the global variable #"APP_SINGLETON;3" while the app 
  runs. The templated convenience method #"app::Get" may be used to statically cast the instance 
  to the derived type.

- Integrated CLI<br>
  #"App" always owns a #"CommandLine" instance in the field #"App::cli;*". The dedicated CLI
  chapter below explains the default help/version/info commands, the bootstrap hooks, and how
  to keep or disable CLI behavior in derived applications.

\I{################################################################################################}
# 3\. App::cOut/cErr vs. ALox Output # {#alib_app_output}

This module offers two complementary channels for producing text:

- Clean, buffered user output via #"App::cOut;*" and #"App::cErr;*"
- Structured diagnostic output via ALox logging (release/debug \b Lox and attached loggers)

The key idea is to keep user-facing console output minimal and deterministic, while providing
rich diagnostics when needed or verbosity is requested. This is especially important for command-line
tools that print results for end users or for further processing (e.g., piping into other tools).

## 3.1 Purpose of cOut and cErr

Buffers #"App::cOut;*" (for standard output) and #"App::cErr;*" (for error output)
are foremost designed for command-line tools that print results for end users or for further
processing (e.g., piping into other tools).

- Both buffers are instances of type #"Paragraphs" (see `src/alib/format/paragraphs.inc`).
  `Paragraphs` is tailored to composing longer, well‑formatted texts (wrapping, indentation,
  headings, lists, etc.) as commonly needed for CLI help screens and summaries.
- They do not add meta information such as timestamps, log levels, or thread IDs.
- Output is collected during execution and emitted in a controlled fashion by
  #"App::onSdOutput;*", which writes to `std::cout` and `std::cerr`, flushes, and clears the
  buffers. You may call this method manually (e.g., after milestones) to emit partial output early.

In contrast, #"alib_mod_alox;ALox log output" is primarily for diagnostics and typically
includes meta-information that is \b not suitable for machine consumption via pipes.

## 3.2 When to use logging vs. cOut/cErr

For classic CLI-style applications, the recommended policy is:

- Prefer cOut/cErr for the program’s “canonical” output that users expect on the console.
- Use ALox logging only
  - in debug builds, or
  - when the user explicitly requests it (e.g., with an option like `--verbose`), or
  - to write logs into separate text files (e.g., `/var/tmp/yourapp.log`), keeping them
    separate from cOut/cErr.

Other applications may still mix the two: use cOut/cErr for user-visible results and employ
logging for additional detail where formatted log output is helpful. Just be deliberate to avoid
confusing users with interleaved formats on the console.

## 3.3 Practical guidelines

- Default channels
  - Write user-facing messages to `cOut`, and user-visible warnings/errors to `cErr`.
  - Avoid writing directly to `std::cout`/`std::cerr`; use the buffers and call
    #"App::onSdOutput;*" so output remains deterministic and nicely grouped.

- Flushing
  - `onSdOutput` runs late during shutdown by default. You can also invoke it at any time
    (e.g., after an important milestone or before a long-running step) to emit partial results.

- Logging setup
  - If #"App::Flags::CreateReleaseLox;*" is set, a release \b Lox and a release logger are
    created. With #"App::Flags::UseReleaseLoggerForDebugLogging;*", the same logger is also
    attached to the debug \b Lox.
  - Use ALox for diagnostics and detailed traces. Increase verbosity only when the user asks for
    it (e.g., `--verbose`) or in debug builds.
  - To keep console output clean, attach a file-based text logger and direct diagnostic logs there,
    leaving `cOut`/`cErr` for user-facing text.

- Mixing is allowed, but be intentional
  - It is fine to use both channels in one application. As a rule of thumb, prefer `cOut`/`cErr`
    for information users explicitly asked for (results, summaries, help text) and reserve
    logging for developers and advanced users.

## 3.4 Small examples

Buffered user output:
```cpp
// Collect user-visible messages
cOut->Add("Summary: processed 42 items\n");
cErr->Add("Warning: 2 items were skipped due to validation errors\n");

// Emit now (optional). Otherwise this happens in the corresponding state.
onSdOutput();
```

Selective logging (release/debug-logging, different verbosity levels):
```cpp
Lox_Verbose("Additional detail: {}", 42) // Release-logging (remains active in release-builds) 
Log_Info("Calculating results")          // Debug-logging (pruned in release-builds) 
```

With these conventions, command-line tools remain predictable and composable (clean stdout/stderr),
while developers and power users can still access detailed diagnostics via logging when needed.

## 3.5 Controlled Early Exit (Exceptions::ControlledEarlyExit) ## {#alib_app_controlledearlyexit}

Some applications want to stop processing “right now” and return a well‑defined exit code without
plumbing return values through many layers of code. For this common scenario, the module provides 
the exception type #"App::Exceptions::ControlledEarlyExit;*".

In short:
                                                        
- While in the #"States::Run;run phase", you set the desired exit code on the state machine using 
  #"SetExitCode".
- You typically print user output to either #"cErr" or #"cOut".
- You then throw an #"exc Exception" with the enum element #"ControlledEarlyExit".
- The framework catches this specific exception at the top level of the run loop., 
- In debug-builds, the default implementation of the method #"exceptionDisplay" will
  print the call site to allow developers to quickly navigate to the throwing code.
- The state machine simply proceeds with the next state, which is usually the shutdown phase.
- No mapping via #"App::exceptionToExitCode;*" is performed for this case; because the 
  paradigm explicitly asks for setting the exit code already at the throw site!

This replaces scattered “if (err) return err;” checks with a single, explicit signal that is
understood by the application framework. Because the exception is caught internally, user code does
not need to add handlers unless it wants to perform cleanup or logging at the throw site.

Important details:

- Valid context: The exception is meant to be thrown during the built‑in state
  #"App::States::RunStart", #"App::States::Run", or #"App::States::RunEnd". 
  In debug builds, \alib asserts if it is thrown from any other state.
- Exit code required: You must call `machine.SetExitCode(...)` before throwing. A debug assertion
  checks this as well.
- No payload: The exception carries no records or arguments. In debug builds, the default
  #"exc Exception" display adds the #"CallerInfo" so developers see the source
  location where it was thrown.
- Output prior to exit: If you want to print user‑facing messages, write them to `cOut`/`cErr`
  before throwing. They will be emitted by #"App::onSdOutput;*" later in the shutdown phase.

### 3.5.1 Minimal usage example

```cpp
void MyApp::onRun() {
    // ... detect a condition that requires immediate termination ...

    // 1) Choose an exit code
    machine.SetExitCode( MyExitCodes::FileNotFound );

    // 2) Optionally inform the user
    cErr->Add("Fatal: input file not readable\n");

    // 3) Abort processing in a controlled way
    throw alib::Exception( ALIB_CALLER_NULLED, App::Exceptions::ControlledEarlyExit );
}
```

### 3.5.2 When to use it - and when not

- Use `ControlledEarlyExit` when a condition deep in your logic should end the run phase immediately
  with a defined exit code, and you prefer not to bubble error codes up through every call.
- Prefer regular exceptions mapped via `exceptionToExitCode` when you want to centralize the
  mapping of many exception types to exit codes or when the failure should be treated like any other
  error (with stack‑dependent diagnostics, etc.).

Both approaches work together: mapping handles the "usual" errors, while `ControlledEarlyExit`
covers the intentional, well-known early-termination path without return-code plumbing.


\I{################################################################################################}
# 4\. CLI Integration In App # {#alib_cliapp}

CLI support is implemented with a few separated types placed in this namespace/module.
While these classes may be used directly (standalone), class #"App" makes their use much easier. 
All details of the CLI interface are given in the later chapter #"alib_app_cli". 

## 4.1 CLI Facilities Built Into Class App ## {#alib_app_cli_provides}

Every instance of #"App" contains:

- a #"CommandLine" parser in field #"App::cli;*",
- a stop flag #"App::cliStop;*",
- bootstrap states dedicated to CLI setup and early option handling, and
- some default implementations that connect CLI parsing and even provides some 
  features like <em>--version</em>, <em>--dryrun</em>, or <em>--help=topic</em>.


## 4.2 CLI-Related States And Hooks ## {#alib_app_cli_states}

The built-in state machine contains these CLI-related steps:

- #"App::States::CLIDefine;*"
  - implemented by #"App::onBsCLIDefine;*"
  - initializes #"App::cli;*" and registers the built-in commands, options, parameters and
    exit-code declarations.

- #"App::States::CLIReadOptions;*"
  - implemented by #"App::onBsCLIReadOptions;*"
  - parses options early, so configuration-relevant options such as `--config` are already known
    before configuration files are imported.

- #"App::States::ConfigureCLI;*"
  - implemented by #"App::onBsConfigureCLI;*"
  - finalizes the CLI after configuration import, for example by wiring ignored option arguments
    into the variables plugin and enabling shortcut categories.

- #"App::States::ReadDryRunOption;*"
  - implemented by #"App::onBsReadDryRunOption;*"
  - evaluates the built-in `--dryrun` option before logging is set up.

All four hooks are virtual. Applications commonly override the first three of them by invoking
the default implementation first and then performing the similar steps adding/handling the custom
commands and options.<br>
Then method #"App::processCLICmd" is to be implemented to perform the actual work.
 
Applications that do not want CLI behavior can replace the methods with empty implementations and 
directly override the method #"%App::onRun;".

## 4.3 Built-In CLI Behavior ## {#alib_app_cli_builtin}

The default CLI bootstrap already defines a small standard vocabulary:

- commands `help`, `version` and `info`,
- options `--help`, `--version`, `--config` and `--dryrun`,
- the help-topic parameter `TOPIC`, and
- a set of standard exit codes for parsing and configuration failures.

The default run path then provides these behaviors:

- `"help"` and `--help` render help text through #"CLIUtil::GetHelp(const String&);" into #"App::cOut;*".
- `"version"` and `--version` write #"App::GetVersion;*" to #"App::cOut;*".
- `"info"` writes #"App::GetInfo;*" and configuration file information collected by
  #"App::printConfigFileInfo;*".
- `--config` influences the list built by #"App::getConfigFilePaths;*" through
  #"App::getConfigFilePathsFromCLIParam;*".
- `--dryrun` sets #"App::dryrun;*" early so later bootstrap or run logic can react to it.

This is intentionally modest. Derived applications are expected to add their own commands and
options and to override #"App::processCLICmd;*" for the real work.

## 4.4 Declaring Additional Commands And Options ## {#alib_app_cli_define}

The default #"App::onBsCLIDefine;*" registers the built-in enums with class #"CommandLine".
Derived applications typically extend that definition step with their own declarations.
For this purpose, custom enumeration types have to be provided, which are resourced 
accordingly. 
             
The best approach to do this, is to extend the sample application shown in the chapter 
#"alib_app_sample". Otherwise, all details are given in chapter #"alib_app_cli".

## 4.5 Configuration Files Selected From The CLI ## {#alib_app_cli_cfgfiles}

Configuration file selection a further feature of the regular #"App" workflow:

- The method #"App::onBsCLIReadOptions" parses options which influence the configuration file selection.
- The method #"App::getConfigFilePaths" calls #"App::getConfigFilePathsFromCLIParam;*" first.
- Next #"App::getConfigFilePathsFromResources;*" choses file paths from resources, if no other
  ones were configured. In addition this method performs some other convenience steps as documented.
- #"App::getConfigFilePathsMakeAbsolutePaths;*" normalizes the resulting paths.

The default implementation of #"App::getConfigFilePathsFromCLIParam;*" interprets `--config` as a
comma-separated list of file names. Entries left as `Default...` keep the resource-defined path at
that position. Descendants may override the method to support different CLI syntax.

This preserves resource-based defaults while still giving users a straightforward way to redirect
configuration storage.

## 4.6 Processing Commands ## {#alib_app_cli_processing}

The default #"App::onRun;*" is a simple CLI loop:

- it consumes parsed commands from #"App::cli;*",
- dispatches each recognized command to #"App::processCLICmd;*",
- stops when #"App::cliStop;*" becomes true or no further commands are left, and
- uses #"App::exitWithHelpOutput;*" for standard error/help flows.

This gives derived types two common extension patterns:

- Override #"App::processCLICmd;*", call the base first and then perform the actual work.
  This is the standard choice for command-oriented tools.
- Override #"App::onRun;*" completely.
  This is useful if your application does not follow a command-driven CLI model at all.

## 4.7 Generating Help Pages ## {#alib_app_cli_help}

The class #"CLIUtil" provided with this module can generate rich help pages and write them into
#"App::cOut":

- Command and option metadata registered with the CLI tools can be turned into formatted help text,
  taking localized resource strings into account.
- This makes entry points like `--help` and topic-specific help such as `--help=topic`
  straightforward.
- Because `cOut`/`cErr` are buffers of type #"Paragraphs", line wrapping and layout are handled 
  to suite terminal output.

Its output for command `help` may look as follows:

\verbatim
----------------------------------------------------------------------
ALib Resource Compiler V. 2605.1
(c) 2023-2026 AWorx GmbH. Published under MIT License (Open Source).
For more information, see: https://alib.dev/alib_mod_resources.html
----------------------------------------------------------------------

ABOUT ALibRC
  This is a tool provided by the C++ Framework ALib that compiles external resources and inserts a corresponding code
  snippet into a C++ source file. The resources are specified in a resource file, while the C++ source file must exist
  and contain special markers for the insertion position.
  All details are given in the documentation of the ALib C++ Framework at:
    https://alib.dev/ 

USAGE:
  alibrc resourcefile cppfile 

COMMANDS:
  * ALibRC help [TOPIC]
    Shows general help or help on topics
  * ALibRC version
    Shows the version of this software.

OPTIONS:
  --help[[=]TOPIC]
  --version
  -d|--dryrun[=yes|no]
  --verbose[="level"]
  --validate resourcefile [cppfile]
EXIT-CODES:
      1: ErrNoCmdGiven
         No command given.
      3: ErrParsingOption
         Unknown option {!Q} was given.
      4: ErrParsingCommand
         An error occured when parsing the command {!Q}:
      5: ErrMissingCmdParam
         Missing a mandatory parameter {!Q} of the given command {!Q}.
      6: ErrMissingOptParam
         Missing a mandatory parameter {!Q} of the given option {!Q}.
      7: ErrBadParamValue
         Parameter {!Q} is not suitable for command or option {!Q}.
    101: ErrMissingRCFilename
         No input resource file name given.
    102: ErrRCFileNotFound
         Given resource file not found.
    103: ErrRCFileNotAccessible
         Resource file is not readable.
    104: ErrMissingCPPFilename
         No C++ source file to patch given.
    105: ErrCPPFileNotFound
         Given C++ resource file not found.
    106: ErrCPPFileNotAccessible
         C++ file is not writable.
    107: ErrInResources
         The resource file is erroneous.
\endverbatim
                               
Note that for creating this output, not a single line of custom code has to be written.
Instead, the help texts are dynamically generated from the defined commands, options,
parameters and exit codes and their according resource strings.

## 4.8 Applications Without A CLI ## {#alib_app_cli_without}

Even though the class #"App" includes CLI support by default, an application that does not expect 
commands or options is easy to create:

- Override #"App::onRun;*" with your own application logic.
- If desired, override #"App::onBsCLIDefine", #"App::onBsCLIReadOptions",
  #"App::onBsConfigureCLI" and #"App::onBsReadDryRunOption" with empty implementations.
- Keep using the rest of the lifecycle unchanged: camps, resources, configuration, output,
  logging and shutdown still work the same way.

This keeps the common case convenient without forcing a command model onto every application.

                                                                          
\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 5\. Sample Application # {#alib_app_sample}

The various small details of classes that need to be reflected when using #"App" and 
#"CommandLine", is best handled by using a sample application as a jump-start.<br>
A very small sample application is provided with the ALib C++ Framework which implements a
command-line tool that reports the current date or the modification date of a file.
The declaration of the class looks like this:

\snippet "App/sample.hpp"        DOX_APP_SAMPLE_CLASS
                                                                 
The full header file can be seen here: #"F;App/sample.hpp;3". 
Its implementation is provided in the file #"F;App/sample.cpp;3".

When an application becomes more complex, especially by the additional burden of defining some 
resource strings, we recomment to use the sources of the #"alib_camp_rc;ResourceCompiler Tool" 
as a jump-start.<br>
From here, your custom application can be developed step by step, and things should become
relatively easy and clear.<br>
The compiler is a rather simple tool whose parsing and code-generation logic is encapsulated in the
the module \alib_camp and hence just a little application-specific code is contained in the
project.

This tool is found in the folder: 

    ALIB_BASE_DIR/tools/ResourceCompiler

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
                

\I{################################################################################################}
# 6\. The Command Line Interface Implementation # {#alib_app_cli}

\I{################################################################################################}
## 6.1 Introduction # {#alib_app_cli_intro}

\note
  The types described here may be used stand-alone from class #"App". Nevertheless, it is 
  recommended to use the #"%App" class for the most common use cases, because it provides a 
  convenient way to set up the CLI and to perform the actual work.<br>
  Also, even reading this manual chapter may not be required, if the sample application 
  described in the previous chapter #"alib_app_sample" is used as a jump-start.

\note
  Command-line parsing is a tricky and - due to the lack of strict standards - a rather 
  cumbersome task. We tried our best to provide a simple and robust implementation that enables 
  the most common command-line tools requirements.

The types introduced in this chapter support parsing and processing command-line parameters.
Such processing is not only useful with "pure CLI commands" (shell applications) but also with
software that only optionally receives arguments, may it be daemons or GUI-driven applications.

This manual and library uses four terms for CLI input:
- \b commands: input strings that do not start with hyphens
- \b options: input strings that do start with one or two hyphens
- \b parameters: input strings that are following commands or options or that are attached
     to options using, for example, an equal sign \e '='.
- \b arguments: any of the three above.

\I{################################################################################################}
### 6.1.1 The Challenge of Command Line Interfaces # {#alib_app_cli_intro_challenge}
Command line parameter "languages" are in most cases not defined in very consistent ways, 
for example, in comparison to programming languages. 
In contrast, they are very much dependent on the field of application. 
In favor of simplicity of usage, in most cases no consistent grammar and syntax definition exists. 
Different "commands" sometimes share "parameters", others do not and often "parameters" may be 
given without a command. Then, commands and options/parameters are often allowed to be
given in arbitrary order. Finally, options may allow to omit the hyphens and so forth.

Therefore, by definition, helper libraries like this one, can never support each and every use case
and any unforeseen tweak that a programmer wants to implement for his specific CLI.

As a result, the software found in this module:
- Uses an open, pragmatic code style. For example, fields are mostly public and the interface
  methods allow data modifications between the several steps of their invocation.

- Is incomplete and even partly inconsistent in respect to what can be defined as commands,
  parameters, etc. and what action can be "automated" with the information provided.

This means, that using the types provides basic tools and probably <em>a guideline</em> on how to
implement the CLI processing, but not a full-featured CLI processor that just needs a grammar
definition input and some callback functions.<br>

This sort of "incompleteness" can also positively phrased: The implementable grammar and syntax of
the CLI interface is open for implementing any weird interface and not limited to a certain scheme.

One major burden that this \alibmod may relieve a programmer from, is that CLI command names and 
other syntax tokens, and probably more important, the error messages and help texts that the CLI 
interface produces, are duly externalized and thus can be easily translated into other human 
languages. This is achieved by the provision of an \alibcamp and thus leveraging inner namespace
#"alib::resources;2" of the module \alib_resources.

\I{################################################################################################}
### 6.1.2 ALib Enum Records And ALib Resources # {#alib_app_cli_intro_enum_records}
The classes found in this namespace make a lot of use of #"alib_enums_records;ALib Enum Records" 
retrieved from an instance of type #"ResourcePool", usually the one found in the singleton of 
the associated #"Camp".

Therefore, it is good advice to get familiar with the features provided by inner namespace
#"alib::resources;2" of the module \alib_resources as a start. That module in turn uses so-called
#"alib_enums_records;Resourced ALib Enum Records" which are provided with the module
\alib_enumrecords.

Or, the other way round: The tutorial found below might also be a good starter to understand
#"alib_mod_resources;externalized resources" and \alib_enumrecords_nl in their basics, and
therefore the better advice might be to just continue reading.

\I{################################################################################################}
### 6.1.3 Features # {#alib_app_cli_intro_features}
The CLI-types of this module \alib_app_nl offer the following features:

- Rather simple interface through central class #"CommandLine".
- Resourced and optionally localized command names, output and help texts
- If commands or options expect mandatory arguments, such arguments may be read
  automatically and attached to the command objects as additional information.<br>
  Optional arguments might also be read automatically, depending on their specification.
- Support for general help output (e.g., with <em>--help</em> ) as well as
  help output on certain topics (e.g., with <em>--help commandXYZ</em>).
- CLI input - or parts of it that have not been "consumed" - may be passed forward into other
  parts and libraries embedded in an application.<br>
  For example, \alib module \alib_variables allows overwriting configuration data stored in
  configuration files transparently with by giving command-line options. CLI input which is not
  processed by this module, is filtered out and can easily be forwarded to module \alib_variables,
  or other parts of the software that accept CLI input.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
## 6.2\. Classes and Paradigms # {#alib_app_cli_candp}

### 6.2.1 Defining Commands, Parameters, Options and ExitCodes ## {#alib_app_cli_candp_defines}

As a preparation to parsing command-line arguments, instances of the following structs are to be
created and duly filled with data:

1. #"CommandDecl"
2. #"ParameterDecl"
3. #"OptionDecl"
4. #"ExitCodeDecl"

Please use the links above and quickly read the general descriptions of the types.

Such struct definition is considered static data, and as such is usually stored in so-called
resources. How resourced data is used to automatically create and fill the structs is shown
in the next chapter #"alib_app_cli_tut".

All enums which are using resourced #"alib_enums_records;ALib Enum Records" are demanding a
specific record type to be associated to a custom enumeration.

In addition, for such custom enum types, a specialization of #"ResourcedTraits",
has to be provided, which is used to load further resource strings.

The following table summarizes the types, the associated <em>ALib Enum Record</em> type and
the additional resource strings that need to be available:

|Object Type       | Record Type To Use| Additional Resource Strings
|------------------|-------------------|-----------------------------------------------------
|#"CommandDecl"    | #"ERCommandDecl"  |  <b>"THlpCmdSht_NN"</b>: Short version of help text for each parameter.<br><b>"THlpCmdLng_NN"</b>: Long version of help text for each parameter
|#"ParameterDecl"  | #"ERParameterDecl"| <b>"THlpParSht_NN"</b>: Short version of help text for each parameter.<br><b>"THlpParLng_NN"</b>: Long version of help text for each parameter.
|#"OptionDecl"     | #"EROptionDecl"   | <b>"TOptUsg_NN"</b>: Help text for each option describing its usage.<br><b>"TOptHlp_NN"</b>: Help text for each Option.
|#"ExitCodeDecl"   | #"ERSerializable" | <b>"TExit_NN"</b>: A format string returned by method #"ExitCodeDecl::FormatString;*".

\I{################################################################################################}
### 6.2.2 Main Class 'CommandLine' ## {#alib_app_cli_candp_cmdline}
With the above resourced definitions in place, class \b CommandLine is used to
bootstrap all enum records to be ready for parsing the command-line.

To avoid cluttering this types' interface, various helper methods have been put into
friend class \b CLIUtil.

See the reference documentation of types #"CommandLine" and #"CLIUtil" for details.

Alternatively, just follow the tutorial provided in the next chapter.

\I{################################################################################################}
\I{###############################     3. Tutorial      ###########################################}
\I{################################################################################################}

## 6.3\. Tutorial Sample # {#alib_app_cli_tut}
Due to the usage of #"alib_mod_resources;resourced strings", and
even more due to the usage of #"alib_enums_records;Resourced ALib Enum Records", to
start a new project using the command-line facilities of this module becomes more complex than
one might think.<br>
The best way out is the provision of a "template" project, which can be used for jump-starting
into an own development.

Such template is provided with this tutorial chapter!
\note The full source code of the sample can be seen (and copied!) here:
      - #"F;CLI/sample.cpp;3"
      - #"F;CLI/samplecamp.hpp;3"
      - #"F;CLI/samplecamp.cpp;3"
\note      
   Because the documentation tool used here (overall exquisite software \https{Doxygen,doxygen.nl})
   destroys some formatting, it is recommended to copy the sources directly from your filesystem
   instead of from this tutorial chapter.<br>
   
\note      
   However, as already mentioned, <b>the even better approach</b> is to use the CLI-types embedded
   in class #"App" as sampled in chapter #"alib_app_sample".
            
The sample application is a little similar to the GNU/Linux "date" command. Its functionality
is described as follows:
- Without any argument, the current date and time should be written (to the standard output stream).
- Alternatively, if the argument "now" is given, the same should be performed.
- To receive the modification date of a file or directory, the command \e 'file' has to be
  given along with a non-optional next argument containing the filename.
- Option \e '--format' allows modifying the date format of the output. The format string has
  to be attached to the option using equal sign \e '=' and may (must) be quoted either with
  single or double quotes.
- A help output shall be available with either a command \e "help" or option \e "--help"
  In the first case an optional help topic may be added separated by a whitespace, in the
  option case, such a parameter shall be added using equal sign \e '=' (without whitespaces).
- Finally, more than one command should be able to be passed, for example:
        date now file /etc
  which would first print the current date and time and in a second row the modification date
  of directory \e "/etc".

With this project scope in mind, let's start coding.

\I{################################################################################################}
### 6.3.1 Definition of Enumerations # {#alib_app_cli_tut_enums}
Commands, options, parameters, and exit codes of our software are easily defined with
simple C++ enumerations:

\snippet "samples/CLI/samplecamp.hpp"        DOX_EXPR_TUT_CLI_ENUMS

The little magic comes with assigning #"alib_enums_records;ALib Enum Records" to these
enumerations using the helper macros. For each of the enumerations, a corresponding record type
is defined with this module:

\snippet "samples/CLI/samplecamp.hpp"        DOX_EXPR_TUT_CLI_ENUMS_ASSIGN

This assignment entitles the main class of this module to "know" how to parse the objects
from the command-line.

\I{################################################################################################}
### 6.3.2 Definition of a Custom Camp # {#alib_app_cli_tut_module}
The enum records that just have been assigned to the enums need to be filled with values.
Such values can be automatically loaded from resources.
To define such resources, the easiest way is to create a custom \alibcamp.

\note The CLI system does not force you to create a custom camp. Alternatively, the creation of
      a #"ResourcePool" instance would be enough.
      However, when you create your own, more complex software, it might be a good advantage to 
      have such a camp in place from the start. A camp solves several problems, for example,
      the often annoying C/C++ bootstrapping issues. And, as you will see, creating a module
      instead of a \b ResourcePool is not very complicated.

This is the declaration of the camp in the header file:
\snippet "samples/CLI/samplecamp.hpp"    DOX_EXPR_TUT_CLI_CUSTOM_CAMP_DECL

\alibcamps_nl are singleton objects, and as such we create one global instance:

\snippet "samples/CLI/samplecamp.hpp"    DOX_EXPR_TUT_CLI_CUSTOM_CAMP_SINGLETON

\I{################################################################################################}
### 6.3.3 Assigning the Resources to the Enumerations # {#alib_app_cli_tut_assign_resources}
Now, as the camp is defined, a second macro is used which tells \alib where to find the
resources for each enumeration:

\snippet "samples/CLI/samplecamp.hpp"        DOX_EXPR_TUT_CLI_ENUMS_ASSIGN2

\I{################################################################################################}
### 6.3.4 Implementation of the Camp # {#alib_app_cli_tut_camp_impl}
The code shown above, was all from the header file. Now we turn to the implementation part.
Let's start with the implementation of the custom \b Camp. It is full of resource strings!                                 
                                                           

\snippet "samples/CLI/samplecamp.cpp"    DOX_EXPR_TUT_CLI_CUSTOM_CAMP_IMPL

Being an experienced programmer, if you have a brief look at the string data defined in the 
code above, the general idea about the sort of information that is provided should become clear.
Together with the macros used in the headers, parts of the resources are now "attached" to the 
C++ enums.

\I{################################################################################################}
### 6.3.5 Function main() # {#alib_app_cli_tut_main}
Now, we are set to implement function <em>main()</em>! <br>
To make this easier to read, we pull the command processing into a separate function, which is
forward-declared like this:

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_FWDDECL

With this extraction of the custom command processing itself,
function main becomes quite short and straightforward. 

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_MAIN

\note You may have noticed the numbering of 7 steps in the comment lines. Again we want to hint
      to the higher-level module \alib_app. Using that, separates these steps (and more!) 
      in a simple little state-machine, with many already built-in features.  

Finally, we can look at our custom command processing function.
Also here, just note the source comments. Everything should be quite understandable:

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_PROCESS

You might have noticed that this function uses static utility methods of class
#"CLIUtil" to generate help texts for general use, or specifically for commands, options
and parameters.

\I{################################################################################################}
### 6.3.5 Sample Invocations # {#alib_app_cli_tut_invocations}
After compiling this sample to binary \b "Sample" it can be invoked from the command-line.
Lets have some tries:

<b> 1. Invocation without parameters </b><br>
This is already a special case that method \b processCLI explicitly handles with the same code
as if parameter <em>now</em> was given.

        Sample

The result shown on the console is:
\verbinclude "DOX_CLI_NOCMD.txt"

\note In all output samples, the return code, which often is an important piece of information
      of CLI software, is shown as <em>\<Exitcode N\></em>.
      This is not generated as output, but added here for this tutorial.

<b> 2. Command 'now' </b><br>
With command <em>now</em>

        Sample now

the same result is shown:
\verbinclude "DOX_CLI_CMDNOW.txt"

(Note that the date displayed in this tutorial is when the unit tests were run the last time
before creating the documentation with doxygen.)

<b> 3. With option 'format' </b><br>
Option <em>format</em> allows specifying a date format, as documented with
#"CalendarDateTime::Format". The format string can be given concatenated to
the option name using an equal sign <em>'='</em> or just as a next argument (with a whitespace
instead of the equl sign):

        Sample --format="dd MMM YYYY"

Outputs:
\verbinclude "DOX_CLI_CMDNOW_FORMAT.txt"

<b> 4. Command 'file' </b><br>
The command <em>file</em> expects a next command-line argument (separated by whitespaces), that
specifies a file or directory name. We choose "/home" here and hope that this is available on
the machine where this documentation was generated:

        Sample file /home

Outputs:
\verbinclude "DOX_CLI_CMDFILE.txt"

<b> 5. Erroneous input </b><br>
Lets see what happens if we omit the mandatory parameter of command <em>file</em>:

        Sample file

Outputs:
\verbinclude "DOX_CLI_FILE_MISSING_PAR.txt"

Similar to this, we might omit the parameter of option <em>format</em> :

        Sample --format

Outputs:
\verbinclude "DOX_CLI_FILE_MISSING_PAR2.txt"

In contrast to the previous output, this one shows a very different error message.
The reason for this is as follows:
While the missing parameter of command <em>file</em> is detected by our simple sample program
(which generates a quick error message), the missing mandatory parameter of option <em>--format</em>
can be detected by the CLI system internally and automatically.
Here, the library throws an #"exc Exception", which is "annotated" with further information
while it is passed through the unwinded execution stack.

Consequently, this sample should be extended to fetch and analyze the internal exceptions and
create a similar output as in the first case. We kept this here as it is to <b>a)</b> keep the sample
code simple and <b>b)</b> demonstrate the power of <b>ALib Exceptions</b> in respect to the generation
of human-readable, formatted error output.<br>
More complex projects that use this library should follow the approach to use and throw \alib exceptions
anywhere possible. Only in the very end (for example, at the end of the main() method), such exceptions
should get "translated" to one of the more general exit codes that command-line software usually
returns. The benefits: Error handling is then all in one place, and internally it is all done nicely
with C++ exception handling.

A last erroneous input we want to test here is the provision of unknown commands or options:

        Sample unknown

outputs:
\verbinclude "DOX_CLI_UNKN_CMD.txt"

while

        Sample --unknown
shows:

\verbinclude "DOX_CLI_UNKN_OPT.txt"

<b> 6. Help texts</b><br>
The sample of this tutorial does not show too much code that generates help messages.
The reader might be surprised about the output of

        Sample --help

The library is able to generate the following output:

\verbinclude "DOX_CLI_HELP.txt"

Furthermore, help topics are supported. Possible topics are commands, options and parameters
as shown in these final samples:

        Sample --help now

\verbinclude "DOX_CLI_HELP_NOW.txt"


    Sample --help file

\verbinclude "DOX_CLI_HELP_FILE.txt"

    Sample --help format

\verbinclude "DOX_CLI_HELP_FORMAT.txt"

\I{################################################################################################}
\I{###############################   4. Detail Topics    ##########################################}
\I{################################################################################################}
## 6.4\. Detail Topics # {#alib_app_cli_details}

\I{################################################################################################}
### 6.4.1 Allowing Other Parts Of The Software To Access CLI Arguments ## {#alib_app_cli_detail_arg_sharing}
Independent of the use of this \alibmod_nl, the original command-line strings are always
available through the global \alib variables #"ARG_C;2", #"ARG_VN;2" and #"ARG_VW;2" (if set
by the user's <c>main()</c>-function properly).

Class #"CommandLine" exposes field #"CommandLine::OptionArgsIgnored",
that hold all CLI arguments that start with a hyphen <c>'-'</c> and are not recognized
by #"OptionDecl". software might either generate an error if unrecognized options
are left (just like the tutorial-sample of the previous chapter does), or pass this list
to other parts and libraries that software is using..

With the use of camp \alib_variables such use of CLI arguments outside this camp already
occurs: #"Variable;Configuration variables" can be defined using CLI parameters
the same as by using environment variables, configuration files or other data sources.
Such use is still be possible, independent of the CLI interface.

The other way round, arguments may be removed from the internal list held by this class, prior
to having this class processing the arguments! Note that this is a must for arguments that do not
start with a hyphen (and are addressing other software parts), because such arguments would
lead to "unknown command" errors.
Such removal has to be done after the invocations of the various \b %Define methods and prior
to invoking #"CommandLine::ReadOptions;*".

\I{################################################################################################}
### 6.4.2 Undefine CLI Elements ## {#alib_app_cli_detail_undef}
For each component type, class #"CommandLine" provides a method to undefine single elements,
namely #"CommandLine::UndefineCommand", #"CommandLine::UndefineOption",
#"CommandLine::UndefineParameter", and #"CommandLine::UndefineExitCode".
By using these methods, such components are not recognized by the CLI processing and 
will not be displayed in the generated help messages.<br>

This can be used, for example, if software consists of different building blocks where some
foundational elements like exit codes or options are provided, while some derived components 
want to disable a part of these.
As an example, class #"App" of module \alib_app offers some built-in features
like option <c>--dryrun</c>, that a using code may want to disable and/or redefine. 
    

<br><br><br><br><br><br> */
