// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
\page alib_manual_why_layering    Benefits of Strictly Layered Software Architectures

1. <b>Design Principle of Orthogonality</b><br>
   The modularization of the library is first of all a design principle, forcing library
   features to "fall into place" and make types and methods "orthogonal". (Orthogonal software
   can be considered as the opposite of "spaghetti code".)

2. <b>Comprehensible</b><br>
   Users of the library are able to grasp the overall feature set much faster due to the
   modularization. As an example, the
   \ref alib_expressions_intro_procon "features of module ALib Expressions", is currently more
   than 25 bullet points. A user not interested in run-time expression evaluation, will
   not be bothered with that, because the module has an own namespace, separated
   \ref alib::expressions "reference documentation" and an own
   \ref alib_mod_expressions "Programmer's Manual".

3. <b>Well Defined Library Bootstrapping</b><br>
   As explained in a later chapter, bootstrapping software process written in C++ needs to
   obey to some certain rules which the programming language itself does not provide support
   for. \alibmods_nl tremendously helps to organize the library bootstrap process.

4. <b>Minimum Footprint</b><br>
   Modules can be compiled and used separately from others, as long as there is no dependency
   between modules.
   When using \https{CMake,cmake.org/} to build the library, all module-dependencies are
   automatically resolved: If a using application just needs a single module, that module is
   all that has to be specified with CMake. The provided scripts select the dependent modules and
   corresponding source files automatically.

5. <b>Build-Speed</b><br>
   The ability to just partly compile (and use) \alib also increases build speed. Not only that
   of the library itself, but also that of the user code (due to the selective header inclusion).
                           
\I{################################################################################################}
\I{### C++20 Module experiences}
\I{################################################################################################}
\page alib_c20module_shift    Observations Made When Shifting ALib to C++20 Modules
                                                                                           
In the beginning of the year 2025 we migrated \alib to use C++20 Modules.
On this page we want to describe our observations.

\attention
  Update: Since Clang's Version 21, this compiler has also adopted the strict C++20 module syntax 
  rules. As a result, the technique we previously used and described below no longer works.
  All that is said below about GCC and MSVC compilers is now also true for Clang.
  
\attention
  For now, <b>we are suspending our attempts to support C++20 modules!</b> 
  It may well be that, for libraries like \alib, the entire "concept" is not a good fit.<br>
  Or we are doing something completely wrong? The discussion below should explain our pain.
      
\I{################################################################################################}
# 1. How C++20 Modules Reshaped Our Library’s Structure# {#alib_c20_modshift_structure}
Before C++20 Modules entered the scene, we had already broken our library, \alib, into smaller,
self-contained units we called \alibmods.
The reasoning behind this was explained in an older version of the manual, but since it’s a
common practice that most libraries follow, we decided to move the discussion
\ref alib_manual_why_layering "to an extra page".
In short, we didn’t want to clutter the main manual with something that seasoned developers
would find obvious.

Still, when C++20 Modules came knocking, they had a huge positive impact on \alib’s structure.
Before the transition, we did our best to avoid circular inclusions of header files... but in
some cases, we intentionally allowed them because they simply made sense!
To understand why circular dependencies cropped up, let’s look at how \alib handles optional
dependencies (\ref alib_manual_alib_build "explained in the manual").
Some optional dependencies naturally form a circle with the modules they rely on. For example:
   
- Module \b B depends on module \b A to function. (A mandatory dependency.)
- However, module \b A can offer extra features \e if module \b B is also included in the
  \alibbuild.

A real-world example is the relationship between \alib_enumrecords and \alib_resources.
Enum records don’t need resourced strings to work. But when resourced strings are available,
it makes sense to represent those records using them.
So, where do you put the code for this optional feature?
Before C++20 Modules, the answer was: “in the lower-level module, of course!”
After all, this follows the principle of “separation of concerns.”

Here’s what we did: we included the higher-level module’s header file \e conditionally — only if
it was part of the build. Simple, right?

<b>Structural Improvements with C++20 Modules</b><br>
C++20 Modules made us rethink this tactic entirely. The new language rules <em>don’t allow</em> this
kind of interdependence anymore.
We had to shift all extension code to the higher-level module.
Why? Because, with C++20 Modules, the module interface definition \b must have visibility into both
the dependent and dependent-on modules from the start.

Now here’s the fun part: C++20 Modules and C++ Namespaces are totally independent concepts.
This means we could keep the original namespace structure intact while reorganizing the
implementation.
For instance, \alib_resources (which uses the alib::resources namespace) now "injects" additional
functions and types into the alib::enumrecords namespace. Neat, right?

If you’re new to C++20 Modules, this behavior might seem strange at first. But technically,
it’s the correct approach. In fact, it’s often the only way to make things work!
The beauty of this approach became more apparent over time.
It wasn’t any one big change but rather a series of small adjustments that eventually started
to align.
Things we had long accepted as "not ideal, but manageable" just <b>naturally began to fall into
place.</b>
 

\par Here is what we gained:
  Our library expanded from 18 to 25 modules, with responsibilities better separated than ever.
  There are far fewer “exceptions to the rule” in our Programmer’s Manual now!
  The structure is cleaner, simpler, and easier to navigate.
  Most importantly, the learning curve for new users has significantly eased.

                           
\divkt
- Sometimes, being forced to do things "the right way" works out for the best.
  C++20 Modules really pushed us toward better architecture.
- Don’t forget: C++ Modules and Namespaces are independent concepts.
  Injecting new functionality into existing namespaces is totally fine.
  From the user’s perspective, the interface feels like it belongs exactly where it’s supposed to;
  meanwhile, the compiler has everything neatly organized behind the scenes.
- Yes, transitioning to C++20 modules was tedious. We gained a better structure of our codebase.
  (But, read on, this does not mean that we love C++20 Modules!)
\divend


\I{################################################################################################}
# 2. How The Transition Affected Legacy Compilation Speeds # {#alib_c20_modshift_legacy}
It’s important to mention that the structural improvements we talked about earlier
were \b not technically tied to C++20 Modules.
We could have made those changes earlier - and, in fact, we had to make them before we could even
start using C++20 Modules.
It was a true prerequisite and preparing our code for the new \c module, \c export, and \c import
keywords.
  
That said, we did have some concerns about compilation-performance during this transition.
Specifically, we were a bit nervous that abandoning the well-established paradigm of
<b>“One Type, One Header”</b> might negatively impact compile times for the library.
This approach had been great for keeping include chains granular and as lean as possible.
However, we moved away from it in favor of a new strategy: <b>“One Header Per C++20 Module”</b>.
(\ref alib_manual_modules_impludes "Details here")
         
Under the new system, if a small piece of code uses something like the \b String class, it now
also pulls in all of the related string types, corresponding utilities like type traits and so forth.
Sounds inefficient? We thought so too!
   
But here’s where things got interesting: The drop in the compile times of our legacy
(non-C++20 Module) builds before and after the change was only around <b>10%</b>.
Why? There are a couple of reasons for that:

- The reality is, <b>small units that use only one type are rare</b>.
  Even in cases where you start small, you’ll very quickly need another type, or another header,
  and so on. Thanks to indirect dependencies, before you know it, nearly everything gets included
  anyway.
- To boost compilation speed, we were already using <b>precompiled headers</b>, which rendered
  the “keep headers granular” effort largely pointless.
  Honestly, with precompiled headers in play, keeping headers lean is hardly worth discussing!
<p><p>
                                                                                              
\divkt
In the end, this change didn’t impact the (<b>legacy, non-C++20 Module!</b>)-build times
very much. With the added simplicity of module-level headers, it was absolutely worth it.
Sometimes, breaking away from old paradigms isn’t as scary as it seems!
\divend


\I{################################################################################################}
# 3. How C++20 Module Builds Perform# {#alib_c20_modshift_modules}
When transitioning our (now) 25 traditional \alibmods, each representing a distinct namespace
(like alib::strings, alib::boxing, or alib::expressions) into 56 discrete C++20 modules, we
made sure to carefully evaluate and optimize the boundaries between these modules to minimize
compile-time overhead.
             
The result? <b>Strategic segmentation</b>.
While the core namespaces remained intact, we split less frequently used components into
smaller <b>C++20 submodules</b>.
That’s how our 25 \alibmods grew into <b>56 C++ modules</b>!
The relationship between these modules and submodules is detailed in the
manual (\ref alib_manual_modules_impludes "see this section"), where a quick look at the table at
the end tells you everything about the strategy we followed.

But now for the big surprise:
<b>Switching the build system to use C++20 modules caused the overall compilation time for the
library to increase... by almost a <em>factor of 10</em>!</b>

## 3.1 Wait… What? Isn’t C++20 Supposed to Speed Things Up? ##
At first glance, this feels completely counterintuitive.
After all, wasn’t one of the key promises of C++20 Modules to reduce <b>compilation times</b>?
Let’s break it down step by step to uncover what’s going on.
Stay tuned as we unravel what’s happening behind the scenes—and the lessons we learned along the way!

Before diving into the details, it's worth noting that we execute our builds on a
<b>high-performance machine</b> equipped with an AMD Threadripper processor boasting
<b>128 hardware threads</b>.
While this provides significant computational power, it also \e amplifies the bottlenecks
described below.
Because fewer threads are utilized during key stages of a C++20 Module build, the impact is more
pronounced on this system.
To put it in perspective: on machines with fewer threads, the observed increase in build times
might be closer to a <b>factor of 5</b>, rather than 10.
         
<b>Legacy Build Behavior</b><br>
In traditional builds, all `.cpp` files (compilation units) are processed in parallel.
During a fresh build, all hardware threads quickly hit <b>100% workload utilization</b>.
Compilation units are independent of each other, which makes it easy for the build system to
maximize parallelism by utilizing as many threads as available.

<b> Differences with C++20 Module Builds</b><br>
When building with C++20 Modules, the process changes significantly, introducing new bottlenecks:
1. Dependency Graph Construction<br>
   Before starting any compilation, the build system needs to determine the dependency graph for
   all modules. No compilation can begin until this is complete, meaning that 127 out of 128
   threads are left waiting while this step is performed.
2. Module Interface Compilation<br>
   For C++20 Modules, module interface files (e.g., `.pcm` files with Clang) must be generated
   before the implementation files can be compiled. However:
    - Modules that depend on others cannot be processed until their dependencies are built.
    - In the case of \alib, the module dependency graph is <b>deep, not wide</b>
      (see \ref alib_manual_modules_graph "the dependency graph"), further limiting parallelism.
      Only a few modules can be compiled simultaneously due to the sequential nature of the graph.
3. Implementation File Compilation<br>
   Once the module interfaces are built, the process moves on to compiling the implementation files.
   However, further challenges arise:<br>
    - At the time of testing, \alib had <b>199 headers</b> (each covering a few types and
      included by `.mpp` files) versus only <b>116 compilation units</b>.
    - While \alib is far from being a “header-only” library, many core types and templates must
      be provided as headers due to its <b>low-level</b> and <b>general-purpose nature</b>.
    - This imbalance limits opportunities for parallelism during the compilation of implementation
      files.

## 3.2 The Shocking Discovery: Slower Application Builds ##
Perhaps even more surprising than the library build times is this:
<b>Applications consuming the C++20 Modules of \alib also experience an increase in build times.</b>
While we do not see factors, we still see rates of 10% to 30% increase.
Note that we here compare module-based builds to traditional ones that use precompiled headers.
We initially anticipated improvements here: precompiled module interfaces
(which are better tailored to individual compilation units) seemed likely to outperform the
broader precompiled headers of legacy builds.
Unfortunately, current compiler implementations seem to incur significant
<b>overhead when opening and handling C++20 Modules</b> for individual compilation units.
This overhead appears to outweigh the potential benefits of modular builds.
 
## 3.3 A Specific Problem For Low-Level Libraries? ##
We suspect that **implementers of the C++ Standard Library** may encounter similar challenges as
we did.
The need to <b>export a significant portion of overall types</b> can result in higher build times,
even with future toolchain improvements.
While current performance drawbacks are often attributed to tooling inefficiencies, our
analysis suggests that <b>C++20 Modules inherently introduce additional overhead</b>
for <b>low-level, type-rich libraries</b> in comparison to traditional approaches, such as
precompiled headers.

Although we remain optimistic that advancements in compiler technology will alleviate some of
these challenges, we believe it is <b>important to temper expectations</b> regarding the
compilation performance of <b>foundational libraries</b>, such as the C++ Standard Library or
Boost, when adopting C++20 Modules.

# 4. Further Issues With Other Compilers#
We had performed the transition using in its newest version. When this compiled we tried
GCC and the next huge problem appeared: GCC interprets the C++20 standard according to modules
probably more precisely than Clang. Unfortunately, this means <b>more strictly!</b>
With GCC an implementation unit must not reference any types from another module whose implementation
unit in turn references the current module. This is a very strict rule and our source code
violates it in many cases. Again: That is the same source code that compiles well with Clang.
To solve this issue, we had to come up with new "intermediate modules", real artificial ones,
only for the sake of aligning the code to the standard. Given the already huge negative impact
on compile times with the current structure, we decided to postpone this issue until we have more
time to investigate it - or until the standard is more mature.

With MSVC things are even worse: Here, source files must not use the preprocessor prior
to the statement \e module. This is a very strict rule which our source code violates just everywhere,
because this was the only way to allow a dual build setup (with and without C++20 Modules).
Again: That is the same source code that compiles well with Clang.
As of today, we do not see a chance to support MSVC in respect to C++20 Modules.

# 5. Conclusion #
To ensure maximum flexibility, \alib provides support for both <b>modular</b> and
<b>legacy builds</b>.
If <b>compilation speed</b> is your primary priority—particularly with an optimized precompiled
header setup—the traditional header-based version may be the better choice.

Alternatively, if you prioritize using the latest technology and concepts, or are
actively modernizing your codebase to leverage the benefits of C++20 Modules, the modular version
is likely what you want to choose. But remember: Once you choose \alib and in its C++20 Modules
version, you are bound to use the Clang compiler.

This dual-mode support allows \alib to adapt to a wide range of build system requirements and
levels of C++ modernization, ensuring it aligns with your project’s goals and constraints.

\attention
    This conclusion is not true anymore. Currently \alib does not support dual compilation mode.
    See the entry statement. We just left this text untouched to allow you to follow our path
    of thoughts when we originally tried the transition.    
    

<br><br><br><br><br><br> */
