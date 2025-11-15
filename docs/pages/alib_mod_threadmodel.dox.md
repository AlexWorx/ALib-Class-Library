// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_threadmodel   ALib Module ThreadModel - Programmer's Manual

\tableofcontents

\attention
   This \alibmod was introduced only with the \alib version 2412
   and is considered unfinished and highly experimental.<br>
   Both attributes likewise apply to this "manual"!<br>
   Please do not read it and please do not use this module, yet - <b>unless you are like
   experiments! :-)</b>
   

\I{################################################################################################}
# 1. Introduction # {#alib_thrmod_intro}
This is a rather high-level \alibmod. It is by design separated from the low-level module
\alib_threads, which provides just fundamental functionality like a simple thread class, thread
registration and identification and several locking and asynchronous notification mechanics.

In contrast to that, this module introduces a <em>thread model</em>. So what is that? The answer
is that programmers have various different options to implement and manage parallelism. The
choice is dependent of the type of application and quite a lot also a matter of "taste".
Once more, \alib here chooses a rather traditional design.

This is the main reason why this module is separated from the low-level functionality: It is purely
a matter of the library's user to select this module into an \alibbuild or not.
The second important reason for separating this module from module \alib_threads is simply
to avoid massive cyclic dependencies with other modules.

\I{################################################################################################}
## 1.1 Thread Types # {#alib_thrmod_intro_threadtypes}
Massive multi-threading applications like server-software do not use simple thread pools, but
rather more sophisticated concepts like partitioned thread pools or task-affinity scheduling.
It is not the ambition and scope of \alib to support such software.

Instead, \alib aims to provide reasonable efficient concepts, which are easily manageable
and understandable.

This module differentiates between two sorts of worker threads, which are now quickly discussed
in theory.
                                                               
\I{################################################################################################}
### 1.1.1 Single Threads Dedicated to a Group of Jobs # {#alib_thrmod_intro_threadtypes_single}

Dedicated threads can be established to process tasks that are related by data, context, or type.
Each thread works with a specific dataset or set of tasks, which minimizes the need for locking
and helps avoid conflicts.

The advantages are:
- <b>Reduced Synchronization Overhead:</b><br>
  Because each thread deals with a specific set of data, there’s less contention for shared
  resources, which can reduce the need for locks.
- <b>Cache Efficiency:</b><br>
  Keeping similar tasks or data with the same thread can help improve cache locality, as data
  accessed by one thread is likely to remain in its cache.
- <b>Predictable Task Execution:</b><br>
  A programmer gains more control over the execution flow, as tasks of a particular type or
  priority are handled by dedicated threads.
   
When It’s Ideal:
This approach is especially effective when you have distinct task categories with minimal
cross-dependency. It’s common in real-time systems, certain types of game loops, or applications
where tasks have strict priorities or dependencies on specific resources.

\I{################################################################################################}
### 1.1.2 Thread Pools with Random Assignment # {#alib_thrmod_intro_threadtypes_pools}
Very common in modern libraries are thread pools. Those are collections of threads that are
assigned tasks as they become available.
Each task is handled by the next available thread, regardless of its type or data.

Advantages:
- <b>High Throughput and Scalability:</b><br>
  Pools can dynamically balance a load by assigning tasks to any available thread, which makes
  them very flexible for handling a large number of small, independent tasks.
- <b>Optimal for Short-Lived, Independent Tasks:</b><br>
  This approach works well when tasks are quick and mostly independent, as it maximizes
  CPU utilization with minimal idle time.
- <b>Reduced Overhead in Thread Management:</b><br>
  Rather than constantly creating and destroying threads, pools reuse threads, which
  reduces overhead.
   
When It’s Ideal:
Thread pools are ideal for workloads where tasks are mostly independent, like web servers
handling requests, background processing jobs, or any system where tasks are numerous and
lightweight.

\I{################################################################################################}

## 1.2 Pros and Cons # {#alib_thrmod_intro_threadtypes_procon}

The following table recaps the pros and cons associated with each type:

Aspect                          | Dedicated Threads                                       | Thread Pools (Library Approach)
------------------------------- |---------------------------------------------------------| ----------------------------------------------------
<b>Task Locality            </b>| High, due to grouped tasks by type/data                 | Low to moderate, as tasks go to any thread
<b>Synchronization Need     </b>| Lower, fewer locks if data sets are disjoint            | Higher, requires locks or atomic operations for shared data
<b>CPU Utilization          </b>| May have idle threads if tasks are unbalanced           | High, as all threads are actively used when needed
<b>Cache Efficiency         </b>| Good, better locality due to fixed data per thread      | Variable, depends on task scheduling
<b>Adaptability to Load     </b>| Less adaptable, might require load-balancing strategies | Very adaptable, dynamic balancing by pool
<b>Implementation Complexity</b>| Higher, requires more explicit management               | Lower, handled by the library
<b>Application Control      </b>| Higher, and probably better structured code entities    | Lower, handled by the library

                                 
\I{################################################################################################}
# 2. Class ThreadPool # {#alib_thrmod_threadpool}
The class \b ThreadPool implements the concept of pooled threads, as discussed in the introductory
sections above.
                                            
## 2.1 Source-Code Sample # {#alib_thrmod_threadpool_sample}
Let us start with a simple sample.
The first thing to do is defining a job-type, derived from class \alib{threadmodel;Job}:
                                                                     
\snippet "ut_threadmodel.cpp"     DOX_THREADMODEL_MYJOB

With this in place, we can pass jobs of this type to a thread pool:

\snippet "ut_threadmodel.cpp"     DOX_THREADMODEL_POOL_SIMPLE

This already showed the basic idea how the type can be used.

## 2.2 Implementing Derived Thread-Pools and Workers# {#alib_thrmod_threadpool_derived}
Beside the definition of custom \alib{threadmodel;Job}-types that are implementing its virtual
method \alib{threadmodel::Job;Do}, the classes \b ThreadPool and \b PoolWorker are also virtual
types and may be derived to allow more specialized use-cases.
The virtual methods that may be overwritten by custom types are:

- \alib{threadmodel;ThreadPool::CreateWorker}:<br>
  This method may be overwritten and return a custom derived type of class \b PoolWorker
- \alib{threadmodel;ThreadPool::DisposeWorker}:<br>
  Disposes a \b PoolWorker and may perform additional custom steps.
- \alib{threadmodel;PoolWorker::PrepareJob}:<br>
  This method is called by the worker before a job is executed. It may be overwritten
  to perform additional custom steps. Especially, here, the job may be equipped with custom
  information available through the \c this pointer.
  
\note The virtual method \alib{threadmodel;PoolWorker::Run} is inherited from class
      \alib{threads;Thread} and should usually \b not be overridden.

A typical use-case for such customization is to provide a per-thread
\alib{monomem;TMonoAllocator;MonoAllocator}, which is used to allocate memory during the
execution of the jobs, and which is reset after each job.
This approach allows efficient memory management without the need for locking.
For this, the following steps are needed:
- Define a custom \b PoolWorker type, which derives from class \b PoolWorker and which holds
  a \b MonoAllocator (and/or \b PoolAllocator) instance.
- Override the method \alib{threadmodel;ThreadPool::CreateWorker} to create such a custom
  worker. For this, copy the original implementation and just replace the type of the worker
  by the custom one.
- Override the method \alib{threadmodel;ThreadPool::DisposeWorker} to duly destruct and
  deallocate custom worker instances.
- Override the method \alib{threadmodel;PoolWorker::PrepareJob} to pass the worker's
  \b MonoAllocator instance to the (anyhow) custom job type.
- The job may now use the \b MonoAllocator instance to allocate memory.
  When the job is done, it is responsible for \alib{monomem;MonoAllocator::Reset;resetting} the
  allocator to its initial state.

## 2.3 Further Reading# {#alib_thrmod_threadpool_further}
\par Please Read Now:
    To avoid redundancy, we would ask you to read the following pieces of the reference
    documentation:
    - The reference documentation of class \alib{threadmodel;PoolWorker},
    - The reference documentation of class \alib{threadmodel;ThreadPool}, and
    - the reference documentation of class \alib{threadmodel;Job}.
    
This might be all that is needed to explain in this chapter.

     
\I{################################################################################################}
# 3. Class DedicatedWorker # {#alib_thrmod_dedicatedworker}
As discussed in the introductory sections above, one principle type of threads are ones
that are "dedicated to a group of jobs".
While the foundational module \alib_threads already provides the simple type \alib{threads;Thread},
which implements this concept along the design provided with class \b Thread of the
Java programming language, this module introduces a more sophisticated implementation with the
class \alib{threadmodel;DedicatedWorker}.

Here is a quick sample code that demonstrates the use of this class.
As a prerequisite we rely on the same class \b MyJob that had been introduced in the previous
section:
\snippet "ut_threadmodel.cpp"     DOX_THREADMODEL_MYJOB

Having this in place, a simple dedicated worker is quickly created and fed with such a job:

\snippet "ut_threadmodel.cpp"     DOX_THREADMODEL_DEDICATED_WORKER_SIMPLE

As the using sample shows, two interface versions are offered in this sample: One that returns
the Job instance, a second that does not return anything. Both versions have advantages
and disadvantages (explained in the comments above and in the reference documentation).

In more complicated cases it may be necessary to receive the job to be able to periodically
check for processing, but then the sender may "lose interest" in it.
To enable a due deletion of an unprocessed job, the method
\alib{threadmodel::DedicatedWorker;DeleteJobDeferred} is offered.
This pushes a new job (of a special internal type) into the execution queue of the worker, which
cares for deletion.
                   
The sample furthermore showed that the very same job-type which had been used in the previous
section with class \alib{threadmodel;ThreadPool}, can be used with the dedicated worker.
If done so, the advantage lies exactly here, namely that a job can be used with both concepts.<br>
However, this usually is neither needed nor wanted, just because the decision which thread-concept
to use, is dependent from exactly the nature of the job-types!

Therefore, the more common option of processing jobs with class \b DedicatedWorker is to override
its virtual method \alib{threadmodel::DedicatedWorker;process} and perform execution there.

Here is a sample code:

\snippet "ut_threadmodel.cpp"     DOX_THREADMODEL_DEDICATED_WORKER_USING_PROCESS
                                                                                
If the overridden method returns \c true for a job passed, then the virtual method
\alib{threadmodel;Job::Do} is not even called. In the sample above, both implementation even
do different things. The first doubles the input value, the second triples it.

Let us summarize this:
- When used exclusively with class \b DedicatedWorker, class \b Job does not need to override
  method \b Job::Dos().
- Instead, method \b DedicatedWorker::process is implemented which checks for the different
  types of jobs, which are scheduled by its dedicated public interface methods.
- The advantage here is that the derived worker type may hold data which is needed to
  execute the jobs.
- From a programmer's perspective, an additional advantage might be that all execution code
  is aggregated in one place, namely method \b process().

\par Please Read Now:
    To avoid redundancy, for further information, we would ask you to read the following pieces
    of the reference documentation:
    - The reference documentation of class \alib{threadmodel;DedicatedWorker},
    - the reference documentation of class \alib{threadmodel;Job}, and
    - the reference documentation of class \alib{threadmodel;DWManager}.
                                                                        

\I{################################################################################################}
# 4. Class Trigger and Interface class Triggered # {#alib_thrmod_trigger}
               
This pair of classes offers a next method to execute tasks asynchronously.
Here is a quick sample:

\snippet "ut_threadmodel.cpp"     DOX_THREADMODEL_TRIGGER_SIMPLE
                     
\par Please Read Now:
    To avoid redundancy, for further information, we would ask you to read the following pieces
    of the reference documentation:
    - The reference documentation of class \alib{threadmodel;Trigger}, and
    - the reference documentation of class \alib{threadmodel;Triggered}.

As a final not, class \alib{threadmodel;DedicatedWorker} implements the interface
\b %Triggered in order that it can be attached to a trigger.
If done, a trigger-job will be pushed in its command queue, and with that, the execution of
interface method \alib{threadmodel;Triggered::Trigger} is performed asynchronously.

<br><br><br><br><br><br> */
