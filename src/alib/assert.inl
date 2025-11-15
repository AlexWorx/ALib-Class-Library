//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
/// With supporting legacy or module builds, .mpp-files are either recognized by the build-system
/// as C++20 Module interface files, or are included by the
/// \ref alib_manual_modules_impludes "import/include headers".
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_DEBUG

/// This namespace exposes entities of module \alib_assert.
ALIB_EXPORT namespace alib::assert {

#if !DOXYGEN
#   if ALIB_SINGLE_THREADED && ALIB_EXT_LIB_THREADS_AVAILABLE
        ALIB_DLL  void SingleThreaded();
#   else
inline    void SingleThreaded() {}  // optimized out
#   endif
#endif


#if ALIB_DEBUG_ASSERTION_PRINTABLES
/// This is the implementation of the templated sibling method, which fetches the variadic
/// templated printables of an \alib_assertion.<br>
///
/// Available only if the compiler-symbol \ref ALIB_DEBUG_ASSERTION_PRINTABLES is given.
/// @param ci       Source location of the assertion raised.
/// @param args     The arguments of the assertion message.
ALIB_DLL void CheckArgsImpl( const CallerInfo& ci, const std::span<std::any>& args );

/// This method is called by the assertion macros with \b any \alib invocation - hence regardless
/// whether the assertion or warning was raised or not -  in the case that the compiler-symbol
/// \ref ALIB_DEBUG_ASSERTION_PRINTABLES is given.<br>
/// This is used to ensure that all arguments passed are of
/// \alib{assert;RegisterPrintable;a known type}.
/// Otherwise, assertions that happen seldom would raise an inner assertion due to unsupported
/// arguments.
/// @tparam TArgs   Types of the variadic arguments \p{args}.
/// @param ci       Source location of the assertion raised.
/// @param args     Variadic argument list comprising the assertion message, which are to be
///                 tested.
template<typename... TArgs>
void CheckArgs( const CallerInfo& ci, TArgs&&... args) {
    constexpr size_t numArgs = sizeof...(TArgs);
    if constexpr (numArgs > 0) {
        std::any argArray[numArgs]= { std::forward<TArgs>(args)... };
        CheckArgsImpl( ci, argArray );
    }
}
#endif

/// Embeds two flags that allow control of whether function \alib{assert::Raise} halts on errors,
/// respectively warnings.
/// The default is to halt on errors, but not on warnings.<br>
/// A thread-local instance is given (and internally tested) with function
/// \alib{assert::GetHaltFlagAndCounters}.
///
/// Note that this mechanism is mostly needed for unit-testing.
/// Here, errors may need to be tested to be properly raised.
struct TLD {

    /// Flag to enable/disable the use of <c>assert(0)</c> when an \alib error was raised.
    bool HaltOnErrors                                                                        = true;

    /// Flag to enable/disable the use of <c>assert(0)</c> when an \alib_warning was raised.
    bool HaltOnWarnings                                                                     = false;

    /// The number of errors counted for this thread.
    size_t CtdErrors                                                                            = 0;

    /// The number of warnings counted for this thread.
    size_t CtdWarnings                                                                          = 0;

    /// The number of messages counted for this thread.
    size_t CtdMessages                                                                          = 0;
};

/// Returns the \c thread_local instance of flags and counters used by function
/// #alib::assert::Raise.
/// @return The singleton, thread-local instance of the flags.
ALIB_DLL TLD& GetHaltFlagAndCounters();


/// A type alias for a function pointer that defines a high-level mechanism for appending a
/// <c>std::any</c> value to a <c>std::string</c>.
/// Custom functions may be registered using function \alib{assert;RegisterPrintable}.
/// This then allows to use custom objects as "printables" in assertions.
using AnyConversionFunc = void(*)(const std::any&, std::string&);

/// A predefined format string used to generate consistent assertion messages.
/// A goal here is to allow a user of \alib to change this format to have assertions, warnings,
/// and messages in the IDE-console be clickable (to jump to the right source location).
/// The default format works well with \https{JetBrains CLion,www.jetbrains.com/clion}.
///
/// Defaults to: <c>"{file}:{line} {type}:\\n{message}"</c>.
///
/// (With this default-value, the placeholder syntax should be self-explanatory.)
ALIB_DLL extern std::string_view FORMAT;

/// The output stream used to with function #alib::assert::Raise, when the parameter \p{type}
/// equals \c 0.<br>
/// Defaults to <c>std::err</c>.
ALIB_DLL extern std::ostream*    STREAM_ERRORS;

/// The output stream used to with function #alib::assert::Raise, when the parameter \p{type}
/// equals \c 1.<br>
/// Defaults to <c>std::cout</c>.
ALIB_DLL extern std::ostream*    STREAM_WARNINGS;

/// The output stream used to with function #alib::assert::Raise, when the parameter \p{type}
/// does not equal \c 0 or \c 1.<br>
/// Defaults to <c>std::cout</c>.
ALIB_DLL extern std::ostream*    STREAM_MESSAGES;

/// Allows registering custom types to be printable with debug-function #alib::assert::Raise.
/// The following snippet from the internal codebase that registers basic types as lambda
/// functions demonstrates how this function can be called:
///
/// \snippet "assert.cpp"        DOX_ASSERT_REGISTER_PRINTABLE
/// @param typeIndex The type to register a conversion method for.
/// @param func      The conversion function.
ALIB_DLL void RegisterPrintable(std::type_index typeIndex, AnyConversionFunc func);


/// The implementation of sibling method #Raise that takes a variadic number of arguments.
/// @param ci      Caller information.
/// @param type    The type of the message: \c 0 for error, \c 1 for warning, higher values for
///                messages.
/// @param domain  The domain of the assertion, warning, or message.
///                (Usually the name of the \alibmod_nl.)
///                Has to be capital alphanumeric letters.
/// @param args    The array of arguments.
ALIB_DLL void raise( const lang::CallerInfo& ci, int type, std::string_view domain,
                     const std::span<std::any>&  args );

/// This method is used by \alib modules to raise assertions, warnings and display debug-messages.
/// (The latter usually depends on special the compiler-symbols like \ref ALIB_DEBUG_MEMORY.)
///
/// It may be used by custom code for custom assertions as well.
///
/// This method first checks if the function pointer \alib{assert::PLUGIN} is set
/// and if yes, passes the parameters to this method and exits.
///
/// Otherwise, the method just writes to the standard error and output streams (which is changeable)
/// and then, if \p{type} equals \c 0, invokes <c>assert(0)</c>.
///
/// @see
///   The following entities are related to \alib_assertions:
///   - \alib{assert::raise} (called by this method after converting the variadic arguments to
///     an array of <c>std::any</c> objects.
///   - Type definition \alib{assert::AnyConversionFunc} to register custom printable types.
///   - Function \alib{assert;RegisterPrintable} to register a conversion function.
///   - Namespace variable \alib{assert::FORMAT} that defines the output format.
///   - Namespace variables \alib{assert::STREAM_ERRORS}, \alib{assert::STREAM_WARNINGS}, and
///     \alib{assert::STREAM_MESSAGES} that allow to redirect assertion messages.
///   - Thread-local struct HALT_FLAGS_AND_COUNTERS, which allows tweaking
///     the assertion behavior and implements usage counters. (Mostly done to support unit-tests.)
///   - Assertion and message macros:
///     - ALIB_ERROR,
///     - ALIB_WARNING,
///     - ALIB_MESSAGE,
///     - ALIB_ASSERT,
///     - ALIB_ASSERT_ERROR,
///     - ALIB_ASSERT_WARNING, and
///     - ALIB_ASSERT_MESSAGE.
///     - ALIB_ERROR,
///     - ALIB_ERROR,
///
/// @tparam TArgs  The types of the variadic function arguments \p{args}.
/// @param ci      Caller information.
/// @param type    The type of the message: \c 0 for error, \c 1 for warning, higher values for
///                messages.
/// @param domain  The domain of the assertion, warning, or message.
///                (Usually the name of the \alibmod_nl.)
///                Has to be capital alphanumeric letters.
/// @param args    Variadic argument list comprising the message.
template<typename... TArgs>
void Raise( const lang::CallerInfo& ci, int type, std::string_view domain, TArgs&&... args) {
    if constexpr (sizeof...(TArgs) == 0)
        raise( ci, type, domain, std::span<std::any>{} );
    else {
        std::array<std::any, sizeof...(TArgs)> argArray  { std::forward<TArgs>(args)... };
        alib::assert::raise( ci, type, domain, argArray );
}   }

/// This function pointer defaults to \c nullptr and may be set by from outside to redirect
/// writing the assertion, warning or message text of functions #Raise, respectively
/// #raise.
///
/// With the use of module \alib_alox this pointer can be set to redirect the output to a
/// \b Lox. Further information on this is found with the chapter
/// \ref alib_mod_alox_debug_and_release_logging_ft_4 of the Programmer's Manual of module
/// \alib_alox_nl.
///
/// - \p{ci}:      Information about the scope of invocation.
/// - \p{type}:    The type of the message. As a convention, 0 is severe error, others are warning
///                levels.
/// - \p{domain}:  Usually the name of the module that raised the assertion.
/// - \p{msg}:     The assembled message to print.
extern void (*PLUGIN)( const lang::CallerInfo&  ci,
                       int                      type,
                       std::string_view         domain,
                       std::string_view         msg     );

} // namespace [alib::assert]
#endif // ALIB_DEBUG
