// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_singletons   ALib Module Singleton - Programmer's Manual



\tableofcontents


\I{################################################################################################}
# 1. Introduction # {#alib_singleton_intro}
This namespace is the home of the very tiny and low-level \alibmod "Singleton". While the
scope is almost too small to be named a module, for organizing the library as "orthogonal" as
possible, the separation is helpful. And, should a programmer be really not interested in
anything else of \alib than the singleton feature, this module is well extractable from
the rest of the library and only a few files remain with no "clutter" around class
\alib{singletons;Singleton}.

It could be said, that this manual is written in reverse order. The use case of the library
and its alternatives is only given in the last chapter. The reason for this is that
presenting the solution first, just makes it more easy to understand when this library is applicable.<br>
Consequently, experienced programmers might want to read sections \ref alib_singleton_uao_usecase
and \ref alib_singleton_uao_alt first!


\I{################################################################################################}
# 2. C++ Singletons # {#alib_singleton_background}

The only class found in this module is \alib{singletons;Singleton}, which implements the well known
<b>Singleton Design Pattern</b>.
With C++ templates, the usually proposed solution is so simple, that it would not justify even this
tiny module. Such simple solution looks as follows:

\code{.cpp}
    // template class to provide singleton functionality
    template<typename T> class Singleton
    {
        public: static T* get()
        {
            static T thesingleton;
            return &thesingleton;
        }
    };

    // usage:
    class Sample : public Singleton<Sample>
    {
        // implementation of sample class...
    };

    Sample* sample= Sample::get();
\endcode

This works fine in general unless a software running on Windows OS, starts to access the same
singleton object from within different Windows DLLs or a Windows DLL and the main executable.
Although running in one process, the singletons retrieved are singletons "per DLL/Executable".
The reason for this is simply spoken that Windows DLLs dispose of an own data segment for global
variables.<br>
This is different with shared libraries under GNU/Linux or macOS, which are designed to behave
exactly as statically linked libraries.

The code provided with this repository creates true singleton objects, even when Windows OS DLLs are used.
This is achieved by creating a static hash map (only once) and collecting all singletons herein.
The C++ runtime type information struct \c std::type_info is used as the key to the singletons in
the hash map.

\I{################################################################################################}
# 3. Switching Compilation Mode #  {#alib_singleton_mode}
As said above, the effort done with this implementation is needed only if a software process
uses different data segments for different portions of the code, for example processes which open
DLLs on Windows OS.
As this effort imposes a (small) overhead, the implementation of this class can be switched to
a simple implementation version, which just stores the singletons in a static field of the class.
The implementation is controlled by compiler symbol \ref ALIB_FEAT_SINGLETON_MAPPED.
On Windows OS the symbol defaults to \c true, otherwise to false.<br>

Changing these defaults could be done for the following reasons:
- \alib is used on Windows OS, but not compiled as a DLL and the code that is using \alib
  is restricted to one DLL or the main application exclusively. In this case, symbol
  <b>ALIB_FEAT_SINGLETON_MAPPED=0</b> may be passed to the compiler to switch compilation
  to the simple mode.
- \alib is used on GNU/Linux, but the debug features for listing all singletons created
  (described below) should be used. In this case, for the purpose of debugging,
  <b>ALIB_FEAT_SINGLETON_MAPPED=1</b> may be passed to the compiler to switch compilation
  to use the hash map, even though this is otherwise not needed for that platform.

\I{################################################################################################}
# 4. Usage # {#alib_singleton_usage}
The implementation details described above are transparent in respect to using the class.

A user type may inherit the singleton feature from this class by being derived from it,
with the template parameter \p{TDerivedClass} set to name just of the derived class type itself.
Then, static method \alib{singletons;Singleton::GetSingleton} will return a singleton of the
derived type.

Here is a sample code deriving from this class:

 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_1

Now, the singleton can be requested as follows:

 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_2

\I{################################################################################################}
# 5. Strict Singletons #  {#alib_singleton_strict}
Note that the object returned by method \alib{singletons;Singleton::GetSingleton} is
not automatically the only instance of the custom class: While it is "the singleton" object,
other instances might still be created. Usually, the term "strictness" is used to denote if
a singleton type is to be allowed to have other instances of the type next to the singleton
instance or not.

In this respected, types derived from class \b %Singleton are not strict, as shown here:

 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_3

To create classes that implement strict singletons, two things have to be done:
1. Provide a private constructor to the derived class and
2. make parent class \b %Singleton be a friend of it.

This is shown in the following sample:
 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_STRICT_1
 \snippet "DOX_SINGLETON.cpp"     DOX_SINGLETON_STRICT_2

\I{################################################################################################}
# 6. Memory Cleanup #  {#alib_singleton_termination}
Before terminating the process, the singletons collected in the global map, can be destructed and
deleted using namespace function \alib{singletons;Shutdown}.
See this method's description for more information.

Especially note that \ref alib_manual_bootstrapping "default module termination" (namely invoking
function \alib{Shutdown} invokes this method already.


\I{################################################################################################}
# 7. Restrictions / Penalties#  {#alib_singleton_restrictions}

- The singleton class needs to have a default (parameterless) constructor.
  (This might be overcome with a templated get-method that forwards variadic arguments, but
  is not implemented here.)
- Singletons must not be received by constructor code of global or static objects, aka code
  that is executed before entering method \c main(). The reason for this is that
  the hashtable used to store singletons, is likewise a global object and might not be initialized
  at that point in time yet.
- The singleton class is virtual and hence all derived classes become virtual. This is needed
  to be able to have a virtual destructor.
- There is a minimal performance penalty in comparison to more simple implementations of the singleton
  design pattern: Each <em>'code entity'</em> (DLL or main executable)
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
  Often, this is considered bad code design. However, there can be very good reasons for having
  singletons in software.
  And do not be frustrated if "very wise people" tell you that you must never use a singleton.
  Your answer to them is: Each and every created instance of a virtual C++ class contains
  a pointer to a singleton, namely the \https{vtable,en.wikipedia.org/wiki/Virtual_method_table}!

\I{################################################################################################}
# 8. Listing Types in Debug Compilations #  {#alib_singleton_debug}
As described above, to extend the singleton concept across DLL bounds, a static hash map is
needed. Now, with debug-builds (see compiler symbol \ref ALIB_DEBUG), this map can be
accessed with namespace function  \alib{singletons;DbgGetSingletons}.

If the \alibdist includes module \alib_strings, then an overloaded version exists with
\doxlinkproblem{namespacealib_1_1singletons.html;a948c1f6836cb54448392fedaa213fc79;DbgGetSingletons(NAString&);singletons::DbgGetSingletons(NAString&)}.
This function performs a simple dump of all singleton types into the given \b %AString.

\I{################################################################################################}
# 9. Use-Cases, Alternatives, Optimization #  {#alib_singleton_uao}

\I{################################################################################################}
## 9.1 Use-Cases ##  {#alib_singleton_uao_usecase}

With the explanations given in the previous chapters, the precise use-case for this library
can be defined:

\par
  <b>ALib Singletons is to be used when implicit definitions of singleton objects of
  templated types are needed.</b>

In other words: The ultimate goal of this module was to allow a library to present a templated singleton type to a
using code. When the using code created a custom instantiation of the type, no need for a parallel
definition of a global singleton instance for this template instantiation should be needed.

This might be explained with a quick sample. Module \alib_boxing provides type \b Box that
is a container for any sort of object. For example:

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

- If no templated singletons are needed. In this case, a virtual class should be preferred that
  provides an abstract virtual method \b Get which has to be overloaded by each derived class.

- If templated singletons are needed, but the set of template instantiations of that singleton type
  is known upfront - or it is no problem to demand a definition of a singleton object for each
  new instantiation performed.

- If a software is \b not planned to be compiled for Windows OS, or only for monolithic (non-DLL)
  executables. In this case the simple alternative is to use a code
  similar to the one \ref alib_singleton_background "sampled above".


\I{################################################################################################}
## 9.3 Optimization With Mixed Approach ##  {#alib_singleton_uao_opt}

It is possible to "optimize" the creation of singletons, when using this module, by providing
specializations of dedicated "foreseeable" instantiations of the custom template type that is
derived from class \alib{singletons;Singleton}.

As a sample, we stick to the one presented in section \ref alib_singleton_uao_usecase.
For all fundamental types and other important, aka "frequently boxed" types, the singleton
mechanism of class \b Singleton is "disabled". Hence, if an integral is boxed:

        Box box= 42;

then the singleton of the \b VTable object is \b not created by method \alib{singletons;Singleton::GetSingleton}.
Only for "unknown" types this is done.<br>
With that, not only the code size of the templated constructor of class \b Box shrinks, but it
is allowed to create global objects of type \b Box that are initialized with values of such types.

\note
  Due to the potential hashtable access of method \b Singleton::GetSingleton, the initialization
  of a global instance of class \b Box is otherwise undefined behavior, as it is not guaranteed
  that the likewise global hashmap is yet initialized at this point in time!

In contrast to what was proposed above, namely by giving specializations of the templated
\b VTable type, module \alib_boxing_nl takes a slightly modified approach: The \b VTable
singletons are exclusively received through a next template type \alib{boxing::detail;T_VTableFactory}
Instead of specializing the \b VTable, this type is specialized for "frequently used boxed types".

For further details on this implementation, consult chapter
\ref alib_boxing_more_opt_staticvt of the Programmer's Manual of module \alib_boxing_nl
and feel free to review relevant source code found in \alibheader{boxing/detail/vtable.inl}.

<br><br><br><br><br><br> */
