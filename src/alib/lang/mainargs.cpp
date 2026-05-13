namespace alib {
    int                 ARG_C = 0;
    const char**        ARG_VN= nullptr;
    const wchar_t**     ARG_VW= nullptr;
}

//##################################################################################################
// Namespace documentation alib
//##################################################################################################

/// # 1. Introduction #
/// This is the root namespace of \alib, a general purpose C++ library. \alib is developed by
/// A-Worx GmbH, Germany, published under <em>Boost Software License</em>.
///
/// While this namespace is the owner of the various sub-namespaces imposed by the \alibmods_nl,
/// it does not introduce many "own" entities. One exclamation is that the types and functions
/// of module \alib_bootstrap are not placed into a separate sub-namespace, but directly here.
///
/// Nevertheless, this namespace contains a substantial number of **type aliases**.
/// Almost every type found within the sub-namespaces which are dedicated to certain \alibmods,
/// has a corresponding alias in this namespace.
/// For templated types, there are often multiple aliases available.
/// The purpose and usage of these aliases are explained in detail in the appendix chapter
/// #"alib_manual_appendix_typealiases" of the General Programmer's Manual of \alib.
///
/// # 2. Domain-Specific Constants and Variables #
/// Similar to the concept of aliasing many types into this <b>namespace alib</b>
/// (which is described in the previous paragraph), it is also agreed to place important global
/// constants here. The impact and benefits are quite the same.<br>
/// As samples, see #"NEW_LINE" or #"BASECAMP;2", which appear in this namespace as
/// soon as the module \alib_strings, respectively \alib_camp is included.
///
/// # 3. Documentation #
/// The #"alib_manual" provides extensive documentation, including various separated manuals of
/// \alibmods, tutorials with sample code, information about configuring and compiling the
/// library, and so forth.<br>
/// For people new to \alib, the #"alib_manual" is the right place to start!
///
/// # Reference Documentation #
namespace alib {}

/// \dir ALib/build
/// Configuration files for build-systems and IDEs.

/// \dir ALib/build/cmake
/// Configuration scripts for the CMake buildsystem.

/// \dir alib/alox
/// This directory contains header-files and compilation units of the module \alib_alox.

/// \dir alib/alox/detail
/// This directory contains header-files and compilation units of the module \alib_alox.

/// \dir alib/alox/textlogger
/// This directory contains header-files and compilation units of the module \alib_alox.

/// \dir alib/alox/loggers
/// This directory contains header-files and compilation units of the module \alib_alox.

/// \dir alib/app
/// This directory contains header-files and compilation units of the module \alib_app.
///
/// \dir alib/bitbuffer
/// This directory contains header-files and compilation units of the module \alib_bitbuffer.

/// \dir alib/bitbuffer/ac_v1
/// This directory contains detail header-files and compilation units of the module \alib_bitbuffer.

/// \dir alib/boxing
/// This directory contains header-files and compilation units of the module \alib_boxing.

/// \dir alib/boxing/detail
/// This directory contains detail header-files and compilation units of the module \alib_boxing.

/// \dir alib/characters
/// This directory contains header-files and compilation units of the module \alib_characters.

/// \dir alib/variables
/// This directory contains header-files and compilation units of the module \alib_variables.

/// \dir alib/containers
/// This directory contains header-files and compilation units of the module \alib_containers.

/// \dir alib/containers/detail
/// This directory contains detail header-files and compilation units of the module \alib_containers.

/// \dir alib/enumops
/// This directory contains header-files and compilation units of the module \alib_enumops.

/// \dir alib/enumrecords
/// This directory contains header-files and compilation units of the module \alib_enumrecords.

/// \dir alib/expressions
/// This directory contains header-files and compilation units of the module \alib_expressions.

/// \dir alib/expressions/detail
/// This directory contains detail header-files and compilation units of the module \alib_expressions.

/// \dir alib/expressions/plugins
/// This directory contains header-files and compilation units of the module \alib_expressions.

/// \dir alib/expressions/util
/// This directory contains header-files and compilation units of the module \alib_expressions.

/// \dir alib/lang
/// This directory contains general language extension types, functions and preprocessor code
/// of \aliblong, which are to be included in any \alibbuild, regardless of the selected module subset.

/// \dir alib/threads
/// This directory contains foundational types needed for multi-threading compilations of \alib.
/// Types herein are unavailable (or shortened) in case symbol #"ALIB_SINGLE_THREADED" is set.

/// \dir alib/time
/// This directory contains foundational times which are included in any \alibbuild.
/// A Programmer's Manual is given with \alib_time.

/// \dir alib/camp
/// This directory contains header-files that define \alibcamps and extended bootstrapping
/// functionality as well as the compilation units of special \alib module \alib_camp.

/// \dir alib/system
/// This directory contains header-files and compilation units of the module \alib_system.

/// \dir alib/resources
/// This directory contains header-files and compilation units of the module \alib_resources.

/// \dir alib/resources/detail
/// This directory contains detail header-files and compilation units of the module \alib_resources.

/// \dir alib/exceptions
/// This directory contains header-files and compilation units of the module \alib_exceptions.

/// \dir alib/format
/// This directory contains header-files and compilation units of the module \alib_format.

/// \dir alib/monomem
/// This directory contains header-files and compilation units of the module \alib_monomem.

/// \dir alib/singletons
/// This directory contains header-files and compilation units of the module \alib_singletons.

/// \dir alib/strings
/// This directory contains header-files and compilation units of the module \alib_strings.

/// \dir alib/strings/detail
/// This directory contains detail header-files and compilation units of the module \alib_strings.

/// \dir alib/strings/util
/// This directory contains header-files and compilation units of utility types of the module \alib_strings.
