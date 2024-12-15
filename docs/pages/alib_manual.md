// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_manual    ALib Programmer's Manual

\tableofcontents


\I{################################################################################################}
# 1. Introduction # {#alib_manual_intro}

Welcome the Programmer's Manual of the  <b>%ALib C++ Class Library</b>!

This manual explains the structure and concepts of the library and it's general use.
The library is divided into <b>"modules"</b> which together comprise the actual library
functionality. Please note, that for each of these modules a dedicated
\ref alib_manual_modules_overview "Programmer's Manual" exists,
which solely address a module's field of application.

The module's entities reside in inner namespaces of the libraries root namespace \ref alib,
which consequently is rather quite empty - apart from tons of <em>type aliases</em>.
(The rationale for these aliases is \ref alib_nsalib_type_aliases "explained here".)
The majority of the remaining code in that namespace is related to organize the
modules.

It becomes quite obvious that, before talking any further about this library, all about
<b>%ALib Modules</b> has to be explained.


\I{################################################################################################}
# 2. ALib Modules # {#alib_manual_modules}

Being a "general purpose" C++ library, quite different areas of programming are addressed by \alib.
This is why the library code and documentation is organized into different "modules".

\note
  The term \e module relates to the planned future C++ language feature with the same name.
  As of today C++ modularization is not technically used with \alib, yet.
  Experimental support for implementing \alibmods_nl as C++ modules is planned for
  \checkpromise{one of the next release, probably in 2025}. It is expected that \alibmods_nl
  can be 100% translated into true C++ 20 modules.

The rationals for organizing the library in modules are:

1. <b>Design Principle of Orthogonality</b><br>
   The modularization of the library is first of all a design principle, forcing library
   features to "fall into place" and make types and methods "orthogonal". (Orthogonal software
   can be considered as the opposite of "spaghetti code".)

2. <b>Comprehensible</b><br>
   Users of the library are able to grasp the overall feature set much faster due to the
   modularization. As an example, the
   \ref alib_expressions_intro_procon "features of module ALib Expressions", is currently more
   than 25 bullet points. A user not interested in run-time expression evaluation, will
   not be bothered with that, because the module has an own namespace, separated
   \ref alib::expressions "reference documentation" and an own
   \ref alib_mod_expressions "Programmer's Manual".

3. <b>Well Defined Library Bootstrapping</b><br>
   As explained in a later chapter, bootstrapping a software process written in C++ needs to
   obey to some certain rules which the programming language itself does not provide support
   for. \alibmods_nl tremendously helps organizing the library bootstrap process.

4. <b>Minimum Footprint</b><br>
   Modules can be compiled and used separately from others, as long as there is no dependency
   between modules.
   When using \https{CMake,cmake.org/} to build the library, all module-dependencies are
   automatically resolved: If a using application just needs a single module, that module is
   all that has to be specified with CMake. The provided scripts select the dependent modules and
   corresponding source files automatically.

5. <b>Build-Speed</b><br>
   The ability to just partly compile (and use) \alib also increases build speed. Not only that
   of the library itself, but also that of the user code (due to the selective header inclusion).


\I{################################################################################################}
## 2.1 Normal Modules vs. ALib Camps ## {#alib_manual_modules_twotypes}
\alib distinguishes between normal modules and so called \alibcamps_nl.
This term is explained in a \ref alib_manual_camp_modules "later chapter". For now, it is
sufficient to know that an \alibcamp_nl is a more "heavy-weighted" entity, compared to a
normal module.

\I{################################################################################################}
## 2.2 Overview Of Modules ## {#alib_manual_modules_overview}

### 2.2.1 Non-Camp Modules ###
The following table lists all non-Camp modules in alphabetical order:

Name <br>(Programmer's Manual)             |Namespace <br>(Reference Documentation)    | Description |
-------------------------------------------|-------------------------|-------------------------------
\ref alib_mod_bitbuffer     "BitBuffer"    | \ref alib::bitbuffer    |Provides types to read and write data into a stream of bits in a platform-independent manner. Furthermore different compression algorithms are given.
\ref alib_mod_boxing        "Boxing"       | \ref alib::boxing       |Implements <b>"auto-boxing"</b> for C++, known from higher level programming languages. Transparently wraps C++ values of arbitrary type together with run-time type information into very lightweight objects. In other words, this library provides <em>"type-safe void pointers which support virtual function calls"</em>, provides type-safe a variadic argument paradigm and much more.
\ref alib_mod_characters    "Characters"   | \ref alib::characters   |Mitigates C++ character type, character pointer and character array hell.
\ref alib_mods_contmono     "Containers"   | \ref alib::containers   |ALib container types. Furthermore tools to enable the containers of the C++ Standard Library to use \alib{lang;Allocator;alib allocators} are included.
\ref alib_mod_enums         "Enums"        | \ref alib::enums        |Extensions for otherwise limited C++ enum types.
\ref alib_mods_contmono     "Monomem"      | \ref alib::monomem      |Monotonic allocation utilities to avoid heap-memory usage. Together with the concepts introduced with module \alib_containers, the amount of potential use cases for monotonic allocation are tremendously extended. Note that the cost of heap allocation is largely underestimated by programmers.
\ref alib_mod_singletons    "Singletons"   | \ref alib::singletons   |Implements the singleton paradigm that overcomes the Windows OS DLL boundaries (caused by Window DLL's local data segments), while under other operating systems chooses a faster, straight-forward implementation.
\ref alib_mod_strings       "Strings"      | \ref alib::strings      |With a least intrusive design offers compatible, lightweight, secure, efficient, complete and convenient C++ string classes.
\ref alib_mod_threads       "Threads"      | \ref alib::threads      |Low-level thread primitives.
\ref alib_mod_threadmodel   "ThreadModel"  | \ref alib::threadmodel  |High-level multithreading tools.
\ref alib_mod_time          "Time"         | \ref alib::time         |A few useful tools for otherwise limited C++ scoped and non-scoped enum types.

### 2.2.2 ALib Camps ###
The following table lists all "ALib Camps" in alphabetical order.

Name <br>(Programmer's Manual)             |Namespace <br>(Reference Documentation)    | Description |
-------------------------------------------|---------------------------|-------------------------------
\ref alib_mod_alox          "ALox"         | \ref alib::lox            |A very unique and powerful approach to implement a software's debug and release logging facilities.
\ref alib_mod_basecamp      "BaseCamp"     | \ref  alib::lang::basecamp|Fundamental types related to the operating system, but also fundamental concepts for the C++ language, like the sophisticated standard \alib throwable type \alib{lang;Exception}, text formatting tools and other things that found no other home.
\ref alib_mod_cli           "CLI"          | \ref alib::cli            |Implements parsing and partly the processing of command line arguments (CLI stands for "command line interface"). In case of pure CLI software, provides tools for formatted and localized help and result output.
\ref alib_mod_config        "Configuration"| \ref alib::config         |Transparently encapsulates access to configuration or other input data coming from various sources, like command line parameters, environment variables, INI-files, 3rd-party configuration databases, etc. Support for persistent write-back of such data is given.
\ref alib_mod_expressions   "Expressions"  | \ref alib::expressions    |Expression parser, compiler and a virtual machine for evaluation of compiled expressions. Excels in respect to speed and flexibility and is very easy to adopt in own projects.
\ref alib_mod_files         "Files"        | \ref alib::files          |Directory and File scanner, optionally filtering with \alib_expressions_nl.

\I{################################################################################################}
## 2.3 Module Dependencies ## {#alib_manual_modules_dependencies}
### 2.3.1 Mandatory Dependencies ### {#alib_manual_modules_dependencies_mandatory}
One feature of \alib is that active support for the use of just a subset of its modules is
provided. Now, with the selection of a single module, all modules that the select one depends on,
have to be added to that specific reduced library built.
For example, module \alib_strings does not compile without the availability of module
\alib_characters.

The following diagram shows the mandatory dependencies between the modules:
\anchor alib_manual_modules_dependency_graph

\I{Comment: Sometimes, doxygen does not show the graph below. Then an empty graph helps. Dont know what this is!}

  \dot
  digraph forSomeReasonsDoxygenSometimesNeedsAnEmptyGraphHere_Strange {}
  \enddot

\dot
digraph ModuleDependencies
{
    newrank=true
    compound=true;
    fontsize="10";
    fontname="Open Sans";

    node [shape=plaintext, fontsize="10", fontname="Open Sans"]


    //--- ALib Camps ---
    BaseCamp       [label= BaseCamp        shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_basecamp"     ];
    ALox           [label= ALox            shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_alox"         ];
    CLI            [label= CLI             shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_cli"          ];
    Configuration  [label= Configuration   shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_config"       ];
    Expressions    [label= Expressions     shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_expressions"  ];
    Files          [label= Files           shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_files"        ];


    //--- dependencies camp -> camp
    ALox            ->  Configuration     [ color="midnightblue" ];
    Configuration   ->  BaseCamp          [ color="midnightblue" ];
    CLI             ->  BaseCamp          [ color="midnightblue" ];
    Expressions     ->  BaseCamp          [ color="midnightblue" ];
    Files           ->  BaseCamp          [ color="midnightblue" ];

    //--- Non-Camp Modules ---
    BitBuffer      [label= BitBuffer       shape= "box"        color="cornflowerblue"   URL="\ref alib_mod_bitbuffer"    ];
    Boxing         [label= Boxing          shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_boxing"       ];
    Characters     [label= Characters      shape= "box"        color="cornflowerblue"   URL="\ref alib_mod_characters"   ];
    Containers     [label= Containers      shape= "box"        color="cornflowerblue"   URL="\ref alib_mods_contmono"    ];
    Enums          [label= Enums           shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_enums"        ];
    Monomem        [label= Monomem         shape= "box"        color="cornflowerblue"   URL="\ref alib_mods_contmono"    ];
    Singletons     [label= Singletons      shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_singletons"   ];
    Strings        [label= Strings         shape= "box"        color="cornflowerblue"   URL="\ref alib_mod_strings"      ];
    ThreadModel    [label= ThreadModel     shape= "box"        color="cornflowerblue"   URL="\ref alib_mod_threads"      ];
    Threads        [label= Threads         shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_threads"      ];
    Time           [label= Time            shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_time"         ];

    subgraph sameRankE   {  rank="same";  Time; Characters; Containers ; Monomem; Singletons;  }

    //--- dependencies module -> module
    BitBuffer       -> Monomem       [ color="cornflowerblue"  ];
    BitBuffer       -> Containers    [ color="cornflowerblue"  ];
    BitBuffer       -> Enums         [ color="cornflowerblue"  ];
    Boxing          -> Characters    [ color="cornflowerblue"  ];
    Boxing          -> Singletons    [ color="cornflowerblue"  ];
    Enums           -> Singletons    [ color="cornflowerblue"  ];
    Enums           -> Strings       [ color="cornflowerblue"  ];
    Strings         -> Characters    [ color="cornflowerblue"  ];
    ThreadModel     -> Boxing        [ color="cornflowerblue"  ];
    ThreadModel     -> Containers    [ color="cornflowerblue"  ];
    ThreadModel     -> Enums         [ color="cornflowerblue"  ];
    ThreadModel     -> Monomem       [ color="cornflowerblue"  ];
    ThreadModel     -> Threads       [ color="cornflowerblue"  ];
    Threads         -> Strings       [ color="cornflowerblue"  ];
    Threads         -> Time          [ color="cornflowerblue"  ];

    //--- dependencies camp -> module
    BaseCamp        -> Boxing        [ color="cornflowerblue"  ];
    BaseCamp        -> Containers    [ color="cornflowerblue"  ];
    BaseCamp        -> Enums         [ color="cornflowerblue"  ];
    BaseCamp        -> Monomem       [ color="cornflowerblue"  ];
    BaseCamp        -> Strings       [ color="cornflowerblue"  ];
    BaseCamp        -> Time          [ color="cornflowerblue"  ];
}
\enddot

\note
  - The light blue modules show "normal" \alibmods_nl.
  - The dark blue modules show \alibcamps_nl.
  - The two small rectangles on the left side of some of the module boxes indicate that a
    module has initialization and termination functions that needs to be invoked with bootstrapping
    a process, respectively terminating it. As it can be seen (and will be discussed later)
    this is true for all \alibcamps_nl, but also for some of the normal ones.

\note
  The exact meaning of these differences will be discussed later in this manual.


\I{################################################################################################}
### 2.3.2 Optional Dependencies ### {#alib_manual_modules_dependencies_supporting}
Besides the mandatory dependencies shown in the graph above, also \b optional relations between the
modules exist. An optional relationship from module \b A to module \b B could be phrased as:<br>
<em>\"Module \b A provides more/extended functionality in case that module \b B is included in the
library built.\"</em>.

These relationships are documented with each module's Programmer's Manual.
Furthermore, within the reference documentation, you will find sentences like
<em>\"This feature is available only if the module <b>ALib XYZ</b> is included in the \alibdist_nl.\"</em>

The following diagram shows the optional dependencies:

\dot
digraph OptionalModuleDependencies
{
    compound=true;
    fontsize="10";
    fontname="Open Sans";

    node [shape=plaintext, fontsize="10", fontname="Open Sans"]


    //--- ALib Camps ---
    BaseCamp       [label= BaseCamp        shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_basecamp"     ];
    ALox           [label= ALox            shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_alox"         ];
    Configuration  [label= Configuration   shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_config"       ];
    CLI            [label= CLI             shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_cli"          ];
    Expressions    [label= Expressions     shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_expressions"  ];
    Files          [label= Files           shape= "component" style="bold" color="midnightblue"  URL="\ref alib_mod_files"        ];

//    subgraph sameRank1   {  rank="same";  CLI; Expressions; ALox;     }
//    subgraph sameRank1   {  rank="same";  Configuration; BaseCamp;      }

    //--- optional dependencies camp -> camp
    Expressions     ->  Configuration   [ color="midnightblue" ];
    Files           ->  Expressions     [ color="midnightblue" ];

    //--- Non-Camp Modules ---
    Boxing         [label= Boxing          shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_boxing"       ];
    Characters     [label= Characters      shape= "box"        color="cornflowerblue"   URL="\ref alib_mod_characters"   ];
    Containers     [label= Containers      shape= "box"        color="cornflowerblue"   URL="\ref alib_mods_contmono"    ];
    Enums          [label= Enums           shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_enums"        ];
    Monomem        [label= Monomem         shape= "box"        color="cornflowerblue"   URL="\ref alib_mods_contmono"    ];
    Singletons     [label= Singletons      shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_singletons"   ];
    Strings        [label= Strings         shape= "box"        color="cornflowerblue"   URL="\ref alib_mod_strings"      ];
    ThreadModel    [label= ThreadModel     shape= "box"        color="cornflowerblue"   URL="\ref alib_mod_threads"      ];
    Threads        [label= Threads         shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_threads"      ];
    Time           [label= Time            shape= "component"  color="cornflowerblue"   URL="\ref alib_mod_time"         ];

      subgraph sameRankA   {  rank="same";  Expressions; BaseCamp }
      subgraph sameRankB   {  rank="same";  ALox; Configuration ;CLI }
      subgraph sameRankE   {  rank="same";  Boxing;    Strings;                         }
      subgraph sameRankF   {  rank="same";  Time; Characters ; Containers ; Monomem; Singletons  }


    //--- optional dependencies headless -> headless
    BaseCamp        -> Threads       [ color="cornflowerblue"  ];
    BaseCamp        -> Configuration [ color="cornflowerblue"  ];

    Threads         -> Enums         [ color="cornflowerblue"  ];
    Threads         -> Monomem       [ color="cornflowerblue"  ];

    Strings         -> Threads       [ color="cornflowerblue"  ];
    Strings         -> Enums         [ color="cornflowerblue"  ];
    Strings         -> BaseCamp      [ color="cornflowerblue"  ];

    Boxing          -> Threads       [ color="cornflowerblue"  ];
    Boxing          -> Strings       [ color="cornflowerblue"  ];
    Boxing          -> BaseCamp      [ color="cornflowerblue"  ];
    Boxing          -> Monomem       [ color="cornflowerblue"  ];

    Enums           -> BaseCamp      [ color="cornflowerblue"  ];
    Enums           -> Strings       [ color="cornflowerblue"  ];

    Monomem         -> Threads       [ color="cornflowerblue"  ];
    Monomem         -> Strings       [ color="cornflowerblue"  ];

    Singletons      -> Threads       [ color="cornflowerblue"  ];
    Singletons      -> Monomem       [ color="cornflowerblue"  ];
    Singletons      -> Strings       [ color="cornflowerblue"  ];


    //--- optional dependencies camp -> normal
    ALox            -> Threads       [ color="cornflowerblue"  ];
    Configuration   -> Threads       [ color="cornflowerblue"  ];
    BaseCamp        -> Threads       [ color="cornflowerblue"  ];
    Expressions     -> Time          [ color="cornflowerblue"  ];
}
\enddot


\I{################################################################################################}
## 2.4 Impacts of Module Dependencies ## {#alib_manual_modules_impact}
If a user wishes to use and compile only a subset of \alib, the fact of having (selectable) modules
that are connected through a set of dependencies, has to be taken into account in different ways.

To denote a full or reduced version of \alib, the term \alibdist is used.

\I{################################################################################################}
### 2.4.1 Impact on Source Selection / Compilation ### {#alib_manual_modules_impact_sources}

The selection of \alib library source code to compile (either to a dedicated library file or directly
into a custom software unit) depends on the permutation of selected modules.
Now, as the modules have dependencies, all sources of all dependent modules have to be
(recursively) selected.

\alib comes with a set of \e CMake scripts that perform such selection. Even if \e CMake is not used
for the daily build process, the scripts may be used once to select the source and header files
needed to feed them into the build system in place.

All details about the build process are given in section \ref alib_manual_build "6. Building The Library".

\I{################################################################################################}
### 2.4.2 Impact on Header File Inclusion and Code Selection ### {#alib_manual_modules_impact_codeselection}

\alib header files are designed to be always includable, without the precondition to include other
files before. For example, to work with type \alib{strings;TAString;AString}, it is enough
to state

        #include "alib/strings/astring.hpp"

at the top of the compilation unit in question. Through the recursive header inclusion, the very
first header that is processed is always:

&nbsp;&nbsp;&nbsp;&nbsp;\alibheader{alib.hpp}

This header analyzes the given set of \ref GrpALibPreproSymbols "compiler symbols" which
define the modules included in a distribution. These symbols are named <b>ALIB_<em>MODULENAME</em></b>
and thus enumerate to:

- \ref ALIB_ALOX
- \ref ALIB_BOXING
- ...
- ...
- \ref ALIB_TIME

Each symbol that is not given will be defined as either \c 0 or \c 1, by resolving all dependencies
of those symbols given.

\I{################################################################################################}
### 2.4.3 Impact on Bootstrapping ### {#alib_manual_modules_impact_bootstrapping}

Selected modules have to be initialized in the right order. "Most independent" have to be
initialized first, dependent modules afterwards.
All information about bootstrapping the library is provided later in this manual, namely
in chapter \ref alib_manual_bootstrapping "4. Bootstrapping ALib".

\I{################################################################################################}
### 2.4.4 Single Threaded Library Compilation ### {#alib_manual_modules_impact_singlethreaded}
Excluding module \alib_threads from an \alibdist_nl has the special effect of
compiling \alib for single-threaded execution. Due to the fact that \alib uses class
\alib{threads;Lock} and sibling types from this module for all its <em>"mutex locks"</em>,
leaving out this module removes just all concurrency protection from <b>every remaining module</b>!

The advantage of this approach lies of course in performance and code size of single-threaded
compilations, because setting a mutex is a redundant action there. It is important to understand that
module \alib_threads_nl is <b>mandatory</b> if concurrent access to library functions
is performed.

In debug-compilations of the library, if module \alib_threads_nl is omitted, single-threaded
use is therefore asserted: An \alib assertion is raised when multi-threading is detected.

To detect multi-threading in debug-code, the underlying threading library (e.g., "pthreads" on
GNU/Linux) has to be provided with the build. For this reason, the default <b>CMake</b>-script,
which is described in later sections, does \b not depend on the inclusion of module
\alib_threads_nl, but just \b always searches the target system's thread library.
If found, symbol \ref ALIB_EXT_LIB_THREADS_AVAILABLE is passed to the C++ compiler.

The way to prevent searching and adding a thread library is by setting special <b>CMake</b> cached
variable \ref alib_manual_build_cmake_3 "ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH" to \c true.

In general it could be said:
\note
   In case of doubt, it is recommended to include module \alib_threads in an \alibdist_nl.

...unless a user knows exactly what she is doing!

\I{################################################################################################}
## 2.4.5 Common library files ## {#alib_manual_modules_common_files}
There are a few source files and corresponding types in \alib that are not associated with a module,
but rather belong to the library in general.
The source files are located in subfolder <c>alib/lang</c> - without its subfolders (!) - and the
corresponding C++ types are collected in the library's root namespace #alib::lang.

These files are not further organized and have to be included in the build process independent of
the selected modules.

\I{################################################################################################}
## 2.5  The Term "ALib Distribution" ## {#alib_manual_modules_dist}
Within this manual, the distinct manuals of \alibmods_nl and within the reference documentation,
the term <em><b>"ALib Distribution"</b></em> is frequently used. The term addresses the permutation of modules
that have been selected from the overall set of modules, at the time of compiling the library.
It might be in a user's interest to reduce \alib to a subset of its functionality and this way
speed up compilation and decrease library footprint.

The omission of single modules might have "side effects" on other modules in that respect, that
while they are still compilable, certain features might be dropped. If this is the case,
such dropping is always documented with the feature (function, type, method, member, etc.).

As a sample, \alib_expressions can be compiled with the absence of module \alib_config.
But in this case functionality for loading and automatically compiling nested expression
strings defined in INI-files, command-line arguments, etc. is dropped.

\I{################################################################################################}
## 2.6  Special Treatment For Modules ALox And Threads ## {#alib_manual_modules_special_alox}
A special treatment is granted to module \alib_alox, by having header file \alibheader{alox.hpp}
always included in any \alibdist. For this reason, the file is placed outside of
directory \alibsrcdir{alox}, and instead right inside the source's root folder \alibsrcdir.

This allows a user to place \alox log statements in a compilation unit without the need of testing
preprocessor symbol \ref ALIB_ALOX or the use of macros \ref IF_ALIB_ALOX and \ref IFNOT_ALIB_ALOX,
in the case that her sources should be ready to compile likewise in an \alibdist_nl that does
not include \alox.

Similar to this, macros \ref ALIB_LOCK and \ref ALIB_LOCK_WITH remain available in the absence
of module \alib_threads, as already discussed in previous section
\ref alib_manual_modules_impact_singlethreaded.


\I{################################################################################################}
\I{##########  3.  ALib Camps and Module BootCamp                                                }
\I{################################################################################################}
# 3. ALib Camps and special Module BootCamp # {#alib_manual_camp_modules}
After the previous chapter has given detailed information about how \alib is structured into
modules and that an \alibdist can be reduced to a subset of these modules, it is time to
talk about \alibcamps_nl.

\I{################################################################################################}
## 3.1 Terminology ## {#alib_manual_camp_modules_terminology}

In short, an \alibcamp_nl is an \alibmod_nl, which
1. Follows a defined "bootstrap" and "shutdown" process, and
2. Is enabled to manage externalized resources.

Both techniques are required by most softwares, but are not well supported by the C++ language.
This unfortunately is likewise true for the C++ language feature called "C++ Modules", introduced
with language version C++ 20.

Details for both features (bootstrapping and externalized resources) is provided in the next
chapter. For now, the following facts and terminology is important to understand:

1. Those \alibmods_nl which require sophisticated bootstrapping and externalized resources,
   are named \alibcamps_nl.
2. Both features become available by adding a singleton class to a module which derives from
   class \alib{lang;Camp}.
3. The concept can be used by custom code. Once the concept is understood, creating a custom
   \b Camp can help structure the boot process and the resource management
   of an application tremendously.
   Again: unfortunately, there is a huge lack of support in the C++ language for both fields
   of concern and hence are often challenging and error-prone parts of software.

With this in mind, specific module \alib_basecamp can be explained. This module resides in
source folder \alibsrcdir{lang/basecamp} and introduces class \alib{lang::Camp} and all tool types needed
for bootstrapping and resource management.

It would be as simple as this, if we did not add something more to it: We defined this module
as a home for classes where we did not think it made much sense to introduce an extra module for.
Or, in other words: where we thought: Whenever a user of the library decides to include
an \alibcamp_nl in the \alibdist, then certain functionality would usually be needed and added
likewise.<br>
The module organizes this functionality by introducing inner namespaces in \ref alib::lang.
Those are:
- %alib::lang::\alib{lang;format}: Classes that support text formatting in various ways.
- %alib::lang::\alib{lang;resources}: Managing externalized resources.
- %alib::lang::\alib{lang;system}: System-related types like \alib{lang::system;ProcessInfo}
  or \alib{lang::system;EnvironmentVariables}.

In addition, source files found in folder \alibsrcdir{lang/message} add types like
\alib{lang;Exception} and \alib{lang;Report} to namespace \ref alib::lang.


This functionality needs bootstrapping and resources already, and this is why module \alib_basecamp_nl
is an \alibcamp_nl itself. And finally, because this is the lowest and most fundamental of all
\alibcamps_nl, its name was chosen to be "BaseCamp"!

Let's recap this chapter quickly:
- Some of the \alibmods_nl are \alibcamps_nl
- \alibcamps_nl provide externalized resources and bootstrap management
- Custom software might use this concept and create <b> custom camp modules</b>.
- The most fundamental \alibcamp_nl is module \alib_basecamp and provides all that is needed to
  implement further \alibcamps_nl.
- in addition, module \alib_basecamp provides certain standard functionality that all "higher level-"
  \alibcamps need.

\I{################################################################################################}
## 3.2 Class Camp ## {#alib_manual_camp_modules_campmodule}
As just mentioned, module \alib_basecamp_nl introduces class \alib{lang;Camp} and every
other "high level" module, aka \alibcamp_nl, presents a singleton instance of a derived type.
For example, the singleton type of camp \alib_expressions is class \alib{expressions;ExpressionsCamp}
and the one of camp \alib_cli is class \alib{cli;CliCamp}.<br>

These singleton objects - including that of \alib_basecamp_nl itself - are collected in namespace
#alib with global instances and are named in capital letters:
- \ref alib::BASECAMP
- \ref alib::ALOX
- \ref alib::CLI
- \ref alib::CONFIG
- \ref alib::EXPRESSIONS
- \ref alib::FILES

This is the basic setup for solving the two challenges: Bootstrapping and Resource Management.
Before this manual now dives into the details, one final prerequisite should be given:
\alibcamps_nl may (and usually do) share resources and configuration data. Therefore class
\b Camp implements its data members \alib{lang::resources;ResourcePool} and \alib{config;Configuration}
as pointers.

\I{################################################################################################}
\I{##########  4.  Bootstrapping ALib                                                              }
\I{################################################################################################}
# 4. Bootstrapping And Shutting Down ALib # {#alib_manual_bootstrapping}

The term "bootstrapping" means the one-time initialization of the library that is to be performed
in an early stage of a software process.
The counterpart of bootstrapping is "shutting down" the library.

Standard bootstrapping and shutdown of \alib is performed by invoking functions #alib::Bootstrap
and #alib::Shutdown.
Such invocation usually is performed as one of the first and last actions in function <c>main()</c>.
In source code, this simply looks like this:

\code{.cpp}
        #include "alib/alib.hpp"

        int main( int argc, const char **argv )
        {
            // save cli arguments to global ALib variables
            alib::ARG_C=  argc;
            alib::ARG_VN= argv;

            // bootstrapping
            alib::Bootstrap();
            
            ...
            ...
            ...

            // termination of ALib
            alib::Shutdown();

            return 0;
        }
\endcode

The following chapters explain the details behind these invocations, provide recipes to customize
certain aspects of the process, and explain how  - optionally - the provided mechanics can be
leveraged for the bootstrapping and shutdown of custom code entities likewise.

\attention If you are new to \alib, you should skip the rest of this chapter completely and
           rather continue reading next manual chapter \ref alib_manual_build "6. Building The Library".<br>
           The reason for this is that the recipe above is good for the standard use cases of \alib
           and that the rest of this chapter becomes more and more complex and <b>probably very
           irritating to a new user!</b><br>
           So \ref alib_manual_build "please go ahead, nothing to see here!"  \emoji :sweat_smile:



\I{################################################################################################}
## 4.1 Weak Support For Bootstrapping In C++ ## {#alib_manual_bootstrapping_cpp}

The following circumstances increase the complexity of bootstrapping:

<b>1. The C++ Language:</b><br>

\par
The C++ language specification misses sophisticated support for bootstrapping a software process.
Basically, the only two things that are guaranteed by the language are:
- Global and static data will be initialized.
- After this is done, a method called <c>main()</c> is invoked.

\par
This specification especially misses an order of the initialization of global or static data.
Note that data initialization might execute arbitrary constructor code (before method <c>main()</c>
is invoked!), hence also the order of such custom code execution is random.

<b>2. Resources And Configuration Data:</b><br>

\par
Bootstrapping becomes even more complicated with the use of
1. Resources (for example externally managed string resources) and
2. Configuration data (e.g., configuration files, command line parameters, etc.).

\par
With \alib, such custom data sources might furthermore be provided by plug-ins, which means the code
entity that performs the provision of resources and configuration data needs to be initialized
before the data is used.

<b>4. Module Dependencies:</b><br>

\par
The dependencies between the modules have to be respected during initializations.
As mentioned already, dependent modules have to be initialized first.

<b>4. Multithreaded Access:</b><br>

\par
Multithreaded software has to take preparations to avoid undefined behavior due to thread-racing
conditions while accessing "shared resources", which is often simply shared memory.
Such preparation involves a performance penalty.
In the case that all modifications of the memory that is to be protected can be aggregated to
a one-time initialization action during bootstrap, such precaution could be omitted if
bootstrapping guaranteed a period of single threaded access.

\I{################################################################################################}
## 4.2 Bootstrapping Non-Camp Modules ## {#alib_manual_bootstrapping_nocamps}
An \alibdist might consist of only "non-Camp" modules. Currently those are:
\ref alib_mod_bitbuffer     "BitBuffer",
\ref alib_mod_boxing        "Boxing",
\ref alib_mod_characters    "Characters",
\ref alib_mods_contmono     "Containers",
\ref alib_mod_enums         "Enums",
\ref alib_mods_contmono     "Monomem",
\ref alib_mod_singletons    "Singletons",
\ref alib_mod_strings       "Strings",
\ref alib_mod_threads       "Threads" and
\ref alib_mod_time          "Time",

In this case that such a reduced  \alibdist_nl is wanted, the snippet of function <c>main()</c>
shown above is all that is needed, because their bootstrapping is done by namespace functions
#alib::Bootstrap, respectively #alib::Shutdown. These two functions are internally changed
depending on the \alibdist_nl and thus a user of the library does not need to care on the details.

                  b
Nevertheless, the details are listed here. In fact, only \b five of the non-camp modules provide any
bootstrapping functionality. Each module provides either namespace functions \b %Bootstrap or
\b %Shutdown (or both) for that. Those are:

Module             | Bootstrap function                | Shutdown Function                 
-------------------| ----------------------------------| -------------------------------
\alib_boxing       | \ref alib::boxing::Bootstrap()    | (no shutdown necessary/available)
\alib_enums        | \ref alib::enums::Bootstrap()     | (no shutdown necessary/available)
\alib_singletons   | (no bootstrap necessary/available)| \ref alib::singletons::Shutdown()
\alib_time         | \ref alib::time::Bootstrap()      | \ref alib::time::Shutdown()
\alib_threads      | \ref alib::threads::Bootstrap()   | \ref alib::threads::Shutdown()

\note For that reason, these five modules have the small rectangular markers in the
      \ref alib_manual_modules_dependencies "dependency graphs"


More complex boot strapping strategies that are shown in the following chapters, will always
under the hood and automatically care for the non-Camp modules. Therefore, a reader might easily
forget about them!


\I{################################################################################################}
## 4.3 Bootstrapping ALib Camps ## {#alib_manual_bootstrapping_camps}
When \alibcamps are included in the \alibdist, then things can become a little more complex.

But first of all the good news: Even in the usual case that one or more \alibcamps_nl are included in
an \alibdist, bootstrapping and shutdown of the library is done exactly the same
as \ref alib_manual_bootstrapping "shown above". In other words:

\attention The following chapters about bootstrapping should be read only if you want to modify
           the bootstrap process for one of the following reasons:
           - A custom \alibcamp_nl was built, which should be integrated in the bootstrap process
           - Customized configuration and/or resource-data setups or mechanics should be
             implemented
           - Other custom reasons, not foreseen by the author of this text.

If the above does not apply to your field of application, all you might need to know is that
functions #alib::Bootstrap and #alib::Shutdown are internally largely extended in the presence of
\alibcamps_nl but, it is still the same simple call.

\I{################################################################################################}
### 4.3.1 ALib Camps Bootstrap Interface ### {#alib_manual_bootstrapping_camps_if}

For bootstrapping and shutdown, types derived from class \alib{lang::Camp} need to implement two
protected, \b abstract methods, namely 
- \alib{lang;Camp::bootstrap} and
- \alib{lang;Camp::shutdown}.

Both methods are invoked more than once: Bootstrapping is done in <b>three phases</b>, defined by
enumeration \alib{BootstrapPhases} and shutdown is done in <b>two phases</b>, defined by
enumeration \alib{ShutdownPhases}.
The implementations of the methods need to switch upon the given levels and perform different tasks.

Each phase will be finished for all camps, before a subsequent phase is initiated.
The loop over the phases and its inner loop over the list of camps is implemented with
namespace functions:
- \doxlinkproblem{namespacealib.html;ac33999bb9be31380c6ae139210d23083;alib::Bootstrap(BootstrapPhases targetPhase; lang::Camp* targetCamp, int,int,TCompilationFlags)}.
,
  respectively
- \doxlinkproblem{namespacealib.html;acd654b95c6e1833f4f04d0fc19e4ce36;alib::Shutdown(ShutdownPhases targetPhase; lang::Camp* targetCamp)}
.

\note
    These new overloaded versions are available only with inclusion of header
    \alibheader{lang/basecamp/bootstrap.hpp}, which in turn is only available with inclusion of
    the \alib_basecamp in the \alibdist_nl.<p>
    The parameterless versions we had seen before are declared in header file
    \alibheader{alib.hpp}, and thus "always" available.


\I{################################################################################################}
### 4.3.2 Bootstrap Phases ### {#alib_manual_bootstrapping_camps_bs}

With function \doxlinkproblem{namespacealib.html;ac33999bb9be31380c6ae139210d23083;Bootstrap;alib::Bootstrap(BootstrapPhases targetPhase, lang::Camp* targetCamp, int,int,TCompilationFlags)},
an outer loop over all phases is initiated. An inner loop iterates over all camps, in order of their
"dependency level". For each combination of phase and camp, virtual method \alib{lang;Camp::bootstrap}
is called. This ensures that for each phase, "low level camps" are initialized first and those of
"higher level" are initialized next.

The three phases are defined as follows:

1. \alib{BootstrapPhases::PrepareResources}<br>
   All initialization actions that exclusively involve static data, is deemed to be performed
   in this phase. For example, the \alib{boxing;BootstrapRegister;registration} of
   \ref alib_boxing_functions "box-functions" is to be done in this phase.

   The phase received its name from the fact that the  \alib{lang::resources;ResourcePool} is already
   available and a camp is supposed to feed its static default resource strings to the instance
   retrieved with \alib{lang;Camp::GetResourcePool}.

   More on this topic will be explained in a later chapter.<br>


2. \alib{BootstrapPhases::PrepareConfig}<br>
   In this phase, the configuration object is available and receivable with \alib{lang;Camp::GetConfig}.
   It may now be extended with plug-ins, but <b>access to configuration variables is not allowed yet!</b> <br>
   For most camps there is not much to do in this phase.<br>
   One important action usually placed here is to initialize static
   \ref alib_enums_records "ALib Enum Records". If - as it is recommended -
   \ref alib_enums_records_resourced "record definitions are given using externalized strings",
   then this is definitely the right place to invoke \alib{enums;EnumRecords::Bootstrap} for each
   enumeration in question.
   \note
     While the enumeration records are static data and could also be initialized in the first
     phase, after the definition of resources, placing it in this phase allows camps of higher
     dependency levels to modify (patch!) the resources of a lower level camp  - before their use.

   <br>

3. \alib{BootstrapPhases::Final}<br>
   In this phase, final initialization steps may be performed.<br>
   Only in this phase the <b>start of threads is allowed (!)</b>, as any mandatory \alib initializations which
   are not protected against racing conditions are deemed to be performed in the previous phases.<br>
   Furthermore, access to configuration variables is allowed in this phase.


\I{################################################################################################}
### 4.3.3 Shutdown Phases ### {#alib_manual_bootstrapping_camps_sd}

With function \doxlinkproblem{namespacealib.html;acd654b95c6e1833f4f04d0fc19e4ce36;alib::Shutdown(ShutdownPhases targetPhase; lang::Camp* targetCamp)}
,
an outer loop over the two shutdown phases is initiated.
Again, an inner loop iterates over all camps, but this time in \b reverse order of their
"dependency level". For each combination of phase and camp, virtual method \alib{lang;Camp::shutdown}
is called. This ensures that for each phase, "high level camps" are shut down first and those of
"lower level" are shutdown later.

The two phases are defined as follows:

1. \alib{ShutdownPhases::Announce}<br>
   Within this phase, a camps can rely on the full stability of a software and all camps.
   Only actions might be taken that do not result in defects if features of a camp are still
   used. Typical samples of actions to perform with this phase are writing out configuration data or
   making an application's state  persistent, to be able to restore it with a next run of a
   software.
   <br><br>

2. \alib{ShutdownPhases::Destruct}<br>
   This is the "true" shutdown phase. After this phase was invoked, a camp is obligated to have
   freed all its resources and is allowed to be dysfunctional afterwards.


\I{################################################################################################}
## 4.4 List "Camps" ## {#alib_manual_bootstrapping_list_camps}

For default bootstrapping and shutdown, this is almost all we need to know. The remaining question is:
Where is the list of \alibcamps_nl, which is used for the inner loops of functions \b Bootstrap and
\b Shutdown, defined?

Just like the overloaded, parameterized \b Bootstrap and \b Shutdown functions, the list becomes
available with the inclusion of camp \alib_basecamp and with including its header file
\alibheader{lang/basecamp/bootstrap.hpp}. There, the declaration \ref alib::CAMPS is given,
along with function \ref alib::BootstrapAddDefaultCamps, which fills list <b>%alib::CAMPS</b> in the
right order. The right order means: respecting the dependency hierarchy and permutation of camp
\alib{lang;basecamp::BaseCamp} and those other camps, which might be included in \alibdist_nl.

With a complete \alibdist_nl, the list will be:
1. \ref alib::BASECAMP
2. \ref alib::CONFIG
3. \ref alib::ALOX
4. \ref alib::CLI
5. \ref alib::EXPRESSIONS
6. \ref alib::FILES

Again, the list is traversed from top to bottom with bootstrapping and in reverse order when
the camps are shut down.

Now, if function \ref alib::Bootstrap is invoked without explicitly filling the list beforehand,
(right as it was shown in the introductory section of this chapter), then the empty list will
be detected and function \b BootstrapAddDefaultCamps is automatically invoked.

In later sections of this manual, options for modifying these defaults will be demonstrated.

\I{################################################################################################}
## 4.5 Standard Bootstrapping ## {#alib_manual_bootstrapping_standard}
With the knowledge taken from the previous sections, it is now easily understood what was said
in the introductory chapter:
\par
    <em>Standard bootstrapping of \alib is performed by invoking function \alib{Bootstrap}</em>

Consequently the simple version of bootstrapping was given like this:

        int main( int argc, const char **argv )
        {
            alib::Bootstrap();
            ...
            ...


This standard invocation of function \alib{Bootstrap} not only fills the list \ref alib::CAMPS,
but also invokes overloaded function<br>
\doxlinkproblem{namespacealib.html;ac33999bb9be31380c6ae139210d23083;alib::Bootstrap(BootstrapPhases targetPhase, lang::Camp* targetCamp, int,int,TCompilationFlags)}.
<br>
with parameters:
- \p{targetPhase}= \alib{BootstrapPhases::Final}, and
- \p{targetCamp} = <b>alib::CAMPS.Back()</b>

The loop iterating the phases, starts with the first phase that the camps have \b not been
bootstrapped with, yet, and ends with the given \p{targetPhase}. With that, all phases are executed
in above sample. And because the target camp is set to the last one in the list, the inner loop
covers all camps.

What was not discussed, yet is when the instances of \alib{lang::resources;ResourcePool} and
\alib{config::Configuration} are created and how the corresponding pointer
members \alib{lang;Camp::resourcePool} and \alib{lang;Camp::config} are set.

This is how this is done:
- An instance of class \alib{lang::resources;ResourcePool} is created before executing the first
  phase \alib{BootstrapPhases;PrepareResources} and is distributed among the camps.<br>
  The distribution is performed in reverse order, starting with the given \p{targetCamp}
  and from there to the lower level camps.

- The same is done with a new instance of class \alib{config;Configuration}, but this is
  done only before executing second phase \alib{BootstrapPhases;PrepareConfig}.


The following schematic summarizes this:

  - Outer loop: Phase PrepareResources
    - Creation of a resource pool.
    - 1st inner loop: Distribution to lower level camps in reverse (descending) order.
    - 2nd inner loop: Invocation of <em>bootstrap( PrepareResources )</em> on all dependent camps, in ascending order.
  - Outer loop: Phase PrepareConfig
    - Creation of configuration instance.
    - 1st inner loop: Distribution to lower level camps in reverse (descending) order.
    - 2nd inner loop: Invocation of <em>bootstrap( PrepareConfig )</em> on all dependent camps, in ascending order.
  - Outer loop: Phase Final
    - Invocation of <em>bootstrap( Final )</em> on all camps, in ascending order.

With this information the standard process of bootstrapping is well defined. The following
chapters introduce different ways to customize bootstrapping.

\I{################################################################################################}
## 4.6 Customizing The Bootstrap Process ## {#alib_manual_bootstrapping_customize}

\I{################################################################################################}
### 4.6.1 Bootstrapping Custom Camps ### {#alib_manual_bootstrapping_custommods}
As explained in previous chapter \ref alib_manual_bootstrapping_cpp, the reason why bootstrapping
\alib is a non-trivial task, does not lie in specifics of this library, but in a general lack of
support for bootstrapping in the C++ language. Any more complex software needs to solve this
task.

Consequently, it might be a reasonable decision, to adopt what this library offers and use this
concept and the mechanics to bootstrap custom code units likewise.

Doing this is rather simple. The steps are:
1. Create a custom type derived from class \alib{lang;Camp} along with a global singleton instance of
   this type.
2. In the <c>main()</c> function, invoke \alib{BootstrapAddDefaultCamps}.
3. Next, use method \alib{containers;List::PushBack} to add the static instance of the custom
   camp type(s) to variable \alib{CAMPS}.
4. Perform bootstrapping by invoking function \ref alib::Bootstrap.

\note A complete source code sample is given with the \ref alib_cli_tut "tutorial of ALib Camp CLI".
      Here is the corresponding excerpt from the its <b>main()</b>-function:
      \code{.cpp}
        int main( int argc, const char **argv )
        {
            alib::ARG_C  = argc;
            alib::ARG_VN = argv;

            // 1. Add our custom camp to the list of camps
            alib::BootstrapAddDefaultCamps();
            alib::CAMPS.PushBack( &SAMPLE_CAMP );

            // 2. Initialize all modules
            alib::Bootstrap();
            ...
            ...
      \endcode


In more complex scenarios, a software can also create more than one module.
Each camp may be appended to list \ref alib::CAMPS (see step 3). If these camps are following a
dependency hierarchie, then the lowest (most independent) camps should be attached first.
The target camp of the bootstrapping (step 4) is the latest camp added.

This recipe addresses the simple cases. When a software chooses to hook itself into the bootstrap
mechanism of \alib as shown above, the resources and configuration data (including for example INI-files)
are shared between all modules and thus contain both, data of \alib and that of custom camps.

Often, this is not a desired scenario, because:
- A software does not want to bother an end-user with the rather technical resources and
  configuration variables provided by \alib. Cluttering custom data with \alib data should be avoided
- A software might want to disallow an end-user to modify configuration data and/or resources
  of \alib.
- etc.

How these goals can be reached is explained in the next section.

\I{################################################################################################}
### 4.6.2 Using Custom Resources and/or Configuration Plug-ins ### {#alib_manual_bootstrapping_customize_resconf}

The default implementation of class \alib{lang::resources;ResourcePool} used with non-customized
bootstrapping is of type  \alib{lang::resources;LocalResourcePool} and this does \b not externalize
the resource strings. To use either alternative, built-in type \alib{config;ConfigResourcePool}
or a custom implementation of the interface that attaches to a 3rd-party "resource pool backend",
all that needs to be done is to create such instance and pass it to method
\alib{lang;Camp::BootstrapSetResourcePool} of the last (!) camp in \ref alib::CAMPS.<br>
This has to be done \b before the first invocation of function
\doxlinkproblem{namespacealib.html;ac33999bb9be31380c6ae139210d23083;Bootstrap;alib::Bootstrap(BootstrapPhases targetPhase, lang::Camp* targetCamp, int,int,TCompilationFlags)}.

As documented with class \alib{config;Configuration}, the type allows being equipped with custom
mechanics for loading external data. To do so, the bootstrapping process has to be interrupted
after the configuration is internally created. This is done by explicitly specifying
\alib{BootstrapPhases::PrepareConfig} with function \b Bootstrap.

Now, the configuration can be accessed with \alib{lang;Camp::GetConfig} and
is manipulated as wanted. When done, bootstrapping is finalized by invoking \b Bootstrap a
second time with argument \alib{BootstrapPhases::Final}.

The schematic that shows both approaches at once, then looks as follows:
- Create instance \b myResources on the last camp in \ref alib::CAMPS.
- Invoke \b BootstrapSetResourcePool(myResources).
- Invoke \b Bootstrap(PrepareConfig).
- Modify configuration object received with \b GetConfig (on any of the \alibcamps_nl, because
  they still all share the same instance).
- Invoke \b Bootstrap(Final).


\I{################################################################################################}
### 4.6.3 Using Multiple Resources and/or Configuration Instances ### {#alib_manual_bootstrapping_customize_non_shared}

With the bootstrap process described so far,
- different instances of a resource pool, and
- \b one instance of class \b Configuration, but customized, e.g., with custom
  \alib{config;ConfigurationPlugin}


To also add a different configuration instances, method \alib{lang::Camp;BootstrapSetConfig} is
given, just like the already introduced method \alib{lang;Camp::BootstrapSetResourcePool}.

Both methods may be called on arbitrary camps, \b before starting bootstrapping the corresponding
phase.

An object set this way will not be overwritten when the corresponding bootstrap phase distributes
instances from higher- to lower-level camps. Moreover, if the algorithm finds an object already
set, then the distribution loop continues with this object!

This allows a rather natural way to separate all camps into logical sets that share one
resource pool and/or configuration.

Nevertheless, to also distribute different configuration objects, the bootstrap phases have
to be separated, as shown in the next section.

\I{################################################################################################}
### 4.6.4 Separating Bootstrap Phases ### {#alib_manual_bootstrapping_customize_separated}
The fact that method
\doxlinkproblem{namespacealib.html;ac33999bb9be31380c6ae139210d23083;alib::Bootstrap(BootstrapPhases targetPhase; lang::Camp* targetCamp, int,int,TCompilationFlags)}.

is allowed to be called with setting parameter \p{targetCamp} to a "lower level" camp
(one that is not the last of list \ref alib::CAMPS), allows bootstrapping lower-level camps separately
from bootstrapping higher-level ones.

As discussed in previous section, such approach might be needed in case that custom camps are integrated
into the \alib bootstrap process.

Let's consider the following sample:
1. Custom camp \b MyApp is  derived from class \alib{lang::Camp}
2. In the <c>main()</c> function, \alib{BootstrapAddDefaultCamps} is invoked.
3. The singleton of \b MyApp is added to the end of list \alib{CAMPS} (hence on the last \alib camp)
4. Bootstrapping is fully performed on the second last singleton in list \b CAMPS.
5. Bootstrapping is afterwards fully performed on singleton \b MyApp.

The consequences from this approach are:
- The set of \alibcamps_nl share a different resource pool and configuration object than \b MyApp.
- During the bootstrapping of \b MyApp, built-in \alibcamps_nl are already fully bootstrapped and functional.

\note If you want to test that you have understood the mechanics of bootstrapping \alib, you might
      answer the following question for yourself: Why can step 5 above be done by simply
      invoking parameterless funcition \ref alib::Bootstrap()?

Alternatively the same can be reached with the following recipe:
1. - 3. same as above
4. Create instance \b alib_Resources and set it on the last \alibcamp_nl in \ref alib::CAMPS.
5. Invoke \b Bootstrap(PrepareConfig).
6. Create an instance alib_Configuration and set it also on the last \alibcamps_nl  in \ref alib::CAMPS.
7. Invoke \b Bootstrap(Final).

\I{################################################################################################}
### 4.6.5 Conclusion ### {#alib_manual_bootstrapping_customize_conclusion}
We admit, that what was said above is a little complex. But once you understand why both recipes
given above lead to the same very same result, you can tell that you understood
<b>ALib Bootstrapping</b>!<br>
With this toolset and the knowledge that:
- Both recipes can be mixed,
- Custom resource pool instances might load resources externally, instead of compiling them into
  an executable's data section,
- Different camps may use different resource pools
- The same is true for configuration data
- Completely other custom operations can be performed in the different bootstrap phases defined with
  \alibcamps_nl,
- etc.

... bootstrapping a software becomes a very well defined and organized undertaking!


\I{################################################################################################}
### 4.6.5 Customization Of Shutdown ### {#alib_manual_bootstrapping_custom_shutdown}
Customization of shutting down \alib should be a seldom need and works the same in principle.
Everything is done in reverse order.
Function \doxlinkproblem{namespacealib.html;acd654b95c6e1833f4f04d0fc19e4ce36;alib::Shutdown(ShutdownPhases targetPhase; lang::Camp* targetCamp)}

has defaulted parameters \p{targetPhase} and \p{targetCamp} and if not specified alle camps
in list #alib::CAMPS are shutdown over both phases. For this, parameter \p{targetCamp}, this time
defaults to the first camp in the list.

if \p{targetPhase} is specified as \alib{ShutdownPhases;Announce} then only the first
phase is performed and custom actions might be taken before invoking the second phase
\alib{ShutdownPhases::Destruct}.

For simplicity, in contrast to bootstrapping the resource and configuration objects are
destroyed right in the reverse loop of phase \b Destruct, in contrast to adding a post-process
second loop and done with bootstrapping. Thus, a "lower" camp's resources and config objects
might be already destroyed, at the moment the camp itself is destructed. Hence, phase \b Announce
is the true last point of action in respect to an involvement of these objects.


\I{################################################################################################}
## 4.7 Assuring Compilation Compatibility ## {#alib_manual_bootstrapping_assertcomp}

Due to the facts that
- \alib allows reducing to a subset of its functionality by changing the \alibdist_nl, and
- \alib provides a set of compilation flags, which for example switches default character sizes,
        or turns other features on or off, and also because
- \alib follows the policy of having the freedom to quite radically changing the internal structure
         and types with any new version, in other words is never "downwards compatible",

it is important that a compiled \alib library must be incompatible to compilation units
that actually use the library. For example, the footprint (size) of library types might be different.

With C++, if incompatible compilation units are mixed, often no warning is issued (neither by
the compiler nor the linker) and the execution of a process might lead to very strange behavior
which is irritating and hardly to debug.

To detect incompatible compilation settings and library versions, three preprocessor symbols have
been defined. Each symbol has a corresponding namespace variable compiled into the library.
The symbols and variables are:

Preprocessor Symbol          | Namespace Variable
-----------------------------| ---------------------------------
\ref ALIB_VERSION            | \ref alib::VERSION
\ref ALIB_REVISION           | \ref alib::REVISION
\doxlinkproblem{group__GrpALibPreproMacros.html;gaac71b6511690f5af0abf5213b0434111;ALIB_COMPILATION_FLAGS;ALIB_COMPILATION_FLAGS}   | \ref alib::COMPILATION_FLAGS

Along with that, namespace function \ref alib::AssertALibVersionAndFlags is given, which expects
comparison values for each of the namespace variables. The little trick here is, that the
parameters are defaulted with the macros. Consequently, the parameters must not be given when
the function is invoked.

Furthermore, the parameters are also silently (aka defaulted) added to overloaded functions
\ref alib::Bootstrap, which internally invokes \b AssertALibVersionAndFlags().

<b>For this reason, nothing specific has to be done by a user of the library, as the checks are already
performed with bootstrapping!</b>

\note
   With \e CMake builds, if \e CMake function \ref alib_manual_build_cmake_5 "ALibSetCompilerAndLinker"
   is used with a custom target, all settings are guaranteed to be compatible to a library that
   is built within the same CMake process.

\I{################################################################################################}
\I{##########  5.  Multithreading                                                                  }
\I{################################################################################################}

# 5. Multithreading Support # {#alib_manual_multithreading}
With \alib, switching support for multithreading on and off is not a preprocessor symbol. 
Instead, the support is dependent on selecting module \alib_threads into the \alibdist
or not.

If it is not selected, certain macros and function remain to exists: They are just emptied and 
with that pruned by the preprocessor or optimized out bei the compiler.

This slight breaking of the rule - namely to have entities belonging to a certain module still 
existing when this module is not available - brings a huge benefit: All other modules and likewise
the custom code library users, can be implemented without much code cluttering. 

The gaol here is: Protection mechanisms against thread racing conditions should generally be
included in any user's project, they are just pruned in case that code used with an
\alibdist_nl that misses module \alib_threads_nl.

All further information is provided with the \ref alib_mod_threads "dedicated Programmer's Manual"
of module \alib_threads_nl.


\I{################################################################################################}
\I{##########  6.  Build                                                                           }
\I{################################################################################################}

# 6. Building The Library # {#alib_manual_build}

\I{################################################################################################}
## 6.1 Overview ## {#alib_manual_build_overview}

\I{################################################################################################}
### 6.1.1 Platforms and Toolchains ### {#alib_manual_build_overview_platforms}

As of today, <b>%ALib for C++</b> is compiled and tested under the following platforms and
toolchain combinations:
- GNU/Linux Arch 6.12.1, GNU C++ 14.2.1 / Clang++ 18.1.8, C++ 17/20/23, 32-Bit / 64-Bit<br>
  (This is the main development platform.)
- WindowsOS 10/11, MSC 19.42 (Visual Studio 2022), C++ 17/20, 32-Bit / 64-Bit
- WindowsOS 10/11, MinGW, GCC 13.47  C++ 17/20, 64-Bit 
- macOS Sequoia 15.2, Apple M2 / ARM64, Apple Clang Version 16.0.0, C++ 17/20/23, 64-Bit
- Raspberry 3, ARM, 64-bit OS, GNU C++ 12.2.0, C++ 17/20/23
- Raspberry 4, ARM, 64-bit OS, GNU C++ 12.2.0, C++ 17/20/23

All development was performed in a) CLion/CMake (all platforms) and b) Visual Studio Community
Edition 2022. All necessary IDE files are included in the repository.

Adoptions to other platforms, toolchains, and IDEs should be implementable with limited efforts.
All relevant code which selects platform/toolchain specific libraries, intrinsics, etc., will
expose a preprocessor error if a section fails due to an unknown environment.
This allows quickly inserting the right platform/toolchain specific code at these places.

\note We would be very happy to receive your feedback/input on necessary changes for other
      platforms!

Especially errors occurring in header \alibheader{lang/integers.hpp} might be quite likely for unknown
platform / toolchain combinations. Here, a set of five compiler symbols might be passed using
the build system (e.g., CMake), which are documented with symbol \ref ALIB_SIZEOF_INTEGER.

<b>The C++ compiler warning level is defaulted to the bearable maximum.</b> This means, that the
inclusion of \alib headers into a custom project's compilation process should never lead to
compilation warnings or errors when similar high custom warning levels (of the including project)
are used.


\I{################################################################################################}
### 6.1.2 Reference Toolchain CMake ### {#alib_manual_build_overview_cmake}

While project files for different IDEs might be provided with the codebase, the main development of \alib is
performed using \https{CMake,cmake.org/} scripts.
Also, the relationship of source file and \alibmod_nl selection is <b>"officially defined"</b>
by CMake scripts.

\par
  <b>The \e CMake scripts included in the \alib distributions are to be seen as the
  reference guideline to building \alib. The scripts may even be more precise (and up-to-date!)
  than the documentation you currently read itself.</b>


Even for non-experienced users (in respect to CMake), the syntax of the file should be easily understood.
So, do not hesitate to open and read the CMake files for your project setup reference.

A Microsoft Visual Studio solution and according project files which build an \alib DLL and the
unit tests, are included in the \alib distribution.
These may be used to compile an \alib library that includes all of \alib, which can be used
for own projects.<br>
Limited library projects that include only a selection of modules of \alib are not provided
and thus have to be created if they are desired.


\I{################################################################################################}
### 6.1.3 Library Installation ### {#alib_manual_build_overview_noinstall}
As of today, no installation process of a shared library version of \alib is available with
the build process defined. Not installing a library has of course the disadvantage, that every
software project needs to compile its own version of it, and the library is by default not shared between
applications, even if compiled as a "shared library", respectively DLL.

While this may change in the future, the advantage of this approach is that an application has
a lot flexibility in respect to compiling \alib using the exact set of features it desires.

Therefore, to enable a software to use \alib, the sources of the library have to become a part
of the build process in any form. As usual, there are three possible basic options:
1. Compiling \alib to a static library,
2. compiling \alib to a shared library or
3. compiling \alib sources directly into another software entity.

When this manual section talks about "building the ALib library", one of the three options is meant.

\note
  Of course, a custom installation process can be established to place library binary and header
  files in corresponding system folders. It is just not provided today.


\I{################################################################################################}
### 6.1.4 Unit Tests ### {#alib_manual_build_overview_unittests}
An extensive set of unit tests is included in the distribution.


\I{################################################################################################}
## 6.2 Performing A Test Compilation  ## {#alib_manual_build_test}
Clone the \alib repository from \https{ALib at GitHub,github.com/AlexWorx/ALib-Class-Library}
to a place where you commonly store 3rd party libraries that your projects are using.
Alternatively, download and unpack the ZIP file to that same place.

In this documentation, we will refer to this library location as the <b><em>ALIB_BASE_DIR</em></b>.
After a fresh installation, it should contain at least the following subfolders:

        ALIB_BASE_DIR/build
        ALIB_BASE_DIR/docs
        ALIB_BASE_DIR/html
        ALIB_BASE_DIR/src
        ALIB_BASE_DIR/tools


To build the unit tests, perform the following steps:

- open a console window and cd into directory:

        ALIB_BASE_DIR/build/cmake/unittests

- create two subfolders from here, one named *debug* and the other *release*
- cd into directory *debug* and type

        cmake ..

- once \e CMake has run, type

        make -j

The compiled sample executable should have been created and you can start it with

    ./ALib_UT

Within a few seconds all unit tests should have been performed.

For a <b>release build</b>, the steps are similar.
After you cd into the 'release' folder, the cmake command is:

    cmake -DCMAKE_BUILD_TYPE=Release ..

\note
  For the unit tests, \e Google \e gtest gets downloaded (once) and incorporated
  as a source project within the binary folder. This means:
   - This library does not need to be installed anywhere else on the system
   - This library gets deleted when deleting the CMake build folder
     (in the sample above named "debug" respectively "release").


\I{################################################################################################}
## 6.3 A Step-By-Step CMake Sample ##  {#alib_manual_build_sample}

With using \e CMake, compiling and using \alib  is very straight forward.
This is because a set of easy to use \e CMake scripts is provided, of which one is to be included
into the custom  \e CMake script.

The following demonstrates this step-by-step along the sample project found in folder

        ALIB_BASE_DIR/src/samples/alox

<br>
<b>Step 1: Creating the CMake file</b><br>
A custom \e CMake file has to be created. To start with, the top of the file might look like this:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_1

<br>
<b>Step 2: Choose %ALib Modules</b><br>
The list of \alib modules to be included in the built is defined with \e CMake list variable
\c ALIB_DISTRIBUTION which has to be set before invoking the \alib \e CMake script.
If the list is left empty, it will be defaulted to \b "ALL", which chooses all \alib modules
available.<br>
In our sample, we add \b "ALOX", which chooses module \alib_alox and all
\ref alib_manual_modules_dependency_graph "dependent modules".


 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_2


<br>
<b>Step 3: Set Other Feature Variables</b><br>
Our project should be compiled using C++ 17. This is set with:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_3


<br>
<b>Step 4: Include "ALib.cmake" </b><br>
Now we are ready to include the main \alib \e CMake script:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_4

Note that this sample is using a relative path. In a real-world sample, the path might as well
be an absolute one.
After the script is run, some global \e CMake variables and functions are defined.
All of those are documented in the next chapter. Let us first continue the sample:

<br>
<b>Step 5: Define A Library Project </b><br>
We invoke \e CMake function \c ALibAddSharedLibrary, which creates a \e CMake target called
\c "ALib_SharedLib" having all necessary settings:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_5

<br>
<b>Step 6: Define The Custom Project </b><br>
Now we are good to define our custom project in a usual way:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_6

<br>
<b>Step 7: Add Compiler And Linker Settings</b><br>
Our main project needs to share some \alib compiler and linker settings with the \alib
library project. To achieve this we invoke \e CMake function <c>ALibSetCompilerAndLinker</c>:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_7

<br>
<b>Step 8: Add Library Project To Custom Project</b><br>
The final step is to add the library project to the custom sample project, which is again a
standard \e CMake task:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_8


That's it. With this simple CMake file we have created a tailored \alibdist library
and have linked it to our sample project, which is now ready to be built!


\I{################################################################################################}
## 6.4 The Build Process In Detail##  {#alib_manual_build_cmake}

The previous chapter demonstrated the use of the \e CMake script \c ALib.cmake provided with
\alib. In the following sections a reference documentation on all aspects of the script is
given.

Along these lines, the build requirements of \alib is explained - also for users of any other build
system. independent of the build-system used, the following information has to be collected and
accordingly set:

- The set of \alib source files that comprise the desired set of \alibmods.
- C++ Version 17 or higher (to be set with the compiler).
- compiler symbols that select the \alibmods have to be passed to the compiler.
- compiler symbols that choose \alib features have to be passed to the compiler.
- External library dependencies have to be determined.
- Linker flags have to be defined.

\note
  The complete set of symbols that the \alib library accepts from the compiler (usually \e -D option)
  are \ref GrpALibPreproSymbols "documented here".

\I{################################################################################################}
### 6.4.1  Selecting ALib Modules ###  {#alib_manual_build_cmake_1}
\e CMake list variable \c ALIB_DISTRIBUTION is an input/output list which defines the particular
\alibmods that should be included in the built.
The script will process the values given and will extend the list to include all necessary modules that
the \ref alib_manual_modules_dependency_graph "given selection depends on".

The values correspond to the \ref alib_manual_modules_overview "module names" in upper case letters,
hence \b ALOX, \b BOXING, \b CHARACTERS, etc...

If the variable is not set or contains special name \b "ALL", all modules are chosen to be built
and included.

\I{################################################################################################}
### 6.4.2  Variables For Directory and Source Code Definitions ###  {#alib_manual_build_cmake_2}
The following \e CMake variables are available after the invocation of the script:
- \c ALIB_BASE_DIR<br>
  Defines the directory where the \alib project is stored.<br>
  This variable may also be specified before entering the script to specify a different directory
  than the default, which is inferred from the full path name of the script invoked.
<p>

- \c ALIB_SOURCE_DIR<br>
  Defines the directory where the \alib source files are stored. The header files are located
  in the same directories.<br>
  This variable may also be specified before entering the script to specify a different directory
  than the default, which is

            ${ALIB_BASE_DIR}/src
<p>

- \c ALIB_SOURCE_FILES<br>
  The list of source files (compilation units) needed to compile \alib.
<p>

- \c ALIB_INCLUDE_FILES<br>
  The list of header files needed to compile \alib.


\I{################################################################################################}
### 6.4.3  Cached CMake Variables ###  {#alib_manual_build_cmake_3}
The script will create a set of cached boolean \e CMake variables (which are variables that can be
edited with CMake GUI tools and various C++ IDEs).

The following variables correspond directly to \ref GrpALibPreproSymbols "preprocessor symbols"
used for code selection and thus this list links to their corresponding documentation:

- \ref ALIB_CHARACTERS_WIDE
- \ref ALIB_CHARACTERS_SIZEOF_WCHAR
- \ref ALIB_DEBUG
- \ref ALIB_DEBUG_BOXING
- \ref ALIB_DEBUG_CONTAINERS
- \ref ALIB_DEBUG_CRITICAL_SECTIONS
- \ref ALIB_DEBUG_ALLOCATIONS
- \ref ALIB_DEBUG_MONOMEM
- \ref ALIB_DEBUG_ARRAY_COMPRESSION
- \ref ALIB_DEBUG_RESOURCES
- \ref ALIB_DEBUG_STRINGS
- \ref ALIB_EXT_LIB_THREADS_AVAILABLE
- \ref ALIB_FEAT_BOOST_REGEX
- \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
- \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
- \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
- \ref ALIB_FEAT_SINGLETON_MAPPED
- \ref ALIB_FILES_FORCE_STD_SCANNER
- \ref ALIB_GDB_PP_FIND_POINTER_TYPES
- \ref ALIB_GDB_PP_SUPPRESS_CHILDREN
- \ref ALIB_PRECOMPILED_HEADER
- \ref ALIB_RESOURCES_OMIT_DEFAULTS
- \ref ALOX_DBG_LOG
- \ref ALOX_DBG_LOG_CI
- \ref ALOX_REL_LOG
- \ref ALOX_REL_LOG_CI

In addition, the following further cached variables are set:

- <b>ALIB_VERSION</b><br>
  Defines the \alib library version. This variable cannot be changed, respectively will be
  overwritten on \e CMake generation. It is specified as a cached \e CMake variable just for the
  reason of presenting the \alib version to tools that allow to modify \e CMake settings.

  Furthermore, non-cached version variables \c ALIB_VERSION_NO and \c ALIB_VERSION_REV are
  set which hold the version number and revision number as separate integral values.

- <b>ALIB_CMAKE_VERBOSE</b><br>
  If set to \c true, extended information will be printed with running the \e CMake script.
  See \ref alib_manual_build_more_otherbuildsystems for details.

- <b>ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH</b><br>
  If set to \c true, the provided script will not search for a thread library on the target
  platform. For more information see chapter \ref alib_manual_modules_impact_singlethreaded.

- <b>ALIB_DEBUG_GLIB</b><br>
  If \c true, symbols \b _GLIBCXX_DEBUG, \b _GLIBCXX_DEBUG_PEDANTIC and
  \b _GLIBCPP_CONCEPT_CHECKS are passed to the compiler.

- <b>ALIB_COVERAGE_COMPILE</b><br>
  If \c true, option <b>--coverage</b> is added to CMake variables \b ALIB_COMPILER_OPTIONS and
  \b ALIB_LINKER_OPTIONS



\I{################################################################################################}
### 6.4.4  CMake Build-Setting Variables ###  {#alib_manual_build_cmake_4}
The script will create the following non-cached \e CMake variables, which can be used to define
build-settings of custom projects:

- <b>ALIB_SYMBOLS</b><br>
  Will contain the \ref GrpALibPreproSymbols "ALib preprocessor symbols", as defined by the cached variables.
  This variable can for example be used as a parameter to \e CMake function \c target_compile_definitions().
<p>

- <b>ALIB_COMPILER_WARNINGS</b><br>
  Will contain compiler parameters to set the (high!) warning level used with compiling \alib.
  This variable can for example be used as a parameter to \e CMake function
  \c target_compile_options().<br>
  Custom entries may be added to the list before invoking \b ALib.cmake.
  If entry <em>"ALIB_SUPPRESS_COMPILER_WARNINGS"</em> is found, that entry is removed and no
  compiler-specific warning settings are added to this symbol.
<p>

- <b>ALIB_COMPILER_OPTIONS</b><br>
  Will contain parameters to be passed to the compiler when linking \alib.
  This variable can for example be used as a parameter to \e CMake function
  \c target_compile_options().
<p>

- <b>ALIB_LINKER_OPTIONS</b><br>
  Will contain parameters to be passed to the linker when linking \alib.
  This variable can for example be used as a parameter to \e CMake function
  \c set_target_properties().
<p>

- <b>ALIB_EXTERNAL_LIBS</b><br>
  Will contain a list of external libraries needed to build \alib.
  This variable can for example be used as a parameter to \e CMake function
  \c target_link_libraries().

In addition, the following non-cached variable is an input variable which may be set before
invoking the script:

- <b>ALIB_COMPILER_FEATURES</b><br>
  May contain \e CMake feature parameters to be passed to the compiler.
  This variable can for example be used as a parameter to \e CMake function \c target_compile_features().

\I{################################################################################################}
### 6.4.5  CMake Functions Defined By The Script ###  {#alib_manual_build_cmake_5}
The script will define the following \e CMake functions:

- <b>ALibAddStaticLibrary</b><br>
  This parameterless function creates a static library target called \c ALib_StaticLib.
<p>

- <b>ALibAddSharedLibrary</b><br>
  This parameterless function creates a shared library (under Windows OS a DLL) target called
  \c ALib_SharedLib.
<p>

- <b>ALibSetCompilerAndLinker( target )</b><br>
  This function applies the settings of all variables listed in the previous section to the given
  \p{target}. In addition, the \e target's CMake property \c POSITION_INDEPENDENT_CODE is switched
  on.

\I{################################################################################################}
### 6.4.6  Library Filename ###  {#alib_manual_build_cmake_6}
The library's filename is determined by (non-cached) \e CMake variable \c ALIB_LIBRARY_FILENAME.
If this variable is \b not set before invoking script <c>ALib.cmake</c>, then the name is
determined automatically from the selected modules.

The name is used with functions \c ALibAddStaticLibrary and \c ALibAddSharedLibrary, introduced
in the previous section.

If not provided, the name will be assembled according to the following set of rules:

- The name starts with \c "alib_".
- The library version is appended, for example \c "2412R0".
- In the case of debug-builds, term \c "_DBG" is appended.
- In the case that all modules are selected in the \alibdist, no further rule is applied.

- Otherwise, each selected module's name is added in capital letters, separated by an underscore
  character (<c>'_'</c>) unless a module is not "superseded" by another module that has a
  \ref alib_manual_modules_dependency_graph "mandatory dependency" to it.

- As an exception, in the case that module \alib_strings is included (and no other module
  supersedes it) and furthermore, in the case that \ref ALIB_DEBUG_STRINGS is set,
  suffix <c>"_DBGSTRINGS"</c> is appended instead of <c>"_STRINGS"</c> .

- In the case that \alib_alox is included, the suffixes <c>_ALOX</c> is extended by
  suffix <c>NRL</c> if release logging is switched off. In addition, suffix <c>NDL</c> is added with
  debug-builds that do not include debug logging. Note that both exception cover the
  non-default, unusual compilation cases in respect to the availability of debug- and release logging.


\I{################################################################################################}
## 6.5 More Information ##  {#alib_manual_build_more}

### 6.5.1 Other Build Systems ### {#alib_manual_build_more_otherbuildsystems}

As already explained, the \e CMake build process is viewed to be the reference process for \alib
compilation.

For none\e CMake users, the selection of the exact minimum set of source and header files,
is probably the most difficult (and annoying) task.
In \e CMake we have separated the source selection into script:

            ALIB_BASE_DIR/build/cmake/ALibSources.cmake

which is invoked by the main script.

This script might be analysed to identify the source and header file dependencies of the
different \alibmods.

Likewise, script

            ALIB_BASE_DIR/build/cmake/ALibModules.cmake

might be analysed to get actual (and correct) information about module dependencies.


Furthermore, by setting CMake cache variable \b ALIB_CMAKE_VERBOSE to \c true, running \e CMake
will write extended information that might be used and copied into the configuration files
of other build systems or into IDE project files.

Among the data displayed is:
- List of modules included in the distribution.
  (Modules that the user \ref alib_manual_build_cmake_1 "explicitly selected" plus recursive dependencies)
- List of source and header files that are needed for the build.
- Resulting library filename.
- Base folder of source files.
- The compiler symbols (definitions) passed.
- The compiler warning flags.
- The compiler features and other flags.
- The linker flags
- External libraries used.


\I{################################################################################################}
### 6.5.2 Choosing C++ Language Version ### {#alib_manual_build_more_cppversion}

As demonstrated in the chapter \ref alib_manual_build_sample "A Step-By-Step CMake Sample" (Step 3),
\e CMake variable \c ALIB_COMPILER_FEATURES may be used to determine the C++ language standard for
\alib targets and optionally for custom targets (i.e. if function \c ALibSetCompilerAndLinker is
invoked for a custom target).

\alib requires C++ language level \c 17 as a minimum and is compatible with levels \c 20 and \c 23.
\alib might perform better with higher language levels.

\e CMake provides other (maybe even more preferable) mechanics to determine/set the C++ language
level, which of course may be used alternatively.

\I{################################################################################################}
### 6.5.3 Copying The Sources ### {#alib_manual_build_more_copysources}

If \e CMake variable \c ALIB_SOURCE_COPY_TARGET_DIR is set before invoking CMake script
\c %ALib.cmake, then the source code (compilation units and header files) are copied to the
directory specified in the variable. If the variable does not point to a valid directory, an
error is raised and the \e CMake script is stopped.

The files copied represent exactly the set of files which are needed for compiling the combination
of \alibmods, which are optionally specified with list variable \c ALIB_DISTRIBUTION
(see \ref alib_manual_build_cmake_1).

This feature therefore can be used to create a fresh, filtered copy of the \alib source tree
tailored to an application. Nevertheless, it is usually not recommended to do so, because
source files that are not used by a combination of modules are neither included as header files,
nor compiled. The feature is rather used by the \alib developers to verify module and source code
dependencies.

\I{################################################################################################}
### 6.5.4 External Library Dependencies ### {#alib_manual_build_more_externallibs}

ALib has <b>no mandatory</b> dependencies to external (3rd-party) libraries.
The following \b optional dependencies exist:

1. Boost RegEx<br>
   If enabled by compiler symbol \ref ALIB_FEAT_BOOST_REGEX, a dependency to
   \https{boost,www.boost.org} library component
   \https{regex,www.boost.org/doc/libs/1_68_0/libs/regex/doc/html/index.html} is made.
   As a result, the following \alib features become available:
   - In module \alib_strings, class \alib{strings::util;RegexMatcher}
   - In module \alib_expressions, with expression compiler plugin \alib{expressions::plugins;Strings},
     expression term \b RegExMatch.

2. Standard C++ library<br>
   Some support to make types of the C++ standard library compatible with various concepts of
   \alib is activated by including compatibility header files found in source folder
   \alibsrcdir{compatibility}.<br>
   For more information see the following namespace documentations:
   - #alib::characters::compatibility::std
   - #alib::strings::compatibility::std
   - #alib::boxing::compatibility::std

3. QT Library<br>
   If \alib is used in combination with the \https{QT Class Library,www.qt.io}, compatibility
   support for character array types (strings and character vectors) of the <em>QT Library</em>
   becomes available with modules \alib_boxing and \alib_strings.<br>
   Such support is activated simply by including compatibility header files found in source folder
   \alibsrcdir{compatibility}.<br>
   For more information see the following namespace documentations:
   - #alib::characters::compatibility::qt
   - #alib::boxing::compatibility::qt

\I{################################################################################################}
\I{##########  7.  Using the library                                                               }
\I{################################################################################################}

# 7. Using The Library # {#alib_manual_use}
There is little to say here, so we note just some bullet points: 

- A shortest C++ program was given with chapter \ref alib_manual_bootstrapping.
  From there, a user of the library should start directly with the topic and module of 
  interest. 
- Before releasing \alib, all header files are tested to be includable without the need to 
  include a dependency header. This means, for example, if a user wants to use \alib for scanning
  a directory and working with file trees, the single include of 
  header file <b>"alib/files/fscanner.hpp"</b> would include all necessary dependent header files.
- When compiling code against \alib, each compilation unit needs to receive the same compiler
  symbols as were given to the library itself. 
  With debug-compilations, this will be checked with the invocation of function \alib{Bootstrap}.<br>
  If different compilation symbols are used, then   
- When using \alib with Microsoft Windows OS, it may be important to
  include <em>windows.h</em> before any \alib header file. 
  The reason is that \alib includes <em>windows.h</em> unless it was included already. 
  When the \alib library includes <em>windows.h</em>, it uses some defines like 
  <em>WIN32_LEAN_AND_MEAN</em> or <em>NOGDI</em> to minimize the impact of that huge header file. 
  If <em>windows.h</em> is included before including ALox (AWorx library), it is up to the 
  embedding application which level of windows functionality is needed.

\I{################################################################################################}
\I{##########  Appendices                                                                      }
\I{################################################################################################}
# Appendices # {#alib_manual_appendix}

\I{################################################################################################}
## A.1 Naming Conventions ## {#alib_manual_appendix_naming}

The C++ language standard does not suggest naming rules. This is different to, for example,
with the \c JAVA language. While some people could say, that the C++ standard library naming scheme
suggests such rules, others say that it is especially valuable, that code outside namespace
\c std differs from that - to be better distinguishable within source code.

\alib uses the following rules:
- Preprocessor symbols are "UPPER_SNAKE_CASE" and mostly are prefixed "ALIB_".
- Global variables are also UPPER_SNAKE_CASE.
- Otherwise, underscores (snake_case) are almost never used.
- Namespaces are in lower case letters consisting of single words (no camel casing).
- All other entity names are either lowerCamelCase or UpperCamelCase.
- Public namespace functions are UpperCamelCase.
- Hidden (e.g., anonymous) namespace functions are lowerCamelCase.
- Types on namespace scope are UpperCamelCase.
- Members and inner types are UpperCamelCase if public, otherwise lowerCamelCase.
- Types and members that are available only in debug-compilations, are prefixed \b "Dbg".
- Method and function parameters are lowerCamelCase.
- Templated types UpperCamelCase with a leading <b>'T'</b>, for example \b TString.
  Those template types which are not \ref alib_manual_appendix_typealiases "aliased"
  (like \b TString is aliased to \b String, \b WString, \b NString etc.), often are missing the
  leading <b>'T'</b>. 
  This is for better readability of the end-user code and also to just indicate that the direct use 
  of the templated class is the common use. 
  This is why, for example, class \alib{containers;SharedVal} carries no leading <b>'T'</b>,  
  while class \alib{monomem;TSharedMonoVal} does.   
- Template parameters types are also UpperCamelCase with a leading <b>'T'</b>. Only in seldom cases, a
  template parameter is just named <b>"T"</b>. Usually the name should indicate what kind
  of templated type is expected or what is to be done with it.
- Template structs that might specialized by users of this library are UpperCamelCase with
  a (snake_case) prefix of <b>"T_"</b>. For example, \b T_Boxer or \b T_CharArray.
- Template structs that are \b not to be specialized by users (because they have been specialized
  sufficiently by \alib already) are UpperCamelCase with a prefix of <b>"TT_"</b>.
  This second 'T' could be considered indicating the word "tool", as usually these are tool structs
  that provide a certain information about a types. Examples are found with
  \alib{characters::TT_IsChar}, \alib{strings::TT_IsAppendable} or
  \alib{boxing::TT_IsCustomized}.
         
- Methods and field-members that are available only with debug-builds, are prefixed
  <em>"Dbg"</em>. Often, these members remain available in release-builds, but are empty
  and often static and constexpr. This way they are optimized out by the compiler, while
  their use does not have to be pruned using the preprocessor (i.e. macro \ref ALIB_DBG()
  or symbol \ref ALIB_DEBUG). 

As a final remark, when browsing the code it could be noticed that a lot of block-formatting is used.
While a line width of 100 characters is usually not exceeded, for the sake of building code
blocks, sometimes this line width barrier is ignored. From our perspective, the usefulness of tabbed
code blocks is very much underestimated. Besides readability, a lot of annoying typos can
be detected easily, before a next build run.<br>
In addition, tabbed code blocks often largely improve the ability to quickly refactor a codebase
manually. It should be allowed to mention, that the editor of C++ IDE \b CLion here has some
unrivalled features: Instead of only supporting block selection and copy/paste of blocks, this
editor creates a set of completely new "carets" at the moment block selection starts.
We want to thank the team of [JetBrains](www.jetbrains.com) for supporting \alib development
by providing a free \https{Open Source License,www.jetbrains.com/buy/opensource/}
of their absolutely superb set of IDEs.



\I{################################################################################################}
## A.2 How To Use This Library ## {#alib_manual_appendix_howto}

Most of the time, a 3rd party library like \alib is used to help solving a certain task. Such
tasks are addressed with higher level modules, like \alib_alox, \alib_cli, or \alib_expressions.
In this case, the corresponding module documentation should be read directly, without loosing time
on learning too much about the lower level modules and types.

If you find the lower level types useful for your own projects, those will be understood over
time and by reading the reference documentation of types found in their corresponding namespaces.

Should you really just be interested in general and therefore absolutely not knowing where to start
reading, we recommend to start with some of the module's programming manuals, for example
\alib_boxing or \alib_strings. As debug log-output is something most projects needs, another
recommendation might be the to check out the \ref alib_alox_tut "tutorial of module ALox".

The manuals mentioned above may provide you with a good grasp of the design principles and
features of \alib in general and thus with the value that \alib might bring to your own software
projects.

\warning
  Experienced C++ programmers might complain that the Programmer's Manuals are to lengthy.
  Together they comprise almost <b>500 pages</b>!<br>
  We could now  say that the reason for often being quite "verbose" is that the manual address
  less experienced programmers. But this is not the truth. Explaining the details just helps
  us to understand a module's goals better and leads to a better code design.
  It happens that manual sections are written \e before the corresponding library feature is
  even implemented. Or, that while writing a manual chapter, the already written code gets heavily
  refactored.<br>

\warning
  While we excuse for verbosity, we think that often, the
  \ref alib "reference documentation" of types and entities found in the library provide a
  good way to avoid reading the libraries manuals! :-)

\I{################################################################################################}
## A.3 Type Aliases in Namespace ::alib ## {#alib_manual_appendix_typealiases}
### A.3.1 Why Type Aliases? ###
Once a C++ library becomes more complex, their types get split into different
namespaces and inner namespaces. 
This is also true for \alib, which is split into various modules. 
Consequently, the common need to conveniently address all types is to add several using 
statements to a compilation unit. With \alib this could, for example, look like this:

     using namespace alib::threads;
     using namespace alib::strings;
     using namespace alib::expressions;
     
To avoid the need of the right "permutation" of using statements at the top of a users'
compilation unit, \alib "mirrors" all important types into this outer namespace with type definition
statements. For example, at the end of header file \alibheader{expressions/compiler.hpp},
just before closing outer namespace \b alib, the following statement is made:
\snippet "expressions/compiler.hpp"    DOX_MANUAL_ALIASES_COMPILER
With that, a single:

     using namespace alib;
     
fits for all \alib types and in most cases, this single using statement is all that is
needed. This approach does not generate conflicts: If for example the using code would have
an own class named "Compiler", it can fall back to the standard schematic, by adding just:

     using namespace alib::threads;
     using namespace alib::strings;
     
and accessing class Compiler explicitly as <c>alib::expressions::Compiler</c>.
\note
  The reason why this all is possible is due to (a next great) design decision of C++.
  While the  \em CODE in the following snippet:
         namespace A { namespace B {  CODE } }
\note
  "sees" all types in namespace \c A, with this snippet:
         using namespace A::B;
         CODE
\note
  the \e CODE does not "see" types of namespace \c A, but only those of \c B.
  
### A.3.2 Aliases of Template Types  ###
Most of the type aliases are as simple as sampled with class \b Compiler above.
Also templated types are often just a 1:1 alias, for example:
\snippet "monomem/sharedmonoval.hpp"    DOX_MANUAL_ALIASES_SHAREDMONOVAL

But sometimes template parameters become fixed.
Often (but not always) with that the name changes, i.e., the leading \b T is removed.
As an example, take:
\snippet "files/ftree.hpp"    DOX_MANUAL_ALIASES_FTREE

In some few situations, mostly due to technical reasons, template parameters are even changing
their position. Of-course, some special care has to be taken if such types are used
aliased or not. As an example, take:
\snippet "lang/bitset.hpp"    DOX_MANUAL_ALIASES_BITSET

Finally, often several different aliases are defined in namespace <b>%alib</b>, providing
different permutations of template types.
For example, type #alib::strings;TAString is aliased as:
- \ref alib::AString,
- \ref alib::NString,
- \ref alib::WString,
- \ref alib::AStringMA,
- \ref alib::AStringPA,
- \ref alib::AStringPA,
- \ref alib::NAStringPA, and
- a plethora of more!


\I{################################################################################################}
## A.4 Collecting Caller Information ## {#alib_manual_appendix_callerinfo}
A core class included in any \alibdist is \alib{lang::CallerInfo}. 
It stores the following information:
- Filename, line number, and function name received with corresponding built-in preprocessor
  symbols \b __FILE__, \b __LINE__ and a compiler-dependent symbol which is fetched with
  \ref ALIB_CALLER_FUNC.
- The C++ type id, fetched with keyword \c typeid, usually passing argument <c>*this</c>.
- The C++ thread id of type <c>std::thread::id</c>. This is only included if compiler symbol
  \ref ALIB_EXT_LIB_THREADS_AVAILABLE evaluates to \c true.
  
It is used to collect information about the caller of a function or method member.
\alib collects such information mainly on two occasions:
- For collecting information in debug-builds of the library, and
- with module \alib_alox to make any log output 'clickable' in a developer's IDE and to provide
  log "scopes" which allow enabling or disabling log statements depending on their location.

To ease the type's use, several preprocessor macros are defined. 
To understand their difference, it is important to be aware that, depending on the use-case, 
caller information is either exclusively collected in debug-builds or collected in any build-type.
In the second case, it still should be the user of the library to decide whether
source-code information in a release executable or not. 

With this background thought in mind, the reference documentation of the following
macro list should be enough to fully understand how the data flow is managed and how to 
efficiently use the macros. 

The macros directly relate to class \b CallerInfo:
- \ref ALIB_CALLER
- \ref ALIB_CALLER_PRUNED
- \ref ALIB_COMMA_CALLER_PRUNED
- \ref ALIB_CALLER_NULLED
- \ref ALIB_CALLER_NULLED
- \ref ALIB_DBG_TAKE_CI

The following macros make use of the caller-macros:
- \ref ALIB_ERROR, \ref ALIB_ERROR, \ref ALIB_MESSAGE, and related.
- \ref ALIB_OWN, \ref ALIB_OWN_RECURSIVE, \ref ALIB_OWN_SHARED.
- \ref ALIB_LOCK, \ref ALIB_LOCK_RECURSIVE, \ref ALIB_LOCK_SHARED, and related.
- \ref ALIB_DCS, \ref ALIB_DCS_SHARED, \ref ALIB_DCS_ACQUIRE, and related.
- All macros are used for debug-logging and for release-logging (the latter with optional inclusion
  of caller information), like \ref Log_Info, or \ref Lox_Info. 

\attention 
  The macros fail to work and produce compiler errors if used within static methods or namespace 
  functions.
   
This is because they use keyword <c>this</c> to identify a caller's type. 
Unfortunately, the C++ language in combination with the preprocessor does not allow automatic 
detection of the absence of keyword \c this. 
For this reason the core macro \ref ALIB_CALLER has to be redefined before a code unit 
defines functions and restored back afterward. 
To do this, header files \alibheader{lang/callerinfo_functions.hpp} and 
\alibheader{lang/callerinfo_methods.hpp} are provided. 
The following example demonstrates how they are to be used:

  \snippet "ut_lang.cpp"    DOX_LANG_CALLER 

\I{################################################################################################}
## A.5 T.HPP-Files And Custom Template Instantiations ## {#alib_manual_appendix_t_hpp_files}

\I{################################################################################################}
### A.5.1 The Challenge and ALib's Solution ### {#alib_manual_appendix_t_hpp_files_challenge}
In C++, templates offer a powerful mechanism for writing generic and reusable code.
But using templates extensively in header files leads to longer compilation times and
potential code bloat, particularly when templates are instantiated multiple times across different
translation units.
The problem becomes significant in larger projects where a template may be used with many types or
included in multiple source files, forcing the compiler to repeatedly generate code for the same
template.

To mitigate these issues, with a few template types, \alib separates
1. template declarations,
2. a subset of that template's method definitions, and
3. instantiations for combinations of template types,

into three different files. These files are:

1. A <b>HPP-file</b> (.hpp) for template class declarations.<br>
   This header is missing the definition of those methods that should not be compiled with a
   unit that does not instantiate a templated type with a specific combination
   of template parameters.<br>
   In other words, this header is to be included by code that relies on the fact that a different
   compilation unit has instantiated that type already.
2. A <b>T.HPP-file</b> (.t.hpp) providing the missing (and and not inlined) method definitions.<br>
3. A <b>CPP-file</b> (.cpp) that includes both, the declaration and definition files, and explicitly
   instantiates the template for specific, commonly used types.

This approach results in improved compilation speed, code modularity, and manageable build times
while still allowing users the flexibility of creating custom instantiations when necessary.

\I{################################################################################################}
### A.5.2 Instantiations of Custom Types ### {#alib_manual_appendix_t_hpp_files_customize}
With this recipe in place, it is fairly straight forward how to instantiate your custom
version of an \alib-type of that sort. Let's quickly look at a sample.

For class #alib::monomem::TMonoAllocator, provided with module \alib_monomem, only one built-in
template instantiation is provided.
This specifies the single template parameter \p{TAllocator} to be of type
\alib{lang;HeapAllocator}. This instantiation is then aliased as #alib::MonoAllocator.<br>
The corresponding files are:

- \alibheader{monomem/monoallocator.hpp},
- \alibheader{monomem/monoallocator.t.hpp}, and
- \alibheader{monomem/monoallocator.cpp}

Let us look at the compilation unit (the \b CPP-file):

\snippet "alib/monomem/monoallocator.cpp"        ALIB_MANUAL_T_HPP_FILE_USAGE

That is all that the CPP-file needs to contain.

With this in mind, we can now instantiate a custom mono-allocator, one that uses a \b PoolAllocator
as its memory source. This is done in the \alib unit-tests. Here is the excerpt:

\snippet "containers_monomem/ut_monomem.cpp"               ALIB_MANUAL_T_HPP_FILE_USAGE2

With this in place, we can start using the instantiation:

\snippet "containers_monomem/ut_monomem.cpp"               ALIB_MANUAL_T_HPP_FILE_USAGE3


To summarize, the fast track approach when a specific instantiation of an \alib type that
splits some method definitions from the type declaration is needed, is to have a look at the
compilation unit (the CPP-file), copy its code to an own compilation unit, and replace the given type
with your own type. 

If this type is needed in different places, create your custom header file and copy the 
instantiation code and add keyword <c>extern</c> to turn it into a declaration. 
This announces that the linker will find the instantiation.
Such a header file has to include only the HPP-file that \alib provides, and not the "T.HPP"-file.

\attention
  With WindowsOS and DLL-compilation, things may become more complicated. 
  Here, compiler-symbol \ref ALIB_API has to be set rightfully for the code-unit that implements
  an instantiation. 
  This manual cannot go into further details of this topic. 
  Instead general information on compiling and linking under WindowsOS has to be consulted.     

\I{################################################################################################}
### A.6  Pretty Printers ### {#alib_manual_appendix_t_prettyprinters}
For gdb (GNU Debugger), some "pretty printers" are available. Please consult page
\ref alibtools_debug_helpers_gdb.

<br><br><br><br><br><br> */
