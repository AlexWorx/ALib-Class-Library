// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/**
\mainpage    Overview

## Abstract ##

\alib is a general purpose, use-case agnostic, platform-independent, low-level C++ class library.

Its mission is to provide foundational concepts, types and idioms relevant to any C++ project.
As of today, \alib consists of \ref alib_manual_modules_dependencies "15 modules", each addressing
different topics.<br>
A subset of the available modules can be selectively included in a custom library build.
This means, you just get what you choose from the menu.

Some highlights of the functionality:
- \alib_strings: String-types with interfaces similar to Java/C#, compatible with anything that "smells" like a string
  (due to some template meta programming magic).
- \alib_boxing: Consider this "std::any on steroids".
- \alib_enums: Finally, we get what we expected from C++ enums.
- \alib_monomem: Monotonic allocation with recycling. Why use the oh-so-slow heap?
- \ref alib_mod_alox "ALib ALox": Logging for adults.
- \alib_bitbuffer: Write bit-streams instead of human-readable text.
- \alib_cli: Command line parser with support of environment variables and configuration files.
- \alib_expressions: Type-safe run-time expression compiler. Easily extensible to support your
  custom expression functions. 130+ (optional) predefined functions (math, string compare, date/time, etc.)
- \alib_config: Run-time variables for C++. 
  Its priority-management allows hard-coding defaults and having them be overridden by
  configuration files, environment-variables or command-line parameters.  
- \alib_files: A directory and file scanner (with run-time expression support).
- \alib_threadmodel: A dynamic thread-pool implementation, dedicated worker threads with prioritized
  job-management, and periodic event-triggering.
- And last but not least: Many more tools like managing bootstrapping of C++ software, externalized resources,
  configuration data, singletons with Windows DLLs, ...

\anchor mainpage_goals
## Main Characteristics And Design Goals ##

- \alib is <b>free software</b>.
- Compiles and tested with <b>C++ 17, 20 and  23</b>.
- \b Modularization: Possible selective use of only parts of the library.
- Extensive \b documentation.
- <b>Least intrusive</b>: Designed to keep user code as independent of \alib types and idioms as possible.
- <b>Ease of use</b> by design.<br>
  When things become complex, features are hidden behind default
  behavior and are only available to users who read the detail chapters of the various Programmer's Manuals.  \emoji :sweat_smile:
- Developed and steadily tested under <b>GNU/Linux</b>, <b>Windows OS</b>, <b>Mac OS</b>, and
  <b>Raspberry</b>. For details in respect to the current release, see section below.

## Documentation ##

The following documentation is provided.

1. A \ref alib_manual "General Library Manual" is available describing the library structure,
   its setup and compilation, bootstrapping, etc.

2. \b Fifteen separated \ref alib_manual_modules_overview "Programmer's Manuals" are published
   on this website! (One for each \alibmod_nl).<br>
   The manuals are well-structured, provide <b>step-by-step source-code tutorials</b> and 
   often go into in-depth discussions in respect to design decisions and overall
   rationals behind important features of the \alibmods.

3. \ref alib "Reference Documentation" which is covering 100% of the provided types, members,
   functions, namespaces, macros, etc. In short, <b>each and every C++ entity is documented</b>.

4. A detailed version history, documenting every interface change is found in the \ref alib_changelog.

5. All documentation provided leverages the full power of \https{Doxygen,doxygen.nl} (the industry standard).
   Therefore, changes in the library's interfaces are always detected and corrected.
   The many <b>code samples</b> are implemented as unit tests and thus are "live"-integrated
   to the documentation, often together with the output text snippets of the recent unit test run.
   (See \ref alib_expressions_calculator "an example here".)

\note
   Summary: \alib comes with a <b>complete book of documentation</b>, which has more than
   <b>1000 pages</b> if printed. Its all explained, for beginners and experts!

## IDE / Build System Setup ##
The current release got tested on the following platform combinations:
- GNU/Linux Arch 6.12.1, GNU C++ 14.2.1 / Clang++ 18.1.8, C++ 17/20/23, 32-Bit / 64-Bit<br>
  (This is the main development platform.)
- WindowsOS 10/11, MSC 19.42 (Visual Studio 2022), C++ 17/20, 32-Bit / 64-Bit
- WindowsOS 10/11, MinGW, GCC 13.47  C++ 17/20, 64-Bit 
- macOS Sequoia 15.2, Apple M2 / ARM64, Apple Clang Version 16.0.0, C++ 17/20/23, 64-Bit
- Raspberry 3, ARM, 64-bit OS, GNU C++ 12.2.0, C++ 17/20/23
- Raspberry 4, ARM, 64-bit OS, GNU C++ 12.2.0, C++ 17/20/23

Check out chapter \ref alib_manual_build "6. Building The Library" for details of how to compile
and use ALib in your C++ environment.

##  C# and Java Versions Of ALib ##

Parts of \alib have sibling incarnations in programming languages C# and JAVA. Historically,
\alib aimed to be a cross platform/cross language library. This goal was (mostly) dropped
in favor to be able to independently develop <b>ALib for C++</b>.

<b>ALib for C#</b> and <b>ALib for Java</b> are included in and distributed with the
cross platform \https{ALox Logging Library,alexworx.github.io/ALox-Logging-Library/}.

For C++, since May 2018 release, the situation was inverted: Instead of having \alib bundled
with \alox, the logging library is now an \alibmod.

\anchor mainpage_license
## License ##

\alib is free software, can be \https{downloaded at Github,github.com/AlexWorx/ALib-Class-Library}
and sources are published under Boost Software License:

\verbinclude "../../LICENSE.txt"

## Library Dependencies ##

\alib compiles as is, hence it is <b>not dependent on any 3rd-party library</b>.
Optional dependencies exist. For example if \https{boost,www.boost.org} is available,
\alib_strings_nl are using its regex search.<br>
In contrast, \alib provides optional compatibility headers for 3rd-party libraries
(e.g., \https{QT Class Library,www.qt.io}), which for example provide adoptions of \alib type-traits
for QT-types.

##  Contributions ##
We would be happy, if the community started to support this library and are willing to receive
contributions and, if accepted, to update the code accordingly.<br>
Note that for legal reasons, we hereby explicitly refuse and reject code (or ideas for code)
that are not explicitly provided under the Boost Software License.
Please do not even contact us with your ideas/code in that case.


## Thanks ##

Our thanks go to all supporters that did and do help realizing this project. Furthermore
to just all of these millions of supporters of *free software*, including:
- The \https{GNU/Linux,gnu.org} project and community,
- The \https{LLVM/Clang,llvm.org/} project,
- The \https{CMake,cmake.org/} project,
- To <b>Dimitri van Heesch</b> for providing marvelous documentation software \https{Doxygen,doxygen.nl}.
- To company \https{JetBrains,www.jetbrains.com} for providing a free
  \https{Open Source License,www.jetbrains.com/buy/opensource/}
  of their absolutely superb and unrivalled set of IDEs for various programming languages.
- The \https{QT Creator,doc.qt.io/qtcreator} team,
- Microsoft for providing \https{Visual Studio Community Edition,www.visualstudio.com/vs/community/} project,
- \https{Uniki GmbH,uniki.de} for supporting cross-platform compatibility tests.

Special thanks also to C. Darwin, who created life on earth hundreds of millions of years ago,
until he - when things became too crazy - disappeared in 1882.


\I{################################################################################################}
\I{################################################################################################}
\I{######################################   DOWNLOAD Page     #####################################}
\I{################################################################################################}
\I{################################################################################################}
\page alib_download    How to Download

\alib can be downloaded via the following link:
[ALib C++ Library on <img src=github-mark.png height=20 width=20> GitHub](https://github.com/AlexWorx/ALib-Class-Library)

Note that GitHub is the perfect tool to add your contributions!


\note
    A similar, but less feature rich <b>version for C# and Java</b> is available as well:
    [ALox Logging Library on <img src=github-mark.png height=20 width=20> GitHub](https://github.com/AlexWorx/ALox-Logging-Library)
                                                                                 
\I{################################################################################################}
\I{################################################################################################}
\I{########################################   Legal Page     ######################################}
\I{################################################################################################}
\I{################################################################################################}
\page alib_legal_notice Legal Notice
<IMG SRC="legal-notice.png"/>
<br><br><br><br><br><br> */
