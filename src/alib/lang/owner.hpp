/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does not belong to an \alibmod and is
 * included in any \alibdist.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_OWNER
#define HPP_ALIB_LANG_OWNER 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

#if ALIB_STRINGS

#if !defined(HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

namespace alib { namespace lang {

/** ************************************************************************************************
 * Ensures that an object of template type \p{TOwnable} is acquired and properly released when
 * unwinding the stack. This class is meant to be allocated only on the stack and not on the heap.
 * Therefore, the new operators are declared private.
 *
 * With debug builds the constructor expects caller source information parameters
 * \p{file}, \p{line} and \p{func}. It is convenient to use macro
 * \ref ALIB_CALLER_PRUNED to provide those.
 *
 * Note that this type is available only if \alib_strings is included in the \alibdist.
 *
 * \see
 *   Preprocessor macros \ref ALIB_OWN, \ref ALIB_LOCK and \ref ALIB_LOCK_WITH for a convenient way
 *   to use this class.
 *
 * @tparam TOwnable The type to own. The type needs to have methods \b %Acquire and \b %Release
 *                  available.
 **************************************************************************************************/
template <typename TOwnable>
class Owner
{
    // #############################################################################################
    // Disallow new operations.
    // #############################################################################################
    private:
        /** Private new to disallow heap allocation.
         *  @return Never called.                        */
        void* operator new  (size_t);
        /** Private new to disallow heap allocation.
         *  @return Never called.                        */
        void* operator new  (size_t, void*);
        /** Private new to disallow heap allocation.
         *  @return Never called.                        */
        void* operator new[](size_t);
        /** Private new to disallow heap allocation.
         *  @return Never called.                        */
        void* operator new[](size_t, void*);
        /** Private assignment operator. */
        void  operator =    (const Owner& );

    protected:
        /** All we own is this. */
        TOwnable&    theOwnable;

    public:

        /** ****************************************************************************************
         *  The constructor. Invokes Acquire() on the owner.
         * @param ownable    The ownable to acquire.
         *
         * @param dbgFile  Caller information. Available only with debug builds.
         * @param dbgLine  Caller information. Available only with debug builds.
         * @param dbgFunc  Caller information. Available only with debug builds.
         ******************************************************************************************/
         #if ALIB_DEBUG
             Owner( TOwnable& ownable, const NCString& dbgFile, int dbgLine, const NCString& dbgFunc)
             : theOwnable(ownable)
             {
                ownable.Acquire( dbgFile, dbgLine, dbgFunc );
             }
         #else
             Owner( TOwnable& ownable ) : theOwnable(ownable)
             {
                ownable.Acquire();
             }
         #endif

        /** ****************************************************************************************
         *  The destructor. Releases the owner by invoking Release().
         ******************************************************************************************/
         ~Owner()                                        { theOwnable.Release(); }
}; //class Owner

} // namespace alib[::lang]

/// Type alias in namespace \b alib.
template <typename TOwnable>
using Owner      = lang::Owner<TOwnable>;

} // namespace [alib]


// #################################################################################################
// Macros
// #################################################################################################
#define   ALIB_OWN(ownable)                                                                        \
alib::Owner<decltype(ownable)> ALIB_IDENTIFIER(owner) (ownable ALIB_COMMA_DBG ALIB_CALLER_PRUNED);


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
    void  Acquire( const NCString&, int, const NCString& func )                                    \
    {                                                                                              \
       ALIB_ASSERT_ERROR(TestMember==false, "FSOWNER", "Forbidden recursive use of method ", func) \
       TestMember= true;                                                                           \
    }                                                                                              \
    void  Release()    { TestMember= false;    }                                                   \
};                                                                                                 \
RecursionDetection dbgRecursionDetection( dbgRecursionDetectionFlag );                             \
ALIB_OWN(dbgRecursionDetection);
#endif

#endif // ALIB_STRINGS

#endif // HPP_ALIB_LANG_OWNER
