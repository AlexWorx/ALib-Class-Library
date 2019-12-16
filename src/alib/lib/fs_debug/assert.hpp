/** ************************************************************************************************
 * \file
 * This header file  is part of file set \alibfs_debug of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FS_DEBUG_ASSERT
#define HPP_ALIB_FS_DEBUG_ASSERT 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
#endif

ALIB_ASSERT_FILESET(DEBUG)

#if !defined(HPP_ALIB_TOOLS) && !defined(ALIB_DOX)
#   include "alib/lib/tools.hpp"
#endif

#if !defined(HPP_ALIB_FS_INTEGERS_INTEGERS)
#   include "alib/lib/fs_integers/integers.hpp"
#endif

#if !defined(HPP_ALIB_FS_DEBUG_ASSERT)
#   include "alib/lib/fs_debug/assert.hpp"
#endif


#if !defined (_ASSERT_H) && !defined(assert)
#   include <assert.h>
#endif

#if ALIB_DEBUG

namespace aworx { namespace lib {

    /**
     * Some \alib modules do not (must not) rely on
     * \ref aworx::lib::results::Report "Report" /
     * \alib{results,ReportWriter} mechanics. Therefore, this simple method is
     * used for error handling in those portions of \alib that are exposed in such modules.<br>
     * This method first checks if static function pointer
     * \alib{DBG_SIMPLE_ALIB_MSG_PLUGIN} is set and if yes, passes the parameters
     * to this method and exits.
     * If the complete \alib distribution is used, method
     * \alib{ALibDistribution::Bootstrap}
     * sets this plug-in function to a custom one which passes the message(s) to a proper
     * \ref aworx::lib::results::Report "ALib Report".
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
     * @param msg5    Optional 5th message string.
     *
     */
    ALIB_API
    extern void DbgSimpleALibMsg( const char* file,  int line,  const char* method,
                                  int          type,
                                  const char* msg1,
                                  const char* msg2= nullptr,
                                  const char* msg3= nullptr,
                                  const char* msg4= nullptr,
                                  const char* msg5= nullptr );

    /**
     * Overloaded version of
     * \ref DbgSimpleALibMsg(const char*,int,const char*,int,const char*,const char*,const char*,const char*,const char*) "DbgSimpleALibMsg"
     * which accepts one integral value and writes \p{msg} and \p{intValue} in sequence.
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
    extern void DbgSimpleALibMsg( const char* file,  int line,  const char* method,
                                  int           type,
                                  const char*   msg,
                                  const integer intValue );

    /**
     * This function pointer defaults to \c nullptr and may be set to replace function
     * #DbgSimpleALibMsg.
     * With the use of \alib_results, the module's bootstrap code (precisely method
     * \alib{results,Results::bootstrap}) sets this pointer to a small method which creates an
     * \alib{results,Report} on the default \alib{results,ReportWriter}.
     *
     * - \p{file}:    Information about the scope of invocation.
     * - \p{line}:    Information about the scope of invocation.
     * - \p{method}:  Information about the scope of invocation.
     * - \p{type}:    The type of the message. As a convention, 0 is severe error, others are  warning levels.
     * - \p{qtyMsgs}: The number of messages passed.
     * - \p{msgs}:    The message strings.
     */
    extern void (*DBG_SIMPLE_ALIB_MSG_PLUGIN)( const char* file,  int line,  const char* method,
                                               int type, int qtyMsgs, const char** msgs         );


    }} // namespace [aworx::lib]

    #define ALIB_ERROR(...)                { aworx::lib::DbgSimpleALibMsg( ALIB_CALLER_PRUNED, 0, __VA_ARGS__); }
    #define ALIB_WARNING(...)              { aworx::lib::DbgSimpleALibMsg( ALIB_CALLER_PRUNED, 1, __VA_ARGS__); }
    #define ALIB_MESSAGE(...)              { aworx::lib::DbgSimpleALibMsg( ALIB_CALLER_PRUNED, 2, __VA_ARGS__); }
    #define ALIB_ASSERT(cond)              { if(!(cond)) ALIB_ERROR( "Assertion Failed" ); }
    #define ALIB_ASSERT_ERROR(cond, ...)   { if(!(cond)) ALIB_ERROR( __VA_ARGS__ );        }
    #define ALIB_ASSERT_WARNING(cond, ...) { if(!(cond)) ALIB_WARNING( __VA_ARGS__ );      }
    #define ALIB_ASSERT_MESSAGE(cond, ...) { if(!(cond)) ALIB_MESSAGE( __VA_ARGS__ );      }

#else
    #define ALIB_ERROR(...)                 ;
    #define ALIB_WARNING(...)               ;
    #define ALIB_MESSAGE(...)               ;
    #define ALIB_ASSERT(cond)               ;
    #define ALIB_ASSERT_ERROR(cond, ...)    ;
    #define ALIB_ASSERT_WARNING(cond, ...)  ;
    #define ALIB_ASSERT_MESSAGE(cond, ...)  ;
#endif

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

#endif // HPP_ALIB_FS_DEBUG_ASSERT
