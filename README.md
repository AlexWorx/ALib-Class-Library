## ALib Homepage ##
Please find all about the **ALib For C++**, including

* **Programmer's Manuals**
* **Reference Documentation**
* **Tutorials**
* **Project setup guide**
* **Version History / Change log**

at the [ALib Homepage](http://alexworx.github.io/ALib-Class-Library).

## Abstract ##

**ALib** is a general purpose, use-case agnostic, platform independent, low-level C++ class library.

Its mission is to provide foundational concepts, types and idioms relevant to any C++ project.
As of today, **ALib** consists of **16 modules**, each addressing
different topics.
A subset of the available modules can be selectively included in a custom library build.

## Main Characteristics And Design Goals ##

- **ALib** is <b>free software</b>.
- The minimum C++ language standard is <b>C++ 11</b>, selective support for more modern C++
  versions included.
- **Modularization**: Possible selective use of only parts of the library.
- Extensive **documentation**.
- <b>Least intrusive</b>: Designed to keep user code as independent from **ALib** types and idioms as possible.
- Developed and steadily tested under <b>GNU/Linux</b>, <b>Windows OS</b> and <b>Mac OS</b>
  with compilers GNU,  Clang and Microsoft Visual C++.

## Library Dependencies ##

**ALib** compiles as is, hence it is <b>not dependent on any 3rd-party library</b>.
Optional dependencies exist. For example if [boost](https://www.boost.org) is available,
**ALib**mod_nolink_strings are using its regex search.<br>
In contrast, **ALib** provides optional compatibility headers for 3rd-party libraries
(e.g. [QT Class Library](https://www.qt.io)), which for example provide adoptions of **ALib** type-traits
for QT-types.

## Documentation ##

The following documentation is provided.

1. **Reference Documentation** covering 100% of types, members, functions, namespaces, macros
   and other C++ entities.

2. **Programmer's Manuals** are available for (almost) each module of the library.<br>
   Together more than <b>450 pages</b> of well structured in-depth discussion of library
   features as well as <b>step by step tutorials</b> are available.

3. A detailed version history is given.

4. A separated **General Manual** is available describing the library structure,
   project setup, bootstrapping, etc.

5. A reference to C++ macros and compilation symbols is given.

6. Tutorial and reference documentation's <b> code samples</b> are implemented as unit tests and
   are integrated as source and output text snippets into the documentation.
   
7. For module **ALox**, a separated website exists. This includes plenty of **ALox**-specific
   documentation, a lengthy
   [tutorial](http://alexworx.github.io/ALox-Logging-Library/cpp_ref/alox_cpp_tutorial.html), definitions of
   [configuration/environment variables](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html)
   and more.


## IDE / Build System Setup ##
The C++ Version got tested on the following platform combinations:
- GNU/Linux (Arch Linux)
    - [GNU Compiler Collection 8.2.1](https://gcc.gnu.org/)
    - [clang compiler 7.0.1](http://llvm.org/)
    - [CMake 3.13.4](https://cmake.org/)
    - [CLion 2019.1 EAP](https://www.jetbrains.com/cpp)
    - [Valgrind V. 3.14.0](http://valgrind.org/)

- Windows 10
    - Visual Studio 2017, Version 15.9.4

- macOS Sierra: <em>The current release is not tested and most probably does not compile!</em>.<br>
  <b>Please volunteer for help!</b>

The Programmer's Manual contains an extensive chapter about how to compile and use ALib in your 
C++ environment.


## C# and Java Versions Of ALib ##

Parts of **ALib** have sibling incarnations in programming languages C# and JAVA. Historically,
**ALib** aimed to be a cross platform/cross language library. This goal was (mostly) dropped
in favour to be able to independently develop <b>ALib for C++</b>.

<b>ALib for C#</b> and <b>ALib for Java</b> are included in and distributed with the
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
- To company [JetBrains](https://www.jetbrains.com) for providing a free
  [Open Source License](https://www.jetbrains.com/buy/opensource/)
  of their absolutely superb and unrivalled set of IDEs for various programming languages.
- To <b>Dimitri van Heesch</b> for providing marvelous documentation software [Doxygen](http://doxygen.nl).

Special thanks also to C. Darwin, who created life on earth hundreds of millions of years ago,
until he - when things became too crazy - passed away in 1882.


## More Information... ##
Please visit the [ALib Homepage](http://alexworx.github.io/ALib-Class-Library)!


