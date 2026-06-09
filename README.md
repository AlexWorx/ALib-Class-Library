## Abstract ##
The **ALib C++ Framework** provides industrial-strength infrastructure components for modern 
C++20 applications, including logging, formatting, memory management, configuration systems, 
expression parsing, and diagnostics.

Its mission is to provide reusable infrastructure, diagnostics, utilities,
and development patterns relevant to modern C++ applications.
                   
**ALib** is modular and subsets of the available modules can be selectively included in 
custom builds of the framework.

## Why ALib?
ALib was created as an alternative to assembling large C++ projects from dozens of disconnected 
micro-libraries. It is a cohesive infrastructure layer for C++20 applications.

The focus is:
- high runtime efficiency
- low allocation overhead
- modular integration (choose what you need)
- cross-platform consistency
- deep debug capabilities
                               
**ALib** combines capabilities typically spread across libraries such as
spdlog, fmt, Boost, CLI11, magic_enum, Abseil, folly, exprtk,
and dedicated diagnostics, configuration, allocator, and utility frameworks.
For many C++20 applications, **ALib** might be the better-integrated alternative.
                                      
ALib is particularly suited for:
- small and large native C++ applications
- tools and infrastructure software
- performance-sensitive systems
- long-lived enterprise codebases
- diagnostic-heavy development environments
 
## New In Version 2605 ##
Introduced the new module [ALib App](https://alib.dev/alib_mod_app.html) which aggregates several
features of lower-level modules and orchestrates bootstrapping. 
With this addition, we felt it was appropriate to rename **ALib** from a "class library" to 
a "framework".
Furthermore, the optional C++20-Module compilation is back. Nevertheless, it is only supported on
a GNU/Linux platform with the newest Clang compiler version. 
We are waiting for GCC to relax its currently restrictive interpretation of the standard.

In addition to many smaller improvements, we added an external resource format definition and  
introduced a corresponding resource compiler that writes C++ code.
                                                                        
All details of changes are provided in the [Changelog](https://alib.dev/alib_changelog.html). 

## ALib Homepage ##
Please find all about the **ALib For C++**, including

* **Programmer's Manuals**
* **Reference Documentation**
* **Tutorials**
* **Project setup guide**
* **Version History / Change log**

at the [ALib Homepage](https://alib.dev).

## A Quick Sample ##
```cpp
#include "ALib.Bootstrap.H"
#include "ALib.Lang.CIFunctions.H"
#include "ALib.Expressions.H"

int main( int, const char ** ) {
    alib::Bootstrap();
 
    alib::Compiler compiler;
    compiler.SetupDefaults();

    alib::Expression expression = compiler.Compile( "40+2 * 3-4" );
 
    Log_Info( "Input:      {!Q}", expression->GetOriginalString()   )
    Log_Info( "Normalized: {!Q}", expression->GetNormalizedString() )
    Log_Info( "Optimized:  {!Q}", expression->GetOptimizedString()  )

    alib::expressions::Scope scope( compiler.CfgFormatter );
    Log_Info( "Result:     "    , expression->Evaluate( scope )     )
 
    alib::Shutdown();
    return 0;
}
```
Produces the following output:
```
sample.cpp:13: main [0.000 +---   ][PROCESS][/] #002: Input:      "40+2 * 3-4"
sample.cpp:14: main [0.000 +---   ][PROCESS][/] #003: Normalized: "40 + (2 * 3) - 4"
sample.cpp:15: main [0.000 +---   ][PROCESS][/] #004: Optimized:  "42"
sample.cpp:18: main [0.000 +---   ][PROCESS][/] #005: Result:     42
```
     
This small example already demonstrates several core **ALib** capabilities:
- bootstrapping and lifecycle management
- expression parsing (extensible with custom functions and context variables)
- logging 
- formatting
- boxing (much more than std::any provides)
- type erasure
- runtime diagnostics


## Some highlights of the functionality: ##
- [ALib Strings](https://alib.dev/alib_mod_strings.html): String types with interfaces similar to Java/C#, compatible with anything 
  that "smells" like a string. (Technically achieved using C++20 concepts).
- [ALib Boxing](https://alib.dev/alib_mod_boxing.html): Consider this "std::any on steroids".
- [ALib Enums](https://alib.dev/alib_mod_enums.html): Finally, we get what we expected from C++ enums.
- [ALib Monomem](https://alib.dev/alib_mods_contmono.html): Monotonic allocation with recycling. Why use the oh-so-slow heap?
- [ALib ALox](https://alib.dev/alib_mod_alox.html): High-performance logging with deep diagnostics support.
- [ALib App](https://alib.dev/alib_mod_app.html): Applications with a well-structured execution design and command-line parsing.
- [ALib Bitbuffer](https://alib.dev/alib_mod_bitbuffer.html): Write bit-streams instead of human-readable text.
- [ALib Expressions](https://alib.dev/alib_mod_expressions.html): Type-safe runtime expression compiler. Easily extensible to support your 
  custom expression functions. 130+ (optional) predefined functions (math, string compare, date/time, etc.)
- [ALib Variables](https://alib.dev/alib_mod_variables.html): Runtime variables for C++. 
  Its priority-management allows hard-coding defaults and having them be overridden by
  configuration files, environment-variables or command-line parameters.  
- [ALib FileTree](https://alib.dev/alib_mod_filetree.html): A directory and file scanner (with runtime expression support)
- [ALib Format](https://alib.dev/alib_mod_format.html): Formatting of strings with Python-like and alternatively Java-like syntax.
- [ALib Threads](https://alib.dev/alib_mod_threads.html): Thread diagnostics and deadlock detection. 
- [ALib ThreadModel](https://alib.dev/alib_mod_threadmodel.html): A dynamic thread-pool implementation, 
  dedicated worker threads with prioritized job-management, and periodic event triggering.
- And last but not least: Many more tools like managing bootstrapping of C++ software, externalized resources,
  configuration data, singletons with Windows DLLs, ...  

## IDE / Build System Setup ##
The current version was tested on the following platform combinations:
- GNU/Linux Arch 7.0.5, Clang++ 21.1.5, C++20/23, 32-Bit / 64-Bit<br>
  (This is the main development platform.)
- GNU/Linux Arch 7.0.5, GNU C++ 16.1.1, C++20/23, 32-Bit / 64-Bit
- WindowsOS 11, MSC 19.51 (Visual Studio 2026 Insiders), C++20, 32-Bit/64-Bit
- WindowsOS 11, MinGW, 64-Bit, GCC 13.47, C++20
- macOS Tahoe 26.4.1, Apple M2 / ARM64, Apple Clang Version 21.0.0, C++20/23, 64-Bit
- Raspberry 3, aarch64, Cortex-A53, GNU C++ 12.2.0, C++20/23
- Raspberry 4, aarch64, Cortex-A72, GNU C++ 14.2.0, C++20/23
- Raspberry 4, armhf (32-bit), Cortex-A72, GNU C++ 12.2.0, C++20/23

The Programmer's Manual contains an extensive chapter about how to compile and use **ALib** in your 
C++ environment.

## Documentation ##

The following documentation is provided:

1. A [General Framework Manual](https://alib.dev/alib_manual.html) is available describing 
   **ALib**'s structure, its compilation, bootstrapping, etc.

2. Separated [Programmer's Manuals](https://alib.dev/alib_manual.html#alib_manual_modules_table) are published with the [ALib Homepage](https://alib.dev)! 
   One dedicated manual is provided for each **ALib** Module. 
 
   The manuals are well-structured, provide **step-by-step source code tutorials** and sometimes go 
   into in-depth discussion in respect to design decisions and overall rationales behind important 
   features of the ALib Modules. 

3. [Reference Documentation](https://alib.dev/annotated.html) covering 100% of the provided types, members,
   functions, namespaces, macros, etc. In short, **each and every C++ entity is documented**.

4. A detailed version history, documenting every interface change is found in the [Changelog](https://alib.dev/alib_changelog.html).

5. All documentation provided leverages the full power of [Doxygen](https://doxygen.nl)
   (the industry standard).

   Therefore, changes in the library's interfaces are always detected and corrected.
   Most **code samples** are implemented as unit tests and thus are tested when compiling
   the documentation. This is also mostly true for the output text of the code samples.
   (See  [an example here](https://alib.dev/alib_mod_expressions.html#alib_expressions_calculator)).

Summary: **ALib** comes with a **complete book of documentation**, which has more than
**1000 pages** if it was printed. It is all explained, for beginners and experts!

## License ##
**ALib** is free software and can be 
[downloaded at Github](https://github.com/AlexWorx/ALib-Class-Library).

Starting with version 2605, **ALib** is licensed under the [MIT License](LICENSE).

The change from the Boost Software License 1.0 to the MIT License is intended
to simplify future integration of third-party MIT-licensed components and
adaptations within **ALib**.

Previous releases remain available under the Boost Software License 1.0.

## Main Characteristics And Design Goals ##

- **ALib** is **free software**.
- Compiles with standards **C++20 and 23**.
- **Modularization**: Possible selective use and compilation of the Framework.
- Extensive **documentation**.
- **Least intrusive**: Designed to keep user code independent of **ALib** types and idioms when possible.
- **Ease of use** by design.
  When things become complex, features are hidden behind default
  behavior and are only available to users who investigate deeper. 
  For example, by consulting the detailed chapters of the Programmer's Manuals. 
  There is one manual available for each module. 
- Developed and steadily tested under **GNU/Linux**, **Windows OS**, **macOS**, and
  **Raspberry Pi** devices. For details in respect to the current release, see the section below. 

## No Dependencies ##

**ALib** builds without mandatory third-party dependencies.
Optional integrations exist. For example, if [boost](https://www.boost.org) is available,
**ALib Strings** are using its regular-expression searches.
In contrast, **ALib** provides optional compatibility headers for 3rd-party libraries
(e.g., [QT Class Library](https://www.qt.io)), which, for example, provide adaptations of 
**ALib** type-traits for QT-types.

##  Contributions ##
We are happy about community input and contributions.
For legal clarity, contributions must be provided under the MIT License
(or a more permissive license). 

## C# and Java Versions Of ALib ##
Historically, parts of ALib were also developed for C# and Java.
Those projects are currently unmaintained but remain available as reference
implementations and experimental sibling projects.   

The primary and actively maintained version is ALib for C++.         

**ALib for C#** and **ALib for Java** are included in and distributed with the
cross-platform [ALox Logging Library](https://alexworx.github.io/ALox-Logging-Library/).

## Thanks ##
Our thanks go to all supporters that did and do help to realize this project. 
Furthermore, to just all of these millions of supporters of *free software*, including:
- The [GNU/Linux](https://gnu.org) project and community,
- The [LLVM/Clang](http://llvm.org/) project,
- The [CMake](https://cmake.org/) project,
- To **Dimitri van Heesch** for providing marvelous documentation software [Doxygen](http://doxygen.nl).
- To company [JetBrains](https://www.jetbrains.com) for providing 
  [free licenses](https://www.jetbrains.com/buy/opensource/) to open source developers
  of their absolutely superb and unrivalled set of IDEs for various programming languages.
- The [QT Creator](http://doc.qt.io/qtcreator) team,
- Microsoft for providing [Visual Studio Community Edition](https://www.visualstudio.com/vs/community/) project,
- [Uniki GmbH](https://uniki.de) for supporting cross-platform compatibility tests.

## All Information ##
Please visit the [ALib Homepage](https://alib.dev)!

