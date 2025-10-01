// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#define ALIB_COMPILATION_FLAGS


#define A_CHAR(STR)
#define A_SCHAR(STR)
#define A_CCHAR(STR)
#define A_NCHAR(STR)
#define A_WCHAR(STR)
#define A_XCHAR(STR)

#define LOX_LOX

/**
@defgroup GrpALibPreproMacros    ALib Preprocessor Macros
\details

This document lists all preprocessor macros of \alib used to generate code or prune code pieces.
Often, the definition of macros depend on the \ref GrpALibPreproSymbols,

The following sections of this document lists macros that are not related to a specific
\alibmod:

&nbsp;&nbsp;      \ref GrpALibMacros_alib_version      "ALib Version And Feature Verification"
<br> &nbsp;&nbsp; \ref GrpALibMacros_preprohelper      "C/C++ Preprocessor Helpers"
<br> &nbsp;&nbsp; \ref GrpALibMacros_langlink          "C/C++ Language And Linkage"
<br> &nbsp;&nbsp; \ref GrpALibMacros_warnings          "C/C++ Compiler Warning Control"
<br> &nbsp;&nbsp; \ref GrpALibMacros_dbgmessages       "Debug Assertions, Warnings and Errors"
<br> &nbsp;&nbsp; \ref GrpALibMacros_templ_tools       "C++ Template Programming Tools"
<br> &nbsp;&nbsp; \ref GrpALibMacros_locksandrecursion "Resource Locking and Recursive Programming"

Followed to this, module-specific macros are documented:

&nbsp;&nbsp;      \ref GrpALibMacros_mod_alox_debug    "Module ALox - Debug Logging"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_alox_release  "Module ALox - Release Logging"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_alox_lowlevel "Module ALox - Lowlevel Macros"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_camp          "Module Basecamp"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_boxing        "Module Boxing"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_characters    "Module Characters"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_config        "Module Config"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_enums         "Module Enums"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_expressions   "Module Expressions"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_strings       "Module Strings"
<br> &nbsp;&nbsp; \ref GrpALibMacros_mod_threads       "Module Threads"

The last section provides macros used to include or exclude (prune) code:
<br> &nbsp;&nbsp; \ref GrpALibMacros_pruning           "Code Pruning"

@{

\I{########################    ALib Version And Feature Verification      ######################## }
\name ALib Version And Feature Verification

\anchor GrpALibMacros_alib_version
Macros used to verify that the \alib binary used has the right version is compiled with
the same feature set as the current compilation unit.
For this, version and feature flags are compiled statically into the library.
The macros are used internally, respectively 'automatically'. Hence, they need not to be used
directly clients of the library.

@{
\def ALIB_VERSION
  The \alib version number.
  The value of this macro is stored in namespace variable \ref alib::VERSION.

  \see Programmer's manual chapter \ref alib_mod_bs_assertcomp.


\def ALIB_REVISION
  The \alib revision number.
  The value of this macro is stored in namespace variable \ref alib::REVISION.

  \see Programmer's manual chapter \ref alib_mod_bs_assertcomp.


\def ALIB_COMPILATION_FLAGS
  Macro to provide a compilation flag verifier value to be used with method
  \ref alib::AssertALibVersionAndFlags which is automatically invoked with function
   \ref alib::Bootstrap (and overloads).
  The value of this macro is stored in namespace variable \ref alib::COMPILATION_FLAGS.

  \see Programmer's manual chapter \ref alib_mod_bs_assertcomp.

\def ALIB_ASSERT_MODULE
  Asserts if a given module is included in the \alibbuild.
  @param modulename   The name of the module to assert as available.


\def ALIB_BASE_DIR
  String containing the source folder of \alib. Used with unit tests.

@}

\I{########################    C/C++ Preprocessor Helper Macros   #############################  }
\name C/C++ Preprocessor Helper Macros
\anchor GrpALibMacros_preprohelper

The following macros are fundamental and building blocks for other macros.

@{

 \def ALIB_STRINGIFY
   Makes as string from a preprocessor macro parameter. The string character type equals the
   default character type \alib{characters;character}.

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
   As a sample, see macro \ref ALIB_OWN.

   @param prefix  A prefix token to use.

 \def ALIB_EMPTY
   Defines an empty macro. This is useful for example if a macro defined variadic arguments
   (<c>'...'</c>) and a user wants to omit to provide a value. Some compilers (preprocessors)
   might warn about empty variadic arguments, if nothing was given. This macro avoids this
   and increased the readability.


 \def ALIB_COMMA
   Defines a simple comma (<c>','</c>). Used with macros when otherwise the use of a comma symbol
   leads to ambiguous syntax. For example when passing template types to macros.

   Used with macros when otherwise the use of a comma symbol
   leads to ambiguous syntax. For example when passing template types to macros as shown here:

           MY_MACRO( std::vector<int ALIB_COMMA double> )


 \def ALIB_COMMA_DBG
   Defines a simple comma (<c>','</c>) with \ref ALIB_DEBUG "debug-builds". In release
   compilations the macro is empty. This is useful for example when methods change their
   signature depending on the compilation type. The latter is unavoidable i.e if caller source code
   information should be passed for debugging.

   Used with macros when otherwise the use of a comma symbol
   leads to ambiguous syntax. For example when passing template types to macros.

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
   See macro \ref ALIB_STATIC_ASSERT for a general explanation. The only difference from this is 
   that the negation of the given expression is used. Hence the opposite of an assert, a "denial".
   @param CondVariable This should be a "speaking name" that tells the user right away what 
                       happened.
   @param Cond         The condition that is asserted to be \c false, and that is hid away from the 
                       compiler's message text.
   @param Message      The message that details what is said with \p{CondVariable}. 

@}


\I{######################    C/C++ Language And Linkage Macros    ###############################  }
\name C/C++ Language And Linkage Macros

\anchor GrpALibMacros_langlink

The macros listed here are supporting C++ compilation and linking control and language
specific utilities.

@{

\def ALIB_DLL
  Used to export/import C++ symbols into a dynamic link library.
  Defined under Windows/MSC when compiling or using \alib classes inside a DLL.
  Dependent on \ref ALIB_API_IS_DLL and \ref ALIB_API_NO_DLL.
                                      
\def ALIB_EXPORT
  Depends on the compiler-symbol \ref ALIB_C20_MODULES. Equals to keyword \c export if 
  compiled in module mode, and is empty otherwise.

\def ALIB_ASSERT_GLOBAL_NAMESPACE
  If this macro is placed outside the global namespace, a <c>static_assert</c> is raised at
  compile time.

\def ALIB_SIZEOF_WCHAR_T
  Compiler/platform-dependent value. Gives the sizeof values of type \c wchar_t in bytes.
  Possible values are \b 2 and \b 4.

\def ALIB_CALLER_FUNC
  Used by macro \ref ALIB_CALLER to retrieve the compiler-specific preprocessor variable
  of the current function name. With MSVC evaluates to \b __FUNCTION__, 
  with other compilers to \b __func__.

\def ALIB_CALLER
  This macro fills in the built-in compiler-symbols that provide the current source file,
  line number and function strings.<p>
  The macro should be used anywhere where this information is passed (as a nested macro in
  debug macros)
  @see Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.

\def ALIB_CALLER_PRUNED
  This macro is the same as \ref ALIB_CALLER but empty if \ref ALIB_DEBUG is
  not defined.<br>
  In comparison to \ref ALIB_CALLER_NULLED, this macro is to be used with invocations of
  methods that lack the corresponding parameters in release compilations.

\def ALIB_COMMA_CALLER_PRUNED
  Same as \ref ALIB_CALLER_PRUNED, but prepends a <c>','</c>  
  This is needed if the caller information is not the only, but the last argument in a call.
  
\def ALIB_CALLER_PRUNED_COMMA
  Same as \ref ALIB_CALLER_PRUNED, but adds a <c>','</c>  
  This is needed if the caller information is followed by (non-debug) arguments in a call.
  The latter is usually avoided but necessary when default values for the next argument should 
  be given.

\def ALIB_CALLER_NULLED
  In \ref ALIB_DEBUG "debug-builds" this macro is the same as \ref ALIB_CALLER.
  Otherwise evaluates to <c>{nullptr,0,nullptr,::std::thread::id(), nullptr}</c>.<br>
  In comparison to \ref ALIB_CALLER_PRUNED, this macro is to be used with invocations of
  methods that keep the corresponding parameters in release compilations.
  \note If methods keep the parameter, then a user can decide if caller information should be
        included in release-builds or not by either using \ref ALIB_CALLER or this macro.   
                                 
\def ALIB_COMMA_CALLER_NULLED
  Same as \ref ALIB_CALLER_NULLED, but prepends a <c>','</c>  
  This is needed if the caller information is not the only, but the last argument in a call.
  
\def ALIB_CALLER_NULLED_COMMA
  Same as \ref ALIB_CALLER_NULLED, but adds a <c>','</c>  
  This is needed if the caller information is followed by (non-debug) arguments in a call.
  The latter is usually avoided but necessary when default values for the next argument should 
  be given.
  
\def ALIB_DBG_TAKE_CI
  Used with method declarations to define parameter \p{ci} of type \alib{lang;CallerInfo}.
  

\def bitsof(type)
  Like C++ keyword <c>sizeof</c> but returns the number of bits of the type of the given value.
  The return type is <c>int</c> instead of <c>size_t</c>, which satisfies \alib code conventions.

  \note To improve code readability, namely to a) indicate that this is an inlined, constant
        expression and b) to indicate that this is just using keyword <c>sizeof</c>,
        as an exception from the naming rules, this function is spelled in lower case.

  @see Function \ref alib::lang::bitsofval(const T&)
  
\def ALIB_STACK_ALLOCATED_TYPE
  To be placed at the beginning of a type which is to be allocated exclusively on the stack.
  For this, its <c>new-operators</c> as well as copy and move operators are declared private.
  @param T The name of the type that this macro is placed in. 
@}


\I{#########################     C/C++ Compiler Warning Control      ############################  }
\name C/C++ Compiler Warning Control
\anchor GrpALibMacros_warnings

\alib sources are designed to be compiled on different compilers using the highest possible
compiler warning levels available.
In some situations, some warnings need to be temporarily disabled. The following macros do
this in a compiler agnostic way.

@{
\def ALIB_WARNINGS_RESTORE
 Resets compiler warning settings to the state before one of the other macros of this section
 had been placed. Every such placement should have a corresponding placement of this
 macro.


\def ALIB_WARNINGS_UNINITIALIZED_OFF
 Preprocessor macro to disable compiler warnings about uninitialized values.

\def ALIB_WARNINGS_ALLOW_NULL_POINTER_PASSING
 Preprocessor macro to disable GCC warning "nonnull". Used for example with template meta
 programming below C++ 14 standard.
                                            
\def ALIB_WARNINGS_ALLOW_UNREACHABLE_CODE
 Preprocessor macro to disable compiler warnings about detected unreachable code.
 (i.e with clang this ignores "-Wunreachable-code" )

\def ALIB_WARNINGS_MACRO_NOT_USED_OFF
 Preprocessor macro to disable compiler warnings about unused preprocessor macros.

\def ALIB_WARNINGS_RESERVED_MACRO_NAME_OFF
 Preprocessor macro to disable compiler warnings about reserved macro names.
 (i.e with clang this ignores "-Wreserved-macro-identifier" )

\def ALIB_WARNINGS_ALLOW_MACRO_REDEFINITION
 Preprocessor macro to disable compiler warnings on redefining macros without prior un-defining.

\def ALIB_WARNINGS_ALLOW_UNSAFE_FUNCTION_OR_VARIABLE
 Preprocessor macro to disable compiler warnings for use of 'unsafe' (deprecated) library
 functions. Currently used with MSVC 4996 only.

\def ALIB_WARNINGS_OVERLOAD_VIRTUAL_OFF
 Preprocessor macro to disable compiler warnings about virtual methods that become hidden
 by overloaded methods with a different signature.

\def ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
 Preprocessor macro to disable compiler warnings when an enumeration element is switched while
 not all enumeration elements get caught.

\def ALIB_WARNINGS_ALLOW_BITWISE_SWITCH
 Preprocessor macro to disable compiler warnings when a
 "bitwise type scoped enumeration" (see \alib{enumops;BitwiseTraits}) or similar types with
 'sparse' case coverage are used in a switch statement.

\def ALIB_WARNINGS_IGNORE_DOCS
 Preprocessor macro to disable compiler warnings according to source documentation (as of today
 known to be thrown by \b clang only).

\def ALIB_WARNINGS_ALLOW_SHIFT_COUNT_OVERFLOW
 Preprocessor macro to disable compiler warnings concerning overflows during bit-shift operations.
     
\def ALIB_WARNINGS_IGNORE_DEPRECATED
 Preprocessor macro to disable compiler warnings concerning the use of deprecated functions and
 types.

\def ALIB_WARNINGS_IGNORE_UNUSED_MACRO
 Preprocessor macro to disable compiler warnings concerning unused macro definitions.

\def ALIB_WARNINGS_IGNORE_UNUSED_PARAMETER
 Preprocessor macro to disable compiler warnings concerning unused parameters.
 (This macro is only used with unit testing and documentation sample code.)

\def ALIB_WARNINGS_IGNORE_UNUSED_VARIABLE
 Preprocessor macro to disable compiler warnings concerning unused parameters.
(This macro is only used with unit testing and documentation sample code.)

\def ALIB_WARNINGS_IGNORE_UNUSED_FUNCTION
 Preprocessor macro to disable compiler warnings concerning unused (member-) functions.
 (This macro is only used with unit testing and documentation sample code.)

\def ALIB_WARNINGS_IGNORE_UNUSED_LAMBDA_CAPTURE
 Preprocessor macro to disable compiler warnings concerning unused (member-) functions.
 (Needed, because it can happen that gcc needs a \c this captured, while clang complains
 it was unnecessary.)  
 
\def ALIB_WARNINGS_IGNORE_FUNCTION_TEMPLATE
 Preprocessor macro to disable compiler warnings concerning unused function templates.

\def ALIB_WARNINGS_IGNORE_RESERVED_IDENTIFIER
 Preprocessor macro to disable compiler warnings concerning conflicting identifier names.
 Usually identifiers with double underscore (<c>'_'</c>) are forbidden, but with some
 preprocessor macros, those are not easily avoidable.

\def ALIB_WARNINGS_IGNORE_NOTHING_RETURNED
 Preprocessor macro to disable compiler warnings concerning non-void functions that may
 not return a value. Needed when compiler cannot properly detect the inherent safeness (correctness) 
 of a function that always is returning a type.

\def ALIB_FALLTHROUGH
 Used with keyword \c switch to annotate a \c case block that has no \b break or (\c return, etc.).
 Avoids a warning with some compilers.

\def ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW
 Preprocessor macro to disable compiler warnings about code that may cause an overflow of an
 integral constant.

\def ALIB_WARNINGS_IGNORE_SIGN_CONVERSION
 Preprocessor macro to disable compiler warnings about code that changes signedness of integral
 values.

\def ALIB_WARNINGS_IGNORE_INTEGER_OVERFLOW
 Preprocessor macro to disable compiler warnings about code that may cause an overflow of an
 integral value, e.g., with bit-shift operations.

@}


\I{#################    Debug Assertions, Warnings and Errors     ##########  }
\name Debug Assertions, Warnings and Errors

\anchor GrpALibMacros_dbgmessages
These macros are used with debug-builds of \alib, respectively when symbol \ref ALIB_DEBUG
is given.
They provide shortcuts into functions of the small foundational module \ref alib_mod_assert.

If also module \alib_alox is compiled into the library, then a plug-in that uses
\alox for the message output is used. 
See class \alox{ALoxAssertionPlugin} for more information.

@{
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

@}


\I{#########################     C++ Template Programming Tools      ############################  }
\name C++ Template Programming Tools

\anchor GrpALibMacros_templ_tools
Macros for handling template types.

@{

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

@}

\I{########################  Resource Locking and Recursive Programming   #######################  }
\name Macros for Resource Locking and Recursive Programming

\anchor GrpALibMacros_locksandrecursion
The macros listed here have a direct relationship with classes defined in \alib
and with their use.

@{
\def ALIB_OWN
 This preprocessor macro defines an anonymous instance of type \alib{lang;Owner}. 
 The template type is deduced from parameter \p{ownable} using C++ keyword \c decltype.

 By using this macro there is no need to "invent" an (otherwise unreferenced) identifier for
 that definition. 
 \note This macro exists mainly because C++ does not support anonymous local instances.

 \see 
  - A source-sample on how this macro is used, is given with chapter 
    \ref alib_threads_locks_auto_owner of the Programmer's Manual of module \alib_threads.
  - Sibling macros 
    - \ref ALIB_OWN_RECURSIVE, and 
    - \ref ALIB_OWN_SHARED, <p>
    which instantiate an object of type \alib{lang;OwnerRecursive}, 
    respectively \alib{lang;OwnerShared}, instead  of class \alib{lang;Owner}. 
  - Macros \ref ALIB_LOCK and \ref ALIB_LOCK_WITH are using this macro to lock
    types \alib{threads;Lock} and \alib{threads;RecursiveLock}.<br>
  - Macros \ref ALIB_DCS and \ref ALIB_DCS_WITH are using this macro to 
    signal entering a \alib{lang;DbgCriticalSections;critical section}.
  - Macro \ref ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS implements a mechanism to detect and assert
    recursive function calls.
  - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.

  @param ownable   The \b %Ownable to acquire and release.

\def ALIB_OWN_RECURSIVE
 Same as \ref ALIB_OWN, but creates a local anonymous instance of class \alib{lang;OwnerRecursive}.

\def ALIB_OWN_SHARED
 Same as \ref ALIB_OWN, but creates a local anonymous instance of class \alib{lang;OwnerShared}.

\def ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS
 This macro may be placed at the beginning of a types' method to detect and assert recursive
 invocations - which by definition must happen.<br>
 The macro internally uses macro \ref ALIB_OWN and hence class \alib{lang;Owner} that uses C++ stack
 unwinding to be sure to always clear the flag, even if an exception occurs.

 In the case that the macro is placed in more than one method, it can be used to ensure that none
 of the methods calls any of the others.

 A prerequisite of using this macro is to have macro
 \ref ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL placed in the member declaration area
 of the type.

 This macro is only active in debug-compilations.


\def ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL
 Used with macro \ref ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS, usually in header compilation unit
 to declare a boolean class member that is used for detecting recursions.

@}

\I{##############################    Code Selection / Pruning       #############################  }

\name Code Selection
\anchor GrpALibMacros_pruning
The macros listed here include or prune code, dependent on different preprocessor symbols.
They are mainly given to avoid <c>\#if</c>/<c>\#else</c>/<c>\#endif</c> statements for small
code pieces.


@{
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
  Similar to \ref ALIB_DBG, but accepts a release version of the code as well. The release
  version is expected as first macro parameter. Note, that the release code must not contain a
  comma (',')  while the debug code is allowed to. A comma in release code may be substituted
  with macro \ref ALIB_COMMA. However, more complicated stuff should be placed in usual
  <c>\#if</c>/<c>\#else</c>/<c>\#endif</c> statements.

\def ALIB_REL
  As a counterpart to \ref ALIB_DBG, this macro may be used for placing code that is only
  available in release compilations. The macro is provided for completeness only and should
  be used seldom and with care, as it generally implies more intense release code testing.
  As of Version 1810, \alib does not use this macro internally.

\def ALIB_CPP_23
  This simple macro may be used for placing code that is only applicable to the C++ language
  standard \c 23 or higher. 
  With the lower standard, the code given is pruned.
  @param ... The source to select.
  @see Macro \ref ALIB_CPP_BEFORE_23

 \def ALIB_CPP_BEFORE_23
  This simple macro may be used for placing code that is only applicable to the C++ language
  standard \c 20 or below. 
  With the higher standard, the code given is pruned.
  @param ... The source to select.
  @see Macro \ref ALIB_CPP_23


This group of simple macros either include or prune code dependent on the inclusion
of \alibmods_nl in an \alibbuild.

The symbols are defined in the internal header \alibheader{alib.inl} which always is 
the first \alib header-file included.

\def IF_ALIB_ALOX
  Prunes given code \p{...} if \alib_alox is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_ALOX and sibling macro \ref IFNOT_ALIB_ALOX.
  @param ... The source to select.

\def IFNOT_ALIB_ALOX
  Prunes given code \p{...} if \alib_alox is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_ALOX and sibling macro \ref IF_ALIB_ALOX.
  @param ... The source to select.

\def IF_ALIB_BOXING
  Prunes given code \p{...} if \alib_boxing is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_BOXING and sibling macro \ref IFNOT_ALIB_BOXING.
  @param ... The source to select.

\def IFNOT_ALIB_BOXING
  Prunes given code \p{...} if \alib_boxing is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_BOXING and sibling macro \ref IF_ALIB_BOXING.
  @param ... The source to select.

\def IF_ALIB_BITBUFFER
  Prunes given code \p{...} if \alib_bitbuffer is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_BITBUFFER and sibling macro \ref IFNOT_ALIB_BITBUFFER.
  @param ... The source to select.

\def IFNOT_ALIB_BITBUFFER
  Prunes given code \p{...} if \alib_bitbuffer is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_BITBUFFER and sibling macro \ref IF_ALIB_BITBUFFER.
  @param ... The source to select.

\def IF_ALIB_VARIABLES
  Prunes given code \p{...} if \alib_variables is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_VARIABLES and sibling macro \ref IFNOT_ALIB_VARIABLES.
  @param ... The source to select.

\def IFNOT_ALIB_VARIABLES
  Prunes given code \p{...} if \alib_variables is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_VARIABLES and sibling macro \ref IF_ALIB_VARIABLES.
  @param ... The source to select.

\def IF_ALIB_CONTAINERS
  Prunes given code \p{...} if \alib_containers is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_CONTAINERS and sibling macro \ref IFNOT_ALIB_CONTAINERS.
  @param ... The source to select.

\def IFNOT_ALIB_CONTAINERS
  Prunes given code \p{...} if \alib_containers is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_CONTAINERS and sibling macro \ref IF_ALIB_CONTAINERS.
  @param ... The source to select.

\def IF_ALIB_CLI
  Prunes given code \p{...} if \alib_cli is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_CLI and sibling macro \ref IFNOT_ALIB_CLI.
  @param ... The source to select.

\def IFNOT_ALIB_CLI
  Prunes given code \p{...} if \alib_cli is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_CLI and sibling macro \ref IF_ALIB_CLI.
  @param ... The source to select.

\def IF_ALIB_CAMP
  Prunes given code \p{...} if \alib_resources is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_CAMP and sibling macro \ref IFNOT_ALIB_CAMP.
  @param ... The source to select.

\def IFNOT_ALIB_CAMP
  Prunes given code \p{...} if \alib_resources is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_CAMP and sibling macro \ref IF_ALIB_CAMP.
  @param ... The source to select.

\def IF_ALIB_ENUMRECORDS
  Prunes given code \p{...} if \alib_enumrecords is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_ENUMRECORDS and sibling macro \ref IFNOT_ALIB_ENUMRECORDS.
  @param ... The source to select.

\def IFNOT_ALIB_ENUMRECORDS
  Prunes given code \p{...} if \alib_enumrecords is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_ENUMRECORDS and sibling macro \ref IF_ALIB_ENUMRECORDS.
  @param ... The source to select.

\def IF_ALIB_EXPRESSIONS
  Prunes given code \p{...} if \alib_expressions is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_EXPRESSIONS and sibling macro \ref IFNOT_ALIB_EXPRESSIONS.
  @param ... The source to select.

\def IFNOT_ALIB_EXPRESSIONS
  Prunes given code \p{...} if \alib_expressions is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_EXPRESSIONS and sibling macro \ref IF_ALIB_EXPRESSIONS.
  @param ... The source to select.

\def IF_ALIB_FILES
  Prunes given code \p{...} if \alib_files is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_FILES and sibling macro \ref IFNOT_ALIB_FILES.
  @param ... The source to select.

\def IFNOT_ALIB_FILES
  Prunes given code \p{...} if \alib_files is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_FILES and sibling macro \ref IF_ALIB_FILES.
  @param ... The source to select.

\def IF_ALIB_THREADMODEL
  Prunes given code \p{...} if \alib_threadmodel is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_THREADMODEL and sibling macro \ref IFNOT_ALIB_THREADMODEL.
  @param ... The source to select.

\def IFNOT_ALIB_THREADMODEL
  Prunes given code \p{...} if \alib_threadmodel is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_THREADMODEL and sibling macro \ref IF_ALIB_THREADMODEL.
  @param ... The source to select.

\def IF_ALIB_THREADS
  Prunes given code \p{...} if \alib_threads is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_SINGLE_THREADED and sibling macro \ref IFNOT_ALIB_THREADS.
  @param ... The source to select.

\def IFNOT_ALIB_THREADS
  Prunes given code \p{...} if \alib_threads is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_SINGLE_THREADED and sibling macro \ref IF_ALIB_THREADS.
  @param ... The source to select.

\def IF_ALIB_STRINGS
  Prunes given code \p{...} if \alib_strings is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_STRINGS and sibling macro \ref IFNOT_ALIB_STRINGS.
  @param ... The source to select.

\def IFNOT_ALIB_STRINGS
  Prunes given code \p{...} if \alib_strings is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_STRINGS and sibling macro \ref IF_ALIB_STRINGS.
  @param ... The source to select.

\def IF_ALIB_MONOMEM
  Prunes given code \p{...} if \alib_monomem is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_MONOMEM and sibling macro \ref IFNOT_ALIB_MONOMEM.
  @param ... The source to select.

\def IFNOT_ALIB_MONOMEM
  Prunes given code \p{...} if \alib_monomem is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_MONOMEM and sibling macro \ref IF_ALIB_MONOMEM.
  @param ... The source to select.

\def IF_ALIB_RESOURCES
  Prunes given code \p{...} if \alib_resources is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_RESOURCES and sibling macro \ref IFNOT_ALIB_RESOURCES.
  @param ... The source to select.

\def IFNOT_ALIB_RESOURCES
  Prunes given code \p{...} if \alib_resources is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_RESOURCES and sibling macro \ref IF_ALIB_RESOURCES.
  @param ... The source to select.

\def IF_ALIB_EXCEPTIONS
  Prunes given code \p{...} if \alib_exceptions is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_EXCEPTIONS and sibling macro \ref IFNOT_ALIB_EXCEPTIONS.
  @param ... The source to select.

\def IFNOT_ALIB_EXCEPTIONS
  Prunes given code \p{...} if \alib_exceptions is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_EXCEPTIONS and sibling macro \ref IF_ALIB_EXCEPTIONS.
  @param ... The source to select.

\def IF_ALIB_SYSTEM
  Prunes given code \p{...} if \alib_system is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_SYSTEM and sibling macro \ref IFNOT_ALIB_SYSTEM.
  @param ... The source to select.

\def IFNOT_ALIB_SYSTEM
  Prunes given code \p{...} if \alib_system is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_SYSTEM and sibling macro \ref IF_ALIB_SYSTEM.
  @param ... The source to select.

\def IF_ALIB_FORMAT
  Prunes given code \p{...} if \alib_format is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_FORMAT and sibling macro \ref IFNOT_ALIB_FORMAT.
  @param ... The source to select.

\def IFNOT_ALIB_FORMAT
  Prunes given code \p{...} if \alib_format is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_FORMAT and sibling macro \ref IF_ALIB_FORMAT.
  @param ... The source to select.

\def IF_ALIB_SINGLETONS
  Prunes given code \p{...} if \alib_singletons is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_SINGLETONS and sibling macro \ref IFNOT_ALIB_SINGLETONS.
  @param ... The source to select.

\def IFNOT_ALIB_SINGLETONS
  Prunes given code \p{...} if \alib_singletons is not included in the \alibbuild.
  \see
    Symbol \ref ALIB_SINGLETONS and sibling macro \ref IF_ALIB_SINGLETONS.
  @param ... The source to select.

@}



\I{################################################################################################}
\I{###########################   ALox Macros For Debug Logging     ##############################  }
\I{################################################################################################}
\name ALox Macros For Debug Logging

\anchor GrpALibMacros_mod_alox_debug
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
lines, or embedded in macro \ref Log_Prune.
\note To be clear: When using the macros, the <em>\#if / \#endif</em> statements are \e not needed!

All macro names are prefixed "Log_". This implies that they are macros to implement *debug*
logging. In contrast to this, a set of similar macros exists for release logging
(see \ref GrpALibMacros_mod_alox_release "release logging macros"). Those are
prefixed "Lox_". (The choice of the prefixes *Log_* and *Lox* provide  maximum source code
compatibility of <b>%ALox for C++</b> log lines in comparison to ALox for C# and ALox for Java.<p>

Most macros make use of the macro \ref LOG_LOX, which references a singleton
object of class \ref alib::lox::Lox "Lox" that is used for all debug logging.
This singleton concept covers most
uses cases for debug logging. If more flexibility is wanted, then either macro
\ref LOG_LOX might be changed for different compilation units or the <b>%ALox for C++ API</b>
might be used instead of the macros listed here.

\note The <em>Scope Domain</em> mechanism of \alox, as well as other \alox functionality
which relies on \e Scopes use the information provided by \alib macro \ref ALIB_CALLER.
Changing this macro, might cause these mechanism to fail.


@{
\def Log_Prune
  This very simple macro is used for pruning debug <em>Log Statements</em>.
  While it is used as a building block of all other macros for debug logging, for code entities
  using \alox, it provides an easy way to prune code lines that get inserted purely to support
  logging, e.g., to create \alox loggers or to prepare more complex log output. (The alternative
  way is to enclose such code within
  <code> \#ifdef ALOX_DBG_LOG </code> / <code> \#endif </code> preprocessor lines.
  <p>

  Depends on \ref ALOX_DBG_LOG. If this is set, the macro just copies the code provided, else it
  does not copy it, hence removes the code.

\def Log_SetSourcePathTrimRule
  Invokes \alox{Lox::SetSourcePathTrimRule} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_ClearSourcePathTrimRules
  Invokes \alox{Lox::ClearSourcePathTrimRules} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_AddDebugLogger
  Invokes \alox{Log::AddDebugLogger} to create, add and configure a default debug logger suitable for the platform and toolset.
  @param ...  The parameters to pass.

\def Log_RemoveDebugLogger
  Invokes \alox{Log::RemoveDebugLogger} to remove and delete a debug logger created by \ref alib::lox::Log::AddDebugLogger "Log::AddDebugLogger".
  @param ...  The parameters to pass.

\def Log_GetLogger
  Invokes \alox{Lox::GetLogger} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param identifier  The identifier name of the variable that receives the pointer to the logger.
  @param name        The name of the logger to retrieve.

\def Log_RemoveLogger
  Invokes \alox{Lox::RemoveLogger} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param logger  Either the name of or a pointer to the logger to remove.

\def Log_SetDomain
  Invokes \alox{Lox::SetDomain} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_RemoveThreadDomain
  Invokes \alox{Lox::RemoveThreadDomain} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_SetDomainSubstitutionRule
  Invokes \alox{Lox::SetDomainSubstitutionRule} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_SetVerbosity
  Invokes one of the overloaded methods \alox{Lox::SetVerbosity} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_SetStartTime
  Invokes \alox{Lox::SetStartTime} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_MapThreadName
  Invokes \alox{Lox::MapThreadName} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_LogState
  Invokes \alox{Lox::State} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_Verbose
  Invokes \alox{Lox::Verbose} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_Info
  Invokes \alox{Lox::Info} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_Warning
  Invokes \alox{Lox::Warning} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_Error
  Invokes \alox{Lox::Error} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_Assert
  Invokes \alox{Lox::Assert} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_If
  Invokes \alox{Lox::If} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_Once
  Invokes \alox{Lox::Once} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_Entry
  Invokes \alox{Lox::Entry} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_SetPrefix
  Invokes \alox{Lox::SetPrefix} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_Store
  Invokes \alox{Lox::Store} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

\def Log_Retrieve
  Invokes \alox{Lox::Retrieve} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param data  The identifier name of the \alib{boxing;Box} object to take the retrieved data.
  @param ...   The parameters to pass.

\def Log_IsActive
  Invokes \alox{Lox::IsActive} on the object of type Lox defined in macro \ref LOG_LOX.
  The result value is assigned to given variable \p{result}
  @param result The name of a variable of type \c int that the return value of \b IsActive() is
                assigned to.
  @param ...    The parameters to pass.

\def Log_Exception
  Invokes \alox{LogTools::Exception} providing the debug singleton of class Lox defined in macro \ref LOG_LOX.
  @param ...  The parameters to pass.

@}

\I{################################################################################################}
\I{###########################   ALox Macros For Release Logging   ##############################  }
\I{################################################################################################}
\name ALox Macros For Release Logging

\anchor GrpALibMacros_mod_alox_release
The macros listed here, are provided to place release <em>Log Statements</em> within source code
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
release log macros. Alternatively, such code might be embedded in macro \ref Lox_Prune.
(Pruning of release logging can be enabled by defining the compiler-symbol \ref ALOX_REL_LOG to \c 0
and could be useful in certain situations.)

Before using the macros, each code entity has to set the preprocessor macro \ref LOX_LOX
This can be done in a general header-file of the software,
(e.g., the same that exposes the release-Lox object to that source), or, in more complex
scenarios with more than one release-Lox object, at any other appropriate source location.

All macro names are prefixed with the term <b>Lox_</b>. This implies that they are macros to
implement <em>release logging</em>. In contrast to this, a set of similar macros exists for
<em>debug logging</em> (see \ref GrpALibMacros_mod_alox_debug "debug logging macros"). Those are
prefixed <b>Log_</b>.
\note The choice of the prefixes <b>Log_</b> and <b>Lox_</b> was made to provide  maximum source
code compatibility of <b>%ALox for C++</b> log lines in comparison to other ALox implementations
in other programming languages, which provide a class Log as a kind of 'mirror' class of
class Lox. Invocations to class Log get pruned in these languages. <p>

Most macros make use of the macro \ref LOX_LOX, which references a singleton
object of class \ref alib::lox::Lox "Lox" that is used for all release logging.
This singleton concept covers most
uses cases for release logging. If more flexibility is wanted, then either the macro
\ref LOX_LOX might be changed for different compilation units or the <b>%ALox C++ API</b>
might be used instead of the macros listed here.


@{
\def Lox_Prune
  This very simple macro is used for pruning release <em>Log Statements</em> at the moment release logging
  gets disabled (what in standard release scenarios is not done).<p>

  While it is used as a building block of all other macros for release logging, for code entities
  using \alox, it provides an easy way to prune code lines that get inserted purely to support
  logging, e.g., to create \alox loggers or to prepare more complex log output. (The alternative
  way is to enclose such code within
  <code> \#ifdef ALOX_REL_LOG </code> / <code> \#endif </code> preprocessor lines.
  <p>

  Depends on \ref ALOX_REL_LOG. If this is set, the macro just copies the code provided, else it
  does not copy it, hence removes the code.

  @param ...  The code to prune.

\def Lox_SetSourcePathTrimRule
  Invokes \alox{Lox::SetSourcePathTrimRule} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_ClearSourcePathTrimRules
  Invokes \alox{Lox::ClearSourcePathTrimRules} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_GetLogger
  Invokes \alox{Lox::GetLogger} on the object of type Lox defined in macro \ref LOX_LOX.
  @param identifier  The identifier name of the variable that receives the pointer to the logger.
  @param name        The name of the logger to retrieve.

\def Lox_RemoveLogger
  Invokes \alox{Lox::RemoveLogger} on the object of type Lox defined in macro \ref LOX_LOX.
  @param logger  Either the name of or a pointer to the logger to remove.


\def Lox_SetDomain
  Invokes \alox{Lox::SetDomain} on the object of type Lox defined in macro \ref LOX_LOX.
  \attention
    If \ref ALOX_REL_LOG_CI is not set, which is the default for release logging, and when used with language-related
    \e Scopes, this method will log an internal warning and will not be effective in respect to
    \e %Scope::Path, \e %Scope::Filename, and \e %Scope::Method.
    See \ref alib_mod_alox for detailed information.

    If Scope Domains based on source-related scopes should be supported in release logging, the
    \alibbuild as well as the software entity have to be compiled with the compiler-symbol \ref ALOX_REL_LOG_CI.
    Note that one effect of setting this symbol is, that information on source code paths and file names, as well as
    method names make their way into the release executable. This may not be wanted.

  @param ...  The parameters to pass.

\def Lox_RemoveThreadDomain
  Invokes \alox{Lox::RemoveThreadDomain} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.


\def Lox_SetDomainSubstitutionRule
  Invokes \alox{Lox::SetDomainSubstitutionRule} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_SetVerbosity
  Invokes one of the overloaded methods \alox{Lox::SetVerbosity} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_SetStartTime
  Invokes \alox{Lox::SetStartTime} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_MapThreadName
  Invokes \alox{Lox::MapThreadName} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_LogState
  Invokes \alox{Lox::State} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_Verbose
  Invokes \alox{Lox::Verbose} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_Info
  Invokes \alox{Lox::Info} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_Warning
  Invokes \alox{Lox::Warning} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_Error
  Invokes \alox{Lox::Error} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_Assert
  Invokes \alox{Lox::Assert} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_If
  Invokes \alox{Lox::If} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_Once
  Invokes \alox{Lox::Once} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_Entry
  Invokes \alox{Lox::Entry} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_Store
  Invokes \alox{Lox::Store} on the object of type Lox defined in macro \ref LOX_LOX.
  \attention
    If \ref ALOX_REL_LOG_CI is not set, which is the default for release logging
    statements, Log Data cannot be used in conjunction with language-related \e Scopes.<br>
    If <em>Log Data</em> should be supported in release logging, the \alibbuild as well as the
    software entity have to be compiled with the compiler-symbol \ref ALOX_REL_LOG_CI.
    Note that one effect of setting this symbol is, that information on source code paths and
    file names, as well as method names make their way into the release executable.
    This may not be wanted.<br>
    In general, methods \b Lox::Store and \b Lox::Retrieve should exclusively be used for
    debug logging. See user manual for more information.
  @param ...  The parameters to pass.

\def Lox_Retrieve
  Invokes \alox{Lox::Retrieve} on the object of type Lox defined in macro \ref LOX_LOX.
  @param data  The identifier name of the \alib{boxing;Box} object to take the retrieved data.
  @param ...   The parameters to pass.

\def Lox_SetPrefix
  Invokes \alox{Lox::SetPrefix} on the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.

\def Lox_IsActive
  Invokes \alox{Lox::IsActive} on the object of type Lox defined in macro \ref LOX_LOX.
  The result value is assigned to given variable \p{result}
  @param result The name of a variable of type \c int that the return value of \b IsActive() is
                assigned to.
  @param ...    The parameters to pass.


\def Lox_Exception
  Invokes \alox{LogTools::Exception} providing the object of type Lox defined in macro \ref LOX_LOX.
  @param ...  The parameters to pass.
@}

\I{################################################################################################}
\I{###############################    ALox Low Level Macros    ##################################  }
\I{################################################################################################}
\name ALox Low level Macros

\anchor GrpALibMacros_mod_alox_lowlevel

The preprocessor macros listed here are the foundation for
\ref GrpALibMacros_mod_alox_debug "debug logging" and
\ref GrpALibMacros_mod_alox_release "release logging" macros introduced below.

Developers that use \alox in standard use cases do not need to know and use the macros listed here.

@{

\def LOG_LOX
  The Lox instance used by all debug logging macros. This can be overwritten (prior or after
  including alox.hpp) to allow different instances of class Lox for debug logging within
  different source entities. However, other ways to structure log output and to separate
  log information into different streams exists in \alox and overwriting this macro is not
  recommended for standard use cases.<p>

  Note: The definition must provide a reference (not a pointer) to the \b %Lox object.

\def LOX_LOX
  The Lox instance used by all release logging macros. This has to be set (prior or after
  including alox.hpp) to provide access to a, dedicated instance of class Lox created for
  release logging within software.<br>
  It is of course allowed to use different instances within different source entities.
  However, other ways to structure log output and separate log streams exists in \alox and should
  be evaluated before introducing different instances of class Lox.<p>

  Note: The definition must provide a reference (not a pointer) to the \b %Lox object.

\def LOG_CI
  Dependent on #ALOX_DBG_LOG_CI, this macro provides comma delimited source information
  (sourcefile, line number, function name) or corresponding \c null values.

\def LOX_CI
  Dependent on #ALOX_REL_LOG_CI, this macro provides comma delimited source information
  (sourcefile, line number, function name) or corresponding \c null values.


\def LOG_ACQUIRE
  Macro that is placed at the beginning of almost all \ref GrpALibMacros_mod_alox_debug "debug logging macros".
  Provides scope information (provided that \ref ALOX_DBG_LOG_CI is set) to the lox and places
  the debug lox instance access code using \ref LOG_LOX.

\def LOG_RELEASE
  Macro that is placed at the end of almost all \ref GrpALibMacros_mod_alox_debug "debug logging macros".
  Releases the lox which was acquired when passing scope information (in macro \ref LOG_ACQUIRE.).


\def LOX_ACQUIRE
  Macro that is placed at the beginning of almost all \ref GrpALibMacros_mod_alox_release "release logging macros".
  Provides scope information (provided that \ref ALOX_REL_LOG_CI is set) to the lox and places
  the release lox instance access code using \ref LOX_LOX.

\def LOX_RELEASE
  Macro that is placed at the end of almost all \ref GrpALibMacros_mod_alox_release "release logging macros"..
  Releases the lox which was acquired when passing scope information (in macro \ref LOG_ACQUIRE.).

@}


\I{################################################################################################}
\I{#############################      ALib Module Boxing       ##################################  }
\I{################################################################################################}

\name ALib Module Boxing

\anchor GrpALibMacros_mod_boxing
The macros in this section are introduced by module \alib_boxing.

@{
\def ALIB_BOXING_CUSTOMIZE
  Macro used to specialize template struct \alib{boxing;BoxTraits}, which customizes boxing for given
  boxable type \p{TSource} to be mapped to non-array type \p{TMapped}.

  Type alias \alib{boxing;BoxTraits::Mapping} is implemented by the macro to evaluate to 
  \p{TMapped>}.
  The implementations of methods \alib{boxing;BoxTraits::Write} and \alib{boxing;BoxTraits::Read}
  are to be provided with the variadic macro arguments.

  This macro belongs to set of sibling macros. For a customization of boxing the best
  suitable macro should be chosen. The set is
  - \b   ALIB_BOXING_CUSTOMIZE
  - \ref ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING
  - \ref ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE
  - \ref ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE
  - \ref ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE_NON_UNBOXABLE
  - \ref ALIB_BOXING_CUSTOMIZE_DENY_BOXING

  \see
     For more information, consult chapter \ref alib_boxing_customizing "7. Customizing Boxing"
     of the Programmer's Manual of module \alib_boxing.

  @param TSource  The C++ 'source' type to specialize struct \alib{boxing;BoxTraits} for.
  @param TMapped  The target type to map \p{TSource} to.
  @param ...      Definition of \c static \c inline methods \alib{boxing;BoxTraits::Write} and
                  \alib{boxing;BoxTraits::Read}.


\def ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING

  Specializes template struct \alib{boxing;BoxTraits}.
  This version of \ref ALIB_BOXING_CUSTOMIZE "a set of sibling macros", is to be used when
  simple type mappings are to be performed between types that are statically castable, or
  when boxing as pointer should be enforced.

  The macro provides the implementations of all three entities of the type trait \b BoxTraits:
  - Type \alib{boxing::BoxTraits;Mapping} evaluates to \p{TMapped}.
  - Static method \alib{boxing::BoxTraits;Write} is defined to do static cast of the
    source value to the destination type and to then pass the result to
    \alib{boxing;Placeholder::Write}.
  - Static method \alib{boxing;BoxTraits::Read} is defined to read the mapped type
    using \alib{boxing;Placeholder::Read;Placeholder::Read<TMapped>} and statically cast
    this to \p{TSource}.

  \see
    Macro \ref ALIB_BOXING_CUSTOMIZE for information about the sibling macros.

  @param TSource  The C++ 'source' type to specialize struct \alib{boxing;BoxTraits} for.
  @param TMapped  The target type to map \p{TSource} to.

\def ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE

  Specializes template struct \alib{boxing;BoxTraits}.
  This version of \ref ALIB_BOXING_CUSTOMIZE "a set of sibling macros", is to be used when
  a simple type mapping is to be performed for types \p{TSource} that are statically castable
  to \p{TMapped}.<br>
  In contrast to macro \ref ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING, with this macro, unboxing
  the source type is denied.

  The macro provides the implementations of all three entities of the type trait \b BoxTraits:
  - Type \alib{boxing::BoxTraits;Mapping} evaluates to using \p{TMapped}.
  - Static method \alib{boxing::BoxTraits;Write} performs a static cast of the
    source value to the destination type and then passes the result to
    \alib{boxing;Placeholder::Write}.
  - Static method \alib{boxing;BoxTraits::Read} is declared to return \c void, with
    no implementation given.

  \see
    Macro \ref ALIB_BOXING_CUSTOMIZE for information about the sibling macros.

  @param TSource  The C++ 'source' type to specialize struct \alib{boxing;BoxTraits} for.
  @param TMapped  The target type to map \p{TSource} to.


\def ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE

  Specializes template struct \alib{boxing;BoxTraits}, to customize boxing for given
  boxable type \p{TSource} to be mapped to array type \p{TMapped[]}.

  Type definition \alib{boxing;BoxTraits::Mapping} is implemented by the macro to evaluate to
  \p{TMapped}.
  The implementations of methods \alib{boxing;BoxTraits::Write} and \alib{boxing;BoxTraits::Read}
  are to be provided with the variadic macro arguments.

  \see
    Macro \ref ALIB_BOXING_CUSTOMIZE for information about the sibling macros.

  @param TSource  The C++ 'source' type to specialize struct \alib{boxing;BoxTraits} for.
  @param TElement The element type of the array to map \p{TSource} to.
  @param ...      Definition of \c static \c inline methods \alib{boxing;BoxTraits::Write} and
                  \alib{boxing;BoxTraits::Read}.

\def ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE_NON_UNBOXABLE
  Specializes template struct \alib{boxing;BoxTraits}, to customize boxing for given
  boxable type \p{TSource} to be mapped to array type \p{TMapped[]}.

  Type definition \alib{boxing;BoxTraits::Mapping} is implemented by the macro to evaluate to
  \p{TMapped} and \alib{boxing;BoxTraits::IsArray} evaluates to \c true.<br>
  Static method \alib{boxing;BoxTraits::Read} is declared to return \c void, with 
  no implementation given (as it will not be called).<br>
  The implementation of method \alib{boxing;BoxTraits::Write} is to be provided with the variadic
  portion of the macro arguments.

  \see
    Macro \ref ALIB_BOXING_CUSTOMIZE for information about the sibling macros.

  @param TSource  The C++ 'source' type to specialize struct \alib{boxing;BoxTraits} for.
  @param TElement The element type of the array to map \p{TSource} to.
  @param ...      Definition of \c static \c inline method \alib{boxing;BoxTraits::Write}.

\def ALIB_BOXING_CUSTOMIZE_DENY_BOXING
  Specializes template struct \alib{boxing;BoxTraits}, to completely forbid boxing and unboxing
  of type \p{TSource}.

  For this, type definition \alib{boxing;BoxTraits::Mapping} is implemented by the macro to evaluate 
  to \p{TMapped} and just declarations of static methods
  \alib{boxing;BoxTraits::Write} and \alib{boxing;BoxTraits::Read} are given.<br>
  The latter is declared to return \c void. Note that this in principle is not necessary because
  using \alib{boxing;NotBoxableTag} already forbids unboxing. Nevertheless, this way
  concept \alib{boxing;IsLocked} is satisfied for \p{TSource} as well.

  \see
    Macro \ref ALIB_BOXING_CUSTOMIZE for information about the sibling macros.

  @param TSource  The type that should be denied to be used with \alib_boxing_nl.
                                

\def ALIB_BOXING_VTABLE_DECLARE
  Declares an \c extern object of type \alib{boxing::detail;VTable} named \p{Identifier} in
  namespace \ref alib::boxing::detail.<br>
  Furthermore specializes the type trait \alib{boxing;VTableOptimizationTraits} for
  mapped type \p{TMapped} to have its method \b Get return the extern <em>VTable</em>-instance.

  This macro has to be placed in a header included by each compilation unit that
  creates or accesses boxes of type \p{TMapped}.

  \see
    - Chapter \ref alib_boxing_more_opt_staticvt of the Programmer's Manual of module 
      \alib_boxing_nl.
    - Macros \ref ALIB_BOXING_VTABLE_DEFINE and \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER.

  @param TMapped     The mapped type to declare a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.


\def ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE
  Same as \ref ALIB_BOXING_VTABLE_DECLARE, but used with mapped array types.
  Specializes \alib{boxing;VTableOptimizationTraits} for mapped type
  \p{TMapped}.

  \see
     Macros \ref ALIB_BOXING_VTABLE_DECLARE (for plain types), \ref ALIB_BOXING_VTABLE_DEFINE,
     \ref ALIB_BOXING_VTABLE_DEFINE and \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER.
    
  @param TMapped     The mapped type to declare a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.


\def ALIB_BOXING_VTABLE_DEFINE
  Defines the external object declared with \ref ALIB_BOXING_VTABLE_DECLARE.

  This macro has to be placed in a compilation unit.

  @param TMapped     The mapped type to define a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.


\def ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE
  Defines the external object declared with \ref ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE.

  This macro has to be placed in a compilation unit.

  @param TMapped     The mapped type to define a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.


\def ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER
  Registers a statically created \e vtable singleton declared with either
  \ref ALIB_BOXING_VTABLE_DECLARE or \ref ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE.

  This macro has to be placed in the \ref alib_mod_bs "bootstrap code" of software.
  In debug-compilations, this macro is empty.
  \see
    Chapter \ref alib_boxing_more_opt_staticvt of the \ref alib_mod_boxing "Programmer's Manual"
    of module \alib_boxing_nl.

  @param Identifier  The identifier name of the \e vtable singleton as passed to macros
                     \ref ALIB_BOXING_VTABLE_DECLARE or \ref ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE.   

\def ALIB_BOXING_DEFINE_FEQUALS_FOR_COMPARABLE_TYPE
  This macro \alib{boxing;BootstrapRegister;registers} templated box-function
  \alib{boxing;FEquals::ComparableTypes} for given mapped type \p{TComparable}.

  If a type is boxed as pointer type, then \p{TComparable} has to be given as pointer type.
  For comparison, the unboxed pointers will be dereferenced. If both are \e nulled, \c true
  is returned, if one is \e nulled, \c false.

  As all function registrations have to be performed at run-time, this macro is to be placed in the
  \ref alib_mod_bs "bootstrap section" of an application.

  @param TComparable  The comparable type name.


\def ALIB_BOXING_DEFINE_FISLESS_FOR_COMPARABLE_TYPE
  This macro \alib{boxing;BootstrapRegister;registers} templated box-function
  \alib{boxing;FIsLess::ComparableTypes} for given type \p{TComparable}.

  If a type is boxed as pointer type, then \p{TComparable} has to be given as pointer type.
  For comparison, the unboxed pointers will be dereferenced. If both are \e nulled, \c false
  is returned, if only \p{lhs} is \e nulled, \c true and if only \p{rhs} is \e nulled, then
  \c false.

  As all function registrations have to be performed at run-time, this macro is to be placed in the
  \ref alib_mod_bs "bootstrap section" of an application.

  @param TComparable  The comparable type name.


\def ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE

  This macro registers templated box-function implementation \alib{boxing;FAppend::Appendable}
  for the given type \p{TAppendable}.

  The macro is to be placed in the bootstrap section of an application, for any type that is
  \ref alib_strings_assembly_ttostring "appendable" to class \b %AString.

  This macro selects version box-function type <b>%FAppend\<character\></b>.
  If a custom type is appendable to two or three underlying character types of class \b %AString,
  then macros
  - \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N,
  - \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W and
  - \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X

  should be used. Note that for each character type of \alib_strings_nl, a different
  box-function is registered.

  \note
    As by default, custom types get boxed as pointers, the type in question usually has to be
    provided here as pointer type, for example:

           ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( my_namespace::MyType* )

  @param TAppendable  The appendable type.


\def ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N
  Same as macro \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE but implements
  interface \b FAppend<nchar> instead of \b FAppend<character>.

  @param TAppendable  The appendable type.


\def ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W
  Same as macro \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE but implements
  interface \b FAppend<wchar> instead of \b FAppend<character>.

  @param TAppendable  The appendable type.


\def ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X
  Same as macro \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE but implements
  interface \b FAppend<xchar> instead of \b FAppend<character>.

  @param TAppendable  The appendable type.
  

\def ALIB_BOXING_VTABLE_DECLARE_NOEXPORT
  This is an experimental version of \ref ALIB_BOXING_VTABLE_DECLARE, which has to be used, if:
  - Symbol \ref ALIB_C20_MODULES is set with compiling \alib, and
  - the using code does not itself compile into C++20 Modules (just consuming them).
  The difference to the original is that symbol \ref ALIB_EXPORT is not used with this version.  

  @param TMapped     The mapped type to declare a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.


\def ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE_NOEXPORT
  This is an experimental version of \ref ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE, which has to be 
  used, if:
  - Symbol \ref ALIB_C20_MODULES is set with compiling \alib, and
  - the using code does not itself compile into C++20 Modules (just consuming them).
  The difference to the original is that symbol \ref ALIB_EXPORT is not used with this version.  
    
  @param TMapped     The mapped type to declare a \e vtable singleton for.
  @param Identifier  The identifier name of the \e vtable singleton.




@}


\I{################################################################################################}
\I{################################     ALib Module Characters      #############################  }
\I{################################################################################################}

\name ALib Module Characters

\anchor GrpALibMacros_mod_characters
The macros in this section are introduced by module \alib_characters.

@{
\def A_CHAR
  Macro used to express C++ character and string literals in a platform-independent way.
  Dependent on code selection symbols \ref ALIB_CHARACTERS_WIDE and eventually
  \ref ALIB_CHARACTERS_NATIVE_WCHAR, this macro possibly prepends character <c>'L'</c>, <c>'u'</c>
  or <c>'U'</c> to the character or string literal given with parameter \p{STR}.

  Whenever software targets different platforms that change the underlying character width of
  the string-types, the string literals have to change as well. To be able to compile on different
  platforms while using different compiler-symbols to manipulate the character width, almost all string
  literals in \alib sources are enclosed by this macro. The exception concerns such
  literals which are assigned to narrow string type \alib{NString}, or those where explicit
  string types \alib{WString} or \alib{XString} are used.

  \note To avoid too much code clutter, this macro, as well as its siblings
        \ref A_CCHAR, \ref A_SCHAR, \ref A_NCHAR, \ref A_WCHAR and \ref A_XCHAR,
        have an abbreviated name, as they are quite frequently used.
        (According to the \ref alib_manual_appendix_naming "ALib naming scheme", the macros would
        be prefixed with <b>"ALIB_CHARACTERS_"</b>)

  \see For more information, consult chapter \ref alib_characters_prepro_literals
       of the \ref alib_mod_characters "Programmer's Manual" of module \alib_characters_nl.

  @param STR The character or string literal to be eventually prefix with <c>'L'</c>, <c>'u'</c> or <c>'U'</c>.

\def A_CCHAR
  Macro used to express C++ string literals in a platform-independent way. Dependent on
  the definition of type \alib{characters;complementChar} (which is in turn dependent on the platform, the
  compiler and optional compiler-symbols), this macro defines string literals, which have either
  a one, two or four byte character width, namely the width corresponding to the width of
  \b %complementChar.

  \see Sibling macro \ref A_CHAR for more information.
       Further sibling macros are \ref A_SCHAR, \ref A_NCHAR, \ref A_WCHAR and \ref A_XCHAR.<br>
       Details are described in chapter \ref alib_characters_prepro_literals
       of the \ref alib_mod_characters "Programmer's Manual" of module \alib_characters_nl.


   @param STR The character or string literal to eventually prefix with <c>'u'</c>, <c>'U'</c> or <c>'L'</c> .

\def A_SCHAR
  Macro used to express C++ string literals in a platform-independent way. Dependent on
  the definition of type \alib{characters;strangeChar} (which is in turn dependent on the platform, the
  compiler and optional compiler-symbols), this macro defines string literals, which have either
  a two or four byte character width, namely the width corresponding to the width of
  \b %complementChar.

  \note
    The same as type \alib{characters;strangeChar} is always equivalent to type \alib{characters;xchar},
    this macro is always equivalent to macro \ref A_XCHAR.

  \see Sibling macro \ref A_CHAR for more information.
       Further sibling macros are \ref A_CCHAR, \ref A_NCHAR, \ref A_WCHAR and \ref A_XCHAR.<br>
       Details are described in chapter \ref alib_characters_prepro_literals
       of the \ref alib_mod_characters "Programmer's Manual" of module \alib_characters_nl.


   @param STR The character or string literal to prefix with <c>'u'</c>, <c>'U'</c> or <c>'L'</c> .

\def A_NCHAR
  This is an "ident macro" that just produces the given parameter \p{STR}. It is provided only for
  completeness. \alib does not use this macro internally, as string literals that are not surrounded
  by one of the character literal macros are very well identifiable as narrow literals.

  \see Sibling macro \ref A_CHAR for more information.
       Further sibling macros are \ref A_CCHAR, \ref A_SCHAR, \ref A_WCHAR and \ref A_XCHAR.<br>
       Details are described in chapter \ref alib_characters_prepro_literals
       of the \ref alib_mod_characters "Programmer's Manual" of module \alib_characters_nl.

  @param STR The character or string literal that is returned as is.


\def A_WCHAR
  Macro used to express C++ string literals in a platform-independent way. Dependent on
  the definition of type \alib{characters;wchar} (which is in turn dependent on the platform, the
  compiler and optional compiler-symbols), this macro defines string literals, which have an either
  two bytes or four byte character width, namely the width corresponding to the width of
  \b %wchar.

  \see Sibling macro \ref A_CHAR for more information.
       Further sibling macros are \ref A_CCHAR, \ref A_SCHAR, \ref A_NCHAR and \ref A_XCHAR.<br>
       Details are described in chapter \ref alib_characters_prepro_literals
       of the \ref alib_mod_characters "Programmer's Manual" of module \alib_characters_nl.


   @param STR The character or string literal to prefix with <c>'u'</c>, <c>'U'</c> or <c>'L'</c> .

\def A_XCHAR
  Macro used to express C++ string literals in a platform-independent way. Dependent on
  the definition of type \alib{characters;xchar} (which is in turn dependent on the platform, the
  compiler and optional compiler-symbols), this macro defines string literals, which have an either
  two bytes or four byte character width, namely the width corresponding to the width of
  \b %xchar.

  \note
    The same as type \alib{characters;strangeChar} is always equivalent to type \alib{characters;xchar},
    this macro is always equivalent to macro \ref A_SCHAR.

  \see Sibling macro \ref A_CHAR for more information.
       Further sibling macros are \ref A_CCHAR, \ref A_SCHAR, \ref A_NCHAR and \ref A_WCHAR.<br>
       Details are described in chapter \ref alib_characters_prepro_literals
       of the \ref alib_mod_characters "Programmer's Manual" of module \alib_characters_nl.

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
  The rationale here is, that the preprocessor can only choose code depending on integral values
  and not on string contents.

\def ALIB_CHAR_TYPE_ID_CHARACTER
  Defined as \ref ALIB_CHAR_TYPE_ID_N or \c ALIB_CHAR_TYPE_ID_W, depending symbol
  \ref ALIB_CHARACTERS_WIDE.

\def ALIB_CHAR_TYPE_ID_COMPLEMENT
  Defined as \ref ALIB_CHAR_TYPE_ID_N or \c ALIB_CHAR_TYPE_ID_W, depending symbol
  \ref ALIB_CHARACTERS_WIDE.

\def ALIB_CHAR_TYPE_ID_STRANGE
  Defined as \ref ALIB_CHAR_TYPE_ID_X.

ALIB_CHARACTERS_WIDE
\def ALIB_CHARACTER_ARRAY
  Specializes the type trait \alib{characters;ArrayTraits} for type \p{T}.

  To implement the corresponding static methods in alignment with the specialized access and construction
  flags, macros
  \ref ALIB_CHARACTER_ARRAY_IMPL_BUFFER,
  \ref ALIB_CHARACTER_ARRAY_IMPL_LENGTH and
  \ref ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT are proposed to be used.

  \see
    For more information about character array traits, see chapter
    \ref alib_characters_arrays "4. Character Arrays" of  the Programmer's Manual of module \alib_characters.

  @param T            The type to provide array type-traits for.
  @param TChar        The character type of character arrays that \p{T} represents or might be created of.
  @param Access       One of the values \b NONE, \b Implicit or \b ExplicitOnly.
                      Value \b Mutable is not permitted. Instead, macro \ref ALIB_CHARACTER_ARRAY_MUTABLE
                      is to be used for that case.
  @param Construction One of the values \b NONE, \b Implicit or \b ExplicitOnly.


\def ALIB_CHARACTER_ARRAY_MUTABLE
  Specializes the type trait \alib{characters;ArrayTraits} for type \p{T} with
  access modifier \b Mutable.

  To implement the corresponding static methods in alignment with the specialized access and construction
  flags, macros
  \ref ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE,
  \ref ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE and
  \ref ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT are proposed to be used.

  \see
    For more information about character array traits, see chapter
    \ref alib_characters_arrays "4. Character Arrays" of  the Programmer's Manual of module \alib_characters.

  @param T            The type to provide array type-traits for.
  @param TChar        The character type of character arrays that \p{T} represents or might be created of.
  @param Construction One of the values \b NONE, \b Implicit or \b ExplicitOnly.


\def ALIB_CHARACTER_ZT_ARRAY
  Specializes the type trait \alib{characters;ZTArrayTraits} for type \p{T}.

  To implement the corresponding static methods in alignment with the specialized access and construction
  flags, macros
  \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER,
  \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH and
  \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT are proposed to be used.

  \see
    For more information about character array traits, see chapter
    \ref alib_characters_arrays "4. Character Arrays" of  the Programmer's Manual of module \alib_characters.

  @param T            The type to provide array type-traits for.
  @param TChar        The character type of character arrays that \p{T} represents or might be created of.
  @param Access       One of the values \b NONE, \b Implicit or \b ExplicitOnly.
                      Value \b Mutable is not permitted. Instead, macro \ref ALIB_CHARACTER_ARRAY_MUTABLE
                      is to be used for that case.
  @param Construction One of the values \b NONE, \b Implicit or \b ExplicitOnly.


\def ALIB_CHARACTER_ZT_ARRAY_MUTABLE
  Specializes the type trait \alib{characters;ZTArrayTraits} for type \p{T} with
  access modifier \b Mutable.

  To implement the corresponding static methods in alignment with the specialized access and construction
  flags, macros
  \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE,
  \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE and
  \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT are proposed to be used.

  \see
    For more information about character array traits, see chapter
    \ref alib_characters_arrays "4. Character Arrays" of  the Programmer's Manual of module \alib_characters.

  @param T            The type to provide array type-traits for.
  @param TChar        The character type of character arrays that \p{T} represents or might be created of.
  @param Construction One of the values \b NONE, \b Implicit or \b ExplicitOnly.


\def ALIB_CHARACTER_ARRAY_IMPL_BUFFER
  This macro may be used to implement static method \alib{characters;ArrayTraits::Buffer} of
  specializations of \b ArrayTraits that have been defined using macro \ref ALIB_CHARACTER_ARRAY.

  If macro \ref ALIB_CHARACTER_ARRAY_MUTABLE was used, corresponding macro
  \ref ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE has to be used instead of this one.

  The argument providing a constant reference of type \p{T} to the method's implementation, and
  which has to be accessed in the given implementation code, is named \p{src}.
  The implementation has to return a constant pointer to an array of character type \p{TChar}.

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER
  Same as \ref ALIB_CHARACTER_ARRAY_IMPL_BUFFER, but for zero-terminated character arrays.<br>
  (Implements method \alib{characters;ZTArrayTraits::Buffer} instead of
  \alib{characters;ArrayTraits::Buffer}.)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE
  Alternative macro version of \ref ALIB_CHARACTER_ARRAY_IMPL_BUFFER, which declares
  method argument \p{src} as a mutual reference of type \p{T}.

  This version is to be used if specialization was performed using
  \ref ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE.

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE
  Same as \ref ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE, but for zero-terminated character arrays.<br>
  (Implements method \alib{characters;ZTArrayTraits::Buffer} instead of
  \alib{characters;ArrayTraits::Buffer}.)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ARRAY_IMPL_LENGTH
  This macro may be used to implement static method \alib{characters;ArrayTraits::Length} of
  specializations of \b ArrayTraits that have been defined using macro \ref ALIB_CHARACTER_ARRAY.

  The argument providing a constant reference of type \p{T} to the method's implementation, and
  which has to be accessed in the given implementation code, is named \p{src}.
  The implementation has to return the length of the character array as type \alib{integer}.

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH
  Same as \ref ALIB_CHARACTER_ARRAY_IMPL_LENGTH, but for zero-terminated character arrays.<br>
  (Implements method \alib{characters;ZTArrayTraits::Length} instead of
  \alib{characters;ArrayTraits::Length}.)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE
  Alternative macro version of \ref ALIB_CHARACTER_ARRAY_IMPL_LENGTH, which declares
  method argument \p{src} as a mutual reference of type \p{T}.

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE
  Same as \ref ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE, but for zero-terminated character arrays.<br>
  (Implements method \alib{characters;ZTArrayTraits::Length} instead of
  \alib{characters;ArrayTraits::Length}.)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.

\def ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT
  This macro may be used to implement static method \alib{characters;ArrayTraits::Construct} of
  specializations of \b ArrayTraits that have been defined using macro \ref ALIB_CHARACTER_ARRAY
  or \ref ALIB_CHARACTER_ARRAY_MUTABLE.

  The arguments providing the array data to the method's implementation, which is to be used to
  create the object of type \p{T}, are named \p{array} and \p{length}.
  The implementation has to return a value of type \p{T}.

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be
                created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.


\def ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT
  Same as \ref ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT, but for zero-terminated character arrays.<br>
  (Implements method \alib{characters;ZTArrayTraits::Construct} instead of
  \alib{characters;ArrayTraits::Construct}.)

  @param T      The type to provide the specialized static method for.
  @param TChar  The character type of character arrays that \p{T} represents or might be
                created of.
  @param ...    The variadic arguments of the macro constitute the implementation code
                of the method.
@}



\I{################################################################################################}
\I{############################      ALib Module Config        ##################################  }
\I{################################################################################################}
\name ALib Module Config

\anchor GrpALibMacros_mod_config
The macros in this section are introduced by \alibcamp \alib_variables.

@{
\def ALIB_VARIABLES_DEFINE_TYPE
  Defines a struct named <c>alib::variables::detail::VMeta_<<CPPName>></c>, which
  is derived from struct \alib{variables;VMeta} and which declares all abstract virtual
  methods and implements most of them already. The methods are:
  Method                           | Status
  ---------------------------------|--------------------
  \alib{variables::VMeta;typeName}    | defined
  \alib{variables::VMeta;dbgTypeID}   | defined (in debug-compilations only)
  \alib{variables::VMeta;construct}   | defined
  \alib{variables::VMeta;destruct}    | defined
  \alib{variables::VMeta;size}        | defined
  \alib{variables::VMeta;imPort}      | declared only
  \alib{variables::VMeta;exPort}      | declared only

  This is the first step on the way to create an \alib variable of a custom storage type.

  \see
    - Sibling macro \ref ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR.
    - See struct \alib{variables;VMeta}.
    - See chapter \ref alib_variables_types_custom of the Programmer's Manual of camp \alib_variables_nl
      for more details.
    - Macro \ref ALIB_VARIABLES_REGISTER_TYPE for how to register a variable type with
      a configuration
    Chapter \ref alib_strings_assembly_ttostring of the Programmer's Manual of module
    \alib_strings.

  @param Namespace      The namespace of the type to enable as an \alib variable.
  @param CPPName        The name of the type to enable as an \alib variable.
  @param CfgTypeString  The type string used to declare variables of this type.

\def ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR
  Same as \ref ALIB_VARIABLES_DEFINE_TYPE with the only difference that virtual override
  \alib{variables::;VMeta::construct} passes the pool allocator of the configuration
  instance to the constructor of the custom type.
  @param Namespace      The namespace of the type to enable as an \alib variable.
  @param CPPName        The name of the type to enable as an \alib variable.
  @param CfgTypeString  The type string used to declare variables of this type.

\def ALIB_VARIABLES_REGISTER_TYPE(CPPName)
  Registers a custom implementation of abstract class \alib{variables;VMeta}, which has been
  previously defined with macro \ref ALIB_VARIABLES_DEFINE_TYPE with the configuration
  object received by a call to <c>GetConfig()</c>.
  This macro is to be placed in \alib{camp;Camp::Bootstrap} in the section of phase
  \alib{BootstrapPhases;PrepareConfig}.
  The macro invokes method \alib{variables;Configuration::RegisterType}, which has to be called
  'manually' if for some reason the placement of registration can't be done as proposed.

  @param CPPName   The name of the type to enable as an \alib variable (excluding the namespace).
@}

\I{################################################################################################}
\I{#############################      ALib Module Enums      ####################################  }
\I{################################################################################################}
\name ALib Module Enums

\anchor GrpALibMacros_mod_enums

The macros in this section are introduced by modules \alib_enumops and \alib_enumrecords.
They allow specializing the following type traits for scoped or non-scoped enum types:
\alib{enumrecords;RecordsTraits},
\alib{enumops;ArithmeticalTraits},
\alib{enumops;BitwiseTraits} and
\alib{enumops;IterableTraits} 

\attention
- All macros in this section have to be placed in global scope (outside namespaces and types).
- For technical reasons, neither the macros nor the concepts behind them are applicable to
  enum types that are defined as \c private or \c protected inner types of structs or classes.

@{

\I{################################################################################################}
\def ALIB_ENUMS_ASSIGN_RECORD
  Macro used to specialize the type trait
  \alib{enumrecords;RecordsTraits;RecordsTraits<TEnum>} to associate C++ enumeration type \p{TEnum} with
  \ref alib_enums_records "ALib Enum Records" of type \p{TRecord}.

  @param TEnum    The enumeration type to define data records for.
  @param TRecord  The type of the data record to assign.

\I{############### ArithmeticalTraits (has to be doxed here inside this subgroup) #########  }

\def ALIB_ENUMS_MAKE_ARITHMETICAL
  Specializes the type trait \alib{enumops;ArithmeticalTraits} to inherit \c std::true_type to
  enable a set of arithmetic operators on the elements of \p{TEnum}.

  Usually, this macro is placed in a header-file, probably close to the enum type definition.
  However, it has to be placed <b>in global (no) namespace</b>.

  \see Chapter \ref alib_enums_arithmetic_standard of the Programmer's Manual of module
       \alib_enumops for more information.

  @param TEnum    Type of a scoped or non-scoped enumeration that is to be declared an arithmetical
                  type.

\I{################## BitwiseTraits (has to be doxed here inside this subgroup) ###########  }

\def ALIB_ENUMS_MAKE_BITWISE
  Specializes the type trait \alib{enumops;BitwiseTraits} to inherit \c std::true_type.

  Usually, this macro is placed in a header-file, probably close to the enum type definition.
  However, it has to be placed <b>in global (no) namespace</b>.

  \see Chapter \ref alib_enums_arithmetic_bitwise of the Programmer's Manual of module
       \alib_enumops for more information.

  @param TEnum    Type of a scoped or non-scoped enumeration that is to be declared a bitwise
                  type.


\I{################## IterableTraits (has to be doxed here inside this subgroup) ###############  }

\def ALIB_ENUMS_MAKE_ITERABLE
  Shortcut to \ref ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END, providing <c>TEnum(0)</c> as macro
  parameter \p{StartElement}.

  Usually, this macro is placed in a header-file, probably close to the enum type definition.
  However, it has to be placed <b>in global (no) namespace</b>.

  \see Sibling macro \ref ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END and chapter
       \ref alib_enums_iter "3. Enum Iteration" of the Programmer's Manual of module
       \alib_enumops for more information.

  @param TEnum         Type of a scoped or non-scoped enumeration that is to be declared an
                       iterable enum type.
  @param StopElement   The enum element after the last "valid" element in the enumeration.<br>
                       Will be used as \c constexpr \alib{enumops;IterableTraits::End}.

\def ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END
  Specializes the type trait \alib{enumops;IterableTraits} to implement methods:
  - \b %Begin to return \p{StartElement}
  - \b %End to return \p{StopElement}

  Usually, this macro is placed in a header-file, probably close to the enum type definition.
  However, it has to be placed <b>in global (no) namespace</b>.

  \see Sibling macro \ref ALIB_ENUMS_MAKE_ITERABLE and chapter
       \ref alib_enums_iter "3. Enum Iteration" of the Programmer's Manual of module
       \alib_enumops for more information.
  \see

  @param TEnum        Type of a scoped or non-scoped enumeration that is to be declared an
                      iterable enum type.
  @param StartElement The first element of the enumeration.<br>
                      Will be used as \c constexpr \alib{enumops;IterableTraits::End}.
  @param StopElement  The enum element after the last "valid" element in the enumeration.<br>
                      Will be used as \c constexpr \alib{enumops;IterableTraits::End}.

@}

\I{################################################################################################}
\I{############################     ALib Module Expressions     #################################  }
\I{################################################################################################}
\name ALib Module Expressions

\anchor GrpALibMacros_mod_expressions
The macros in this section are introduced by module \alib_expressions.

@{
\def CALCULUS_CALLBACK

  Writes given callback function name \p{func} along with a comma (<c>','</c>) and a "stringified"
  version of the C++ function name.

  In release compilations, the macro resolves just to \p{func}.

  This macro is to be used to define records of types
  \alib{expressions::plugins;Calculus::FunctionEntry},
  \alib{expressions::plugins;Calculus::OperatorTableEntry} and to provide parameters to function
  \alib{expressions::plugins;Calculus::AddOperator}.

  @param func     The name of the callback function.


\def CALCULUS_DEFAULT_AUTOCAST

  This macro is to be used to define records of type
  \alib{expressions::plugins;Calculus::AutoCastEntry}.

  With debug-builds, it provides \c nullptr for table entries
 \doxlinkproblem{structalib_1_1expressions_1_1plugins_1_1Calculus_1_1AutoCastEntry.html;ab0a3a9d76927f2237a65d8b2df28469e;AutoCastEntry::Callback}
 and
 \doxlinkproblem{structalib_1_1expressions_1_1plugins_1_1Calculus_1_1AutoCastEntry.html;aa7e29bf0978a68fbdf9533a9e73516db;AutoCastEntry::DbgCallbackName}.
  In release compilations the latter is omitted.


\def CALCULUS_SIGNATURE

  This macro is to be used to define records of type
  \alib{expressions::plugins;Calculus::FunctionEntry}.

  The macro resolves to two values separated by a comma (<c>','</c>).
  The first is a plain copy of given \p{BoxPointerArray}, which has to be a C++ array of pointers
  to type \b Box. The second value written will be the array's extent.

  @param BoxPointerArray  \c nullptr for identifiers, otherwise the list of argument sample boxes
                          provided as a <b>Box**</b>.
@}


\I{################################################################################################}
\I{############################      ALib Module Strings       ##################################  }
\I{################################################################################################}
\name ALib Module Strings

\anchor GrpALibMacros_mod_strings
The macros in this section are introduced by module \alib_strings.

@{
\def ALIB_STRINGS_APPENDABLE_TYPE
  Helper macro for specializing functor \alib{strings;AppendableTraits} for type \p{TYPE}.
  This macro has to be positioned outside any namespace, and the given type has to include
  its full namespace qualification.

  This macro is to be used in combination with macro #ALIB_STRINGS_APPENDABLE_TYPE_DEF.<br>
  As an alternative to the two macros, #ALIB_STRINGS_APPENDABLE_TYPE_INLINE might be used, which
  will specialize \b AppendableTraits and define its \b operator() inline.

  \see
    Chapter \ref alib_strings_assembly_ttostring of the Programmer's Manual of module
    \alib_strings.

  @param TYPE The type to specialize functor \b AppendableTraits for.


\def ALIB_STRINGS_APPENDABLE_TYPE_N
  Same as \ref ALIB_STRINGS_APPENDABLE_TYPE but for character type \ref alib::nchar.

  @param TYPE The type to specialize functor \b AppendableTraits for.


\def ALIB_STRINGS_APPENDABLE_TYPE_W
  Same as \ref ALIB_STRINGS_APPENDABLE_TYPE but for character type \ref alib::wchar.

  @param TYPE The type to specialize functor \b AppendableTraits for.


\def ALIB_STRINGS_APPENDABLE_TYPE_DEF
  Macro used in combination with \ref ALIB_STRINGS_APPENDABLE_TYPE which specializes functor
  \alib{strings;AppendableTraits} for type \p{TYPE} and standard character type, and with this declares its
  member \b operator().<br>
  This macro is used for the implementation of this member function.

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_DEF_N
  Same as \ref ALIB_STRINGS_APPENDABLE_TYPE_DEF but for character type \ref alib::nchar.

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_DEF_W
  Same as \ref ALIB_STRINGS_APPENDABLE_TYPE_DEF but for character type \ref alib::wchar.

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_INLINE
  Helper macro for specializing functor \alib{strings;AppendableTraits} for a custom type \p{TYPE}.
  This macro has to be positioned outside any namespace, and the given type has to include
  its full namespace qualification.

  With the specialization of struct, <b>AppendableTraits<TYPE>::operator()</b> will be defined and
  implemented inline.

  Macros #ALIB_STRINGS_APPENDABLE_TYPE and #ALIB_STRINGS_APPENDABLE_TYPE_DEF
  provide a non-inline alternative to this macro.

  \see
    Chapter \ref alib_strings_assembly_ttostring of the Programmer's Manual of module
    \alib_strings.

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_INLINE_N
  Same as \ref ALIB_STRINGS_APPENDABLE_TYPE_INLINE but for character type \ref alib::nchar.

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_APPENDABLE_TYPE_INLINE_W
  Same as \ref ALIB_STRINGS_APPENDABLE_TYPE_INLINE but for character type \ref alib::wchar.

  @param TYPE The type to specialize functor \b AppendableTraits for.
  @param IMPL The implementation code for \b operator().


\def ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR
  Helper macro for specializing type trait
  \alib{strings::compatibility::std;SuppressStdOStreamOpTraits} for a custom type \p{TYPE}.
  This macro has to be positioned outside any namespace, and the given type has to include
  its full namespace qualification.

  @param TYPE The type to specialize functor \b AppendableTraits for.


\def ALIB_STRING_DBG_CHK
  Simple macro that just invokes method _dbgCheck(), which is defined for classes
  \alib{strings;TString;String},
  \alib{strings;TCString;CString} and
  \alib{strings;TAString;AString}.
  It is active only when the compiler-symbol \ref ALIB_DEBUG_STRINGS is \c true.
  The macro is placed in almost every method.



\def ALIB_STRINGS_TO_NARROW
  Creates a new local string variable of narrow character type.
  The name of the variable is defined by parameter \p{dest}.
  If code selection symbol \ref ALIB_CHARACTERS_WIDE is \c false, then \p{dest} becomes just a reference
  to \p{src}. The macro in this case is defined as:

        decltype(src)& dest= src;

  Otherwise, \p{dest} is of type \alib{strings;TLocalString;LocalString<nchar,bufSize>}
  and \p{src} is passed to its constructor.

  @param src        The source string.
  @param dest       The name of the destination variable.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_FROM_NARROW
  Creates a new local string variable of standard \alib{characters;character} type.
  The name of the variable is defined by parameter \p{dest}.
  If code selection symbol \ref ALIB_CHARACTERS_WIDE is \c false, then \p{dest} becomes just a reference
  to \p{src}. The macro in this case is defined as:

        decltype(src)& dest= src;

  Otherwise, \p{dest} is of type \alib{strings;TLocalString;LocalString<character,bufSize>}
  and \p{src} is passed to its constructor.

  @param src        The source string.
  @param dest       The name of the destination variable.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_TO_NARROW_ARG
  Used when \alib strings of standard character size should be passed as a string argument of narrow
  size.
  If code selection symbol \ref ALIB_CHARACTERS_WIDE is \c false, then the macro simply
  passes (is defined as) \p{src}.<br>
  Otherwise, the macro wraps \p{src} in an object of type
  \alib{strings;TLocalString;LocalString<nchar,bufSize>} which converts the string to the right
  character type.

  @param src        The source string.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_FROM_NARROW_ARG
  Used when \alib strings of narrow character size should be passed as a string argument of standard
  size.
  If code selection symbol \ref ALIB_CHARACTERS_WIDE is \c false, then the macro simply
  passes (is defined as) \p{src}.<br>
  Otherwise, the macro wraps \p{src} in an object of type
  \alib{strings;TLocalString;LocalString<character,bufSize>}.
  @param src        The source string.
  @param bufSize    The local buffer size used for the conversion.


\def ALIB_STRINGS_TO_WIDE
  Creates a new local string variable of wide character type.
  The name of the variable is defined by parameter \p{dest}.
  If code selection symbol \ref ALIB_CHARACTERS_WIDE is \c true, then \p{dest} becomes just a reference
  to \p{src}. The macro in this case is defined as:

        decltype(src)& dest= src;

  Otherwise, \p{dest} is of type \alib{strings;TLocalString;LocalString<nchar,bufSize>}
  and \p{src} is passed to its constructor.

  @param src        The source string.
  @param dest       The name of the destination variable.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_FROM_WIDE
  Creates a new local string variable of standard \alib{characters;character} type.
  The name of the variable is defined by parameter \p{dest}.
  If code selection symbol \ref ALIB_CHARACTERS_WIDE is \c true, then \p{dest} becomes just a reference
  to \p{src}. The macro in this case is defined as:

        decltype(src)& dest= src;

  Otherwise, \p{dest} is of type \alib{strings;TLocalString;LocalString<character,bufSize>}
  and \p{src} is passed to its constructor.

  @param src        The source string.
  @param dest       The name of the destination variable.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_TO_WIDE_ARG
  Used when \alib strings of standard character size should be passed as a string argument of wide
  size.
  If code selection symbol \ref ALIB_CHARACTERS_WIDE is \c true, then the macro simply
  passes (is defined as) \p{src}.<br>
  Otherwise, the macro wraps \p{src} in an object of type
  \alib{strings;TLocalString;LocalString<nchar,bufSize>}.

  @param src        The source string.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRINGS_FROM_WIDE_ARG
  Used when \alib strings of wide character size should be passed as a string argument of standard
  size.
  If code selection symbol \ref ALIB_CHARACTERS_WIDE is \c true, then the macro simply
  passes (is defined as) \p{src}.<br>
  Otherwise, the macro wraps \p{src} in an object of type
  \alib{strings;TLocalString;LocalString<character,bufSize>}.

  @param src        The source string.
  @param bufSize    The local buffer size used for the conversion.

\def ALIB_STRING_RESETTER
  Creates an \e 'anonymous' instance of class \alib{strings;TStringLengthResetter}. Its
  identifier name is assembled using macro \ref ALIB_IDENTIFIER, hence from the given
  \b AString's identifier and the line number in the code.

  @param astring The \alib{strings;TAString;AString} to reset to its original length when the
                 C++ block scope where this macro is placed, is left.
@}
                              
\I{################################################################################################}
\I{############################      ALib Module Threads       ##################################  }
\I{################################################################################################}
\name ALib Module Threads

\anchor GrpALibMacros_mod_threads
While macros in this section logically belong to module \alib_threads, they are available
(but empty) also when this module is not included in the \alibbuild. 
An explanation to this is given with chapter \ref alib_threads_intro_agnostic 
of this module's Programmer's Manual.

Their definition depends on the setting of \ref ALIB_DEBUG_CRITICAL_SECTIONS, which defaults
to \c 1, in case module \alib_threads_nl is included in the \alibbuild_nl.

If the symbol is not set, all the symbols in this section are empty.

@{

\def ALIB_LOCK
 Alias of macro \ref ALIB_OWN, providing <c>*this</c> as the owner. 
 Commonly used with types derived from classes \alib{threads;Lock} or 
 \alib{threads;RecursiveLock}.

 \note
    If module \alib_threads is not available in the \alibbuild, this macro is still defined with
    debug-builds.
    In this case, namespace function\alib{assert::SingleThreaded} is invoked. 
    This detects multithreaded use of a non-supporting \alibbuild.

 @see Alternative macro ALIB_LOCK_WITH                                                                         

\def ALIB_LOCK_WITH
 Alternative to macro \ref ALIB_LOCK which allows specifying the ownable. 
 (Macro \b ALIB_LOCK fixes this to <c>*this</c>).
 @param lock  The mutex to acquire and release.

\def ALIB_LOCK_RECURSIVE
 Alternative to macro \ref ALIB_LOCK which uses macro \ref ALIB_OWN_RECURSIVE 
 instead of \ref ALIB_OWN and thus creates an anonymous instance of \alib{lang;OwnerRecursive}
 instead of \alib{lang;Owner}. 
                            
\def ALIB_LOCK_RECURSIVE_WITH
 Alternative to macro \ref ALIB_LOCK_WITH which uses macro \ref ALIB_OWN_RECURSIVE 
 instead of \ref ALIB_OWN and thus creates an anonymous instance of \alib{lang;OwnerRecursive}
 instead of \alib{lang;Owner}. 
 @param lock  The mutex to acquire and release.

\def ALIB_LOCK_SHARED
 Alternative to macro \ref ALIB_LOCK which uses macro \ref ALIB_OWN_SHARED 
 instead of \ref ALIB_OWN and thus creates an anonymous instance of \alib{lang;OwnerShared}
 instead of \alib{lang;Owner}. 
                            
\def ALIB_LOCK_SHARED_WITH
 Alternative to macro \ref ALIB_LOCK_WITH which uses macro \ref ALIB_OWN_SHARED 
 instead of \ref ALIB_OWN and thus creates an anonymous instance of \alib{lang;OwnerShared}
 instead of \alib{lang;Owner}. 
 @param lock  The mutex to acquire and release.
                            
\def ALIB_DCS
 Alias of macro \ref ALIB_OWN, providing <c>*this</c> as the owner. 
 Commonly used with types derived from classes \alib{lang;DbgCriticalSections}.

 \note
    If the compiler-symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is not set, this macro is still defined 
    with debug-builds.
    In this case, namespace function\alib{assert::SingleThreaded} is invoked. 
    This detects multithreaded use of a non-supporting \alibbuild.

 @see 
   - Sibling macros \ref ALIB_DCS_WITH, \ref ALIB_DCS_SHARED, \ref ALIB_DCS_ACQUIRE, etc.      
   - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
                                                                   
                  
\def ALIB_DCS_WITH
 Alternative to macro \ref ALIB_DCS which allows specifying the ownable. 
 (Macro \b ALIB_DCS fixes this to <c>*this</c>).
 @param CS  The \alib{lang;DbgCriticalSections} to acquire and release.

\def ALIB_DCS_SHARED
 Alternative to macro \ref ALIB_DCS which uses macro \ref ALIB_OWN_SHARED 
 instead of \ref ALIB_OWN and thus creates an anonymous instance of \alib{lang;OwnerShared}
 instead of \alib{lang;Owner}. 
                            
\def ALIB_DCS_SHARED_WITH
 Alternative to macro \ref ALIB_DCS_WITH which uses macro \ref ALIB_OWN_SHARED 
 instead of \ref ALIB_OWN and thus creates an anonymous instance of \alib{lang;OwnerShared}
 instead of \alib{lang;Owner}. 
 @param CS  The \alib{lang;DbgCriticalSections} to acquire and release.
                            
\def ALIB_DCS_ACQUIRE
  Calls \alib{lang;DbgCriticalSections::Acquire} (on \c this).
  
\def ALIB_DCS_ACQUIRE_WITH
  Calls \alib{lang;DbgCriticalSections::Acquire} on the instance given with \p{CS}.
  @param CS The \b DbgCriticalSections instance to use. 
  
\def ALIB_DCS_ACQUIRE_SHARED
  Calls \alib{lang;DbgCriticalSections::AcquireShared} (on \c this).
  
\def ALIB_DCS_ACQUIRE_SHARED_WITH
  Calls \alib{lang;DbgCriticalSections::AcquireShared} on the instance given with \p{CS}.
  @param CS The \b DbgCriticalSections instance to use. 
  
\def ALIB_DCS_RELEASE
  Calls \alib{lang;DbgCriticalSections::Release} (on \c this).
  
\def ALIB_DCS_RELEASE_WITH
  Calls \alib{lang;DbgCriticalSections::Release} on the instance given with \p{CS}.
  @param CS The \b DbgCriticalSections instance to use. 
  
\def ALIB_DCS_RELEASE_SHARED
  Calls \alib{lang;DbgCriticalSections::ReleaseShared} (on \c this).
  
\def ALIB_DCS_RELEASE_SHARED_WITH
  Calls \alib{lang;DbgCriticalSections::ReleaseShared} on the instance given with \p{CS}.
  @param CS The \b DbgCriticalSections instance to use. 
  
@}

\I{################################################################################################}
\I{################################     ALib Module Basecamp       ##############################  }
\I{################################################################################################}
\name ALib Module Basecamp

\anchor GrpALibMacros_mod_camp
The macros in this section are introduced by modle \alib_resources.

@{
\def ALIB_RESOURCED
   Macro used to specialize the type trait \alib{resources;ResourcedTraits} for given type 
   \p{T}.

   @param T            The type to specialize the type trait \b %ResourcedTraits for.
   @param ResPool      Expression providing a pointer to the \alib{resources;ResourcePool} 
                       object.
   @param ResCategory  Expression providing the resource category.
   @param ResName      Expression providing the resource name.

\def ALIB_RESOURCED_IN_MODULE
   Variant of macro \ref ALIB_RESOURCED, which specializes the type trait
   \alib{resources;ResourcedTraits} for type \p{T} to use the resource-backend found in the 
   given \alib{camp;Camp}, as well as its default \alib{camp::Camp;ResourceCategory}.

   \par Availability
     This macro is available only if \ref ALIB_CAMP equals \c true.

   @param T       The type to specialize the type trait \b %ResourcedTraits for.
   @param Camp    Pointer to the \alib{camp;Camp} that provides the
                  \alib{resources;ResourcePool} object.
   @param ResName Expression providing the resource name.
   
\def A_PATH
  Macro used to define \alib{system;Path}-string literals in a platform-independent way.
  Similar to macro \ref A_CHAR, but dependent on the character type defined by the C++ standard
  library with type <c>std::filesystem::path::value_type</c>.<br>
  This is usually single-byte characters, and only on Windows OS, prefix <c>'L'</c> is added.
  @see Compilation symbol \ref ALIB_PATH_CHARACTERS_WIDE.
  @param literal The string literal to be eventually prefix with <c>'L'</c>.
  
\def ALIB_PATH_TO_NARROW
  Similar to \ref ALIB_STRINGS_TO_NARROW. If \alib{system;PathCharType} does not 
  equal \alib{characters::nchar}, a local narrow string is created and the given path is converted.
  Otherwise, just a reference to the given path is created, which will ultimately be optimized
  out by the compiler.
  @param src     The name of the source instance.
  @param dest    The name of the destination instance (or reference to the source).
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer. 
    
\def ALIB_PATH_TO_NARROW_ARG
  Similar to \ref ALIB_STRINGS_TO_NARROW and \ref ALIB_STRINGS_TO_NARROW_ARG. 
  Used to pass \alib path-strings as an argument when a narrow-string is expected.
  @param src     The name of the source instance.
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer. 
  
\def ALIB_PATH_TO_STRING
  Similar to \ref ALIB_STRINGS_TO_NARROW. If \alib{system;PathCharType} does not 
  equal \alib{characters::character}, a local string is created and the given path is converted.
  Otherwise, just a reference to the given path is created, which will ultimately be optimized
  out by the compiler.
  @param src     The name of the source instance.
  @param dest    The name of the destination instance (or reference to the source).
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer. 
    
\def ALIB_PATH_TO_STRING_ARG
  Similar to \ref ALIB_PATH_TO_STRING and \ref ALIB_STRINGS_TO_NARROW_ARG. 
  Used to pass \alib path-strings as an argument when an \alib string is expected.
  @param src     The name of the source instance.
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer. 

\def ALIB_STRING_TO_PATH
  Similar to \ref ALIB_STRINGS_TO_NARROW. If \alib{system;PathCharType} does not 
  equal \alib{characters::character}, a local path-string is created and the given string is 
  converted.
  Otherwise, just a reference to the given string is created, which will ultimately be optimized
  out by the compiler.
  @param src     The name of the source instance.
  @param dest    The name of the destination instance (or reference to the source).
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer. 
    
\def ALIB_STRING_TO_PATH_ARG
  Similar to \ref ALIB_STRINGS_TO_NARROW and \ref ALIB_STRINGS_TO_NARROW_ARG. 
  Used to pass \alib strings as an argument when a path-string is expected.
  @param src     The name of the source instance.
  @param bufSize The (initial) size of the local string, which is used as the conversion buffer. 
@}



@}
*/
