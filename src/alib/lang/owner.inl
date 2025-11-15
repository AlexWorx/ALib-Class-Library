//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace lang {

/// Ensures that an object of template type \p{TOwnable} is acquired and properly released when
/// unwinding the stack (\https{RAII idiom,https://en.cppreference.com/w/cpp/language/raii}).
/// This class is meant to be allocated only on the stack and not on the heap.
/// Therefore, the new operators as well as copy and move constructors and assignment operators
/// are declared private.
///
/// With debug-builds, the constructor expects caller source information parameters
/// of type \b CallerInfo.
/// Use macro \ref ALIB_CALLER_PRUNED to prune those with release-builds.
///
/// In the rather unusual case that it is a run-time decision whether this object should
/// carry an object to acquire or not, the template parameter \p{TOptional} may be set to \c true.
/// With that, pointers are accepted with construction and method #Set becomes available.
///
/// \see
///   - Several sibling types exist.
///     They are described and listed in chapter \ref alib_threads_locks_auto_owner of the
///     Programmer's Manual of module \alib_threads.
///   - Besides generic preprocessor macro \ref ALIB_OWN, several application-specific alias macros
///     exist.
///     For example, macros
///     - \ref ALIB_LOCK,
///     - \ref ALIB_LOCK_WITH,
///     - \ref ALIB_DCS, or
///     - \ref ALIB_DCS_WITH
///
///     provide a convenient and "speaking" way to use this class.
///   - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
///
/// @tparam TOwnable  The type to own.
///                   Requirements are to have methods \b %Acquire and \b %Release available.
/// @tparam TOptional If \c true, then checks is performed, whether the given #owned is
///                   \e nulled and thus not acquired and released.
///                   If \c false such checks are omitted.
template <typename TOwnable, bool TOptional= false>
class Owner
{
    ALIB_STACK_ALLOCATED_TYPE(Owner)

  public:
    /// The pointer type of the owned object.
    using OwnablePointer= std::remove_reference_t<TOwnable>*;

  protected:
    OwnablePointer    owned;  ///< The resource to acquire and release.
    #if ALIB_DEBUG
    CallerInfo    dbgCI;  ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release, dbgCI))
    void callRelease()                                                    { owned->Release(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release,      ))
    void callRelease()                                                         { owned->Release(); }
    #endif
  public:

    #if ALIB_DEBUG
    /// Constructor. Invokes Acquire() on the ownable.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    Owner( TOwnable& ownable, const CallerInfo& ci)
    : owned(&ownable)
    , dbgCI(ci)                                                           { ownable.Acquire( ci ); }

    /// Constructor taking a pointer to the ownable. This constructor is only available if
    /// the template parameter \p{TOptional} is \c true.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    Owner( OwnablePointer ownable, const CallerInfo& ci)
    requires TOptional
    : owned(ownable)
    , dbgCI(ci)                                  { if (ownable != nullptr) ownable->Acquire( ci ); }
    #else
        Owner( TOwnable&      ownable )                     : owned(&ownable) { ownable.Acquire(); }
        Owner( OwnablePointer ownable)
        requires TOptional
        : owned(ownable)                             { if (ownable != nullptr) ownable->Acquire(); }
    #endif

    /// Destructor. Releases the owner by invoking Release().
    ~Owner()                                             { if (!TOptional || owned) callRelease(); }

    /// Sets the ownable after construction. This method is only available if the template
    /// parameter \p{TOptional} is \c true. It must not be called if an object was given with
    /// construction already, and it must not be called twice. If done, an assertion is raised.
    /// @param ownable  The ownable to acquire.
    void Set(OwnablePointer ownable)
    requires TOptional {
        ALIB_ASSERT_ERROR(owned==nullptr, "LANG", "Owner already set.")
        (owned= ownable)->Acquire( ALIB_DBG(dbgCI) );
    }
}; //class Owner

/// Similar to class \alib{lang;Owner}, but calls method \b TryAcquire instead of \b Acquire.
/// The result is retrievable with method #IsOwning().
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of module
///      \alib_threads
/// @tparam TOwnable  The type to own.
///                   Requirements are to have methods \b %TryAcquire and \b %Release available.
/// @tparam TOptional If \c true, then checks is performed, whether the given #owned is
///                   \e nulled and thus not acquired and released.
///                   If \c false such checks are omitted.
template <typename TOwnable, bool TOptional= false>
class OwnerTry
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerTry)

  public:
    /// The pointer type of the owned object.
    using OwnablePointer= std::remove_reference_t<TOwnable>*;

  protected:
    OwnablePointer  owned;     ///< The resource to acquire and release.
    bool            isOwning;  ///< The result of the call to \b TryAcquire.
    #if ALIB_DEBUG
    CallerInfo  dbgCI;     ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release, dbgCI))
    void callRelease()                           { if (!TOptional || owned) owned->Release(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release,      ))
    void callRelease()                                { if (!TOptional || owned) owned->Release(); }

        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive, dbgCI))
    void callRelease()                  { if (!TOptional || owned) owned->ReleaseRecursive(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive,      ))
    void callRelease()                       { if (!TOptional || owned) owned->ReleaseRecursive(); }

    #endif
  public:

    #if ALIB_DEBUG
    /// Constructor. Invokes TryAcquire() on member #owned.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    OwnerTry( TOwnable& ownable, const CallerInfo& ci)
    : owned(&ownable)
    , dbgCI(ci)                                                { isOwning= ownable.TryAcquire(ci); }

    /// Constructor taking a pointer to the ownable. This constructor is only available if
    /// the template parameter \p{TOptional} is \c true.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    OwnerTry( OwnablePointer ownable, const CallerInfo& ci)
    requires TOptional
    : owned(ownable)
    , dbgCI(ci)                               { isOwning = !ownable  || ownable->TryAcquire( ci ); }
#else
        OwnerTry( TOwnable& ownable )
        : owned(&ownable)                                         { isOwning= ownable.TryAcquire(); }
        OwnerTry( OwnablePointer ownable)
        requires TOptional
        : owned(ownable)               { isOwning = !ownable  || ownable->TryAcquire(); }
    #endif


    /// Destructor. Invokes Release() on member #owned.
    ~OwnerTry()                            { if ((!TOptional || owned) && isOwning) callRelease(); }

    /// Sets the ownable after construction. This method is only available if the template
    /// parameter \p{TOptional} is \c true. It must not be called if an object was given with
    /// construction already, and it must not be called twice. If done, an assertion is raised.
    /// @param ownable  The ownable to acquire.
    /// @return \c true if the try to acquire the owner was successful, \c false if not.
    bool Set(OwnablePointer ownable)
    requires TOptional {
        ALIB_ASSERT_ERROR(owned==nullptr, "LANG", "Owner already set.")
        return isOwning= (owned= ownable)->TryAcquire( ALIB_DBG(dbgCI) );
    }

    /// @return \c true if the try to acquire the #owned with construction of this type was
    ///         successful. Furthermore, \c true is returned in the case that the template
    ///         parameter \p{TOptional} is \c true and no ownable was given with construction.
    ///         Otherwise, \c false is returned.
    bool        IsOwning()                                       const noexcept { return isOwning; }
}; //class OwnerTry

/// Similar to class \alib{lang;Owner}, but calls method \b TryAcquireTimed instead of \b Acquire.
/// The result is retrievable with method #IsOwning().
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of module
///      \alib_threads
/// @tparam TOwnable  The type to own.
///                   Requirements are to have methods \b %TryAcquireTimed and \b %Release
///                   available.
/// @tparam TOptional If \c true, then checks is performed, whether the given #owned is
///                   \e nulled and thus not acquired and released.
///                   If \c false such checks are omitted.
template <typename TOwnable, bool TOptional= false>
class OwnerTimed
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerTimed)

  public:
    /// The pointer type of the owned object.
    using OwnablePointer= std::remove_reference_t<TOwnable>*;

  protected:
    OwnablePointer  owned;      ///< The resource to acquire and release.
    bool            isOwning;   ///< The result of the call to \b TryAcquire.
    #if ALIB_DEBUG
    CallerInfo  dbgCI;      ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release, dbgCI))
    void callRelease()                                                    { owned->Release(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, Release,      ))
    void callRelease()                                                         { owned->Release(); }

        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive, dbgCI))
    void callRelease()                                           { owned->ReleaseRecursive(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive,      ))
    void callRelease()                                                { owned->ReleaseRecursive(); }

    #endif
  public:

    #if ALIB_DEBUG
    /// Constructor. Invokes TryAcquire() on member #owned.
    /// @param ownable     The ownable to acquire.
    /// @param time        The duration to wait for, or point in time to wait until.
    /// @param ci          Caller information.
    /// @tparam TTimeValue Type of time parameter accepted with construction and passed to
    ///                    method <b>TOwnable::TryAcquireTimed</b>.<br>
    ///                    Usually this is type \alib{time;Ticks} or
    ///                    \alib{time;TimePointBase::Duration;Ticks::Duration}.
    template<typename TTimeValue>
    OwnerTimed( TOwnable& ownable, const TTimeValue& time, const CallerInfo& ci)
    : owned(&ownable)
    , dbgCI(ci)                                    { isOwning= ownable.TryAcquireTimed( time, ci); }

    /// Constructor taking a pointer to the ownable. This constructor is only available if
    /// the template parameter \p{TOptional} is \c true.
    /// @param ownable     The ownable to acquire.
    /// @param time        The duration to wait for, or point in time to wait until.
    /// @param ci          Caller information.
    /// @tparam TTimeValue Type of time parameter accepted with construction and passed to
    ///                    method <b>TOwnable::TryAcquireTimed</b>.<br>
    ///                    Usually this is type \alib{time;Ticks} or
    ///                    \alib{time;TimePointBase::Duration;Ticks::Duration}.
    template<typename TTimeValue>
    requires TOptional
    OwnerTimed( OwnablePointer ownable, const TTimeValue& time, const CallerInfo& ci)
    : owned(ownable)
    , dbgCI(ci)                    { isOwning = !ownable  || ownable->TryAcquireTimed( time, ci ); }
    #else
        template<typename TTimeValue>
        OwnerTimed( TOwnable& ownable, const TTimeValue& time )
        : owned(&ownable)                              { isOwning= ownable.TryAcquireTimed( time ); }

        template<typename TTimeValue>
        requires TOptional
        OwnerTimed( OwnablePointer ownable, const TTimeValue& time)
        : owned(ownable)    { isOwning = !ownable  || ownable->TryAcquireTimed(time); }
    #endif


    /// Destructor. Invokes Release() on member #owned.
    ~OwnerTimed()                          { if ((!TOptional || owned) && isOwning) callRelease(); }

    /// Sets the ownable after construction. This method is only available if the template
    /// parameter \p{TOptional} is \c true. It must not be called if an object was given with
    /// construction already, and it must not be called twice. If done, an assertion is raised.
    /// @param  ownable    The ownable to acquire.
    /// @param  time       The duration to wait for, or point in time to wait until.
    /// @tparam TTimeValue Type of time parameter accepted with construction and passed to
    ///                    method <b>TOwnable::TryAcquireTimed</b>.<br>
    ///                    Usually this is type \alib{time;Ticks} or
    ///                    \alib{time;TimePointBase::Duration;Ticks::Duration}.
    /// @return \c true if the try to acquire the owner was successful, \c false if not.
    template<typename TTimeValue>
    requires TOptional
    bool Set(OwnablePointer ownable, const TTimeValue& time) {
        ALIB_ASSERT_ERROR(owned==nullptr, "LANG", "Owner already set.")
        return isOwning= (owned= ownable)->TryAcquireTimed( time  ALIB_DBG(, dbgCI) );
    }

    /// @return \c true if the try to acquire the #owned with construction of this type was
    ///         successful. Furthermore, \c true is returned in the case that the template
    ///         parameter \p{TOptional} is \c true and no ownable was given with construction.
    ///         Otherwise, \c false is returned.
    bool        IsOwning()                                       const noexcept { return isOwning; }
}; //class OwnerTimed


/// Similar to class \alib{lang;Owner}, but calls methods \b AcquireRecursive and
/// \b ReleaseRecursive.
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of module
///      \alib_threads
/// @tparam TOwnable  The type to own.
///                   Requirements are to have methods \b %AcquireRecursive and \b %ReleaseRecursive
///                   available.
/// @tparam TOptional If \c true, then checks is performed, whether the given #owned is
///                   \e nulled and thus not acquired and released.
///                   If \c false such checks are omitted.
template <typename TOwnable, bool TOptional= false>
class OwnerRecursive
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerRecursive)

  public:
    /// The pointer type of the owned object.
    using OwnablePointer= std::remove_reference_t<TOwnable>*;

  protected:
    OwnablePointer  owned;    ///< The resource to acquire and release.
    #if ALIB_DEBUG
    CallerInfo  dbgCI;    ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive,dbgCI))
    void callRelease()                                           { owned->ReleaseRecursive(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseRecursive, ))
    void callRelease()                                                { owned->ReleaseRecursive(); }
    #endif
  public:

    #if ALIB_DEBUG
    /// Constructor. Invokes AcquireRecursive() on member #owned.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    OwnerRecursive( TOwnable& ownable, const CallerInfo& ci)
    : owned(&ownable)
    , dbgCI(ci)                                                    { ownable.AcquireRecursive(ci); }

    /// Constructor taking a pointer to the ownable. This constructor is only available if
    /// the template parameter \p{TOptional} is \c true.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    OwnerRecursive( OwnablePointer ownable, const CallerInfo& ci)
    requires TOptional
    : owned(ownable)
    , dbgCI(ci)                         { if (ownable != nullptr) ownable->AcquireRecursive( ci ); }
    #else
        OwnerRecursive( TOwnable& ownable ) : owned(&ownable)       { ownable.AcquireRecursive(); }

        OwnerRecursive( OwnablePointer ownable)
        requires TOptional
        : owned(ownable)                { if (ownable != nullptr) ownable->AcquireRecursive(); }
    #endif

    /// Destructor. Invokes ReleaseRecursive() on member #owned.
    ~OwnerRecursive()                                    { if (!TOptional || owned) callRelease(); }

    /// Sets the ownable after construction. This method is only available if the template
    /// parameter \p{TOptional} is \c true. It must not be called if an object was given with
    /// construction already, and it must not be called twice. If done, an assertion is raised.
    /// @param ownable  The ownable to acquire.
    void Set(OwnablePointer ownable)
    requires TOptional {
        ALIB_ASSERT_ERROR(owned==nullptr, "LANG", "Owner already set.")
        (owned= ownable)->AcquireRecursive( ALIB_DBG(dbgCI) );
    }
}; //class OwnerRecursive

/// Similar to class \alib{lang;Owner}, but calls methods \b AcquireShared and
/// \b ReleaseShared.
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of module
///      \alib_threads
/// @tparam TOwnable  The type to own.
///                   Requirements are to have methods \b %AcquireShared and \b %ReleaseShared
///                   available.
/// @tparam TOptional If \c true, then checks is performed, whether the given #owned is
///                   \e nulled and thus not acquired and released.
///                   If \c false such checks are omitted.
template <typename TOwnable, bool TOptional= false>
class OwnerShared
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerShared)

  public:
    /// The pointer type of the owned object.
    using OwnablePointer= std::remove_reference_t<TOwnable>*;

  protected:
    OwnablePointer    owned;    ///< The resource to acquire and release.
    #if ALIB_DEBUG
    CallerInfo    dbgCI;    ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared, dbgCI))
    void callRelease()                                        const { owned->ReleaseShared(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared,     ))
    void callRelease()                                             const { owned->ReleaseShared(); }
    #endif
  public:

    #if ALIB_DEBUG
    /// Constructor. Invokes AcquireShared() on member #owned.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    OwnerShared( TOwnable& ownable, const CallerInfo& ci)
    : owned(&ownable)
    , dbgCI(ci)                                                       { ownable.AcquireShared(ci); }

    /// Constructor taking a pointer to the ownable. This constructor is only available if
    /// the template parameter \p{TOptional} is \c true.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    OwnerShared( OwnablePointer ownable, const CallerInfo& ci)
    requires TOptional
    : owned(ownable)
    , dbgCI(ci)                            { if (ownable != nullptr) ownable->AcquireShared( ci ); }
    #else
        OwnerShared( TOwnable& ownable ) : owned(&ownable)             { ownable.AcquireShared(); }
        OwnerShared( OwnablePointer ownable )
        requires TOptional
        : owned(ownable)                       { if (ownable != nullptr) ownable->AcquireShared(); }
    #endif


    /// Destructor. Invokes ReleaseShared() on member #owned.
    ~OwnerShared()                                       { if (!TOptional || owned) callRelease(); }

    /// Sets the ownable after construction. This method is only available if the template
    /// parameter \p{TOptional} is \c true. It must not be called if an object was given with
    /// construction already, and it must not be called twice. If done, an assertion is raised.
    /// @param ownable  The ownable to acquire.
    void Set(OwnablePointer ownable)
    requires TOptional {
        ALIB_ASSERT_ERROR(owned==nullptr, "LANG", "Owner already set.")
        (owned= ownable)->AcquireShared( ALIB_DBG(dbgCI) );
    }
}; //class OwnerShared

/// Similar to class \alib{lang;Owner}, but calls method \b TryAcquireShared instead of \b Acquire
/// and \b ReleaseShared instead of \b Release.
/// The result is retrievable with the method #IsOwning().
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of module
///      \alib_threads
/// @tparam TOwnable  The type to own.
///                   Requirements are to have methods \b %TryAcquireShared and \b %ReleaseShared
///                   available.
/// @tparam TOptional If \c true, then checks is performed, whether the given #owned is
///                   \e nulled and thus not acquired and released.
///                   If \c false such checks are omitted.
template <typename TOwnable, bool TOptional= false>
class OwnerTryShared
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerTryShared)

  public:
    /// The pointer type of the owned object.
    using OwnablePointer= std::remove_reference_t<TOwnable>*;

  protected:
    OwnablePointer  owned;      ///< The resource to acquire and release.
    bool            isOwning;   ///< The result of the call to \b TryAcquire.
    #if ALIB_DEBUG
    CallerInfo  dbgCI;      ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared, dbgCI))
    void callRelease()                                              { owned->ReleaseShared(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared,      ))
    void callRelease()                                                   { owned->ReleaseShared(); }
    #endif
  public:

    #if ALIB_DEBUG
    /// Constructor. Invokes TryAcquire() on member #owned.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    OwnerTryShared( TOwnable& ownable, const CallerInfo& ci)
    : owned(&ownable)
    , dbgCI(ci)                                          { isOwning= ownable.TryAcquireShared(ci); }

    /// Constructor taking a pointer to the ownable. This constructor is only available if
    /// the template parameter \p{TOptional} is \c true.
    /// @param ownable  The ownable to acquire.
    /// @param ci       Caller information.
    OwnerTryShared( OwnablePointer ownable, const CallerInfo& ci)
    requires TOptional
    : owned(ownable)
    , dbgCI(ci)                         { isOwning = !ownable  || ownable->TryAcquireShared( ci ); }
    #else
        OwnerTryShared( TOwnable& ownable )
        : owned(&ownable)                                 { isOwning= ownable.TryAcquireShared(); }

        OwnerTryShared( OwnablePointer ownable)
        requires TOptional
        : owned(ownable)         { isOwning = !ownable  || ownable->TryAcquireShared(); }
    #endif


    /// Destructor. Invokes Release() on member #owned.
    ~OwnerTryShared()                     { if ((!TOptional || owned) && isOwning)  callRelease(); }

    /// @return \c true if the try to acquire the #owned with construction of this type was
    ///         successful. Furthermore, \c true is returned in the case that the template
    ///         parameter \p{TOptional} is \c true and no ownable was given with construction.
    ///         Otherwise, \c false is returned.
    bool        IsOwning()                                       const noexcept { return isOwning; }

    /// Sets the ownable after construction. This method is only available if the template
    /// parameter \p{TOptional} is \c true. It must not be called if an object was given with
    /// construction already, and it must not be called twice. If done, an assertion is raised.
    /// @param ownable  The ownable to acquire.
    /// @return \c true if the try to acquire the owner was successful, \c false if not.
    bool Set(OwnablePointer ownable)
    requires TOptional {
        ALIB_ASSERT_ERROR(owned==nullptr, "LANG", "Owner already set.")
        return isOwning= (owned= ownable)->TryAcquireShared( ALIB_DBG(dbgCI) );
    }

}; //class OwnerTryShared


/// Similar to class \alib{lang;Owner}, but calls method \b TryAcquireSharedTimed instead
/// of \b Acquire and \b ReleaseShared instead of \b Shared.
/// The result is retrievable with the method #IsOwning().
/// @see Chapter \ref alib_threads_locks_auto_owner of the Programmer's Manual of module
///      \alib_threads
/// @tparam TOwnable  The type to own.
///                   Requirements are to have methods \b %TryAcquireSharedTimed and
///                   \b %ReleaseShared available.
/// @tparam TOptional If \c true, then checks is performed, whether the given #owned is
///                   \e nulled and thus not acquired and released.
///                   If \c false such checks are omitted.
template <typename TOwnable, bool TOptional= false>
class OwnerSharedTimed
{
    ALIB_STACK_ALLOCATED_TYPE(OwnerSharedTimed)

  public:
    /// The pointer type of the owned object.
    using OwnablePointer= std::remove_reference_t<TOwnable>*;

  protected:
    OwnablePointer  owned;      ///< The resource to acquire and release.
    bool            isOwning;   ///< The result of the call to \b TryAcquire.
    #if ALIB_DEBUG
    CallerInfo  dbgCI;      ///< Caller information. Available only with debug-builds.
    #endif

    #if !DOXYGEN
        // choosing the right release method, with or without caller parameters.
        #if  ALIB_DEBUG
        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared, dbgCI))
    void callRelease()                                              { owned->ReleaseShared(dbgCI); }
        #endif

        template<typename TRequires= TOwnable>
        requires ( ALIB_HAS_METHOD(TRequires, ReleaseShared,      ))
    void callRelease()                                                   { owned->ReleaseShared(); }
    #endif
  public:

    #if ALIB_DEBUG
    /// Constructor. Invokes TryAcquire() on member #owned.
    /// @tparam TTimeValue Type of time parameter accepted with construction and passed to
    ///                    method <b>TOwnable::TryAcquireTimed</b>.<br>
    ///                    Usually this is type \alib{time;Ticks} or
    ///                    \alib{time;TimePointBase::Duration;Ticks::Duration}.
    /// @param  time       The duration to wait for, or point in time to wait until.
    /// @param  ownable    The ownable to acquire.
    /// @param ci          Caller information.
    template<typename TTimeValue>
    OwnerSharedTimed( TOwnable& ownable, const TTimeValue& time, const CallerInfo& ci)
    : owned(&ownable)
    , dbgCI(ci)                              { isOwning= ownable.TryAcquireSharedTimed( time, ci); }

    /// Constructor taking a pointer to the ownable. This constructor is only available if
    /// the template parameter \p{TOptional} is \c true.
    /// @tparam TTimeValue Type of time parameter accepted with construction and passed to
    ///                    method <b>TOwnable::TryAcquireTimed</b>.<br>
    ///                    Usually this is type \alib{time;Ticks} or
    ///                    \alib{time;TimePointBase::Duration;Ticks::Duration}.
    /// @param  time       The duration to wait for, or point in time to wait until.
    /// @param  ownable    The ownable to acquire.
    /// @param ci          Caller information.
    template<typename TTimeValue>
    requires TOptional
    OwnerSharedTimed( OwnablePointer ownable, const TTimeValue& time, const CallerInfo& ci)
    : owned(ownable)
    , dbgCI(ci)                { isOwning= !ownable  || ownable->TryAcquireSharedTimed( time, ci); }
    #else
        template<typename TTimeValue>
        OwnerSharedTimed( TOwnable& ownable, const TTimeValue& time )
        : owned(&ownable)                        { isOwning= ownable.TryAcquireSharedTimed( time ); }

        template<typename TTimeValue>
        requires TOptional
        OwnerSharedTimed( OwnablePointer ownable, const TTimeValue& time)
        : owned(ownable) { isOwning= !ownable || ownable->TryAcquireSharedTimed( time); }
    #endif


    /// Destructor. Invokes Release() on member #owned.
    ~OwnerSharedTimed()                    { if ((!TOptional || owned) && isOwning) callRelease(); }

    /// @return \c true if the try to acquire the #owned with construction of this type was
    ///         successful. Furthermore, \c true is returned in the case that the template
    ///         parameter \p{TOptional} is \c true and no ownable was given with construction.
    ///         Otherwise, \c false is returned.
    bool        IsOwning()                                       const noexcept { return isOwning; }

    /// Sets the ownable after construction. This method is only available if the template
    /// parameter \p{TOptional} is \c true. It must not be called if an object was given with
    /// construction already, and it must not be called twice. If done, an assertion is raised.
    /// @param  ownable    The ownable to acquire.
    /// @param  time       The duration to wait for, or point in time to wait until.
    /// @tparam TTimeValue Type of time parameter accepted with construction and passed to
    ///                    method <b>TOwnable::TryAcquireTimed</b>.<br>
    ///                    Usually this is type \alib{time;Ticks} or
    ///                    \alib{time;TimePointBase::Duration;Ticks::Duration}.
    /// @return \c true if the try to acquire the owner was successful, \c false if not.
    template<typename TTimeValue>
    bool Set(OwnablePointer ownable, const TTimeValue& time)
    requires TOptional {
        ALIB_ASSERT_ERROR(owned==nullptr, "LANG", "Owner already set.")
        return isOwning= (owned= ownable)->TryAcquireSharedTimed( time  ALIB_DBG(, dbgCI) );
    }

}; //class OwnerSharedTimed

} // namespace alib[::lang]

} // namespace [alib]
