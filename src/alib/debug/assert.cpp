// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if ALIB_DEBUG
    #include <iostream>
    #include <cassert>

    #if defined( _WIN32 )
        #include <string>
    #endif
#endif


// #################################################################################################
// Debug functions
// #################################################################################################
#if ALIB_DEBUG

namespace aworx { namespace lib { namespace debug {


void (*DbgSimpleALibMsg_Plugin)(const char* file, int line, const char* method, int type, int qtyMsgs, const character** msgs)     =nullptr;


void DbgSimpleALibMsg( const char* file, int line, const char* method, int type,
                       const character* msg1, const character* msg2,
                       const character* msg3, const character* msg4                )
{

    int         qtyMsgs=  msg2 != nullptr ? (msg3 != nullptr ? (msg4 != nullptr ? 4 : 3) : 2) : 1;
    const character* msgs[4]= { msg1, msg2, msg3, msg4 };

    if( DbgSimpleALibMsg_Plugin )
    {
        DbgSimpleALibMsg_Plugin( file, line, method, type, qtyMsgs, msgs  );
        return;
    }

    // internal implementation
    #if ALIB_NARROW_STRINGS
        for (int i= 0; i< qtyMsgs; i++ )
            std::cout << msgs[i];
        std::cout << std::endl;
    #else
        for (int i= 0; i< qtyMsgs; i++ )
            std::wcout << msgs[i];
        std::wcout << std::endl;
    #endif
    assert(type);
}

void DbgSimpleALibMsg( const char* file, int line, const char* method,
                       int type, const character* msg, integer intVal )
{

    std::basic_string<character> sIntVal=
                                        #if ALIB_NARROW_STRINGS
                                            std::to_string( intVal );
                                        #else
                                            std::to_wstring( intVal );
                                        #endif

    if( DbgSimpleALibMsg_Plugin )
    {
        const character* msgs[2]= { msg, sIntVal.c_str() };
        DbgSimpleALibMsg_Plugin( file, line, method, type, 2, msgs  );
        return;
    }

    // internal implementation
    #if ALIB_NARROW_STRINGS
        std::cout << msg << sIntVal << std::endl;
    #else
        std::wcout << msg << sIntVal << std::endl;
    #endif
    assert(type);
}

}}}// namespace [aworx::lib::debug]

#endif

// #################################################################################################
// Set 'flags' for pretty printers by defining global symbols.
// See file $ALOX_LIB_PATH/ALox/tools/ideplugins/gdb/alibpp.py for more information
// #################################################################################################
#if ALIB_DEBUG
    #if defined(DOX_PARSER)
        #define ALIB_GDB_PP_SUPPRESS_CHILDREN
        #define ALIB_GDB_PP_FIND_POINTER_TYPES
    #endif

    /**
     * @addtogroup GrpALibCompilerSymbols
     * @{ \def  ALIB_GDB_PP_SUPPRESS_CHILDREN
     *  This symbol may be passed to the compiler to control the behavior of pretty printers scripts
     *  for GDB. If defined, external symbol \e "ALIB_PP_SUPPRESS_CHILDREN"
     *  is created in debug compilations. This will be detected by pretty printer python script
     *  provided with \alib/\alox.
     *
     *  \see Documentation of pretty printers in tools section of \alib documentation.
     * @}
     */

    #if  defined(ALIB_GDB_PP_SUPPRESS_CHILDREN)
        extern int ALIB_PRETTY_PRINTERS_SUPPRESS_CHILDREN;
               int ALIB_PRETTY_PRINTERS_SUPPRESS_CHILDREN;
    #endif

    /**
     * @addtogroup GrpALibCompilerSymbols
     * @{ \def  ALIB_GDB_PP_FIND_POINTER_TYPES
     *  This symbol may be passed to the compiler to control the behavior of pretty printers scripts
     *  for GDB. If defined, external symbol \e "ALIB_PP_FIND_POINTER_TYPES"
     *  is created in debug compilations. This will be detected by pretty printer python script
     *  provided with \alib/\alox.
     *
     *  \see Documentation of pretty printers in tools section of \alib documentation.
     * @}
     */

    #if  defined(ALIB_GDB_PP_FIND_POINTER_TYPES)
        extern int ALIB_PRETTY_PRINTERS_FIND_POINTER_TYPES;
               int ALIB_PRETTY_PRINTERS_FIND_POINTER_TYPES;
    #endif


    //
    // define further global symbols which are detected by the pretty printer python script for gdb
    //
    #if  !ALIB_NARROW_STRINGS
        extern int ALIB_PRETTY_PRINTERS_DEFAULT_CHAR_IS_WIDE;
               int ALIB_PRETTY_PRINTERS_DEFAULT_CHAR_IS_WIDE;
    #endif

    #if ALIB_SIZEOF_WCHAR_T == 4
        extern int ALIB_PRETTY_PRINTERS_WCHAR_SIZE_IS_4;
               int ALIB_PRETTY_PRINTERS_WCHAR_SIZE_IS_4;

    #endif

#endif


// #################################################################################################
// static assertions for the platform
// #################################################################################################

static_assert(         sizeof(aworx::integer )       ==         sizeof(aworx::uinteger  ),       "Error in ALib type definitions" );
static_assert(         sizeof(aworx::integer )       ==         sizeof(std::size_t    ),         "Error in ALib type definitions" );
static_assert(std::is_signed< aworx::integer>::value == std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed< aworx::integer>::value != std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<aworx::uinteger>::value == std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<aworx::uinteger>::value != std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );


// #################################################################################################
// Namespace documentation aworx and aworx::lib
// #################################################################################################

/**
 * Namespace
 *  - \c aworx
 *
 *  is the outer namespace for C++ software published by A-Worx GmbH, Germany. <p>
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
 *  \anchor CPP_AWORX_NS_SHORTCUTS
 * <b>Type Shortcuts of C++ Namespace aworx</b><p>
 * \alib and other software of A-Worx GmbH, \e \"mirror\" important types into <b>namespace %aworx</b>
 * with \c using statements. For example, by having
 *
 *      using  String =  aworx::lib::strings::String;
 *
 * stated within namespace \c %aworx but in the header file of class String (!), this class is
 * made available with just adding
 *
 *      using aworx;
 *
 * to a source file. This is convenient when \alib is \e used in 3rd party code outside of the
 * \c %aworx namespace, and also for the classes of \alib itself and for 'sibling'
 * libraries that reside next to each other underneath namespace \c %aworx.<br>
 * For example logging library \alox with namespace \c %aworx::lib::lox. Such classes may refer to all
 * mirrored sibling-classes already within the header files without providing any namespace,
 * because in C++, code that is located inside a namespace 'sees' all types defined in parent
 * namespaces.<br>
 * Therefore, header files become clear and simple without using full qualified type names.
 *
 * In contrast to this, C++ sources that are \"\e using\" a sub-namespace do \b not automatically
 * 'see' classes in parent namespaces of such used namespace.
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
 *   using namespace prefix <b>aworx::</b>.
 * - Inside external cpp-files, if no collision with other libraries or local classes exist,
 *   all types of all A-Worx libraries used, can be made visible with one single using statement for
 *   namespace \b aworx. Should collisions occur, then still selected classes of A-Worx libraries
 *   that do not collide can be made visible by choosing the complete namespace name for the
 *   \e using statement.
 * - The mirrored class names appear only in this <b>namespace aworx</b>, if the corresponding
 *   header file of the original class is included in a compilation unit.
 *
 * <b>Domain-Specific Constants and Variables</b><p>
 * Similar to the concept of mirroring important interface classes into this <b>namespace aworx</b>
 * (which is described in the previous paragraph), it is also agreed to place important global
 * constants here. The impact and benefits are quite the same.<br>
 * As a sample, see
 * \ref aworx::DirectorySeparator which appears in this namespace as soon as header
 * "alib/system/directory.hpp" is included.
 *
 */
namespace aworx {


/**
 *
 * This is the C++ base namespace for \alib - a set of core utility classes developed by
 * A-Worx GmbH, Germany, published under <em>Boost Software License</em>.
 *
 * For programmer's manual(s), tutorials and other documentation, please visit the
 * [ALib for C++ Homepage](http://alexworx.github.io/ALib-Class-Library/).
 *
 *
 */
namespace lib { }}



