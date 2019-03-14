// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_THREADS_SMARTLOCK
#define HPP_ALIB_THREADS_SMARTLOCK 1

#if !defined (HPP_ALIB_THREADS_THREADLOCK)
    #include "alib/threads/threadlock.hpp"
#endif

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif

namespace aworx { namespace lib {  namespace threads {

/** ************************************************************************************************
 * This class extends class ThreadLock, adding functionality to register 'acquirers' of type
 * \b %ThreadLock. Only with the second \e acquirer added, the lock is activated using method
 * \alib{threads,ThreadLock::SetSafeness}.
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
 * explained static field #StdOutputStreams.
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
        /** A lock for acquirer management. */
        ThreadLockNR                        lock;

        /** The list of acquirers. */
        std::vector<ThreadLock*>            acquirers;

    // #############################################################################################
    // Constructors
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Constructs a SmartLock. Parent ThreadLock is initialized to Unsafe mode.
         ******************************************************************************************/
        SmartLock(): ThreadLock( Safeness::Unsafe )
        {}

        /** ****************************************************************************************
         * Overwriting ThreadLock::Acquire. In debug compilations, asserts that at least one
         * acquirer is set.
         *
         * \note
         *   In debug-compilations of the library, this method accepts three parameters,
         *   providing information about the caller. In the release version these parameters do not
         *   exist. Therefore use macro #ALIB_CALLER_PRUNED to provide the parameters:
         *
         *          sample.Acquire( ALIB_CALLER_PRUNED );
         *
         * @param file  Caller information. Available only in debug compilations.
         * @param line  Caller information. Available only in debug compilations.
         * @param func  Caller information. Available only in debug compilations.
         ******************************************************************************************/
        inline
        #if ALIB_DEBUG
            void  Acquire( const NCString& file, int line, const NCString& func )
            {
                ALIB_ASSERT_ERROR( acquirers.size() > 0, "Must not be acquired without acquirers." )
                ThreadLock::Acquire(file,line,func);
            }
        #else
            void  Acquire()
            {
                ALIB_ASSERT_ERROR( acquirers.size() > 0, "Must not be acquired without acquirers." )
                ThreadLock::Acquire();
            }
        #endif

        using ThreadLock::Release;

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Adds an acquirer. With the second acquirer added, this lock will be set into safe mode.
         * @param newAcquirer The acquirer to add.
         * @return The new number of \e acquirers set.
         ******************************************************************************************/
        ALIB_API
        int     AddAcquirer( ThreadLock* newAcquirer );

        /** ****************************************************************************************
         * Removes an acquirer. If the amount of acquirers after removal equals \c 1, then
         * this lock will be set to unsafe mode.
         * @param acquirer The acquirer to remove.
         * @return The new number of \e acquirers set.
         ******************************************************************************************/
        ALIB_API
        int     RemoveAcquirer( ThreadLock* acquirer );

        /** ****************************************************************************************
         * Returns the number of \e acquirers. This is for debug and statistics purposes.
         * @return The number of \e acquirers set.
         ******************************************************************************************/
        ALIB_API
        int     CntAcquirers();

        /** ****************************************************************************************
         * This is a static singleton of this class that allows to lock an application's
         * <em>standard output streams</em>.
         *
         * In multi-threaded processes, to protect the output streams from concurrent access,
         * this smart lock might be used by any \e entity that writes data to the streams.
         * Before it can be used (acquired and released), it is needed to register with the object
         * using \alib{threads,SmartLock::AddAcquirer}.
         * This has to be done once per thread that aims to write to the stream. Then, prior to
         * writing, this object has to be acquired and after writing released.
         *
         * Because often, the standard \e output stream and standard \e error stream are identical,
         * \alib provides one single lock for both, to protect also against interwoven
         * standard output and error information.
         *
         * If the 'entity' that is registering is not of type \alib{threads,ThreadLock} it is allowed to
         * provide \c nullptr in the parameter of method \b AddAcquirer. In this case, the process
         * of adding and removing \e acquirers is not performed in a thread safe way.
         * Therefore it is advised to register so called anonymous (\c nullptr) \e acquirers only at
         * bootstrap time, when no parallel threads were started, yet.
         *
         * If an application is deemed to always write to the standard output streams from within
         * multiple threads, an alternative to registering each writing entity, is to
         * invoke \b AddAcquirer just two times in a row with \c nullptr at the start of a process
         * and then never do this again (and never de-register). This way, no thread needs
         * to register/de-register but threads may still \b Acquire and \b Release the lock without
         * being registered. In other words, once a smart lock is enabled, subsequent registrations
         * are just used to count and identify the de-registration.
         *
         * \note
         *   The advantage of the SmartLock is that if only one 'entity' registered, no
         *   system \e mutexes will be used with \b Acquire and \b Release, hence there is
         *   a performance gain. Such gain is not noticeable for the 'slow' terminal console output,
         *   but it is for fast, buffered output streams.
         * <p>
         * \note
         *   Logging module \alibmod_alox, which is built on \alib, will register whenever a \e Logger
         *   is used that writes to the standard output stream. Hence, applications that in
         *   parallel use, e.g. 'std::cout', should register at bootstrap and \e acquire this
         *   instance prior to writing. This way, log output and other application output is
         *   not mixed, but separated in different Lines.
         * <p>
         ******************************************************************************************/
         ALIB_API static
         SmartLock     StdOutputStreams;
};



}} // namespace aworx[::lib::threads]

/// Type alias in namespace #aworx.
using       SmartLock=  aworx::lib::threads::SmartLock;

}  // namespace [aworx]

#endif // HPP_ALIB_THREADS_SMARTLOCK
