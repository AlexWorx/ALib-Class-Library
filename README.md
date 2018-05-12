## ALib For C++ Homepage ##
Please find all about the **ALib For C++**, including

* **Programmer's manual**
* **Reference documentation**
* **Project setup guide**
* **Version History / Change log**

at the [ALib Homepage](http://alexworx.github.io/ALib-Class-Library).


## Overview ##

**ALib** is a C++ class library developed since 2014. The original goal was to provide a 
multi-language general purpose class library for languages C++, C# and Java. 
Since 2018, **ALib For C++** was extracted into this separate repository and is developed mostly 
independently from the the other language versions. 

**ALib** is organized in so called "modules", which means that a custom project might use 
(and compile) only a subset of the overall **ALib** menu.

The modules currently included are:      

- [ALib Singleton](https://alexworx.github.io/ALib-Class-Library/classaworx_1_1lib_1_1lang_1_1Singleton.html) <br>
  This smallest module (almost only one class) of \alib implements a singleton that overcomes the 
  Windows OS DLL boundaries, while under GNU/Linux chooses a faster straight-forward implementation.

- [ALib Boxing](https://alexworx.github.io/ALib-Class-Library/namespaceaworx_1_1lib_1_1boxing.html) <br>
  Implements <b>"auto-boxing"</b>, known from higher level programming languages.
  Transparently wraps C++ values of arbitrary type together with run-time type information into
  very lightweight objects. In other words, this library provides "typesafe void pointers which
  support virtual function calls".** <br>


- [ALib Strings](https://alexworx.github.io/ALib-Class-Library/namespaceaworx_1_1lib_1_1strings.html) <br>
  Very complete, non-intrusive, modern, powerful but yet lightweight C++ string classes.

- [ALib Core](https://alexworx.github.io/ALib-Class-Library/alib_man_module_core.html) <br>
  This is a rather "virtual" or "aggregation" module to select various basic **ALib** support classes
  that other modules depend upon.** <br>
  The classes found here are not or only loosely connected.
  While it had been decide to not exposed the types in dedicated modules, still some nice "gems" are
  found here!

- [ALib Configuration](https://alexworx.github.io/ALib-Class-Library/namespaceaworx_1_1lib_1_1config.html) <br>
  Transparently encapsulates access to configuration or other input data coming from various
  sources, like command line parameters, environment variables, INI-files, 3rd-party configuration
  databases, etc.

- [ALib ALox](https://alexworx.github.io/ALib-Class-Library/namespaceaworx_1_1lib_1_1alox.html) <br>
  A very unique and powerful approach to implement a software's debug and release logging facilities.

- [ALib Expressions](https://alexworx.github.io/ALib-Class-Library/namespaceaworx_1_1lib_1_1expressions.html) <br>
  Compilation and evaluation of regular expressions at run-time.
  The default expression syntax is along the lines of C++ expressions, including type safeness and 
  operator overloading. 
  Expressions are "compiled" at runtime into a "program" which is executed on a built-in small
  virtual machine (no worries, the VM is extremely lightweight and fast).
  The definition of custom functions and custom data types is extremely simple as the provided
  tutorial proves.     

- [ALib CLI](https://alexworx.github.io/ALib-Class-Library/namespaceaworx_1_1lib_1_1cli.html) <br>
  Library to parse and process command line parameters. In case of pure CLI software, provides
  tools for formatted and internationalized help and result output.
  (Beta state, not yet documented well!).



For all details, please continue to the [ALib Homepage](http://alexworx.github.io/ALib-Class-Library).

