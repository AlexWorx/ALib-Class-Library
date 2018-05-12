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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_THREADS_SMARTLOCK)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_THREADS_SMARTLOCK
//! @cond NO_DOX
#define HPP_ALIB_THREADS_SMARTLOCK 1
//! @endcond

// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_THREADS_THREADLOCK)
    #include "alib/threads/threadlock.hpp"
#endif
#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif

namespace aworx { namespace lib { namespace threads
{

/** ************************************************************************************************
 * This class extends class ThreadLock, adding functionality to register 'acquirers' of type
 * \b %ThreadLock. Only with the second \e acquirer added, the lock is activated
 * using \ref aworx::lib::threads::ThreadLock::SetSafeness "ThreadLock::SetSafeness(Safeness::Safe)".
 * The goal is to not use a mutex, when such use is not needed. In occasions with very high
 * frequency of acquisition, this can provide a performance benefit.
 *
 * <b>The following rules apply:</b><br>
 * - An instance of this type must not be acquired before an \e acquirer is registered.
 * - The \e acquirers have to be in recursive mode.
 * - If \e acquirers are locked in a nested fashion, then they have to be added
 *   in the same order they are locked and removed in reverse order
 * - An \e acquirer must not be added twice. (This is not a technical restriction, but a chosen
 *   design. While a second addition is ignored, in debug versions of the code, an
 *   <em>ALib Error Report</em> is written (by default this triggers an assertion).
 *
 * <b>Using nulled acquirers:</b><br>
 * Sometimes it is useful to add a \c nullptr as an \e acquirer. A sample for this is found and
 * explained with
 * \ref aworx::lib::ALib::StdOutputStreamsLock "ALib::StdOutputStreamsLock".
 * If the first acquirer is nullptr, the second should be added in a thread-safe way. This means,
 * the code invoking #AddAcquirer needs to care for itself, that this object is not acquired
 * during this process. E.g. it can be done in the bootstrap section of a process, when no parallel
 * threads were started. For further acquirers, such care does not need to be taken.
 * While an \e acquirer must not be attached twice, 'anonymous' (nullptr) \e acquirers may.
 * For each anonymous invocation of #AddAcquirer, a corresponding call #RemoveAcquirer is
 * needed, to get back to \b Safeness::Unsafe.
 **************************************************************************************************/
class SmartLock : public ThreadLock
{
    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /// A lock for acquirer management.
        ThreadLockNR                        lock;

        /// The list of acquirers.
        std::vector<ThreadLock*>             acquirers;

    // #############################################################################################
    // Constructors
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Constructs an SmartLock. Parent ThreadLock is initialized to Unsafe mode.
         ******************************************************************************************/
        SmartLock(): ThreadLock( lang::LockMode::Recursive, lang::Safeness::Unsafe    )
        {}

        /** ****************************************************************************************
         * Virtual (empty) destructor.
         ******************************************************************************************/
        virtual ~SmartLock()
        {}

        /** ****************************************************************************************
         * Overwriting ThreadLock::Acquire. Writes an error report, if no \e acquirers are
         * attached.
         * @param file  Caller information. Available only in debug compilations.
         * @param line  Caller information. Available only in debug compilations.
         * @param func  Caller information. Available only in debug compilations.
         ******************************************************************************************/
        inline
        #if ALIB_DEBUG
            void  Acquire( const NTString& file, int line, const NTString& func )
            {
                ALIB_ASSERT_ERROR( acquirers.size() > 0, "Must not be acquired without acquirers." );
                ThreadLock::Acquire(file,line,func);
            }
        #else
            void  Acquire()
            {
                ALIB_ASSERT_ERROR( acquirers.size() > 0, "Must not be acquired without acquirers." );
                ThreadLock::Acquire();
            }
        #endif

        using ThreadLock::Release;

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Adds an acquirer.
         * @param newAcquirer The acquirer to add.
         * @return The new number of \e acquirers set.
         ******************************************************************************************/
        ALIB_API
        virtual int   AddAcquirer( ThreadLock* newAcquirer );

        /** ****************************************************************************************
         * Removes an acquirer.
         * @param acquirer The acquirer to remove.
         * @return The new number of \e acquirers set.
         ******************************************************************************************/
        ALIB_API
        virtual int   RemoveAcquirer( ThreadLock* acquirer );

        /** ****************************************************************************************
         * Returns the number of \e acquirers. This is for debug and statistics purposes.
         * @return The number of \e acquirers set.
         ******************************************************************************************/
        ALIB_API
        int           CntAcquirers();

};



}} // namespace lib::threads

/// Type alias in namespace #aworx.
using       SmartLock=  aworx::lib::threads::SmartLock;

}  // namespace aworx

#endif // HPP_ALIB_THREADS_SMARTLOCK
