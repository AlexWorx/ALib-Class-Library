// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_LANG_OWNER)
    #error "Header already included"
#endif

// Due to our blocker above, this include will never be executed. But having it, allows IDEs
// (e.g. QTCreator) to read the symbols when opening this file
#if !defined (HPP_ALIB)
    #include "alib/alib.hpp"
#endif

// then, set include guard
#ifndef HPP_ALIB_LANG_OWNER
//! @cond NO_DOX
#define HPP_ALIB_LANG_OWNER 1
//! @endcond



// #################################################################################################
// Macros (for technical reasons, doxed in file alib.cpp)
// #################################################################################################
#define   ALIB_OWN(ownable)                                                                             \
aworx::lib::lang::Owner<decltype(ownable)> ALIB_IDENTIFIER(owner)                                  \
   (ownable  ALIB_COMMA_DBG   ALIB_CALLER_PRUNED);


namespace aworx { namespace lib { namespace lang {

/** ************************************************************************************************
 * Ensures that an object of template type \p{TOwnable} is acquired and properly released when
 * unwinding the stack. This class is meant to be allocated only on the stack and not on the heap.
 * Therefore, the new operators are declared private.
 *
 * In debug compilations the constructor expects caller source information parameters
 * \p{file}, \p{line} and \p{func}. It is convenient to use macro
 * \ref ALIB_CALLER_PRUNED to provide those.
 *
 * \see
 *   Preprocessor macros \ref ALIB_OWN, \ref ALIB_LOCK and \ref ALIB_LOCK_WITH for a convenient way to
 *   use this class.
 *
 * @tparam TOwnable The type to own. The type needs to have methods \b %Acquire and \b %Release
 *                  available.
 **************************************************************************************************/
template <typename TOwnable>
struct Owner
{
    // #############################################################################################
    // Disallow new operations.
    // #############################################################################################
    private:
        void* operator new  (size_t);        ///< Private declared new, to disallow heap allocation.
        void* operator new  (size_t, void*); ///< Private declared new, to disallow heap allocation.
        void* operator new[](size_t);        ///< Private declared new, to disallow heap allocation.
        void* operator new[](size_t, void*); ///< Private declared new, to disallow heap allocation.
        void  operator =    (const Owner& ); ///< Private declared assignment operator.

    protected:
        /// All we own is this.
        TOwnable&    theOwnable;

    public:

        /** **************************************************************************************
         *  The constructor. Invokes Acquire() on the owner.
         * @param ownable    The ownable to acquire.
         *
         * @param file  Caller information. Available only in debug compilations.
         * @param line  Caller information. Available only in debug compilations.
         * @param func  Caller information. Available only in debug compilations.
         *****************************************************************************************/
         #if ALIB_DEBUG
             Owner( TOwnable& ownable, const NTString& file, int line, const NTString& func)
             : theOwnable(ownable)
             {
                ownable.Acquire( file, line, func );
             }
         #else
             Owner( TOwnable& ownable ) : theOwnable(ownable)
             {
                ownable.Acquire();
             }
         #endif

        /** **************************************************************************************
         *  The destructor. Releases the owner by invoking Release().
         *****************************************************************************************/
         ~Owner()                                        { theOwnable.Release(); }
};

};}}// namespace [aworx::lib::lang]


#if !ALIB_DEBUG
#define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL
#define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS

#else

#define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL                                     \
bool   dbgRecursionDetectionFlag   = false;

#define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS                                                 \
struct RecursionDetection                                                                          \
{                                                                                                  \
    bool& TestMember;                                                                              \
    RecursionDetection( bool& testMember ) : TestMember(testMember) {}                             \
                                                                                                   \
    void  Acquire( const NTString&, int, const NTString& func )                                    \
    {                                                                                              \
       ALIB_ASSERT_ERROR( TestMember==false,                                                       \
                          ASTR("Forbidden recursive use of method "), AString(func).ToCString() )  \
       TestMember= true;                                                                           \
    }                                                                                              \
    void  Release()    { TestMember= false;    }                                                   \
};                                                                                                 \
RecursionDetection dbgRecursionDetection( dbgRecursionDetectionFlag );                             \
ALIB_OWN(dbgRecursionDetection);
#endif

#endif // HPP_ALIB_LANG_OWNER
