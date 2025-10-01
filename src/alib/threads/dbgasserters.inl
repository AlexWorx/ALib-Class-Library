//==================================================================================================
/// \file
/// This header-file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if !ALIB_SINGLE_THREADED && ALIB_DEBUG

ALIB_EXPORT namespace alib::threads {

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

    /// The format string used to write exceptions to the console.
    /// This string can be changed if the source information is not "clickable" in a user's
    /// development environment.<br>
    ///
    /// The default string is optimized for
    /// \https{JetBrains CLion,www.jetbrains.com/clion} and is defined as:
    /**  \verbatim
Multi-Threadding {} in Lock \"{}\"
                Message: {}
   In (Member-)Function: {}
               Is Owned: {} ({})

              Called By: {}::{}
                     At: {}:{}
                 Thread: {}

  Latest Acquisition By: {}::{}
                     At: {}:{}
                 Thread: {}
      Latest Release By: {}:{}
                     At: {}:{}
                 Thread: {}
      \endverbatim
      <p>
      The placeholder fields that this format string refers to are set as follows:
      - \c 0: String "Assertion" or "Warning"
      - \c 1: Debug-name of the lock.
      - \c 2: Headline.
      - \c 3: Function name.
      - \c 4: Is acquired (true/false).
      - \c 5: number of acquirements.
      - \c 6-10: \b %CallerInfo of caller.
      - \c 11-15: \b %CallerInfo of the latest acquisition.
      - \c 16-20: \b %CallerInfo of the latest release.
*/
    ALIB_DLL
    static const char* ASSERTION_FORMAT;


    /// This is a threshold that causes non-timed Acquire() methods to raise a \alib warning in
    /// debug-builds in case a thread is blocked longer than the given duration.
    ///
    /// To disable warnings in cases that high block times are suitable, set this value to \c 0.
    /// The default value is two seconds.
    Ticks::Duration                WaitTimeLimit          = Ticks::Duration::FromAbsoluteSeconds(2);

    /// Limit of recursions. If the limit is reached or a multiple of it, an
    /// \alib warning is raised.
    /// Defaults is \c 10. To disable, set to \c 0.<p>
    /// Available only in debug versions of \alib.
    int                            RecursionLimit                                               =10;

    /// Destructor.
    virtual ~DbgLockAsserter()                                                                    {}
    
    /// Returns a pointer the owning thread.<p>
    /// Available only in debug-builds.
    /// @return Pointer to the owning thread, or \c nullptr if not owned.
    ALIB_DLL
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

    /// Collects assertion info and \ref alib_mod_assert "raises a warning or error".
    /// @see
    ///    Field #ASSERTION_FORMAT which allows changing the output format to achieve 'clickable'
    ///    assertion messages.
    /// @param type     0= assertion, 1= warning.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    ALIB_DLL
    virtual
    void DoAssert       (int type, const CallerInfo& assertCI, const CallerInfo& ci,
                         const char* headline );

    /// Asserts that #CntAcquirements is not \c 0
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertOwned    (const CallerInfo& assertCI, const CallerInfo& ci, const char* headline )
    {
        if( CntAcquirements == 0 )
            DoAssert( 0, assertCI, ci, headline );
    }

    /// Asserts that #CntAcquirements is \c 0
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertNotOwned (const CallerInfo& assertCI, const CallerInfo& ci, const char* headline )
    {
        if( CntAcquirements > 0 )
            DoAssert( 0, assertCI, ci, headline );
    }

    /// Asserts that either #CntAcquirements is \c 0 or the lock is owned by calling thread.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertNotOwnedOrMe (const CallerInfo& assertCI, const CallerInfo& ci, const char* headline )
    {
        if( CntAcquirements > 0 && ci.ThreadID != AcqCI.ThreadID )
            DoAssert( 0, assertCI, ci, headline );
    }

    /// Asserts that this lock is owned by the thread in \p{ci}.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertOwning   (const CallerInfo& assertCI, const CallerInfo& ci, const char* headline )
    {
        if( CntAcquirements == 0 || ci.ThreadID != AcqCI.ThreadID )
            DoAssert( 0, assertCI, ci, headline);
    }

    /// Asserts that this lock is not owned by the thread in \p{ci}.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    void AssertNotOwning(const CallerInfo& assertCI, const CallerInfo& ci, const char* headline )
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

    /// The format string used to write exceptions to the console.
    /// This string can be changed if the source information is not "clickable" in a user's
    /// development environment.<br>
    ///
    /// The default string is optimized for
    /// \https{JetBrains CLion,www.jetbrains.com/clion} and is defined as:
    /**  \verbatim
Multi-Threadding {} in Shared-Lock \"{}\"
                       Message: {}
          In (Member-)Function: {}
                      Is Owned: {} ({})
               Is Shared Owned: {} ({})

                     Called By: {}::{}
                            At: {}:{}
                        Thread: {}

         Latest Acquisition By: {}::{}
                            At: {}:{}
                        Thread: {}
             Latest Release By: {}::{}
                            At: {}:{}
                        Thread: {}

  Latest Shared Acquisition By: {}::{}
                            At: {}:{}
                        Thread: {}
       Latest SharedRelease By: {}::{}
                            At: {}:{}
                        Thread: {}
      \endverbatim
      <p>
      The placeholder fields that this format string refers to are set as follows:

      - \c 0: String "Assertion" or "Warning"
      - \c 1: Debug-name of the lock.
      - \c 2: Headline.
      - \c 3: Function name.
      - \c 4: Is acquired (true/false).
      - \c 5: number of acquirements.
      - \c 6: Is shared acquired (true/false).
      - \c 7: number of shared-acquirements.
      - \c 8-12:  \b %CallerInfo of caller.
      - \c 13-17: \b %CallerInfo of the latest acquisition.
      - \c 18-22: \b %CallerInfo of the latest release.
      - \c 23-27: \b %CallerInfo of the latest shared-acquisition.
      - \c 32-36: \b %CallerInfo of the latest shared-release.
 */
    ALIB_DLL
    static const char* ASSERTION_FORMAT_SHARED;


    /// Destructor.
    virtual ~DbgSharedLockAsserter()                                                     override {}
    
    /// Assembles assertion info and \ref alib_mod_assert "raises a warning or an error".
    /// @see
    ///    Field #ASSERTION_FORMAT which allows changing the output format to achieve 'clickable'
    ///    assertion messages.
    /// @param type     0= assertion, 1= warning.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    ALIB_DLL
    void DoAssert       (int type, const CallerInfo& assertCI, const CallerInfo& ci,
                         const char* headline )                                         override;

    /// Returns \c true if currently a reader is registered. This method is used to
    /// create assertions. of course, to detect assertions, it would be more efficient to check
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
    void AssertNotOwned (const CallerInfo& assertCI, const CallerInfo& ci, const char* headline )
    {
        if( CntAcquirements > 0 || CntSharedAcquirements.load() > 0 )
            DoAssert( 0, assertCI, ci, headline );
    }
}; // struct DbgSharedLockAsserter


//==================================================================================================
/// This type is used for debugging and asserting types \alib{threads;TCondition}.
/// With debug compilations that class holds one member of this struct, which aggregates all
/// debug information.
//==================================================================================================
struct DbgConditionAsserter
{
    const character*  Name;                  ///< The name of this instance.
    Thread*           Owner{nullptr};        ///< Tracks the current owner.
    CallerInfo        AcqCI ;                ///< Source location of the most recent acquirement.
    CallerInfo        RelCI ;                ///< Source location of the most recent release.
    CallerInfo        WaitCI;                ///< The most recent call to \b WaitForNotification.
    CallerInfo        NotifyCI;              ///< The most recent call to \b ReleaseAndNotify or
    std::atomic<int>  CntWaiters{0};         ///< The number of currently waiting threads.
    std::thread::id   AssertExclusiveWaiter; ///< If set from outside, methods \b WaitForNotification
                                             ///< will raise an assertion in the case they are called
                                             ///< from a different thread.
    /// The format string used to write exceptions to the console.
    /// This string can be changed if the source information is not "clickable" in a user's
    /// development environment.<br>
    ///
    /// The default string is optimized for
    /// \https{JetBrains CLion,www.jetbrains.com/clion} and is defined as:
    /**  \verbatim
Assertion failed in method TCondition::{}
                 Message: {}
                Instance: {}

               Called By: {}::{}
                      At: {}:{}
                  Thread: {}

           Current Owner: {}
             #Of Waiters: {}
        Exclusive Waiter: {}

   Latest Acquisition By: {}::{}
                      At: {}:{}
                  Thread: {}
       Latest Release By: {}::{}
                      At: {}:{}
                  Thread: {}

          Latest Wait By: {}::{}
                      At: {}:{}
                  Thread: {}
        Latest Notify By: {}::{}
                      At: {}:{}
                  Thread: {}
      \endverbatim
*/
    ALIB_DLL
    static const char* ASSERTION_FORMAT;

    /// Writes assertion info and calls \ref ALIB_ASSERT.
    /// @see
    ///    Field #ASSERTION_FORMAT which allows changing the output format to achieve 'clickable'
    ///    assertion messages.
    /// @param cond     The condition that is to be met.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    ALIB_DLL
    void Assert( bool cond, const CallerInfo& assertCI, const CallerInfo& ci,
                 const char* headline );

    /// Returns \c true if the current owner is the current thread.<p>
    /// Available only in debug-builds.
    /// @return \c true, if the lock is owned by this thread, \c false if it is owned by
    ///            another thread or not owned.
    bool            IsOwnedByCurrentThread()                                                   const
    { return Owner != nullptr &&  Owner == Thread::GetCurrent() ; }

}; // struct DbgConditionAsserter

} // namespace [alib::threads]

#endif // !ALIB_SINGLE_THREADED && ALIB_DEBUG
