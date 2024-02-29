## ALib Homepage ##
Please find all about the **ALib For C++**, including

* **Programmer's Manuals**
* **Reference Documentation**
* **Tutorials**
* **Project setup guide**
* **Version History / Change log**

at the [ALib Homepage](https://alib.dev).

## Abstract ##

**ALib** is a general purpose, use-case agnostic, platform independent, low-level C++ class library.

Its mission is to provide foundational concepts, types and idioms relevant to any C++ project.
As of today, **ALib** consists of **15 modules**, each addressing
different topics.
A subset of the available modules can be selectively included in a custom library build. 
This means, you just get what you choose from the menu.

Some highlights of the functionality:
- ALib Strings: String types with interfaces similar to Java/C#, compatible with anything that "smells" like a string
  (due to some template meta programming magic).
- ALibBoxing: Consider this "std::any on steroids".
- ALib Enums: Finally, we get what we expected from C++ enums.
- ALib Monomem: Monotonic allocation with recycling. Why use the oh-so-slow heap?
- ALib ALox: Logging for adults.
- ALib Bitbuffer: Write bit-streams instead of human readable text.
- ALib CLI: Command line parser with support of environment variables and configuration files.
- ALib Expressions: Type-safe run-time expression compiler. Easily extensible to support your 
  custom expression functions. 130+ (optional) predefined functions (math, string compare, date/time, etc.)
- ALib Files: A directory and file scanner (with run-time expression support)
- And last but not least: Many more tools like managing bootstrapping of C++ software, externalized resources,
  resources, configuration data, singletons with Windows DLLs, ...  

## Main Characteristics And Design Goals ##

- **ALib** is <b>free software**.
- Compiles and tested with **C++ 17 and  20**.
- **Modularization**: Possible selective use of only parts of the library.
- Extensive **documentation**.
- **Least intrusive**: Designed to keep user code as independent from **ALib** types and idioms as possible.
- Developed and steadily tested under
  - **GNU/Linux**,
  - **Windows OS**,
  - **Mac OS**, and
  - **Raspberry**
 

## Library Dependencies ##

**ALib** compiles as is, hence it is **not dependent on any 3rd-party library**.
Optional dependencies exist. For example if [boost](https://www.boost.org) is available,
**ALib Strings** are using its regex search.<br>
In contrast, **ALib** provides optional compatibility headers for 3rd-party libraries
(e.g. [QT Class Library](https://www.qt.io)), which for example provide adoptions of **ALib** type-traits
for QT-types.

## Documentation ##

The following documentation is provided.

1. [Reference Documentation](https://alib.dev/annotated.html), ** covering 100% of the types, 
   members, functions, namespaces, macros  and other C++ entities.

2. [Programmer's Manuals](https://alib.dev/alib_manual.html#alib_manual_modules_overview) are 
   available for each module of the library.<br>
   Together more than **500 pages** of well structured in-depth discussion of library
   features as well as **step by step tutorials** are available.

3. Page [Changelog](https://alib.dev/alib_changelog.html) provides a detailed version history.

4. A separated [General Manual](https://alib.dev/alib_manual.html) is available describing the 
   library structure, project setup, bootstrapping, etc.

5. 100% complete [Reference documentation](https://alib.dev/alib_ref_prepro.html) for C++ compiler symbols and macros.
   (Probably another 500 pages, next to the Programmer's Manuals.)

6. Tutorial and reference documentation's ** code samples** are implemented as unit tests and
   are integrated as source and output text snippets into the documentation.<br>
   (As for example [done here](https://alib.dev/alib_mod_enums.html#alib_enums_records)).
   

## IDE / Build System Setup ##
The current Version got tested on the following platform combinations:
- GNU/Linux Arch 6.7.6, GNU C++ 13.2.1 / Clang++ 16.0.6, C++ 17/20, 32-Bit / 64-Bit<br>
  (This is the main development platform.)
- Windows 10/11, MSC 17.8.3 (Visual Studio 2022), C++ 17/20, 32-Bit / 64-Bit
- Windows 10/11, MinGW,  C++ 17/20, 64-Bit
- MacOS Sonoma 14.3.1, Apple M2 / ARM64, Apple Clang Version 15.0.0, C++ 17/20, 64-Bit
- Raspberry 3, ARM, 64-bit OS, GCC 10.2.1, C++ 17/20, 64-Bit
- Raspberry 4, ARM, 64-bit OS, GNU C++ 12.2.0, C++ 17/20

The Programmer's Manual contains an extensive chapter about how to compile and use ALib in your 
C++ environment.

## C# and Java Versions Of ALib ##

Parts of **ALib** have sibling incarnations in programming languages C# and JAVA. Historically,
**ALib** aimed to be a cross platform/cross language library. This goal was (mostly) dropped
in favour to be able to independently develop **ALib for C++**.

**ALib for C#** and **ALib for Java** are included in and distributed with the
cross platform [ALox Logging Library](https://alexworx.github.io/ALox-Logging-Library/).


## License ##

**ALib** is free software, can be [downloaded at Github](https://github.com/AlexWorx/ALib-Class-Library)
and sources are published under [Boost Software License](LICENSE.txt)

##  Contributions ##
We would be happy, if the community started to support this library and are willing to receive
contributions and, if accepted, to update the code accordingly.<br>
Note that for legal reasons, we hereby explicitly refuse and reject code (or ideas for code)
that are not explicitly provided under the Boost Software License.
Please do not even contact us with your ideas/code in that case.


## Thanks ##

Our thanks go to all supporters that did and do help realizing this project. Furthermore
to just all of these millions of supporters of *free software*, including:
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


## More Information... ##
Please visit the [ALib Homepage](https://alib.dev)!


