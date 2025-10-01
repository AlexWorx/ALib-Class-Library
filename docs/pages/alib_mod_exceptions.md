// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_exceptions  ALib Module Exceptions - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1. Introduction #  {#alib_exceptions_intro}

This rather small \alibmod introduces two types, class \alib{exceptions;Message} which builds
the foundation for the second and more important type \alib{exceptions;Exception}. 

# 2. Class Message # {#alib_exceptions_message}

When measured in source code, the size of class \alib{exceptions;Message} is very short.
Nevertheless, as the type leverages almost all features of module \alib_boxing, it is a quite
powerful tool to have.

The type acts as an information object of the central \b throwable type \alib{exceptions;Exception},
discussed in the next chapter.

\I{################################################################################################}
## 2.1 Source Location ## {#alib_exceptions_message_location}
Objects of type \alib{exceptions;Message} relate to a source code location, which is reflected by field
\alib{exceptions::Message;CI}.

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
## 2.2 Message Types ## {#alib_exceptions_message_types}
To define the type of a message, class \alib{boxing;Enum} provided by module \alib_boxing
is used, by letting field \alib{exceptions;Message::Type} be of type \b %Enum.

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
## 2.3 Message Arguments ## {#alib_exceptions_message_args}
The contents of the message is comprised by a list of boxed objects. For this, class \b Message
inherits class \alib{BoxesMA}, which itself is a container (vector) of objects of type
\alib{boxing;Box}. The interpretation of the data given in a message is defined by an application.
As described in a later section of this manual, \alib class \b Exception, which includes a whole
list of \b Message objects, proposes a certain schematic for the attached data.
But also in this case, the proposal is just a recommendation, not a mandatory requirement.

\I{################################################################################################}
## 2.4 Memory Allocation ## {#alib_exceptions_message_alloc}
As documented with class \alib{boxing;TBoxes}, its underlying <c>std::vector</c> may use
either standard dynamic memory or "monotonic allocation" introduced by module \alib_monomem.
With class \alib{exceptions;Message}, monotonic allocation is chosen. A reference to the allocator has to
be passed with construction.
at that moment and only be initialized before adding information to the message. This
supports to derive from class \b Message which hold the allocator instance of a member.

\I{################################################################################################}
## 2.5 Argument Life-Cycle ## {#alib_exceptions_message_lifecycle}
Restricted life-cycles of a message's arguments is a potential pitfall.
Because class \alib{exceptions;Message} is a \ref alib::BoxesMA "vector of boxes" (by inheritance),
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
# 3. Exceptions # {#alib_exceptions_exceptions}

\I{################################################################################################}
## 3.1 Exception Handling Paradigms ## {#alib_exceptions_exception_paradigm}

Class \alib{exceptions;Exception} is used within \alib as <b>the one and only type</b> thrown with
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


\alib adheres to a different pattern and class \alib{exceptions;Exception} supports, respectively
implements this.
An exception once thrown is never replaced. Instead, while unwinding the stack new "entries"
are added to the exception. The entries are of type \b %Message, introduced in the first chapter
of this manual.

Now, a new entry can have two effects:
- Either the new message is changing the meaning of an exception, or
- the new message extends the existing meaning of an exception by adding detail information
  to the exception.

While this sounds complicated, it is not: the whole class is not much more than a list of objects
of type \alib{exceptions;Message} and the exception information passed with the constructor of the class,
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
## 3.2 Exception Types ## {#alib_exceptions_exception_types}

With the paradigm/design pattern implemented (as discussed in the previous introductory section),
usually an important problem is raised: What can be used as an "error type" and how can it be
ensured that error types defined in independent code units of software are not ambiguous?
Remember that the more common approach to implement exceptions, which is also taken in the
C++ standard library that defines class <c>std::exception</c> to be a general base class for
exceptions, such ambiguities do not occur: There, the C++ type system is used to distinguish
exceptions.
With the \alib implementation however, the entries need to use a "type-code" and these codes must
not overlap.

This problem is solved by using type \alib{exceptions;Message} as entries of class
\alib{exceptions;Exception}. As discussed in previous chapter \ref alib_exceptions_message_types,
this way a two-level hierarchy of exception types is established, where the first level uses
run-time type information and hence no clash of exception types may occur, even between different
code units.

There is one question left: Which of the messages in the collected list now does define the resulting
"exception type"? Is it the first message added? Or the last one?
In the paradigm \alib uses it might be any of them, or even one in-between.

The exception type is defined as:
<b>"The type of an exception is defined by the most recently added message, who's enum element's
integral value is positive."</b>

This \b Enum is searched and returned with method \alib{exceptions::Exception;Type}.
With this definition, enumeration element values of exception message entries that are considered
"additional information" are to be declared with a negative integral initializer, while
those that constitute an own exception type, are to be declared with a positive value.

Consequently, if a catch handler uses method \b Type, it does not need to
switch over just all possible entry types (aka enumeration elements), but just over those that are
positive and that therefore bring a different meaning to an exception.



\I{################################################################################################}
## 3.3 Self-Contained Allocation ## {#alib_exceptions_exception_smv}

When an exception is thrown, something "bad" happened. In that situation software should not
perform too many complex things, for example dynamic memory allocations.

Simple exception types are therefore very lightweight, as for example class \c std::runtime_error
which contains a simple, dynamically allocated string message.

The exception type introduced here seems in contrast rather heavy! It allows collecting
many new entries and each entry, as being of type \alib{exceptions;Message} in turn may consist of
a bigger collection of message arguments.

Nevertheless, the probably astonishing facts are that:
- Creating and throwing an exception involves only one heap allocation of one kilobyte.
  In this single allocation, a first message entry will be stored, as well as upcoming ones.
  Only if the first buffer is filled, a next dynamica allocation of one kilobyte is performed.  
- The \c sizeof class \b Exception equals <c>sizeof(void*)</c>.

This all is achieved by inheriting class \b Exception from class \alib{monomem;TSharedMonoVal}.
Please consult that types' reference documentation as well as chapter
\ref alib_contmono_smv of the Programmer's Manual of module \alib_monomem_nl.

Each time an entry is added with method \alib{exceptions;Exception::Add}, a new message object is
allocated in the monotonic allocator and the values of the given boxes are cloned
(see previous chapter \ref alib_exceptions_message_lifecycle).

The only operation that the destructor of class \e Exception performs is to delete the allocated
memory buffer. This is because none of the contained objects needs further destruction calls.
Neither the boxed objects, nor the vector of such, nor the monotonically allocated list of message
entries.

As a result, the rather complex type \b Exception becomes extremely lightweight.
While it has a smaller footprint than standard C++ library exceptions <c>std::runtime_error</c>,
it (usually) also performs only one single memory allocation to store all information collected while
unwinding the stack.


\I{################################################################################################}
## 3.4 Exception Entries And Arguments ## {#alib_exceptions_exceptions_args}
We learned so far that class \alib{exceptions;Exception} uses a list of entries
of type \alib{exceptions;Message}, discussed in the \ref alib_exceptions_message "first chapter"
of this manual.

Besides the source information and the message types, there is a third aspect that class
\alib{exceptions;Exception} leverages from its entry type \alib{exceptions;Message}: the list of
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
  \ref alib_mod_format "ALib Formatters" (or custom formatter syntax implementations). 
- The following arguments are "computational" values that comprise valuable information about the
  state of the software when an exception was thrown.

Of course, all exceptions thrown by \alib itself adheres to this scheme. To support the generation
of human-readable exception descriptions, a ready to use formatting feature is given with
overloaded methods \alib{exceptions;Exception::Format}.
In addition, when using the debug- and release-logging facilities provided with module \alib_alox,
exceptions can be comfortably logged with static method \alib{lox;LogTools::Exception}.
In the latter case, during development, a very helpful and comfortable feature is that
each exception entry collected during unwinding the call stack is "clickable" in an IDE's output
window, so that the source code that created an entry can be displayed and analyzed right away.

Note, that both formatting options (the built in method and \alox) are respecting the agreement of
message entry types having either positive or negative values. As explained above, this agreement is
constituted by method \alib{exceptions;Exception::Type} that searches the most recent enty with a positive
enum element value. "Informational entries" are marked as such in the output.

Considering the message arguments to be doubly used, once while formatting an exception description
and once while analysing the cause of an exception, a small conflict may arise:
If a catch handler may need some information that should not be used with the description.
In this case, a corresponding "suppression placeholder" may be added to the format string.
For example, using the \alib{format;FormatterPythonStyle;python-style formatter}, placeholder
<c>{!X}</c> would suppress an argument completely. (Note: This is an extension to the original
python formatter syntax.)


\I{################################################################################################}
## 3.5 Catching Exceptions, Adding Information, Rethrowing ## {#alib_exceptions_exceptions_catch}

It is recommended to catch \alib \b Exceptions by reference:

         catch( alib::Exception& e ) {
            ...
         }

While the footprint is only a single pointer, the call of the copy constructor would impose an
unnecessary overhead similar to the overhead of copying an instance of C++ standard type
<c>std::shared_ptr</c>.

In the case an exception should be rethrown, method \alib{exceptions;Exception::Add} may be used to
add one or more new entries to the exception instance.
Method \b %Add has the very same signature as the constructor of the exception.

For the same reasons that exceptions are to be caught by reference, rethrowing is recommended
to be done as reference.

While the following code compiles and works:

       catch( alib::Exception& e )  {
          throw e.Add( ... );
       }

...this is the more efficient way to do it:

       catch( alib::Exception& e ) {
          e.Add( ... );
          throw;
       }

\I{################################################################################################}
## 3.6 Collecting Exceptions ## {#alib_exceptions_exceptions_collect}
Being a \ref alib_contmono_smv "self-containted type" that implements a shared-pointer
behavior, exceptions and their collected entries, can nicely be collected, for example for later
analysis or repeated log output. This is done for example by just copy the exception as a value
into a vector. As soon as the vector is cleared, all dynamic allocations that the exception
instances performed (usually one allocation of 1 kilobyte per exception) are automatically freed.

\I{################################################################################################}
\I{################################################################################################}
## 3.6 Resourced Exceptions ## {#alib_exceptions_exceptions_res}

To enable the external maintenance of the description strings of exception entries (as explained
in previous chapters, it is proposed and recommended to add a descriptive, human-readable string
as a first argument to each exception entry), this module uses the concept of
\ref alib_enums_records "ALib Enum Records".
This concept in turn allows the definition of records be performed using
\ref alib_mod_resources "externalized string resources".

When custom enum types are associated with <em>ALib Enum Records</em> of type \alib{exceptions;ERException},
elements of this type become accepted by method \alib{exceptions;Exception::Add} which internally adds
the resourced description string.

Please consult the reference documentation provided with the links in this section for further
information.
 
*/
