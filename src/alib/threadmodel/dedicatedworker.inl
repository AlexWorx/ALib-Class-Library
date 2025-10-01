//==================================================================================================
/// \file
/// This header-file is part of module \alib_threadmodel of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace threadmodel {
class DWManager;

//==================================================================================================
/// \attention This class belongs to module \alib_threadmodel, which is not in a stable and
///            consistent state, yet.
///            Also this type is considered experimental.
///
/// This \alib{singletons;Singleton;singleton-class} manages worker threads of type
/// \alib{threadmodel;DedicatedWorker}.
/// Such threads are started by adding them to the singleton instance of this type and are stopped
/// with removal.
///
/// \b %DedicatedWorkers use this type's \b PoolAllocator to create and dispose \alib{threadmodel;Job}
/// objects and derived types are allowed to allocate resources in its \b %MonoAllocator.
//==================================================================================================
class DWManager :  public  singletons::Singleton<DWManager>
                ,  public  Lock

{
    friend class singletons::Singleton<DWManager>;

  protected:
    /// Mono allocator. Used for commands and by <b>DedicatedWorker</b>s.
    MonoAllocator                           ma;

    /// Pool allocator. Used for command objects.
    PoolAllocator                           pool;

    /// The list of workers.
    List<MonoAllocator, DedicatedWorker*>   workers;

  private:
    /// Constructor.
    ALIB_DLL                DWManager();

  public:
    /// Allows access to the singleton instances' allocator.
    /// @return The monotonic allocator.
    MonoAllocator&          GetAllocator()                                            { return ma; }

    /// Allows access to the singleton instances' pool allocator.
    /// @return The pool allocator.
    PoolAllocator&          GetPoolAllocator()                                      { return pool; }

    /// Adds and starts a worker.
    /// @param thread The thread to add.
    ALIB_DLL void           Add( DedicatedWorker& thread );

    /// Remove a previously added worker.
    /// The thread is stopped by invoking \alib{threadmodel;DedicatedWorker::ScheduleStop} using the given
    /// priority. It is waited until it exits, and finally the thread is joined.<br>
    /// With debug-compilations, an \alib warning is raised every second if the thread
    /// does not stop.
    /// @param thread       The thread to remove.
    /// @param stopPriority The priority passed to <b>DedicatedWorker::ScheduleStop</b>.
    ///                     Use \b Lowest (the default) to ensure that all other commands are duly
    ///                     executed before exiting.
    /// @return \c true if the thread was found and removed, \c false otherwise.
    ALIB_DLL bool           Remove( DedicatedWorker& thread,
                                    Priority         stopPriority= Priority::Lowest );


    #if DOXYGEN
    /// Waits until all threads are idle.
    /// @param timeout      The maximum time to wait.
    /// @param dbgWarnAfter The time after which a warning message will be printed to the
    ///                     debug log if the timeout was reached.<br>
    ///                     This parameter is only available in debug-compilations and thus
    ///                     should be passed using macro \ref ALIB_DBG.
    /// @return \c true if all threads are idle, \c false otherwise.
    ALIB_DLL
    bool                    WaitForAllIdle( Ticks::Duration timeout,
                                            Ticks::Duration dbgWarnAfter );
    #else
      ALIB_DLL bool WaitForAllIdle( Ticks::Duration timeout
                         ALIB_DBG(, Ticks::Duration dbgWarnAfter) );
      bool WaitForAllIdle( Ticks::Duration::TDuration timeout
                ALIB_DBG(, Ticks::Duration::TDuration dbgWarnAfter)  )
                { return WaitForAllIdle( Ticks::Duration(timeout)
                              ALIB_DBG(, Ticks::Duration(dbgWarnAfter) ) ); }

    #endif


    /// Stops execution and terminates all workers by invoking
    /// \alib{threadmodel;DedicatedWorker::ScheduleStop}.<br>
    /// After this method has been invoked, no further commands should be scheduled, even if
    /// the given priority equals \b Lowest. This <b>should be assured</b> by the using code.
    ///
    /// With debug-compilations, an \alib warning is raised every second until all managed
    /// threads stopped.
    /// @param stopPriority The priority passed to <b>DedicatedWorker::ScheduleStop</b>.
    ///                     Use \b Lowest (the default) to ensure that all other commands are duly
    ///                     executed before exiting.
    ALIB_DLL void           RemoveAll(Priority stopPriority= Priority::Lowest);

};  // class DWManager

//==================================================================================================
/// \attention
///   This class is part of \b experimental module \alib_threadmodel and thus is experimental
///   in respect to interface, functionality, and documentation.
///
///
/// This class implements a worker thread that receives jobs from a private queue.
/// Jobs can have a different \alib{threadmodel;Priority}, and thus may not be executed in the
/// order of insertion.
/// This concept comprises a very simple scheduling approach and should not be overused.
/// In the end, any multithreading software has to be designed in a way that a job queue never
/// grows to a high level, so that all jobs will be processed in a maximum time.
///
/// This class is virtual, and it is designed for inheritance: Using software has to derive custom
/// types that provide the custom functionality.
///
/// Jobs are represented with the protected type \alib{threadmodel;Job}.
///
/// Derived custom types need to duly fulfill a certain contract of interaction.
/// In short, the following rules are given:
/// 1. Derived types define jobs by exposing a type derived from virtual class
///    \alib{threadmodel;Job}.
///    Preferably, the derived types are designed to be public inner types because they
///    usually explicitly belong to the scope of an \b %DedicatedWorker.<br>
///    A \b %Job - besides its inherited type-ID - contains input data to the \b %DedicatedWorker
///    as well as output data to the requesting thread.
///    (Both are optional, as can be seen, for example, with the very simple built-in type
///    \alib{threadmodel::DedicatedWorker;JobTrigger} that has neither input nor output data.)
///    <br><br>
/// 2. The derived type exposes interface methods according to the different jobs available
///    to be scheduled.
///    Here, the \b %Job is constructed and returned to the caller, usually as a reference
///    in its derived form.<br>
///    Thus, the worker has knowledge of what is to be expected when extracting a job from its
///    queue, and with that is able to safely cast the virtual type "up" to the according
///    derived type.
///    <br><br>
/// 3. The \b %Jobs returned by the interface (usually) contain some sort of thread-safe acknowledge
///    mechanics, for example, a \alib{threads;Promise} or a \alib{threads;Condition}
///    that a caller can decide to wait on.
///    <br><br>
/// 4. Disposal of \b %Job instances has to be explicitly performed by the thread that schedules
///    a job. There are two methods provided for doing it.
///    The first is #DeleteJob, which performs instant deletion. This can be called \b after
///    the job was processed. The latter has to be awaited, as described in the previous paragraph.
///    The second is #DeleteJobDeferred, which covers the case that the caller decides not to wait
///    for an acknowledgment.
///    Here, the instance must not be deleted because the \b %DedicatedWorker will access it when
///    it is extracted from the queue (or already is working on it).
///    Thus, this second version schedules a deletion job and lets the worker do it.<br>
///    The deletion job has a low priority of
///    \alib{threadmodel;DedicatedWorker::Priority;DeferredDeletion}. Jobs scheduled with an
///    even lower priority must not be deferred-deleted.
///    \note A similar concept of deferred-deletion is provided with sibling type
///          \alib{threadmodel;ThreadPool}.
///          There, deferred-deletion has a huge negative impact on performance of the pool.
///          With type \b %DedicatedWorker, the negative impact of deferred deletion is negligible.
///    <br><br>
///    
/// 5. The custom types \b %Job types have to likewise fulfill a few contractual rules.
///    Those are given with the type's \alib{threadmodel;Job;reference documentation}.
///
/// ### Starting and Stopping: ###
/// The class inherits type \alib{threads;Thread} as a protected base type. With that, the
/// conventional methods to start and stop a thread are hidden.<br>
/// Starting and stopping a \b %DedicatedWorker is instead performed by adding, respectively
/// removing an instance of this type to singleton class \alib{threadmodel;DWManager}.
/// 
/// ### Triggering: ###
/// This type implements abstract interface \alib{threadmodel;Triggered}.
/// If this is considered useful by a derived type, then three things have to be performed:
/// - The parameterless job \alib{threadmodel::DedicatedWorker;JobTrigger} has to be processed
///   in the overridden method #process.
/// - Field #triggerDuration has to be adjusted as required, or as an alternative, the method
///   #triggerPeriod has to be overridden. The latter might allow more flexibility to
///   adjust the trigger time dependent on certain custom states.
/// - The instance of the custom \b %DedicatedWorker has to be registered by calling method
///   \alib{threadmodel;Trigger::Add}.
//==================================================================================================
class DedicatedWorker : protected alib::Thread
                      , protected Triggered
                      , protected TCondition<DedicatedWorker>
{
    friend class DWManager;
    friend struct threads::TCondition<DedicatedWorker>;

  
  protected:
    friend class lang::Owner<DedicatedWorker&>; /// needed as we inherit TCondition

    //==============================================================================================
    // Fields
    //==============================================================================================

    /// Reference to \b %DWManager instance.
    DWManager&			manager;

    /// Statistical information: Point in time of last job execution.
    /// In case no job was executed, yet, this is the creation time of the object.
    alib::Ticks         statLastJobExecution;

    /// Flag which is set when the stop-job was scheduled.
    bool                stopJobPushed                                                       = false;

    /// Flag which is set when the stop-job was executed.
    bool                stopJobExecuted                                                     = false;

    /// If this \b %DedicatedWorker is (in addition) attached to a \alib{threadmodel;DWManager} as a
    /// triggered object, this duration is returned by overridden method
    /// \alib{threadmodel;Triggered::triggerPeriod} to determine the interval between
    /// scheduling two trigger jobs.<br>
    /// Defaults to one second, which usually is changed by a derived type.
    Ticks::Duration     triggerDuration                           = Ticks::Duration::FromSeconds(1);

    //==============================================================================================
    // The queue
    //==============================================================================================
    /// Container element of the queue.
    struct QueueElement
    {
        Job*       job;      ///< The job containing the pool-allocated shared data.
        Priority   priority; ///< The job's priority.
        bool       keepJob;  ///< Flag which indicates whether the job should be deleted after
                             ///< execution.
    };

    /// The queue of jobs.
    /// This is a simple list, instead of a 'real' priority queue.
    /// This design decision was taken because there should never be too many jobs queued
    /// and the naive iteration is more efficient than using a 'real' priority queue type.
    List<HeapAllocator, QueueElement>  queue;

    /// The current number of jobs in the queue.
    int                                length;

    /// Mandatory method needed and invoked by templated base type \alib{threads;TCondition}.
    /// @return \c true if field #queue is not empty, \c false otherwise.
    bool        isConditionMet()                                              { return length > 0; }

    /// Pushes the given \p{jobInfo} into the priority queue that this class implements.
    /// When invoked, the thread-manager as well as this instance are both locked.
    /// @param jobInfo  The job, the priority, and a flag if this job is to be deleted
    ///                 automatically.
	ALIB_DLL
    void        pushAndRelease(QueueElement&& jobInfo);

    /// Moves the job of highest priority out of the queue.
    /// Blocks the thread until a job is available.
    /// @return The job with the highest priority.
    std::pair<Job*, bool>      pop();

    //==============================================================================================
    // Inner Job types
    //==============================================================================================
    /// The stop job sent by method #ScheduleStop.
    struct JobStop          : Job
    {
        /// Constructor.
        JobStop()                                                      : Job( typeid(JobStop) )   {}
    };

    /// The job sent by method #DeleteJobDeferred.
    struct JobDeleter : Job
    {
        /// The job to be deleted.
        Job*   JobToDelete;

        /// Constructor.
        /// @param job    The job that is scheduled to be deleted.
        JobDeleter( Job* job )
        : Job( typeid(JobDeleter) )
        , JobToDelete( job )                                                                      {}

        /// Overrides the parent function as necessary.
        /// @return The sizeof this derived type.
        virtual size_t  SizeOf()                            override  { return sizeof(JobDeleter); }
    };

    /// The job sent if (optional) trigger-interface \alib{threadmodel;Triggered::trigger}
    /// is invoked.
    struct JobTrigger : Job
    {
        /// Constructor.
        JobTrigger()                                                  : Job( typeid(JobTrigger) ) {}
    };

    /// Pushes a custom job into the priority queue.<br>
    /// This method is protected because derived types should provide dedicated "speaking"
    /// interfaces for scheduling jobs.
    /// Those are typically short inline methods, which/ are optimized out by C++ compilers.
    /// @tparam TJob The job type as well as the job's shared data type.
    /// @tparam TArgs       Types of the variadic arguments \p{args} that construct \p{TJob}.
    /// @param  priority    The priority of the job. 
    /// @param  keepJob     Denotes whether the job should be deleted after execution or not.
    /// @param  args        Variadic arguments forwarded to the constructor of \p{TJob}.
    /// @return The shared data. Deletion of the object is the responsibility of the caller and
    ///         is to be done by either invoking \alib{threadmodel:DedicatedWorker;DeleteJob}
    ///         or \alib{threadmodel:DedicatedWorker;DeleteJobDeferred}.
    template<typename TJob, typename... TArgs>
    [[nodiscard]]
    TJob&        schedule( Priority   priority,
                           bool       keepJob,
                           TArgs&&... args      )
    {
        manager.Acquire(ALIB_CALLER_PRUNED);
        TJob* job= manager.GetPoolAllocator()().New<TJob>( std::forward<TArgs>(args)... );
        manager.Release(ALIB_CALLER_PRUNED);
        ALIB_ASSERT_ERROR( job->SizeOf()==sizeof(TJob), "MGTHR",
            "Error in DedicatedWorker::schedule: Job size mismatch. Expected {} "
            "while virtual method SizeOf returns {}.\n"
            "Override this method for job-type <{}>",
            sizeof(TJob), job->SizeOf(), &typeid(*job) )

        ALIB_ASSERT_ERROR(     GetState() == State::Started
                            || GetState() == State::Running, "MGTHR",
            "Error in DedicatedWorker::schedule: Job pushed while this thread was not started, yet. "
            "State: ", GetState() )

        pushAndRelease( {job, priority, keepJob} );
        return *job;
    }

    /// Pushes a custom job into the priority queue.<br>
    /// The job is returned to the caller to be able to await results.
    /// It is the responsibility of the caller to pass the job to either method
    /// #DeleteJob or #DeleteJobDeferred for disposal.
    /// @tparam TJob     The job type to create and schedule.
    /// @tparam TArgs    Types of the variadic arguments \p{args} that construct \p{TJob}.
    /// @param  priority The priority of the job.
    /// @param  args     Variadic arguments forwarded to the constructor of \p{TJob}.
    /// @return The scheduled job.
    template<typename TJob, typename... TArgs>
    TJob&           Schedule( Priority priority, TArgs&&... args  )
    { return schedule<TJob, TArgs...>( priority, true,  std::forward<TArgs>(args)... ); }

    /// Pushes a custom job into the priority queue.
    /// In contrast to the sibling method #Schedule, the job is not returned by this method.
    /// Instead, it is scheduled for automatic disposal after execution.
    /// @tparam TJob     The job type to create and schedule.
    /// @tparam TArgs    Types of the variadic arguments \p{args} that construct \p{TJob}.
    /// @param  priority The priority of the job.
    /// @param  args     Variadic arguments forwarded to the constructor of \p{TJob}.
    template<typename TJob, typename... TArgs>
    void           ScheduleVoid( Priority priority, TArgs&&... args  )
    { (void) schedule<TJob, TArgs...>( priority,  false, std::forward<TArgs>(args)... ); }


    //==============================================================================================
    // Triggered interface implementation
    //==============================================================================================
    /// Return the sleep time, between two trigger events.
    /// Precisely, this method is called after #trigger has been executed and defines the
    /// next sleep time.
    /// @return The desired sleep time between two trigger events.
    virtual Ticks::Duration triggerPeriod()                     override { return triggerDuration; }

    /// Schedules \alib{threadmodel::DedicatedWorker;JobTrigger} need to implement this function and
    /// perform their trigger actions here.
    virtual void            trigger()  override
    { (void) schedule<JobTrigger>( Priority::Low, false ); }

    //==============================================================================================
    // Protected virtual execution methods
    //==============================================================================================
    /// This implementation of method \alib{threads;Thread::Run} constitutes a very simple loop
    /// that waits for jobs in the #queue and passes them to likewise virtual method #process.
    /// The (only) condition to continuing the loop is that the flag #stopJobExecuted is \c false.
    /// This flag will be set by the internal job type \alib{threadmodel::DedicatedWorker;JobStop}
    /// which is pushed with the method #ScheduleStop.
    ALIB_DLL
    virtual void            Run()                                                          override;


    /// This virtual method is called during thread execution (method #Run) for each job
    /// extracted from the internal job-queue.<br>
    /// While this default-implementation is empty, internally, before a call to this method,
    /// the following jobs are already detected and processed:
    /// - \alib{threadmodel::DedicatedWorker;JobStop}, which signals flag #stopJobExecuted, and
    ///   this way lets calling method #Run leave its loop, and
    /// - \alib{threadmodel::DedicatedWorker;JobDeleter}, which deletes the given other job.
    ///   (See method #DeleteJobDeferred.)
    ///
    /// A derived type may decide to call this parent method before or after checking for its own
    /// jobs. If the call returns \c true, then the job was processed.
    ///
    /// It is also allowed to implement a custom processing, for example, with \b %JobStop and thus
    /// not to call this version for the internal jobs covered by the custom version.
    ///
    /// It is important that overridden versions do return the \c true if the job was processed
    /// and \c false if not.
    /// @param  vjob  The job to process.
    /// @return \c true if the job was processed, \c false if not.
    virtual bool            process(Job& vjob)                         { (void)vjob; return false; }

  public:
    #if ALIB_DEBUG
    /// The maximum number of jobs in the queue at any time.
    /// Available only with debug-compilations.
    int                     DbgMaxQueuelength;
    #endif


    //==============================================================================================
    // Construction/Destruction
    //==============================================================================================
    /// Constructor taking a thread name that is passed to parent class \alib{threads;Thread}.
    /// @param threadName    The name of this thread.
    DedicatedWorker(const character* threadName)
    : Thread       ( threadName )
#if ALIB_STRINGS
    , Triggered    ( threadName )
#endif
    , TCondition   ( ALIB_DBG(threadName) )
    , manager      { DWManager::GetSingleton() }
    , statLastJobExecution(lang::Initialization::Nulled )
    , length       {0}
    ALIB_DBG(,DbgMaxQueuelength{0})
    {}

    /// Destructor.
    ~DedicatedWorker()                                                                      override
    {
        #if ALIB_STRINGS
            ALIB_ASSERT_WARNING( Load() == 0, "MGTHR",
             "DedicatedWorker \"{}\" destructed, while job-queue is not empty.", GetName()  )
        #else
            ALIB_ASSERT_WARNING( Load() == 0, "MGTHR",
             "DedicatedWorker destructed, while job-queue is not empty.")
        #endif
    }

    using Thread::GetName;
    
    //==============================================================================================
    // Load/Stop
    //==============================================================================================
    /// Returns the current number of jobs in the queue.
    /// @return The number of jobs to process, including any currently processed one.
    int             Load()                                                  const { return length; }

    /// Returns the state of the internal flag, which is set with the invocation of #ScheduleStop.
    /// @return \c true, if the #ScheduleStop was called, \c false otherwise.
    bool            StopIsScheduled()                                      { return stopJobPushed; }

    /// Returns the state of the internal flag, which is set with the execution of #ScheduleStop.
    /// @return \c true, if the #ScheduleStop was processed, \c false otherwise.
    bool            StopIsExecuted()                                     { return stopJobExecuted; }


    //==============================================================================================
    // Job Interface
    //==============================================================================================
    /// Schedules a stop job into this thread's job queue.
    /// When this job is processed from the queue, this thread will exit.
    /// @param priority The priority of the job. Use \b Lowest if it is assured that no
    ///                 other jobs will be pushed.
    void        ScheduleStop(Priority priority)
    {
        stopJobPushed= true;
        (void) schedule<JobStop>( priority, false );
    }

    /// Deletes a data object previously received via method #schedule, one of its siblings, or
    /// scheduling methods of derived types.
    ///
    /// The latter might have names that do not contain the term "schedule" but still internally
    /// create and return data objects. Any object returned needs to be deleted.
    ///
    /// \attention
    ///   Deletion of data objects must not be done by the caller of a schedule-method before
    ///   the job is processed by this \b %DedicatedWorker.
    ///   For example, if a derived type's interface returns an job instance of type
    ///   \alib{threadmodel;JPromise}, then such a result must only be deleted once the promise is
    ///   fulfilled.
    ///   <p>
    ///   This can be cumbersome in case the calling thread is just not interested in
    ///   the result. For this case, the alternative method # DeleteJobDeferred is given.
    ///
    /// @param  job  The job returned when scheduling a command.
    void        DeleteJob(Job& job)
    {
        ALIB_LOCK_WITH(manager)
            auto size= job.SizeOf();
            job.~Job();
            manager.GetPoolAllocator().free(&job, size);
    }

    /// Same as #DeleteJob but schedules the deletion to be performed. Scheduling is
    /// done with \alib{threadmodel;Priority;Priority::DeferredDeletion}.<br>
    /// This assures that the job deletion is performed \b after the job execution.
    ///
    /// This method is useful when the thread that schedules a job with this \b %DedicatedWorker is
    /// not interested in the result, i.e., does not perform an asynchronous wait.
    /// Custom jobs where this is rather usually the case, should be exposed in two versions,
    /// one that returns a result and another that does not.
    /// With the second, the derived \b %DedicatedWorker would delete the given shared data
    /// with processing the job.
    /// If this is offered, then this method does not need to be called (and cannot be called
    /// because the caller does not receive any data).
    /// @param job    The job object to delete.
    void        DeleteJobDeferred(Job& job)
    { (void) schedule<JobDeleter>( Priority::DeferredDeletion, false, &job ); }

}; // class DedicatedWorker


} // namespace alib[::threadmodel]

/// Type alias in namespace \b alib.
using      DedicatedWorker  = threadmodel::DedicatedWorker;

/// Type alias in namespace \b alib.
using      DWManager        = threadmodel::DWManager;

}  // namespace [alib]

#if ALIB_ENUMRECORDS
    ALIB_ENUMS_ASSIGN_RECORD(alib::threadmodel::Priority, ERSerializable)
#endif

