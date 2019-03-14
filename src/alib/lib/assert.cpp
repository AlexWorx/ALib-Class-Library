// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/lib/assert.hpp"


#if ALIB_DEBUG
#   include <iostream>

#   include <cassert>

    #if defined( _WIN32 ) && !defined(_STRING_)
    #   include <string>
    #endif
#endif


// #################################################################################################
// Debug functions
// #################################################################################################
#if ALIB_DEBUG

namespace aworx { namespace lib {


void (*DBG_SIMPLE_ALIB_MSG_PLUGIN)( const char* file, int line, const char* method,
                                    int type, int qtyMsgs, const char** msgs        )      =nullptr;


//! @cond NO_DOX
void DbgSimpleALibMsg( const char* file, int line, const char* method, int type,
                       const char* msg1, const char* msg2, const char* msg3,
                       const char* msg4, const char* msg5                         )
{

    int   qtyMsgs=  msg2 != nullptr ? (msg3 != nullptr ? (msg4 != nullptr ? (msg5 != nullptr ? 5 : 4) : 3) : 2) : 1;
    const char* msgs[5]= { msg1, msg2, msg3, msg4, msg5 };

    if( DBG_SIMPLE_ALIB_MSG_PLUGIN )
    {
        DBG_SIMPLE_ALIB_MSG_PLUGIN( file, line, method, type, qtyMsgs, msgs );
        return;
    }

    // internal implementation
    for (int i= 0; i != qtyMsgs; i++ )
        std::cout << msgs[i];
    std::cout << std::endl;
    assert(type);
}

void DbgSimpleALibMsg( const char* file, int line, const char* method, int type,
                       const char* msg, integer intVal )
{
    std::basic_string<char> sIntVal=       std::to_string( intVal );
    if( DBG_SIMPLE_ALIB_MSG_PLUGIN )
    {
        const char* msgs[2]= { msg, sIntVal.c_str() };
        DBG_SIMPLE_ALIB_MSG_PLUGIN( file, line, method, type, 2, msgs );
        return;
    }

    // internal implementation
    std::cout << msg << sIntVal << std::endl;
    assert(type);
}

//! @endcond

}}// namespace [aworx::lib]



// #################################################################################################
// static assertions for the platform
// #################################################################################################

static_assert(         sizeof(aworx::integer )       ==         sizeof(aworx::uinteger  ),       "Error in ALib type definitions" );
static_assert(         sizeof(aworx::integer )       ==         sizeof(std::size_t    ),         "Error in ALib type definitions" );
static_assert(std::is_signed< aworx::integer>::value == std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed< aworx::integer>::value != std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<aworx::uinteger>::value == std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<aworx::uinteger>::value != std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );

#endif

// #################################################################################################
// Namespace documentation aworx and aworx::lib
// #################################################################################################

/**
 * # 1. Introduction #
 * This namespace documented here, namely
 *  - \c aworx
 *
 * is the outer namespace for C++ software published by A-Worx GmbH, Germany. <p>
 * While the Java language strongly proposes to use the complete 'reverse domain' name
 * and therefore Java code published by A-Worx GmbH resides in namespace
 * - \c com.aworx,
 *
 * for C# the prefix \b 'com.' is changed to \b 'cs.'. This way, the C# version of this namespace is
 * - \c cs.aworx
 *
 * This distinction is needed to avoid ambiguities within the documentation system
 * ([Doxygen](http://www.stack.nl/~dimitri/doxygen)) which is used in scenarios of mixed
 * documentation of similar code written in different languages.
 *
 * Besides nested namespaces, there are no functions or types defined in this namespace.
 * The C++ language, has one important exception to this rule, as described in the next paragraph.
 *
 * \anchor CPP_AWORX_NS_SHORTCUTS
 * # 2. Type Aliases In Namespace aworx #
 * \aliblink and other software of A-Worx GmbH, \e \"mirror\" important types into <b>namespace %aworx</b>
 * with \c using statements. For example, by having
 *
 *      using  Compiler=   aworx::lib::expressions::Compiler;
 *
 * stated within namespace \c %aworx (but in the header file of class
 * \ref aworx::lib::expressions::Compiler!), this class is made available with just adding
 *
 *      using namespace aworx;
 *
 * to a source file. This is convenient when \alib is \e used in 3rd party code outside of the
 * \c %aworx namespace, but also for the classes of \alib itself, which are organized in
 * "modules", which are located in 'sibling-namespaces' beyond \c %aworx::lib.
 * All classes of these modules may refer to all classes in other modules just due to the "mirroring"
 * of the type in this joint parent namespace \c aworx. This way, only seldom an explicit namespace
 * has to be given within the \alib sources.
 *
 * So, while code that is located inside a sub-namespace 'sees' all types defined in parent
 * namespaces, with code that is located elsewhere and is just  \e "using" a sub-namespace of
 * \alib, this is different. Types in parent namespaces here are \b not automatically 'seen'
 * when a sub-namespace is \e "used".
 *
 * This is an important difference and a great design decision of the C++ language!
 * As a conclusion, a user of libraries (types) residing
 * in sub-namespaces of \b %aworx, has different choices on selecting namespaces with \c using statement:
 * - <c>using namespace %aworx;           // see all types as 'mirror' types </c>
 * - <c>using namespace %aworx::abc::xyz; // see only types of distinct namespace </c>
 * - or: do not 'use' any namespace but select A-Worx types by full qualified name.
 *
 * To summarize, the effect of mirroring the main interface classes is as follows:
 * - Inside header files of sub-namespaces (and their sub-namespaces) of \e aworx, e.g. \e aworx::lib and \e aworx::lib::lox,
 *   these names are visible and used.
 * - Inside external header files, they are not visible but might be quite shortly accessed
 *   using namespace prefix <b>"aworx::"</b> (addressing the mirrored type alias).
 * - Inside external cpp-files, if no collision with other libraries or local classes exist,
 *   all types of all A-Worx libraries included by header files, can be made visible with one single
 *   using statement for namespace \b aworx. Should collisions occur, then still selected classes of A-Worx libraries
 *   that do not collide can be made visible by choosing the complete namespace name for the
 *   \e using statement.
 * - Of-course, the mirrored class names appear only in this <b>namespace aworx</b>, if the
 *   corresponding header file of the original class is included in a compilation unit.
 *
 * # 3. Domain-Specific Constants and Variables #
 * Similar to the concept of mirroring important interface classes into this <b>namespace aworx</b>
 * (which is described in the previous paragraph), it is also agreed to place important global
 * constants here. The impact and benefits are quite the same.<br>
 * As a sample, see
 * \aworx{DirectorySeparator} or  \aworx{NewLine}, which appear in this namespace as soon as header
 * <c>"alib/system/directory.hpp"</c>, respectively <c>"alib/strings/cstring.hpp"</c> is included.
 *
 * # 4. This Documentation #
 * You are reading the documentation of this namespace generated with the documentation of
 * \aliblink. For this reason, all "mirrored" types of \alib will be presented below.
 * Note that for the reasons explained above, with other library/software documentation, other
 * types will appear in the documentation of this namespace.
 *
 * # Reference Documentation #
 */
namespace aworx {


/**
 * This is the root namespace of \alib, a general purpose C++ library. \alib is developed by
 * A-Worx GmbH, Germany, published under <em>Boost Software License</em>.
 *
 * While being the root namespace of \aliblink, the vast majority of library types are found in
 * sub-namespaces hereof, which are associated with so called \alibmods.
 * The only sub-namespace that is not related to a single module is
 *
 * - #aworx::lib::compatibility
 *
 * and this is not even a real C++ namespace! In fact, it is used by various modules to place
 * "documentation" of entities that provide compatibility to types of the standard C++ class
 * library and 3rd-party libraries.
 *
 * Next, in this namespace are classes \alib{Module} and \alib{ALibModules} found, which are
 * used to organize the library in respect to resource management and bootstrapping.
 *
 * Besides this, only a very few types and simple type definitions are placed in
 * \b aworx::lib. These are types that support more than one module, were too simple to comprise
 * a module or are for other reasons not eligible to be located in an \alibmod.<br>
 * Among these types are a set of "common enums" which are used by different \alibmods_nolink,
 * and which might be used by users of the library as well. These enums become available with
 * the inclusion of header file \ref alib/lib/commonenums.hpp. If module \alibmod_resources is
 * included in the \alibdist_nolink, and furthermore a code unit wants to read and write
 * element values of the common enums, then furthermore header \ref alib/lib/commonenumsresourced.hpp
 * is to be included.
 *
 *
 * # Documentation #
 * The \ref alib_manual provides extensive documentation, including various separated manuals of
 * \alibmods, tutorials with sample code, information about configuring and compiling the
 * library and so forth.<br>
 * For people new to \alib, the \ref alib_manual is the right place to start!
 *
 *
 * # Reference Documentation #
 */
namespace lib { }}



