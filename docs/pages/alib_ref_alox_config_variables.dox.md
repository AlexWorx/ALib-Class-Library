// #################################################################################################
//  Documentation - ALox Logging Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/**
\page alib_alox_cfgvars    ALox Configuration Variables

## Description ##
Trough the use of camp module \alib_variables of the \aliblong, external configuration variables can be comfortably defined and accessed.

While this is a reference appendix chapter, for general information, please refer to:
- The \ref alib_mod_variables "Programmer's Manual" of module \alib_variables_nl.
- The \ref alib::variables "Reference Documentation" of module \alib_variables_nl.
- A quick sample how to attach an INI-file with minimum effort to an \alib enabled application
  is provided in chapter \ref alib_mod_alox_external_verbosity_configuration of the
  Programmer's Manual of module \alib_alox.

A few further configuration variables that \alib internally uses are defined by camp \alib{BASECAMP}.
The variables defined with that module are \ref alib_cfgvars "documented here".

<b>Default Behavior:</b><p>
\alox and the \aliblong are designed to run fine without setting configuration variables, because
all of them have \ref alib_resources_intro "resourced" default values.
Therefore, these variables provide an additional way to tweak \alox and change its default
behavior.

<b>Declaration:</b><p>
The variables listed here are resourced with camp \alib_alox_nl and declared with enum
class \alib{lox::Variables}.

\note Setting environment variables is platform-specific and is not part of this documentation. To
get an overview, refer to [Wikipedia](https://en.wikipedia.org/wiki/Environment_variable).<p>

## Variable Index ##
- \ref alxcvALOX_CODEPAGE
- \ref alxcvALOX_CONSOLE_TYPE
- \ref alxcvALOX_CONSOLE_LIGHT_COLORS
- \ref alxcvALOX_GLOBAL_SOURCE_PATH_TRIM_RULES
- \ref alxcvALOX_NO_IDE_LOGGER

- \ref alxcvALOX_LOXNAME_DOMAIN_SUBSTITUTION
- \ref alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT
- \ref alxcvALOX_LOXNAME_PREFIXES
- \ref alxcvALOX_LOXNAME_SOURCE_PATH_TRIM_RULES

- \ref alxcvALOX_LOGGERNAME_AUTO_SIZES
- \ref alxcvALOX_LOGGERNAME_FORMAT
- \ref alxcvALOX_LOGGERNAME_FORMAT_DATE_TIME
- \ref alxcvALOX_LOGGERNAME_FORMAT_TIME_DIFF
- \ref alxcvALOX_LOGGERNAME_FORMAT_MULTILINE
- \ref alxcvALOX_LOGGERNAME_FORMAT_OTHER
- \ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME
- \ref alxcvALOX_LOGGERNAME_REPLACEMENTS

\I{################################################################################################}
# Reference Documentation #

\I{################################################################################################}
## ALOX/CONSOLE_TYPE ## {#alxcvALOX_CONSOLE_TYPE}
<em>Type</em>: \c "S" (AStringPA)
<em>Default Value</em>: \c "default"<br>

Influences the type of console logger that will be created by method
\alib{lox;Lox::CreateConsoleLogger}, which is also used by \alib{lox;Log::AddDebugLogger}.

Possible values are:
<center>Value</center>   | <center>Effect</center>
- - - - - - - - - - - - -| - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Default    | No influence, same as if variable was not set (An OS-specific, colorful logger is chosen).
   Plain      | Plain text \e Logger of type \alib{lox::loggers;ConsoleLogger} is created.
   Ansi       | A \e Logger of type \alib{lox::loggers;AnsiLogger} is created.
   Windows    | A \e Logger of type \alib{lox::loggers;WindowsConsoleLogger} is created.


\I{################################################################################################}
## ALOX/CONSOLE_LIGHT_COLORS ## {#alxcvALOX_CONSOLE_LIGHT_COLORS}
<em>Type</em>: \c "ALOXCLP" (\alib{lox::textlogger;ColorfulLoggerParameters})
<em>Default Value</em>: \c "Auto"<br>

Evaluated by colorful loggers that dispose of light and dark colors to adjust their foreground
color accordingly. E.g., for instances of class \alib{lox::loggers;AnsiLogger}, field
\alib{lox::loggers::AnsiLogger;CFP} is set.

If not given, under Windows OS the right value is detected (here, the color of a terminal
is detectable). Otherwise the value defaults to be an empty string, which chooses a
programmatically evaluated (internal) default value. In some occasions the (detected or set) runtime
environment might influence such default value.

Value options are:
- \b "Foreground": Sets the foreground color to a light color. This is recommended if for consoles
  with dark background.
- \b "Background": Sets the background color to a light color. This is recommended if for consoles
  with a light background.
- \b "Never": Sets foreground and background color to the same "brightness". This is recommended
  solely for systems that do not support light colors.


\I{################################################################################################}
## ALOX/NO_IDE_LOGGER ## {#alxcvALOX_NO_IDE_LOGGER}
<em>Type</em>: \c "B" (boolean)
<em>Default Value</em>: \c "false"<br>

Boolean variable that is evaluated in method \alib{lox;Log::AddDebugLogger}.
If not set, when debugging, an additional \e Logger supporting the log output of the IDE is created.
The only logger currently available (needed) is with WindowsOS / VisualStudio, class
\alib{lox::loggers;VStudioLogger}.

This variable can be used to suppress this.


\I{################################################################################################}
## ALOX/CODEPAGE ## {#alxcvALOX_CODEPAGE}
<em>Type</em>: \c "I" (integral)
<em>Default Value</em>: \c 65001<br>
\e Availability: On target platform WindowsOS only.<br>

Evaluated once in the constructor of class \alib{lox::loggers;WindowsConsoleLogger}.
Overwrites the default value of field \alib{lox::loggers::WindowsConsoleLogger;CodePage}.
For possible values refer to
[Microsoft documentation] (https://msdn.microsoft.com/EN-US/library/windows/desktop/dd317756%28v=vs.85%29.aspx).


\I{################################################################################################}
## ALOX/LOGGERNAME/AUTO_SIZES ## {#alxcvALOX_LOGGERNAME_AUTO_SIZES}
<em>Type</em>: \c "ALOXAS" (\alib{lox::textlogger;FormatAutoSizes})
<em>Default Value</em>: none.<br>

Format: <c> ALOX/\<LOGGERNAME\>/AUTO_SIZES = [!] [ T|F Actual[,session] ]   [/ T|F Actual[,session] ]   [; [!] [ T|F Actual[,session] ]   [/ T|F Actual[,session] ] ] </c>

While the format definition above looks rather complicated, in fact it is just two optional
\alib{strings::util;AutoSizes::Export;exports auf the contained AutoSizes objects}, separated
 by a semicolon <c>';'</c>.

This variable is used by \e Loggers derived from abstract type \alib{lox::textlogger;TextLogger}.
It is read when a \b %TextLogger is registered with a \b %Lox.<br>
This mechanism maintains automatically adjusted tabulator positions in the generated meta
information across different runs of software.

Note that the portion <c>'LOGGERNAME'</c> in the variable name is replaced by \alox with the value of
\alib{lox::detail;Logger::GetName}. This allows having different sizes for different \e %Loggers.

Consider this variable a 'cookie' that \alox uses to store tabulator widths which automatically
increase as needed, across sessions.

\note Being a pure convenience feature that optimizes the log output slightly, this variable is
      considered a "session variable" in contrast to a real "configuration variable".
      Consequently, the variable is defined with \alib{variables;Priority;Priority::Session} when written by
      a \b TextLogger.
      More information on this difference and how to best store this variable is given
      with chapter \ref alib_variables_external_session of the Programmer's Manual of module
      \alib_variables_nl.

The value of the variable can be retrieved with \alib{lox::textlogger;TextLogger::GetAutoSizes}.
If a certain set auf sizes is to be used and never changed, then flags
\alib{strings::util;AutoSizes::WriteProtected} can be set in one or both \b AutoSizes instances of
this variable. The flag will also be set by the logger, if the configuration variable becomes
defined with a priority higher than \alib{variables;Priority;Priority::Session}.

\see
- Reference documentation of this \alib{lox::textlogger;FormatAutoSizes;variable's type}
  for further information.
- Chapter \ref alib_mod_alox_apdx_auto_config_list_logables_textlogger.


\I{################################################################################################}
## ALOX/LOGGERNAME/FORMAT ## {#alxcvALOX_LOGGERNAME_FORMAT}
<em>Type</em>: \c "ALOXFMI" (\alib{lox::textlogger;FormatMetaInfo})
Default Value: <c>"%SF:%SL:%A3%SM %A3[%TC +%TL][%tN][%D]%A1#%# %V: ,[ERR],[WRN],     ,[***]"</c>.

This variable is used by \e Loggers derived of type \alib{lox;textlogger::TextLogger}.<br>
It is read when a \e Logger of that type is added to a \b %Lox and if found, fields
- \alib{lox::textlogger::FormatMetaInfo;Format},
- \alib{lox::textlogger::FormatMetaInfo;VerbosityError},
- \alib{lox::textlogger::FormatMetaInfo;VerbosityWarning},
- \alib{lox::textlogger::FormatMetaInfo;VerbosityInfo},
- \alib{lox::textlogger::FormatMetaInfo;VerbosityVerbose} and
- \alib{lox::textlogger::FormatMetaInfo;MsgSuffix}

of struct \alib{lox::textlogger;FormatMetaInfo} are set according to the variables' content.
The contents of the variable is specified as follows:

Format: <c> ALOX/\<LOGGERNAME\>/FORMAT = ["]Format["] [, ["]Error["] [, ["]Warning["] [, ["]Info["] [, ["]Verbose["] ] [, ["]MsgSuffix["] ]]]]] </c>

Values that start or end with whitespace characters or which contain comma characters (<c>,</c>) ,
need to be enclosed by quotation marks (<c>\"</c>).

Note that the portion <c>'LOGGERNAME'</c> in the variable name is replaced by \alox as follows:
- \alib{lox::detail;Logger::GetName} of the \b Logger constructed is used.
- If no configuration setting is found for that name, the value of
  \alib{lox::detail;Logger::GetTypeName} is used.

This allows configuring either a single \e %Logger or a whole class of \e %Loggers.

As a sample, configuration variable \b ALOX/ANSI_CONSOLE_LOGGER/FORMAT, affects all loggers of type
\alib{lox::loggers;AnsiConsoleLogger} as long as no specific name is set when such \e %Logger is
constructed and no separate configuration for that name is set.

Note that the default debug \e %Logger which is automatically created if no \e %Logger is
attached to the debug \b %Lox singleton, is named \c "DEBUG_LOGGER". Therefore, by setting variable
\c ALOX/DEBUG_LOGGER/FORMAT, this \e Logger is affected.
Furthermore, with debugging, a second debug-\e Logger for the IDE might be created (depending on
the platform and IDE used). Its' name is \c "IDE_LOGGER".


\I{################################################################################################}
## ALOX/LOGGERNAME/FORMAT_DATE_TIME ## {#alxcvALOX_LOGGERNAME_FORMAT_DATE_TIME}
<em>Type</em>: \c "ALOXFDT" (\alib{lox::textlogger;FormatDateTime})
Default Value: <c>"yyyy-MM-dd,HH:mm:ss, Days "</c>.

This variable is used and applied in the same way as \ref alxcvALOX_LOGGERNAME_FORMAT and
the fields of struct \alib{lox::textlogger;FormatDateTime}:

- \alib{lox::textlogger::FormatDateTime;Date},
- \alib{lox::textlogger::FormatDateTime;TimeOfDay} and
- \alib{lox::textlogger::FormatDateTime;ElapsedDays}.


\I{################################################################################################}
## ALOX/LOGGERNAME/FORMAT_TIME_DIFF ## {#alxcvALOX_LOGGERNAME_FORMAT_TIME_DIFF}
<em>Type</em>: \c "ALOXFTD" (\alib{lox::textlogger;FormatTimeDiff})
Default Value: <c>"1000,---   , ns, \\u00B5s, ms, s, m, h, days"</c>.

This variable is used and applied in the same way as \ref alxcvALOX_LOGGERNAME_FORMAT and
the fields of struct \alib{lox::textlogger;FormatTimeDiff}:

- \alib{lox::textlogger::FormatTimeDiff;Minimum},
- \alib{lox::textlogger::FormatTimeDiff;None},
- \alib{lox::textlogger::FormatTimeDiff;Nanos},
- \alib{lox::textlogger::FormatTimeDiff;Micros},
- \alib{lox::textlogger::FormatTimeDiff;Millis},
- \alib{lox::textlogger::FormatTimeDiff;Secs},
- \alib{lox::textlogger::FormatTimeDiff;Mins},
- \alib{lox::textlogger::FormatTimeDiff;Hours}, and
- \alib{lox::textlogger::FormatTimeDiff;Days}.


\I{################################################################################################}
## ALOX/LOGGERNAME/FORMAT_MULTILINE ## {#alxcvALOX_LOGGERNAME_FORMAT_MULTILINE}
<em>Type</em>: \c "ALOXFML" (\alib{lox::textlogger;FormatMultiLine})
Default Value: <c>"2,ALox: Multi line message follows: ,> ,,nulled,\\r"</c>.

This variable is used and applied in the same way as \ref alxcvALOX_LOGGERNAME_FORMAT and
the fields of struct \alib{lox::textlogger;FormatMultiLine}:

- \alib{lox::textlogger::FormatMultiLine;Mode},
- \alib{lox::textlogger::FormatMultiLine;Delimiter},
- \alib{lox::textlogger::FormatMultiLine;Headline},
- \alib{lox::textlogger::FormatMultiLine;Prefix},
- \alib{lox::textlogger::FormatMultiLine;Suffix},
- \alib{lox::textlogger::FormatMultiLine;DelimiterReplacement},

\I{################################################################################################}
## ALOX/LOGGERNAME/FORMAT_OTHER ## {#alxcvALOX_LOGGERNAME_FORMAT_OTHER}
<em>Type</em>: \c "ALOXFO" (\alib{lox::textlogger;FormatOther})
<em>Default Value</em>: <c>"---, ---, 3"</c>.

This variable is used and applied in the same way as \ref alxcvALOX_LOGGERNAME_FORMAT and
defines the fields of struct \alib{lox::textlogger;FormatOther}:

- \alib{lox::textlogger::FormatOther;NoSourceFileInfo},
- \alib{lox::textlogger::FormatOther;NoMethodInfo},
- \alib{lox::textlogger::FormatOther;LogNumberMinDigits},

\I{################################################################################################}
## ALOX/LOGGERNAME/VERBOSITY_WITH_LOXNAME ## {#alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME}
<em>Type</em>: \c "SV;" (\alib{variables;StringVectorSemicolon})
<em>Default Value</em>: none.<br>

This variable is used to set \e Verbosities for a \e %Logger attached to an instance of class <b>%Lox</b>.
The portions <c>'LOGGERNAME'</c> and <c>'LOXNAME'</c> of the variable name have to be replaced by
the names of the instances of \b %Lox and \b %Logger in question.

Format: <c> ALOX/\<LOGGERNAME\>/VERBOSITY_WITH_\<LOXNAME\> = [ExportAll ;]  [*]domainpath[*] = verbosity [ ; &hellip; ] </c>

Details are described in chapter \ref alib_mod_alox_external_verbosity_configuration_variable
of the Programmer's Manual.

\I{################################################################################################}
## ALOX/LOGGERNAME/REPLACEMENTS ## {#alxcvALOX_LOGGERNAME_REPLACEMENTS}
<em>Type</em>: \c "S" (AStringPA)
Format: <c> ALOX/\<LOGGERNAME\>/REPLACEMENTSFORMAT = , ["]search["]=["]replace["] [, ...]</c>

Note that the portion <c>'LOGGERNAME'</c> in the variable name is replaced as documented in
\ref alxcvALOX_LOGGERNAME_FORMAT.

The variable allows setting pairs of search and replace strings for a text logger. Pairs found here,
are set using method \alib{lox::textlogger;TextLogger::SetReplacement}.

Values that start or end with whitespace characters or which contain comma characters (<c>,</c>),
need to be enclosed by quotation marks (<c>\"</c>).


\I{################################################################################################}
## ALOX/LOXNAME/DOMAIN_SUBSTITUTION ## {#alxcvALOX_LOXNAME_DOMAIN_SUBSTITUTION}
<em>Type</em>: \c "SV;" (\alib{variables;StringVectorSemicolon})
<em>Default Value</em>: none.<br>
This variable is used to define <em>Domain Substitution Rules</em> for a \b %Lox.
The portion <c>'LOXNAME'</c> of the variable name has to be replaced by the name of the instance
of class \b %Lox in question.

Format: <c> ALOX/\<LOXNAME\>/DOMAIN_SUBSTITUTION = [*]domainpath[*] -> replacement [ ; &hellip; ] </c>

Details are described in \ref alib_mod_alox_domain_substitution_config "Substitution Rules and External Configuration".


\I{################################################################################################}
## ALOX/GLOBAL_SOURCE_PATH_TRIM_RULES ## {#alxcvALOX_GLOBAL_SOURCE_PATH_TRIM_RULES}
<em>Type</em>: \c "S" (AStringPA)
<em>Default Value</em>: \c "" (empty string).<br>

This variable is used to define global <em>Source Path Trim Rules</em>.

Format: <c> ALOX/GLOBAL_SOURCE_PATH_TRIM_RULES = [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] [ ; &hellip; ] </c>

Details are described in \ref alib_mod_alox_trim_source_path_set_config "External Configuration of Trim Rules".


\I{################################################################################################}
## ALOX/LOXNAME/SOURCE_PATH_TRIM_RULES ## {#alxcvALOX_LOXNAME_SOURCE_PATH_TRIM_RULES}
<em>Type</em>: \c "S" (AStringPA)
<em>Default Value</em>: \c "" (empty string).<br>

This variable is used to define <b>%Lox</b>-specific <em>Source Path Trim Rules</em>.
The portion <c>'LOXNAME'</c> of the variable name has to be replaced by the name of the instance of
class \b %Lox in question.

Format: <c> ALOX/\<LOXNAME\>/SOURCE_PATH_TRIM_RULES = [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] [ ; &hellip; ] </c>

Details are described in \ref alib_mod_alox_trim_source_path_set_config "External Configuration of Trim Rules".


\I{################################################################################################}
## ALOX/LOXNAME/PREFIXES ## {#alxcvALOX_LOXNAME_PREFIXES}
<em>Type</em>: \c "S" (AStringPA)
<em>Default Value</em>: \c "" (empty string).<br>

This variable is used to define <b>%Lox</b> and <em>Log Domain</em> specific <em>Prefix Logables</em>
(strings only). The portion <c>'LOXNAME'</c> of the variable name has to be replaced by
the name of the instance of class \b %Lox in question.

Format: <c> ALOX/\<LOXNAME\>/PREFIXES = [*]domainpath[*] = prefixstring [, inclusion] [ ; &hellip; ] </c>

The variable value may contain more than one definition, separated by semicolons \b ';'.
Each definition contains of the following parameters:
- \p{domainpath}<br>
  An absolute or relative domain path. When using the optional asterisk characters \b '*',
  a start of a domain path, the end of a domain path or an arbitrary substring of a domain path can
  also be denoted. This way, more than one <em>Log Domain</em> might be affected.
- \p{prefixstring}<br>
  The <em>Prefix Logable</em> to set. The string is trimmed from whitespaces unless surrounded
  by quotation marks, \b \". Outer quotation marks are trimmed as well.
- \p{inclusion}<br>
  Can evaluate to the terms <em>Include</em> or <em>Exclude</em>. Can be arbitrarily abbreviated
  (i.e. \b "E" is enough) and is read in case insensitive way. The meaning is the same as parameter
  \p{otherPLs} of method
  \alib{lox;Lox::SetPrefix( const Box&, const NString&, lang::Inclusion otherPLs= lang::Inclusion::Include);Lox::SetPrefix}.

\I{################################################################################################}
## ALOX/LOXNAME/DUMP_STATE_ON_EXIT ## {#alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT}
<em>Type</em>: \c "S" (AStringPA)
<em>Default Value</em>: \c "none, verbosity=info, domain=/ALOX" (empty string).<br>

Used for debugging or for investigation into ALox enabled 3rd party libraries or applications.

\note If you want to prevent such investigation for your software, refer to the documentation of
      module \alib_variables to learn how to protect external variables from being set.

Format: <c> ALOX/\<LOXNAME\>/DUMP_STATE_ON_EXIT = arg1 [, arg2][, &hellip; ] </c>

The portion <c>'LOXNAME'</c> of the variable name has to be replaced by
the name of the instance of class \b %Lox in question.

If this variable is defined, class Lox will automatically invoke method \alib{lox;Lox::State}
at the moment a first \e Logger is removed. In most cases this would be the at the end of the process.
(The other cases are: If after a removal of a \e Logger, the same or another \e Logger is added,
then the next removal of a \e Logger will again invoke the method.)

There are two types of arguments possible which can appear in arbitrary order. All argument
identifiers are letter case agnostic.
<br>
<p>
<b>1. State Information Flags:</b><br>
See reference documentation of
\alib{lox;Lox::State} for more information. The identifiers expected in this variables' arguments
are:
ww
<center>Identifier</center>   | <center>Description</center>
- - - - - - - - - - - - -| - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 \c None                 | Selects nothing. This identifier overrules all others. If found, nothing is done.
 \c All                  | Selects everything
 \c Basic                | Name of the %Lox and number of log calls
 \c Version              | Library version and thread-safeness
 \c Loggers              | Loggers
 \c Domains              | Log domains currently registered
 \c Internaldomains      | Internal domains
 \c Scopedomains         | Scope domains
 \c DSR                  | Domain substitution rules
 \c PrefixLogables       | Prefix logables
 \c Once                 | Log once counters
 \c Logdata              | Log data objects
 \c Threadmappings       | Named threads
 \c SPTR                 | Source path trim rules (<b>C++ and C# only</b>)
 \c Compilationflags     | \alib compilation flags (<b>C++ only</b>)

<br>
<p>
<b>2. Attributes:</b><br>
Two attributes that are affecting the invocation of \alib{lox;Lox::State} can be provided as arguments:

<center>Attribute</center>       | <center>Description</center>
- - - - - - - - - - - - - - - - -| - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 \c domain \c = \b DOMAIN_PATH   | Sets a domain path for the \b %Log.State invocation (defaults to \c "").
 \c verbosity \c = \b VERBOSITY  | Sets a verbosity for the \b %Log.State invocation (defaults to \c "Info").

As a sample, take the following external variable:

        ALOX/LOG_DUMP_STATE_ON_EXIT= domain=/DEBUG, verbosity = info, logdata, scopedomains

When the process terminates (precisely the first \e Logger is removed from the standard debug
\b %Lox instance), then method \alib{lox;Lox::State} is invoked, with
- absolute domain \b '/DEBUG' provided for parameter \p{domain},
- value \c %Verbosity.Info for parameter \p{verbosity} and
- parameter \p{flags} set to <c>( LogData & ScopeDomains )</c>

*/
