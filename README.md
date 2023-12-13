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
As of today, **ALib** consists of **17 modules**, each addressing
different topics.
A subset of the available modules can be selectively included in a custom library build.

## Main Characteristics And Design Goals ##

- **ALib** is <b>free software**.
- Compiles and tested with **C++ 11, 14, 17 and  20**.
- **Modularization**: Possible selective use of only parts of the library.
- Extensive **documentation**.
- **Least intrusive**: Designed to keep user code as independent from **ALib** types and idioms as possible.
- Developed and steadily tested under
  - **GNU/Linux**,
  - **Windows OS**,
  - **Mac OS (Intel/ARM)**, and
  - **Raspberry (V3 and V4, 32/64-bit)**
 

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
   Together more than **450 pages** of well structured in-depth discussion of library
   features as well as **step by step tutorials** are available.

3. Page [Changelog](https://alib.dev/alib_changelog.html) provides a detailed version history.

4. A separated [General Manual](https://alib.dev/alib_manual.html) is available describing the 
   library structure, project setup, bootstrapping, etc.

5. [Reference documentation](https://alib.dev/alib_ref_prepro.html) for C++ compiler symbols and macros.

6. Tutorial and reference documentation's ** code samples** are implemented as unit tests and
   are integrated as source and output text snippets into the documentation.<br>
   (As for example [done here](https://alib.dev/alib_mod_enums.html#alib_enums_records)).
   
7. For module **ALox**, a separated website exists. This includes plenty of **ALox**-specific
   documentation, a lengthy
   [tutorial](http://alexworx.github.io/ALox-Logging-Library/cpp_ref/alox_cpp_tutorial.html), definitions of
   [configuration/environment variables](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html)
   and more.


## IDE / Build System Setup ##
The C++ Version got tested on the following platform combinations:
- GNU/Linux Arch 6.6.6, GNU C++ 13.2.1 / Clang++ 16.0.6, C++ 11 - 20, 32-Bit / 64-Bit<br>
  (This is the main development platform.)
- Windows 10/11, MSC 17.8.3 (Visual Studio 2022), , C++ 11 - 20, 32-Bit / 64-Bit
- Windows 10/11, MinGW 11.0,  C++ 11 - 17, 64-Bit ( <c>C++ 20 does  not compile! with current MinGW!</c>)
- MacOS 13, Intel i9, AppleClang 14.0.3, C++ 11 - 20 C++, 64-Bit
- MacOS 13, Apple M1/ARM64, C++ 11-20, 64-Bit
- Raspberry 3, ARM, 64-bit OS, GCC 10.2.1, C++ 11 - 20, 64-Bit
- Raspberry 4, ARM, 32-bit OS, GNU C++ 12.2.0, C++11-20
- Raspberry 4, ARM, 64-bit OS, GNU C++ 12.2.0, C++11-20

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


