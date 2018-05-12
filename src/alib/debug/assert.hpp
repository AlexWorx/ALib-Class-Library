// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Preprocessor macros for warnings and assertions given in debug compilations.
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION) || defined(HPP_ALIB_LIB_ASSERT)
    #error "include alib/alib.hpp instead of this header"
#endif

#ifndef HPP_ALIB_LIB_ASSERT
#define HPP_ALIB_LIB_ASSERT 1

/** ********************************************************************************************
 * @name Warning-, Error- and Assertion- Reports for Debug Compilations
 *
 * The macros listed here are defined in two different versions, depending on the \alib distribution.
 * With single <b>%ALib Modules</b> that do not incorporate class
 * \ref aworx::lib::lang::Report and family, namespace function
 * \ref aworx::lib::debug::DbgSimpleALibMsg (and overloads) are used to write the messages.
 * The default implementation of this message invokes \b %assert(false) if the message is of
 * error type.<br>
 * There is a very simple "plug-in" concept in place that allows to redirect this method to
 * a user defined one which may act differently.
 *
 * With the complete \alib library distribution two things happen:
 * - the aforementioned plug-in is implemented and an \alib Report is generated inside
 *   the plug-in function.
 * - The macros themselves are redefined to directly use the \alib Report facility.
 * Such redefinition happens "in the middle" of header inclusion (as early as possible), with
 * the effect that most classes of \alib are compiled using the simple version, but still use
 * the mechanics of \alib Reports. Other parts of \alib classes, and of-course all user code,
 * will have the more sophisticated, report-based versions of the macros in place. The
 * huge advantage of them is, that those accept an arbitrary list of arguments which are boxed
 * and passed to a formatter. This allows to easily compose detailed and formatted debug messages.
 *
 * \note It is advised for users of complete \alib to rather use the
 *       [ALox Logging Library](http://alexworx.github.io/ALox-Logging-Library/index.html) instead
 *       of these macros for debug output. \alib uses these macros internally for the sole reason
 *       to be lean in respect module deployment and to avoid cyclic dependencies with \alox.
 *
 *
 * \def  ALIB_ERROR
 * Writes the given message objects as an error.
 *
 * \def  ALIB_WARNING
 * Writes the given message objects as an warning.
 *
 * \def  ALIB_MESSAGE
 * Writes the given message.
 *
 * \def  ALIB_ASSERT
 * If given condition is \c false, error message "Assertion Failed" is written.
 *
 * \def  ALIB_ASSERT_ERROR
 * If given condition is \c false, given message objects are written as an error.
 *
 * \def  ALIB_ASSERT_WARNING
 * If given condition is \c false, given message objects are written as a warning.
 *
 * @}
 */

#if ALIB_DEBUG

    namespace aworx { namespace lib { namespace debug {

    /**
     * Some \alib modules do not (must not) rely on
     * \ref aworx::lib::lang::Report "Report" /
     * \ref aworx::lib::lang::ReportWriter "ReportWriter" mechanics. Therefore, this simple method is
     * used for error handling in those portions of \alib that are exposed in such modules.<br>
     * This method first checks if static function pointer
     * \ref aworx::lib::debug::DbgSimpleALibMsg_Plugin is set and if yes, passes the parameters
     * to this method and exits.
     * If the complete \alib distribution is used, method
     * \ref aworx::lib::ALib::Init "ALib::Init"
     * sets this plug-in function to a custom one which passes the message(s) to a proper
     * \ref aworx::lib::lang::Report "ALib Report".
     *
     * Otherwise the method just writes to the standard output stream and then,
     * if \p{type} equals \c 0, invokes <c>assert(0)</c>.
     *
     * @param file    The source file of the message invocation.
     * @param line    The line number within \p{file}.
     * @param method  The method invoking this function.
     * @param type    The type of the message. The default implementation does not use this, other
     *                than invoking <c>assert(0)</c> in the case this parameter equals \c 0.
     * @param msg1    The first message string.
     * @param msg2    Optional 2nd message string.
     * @param msg3    Optional 3rd message string.
     * @param msg4    Optional 4th message string.
     *
     */
    ALIB_API
    extern void DbgSimpleALibMsg( const char* file, int line, const char* method,
                                  int type,
                                  const character* msg1,
                                  const character* msg2= nullptr,
                                  const character* msg3= nullptr,
                                  const character* msg4= nullptr );

    /**
     * Overloaded version of
     * \ref DbgSimpleALibMsg(const char* file,int,const char*,int,const character*,const character*,const character*,const character*) "DbgSimpleALibMsg"
     * which accepts one integer value and writes \p{msg} and \p{intValue} in sequence.
     *
     * @param file     The source file of the message invocation.
     * @param line     The line number within \p{file}.
     * @param method   The method invoking this function.
     * @param type     The type of the message. The default implementation does not use this, other
     *                 than invoking <c>assert(0)</c> in the case this parameter equals \c 0.
     * @param msg      The message string.
     * @param intValue An integer parameter (optional due to overload).
     *
     */
    ALIB_API
    extern void DbgSimpleALibMsg( const char* file, int line, const char* method,
                                  int type,
                                  const character* msg, const integer intValue );

    /**
     * This function pointer defaults to nullptr and may be set to replace function
     * #DbgSimpleALibMsg. When \alib complete library is used (instead of one or more
     * single \alib modules), then
     * \ref aworx::lib::ALib::Init         "ALib::Init" sets this pointer to a small method which
     * creates an
     * \ref aworx::lib::lang::Report       "Report" on the default
     * \ref aworx::lib::lang::ReportWriter "ReportWriter".
     *
     * - \p{file}:    Information about the scope of invocation.
     * - \p{line}:    Information about the scope of invocation.
     * - \p{method}:  Information about the scope of invocation.
     * - \p{type}:    The type of the message. As a convention, 0 is severe error, others are  warning levels.
     * - \p{qtyMsgs}: The number of messages passed.
     * - \p{msgs}:    The message strings.
     */
    extern
    void (*DbgSimpleALibMsg_Plugin)(const char* file, int line, const char* method,
                                    int type, int qtyMsgs, const character** msgs);


    }}} // namespace [aworx::lib::debug]

    #define ALIB_ERROR(...)                { aworx::lib::debug::DbgSimpleALibMsg( ALIB_CALLER_PRUNED, 0, __VA_ARGS__); }
    #define ALIB_WARNING(...)              { aworx::lib::debug::DbgSimpleALibMsg( ALIB_CALLER_PRUNED, 1, __VA_ARGS__); }
    #define ALIB_MESSAGE(...)              { aworx::lib::debug::DbgSimpleALibMsg( ALIB_CALLER_PRUNED, 2, __VA_ARGS__); }
    #define ALIB_ASSERT(cond)              { if(!(cond)) ALIB_ERROR( ASTR("Assertion Failed") ); }
    #define ALIB_ASSERT_ERROR(cond, ...)   { if(!(cond)) ALIB_ERROR( __VA_ARGS__ );      }
    #define ALIB_ASSERT_WARNING(cond, ...) { if(!(cond)) ALIB_WARNING( __VA_ARGS__ );    }

#else
    #define ALIB_ERROR(...)                {}
    #define ALIB_WARNING(...)              {}
    #define ALIB_MESSAGE(...)              {}
    #define ALIB_ASSERT(cond)              {}
    #define ALIB_ASSERT_ERROR(cond, ...)   {}
    #define ALIB_ASSERT_WARNING(cond, ...) {}
#endif

/**
 * \def  ALIB_ASSERT_RESULT_EQUALS
 * Asserts that a return value of a function call equals the given expected value.
 * In release compilation, the function is still invoked, but no check is performed.
 *
 *
 * \def  ALIB_ASSERT_RESULT_NOT_EQUALS
 * Asserts that a return value of a function call is not equal to the given value.
 * In release compilation, the function is still invoked, but no check is performed.
 *
 *
 * \def  ALIB_ASSERT_RESULT_GREATER_THAN
 * Asserts that a return value of a function call is greater than the given value.
 * In release compilation, the function is still invoked, but no check is performed.
 *
 *
 * \def  ALIB_ASSERT_RESULT_LESS_THAN
 * Asserts that a return value of a function call is less than the given expected value.
 * In release compilation, the function is still invoked, but no check is performed.
 */
#if !defined( DOX_PARSER )
    #if ALIB_DEBUG
        #define ALIB_ASSERT_RESULT_EQUALS(      func, value ) { auto result= func; assert(result == value); ((void) result); }
        #define ALIB_ASSERT_RESULT_NOT_EQUALS(  func, value ) { auto result= func; assert(result != value); ((void) result); }
        #define ALIB_ASSERT_RESULT_GREATER_THAN(func, value ) { auto result= func; assert(result >  value); ((void) result); }
        #define ALIB_ASSERT_RESULT_LESS_THAN(   func, value ) { auto result= func; assert(result <  value); ((void) result); }
    #else
        #define ALIB_ASSERT_RESULT_EQUALS(      func, value ) { func; }
        #define ALIB_ASSERT_RESULT_NOT_EQUALS(  func, value ) { func; }
        #define ALIB_ASSERT_RESULT_GREATER_THAN(func, value ) { func; }
        #define ALIB_ASSERT_RESULT_LESS_THAN(   func, value ) { func; }
    #endif

#else // doxygen version
    #define ALIB_ASSERT_RESULT_EQUALS(      func, value ) {}
    #define ALIB_ASSERT_RESULT_NOT_EQUALS(  func, value ) {}
    #define ALIB_ASSERT_RESULT_GREATER_THAN(func, value ) {}
    #define ALIB_ASSERT_RESULT_LESS_THAN(   func, value ) {}
#endif



/** ************************************************************************************************
 * \name Macros for One time warnings
 *
 * This group of macros enable the implementation of "one-time warnings", which
 * are warnings that are issued only once per debug session.

 * The warnings are executed using
 * \ref aworx::lib::lang::Report::DoReport   "Report::DoReport" (on the \b Report instance received by
 * \ref aworx::lib::lang::Report::GetDefault "Report::GetDefault")
 * and are effective only in debug compilations. They are pruned in release code
 * (dependent on symbol #ALIB_DEBUG).
 *
 * <b> Declaration of an one-time warning </b><p>
 * If an one-time warning is declared per type using \ref ALIB_WARN_ONCE_PER_TYPE_DECL only one
 * warning for all objects will occur and the warnings can be enabled or disabled only for all
 * instances of a class together.
 *
 * In contrast to this, declaring an one-time warning per instance using
 * \ref ALIB_WARN_ONCE_PER_INSTANCE_DECL, a warning will occur once for each object.
 *
 * Both macros have to be placed in the public member section of a class. In addition to that,
 * per type declarations need to be completed with a definition using macro
 * \ref ALIB_WARN_ONCE_PER_TYPE_DEFINE in the implementation file of the type.
 *
 * <b> Raise Warnings </b><p>
 * Within the implementation of a class that has OTW-declarations, the macros
 * #ALIB_WARN_ONCE, #ALIB_WARN_ONCE_IF and #ALIB_WARN_ONCE_IF_NOT is used to issue the warning.
 *
 * <b> Controlling the warnings</b>
 * The using code of a class that has OTW-declarations can enable and disable (or re-enable)
 * one-time warnings using macros
 * - \ref ALIB_WARN_ONCE_PER_TYPE_ENABLE,
 * - \ref ALIB_WARN_ONCE_PER_TYPE_DISABLE,
 * - \ref ALIB_WARN_ONCE_PER_INSTANCE_ENABLE and
 * - \ref ALIB_WARN_ONCE_PER_INSTANCE_DISABLE.
 *
 * For example, if a certain warning or condition seems right for a specific situation,
 * an one-time warning can be disabled and re-enabled after that situation. Or if it is a per
 * instance warning, it can be disabled after creating that instance.
 *
 * <b> Parameters of the macros</b>
 * The following parameters are expected by the macros:
 *
 * - <em>libdecl:</em><br>
 *   Must match a macro that defines the import/export declaration of the source code entity
 *   (see \ref ALIB_API for a sample of how this symbol is declared in \alib itself).
 * - <em>identifier</em><br>
 *   Identifies the one-time warning. Has to apply to the rules of C++ variable names and can
 *   other wise be chosen freely.
 * - <em>className</em><br>
 *   Names the class when defining or setting a per class one-time warnings.
 * - <em>instance</em><br>
 *   Names a reference to the instance when using per instance one-time warnings.
 *   Within a method of the corresponding type, use \c *this.
 * - <em>default</em><br>
 *   The default value when declaring, respectively implementing the variable. Can be \c true for
 *   enabled by default, or \c false for disabled.
 **************************************************************************************************/

/**
 *
 * @{
 * \def  ALIB_WARN_ONCE_PER_TYPE_DECL
 * Declares a static boolean field in a class (struct) to store information about whether a one-time
 * warning was already issued or not.
 * Needs to be placed in the public section of a type declaration.
 * \note This macro needs a corresponding \ref ALIB_WARN_ONCE_PER_TYPE_DEFINE in the implementation
 *       source of that type.
 *
 * \def  ALIB_WARN_ONCE_PER_TYPE_DEFINE
 * Defines a boolean field in a class (struct) to store information about whether a one-time warning
 * was already issued for an object or not. Declaration of the field is done using
 * \ref ALIB_WARN_ONCE_PER_TYPE_DECL.
 *
 * @{ \def  ALIB_WARN_ONCE_PER_TYPE_ENABLE
 * Enables a type-wide one-time warning.
 *
 * @{ \def  ALIB_WARN_ONCE_PER_TYPE_DISABLE
 * Disables a type-wide one-time warning.
 *
 * \def  ALIB_WARN_ONCE_PER_INSTANCE_DECL
 * Defines a boolean field in a class (struct) to store information about whether a one-time warning
 * was already issued for an object or not.
 * This macro needs to be placed in the public section of a class (struct) declaration.
 *
 * \def  ALIB_WARN_ONCE_PER_INSTANCE_ENABLE
 * Enables a per-instance one-time warning.
 *
 * \def  ALIB_WARN_ONCE_PER_INSTANCE_DISABLE
 * Disables a per-instance one-time warning.
 *
 * \def  ALIB_WARN_ONCE
 * Used to 'raise' an one-time warning using the named variable which has to be declared using either
 * \ref ALIB_WARN_ONCE_PER_TYPE_DECL or
 * \ref ALIB_WARN_ONCE_PER_INSTANCE_DECL.
 *
 * \def  ALIB_WARN_ONCE_IF
 * Used to 'raise' an one-time warning, dependent on the given condition and of the state of the
 * named variable which has to be declared using either
 * \ref ALIB_WARN_ONCE_PER_TYPE_DECL or
 * \ref ALIB_WARN_ONCE_PER_INSTANCE_DECL.
 *
 * \def  ALIB_WARN_ONCE_IF_NOT
 * Used to 'raise' an one-time warning, dependent on the given condition and of the state of the
 * named variable which has to be declared using either
 * \ref ALIB_WARN_ONCE_PER_TYPE_DECL or
 * \ref ALIB_WARN_ONCE_PER_INSTANCE_DECL.
 */

#if ALIB_DEBUG && !defined( DOX_PARSER )

    #define ALIB_WARN_ONCE_PER_INSTANCE_DECL(            identifier, default )  public: bool ALIB_OTW_##identifier= default;
    #define ALIB_WARN_ONCE_PER_INSTANCE_ENABLE(   instance, identifier       )  instance. ALIB_OTW_##identifier= true;
    #define ALIB_WARN_ONCE_PER_INSTANCE_DISABLE(   instance, identifier      )  instance. ALIB_OTW_##identifier= false;

    #define ALIB_WARN_ONCE_PER_TYPE_DECL(     libdecl,   identifier          )  public: libdecl static bool ALIB_OTW_##identifier;
    #define ALIB_WARN_ONCE_PER_TYPE_DEFINE(   className, identifier, default )  bool className::ALIB_OTW_##identifier= default;
    #define ALIB_WARN_ONCE_PER_TYPE_ENABLE(   className, identifier          )  className::ALIB_OTW_##identifier= true;
    #define ALIB_WARN_ONCE_PER_TYPE_DISABLE(  className, identifier          )  className::ALIB_OTW_##identifier= false;

    #define ALIB_WARN_ONCE(instance, identifier, ...)                                            \
    {                                                                                            \
        if( (instance).ALIB_OTW_##identifier )                                                   \
        {                                                                                        \
            ALIB_OTW_##identifier= false;                                                        \
            ALIB_WARNING( __VA_ARGS__ );                                                         \
        }                                                                                        \
    }

    #define ALIB_WARN_ONCE_IF(instance, identifier, cond, ...)                                   \
    {                                                                                            \
        if( (instance).ALIB_OTW_##identifier && (cond) )                                         \
        {                                                                                        \
            ALIB_OTW_##identifier= false;                                                        \
            ALIB_WARNING( __VA_ARGS__ );                                                         \
        }                                                                                        \
    }

    #define ALIB_WARN_ONCE_IF_NOT(instance, identifier, cond, ...)                               \
    {                                                                                            \
        if( (instance).ALIB_OTW_##identifier && !(cond) )                                        \
        {                                                                                        \
            ALIB_OTW_##identifier= false;                                                        \
            ALIB_WARNING( __VA_ARGS__ );                                                         \
        }                                                                                        \
    }
#else
    #define ALIB_WARN_ONCE_PER_INSTANCE_DECL(              identifier, default   )
    #define ALIB_WARN_ONCE_PER_INSTANCE_ENABLE(  instance, identifier            ) {}
    #define ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( instance, identifier            ) {}

    #define ALIB_WARN_ONCE_PER_TYPE_DECL(        libdecl,  identifier            )
    #define ALIB_WARN_ONCE_PER_TYPE_DEFINE(     className, identifier, default   )
    #define ALIB_WARN_ONCE_PER_TYPE_ENABLE(     className, identifier            ) {}
    #define ALIB_WARN_ONCE_PER_TYPE_DISABLE(    className, identifier            ) {}

    #define ALIB_WARN_ONCE(                      instance, identifier,       ... ) {}
    #define ALIB_WARN_ONCE_IF(                   instance, identifier, cond, ... ) {}
    #define ALIB_WARN_ONCE_IF_NOT(               instance, identifier, cond, ... ) {}
#endif

/// @}



#endif // HPP_ALIB_LIB_ASSERT
