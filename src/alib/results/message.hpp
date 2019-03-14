// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_RESULTS_MESSAGE
#define HPP_ALIB_RESULTS_MESSAGE 1

#if !defined (HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

ALIB_ASSERT_MODULE(RESULTS)
ALIB_ASSERT_MODULE(BOXING)

#if !defined(HPP_ALIB_BOXING_ENUM)
#   include "alib/boxing/enum.hpp"
#endif

#if !defined(_GLIBCXX_STACK) && !defined(_STACK_)
    #include <stack>
#endif


namespace aworx { namespace lib { namespace results {

/** ************************************************************************************************
 * This struct implements a message object.
 * It is used with types \alib{results,Exception} and \alib{results,Report}.
 *
 * Besides information on source code (place of construction) given with fields
 * #File, #Line and #Func, a message has an identifier with field #Type and a set of arguments
 * defined with field #Args which is a list of \alib{boxing,Box,boxes}.
 **************************************************************************************************/
struct Message
{
    /** The file name that reported. */
    NCString      File;

    /** The line number in the source file that reported. */
    int           Line;

    /** The function/method name that reported. */
    NCString      Func;

    /**
     * The message type. \c 0 indicates \e 'severe' errors, \c 1 warnings.
     * Others are status messages and may be defined (interpreted) by custom
     * implementations of
     * \alib{results,ReportWriter}.
     */
    Enum          Type;

    /** Arbitrary message arguments. */
    Boxes         Args;

    /** ********************************************************************************************
     * Constructs a message.
     * @param file   Information about the scope of invocation.
     * @param line   Information about the scope of invocation.
     * @param func   Information about the scope of invocation.
     * @param type   The message type.
     * @param args   Variadic, templated list of arguments.
     **********************************************************************************************/
    template <typename... TBoxables>
    Message( const NCString& file, int line, const NCString& func,
             const Enum& type, TBoxables&&... args )
    : File(file),  Line(line), Func(func), Type(type)
    {
        Args.Add( std::forward<TBoxables>(args)... );
    }

    /** ********************************************************************************************
     * Loops over all boxes in field #Args and invokes box-function \alib{boxing,FClone}.
     *
     * It is the responsibility of the user or creator of a message object to invoke this method
     * and also to provide the given \p{memoryBlocks} object.
     *
     * \see
     *   Manual chapter \ref alib_results_message_lifecycle for details.
     *
     * @param memoryBlocks The block allocator passed to interface method \b FClone.
     **********************************************************************************************/
    ALIB_API
    void CloneArguments( memory::MemoryBlocks& memoryBlocks );
};

}}} // namespace [aworx::lib::results]

#endif // HPP_ALIB_RESULTS_MESSAGE
