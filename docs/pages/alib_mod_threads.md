// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_threads   ALib Module Threads - Programmer's Manual

\tableofcontents
      
\I{################################################################################################}
# 1. Introduction # {#alib_threads_intro}

The fundamental two topics that a class library dedicated "multithreaded programming" adress are:
1. Launching and managing threads, and
2. protecting data against corruption caused by uncontrolled concurrent access 
   (racing conditions on critical sections).
    
And this is what this small foundational \alibmod provides. 
All types rely on the C++ standard libraries for thread management, and in fact are not much 
more than wrappers. 
What is added is mainly debug-features, which disappear in release compilations.<br>
The classes are quite aligned with the types found in the class libraries of the Java language
in their early versions. 

\note \alib provides sophistic threading support is provided with a higher-leve module called
      \alib_threadmodel.
      
Besides the provision of these two fundamental requirements, this module as acts as a code
selector for multithreading support of \alib in general. This is explained in the next section.

# 1.1 The Special Role Of This Module  # {#alib_threads_intro_codeselector}
One of \alib's design targets is modularity, and a user is free to compile a reduced library, 
containing only a subset of the modules available, picking the things a user is interested in and
skipping modules they are not.

With this module \alib_threads_nl it is a little different. 
Therefore, we give a red signal for this rule:

\attention
  As an exception to the rule, module \alib_threads_nl has to be chosen to be included
  in an \alibdist, if a project uses multithreading. 
  Thus, the choice is independent of a user's wish to use the types found in this module!
  In other words: A user might use any other library to create and manage threads. 
  Nevertheless, as soon as parallel calls into \alib are made, this module has to be included.

In the case this module is used within a very limited \alibdist, please note the explanations found
in the \ref alib_manual, chapter \ref alib_manual_bootstrapping_nocamps.
In short, it has to be ensured that namespace functions #alib::threads::Bootstrap and
#alib::threads::Bootstrap are duly called with the start, respectively at the end of an application,
which is ensured if the guidelines given in the chapter linked above are followed.   

   
# 1.2 Threading-Agnostic Software  # {#alib_threads_intro_agnostic}
The term used in the headline of this section needs to be explained: As a general-purpose library,
\alib aims to support both use with single threaded and multithreaded environments. 
To achieve this, three guidelines are followed:
1. With the inclusion of this module, critical sections become internally protected.
2. Only those sections that the using code cannot protect by himself have built-in protection.      
   \note As an example, container class \alib{containers;HashTable} is not protected because 
         a using code can protect interfacing with an instance by itself. 
         On the other hand, accessing the \alib{lang::format;Formatter::Default;default formatter instance} 
         is protected because it may be used internally with some higher level functionality, 
         for example with \alib{lang;Exception::Format}.   
3. With the exclusion of this module, all protection is pruned to maximize execution performance.
                                    
To achieve this, some sort of "cooperation" between the core of \alib and this module is necessary.
This cooperation means: In the absence of \alib_threads_nl, still some skeleton of types and
macros need to exist. 
As samples for this take macro \ref ALIB_LOCK_WITH or debug-type 
#alib::lang::DbgCriticalSections. 
As seen, the latter type is placed in core namespace #alib::lang and in the absence of this module,
remains there as an empty type. Usages of it are optimized out.  

"Cooperation" here is a positive wording. 
Negatively expressed, it could be said that "the isolation rules were broken". 
The benefit of this design decision is that all other code can use the macros and types independent 
from the \alibdist_nl. This avoids code clutter in \alib as well as in custom code that
opts in to using these features for themselves. 
 
# 1.3 Pruning  # {#alib_threads_intro_pruning}
This section lists entities placed in #alib::lang instead of #alib::threads
and macros that are likewise always available regardless if this module is included in an 
\alibdist or not. 
If it is not included, the macros are defined <em>"empty"</em>, so that their use is pruned.

Please consult the reference documentation of the listed entities for more information.

- Owner types (also used in other areas, but especially valuable to safely lock mutexes):  
  - \alib{lang;Owner}, 
  - \alib{lang;OwnerTry}, 
  - \alib{lang;OwnerTimed}, 
  - \alib{lang;OwnerRecursive},  
  - \alib{lang;OwnerShared},
  - \alib{lang;OwnerTryShared}, and
  - \alib{lang;OwnerSharedTimed}.

<p>     
- Macros using the owner types:
  - \ref ALIB_LOCK, 
  - \ref ALIB_LOCK_WITH,  
  - \ref ALIB_LOCK_RECURSIVE, 
  - \ref ALIB_LOCK_RECURSIVE_WITH,  
  - \ref ALIB_LOCK_SHARED, and 
  - \ref ALIB_LOCK_SHARED_WITH,
       
<p>     
- Class \alib{lang;DbgCriticalSections} (discussed in the next section.)
  - Macros for owning critical sections: 
    - \ref ALIB_DCS, 
    - \ref ALIB_DCS_SHARED, and 
    - \ref ALIB_DCS_WITH, 
    - \ref ALIB_DCS_SHARED_WITH,
         
  - Macros for acquiring and releasing critical sections: 
    - \ref ALIB_DCS_ACQUIRE
    - \ref ALIB_DCS_ACQUIRE_WITH(CS)
    - \ref ALIB_DCS_RELEASE
    - \ref ALIB_DCS_RELEASE_WITH(CS)
    - \ref ALIB_DCS_ACQUIRE_SHARED
    - \ref ALIB_DCS_ACQUIRE_SHARED_WITH(CS)
    - \ref ALIB_DCS_RELEASE_SHARED
    - \ref ALIB_DCS_RELEASE_SHARED_WITH(CS)
    
# 1.4 Assertions With Debug-Builds  ## {#alib_threads_intro_assert}
## 1.4.1 Background Considerations  ## {#alib_threads_intro_assert_background}
The few fundamental types offered by this module provide typical assertions in debug-builds,
as found with similar types provided elsewhere. 
For example, if a \alib{threads;Lock} is destructed, it is asserted that it is not locked.

Besides those, \alib proposes a rather uncommon type of assertion mechanic. This is implemented
with class \alib{lang::DbgCriticalSections} (explicitly located not in this module) and 
those preprocessor macros listed in the previous section whose name starts with \b ALIB_DCS.   
The approach taken is simply to mark certain code segments as being critical. 
Critical sections vary by use case, making it impossible for "lower-level" code entities to 
consistently judge their presence.

Let us look at the classic sample: A value is 
1. read, 
2. changed, and 
3. written back. 

If executed in parallel threads, these three steps comprise a critical section.
Now, if the value resides in type \b ContainerType, formally the instructions are:

1. value= ContainerType.<b>Read(key)</b> 
2. value.Change 
3. ContainerType.<b>Write(key, value)</b>. 
     
At the level of the container code, namely inside methods \b Read and \b Write, it cannot be 
decided whether this is a critical section or not: 
If \b Write is not included in an operation, it is not, while if it is,
both operations are to be included in the critical section.

Therefore, putting locking mechanisms into container types mitigates only the most simple cases. 
Only the using code is able to lock all necessary sequences and can do this as a whole.

\note 
  A prominent sample when indeed locking was performed on container type level, is found in the 
  initial version of the Java Class Library. 
  Here, types like \b util.Vector and \b util.Hashtable, synchronized every method call,
  aiming to ensure thread safety. 
  As just explained, this approach can be considered flawed, because:
  1. Unnecessary performance costs are imposed, and 
  2. it provided an \e illusion of safety.
  \note 
    This highlights the importance of context-aware synchronization rather being able to
    relying on built-in, method-level locking. 
    The C++ standard library does not perform  locks, with only a very few exceptions.
    For example, allocation of heap memory is synchronized  with methods <c>std::malloc</c> 
    and <c>std::free</c>.    
                                
From this perspective it could be assumed that likewise any debug-assertion concerning the 
protection of a critical section cannot be made in low-level code.
To stay with the sample, it could be assumed that it cannot be made with methods 
<b>ContainerType.Read</b> and <b>Write</b>.

But it can! For that turn, two things are necessary:
1. It must be allowed that not all cases can be fetched:<br>
   This condition is met, because in the case of assertions, if at least some cases are fetched,
   it is better than none.
2. Operations have to be separated into \e writing and <em>read-only</em>-operations:<br>
   While this sounds a little like turning the use case to one that fits C++ 17 type  
   \https{std::shared_mutex,https://en.cppreference.com/w/cpp/thread/shared_mutex}, namely
   a two-level access strategy, it is not: 
   Only "raising assertions" is turned into such a use case - but not the use case of locking!    
                                                                           
                          
## 1.4.2 Asserting Critical Sections' Entry And Exit ## {#alib_threads_intro_assert_entry}
With this theory discussed, type \alib{lang;DbgCriticalSections} can now be quickly explained.
- A combined approach of asserting sections that might get protected by standard locks, 
  recursive locks, and shared locks is implemented with this single type.
- The type usually is inherited publicly by types that are typical candidates to impose critical 
  sections and multithreaded access. 
  For example, within \alib among such types are \ref #alib::monomem "allocators" and  
  \ref #alib::containers "container types".<br> 
  If compiler symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is set, these types conditionally derive 
  from \b DbgCriticalSections or add a corresponding member.
- Acquirement, normal or shared, is always recursive with this debug-type. 
  Otherwise, its use would become too complicated, because, for example, the container types
  often call its functions in a nested fashion.<br>     
  Furthermore, shared acquirement is allowed if the same thread already performed full
  acquirement. 
  The opposite is asserted: It is not allowed to perform full acquirement when
  already shared acquisition was made before.    
- The interface functions of types equipped with an instance of \b DbgCriticalSections acquire 
  this instance in either normal or shared mode.<br>
  For example, all interface methods of the \b allocator-types assert for full (non-shared) access. <br>
  The write methods of container types likewise check for full access, while their read 
  methods check for shared access only.<br>
- If a type inherited from \b DbgCriticalSections, the easiest way to acquire, is to use 
  macros \ref ALIB_DCS or \ref ALIB_DCS_SHARED.<br>
  If \b DbgCriticalSections is implemented as a member, alternative macros \ref ALIB_DCS_WITH and
  \ref ALIB_DCS_SHARED_WITH are to be used. <br>
  These macros create an anonymous instance of class \alib{lang;Owner} 
  (respectively \alib{lang;OwnerShared}), which assures the release of the section, even if
  exceptions are thrown.   
- Alternative macros, which do not rely on the automation mechanics of class \alib{lang;Owner}
  and it siblings, are given with:   
  - \ref ALIB_DCS_ACQUIRE
  - \ref ALIB_DCS_ACQUIRE_WITH(CS)
  - \ref ALIB_DCS_RELEASE
  - \ref ALIB_DCS_RELEASE_WITH(CS)
  - \ref ALIB_DCS_ACQUIRE_SHARED
  - \ref ALIB_DCS_ACQUIRE_SHARED_WITH(CS)
  - \ref ALIB_DCS_RELEASE_SHARED
  - \ref ALIB_DCS_RELEASE_SHARED_WITH(CS)
- Finally, if module \alib_threads_nl is not included in an \alibdist_nl, then with 
  debug-compilations, the macros above   
  \ref #alib::DbgAssertSingleThreaded "assert single threaded use" of \alib.   
                                  
The output written with assertions should be 'clickable' inside a users' IDE.
The default output string is optimized for \https{JetBrains CLion,www.jetbrains.com/clion} and can 
be changed by manipulating the static field member 
\alib{lang;DbgCriticalSections::ASSERTION_FORMAT}.

## 1.4.3 Asserting Critical Section Locks   ## {#alib_threads_intro_assert_locks}

As discussed in the previous chapters, only the using code entities have "knowledge" about
what are the real sequences which have to be executed atomically by threads.
Therefore, the captured assertions in lower-level code entities can fetch only a subset of the 
compromising cases.

It is clear that the underlying code even more has no knowledge about which instance of a 
mutex has to be locked when entering a critical section. 
Class \alib{lang;DbgCriticalSections} allows receiving this knowledge. 
This is done using \alib{lang::DbgCriticalSections;AssociatedLock}, which is a virtual struct  
that allows answering the question about whether or not "something" is acquired. 
With compiler symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is set, the six lock types of \alib 
inherit this interface and thus can be optionally set to member 
\alib{lang::DbgCriticalSections;DCSLock}.
If done, with the acquisition and release of the section, the state of the lock provided is 
asserted.

Three samples introduced by other \alibmods_nl quickly demonstrate this:
\par Sample 1:
  Unique namespace instance \alib{monomem;GLOBAL_ALLOCATOR} is accompanied by mutex
  \alib{monomem;GLOBAL_ALLOCATOR_LOCK}. 
  During \ref alib_manual_bootstrapping "bootstrap", 
  this lock is attached to the allocator instance like this:
   \snippet "alib.cpp"                      DOX_CRITICAL_SECTIONS_ADD_LOCK
\par    
  Consequently, assertions are raised, if:
  - It is a debug-compilation,
  - Module \alib_threads_nl is included in the distribution,      
  - Compiler symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is set, and
  - the \b GLOBAL_ALLOCATOR is used without prior acquisition of \b GLOBAL_ALLOCATOR_LOCK.
          
\par Sample 2:
  Static member \alib{lang::format;Formatter::Default} is accompanied by mutex
  \alib{lang::format;Formatter::DefaultLock}. 
  During bootstrap, this lock is attached to the formatter instance like this:
   \snippet "lang/basecamp/basecamp.cpp"    DOX_CRITICAL_SECTIONS_ADD_LOCK2 
\par    
  The consequences are the same as described above: if all preconditions are met, the use
  of the default-formatter asserts that the corresponding mutex is acquired. 
  
\par Sample 3:
  Class \alib{threads;TCondition} is a next candidate for a potential lock usable with 
  critical section testing. But because the type is a template and designed to be usable 
  as a base type only, it technically can not implement the debugging interface \b AssociatedLock.
  But derived types can. This is, for example, the case with class \alib{threadmodel;ThreadPool}.
  When critical section debugging is active, it attaches itself with field \b DCSLock of 
  its \b MonoAllocator member. This ensures that allocations are only performed when the 
  thread pool is locked.     
                                                                                                                                                                                                 
\divkt
- <b>Fundamental Purpose:</b><br>
  The module primarily addresses two areas in multithreaded programming: launching/managing threads and protecting data against race conditions. It serves as a foundational component for multithreading support.
- <b>Role as a Code Selector:</b><br>
  Beyond providing basic threading functionality, this module acts as a code selector for determining multithreading support across the entire \alib library.
- <b>Threading-Agnostic Design:</b><br>
  \alib aims to support both, single-threaded and multithreaded environments, 
  by making critical sections optional and pruning protection where it is unnecessary
  for performance optimization.
- <b>Cooperation Between Modules:</b><br>
  The module’s design includes some integration with other \alib components to maintain flexibility 
  and avoid unnecessary complexity, ensuring seamless multithreading support.
- <b>Debug-Build Assertions:</b><br>
  Special debug assertions are implemented to mark critical sections and validate thread safety in 
  debug-builds, promoting proactive identification of concurrency issues.
- <b>Historical Lesson:</b><br>
  The design highlights the importance of context-aware synchronization, learning from early Java 
  container implementations that misapplied method-level locking, resulting in performance 
  penalties and misleading safety guarantees.
- <b>Practical Implementation:</b><br>
  The module provides macros and types that work even in environments where threading support 
  is excluded, maintaining flexibility and code simplicity.
- <b>Debug Support:</b><br>
  Specific features like DbgCriticalSections assert proper acquisition and release of critical 
  sections, allowing developers to catch issues during development, without using that are 
  specialized for this purpose.  
\divend
 
\I{################################################################################################}
# 2. Thread Creation And Management# {#alib_threads_creation}
This module provides only basic support for thread creation, registration, and termination.
The reason is that, this module (as elaborated in the introduction), has the role of switching
the whole library to have thread support - or not. 
 
More sophisticated thread management is found with the higher-level module \alib_threadmodel.

All that is available is
- class \alib{threads;Thread} and
- abstract class \alib{threads;Runnable}.

Their use is quite self-explanatory and documented with reference documentation of the types.

\note 
  It is no problem to have threads started using different methods and libraries than the
  one found here, as long as such threads become "native" operating system threads.
  If later such a thread uses method  \alib{threads;Thread::GetCurrent}, a corresponding
  \alib{threads;Thread} object of \alib is created internally and returned.
  This way, the externally created thread is automatically "registered" with \alib.
\note 
  If not stated differently in the detailed documentation, from this point in time,
  the thread can be interfaced with \alib classes in the same manner as if it was created
  using them. 
  The same or similar should be \c true for the opposite situation.

\I{################################################################################################}
# 3. Locks# {#alib_threads_locks}
In the area of data protection and controlling concurrent access to critical code sections, 
this module provides the following six types, 
which are wrappers around corresponding C++ Standard Library types:
                                                                                    
ALib Type                         | Wrapped C++ Standard Library Type
----------------------------------|------------------------------------------
\alib{threads;Lock}               | <c>std::mutex</c>
\alib{threads;TimedLock}          | <c>std::timed_mutex</c>
\alib{threads;RecursiveLock}      | <c>std::recursive_mutex</c>
\alib{threads;RecursiveTimedLock} | <c>std::recursive_timed_mutex</c>
\alib{threads;SharedLock}         | <c>std::shared_mutex</c>
\alib{threads;SharedTimedLock}    | <c>std::shared_timed_mutex</c>

The timed methods (of the timed lock versions), detect spurious wake-ups and mitigate those.  
                                          
## 3.1 Performance ## {#alib_threads_locks_perf}
In respect to acquiring and releasing a previously unlocked instance, all types
perform on modern GNU/Linux, MacOS, and Windows machines in the area of nanoseconds.
Especially, no difference between the normal and the <em>timed</em>-versions seem to exist
(when messured under GNU/Linux). 
Furthermore, the timed versions have the same footprint on these platforms.
\note 
  The designers of this module were hesitant to even separate the types. 
  But we thought there might be a reason why the C++ Standard introduced the separation and so we
  followed this guideline. 
  While a timed try to acquire a lock is a rather seldom requirement,  when choosing the lock-type, 
  a due reasoning about whether timed-tries will be required by a using code entities or not. 

## 3.2 Debug Features ## {#alib_threads_locks_dbg}
With debug-compilations, each lock receives a member called \b Dbg, which is of type
\alib{threads;DbgLockAsserter}, respectively for the two shared-locks of type 
\alib{threads;DbgSharedLockAsserter}. 
This type stores \alib{lang;CallerInfo;caller information} of recent acquisition and release
actions. 
The information includes source code location, caller type-info, and the calling thread.
Assertions produce exhaustive information, which tremendously helps to quickly identify
many problems.
\note The output should be "clickable" in your development environment. 
      If it is not, the format can be changed so that it is recognized by a users' IDE. 
      It is given with static fields \alib{threads;DbgLockAsserter::ASSERTION_FORMAT}
      for the non-shared lock types and with 
      \alib{threads;DbgSharedLockAsserter::ASSERTION_FORMAT_SHARED} for the two shared lock types. 

Assertions are raised, when 
- nested acquisitions on non-recursive locks are performed,
- acquired locks are destructed,
- a non-acquired lock is released, and
- a lock is released, which is acquired by a different thread. 
                                      
Furthermore, two sorts of warnings are given:
- The three non-timed lock types are internally wrapping the corresponding timed versions of the
  C++ Library. 
  With that, they optionally write debug-warning messages about waiting times higher than a given 
  threshold. 
  This threshold is found with debug-member \alib{threads;DbgLockAsserter::WaitTimeLimit}.
- A maximum recursion threshold is provided with field \alib{threads;DbgLockAsserter::WaitTimeLimit}
- Field \alib{threads;DbgLockAsserter::RecursionLimit} provides a threshold for the number of 
  nested acquisitions, which defaults to \c 10. 
  Note that recursion sometimes is not easily avoidable, but if a recursive lock is in fact needed, 
  it should still not be overused.
  Of-course the threshold can be lifted if needed.  
          
\note Similar assertions are implemented with type \alib{threads;TCondition}.

## 3.3 Automatic Locking ## {#alib_threads_locks_auto}
### 3.3.1 Class Owner And Its Siblings ## {#alib_threads_locks_auto_owner}
\alib provides a set of core classes which magically fit the interface of the locks found
in this module. It is strongly advised to use their automation mechanics whenever possible.
\note In fact this means using strong safety that C++ compilers provide in respect to unwinding the 
      stack in case of emergency. 

The main type of this set is class \alib{lang;Owner} which on construction invokes a method called 
\b Acquire with its templated type \p{TOwnable} and \b Release on destruction.

Along with the different <em>Owner</em>-types come three macros that makes their use easier by
creating an "anonymous local variable". Those are:
- \ref ALIB_OWN, 
- \ref ALIB_OWN_RECURSIVE, and 
- \ref ALIB_OWN_SHARED. 
 
\note Anonymous local variables are not available by the C++ language. 
      The macros do the trick, by internally using macro \ref ALIB_IDENTIFIER.    

The following table lists all <em>Owner</em>-types, the functions they call on their templated
owned object, and the macro to use:   
  
Owner Type                  | Constructor Call         | Destructor Call                   | Macro To Use 
----------------------------|--------------------------|-----------------------------------|--------------------- 
\alib{lang;Owner}           | \b Acquire               | \b Release                        | \ref ALIB_OWN
\alib{lang;OwnerTry}        | \b TryAcquire            | \b Release or \b ReleaseRecursive | <em>Not applicable</em>
\alib{lang;OwnerTimed}      | \b TryAcquireTimed       | \b Release or \b ReleaseRecursive | <em>Not applicable</em>
\alib{lang;OwnerRecursive}  | \b AcquireRecursive      | \b Release                        | \ref ALIB_OWN_RECURSIVE
\alib{lang;OwnerShared}     | \b AcquireShared         | \b Release                        | \ref ALIB_OWN_SHARED
\alib{lang;OwnerTryShared}  | \b TryAcquireShared      | \b Release                        | <em>Not applicable</em>
\alib{lang;OwnerSharedTimed}| \b TryAcquireSharedTimed | \b Release                        | <em>Not applicable</em>

The owners and their use are best explained with an example. Consider you have custom type
\b MyAcquirable along with a global instance of this type:
  \snippet "ut_lang.cpp"    DOX_LANG_OWNER1

Now, without using the macro, you would have to create a named owner instance for acquiring
this instance:
  \snippet "ut_lang.cpp"    DOX_LANG_OWNER2
  
Using macros makes things much simpler:  
  \snippet "ut_lang.cpp"    DOX_LANG_OWNER3

Those <em>Owner</em>-types that have the word <em>"Try"</em> in their name provide 
methods to receive the result of the acquisition, for example, \alib{lang;OwnerTry::IsOwning}.  
Hence they must not be used anonymously, because the using code needs to check if a tried acquisition 
succeeded. For this reason, no corresponding macros are provided.

Note that the use of the macros from within static methods and namespace functions, 
needs one line of code as preparation. 
This is explained in chapter \ref alib_manual_appendix_callerinfo of the General 
Programmer's Manual.
      
### 3.3.2 Lock-Macros ## {#alib_threads_locks_auto_macros}
The different lock types introduced in previous section \ref alib_threads_locks, are "compatible" 
with the <em>Owner</em>-types that \alib provides. 
Instead of the accompanying macros, aliased versions are recommended to be used: 

Owner Type                  | Original Macro           | Aliased Macros
----------------------------|--------------------------|--------------------------
\alib{lang;Owner}           | \ref ALIB_OWN            | \ref ALIB_LOCK <br> \ref ALIB_LOCK_WITH
\alib{lang;OwnerRecursive}  | \ref ALIB_OWN_RECURSIVE  | \ref ALIB_LOCK_RECURSIVE <br> \ref ALIB_LOCK_RECURSIVE_WITH
\alib{lang;OwnerShared}     | \ref ALIB_OWN_SHARED     | \ref ALIB_LOCK_SHARED <br> \ref ALIB_LOCK_SHARED_WITH

The alias macros do not only increase readability, but they also prune their contents in case
that module \alib_threads_nl is not included in the \alibdist. 
Hence they support the creation of \ref alib_threads_intro_agnostic "threading-agnostic software".    

\note As a background information:<br> 
      Classes \alib{threads;RecursiveLock} and \alib{threads;RecursiveTimedLock} 
      come with methods \b AcquireRecursive and \b ReleaseRecursive, instead of just \b Acquire and
      \b Release. 
      This is a design decision. 
      If this was not chosen, the standard \b Owner and corresponding macros could be used. 
      However, the specific owner type \alib{lang;OwnerRecursive} was necessary to satisfy the 
      different method names found with class \alib{lang;DbgCriticalSections}, 
      where both acquirement functions are found in just one type. 
      With that, the decision was taken to also use these function names also with the recursive  
      lock types. This way, the using code has a more explict language. 

Note that the use of the macros from within static methods and namespace functions, 
needs one line of code as preparation. 
This is explained in chapter \ref alib_manual_appendix_callerinfo of the General 
Programmer's Manual.
       
\I{################################################################################################}
## 3.4. Specific Locks Found Across ALib## {#alib_threads_alib}
The following locks are found across this library.
While their namespace and type names should hint to their use-case, for details, please consult 
their reference documentation.
 
- #alib::threads::STD_IOSTREAMS_LOCK               
- #alib::monomem::GLOBAL_ALLOCATOR_LOCK               
- #alib::lang::format::Formatter::DefaultLock 

\I{################################################################################################}
# 4. Other Types Introduced # {#alib_threads_other}

Two more fundamental types are introduced by this module. Those are:  

- \alib{threads;TCondition}, and 
- \alib{threads;Promise}.

Because this module is considered to be fundamental and low-level, other tools to manage threads and
concurrency are not in its domain.<br> 
However, with module \alib_threadmodel, a high-level approach into threading is followed.
It proposes an architecture for multithreaded software and provides classes that abstract away many 
plenty of the basics.         




<br><br><br><br><br><br> */
