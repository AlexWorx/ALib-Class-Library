// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_PREDEF_MODULES
#define HPP_ALIB_LIB_PREDEF_MODULES 1

#define ALIB_VERSION              1903
#define ALIB_REVISION                0


// #################################################################################################
// Check that no code selection symbol was given
// #################################################################################################

#if defined(ALIB_MODULE_ALOX)
#   error "Code selection symbol ALIB_MODULE_ALOX is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_EXPRESSIONS)
#   error "Code selection symbol IB_MODULE_EXPRESSIONS is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_CONFIGURATION)
#   error "Code selection symbol ALIB_MODULE_CONFIGURATION is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_CLI)
#   error "Code selection symbol ALIB_MODULE_CLI is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_SYSTEM)
#   error "Code selection symbol ALIB_MODULE_SYSTEM is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_RESULTS)
#   error "Code selection symbol ALIB_MODULE_RESULTS is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_STRINGFORMAT)
#   error "Code selection symbol ALIB_MODULE_STRINGFORMAT is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_RESOURCES)
#   error "Code selection symbol ALIB_MODULE_RESOURCES is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_THREADS)
#   error "Code selection symbol ALIB_MODULE_THREADS is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_STRINGS)
#   error "Code selection symbol ALIB_MODULE_STRINGS is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_BOXING)
#   error "Code selection symbol ALIB_MODULE_BOXING is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_TIME)
#   error "Code selection symbol ALIB_MODULE_TIME is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_CHARACTERS)
#   error "Code selection symbol ALIB_MODULE_CHARACTERS is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_ENUMS)
#   error "Code selection symbol ALIB_MODULE_ENUMS is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_SINGLETONS)
#   error "Code selection symbol ALIB_MODULE_SINGLETONS is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

#if defined(ALIB_MODULE_MEMORY)
#   error "Code selection symbol ALIB_MODULE_MEMORY is given. Use corresponding compiler symbol with postfix '_ON' or '_OFF' instead."
#endif

// #################################################################################################
// Check that not both compiler symbols were given
// #################################################################################################

#if defined(ALIB_MODULE_ALOX_ON) && defined(ALIB_MODULE_ALOX_OFF)
#   error "Code selection symbols ALIB_MODULE_ALOX_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_EXPRESSIONS_ON) && defined(ALIB_MODULE_EXPRESSIONS_OFF)
#   error "Code selection symbols IB_MODULE_EXPRESSIONS_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_CONFIGURATION_ON) && defined(ALIB_MODULE_CONFIGURATION_OFF)
#   error "Code selection symbols ALIB_MODULE_CONFIGURATION_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_CLI_ON) && defined(ALIB_MODULE_CLI_OFF)
#   error "Code selection symbols ALIB_MODULE_CLI_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_SYSTEM_ON) && defined(ALIB_MODULE_SYSTEM_OFF)
#   error "Code selection symbols ALIB_MODULE_SYSTEM_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_RESULTS_ON) && defined(ALIB_MODULE_RESULTS_OFF)
#   error "Code selection symbols ALIB_MODULE_RESULTS_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_STRINGFORMAT_ON) && defined(ALIB_MODULE_STRINGFORMAT_OFF)
#   error "Code selection symbols ALIB_MODULE_STRINGFORMAT_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_RESOURCES_ON) && defined(ALIB_MODULE_RESOURCES_OFF)
#   error "Code selection symbols ALIB_MODULE_RESOURCES_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_THREADS_ON) && defined(ALIB_MODULE_THREADS_OFF)
#   error "Code selection symbols ALIB_MODULE_THREADS_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_STRINGS_ON) && defined(ALIB_MODULE_STRINGS_OFF)
#   error "Code selection symbols ALIB_MODULE_STRINGS_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_BOXING_ON) && defined(ALIB_MODULE_BOXING_OFF)
#   error "Code selection symbols ALIB_MODULE_BOXING_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_TIME_ON) && defined(ALIB_MODULE_TIME_OFF)
#   error "Code selection symbols ALIB_MODULE_TIME_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_CHARACTERS_ON) && defined(ALIB_MODULE_CHARACTERS_OFF)
#   error "Code selection symbols ALIB_MODULE_CHARACTERS_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_ENUMS_ON) && defined(ALIB_MODULE_ENUMS_OFF)
#   error "Code selection symbols ALIB_MODULE_ENUMS_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_SINGLETONS_ON) && defined(ALIB_MODULE_SINGLETONS_OFF)
#   error "Code selection symbols ALIB_MODULE_SINGLETONS_ON/_OFF are both given."
#endif

#if defined(ALIB_MODULE_MEMORY_ON) && defined(ALIB_MODULE_MEMORY_OFF)
#   error "Code selection symbols ALIB_MODULE_MEMORY_ON/_OFF are both given."
#endif

// #################################################################################################
// Set modules as given
// #################################################################################################
#if defined(ALIB_MODULE_ALOX_ON)
#   define   ALIB_MODULE_ALOX          1
#else
#   define   ALIB_MODULE_ALOX          0
#endif

#if defined(ALIB_MODULE_EXPRESSIONS_ON)
#   define ALIB_MODULE_EXPRESSIONS     1
#else
#   define ALIB_MODULE_EXPRESSIONS     0
#endif

#if defined(ALIB_MODULE_CONFIGURATION_ON)
#   define   ALIB_MODULE_CONFIGURATION 1
#else
#   define   ALIB_MODULE_CONFIGURATION 0
#endif

#if defined(ALIB_MODULE_CLI_ON)
#   define   ALIB_MODULE_CLI           1
#else
#   define   ALIB_MODULE_CLI           0
#endif

#if defined(ALIB_MODULE_SYSTEM_ON)
#   define   ALIB_MODULE_SYSTEM        1
#else
#   define   ALIB_MODULE_SYSTEM        0
#endif

#if defined(ALIB_MODULE_RESULTS_ON)
#   define   ALIB_MODULE_RESULTS       1
#else
#   define   ALIB_MODULE_RESULTS       0
#endif

#if defined(ALIB_MODULE_STRINGFORMAT_ON)
#   define   ALIB_MODULE_STRINGFORMAT  1
#else
#   define   ALIB_MODULE_STRINGFORMAT  0
#endif

#if defined(ALIB_MODULE_RESOURCES_ON)
#   define   ALIB_MODULE_RESOURCES     1
#else
#   define   ALIB_MODULE_RESOURCES     0
#endif

#if defined(ALIB_MODULE_THREADS_ON)
#   define   ALIB_MODULE_THREADS       1
#else
#   define   ALIB_MODULE_THREADS       0
#endif

#if defined(ALIB_MODULE_STRINGS_ON)
#   define   ALIB_MODULE_STRINGS       1
#else
#   define   ALIB_MODULE_STRINGS       0
#endif

#if defined(ALIB_MODULE_BOXING_ON)
#   define   ALIB_MODULE_BOXING        1
#else
#   define   ALIB_MODULE_BOXING        0
#endif

#if defined(ALIB_MODULE_TIME_ON)
#   define   ALIB_MODULE_TIME          1
#else
#   define   ALIB_MODULE_TIME          0
#endif

#if defined(ALIB_MODULE_CHARACTERS_ON)
#   define   ALIB_MODULE_CHARACTERS    1
#else
#   define   ALIB_MODULE_CHARACTERS    0
#endif

#if defined(ALIB_MODULE_ENUMS_ON)
#   define   ALIB_MODULE_ENUMS         1
#else
#   define   ALIB_MODULE_ENUMS         0
#endif

#if defined(ALIB_MODULE_SINGLETONS_ON)
#   define   ALIB_MODULE_SINGLETONS    1
#else
#   define   ALIB_MODULE_SINGLETONS    0
#endif

#if defined(ALIB_MODULE_MEMORY_ON)
#   define   ALIB_MODULE_MEMORY        1
#else
#   define   ALIB_MODULE_MEMORY        0
#endif

// No module selection symbol given? -> choose ALL
#if !(  ALIB_MODULE_ALOX                \
      | ALIB_MODULE_EXPRESSIONS         \
      | ALIB_MODULE_CONFIGURATION       \
      | ALIB_MODULE_CLI                 \
      | ALIB_MODULE_SYSTEM              \
      | ALIB_MODULE_RESULTS             \
      | ALIB_MODULE_STRINGFORMAT        \
      | ALIB_MODULE_RESOURCES           \
      | ALIB_MODULE_THREADS             \
      | ALIB_MODULE_STRINGS             \
      | ALIB_MODULE_BOXING              \
      | ALIB_MODULE_TIME                \
      | ALIB_MODULE_CHARACTERS          \
      | ALIB_MODULE_ENUMS               \
      | ALIB_MODULE_SINGLETONS          \
      | ALIB_MODULE_MEMORY              )
#   undef   ALIB_MODULE_ALOX
#   define  ALIB_MODULE_ALOX                1
#   undef   ALIB_MODULE_EXPRESSIONS
#   define  ALIB_MODULE_EXPRESSIONS         1
#   undef   ALIB_MODULE_CLI
#   define  ALIB_MODULE_CLI                 1
#endif

// #################################################################################################
// Resolve module dependencies
// #################################################################################################
#if ALIB_MODULE_EXPRESSIONS
#    undef  ALIB_MODULE_RESULTS
#    define ALIB_MODULE_RESULTS             1
#endif
#if ALIB_MODULE_ALOX
#    undef  ALIB_MODULE_CONFIGURATION
#    define ALIB_MODULE_CONFIGURATION       1
#    undef  ALIB_MODULE_THREADS
#    define ALIB_MODULE_THREADS             1
#endif
#if ALIB_MODULE_CONFIGURATION
#    undef  ALIB_MODULE_SYSTEM
#    define ALIB_MODULE_SYSTEM              1
#endif
#if ALIB_MODULE_SYSTEM
#    undef  ALIB_MODULE_RESULTS
#    define ALIB_MODULE_RESULTS             1
#    undef  ALIB_MODULE_TIME
#    define ALIB_MODULE_TIME                1
#endif
#if ALIB_MODULE_CLI
#    undef  ALIB_MODULE_RESULTS
#    define ALIB_MODULE_RESULTS             1
#endif
#if ALIB_MODULE_RESULTS
#    undef  ALIB_MODULE_STRINGFORMAT
#    define ALIB_MODULE_STRINGFORMAT        1
#endif
#if ALIB_MODULE_STRINGFORMAT
#    undef  ALIB_MODULE_RESULTS
#    define ALIB_MODULE_RESULTS             1
#    undef  ALIB_MODULE_STRINGS
#    define ALIB_MODULE_STRINGS             1
#    undef  ALIB_MODULE_BOXING
#    define ALIB_MODULE_BOXING              1
#    undef  ALIB_MODULE_RESOURCES
#    define ALIB_MODULE_RESOURCES           1
#    undef  ALIB_MODULE_ENUMS
#    define ALIB_MODULE_ENUMS               1
#endif
#if ALIB_MODULE_RESOURCES
#    undef  ALIB_MODULE_STRINGS
#    define ALIB_MODULE_STRINGS             1
#    undef  ALIB_MODULE_MEMORY
#    define ALIB_MODULE_MEMORY              1
#endif
#if ALIB_MODULE_THREADS
#    undef  ALIB_MODULE_STRINGS
#    define ALIB_MODULE_STRINGS             1
#    undef  ALIB_MODULE_TIME
#    define ALIB_MODULE_TIME                1
#endif
#if ALIB_MODULE_STRINGS
#    undef  ALIB_MODULE_CHARACTERS
#    define ALIB_MODULE_CHARACTERS          1
#endif
#if ALIB_MODULE_BOXING
#    undef  ALIB_MODULE_SINGLETONS
#    define ALIB_MODULE_SINGLETONS          1
#    undef  ALIB_MODULE_CHARACTERS
#    define ALIB_MODULE_CHARACTERS          1
#endif


// #################################################################################################
// Resolve file-set dependencies
// #################################################################################################
#define ALIB_FILESET_PREDEF_PF        (  ALIB_MODULE_CHARACTERS       \
                                       | ALIB_MODULE_BOXING           \
                                       | ALIB_MODULE_MEMORY           )

#define ALIB_FILESET_PREDEF_TMP       (  ALIB_MODULE_ENUMS            \
                                       | ALIB_MODULE_CHARACTERS       )

#define ALIB_FILESET_TYPEMAP          (  ALIB_MODULE_SINGLETONS       )

#define ALIB_FILESET_INTEGERS         (  ALIB_MODULE_CHARACTERS       \
                                       | ALIB_MODULE_BOXING           \
                                       | ALIB_MODULE_MEMORY           \
                                       | ALIB_MODULE_TIME             )

#define ALIB_FILESET_DEBUG            (  ALIB_MODULE_CHARACTERS       \
                                       | ALIB_MODULE_BOXING           \
                                       | ALIB_MODULE_MEMORY           )

#define ALIB_FILESET_OWNER            (  ALIB_MODULE_THREADS          \
                                       | ALIB_MODULE_STRINGFORMAT     )

#define ALIB_FILESET_COMMON_ENUMS     (  ALIB_MODULE_CHARACTERS       \
                                       | ALIB_MODULE_BOXING           \
                                       | ALIB_MODULE_STRINGS          \
                                       | ALIB_MODULE_TIME             )

#define ALIB_FILESET_MODULES          (  ALIB_MODULE_STRINGFORMAT     )

#define ALIB_FILESET_PLUGINS          (  ALIB_MODULE_CONFIGURATION    \
                                       | ALIB_MODULE_EXPRESSIONS      )


// #################################################################################################
// Macros for checking availability of modules and filesets
// #################################################################################################
#define  ALIB_DOCUMENTATION_URL   "https://alexworx.github.io/ALib-Class-Library/"

#define ALIB_ASSERT_MODULE(modulename)                                                             \
static_assert( ALIB_MODULE_ ## modulename,                                                         \
               "This module is not included in the ALib distribution/built. "                      \
               "See " ALIB_DOCUMENTATION_URL "alib_manual.html for more information" );            \

#define ALIB_ASSERT_FILESET(filesetname)                                                           \
static_assert( ALIB_FILESET_ ## filesetname,                                                       \
               "This file is not included in the ALib distribution/built. "                        \
               "See " ALIB_DOCUMENTATION_URL "alib_manual.html for more information" );            \

#endif // HPP_ALIB_LIB_PREDEF_MODULES

