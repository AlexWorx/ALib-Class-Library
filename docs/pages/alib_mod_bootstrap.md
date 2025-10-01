// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_bs   ALib Module Bootstrap - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1. Introduction # {#alib_mod_bs_intro}

The term "bootstrapping" means the one-time initialization of the library that is to be performed
in an early stage of a process started on an executing hardware.
The counterpart of bootstrapping is "shutting down" the library.

Standard bootstrapping and shutdown of \alib is performed by invoking the functions #alib::Bootstrap
and #alib::Shutdown.
Such invocation usually is performed as one of the first and last actions in the function <c>main()</c>.
In source code, this simply looks like this:
                                     
\code{.cpp}
        #include "ALib.Bootstrap.H"

        int main( int argc, const char **argv )
        {
            // store cli arguments to global ALib variables
            alib::ARG_C=  argc;
            alib::ARG_VN= argv;

            // bootstrap the library
            alib::Bootstrap();
            ...
            ...
            ...
            // termine the library
            alib::Shutdown();

            return 0;
        }
\endcode

The following chapters explain the details behind these invocations, provide recipes to customize
certain aspects of the process, and explain how  - optionally - the provided mechanics can be
leveraged for the bootstrapping and shutdown of custom code entities likewise.

Bootstrapping and shutdown processes involve all modules that are in need of it. Therefore,
this module is always the topmost module in the module hierarchy.
While this is shown in the \ref alib_manual_modules_graph "hierarchy graph" of \alib's
main manual, there, only optional dependency-arrows to the next lower level camp modules
are shown. This was made to simplify the graph. The truth is that an optional dependency exists
to almost every \alibmod_nl.
These dependency-arrows verbally state: \"If the targeted module is included in the \alibbuild, 
then module \alib_bootstrap_nl will care about performing necessary initialization and shutdown 
actions.\"


\attention If you are new to \alib, you should skip this manual for the time being and rather 
           concentrate on learning how to use \alib.<br>
           The simple recipe above is good for the standard use cases of \alib
           and the rest of this chapter becomes more and more complex and <b>probably a little
           irritating to a new user!</b><br>
           So \ref alib_manual_bs "please go ahead, nothing to see here!"  \emoji :sweat_smile:



\I{################################################################################################}
# 2 Weak Support For Bootstrapping In C++ # {#alib_mod_bs_cpp}

The following circumstances increase the complexity of bootstrapping:

<b>1. The C++ Language:</b><br>

\par
The C++ language specification misses sophisticated support for bootstrapping software process.
Basically, the only two things that are guaranteed by the language are:
- Global and static data will be initialized.
- After this is done, a function called <c>main()</c> is invoked.

\par
This specification especially misses an order of the initialization of global or static data.
Note that data initialization might execute arbitrary constructor code (before method <c>main()</c>
is invoked!), hence also the order of such custom code execution is random.

<b>2. Resources And Configuration Data:</b><br>

\par
Bootstrapping becomes even more complicated with the use of
1. Resources (for example externally managed string resources) and
2. Configuration data (e.g., configuration files, command line parameters, etc.).

\par
With \alib, such custom data sources might furthermore be provided by plug-ins, which means the code
entity that performs the provision of resources and configuration data needs to be initialized
before the data is used.

<b>4. Module Dependencies:</b><br>

\par
The dependencies between the modules have to be respected during initializations.
As mentioned already, dependent modules have to be initialized first.

<b>5. Multithreaded Access:</b><br>

\par
Multithreaded software must implement measures to prevent undefined behavior caused by race 
conditions when accessing shared resources, often shared memory. 
These measures, however, come with a performance cost. 
If all modifications to the shared memory can be confined to a one-time initialization during 
the bootstrap phase, such protective steps may be skipped, provided the bootstrap process ensures 
single-threaded access during that period and subsequent operations are read-only.
             
An example of this scenario is externalized resource string data: once such data is fetched during 
bootstrap, it is treated as constant and accessed read-only thereafter.


\I{################################################################################################}
# 3. Bootstrapping Non-Camp Modules # {#alib_mod_bs_nocamps}
Some of the \ref alib_manual_modules_camps "non-camp modules" have a requirement to perform some 
bootstrap and shutdown actions. 
These calls are performed internally with the invocation of methods #alib::Bootstrap and 
#alib::Shutdown as sampled above.<br>
The library here respects the module dependencies and selects code according to the \alibbuild.

Thus, a user of the library does not need to care about the details but just needs to
include \implude{Bootstrap} and invoke the functions at the beginning and end of 
method <c>main()</c> as shown in the introduction.

More complex boot strapping strategies with some user involvement can be necessary when 
it comes to \alibcamps, especially if a user decides to implement custom \alibcamps_nl.

\I{################################################################################################}
# 4. Bootstrapping ALib Camps # {#alib_mod_bs_camps}
When \alibcamps are included in the \alibbuild, then things can become a little more complex.

But first of all the good news: Even in the usual case that one or more \alibcamps_nl are included 
in an \alibbuild, bootstrapping and shutdown of the library is done exactly the same
as \ref alib_mod_bs_intro "shown above". In other words:

\attention 
  The following chapters about bootstrapping should be read by you only if you want to modify
  the bootstrap process for one of the following reasons:
  - A custom \alibcamp_nl was built, which should be integrated in the bootstrap process
  - Customized configuration and/or resource-data setups or mechanics should be
    implemented
  - Other custom reasons, not foreseen by the author of this text.

If the above does not apply to your field of application, all you might need to know is that
functions #alib::Bootstrap and #alib::Shutdown are internally largely extended in the presence of
\alibcamps_nl but, it is still the same simple call.

\I{################################################################################################}
## 4.1 ALib Camps Bootstrap Interface ## {#alib_mod_bs_camps_if}

For bootstrapping and shutdown, types derived from class \alib{camp::Camp} need to implement two
protected, \b abstract methods, namely 
- \alib{camp;Camp::Bootstrap} and
- \alib{camp;Camp::Shutdown}.

Both methods are invoked more than once: Bootstrapping is done in <b>three phases</b>, defined by
the \c enum type \alib{BootstrapPhases} and shutdown is done in <b>two phases</b>, defined by
enumeration \alib{ShutdownPhases}.
The implementations of the methods need to \c switch upon the given levels and perform different 
tasks.

By default, each phase will be finished for all camps, before a subsequent phase is initiated.
The loop over the phases and its inner loop over the list of camps is implemented with
namespace functions:
- \doxlinkproblem{namespacealib.html;a78bb34888e5142adb87e265e23ee3c2e;alib::Bootstrap(BootstrapPhases, camp::Camp*, int,int,TCompilationFlags)},
  respectively
- \doxlinkproblem{namespacealib.html;a4374717714582ef1a859b773367ec5d9;alib::Shutdown(ShutdownPhases, camp::Camp*)}.

\note
    These overloaded versions are available automatically with the inclusion of the module 
    \alib_camp in the \alibbuild_nl.
    The versions with less parameters we had seen before are then not available.<br>
    Due to the fact that all parameters are defaultet, still the invocation remains the same
    in case the bootstrap-process should not be adjusted.

\I{################################################################################################}
## 4.2 Bootstrap Phases ## {#alib_mod_bs_camps_bs}

With function \doxlinkproblem{namespacealib.html;a78bb34888e5142adb87e265e23ee3c2e;alib::Bootstrap(BootstrapPhases, camp::Camp*, int,int,TCompilationFlags)},
an outer loop over all phases is initiated. An inner loop iterates over all camps, in order of their
"dependency level". For each combination of phase and camp, virtual method \alib{camp;Camp::Bootstrap}
is called. This ensures that for each phase, "low level camps" are initialized first and those of
"higher level" are initialized next.

The three phases are defined as follows:

1. \alib{BootstrapPhases::PrepareResources}<br>
   All initialization actions that exclusively involve static data, is deemed to be performed
   in this phase. For example, the \alib{boxing;BootstrapRegister;registration} of
   \ref alib_boxing_functions "box-functions" is to be done in this phase.

   The phase received its name from the fact that the  \alib{resources;ResourcePool} is already
   available and a camp is supposed to feed its static default resource strings to the instance
   retrieved with \alib{camp;Camp::GetResourcePool}.

   More on this topic will be explained in a later chapter.<br>


2. \alib{BootstrapPhases::PrepareConfig}<br>
   In this phase, the configuration object is available and receivable with \alib{camp;Camp::GetConfig}.
   It may now be extended with plug-ins, but <b>access to configuration variables is not allowed yet!</b> <br>
   For most camps there is not much to do in this phase.<br>
   One important action usually placed here is to initialize static
   \ref alib_enums_records "ALib Enum Records". If - as it is recommended -
   \ref alib_enums_records_resourced "record definitions are given using externalized strings",
   then this is definitely the right place to invoke one of the overloades of 
   \alib{enumrecords::bootstrap;Bootstrap} for each enumeration in question.
   \note
     While the enumeration records are static data and could also be initialized in the first
     phase, after the definition of resources, placing it in this phase allows camps of higher
     dependency levels to modify (patch!) the resources of a lower level camp - before their use.

   <br>

3. \alib{BootstrapPhases::Final}<br>
   In this phase, final initialization steps may be performed.<br>
   Only in this phase the <b>start of threads is allowed (!)</b>, as any mandatory \alib initializations which
   are not protected against racing conditions are deemed to be performed in the previous phases.<br>
   Furthermore, access to configuration variables is allowed in this phase.


\I{################################################################################################}
## 4.3 Shutdown Phases ## {#alib_mod_bs_camps_sd}

With function \doxlinkproblem{namespacealib.html;a4374717714582ef1a859b773367ec5d9;alib::Shutdown(ShutdownPhases, camp::Camp*)},
an outer loop over the two shutdown phases is initiated.
Again, an inner loop iterates over all camps, but this time in \b reverse order of their
"dependency level". For each combination of phase and camp, the virtual method 
\alib{camp;Camp::Shutdown} is called. 
This ensures that for each phase, "high level camps" are shut down first and those of
"lower level" are shutdown later.

The two phases are defined as follows:

1. \alib{ShutdownPhases::Announce}<br>
   Within this phase, a camps can rely on the full stability of software and all camps.
   Only actions might be taken that do not result in defects if features of a camp are still
   used. Typical samples of actions to perform with this phase are writing out configuration data or
   making an application's state persistent, to be able to restore it with a next run of a
   software.<br>
   Also, with this signal, running threads should notified to terminate and be joined.
   <br><br>

2. \alib{ShutdownPhases::Destruct}<br>
   This is the "true" shutdown phase. After this phase was invoked, a camp is obligated to have
   freed all its resources and is allowed to be dysfunctional afterwards.


\I{################################################################################################}
# 5. The List Of Camps # {#alib_mod_bs_list_camps}

For default bootstrapping and shutdown, this is almost all we need to know. 
The only remaining question is:
Where is the list of \alibcamps_nl, which is used for the inner loops of functions \b Bootstrap and
\b Shutdown, defined, so that I can plug my custom module in?

The list is a public namespace instance given with \ref alib::CAMPS.
Along with it, function \ref alib::BootstrapAddDefaultCamps is provided, which fills this list with
the camps delivered with \alib (and selected in the \alibbuild) in the right order. 
The right order means: respecting the \ref alib_manual_modules_graph "dependency hierarchy".

With a complete \alibbuild_nl, the list will be:
1. \ref alib::BASECAMP
3. \ref alib::ALOX
4. \ref alib::CLI
5. \ref alib::EXPRESSIONS
6. \ref alib::FILES

The list is traversed from \b 1 to \b 6 with bootstrapping and in reverse order when
the camps are shut down.

Now, if function \ref alib::Bootstrap is invoked without explicitly filling the list beforehand 
(right as it was shown in the introductory section of this chapter), then the list will
be detected to be empty and function \b BootstrapAddDefaultCamps is automatically invoked.

In later sections of this manual, options for modifying these defaults will be demonstrated.

\I{################################################################################################}
# 6. Standard Bootstrapping # {#alib_mod_bs_standard}
With the knowledge taken from the previous sections, it is now easily understood what was said
in the introductory chapter:
\par
    <em>Standard bootstrapping of \alib is performed by invoking function \alib{Bootstrap}</em>

Consequently, the simple version of bootstrapping was given like this:

        int main( int argc, const char **argv )
        {
            alib::Bootstrap();
            ...
            ...


This standard invocation of function \alib{Bootstrap} not only fills the list \ref alib::CAMPS,
but also invokes overloaded function<br>
\doxlinkproblem{namespacealib.html;a78bb34888e5142adb87e265e23ee3c2e;alib::Bootstrap(BootstrapPhases, camp::Camp*, int,int,TCompilationFlags)}.
<br>
with parameters:
- \p{targetPhase}= \alib{BootstrapPhases::Final}, and
- \p{targetCamp} = <b>alib::CAMPS.Back()</b>

The loop iterating the phases, starts with the first phase that the camps have \b not been
bootstrapped with, yet, and ends with the given \p{targetPhase}. With that, all phases are executed
in the default case. 
And because the target camp is set to the last one in the list, the inner loop covers all camps.

What was not discussed yet is when the instances of \alib{resources;ResourcePool} and
\alib{variables::Configuration} are created and how the corresponding pointer
members \alib{camp;Camp::resourcePool} and \alib{camp;Camp::config} are set.

This is how this is done:
- An instance of class \alib{resources;ResourcePool} is created before executing the first
  phase \alib{BootstrapPhases;PrepareResources} and is distributed among the camps.<br>
  The distribution is performed in reverse order, starting with the given \p{targetCamp}
  and from there to the lower level camps.

- The same is done with a new instance of class \alib{variables;Configuration}, but this is
  done only before executing second phase \alib{BootstrapPhases;PrepareConfig}.


The following schematic summarizes this:

  - Outer loop: Phase PrepareResources
    - Creation of a resource pool.
    - 1st inner loop: Distribution to lower level camps in reverse (descending) order.
    - 2nd inner loop: Invocation of <em>bootstrap( PrepareResources )</em> on all dependent camps, in ascending order.
  - Outer loop: Phase PrepareConfig
    - Creation of configuration instance.
    - 1st inner loop: Distribution to lower level camps in reverse (descending) order.
    - 2nd inner loop: Invocation of <em>bootstrap( PrepareConfig )</em> on all dependent camps, in ascending order.
  - Outer loop: Phase Final
    - Invocation of <em>bootstrap( Final )</em> on all camps, in ascending order.

With this information, the standard process of bootstrapping is well-defined. 
The following chapters now introduce different ways to customize bootstrapping.

\I{################################################################################################}
# 7. Customizing The Bootstrap Process # {#alib_mod_bs_customize}

\I{################################################################################################}
## 7.1 Bootstrapping Custom Camps ## {#alib_mod_bs_custommods}
As explained in the previous chapter \ref alib_mod_bs_cpp, the reason why bootstrapping
\alib is a non-trivial task, does not lie in specifics of this library, but in a general lack of
support for bootstrapping in the C++ language. Any more complex software needs to solve this
task.

Consequently, it might be a reasonable decision to adopt what this library offers and use this
concept and the mechanics to bootstrap custom code units likewise.

Doing this is rather simple. The steps are:
1. Create a custom type derived from class \alib{camp;Camp} along with a global singleton instance 
   of this type.
2. In the <c>main()</c> function, invoke \alib{BootstrapAddDefaultCamps}.
3. Next, use method \alib{containers;List::push_back} to add the static instance of the custom
   camp type(s) to variable \alib{CAMPS}.
4. Perform bootstrapping by invoking function \ref alib::Bootstrap.

\note A complete source code sample is given with the \ref alib_cli_tut "tutorial of ALib Camp CLI".
      Here is the corresponding excerpt from the its <b>main()</b>-function:
      \code{.cpp}
        int main( int argc, const char **argv )
        {
            alib::ARG_C  = argc;
            alib::ARG_VN = argv;

            // 1. Add our custom camp to the list of camps
            alib::BootstrapAddDefaultCamps();
            alib::CAMPS.push_back( &SAMPLE_CAMP );

            // 2. Initialize all modules
            alib::Bootstrap();
            ...
            ...
      \endcode


In more complex scenarios, software can also create more than one module.
Each camp may be appended to list \ref alib::CAMPS (see step 3). If these camps are following a
dependency hierarchy, then the lowest (most independent) camps should be attached first.
The target camp of the bootstrapping (step 4) is the latest camp added.

This recipe addresses the simple cases. When software chooses to hook itself into the bootstrap
mechanism of \alib as shown above, the resources and configuration data (including for example INI-files)
are shared between all modules and thus contain both, data of \alib and that of custom camps.

Often, this is not a desired scenario, because:
- software does not want to bother an end-user with the rather technical resources and
  configuration variables provided by \alib. Cluttering custom data with \alib data should be avoided
- software might want to disallow an end-user to modify configuration data and/or resources
  of \alib.
- etc.

How these goals can be reached is explained in the next sections.

\I{################################################################################################}
## 7.2 Using Custom Resources and/or Configuration Plug-ins ## {#alib_mod_bs_customize_resconf}

The default implementation of class \alib{resources;ResourcePool} used with non-customized
bootstrapping is of type  \alib{resources;LocalResourcePool} and this does \b not externalize
the resource strings. To use either alternative, built-in type \alib{variables;ConfigResourcePool}
or a custom implementation of the interface that attaches to a 3rd-party "resource pool backend",
all that needs to be done is to create such instance and pass it to method
\alib{camp;Camp::BootstrapSetResourcePool} of the last (!) camp in the global list 
\ref alib::CAMPS.<br>
This has to be done \b before the first invocation of function
\doxlinkproblem{namespacealib.html;a78bb34888e5142adb87e265e23ee3c2e;Bootstrap;alib::Bootstrap(BootstrapPhases targetPhase, camp::Camp* targetCamp, int,int,TCompilationFlags)}.

As documented with class \alib{variables;Configuration}, the type allows being equipped with custom
mechanics for loading external data. To do so, the bootstrapping process has to be interrupted
after the configuration is internally created. This is done by explicitly specifying
\alib{BootstrapPhases::PrepareConfig} with function \b Bootstrap.

Now, the configuration can be accessed with \alib{camp;Camp::GetConfig} and
is manipulated as wanted. When done, bootstrapping is finalized by invoking \b Bootstrap a
second time with argument \alib{BootstrapPhases::Final}.

The schematic that shows both approaches at once, then looks as follows:
- Create instance \b myResources on the last camp in \ref alib::CAMPS.
- Invoke \b BootstrapSetResourcePool(myResources).
- Invoke \b Bootstrap(PrepareConfig).
- Modify configuration object received with \b GetConfig (on any of the \alibcamps_nl, because
  they still all share the same instance).
- Invoke \b Bootstrap(Final).


\I{################################################################################################}
## 7.3 Using Multiple Resources and/or Configuration Instances ## {#alib_mod_bs_customize_non_shared}

With the bootstrap process described so far,
- a different (also custom) type implementing the class \alib{resources;ResourcePool}, and
- an - potentially customized instance of class \b Configuration

is attached.

To also add a different configuration instances, method \alib{camp::Camp;BootstrapSetConfig} is
given, just like the already introduced method \alib{camp;Camp::BootstrapSetResourcePool}.

Now, both methods may be called on arbitrary camps, \b before starting bootstrapping the 
corresponding phase!
An object set this way will \b not be overwritten when the corresponding bootstrap phase distributes
instances from higher- to lower-level camps. Moreover, if the algorithm finds an object already
set, then the distribution loop continues with this object!
This means that from there on, this instance is passed downward.

This allows a rather natural way to separate all camps into logical sets that share one
resource pool and/or configuration.

\I{################################################################################################}
## 7.4 Separating Bootstrap Phases ## {#alib_mod_bs_customize_separated}
The fact that method
\doxlinkproblem{namespacealib.html;a78bb34888e5142adb87e265e23ee3c2e;alib::Bootstrap(BootstrapPhases, camp::Camp*, int,int,TCompilationFlags)}.

is allowed to be called with setting parameter \p{targetCamp} to a "lower level" camp
(one that is not the last of the list \ref alib::CAMPS), allows bootstrapping lower-level camps separately
from bootstrapping higher-level ones.

As discussed in previous section, such approach might be needed in case that custom camps are integrated
into the \alib bootstrap process.

Let's consider the following sample:
1. Custom camp \b MyApp is derived from class \alib{camp::Camp}
2. In the <c>main()</c> function, \alib{BootstrapAddDefaultCamps} is invoked.
3. The singleton of \b MyApp is added to the end of list \alib{CAMPS} (hence on the last \alib camp)
4. Bootstrapping is fully performed on the second last singleton in list \b CAMPS.
5. Bootstrapping is afterwards fully performed on singleton \b MyApp.

The consequences from this approach are:
- The set of \alibcamps_nl share a different resource pool and configuration object than \b MyApp.
- During the bootstrapping of \b MyApp, built-in \alibcamps_nl are already fully bootstrapped and functional.

\note If you want to test that you have understood the mechanics of bootstrapping \alib, you might
      answer the following question for yourself: Why can step 5 above be done by simply
      invoking function \ref alib::Bootstrap() keeping the default parameters?

Alternatively the same can be reached with the following recipe:
1. - 3. same as above
4. Create instance of class \b ResourcPool and set it on the last \alibcamp_nl in \ref alib::CAMPS.
5. Invoke \b Bootstrap(PrepareConfig).
6. Create an instance alib_Configuration and set it also on the last \alibcamps_nl  in \ref alib::CAMPS.
7. Invoke \b Bootstrap(Final).

\I{################################################################################################}
## 7.5 Conclusion ## {#alib_mod_bs_customize_conclusion}
We admit, that what was said above is a little complex. But once you understand why both recipes
given above lead to the same result, you can tell that you understood
<b>ALib Bootstrapping</b>!<br>
With this toolset and the knowledge that:
- Both recipes can be mixed,
- Custom resource pool instances might load resources externally, instead of compiling them into
  an executable's data section,
- Different camps may use different resource pools
- The same is true for configuration data
- Completely other custom operations can be performed in the different bootstrap phases defined with
  \alibcamps_nl,
- etc.

... bootstrapping software becomes a very well defined and organized undertaking!


\I{################################################################################################}
# 8. Customization Of Shutdown # {#alib_mod_bs_custom_shutdown}
Customization of shutting down \alib should be a seldom need and works the same in principle.
Everything is done in reverse order.
Function \doxlinkproblem{namespacealib.html;a4374717714582ef1a859b773367ec5d9;alib::Shutdown(ShutdownPhases, camp::Camp*)}
has defaulted parameters \p{targetPhase} and \p{targetCamp} and if not specified, all camps
in list #alib::CAMPS are shutdown over both phases. For this, parameter \p{targetCamp}, this time
defaults to the first camp in the list.

if \p{targetPhase} is specified as \alib{ShutdownPhases;Announce} then only the first
phase is performed and custom actions might be taken before invoking the second phase
\alib{ShutdownPhases::Destruct}.

For simplicity, in contrast to bootstrapping the resource and configuration objects are
destroyed right in the reverse loop of phase \b Destruct, in contrast to adding a post-process
second loop and done with bootstrapping. Thus, a "lower" camp's resources and config objects
might be already destroyed, at the moment the camp itself is destructed. Hence, phase \b Announce
is the true last point of action in respect to an involvement of these objects.


\I{################################################################################################}
# 9. Assuring Compilation Compatibility # {#alib_mod_bs_assertcomp}

Due to the facts that:
- \alib allows reducing to a subset of its functionality by changing the \alibbuild_nl, and
- \alib provides a set of compilation flags, which for example switches default character sizes,
        or turns other features on or off, and also because
- \alib follows the policy of having the freedom to quite radically changing the internal structure
         and types with any new version, in other words is never "downwards compatible",

it is important that a compiled \alib library must be incompatible to compilation units
that actually use the library. For example, the footprint (size) of library types might be different.

With C++, if incompatible compilation units are mixed, often no warning is issued (neither by
the compiler nor the linker) and the execution of a process might lead to very strange behavior
which is irritating and hardly to debug.

To detect incompatible compilation settings and library versions, three preprocessor symbols have
been defined. Each symbol has a corresponding namespace variable compiled into the library.
The symbols and variables are:

Preprocessor Symbol          | Namespace Variable
-----------------------------| ---------------------------------
\ref ALIB_VERSION            | \ref alib::VERSION
\ref ALIB_REVISION           | \ref alib::REVISION
\doxlinkproblem{group__GrpALibPreproMacros.html;gaac71b6511690f5af0abf5213b0434111;ALIB_COMPILATION_FLAGS;ALIB_COMPILATION_FLAGS}   | \ref alib::COMPILATION_FLAGS

Along with that, namespace function \ref alib::AssertALibVersionAndFlags is given, which expects
comparison values for each of the namespace variables. The little trick here is, that the
parameters are defaulted with the macros. Consequently, the parameters must not be given when
the function is invoked.

Furthermore, the parameters are also silently (aka defaulted) added to overloaded functions
\ref alib::Bootstrap, which internally invokes \b AssertALibVersionAndFlags().

<b>For this reason, nothing specific has to be done by a user of the library, as the checks are already
performed with bootstrapping!</b>

\note
   With \e CMake builds, if \e CMake function \ref alib_manual_build_cmake_5 "ALibSetCompilerAndLinker"
   is used with a custom target, all settings are guaranteed to be compatible to a library that
   is built within the same CMake process.

<br><br><br><br><br><br> */
