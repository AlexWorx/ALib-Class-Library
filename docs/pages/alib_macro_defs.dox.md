// #################################################################################################
//  Documentation - ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**

\I{################################################################################################}
\I{################################################################################################}
\I{##########################         CONFIGURATION MACROS         ################################}
\I{################################################################################################}
\I{################################################################################################}

\I{################################################################################################}
\I{##############################           General              ################################  }
\I{################################################################################################}

\def ALIB_DEBUG
  If true (1), plausibility checks and \alib_assertions are enabled.

\def ALIB_SINGLE_THREADED
  If true (1), a single-threaded library is compiled. With debug compilations, the multi-threading
  use of \alib may be detected, and if so, #"alib_mod_assert;raises an assertion".<br>
  If this configuration macro is not given, a library enabled for multi-threading use is compiled. 
  This is the default.
  
  If using the #"alib_manual_build_cmake_3;corresponding CMake variable", the script
  #"^F;ALib.cmake" will include <b>_ST</b> to the compiled library's filename.
             
  \see
    - Corresponding macros #"IF_ALIB_THREADS" and #"IFNOT_ALIB_THREADS" which are useful
      for pruning short code snippets.
    - Chapter #"alib_threads_intro_agnostic" of the Programmer's Manual module \alib_threads.


\def ALIB_EXT_LIB_THREADS_AVAILABLE
  If this configuration macro is given, then \alib will insert assertions in debug-compilations in 
  the case that module \alib_threads is not included in the \alibbuild. The assertions will test
  if a new, unknown thread is executing the code.
  This test is performed each time a threaded version of the library would acquire a mutex. <br>
  If using the default \b CMake script for \alib, this variable will be set unless either
  <b>CMake</b>-variable #"ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH;1" is set to \c true, or no
  thread library was found.


\def ALIB_API_IS_DLL
 This configuration macro has to be defined when compiling \alib as a DLL, and has to be undefined
 (or \c 0) when compiling user code that includes \alib header-files.

 If #"ALIB_API_NO_DLL" is given, this configuration macro is ignored.


\def ALIB_API_NO_DLL
 This configuration macro has to be defined when compiling \alib classes directly into a project
 Windows/MSC (not using a DLL).<br>
 If given, #"ALIB_API_IS_DLL" is ignored. If not given, \b ALIB_API_IS_DLL has to be set
 to \c 1 if the DLL is compiled, and to \c 0 if code that is using \alib residing in a DLL is compiled.

 \see #ALIB_API_IS_DLL and #ALIB_DLL

\def DOXYGEN
  Defined by \https{Doxygen,https://www.doxygen.nl} when parsing source code.
  Not defined with parsers of a C++ compiler. Primarily used to provide a doxygen
  compatible/readable version of complex declarations and definitions.
               
\def ALIB_DEBUG_ASSERTION_PRINTABLES
  If this configuration macro is set, then all assertion macros will call an internal test-function upfront
  that checks that for each given message parameter, a
  #"RegisterPrintable;registered serialization function" exists. This is
  especially true for the conditional assertion macros, like #"ALIB_ASSERT_ERROR" or
  #"ALIB_ASSERT_WARNING". With that, it can be tested that all assertions are properly
  implemented, even if they are not raised in a test run of a program.
  
  If set, such an assertion is performed by calling function #"assert::CheckArgs".
   
\def ALIB_GTEST
  Selects unit test code in accordance to the Google Test libraries.
  Defaults to \c 0 if not specified.
  
\def ALIB_PRECOMPILED_HEADER
  Configuration macro that can be passed to enable inclusions in the header file 
  #"F;alib_precompile.hpp".
  
\def ALIB_C20_MODULES
  This configuration macro is deprecated and must not be used. 
   @see Our #"alib_c20module_shift;blog page" on that topic. 
    

\I{################################################################################################}
\I{########################    General Deduced Configuration macros   #############################}
\I{################################################################################################}

\def ALIB_CPP_STANDARD
  Configuration macro containing the C++ language standard used for compilation.
  Possible values are \c 20 and \c 23.<br>
  For MSC and GNU GCC (and compatible compilers like Clang), this macro is automatically detected.
  For unknown compilers/toolchains, this configuration macro may be passed to the compiler.<br>

\def ALIB_GCC
  Configuration macro which is \c 1 defined when an unknown compiler is used and this compiler is
  specifying \c __GNUC__ (what most compilers do).
  This deduction to "the real gcc" can never be complete, but is the best guess available.
  A much more complete deduction is provided by boost libraries for example.
  \alib is tested only on a limited set of platforms/compilers.

\def ALIB_SIZEOF_INTEGER
  Contains the size of \c std::size_t, \c std::ptrdiff_t, <c>void*</c>, #"alib::integer;2", and
  #"alib::uinteger;2", which all should be the same, but independent on the compiler and platform
  (processor type).
  Usually, this configuration macro is deduced (in the internal header #"F;lang/integers.hpp;2").
  If this deduction fails, this may result in two possible outcomes:
  1. A dedicated compilation error hinting to this documentation.
  2. Less directly connected compilation errors.

  In this case, the right value has to be provided to the compiler. When doing this, configuration macros
  - #"ALIB_INTGAP_TYPE",
  - #"ALIB_SIZEOF_INTGAP",
  - #"ALIB_SIZEOF_LONGDOUBLE_REPORTED", and
  - #"ALIB_SIZEOF_LONGDOUBLE_WRITTEN"

  have to be provided as well. This is true for each of these configuration macros: 
  If one of them is given explicitly to the compiler, all the five have to be given.
  Otherwise, an explicit error message will be given during compilation.

\def ALIB_INTGAP_TYPE
  The type of #"lang::intGap_t".
  Usually, this configuration macro is deduced (in the internal header #"F;lang/integers.hpp;2")
  depending on the compiler and platform.
  @see If platform-dependent deduction fails, see documentation of #"ALIB_SIZEOF_INTEGER" for
       information about providing configuration macros.

\def ALIB_SIZEOF_INTGAP
  Contains the size of integral types #"lang::intGap_t" and #"lang::uintGap_t".<br>
  Usually, this configuration macro is deduced (in the internal header #"F;lang/integers.hpp;2")
  depending on the compiler and platform.

  @see If platform-dependent deduction fails, see documentation of #"ALIB_SIZEOF_INTEGER" for
       information about providing configuration macros.

\def ALIB_SIZEOF_LONGDOUBLE_REPORTED
  Contains the size of <c>long double</c>. This is the value that <c>sizeof(long double)</c>
  returns. This size might be higher to what a copy operation of a value of the type might
  write.
  Usually, this configuration macro is deduced (in the internal header #"F;lang/integers.hpp;2")
  depending on the compiler and platform.

  @see If platform-dependent deduction fails, see documentation of #"ALIB_SIZEOF_INTEGER" for
       information about providing configuration macros.<br>

  @see Sibling configuration macro #"ALIB_SIZEOF_LONGDOUBLE_WRITTEN".

\def ALIB_SIZEOF_LONGDOUBLE_WRITTEN
  Configuration macro which denotes how many bytes the hardware will write with a value of type 
  <c>long double</c>.
  This size might be smaller than the amount of memory that an allocation of that type would consume,
  hence what is reported by <c>sizeof(long double)</c>.<br>
  Usually, this configuration macro is deduced (in the internal header #"F;lang/integers.hpp;2")
  depending on the compiler and platform.

  @see If platform-dependent deduction fails, see documentation of #"ALIB_SIZEOF_INTEGER" for
       information about providing this configuration macro together with four others.<br>

  @see The value of this configuration macro is used to define the specialization of
       #"SizeTraits" for template type <c>long double</c>.


\I{################################################################################################}
\I{############################           Module Selection          #############################  }
\I{################################################################################################}
\def ALIB_ALOX
  Denotes if module \alib_alox is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_ALOX" and #"IFNOT_ALIB_ALOX" which are useful
    for pruning short code snippets.

\def ALIB_BITBUFFER
  Denotes if module \alib_bitbuffer is included in the \alibbuild.

\def ALIB_APP
  Denotes if module \alib_app is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_APP" and #"IFNOT_ALIB_APP" which are useful
    for pruning short code snippets.

\def ALIB_BOXING
  Denotes if module \alib_boxing is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_BOXING" and #"IFNOT_ALIB_BOXING" which are useful
    for pruning short code snippets.

\def ALIB_CAMP
  Denotes if module \alib_camp is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_CAMP" and #"IFNOT_ALIB_CAMP" which are useful
    for pruning short code snippets.

\def ALIB_FORMAT
  Denotes if module \alib_format is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_FORMAT" and #"IFNOT_ALIB_FORMAT" which are useful
    for pruning short code snippets.

\def ALIB_SYSTEM
  Denotes if module \alib_system is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_SYSTEM" and #"IFNOT_ALIB_SYSTEM" which are useful
    for pruning short code snippets.
                                     
\def ALIB_EXCEPTIONS
  Denotes if module \alib_exceptions is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_EXCEPTIONS" and #"IFNOT_ALIB_EXCEPTIONS" which are useful
    for pruning short code snippets.

\def ALIB_RESOURCES
  Denotes if module \alib_resources is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_RESOURCES" and #"IFNOT_ALIB_RESOURCES" which are useful
    for pruning short code snippets.

\def ALIB_VARIABLES
  Denotes if module \alib_variables is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_VARIABLES" and #"IFNOT_ALIB_VARIABLES" which are useful
    for pruning short code snippets.

\def ALIB_CONTAINERS
  Denotes if module \alib_containers is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_CONTAINERS" and #"IFNOT_ALIB_CONTAINERS" which are useful
    for pruning short code snippets.

\def ALIB_ENUMRECORDS
  Denotes if module \alib_enumrecords is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_ENUMRECORDS" and #"IFNOT_ALIB_ENUMRECORDS" which are useful
    for pruning short code snippets.

\def ALIB_EXPRESSIONS
  Denotes if module \alib_expressions is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_EXPRESSIONS" and #"IFNOT_ALIB_EXPRESSIONS" which are useful
    for pruning short code snippets.

\def ALIB_FILETREE
  Denotes if module \alib_filetree is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_FILETREE" and #"IFNOT_ALIB_FILETREE" which are useful
    for pruning short code snippets.

\def ALIB_MONOMEM
  Denotes if module \alib_monomem is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_MONOMEM" and #"IFNOT_ALIB_MONOMEM" which are useful
    for pruning short code snippets.

\def ALIB_SINGLETONS
  Denotes if module \alib_singletons is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_SINGLETONS" and #"IFNOT_ALIB_SINGLETONS" which are useful
    for pruning short code snippets.

\def ALIB_STRINGS
  Denotes if module \alib_strings is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_STRINGS" and #"IFNOT_ALIB_STRINGS" which are useful
    for pruning short code snippets.

\def ALIB_THREADMODEL
  Denotes if module \alib_threadmodel is included in the \alibbuild.
  \see
    Corresponding macros #"IF_ALIB_THREADMODEL" and #"IFNOT_ALIB_THREADMODEL" which are useful
    for pruning short code snippets.

\I{################################################################################################}
\I{###################################       BitBuffer       ######################################}
\I{################################################################################################}
\def ALIB_DEBUG_ARRAY_COMPRESSION
  Selects code that reads back integral arrays after compression with class
  #"ac_v1::ArrayCompressor" to check compression/decompression algorithms.<br>
  This configuration macro defaults to \b true in debug-builds of the library.

\I{################################################################################################}
\I{###################################         Boxing        ####################################  }
\I{################################################################################################}

\def ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
  If \c 0 (the default), selects code for boxing any C++ integral type to #"lang::integer",
  respectively #"lang::uinteger".

  If passed as \c 1, bijective boxing is enabled, what boxes each type "as is".

  \see
    Documentation of namespace #"alib_boxing_fundamentals"
    for more information.

\def ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
  If \c 0 (the default), selects code for boxing types \c char, \c wchar_t, \c char16_t and \c char32_t as a
  #"characters::character" and disallowing to unbox any other.

  If passed as \c 1, bijective boxing is enabled, what boxes each type as it is.

  \see
    Documentation of namespace #"alib_boxing_fundamentals"
    for more information.

\def ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
  If \c 0(the default), selects code for boxing type \c float as a \c double value and disallowing to unbox
  \c float.

  If passed as \c 1, bijective boxing is enabled, what boxes type \c float as it is.

  \see
    Documentation of namespace #"alib_boxing_fundamentals"
    for more information.

\def ALIB_DEBUG_BOXING
  Selects extended debug code in module \alib_boxing.<br>
  If given as \c true in release-builds, the configuration macro is redefined to \c false, and a compiler
  pragma-warning is given.

  Details on the features enabled by this configuration macro are given in chapter
  #"alib_boxing_more_debug_general" of the Programmer's Manual of module \alib_boxing_nl.

\I{################################################################################################}
\I{###################################       Characters      ####################################  }
\I{################################################################################################}

\def ALIB_CHARACTERS_WIDE
  If \c false, type #"characters::character" is equivalent to C++ built-in type \c char.
  Otherwise, #"characters::character" equals the multi-byte character type #"characters::wchar",
  which in turn is equivalent to either of \c wchar_t, \c char16_t or \c char32_t.

  Note, that the use of this configuration macro to select code should be needed very seldom.
  Instead, it is advised to use "logical character" types and other provided helpers and macros
  that convert characters and string-types transparently.

  If the configuration macro is not externally set (passed to the compiler), the choice of the default character
  type is system-dependent:
  On Windows OS, wide character strings are chosen, while on Unix-like OSes, narrow character
  strings are used by default.

  \see
    Chapters #"alib_characters_prepro_literals" and chapter
    #"alib_characters_chars" of the Programmer's Manual of module
    \alib_characters.


\def ALIB_CHARACTERS_NATIVE_WCHAR
  If \c true, type #"characters::wchar" is equivalent to C++ built-in type \c wchar_t.
  Otherwise, #"characters::wchar" has a different width than \c wchar_t and equals
  to either \c char16_t or \c char32_t: If a compiler's defines \c wchar_t as a 2-byte integral,
  \c char32_t is chosen and \c char16_t if it is a 4-byte integral.

  This configuration macro depends on operating system and compiler defaults and may be manipulated by passing
  configuration macro #"ALIB_CHARACTERS_SIZEOF_WCHAR" with library compilation.

  Note, that the use of this configuration macro to select code should be needed very seldom.
  Instead, it is advised to use "logical character" types and other provided helpers and macros
  that convert characters and string-types transparently.

  \see
     Chapter #"alib_characters_prepro_literals" of the
     Programmer's Manual of module \alib_characters.


\def ALIB_CHARACTERS_SIZEOF_WCHAR
  Provides the width of \alib type #"characters::wchar" in bytes.

  If not set, a compiler/platform-dependent default value is chosen.

  If passed to the compiler, then type #"characters::wchar" will be set independent of the
  width of \c wchar_t, which is compiler dependent and the configuration macro 
  ALIB_CHARACTERS_NATIVE_WCHAR will be set accordingly.

  If a value of \c 2 is given, then type #"characters::wchar" will be \b 2 bytes wide.
  Precisely, \b wchar will be equivalent to type \c wchar_t with compilers that define
  the latter as a \b 2-byte integral value and equivalent to type \c char16_t, if \c wchar_t is a
  \b 4-byte integral.

  If a value of \c 2 is given, then type #"characters::wchar" will be \b 4 bytes wide.
  Precisely, \b wchar will be equivalent to type \c wchar_t with compilers that define
  the latter as a \b 4-byte integral value and equivalent to type \c char32_t, if \c wchar_t is a
  \b 2-byte integral.

  Other values are not allowed.

  \see
     Chapter #"alib_characters_prepro_literals" of the
     Programmer's Manual of module \alib_characters.

\I{################################################################################################}
\I{###################################      Containers       ######################################}
\I{################################################################################################}

\def ALIB_DEBUG_CONTAINERS
  Selects extended debug code in module \alib_containers.<br>
  If given as \c true in release-builds, the configuration macro is redefined to \c false, and a compiler
  pragma-warning is given.

  Please consult chapter #"alib_contmono_further_debug" of the Programmer's Manual of module
  \alib_containers for further information about the debug features that come available with
  this configuration macro.


\I{################################################################################################}
\I{###################################         Monomem       ######################################}
\I{################################################################################################}

\def ALIB_DEBUG_MEMORY
  Selects extended debug code in module \alib_monomem.<br>
  If given as \c true in release-builds, the configuration macro is redefined to \c false, and a compiler
  pragma-warning is given.

  Please consult chapter #"alib_contmono_further_debug" of the Programmer's Manual of camp
  \alib_monomem for further information about the debug features that come available with
  this configuration macro.

  \note Setting this configuration macro requires the availability of \alib_alox in the \alibbuild.
  
  @see Configuration macro #"ALIB_DEBUG_ALLOCATIONS".
  
\def ALIB_DEBUG_ALLOCATIONS
  Maeks all #"lang::Allocator;ALib allocators" to surround allocations with magic bytes.
  If given as \c true in release-builds, the configuration macro is redefined to \c false, and a compiler
  pragma-warning is given.

  Please consult chapter #"alib_contmono_further_debug" of the Programmer's Manual of camp
  \alib_monomem for further information about the debug features that come available with
  this configuration macro.

  \note Setting this configuration macro requires the availability of \alib_alox in the \alibbuild.
  
  @see Configuration macro #"ALIB_DEBUG_MEMORY".
  
\def ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT
  This configuration macro defaults to <c>"alignof(uint64_t)"</c> and is used as the default value
  of template parameter \p{TAlignment} of class #"TPoolAllocator".
  The importance of this default value arises from the fact that the alias type definition
  #"alib::PoolAllocator;2" uses this default-alignment and this alias is in turn used across
  \alib.
  In case this value needs to be changed, please note that the using code linked against
  a compiled library needs to have this configuration macro set accordingly.
  Otherwise, undefined behavior may occur.
  
  @see Chapter #"alib_contmono_further_alignment" of the Programmer's Manual of module
       \alib_monomem.
            
\I{################################################################################################}
\I{#################################          Singletons         ################################  }
\I{################################################################################################}

\def ALIB_FEAT_SINGLETON_MAPPED
  Affects implementation of class
  #"Singleton". If this configuration macro represents \c true, then
  a static hash map is used to find the singleton objects, even if different data segments
  exist within one software process.<br>
  While the overhead imposed by such implementation is quite small, with this configuration macro being
  \c false, the overhead is omitted in scenarios when such effort is not necessary.

  The configuration macro defaults to \c true on the windows platform, otherwise to \c false, which may
  be overwritten by passing the configuration macro to the compiler.


\I{################################################################################################}
\I{#################################          Strings         ###################################  }
\I{################################################################################################}

\def ALIB_FEAT_BOOST_REGEX
  Selects code for class #"util::RegexMatcher" and features within \alib
  that uses this wrapper class.

  If this configuration macro is not explicitly given to the compiler, it is set to \c 0,
  the \b %boost library is not included and class \b %RegexMatcher is not available.

  If using the #"alib_manual_build_cmake_3;corresponding CMake variable", the script
  #"^F;ALib.cmake" will search and add \b boost_regex to \e CMake variable
  \b ALIB_EXTERNAL_LIBS.

  Please note that this script also performs

        set(Boost_USE_STATIC_LIBS   ON)

  before searching the library.


\def ALIB_DEBUG_STRINGS
  Selects extended debug code, mostly within class #"^AString".<br>
  If given as \c true in release-builds, the configuration macro is redefined to \c false, and a compiler
  pragma-warning is given.

  In particular, the configuration macro enables internal consistency checks as described in chapter
  #"alib_strings_details_debugging" of the Programmer's Manual of module \alib_strings_nl.
  Internally, macro #"ALIB_STRING_DBG_CHK" is used and users of the library are free (recommended)
  to use the ame for custom consistency checks when manipulating \b %AString buffers directly.

\I{################################################################################################}
\I{###################################       Threads         ####################################  }
\I{################################################################################################}

\def ALIB_DEBUG_CRITICAL_SECTIONS
  This configuration macro - while associated to module \alib_threads - activates type
  #"lang::DbgCriticalSections", which is available independent of the inclusion of
  this module. However, the configuration macro itself will always be disabled (set to
  \c 0) if module \alib_threads_nl is not included in the \alibbuild. This design allows using
  the corresponding macros anywhere in the code, without further checks. In the absence of
  \alib_threads_nl, all macros will be just empty. The same is true for type
  \b DbgCriticalSections, which will be completely empty and optimized out.
  
 @see Macros alib_macros_mod_threads

\I{################################################################################################}
\I{################################          Basecamp         ####################################}
\I{################################################################################################}

\def ALIB_CAMP_OMIT_DEFAULT_RESOURCES
  If set, code that defines static default resource strings is pruned, and thus, all
  static resource string data is pruned from the library's executable's data segment.

  The omission occurs in the implementations of #"Camp::Bootstrap;*" of various \alibmods,
  where methods #"ResourcePool::Bootstrap;*" and
  #"ResourcePool::BootstrapBulk;*" are used to define the default resources.

  If resources are omitted, the bootstrap process has to be customized to use an external
  resource provider. Otherwise the library has undefined behavior as resources are missing.

  \see
    Chapter #"alib_resources_details_export" of the
    Programmer's Manual of module \alib_resources for more information.


\def ALIB_DEBUG_RESOURCES
  Selects extended debug code in module \alib_resources.<br>
  If given as \c true in release-builds, the configuration macro is redefined to \c false, and a compiler
  pragma-warning is given.

  In particular, the following features become enabled:
  - Static field #"LocalResourcePool::DbgResourceLoadObserver" becomes available
    which supports finding resource definition errors. If set before bootstrapping,
    methods #"LocalResourcePool::BootstrapBulk;*" and #"LocalResourcePool::BootstrapAddOrReplace;*" 
    will write information on bulk and singular resource data definitions.
  - Type #"LocalResourcePool" collects statistics about the number of acquisitions of particular 
    resources. These statistics can be read by directly accessing the internal hash table using 
    the method #"LocalResourcePool::BootstrapGetInternalHashMap"
    or using one of the convenient functions below.
  - Method #"ResourcePool::DbgGetList;*"
  - Method #"ResourcePool::DbgGetCategories;*"
  - Function #"resources::DbgDump"
  - Method #"LocalResourcePool::DbgGetList;*"
 
\def ALIB_PATH_CHARACTERS_WIDE
  This configuration macro is set to \c 1 if the character type defined by the C++ standard
  library with type <c>std::filesystem::path::value_type</c> is <c>wchar_t</c>.
  Otherwise, it is set to \c 0.<br>
  The configuration macro is set in #"F;ALib.System.H" but can only be "guessed".
  The guess is that only if the configuration macro <c>_MSC_VER</c> is set, wide characters are used.
  The header raises a static assertion error if the macro is wrongly guessed or externally set.
  
  Should the guess be wrong, the right value has to be passed with compilation of the library.
                                       
\I{################################################################################################}
\I{###################################        System          ####################################  }
\I{################################################################################################}

\def ALIB_SYSTEM_FORCE_STD_FILE_STATUS
  If given, platform-specific code for scanning file trees and probing a file's satus is ignored 
  and instead the overloaded function #"ScanFiles(FTree&)", respectively methods 
  #"FileStatus::Update(Path)" are using an implementation based on C++ library 
  <c>std::filesystem</c>.
  This flag is used during development of \alib to test the standard implementation and should
  not be set otherwise, because platform-specific implementations are more feature reach and better
  performing.

\def ALIB_SYSTEM_FILE_STATUS_IMPL
  This configuration macro is evaluated internally with the inclusion of #"F;ALib.FileTree.H".
  It allows determining the implementation of the file scanner used.
  As of today, the following values are possible:<br>
  - #"ALIB_SYSTEM_FILE_STD_STATUS"
  - #"ALIB_SYSTEM_FILE_POSIX_STATUS"

  The configuration macro must not be set from outside.
  To manipulate the choice, use configuration macro #"ALIB_SYSTEM_FORCE_STD_FILE_STATUS" instead.
    
\def ALIB_SYSTEM_FILE_STD_STATUS
  A possible value of #"ALIB_SYSTEM_FILE_STATUS_IMPL", evaluating to \b 1, and denoting the
  implementation using C++ library <c>std::filesystem</c>.<br>
  Must not be set from outside.
   
\def ALIB_SYSTEM_FILE_POSIX_STATUS
  A possible value of #"ALIB_SYSTEM_FILE_STATUS_IMPL", evaluating to \b 2, and denoting the
  implementation using the Posix library calls.<br>
  Must not be set from outside.

\def ALIB_POSIX_MAPPED_FILES
  This is directly derived from the posix variable \b _POSIX_MAPPED_FILES and thus determines if
  mapped files are supported by the operating system. This is never true on Windows OS and true
  with most other operating systems.

\I{################################################################################################}
\I{###################################          ALox         ####################################  }
\I{################################################################################################}

This group of code selection macros is defined with the use of module \alib_alox and
used to select the inclusion of log code. The macros are seldom to be used by end-user code.
Instead, the macros used for the log statements themselves (see
\ref alib_macros_mod_alox_debug "debug logging macros" and
\ref alib_macros_mod_alox_release "release logging macros".) are variably defined based on these macros.

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
  #"alib_mod_alox_debug_and_release_logging_ft_1".)<br>
  This is useful for example, to conditionally compile code that calculates and gathers
  information to do some more complex log output.


\def ALOX_DBG_LOG_CI
  If defined, scope information is passed to \alox on invocation of debug
  <em>Log Statements</em>.
  Hence, users of \alox can conditionally compile special debug logging code that normally
  belongs to corresponding debug logging statements based on this configuration macro. A sample would be
  the definition of different log line meta-information formats, depending on the availability
  of scope information.

  \note The \alox <em>Scope Domain</em> mechanism as well as ALox features provided with
        #"Lox::Once(const NString&, Verbosity, const Box&, const String&, Scope, int);Lox::Once" and #"Lox::Store(const Box&, Scope);Lox::Store" rely on caller information.
        These mechanisms are therefore not available to debug logging unless this configuration macro is set.

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
        #"Lox::Once(const NString&, Verbosity, const Box&, const String&, Scope, int);Lox::Once" and #"Lox::Store(const Box&, Scope);Lox::Store" rely on caller information.
        These mechanisms are therefore not available to release logging unless this configuration macro is set.


\I{################################################################################################}
\I{################################################################################################}
\I{##########################                 MACROS               ################################}
\I{################################################################################################}
\I{################################################################################################}

\I{################################################################################################}
\def ALIB_VERSION
  The \alib version number.
  The value of this macro is stored in namespace variable #"VERSION;2".

  \see Programmer's manual chapter #"alib_mod_bs_assertcomp".

\def ALIB_REVISION
  The \alib revision number.
  The value of this macro is stored in namespace variable #"REVISION;2".

  \see Programmer's manual chapter #"alib_mod_bs_assertcomp".

\def ALIB_COMPILATION_FLAGS
  Macro to provide a compilation flag verifier value to be used with the method
  #"AssertALibVersionAndFlags;2" which is automatically invoked with function
  #"alib::Bootstrap(BootstrapPhases);2" (and overloads).
  The value of this macro is stored in namespace variable #"COMPILATION_FLAGS;2".

  \see Programmer's manual chapter #"alib_mod_bs_assertcomp".

\def ALIB_ASSERT_MODULE
  Asserts if a given module is included in the \alibbuild.
  @param modulename   The name of the module to assert as available.

\def ALIB_BASE_DIR
  String containing the source folder of \alib. Used with unit tests.

\I{################################################################################################}
 \def ALIB_STRINGIFY
   Makes as string from a preprocessor macro parameter. The string character type equals the
   default character type #"characters::character".

   @param a   The token to stringyfy.


 \def ALIB_NSTRINGIFY
   Makes as narrow string from a preprocessor macro parameter.

   @param a   The token to stringyfy.


 \def ALIB_CONCAT
   Concatenates two preprocessor macro parameters into one symbol.

   @param a   The first part of the concatenated code.
   @param b   The second part of the concatenated code.

 \def ALIB_IDENTIFIER
   This macro assembles an 'anonymous' identifier using the given prefix and the current
   line number within the source code file. This macro is used within other macros that
   need a C/C++ identifier internally.<br>
   As a sample, see macro #"ALIB_OWN".

   @param prefix  A prefix token to use.

 \def ALIB_EMPTY
   Defines an empty macro. This is useful, for example, if a macro defined variadic arguments
   (<c>'...'</c>) and a user wants to omit to provide a value. Some compilers (preprocessors)
   might warn about empty variadic arguments, if nothing was given. This macro avoids this
   and increased the readability.


 \def ALIB_COMMA
   Defines a simple comma (<c>','</c>). Used with macros when otherwise the use of a comma symbol
   leads to ambiguous syntax. For example, when passing template types to macros.

   Used with macros when otherwise the use of a comma symbol
   leads to ambiguous syntax. For example, when passing template types to macros as shown here:

           MY_MACRO( std::vector<int ALIB_COMMA double> )


 \def ALIB_COMMA_DBG
   Defines a simple comma (<c>','</c>) with #"ALIB_DEBUG;debug-builds". In release
   compilations the macro is empty. This is useful, for example, when methods change their
   signature depending on the compilation type. The latter is unavoidable i.e if caller source code
   information should be passed for debugging.

   Used with macros when otherwise the use of a comma symbol
   leads to ambiguous syntax. For example, when passing template types to macros.

 \def ALIB_STATIC_ASSERT
   A simple macro that wraps language keyword \c static_assert. Common compilers display
   the condition expression that failed with the compilation error. The purpose of this
   macro is to hide this expression away. For this, the expression is assigned to a \c constexpr
   boolean value first, and then the assertion uses only this variable as its expression.

   The variable name is given with parameter \p{CondVariable} and
   @param CondVariable This should be a "speaking name" that tells the user right away what
                       happened.
   @param Cond         The condition that is asserted and hid away from the compiler's message text.
   @param Message      The message that details what is said with \p{CondVariable}.

 \def ALIB_STATIC_DENY
   See macro #"ALIB_STATIC_ASSERT" for a general explanation. The only difference from this is
   that the negation of the given expression is used. Hence the opposite of an assert, a "denial".
   @param CondVariable This should be a "speaking name" that tells the user right away what
                       happened.
   @param Cond         The condition that is asserted to be \c false, and that is hid away from the
                       compiler's message text.
   @param Message      The message that details what is said with \p{CondVariable}.
\I{################################################################################################}
\def ALIB_DLL
  Used to export/import C++ symbols into a dynamic link library.
  Defined under Windows/MSC when compiling or using \alib classes inside a DLL.
  Dependent on #"ALIB_API_IS_DLL" and #"ALIB_API_NO_DLL".
                                      
\def ALIB_EXPORT
  This is empty, as \alib currently does not provide support for C++20-Module compilation.
  @see Our #"alib_c20module_shift;blog page" on that topic.

\def ALIB_ASSERT_GLOBAL_NAMESPACE
  If this macro is placed outside the global namespace, a <c>static_assert</c> is raised at
  compile time.

\def ALIB_SIZEOF_WCHAR_T
  Compiler/platform-dependent value. Gives the sizeof values of type \c wchar_t in bytes.
  Possible values are \b 2 and \b 4.

\def ALIB_CALLER_FUNC
  Used by macro #"ALIB_CALLER" to retrieve the compiler-specific preprocessor variable
  of the current function name. With MSVC evaluates to \b __FUNCTION__,
  with other compilers to \b __func__.

\def ALIB_CALLER
  This macro fills in the built-in configuration macros that provide the current source file,
  line number and function strings.<p>
  The macro should be used anywhere where this information is passed (as a nested macro in
  debug macros)
  @see Chapter #"alib_manual_appendix_callerinfo" of the General Programmer's Manual.

\def ALIB_CALLER_PRUNED
  This macro is the same as #"ALIB_CALLER" but empty if #"ALIB_DEBUG" is
  not defined.<br>
  In comparison to #"ALIB_CALLER_NULLED", this macro is to be used with invocations of
  methods that lack the corresponding parameters in release compilations.

\def ALIB_COMMA_CALLER_PRUNED
  Same as #"ALIB_CALLER_PRUNED", but prepends a <c>','</c>
  This is needed if the caller information is not the only, but the last argument in a call.
  
\def ALIB_CALLER_PRUNED_COMMA
  Same as #"ALIB_CALLER_PRUNED", but adds a <c>','</c>
  This is needed if the caller information is followed by (non-debug) arguments in a call.
  The latter is usually avoided but necessary when default values for the next argument should
  be given.

\def ALIB_CALLER_NULLED
  In #"ALIB_DEBUG;debug-builds" this macro is the same as #"ALIB_CALLER".
  Otherwise evaluates to <c>{nullptr,0,nullptr,::std::thread::id(), nullptr}</c>.<br>
  In comparison to #"ALIB_CALLER_PRUNED", this macro is to be used with invocations of
  methods that keep the corresponding parameters in release compilations.
  \note If methods keep the parameter, then a user can decide if caller information should be
        included in release-builds or not by either using #"ALIB_CALLER" or this macro.
                                 
\def ALIB_COMMA_CALLER_NULLED
  Same as #"ALIB_CALLER_NULLED", but prepends a <c>','</c>
  This is needed if the caller information is not the only, but the last argument in a call.
  
\def ALIB_CALLER_NULLED_COMMA
  Same as #"ALIB_CALLER_NULLED", but adds a <c>','</c>
  This is needed if the caller information is followed by (non-debug) arguments in a call.
  The latter is usually avoided but necessary when default values for the next argument should
  be given.
  
\def ALIB_DBG_TAKE_CI
  Used with method declarations to define parameter \p{ci} of type #"CallerInfo".
  

\def bitsof(type)
  Like C++ keyword <c>sizeof</c> but returns the number of bits of the type of the given value.
  The return type is <c>int</c> instead of <c>size_t</c>, which satisfies \alib code conventions.

  \note To improve code readability, namely to a) indicate that this is an inlined, constant
        expression and b) to indicate that this is just using keyword <c>sizeof</c>,
        as an exception from the naming rules, this function is spelled in lower case.

  @see Function #"alib::lang::bitsofval(const" T&)
  
\def ALIB_STACK_ALLOCATED_TYPE
  To be placed at the beginning of a type which is to be allocated exclusively on the stack.
  For this, its <c>new-operators</c> as well as copy and move operators are declared private.
  @param T The name of the type that this macro is placed in.
\I{################################################################################################}
\def ALIB_POP_ALLOWANCE
 Resets compiler warning settings to the state before one of the other macros of this section
 had been placed. Every such placement should have a corresponding placement of this
 macro.


\def ALIB_ALLOW_IMPLEMENTATION_INCLUSION
 Switches of compiler warnings about "header hygiene" (i.e with clang this sets "-Wheader-hygiene").
 \alib needs this when including compilation units. This is done in C++20-Module compilation
 in files of type <c>*.mod.cpp</c> and in standard compilation in files of type <c>*.uni.cpp</c>
 where a compile guesses that a header is included, while it is not.   

\def ALIB_ALLOW_UNINITIALIZED
 Preprocessor macro to disable compiler warnings about uninitialized values.

\def ALIB_ALLOW_NULL_POINTER_PASSING
 Preprocessor macro to disable GCC warning "nonnull". 
 Used, for example, to suppress warnings related to \c constexpr conditional programming.
                                            
\def ALIB_ALLOW_UNREACHABLE_CODE
 Preprocessor macro to disable compiler warnings about detected unreachable code.
 (i.e with clang this ignores "-Wunreachable-code")

\def ALIB_ALLOW_UNUSED_MACRO
 Preprocessor macro to disable compiler warnings about unused preprocessor macros.

\def ALIB_ALLOW_MACROS_WITH_RESERVED_NAME
 Preprocessor macro to disable compiler warnings about reserved macro names.
 (i.e with clang this ignores "-Wreserved-macro-identifier")

\def ALIB_ALLOW_MACRO_REDEFINITION
 Preprocessor macro to disable compiler warnings on redefining macros without prior un-defining.

\def ALIB_ALLOW_MISSING_FIELD_INITIALIZERS
 Preprocessor macro to disable compiler warnings on missing field initializers.

\def ALIB_ALLOW_UNSAFE_FUNCTION_OR_VARIABLE
 Preprocessor macro to disable compiler warnings for use of 'unsafe' (deprecated) library
 functions. Currently used with MSVC 4996 only.

\def ALIB_ALLOW_OVERLOADED_VIRTUALS
 Preprocessor macro to disable compiler warnings about virtual methods that become hidden
 by overloaded methods with a different signature.

\def ALIB_ALLOW_SPARSE_ENUM_SWITCH
 Preprocessor macro to disable compiler warnings when an enumeration element is switched while
 not all enumeration elements get caught.

\def ALIB_ALLOW_BITWISE_SWITCH
 Preprocessor macro to disable compiler warnings when a
 "bitwise type scoped enumeration" (see #"BitwiseTraits") or similar types with
 'sparse' case coverage are used in a switch statement.

\def ALIB_ALLOW_SWITCH_WITHOUT_DEFAULT
 A typical use case is a switch over a value of an enum type, where all enumerators are handled 
 explicitly. In such cases, the compiler should still warn about newly added, unhandled enum values, 
 but it should not complain about the absence of a default label or about hypothetical enumerators 
 artificially created by a software. Such artificial enumeration elements (created, for example,
 by a code like <c>MyEnum(42)</c>) should only be introduced in rare circumstances.

\def ALIB_ALLOW_DOCS
 Preprocessor macro to disable compiler warnings according to source documentation (as of today
 known to be thrown by \b clang only).

\def ALIB_ALLOW_SHIFT_COUNT_OVERFLOW
 Preprocessor macro to disable compiler warnings concerning overflows during bit-shift operations.
     
\def ALIB_ALLOW_DEPRECATED
 Preprocessor macro to disable compiler warnings concerning the use of deprecated functions and
 types.

\def ALIB_ALLOW_UNUSED_MACRO
 Preprocessor macro to disable compiler warnings concerning unused macro definitions.

\def ALIB_ALLOW_UNUSED_PARAMETER
 Preprocessor macro to disable compiler warnings concerning unused parameters.
 (This macro is only used with unit testing and documentation sample code.)

\def ALIB_ALLOW_UNUSED_VARIABLE
 Preprocessor macro to disable compiler warnings concerning unused parameters.
(This macro is only used with unit testing and documentation sample code.)

\def ALIB_ALLOW_UNUSED_FUNCTION
 Preprocessor macro to disable compiler warnings concerning unused (member-) functions.
 (This macro is only used with unit testing and documentation sample code.)

\def ALIB_ALLOW_UNUSED_LAMBDA_CAPTURE
 Preprocessor macro to disable compiler warnings concerning unused (member-) functions.
 (Needed, because it can happen that gcc needs a \c this captured, while clang complains
 it was unnecessary.)
 
\def ALIB_ALLOW_FUNCTION_TEMPLATE
 Preprocessor macro to disable compiler warnings concerning unused function templates.

\def ALIB_ALLOW_RESERVED_IDENTIFIER
 Preprocessor macro to disable compiler warnings concerning conflicting identifier names.
 Usually identifiers with double underscore (<c>'_'</c>) are forbidden, but with some
 preprocessor macros, those are not easily avoidable.

\def ALIB_ALLOW_NOT_ELIDING_COPY_ON_RETURN
 Preprocessor macro to disable compiler warnings concerning non-optimizable return values.
(Clang <c>"-Wnrvo"</c>)

\def ALIB_ALLOW_NOTHING_RETURNED
 Preprocessor macro to disable compiler warnings concerning non-void functions that may
 not return a value. Needed when compiler cannot properly detect the inherent safeness (correctness)
 of a function that always is returning a type.

\def ALIB_FALLTHROUGH
 Used with keyword \c switch to annotate a \c case block that has no \b break or (\c return, etc.).
 Avoids a warning with some compilers.

\def ALIB_ALLOW_INTEGRAL_CONSTANT_OVERFLOW
 Preprocessor macro to disable compiler warnings about code that may cause an overflow of an
 integral constant.

\def ALIB_ALLOW_SIGN_CONVERSION
 Preprocessor macro to disable compiler warnings about code that changes signedness of integral
 values.

\def ALIB_ALLOW_INTEGER_OVERFLOW
 Preprocessor macro to disable compiler warnings about code that may cause an overflow of an
 integral value, e.g., with bit-shift operations.
\I{################################################################################################}
\def ALIB_ERROR
  Writes the given message objects as an error.

  @param domain The domain of the assertion. (Usually the \alibmod_nl.)
  @param ...    The objects used to format the message string.

\def ALIB_WARNING
  Writes the given message objects as an warning.

  @param domain The domain of the assertion. (Usually the \alibmod_nl.)
  @param ...   The objects used to format the message string.

\def ALIB_MESSAGE
  Writes the given message.

  @param domain The domain of the assertion. (Usually the \alibmod_nl.)
  @param ...   The objects used to format the message string.

\def ALIB_ASSERT
  If given condition is \c false, error message "Assertion Failed" is written.

  @param cond  The condition assert.
  @param domain The domain of the assertion. (Usually the \alibmod_nl.)

\def ALIB_ASSERT_ERROR
  If given condition is \c false, given message objects are written as an error.

  @param cond  The condition assert.
  @param domain The domain of the assertion. (Usually the \alibmod_nl.)
  @param ...   The objects used to format the message string.

\def ALIB_ASSERT_WARNING
  If given condition is \c false, given message objects are written as a warning.

  @param cond  The condition assert.
  @param domain The domain of the assertion. (Usually the \alibmod_nl.)
  @param ...   The objects used to format the message string.

\def ALIB_ASSERT_MESSAGE
  If given condition is \c false, given message objects are written as a message.

  @param cond  The condition assert.
  @param domain The domain of the assertion. (Usually the \alibmod_nl.)
  @param ...   The objects used to format the message string.

\def ALIB_ASSERT_RESULT_EQUALS
  Asserts that a return value of a function call equals the given expected value.
  In release compilation, the function is still invoked, but no check is performed.

  @param func   The function to invoke.
  @param value  The value to test for.

\def ALIB_ASSERT_RESULT_NOT_EQUALS
  Asserts that a return value of a function call is not equal to the given value.
  In release compilation, the function is still invoked, but no check is performed.

  @param func   The function to invoke.
  @param value  The value to test for.

\def ALIB_ASSERT_RESULT_GREATER_THAN
  Asserts that a return value of a function call is greater than the given value.
  In release compilation, the function is still invoked, but no check is performed.

  @param func   The function to invoke.
  @param value  The value to test for.

\def ALIB_ASSERT_RESULT_LESS_THAN
  Asserts that a return value of a function call is less than the given expected value.
  In release compilation, the function is still invoked, but no check is performed.

  @param func   The function to invoke.
  @param value  The value to test for.
\I{################################################################################################}
\def ALIB_TVALUE
 Shortcut to nested type modifiers
 <c>std::remove_cv_t</c>, <c>std::remove_pointer_t</c> and <c>std::remove_reference_t</c>.

 @param T     The type that is to be decayed to its plain value-type.

\def ALIB_HAS_METHOD
 Evaluates to \c true if the given type \p{T} has a method called \p{Method} with the given
 signature. Otherwise, evaluates to \c false, respectively causes \b SFINAE.

 @param T      The type to test a method for.
 @param Method The name of the method.
 @param ...    Variadic list of args of the method.
\I{################################################################################################}
\def ALIB_OWN
 This preprocessor macro defines an anonymous instance of type #"lang::Owner".
 The template type is deduced from parameter \p{ownable} using C++ keyword \c decltype.

 By using this macro there is no need to "invent" an (otherwise unreferenced) identifier for
 that definition.
 \note This macro exists mainly because C++ does not support anonymous local instances.

 \see
  - A source-sample on how this macro is used, is given with chapter
    #"alib_threads_locks_auto_owner" of the Programmer's Manual of module \alib_threads.
  - Sibling macros
    - #"ALIB_OWN_RECURSIVE", and
    - #"ALIB_OWN_SHARED", <p>
    which instantiate an object of type #"OwnerRecursive",
    respectively #"OwnerShared", instead of class #"lang::Owner".
  - Macros #"ALIB_LOCK" and #"ALIB_LOCK_WITH" are using this macro to lock
    types #"threads::Lock" and #"RecursiveLock".<br>
  - Macros #"ALIB_DCS" and #"ALIB_DCS_WITH" are using this macro to
    signal entering a #"lang::DbgCriticalSections;critical section".
  - Macro #"ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS" implements a mechanism to detect and assert
    recursive function calls.
  - Chapter #"alib_manual_appendix_callerinfo" of the General Programmer's Manual.

  @param ownable   The \b %Ownable to acquire and release.

\def ALIB_OWN_RECURSIVE
 Same as #"ALIB_OWN", but creates a local anonymous instance of class #"OwnerRecursive".

\def ALIB_OWN_SHARED
 Same as #"ALIB_OWN", but creates a local anonymous instance of class #"OwnerShared".

\def ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS
 This macro may be placed at the beginning of a types' method to detect and assert recursive
 invocations - which by definition must happen.<br>
 The macro internally uses macro #"ALIB_OWN" and hence class #"lang::Owner" that uses C++ stack
 unwinding to be sure to always clear the flag, even if an exception occurs.

 In the case that the macro is placed in more than one method, it can be used to ensure that none
 of the methods calls any of the others.

 A prerequisite of using this macro is to have macro
 #"ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL" placed in the member declaration area
 of the type.

 This macro is only active in debug-compilations.


\def ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL
 Used with macro #"ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS", usually in header compilation unit
 to declare a boolean class member that is used for detecting recursions.
\I{################################################################################################}
\def ALIB_DBG
  This simple macro may be used for placing debug statements into source code. Those get 'pruned'
  in release versions of \alib (respectively of the software that uses \alib and then this macro).
  It replaces
\verbatim
    #if ALIB_DEBUG
        // do stuff
        ...
        ...
    #endif
\endverbatim
   and is useful especially for single line statements.

  @param ...   Source code to prune in release-builds.


\def ALIB_REL_DBG
  Similar to #"ALIB_DBG", but accepts a release version of the code as well. The release
  version is expected as first macro parameter. Note, that the release code must not contain a
  comma (',') while the debug code is allowed to. A comma in release code may be substituted
  with macro #"ALIB_COMMA". However, more complicated stuff should be placed in usual
  <c>\#if</c>/<c>\#else</c>/<c>\#endif</c> statements.

\def ALIB_REL
  As a counterpart to #"ALIB_DBG", this macro may be used for placing code that is only
  available in release compilations. The macro is provided for completeness only and should
  be used seldom and with care, as it generally implies more intense release code testing.
  As of Version 1810, \alib does not use this macro internally.

\def ALIB_CPP_23
  This simple macro may be used for placing code that is only applicable to the C++ language
  standard \c 23 or higher.
  With the lower standard, the code given is pruned.
  @param ... The source to select.
  @see Macro #"ALIB_CPP_BEFORE_23"

 \def ALIB_CPP_BEFORE_23
  This simple macro may be used for placing code that is only applicable to the C++ language
  standard \c 20 or below.
  With the higher standard, the code given is pruned.
  @param ... The source to select.
  @see Macro #"ALIB_CPP_23"


This group of simple macros either include or prune code dependent on the inclusion
of \alibmods_nl in an \alibbuild.

The macros are defined in the internal header #"F;alib.prepro.hpp;2" which always is
the first \alib header-file included.

\def IF_ALIB_ALOX
  Prunes given code \p{...} if \alib_alox is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_ALOX" and sibling macro #"IFNOT_ALIB_ALOX".
  @param ... The source to select.

\def IFNOT_ALIB_ALOX
  Prunes given code \p{...} if \alib_alox is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_ALOX" and sibling macro #"IF_ALIB_ALOX".
  @param ... The source to select.

\def IF_ALIB_APP
  Prunes given code \p{...} if \alib_app is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_APP" and sibling macro #"IFNOT_ALIB_APP".
  @param ... The source to select.

\def IFNOT_ALIB_APP
  Prunes given code \p{...} if \alib_app is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_APP" and sibling macro #"IF_ALIB_APP".
  @param ... The source to select.

\def IF_ALIB_BOXING
  Prunes given code \p{...} if \alib_boxing is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_BOXING" and sibling macro #"IFNOT_ALIB_BOXING".
  @param ... The source to select.

\def IFNOT_ALIB_BOXING
  Prunes given code \p{...} if \alib_boxing is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_BOXING" and sibling macro #"IF_ALIB_BOXING".
  @param ... The source to select.

\def IF_ALIB_BITBUFFER
  Prunes given code \p{...} if \alib_bitbuffer is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_BITBUFFER" and sibling macro #"IFNOT_ALIB_BITBUFFER".
  @param ... The source to select.

\def IFNOT_ALIB_BITBUFFER
  Prunes given code \p{...} if \alib_bitbuffer is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_BITBUFFER" and sibling macro #"IF_ALIB_BITBUFFER".
  @param ... The source to select.

\def IF_ALIB_VARIABLES
  Prunes given code \p{...} if \alib_variables is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_VARIABLES" and sibling macro #"IFNOT_ALIB_VARIABLES".
  @param ... The source to select.

\def IFNOT_ALIB_VARIABLES
  Prunes given code \p{...} if \alib_variables is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_VARIABLES" and sibling macro #"IF_ALIB_VARIABLES".
  @param ... The source to select.

\def IF_ALIB_CONTAINERS
  Prunes given code \p{...} if \alib_containers is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_CONTAINERS" and sibling macro #"IFNOT_ALIB_CONTAINERS".
  @param ... The source to select.

\def IFNOT_ALIB_CONTAINERS
  Prunes given code \p{...} if \alib_containers is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_CONTAINERS" and sibling macro #"IF_ALIB_CONTAINERS".
  @param ... The source to select.

\def IF_ALIB_CAMP
  Prunes given code \p{...} if \alib_resources is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_CAMP" and sibling macro #"IFNOT_ALIB_CAMP".
  @param ... The source to select.

\def IFNOT_ALIB_CAMP
  Prunes given code \p{...} if \alib_resources is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_CAMP" and sibling macro #"IF_ALIB_CAMP".
  @param ... The source to select.

\def IF_ALIB_ENUMRECORDS
  Prunes given code \p{...} if \alib_enumrecords is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_ENUMRECORDS" and sibling macro #"IFNOT_ALIB_ENUMRECORDS".
  @param ... The source to select.

\def IFNOT_ALIB_ENUMRECORDS
  Prunes given code \p{...} if \alib_enumrecords is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_ENUMRECORDS" and sibling macro #"IF_ALIB_ENUMRECORDS".
  @param ... The source to select.

\def IF_ALIB_EXPRESSIONS
  Prunes given code \p{...} if \alib_expressions is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_EXPRESSIONS" and sibling macro #"IFNOT_ALIB_EXPRESSIONS".
  @param ... The source to select.

\def IFNOT_ALIB_EXPRESSIONS
  Prunes given code \p{...} if \alib_expressions is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_EXPRESSIONS" and sibling macro #"IF_ALIB_EXPRESSIONS".
  @param ... The source to select.

\def IF_ALIB_FILETREE
  Prunes given code \p{...} if \alib_filetree is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_FILETREE" and sibling macro #"IFNOT_ALIB_FILETREE".
  @param ... The source to select.

\def IFNOT_ALIB_FILETREE
  Prunes given code \p{...} if \alib_filetree is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_FILETREE" and sibling macro #"IF_ALIB_FILETREE".
  @param ... The source to select.

\def IF_ALIB_THREADMODEL
  Prunes given code \p{...} if \alib_threadmodel is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_THREADMODEL" and sibling macro #"IFNOT_ALIB_THREADMODEL".
  @param ... The source to select.

\def IFNOT_ALIB_THREADMODEL
  Prunes given code \p{...} if \alib_threadmodel is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_THREADMODEL" and sibling macro #"IF_ALIB_THREADMODEL".
  @param ... The source to select.

\def IF_ALIB_THREADS
  Prunes given code \p{...} if \alib_threads is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_SINGLE_THREADED" and sibling macro #"IFNOT_ALIB_THREADS".
  @param ... The source to select.

\def IFNOT_ALIB_THREADS
  Prunes given code \p{...} if \alib_threads is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_SINGLE_THREADED" and sibling macro #"IF_ALIB_THREADS".
  @param ... The source to select.

\def IF_ALIB_STRINGS
  Prunes given code \p{...} if \alib_strings is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_STRINGS" and sibling macro #"IFNOT_ALIB_STRINGS".
  @param ... The source to select.

\def IFNOT_ALIB_STRINGS
  Prunes given code \p{...} if \alib_strings is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_STRINGS" and sibling macro #"IF_ALIB_STRINGS".
  @param ... The source to select.

\def IF_ALIB_MONOMEM
  Prunes given code \p{...} if \alib_monomem is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_MONOMEM" and sibling macro #"IFNOT_ALIB_MONOMEM".
  @param ... The source to select.

\def IFNOT_ALIB_MONOMEM
  Prunes given code \p{...} if \alib_monomem is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_MONOMEM" and sibling macro #"IF_ALIB_MONOMEM".
  @param ... The source to select.

\def IF_ALIB_RESOURCES
  Prunes given code \p{...} if \alib_resources is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_RESOURCES" and sibling macro #"IFNOT_ALIB_RESOURCES".
  @param ... The source to select.

\def IFNOT_ALIB_RESOURCES
  Prunes given code \p{...} if \alib_resources is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_RESOURCES" and sibling macro #"IF_ALIB_RESOURCES".
  @param ... The source to select.

\def IF_ALIB_EXCEPTIONS
  Prunes given code \p{...} if \alib_exceptions is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_EXCEPTIONS" and sibling macro #"IFNOT_ALIB_EXCEPTIONS".
  @param ... The source to select.

\def IFNOT_ALIB_EXCEPTIONS
  Prunes given code \p{...} if \alib_exceptions is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_EXCEPTIONS" and sibling macro #"IF_ALIB_EXCEPTIONS".
  @param ... The source to select.

\def IF_ALIB_SYSTEM
  Prunes given code \p{...} if \alib_system is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_SYSTEM" and sibling macro #"IFNOT_ALIB_SYSTEM".
  @param ... The source to select.

\def IFNOT_ALIB_SYSTEM
  Prunes given code \p{...} if \alib_system is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_SYSTEM" and sibling macro #"IF_ALIB_SYSTEM".
  @param ... The source to select.

\def IF_ALIB_FORMAT
  Prunes given code \p{...} if \alib_format is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_FORMAT" and sibling macro #"IFNOT_ALIB_FORMAT".
  @param ... The source to select.

\def IFNOT_ALIB_FORMAT
  Prunes given code \p{...} if \alib_format is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_FORMAT" and sibling macro #"IF_ALIB_FORMAT".
  @param ... The source to select.

\def IF_ALIB_SINGLETONS
  Prunes given code \p{...} if \alib_singletons is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_SINGLETONS" and sibling macro #"IFNOT_ALIB_SINGLETONS".
  @param ... The source to select.

\def IFNOT_ALIB_SINGLETONS
  Prunes given code \p{...} if \alib_singletons is not included in the \alibbuild.
  \see
    Configuration macro #"ALIB_SINGLETONS" and sibling macro #"IF_ALIB_SINGLETONS".
  @param ... The source to select.

\I{################################################################################################}
\def Log_Prune
  This very simple macro is used for pruning debug <em>Log Statements</em>.
  While it is used as a building block of all other macros for debug logging, for code entities
  using \alox, it provides an easy way to prune code lines that get inserted purely to support
  logging, e.g., to create \alox loggers or to prepare more complex log output. (The alternative
  way is to enclose such code within
  <code> \#ifdef ALOX_DBG_LOG </code> / <code> \#endif </code> preprocessor lines.
  <p>

  Depends on #"ALOX_DBG_LOG". If this is set, the macro just copies the code provided, else it
  does not copy it, hence removes the code.

\def Log_SetSourcePathTrimRule
  Invokes #"Lox::SetSourcePathTrimRule;2" on the debug-singleton of the <b>Lox</b>-instance  
defined in  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_ClearSourcePathTrimRules
  Invokes #"Lox::ClearSourcePathTrimRules;2" on the debug-singleton of the <b>Lox</b>-instance  
  defined in  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_AddDebugLogger
  Invokes  #"Log::AddDebugLogger;2" to create, add and configure a default debug logger suitable 
  for the platform and toolset.
  @param ...  The parameters to pass.

\def Log_RemoveDebugLogger
  Invokes  #"Log::RemoveDebugLogger;2" to remove and delete a debug logger created by 
  #"Log::AddDebugLogger;2".
  @param ...  The parameters to pass.

\def Log_GetLogger
  Invokes #"Lox::GetLogger;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param identifier  The identifier name of the variable that receives the pointer to the logger.
  @param name        The name of the logger to retrieve.

\def Log_RemoveLogger
  Invokes #"Lox::RemoveLogger(const NString&);2" (or its overload) on the debug-singleton of the 
  <b>Lox</b>-instance defined by the macro #"LOG_LOX".
  @param logger  Either the name of or a pointer to the logger to remove.

\def Log_SetDomain
  Invokes #"Lox::SetDomain(const NString&, Scope);2" (or its overload) on the debug-singleton of 
  the <b>Lox</b>-instance defined by the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_RemoveThreadDomain
  Invokes #"Lox::RemoveThreadDomain;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_SetDomainSubstitutionRule
  Invokes #"Lox::SetDomainSubstitutionRule;2" on the debug-singleton of the <b>Lox</b>-instance  
  defined in  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_SetVerbosity
  Invokes one of the overloaded methods #"Lox::SetVerbosity(const NString&, Verbosity);2" (or
  one of its overloads) on the debug-singleton of the 
  <b>Lox</b>-instance defined in macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_SetStartTime
  Invokes #"Lox::SetStartTime(Ticks, const NString&);2" (or one of its overloads) on the 
  debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_MapThreadName
  Invokes #"Lox::MapThreadName;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_LogState
  Invokes #"Lox::State;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_Verbose
  Invokes #"Lox::Verbose;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_Info
  Invokes #"Lox::Info;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_Warning
  Invokes #"Lox::Warning;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_Error
  Invokes #"Lox::Error;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_Assert
  Invokes #"Lox::Assert;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_If
  Invokes #"Lox::If(bool, Verbosity, BoxedObjects&& ...);2" (or one of its overloads) on the 
  debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_Once
  Invokes #"Lox::Once(Verbosity, const Box&, int);2" (or one of its overloads) on the 
  debug-singleton of the <b>Lox</b>-instance defined by the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_Entry
  Invokes #"Lox::Entry;2" on the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_SetPrefix
  Invokes #"Lox::SetPrefix(const Box&, Scope);2" (or one of its  overloads) on the debug-singleton 
  of the <b>Lox</b>-instance defined by the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_Store
  Invokes #"Lox::Store(const Box&, Scope);2" or an overloaded version on the debug-singleton of 
  the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.

\def Log_Retrieve
  Invokes #"Lox::Retrieve(Scope);2" (or its overload) on the debug-singleton of the 
  <b>Lox</b>-instance defined by the macro #"LOG_LOX".
  @param data  The identifier name of the #"Box" object to take the retrieved data.
  @param ...   The parameters to pass.

\def Log_IsActive
  Invokes #"Lox::IsActive;2" on the object of type Lox defined by the macro #"LOG_LOX".
  The result value is assigned to given variable \p{result}
  @param result The name of a variable of type \c int that the return value of \b IsActive() is
                assigned to.
  @param ...    The parameters to pass.

\def Log_GetVerbosity
  Invokes #"Lox::GetVerbosity;2" on the object of type Lox defined by the macro #"LOG_LOX".
  The result value is assigned to given variable \p{result}
  @param result The name of a variable of type \b Verbosity int that the return value of 
                \b GetVerbosity() is assigned to.
  @param ...    The parameters to pass.

\def Log_SetVerbosityExport
  Invokes #"Lox::SetVerbosityExport(detail::Logger*);2" on the object of type Lox defined by the 
  macro #"LOG_LOX".
  @param ...    The parameters to pass.

\def Log_Exception
  Invokes #"LogTools::Exception" providing the debug-singleton of the <b>Lox</b>-instance defined by 
  the macro #"LOG_LOX".
  @param ...  The parameters to pass.
\I{################################################################################################}
\def Lox_Prune
  This very simple macro is used for pruning release <em>Log Statements</em> at the moment release 
  logging gets disabled (what in standard release scenarios is not done).<p>

  While it is used as a building block of all other macros for release logging, for code entities
  using \alox, it provides an easy way to prune code lines that get inserted purely to support
  logging, e.g., to create \alox loggers or to prepare more complex log output. (The alternative
  way is to enclose such code within
  <code> \#ifdef ALOX_REL_LOG </code> / <code> \#endif </code> preprocessor lines.
  <p>

  Depends on #"ALOX_REL_LOG". If this is set, the macro just copies the code provided, else it
  does not copy it, hence removes the code.

  @param ...  The code to prune.

\def Lox_SetSourcePathTrimRule
  Invokes #"Lox::SetSourcePathTrimRule;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_ClearSourcePathTrimRules
  Invokes #"Lox::ClearSourcePathTrimRules;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_GetLogger
  Invokes #"Lox::GetLogger;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param identifier  The identifier name of the variable that receives the pointer to the logger.
  @param name        The name of the logger to retrieve.

\def Lox_RemoveLogger
  Invokes #"Lox::RemoveLogger(const NString&);2" (or its overload) on the object of type Lox 
  defined in macro #"LOX_LOX".
  @param logger  Either the name of or a pointer to the logger to remove.


\def Lox_SetDomain
  Invokes #"Lox::SetDomain(const NString&, Scope);2" (or its overload) on the object of type Lox 
  defined in macro #"LOX_LOX".
  \attention
    If #"ALOX_REL_LOG_CI" is not set, which is the default for release logging, and when used with language-related
    \e Scopes, this method will log an internal warning and will not be effective in respect to
    \e %Scope::Path, \e %Scope::Filename, and \e %Scope::Method.
    See #"alib_mod_alox" for detailed information.

    If Scope Domains based on source-related scopes should be supported in release logging, the
    \alibbuild as well as the software entity have to be compiled with the configuration macro #"ALOX_REL_LOG_CI".
    Note that one effect of setting this macro is, that information on source code paths and file names, as well as
    method names make their way into the release executable. This may not be wanted.

  @param ...  The parameters to pass.

\def Lox_RemoveThreadDomain
  Invokes #"Lox::RemoveThreadDomain;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.


\def Lox_SetDomainSubstitutionRule
  Invokes #"Lox::SetDomainSubstitutionRule;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_SetVerbosity
  Invokes one of the overloaded methods  #"Lox::SetVerbosity(const NString&, Verbosity);2" 
  (or its overload) on the object of type Lox defined by the macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_SetStartTime
  Invokes #"Lox::SetStartTime(Ticks, const NString&);2" (or one of its overloads) on the object of 
  type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_MapThreadName
  Invokes #"Lox::MapThreadName;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_LogState
  Invokes #"Lox::State;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_Verbose
  Invokes #"Lox::Verbose;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_Info
  Invokes #"Lox::Info;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_Warning
  Invokes #"Lox::Warning;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_Error
  Invokes #"Lox::Error;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_Assert
  Invokes #"Lox::Assert;2" on the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_If
  Invokes #"Lox::If(bool, Verbosity, BoxedObjects&& ...);2" (or its overload) on the object of 
  type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_Once
  Invokes #"Lox::Once(const Box&, int);2" (or one of its overloads) on the object of type Lox 
  defined by the macro 
  #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_Entry
  Invokes #"Lox::Entry(const NString&, Verbosity);2" (or its overload) on the object of type Lox 
  defined by 
  the macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_Store
  Invokes #"Lox::Store(const Box&, Scope);2" (or its overload) on the object of type Lox defined 
  by the macro #"LOX_LOX".
  \attention
    If #"ALOX_REL_LOG_CI" is not set, which is the default for release logging
    statements, Log Data cannot be used in conjunction with language-related \e Scopes.<br>
    If <em>Log Data</em> should be supported in release logging, the \alibbuild as well as the
    software entity have to be compiled with the configuration macro #"ALOX_REL_LOG_CI".
    Note that one effect of setting this macro is, that information on source code paths and
    file names, as well as method names make their way into the release executable.
    This may not be wanted.<br>
    In general, methods \b Lox::Store and \b Lox::Retrieve should exclusively be used for
    debug logging. See user manual for more information.
  @param ...  The parameters to pass.

\def Lox_Retrieve
  Invokes #"Lox::Retrieve(Scope);2" (or its overload) on the object of type Lox defined in macro #"LOX_LOX".
  @param data  The identifier name of the #"Box" object to take the retrieved data.
  @param ...   The parameters to pass.

\def Lox_SetPrefix
  Invokes #"Lox::SetPrefix(const Box&, Scope);2" (or one of its  overloads) on the object of type 
  Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.

\def Lox_IsActive
  Invokes #"Lox::IsActive;2" on the object of type Lox defined in macro #"LOX_LOX".
  The result value is assigned to given variable \p{result}
  @param result The name of a variable of type \c int that the return value of \b IsActive() is
                assigned to.
  @param ...    The parameters to pass.

\def Lox_GetVerbosity
  Invokes #"Lox::GetVerbosity;2" on the object of type Lox defined in macro #"LOX_LOX".
  The result value is assigned to given variable \p{result}
  @param result The name of a variable of type \b Verbosity that the return value of 
                \b GetVerbosity() is  assigned to.
  @param ...    The parameters to pass.

\def Lox_SetVerbosityExport
  Invokes #"Lox::SetVerbosityExport(detail::Logger*);2" on the object of type Lox defined in macro 
  #"LOX_LOX".
  @param ...    The parameters to pass.


\def Lox_Exception
  Invokes #"LogTools::Exception" providing the object of type Lox defined in macro #"LOX_LOX".
  @param ...  The parameters to pass.
\I{################################################################################################}
\def LOG_LOX
  The Lox instance used by all debug logging macros. This can be overwritten (prior or after
  including alox.hpp) to allow different instances of class Lox for debug logging within
  different source entities. However, other ways to structure log output and to separate
  log information into different streams exists in \alox and overwriting this macro is not
  recommended for standard use cases.<p>

  Note: The definition must provide a reference (not a pointer) to the \b %Lox object.
                                                

\def LOX_LOX
  The Lox instance that is used by all release logging macros. This has to be set (prior or after
  including alox.hpp) to provide access to a, dedicated instance of class Lox created for
  release logging within software.<br>
  It is of course allowed to use different instances within different source entities.
  However, other ways to structure log output and separate log streams exist in \alox and should
  be evaluated before introducing different instances of class Lox.<p>

  Note: The definition must provide a reference (not a pointer) to the \b %Lox object.

\def LOG_CI
  Dependent on #ALOX_DBG_LOG_CI, this macro provides comma delimited source information
  (sourcefile, line number, function name) or corresponding \c null values.

\def LOX_CI
  Dependent on #ALOX_REL_LOG_CI, this macro provides comma delimited source information
  (sourcefile, line number, function name) or corresponding \c null values.


\def LOG_ACQUIRE
  Macro that is placed at the beginning of almost all 
  \ref alib_macros_mod_alox_debug "debug logging macros".
  Provides scope information (provided that #"ALOX_DBG_LOG_CI" is set) to the lox and places
  the debug lox instance access code using #"LOG_LOX".

\def LOG_RELEASE
  Macro that is placed at the end of almost all 
  \ref alib_macros_mod_alox_debug "debug logging macros".
  Releases the lox which was acquired when passing scope information (in macro #"LOG_ACQUIRE".).


\def LOX_ACQUIRE
  Macro that is placed at the beginning of almost all 
  \ref alib_macros_mod_alox_release "release logging macros".
  Provides scope information (provided that #"ALOX_REL_LOG_CI" is set) to the lox and places
  the release lox instance access code using #"LOX_LOX".

\def LOX_RELEASE
  Macro that is placed at the end of almost all 
  \ref alib_macros_mod_alox_release "release logging macros".
  Releases the lox which was acquired when passing scope information (in macro #"LOG_ACQUIRE".).

\I{################################################################################################}
\def ALIB_BOXING_CUSTOMIZE
  Macro used to specialize template struct #"BoxTraits", which customizes boxing for given
  boxable type \p{TSource} to be mapped to non-array type \p{TMapped}.

  Type alias #"BoxTraits::Mapping" is implemented by the macro to evaluate to
  \p{TMapped>}.
  The implementations of methods #"BoxTraits::Write" and #"BoxTraits::Read"
  are to be provided with the variadic macro arguments.

  This macro belongs to a set of sibling macros. For customization of boxing the best
  suitable macro should be chosen. The set is
  - #"%ALIB_BOXING_CUSTOMIZE"
  - #"ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING"
  - #"ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE"
  - #"ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE"
  - #"ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE_NON_UNBOXABLE"
  - #"ALIB_BOXING_CUSTOMIZE_DENY_BOXING"

  \see
     For more information, consult chapter #"alib_boxing_customizing"
     of the Programmer's Manual of module \alib_boxing.

  @param TSource  The C++ 'source' type to specialize struct #"BoxTraits" for.
  @param TMapped  The target type to map \p{TSource} to.
  @param ...      Definition of \c static \c inline methods #"BoxTraits::Write" and
                  #"BoxTraits::Read".


\def ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING

  Specializes template struct #"BoxTraits".
  This version of #"ALIB_BOXING_CUSTOMIZE;a set of sibling macros", is to be used when
  simple type mappings are to be performed between types that are statically castable, or
  when boxing as pointer should be enforced.

  The macro provides the implementations of all three entities of the type trait \b BoxTraits:
  - Type #"BoxTraits::Mapping" evaluates to \p{TMapped}.
  - Static method #"BoxTraits::Write" is defined to do static cast of the source value to the 
    destination type and to then pass the result to #"Placeholder::Write(const TInt)".
  - Static method #"BoxTraits::Read" is defined to read the mapped type
    using #"boxing::Placeholder::Read;Placeholder::Read<TMapped>" and statically cast
    this to \p{TSource}.

  \see
    Macro #"ALIB_BOXING_CUSTOMIZE" for information about the sibling macros.

  @param TSource  The C++ 'source' type to specialize struct #"BoxTraits" for.
  @param TMapped  The target type to map \p{TSource} to.

\def ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE

  Specializes template struct #"BoxTraits".
  This version of #"ALIB_BOXING_CUSTOMIZE;a set of sibling macros", is to be used when
  a simple type mapping is to be performed for types \p{TSource} that are statically castable
  to \p{TMapped}.<br>
  In contrast to macro #"ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING", with this macro, unboxing
  the source type is denied.

  The macro provides the implementations of all three entities of the type trait \b BoxTraits:
  - Type #"BoxTraits::Mapping" evaluates to using \p{TMapped}.
  - Static method #"BoxTraits::Write" performs a static cast of the
    source value to the destination type and then passes the result to
    #"Placeholder::Write(const TInt)".
  - Static method #"BoxTraits::Read" is declared to return \c void, with
    no implementation given.

  \see
    Macro #"ALIB_BOXING_CUSTOMIZE" for information about the sibling macros.

  @param TSource  The C++ 'source' type to specialize struct #"BoxTraits" for.
  @param TMapped  The target type to map \p{TSource} to.


\def ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE

  Specializes template struct #"BoxTraits", to customize boxing for given
  boxable type \p{TSource} to be mapped to array type \p{TMapped[]}.

  Type definition #"BoxTraits::Mapping" is implemented by the macro to evaluate to
  \p{TMapped}.
  The implementations of methods #"BoxTraits::Write" and #"BoxTraits::Read"
  are to be provided with the variadic macro arguments.

  \see
    Macro #"ALIB_BOXING_CUSTOMIZE" for information about the sibling macros.

  @param TSource  The C++ 'source' type to specialize struct #"BoxTraits" for.
  @param TElement The element type of the array to map \p{TSource} to.
  @param ...      Definition of \c static \c inline methods #"BoxTraits::Write" and
                  #"BoxTraits::Read".

\def ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE_NON_UNBOXABLE
  Specializes template struct #"BoxTraits", to customize boxing for given
  boxable type \p{TSource} to be mapped to array type \p{TMapped[]}.

  Type definition #"BoxTraits::Mapping" is implemented by the macro to evaluate to
  \p{TMapped} and #"BoxTraits::IsArray" evaluates to \c true.<br>
  Static method #"BoxTraits::Read" is declared to return \c void, with
  no implementation given (as it will not be called).<br>
  The implementation of method #"BoxTraits::Write" is to be provided with the variadic
  portion of the macro arguments.

  \see
    Macro #"ALIB_BOXING_CUSTOMIZE" for information about the sibling macros.

  @param TSource  The C++ 'source' type to specialize struct #"BoxTraits" for.
  @param TElement The element type of the array to map \p{TSource} to.
  @param ...      Definition of \c static \c inline method #"BoxTraits::Write".

\def ALIB_BOXING_CUSTOMIZE_DENY_BOXING
  Specializes template struct #"BoxTraits", to completely forbid boxing and unboxing
  of type \p{TSource}.

  For this, type definition #"BoxTraits::Mapping" is implemented by the macro to evaluate
  to \p{TMapped} and just declarations of static methods
  #"BoxTraits::Write" and #"BoxTraits::Read" are given.<br>
  The latter is declared to return \c void. Note that this in principle is not necessary because
  using #"NotBoxableTag" already forbids unboxing. Nevertheless, this way
  concept #"IsLocked" is satisfied for \p{TSource} as well.

  \see
    Macro #"ALIB_BOXING_CUSTOMIZE" for information about the sibling macros.

  @param TSource  The type that should be denied to be used with \alib_boxing_nl.
                                

\def ALIB_BOXING_VTABLE_DECLARE
  Declares an \c extern object of type #"detail::VTable" named \p{Identifier} in
  namespace #"alib::boxing::detail".<br>
  Furthermore specializes the type trait #"VTableOptimizationTraits" for
  mapped type \p{TMapped} to have its method \b Get return the extern <em>VTable</em>-instance.

  This macro has to be placed in a header included by each compilation unit that
  creates or accesses boxes of type \p{TMapped}.

  \see
    - Chapter #"alib_boxing_more_opt_staticvt" of the Programmer's Manual of module
      \alib_boxing_nl.
    - Macros #"ALIB_BOXING_VTABLE_DEFINE" and #"ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER".

  @param TMapped     The mapped type to declare a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.


\def ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE
  Same as #"ALIB_BOXING_VTABLE_DECLARE", but used with mapped array types.
  Specializes #"VTableOptimizationTraits" for mapped type
  \p{TMapped}.

  \see
     Macros #"ALIB_BOXING_VTABLE_DECLARE" (for plain types), #"ALIB_BOXING_VTABLE_DEFINE",
     #"ALIB_BOXING_VTABLE_DEFINE" and #"ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER".
    
  @param TMapped     The mapped type to declare a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.


\def ALIB_BOXING_VTABLE_DEFINE
  Defines the external object declared with #"ALIB_BOXING_VTABLE_DECLARE".

  This macro has to be placed in a compilation unit.

  @param TMapped     The mapped type to define a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.


\def ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE
  Defines the external object declared with #"ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE".

  This macro has to be placed in a compilation unit.

  @param TMapped     The mapped type to define a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.


\def ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER
  Registers a statically created \e vtable singleton declared with either
  #"ALIB_BOXING_VTABLE_DECLARE" or #"ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE".

  This macro has to be placed in the #"alib_mod_bs;bootstrap code" of software.
  In debug-compilations, this macro is empty.
  \see
    Chapter #"alib_boxing_more_opt_staticvt" of the #"alib_mod_boxing;Programmer's Manual"
    of module \alib_boxing_nl.

  @param Identifier  The identifier name of the \e vtable singleton as passed to macros
                     #"ALIB_BOXING_VTABLE_DECLARE" or #"ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE".

\def ALIB_BOXING_DEFINE_FEQUALS_FOR_COMPARABLE_TYPE
  This macro #"boxing::BootstrapRegister;registers" templated box-function
  #"FEquals::ComparableTypes" for given mapped type \p{TComparable}.

  If a type is boxed as pointer type, then \p{TComparable} has to be given as pointer type.
  For comparison, the unboxed pointers will be dereferenced. If both are \e nulled, \c true
  is returned, if one is \e nulled, \c false.

  As all function registrations have to be performed at runtime, this macro is to be placed in the
  #"alib_mod_bs;bootstrap section" of an application.

  @param TComparable  The comparable type name.


\def ALIB_BOXING_DEFINE_FISLESS_FOR_COMPARABLE_TYPE
  This macro #"boxing::BootstrapRegister;registers" templated box-function
  #"FIsLess::ComparableTypes" for given type \p{TComparable}.

  If a type is boxed as pointer type, then \p{TComparable} has to be given as pointer type.
  For comparison, the unboxed pointers will be dereferenced. If both are \e nulled, \c false
  is returned, if only \p{lhs} is \e nulled, \c true and if only \p{rhs} is \e nulled, then
  \c false.

  As all function registrations have to be performed at runtime, this macro is to be placed in the
  #"alib_mod_bs;bootstrap section" of an application.

  @param TComparable  The comparable type name.


\def ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE

  This macro registers templated box-function implementation #"FAppend::Appendable"
  for the given type \p{TAppendable}.

  The macro is to be placed in the bootstrap section of an application, for any type that is
  #"alib_strings_assembly_ttostring;appendable" to class \b %AString.

  This macro selects version box-function type <b>%FAppend\<character\></b>.
  If a custom type is appendable to two or three underlying character types of class \b %AString,
  then macros
  - #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N",
  - #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W" and
  - #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X"

  should be used. Note that for each character type of \alib_strings_nl, a different
  box-function is registered.

  \note
    As by default, custom types get boxed as pointers, the type in question usually has to be
    provided here as pointer type, for example:

           ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( my_namespace::MyType* )

  @param TAppendable  The appendable type.


\def ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N
  Same as macro #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE" but implements
  interface \b FAppend<nchar> instead of \b FAppend<character>.

  @param TAppendable  The appendable type.


\def ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W
  Same as macro #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE" but implements
  interface \b FAppend<wchar> instead of \b FAppend<character>.

  @param TAppendable  The appendable type.


\def ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X
  Same as macro #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE" but implements
  interface \b FAppend<xchar> instead of \b FAppend<character>.

  @param TAppendable  The appendable type.
\I{################################################################################################}
\def A_CHAR
  Macro used to express C++ character and string literals in a platform-independent way.
  Dependent on code selection macros #"ALIB_CHARACTERS_WIDE" and eventually
  #"ALIB_CHARACTERS_NATIVE_WCHAR", this macro possibly prepends character <c>'L'</c>, <c>'u'</c>
  or <c>'U'</c> to the character or string literal given with parameter \p{STR}.

  Whenever software targets different platforms that change the underlying character width of
  the string-types, the string literals have to change as well. To be able to compile on different
  platforms while using different configuration macros to manipulate the character width, almost all string
  literals in \alib sources are enclosed by this macro. The exception concerns such
  literals which are assigned to narrow string type #"NString;2", or those where explicit
  string types #"WString" or #"XString" are used.

  \note To avoid too much code clutter, this macro, as well as its siblings
        #"A_CCHAR", #"A_SCHAR", #"A_NCHAR", #"A_WCHAR" and #"A_XCHAR",
        have an abbreviated name, as they are quite frequently used.
        (According to the #"alib_manual_appendix_naming;ALib naming scheme", the macros would
        be prefixed with <b>"ALIB_CHARACTERS_"</b>)

  \see For more information, consult chapter #"alib_characters_prepro_literals"
       of the #"alib_mod_characters;Programmer's Manual" of module \alib_characters_nl.<br>
       With module \alib_system, the further similar macro #"A_PATH" privided.

  @param STR The character or string literal to be eventually prefix with <c>'L'</c>, <c>'u'</c> or <c>'U'</c>.

\def A_CCHAR
  Macro used to express C++ string literals in a platform-independent way. Dependent on
  the definition of type #"characters::complementChar" (which is in turn dependent on the platform, the
  compiler and optional configuration macros), this macro defines string literals, which have either
  a one, two or four byte character width, namely the width corresponding to the width of
  \b %#"characters::complementChar".

  \see Sibling macro #"A_CHAR" for more information.
       Further sibling macros are #"A_SCHAR", #"A_NCHAR", #"A_WCHAR" and #"A_XCHAR".<br>
       Details are described in chapter #"alib_characters_prepro_literals"
       of the #"alib_mod_characters;Programmer's Manual" of module \alib_characters_nl.


   @param STR The character or string literal to eventually prefix with <c>'u'</c>, <c>'U'</c> or <c>'L'</c> .

\def A_SCHAR
  Macro used to express C++ string literals in a platform-independent way. Dependent on
  the definition of type #"characters::strangeChar" (which is in turn dependent on the platform, the
  compiler and optional configuration macros), this macro defines string literals, which have either
  a two or four byte character width, namely the width corresponding to the width of
  \b %#"characters::complementChar".

  \note
    The same as type #"characters::strangeChar" is always equivalent to type #"characters::xchar",
    this macro is always equivalent to macro #"A_XCHAR".

  \see Sibling macro #"A_CHAR" for more information.
       Further sibling macros are #"A_CCHAR", #"A_NCHAR", #"A_WCHAR" and #"A_XCHAR".<br>
       Details are described in chapter #"alib_characters_prepro_literals"
       of the #"alib_mod_characters;Programmer's Manual" of module \alib_characters_nl.


   @param STR The character or string literal to prefix with <c>'u'</c>, <c>'U'</c> or <c>'L'</c> .

\def A_NCHAR
  This is an "ident macro" that just produces the given parameter \p{STR}. It is provided only for
  completeness. \alib does not use this macro internally, as string literals that are not surrounded
  by one of the character literal macros are very well identifiable as narrow literals.

  \see Sibling macro #"A_CHAR" for more information.
       Further sibling macros are #"A_CCHAR", #"A_SCHAR", #"A_WCHAR" and #"A_XCHAR".<br>
       Details are described in chapter #"alib_characters_prepro_literals"
       of the #"alib_mod_characters;Programmer's Manual" of module \alib_characters_nl.

  @param STR The character or string literal that is returned as is.


\def A_WCHAR
  Macro used to express C++ string literals in a platform-independent way. Dependent on
  the definition of type #"characters::wchar" (which is in turn dependent on the platform, the
  compiler and optional configuration macros), this macro defines string literals, which have an either
  two bytes or four byte character width, namely the width corresponding to the width of
  \b %wchar.

  \see Sibling macro #"A_CHAR" for more information.
       Further sibling macros are #"A_CCHAR", #"A_SCHAR", #"A_NCHAR" and #"A_XCHAR".<br>
       Details are described in chapter #"alib_characters_prepro_literals"
       of the #"alib_mod_characters;Programmer's Manual" of module \alib_characters_nl.


   @param STR The character or string literal to prefix with <c>'u'</c>, <c>'U'</c> or <c>'L'</c> .

\def A_XCHAR
  Macro used to express C++ string literals in a platform-independent way. Dependent on
  the definition of type #"characters::xchar" (which is in turn dependent on the platform, the
  compiler and optional configuration macros), this macro defines string literals, which have an either
  two bytes or four byte character width, namely the width corresponding to the width of
  \b %xchar.

  \note
    The same as type #"characters::strangeChar" is always equivalent to type #"characters::xchar",
    this macro is always equivalent to macro #"A_SCHAR".

  \see Sibling macro #"A_CHAR" for more information.
       Further sibling macros are #"A_CCHAR", #"A_SCHAR", #"A_NCHAR" and #"A_WCHAR".<br>
       Details are described in chapter #"alib_characters_prepro_literals"
       of the #"alib_mod_characters;Programmer's Manual" of module \alib_characters_nl.

   @param STR The character or string literal to prefix with <c>'u'</c>, <c>'U'</c> or <c>'L'</c> .

\def ALIB_CHAR_TYPE_ID_N
  Defined as \c 1. Used in situations where code is to be selected by the preprocessor.
  The rationale here is, that the preprocessor can only choose code depending on integral values
  and not on string contents.

\def ALIB_CHAR_TYPE_ID_W
  Defined as \c 2. Used in situations where code is to be selected by the preprocessor.
  The rationale here is, that the preprocessor can only choose code depending on integral values
  and not on string contents.

\def ALIB_CHAR_TYPE_ID_X
  Defined as \c 3. Used in situations where code is to be selected by the preprocessor.
  The rationale here is that the preprocessor can only choose code depending on integral values
  and not on string contents.

\def ALIB_CHAR_TYPE_ID_CHARACTER
  Defined as #"ALIB_CHAR_TYPE_ID_N" or \c ALIB_CHAR_TYPE_ID_W depending on configuration macro
  #"ALIB_CHARACTERS_WIDE".

\def ALIB_CHAR_TYPE_ID_COMPLEMENT
  Defined as #"ALIB_CHAR_TYPE_ID_N" or \c ALIB_CHAR_TYPE_ID_W depending on configuration macro
  #"ALIB_CHARACTERS_WIDE".

\def ALIB_CHAR_TYPE_ID_STRANGE
  Defined as #"ALIB_CHAR_TYPE_ID_X".

ALIB_CHARACTERS_WIDE
\def ALIB_CHARACTER_ARRAY
  Specializes the type trait #"ArrayTraits" for type \p{T}.

  To implement the corresponding static methods in alignment with the specialized access and construction
  flags, macros
  #"ALIB_CHARACTER_ARRAY_IMPL_BUFFER",
  #"ALIB_CHARACTER_ARRAY_IMPL_LENGTH" and
  #"ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT" are proposed to be used.

  \see
    For more information about character array traits, see chapter
    #"alib_characters_arrays" of the Programmer's Manual of module \alib_characters.

  @param T            The type to provide array type-traits for.
  @param TChar        The character type of character arrays that \p{T} represents or might be created of.
  @param Access       One of the values \b NONE, \b Implicit or \b ExplicitOnly.
                      Value \b Mutable is not permitted. Instead, macro #"ALIB_CHARACTER_ARRAY_MUTABLE"
                      is to be used for that case.
  @param Construction One of the values \b NONE, \b Implicit or \b ExplicitOnly.


\def ALIB_CHARACTER_ARRAY_MUTABLE
  Specializes the type trait #"ArrayTraits" for type \p{T} with
  access modifier \b Mutable.

  To implement the corresponding static methods in alignment with the specialized access and construction
  flags, macros
  #"ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE",
  #"ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE" and
  #"ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT" are proposed to be used.

  \see
    For more information about character array traits, see chapter
    #"alib_characters_arrays" of the Programmer's Manual of module \alib_characters.

  @param T            The type to provide array type-traits for.
  @param TChar        The character type of character arrays that \p{T} represents or might be created of.
  @param Construction One of the values \b NONE, \b Implicit or \b ExplicitOnly.


\def ALIB_CHARACTER_ZT_ARRAY
  Specializes the type trait #"ZTArrayTraits" for type \p{T}.

  To implement the corresponding static methods in alignment with the specialized access and construction
  flags, macros
  #"ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER",
  #"ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH" and
  #"ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT" are proposed to be used.

  \see
    For more information about character array traits, see chapter
    #"alib_characters_arrays" of the Programmer's Manual of module \alib_characters.

  @param T            The type to provide array type-traits for.
  @param TChar        The character type of character arrays that \p{T} represents or might be created of.
  @param Access       One of the values \b NONE, \b Implicit or \b ExplicitOnly.
                      Value \b Mutable is not permitted. Instead, macro #"ALIB_CHARACTER_ARRAY_MUTABLE"
                      is to be used for that case.
  @param Construction One of the values \b NONE, \b Implicit or \b ExplicitOnly.


\def ALIB_CHARACTER_ZT_ARRAY_MUTABLE
  Specializes the type trait #"ZTArrayTraits" for type \p{T} with
  access modifier \b Mutable.

  To implement the corresponding static methods in alignment with the specialized access and construction
  flags, macros
  #"ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE",
  #"ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE" and
  #"ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT" are proposed to be used.

  \see
    For more information about character array traits, see chapter
    #"alib_characters_arrays" of the Programmer's Manual of module \alib_characters.

  @param T            The type to provide array type-traits for.
  @param TChar        The character type of character arrays that \p{T} represents or might be created of.
  @param Construction One of the values \b NONE, \b Implicit or \b ExplicitOnly.


\def ALIB_CHARACTER_ARRAY_IMPL_BUFFER
  This macro may be used to implement static method #"ArrayTraits::Buffer;*" of
  specializations of \b ArrayTraits that have been defined using macro #"ALIB_CHARACTER_ARRAY".

  If macro #"ALIB_CHARACTER_ARRAY_MUTABLE" was used, corresponding macro
  #"ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE" has to be used instead of this one.

  The argument providing a constant reference of type \p{T} to the method's implementation, and
  which has to be accessed in the given implementation code, is named \p{src}.
  The implementation has to return a constant pointer to an array of character type \p{TChar}.

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER
  Same as #"ALIB_CHARACTER_ARRAY_IMPL_BUFFER", but for zero-terminated character arrays.<br>
  (Implements method #"ZTArrayTraits::Buffer;*" instead of  #"ArrayTraits::Buffer;*".)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE
  Alternative macro version of #"ALIB_CHARACTER_ARRAY_IMPL_BUFFER", which declares
  method argument \p{src} as a mutual reference of type \p{T}.

  This version is to be used if specialization was performed using
  #"ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE".

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE
  Same as #"ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE", but for zero-terminated character arrays.<br>
  (Implements method #"ZTArrayTraits::Buffer;*" instead of #"ArrayTraits::Buffer;*".)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ARRAY_IMPL_LENGTH
  This macro may be used to implement static method #"ArrayTraits::Length;*" of
  specializations of \b ArrayTraits that have been defined using macro #"ALIB_CHARACTER_ARRAY".

  The argument providing a constant reference of type \p{T} to the method's implementation, and
  which has to be accessed in the given implementation code, is named \p{src}.
  The implementation has to return the length of the character array as type #"lang::integer".

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH
  Same as #"ALIB_CHARACTER_ARRAY_IMPL_LENGTH", but for zero-terminated character arrays.<br>
  (Implements method #"ZTArrayTraits::Length;*" instead of #"ArrayTraits::Length;*".)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE
  Alternative macro version of #"ALIB_CHARACTER_ARRAY_IMPL_LENGTH", which declares
  method argument \p{src} as a mutual reference of type \p{T}.

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE
  Same as #"ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE", but for zero-terminated character arrays.<br>
  (Implements method #"ZTArrayTraits::Length;*" instead of #"ArrayTraits::Length;*".)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT
  This macro may be used to implement static method #"ArrayTraits::Construct;*" of
  specializations of \b ArrayTraits that have been defined using macro #"ALIB_CHARACTER_ARRAY"
  or #"ALIB_CHARACTER_ARRAY_MUTABLE".

  The arguments providing the array data to the method's implementation, which is to be used to
  create the object of type \p{T}, are named \p{array} and \p{length}.
  The implementation has to return a value of type \p{T}.

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be
                created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.


\def ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT
  Same as #"ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT", but for zero-terminated character arrays.<br>
  (Implements method #"ZTArrayTraits::Construct;*" instead of #"ArrayTraits::Construct;*".)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be
                created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.
\I{################################################################################################}
\def ALIB_VARIABLES_DEFINE_TYPE
  Defines a struct named <c>alib::variables::detail::VMeta_<<CPPName>></c>, which
  is derived from struct #"VMeta" and which declares all abstract virtual
  methods and implements most of them already. The methods are:
  Method                           | Status
  ---------------------------------|--------------------
  #"VMeta::typeName"    | defined
  #"VMeta::dbgTypeID"   | defined (in debug-compilations only)
  #"VMeta::construct"   | defined
  #"VMeta::destruct"    | defined
  #"VMeta::size"        | defined
  #"VMeta::imPort"      | declared only
  #"VMeta::exPort"      | declared only

  This is the first step on the way to create an \alib variable of a custom storage type.

  \see
    - Sibling macro #"ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR".
    - See struct #"VMeta".
    - See chapter #"alib_variables_types_custom" of the Programmer's Manual of camp \alib_variables_nl
      for more details.
    - Macro #"ALIB_VARIABLES_REGISTER_TYPE" for how to register a variable type with
      a configuration
    Chapter #"alib_strings_assembly_ttostring" of the Programmer's Manual of module
    \alib_strings.

  @param Namespace      The namespace of the type to enable as an \alib variable.
  @param CPPName        The name of the type to enable as an \alib variable.
  @param CfgTypeString  The type string used to declare variables of this type.

\def ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR
  Same as #"ALIB_VARIABLES_DEFINE_TYPE" with the only difference that virtual override
  #"VMeta::construct" passes the pool allocator of the configuration
  instance to the constructor of the custom type.
  @param Namespace      The namespace of the type to enable as an \alib variable.
  @param CPPName        The name of the type to enable as an \alib variable.
  @param CfgTypeString  The type string used to declare variables of this type.

\def ALIB_VARIABLES_REGISTER_TYPE(CPPName)
  Registers a custom implementation of abstract class #"VMeta", which has been
  previously defined with macro #"ALIB_VARIABLES_DEFINE_TYPE" with the configuration
  object received by a call to <c>GetConfig()</c>.
  This macro is to be placed in #"Camp::Bootstrap;*" in the section of phase
  #"BootstrapPhases::PrepareConfig".
  The macro invokes the method #"Configuration::RegisterType;*", which has to be called
  'manually' if for some reason the placement of registration can't be done as proposed.

  @param CPPName   The name of the type to enable as an \alib variable (excluding the namespace).
\I{################################################################################################}
\def ALIB_ENUMS_ASSIGN_RECORD
  Macro used to specialize the type trait
  #"enumrecords::RecordsTraits;RecordsTraits<TEnum>" to associate C++ enumeration type \p{TEnum} with
  #"alib_enums_records;ALib Enum Records" of type \p{TRecord}.

  @param TEnum    The enumeration type to define data records for.
  @param TRecord  The type of the data record to assign.

\def ALIB_ENUMS_MAKE_ARITHMETICAL
  Specializes the type trait #"ArithmeticalTraits" to inherit \c std::true_type to
  enable a set of arithmetic operators on the elements of \p{TEnum}.

  Usually, this macro is placed in a header-file, probably close to the enum type definition.
  However, it has to be placed <b>in global (no) namespace</b>.

  \see Chapter #"alib_enums_arithmetic_standard" of the Programmer's Manual of module
       \alib_enumops for more information.

  @param TEnum    Type of a scoped or non-scoped enumeration that is to be declared an arithmetical
                  type.
                 
\def ALIB_ENUMS_MAKE_BITWISE
  Specializes the type trait #"BitwiseTraits" to inherit \c std::true_type.

  Usually, this macro is placed in a header-file, probably close to the enum type definition.
  However, it has to be placed <b>in global (no) namespace</b>.

  \see Chapter #"alib_enums_arithmetic_bitwise" of the Programmer's Manual of module
       \alib_enumops for more information.

  @param TEnum    Type of a scoped or non-scoped enumeration that is to be declared a bitwise
                  type.


\def ALIB_ENUMS_MAKE_ITERABLE
  Shortcut to #"ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END", providing <c>TEnum(0)</c> as macro
  parameter \p{StartElement}.

  Usually, this macro is placed in a header-file, probably close to the enum type definition.
  However, it has to be placed <b>in global (no) namespace</b>.

  \see Sibling macro #"ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END" and chapter
       #"alib_enums_iter" of the Programmer's Manual of module
       \alib_enumops for more information.

  @param TEnum         Type of a scoped or non-scoped enumeration that is to be declared an
                       iterable enum type.
  @param StopElement   The enum element after the last "valid" element in the enumeration.<br>
                       Will be used as field #"IterableTraits::End;*".

\def ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END
  Specializes the type trait #"IterableTraits" to implement methods:
  - \b %Begin to return \p{StartElement}
  - \b %End to return \p{StopElement}

  Usually, this macro is placed in a header-file, probably close to the enum type definition.
  However, it has to be placed <b>in global (no) namespace</b>.

  \see Sibling macro #"ALIB_ENUMS_MAKE_ITERABLE" and chapter
       #"alib_enums_iter" of the Programmer's Manual of module
       \alib_enumops for more information.
  \see

  @param TEnum        Type of a scoped or non-scoped enumeration that is to be declared an
                      iterable enum type.
  @param StartElement The first element of the enumeration.<br>
                      Will be used as field #"IterableTraits::End;*".
  @param StopElement  The enum element after the last "valid" element in the enumeration.<br>
                      Will be used as field #"IterableTraits::End;*".

\I{################################################################################################}
\def CALCULUS_CALLBACK

  Writes given callback function name \p{func} along with a comma (<c>','</c>) and a "stringified"
  version of the C++ function name.

  In release compilations, the macro resolves just to \p{func}.

  This macro is to be used to define records of types
  #"Calculus::FunctionEntry",
  #"Calculus::OperatorTableEntry" and to provide parameters to function
  #"Calculus::AddOperator".

  @param func     The name of the callback function.


\def CALCULUS_DEFAULT_AUTOCAST

  This macro is to be used to define records of type
  #"Calculus::AutoCastEntry".

  With debug-builds, it provides \c nullptr for table entries #"AutoCastEntry::Callback"
  and #"AutoCastEntry::DbgCallbackName".
  In release compilations the latter is omitted.


\def CALCULUS_SIGNATURE

  This macro is to be used to define records of type
  #"Calculus::FunctionEntry".

  The macro resolves to two values separated by a comma (<c>','</c>).
  The first is a plain copy of given \p{BoxPointerArray}, which has to be a C++ array of pointers
  to type \b Box. The second value written will be the array's extent.

  @param BoxPointerArray  \c nullptr for identifiers, otherwise the list of argument sample boxes
                          provided as a <b>Box**</b>.
\I{################################################################################################}
\def ALIB_STRINGS_APPENDABLE_TYPE
  Helper macro for specializing functor #"AppendableTraits" for type \p{TYPE}.
  This macro has to be positioned outside any namespace, and the given type has to include
  its full namespace qualification.

  This macro is to be used in combination with macro #ALIB_STRINGS_APPENDABLE_TYPE_DEF.<br>
  As an alternative to the two macros, #ALIB_STRINGS_APPENDABLE_TYPE_INLINE might be used, which
  will specialize \b AppendableTraits and define its \b operator() inline.

  \see
    Chapter #"alib_strings_assembly_ttostring" of the Programmer's Manual of module
    \alib_strings.

  @param TYPE The type to specialize functor \b AppendableTraits for.


\def ALIB_STRINGS_APPENDABLE_TYPE_N
  Same as #"ALIB_STRINGS_APPENDABLE_TYPE" but for character type #"alib::nchar;2".

  @param TYPE The type to specialize functor \b AppendableTraits for.


\def ALIB_STRINGS_APPENDABLE_TYPE_W
  Same as #"ALIB_STRINGS_APPENDABLE_TYPE" but for character type #"alib::wchar;2".

  @param TYPE The type to specialize functor \b AppendableTraits for.


\def ALIB_STRINGS_APPENDABLE_TYPE_DEF
  Macro used in combination with #"ALIB_STRINGS_APPENDABLE_TYPE" which specializes functor
  #"AppendableTraits" for type \p{TYPE} and standard character type, and with this declares its
  member \b operator().<br>
  This macro is used for the implementation of this member function.

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_DEF_N
  Same as #"ALIB_STRINGS_APPENDABLE_TYPE_DEF" but for character type #"alib::nchar;2".

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_DEF_W
  Same as #"ALIB_STRINGS_APPENDABLE_TYPE_DEF" but for character type #"alib::wchar;2".

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_INLINE
  Helper macro for specializing functor #"AppendableTraits" for a custom type \p{TYPE}.
  This macro has to be positioned outside any namespace, and the given type has to include
  its full namespace qualification.

  With the specialization of struct, <b>AppendableTraits<TYPE>::operator()</b> will be defined and
  implemented inline.

  Macros #ALIB_STRINGS_APPENDABLE_TYPE and #ALIB_STRINGS_APPENDABLE_TYPE_DEF
  provide a non-inline alternative to this macro.

  \see
    Chapter #"alib_strings_assembly_ttostring" of the Programmer's Manual of module
    \alib_strings.

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_INLINE_N
  Same as #"ALIB_STRINGS_APPENDABLE_TYPE_INLINE" but for character type #"alib::nchar;2".

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_INLINE_W
  Same as #"ALIB_STRINGS_APPENDABLE_TYPE_INLINE" but for character type #"alib::wchar;2".

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR
  Helper macro for specializing type trait
  #"std::SuppressStdOStreamOpTraits" for a custom type \p{TYPE}.
  This macro has to be positioned outside any namespace, and the given type has to include
  its full namespace qualification.

  @param TYPE The type to specialize functor \b AppendableTraits for.


\def ALIB_STRING_DBG_CHK
  Simple macro that just invokes method _dbgCheck(), which is defined for classes
  #"^String", #"^CString" and #"^AString".
  It is active only when the configuration macro #"ALIB_DEBUG_STRINGS" is \c true.
  The macro is placed in almost every method.



\def ALIB_STRINGS_TO_NARROW
  Creates a new local string variable of narrow character type.
  The name of the variable is defined by parameter \p{dest}.
  If code selection macro #"ALIB_CHARACTERS_WIDE" is \c false, then \p{dest} becomes just a reference
  to \p{src}. The macro in this case is defined as:

        decltype(src)& dest= src;

  Otherwise, \p{dest} is of type #"strings::TLocalString;LocalString<nchar,bufSize>"
  and \p{src} is passed to its constructor.

  @param src        The source string.
  @param dest       The name of the destination variable.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_FROM_NARROW
  Creates a new local string variable of standard #"characters::character" type.
  The name of the variable is defined by parameter \p{dest}.
  If code selection macro #"ALIB_CHARACTERS_WIDE" is \c false, then \p{dest} becomes just a reference
  to \p{src}. The macro in this case is defined as:

        decltype(src)& dest= src;

  Otherwise, \p{dest} is of type #"strings::TLocalString;LocalString<character,bufSize>"
  and \p{src} is passed to its constructor.

  @param src        The source string.
  @param dest       The name of the destination variable.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_TO_NARROW_ARG
  Used when \alib strings of standard character size should be passed as a string argument of narrow
  size.
  If code selection macro #"ALIB_CHARACTERS_WIDE" is \c false, then the macro simply
  passes (is defined as) \p{src}.<br>
  Otherwise, the macro wraps \p{src} in an object of type
  #"strings::TLocalString;LocalString<nchar,bufSize>" which converts the string to the right
  character type.

  @param src        The source string.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_FROM_NARROW_ARG
  Used when \alib strings of narrow character size should be passed as a string argument of standard
  size.
  If code selection macro #"ALIB_CHARACTERS_WIDE" is \c false, then the macro simply
  passes (is defined as) \p{src}.<br>
  Otherwise, the macro wraps \p{src} in an object of type
  #"strings::TLocalString;LocalString<character,bufSize>".
  @param src        The source string.
  @param bufSize    The local buffer size used for the conversion.


\def ALIB_STRINGS_TO_WIDE
  Creates a new local string variable of wide character type.
  The name of the variable is defined by parameter \p{dest}.
  If code selection macro #"ALIB_CHARACTERS_WIDE" is \c true, then \p{dest} becomes just a reference
  to \p{src}. The macro in this case is defined as:

        decltype(src)& dest= src;

  Otherwise, \p{dest} is of type #"strings::TLocalString;LocalString<nchar,bufSize>"
  and \p{src} is passed to its constructor.

  @param src        The source string.
  @param dest       The name of the destination variable.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_FROM_WIDE
  Creates a new local string variable of standard #"characters::character" type.
  The name of the variable is defined by parameter \p{dest}.
  If code selection macro #"ALIB_CHARACTERS_WIDE" is \c true, then \p{dest} becomes just a reference
  to \p{src}. The macro in this case is defined as:

        decltype(src)& dest= src;

  Otherwise, \p{dest} is of type #"strings::TLocalString;LocalString<character,bufSize>"
  and \p{src} is passed to its constructor.

  @param src        The source string.
  @param dest       The name of the destination variable.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_TO_WIDE_ARG
  Used when \alib strings of standard character size should be passed as a string argument of wide
  size.
  If code selection macro #"ALIB_CHARACTERS_WIDE" is \c true, then the macro simply
  passes (is defined as) \p{src}.<br>
  Otherwise, the macro wraps \p{src} in an object of type
  #"strings::TLocalString;LocalString<nchar,bufSize>".

  @param src        The source string.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_FROM_WIDE_ARG
  Used when \alib strings of wide character size should be passed as a string argument of standard
  size.
  If code selection macro #"ALIB_CHARACTERS_WIDE" is \c true, then the macro simply
  passes (is defined as) \p{src}.<br>
  Otherwise, the macro wraps \p{src} in an object of type
  #"strings::TLocalString;LocalString<character,bufSize>".

  @param src        The source string.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRING_RESETTER
  Creates an \e 'anonymous' instance of class #"TStringLengthResetter". Its
  identifier name is assembled using macro #"ALIB_IDENTIFIER", hence from the given
  \b AString's identifier and the line number in the code.

  @param astring The #"^AString" to reset to its original length when the C++ block scope where 
                 this macro is placed, is left.
\I{################################################################################################}
\def ALIB_LOCK
 Alias of macro #"ALIB_OWN", providing <c>*this</c> as the owner.
 Commonly used with types derived from classes #"threads::Lock" or #"RecursiveLock".

 \note
    If module \alib_threads is not available in the \alibbuild, this macro is still defined with
    debug-builds.
    In this case, namespace function#"assert::SingleThreaded" is invoked.
    This detects multithreaded use of a non-supporting \alibbuild.

 @see Alternative macro ALIB_LOCK_WITH

\def ALIB_LOCK_WITH
 Alternative to macro #"ALIB_LOCK" which allows specifying the ownable.
 (Macro \b ALIB_LOCK fixes this to <c>*this</c>).
 @param lock  The mutex to acquire and release.

\def ALIB_LOCK_RECURSIVE
 Alternative to macro #"ALIB_LOCK" which uses macro #"ALIB_OWN_RECURSIVE"
 instead of #"ALIB_OWN" and thus creates an anonymous instance of #"OwnerRecursive"
 instead of #"lang::Owner".
                            
\def ALIB_LOCK_RECURSIVE_WITH
 Alternative to macro #"ALIB_LOCK_WITH" which uses macro #"ALIB_OWN_RECURSIVE"
 instead of #"ALIB_OWN" and thus creates an anonymous instance of #"OwnerRecursive"
 instead of #"lang::Owner".
 @param lock  The mutex to acquire and release.

\def ALIB_LOCK_SHARED
 Alternative to macro #"ALIB_LOCK" which uses macro #"ALIB_OWN_SHARED"
 instead of #"ALIB_OWN" and thus creates an anonymous instance of #"OwnerShared"
 instead of #"lang::Owner".
                            
\def ALIB_LOCK_SHARED_WITH
 Alternative to macro #"ALIB_LOCK_WITH" which uses macro #"ALIB_OWN_SHARED"
 instead of #"ALIB_OWN" and thus creates an anonymous instance of #"OwnerShared"
 instead of #"lang::Owner".
 @param lock  The mutex to acquire and release.
                            
\def ALIB_DCS
 Alias of macro #"ALIB_OWN", providing <c>*this</c> as the owner.
 Commonly used with types derived from classes #"lang::DbgCriticalSections".

 \note
    If the configuration macro #"ALIB_DEBUG_CRITICAL_SECTIONS" is not set, this macro is still defined
    with debug-builds.
    In this case, namespace function#"assert::SingleThreaded" is invoked.
    This detects multithreaded use of a non-supporting \alibbuild.

 @see
   - Sibling macros #"ALIB_DCS_WITH", #"ALIB_DCS_SHARED", #"ALIB_DCS_ACQUIRE", etc.
   - Chapter #"alib_manual_appendix_callerinfo" of the General Programmer's Manual.
                                                                   
                  
\def ALIB_DCS_WITH
 Alternative to macro #"ALIB_DCS" which allows specifying the ownable.
 (Macro \b ALIB_DCS fixes this to <c>*this</c>).
 @param CS  The #"lang::DbgCriticalSections" to acquire and release.

\def ALIB_DCS_SHARED
 Alternative to macro #"ALIB_DCS" which uses macro #"ALIB_OWN_SHARED"
 instead of #"ALIB_OWN" and thus creates an anonymous instance of #"OwnerShared"
 instead of #"lang::Owner".
                            
\def ALIB_DCS_SHARED_WITH
 Alternative to macro #"ALIB_DCS_WITH" which uses macro #"ALIB_OWN_SHARED"
 instead of #"ALIB_OWN" and thus creates an anonymous instance of #"OwnerShared"
 instead of #"lang::Owner".
 @param CS  The #"lang::DbgCriticalSections" to acquire and release.
                            
\def ALIB_DCS_ACQUIRE
  Calls #"DbgCriticalSections::Acquire;*" (on \c this).
  
\def ALIB_DCS_ACQUIRE_WITH
  Calls #"DbgCriticalSections::Acquire;*" on the instance given with \p{CS}.
  @param CS The \b DbgCriticalSections instance to use.
  
\def ALIB_DCS_ACQUIRE_SHARED
  Calls #"DbgCriticalSections::AcquireShared;*" (on \c this).
  
\def ALIB_DCS_ACQUIRE_SHARED_WITH
  Calls #"DbgCriticalSections::AcquireShared;*" on the instance given with \p{CS}.
  @param CS The \b DbgCriticalSections instance to use.
  
\def ALIB_DCS_RELEASE
  Calls #"DbgCriticalSections::Release;*" (on \c this).
  
\def ALIB_DCS_RELEASE_WITH
  Calls #"DbgCriticalSections::Release;*" on the instance given with \p{CS}.
  @param CS The \b DbgCriticalSections instance to use.
  
\def ALIB_DCS_RELEASE_SHARED
  Calls #"DbgCriticalSections::ReleaseShared;*" (on \c this).
  
\def ALIB_DCS_RELEASE_SHARED_WITH
  Calls #"DbgCriticalSections::ReleaseShared;*" on the instance given with \p{CS}.
  @param CS The \b DbgCriticalSections instance to use.

\I{################################################################################################}
\def ALIB_RESOURCED
   Macro used to specialize the type trait #"ResourcedTraits" for given type
   \p{T}.

   @param T            The type to specialize the type trait \b %ResourcedTraits for.
   @param ResPool      Expression providing a pointer to the #"ResourcePool"
                       object.
   @param ResCategory  Expression providing the resource category.
   @param ResName      Expression providing the resource name.

\def ALIB_RESOURCED_IN_CAMP
   Variant of macro #"ALIB_RESOURCED", which specializes the type trait
   #"ResourcedTraits" for type \p{T} to use the resource-backend found in the
   given #"Camp", as well as its default #"^Camp::ResourceCategory".

   \par Availability
     This macro is available only if #"ALIB_CAMP" equals \c true.

   @param T       The type to specialize the type trait \b %ResourcedTraits for.
   @param Camp    Pointer to the #"Camp" that provides the
                  #"ResourcePool" object.
   @param ResName Expression providing the resource name.
                           
\def ALIB_CAMP_ENUM
  Combines macros #"ALIB_ENUMS_ASSIGN_RECORD" and #"ALIB_RESOURCED_IN_CAMP".
  @param T         The enumeration to assign a record and resources to.
  @param TRecord   The record type to assign.
  @param ResName   Resource name string.
  @param Camp      Pointer to the #"Camp" that provides the #"ResourcePool" object.
  
\def A_PATH
  Macro used to define #"system::Path"-string literals in a platform-independent way.
  Similar to macro #"A_CHAR", but dependent on the character type defined by the C++ standard
  library with type <c>std::filesystem::path::value_type</c>.<br>
  This is usually single-byte characters, and only on Windows OS, prefix <c>'L'</c> is added.
  @see Configuration macro #"ALIB_PATH_CHARACTERS_WIDE".
  @param literal The string literal to be eventually prefix with <c>'L'</c>.
  
\def ALIB_PATH_TO_NARROW
  Similar to #"ALIB_STRINGS_TO_NARROW". If #"PathCharType" does not
  equal #"characters::nchar", a local narrow string is created and the given path is converted.
  Otherwise, just a reference to the given path is created, which will ultimately be optimized
  out by the compiler.
  @param src     The name of the source instance.
  @param dest    The name of the destination instance (or reference to the source).
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer.
    
\def ALIB_PATH_TO_NARROW_ARG
  Similar to #"ALIB_STRINGS_TO_NARROW" and #"ALIB_STRINGS_TO_NARROW_ARG".
  Used to pass \alib path-strings as an argument when a narrow-string is expected.
  @param src     The name of the source instance.
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer.
  
\def ALIB_PATH_TO_STRING
  Similar to #"ALIB_STRINGS_TO_NARROW". If #"PathCharType" does not
  equal #"characters::character", a local string is created and the given path is converted.
  Otherwise, just a reference to the given path is created, which will ultimately be optimized
  out by the compiler.
  @param src     The name of the source instance.
  @param dest    The name of the destination instance (or reference to the source).
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer.
    
\def ALIB_PATH_TO_STRING_ARG
  Similar to #"ALIB_PATH_TO_STRING" and #"ALIB_STRINGS_TO_NARROW_ARG".
  Used to pass \alib path-strings as an argument when an \alib string is expected.
  @param src     The name of the source instance.
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer.

\def ALIB_STRING_TO_PATH
  Similar to #"ALIB_STRINGS_TO_NARROW". If #"PathCharType" does not
  equal #"characters::character", a local path-string is created and the given string is
  converted.
  Otherwise, just a reference to the given string is created, which will ultimately be optimized
  out by the compiler.
  @param src     The name of the source instance.
  @param dest    The name of the destination instance (or reference to the source).
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer.
    
\def ALIB_STRING_TO_PATH_ARG
  Similar to #"ALIB_STRINGS_TO_NARROW" and #"ALIB_STRINGS_TO_NARROW_ARG".
  Used to pass \alib strings as an argument when a path-string is expected.
  @param src     The name of the source instance.
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer.

*/
