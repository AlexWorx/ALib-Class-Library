// #################################################################################################
//  Documentation - ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_resources   ALib Module Basecamp - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1\. Introduction # {#alib_resources_intro}

\alib uses the term <em>"resources"</em> for string data that technically may be defined in a
constant and static fashion, but that software volunteers to make configurable.
Typical samples of data that software exposes for external management are "themes" (
color and font schemes), or so-called <em>"externalized strings"</em>, which mostly may be used to
translate software into a different "locale" and human language.

While the conceptual definition of \e resources is very similar to the concept of
<em>configuration data</em>, it is good practice to thoroughly decide for each specific
piece of data that is deemed to be made configurable, whether it is \e resource or \e configuration
data. While the mentioned samples of language translation strings and color schemes are
typically \e resources, a server name that software attaches to is typically
<em>configuration data</em>. End users commonly know and understand the difference between the
two intuitively and appreciate if the corresponding data sources are separated, especially as
this way, <em>resource data</em> does not "clutter" the definitions of <em>configuration</em>
sources.

The best rule of thumb to differentiate \e resource from \e configuration data is to check if
it is possible to distribute software without turning the data in question into either of the
two externalization concepts, thus "hard coding" the data.
If so, the data more likely is \e resource data. In the sample of the "server address" above, this
is not possible if such a server address targets a machine that is under the control of an end user.
However, if it is a machine provided by the distributor of the software which is the same with any
installation, then the concept of resourced data rather fits.

\note
  The concept of <em>configuration data</em> is implemented with module \alib_variables.

\I{################################################################################################}
# 2\. Resources and Modules # {#alib_resources_modules}

As documented in chapter #"alib_manual_modules" of the \alib Programmer's Manual,
the various \alibmods_nl can be separated into those who are \alibcamps_nl and those which are not.
In this #"alib_manual_modules_graph;module dependency graph" it could be noted that
all \alibcamps_nl are dependent on module \alib_camp_nl, which in turn is dependent on
this module \alib_resources_nl.

This relation is explained as follows:
- All high-level \alib modules need externalized string resources
  (e.g., for #"alib_mod_exceptions;ALib Exceptions",
  #"alib_enums_records;ALib Enum Records", etc.).
- Resources need a controlled process of bootstrapping software.
- Library class #"Camp" holds a pointer to an object of type
  #"ResourcePool".
- This class furthermore provides strictly defined and phased bootstrapping mechanics, especially
  designed to support the customization of resource management.

Therefore, all \alibcamps_nl not only depend on module \alib_camp_nl, but also dispose
of a singleton type derived from class \b %Camp.

\I{################################################################################################}
# 3\. Data Contract / Conceptual Invariants # {#alib_resources_intro_contract}
\alib resources implemented with this namespace, conceptually impose a set of rules which may be named
a "contract", "invariants" or just "determinations". These rules are given in the following
subchapters.

\I{################################################################################################}
## 3.1 Resource Keys ## {#alib_resources_intro_contract_keys}
Key values to resources are defined as a two-level hierarchy of a \e category and a \e name.
This determination complies with how <em>configuration data</em> is addressed with module
\alib_variables. This is also in alignment with common 3rd-party resource management systems
established in the industry.

Independent of the compilation options of \alib in respect to
#"alib_characters_prepro;choosing the default character width", the string-type of the
\e category and \e name is fixed to #"NString", hence using the narrow #"characters::nchar"
type. This is \b not in alignment with <em>configuration data</em> as defined in \alib_variables.
The rationale for this is that while configuration data categories and names may be translated
themselves to localized versions (preferably by defining those strings as \e resources!), the category and
name strings of the resources are deemed to be hard-coded in the source code that accesses resources.
As such, they are not shared with end-users, are never localized, and should be using the plain
<em>7-bit printable ASCII character set</em>.

\I{################################################################################################}
## 3.2 Restricted To String Type ## {#alib_resources_intro_contract_strings}
All resource data is of #"^String" type (of compilation-dependent character width).

This restriction imposes that any other data type (for example, color codes) has to be
de-serialized (decoded) when resourced.

\I{################################################################################################}
## 3.3 Static Data ## {#alib_resources_intro_contract_static}
Although resource data technically is non static data, conceptually with this implementation
it is.

This determination has the following impacts:
- Resources do not have to be "acquired" and "released" when accessed, which tremendously simplifies
  the use of resources.
- A using code can rely on the fact that the life-cycle of the string buffers of accessed resources
  is infinite. No local copies of the string data is required. (Note, that due to the missing option of
  acquisition/release of resources, copying a resource string would not even be possible.)
- Resource data does not dynamically change during the life-cycle of software process.
  In other words, accessing resource data of a specific \e category and \e name, will
  always result in the same returned value.
- Consequently, an implementation of abstract interface type #"ResourcePool"
  that attaches to a 3rd-party resource system which supports dynamic resources, usually has to
  create a copy of the data returned from the 3rd-party system and return this copy instead of
  the original value.

\note
  The latter point is implemented with built-in, alternative resource type
  #"ConfigResourcePool", which is introduced by module \alib_variables. It allows
  maintaining resources externally, for example, in INI-files.

\I{################################################################################################}
## 3.4 Thread Safeness ## {#alib_resources_intro_contract_threadsafeness}
Accessing resources using abstract interface method #"ResourcePool::Get;*"
is a thread-safe operation.

In contrast to this, an invocation to any of the methods that define resources, namely
#"ResourcePool::Bootstrap;*" and #"ResourcePool::BootstrapBulk;*"
is \b not a thread-safe operation. This is true in respect to each other as well - and most important -
also in respect to parallel resource access with method #"ResourcePool::Get".

This determination has the following impacts:
- Interface methods prefixed with the term <b>Bootstrap</b> of abstract type
  #"ResourcePool" are deemed to exclusively being invoked during
  #"alib_mod_bs;bootstraping" of \alib as well as the of the according software
  using \alib.
  Such bootstrapping has to be performed before starting any threads that potentially modify
  \e resources.
- An implementation of interface #"ResourcePool" may need to impose internal
  protection in respect to race conditions of multithreaded access, if that type was
  used after bootstrap, as \alib itself or custom software still performs read operations after
  bootstrap.
- With built-in types #"LocalResourcePool" and #"ConfigResourcePool",
  no such protection is given.
  Both do not need protection against parallel read operations.
 

\I{################################################################################################}
# 4\. Interface Class ResourcePool # {#alib_resources_interface}
The central type of the module, class #"ResourcePool" was already mentioned several
times. It constitutes a pure abstract interface. Due to the determinations of the concept
given in the previous chapter #"alib_resources_intro_contract", its interface is very simple 
especially in respect to accessing resources, which is exclusively done with the method 
#"ResourcePool::Get;*".

A user of \alib should have no bigger effort to implement this interface and adopt her own or
any 3rd-party "backend" that performs the resource management and the externalization of strings.

Apart from that, two implementations of the interface are provided with \alib. Those are
quickly introduced in the following sections.

\I{################################################################################################}
## 4.1 Class LocalResourcePool ## {#alib_resources_interface_default}
As explained above, an implementation of interface \b ResourcePool has to be constructed during
bootstrap of \alib and distributed among the modules.

In case the bootstrap process is not #"alib_mod_bs_customize;customized", an
instance of class #"LocalResourcePool" is created and shared.

This class does not allow any externalization of resources and simply stores the given pointers
to the static data in a #"HashTable", using
#"alib_mods_contmono;monotonic allocation".

\I{################################################################################################}
## 4.2 Class ConfigResourcePool ## {#alib_resources_interface_config}
A second built-in implementation of class \b ResourcePool which can be created and shared among
the modules of \alib by #"alib_mod_bs_customize;customizing the bootstrap process"
of the library, is given with class #"ConfigResourcePool".

The type externalizes resources by inheriting from class #"Configuration". With that,
flexible ways of externalizing the data are given.
The class is enabled to fetch all "hard-coded" resources fed by camps with methods
#"ResourcePool::Bootstrap" and #"ResourcePool::BootstrapBulk".

If after creation of an instance of the type, this instance is not changed and just used, then
it behaves in an identical way as type \b LocalResourcePool (with only weaker performance and
increased memory overhead).<br>
The huge advantage of this type is, that any custom externalization is achieved along the very same
lines as done with configuration data. Thus, a user of the library that attached such data, will
have no problem in also load resources from external custom sources.

The type is suitable in any situation where no other ("more professional") 3rd-party "backend"
for resource management is available.<br>
Here are some tips for the usage:
- Using class #"IniFileFeeder", it is easy possible to export all "hard-coded" resources
  to an INI-file which are not contained there, yet. Thus an empty INI-file is filled with the
  initial values defined during bootstrap of software.
- Such INI-file could then be used as a "jump-start" for translating software from its
  (hard-coded ) default language into a different one.
- With the prioritization mechanics of class #"Configuration" it is allowed to use
  two or more INI-files, in parallel. This allows, for example, to have language translations of
  resources in place, which - if only sparsely defined - automatically fallback through the
  different configuration priorities until a value is found, latest the hard-coded one.<br>
  In other words, language translations can be stacked and software might allow to
  use not only specify a preferred language but a prioritized list of preferred languages.
- Similar to the previous point two different configuration plug-ins (e.g., INI-files) could be
  used and have one storing only translated resources, the second all resources relevant for an
  application's color scheme ("Theme"). This way, language and color scheme can be chosen
  independently from each other.

\note
  Despite its flexibility, type #"ConfigResourcePool" is provided for convenience.
  It was just a "low hanging fruit" of implementation making use of sibling module
  \alib_variables_nl.<br>
  There are no plans on the road map of \alib to impose a more sophisticated implementation
  for externalized string resources. It is simply not in the domain of this library to provide
  a higher level of functionality. The whole purpose of this module itself is to have an
  abstract and very simple interface into \b any sort of "resources backend" and its choice
  is completely independent of the choice of using \alib with software!


\I{################################################################################################}
# 5\. Indirect Resource Access # {#alib_resources_t_resourced}

\I{################################################################################################}
## 5.1 Type Trait ResourcedTraits ## {#alib_resources_t_resourced_t_resourced}

Sometimes it is required to define resource information, namely
- the #"ResourcePool" instance,
- the resource category and
- the resource name

for use by other components.
Type trait #"ResourcedTraits" may be specialized to do such definition for
C++ types.
A specialization of the struct can be easily implemented using macro #"ALIB_RESOURCED".

A sample for the use of this struct is given with module \alib_variables: To load and store configuration
data, this module exposes a type of #"alib_enums_records;ALib Enum Records" and accepts custom enumeration
types in various interface methods, if they just have this specific record type associated.

Now, if an element of a custom enumeration type that disposes of a specialization of
\b ResourcedTraits is passed to such an interface method, internally this information is used
to load further details of the variable from the resource pool.


\I{################################################################################################}
## 5.2 Helper-Struct Resourced ## {#alib_resources_t_resourced_resourced}

As soon as struct #"ResourcedTraits" is specialized for a type, helper-struct
static struct #"ResourcedType" becomes available.

The type has two overloaded methods #"ResourcedType::Get": The first is parameterless
and simply receives the resource string associated to a type with the specialization of
\b ResourcedTraits. The second takes a replacement for the resource name. This may be used to
retrieve resource strings which are likewise associated to the type.

Furthermore, the struct provides methods #"ResourcedType::TypeNamePrefix" and
#"ResourcedType::TypeNamePostfix" which are meant to provide a standardized way
to define a type's name using resources. The methods are, for example, used with
specializations
#"APPENDABLES::AppendableTraits<TEnum,TChar,TAllocator>" and
#"APPENDABLES::AppendableTraits<TBitwiseEnum,TChar,TAllocator>"
which write enum element names into instances of type #"^AString".

\I{################################################################################################}
## 5.3 Helper-Struct ResourceInfo ## {#alib_resources_t_resourced_resourceinfo}

A next helper-struct is given with #"ResourceInfo" which first of all is a simple
struct that stores resourcing information (the resource pool and category and name strings)
for later use.

While this struct is usable without a specialization of #"ResourcedTraits",
in most use cases it is, because it allows converting the compile-time information given by
\b ResourcedTraits into runtime information.

\I{################################################################################################}
# 6\. Further Details # {#alib_resources_details}

\I{################################################################################################}
## 6.1 Resourced Data Records And Tables ## {#alib_resources_details_data}

Besides just externalizing strings, many use cases require accessing externalized data sets
or even whole tables of this.

\alib module \alib_enumrecords provides a solution for this with its
concept #"alib_enums_records;ALib Enum Records". There, a table of data is addressed
using the C++ type information of enum types. Single records of a table may (or may not) be
addressed by elements of the corresponding enumeration.
The module provides convenient facilities for the fields of the records and for parsing whole 
tables from resourced strings.

Before you go ahead and implement your "own" solution for externalized static data, it might be
worthwhile to check out if #"alib_enums_records;ALib Enum Records" meets your requirements.

\I{################################################################################################}
## 6.2 Exporting Resources For Externalization ## {#alib_resources_details_export}
When resources are externalized, for example, for translation to different human languages,
the list of resources has to be imported to the external "backend". To do so, all resources
have to be queried from the library.

Here is a sample program that performs this task:

 \snippet "../tools/ResourceExport/resourceexport.cpp"     DOX_RESOURCES_EXPORT

The output of this sample can directly be loaded by class #"IniFile", hence with a plug-in
attached to an instance of built-in resource pool implementation
#"ConfigResourcePool". The sample might be adopted accordingly to write a
format suitable to be imported to the backend of choice.

With every update of the library, changes of the resource strings have to be determined.
This might be done, for example, with a custom unit test that compares the default entries
with the ones currently stored in an external backend. New resources might be added, others might
disappear, and worst: existing ones might change their content format. In the latter case, an
externalized resource might be erroneous and lead to undefined behavior.

Starting with library version \b 1903, to support the detection of changes, the version history is found in

        ALIB_BASE_DIR/docs/pages/resource-exports/

\releasetask{Run export tool, create file, add link to new pages below, add a new page at the end of this file}
The following exports are available:
- #"alib_resources_exported_resources_2605"
- #"alib_resources_exported_resources_2511"
- #"alib_resources_exported_resources_2510"
- #"alib_resources_exported_resources_2412"
- #"alib_resources_exported_resources_2402"
- #"alib_resources_exported_resources_2312"
- #"alib_resources_exported_resources_1912"
- #"alib_resources_exported_resources_1903"

\I{################################################################################################}
## 6.3 Debug- And Usage Statistics  ## {#alib_resources_details_debug}

With the provision of the configuration macro #"ALIB_DEBUG_RESOURCES", static field
#"LocalResourcePool::DbgResourceLoadObserver;2" becomes available. If set to <c>&std::cout</c>
before bootstrapping \alib, the resource load process can be observed on the console, because 
the methods #"LocalResourcePool::BootstrapBulk;2" and #"LocalResourcePool::BootstrapAddOrReplace;2" 
will write information on bulk and singular resource data definitions.
This tremendously helps to find errors in resource strings, which often are simply missing commas
and similar.

Next, the virtual methods #"ResourcePool::DbgGetCategories;2" and #"ResourcePool::DbgGetList;2" 
become available. The latter returns a list of all resources defined, including a usage counter. 
The method is only implemented by class #"LocalResourcePool". 
Alternative type #"ConfigResourcePool" does not implement it.

Furthermore, the convenience function #"resources::DbgDump" becomes available which
simply writes the list of symbols into an \b %AString, sorted by category and in alphabetical
order.

The usage counter included in the list may be used to identify two groups of
resource strings:
- Resource strings that have never been loaded, and
- Resource strings that have been loaded very often during the actual execution of software.

While entries of the first type may have become obsolete and are candidates for removal,
those of the second type, software might consider to "cache" the symbol in a variable instead
of repeatedly retrieving it from the resource pool.

Remember: While trivial implementation class \b LocalResourcePool is very fast and resource access
is not really noticeable, other implementations might not be.

The following code snippets taken from the \alib unit tests, demonstrate how to quickly leverage
these debug features, depending on the configuration macro #"ALIB_DEBUG_RESOURCES". The snippets
might be copied to own projects and remain there forever.

Bootstrapping may look as follows:
 \snippet "gtest_main.cpp"     DOX_RESOURCES_DEBUG_BOOTSTRAP1
 \snippet "gtest_main.cpp"     DOX_RESOURCES_DEBUG_BOOTSTRAP2

Before the termination of software (hence, before invoking #"alib::Shutdown;2"), the following code 
may be placed:
 \snippet "gtest_main.cpp"     DOX_RESOURCES_DEBUG_SHUTDOWN

\I{################################################################################################}
# 7\. Sample # {#alib_resources_samplehint}

A comprehensive sample of using \alib resources placed in a custom module is provided with the
#"alib_app_cli_tut;tutorial of ALib Module 'App'".
The sample code provided there can be easily used as a jump start into an own project that
creates a custom \alib module and leverages the resource features provided.

\I{################################################################################################}
# 8\. Resource Compiler # {#alib_resources_compiler}

The higher-level module \alib_camp provides a resource compiler that can be used to generate
C++ code that #"ResourcePool::BootstrapBulk;bulk-loads" resource strings. 
With that, resources are defined in an external text-file, which is much easier to maintain
than the C++ bootstrap code.<br>
Details are found in the chapter #"alib_camp_rc" of the Programmer's Manual of the module
\alib_camp. 


<br><br><br><br><br><br>
\I{################################################################################################}
\I{ Resource export pages   }
\I{################################################################################################}

\page alib_resources_exported_resources_1903           Export of Resourced Strings - V. 1903
\verbinclude "pages/resource-exports/resources-1903.txt"

\page alib_resources_exported_resources_1912           Export of Resourced Strings - V. 1912
\verbinclude "pages/resource-exports/resources-1912.txt"

\page alib_resources_exported_resources_2312           Export of Resourced Strings - V. 2312
\verbinclude "pages/resource-exports/resources-2312.txt"

\page alib_resources_exported_resources_2402           Export of Resourced Strings - V. 2402
\verbinclude "pages/resource-exports/resources-2402.txt"

\page alib_resources_exported_resources_2412           Export of Resourced Strings - V. 2412
\verbinclude "pages/resource-exports/resources-2412.txt"

\page alib_resources_exported_resources_2510           Export of Resourced Strings - V. 2510
\verbinclude "pages/resource-exports/resources-2510.txt"

\page alib_resources_exported_resources_2511           Export of Resourced Strings - V. 2511
\verbinclude "pages/resource-exports/resources-2511.txt"

\page alib_resources_exported_resources_2605           Export of Resourced Strings - V. 2605
\verbinclude "pages/resource-exports/resources-2605.txt"
*/
