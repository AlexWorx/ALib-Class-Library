// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_enums  ALib Modules EnumOps & EnumRecords - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1. Introduction # {#alib_enums_intro}

This manual covers \b two foundational \alibmods which both aim to improve and extend the concept 
of C++ enumerations. The only reason why this topic is split into two different \alibmods_nl
is because this allows placing the first one, namely \alib_enumops_nl, at a very low
level of the \ref alib_manual_modules_graph "module hierachie".
(In fact, in previous versions of \alib, all functionality was gathered in one module and only
later, the separation was made.)  

All enhancements that are achieved with both modules rely on a 
\ref alib_manual_appendix_tca "type traits". 
Three of them are provided by module \alib_enumops_nl. Those are:
-  \alib{enumops;ArithmeticalTraits}<br>
   Enables simple math operators on elements of enumerations.
-  \alib{enumops;BitwiseTraits}<br>
   Enables bitwise operators on elements of enumerations with bitwise
   numbering.
-  \alib{enumops;IterableTraits}<br>
   Specifies the lower and upper bounds of enum types with consecutive numbering of
   elements.

Note that this module and with it the type traits are included in any \alibbuild.                                                                          

A forth one is introduced by separate module \alib_enumrecords_nl:

-  \alib{enumrecords;RecordsTraits}<br>
   Assigns a <em>"record type"</em> to an enumeration and
   allows retrieving a static definition of such record for it's elements.

Along with the type-traits, corresponding concepts, operators, and namespace functions
are provided. Please note that due to the different module assignments, two different
header files have to be included:
- \implude{EnumOps} for module \alib_enumops_nl, and 
- \implude{EnumRecords} for module \alib_enumrecords_nl. 

Note that this module can be chosen to be excluded from an \alibbuild_nl.                                                                  

The following chapters cover each type trait and the associated functionality.
                 

\I{################################################################################################}
\I{###################            Arithmetical and Bitwise                 ######################  }
\I{################################################################################################}
# 2. Arithmetical and Bitwise Enums #  {#alib_enums_arithmetic}

With scoped enums, neither arithmetical nor logical boolean operators that use enum elements as
arguments, found their way into the language. This seems a little contradiction as enums have their
underlying integral type, which even can be changed using a sort of inheritance syntax and with
just a little static casting, these integral values are accessible.<br>
Also, the other way round, an enum value in C++ can be initialized by passing arbitrary integral
values (even values that no corrsponding element exist for). With enum declaration:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ARITHMETIC_INTRO

the following code compiles:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ARITHMETIC_INTRO_2

Therefore, if operators are needed, the common approaches are to either
- define operators for each scoped enum type that need them, or to
- provide a set of operator functions that accept any enum types (restricted using
  <c>std::is_enum</c>).

The first approach imposes a lot of code duplication, the second has the disadvantage that it
undermines the intention of the C++ language standard: The compiler would allow the operators
even on enum types that are not considered to be <em>"arithmetically defined"</em>.

Therefore, \alib takes a slightly more advanced approach: The operators defined by this
\alibmod_nl are available only for enum types which dispose of a specialization of
either one or both of two type traits.

This way, a subset of the provided operators can be "enabled" specifically for certain types and
will not collide with similar operators found in other libraries or an \alib user's code base.

\alib differs between <em>"arithmetical enums"</em> and <em>"bitwise enums"</em> , which are
introduced in the next two subsections.

\I{################################################################################################}
## 2.1 Standard Arithmetical Operators ## {#alib_enums_arithmetic_standard}

Type trait \alib{enumops;ArithmeticalTraits} by default is derived from <c>std::false_type</c>
and is otherwise empty. If a specialization for an enumeration type derives from
<c>std::true_type</c> instead, the following set of operators become available to a scoped
enumerations:

- \alib{enumops::arithmetical;operator<}
- \alib{enumops::arithmetical;operator<=}
- \alib{enumops::arithmetical;operator>}
- \alib{enumops::arithmetical;operator>=}
- \alib{enumops::arithmetical;operator+}
- \alib{enumops::arithmetical;operator-}
- \alib{enumops::arithmetical;operator+=}
- \alib{enumops::arithmetical;operator-=}

\attention
  While in this documentation the operators are appearing under namespace <c>alib::enumops</c>,
  in <em>reality</em> they are defined in the global namespace!<br>
  Faking the documentation namespace was done to have the documentation of the operator functions
  collected in this \alibmod_nl's namespace.<br>
  By using the global namespace, a using statement like

         using namespace alib::enumops;
  is \b not needed in each source location that uses the operators.

\attention
  As a result of the \ref alib_manual_appendix_tca "type traits" approach used, this does 
  not "clutter" the global namespace in any harmful way, and the operators do not interfere 
  with any existing user code that defines similar operators on enumerations.

For most operators two versions exist: one accepting an enum element for both operands and
a second that accepts the underlying integral type of the enumeration for the <b>right hand side</b>
operand.

While the specialization of the type trait \alib{enumops;ArithmeticalTraits} is a simple task,
using provided macro \ref ALIB_ENUMS_MAKE_ARITHMETICAL makes the code more readable.

If applied to the sample enum class given above as follows:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ARITHMETIC

then the following code compiles:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ARITHMETIC_2

(Please do not forget to include the header \implude{EnumOps}.)

\I{################################################################################################}
## 2.2 Bitwise Operators ## {#alib_enums_arithmetic_bitwise}

Type trait \alib{enumops;BitwiseTraits} by default is derived from <c>std::false_type</c>
and is otherwise empty. If a specialization for an enumeration type derives from
<c>std::true_type</c> instead, the following set of operators become available to a scoped
enumerations:

- \alib{enumops::bitwise;operator&}
- \alib{enumops::bitwise;operator&=}
- \alib{enumops::bitwise;operator|}
- \alib{enumops::bitwise;operator|=}
- \alib{enumops::bitwise;operator^}
- \alib{enumops::bitwise;operator^=}
- \alib{enumops::bitwise;operator~}
- \alib{enumops::bitwise;operator+}  (Alias for \alib{enumops::bitwise;operator|})
- \alib{enumops::bitwise;operator-}  (Alias for a combination of operators
  \alib{enumops::bitwise;operator&} and
  \alib{enumops::bitwise;operator~})
- \alib{enumops::bitwise;operator+=} (An alias for \alib{enumops::bitwise;operator|=})
- \alib{enumops::bitwise;operator-=} (Removes given bit(s) )

\attention
  Likewise with the operators introduced in the previous section, this documentation "fakes" the
  operators into namespace <c>alib::enumops::bitwise</c>, while in fact they are defined in
  the global namespace!<br>
  See note in the previous section for details.

The term <em>"bitwise"</em>  denotes that the elements of enums have numbers assigned which
each represent one (or multiple) bit(s) in the underlying integral type.

As a sample, consider the following two enum types:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_NORMAL
 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_BITWISE_DECLARATION


While type \b Fruits is an "ordinary" enumeration, type \b States is obviously of <em>"bitwise nature"</em>.
Obviously values of this enum represent "states of windows in a window manager",
and for this, enum element values with multiple bits set might occur.

Therefore, in the sample, macro \ref ALIB_ENUMS_MAKE_BITWISE is used to define the type trait
\alib{enumops;BitwiseTraits} for the type.

With that, the following code snippet compiles:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_BITWISE_2


In addition to these operators, namespace functions
- \alib{enumops::bitwise;HasBits},
- \alib{enumops::bitwise;CountElements},
- \alib{enumops::bitwise;ToBitwiseEnumeration} and
- \alib{enumops::bitwise;ToSequentialEnumeration}

become applicable. Please consult the functions' reference documentation for further information.
\attention
  Likewise with the operators, this documentation "fakes" these functions into namespace
  <c>alib::enumops::bitwise</c>, while in fact they are defined in namespace #alib.

\see A different approach to allow bitwise operations on <em>"ordinary"</em> enums
     (like \b Fruits in the sample above) is introduced in later chapter
     \ref alib_enums_iter_bitset "3.5 Using Class TBitSet with Iterable Enums".


\I{################################################################################################}
\I{###################                  Iteration                       ########################  }
\I{################################################################################################}
# 3. Iterable Enums  # {#alib_enums_iter}
Another "missing feature" of the C++ language in respect to scoped enums is the possibility
to iterate over all enum elements defined in an enum type.
The obvious reason why standard iterator functionality \c std::iterator_traits and C++ range-based
iterations are not applicable to enumerations is that enumerations are types and not
containers or other iterable object instances.

Nevertheless it would still be nice if iteration was possible and for this to achieve, this tiny
module provides a simple solution, likewise available with the inclusion of the header
\implude{EnumOps}.

\I{################################################################################################}
## 3.1 Type Trait IterableTraits ## {#alib_enums_iter_struct}

To have an easy mechanism for iterating over enum types, the type trait 
\alib{enumops;IterableTraits} may be specialized for a custom enum type that is not "sparsely" 
defined. The latter here means that each element has an adjacent element with a difference 
of \c 1 of their assigned integral value. (All, but the last, of course.)

\attention
  \alib is not able to check if this requirement is met for a given type. It is the user's
  responsibility to ensure this and specialize this type trait only for such types.

The following gives a simple sample of a type that obviously meets the requirement:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ITER_SAMPLE

A typical C++ code iterating over all enumerations would look like this:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ITER_SAMPLE_LOOP

The enums are stuffed in an array using a <c>std::initializer_list</c>  to be iterable.
This is inefficient and error prone with changes of the enumeration definition.

As an alternative, this module provides the type trait \alib{enumops;IterableTraits} which 
we specialize for enumeration \c Pets using helper macro \ref ALIB_ENUMS_MAKE_ITERABLE as follows:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ITER_MAKE_ITERABLE

With that in place, templated class \alib{enumops;EnumIterator} becomes available for the enumeration
type. The loop can be rewritten as follows:
 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ITER_SAMPLE_LOOP_NEW

\I{################################################################################################}
## 3.2 Details On Iterable Enums ## {#alib_enums_iter_details}

### 3.2.1 operators+/-(TEnum, int) ###  {#alib_enums_iter_details_1}
In the previous section we used
 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ITER_MAKE_ITERABLE
to announce enum \c Pets to \alib.
Besides the enum type, the macro expects the
<em>"integral value of the last enum element plus 1"</em>.

You might have noticed that the term <c>Pets::Snake + 1</c> usually is not valid
C++ code, as we are adding an integral value to a scoped enum element.

The reason why this still compiles is that with a specialization of \alib{enumops;IterableTraits}
\alib{enumops::iterable;operator+;operator+<TEnum; int>} and
\alib{enumops::iterable;operator-;operator-<TEnum; int>} become available.

### 3.2.2 Stopper Elements ###  {#alib_enums_iter_details_2}
In the sample discussed,  <c>Pets::Snake + 1</c> was used as the "end value" of an iteration.
This is error prone in the respect that if the enumeration type gets extended, our macro
invocation might be changes, as <c>Pets::Snake</c> then is not the last in the list.

A way out, is to add a "stopper" element to the enumeration and name it special, e.g., in
upper case <c>"END_OF_ITERABLE_ENUM"</c>. It is then rather unlikely, that some programmer would
put a new element behind this one. Furthermore, the macro statement would never needed to be
changed:

     ALIB_ENUMS_MAKE_ITERABLE(MyEnum, MyEnum::END_OF_ITERABLE_ENUM )

A next advantage is that within the enum declaration itself it becomes obvious that this is
an iterable enum type, and somewhere in the global namespace of the same header-file, 
the specialization for \b IterableTraits will be found.
Of course, the drawback is that an enum element is presented to the C++ compiler that is not
an element like the other ones.

### 3.2.3 Starter Elements ###    {#alib_enums_iter_details_3}
So far, all our samples used macro
- \ref ALIB_ENUMS_MAKE_ITERABLE

to specialize this struct. In fact, this macro is just a shortcut to macro
- \ref ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END

passing <c>TEnum(0)</c> as a start value.

This lifts the restriction of having integral \c 0 underlying the first enum element.

### 3.2.4 Helper-Type EnumIterator ###  {#alib_enums_iter_details_4}
The \c std::iterator_traits returned with methods \alib{enumops;EnumIterator::begin} and
\alib{enumops;EnumIterator::begin} implements the standard library concept of
\https{RandomAccessIterator,en.cppreference.com/w/cpp/concept/RandomAccessIterator} and with this
offers various operators, including subscript \c operator[].

\I{################################################################################################}
## 3.3 Bitwise Enums And Iteration ## {#alib_enums_iter_bitwise}
Iteration works well if the type trait \alib{enumops;BitwiseTraits} is specialized in parallel
to \alib{enumops;IterableTraits}.
The restriction described in \ref alib_enums_iter_struct, namely that enum types must not be
"sparsely" defined, in this case means that every next enum element has the next bit set,
hence its internal value is doubled with each next element.

Macro \ref ALIB_ENUMS_MAKE_ITERABLE, chooses integral value \c 1 as a start element.
Again, if \ref ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END is used, iteration might start on higher values.

\I{################################################################################################}
## 3.4 Performance Considerations  ## {#alib_enums_iter_perf}
Class \alib{enumops;EnumIterator} is empty in respect to fields. Created on the stack there
is no performance penalty. The same is true for the internal iterator type, which is
returned with class \alib{enumops;EnumIterator::begin} and class \alib{enumops;EnumIterator::end}.
This iterator class uses an \p{TEnum} element as its only field member.
While the code with operators, casting and conversion seems quite complex, at least with
compiler optimizations turned on (release-builds), the loop will perform the same as an integral
while loop:

     int i= 0; int stop= 5;
     while( i++ < stop) { ... }

\I{################################################################################################}
## 3.5 Using Class TBitSet with Iterable Enums ## {#alib_enums_iter_bitset}
Often, a subset of enumeration elements need to be stored in a set.
If the only purpose for the enumeration is to do exactly this, the solution is to
define an enum as "bitwise", as discussed in previous chapter
\ref alib_enums_arithmetic_bitwise "2.2 Bitwise Operators". With that, a combination of elements
can easily be be stored and tested in an integral value.
If however, in contrast, the enumeration still "needs" to have a standard sequential numbering, then
for the sake of storing permutations of elements, a "bitset", for example <c>std::bitset</c>
which holds one bit per possible element is the way to go.

\alib provides a powerful alternative to standard type
<c>std::bitset</c>, which makes the work with enumerations very convenient.

Let us come back to our \ref alib_enums_iter_struct "previous sample" of enum \e Pets:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_ITER_SAMPLE

With the module \alib_enumops, the type definition \alib{enumops;EnumBitSet} becomes available. 
This type simply fills out the right template parameters for target type \alib{lang;TBitSet}.<br>
Those are:
- The "interface type into the bitset", namely \b Pets. This allows use to use elements of this
  enumeration to be passed as the bit number specifiers.
- The length of the enumeration by passing the field \alib{enumops;IterableTraits::End}.
- The start of numbering. In our case, this is \c 0, but the implementation also allows other
  ranges like 1000..1050, resulting in a bit set with a capacity of 50.

With this type, we can now define a bitset as follows:
 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_BITSET_DEF

And fill it with:
 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_BITSET_FILL

Because \alib{lang;TBitSet} provides efficient bidirectional iterator types that deliver just the bits
that are set, a simple loop like this can be implemented:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_BITSET_USE1

We flip the set and loop again:
 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_BITSET_USE2

The output of this code is:

\snippet "DOX_ENUMS_BITSET.txt"     OUTPUT

For the full documentation of the features of type \b EnumBitSet, consult the
\ref alib::lang::TBitSet "reference documentation" of underlying class \b TBitSet and keep in mind, that
wherever template type \p{TInterface} is mentioned, a C++ enum element can be directly
provided. The only prerequisite is that preprocessor macro \ref ALIB_ENUMS_MAKE_ITERABLE
is applied to the enum class.


\I{################################################################################################}
\I{###################                  Enum Records                    ########################  }
\I{################################################################################################}
# 4. Enum Records # {#alib_enums_records}

We have seen in the previous sections of this manual that C++ enumeration types are - by language
design - quite limited in their functionality. So far, we have added various operators and
an iterator type, which all become activated by specializing corresponding type traits.
These traits are defined in module \alib_enumops_nl which is always included in any \alibbuild.

Probably the most powerful feature of \alib in respect to C++ enums is provided by the separate 
module \alib_enumrecords_nl, which is included with the header \implude{EnumRecords}.<br>
This module provides the concept of <b>ALib Enum Records</b>, which again is enabled for a custom 
enumeration type by specializing another struct, namely \alib{enumrecords;RecordsTraits}.


The features achieved with this are:
- A custom data record type is associated with an enum type.
- One or more static data records may be defined for each element of such enum type.
- The inheritance graph of the record types, may be used to constitute an inheritance relationship
  for enums. This allows phrasing questions like:<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <em>"Is enum type X inherited from enum type Y?"</em><br>
  This is answered at compile-time and for example allows a custom method to accept
  elements of an arbitrary enum types as an argument, as long as they are of a certain <em>"base type"</em>.
- In the presence of module \alib_strings in the \alibbuild, enum elements can be easily serialized
  (to log files, configuration files, etc).
- In the presence of module \alib_camp in the \alibbuild_nl, data record definitions can be easily
  <em>"externalized"</em>, for example, with the aim of translating software to different locales
  or for providing the ability to change default values of software without recompilation
  and roll-out.

While technically the implementation of <b>ALib Enum Records</b> is very simple and their use
is likewise very straightforward, to leverage their potential, it is important to understand
a design pattern of their use.
A step-by-step sample of this design pattern is explained in chapter \ref alib_enums_records_paradigm.

For now, lets start with the simple things.

\note
  While the only dependency of this \alibmod is with \alib_singletons, and therefore the
  module compiles well in the absence of module \alib_strings in an \alibbuild, this
  manual's samples use \alib string-types and thus to compile the samples that module has to
  be included.<br>
  By the same token it should be mentioned that the utility functions used for parsing
  (externalized) string-defined data records are implemented with \alib strings and hence
  are not available in the absence of module \alib_strings_nl.
  It is therefore greatly recommended to include this module when using <b>ALib Enum Records</b>,
  independent of the fact if a user code intends to make use of \alib string types byond that
  or not.



\I{################################################################################################}
## 4.1 Equipping An Enumeration With Data Records ## {#alib_enums_records_firststep}

In chapter \ref alib_enums_arithmetic_bitwise a simple enumeration type \b Fruits was
given. If this was not C++ but a "higher level" programming language, we could print out
the name of the enum elements easily.

This is for example \b Java code:

\code{.java}
enum Fruits
{
    Apple,
    Orange,
    Banana,
}

// print fruits
public void printFruit(Fruits fruit)
{
    System.out.println( fruit );
}
\endcode

For a typical C++ programmer, the purpose of having a language feature that provides run-time
information on "source code elements" might be very questionable, but still let's have a try to do
something similar with <b>ALib Enum Records</b>.

Here is our enumeration:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS

It is only three simple steps to be done.

\I{################################################################################################}
### 4.1.1 Step 1/3: Declaring The Record Type ### {#alib_enums_records_firststep_recordtype}
First we need a suitable <em><b>"record type"</b></em> to store the element names:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_ER


\I{################################################################################################}
### 4.1.2 Step 2/3: Assigning The Record To The Enumeration ### {#alib_enums_records_firststep_assign}
Now we specialize the type trait \alib{enumrecords;RecordsTraits} for enum \b Fruits. The only single
entity in this struct is given with <c>using</c>-statement \alib{enumrecords;RecordsTraits::Type}, which
defaults to <c>void</c> in the non-specialized version. With the specialization this is set
to our record type \b ERFruits.<br>
The struct is included with:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_HEADER_MAIN

The easiest way to do the specialization is by using macro \ref ALIB_ENUMS_ASSIGN_RECORD
as follows:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_ASSIGN

\note
  This is the only macro needed for the use of <b>ALib Enum Records</b>!<br>
  It has to be in the global namespace and usually is placed in same header-file that
  defines the enumeration.

\I{################################################################################################}
### 4.1.3 Step 3/3: Initializing The Data ### {#alib_enums_records_firststep_init}
We are almost done. The final step of preparation is to define the data records. This is to be
done when \ref alib_mod_bs "bootstrapping software".

\note
  Enum records are <em>"by contract"</em> static data! The records once assigned cannot
  be changed. This is in alignment what the \b Java language sampled above offers: Also there, the
  name of the enumeration types is fixed and just equals the name given to the element in the source.<br>
  This restriction is not technical, but a design decision.

\note
  The benefits of this approach will be discussed in later chapters. For now it is enough to
  acknowledge:<br>
  <b>Record definitions are to be done in the single-threaded, one-time executed, bootstrap section
  of software.</b>

To define enum records, several overloads of namespace function \alib{enumrecords::bootstrap;Bootstrap} 
are provided. 
These methods become available with the inclusion of \implude{EnumRecords.Bootstrap}:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_HEADER_INIT

The name of the header and the namespace of the functions already indicate that their use
is to be made only during bootstrap code, for example, in implementations of abstract 
function \alib{camp;Camp::Bootstrap}.

The simplest version of \b Bootstrap accepts one enumeration element along with variadic template
parameters used to construct its record. This has to be invoked for each element:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_INIT_3CALLS

To avoid the multiple invocations, a first overload of \b Bootstrap exists that accepts a
<c>std::initializer_list</c>, which is more performant and has a shorter footprint:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_INIT_1CALL

That's it, the enum records are ready to be used! While later in this manual even more efficient
ways of initializing enum records will be introduced, the next two sections are about using
the data.

\I{################################################################################################}
## 4.2 Accessing Enum Records ## {#alib_enums_records_access}
To access the three enum records of type \b Fruits, two possibilities are offered.

\I{################################################################################################}
### 4.2.1 Retrieving A Specific Record ### {#alib_enums_records_access_specific}
In the Java sample above, method \b printFruit was provided as:

\code{.java}
public void printFruit(Fruits fruit)
{
    System.out.println( fruit );
}
\endcode

Here is now the corresponding C++ code:

\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_GETRECORD

This is using namespace function \alib{enumrecords;GetRecord} passing the given enum element.
The function returns a reference to the record defined during bootstrap.

Invoked like this:
\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_GETRECORD_INVOKE

produces the following output:
 \verbinclude "DOX_ENUMS_RECORDS_FRUITS_GETRECORD"

We're done! We have mimicked the functionality of enums that is built into the Java language!

But in C++ things are always a little more complicated. The following is a valid invocation of
our method:

\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_TRYRECORD_INVOKE

While no "named" enum element is given in the enumeration, still the language allows "constructing"
elements with arbitrary numbers. As there is no record assigned to element <c>42</c>,
the invocation would generate a runtime error, because method \b GetRecord returned
a null-reference. In debug-compilations, function \b GetRecord 
\ref alib_mod_assert "raises an ALib error" in this case.

Therefore, in situations where a code does not "know" if undefined enum elements are passed,
the way out of this is to use sibling function \alib{enumrecords;TryRecord}. This returns a pointer,
and does not \e assert, but rather returns \c nullptr if a record is not found.

The implementation then looks like this:

\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_TRYRECORD

and invoked as above, it produces:

\verbinclude "DOX_ENUMS_RECORDS_FRUITS_TRYRECORD.txt"

These two namespace functions are all that is given to retrieve specific enumeration records.
It is as simple as this!


\I{################################################################################################}
### 4.2.2 Iterating Enum Records ### {#alib_enums_records_access_iteration}

The second method to access enum records is to iterate over all defined records.
Iteration is limited to <em>forward iteration</em>  and the order of records follows the order of
their definition during bootstrap.

Iteration is performed using static templated class \alib{enumrecords;EnumRecords;EnumRecords<TEnum>}
which provides static \b begin() and \b end() methods. When using a <em>range-based for(:)-loop</em>,
C++ requires an iterable object. For this, the static class has a default constructor, which
is needed to be called.

Staying with our sample, a simple loop looks like this:

\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_ITERATE

and produces the following output:

\snippet "DOX_ENUMS_RECORDS_FRUITS_ITERATE.txt"     OUTPUT

C++ <em>range-based for(:)-loops</em> dereference the iterators returned, and this gives us a
reference to the enum records. Unfortunately, the enumeration's element is not accessible this
way. Therefore, if needed, a <em>standard for(;;)-loop</em> has to be used.

As an example, let us "parse" an enum value from a given string:

\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_ITERATE_2

Inner iterator type \alib{enumrecords;EnumRecords::ForwardIterator} offers methods
\alib{enumrecords::EnumRecords;ForwardIterator;Enum} and
\alib{enumrecords::EnumRecords;ForwardIterator;Integral}
which return the enum element, respectively its underlying integral value.

The following code compiles and executes without an assertion:
\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_ITERATE_2_INVOKE


\I{################################################################################################}
## 4.3 Detail Topics## {#alib_enums_records_details}
The author of this manual anticipates that an experienced C++ programmer might not be impressed
much of the functionality that <b>ALib Enum Records</b> offer and that - by reading so far - its
use might be questionable.

The answer, why we think this concept is very valuable is given only in later chapter
\ref alib_enums_records_paradigm and a curious reader might "fast forward" to that chapter.

Meanwhile, we have some details to explain.


\I{################################################################################################}
### 4.3.1 Serialization/Deserialization ### {#alib_enums_records_details_serialization}
What was sampled in the previous sections, namely writing out the C++ element names of
enum class \b Fruits and parsing it back from a string, could be named
<em>serialization and de-serialization</em> of enum elements.

Because this is a frequent requirement (and therefore even a built-in feature with languages like Java)
functionality for this is built-into this module, ready to be used.

The clue to this feature is predefined enum record type \alib{enumrecords;ERSerializable}. Besides
field \alib{enumrecords::ERSerializable;EnumElementName}, this record has a second member with
\alib{enumrecords::ERSerializable;MinimumRecognitionLength}. If this is set to a value greater than
\c 0 it determines the minimum characters of the element name needed to give when parsing.

Implementing the <b>Fruits</b>-sample is now only two steps, because the first step, defining
a record type can be omitted:

<b>Step 1: Assigning record type \b ERSerializable to enum class \b Fruits:</b><br>

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_ASSIGN_ERSERIALZEABLE

<b>Step 2: Defining the records (during bootstrap):</b><br>
 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_INIT_ERSERIALZEABLE

Because all of the element's names start with a different character, we allow to recognize
each with just \c 1 minimum character specified.

As soon as:
1. Record type \b ERSerializable is assigned to an enumeration, and
2. Header file
    \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_HEADER_RESOURCES

is included, elements of that enum become appendable to instances of type 
\alib{strings;TAString;AString}:

\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_APPEND

\note Header file <b>Resources.H</b> of the sibling module \alib_resources is needed to be included
      because this module "replaces" the serialization functionality provided with this module
      \alib_enumrecords_nl. In other words, if the module \alib_resources_nl is \b not included in 
      an \alibbuild, then slightly reduced versions of the serialization functors are defined. 

As with every type that is appendable to \b AString instances with inclusion of the header-file:
\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_HEADER_COMPAT_IOSTREAM

we can use <c>std::ostream::operator\<\<</c> likewise:
\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_OSTREAM

For parsing enum elements back from strings, templated namespace function \alib{enumrecords;Parse} is given:

\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_PARSE


The built-in facilities to serialize and deserialize enumeration element are:
- \alib{strings::APPENDABLES;AppendableTraits<TEnum,TChar,TAllocator>},
- \alib{strings::APPENDABLES;AppendableTraits<TBitwiseEnum,TChar,TAllocator>},
- \alib{enumrecords;Parse},
- \alib{enumrecords;ParseBitwise}, and
- \alib{enumrecords;ParseEnumOrTypeBool}.

\I{################################################################################################}
### 4.3.2 Enum Inheritance Relationships ### {#alib_enums_records_details_inheritance}

By language definition, inheritance hierarchies are not available for C++ enumeration types.
With assigning a record type to enumeration types, <b>the inheritance hierarchy of the record
can be used!</b>. Questions like "Is enum type X inherited from enum type Y?" can be decided
at compile time.
This "side effect" simply <em>emerges</em> of from the concept <em>"ALib Enum Records"</em>.

The most relevant use case that leverages this inheritance relationship is to have functions accept
enum elements as arguments only if they are of a certain "derived enum type".

A quick sample demonstrates this. If the following is given:

\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_INHERITANCE

then, function \b acceptBaseOrDerived can be invoked with enums of type \b Base or \b Derived, but not
with elements of type \b Anything:

\snippet "DOX_ENUMS.cpp"     DOX_ENUMS_INHERITANCE_INVOKE

This feature is a foundation for a powerful design pattern that is introduced in later
chapter \ref alib_enums_records_paradigm and which is used with various other \alibmods_nl.

In respect to what we have seen so far, it is notable that the built-in serialization and
de-serialization functionality introduced in the previous section (given with
\alib{strings::APPENDABLES;AppendableTraits<TEnum,TChar,TAllocator>},
\alib{enumrecords;Parse},
\alib{enumrecords;ParseBitwise},
\alib{enumrecords;ParseEnumOrTypeBool}) are applicable not only to enum types associated with
records of type \alib{enumrecords;ERSerializable} but also with any custom record type that derives
from this!

Consequently, almost all enumeration records found within other \alibmods_nl are derived from
\b ERSerializable and for most custom record types it is appropriate to do so.

\I{################################################################################################}
### 4.3.3 Defining Multiple Records Per Element ### {#alib_enums_records_details_multiple}
It is allowed to define multiple enum records for a single element of an enumeration.
If done, then
- Methods \alib{enumrecords;GetRecord} and \alib{enumrecords;TryRecord} will retrieve the first record
  that was initialized during bootstrap.
- The additional records can only be retrieved by iterating the records, as explained in
  previous section \ref alib_enums_records_access_iteration.

Again a sample helps to quickly understand the rationale and a use case for multiple records.
Built-in \alib enumeration type \alib{lang;Bool} has two elements; \alib{lang;Bool::False} and 
\alib{lang;Bool::True}.
The data record definition performed during bootstrap is found in this module's internal
initialization function:

\snippet "bootstrap/bootstrap.cpp"  DOX_ENUMS_MULTIPLE_RECORDS

The implementation of appending an element of a serializable enum type
\alib{strings::APPENDABLES;AppendableTraits<TEnum,TChar,TAllocator>} uses \alib{enumrecords;TryRecord} and
thus receives the first given name for the elements, namely <c>"False"</c> and <c>"True"</c>.
In contrast, method \alib{enumrecords;Parse} iterates over all records and tries to recognize
any of the names associated with an element. 
With the multiple names given, alternatives to <c>"False"</c> are <c>"0"</c>, <c>"No"</c>, 
<c>"Off"</c> and <c>"-"</c>.<br>
For example, if a boolean value should be parsed from an INI-file, all of these values are
recognized.

With the exception of given names <c>"On"</c>, <c>"Off"</c> and <c>"OK"</c>, all names start with
a different character and a value of \c 1 is given for field
\alib{enumrecords;ERSerializable::MinimumRecognitionLength}. This allows string <c>"F"</c> to be parsed
as <b>Bool::False</b>. For the names starting with <c>'O'</c> this value is \c 2 to avoid
ambiguities while parsing.

Especially with parsing elements, sometimes the order of the records is important. This should
quickly be demonstrated with the record definitions of built-in enum type\alib{lang;ContainerOp}:

\snippet "bootstrap/bootstrap.cpp"  DOX_ENUMS_MULTIPLE_RECORDS_2

Here, element name <c>"Get"</c> and <c>"GetCreate"</c> share the same first letter. Nevertheless,
element <c>"Get"</c> is allowed to be recognized by only one character. To avoid ambiguities,
only the minimum recognitions length of <c>"GetCreate"</c> was increased to \c 4. If this is done,
then the longer name has to be placed first in the list, otherwise even the full string <c>"GetCreate"</c>
was recognized as <c>"Get"</c>.

The way of appending enum elements to \b AString instances is implemented with two
different methods, depending on whether \alib{enumops;BitwiseTraits} is specialized for an enum type
or not (see chapter \ref alib_enums_arithmetic_bitwise).
If it is, instead of just trying to receive a defined record for an enum value, the bitwise version
acknowledges multiple definitions in a tricky and convenient way.
Details and a sample code for this is given with
\alib{strings::APPENDABLES;AppendableTraits<TBitwiseEnum,TChar,TAllocator>}.


\I{################################################################################################}
## 4.4 Resourced/Externalized Enum Records ## {#alib_enums_records_resourced}

As stated in the previous sections, <em>ALib Enum Records</em> are considered <b>static data</b>,
which is <em>"manifested"</em> with three design decisions:
1. Definition of records is allowed only during bootstrap,
2. Records are constant, unchangeable data, and
3. Access to records is not protected against race conditions in multithreaded software
   (and does not need to be, because 1. and 2.).

This is very well in alignment with common string resources that may either reside in the data
segment of a process or that are <em>externalized</em> to be maintainable without recompiling
the software (translations, core-configuration, etc.).

\alib provides module \alib_resources_nl which implements the concept of
\ref alib_mod_resources "Externalized String Resources" and, as shown in this chapter, both
modules go along very well.

\note
  This is achieved by code selection in this module \alib_enumrecords_nl, which provides additional
  interfaces for record definition if module \alib_camp_nl is included in an \alibbuild.


\I{################################################################################################}
### 4.4.1 Parsing Enum Records From Strings ### {#alib_enums_records_resourced_parsing}
So far, in this manual the initialization of enum records has been performed using methods

- \alib{enumrecords::bootstrap;Bootstrap(TEnum element, TArgs&&... args)} for a single record, or
- \doxlinkproblem{namespacealib_1_1enums_1_1bootstrap.html;ae0d44dc0bbb6a855bb44eb603b2b79c7;Bootstrap(std::initializer_list<Initializer>TEnum>>)}
  for an array of records.

(See chapter \ref alib_enums_records_firststep_init for sample code).

A next method offered accepts a string and two delimiter characters:
- \doxlinkproblem{structalib_1_1enums_1_1EnumRecords.html;a0709856b90cc7a6f4eb3d7d7d90f9f06;Bootstrap(const String& input, character innerDelim, character outerDelim)}

It allows parsing a single record or an array of records from a string.
In alignment with the constraints of enum records, the string provided to this method has to be
static itself, for example, a C++ string literal. This restriction implies that the records
of single string type fields do not have to be edited. The strings in the record
simply point to the corresponding substring of the given string after parsing!

Parsing of custom record types has to be supported by custom code implementing parameterless method
\b Parse, which "by contract" imposed by the template code has to be available. 
Details are given with the non-existing, pure documentation type \alib{enumrecords;EnumRecordPrototype}.
Method \b Parse is parameterless because all parsing information (current remaining input string
and delimiters) is accessible through 100% static helper-type \alib{enumrecords::bootstrap;EnumRecordParser}.
The latter provides convenient methods to parse fields of string, integral, floating point,
character and enum type.

\note
  For details of how to implement method \b Parse for your custom type, consult the reference
  documentation of aforementioned entities.

With this knowledge, and the fact that the arguments defining the inner and outer delimiter
characters default to <c>','</c>, the definition of our sample enum <b>Fruit</b>'s records changes
now from:

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_INIT_ERSERIALZEABLE

to

 \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_RECORDS_FRUITS_INIT_STRING

\note
  Although the parsing code has to be included by the compiler, the use of C++
  string literals may lead to smaller footprint of an executable when reasonable numbers of
  records are defined. Should this be important to your software (e.g., embedded systems),
  a comparison might be done.


\I{################################################################################################}
### 4.4.2 ALib Camps And Enum Records  ### {#alib_enums_records_resourced_camps}

With the inclusion of module \alib_camp_nl in an \alibbuild, the strings used to define enum
records should \ref alib_mod_resources "be resourced".
A next overload of method \b Bootstrap supports this:

- \doxlinkproblem{namespacealib_1_1enums_1_1bootstrap.html;a6b2924cfafabfad08496d390d7afd5e5;Bootstrap(ResourcePool&, const NString& category, const NString& name, character innerDelim, character outerDelim)}
      
which can be accessed after the inclusion of the header \implude{Camp}.

While its use is straight forward, it has a specific feature, which is about separating each 
record's definition string into an indexed list of separated resource strings.<br>
Please consult the reference documentation of this method for further information.
This feature is likewise available for the upcoming two further methods.


\I{################################################################################################}
### 4.4.3 Using the Type Trait ResourcedTraits  ### {#alib_enums_records_resourced_tresourced}

Module \alib_resources_nl provides the type trait \alib{resources;ResourcedTraits} to announce
resource information for a type, aiming to have this information available in independent places.
If a specialization of that struct is given for an enumeration type, overloaded method

- \doxlinkproblem{namespacealib_1_1enums_1_1bootstrap.html;a36e2384bf6a7746d8e30e4382d7d6ed2;Bootstrap(character innerDelim, character outerDelim)}

becomes available. The fact that this overload reads the information from \b ResourcedTraits  
becomes obvious from the reduced argument list.

An important advantage of using the type trait \alib{resources;ResourcedTraits} when used with
<em>ALib Enum Record</em> definition strings, is that code that reads and uses a record may use
string members of the records in turn as resource names, which it then loads when needed.
For this, it ignores the method \alib{resources;ResourcedTraits::Name} and uses the name 
given in that field instead.

If so, the "contract" that custom enums that are passed to such code have to fulfill, then
may include that a specialization for \b ResourcedTraits is given.

\note
  Within \alib, two record types exist that make use of this paradigm. In both cases, the
  using code determines if \b ResourcedTraits is specialized. In the case it is, some fields of
  the records are interpreted as names of further resources to load. In case it is not,
  the field's value will be used instead.<br>
  The two types will be introduced in a later chapter.

\I{################################################################################################}
### 4.4.4 Using Resources In ALib Modules  ### {#alib_enums_records_resourced_from_modules}
The final overload of method is given with:

- \doxlinkproblem{namespacealib_1_1enums_1_1bootstrap.html;a2c973ee8ab384d73005f2d35e65a2b06;EnumRecords::Bootstrap(camp::Camp&, const NString& name, character innerDelim, character outerDelim)}

This method should be used when software uses class \alib{camp;Camp} to organize bootstrapping,
resource management, configuration data, etc., just the same as any \e full \alibmod does.

If so, this method is preferred over all others versions, except for the cases where the use of
\b ResourcedTraits is mandatory or otherwise superior, as described in the previous section.

Further information is given with the \ref alib_mod_resources "programmer's manual" of module
\alib_resources_nl.<br>
A comprehensive sample of using \alib resources placed in a custom module is provided with the
\ref alib_cli_tut "tutorial of ALib Module 'CLI'".
The sample code provided there, can be easily used as a jump start into an own project that
creates a custom \alib module and leverages the resource features provided.


\I{################################################################################################}
## 4.5 A Design Pattern For Using Enum Records  ## {#alib_enums_records_paradigm}
The previous chapters 4.1 - 4.2 provided a step-by-step tutorial-style guide into the concept of
<em>ALib Enum Records</em>. The code behind this small \alibmod_nl is easy but still needs
a little explanation - because it is as much a design concept as it is a library.<br>
This chapter wants to give further thoughts about this concept.

A reader might nevertheless think:
- What is this good for? To be able to print out names of enum elements?
- Why should I use this?
- I could use a simple static instance of <c>std::vector</c>  or <c>std::unordered_map</c>
  instead, without the need of using a library for this.

And right: Instead of talking about "Enum Records" this library could talk about
1. <em>"Static data tables"</em>
2. Records are collected in a simple forward list to be iterable.
3. Record are in addition hashed using a pair of a table name and a simple integral value
   as an index to the record.

From this angle of view, the only difference this concept brings is:

<ol start="4">
<li> A C++ enumeration \e type (!) is used for the name that a) addresses an iterable record list
     and b) is the first part of the hash key to retrieve single records.
     (Technically implemented by using run-time type information, aka keyword <c>typeid</c>).</li>
<li> The numbering of the records of a table is done with the elements of that enumeration and
     used as the second part of the hash key.</li>
</ol>

These two small points constitute the real value <em>ALib Enum Records</em> and their
reason for existing.

The single important consequence of this design was mentioned already in section
\ref alib_enums_records_details_inheritance. The inheritance relationship simply emerges from
the inheritance relationship of the assigned record types.

The circle closes at the moment that two different code unit start interacting: Unit A offers
a service that unit B wants to use. In some occasions, <em>ALib Enum Records</em> can be a perfect
concept to define a very elegant interface for B into A.

With that, the \e concept of enum records, transitions in to a <em>"programming paradigm"</em> or
<em>"design pattern"</em>.

Implementations of this pattern are found several times with other \alibmods_nl.
We want to quickly sample one implementation found with module \alib_variables:

This module manages external configuration data. The data is organized in a
\alib{containers;StringTree} of <em>"variables"</em>. These variables can either be
declared in a "hard-coded" fashion or optionally be declared by meta-information struct
\alib{variables;Declaration}.

This declaration struct extends built-in enum record type \alib{enumrecords;ERSerializable}. While the
variable name is taken from the base class, the record provides additional data needed to declare
and define a variable with a reasonable default value in case no external configuration data is
found

Then, the module provides constructors and declaration methods that accept elements of custom enum
types, as long as those are equipped with enum records of type \b Declaration.
(See \alib{variables;Variable;the reference documentation} of class \b Variable.)

Consequently, the user of the module declares a custom enum type that enumerates all variables
that her software wants to store and retrieve externally. This custom enum type is associated with
enum records of type \alib{variables;Declaration}. If the records are resourced, then variable names,
description text and default values are nicely externalized and can be translated to different
locales without recompiling the software.

This is all that needs to be done. The advantages are
- All information about variables are defined in one place.
- The code that stores and retrieves variables is 100% independent of the internals needed to
  declare variables. It just passes the enum element.
- It is very convenient to declare and access variables. No misspelled variable strings may
  occur, and a developer's IDE can propose the choice of elements while typing.
- Special method \alib{variables;Configuration::PreloadVariables} goes even one step further: Just
  all elements, hence all variables of the enumeration types are preloaded with their default value
  with just one invocation.

Another prominent sample is found with class \alib{exceptions;Exception}. Furthermore, module
\alib_cli makes really heavy use of this paradigm.


\I{################################################################################################}
# 5. Class Enum Of Module Boxing # {#alib_enums_furtherreading}

As this module's name indicates, the four major features introduced in this manual are all
about adding features in the area of C++ enumeration types

A fifth valuable tool type that supports working with enumerations is given with class
\alib{boxing::Enum}. The type is located in module \alib_boxing, because it leverages that
module's features and in fact it is just a small extension of the module's central class
\alib{boxing;Box} itself.

In short, class \b Enum is a small wrapper that can be constructed with enumeration elements of
arbitrary type. Along with the underlying integral value of an enum element, run-time
type-information is stored.

This mechanism allows having custom methods with arguments that in turn accept arbitrary enumeration
elements and that <em>defer</em> actions with these arguments for later.

With the namespace functions \alib{boxing;GetRecord} and \alib{boxing;TryRecord}, such postponement
is also offered for the concept of <b>ALib Enum Records</b>.


<br><br><br><br><br><br> */
