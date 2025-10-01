//==================================================================================================
/// \file
/// This header-file is part of module \alib_threadmodel of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {

/// This is the namespace of \alibmod <b>"ThreadModel"</b>. Please refer to the
/// \ref alib_mod_threadmodel "Programmer's Manual" of this module for information
/// about how the types found in this namespace are used.
/// \attention This module is not in a stable and consistent state, yet.
///            Instead, it is considered experimental.
namespace threadmodel {

//#if !DOXYGEN
struct PWorker; 
//#endif
    
//==================================================================================================
/// \attention This class belongs to module \alib_threadmodel, which is not in a stable and
///            consistent state, yet.
///            Also this type is considered experimental.
///
/// This class provides a configurable and scalable thread pooling mechanism for managing
/// concurrent job execution.
/// It supports fixed or dynamically resizing thread pools to balance workload and optimize
/// resource usage, along with several key methods to schedule, manage, and monitor job execution.
///
/// ### Key Features
/// 1. <b>Thread Pool Management</b>:<br>
///    - Supports fixed-size and dynamically resizable thread pools.
///    - Dynamic resizing is handled through customizable parameters found in the struct
///      \alib{threadmodel::ThreadPool;ResizeStrategy}.
///    - Automatically adjusts the number of threads based on workload.
///
/// 2. <b>%Jobs</b>:<br>
///    The worker-threads managed by the pool are processing \alib{threadmodel;Job} objects,
///    which<br>
///    a) Carry the data needed for processing, and<br>
///    b) Optionally provide synchronization mechanics that allow a caller to wait until
///       a job was processed or periodically test for completion.
///
///    Allocation and life-cycle management of jobs is efficiently implemented leveraging
///    the mechanics provided by the module \alib_monomem.
///
/// 3. <b>Synchronization and %Job-Cleanup</b>:<br>
///    Provides \alib{threadmodel;ThreadPool::Sync;a method to synchronize} all worker threads
///    to ensure that processing continues only after a certain set of jobs has been executed.
///    <br><br>
///
/// 4. <b>Worker Management and Monitoring</b>:<br>
///    - Supports querying idle worker status and actively running threads.
///    - Debugging options to analyze job types and execution states.
///
/// @see Chapter \ref alib_thrmod_threadpool of this module's Programmer's Manual
///      provides a quick source code sample that demonstrates the use this class.
//==================================================================================================
class ThreadPool : protected TCondition<ThreadPool>
#if ALIB_DEBUG_CRITICAL_SECTIONS
, public lang::DbgCriticalSections::AssociatedLock
#endif

{
    friend struct PWorker;
    friend struct threads::TCondition<ThreadPool>;

  public:
    /// The set of management parameters that determine how a thread pool balances the number
    /// of worker threads.
    struct ResizeStrategy
    {
        friend class ThreadPool;

        /// The modes, fixed or automatic.
        enum class Modes
        {
            /// The number of threads is fixed.
            Fixed,

            /// The number of threads is increased when the load increases and
            /// decreased when the load decreases.
            Auto,
        };

        /// The mode of operation.
        Modes           Mode                                                           =Modes::Auto;

        /// The number of threads to create (or decrease to).
        /// If #Mode equals \b Fixed, this is used and all other parameters are ignored.
        /// If #Mode equals \b Auto, this field is ignored and the other parameters are used.
        int             FixedSize                               = ThreadPool::HardwareConcurrency();

        /// The maximum number of threads to create.
        int             WorkersMax                              = ThreadPool::HardwareConcurrency();

        /// The minimum number of threads to keep alive.
        int             WorkersMin                                                              = 0;

        /// A threshold in percent that determines at which overload factor the number of threads
        /// are increased.
        /// Defaults to \c 300%. For example, this means if the pool currently holds 10 threads,
        /// then new threads are created when the load increases to \c 30 unprocess jobs.
        int             IncreaseThreshold                                                     = 300;

        /// A threshold in percent that determines at which underload factor the number of threads
        /// are decreased.
        /// Defaults to \c 50%.
        /// For example, \c 30% this means that if 70% of the threads are idle the number of
        /// threads is decreased.
        int             DecreaseThreshold                                                      = 70;

        /// The duration of that the pool has to be overloaded before an increase of threads
        /// starts.
        /// Defaults to zero time interval.
        Ticks::Duration IncreaseSchedule             = Ticks::Duration::FromAbsoluteMilliseconds(0);

        /// The duration of that the pool has to be underloaded before a decrease of threads starts.
        /// Defaults to 500ms.
        Ticks::Duration DecreaseSchedule           = Ticks::Duration::FromAbsoluteMilliseconds(500);

    protected:

        /// Calculates the target size, depending on the parameters set in this struct and
        /// the actual values passed.
        /// @param  currentWorkers The number of threads currently in the pool.
        /// @param  idleWorkers    The size of the subset of threads currently idle.
        /// @param  load           The number of jobs currently queued.
        /// @param  lastChangeTime Time point of last increase or decrease.
        /// @return The new target size.
        inline
        int GetSize( int currentWorkers , int    idleWorkers, int load,
                     Ticks& lastChangeTime )
        {
            int target= currentWorkers;

            // fixed mode? -> nothing to do
                if (Mode == Modes::Fixed)  { target= FixedSize; }

            // check bounds
            else if( target < WorkersMin ) { target= WorkersMin; }
            else if( target > WorkersMax ) { target= WorkersMax; }

            // increase?
            else if(     lastChangeTime.Age() >= IncreaseSchedule
                && (load >= (currentWorkers * IncreaseThreshold / 100) )   )
            { target = (std::min)(WorkersMax, currentWorkers + 1); }

            // decrease?
            else if(     lastChangeTime.Age() >= DecreaseSchedule
                &&  (currentWorkers - idleWorkers) <= (currentWorkers * DecreaseThreshold / 100) )
            { target = (std::max)(WorkersMin, currentWorkers - 1); }

            // that's it
            if (target != currentWorkers)
                lastChangeTime.Reset();
            return target;
        }
    };


  protected:
    /// Mono allocator. Used for jobs and by PWorkers.
    MonoAllocator                           ma;

    /// Pool allocator. Used for job objects.
    PoolAllocator                           pool;

    /// The list of worker threads.
    HashSet<MonoAllocator, PWorker*>        workers;

    /// The counted number of currently of workers.
    int                                     ctdWorkers                                           =0;

    /// The counted number of currently idle workers.
    int                                     ctdIdle                                              =0;

    /// The point in time of the last change of thread size.
    Ticks                                   timeOfLastSizeChange;

    /// A number that is increased with the creation of new workers and added to their
    //  \alib{threads;Thread::GetName;thread name}.
    int                                     nextWorkerID                                         =0;

    #if ALIB_DEBUG
        /// Entry in the field #DbgKnownJobs.
        struct DbgKnownJobsEntry
        {
            const std::type_info*   TID;     ///< The job type.
            size_t                  JobSize; ///< The size of the job object.
            size_t                  Usage;   ///< Counter of scheduled jobs of this type.
        };

        /// Serves as template parameter \p{TValueDescriptor} of field #DbgKnownJobs.
        struct DbgKnownJobsVD : containers::TSubsetKeyDescriptor< DbgKnownJobsEntry,
                                                                  const std::type_info* >
        {
            /// Mandatory function to implement.
            /// @param entry The table entry to extract the key from.
            /// @return The key portion of the given \p{entry}.
            const std::type_info*   Key(DbgKnownJobsEntry& entry)        const { return entry.TID; }
        };

        /// Table of known job types and their sizes.
        HashTable<MonoAllocator,
                  DbgKnownJobsVD  >        DbgKnownJobs;
    #endif

    /// Special synchronization job. Pushed with #Sync and #DeleteJobDeferred.
    /// With the latter, field #JobToDelete will be given, otherwise this is nulled.
    struct JobSyncer : Job
    {
        /// Optionally a job to be deleted.
        Job*   JobToDelete;

        /// Constructor.
        /// @param job    The job that is scheduled to be deleted.
        JobSyncer( Job* job )
        : Job(typeid(JobSyncer))
        , JobToDelete(job)                                                                        {}

        /// Overrides the parent function as necessary.
        /// @return The sizeof this derived type.
        virtual size_t  SizeOf()                             override  { return sizeof(JobSyncer); }
    };

    //==============================================================================================
    // The queue
    //==============================================================================================
    /// Container element of the queue.
    struct QueueEntry
    {
        Job*       job;     ///< The job.
        bool       keep;    ///< If true, the job is not deleted by the processing worker,
                            ///< but has to be deleted by the caller.
    };

    /// The queue of jobs.
    List<PoolAllocator,
         QueueEntry,
         Recycling::None>           queue;


    /// The number of jobs in the queue.
    uinteger                        ctdStatJobsScheduled                                        {0};

    /// The number of jobs in the queue.
    int                             ctdOpenJobs                                                 {0};

    /// Mandatory method needed and invoked by templated base type \alib{threads;TCondition}.
    /// @return \c true if field #queue is not empty and either no sync-job is next or
    ///            all are idle.
    bool        isConditionMet()  { return       queue.IsNotEmpty()
                                             &&  (    queue.back().job->ID != typeid(JobSyncer)
                                                   || ctdIdle == ctdWorkers               ); }

    /// Pushes the given \p{cmd} into the priority queue that this class implements.
    /// @param entry The Job and the deletion flag.
    void        pushAndRelease(QueueEntry&& entry)
    {
        // insert before found
        queue.emplace_front( entry );
        ++ctdOpenJobs;
        ++ctdStatJobsScheduled;

        ALIB_MESSAGE( "MGTHR/QUEUE", "Pool({}}/{} -> {}/) Job({}) pushed",
            ctdOpenJobs, ctdStatJobsScheduled, ctdIdle, ctdWorkers, &entry.job->ID )

        ReleaseAndNotify(ALIB_CALLER_PRUNED);
    }

    /// Set if the last thread is terminated and #ctdWorkers goes to \c 0.
    /// This thread is joined by #Shutdown or when a new thread is added.
    PWorker*                lastThreadToJoin                                              = nullptr;

    /// Moves the job of highest priority out of the queue.
    /// Blocks the thread until a job is available.
    /// @param worker The instance that called this method.
    /// @return The job with the highest priority.
    QueueEntry              pop(PWorker* worker);

    /// Internal method that adds a thread. Must only be called when acquired.
    ALIB_DLL
    void                    addThread();

    /// Implementation of #Schedule and #ScheduleVoid.
    /// @tparam TJob    The job type to create and schedule.
    /// @tparam TArgs   Types of the variadic arguments \p{args} that construct \p{TJob}.
    /// @param  keepJob Denotes whether the job should be deleted after execution or not.
    /// @param  args    Variadic arguments forwarded to the constructor of \p{TJob}.
    /// @return The scheduled job.
    template<typename TJob, typename... TArgs>
    [[nodiscard]]
    TJob*           schedule( bool keepJob, TArgs&&... args  )
    {
        Acquire(ALIB_CALLER_PRUNED);
        // first check if this pool is active (has threads)
        if (ctdWorkers == 0)
        {
            ALIB_ASSERT_ERROR(       Strategy.WorkersMax > 0
                                && ( Strategy.Mode != ResizeStrategy::Modes::Fixed
                                     || Strategy.FixedSize > 0   ), "MGTHR/STRGY",
                "No threads to schedule job. Strategy values:\n"
                "       WorkersMax:    {}\n"
                "       Strategy.Mode: {}\n"
                "  Strategy.FixedSize: ",
                    Strategy.WorkersMax ,
                    Strategy.Mode == ResizeStrategy::Modes::Auto ? "Auto" : "Fixed",
                    Strategy.FixedSize )

                addThread();
        }
        TJob* job= pool().New<TJob>( std::forward<TArgs>(args)... );
        ALIB_ASSERT_ERROR( job->SizeOf()==sizeof(TJob), "MGTHR",
            "Error in ThreadPool::schedule: Job size mismatch. Expected {} "
            "while virtual method SizeOf returns {}.\n"
            "Override this method for job-type <{}>", sizeof(TJob), job->SizeOf(), &typeid(*job) )

        ALIB_ASSERT_ERROR(  Strategy.WorkersMax > 0, "MGTHR",
            "Error: Job pushed while this pool is shut down already. (Strategy.WorkersMax == 0) " )

        #if ALIB_DEBUG
            auto pair= DbgKnownJobs.EmplaceIfNotExistent( DbgKnownJobsEntry{ &typeid(TJob),
                                                                             sizeof(TJob), 0 } );
            ++pair.first.Value().Usage;
        #endif

        pushAndRelease( {job, keepJob} );
        return job;
    }

  public:
    /// The parameters used for scaling the amount of worker threads.
    /// The values herein can be changed from outside with direct access.
    ResizeStrategy          Strategy;

    /// Constructor.
    /// Initializes the thread pool with default settings for field #Strategy.
    ALIB_DLL                ThreadPool();

    #if ALIB_DEBUG_CRITICAL_SECTIONS
        /// Destructor. Cleans up and shuts down the thread pool.
        ALIB_DLL           ~ThreadPool() override;

        /// @return \c true if the lock is acquired (in non-shared mode), \c false otherwise.
        ALIB_DLL virtual bool DCSIsAcquired()                                        const override;

        /// @return \c true if the lock is shared-acquired (by at least any thread).
        ///            Otherwise, returns \c false.
        ALIB_DLL virtual bool DCSIsSharedAcquired()                                  const override;
    #else
        ALIB_DLL           ~ThreadPool();
    #endif

    using TCondition::Acquire;
    using TCondition::Release;
    
    /// Returns the mono allocator used by the thread pool.
    /// The pool has to be acquired before using it.
    /// @return The mono allocator.
    MonoAllocator&  GetAllocator()                                            { return ma; }

    /// Returns the pool allocator used by the thread pool.
    /// The pool has to be acquired before using it.
    /// @return The pool allocator.
    PoolAllocator&  GetPoolAllocator()                                      { return pool; }

    /// Just an alias to
    /// \https{Empty Base Optimization,en.cppreference.com/w/cpp/thread/thread/hardware_concurrency}.
    ///
    /// While the specification says
    /// <em>"If the value is not well-defined or not computable, returns \c 0"</em>,
    /// this method returns \c 1 in this case.
    ///
    /// Used as the default value for constructor parameter \p{pWorkersMax}.
    /// @return Returns the maximum number of threads that can be expected to run concurrently.
    static int      HardwareConcurrency()                                                   noexcept
    { return int(std::thread::hardware_concurrency()); }

    /// Returns the current number of worker threads.
    /// @return The number of jobs to process, including any currently processed one.
    int             CountedWorkers()                                          { return ctdWorkers; }

    /// Returns the current number of idle workers.
    /// @return The number of workers waiting on jobs to process.
    int             CountedIdleWorkers()                                         { return ctdIdle; }

    /// Checks if all workers are idle.
    /// @return \c true if the number of idle workers equals the number of workers,
    ///            \c false otherwise.
    bool            IsIdle()                                       { return ctdIdle == ctdWorkers; }

    /// Pushes a job of the custom type \p{TJob} into the priority queue.<br>
    /// The job is returned to the caller to be able to await results.
    /// It is the responsibility of the caller to pass the job to either method
    /// #DeleteJob or #DeleteJobDeferred for disposal.
    /// Note that the latter causes a #Sync on this pool, while with use of the former,
    /// the fulfilment of the returned job object has to be awaited first.
    /// @tparam TJob    The job type to create and schedule.
    /// @tparam TArgs   Types of the variadic arguments \p{args} that construct \p{TJob}.
    /// @param  args    Variadic arguments forwarded to the constructor of \p{TJob}.
    /// @return A reference to the job object for the caller to await results.
    template<typename TJob, typename... TArgs>
    [[nodiscard]]
    TJob&           Schedule( TArgs&&... args  )
    { return *schedule<TJob, TArgs...>( true,  std::forward<TArgs>(args)... ); }

    /// Pushes a job of the custom type \p{TJob} into the priority queue.
    /// In contrast to the sibling method #Schedule, the job is not returned by this method.
    /// Instead, it is scheduled for automatic disposal after execution.
    /// @tparam TJob    The job type to create and schedule.
    /// @tparam TArgs   Types of the variadic arguments \p{args} that construct \p{TJob}.
    /// @param  args    Variadic arguments forwarded to the constructor of \p{TJob}.
    template<typename TJob, typename... TArgs>
    void           ScheduleVoid( TArgs&&... args  )
    {  (void) schedule<TJob, TArgs...>( false, std::forward<TArgs>(args)... ); }

    /// Deletes a job object previously scheduled with #Schedule.
    ///
    /// \attention
    ///   The caller must not delete received job instances before they are processed.
    ///
    /// \attention
    ///   In case a caller does not want to wait longer, the method #DeleteJobDeferred is to be used,
    ///   which causes a #Sync on this pool.
    ///   Therefore, it is preferable to either wait on the job and use this method for deletion,
    ///   or to use the method #ScheduleVoid instead of #Schedule to not even get involved
    ///   with job-deletion.
    ///
    /// @param  job  The job returned from method #Schedule.
    void        DeleteJob(Job& job)
    {
        ALIB_LOCK
            auto size= job.SizeOf();
            job.~Job();
            pool.free(&job, size);
    }

    /// Same as #DeleteJob but schedules the deletion to be performed.
    /// This method is useful when a job instance was received with method #Schedule, but the
    /// caller does not want to continue waiting for the execution of the job.<br>
    /// If jobs indicate that they have been processed, then the method #DeleteJob is to be used.
    ///
    /// \attention Calling this method schedules a #Sync.
    ///            Therefore, the use of this method should be avoided.
    ///
    /// @see Methods #ScheduleVoid, #DeleteJob and #Sync.
    /// @param job    The job object to delete.
    void        DeleteJobDeferred(Job& job)            {  (void) schedule<JobSyncer>(false, &job); }

    /// This method ensures all worker threads in the thread pool complete their currently running
    /// jobs and also process all jobs that have been scheduled before a call to this method.
    /// This forces synchronization such that no new jobs are processed until the
    /// synchronization request is fulfilled.<br>
    /// It is particularly useful for scenarios requiring a consistent state or ensuring all pending
    /// asynchronous jobs are complete before proceeding.
    ///
    /// Consequently, a call to this method may inherently involve blocking the execution in the
    /// pool until all prior tasks are finalized. While it is designed to work efficiently with the
    /// thread pool mechanism, unnecessary or frequent calls to this method impose a performance
    /// disadvantage.
    ///
    /// Invoking \b %Sync() schedules
    /// \alib{threadmodel::ThreadPool;JobSyncer;a special synchronization job} in the queue of
    /// this \b %ThreadPool.
    /// Thus, the method is non-blocking and instantly returns.
    ///
    /// \par Necessity for Synchronization Explained with a Sample:
    ///   The requirement for synchronization is best illustrated with a practical scenario.
    ///   Consider a case where the main thread is responsible for scanning the filesystem.
    ///   For each file that meets certain criteria, a job is scheduled in the \b ThreadPool
    ///   to read the content of that file.
    ///   At this stage, the main thread is focused solely on scheduling file-reading jobs and
    ///   isn't directly processing the files.
    ///
    /// \par
    ///   Now, before the application proceeds to schedule further jobs, such as processing,
    ///   analyzing, or aggregating the file data, it is crucial to ensure that all file-reading
    ///   jobs have completed.
    ///   Without a synchronization mechanism, there is a risk that some worker threads are still
    ///   reading files while other threads - already assigned to the dependent processing tasks —
    ///   begin before the file data is available.
    ///
    /// \par
    ///   A call to \b %Sync() resolves this issue by ensuring that all file-reading jobs are
    ///   completed before any subsequent jobs that rely on their output are scheduled or processed.
    ///   This guarantees consistency, prevents race conditions, and ensures that no dependent
    ///   thread gets an incomplete or inconsistent dataset to work with.
    ///   In summary, synchronization acts as a safeguard in parallelized workflows where the
    ///   logical order of operations must be maintained across multiple threads, particularly
    ///   when tasks are interdependent.
    ///
    /// @see Method #DeleteJobDeferred, which likewise causes a synchronization.
    void        Sync()                             {  (void) schedule<JobSyncer>(false, nullptr); }

    #if DOXYGEN
    /// Waits until all threads are idle.
    /// @param timeout      The maximum time to wait.
    /// @param dbgWarnAfter The time after which a warning message will be printed to the
    ///                     debug log if the timeout was reached.<br>
    ///                     This parameter is only available in debug-compilations and thus
    ///                     should be passed using macro \ref ALIB_DBG.
    /// @return \c true if all threads are idle, \c false otherwise.
    ALIB_DLL
    bool        WaitForAllIdle( Ticks::Duration timeout,
                                Ticks::Duration dbgWarnAfter );
    #else
      ALIB_DLL bool WaitForAllIdle( Ticks::Duration timeout
                         ALIB_DBG(, Ticks::Duration dbgWarnAfter) );
      bool WaitForAllIdle( Ticks::Duration::TDuration timeout
                ALIB_DBG(, Ticks::Duration::TDuration dbgWarnAfter)  )
                { return WaitForAllIdle( Ticks::Duration(timeout) ALIB_DBG(, Ticks::Duration(dbgWarnAfter) ) ); }
          
    #endif

    /// Removes all threads.
    /// While this method waits that all jobs are finalized just as the method #WaitForAllIdle does,
    /// it is recommended to #WaitForAllIdle explicitly, before this method is called.
    /// This allows a more graceful shutdown with the possibility to take action on timeouts.
    ///
    /// If after the call to \b WaitForAllIdle no jobs were scheduled, this method is \b not #
    /// supposed to block.
    ALIB_DLL
    void                    Shutdown();

    /// Returns the current number of jobs in the queue.
    /// \note To get the overall number of unprocessed jobs, the difference between
    ///       #CountedWorkers and #CountedIdleWorkers has to be added.
    ///       However, under racing conditions, this difference might evaluated wrongly.
    ///       Therefore, if crucial, this pool has to be acquired before determining this.
    /// @return The number of jobs to process, not including any currently processed one.
    int                     CountedOpenJobs()                                { return ctdOpenJobs; }

    /// Returns the number of Jobs that have been scheduled during the lifetime of this instance.
    /// This is a statistics method.
    /// @return The number of jobs to process, including any currently processed one.
    uinteger                StatsCountedScheduledJobs()             { return ctdStatJobsScheduled; }

    #if ALIB_DEBUG && ALIB_STRINGS
        /// Writes the list of known jobs and their object sizes to the given target.
        /// \par Availability
        ///   This function is available only with debug-compilations and if the module
        ///   \alib_strings is included in the\alibbuild.
        /// @see Field DbgDumpKnownJobs.
        ///
        /// @param target     The string to write to.
        /// @param linePrefix A prefix string to each line. Defaults to two spaces.
        /// @return The number of known jobs.
        ALIB_DLL
        int DbgDumpKnownJobs(NAString& target, const NString& linePrefix= "  " );
    #endif

};  // class ThreadPool

} // namespace alib[::threadmodel]

/// Type alias in namespace \b alib.
using      ThreadPool    = threadmodel::ThreadPool;


}  // namespace [alib]

