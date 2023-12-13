/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * This header does neither belong to a fileset, nor to a module of \alib, but is
 * included in any \alibdist. Its purpose is to identify given \ref GrpALibPreproSymbols
 * related to module selection and enable corresponding symbols (that may be not given) of
 * dependent modules.
 *
 * \note
 *   This header is not shown in inclusion graphs of this documentation, as it is always
 *   included directly or indirectly.
 **************************************************************************************************/
#ifndef HPP_ALIB_MODULES
#define HPP_ALIB_MODULES 1

#define ALIB_VERSION              2312
#define ALIB_REVISION             0

#if !defined(ALIB_DOX)

// #################################################################################################
// No module selection symbol given but threads? -> choose ALL
// #################################################################################################
#if     !defined( ALIB_ALOX         )       \
     && !defined( ALIB_BITBUFFER    )       \
     && !defined( ALIB_BOXING       )       \
     && !defined( ALIB_CHARACTERS   )       \
     && !defined( ALIB_CLI          )       \
     && !defined( ALIB_CONFIGURATION)       \
     && !defined( ALIB_ENUMS        )       \
     && !defined( ALIB_EXPRESSIONS  )       \
     && !defined( ALIB_MONOMEM      )       \
     && !defined( ALIB_RESOURCES    )       \
     && !defined( ALIB_RESULTS      )       \
     && !defined( ALIB_SINGLETONS   )       \
     && !defined( ALIB_STRINGS      )       \
     && !defined( ALIB_SYSTEM       )       \
     && !defined( ALIB_TEXT         )       \
     && !defined( ALIB_TIME         )
#   define  ALIB_ALOX                1
#   define  ALIB_EXPRESSIONS         1
#   define  ALIB_CLI                 1
#   define  ALIB_THREADS             1
#   define  ALIB_BITBUFFER           1
#endif

#if !defined(ALIB_ALOX         )
#   define ALIB_ALOX                0
#endif
#if !defined(ALIB_BITBUFFER    )
#   define ALIB_BITBUFFER           0
#endif
#if !defined(ALIB_BOXING       )
#   define ALIB_BOXING              0
#endif
#if !defined(ALIB_CLI          )
#   define ALIB_CLI                 0
#endif
#if !defined(ALIB_CHARACTERS   )
#   define ALIB_CHARACTERS          0
#endif
#if !defined(ALIB_CONFIGURATION)
#   define ALIB_CONFIGURATION       0
#endif
#if !defined(ALIB_ENUMS        )
#   define ALIB_ENUMS               0
#endif
#if !defined(ALIB_EXPRESSIONS  )
#   define ALIB_EXPRESSIONS         0
#endif
#if !defined(ALIB_MONOMEM      )
#   define ALIB_MONOMEM              0
#endif
#if !defined(ALIB_RESOURCES    )
#   define ALIB_RESOURCES           0
#endif
#if !defined(ALIB_RESULTS      )
#   define ALIB_RESULTS             0
#endif
#if !defined(ALIB_SINGLETONS   )
#   define ALIB_SINGLETONS          0
#endif
#if !defined(ALIB_STRINGS      )
#   define ALIB_STRINGS             0
#endif
#if !defined(ALIB_SYSTEM       )
#   define ALIB_SYSTEM              0
#endif
#if !defined(ALIB_TEXT         )
#   define ALIB_TEXT                0
#endif
#if !defined(ALIB_TIME         )
#   define ALIB_TIME                0
#endif
#if !defined( ALIB_THREADS     )
#   define  ALIB_THREADS            0
#endif


// #################################################################################################
// Resolve module dependencies
// #################################################################################################
#if ALIB_EXPRESSIONS
#    undef  ALIB_RESULTS
#    define ALIB_RESULTS             1
#endif
#if ALIB_ALOX
#    undef  ALIB_CONFIGURATION
#    define ALIB_CONFIGURATION       1
#endif
#if ALIB_CONFIGURATION
#    undef  ALIB_SYSTEM
#    define ALIB_SYSTEM              1
#endif
#if ALIB_SYSTEM
#    undef  ALIB_RESULTS
#    define ALIB_RESULTS             1
#    undef  ALIB_TIME
#    define ALIB_TIME                1
#endif
#if ALIB_CLI
#    undef  ALIB_RESULTS
#    define ALIB_RESULTS             1
#endif
#if ALIB_RESULTS
#    undef  ALIB_TEXT
#    define ALIB_TEXT                1
#endif
#if ALIB_TEXT
#    undef  ALIB_RESULTS
#    define ALIB_RESULTS             1
#    undef  ALIB_STRINGS
#    define ALIB_STRINGS             1
#    undef  ALIB_BOXING
#    define ALIB_BOXING              1
#    undef  ALIB_RESOURCES
#    define ALIB_RESOURCES           1
#    undef  ALIB_ENUMS
#    define ALIB_ENUMS               1
#endif
#if ALIB_RESOURCES
#    undef  ALIB_STRINGS
#    define ALIB_STRINGS             1
#    undef  ALIB_MONOMEM
#    define ALIB_MONOMEM             1
#endif
#if ALIB_BITBUFFER
#    undef  ALIB_MONOMEM
#    define ALIB_MONOMEM             1
#    undef  ALIB_ENUMS
#    define ALIB_ENUMS               1
#endif
#if ALIB_ENUMS
#    undef  ALIB_STRINGS
#    define ALIB_STRINGS             1
#    undef  ALIB_SINGLETONS
#    define ALIB_SINGLETONS          1
#endif
#if ALIB_THREADS
#    undef  ALIB_STRINGS
#    define ALIB_STRINGS             1
#endif
#if ALIB_STRINGS
#    undef  ALIB_CHARACTERS
#    define ALIB_CHARACTERS          1
#endif
#if ALIB_BOXING
#    undef  ALIB_SINGLETONS
#    define ALIB_SINGLETONS          1
#    undef  ALIB_CHARACTERS
#    define ALIB_CHARACTERS          1
#endif

// #################################################################################################
// Resolve file-set dependencies
// #################################################################################################
#if     defined( ALIB_FILESET_OWNER        ) \
     || defined( ALIB_FILESET_COMMON_ENUMS ) \
     || defined( ALIB_FILESET_MODULES      ) \
     || defined( ALIB_FILESET_LISTS        ) \
     || defined( ALIB_FILESET_PLUGINS      )
#   error "ALib preprocessor symbols for fileset selection (ALIB_FILESET_XYZ) may not be " \
          "defined from outside (passed to the compiler)."
#endif

#define ALIB_FILESET_OWNER            (    ALIB_THREADS          \
                                         | ALIB_TEXT             )

#define ALIB_FILESET_COMMON_ENUMS     (    ALIB_CHARACTERS       \
                                         | ALIB_MONOMEM          \
                                         | ALIB_TIME             \
                                         | ALIB_BOXING           )

#define ALIB_FILESET_MODULES          (    ALIB_TEXT             )

#define ALIB_FILESET_LISTS            (    ALIB_MONOMEM          )

#define ALIB_FILESET_PLUGINS          (    ALIB_CONFIGURATION    \
                                         | ALIB_EXPRESSIONS      )
// if defined(ALIB_DOX)
#else

#define ALIB_ALOX                     1
#define ALIB_BITBUFFER                1
#define ALIB_BOXING                   1
#define ALIB_CHARACTERS               1
#define ALIB_CLI                      1
#define ALIB_CONFIGURATION            1
#define ALIB_ENUMS                    1
#define ALIB_EXPRESSIONS              1
#define ALIB_FILESET_COMMON_ENUMS     1
#define ALIB_FILESET_LISTS            1
#define ALIB_FILESET_MODULES          1
#define ALIB_FILESET_OWNER            1
#define ALIB_FILESET_PLUGINS          1
#define ALIB_MONOMEM                  1
#define ALIB_RESOURCES                1
#define ALIB_RESULTS                  1
#define ALIB_SINGLETONS               1
#define ALIB_STRINGS                  1
#define ALIB_SYSTEM                   1
#define ALIB_TEXT                     1
#define ALIB_THREADS                  1
#define ALIB_TIME                     1

#endif //!defined(ALIB_DOX)

// #################################################################################################
// Macros for checking availability of modules and filesets
// #################################################################################################
#define  ALIB_DOCUMENTATION_URL   "https://alib.dev/"

#define ALIB_ASSERT_MODULE(modulename)                                                             \
static_assert( ALIB_ ## modulename,                                                                \
               "This module is not included in the ALib distribution/built. "                      \
               "See " ALIB_DOCUMENTATION_URL "alib_manual.html for more information" );            \

#define ALIB_ASSERT_FILESET(filesetname)                                                           \
static_assert( ALIB_FILESET_ ## filesetname,                                                       \
               "This file is not included in the ALib distribution/built. "                        \
               "See " ALIB_DOCUMENTATION_URL "alib_manual.html for more information" );            \


// #################################################################################################
// Macros to sect code (without using #if/#endif)
// #################################################################################################
#if ALIB_ALOX
#   define  ALIB_IF_ALOX(...)               __VA_ARGS__
#   define  ALIB_IFN_ALOX(...)
#else
#   define  ALIB_IF_ALOX(...)
#   define  ALIB_IFN_ALOX(...)              __VA_ARGS__
#endif

#if ALIB_BOXING
#   define  ALIB_IF_BOXING(...)             __VA_ARGS__
#   define  ALIB_IFN_BOXING(...)
#else
#   define  ALIB_IF_BOXING(...)
#   define  ALIB_IFN_BOXING(...)            __VA_ARGS__
#endif

#if ALIB_CHARACTERS
#   define  ALIB_IF_CHARACTERS(...)         __VA_ARGS__
#   define  ALIB_IFN_CHARACTERS(...)
#else
#   define  ALIB_IF_CHARACTERS(...)
#   define  ALIB_IFN_CHARACTERS(...)        __VA_ARGS__
#endif

#if ALIB_CLI
#   define  ALIB_IF_CLI(...)                __VA_ARGS__
#   define  ALIB_IFN_CLI(...)
#else
#   define  ALIB_IF_CLI(...)
#   define  ALIB_IFN_CLI(...)               __VA_ARGS__
#endif

#if ALIB_CONFIGURATION
#   define  ALIB_IF_CONFIGURATION(...)      __VA_ARGS__
#   define  ALIB_IFN_CONFIGURATION(...)
#else
#   define  ALIB_IF_CONFIGURATION(...)
#   define  ALIB_IFN_CONFIGURATION(...)     __VA_ARGS__
#endif

#if ALIB_ENUMS
#   define  ALIB_IF_ENUMS(...)              __VA_ARGS__
#   define  ALIB_IFN_ENUMS(...)
#else
#   define  ALIB_IF_ENUMS(...)
#   define  ALIB_IFN_ENUMS(...)             __VA_ARGS__
#endif

#if ALIB_EXPRESSIONS
#   define  ALIB_IF_EXPRESSIONS(...)        __VA_ARGS__
#   define  ALIB_IFN_EXPRESSIONS(...)
#else
#   define  ALIB_IF_EXPRESSIONS(...)
#   define  ALIB_IFN_EXPRESSIONS(...)       __VA_ARGS__
#endif

#if ALIB_MONOMEM
#   define  ALIB_IF_MONOMEM(...)            __VA_ARGS__
#   define  ALIB_IFN_MONOMEM(...)
#else
#   define  ALIB_IF_MONOMEM(...)
#   define  ALIB_IFN_MONOMEM(...)           __VA_ARGS__
#endif

#if ALIB_RESULTS
#   define  ALIB_IF_RESULTS(...)            __VA_ARGS__
#   define  ALIB_IFN_RESULTS(...)
#else
#   define  ALIB_IF_RESULTS(...)
#   define  ALIB_IFN_RESULTS(...)           __VA_ARGS__
#endif

#if ALIB_RESOURCES
#   define  ALIB_IF_RESOURCES(...)          __VA_ARGS__
#   define  ALIB_IFN_RESOURCES(...)
#else
#   define  ALIB_IF_RESOURCES(...)
#   define  ALIB_IFN_RESOURCES(...)         __VA_ARGS__
#endif

#if ALIB_SINGLETONS
#   define  ALIB_IF_SINGLETONS(...)         __VA_ARGS__
#   define  ALIB_IFN_SINGLETONS(...)
#else
#   define  ALIB_IF_SINGLETONS(...)
#   define  ALIB_IFN_SINGLETONS(...)        __VA_ARGS__
#endif

#if ALIB_STRINGS
#   define  ALIB_IF_STRINGS(...)            __VA_ARGS__
#   define  ALIB_IFN_STRINGS(...)
#else
#   define  ALIB_IF_STRINGS(...)
#   define  ALIB_IFN_STRINGS(...)           __VA_ARGS__
#endif

#if ALIB_SYSTEM
#   define  ALIB_IF_SYSTEM(...)             __VA_ARGS__
#   define  ALIB_IFN_SYSTEM(...)
#else
#   define  ALIB_IF_SYSTEM(...)
#   define  ALIB_IFN_SYSTEM(...)            __VA_ARGS__
#endif

#if ALIB_TEXT
#   define  ALIB_IF_TEXT(...)               __VA_ARGS__
#   define  ALIB_IFN_TEXT(...)
#else
#   define  ALIB_IF_TEXT(...)
#   define  ALIB_IFN_TEXT(...)              __VA_ARGS__
#endif

#if ALIB_THREADS
#   define  ALIB_IF_THREADS(...)            __VA_ARGS__
#   define  ALIB_IFN_THREADS(...)
#else
#   define  ALIB_IF_THREADS(...)
#   define  ALIB_IFN_THREADS(...)           __VA_ARGS__
#endif

#if ALIB_TIME
#   define  ALIB_IF_TIME(...)               __VA_ARGS__
#   define  ALIB_IFN_TIME(...)
#else
#   define  ALIB_IF_TIME(...)
#   define  ALIB_IFN_TIME(...)              __VA_ARGS__
#endif

#endif // HPP_ALIB_MODULES
