// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/**
\page alib_mod_boxing   ALib Module Boxing - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1. Introduction # {#alib_boxing_intro}

The original motivation to implement \alib_boxing_nl was the need to allow functions to
accept an arbitrary amount of arguments of arbitrary type.
While C++ has all mechanisms to implement this (using
\https{variadic template arguments,en.cppreference.com/w/cpp/language/parameter_pack}),
the limitation of the template approach is that all needs to happen at compile-time.
This limits the concept tremendously - for the sake of gaining the typical unrivalled C++
performance!

We were searching for a way to collect the arguments and pass them further for run-time interpretation.
With other programming languages which provide a superclass \c Object and <b>run-time type information</b>
this is a no-brainer. In C++ it needs some effort to achieve this. This library provides a very
generalized, extensible approach that is not at all limited to variadic function arguments.

The prerequisites needed to reach the original goal were much more than we first expected, and in fact,
only chapter \ref alib_boxing_boxes "11. Variadic Function Arguments and Class TBoxes" presents
the solution for this.


\I{################################################################################################}
## 1.1 Abstract ## {#alib_boxing_intro_abstract}

This module, \alib_boxing_nl, provides means to use C++ run-time type information in
a most easy fashion.

For this, any C++ type, from fundamental "scalar" types to complex composite custom classes, can be
assigned to an object of type \alib{boxing;Box}.
With the assignment, besides the object's value or a pointer to it, <em>"run-time type information"</em>
is stored.
The so-called "Boxes", including their content, can be passed to functions as arguments, returned
by functions or stored for later use. Finally, the contents can of course be unboxed in a type-safe
fashion.

The seamless way of how \alib boxes are usable, is achieved using
\ref alib_manual_appendix_tca "type traits" and quite complex C++20 Concepts, which in earlier
versions was implemented with template meta programming.

\note
  While the default behavior handles almost all custom type properly, the two necessary conversions
  which are called "boxing" and "unboxing", can be customized.

The concept of "boxing" is available in many programming languages and often even done in an
inherent, hidden fashion (then sometimes called "auto-boxing").

\note
  For example, in programming languages \e Java and \e C# (that in this case share exactly the same syntax)
  the following simple line of code perform "inherent auto-boxing":

        int    i  = 5; // No boxing, as simple "value-type" int is used.
        Object box= 6; // Auto-boxing: Creation of a container-object that includes run-time type-information.


Starting with version C++17, the standard C++ library provides type \c std::any, which implements
a similar concept. The differences between class \b %Box introduced by this \alibmod and
class \c std::any will be examined in detail in this Programmer's Manual.

As a quick summary and motivation, in short, the differences are:
1. <b>Bijective Type Mapping</b><br>
   Different types may be boxed to the same target type. Unboxing may be restricted to a subset of
   the originating types. The advantage of this approach lies in a tremendous reduction of
   type-checking when processing boxes.
2. <b>Automatic Pointer-Boxing</b><br>
   Types that by their size do not "fit" into a box (which is true for most structs and classes)
   are be default boxed as pointers. In contrast to this, std::any allocates memory and copies such
   types.
3. <b>Virtual (Box-)Function Calls</b><br>
   This library allows defining functions that can be invoked on boxes. Depending on the type
   stored in the box, a custom implementation of the function is chosen. This is done efficiently
   using "v-tables", just like the C++ compiler does with virtual types.<br>
   This concept not only avoids unboxing and reduces "type guessing", but enables the handling of
   third-party types in otherwise closed libraries.
4. <b>Array Boxing</b><br>
   Class \b Box allows boxing single-dimensional array types.
   While causing a small memory/performance penalty, this feature provides huge benefits for example
   for the frequent use case of boxing any sort of character strings.
5. <b>Constexpr Boxes</b><br>
   Class \b Box in general is a "C++ literal type", hence allows \c constexpr definition of
   its instances.
6. <b>Performance</b><br>
   In many cases, class \b Box outperforms class \c std::any, due to
   - the usually reduced number of destination types,
   - the availability of box-functions, and
   - the fact that no deep copy of boxed values is performed.
   - no destruction mechanics are performed with boxes and their stored data.

   The only performance penality we can think of is that the footprint of class \b Box is 50%
   higher than that of common implementations of class \c std::any.
   (While such implementation details of \c std::any are not specified.)
7. <b>Ease Of Use</b><br>
   We furthermore think that the use of class \b Box is much easier than that of \c std::any.

One drawback is: Class \alib{boxing;Box} does not free the user of life-cycle considerations.
Classes with a footprint not fitting into class \b Box, are, by default, boxed as pointers to
the source value. Such source values have to survive the \b Box instance.
We believe that in 99% of all use-cases such survival is guaranteed "by nature".
This is why class \b Box has no mechanism to deallocate or otherwise destruct boxed data,
in the moment a \b Box goes out of scope.

\note
  By no means, the authors of this \alibmod_nl or this manual want to give the impression that the
  comparison to \c std::any is about indicating a "superiority" of the \alib concept over that of
  the standard library. In contrast, we want to clearly state that the standard library just follows
  different design goals: It is rightfully very abstract and provides an approach of completeness in
  a mathematical and procedural sense.<br>
  
\note
  Comparisons to \c std::any are made because, C++ programmers usually know about it and thus,
  it is important and very helpful when we hint to the differences.
  In general, humans are good in learning new things, through comparison with existing knowledge.


\I{################################################################################################}
## 1.2 Module Dependencies ## {#alib_boxing_intro_dependecies}
This \alibmod is located at a quite low level of the \ref alib_manual_modules_graph and hence can
be extracted and compiled with a surprisingly small fraction of the overall library source.
For the convenience of the authors, the samples in this manual rely on
(and therefore probably compile only with) the full \alibbuild.

However, several sections of this manual give detail on the
\ref alib_manual_alib_build "optional module dependencies" and the according features of \alib,
which leverage this module.


\I{################################################################################################}
## 1.3 How To Read This Documentation ## {#alib_boxing_intro_howtoread}

This documentation mixes tutorial sections and such that provide in-depth information.
The tutorial chapters use the word "tutorial" in their headline and are usually followed by in-depth
information.

In addition, some detailed topics are explained with the reference documentation of corresponding
library types. If so, this manual will note the reader and offer deep links into the reference guide.

We hope that with this structure, experienced C++ programmers will be able to quickly grasp
what they need, while less experienced ones get all information needed to fully understand all
pros and cons of (using) this library.<br>

While this manual is very detailed and quite lengthy, the good news is that it addresses programmers
that include this module into own code only. If software offers an API interface that
accepts class \b Box as function arguments, the user of that interface does not need to know
much about \alib_boxing_nl. Only, if she wishes to in turn implement box-functions
for her types or to start customizing boxing of those, then some deeper understanding is necessary.


\I{################################################################################################}
\I{##############################         Basics          #######################################  }
\I{################################################################################################}
# 2.0 The Basics: Boxing, Type Checks and Unboxing # {#alib_boxing_basics}

\I{############################################################################################## }
## 2.1 Tutorial: Boxing Values ## {#alib_boxing_basicstut_boxing}
Let us now quickly jump into code and have a look at a "hello world" sample:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_MAIN

\note
  This manual will seldom show the inclusion of necessary header-files and "bootstrapping" of \alib.<br>
  Manual chapter \ref alib_boxing_more_bootstrap will give details on what is needed.

Compiling and running this program, the output is:

\verbinclude "DOX_BOXING_TUT_MAIN.txt"

The central type of this module is class \alib{boxing;Box}, located in this module's
namespace #alib::boxing.
As done with most \alib classes, it has an \ref alib_manual_appendix_typealiases "alias name"
defined in namespace #alib, hence the shortcut #alib::Box can be used.
Now, as the sample states

       using namespace alib;

just "Box" becomes sufficient.

The act of "emplacing a value in an instance of class Box" is called \e "boxing".
The sample above shows how such \e "boxing" is performed: It is obviously done "inherently" with the
simple C++ assignment operator.
We can assign <b>just anything</b> to our "box" without getting compiler errors:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_MAIN2

Compiling and running this program, the output is:

\verbinclude "DOX_BOXING_TUT_MAIN2.txt"


For programmers who know C++17 type \c std::any already, this is not too surprising.
The pure C++ language standards do not suggest such code, because C++ is a strongly type-safe
language!

Besides with assignments, this mechanism of "auto-boxing" works well with function calls.
C++ allows exactly one implicit type conversion, if a function argument is defined as a
constant reference type:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_FUNCCALL

The function can be invoked with any argument. Therefore, the following invocations:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_FUNCCALL_CALL

produce this output:

\snippet "DOX_BOXING_TUT_FUNCALL.txt"     OUTPUT

The "opposite", namely returning boxes is comparably simple. A function with a return type of
class \b %Box (here a value type!), can return any C++ type:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_FUNCRESULT

The following sample and output combines the two functions.
We repeat the nested call several times to get a random result:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_FUNCCALLRESULT

\snippet "DOX_BOXING_TUT_FUNCRESULT.txt"     OUTPUT


\I{############################################################################################## }
## 2.2 Tutorial: Type Detection ## {#alib_boxing_basicstut_typechecks}

In the samples of the previous sections, values have been \e boxed and the boxes then have been
streamed into \c std::cout.
The overloaded streaming <c>operator <<</c>, that accepts type \b Box, was provided with the
inclusion of header \implude{Strings.StdIOStream}.<br>
This operator is able to unbox values and print their contents to the stream.

\note
  The full truth is that the operator code itself, does not unbox. Magically, the operator
  can perform its task without "knowing" how to unbox different types. Instead, it defers unboxing
  to another instance. It will be explained in a later chapter, how this operator is implemented.

Before we start unboxing values from boxes, we first need to demonstrate how the type
of a box can be detected. The reason for this is simple: Unboxing a wrong type is forbidden
and considered a severe error.

We cannot simply unbox a value if the type that was boxed is not known.
Instead, unfortunately <b>type detection is a game of guessing!</b>.
For making a guess, templated method \alib{boxing;Box::IsType} exists. This method has no
arguments, but expects the type to "guess" as a template parameter.
As the method's name suggests, the return value is boolean:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_ISTYPE

The output is:
\verbinclude "DOX_BOXING_TUT_ISTYPE.txt"

For the time being, this is all we need to know to proceed with unboxing.


\I{############################################################################################## }
## 2.3 Tutorial: Unboxing ## {#alib_boxing_basicstut_unboxing}

Likewise method \b IsType, introduced in the previous chapter (and likewise the overloaded
constructors of class \b Box), method \alib{boxing;Box::Unbox} used for unboxing a value is
templated.

The template type determines the type of value to be unboxed:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_UNBOX

The output of this code snippet is:
\snippet "DOX_BOXING_TUT_UNBOX.txt"     OUTPUT

This was rather simple! We boxed a \c double value and also unboxed one. So what happens if
we unboxed a different type? This code does this:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_UNBOX2

The bad news is: this code compiles well! This means, the error in the code is not detected by
the compiler.
Unfortunately, the malformed code is detected only at run-time.
In debug-compilations of \alib, an assertion would be raised, with a message similar to

        Cannot unbox type <long> from boxed type <double>.

Even worse, in release compilations of \alib, running such code results in "undefined behavior",
which is the nice wording for "this software will probably crash very soon!".

\note
  This simple sample shows the biggest pitfall when using module \alib_boxing_nl.
  The problem behind this is a very general topic of computer language theory:
  Computer languages may be rather type-safe or less type-safe.
  For example, many scripting languages are not much type-safe.
  Here, bugs in the code are exposed often only when actually running (testing) the code. With strongly
  type-safe languages like C++, many types of malformed code are detected already by the compiler.
  Both approaches have pros and cons and both have a good right for existence.

\note
  While we just named this behavior a "pitfall", on the other hand it could be said that this
  is exactly what module \alib_boxing_nl is all about: transfer type checking from
  compile-time to run-time to enable type-agnostic coding.<br>
  With C++, generic programming is usually performed using templates. However, later in this manual we
  will see use cases, that show when boxing and run-time type detection is just superior in respect
  to code design, code size and even sometimes in respect to execution performance.<br>
  At the end of the day, these benefits are probably why type \c std::any was included in the
  standard library with C++17 and \alib_boxing_nl provides a little more.

The two recent code samples, one that rightfully unboxes a \c double and the other that
asserts at run-time, make little sense. An obvious use case for \alib_boxing_nl
is given, when the acts of \e boxing and \e unboxing are decoupled. So let's look at how
type-safe unboxing is performed in a function that accepts a \b Box.<br>
Function \b ProcessBox tests the given box for "known" types, unboxes values and displays them.
For unknown types, a warning is written and \c false is returned:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_UNBOX3

These sample invocations:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_UNBOX3_INVOKE

produce the following output:

\snippet "DOX_BOXING_TUT_UNBOX3.txt"     OUTPUT

Using the "type guessing" method \alib{boxing;Box::IsType}, introduced in the previous chapter,
this code is back to be fully type-safe. Nothing can crash at run-time. Of course, code that
invokes function \b ProcessBox needs to check the return value at (again run-time) and react
properly if the box type was not "known" and \c false was returned.

There are two drawbacks, one minor and a real major one. The minor is that in the case that
many different known types are to be processed, the execution performance of \c ProcessBox
be weak. A first help would be to sort the guesses and put the more frequent types to the top.
Using the much more performant \c switch statement is not possible, because type information is
no constant data.

The eventually much worse drawback lies in the fixed set of types that a function can process
if it is designed based on "guessing" like sampled here. While in a closed source unit,
this might be not a problem, imagine that function \c ProcessBox resides in an external class library,
where it cannot be extended.
In this case, the function cannot be used for custom types that are not known to the library.

For both problems, module \alib_boxing_nl provides a solution, which is introduced
in a later chapter \ref alib_boxing_functions "8. Box-Function Calls".

\note
  While what we have seen so far could be implemented with C++17 type \c std::any in a similar,
  fashion, a solution for the two drawbacks named is not offered by \c std::any .

\I{############################################################################################## }
## 2.4 How The Basics Work ## {#alib_boxing_basics_expl}

The previous tutorial sections showcased \b boxing, \b unboxing and <b>type guessing</b>.
We will see that for all three aspects, a lot more has to be said and showcased.
While this chapter for this reason cannot go much into technical details, yet, some important
facts can be named and explained already.

\I{############################################################################################## }
### 2.4.1 Templated Approach ### {#alib_boxing_basics_expl_templated}
Class \b Box provides templated method \alib{boxing;Box::IsType} and \alib{boxing;Box::Unbox}
to guess and unbox specific types of and from a box. The types in question are provided with the
template parameter. Likewise, the constructor, which is also used by the copy-assign \c operator=
of that class, uses templates. Otherwise, the straightforward assignment of any object to a
box was not possible.
                  
Besides using templates for "generic programming", a programming paradigm quickly described in
chapter \ref alib_manual_appendix_tca of the general \alib manual is used.

\alib_boxing_nl makes quite a lot of use of "type traits" and hence the whole module can
be easily considered as based on what was formerly called "template meta programming" and
with language version C++20 received a quite proper syntax.
To understand the library code, a solid knowledge of this paradigm is therefore needed. However,
for using the library, fortunately it is not.

\I{############################################################################################## }
### 2.4.2 Boxing And Unboxing ### {#alib_boxing_basics_expl_boxunbox}
Class \b Box contains a data segment, aka an internal piece of memory, that can hold
a certain amount of bytes to store values in.
With each type given, one of a set of constructors selected via keyword \c required is activated.
The constructors copy the source object into this generic piece of computer memory.

With unboxing, according to the requested type the contrary operation is performed: the internal
data stored in the box is re-interpreted back to the original type.

In most cases both actions result in a basic (efficient) copy operation of a (probably)
64-bit value. While the code that is invoked may look longer and complicated and even
function calls to other code entities may be made, the C++ compiler ensures that the generated
assembly code is short and efficient for both, boxing and unboxing.

\note
  Readers that do not believe that, should debug into some methods of class \c std::vector and
  wonder what is going on there and how this class can be so fast while the debugger
  shows plenty of invocations for even the simplest action.
  Most of these invocations seen in a debugger are 100% optimized out by the compiler.
  This is the same for a lot of code found in this module.


\I{############################################################################################## }
### 2.4.3 Type Guessing ### {#alib_boxing_basics_expl_typeinfo}
In addition to the boxed data, class \b Box stores type information. Otherwise, method
\alib{boxing;Box::IsType} could obviously not be implemented.
In C++, type information is received with operator keyword \b typeid. While using standard function
call syntax (round braces), it takes a C++ type as an argument. Returned is a constant reference to
struct \c std::type_info.
The struct does not offer too much functionality, in fact the only useful thing that can be done with
it is to compare it to another reference received with another use of keyword \b typeid.
This way, it can be determined if two types are the same or not.

With that, the type guessing can be performed: Consider a reference to struct \c type_info being
stored with the constructor of class \b Box along with the boxed value data.
As already mentioned, the set of constructors are templated, and the type information is generated
at compile-time.<br>
Likewise templated method \b IsType compares the stored type with the type that its template
parameter denotes at compile-time!

These mechanics explain why types can only be "guessed"!

\note
  What we mean with "guessing" here is can be explained by comparing a C++ <c>switch</c>-statement
  with an alternative version that uses a series of <c>if</c>-clauses instead.
  While both versions may compute the same result, the <c>switch</c>-statement instantly jumps
  to the right "case" and thus is more performant.
  Now, the use of keyword \b switch is restricted to certain constant cases, and especially can
  not be used to "switch over" the type of an \alib %Box.
    

\I{################################################################################################}
\I{##############################       3. Non-Bijective      ###################################  }
\I{################################################################################################}
# 3. Non-Bijective Type Relationships # {#alib_boxing_bijective}

\I{################################################################################################}
## 3.1 Type Relationships ## {#alib_boxing_bijective_intro}

The term \https{bijective,en.wikipedia.org/wiki/Bijection} is used for describing the
relationship between elements of two sets. Bijective relations, mean that each element of set \b A
corresponds to exactly one element of set \b B and vice versa.

The two sets we are looking at in this case is the set of boxable types and the set of resulting
types found in boxes created from the boxable types. This manual calls the latter set
"boxed types" or "mapped types". Both terms mean the same.

In the case of C++ type \c std::any, the relationship between these two sets
is bijective - just as a programmer should expect! It is a  simple, straight-forward one to one
relationship: The type you store in an \c std::any object, is exactly the type that you can get back
from it.

To investigate into the type relationship of \alib_boxing_nl, let us continue with an easy
tutorial sample.

\I{############################################################################################## }
## 3.2 Tutorial: A Reduced Set Of Types To Test ## {#alib_boxing_bijective_tut1}

In previous chapter \ref alib_boxing_basicstut_unboxing, the following simple function
\b ProcessBox was introduced:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_UNBOX3

It was shown, that if invoked with a C++ string literal, a due warning about an unknown type was
written.

Now, have a look at the following sample invocations:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_UNBOX3_BIJECTIVE

You should be quite surprised about the following output:

\snippet "DOX_BOXING_TUT_UNBOX3_BIJECTIVE.txt"     OUTPUT

While only \b two boxed (target) types are tested by function \b ProcessBox, namely
\ref alib::integer and \c double, a variety of \b six types can be passed to the function.
Obviously, different signed integral types are all "mapped" to the same destination type and
the two floating point types \c float and \c double are both mapped to type \c double.

Any programmer can easily see the benefit: with just two code blocks that perform "type guessing"
all relevant boxable types can be processed. The term "relevant" can be very rightfully used: In the integral
case even the C++ compiler itself would allow an automatic, inherent type conversion (cast) with
assignments between the types in question. Not even with the toughest set of warning options,
the compiler would complain.<br>
Ok, in the floating point case, the compiler would warn like this:

        implicit conversion increases floating-point precision: 'float' to 'double'

if no \c static_cast<double>() was applied in to the \c float value. This is because of the fact
that the \c float to \c double conversion is not free of precision loss.

\note
  Later in this manual two things will be discussed:
  - Why a precision loss is not a problem with the common use-cases of \alib_boxing_nl.
  - How - in the case of an uncommon use-case - the \c float to \c double conversion can be suppressed.

Here, we quickly interrupt this tutorial an continue with a manual documentation.

\I{################################################################################################}
## 3.3 Non-Injective Type Mapping ## {#alib_boxing_bijective_noninjective}

The relationship between C++ types and resulting mapped types is not \e injective.
This means, two different C++ types may result in the same boxed type.
For example, by default, all signed integral types (of different byte width) are boxed
as the same type \ref alib::integer, which is just an alias to the "biggest natural integral type"
of the compilation platform. (In short, type \b alib::integer aliases \c std::ptrdiff_t).<br>
Likewise, all unsigned integral types are boxed to type \ref alib::uinteger, which is
an alias to \c std::size_t.

This relationship of boxing C++ fundamental types, is the built-in default behavior.
As such, it can be modified. This leads us to general important statement:

\par
  <b>The process of boxing and unboxing can be manipulated per C++ type.
  For various fundamental and non-fundamental C++ types, such customization of boxing
  exists, which leads to non-bijective type mappings.<br>
  Built-in customizations can be disabled.</b>

The details of how boxing can be customized for a type can only be explained
\ref alib_boxing_customizing "in a later chapter", when other prerequisites are made.

We have learned that \alib_boxing_nl is not \e injective. The next question
is whether it is at least \e surjective. If it was, all types that can be boxed, can also
be unboxed.

As a sample, the question is: Can type \c int16_t be unboxed, regardless of the fact that
it is possible to unbox type \b integer from a boxed <c>int16_t</c>?

Again, a tutorial section should investigate into this question.

\I{################################################################################################}
## 3.4 Tutorial: Unboxing Non-Injective Types ## {#alib_boxing_bijective_nonsurjective_tut}

We have seen so far, that
- type \b integer can be unboxed from any boxable signed integral type,
- type \b uinteger can be unboxed from any boxable unsigned integral type and
- type \b double can be unboxed from boxable types \c float and \c double.

The benefit from this is that only a reduced set of types have to be "guessed" when processing
boxes.

Let us still try to unbox the original type:
\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_SURJECTIVE_COMPILE_ERROR

This code does not even compile! In the compiler's output, the following error is found,
hinting to the third line of the snippet:

    static_assert failed due to requirement 'CustomBoxingRule7'
        Customized boxing forbids unboxing this value type: 'BoxTraits<T>::Read' returns a different type.


This seems surprising in two ways. Not only that this type can't be unboxed, but also that this is
not a run-time assertion but caused by a C++ \c static_assert which is a compile-time message.
As the message's text elaborates, it is just not possible to unbox type \c int16_t - no matter
what was previously stored in the box.
Furthermore we understand: This was explicitly forbidden, which means "voluntarily" in this case.<br>

What we have here is a <b>design decision</b> of this \alibmod_nl. Technically, it would be easy
to allow unboxing that type. All that is needed is a static type cast, which by the way can
be performed by the programmer easily herself if needed:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_SURJECTIVE_STATIC_CAST

The point here is that with the standard use-cases of \alib_boxing_nl, the width of an
integral is seldom of any interest. It is just enough to know that an integral value was given,
no matter what size it was. Now, to prevent to accidentally start \e guessing types that
belongs to a group of types that are "aggregated" to one destination type,
the built-in customization of these types are explicitly forbidding that.

Why "accidentally"? Well, in respect to previous sample function \b ProcessBox, testing for all
sorts of integrals would be just redundant code. For the same reason, to perform type guessing on
an not-unboxable type, is already illegal. This code:
\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_SURJECTIVE_COMPILE_ERROR2
produces the very same compilation error as the one above that tries to unbox the type.

\note This design decision is only effective with the library defaults. Chapter
      \ref alib_boxing_fundamentals "4. Boxing Fundamental Types" will show how this behavior
      can be changed.

To conclude this tutorial section, an next important observation has to be made.
For this, let us look at the following code snippet:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_TUT_SURJECTIVE_STRINGTYPES

This sample shows that \alib string-types can be unboxed from a box that previously got a
\c std::string_view assigned and vice versa. Each original type can also be unboxed and
type guessing for both types returns \c true.

The takeaway from this is: Just from the fact that a type \b B is unboxable from type \b A,
it cannot be concluded  that the original type \b A is not unboxable. While this is
true for types \c int16_t and \b integer, this is not true for types \c std::string and \alib strings.

\note
  With \ref alib_boxing_strings "10. Boxing Character Strings", this manual later dedicates a whole
  chapter on using character strings with \alib_boxing_nl.


\I{################################################################################################}
## 3.5 Non-Surjective Type Mapping ## {#alib_boxing_bijective_nonsurjective}
In the first sections of this chapter, it was explained that type mapping is not \e injective.
This means that different source types can result in the same boxed type.

Now, with the latest tutorial section, it was demonstrated that some boxable types cannot
be unboxed. For these types, this manual uses the term "not unboxable types" or "locked types".

Conceptually this means that \alib_boxing_nl is also not \e surjective:
Not all origin types are "found" in the destination type set.

A relation is \e bijective if it is both \e injective and \e surjective.
Consequently it is not bijective if it is either not \e injective or \e not surjective.
Unfortunately, no word exists for the condition \"not \e injective \b and \e not surjective\".
Therefore, this manual uses "not bijective" and this is meant in the broadest sense.



\I{################################################################################################}
## 3.6 Summary And Rationals ## {#alib_boxing_bijective_rationals}

A quick summary of what was said in this chapter should be given in bullets:
- While the type conversion of C++17 type \c std::any is \e bijective, a huge difference of
  \alib_boxing_nl is that its type relationship is not \e bijective, precisely it is
  neither \b injective nor \b surjective.
- With the reduction of possible target types, less type guessing has to be performed
  when processing boxes.
- While usually any type can be boxed, unboxing of certain types may be forbidden by
  compile-time assertions. In this case, usually the target type of the boxing conversion can
  be unboxed. Besides that, also other types might be available for unboxing.
- The exact same static assertions given with method \alib{boxing;Box::Unbox} are
  applied with method \alib{boxing;Box::IsType}. This means, if a type must not be unboxed,
  it must not even be "guessed".
- Apart from one important exception (which is only explained later), by default
  a \e bijective type relationship is established, likewise class \c std::any suggests.<br>
- Only if the boxing becomes customized (explained later), \e non-bijective boxing is activated.
  This is not supported by \c std::any.
- Such customization is already built into the library with its default compilation settings,
  but it can be disabled (explained later).

<b>Some rationals why non-bijective type mapping is even defaulted in the library:</b><br>
The approach taken with non-bijective type mapping, of course also has obvious disadvantages.
First of all, type information is just lost: When detecting an \b integer type stored in a box,
the processing code cannot perform different actions depending on the width of the given
original integral type. The information on the size is just lost.
Even worse, in the case of floating point values, the inherent conversion of values of type \c float
to those of type \c double, even include a loss of the precision of the value.

So why does \alib_boxing_nl take these restriction into account by default? Why is the benefit of
just having to cope with a shorter set of target types, weighted a higher gain than the loss
in precision?<br>
This can be answered only by looking at the use cases of boxing. Remember that C++ until its
version 17, not even suggested to do something like boxing. Instead, the language is known for its
type safeness and its close binding to the underlying hardware, where the difference between
\c int16_t and \c int32_t is considered a very huge one.<br>
So this answer is rather, that boxing is not used in these areas of software that contributed
to the overall decision to use C++ as the source language. Instead, the use cases are rather
found where more relaxed demands are applicable - and these can be parts of the same software.
Take, for example, software that calculates tomorrows weather forecast:
A C++ software would be able to process billions of calculations,
or at least feed corresponding dedicated "number crunchers" with the input data and process the
result. For this task, the data should never be boxed and transported in a generic way.
This is absolutely no use case for boxing! However, the very same software would also write
a log file or display some messages on the console. Here, even the most valuable final results,
namely tomorrows average temperature and wind speed may be intermediately converted to a boxed value:
When unboxed, the first fractional digits of the floating point value will still be intact and
precise enough to displayed to a homo sapiens.

Consequently, a rather "convenient" formatting function is needed, as known from \c printf (which is
not type-safe and therefore a "no-go") or from the standard libraries of various different programming
languages. It can be noted that neither the complex syntax options of format strings introduced
by the \b Python language (using brackets <c>"{}"</c> as placeholders) nor those introduced by the
\b Java language (using <c>"%"</c> as placeholder and extending the good old \c printf format) provide
any means to distinguish 16- from 32-bit integrals. While the output can be altered in various ways, the
originating type is just irrelevant.

This is important to understand: The use of \alib_boxing_nl has to be justified.
It is not just to be seen as a convenience library that enables easy, generic coding.

Sometimes however, as we will see in later chapters and also in the appendix chapters,
\alib_boxing_nl solves a real problem that arises from the nature of the C++ language,
which otherwise can be solved only with \c std::any or using bare keyword \c typeid directly.
But even in these cases, \e bijective boxing remains the default.


\I{################################################################################################}
\I{##########################      4. Boxing Fundamental Types    ###############################  }
\I{################################################################################################}
# 4. Boxing Fundamental Types # {#alib_boxing_fundamentals}

\I{################################################################################################}
## 4.1 Definition Of Fundamental Types ## {#alib_boxing_fundamentals_def}

so-called "Fundamental C++ types" are \https{specified by the C++ language,de.cppreference.com/w/cpp/language/types}.

In short, those are all types that can be defined using a <b>valid combination</b> of
the type keywords
<c>bool, int, long, int16_t, int32_t, float, double, char, wchar_t, char16_t</c> and <c>char32_t</c>
as well as modifier keywords
<c>signed, unsigned, short</c> and <c>long</c>.


\I{################################################################################################}
## 4.2 Default Boxing On 64-Bit Systems ## {#alib_boxing_fundamentals_64bit}

The following defaults are set if \alib is compiled on a 64-bit compiler/platform
(precisely one where \c std::size_t has a width of 64 bits).

The subset of fundamental exceeding a size of 64 bits are always boxed in a bijective way, which means in
a one to one relationship. Those are:

- Integrals larger than 64-Bit (platform/compiler dependent, e.g., with GCC type <c>__uint128_t</c> is concerned).
- <c>long double</c> (a floating point value usually larger than 64 bits)

Furthermore, character types (<c>char</c>, <c>wchar_t</c>, <c>char16_t</c> and <c>char32_t</c>)
are (by default) boxed non-bijective: Characters of any type are converted to type #alib::wchar
when boxed.

All remaining fundamentals by default are boxed in an \e injective way. By that, they can be grouped
into three different sets:

1. All signed integrals up to a maximum of 64 bits length, will be boxed to type \ref alib::integer.
2. All unsigned integrals up to a maximum of 64 bits length, will be boxed to type \ref alib::uinteger.
3. Types \c float and \c double will be boxed to type \c double.
4. Character types <c>char</c>, <c>wchar_t</c>, <c>char16_t</c> and <c>char32_t</c>
   will be boxed to \ref alib::wchar.

Only the destination type of each group is allowed to be guessed and unboxed.

\I{################################################################################################}
## 4.3 Default Boxing On 32-Bit Systems ## {#alib_boxing_fundamentals_32bit}

The following defaults are set if \alib is compiled on a 32-bit compiler/platform
(precisely one where \c std::size_t has a width of 32 bits).

Integrals of a size of 64 bits are boxed in a bijective way, which means in a one to one relationship.

\attention
  Integrals larger than 64 bits, as well as type <c>long double</c>
  (if even available on a 32-bit platform), by default cannot be boxed at all!


All remaining fundamentals by default are boxed in an \e injective way.
By that, they can be grouped into \b four different sets:

1. All signed integrals up to a maximum of 32 bits length, will be boxed to type \ref alib::integer.
2. All unsigned integrals up to a maximum of 32 bits length, will be boxed to type \ref alib::uinteger.
3. Types \c float and \c double will be boxed to type \c double.
4. Character types <c>char</c>, <c>wchar_t</c>, <c>char16_t</c> and <c>char32_t</c>
   will be boxed to \ref alib::wchar.

Only the destination type of each group is allowed to be guessed and unboxed.


\I{################################################################################################}
## 4.4 Disabling The Default Customized Boxing ## {#alib_boxing_fundamentals_disable}

In the previous two sections, a fourth group of aggregated types was named with character types.
Note, that the non-bijective boxing of character types was not shown in the tutorial.
Destination type \alib{characters;wchar} is defined with dependency module \alib_characters,
which sorts a little of the "mess" a C++ programmer faces when dealing with characters.
\alib_boxing_nl leverages this module here for boxing plain character types.
As we will see later, the benefits of module \alib_characters_nl for boxing are even
much greater.

Three \ref GrpALibPreproSymbols are available, which disable the custom boxing definitions.
- Symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS is used to enable bijective boxing
  of integrals.<br>
- Symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS is used to enable bijective boxing
  of floating point values.<br>
- Symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS is used to enable bijective boxing
  of characters.<br>

The consequences of changing the defaults (enabling bijective behavior) should be obvious.
For example, a processing code may now have to guess different integral types and it can and
it has to unbox and process them separately.

Different code units that use a different setting in respect to one of the three compilation
symbols, <b>must not be mixed</b>. For example, a box created from type \c int16_t in a code
unit that enabled bijective boxing on compilation, cannot be processed by a code unit that
uses default non-bijective boxing enabled.
\ref alib_boxing_bijective_nonsurjective_tut "Remember" that the processing code unit would receive
a compile-time assertion, if it tried to unbox the value.

To write code that is agnostic to the compilation symbols above, the following set of methods
may help:
- \alib{boxing;Box::IsSignedIntegral}
- \alib{boxing;Box::IsUnsignedIntegral}
- \alib{boxing;Box::IsFloatingPoint}
- \alib{boxing;Box::IsCharacter}
- \alib{boxing;Box::UnboxSignedIntegral}
- \alib{boxing;Box::UnboxUnsignedIntegral}
- \alib{boxing;Box::UnboxFloatingPoint}
- \alib{boxing;Box::UnboxCharacter}
 
These methods internally select code depending on the according compiler-symbols.
Some of these methods are just a simple inline alias functions in the non-bijective case,
while otherwise they become true library functions that aggregate all necessary tests and unboxing
operations of the different possible types.

\note
  Any \alibmod_nl that relies on \alib_boxing_nl, for example, modules \alib_format or
  \alib_expressions, uses this symbol to compile and be compatible with any of the selected
  setting.<br>
  However, if the defaults are disabled and then furthermore a custom boxing for fundamental
  types is defined (which is explained in a later chapter), then these \alibmods_nl
  might become incompatible!<br>
  While we do not see a technical solution for this, we as well do not foresee good reasons for
  replacing the built-in non-bijective boxing of fundamental types with an own definition.
  In other words: Disabling the defaults might be justified in rare cases and is supported, but a
  replacement of the non-bijective boxing relationships by a custom one is not.

\see
   Chapter \ref alib_manual_build "6. Building The Library" of the Programmer's Manual of \alib
   for more information on compiling the library and using the compiler-symbols. For example,
   if using \https{CMake,cmake.org/}, corresponding cached \b CMake variables
   \e %ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS, \e %ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS and
   \e %ALIB_FEAT_BOXING_BIJECTIVE_FLOATS are available.


\I{################################################################################################}
\I{##############################      5. Boxing Arrays         #################################  }
\I{################################################################################################}
# 5. Boxing Arrays And Vectors # {#alib_boxing_arrays}

\I{################################################################################################}
## 5.1 Support For 1-Dimensional Arrays ## {#alib_boxing_arrays_intro}
Module \alib_boxing_nl has a built-in support for boxing C++ arrays with one dimension.
With the current proceeding of this manual, it cannot be easily justified and discussed what
the rationale for this support is. Only a bigger picture, looking at prominent use cases
and the "side effects" that this feature enables, allow to give a complete answer to that.

Therefore, let us at this point rather describe what is available and provide more rationals
in later sections.

Type guessing and unboxing for boxed array types slightly differs from those of scalar types.
Method \alib{boxing;Box::IsType} is not applicable to array types. The reason is simply, that
the C++ language does not allow to specify template types to be arrays of arbitrary size.
The template parameter \p{TBoxable} of the method \b IsType might be \c int[3] or
\c double[25], but cannot be just \c int[] or \c double[].

Therefore, alternative method \alib{boxing;Box::IsArrayOf} is provided. For example
\c IsArrayOf<int>() may be used to guess a boxed array of \b int values.

\note
  While \ref alib_boxing_fundamentals_disable "by default", fundamental integral types become boxed
  as type \alib{integer}, the element types of arrays are not (and cannot) be modified.
  For example, an array of type \c int16_t[10], will be boxed as an array of \c int16_t.


Further methods according to array boxing are:
- \alib{boxing;Box::IsArray}
  This is a non-templated method that returns \c true if the box holds an array of any type.

- \alib{boxing;Box::UnboxLength}
  Returns the length of a boxed array.
  The result of the method is well defined for boxed arrays and a due prior test for a boxed
  array type using method \alib{boxing;Box::IsArray} is array.<br>
  However, no compile- or runtime- assertion is made if invoked on non-array boxes. Also, a
  process will not signal any exception. In other words, the invocation on non-array types is
  harmless but the result is undefined.

- \alib{boxing;Box::UnboxArray}
  This templated method returns a pointer to the boxed array. The template type that has to
  be provided is the element type of the array (likewise with method \alib{boxing;Box::IsArrayOf}).

- \alib{boxing;Box::UnboxElement}
  This templated method returns the value of an array element. Besides the template parameter
  specifying element type, the method has one argument denoting the element's index.

With that information, we can do two tutorial sections:

\I{################################################################################################}
## 5.2 Tutorial: Boxing And Processing Arrays ## {#alib_boxing_arrays_tut}

The following simple method prints the contents of boxed \c int and \c double arrays:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_SAMPLE_ARR

Some test invocations:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_SAMPLE_ARR_2

lead to the following result:

\verbinclude "DOX_BOXING_SAMPLE_ARR.txt"

Note the two different ways of implementing the array-loop. For type \c int, each element is
unboxed one by one, which avoids unboxing and locally storing the pointer to the array. However,
for type \c double this effort is made. The element loop itself then runs directly on the array
instead of the box.<br>
In release-compilations, both alternatives should result in the very same object code and thus share the
same runtime performance. In debug-compilations, the first version performs a type check
as well as a bounds check of the given argument in respect to the boxed array's size for each element.
It is depending on the situation, which alternative is to be preferred. In this simple case,
we would choose the second alternative, because neither type nor range checks are necessary
in debug-compilations. Maybe a matter of taste.

Further note the use of method \alib{boxing;Box::TypeID}. In its default implementation,
obviously this method returns the boxed type in case of scalars and the element type in case
of vectors. This decision can be altered, by explicitly providing an otherwise defaulted and
therefore in this sample not visible, template parameter.
Please see the \alib{boxing;Box::TypeID;method's documentation} for further information.

Finally, in debug-compilations - the result of method \b TypeID can just be streamed to \c std::out.
This is very convenient and possible due to some tricks of other \alib modules, which includes the use of type
\ref alib::lang::DbgTypeDemangler.
For technical reasons, type \b DbgTypeDemangler is only available in debug-compilations.
Method \b TypeID itself is available also in release-compilations.


\I{################################################################################################}
## 5.3 Tutorial: Multi-Dimensional C++ Arrays  ## {#alib_boxing_arrays_tutmuldim}
As already mentioned, \alib_boxing_nl does not provide a similar solution for multi-dimensional
arrays. When multi-dimensional arrays are boxed and unboxed, the sizes of the higher
dimensions need to be known. The following quick sample demonstrates this:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_SAMPLE_ARR_3

Output:

\verbinclude "DOX_BOXING_SAMPLE_ARR_3.txt"

While the code above is feasible, multi-dimensional arrays are better boxed when wrapped in a custom
type, e.g., one that stores the sizes explicitly and allows restoring them after unboxing.
We do not consider this being a huge drawback for this module in general, especially in respect
to the use cases of \alib_boxing_nl, which probably seldom include multi-dimensional arrays.

Type \c std::any does not provide any more support, even the size of one-dimensional
arrays is not stored there.

\I{################################################################################################}
## 5.4 Boxing Vector Types ## {#alib_boxing_arrays_vectors}
The term "Vector Types" here means collection type <c>std::vector<T, std::allocator<T>></c> and similar
custom (3rd-party) types that store their elements in a single memory buffer.

\alib_boxing_nl provides built-in support to customize the boxing of class \c std::vector.
\note For built-in support of vector types of other (3rd-party) libraries, checkout
      namespace \ref alib::boxing::compatibility and its inner namespaces.

With the customization, objects of the type \c std::vector are boxed to C++ arrays of the templated
element type. The customization, requests a vector's allocated memory (method \c std::vector::data)
and stores this pointer besides its size.<br>
The advantage of this approach is (as with any non-bijective type mapping) that the code that
processes boxes needs to check for + arrays of a certain element type only.
Separated checks for other vector types are not needed.

Because it was not discussed yet, how custom boxing is performed, for now the important
takeaway is that the \e injective boxing of objects of type \c std::vector to one-dimensional
array types can be enabled per compilation unit, by simply including header-file
\implude{Compatibility.StdBoxtraits}.

If so, we can feed method \b ProcessArray sampled above with objects of type vector:

\snippet "DOX_BOXING.cpp"    DOX_BOXING_SAMPLE_ARR_4
The output will be:
\verbinclude "DOX_BOXING_SAMPLE_ARR_4.txt"

The built-in customization does not allow to unbox type \c std::vector from boxed C++ arrays.
Again this is a design decision, technically this would be possible. The rationale for this is that
unboxing to \c std::vector would impose a memory allocation and a deep copy of the data.

Therefore, such unboxing should be performed only with very explicit code. With the inclusion
of the compatibility header named above, a templated, inline function for this task is
\alib{boxing::compatibility::std;CopyToVector;already provided}. This is its simple source code:

\snippet "ALib.Compatibility.StdBoxtraits.H"   DOX_BOXING_SAMPLE_ARR_UNBOX_VECTOR_IMPLEMENTATION

With that, unboxing a \c std::vector from a boxed C++ array is done as sampled here:
\snippet "DOX_BOXING.cpp"    DOX_BOXING_SAMPLE_ARR_UNBOX_VECTOR


\I{################################################################################################}
## 5.5 Rationale and Technical Background ## {#alib_boxing_arrays_rational}

Technically, C++ arrays are boxed by storing the pointer to the first element together with the
array's length.

While we have not discussed the possibilities of customization of boxing for a certain type, yet,
it can be said here that C++ array types constitute an exception: Their way of boxing is
\b not customizable.
\note
  Again this is due to the fact that the language syntax does not allow a single template parameter
  to denote C++ array types of arbitrary length. As we had seen before,
  it is not even possible to unbox the C++ type. Therefore, the interface of class
  box allows either unboxing a pointer to the start of the array or a single element.

But what is possible, is to customize the boxing of other types (structs and classes) to
result in the same boxed type as if a native C++ array was boxed. This was demonstrated in the
previous tutorial section.
Consequently such types might also well be unboxed from boxes created originally by C++ arrays.

The special treatment of one-dimensional arrays with \alib_boxing_nl imposes advantages and
disadvantages and hence is the result of a design decision.

The disadvantages are:
- The array length needs to be stored with every \b %Box object.
  This "space" for this exists with each and every instance of class \b Box, whether it is used or not.<br>
  In fact, the footprint of type \alib{boxing;Box} on a standard 64-bit machine is 24 bytes
  (three times the size of a pointer). In contrast, the size of type \c std::any is only two thirds
  of that, namely 16 bytes.

- The Complexity of the API and thus its use increases. For example, it was needed to provide extra
  interface methods that enable to check for boxed arrays in general, to guess array types and to
  unbox one-dimensional arrays, respectively single elements of those.

The advantages of the approach taken are:
- One-dimensional arrays can be boxed along with their length in a type-safe way. Precisely spoken,
  \alib_boxing_nl distinguishes between a simple pointer to a type and a one-dimensional
  array of that type.

- The provided customizations of boxing type \c std::vector and optional customizations of boxing
  similar container types (of any arbitrary 3rd-party libraries!) unify the boxed type
  to be a simple C++ array type. With such, the processing code can uniquely handle boxes
  from arbitrary "vector-like" sources.

- The additional storage capacity used to store the length of an array, is available for just
  other use in the case of boxing scalar types.


It will be discussed in \ref alib_boxing_strings "a later chapter", that array-boxing is especially
helpful in the domain of string types: Arbitrary string types can be boxed as nothing else but
simple one-dimensional character arrays. This way, this messy bunch of types, coming from tons of
3rd-party libraries, can all be aggregated to the very same type!

\I{################################################################################################}
## 5.6 Exclamation With Boxing The Length Of Character Arrays ## {#alib_boxing_arrays_of_char}

For one-dimensional character array types \c char[], \c wchar_t[], \c char16_t[] and \c char32_t[]
the selected constructor of class \b Box shortens the stored array length by one.

The rationale for this is that in most cases, boxed character arrays are string literals.
String literals are zero-terminated arrays, hence the following line compiles:

        char string[4]= "123"; // well formed!

while compiling this fails:

        char string[3]= "123"; // Compiler error

With this exception in place, character strings are stored with the "right" size. The term
is justified at the moment that a programmer believes that zero-terminated strings are
not nice. The zero-termination is "forgotten" at that moment. However, the benefit is, that
the length of the box represents the true length of the string given!

This feature cannot be disabled. On the one hand, custom boxing is not available for
C++ character types (for technical reasons, as already mentioned). Also, there is no
\ref GrpALibPreproSymbols "preprocessor symbol" introduced to disable this behavior, as we cannot
consider a use case where this behavior wasn't acceptable. If it was, too many dependent features
of various \alibmods_nl would discontinue working and had to be disabled.


\I{################################################################################################}
\I{############################   6. Boxing Structs And Classes    ##############################  }
\I{################################################################################################}
# 6. Boxing Structs And Classes # {#alib_boxing_classes}

So far in this manual we have only been boxing fundamental C++ types and C++ arrays.
The only exception we saw, was class \c std::vector<T> in the previous chapter. Here, it was only
explained that it uses a customized boxing and nothing was said about how this works.<br>
Still, customization is only explained in the next chapter, because for most custom composite
types, \alib_boxing_nl works very well "out of the box"!

The first thing we do is looking at a few samples.

\I{################################################################################################}
## 6.1 Tutorial: Boxing and Unboxing Custom Classes ## {#alib_boxing_classes_tut}

Let's declare a simple custom class:

\snippet "DOX_BOXING.cpp"    DOX_BOXING_CLASSES_SMALL

With this in place, we can box, guess and unbox an object of that class:

\snippet "DOX_BOXING.cpp"    DOX_BOXING_CLASSES_SMALL_USING

The output of this code sequence will simply be:
\snippet "DOX_BOXING_CLASSES_SMALL_USING.txt"     OUTPUT

While this was very easy and straightforward, here comes <b>the pitfall!</b>
We define a "bigger" class:

\snippet "DOX_BOXING.cpp"    DOX_BOXING_CLASSES_BIG

With that, we use the same code as above:

\snippet "DOX_BOXING.cpp"    DOX_BOXING_CLASSES_BIG_USING

Unfortunately, this code does not compile.
The compiler complains twice, once with call \alib{boxing;Box::IsType;IsType<BigClass>}
and also with \alib{boxing;Box::Unbox;Unbox<BigClass>}.
The error message is as follows:

    static_assert failed due to requirement 'DefaultBoxingRule1'
        This type cannot be unboxed by value: By default, values that do not fit into boxes
        are boxed as pointers.

This tells us that instead of type \c BigClass, type \c BigClass* was boxed! We need to fix
the code above in three places: Twice for providing the pointer type as the template parameter to
methods \c IsType and \c Unbox and also change <c>operator.</c> to <c>operator-></c>, when
invoking method \b Get of the unboxed pointer:

\snippet "DOX_BOXING.cpp"    DOX_BOXING_CLASSES_BIG_USING_FIXED


Now the code compiles and runs fine. Its output is:
\snippet "DOX_BOXING_CLASSES_BIG_USING.txt"     OUTPUT

"Easy!" you could say, because the \c static_assert helps to create this clear compiler message seen
above. Just make it a pointer type, and here we go! Unfortunately, there is a next pitfall related
with switching to pointers, but this is discussed in a next chapter.

Before we elaborate more theory, let's quickly finalize this tutorial part with a last,
probably astonishing thing. From the compiler error message that said
<em>"...values that do not fit into boxes are boxed as pointers"</em> an attentive reader might
get suspicious and wonder: Does this mean that the opposite is also true? Are "fitting objects"
just always boxed as values, even if a \e pointer to a fitting type is boxed?

This code gives the answer, as it compiles and runs well:

\snippet "DOX_BOXING.cpp"    DOX_BOXING_CLASSES_SMALL_AS_POINTER
\snippet "DOX_BOXING_CLASSES_SMALL_AS_POINTER.txt"     OUTPUT

Consequently, trying to unbox a pointer to class \b SmallClass, leads to compiler error:

    static_assert failed due to requirement 'DefaultBoxingRule3'
        This type cannot be unboxed as pointer: Default boxing of types that fit into boxes
        and are copy-constructible and trivially destructible, is performed by value.

\I{################################################################################################}
## 6.2 Value vs. Pointer Boxing ## {#alib_boxing_classes_pointers}

By default, when a composite type (a \c struct or a \c class) is boxed, \alib_boxing_nl
checks whether a value of the given type "fits" into the data segment of class \b Box and from this
decides if the type is boxed as value or as pointer.
In both cases, the chosen type is used, no matter if a pointer to the type a value is passed.
\note
  In the case that value boxing is chosen and \c nullptr is boxed, the internal memory
  of the box (introduced in detail in the next chapter) is set to zero values.

A question now is: what types do fit in? The answer is quite simple. On a 64-bit platform,
class \b Box is ready to store a pointer or any other 64-bit wide argument. In addition to that,
due to the built-in ability of \ref alib_boxing_arrays "boxing one-dimensional C++ arrays", a second
64-bit value can be stored. With C++ array types, this member holds the array's length. With
other types, it is available for free use.<br>
As a result two times 64-bit, hence 16 bytes can be stored. If type \b BigClass from the previous
tutorial section held only two values of type \alib{integer} instead of three, it would fit
in and became boxed as value. Likewise, on a 32-bit platform, the usable value data of class
\b Box is two times 32-bit equalling 8 bytes.

A second constraint that defaults boxing of a type as pointers, is when a type is either not
copy-constructible or not trivially destructible.
Coincidentally, a good sample for such a type is one of the C++ standard
library, that this module heavily uses: \c std::type_info. While on common platforms values
of the type fit nicely into a box, the type is boxed as pointer type, because only references
and pointers of it may exist. The selected constructor of class \b Box detects that and performs
pointer boxing. Trying to unbox that type as value, leads to compiler error:

    static_assert failed due to requirement 'DefaultBoxingRule2'
        This type cannot be unboxed by value: By default, types that are not copy-constructible,
        or not trivially destructible are boxed as pointers."

This design aspect of \alib_boxing_nl might be surprising. In fact it could be legitimately
argued that this behavior is not along the design lines of C++. Consider that the
following to lines of code:

        Box box1=  myValue;
        Box box2= &myValue;

create two boxes with the very same contents! And: without knowing the size of the type, a reader
even cannot tell if both times a pointer is boxed or if the objects are copied by value.

On the positive side of the two lines above is that a programmer does not need to care if a value
or a pointer was passed: things will just be boxed to the right type.
One of the answers why \alib_boxing_nl is allowed here to trade "convenience" against
pure C++ standards, is once more given from the limited set of scenarios where boxing should be used
at all.

This and some other aspects should be discussed in the following few sections.

\I{################################################################################################}
## 6.3 Non-Bijective Boxing - What Is Default And What Not? ## {#alib_boxing_classes_nonbijective}

Value and pointer boxing and its transparent treatment, constitutes a next aspect of
\e non-bijective boxing, that we have already discussed in depth in chapter \ref
alib_boxing_bijective "3. Non-Bijective Type Relationships".
By default, all pairs of type \b T and \b T* are boxed to either one of the two, just depending on
the size of the type. This approach effectively reduces the number of types that need to be
guessed when processing boxes by half.

In the next chapter we will see how boxing can be customized per type. This includes the option
to redefine this automatic default treatment. Arbitrary combinations are possible:
- The destination type of both \b T and \b T* can be "switched" from the one chosen by the
  default rules, to the complementary type.
- Both types can be treated independently, including the definition of different target types
  (\b T &rarr; \b TX and \b T* &rarr; \b TY)
- Unboxing can be forbidden for one of the types, or both.
- Boxing can be forbidden for one of the types, or both.

Between the type mapping seen so far and this mapping of value and pointer types, two
differences exist:
1. The mapping of a type to a completely different type
   (like all signed integer types to type \alib{integer}) needs customizing. In contrast
   The pointer/value mapping is a built-in default and needs customizing to be switched off.
2. As we will see later, often the mapping of a type to a completely different type needs
   some custom "conversion" method. In contrast to this, the compiler can easily perform the
   mapping between values und pointers of the same type autonomously. It is just a matter
   of applying either address operator <c>'&'</c>, or indirection operator <c>'*'</c>.

The latter might be important to understand: The conversion with operators <c>'&'</c> and <c>'*'</c>
is done as the very first step. It could be said, that in fact the complement type is boxed instead
of boxing the given type itself.

\I{################################################################################################}
## 6.4 Pointers To Fundamental Types ## {#alib_boxing_classes_pointers_fundamentals}

In previous chapter \ref alib_boxing_fundamentals "4. Boxing Fundamental Types", nothing was
said about boxing pointers to fundamental types. But this was only to avoid confusion at that
point in time! Instead, it was explained was that the non-bijective boxing groups all fundamental
types into four sets:

1. Signed integral types that get boxed to type \alib{integer}
2. Unsigned integral types that get boxed to type \alib{uinteger}
3. Types \b float and \b double that get boxed to type \c double.
4. Character types <c>char</c>, <c>wchar_t</c>, <c>char16_t</c> and <c>char32_t</c>
   will be boxed to \ref alib::wchar.

Now, pointers to all fundamental types are boxed like their value counterpart.
Likewise with structs and classes, the two boxes from the following sample:

        int i= 42;
        Box box1=  i;
        Box box2= &i;

receive the identical contents of type \b integer and value 42.

\note
  The only exception are constant pointers to character types, for example, <c>const char*</c>
  or <c>const alib::character*</c>.
  These are considered zero-terminated strings and are boxed to C++ array types.
  A rationale for, and all details on this exception will be given in chapter
  \ref alib_boxing_strings "10. Boxing Character Strings".

\I{################################################################################################}
## 6.5 Constant vs. Mutable Box Contents ## {#alib_boxing_classes_constant}

A next \e non-bijective behavior of \alib_boxing_nl is constituted by following boxing
rules:

If \b T is a non-constant value type, then:

1. If value boxing applies for \b T, then types \b T and <b>const T</b> are both boxed as <b>T</b>.
2. If pointer boxing applies for \b T, then types <b>T*</b> and <b>const T*</b> are both boxed
   as <b>const T*</b>.

The same two rules can be phrased from the perspective of the boxed types as follows:

1. Mapped value types are unboxed as non-constant values, regardless if a constant or non-constant
   pointer or value was boxed.
2. Mapped pointer types are unboxed as constant pointers, regardless if a constant or non-constant
   pointer or value was boxed.

The rationals for this are:

1. Value boxing copies the object and thus can always returns a non-constant copy.
   This reduces the size of the set of mapped value types by half, as it is irrelevant whether a
   constant or mutable object was boxed.
2. Pointer boxing copies the pointer. To reduce the size of the set of mapped pointer types by
   half, \alib_boxing_nl volunteers to always treat pointers to boxed objects as pointers to
   constant instances, even if a mutable object was boxed.

This all means that the information about whether a type was constant or mutable is lost with
boxing it. Only when a processing code is "sure" that a boxed pointer points to a mutable object
it might apply a \c static_cast on the result of method \b Unbox if it intents to perform
modifications. Furthermore, for convenience, method \alib{boxing;Box::UnboxMutable} is available,
which just calls \b Unbox() and performs the <c>const_cast</c> to return a mutable result.

Finally, it is important to understand that although types that are boxed as pointers are always
treated as pointers to constant objects, this is never noted anywhere.
For example, template parameters of method \alib{boxing;Box::IsType} and \alib{boxing;Box::Unbox}
expect a non-const type for value- and pointer-types.<br>
The rationale for this is: Because all pointer types are returned as pointers to constant objects,
a need to pass keyword \c const with pointer types was redundant.<br>
The following code snippet should make this clear:

\snippet "DOX_BOXING.cpp"    DOX_BOXING_CLASSES_CONST_POINTERS

While \b BigClass is unboxed as <c>const BigClass*</c>, the template parameter just says <b>\<BigClass*\></b>.

A corresponding static assertion will fail, if keyword \c const is used with type specifications.


\I{################################################################################################}
## 6.6 Boxing Volatile Types ## {#alib_boxing_classes_volatile}

For types that are boxed as values, type attribute \c volatile is removed from the copy.
\note
  It is in the user's responsibility to decide if it is a good idea to copy a volatile object
  to a box.

Volatile objects of types that are boxed as pointers, are not allowed to be boxed.
If tried, compile-time assertion:

    DefaultBoxingRule4
        Types boxed as pointers cannot be boxed if volatile.

will be given.

Methods \alib{boxing;Box::IsType} and \alib{boxing;Box::Unbox} will statically assert
if type specifier \c volatile was given with template parameter \p{TUnboxable}.

\I{################################################################################################}
## 6.6 Life-Cycle Constraints Of Boxes And Their Contents  ## {#alib_boxing_classes_lifecycle}

In the case of value boxing, performed for fundamental types and such composite types that
"fit into" a box, a all necessary data is \b copied into the box. Therefore, the
life-cycle of the box instance is independent of the source value.

This is different when pointers are boxed. Here, the exact same rules as using normal
pointers apply: A pointer must be dereferenced only if the objects it points to is
still valid.

Now one could argue that this becomes a little "delicate" at the moment a programmer does not know
if a type is boxed by pointer or by value. Maybe she would think that an object just fits and
therefore delete the source type after boxing, which of course leads to undefined behavior if
the type didn't fit!

The simple solution to this is: When an object of a composite type (struct or class) is boxed, the box
just always should be considered to have a life-cycle bound to the object, regardless if by
coincidence the value fits to the box and is thus copied. A programmer should just volunteer
to take this little chance of her worries being unnecessary into account.<br>

However, some thinking has always to be given. For example, reconsider how class \b std::vector<T>
is boxed to a C++ array, as demonstrated in \ref alib_boxing_arrays_vectors. Well, while this
is not pointer boxing, still a pointer to the first array element is stored.
Now a user of the standard C++ library knows that class \b std::vector<T> allocates dynamic
memory for storing the values. This memory is deleted with the destruction of the vector.
Hence, the life-cycle of the box is bound to its source object.<br>
But it is even worse: During the life-cycle of the box, the vector must also not be modified!
Appending a new element might or might not lead to a re-allocation of the internal array.
Consequently, a certain level of care has to be taken when passing boxes around to different
code entities.

Once more, the good news about the pitfalls of life-cycle-management lie in the limitations of
typical use cases of \alib_boxing_nl. In most cases, are not even actively created by
software. Instead, they are implicitly created when generic functions accept arguments
of type <c>const Box&</c>. In this most frequent case, after the function returns,
the current thread's stack frame is \https{unwinded,en.wikipedia.org/wiki/Call_stack#Unwinding},
and the boxed argument objects are disposed!<br>
A sample for this is given in \ref alib_boxing_appendix_usecases_format "appendix chapter C1".
of this manual.

Should the processing function want to store some data that it received from a box argument
"for later use", then such function itself should be responsible to create copies of such boxed data
that might be not available after the function returns. The function can quite easily
perform this, as it has anyhow knowledge about how to interpreted different boxed types and their
contents.<br>
A sample for this is implemented with \alib_expressions, which is discussed in more detail
in chapter \ref alib_boxing_appendix_usecases_expressions of this manual.

With \alib_expressions_nl, class \b Box is also used as a return value of functions.
While this is a more rare case, it is absolute rightful and necessary to do so in that module.
The constraints applied here is that the functions that return a box are responsible to ensure
that the contents is valid during a certain "scope" of the execution of the software.
This \e scope is individual per library and in case of \alib_expressions_nl it is well
documented in the according Programmer's Manual.

Finally, if the contents of boxes need to survive their originating object's deletion,
then a next option to achieve this, is given in chapter \ref alib_boxing_more_iclone.

\I{################################################################################################}
## 6.7 Comparison To std::any ## {#alib_boxing_classes_stdany}
It was already pointed out in chapter \ref alib_boxing_bijective "3. Non-Bijective Type Relationships"
that C++17 type \c std::any does not offer non-bijective boxing. Value type \c T is boxed as value
and type \c T* is boxed as pointer.
Consequently, a processing function implemented with \c std::any always had to check both types,
if it wants to support both.

In the case of storing pointers with \c std::any, the same care about life-cycle management is
needed as with using \alib_boxing_nl.

In the case of values, things can become quite ineffective. As type any does not "automatically"
switch to a pointer type, the copy constructor of objects provided as value is invoked.
For the storage of the copied object a heap allocation is performed. Note that many
developers underestimate the execution costs of allocating dynamic memory.<br>
Furthermore, the copy constructor of many types perform a "deep copy".
For example, in the case \c std::string<T> this means string data is copied. Besides the effort for
copying the string data itself, a second heap allocation has to be performed for the internal
string buffer.

From the other perspective: while \c std::any allows storing values of "any" size, class
\b Box does not. Even when boxing is customized, the conversion from the source object to
the boxed data must not perform (and store a pointer) to heap allocations.
\alib_boxing_nl simply does not perform any object destruction or deletion.

In this sample:

    MyClass  myClass1;
    MyClass  myClass2;
    Box box= myClass1;
        box= myClass2;

the boxing performed with the second assignment in the last line, simply overwrites what was
previously boxed, independent of the fact what that previous contents was.
The benefit of this is that boxing an extremely fast and efficient code. Often, the compiler
optimizes the assignment to a box to just writing directly the three <b>integer</b>-sized words.

Once more, the rationals behind this design is found in the use cases of \alib_boxing_nl,
which do not need anything else and heavily benefit from this behavior.

To conclude this section, let's imagine two functions, one accepting a variadic list of
\c std::any objects the other a variadic list of boxes. While to the latter, just any variable can
be passed "as is" because the automatic choice of the right type, with the \c std::any
implementation, each parameter has to be checked by the programmer to apply the right of operators
<c>'&'</c> or <c>'*'</c> that lead to an efficient and to the wanted behavior: copy or not!


\I{################################################################################################}
\I{############################      7. Customizing Boxing       ##############################  }
\I{################################################################################################}
# 7. Customizing Boxing # {#alib_boxing_customizing}
In previous chapters it was mentioned already several times that \alib_boxing_nl can be
customized per source type. From this, a good indication of what is customizable was already
given. At this point in the manual, it is a good time for explaining the customization in detail.

\I{################################################################################################}
## 7.1 Customization Features ## {#alib_boxing_customizing_goals}
The following customizations can be performed for a type:

<b>1. Type Mapping</b><br>
Customization allows mapping a source type (aka "boxable type") to a specific target type (aka "mapped type").
For example, the built-in customization (which can be deactivated) maps all common signed integral
types to the same destination type #alib::integer, unless they are bigger than the latter.

<b>2. Type Conversion Mechanics</b><br>
Depending on the customization performed, specific code for type conversion for both, boxing and
unboxing may be provided which replaces the built-in default mechanics.

<b>3. Manipulation Of Automatic Value-/Vector Boxing</b><br>
By default, \alib_boxing_nl does not distinguish between boxing a value type \b T or
its counterpart \b T*. The joint (same) mapped type of both is either one of them, depending on
a value's physical size and whether a type is copy-constructible and trivially destructible.<br>

This default behavior can be altered in arbitrary ways.

<b>4. Disallowing Unboxing</b><br>
If a type is mapped to a different target type, it might still be unboxable from this target type.
Sometimes, to forbid unboxing can be just a voluntary design decision. In other cases, unboxing the
original type might technically just not be feasible.<br>
(A sample for both options had been given in \ref alib_boxing_bijective_nonsurjective_tut.
A further sample was already explained in \ref alib_boxing_arrays_vectors)

<b>5. Disallowing Boxing</b><br>
Finally, boxing may also be completely forbidden for a type. With that, any assignment to
an object of type \b Box fails to compile. Forbidding boxing, by the same token disallows
unboxing.


\I{################################################################################################}
## 7.2 When Customization Is Needed ## {#alib_boxing_customizing_usecases}

The good news is, that the defaults of \alib_boxing_nl work well with most types.
The most frequent use case for customization, is to perform non-bijective type boxing, to
reduce the effort of processing boxes or to generalize a type to a common mapped type to enable the
processing of otherwise unknown (source) types.

With type mapping, two scenarios may occur:
1. The full information of the source type remains available.<br>
   The obvious sample is the built-in customization of types \c int8_t or \c int16_t to type
   \alib{integer}. As the latter is larger than the source types, all information contained
   in the source remains, except for the original type information.<br>
   In such a case it generally is a design decision, if unboxing the source type is still
   allowed. We had \ref alib_boxing_bijective_nonsurjective_tut "already seen" that the library
   itself has samples of both decisions implemented.

2. Only a part of the information stored in the source object is boxed.<br>
   Also for this, a sample was given already with chapter \ref alib_boxing_arrays_vectors which
   explained the built-in optional use of class \c std::vector<T>. This type holds a pointer
   to its buffer as well as the length of the stored array. In addition, also the length of the
   allocated buffer is stored. This is equal or greater to the array length.
   With the optional built-in boxing, this information is not stored. Instead, the type is boxed
   as a C++ array type, hence only the pointer to the buffer and its fill-length survive.<br>
   While in this specific case, unboxing is still feasible (by creating a new instance and copying
   the data into it like done by tool function \alib{boxing::compatibility::std;CopyToVector}),
   in more complex cases, unboxing might not be feasible at all. The recommendation is
   to disallow unboxing in general for partly stored types.



\I{################################################################################################}
## 7.3 Type Trait BoxTraits ## {#alib_boxing_customizing_traits}

As noticed in chapter \ref alib_boxing_basics_expl_templated, the design pattern of using
\ref alib_manual_appendix_tca "type-traits" is also leveraged with \alib_boxing_nl.
The type trait that is to be specialized for customization is given with
\ref alib::boxing::BoxTraits.

\attention
  Two code units must use the same custom boxing settings for all types that they share
  by passing boxes. Otherwise, this library has undefined behavior.
  The reason for this, was already explained in chapter \ref alib_boxing_fundamentals_disable.<br>
  This means that specializations of the type trait \b BoxTraits, need to be shared between all
  code units in question.
  In practice, specializations for this reason are made in header-files and such are to be included
  by each code unit that boxes or unboxes shared types.

Type trait \alib{boxing;BoxTraits} is well documented and should be referred to for all
details. The specialization of the struct can optionally be performed using helper macro
\ref ALIB_BOXING_CUSTOMIZE and its siblings.

Instead of repeating what is said in the reference documentation of the struct and macro,
this manual rather gives various real life samples along the lines of the important use cases.

\I{################################################################################################}
## 7.4 Tutorial: Mapping Type 'int16_t' to Type 'integer' ## {#alib_boxing_customizing_tut_int16_t}

The mapping of type \c int16_t to \c integer was already used as a sample in various parts of
this documentation.
Let's now look at how this is done with the built-in customization of type <c>int16_t</c>.
This piece of code does the job:

\snippet "boxing/boxingcustoms.inl.inl"    DOX_BOXING_CUSTOM_MANUAL


This is what is done:
- Struct \alib{boxing;BoxTraits} is specialized for source-type \c int_16_t.
  The type that a specialization is made for, always denotes the C++ source type supposed to be
  boxed differently.
- The type alias <em>"Mapping"</em>, specifies the target type as \c integer.
- The \c constexpr boolean value \b is specified to \c false.
- Static method <em>"Write"</em> is defined in a standard way: It just calls one of the overloaded
  \b Write methods of class \alib{boxing;Placeholder}, but is as well casting the 2-byte type to the
  platform-dependent type #alib::integer.
- Finally Method <em>"Read"</em> is declared. However, it is declared to return \c void, instead of
  the source type \c int16_t. Declaring \b Read to return \c void disables unboxing!
  As it is not called, no implementation of the function needs to be given.

Note that with non-bijective type mapping, all boxable types (source types) that are mapped to the
same destination type, have to "agree" to write the data in the same format.
It should be easy to understand that if doing otherwise, the result is undefined behavior.
The format that default boxing, as well as built-in customized boxing use, is documented with union
\alib{boxing;Placeholder}. In a later chapter, more information on this class is given.

Instead of providing all the code "manually", we could also pick and use one out of
\ref ALIB_BOXING_CUSTOMIZE "a set of provided macros":

    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE( int16_t, integer )

This macro produces the exact same code as shown above.

The principal differences when using the macros, are:
1. The code is less error prone.
2. The code is better readable.
3. Most important: Chances are high, that the code is compatible with future versions of the
   library.


\I{################################################################################################}
## 7.5 Tutorial: Mapping Type 'std::vector<T>' to Type 'T[]' ## {#alib_boxing_customizing_tut_vector}

In chapter \ref alib_boxing_arrays_vectors, boxing of \c std::vector<T> was demonstrated. It was
said that by including the header-file \implude{Compatibility.StdBoxtraits} a default
customization was given. In comparison to the sample of the previous tutorial section, there is one
small challenge here: The type is templated. The goal is now to define custom boxing for the type
\c std::vector<T> - independent of any element type \c T.

The syntax introduced with C++20 Concepts supports templated specializations in a straight forward
way.
In chapter \ref alib_boxing_strings "10. Boxing Character Strings" it will be shown that
\c std::vector<T> is to be customized differently if \b T is a character type.
Therefore, those have to be excluded from the templated specialization.

Here is the code for specializing the struct for all but character types, taken from the header
file named above:

\snippet "ALib.Compatibility.StdBoxtraits.H"    DOX_BOXING_CUSTOM_VECTOR

This specialization defines \c constexpr boolean value \b IsArray to \c true.
This denotes that the type should be boxed to a C++ array type, with elements of type \b Mapped.
Remember that C++ array types of arbitrary size cannot be defined with a (non-templated) type
definition. This is just not possible by the language.

The boxing method \b Write is so simple, its definition should not need any further explanation.
Finally, like in the sample shown in \ref alib_boxing_customizing_tut_int16_t, method
\b Read is declared to return \c void, which disables any unboxing of class \c std::vector<T>.
If a code still tried to unbox one, the compiler would complain something like that:

    static_assert failed due to requirement 'CustomBoxingRule7'
        Customized boxing forbids unboxing this value type: 'BoxTraits<T>::Read' returns a different type.

Note, that for templated specializations that specialize a whole group of types, no helper macro
exists.

\I{################################################################################################}
## 7.6 Customizing Value And Pointer Boxing ## {#alib_boxing_customizing_val_ptr}

In the previous sections, including the tutorial parts, we had only seen how value types \b T
continued to be boxed as \b T, or sampled with class \c std::vector<T> how a type
that would by default be boxed as \b T* is customized to always be boxed as a different
value type, in this case a C++ array.

There are two further cases possible:
1. A value type \b T which by default is boxed as value (because it just fits) should always be
   boxed as pointer type \b T*
2. A type \b T should be boxed just differently, if given by value \b T or by pointer \b T*

Both variants are explained now.

<b>Boxing both, \b T and \b T* as pointer \b T*:</b><br>
Should - for whatever reason - a fitting (small) and copy-constructible and trivially destructible
type be boxed as a pointer, a customization for the pointer type has to be given. The mapped type
then is the same pointer type as the source type.<br>
For example, if we wanted to have class \b SmallClass from a
\ref alib_boxing_classes_tut "previous tutorial sample", to always be boxed as pointers,
the customization would look like this:

    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( SmallClass*, SmallClass* )

As the default boxing and unboxing mechanics work well with pointer types, we can simply use
macro \ref ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING for this.

Of course, a different target type could be specified likewise. The important point here is, that
if a specialization for \b T* is given and none for \b T, this customization is used for mapping
both \b T and \b T* to \b T*.

\note
  Within the \alib library as a whole (which makes quite some use of this basic \alibmod) no
  fitting and copy-constructible and trivially destructible type exists with such a customization.
  We have not found a use case, yet.

<b>Boxing types \b T and \b T* differently:</b><br>
The last case revokes the non-bijective default behavior of boxing complement types \b T and \b T*.
Instead, a one to one mapping is enabled.<br>
All that is needed for this is to specify just both customizations:

    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( SmallClass , SmallClass  )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( SmallClass*, SmallClass* )

Again, different mapped type and custom <b>Write</b>/<b>Read</b> methods may be given, if other
macros were used.

For this variant, valid use cases exist - although again, no \alibmod uses that internally.
As a sample, let us stick to type \c std::vector<T>.
We learned, that with the inclusion of header \implude{Compatibility.StdBoxtraits},
values and pointers to the type becomes boxed as C++ array type.<br>
With this custom boxing, internal information of the vector object is lost (the capacity).
A processing function, can only access the currently stored elements, but the vector
cannot be unboxed to be modified. If unboxing a pointer was allowed, the unboxed vector could
be modified (what of course would modify the original object).<br>
This could rightfully be wanted behavior and looking at C++17 type \c std::any tells us, that
with its lack of non-bijective type mapping, this is even the only possible behavior there.

Because for a templated specialization, none of the helper macros can be used, the following
templated specialization of the type trait \alib{boxing;BoxTraits} has to be given:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_CUSTOM_VECTOR_POINTERS_CUSTOMIZE

Still the header \implude{Compatibility.StdBoxtraits}
is to be included, as we want to keep mapping of value types to C++ arrays intact.

Without the customization shown above, the following code would not compile:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_CUSTOM_VECTOR_POINTERS_SAMPLE

The compiler would complain in line 4:

    static_assert failed due to requirement 'CustomBoxingRule1'
        This pointer type T* cannot be unboxed, because custom boxing is defined for value type T,
        while no custom boxing is defined for pointer type T*.

When patiently reading further, a next compiler error tells us:

    static_assert failed due to requirement 'CustomBoxingRule9'
        Customized boxing forbids unboxing value type T ('BoxTraits<T>::Read' returns a different type),
        while no customization for this pointer type T* was given.


With the additional customization, the code compiles fine and the output is:
\snippet "DOX_BOXING_CUSTOM_VECTOR_POINTERS.txt"     OUTPUT

\I{################################################################################################}
## 7.7 Tutorial: Conditional Customization ## {#alib_boxing_customizing_conditional}

This manual cannot go into the all details of the C++ language. The good news is that with the
C++20 language version, the syntax of what was formerly called "template meta programming"
became a lot of easier to understand. Once more, we want to hint to the quick appendix chapter
\ref alib_manual_appendix_tca of the general \alib manual.

Now, this tutorial section gives an example and an indication of what is possible.

We had seen, that specializing the type trait \alib{boxing;BoxTraits} for a single type
has the following syntax:

    template<>
    struct BoxTraits<MyType>
    {
        using Mapping= MyTargetType;
        ...
    };

To do the same in a templated fashion for a generic type, we used:

    template<typename T>
    struct BoxTraits< MyGeneric<T> >
    {
        using Mapping= MyTargetType<T>;
        ...
    };

This maps a whole set of types to the same target type.
But how about other sets of types? Sets not defined by generics?
For example, an obvious question is: how can a type and all its derived types be customize
at once?

All that is needed to achieve this, is using C++20 keyword <c>requires</c> with the specialization.
Consider the following two types:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_CONDITIONAL_TYPES

We do specialization for type \b MyBase and all derived types:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_CONDITIONAL_CUSTOMIZING

The following sample proves that we achieved what we wanted, because it successfully compiles
and when running, it does not produce a run-time assertion about unboxing wrong types:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_CONDITIONAL_USING

The output is:
\snippet "DOX_BOXING_CONDITIONAL.txt"     OUTPUT

Finally, if we tried to unbox the derived type:
\snippet "DOX_BOXING.cpp"     DOX_BOXING_CONDITIONAL_USING2

The following compiler error was given:

    static assertion failed due to requirement 'CustomBoxingRule7':
      Customized boxing forbids unboxing this value type: 'BoxTraits<T>::Read' returns a different type.


\I{################################################################################################}
## 7.8 The Nested Union "Placeholder" ## {#alib_boxing_customizing_placeholder}

We have seen in the previous chapters, that even when boxing is customized, such customization often
can conveniently use the simple default implementations of methods \alib{boxing;Placeholder::Write}
and \alib{boxing;Placeholder::Read}.

This is because the methods are implemented by a set of overloaded and type-traits enabled
methods, that go along well with fundamental and fitting value types.

A closer look at the \alib{boxing;Placeholder;reference documentation of union Placeholder},
shows that this type is a union of different unions.<br>
Besides using the overloaded methods \b Read and \b Write, it is also possible to directly access
its different members from within the \b BoxTraits specialization.

Two little challenges should be considered when writing custom values.
<b>1. Constexpr Boxing </b><br>
In a few special cases, \c constexpr boxing may be desired. This requirement was the reason
to introduce all these unions with their \c constexpr constructors. All built-in boxing of
fundamental types meet this requirement.
More on this topic is given in later chapter \ref alib_boxing_more_opt_constexpr.

<b>2. Gapless Use Of the Placeholder </b><br>
A second goal that the various unions try to enable is that values of custom type are written
without gaps. While a gap is not the real problem, problems might occur if random data
remain in these gaps. A rationale why this might be a problem is given in the upcoming
chapter \ref alib_boxing_functions.<br>
By the same token, the size of the data written to the placeholder should be announced
by the user in the case that it differs from the value size, respectively pointer size.
Such announcement can be done by modifying the traits struct \alib{boxing;SizeTraits}.
  
\I{################################################################################################}
\I{################################################################################################}
## 7.9 Bypassing Custom Boxing With Identity-Boxing  ## {#alib_boxing_customizing_identity}

There might be situations, where an exception to the bijective, simplifying nature of
\alib_boxing_nl is needed. For example, as it was explained already, if a set of custom types
are all boxed to the same more fundamental mapped type, then of course information is lost.
Because customization of boxing has to be the same throughout all compilation units, a decision
for such "reduced" boxing of a type is a global decision.

But there is an easy way to "bypass" custom boxing: All that is needed is to "wrap" an object
into another one and box the wrapper type. A convenient wrapper type that can well be used
is found in the C++ standard library with \c std::reference_wrapper. This templated class is very
simple and stores a reference to the object given in its constructor.<br>
Of course, a wrapped object of type \b T has to be guessed and unboxed as type \c std::reference_wrapper<T>
and furthermore different life-cycle restrictions might apply in contrast to using the customized
boxing (according to the standard C++ mechanics and rules).

The following sample demonstrates the technique with two types:
- \c float<br>
  For fundamental types, \c std::reference_wrapper cannot be used. Therefore , a quick
  custom struct \b WrappedFloat is given. By default, fundamental type \c float is converted
  to \c double when boxed.

- \alib{strings;TAString;AString}<br>
  This is a class found in module \alib_strings.
  While an own, \ref alib_boxing_strings "dedicated chapter about string-boxing" is given later,
  so much should be explained here: The class manages an own string buffer and does not fit
  into a box's placeholder. Customization defines it to be boxed as character array.
  Thus, an \b AString cannot be retrieved back when unboxing.


The wrapper types consequently are:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_CUSTOM_BYPASS_WRAPPERS

With that in place, either use custom boxing can be used, or it can be bypassed:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_CUSTOM_BYPASS

With method \b process defined like this:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_CUSTOM_BYPASS_PROCESS

The output of the code above will be:

\snippet "DOX_BOXING_CUSTOM_BYPASS.txt"     OUTPUT


\I{################################################################################################}
## 7.10 Summary And Rationals ## {#alib_boxing_customizing_summary}

In the previous sections of this chapter, most details of "custom boxing" were explained.
Technically, custom boxing allows modifying the type mapping as well as the way object data
is boxed and unboxed.

While this chapter was quite lengthy, and while C++20 Concepts and the creation of
specializations of type traits may challenge less experienced C++ programmers a bit, a user of
this \alibmod_nl should not fear too many troubles in respect to custom boxing.<br>
This is true, because:

1. The defaults work well for most types, hence custom boxing is not often needed.
2. The customization is quite simple: All that is needed to be given is:
   - The source and the target type
   - Two rather short static methods that perform a simple data conversion.

In consideration of this effort, the benefits are huge. The main goal that custom boxing achieves
is to further shrink the set of mapped types. Using \alib without any modification,
this set is already reduced dramatically:
- Any value type \b T and its corresponding pointer type \b T* are boxed to the same type.
- The built-in customizations reduce the set of fundamental types to only three main types.
- Types \c std::vector<T> and similar 3rd party types, are boxed to corresponding C++ arrays.

Reducing the types, does not only mean that the effort of guessing types when processing boxes
is reduced. Often, a custom type is mapped to a common, "already known" type. In this case
a processing function does not even need to be changed. This is very helpful if a programmer
just can't change such function, when it resides in a library, or her co-worker is just responsible
for it.

We will see in later chapter \ref alib_boxing_strings "10. Boxing Character Strings" that
\alib_boxing_nl by default maps arbitrary (3rd-party) string types to simple C++ character
arrays. A function that processes boxed character arrays
will this way \b generically be able to digest any 3rd party string without the need of adoption.

Of course, there are limits in achieving generic processing of arbitrary boxed types by just
mapping the types. While strings are a great sample, often it is not an option to just map a
type to something else, maybe because in other places more of the original type's data is needed and
boxing it as a pointer to the original is mandatory.

To revoke these limitations, lets quickly move on to the next chapter of this manual!


\I{################################################################################################}
\I{##############################          8. Functions          ################################  }
\I{################################################################################################}
# 8. Box-Functions # {#alib_boxing_functions}

You, the reader of this manual, probably know all details of C++ and virtual functions.
The first section of this chapter provides a brief recap of some basic knowledge on that matter,
You are free to skip that!

People in a hurry, might also want to skip section \ref alib_boxing_functions_concepts
and instead right jump into code with \ref alib_boxing_functions_tut.


\I{################################################################################################}
## 8.1 Introduction: OO-Languages And Virtual Classes ## {#alib_boxing_functions_introduction}

In previous chapters, it was explained that mapping several C++ types to the very same boxed type,
does not only reduce the efforts of processing boxes. It further allows processing boxes created
from "unknown" types that are mapped to a known boxed type.
While this is well feasible for some types, for others type mapping may not be an option, when
too much information gets lost.

To resolve the general problem, object oriented languages offer <em>"virtual functions"</em>:
Instead of performing the task themselves, the processing code calls a type-specific, "virtual"
function on a given argument.
This way, the responsibility is passed back to the object that is processed.<br>
But how is this technically solved? How does the processing function know the address of the
function that is to be called, when it is a different function for each object type?

C++ uses run-time type information for that. While non-virtual class methods are statically
linked at compile-time (respectively at link time), the address of a virtual function
call is only evaluated at run-time.
As soon as a first virtual function is declared with a class (or one of its base classes),
a \https{virtual function table,en.wikipedia.org/wiki/Virtual_method_table} (aka "vtable")
is added by the compiler, and each new instance of that type contains a hidden member that points
to that table. Such types are called "polymorphic types" or just "virtual types".

Adding this \e vtable increases the footprint of virtual C++ types by the size of one pointer.
Together with the loss of run-time performance, this increase of object size is the general disadvantage of
virtual classes. It is technically just not avoidable: If a processing function should be able
to call variants of methods tailored to types that it does not "know" at compile-time, then the
memory addresses of these methods have to be passed together with the argument object.

Virtual functions are just one out of <b>two purposes</b> for having a
\https{vtable,en.wikipedia.org/wiki/Virtual_method_table} in C++.
Its second use is with C++ keyword \c dynamic_cast<T>. While a \c static_cast is performed by the
compiler, a \c dynamic_cast<T> is performed at run-time by special code inserted by the compiler.
This code performs a type-check using the vtable. On failure, \c dynamic_cast returns \c nullptr.

We had learned in chapter \ref alib_boxing_basics_expl, that \alib_boxing_nl stores
run-time type-information along with the boxed data.
You could rightfully say, that the disadvantage of needing a \e vtable with instances of virtual
C++ classes, is of the very same nature like the need to store type-information with boxes.
While C++ uses this pointer for type-checks and virtual function calls, so far we had seen
\alib_boxing_nl using it for type checks only.

Well, here is the good news: Also \alib_boxing_nl supports virtual function calls!


\I{################################################################################################}
## 8.2 Function Declarations, Implementations and Registrations ## {#alib_boxing_functions_concepts}

In the sense of the C++ language, any function that is invokable on an instance of
type \alib{boxing;Box} is "virtual", because run-time type information is used to determine
the right version of the function for a box containing a certain mapped type.
However, from the perspective of \alib_boxing_nl, there is nothing like a "static" or "link-time" function.
As a consequence, this manual of module \alib_boxing_nl does not talk about
"virtual functions" but just "functions" or "box-functions".

This section explains the three steps to define box-functions.


\I{################################################################################################}
### 8.2.1 Function Declarations ### {#alib_boxing_functions_concepts_decl}
Type-safeness is a mandatory feature of any C++ software. \alib_boxing_nl is a type-safe
software, although - for technical reasons - some heavy use of keyword \c reinterpret_cast
is done when boxing and unboxing values.
While the type-safeness is lost at compile-time, it is regained at run-time with the
use of the templated interface methods.
For example, if \p{T} in a call to \alib{boxing;Box::Unbox<T>} does not match the boxed type,
a run-time assertion is given. And such can be prevented using \alib{boxing;Box::IsType<T>},
which never asserts.

With box-functions, the situation is similar: For technical reasons, the \e vtable of a box stores
the address of invokable functions as a \c void*. However, when the function is invoked, a template
parameter used with the invocation ensures that the signature of the function stored matches the
function parameters given.

We call the template parameter types used with function invocations <b>"FunctionDescriptors"</b>.
Such \b FunctionDescriptor is just a struct with a single type definition.<br>
Here is a sample:

\code{.cpp}
  struct FMyDescriptor
  {
      using Signature = bool (*) ( const Box& self, int arg1, double arg2 );
  };
\endcode

Besides the requirement that the type definition in the struct is named "Signature" and
that it denotes a function pointer, only two further conditions need to be met:

1. <b>The first argument of the function has to be of type 'const Box&' or 'Box&':</b><br>
   When a function is invoked on a box, a reference to the box is passed as the first argument.
   Most box-functions do not modify the box and use <c>const Box&</c>.
   We will learn in \ref alib_boxing_functions_mutable "a later section" about the difference of
   invoking constant and non-constant box-functions.<br>
   The name of this parameter is not necessarily needed to be specified. Internally,
   \alib_boxing_nl volunteers to do so and always use name \b "self".<br>
   <br>
2. <b>The return type of box-functions must be default-constructible:</b><br>
   When invoking a function on a box, the result of that invocation is returned. As it might
   happen that a function is not defined for a specific mapped type, a default value is needed.
   Then a default value is created and returned.<br>

   In the case that a function should return a type which is not default-constructible,
   then the approach to do this, is to declare the function \c void and instead add an output
   parameter. For example, a pointer to a pre-constructed object or a pointer to a pointer,
   if the object should be dynamically allocated by the function.


\I{################################################################################################}
### 8.2.2 Function Definitions ### {#alib_boxing_functions_concepts_def}

The second ingredient needed are function implementations - one for each mapped type that
is to be supported.
Implementations can be defined globally or within a namespace.
Furthermore, static member functions are likewise accepted.

However, it is always a good idea to place box-functions in an anonymous namespace of a compilation
unit (aka non-header-file).
With that, it is hidden from the C++ linker and does not clutter a compilation unit's linker
information.<br>
It is possible to do so, because the functions are not called using the linker or C++ virtual
tables. Instead, \alib_boxing_nl uses the C++ call <c>operator()</c> directly on
their address stored in the vtable of the box.



\I{################################################################################################}
### 8.2.3 Function Registration ### {#alib_boxing_functions_concepts_reg}

The final step is to associate the function implementation with boxes of a specific mapped type.
This is done with templated namespace function \ref alib::boxing::BootstrapRegister.

\attention
  Function registration and function invocation are not protected against racing conditions
  of multithreaded access. For this reason, it is mandatory to perform function registration exclusively
  while \ref alib_mod_bs "bootstrapping" software, when no threads are started,
  yet. Registrations can be made before bootstrapping \alib, respectively during or after
  phase \alib{BootstrapPhases::PrepareResources}.

\attention
  If for any reason registration is performed \b after bootstrapping \alib and module
  \alib_monomem is included in the \alibbuild, and this function is invoked after
  \alib was bootstrapped, then before an invocation of this method, mutex
  \alib{monomem;GLOBAL_ALLOCATOR_LOCK} has to be acquired. This can be done with:
           \snippet "ut_monomem.cpp"     DOX_MONOMEM_LOCK_GLOBALALLOCATOR
 
\attention
  Note that even when this lock is set, still multithreaded access to registration and/or
  box-function invocations is <b>not allowed</b>.

The function uses three template parameters that have to be explicitly specified because they
cannot be deduced by the compiler:
1. The function descriptor type.
2. The mapped type.
3. A boolean value denoting whether array-boxing is used or not.
   This third value defaults to \c false and hence can be omitted for plain-type boxing.

The address of the box-function is to be passed to \alib{boxing;BootstrapRegister} as the single
normal argument of the function.

\note
  Details on the internal implementation of boxes are given in later chapter
  \ref alib_boxing_more_opt_staticvt_bg.<br>
  An understanding of these details will make clear, why box-functions have to be registered
  at <b>run-time</b>.



\I{################################################################################################}
## 8.3 Tutorial: Implementing A 'ToString()' For Boxes ## {#alib_boxing_functions_tut}

The previous chapter gave a detailed (rather lengthy) explanation about box-functions.
This tutorial section now shows how simple their definition and use in deed is.

The goal of the sample we are looking at, is to enable boxes to write their contents to
a string. In other programming languages, such function is often called \b ToString().

Here is the declaration of the function:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_FUNCTIONS_DESCRIPTOR

Besides the box itself, the function expects an \alib{strings;TAString;AString} defined with module
\alib_strings. This is used as a buffer to write to.
The return value is \alib{strings;TString;String}, which is a lightweight string type, similar to
C++17 type \c std::string_view.

Let's create three implementations for different types:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_FUNCTIONS_IMPLEMENTATION

First of all, it has to be noticed that unboxing from parameter \p{self} does not need
type-guessing with \alib{boxing;Box::IsType}.
The reason is that each function is associated with boxes of a corresponding type and thus \p{self}
always contains the right type.

The first two implementations simply unbox the right type and use
\alib{strings;TAString::operator<<;AString::operator<<} to convert the type.<br>
The third function is templated. It is designed to be usable with different boxed array types.
Unfortunately, we cannot attach a templated method to just various boxes.
Instead, an instantiation of the templated function has to be given for each boxed array type
that we want to support.
Such instantiation is implicitly performed by the compiler when passing the function to
\alib{boxing;BootstrapRegister}.

Let's register 4 functions that way:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_FUNCTIONS_REGISTER

A call to \e RegisterMyFunctions() needs to go to the
\ref alib_mod_bs "bootstrap section" of the process.

With all that in place, functions can be "called" with templated method \alib{boxing;Box::Call}.
It expects the function declaration as a template type and the function argument as its own
arguments. Its return type is equivalent to the return type of the box-function!<br>

The following code creates an array of boxes and calls their method in a loop:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_FUNCTIONS_INVOKE

The output of the code above will be:

\snippet "DOX_BOXING_FUNCTIONS_INVOKE.txt"     OUTPUT

We conclude this tutorial section with a test: What happens if we invoke the method on a box
of a mapped type that no implementation is registered for? As we were lazy, for example
\b uinteger is not covered:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_FUNCTIONS_INVOKE2

Running this does not assert! The output is:

\verbinclude "DOX_BOXING_FUNCTIONS_INVOKE2.txt"

Obviously an empty string was returned by \alib{boxing;Box::Call}, without further complaints.

\I{################################################################################################}
## 8.4 Default Functions ## {#alib_boxing_functions_defaults}

It is a design decision of \alib_boxing_nl, that calls to box-functions that are not
registered for the actually boxed type, do not assert. Method \alib{boxing;Box::Call} just returns
a default value of the designated return type, that's it. The rationale for this design is once more
to favor convenience when handling boxes over other considerations. A processing code could
use \alib{boxing;Box::GetFunction} before invoking the function, if it wanted to react on
boxes that do not support a box-function.

Looking at virtual functions of OO-languages once more: There, virtual functions may or may not be
specialized with each derived class. If a function is invoked on a derived class, the "best"
implementation is chosen, by walking up the inheritance chain and choosing the first implementation
found in a base class.

The type system of \alib_boxing_nl is not hierarchical and does not know inheritance.
But in theory there are at least two levels!
1. A box with a mapped type, and
2. A box of just any type.

And that is our little fallback: This library supports to define "default functions" that - if
available - are used when no specific function.

Often, there is not much to do for them, because interpreting the \alib{boxing;Placeholder}
contents without knowing the type, is not possible. Still we will see in a next chapter that
there are some good use cases for them.<br>
Sometimes it is useful to implement and register a default function solely in debug-compilations of
software: These can then assert, write log file warnings or perform other appropriate actions.

Default functions are registered with namespace function \alib{boxing;BootstrapRegisterDefault}.
Compared to \alib{boxing;BootstrapRegister}, the function omits the second template parameter specifying
the mapped type.

\I{################################################################################################}
## 8.5 Tutorial: A Default ToString() Function ## {#alib_boxing_functions_tutdefaults}

To continue the sample of section \ref alib_boxing_functions_tut, a default implementation
usable with any box of \b FToString should be developed. Here it is:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_FUNCTIONS_IMPLEMENTATION3

It is registered with \alib{boxing;BootstrapRegisterDefault}

\snippet "DOX_BOXING.cpp"     DOX_BOXING_FUNCTIONS_REGISTER3

We repeat the "failed" invocation we had with type \b uinteger and also test a call on a
boxed array with an unknown element type. A third type repeats the call on a character array,
that got a specialized implementation:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_FUNCTIONS_INVOKE3

The result is now:

\snippet "DOX_BOXING_FUNCTIONS_INVOKE3.txt"     OUTPUT


\I{################################################################################################}
## 8.6 Calls To Undefined Functions And Empty Boxes## {#alib_boxing_functions_tolerance}

It was already mentioned, that \alib_boxing_nl is tolerant towards calling a function
on a box whose mapped type is not associated with an implementation.
The call is just not performed and instead, a default-constructed value of the according return
type is returned by method \alib{boxing;Box::Call}.<br>
By the same token, a call of a function performed on a box that "does not contain a value"
(see chapter \ref alib_boxing_more_void) is likewise tolerated.

This design decision is once more justified with the common use cases for this module.
The expectation of a programmer calling a box-function is: "Perform what is appropriate with
the boxed type". And if there is no implementation, well, to do nothing is the appropriate action.
Consequently, specific checks for the availability of function implementations can be omitted.

If a code wanted to take action on the fact that no type-specific implementation exists or
that neither a type specific, nor a default implementation exists, such availabilities
can be queried using \alib{boxing;Box::GetFunction}. The method's parameter \p{searchScope}
controls which sorts of functions are searched. The method is likewise tolerant against
unset boxes.<br>
If this is done, the returned function pointer already contains the function found, respectively is
\c nullptr on failure. To avoid a repeated search for that same function with a subsequent
\b Call, alternative method \alib{boxing;Box::CallDirect} can be used, which omits the search
and instead expects the function pointer as a first parameter.

Finally, to check whether a box does not contain a value before calling a box-function,
type-guessing for type \c void is to be used with \alib{boxing::Box;IsType<void>()}.

\I{################################################################################################}
## 8.7 Function Calls On Mutable Boxes ## {#alib_boxing_functions_mutable}
It might happen that a box-function intends to change the contents of a box. In theory, such change
could even include changing the mapped type, but changing the value only is probably a more
common use-case.

Two things are needed to allow that:
1. The type definition \b Signature of the function descriptor needs to specify mutable type
   <c>Box&</c> for first parameter \p{self}.
2. The non-constant version of \alib{boxing;Box::Call} has to be selectable by the compiler, hence
   the box that \b call is invoked on has to be mutable.<br>
   Method \alib{boxing;Box::CallDirect} is likewise available in a non-constant version for cases
   that tested for the availability of a function upfront.

The next chapter introduces the built-in functions of \alib_boxing_nl.
With them, one quite useful sample of a mutable box-function is found.

\I{################################################################################################}
## 8.8 Built-In Box-Functions ## {#alib_boxing_functions_builtin}

### 8.8.1 Equals, Hashcode, Clone...   ### {#alib_boxing_functions_builtin_intro}
In contrast to C++, many other object oriented programming languages declare any class to be
inherited of a built-in base type. For example, in \b JAVA, all classes inherit class \b Object.
Such  "mother of all objects", usually provides a set of methods that are available for
any object in the language. In \b JAVA, the methods, for example, include
\e equals(), \e hashCode(), \e clone() and \e toString().

Likewise, module \alib_boxing_nl implements a set of built-in box-functions. Those are:
- \alib{boxing;FIsNotNull}
- \alib{boxing;FHashcode}
- \alib{boxing;FEquals}
- \alib{boxing;FIsLess}
- \alib{boxing;FIsTrue}
- \alib{boxing;FClone} (Available only with the inclusion of module \alib_monomem in the \alibbuild.)
- \alib{boxing;FAppend} (Available only with the inclusion of module \alib_strings in the \alibbuild.)

With the inclusion of module \alib_format, furthermore function \alib{format;FFormat}
becomes available.

The following implementations are given:
- Default implementations are registered for all built-in function types.
- If appropriate, implementations for C++ fundamentals are given.
- Some of the functions provide templated implementations that can be generically registered with
  mapped types that meet certain conditions.
  If available, these templates are defined as static members of the corresponding function
  descriptor struct. This way, notes to such templated functions are included in the reference
  documentation of the function descriptors listed above.
- With the call of certain bootstrap functions declared in header-files like
  \implude{Compatibility.StdBoxtraits}, or \implude{Compatibility.QTBoxing}, implementations for
  types of namespace \c std and 3rd-party types become available.

\attention
  The registration of built-in box-function implementations, needs due bootstrapping of the
  library. See chapter \ref alib_boxing_more_bootstrap for more information.

This manual will not repeat a description of each function. Instead, please see the
corresponding reference documentation, linked above with the enumeration of functions.
Therefore, we conclude this section with just some quick facts:
- Functions \alib{boxing;FEquals} and \alib{boxing;FIsLess} are called with
  global operators
  - \alib{boxing;Box::operator==}
  - \alib{boxing;Box::operator!=}
  - \alib{boxing;Box::operator<}
  - \alib{boxing;Box::operator<=}
  - \alib{boxing;Box::operator>}
  - \alib{boxing;Box::operator>=}

  For example, the following if statement:
  \code{.cpp}
  if( box1 == box2 )
  {
     ...
  }
  \endcode
  translates to:
  \code{.cpp}
  if( box1.Call<FEquals>( box2 ) )
  {
     ...
  }
  \endcode
  The fist version results to shorter code, but slightly slower code, because the operator's
  implementations are not inlined.

- Functions \alib{boxing;FIsNotNull} and \alib{boxing;FHashcode} are likewise available through members
  \alib{boxing;Box::IsNotNull} and \alib{boxing;Box::IsNull}, respectively \alib{boxing;Box::Hashcode}.
  Also these methods are not inlined.<br>
  More information on this function is given in chapter \ref alib_boxing_more_void_nulled.

- Function \alib{boxing;FClone} is explained in some detail in manual chapter
  \ref alib_boxing_more_iclone. Furthermore, it is used with module \alib_exceptions, as
  documented in the \ref alib_exceptions_message_lifecycle "Programmer's Manual of that module".

- Function \alib{boxing;FAppend}, available with module \alib_strings is explained
  in manual chapter \ref alib_boxing_strings "10. Boxing Character Strings".

- Function \alib{format;FFormat}, introduced by module \alib_format is not considered
  a built-in box-function (of this module) and therefor explained in that module's manual chapter
  \ref alib_format_custom_types.
  There, source-code samples of both, function \b %FAppend and \b %FFormat are given.

As a final remark, some of the built-in function declarators provide inner static functions,
with some of them being templated. Those may be used to create custom specializations. Again,
please consult the reference documentation for further details.

\I{################################################################################################}
### 8.8.2 Overwriting A Built-In Function ### {#alib_boxing_functions_builtin_override}

Repeated registrations of default or type-specific functions using \alib{boxing;BootstrapRegisterDefault}
and \alib{boxing;BootstrapRegister}, are allowed.
Any formerly set function is simply replaced. It is also allowed to register \c nullptr, which
disables a built-in function without providing a new one.

The built-in default and type-specialized functions are registered with namespace function
\alib{boxing;bootstrap}. This function is automatically invoked with
\ref alib_mod_bs "bootstrapping the library".

Any function implementation that specializes the behavior for a mapped type, may call the
default implementation internally, for example, to take specific action if a certain state of the
boxed value is given, otherwise use the default implementation and probably return its result.
To achieve this, the pointer to the default function implementation has to be received, which
is done with method \alib{boxing;detail::FunctionTable::Get} that has to be invoked on
singleton object \alib{boxing;detail::DEFAULT_FUNCTIONS}.

While this already touches objects in namespace \b detail, calling a specialized version of a
function that was replaced by another (like calling the implementation of a base class in C++)
is not explicitly supported by the library, but possible. For this, the bootstrap code that
registers a function has to receive and store the previously registered implementation, which
then can be called and which in turn may call another one or the default.<br>
The rationale why this is not otherwise offered by the library is that such complicated use of
box-function is out of the scope of the usual use cases for \alib_boxing_nl.


\I{################################################################################################}
\I{#########################         9. Boxing Enumerations         #############################  }
\I{################################################################################################}
# 9. Boxing Enumerations # {#alib_boxing_enums}

\I{################################################################################################}
## 9.1 Boxing Enumeration Elements  ## {#alib_boxing_enums_integer}
Scoped enumerations as well as traditional enumerations, receive a special
treatment with \alib_boxing_nl. Unless their boxing is not customized, they are boxed
to their identical type, the value stored in the \alib{boxing;Placeholder} is cast
from their underlying integral type to \alib{integer}.
When unboxed, the value is cast back from \c integer to the original underlying type.

While this speciality is not noticeable when boxing and unboxing enumerations, the advantage
of this treatment is that the different enum element-values of different enum types become
"generically usable" when read directly from \alib{boxing;Placeholder::Integrals}.
The rationale why this constitutes "an advantage", is given in the next section.

\I{################################################################################################}
## 9.2 Class Enum  ## {#alib_boxing_enums_class}

Class \alib{boxing;Enum} is the only derivate of class \alib{boxing;Box} found in the library.

The class is useful to store and pass around enum values of arbitrary C++ scoped enum types in
a type-safe way.
It is implemented to ease the use of scoped enums in situations where programmers
otherwise tend to "fall back" to non-scoped (non type-safe enum types). This is the case, when
enum elements of different types should be allowed as a function argument or otherwise used
as an "identifier". While C++ 11 introduced the syntax for <c>enum class</c> types
(aka "scoped enums"), still these are very limited. In especial, those do not support inheritance.
Thus, an API cannot define an interface method that accepts enums of "custom derived types".
This is quite often a problem. Of course, using module \alib_boxing_nl, an interface method
now may accept a box, but then anything else apart from enumeration types was accepted.
Class \alib{boxing;Enum} as a good tool to help here.

In the constructor, enum elements of arbitrary type are accepted. With the run-time type-information
added, the processing function can now work with any enum type transparently.

A good example use case is given with type \alib{exceptions;Exception} of module \alib_exceptions.
Any exception is created with an enum element of arbitrary type. The exception handlers then
can use nested \c if statements: The outer \c if is about the exception type, the inner about
the concrete exception. This gives a nice two-level order scheme for exceptions with no
need to define "error number ranges" for each code unit.




\I{################################################################################################}
\I{#########################      10. Boxing Character Strings       #############################  }
\I{################################################################################################}
# 10. Boxing Character Strings # {#alib_boxing_strings}

\I{################################################################################################}
## 10.1 Dependency Modules "Characters" And "Strings" ## {#alib_boxing_strings_intro}
A lot was said already in this manual about non-bijective boxing and its advantages. When it
comes to boxing string types, the way to go is obvious: Whatever string type is boxed (and there
might be many of them found in software that uses 3rd-party libraries) - everything is simply
boxed to a C++ array of the corresponding character type.
A processing function then does not need to care about the origin type, but by only handling
character arrays, any sort of string is treated correctly.

To achieve this, this module leverages type definitions and type-traits found with module
\alib_characters. This is explained in the next section.<br>
The section after that, covers further options that are available when module \alib_strings
is included in the \alibbuild_nl. Finally, some good use of \alib_boxing_nl and
\alib_strings_nl is made by the module \alib_format. While this is not a part of this
manual, some overview on it is provided in appendix
chapter \ref alib_boxing_appendix_usecases_format "C.1 Use Case: Module Format".

\I{################################################################################################}
## 10.2 Character Arrays ## {#alib_boxing_strings_characters}
Previous manual chapter \ref alib_boxing_customizing "7. Customizing Boxing" explained in detail
how the type trait \alib{boxing;BoxTraits} is used to provide information and static methods
that allow to customize boxing of any type. The gaol with boxing string types is to map any of them
to a character array. This could be done in the straight forward way, for example, by just
specializing <b>BoxTraits<std::string></b> for C++ standard type \c std::string.

But this is not what this library does! Instead, it leverages module \alib_characters_nl.
An interested reader should read this module's \ref alib_mod_strings "Programmer's Manual" now
first, before continuing with this chapter of \alib_boxing_nl.
A short summary of what is provided by this module should be given in bullets:
- Module \alib_characters_nl defines a set of new character types with the aim to replace
  the C++ ones:
  - Three types that aim to make C++ character types \c wchar_t independent of platform and compiler:
    \alib{characters;nchar}, \alib{characters;wchar} and \alib{characters;xchar}.
  - Three types that make the use of narrow or wide character types transparent to a user's code:
    \alib{characters;character}, \alib{characters;complementChar} and \alib{characters;strangeChar}.
  - Each type in the two groups is equivalent to one type of the other group.
- As a result of these definitions, the module emphasizes the use of type \alib{characters;character}
  whenever possible. Depending on the platform and the chosen library compile options, this type
  is an alias of either \c char, \c wchar_t, \c char16_t or \c char32_t.
- Next, the type trait \alib{characters;ArrayTraits<T, TChar>} is provided. Specializations can be
  given to denote that a type \b T is a of array type of \b TChar. Furthermore access methods to
  an object's array data and length are provided and optionally also a method that creates an object
  of type \b T from a given character array.
- The library defines such type-traits for built-in C++ "string types", like <c>const character*</c>,
  string literals and character arrays.
  With the inclusion of module \alib_strings, type-traits for the five string-types found in that
  module are given.
  Finally,compatibility headers are provided which, for example, specialize \b ArrayTraits for string
  and vector types of namespace \c std or those found in the \https{QT Class Library,www.qt.io}.

\note
  Module \alib_characters in addition provides type-traits to denote "bad old" zero-terminated
  character arrays. \alib_boxing_nl does not make any use of this but instead treats any
  character array as if it was \b not zero-terminated.

With this in place, all that this module provides is a conditional specialization of type-traits
struct \alib{boxing;BoxTraits} for all types that \alib{characters;ArrayTraits} is specialized for!
\note
  A sample for a "conditional specialization" was given in tutorial chapter
  \ref alib_boxing_customizing_conditional.

Precisely, two conditional specializations are given:
1. If field \alib{characters;ArrayTraits::Access} is specialized to equal
   \alib{characters;Policy::Implicit}, \b and
   field \alib{characters;ArrayTraits::Construction} is specialized to equal
   \alib{characters;Policy::Implicit}, then \b BoxTraits is specialized to enable
   boxing and unboxing of the string type.<br>
   The latter of both conditions, namely the "implicit construction", indicates to \alib_boxing_nl
   that the string type is a "lightweight type" that can be unboxed with no effort.<br>
   Samples for such types are C++17 type \c std::string_view, \alib type \alib{strings;TString;String}
   or \b QStringView.
2. If the second condition is not met (field \alib{characters;ArrayTraits::Construction} does \b not
   equal \alib{characters;Policy::Implicit}), then the type is
   \ref alib_boxing_bijective_nonsurjective "locked" and cannot be unboxed from character arrays.<br>
   Character array types that are not implicitly constructible, usually are "heavy types" which, for example, allocate
   memory and copy given string data when constructed. If such types are to be created from boxed
   character arrays, this has to be done using an \e explicit constructor invocation and passing
   either an unboxed "lightweight type" or the result of methods \alib{boxing;Box::UnboxArray} and
   \alib{boxing;Box::UnboxLength}. <br>
   Samples for locked types are \c std::string, \alib type \alib{strings;TAString;AString}
   or \b QString.

As a result, to customize boxing for a custom string type, it is recommended to specialize
\alib{characters;ArrayTraits} instead of \b BoxTraits.

While it is still possible to use \b BoxTraits for customization, the advantage of the recommended
approach is obvious: generally announcing the custom type to be of character array type enables
it's use with module \alib_strings_nl as well as with boxing. Also other modules and
software built on \alib might directly benefit from such type-traits.

In the unlikely case that \b ArrayTraits is specialized and still \b BoxTraits should be specialized
(with the aim to provide a certain customization that is different from the one that this module
automatically provides if \b ArrayTraits is given), then, to avoid ambiguities, the helper-type trait
\alib{boxing;SuppressCharArrayBoxingTraits} may be specialized to inherit \c std::true_type.
As its name says, a specialization of this type disables the automatic custom boxing and hence
allows a specialization of \b ArrayTraits and a parallel specialization of \b BoxTraits.

\attention
  <b>Header Inclusion Order:</b><br>
  The use of "underlying" module \alib_characters_nl and its type trait
  \alib{characters;ArrayTraits} to specialize \alib{boxing;BoxTraits} for a whole set of types at
  once, imposes the requirement of keeping the right <b>header inclusion order</b> (respectively
  C++20 Module import) order: Any specialization of \b ArrayTraits of module \alib_characters
  (which included with \implude{Lang}) that is to be announced to a higher level
  compilation unit like \alib_boxing_nl, has to be made \b before the conditional customization
  of struct \b BoxTraits is given.<br>
  In practice, this means that any specializations of \b ArrayTraits have to be made right
  after the inclusion of \implude{Lang}, and before the inclusion of \implude{Boxing}!

\I{################################################################################################}
## 10.3 Box-Function FAppend ##{#alib_boxing_strings_fappend}

With the inclusion of module \alib_strings in the \alibbuild, built-in box-function
\alib{boxing;FAppend} becomes available.


Class \alib{strings;TAString;AString} supports a concept-based mechanism to append objects of
arbitrary type.
This is documented with chapter \ref alib_strings_assembly_ttostring of the Programmer's Manual
of module \alib_strings.

Of course, if an object of type \b Box is "appended", then compile-time information about the boxed
data is lost. (It was lost when the box was created.)
Consequently, the box-function \b FAppend is needed that performs the job in a virtual fashion.
If a box is appended to an \b AString, simply this function is called.

For all types which already specialize functor \alib{strings;AppendableTraits}, a templated
implementation of this function can be used: This unboxes the template type and appends it.
This template function is provided with static member \alib{boxing;FAppend::Appendable}.

As a result, there are two ways of implementing interface \b %FAppend for a custom boxable type:
1. As with other boxing interfaces: Provide a custom implementation.
2. Make the type \ref alib_strings_assembly_ttostring "appendable" to class \b %AString and then
   register \alib{boxing;FAppend::Appendable} for the mapped type.

The second approach has the advantage, that the custom type is directly appendable to objects of
class \b %AString - independent of boxing. Therefore, this is the recommended option.

\note
  With module \alib_format, a next string-related box-function \alib{format;FFormat}
  becomes available, which allows controlling the string conversion of boxed values by the use of a
  format string.<br>
  Information and a sample implementation about both, \b FAppend and \b FFormat, is provided in
  chapter \ref alib_format_custom_types of the
  \ref alib_mod_format "Programmer's Manual" of that module.

\I{################################################################################################}
\I{##################     11. Class TBoxes and Variadic Function Arguments     ###################  }
\I{################################################################################################}
# 11. Variadic Function Arguments and Class TBoxes # {#alib_boxing_boxes}

\I{################################################################################################}
## 11.1 Variadic Function Arguments ## {#alib_boxing_boxes_variadic}

With class \alib{boxing;Box} in place, it becomes possible to define functions and methods that take
an arbitrary value as an argument. The need for this is often combined with the need to allow an
arbitrary number of such arbitrary arguments. C++ 11 introduced
\https{variadic template arguments,en.cppreference.com/w/cpp/language/variadic_arguments} for this.

Class \b %Box might greatly simplify the use of this language feature and provide a \b type-safe
and \b indexed way to access variadic arguments.
(In fact, this was one of the original goals for creating module \alib_boxing_nl!)

The following quick sample demonstrates this:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_VARIADIC_SAMPLE

With this function definition, it can be called like this:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_VARIADIC_SAMPLE_1

It is only a single, simple line of code that fetches all function parameters and puts them
into an array of boxes.

Of course, the classical recursive approach to process template arguments using class \b Box may
also be implemented but avoiding the recursion makes the code easier and more readable.

The sample above can be slightly modified to use
\https{C++ 11 Perfect Forwarding,en.cppreference.com/w/cpp/utility/forward} which in some
situations is a little more efficient and produces smaller code. The following code snippet
uses this technique and may be copied as a recipe on how to implement variadic template
functions with \alib_boxing_nl:

\snippet "DOX_BOXING.cpp"     DOX_BOXING_VARIADIC_RECIPE


\I{################################################################################################}
## 11.2 Class TBoxes ## {#alib_boxing_boxes_boxes}
In the previous chapter it was demonstrated how simple the use of variadic template arguments
becomes with \alib_boxing_nl. The recipe given, uses a single line of code to let the compiler
create an array of objects of class \b Box.
This is sufficient in many cases, but obviously using container class \c std::vector<alib::Box>
instead of a simple array would give more flexibility: It allows adding and removing boxes from the
array and to pass the array to other (non-templated functions) without passing its size in an
extra parameter.

For this and more purposes, templated class \alib{boxing;TBoxes} is provided. It publicly inherits
from \c std::vector<alib::Box> and introduces method \b %Add accepting templated variadic arguments.
This way, its use is as simple as this:

\snippet "DOX_BOXING.cpp"      DOX_BOXING_BOXES_SAMPLE_1

In this sample, five boxed objects are added to the container using method
\alib{boxing;TBoxes::Add}.

We replace the simple C++ array of the recipe given in the previous section by an object of
this type:

\snippet "DOX_BOXING.cpp"      DOX_BOXING_BOXES_SAMPLE_VARIADIC

The advantage of the former version is that the array was created on the \b "stack".
In contrast to this, class \alib{Boxes} uses dynamic memory to store an arbitrary amount
of boxes.<br>
Even more efficient is the use of type \alib{BoxesMA}, which performs only one single
allocation (as long as the list of boxes is not exceeding around 40 boxes, then a second allocation
would be performed). More on this topic of memory management is discussed in the next section.

\I{################################################################################################}

## 11.3 Memory Management ## {#alib_boxing_boxes_memory}
The previous chapter introduced class \b %TBoxes. It was said, that the class is derived from
<c>std::vector<Box></c>. This is not exactly true. In fact it is derived from
<c>std::vector<Box, lang::StdAllocator<Box, TAllocator>></c>. Together with the classes'
template parameter \p{TAllocator} and the \c std allocator type \alib{lang;StdAllocator},
different allocation strategies than just heap allocation can be implemented. Now, in case that
module \alib_monomem is included in the \alibbuild, the number of heap allocations can be
reduced or even eliminated.

The use cases for monotonic allocation mode are described with module \alib_monomem and not repeated
here, besides the following hint: Should the given \b MonoAllocator be
\alib{monomem;MonoAllocator::Reset;reset}, and the \b %TBoxes instance not be destructed but continued
to be used, then the instance has to be "reset" as well. This is done by performing a C++
placement-new, as \ref alib_ns_monomem_stdma_reset "described here".

\I{################################################################################################}
## 11.4 Advanced Usage of Class %TBoxes ## {#alib_boxing_boxes_advanced}

Besides providing variadic template arguments, method \b %TBoxes::Add uses some template meta
programming to "flatten" the array in the case that another instance of class \b %TBoxes is added.
In other words, if an instance of class \b %TBoxes is passed to \b %TBoxes::Add, the boxes contained
in this instance are copied into the destination vector! Due to this fact, when using sample
method \b VariadicFunction from above, the invocation:

\snippet "DOX_BOXING.cpp"      DOX_BOXING_BOXES_SAMPLE_VARIADIC_CALL_1
produces the following output:
\verbinclude "DOX_BOXING_BOXES_SAMPLE_VARIADIC_CALL_1.txt"

The reason why this is implemented like this, is that the user of a method has a next benefit:
He/she has the freedom of choice to either pass all parameters just inside the function
call or to collect all objects before the call in an own instance of class \b %TBoxes and then just
pass this instance as a single argument - even together with other, fixed arguments.

This makes the use of the function more flexible, without the need of providing an overloaded
version that accepts and processes an object of \b %TBoxes directly.

Finally, besides detecting objects of class \b %TBoxes inside method \b %TBoxes::Add, it is also
detected if an object of class \b %TBoxes is passed as a boxed object.
Let us first look at a sample and its result:

\snippet "DOX_BOXING.cpp"      DOX_BOXING_BOXES_SAMPLE_VARIADIC_CALL_2
\verbinclude "DOX_BOXING_BOXES_SAMPLE_VARIADIC_CALL_2.txt"

Looking at this sample a reader might think "Wow, this is cool, but where is the use case for this?".
Generally spoken, this is useful when a method has several overloaded versions with different
parameters, and still should support to accept an arbitrary amount of any type of arguments.
In this moment, it might get quite complicated (or impossible!) to define the methods properly in
the sense that no ambiguities may occur when invoking them.
A solution here is to declare the method to accept just exactly one <c>const %alib::Box&</c>
argument instead of a variadic list of arguments.

If inside the method this box is passed into a local instance of class \b %TBoxes,
a user might invoke the method with just a single argument of arbitrary type (which gets boxed),
or with an arbitrary amount of arguments, by collecting those in class \b %TBoxes. This might be
done right in the invocation.<br>
To demonstrate this, we use the method from above, but instead of accepting variadic template arguments,
it accepts now just a single argument of type <c>const %Box&</c>:

\snippet "DOX_BOXING.cpp"      DOX_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE

This can be invoked as follows:

\snippet "DOX_BOXING.cpp"      DOX_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE_1

...which produces:

\verbinclude "DOX_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE.txt"

A real world sample can be found in the logging library \ref alib::lox "ALox" which is
built on \alib and makes a lot of use of \alib_boxing_nl.
While straightforward methods
\ref alib::lox::Lox::Info    "Lox::Info",
\ref alib::lox::Lox::Verbose "Lox::Verbose", etc. accept variadic template arguments as objects
to be logged, method
\ref alib::lox::Lox::Once "Lox::Once" is more complicated: Various overloaded versions exist
that interpret the term "once" differently. Therefore, each overloaded version accepts only one
object to log - which at the first sight is only suitable to accept a simple log message string.
But internally, a \b %TBoxes instance is created and this way, multiple objects can be passed just
as with other interface functions.

As a final note, besides "flattening" a boxed instance of class \b %TBoxes,
method \alib{boxing;TBoxes::Add} will do the same with a "boxed array of boxes".
Hence the following code:

\snippet "DOX_BOXING.cpp"      DOX_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE_2

produces:

\verbinclude "DOX_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE_2.txt"


\I{################################################################################################}
\I{#########################     12. Further Topics And Details     #############################  }
\I{################################################################################################}
# 12. Further Topics And Details # {#alib_boxing_more}

\I{################################################################################################}
\I{#########################        12.1 Void And Nulled Boxes      #############################  }
\I{################################################################################################}
## 12.1 Void And Nulled Boxes ## {#alib_boxing_more_void}

\I{################################################################################################}
### 12.1.1 Void Boxes {#alib_boxing_more_void_void} ###
Default-constructed instances of class \b Box or those constructed passing keyword \c nullptr
as an argument, do not contain a boxed value.
Technically this means, that no \b VTable singleton is set, because \b VTables only exist for
mapped types.

To test if a box <em>"is void"</em>, aka does not contain a value, a test for type \c void is to be
performed by invoking \alib{boxing;Box::IsType;Box::IsType<void>}.<br>
As soon as anything else but \c nullptr is boxed (with construction or assignment), the instance
loses its void state.
Vice versa, by assigning keyword \c nullptr, a box is "reset" to void state.

The following methods are allowed to be called on void boxes:
- All type guessing functions, like \alib{boxing::Box;IsType} or \alib{boxing::Box;IsArray},
  also including \alib{boxing::Box;IsSameType}. However, the latter returns \c false, even if both
  boxes are in void state.<br>
  Method \alib{boxing;Box::TypeID} returns <c>typeid(void)</c> if a box is not initialized.
- Box-function methods, including \alib{boxing::Box;Call}, \alib{boxing::Box;CallDirect},
  and \alib{boxing::Box;GetFunction}.
  Nothing will be invoked with these methods, also no default methods.
- Consequently also all interface methods and operators that internally call a box-function, namely
  - \alib{boxing;Box::IsNull}
  - \alib{boxing;Box::Hashcode}
  - \alib{boxing;Box::Clone}
  - \alib{boxing;Box::operator==}
  - \alib{boxing;Box::operator!=}
  - \alib{boxing;Box::operator<}
  - \alib{boxing;Box::operator<=}
  - \alib{boxing;Box::operator>}
  - \alib{boxing;Box::operator>=}


\b Forbidden methods that produce undefined behavior if invoked, are:

- Method \alib{boxing::Box;Unbox} and related methods \alib{boxing::Box;UnboxArray},
  \alib{boxing::Box;UnboxElement}, \alib{boxing::Box;UnboxLength} or \alib{boxing::Box;Data}.
- Methods \alib{boxing::Box;ArrayElementSize} and \alib{boxing::Box;GetPlaceholderUsageLength}.

In debug-compilations these methods raise a run-time assertion when invoked on a void box.
Most of the times an explicit test on whether a box is void is still not necessary, because
unboxing is only allowed after successful type guessing.

The void state constitutes a piece of information that might be used in APIs.


\I{################################################################################################}
### 12.1.2 Nulled Boxes {#alib_boxing_more_void_nulled} ###
Very different from the attribute of a Box being \e void, is the attribute of being \e nulled.
The latter applies only to non-void boxes. In theory, the \e nulled-state of a box is undefined
if no value is boxed.

Whether a box is \e nulled or not is evaluated using built-in box-function \alib{boxing;FIsNotNull},
which is invoked on the box with methods \alib{boxing;Box::IsNull} and \alib{boxing;Box::IsNotNull}.

\alib_boxing_nl is tolerant in respect to calling box-functions on \e void boxes.
Calling \b FIsNotNull on a \e void box returns the default value of the type \c bool, which is
\c false.
This way, boxes that do not contain a value appear to be \e nulled, which is appropriate
behavior with most use cases.

Default implementations of \b FIsNotNull for fundamental types return \c true, as such types
are not considered \e nullable. The default implementation returns \c false (\e nulled), for
\alib{boxing;Box::IsArray;array types} that have a length of \c 0 and for
\alib{boxing;Box::IsPointer;pointer types} that have value \c nullptr.
Otherwise the default implementation returns \c true (not \e nulled).


\I{################################################################################################}
\I{#########################    12.2 Optimizations With Static VTables    #######################  }
\I{################################################################################################}
## 12.2 Optimizations With Static VTables ## {#alib_boxing_more_opt_staticvt}

\I{################################################################################################}
### 12.2.1 Introduction ### {#alib_boxing_more_opt_staticvt_intro}
Using class \b Box to pass data between code entities, causes a certain amount of "effort",
which has an impact on the code size and the execution performance.

Before it is explained how to minimize this effort, the following important note is to be
made:

\note
  Disclaimer for this chapter:
  1. Without custom optimization (described below), the built-in mechanics of
     \alib_boxing_nl performs very well already.
     In most use cases, doing so is not worth the effort.
     Generally, a programmer can skip reading this chapter and leave things as they are.
     In older versions of this library, the optimization was not even possible.
  2. For C++ fundamental types and many \alib types, such optimization is already built-in.
  3. To keep this and the next manual section short, technical details are postponed to
     a \ref alib_boxing_more_opt_staticvt_bg "third section" of this chapter only.


The processes of boxing, type guessing and unboxing should be implemented in a fast
and lean code. The three share two actions:
1. Identifying the mapped type: this is done with boxing and type guessing.
2. Copying the data: this is done with boxing and unboxing.

Point two is a matter of the implementation of struct \alib{boxing;BoxTraits}.
If the default for methods \b Write and \b Read can be used, this is implemented most efficiently
and cannot be optimized. What is left is point one. This in turn is split into three steps:
1. Decide about the mapped type.
2. Retrieve a corresponding singleton of struct \b VTable.
3. Store the singleton in the box (used with boxing) or compare the singleton with the stored one
   (used with type guessing).

The good news is, that step one is performed at compile-time and this way has no run-time effects.
Step three is a most efficient simple pointer assignment, respectively comparison.<br>
As a result, the only point that leaves room for optimizations is with step two, retrieving the
\e vtable singleton.
If it is done, then retrieving the singleton is nothing else than a single direct memory access.

For example, "boxing a value" is compiled to nothing more than just filling (all or a part
of) the 24 bytes (respectively 12 bytes on a 32-bit platform) with values that the CPU can
simply load from other memory addresses!<br>
As mentioned above, the impact of not performing the optimization for a mapped type, is described
in the section \ref alib_boxing_more_opt_staticvt_bg.

\I{################################################################################################}
### 12.2.2 Declaration, Definition and Registration Of Static VTables ### {#alib_boxing_more_opt_staticvt_howto}

The goal of the optimization is to provide a \b named singleton object for the \e vtable of a
mapped type.
To do so, three simple steps are involved. As optimizations for all fundamental types are already
built into the library, the library code itself, namely that used for types \c bool and
\c char[], are used as a sample.

#### 1. Declaration of the vtable ####
Named singletons of struct \alib{boxing;detail::VTable} have to be declared in a header-file.
For this, macros
- \ref ALIB_BOXING_VTABLE_DECLARE and
- \ref ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE

are to be used. For types \c bool and \c char[], the internal (always included) header-file
\b alib/boxing/boxingcustoms.inl states:

\snippet "boxing/boxingcustoms.inl.inl"    DOX_BOXING_OPTIMIZE_DECLARE_1
\snippet "boxing/detail/vtable.inl"               DOX_BOXING_OPTIMIZE_DECLARE_2

Besides the mapped type, a second parameter specifies a valid and unique C++ identifier name.

\attention
  Likewise with boxing customization (the provision of specializations of the type trait
  \alib{boxing;BoxTraits}) it is mandatory, that each compilation unit that boxes, guesses,
  or unboxes the mapped type, needs to "be aware" of that optimization.
  It is recommended to place the optimizations in the in the same header-file as the
  customizations and make sure it is always included.
  Otherwise, the result is undefined behavior!<br>
  \alib_boxing_nl will give a run-time assertion in debug-compilations if a code unit
  misses such inclusion.

#### 2. Definition of the vtable ####
The singleton objects have to be defined in a <b>compilation unit</b> (e.g., cpp-file).
Corresponding macros
- \ref ALIB_BOXING_VTABLE_DEFINE and
- \ref ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE

are used:

\snippet "boxing/detail/boxingcustoms.cpp"    DOX_BOXING_OPTIMIZE_DEFINE_1
\snippet "boxing/detail/boxingcustoms.cpp"    DOX_BOXING_OPTIMIZE_DEFINE_2

The macro parameters are the very same as for the declaration.

#### 3. Registration of the vtable ####
This final step is needed <b>only in debug-compilations</b>.
Consequently, macro
- \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER

(which is used to register both, non-array-type and array-type \e vtables) is empty when
compiling a release-version.

Similar to the \ref alib_boxing_functions_concepts_reg "registration of box-function implementations",
the registration of static \e vtables has to be performed with the
\ref alib_mod_bs "bootstrap code" of software. It is a good
idea to place the macros to the same bootstrap section, where function registrations are done.

In our sample, this looks as follows:

\snippet "boxing/boxstrap.cpp"    DOX_BOXING_OPTIMIZE_REGISTER_1
\snippet "boxing/boxstrap.cpp"    DOX_BOXING_OPTIMIZE_REGISTER_2

The registration done in debug-compilations has two effects:
1. It helps to detect if two or more code units use the mapped type in conflicting ways: some with
   the declaration of the static table, while others without it.
2. It allows enumerating the overall known \e vtables, when creating debug status information
   about \alib_boxing_nl, as documented with chapter \ref alib_boxing_more_debug.
   In turn, the use of such debug options helps <b>to identify vtable candidates</b> that might
   benefit from optimization, as this also lists the overall usage of a type.

\note
  Although registration is purely introduced and performed for debug-purposes, it is mandatory
  to be done in debug-compilations. Methods \alib{boxing;Box::Unbox}, \alib{boxing;Box::UnboxArray}
  and \alib{boxing;Box::UnboxElement} will assert if a non-registered \e vtable is used with the current
  box.

This is all that is needed to do. With that, \alib_boxing_nl is as fast as technically
possible. The penalty of the use of boxes is marginalized in both respects: code size and
execution performance.

\I{################################################################################################}
### 12.2.3 Technical Background On VTables ### {#alib_boxing_more_opt_staticvt_bg}

#### Creating Strict Singleton VTables: ####
What the \https{vtable is in C++,en.wikipedia.org/wiki/Virtual_method_table}, is
struct \alib{boxing;detail::VTable} for \alib_boxing_nl.
Both are strict singletons, which means that two objects of the same mapped type share a pointer
to the same \b vtable and that for each mapped type only one instance exists.

At compile-time, when an object is boxed, the right singleton has be chosen and stored together with
the object's data in the box.
The small challenge now is to find a way of how to define a singleton for the endless amount of
types that can be mapped? The solution is done with a simple trick: An otherwise empty template class
\alib{boxing;detail::VTableUnoptimized} is derived from \b VTable. In parallel this template class is also
derived from \alib class \alib{singletons;Singleton}. Two template parameters are specified,
\p{TMapped} and \p{TIsArray}.
(Note that these are the same as found in traits-struct \alib{boxing;BoxTraits} and this way
uniquely specify a mapped type.)

If the \e vtable was \b not optimized (as shown in the previous section), then the static
method \alib{singletons;Singleton::GetSingleton} is invoked on type \b VTableUnoptimized:

    VTableUnoptimized<TMapped, TIsArray>::GetSingleton()

This gives the constructors of class \b Box the strict singleton object it needs to store.

Now, to allow optimizations, class \b Box does not perform the retrieval of the right singleton
directly. Instead, it is done indirectly through a next type trait, namely
\alib{boxing;VTableOptimizationTraits}.
Only its default implementation (in the non-optimized case)  acts as described above.
Specialized versions for its template parameters \p{TMapped} and \p{TIsArray} may return an
\c extern dedicated object (which is DLL-exported under WindowsOS).

The macros \ref ALIB_BOXING_VTABLE_DECLARE and \ref ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE
declare such singleton and at the same time specialize the factory-function for the given mapped
type to return this instance.
Those are to be placed in header-files. The definition of the static <em>vtable</em>-instance
is performed in a compilation unit usign macro \ref ALIB_BOXING_VTABLE_DEFINE.
Finally, to support debugging facilities, macro \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER
needs to be placed in the bootstrap section of your program. The latter is empty in release builds.

#### Impact of non-optimized vtables: ####
The final technical question is now: what negative impact does the use of class
\alib{singletons;Singleton::GetSingleton} have? As type \b Singleton has to be
templated, it's construction has to be performed inline. The same is the case with
struct \b VTableUnoptimized which derives both \b VTable and the singleton. The first thing that
\b Singleton::Get does is to check whether the singleton was already created by an earlier call.
If yes, it is instantly returned. If not, construction has to be performed. Although the latter
is done only once, each time a value is boxed, the whole (inlined) code has to be added to
the construction. Therefore, the impact on code size is rather high, while the execution
performance - from the second invocation on - suffers from only a marginal penalty.

On Windows OS, with the use of \e DLLs, things become even a little more complicated.
This is the main reason for the existence of dedicated \alib helper-class \b Singleton.
Different DLLs and the main process that loads them, do not share one data segment. Because of
this, before a singleton is created, a check has to be made whether the same singleton was created
already in a different data segment. Of course, such check needs to avoid race conditions and
therefore uses a semaphore. Luckily, this code does not need to be inlined.<br>
Note that this \e "DLL-problem" does \b not apply for the optimized, static \e vtable objects.
Here, a definition can be used in a distinct compilation unit, that the process and the DLLs share.

\attention
  If static (global) instances of class \alib{boxing;Box} should be defined which are initialized
  by the non-structured initializer-code of C++, then optimization of the corresponding
  \e vtable becomes mandatory, if module \alib_singletons works in "mapped mode".<br>
  The latter is usually \b true on WindowsOS when working with DLLs.<br>
  The reason for this is that the hashtable access of method \b Singleton::GetSingleton caused by
  the initialization of a global instance of class \b Box is undefined behavior, because it is not
  guaranteed that the likewise global hashmap is yet initialized at this point in time!

More details on this topic are found with the \ref alib_mod_singletons "Programmer's Manual" of
the module \alib_singletons_nl.
                  


#### Management Of Boxed Functions: ####
With field \alib{boxing::detail;VTable::Functions}, each \e vtable embeds struct
\alib{boxing::detail;FunctionTable} which is responsible to store and retrieve implementations of
box-functions. Furthermore, one dedicated instance of this type is defined in the namespace
to store the default implementations.

Methods \alib{boxing::detail;FunctionTable::Set} and \alib{boxing::detail;FunctionTable::Get}
use concept-based overload mechanics by their template type \p{TFDescr}.
For the built-in functions \b FClone, \b FEquals, etc, a direct access to a corresponding pointer
member is performed.

For registered custom functions, a global hash table is used that maps the function table and
the function type to the function's implementation. Besides the hash table access needed,
in addition a mutex is acquired to protect the global hash table against concurrent access.

\I{################################################################################################}
\I{########################    12.3 Optimizations With Constexpr Boxing    ######################  }
\I{################################################################################################}
## 12.3 Optimizations With "constexpr"-Boxing ## {#alib_boxing_more_opt_constexpr}
Under certain conditions, instances of class \b Box are \c constexpr values.
For example, the following code compiles without an error:

\snippet "DOX_BOXING.cpp"                    DOX_BOXING_CONSTEXPR_TRYDEBUG

While the typical use cases of \alib_boxing_nl do not raise the requirement to be able to
define \c constexpr \b Box variables by users of the library, still there is some advantage of
\c constexpr boxes with the possibility for the compiler to optimize the object code.
In addition, such box instances objects may be placed in the data segment of an executable,
that is residing in read-only memory (e.g., embedded systems).

\note
  The latter (storing \c constexpr boxes in read-only memory) imposes the only mandatory
  rationale for this type of optimization that we can think of so far.
  For other purposes, it is very questionable if the result is worth the effort and a reader might
  skip the following explanations.

### Requirements ###
The C++ rules for creating \c constexpr objects imposes that the constructor of class \b Box
that is chosen according to a given argument type \p{TBoxable}, is implemented \c constexpr.
The constructor creates two field members, the \e vtable and the \alib{boxing;Placeholder}.
Consequently, the creation of both objects need to be implemented \c constexpr.

\note
  From the sample above it can be told that obviously both requirements are met with
  a) \e vtables of mapped character array type, and
  b) boxable type <em>C++ String Literal</em>.

<b>1. Static VTable:</b><br>
For the \e vtable to meet the requirement, the optimization discussed in previous chapter
\ref alib_boxing_more_opt_staticvt has to be performed and thus is the first mandatory requirement
to enable \c constexpr boxes is to implement what is described in this chapter for the
mapped type in question.
\attention
  This first requirement is only needed, in case a \alib and your software is run under Windows OS,
  using Windows DLL and thus the implementation of module \alib_singletons is forced to use
  feature \ref ALIB_FEAT_SINGLETON_MAPPED. On GNU/Linux and other OSes, this first requirement
  is not needed to be met.
     

<b>2. Constexpr Definition of BoxTraits::Write:</b><br>
The second requirement of creating the \b Placeholder in a \c constexpr way, can be
achieved with some of the overloaded versions of the method \alib{boxing;BoxTraits::Write}.
Fortunately, with C++20 the restrictions got further relaxed in respect to C++ Unions, which
class \b Placeholder uses in an even nested fashion.<br>

Please refer to the reference documentation of class \alib{boxing;Placeholder} for further
information on your options of implementing a custom \b Write method that satisfies \c constexpr
compilation.

### Built-In Behavior ###
The following rules apply for different types:

- <b>Void, void* And Nulled Boxes:</b><br>
  \ref alib_boxing_more_void "Void and nulled boxes" are \c constexpr.

- <b>Fundamental Types:</b><br>
  All C++ fundamental types can be created \c constexpr (built-in adoptions)

- <b>Array Types:</b><br>
  If boxing is performed as an array type, the following applies:
  - For character arrays, a static \e vtable is defined. For other element types, a definition
    has to be performed
  - If boxed from a C++ array (which is non-customizable boxing) boxing is performed \c constexpr.
  - With all \alib string-types (like \b AString), boxing is performed \c constexpr.
  - Custom types need to provide a \c constexpr specialization of \b BoxTraits::Write as well
    as a static \e vtable.

- <b>Enum Types:</b><br>
  Boxing of all enum types is performed \c constexpr. Consequently all that is needed is
  to define a static \e vtable for the mapped enum type.<br>
  For all major enum types of \alib, such static \e vtable is defined.

- <b>Pointer Types</b><br>
  Types boxed as pointers are boxed in a \c constexpr fashion by default. Hence the
  only precondition is to define a static \e vtable for the type.


\I{################################################################################################}
\I{###############    12.4 Global And Static Box Instances And Their Initialization     ########  }
\I{################################################################################################}
## 12.4 Global And Static Box Instances And Their Initialization ## {#alib_boxing_more_static_instances}

Instances of class \alib{boxing;Box} may generally exist as global data or static members as long
as they not initialized with a boxed value.

If a default-initialization should be given (and if WindowsOS/DLL use is required), then the
resulting mapped type's \e vtable has to be statically defined as described in chapter
\ref alib_boxing_more_opt_staticvt.
The reason for this is, that dynamically created \e vtables are using the mechanics implemented
with \alib type \alib{singletons;Singleton}. To achieve the creation of process-wide "true"
singleton objects, this class uses a globally defined hash-map that in case of a first creation
within a compilation unit might be used to receive one already created in another compilation unit.
The technical background for this is explained with module \alib_singletons. In short, the
problematic platform here is WindowsOS, which allows a \b DLL to have an own global data segment.

Because the sequence order of initialization of global objects is not defined with the C++ language,
it cannot be ensured that the hash-map is already initialized when the singleton \e vtable of
an initialized global or static box is required.

As it was documented in chapter \ref alib_boxing_more_opt_staticvt, for all fundamental types
as well as for character arrays, a static \e vtable implementation is always in place.
Therefore, global or static boxes may well be initialized with values of these types.
If a custom type is to be used for initialization, a static \e vtable has to be given.

In debug-compilations, the use of dynamic \e vtables with global or static instances of class \b Box
\ref alib_mod_assert "raises an error".

\I{################################################################################################}
\I{##########################    12.5 Bootstrapping This Module        ##########################  }
\I{################################################################################################}
## 12.5 Compilation, Header Inclusion And Bootstrapping ## {#alib_boxing_more_bootstrap}

\I{################################################################################################}
### 12.5.1 Compilation Options ### {#alib_boxing_more_bootstrap_compilation}

The following \ref GrpALibPreproSymbols are provided by this \alibmod_nl:

- \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
- \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
- \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS

\see
  Chapter \ref alib_manual_build "6. Building The Library"
  of the \ref alib_manual "ALib Programmer's Manual".



\I{################################################################################################}
\I{###########################     12.6 Life-Cycle Considerations     ############################  }
\I{################################################################################################}
## 12.6 Life-Cycle Considerations ## {#alib_boxing_more_iclone}

With \alib_boxing_nl, \b no mechanisms are in place that link the life-cycle of boxes with
their boxed values. Class \alib{boxing;Box} does not even have a destructor defined!
This is a huge difference to C++17 class \c std::any.<br>
It is completely left to the user of the library to make sure that any pointer or data that
otherwise references values available during boxing, are still intact and available when unboxed
and vice versa, that allocated objects that become boxed are de-allocated after a box that
refers to them is disposed.

In many use cases, this is absolutely no problem: Often, \alib_boxing_nl is used to
implement generic (and \ref alib_boxing_boxes "optionally variadic") function arguments.
If those are then used inside the function only and not stored otherwise, the access to
boxed data is safe. A prominent sample for this use case is given with
\ref alib_boxing_appendix_usecases_format "appendix chapter C.1".

However, other use-cases might introduce the need to use boxed data out of the scope that
boxed the data. A good sample for this is given with appendix chapter
\ref alib_boxing_appendix_usecases_exceptions. Objects of type \b Exception carry exception
arguments while the function-call stack is "unwinded". Hence, all locally defined objects
are destructed and get out of scope.

In this and similar cases, a user of the library has to ensure that boxes of mapped types whose
data might become corrupted, are either not unboxed or the data is copied before having the box
leaving the scope.
A nice way to perform such copying is provided with built-in box-function \alib{boxing;FClone}.
Its default implementation copies the data of boxed arrays.

Depending on the use-case, the concept of "cloning" does not need be taken too literally, because
function \b FClone might take other actions as well. Implementations are allowed to overwrite the
given box's content, hence including to change the mapped type of the box!
Often it is enough to create some sort of representation of an object, for example, just an
ID or another sort of key value. In the mentioned use case of exception handling, sometimes just
a string representation of an object might be created, which is later used for assembling a human
readable formatted log output.



\I{################################################################################################}
\I{################################     12.7 Debug Helpers     ##################################  }
\I{################################################################################################}
## 12.7 Debug Helpers ## {#alib_boxing_more_debug}

\I{################################################################################################}
### 12.7.1 Available Debug Objects And Fields ### {#alib_boxing_more_debug_general}

In debug-compilations, the compiler-symbol \ref ALIB_DEBUG_BOXING may be set.
With it, following entities become available:
- Namespace \alib{boxing::debug} and its functions.
- Method \alib{boxing;Box::DbgGetVTable}
- Field \alib{boxing::detail;FunctionTable::DbgCntInvocationsFClone;FunctionTable::DbgCntInvocationsXYZ}
- Field \alib{boxing::detail;VTable::DbgProduction}
- Field \alib{boxing::detail;VTable::DbgCntUsage}
- Function \alib{boxing;debug::DbgCheckIsInitialized}
- Function \alib{boxing;debug::DbgRegisterVTable} along with macro
  \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER
- Function \alib{boxing;debug::DbgCheckRegistration}

Together, this discloses all information necessary to investigate into the built-in and default
behavior of \alib_boxing_nl. Please consult the reference manual of the named types,
for further details.


\I{################################################################################################}
### 12.7.2 Namespace alib::boxing::debug ### {#alib_boxing_more_debug_dbgboxing}
Instead of using the methods and objects listed above, namespace \alib{boxing::debug} provides
a more handy alternative at the moment that module \alib_format is included in the \alibbuild.
The namespace then offers additional functions that collect and format various sorts of information.

For details, please consult the namespaces \alib{boxing;debug;reference documentation}.
In this Programmer's Manual, we just want to provide some sample invocations.

#### Showing The Mapped Type: ####
If a programmer is unsure, which mapped type results from boxing, all that is needed to do is to
pass a "sample box" to function \alib{boxing::debug;TypeName}:

\snippet "DOX_BOXING.cpp"                    DOX_BOXING_DEBUG_TYPENNAME
\verbinclude "DOX_BOXING_DEBUG_TYPENAME.txt"

#### Detailed Info On Boxable And Corresponding Mapped Type: ####
A next, quite powerful function is \alib{boxing::debug;TypeInfo}. It provides all information
on a boxable type \p{TBoxable} and its a mapped type.<br>
The boxable type needs to be provided as a template parameter \p{TBoxable}.
If it is not default-constructible, a corresponding the sample box has to be provided as well.
To stay with sample above, to get information for mapped type \c char[], one possible \p{TBoxable}
is \ref alib::String "alib::String". With that, the invocation looks like this:

\snippet "DOX_BOXING.cpp"                    DOX_BOXING_DEBUG_TYPEINFO
It produces the following details:
\verbinclude "DOX_BOXING_DEBUG_TYPEINFO.txt"

For readers of this manual, all information should be easily understandable. Line \b "Usage Counter"
provides the quantity of unboxing operations and function invocations that have been performed
on boxes of the mapped type so far.
The value also depends on the point in time of a process's life-cycle the function was invoked.
If this value indicates a high usage and line <b>"VTable Type"</b> denotes a dynamically created
\e vtable type, it might make sense to \ref alib_boxing_more_opt_staticvt "define a static vtable"
for that mapped type.

Likewise, with each specialized box-function, the number of its invocations is given in brackets
behind their names.

#### Listing All Known Box-Functions: ####
To get a list of just all box-functions, that either a defaulted or one or more specialized
implementation has been registered for, the following code can be used:
\snippet "DOX_BOXING.cpp"                          DOX_BOXING_DEBUG_KNOWNFUNCTIONS
Resulting to:
\verbinclude "DOX_BOXING_DEBUG_KNOWNFUNCTIONS.txt"

For those box-functions that dispose over an associated default implementation, the number of
invocation of that default is given in brackets.


#### Listing Mapped Types With Static/Dynamic VTables: ####
To list just all types that a dynamic \e vtable is created for (and therefore could be
\ref alib_boxing_more_opt_staticvt "optimized"), the following line of code can be used:

\snippet "DOX_BOXING.cpp"                    DOX_BOXING_DEBUG_DYNAMICTABLE
Here is the list:
\verbinclude "DOX_BOXING_DEBUG_DYNAMICTABLE.txt"

If \c true was passed to \alib{boxing::debug;DumpVTables}, then those with static tables
had been given. A second, default boolean parameter can be used to trigger the list of
specialized functions with each \e vtable listed.

#### Getting A Quick Overview: ####
To finish this chapter, function \alib{boxing;debug::DumpAll} is invoked, which aggregates much of
the above.<br>
The following shows the invocation and a possible corresponding output:

\snippet "DOX_BOXING.cpp"                    DOX_BOXING_DEBUG_DUMPALL

\verbinclude "DOX_BOXING_DEBUG_DUMPALL.txt"

Note that the output include types that we have used during this tutorial.
This is due to the fact that the unit test that produce this manual's output run all in one process
and are run in the order of the chapters.


\I{################################################################################################}
\I{###########################     12.8 Performance Considerations     ############################  }
\I{################################################################################################}
## 12.8 Performance Considerations ## {#alib_boxing_more_performance}

The reason why the effort of implementing this library is needed is the C++ language design
principle to be as performant and close to the hardware as possible. Other programming languages
are designed for other goals. For example, in languages \b Java or \b C#, the principle "everything is an object"
is (almost) implemented. In these languages, all instances (!) of class types have run-time type
information attached. In C++, only virtual classes have that.

And what happens in \b Java and \b C# when a plain, fundamental type is passed to a method that
expects an object? The corresponding compiler performs "auto-boxing" of the values to pre-defined
class types like \b Char, \b Integer or \b Double!

\alib_boxing_nl allows very similar things in C++.
Therefore , it is indicated to have do quick analysis of the memory and performance impact.
We do this in a rather loose order:

\I{################################################################################################}
### 12.8.1 A General Note On C++ RTTI ### {#alib_boxing_more_performance_rtti}

Due to the C++ language history, there is some confusion and wrong information spread in consideration
of run-time type information (\b RTTI), especially with programmers that have a long-term record
of C++ experience (because they probably went through the painful discussions of older days, which
freshmen did not).

Therefore , quickly some facts:
 - All standard compilers nowadays support RTTI and such support is not switched-off by default.
 - Such support has no influence on programs that do not use the feature. In other words, switching
   RTTI off (what some compilers still support!) makes no sense.
 - If RTTI is used in a compilation unit, only the code-lines that use it are affected.
 - The performance impact of using RTTI in C++ is extremely marginal,
   probably more marginal than in almost any other programming language.
   For each type that keyword \c typeid is used on, the footprint of an executable increases by
   the size of the corresponding \c std::type_info struct that the linker has to place in the data
   segment for that type.<br>
   The impact to get information on a type using keyword \c typeid is negligible.
   It is constant time, in \https{Big O notation,en.wikipedia.org/wiki/Big_O_notation} it is \b O(1).
   Keyword \c typeid just reads the pointer to a global struct residing in the data segment of an
   executable.


\I{################################################################################################}
### 12.8.2 VTables ### {#alib_boxing_more_performance_vtables}
For each mapped type, a singleton of a type \alib{boxing;detail::VTable} is created once.

This is again is negligible, even if no \ref alib_boxing_more_opt_staticvt "static vtable" is
declared for a mapped type. If it is, then the impact of using a mapped type is comparable to
the use of \https{C++ vtables,en.wikipedia.org/wiki/Virtual_method_table} which are created
by the compiler and included by the linker for each virtual C++ class used.

\I{################################################################################################}
### 12.8.3 Footprint Of Class Box ### {#alib_boxing_more_performance_footpring}
Class Box contains two members: A pointer to the vtable singleton and the data union \alib{boxing;Placeholder},
which consists of two "words".
For example, on a standard 64-bit platform a pointer and a word is 8 bytes wide, hence
an instance of class \b %Box on those platforms has a size of 24 bytes.
With many use cases, boxes are created in "stack memory" which allocates and deallocates in
zero time (yes, its less than "O(1)", it is just nothing).

Once created, to pass them to another function or store them in a container like
\alib{boxing;TBoxes}, these 24 bytes have to be copied.

While this is three times more than copying just a pointer, it might me much less effort in cases
that composite types automatically become boxed as pointers. If those had been passed for example
as variadic templated parameter, a deep-copy of the argument value had to be performed. With
\alib_boxing_nl, it is always only the 24 bytes.

\I{################################################################################################}
### 12.8.4 Construction And Destruction Of Class Box ### {#alib_boxing_more_performance_construction}
When a value is boxed, hence an object of class \b %Box is created, two things have to be done.
First the right \e vtable is identified. This is done using templated code that performs the
retrieval of a singleton inlined.

This rather tricky procedure is very efficient after it was done once for a type, but still the code
that nees to be inlined for the case that it is the very first request to a singleton is rather big.
This overhead in code-size can be optimized using
\ref alib_boxing_more_opt_staticvt "static vtables".
With such optimization, the effort is reduced to single copy operation of a pointer to a data
structure residing in the global data segment of an executable.

Secondly, the \alib{boxing;Placeholder} found with member \alib{boxing;Box::data} has to be set.
Again, this is mostly inlined template code and when compiled should be in most cases result
in one or two simple copy operations of pointers or fundamental C++ values.

Because the destructor of class \b Box is empty (as well as that of its embedded union
\alib{boxing;Placeholder} and in turn that objects' members), destruction of boxes is a no-op.


\I{################################################################################################}
### 12.8.5 Type Guessing ### {#alib_boxing_more_performance_typeguessing}
Template method
\alib{boxing;Box::IsType} compares the internal pointer to the singleton \e vtable with the that
singleton that would be chosen if the given type (the template parameter) was boxed.
Therefore, the impact is the same as boxing a value, minus the process of boxing data,
plus a pointer comparison.

Again, if \ref alib_boxing_more_opt_staticvt "optimized vtables" are used for the mapped type
resulting from the guessed type, method \b IsType is compiled to one simple inlined pointer comparison.

Template methods \alib{boxing;Box::IsArray} and \alib{boxing;Box::IsArrayOf} have to perform
an additional check for \ref alib_boxing_more_void "a void box", and then otherwise perform
a similar pointer comparison.

\I{################################################################################################}
### 12.8.6 Methods Box::GetFunction And Box::Call ### {#alib_boxing_more_performance_functions}
Template method \alib{boxing;Box::GetFunction} performs a lookup of the function in struct
\alib{boxing;detail::FunctionTable} that is embedded in the \e vtable member of each box.
This struct has simple pointer "slots" for each built-in function which are selected using template
specializations of the corresponding access functions.

For custom box-functions, a global hash table is used to search the function implementation
using a pair of a function table pointer and the function type as the key value.

As a result, a function lookup for built-in function is performed in \e O(1), one for
is slower and only in the average case is \e O(1).

If parameter \p{searchScope} of method \b Box::GetFunction equals \b Reach::Global, then
in case of not finding a specific implementation, the search is repeated using namespace
object \alib{boxing;detail::DEFAULT_FUNCTIONS}.

Finally, template method \alib{boxing;Box::Call} uses \b GetFunction and then just passes
any given parameters to a C++ function call. Parameters are passed using C++ 11 "perfect forwarding".
In the case that no interface method is found, a default value of the return type \p{TReturn}
is created. Depending on the type, this might invoke a default constructor.

\I{################################################################################################}
### 12.8.7 Compile Times ### {#alib_boxing_more_performance_compiletimes}
Due to the use of type-traits and concept-based selected methods with rather complicated type
expressions that the compiler has to evaluate, the time to compile a code unit increases with the
use of \alib_boxing_nl.

Unfortunately, while we have done our best to reduce it as much as possible, this increase can be
reasonably high.

\I{################################################################################################}
### 12.8.8 Conclusion And Comparison To std::any ### {#alib_boxing_more_performance_conclusion}
We consider the implementation of \alib_boxing_nl to be as performant as it is possible.

It is hard or impossible to compare the impact on code size and performance between using of
techniques like <em>C++ variadic template arguments</em> and the invocation of methods that do
auto-boxing, probably using \ref alib_boxing_boxes_boxes "class TBoxes" to fetch variadic arguments.

In comparison to using C++17 type \c std::any, the most important advantage of \alib_boxing_nl
is that no heap memory allocations are performed, because class \b Box "switches" to pointer-boxing
in the case a value does not fit to its placeholder.
Reversely, when just fundamental types and small value classes are boxed, then \c std::any has an
advantage in construction performance and memory footprint.

At the end of the day, the typical \ref alib_boxing_appendix_usecases "use cases of ALib Boxing"
anyhow do not impose high demands on performance. The main motivation for providing this
manual chapter is for the sake of completeness and furthermore, that the authors of the manual
think that the previous considerations help to profoundly understand how \alib_boxing_nl
is implemented and therefore is to be used.


\I{################################################################################################}
\I{###################         Appendix A: 3rd Party Library Compatibility          #############  }
\I{################################################################################################}

\I{################################################################################################}
# Appendix A Quick Reference # {#alib_boxing_appendix_quickref}

While the \ref alib::boxing "namespace documentation" provides an extensive reference index
(generated with marvelous \https{Doxygen,https://www.doxygen.nl}),
the following quick lists should help finding the information you need:

## A.1 Type Guessing ## {#alib_boxing_appendix_quickref_1}

|Method   |Description
|------------------------------------|---------------------
|\alib{boxing;Box::IsType<void>}       | Tests for boxes that are default constructed or have a \c nullptr assigned, hence have no value boxed.
|\alib{boxing;Box::IsType<T>}          | Tests a box for containing a value of boxable type \b T.
|\alib{boxing;Box::IsArray}            | Returns \c true, if a one-dimensional C++ array had been boxed.
|\alib{boxing;Box::IsArrayOf<T>}       | Returns \c true, if \b IsArray returns \c true and if the boxed array element type corresponds to given type \b T.
|\alib{boxing;Box::IsPointer}          | Returns \c true, if the mapped type is of pointer type.
|\alib{boxing;Box::IsEnum}             | Returns \c true, if the box contains an enumeration element.
|\alib{boxing;Box::IsSameType<Box>}    | Non-template method that returns \c true if a box contains the same mapped-type than a given one.
|\alib{boxing;Box::IsCharacter}        | Aggregation function that tests for mapped character types, respecting the compiler-symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS.
|\alib{boxing;Box::IsSignedIntegral}   | Aggregation function that tests for mapped signed integral types, respecting the compiler-symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS.
|\alib{boxing;Box::IsUnsignedIntegral} | Aggregation function that tests for mapped unsigned integral types, respecting the compiler-symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS.
|\alib{boxing;Box::IsFloatingPoint}    | Aggregation function that tests for mapped floating point types, respecting the compiler-symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS.


## A.2 Unboxing ## {#alib_boxing_appendix_quickref_2}

|Method   |Description
|------------------------------------|---------------------
|\alib{boxing;Box::Unbox<T>}             | Unboxes non-array type \b T.
|\alib{boxing;Box::UnboxArray<T>}        | Unboxes the pointer to an array of element type \b T.
|\alib{boxing;Box::UnboxElement<T>}      | Unboxes an array's element of type \b T.
|\alib{boxing;Box::UnboxLength}          | Unboxes an array's length.
|\alib{boxing;Box::UnboxCharacter}       | Aggregation function that unboxes a \alib{characters;wchar}, respecting the compiler-symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS.
|\alib{boxing;Box::UnboxSignedIntegral}  | Aggregation function that unboxes a \alib{integer}, respecting the compiler-symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS.
|\alib{boxing;Box::UnboxUnsignedIntegral}| Aggregation function that unboxes a \alib{uinteger}, respecting the compiler-symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS.
|\alib{boxing;Box::UnboxFloatingPoint}   | Aggregation function that unboxes a value of type \b double, respecting the compiler-symbol \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS.
|\alib{boxing;Box::Data}                 | Allows direct constant access to a box's placeholder.


## A.3 Built-In Box-Functions ## {#alib_boxing_appendix_quickref_3}
The following box-functions are predefined with the library:

|Name                    | Description/Notes
|------------------------|-----------------------------------------------------------------------------------
|\alib{boxing;FEquals}   |Logical comparison of the contents of two boxes. Specialization given for all fundamental and character array types<br>Templated implementations for comparable types are given with \alib{boxing;FEquals::ComparableTypes}.
|\alib{boxing;FIsLess}   |Logical comparison of the contents of two boxes. Specialization given for all fundamental and character array types<br>Templated implementations for comparable types are given with \alib{boxing;FIsLess::ComparableTypes}.
|\alib{boxing;FIsNotNull}|See chapter \ref alib_boxing_more_void_nulled for more information.
|\alib{boxing;FClone}    |See chapter \ref alib_boxing_more_iclone for more information.
|\alib{boxing;FIsTrue}   |Returns \c true if a boxed value is considered to represent <em>value true</em>; \c false otherwise.<br>The default implementation returns \c true for array types with zero length and for non-array types; if the \alib{boxing;Box::GetPlaceholderUsageLength;used placeholder bytes} do not all contain \c 0.<br>No type-specific implementations are given.
|\alib{boxing;FHashcode} |Calculates a hash-code using the boxed type information as well as the boxed data.<br>A default implementation is given that takes all \alib{boxing;Box::GetPlaceholderUsageLength;used placeholder bytes} into account for types boxed as values or enums; the pointer address for types boxed as pointers and the array contents for boxed arrays. Furthermore specializations for all fundamental types are given by using static templated member \alib{boxing;FHashcode::UsePlaceholderBytes}.<br>For pointer types, the provision of a specialization that collects type-specific hashable data from the pointer may lead to advanced hashing results.
|\alib{boxing;FAppend}   |Appends a string representation of the contents of the box to a given \b AString.<br>The default implementation writes the type name and a hexadecimal number in brackets behind for pointer types and similar information for other types. As type name information is available in debug-compilations only, in release code, the words "ValueType", "PointerType", "ArrayType" or "EnumType" are written instead. Hence, this indicates that a missing specialization is in fact an error and the default implementation is rather given for convenience and testing purposes.<br>Specializations are given for fundamental and character array types.

## A.4 Box-Function Invocation ## {#alib_boxing_appendix_quickref_4}

|Method   |Description
|----------------------------------|---------------------
|\alib{boxing;Box::Call}           | Calls a box-function.
|\alib{boxing;Box::CallDirect}     | Calls box-function previously received with \b GetFunction.
|\alib{boxing;Box::GetFunction}    | Returns a box-function's implementation.
|\alib{boxing;Box::Clone}          | Implicitly calls box-function \alib{boxing;FClone}.
|\alib{boxing;Box::Hashcode}       | Implicitly calls box-function \alib{boxing;FHashcode}.
|\alib{boxing;Box::IsNull}         | Implicitly calls box-function \alib{boxing;FIsNotNull}.
|\alib{boxing;Box::IsNotNull}      | Implicitly calls box-function \alib{boxing;FIsNotNull}.
|\alib{boxing;Box::operator bool()}| Implicitly calls box-function \alib{boxing;FIsTrue}.
|\alib{boxing;Box::operator==}     | Implicitly calls box-function \alib{boxing;FEquals}.
|\alib{boxing;Box::operator!=}     | Implicitly calls box-function \alib{boxing;FEquals}.
|\alib{boxing;Box::operator<}      | Implicitly calls box-function \alib{boxing;FIsLess}.
|\alib{boxing;Box::operator<=}     | Implicitly calls box-functions \alib{boxing;FIsLess} and \alib{boxing;FEquals}.
|\alib{boxing;Box::operator>}      | Implicitly calls box-functions \alib{boxing;FIsLess} and \alib{boxing;FEquals}.
|\alib{boxing;Box::operator>=}     | Implicitly calls box-function \alib{boxing;FIsLess}.

## A.5 Further Methods Of Class Box: ## {#alib_boxing_appendix_quickref_5}

|Method   |Description
|------------------------------------|---------------------
|\alib{boxing;Box::TypeID}           | Returns the \c typeid of a mapped type.
|\alib{boxing;Box::ElementTypeID}    | Returns the \c typeid of a boxed array's element type.
|\alib{boxing;Box::GetPlaceholderUsageLength}| Returns the bytes used in the placeholder. Usefull to write generic code, e.g., to implement default versions of box-functions.


## A.6 Debug Methods And Entities: ## {#alib_boxing_appendix_quickref_6}

| Method                                      |Description
|---------------------------------------------|---------------------
| \alib{boxing::debug}                        | Namespace with tool functions to create human-readable information about the configuration of \alib_boxing_nl.
| \alib{boxing;Box::DbgGetVTable}             | Returns the \e vtable singleton of a box.
| \alib{boxing::detail;VTable::Functions}     | Has a set of fields whose names are prefixed \b "DbgCntInvocations" and provide the number of invocations of the corresponding built-in box-function. Likewise; method \alib{boxing;debug::GetSpecificFunctionTypes} returns the usage number with each registered custom box-function.
| \alib{boxing::detail;VTable::DbgProduction} | Denotes if a \e vtable singleton was dynamically created or is an \ref alib_boxing_more_opt_staticvt "optimized static" object.
| \alib{boxing::detail;VTable::DbgCntUsage}   | A usage counter for the mapped type. The counter is increased with the invocation of various unboxing methods and when a box-function invocation is performed.


## A.7 Built-In Non-Bijective Boxing ## {#alib_boxing_appendix_quickref_7}
With default compilations, the following bijective boxing rules apply:

|Source Type   | Mapped Type      | Unboxing/Comments
|--------------|-----------------------|-----------------------------------------------------------------------
|References and values of composite types (structs and classes) that either do not fit into union \alib{boxing;Placeholder} or that are not copy-constructible or trivially destructible.|Pointers to corresponding composite types| Only the pointer type can be unboxed.
|Pointers to objects of composite types (structs and classes) that do fit into union \alib{boxing;Placeholder} and that are copy-constructible or trivially destructible.|Values of corresponding composite types| Only the value type can be unboxed.
|Signed integral types of any size|\alib{integer}| Only type \alib{integer} can be unboxed.
|Unsigned integral types of any size|\alib{uinteger}| Only type \alib{uinteger} can be unboxed.
|\c float|\c double| Only type \c double can be unboxed.
|\c char, \c wchar_t, \c char16_t and \c char32_t|\alib{characters;wchar}| Only type \alib{characters;wchar} can be unboxed.
|\c const pointer to any of the three character types \alib{characters;nchar}, \alib{characters;wchar} or \alib{characters;xchar}<br>string literals<br><c>char[]</c><br><c>std::string</c><br><c>std::string_view</c><br><c>std::vector<char></c><br>\alib string types|Array of corresponding character type|"Lightweight" string types like \c std::string_view or \alib{strings;TString;String} can be unboxed, "heavyweight" string types like \b AString cannot.



\I{################################################################################################}
\I{###################         Appendix A: 3rd Party Library Compatibility          #############  }
\I{################################################################################################}

\I{################################################################################################}
# Appendix B: 3rd Party Library Compatibility # {#alib_boxing_appendix_compat}
Within the list of \ref alib_manual_modules_impludes "includable headers" some
special headers named <b>"ALib.Compatibility.Xyz"</b> are found.
These headers are \b not included by other library headers, but instead may optionally be added
to the using code.<br>
The headers concern different \alibmods, targeting different 3rd-party libraries
(in this respect we consider also the C++ standard library as such, as its use is always optional).

There is no further documentation given in this user manual. However, in the reference
documentation of this module, which is found with the documentation of namespace
\ref alib::boxing, inner namespace \ref alib::boxing::compatibility exists, which
aggregates some of the customization content.

To achieve this, the documentation even sometimes "fakes" entities into this namespace, that
technically must not be there - and in reality therefore are not there.<br>
As a sample, take functor struct
\ref alib::boxing::compatibility::std::hash<alib::boxing::Box>.
While the documentation claims it to be in that deep namespace, it is a specialization of
struct \c std::hash and therefore "in reality" is (and has to be) located in namespace \c std.
The reference documentation of all "faked", moved entities will individually hint to this fact.

Compatibility headers provided for module \alib_characters, have to be included before those
provided for \alib_boxing_nl.

The headers found should give a good guidance for implementing custom ones as needed. Please feel
free to sent us your implementations for inclusion in this library. But please do this only together
with a due approval that those contributions are allowed to be published by us under the
<b>ALib License Terms</b>.

\I{################################################################################################}
\I{#########################         Appendix B: Use Cases          #############################  }
\I{################################################################################################}
# Appendix C: Use Cases # {#alib_boxing_appendix_usecases}
Quite often in this Programmer's Manual, it was talked about certain "design decisions" and that
those are "justifiable" with the typical use-case scenarios of module \alib_boxing_nl.

The following presentation of sample use cases now intent to give such justification.
For example, it will be shown that:
- Loosing type information and data of the original type with non-bijective boxing is very seldom
  a problem, and if it is, it can be easily bypassed using
  \ref alib_boxing_customizing_identity "identity boxing".
- Loosing information about whether a source type was constant or not, and making constant the
  default for pointer types, is seldom be a problem, because processing functions usually
  perform read-only operations
- Not performing "deep copies" of values larger than \b 16 bytes (respectively \b 8 bytes on a
  32-bit system), but instead just boxing a pointer to the original, only seldom imposes a
  life-cycle conflict.<br>
  And if it does, box-function \alib{boxing;FClone} helps out, even in a way that
  reduces dynamic memory allocations to a bare minimum.
- Box-functions enable a library to offer support for 3rd-party types, without the need of
  touching those. In other words, the processing function may reside in a library as well
  as the boxed types may do. Even then, the external types can be made compatible with the
  also external processing function.
- Finally, it will be shown that customization of boxing, as well as defining implementations
  of box-functions is seldom needed. And if it is, a custom code can provide nice macros for
  its own audience, to enable those programmers to use the custom library.

All use-case are taken from other \alibmods, which depend on module \alib_boxing_nl.
\I{################################################################################################}
## C.1 Use Case: Inner namespace "format" of Module Basecamp ## {#alib_boxing_appendix_usecases_format}
The \ref alib::format "format types" of module \alib_format are more than a use-case.
In fact they were the initial reason and motivation of creating \alib_boxing_nl!<br>
That module implements the well known <em>"printf paradigm"</em>, which is available in standard libraries
of various programming languages. A <em>printf-like</em> function interface is used to create a
string representation of an arbitrary amount of arguments of arbitrary type. To do so, a
<em>"format string"</em> that contains one placeholder for each provided argument is passed along
with the arbitrary arguments. The placeholders within the format string, follow a certain syntax
which allow various output modifications, like number formats, horizontal alignment, etc.

Module \alib_format_nl provides abstract class \alib{format;Formatter} which
offers two overloaded versions of method \alib{format::Formatter;Format}: both accept a
target \b AString as the first argument. The first accepts a reference to class \alib{boxing;TBoxes},
while the second accepts variadic template parameters besides the target string.
How the latter invokes the first with a few lines of inlined code is explained in
chapter \ref alib_boxing_boxes "11. Variadic Function Arguments and Class TBoxes".

\I{################################################################################################}
### Most Flexible Invocation: :###
So, where is the format string then found in this interface? Well, here is a first idiosyncrasy
of this implementation: The format string is not expected as a separated string type but
just as a first of the arbitrary arguments. This approach has the following advantages:
- Being of type \b Box, just as the arbitrary argument list, allows any type that boxes as a
  characters string to be passed.<br>
  If the first argument is \b not of string type, then its contents is just appended to the
  target string using box-function \alib{boxing;FAppend}. Then the next argument is checked
  to be string type. And so on.
- The second advantage lies in the fact that consequently, if a format string was found and all
  of it's placeholders were processed and still arguments remain in the argument list, the whole
  procedure starts from scratch. This way, a user of the method is free to perform more than
  one format operation in one invocation.
- Out of this advantage, a next one results: the overloaded version of method \b Format that takes
  a reference to class \alib{boxing;TBoxes} becomes very interesting: A using code might collect various
  format operations during its course of execution in an object of that type and when done
  performing all formatting in one invocation. This means, depending on the branches that a code
  takes, different format strings and format arguments might be collected.<br>
  This is especially helpful when module \alib_alox_nl is used in the context
  of debug- and release-logging, which is discussed in a later chapter.

All three advantages together make the format-interface given with module \alib_format_nl
unrivalled in respect to flexibility.

### Efficiency Due To Implicit Pointer-Conversion: ###
A next aspect that this use-case nicely shows is the exclusive use of class \b Box as function arguments.
With this, no concerns of life-cycle management of the boxed data has to be taken into account.
(We refer to those discussed in chapter \ref alib_boxing_more_iclone). When arguments are passed
and boxes are created implicitly on the stack, their life-cycle ends exactly when the function
returns. This greatly justifies the design decision to "automatically" box pointers to objects in
the case that given values do not fit into union \alib{boxing;Placeholder}.
If C++17 class \c std::any was used instead, unless the library documentation would demand its users
to explicitly pass pointers, deep copies of "bigger" objects would be created.
And this would be completely unnecessary overhead, because the formatters treat each argument
as a constant (read-only) object.<br>
One could argument, that it is typical and thus rightful C++ style, to use address <b>operator&</b> when
passing objects, while in contrary this boxing approach hides away the pointerization.
Our counter-argument is: A concept as implemented with \c std::any hides away the deep copy
operation if just no pointer is passed.
This is a negative impact on the performance, while the implicit pointerization is not!

### Support for Custom Format Syntax: ###
Finally, the use case implemented with module \alib_format_nl shows nicely how
\alib_boxing_nl enables to offer a library that can be extended to serve custom types
in a most flexible way. This is shown with the provision of box-function \alib{format;FFormat}
by that module. This allows introducing new placeholder syntax (!) for custom types, of course
without touching the original source code of the module.<br>
A sample of how a custom type can be featured with a custom placeholder syntax is given in the
Programmer's Manual of that module with chapter
\ref alib_format_custom_types


\I{################################################################################################}
## C.2 Use Case: ALib Exceptions   ## {#alib_boxing_appendix_usecases_exceptions}
Module \alib_exceptions introduces class \alib{exceptions;Exception}, which is used in all
\alibmods_nl as \b the \e throwable.

### Use Arbitrary Scoped Enums: ###
Class \b Exception stores a list of \alib{exceptions;Message} objects that may extend
the exception object with new information while the call stack is unwinded. Each message entry has
an identifier that is implemented with field \alib{exceptions;Message::Type}. This field
is of type \alib{boxing;Enum} and is a very good sample for using this type. With that it became
possible that every \alibmod_nl (and likewise a using custom software) defines its own scoped
\c enum type that enumerates all exceptions that the module (respectively custom software) may trow.
As a result, an exception entry's type can contain \c enum elements of custom \c enum types
transparently. A two-level hierarchy results from that. A usual \c catch handler consists of
nested <c>if</c>-statements: The outer uses \alib{boxing;Enum::IsEnumType} to test for the
general exception type. The inner then uses \alib{boxing;Enum::operator==} to test for a specific
element of that exception type.

### Attaching Arbitrary Arguments: ###
Each \alib{exceptions;Message} of an exception may store an arbitrary amount of arbitrary
objects that provides further information about the entry, hence about the cause of the exception or
about state information of the code that threw the exception.<br>
For this, field class \b Message inherits type \alib{boxing;TBoxes} which is a container storing
elements of type \alib{boxing;Box}.
The information stored can (has to) be interpreted in a custom way by corresponding implementations of the
exception handlers. A recommendation for users of this \alibmod_nl, is to prepend a
<em>format string</em> as the first element of this list. Such format string should
contain a placeholder for every provided message argument and together this provides the possibility
for an exception handler to easily create a human-readable text message from an exception entry,
by just passing the \b TBoxes object to a \alib{format;Formatter}, as discussed in the previous
use-case chapter.

### Cloning Exception Arguments: ###
In contrast to the previous use case of text formatting, with \alib{exceptions;Exception} and
its used \alib{exceptions;Message} object, the life-cycle management of the boxed message arguments
is a quite critical issue. To resolve this, method \alib{boxing;TBoxes::CloneAll} is used, which
simply invokes \alib{boxing;TBoxes::CallAll;TBoxes::CallAll<FClone>} and hence clones all
relevant data of values that do not fit into a box, into the internal mono allocator.

A code that throws an exception or while handling one, appends a new message to an exception, has
to ensure that either of the following is true for each boxed argument attached:
1. The argument is boxed as value type.
2. The argument is boxed as pointer type and the object passed survives unwinding the call-stack.
3. The argument is boxed to an array. (The default implementation of box-function \alib{boxing;FClone}
   copies arrays.)
4. The argument is boxed to a mapped type that is equipped with a proper specific implementation
   of \alib{boxing;FClone}.

Class \b Exception provides - and is even allocated within (!) - an object of type
\alib{MonoAllocator}, which itself is allocated in its own first buffer of memory!
If the first buffer is sufficient, then <b>only one single dynamic memory allocation</b> is
performed for the creation of the exception, including the copies of all message arguments!


\I{################################################################################################}
## C.3 Use Case: Module ALib ALox         ## {#alib_boxing_appendix_usecases_alox}
We said \ref alib_boxing_appendix_usecases_format "in appendix C.1", that it was the original motivation
for creating module \alib_boxing_nl. The truth it, module \alib_alox was it, just as
the whole library once started with the development of \alox.<br>

Of course, \alox uses the formatting features of \alib_format and thus all that was said for this
use case applies to \b ALox.

### Prefix Logables: ###
In the context of \alox, boxes are called <em>"logables"</em> because they are the input to the
logger. Now, \alox has an option to define prefixes (objects that are prepended to each log entry)
in various ways. They can appear globally, or only with log-messages that are placed in a certain
scope. The scope can be a source code file, a function or method, or even a certain execution thread.

A particular interesting thing is that if these prefixes are string objects (note that \alox also
supports non-textual logging) these strings are copied when set as a prefix. The rationale for this
is to allow the assembly of a local string object and pass this to \alox as a prefix logable.
This is a pure convenience feature. However, in some seldom cases software might wish to
set a mutable string object as prefix logable. In this case the string must not be copied, but
rather stored as a pointer to the original string object that then might be modified by other
code entities. To achieve this and bypass the string copy feature, the string object has to
be wrapped in \c std::reference_wrapper.<br>
Consequently, this is a sample use-case for what is explained in chapter
\ref alib_boxing_customizing_identity "7.9 Bypass Custom Boxing With Identity-Boxing".

### Overloaded Methods Using Variadic Arguments: ###
With C++, when overloading methods that use templated variadic arguments, quite quickly
compile-time ambiguities occur: From a given set of arguments, the compiler can often not decide
which of the overloaded versions to take, because two or more are matching the variadic portion.
\alib_boxing_nl solves this issue and allows \alox to offer a flexible API with
many variants of overloaded methods that still accept variadic arguments.
How this is done is explained in chapter
\ref alib_boxing_boxes_advanced "11.3 Advanced Usage of Class TBoxes".

\I{################################################################################################}
## C.4 Use Case: Module ALib Expressions  ## {#alib_boxing_appendix_usecases_expressions}

### Goal of Module ALib Expressions: ###
The aim of module \alib_expressions is to provide an easy, yet powerful C++ library that
allows run-time compilation of expressions. Expression syntax mimics and covers the whole set
of C++ operators and like C++ is deemed to by type-safe during compilation
(here: expression compilation performed at run-time!), while allowing custom intermediate and result types, processed by custom
expression identifiers and functions. Expression strings are compiled to a "program", which is
executed by a virtual machine (a simple stack machine provided with the module) to evaluate
an expression result. Together with the program, the virtual machine is fed with an "expression scope"
that provides access to custom data used by the program's identifiers and functions.

The use of \alib_boxing_nl with this module, probably provides the most uncommon - but thus even more
exciting - use case of \alib_boxing_nl.<br>
The full truth is, when planning that module, its authors did not expect how compelling
and helpful the use of class \alib{boxing;Box} would be for the implementation. Only during
the development it became clear that the use of \alib_boxing_nl simplifies almost every
aspect of that library. And this is not only true for the library development itself, but also from
the perspective of an "end-user" that incorporates that module into his own software.<br>
What during development first seemed a like a "misuse" of class \b Box (and was deemed to be
replaced later during development), turned out to tremendously easy the module's interface,
to boost performance, and minimize its code size.

### Transport Of Type Information: ###
This at first considered "misuse" is documented with manual chapter \ref alib_expressions_prereq_sb.
Note that manual still talks of "lazy use" or even "mis-use". In fact this is not really true.
The effect on code size and ease of use is tremendous and it was a thorough decision to keep
this concept since the first released library version.<br>

Let us try to generalize the use-case described in the manual section linked above:
Class \b Box is used to transport type-information between a user's code and a library code.
In contrast to exchanging this information using \c std::type_info references and C++ keyword \c typeid,
along with the information about whether that type is an array type or not, references
to simple "sample boxes" are exchanged. A user of module \alib_expressions_nl this way is
not bothered with things like non-bijective boxing, value or pointer boxing, array boxing and the
rather uncommon C++ RTTI mechanics.
All that a user needs to do is to assign a simple sample value to an object of type \b Box and
pass a reference to this box around.

The reason why the other use cases presented above did not need such use is obvious: Only module
\alib_expressions_nl deals with run-time type information "officially". It's whole goal
is to allow the compilation of expression strings defined by end-users at run-time. Expression
strings that a user feeds into a compiled software might result in different types - at run-time!
Now the code using the library has to tell the library for example, which result type an expression
is allowed to have. Another sample are user-defined expression functions that have a signature of
arguments and the result value. Now, during the type-safe compilation of expression strings
(at run-time), the compiler needs to be able to select the right overloaded functions.
The signature of a custom expression function is defined by a simple list of sample boxes.

### Return a Value of Type Box: ###
The previous use-cases introduced in this appendix did not include a sample where a function or
method returns a value of type \b Box. While the principle of doing so was presented in this manual
very early already (see chapter \ref alib_boxing_basicstut_boxing), it seems it is not too easy to
find a good real-life use case for this. But module \alib_expressions_nl has that.

The module allows defining custom expression functions. When the built-in virtual machine
executes an expression program (aka evaluates an expression) such functions are called.
As input arguments, the current boxes placed on the machine's stack are passed. Each function
returns its result with a value, which replaces the input arguments on the stack.
Consequently, all custom expression functions (which are also used to define custom unary or binary
operators, auto-casts, etc) use boxes as input arguments and return a box.

### Generating Custom Expression Literals: ###
Finally, another nice sample that module \alib_expressions_nl demonstrates is in the area of
box-functions. The module introduces the declaration \alib{expressions;FToLiteral}. This is used
by the expression compiler to generate an "optimized expression string". This may be wanted when
a user passes an expression that can be optimized by the compiler to a shorter expression.
While the optimization internally works and can be used, software might want to present an
expression string back to the user that - if compiled - directly resulted in the optimized
expression program.<br>
Details on that use-case are given in chapter
\ref alib_expressions_details_optimizations "11.5 Optimizations" of that module's Programmer's Manual,
as well as in the reference documentation of the box-function declarator \alib{expressions;FToLiteral}.


<br><br><br><br><br><br> */
