// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#include "alib/alib.hpp"

#if ALIB_MODULE_CONFIGURATION && !defined(HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

#if !defined(_GLIBCXX_TYPEINDEX) && !defined(_TYPEINDEX_)
#   include <typeindex>
#endif

namespace aworx { namespace lib {



/** ************************************************************************************************
# %ALib %Boxing #
This is namespace documentation includes a lengthy introduction and tutorial section.<br>
<b>Reference documentation</b> is found below:

- [Nested Namespaces](#namespaces)
- [Classes](#nested-classes)
- [Typedefs](#typedef-members) and
- [Functions](#func-members)


Contents of the <b>manual</b> section:

\ref              alib_namespace_boxing_prerequisites       "1. Prerequisites"                                               <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_prereq_modules        "1.1 Modularization of ALib"                                   <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_prereq_unittests      "1.2 Unit Tests"                                               <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_prereq_reading        "1.3 How To Read This Documentation"                           <br>

\ref              alib_namespace_boxing_intro               "2. Introduction to Boxing"                                      <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_intro_goals           "2.1 Goals of ALib Boxing"                                     <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_intro_boxing          "2.2 The Term 'Boxing'"                                        <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_intro_cpp             "2.3 Boxing In C++?"                                           <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_intro_templated       "2.4 Using A Templated Constructor"                            <br>
                                                                                                                             <br>
\ref              alib_namespace_boxing_basics             "3. Basics Of ALib Boxing"                                        <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_basics_tmp           "3.1 Perfect Auto-Boxing in C++ With Template Meta Programming" <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_basics_boxer         "3.2 Class Boxer: The Master Of A Boxed Type"                   <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_basics_dots          "3.3 Struct BoxData: The Values Stored in a Box"                <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_basics_dots          "3.4 Connecting The Dots: Boxing And Unboxing"                  <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_basics_nonbij        "3.5 Non-Bijective Type Mapping"                                <br>
                                                                                                                             <br>
\ref              alib_namespace_boxing_types               "4. Arrays, Fundamental Types and Strictness of Unboxing"        <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_types_arrays          "4.1 Boxing Arrays And Vectors"                                <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_types_ftypes          "4.2 Boxing Fundamental Types"                                 <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_types_nonunboxable    "4.3 Non-unboxable Types"                                      <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_types_enums           "4.4 Boxing Enum Types"                                        <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_types_boxedas         "4.5 Boxing Types As They Are"                                 <br>

\ref              alib_namespace_boxing_interfaces          "5. Box Interfaces: Virtual Method Invocation on Boxes"          <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_interfaces_sample     "5.1 A Simple Interface Sample"                                <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_interfaces_default    "5.2 Default Interfaces"                                       <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_interfaces_built-in   "5.3 The Built-In Interface Classes"                           <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_interfaces_change     "5.4 Changing The Default Behavior"                            <br>

\ref              alib_namespace_boxing_strings             "6. Boxing Strings"                                              <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_strings_chararr       "6.1 Strings As Character Arrays"                              <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_strings_lw_vs_hvy     "6.2 'Lightweight' And 'Heavy' String Types"                   <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_strings_iapply        "6.3 Interface IApply"                                         <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_strings_iformat       "6.4 Interface IFormat"                                        <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_strings_custom        "6.5 Boxing Custom String Types"                               <br>

\ref              alib_namespace_boxing_variadic            "7. Variadic Arguments"                                          <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_variadic_standard     "7.1 Using Boxes With Variadic Arguments"                      <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_variadic_class_boxes  "7.2 Class Boxes"                                              <br>

\ref              alib_namespace_boxing_usage                "8. Using The Library"                                          <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_usage_bootstrap        "8.1 Bootstrapping ALib Boxing"                               <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_usage_3rdparty         "8.2 Built-In Support for 3rd Party Types"                    <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_usage_lifecycle        "8.3 Lifecycle Management And Data Deletion"                  <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_usage_performance      "8.4 Performance Considerations"                              <br>
&nbsp;&nbsp; \ref alib_namespace_boxing_usage_dump             "8.5 Debugging ALib Boxing"                                   <br>

\ref              alib_namespace_boxing_summary              "9. Summary"                                                    <br>


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_prerequisites
# 1. Prerequisites #

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_prereq_modules
## 1.1 Modularization of %ALib

Classes found in this namespace comprise what is called a \b 'module' of the \alib class library.
The term "module" here means, that this feature of \alib may be used independently from
the \alib class library itself. We have designed module \alibmod_nolink_boxing to have (almost) \b no
dependencies to other portions of \alib in respect to header file inclusion, the use of other common
\alib types, macros and other sweet \alib sugar.


The rational for this effort lies in the fact that this way the community can use
\alibmod_nolink_boxing without including the complete \alib stack of utilities.

The separated module classes, structured as a lean library are found at GitHub project
[%ALib %Boxing](https://github.com/AlexWorx/ALib-Boxing).<br>
Besides using this module or the full \alib class library, there is a third option, which is
found with
[%ALib %Boxing & Strings](https://github.com/AlexWorx/ALib-Boxing-And-Strings). This is is
module \alibmod_nolink_boxing packaged together with module
[ALib Strings](https://github.com/AlexWorx/ALib-Strings). The rational for providing this mixed
module package is that boxing and strings have some nice synergies, but still users of both
might not want to use more of \alib.

The complete overview of available distributions/modules is given  \ref aworx::lib "here".


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_prereq_unittests
## 1.2 Unit Tests

The
[single distribution version](https://github.com/AlexWorx/ALib-Boxing)
of this module is not equipped with extensive unit tests. Instead, just some quick sample and test
executables are available. Unit tests are placed in the
[full ALib distribution](https://github.com/AlexWorx/ALox) (currently bundled with ALox).

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_prereq_reading
## 1.3 How To Read This Documentation

This documentation is a mix of a manual and tutorial and is probably too detailed and
<em>time-consuming</em> for experienced C++ programmers.<br>
If you just want to get to understand quickly what this library is about and how to use it,
the short version is given in last chapter
\ref alib_namespace_boxing_summary "9. Summary".


 \~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_intro
# 2. Introduction to %Boxing
\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_intro_goals
## 2.1 Goals of %ALib %Boxing

The original motivation to implement \alibmod_nolink_boxing was the need to forward an arbitrary amount
of function arguments of arbitrary type to other functions.
Until here, C++ 11 has all mechanisms to implement this, using
[variadic template arguments](http://en.cppreference.com/w/cpp/language/parameter_pack).
The limitation of the template approach is of-course that all needs to happen at compile time what
tremendously limits the concept - for the sake of gaining unrivalled performance!

We were searching for a way to pass arguments and interpret their type and value at runtime.
With other programming languages which provide a superclass \c Object and <b>runtime type information</b>
this is a no-brainer. In C++ it needs some effort to achieve this. This library provides a very
generalized, extensible approach that is not limited to variadic function arguments.

Thinking further, it quickly becomes clear that identifying the original type of an object and
retrieving its original value is not sufficient: With multiple-inheritance in place, it is often
not interesting if an unknown object originates from a certain base type.
The main purpose of multiple-inheritance is to be able to invoke an interface method that a
base type provides. (This statement is arguable: our justification for stating this is the
observation that e.g. in programming language \e Java, multiple-inheritance is limited to
implementing interface classes - a limitation that does not seem to be very severe for
Java programmers!).

In C++, keyword \c dynamic_cast allows to convert pointers to target types and failures to do
so (if the type is not convertible) are detected by having a \c nullptr as a result. Now, this
works fine as long the original type is a pointer to a virtual class.
Our goal was to support non-virtual classes as well as fundamental C++ types, e.g. \c int or \c char*.
Furthermore, we wanted to support instances of 3rd party types. This means, objects of types that
are instantiated in code parts that are not aware of the interfaces needed when we use them.
For example we wanted to be able to pass a "window" object created in a windowing toolkit library
 or an object representing an open file, created in a library of the operating system.

Finally, there was a last goal: Passing and processing the values should be \e "easy".
The easiest way to to pass an object definitely is to pass it "as it is".
We will show in this documentation what "easiness" means in respect to processing the values!


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_intro_boxing
## 2.2 The Term '%Boxing'

In context of programming languages the term \e "boxing" describes the act of
embedding a \e 'value' that is defined by a \e 'type' into a different type and probably a different
way of physically storing the value in the memory of the machine.
Together with this conversion, meta information may be added.

For example in programming languages \e Java and \e C# (that in this case share exactly the same syntax)
the following simple line of code performs \e boxing:

        int i= 5;
        Object myInteger= i;

The boxing is performed \b implicitly. There is no "cast" syntax or other boxing
syntax needed to tell the compiler what to do. Therefore, this is also called "auto boxing" and
in fact the term boxing mostly is used when auto boxing is done.

Some IDEs are smart enough to provide information about where in the source code such 'auto-boxing'
takes place, because it can be easily overseen by a programmer. A programmer should be aware of
boxing, because some runtime effort for boxing a value is involved.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_intro_cpp
## 2.3 %Boxing in C++?
In C++, a different concept exists that may be used to reach similar results. This mechanism
is called
[Implicit type conversion with single-argument constructors](http://www.cplusplus.com/doc/tutorial/typecasting/).

Let us look at a quick sample:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_IMPLICIT_CONSTRUCTOR

With this class definition, we now can write:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_IMPLICIT_CONSTRUCTOR_INVOCATION

In this sample, the C++ compiler includes an implicit constructor invocation and creates an object
of type integer. This can be seen when 'stepping into' the line of code with a C++ debugger.<br>
When function parameters are declared to take a constant parameter of a class type, the implicit
constructor invocation is also inserted by the compiler along with the machine code of invoking the
function:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_IMPLICIT_CONSTRUCTOR_FUNCTION
\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_IMPLICIT_FUNCTION_INVOCATION

\note
  - While \c const is mandatory for parameters to support implicit constructor invocation,
    using reference types (\c '&') is not. The advantage of using
    reference types is that in the case the parameter is provided already as a value of the expected
     type (and therefore no implicit construction is needed).
  - C++ keyword \c explicit used with constructor declarations, instructs the compiler to suppress
    implicit invocations.
  - Implicit constructor invocation is only performed on <b>one direct level</b>. Possible nested
    invocations are not detected and need manual insertion of constructor calls.

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_intro_templated
## 2.4 Using A Templated Constructor

To extend the sample of class \b %IntegerBox above to accept other types for implicit conversion,
a dedicated constructor for such type is needed to be added. This quickly takes
us to the idea to use a templated constructor. Let's move away from class \b %IntegerBox
and call it just \b %Box, because we want to box arbitrary values, not only integer types:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOX_TEMPLATED

And yeah, this works, the compiler has no problem with this code now:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOX_TEMPLATED_2

Furthermore, with the following function defined:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOX_TEMPLATED_3

...also the following code gets nicely compiled:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOX_TEMPLATED_4

The code above does not store the value. It is just the proof of concept that shows that the
invocation of method \b %myFunction without explicit conversion of values is accepted by the
compiler.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_basics
# 3. Basics Of %ALib %Boxing

\anchor alib_namespace_boxing_basics_tmp
## 3.1 Perfect Auto-Boxing in C++ With Template Meta Programming

The solution of this library for implementing the templated constructor discussed above, is the
use of template meta programming (aka [TMP](https://en.wikipedia.org/wiki/Template_metaprogramming)).
\alib class
\ref aworx::lib::boxing::Box "Box" utilizes TMP utilities found in standard library header
[type_traits](http://www.cplusplus.com/reference/type_traits) and secondly invokes
template functions defined in this namespace, which can be specialized for supported types
(aka boxable types). This way, the "supported types" have no need to be extended with a specific
interface method.
The types just do not "know" about the fact that their objects might be boxed and shipped to other
places inside these boxes.

But before we talk about the details of these <em>'specialized template functions'</em> that a user
of the library needs to implement to make a custom type \e 'boxable', we first
need to discuss another important detail of the implementation of \alibmod_nolink_boxing. This detail
is about how we interface into boxed values.

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_basics_boxer
## 3.2 Class %Boxer: The Master of a Boxed Type
Reading this introduction, it should have become clear, that class \b %Box can not have
virtual methods. The simple reason is, that for auto-boxing values, everything has to go into
the same type of box. Therefore, class \b %Box will never have derived types and virtual methods
are not a way to proceed.

Consequently, the duties of virtualization need to be burdened to some other type.
With \alibmod_nolink_boxing this type is implemented with class \ref aworx::lib::boxing::Boxer "Boxer".
Objects of this class are the true masters of a \b %Box. Each boxed object, along with the data that
it embeds, stores a pointer to "its" \b %Boxer.

For each boxable type, a corresponding  derived \b %Boxer type is needed. Furthermore, derived
boxer types follow the <b>strict singleton</b> design pattern. This means that
- a singleton instance of each boxer type exists
- this instance is the only instance of this type.

\note Class \b %Boxer is similar to [vtables](https://en.wikipedia.org/wiki/Virtual_method_table)
      of instances of virtual C++ class types.

In the process of boxing a value, the right singleton of class \b %Boxer, representing the boxed
type needs to be chosen and added as a pointer to the boxed object.

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_basics_boxdata
## 3.3 Struct %BoxData: The Values Stored in a %Box
Before we connect all dots, let us quickly discuss a final important point: What data do we
store in a box? Remember, the constraints:
 - Any type should be boxable.
 - We can not have different versions of class \b %Box.

The latter constraint disallows embedding different data fields for different types.
We also want to disallow the creation of a heap-allocated, type-specific data object.
Heap allocations in such situations are strict "no-go" for the \alib library.
Its design rules clearly forbid that auto-boxing involved the allocation of heap memory.

This imposes, that all non-trivial types have to be boxed as pointers to their original object.
While fundamental types are stored by value, the data that is to be stored needs
to have as many bytes as the maximum of
  a) the biggest fundamental type and
  b) the size of a pointer on the target machine
have.
This type is defined with \ref aworx::lib::boxing::boxvalue "boxvalue".

Besides this box value \alibmod_nolink_boxing stores a second value that is called \b %Length.
The Length is used when boxing C++ array types or std::vector types. Custom types might make use
of this integer value differently and thus give it a completely different meaning
(without being able to change the name).

Both values are aggregated in simple struct \ref aworx::lib::boxing::BoxData "BoxData".

\note
 Although this library provides \b %Length to represent the length of the first dimensions of
 arrays, this does not mean that multi-dimensional arrays can not be boxed.
 The decision to store the one array dimension explicitly is a compromise. The rational is:
 - This allows convenient access to the first dimension of array elements.
 - This allows container types to offer an indexed access to elements of boxed values.
 - The overhead of storing a length value even for types that do not need it, is taken into account
   to just provide this convenience.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_basics_dots
## 3.4 Connecting The Dots: %Boxing And Unboxing

Let us summarize what was described so far about the classes and relationships:

- Class \b %Box stores a pointer to a singleton of a type derived from class \b %Boxer
- In addition class \b %Box incorporates a struct \b %BoxData to store the boxed value. This struct
  in turn contains
  - a value of type \ref aworx::lib::boxing::boxvalue "boxvalue"
  - an integer value to store the length of arrays and container types.

Together, these are \b three values. On a typical 64-Bit machine, each value is 64-Bit long. The
size of a \b %Box hence is 24 bytes.

Furthermore it was said, that class \b %Box uses a templated constructor.
This constructor needs to set the three values.

We have never talked yet about retrieving values back from a box. We call this process \b unboxing.
Class \b %Box provides template methods
- \ref aworx::lib::boxing::Box::Unbox        "Box::Unbox" and
- \ref aworx::lib::boxing::Box::UnboxElement "Box::UnboxElement"

A sample shows how \b %Box::Unbox can be used. Template parameter \p{TBoxable} of this method
specifies the result type:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_1

We can write:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_1_2

what generates output:

\snippet "DOX_ALIB_BOXING_SAMPLE_1.txt"     OUTPUT

Fine! But this code is missing something very important: prior to unboxing a boxed value, it is
needed to check the box type. This is done with template method
\ref aworx::lib::boxing::Box::IsType  "Box::IsType":

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_2

It is now OK to invoke function \b useBox with arbitrary types:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_2_2

The output is:

\snippet "DOX_ALIB_BOXING_SAMPLE_2.txt"     OUTPUT

As can be seen from the sample, when template method
\ref aworx::lib::boxing::Box::IsType  "Box::IsType" is used to determine the type of a boxed
value, the type is specified with the template parameter of the method. The implementation of
the method is very simple and thus performs fast: the method compares the pointer to the boxer
singleton stored in the box with that boxer singleton that would be used if a value
of template parameter \p{TBoxable} was boxed. Now, due to the template meta programming, this is
quite all done by the compiler at compile time. The assembly code is just comparing two memory
addresses.

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_basics_nonbij
## 3.5 Non-Bijective Type Mapping

There is another small but very important detail hidden in the sample code of the previous chapter:
Although a reference of an object of type \b %MyClass is passed to the implicit constructor call of
class \b %Box, what we are asking for when using the box, is a \b pointer to the class.
So, what happens then if we pass a pointer?

With code:
\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_2_3

The output is:

\snippet "DOX_ALIB_BOXING_SAMPLE_2_3.txt"     OUTPUT

Note that we have not changed the code of method \b %useBox above. The interesting thing is
that it seems to be irrelevant if we pass a reference or a pointer to an object when boxing it.
At first sight, this might be confusing to a C++ programmer - but it is intended design.
Mathematically spoken, \alibmod_nolink_boxing implements a <b>non-bijective</b> mapping of
C++ types and boxed types. In other words: two different C++ types can be mapped and boxed to the
same boxed type.

The benefit of this was demonstrated already: method \b %useBox did not need to be extended to
support a next type check. And furthermore, there is no need for the
programmer to care about adding an ampersand <c>'\&'</c> when passing the object.
Reconsidering the goals of \alibmod_nolink_boxing, this makes a lot of sense and still
\alibmod_nolink_boxing is type safe!

Of-course, the drawback is, that once the object is boxed, you can not determine any more if it
came from a pointer or an object.

What was observed is true for the default behaviour of boxing. Default in the sense that
custom type \b MyClass was boxed without giving explicit instructions to \alibmod_nolink_boxing on
how to do this. The default behavior maps
- Non-pointer types to pointer versions of the types (\c T &rarr; \c T*) and
- Pointer types to itself (\c T* &rarr; \c T* ).

But we can change the way any C++ type is boxed. Let us build a sample where a reference to a type
is boxed as such:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_BIJ

Invoking the test method with a reference and with a pointer:
\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_BIJ_2

..leads to the following output:
\snippet "DOX_ALIB_BOXING_SAMPLE_BIJ.txt"     OUTPUT

From the output we can see that now a pointer type is distinguished from a value or reference
type when boxing an instance of class \b %MyBijBoxed.<br>
Some notes on the code:

- To define a custom mapping, struct \ref aworx::lib::boxing::T_Boxing needs to be specialized.
  Helper macro \ref ALIB_BOXING_SPECIALIZE is used in this sample for doing this
  (which is in general recommended).
- We need to specialize \b %T_Boxing for both types, although
  the default implementation had mapped  \b MyBijBoxed* already to identical type \b %MyBijBoxed*.
  The reason for this is that if we only customized the mapping only for \b MyBijBoxed,
  then the default implementation had detected that and had boxed type \b MyBijBoxed* also
  as a value type!
- For type \b MyBijBoxed, specializations of methods
  \ref aworx::lib::boxing::T_Boxing::Boxing   "T_Boxing::Boxing" and
  \ref aworx::lib::boxing::T_Boxing::Unboxing "T_Boxing::Unboxing" are given.
  They copy the value of the object into the box.
- For type \b MyBijBoxed*, \b no specialization for these methods are given.
  Here we can rely on the default implementation, which copies the pointer.

Well, this sample, is really just a sample! It seldom makes sense - or is even possible - to
box custom types as \b values. The reason is the very limited "data space" in class \b %Box.
We gave the sample because it is important to understand these basics for later chapters
of this documentation:
- When it comes to boxing fundamental C++ types, the non-bijective approach
  is very helpful by greatly simplifying the use of boxed values.
- In the area of boxing arbitrary string types (\c char*, \c std::string, \c QTString,
  <b>%ALib Strings</b>, etc.), this mechanism does not only simplify things, it also allows to
  write methods that process external types which are not "known" to them!


Let us summarize and repeat the important takeaways of this chapter.<br>
\par Important:<br>
1. The mapping of types is not bijective: Two or more <b>C++ types</b> may be mapped (boxed) to the same
   resulting <b>"boxed type"</b>.
2. Only the resulting types (the boxed types), need to be checked and retrieved when using boxes.
3. The mapping of any C++ type is customizable.
4. By default, values (references) and pointers of a C++ type get both boxed to the pointer type
   (\c T &rarr; \c T*, \c T* &rarr; \c T*).<br>
   As a result (in the default case) there is no need to add either '\&' or '\*'! when passing
   "something" to the constructor of class \b %Box, while still everything is type-safe.
5. In respect to values and pointers of the same type, the following options for boxing
   are possible:
   - No specialization: default (as described in point 4.)
   - Exclusive specialization of \c T*: Same result types as with no specialization, but optional
     custom boxing of the pointer.
   - Exclusive specialization of \c T: Both, \c T and \c T* get boxed as \c T. Optional custom
     boxing/unboxing of \c T applies to \c T* as well. If in this case \c nullptr is boxed,
     then the raw value of the boxed will become \c 0. This possibility has to be taken into account
     on unboxing.
   - Specialization of both types: \c T gets boxed as \c T, and \c T* as \c T*. Both types
     can have custom boxing/unboxing as needed.

\note
  In later chapter \ref alib_namespace_boxing_types_boxedas "4.5 %Boxing Types As They Are" a
  mechanism is shown that allows to suppress non-bijective boxing.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_types
# 4. Arrays, Fundamental Types and Strictness of Unboxing
\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_types_arrays
## 4.1 %Boxing Arrays and Vectors

### 4.1.1 C++ Arrays

Before we talk about boxing fundamental types, let us quickly talk about boxing arrays. We modify
the simple sample above to print the contents of \c int arrays.:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_ARR

A test invocation:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_ARR_2

Leads to the following result:

\snippet "DOX_ALIB_BOXING_SAMPLE_ARR.txt"     OUTPUT


For array types, method \ref aworx::lib::boxing::Box::IsType "Box::IsType" is not applicable.
The reason is, that it would not be possible to ask for arrays of arbitrary size.
The template parameter \b TBoxable might be \c int[3] or \c int[25], but can not be \c int[] or
similar. This is not supported by C++. Therefore, we need to use template method
\ref aworx::lib::boxing::Box::IsArrayOf "Box::IsArrayOf", and in this case \c IsArrayOf<int>()
to detect the array.

\note
  With method \ref aworx::lib::boxing::Box::IsArray "Box::IsArray", there is a second, more general
  method existing, which is not a template method and returns true if the box contains an array of
  arbitrary type.<br>
  In addition, method \ref aworx::lib::boxing::Box::ArrayElementSize "Box::ArrayElementSize"
  returns the size of elements stored in the array.

Likewise method \b %Box::IsType, method \b %Box::Unbox is not applicable for array types.
Instead, method
\ref aworx::lib::boxing::Box::UnboxElement "Box::UnboxElement" is to be used to unbox single
elements of an array. The length of an array is received with
\ref aworx::lib::boxing::Box::Length "Box::Length". Both methods are used in the sample above.


### 4.1.2 Multi-Dimensional C++ Arrays
As already mentioned, \alibmod_nolink_boxing does not provide a similar solution for multi-dimensional
arrays. When multi-dimensional arrays are boxed and unboxed, the sizes of the higher
dimensions need to be known:


\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_ARR_3

Output:

\snippet "DOX_ALIB_BOXING_SAMPLE_ARR_2.txt"     OUTPUT

If more dimensions need to be boxed and unboxed with keeping the (runtime) information of
the sizes of the higher dimensions, then those arrays have to be wrapped in a custom (template) type
that allows to retrieve the sizes prior to boxing them and restores them on unboxing.
We do not consider this a huge drawback for the boxing library, especially when
reconsidering its \ref alib_namespace_boxing_intro_goals "goals". Also, we do not provide a
built-in implementation for this.

\note Of-course, alike the sampled arrays of type \c int, any element type can be boxed
      in the same way.

### 4.1.3 Rationale and Technical Background
The special treatment of arrays with \alibmod_nolink_boxing imposes advantages and disadvantages and
hence is the result of a design decision. Alternatives to this implementations are:

1. Storing arrays as pointers to the element type with forgetting their length, or
2. storing even multi-dimensional arrays the way it is done with one-dimensional arrays.

The advantages of the approach taken are:
- One-dimensional arrays can be accessed in a type safe way: \alibmod_nolink_boxing distinguishes
  between a simple pointer to a type and an array of a type.
- The length of the first dimension of an array - if known to the compiler - is automatically stored.

The disadvantages are:
- An extra field for the length of the array needs to be stored with every \b %Box object, even
  if the type does not represent an array.
- Multi-dimensional arrays loose the size of the dimensions starting with the second, even if
  known to the compiler when boxing.
- The Complexity of the API and its use increases, e.g. with extra interface methods \b %Box::IsArray,
  \b %Box::IsArrayOf, \b %Box::ArrayElementSize and \b %Box::UnboxElement.

Technically, arrays are always stored as a pointer to the first element and the length of the
array. While the process of boxing and unboxing values in general is customizable, this not true for
boxing C++ arrays and not true unboxing any type that was boxed as an array.

This has the following consequences:

- Standard or custom collection types can only be boxed as arrays, if they organize their values
  in a block of memory, like class \c std::vector does.
- To avoid receiving each element one by one using \b %Box::UnboxElement, the address of the
  first element can be taken and pointer arithmetic is allowed. The sample from above hence can
  also be (more efficiently!) written as:

  \snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_ARR_4


### 4.1.4 %Boxing Vectors
The term "Vectors" here means collection type <c>std::vector<T, std::allocator<T>></c> and similar
custom (3rd party) types that store their elements in a single memory sequence.
\alibmod_nolink_boxing provides built-in support for class \c std::vector. Objects of the type
are boxed to arrays of the element type, by requesting a vectors' allocated memory (method
\c std::vector::data) and storing this pointer besides its size. Consequently, the vector
object itself can not be unboxed from the type! The advantage of this approach is
(as with any non-bijective type mapping) that the code that processes boxes needs to check for
arrays of a certain element type only. Separated checks for other vector types are not needed.

This behavior can be switched off with compilation symbol \ref ALIB_FEAT_BOXING_STD_VECTOR_OFF.

The following code snippet is taken from header file \c "boxinglib.hpp" and is all that is needed.
Struct \ref aworx::lib::boxing::T_Boxing "T_Boxing" is specialized using a template!
The code can be used as a recipe on how to box other custom vector-like collection types:

\snippet "alib/boxing/boxinglib.hpp"    DOX_ALIB_BOXING_STD_VECTOR



\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_types_ftypes
## 4.2 %Boxing Fundamental Types

In the previous chapters enough information was given, to now start boxing C++ fundamental types.
All that is needed to do, is to specialize struct \b %T_Boxing for the types.
But as we have seen in chapter
\ref alib_namespace_boxing_basics_nonbij   "Non-Bijective Type Mapping",
there are different options to do that: We are free to choose different target types when
boxing one certain type.
This is why the built-in code for boxing fundamental types is just one of many possible ways to do it.
Thus, module \alibmod_nolink_boxing allows to set a custom version in place that
may assign the types differently. A reason to do so might be, to be able to distinguish between
various original fundamental types when using the boxes.

\note
    This freedom of choice is \b limited depending on the distribution of \alibmod_nolink_boxing.
    See \ref aworx::lib::boxing::ftypes "namespace ftypes" for further information.


To understand the built-in boxing of fundamental types, it may be easiest to look at
a code sample that uses it:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_FTYPES_1

\note
  The sample uses classes \c std::string, \c std::wstring, \c std::u16string
  and \c std::u32string for unboxing character arrays.
  To be able to to this, header file \c "alib/compatibility/std_string.hpp"
  needs to be included.
  All details of boxing and unboxing character arrays and string types are discussed in later
  chapter \ref alib_namespace_boxing_strings "6. Boxing Strings".


With the these invocations:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_FTYPES_1_2

the following output is generated:

\snippet "DOX_ALIB_BOXING_SAMPLE_FTYPES_1.txt"     OUTPUT

Method \b %useBox of the sample checks and processes only <b>11 boxed types</b>, while it is
able to 'fetch' <b>33 C++ fundamental types</b>. This demonstrates that the fundamental
types of C++ get aggregated to a much smaller set of destination types when they are boxed.<br>
The rules of aggregation can be described as follows:
- Integer Types
  - Signed integer types (namely \c int_8, \c int_16, \c int_32, \c int_64, and
    \ref aworx::lib::lang::intGap_t "intGap_t") are boxed as
    \ref aworx::lib::boxing::ftypes::boxed_int "ftypes::boxed_int".
  - Corresponding unsigned integer types are boxed as
    \ref aworx::lib::boxing::ftypes::boxed_uint "ftypes::boxed_uint".
- Type \c bool remains \c bool
- Type \c float is converted to \c double, while \c double remains \c double.
- Types \c char, \c wchar_t and \ref aworx::strangeChar (aka \c char16_t or \c char32_t depending
  on the platform) remain identical.
- Pointers to these three character types, are boxed to arrays of those types.

To summarize, the built-in implementation of boxing fundamental types is providing simplification
in respect to using the types while taking into account that information about the originating
type gets lost. The most important loss is probably with type \c float as boxing this types involves
an arithmetic conversion to type \c double, with all its drawbacks.

While this sample already demonstrates the benefit of this simplification, in later chapters
\ref alib_namespace_boxing_interfaces "5. Box Interfaces: Virtual Method Invocation on Boxes" and
\ref alib_namespace_boxing_strings    "6. Boxing Strings" more advantage are shown.

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_types_nonunboxable
# 4.3 Non-Unboxable Types

In the previous chapters, the principle of
\ref alib_namespace_boxing_basics_nonbij "Non-Bijective Type Mapping" was explained and it was
demonstrated how the various C++ fundamental types are reduced to only a few target types.
As shown, the benefit lies in a simplification and performance gain of the code that is using
boxes. Instead of checking for various types of a similar kind, only one boxed type needs to be
processed.

Designing \alibmod_nolink_boxing, the question came up if it should still be allowed to receive
the original type back when unboxing a type, even if it was converted on boxing:
- The advantage if it was allowed would be that no extra conversion (e.g. cast) is needed after
  unboxing a type that is \b not the boxed type.
- The advantage if it was \b not allowed would be that <b>unnecessary code</b> that performs
  type checks and unboxing can be easily avoided, as it is just forbidden!

The decision was taken to make this customizable. In general there are three cases:

<b>1. Unboxing is impossible and therefore forbidden:</b><br>
A sample where unboxing a type is not (easily) possible was already shown in chapter
\ref alib_namespace_boxing_types_arrays "4.1 Boxing Arrays and Vectors". Objects of class
\c std::vector are boxed as arrays: their data is received (using method \c std::vector::data)
together with its length. The original vector object is not stored. Therefore, it can not be unboxed.
If unboxing the type should be allowed, then a new object would need to be created, and the values
copied into it. This is not in alignment with the goals of \alibmod_nolink_boxing and therefore not
implemented.

<b>2. Unboxing would be easily possible but still is forbidden:</b><br>
This is the common practice with the default implementation of boxing C++ fundamental types.
For example, type \c int16_t is boxed as \ref aworx::lib::boxing::ftypes::boxed_int "boxed_int"
(an alias to \c int64_t). Unboxing would just need to cast the value stored "down".
However, it makes no difference in performance to add the cast to the value returned from unboxing
the \b %boxed_int. By disallowing to unbox all possible source types that lead to the same
boxed type, the code that uses a box becomes more efficient.

Here is a quick sample:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_FTYPES_2


<b>3. Unboxing of a type is allowed:</b><br>
Sometimes it is very helpful to allow unboxing a certain boxed type back to several possible
source types. We will learn later in this documentation details about boxing string types.
What we will see is that various string types, like string literals, character pointers,
\c std::string, <b>%ALib Strings </b>, QT-Strings, etc., are all boxed to the same destination type,
namely an array of characters. Now, the implementation allows to unbox all of the named custom
string types from boxed character arrays. This means: A function accepting boxed strings, may
be invoked with any sort of custom string type, while internally it uses solely "its preferred" type
by unboxing into it and processing the string in the format of desire!


Struct  \ref aworx::lib::boxing::T_Boxing
includes method
\ref aworx::lib::boxing::T_Boxing::IsUnboxable "T_Boxing::IsUnboxable"
that is used to create compilation errors if unboxing is forbidden.
The errors are raised (using \c static_assert) when invoking template methods
\ref aworx::lib::boxing::Box::IsType "Box::IsType" or
\ref aworx::lib::boxing::Box::Unbox  "Box::Unbox" for a "forbidden" type if
this method returns \c false.

\note
  Even if unboxing is not allowed, still the declarations of methods
  \ref aworx::lib::boxing::T_Boxing::HasCustomUnboxing "T_Boxing::HasCustomUnboxing" and
  \ref aworx::lib::boxing::T_Boxing::Unboxing          "T_Boxing::Unboxing" have to be given
  in the specialization of the struct with an empty definition. This (odd) fact is
  a matter of the [TMP](https://en.wikipedia.org/wiki/Template_metaprogramming) used.

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_types_enums
# 4.4 %Boxing %Enum Types

All enum types are boxed as values and can be boxed and unboxed without any effort by the user
of the library (i.e. no specialization of \ref aworx::lib::boxing::T_Boxing "T_Boxing" is needed).

This is true for old-style enum types as well as enum classes.

The following code shows a quick sample:
\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_ENUMS
\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_ENUMS_2

\note
  While C++ 11 introduced <c>enum class</c> types, still these are 'limited' classes in the respect
  that enum classes do not provide inheritance. Thus, you can not define an interface method
  that accepts enums coming from custom derived types. This is quite often a problem. Of-course,
  using module \alibmod_nolink_boxing, an interface method now may accept a box, which allows just any
  type passed. But such then accepts anything else and this is not what was wanted.

\note
  For this reason \alib provides class \alib{lang,Enum} which builds on \alib boxing and provides
  a special box that is restricted to boxing enumerations. With overloaded <c>operator==</c>, etc.,
  this class is often the better choice if the goal is to accept different enum types. A good
  sample use case is found with class \alib{lang,Exception}.

\note
  Note, that this class is not included in module distribution of module \alibmod_boxing, but only
  with the full distribution of \alib.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_types_boxedas
# 4.5 %Boxing Types As They Are

There might be situations, where the bijective, simplifying nature of \alibmod_nolink_boxing
should be suppressed and a type that usually would be converted when boxed, passed as is.
To do so, a wrapper class is needed. Such class is provided with template
\ref aworx::lib::boxing::BoxedAs "BoxedAs<Type>". The class is very simple and stores a reference
to the type while accepting references and pointers in its constructors.
The only restriction is that no \c nullptr values must be passed.

In the reference documentation of the class, a
\ref aworx::lib::boxing::BoxedAs "source code sample" is given.

Using this mechanism should be done only if it can not be avoided because the unifying nature
of \alibmod_nolink_boxing is \e undermined this way: Methods that accept boxes as parameters and
process them, need dedicated type detection and unboxing code for boxed type <b>%BoxedAs\<Type\></b>.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_interfaces
# 5. %Box Interfaces: Virtual Method Invocation on %Boxes

In the previous chapters it was shown how \alibmod_nolink_boxing uses C++ techniques like
implicit constructor invocation and template meta programming to be able convert any C++ into
an object of type \ref aworx::lib::boxing::Box "Box". Reconsidering the
\ref alib_namespace_boxing_intro_goals "goals" given in the introduction, then we have met most
of them already. But one is missing: Detect the support for interface methods without knowing
what type we have in front of us!

To reach this goal, \alibmod_nolink_boxing enables the definition and invocation of virtual methods
on boxes. This is very similar to normal virtual method invocations, but as this is working on
boxes and class \b Box can not be virtualized using the normal C++ techniques, we have to defer
this task to the singleton of class
\ref aworx::lib::boxing::Boxer "Boxer" which is attached to each box.

In the previous samples, the code that is using a box (methods \b %useBox of the samples), checks
for different "known" types, extracts these values of the types and does some type-specific things.
With virtual methods, this code is shortened to just one invocation of the virtual method on
the box, no matter what type it has.

Module \alibmod_nolink_boxing introduces class
\ref aworx::lib::boxing::Interface "Interface" which, similar to the boxers, use the singleton
design pattern. During the bootstrap (initialization) of a process, interface singletons are
attached to boxer singletons. This way, one interface singleton is responsible to implement
one or more interface methods for a dedicated boxed type.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_interfaces_sample
# 5.1 A Simple %Interface Sample

Again it is easiest to explain by using some sample code.
Imagine a method is supposed to store boxed objects in a certain format. And this now should be done
wit boxed objects of arbitrary type, by invoking a virtual interface method on those.

<b>Step 1: Creating the Interface Type:</b><br>
What we first need is a new type which represents the interface class. This is created
using three pieces of information:

1. The type needs to be derived from class \ref aworx::lib::boxing::Interface "Interface":
2. In the constructor implementation, the \c typeid of the class itself has to be passed to the
   parent constructor .
3. One abstract virtual method named \b %Invoke has to be added.
   The first parameter of this virtual method needs to be of type <c>const aworx::Box&</c>. It
   will receive the boxed object that the interface method is invoked on. Other parameters
   and the return type may be chosen as needed.

Here is the interface class:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_1_1

<b>Step 2: Creating Derived Types Of The New %Interface:</b><br>
Now, let us assume that this interface should be implemented for integer types and literal strings.
We need two specializations of the interface class. These specializations in addition inherit from
class \ref aworx::lib::lang::Singleton "Singleton" so that we can get a singleton instance for each
of them:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_1_2

We decided to have \b 2 interface classes that are obviously responsible for \b 3 boxed types.
This is reached by doing a type check in method \b IStoreInMyFormat_Tint::Invoke.


In the bootstrap section of our process the singletons of the two interface classes need to be
attached to the right boxers. The easiest way to do this is using templated namespace function
\ref aworx::lib::boxing::DefineInterface "DefineInterface":

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_1_3

This is all that was needed for preparation. Now we can define a function that
- is accepting boxed values
- checks the availability of an interface specialization on the box using
  \ref aworx::lib::boxing::Box::HasInterface  "Box::HasInterface", and
- invokes the interface.

Such method is sampled here:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_1_4

When we invoke as follows:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_1_5

...we will see this result:
\snippet "DOX_ALIB_BOXING_SAMPLE_IF_1.txt"     OUTPUT

We see, the first two invocations obviously invoked the virtual method, while the third one
produced an error. This is correct, as we have no interface defined for boxed type \c double.

There are some interesting things to note here:
- The interface method of the demonstrated custom interface class \b IStoreInMyFormat
  has to be named like \b %Invoke. Furthermore, the first parameter has to be a of type
  <c>const aworx::Box&</c>. The other parameter(s) are user specific, the same as the return type
  is, in our case \c void.<br>
  This method must not be mixed up with method
  \ref aworx::lib::boxing::Box::Invoke "Box::Invoke". The latter is the templated built-in
  method of class \b %Box. It takes variadic template arguments of type \b Arg&& which are
  passed inside the method together with the reference to the box itself to the custom method
  \b %Invoke. This is done with so called
  [C++ 11 perfect forwarding](http://en.cppreference.com/w/cpp/utility/forward), using
  \c std::forward.
- If the return type of our method was different to \c void, this return type needed to be
  specified as a second template parameter (which otherwise defaults to \c void). For example,
  if our method returned \c bool, the invocation would be:

            bool result= box.Invoke<IStoreInMyFormat, bool>( fileID );
- The code would perform slightly faster, if we had split the specialization type
  \b IStoreInMyFormat_Tint into a signed and an unsigned version. The way it was demonstrated here
  we have to user an otherwise unnecessary \c if-statement in the generalized version.
  At the end of the day, this is a matter of taste and constraints. The disadvantage would of
  course be the extra class introduced.<br>
  It is important to understand however, that it is possible to attach one interface singleton
  to more than only one boxable type! In more complicated cases it might be quite reasonable
  to aggregated several types into one specialization.

Because a specialized version of a custom interface class (method) is needed for every type,
the fact that \alibmod_nolink_boxing is not bijective as explained
\ref alib_namespace_boxing_basics_nonbij "previously", may be great relief: If C++ types
are nicely aggregated to fewer boxed types, then fewer interface specializations are needed.

Advanced users may have a look at class
\ref aworx::lib::strings::boxing::IApply_TApplicable "IApply_TApplicable" provided
with module <b>%ALib Strings</b> if packaged together with \alibmod_nolink_boxing. This specialization
of interface class
\ref aworx::lib::strings::boxing::IApply "IApply" is templated and this way allows
to be attached to all types of boxes which represents types that have a built-in or custom support
of applying them to objects of type
\alib{strings,AStringBase,AString}. This way, this templated interface class can
be registered with \alibmod_nolink_boxing in the bootstrap section of a process as follows:

\snippet "stringlib.cpp"     DOX_ALIB_BOXING_SAMPLE_TEMPLATED_INTERFACE

The same can be done in custom bootstrap code with every applicable custom type!


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_interfaces_default
# 5.2 Default Interfaces
Sometimes it is useful to have a default interface in place that becomes used in the case
that no type specific version of an interface is provided. This concept is similar to
C++ virtual methods that are not "pure abstract" and this way are only optionally overwritten
in descendant classes. \alibmod_nolink_boxing supports this concept with method
\alib{boxing,Boxer::DefineDefaultInterface}, or preferably with templated namespace function
\alib{boxing,DefineDefaultInterface} which is a convenience function and internally invokes the
former.

A useful sample is found with module <b>%ALib Strings</b>. If packaged together with
\alibmod_nolink_boxing, templated interface class \alib{strings::boxing,IApply,IApply<TChar>}
becomes available.
Specializations of this interface are supposed to append the contents of a box to a given
\alib{strings,AStringBase,AStringBase<TChar>}. If no type specific version is registered, the
default implementation is used, which writes the type name (using C++ RTTI) and the memory
address of the object in hexadecimal format into the string.

The information whether a default implementation of an interface is provided or not, should be
documented with each custom interface. If so, the corresponding interface can be invoked
without prior checking for the availability.
If an instance of class \b %Box named \b myBox exists, it is safe to invoke:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_2_1

which writes the contents of \c myBox int \c myAString.

In most cases, method \alib{boxing,Box::HasInterface} must not even be used for checking,
as it returns \c true if and only if a type specific interface version is defined.
This means that this code:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_2_2

will \b not invoke the virtual box interface for those types that have only the default
implementation in place!

While this might be irritating in the first moment, the benefit of this approach is that
default interface implementations can be identified and treated differently in the user code:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_2_3


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_interfaces_built-in
# 5.3 The Built-In %Interface Classes

\note
  A quick warning: \alibmod_nolink_boxing is no programming language: It neither provides inheritance, nor
  polymorphism, and much, much more is missing. Maybe we should not even talk about "virtual method
  invocation" as we do when it comes to boxing interfaces. It is really something different, but
  it is language and words that people understand and therefore we use it, knowing that we are talking
  about boxes not objects!

\alibmod_nolink_boxing interface classes which are available as "default interfaces" may be compared to
methods available to root class \b Object of programming languages like C# or Java.
There are three of such default interfaces provided.

<b>%IEquals:</b><br>
%Interface \ref aworx::lib::boxing::IEquals "IEquals" is defined to return \b true, if the contents
of a box equals the given parameter. The parameter to be provided is declared as <c>const %Box%</c>.
Using this 'trick', compared values are boxed prior to compare them. This way, the implementation
already works quite well in some respect as here simply the type and value of two
boxes get compared. For custom class types, which by default get stored as a pointer to an object
of the custom class, this way a pointer comparison is made. If necessary, a specialization
of this interface for custom types may be provided, which does a logical comparison of the contents
of such types. Later in this document, we will talk about boxing string types. Here, such deeper
look into the contents of the various boxable string types (including 3rd party library strings)
may make sense. It all depends on if such comparison is needed the methods that are processing
boxes or not.

The built-in \ref alib_namespace_boxing_types_ftypes "boxing of fundamental types" adds some
specialization already:
- \ref aworx::lib::boxing::ftypes::IEquals_Tdouble     "IEquals_Tdouble" to compare floating point values.
- \ref aworx::lib::boxing::ftypes::IEquals_TncharArr    "IEquals_TncharArr" and
  \ref aworx::lib::boxing::ftypes::IEquals_TwcharArr "IEquals_TwcharArr" to character arrays.

This is no specialization for integer types and other array types given. Consequently, in the
area of fundamental types, the following rules apply:
- If two integer types have the same signedness and the same value, \b %IEquals returns \c true.
  However, it will return \c false if the signedness of the originating type is different, even
  of both share the same value, for example \c 0.
- Only if two arrays have the same C++ fundamental element type, the same length and share the same
  pointer to the data array, this method returns \c true. It will return \c false in all other
  cases, even if the value represented by the array elements are the same.

This behavior can be changed (extended) if needed: as it was explained, specializations for
fundamental of any interface may be added even if the built-in boxing of fundamental types is chosen
(which is mandatory for all \alib distributions apart from the pure \alibmod_nolink_boxing module).

The different \c operator methods of class \b Box use this interface internally, as well as
the next interface:

<b>%IIsLess:</b><br>
%Interface \ref aworx::lib::boxing::IIsLess "IIsLess" is defined to return \b true, if the contents
of a box is less than the given parameter. Same as with \b IEquals, the parameter to be provided is
declared as <c>const %Box%</c>.

Using this interface in combination with \b %IEquals, the operator methods in class box are
implemented. Those are:
- \alib{boxing,Box::operator==},
- \alib{boxing,Box::operator!=},
- \alib{boxing,Box::operator<},
- \alib{boxing,Box::operator<=},
- \alib{boxing,Box::operator>} and
- \alib{boxing,Box::operator>=},

With the default implementations provided with \alib (including "string %boxing" what is described
in the next chapter) boxes can be used in algorithms like in the following sample:

  \snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_COMPARE

This generates the following output:

  \snippet "DOX_ALIB_BOXING_COMPARE.txt"     OUTPUT




<b>%IIsNull:</b><br>
%Interface \b IIsNull introduces the concept of <em>"nullable"</em> types to \alibmod_nolink_boxing.
This concept is useful for pointer types or string types and may be adopted for custom types as well.
For information on how the default implementation of \alibmod_nolink_boxing defines this concept,
by default, refer to the \ref aworx::lib::boxing::IIsNull "reference documentation of class IIsNull".

<b>%IIsEmpty:</b><br>
%Interface \b IIsEmpty introduces the concept of <em>"emptiness"</em> to \alibmod_nolink_boxing.
This concept is useful for array types or string types and may be adopted for custom types as well.
For information on how the default implementation of \alibmod_nolink_boxing defines this concept,
by default, refer to the \ref aworx::lib::boxing::IIsEmpty "reference documentation of class IIsEmpty".

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_interfaces_change
# 5.4 Changing The Default Behavior

The description of \alibmod_nolink_boxing so far showed that - while the core concept is straightforward,
lean and clean  - different boxing behavior is possibly implemented for C++ types.
The degrees of freedom are in three areas:
- Mapping of C++ types to boxed types
  (see \ref alib_namespace_boxing_basics_nonbij  "3.5 Non-Bijective Type Mapping"),
- the "strictness" about unboxing types that are mapped to other types
  (see \ref alib_namespace_boxing_types_nonunboxable  "4.3 Non-Unboxable Types") and
- the availability of invokable interfaces.

Technically the first two items are not changeable without using preprocessor switches or leaving
out header files. Here, specializations of template structs and functions are used and C++ does not
allow to change such in later header files. Therefore, \alib
provides compilation symbols \ref ALIB_FEAT_BOXING_FTYPES_OFF and \ref ALIB_FEAT_BOXING_STD_VECTOR_OFF
to tweak the behaviour during compilation.

The third one, the definition of interfaces \b allows overriding an already set interface!
It is no problem, to specify a different interface implementation class in a repeated invocation of
\ref aworx::lib::boxing::DefineInterface          "DefineInterface" and
\ref aworx::lib::boxing::DefineDefaultInterface   "DefineDefaultInterface".

This way, a user of \alibmod_nolink_boxing might replace the default interfaces to suit an applications'
need. The warning here is: Changes of interfaces change runtime behavior. This means that the whole
process, including library code is affected!

\note
  All interface definitions should be made in the bootstrap section of a process. First,
  built-in definitions should be performed and then custom initializations which extend (or overwrite)
  then existing definitions. Later chapter
  \ref alib_namespace_boxing_usage_bootstrap  "8.1 Bootstrapping %ALib %Boxing"
  of this document elaborates more on this topic.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_strings
# 6. %Boxing Strings

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_strings_chararr
# 6.1 Strings As Character Arrays

Simple string types are nothing more than an array of characters. Hence, to describe them,
a pointer to the beginning of the array is needed and a length. So called <em>C-strings</em>
which are generated by the C++ compiler for string literals like \c "ABC",
are using termination character \c '\0' instead of a length.
A C++ string literal, has a an array type with the array length being the length the string <b>plus 1</b>.
For example, literal <c>"ABC"</c> has type \c char[4] or <c>L"12345"</c> has type <c>wchar_t[6]</c>).
When boxed, the length of the array is evaluated using TMP and stored in field
\ref aworx::lib::boxing::BoxData::Length "BoxData::Length". However, because with literals the
trailing \c '\0' is counted in the type declaration by the compiler, the stored length has to be
decreased by one. This is achieved with partially specialized template method
\ref aworx::lib::boxing::T_SetArraySize  "T_SetArraySize".
The whole reason to have this method is to fix the length of literals! Consequently
the default implementation of boxing fundamental types, specializes this method for each of the
four C++ C-string literal types.

In the moment a literal is assigned to a \b pointer to the character type, the information about the
array length is lost! It may not be clear to all C++ programmers, but in this line of code:

        const char* cstring= "ABC";

uses an implicit conversion from \c char[4] to <c>const char*</c>! Along with this conversion,
the information that the pointer is a pointer to a zero-terminated string array (C-string) is
lost.<br>

Now, in the following sample:

        Box box1( "ABC" );
        const char* cstring= "ABC";
        Box box2( cstring )

objects \c box1 and \c box2 share the same boxed type. Unfortunately, as the compiler has
casted away the array size, when boxing object \c cstring, the length of the array is not known
when boxing.

A lazy mechanism is implemented to detect the array length:
- The length is set to \c 0 if a \c nullptr was boxed or if the first character in the array
  equals termination character  \c '\0'.
- Otherwise, the length is set to <c>-1</c>!

By using this approach, built-in interfaces
\ref aworx::lib::boxing::IIsNull  "IIsNull" and
\ref aworx::lib::boxing::IIsEmpty "IIsEmpty" already work well. Only when needed, for example
with specialization of interface \ref aworx::lib::boxing::IEquals "IEquals" for character array,
types, it is checked if the size is \c -1. If it is, then the true length of the string is evaluated using
\c std::strlen, respectively \c std::wcslen.

We conclude all assumptions and rules as follows:
- All character pointers and string types are boxed to arrays of the corresponding character type.
- If the length of the array is known, it is stored with field \b BoxData::Length.
- If the length of the array is not known (when boxing a pointer to a character type),
  field \b BoxData::Length is set to \b -1 and it is assumed that the character is zero-terminated!

These rules apply to the following parts of \alibmod_nolink_boxing

1. The default implementation of \ref alib_namespace_boxing_types_ftypes "boxing fundamental types".
2. The implementation of boxing \ref aworx::lib::strings "ALib Strings".
3. The implementation of boxing 3rd-party types provided with optionally includable header files.<br>
   Among the supported types are \c std::string (and siblings) and string classes of the
   [QT Class Library](https://www.qt.io).

This is why the use of complete \alib or combined module <b>%ALib %Boxing and Strings</b> are
missing the freedom of choice to use compiler symbol \ref ALIB_FEAT_BOXING_FTYPES_OFF to switch
off the built-in boxing of fundamental types!


\note
  It is important to understand, that \alib assumes that boxed pointers to character arrays are
  pointers to zero terminated character arrays!
\note
  If character pointers should be boxed that do \b not point to zero-terminated C-strings, those
  have to be wrapped in a custom class (with optionally custom boxing and interfaces).
  Otherwise, unboxing those values and the invocation of certain interface methods lead to
  undefined behavior.

The sample of chapter \ref alib_namespace_boxing_types_ftypes "4.2 Boxing Fundamental Types"
used classes \c std::string, \c std::wstring, \c std::u16string and \c std::u32string
for unboxing character arrays and pointers to character types.
Now we will explain how this works.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_strings_lw_vs_hvy
# 6.2 'Lightweight' And 'Heavy' String Types

In the moment \alibmod_nolink_boxing is used in combination with module
\ref aworx::lib::strings "ALib Strings", classes
\alib{strings,StringBase,String} and
\alib{strings,AStringBase,AString} become available. The first is a "lightweight" class:
It is non-virtual and stores solely the pointer and the length of the string.
Class \b %AString instead is 'heavy' in the respect that though it is still not virtual, it
provides its own memory management.
Both types get boxed to the same destination type, \c char[], ust as "fundamental" C-strings are.

The advantage of boxing all string types, including custom, 3rd-party
types as proposed is that they can be handled in one <c>if</c>-statement:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_3_0

Furthermore, if for all lightweight string types a corresponding mechanism for unboxing such type
from a character array is in place, then each of these types may be unboxed, no matter what the
original string type was:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_IF_3_0_2

This is a great simplification and enables code that processes strings to unbox any string to
just a (lightweight) string type type of choice.

Of-course, there is a disadvantage: In case of "heavy" string types, it is not possible to
unbox the original type. For example, by storing just the buffer and length when boxing objects
of class
\alib{strings,AStringBase,AString}, of-course the object itself is lost and is not
unboxable. If a an object of a "heavy" string type is needed to be boxed in its original type
(e.g. for the purpose of modifying its contents inside the function that processes the box), such
objects need to be wrapped as explained in
\ref alib_namespace_boxing_types_boxedas  "4.5 Boxing Types As They Are".
The
\ref aworx::lib::boxing::BoxedAs "documentation of class BoxedAs" shows a sample of how to do this
with objects of class \b %AString.

In a later chapter, a step by step sample of how boxing custom string types should be implemented
is given.
But let us first elaborate further what boxing of <b>%ALib Strings</b> provides in addition.

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_strings_iapply
# 6.3 %Interface %IApply

Class \alib{strings,AStringBase,AString} already supports a TMP-based mechanism to append
any sort of custom objects in a string representation (see
\alib{strings,T_Apply}). However, for boxing we can not use this.
The reason is obvious: once a type is boxed, the compiler looses information about the boxed
type. Therefore, of-course, no template programming allows to append (in \b %AString-terminology
to "apply") boxed values.

Consequently, we need a box interface that is invokable on a boxed object and writes the box
contents to a given instance of class \b %AString. Such interface is provided with class
\ref aworx::lib::strings::boxing::IApply "IApply".
Dedicated specializations exist for character and wide character array types.

For all types which already implement \alib{strings,T_Apply},
an existing templated specialization can be used! This is provided with template class
\ref aworx::lib::strings::boxing::IApply_TApplicable  "IApply_TApplicable".
As a result, there are two ways of implementing interface \b %IApply for a custom boxable type:
1. As with other boxing interfaces: Derive the interface class and implement the writing of values
   of the boxed type to the AString in a custom way. Then attach this interface to the boxing system.
2. Make the type \e "applicable" to class \b %AString (as instructed
   \ref aworx::lib::strings::T_Apply "here") and then attach the templated,
   generic interface \b %IApply_TApplicable to the \alibmod_nolink_boxing system using the following
   line of code to the bootstrap section of your software:

        aworx::lib::boxing::DefineInterface<MyType*, false, aworx::lib::strings::boxing::IApply_TApplicable<MyType*>>();

   A simple preprocessor macro is defined with #ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE, which
   allows to shorten this definition code to:

         ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE(MyType*)

   (Replace \e 'MyType' with the name of your type in both cases.)


The second approach has the advantage, that this way, the custom type is then directly applicable
to class \b %AString - independent from boxing. Therefore, this is the recommended way to proceed.

To close the circle, class \b %Box is made applicable to \b %AString. This means, any box can
be applied to \b %AString objects using operator \c '<<' or method \c _() (apply).
The implementation simply invokes box interface \b %IApply on the \b %AString object.
Here is a quick sample code:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_STRINGS

which produces the output:

 \snippet "DOX_ALIB_BOXING_SAMPLE_STRINGS.txt"     OUTPUT

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_strings_iformat
# 6.4 %Interface  %IFormat

%Box interface \alib{strings::boxing,IApply}, introduced in the previous chapter, is used
to simply append the contents of a boxed value to an \b %AString. There are no options on
formatting the value available.

This is changed with interface
\ref aworx::lib::strings::boxing::IFormat "IFormat" which is very similar to \b %IApply but takes
an additional parameter \p{formatSpec}, which is a string value providing information about how
the contents is written. The format specification is fully type and implementation specific.

For example, \b %ALIb class \alib{time,CalendarDateTime} provides (native) method
\alib{time::CalendarDateTime,Format} to write time and date values in a human readable and customizable
way. This method also requires a format specification.
Because \b %CalendarDateTime is only a helper class, the boxing interface class that
invokes CalendarDateTime::Format, is available for boxed values of type
\alib{time,DateTime}. The interface attached to boxed values of this type is found with boxing
interface class \alib{time,IFormat_DateTime}. The implementation of the interface method \b %Invoke
is therefore again rather simple:

\snippet "alib/time/timelib.cpp"     DOX_ALIB_BOXING_IFORMAT_DATETIME

%Boxing interface \b %IFormat is used by \alib internally in module <b>%ALib Strings</b> with class
\alib{strings::format,FormatterBase,Formatter} which is used to format the contents of \b %AString
objects. The whole class relies on \alibmod_nolink_boxing and therefore it is not included
with the single module distribution of <b>%ALib Strings</b> (which excludes \alib boxing).


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_strings_custom
# 6.5 %Boxing Custom %String Types

The previous four chapters have shown how fundamental C++ strings types (C-strings),
"lightweight" class \alib{strings,StringBase,String} and rather "heavy" class
\alib{strings,AStringBase,AString} are boxed.

Furthermore, interfaces
\ref aworx::lib::strings::boxing::IApply  "IApply" and
\ref aworx::lib::strings::boxing::IFormat "IFormat" have been introduced.

ALib provides built-in support for 3rd party string libraries already:
- Classes \c std::string, \c std::wstring, \c std::u16string and \c std::u32string.<br>
  (These types may not really be named "3rd party", but from the \alib perspective, they
   are because they are not used internally.)

- For string types found in [QT Class Library](https://www.qt.io).<br>

More information about support for boxing third party types is given with the documentation of
namespace  \ref aworx::lib::boxing::thirdparty.


This following chapter wants to provide a recipe on how to implement boxing for a 3rd party
"lightweight" string so that it smoothly fits into this setup.
We are doing the exercise by walking through the source code from the \c std::string support as
a step by step sample.


<b>Step 1:</b><br>
Within namespace \c %aworx::lib::boxing, we specialize template struct
\ref aworx::lib::boxing::T_Boxing "T_Boxing". The recommended way to do this, is the use of macro
\ref ALIB_BOXING_SPECIALIZE_CB_CUB "ALIB_BOXING_SPECIALIZE_CB_CUB":

\snippet "alib/compatibility/std_string.hpp"     DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_1

With this mapping, type \c std::string is boxed to \c char[]. And because no specialization
for type \c std::string* is given, this pointer type is treated the same as the value/reference type.

The 4th parameter \c true of the macro denotes that unboxing of the type is allowed.
The version of the macro with suffix \c _CB_CUB specializes struct \b %T_Boxing in a way that
custom implementation for boxing and unboxing get declared.

<b>Step 2:</b><br>
Define method
\ref aworx::lib::boxing::T_Boxing::Boxing "T_Boxing<std::string>::Boxing" for the specialized type
struct.
This function performs the boxing  and receives the address of the string buffer as well
as the string length:

\snippet "alib/compatibility/std_string.hpp"     DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_2

<b>Step 3:</b><br>
Define method
\ref aworx::lib::boxing::T_Boxing::Unboxing "T_Boxing<std::string>::Unboxing".
This function performs the unboxing and returns a value of type \c std::string:

\snippet "alib/compatibility/std_string.hpp"     DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_3

Note the check for negative array length! This might happen, because the box that gets unboxed
using this method is not necessarily created from an object of type <c>std::string</c>! In case
it was created from type \c char*, the string length is not known. This was explained in previous
chapter \ref alib_namespace_boxing_strings_chararr "6.1 Strings As Character Arrays".

With these three little pieces code in place, we are already done! To summarize, all we needed to do is
- tell the boxing system that type \c std::string is mapped to \c char[] and that unboxing
  the original type is allowed.
- specify how boxing and unboxing values is performed.

Once a \c std:string is boxed, the box is not distinguishable from a similar box that
was created using
- a string literal,
- a \c char*
- an object of type \b %aworx::String
- a std::string or
- an other custom string type that is boxed using this recipe.

\note
  The consequence is quite helpful: A function that accepts string types as a boxed parameter, can
  be implemented by testing for and unboxing <b>only one</b> custom string type, while still a user
  of the function might pass any other string type, even types that the processing code does not
  even "know" about!<br>
  In addition, using this recipe, no specializations of the standard interface methods (e.g.
  \ref aworx::lib::boxing::IEquals "IEquals") are needed, because they are implemented for
  destination type \c char[] already.


Here is some sample code on boxing \b std::string with this setup:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_SAMPLE_RECIPE

This produces the output:

\snippet "DOX_ALIB_BOXING_SAMPLE_RECIPE_1.txt"     OUTPUT

<b>%Boxing "Heavy" String Types</b><br>

"Heavy" string types are boxed to \c char[] without the possibility of unboxing the original
type and hence, they can be unboxed to a "lightweight" string type only.

Therefore, adding support for boxing of "heavy" string types is even more simple:
- In <b>Step 1</b> macro \ref ALIB_BOXING_SPECIALIZE_CB is used. This declares a custom boxing method but
  no custom unboxing.
- <b>Step 2</b> remains the same.
- <b>Step 3</b> is omitted. You're done.


Optionally, to be complete, one thing might be done for "heavy" string types: As explained already,
to alternatively box such types without loosing the object, wrapper class
\ref aworx::lib::boxing::BoxedAs "BoxedAs" might be used (see chapter
\ref alib_namespace_boxing_types_boxedas  "4.5 Boxing Types As They Are"). Now, it might be
useful to add support for <b>%ALib Strings</b> for this boxed type. I.e. to implement interface
\ref aworx::lib::strings::boxing::IApply "IApply" for values of this "BoxedAs"-type.
A ready to use templated incarnation of interface \b IApply exists with template class
\ref aworx::lib::strings::boxing::IApply_BoxedAs_TApplicable "IApply_BoxedAs_TApplicable".
As its name suggests, it may be used only with types that implement
\alib{strings,T_Apply}. But this support is suggested to be added anyhow for
custom string types (as noted in chapter
\ref alib_namespace_boxing_strings_iapply        "6.3 Interface IApply").

As a consequence, support for applying custom types wrapped in \b BoxedAs is a matter of a single
line of bootstrap code. Again, we are using the original source from \c std::string support to
sample this:

\snippet "alib/compatibility/std_string.hpp"     DOX_ALIB_BOXING_STRINGS_CUSTOM_RECIPE_4



 \~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_variadic
# 7. Variadic Arguments

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_variadic_standard
## 7.1 Using %Boxed Objects With Variadic Template Arguments
With class Box in place, it becomes possible to define functions and methods that take an arbitrary
value as an argument. The need for this is often combined with the need for the possibility to
allow an arbitrary number of such arbitrary arguments. C++ 11 introduced
[variadic template arguments](http://en.cppreference.com/w/cpp/language/variadic_arguments) for this.

Class \b %Box might greatly simplify the use of this language feature and provide a \b type-safe
and \b indexed way to access variadic arguments. (Of-course it does, this was the original
\ref alib_namespace_boxing_intro_goals "goal"!)

The following quick sample demonstrates this:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_VARIADIC_SAMPLE

With this function definition, it can be called like this:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_VARIADIC_SAMPLE_1

It is only a single, simple line of code that fetches all function parameters and puts them
into an array of boxes.

Of-course, the classical recursive approach to process template arguments using class Box may
also be implemented but avoiding the recursion makes the code easier and more readable.

The sample above can be slightly modified to use
[C++ 11 Perfect Forwarding](http://en.cppreference.com/w/cpp/utility/forward) which in some
situations is a little more efficient and produces smaller code. The following code snippet
uses this technique and may be copied as a recipe on how to implement variadic template
functions with \alibmod_nolink_boxing:

\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_VARIADIC_RECIPE


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_variadic_class_boxes
## 7.2 Class %Boxes

In the previous chapter it was demonstrated how simple the use of variadic template arguments
gets with \alibmod_nolink_boxing. The recipe given, uses a single line of code to let the compiler
create an array of objects of class Box.
This is sufficient in many cases, but obviously using container class \c std::vector<aworx::Box>
instead of a simple array would give more flexibility: It allows to add and remove boxes from the
array and to pass the array to other (non-templated functions) without passing its size in an
extra parameter.

For this and more purposes, class #Boxes is provided. It publically inherits from
\c std::vector<aworx::Box> and provides a constructor and method \b %Add, each accepting
templated variadic arguments. This way, its use is as simple as this:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOXES_SAMPLE_1

In this sample, three boxed objects are added to the vector in the constructor and then two
more are added using method
\ref aworx::lib::boxing::Boxes::Add "Boxes::Add".
All methods of parent class \c vector are available, e.g. this code:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOXES_SAMPLE_2

clears the array and adds two new boxed objects to it.

Using this class instead of a simple C++ array in a function using variadic template arguments
then looks like this:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC

\note
  The only change to the recipe given at the end of the previous chapter is the use of
  class \b Boxes instead of an array. The advantage of the recipe version is that the array
  is created on the \b "stack". In contrast to this class Boxes, being derived from \c std::vector,
  uses \b "heap memory" to store the boxes.<br>
  This is a disadvantage of using class \b %Boxes that should not be underestimated. (Programmers
  generally tend to underestimate the performance impact of heap allocations.)
  Hence, its use is only recommended if the flexibility of using a vector is needed.

Besides providing variadic template arguments, method \b %Boxes::Add uses some template meta
programming to "flatten" the array in the case that another instance of class \b %Boxes is added.
In other words, if an instance of class \b %Boxes is passed to \b %Boxes::Add, the boxes contained
in this instance are copied into the destination vector! Due to this fact, when using the sample
method from above, the invocation:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_1
produces the following output:
\snippet "DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_1.txt"     OUTPUT

The reason why this is implemented like this, is that the user of a method has a next benefit:
He/she has the freedom of choice to either pass all parameters just inside the function
call or to collect all objects prior to the call in an own instance of class \b %Boxes and then just
pass this instance as a single argument (or together with other, fixed arguments).

This makes the use of the function more flexible, without the need of providing an overloaded
version that accepts and processes an object of \b %Boxes directly.

Finally, besides detecting objects of class \b %Boxes inside method \b %Boxes::Add, it is also
detected if an object of class \b %Boxes is passed as a boxed object.
Let us first look at a sample and its result:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_2
\snippet "DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_2.txt"     OUTPUT

Looking at this sample a reader might think "Wow, this is cool, but where is the use case for this?".
Generally spoken, this is useful when a method has several overloaded versions with different
parameters, and still should support to accept an arbitrary amount of any type of arguments.
In this moment, it might get quite complicated (or impossible!) to define the methods properly in
the sense that no ambiguities may occur when invoking them.
A solution here is to declare the method to accept just exactly one <c>const %aworx::Box&</c>
argument instead of a variadic list of arguments.

If inside the method this box is passed into a local instance of class \b %Boxes,
a user might invoke the method with just a single argument of arbitrary type (which gets boxed),
or with an arbitrary amount of arguments, by collecting those in class \b %Boxes. This might be
done right in the invocation.<br>
To demonstrate this, we use the method from above, but instead of accepting variadic template arguments,
it accepts now just a single argument of type <c>const %Box&</c>:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE

This can be invoked as follows:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE_1

...which produces:

\snippet "DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE.txt"     OUTPUT

A real world sample can be found in the logging library \ref aworx::lib::lox "ALox" which is
built on \alib and makes a lot of use of \alibmod_nolink_boxing.
While straightforward methods
\ref aworx::lib::lox::Lox::Info    "Lox::Info",
\ref aworx::lib::lox::Lox::Verbose "Lox::Verbose", etc. accept variadic template arguments as objects
to be logged, method
\ref aworx::lib::lox::Lox::Once "Lox::Once" is more complicated: Various overloaded versions exist
that interpret the term "once" differently. Therefore, each overloaded version accepts only one
object to log - which is often enough for a simple log message.
However, this is no restriction of \alox: If multiple objects should be logged "once", e.g. to
provide a formatted output of custom objects, then those multiple objects may be passed by wrapping
them in class \b %Boxes.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_usage
# 8. Using The Library

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_usage_bootstrap
## 8.1 Header Inclusion And Bootstrapping %ALib %Boxing

### 8.1.1 Header Inclusion

To make \alibmod_nolink_boxing available in a compilation unit, header file \c "alib/alib.hpp" has
to be included. Depending on the \alib distribution that is used (complete \alib vs. the use
of just a module of \alib that incorporates the boxing facilities), this header will
include the provided variant of header \c "alib/distribution.hpp" to select the right
(available) portions.

\note
  A direct inclusion of headers of namespace \b %aworx::lib::boxing is forbidden and will
  throw a \c \#pragma error.

If pure module
[%ALib %Boxing](https://github.com/AlexWorx/ALib-Boxing) is used, compilation symbol
\ref ALIB_FEAT_BOXING_FTYPES_OFF may be given to suppress the use of the built-in
\ref alib_namespace_boxing_types_ftypes "default implementation of boxing fundamental C++ types".

If combined module [%ALib %Boxing & Strings](https://github.com/AlexWorx/ALib-Boxing-And-Strings)
is used, the provision of compilation symbol \ref ALIB_FEAT_BOXING_FTYPES_OFF will suppress
the the default implementation of boxing <b>%ALib String</b> types as well, and consequently will
not allow to use class
\alib{strings::format,FormatterBase,Formatter} and associated types.

Besides header \c "alib/alib.hpp", no other header needs to be included, apart from what is
documented in  chapter
\ref alib_namespace_boxing_usage_3rdparty "8.2 Built-In Support For 3rd Party Types".

### 8.1.2 Bootstrapping %ALib %Boxing
Bootstrapping means the creation of singletons of derived Interface types and the assignment of
those to the right singletons of class Boxer. This needs to be done only once and preferably
in the moment a process is created, especially before boxing is used and before parallel
execution threads are started.

For bootstrapping \alibmod_nolink_boxing all that is needed to do is to invoke method
\alib{lang,Library::Init} on static singleton \ref aworx::lib::ALIB.
This method will do the bootstrapping for all alib portions that are included in the
selected \alib distribution.

If a software defines own types of class \b %Interface for custom boxable types, such interfaces
have to be registered within the bootstrap section likewise. See chapter
\ref alib_namespace_boxing_interfaces_sample "5.1 A Simple Interface Sample" for more information
on how to do this.

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_usage_3rdparty
## 8.2 Built-In Support for 3rd Party Types
ALib provides built-in support for boxing a few 3rd party types, among them are various \c std::string
types, \c std::string_view (if compiled with language standard C++ 17) and string types of the
 [QT Class Library](https://www.qt.io).
 This support is documented in #aworx::lib::boxing::thirdparty (which is not a "real" namespace but
used for collecting such documentation).


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_usage_lifecycle
## 8.3 Lifecycle Management And Data Deletion

This chapter now can be kept really short: \alibmod_nolink_boxing does not provide lifecycle
management or data creation or deletion mechanisms in respect to values (objects) that get boxed.

Instead, a box is just a very lightweight wrapper class and the responsibility for the data
wrapped remains where it is (in the hands of those code parts, which created the boxed object).

Of-course, if a box contains a pointer to data (which is the default when custom types are boxed),
then the pointer has to remain valid until the box is not used anymore.

When reconsidering the \ref alib_namespace_boxing_intro_goals "goals" of this
software library, the main use case is to enable functions and methods to accept just anything
possible as a parameter defined as <c>const Box&</c>.
Here, everything is very safe: The box is created on the program stack
(implicitly constructed by the compiler) and when the receiving function (method) returns, the
stack is [unwinded](https://en.wikipedia.org/wiki/Call_stack#Unwinding)
and the box object is gone. In this standard use case, the only thing that
might happen is that a parallel thread destroyed data that the box points to. But this is
really all standard C++ behavior.
\par Again in short:
  There are no destructors, no delete statements, shared pointer or other mechanisms provided for
  the life-cycle management of boxed data.

As a final note, boxes are rather "immutable" objects. Unless custom interface methods do so,
there is no method that overwrites or otherwise changes the data wrapped in a box.
But in respect of changing the data that the box wraps, the box is very mutable! Just as
with construction, assignment operator might be used. As the box is a POD-type class, standard
compiler implementation move semantics is used.<br>
Here is an example on how to change the contents (and type!) of a box:
\snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_BOX_SET

In the moment an instance of class \b %Box is boxed, the TMP constructor detects this and
copies the contents of the given box. As a result, both boxes wrap the same object instead of
having one box wrapping the other.

\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_usage_performance
## 8.4 Performance Considerations
The reason why the effort of implementing this library is needed is the C++ language design
principle to be as performant and close to the hardware as possible. Other programming languages
are designed for other goals. E.g. in languages Java or C# the principle "everything is an object"
is (almost) implemented. In these languages, all class types have runtime type information and the
only types that do not have that are plain arithmetic types like \c char, \c int or \c double.
(In C++ we have non-virtual classes, structs and POD types.)
And what do these languages do when a plain type is passed to a method that expects an object?
Right: The compilers perform "auto-boxing" of the values to pre-defined class types
\c Char, \c Integer or <c>Double</c>!

Therefore it is indicated to have do quick analysis of the memory and performance impact of
using \alibmod_nolink_boxing. We do this in a rather loose order:

### 8.4.1 A General Note On C++ RTTI
It is a widespread misunderstanding that using RTTI (runtime type information) with C++
has a huge performance impact. In fact the impact to get information on a type using keyword
\c typeid is "almost zero" (it is constant, in
[Big O notation](https://en.wikipedia.org/wiki/Big_O_notation):  O(1)). It is just about
copying a pointer to a static struct residing in the fixed data segment of a compilation unit.

### 8.4.2 %Boxer And %Interface Singletons
For each (destination!) type that is boxed, a singleton of a type derived from class
\ref aworx::lib::boxing::Boxer "Boxer" is created. This is almost irrelevant and comparable to
the singletons of [vtables](https://en.wikipedia.org/wiki/Virtual_method_table) which are created
by the compiler for each virtual class.

Likewise, for each derived version of type \ref aworx::lib::boxing::Interface "Interface"
one instance has to be created.

### 8.4.3 Footprint Of Class %Box
Class Box contains three members: A pointer to the boxer singleton and the data struct
(which consists of two). For example on a standard 64-Bit platform each is 8 bytes wide, hence
an instance of class \b %Box on those platforms has a size of 24 bytes.
In most cases, such boxes are created in "stack memory" which allocates and deallocates in
zero time (yes, its less than "O(1)", it is just nothing).
Once created, to pass them to another function or store them in a container like
\ref aworx::lib::boxing::Boxes "Boxes", these 24 bytes have to be copied. Well, this is three times
more than copying just a pointer.

### 8.4.4 Creation Of A %Box
When a value is boxed, hence an object of class \b %Box is created, two things have to be done.
First the right boxer is identified. This is done using (inlined) TMP code and "magically"
this is reduced to the inlined retrieval of a singleton.

\note
  The reader might assume that the retrieval of a singleton in inlined template code is reduced
  to a single assembly statement that copies a pointer to a static object. Unfortunately
  at least when support for Windows OS DLLs is needed, this gets a little more complicated
  due to the nature of the DLL design on this operating system.
  The good news is that it still is very performant, the bad news is that the code that is inlined
  is much larger than expected.<br>
  Details are given in documentation of class \ref aworx::lib::lang::Singleton.

Secondly, field \ref aworx::lib::boxing::Box::data "Box::data" has to be set. Again, this is
inlined TMP code and when optimized should in most cases be as fast and short as copying a pointer
and an integer value.

### 8.4.5 %Box::IsType And %Box::IsArrayOf
Template method
\ref aworx::lib::boxing::Box::IsType    "Box::IsType" compares the internal pointer to the
singleton of class \b %Boxer with the singleton of class \b %Boxer that would be chosen
if the given type (the template parameter) was boxed. Therefore, the impact is the same
as boxing a value - excluding the process of boxing the data itself - plus a pointer comparison.

Template method \ref aworx::lib::boxing::Box::IsArrayOf "Box::IsArrayOf" performs even faster:
Just two simple pointer comparisons are needed.

### 8.4.6 %Box::HasInterface, %Box::GetInterface And %Box::Invoke
Template method
\ref aworx::lib::boxing::Box::HasInterface  "Box::HasInterface" performs a lookup in
a hash table ([std::unordered_map](http://en.cppreference.com/w/cpp/container/unordered_map))
and returns \c true if the interface was found, \c false otherwise.
The key is the C++ RTTI struct \c type_info of the given template type, a static value for the
compiler.
The lookup is also performed in O(1) - if you believe in computer science! - which in reality means
"almost no time".

Template method
\ref aworx::lib::boxing::Box::GetInterface  "Box::GetInterface" has double the work: if the
interface is not found with the search in the hash table for type-specific interfaces, then
a next lookup is done in a different hash table, the one used to store default interfaces.

Finally, template method
\ref aworx::lib::boxing::Box::Invoke  "Box::Invoke" first uses \b %GetInterface to receive
the interface. Then, a standard C++ virtual function call is performed.
Passing the templated parameters "twice" should be irrelevant, due to "perfect forwarding".
In the case that no interface method is found a default value of the return type \p{TReturn}
is created. Depending on the type, this might invoke a constructor.

### 8.4.7 Compile Times
There is another negative 'performance' impact when using \alibmod_nolink_boxing: Due to the
use of TMP methods and need for the inclusion of standard header \c \<type_traits\> that provides
support for that, the time to compile a code unit increases with the use of \alibmod_nolink_boxing.
This increase can be quite "dramatic" in the case that otherwise such techniques are not used (with
other includes or the compilation unit itself).

### 8.4.8 Conclusion
Honestly, we consider the implementation of \alibmod_nolink_boxing to be as performant as possible.
The impact on code size when invoking methods that do auto-boxing of arguments is relatively higher
than the impact on execution performance. Inlining is used wherever reasonable and the library
chooses generally to take preference of performance over code size.

Most important, no heap memory allocations are performed with boxing types.
The only exclamation is with the use of class #Boxes. Therefore, methods that "emplace" arbitrary
arguments in this container, might choose to reuse such object, e.g. by making it a protected field
of the class that offers the method.


\~Comment ####################################################################################### \~
\anchor alib_namespace_boxing_usage_dump
## 8.5 Debugging %ALib %Boxing

In debug compilations, a few debug methods are available that allow a little insight into
the setup of \alibmod_nolink_boxing.  Class \b %Box provides method

- \alib{boxing,Box::DbgGetReferenceType}

which returns an internal reference type of the boxer that the box points to.
The type can be passed to static method

- \alib{boxing,Boxing::DbgGetInterfaces}

which returns a list of types representing the
\ref aworx::lib::boxing::Interface "Interface"(s) attached to the box type.


Then, library class \b %Boxing exposes the static methods used to get information about the overall
setup of \alibmod_nolink_boxing:

- \alib{boxing,Boxing::DbgGetKnownBoxers}
- \alib{boxing,Boxing::DbgGetKnownInterfaces}
- \alib{boxing,Boxing::DbgGetDefaultInterfaces}
- \alib{boxing,Boxing::DbgGetKnownInterfaceImpl}

All these methods are quite basic as they are returning one or a vector of \c std::typeinfo structs.
Class \ref aworx::lib::debug::TypeDemangler "TypeDemangler", which is also only available in debug
compilations, is a very simple helper to "demangle" type names returned by \c std::typeinfo.name().

When module \alibmod_nolink_boxing is used with a distribution that includes module <b>%ALib Strings</b>,
things become more "handy". Module <b>%ALib Strings</b> injects class
- \ref aworx::lib::debug::Boxing "Boxing"

into namespace \b %aworx::lib::debug that uses the functions listed above and does a little
formatting and conversion. Please consult the reference documentation of class
\ref aworx::lib::debug::Boxing "Boxing" for detailed information. At this point, we want
to conclude with some quick samples.

Let us start with template method
\ref aworx::lib::debug::Boxing::GetBoxingInfo "Boxing::GetBoxingInfo", which collects information
about how C++ types are boxed. The type is passed as a template parameter:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_DEBUG_4

The output is:

\snippet "DOX_ALIB_BOXING_DEBUG_4.txt"     OUTPUT


To inspect the type of a box, the following code may be used:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_DEBUG

The output is:

\snippet "DOX_ALIB_BOXING_DEBUG_1.txt"     OUTPUT

Now, to inspect which interfaces are invokable on a box, these can be listed as follows:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_DEBUG_2

Note that in this sample, the box is created once more implicitly, as a string literal is passed
to the method!<br>
The output will be:

\snippet "DOX_ALIB_BOXING_DEBUG_2.txt"     OUTPUT

Method \ref aworx::lib::debug::Boxing::Dump "Boxing::Dump" aggregates all of the above and provides
overview about the boxed types and their. Invoked like this:

\snippet "DOX_ALIB_BOXING.cpp"      DOX_ALIB_BOXING_DEBUG_3

It produces an output similar to this:

\snippet "DOX_ALIB_BOXING_DEBUG_3.txt"     OUTPUT

Note that the output includes types that we have used during this tutorial.



\~Comment ####################################################################################### \~
\anchor  alib_namespace_boxing_summary
# 9. Summary

This is a fast recap of what was explained in this documentation/tutorial and
maybe all that experienced C++ programmes need to read:

<b>What %ALib %Boxing is:</b><br>
- \alibmod_nolink_boxing is a free C++ software library that provides runtime type information and
  virtual method invocation on any C++ type. To make this possible, values of arbitrary types are
  "boxed", which means wrapped into another, generic type, namely class
  \ref aworx::lib::boxing::Box "Box"
  Then, the wrapper objects provide an interface identify the original types at runtime, to "unbox"
  the values and to invoke virtual methods on the boxes.
- The disadvantage of this approach is that code that wants to make use of runtime type information
  need to work on the "boxes" instead on working on the original types.
- The advantage of this approach are:
  - Works with any type, including non-virtual types and even non class types, like fundamental
    types \c int or \c double.
  - Works on externally created objects (e.g. objects created in a system library)
  - Run-time Interfaces can be implemented independent from the original type implementation
    (e.g. for types of 3rd party libraries).
  - Used only when needed: No performance or memory penalty for supported types.
  - A built-in, minimum support is available for just any C++ type (including custom types),
    which includes a default version of boxing and unboxing the data and to perform the
    runtime type checks.
  - Functions and methods can be declared to accepts any type of object, without the
    need of conversion: Any value may be passed to such functions "as is".
- The central class of \alibmod_nolink_boxing is class
  \ref aworx::lib::boxing::Box "Box", which uses
  [TMP](https://en.wikipedia.org/wiki/Template_metaprogramming) to accept and store a single value of
  \b arbitrary C++ type in its constructor. Passing and encapsulating a value in an object of
  class \b %Box is called "boxing".
- One-dimensional array types are boxed in a special way. Their value can be accessed by returning
  the original array type or by accessing individual array elements.
- Once a value is boxed, it can be passed to other methods and be processed. Processing a boxed value
  involves three possible actions:
  1. Determine the type of the boxed value (at runtime) using methods
     \ref aworx::lib::boxing::Box::IsType    "Box::IsType",
     \ref aworx::lib::boxing::Box::IsArray   "Box::IsArray" and
     \ref aworx::lib::boxing::Box::IsArrayOf "Box::IsArrayOf".
  2. Retrieving the value back. This is called "unboxing" and done with methods
     \ref aworx::lib::boxing::Box::Unbox        "Box::Unbox" and
     \ref aworx::lib::boxing::Box::UnboxElement "Box::UnboxElement"
  3. Invoking definable "virtual methods" on the box, which process the boxed value in a
     type-specific way. Methods that support this are
     \ref aworx::lib::boxing::Box::HasInterface "Box::HasInterface",
     \ref aworx::lib::boxing::Box::GetInterface "Box::GetInterface" and
     \ref aworx::lib::boxing::Box::Invoke       "Box::Invoke",

- The advantage of the invocation of virtual methods are twofold:
  - The processing code does not need to detect (and "know") the type of the boxed value
  - The logic performed in the virtual method can be used anywhere

- Different C++ types may be boxed to the same boxed type ("destination type"). This means, that
  the mapping of C++ types to boxed types is not bijective.

<b>How %ALib %Boxing is used:</b><br>
- Depending on the configuration and distribution module of \alib, built-in support for
  boxing and unboxing of the following C++ types is provided:
  - Fundamental C++ types like \c char, \c int, \c double, etc. and pointers and arrays of those.
  - \alib string types
  - Third party string types, like \c std::string or those of [QT Class Library](https://www.qt.io).

- The provided implementation of
  \ref alib_namespace_boxing_types_ftypes "boxing fundamental types"
  makes a lot of use of the non-bijective boxing and hence reduces the complexity of code when
  processing boxed values. The huge list of C++ fundamental types is covered with only a few type
  checks.

- The following built-in invokable interface methods (in fact they are classes providing a virtual
  method named \b %Invoke) are available:
  - \ref aworx::lib::boxing::IEquals  "IEquals"
  - \ref aworx::lib::boxing::IIsNull  "IIsNull"
  - \ref aworx::lib::boxing::IIsEmpty "IIsEmpty"

  With module <b>%ALib Strings</b> this list is extended to
  - \ref aworx::lib::strings::boxing::IApply    "IApply"
  - \ref aworx::lib::strings::boxing::IFormat   "IFormat"

- A default interface method implementation, available to all boxed types that are not equipped with
  a type-specific implementation, can be registered. The following built-in interface types define such
  default:
  - \ref aworx::lib::boxing::IEquals  "IEquals"
  - \ref aworx::lib::boxing::IIsNull  "IIsNull"
  - \ref aworx::lib::boxing::IIsEmpty "IIsEmpty"
  - \ref aworx::lib::strings::boxing::IApply   "IApply"

  When a default implementation is available, method
  \ref aworx::lib::boxing::Box::HasInterface "Box::HasInterface" does not need to be used prior
  to invoking the interface on a \b %Box. In fact, method \b %Box::HasInterface returns \c true
  if and only if a type specific interface is in place.

<b>How %ALib %Boxing is extended:</b><br>
- User defined types (e.g. structs and classes) are supported without the need of adding type
  specific  code to \alibmod_nolink_boxing. The boxed type by default is a pointer type of the
  original type.

- To change the default behaviour of boxing a custom type, template struct
  - \ref aworx::lib::boxing::T_Boxing      "T_Boxing<TBoxable>"

  needs to be specialized. It is recommended to use macro \ref ALIB_BOXING_SPECIALIZE
  for this purpose.

- To implement custom logic for boxing and unboxing a value, methods
  - \ref aworx::lib::boxing::T_Boxing::HasCustomBoxing    "T_Boxing<TBoxable>::HasCustomBoxing" and
  - \ref aworx::lib::boxing::T_Boxing::HasCustomUnboxing  "T_Boxing<TBoxable>::HasCustomUnboxing"

  need to return \c true in the specialization. Furthermore, methods

  - \ref aworx::lib::boxing::T_Boxing::Boxing    "T_Boxing<TBoxable>::Boxing"
  - \ref aworx::lib::boxing::T_Boxing::Unboxing  "T_Boxing<TBoxable>::Unboxing"

  need to be defined.



- To add new interface methods to custom types or any other boxable type, an interface type has
  to be derived from class
  \ref aworx::lib::boxing::Interface  "Interface". An optional default implementation and
  type-specific versions of that interface have to be implemented by deriving in turn from the
  custom interface class and by overwriting method \b %Invoke.<br>
  The interfaces have to be registered for boxed type once in the bootstrap section of a software
  process, using namespace functions
  \ref aworx::lib::boxing::DefineDefaultInterface  "DefineDefaultInterface" and
  \ref aworx::lib::boxing::DefineInterface         "DefineInterface".

- Reasons for extending \alibmod_nolink_boxing to add support for custom types are:
  - If a different boxed type should be the result of boxing a custom type. e.g. to do
    non-bijective boxing, which means several custom types should result in the same boxed type.
  - If boxing or unboxing need to be customized.
  - If built-in interface methods should be specialized for a custom type
  - If new, custom interface methods should be made available.

<b>Functions With Arbitrary Arguments:</b><br>
- \alibmod_nolink_boxing is very valuable when functions or methods that should accept arbitrary
  types are needed. When a function parameter is of type <c>const aworx::Box&</c>, this
  parameter accepts any C++ type "as is".
- In addition, the implementation and use of
  [variadic template arguments](http://en.cppreference.com/w/cpp/language/parameter_pack)
  becomes extremely easy.
- For even more more sophisticated function parameter processing, class
  \ref aworx::lib::boxing::Boxes   "Boxes" is provided.

<b>Other Notes:</b><br>
- \alibmod_nolink_boxing needs to be \ref alib_namespace_boxing_usage_bootstrap "bootstrapped"
  once at the start of a process
- \ref  alib_namespace_boxing_usage_dump "Debug facilities" are provided that allow retrospection
  of the setup of \alibmod_nolink_boxing in respect to types that were boxed and interfaces that
  were set at a distinct point in time.

***************************************************************************************************/
namespace boxing
{

// #################################################################################################
// Init()
// #################################################################################################
#if ALIB_MODULE_STRINGS
void Boxing::init( Phases phase )
#else
void Boxing::Init()
#endif
{
#if ALIB_MODULE_STRINGS
    if( phase == Phases::resourceset )
#endif
    {
        // set built-in boxer interfaces
        DefineDefaultInterface<IEquals >();
        DefineDefaultInterface<IIsLess >();
        DefineDefaultInterface<IIsNull >();
        DefineDefaultInterface<IIsEmpty>();
        DefineDefaultInterface<IIsTrue >();

        #if ALIB_FEAT_BOXING_FTYPES

            DefineInterface<double         , false, ftypes::IEquals_Tdouble  >();
            DefineInterface<nchar          , true , ftypes::IEquals_TncharArr>();
            DefineInterface<wchar_t        , true , ftypes::IEquals_TwcharArr>();
#if ALIB_MODULE_STRINGS
            DefineInterface<strangeChar    , true , ftypes::IEquals_TstrangeCharArr>();
#endif

            DefineInterface<boxed_int      , false, ftypes::IIsLess_boxed_int>();
            DefineInterface<double         , false, ftypes::IIsLess_double   >();


            DefineInterface<boxed_int      , false,         IIsNull_false    >();
            DefineInterface<boxed_uint     , false,         IIsNull_false    >();
            DefineInterface<double         , false,         IIsNull_false    >();

        #endif
    }
}

#if ALIB_MODULE_STRINGS
void Boxing::terminationCleanUp()
#else
void Boxing::TerminationCleanUp()
#endif
{
    // delete boxer and interface singletons. This is to be done only for tools like
    // valgrind, hence in debug mode.
    ALIB_DBG(
        for( auto& it : dbgKnownBoxers         ) delete it.second;
        for( auto& it : dbgKnownInterfaceImpl  ) delete it.second;
    )
}

#if ALIB_DEBUG
std::vector<const std::type_info*>        Boxing::DbgGetInterfaces( const std::type_info& boxerType )
{
    std::vector<const std::type_info*> target;
    Boxer* boxer= dbgKnownBoxers[boxerType];
    for( auto& it : boxer->interfaces )
        target.emplace_back( &it.first.get() );
    return target;
}

std::vector<const std::type_info*>        Boxing::DbgGetKnownBoxers()
{
    std::vector<const std::type_info*> target;
    for( auto& it: dbgKnownBoxers )
        target.emplace_back( &it.first.get() );
    return target;
}

std::vector<const std::type_info*>        Boxing::DbgGetKnownInterfaces()
{
    std::vector<const std::type_info*> target;
    for( auto& it: dbgKnownInterfaces )
        target.emplace_back( &it.first.get() );
    return target;
}

std::vector<const std::type_info*>        Boxing::DbgGetDefaultInterfaces()
{
    std::vector<const std::type_info*> target;
    for( auto& it: Boxer::defaultInterfaces )
        target.emplace_back( &it.first.get() );
    return target;
}

std::vector<const std::type_info*>        Boxing::DbgGetKnownInterfaceImpl()
{
    std::vector<const std::type_info*> target;
    for( auto& it: dbgKnownInterfaceImpl )
        target.emplace_back( &it.first.get() );
    return target;
}

void  Boxing::dbgCheckNewBoxer( Boxer* boxer )
{
    if ( dbgKnownBoxers.find( boxer->type ) != dbgKnownBoxers.end() )
    {
        #if ALIB_MODULE_STRINGS
            ALIB_ERROR( ASTR("Double instantiation of Boxer of Type: \""),
                        AString(debug::TypeDemangler( boxer->type ).Get()).ToCString(), ASTR("\"") )
        #else
            ALIB_ERROR( ASTR("Double instantiation of Boxer of Type: \""),
                                debug::TypeDemangler( boxer->type ).Get()             , ASTR("\"") )
        #endif
        return;
    }

    //dbgKnownBoxers[type]= this;
    dbgKnownBoxers.insert(std::make_pair(std::reference_wrapper<const std::type_info>( boxer->type ), boxer ) );
}

void  Boxing::debugCheckInterfaceExists( const std::type_info& type )
{
    if ( dbgKnownInterfaces.find( type ) == dbgKnownInterfaces.end() )
    {
        #if ALIB_MODULE_STRINGS
            ALIB_ERROR( ASTR("Requested interface was never registered for any boxable type: "),
                        AString(debug::TypeDemangler(type).Get()).ToCString() )
        #else
            ALIB_ERROR( ASTR("Requested interface was never registered for any boxable type: "),
                                debug::TypeDemangler(type).Get()              )
        #endif
        return;
    }
}
#endif



// #################################################################################################
// Class Box
// #################################################################################################

bool Box::operator==(Box const& rhs) const
{
    return  Invoke<aworx::lib::boxing::IEquals, bool>( rhs );
}

bool Box::operator< (const Box& rhs)  const
{
    return  Invoke<aworx::lib::boxing::IIsLess, bool>( rhs );
}


// #################################################################################################
// Default interfaces
// #################################################################################################

// static member definition
TypeMap<Interface*>   Boxer::defaultInterfaces;


bool IEquals::Invoke( const Box& box, const Box& comp )
{
    // non-array types
    if( !box.IsArray() )
        return      box.IsSameType( comp )
                &&  box.PeekRawValue() == comp.PeekRawValue()
                &&  box.Length()       == comp.Length();

    // array types
    if(   !box.IsSameType( comp )
        || box.Length() != comp.Length()
        || ( (box.PeekRawValue() == 0)  != ( comp.PeekRawValue() == 0 ) )   )
        return false;

    return    box.PeekRawValue() == 0
           || box.Length() == 0
           || memcmp( reinterpret_cast<void* >( box .PeekRawValue() ),
                      reinterpret_cast<void* >( comp.PeekRawValue() ),
                      static_cast     <size_t>( box.Length() ) * box.ArrayElementSize()     ) == 0;
}


bool IIsLess::Invoke( const Box& box, const Box& comp )
{
    return           std::type_index( box.GetTypeInfo() ) < std::type_index(comp.GetTypeInfo() )
            ||  (    box.GetTypeInfo<0>() == comp.GetTypeInfo<0>()
                   &&box.PeekRawValue()   <  comp.PeekRawValue()                                  );

}


} // namespace aworx::lib[::boxing]

// The singleton of the library class located in namespace aworx::lib.
boxing::Boxing BOXING;

}} // namespace [aworx::lib]

