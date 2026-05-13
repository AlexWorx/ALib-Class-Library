// #################################################################################################
//  Documentation - ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page   alib_macros         ALib and the C++ Preprocessor

\tableofcontents

\I{################################################################################################}
# 1\. On ALib Preprocessor Macros #
The \aliblong comes with a large number of preprocessor <c>#define</c> statements (around 350 at
the time of writing ). To keep this manageable, some rules have been adopted in 
respect to naming and grouping of the macros. 

##1\.1 Naming scheme #
Almost all configuration macros are written in \e "CAPITAL_SNAKE_CASE" and are prefixed by 
<b>"ALIB_"</b>. Most macros then continue with the name of the \alibmod that uses/defines them.

One important exception to this naming scheme is the macros defined by module \alib_alox,
which are prefixed <b>"ALOX_"</b>. Furthermore, the module \alib_alox_nl defines macros for 
logging that even use \e "CamelCase" names. 
    
\I{################################################################################################}
## 1\.2 Configuration Macros Vs. Macros ## {#alib_macros_types}
\alib separates two different kinds of macros:

### 1\.2.1 Configuration Macros ##
Those are macro-constants that are mainly used to select different versions of code, dependent on
- the platform,
- the <em>build type</em> (e.g., debug or release-builds),
- the \alibbuild, and
- the selection of \alib features.

This documentation sometimes refers to such macros as "code selection macros".
Many of the <em>configuration macros</em> can be passed to the compiler to overwrite 
their default value. This is usually done with the <b>-D</b>-option of a compiler's invocation 
command.

Besides the internal use to select \alib library code, the macros can be used with custom code
for the same purpose. This is especially necessary if custom software is designed to work with 
different \alibbuilds and different optional feature sets of \alib.

All configuration macros are documented in paragraph 2 of this manual page.

\I{################################################################################################}
### 1\.2.2 ALib Macros #
Preprocessor macros, which are \b not passable with the compiler's command line and which usually
are not just static constants but "real" macros that expand to code, are simply called "macros".  

The documentation of all \b macros is given in in paragraph 3 of this manual page.
                                                             
       
\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 2\. Configuration Macros # {#alib_macros_config}


\I{################################################################################################}
## 2\.1 General Configuration Macros (Not Related to a Specific Modules) ##
The following sections list configuration macros that are not related to a specific \alibmod.
This is the reference documentation for configuration macros that evaluate to \c true or \c false
(precisely \c 1 or \c 0). Unless explicitly mentioned, the macros may be passed to the compiler
to overwrite their documented default value.

\I{################################################################################################}
### 2\.1.1 Fundamental Configuration Macros # {#alib_macros_config_general}
The following macros are very fundamental to \alib:

- #"ALIB_DEBUG"
- #"ALIB_SINGLE_THREADED"
- #"ALIB_EXT_LIB_THREADS_AVAILABLE"
- #"ALIB_API_IS_DLL"
- #"ALIB_API_NO_DLL"
- #"DOXYGEN"
- #"ALIB_DEBUG_ASSERTION_PRINTABLES"
- #"ALIB_GTEST"
- #"ALIB_PRECOMPILED_HEADER"
- #"ALIB_C20_MODULES"


\I{################################################################################################}
### 2\...2 Configuration Macros Exposed By ALib ## {#alib_macros_config_general_deduced}
The following general configuration macros (aka macros not related to distinct \alibmods) are deduced by
\alib headers and thus usually are not to be passed to the compiler.

 \note Since \alib release 2312, the automatic detection of some macros may be disabled by passing
       them from outside. This allows compiling the code with platform/toolchain combinations that
       are not 'known' to \alib.
       See #"ALIB_SIZEOF_INTEGER" for more information.

- #"ALIB_CPP_STANDARD"
- #"ALIB_GCC"
- #"ALIB_SIZEOF_INTEGER"
- #"ALIB_INTGAP_TYPE"
- #"ALIB_SIZEOF_INTGAP"
- #"ALIB_SIZEOF_LONGDOUBLE_REPORTED"
- #"ALIB_SIZEOF_LONGDOUBLE_WRITTEN"


\I{################################################################################################}
### 2\.1.3 ALib Module Selection # {#alib_macros_config_modsel}

This group of configuration macros determine the availability of different <b>%ALib Modules</b> of the
\alibbuild used.
The macros are defined in the internal header #"F;alib.prepro.hpp;2", which always is and has to
be the first \alib header-file included (directly or indirectly).
If none of the macros is passed to the compiler, this header-file will define all to \c 1,
hence will include all modules an \alibbuild.

If a macro is given as \c 1, all corresponding macros of dependent modules are
also defined to \c 1, even if one had explicitly given as \c 0, which is considered a
contradicting input.

- #"ALIB_ALOX"
- #"ALIB_BITBUFFER"
- #"ALIB_APP"
- #"ALIB_BOXING"
- #"ALIB_CAMP"
- #"ALIB_FORMAT"
- #"ALIB_SYSTEM"
- #"ALIB_EXCEPTIONS"
- #"ALIB_RESOURCES"
- #"ALIB_VARIABLES"
- #"ALIB_CONTAINERS"
- #"ALIB_ENUMRECORDS"
- #"ALIB_EXPRESSIONS"
- #"ALIB_FILETREE"
- #"ALIB_MONOMEM"
- #"ALIB_SINGLETONS"
- #"ALIB_STRINGS"
- #"ALIB_THREADMODEL"

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
## 2\.2 Module-Specific Configuation Macros #
The following sections list configuration macros that are related to specific \alibmod.

\I{################################################################################################}
### 2\.2.1 ALib Module ALox Feature Selection # {#alib_macros_config_alox}

This group of code selection macros is defined with the use of module \alib_alox and
used to select the inclusion of log code. The macros are seldom to be used by end-user code.
Instead, the macros used for the log statements themselves (see
\ref alib_macros_mod_alox_debug "debug logging macros" and
\ref alib_macros_mod_alox_release "release logging macros".) are variably defined based on these macros.

- #"ALOX_DBG_LOG"
- #"ALOX_REL_LOG"
- #"ALOX_DBG_LOG_CI"
- #"ALOX_REL_LOG_CI"
                                                                      
\I{################################################################################################}
### 2\.2.2 ALib Module Basecamp Feature Selection # {#alib_macros_config_camp}

- #"ALIB_CAMP_OMIT_DEFAULT_RESOURCES"
- #"ALIB_DEBUG_RESOURCES"
- #"ALIB_PATH_CHARACTERS_WIDE"
                                       
\I{################################################################################################}
### 2\.2.3 ALib Module BitBuffer Feature Selection # {#alib_macros_config_bitbuffer}

- #"ALIB_DEBUG_ARRAY_COMPRESSION"


\I{################################################################################################}
### 2\.2.4 ALib Module Boxing Feature Selection # {#alib_macros_config_boxing}

- #"ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS"
- #"ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS"
- #"ALIB_FEAT_BOXING_BIJECTIVE_FLOATS"
- #"ALIB_DEBUG_BOXING"

\I{################################################################################################}
### 2\.2.5 ALib Module Characters Feature Selection # {#alib_macros_config_characters}

- #"ALIB_CHARACTERS_WIDE"
- #"ALIB_CHARACTERS_NATIVE_WCHAR"
- #"ALIB_CHARACTERS_SIZEOF_WCHAR"

\I{################################################################################################}
### 2\.2.6 ALib Module Containers Feature Selection # {#alib_macros_config_containers}

- #"ALIB_DEBUG_CONTAINERS"

\I{################################################################################################}
### 2\.2.7 ALib Module Monomem Feature Selection # {#alib_macros_config_monomem}

- #"ALIB_DEBUG_MEMORY"
- #"ALIB_DEBUG_ALLOCATIONS"
- #"ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT"
            
\I{################################################################################################}
### 2\.2.8 ALib Module Singletons Feature Selection # {#alib_macros_config_singletons}

- #"ALIB_FEAT_SINGLETON_MAPPED"


\I{################################################################################################}
### 2\.2.9 ALib Module Strings Feature Selection # {#alib_macros_config_strings}

- #"ALIB_FEAT_BOOST_REGEX"
- #"ALIB_DEBUG_STRINGS"

\I{################################################################################################}
### 2\.2.10 ALib Module System Feature Selection # {#alib_macros_config_files}

- #"ALIB_SYSTEM_FORCE_STD_FILE_STATUS"
- #"ALIB_SYSTEM_FILE_STATUS_IMPL"
- #"ALIB_SYSTEM_FILE_STD_STATUS"
- #"ALIB_SYSTEM_FILE_POSIX_STATUS"
 
\I{################################################################################################}
### 2\.2.11 ALib Module Threads Feature Selection # {#alib_macros_config_threads}

- #"ALIB_DEBUG_CRITICAL_SECTIONS"

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 3\. Macros #{#alib_macros_macros}
             
This section lists all preprocessor <em>Macros</em> of \alib used to generate code or prune code 
pieces. Often, the definition of macros depends on <em>Configuration Macros</em>, which have 
been documented in the previous section #"alib_macros_config". 



\I{################################################################################################}
## 3\.1 General Macros (Not Module-Related) # {#alib_macros_general}
The following sections list macros that are not related to a specific \alibmods:

\I{################################################################################################}
### 3\.1.1 ALib Version And Feature Verification # {#alib_macros_alib_version}
Macros used to verify that the \alib binary used has the right version is compiled with
the same feature set as the current compilation unit.
For this, version and feature flags are compiled statically into the library.
The macros are used internally, respectively 'automatically'. 
Hence, they need not be used directly by clients of the library.

- #"ALIB_VERSION"
- #"ALIB_REVISION"
- #"ALIB_COMPILATION_FLAGS"
- #"ALIB_ASSERT_MODULE"
- #"ALIB_BASE_DIR"

\I{################################################################################################}
### 3\.1.2  C/C++ Preprocessor Helper Macros # {#alib_macros_preprohelper}

The following macros are fundamental and building blocks for other macros.

- #"ALIB_STRINGIFY"
- #"ALIB_NSTRINGIFY"
- #"ALIB_CONCAT"
- #"ALIB_IDENTIFIER"
- #"ALIB_EMPTY"
- #"ALIB_COMMA"
- #"ALIB_COMMA_DBG"
- #"ALIB_STATIC_ASSERT"
- #"ALIB_STATIC_DENY"


\I{################################################################################################}
### 3\.1.3 C/C++ Language And Linkage Macros  # {#alib_macros_langlink}

The macros listed here are supporting C++ compilation and linking control and language
specific utilities.

- #"ALIB_DLL"
- #"ALIB_EXPORT"
- #"ALIB_ASSERT_GLOBAL_NAMESPACE"
- #"ALIB_SIZEOF_WCHAR_T"
- #"ALIB_CALLER_FUNC"
- #"ALIB_CALLER"
- #"ALIB_CALLER_PRUNED"
- #"ALIB_COMMA_CALLER_PRUNED"
- #"ALIB_CALLER_PRUNED_COMMA"
- #"ALIB_CALLER_NULLED"
- #"ALIB_COMMA_CALLER_NULLED"
- #"ALIB_CALLER_NULLED_COMMA"
- #"ALIB_DBG_TAKE_CI"
- #"bitsof(type)"
- #"ALIB_STACK_ALLOCATED_TYPE"

\I{################################################################################################}
### 3\.1.4 C/C++ Compiler Warning Control # {#alib_macros_warnings}

\alib sources are designed to be compiled on different compilers using the highest possible
compiler warning levels available.
In some situations, some warnings need to be temporarily disabled. The following macros do
this in a compiler agnostic way.

- #"ALIB_POP_ALLOWANCE"
- #"ALIB_ALLOW_UNINITIALIZED"
- #"ALIB_ALLOW_NULL_POINTER_PASSING"
- #"ALIB_ALLOW_UNREACHABLE_CODE"
- #"ALIB_ALLOW_UNUSED_MACRO"
- #"ALIB_ALLOW_MACROS_WITH_RESERVED_NAME"
- #"ALIB_ALLOW_MACRO_REDEFINITION"
- #"ALIB_ALLOW_MISSING_FIELD_INITIALIZERS"
- #"ALIB_ALLOW_UNSAFE_FUNCTION_OR_VARIABLE"
- #"ALIB_ALLOW_OVERLOADED_VIRTUALS"
- #"ALIB_ALLOW_SPARSE_ENUM_SWITCH"
- #"ALIB_ALLOW_BITWISE_SWITCH"
- #"ALIB_ALLOW_SWITCH_WITHOUT_DEFAULT"
- #"ALIB_ALLOW_DOCS"
- #"ALIB_ALLOW_SHIFT_COUNT_OVERFLOW"
- #"ALIB_ALLOW_DEPRECATED"
- #"ALIB_ALLOW_UNUSED_MACRO"
- #"ALIB_ALLOW_UNUSED_PARAMETER"
- #"ALIB_ALLOW_UNUSED_VARIABLE"
- #"ALIB_ALLOW_UNUSED_FUNCTION"
- #"ALIB_ALLOW_UNUSED_LAMBDA_CAPTURE"
- #"ALIB_ALLOW_FUNCTION_TEMPLATE"
- #"ALIB_ALLOW_RESERVED_IDENTIFIER"
- #"ALIB_ALLOW_NOT_ELIDING_COPY_ON_RETURN"
- #"ALIB_ALLOW_NOTHING_RETURNED"
- #"ALIB_FALLTHROUGH"
- #"ALIB_ALLOW_INTEGRAL_CONSTANT_OVERFLOW"
- #"ALIB_ALLOW_SIGN_CONVERSION"
- #"ALIB_ALLOW_INTEGER_OVERFLOW"

\I{################################################################################################}
### 3\.1.5 Debug Assertions, Warnings and Errors # {#alib_macros_dbgmessages}
These macros are used with debug-builds of \alib, respectively when macro #"ALIB_DEBUG"
is given.
They provide shortcuts into functions of the small foundational module #"alib_mod_assert".

If also module \alib_alox is compiled into the library, then a plug-in that uses
\alox for the message output is used.
See class  #"ALoxAssertionPlugin" for more information.

- #"ALIB_ERROR"
- #"ALIB_WARNING"
- #"ALIB_MESSAGE"
- #"ALIB_ASSERT"
- #"ALIB_ASSERT_ERROR"
- #"ALIB_ASSERT_WARNING"
- #"ALIB_ASSERT_MESSAGE"
- #"ALIB_ASSERT_RESULT_EQUALS"
- #"ALIB_ASSERT_RESULT_NOT_EQUALS"
- #"ALIB_ASSERT_RESULT_GREATER_THAN"
- #"ALIB_ASSERT_RESULT_LESS_THAN"


\I{################################################################################################}
### 3\.1.6 C++ Template Programming Tools  # {#alib_macros_templ_tools}

Macros for handling template types.

- #"ALIB_TVALUE"
- #"ALIB_HAS_METHOD"
  
\note With the switch to C++20 and using concepts, this section was largely reduced.

\I{################################################################################################}
### 3\.1.7 Macros for Resource Locking and Recursive Programming # {#alib_macros_locksandrecursion}
The macros listed here have a direct relationship with classes defined in \alib
and with their use.

- #"ALIB_OWN"
- #"ALIB_OWN_RECURSIVE"
- #"ALIB_OWN_SHARED"
- #"ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS"
- #"ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL"

\I{##############################    Code Selection / Pruning       #############################  }

### 3\.1.8 Code Selection / Pruning # {#alib_macros_pruning}
The macros listed here include or prune code, dependent on different configuration macros.
They are mainly given to avoid <c>\#if</c>/<c>\#else</c>/<c>\#endif</c> statements for small
code pieces.

- #"ALIB_DBG"
- #"ALIB_REL_DBG"
- #"ALIB_REL"
- #"ALIB_CPP_23"
- #"ALIB_CPP_BEFORE_23"
- #"IF_ALIB_ALOX"
- #"IFNOT_ALIB_ALOX"
- #"IF_ALIB_APP"
- #"IFNOT_ALIB_APP"
- #"IF_ALIB_BOXING"
- #"IFNOT_ALIB_BOXING"
- #"IF_ALIB_BITBUFFER"
- #"IFNOT_ALIB_BITBUFFER"
- #"IF_ALIB_VARIABLES"
- #"IFNOT_ALIB_VARIABLES"
- #"IF_ALIB_CONTAINERS"
- #"IFNOT_ALIB_CONTAINERS"
- #"IF_ALIB_CAMP"
- #"IFNOT_ALIB_CAMP"
- #"IF_ALIB_ENUMRECORDS"
- #"IFNOT_ALIB_ENUMRECORDS"
- #"IF_ALIB_EXPRESSIONS"
- #"IFNOT_ALIB_EXPRESSIONS"
- #"IF_ALIB_FILETREE"
- #"IFNOT_ALIB_FILETREE"
- #"IF_ALIB_THREADMODEL"
- #"IFNOT_ALIB_THREADMODEL"
- #"IF_ALIB_THREADS"
- #"IFNOT_ALIB_THREADS"
- #"IF_ALIB_STRINGS"
- #"IFNOT_ALIB_STRINGS"
- #"IF_ALIB_MONOMEM"
- #"IFNOT_ALIB_MONOMEM"
- #"IF_ALIB_RESOURCES"
- #"IFNOT_ALIB_RESOURCES"
- #"IF_ALIB_EXCEPTIONS"
- #"IFNOT_ALIB_EXCEPTIONS"
- #"IF_ALIB_SYSTEM"
- #"IFNOT_ALIB_SYSTEM"
- #"IF_ALIB_FORMAT"
- #"IFNOT_ALIB_FORMAT"
- #"IF_ALIB_SINGLETONS"
- #"IFNOT_ALIB_SINGLETONS"


\I{################################################################################################}
\I{###########################   Module-Specific Macros     ##############################  }
\I{################################################################################################}
## 3.2 Module-Specific Macros # {#alib_macros_modspecific}
The following sections list macros that specific to  \alibmods.

#########################################   Module-ALox   #########################################}
### 3.2.1 Macros Of Module ALox # {#alib_macros_mod_alox}
#### 3.2.1.1 Macros For Debug Logging # {#alib_macros_mod_alox_debug}
The macros listed here, are provided to place debug <em>Log Statements</em> within source code
using module \alib_alox.
Besides that, macros controlling and setting preferences for \alox exists.<p>

The exclusive use of these macros should be sufficient to support most of common debug logging
statements with \alox. Should some functionality be not available easily with using the macros,
of course, the normal C++ API of \alox can be used in parallel to using the macros.
For proper pruning of code that is using the C++ API, such code has to be enclosed by<br>
\verbatim
#if ALOX_DBG_LOG
   ...
   ...
#endif
\endverbatim
lines, or embedded in macro #"Log_Prune".
\note To be clear: When using the macros, the <em>\#if / \#endif</em> statements are \e not needed!

All macro names are prefixed "Log_". This implies that they are macros to implement *debug*
logging. In contrast to this, a set of similar macros exists for release logging
(see \ref alib_macros_mod_alox_release "release logging macros"). Those are
prefixed "Lox_". (The choice of the prefixes *Log_* and *Lox* provide maximum source code
compatibility of <b>%ALox for C++</b> log lines in comparison to ALox for C# and ALox for Java.<p>

Most macros make use of the macro #"LOG_LOX", which references a singleton object of the class 
#"Lox" that is used for all debug logging.
This singleton concept covers most use cases for debug logging. If more flexibility is wanted, then either macro
#"LOG_LOX" might be changed for different compilation units or the <b>%ALox for C++ API</b>
might be used instead of the macros listed here.

\note The <em>Scope Domain</em> mechanism of \alox, as well as other \alox functionality
which relies on \e Scopes use the information provided by \alib macro #"ALIB_CALLER".
Changing this macro, might cause these mechanism to fail.


- #"Log_Prune"
- #"Log_SetSourcePathTrimRule"
- #"Log_ClearSourcePathTrimRules"
- #"Log_AddDebugLogger"
- #"Log_RemoveDebugLogger"
- #"Log_GetLogger"
- #"Log_RemoveLogger"
- #"Log_SetDomain"
- #"Log_RemoveThreadDomain"
- #"Log_SetDomainSubstitutionRule"
- #"Log_SetVerbosity"
- #"Log_SetStartTime"
- #"Log_MapThreadName"
- #"Log_LogState"
- #"Log_Verbose"
- #"Log_Info"
- #"Log_Warning"
- #"Log_Error"
- #"Log_Assert"
- #"Log_If"
- #"Log_Once"
- #"Log_Entry"
- #"Log_SetPrefix"
- #"Log_Store"
- #"Log_Retrieve"
- #"Log_IsActive"
- #"Log_GetVerbosity"
- #"Log_SetVerbosityExport"
- #"Log_Exception"

\I{###########################   ALox Macros For Release Logging   ##############################  }
#### 3.2.1.2 Macros For Release Logging # {#alib_macros_mod_alox_release}
The macros listed here are provided to place release <em>Log Statements</em> within source code
using module \alib_alox. Besides that, macros controlling and setting preferences for ALox exists.<p>

The exclusive use of these macros should be sufficient to support most of common release logging
statements with \alox. Should some functionality be not available easily using the macros,
of course, the normal C++ API of \alox can be used in parallel to using the macros.
Code that is using the C++ API might be enclosed by preprocessor directives<br>
\verbatim
#if ALOX_REL_LOG
   ...
   ...
#endif
\endverbatim
to remove them when compiling a release version of the software unit with pruned
release log macros. Alternatively, such code might be embedded in macro #"Lox_Prune".
(Pruning of release logging can be enabled by defining the configuration macro #"ALOX_REL_LOG" to \c 0
and could be useful in certain situations.)

Before using the macros, each code entity has to set the preprocessor macro #"LOX_LOX"
This can be done in a general header-file of the software,
(e.g., the same that exposes the release-Lox object to that source), or, in more complex
scenarios with more than one release-Lox object, at any other appropriate source location.

All macro names are prefixed with the term <b>Lox_</b>. This implies that they are macros to
implement <em>release logging</em>. In contrast to this, a set of similar macros exists for
<em>debug logging</em> (see \ref alib_macros_mod_alox_debug "debug logging macros"). Those are
prefixed <b>Log_</b>.
\note The choice of the prefixes <b>Log_</b> and <b>Lox_</b> was made to provide  maximum source
code compatibility of <b>%ALox for C++</b> log lines in comparison to other ALox implementations
in other programming languages, which provide a class Log as a kind of 'mirror' class of
class Lox. Invocations to class Log get pruned in these languages. <p>

Most macros make use of the macro #"LOX_LOX", which references a singleton
object of class #"Lox" that is used for all release logging.
This singleton concept covers most
use cases for release logging. If more flexibility is wanted, then either the macro
#"LOX_LOX" might be changed for different compilation units or the <b>%ALox C++ API</b>
might be used instead of the macros listed here.


- #"Lox_Prune"
- #"Lox_SetSourcePathTrimRule"
- #"Lox_ClearSourcePathTrimRules"
- #"Lox_GetLogger"
- #"Lox_RemoveLogger"
- #"Lox_SetDomain"
- #"Lox_RemoveThreadDomain"
- #"Lox_SetDomainSubstitutionRule"
- #"Lox_SetVerbosity"
- #"Lox_SetStartTime"
- #"Lox_MapThreadName"
- #"Lox_LogState"
- #"Lox_Verbose"
- #"Lox_Info"
- #"Lox_Warning"
- #"Lox_Error"
- #"Lox_Assert"
- #"Lox_If"
- #"Lox_Once"
- #"Lox_Entry"
- #"Lox_Store"
- #"Lox_Retrieve"
- #"Lox_SetPrefix"
- #"Lox_IsActive"
- #"Lox_GetVerbosity"
- #"Lox_SetVerbosityExport"
- #"Lox_Exception"

\I{################################################################################################}
\I{###############################    ALox Low-Level Macros    ##################################  }
\I{################################################################################################}
#### 3.2.1.3 Low-level ALox Macros  # {#alib_macros_mod_alox_lowlevel}

The preprocessor macros listed here are the foundation for
\ref alib_macros_mod_alox_debug "debug logging-" and
\ref alib_macros_mod_alox_release "release logging macros" introduced below.

Developers that use \alox in standard use cases do not need to know and use the macros listed here.

- #"LOG_LOX"
- #"LOX_LOX"
- #"LOG_CI"
- #"LOX_CI"
- #"LOG_ACQUIRE"
- #"LOG_RELEASE"
- #"LOX_ACQUIRE"
- #"LOX_RELEASE"

\I{################################     ALib Module Basecamp       ##############################  }
### 3.2.2 Macros Of Module Basecamp # {#alib_macros_mod_camp}
The macros in this section are introduced by module \alib_resources.

- #"ALIB_RESOURCED"
- #"ALIB_RESOURCED_IN_CAMP"
- #"ALIB_CAMP_ENUM"
- #"A_PATH"
- #"ALIB_PATH_TO_NARROW"
- #"ALIB_PATH_TO_NARROW_ARG"
- #"ALIB_PATH_TO_STRING"
- #"ALIB_PATH_TO_STRING_ARG"
- #"ALIB_STRING_TO_PATH"
- #"ALIB_STRING_TO_PATH_ARG"

\I{#############################      ALib Module Boxing       ##################################  }
\I{################################################################################################}

### 3.2.3 Macros Of Module Boxing # {#alib_macros_mod_boxing}
The macros in this section are introduced by module \alib_boxing.

- #"ALIB_BOXING_CUSTOMIZE"
- #"ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING"
- #"ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE"
- #"ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE"
- #"ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE_NON_UNBOXABLE"
- #"ALIB_BOXING_CUSTOMIZE_DENY_BOXING"
- #"ALIB_BOXING_VTABLE_DECLARE"
- #"ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE"
- #"ALIB_BOXING_VTABLE_DEFINE"
- #"ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE"
- #"ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER"
- #"ALIB_BOXING_DEFINE_FEQUALS_FOR_COMPARABLE_TYPE"
- #"ALIB_BOXING_DEFINE_FISLESS_FOR_COMPARABLE_TYPE"
- #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE"
- #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N"
- #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W"
- #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X"


\I{################################     ALib Module Characters      #############################  }
\I{################################################################################################}

### 3.2.4 Macros Of Module Characters # {#alib_macros_mod_characters}
The macros in this section are introduced by module \alib_characters.

- #"A_CHAR"
- #"A_CCHAR"
- #"A_SCHAR"
- #"A_NCHAR"
- #"A_WCHAR"
- #"A_XCHAR"
- #"ALIB_CHAR_TYPE_ID_N"
- #"ALIB_CHAR_TYPE_ID_W"
- #"ALIB_CHAR_TYPE_ID_X"
- #"ALIB_CHAR_TYPE_ID_CHARACTER"
- #"ALIB_CHAR_TYPE_ID_COMPLEMENT"
- #"ALIB_CHAR_TYPE_ID_STRANGE"
- #"ALIB_CHARACTER_ARRAY"
- #"ALIB_CHARACTER_ARRAY_MUTABLE"
- #"ALIB_CHARACTER_ZT_ARRAY"
- #"ALIB_CHARACTER_ZT_ARRAY_MUTABLE"
- #"ALIB_CHARACTER_ARRAY_IMPL_BUFFER"
- #"ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER"
- #"ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE"
- #"ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE"
- #"ALIB_CHARACTER_ARRAY_IMPL_LENGTH"
- #"ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH"
- #"ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE"
- #"ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE"
- #"ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT"
- #"ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT"

\I{############################      ALib Module Config        ##################################  }
### 3.2.5 Macros Of Module Config # {#alib_macros_mod_config}
The macros in this section are introduced by \alibcamp \alib_variables.

- #"ALIB_VARIABLES_DEFINE_TYPE"
- #"ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR"
- #"ALIB_VARIABLES_REGISTER_TYPE(CPPName)"

\I{#############################      ALib Module Enums      ####################################  }
### 3.2.6 Macros Of Module Enums # {#alib_macros_mod_enums}

The macros in this section are introduced by modules \alib_enumops and \alib_enumrecords.
They allow specializing the following type traits for scoped or non-scoped enum types:
#"RecordsTraits",
#"ArithmeticalTraits",
#"BitwiseTraits" and
#"IterableTraits"

\attention
- All macros in this section have to be placed in global scope (outside namespaces and types).
- For technical reasons, neither the macros nor the concepts behind them are applicable to
  enum types that are defined as \c private or \c protected inner types of structs or classes.

- #"ALIB_ENUMS_ASSIGN_RECORD"
- #"ALIB_ENUMS_MAKE_ARITHMETICAL"
- #"ALIB_ENUMS_MAKE_BITWISE"
- #"ALIB_ENUMS_MAKE_ITERABLE"
- #"ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END"

\I{############################     ALib Module Expressions     #################################  }
### 3.2.7 Macros Of Module Expressions # {#alib_macros_mod_expressions}
The macros in this section are introduced by module \alib_expressions.

- #"CALCULUS_CALLBACK"
- #"CALCULUS_DEFAULT_AUTOCAST"
- #"CALCULUS_SIGNATURE"


\I{############################      ALib Module Strings       ##################################  }
### 3.2.8 Macros Of Module Strings # {#alib_macros_mod_strings}
The macros in this section are introduced by module \alib_strings.

- #"ALIB_STRINGS_APPENDABLE_TYPE"
- #"ALIB_STRINGS_APPENDABLE_TYPE_N"
- #"ALIB_STRINGS_APPENDABLE_TYPE_DEF"
- #"ALIB_STRINGS_APPENDABLE_TYPE_DEF_N"
- #"ALIB_STRINGS_APPENDABLE_TYPE_DEF_W"
- #"ALIB_STRINGS_APPENDABLE_TYPE_INLINE"
- #"ALIB_STRINGS_APPENDABLE_TYPE_INLINE_N"
- #"ALIB_STRINGS_APPENDABLE_TYPE_INLINE_W"
- #"ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR"
- #"ALIB_STRING_DBG_CHK"
- #"ALIB_STRINGS_TO_NARROW"
- #"ALIB_STRINGS_FROM_NARROW"
- #"ALIB_STRINGS_TO_NARROW_ARG"
- #"ALIB_STRINGS_FROM_NARROW_ARG"
- #"ALIB_STRINGS_TO_WIDE"
- #"ALIB_STRINGS_FROM_WIDE"
- #"ALIB_STRINGS_TO_WIDE_ARG"
- #"ALIB_STRINGS_FROM_WIDE_ARG"
- #"ALIB_STRING_RESETTER"
                              
\I{############################      ALib Module Threads       ##################################  }
### 3.2.9 Macros Of Module Threads # {#alib_macros_mod_threads}
While macros in this section logically belong to module \alib_threads, they are available
(but empty) also when this module is not included in the \alibbuild.
An explanation to this is given with chapter #"alib_threads_intro_agnostic"
of this module's Programmer's Manual.

Their definition depends on the setting of #"ALIB_DEBUG_CRITICAL_SECTIONS", which defaults
to \c 1, in case module \alib_threads_nl is included in the \alibbuild_nl.

If the macro is not set, all the macros in this section are empty.

- #"ALIB_LOCK"
- #"ALIB_LOCK_WITH"
- #"ALIB_LOCK_RECURSIVE"
- #"ALIB_LOCK_RECURSIVE_WITH"
- #"ALIB_LOCK_SHARED"
- #"ALIB_LOCK_SHARED_WITH"
- #"ALIB_DCS"
- #"ALIB_DCS_WITH"
- #"ALIB_DCS_SHARED"
- #"ALIB_DCS_SHARED_WITH"
- #"ALIB_DCS_ACQUIRE"
- #"ALIB_DCS_ACQUIRE_WITH"
- #"ALIB_DCS_ACQUIRE_SHARED"
- #"ALIB_DCS_ACQUIRE_SHARED_WITH"
- #"ALIB_DCS_RELEASE"
- #"ALIB_DCS_RELEASE_WITH"
- #"ALIB_DCS_RELEASE_SHARED"
- #"ALIB_DCS_RELEASE_SHARED_WITH"


*/
