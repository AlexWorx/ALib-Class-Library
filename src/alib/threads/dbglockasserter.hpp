//==================================================================================================
/// \file
/// This header file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_THREADS_DBGLOCKS
#define HPP_ALIB_THREADS_DBGLOCKS 1
#pragma once
#include "thread.hpp"

#if ALIB_DEBUG
#include <atomic>
#include "alib/strings/string.hpp"
#include "alib/time/ticks.hpp"

namespace alib::threads {

class Thread;

//==================================================================================================
/// This type is used for debugging and asserting \alib lock (mutex) types.
/// With debug compilations the non-shared lock types hold one member of this struct, which
/// aggregates all debug information.
//==================================================================================================
struct DbgLockAsserter
{
    /// The name of this Lock.
    /// Set to <em> "<unnamed>"</em> by default.
    /// Used for debug-output.
    /// For automatic pruning of changes to this name, macro \ref ALIB_DBG should be used.
    const char* Name                                                                   ="<unnamed>";
    CallerInfo  AcqCI ;                  ///< Source location of the most recent acquirement.
    CallerInfo  RelCI ;                  ///< Source location of the most recent release.
    int16_t     CntAcquirements= 0;      ///< The number of nested acquirements.

#if ALIB_CAMP
    /// The format string used to write exceptions to the console.
    /// This string can be changed if the source information is not "clickable" in a user's
    /// development environment.<br>
    ///
    /// The default string is optimized for
    /// \https{JetBrains CLion,www.jetbrains.com/clion} and is defined as:
    /**  \verbatim
Multi-Threadding {} in Lock {!Q}
                Message: {}
   In (Member-)Function: {:ya}
               Is Owned: {7}

              Called By: {4:ya}
                     At: {4:sf:sl}
                 Thread: {4:ta}

  Latest Acquisition By: {5:ya}
                     At: {5:sf:sl}
                 Thread: {5:ta}
      Latest Release By: {6:ya}
                     At: {6:sf:sl}
                 Thread: {6:ta}
      \endverbatim
      <p>
      The placeholder fields that this format string refers to are set as follows:
      - \c 0: String "Assertion" or "Warning"
      - \c 1: Debug-name of the lock.
      - \c 2: Headline.
      - \c 3: \b %CallerInfo of assertion.
      - \c 4: \b %CallerInfo of caller.
      - \c 5: \b %CallerInfo of latest acquisition.
      - \c 6: \b %CallerInfo of latest release.
      - \c 7: Acquirement information string

      The format specification of the type \c CallerInfo is defined with class
      \alib{lang::format;FMTCallerInfo}.
      \par Availability:
        This field is only available with the inclusion of \alib_basecamp in the \alibdist.<br> */
    ALIB_API
    static NString ASSERTION_FORMAT;
#endif


    /// This is a threshold that causes non-timed Acquire() methods to raise a \alib warning in
    /// debug-builds in case a thread is blocked longer than the given duration.
    ///
    /// To disable warnings in cases that high block times are suitable, set this value to \c 0.
    /// The default value is two seconds.
    Ticks::Duration                WaitTimeLimit          = Ticks::Duration::FromAbsoluteSeconds(2);

    /// Limit of recursions. If the limit is reached or a multiple of it, a warning is passed
    /// to \alib{lang;ReportWriter}.
    /// Defaults is \c 10. To disable, set to \c 0.<p>
    /// Available only in debug versions of \alib.
    int                            RecursionLimit                                               =10;

    /// Destructor.
    virtual ~DbgLockAsserter()                                                                    {}
    
    /// Returns a pointer the owning thread.<p>
    /// Available only in debug-builds.
    /// @return Pointer to the owning thread, or \c nullptr if not owned.
    ALIB_API
    Thread*         GetOwner()                                                                const;

    /// Returns \c true if the current owner is the current thread.<p>
    /// Available only in debug-builds.
    /// @return \c true, if the lock is owned by this thread, \c false if it is owned by
    ///            another thread or not owned.
    bool            IsOwnedByCurrentThread()                                                   const
    { return CntAcquirements > 0 &&  AcqCI.ThreadID == std::this_thread::get_id(); }

    /// Returns \c true if this is a non-recursive lock or a recursive instance which is acquired
    /// exactly once.<br>
    /// Available only in debug-builds.
    ///
    /// \note
    ///   This method is not (and cannot) be synchronized. Consequently, a reliable result
    ///   is only guaranteed in case #IsOwnedByCurrentThread returns \c true before this method
    ///   is invoked.
    ///
    /// @return \c true if the next release will free this lock, \c false otherwise.
    bool            WillRelease()                   const  noexcept { return CntAcquirements == 1; }

    /// Writes assertion info and calls \ref ALIB_ASSERT, respectively \ref ALIB_WARNING.
    /// @see
    ///    Field #ASSERTION_FORMAT which  allows  changing the output format to achieve 'clickable'
    ///    assertion messages.
    /// @param type     0= assertion, 1= warning.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    ALIB_API
    virtual
    void DoAssert       (int type, const CallerInfo& assertCI, const CallerInfo& ci,
                         const NString& headline );

    /// Asserts that #CntAcquirements is not \c 0
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertOwned    (const CallerInfo& assertCI, const CallerInfo& ci, const NString& headline )
    {
        if( CntAcquirements == 0 )
            DoAssert( 0, assertCI, ci, headline );
    }

    /// Asserts that #CntAcquirements is \c 0
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertNotOwned (const CallerInfo& assertCI, const CallerInfo& ci, const NString& headline )
    {
        if( CntAcquirements > 0 )
            DoAssert( 0, assertCI, ci, headline );
    }

    /// Asserts that either #CntAcquirements is \c 0 or the lock is owned by calling thread.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertNotOwnedOrMe (const CallerInfo& assertCI, const CallerInfo& ci, const NString& headline )
    {
        if( CntAcquirements > 0 && ci.ThreadID != AcqCI.ThreadID )
            DoAssert( 0, assertCI, ci, headline );
    }

    /// Asserts that this lock is owned by the thread in \p{ci}.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertOwning   (const CallerInfo& assertCI, const CallerInfo& ci, const NString& headline )
    {
        if( CntAcquirements == 0 || ci.ThreadID != AcqCI.ThreadID )
            DoAssert( 0, assertCI, ci, headline);
    }

    /// Asserts that this lock is not owned by the thread in \p{ci}.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertNotOwning(const CallerInfo& assertCI, const CallerInfo& ci, const NString& headline )
    {
        if( CntAcquirements > 0 && ci.ThreadID == AcqCI.ThreadID )
            DoAssert( 0, assertCI, ci, headline);
    }
}; // struct DbgLockAsserter

//==================================================================================================
/// This type is used for debugging and asserting \alib lock (mutex) types.
/// With debug compilations the shared lock types hold one member of this struct, which
/// aggregates all debug information.
//==================================================================================================
struct DbgSharedLockAsserter : DbgLockAsserter
{
    CallerInfo          SAcqCI;               ///< Source location of the most recent shared acquirement.
    CallerInfo          SRelCI;               ///< Source location of the most recent shared release.
    std::atomic<int>    CntSharedAcquirements{0};  ///< The number of shared acquirements.

#if ALIB_CAMP
    /// The format string used to write exceptions to the console.
    /// This string can be changed if the source information is not "clickable" in a user's
    /// development environment.<br>
    ///
    /// The default string is optimized for
    /// \https{JetBrains CLion,www.jetbrains.com/clion} and is defined as:
    /**  \verbatim
Multi-Threadding {} in Lock {!Q}
                       Message: {}
          In (Member-)Function: {:ya}
                      Is Owned: {7}
               Is Shared Owned: {10}
                     Called By: {4:ya}
                            At: {4:sf:sl}
                        Thread: {4:ta}
         Latest Acquisition By: {5:ya}
                            At: {5:sf:sl}
                        Thread: {5:ta}
             Latest Release By: {6:ya}
                            At: {6:sf:sl}
                        Thread: {6:ta}
  Latest Shared Acquisition By: {8:ya}
                            At: {8:sf:sl}
                        Thread: {8:ta}
       Latest SharedRelease By: {9:ya}
                            At: {9:sf:sl}
                        Thread: {9:ta}
      \endverbatim
      <p>
      The placeholder fields that this format string refers to are set as follows:

      - \c 0: String "Assertion" or "Warning"
      - \c 1: Debug-name of the lock.
      - \c 2: Headline.
      - \c 3: \b %CallerInfo of assertion.
      - \c 4: \b %CallerInfo of caller.
      - \c 5: \b %CallerInfo of latest acquisition.
      - \c 6: \b %CallerInfo of latest release.
      - \c 7: Acquirement information string
      - \c 8: \b %CallerInfo of latest shared acquisition.
      - \c 9: \b %CallerInfo of latest shared release.
      - \c 10: Shared acquirement information string.

      The format specification of the type \c CallerInfo is defined with class
      \alib{lang::format;FMTCallerInfo}.
      \par Availability:
        This field is only available with the inclusion of \alib_basecamp in the \alibdist.<br> */
    ALIB_API
    static NString ASSERTION_FORMAT_SHARED;
#endif


    /// Destructor.
    virtual ~DbgSharedLockAsserter()                                                     override {}
    
    /// Writes assertion info and calls \ref ALIB_ASSERT, respectively \ref ALIB_WARNING.
    /// @see
    ///    Field #ASSERTION_FORMAT which allows changing the output format to achieve 'clickable'
    ///    assertion messages.
    /// @param type     0= assertion, 1= warning.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    ALIB_API
    void DoAssert       (int type, const CallerInfo& assertCI, const CallerInfo& ci,
                         const NString& headline )                                         override;

    /// Returns \c true if currently a reader is registered. This method is used to
    /// create assertions. Of-course, to detect assertions, it would be more efficient to check
    /// if shared ownership is with the current thread, but a check on this would cost
    /// a lot of overhead to realize. This way, at least assertions can be raised when no other
    /// thread acquired this lock in shared mode.<p>
    /// Available only in debug-builds.
    /// @return \c true, if the lock is owned by this thread, \c false if it is owned by
    ///         another thread or not owned.
    bool    IsSharedOwnedByAnyThread()             const{ return CntSharedAcquirements.load() > 0; }

    /// Asserts that #CntAcquirements is \c 0
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertNotOwned (const CallerInfo& assertCI, const CallerInfo& ci, const NString& headline )
    {
        if( CntAcquirements > 0 || CntSharedAcquirements.load() > 0 )
            DoAssert( 0, assertCI, ci, headline );
    }


}; // struct DbgSharedLockAsserter

} // namespace [alib::threads]


#endif // ALIB_DEBUG
#endif // HPP_ALIB_THREADS_DBGLOCKS

