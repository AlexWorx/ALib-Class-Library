// #################################################################################################
//  Documentation - ALox Logging Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/**
\page alib_cfgvars    ALib Configuration Variables
## Description ##
Trough the use of class \alib{variables;Configuration} defined in module \alib_variables of the
\aliblong, external configuration variables can be comfortably defined and accessed.

The variables documented here are defined and used with module \alib_camp and provide
some small tools for general use.

Configuration data can be provided in configuration files, by setting environment variables,
providing command line parameters or by any custom mechanism.

While this is a reference appendix chapter, for general information, please refer to:
- The \ref alib_mod_variables "Programmer's Manual" of module \alib_variables_nl.
- The \ref alib::variables "Reference Documentation" of module \alib_variables_nl.
- A quick sample how to attach an INI-file with minimum effort to an \alib enabled application
  is provided in chapter \ref alib_mod_alox_external_verbosity_configuration of the
  Programmer's Manual of module \alib_alox.

\note Setting environment variables is platform-specific and is not part of this documentation.
      To get an overview, refer to \https{Wikipedia,en.wikipedia.org/wiki/Environment_variable}.<p>
                                 
This page lists the few variables defined with module \alib_camp.
The variables are resourced with this module and declared with enum class \alib{camp::Variables}.


\I{################################################################################################}
## Variable Index ##

- \ref alibcvALIB_LOCALE
- \ref alibcvALIB_HAS_CONSOLE_WINDOW
- \ref alibcvALIB_WAIT_FOR_KEY_PRESS

\attention <b>The variables defined with ALox \ref alib_alox_cfgvars "are documented here"!</b>


\I{################################################################################################}
# Reference Documentation #

\I{################################################################################################}
## ALIB/LOCALE ## {#alibcvALIB_LOCALE}
<em>Default Value</em>: \c "" (empty string)<br>

This boolean variable is evaluated within \ref alib_mod_bs "bootstrapping" camp
\alib{BASECAMP}.
The behavior is as follows:
- if not set, the environment variables <em>LANG</em> and <em>LANGUAGE</em> are evaluated and
  if set, their value is passed to glibc method <em>setlocale()</em>.
- if set to "none", the invocation of setlocale() is suppressed.
- if set to anything different than "none", the value is passed to glibc method <em>setlocale()</em>.

The right definition of the locale is important, when using methods of module \alib_strings for
converting multi-byte character strings to wide character strings. Furthermore, default values
for \ref alib_mod_format "formatting numbers" and other types are influenced by this variable.


\I{################################################################################################}
## ALIB/HAS_CONSOLE_WINDOW ## {#alibcvALIB_HAS_CONSOLE_WINDOW}
<em>Default Value</em>: \c "" (empty string)<br>

This boolean configuration variable overwrites the value determined in method
\alib{camp;Basecamp::HasConsoleWindow}.


\I{################################################################################################}
## ALIB/WAIT_FOR_KEY_PRESS ## {#alibcvALIB_WAIT_FOR_KEY_PRESS}
<em>Default Value</em>: \c "" (empty string)<br>
This boolean variable is evaluated within \ref alib_mod_bs "bootstrapping" camp
\alib{BASECAMP}.

If set, it overrules the default behavior of detecting an appropriate value for field
\alib{camp;Basecamp::WaitForKeyPressOnTermination}.

*/
