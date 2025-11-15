//==================================================================================================
/// \file
/// This header-file is part of module \alib_threadmodel of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace threadmodel {

class DedicatedWorker;
class ThreadPool;
class DWManager;

//==================================================================================================
/// \attention This class belongs to the module \alib_threadmodel, which is not in a stable and
///            consistent state, yet.
///            Also, this type is considered experimental.
///
/// Defines jobs which are scheduled with instances of types \alib{threadmodel;ThreadPool} and
/// \alib{threadmodel;DedicatedWorker}.<br>
///
/// The type is virtual and designed to be the base type for custom <em>"jobs"</em>.
/// It holds a job-ID of type <c>std::type_info</c> which is used to identify the job type with
/// processing.
/// This ID usually represents - but does not need to! - the derived type.
/// By including the ID in the job, one derived job-type object can be used with different
/// job definitions.
/// For example, built-in derived type \alib{threadmodel;JPromise} may be used as the "shared data"
/// between the sender and the processing thread.
/// The constructor of \b %JPromise accepts a type ID to store.
///
/// This explains that class Job has two main aspects. It
/// - provides information to a worker thread about what is to be done.
/// - provides shared data between the sender and the worker.
///
/// Instances of this class are always \alib{monomem;TPoolAllocator;pool-allocated}.
/// Allocation, construction, destruction and deletion are always under the control of either
/// type \alib{threadmodel;ThreadPool} or \alib{threadmodel;DedicatedWorker}.
/// Pool allocation requires passing the object size with deletion.
/// For this, derived types that add new field members have to override method #SizeOf in a way
/// described with the documentation of that virtual method, to return the correct size of the
/// derived type. (All details on this topic are
/// \ref alib_contmono_poolallocator_allocinfo "found here").
///
/// Virtual method #Do has to be overridden in case a job is to be scheduled with the class
/// \alib{threadmodel;ThreadPool}.
/// In case a derived type is (exclusively) used with type \alib{threadmodel;DedicatedWorker},
/// it is up to the user of this module whether method #Do is implemented or not.
/// If not, then a custom type derived from \b DedicatedWorker has to override the method
/// \alib{threadmodel::DedicatedWorker;process} and handle the job type there.
///
/// @see The Programmer's Manual of this module \alib_threadmodel for detailed information.
//==================================================================================================
struct Job
{
    /// The identifier of the job. Any custom type can be given. Jobs with complex
    /// shared data types, which are used only for this specific job, usually use the
    /// shared data type itself here.
    const std::type_info&   ID;

    /// Protected constructor.
    /// @param id      Assigned to #ID.
    Job( const std::type_info& id )
    : ID        (id)                                                                              {}

    /// Protected destructor.
    virtual ~Job()                                                                         =default;

    /// This method is called by the worker thread in the case this job was scheduled
    /// for deferred deletion using \alib{threadmodel;ThreadPool::DeleteJobDeferred}
    /// or \alib{threadmodel;DedicatedWorker::DeleteJobDeferred}.
    /// The method is intended to prepare the deletion of this job without further actions
    /// that the sender of a job otherwise would perform.
    ///
    /// Overriding this method must be considered well. It is up to the contract between
    /// the caller and the implementation of a custom worker, how to handle this situation.
    ///
    /// As a sample, within the implementation of this module, it is used with class
    /// \alib{threadmodel;JPromise} to disable the warnings of deleting an unfulfilled and
    /// un-awaited promise in debug-compilations.
    ///
    /// @note This method is called by the worker thread, so it is not allowed to be called
    ///       by any other methods of the worker or caller
    virtual void    PrepareDeferredDeletion()                                                     {}

    /// Virtual method that returns the size of the type.<br>
    /// Derived types that add fields or use multi-inheritance, have to override this virtual
    /// function like done in this foundational version, but providing the custom type:
    ///          virtual size_t SizeOf()    override
    ///          {
    ///              return sizeof(MyJobType);
    ///          }
    ///
    /// @return The sizeof this type. Derived types have to return their size.
    virtual size_t  SizeOf()                                                 { return sizeof(Job); }

    /// Tests if this job's type ID equals the explicitly provided (non-deducible) template
    /// type \p{TOther}.
    /// @return \c true, if this instance represents job \p{TOther}, \c false otherwise.
    template<typename TOther>
    bool            Is()                                           { return  ID == typeid(TOther); }

    /// Tests if this instance is not initialized.
    /// @return \c true, if this instance represents non-job-type <c>void</c>, and
    /// \c false otherwise.
    template<typename TOther>
    bool            IsNull()                                         { return  ID == typeid(void); }

    /// Returns the shared data dynamically cast to provided (non-deducible) template type
    /// \p{TJob}.<br>
    /// With debug-compilations it is asserted that the requested job type \p{TJob} matches
    /// to what it really is.
    ///
    /// @tparam TJob The true type <c>this</c> points to.
    /// @return A reference to the shared data stored with this job.
    template<typename TJob>
    TJob&           Cast() {
        ALIB_ASSERT_ERROR( typeid(TJob) == ID, "TMOD",
            "Bad job casting.\n"
            "         Job type: <{}>\n"
            "   Requested type: <{}>",  &ID, &typeid(TJob) )

        auto* result= dynamic_cast<TJob*>(this);

        ALIB_ASSERT_ERROR( result != nullptr, "TMOD",
            "Job casting failed.\n"
            "         Job type: <{}>\n"
            "   Requested type: <{}>",  &ID, &typeid(TJob) )
        return *result;
    }

    /// Virtual function that may be overridden to execute the job when the job is executed by an
    /// \alib{threadmodel;DedicatedWorker}.
    /// In case it is executed by a \alib{threadmodel;ThreadPool} it has to be overridden.
    ///
    /// @return This default implementation returns \c false, which indicates that it is not
    ///         implemented.
    ///         Implementations need to return \c true.
    virtual bool        Do()                                                       { return false; }
                           
}; // struct Job

/// A simple encapsulated promise.
struct JPromise : public  Job
                , public  Promise
{
    /// Constructor.
    /// @param id     The type-info of the derived type. Passed to parent class \b %Job.
    JPromise(const std::type_info& id )                          : Job(id)                        {}

    /// Virtual destructor.
    virtual             ~JPromise()                                               override =default;

    /// Overrides the parent function as necessary.
    /// @return The sizeof this derived type.
    virtual size_t      SizeOf()                               override { return sizeof(JPromise); }

    #if ALIB_DEBUG
    /// Overrides the parent function only with debug-compilations.
    /// Avoids warnings about destroying unused promise
    virtual void    PrepareDeferredDeletion()              override { DbgOmitDestructionWarning(); }
    #endif
};


/// Possible priorities of jobs assigned to an \alib{threadmodel;DedicatedWorker}.
enum class Priority
{
    Lowest            =      0,  ///< As the name indicates.
    DeferredDeletion  =    500,  ///< As the name indicates.
    Low               =   1000,  ///< As the name indicates.
    Standard          =   2000,  ///< As the name indicates.
    High              =   3000,  ///< As the name indicates.
    Highest           =   4000,  ///< As the name indicates.
};

} // namespace alib[::threadmodel]

/// Type alias in namespace \b alib.
using      Job          = threadmodel::Job;

/// Type alias in namespace \b alib.
using      JPromise    = threadmodel::JPromise;

}  // namespace [alib]
