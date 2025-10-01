// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_singletons   ALib Module Singleton - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1. Introduction # {#alib_singleton_intro}
The namespace #alib::singletons contains the concise and low-level module \alib_singletons. 
Although its scope is minimal, organizing the library orthogonally makes this separation beneficial. 

This manual tries to tell you all about singletons and shows you alternative implementations.
The use-case that class \b Singleton tackles are 
- Templated types that should have a single instance per template instantiation.
- The number of instantiations is not foreseeable: For example, this is the case when a library 
  offers such a type, not "knowing" which instantiations a using code will perform.  
- No control about the usage of the code in respect to its use in WindowsOS DLLs and their 
  management. (Which is the typical situation for \alib).   

\I{################################################################################################}
# 2. C++ Singletons # {#alib_singleton_background}
This module contains a single class \alib{singletons;Singleton}, which implements a singleton
suitable for the use-case described above. 
 
In C++, the commonly proposed solution using templates is straightforward 
and might not seem to warrant even a small module. 
This design pattern is called <em>"Meyers’ Singleton"</em> and is implemented as follows:

\code{.cpp}
    // template class to provide singleton functionality
    template<typename T> 
    class Singleton
    {
        // static class to get the singleton
        public: static T* Get()
        {
            static T theSingleton;  // a static local variable containing the singleton
            return &theSingleton;
        }
    };

    // usage:
    class MyClass : public Singleton<MyClass>
    {
        // implementation of sample class...
    };

    MyClass* sample= MyClass::Get();
\endcode
                       
This approach generally works well. However, on Windows OS, if software accesses the same 
singleton object from different DLLs or from both a DLL and the main executable, each retrieves 
its own instance. 
Although running in one process, the singletons are "per DLL/Executable" because Windows DLLs 
have their own data segments for global variables. 
This contrasts with shared libraries on GNU/Linux or macOS, which behave like statically 
linked libraries.

The code in this repository creates true singleton objects, even when using WindowsOS DLLs. 
This is achieved by creating a static hash map that stores all singletons. 
The C++ runtime type information structure <c>std::type_info</c> is used as the key for the 
singletons in the hash map.<br>
Then, the type of the static local variable is turned into a pointer. 
If the pointer is not set, then the singleton is searched in the hash map, and if not found, 
created once.

This assures that only one singleton per process is created, regardless of how many times a 
DLL was loaded. 
From a performance perspective, the impact is minimal, because per DLL instance, only the first 
access to a singleton imposes an overhead. For the second, only an <c>std::atomic</c> access
to the already valid pointer is performed. 
On other systems, like GNU/Linux or macOS, the whole mechanism is omitted and the simple fallback 
variant that implements the design shown above is activated. 

In case \alib is compiled in \ref alib_manual_multithreading "multithreaded mode", the 
creation and access of the singletons is threadsafe.


\I{################################################################################################}
# 3. Switching Compilation Mode #  {#alib_singleton_mode}
As mentioned above, this implementation is necessary only if a software process uses different 
data segments for different portions of the code, such as processes that load DLLs on Windows OS. 
Since this approach introduces a small overhead, the implementation can be switched to a simpler 
version that stores the singletons in a static field of the class. 
This behavior is controlled by the compiler-symbol \ref ALIB_FEAT_SINGLETON_MAPPED, which defaults 
to \c true on Windows OS and \c false otherwise.

                                       
You might consider changing these defaults for the following reasons:
- If \alib is used on WindowsOS but not compiled as a DLL, and the code using \alib is restricted 
  to a single DLL or the main application exclusively, you can pass 
  \ref ALIB_FEAT_SINGLETON_MAPPED <c>=0</c> to the compiler to switch to the simple mode.
- If ALib is used on GNU/Linux and you want to utilize the debug features for listing all created 
  singletons (described below), you can pass \ref ALIB_FEAT_SINGLETON_MAPPED <c>=1</c> to the 
  compiler to enable the hash map, even though it's not necessary for that platform.

\I{################################################################################################}
# 4. Usage # {#alib_singleton_usage}
The implementation details described above are transparent to the user when utilizing the 
Singleton class.
A user-defined type can inherit the singleton functionality by deriving from 
<b>Singleton<TDerivedClass></b>, where the template parameter is simply the name of the derived 
class. 
The static method \alib{singletons;Singleton::GetSingleton} will then return a singleton instance 
of the derived type.

Here is a sample code deriving from this class:

 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_1

Now, the singleton can be requested as follows:

 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_2
 
Since Singleton::GetSingleton() is static, the call can be made without explicitly instantiating 
the class, ensuring a <em>lazy initialization</em>.


\I{################################################################################################}
# 5. Strict Singletons #  {#alib_singleton_strict}
Note that the object returned by method \alib{singletons;Singleton::GetSingleton} is
not automatically the only instance of the custom class: While it can be considered to be
<em>"the"</em> singleton instance, other instances might still be created. 
Usually, the term "strictness" is used to denote if a singleton type is to be allowed to have 
other instances of the type next to the singleton instance or not.

In this respected, types derived from class \b %Singleton are not strict, as shown here:

 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_3

To create classes that implement strict singletons, two things have to be done:
1. Provide a private constructor to the derived class, and
2. make parent class \b %Singleton be a friend of it.

This is shown in the following sample:
 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_STRICT_1
 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_STRICT_2

\I{################################################################################################}
# 6. Memory Cleanup #  {#alib_singleton_termination}
Before terminating the process, the singletons collected in the global map, are to be destructed and
deleted with namespace function \alib{singletons::shutdown}, which is done with standard
\ref alib_mod_bs_nocamps "library shutdown"


Especially note that the default \ref alib_mod_bs "ALib termination process" described with
module \alib_bootstrap, invokes this method already.

\I{################################################################################################}
# 7. Restrictions / Penalties#  {#alib_singleton_restrictions}
- The singleton class needs to have a default (parameterless) constructor.
  (This might be overcome with a templated get-method that forwards variadic arguments, but
  is not implemented here.)
- Singletons must not be received by constructor code of global or static objects, aka code
  that is executed before entering method \c main(). The reason for this is that
  the hashtable used to store singletons, is likewise a global object and might not be initialized
  at that point in time.
- The singleton class is virtual and hence all derived classes become virtual. This is needed
  to be able to have a virtual destructor.
- There is a minimal performance penalty in comparison to more simple implementations of the 
  singleton design pattern: Each <em>'code entity'</em> (DLL or main executable)
  needs to retrieve a singleton which was potentially already created by another code
  entity, \b once. In other words, only the first request for the singleton incorporates two
  small performance penalties for
  1. setting and releasing a thread lock (mutex)
  2. retrieving the singleton or inserting the singleton if not retrieved.
- The memory penalty is also quite minimal and imposed by the creation of a static hash table,
  which holds a pointer to each Singleton.
- Upon exit of the process, programmers might want to explicitly free the hash table to avoid
  the detection of memory leaks by metrics tools like \http{Valgrind,valgrind.org/}.
  (Otherwise this can be omitted, as the memory is cleaned by the OS probably much faster when a
  process exits).
- In general, it is advised to think twice before implementing the singleton design pattern.
  Often, this is considered bad code design. 
  However, there can be great reasons for having singletons in software.

\I{################################################################################################}
# 8. Listing Types in Debug Compilations #  {#alib_singleton_debug}
As described above, to extend the singleton concept across DLL bounds, a static hash map is
needed. Now, with debug-builds (see compiler-symbol \ref ALIB_DEBUG), this map can be
accessed with namespace function \alib{singletons;DbgGetSingletons}.

If the \alibbuild includes module \alib_strings, then an overloaded version exists with
\doxlinkproblem{namespacealib_1_1singletons.html;a948c1f6836cb54448392fedaa213fc79;DbgGetSingletons(NAString&);singletons::DbgGetSingletons(NAString&)}.
This function performs a simple dump of all singleton types into the given \b %AString.

\I{################################################################################################}
# 9. Use-Cases, Alternatives, Optimization #  {#alib_singleton_uao}

\I{################################################################################################}
## 9.1 Use-Cases ##  {#alib_singleton_uao_usecase}
The use-case definition was already given in the introduction. Here we want to give a 
real-world sample, taken from the module \alib_boxing. This module provides type \b Box that
is a container for any sort of object. For example, the statement:

        Box box= 42;

stores an integral value in a "box". The assignment operator (here constructor) stores
a pointer to a singleton object along with the value. The singleton is of type \b VTable<T>, where
\b T is the type that was boxed. In the case above \b VTable<int>.
This singleton is used with any instance of class \b Box that holds an int value.<br>

Now, if a custom code says:

        MyType myObject;
        Box box= myObject;

a pointer to a singleton of type \b VTable<MyType> is needed. The use of class
\alib{singletons;Singleton} allows creating such singletons "on-the-fly" without the need of a
corresponding global object definition.



\I{################################################################################################}
## 9.2 Alternatives ##  {#alib_singleton_uao_alt}

This module should maybe not be used (to avoid unnecessary dependencies of your code to this library)
in the following situations:

- If no templated singletons are needed, or if the set of template instantiations of that singleton 
  type is known upfront.

- If software is \b not planned to be compiled for Windows OS, or only for monolithic (non-DLL)
  executables. In this case, the simple alternative is to use a code
  similar to the one \ref alib_singleton_background "sampled above".
        
In these cases, alternatives may be:
- Meyers’ Singleton (static local variable in a function) – Sufficient if cross-DLL usage is not 
  required.
- Explicitly Exported Singletons (__declspec(dllexport)) – Works for a fixed set of singletons 
  but requires manual instantiation in the DLL.
- Windows Named Kernel Objects (CreateMutex, CreateFileMapping) – Ensures singleton instances 
  across processes, useful for system-wide singletons.                        


\I{################################################################################################}
## 9.3 Optimization With Mixed Approach ##  {#alib_singleton_uao_opt}

It is possible to "optimize" the creation of singletons when using this module by providing
specializations of dedicated "foreseeable" instantiations of the custom template type that is
derived from class \alib{singletons;Singleton}.

As a sample, we stick to the one presented in the section \ref alib_singleton_uao_usecase.
For all fundamental types and other important, aka "frequently boxed" types, the singleton
mechanism of class \b Singleton is "disabled". Hence, if an integral is boxed:

        Box box= 42;

the singleton of the \b VTable object is \b not created by method 
\alib{singletons;Singleton::GetSingleton}. Only for "unknown" types this is done.<br>
With that, not only the code size of the templated constructor of class \b Box shrinks, but it
is allowed to create global objects of type \b Box that are initialized with values of such types.

\note
  Due to the potential hashtable access of method \b Singleton::GetSingleton, the initialization
  of a global instance of class \b Box is otherwise undefined behavior, as it is not guaranteed
  that the likewise global hashmap is yet initialized at this point in time!

In contrast to what was proposed above, namely by giving specializations of the templated
\b VTable type, module \alib_boxing_nl takes a slightly modified approach: The \b VTable
singletons are exclusively received through the type trait 
\alib{boxing;VTableOptimizationTraits}.
Instead of specializing the \b VTable, this type is specialized for frequently used boxed types.

For further details on this implementation, consult chapter
\ref alib_boxing_more_opt_staticvt of the Programmer's Manual of module \alib_boxing_nl
and feel free to review the relevant source code found in \alibheader{boxing/detail/vtable.inl}.

<br><br><br><br><br><br> */
