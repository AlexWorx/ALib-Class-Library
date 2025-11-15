// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_camp   ALib Module Camp - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1. Introduction # {#alib_camp_intro}

This module is the fundament for the subset of \alibmods that we call \alibcamps_nl.

\par Please Read Now:
    To avoid redundancy, we would ask you to read the short chapter
    \ref alib_manual_modules_camps of the Programmer's Manual of \alib, which gives an
    explanation to this statement.
                                       
The three key aspects mentioned in the link above — 1) managing externalized resources, 2) accessing
external configuration, and 3) defining a bootstrap process — are relevant
to most software, even to small command-line tools.
However, these areas are not well-supported by the C++ language and also the introduction of
C++20 Modules left these challenges unresolved.

This module addresses these issues by introducing the virtual interface \alib{camp;Camp}, which
is designed to handle these three concerns.
Other \alibmods_nl already integrate this interface, and custom code units can also derive one
or more types from it as needed.

In traditional application design, there is typically a centralized application class managing
these responsibilities. From a broader perspective, \alibcamps_nl offer an alternative by allowing
the delegation of these tasks to the specific code entities responsible for them.
Put simply, conventional software design often relies on each code unit being aware of a
central application class that provides the necessary interfaces.
This can lead to circular dependencies between the application class and the code units.
One of the primary goals of this module is to eliminate such design limitations.
   
 
\I{################################################################################################}
# 2. Class Camp # {#alib_camp_camp}
To implement the proposed functionality, class \alib{camp;Camp} relies on the aggregation
of functionality stemming from other \alibmods_nl.<br>
Let us look at them one by one:

## 2.1 Resource Management ##
For resource management, the class provides a pointer to an instance of the virtual class
\alib{resources;ResourcePool} defined in the module \alib_resources.
Derived types can use inherited methods
- \alib{camp;Camp::GetResource},
- \alib{camp;Camp::TryResource}, and
- \alib{camp;Camp::GetResourcePool}

to access externalized string resources conveniently.

## 2.2 Configuration Data ##
For external configuration data, the class provides a pointer to an instance of
class \alib{variables;Configuration}, defined in the module \alib_variables.
Derived types can retrieve this instance with method \alib{camp;Camp::GetConfig} and with that
get access to \alib run-time variables.

In this context, one core feature of \alib_variables_nl is that their values can transparently
emerge from
- command-line parameters,
- environment variables,
- configuration files, or
- any other external configuration system.

This gives access to such data at the place where it is needed.

Note that run-time variables can also be defined by other code entities to share information
between modules.


## 2.3 Bootstrapping and Shutdown ##
Implementing a well-defined bootstrap and shutdown process is the responsibility of the
overarching module \alib_bootstrap. The namespace functions #alib::Bootstrap and #alib::Shutdown
change their signature, depending on the inclusion of this module \alib_camp_nl in the \alibbuild.
Without the inclusion of any \alibcamp_nl, a heavily simplified bootstrap and shutdown process is
in place, which calls corresponding bootstrap- and shutdown-functions
of certain non-camp modules (among only a few other things).

The altered versions of the functions allow to either share or not share resources and configuration
data between the different \alibcamps_nl.
This is why the internal field members \alib{camp;Camp::resourcePool} and \alib{camp;Camp::config}
are implemented as pointer types.

Furthermore, the altered functions divide the processes into different phases. For each phase,
they invoke the abstract virtual methods \alib{camp::Camp;Bootstrap}, and
\alib{camp::Camp;Shutdown} which have to be implemented by derived classes.<br>

Finally, the bootstrap functions also make sure that the built-in \alibcamps_nl are initialized
in the right order, namely from lower-level camps to higher ones. The same applies for the
shutdown process, but in reverse order.

All information about
- how default bootstrapping works,
- how an application can alter this default to adjust the distribution of different configuration
  and resource management instances, and
- how a using code may inject own \alibcamps_nl into these processes,

is documented with the \ref alib_mod_bs  "Programmer's Manual of module ALib Bootstrap".

When developing custom camps, the Programmer's Manuals of the modules \alib_resources and
\alib_variables should be consulted as well.
A good jump-start to copy from could be the straightforward
\alibcppfile{filescamp;reference implementation} found in the \alib_files camp.
 
\I{################################################################################################}
# 3. The Basecamp # {#alib_camp_basecamp}
In the introduction to this manual, it was explained that the \alib_camp_nl module introduces
the \alib{camp;Camp} class.
Each module designed to be an \alibcamp must provide a singleton instance derived from this class.

Below is a summary of the currently available camps in \alib:

Module                         | Camp Class                          | Singleton Instance
-------------------------------|-------------------------------------|--------------------------
\ref alib_mod_alox "ALib ALox" | #alib::lox::ALoxCamp                | #alib::ALOX
\alib_camp                     | #alib::camp::Basecamp               | #alib::BASECAMP
\alib_cli                      | #alib::cli::CliCamp                 | #alib::CLI
\alib_expressions              | #alib::expressions::ExpressionsCamp | #alib::EXPRESSIONS
\alib_files                    | #alib::files::FilesCamp             | #alib::FILES
       
As shown in the table, this very module creates its own singleton instance with #alib::BASECAMP.
Consequently, it qualifies as an \alibcamp_nl itself.

A natural question might arise: why would a module like this, which mainly provides the interface
class \alib{camp;Camp}, require any extensive setup, resources, or configuration?

The answer lies in the way this module subtly enhances functionality. When included in the
\alibbuild, it replaces some lower-level resources, primarily related to
\ref alib_enums_records "ALib Enum Records".
Without this module, these records are usually hardcoded or just unavailable.
By incorporating this module, the build process dynamically constructs these records during the
module's bootstrap phase.
This approach takes advantage of the externalized resources managed by the \b Basecamp class,
doing away with the hard-coded implementation via preprocessor directives.<br>
This improvement applies not just to enum records but also to a variety of format strings and
exception-related messages (notably in the \alib_format and \alib_system modules).

Additionally, during bootstrap, the \b Basecamp class reads and processes external
\ref alib_cfgvars.

For those curious about the full range of tasks this module performs,
\alibcppfile{basecamp;the source code} of the \b Basecamp implementation serves as a useful
reference.

For developers looking to create custom camp-modules, it is recommended to begin with the much
simpler and more straightforward \alibcppfile{filescamp;reference implementation} found in
the \alib_files camp.

\I{################################################################################################}
# 4. Extensions For ALib Variables # {#alib_camp_variables}
The inclusion of this module with the header \implude{Camp}, injects various overloads
of the function \alib{variables;CampVariable} into the namespace #alib::variables of the module
\alib_variables.<br>
These functions are inline shortcuts used for creating variables associated with the
\alib{variables;Configuration} instance found in a \alib{camp;Camp}.

*/
