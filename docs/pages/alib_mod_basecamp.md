// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_basecamp   ALib Module BaseCamp - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1. Introduction #  {#alib_basecamp_intro}

This \alibcamp is the fundament for all other \alibcamps_nl.
An explanation to this statement is given in chapter
\ref alib_manual_camp_modules "3. ALib Camps and Module BootCamp" of the Programmer's Manual of
\alib.

In addition to that, this module aggregates different sets of classes, which did not find
another home and which absolutely made sense to place in this foundational \alibcamp_nl.

As an exception from the general rules of organizing header files and namespaces, this
\alibmod_nl, respectively \alibcamp_nl, adds files to the following subfolders of
\alibsrcdir{lang}, namely:
- \alibsrcdir{lang/format},
- \alibsrcdir{lang/message},
- \alibsrcdir{lang/resources} and
- \alibsrcdir{lang/system}.

These files expose their types in corresponding inner namespaces of \ref alib::lang, except (and that
is a next exception of the rules), that the types defined with headers in \alibsrcdir{lang/message},
expose their types directly to namespace <b>%alib::lang</b>.

The following chapters address these four sets of header files.

\I{################################################################################################}
# 2. Types Defined with Header Files in Directory "alib/lang/message" {#alib_basecamp_message}
As just explained, the following types defined in headers and compilation units of
\alibsrcdir{lang/message} are exposed in namespace \ref alib::lang.
This is because they are very fundamental and provide general \alib functionality.


## 2.1. Class Message ## {#alib_basecamp_message_message}

When measured in source code, the size of class \alib{lang;Message} is very short.
Nevertheless, as the type leverages almost all features of module \alib_boxing, it is a quite
powerful tool to have.

The type acts as an information object of the central \b throwable type \alib{lang;Exception},
discussed in the next chapter. While class \b Exception stores a whole list of messages, only one
single object is used with class \alib{lang;Report}, which is discussed in the final chapter of
this small manual.

\I{################################################################################################}
### 2.1.1 Source Location ### {#alib_basecamp_message_message_location}
Objects of type \alib{lang;Message} relate to a source code location, which is reflected by field
\alib{lang::Message;CI}.

The inclusion of the \b Function, which is of course a redundant piece of information, already
indicates that this source information has a general notion of "debug information" and at least is
supposed to be presented to experts only. <br>
Interface methods which create a \b %Message object and for this expect the caller's source
information, are usually invoked using a preprocessor macro to provide the three arguments.
There are two different macros defined for this purpose, \ref ALIB_CALLER and \ref ALIB_CALLER_NULLED.
The first macro provides the corresponding built-in preprocessor symbols for the source file,
line number and currently compiled method/function name in any compilation configuration.
The second macro is equal to the first only in debug-compilations. In release (or other non-debug)
builds, macro \ref ALIB_CALLER_NULLED provides \e nulled values for the arguments.

It depends on the using software which macro should be used. It must be noted that with the use
of macro \ref ALIB_CALLER, even in release compilations, information about the name and location
of C++ source code files and method names is included in the data segment of the executable or
library built. 
Hence, this is rather a "political" or "commercial" question to answer.
Internally, \alib exclusively uses the "pruning" version \ref ALIB_CALLER_NULLED. 
This is because otherwise using software's executable would include source information of \alib 
which may not be wanted.

\note
  There is a third macro existing with \ref ALIB_CALLER_PRUNED, which is empty with non-debug
  compilations. This macro is to be used with code that completely omits source information
  parameters in release compilations, which is not true for class \b Message and therefore not
  applicable with the creation of \b Message objects.

\I{################################################################################################}
### 2.1.2 Message Types ### {#alib_basecamp_message_message_types}
To define the type of a message, class \alib{boxing;Enum} provided by module \alib_boxing
is used, by letting field \alib{lang;Message::Type} be of type \b %Enum.

This leads to having a two-level hierarchy for defining message types. What is meant by this,
should be explained by looking at other options:
- If field \b %Type was of integral type, it would be no hierarchy, but just a "flat" numbering
  system. With that it would be very problematic to find a numbering scheme that allows software
  modules that do not "know of each other" to define message types without ambiguity.
- If the field was of a virtual base type, it would be a hierarchy of arbitrary depth: A message
  type class might be derived from the virtual base class, which in turn has derived types.
  Now, when investigating into a message's type, a user could try to dynamically cast the base
  type to a certain derived message type and on success would know that the message is of that
  type or a derived type.

By using class \alib{boxing;Enum}, there are exactly two hierarchy levels: the first is
accessed with method \alib{boxing;Enum::IsType}. With that it can be determined if the
"boxed" enumeration element is of a certain scoped enum type. To check both levels at once,
\alib{boxing;Enum::operator==} can be used.

\note
  - While this is not recommended in common use cases, method \alib{boxing;Enum::Integral} allows
    testing only the second level of the hierarchy, which is the underlying enum element's
    integral value.
  - It is also not recommended to use non-scoped (anonymous) enumeration types. While two
    elements of two anonymous enum types are well distinguished with \alib{boxing;Enum::operator==},
    the use of \alib{boxing;Enum::IsType} is rather tricky and not well readable: the declaration
    type of a sample element has to be given as the template parameter to the method.

\I{################################################################################################}
### 2.1.3 Message Arguments ### {#alib_basecamp_message_message_args}
The contents of the message is comprised by a list of boxed objects. For this, class \b Message
inherits class \alib{BoxesMA}, which itself is a container (vector) of objects of type
\alib{boxing;Box}. The interpretation of the data given in a message is defined by an application.
As described in a later section of this manual, \alib class \b Exception, which includes a whole
list of \b Message objects, proposes a certain schematic for the attached data.
But also in this case, the proposal is just a recommendation, not a mandatory requirement.

\I{################################################################################################}
### 2.1.4 Memory Allocation ### {#alib_basecamp_message_message_alloc}
As documented with class \alib{boxing;TBoxes}, its underlying <c>std::vector</c> may use
either standard dynamic memory or "monotonic allocation" introduced by module \alib_monomem.
With class \alib{lang;Message}, monotonic allocation is chosen. A reference to the allocator has to
be passed with construction.
at that moment and only be initialized before adding information to the message. This
supports to derive from class \b Message which hold the allocator instance of a member.

\I{################################################################################################}
### 2.1.5 Argument Life-Cycle ### {#alib_basecamp_message_message_lifecycle}
Restricted life-cycles of a message's arguments is a potential pitfall.
Because class \alib{lang;Message} is a \ref alib::BoxesMA "vector of boxes" (by inheritance),
for non-trivial arguments, a pointer to the corresponding original object instance is stored.

\note
  For details on how a value of a certain type is "boxed" when added as an argument to class
  \b Message, wider parts of the programming manual of module \alib_boxing have to be understood.
  As a rule of thumb: objects that have a bigger size than 2 x 64 bit (respectively 2 x 32 bit on
  a 32-bit system) will not be copied, but a pointer to the original is stored.

Especially in the case of C++ exception handling (what the type is used for), this means that
boxed objects must not be stack-allocated, because the stack is "unwinded" when the exception is
thrown.<br>
Therefore, (complex) stack-allocated objects and all other objects that might not survive the
life-cycle of the message, need to be copied if they should be used as a message argument.

One solution for this and similar cases is already offered by module \alib_boxing with
built-in box-function \alib{boxing;FClone}.
This function expects an object of type \alib{MonoAllocator} that is used to allocate
memory for cloning the contents of boxed data.

\see
  For more information, see the \alib{boxing;FClone;reference documentation} of box-function \b FClone
  and chapter \ref alib_boxing_more_iclone of the Programmer's Manual of module \alib_boxing_nl.


\I{################################################################################################}
\I{################################################################################################}
## 2.2. Exceptions ## {#alib_basecamp_message_exceptions}

\I{################################################################################################}
### 2.2.1 Exception Handling Paradigms ### {#alib_basecamp_message_exception_paradigm}

Class \alib{lang;Exception} is used within \alib as <b>the one and only type</b> thrown with
C++ keyword \c throw.

Different paradigms or "design pattern" exists for the C++ language, proposing  how to implement
exception handling. There is a lot of discussion about this going on in the community and different
paradigms have different advantages. For example, one alternative is to throw one exception object
and while the stack is unwinded, this original exception gets replaced (by intermediate exception
handlers) with a more meaningful one.
This replacement object might be derived from the original one or belong to a
different exception hierarchy. For example, if a resource string cannot be loaded, a type of
"resource exception" may be thrown. Then, the caller of this method has different information,
for example that it cannot access a server (because the server name was meant to be read from the
resource) and now replaces the resource-related throwable with a server-access-related one.


\alib adheres to a different pattern and class \alib{lang;Exception} supports, respectively
implements this.
An exception once thrown is never replaced. Instead, while unwinding the stack new "entries"
are added to the exception. The entries are of type \b %Message, introduced in the first chapter
of this manual.

Now, a new entry can have two effects:
- Either the new message is changing the meaning of an exception, or
- the new message extends the existing meaning of an exception by adding detail information
  to the exception.

While this sounds complicated, it is not: the whole class is not much more than a list of objects
of type \alib{lang;Message} and the exception information passed with the constructor of the class,
simply becomes the first entry in the list. Each new message, regardless if the message
changes the meaning of the exception or extends it with detail information, is simply appended
to the list of entries. The exception just keeps track of the most recent entry that changed
the meaning, while the overall order of message entries is always kept intact.

With this approach, all information "along the call stack" is preserved in the order of the
information occurrence.

This allows:
- Exception handling code to inspect "older" entries and perform different actions depending
  on the original cause.
- Exception output that gives more meaningful information about what happened exactly, e.g.
  during software development or when logging exceptions "from the field" for later analysis.

Just to be clear: class \b %Exception is not designed to be a base class for derived exception types.
The recommended use of the class is that the type itself is the exclusive throwable. Consequently,
when catching \alib exceptions, only one \c catch block is needed (possible) and within this catch block
code performs different actions depending on the exception entries.

Of course, a user of the library might break this rule and derive own exception types, but it is
really not recommended. Think about it first! No \alibmod uses derived types and only this type
needs to be "caught" when invoking \alib code that throws.


\I{################################################################################################}
### 2.2.2 Exception Types ### {#alib_basecamp_message_exception_types}

With the paradigm/design pattern implemented (as discussed in the previous introductory section),
usually an important problem is raised: What can be used as an "error type" and how can it be
ensured that error types defined in independent code units of a software are not ambiguous?
Remember that the more common approach to implement exceptions, which is also taken in the
C++ standard library that defines class <c>std::exception</c> to be a general base class for
exceptions, such ambiguities do not occur: There, the C++ type system is used to distinguish
exceptions.
With the \alib implementation however, the entries need to use a "type-code" and these codes must
not overlap.

This problem is solved by using type \alib{lang;Message} as entries of class
\alib{lang;Exception}. As discussed in previous chapter \ref alib_basecamp_message_message_types,
this way a two-level hierarchy of exception types is established, where the first level uses
run-time type information and hence no clash of exception types may occur, even between different
code units.

There is one question left: Which of the messages in the collected list now does define the resulting
"exception type"? Is it the first message added? Or the last one?
In the paradigm \alib uses it might be any of them, or even one in-between.

The exception type is defined as:
<b>"The type of an exception is defined by the most recently added message, who's enum element's
integral value is positive."</b>

This \b Enum is searched and returned with method \alib{lang::Exception;Type}.
With this definition, enumeration element values of exception message entries that are considered
"additional information" are to be declared with a negative integral initializer, while
those that constitute an own exception type, are to be declared with a positive value.

Consequently, if a catch handler uses method \b Type, it does not need to
switch over just all possible entry types (aka enumeration elements), but just over those that are
positive and that therefore bring a different meaning to an exception.



\I{################################################################################################}
### 2.2.3 Self-Contained Allocation ### {#alib_basecamp_message_exception_smv}

When an exception is thrown, something "bad" happened. In that situation a software should not
perform too many complex things, for example dynamic memory allocations.

Simple exception types are therefore very lightweight, as for example class \c std::runtime_error
which contains a simple, dynamically allocated string message.

The exception type introduced here seems in contrast rather heavy! It allows collecting
many new entries and each entry, as being of type \alib{lang;Message} in turn may consist of
a bigger collection of message arguments.

Nevertheless, the probably astonishing facts are that:
- Creating and throwing an exception involves only one heap allocation of one kilobyte.
  In this single allocation, a first message entry will be stored, as well as upcoming ones.
  Only if the first buffer is filled, a next dynamica allocation of one kilobyte is performed.  
- The \c sizeof class \b Exception equals <c>sizeof(void*)</c>.

This all is achieved by inheriting class \b Exception from class \alib{monomem;TSharedMonoVal}.
Please consult that types' reference documentation as well as chapter
\ref alib_contmono_smv of the Programmer's Manual of module \alib_monomem_nl.

Each time an entry is added with method \alib{lang;Exception::Add}, a new message object is
allocated in the monotonic allocator and the values of the given boxes are cloned
(see previous chapter \ref alib_basecamp_message_message_lifecycle).

The only operation that the destructor of class \e Exception performs is to delete the allocated
memory buffer. This is because none of the contained objects needs further destruction calls.
Neither the boxed objects, nor the vector of such, nor the monotonically allocated list of message
entries.

As a result, the rather complex type \b Exception becomes extremely lightweight.
While it has a smaller footprint than standard C++ library exceptions <c>std::runtime_error</c>,
it (usually) also performs only one single memory allocation to store all information collected while
unwinding the stack.


\I{################################################################################################}
### 2.2.4 Exception Entries And Arguments ### {#alib_basecamp_message_exceptions_args}
We learned so far that class \alib{lang;Exception} uses a list of entries
of type \alib{lang;Message}, discussed in the \ref alib_basecamp_message_message "first chapter"
of this manual.

Besides the source information and the message types, there is a third aspect that class
\alib{lang;Exception} leverages from its entry type \alib{lang;Message}: the list of
arbitrary arguments of arbitrary length.

But what are the message arguments used for in exceptions and what arguments should be added
to exception entries? The answer is that this is completely use-case specific.
For example, the provided arguments might help to implement an exception handler that mitigates
the situation by being enabled to analyse in detail what happened.<br>
In parallel, usually a human-readable textual description of the exception is desired.

To have just both - a human-readable textual description and computational values representing the
state that caused the exception - it is recommended to adhere to the following simple scheme
for the arbitrary argument list: 
- The first argument should be a string which comprises a
  format string containing placeholders and following a syntax usable with
  \ref alib_basecamp_format "ALib Formatters" (or custom formatter syntax implementations). 
- The following arguments are "computational" values that comprise valuable information about the
  state of the software when an exception was thrown.

Of course, all exceptions thrown by \alib itself adheres to this scheme. To support the generation
of human-readable exception descriptions, a ready to use formatting feature is given with
overloaded methods \alib{lang;Exception::Format}.
In addition, when using the debug- and release-logging facilities provided with module \alib_alox,
exceptions can be comfortably logged with static method \alib{lox;LogTools::Exception}.
In the latter case, during development, a very helpful and comfortable feature is that
each exception entry collected during unwinding the call stack is "clickable" in an IDE's output
window, so that the source code that created an entry can be displayed and analyzed right away.

Note, that both formatting options (the built in method and \alox) are respecting the agreement of
message entry types having either positive or negative values. As explained above, this agreement is
constituted by method \alib{lang;Exception::Type} that searches the most recent enty with a positive
enum element value. "Informational entries" are marked as such in the output.

Considering the message arguments to be doubly used, once while formatting an exception description
and once while analysing the cause of an exception, a small conflict may arise:
If a catch handler may need some information that should not be used with the description.
In this case, a corresponding "suppression placeholder" may be added to the format string.
For example, using the \alib{lang::format;FormatterPythonStyle;python-style formatter}, placeholder
<c>{!X}</c> would suppress an argument completely. (Note: This is an extension to the original
python formatter syntax.)


\I{################################################################################################}
### 2.2.5 Catching Exceptions, Adding Information, Rethrowing ### {#alib_basecamp_message_exceptions_catch}

It is recommended to catch \alib \b Exceptions by reference:

         catch( alib::Exception& e )
         {
            ...
         }

While the footprint is only a single pointer, the call of the copy constructor would impose an
unnecessary overhead similar to the overhead of copying an instance of C++ standard type
<c>std::shared_ptr</c>.

In the case an exception should be rethrown, method \alib{lang;Exception::Add} may be used to
add one or more new entries to the exception instance.
Method \b %Add has the very same signature as the constructor of the exception.

For the same reasons that exceptions are to be caught by reference, rethrowing is recommended
to be done as reference.

While the following code compiles and works:

       catch( alib::Exception& e )
       {
          throw e.Add( ... );
       }

...this is the more efficient way to do it:

       catch( alib::Exception& e )
       {
          e.Add( ... );
          throw;
       }

\I{################################################################################################}
### 2.2.6 Collecting Exceptions ### {#alib_basecamp_message_exceptions_collect}
Being a \ref alib_contmono_smv "self-containted type" that implements a shared-pointer
behavior, exceptions and their collected entries, can nicely be collected, for example for later
analysis or repeated log output. This is done for example by just copy the exception as a value
into a vector. As soon as the vector is cleared, all dynamic allocations that the exception
instances performed (usually one allocation of 1 kilobyte per exception) are automatically freed.

\I{################################################################################################}
\I{################################################################################################}
### 2.2.6 Resourced Exceptions ### {#alib_basecamp_message_exceptions_res}

To enable the external maintenance of the description strings of exception entries (as explained
in previous chapters, it is proposed and recommended to add a descriptive, human-readable string
as a first argument to each exception entry), this module uses the concept of
\ref alib_enums_records "ALib Enum Records".
This concept in turn allows the definition of records be performed using
\ref alib_basecamp_resources "externalized string resources".

When custom enum types are associated with <em>ALib Enum Records</em> of type \alib{lang;ERException},
elements of this type become accepted by method \alib{lang;Exception::Add} which internally adds
the resourced description string.

Please consult the reference documentation provided with the links in this section for further
information.

 \I{################################################################################################}
## 2.3. Reports ## {#alib_basecamp_message_reports}

What today is called <b>"ALib Reports"</b> is a rather simple concept of raising C++ assertions,
similar warnings, and general messages, currently all of them in debug-compilations only.

The concept is implemented with types:
- \alib{lang;Report}
- \alib{lang;ReportWriter}
- \alib{lang;ReportWriterStdIO}

Furthermore, module \alib_alox introduces plug-in

- \alib{lox;ALoxReportWriter}

The concept is not very advanced and might undergo changes in the future.
Consequently, we do not recommend the use of \b %Reports in custom code outside of \alib and
therefore, apart from the reference documentation of the classes listed above, no Programmer's
Manual exists.



\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}

# 3. Types Defined with Header Files in Directory "alib/lang/resources" {#alib_basecamp_resources}

\I{################################################################################################}
## 3.1. Introduction ## {#alib_basecamp_resources_intro}


\alib uses the term <em>"resources"</em> for string data that technically may be defined in a
constant and static fashion, but that a software volunteers to make configurable.
Typical samples of data that a software exposes for external management are "themes" (
color and font schemes),  or so called <em>"externalized strings"</em>, which mostly may be used to
translate a software into a different "locale" and human language.

While the conceptual definition of \e resources is very similar to the concept of
<em>configuration data</em>, it is good practice to thoroughly decide for each specific
piece of data that is deemed to be made configurable, whether it is \e resource or \e configuration
data. While the mentioned samples of language translation strings and color schemes are
typically \e resources, a server name that a software attaches to is typically
<em>configuration data</em>. End users commonly know and understand the difference between the
two intuitively and appreciate if the corresponding data sources are separated, especially as
this way, <em>resource data</em> does not "clutter" the definitions of <em>configuration</em>
sources.

The best rule of thumb to differentiate \e resource from \e configuration data is to check if
it is possible to distribute a software without turning the data in question into either of the
two externalization concepts, thus "hard coding" the data.
If so, the data more likely is \e resource data. In the sample of the "server address" above, this
is not possible if such server address targets a machine that is under the control of an end user.
However, if it is a machine provided by the distributor of the software which is the same with any
installation, then the concept of resourced data rather fits.

\note
  The concept of <em>configuration data</em> is implemented with module \alib_config.

\I{################################################################################################}
## 3.2. Resources and Modules ## {#alib_basecamp_resources_modules}

As documented in chapter \ref alib_manual_modules "2. ALib Modules" of the \alib Programmer's Manual,
the various \alibmods_nl can be separated into those who are \alibcamps_nl and those which are not.
In this \ref alib_manual_modules_dependency_graph "module dependency graph" it could be noted that
all \alibcamps_nl (dark blue) are dependent on module \alib_basecamp_nl, discussed in this manual.

This relation is explained as follows:
- All high-level \alib modules need externalized string resources
  (e.g., for \ref alib_basecamp_message_exceptions "ALib Exceptions",
  \ref alib_enums_records "ALib Enum Records",  etc.).
- Resources need a controlled process of bootstrapping a software.
- Library class \alib{lang;Camp} holds a pointer to an object of type \alib{lang::resources;ResourcePool}.
- This class furthermore provides strictly defined and phased bootstrapping mechanics, especially
  designed to support the customization of resource management.

Therefore, all \alibcamps_nl not only depend on module \alib_basecamp_nl, but also dispose
about a singleton type derived from class \b %Camp.


\I{################################################################################################}
## 3.3. Data Contract / Conceptual Invariants ## {#alib_basecamp_resources_intro_contract}
\alib resources implemented with this namespace, conceptually impose a set of rules which may be named
a "contract", "invariants" or just "determinations". These rules are given in the following
subchapters.

\I{################################################################################################}
### 3.3.1 Resource Keys ### {#alib_basecamp_resources_intro_contract_keys}
Key values to resources are defined as a two level hierarchy of a \e category and a \e name.
This determination complies with how <em>configuration data</em> is addressed with module
\alib_config. This is also in alignment with common 3rd-party resource management systems
established in the industry.

Independent of the compilation options of \alib in respect to
\ref alib_characters_prepro "choosing the default character width", the string-type of the
\e category and \e name is fixed to \alib{NString}, hence using the narrow \alib{characters;nchar}
type. This is \b not in alignment with <em>configuration data</em> as defined in \alib_config.
The rationale for this is that while configuration data categories and names may be translated
themselves to localized versions (preferably by defining those strings as \e resources!), the category and
name strings of the resources are deemed to be hard-coded in the source code that accesses resources.
As such, they are not shared with end-users, are never localized, and should be using the plain
<em>7-bit printable ASCII character set</em>.

\I{################################################################################################}
### 3.3.2 Restricted To String Type ### {#alib_basecamp_resources_intro_contract_strings}
All resource data is of \alib{strings;TString;String} type (of compilation-dependent
character width).

This restriction imposes that any other data type (for example color codes) has to be
de-serialized (decoded) when resourced.

\I{################################################################################################}
### 3.3.3 Static Data ### {#alib_basecamp_resources_intro_contract_static}
Although resource data technically is non static data, conceptually with this implementation
it is.

This determination has the following impacts:
- Resources do not have to be "acquired" and "released" when accessed, which tremendously simplifies
  the use of resources.
- A using code can rely on the fact that the life-cycle of the string buffers of accessed resources
  is infinite. No local copies of the string data is required. (Note, that due to the missing option of
  acquisition/release of resources, copying a resource string would not even be possible.)
- Resource data does not dynamically change during the life-cycle of a software process.
  In other words, accessing resource data of a specific \e category and \e name, will
  always result in the same returned value.
- Consequently, an implementation of abstract interface type \alib{lang::resources;ResourcePool}
  that attaches to a 3rd-party resource system which supports dynamic resources, usually has to
  create a copy of the data returned from the 3rd-party system and return this copy instead of
  the original value.

\note
  The latter point is implemented with built-in, alternative resource type
  \alib{config;ConfigResourcePool}, which is introduced by module \alib_config. It allows 
  maintaining resources externally, for example in INI-files.

\I{################################################################################################}
### 3.3.4 Thread Safeness ### {#alib_basecamp_resources_intro_contract_threadsafeness}
Accessing resources using abstract interface method \alib{lang::resources;ResourcePool::Get}
is a thread-safe operation.

In contrast to this, an invocation to any of the methods that define resources, namely
\alib{lang::resources;ResourcePool::Bootstrap} and \alib{lang::resources;ResourcePool::BootstrapBulk}
is \b not a thread-safe operation. This is true in respect to each other as well - and most important -
also in respect to parallel resource access with method \alib{lang::resources::ResourcePool;Get}.

This determination has the following impacts:
- Interface methods prefixed with the term <b>Bootstrap</b> of abstract type 
  \alib{lang::resources;ResourcePool} are deemed to exclusively being invoked during 
  \ref alib_manual_bootstrapping "bootstraping" of \alib as well as the of the according software 
  using \alib. 
  Such bootstrapping has to be performed before starting any threads that potentially modify 
  \e resources.
- An implementation of interface \alib{lang::resources;ResourcePool} may need to impose internal
  protection in respect to race conditions of multithreaded access, if that type was 
  used after bootstrap, as \alib itself or custom software still performs read operations after
  bootstrap. 
- With built-in types \alib{lang::resources;LocalResourcePool} and \alib{config;ConfigResourcePool}, 
  no such protection is given.
  Both do not need protection against parallel read operations.
 

\I{################################################################################################}
## 3.4. Interface Class ResourcePool ## {#alib_basecamp_resources_interface}
The central type of the module, class \alib{lang::resources;ResourcePool} was already mentioned several
times. It constitutes a pure abstract interface. Due to the determinations of the concept
given in previous chapter \ref alib_basecamp_resources_intro_contract "3. Data Contract / Conceptual Invariants",
its interface is very simple especially in respect to accessing resources, which is exclusively done
with method \alib{lang::resources;ResourcePool::Get}.

A user of \alib should have no bigger effort to implement this interface and adopt her own or
any 3rd-party "backend" that performs the resource management and the externalization of strings.

Apart from that, two implementations of the interface are provided with \alib. Those are
quickly introduced in the following sections.

\I{################################################################################################}
### 3.4.1 Class LocalResourcePool ### {#alib_basecamp_resources_interface_default}
As explained above, an implementation of interface \b ResourcePool has to be constructed during
bootstrap of \alib and distributed among the modules.

In case the bootstrap process is not \ref alib_manual_bootstrapping_customize "customized", an
instance of class \alib{lang::resources;LocalResourcePool} is created and shared.

This class does not allow any externalization of resources and simply stores the given pointers
to the static data in a \alib{containers;HashTable}, using
\ref alib_mods_contmono "monotonic allocation".

\I{################################################################################################}
### 3.4.2 Class ConfigResourcePool ### {#alib_basecamp_resources_interface_config}
A second built-in implementation of class \b ResourcePool which can be created and shared among
the modules of \alib by \ref alib_manual_bootstrapping_customize "customizing the bootstrap process"
of the library, is given with class \alib{config;ConfigResourcePool}.

The type externalizes resources by inheriting from class \alib{config;Configuration}. With that,
flexible ways of externalizing the data are given.
The class is enabled to fetch all "hard-coded" resources fed by camps with methods
\alib{lang::resources::ResourcePool;Bootstrap} and
\alib{lang::resources::ResourcePool;BootstrapBulk}.

If after creation of an instance of the type, this instance is not changed and just used, then
it behaves in an identical way as type \b LocalResourcePool (with only weaker performance and
increased memory overhead).<br>
The huge advantage of this type is, that any custom externalization is achieved along the very same
lines as done with configuration data. Thus, a user of the library that attached such data, will
have no problem in also load resources from external custom sources.

The type is suitable in any situation where no other ("more professional") 3rd-party "backend"
for resource management is available.<br>
Here are some tips for the usage:
- Using class \alib{config;IniFileFeeder}, it is easy possible to export all "hard-coded" resources
  to an INI-file which are not contained there, yet. Thus an empty INI-file is filled with the
  initial values defined during bootstrap of a software.
- Such INI-file could then be used as a "jump-start" for translating a software from its
  (hard-coded ) default language into a different one.
- With the prioritization mechanics of class \alib{config;Configuration} it is allowed to use
  two or more INI-files, in parallel. This allows for example to have language translations of
  resources in place, which - if only sparsely defined - automatically fallback through the
  different configuration priorities until a value is found, latest the hard-coded one.<br>
  In other words, language translations can be stacked and software might allow to
  use not only specify a preferred language but a prioritized list of preferred languages.
- Similar to the previous point two different configuration plug-ins (e.g., INI-files) could be
  used and have one storing only translated resources, the second all resources relevant for an
  application's color scheme ("Theme"). This way, language and color scheme can be chosen
  independently from each other.

\note
  Despite its flexibility, type \alib{config;ConfigResourcePool} is provided for convenience.
  It was just a "low hanging fruit" of implementation making use of sibling module
  \alib_config_nl.<br>
  There are no plans on the road map of \alib to impose a more sophisticated implementation
  for externalized string resources. It is simply not in the domain of this library to provide
  a higher level of functionality. The whole purpose of this module itself is to have an
  abstract and very simple interface into \b any sort of "resources backend" and its choice
  is completely independent of the choice of using \alib with a software!


\I{################################################################################################}
## 3.5. Indirect Resource Access ## {#alib_basecamp_resources_t_resourced}

\I{################################################################################################}
### 3.5.1 TMP Struct T_Resourced ### {#alib_basecamp_resources_t_resourced_t_resourced}

Sometimes it is required to define resource information, namely
- the \alib{lang::resources;ResourcePool} instance,
- the resource category and
- the resource name

for use by other components. TMP struct \alib{lang::resources;T_Resourced} may be specialized to do such
definition for C++ types.
A specialization of the struct can be easily implemented using macro \ref ALIB_RESOURCED.

A sample for the use of this struct is given with module \alib_config: To load and store configuration
data, this module exposes a type of \ref alib_enums_records "ALib Enum Records" and accepts custom enumeration
types in various interface methods, if they just have this specific record type associated.

Now, if an element of a custom enumeration type that disposes about a specialization of \b T_Resourced
is passed to such interface method, internally this information is used to load further
details of the variable from the resource pool.


\I{################################################################################################}
### 3.5.2 Helper-Struct Resourced ### {#alib_basecamp_resources_t_resourced_resourced}

As soon as struct \alib{lang::resources;T_Resourced} is specialized for a type, helper-struct
static struct \alib{lang::resources;ResourcedType} becomes available.

The type has two overloaded methods \alib{lang::resources::ResourcedType;Get}: The first is parameterless
and simply receives the resource string associated to a type with the specialization of
\b T_Resourced. The second takes a replacement for the resource name. This may be used to
retrieve resource strings which are likewise associated to the type.

Furthermore, the struct provides methods \alib{lang::resources::ResourcedType;TypeNamePrefix} and
\alib{lang::resources::ResourcedType;TypeNamePostfix} which are meant to provide a standardized way
to define a type's name using resources. The methods are for example used with
specializations \alib{strings::APPENDABLES;T_Append<TEnum,TChar,TAllocator>;T_Append<TEnum,TChar,TAllocator>}
and \alib{strings::APPENDABLES;T_Append<TEnumBitwise,TChar,TAllocator>}
which write enum element names into instances of type \alib{strings;TAString;AString}.

\I{################################################################################################}
### 3.5.3 Helper-Struct ResourceInfo ### {#alib_basecamp_resources_t_resourced_resourceinfo}

A next helper-struct is given with \alib{lang::resources;ResourceInfo} which first of all is a simple
struct that stores  resourcing information (the resource pool and category and name strings)
for later use.

While this struct is usable without a specialization of \alib{lang::resources;T_Resourced}, in most
use cases it is, because it allows converting the compile-time information given by
\b T_Resourced into run-time information.

\I{################################################################################################}
## 3.6. Further Details ## {#alib_basecamp_resources_details}

\I{################################################################################################}
### 3.6.1 Resourced Data Records And Tables ### {#alib_basecamp_resources_details_data}

Besides just externalizing strings, many use cases require to access externalized data sets
or even whole tables of this.

\alib module \alib_enums provides a solution for this with its
concept \ref alib_enums_records "ALib Enum Records". There a table of data is addressed
using the C++ type information of enum types. Single records of a table may (or may not) be
addressed by elements of the corresponding enumeration.
The module provides convenient facilities the fields of the records and whole tables from
resourced strings.

Before you go ahead and implement your "own" solution for externalized static data, it might be
worthwhile to check out if \ref alib_enums_records "ALib Enum Records" meet your requirements.

\I{################################################################################################}
### 3.6.2 Exporting Resources For Externalization ### {#alib_basecamp_resources_details_export}
When resources are externalized, for example for translation to different human languages,
the list of resources have to be imported to the external "backend". To do so all resources
have to be queried from the library.

Here is a sample program that performs this task:

 \snippet "../tools/ResourceExport/resourceexport.cpp"     DOX_RESOURCES_EXPORT

The output of this sample can directly be loaded by class \alib{config;IniFile}, hence with a plug-in
attached to an instance of built-in resource pool implementation
\alib{config;ConfigResourcePool}. The sample might be adopted accordingly to write a
format suitable to be imported to the backend of choice.

With every update of the library, changes of the resource strings have to be determined.
This might be done for example with a custom unit test that compares the default entries
with the ones currently stored in an external backend. New resources might be added, others might
disappear, and worst: existing ones might change their content format. In the latter case, an
externalized resource might be errorneous and lead to undefined behavior.

Starting with library version \b 1903, to support the detection of changes, the version history is found in

        ALIB_BASE_DIR/docs/pages/resource-exports/

\releasetask{Run export tool, create file, add link to new pages below, add new page at the end of this file}
The following exports are available:
- \ref alib_basecamp_resources_exported_resources_2412
- \ref alib_basecamp_resources_exported_resources_2402
- \ref alib_basecamp_resources_exported_resources_2312
- \ref alib_basecamp_resources_exported_resources_1912
- \ref alib_basecamp_resources_exported_resources_1903

\I{################################################################################################}
### 3.6.3 Debug- And Usage Statistics  ### {#alib_basecamp_resources_details_debug}

With the provision of compiler symbol \ref ALIB_DEBUG_RESOURCES, static field
\alib{lang::resources;LocalResourcePool::DbgResourceLoadObserver} becomes available. If set to <c>&std::cout</c>
before bootstrapping \alib, the resource load process can be observed on the console, because methods
\alib{lang::resources;LocalResourcePool::BootstrapBulk} and
\alib{lang::resources;LocalResourcePool::BootstrapAddOrReplace} will write information on
bulk and singular resource data definitions.
This tremendously helps to find errors in resource strings, which often are simply missing commas
and similar.

Next, virtual method  \alib{lang::resources;ResourcePool::DbgGetCategories} and
\alib{lang::resources;ResourcePool::DbgGetList} become available. The latter returns a
list of all resources defined, including a usage counter. The method is only implemented by class
\alib{lang::resources;LocalResourcePool}. Alternative type \alib{config;ConfigResourcePool} does not
implement it.

Furthermore, convenience method \alib{lang::resources;ResourcePool::DbgDump} becomes available which
simply writes the list of symbols into an \b %AString, sorted by category and in an alphabetical
order.

The usage counter included in the list may be used to identify two groups of
resource strings:
- Resource strings that never have been loaded, and
- Resource strings that have been loaded very often during the actual execution of a software.

While entries of the first type may have become obsolete and are candidates for removal,
those of the second type, a software might consider to "cache" the symbol in a variable instead
of repeatedly retrieving it from the resource pool.

Remember: While trivial implementation class \b LocalResourcePool is very fast and resource access
is not really noticeable, other implementations might not be.

The following code snippets taken from the \alib unit tests, demonstrate how to quickly leverage
these debug features, depending on compiler symbol \ref ALIB_DEBUG_RESOURCES. The snippets
might be copied to own projects and remain there forever.

Bootstrapping may look as follows:
 \snippet "gtest_main.cpp"     DOX_RESOURCES_DEBUG_BOOTSTRAP1
 \snippet "gtest_main.cpp"     DOX_RESOURCES_DEBUG_BOOTSTRAP2

Before termination of a software (hence, before invoking \alib{Shutdown}), the following code may
be placed:
 \snippet "gtest_main.cpp"     DOX_RESOURCES_DEBUG_SHUTDOWN

\I{################################################################################################}
## 3.7. Sample  ## {#alib_basecamp_resources_samplehint}

A comprehensive sample of using \alib resources placed in a custom module is provided with the
\ref alib_cli_tut "tutorial of ALib Module 'CLI'".
The sample code provided there, can be easily used as a jump start into an own project that
creates a custom \alib module and leverages the resource features provided.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}

# 4. Types Defined with Header Files in Directory "alib/lang/format" {#alib_basecamp_format}

\I{################################################################################################}
## 4.1. Introduction ## {#alib_basecamp_format_intro}
This \alibmod provides string formatting facilities by implementing an approach that is common
to many programming languages and libraries. This approach offers an interface that includes the
use of a <em>"format string"</em> containing placeholders. Besides this format string, a list of
data values can be given, used to fill the placeholders.<br>

Probably one of best known samples of such an interface is the \c printf method of the
C Language. A variation of this interface is found in almost any high-level, general purpose
programing language.

Of course, this module leverages module \alib_strings for all general string functions needed.
Similar important is the use of module \alib_boxing, which brings type-safe variadic argument
lists and allows with its feature of having "virtual functions" on boxed arguments, to have
custom formatting syntax for placeholders of custom argument type.

While it is possible to implement a formatter providing a custom placeholder syntax, two very
prominent ones are built-in with formatters:
- \alib{lang::format;FormatterJavaStyle;FormatterJavaStyle}<br>
  Implements the syntax provided with the formatter included with the core class libraries of the
  JAVA programming language. This syntax is an extension of the good old \c printf format string
  style.

- \alib{lang::format;FormatterPythonStyle;FormatterPythonStyle}<br>
  Implements the syntax provided with the formatter included with the core class libraries of the
  Python programming language. This syntax is very powerful and flexible in respect to the provision
  of syntax extensions for custom types.<br>
  Over time, this formatting syntax became the preferred syntax within \alib itself and we have
  extended the syntax even in some respects in comparison to the original definition.

Another good news is that in its very basics, <b>Python Style</b> is similar to <b>.Net</b> formatting.
This way, there is some "familiar basic syntax" available for everybody that has used formatting
in one of the languages <b>C, C++, C#, Java, or Python</b> and in languages that have also mimicked
one of these styles!<br>


\I{################################################################################################}
## 4.2. Using The Formatters ## {#alib_basecamp_format_using}

By leveraging module \alib_boxing, which implies the use of variadic template arguments,
the invocation of the final format method is as simple as it is possible. The following
samples a simple format action with each of the two built-in formatters:

\snippet "DOX_FORMAT.cpp"     DOX_SF_USING_1

This produces the following result:

\snippet "DOX_SF_USING_1.txt"     OUTPUT

Values of or pointers to any type that is "boxable" may be passed as an argument to method
\alib{lang::format;Formatter::Format}. The specific implementation of the formatter will
match the "placeholder type" with the given argument type and format the argument according to
the placeholder attributes.

\I{################################################################################################}
### 4.2.1 Concatenated Formatters ### {#alib_basecamp_format_using_concat}

In the sample above, two different formatters are created and each is used "properly", namely with
its according syntax.<br>
To increase flexibility, the formatters of this \alibmod_nl provide two features:
- Formatters can be concatenated
- Formatters detect format strings and on failure, pass processing to concatenated formatter.

With that information, the following code can be written:

\snippet "DOX_FORMAT.cpp"     DOX_SF_USING_2

\note
  While the first formatter is a simple local object (stack allocated), the second formatter is
  created on the heap (keyword \c new) then stored in field \alib{lang::format::Formatter;Next}
  of the first formatter. This field is of type \alib{SPFormatter}, which is an alias for
  <c>std::shared_ptr<Formatter></c>, hence a C++ standard "smart pointer" that deletes it's
  contained object automatically with the deletion of the last referrer.
  Only in later chapters it will be explained why it is the preferred method to manage \alib formatter
  instances in shared pointers.


The short sample code correctly produces the following output:

\snippet "DOX_SF_USING_2.txt"     OUTPUT

However, the placeholder syntax must not be mixed within one format string. Let's try:

\snippet "DOX_FORMAT.cpp"     DOX_SF_USING_3

The output is:

\verbinclude "DOX_SF_USING_3.txt"     

This is obviously not what we wanted, but then it also did not produce an exception and it even
included the second argument, <b>"Python"</b> in the output. While exceptions are discussed in a
later chapter only, the reason that no exception is thrown here is simply explained: The first
formatter in the chain, which we defined as type
\alib{lang::format;FormatterJavaStyle;FormatterJavaStyle}, identified the format string by
reading <b>"%s"</b>. It then "consumes" this string along with as many subsequent arguments as
placeholders are found in the format string. This number is just one, as the placeholder <b>"{}"</b> is
not recognized by this formatter.

The intermediate result consequently is <b>"---Java---{}---"</b>, while the argument <b>"Python"</b>
remains unprocessed. The next section explains what happens with this remaining argument.

\I{################################################################################################}
### 4.2.2 Concatenating Format Operations ### {#alib_basecamp_format_using_concatops}

We just continue with the sample of the previous section: The unprocessed argument <b>"Python"</b>
is not dropped, as it would have been with most implementations of a similar format functions
in other libraries and programming languages. Instead, with \alib_basecamp_nl, the
formatting process starts all over again using the remaining argument as the format string.

Now, as it is not a format string (it does not contain placeholders in any known syntax) it is
just appended to the target string "as is".

\note
  For users who are familiar with modules \alib_boxing and \alib_strings: The words
  "appending as is", here means, that the remaining argument is appended to the target string in a
  \ref alib_strings_assembly_ttostring "type-specific way".
  Because all arguments are of the same type, namely \alib{boxing;Box}, this in turn means that
  box-function \alib{boxing;FAppend} is invoked on the box, which just performs
  the \ref alib_strings_assembly_ttostring "type-dependent string conversion".

In fact, for this last operation, none of the two formatters became active.
The trick here is that the abstract base class, \alib{lang::format;Formatter} already implements
method \b Format. This implementation loops over all arguments. It checks if the current first
argument is a string recognized as a format string by any of the chained formatters. If it is not,
this argument is just \ref alib_strings_assembly_ttostring "appended" to the target string and the
loop continues with the next argument in the list.<br>
If a format string is identified, control is passed to the corresponding formatter that consumes as
many further arguments as placeholders are found in that format string, and then passes control back
to the main loop.

Consequently, this approach allows invoking \b %Format without even a format string:

\snippet "DOX_FORMAT.cpp"     DOX_SF_USING_4
\verbinclude "DOX_SF_USING_4.txt"

which probably does not make any sense, because the same result could have been achieved
much more efficiently by stating:

\snippet "DOX_FORMAT.cpp"     DOX_SF_USING_4b


Even the following sample, still might not make too much sense to a reader:

\snippet "DOX_FORMAT.cpp"     DOX_SF_USING_5

because the usual way to come to the same result, was to have only one format string with two
arguments, instead of two format strings with one argument each:

\snippet "DOX_FORMAT.cpp"     DOX_SF_USING_5b

So, why is this loop implemented with its auto-detection and the option of having more than one
format string? Some sound rationale for the loop is given in the next section.


\I{################################################################################################}
### 4.2.3 Decoupled Format Argument Collection ### {#alib_basecamp_format_using_argcoll}

Method \b %Format collects the variadic template arguments to an internally allocated container
of type \alib{boxing;TBoxes}. This container is then passed to the internal format loop.

Alternatively, user code may perform the collection of format arguments in a container object 
"manually". In this case, the formatter is invoked with one of the two overloaded methods
\alib{lang::format;Formatter::FormatArgs}. 
For this, either an external container can be used, or the internal container may be received with 
the method \alib{lang::format;Formatter::GetArgContainer}. The latter case saves allocation effort
as the returned vector is never freed but reused instead. However, a user must be aware of 
racing conditions in multithreaded software, especially when working with the global instance
\alib{lang::format;Formatter::Default}.<br> 
External containers might of-course also be of derived type, for example type \alib{lang;Message}
may be passed.

With this knowledge it becomes obvious that the collection of formatting arguments can be
"decoupled" from the invocation of the formatter. Note that the argument list may include zero, 
one, or even multiple format strings, which each are followed by corresponding placeholder values:

\snippet "DOX_FORMAT.cpp"     DOX_SF_USING_6
\snippet "DOX_SF_USING_6.txt"     OUTPUT

A reader might think for herself if and when this might become useful. It should be noted that
the interface of logging module \alib_alox builds on the same mechanism.
The arguments there are called "logables" and might be format strings or anything else.
Therefore, also with \b %ALox the collection of log entry data can be decoupled from the final creation of
the log entry. This is especially useful for complex log-entries whose arguments are collected during
the execution of an algorithm and for example are only logged in case of an exception or other unexpected
conditions.


\I{################################################################################################}
### 4.2.4 Default Formatters ### {#alib_basecamp_format_using_default}

In the previous samples, a local instance of a formatter (or two) has been created.
For general purpose use, this module provides a global pair of (concatenated) formatters
which are accessible static member \alib{lang::format;Formatter::Default}.

The formatter is embedded in "automatic pointer type" \alib{SPFormatter}, which builds on 
\alib{containers;SharedPtr}.
During \ref alib_manual_bootstrapping "bootstrapping" of the library, a formatter of type
\alib{lang::format;FormatterPythonStyle;FormatterPythonStyle} is created with a concatenated
object of \alib{lang::format;FormatterJavaStyle;FormatterJavaStyle}.

One obvious rationale for the provision of these default formatters is to save resources by 
reusing the formatter instances in different parts of an application.
However, probably in most cases more important is the fact that this way, the same default
configuration is used with formatting operations. For example, if the decimal point character
of floating point numbers should be defaulted to be different from US/English standard \c '.', then
such a setting could be performed with the bootstrap of the library once and for all usages across
a process.

With multithreaded software, the default-formatter is to be locked using mutex
\alib{lang::format;Formatter::DefaultLock}. 
This is asserted in debug-compilations, as described in detail in chapter 
\ref alib_threads_intro_assert_locks of the Programmer's Manual of module \alib_threads.   

\I{################################################################################################}
### 4.2.5 Cloning Formatters ### {#alib_basecamp_format_using_clone}
Formatter implementations may or may not provide default settings that, for example, influence
a format operation that uses minimal placeholders that omit optional formatting flags.
The built-in formatters do have such default settings.

If a software unit wishes to change some settings, the advised approach is as follows:
- Retrieve the default formatter(s)
- Create a clone of the default formatter(s) by invoking \alib{lang::format;Formatter::Clone}.
- Change the default settings of the cloned formatter.
- Use the cloned formatter.

With this procedure, any changes that an application applied to the default formatters (e.g., during
bootstrap) will remain valid in the cloned formatters in addition to the "local" changes, while the
default formatters remain untouched.

For example, built-in formatters provide in fields
\alib{lang::format::FormatterStdImpl;DefaultNumberFormat} and
\alib{lang::format::FormatterStdImpl;AlternativeNumberFormat} to reflect some default behavior of
their formatting syntax.<br>
The attributes of these members might be modified to change those defaults.
While this leads to a deviation of the formatting standard, it may be used instead of providing
corresponding syntactic information within the placeholder field of every format string.
Some modifications may not even be possible with the given format specification syntax.

\I{################################################################################################}
### 4.2.6 Exceptions ### {#alib_basecamp_format_using_exceptions}
The simple samples shown so far used correct format strings. In case of errorneous format strings,
the built-in formatters will throw an \alib \alib{lang;Exception} defined with enumeration
\ref alib::lang::format::FMTExceptions.

While in the case of "hard-coded" format strings, such exceptions are not needed to be caught,
their evaluation (with debug-builds) might be very helpful for identifying what is wrong.
Of course, when format strings are not hard-coded but instead can be provided by the users
of a software (for example in configuration files or command line parameters), a <c>try/catch</c>
block around formatting invocations is a mandatory thing, also in release compilations.

The following sample shows how an exception can be caught and its description may be written
to the standard output:

\snippet "DOX_FORMAT.cpp"     DOX_SF_USING_EXC_1

The output of running this code is:

\snippet "DOX_SF_USING_EXC_1.txt"     OUTPUT

In most cases, a detailed text message, including a copy of the format string
and a "caret" symbol <c>'^'</c> that hints to the parsing error in the string is given with the
exception's description.

\I{################################################################################################}
### 4.2.7 Escape Sequences In Format Strings ### {#alib_basecamp_format_using_escapes}

Escape characters, like for example <c>"\t"</c>, <c>"\n"</c> or <c>\"\\\\\"</c> might be given with
either one or two backslashes.
The formatters will convert them to the corresponding ASCII code, if the backslash itself is escaped.

Class \alib{lang::format;FormatterPythonStyle;FormatterPythonStyle} recognizes
double curly braces <c>"{{"</c> and <c>"}}"</c> and converts them to a single brace.
Similar to this, class \alib{lang::format;FormatterJavaStyle;FormatterJavaStyle} recognizes
<c>"%%"</c> and converts it to a single percentage symbol.

\I{################################################################################################}
## 4.3. Formatting Custom Types ## {#alib_basecamp_format_custom_types}

As we have seen, the use of module \alib_boxing allows the formatters of this module to accept
any third-party data type as formatting arguments. The formatters of course are enabled to "convert"
all C++ fundamental types to strings. But how about custom types?

The solution for custom conversion is given with the support of
\ref alib_boxing_functions "\"box-functions\"", which implement a sort of "virtual function call"
on boxed types.<br>
There are two box-functions that the built-in formatters are using.

\I{################################################################################################}
### 4.3.1 Box-Function FAppend ### {#alib_basecamp_format_custom_types_iappend}

By default, the very simple box-function that is used by the built-in formatters for converting
arbitrary types to string values, is \alib{boxing;FAppend}.
This function is one of the built-in functions of module \alib_boxing and this way is not
specific to this module \alib_basecamp_nl.

Usually this function's implementation just unboxes the corresponding type and
\alib{strings;T_Append;appends} the object to the target string.<br>
Let as look at an example. The following struct stores a temperature in Kelvin:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_APPEND_0

If an object of this class is used with a formatter without any further preparation, the
default implementation of function \b %FAppend is invoked, which writes the memory address of the
given object. In debug-compilations, this implementation in addition writes the boxed type's name
(platform-dependent and implemented with class \alib{lang;DbgTypeDemangler}). This is shown in the
following code and output snippet:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_APPEND_1

\verbinclude "DOX_SF_CUSTOM_APPEND_1.txt"

The first step to implement function \b %FAppend for sample type \b %Kelvin is to specialize
functor \alib{strings;T_Append} for the type:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_APPEND_2

With that in place, it is possible to apply an object of this type to an AString:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_APPEND_3

\verbinclude "DOX_SF_CUSTOM_APPEND_3.txt"

Now, we can easily implement box-function \b %FAppend, because for types that are "appendable" already,
this is done with just a simple macro that has to be placed in the
\ref alib_manual_bootstrapping "bootstrap section" of a software:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_APPEND_4

With that in place, it is possible to append a \b boxed object of this type to an AString:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_APPEND_5

\verbinclude "DOX_SF_CUSTOM_APPEND_5.txt"

Because the formatters use the same technique with the boxed arguments they receive, our sample
class can now already be used with formatters:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_APPEND_6

\verbinclude "DOX_SF_CUSTOM_APPEND_6.txt"

To summarize this section, some bullet points should be given:
- Independently from this module \alib_basecamp_nl and the formatters defined here,
  class \alib{strings;TAString;AString} provides a concept based on template meta programming that
  allows \ref alib_strings_assembly_ttostring "appending objects of arbitrary type to strings".
- With the availability of module \alib_boxing, a box-function named \b FAppend is established
  that is invoked at the moment an instance of class \alib{boxing;Box} is <em>appended</em>
  to an \b %AString.
- By defining a specialized version of this function for a custom type, boxed values of the
  custom type can be appended to an \b %AString.
- The formatter classes provided with this module, use this function with custom types.
- Consequently, if a custom type has already been made compatible with both modules,
  \alib_strings_nl and \alib_boxing_nl, no special preparations have to be made to use
  the type with the formatter classes.

\I{################################################################################################}
### 4.3.2 Box-Function FFormat ### {#alib_basecamp_format_custom_types_fformat}

The previous section demonstrated how a custom type can be made "compatible" to \alib formatters
found in this module \alib_basecamp_nl.

The given approach using box-function \b %FAppend is quite limited in that respect, that within a
format string no attributes might be given that determine how to format a custom type.
With the sampled temperature type \b "Kelvin", the output format was in celsius with one decimal
digits. If we wanted to allow Fahrenheit as an alternative output, we need to implement boxing
function \alib{lang::format;FFormat}, which was specifically created for this purpose and
consequently is defined in this module.

\note
  Type-specific format specification strings are allowed only with the Python-like syntax of format
  strings. The Java-like formatter does not provide a feature of "embedding" custom format
  specifications in the format string.

The function has three parameters: besides the box that it is invoked on and
the target string to write to, it receives a string that provides type-specific information
about how the contents are to be written. This format specification is fully type- and 
implementation-specific and has to be documented with the specific function's documentation.

We want to implement a format string that starts with character <c>'C'</c>, <c>'F'</c>  or <c>'K'</c> to
specify celsius, fahrenheit or kelvin and a following integral number that specifies the fractional
digits of the output.

To do this, the following function declaration needs to go to a header file:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_IFORMAT_1

Then, the implementation of the function has to be placed in a compilation unit. This might
look like this:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_IFORMAT_2

Within the bootstrap section of the process, the function has to be registered with \alib_boxing:

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_IFORMAT_3

With that in place, we can use the custom format specification with our custom type

\snippet "DOX_FORMAT.cpp"     DOX_SF_CUSTOM_IFORMAT_4

The following output is produced.

\snippet "DOX_SF_CUSTOM_IFORMAT_4.txt"     OUTPUT


As a second sample we want to look at the internal implementation of formatting date and time values.
\alib class \alib{lang::system;CalendarDateTime} provides (native) method
\alib{lang::system::CalendarDateTime;Format} to write time and date values in a human-readable and customizable
way. This method also requires a format specification.
Now, this helper-class is used to implement \b %FFormat for boxed arguments of type
\alib{time;DateTime}, which is given with class \alib{lang::system;FFormat_DateTime}.
Due to the existence of the helper-class, the implementation of the function is therefore rather
simple:

\snippet "alib/lang/basecamp/basecamp.cpp"     DOX_BOXING_IFORMAT_DATETIME


\I{################################################################################################}
## 4.4. Custom Formatters ## {#alib_basecamp_format_custom_formatters}

To implement a custom formatter that uses a custom format string syntax, no detailed
manual or step-by-step sample is given here. Instead, just some hints as bullet points should
be enough:

- The typical use-case for implementing a custom format string is to mimic an existing formatter
  of a different programing language or different C++ library, to be able to reuse the formatting
  strings, which might be resourced and shared between different implementations of software.
- The built-in formatters both use "intermediate" class \alib{lang::format;FormatterStdImpl}
  as a base class. This class might be used for custom formatters as well, as it already implements
  a skeleton that has to be completed by implementing a set of specific abstract methods.
- It is recommended to review (and copy) the sources of one of the given formatter implementations.
  While \alib{lang::format;FormatterPythonStyle;FormatterPythonStyle} is by far the more
  powerful implementation, class \alib{lang::format;FormatterJavaStyle;FormatterJavaStyle}
  might be less complicated to start with.
- A thorough understanding of modules \alib_boxing and \alib_strings is a precondition
  for the implementation of a custom formatter.


\I{################################################################################################}
## 4.5. Further Types Provided By This Module ## {#alib_basecamp_format_othertypes}

Besides the formatter classes that have been discussed in this Programmer's Manual, module
\alib_basecamp_nl provides some other types which make use of the formatters.

As of today, these types are
- \alib{lang::format;Paragraphs},
- \alib{lang::format;PropertyFormatter} and
- \alib{lang::format;PropertyFormatters}.

Please consult the class's extensive reference documentation for more information about the
features and use of these types.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}

# 5. Types Defined with Header Files in Directory "alib/lang/system" {#alib_basecamp_system}

This inner namespace of \alib_basecamp_nl provides types that interface into the operating system
of the host computer. The types found here are just a very few and this namespace is
<b>not meant as being anything of huge value for third party applications</b>.
Rather, the types have been implemented when other parts of \alib needed corresponding
functionality.

That being said, we refer the reader to the \ref alib::lang::system "reference documentation".

<br><br><br><br><br><br> 
\I{################################################################################################}
\I{ Resource export pages   }
\I{################################################################################################}

\page alib_basecamp_resources_exported_resources_1903           Export of Resourced Strings - V. 1903
\verbinclude "pages/resource-exports/resources-1903.txt"

\page alib_basecamp_resources_exported_resources_1912           Export of Resourced Strings - V. 1912
\verbinclude "pages/resource-exports/resources-1912.txt"

\page alib_basecamp_resources_exported_resources_2312           Export of Resourced Strings - V. 2312
\verbinclude "pages/resource-exports/resources-2312.txt"

\page alib_basecamp_resources_exported_resources_2402           Export of Resourced Strings - V. 2402
\verbinclude "pages/resource-exports/resources-2402.txt"

\page alib_basecamp_resources_exported_resources_2412           Export of Resourced Strings - V. 2412
\verbinclude "pages/resource-exports/resources-2412.txt"
*/
