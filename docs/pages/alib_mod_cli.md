// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_cli   ALib Module CLI - Programmer's Manual


\tableofcontents

\I{################################################################################################}
# 1. Introduction # {#alib_cli_intro}

\note
  While being included in previous versions in an alpha state, with \alib Release 2312
  this module is considered to be in a rather stable state for the first time.<br>
  Still, functionality may change in the future (the \alib policy in general is open to
  incompatible changes with new releases), but so far \alib_cli_nl works quite well in an actual
  software project.

This \alibmod supports parsing and processing command line parameters.
Such processing is not only useful with "pure CLI commands" (shell applications) but also with
software that only optionally receive arguments, may it be daemons or GUI-driven applications.

\note
This manual and library uses four terms for CLI input:
- \b commands: input strings that do not start with hyphens
- \b options: input strings that do start with one or two hyphens
- \b parameters: input strings that are followed by commands or options or that are attached
     to options using for example an equal sign \e '='.
- \b arguments: any of the three above.

## 1.1 The Challenge of Command Line Interfaces # {#alib_cli_intro_challenge}
Command line parameter "languages" are in most cases not defined in very consistent ways, for example
in comparison to programming languages.  In contrast, they are very much dependent on the field of
application. In favor to simplicity of usage, in most cases no consistent grammar and syntax
definition exists. Different "commands" sometimes share "parameters", others do not and often
"parameters" may be given without a command. Then, commands and parameters are often allowed to be
given in arbitrary order, parameters may allow to omit the hyphens and so forth.

Therefore, by definition, helper libraries like this one, can never support each and every use case
and any unforeseen tweak that a programmer wants to implement for his specific CLI.

As a result, the software found in this module:
- Uses an open, pragmatic code style. For example, fields are mostly public and the interface
  methods allow data modifications between the several steps of their invocation.

- Is incomplete and even partly inconsistent in respect to what can be defined as commands,
  parameters, etc. and what action can be "automated" with the information provided.

This means, that using the module provides basic tools and probably <em>a guideline</em> on how to
implement the CLI processing, but not a full-featured CLI processor that just needs a grammar
definition input and some callback functions.<br>

This sort of "incompleteness" can also positively phrased: The implementable grammar and syntax of
the CLI interface is open for implementing any weird interface and not limited to a certain scheme.

One major burden that this \alibmod may relieve a user from, is that CLI command names and other
syntax tokens, and probably more important the error messages and help texts that the CLI interface
produces, are duly externalized and thus can be easily translated into other human languages.
This is achieved by being a \alibcamp and thus leveraging inner namespace
#alib::resources of module \alib_resources.

\I{################################################################################################}
## 1.2 ALib Enum Records And ALib Resources # {#alib_cli_intro_enum_records}
The classes found in this namespace make a lot of use of
\ref alib_enums_records "ALib Enum Records" retrieved from an instance of type
\alib{resources;ResourcePool}, usually the one found in the singleton of the associated \alib{camp;Camp}.

Therefore, it is good advice to get familiar with the features provided by inner namespace
#alib::resources of module \alib_resources as a start. That module in turn uses so-called
\ref alib_enums_records "Resourced ALib Enum Records" which are provided with the module 
\alib_enumrecords.

Or other way round: The tutorial found below might also be a good starter to understand
\ref alib_mod_resources "externalized resources" and \alib_enumrecords_nl in their basics, and 
therefore the better advice might be to just continue reading.

\I{################################################################################################}
## 1.3 Features # {#alib_cli_intro_features}
Module \alib_cli_nl offers the following features:

- Rather simple interface through central class \alib{cli;CommandLine}.
- Resourced and optionally localized command names, output and help texts
- If commands or options expect mandatory arguments, such arguments may be read
  automatically and attached to the command objects as additional information.<br>
  Optional arguments might also be read automatically, depending on their specification.
- Support for general help output (e.g., with <em>--help</em> ) as well as
  help output on certain topics (e.g., with <em>--help commandXYZ</em>).
- CLI input - or parts of it that have not been "consumed" -  may be passed forward into other
  parts and libraries embedded in an application.<br>
  For example, \alib module \alib_variables allows overwriting configuration data stored in
  configuration files transparently with by giving command line options. CLI input which is not
  processed by \alib_cli_nl, is filtered out and can easily be forwarded to module \alib_variables,
  or other parts of the software that accepts CLI input.
- ALib \alib{exceptions;Exception;Exception Handling} with support for automatic "translation" of
  C++ exceptions to a (usually) more narrow set of application exit codes.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 2. Classes and Paradigms # {#alib_cli_candp}

## 2.1 Defining Commands, Parameters, Options and ExitCodes ## {#alib_cli_candp_defines}

As a preparation to parsing command line arguments, instances of the following structs are to be
created and duly filled with data:

1. \alib{cli;CommandDecl}
2. \alib{cli;ParameterDecl}
3. \alib{cli;OptionDecl}
4. \alib{cli;ExitCodeDecl}

Please use the links above and quickly read the general descriptions of the types.

Such struct definition is considered static data, and as such are usually stored in so-called
resources. How resourced data is used to automatically create and fill the structs is shown
in the next chapter \ref alib_cli_tut "3. Tutorial Sample".

All enums which are using resourced \ref alib_enums_records "ALib Enum Records" are demanding a
specific record type to be associated to a custom enumeration.

In addition, for such custom enum types, a specialization of \alib{resources;ResourcedTraits},
has to be provided, which is used to load further resource strings.

The following table summarizes the types, the associated <em>ALib Enum Record</em> type and
the additional resource strings that need to be available:

|Object Type               | Record Type To Use        | Additional Resource Strings
|--------------------------|---------------------------|-----------------------------------------------------
|\alib{cli;CommandDecl}    | \alib{cli;ERCommandDecl}  |  <b>"THlpCmdSht_NN"</b>: Short version of help text for each parameter.<br><b>"THlpCmdLng_NN"</b>: Long version of help text for each parameter
|\alib{cli;ParameterDecl}  | \alib{cli;ERParameterDecl}| <b>"THlpParSht_NN"</b>: Short version of help text for each parameter.<br><b>"THlpParLng_NN"</b>: Long version of help text for each parameter.
|\alib{cli;OptionDecl}     | \alib{cli;EROptionDecl}   | <b>"TOptUsg_NN"</b>: Help text for each option describing its usage.<br><b>"TOptHlp_NN"</b>: Help text for each Option.
|\alib{cli;ExitCodeDecl}   | \alib{cli;ERExitCodeDecl} | <b>"TExit_NN"</b>: A format string returned by method \alib{cli;ExitCodeDecl::FormatString}.

## 2.2 Main Class 'CommandLine' ## {#alib_cli_candp_cmdline}
With the above resourced definitions in place, class \b CommandLine is used to
bootstrap all enum records to be ready for parsing the command line.

To avoid cluttering this types' interface, various helper methods have been put into
friend class \b CLIUtil.

See the reference documentation of types \alib{cli;CommandLine} and \alib{cli;CLIUtil} for details.

Alternatively, just follow the tutorial provided in the next chapter.

\I{################################################################################################}
\I{###############################     3. Tutorial      ###########################################}
\I{################################################################################################}
# 3. Tutorial Sample # {#alib_cli_tut}
Due to the usage of \ref alib_mod_resources "resourced strings", and
even more due to the usage of \ref alib_enums_records "Resourced ALib Enum Records", to
start a new project using the command line facilities of this module becomes more complex than
one might think.<br>
The best way out is the provision of a "template" project, which can be used for jump starting
into an own development.

Such template is provided with this tutorial chapter!
\note The full source code of the sample can be seen (and copied!) here:
      \alibfile{src.samples/CLI/sample.cpp}.<br>
      Because the documentation tool used here (overall exquisite software \https{Doxygen,doxygen.nl})
      destroys some of the formatting, it is recommended to copy the source directly from
      the source file located at:

            ALIB_BASE_DIR/src.samples/CLI/sample.cpp
\note Furthermore, it is recommended to split it's contents into proper header and source files.

The sample application is a little similar to the GNU/Linux "date" command. Its functionality
is described as follows:
- Without any argument, the current date and time should be written (to the standard output stream).
- Alternatively, if argument "now" is given, the same should be performed.
- To receive the modification date of a file or directory, the command \e 'file' has to be
  given along with a non-optional next argument containing the filename.
- Option \e '--format' allows modifying the date format of the output. The format string has
  to be attached to the option using equal sign \e '=' and may (must) be quoted either with
  single \e '\'' or double marks \e "\"".
- A help output shall be available with either a command \e "help" or option \e "--help"
  In the first case an optional help topic may be added separated by a whitespace, in the
  option case, such parameter shall be added using equal sign \e '=' (without whitespaces).
- Finally, more than one command should be able to be passed, for example:
        date now file /etc
  which would first print the current date and time and in a second row the modification date
  of directory \e "/etc".

With this project scope in mind, let's start coding.

## 3.1 Needed includes # {#alib_cli_tut_includes}
The following includes are necessary for this sample.
\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_INCLUDES

## 3.2 Definition of Enumerations  # {#alib_cli_tut_enums}
Commands, options, parameters and exit codes of our software are easily defined with
simple C++ enumerations:

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_ENUMS

The little magic comes with assigning \ref alib_enums_records "ALib Enum Records" to these
enumerations using the helper macros. For each of the enumerations, a corresponding record type
is defined with this module:

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_ENUMS_ASSIGN

This assignment, entitles the main class of this module to "know" how to parse the objects
from the command line.

## 3.3 Definition of a Custom Camp  # {#alib_cli_tut_module}
The enum records that were just assigned to the enums, need to be filled with values.
Such values can be automatically loaded from resources.
To define such resources, the easiest way is to create just another \alibcamp.
Of course using this term here does not mean that this will be a part of ALib. But ALib
exports its camp concept to the user and here we make use of this.

\note \alib_cli_nl does not force you to create a custom camp. Alternatively, the creation of
      an object of type \alib{resources;ResourcePool} would be enough.
      However, when you create your own more complex software, it might be a good advantage to have
      such camp in place from the start. A camp solves several problems, for example
      the often annoying C/C++ bootstrapping issues. And, as you will see, creating a module
      instead of a \b ResourcePool is not very complicated.

In this simple sample, all that this camp does is defining and parsing string data which
comprises the enum records plus some additional help and information strings.

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_CUSTOM_CAMP

\alibcamps_nl are singleton objects, and as such we create one global instance:

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_CUSTOM_CAMP_SINGLETON

If you only have a brief look at the string data defined in the code above, as an experienced
programmer, you should easily grasp the general idea of what sort of information is provided
with the resourced \ref alib_enums_records "ALib Enum Records" of this camp.

## 3.4 Assigning the Resources to the Enumerations  # {#alib_cli_tut_assign_resources}
Now, as the camp is defined, a second macro is used which tells \alib where to find the
resources for each enumeration:

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_ENUMS_ASSIGN2

## 3.4 Function main()  # {#alib_cli_tut_main}
Now, we are already set to implement function <em>main()</em>! <br>
To make this easier to read, we pull the command processing into a separate function, which is
forward-declared like this:

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_ENUMS_FWDDECL

With this extraction of the custom command processing itself,
function main becomes quite short and straightforward. Note the 7 steps commented in the source:

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_ENUMS_MAIN

Finally, we can look at our custom command processing function.
Also here, just note the source comments. Everything should be quite understandable:

\snippet "samples/CLI/sample.cpp"        DOX_EXPR_TUT_CLI_ENUMS_PROCESS

You might have noticed that this function uses static utility methods of class
\alib{cli;CLIUtil} to generate help texts for general use, or specifically for commands, options
and parameters.

## 3.5 Sample Invocations  # {#alib_cli_tut_invocations}
After compiling this sample to binary \b "Sample" it can be invoked from the command line.
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
\alib{strings::util;CalendarDateTime::Format}. The format string can be given concatenated to
the option name using an equal sign <em>'='</em> or just as a next argument (with a whitespace
instead of the equl sign):

        Sample --format="dd MMM YYYY"

Outputs:
\verbinclude "DOX_CLI_CMDNOW_FORMAT.txt"

<b> 4. Command 'file' </b><br>
The command <em>file</em> expects a next command line argument (separated by whitespaces), that
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
can be detected by the \alib_cli_nl internally and automatically.
Here, the library throws an \alib{exceptions;Exception}, which is "annotated" with further information
while it is passed through the unwinded execution stack.

Consequently, this sample should be extended to fetch and analyse the internal exceptions and
create a similar output as in the first case. We kept this here as it is to <b>a)</b> keep the sample
code simple and <b>b)</b> demonstrate the power of <b>ALib Exceptions</b> in respect to the generation
of human-readable, formatted error output.<br>
More complex projects that use this library should follow the approach to use and throw \alib exceptions
anywhere possible. Only in the very end (for example at the end of the main() method), such exceptions
should get "translated" to one one of the more general exit codes that a command line software usually
returns. The benefits: Error handling is then all in one place and internally it is all done nicely
with C++ exception handling.

A last erroneous input we want to test here is the provision of unknown commands or options:

        Sample unknown

outputs:
\verbinclude "DOX_CLI_UNKN_CMD.txt"

while

        Sample --unknown
shows:

\verbinclude "DOX_CLI_UNKN_OPT.txt"

<b> 6. Help texts  </b><br>
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
# 4. Detail Topics # {#alib_cli_details}

\I{################################################################################################}
## 4.1 Allowing Other Parts Of The Software To Access CLI Arguments ## {#alib_cli_detail_arg_sharing}
Independent of the use of this \alibmod_nl, the original command line strings are always
available through the global \alib variables \alib{ARG_C}, \alib{ARG_VN} and \alib{ARG_VW} (if set
by the user's <c>main()</c>-function properly).

Class \alib{cli;CommandLine} exposes field \alib{cli::CommandLine;OptionArgsIgnored},
that hold all CLI arguments that start with a hyphen <c>'-'</c> and are not recognized
by \alib{cli;OptionDecl}. software might either generate an error if unrecognized options
are left (just like the tutorial-sample of the previous chapter does), or pass this list
to other parts and libraries that software is using..

With the use of camp \alib_variables such use of CLI arguments outside this camp already
occurs: \alib{variables;Variable;Configuration variables} can be defined using CLI parameters
the same as by using environment variables, configuration files or other data sources.
Such use is still be possible, independent of the CLI interface.

The other way round, arguments may be removed from the internal list held by this class, prior
to having this class processing the arguments! Note that this is a must for  arguments that do not
start with a hyphen (and are addressing other software parts), because such arguments would
lead to "unknown command" errors.
Such removal has to be done after the invocations of the various \b %Define methods and prior
to invoking \alib{cli;CommandLine::ReadOptions}.

<br><br><br><br><br><br> */
