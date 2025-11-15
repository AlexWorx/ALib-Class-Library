// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

####################################################################################################
### Definitions needed by doxygen
####################################################################################################
#define ALIB_GCC
#define ALIB_COMPILATION_FLAGS
#define ALIB_EXT_LIB_THREADS_AVAILABLE
#define ALIB_C20_MODULES

#define ALIB_API_IS_DLL
#define ALIB_API_NO_DLL

#define ALIB_CPP_STANDARD

#define ALIB_DEBUG
#define ALIB_SINGLE_THREADED

#define ALIB_SIZEOF_INTGAP
#define ALIB_INTGAP_TYPE
#define ALIB_SIZEOF_LONGDOUBLE_REPORTED
#define ALIB_SIZEOF_LONGDOUBLE_WRITTEN

#define ALIB_CHARACTERS_NATIVE_WCHAR

#define ALIB_GTEST
#define ALIB_DEBUG_ASSERTION_PRINTABLES
#define DOXYGEN
#define ALIB_BASE_DIR
#define ALIB_PRECOMPILED_HEADER


#define ALIB_FEAT_SINGLETON_MAPPED                 1
#define ALIB_DEBUG_ALLOCATIONS                     1
#define ALIB_DEBUG_BOXING                          1
#define ALIB_DEBUG_STRINGS                         1
#define ALIB_DEBUG_CONTAINERS                      1
#define ALIB_DEBUG_CRITICAL_SECTIONS               1
#define ALIB_DEBUG_MEMORY                         1
#define ALIB_DEBUG_ARRAY_COMPRESSION               1
#define ALIB_DEBUG_RESOURCES                       1
#define ALIB_FEAT_BOOST_REGEX                      1
#define ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS      1
#define ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS       1
#define ALIB_FEAT_BOXING_BIJECTIVE_FLOATS          1
#define ALOX_DBG_LOG                               1
#define ALOX_REL_LOG                               1
#define ALOX_DBG_LOG_CI                            1
#define ALOX_REL_LOG_CI                            1
                                            
####################################################################################################
### Page alib_prepro
####################################################################################################

/**
\page alib_ref_prepro    ALib and the C++ Preprocessor

\I{################################################################################################}
# Naming scheme #
All preprocessor symbols and macros are prefixed with <b>"ALIB_"</b>. Most symbols then continue with
the name of the \alibmod that uses/defines them.
One important exception to this naming scheme is the symbols defined by module \alib_alox,
which are prefixed <b>"ALOX_"</b>.

\I{################################################################################################}
# ALib Compiler Symbols #
Compiler symbols are used to select different versions of code, dependent on
- the platform,
- the <em>build type</em> (e.g., debug or release-builds),
- ALib module selection and
- ALIb feature selection.

This documentation often refers to such symbols as "code selection symbols".
Many of the symbols can be passed to the compiler to overwrite their default value.
This is usually done with the <b>-D</b>-option of a compiler's invocation command.

Besides the internal use to select \alib library code, the symbols can be used with custom code
for the same purpose. This is especially necessary if a custom software is designed to
work with different \alibbuilds and different optional feature sets of \alib.

All symbols are documented with:

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref GrpALibPreproSymbols <br>

\I{################################################################################################}
# ALib Macros #
Preprocessor macros, which expand given arguments to more complex code, are given with

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref GrpALibPreproMacros <br>

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}

@defgroup GrpALibPreproSymbols ALib Compiler Symbols
\details
This is the reference documentation for preprocessor symbols that evaluate to \c true or \c false
(precisely \c 1 or \c 0). Unless explicitly mentioned, the symbols may be passed to the compiler
to overwrite their documented default value.

\note
   The reference documentation for preprocessor <em>macros</em> , is
   \ref GrpALibPreproMacros "separately given here".

The following sections of this document lists macros that are not related to a specific
\alibmod:

&nbsp;&nbsp;      \ref GrpALibPreproSymbols_general          "General Symbols"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_general_deduced  "General Symbols Deduced By Headers"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_modsel           "ALib Module Selection"

Followed to this, module-specific symbols are documented:

&nbsp;&nbsp;      \ref GrpALibPreproSymbols_alox        "Module ALox"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_bitbuffer   "Module BitBuffer"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_boxing      "Module Boxing"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_characters  "Module Characters"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_containers  "Module Containers"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_monomem     "Module Monomem"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_singletons  "Module Singletons"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_strings     "Module Strings"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_threads     "Module Threadsd"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_camp        "Camp Basecamp"
<br> &nbsp;&nbsp; \ref GrpALibPreproSymbols_strings     "Camp Files"


@{


\I{################################################################################################}
\I{##############################           General              ################################  }
\I{################################################################################################}

\name General Preprocessor Symbols
\anchor GrpALibPreproSymbols_general
The following general symbols (aka symbols not related to distinct \alibmods) may be passed to the
compiler:
@{
\def ALIB_DEBUG
  If true (1), plausibility checks and \alib_assertions are enabled.

\def ALIB_SINGLE_THREADED
  If true (1), a single-threaded library is compiled. With debug compilations, the multi-threading
  use of \alib may be detected, and if so, \ref alib_mod_assert "raises an assertion".<br>
  If the symbol is not given, a library enabled for multi-threading use is compiled. This is the
  default.
  
  If using the \ref alib_manual_build_cmake_3 "corresponding CMake variable", script
  \alibfile{build/cmake/ALib.cmake} will include <b>_ST</b> to the compiled library's filename.
             
  \see
    - Corresponding macros \ref IF_ALIB_THREADS and \ref IFNOT_ALIB_THREADS which are useful
      for pruning short code snippets.
    - Chapter \ref alib_threads_intro_agnostic "Threading-Agnostic Software" of the
      Programmer's Manual module \alib_threads.


\def ALIB_EXT_LIB_THREADS_AVAILABLE
  If this symbol is given, then \alib will insert assertions in debug-compilations in the case
  that module \alib_threads is not included in the \alibbuild. The assertions will test
  if a new, unknown thread is executing the code.
  This test is performed each time a threaded version of the library would acquire a mutex. <br>
  If using the default \b CMake script for \alib, this variable will be set unless either
  <b>CMake</b>-variable \ref ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH is set to \c true, or no
  thread library was found.


\def ALIB_API_IS_DLL
 This compiler-symbol has to be defined when compiling \alib as a DLL, and has to be undefined
 (or \c 0) when compiling user code that includes \alib header-files.

 If \ref ALIB_API_NO_DLL is given, this symbol is ignored.


\def ALIB_API_NO_DLL
 This compiler-symbol has to be defined when compiling \alib classes directly into a project
 Windows/MSC (not using a DLL).<br>
 If given, \ref ALIB_API_IS_DLL is ignored. If not given, \b ALIB_API_IS_DLL has to be set
 to \c 1 if the DLL is compiled, and to \c 0 if code that is using \alib residing in a DLL is compiled.

 \see #ALIB_API_IS_DLL and #ALIB_DLL

\def DOXYGEN
  Defined by \https{Doxygen,https://www.doxygen.nl} when parsing source code.
  Not defined with parsers of a C++ compiler. Primarily used to provide a doxygen
  compatible/readable version of complex declarations and definitions.
               
\def ALIB_DEBUG_ASSERTION_PRINTABLES
  If this symbol is set, then all assertion macros will call an internal test-function upfront
  that checks that for each given message parameter, a
  \alib{assert;RegisterPrintable;registered serialization function} exists. This is
  especially true for the conditional assertion macros, like \ref ALIB_ASSERT_ERROR or
  \ref ALIB_ASSERT_WARNING. With that, it can be tested that all assertions are properly
  implemented, even if they are not raised in a test run of a program.
  
  If set, such an assertion is performed by calling function \alib{assert::CheckArgs}.
   
\def ALIB_GTEST
  Selects unit test code in accordance to the Google Test libraries.
  Defaults to \c 0 if not specified.
  
\def ALIB_PRECOMPILED_HEADER
  Compiler symbol that can be passed to enable inclusions in the header file
  \ref alib_precompile.hpp.
  
\def ALIB_C20_MODULES
  This symbol is deprecated and must not be used. 
   @see Our \ref alib_c20module_shift "blog page" on that topic. 
    
@}

\I{################################################################################################}
\I{##############################    General Deduced Symbols   ####################################}
\I{################################################################################################}

\name General Preprocessor Symbols Exposed By ALib
\anchor GrpALibPreproSymbols_general_deduced
The following general symbols (aka symbols not related to distinct \alibmods) are deduced by
\alib headers and thus usually are not to be passed to the compiler.

 \note Since \alib release 2312, the automatic detection of some symbols may be disabled by passing
       them from outside. This allows compiling the code with platform/toolchain combinations that
       are not 'known' to \alib.
       See \ref ALIB_SIZEOF_INTEGER for more information.

@{

\def ALIB_CPP_STANDARD
  Symbol containing the C++ language standard used for compilation.
  Possible values are \c 20 and \c 23.<br>
  For MSC and GNU GCC (and compatible compilers like Clang), this macro is automatically detected.
  For unknown compilers/toolchains, this symbol may be passed to the compiler.<br>

\def ALIB_GCC
  Symbol which is \c 1 defined when an unknown compiler is used and this compiler is
  specifying \c __GNUC__ (what most compilers do).
  This deduction to "the real gcc" can never be complete, but is the best guess available.
  A much more complete deduction is provided by boost libraries for example.
  \alib is tested only on a limited set of platforms/compilers.

\def ALIB_SIZEOF_INTEGER
  Contains the size of \c std::size_t, \c std::ptrdiff_t, <c>void*</c>, #alib::integer, and
  #alib::uinteger, which all should be the same, but independent on the compiler and platform
  (processor type).
  Usually, this symbol is deduced (in the internal header \alibheader{lang/integers.inl}).
  If this deduction fails, this may result in two possible outcomes:
  1. A dedicated compilation error hinting to this documentation.
  2. Less directly connected compilation errors.

  In this case, the right value has to be provided to the compiler. When doing this, symbols
  - \ref ALIB_INTGAP_TYPE,
  - \ref ALIB_SIZEOF_INTGAP,
  - \ref ALIB_SIZEOF_LONGDOUBLE_REPORTED and<br>
  - \ref ALIB_SIZEOF_LONGDOUBLE_WRITTEN<br>

  have to be provided as well. This is true for each symbol: If one of them is given explicitly
  to the compiler, all the five have to be given.
  Otherwise, an explicit error message will be given during compilation.

\def ALIB_INTGAP_TYPE
  The type of \alib{intGap_t}.
  Usually, this symbol is deduced (in the internal header \alibheader{lang/integers.inl})
  depending on the compiler and platform.
  @see If platform-dependent deduction fails, see documentation of \ref ALIB_SIZEOF_INTEGER for
       information about providing compiler-symbols.

\def ALIB_SIZEOF_INTGAP
  Contains the size of integral types \alib{intGap_t} and \alib{uintGap_t}.<br>
  Usually, this symbol is deduced (in the internal header \alibheader{lang/integers.inl})
  depending on the compiler and platform.

  @see If platform-dependent deduction fails, see documentation of \ref ALIB_SIZEOF_INTEGER for
       information about providing compiler-symbols.

\def ALIB_SIZEOF_LONGDOUBLE_REPORTED
  Contains the size of <c>long double</c>. This is the value that <c>sizeof(long double)</c>
  returns. This size might be higher to what a copy operation of a value of the type might
  write.
  Usually, this symbol is deduced (in the internal header \alibheader{lang/integers.inl})
  depending on the compiler and platform.

  @see If platform-dependent deduction fails, see documentation of \ref ALIB_SIZEOF_INTEGER for
       information about providing compiler-symbols.<br>

  @see Sibling symbol \ref ALIB_SIZEOF_LONGDOUBLE_WRITTEN.

\def ALIB_SIZEOF_LONGDOUBLE_WRITTEN
  Symbol which denotes how many bytes the hardware will write with a value of type <c>long double</c>.
  This size might be smaller than the amount of memory that an allocation of that type would consume,
  hence what is reported by <c>sizeof(long double)</c>.<br>
  Usually, this symbol is deduced (in the internal header \alibheader{lang/integers.inl})
  depending on the compiler and platform.

  @see If platform-dependent deduction fails, see documentation of \ref ALIB_SIZEOF_INTEGER for
       information about providing this compiler-symbol together with four others.<br>

  @see The value of this symbol is used to define the specialization of
       \alib{boxing;SizeTraits} for template type <c>long double</c>.
@}


\I{################################################################################################}
\I{############################           Module Selection          #############################  }
\I{################################################################################################}
\name ALib Module Selection
\anchor GrpALibPreproSymbols_modsel

@{

This group of symbols determine the availability of different <b>%ALib Modules</b> of the
\alibbuild used.
The symbols are defined in the internal header \alibheader{alib.inl}, which always is and has to
be the first \alib header-file included (directly or indirectly).
If none of the symbols is passed to the compiler, this header-file will define all to \c 1,
hence will include all modules an \alibbuild.

If a symbol is given as \c 1, all corresponding symbols of dependent modules are
also defined to \c 1, even if one had explicitly given as \c 0, which is considered a
contradicting input.

\def ALIB_ALOX
  Denotes if module \alib_alox is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_ALOX and \ref IFNOT_ALIB_ALOX which are useful
    for pruning short code snippets.

\def ALIB_BITBUFFER
  Denotes if module \alib_bitbuffer is included in the \alibbuild.

\def ALIB_BOXING
  Denotes if module \alib_boxing is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_BOXING and \ref IFNOT_ALIB_BOXING which are useful
    for pruning short code snippets.

\def ALIB_CAMP
  Denotes if module \alib_camp is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_CAMP and \ref IFNOT_ALIB_CAMP which are useful
    for pruning short code snippets.

\def ALIB_FORMAT
  Denotes if module \alib_format is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_FORMAT and \ref IFNOT_ALIB_FORMAT which are useful
    for pruning short code snippets.

\def ALIB_SYSTEM
  Denotes if module \alib_system is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_SYSTEM and \ref IFNOT_ALIB_SYSTEM which are useful
    for pruning short code snippets.
                                     
\def ALIB_EXCEPTIONS
  Denotes if module \alib_exceptions is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_EXCEPTIONS and \ref IFNOT_ALIB_EXCEPTIONS which are useful
    for pruning short code snippets.

\def ALIB_RESOURCES
  Denotes if module \alib_resources is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_RESOURCES and \ref IFNOT_ALIB_RESOURCES which are useful
    for pruning short code snippets.



\def ALIB_CLI
  Denotes if module \alib_cli is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_CLI and \ref IFNOT_ALIB_CLI which are useful
    for pruning short code snippets.

\def ALIB_VARIABLES
  Denotes if module \alib_variables is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_VARIABLES and \ref IFNOT_ALIB_VARIABLES which are useful
    for pruning short code snippets.

\def ALIB_CONTAINERS
  Denotes if module \alib_containers is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_CONTAINERS and \ref IFNOT_ALIB_CONTAINERS which are useful
    for pruning short code snippets.

\def ALIB_ENUMRECORDS
  Denotes if module \alib_enumrecords is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_ENUMRECORDS and \ref IFNOT_ALIB_ENUMRECORDS which are useful
    for pruning short code snippets.

\def ALIB_EXPRESSIONS
  Denotes if module \alib_expressions is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_EXPRESSIONS and \ref IFNOT_ALIB_EXPRESSIONS which are useful
    for pruning short code snippets.

\def ALIB_FILES
  Denotes if module \alib_files is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_FILES and \ref IFNOT_ALIB_FILES which are useful
    for pruning short code snippets.

\def ALIB_MONOMEM
  Denotes if module \alib_monomem is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_MONOMEM and \ref IFNOT_ALIB_MONOMEM which are useful
    for pruning short code snippets.

\def ALIB_SINGLETONS
  Denotes if module \alib_singletons is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_SINGLETONS and \ref IFNOT_ALIB_SINGLETONS which are useful
    for pruning short code snippets.

\def ALIB_STRINGS
  Denotes if module \alib_strings is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_STRINGS and \ref IFNOT_ALIB_STRINGS which are useful
    for pruning short code snippets.

\def ALIB_THREADMODEL
  Denotes if module \alib_threadmodel is included in the \alibbuild.
  \see
    Corresponding macros \ref IF_ALIB_THREADMODEL and \ref IFNOT_ALIB_THREADMODEL which are useful
    for pruning short code snippets.
@}


\I{################################################################################################}
\I{###################################       BitBuffer       ######################################}
\I{################################################################################################}
\name ALib Module BitBuffer Feature Selection
\anchor GrpALibPreproSymbols_bitbuffer
@{

\def ALIB_DEBUG_ARRAY_COMPRESSION
  Selects code that reads back integral arrays after compression with class
  \alib{bitbuffer::ac_v1;ArrayCompressor} to check compression/decompression algorithms.<br>
  This symbol defaults to \b true in debug-builds of the library.
@}


\I{################################################################################################}
\I{###################################         Boxing        ####################################  }
\I{################################################################################################}
\name ALib Module Boxing Feature Selection
\anchor GrpALibPreproSymbols_boxing

@{

\def ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
  If \c 0 (the default), selects code for boxing any C++ integral type to \alib{integer},
  respectively \alib{uinteger}.

  If passed as \c 1, bijective boxing is enabled, what boxes each type "as is".

  \see
    Documentation of namespace \ref alib_boxing_fundamentals "4. Boxing Fundamental Types"
    for more information.

\def ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
  If \c 0 (the default), selects code for boxing types \c char, \c wchar_t, \c char16_t and \c char32_t as a
  \alib{characters;character} and disallowing to unbox any other.

  If passed as \c 1, bijective boxing is enabled, what boxes each type as it is.

  \see
    Documentation of namespace \ref alib_boxing_fundamentals "4. Boxing Fundamental Types"
    for more information.

\def ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
  If \c 0(the default), selects code for boxing type \c float as a \c double value and disallowing to unbox
  \c float.

  If passed as \c 1, bijective boxing is enabled, what boxes type \c float as it is.

  \see
    Documentation of namespace \ref alib_boxing_fundamentals "4. Boxing Fundamental Types"
    for more information.

\def ALIB_DEBUG_BOXING
  Selects extended debug code in module \alib_boxing.<br>
  If given as \c true in release-builds, the symbol is redefined to \c false, and a compiler
  pragma-warning is given.

  Details on the features enabled by this symbol are given in chapter
  \ref alib_boxing_more_debug_general of the Programmer's Manual of module \alib_boxing_nl.


@}

\I{################################################################################################}
\I{###################################       Characters      ####################################  }
\I{################################################################################################}
\name ALib Module Characters Feature Selection
\anchor GrpALibPreproSymbols_characters
@{

\def ALIB_CHARACTERS_WIDE
  If \c false, type \alib{characters;character} is equivalent to C++ built-in type \c char.
  Otherwise, \alib{characters;character} equals the multi-byte character type \alib{characters;wchar},
  which in turn is equivalent to either of \c wchar_t, \c char16_t or \c char32_t.

  Note, that the use of this symbol to select code should be needed very seldom.
  Instead, it is advised to use "logical character" types and other provided helpers and macros
  that convert characters and string-types transparently.

  If the symbol is not externally set (passed to the compiler), the choice of the default character
  type is system-dependent:
  On Windows OS, wide character strings are chosen, while on Unix-like OSes, narrow character
  strings are used by default.

  \see
    Chapters \ref alib_characters_prepro_literals and chapter
    \ref alib_characters_chars "2. Character Types" of the Programmer's Manual of module
    \alib_characters.


\def ALIB_CHARACTERS_NATIVE_WCHAR
  If \c true, type \alib{characters;wchar} is equivalent to C++ built-in type \c wchar_t.
  Otherwise, \alib{characters;wchar} has a different width than \c wchar_t and equals
  to either \c char16_t or \c char32_t: If a compiler's defines \c wchar_t as a 2-byte integral,
  \c char32_t is chosen and \c char16_t if it is a 4-byte integral.

  This symbol depends on operating system and compiler defaults and may be manipulated by passing
  symbol \ref ALIB_CHARACTERS_SIZEOF_WCHAR with library compilation.

  Note, that the use of this symbol to select code should be needed very seldom.
  Instead, it is advised to use "logical character" types and other provided helpers and macros
  that convert characters and string-types transparently.

  \see
     Chapter \ref alib_characters_prepro_literals of the
     Programmer's Manual of module \alib_characters.


\def ALIB_CHARACTERS_SIZEOF_WCHAR
  Provides the width of \alib type \alib{characters;wchar} in bytes.

  If not set, a compiler/platform-dependent default value is chosen.

  If passed to the compiler, then type \alib{characters;wchar} will be set independent of the
  width of \c wchar_t, which is compiler dependent and
  symbol ALIB_CHARACTERS_NATIVE_WCHAR will be set accordingly.

  If a value of \c 2 is given, then type \alib{characters;wchar} will be \b 2 bytes wide.
  Precisely, \b wchar will be equivalent to type \c wchar_t with compilers that define
  the latter as a \b 2-byte integral value and equivalent to type \c char16_t, if \c wchar_t is a
  \b 4-byte integral.

  If a value of \c 2 is given, then type \alib{characters;wchar} will be \b 4 bytes wide.
  Precisely, \b wchar will be equivalent to type \c wchar_t with compilers that define
  the latter as a \b 4-byte integral value and equivalent to type \c char32_t, if \c wchar_t is a
  \b 2-byte integral.

  Other values are not allowed.

  \see
     Chapter \ref alib_characters_prepro_literals of the
     Programmer's Manual of module \alib_characters.

@}

\I{################################################################################################}
\I{###################################      Containers       ######################################}
\I{################################################################################################}
\name ALib Module Containers Feature Selection
\anchor GrpALibPreproSymbols_containers
@{

\def ALIB_DEBUG_CONTAINERS
  Selects extended debug code in module \alib_containers.<br>
  If given as \c true in release-builds, the symbol is redefined to \c false, and a compiler
  pragma-warning is given.

  Please consult chapter \ref alib_contmono_further_debug of the Programmer's Manual of module
  \alib_containers for further information about the debug features that come available with
  this symbol.
@}


\I{################################################################################################}
\I{###################################         Monomem       ######################################}
\I{################################################################################################}
\name ALib Module Monomem Feature Selection
\anchor GrpALibPreproSymbols_monomem
@{

\def ALIB_DEBUG_MEMORY
  Selects extended debug code in module \alib_monomem.<br>
  If given as \c true in release-builds, the symbol is redefined to \c false, and a compiler
  pragma-warning is given.

  Please consult chapter \ref alib_contmono_further_debug of the Programmer's Manual of camp
  \alib_monomem for further information about the debug features that come available with
  this symbol.

  \note Setting this symbol requires the availability of \alib_alox in the \alibbuild.
  
  @see Compiler symbol \ref ALIB_DEBUG_ALLOCATIONS.
  
\def ALIB_DEBUG_ALLOCATIONS
  Maeks all \alib{lang;Allocator;ALib allocators} to surround allocations with magic bytes.
  If given as \c true in release-builds, the symbol is redefined to \c false, and a compiler
  pragma-warning is given.

  Please consult chapter \ref alib_contmono_further_debug of the Programmer's Manual of camp
  \alib_monomem for further information about the debug features that come available with
  this symbol.

  \note Setting this symbol requires the availability of \alib_alox in the \alibbuild.
  
  @see Compiler symbol \ref ALIB_DEBUG_MEMORY.
  
\def ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT
  This symbol defaults to <c>"alignof(uint64_t)"</c> and is used as the default value
  of template parameter \p{TAlignment} of class \alib{monomem;TPoolAllocator}.
  The importance of this default value arises from the fact that the alias type definition
  \ref alib::PoolAllocator uses this default-alignment and this alias is in turn used across
  \alib.
  In case this value needs to be changed, please note that the using code linked against
  a compiled library needs to have this symbol set accordingly.
  Otherwise, undefined behavior may occur.
  
  @see Chapter \ref alib_contmono_further_alignment of the Programmer's Manual of module
       \alib_monomem.
@}
            
\I{################################################################################################}
\I{#################################          Singletons         ################################  }
\I{################################################################################################}
\name ALib Module Singletons Feature Selection
\anchor GrpALibPreproSymbols_singletons
@{

\def ALIB_FEAT_SINGLETON_MAPPED
  Affects implementation of class
  \alib{singletons;Singleton}. If this symbol represents \c true, then
  a static hash map is used to find the singleton objects, even if different data segments
  exist within one software process.<br>
  While the overhead imposed by such implementation is quite small, with this symbol being
  \c false, the overhead is omitted in scenarios when such effort is not necessary.

  The symbol defaults to \c true on the windows platform, otherwise to \c false, which may
  be overwritten by passing the symbol to the compiler.
@}


\I{################################################################################################}
\I{#################################          Strings         ###################################  }
\I{################################################################################################}
\name ALib Module Strings Feature Selection
\anchor GrpALibPreproSymbols_strings
@{

\def ALIB_FEAT_BOOST_REGEX
  Selects code for class \alib{strings::util;RegexMatcher} and features within \alib
  that uses this wrapper class.

  If this symbol is not explicitly given to the compiler, it is set to \c 0,
  the \b %boost library is not included and class \b %RegexMatcher is not available.

  If using the \ref alib_manual_build_cmake_3 "corresponding CMake variable", script
  \alibfile{build/cmake/ALib.cmake} will search and add \b boost_regex to \e CMake variable
  \b ALIB_EXTERNAL_LIBS.

  Please note that this script also performs

        set(Boost_USE_STATIC_LIBS   ON)

  before searching the library.


\def ALIB_DEBUG_STRINGS
  Selects extended debug code, mostly within class \alib{strings;TAString;AString}.<br>
  If given as \c true in release-builds, the symbol is redefined to \c false, and a compiler
  pragma-warning is given.

  In particular, the symbol enables internal consistency checks as described in chapter
  \ref alib_strings_details_debugging of the Programmer's Manual of module \alib_strings_nl.
  Internally, macro \ref ALIB_STRING_DBG_CHK is used and users of the library are free (recommended)
  to use the ame for custom consistency checks when manipulating \b %AString buffers directly.
@}

\I{################################################################################################}
\I{###################################       Threads         ####################################  }
\I{################################################################################################}
\name ALib Module Threads Feature Selection
\anchor GrpALibPreproSymbols_threads
@{

\def ALIB_DEBUG_CRITICAL_SECTIONS
  This symbol - while associated to module \alib_threads - activates type
  \alib{lang::DbgCriticalSections}, which is available independent of the inclusion of
  this module. However, the symbol itself will always be disabled (set to
  \c 0) if module \alib_threads_nl is not included in the \alibbuild. This design allows using
  the corresponding macros anywhere in the code, without further checks. In the absence of
  \alib_threads_nl, all macros will be just empty. The same is true for type
  \b DbgCriticalSections, which will be completely empty and optimized out.
  
 @see Macros GrpALibMacros_mod_threads
@}

\I{################################################################################################}
\I{################################          Basecamp         ####################################}
\I{################################################################################################}
\name ALib Module Basecamp Feature Selection
\anchor GrpALibPreproSymbols_camp
@{

\def ALIB_CAMP_OMIT_DEFAULT_RESOURCES
  If set, code that defines static default resource strings is pruned, and thus, all
  static resource string data is pruned from the library's executable's data segment.

  The omission occurs in the implementations of \alib{camp;Camp::Bootstrap} of various \alibmods,
  where methods \alib{resources;ResourcePool::Bootstrap} and
  \alib{resources;ResourcePool::BootstrapBulk} are used to define the default resources.

  If resources are omitted, the bootstrap process has to be customized to use an external
  resource provider. Otherwise the library has undefined behavior as resources are missing.

  \see
    Chapter \ref alib_resources_details_export "3.6.2 Exporting Resources For Externalization" of the
    Programmer's Manual of module \alib_resources for more information.


\def ALIB_DEBUG_RESOURCES
  Selects extended debug code in module \alib_resources.<br>
  If given as \c true in release-builds, the symbol is redefined to \c false, and a compiler
  pragma-warning is given.

  In particular, the following features become enabled:
  - Static field \alib{resources;LocalResourcePool::DbgResourceLoadObserver} becomes available
    which supports finding resource definition errors. If set before bootstrapping,
    methods \alib{resources;LocalResourcePool::BootstrapBulk} and
    \alib{resources;LocalResourcePool::BootstrapAddOrReplace} will write information on
    bulk and singular resource data definitions.
  - Type \alib{resources;LocalResourcePool} collects statistics about the number of
    acquisitions of particular resources. These statistics can be read by directly accessing the
    internal hash table using method \alib{resources::LocalResourcePool;BootstrapGetInternalHashMap}
    or using one of the convenient functions below.
  - Method \alib{resources;ResourcePool::DbgGetList}
  - Method \alib{resources;ResourcePool::DbgGetCategories}
  - Function \alib{resources;DbgDump}
  - Method \alib{resources;LocalResourcePool::DbgGetList}
 
\def ALIB_PATH_CHARACTERS_WIDE
  This symbol is set to \c 1 if the character type defined by the C++ standard
  library with type <c>std::filesystem::path::value_type</c> is <c>wchar_t</c>.
  Otherwise, it is set to \c 0.<br>
  The symbol is set in \implude{System} but can only be "guessed".
  The guess is that only if the symbol <c>_MSC_VER</c> is set, wide characters are used.
  The header raises a static assertion error if the macro is wrongly guessed or externally set.
  
  Should the guess be wrong, the right value has to be passed with compilation of the library.
@}
                                       
\I{################################################################################################}
\I{###################################        Files          ####################################  }
\I{################################################################################################}
\name ALib Module Files Feature Selection
\anchor GrpALibPreproSymbols_files
@{

\def ALIB_FILES_FORCE_STD_SCANNER
  If given, platform-specific code for scanning files is ignored and instead function
  \alib{files;ScanFiles} uses an implementation based on C++ library <c>std::filesystem</c>.
  This flag was is used by the developers of \alib to test the standard implementation and should
  not be set otherwise, because platform-specific implementations are more feature reach and better
  performing.

\def ALIB_FILES_SCANNER_IMPL
  This symbol is evaluated internally with the inclusion of \implude{Files}.
  It allows to determine the implementation of the file scanner used.
  As of this library version, the following values are possible:<br>
  - \ref ALIB_FILES_SCANNER_STDFS
  - \ref ALIB_FILES_SCANNER_POSIX

  The symbol must not be set from outside.
  To manipulate the choice, use symbol \ref ALIB_FILES_FORCE_STD_SCANNER instead.
    
\def ALIB_FILES_SCANNER_STDFS
  A possible value of \ref ALIB_FILES_SCANNER_IMPL, evaluating to \b 1, and denoting the
  implementation using C++ library <c>std::filesystem</c>.<br>
  Must not be set from outside.
   
\def ALIB_FILES_SCANNER_POSIX
  A possible value of \ref ALIB_FILES_SCANNER_IMPL, evaluating to \b 2, and denoting the
  implementation using the Posix library calls.<br>
  Must not be set from outside.

@}

\I{################################################################################################}
\I{###################################          ALox         ####################################  }
\I{################################################################################################}
\name ALib Module ALox Feature Selection
\anchor GrpALibPreproSymbols_alox
@{

This group of code selection symbols is defined with the use of module \alib_alox and
used to select the inclusion of log code. The symbols are seldom to be used by end-user code.
Instead, the macros used for the log statements themselves (see
\ref GrpALibMacros_mod_alox_debug "debug logging macros" and
\ref GrpALibMacros_mod_alox_release "release logging macros".) are variably defined based on these symbols.

\def ALOX_DBG_LOG
  If defined as \c true, debug <em>Log Statements</em> are enabled. Hence, users of \alox can
  conditionally compile special debug logging code that normally belongs to corresponding
  debug logging statements.
  This is useful for example, to conditionally compile code that calculates and gathers
  information to do some more complex log output.

\def ALOX_REL_LOG
  If defined, release <em>Log Statements</em> are enabled. Hence, users of \alox can conditionally
  compile special release logging code that belong to corresponding release logging statements.
  (Note that release logging still can be pruned as explained in
  \ref alib_mod_alox_debug_and_release_logging_ft_1.)<br>
  This is useful for example, to conditionally compile code that calculates and gathers
  information to do some more complex log output.


\def ALOX_DBG_LOG_CI
  If defined, scope information is passed to \alox on invocation of debug
  <em>Log Statements</em>.
  Hence, users of \alox can conditionally compile special debug logging code that normally
  belongs to corresponding debug logging statements based on this symbol. A sample would be
  the definition of different log line meta-information formats, depending on the availability
  of scope information.

  \note The \alox <em>Scope Domain</em> mechanism as well as ALox features provided with
        \alib{lox;Lox::Once} and \alib{lox;Lox::Store} rely on caller information.
        These mechanisms are therefore not available to debug logging unless this symbol is set.


\def ALOX_REL_LOG_CI
  Explicitly enables the generation of scope information using the built-in preprocessor
  macros like "__FILE__" and "__LINE__" for \alox release logging statements, which is
  disabled by default.

  Enabling source info for release logging is seldom wanted. Release executables should
  not generate log output that an end user is not able to understand.
  It can make sense however, if release log information from the field goes back to the
  software development team.
  Furthermore, it can be very helpful to enable scope information for release <em>Log Statements</em>
  if set in the debug version of a compilation unit.

  \note The \alox <em>Scope Domain</em> mechanism as well as ALox features provided with
        \alib{lox;Lox::Once} and \alib{lox;Lox::Store} rely on caller information.
        These mechanisms are therefore not available to release logging unless this symbol is set.
@}

@}

*/
