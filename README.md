## Abstract ##

**ALib** is a general purpose, use-case agnostic, platform-independent, low-level C++ class library.

Its mission is to provide foundational concepts, types and idioms relevant to any C++ project.
As of today, **ALib** consists of **25 modules**, each addressing
different topics.
A subset of the available modules can be selectively included in a custom library build. 
This means you only get what you choose from the menu.
                                        

## New In this Version ##
Support of language version C++17 was dropped. With that:
- All **std::enable_if** constructs (and similar) have been replaced it by **C++20 Concepts**.  
- Support for **C++20 Modules** was added. (This is in alpha state and works only on newest Clang compilers).

The latter was a little bit of a journey, and we have created a 
[blog-page](https://alib.dev/alib_c20module_shift.html) describing our experiences. 
                                                                         
For more news, consult the [Changelog](https://alib.dev/alib_changelog.html). 

## ALib Homepage ##
Please find all about the **ALib For C++**, including

* **Programmer's Manuals**
* **Reference Documentation**
* **Tutorials**
* **Project setup guide**
* **Version History / Change log**

at the [ALib Homepage](https://alib.dev).

## Some highlights of the functionality: ##
- [ALib Strings](https://alib.dev/alib_mod_strings.html): String-types with interfaces similar to Java/C#, compatible with anything 
  that "smells" like a string. (Technically achieved with template programming magic).
- [ALib Boxing](https://alib.dev/alib_mod_boxing.html): Consider this "std::any on steroids".
- [ALib Enums](https://alib.dev/alib_mod_enums.html): Finally, we get what we expected from C++ enums.
- [ALib Monomem](https://alib.dev/alib_mods_contmono.html): Monotonic allocation with recycling. Why use the oh-so-slow heap?
- [ALib ALox](https://alib.dev/alib_mod_alox.html): Logging for adults.
- [ALib Bitbuffer](https://alib.dev/alib_mod_bitbuffer.html): Write bit-streams instead of human-readable text.
- [ALib CLI](https://alib.dev/alib_mod_cli.html): Command line parser with support of environment variables and configuration files.
- [ALib Expressions](https://alib.dev/alib_mod_expressions.html): Type-safe run-time expression compiler. Easily extensible to support your 
  custom expression functions. 130+ (optional) predefined functions (math, string compare, date/time, etc.)
- [ALib Variables](https://alib.dev/alib_mod_variables.html): Run-time variables for C++. 
  Its priority-management allows hard-coding defaults and having them be overridden by
  configuration files, environment-variables or command-line parameters.  
- [ALib Files](https://alib.dev/alib_mod_files.html): A directory and file scanner (with run-time expression support)
- [ALib Format](https://alib.dev/alib_mod_format.html): Formatting of strings with Python-like and alternatively Java-like syntax.
- [ALib ThreadModel](https://alib.dev/alib_mod_threadmodel.html): A dynamic thread-pool implementation, 
  dedicated worker threads with prioritized job-management, and periodic event-triggering.
- And last but not least: Many more tools like managing bootstrapping of C++ software, externalized resources,
  configuration data, singletons with Windows DLLs, ...  

## Main Characteristics And Design Goals ##

- **ALib** is **free software**.
- Compiles with standards **C++20 and 23**.
- **Modularization**: Possible selective use and compilation of the library.
- Extensive **documentation**.
- **Least intrusive**: Designed to keep user code independent of **ALib** types and idioms when possible.
- **Ease of use** by design.
  When things become complex, features are hidden behind default
  behavior and are only available to users who investigate deeper. 
  For example, by consulting the detail chapters of the Programmer's Manuals. There is one available 
  for each module. 
- Developed and steadily tested under **GNU/Linux**, **Windows OS**, **macOS**, and
  **Raspberry Pi**-devices. For details in respect to the current release, see the section below. 

## Documentation ##

The following documentation is provided:

1. A [General Library Manual](https://alib.dev/alib_manual.html) is available describing the library structure,
   its setup and compilation, bootstrapping, etc.

2. Separated [Programmer's Manuals](https://alib.dev/alib_manual.html#alib_manual_modules_table) are published with the [ALib Homepage](https://alib.dev)! 
   One dedicated manual is provided for each ALib Module. 
 
   The manuals are well-structured, provide **step-by-step sourcecode tutorials** and sometimes go 
   into in-depth discussion in respect to design decisions and overall rationals behind important features
   of the ALib Modules.

3. [Reference Documentation](https://alib.dev/annotated.html) which is covering 100% of the provided types, members,
   functions, namespaces, macros, etc. In short, **each and every C++ entity is documented**.

4. A detailed version history, documenting every interface change is found in the [Changelog](https://alib.dev/alib_changelog.html).

5. All documentation provided leverages the full power of [Doxygen](https://doxygen.nl)
   (the industry standard).

   Therefore, changes in the library's interfaces are always detected and corrected.
   The many **code samples** are implemented as unit tests and thus are "live"-integrated
   to the documentation, often with the output text snippets of the recent unit test run.
   (See  [an example here](https://alib.dev/alib_mod_expressions.html#alib_expressions_calculator)).

Summary: **ALib** comes with a **complete book of documentation**, which has more than
**1000 pages** if it was printed. It is all explained, for beginners and experts!


## IDE / Build System Setup ##
The current Version got tested on the following platform combinations:
- GNU/Linux Arch 6.16.8, Clang++ 20.1.8, C++20/23, 32-Bit / 64-Bit, optional **C++20 Module Support**<br>
  (This is the main development platform.)
- GNU/Linux Arch 6.16.8, GNU C++ 15.2.1, C++20/23, 32-Bit / 64-Bit
- WindowsOS 11, MSC 19.44 (Visual Studio 2026 Insiders, Platform v145), C++20, 32-Bit/64-Bit
- WindowsOS 11, MinGW, 64-Bit, GCC 13.47, C++20
- macOS Tahoe 26.0, Apple M2 / ARM64, Apple Clang Version 17.0.0, C++20/23, 64-Bit
- Raspberry 3, aarch64, Cortex-A53, GNU C++ 12.2.0, C++20/23   
- Raspberry 4, aarch64, Cortex-A72, GNU C++ 12.2.0, C++20/23   
- Raspberry 4, armhf (32-bit), Cortex-A72, GNU C++ 12.2.0, C++20/23 

The Programmer's Manual contains an extensive chapter about how to compile and use ALib in your 
C++ environment.

## C# and Java Versions Of ALib ##

Parts of **ALib** have sibling incarnations in programming languages C# and JAVA. Historically,
**ALib** aimed to be a cross platform/cross language library. This goal was (mostly) dropped
in favor to be able to independently develop **ALib for C++**.

**ALib for C#** and **ALib for Java** are included in and distributed with the
cross platform [ALox Logging Library](https://alexworx.github.io/ALox-Logging-Library/).

## License ##

**ALib** is free software, can be [downloaded at Github](https://github.com/AlexWorx/ALib-Class-Library)
and sources are published under [Boost Software License](LICENSE.txt)

## Library Dependencies ##

**ALib** compiles as is, hence it is **not dependent on any 3rd-party library**.
Optional dependencies exist. For example, if [boost](https://www.boost.org) is available,
**ALib Strings** are using its regex-search.
In contrast, **ALib** provides optional compatibility headers for 3rd-party libraries
(e.g., [QT Class Library](https://www.qt.io)), which, for example, provide adoptions of **ALib** type-traits
for QT-types.

##  Contributions ##
We would be happy if the community started to support this library and are willing to receive
contributions and, if accepted, to update the code accordingly.

Note that for legal reasons, we hereby explicitly refuse and reject code (or ideas for code)
that are not explicitly provided under the Boost Software License.
Please do not even contact us with your ideas/code in that case.


## Thanks ##

Our thanks go to all supporters that did and do help to realize this project. 
Furthermore, to just all of these millions of supporters of *free software*, including:
- The [GNU/Linux](https://gnu.org) project and community,
- The [LLVM/Clang](http://llvm.org/) project,
- The [CMake](https://cmake.org/) project,
- The [QT Creator](http://doc.qt.io/qtcreator) team,
- Microsoft for providing [Visual Studio Community Edition](https://www.visualstudio.com/vs/community/) project,
- To company [JetBrains](https://www.jetbrains.com) for providing a 
  [free license to open source developers](https://www.jetbrains.com/buy/opensource/)
  of their absolutely superb and unrivalled set of IDEs for various programming languages.
- To **Dimitri van Heesch** for providing marvelous documentation software [Doxygen](http://doxygen.nl).
- [Uniki GmbH](https://uniki.de) for supporting cross-platform compatibility tests.

Special thanks also to C. Darwin, who created life on earth hundreds of millions of years ago,
until he - when things became too crazy - disappeared in 1882.

## All Information ##
Please visit the [ALib Homepage](https://alib.dev)!


