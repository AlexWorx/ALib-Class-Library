// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/**
\page alib_mods_contmono   ALib Modules Containers & Monomem - Joint Programmer's Manual

\tableofcontents
                  
      
\I{################################################################################################}
# 1. Introduction # {#alib_contmono_intro}
This manual covers two \alibmods, namely:

1. \alib_containers_nl<br>
   Provides generic container types, with some of them being alternatives to containers found in the
   C++ Standard Library.

2. \alib_monomem_nl.<br>
   Provides alternatives to standard heap allocation.

The rationale behind joining both modules in one manual is because both topics are closely tight
together:
1. Containers define how a software organizes its objects, and
2. Monotonic allocation is an alternative way for containers to allocate the memory they need.

The reason why two separate \alibmods_nl have been established is because still both topics are
technically separate:
1. Containers are designed "allocation agnostic", and on the other hand,
2. Monotonic allocation can be used directly, without using container types.

This technical separation is also expressed by the fact that both modules (as always) live in
different namespaces:
- #alib::containers and
- #alib::monomem.

Finally, outside both modules, in namespace #alib::lang, which is available independent
from the list of modules included in an \alibdist, a few types implement an abstraction layer to 
memory allocation. Among those types are:
- \alib{lang::Allocator} and
- \alib{lang::AllocatorInterface}, and
- \alib{lang::AllocatorMember}

Their use is also covered in this manual.

\par Disclaimer:
\note
    A reader who is familiar with the concepts mentioned above,
    <b>should skip the following subsections</b> and may continue with chapter
    \ref alib_contmono_alib_prereq.<br>
    In consideration of the "weak monotonic" use cases, also experienced programmers might
    get some takeaways from introduction chapters:
    - \ref alib_contmono_intro_strictweak, and
    - \ref alib_contmono_intro_recycling.

\divkt
 - \b Modules \alib_containers and \alib_monomem modules work together for efficient 
   memory management and allocation.
 - <b>Separation of Concerns:</b> Both modules are technically independent.
    - Containers in \alib are allocation-agnostic.
    - <b>Monotonic and pool-allocation</b> offers efficiency apart from container use.
 - \b Containers benefit from being combined with \alib allocators. 
\divend   


\I{################################################################################################}
## 1.1 Allocation Models ## {#alib_contmono_intro_builtin_allocation}
Two "built-in" sorts of memory are available with the C++ language:
- <b>Local- Or Stack-Allocation<br></b>
  Local variables are created on the so called <em>"stack-frame"</em>, which is a limited, usually
  fixed size, linear memory buffer associated with each execution thread of a running process.
  Local variables are often called <em>"automatic variables"</em>, because they are constructed and
  deleted without the need of keywords <c>new</c> and <c>delete</c>.<br>
  Stack-allocation is by far the most performant allocation possible.
  Allocations and de-allocations are performed in constant time <b>O(1)</b>, in most cases even
  in "zero time", causing no performance overhead at all!<br>
  A jump-start for further reading may be the
  \https{corresponding Wikipedia article,en.wikipedia.org/wiki/Automatic_variable}.
- <b>Dynamic Allocation<br></b>
  <em>Dynamic memory</em>, or <em>"heap memory"</em> is used with non-local variables allocated
  with keyword operator <c>new</c> and de-allocated with <c>delete</c>.<br>
  Internally C++ standard library functions <c>std::malloc</c> and <c>std::free</c> are used,
  which have complex (and smart) implementations in place. In short, usually the functions are
  interfacing with the underlying operating system to retrieve bigger memory buffers, but provide
  an own "management layer" to efficiently organize allocated and de-allocated fragments.<br>
  A jump-start for further reading may be the
  \https{corresponding Wikipedia article,en.wikipedia.org/wiki/Memory_management}.

While the use of local allocation is very restricted (namely to local variables), dynamic allocation
is the opposite: it provides the maximum freedom possible.
But this degree of freedom comes at a high cost due to the necessary memory-management overhead
and other factors discussed in the next quick chapters.

To be motivated to use monotonic allocation, the following is important to understand (and to believe):
\note
  The performance overhead of frequent dynamic memory allocations and corresponding de-allocations,
  and the resulting <em>"fragmentation of the heap"</em>, is often underestimated by programmers,
  because it is not easily measurable with simple test software. The performance impact becomes
  relevant only when software runs for a longer time or otherwise performs many unordered sequences
  of \b new and \b delete operations.<br>
  Just like a freshly formatted hard-disk operates at its maximum speed but becomes slower
  over time (until a hard-disk de-fragmentation software is run), the
  dynamic memory management also loses efficiency over time.
  Simple performance or unit tests, in most cases will not disclose bottlenecks, because they
  operate on a "freshly formatted and clean heap"!

This is why \alib (and various other 3rd-party libraries) offers further memory allocation models.
                     
\divkt
 - <b>Local (Stack) Allocation</b>: 
   Fast and efficient but limited to local variables. 
   It works in constant time (O(1)) and sometimes even "zero time" without overhead.
 - <b>Dynamic (Heap) Allocation</b>: 
   Provides maximum flexibility but incurs a performance cost due to overhead from memory
   management and fragmentation, especially in long-running applications.
 - <b>Monotonic Allocation Rationale</b>: 
   Reducing heap fragmentation can greatly improve performance, particularly in long-running or 
   high-allocation scenarios.
\divend   

\I{################################################################################################}
## 1.2 Monotonic Allocation ## {#alib_contmono_intro_definition}

The module name <em>"monomem"</em> stands for <em>"monotonic memory"</em>.
The term <em>monotonic</em>  here relates to the way that memory allocation may be performed
by a software: At a certain point in execution time, a set of allocations is performed which are
all deleted at a distinct future point in time. The used memory for allocation is
<em>"monotonically growing"</em> during a defined time span.

This is what we call the "strict" definition of monotonic allocations. And this use case
is what common C++ libraries in this area offer to support.
In this introduction chapter, we will see how "weaker" definitions lead to a much wider range of
possible use-cases and thus leveraging a great potential for performance improvements.

The possibility of using monotonic allocation has to be identified and decided by a programmer.
Such analysis and decision is already the daily business in respect to whether object can be a fast
automatic local variable, or whether it is needed to allocate an object on the heap using <c>new</c>.

Due to the name "local variable", the decision about automatic vs. dynamic allocation seems to be
bound to the fact whether an object is used only locally in a method or function, or if it is
shared between different code entities. But this is only partly true: A local variable may well be
passed to arbitrary functions invoked by the code block that declares the variable.<br>
Instead, the decision is much more considering an object's life cycle: If an object can
be deleted with the end of the code block it was declared in, then an automatic variable is
to be used. If otherwise the life cycle has to survive the execution of such code block,
dynamic allocation has to be used. (Note: An exception is of course the needed object size: Large
objects may be dynamically allocated, due to the limited size of stack-memory.)

Now, the extended decision about the potential use of monotonic allocation is also bound to observing
life cycles. But this time, it is not about the life cycle of one single object, but instead about
the life cycle of a set of objects.<br>
The decision rule can be phrased as follows:

\par Definition:
<em><b>If a set of objects can be identified, that share the same, limited, and determined life
cycle, and if such life cycle is not identical to a code block (which indicated automatic allocation
for each object), the allocation of the set of objects can be performed using monotonic allocation.</b></em>

With this definition, the implementation of monotonic allocation can be easily specified:

1. At the start of the joint life cycle, a bigger piece of dynamic memory is allocated.
   In this manual, these pieces are called <em>"buffers"</em>.
2. Objects of the set are allocated in this buffer using
   \https{C++ placement-new,en.cppreference.com/w/cpp/language/new}.
3. If a buffer is full, it is added to a list of used buffers for later de-allocation
   and a new buffer is created.
4. At the end of the life cycle of the objects, objects are not deleted using keyword <c>delete</c>.
   Instead, the object's destructor methods are "manually" invoked.
5. Finally, the list of allocated buffers is de-allocated.

The performance advantages can be tremendous, not only because what was already mentioned:

- This approach relieves the built-in dynamic memory management tremendously. It avoids fragmentation,
  especially if it is taken into account that in parallel to the monotonic allocation, objects that are not
  belonging to the set, are allocated and de-allocated. Those objects would cause "holes" in the
  heap memory, if no monotonic allocation was in place.

But further reasons of performance gain are:

- The monotonic allocations themselves are performed very fast.
- Often, no thread-safeness has to be guaranteed with monotonic allocation.
  \note This is not always true. At the end of this manual,
  \ref alib_contmono_threadsharing "an own chapter" on this complex topic is provided.


- On the hardware level: It is likely that the different access operations to the memory of monotonic
  allocated objects is done in code sequence that are executed in timely relations, respectively in
  code sequences that are executed in series. This way, the hardware memory cache will less often be
  reloaded, as all objects tend to lie side-by-side in the same memory buffer.<br>

  \note
    By being bound to the impact of fragmentation, the impact of this advantage is likewise very
    hard to measure (and to prove!) in simple testing software.

<p>
\divkt
 - <b>Definition</b> : 
   Monotonic allocation grows memory during a defined time span and only deallocates at the end 
   of an object <b>set’s</b> lifecycle.
 - <b>Benefits</b> : 
   Reduces heap fragmentation, eliminates the need for frequent deallocations, and improves 
   performance in scenarios where objects share the same lifecycle.
 - <b>Use Cases</b> : 
   Effective for software entities whose allocated objects have predictable lifecycles.
\divend   


\I{################################################################################################}
## 1.3 Samples  ## {#alib_contmono_intro_samples}

A well understandable sample use case for monotonic allocation, is found with module \alib_expressions.
There, this technique is used twice:
1. During the compilation of an expression, the abstract syntax tree, non-optimized portions of
   the resulting expression program, and many other intermediately needed objects are
   monotonically allocated and disposed at once when compilation is done.
2. During the evaluation of compiled expressions, intermediate results like concatenated strings
   or for example the virtual machine's stack memory, is monotonically allocated and disposed when
   the expression is evaluated against a specific scope.

The second use of monotonic allocation is even more important due to the fact that an expression is
compiled only once, but may be evaluated plenty of times against different scopes.
Now, with the first evaluation, buffers for monotonic memory are allocated, but after the
evaluation is done, they are not freed! While the intermediate objects are duly destructed,
the buffers already allocated are kept for the next run.<br>
This way, starting with the second evaluation of an expression, not a single dynamic memory
allocation is performed!

Another prominent sample is found in server software that offer a certain service. Let's consider
a simple <em>http-server</em>. Usual implementations maintain a pool of threads and with each
incoming request a next free thread is chosen to process the request. Now, all memory needed during
processing can be considered monotonically allocatable.
And you can bet that all prominent implementations of <em>http-serves</em> are using this concept,
as introduced here.

\I{################################################################################################}
## 1.4 Strict And Weak Monotonic Allocation  ## {#alib_contmono_intro_strictweak}
The definition given above stated that those objects that share the same life cycle may
be considered a set and be monotonically allocated.
This definition is quite strict and therefore strongly narrows the use cases.

There are several ways to weaken the requirements. The following <em>"thought experiment"</em> may
easily demonstrate this: Consider a simple command-line calculator software, as
\ref alib_expressions_calculator "demonstrated with module Expressions" or any other "simple"
software that accepts an input, creates an output, and exits.
What would happen if every dynamic allocation would be turned into a monotonic allocation
by extending the life cycle of all allocations to the overall life cycle of the software process?

The answer is: no problem! The result executable would potentially show slightly more performance
while it would most probably allocate a higher total amount of dynamic memory during it's run.
But because it is known that the total amount of allocations is very limited - considered the
simplicity of the task - it is ensured that no situation of <b>memory exhaustion</b> happens - which is
exactly the danger if the strictness of the definition of monotonic allocation is weakened.

With this thought experiment, we can "weaken" the definition and can extend the use cases in the
following ways

\par Definition:
<em><b>If a lifecycle can be determined for a set of objects, so that each object's life cycle
is included in this life cycle, the allocation of the set of objects can be performed using
WEAK monotonic allocation.</b></em>

The technical implementation of weak monotonic allocation is the same as that of
strict monotonic allocation and for this reason, this \alibmod_nl does not provide any specific
interface to support such approach. It is moreover not transparent to the module's code entity
if monotonic allocation is strict or weak.

\attention
  To summarize and strongly note: It has to be acknowledged by a programmer that the peak consumption
  of dynamic memory might increase with weak monotonic allocation, due to the fact that
  de-allocation of all allocated memory only happens at the end of the joint life cycle of
  all objects included.

Nevertheless, if the increase of the peak memory consumption is guaranteed to be limited, then the
performance gain of monotonic allocation usually goes far beyond the performance loss that might
be caused (in the whole system!) by a higher peak memory consumption.
             
\divkt
 - <b>Strict Monotonic Allocation</b>: Objects with the same lifecycle are allocated in large 
   buffers and deallocated together.
 - <b>Weak Monotonic Allocation</b>: Extending the lifecycle of portions of an object set
   voluntarily beyond their needs. While this may increase peak memory usage, it turns a 
   use case into one that allows monotonic allocation.  
\divend   


\I{################################################################################################}
## 1.5 Recycling  ## {#alib_contmono_intro_recycling}

The takeaway from the previous section was that weakening the requirements to perform
monotonic allocation might immensely broaden its use cases and with that the potential to
optimize the performance of a software.

Now, let us construct a sample that - at the first sight - is not a candidate for optimization
and let's think ways to turn it into a candidate. Consider the following scenario:

- A http server software runs (theoretically) infinitely.
- A pool of threads process http requests (using monotonic allocation, as noted previously).
- The threads have access to a global storage collecting user sessions.
- The processing threads can search sessions in this global storage and create new sessions as needed
- A dedicated background thread removes expired sessions.

A typical implementation in C++ would use an <c>std::unordered_map</c> (aka hash table) to
store the session objects. With each insertion, the <c>std::unordered_map</c> allocates an
internal "node" object and with removals such node object is deleted.

Together with dynamic memory allocations performed by the server in other places, over time a
reasonable fragmentation of the heap memory will occur. The internal node objects allocated by
the hash table at a certain point in time, will be spread (fragmented) across the whole
area of dynamic memory allocated from the operating system.

\note
  In consideration of what was said at the end of section \ref alib_contmono_intro_definition
  about the effects on memory access on the hardware level, this fragmentation may hurt performance
  dramatically: Consider the worker threads running in separated "hyper threads" of CPU kernels.
  While their local memory is monotonic and thus objects are sitting nicely side-by-side, when
  searching the hash table, internal node objects that reside in fragmented (different)
  memory areas have to be accessed. This might easily result in (repeated!) misses of the first
  level cache of the CPU kernels, with all related dramatic performance drawbacks.

There is a very simple way to mitigate this problem: Instead of de-allocating a node object
when a session is removed from the storage, such node object has to be destructed only, while
it's allocated memory is to be pushed to the front of a simple linked list of
<b>"recyclable nodes"</b>. When a new user session is inserted, it is checked if a
recyclable node is available and only if not, a new node is allocated.

The consequence of this approach are:
- The software never de-allocates a node (while it is infinitely running)
- The number of overall node allocations is limited to the peak number of parallel sessions
  handled by the server (without that, the number would be infinite).
- Pushing and popping node objects to a "recycler stack" is performed in constant time and can
  in theory be considered a no-cost operation.
- The peak memory consumption that has to be taken into account is not higher than the
  the peak that occurs with a peak server load. This means it is ignorable and does not contribute
  to the calculated need of hardware dimensions.

<b>But maybe the best of all is:</b>
- This approach turns this scenario into a use case for strict monotonic allocation!<br>
  (Remember: monotonic allocation can be used when a set of objects share the same lifespan,
   which is now the case.)

A reader could now comment that such recycling technique is not necessarily related to monotonic
memory and can easily be used without it.
This is very true and reflected by the fact that we are talking about
two separate \alibmods_nl in this manual: \alib_containers_nl and \alib_monomem_nl. Both are
\ref alib_manual_modules_dependencies_mandatory "not dependent" on each other. The topic of
"recycling container objects" is completely covered with module \alib_containers_nl and
furthermore, type \alib{lang::StdContainerAllocatorRecycling} even provides mechanics to enable
recycling for containers of the C++ Standard Library. Finally, in a next independent effort,
module \alib_monomem_nl, besides monotonic allocation offers in addition mechanics for recycling
non-container objects.

Note that with both in place, recycling, and monotonic allocation, the joint benefit surpasses
the benefit of the two single efforts: The number of potential cache-misses on the hardware level is 
greatly reduced, if already the first memory buffer allocated is large enough to store all
(recyclable) nodes needed to handle the peak of parallel allocated objects.

Besides the fact that the use of container types can often be turned into a use case of
monotonic allocation, the more general takeaway of this final introductory chapter is:

                
\divkt
 - <b>Recycling Concept</b>: Recycling allows reusing memory instead of deallocating and 
   reallocating it, especially useful for long-running applications with dynamically created 
   and destroyed objects.
 - <b>Recycling Strategy</b>: By recycling objects, memory fragmentation is minimized, 
   hardware-cache efficiency is raised, allocation and deallocation become very fast, 
   and thus a tremendous overall performance gain is plausible.
 - <b>Recommondation</b>: It might be worthwhile to think a little longer about the possibility 
   of applying monotonic allocation to a specific use case. With a little recycling of objects and 
   other similar simple tricks, many use-case-scenarios, which - at a first glance - do not seem    
   suitable for monotonic allocation, can be turned into a suitable one.
\divend   

\I{################################################################################################}
# 2. The ALib Allocation Model # {#alib_contmono_alib_prereq}
This was quite a long introduction chapter, and unfortunately before we start with monotonic
allocation, a few prerequisites that \alib provides outside modules \alib_containers_nl
and \alib_monomem_nl, have to be introduced.

These prerequisites consider the generalization of the idioms introduced by module \alib_monomem_nl,
with the result that \b all other \alibmods_nl can make use of monotonic allocation,
without having any header file dependencies into module \alib_monomem_nl.

Consequently, the types introduced here neither belong to \alib_monomem_nl nor to
\alib_containers_nl. 
Instead, they do belong to the core of \alib and thus reside in namespace #alib::lang.
Remember that types of that namespace are always included in an \alibdist, independent of its module
selection.


\I{------------------------------------------------------------------------------------------------}
## 2.1 Class Allocator ## {#alib_contmono_alib_prereq_allocator}
Class \alib{lang;Allocator} is a prototype class. In fact, this class is not exposed to the
C++ compiler and it is just living in the documentation!
The prototype defines what an "allocator" needs to offer to be usable as a template
parameter that expects an "ALib Allocator". Such template parameters are always named
\p{TAllocator} across all of \alib.<br>

Allocators have to provide four basic allocation/de-allocation functions, which are quite user-unfriendly.
Besides those, a few informational values and methods for debug-purposes are to be given.

\par Please Read Now:
To avoid redundancy, we would ask you to
\alib{lang;Allocator;read the reference documentation of the class now} and then return here.

Finally, types that implement this prototype, need to provide \b operator()(), which is
explained in the next section.

\I{------------------------------------------------------------------------------------------------}
## 2.2 Class AllocatorInterface ## {#alib_contmono_alib_prereq_allocinterface}
As just seen, the prototype for \alib \b %Allocator implementations demands to specify the
\alib{lang::Allocator;operator()()} to return a value of type \alib{lang;AllocatorInterface}.
This type provides a comfortable (and much safer!), high-level interface into allocators.

\par Please Read Now:
To avoid redundancy, we would ask you to
\alib{lang;AllocatorInterface;read the reference documentation of the class} now, which includes
a quick tutorial sample code,  and then return here.

Of course, these high-level convenient methods make use of the four "basic"
allocation/de-allocation methods provided with any \b Allocator.

With this "design trick" in place, the using code can always "call" any given templated operator
and receive the very same high-level interface. Standard C++ compilers will optimize out
these intermediate steps: the invocation of the call operator as well as the calls of the
methods found with \b AllocatorInterface.

\I{------------------------------------------------------------------------------------------------}
## 2.3 Struct AllocatorMember ## {#alib_contmono_alib_prereq_allocmember}
The third type found in namespace #alib::lang related to allocation, is \alib{lang;AllocatorMember}.
This type is used to implement templated types that are allocation agnostic and enables to
eliminate an overhead in the size of a type, at the moment that heap allocation is chosen.

\par Please Read Now:
To avoid redundancy, we would ask you to
\alib{lang;AllocatorMember;read the reference documentation of the struct now}, which includes
a quick tutorial sample code, and then return here.

\I{------------------------------------------------------------------------------------------------}
## 2.4 Struct StdContainerAllocator ## {#alib_contmono_alib_prereq_stdcontalloc}
Finally, namespace #alib::lang introduces types \alib{lang;StdContainerAllocator} as well as
\alib{lang;StdContainerAllocatorRecycling}.
For now, let's skip reading their reference documentation.
All details of these types will be discussed in a later chapter \ref alib_contmono_stdcontainers.
                
\divkt
  Unlike C++ 17 std::pmr allocators, which use dynamic (virtual) types, \alib allocators use 
  templates instead.  
  Only low-level methods need to be implemented and are defined by a non-existing 
  \alib{lang;Allocator;prototype class}.<br>
  With standard heap-allocation, containers avoid having a needless reference to 
  \alib{lang;HeapAllocator} and thus, no different version of containers are required.
  (Unlike doubled container types found in <c>std</c> and <c>std::pmr</c>.)   
\divend   

\I{################################################################################################}
# 3. Class MonoAllocator # {#alib_contmono_class_monoalloc}
\note 
  Behind the scenes, class \b MonoAllocator is just a type definition into templated type
  \alib{monomem;TMonoAllocator}. To keep it simpler, for now, we only consider type definition 
  \b MonoAllocator. Only later, in chapter \ref alib_contmono_chaining the use of the 
  templated type is explained.   

The central type of module \alib_monomem_nl is class \alib{monomem;TMonoAllocator}, which is 
usually referred to its default type-definition \alib{MonoAllocator}.
Its interface follows the standards described in the previous chapter
\ref alib_contmono_alib_prereq_allocinterface.

Internally, bigger buffers are requested from the heap and used for the provision of allocations.
No meta-information about the allocations is stored. Instead, allocated objects get laid 
side-by-side, with no gaps, apart from those that may arise by alignment requirements.
 
When the next allocation does not fit into the current buffer, a new buffer is allocated, probably 
with a higher size than the previous buffer had. The latter is configurable.<br>
The memory buffers that an instance of class \alib{MonoAllocator} allocates on the heap
are stored in a single-linked list and are necessarily only deleted with the deletion of the 
allocator instance.


\I{################################################################################################}
## 3.1 Resetting Class MonoAllocator ## {#alib_contmono_class_monoalloc_reset}
The allocator offers a method named \alib{monomem::TMonoAllocator;Reset}. 
Before the invocation of the method, all monotonically allocated objects have to be destructed 
(by using software) - but not deleted. This ensures that any resources these have acquired, will
be released.

After invocation of \b %Reset, the allocator can be re-used, which means that the previously 
allocated buffers are re-used as well! 
Only in the case that more memory is required than in a prior "run", a new buffer is requested from 
the heap.


\I{################################################################################################}
## 3.2 MonoAllocator Snapshots ## {#alib_contmono_class_monoalloc_snapshot}
The simple concept of resetting an allocator to be able to reuse it is internally
realized by a more sophisticated concept. Class \b %MonoAllocator does not only
allow being just reset to its initial empty state but also allows creating a
\alib{monomem;Snapshot} of its current use and later reset back to exactly this state.

A snapshot is taken with the method \alib{monomem::TMonoAllocator;TakeSnapshot}. The returned object
is a lightweight, 16-byte value (on 64-Bit systems, 8 byte on 32-bit systems).
A previously taken snapshot may be passed to method \alib{monomem::TMonoAllocator;Reset} which
replaces its default argument that completely resets an allocator.

If several snapshots are taken, those are "nested": A later snapshot "points to" memory covered
by a prior one. Consequently, if an allocator is reset to an "older" snapshot, all subsequent
snapshots become invalid. In debug-compilations, an \alib assertion is raised if
an invalid snapshot is passed to method \alib{monomem::TMonoAllocator;Reset}.

This concept allows furthermore reducing heap allocations by splitting a set of monotonic
allocations into several, nested ones. Nested snapshots and resets slightly extend the definition
of monotonic allocation as given in the introduction chapters.
It is especially useful in situations where <em>weak monotonic allocation</em> is nested in a more
<em>strict</em> use case because it has the potential to mitigate peaks of overall memory usage.

While the exact conditions of use are not easy to precisely define, during the course of 
implementing monotonic allocation, a programmer will probably identify use cases for nested 
allocations quite naturally.

\I{################################################################################################}
## 3.3 Class LocalAllocator ## {#alib_contmono_class_monoalloc_local}
Class \alib{monomem;TLocalAllocator} is a derivate of \b MonoAllocator, which has a large
internal member which is used as the first memory buffer. The size of the member is defined by
a template parameter. In namespace #alib, several predefined type definitions for different
sizes are given with
- \alib{LocalAllocator1K},[alib_key_takeaways.md](../../../../../a/Downloads/alib_key_takeaways.md)
- \alib{LocalAllocator2K}
- \alib{LocalAllocator4K}
- ...
- \alib{LocalAllocator64K}.

This class is designed to be used as a local function variable. With that, the first buffer of
memory (and typically the single allocated buffer) resides on the execution stack of the current
thread. This is the most efficient allocation method available: Not only does it avoid just any
heap allocation, but also the memory used is in the same hardware range as other local variables of 
the execution thread. 
With this, it is almost guaranteed that the memory is available in the first level cache of the 
microprocessor.

Of course, a user has to be aware, that:
- Stack memory is a limited resource.
- Recursive function calls can very quickly lead to stack overflows.
- All memory is lost (invalid) as soon as the function exits.

<p>              
\divkt
 Class \alib{monomem;TMonoAllocator} is a monotonic allocator with two special features. Firstly, it 
 allows nested snapshots and resets. Secondly, externally provided first buffers are accepted.<br>
 With the latter, not only stack memory can be used (class \b LocalAllocator), but other
 nice tricks are possible too, as it will be described in later chapters.  
\divend   

\I{################################################################################################}
# 4. Container Types # {#alib_contmono_containers}

\note
  While the allocator types discussed in the previous section are available with module
  \alib_monomem_nl, now we discuss types imposed by module \alib_containers_nl. Remember, this
  is a joint Programmer's Manual for the two \alibmods.

Module \alib_containers_nl provides implementations of container types that are very similar to
types found in the C++ standard library.
Some of the provided types are considered alternatives for the standard types to be used with
allocators, others are types not existing in the standard.

Only types that internally allocate node objects are replaced. Standard containers that do not
allocate node objects are:
- <c>std::vector</c> and
- <c>std::deque</c>.

No alternatives are given for these two types. This is true for the current release of the library
and it is foreseeable that also in future releases no alternative will be given.
                     
\attention
  Many readers here may think now: No, why should I use other containers but those provided
  with the standard?<br>
  Our answer is this: Providing and maintaining the types  was and is lot of work and we would not 
  have started it and continue to maintain the types, if the barriers that the architecture of 
  the <c>std</c>-types imposes towards implementing efficient allocation strategies, 
  were not so high. 
  We had once hoped that with C++ 17 namespace <c>std::pmr</c> things become better, 
  but this was only partly the case.   
                     
\attention
  While the rationale for the provision of the alternatives to the C++ standard container types
  is given mainly in this and the next chapter \ref alib_contmono_stdcontainers, accross
  this manual and in the reference documentation, even more indications of why we started replacing
  them in our software, can be found.<br>


\I{################################################################################################}
## 4.1 Provided Containers ## {#alib_contmono_containers_types}

The following container types are provided with module \alib_containers_nl :

- \alib{containers;FixedCapacityVector}, which is a fixed size array, something between
  A C++ array and <c>std::vector</c>. With that, type definition
  \alib{FixedSizePriorityQueue} is also given.
- \alib{containers;List}, which is an alternative to <c>std::list</c>.
- \alib{containers;LRUCacheTable}, implementing a simple caching container that follows the
   "least recently used" principle. Along with it come type definitions
    - \alib{containers;LRUCacheSet} and
    - \alib{containers;LRUCacheMap},

  This type has no alternative in the C++ standard library.
- \alib{containers;HashTable}, which is an alternative for four container types of \c std.
  In fact this type provides even more flexibility then the four together and is still more
  convenient and equally fast. Along with it come type definitions
    - \alib{containers;HashSet}, which is an alternative to types <c>std::unordered_set</c> and
       <c>std::unordered_multiset</c>.
    - \alib{containers;HashMap}, which is an alternative to types <c>std::unordered_map</c> and
      <c>std::unordered_multimap</c>.
- \alib{containers;SharedVal}, similar to <c>std::shared_ptr</c> but which holds a value, instead
  of a pointer.
- \alib{containers;SharedPtr}, similar to <c>std::shared_ptr</c> with pros and cons.
- \alib{monomem;TSharedMonoVal}, which is an automatic pointer including a dedicated monotonic memory 
  resource. Note that this type is located in (and available with) module \alib_monomem_nl
  and is discussed in separate later chapter \ref alib_contmono_smv.   
- \alib{containers;StringTree}, a specialized container which has a wide area of use cases.
  This container is for example used with camps \alib_config, \alib_alox, and \alib_files.

All container types provided, accept an \alib{lang;Allocator} with construction which they use
for allocations. In the "simple" case of using class \alib{lang::HeapAllocator}, (which is
always included in any \alibdist, independent of the inclusion of module \alib_monomem_nl,)
alternative constructors exist that do not expect an external allocator.<br>
Of course, a user of the library could also provide an own allocator type and run these containers
with that.

\note Some further types provided by other \alibmods, likewise support template parameter
      \p{TAllocator}. Those are listed in separate chapter \ref alib_contmono_further.

For a detailed description of the container types, please consult their reference
documentation, as linked above with the type names.

Besides generic allocation, the some of the container types support recycling of internal node
objects. This is discussed in the next chapter.

\I{################################################################################################}
## 4.2 Recycling Support ## {#alib_contmono_containers_recycling_private}

The concept of "node recycling", as it was introduced in chapter \ref alib_contmono_intro_recycling,
is supported by all container types that internally allocate nodes, which are
\alib{containers;List}, \alib{containers;HashTable} and
\alib{containers;StringTree}.
For that, each type specifies a template parameter named \p{TRecycling} which has to be set as any
of the following three enum elements:

- \alib{containers;Recycling;None},
- \alib{containers;Recycling;Private} (the default), or
- \alib{containers;Recycling;Shared}

\b None switches recycling off and supports use cases of \b strict monotonic allocation.
Its advantage is that the (negligible) overhead of collecting destructed nodes and re-using with
insertions is omitted.

The default value \b Private establishes an internal stack implementation (a forward list) and all
recycling takes place internally.

The next chapter introduces details of shared recycling.

\I{################################################################################################}
## 4.3 Shared Recycling ## {#alib_contmono_containers_recycling_shared}

The concept of \b shared recycling aims to reduce the overall peak size of allocated
memory from being the sum of each containers peak size (in case of private recycling), to reflecting
the true peak of all concurrently allocated nodes.

But this slight optimization is not the main reason to offer shared recycling. More important,
use cases exist, which cannot be made suitable for monotonic allocation if only
private recycling was available.

As a sample let us have a look at a situation where a top level list contains container objects
which in turn contain the element objects. In other words, a programmer wants to have tree of
entries, restricted to depth two. Such situation might for example be found with INI-files.
The outer list is the list of INI-file sections and each section holds a list of INI-file variables.

Now, a software should be free to create and delete arbitrary sections and arbitrary variables
within such sections during the life-cycle of the object.
The problem arises if a complete category is deleted: While the object for the category itself
is recycled with the creation of a next category, the list of entries of such recycled
category was destructed on removal.
Now, if private recycling is used with the list of entries, the recycled
entries are "lost" with that destruction. Such loss of objects makes this use case not suitable
for monotonic allocation, because an "infinite" amount of insertions and deletions of categories
might occur.

The way out of this is to share the stack of recyclable elements among different containers.
With that, each variable list uses the same "external" recycler instance and when a list is
destructed, the recycled elements are not lost.

As already noted above, this approach does not only resolve the original problem, it further
optimizes the overall memory use: As soon as a single variable is erased, the next insertion will
recycle this  entry, no matter which section is concerned.

Shared recycling is activated by passing \alib{containers;Recycling;Recycling::Shared},
for template parameter \p{TRecycling} with the corresponding container instance.
When doing that, the container instance requires a constructor parameter which provides
the external, shared recycler object. This parameter replaces the otherwise necessary
parameter \p{allocator}, because the allocator is embedded in the shared recycler that is externally
created.
The exact (templated) type of the recycler that has to be created and passed, is determined by a
type definition named \b SharedRecyclerType that each container type offers. (For example
\alib{containers;List::SharedRecyclerType} or
\alib{containers;HashTable::SharedRecyclerType}.)

The object passed, has to survive the life-span of each container type that uses it.

Here is a quick sample taken from the unit tests that demonstrates the use of shared recycling:

  \snippet "ut_hashtable.cpp"  DOX_MONOMEM_RECYCLER
                                         
\divkt
 - <b>Node Recycling</b>: Internal container nodes can be recycled to minimize memory allocations.
 - <b>Types of Recycling</b>: \alib containers support \b None, \b Private, and \b Shared recycling, with shared recycling offering cross-container memory reuse, optimizing peak memory usage.
\divend   


\I{################################################################################################}
## 4.4 Recycling Of Non-Node Types ## {#alib_contmono_containers_recycling_nonnodetypes}

The implementation of class \alib{containers;HashTable} allocates so called <em>"bucket arrays"</em>
besides node objects. If, with the insertion of a new element, the
\alib{containers;HashTable::MaxLoadFactor;maximum load factor} of the hash-table is reached,
a new, bigger bucket array is allocated and the existing nodes are "re-hashed". Then the former
bucket array becomes useless, hence would be de-allocated if dynamic memory allocation was used.

In general, this is not a problematic situation because the growth continues only up to a
certain limit, which does not violate the concept of \b weak monotonic allocation.
Nevertheless, if recycling (private or shared) is enabled, class \b HashTable passes this
non-node piece of memory to its recycler object: There, the array is sliced into as many
(duly aligned) pieces of the internal node size as possible, and then passed over to recycling
stack.

\note
   While this constitutes a further small optimization of memory use, it is a design decision that
   this technique is not extended into an interface that allows other unused (hence weak monotonic)
   object allocations to be fed to a container's recycler.

What was said above is, however, only true if the underlying allocator is of type
\alib{monomem;TMonoAllocator}. More precisely, if method \alib{lang;Allocator::allowsMemSplit}
returns \c true. The rationale behind this is that containers that do to have their
allocated objects freed as they were allocated (which is also true for heap allocations),
then such buckets must not be split into nodes.
           
\divkt
  \alib containers may recycle non-node memory, by slicing it to node-sized pieces.
  Internally, this behavior is automatically switched on with allocators that allow such
  slicing.    
\divend   

\I{################################################################################################}
## 4.5 Reserving Capacity ## {#alib_contmono_containers_recycling_reserving}

As seen in the last lines of the sample above, the built-in recyclers even allow to reserve an
amount of recyclables upfront. The concept of reserving container capacity in case the future size
of a container is known or at least assessable, from the standard library. With recycling in
place, camp \alib_containers_nl extends this feature to likewise pre-allocating a corresponding
amount of node objects as well. For this, interface methods \b RecyclablesCount and
\b ReserveRecyclables (for example \alib{containers;List::ReserveRecyclables} or
\alib{containers;HashTable::RecyclablesCount}) are provided.

Note that pre-allocation of recyclable node objects can reasonably improve execution performance.
This is due to the caching levels of modern processor hardware. More information on this was given
in chapter \ref alib_contmono_intro_recycling.

Furthermore, preserving capacity can be useful when monotonic allocation is used.
As a sample, consider the creation of (nested)
\ref alib_contmono_class_monoalloc_snapshot "snapshots": During the execution of an
\b "inner" monotonic allocation sequence, which will be reset in the near future, no insertions of
elements into containers that belong to the "outer" sequence must be done - unless it is ensured
that such insertions do not allocate node objects, because those would become invalid with the
reset! Preserving capacity including recyclables is a way to mitigate this common source of
"undefined behavior" when using nested monotonic allocations. Of course, such approach needs
to be taken with a lot of care: the number of preallocated recyclables must not be accidentally
underestimated and some assertions in debug-compilations should be added. Still, often it is
absolutely foreseeable how many insertions are expected until an allocator can be reset and here,
the concept should be used without imposing too many risks.
  
\divkt
  Reserve-operations of \alib containers include the reservation of recyclable nodes.
  This can dramatically improve performance by reducing memory fragmentation and cache misses
  on the hardware level.
\divend   

\I{################################################################################################}
# 5. C++ Standard Library Container Types# {#alib_contmono_stdcontainers}

As quickly mentioned in chapter \ref alib_contmono_alib_prereq_stdcontalloc, namespace #alib::lang
introduces type \alib{lang;StdContainerAllocator}. This is independent of the inclusion of
either module \alib_containers_nl or \alib_monomem_nl in the \alibdist.

\par Please Read Now:
    To avoid redundancy, we would ask you to
    \alib{lang;StdContainerAllocator;read the reference documentation of this type now}, and then
    return here. This documentation also gives a quick sample code which already shows how
    \b StdContainerAllocator is used with a <c>std::vector</c>.

As seen in the sample, the type enables C++ standard container types to be used with class
\alib{monomem;TMonoAllocator}.
Some limitations apply in respect to <em>"node recycling"</em> which is discussed in the next
sections.


\I{################################################################################################}
## Support for 5.1 Std-Containers: Monotonic Allocation Without Recycling # {#alib_contmono_stdcontainers_strict}

For all uses cases that constitute monotonic allocation
\ref alib_contmono_intro_recycling "without the need of recycling internal container node objects",
template types that accept a <c>std::allocator</c> can be equipped with \alib{SCAMono}, which
specializes  \b %StdContainerAllocator to use monotonic allocation.

\note A sibling type definition exists, with \alib{SCAPool} using class
      \alib{monomem;TPoolAllocator;PoolAllocator}.
      This is a next allocator type that is introduced in the next chapter.

To use \b SCAMono, an understanding of the general concept of using <c>std::allocator</c> type
is helpful.

A standard container is declared by providing the allocator types \b alib::SCAMono<T> as the
allocator template typename. The contained object's type has to be repeated:

 \snippet "ut_stdcontainers.cpp"     DOX_MONOMEM_SCAMONO_DECL

With the definition of an instance of a container, an instance of the <c>std::allocator</c> type has
to be passed to the constructor of the container:

 \snippet "ut_stdcontainers.cpp"     DOX_MONOMEM_SCAMONO_DEF
                      
Note that we simply pass the "original" allocator type \b MonoAllocator here, which implicitly
constructs the \b StdContainerAllocator, aka \b SCAMono with the constructor call.

If an allocator used by a standard container is reset, the container object itself
has to be reset. 
Because the standard containers do not support such a reset, the simple way out of it is to:

- Call the container's destructor to free all resources.
- Perform a placement-new to reconstruct a fresh container object.

The documentation of class \alib{lang;StdContainerAllocator} provides a source code sample that
demonstrates this.

\note Of course, the container allocators likewise accept type \alib{monomem;TLocalAllocator}.

A next shortcut to using types \b SCAMono and \b SCAPool in combination with <c>std::vector</c> is 
given with alias \alib{StdVectorMono}. 
The type definition of the sample above then look like this:

 \snippet "ut_stdcontainers.cpp"     DOX_MONOMEM_SCAMONO_DEF_ALTERNATIVE
 
The list of sibling aliases provided are: 
- \alib{StdVectorMono}, \alib{StdVectorPool},
- \alib{StdListMono}, \alib{StdListPool}, and 
- \alib{StdDequeMono}, \alib{StdDequePool}.



\I{################################################################################################}
## 5.2 Std-Containers: Monotonic Allocation With Recycling # {#alib_contmono_stdcontainers_recycling}

The challenge of implementing \ref alib_contmono_intro_recycling "recycling" for the C++ standard container
types lies in the fact that the standard specification does not cover a container's implementation
but only its interface. Consequently, the internal node type is not determined. Even worse,
it is not even determined if the type allocates and de-allocates single node objects at all, or
if any other implementation strategy is followed.

Nevertheless, this \alibmod_nl provides type \alib{lang;StdContainerAllocatorRecycling} which performs
recycling. The problem is: <b>It is not guaranteed to work on each platform/toolset!</b>

The type internally uses \alib{lang;RTTRAllocator}, where \b RTTR stands for
run-time type recycler. This means: The type of recyclable node objects is only detected at
run-time. If such detection fails, in debug-compilations different \alib warnings are raised.

In practice, the type works with all implementations of the standard library known to the authors
of \alib. Nevertheless it has to be warned that switching the library/platform/tools
of a compilation's target platform, imposes the need to test the new tool chain.

For this reason it is generally recommended to use the container types this module provides instead
of using the standard types.

Being a platform agnostic library, of course \alib does not use type \alib{lang;StdContainerAllocatorRecycling}
anywhere internally.


\I{################################################################################################}
## 5.3. C++ 17 Polymorphic Memory Resources ## {#alib_contmono_stdcontainers_pmr}

With C++ language version \c 17, the standard library was extended by namespace <c>std::pmr</c>
which stands for <em>"polymorphic memory resource"</em>. The standard here primarily aims to allow
two containers that use different types of memory allocation, to be of the same type.

Included in this namespace is class
\https{std::pmr::monotonic_buffer_resource,en.cppreference.com/w/cpp/memory/monotonic_buffer_resource}
which introduces monotonic memory allocation to the standard and is comparable to class
\alib{monomem;TMonoAllocator}.

As of today, \alib does not provide a compatibility layer to entities found in <c>std::pmr</c>.
In future versions of \alib, it may be worthwhile to provide more specialized, built-in bridges to 
allow a tight integration with the <c>std::pmr</c> infrastructure.
A foreseeable first bridging entity would be the implementation of abstract interface type
\https{std::pmr::memory_resource,en.cppreference.com/w/cpp/memory/memory_resource} with using
class \b MonoAllocator, and vice versa, the implementation of template along the lines of
prototype \alib{lang::Allocator}, that internally uses a \b std::pmr::memory_resource.

If a user needs such layer between \alib and the C++ Library, such implementation
should be possible with a few lines of code only. Let's quickly sample this:

We need to implement a class that derives from <c>std::pmr::memory_resource</c> and forwards 
memory allocation and deallocation requests to \alib's allocator. 
Below is an implementation that demonstrates how to wrap \alib’s MonoAllocator.
                   
\snippet "ut_monomem.cpp"     DOX_MONOMEM_PMR_BRIDGE_MONOMEM
                                 
Here is a usage example of how to use the \b PmrMonoAllocator with a standard C++17 
<c>std::pmr::vector</c>.
                                                            
\snippet "ut_monomem.cpp"     DOX_MONOMEM_PMR_BRIDGE_MONOMEM_USAGE

Notes
- No deallocation: \b MonoAllocator doesn't deallocate individual objects until it is reset. 
  The <c>PmrMonoAllocator::do_deallocate()</c> method is left empty, as it’s unnecessary in this context.
- Compatibility: This approach works with any standard library container that accepts a polymorphic 
  allocator (<c>std::pmr::unordered_map</c>, <c>std::pmr::list</c>, etc.).

<p>                    
\divkt
  The library offers allocator as well as container types, which are not related to C++ 17 standards
  but can be used stand-alone. The designers of \alib have good reasons for this.<br>
  Of-course we appreciate the need to bridge \alib mechanics into those of C++ standards. 
  Therefore, the library provides those "bridge-types", that are considered reasonable in respect  
  to effort and simplicity, as built-in, ready-to-use tools.<br>
  Further efforts seem to be use-case specific and most probably not in the scope of \alib.<br>
  Hints and suggestions are always welcome!        
\divend   



\I{################################################################################################}
# 6. Class PoolAllocator# {#alib_contmono_poolallocator}

Type \alib{monomem;TPoolAllocator;PoolAllocator} is the next allocator that module \alib_monomem_nl
introduces.

\note In fact, likewise \b MonoAllocator is only a type definition into templated type
      \alib{monomem;TMonoAllocator}, \b PoolAllocator is only a type definition into templated type
      \alib{monomem;TPoolAllocator}. More on this topic
      is found in later chapter \ref  alib_contmono_chaining.<br>
      To keep it simpler, for now, we talk about type \b PoolAllocator, that uses
      a monotonic allocator internally.

\par Please Read Now:
    To avoid redundancy, we would ask you to
    \alib{monomem;TPoolAllocator;read the reference documentation of this allocator now}, and then
    return here.

What you have just learned about the pool allocator should remind you on previous chapters
\ref alib_contmono_intro_recycling and \ref alib_contmono_containers_recycling_private.
And yes, the \b PoolAllocator is nothing else but a proxy class in front of a \b MonoAllocator
that just recycles everything that it allocates.
As explained in the reference documentation, to reach this gaol an average of 25% allocation
overhead is taken into account. Well, there is no free lunch with programming, it is always a
tradeoff between speed and memory consumption. And speed is key here. The use of a pool
allocator has almost no impact on the pure allocation speed. When used correctly (with the
right use cases) the overall performance gain can be dramatic.

\divkt
Class <b>PoolAllocation</b> can be seen as an ultra fast proxy that can be put in front of 
another allocator. 
It efficiently implements recycling for any allocation size.<br>
It is especially useful for scenarios where many objects are created and destroyed frequently. 
\divend   

\I{------------------------------------------------------------------------------------------------}
## 6.1 Using Oversized Results ## {#alib_contmono_poolallocator_oversized}
This allocator type was the reason to make parameters \p{size} and \p{newSize} of prototype
methods \alib{lang;Allocator::allocate} and \alib{lang;Allocator::reallocate} in/out parameters.
These methods expect a reference to the \p{size} parameter, and \b PoolAllocator is the one that
in fact changes the value. This is because the requested memory size is rounded
up to the next higher power of 2. Some types might make use of the extra space. A prominent
sample is class \b %AString, which in its pool allocator version is addressed via type
definition \alib{AStringPA}.<br>
Like thousands of other string class implementations, also class \b AString increases an internal
buffer when things are appended to the string. For this, a new larger buffer is requested, the
current data is copied, and the old buffer is freed. The growth factor of C++ class
<c>std::string</c> is not specified, the same is true for \b AString. However, with type definition
\b AStringPA which uses a \b PoolAllocator (more on this in
\ref alib_contmono_further_strings "a later chapter") any exceeding memory returned by the pool
allocator will be added to the internal capacity member.

Apart from this, there is no other use case found in \alib (as of the time of writing this), because
most of the time, distinct memory sizes are requested and oversized returns are of no use.
Nevertheless it should be good to know and understand about this feature.

The high-level allocation methods available through \alib{lang;AllocatorInterface}
are missing this feature. They only accept values and will thus will never increase the given
sizes. This is a design decision: In case a change should be recognized, the low-level
methods \b allocate and \b reallocate have to be used.

Finally, if compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is set, then an acceptance of a higher
size than requested, has to be announced by calling method
\alib{lang;Allocator::dbgAcknowledgeIncreasedAllocSize}. Otherwise, the internal debug-checks
would (rightfully) report size mismatch errors when an object is freed.
More on this topic is given in chapter \ref alib_contmono_further_debug.
                              
\divkt
Class <b>PoolAllocation</b> may return larger memory objects than requested. 
While most use cases cannot benefit from it, those that can should make use of it! 
\divend   

\I{------------------------------------------------------------------------------------------------}
## 6.2 Using Allocation Information Bits with Freeing Objects  ## {#alib_contmono_poolallocator_allocinfo}
Besides the core interface defined with \alib{lang;Allocator} and the available high-level
interface \alib{lang;AllocatorInterface}, this class offers a specific allocation technique.
This is constituted by the following methods:
-  \alib{lang::PoolAllocator;AllocationInformationBitCount()},
-  \alib{lang::PoolAllocator;GetAllocInformation<typename TIntegral>();GetAllocInformation<T>()},
-  \alib{lang::PoolAllocator;GetAllocInformation<template T>(TIntegral size);GetAllocInformation(size)},
-  \alib{lang::PoolAllocator;AllocateByAllocationInfo} and
-  \alib{lang::PoolAllocator;FreeByAllocationInfo}.

\par Please Read Now:
    To avoid redundancy, we would ask you to read the reference documentation of the five methods
    listed above.

As it is quickly understandable, with deallocation the allocation size of the memory has
to be known by this allocator. While other allocator types may just ignore this parameter,
and function <c>std::free</c> does not even expect a size to be returned, this allocator produces
undefined behavior if wrong values are given.

In some use case scenarios, objects of different size are allocated and at the point of freeing
them, this size might not known to the part of the software that frees the objects.
In this case, one possibility is to store the allocation information with each allocated object.
As you learned from the reference documentation, such storage can be achieved by reserving just
6 bits, respectively 5 bits on 32-bit systems.

A next option could be to make allocated objects virtual and add a method that returns their 
size. 

A third way of coping with the issue of providing the allocation size when freeing memory
is given in the next chapter.
                    
\divkt
A C++ programmer is not used to pass the allocation size back when freeing memory, because for 
example, <c>std::free(void*)</c> does not have such parameter.<br>
Because, the \alib allocation model requires this information, this chapter and the next offer 
three solutions possible to solve the non-trivial cases. 
\divend   
\I{------------------------------------------------------------------------------------------------}
## 6.3 Using Meta-Info Singletons to Reclaim Allocation Information  ## {#alib_contmono_poolallocator_metatrick}
This is a hands-on chapter providing a recipe to implement safe and efficient use of class
\alib{monomem;TPoolAllocator} along with a concrete implementation. Instead of
introducing some artificial sample, we are looking at the internals of camp \alib_config, which
constitutes the access to external configuration variables with \alib.

\par Recycling with Class StringTree
Class \alib{config;Configuration} inherits container type \alib{containers;StringTree},
which in turn uses container type \alib{containers;HashTable} to store the string-based
paths to the elements as well as the element data itself. This architecture implements a nice sample
of a recycling technique: An unlimited number of insertions and removals of entries into a
\b StringTree allocates a number of hash-table entries that equal the maximum of elements in the
tree at any point in time during its life-cycle. Once this maximum is reached no further allocations
are made and the system is stable without touching the system heap, neither with allocations nor
with free operations.

\par Attaching Custom Variable Types
One point on the requirement list of this module was to allow the user of the API
to declare variables containing any type. Not only strings or unions of strings, integral
and floating point values, but real C++ custom types, with any number of fields, specific to an
application. For example, module \alib_alox stores structs like
\alib{lox::textlogger;FormatTimeDiff} directly in a configuration variable.<br>
With class \b StringTree all entries are of the same type (given as a template parameter to the
tree). Therefore, custom storage could only be implemented by adding a pointer to the custom type
to each tree entry. Now the question is: How are these custom types allocated, constructed,
destructed, and freed?

The explanation should be given as step-by-step recipe:
1. Class \c Configuration provides the method 
   \alib{config::Configuration;RegisterType<typename TVMeta>()}.
   This method does only take one single template parameter, and no run-time parameters.
   The template parameter has to specify a type that is derived from virtual abstract base type
   \alib{config;VMeta}.
2. The type registration method creates an instance of this type, and this is the only instance
   of this class ever created. It invokes a first virtual function, namely
   \alib{config;VMeta::typeName}. This is the name of the "variable type" that class
   \b Configuration just was acknowledged of. The returned name is used as a key for an internal
   hash map that carries all meta-information singletons of all registered types
   (see field \alib{config;Configuration::types}).
3. If you look at the \alib{config;VMeta;virtual interface of type VMeta}, the strategy
   already becomes clear, as we find methods \b size(), \b construct() and \b destruct().
   But let's continue to the end.
4. With any overloaded method that "declares" a configuration variable, the typeName is somehow
   provided. With that type name, a pointer to the meta-info singleton is received from the hash
   map and attached to the newly created string tree \alib{config::detail;Entry}.
5. A next field in the entry is a pointer to the data. Using the information given with virtual 
   method \alib{config;VMeta::size}, an appropriate piece of memory is retrieved from the pool 
   allocator. This memory is passed to the method \alib{config;VMeta::construct} and with that 
   the declaration of the variable is done.
   \note In theory this should be called a \b definition of the variable, not a declaration. 
         But camp \alib_config considers variables as defined only after at least a default 
         value was written from any variable source, even though from the C++ perspective, the 
         constructor of the custom type has run.
6. When a variable is deleted, \alib{config;VMeta::destruct} is called and then
   \alib{config;VMeta::size} is invoked again and its value passed to the pool allocator for
   freeing the object.

This approach imposes only one restriction to the custom types that are usable with run-time
variables offered by that \alibmod_nl: The alignment of the types must not exceed what is
specified as the fixed alignment used by the pool-allocator. Class \alib{config;Configuration}
uses the default-value of the template parameter \p{TAlignment}, which is defined by 
compiler symbol \ref ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT. 
If greater alignments are needed, the library and its using code entities have to be compiled with 
specifying this symbol. 
But common use-cases of configuration variables do not require greater alignments for custom types.

On a final note, when you look at the \alox type \alib{lox::textlogger;FormatTimeDiff} mentioned  
above, you will see that this class has several members of type \alib{AStringPA}. 
Consequently, this class needs a pool allocator at construction which is passed to each string 
member. 
This is, of course, again the pool allocator of the \b Configuration object. 
It is offered for custom use with methods \alib{config;VMeta::construct} and 
\alib{config;VMeta::destruct}.

We hope walking you through this real-world sample we have motivated you to consider using 
pool-allocation in future software designs. 
Compared to other allocation strategies, especially compared to using the system heap, the 
performance is unrivalled. 
And the field of applications is vast.

\I{################################################################################################}
# 7. Class TSharedMonoVal# {#alib_contmono_smv}
Module \alib_monomem_nl provides utility class \alib{monomem;TSharedMonoVal}. 
This class has two features, which are described in the next sections.

\I{################################################################################################}
## 7.1 Combining An Allocator With A Type## {#alib_contmono_smv_feature1}
Sets of objects that are eligible to be monotonically allocatable 
(see \ref alib_contmono_intro_definition "introduction chapter"), often at the same time  
do share their scope and life-cycle with a certain, rather prominent compound type. 

As a sample for this, consider class \alib{files;FTree} of module \alib_files: 
The type uses a \b StringTree, which is \alib{containers;StringTree;an extended form of a HashTable},  
to collect information about files in a filesystem. Such a tree may contain many entries, and some  
or all of such entries may even have some custom information attached. 
Altogether, a plethora of single allocations have to be performed when working with large \b FTree 
instances.<br>
The lifecycle of each entry in the tree ends with the lifecycle of the tree itself.<br>
While class \b FTree requires an allocator to be given with construction, which may be used for
other allocations as well, in most use-cases, it makes absolute sense to use a dedicated 
allocator just for the tree and delete this allocator with the disposal of the tree.     
     
Therefore, the first feature of class \b TSharedMonoVal is to combine a dedicated \b MonoAllocator
with a custom type. In contrast to just creating a compound type by adding an internal allocator
member, the approach is a little trickier: The custom type itself is placed inside the monotonic 
allocator. Furthermore - and this may take a moment to think about - the monotonic allocator 
itself is placed in this first buffer!

As a consequence, the "footprint" of class \b TSharedMonoVal (aka the result of 
<c>sizeof(TSharedMonoVal<CustomType>)</c>) is always the size of a single pointer. 
On 64-bit systems the size is \c 8 bytes, on 32-bit systems it is \c 4 bytes. 
This single pointer points somewhere inside the first buffer of the monotonic allocator where
both objects are placed.
                                                                                
\I{################################################################################################}
## 7.2 Sample Code## {#alib_contmono_smv_sample}
Before the second fundamental feature of class \b TSharedMonoVal is discussed in the next section,
a simple sample code should show how to use this class.  

A dictionary for translating single words from one language into another should be implemented. 
The dictionary is nothing but a simple \b HashMap assigning an input string to an output string:
  \snippet "ut_monomem.cpp"  DOX_MONOMEM_SMV_1
                                       
The \b HashMap is defined to use monotonic allocation and thus the constructor of class Dictionary 
expects such external allocator to be passed. This is how it would be created without using
class \b TSharedMonoVal:
    
  \snippet "ut_monomem.cpp"  DOX_MONOMEM_SMV_11

The recipe to moving this type into a \b TSharedMonoVal class is:
1. Derive a new class that inherits \b TSharedMonoVal passing the custom type \b Dictionary
   as its template argument.  
2. Add a constructor that constructs \b TSharedMonoVal, passing a reasonable buffer size and growth
   value and furthermore constructs the custom type. 
   The latter has to be done with a (mandatory) invocation of method 
   \alib{monomem::TSharedMonoVal;ConstructT}.      

That is all that is needed and in the sample looks like this:

  \snippet "ut_monomem.cpp"  DOX_MONOMEM_SMV_2
                                                                                        
\note 
  A default constructor and one taking \b nullptr is available in parallel, both not specifying the
  allocator size. The rationale and use of these will be explained in the next chapter.  
  
Now the instantiation of the type does not need an external \b MonoAllocator anymore:

  \snippet "ut_monomem.cpp"  DOX_MONOMEM_SMV_3

To access the members of the original \b Dictionary class, overloaded operator 
\alib{monomem::TSharedMonoVal;operator->()} is to be used:
 
  \snippet "ut_monomem.cpp"  DOX_MONOMEM_SMV_4

The functionality to \alib{monomem;TMonoAllocator;reset a MonoAllocator} is extended with 
type \b TSharedMonoVal to destructing the contained object, resetting the allocator, and 
re-constructing the contained type again. Consequently, this method expects the 
parameters necessary for (re-)construction of the contained type:
             
  \snippet "ut_monomem.cpp"  DOX_MONOMEM_SMV_RESET

\I{################################################################################################}
## 7.3 Automatic Pointer ## {#alib_contmono_smv_feature2}
As elaborated in the previous sections, class \b TSharedMonoVal makes only one single
(heap) allocation and packs the \b MonoAllocator as well as the custom type in the front 
of this first buffer. Furthermore, the only member that the type has is a pointer into its
allocator's first buffer, and thus the type has a minimal footprint equalling <c>sizeof(void*)</c>. 
Of course, this makes the type attractive to be copied to other places and shared between code
entities.

Consequently, the type received a next feature which is very similar to what C++ standard library
type <c>std::shared_ptr</c> offers, respectively exactly the same what \alib type 
\alib{containers;SharedVal} offers.<br> 
Technically spoken, together with the \b MonoAllocator and the custom type \p{T}, in addition, an 
atomic reference counter is embedded.
Along with the provision of corresponding copy and move semantics, the type offers methods:
- \alib{monomem::TSharedMonoVal;UseCount}, and
- \alib{monomem::TSharedMonoVal;Unique}.

This explains, why we had equipped the sample type \c SharedDictionary with a default constructor, 
that is omitting the parameter that determines the size of the \b MonoAllocator's initial buffer.
If the class is default constructed, nothing is done and the internal pointer is simply set to 
\c nullptr. Secondly, we added a constructor taking \c nullptr. This allows assigning \c nullptr
to values of that type. 

The following further excerpt from the unit-tests demonstrates this and should be all that is
needed for understanding how it works: 
                                                
  \snippet "ut_monomem.cpp"  DOX_MONOMEM_SMV_SHARED

\I{################################################################################################}
## 7.3 Naming Convention ## {#alib_contmono_smv_naming}

Across \alib, two schemes for naming the "inner" type and the type derived from \b %TSharedMonoVal
exist.
The choice depends on whether the "inner" type is commonly used without being encapsulated 
in the outer type. 

\par Scheme One
  This scheme is applied if using the \b TSharedMonoVal is only an option and it is common to 
  use the underlying type directly. Here, the inner type keeps its "natural" name and the 
  derived type is prefixed with <em>"Shared"</em>.<br>
  This scheme was used in the sample code of the previous section.
\par    
  Within \alib, samples are:
  - Classes \alib{files;FTree} and \alib{files;TSharedFTree}, and   
  - classes \alib{config;Configuration} and \alib{config;TSharedConfiguration}.  
               
\par Scheme Two
  This scheme is applied if using the type directly is rather \b not an option and instead, the
  \b TSharedMonoVal is exposed by the API exclusively, or its use is at least strongly recommended.<br>
  The scheme suggests to add postfix <em>"Val"</em> to the inner type name and giving the 
  derived type the "natural" name.
\par    
  Within \alib, samples are:
  - classes \alib{expressions;ExpressionVal} and \alib{expressions;Expression}.   
  - Classes \alib{lang::detail;ExceptionEntry} and \alib{lang;Exception}.<br>
    Well, this is already a slight exception to the rule: class \b %Exception holds a linked list 
    of values, hence here, <em>"Entry"</em> was chosen as the postfix instead of <em>"Val"</em>.   

\I{################################################################################################}
## 7.4 Locking Shared Values ## {#alib_contmono_smv_locking}

Module \alib_threads_nl provides comfortable support for locking critical sections against
racing-conditions in multithreaded software. 
In short: 
- The six lock types of the C++ standard library have 
  \ref alib_threads_locks "corresponding ALib wrappers" which are equipped with debug-information
  and assertions.
- Class \ref alib_threads_locks_auto "Owner and its sibling types" allow automatic acquisition and 
  release of locks.
- Macros \ref ALIB_LOCK_WITH and \ref alib_threads_locks_auto_macros "its sibling definitions", 
  allow to prune all lock operations in case module \alib_threads_nl is not included in an 
  \alibdist and thus multithreading support is willingly suppressed.  
                    
For details consult that module's \ref alib_mod_threads "Programmer's Manual".

It is evident that shared values often need to be accompanied by a corresponding mutex.
To provide and share an instance of such lock right with the shared value itself, 
class \alib{monomem;TSharedMonoVal} provides template parameter \p{TLock}.
While it defaults to <c>void</c>, which indicates not to include a lock, it can be set to 
any of the six \alib lock types. 
If so, not only a corresponding instance of a lock is included, but furthermore the 
specific interface methods of the lock type given become available as corresponding interface 
methods.<br>
As a result of having the lock's interface methods directly invocable class \b TSharedMonoVal,
the \b Owner types and the set of locking macros can all be applied conveniently to instances of 
the class.<br> 

Here is a quick sample snippet that shows this with type \alib{files;TSharedFTree}:

\snippet "DOX_FILES.cpp"     DOX_FILES_LOCK
                              
\note The sample uses type alias \alib{SharedFTree} in namespace #alib, which hard-codes 
      template parameter \p{TLock} (which is remaining in original type #alib::files::TSharedFTree)
      to either \alib{threads;SharedLock} or <c>void</c>.<br> 
      The choice is made in dependence of the inclusion of module \alib_threads_nl.<br>
       
With debug-compilations and furthermore compiler symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS set,
the integrated lock is automatically attached to the instance of \alib{lang;DbgCriticalSections}
that is embedded in the underlying \b StringTree of contained class \alib{files;FTree}.
With that, (often) assertions are raised in case the instance is used without prior acquisition.<br>
If parts of the using code are protected otherwise and thus do not need the lock to be set
in addtion, the assertions can be switched on and with the method 
\alib{files;TSharedFTree::DbgCriticalSections}.

\note Details on the topic of associating critcal sections with a lock can be found in manual 
      chapter \ref alib_threads_intro_assert_locks of the Programmer's Manual of module 
      \alib_threads. 

The very same approach was taken with class \alib{config;TSharedConfiguration} and its alias
type \alib{SharedConfiguration}.   


\I{################################################################################################}
# 8. Allocator Chaining # {#alib_contmono_chaining}

\I{################################################################################################}
# 8.1 Terminology # {#alib_contmono_chaining_intro}
Any allocator - at the end of the day - needs to make calls to the operating system to allocate
<em>"heap memory"</em>, because this is the one and only \e true memory source. 
Even class \alib{monomem;TLocalAllocator} uses <em>stack-memory</em> only for its first buffer. 
Once this buffer is full, the next buffer has to come from the heap.

Now, instead of hard-coding the underlying memory source to be heap memory, the 
\alib allocation architecture allows what we call <em>"allocator chaining"</em>. 
For this, each allocator itself receives a template parameter named \p{TAllocator}, which is
exposed as a type definition \b ChainedAllocator, as with:
- \alib{lang;HeapAllocator::ChainedAllocator} (defined as \c void),
- \alib{monomem;TMonoAllocator::ChainedAllocator}, and 
- \alib{monomem;TPoolAllocator::ChainedAllocator}. 

The allocators implemented with \alib, allow access to the chained allocators through           
the publicly inherited interface of type \alib{lang::AllocatorMember}, namely methods
- \alib{lang::AllocatorMember;GetAllocator}, and 
- \alib{lang::AllocatorMember;AI}.

  

\I{################################################################################################}
# 8.2 Default Chains # {#alib_contmono_chaining_default}
The type definition #alib::MonoAllocator hard-codes template parameter \p{TAllocator} to
type \b HeapAllocator:

        TMonoAllocator<HeapAllocator> 
 
Because this "chain" covers most use cases of monotonic allocation, you could name this the 
"natural choice".

A similar natural choice for the designers of this module was to set type \b MonoAllocator 
as the source for type definition \alib{PoolAllocator}: 

        TPoolAllocator<MonoAllocator> 
        
Resolving this shows that \b PoolAllocator in fact equals type:

        TPoolAllocator<TMonoAllocator<HeapAllocator>> 
                                                      
The answer to the question of why this decision can be called a <em>"natural choice"</em> is easiest 
given by thinking about the alternative: Why should a pool allocator use \b heap memory directly?
A pool allocator is a perfect recycler and as this allocator never frees memory until it is
destructed, here recycling turns any use case into a monotonic allocation case.

Because a user still might wish to have a pool allocator chained directly to \b HeapAllocator,
the library provides a second alias type with \alib{PoolAllocatorHA}, which is defined as
 
        TPoolAllocator<HeapAllocator> 
 
\I{################################################################################################}
# 8.3 Instantiating Other Combinations # {#alib_contmono_chaining_other}
Two chains that are not instantiated with the library and thus would cause linker errors 
when used, are:  

        TMonoAllocator<TPoolAllocator<HeapAllocator>>
        TMonoAllocator<TPoolAllocator<TMonoAllocator<HeapAllocator>>>
        
Still these chains could be valuable in complex scenarios.                                                   
\note If you do not believe that such chains can be useful, consider the creation and destruction
      of plenty of \alib{lang::Exception} instances, for example in a multithreaded server 
      application. Each exception uses a monotonic allocator of 1kb to collect exception messages
      (see detail chapter \ref alib_basecamp_message_exception_smv on \alib exceptions).  
      
      
Another scenario where instantiations of other chains may come handy is when a user of the library
introduces an own type that implements the prototype \alib{lang;Allocator}. 
A user would do this with the intention to chain it with \alib allocators (why else would 
he or she be using this prototype?).  

The \alib source file layout allows instantiating the built-in allocators for different chains.
The explanation and a source-code sample that concretely shows how to instantiate type: 

        TMonoAllocator<TPoolAllocator<HeapAllocator>>
        
is given in the general \alib manual, because the recipe applies to other template types of 
\alib as well.<br> 
On interest, consult chapter \ref alib_manual_appendix_t_hpp_files.  
                
\divkt
 - \alib allows allocators to be chained, where one allocator can delegate memory requests to another. 
   This provides flexibility to compose different allocation strategies.
 - Common default chains include \b MonoAllocator using the \b HeapAllocator, and secondly 
   \b PoolAllocator using \b MonoAllocator. The latter allows efficient memory recycling with 
   minimizing system allocations. 
\divend   

\I{################################################################################################}
# 9. Thread-Safeness And Strategies of Sharing Allocators # {#alib_contmono_threadsharing}

\I{################################################################################################}
## 9.1 Thread-Safeness ## {#alib_contmono_threadsharing_sf}

This chapter can be made brief: There is no built-in protection against racing conditions
caused by parallel allocations invoked by different threads.
Consequently, any mechanism of protection has to be performed by the using code.

With special compiler symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS set, which is enabled by default
with the inclusion of module \alib_threads_nl in the \alibdist_nl, allocators will add code that 
checks the exclusive use of critical sections. If those checks raise an assertion, the using code
needs to add locks to prevent racing conditions in multithreaded software. 

While this is in alignment with all low-level \alib entities (and with the usual designs of low-level
libraries), programmers are very much used that the <c>new/delete operators</c>, respectively calls
to <c>std::malloc/free</c> are thread-safe. This is why this quick chapter hints to that fact that
using \alib allocators, these calls are not thread-safe.

Besides using protection mechanisms, avoiding race conditions can be achieved by using different
instances of allocators in different threads. Thus, the design decision about which parts of a
software share the same allocator, and which parts dispose of an own instance, finally decides
about the necessity of protecting the allocators from parallel access.

For the latter reason, this small manual section was placed together with the next section
into one chapter.

\I{################################################################################################}
## 9.2 Sharing Allocators - Or Not ## {#alib_contmono_threadsharing_sharing}

In case of multi-threading, things become even more complex and a well-thought design is key.
On the other hand, the performance benefits of using monotonic allocation, for various reasons
may also become very huge in multi-threading applications.

An expected multi-threading use of a software entity can be a driver for both: either for sharing an
allocator or for creating dedicated allocators used by specific threads. 
It is use-case-dependent and cannot simply be used as a guideline to decide about it.
 
The feature of taking \ref alib_contmono_class_monoalloc_snapshot "snapshots of a MonoAllocator"
again increases the danger of undefined behavior of malicious code, but on the other hand
can dramatically improve performance. Once more, the combination with multithreaded access
further increases such danger and complexity.

Similar concerns apply to the built-in recycling features and class 
\alib{monomem;TPoolAllocator;PoolAllocator}.

An alternative to creating a local snapshot which might impose the need to protect the code
section against racing conditions, might be the use of class
\alib{monomem;TLocalAllocator;LocalAllocator} instead. 
This alternative should be kept in mind, because the huge benefit may be that stack memory is 
already cached. 
Depending on the use-case however, the currently used buffer of a \b MonoAllocator in use, 
might also be already cached on the hardware level.
<p>

\divkt
 - Unlike <c>std::malloc/free</c>, \alib allocators are not inherently thread-safe, and users need to 
   manage synchronization themselves.<br>
 - While allocators can be shared between threads, it’s often more efficient to give each thread 
   its own instance of an allocator.
 - Compiler symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS enables support for debugging racing conditions. 
 \divend   


\I{################################################################################################}
# 10. The Global MonoAllocator # {#alib_contmono_globalinstance}

\I{################################################################################################}
## 10.1 Permanent Allocations ## {#alib_contmono_globalinstance_permanent}
In the case that the life-span of an object set that is suitable for monotonic allocation
equals the complete life-span of an application itself, the global allocator singleton
\alib{monomem;GLOBAL_ALLOCATOR} may be used for allocations. <br>
Because, this allocator is deemed to be shared across many compilation units, its use has to be 
protected against racing conditions of multithreaded access by acquiring 
\alib{monomem;GLOBAL_ALLOCATOR_LOCK} before use. 

This is most conveniently done by using macro \ref ALIB_LOCK_RECURSIVE_WITH. 
With single-threaded applications, the allocator's lock is not present and thus has to be 
ignored, which is automatically done with that macro.

With multithreaded applications, it has to be acknowledged that the global instance has to
be locked whenever it is used. 
If during acquisition, the global instance is, for example, passed to one of the container types, 
then mutex \alib{monomem;GLOBAL_ALLOCATOR_LOCK} has to be acquired whenever this container type is 
potentially allocating memory. 
This consequence can be "overseen" easily. 
Also note, that several \alib modules make use of this allocator instance also \b after 
bootstrapping. 
For example, when new instances of classes \alib{threads;Thread} and \alib{lox;Lox} are registered,
or when class \alib{lang::system;Path} detects default directories for the first time.

For this reason, with debug-compilations, it is asserted that the global allocator is duly locked.
Further details are given in chapter \ref alib_threads_intro_assert_locks of the 
Programmer's Manual of module \alib_threads. 

\I{################################################################################################}
## 10.2 Snapshots On The Global MonoAllocator ## {#alib_contmono_globalinstance_snapshot}
A second case for using the global allocator singleton is using it in combination with
\ref alib_contmono_class_monoalloc_snapshot "snapshots". If sets of monotonic allocatable objects
of short life-cycle, may use the allocator singleton and reset it to its previous state afterwards.

The advantage of doing so is that memory buffers allocated by the allocator during the "nested"
monotonic allocation, are reused with a next use. 
This furthermore reduces the overall number of dynamic allocations.

Apart from this advantage, the disadvantage may be huge: In multithreaded applications, the
global instance has to remain locked until it is reset to the snapshot. Furthermore, it has to be
ensured that no other code invoked is using the global instance, as resetting the global instance
would invalidate any allocation made since the snapshot was taken. 
Errors resulting from such behavior might be challenging to debug.

As a conclusion, this use pattern is not recommended unless a programmer knows exactly what
and why she is doing it.

\I{################################################################################################}
## 10.3 Changing the Default Size ## {#alib_contmono_globalinstance_initialsize}
The global instance is constructed using special constructor 
\alib{monomem::TMonoAllocator;TMonoAllocator(const char*, std::nullptr_t)}, which does not allocate a 
first buffer and produces an invalid instance. At the beginning of the 
\ref alib_manual_bootstrapping "bootstrapping process" method 
\alib{monomem::TMonoAllocator;IsInitialized} is used to check if the allocator was rightfully 
initialized already. If not, a placement-new is performed, passing an initial size of \c 128 
kilobytes and a growth factor of \c 200.

Now, if a user wants to change the initial size and/or the growth factor, all that is needed to
be done ist to perform such placement-new before bootstrapping. The code could look like 
follows:
\snippet "samples/Text/sample.cpp"        DOX_MONOMEM_GLOBALALLOCATOR_CHANGE_DEFAULT         

       

\I{################################################################################################}
# 11. Further Features # {#alib_contmono_further}

\I{################################################################################################}
## 11.1 Alignment ## {#alib_contmono_further_alignment}

While it is not specified by the C++ language, as a prerequisite, let's list some alignments 
of types as used with common toolchains:
                                                                  
Type                  | Alignment on 64-bit systems | Alignment on 32-bit systems 
----------------------|-----------------------------|---------------------------------
char                  |   1                         |    1
void*                 |   8                         |    4
int64_t               |   8                         |   4 or 8
std::max_align_t      |  16                         |   8

Again, these values may be different with other compilers/systems but already, when comparing
\b GCC with \c MSVC under 32-bit architecture, a difference is noticed for type <c>std::int64_t</c>. 

Function <c>std::malloc</c> does not take an alignment parameter and always aligns to
<c>alignof(std::max_align_t)</c>. If a higher alignment is required, a programmer needs to
allocate a larger memory piece and increase the start of the memory used in own responsibility.

In contrast to this, the interface functions defined with \alib allocators, which are prototyped by 
(non-existing) type \alib{lang::Allocator}, allow the alignment-specification of the allocated memory.
But attention: This parameter is not necessarily respected and cannot be freely chosen in all
cases. 
  
Instead, following rules apply:

- Type \alib{lang;HeapAllocator}, which is a simple wrapper around <c>std::malloc</c> and
  <c>std::free</c>, ignores the parameter and always align to <c>alignof(std::max_align_t)</c>.
- Type \alib{monomem;TMonoAllocator}, duly respects the parameter and does not limit
  the alignment in any other way than that it has to be a power of two. 
- Type \alib{monomem;TPoolAllocator} ignores the parameter and uses the fixed 
  alignment that is given with its template parameter \p{TAlignment}.
  This parameter defaults to compiler symbol \ref ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT, 
  and this symbol in turn defaults to <c>alignof(void*)</c>.<br>
  The type definition \ref alib::PoolAllocator, which is used across other modules of \alib, uses 
  this default value. This has some implications at the moment that a library (or code unit that is 
  not under control of a programmer) fixes the alignment to a value that is too low to allocate
  a user's type. 
  As good example is the restriction concerning the alignment of custom types that are to be stored 
  in instances of class \alib{config;Variable} provided by module \alib_config. 
  (See chapter \ref alib_config_types_custom of the Programmer's Manual of module 
  \alib_config for more information.)<br>
   
  While the maximum value of template parameter \p{TAlignment} depends on the chained allocator,    
  for technical reasons, passing a value lower than <c>alignof(void*)</c> to template 
  parameter \p{TAlignment} is not allowed.

Whenever possible, static assertions are raised if illegal values are given. 
In debug-compilations \alib assertions are raised when unsupported alignments are requested.
     
  

\I{################################################################################################}
## 11.2 String Allocations ## {#alib_contmono_further_strings}

### 11.2.1 Simple String Copies ### {#alib_contmono_further_strings_copy}
String data is often assembled (e.g., using a \alib{lang::format;Formatter}) and the character
buffer for this sometimes need to be allocated for later use. The simplest approach to this
is method \alib{strings;TString::Allocate} (or corresponding constructor
\alib{strings;TString;TString(TAllocator& allocator, const TString<TChar>&)}), which is templated
and thus accepts any \alib{lang;Allocator}. Besides allocating the character array, these
methods also copy the given source to it.

However, some attention has to be given to the fact that only class \alib{strings;TAString;AString} 
manages allocated buffers.
In contrast, class \b String does not know - and thus does not care - about whether explicit 
allocation was used. Therefore, on destruction the memory has to be explicitly freed by using
software. Still the use of type \b String can have performance benefits over \b AString.     

### 11.2.2 Class AString ### {#alib_contmono_further_strings_astring}
<b>ALib</b>'s string buffer class \b %AString is only called "AString" everywhere for better
readability. In fact it is a type definition providing both template parameters to
the "real" string buffer type \ref alib::strings::TString<TChar, TAllocator>. Besides choosing
the standard character size, type definition \alib{AString} chooses heap allocation.

With the inclusion this module \alib_monomem in the \alibdist, further type definitions for
\b %MonoAllocator and \b %PoolAllocator become available. The following table gives an overview
of the different combinations of character and allocator types. Note that the alias type definitions
themselves are made in outer namespace #alib.

Name                       |  Allocator Type      | Character Type
-------------------------- |----------------------|------------------
\alib{AStringMA}           | \alib{MonoAllocator} | \alib{character}
\alib{AStringPA}           | \alib{PoolAllocator} | \alib{character}
\alib{ComplementAStringMA} | \alib{MonoAllocator} | \alib{complementChar}
\alib{ComplementAStringPA} | \alib{PoolAllocator} | \alib{complementChar}
\alib{StrangeAStringMA}    | \alib{MonoAllocator} | \alib{strangeChar}
\alib{StrangeAStringPA}    | \alib{PoolAllocator} | \alib{strangeChar}
\alib{NAStringMA}          | \alib{MonoAllocator} | \alib{nchar}
\alib{NAStringPA}          | \alib{PoolAllocator} | \alib{nchar}
\alib{WAStringMA}          | \alib{MonoAllocator} | \alib{wchar}
\alib{WAStringPA}          | \alib{PoolAllocator} | \alib{wchar}
\alib{XAStringMA}          | \alib{MonoAllocator} | \alib{xchar}
\alib{XAStringPA}          | \alib{PoolAllocator} | \alib{xchar}

The aliases associated with a \b %MonoAllocator are declared in header
\alibheader{monomem/aliases/astringma.hpp}, those associated with \b %PoolAllocator in header
\alibheader{monomem/aliases/astringpa.hpp}.

For cases where the initial buffer size is not known, but an average or maximum size is, an alternative
is to emplace an instance of class \alib{strings;TLocalString;LocalString} inside
monotonic memory. The effect of doing so is that the internal reserved buffer is monotonically
allocated together with the object itself. If the buffer is exceeded, dynamic allocation is
used.<br>
It is good practice to use \b type LocalString (for example \alib{String32} with container types.
They can be used for attributes of the contained value the same as for key-attributes (for example
in case of \c HashTable). Their fixed (templated) internal buffer size has to be chosen carefully
depending on the data that the strings are supposed to hold. The goal here is to find a good
trade-off between minimizing the average allocation overhead and minimizing the number of
occasional dynamic allocations caused by buffer exceeds.
        
Corresponding aliases using different character and allocator types are given:

Name                           |  Allocator Type      | Character Type
------------------------------ |----------------------|------------------
\alib{LocalStringMA}           | \alib{MonoAllocator} | \alib{character}
\alib{LocalStringPA}           | \alib{PoolAllocator} | \alib{character}
\alib{ComplementLocalStringMA} | \alib{MonoAllocator} | \alib{complementChar}
\alib{ComplementLocalStringPA} | \alib{PoolAllocator} | \alib{complementChar}
\alib{StrangeLocalStringMA}    | \alib{MonoAllocator} | \alib{strangeChar}
\alib{StrangeLocalStringPA}    | \alib{PoolAllocator} | \alib{strangeChar}
\alib{NLocalStringMA}          | \alib{MonoAllocator} | \alib{nchar}
\alib{NLocalStringPA}          | \alib{PoolAllocator} | \alib{nchar}
\alib{WLocalStringMA}          | \alib{MonoAllocator} | \alib{wchar}
\alib{WLocalStringPA}          | \alib{PoolAllocator} | \alib{wchar}
\alib{XLocalStringMA}          | \alib{MonoAllocator} | \alib{xchar}
\alib{XLocalStringPA}          | \alib{PoolAllocator} | \alib{xchar}

The aliases associated with a \b %MonoAllocator are declared in header
\alibheader{monomem/aliases/localstringma.hpp}, those associated with \b %PoolAllocator in header
\alibheader{monomem/aliases/localstringpa.hpp}.

Furthermore type definitions corresponding to \alib{String8}, \alib{String16}, \alib{String32}...,
are given in these headers with: 
- \alib{StringMA8}, \alib{StringMA16}, \alib{StringMA32} ... \alib{StringMA4K}, and 
- \alib{StringPA8}, \alib{StringPA16}, \alib{StringPA32} ... \alib{StringPA4K}. 
 

### 11.2.3 Class StringVector ### {#alib_contmono_further_stringvector}
Class \alib{strings;util::TStringVector} is a utility type of module \alib_strings which is
templated with an allocator.

With the inclusion of header \alibheader{monomem/aliases/stringvector.hpp}, the following
type definitions become available:w  \alib{StringVectorMA}, \alib{StringVectorPA},
\alib{NStringVectorMA}, \alib{NStringVectorPA}, \alib{WStringVectorMA}, and \alib{WStringVectorPA}.

\I{################################################################################################}
## 11.3 Class Boxes ## {#alib_contmono_further_boxes}
Class \alib{boxing;TBoxes} constitutes a <c>std::vector</c> of elements of type \alib{boxing;Box}.
Definition of types \alib{BoxesMA} and \alib{BoxesPA} are made with the inclusion
of header \alibheader{boxing/boxing.hpp} already. This is done via forward declarations of types
\b MonoAllocator and \b PoolAllocator, even if module \alib_containers_nl is not included in the  
\alibdist.

\I{################################################################################################}
## 11.4 Debugging ## {#alib_contmono_further_debug}

With debug-compilations and symbol \ref ALIB_DEBUG set, the following mechanics features become 
available: 
- Common assertions are activated in the sources of this module.
- Method \alib{monomem;TMonoAllocator::DbgLock} is activated. If \c true is passed, the allocator
  will assert with further allocations. This is useful in more complex scenarios, for example,
  to assert that no other code entities perform allocations, when a snapshot was taken and
  a \alib{monomem::TMonoAllocator;Reset} is scheduled.<br>
  Another use case is sampled with module \alib_expressions. Here, two "scope" instances are
  established, a compile-time and an evaluation-time scope. Once an expression is compiled, 
  the allocator of the compile-time scope must not be used, because of the multithreaded design
  interaction. To assert that a using code does not accidentally use the wrong scope for 
  allocations, the allocator of the compile-time scope is locked.
- Method \alib{monomem;TMonoAllocator::DbgDumpStatistics} becomes available.        
   
With special compiler symbol \ref ALIB_DEBUG_MONOMEM, the following features are activated:
- Method \alib{monomem;TMonoAllocator::DbgGetStatistics} becomes available and method
  \alib{monomem;TMonoAllocator::DbgDumpStatistics} will collect more metrics than before.         
- A warning in the destructor of class \alib{monomem;TMonoAllocator} raised if more than <b>15</b>
  memory buffers have been created. This might indicate that the allocation size of an allocator
  should be raised.
- A warning in the destructor of class \alib{monomem;TMonoAllocator} raised if memory has been
  requested, that is bigger than the half of the allocator's actual allocation size.
- Various \alox debug-logging into domain <b>/ALIB/MONOMEM/</b> and Subdomains is performed.
- With class \b TMonoAllocator, the following features become available:
  - Method \alib{monomem;TMonoAllocator::DbgGetStatistics}
  - Method \alib{monomem;TMonoAllocator::DbgDumpStatistics}
  - An \alib warning is raised if an allocation request exceeds
    \alib{monomem::TMonoAllocator::nextBuffersUsableSize}.

With special compiler symbol \ref ALIB_DEBUG_CONTAINERS, the following features are activated
for class \b HashTable:
  - Function \alib{containers;DbgGetHashTableDistribution}.
  - Function \alib{containers;DbgDumpDistribution}.
  - Function \alib{containers;DbgDumpHashtable}.


With special compiler symbol \ref ALIB_DEBUG_ALLOCATIONS, the following features are activated:
- Allocated memory of any allocator (\alib{lang;HeapAllocator}, \alib{monomem;TMonoAllocator} and
  \alib{monomem;TPoolAllocator;PoolAllocator}) is surrounded by "magic bytes", which are tested to
  be intact with <b>free</b>-methods. This can be also manually checked with
  \alib{lang;Allocator::dbgCheckMemory}.<br>
  Besides the magic pattern, the allocation size passed to methods \alib{lang::Allocator::free}
  is checked to equal the size that was requested with allocation, respectively with the last
  re-allocation.
  \attention
    As explained in this manual, in/out parameter \p{size} of \alib{lang;Allocator::allocate} and
    \alib{lang;Allocator::reallocate} can be read back by the caller to acknowledge that the
    returned memory is larger than expected.
    If such acknowledgement is performed, and if the increased size is stored by the using code,
    and if this size is then passed back to the allocator on de-allocation, an assertion will be
    raised! This is due to the fact, that with \ref ALIB_DEBUG_ALLOCATIONS set, the originally
    requested size of an object is stored and compared to what is given with de-allocation.
    To mitigate this problem, in case an extended size is acknowledged, method
    \alib{lang;Allocator::dbgAcknowledgeIncreasedAllocSize} has to be called after allocation (respectively prior
    to de-allocation).<br>
    A code sample for this is found in the sources of \alib, with the implementation of type
    \alib{strings;TAString;AString}.
- Freed memory of any allocator will be overwritten (invalidated). This ensures that illegal
  access will be "detected" (by undefined behavior) as soon as possible.<br>
  Likewise memory which is released with method \alib{monomem;TMonoAllocator::Reset},
  is overwritten to invalidate existing data.

Finally, with special compiler symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS set, which is enabled by default
with the inclusion of module \alib_threads_nl in the \alibdist_nl, the following features are activated:
- allocators will add code that checks the exclusive use of critical sections. 
  If those checks raise an assertion, the using code needs to add locks to prevent racing conditions 
  in multithreaded software.
- It is asserted that mutex \alib{monomem;GLOBAL_ALLOCATOR_LOCK} is duly acquired whenever the 
  \alib{monomem;GLOBAL_ALLOCATOR} is used. 
  The assertion is omitted during bootstrapping of \alibmods_nl, precisely, it is only activated at 
  the end of bootstrapping.
  
              
\I{################################################################################################}
## 12. Wrap-Up ## {#alib_contmono_wrapup}

The set of allocators introduced here, together with the "recycling container types", provides a 
well chosen balance of flexibility, simplicity, and performance for a broad range of use cases.<br> 
By leveraging chaining, recycling, and snapshotting, most performance-critical memory allocation 
scenarios can be handled, without needing to introduce additional, more complex allocation 
strategies.

If your project is designed to serve general-purpose C++ development, especially in environments 
where performance and control over memory allocation are critical (e.g., game development, 
real-time systems, server-side applications), then types  \b MonoAllocator, \b LocalAllocator, and 
\b PoolAllocator offered, should be more than sufficient.

                      
The fact that this Programmer's Manual needed quite lengthy introduction chapters and contains 
many general knowledge sections, explains why alternative allocation strategies are not
often found in programming languages.<br> 
Even the C++ language only in their C++ 17 level, received valuable and consistent types in that 
area 

From our perspective, the mechanisms that these two combined \alibmods provide, 
- go quite beyond the C++ standard's approach,
- are more efficient (e.g., because is templated and avoids virtual types and method calls), and
- is more convenient to use.

It remains to notice: If the allocation models are chosen rightfully, the performance gain can be 
enormous for many applications.  
"Bigger" software, like databases, web-servers, CAD systems, etc., in their
development evolution, <b>have all come to the point of using this concept!</b>
Due to the long-term lack of standards, mostly by implementing their own tools and strategies.

<br><br><br><br><br><br> */
