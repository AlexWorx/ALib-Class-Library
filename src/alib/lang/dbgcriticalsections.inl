//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::lang {

/// This external variable is available only if the compiler-symbol
/// \ref ALIB_DEBUG_CRITICAL_SECTIONS is set and module \alib_threads is included in the
/// \alibbuild_nl.<br>
/// When it is \c 0, which is the default, nothing is done.<br>
/// When it is \c 1, then \alib{threads;Thread::YieldToSystem} is invoked with interface methods
/// of class \alib{lang;DbgCriticalSections}.<br>
/// Other values are passed to a call to \alib{threads;Thread::SleepNanos}.
///
/// The purpose of this debug-feature is to be better able to detect non-protected concurrent
/// access to critical sections. With increasing the time slice that a thread remains in
/// a critical section, the probability to be caught by another thread is increased.
extern unsigned  DBG_CRITICAL_SECTION_YIELD_OR_SLEEP_TIME_IN_NS;


/// This class supports detecting a racing condition in multithreaded applications.
/// For this, entering and exiting of critical sections is tracked in either "write" or "read-only"
/// mode.
/// The interface methods are named in accordance with types \alib{lang;Owner} (#Acquire/#Release)
/// and \alib{lang;OwnerShared} (#AcquireShared/#ReleaseShared).
///
/// With the use of atomic counters, both pairs of acquirement are reentrant, which simplifies
/// the use of this type. Furthermore, it is allowed to gain read access after write access by
/// the same thread was acquired. The reverse is not allowed: If the first read access was
/// registered, a later write access will be asserted.
///
/// The type becomes empty, and thus any call will be optimized out in case the compiler-symbol
/// \ref ALIB_DEBUG_CRITICAL_SECTIONS is not set.
/// Nevertheless, it is recommended to exclusively use the
/// \ref GrpALibMacros_mod_threads "ALib Module Threads Macros" when using this type, as those
/// fully guarantee that any use of this type is pruned with release compilations.
/// Only in exclamatory cases, these macros may not be flexible enough for use.
///
/// In case critical sections that are protected using this class are in fact protected by
/// one of the \ref alib_threads_locks "ALib mutex types", it can furthermore be asserted that
/// such mutex is acquired when a section is entered.
/// For this, assign the \b instance to the field #DCSLock.
/// A lock-instance may be assigned to more than one \b DbgCriticalSections instance.
///
/// If the compiler-symbol \b ALIB_DEBUG_CRITICAL_SECTIONS is set, besides counting owners and
/// readers, and raising corresponding assertions, the type can be enabled to simulate some workload
/// on the using machine. For this, a thread can be forced to either yield to the system or even
/// to sleep a given number of nanoseconds before continuing execution.
/// This increases the probability of (detecting) racing conditions.<br>
/// To activate this feature for all instances, the namespace variable
/// \alib{lang;DBG_CRITICAL_SECTION_YIELD_OR_SLEEP_TIME_IN_NS} is to be adjusted. Per-instance
/// adjustments can be made by setting the field #DCSYieldOrSleepTimeInNS.
///
/// The output format of assertions should be 'clickable' inside a users' IDE.
/// The default output string is optimized for
/// \https{JetBrains CLion,www.jetbrains.com/clion} and can be changed by manipulating
/// the member #ASSERTION_FORMAT.
///
/// @see Chapter \ref alib_threads_intro_assert of the Programmer's Manual of the module
///      \alib_threads.
struct DbgCriticalSections {
#if ALIB_DEBUG_CRITICAL_SECTIONS
    /// Virtual class that (usually) holds a lockable type and checks if it is locked or
    /// shared-locked.
    /// An instance of this type may be assigned to a \b DbgCriticalSections instance.<br>
    /// Implementations for the six lock types exists. Custom implementations may of course be
    /// given. This is done, for example, in with class \alib{threadmodel;ThreadPool} of sibling
    /// module \alib_threadmodel.
    struct AssociatedLock
    {
        /// Virtual Destructor
        virtual ~AssociatedLock()                                                                 {}

        /// @return \c true if the lock is acquired (in non-shared mode), \c false otherwise.
        virtual bool DCSIsAcquired()                                                       const =0;

        /// @return \c true if the lock is shared-acquired (by at least any thread).
        ///            Otherwise, returns \c false.
        virtual bool DCSIsSharedAcquired()                                                 const =0;
    }; // struct AssociatedLock

    /// The name of this DCS. Used for debug-output.
    const char*               DCSName;

    /// If positive, the value found here, overwrites what is given with namespace variable
    /// \alib{lang;DBG_CRITICAL_SECTION_YIELD_OR_SLEEP_TIME_IN_NS}.<br>
    /// Defaults to \c -1.
    int                       DCSYieldOrSleepTimeInNS                                          = -1;

    mutable std::atomic<int>  DCSWriterCnt{0};  ///< Tracks enter/exit calls (including readers)
    mutable std::atomic<int>  DCSReaderCnt{0};  ///< Tracks enter/exit calls of readers.
    mutable CallerInfo        DCSAcq         ;  ///< Source location of acquirement.
    mutable CallerInfo        DCSRel         ;  ///< Source location of the last "reader" seen.
    mutable CallerInfo        DCSSAcq        ;  ///< Source location of acquirement.
    mutable CallerInfo        DCSSRel        ;  ///< Source location of the last "reader" seen.

    /// A union of pointers to different lock types.
    /// Those can optionally be attached to be checked whether a lock is duly acquired.
    AssociatedLock*           DCSLock                                                     {nullptr};

    /// The format string used to write exceptions to the console.
    /// This string can be changed if the source information is not "clickable" in a user's
    /// development environment.<br>
    ///
    /// The default string is optimized for
    /// \https{JetBrains CLion,www.jetbrains.com/clion} and is defined as:
    /**  \verbatim
Assertion in Critical Section \"{}\"
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
 */
    ALIB_DLL
    static const char* ASSERTION_FORMAT;

  //################################################################################################
  // internals/helpers
  //################################################################################################

    /// Evaluates #DCSYieldOrSleepTimeInNS, respectively, if this is negative,
    /// \alib{lang;DBG_CRITICAL_SECTION_YIELD_OR_SLEEP_TIME_IN_NS}. For value
    /// - \c 0, nothing is done, for
    /// - \c 1, a yield into the system is done, and for
    /// - values greater than \c 1, the calling thread sleeps for the corresponding number of
    ///   nanoseconds sleep time.
    void yieldOrSleep()                                                                      const {
        int64_t yieldOrSleep= DCSYieldOrSleepTimeInNS >= 0 ? int64_t(DCSYieldOrSleepTimeInNS)
                                                           : DBG_CRITICAL_SECTION_YIELD_OR_SLEEP_TIME_IN_NS;
             if ( yieldOrSleep == 1) std::this_thread::yield();
        else if ( yieldOrSleep >= 2) std::this_thread::sleep_for( std::chrono::nanoseconds( yieldOrSleep ) );
    }

    /// Asserts the condition and if \c false, \ref alib_mod_assert "raises an ALib error".
    /// @param cond     The condition to assert.
    /// @param ciAssert Caller information of the assertion in this class.
    /// @param ci       Caller information.
    /// @param headline The problem that occurred.
    ALIB_DLL
    void doAssert( bool cond, const CallerInfo& ciAssert, const CallerInfo& ci,
                   const char* headline )                                                     const;


  //################################################################################################
  // Constructor/Destructor
  //################################################################################################
    /// Constructor.
    /// @param name The name to display with assertions.
    DbgCriticalSections(const char* name) : DCSName(name)                                         {}

    /// Destructor. Checks that this instance is unused.
    ~DbgCriticalSections()
    {
        doAssert(DCSWriterCnt.load() == 0,  ALIB_CALLER, ALIB_CALLER, "Destructor called while still owned" );
        doAssert(DCSReaderCnt.load() == 0,  ALIB_CALLER, ALIB_CALLER, "Destructor called while still owned (shared)" );
    }


  //################################################################################################
  // Interface
  //################################################################################################

    /// Increases the #DCSWriterCnt and checks for potential assertions.
    /// @param ci Caller information.
    ALIB_DLL
    void        Acquire      ( const CallerInfo& ci )                                         const;

    /// Decreases the #DCSWriterCnt and checks for potential assertions.
    /// @param ci Caller information.
    ALIB_DLL
    void        Release      ( const CallerInfo& ci )                                         const;

    /// Increases #DCSReaderCnt and checks for potential assertions.
    /// @param ci Caller information.
    ALIB_DLL
    void        AcquireShared( const CallerInfo& ci )                                         const;

    /// Decreases #DCSReaderCnt and checks for potential assertions.
    /// @param ci Caller information.
    ALIB_DLL
    void        ReleaseShared( const CallerInfo& ci )                                         const;

#else // ALIB_DEBUG_CRITICAL_SECTIONS
    void  Acquire      ( const lang::CallerInfo& )                                         const {}
    void  Release      ( const lang::CallerInfo& )                                         const {}
    void  AcquireShared( const lang::CallerInfo& )                                         const {}
    void  ReleaseShared( const lang::CallerInfo& )                                         const {}
#endif // ALIB_DEBUG_CRITICAL_SECTIONS
};


} // namespace [alib::lang]
