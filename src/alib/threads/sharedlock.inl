//==================================================================================================
/// \file
/// This header-file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if !ALIB_SINGLE_THREADED
ALIB_EXPORT namespace alib {  namespace threads {

// =================================================================================================
/// This class is a simple wrapper around C++ standard library type \c std::shared_mutex.
/// Thus, it is used to implement <em>mutual exclusive access</em> to resources by protecting
/// critical code sections from being executed in parallel in concurrent threads, while
/// allowing typical read-operations to continue to be executed in parallel.
///
/// When a pair of #Acquire and #Release invocations is performed within the same code block, then
/// it is recommended to use a stack instantiation of class \alib{lang;Owner} to acquire and release
/// objects of this class.
/// Such a use is highly simplified with macros \ref ALIB_LOCK and \ref ALIB_LOCK_WITH.<br>
/// The same is recommended for paired invocations of #AcquireShared and #ReleaseShared.
/// Here, class \alib{lang;OwnerShared} is to be used, best using macros \ref ALIB_LOCK_SHARED and
/// \ref ALIB_LOCK_SHARED_WITH.
///
/// This class does not allow nested calls to the method #Acquire - method #Release has to be
/// invoked (from within the same thread that acquired this mutex), before any other thread can
/// again gain access. Nested acquisitions constitute undefined behavior.
///
/// Typically macros \ref ALIB_LOCK and \ref ALIB_LOCK_WITH should be used to acquire
/// this lock in non-shared mode and macros ALIB_LOCK_SHARED and \ref ALIB_LOCK_SHARED_WITH
/// for shared acquisitions.
///
/// \par Debug-Features
/// Public member #Dbg is available with debug-compilations. It offers the following features:
/// - An assertion is raised when nested use is performed.
/// - The object stores the actual owning thread and the source code position of the last
///   acquirement.
///   Likewise the last shared acquirement's caller information is stored.
/// - Releasing non-acquired instances, as well as destructing acquired one, raise an assertion.
/// - Field \alib{threads;DbgLockAsserter;WaitTimeLimit} enables the raise of \alib warnings in case a
///   certain wait time is exceeded.
///   Note that instead of wrapping \c std::shared_mutex, with debug-compilations class
///   \c std::shared_timed_mutex is wrapped.
/// - Field #DbgWarningMaximumShared enables the raise of \alib warnings in the case that
///   the number of parallel shared acquirements reaches the limit given with this member.
///
/// \par Availability
/// This type is not available if the compiler-symbol \ref ALIB_SINGLE_THREADED is set.
///
/// @see
///  - Chapter \ref alib_threads_locks of the Programmer's Manual of the module \alib_threads_nl.
///  - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
// =================================================================================================
class SharedLock
#if ALIB_DEBUG_CRITICAL_SECTIONS
: public lang::DbgCriticalSections::AssociatedLock
#endif
{
  protected:
  #if !ALIB_DEBUG && !DOXYGEN
    std::shared_mutex         mutex; // the only member in release compilations

  #else
    #if DOXYGEN
        /// The internal object to lock on.
        /// \note With debug-compilations, this is of type <c>std::timed_mutex</c>.
        std::shared_mutex        mutex;
    #else
        std::shared_timed_mutex  mutex;
    #endif

  public:
    /// The debug tool instance.
    DbgSharedLockAsserter        Dbg;

    /// Warning-threshold of maximum number of parallel shared acquisitions.<br>
    /// Defaults to 10.
    std::atomic<int>     DbgWarningMaximumShared                                                =10;
  #endif
  public:

    #if ALIB_DEBUG_CRITICAL_SECTIONS
        /// Destructor. With debug-compilations, asserts that this lock is not acquired.
        ~SharedLock()    override
        { Dbg.AssertNotOwned( ALIB_CALLER, ALIB_CALLER, "Destructing acquired lock" ); }

        /// @return \c true if the lock is acquired (in non-shared mode), \c false otherwise.
        ALIB_DLL virtual bool DCSIsAcquired()                                        const override;

        /// @return \c true if the lock is shared-acquired (by at least any thread).
        ///            Otherwise, returns \c false.
        ALIB_DLL virtual bool DCSIsSharedAcquired()                                  const override;
    #elif ALIB_DEBUG
        ~SharedLock()
        { Dbg.AssertNotOwned( ALIB_CALLER, ALIB_CALLER, "Destructing acquired lock" ); }
    #endif

  // ===============================================================================================
  // ====  Standard Acquire/Release (Writer)
  // ===============================================================================================

  #if ALIB_DEBUG || DOXYGEN
    /// Acquires this lock.
    /// In the case that this object is already owned by another thread, the invoking thread is
    /// suspended until ownership can be gained.
    /// Multiple (nested) calls to this method are not supported and lead to undefined behavior.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
     ALIB_DLL
     void  Acquire( ALIB_DBG_TAKE_CI );

    /// Tries to acquire this lock.
    /// Multiple (nested) successful calls to this method or method #Acquire are not supported and
    /// lead to undefined behavior.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    /// @return \c true if the lock was not acquired by a different thread and thus, this
    ///         call was successful. \c false otherwise.
    ALIB_DLL
    [[nodiscard]]
    bool TryAcquire( ALIB_DBG_TAKE_CI );

    /// Releases ownership of this object.
    /// If this method is invoked on an object that is not acquired, in debug-compilations an
    /// assertion is raised. In release compilations, this leads to undefined behavior.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    ALIB_DLL
    void Release( ALIB_DBG_TAKE_CI );

  #else
                   void  Acquire()                                                 { mutex.lock(); }
    [[nodiscard]]  bool  TryAcquire()                                   { return mutex.try_lock(); }
                   void  Release()                                               { mutex.unlock(); }

  #endif


  // ===============================================================================================
  // ====  Shared Acquire/Release (Reader)
  // ===============================================================================================
  #if ALIB_DEBUG || DOXYGEN
    /// Acquires this lock in shared mode.
    /// In the case that this object is already owned (not shared) by another thread, the invoking
    /// thread is suspended until ownership can be gained.
    /// Multiple (nested) calls to this method are not supported and lead to undefined behavior.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    ALIB_DLL
    void
    AcquireShared( ALIB_DBG_TAKE_CI );

    /// Tries to acquire this lock.
    /// Multiple (nested) calls to this method or method #AcquireShared from within the same thread
    /// are not supported and lead to undefined behavior.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    /// @return \c true if the lock was not acquired by a different thread and thus, this
    ///         call was successful. \c false otherwise.
    ALIB_DLL
    [[nodiscard]]
    bool        TryAcquireShared( ALIB_DBG_TAKE_CI );

    /// Releases shared ownership of this object.
    /// Invoking this method on an object that is not "shared acquired" by this thread constitutes
    /// undefined behavior.<br>
    /// In debug-compilations the overall sum (of any thread) of shared acquirements and releases
    /// is counted, and an \ref alib_mod_assert "error is raised" if more releases than
    /// acquisitions are performed.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    ALIB_DLL
    void        ReleaseShared( ALIB_DBG_TAKE_CI );
  #else
                   void  AcquireShared()                                    { mutex.lock_shared(); }
    [[nodiscard]]  bool  TryAcquireShared()                      { return mutex.try_lock_shared(); }
                   void  ReleaseShared()                                  { mutex.unlock_shared(); }
  #endif

};


} // namespace alib[threads]

/// Type alias in namespace \b alib.
using     SharedLock= threads::SharedLock;

} // namespace [alib]
#endif // !ALIB_SINGLE_THREADED

