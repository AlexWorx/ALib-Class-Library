// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_changelog         Change Log
                                        
\I{################################################################################################}
\I{############################        Version 2510 R0       ######################################}
\I{################################################################################################}

# Version 2510, Revision 0, released October 1st, 2025 #

The main decision that lead to the changes of this release, was 
<b>dropping language support below C++20</b>. With that, this release constitutes a major update 
of \alib and every single file of the library was touched. 

The downside is: Users of \alib that for any reason cannot switch to C++20, have to continue 
working with the previous \alib release <b>V2412</b>.

\par Tested Platforms
The minimum required CMake version is <b>3.20</b>.
This version was successfully tested under the following platform and toolchain combinations:
- GNU/Linux Arch 6.16.8, Clang++ 20.1.8, C++20/23, 32-Bit / 64-Bit, optional <b>C++20 Module Support</b><br>
  (This is the main development platform.)
- GNU/Linux Arch 6.16.8, GNU C++ 15.2.1, C++20/23, 32-Bit / 64-Bit
- WindowsOS 11, MSC 19.44 (Visual Studio 2026 Insiders, Platform v145), C++20, 32-Bit/64-Bit
- WindowsOS 11, MinGW, 64-Bit, GCC 13.47, C++20
- macOS Tahoe 26.0, Apple M2 / ARM64, Apple Clang Version 17.0.0, C++20/23, 64-Bit
- Raspberry 3, aarch64, Cortex-A53, GNU C++ 12.2.0, C++20/23   
- Raspberry 4, aarch64, Cortex-A72, GNU C++ 12.2.0, C++20/23   
- Raspberry 4, armhf (32-bit), Cortex-A72, GNU C++ 12.2.0, C++20/23 


\par C++20 Modules:
We added some (!) support for C++20 Modules. 
Because of this, we were forced to perform a complete overhaul of the library structure,
the previously 17 <em>"ALib Modules"</em>, are now split into 25!
Thus, the structure and \ref alib_manual_modules_graph "dependency graph" of the existing \alibmods 
has been changed. 

We managed to retain the previous flexibility, allowing users to exclude specific 
<em>"ALib Modules"</em> from the build. 
This approach helps minimize both build times and the size of the resulting executable. 
However, due to technical reasons, two small changes have been made in this regard:
- Module \alib_time is not selectable anymore. It will always be included in the 
  \alibbuild.
- Module \alib_threads is now included in the build in case the new compiler-symbol
  \ref ALIB_SINGLE_THREADED is \b not provided. (Before, this module was included using the
  now removed symbol \b ALIB_THREADS).
      
Also, by nature, <b>all header files have changed (!)</b>, because with C++20 Modules we had to 
give up the previous <em>"one class - one header"</em> approach. 
But we consider the new header files much easier to use.<br>  
Please refer to the (rewritten) manual chapter \ref alib_manual_modules for all
details about \alibmods_nl, C++-modules and header files.<br>

\note We consider the current state of the use of C++ 20 Modules by \alib to be in an alpha
      state. It works only with very recent clang compiler and we are all but satisfied with
      the results. We will observe the evolution of the language and the compilers and will
      adjust the \alib accordingly. But for now, we consider low-level libraries like \alib not 
      being eligible for C++ 20 Modules.<br>
      We have created a blog-page dedicated to share our \ref alib_c20module_shift.
         
\par C++20 Concepts
All of \alib is now free of (ugly) template meta programming using <c>std::enable_if</c> and
uses new keywords \c concept and \c requires instead.
(An overview of all concepts introduced by \alib [is found here](concepts.html).)<br>
The implications are:
- Every <c>std::enable_if</c> and corresponding "helper-template-parameters" have been
  removed. Altogether \b 493 replacements across the library! 
- Removed almost all former template-meta-programming macros, as their use was mostly by C++20 
  concepts or similar built-in type traits. The names of the removed macros all started with 
  <b>ATMP_</b>. Now, the two remaining macros were renamed to use prefix <b>ALIB_</b> as all
  other macros do.<br>
  The remaining macros are \ref ALIB_TVALUE and \ref ALIB_HAS_METHOD.        
- Changed the \ref alib_manual_appendix_naming "ALib naming conventions". 
  Type-trait types, which had been prefixed with <b>T_</b> are now instead suffixed by
  <b>Traits</b>. For example, this affects:
  - <b>alib::characters::T_CharArray</b> -> \alib{characters;ArrayTraits}.  
  - <b>alib::characters::T_ZTCharArray</b> -> \alib{characters;ZTArrayTraits}.  
  - <b>alib::strings::T_Append</b> -> \alib{strings;AppendableTraits}.
- Helper structs using template metaprogramming, which formerly were named with the prefix 
  <b>TT_</b> have been replaced by modern idioms like concepts, type-aliases, etc. 
  As an example, former types <b>alib::characters::TT_CharArrayType</b> and 
  <b>alib::characters::TT_ZTCharArrayType</b>  were replaced by concepts \alib{characters;IsArray}
  and \alib{characters;IsZTArray} along with type aliases \alib{characters;Type} and 
  \alib{characters;ZTType}.<br>
  Many more similar replacements have been performed. Please, consult the reference- and 
  programmer's-manuals of the corresponding modules in case your code does not compile 
  with a type prefixed <c>T_</c>, or <c>TT_</c>.      

\par New Module Assert 
This new module contains entities to raise <em>ALib Assertions</em>, <em>ALib Warnings</em>
and other messages. It is implemented using the C++ standard libraries and its string-types.
This replaces the former plug-in based concept implemented with now removed types 
\b Report and \b ReportWriter.<br> 
Likewise, the former plugin \b ALoxReportWriter was removed. However, as the new assertion system
still supports a plug-in mechanism, module \alib_alox now plugs into it with its new 
function \alib{lox;ALoxAssertionPlugin}.<br>
The concepts are primarily made for internal use, but can be used by any software likewise.
Thus, a corresponding \ref alib_mod_assert "Programmer's Manual is available". 
         
\par New Module Bootstrap 
The bootstrap process itself did not change. Nevertheless, all functionality is now gathered
in the new module \alib_bootstrap.
                                                      
\par Further General Changes
- Renamed module <b>ALib Configuration</b> to \alib_variables. 
  In fact, this should have happened with the last release already, where this module was rewritten  
  from scratch.
- Removed documentation of symbols \b ALIB_GDB_PP_FIND_POINTER_TYPES and 
  \b ALIB_GDB_PP_SUPPRESS_CHILDREN (which had been discarded in the previous release already).  
- Added macro \ref ALIB_WARNINGS_IGNORE_DEPRECATED.  
- Removed macros \b ALIB_FORCE_INLINE and \b ALIB_NO_RETURN.  
- Removed macro \b ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE because clang compiler became too strict
  with that flag and it was necessary just too often. Instead, the warning is now disabled globally.  
- Renamed macro \b ALIB_DEBUG_MONOMEN to \ref ALIB_DEBUG_MEMORY and removed its responsibility
  from module \alib_monomem to let it have a general library association.  
- The use of clang's \b libc++ under GNU/Linux is now supported with the new CMake variable
  \b ALIB_CLANG_USE_LIBCPP, which is used with CMake function 
  \ref alib_manual_build_cmake_5 "ALibSetCompilerAndLinker". 
- Added the compiler option \b /utf-8 for MSVC with the shipped project files. 
  
\par Module Characters:
- Added helper \alib{characters;ArrayLength}.

\par Modules EnumOps and EnumRecords:
- Former \alibmod_nl <b>"Enums"</b> was split into the modules \alib_enumops and \alib_enumrecords.
- Former joint namespace \c alib::enums was replaced with #alib::enumops and #alib::enumrecords.
- The bootstrap facilities of module \alib_enumrecords_nl have been extracted and moved 
  to sub-namespace #alib::enumrecords::bootstrap.
- The overloaded function of method \alib{enumrecords::bootstrap;Bootstrap} which accepts a 
  \alib{camp;Camp} instance is now available only after the inclusion of the header \implude{Camp}.    

\par Module Strings:
- The default-constructor of class \alib{strings;TCString} now does not initialize an instance
  to be \e nulled anymore. Instead, no initialization is performed. 
  This is now in alignment with the default constructor of base class \alib{strings;TString}.
- Pointers to instances of \ref alib_strings_assembly_ttostring "appendable" types are not
  accepted by the append-methods of class \b %AString anymore. If desired, the pointer type 
  has to be made \e appendable explicitly (optionally in parallel to the value type).          
- The following classes had been subtypes of the removed type \b TFormat and were moved out:
  -  \alib{strings;TField}, 
  -  \alib{strings;TTab}, 
  -  \alib{strings;TEscape}, 
  -  \alib{strings;TDec}, 
  -  \alib{strings;THex}, 
  -  \alib{strings;TOct}, and 
  -  \alib{strings;TBin}. 

  And, class \alib{strings;TField} has now two different implementations, depending on the inclusion
  of the module \alib_boxing in the \alibbuild.  
- With the inclusion of the header \implude{Compatibility.StdStrings}, specializations of C++20 
  struct <c>std::formatter</c> are now given. This allows to use \alib string types with
  function <c>std::format</c>. 
- Changed inner types and a few method names of classes \alib{strings;TString;String} and 
  \alib{strings;TAString;AString} to gain compatibility with C++ container algorithms.
- Added assign operator to class \alib{strings;TLocalString;LocalString}. (Before, assignment was
  inefficient as the move constructor was used.) 
   
\par Module Boxing:
- With the use of C++20 concepts and some relaxed rules in respect to C++ unions and their use
  in \c constexpr contexts with that language version, module \alib_boxing was overhauled.
- Some former techniques of customizations (to keep things \c constexpr) of boxing could be removed 
  and thus simplified. If this breaks your code, please consult the updated 
  \ref alib_mod_boxing Programmer's Manual      
- Methods \b GetRecord and \b TryRecord of class \alib{boxing;Enum} have been moved to 
  namespace-level with functions \alib{boxing;GetRecord} and \alib{boxing;TryRecord}.
                                                                     
\par Module Containers:
Changed inner types and a few method names of classes \alib{containers;List} to gain compatibility 
with C++ container algorithms.

\par Module ALox:
- Moved methods \alib{lox::Lox;Register} and \alib{lox::Lox;Get} from class \alib{lox;ALoxCamp}
  to class \alib{lox;Lox}. 

\I{################################################################################################}
\I{############################        Version 2412 R0       ######################################}
\I{################################################################################################}
# Version 2412, Revision 0, released December 13th, 2024 #

\par Platforms
The minimum required CMake version is now <b>3.20</b>.
This version was successfully tested under the following platform and toolchain combinations:
- GNU/Linux Arch 6.12.1, GNU C++ 14.2.1 / Clang++ 18.1.8, C++ 17/20/23, 32-Bit / 64-Bit<br>
  (This is the main development platform.)
- WindowsOS 10/11, MSC 19.42 (Visual Studio 2022), C++ 17/20, 32-Bit / 64-Bit
- WindowsOS 10/11, MinGW, GCC 13.47  C++ 17/20, 64-Bit 
- macOS Sequoia 15.2, Apple M2 / ARM64, Apple Clang Version 16.0.0, C++ 17/20/23, 64-Bit
- Raspberry 3, ARM, 64-bit OS, GNU C++ 12.2.0, C++ 17/20/23
- Raspberry 4, ARM, 64-bit OS, GNU C++ 12.2.0, C++ 17/20/23


\par General Changes
- Optimized ALib Homepage for mobile access and several smaller documentation improvements.
- Renamed singleton types derived of class \b lang::Camp along the scheme <em>"XYZCamp"</em>.
- Extended CMake script that defines the library filename to reflect various feature selections.
- Fixed names of various global (namespace) variables to "UPPER_SNAKE_CASE". 
  (As stated in the project's \ref alib_manual_appendix_naming "naming conventions".) 
- Rewritten pretty printers for \alib types and GNU Debugger (gdb). 
- Removed support for \https{Cotire-builds,github.com/sakra/cotire/} because this tool itself
  lost support.
 
\par  Preprocessor Symbols/Macros:
- Removed compilation symbol \b ALIB_AVOID_ANALYZER_WARNINGS.
- Added macro \ref ALIB_WARNINGS_IGNORE_UNUSED_LAMBDA_CAPTURE.  
- Added macros \ref ALIB_WARNINGS_IGNORE_UNUSED_VARIABLE, and 
  \ref ALIB_WARNINGS_IGNORE_UNUSED_FUNCTION (both only needed with unit tests and doxygen samples).
- Fixed macro \ref ALIB_CONCAT to allow internal expansion if macros were given, and thus fixed
  macros \ref ALIB_IDENTIFIER, \ref ALIB_OWN, \ref ALIB_LOCK, \ref ALIB_LOCK_WITH, etc.
- Added macro \ref ALIB_COMMA_CALLER_PRUNED.
- Added template meta programming macros \b ATMP_RESULT_TYPE and \b ATMP_HAS_METHOD.
 
\par Library Core Types
- Added types that support generic allocation mechanisms. Those are:
  - \alib{lang;Allocator} - which is in fact only a prototype for allocators, living justin the documentation,  
  - \alib{lang;HeapAllocator},  
  - \alib{lang;AllocatorInterface}, and  
  - \alib{lang;AllocatorMember}.  
- Added type \alib{lang;Placeholder}.
- Added type \alib{lang;Plugin} in correspondence to the existing class \alib{lang;PluginContainer} and
  revised the latter in accordance.
- Added type \alib{lang;CallerInfo} and replaced any occurrence of "file, line, func"-parameters
  across the library.<br>
  This has some implications on various macros that collect caller information, especially 
  \alox logging macros, mutex lock macros and assertions. 
  Information is given with the new manual appendix chapter \ref alib_manual_appendix_callerinfo.
- Added namespace functions \alib{lang;IsNull}, \alib{lang;IsNotNull}, and \alib{lang;SetNull}.  
- Added method \b DbgTypeDemangler::GetShort. 
  This method is now used with functor 
  \b T_Append<std::type_info,TChar,TAllocator> to shorten default output.
  If the long output is needed, then a temporary of \b DbgTypeDemangler has to be created
  and the result of its \b Get method has to be appended.   
- Added tag-types \alib{CHK} and \alib{NC}. 
   
- Added sibling types to \alib{lang;Owner}. Those are 
  \alib{lang;OwnerTry}, 
  \alib{lang;OwnerTimed}, 
  \alib{lang;OwnerRecursive}, 
  \alib{lang;OwnerShared}, 
  \alib{lang;OwnerTryShared}, and 
  \alib{lang;OwnerSharedTimed}.<br>
  Along with that, macros \ref ALIB_OWN_RECURSIVE and \ref ALIB_OWN_SHARED were added.  
- Added type \alib{lang;DbgCriticalSections}.
                                        
\par Module Characters:
- Moved all static methods of type <b>alib::characters::CharArray</b> to namespace level.
  The then empty type got removed.
- Added \alib{characters;AlignedCharArray}.
- With both these changes, the header files and their names got refactored.
 
   
\par Module Time:
- Added overloaded constructors and operators allowing shorter code when code mixes \alib types
  and with <c>std::chrono</c>-types. 
  Especially, this allows using C++ literals defined in <c>std::literals::chrono_literals</c>
  with the \alib types. 
                  
\par Module Threads:
- This was largely revised and now reflects the decision to drop support of standards below C++ 17.
- Added dependency to module\alib_time.
- Replaced former types \b ThreadLockNR and \b ThreadLock with types
  - \alib{threads;Lock},
  - \alib{threads;TimedLock},
  - \alib{threads;RecursiveLock}, 
  - \alib{threads;RecursiveTimedLock},
  - \alib{threads;SharedLock}, and
  - \alib{threads;SharedTimedLock}.
- The former "safeness control"-features were dropped. 
- Extended macros \b ALIB_LOCK and ALIB_LOCK_WITH, with corresponding new siblings: 
  ALIB_LOCK_RECURSIVE,             
  ALIB_LOCK_SHARED,                
  ALIB_LOCK_RECURSIVE_WITH, and  
  ALIB_LOCK_SHARED_WITH.     
- Removed class \b SmartLock. 
- Added class \alib{threads;Promise}. 
    
\par Module ThreadModel (New):
This is a new module that introduces a proposal of how to organize threads in an application. 

\par  Module Boxing:
- Fixed support for boxing of type <c>long double</c>. The rationale here was rather a bug: Without
  us noticing, the alignment of type \alib{boxing;Box} had increased to <c>2 x alignof(void*)</c>
  (on some systems). Consequently, the size of class \b Box had increased to 32 bytes
  (with 64-bit systems affected from the bug), in contrast to the 24 bytes that are documented
  (8 alignment bytes had been accidentally added).
- Added methods \alib{boxing;Box::ExportType}, \alib{boxing;Box::ExportValue}, overloaded methods
  \alib{boxing;Box::Import}, as well as a constructor that accepts a pair of an exported type and
  value.
- Former class \b Boxes is templated and called \alib{boxing;TBoxes} with type definitions
  \alib{BoxesHA} (equivalent to the prior type) and \alib{BoxesPA}.

\par Module Enums:
- Added namespace function \b enums::bitwise::HasOneOf.
 
\par Module Strings:
- With dropping the support of C++ language versions below \c 17 (done in the previous release of 
  \alib), the various \ref alib_strings_details_constants "string constants" were converted from 
  namespace functions to true namespace variables. 
  By the same token, following this libraries' conventions, their naming was changed 
  from \e CamelCase to \e SCREAMING_SNAKE_CASE.<br>
  Please expect a lot of replacements in your code, like, for example:
  - <b>EmptyString()</b> -> \alib{EMPTY_STRING}, or   
  - <b>DefaultWhitespaces()</b> -> \alib{DEFAULT_WHITESPACES}.   
- Adopted new allocation mechanics (see below) to class \alib{strings;TAString}.
- Added methods \alib{strings;TString::Allocate}, \alib{strings;TString::Free}, and new constructor
  \alib{strings;TString;TString(TAllocator& allocator, const TString<TChar>&)}, which allow
  placing a copy of a given source string in an allocator and also free such allocated memory.<br>
  The same is available for class \b CString, with \alib{strings;TCString::Allocate},
  \alib{strings;TCString::Free}, and new constructor
  \alib{strings;TString;TCString(TAllocator& allocator, const TString<TChar>&)}. Here an additional
  zero-termination character is allocated and freed.<br>
- With the above in place, removed namespace functions <b>alib::strings::AllocateCopy</b> and
  <b>alib::strings::DeleteString</b>.
- Changed template parameter \p{TCheck} of all methods from \c bool to \c typename with 
  changing the default value from \c true to \alib{CHK}.  
- Removed use of C++ 20 starship operator with string-types. It caused plenty of problems
  with the new overload resolution rules, standard strings and different results with different
  compilers. Finally the use of the original six operators results in far better compile times.       
- Added methods
  \doxlinkproblem{classalib_1_1strings_1_1TAString.html;a342533ac2cb261d19e3024d9d159b84d;AString::DeleteStart(const String &)} and
  \doxlinkproblem{classalib_1_1strings_1_1TAString.html;addbbf1ca4f47f94c41fcd6d5c2aefaa5;AString::DeleteEnd(const String &)}.
- Added optional parameter \p{endIndex} method
  \doxlinkproblem{classalib_1_1strings_1_1TString.html;a02042c2357ac4830da72c80b169538f1;String::IndexOf} and
  and overloaded methods \alib{strings;TAString::SearchAndReplace}.
- Fixed a bug in class \b strings::util::TSubstringSearch, which may have caused wrong
  results.
- Added classes \alib{strings;util::StringEscaper} and \alib{strings;util::StringEscaperStandard}.
  The functionality of these was previously implemented with similar types of camp
  \b Config and got generalized now.
- Added optional second parameter \p{includeSeparator} to \alib{strings;Substring::ConsumeToken}.
- Added an option to set an explicit writeable name to class \alib{strings::util::Token}.
  See methods \alib{strings::util::Token;Define} and \alib{strings::util::Token;GetExportName} for
  more information.
- Changed name of method \b Token::GetRawName to \alib{strings::util;Token::GetDefinitionName}.
- Added class \alib{strings;util::TStringVector}.
- Utility type \alib{strings;util::AutoSizes} now tracks changes to enable session file writing on 
  updates.<br>
  Furthermore, flag \alib{strings::util::AutoSizes::WriteProtected} was added.
- Added flag \alib{strings;NumberFormatFlags;NumberFormatFlags::ReplaceLeadingZerosWithSpaces} and
  corresponding functionality. Previously a similar behavior was only reachable by embedding a
  number in a field or with formatters.
- Removed utility type <b>alib::strings::util::Spaces</b>. (A remainder from the compatibility layer
  that \alib once was with its siblings in Java and C#). More efficient replacement is possible
  with the use of the previously mentioned new type \alib{characters;AlignedCharArray}, as well
  as with the type of the next documented change.
- Added formatting type \b strings::TFormat::Fill.    
- Class \alib{strings::compatibility::std;StringWriter} now adjusts newline characters for
  to Windows- and non-Windows standards.     

\par Module Containers (New):
With the introduction of the generic allocation mechanisms (noted above), the container types
became allocator-agnosic and this way were moved from module \alib_monomem_nl to the new 
module <b>"Containers"</b>. Due to the close relationship of topics, both modules share a joint
\ref alib_mods_contmono "Programmer's Manual". Other changes are:
- Added new type \alib{containers;SharedVal}.
- Added new type \alib{containers;SharedPtr}.
- Added new type \alib{containers;LRUCacheTable} as well as corresponding type
  definitions \alib{containers;LRUCacheMap} and \alib{containers;LRUCacheSet}.
- Class \alib{containers;HashTable} was refactored (simplified!) in respect to its template 
  parameters. Type definitions \alib{containers;HashMap} and \alib{containers;HashSet} remained 
  unchanged, so only users of the "base type" are affected by the change. Please consult the
  \alib{containers;HashTable;reference documentation} of the type for information
  on its details of use.
- Cursors of class \b StringTree can now be
  \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a394ceb5c9dfe91414ab93ce87a743cd8;exported}.
   and constructed from exported values.
- Added operator
  \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a4442b1b9eab40e7939e1e01cdedcb7ae;StringTree::TCursor::operator->}.
  which allows easy access to the custom type stored in a \b StringTree node.
- Added optional template parameter \p{TParent} to method
  \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a75140d731555edad87151e9bec346413;StringTree::TCursor::Tree}
  which allows statically casting the returned object to a type derived from the tree.
- Overloaded methods
  \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;add7694edb9782c64c526909dfc73113b;StringTree::TCursor::AssemblePath}.
  now expect an \b AString of the same character type that the \b StringTree uses.
- Added method 
  \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a90ca31117cef1e45de00b32f9eacf151;TCursor::Distance}.
- Renamed the method <b>StringTree::TCursor::GoToTraversedPath</b> to 
  \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a72d46de8db80f0d507db8013cc41c564;GoTo}, 
  as well as the method <b>StringTree::TCursor::TraversePath</b> to 
  \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a55db5acb390a2d38dd8e30c123cb6e4b;operator()(const NameType&)}.
- Class \alib{containers;StringTree::TRecursiveIterator;RecursiveIterator} now correctly adds
  a trailing path separator char to the path returned with 
  \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TRecursiveIterator.html;a1c4208352f97d0d4664977924c8759b7;CurrentPath},
  in the case it was initialized with the root node of the tree.
- Added method \alib{containers;StringTree::Separator}.
- Renamed method \b StringTree::DeleteRootValue to \alib{containers::StringTree;DestructRootValue}.

\par Module Monomem:
Largely extended and refactored monotonic allocation facilities. To reach this goal, namespace
\b alib::lang (which is always included in any \alib Distribution) received new general allocation
facilities, which use heap allocation (as noted above). 
\par
Note that the changes in this module are far-reaching and can't all be listed here.
Please refer to the \ref alib_mods_contmono "Programmer's Manual", which is almost completely rewritten.
\par
- Types that implement the <c>std::allocator</c> contract are now allocator-agnostic and 
  have been moved out of this module and into namespace #alib::lang. The types are now:
  - \alib{lang::StdContainerAllocator},
  - \alib{lang::StdContainerAllocatorRecycling} and
  - \alib{lang::RTTRAllocator}.
- The container types are now allocator-agnostic and have been moved to the new module 
  \alib_containers_nl.
- Type \alib{MonoAllocator} now is just a type definition into templated type
  \alib{monomem;TMonoAllocator}. The template parameter allows the new concept of 
  \ref alib_contmono_chaining "allocator chaining".
- Added the new class \alib{monomem;TLocalAllocator} which uses stack memory for (the first) allocation
  buffer.
- Added the new class \alib{monomem;TPoolAllocator;PoolAllocator}, which implements a general approach
  to \ref alib_contmono_intro_recycling "memory recycling" applicable to a wide range of use-cases.
- Moved namespace function \b alib::monomem::Destruct to \ref alib::lang::Destruct.
- Former class \b SelfContained has been extended with automatic pointer mechanics and 
  was renamed to \alib{monomem;TSharedMonoVal}. 
- Renamed type definition \b StdContMA to \alib{SCAMono} and introduced sibling definitions
  \alib{StdVectorMono}, \alib{StdVectorPool}, \alib{StdListMono}, \alib{StdListPool}, 
  \alib{StdDequeMono}, and \alib{StdDequePool}.
- Added \b AString type definitions for \alib{MonoAllocator} as well as for new
  allocator type \alib{monomem;TPoolAllocator;PoolAllocator}. Those are:
    \b AStringMA, \b AStringPA, \b ComplementAStringMA, \b ComplementAStringPA,
    \b StrangeAStringMA, \b StrangeAStringPA, \b NAStringMA, \b NAStringPA,
    \b WAStringMA, \b WAStringPA, \b XAStringMA} and \b XAStringPA}.<br>
  In turn, previous type \b TMAString and its former type definitions
  \b MAString, \b ComplementMAString, \b StrangeMAString, \b NMAString, \b WMAString, and 
  \b XMAString were removed.
- If the compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is given, class \b MonoAllocator, as well as the
  two new allocators and type \alib{lang;HeapAllocator}, now add padding bytes around allocations 
  and test for validity when memory is freed. 
  Furthermore, freed memory is overwritten to detect illegal memory access early.
  Likewise method \alib{monomem;MonoAllocator::Reset} now overwrites the released memory
  with this special debug mode.
- Removed <b>MonoAllocator::dbgCheckGlobalAllocatorLock</b> in favor of using the new, generic
  concept of critical sections.  
- Removed convenience functions \b monomem::AcquireGlobalAllocator and \b monomem::ReleaseGlobalAllocator. 

\par  Camp BaseCamp:
- Class \b lang::Exception is now copyable and movable. While it is still recommended to catch
  \b references to instances of class \b Exception, instances may now, for example, be collected in
  lists.
- Method \alib{lang;Exception::Format} now appends the \alib{lang;CallerInfo} 
  to the end of each exception entry.  
- Renamed and refactored the former class \b Directory to \b lang::system::Path. The type was
  never really well-developed, and this was also marked in its docs. Now it is in a quite useful
  state for the first time.
- \alib{SPFormatter} is no of new type \alib{containers;SharedPtr} (was <c>std::shared_ptr</c> before).
- Virtual method \b lang::format;Formatter::Default now returns a value of type \b SPFormat.
- Removed static method <b>Formatter::GetDefault()</b> and instead gave public access to member
  \b lang::format::Formatter::Default. The former method <b>Formatter::ReplaceDefault()</b>
  was not even defined, and this bug became now obsolete. 
- Changed field \b lang::format::FormatterPythonStyle::Sizes to being a pointer which by default
  points to the new member \b lang::format::FormatterPythonStyle::SizesDefaultInstance. 
  This allows attaching an external instance.
- Fixed \b lang::format::FormatterPythonStyle to correctly use tab stops when new-line characters
  are given natively with <c>'\\n'</c>. (Before, this worked only when those were given with
  escape sequence <c>"\\n"</c>.)
- Class \b lang::format::FormatterPythonStyle now allows nested curly braces <b>"{...}"</b>
  within its fields. This enables custom format string definitions (implemented with box-function
  \b lang::format::FFormat) to contain curly braces.
- Several exceptions thrown by formatters now contain the argument type of the placeholder
  in question. With debug-builds, this information is displayed if the exception is formatted. 
- Added type-definition \b lang::system::PathCharType, which aliases C++ standard type 
  <c>std::filesystem::path::value_type</c>. 
  With that several string aliases like \b lang::system::PathString, or 
  \b lang::system::PathStringMA have been added, and class \b lang::system::Path uses this 
  type as its underlying character type. 
    

\par Camp Config:
This \alibcamp was rewritten from scratch. This, of course, implies huge changes of the using
code. The good news is that the use of configuration variables is now much easier and
straight forward, and it is now suited to cover an even wider range of use cases. In fact, the former
implementation was one of the few remainders in \alib resulting from its origin in languages
Java and C# and thus was inefficient and not following modern C++ style at all.
\par 
The Programmer's Manual of this module was likewise rewritten and now
contains step-by-step tutorial sections.

\par  Camp Files:
- Added compiler symbol \ref ALIB_FILES_FORCE_STD_SCANNER and preprocessor
  symbol \ref ALIB_FILES_SCANNER_IMPL.
- Added class \alib{files;File} which inherits the cursor type of class \alib{files;FTree}.
  This allows interfacing with entries in the file tree on different levels and paves the way
  for future features of this type that manipulate files on the system.
- Implemented box-function \b lang::format::FFormat for new class \b %File to support
  the output of file data with formatter \b lang::format::FormatterPythonStyle.
  The implementation makes use of method \alib{files;File::Format}, which therefore defines the
  format strings' syntax likewise for the python formatter.
- Added new class \alib{files,TextFile}.

\par  Camp Expressions:
- Former <c>std::shared_ptr</c> \b SPExpression is now of new type \alib{containers;SharedVal}
  and was renamed to \alib{expressions;Expression}. What was \b Expression before, is now 
  named \alib{expressions;ExpressionVal}.
- Various smaller changes and optimizations with class \alib{expressions;Scope}. 
  Removed former vector \b Resources as only 
  \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;a45b12fe152f8e6a90341987ef35c958e;Scope::NamedResources}
  seemed useful.   

\par  Camp ALox:
- Tons of internal changes related to the rewrite of module \b Config. These are not listed
  here in detail as they mostly affect users which implemented custom loggers. Those are
  referred to the \ref alib_mod_alox "Programmer's Manual", \ref #alib::lox "reference documentation"
  and the source code.
- Removed configuration variable \b ALOX_LOGGERNAME_MAX_ELAPSED_TIME in favor of integrating this
  value in variable \ref alxcvALOX_LOGGERNAME_AUTO_SIZES.
- Method \alib{lox;Log::AddDebugLogger} now sets domain <c>"ALIB"</c> to
  \alib{lox;Verbosity;Verbosity::Warning}. This is in good alignment with the more exentsive use of
  \alox internally since this release, for example, by camp \alib_files.
- Added method \alib{lox;Lox::SetFileNameCacheCapacity} and removed static member
  \b DefaultCacheSize from class \alib{lox::detail;ScopeInfo}. The cache implementation now relies
  on the new class \alib{containers;LRUCacheTable} and is more efficient than before.

\I{################################################################################################}
\I{############################        Version 2402 R1       ######################################}
\I{################################################################################################}

# Version 2402, Revision 1, released March 4th, 2024 #
- Fixed the version number, which still was set to 2312R0, now it is 2402R1
- Several minor fixes in the documentation.

\I{################################################################################################}
\I{############################        Version 2402 R0       ######################################}
\I{################################################################################################}

# Version 2402, Revision 0, released February 29th, 2024 #
Yes, we used the gained hours of this extra leap year day to create a fresh release of \alib!

#### This version was successfully tested under the following platform and toolchain combinations: ####
- GNU/Linux Arch 6.7.6, GNU C++ 13.2.1 / Clang++ 16.0.6, C++ 17/20, 32-Bit / 64-Bit<br>
  (This is the main development platform.)
- Windows 10/11, MSC 17.8.3 (Visual Studio 2022), C++ 17/20, 32-Bit / 64-Bit
- Windows 10/11, MinGW,  C++ 17/20, 64-Bit
- MacOS Sonoma 14.3.1, Apple M2 / ARM64, Apple Clang Version 15.0.0, C++ 17/20, 64-Bit
- Raspberry 3, ARM, 64-bit OS, GCC 10.2.1, C++ 17/20, 64-Bit
- Raspberry 4, ARM, 64-bit OS, GNU C++ 12.2.0, C++ 17/20

#### C++ Language Standard Support  ####
<b>Removed support for C++ 11 and 14</b>. Compilation will test the language standard and explicitly
fail with an error message with these standards.<br>
By the same token, added preliminary support for C++ 23. (The library compiles and unit tests
run with C++ 23 compilation under GNU/Linux clang/gcc.)


#### Preprocessor Symbols: ####
- Renamed symbol \b ALIB_CPPVER to \ref ALIB_CPP_STANDARD.
- Symbol \ref ALIB_CPP_STANDARD is now allowed to be given from outside. This may be used with compilers
  unknown to ALib, where the auto-detection fails.
- Symbol \ref ALIB_CPP_STANDARD will now contain \c 23, if this language standard was used in the build
  process.
- Removed symbols \b ALIB_CPP11, \b ALIB_CPP14, \b ALIB_CPP17 and \b ALIB_CPP20 in preference
  to exclusively using \ref ALIB_CPP_STANDARD.
- Due to dropping C++ 11/14 support, removed macros
  - \b ALIB_WARNINGS_IGNORE_IF_CONSTEXPR,
  - \b ALIB_CPP14_CONSTEXPR,
  - \b ALIB_CPP17_CONSTEXPR,
  - \b ALIB_CPP14_REL_CONSTEXPR,
  - \b ALIB_CPP17_REL_CONSTEXPR and
  - \b ALIB_NODISCARD

- Due to preliminary support for  C++ 23 support, added macros
    - \ref ALIB_CPP_23
    - \ref ALIB_CPP_BEFORE_23

#### Library Organization: ####
Several changes have been made in the organization and structure of the library.
Existing projects need to be adopted. This concerns probably each compilation unit, and we are
sorry for this. However, most of the work needed is change inclusion paths of some files
and changing the namespace name of \alib types.<br>
The following gives a rough overview of what have been done:
- Former namespace <b>aworx</b> has been renamed to \ref alib.<br>
  Its inner namespace <b>aworx::lib</b> has been removed completely, so that all namespaces of the
  \alibmods are now directly located in outer namespace \ref alib.
- The concept of "file-sets" has been dropped. Instead, the few files that did not find a home
  with one certain \alibmod have been moved to source folder <c>src/alib/lang</c>
  and just have to be included with an \alib Distribution.
- New namespace \ref alib::lang holds all library types which are very close to the C++ language
  and which are not related to specific \alibmods.
- The terminology of <em>ALib Micro Module</em>, <em>ALib Full Module</em> has been dropped:<br>
  Everything is an \alibmod_nl now. Those that need configuration data and or resources
  (the ones that had been called "full modules"), are now called \alibcamps.<br>
  Instead of former class <b>aworx::lib::Module</b>, they now inherit
  class \b alib::lang::Camp.  
- Special module <b>ALibDistribution</b> was renamed to \b Basecamp. Then the
  following former modules have all been integrated into this single module:
  - <b>aworx::lib::resources</b>
  - <b>aworx::lib::text</b>
  - <b>aworx::lib::results</b>
  - <b>aworx::lib::system</b>

  This dropped the number of modules from 17 to 14. With one new added (see below), \alib now
  presents \b 15 modules.
  
- All bootstrapping and corresponding shutdown is now executed with overloaded namespace functions
  \ref alib::Bootstrap and \ref alib::Shutdown. This is well documented in the completely overhauled
  Programmer's Manual of the library.


#### Core library types: ####
- Fixed a bug im method \alib{lang,CLZ0(TIntegral)} to return <c>sizeof(TIntegral) * 8</c> in case
  the given \e value is zero, just as documented. It returned \c 0 before.
- Added functions \alib{lang,CTZ(TIntegral)} and \alib{lang,CTZ0(TIntegral)} using intrinsics.
- Added functions \alib{lang,UpperMask<TWidth,TIntegral>(),UpperMask<TWidth,TIntegral>}
  and \alib{lang,UpperMask<TIntegral>(ShiftOpRHS),UpperMask<TIntegral>}.
- Removed function <b>BitCounterWidth<TIntegral>()</b> and replaced it with
  \alib{lang,Log2OfSize,Log2OfSize<TIntegral>()}, which returns the former
  functions result <c>-1</c>.
- Changed functions
  \alib{lang,LowerMask<TIntegral>(ShiftOpRHS),LowerMask<TIntegral>} and
  \alib{lang,LowerBits<TIntegral>(ShiftOpRHS,TIntegral),LowerBits<TIntegral>}
  to assert a width overflow. Previously this was tolerated and corrected. This approach was
  considered inefficient in most cases
- Added preprocessor macro \ref bitsof and namespace function \alib{lang,bitsofval}.
- Added class \b lang::TBitSet to new header \b lang/bitset.hpp, which is a more
  sophisticated replica of <c>std::bitset</c>.
- Added type definition \b enums::EnumBitSet which allows to use new type \b lang::TBitSet
  conveniently with enumeration types.

#### CommonEnums: ####
- Added common enum \alib{lang,Recursive}.

#### Module Enums: ####
- Added macro \b ALIB_ENUMS_UNDERLYING_TYPE.
- Added class \b enums::EnumBitSet, which is described in new chapter
  \ref alib_enums_iter_bitset "3.5 Using Class TBitSet with Iterable Enums"
  of the \b "Programmer's Manual" of module \b Enums.

#### Module Time: ####
- Added method \alib{time,TimePointBase::Reset}.


#### Module Strings: ####
- Added type definition \alib{strings,TString::CharType}.
- Added template parameter \p{TCheck} to method \alib{strings,TString::Equals,String::Equals}
  to allow omitting checks for \e nulled strings if appropriate.
- Added method \alib{strings,TAString::DetectLength}.
- Added utility class  \alib{strings,TStringLengthResetter}.
- Aggregated boolean flags in class \b NumberFormat to new enum type
  \alib{strings,NumberFormatFlags}.

#### Module Monomem: ####
- Removed the first three entries (11, 23 and 47) of the list of prime numbers used to determine the
  (next higher) bucket size of class \alib{monomem,HashTable} to avoid unnecessary rehash on
  initial growth in the common use cases. Consequently, the minimum bucket size now is \e 97 instead
  of \e 11.<br>
  This should be a more realistic value. And: global objects that are using the \alib{monomem,GlobalAllocator}
  can not be bootstrapped to a higher capacity (due to C++ bootstrap mess) without adding explicit
  bootstrap code. Those are now preset with a higher size.
- Refactored class \alib{monomem,StringTree} to not request to initialize a value of
  template type \p{T} associated with it's root node during construction. Even more, it is not
  possible anymore; instead new methods <b>monomem::StringTree::ConstructRootValue</b>.
  and <b>monomem::StringTree::DeleteRootValue</b> have to be used to 'optionally' attach
  custom data to the root node. This approach has several advantages. Further information on this
  topic \ref alib_ns_containers_stringtree_rootnodevalues "is found here".
- Renamed \b StringTree::NodePtr to \alib{monomem,StringTree::Cursor}. (Ok, we did this once already
  the other way round. Well...)
- Added types \alib{monomem,StringTree::ConstCursor} and \alib{monomem,StringTree::ConstRecursiveIterator}
  to allow to the read-only traverse of objects type <c>const StringTree</c>.
- Added methods {monomem,StringTree::NodeTable} to allow access to the internal node storage.
  Of-course, this object has to be used with caution.
- Method \alib{monomem,StringTree::TCursor::AssemblePath} was optimized in respect to performance
  and with that, now returns an absolute path. With other words, the path separation character is
  prepended.<br>
  An alternative method, which again returns a relative path from the root node, was added
  as an overloaded version. In addition, this overload allows to create relative paths not only from
  the root node, but from any other parent node.
- Removed unnecessary intermediate dynamic allocations that occurred when using \b StringTree with
  template node maintainer \alib{monomem,StringTreeNamesDynamic}. On the same token, added
  debug statistic variables \alib{monomem,DbgStatsStringTreeNames} and
  \alib{monomem,DbgStatsStringTreeNameOverflows} to support choosing a reasonable internal
  buffer size, which is application dependent.

#### Module BASECAMP: ####
- As explained above, \b Basecamp is a new module aggregating the contents of other former modules.
- Added parameter \p{nf} to \b lang::format::FFormat::Signature to allow custom formatting functions of
  boxes to access these configuration values of the formatter.
- Added enum class \b lang::format::ByteSizeUnits and format types \b lang::format::ByteSizeIEC and
  \b lang::format::ByteSizeSI.

#### Module Files: ####
This is a new module which provides powerful directory and file scanning capabilities.
Still an initial version but equipped with a tutorial-style \ref alib_mod_files "Programmer's Manual".

#### Module ALox: ####
This release finally integrated all artifacts of documentation and things that had remained on
former joint project [ALox for C++,C# and Java](http://alexworx.github.io/ALox-Logging-Library/)
into this site. The complete \ref alib_mod_alox (70 pages) and the \ref alib_alox_tut (another 27 pages)
have been reviewed and overhauled.

\I{################################################################################################}
\I{############################        Version 2312 R0       ######################################}
\I{################################################################################################}

# Version 2312, Revision 0, released December 13th, 2023 #

\note Yes, it hast been four years since the lasst release. What esd called a "global pandemic"
      distracted me (the main programmer of this library) in many respects from working on open source
      projects. Probably in a very different sense than some of you would expect.<br>
      <b>Please take my excuse!</b>

#### This version was successfully tested under the following platform and toolchain combinations: ####
- GNU/Linux Arch 6.6.6, GNU C++ 13.2.1 / Clang++ 16.0.6, C++ 11 - 20, 32-Bit / 64-Bit<br>
  (This is the main development platform.)
- Windows 10/11, MSC 17.8.3 (Visual Studio 2022), , C++ 11 - 20, 32-Bit / 64-Bit
- Windows 10/11, MinGW 11.0,  C++ 11 - 17, 64-Bit ( <c>C++ 20 does  not compile! with current MinGW!</c>)
- MacOS 13, Intel i9, AppleClang 14.0.3, C++ 11 - 20 C++, 64-Bit
- MacOS 13, Apple M1/ARM64, C++ 11-20, 64-Bit
- Raspberry 3, ARM, 64-bit OS, GCC 10.2.1, C++ 11 - 20, 64-Bit
- Raspberry 4, ARM, 32-bit OS, GNU C++ 12.2.0, C++11-20
- Raspberry 4, ARM, 64-bit OS, GNU C++ 12.2.0, C++11-20


#### CMake Build System  ####
- Fixed the meanwhile lost ability to generate a static \alib library. Obviously a change in the
  CMake default behaviour caused to build and link a shared library with the use of
  provided method \ref alib_manual_build_cmake_5 "ALibAddStaticLibrary".
- Added setting <b>set(Boost_USE_STATIC_LIBS   ON)</b> in \alibfile{build/cmake/ALib.cmake}.
  This avoids rebuilding a project on systems that update \e Boost libraries.
- Renamed CMake variables \e ALIB_COMPILE_FLAGS to \b ALIB_COMPILER_OPTIONS and
  \e ALIB_LINKER_FLAGS to \b ALIB_LINKER_OPTIONS.


#### Preprocessor Symbols: ####
- Added symbol \ref ALIB_GCC, which deducts the use of the \https{GNU Compiler Collection,gcc.gnu.org/}
  by excluding other compilers.
- Symbol \b ALIB_CPP20 is now set to \c 1, if the language version is set accordingly.
  (previously, the symbol existed but was never set).
  Likewise symbol \c ALIB_CPPVER will now contain \c 20, if this language version was
  used in the build process.
- Renamed symbol \c ALIB_CONSTEXPR_IF to \b ALIB_CONSTEXPR17 for usage in other situations
  besides C++ 17 const-expression if-statements.
- Added further macros to temporarily suppress compiler warnings:
  - \ref ALIB_WARNINGS_ALLOW_NULL_POINTER_PASSING
  - \b ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
  - \ref ALIB_WARNINGS_ALLOW_SHIFT_COUNT_OVERFLOW
  - \ref ALIB_WARNINGS_IGNORE_FUNCTION_TEMPLATE
  - \ref ALIB_WARNINGS_IGNORE_INTEGER_OVERFLOW
  - \ref ALIB_WARNINGS_ALLOW_SHIFT_COUNT_OVERFLOW
  - \ref ALIB_WARNINGS_IGNORE_FUNCTION_TEMPLATE
  - \ref ALIB_WARNINGS_IGNORE_RESERVED_IDENTIFIER

- Other changes in macros:
  - Added symbol \ref ALIB_BASE_DIR
  - Added macro \b ATMP_IS_INT
  - Added macro \b ATMP_IS_UINT
  - Added macro \b ATMP_IS_SINT
  - Added symbol \ref ALIB_INTGAP_TYPE. See documentation of \ref ALIB_SIZEOF_INTEGER
    as well as manual chapter \ref alib_manual_build_overview_platforms "4.1.1 Platforms and Toolchains"
    of the \alib Programmer's Manual.

#### General Changes: ####
- Adopted code to compile smoothly with current clang version \https{clang compiler 14.0.6,llvm.org/}
  and \https{GNU Compiler Collection 12.2.0,gcc.gnu.org/}. (As always a minimum set of
  notorious warnings are disabled, only)
- Removed former filesets \e debug and \e integers in favour to reduce compilation/setup complexity.
  Corresponding files are now included in each \alib Distribution with header files:
  - \b lib/integers.hpp and
  - \b lib/tools.hpp
  The latter was available before, but now includes the contents of removed files <c>assert.hpp</c>
  and <c>typedemangler.hpp</c>
- Added library header \b lib/bits.hpp, which adds the following functions to
  namespace \b aworx::lib:
  - \b aworx::lib::BitCount
  - \b aworx::lib::CLZ
  - \b aworx::lib::CLZ0
  - \b aworx::lib::MSB
  - \b aworx::lib::MSB0
  - \b aworx::lib::BitCounterWidth
  - \b aworx::lib::LowerMask

- Minor fixes of fileset dependency evaluation in file "lib/modules.hpp".
- Removed use of deprecated type <c>"std::iterator"</c>.
  (Replaced by use of <c>"std::iterator_traits"</c>.)
- Renamed identifiers containing the misspelled word "iteratable" to "iterable".
  I.e. template type \b enums::T_EnumIsIterable, macro \b ALIB_ENUMS_MAKE_ITERABLE
  and also header file \b iterable.hpp}
- Slightly corrected bootstrapping order of micro modules to be
  1. \alib_time,
  2. \alib_boxing and
  3. \alib_threads.


#### ALib Debug Support: ####
- Added parameter \p topic to overloaded methods \b aworx::lib::DbgSimpleALibMsg.
  Changed all invocations within the library to provide a topic.

#### Fileset "CommonEnums": ####
- Extracted enum alias definitions in namespace \c aworx into new header file
  "lib/fs_commonenums/commonenumdefs_aliased.hpp".
- Added common enum \b aworx::lib::ValueReference.

#### Fileset "Lists": ####
- Moved classes of this fileset to new sub-namespace \b aworx::lib::detail.
- Renamed and simplified helper classes of this fileset.

#### Fileset "Plugins": ####
- Moved class \b detail::PluginContainer to new sub-namespace \b aworx::lib::detail.

#### Fileset "Modules": ####
- Added field \b ALibDistribution::LocaleFound that allows to get confirmation that the locale
  that was automatically set as described in \b ALibDistribution::bootstrap.
  This is now used by the unit tests to avoid errors due to failure of unicode
  conversion just for the sake of missing environment variables in dubious development
  environments.

#### Module Resources: ####
- Added the field \b lang::resources::LocalResourcePool::DbgResourceLoadObserver and updated the
  corresponding manual chapter.

#### Module Enums: ####
- Added three more namespace functions for bitwise defined enumeration types:
  \b enums::bitwise::CountElements,
  \b enums::bitwise::ToBitwiseEnumeration and
  \b enums::bitwise::ToSequentialEnumeration.

#### Module Boxing: ####
- Defined \ref alib_boxing_more_opt_staticvt "static vtables for ALib Boxing" for the following types:
  - Common enumeration types provided with \alib.
  - \c std::type_info
  - \b results::Exception
  - \b results::Report::Types}
  - \alib{strings::util,Token}
  - \alib string types when wrapped in \c std::reference_wrapper.
  - Added convenience method \alib{boxing,Box::UnboxMutable}.

#### Module Strings: ####
- Fixed comparison operators to be compatible with ambiguities that occur with C++ 20 language
  standard. Likewise, with C++ 20 the spaceship operator <c>'<=>'</c> is now used instead.
- Added method \alib{strings,TAString::Reverse,AString::Reverse} and underlying implementation
  \alib{characters,CharArray<TChar>::Reverse}.
- Added checks for field \b ostream with output methods of class
  \alib{strings,compatibility::std::StringWriter}.
- Fixed a bug in floating point number conversion, which in rare cases caused an
  \alib exception in debug builds and a falsely rounded output string in release builds.
- Fixed move constructor of class \alib{strings,TAString,AString} not also reset the capacity,
  but also the length of the moved string to \c 0.
- Method \alib{strings,TString::WStringLength,String::WStringLength} now, in case of failure, returns the strings'
  unconverted length instead of -1.

#### Module Time: ####
- Added method \alib{time,TimePointBase::Unset}.
- Added methods \alib{time::TimePointBase,Duration::SetMinimum}
  and           \alib{time::TimePointBase,Duration::SetMaximum}.
- Renamed method <c>TimePointBase::Raw</c> to \alib{time,TimePointBase::ToRaw}.
- Added method \alib{time,TimePointBase::SetFromRaw}.
- Renamed method <c>TimePointBase::NativeValue</c> to \alib{time,TimePointBase::Export}.
  Changed its signature to being \c const and changed return value to a copy of the internal value
  instead of a reference to it.
- Added method \alib{time,TimePointBase::Import}.


#### Module Threads: ####
- Changed signature of most methods of class \alib{threads,Thread} to being \c virtual.
- Added method \alib{threads,Thread::Terminate}, which has to be called prior to destructing
  a thread.
- Added enum \alib{threads,Thread::State} and method \alib{threads,Thread::GetState},
  which replace removed boolean field <c>Thread::isAliveFlag</c> and method <c>Thread::IsAlive</c>.
- Added static method \alib{threads,Thread::YieldToSystem}
- Added static methods \alib{threads,Thread::Sleep} and \alib{threads,Thread::SleepUntil}
- Added class \alib{threads,Sleeper}.
- Improved debug assertions of class \alib{threads,ThreadLockNR}


#### Module Monomem: ####
- Fixed method \alib{monomem,List::ElementAt} (and added missing unit test).
- Added constructor parameter \p chunkGrowthInPercent to
  \alib{monomem,MonoAllocator::MonoAllocator(size_t, unsigned int),MonoAllocator}
  which allows exponential growth of allocated chunk sizes.
- Fixed a bug in method \alib{monomem,HashTable::Erase(ConstIterator, ConstIterator) }
  which lead to a double invocation of a destructor of the first element removed.
  Improved unit tests to detect problems of this kind in the future.
- Added container type <b>monomem::util::FixedCapacityVector</b> and type definition
  \b FixedSizePriorityQueue.
- Removed method \b HashTable::Rehash. See
  \ref alib_ns_containers_hashtable_rehashing "reference documentation" for the rationale.
- Added parameter \p{reference} to methods
  \alib{monomem,HashTable::Reserve},
  \alib{monomem,HashTable::ReserveRecyclables} and
  \alib{monomem,StringTree::ReserveRecyclables}.
- Constructor of \alib{monomem,StringTree} now reserves a minimum number of internal buckets
  to prevent the creation of a non-recyclable bucket list on future growth.
- Added missing template parameter \c TLocalCapacity to alias name \b aworx::StringTreeNamesDynamic.
- Fixed bug in overloaded variant of \alib{monomem,HashTable::EmplaceOrAssign} which
  could generate a compiler error due to not properly forwarding the hash functor.

#### Module "Results": ####
- Adopted method \b results::ReportWriterStdIO::Report to interpret the first parameter of
  a method as a "topic". The rules to detect such topic follow the rules of auto-detecting
  domain names in higher level module \alox, which replaces the report writer for internal
  messages.

#### Module Text: ####
- The backslash of newline characters in format strings do not need to be escaped itself anymore.
  For example, format string

        "Value 1: {}\\nValue 2: {}"

  may now be written

        "Value 1: {}\nValue 2: {}"

  All occurrences in resource strings and log output have been changed to use the single-slash
  version.

- Replaced abstract virtual method <b>FormatterStdImpl::replaceEscapeSequences</b> with
  \b lang::format::FormatterStdImpl::writeStringPortion (performance optimization).
- \b lang::format::FormatterStdImpl now performs custom formatting whenever a custom boxing
  function \b lang::format,FFormat is defined for a given boxed formatting argument.
  In previous versions, this was done only if a custom format string was provided with the
  corresponding placeholder of the format string.<br>
  Consequently, custom implementations of \b FFormat now have to cope with empty format strings,
  for example by using a (resourced) default string.

#### Module Config: ####
- Changed the format of resourced variable declarations.
- Fixed a bug that caused utility method \b config::Configuration::PreloadVariables to overwrite
  existing variables with default values.
- Method \b config::Configuration::Load now adds the default value to the given variable
  instance, also when no default configuration plug-in is added.

#### Module System: ####
- \b FFormat_DateTime now uses a default format string in the case no format was given.
  The format string is resourced with key \b "DFMT".
- Extracted system error definitions from \b system/system.hpp to
  \b system/systemerrors.hpp (to reduce inclusion dependencies)
- Added class \b lang::system::CalendarDate.

#### Module ALox: ####
- Fixed special shortcut header \b alox.hpp to include \b distribution.hpp
  to allow single inclusion of "alib/alox.hpp" to automatically bootstrap and use
  \alox functionality as instructed in the tutorial.
- Added method \alib{lox,Lox::IsActive} to detect the number of loggers that are active
  for a combination of domain and verbosity.<br>
  Likewise added macros #Lox_IsActive and #Lox_IsActive.
- Class \alib{lox::loggers,TextFileLogger} now tests file for possible access during construction.
  Errors may be evaluated with new field \alib{lox::loggers,TextFileLogger::LastSystemError}, which
  replaced former boolean field \b hasIoError that only was set after a (first) log operation.

#### Module Expressions: ####
- Moved mechanics to implement \ref alib_expressions_nested_config "automated named expressions"
  into new dedicated abstract interface class \alib{expressions,ExpressionRepository}.<br>
  A standard implementation for it is provided with \alib{expressions,StandardRepository}.
  Implementations are to be set to new field \alib{expressions,Compiler::Repository}.
  Consequently, all corresponding former entities in class \b Compiler
  (namely \b Config, \b DefaultCategories, \b VariablesLoaded, \b StoreLoadedExpressions and
  \b getExpressionString) have been removed.
- \ref alib_expressions_operators_verbal "Verbal aliases" of binary operators are no longer
  conflicting with identifier/function names. For example, a function named "equals" may
  now be defined while the same string is used for a verbal operator alias.
- Simplified syntax of format strings of type <b>expressions::util::ExpressionFormatter</b>.

#### New Module CLI: ####
This module is not completely new, but with this version it comes in its first "release" state.
Along with it, a brief Programmer's Manual \alib_cli is provided.
This manual includes a step-by-steb sample, which can well be used as a jump-start template for your
own project.

#### New module "BitBuffer": ####
This \alib release introduces a new module \alib_bitbuffer_nl which provides mechanism to serialize
application data into streams of bits. The following classes are included:
- \alib{bitbuffer,BitBuffer}, \alib{bitbuffer,BitBufferMA} and \alib{bitbuffer,BitBufferLocal}
- \alib{bitbuffer,BitWriter} and \alib{bitbuffer,BitReader},
- \alib{bitbuffer::ac_v1,ArrayCompressor}, \alib{bitbuffer::ac_v1,HuffmanEncoder}
  and \alib{bitbuffer::ac_v1,HuffmanDecoder}

A \ref alib_mod_bitbuffer "brief manual" explains the use of this new module.

\I{################################################################################################}
\I{#############################        Version 1912 R0       #####################################}
\I{################################################################################################}

# Version 1912, Revision 0, released December 16th, 2019 #

### Main Achievements Of This Release: ###

1. After the massive restructuring of the library with previous version 1903, some more refactoring
   had to be done to adopt the module concept everywhere and fine tune it.<br>
   Therefore, with this release, various incompatibilities are again expected. We (still) have hopes
   that \alib becomes more stable over time.
2. The concept of "ALib Enum Meta Data"
   - has been renamed to <b>ALib Enum Records</b> and was rewritten from scratch.
     It is now much more simple in use and yet more powerful.
   - was formerly spread between modules \b Enums and \alib \b Resources,
     and is now 100% located in \b Enums.
   - Its documentation had been spread over reference documentation of various types and is now
     concentrated in chapter \ref alib_enums_records "4. Enum Records" of the now available
     \ref alib_mod_enums "Programmer's Manual" of that module.
3. Module \alib \b Resources has been revised. Resources are now more strictly defined to be \e static data.
   While the core principles remained intact, almost every entity in the module has been
   refactored to be simpler and faster.
4. "Bootstrapping of ALib" and custom modules was slightly revised.
5. The most important new feature: Module <em>Memory</em> (which was almost empty before) has been
   renamed to \alib_monomem and filled. Its new functionality has been documented with a new
   Programmer's Manual and it is used with almost every other module.<br>
   This reduced the number of memory allocations made during the unit test by around 80%
   and the time to execute all test by around 25% in debug builds and more than 15% in release!<br>
   Depending on a user's application's nature (in respect to memory consumption) the use of this
   module has <b>a huge potential for a performance boost</b> for many applications.
6. The former separation between <em>"compiler symbols"</em>, aka definitions passed to the
   compiler and <em>"code selection symbols"</em> has been dropped. While it was motivated
   by "good intentions", we realized that it complicates things too much and the industry
   is used to not have this separation.<br>
   When using \https{CMake,cmake.org/} to build the library, this should not have much impact
   on existing user's code base.
7. Finally, we tried to reduce header file dependencies, which results in faster compilation but
   unfortunately also further incompatibilities when switching to this version.

As a summary, existing user code will need various changes to adopt - and we again excuse for that.
We hope that during the process of adopting your code, you will see the benefits of all changes.

#### General Changes: ####
- Dropped the concept of "compiler symbols". If not explicitly documented otherwise,
  code selection symbols (that formerly where derived from compiler symbols passed with option <b>-D</b> )
  are now directly possible to the compiler with the same option name instead of using postfixes
  <b>_ON</b> or <b>_OFF</b>.
- Cleaned header file inclusion across \alib to improve compilation speed.<br>
  Moving to this version will likely impose the need to include some specific headers in a
  using software, which previously had been included internally.
- Removed all redundant semicolons from the source
  (I.e. to allow compiler warning "-Wextra-semi" set with \b GCC or \b Clang).
- Removed all redundant keywords <c>inline</c> from the source.
- Corrected many typos in manuals.
- Updated documentation to be compatible to new doxygen release 1.8.16.
- Added header inclusion diagrams to the documentation. Also added documentation pages for
  each \alib source folder and header file and a link from almost each C++ entity to the
  corresponding header file of its declaration.
- All variadic template types that had been named \p{Args} have been renamed to \p{TArgs} for
  consistency with \alib naming conventions.
- Renamed all code selection symbol for modules from \b ALIB_MODULE_XYZ to \b ALIB_XYZ, for example
  \b ALIB_MODULE_THREADS to \b ALIB_THREADS.<br>
  On the same token added macros \b ALIB_IF_XYZ and \b ALIB_IFN_XYZ for each module.
  (For example \b ALIB_IF_THREADS and \b ALIB_IFN_THREADS.)
  Those are useful to reduce <c>\#if</c>/<c>\#endif</c> clutter in the code.
- Renamed the symbol \b ALIB_DOCUMENTATION_PARSER to \b ALIB_DOX and changed it from being
  defined to \c 0 to being undefined with compiler runs.

#### CMake Build System / Filesets / Sources: ####
- Removed former filesets <b>PREDEF_PF</b> and <b>PREDEF_TMP</b>, and defined the associated
  mostly preprocessor-related header files to be included in any \alib Distribution.
- Moved sources of filesets to sub-directories named <b>src/lib/fs_XYZ</b>.
- Added dependency to filesets "INTEGERS" and "DEBUG" for micro module \alib_singletons.
- Removed fileset "TYPEMAP".
- Added new fileset "LISTS" containing generic node types and iterators for internal use with
  single and doubly-linked lists.
- Prior to the invocation of \b ALib.cmake, entry <em>"ALIB_SUPPRESS_COMPILER_WARNINGS"</em>
  can now be added to list variable \b ALIB_COMPILER_WARNINGS. If so, this entry is removed and
  the script will not add any warning settings to the symbol.
- Fixed bug in CMake script that prevented cached variable \b ALIB_PRECOMPILED_HEADER_DISABLED
  to disable precompiled header \b alib_precompile.hpp.
  Renamed the variable to be \b ALIB_PRECOMPILED_HEADER, defaulting it to \b Off.
- The CMake script now searches the target system's thread library independent from the inclusion of
  module \alib_threads. Instead, new CMake variable
  \ref alib_manual_build_cmake_3 "ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH" controls the search and
  may be used to disable this.
- Renamed CMake variable \b ALIB_MODULE_SOURCE_COPY_DIR to
  \ref alib_manual_build_more_copysources "ALIB_SOURCE_COPY_TARGET_DIR".
- Renamed CMake variable list \b ALIB_MODULES to \ref alib_manual_build_cmake_1 "ALIB_DISTRIBUTION".

#### Preprocessor Symbols: ####
- C++ version macros:
  - Added macro \b ALIB_CPPVER which contains one of C++ version numbers \c 11 to \c 20.
  - Added macro \b ALIB_CPP11 which is set if exactly C++ 11 is used.
  - Added macro \b ALIB_CPP20 which is set if exactly C++ 20 is used (never set, yet).
  - Changed meaning of macro \b ALIB_CPP14, which now is unset if a C++ version higher than
    \c 14 is used.
  - Removed macro \b ALIB_REL_CONSTEXPR. It was replaced in a former release already by
    macros \b ALIB_CPP14_REL_CONSTEXPR and \b ALIB_CPP17_REL_CONSTEXPR


#### Fileset "Modules": ####
Revised bootstrapping which is implemented mainly with class \b Module.
Various changes have been made, for example:
- The point of creation of a resource pool and configuration object, and their distribution during
  the course of bootstrapping was slightly revised.
- A new contract rule now disallows multi-threaded access during bootstrap, which relieves
  bootstrap code from using locks (mutexes).
- Module termination is now a phased process too: The first phase, \b Module::ShutdownPhases::Announce,
  was added to give each module the chance to perform tasks while the library is still 100% intact
  and initialized.
- Methods \b Init and \b TerminationCleanUp have been renamed to \b Module::Bootstrap and
  \b Module::Shutdown. Likewise, all interfaces throughout \alib that are deemed to be used
  exclusively during bootstrap, have also been renamed and prefixed by the term <b>Bootstrap</b>.
  (For example, \b lang::resources::ResourcePool::BootstrapBulk.)

#### Fileset "Common Enums": ####
- Added common enumeration \b Side.
- Added common enumeration \b Caching.
- Most types now use underlying type <c>bool</c>.

#### Fileset "Plugins": ####
- Class \b PluginContainer does not inherit from \alib{threads,ThreadLock} any more.

#### Fileset "Lists" (new): ####
This new fileset starts with providing types
\b ForwardNode,
\b ForwardList,
\b ForwardListIterator,
\b BidiNode,
\b BidiList and
\b BidiListIterator, which are considered "internal types" when implementing single- or
doubly-linked lists.

#### Module Characters: ####
- Added methods \alib{characters,CharArray::ToUpper(TChar*, integer)} and
  \alib{characters,CharArray::ToLower(TChar*, integer)}.

#### Module Boxing: ####
- It is now allowed to invoke \alib{boxing,Box::TypeID} on uninitialized boxes.
  For those, <c>typeid(void)</c> is returned.
- The constraint to box a type by default "as value" was extended to types being trivially
  destructible.
- Class \alib{boxing,Boxes}:
  - Now optionally accepts a \alib{monomem,MonoAllocator} with construction
  - The constructor taking variadic arguments was removed.
    This intentionally disables implicit construction of an instance of the class with the invocation
    of methods that expect a parameter of type <b>const Boxes&</b>.
  - Methods \alib{boxing,Boxes::Add} now return a reference to the invoked object.
- Renamed namespace function \b EnumValue to \b enums::UnderlyingIntegral.
- Renamed method \b Enum::Value to \alib{boxing,Enum::Integral}.
- Fixed a minor memory leak with custom box-functions that were not deleted on program termination.
  (This was only an issue for mem checking tools like \http{Valgrind,valgrind.org}.)
- Added compilation symbol \ref ALIB_DEBUG_BOXING. Without this being given,
  \ref alib_boxing_more_debug_general "debug features of the module" are no longer available in
  debug-builds.
- Renamed macro \b ALIB_BOXING_VTABLE_REGISTER to \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER.
- Removed debug tables \b DBG_KNOWN_VTABLES and \b DBG_KNOWN_VTABLES.
  Instead, method \alib{boxing,DbgBoxing::GetKnownVTables} was added and along with that, method
  \alib{boxing,DbgBoxing::GetKnownFunctionTypes} is now available also in \alib Distributions that only
  contain module \b Boxing.
- Changed management of custom box functions to using one global hash table instead of
  many linked lists. This might slightly decrease performance with small applications
  (not noticeable) but scales in an average of O(1) when many different custom function types are
  declared.


#### Module Strings: ####
- \b Attention: The default constructor of class \alib{strings,TString,String} now is default-implemented
  and leaves an instance in uninitialized state. (Formerly the instance was in <em>nulled</em> state
  with default construction.)<br>
  The rational for this is to avoid redundant initializations and with that, type \b String is now
  a <em>POD type</em>. Consequently it is now a user's obligation to assure strings used
  are initialized.<br>
  (Note: within all of \alib, only a handful of explicit initializations had
  to be added. By using analyzer tool \http{Valgrind,valgrind.org/}, those could be detected with
  little effort.)
- Fixed a bug in  method \alib{strings,TString::indexOfString}, which could cause illegal memory
  access and other undefined behavior in seldom cases.
- Removed class <b>strings::util::StringTable</b>. Its only use was in module boxing to implement
  debug output and consequently was moved to \alib{boxing::detail,DbgStringTable}.
- Removed the simple templated inline namespace functions <b>strings::util::FindStringInTupleVector</b> and
  <b>strings::util::FindStringStartInTupleVector</b>.
  They got replaced by anonymous (local) definitions.
- Removed constructor and methods \b Format and \b FormatArgs from class \alib{strings,TAString,AString}.
  Those methods and constructors had formerly allowed to use an \b AString as an interface to string
  format operations which used the default formatter. The rational to remove this functionality
  is to reduce module dependencies, construction parameter ambiguities and to have a more explicit
  API.
- Moved class \alib{monomem,StringTree} from module \alib_strings_nl to module \alib_monomem_nl
  and completely overhauled its implementation, aiming to optimize memory consumption, performance,
  flexibility and ease of use.
  This overhaul implies a complete change in its interface, public inner types, etc, which are not
  listed here one by one.<br>
- Added inline namespace method \alib{strings,AllocateCopy} and \alib{strings,DeleteString}.
- Moved methods \b ConsumeEnum, \b ConsumeEnumBitwise and \b ConsumeEnumOrBool from
  class \alib{strings,TSubstring,Substring} to corresponding functions in namespace
  \b aworx::lib::enums.


#### Module Threads: ####
- This module can now be excluded from any permutation of modules of an \alib Distribution, which turns
  such distribution into a <em>"single threaded"</em> version of \alib.
  This is considered a new feature (as formerly, module \alib_alox_nl had a strict dependency).
  More on that topic is found in the corresponding new manual section.
- The module is now independent of module \alib_time (the prior minimal use pieces, exclusively in
  debug-builds, were re-implemented)
- Uses \ref alib_mods_contmono "monotonic allocation" for thread management.
- Already in previous versions, macros \ref ALIB_LOCK and \ref ALIB_LOCK_WITH remained defined
  if module \alib_threads was \b not included in an \alib Distribution. Now in addition, these macros will
  invoke test function \b DbgAssertSingleThreaded in debug-compilations, that asserts
  if a second thread visits library code that needs protection.
- Re-implemented bigger parts of \alib{threads,ThreadLock} with slight changes in its interface.

#### Module Monomem (formerly Memory): ####
- Renamed module \b Memory to \b Monomem, which is now found in namespace \b aworx::lib::monomem
  and received an own \ref alib_mods_contmono "Programmer's Manual".
- Class \b MemoryBlocks was renamed to \alib{monomem,MonoAllocator} and received tons of changes
  and extensions. Among them:
    - Renamed method MonoAllocator::Construct to \alib{monomem,MonoAllocator::Emplace}.
    - Changed (simplified) the internal allocation code of object \alib{monomem,MonoAllocator::Chunk}.
    - Added overloaded methods \alib{monomem,MonoAllocator::TakeSnapshot} and
      \alib{monomem,MonoAllocator::Reset(const Snapshot&)} which allow to reset the allocator
      to a certain state and dispose intermediate objects.
- Added struct \alib{monomem,SelfContained}.
- Added class \alib{monomem,List}.
- Moved class \alib{monomem,StringTree} from module \alib_strings_nl, performing a 100% overhaul.
- Added class \alib{monomem,HashTable} and with it type definitions
  - \alib{monomem,HashSet} and
  - \alib{monomem,HashMap}.
- Added class \alib{monomem,TMAString}.
- Added classes \alib{monomem,StdContMA}, \alib{monomem,StdContMAOptional} and
  \alib{monomem,StdContMARecycling} to support monotonic allocation with C++ standard containers.
- Added allocator instance \alib{monomem,GlobalAllocator}, which is used during bootstrap by
  various modules.
- Added compiler symbol \b ALIB_DEBUG_MONOMEM to enable debug-features for this module.
- Added a verbose \ref alib_mods_contmono "Programmer's Manual" for this module.

#### Module Resources: ####
- Overhauled resource management:
  - Programmer's Manual was rewritten.
  - Renamed type \b Resources to \b lang::resources::ResourcePool and made it pure abstract interface type.
  - Renamed methods of \b lang::resources::ResourcePool to more precise and better readable names.
  - Removed ability to store non-static resource strings.
  - Added new type \b lang::resources::LocalResourcePool as the default interface implementation
  - Added new type \b lang::resources::ConfigResourcePool an optional interface implementation.
  - Adopted changes in type \b Module.
  - Adopted changes in library's Programmer's manual chapter.
- Removed all previously included functionality of the former concept <em>"ALib Enum Meta Data"</em>,
  which was completely rewritten to new concept \ref alib_enums_records "ALib Enum Records"
  and located in module \b Enums.
- Uses \ref alib_mods_contmono "monotonic allocation" for resource management.
- Removed class \b ResourceString.
- Fixed a bug that prevented to add resources for common enum types, if module \b Config
  was not included in the \alib Distribution.


#### Module Results: ####
- Adopted concept \ref alib_enums_records "ALib Enum Records".
- Changed class \b results::Message to derive from class \alib{boxing,Boxes}, instead of
  owning a field \b Args of that type.
- With the change of class \b Boxes as documented above, a message may optionally now use monotonic
  allocation.
- Changed class \b results::Exception to derive from new struct \alib{monomem,SelfContained}
  (and removed corresponding proprietary code that previously stored the type's fields inside the
  allocator). Together with the above, this fulfills a former promise to have a typical
  \alib exception perform only one single memory allocation.

#### Module Text: (Formerly Stringformat) ####
- Renamed module \b Stringformat to \alib \b Text
- Method \b lang::format::Formatter::Acquire now returns an internal container of type \b Boxes,
  which optionally can be used to collect formatting arguments.
  If it is used, new method\b lang::format::Formatter::FormatArgs(AString&) allows using this list.
- Overloaded methods method\b lang::format::Formatter::FormatArgs to not acquire the formatter
  anymore. Acquirement and a corresponding release of the formatter object, has to be performed
  explicitly by the calling code. In case of formatting exceptions, those have to be caught
  to release the formatter and eventually be rethrown or otherwise processed.
- Fixed method \b lang::format::FormatterStdImpl::writeStdArgument to write raw integral box
  contents with binary, octal and hexadecimal output formats.
  (This was broken since the most recent release only)<br>
  On the same token, the method was changed to use the full (platform-dependent) output width for
  pointer and array types, in the case no output width specifier had been given. E.g. if a pointer
  is given with hexadecimal output, 16 digits are printed on a 64-bit platform.
- Extended conversion option <b>{!Quote}</b> of formatter \b lang::format::FormatterPythonStyle
  to optionally specify the quote character(s) that an argument should be surrounded by.
- Removed creation of class \alib{boxing,Boxes} with method
  \b lang::format::Formatter::Format(AString, const TArgs&...).
  (This was accidentally inserted and totally superfluous.)

#### Module Configuration: ####
- Adopted concept \ref alib_enums_records "ALib Enum Records".
- Class \b config::Variable now inherits new struct \alib{monomem,SelfContained} and thus
  is very memory efficient. This caused various interface changes in the area of getting and
  setting variable attributes and values.
- Constructors of class \b config::Variable as well as method \b lang::config::Variable::Declare
  now accepts a single \alib{boxing,Box} instead of variadic template arguments to define
  replacement information. In the case that more than one replacement object should be passed,
  those have to be encapsulated in an instance of class \alib{boxing,Boxes}.
- Class \b config::InMemoryPlugin was rewritten and changes adopted by derived type
  \b config::IniFile.
  It now uses \ref alib_mods_contmono "monotonic allocation" and shows better performance.
  Various changes in the rather internal interface are not listed here.
- Added method \b lang::config::IniFile::AddResourcedSectionComments, which replaces a former
  less flexible approach to add resourced (externalized) comments to sections, when they
  get created programatically.
- Added field \b lang::config::IniFile::LineWidth, which may be used to adjust the preferred line
  width when formatting comments of sections and entries.

#### Module ALox: ####
- This module is now independent from the availability of module \alib_threads. If not
  available, the library is deemed to be a "single-threaded version", which was formerly
  not possible when module \alib_alox_nl was included in an \alib Distribution.
- Reorganized code structure to reduce header dependencies. Header file \b alox.hpp
  is now a shortcut to \b alox.hpp, which now only includes
  things needed to perform basic debug- and release-log operations.<br>
  Class \alib{lox,ALox} is not included by default anymore. If needed (e.g. for
  initialization of the module or for registration of instances of \b %Lox), header file
  "alox/aloxmodule.hpp" has to be explicitly included.<br>
  To allow the reduction, class \alib{lox,Lox} was implemented using the
  \https{Pimpl Idiom,en.cppreference.com/w/cpp/language/pimpl}.
- Header file \b alib/alox.hpp is now included in all \alib Distributions, even those that do \b not
  include module \alib_alox_nl. If \b ALox is not available, this header will define the logging
  macros empty. This simplifies the insertion of logging code into sources that are only optionally
  used in combination with \b ALox.
- Text formatters now duly catch exceptions raised by their internal \b lang::format::Formatter
  instances, e.g. when  malformed placeholder syntax occurs in logging code.
  In this case the originally intended log message is extended with information on the exception
  that was raised.
- Uses \ref alib_mods_contmono "monotonic allocation" in various places and this way a further
  reduced use dynamic memory allocations was achieved.
- Moved field \b ALox::InternalDomains to \alib{lox,Lox::InternalDomains}.

#### Module Expressions: ####
- Dropped the support of using
  \https{boost::spirit,www.boost.org/doc/libs/1_66_0/libs/spirit/doc/html/index.html} for parsing.
  To maintain the option to alternatively use this library caused reasonable problems and is
  considered not important due to the huge constraints this library is subject to.
- Various internal changes to replace dynamic memory allocations with
  \ref alib_mods_contmono "monotonic allocation", by adding
  \alib{monomem,MonoAllocator} to type \alib{expressions,Compiler} and by using
  new container types of module \alib_monomem wherever possible.
- Changed method name \b Scope::Clear to \alib{expressions,Scope::Reset}.
- Renamed field \b Scope::nestedExpressionStack to \alib{expressions,Scope::NestedExpressions}
- Moved the compile-time scope object from class \alib{expressions,detail::Program} to
  \alib{expressions,Expression::ctScope}.
- Simplified class \alib{expressions::plugins,Calculus}, which now treats unary and binary
  operators merely the same. As a consequence, almost all fields and methods of the class that
  formerly had the words "unary" or "binary" in their name are replaced by generically named
  version that now handle both operator types.
- Fixed documentation chapter \ref alib_expressions_details_multithreading. The restrictions on
  parallel method invocation (multi-threading) were defined too weak.
  Now, with the optimizations in respect to memory consumption, the restrictions became even
  stronger. Nevertheless, this should not impose a major difficulty in implementing multi-threaded
  expression usage.
- On the same token, removed all internal thread-locking mechanics. Consequently, those are now
  an obligation of the using code to be performed. This is a much easier and clearer design.
- Removed class \b ScopeString and replaced with new class \alib{monomem,TMAString}.
- Renamed enum flag \b AllowBitwiseBooleanOperations to
  \alib{expressions::Compilation,AllowBitwiseBooleanOperators}.
- Resourced (externalized) names of built-in types \alib{expressions,Types}.

#### Compatibility Headers: ####
- Moved overloads of <c>operator<<</c> given with \b std_strings_iostream.hpp
  from namespace \b aworx to the global namespace. Documentation still resides in "fake namespace"
  \b aworx::lib::strings::compatibility::std.
- Various minor changes not listed here.


\I{################################################################################################}
\I{#############################         Version 1903         #####################################}
\I{################################################################################################}

# Version 1903, Revision 0, released March 14th, 2019 #

\attention
  Once more, this library version is not tested (probably does not compile!) under Mac OS.
  While we promise to fix that rather quickly now, we would also be thankful if volunteers from
  the community would provide such support.

This is a complete overhaul of the library. With the separation of this \b C++ implementation
from the sibling libraries implemented in programming languages \b Java and \b C#
(see chang log entry <b>Version 1805</b> below),
this C++ version was "set free" from many compromises that were formerly made to keep the language
versions compatible.

As a result, <b>ALib for C++ is now 100% modern C++!</b>
(To the extent that the authors of this library "think modern".)

With that new freedom, the changes that have been performed in the recent 10 month are too
many to be listed and documented in each detail. (Only for module \alib_expressions_nl,
a detail list is given below.)
Consequently, the adoption of user code might become a little more effort than with previous \alib updates,
as many entities have been moved to a different place.

The following provides a high level list of changes.


- <b>Refactoring of the module structure:</b>
  The former seven modules have been refactored to now 16 modules.
  With that, almost any type of the library found a new "home", respectively namespace.
  The modules now strictly obey to a well defined dependency relationship.

- New \b CMake build scripts support the selection and compilation of only a subset of the 16 modules
  into an so called \alib Distribution.<br>
  For users of other build systems, the \b CMake scripts allow to generate source listings
  to determine the exact subset of library files needed for a built.

- With the restructuring the code, also the documentation was restructured. Besides the complete
  \ref alib "reference documentation", \alib now has
  a separated \ref alib_manual "general user manual" along with <b>14 module-specific
  programmer manuals</b>.
  (Two small modules do not have a dedicated programming manual, hence only reference documentation
  is available for those.)

- New module \alib_characters was extracted from both, module \alib_strings and
  \alib_boxing. While it is rather small, it provides a good abstraction layer to mitigate
  general challenges when using characters, character arrays, string literals and 3rd-party string
  types with C++.<br>
  A dedicated \ref alib_mod_characters "Programmer's Manual" is available for this module.

- Module \alib_boxing was completely rewritten. While the API for boxing and unboxing values
  remained almost unchanged, in the area of
  \ref alib_boxing_customizing "customization of boxing for user types" as well as of the definition of
  \ref alib_boxing_functions "box-functions", existing user code needs to be revised.
  Along with the code, the dedicated \ref alib_mod_boxing "Programmer's Manual" of module
  \alib_boxing_nl was completely rewritten.

- Module \alib_strings was refactored in respect to now building on \alib_characters_nl.
  Apart from the former terminated string type \b %TString, which is now named \c %CString,
  code adoptions should be required mostly in the fields of adopting 3rd-party types to
  be transparently accepted as strings and to make 3rd-party types compatible (appendable) to
  class \alib{strings,TAString,AString}.<br>
  Default character width of strings can now be adjusted to \b 1, \b 2 or \b 4 bytes.
  <b>Tons of further small changes</b> have been implemented, including a change of all internal TMP
  programming.<br>
  This module's \ref alib_mod_strings "Programmer's Manual" was also completely rewritten.

#### Module Expressions: ####
Apart from the many changes due to refactoring the library, explicit change information
this time is only given for module \alib_expressions:

- Added auto-cast option for casting arguments of unary operators (which was available only to
  binary operator and ternary operator <c>Q ? T : F</c> only).
- Added auto-cast feature to helper class \alib{expressions,plugins::Calculus} for easy
  implementation of custom type auto-casting.<br>
  Added manual chapter
  \ref alib_expressions_tut_ffext_autocasts_using_calculus "6.5.2 Implementing Auto-Casts Using Class Calculus"
  to sample the use of this feature.
- Changed name of ternary operator <c>Q ? T : F</c> when given with
  \b CompilerPlugin::CIAutoCast::Op from \b "?:" to \b "Q?T:F" to make it distinguishable from the
  binary "Elvis-operator".
- Renamed fields \b DbgCallBackName and \b DbgCallBackNameRhs in \alib{expressions,CompilerPlugin},
  respectively in derived types, to use a lower case letter \b 'b'.
- Changed field \alib{expressions::plugins,Calculus::FunctionEntry::ResultType} to being a pointer
  to a sample box, instead of a sample box (shorter footprint and slightly faster).
- Added static struct \alib{expressions,Signatures} which contains expression function signatures
  for all built-in functions. On the one hand, this reduced the footprint of the library (slightly)
  and on the other hand these static objects can be also used by custom plug-ins (in case a
  matching signature is found in the list).
- Renamed fields \b CastExpressionFunctionName and \b CastExpressionFunctionNameRhs to
  \alib{expressions::plugins::Calculus,CIAutoCast::ReverseCastFunctionName}, respectively
  \alib{expressions::plugins::Calculus,CIAutoCast::ReverseCastFunctionNameRhs}.
- Fixed internal assertions that had been raised with malformed expression input. In these instances,
  now exceptions of type \alib{expressions,Exceptions::SyntaxErrorExpectation} are thrown.
- Fixed the meta data of \alib{expressions,Exception::NestedExpressionResultTypeError} to
  display the expected and given types correctly (had been reversed).
- Removed class \b FunctionNameDescriptor and replaced it with new class \alib{strings::util,Token}.
  This has two compatibility consequences:
  - Custom expression plug-in resources have to be adopted to the new parsing format of identifier
    and function names.
  - Many built-in function abbreviations have changed to support abbreviation of single
    camel-case humps.
- Added class \b expressions::util::ExpressionFormatter which is similar to existing
  \b lang::format::PropertyFormatter, but supports the use of expressions instead of just
  simple property callbacks.
- Removed division and modulo operators from compiler plug-in \alib{expressions::plugins,Arithmetics}
  with boolean divisor type (not useful and dangerous in respect to division by zero exceptions).
- Fixed minor formatting issues of program listings generated by \alib{expressions::detail,VirtualMachine::DbgList}.
- Corrected a longer list of typos and small errors of the module's manual. Added some notes
  about how sample boxes of abstract types can be created.

#### PS:####
We very much hope that this overhaul and "refactoring orgy" now finally led to a stable structure
that will not dramatically change in the future. The foundation set in this release allows
growth and extension of the library by new independent and optional \alibmods, similar to
modules \alib_alox, \alib_expressions and \alib_cli.

We think that the library is now well prepared to support the future C++ language feature
of modularization (see for example the [current clang draft](https://clang.llvm.org/docs/Modules.html))
without further need of heavy refactoring.

\I{################################################################################################}
\I{#####################################       V. 1805 R0     #####################################}
\I{################################################################################################}

# Version 1805, Revision 0, released May 12th, 2018 #

#### IMPORTANT INFORMATION: New C++ Fork Of ALib: ####

\par
  The C++ part of multi-language project [ALox Logging Library](http://alexworx.github.io/ALox-Logging-Library/)
  has been forked into this project. The goal and consequences of this effort are:
  - Have a repository and website exclusively dedicated to <b>%ALib for C++</b>.
  - Reverse the library nesting in the C++ Version: \alib here is now the main project, while
    \alox becomes one \alibmod.
  - Decouple the development and roadmaps of future \alib modules from <c>Java</c> and <c>C#</c>
    language versions of \alib/\alox.

#### General: ####
With the decoupling of the C++ version, the whole project structure has changed.
This reaches from directory names to the CMake and IDE-specific project files provided.

These changes are not listed in detail here.
Instead, for information about building this new version, users of previous versions should
read the \ref alib_manual and start "from scratch".

Support for preconfigured QTCreator project files was dropped.


#### Macros: ####
- Renamed macro \b %ALIB_SRCPOS_REL_EMPTY to \ref ALIB_CALLER_PRUNED and
- Renamed macro \b %ALIB_SRCPOS_REL_NULLED to \ref ALIB_CALLER_NULLED.
- Renamed macro \b %ALIB_OWN to \ref ALIB_OWN.
- Renamed macro \b %LOCK_HERE to \ref ALIB_LOCK.
- Renamed macro \b %LOCK_HERE_WITH to \ref ALIB_LOCK_WITH.
- Added macro  \ref ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS.

#### Module Core: ####
- Removed virtual base class \b %Ownable. Instead, class \b Owner now is templated.
  This allowed several classes to be non-virtual, for example \alib{threads,ThreadLock} or
  \alox{Lox}.
- Added method \b ThreadLock::Owner.
- Renamed method \b %DbgCountAcquirements to \alib{threads,ThreadLock::CountAcquirements}, removed
  its former parameter. The former behaviour can be accomplished using by comparing result of
  new method \b ThreadLock::Owner with \b Threads::CurrentThread.
- Fixed infinite recursion with method \b Enum::IsType.
- Added class \b Console.
- Overhauled class \b MemoryBlocks. More changes to be expected in future
  versions.
- Introduced \b StringTreeBlockAllocator and made class \b StringTree
  be able to use it. This allows very memory efficient allocations with \b %StringTree in
  appropriate use cases.


#### Module Strings: ####

- The underlying character type of all string classes became a template type.
  This lead to a complete overhaul of almost every type in namespace \b aworx::lib::strings.
  The changes of code are so dramatic that they can not be listed here completely.
  Nevertheless, user code should not be effected very much, because the now templated types have
  alias names that correspond to the former, non-templated types!
- Added virtual method \b FormatterBase::Clone.
- Removed static methods \b %Formatter::AcquireDefault and \b %Formatter::ReleaseDefault.
  Instead, a shared pointer holding a \b %Formatter object is available with
  \b GetDefaultFormatter.<br>
  The feature of resetting the default formatter with former method \b %Formatter::ReleaseDefault
  was moved to now overloaded method \b Formatter::Acquire
  and therefore is now available more generally to all formatters.
- Added compatibility with C++ 17 standard type \c std_string_view.

#### Module ALox: ####

As explained above, \alib_alox has been made a module of \alib. (Before this release, the
relationship was quite reverse: \alib then was the "underlying utility library" of \alox).<br>
With this, the namespace of \alox types has been renamed (moved) as well: from former \b "aworx::lox" to
\b aworx::lib::lox.

Furthermore, used new allocation mechanics of class \b %StringTree to efficiently allocate
internal data in respect to the "language scopes" of \alox.

#### Module Expressions: ####

This release introduces new module \alib_expressions.

#### Module CLI: ####

This release introduces new module \alib_cli. However the programmer's manual of that module was
not written, yet. Due to the lack of documentation, we can not really recommend to use this module.
As we are using it with other projects already, the code itself is considered stable and tested.


\I{################################################################################################}
\I{#####################################        Older         #####################################}
\I{################################################################################################}

# Older Versions #

All previous versions have been maintained as a part of <b>multi-language project</b>
[ALox Logging Library](http://alexworx.github.io/ALox-Logging-Library/).

Please consult this project's [Change-Log](http://alexworx.github.io/ALox-Logging-Library/changelog.html)
for information on changes since the initial release in May 2013.

<br><br><br><br><br><br> */

