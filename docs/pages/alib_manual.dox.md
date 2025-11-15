// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_manual    ALib Programmer's Manual

\tableofcontents


\I{################################################################################################}
# 1. Introduction # {#alib_manual_intro}

Welcome the Programmer's Manual of the <b>%ALib C++ Class Library</b>!

While we are proud of presenting \alib, we are likewise proud of providing you with not only
this manual, but with additional separated manuals which are dedicated to the different
parts of the library - which we call "ALib Modules".

This foundational manual explains the structure and concepts of the library, how to select only
parts of it and build your stripped and lean version, and of course, how the library is used in
general.

The first thing we need to discuss is how the library is structured into \alibmods_nl.
This became especially true, since C++20 likewise introduced the concept and even the new keyword
\c module, which this library supports seamlessly.

\I{################################################################################################}
# 2. ALib Modules # {#alib_manual_modules}

As a "general purpose" C++ library, \alib tackles a wider range of programming areas, which is why
its code and documentation are systematically structured in a
\ref alib_manual_why_layering "top-down layered hierarchy".<br>
For this reason, the library was divided into currently
\ref alib_manual_modules_table "25 different modules", a step taken well in advance of the
introduction of C++20's module concept.

An \alib module:
- Covers a certain topic or field of application.
- Is a subset of the overall list of \alib types.
- Comes with a dedicated Programmer's Manual as well as complete reference documentation.
- Occupies a dedicated inner namespace of the overarching outer namespace #alib.
- Has two defined relationships to other \alibmods, namely "mandatory dependencies" and
  "optional dependencies."
- Has one main header to include the main types of the module in your compilation unit and
  optionally has one or more additional sub-headers. Header names always start with
  "ALib." and end with ".H"
  
<p><p>
As a sample, let us look at module \alib_strings_nl:
- Its about string manipulation, conversion, 3rd-party interoperability, formatting, etc.
- Included types are \alib{String}, \alib{CString}, \alib{AString},
  \alib{Substring}, but also \alib{Tokenizer}, \alib{strings::util;CalendarDate},
  and various others.
- Its Programmer's Manual is \ref alib_mod_strings "is found here".
- Its namespace is #alib::strings (the link targets the module's reference manual)
- It is dependent on modules \alib_characters and \alib_lang and it has
  optional dependencies to modules \alib_time and \alib_monomem.
- Its main header is \implude{Strings} and various sub-headers, for
  example, \implude{Strings.Tokenizer}, or \implude{Strings.Calendar}.

Well, this is all quite normal, but we wanted to point out here that we care for a simple
but reliable and everywhere complete structure and offering.

\note 
  \alibmods_nl are not C++20 modules. While we put some effort in trying to offer C++20 module
  support for \alib, we have stopped that project for the time being. More information. 
  on this topic is found on a \ref alib_c20module_shift "dedicated blog-page". 
      

\I{################################################################################################}
## 2.1 The Term "ALib Build" ## {#alib_manual_alib_build}
Within this manual, within the distinct manuals of the \alibmods_nl and within the reference
documentation, the term <em><b>"ALib Build"</b></em> is frequently used.

\par Definition
   An \alibbuild_nl defines the <em>"permutation of modules"</em> that have been selected
   from the overall set of modules available.
   Such selection may optionally be performed at the time of compiling the library.
   In case it is not performed, simply all \alibmods_nl are included in the \alibbuild_nl.

It might be in a user's interest to reduce \alib to a subset of its functionality, and this way
speed up compilation and decrease library footprint.

It is to be noted that the omission of single modules might have "side effects" on included
modules: While they are still compilable and function well - just certain features might be
dropped!<br>
This is due to what we call an <b><em>"optional dependency"</em></b>.
An optional dependency from module \b A to module \b B could be phrased as:<br>
<em>\"Module \b A provides extended functionality in case that module \b B is included in the
ALib Build.\"</em>

As a sample, \alib_files can be compiled with the absence of module \alib_expressions.
But in this case functionality for filtering file trees using run-time expressions will not
be available.

The reference documentation of functions, methods, or types that are dropped when reducing the
\alibbuild gives (in most cases) a prominent hint on this specific relationship.
To stay with the sample, you can look out for such a note at the end of the reference documentation
of class \alib{files;FileExpressions}.

\I{################################################################################################}
## 2.2 ALib Module Hierarchy ## {#alib_manual_modules_graph}
With the knowledge we now have about \alibmods_nl and \alibbuilds_nl, we can continue this
manual with a graph that shows all \alibmods_nl and their mandatory and optional dependencies:
  

\dot
digraph ALibModules
{
    newrank=true
    compound=true;
    fontsize="10";
    fontname="Open Sans";

    node [shape=plaintext, fontsize="10", fontname="Open Sans"]
                           
    Bootstrap      [label= "Bootstrap"   shape= "box"        color="black" penwidth=2   URL="\ref alib_mod_bs"           ];

    //--- ALib Camps ---
//  Application    [label= Application     shape= "component" style="bold" color="chocolate"   URL="\ref alib_mod_app"          ];
    ALox           [label= ALox            shape= "component" style="bold" color="chocolate"   URL="\ref alib_mod_alox"         ];
    Camp           [label= Camp            shape= "component" style="bold" color="chocolate4"  URL="\ref alib_mod_camp"         ];
    CLI            [label= CLI             shape= "component" style="bold" color="chocolate"   URL="\ref alib_mod_cli"          ];
    Expressions    [label= Expressions     shape= "component" style="bold" color="chocolate"   URL="\ref alib_mod_expressions"  ];
    Files          [label= Files           shape= "component" style="bold" color="chocolate"   URL="\ref alib_mod_files"        ];

    Bootstrap      -> { ALox CLI Expressions Files }          [ color="gray69"  style=dotted, penwidth=2 ];

    //--- Below Camp ---
    Format           [label= Format       shape= "box" color="dodgerblue4"  URL="\ref alib_mod_format"     ];
    Exceptions       [label= Exceptions   shape= "box" color="dodgerblue4"  URL="\ref alib_mod_exceptions" ];
    Resources        [label= Resources    shape= "box" color="dodgerblue4"  URL="\ref alib_mod_resources"  ];
    System           [label= System       shape= "box" color="dodgerblue4"  URL="\ref alib_mod_system"     ];
    Variables        [label= Variables    shape= "box" color="dodgerblue4"  URL="\ref alib_mod_variables"  ];


  //Application     -> { CLI ALox                      }  [ color="midnightblue" ];
  //Application     -> { Files Expressions ThreadModel }  [ color="gray69"  style=dotted, penwidth=2 ];
    Files           -> { Expressions ALox                }  [ color="gray69"  style=dotted, penwidth=2 ];
    Files           -> { Camp                            }  [ color="midnightblue" ];
    CLI             -> { Camp                            }  [ color="midnightblue" ];
    Expressions     -> { Camp                            }  [ color="midnightblue" ];
    ALox            -> { Camp                            }  [ color="midnightblue" ];

    Variables       -> { System Monomem Containers       }  [ color="gray15" ];
    Camp            -> { Format Variables                }  [ color="gray15" ];
    Format          -> { Exceptions                      }  [ color="gray15" ];
    Exceptions      -> { Resources Boxing EnumRecords    }  [ color="gray15" ];
    System          -> { Boxing EnumRecords              }  [ color="gray15" ];
    Resources       -> { Strings Containers Monomem      }  [ color="gray15" ];

                            
    ThreadModel    [label= ThreadModel           shape= "box"        color="dodgerblue4"     style="bold"  URL="\ref alib_mod_threads"      ];
    BitBuffer      [label= BitBuffer             shape= "box"        color="dodgerblue4"     style="bold"  URL="\ref alib_mod_bitbuffer"    ];
                                                 
    Boxing         [label= Boxing                shape= "box"        color="dodgerblue4"   URL="\ref alib_mod_boxing"       ];
    Containers     [label= Containers            shape= "box"        color="dodgerblue4"   URL="\ref alib_mods_contmono"    ];
    EnumRecords    [label= EnumRecords           shape= "box"        color="dodgerblue4"   URL="\ref alib_mod_enums"        ];
    Monomem        [label= Monomem               shape= "box"        color="dodgerblue4"   URL="\ref alib_mods_contmono"    ];
    Singletons     [label= Singletons            shape= "box"        color="dodgerblue4"   URL="\ref alib_mod_singletons"   ];
    Strings        [label= Strings               shape= "box"        color="dodgerblue4"   URL="\ref alib_mod_strings"      ];

    subgraph Modules2        {  rank="same"  ALox CLI Expressions                  color="dodgerblue4" }
    //subgraph Modules3        {  rank="same"  Monomem Strings                       color="dodgerblue4" }
                                                
    //--- dependencies base module -> base module
    BitBuffer       -> { Monomem Containers }            [ color="gray15"  ];
    ThreadModel     -> { Boxing Containers Monomem }     [ color="gray15"  ];


    //--- dependencies base module -> base module
    Boxing          -> { Singletons          }            [ color="gray15"  ];
    EnumRecords     -> { Singletons  Strings }            [ color="gray15"  ];

     
    // optional dependencies
    Singletons      -> { Monomem Containers }          [ color="gray69"  style=dotted, penwidth=2 ];
    Boxing          -> { Monomem Containers Strings}   [ color="gray69"  style=dotted, penwidth=2 ];
    Containers      -> { Strings  }                    [ color="gray69"  style=dotted, penwidth=2 ];
    Strings         -> { Monomem  }                    [ color="gray69"  style=dotted, penwidth=2 ];
    EnumRecords     -> { Monomem Containers Boxing }   [ color="gray69"  style=dotted, penwidth=2 ];
    Containers      -> { Monomem  }                    [ color="gray69"  style=dotted, penwidth=2 ];
    BitBuffer       -> { EnumRecords  }                [ color="gray69"  style=dotted, penwidth=2 ];
    Resources       -> { EnumRecords  }                [ color="gray69"  style=dotted, penwidth=2 ];
    ThreadModel     -> { EnumRecords Strings }         [ color="gray69"  style=dotted, penwidth=2 ];
    System          -> { Monomem Exceptions }          [ color="gray69"  style=dotted, penwidth=2 ];
}
\enddot
      
Besides the topmost Module \alib_bootstrap, the following foundational and lowest-level modules,
which were excluded from the graph above, are <b>always included in any \alibbuild_nl:</b>
\dot
digraph ALibBaseModules
{
    newrank=true
    compound=true;
    fontsize="10";
    fontname="Open Sans";

    node [shape=plaintext, fontsize="10", fontname="Open Sans"]

    Assert         [label= "Assert"      shape= "box"        color="black" penwidth=2   URL="\ref alib_mod_assert"       ];
    Characters     [label= "Characters"  shape= "box"        color="black" penwidth=2   URL="\ref alib_mod_characters"   ];
    EnumOps        [label= "EnumOps"     shape= "box"        color="black" penwidth=2   URL="\ref alib_mod_enums"        ];
    Lang           [label= "Lang"        shape= "box"        color="black" penwidth=2   URL="\ref alib_mod_lang"         ];
    Threads        [label= "Threads"     shape= "box"        color="black" penwidth=2   URL="\ref alib_mod_threads"      ];
    Time           [label= "Time"        shape= "box"        color="black" penwidth=2   URL="\ref alib_mod_time"         ];
    
}
\enddot

This graph is a good reference for answering various questions.
I.e., <em>\"Which other modules will be included in an \alibbuild_nl if I need module Xyz?\"</em>
For example, if a user aims to include the functionality of the module \alib_threadmodel_nl
in an application, the graph tells us that in addition, the modules
\alib_boxing_nl, \alib_containers_nl and \alib_monomem_nl are directly dependent. Furthermore,
the module \alib_boxing_nl has a next dependency to \alib_singletons_nl.
This means, together with the module \alib_threadmodel_nl itself, five selectable modules are
included.

This is not the full picture, yet: On the bottom of the graph there are seven foundational modules
which are always included in any \alibbuild_nl.

The good news is, that the \alib build system will
\ref alib_manual_build_cmake_1 "care about this automatically".
To stay in the sample, all you have to do is announce the module \alib_threadmodel_nl.
The scripts will resolve the mandatory dependencies automatically.<br>
Modules with optional dependencies have to be added explicitly.
In this sample, we would recommend also adding the module \alib_strings.
Details on this are given in the later chapter \ref alib_manual_build.


\I{################################################################################################}
## 2.3 Tabular List Of Modules ## {#alib_manual_modules_table}

The following table lists all \alibmods_nl in alphabetical order.
The table includes links to the reference documentation as well to the Programmer's Manual of each
module.

Name <br>(Programmer's Manual)             | Namespace <br>(Reference Documentation) | Description |
-------------------------------------------|-----------------------------------------|-------------------------------
\ref alib_mod_alox          "ALox"         | \ref alib::lox                          |A very unique and powerful approach to implement software's debug and release logging facilities.
\ref alib_mod_assert        "Assert"       | \ref alib::assert                       |Raising assertions, warnings and other messages. Available only with debug-builds, respectively when the compiler-symbol \ref ALIB_DEBUG is given.
\ref alib_mod_bitbuffer     "BitBuffer"    | \ref alib::bitbuffer                    |Provides types to read and write data into a stream of bits in a platform-independent manner. Furthermore different compression algorithms are given.
\ref alib_mod_bs            "Bootstrap"    | \ref alib                               |Well defined bootstrap mechanisms for \alib as well as - optionally - for consumeers of the library. A phased approach that, for example, to assure availablility of resources and configuration data at defined stages. This module is always ranked highest in the list and has optional dependencies to each module that needs bootstrapping.
\ref alib_mod_boxing        "Boxing"       | \ref alib::boxing                       |Implements <b>"auto-boxing"</b> for C++, known from higher level programming languages. Transparently wraps C++ values of arbitrary type together with run-time type information into very lightweight objects. In other words, this library provides <em>"type-safe void pointers which support virtual function calls"</em>, provides type-safe a variadic argument paradigm and much more.
\ref alib_mod_camp          "Camp"         | \ref  alib::camp                        |Introduces the concept of \alibcamps, which a \alibmods_nl that manage externalized resources, configuration variables and which support extended bootstrapping strategies.
\ref alib_mod_characters    "Characters"   | \ref alib::characters                   |Mitigates C++ character type, character pointer and character array hell.
\ref alib_mods_contmono     "Containers"   | \ref alib::containers                   |ALib container types. Furthermore tools to enable the containers of the C++ Standard Library to use \alib{lang;Allocator;alib allocators} are included.
\ref alib_mod_cli           "CLI"          | \ref alib::cli                          |Implements parsing and partly the processing of command line arguments (CLI stands for "command line interface"). In case of pure CLI software, provides tools for formatted and localized help and result output.
\ref alib_mod_enums         "EnumOps"      | \ref alib::enumops                      |Unary and binary operators for C++ enum elements.
\ref alib_mod_enums         "EnumRecords"  | \ref alib::enumrecords                  |Named (printable) C++ enum elements.
\ref alib_mod_exceptions    "Exceptions"   | \ref alib::exceptions                   |C++ exceptions which allow the collection of information while unwinding the stack. Furthermore, externalized (resourced) user messages, leveraging \alib formatting capabilities to embed dynamic information about the errors.
\ref alib_mod_expressions   "Expressions"  | \ref alib::expressions                  |Expression parser, compiler and a virtual machine for evaluation of compiled expressions. Excels in respect to speed and flexibility and is very easy to adopt in own projects.
\ref alib_mod_files         "Files"        | \ref alib::files                        |Directory and File scanner, optionally filtering with \alib_expressions_nl.
\ref alib_mod_format        "Format"       | \ref alib::format                       |String formatting.
\ref alib_mod_lang          "Lang"         | \ref alib::lang                         |The most foundational module of \alib, providing specific to the (modern) C++ language.
\ref alib_mods_contmono     "Monomem"      | \ref alib::monomem                      |Monotonic allocation utilities to avoid heap-memory usage. Together with the concepts introduced with module \alib_containers, the amount of potential use cases for monotonic allocation are tremendously extended. Note that the cost of heap allocation is largely underestimated by programmers.
\ref alib_mod_resources     "Resources"    | \ref alib::resources                    |Mangement of - optionally externalized - static string resources.
\ref alib_mod_singletons    "Singletons"   | \ref alib::singletons                   |Implements the singleton paradigm that overcomes the Windows OS DLL boundaries (caused by Window DLL's local data segments), while under other operating systems chooses a faster, straight-forward implementation.
\ref alib_mod_strings       "Strings"      | \ref alib::strings                      |With a least intrusive design offers compatible, lightweight, secure, efficient, complete and convenient C++ string classes.
\ref alib_mod_system        "System"       | \ref alib::system                       |Class \alib{system;Path} and a few other types related to operating systems.
\ref alib_mod_threadmodel   "ThreadModel"  | \ref alib::threadmodel                  |High-level multithreading tools.
\ref alib_mod_threads       "Threads"      | \ref alib::threads                      |Low-level thread primitives.
\ref alib_mod_time          "Time"         | \ref alib::time                         |Types for calendrical and non-calendrical date and time processing. Always included in any \alibbuild.
\ref alib_mod_variables     "Variables"    | \ref alib::variables                    |Transparently encapsulates access to configuration or other input data coming from various sources, like command line parameters, environment variables, INI-files, 3rd-party configuration databases, etc. Support for persistent write-back of such data is given.

\I{################################################################################################}
## 2.4 Including/Importing ALib Modules ## {#alib_manual_modules_impludes}

\alib does not follow the traditional “one type / one header” convention.
Instead, it uses a module-oriented header layout:
Each logical \alib module is represented by one primary header:

        #include "ALib.Strings.H"
        #include "ALib.Boxing.H"
        #include "ALib.Format.H"
       
Most modules provide additional, more fine-grained headers for specialized parts of that 
module, for example:

        #include "ALib.Strings.Tokenizer.H"
        #include "ALib.Strings.Calendar.H"
        #include "ALib.Boxing.StdFunctors.H"
        #include "ALib.Format.FormatterJavaStyle.H"
                            
Together, the primary module header and its optional extension headers cover the complete 
public interface of the module. This gives a clear, module-based structure while still allowing 
you to include only the features you actually use.

\attention
   All other filenames in the \alib codebase use lowercase letters, and neither the ".H" nor ".h"
   file extensions are used elsewhere in \alib.
   This design choice was deliberately made to help your IDE to easily propose and auto-complete
   your inclusion statements.<br>
   None of the header files written in lowercase letters must be included directly. 

\note
   We had left the "one class, one header" approach at the moment when we tried to establish 
   a dual-mode compilation option for \alib: Traditional compilation and compilation as 
   a C++20 Module library.<br>
   For those interested in our experiences of transitioning a bigger library to C++20 modules,
   we have written a \ref alib_c20module_shift "blog page" sharing our experiences.
        
                       
\I{################################################################################################}
## 2.4.2 Include/Import Header Reference ## {#w}

<b>The following table lists all header files available.</b>

Module                                     | Headers to Include
-------------------------------------------|---------------------------------------------------------------------
\ref alib_mod_alox          "ALox"         | \implude{ALox}<br>\implude{ALox.Impl}
\ref alib_mod_assert        "Assert"       | \implude{Lang} (No own include)
\ref alib_mod_bitbuffer     "BitBuffer"    | \implude{BitBuffer}
\ref alib_mod_bs            "Bootstrap"    | \implude{Bootstrap}
\ref alib_mod_boxing        "Boxing"       | \implude{Boxing}<br>\implude{Boxing.StdFunctors}
\ref alib_mod_camp          "Camp"         | \implude{Camp}<br>\implude{Camp.Base}
\ref alib_mod_characters    "Characters"   | \implude{Lang} (important type traits are imported here already)<br>\implude{Characters.Functions}
\ref alib_mods_contmono     "Containers"   | \implude{Containers.FixedCapacityVector}<br>\implude{Containers.HashTable}<br>\implude{Containers.List}<br>\implude{Containers.LRUCacheTable}<br>\implude{Containers.SharedPtr}<br>\implude{Containers.SharedVal}<br>\implude{Containers.StringTree}<br>\implude{Containers.StringTreeIterator}
\ref alib_mod_cli           "CLI"          | \implude{CLI}
\ref alib_mod_enums         "EnumOps"      | \implude{EnumOps}
\ref alib_mod_enums         "EnumRecords"  | \implude{EnumRecords}<br>\implude{EnumRecords.Bootstrap}
\ref alib_mod_exceptions    "Exceptions"   | \implude{Exceptions}
\ref alib_mod_expressions   "Expressions"  | \implude{Expressions}<br>\implude{Expressions.Impl}
\ref alib_mod_files         "Files"        | \implude{Files}<br>\implude{Files.TextFile}
\ref alib_mod_format        "Format"       | \implude{Format}<br>\implude{Format.FormatterJavaStyle}<br>\implude{Format.FormatterPythonStyle}<br>\implude{Format.Paragraphs}<br>\implude{Format.PropertyFormatter}<br>\implude{Format.PropertyFormatters}
\ref alib_mod_lang          "Lang"         | \implude{Lang}<br>\implude{Lang.CIFunctions}<br>\implude{Lang.CIMethods}
\ref alib_mods_contmono     "Monomem"      | \implude{Monomem}<br>\implude{Monomem.SharedMonoVal}<br>\implude{Monomem.StdContainers}
\ref alib_mod_resources     "Resources"    | \implude{Resources}
\ref alib_mod_singletons    "Singletons"   | \implude{Singletons}
\ref alib_mod_strings       "Strings"      | \implude{Strings}<br>\implude{Strings.AutoSizes}<br>\implude{Strings.Calendar}<br>\implude{Strings.Escaper}<br>\implude{Strings.Monomem}<br>\implude{Strings.Search}<br>\implude{Strings.StdFunctors}<br>\implude{Strings.StdIOStream}<br>\implude{Strings.Token}<br>\implude{Strings.Tokenizer}<br>\implude{Strings.Vector}
\ref alib_mod_system        "System"       | \implude{System}
\ref alib_mod_threadmodel   "ThreadModel"  | \implude{ThreadModel}
\ref alib_mod_threads       "Threads"      | \implude{Threads}
\ref alib_mod_time          "Time"         | \implude{Time}
\ref alib_mod_variables     "Variables"    | \implude{Variables}<br>\implude{Variables.IniFile}<br>\implude{Variables.Plugins}<br>\implude{Variables.ResourcePool}
                
\I{################################################################################################}
## 2.4.1 Further Public Headers ## {#alib_manual_modules_impludes_further}
Besides the headers which either import or include portions of an \alibmod_nl, a few further
public headers are present.
     
1. <b>"Compatibility Headers"</b><br>
   Five so-called <em>"compatibility header-files"</em> exist.
   More information on those are given in chapter \ref alib_manual_build_more_externallibs.

2. <b>Switching Macro ALIB_CALLER</b><br>
   The two headers \implude{Lang.CIFunctions} and \implude{Lang.CIMethods} are provided to switch the macro
   \ref ALIB_CALLER to either work with functions and static methods, where no \c this pointer
   is available, or to work with method members of types.
   More information on this topic is provided in
   the appendix-chapter \ref alib_manual_appendix_callerinfo.

\I{################################################################################################}
## 2.5 Special Treatment For Module ALox ## {#alib_manual_modules_special_alox}
A special treatment is granted to the module \alib_alox, by having the header-file \implude{ALox}
still defining the logging macros, even if this module is not included in an \alibbuild.

This allows a user to place \alox log statements in a compilation unit without the need of testing
preprocessor symbol \ref ALIB_ALOX or the use of macros \ref IF_ALIB_ALOX and \ref IFNOT_ALIB_ALOX.
Such custom sources are this way still ready to compile against an \alibbuild_nl that does not
include \alox.

If the module \alib_alox_nl is not included, the macros are "empty" and this way, all
log-statements just get "pruned".
                                                         
\note
  This is similar to the behaviour of macros like \ref ALIB_LOCK and \ref ALIB_LOCK_WITH, which
  remain available but empty, even with
  \ref alib_manual_multithreading "single-threaded compilations" of the library.

\I{################################################################################################}
## 2.6 ALib Camps ## {#alib_manual_modules_camps}
To close this section, we quickly want to explain the term \alibcamp_nl.

\par Definition
  An \alibcamp_nl is an \alibmod_nl which uses the facilities provided with the
  equally named module \alib_camp. These facilities include three things:
  1. Access to externalized string resources,
  2. Access to external configuration data, and
  3. A well-defined bootstrap and shutdown process invoked at the start and termination of
     the software process.
                                
In other words: \alib offers a module named \alib_camp, and that subset of other modules
which rely on it, are called <em>"ALib Camp Modules"</em> or just \alibcamps_nl.
 
If you look at the \ref alib_manual_modules_graph "hierarchy graph" of the previous section,
you will quickly identify module \alib_camp and the camp-modules that build on it.

Today, those are:
- \ref alib_mod_alox "ALib ALox",
- \alib_cli,
- \alib_expressions, and
- \alib_files.

More on this topic is given with the dedicated Programmer's Manual \alib_camp.

\I{################################################################################################}
# 3. Bootstrapping And Shutting Down ALib # {#alib_manual_bs}
Bootstrapping a software process can often become complex, especially when various parts of an
application fail to align seamlessly.
            
The C++ language itself does not offer a standard approach for initiating or managing the startup
process effectively.
As a result, larger projects frequently postpone creating their own bootstrapping mechanisms until
later stages of development, leading to unnecessary difficulties and inefficiencies.

To address this, \alib not only provides an interface for bootstrapping the library but also
introduces a structured concept that users can optionally integrate into their projects.

This bootstrapping approach is encapsulated as a dedicated \alibmod, with all related details
and usage instructions discussed thoroughly in its dedicated \ref alib_mod_bs "Programmer's Manual".

\I{################################################################################################}
# 4. Multithreading Support # {#alib_manual_multithreading}
\alib offers two distinct modules for multi-threading support: \alib_threads and \alib_threadmodel.
The \alib_threadmodel_nl module is a higher-level, optional component that can be included or
excluded from an \alibbuild as needed.
       
In contrast, the \alib_threads_nl module operates at a much lower level.
Like foundational modules such as \alib_lang or \alib_time, it cannot be excluded from the build.
This module is relatively compact, containing the \alib{threads;Thread} class, along with various
mutex implementations, such as \alib{threads;Lock}.

Now, the library can be compiled using the \ref ALIB_SINGLE_THREADED preprocessor (and CMake)
symbol.
Single-threaded applications should do this, because with that, some superfluous overhead
is dropped from the build.

But even then, parts of the \alib_threads_nl module are still available when compiled in
single-threaded mode: They remain as empty skeleton types and methods.
Additionally, several corresponding preprocessor macros, for example, \ref ALIB_LOCK_WITH,
are preserved but are likewise emptied.

This approach offers significant advantages: The library code remains uncluttered, and your code
benefits as well. By utilizing the macros and types provided by \alib, you can develop applications,
which likewise support single-threaded and multithreaded versions with minimal effort.

All further information on this topic is provided with the
\ref alib_mod_threads "dedicated Programmer's Manual" of module \alib_threads_nl.
                                                                                          
\I{################################################################################################}
# 5. Building The Library # {#alib_manual_build}

\I{################################################################################################}
## 5.1 Overview ## {#alib_manual_build_overview}

\I{################################################################################################}
### 5.1.1 Platforms and Toolchains ### {#alib_manual_build_overview_platforms}

The current release was compiled and tested under the following platforms and toolchain combinations:
- GNU/Linux Arch 6.16.8, Clang++ 21.1.5, C++20/23, 32-Bit / 64-Bit<br>
  (This is the main development platform.)
- GNU/Linux Arch 6.16.8, GNU C++ 15.2.1, C++20/23, 32-Bit / 64-Bit
- WindowsOS 11, MSC 19.44 (Visual Studio 2026 Insiders, Platform v145), C++20, 32-Bit/64-Bit
- WindowsOS 11, MinGW, 64-Bit, GCC 13.47, C++20
- macOS Tahoe 26.0, Apple M2 / ARM64, Apple Clang Version 17.0.0, C++20/23, 64-Bit
- Raspberry 3, aarch64, Cortex-A53, GNU C++ 12.2.0, C++20/23
- Raspberry 4, aarch64, Cortex-A72, GNU C++ 12.2.0, C++20/23
- Raspberry 4, armhf (32-bit), Cortex-A72, GNU C++ 12.2.0, C++20/23

All development was performed in a) CLion/CMake (all platforms) and b) Visual Studio Community
Edition 2022. All necessary IDE files are included in the repository.

Adoptions to other platforms, toolchains, and IDEs should be implementable with limited efforts.
All relevant code which selects platform/toolchain specific libraries, intrinsics, etc., will
expose a preprocessor error if a section fails due to an unknown environment.
This allows quickly inserting the right platform/toolchain specific code at these places.

\note We would be very happy to receive your feedback/input on necessary changes for other
      platforms!

Especially errors occurring in internal header \alibheader{lang/integers.inl} might be quite
likely for unknown platform / toolchain combinations.
Here, a set of five compiler-symbols might be passed using the build system (e.g., CMake), which
are documented with symbol \ref ALIB_SIZEOF_INTEGER.

<b>The C++ compiler warning level is defaulted to the bearable maximum.</b> This means, that the
inclusion of \alib headers into a custom project's compilation process should never lead to
compilation warnings or errors when similar high custom warning levels (of the including project)
are used.


\I{################################################################################################}
### 5.1.2 Reference Toolchain CMake ### {#alib_manual_build_overview_cmake}

While project files for different IDEs might be provided with the codebase, the main development
of \alib is performed using \https{CMake,cmake.org/} scripts.
Also, the relationship of source file and \alibmod_nl selection is <b>"officially defined"</b>
by CMake scripts.

\par
  <b>The \e CMake scripts coming with \alib are to be seen as the reference guideline to building
  \alib. The scripts may even be more precise (and up to date!) than the documentation you
  currently read itself.</b>

Even for non-experienced users (in respect to CMake), the syntax of the file should be easily
understood.
So, do not hesitate to open and read the CMake files for your project setup reference.

A Microsoft Visual Studio solution and the according project files which build an \alib DLL and the
unit tests and sample applications, are included in the \alib project folder.
These may be used to compile an \alib library that includes all of \alib, which can be used
for own projects.<br>
Limited library projects that include only a selection of the modules of \alib are not provided
and thus have to be created if they are desired.


\I{################################################################################################}
### 5.1.3 Library Installation ### {#alib_manual_build_overview_noinstall}
As of today, no installation process of a shared library version of \alib is available with
the build process defined. Not installing a library has of course the disadvantage, that every
software project needs to compile its own version of it, and the library is by default not shared between
applications, even if compiled as a "shared library", respectively DLL.

While this may change in the future, the advantage of this approach is that an application has
a lot of flexibility in respect to compiling \alib using the exact set of features it desires.
And then: installing libraries that use C++20 Modules (if \alib is compiled so) is currently not
even a well-defined process.
It seems, due to the latter, the trend goes a little into the direction to have a dedicated
compilation of C++ libraries per application.

Therefore, to enable software to use \alib, the sources of the library have to become a part
of the build process in any form. As usual, there are three possible basic options:
1. Compiling \alib to a static library,
2. compiling \alib to a shared library or
3. compiling \alib sources directly into another software entity.

When this manual section talks about "building the ALib library", one of the three options is meant.

\note
  Of course, a custom installation process can be established to place library binary and header
  files in corresponding system folders. It is just not provided today.


\I{################################################################################################}
### 5.1.4 Unit Tests ### {#alib_manual_build_overview_unittests}
An extensive set of unit tests is included in the distribution, along with a corresponding
CMake script to build those.


\I{################################################################################################}
## 5.2 Performing A Test Compilation ## {#alib_manual_build_test}
Clone the \alib repository from \https{ALib at GitHub,github.com/AlexWorx/ALib-Class-Library}
to a place where you commonly store 3rd-party libraries that your projects are using.
Alternatively, download and unpack the ZIP file to that same place.

In this documentation, we will refer to this library location as the <b><em>ALIB_BASE_DIR</em></b>.
After a fresh installation, it should contain at least the following subfolders:

        ALIB_BASE_DIR/build
        ALIB_BASE_DIR/docs
        ALIB_BASE_DIR/src
        ALIB_BASE_DIR/src.samples
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
## 5.3 A Step-By-Step CMake Sample ## {#alib_manual_build_sample}
With using \e CMake, compiling and using \alib is very straight forward.
This is because a set of easy to use \e CMake scripts is provided, of which one is to be included
into the custom \e CMake script.

The following demonstrates this step-by-step along the sample project found in folder

        ALIB_BASE_DIR/src.samples/alox

<br>
<b>Step 1: Creating the CMake file</b><br>
A custom \e CMake file has to be created. To start with, the top of the file might look like this:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_1

<br>
<b>Step 2: Choose %ALib Modules</b><br>
The list of \alib modules to be included in the \alibbuild is defined with \e CMake list variable
\c ALIB_BUILD which has to be set before invoking the \alib \e CMake script.
If the list is left empty, it will be defaulted to \b "ALL", which chooses all \alib modules
available.<br>
In our sample, we add \b "ALOX", which chooses module \alib_alox and all
\ref alib_manual_modules_graph "dependent modules".


 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_2


<br>
<b>Step 3: Set Other Feature Variables</b><br>
Our project should be compiled using C++20. This is set with:

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
Our main project \ref alib_mod_bs_assertcomp "has to share" some \alib compiler and linker settings
with the \alib library project. To achieve this we invoke \e CMake function <c>ALibSetCompilerAndLinker</c>:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_7

<br>
<b>Step 8: Add Library Project To Custom Project</b><br>
The final step is to add the library project to the custom sample project, which is again a
standard \e CMake task:

 \snippet "../../build/cmake/samples/ALox/CMakeLists.txt"        DOXYGEN_CMAKE_INCLUDE_8


That's it. With this simple CMake file we have created a tailored \alibbuild (selection of modules)
and have linked it to our sample project, which is now ready to be built!


\I{################################################################################################}
## 5.4 The Build Process In Detail## {#alib_manual_build_cmake}

The previous chapter demonstrated the use of the \e CMake script \c ALib.cmake provided with
\alib. In the following sections a reference documentation on all aspects of the script is
given.

Along these lines, the build requirements of \alib is explained - also for users of any other build
system. independent of the build-system used, the following information has to be collected and
accordingly set:

- The set of \alib source files that comprise the desired permutation of \alibmods_nl, what
  we call an \alibbuild.
- C++ Version 20 or higher (to be set with the compiler).
- compiler-symbols that select the \alibmods have to be passed to the compiler.
- compiler-symbols that choose \alib features have to be passed to the compiler.
- External library dependencies have to be determined.
- Linker flags have to be defined.

\note
  The complete set of symbols that the \alib library accepts from the compiler (usually \e -D option)
  are \ref GrpALibPreproSymbols "documented here".

\I{################################################################################################}
### 5.4.1 Selecting The Included ALib Modules ### {#alib_manual_build_cmake_1}
\e CMake list variable \c ALIB_BUILD is an input/output list which defines the particular
\alibmods that should be included in the build.
The script will process the values given and will extend the list to include all necessary modules
that the \ref alib_manual_modules_graph "given selection depends on".

The values correspond to the \ref alib_manual_modules_table "module names" in upper case letters,
hence \b ALOX, \b BOXING, \b CHARACTERS, etc... (Lower-case or mixed case names are allowed
as input, but will be internally converted to upper case.)

If the variable is not set or contains special name \b "ALL", all modules are chosen to be built
and included.
                  
\I{################################################################################################}
### 5.4.2 Variables For Directory and Source Code Definitions ### {#alib_manual_build_cmake_2}
The following \e CMake variables are available after the invocation of the script:
- \c ALIB_BASE_DIR<br>
  Defines the directory where the \alib project is stored.<br>
  This variable may also be specified before entering the script to specify a different directory
  than the default, which is inferred from the full path name of the script invoked.
<p>

- \c ALIB_SOURCE_DIR<br>
  Defines the directory where the \alib source files are stored. The header-files are located
  in the same directories.<br>
  This variable may also be specified before entering the script to specify a different directory
  than the default, which is

            ${ALIB_BASE_DIR}/src
<p>

Besides the directory, the following CMake list variables are defined. Each list contains
all names of source files of the corresponding type, which have to be present according
to the \alibbuild defined in the previous step.<br>
The file names come with absolute path names.
 CMake List   | File type description
--------------|--------------------------------------------------------------------------------
  ALIB_MPP    | C++20 Module files (respectively aggregation headers with legacy builds).
  ALIB_INL    | C++ header files which are included by the <em>.mpp</em>-files.
  ALIB_CPP    | Compilation units.
  ALIB_HPP    | Legacy Header, containing mostly necessary preprocessor definitions.
  ALIB_H      | Module access headers. The only header files to be included by using code. Usable in both modes, C++20 and legacy. See chapter \ref alib_manual_modules_impludes).
<p>

\I{################################################################################################}
### 5.4.3 Cached CMake Variables ### {#alib_manual_build_cmake_3}
The script will create a set of cached boolean \e CMake variables (which are variables that can be
edited with CMake GUI tools and various C++ IDEs).

The following variables correspond directly to \ref GrpALibPreproSymbols "preprocessor symbols"
used for code selection, and thus this list links to their corresponding documentation:

- \ref ALIB_CAMP_OMIT_DEFAULT_RESOURCES
- \ref ALIB_CHARACTERS_SIZEOF_WCHAR
- \ref ALIB_CHARACTERS_WIDE
- \ref ALIB_DEBUG
- \ref ALIB_DEBUG_ALLOCATIONS
- \ref ALIB_DEBUG_ARRAY_COMPRESSION
- \ref ALIB_DEBUG_CONTAINERS
- \ref ALIB_DEBUG_BOXING
- \ref ALIB_DEBUG_CRITICAL_SECTIONS
- \ref ALIB_DEBUG_MEMORY
- \ref ALIB_DEBUG_RESOURCES
- \ref ALIB_DEBUG_STRINGS
- \ref ALIB_EXT_LIB_THREADS_AVAILABLE
- \ref ALIB_FEAT_BOOST_REGEX
- \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
- \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
- \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
- \ref ALIB_FEAT_SINGLETON_MAPPED
- \ref ALIB_FILES_FORCE_STD_SCANNER
- \ref ALIB_PRECOMPILED_HEADER
- \ref ALIB_SINGLE_THREADED
- \ref ALOX_DBG_LOG
- \ref ALOX_DBG_LOG_CI
- \ref ALOX_REL_LOG
- \ref ALOX_REL_LOG_CI

In addition, the following further cached variables are set:
                           
- <b>ALIB_CLANG_USE_LIBCPP</b><br>
  If the to true and the Clang compiler is used, then the LLVM \e libc++ library is used instead
  of the default \e libstdc++. The variable is evaluated in CMake function \b ALibSetCompilerAndLinker
  (see \ref alib_manual_build_cmake_5 "section below").
  Technically, this variable adds the option <em>-stdlib=libc++</em> to the compiler command line
  as well as <em>c++</em> to the list of link-libraries of the linker command line.
  \note In case precompiled 3rd-party libraries are used which are not compiled against
        the LLVM \e libc++ library, the build will fail. Hence the variable is set to false
        by default and should only be used if all libraries used by a project are known to be
        compiled against the LLVM \e libc++ library.
  
<p>.


- <b>ALIB_VERSION</b><br>
  Defines the \alib library version. This variable cannot be changed, respectively will be
  overwritten on \e CMake generation. It is specified as a cached \e CMake variable just for the
  reason of presenting the \alib version to tools that allow to modify \e CMake settings.

  Furthermore, non-cached version variables \c ALIB_VERSION_NO and \c ALIB_VERSION_REV are
  set which hold the version number and revision number as separate integral values.

- <b>ALIB_CMAKE_VERBOSE</b><br>
  If set to \c true, extended information will be printed with running the \e CMake script.
  See \ref alib_manual_build_more_otherbuildsystems for details.
             
  \anchor ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH
- <b>ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH</b><br>
  If set to \c true, the provided script will not search for a thread library on the target
  platform.
  In debug-compilations of the library, if the compiler-symbol \ref ALIB_SINGLE_THREADED is passed,
  still a multi-threading library is of use. This is because with such a build, the library
  assures single-threaded use: An \alib_assertion is raised when multi-threading is detected!
  Therefore, the target system's thread library is still searched and linked, unless this
  symbol is set.<br>
  If found, symbol \ref ALIB_EXT_LIB_THREADS_AVAILABLE is passed to the C++ compiler.

  The way to prevent searching and adding a thread library is by setting special <b>CMake</b> cached
  variable \ref alib_manual_build_cmake_3 "ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH" to \c true.

- <b>ALIB_DEBUG_GLIB</b><br>
  If \c true, symbols \b _GLIBCXX_DEBUG, \b _GLIBCXX_DEBUG_PEDANTIC and
  \b _GLIBCPP_CONCEPT_CHECKS are passed to the compiler.

- <b>ALIB_COVERAGE_COMPILE</b><br>
  If \c true, option <b>--coverage</b> is added to CMake variables \b ALIB_COMPILER_OPTIONS and
  \b ALIB_LINKER_OPTIONS


\I{################################################################################################}
### 5.4.4 CMake Build-Setting Variables ### {#alib_manual_build_cmake_4}
The script will create the following non-cached \e CMake variables, which can be used to define
build-settings of custom projects:

- <b>ALIB_SYMBOLS</b><br>
  Will contain the \ref GrpALibPreproSymbols "ALib preprocessor symbols", as defined by the cached variables.
  This variable can, for example, be used as a parameter to \e CMake function \c target_compile_definitions().
<p>

- <b>ALIB_COMPILER_WARNINGS</b><br>
  Will contain compiler parameters to set the (high!) warning level used with compiling \alib.
  This variable can, for example, be used as a parameter to \e CMake function
  \c target_compile_options().<br>
  Custom entries may be added to the list before invoking \b ALib.cmake.
  If the entry <em>"ALIB_SUPPRESS_COMPILER_WARNINGS"</em> is found, that entry is removed and no
  compiler-specific warning settings are added to this symbol.
<p>

- <b>ALIB_COMPILER_OPTIONS</b><br>
  Will contain parameters to be passed to the compiler when linking \alib.
  This variable can, for example, be used as a parameter to \e CMake function
  \c target_compile_options().
<p>

- <b>ALIB_LINKER_OPTIONS</b><br>
  Will contain parameters to be passed to the linker when linking \alib.
  This variable can, for example, be used as a parameter to \e CMake function
  \c set_target_properties().
<p>

- <b>ALIB_EXTERNAL_LIBS</b><br>
  Will contain a list of external libraries needed to build \alib.
  This variable can, for example, be used as a parameter to \e CMake function
  \c target_link_libraries().

In addition, the following non-cached variable is an input variable which may be set before
invoking the script:

- <b>ALIB_COMPILER_FEATURES</b><br>
  May contain \e CMake feature parameters to be passed to the compiler.
  This variable can, for example, be used as a parameter to \e CMake function
  - \c target_compile_features().

\I{################################################################################################}
### 5.4.5 CMake Functions Defined By The Script ### {#alib_manual_build_cmake_5}
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
  \p{target}. In addition, the target's CMake property \c POSITION_INDEPENDENT_CODE is switched
  on. Furthermore, CMake variable \b ALIB_CLANG_USE_LIBCPP is evaluated and, if set to \c true
  and Clang compiler is used, the option \c -stdlib=libc++ is added to the compiler command line
  as well as \c c++ to the list of link-libraries of the linker command line.
<p>

\I{################################################################################################}
### 5.4.6 Library Filename ### {#alib_manual_build_cmake_6}
The library's filename is determined by (non-cached) \e CMake variable \c ALIB_LIBRARY_FILENAME.
If this variable is \b not set before invoking script <c>ALib.cmake</c>, then the name is
determined automatically from the selected modules.

The name is used with functions \c ALibAddStaticLibrary and \c ALibAddSharedLibrary, introduced
in the previous section.

If not provided, the name will be assembled according to the following set of rules:

- The name starts with \c "alib_".
- The library version is appended, for example, \c "2511R0".
- In the case of debug-builds, the term \c "_DBG" is appended.
- In the case that variable \b ALIB_SINGLE_THREADED is set, the term \c "_ST" is appended.
- In the case that all modules are selected in the \alibbuild, no information about modules is
  appended.
  Otherwise, each selected module's name is added in capital letters, separated by an underscore
  character (<c>'_'</c>) unless a module is not "superseded" by another module that has a
  \ref alib_manual_modules_graph "mandatory dependency" to it.
- If one of the variables
  - ALIB_DEBUG_MEMORY,
  - ALIB_DEBUG_CONTAINERS,
  - ALIB_DEBUG_STRINGS, or
  - ALIB_DEBUG_RESOURCES
      
  is given, then the prefix <c>"DBG"</c>" is added to the module name appended.

- In the case that \alib_alox is included, the suffixes <c>_ALOX</c> is extended by
  suffix <c>NRL</c> if release logging is switched off. In addition, suffix <c>NDL</c> is added with
  debug-builds that do not include debug logging. Note that both exceptions cover the
  non-default, unusual compilation cases in respect to the availability of debug- and release logging.


\I{################################################################################################}
## 5.5 Further Information ## {#alib_manual_build_more}

### 5.5.1 Other Build Systems ### {#alib_manual_build_more_otherbuildsystems}

As already explained, the \e CMake build process is viewed to be the reference process for \alib
compilation.

For none\e CMake users, the selection of the exact minimum set of source and header-files,
is probably the most difficult (and annoying) task.
In \e CMake we have separated the source selection into script:

            ALIB_BASE_DIR/build/cmake/ALibSources.cmake

which is invoked by the main script.

This script might be analysed to identify the source and header-file dependencies of the
different \alibmods.

Likewise, script

            ALIB_BASE_DIR/build/cmake/ALibModules.cmake

might be analyzed to get actual (and correct) information about module dependencies.


Furthermore, by setting CMake cache variable \b ALIB_CMAKE_VERBOSE to \c true, running \e CMake
will write extended information that might be used and copied into the configuration files
of other build systems or into IDE project files.

Among the data displayed is:
- List of modules included in the build.
  (Modules that the user \ref alib_manual_build_cmake_1 "explicitly selected" plus recursive
  dependencies)
- List of modules excluded from the build.
- List of source files that are needed for the build.
- The resulting library filename.
- The base folder of source files.
- The compiler-symbols (definitions) passed.
- The compiler warning flags.
- The compiler features and other flags.
- The linker flags
- External libraries used.

\I{################################################################################################}
### 5.5.2 Choosing C++ Language Version ### {#alib_manual_build_more_cppversion}

As demonstrated in the chapter \ref alib_manual_build_sample (Step 3),
\e CMake variable \c ALIB_COMPILER_FEATURES may be used to determine the C++ language standard for
\alib targets and optionally for custom targets (i.e. if function \c ALibSetCompilerAndLinker is
invoked for a custom target).

\alib requires C++ language level \c 20 as a minimum and is compatible with level \c 23.

\e CMake provides other (maybe even more preferable) mechanics to determine/set the C++ language
level, which of course may be used alternatively.

\I{################################################################################################}
### 5.5.3 Copying The Sources ### {#alib_manual_build_more_copysources}

If \e CMake variable \c ALIB_SOURCE_COPY_TARGET_DIR is set before invoking the CMake script
\c %ALib.cmake, then the source files are copied to the directory specified in the variable.
If the variable does not point to a valid directory, an error is raised and the \e CMake script
is stopped.

The files copied represent exactly the set of files which are needed for compiling the
selected \alibbuild (combination of \alibmods), which is optionally specified with the variable
\c ALIB_BUILD (see \ref alib_manual_build_cmake_1).

This feature therefore can be used to create a fresh, filtered copy of the \alib source tree
tailored to an application. Nevertheless, it is usually not recommended to do so, because
source files that are not used by a combination of modules are neither included as header-files,
nor compiled. The feature is rather used by the \alib developers to verify module and source code
dependencies.

\I{################################################################################################}
### 5.5.4 External Library Dependencies ### {#alib_manual_build_more_externallibs}
ALib has <b>no mandatory</b> dependencies to external (3rd-party) libraries.
The following \b optional dependencies exist:

1. Boost RegEx<br>
   If enabled by the compiler-symbol \ref ALIB_FEAT_BOOST_REGEX, a dependency to
   \https{boost,www.boost.org} library component
   \https{regex,www.boost.org/doc/libs/1_68_0/libs/regex/doc/html/index.html} is made.
   As a result, the following \alib features become available:
   - In module \alib_strings, class \alib{strings::util;RegexMatcher}
   - In module \alib_expressions, with expression compiler plugin \alib{expressions::plugins;Strings},
     expression term \b RegExMatch.
                            
2. Standard C++ library<br>
   Some support to make types of the C++ standard library compatible with various concepts of
   \alib are activated by including <em>"compatibility header-files"</em>.
   Those are named <b>"ALib.Compatibility.Std<NAME>"</b>
   For more information, see the following namespace documentation:
   - #alib::characters::compatibility::std
   - #alib::strings::compatibility::std
   - #alib::boxing::compatibility::std

3. QT Library<br>
   If \alib is used in combination with the \https{QT Class Library,www.qt.io}, compatibility
   support for character array types (strings and character vectors) of the <em>QT Library</em>
   becomes available with modules \alib_boxing and \alib_strings.<br>
   Such support is activated simply by including <em>"compatibility header-files"</em>
   named <b>"ALib.Compatibility.QT<NAME>"</b>

   For more information, see the following namespace documentations:
   - #alib::characters::compatibility::qt
   - #alib::boxing::compatibility::qt

\I{################################################################################################}
\I{##########  6.  Using the library                                                               }
\I{################################################################################################}

# 6. Using The Library # {#alib_manual_use}
There is little to say here, so we note just some bullet points:

- A shortest C++ program is given at the start of the
  \ref alib_mod_bs "Programmer's Manual" of module \alib_bootstrap_nl.
  From there, a user of the library should start directly with the topic and module of
  interest. Often, the dedicated Programmer's Manuals linked
  \ref alib_manual_modules_table "in this table" provide some sample code to start with.
- To access an \alibmod, one or a few header files have to be included.
  The complete list of available header files to include (respectively \c import) a module
  is given at the end of chapter \ref alib_manual_modules_impludes.
  For example, if a user wants to use \alib for scanning
  a directory and working with file trees, the single include of the header file \implude{Files}
  is sufficient.
- When compiling code against \alib, each compilation unit needs to receive the same compiler
  symbols as were given to the library itself.
  With debug-compilations, this will be checked with the invocation of function \alib{Bootstrap}.<br>
  If different compilation symbols are used, then, in debug-compilations, an assertion will be
  raised with bootstrapping. The mechanics behind this are quickly explained in chapter
  \ref alib_mod_bs_assertcomp of the Programmer's Manual of the module \alib_bootstrap_nl.
- When using \alib with Microsoft Windows OS, it may be important to
  include <em>windows.h</em> before any \alib header-file.
  The reason is that \alib includes <em>windows.h</em> unless it was included already.
  When the \alib library includes <em>windows.h</em>, it uses some defines like
  <em>WIN32_LEAN_AND_MEAN</em> or <em>NOGDI</em> to minimize the impact of that huge header-file.
  If <em>windows.h</em> is included before including ALox (AWorx library), it is up to the
  embedding application which level of windows functionality is needed.

\I{################################################################################################}
\I{##########  6.  ALib’s Compatibility Philosophy                                                 }
\I{################################################################################################}

# 7. ALib’s Compatibility Policy # {#alib_manual_compatibility_policy}

Unlike many libraries that prioritize strict source and binary compatibility across versions,
\alib favors continuous refinement. We refactor aggressively when it improves clarity, structure,
or performance. This includes renaming types, reshaping APIs, and relocating entities into more
fitting modules or namespaces—even when their semantics remain unchanged.

\par The Pros are:
- Clearer architecture over time: APIs and types move to where they logically belong.
- Better names, better usability: renames improve discoverability and intent.
- Less technical debt: obsolete patterns are removed instead of preserved indefinitely.
- Faster adoption of modern C++: designs evolve with language and tooling progress.

\par Cons:
- Breaking changes occur: upgrades require code adaptations.
- Migration effort: renamed or relocated types and headers need updating.
- Version coupling: consumers should align versions across projects more deliberately.
- Documentation alignment: reading updated manuals/changelogs is advisable during upgrades.

<b>What this means for you:</b><br>
Expect improvements that may require small, mechanical edits (e.g., updated includes,
namespaces, or type names).
The detailed \ref alib_changelog calls out impactful refactorings and suggests migration paths.
With each release, the extensive \ref alib_manual_modules_table "module manuals" are likewise
updated. They provide detailed information about the APIs and their use.<br>
Finally, the built-in bootstrapping mechanism checks that your executable is compiled against
the same version that it uses, and whether both share the same feature flags.

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
- Templated types UpperCamelCase with a leading <b>'T'</b>, for example, \b TString.
  Those template types which are not \ref alib_manual_appendix_typealiases "aliased"
  (like \b TString is aliased to \b String, \b WString, \b NString etc.), often are missing the
  leading <b>'T'</b>.
  This is for better readability of the end-user code and also to just indicate that the direct use
  of the templated class is the common use.
  This is why, for example, class \alib{containers;SharedVal} carries no leading <b>'T'</b>,
  while class \alib{monomem;TSharedMonoVal} does.
- Template parameters types are also UpperCamelCase with a leading <b>'T'</b>.
  Only in seldom cases, a template parameter is just named <b>"T"</b>.
  Usually the name should indicate what kind of templated type is expected or what is to be done
  with it.
- Type traits that might specialized by users of this library are UpperCamelCase with
  the postfix of <b>"Traits"</b>. For example, \alib{characters;ArrayTraits} or
  \alib{boxing;BoxTraits}.
- C++ Concepts usually are prefixed <b>"Is"</b>, for example, with
  \alib{characters;IsImplicitArraySource},
  \alib{strings;IsAppendable} or \alib{boxing;IsCustomized}.
  In fewer cases, the prefix <b>"Has"</b> is used, for example, with
  \alib{resources;HasResources} or \alib{enumrecords;HasRecords}.
- Methods and field-members that are available only with debug-builds are prefixed
  <em>"Dbg"</em>. Often, these members remain available in release-builds but are then empty
  and probably static and constexpr. This way they are optimized out by the compiler, while
  their use does not have to be pruned using the preprocessor (i.e., macro \ref ALIB_DBG()
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
  We could now say that the reason for often being quite "verbose" is that the manual address
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
compilation unit, \alib exposes all important types into this outer namespace with type definition
statements (C++ keyword \c using).
For example, at the end of header-file \alibheader{expressions/compiler.inl},
just before closing outer namespace \b alib, the following statement is made:
\snippet "expressions/compiler.inl"    DOX_MANUAL_ALIASES_COMPILER
With that, a single:

     using namespace alib;
     
fits for all \alib types and in most cases, this single using statement is all that is
needed. This approach does not generate conflicts: If, for example, the using code would have
an own class named "Compiler", it can fall back to the standard schematic, by adding just:

     using namespace alib::threads;
     using namespace alib::strings;
     
and accessing class Compiler explicitly as <c>alib::expressions::Compiler</c>.

\note
  The reason why all this is possible is due to (a next great) design decision of C++.
  While the \em CODE in the following snippet:
         namespace A { namespace B { CODE } }
\note
  "sees" all types in namespace \c A, with this snippet:
         using namespace A::B;
         CODE
\note
  the \e CODE does not "see" types of namespace \c A, but only those of \c B.
  
### A.3.2 Aliases of Template Types   ###
Most of the type aliases are as simple as sampled with class \b Compiler above.
Also templated types are often just a 1:1 alias, for example:
\snippet "monomem/sharedmonoval.inl"    DOX_MANUAL_ALIASES_SHAREDMONOVAL

But sometimes template parameters become predefined with the alias.
Often (but not always) with that the name changes, i.e., the leading \b T is removed.
As an example, take:
\snippet "files/ftree.inl"    DOX_MANUAL_ALIASES_FTREE

In some few situations, mostly due to technical reasons, template parameters are even changing
their position. Of course, some special care has to be taken if such types are used
aliased or not. As an example, take:
\snippet "lang/bitset.inl"    DOX_MANUAL_ALIASES_BITSET

Finally, often several different aliases are defined in namespace <b>%alib</b>, providing
different permutations of template types.
For example, type #alib::strings::TAString is aliased as:
- \ref alib::AString,
- \ref alib::NAString,
- \ref alib::WAString,
- \ref alib::AStringMA,
- \ref alib::NAStringMA,
- \ref alib::WAStringMA,
- \ref alib::AStringPA,
- \ref alib::NAStringPA, and
- \ref alib::WAStringPA.

           
\I{################################################################################################}
## A.4 Type-Traits, Concepts and Adapters ## {#alib_manual_appendix_tca}
### A.4.1 Terminology ###

Understanding the interplay between \e "concepts", \e "type-traits", and \e "adapters" is essential
for mastering modern C++ programming, especially when working with libraries like \alib.
Here's a concise overview of these elements and their interactions:

\par Type-Traits:
Introduced in C++11, type-traits are templates that provide compile-time information about types.
They enable metaprogramming by allowing developers to query and manipulate type properties.
For instance, <c>std::is_integral<T>::value</c> checks if T is an integral type.


\par Concepts:
Added in C++20, concepts define a set of requirements (such as functions, types, or constants)
that a type must satisfy.
They act as compile-time predicates, ensuring that template parameters meet specific criteria,
leading to clearer and more robust code.
For example, a concept can require that a type offers certain member functions or provides
a certain type alias.

\par Adapters:
In C++, adapters are \https{software design-patterns,en.wikipedia.org/wiki/Software_design_pattern}
that allow incompatible interfaces to work together.
They "adapt" one interface to another, facilitating code reuse and flexibility.
For instance, stream adapters can modify data streams to fit different input/output interfaces.


### A.4.2 Use in ALib ###

Most of the \e concepts introduced by \alib are formulated as expressions that depend on the
specialization of certain \e type trait structures.
Then, the methods that use these concepts as requirements often leverage other mandatory members of
the same type-trait structures for their implementation.
The following scheme illustrates this relationship:

\dot
digraph AdapterUse
{
    node [shape=record, fontname=Courier,   fontsize=12];
    edge [shape=record, fontname=Helvetica, fontsize=10];
    typetraits [ label="Type-Traits"];
    concept[ label="Concept"];
    adapter[ label="Adapter::Method"];
    concept -> typetraits[ arrowhead="open" label= "Requirement expression" ];
    adapter -> typetraits[ arrowhead="open" label= "Implementation" ];
    adapter -> concept[ arrowhead="open" label= "Selection" ];
}
\enddot
 

This design ensures:
- <b>Flexibility:</b><br>
  Users can extend or modify type behaviors by specializing type-traits without altering the core
  library code.

- <b>Type Safety:</b><br>
  Concepts enforce that only types meeting specific criteria are used in particular contexts,
  preventing misuse.

- <b>Least Intrusive:</b><br>
  By specializing \alib type-traits, users define how their types interact with ALib's templates
  and functions.
  Such specialization can be performed in code units which are separated from the user's type
  definition.
  Concepts then validate these types at compile-time, ensuring they conform to the expected
  interfaces.
  This layered approach allows seamless interaction between user-defined types and
  \alib's functionalities without introducing unnecessary dependencies on \alib in the using code.

- <b>Easy Adaptability:</b><br>
  By adhering to this <em>design pattern</em> across \alib, users are quickly directed towards
  what they need to do to adapt its custom types - without reading through our (lengthy)
  Programmer's Manuals.

The latter claim about "easiness" should be quickly sampled:
- A user has an instance of type "MyType".

        MyType myInstance;
          
- A user wants to append a string representation of that type to an \alib \b %AString and writes:

        AString as;
        as << myInstance;

- This creates a compiler error-message.
  Among the different hints of the compiler, the user will see something like this:
                         
        note: because 'alib::strings::IsAppendable<MyType, char, alib::lang::HeapAllocator>'
                                                             
- If the user clicks this line, the IDE will show the adapter method that is restricted by the
  concept \alib{strings;IsAppendable}.
   
- From here, the user can navigate to the definition of this concept.
  There he will quickly see that this concept's constraint string exclusively relies on
  the type trait \alib{strings;AppendableTraits}.
   
- The next click in the IDE will lead to the declaration of the type trait.
  Its reference documentation explains on how to specialize it for \b MyType - and that is all
  the user has to do.
 

### A.4.3 Further Reading ###
For a deeper understanding, consider exploring the following resources:
                       
- \https{C++ Reference on Type-Traits,en.cppreference.com/w/cpp/header/type_traits}:
  Provides detailed information on standard type-traits and their usage.
    
- \https{C++ Reference on Constraints and Concepts,en.cppreference.com/w/cpp/language/constraints}:
  Offers insights into how concepts are defined and applied in C++20.

- \https{C++ Short Stories: Type-Traits\, Concepts\, and Type Constraints,www.jakubkonka.com/2017/09/02/type-traits-cpp.html}:
  An article that delves into the practical applications of type-traits and concepts in modern C++.

These resources provide foundational knowledge that complements the design and usage patterns
employed in \alib.

\I{################################################################################################}
## A.5 Collecting Caller Information ## {#alib_manual_appendix_callerinfo}
A core class included in any \alibbuild is \alib{lang::CallerInfo}.
It stores the following information:
- Filename, line number, and function name received with corresponding built-in preprocessor
  symbols \b __FILE__, \b __LINE__ and a compiler-dependent symbol which is fetched with
  \ref ALIB_CALLER_FUNC.
- The C++ type id, fetched with keyword \c typeid, usually passing argument <c>*this</c>.
- The C++ thread id of type <c>std::thread::id</c>. This is only included if the compiler-symbol
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
   
This is because they use the keyword <c>this</c> to identify a caller's type.
Unfortunately, the C++ language in combination with the preprocessor does not allow the automatic
detection whether it is legal to use keyword \c this or not .
For this reason the core macro \ref ALIB_CALLER has to be redefined before a code unit
defines functions and restored back afterward.
To do this, header-files \implude{Lang.CIFunctions} and \implude{Lang.CIMethods}
are provided.
The following example demonstrates how they are to be used:

  \snippet "ut_lang.cpp"    DOX_LANG_CALLER

\I{################################################################################################}
## A.6 T.HPP-Files And Custom Template Instantiations ## {#alib_manual_appendix_t_inl_files}

\I{################################################################################################}
### A.6.1 The Challenge and ALib's Solution ### {#alib_manual_appendix_t_inl_files_challenge}
In C++, templates offer a powerful mechanism for writing generic and reusable code.
But using templates extensively in header-files leads to longer compilation times and
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

1. The internal <b>INL-file</b> (.inl) used for the template class declarations exported with
   the C++ module.<br>
   This header is missing the definition of those methods that should not be compiled with a
   unit that does not instantiate a templated type with a specific combination
   of template parameters.<br>
   In other words, the contents of this file is exposed by importing the module and is not relevant
   for a user.
2. A <b>T.INL-file</b> (.t.inl) providing the missing (and and not inlined) method definitions.<br>
3. A <b>CPP-file</b> (.cpp) that includes both, the declaration and definition files, and explicitly
   instantiates the template for specific, commonly used types.

This approach results in improved compilation speed, code modularity, and manageable build times
while still allowing users the flexibility of creating custom instantiations when necessary.

\I{################################################################################################}
### A.6.2 Instantiations of Custom Types ### {#alib_manual_appendix_t_inl_files_customize}
With this recipe in place, it is fairly straight forward to instantiate your custom
version of an \alib-type of that sort. Let's quickly look at a sample.

For class #alib::monomem::TMonoAllocator, provided with module \alib_monomem, only one built-in
template instantiation is provided.
This specifies the single template parameter \p{TAllocator} to be of type
\alib{lang;HeapAllocator}. This instantiation is then aliased as #alib::MonoAllocator.<br>
The corresponding files are:

- \alibheader{monomem/monoallocator.inl},
- \alibheader{monomem/monoallocator.t.inl}, and
- \alibheader{monomem/monomem.cpp}

Let us look at the compilation unit (the \b CPP-file):

\snippet "monomem/monomem.cpp"        ALIB_MANUAL_T_HPP_FILE_USAGE

That is basically all that the CPP file needs to contain.
Of course, with optional C++20 Module compilation of \alib, the file got a little more complex,
so please refer to the sources for complete insights.

With this in mind, we can now instantiate a custom mono-allocator, one that uses a \b PoolAllocator
as its memory source. This is done in the \alib unit-tests. Here is the excerpt:

\snippet "containers_monomem/ut_monomem_instantiation.cpp"         ALIB_MANUAL_T_HPP_FILE_USAGE_IMPL

Note that the compilation unit - although it is your own file and is located outside \alib -
<b>still needs to specify</b> <c>module ALib.Monomem;</c>.
Because any compilation unit must only specify one module, you have to place this code into
a dedicated compilation unit specifically created for this purpose.

Next, create your custom header-file or place it anywhere in your existing headers.
(In the case of C++20 Modules, do not use the \c export keyword.)
There, copy the instantiation code and place keyword <c>extern</c> in front, to turn it into a
declaration:

\snippet "containers_monomem/ut_monomem.cpp"               ALIB_MANUAL_T_HPP_FILE_USAGE_DECL


This announces that the linker will find the instantiation.

With this in place, we can start using the instantiation:

\snippet "containers_monomem/ut_monomem.cpp"               ALIB_MANUAL_T_HPP_FILE_USAGE_USAGE

To summarize, the fast track approach when a specific instantiation of an \alib type that
splits some method definitions from the type declaration is needed, is to have a look at the
compilation unit (the CPP-file), copy its code to an own compilation unit, and replace the given type
with your own type.


\attention
  With WindowsOS and DLL-compilation, things may become more complicated.
  Here, the compiler-symbol \ref ALIB_DLL has to be set rightfully for the code-unit that implements
  an instantiation.
  This manual cannot go into further details of this topic.
  Instead, general information on compiling and linking under WindowsOS has to be consulted.

\I{################################################################################################}
## A.7 Pretty Printers # {#alib_manual_prettyprinters}
For gdb (GNU Debugger), some "pretty printers" are available. Please consult page
\ref alibtools_debug_helpers_gdb.
                   

<br><br><br><br><br><br> */
