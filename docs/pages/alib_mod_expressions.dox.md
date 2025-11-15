// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_expressions    ALib Module Expressions - Programmer's Manual

\tableofcontents


\I{################################################################################################}
# 1. Introduction # {#alib_expressions_intro}

\I{################################################################################################}
## 1.1 Goals ## {#alib_expressions_intro_goals}
The goal of this \alibmod is to provide a C++ library that helps to integrate functionality
in custom software to allow end users to write expression strings, which are understood and
evaluated at \e run-time by that software.

Usually, to achieve this, it is needed to
- Think about the expression \e grammar and specify such.
- Use programming tools like
  \https{bison,savannah.gnu.org/projects/bison/},
  \https{boost::spirit,www.boost.org/doc/libs/1_66_0/libs/spirit/doc/html/index.html} or alike
  (\https{Wikipedia: Comparison of parser generators,en.wikipedia.org/wiki/Comparison_of_parser_generators}).
- Write custom code that evaluates parsed expressions.

This is of course a lot of work and a month of programming time quickly is consumed, unless
a programmer has done this several times before.

Lets quickly consider two samples.
1. An application that processes files and folders. The end-user should be allowed to write
   "filter" expressions like:

            ( date > today - days(7) ) & (name = "*.jpg")
            isFolder & notEmpty

2. An application that manages a table of employee data. The end-user should be allowed to write
   expressions like:

            (StartDate + years(10) < today   & (NumberOfSalaryRaises = 0)
            StartDate( find( "John", "Miller", "Accounting" ) )
            BirthDate( find( 832735 ) )

While on the first glance, these are very different types of expressions, they still have a
lot in common:

- They use \e functions and \e identifiers like \c date, \c name or \c find()
- They do \e calculations like <c>today - days(7)</c>.
- They use \e comparisons like <c><</c>, <c>></c> or <c>=</c>
- They use \e boolean operators like <c>&</c> or <c>|</c>
- They use \e brackets to overrule operator precedences or just as a redundant helper for better
  readability

The areas where the expressions of the two samples differ is:
- They offer different named functions and identifiers.
- Such functions and identifiers may return <b>custom types</b>. Those types may be intermediate
  results as well as results of the expression.
- The operators and functions used are eventually defined (\e "overloaded") for custom types.

With this said, we can much better explain what module \alib_expressions_nl offers:

\par
   <em><b>"%ALib %Expressions provide an expression string parser, formatter and evaluator using
    customizable operators, identifiers and functions which support to process or
    return built-in and custom types."</b></em>

You will see later in this documentation, that the amount of coding needed to implement
functionality like given in the samples above is <b>surprisingly</b> low.

\I{################################################################################################}
## 1.2 Pros and Cons: When To Use ALib Expressions ## {#alib_expressions_intro_procon}
To give you some help in deciding whether module \alib_expressions_nl suits your needs, the
"pros" and "cons" should be listed in bullets.
 We start with the cons:

<b>Reasons to NOT use %ALib %Expressions Library</b>
- The syntax and grammar rules of expressions is rather fixed along the lines of C++ expressions.<br>
  This is mitigated by several tweaks and options available and furthermore the possibility to
  define custom operator symbols and changing the precedence of existing and custom operators.

<p>
- <b>%ALib %Expressions</b> builds on other core modules of \alib.<br>
  Why is this considered a disadvantage? Well, if you are used to \alib it is absolutely not.
  If not, you may have to learn also some basics of "underlying" \alibmods that this module builds
  on.<br>
  Especially important modules to name are \alib_boxing and \alib_strings.

The pros should be given as a feature list:

<b>Features of %ALib %Expressions</b>
- <b>Free</b> software, boost open source license.

<p>
- Well tested under GNU/Linux, Windows OS and MacOs.

<p>
- Very fast, handwritten code, <b>no generation tools or 3rd party libraries</b> needed for the built.

<p>
- <b>Complete coverage of %expression syntax</b> along the lines of C++ expressions<br>
  All operators implemented, including:
  - Ternary, conditional <c>Q ? T : F</c>
  - \alib{expressions::plugins;ElvisOperator;Elvis operator} <c>A ?: B</c>
  - Array subscript operator <c>[]</c> to access array elements.<br>
    This may also be used as <b>hash-map</b> access operator to form expressions like:

           Preferences["DATA_FOLDER"] + "/database.dat"
<p>
- All functions and operators can be \b "overloaded" to support custom types without interfering
  existing functions and operators.

- <b>Verbal operators</b> like "not", "and", "equals" or "greater" can be defined.

- Optional \b localization of operator names, identifiers, functions, the number format, etc.

<p>
- More than <b>130 built-in functions</b> and <b>180 (overloaded) operators</b>!<br>
  Areas that are covered:
  - Boolean, integer and floating point arithmetics.
  - Math functions.
  - String manipulation, including <b>wildcard</b> and <b>regex matching</b>.
  - Date and time functions.
  - File and directory filtering and inspection. (Brought with sibling \alib module \alib_files.)

  As a sample, the following expression:
   \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_PROS_NASTY

  compiles with (optional) built-in functionality. (Compile time less than 40 &micro;s,
  evaluation time less 15 &micro;s, on a year 2018 developer machine.)

- All built-in identifiers, functions and operators are <b>optional/configurable</b>.


<p>
- Support for <b>n-ary and variadic custom functions</b>, including "ellipsis" parameter definitions,
  like in <c>Format(formatString, ...)</c>.<br>

- Optional definition of <b>custom operators</b>, including custom parsing precedence.

<p>
- Support of <b>nested expressions</b>, which is support of "named" expressions that are
  recursively referred to from within other (named or anonymous) expressions.<br>
  Supports mechanics to externally define nested expressions using command line parameters,
  environment variables or within arbitrary (custom) configuration resources, e.g INI-files.

<p>
- <b>Easy use</b>, integration and customization of the library. (This is proved in the tutorial sections below).

<p>
- <b>Compile-time type safety</b><br>
  \note
  What does this mean and why is this important? Because almost all malformed expression input
  (by end-users) is detected at "compile-time" of the expression. This way, software can tell
  a user that an expression is malformed (almost always) already at the moment that a user
  announces an expression to the software. With that, software can in turn reject the expression
  before taking any action to start working with it.<br>
  The other way round: Once an expression got compiled, its evaluation is deemed to succeed.

<p>
- "Seamless" support of <b>arbitrary custom types</b> within expressions. %Types digested by
  expressions can be any C++ type (class).<br>
  \note
    Custom types are "introduced" to module \alib_expressions_nl just by having
    custom identifiers, functions and/or operators return them!<br>
    To then further "support" these types, operators and functions can to be added (or overloaded)
    to work with the types. Of course, the result of expressions can be of such arbitrary types
    as well.<br>
<p>
- Support of <b>automatic type cast</b> of built-in types as well as custom types.<br>
  (This reduces the amount of needed "permutations" of overloaded operators and
  the types they support, and thus the time to customize).

<p>
- \b Localization of number formats in expression literals, including thousands separator
  character.

<p>
- All identifier and function names are \b "resourced" and can be changed without touching
  the library code.

<p>
- Identifier and function support optional abbreviation and case sensitivity.
  If \b "CamelCase" or \b "snake_case" formats are used, minimum lengths can be defined for each
  "camel hump", respectively"snake segment".

<p>
- <b>Decimal, hexadecimal, binary</b> and \b octal integer number literals.
  Scientific and normal floating point parsing and formatting.

<p>
- Largely configurable <b>normalization</b> of user-defined expression strings.
  Configuration offers a choice of \alib{expressions;Normalization;more than 30 options}, including:
  - Removal of redundant brackets and whitespaces (not optional, always performed)
  - Addition of redundant brackets that make expression more readable (several fine-grained options
    are available).
  - Addition of whitespaces for better readability (several fine-grained options are available).
  - Replacement of 'alias' operators (e.g., <c>&</c> on boolean converts to <c>&&</c> or
    assign <c>=</c> converts to <c>==</c>).
  - Replacement of abbreviations of identifiers and functions to their full name.

<p>
- Configurable compiler options, some most obvious ones with simple flags. For example, to
  allow comparison operator <c>'=='</c> to be aliased by assign operator <c>'='</c>, which is
  more intuitive to end-users.

<p>
- Very <b>fast expression evaluation</b><br>
  - %Expressions get \b compiled to a "program" which are executed by an extremely lightweight
    built-in \alib{expressions;detail::VirtualMachine;virtual machine}.
    This avoids the otherwise needed evaluation based on an "abstract syntax tree" with expensive
    recursive invocations of virtual functions.

  <p>
  - The expression compiler performs various optimizations.
    For example, expression

           2 * 3 + 4

    results in one single program command that provides the constant result \c 10.<br>
    Compile-time optimization is also supported with custom identifiers, functions and operators.

<p>
- Optional \b de-compilation of expression programs. This can be used for generating a normalized
  expression string of the \e optimized expression.
  (Just needed if you are mean enough to tell your user about the redundancies in his/her given
  expressions :-)

<p>
- Throws <b>detailed exceptions</b> (exceptions with additional information collected along the
  stacktrace) that contain information that can be displayed to the user to help finding errors
  in given expressions.<br>
  All exceptions and other strings are resourced and can be changed and/or translated to
  target languages.

<p>
- Generation of formatted, commented <b>listings of the compiled expression programs</b>.<br>
  (Available with debug-compilations only. Offered just for curious users of the library that are
  eager to view the simplistic beauty of a stack machine.)

<p>
- <b>Extensive documentation</b>.
  (Please excuse verbosity, writing docs inspires us to do better code.)

\I{############################################################################################## }
# 2. Tutorial: Hello Calculator # {#alib_expressions_calculator}

This documentation switches between in-depth informational sections and tutorial-like sample
sections. Let's start with a quick tutorial section!

What is "hello world" for each new programming language is a "simple calculator" for expression
compilers. Here is the code for implementing one using module \alib_expressions_nl:

  \snippet "docsamples/DOX_EXPR_CALCULATOR.cpp"    DOX_EXPR_TUT_CALC_MAIN

Compile the program and run it by passing some simple sample expressions
(or be lazy and just read on), we give it some tries:

  \verbinclude "DOX_EXPR_TUT_CALC_MAIN-1.txt"

Fine, it calculates! Notable on this first simple sample are the brackets inserted
in the what we call "normalized" expression string. Compare this to the next sample:

  \verbinclude "DOX_EXPR_TUT_CALC_MAIN-2.txt"

Why are the brackets gone here, while in the first case they had been redundant anyhow?
The answer is, that human beings could easily misunderstand the first version,
so module \alib_expressions_nl feels
free to help making an expression more readable.<br>
You think this is childish? Ok, then what do you think about this expression:

           true && false == true < false

Either you are "a pro" or you need to consult a C++ reference manual and check for the operator
precedence. Here is what our calculator says:

  \verbinclude "DOX_EXPR_TUT_CALC_MAIN-3.txt"

The insertion of redundant brackets is one of more than 30 \alib{expressions;Normalization;normalization options}
that are switchable with enumeration flags.<br>
The recent sample has more to show:
- Boolean arithmetics and operators
- Built-in \e identifiers, namely \c true and \c false.

Note, that we use the term \e "identifier" for parameterless expression functions. By default,
the parameter brackets can be omitted with parameterless functions.

Functions with parameters are, for example, found in the area of maths:

  \verbinclude "DOX_EXPR_TUT_CALC_MAIN-4.txt"

or with string processing:

  \verbinclude "DOX_EXPR_TUT_CALC_MAIN-5.txt"

"tolo()" and "toup()"? Well, \alib_expressions_nl support shortcuts for function names.
Normalization optionally replaces abbreviated names.

Finally, a more complex string function sample:

  \verbinclude "DOX_EXPR_TUT_CALC_MAIN-6.txt"

As it can be seen, a whole lot of identifiers, functions and operators are already available
with the simple calculator example. All of these built-in definitions can be switched off.
In fact, the built-in stuff is implemented with the very same interface that custom extensions
would be. The only difference between built-in expression identifiers, functions and operators
and custom ones is that the built-in ones are distributed with the library.

To get an overview of the built-in functionality, you might have a quick look at the tables
found in the reference documentation of the following classes:
- %Compiler plug-in \alib{expressions::plugins;Arithmetics}
- %Compiler plug-in \alib{expressions::plugins;Strings}
- %Compiler plug-in \alib{expressions::plugins;Math}
- %Compiler plug-in \alib{expressions::plugins;DateAndTime}

\note Camp module \alib_files introduces a further "plug-in", dedicated to expression functions
      working on mass-storage files. The functions are
      \alib{files;FileExpressions;documented here}.

\I{################################################################################################}
# 3. Prerequisites # {#alib_expressions_prerequisites}

To fully understand this tutorial, library source code and finally as a
prerequisite to implementing your custom expression compiler, a certain level of understanding of
some underlying library and principles is helpful.

\I{################################################################################################}
## 3.1 ALib Boxing ## {#alib_expressions_prereq_boxing}

As mentioned in the introduction, module \alib_expressions_nl make intensive use of underlying
module \alib_boxing.

For the time being, lets quickly summarize what module \alib_boxing provides:
- Encapsulates any C++ value or pointer in an object of type \alib{boxing;Box}.
- A box is very lightweight (3 x 8 bytes on a 64-bit system) and contains a copy of the value
  (if possible) or a pointer to the object that it capsules.
- Construction of Boxes is seamless: Using \ref alib_manual_appendix_tca "type traits" and
  implicit constructors, "almost anything" can just be assigned to a box.
- Similar features in other programming languages are called auto-boxing. It is especially useful
  if function arguments or return types are of type \b %Box: Such a function can be invoked with
  (almost) any parameter, without providing explicit conversions.
- \alib_boxing_nl is 100% type-safe: The boxed type can be queried and trying to unbox a
  wrong type, \ref alib_mod_assert "raises an error" (with debug-builds).
- \alib_boxing_nl supports a sort of "virtual function" invocation on boxes. This means,
  that functions can be invoked on boxes without prior type-checking and/or unboxing of values.
  Such functions can simply be implemented (according to the required function signature)
  and then registered for a boxed-type.

For all details, comprehensive \ref alib_mod_boxing "Programmer's Manual for ALib Boxing" is
available.


\I{################################################################################################}
## 3.2 Type Definitions With "Sample Boxes" ## {#alib_expressions_prereq_sb}

The type-safety mechanisms and the possibilities of querying the type encapsulated in a box
is used by module \alib_expressions_nl in an inarguably lazy fashion: Wherever this expression
library needs type information, such information is given as a <b>"sample box"</b> which
is created with a sample value of the corresponding C++ type.

Consequently, the value stored (and passed with) the box is ignored and may even may become invalid
after the creation of the box without any harm (for example, in cases of pointer types).

While this approach causes a little overhead in run-time performance, the benefit in respect
to simplification of the API surpasses any such penalty by far! Also, note that the performance
drawback is restricted to the code that compiles an expression. During the evaluation, no
"sample boxes" are created or passed.

The following code shows how to create sample boxes for some of the
\alib{expressions;Types;built-in standard types}:

       Box sampleBool      =    false;
       Box sampleInteger   =        0;
       Box sampleFloat     =      0.0;
       Box sampleString    = String();

The values assigned in the samples are meaningless. Instead of \c false, the value \c true could be
used and instead of \c 0.0, we could have written \c 3.1415.
Note that the construction of the empty \alib{strings;TString;String} instance, will even be
optimized away by the C++ compiler in release compilations.

For custom types, there is no need for more efforts, as this code snippet demonstrates:

       struct Person
       {
           String   Name;
           int      Age;
           String   Street;
           String   City;
           String   PostCode;
       };

       Box samplePerson= Person();

By default, with \alib_boxing, non-trivial C++ types that do not fit into the small placeholder
embedded in the box are \ref alib_boxing_classes "boxed as pointers". This means that even as a
value of a custom type was assigned to the box, a pointer to it is stored.
In the sample above, the pointer will be invalid in the next line, but that is OK, as only the
type information stored in the box is of interest.

Therefore, we can "simplify" the previous code to the following:

       Box samplePerson= reinterpret_cast<Person*>(0);

Besides the advantage that this omits the creation of an otherwise unused object,
this approach is the only way, to get sample boxes of <b>abstract C++ types</b>!


The magic of module \alib_boxing makes life as simple as this! Let us preempt what is explained in the
following chapters: All native callback functions to be implemented for custom operators,
identifiers and functions are defined to return an object of type \b %Box. Thus, these functions
can return values of arbitrary custom type. The type of the returned (boxed) value has to correspond
with what a custom \b %CompilerPlugin suggested by providing a sample box at expression compile-time.
Once understood, this is all very simple!

\note
   Wherever possible, this library uses alias type definition \alib{expressions;Type} instead
   of <c>const Box&</c> to indicate that a box received is a sample box and not a real value.
   However, sometimes it is not possible. In these cases the parameter or member itself, as well as
   the corresponding documentation will give a hint whether an object is a just a "sample box" or
   a boxed value.

\note
   For the built-in types, static one-time sample boxes are defined with struct
   \alib{expressions;Types}. It is recommended to use those and, if custom types are introduced,
   create one singleton sample box for each custom type in a similar fashion.
   This approach makes the code smaller, because mostly only a reference to the static box is passed,
   and the creation of a sample box on the stack is avoided. Also the use of static constant objects
   in bulk-information-tables (introduced later), allows the compiler to build static compile-time
   tables.

\attention
   If sample boxes for custom types should be globally defined <b>and initialized</b>,
   as done with the built-in ones found in \alib{expressions;Types}, an "optimization step" has to be
   performed. Details are given in chapter \ref alib_boxing_more_opt_staticvt of the
   Programmer's Manual of \alib_boxing.<br>
   Note that this is required only if your custom code has to be able to reside in read-only memory,
   e.g., with embedded systems.
   (It can of course also be done by enthusiasts to minimize an executables code size, or just for
   the fun this brings!)<br>
   If these optimizations are not performed, global or static sample boxes have to be default
   constructed with their definition and then initialized with the right sample value only at
   bootstrap of the library, preferably in the constructor of a custom
   \alib{expressions;CompilerPlugin} that introduces these types.

\I{############################################################################################## }
## 3.3 Use Of Virtual Types Rather Than Templates ## {#alib_expressions_prereq_virtual_types}
A design decision of this \alibmod is to rather use "classic" virtual types instead of
using templates, with all the pros and cons taken into account of such a decision. As a result,
some "contracts" have to be ensured to be fulfilled by the user of the library.
The term "contracts" here means: If a at some place a certain specialization of a virtual type
is expected, at a different place the creation of an object of that virtual type has to be ensured.
Details of these contracts will be explained in the next chapters.

\note
  The main reason to use this traditional virtual library design is the use of plenty
  (mostly very short) native callback functions, which this way can be placed in anonymous namespaces
  of compilation units and thus completely be hidden from library header-files and even from the
  C++ linker.

\I{############################################################################################## }
## 3.4 Bauhaus Code Style ## {#alib_expressions_prereq_bauhaus}
\alib generally sometimes uses what we call "Bauhaus Code Style". It is not easy to state what
we mean by this exactly, but a little notion of what it could be may have come to a programmers
mind already by reading the previous two chapters about:
- (Mis-)using class \b %Box for just type propagation, and
- Imposing contract rules with specialized types, instead of templating things.

In addition to that, it is notable, that a lot of the types of module \alib_expressions_nl are
\e structs rather than classes. Hence, fields and methods are exposed publicly.

The goal of this library is to allow other software (libraries or internal units of software)
to expose an interface that has two main functions:
- Allow the input of expression strings.
- Allow the evaluation of compiled expressions.

Now, lets take a sample: A list of files should be 'filtered' by name, size, date etc. The custom
library or internal software unit, would probably expose
- A class named <b>FileFilter</b> that takes an expression string in the constructor.
- A method called "Includes" that takes a file object and returns \c true if the file matches
  the filter.

Using this custom class could look like this:

       FileFilter photosOfToday( "name * +\".jpg\" && date >= today" );

       if( photosOfToday.Includes( aFile ) ) {
           ...
       }

As it is easily understood, really just nothing of library module \alib_expressions_nl needs to be
exposed to the "end user" of the code. Especially:
- Only the sources (compilation units) that implement class \b %FileFilter need to include
  module \alib_expressions_nl
- Consequently, not only details of module \alib_expressions_nl, like
  \alib{expressions;detail::Parser},
  \alib{expressions;detail::Program} or
  \alib{expressions;detail::VirtualMachine},
  but also central types like
  \alib{expressions;ExpressionVal},
  \alib{expressions;Expression},
  \alib{expressions;Compiler},
  \alib{expressions;CompilerPlugin} or
  \alib{expressions;Scope},
  usually remain completely invisible to most parts of the custom software.
- The same is true for custom, derived types and therefore also for the "contract rules"
  (see previous chapter) between these types.

This all means, that the "natural way" of using module \alib_expressions_nl automatically hides away
all internals, which on the other side gives this module the freedom to generously use Bauhaus
style, what here then finally translates to:
- Generously exposing types and their internals.
- Avoid redundant getter/setter methods.
- Impose contracts and avoid templates.
- Optimizations for speed.
- Optimizations for short code.


\I{################################################################################################}
# 4. Tutorial: Implementing A File Filter # {#alib_expressions_tut_ff}

After this already lengthy introduction and discussion of prerequisites, it is now time to implement
custom expression logic. The sample application that we use to demonstrate how this is done,
implements expressions to filter files of directories, as it may be required by a simple file
search software or otherwise be used by a third party application.

As a foundation, we are using the \https{Filesystem Library,en.cppreference.com/w/cpp/filesystem}
introduced with language-standard <b>C++17</b>.

\note
  As all sample code is extracted directly from special unit-tests that exist just for the purpose
  to be tutorial sample code and generate tutorial sample output, above and in the following
  sections we are addressing some parent directories. This results from the fact that the
  unit tests are executed in the built-directory, which is a subfolder of this library's
  main directory.<br>
  Consequently, our samples are around searching and filtering the source files of the library!
  This avoids introducing sample files, and other overhead in respect to documentation maintenance.

\note
  Furthermore, please note that we are using the following statement to shortcut the standard's
  namespace:

       namespace fs = std::filesystem;


\I{################################################################################################}
## 4.1 Skeleton Code For Filtering Files ## {#alib_expressions_tut_ff_skel}

Now, the loop of the above sample should be extended to use a filter to select a subset of the
files and folders to be printed. Hence, a filter is needed.
We start with a skeleton definition of a struct:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_FilterSkeleton

As we have no clue yet, how our custom filter expressions will look like, we pass a dummy string,
which is anyhow ignored by the filter skeleton. The loop then looks as follows:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_FilterLoop

Of course, the output of this loop remains the same, because constant \c true is returned by
the filter skeleton's method \b %Includes.

What we nevertheless have achieved: The interface of how \alib_expressions_nl will be used
is already defined!<br>
This is a good point in time to quickly sort out the different perspectives on "interfaces",
"libraries" or "APIs" explicitly:
1. Library module \alib_expressions_nl exposes an interface/API to compile and evaluate
   expression strings.
2. The software that uses \alib_expressions_nl usually exposes an own interface/API, either
   - to other parts of the same software, or
   - to other software - in case that this 2nd level is a library itself.
3. The "end user" that uses software of course does not know about any software interface or
   API. What she needs to know is just the syntax of expressions strings that she can pass into
   the software!

The goal should be that on the 2nd level, the API of the 1st level
(which is this \alib_expressions_nl library), is \b not visible anymore.<br>
Well, and with the simple skeleton code above, this goal is already achieved!

 \I{################################################################################################}
## 4.2 Adding Generic Ingredients Needed For Expression Evaluation ## {#alib_expressions_tut_ff_generic}
The next step is about adding all components that we need to compile and evaluate expression
strings to the filter class. And this is not much effort. We had seen the ingredients before in the sample
code of previous section \ref alib_expressions_calculator "2. Tutorial: Hello Calculator".

Because it is so simple, we just present the resulting code of the filter class:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_FilterGeneric

Et voilà: We can now use expression strings to filter the files. Here are two samples:

<b>Sample 1:</b> All files are included with constant expression \c "true":
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_TRUE
The output is:
  \snippet "DOX_EXPR_TUT_FF_TRUE.txt"     OUTPUT

<b>Sample 2:</b> All files are filtered out with constant expression \c "false":
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_FALSE
Which results to the empty output:
  \verbinclude "DOX_EXPR_TUT_FF_FALSE.txt"

While this demonstrates fast progress towards our aim to filter files, of course we have not
linked the expression library with this custom code example, yet. All we can do is providing
expressions that do not refer to the file given, hence either evaluate to \c true for any file
or to \c false.<br>
But before we feel free to start working on this, we first need to put one stumbling block aside.

 \I{################################################################################################}
## 4.3 Checking An Expression's Result Type ## {#alib_expressions_tut_ff_checkresult}
In the samples above we used simple, constant expressions \c "true" and \c "false". As we already
learned in chapter 3, these are built-in identifiers that return the corresponding boolean value.
Well, and a boolean value is what the filter needs. Other valid expressions would be

       5 > 3                 // constant true
       Year(Today) < 1984    // constant false

"Valid" here means, that the expression returns a boolean value! But what would happen if we
constructed the filter class with expression string

       1 + 2

which returns an integral value? The answer is that in method \b %Includes of the filter class
presented in the previous sections a run-time assertion would be raised in the following line of
code:

       return expression->Evaluate( scope ).Unbox<bool>();

The code \e unboxes a value of type \e bool, but it is not asserted that the result of the
evaluation is of that type. This quickly leads us to an enhanced version of that method:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_FilterCheckResultType

So here is some bad news: It is obvious, that there is no way around the effort of throwing and
catching exceptions (or otherwise do some error processing) as soon as software allows an end-user
to "express herself" by passing expression strings to software. Besides wrong return types, the
whole expression might be malformed, for example, by omitting a closing bracket or any other breach
of the expression syntax rules.

The good news is, that with the use of module \alib_expressions_nl, most - if not all -
of the errors can be handled already at compile-time! Once an expression is compiled, not much can
happen when an expression is later evaluated.

And this is also true for our current threat of facing a wrong result type: Due to the fact that module
\alib_expressions_nl implements a <em>type-safe compiler</em>, we can detect the result
type at compile-time.

Consequently, we revert our most recent code changes and rather check the result type already
right after the compilation:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_FilterCheckResultType2

\note
  It is up to the user of this library to decide how strict an implementation would be. Later in
  this tutorial, we will read permissions from the files, which might get "tested" using
  bitwise boolean operators, e.g.

           Permissions & OwnerWrite == OwnerWrite

\note
  Similar to programming languages, it could be allowed to shorten this expression to just

           Permissions & OwnerWrite

\note
  The result is an integral value, respectively a user-defined permission type that probably
  represents an underlying integral value. It is up to the filter class's method \b %Includes,
  to check for and interpret other types than boolean.
\note
  To provide the biggest degree of freedom, the result of box-function \alib{boxing;FIsTrue}
  might be returned instead of unboxing a boolean value. This interface is a good candidate to
  convert just any boxed value to a reasonable representation of a boolean value.
  Again, this is a design decision of the software that uses this library.
  It has to be documented to the end-user what type of expression results are allowed.

 \I{################################################################################################}
## 4.4 Exposing The Directory Entry To ALib Expressions ## {#alib_expressions_tut_ff_scope}
It is time to finally make our sample meaningful, namely to allow to filter selected files by
their attributes.

For this two steps are needed. The first again is extremely simple: We have to expose the current
directory entry of our filter loop to the file filter. All we need to do is to specialize class
\alib{expressions;Scope} to a custom version that provides the current object.<br>
Here is our new struct:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_Scope

With this in place, we just need two small changes in our file filter:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_ScopeFF

Now, the expression's \alib{expressions;detail::Program} that gets compiled in the constructor of
the filter class and that is executed by the built-in \alib{expressions;detail::VirtualMachine}
with the invocation of \alib{expressions::ExpressionVal;Evaluate}, potentially has access to
the directory entry.

The next section connects the final dots and leads to a working sample.

 \I{################################################################################################}
## 4.5 Implementing A Compiler Plug-In ## {#alib_expressions_tut_ff_cp}
We have come quite far without ever thinking about the syntax of the custom expressions that
we need to be able to filter files from a directory. Without much reflection of that, it
is obvious that filtering files by name should be enabled, maybe with support of \e "wildcards"
just like most users know them from the command prompt:

       ls -l *.hpp  // GNU/Linux
       dir *.hpp    // Windows OS

Thus, the first thing we need is to retrieve the file name from the entry. This is done with a
simple custom \e identifier. As it was said already, an \e identifier is a "parameterless function".
So why don't we need a parameter, namely the file entry in the expression syntax? Well, because
the entry is part of the scope. It is the central piece of custom information that the whole effort
is done for. Therefore, the expression:

       Name

should return the name of the actual directory entry that is "in scope". This is lovely simple,
so let's start. Again we start with a skeleton struct, this time derived from
\alib{expressions;CompilerPlugin}:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PluginSkeleton

To make use of the plug-in, we have again two small changes in the custom filter class:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PluginSkeleton2

With this, the plug-in is in place and during compilation it is now asked for help.
Parent class \b %CompilerPlugin exposes a set of overloaded virtual functions named \b TryCompilation.
In their existing default implementation each function just returns constant \c false, indicating
that a plug-in is not responsible. Thus, we have to make our plug-in now responsible for \e identifier \c "Name".
For this we choose to override one of the offered virtual functions as follows:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PluginOverride


As the code shows, the overridden function simply checks for the given name and the function
"signature". If both match, then a native C++ callback function is provided together with the
expected result type of that callback function.

The final step, before we can test the code is to implement the callback function. This is usually
done in an anonymous namespace at the start of the compilation unit of the plug-in itself.
The signature of any callback function that \alib_expressions_nl expects, is given with
\alib{expressions;CallbackDecl}. The documentation shows, that it has three parameters, the
scope and the begin- and end-iterators for the input parameters. The input parameters are
boxed in objects of class \alib{boxing;Box} and the same type is expected to be returned.

Because \alib_boxing_nl makes a programmer's life extremely easy, especially when used
with various kinds of strings, and because we are not reading any input parameters, the
implementation of the callback function is done with just one line of code:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PluginCallback

\note
  The callback function casts the scope object to our custom type \b FFScope. The function can
  trust that this succeeds, if each expression that gets compiled with the compiler that uses the
  plug-in gets a scope object of exactly this derived custom type passed when evaluated.

\note
  This is a sample of the "contracts" that have to be fulfilled by the user of library as
  already stated in previous chapter \ref alib_expressions_prereq_virtual_types "4.3 Use Of Virtual Types".
  Another of such \e contract can be seen with the code of the compiler plug-in:
  The type of the returned boxed value of the callback function has to match the type specified
  in the \b %TryCompilation. And furthermore, all code paths of the callback function have to
  return a box value of that very same type, regardless of the input parameters.

\note
  These are constraints that the user of this library has to ensure to be met. However, as it is recommended
  to implement classes like \b %FileFilter in our sample, this responsibility to keep the contracts
  is only shared within a few implementation units. What we previously called,
  "the custom 2nd level api", hides these constraints completely away, along with all other
  parts of \alib_expressions_nl.

We are set! Our first "real" filter expressions should work. Here are some filter loops and
their output:

<b>Sample 1:</b> :
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_Name1

Output:
  \snippet "DOX_EXPR_TUT_FF_Name1.txt"     OUTPUT

<b>Sample 2:</b> :
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_Name2

Output:
  \snippet "DOX_EXPR_TUT_FF_Name2.txt"     OUTPUT

<b>Sample 3:</b> :
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_Name3

Output:
  \snippet "DOX_EXPR_TUT_FF_Name3.txt"     OUTPUT

<b>This seems to work - mission accomplished!</b>

Some notes on these samples:
- Because the custom identifier \c Name does not introduce a custom type, but returns built-in
  type \alib{expressions;Types::String}, no operators have to be overloaded. In later chapters
  we will see what needs to be done when custom-types are returned by identifiers, functions or
  operators.
- Built-in expression function \b WildcardMatch accepts two strings, the first is the string
  that is matched, the second contains the wildcard string. Function \b WildcardMatch is provided
  with built-in compiler plug-in \alib{expressions::plugins;Strings}.
- The third sample uses an overloaded version of binary operator <c>'*'</c>, with left- and
  right-hand side being strings. This binary operator is also provided with
  plug-in \b %Strings and is just an \e "alias" for function \b WildcardMatch.

We could now easily continue implementing further identifiers, for example:
- \b IsDirectory: Returns \c true if the directory entry is a subfolderory, \c false if it is
  a file.
- \b Size: Returns the size of the file built-in type \alib{expressions;Types::Integer}.
- \b Date: Returns the date of the entry as built-in type \alib{expressions;Types::DateTime}.
- \b Permissions: Returns the access rights of the file or folder. For this, we would probably
  return an integral value and introduce further identifiers like \b GroupRead, \b GroupWrite,
  \b OwnerRead,... and so forth that return constants.

This would lead to inserting further <c>if</c>-statements to the custom plug-in, similar to
the one demonstrated for identifier \b Name.

Before this should be sampled, the next chapter explains the general possibilities of compiler
plug-ins and shows how the creation of a plug-in can be even further simplified.

\I{################################################################################################}
# 5. Compiler Plug-Ins And Class Calculus # {#alib_expressions_cpcc}

In the previous tutorial section, a fully working example program was developed that allows
using custom expression strings to filter files and folders by their name.

It was demonstrated how to attach a custom compiler plug-in to the expression compiler, which
selects a native C++ callback function at compile-time. This callback function is then invoked
each time a compiled expression is evaluated against a scope. The sample implemented the
retrieval of a string value from an object found in a custom specialization of class
\alib{expressions;Scope}.

\I{################################################################################################}
## 5.1 The Compilation Process ## {#alib_expressions_cpcc_process}
When an expression string gets compiled, such compilation is done in two phases. The first step
is called <b>"parsing"</b>.

The result of the parsing process is a recursive data structure called <em>"abstract syntax tree"</em>.
The nodes of this tree can represent one of the following types:
1. <b>Literals:</b><br>
   Literals are constants found in the expression strings. There are three types of literals
   supported: \b Integral (e.g., "42"), \b floating-point (e.g., "3.14") and \b string values (e.g., "Hello").
   A literal node is a so-called \e "terminal" node, which means it has no child nodes.

2. <b>Identifiers:</b><br>
   These are named tokens, starting with an alphabetical character and further consisting of
   alpha-numerical characters or character \c '_'.<br>
   Likewise literals, identifiers are terminal nodes.

3. <b>Functions:</b><br>
   Functions are identifiers followed by a pair of round brackets <c>'()'</c>. Within the brackets,
   a list of expressions, separated by a colon (<c>','</c>) may be given.
   Hence, functions are <em>n-ary</em> nodes, having as many child nodes as parameters are given
   in the brackets.<br>

4. <b>Unary operators:</b><br>
   These are nodes that represent an unary operation like "boolean not" (<c>'!'</c>) or arithmetic
   negation (<c>'-'</c>). These nodes have one child node.

5. <b>Binary operators:</b><br>
   Samples of binary operators are "boolean and" (<c>'&&'</c>) or arithmetic
   subtraction (<c>'-'</c>). These nodes have two child nodes.

6. <b>Ternary operators:</b><br>
   Only one ternary operator is supported. It is called <em>"conditional operator"</em> and parsed in the
   form <b>\"\e Q ? \e T : \e F\"</b> with \e Q, \e T and \e F being expressions.
   The result of the operation is \e T if \e Q evaluates to \c true, otherwise it is \e F.

This first phase of compilation that builds the \e AST (<em>abstract syntax tree</em>) usually does
not need too much customization.
\note
   Various customization options are nevertheless provided. The most important ones are described in:
   - \ref alib_expressions_operators_custom "9.2 Tutorial: Adding A Custom Operator"
   - \ref alib_expressions_operators_verbal "9.3 Verbal Operator Aliases"
   - \ref alib_expressions_details_literals "11.2 Literals"

It could be reasonably argued, that building this tree is all that an expression library needs to
do and in fact, many similar libraries stop at this point. What needs to be done to evaluate
an expression is to recursively walk the \e AST in a so-called "depths first search" manner, and perform
the operations. The result of the evaluation would be the result of the root node of the tree.

\alib_expressions_nl goes one step further, performing a <b>second phase</b> of compilation.
In this phase, the recursive walk over the \e AST is done. The result of the walk is an expression
\alib{expressions::detail;Program}. Such program is a list of "commands" which are later, when
the expression is evaluated, executed by a virtual stack machine.
(This stack machine is implemented with class \alib{expressions;detail::VirtualMachine}).

This second phase is where the customization takes place. When a node of the \e AST is translated
into a program command for the virtual machine, the compiler iterates through an ordered list of
\alib{expressions;CompilerPlugin}s to ask for compilation information. As soon as one plug-in
provides such info, the compiler creates the command and continues walking the tree.

Now, what does the compiler exactly "ask" a plug-in for and what information is included in the
question? To answer this, let us first look at the list of \e AST nodes given above. Of the
six types of \e AST-nodes listed, two do not need customization. These are \e literals and
the <em>ternary operator</em>. What remains is
- Identifiers and Functions,
- Unary operators and
- Binary operators.

It was mentioned before, that \alib_expressions_nl is type-safe. To achieve this, the
result type of each node is identified (deepest nodes first). Whenever a node with child nodes
is compiled, the result types of each child node has already been identified.

With this in mind, the input and output information that compiler plug-ins receive and return
becomes obvious. Input is:
- The node type and according information (e.g., the operator, the identifier or function name)
- The result type of each child

The output information is:
- A pointer to a native C++ callback function that will be invoked by the virtual machine when
  the program command resulting from the node is executed.
- The result type of that native C++ callback function.
- Alternatively to this, in case of "constant nodes", a constant result value may be returned
  (which likewise defines the node type). For example, built-in identifier \b "true" returns
  constant value boolean \c true.

To finalize this section, a quick hint to the benefits of taking this approach should be given:
- Compile-time type safety allows identifying almost all errors in user-defined expression strings
  at compile-time. On the one hand, this allows rejecting malformed expressions right at the
  moment they are given. If such detection was deferred to evaluation-time, then usually software
  has quite some effort to "undo" certain actions that the software did to prepare the evaluation.
- Both compile-time type safety and the fact that the \e AST is translated into a linear program
  of course increase compile time, but this is done in favor to evaluation time. In many use-case
  scenarios, there is an overwhelmingly high ratio of evaluations per expression. Therefore,
  this library is 100% optimized for evaluation performance, while compilation performance is
  considered pretty unimportant.
- Operation overloading avoids type checking at evaluation time and leads to very thin callback
  functions, many of them being just a single line of code. In addition, the implementation of
  the native C++ callback functions can be separated into various compilation units, as already
  demonstrated with the built-in plug-ins that each addresses a certain dedicated "topic", like
  \e "string handling", "date and time", etc.


\I{################################################################################################}
## 5.2 The Built-In Compiler Plug-Ins ## {#alib_expressions_cpcc_bipl}
With the information given in the previous subchapter, some important consequence can be noted:

\par
  <b>The compilation process works on \e "permutations" of the following information:</b>
  - <b>node types,</b>
  - <b>node type specific information (e.g., unary/binary operator type or identifier/function name) and</b>
  - <b>all types of child nodes.</b>

This fact in turn leads to the following statements:
- The compilation process fails, if no plug-in returns compilation information for a certain
  permutation.
- Each permutation may lead to different C++ callback function and result type.<br>
  (In the C++ language, this behavior is called "operator and function overloading".)
- A compiler plug-in with a higher priority, may replace (disable) an implementation of a permutation
  which would be responded by a plug-in of lower priority.

As a sample, let's take two simple expressions

       1 + 2
       "Result " + 42

Both expressions consist of two literal nodes, which are the two children of binary operator
<c> '+'</c>. As literals are not compiled using plug-ins, only the binary operator is passed
to the plug-ins. To successfully compile both, plug-ins have to be available that cover the
following permutations:

       binary op, + , integer, integer
       binary op, + , string, integer

For the addition of integer values, built-in compiler plug-in \alib{expressions::plugins;Arithmetics}
is responsible. For the concatenation of integer values to string values, plug-in
\alib{expressions::plugins;Strings} steps in.

The documentation of the plug-ins therefore mainly consist of tables that list permutations of
operators, function names and input types, together with a description of what is done in the
C++ callback function and what result type is to be expected.

The use of the built-in plug-ins is optional and configurable. Configuration is done by
tweaking member \alib{expressions;Compiler::CfgBuiltInPlugins} before invoking method
\alib{expressions;Compiler::SetupDefaults}. But a use-case to do so, is not so easy to find,
also due to the fact that custom plug-ins default to a higher priority and this way might
replace selected built-in behavior.

To implement a custom compiler plug-in, the following "bottom-up" approach is recommended:

- An application usually provides simple custom identifier names, which, for example, read
  property values from application objects defined from a specialized version of type
  \alib{expressions;Scope}. The compilation of such identifier should be implemented first.

- If an identifier callback function returns values of application-specific type,
  then in addition a reasonable set of operators overloaded for these types should to be
  implemented. (Obviously, if that was not done, only simple expressions, consisting just of the
  custom identifiers themselves, returning that custom type could be compiled.)

- If more complex custom functionality is needed, finally custom expression functions can
  be implemented. Of course, if such functions again introduce so-far unknown return types,
  operators for these types have to be implemented as well.


To finalize this chapter, some obvious facts should be named:
- Each and every calculation is performed by plug-ins. Even a simple <c>"1 + 2"</c> calculation
  might be handled by custom code.
- Usually, there is no need to omit the default plug-ins. There is only very, very small
  performance decrease on compiling an expression with more plug-ins installed. And there is
  absolutely no impact on the usually much more important evaluation performance.
- Three of the built-in types, namely
  \alib{expressions;Types::Integer},
  \alib{expressions;Types::Float} and
  \alib{expressions;Types::String} "emerge" from parsing literals.
- The other built-in types, namely
  \alib{expressions;Types::Boolean},
  \alib{expressions;Types::DateTime} and
  \alib{expressions;Types::Duration} instead emerge as being result types of built-in compiler
  plug-ins. For example, type \b %Boolean is a result type of identifier \b "True", as well as
  the result type of \alib{expressions;DefaultBinaryOperators::Smaller} (<c>'<'</c>) usable with
  various combinations of argument types provided with different built-in compiler plug-ins.
- The introduction of custom types is done by just introducing a custom plug-in that compiles
  \e AST nodes with returning such custom type. There is no need to register the types.
  (With the exception that for the purpose of the creation of human-readable compiler exceptions,
   method \alib{expressions;Compiler::AddType} is provided.)
- After the compilation process is done, the \e AST data structure is deleted.
- The compilation process is a little more complex than presented here. More details will be
  explained in later chapters, for example, in
  \ref alib_expressions_details_optimizations "11.5 Optimizations" and
  \ref alib_expressions_details_types "11.1 Types".


\I{################################################################################################}
## 5.3 Class CompilerPlugin ## {#alib_expressions_cpcc_class}
After a lot of theory was given, it is now quite straight forward to explain how
struct \alib{expressions;CompilerPlugin} is used.

The struct provides an inner struct \alib{expressions::CompilerPlugin;CompilationInfo} which is
the base of several derived further (inner) specializations. The base struct exposes the common base
of the input and all of the output information provided to, and received from compiler plug-ins.
According to the different node types of the parsed \e AST, the specializations
are:
- \alib{expressions::CompilerPlugin;CIUnaryOp}
- \alib{expressions::CompilerPlugin;CIBinaryOp}
- \alib{expressions::CompilerPlugin;CIFunction}

Along with this, for each of these structs, an overloaded virtual method called \b %TryCompilation
is defined. A custom plug-in now simply derives from the plug-in struct, and overrides one or more
of the virtual methods. The original implementation of the base struct returns constant
\c false. In the case that the given information corresponds to a permutation that the custom
plug-in chooses to compile, the plug-in needs to fill in the output parameters of the given struct
and return \c true.

\note
  A fourth specialization of \b %CompilationInfo is given with
  \alib{expressions::CompilerPlugin;CIAutoCast} together with a corresponding overloaded
  method \b %TryCompilation. Its purpose and use is explained in chapter
  \ref alib_expressions_details_types  "11.1 Types".

\I{################################################################################################}
## 5.4 Class Calculus ## {#alib_expressions_cpcc_calculus}

The architecture of the expression compiler and the use of according plug-ins was explained and
we could continue now with extending the sample plug-in given in section
\ref alib_expressions_tut_ff_cp "4.5 Implementing A Compiler Plug-In".

This would quickly lead to inserting a bunch of <c>if</c>-statements to the already overridden
method \b %TryCompilation. Considering all possible permutations of operators and types,
this result in repetitive code. To avoid this, the library provides an optional helper-class.<br>
All built-in compiler plug-ins (with the exception of \alib{expressions::plugins;ElvisOperator}
and \alib{expressions::plugins;AutoCast}) use this class and are therefore not
derived from \b %CompilerPlugin, but from \alib{expressions;plugins::Calculus}.

The trick with that type is that permutations of operators, identifiers, function names
and argument types are provided as static table data, together with the information of how
to compile the permutations.

Then in a custom constructor, these static tables are fed into a hash table that allow a performant
search. The custom plug-in does not need to furthermore override any \b %TryCompilation method,
as class \b %Calculus provides a default implementation that simply searches the hash table.

Consequently, all that these built-in plug-ins do is feeding their keys and corresponding callback
methods to the hash table during construction.
This is not just very efficient in respect to this library's code size and in respect to
the compilation performance of expressions, it also makes the creation of a plug-in an even more
simple and straight-forward task.

\par
  <b>Hence, the advice to library users is to also use helper-type
  \alib{expressions;plugins::Calculus} as the parent class for custom compiler plug-ins, instead
  of deriving from %CompilerPlugin.</b>

The permutations of function arguments that class \b %Calculus uses to identify static
compilation information, includes an option to keep a trailing portion of such arguments
variadic. A sample of such variadic function implemented using this helper-class is expression function
\ref alib_expressions_Strings_Formatting "Format".

We now go back to our tutorial sample and add more file filter functionality, by using this
helper-class \b Calculus.

\I{################################################################################################}
# 6. Tutorial: Extending The File Filter Sample# {#alib_expressions_tut_ffext}


\I{################################################################################################}
## 6.1 Replacing CompilerPlugin By Calculus ## {#alib_expressions_tut_ffext_calc}
Before we start adding new features to the sample code of section
\ref alib_expressions_tut_ff "4. Tutorial: Implementing A File Filter" the first task is
to refactor the sample to use helper-type \alib{expressions;plugins::Calculus}.

The already presented sample plug-in defined a callback function was:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PluginCallback

Furthermore our compiler plugin was derived from \b %CompilerPlugin and implemented method
\b %TryCompilation for functions (identifiers):
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PluginOverride

The callback function remains untouched. Struct \b FFCompilerPlugin is changed in three aspects:
- It is to be derived from struct \b %Calculus,
- it fills the \alib{expressions::plugins;Calculus::Functions;function table} (just one entry so far) and
- the own implementation of \b %TryCompilation is to be removed.

The resulting code of the plugin looks as follows:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PluginCalculus

\I{################################################################################################}
## 6.2 Adding More Identifiers ## {#alib_expressions_tut_ffext_ident}
We can now finally continue with adding more functionality to our file filter sample.
At the end of chapter
\ref alib_expressions_tut_ff_cp "4.5 Implementing A Compiler Plug-In"
we already thought about what we could add:
- \b IsDirectory: Returns \c true if the directory entry is a subfolderory, \c false if it is
  a file.
- \b Size: Returns the size of the file built-in type \alib{expressions;Types::Integer}.
- \b Date: Returns the date of the entry as built-in type \alib{expressions;Types::DateTime}.
- \b Permissions: Returns the access rights of the file or folder.

OK, let's do that! First we add some boxed values that define constants for permission rights.
This is still done in the anonymous namespace, hence the following boxes are on namespace scope,
just as the callback functions are:

 \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_MoreIdentifiersConstants

We are doing two casts here: The first is to get the underlying integral value from the
filesystem library's constants. If we did not do this, we would introduce a new type to
\alib_expressions_nl. In principle, this would not be a bad thing! The advantages and
disadvantages will be explained in a later chapter.<br>
The second cast is to convert the signed integral value to an \c unsigned one. Again, if we did not do this,
this would introduce a new type, namely \alib{uinteger}. Note that this library does
not provide built-in operators for unsigned integers.

With these casts, the permission values become compatible with built-in binary operators
\alib{expressions;DefaultBinaryOperators::BitAnd}, \alib{expressions;DefaultBinaryOperators::BitOr} and
\alib{expressions;DefaultBinaryOperators::BitXOr} which are defined for built-in type
\alib{expressions;Types::Integer}, which in turn is nothing else but a
\alib{integer}!


Next, we add the new callback functions:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_MoreIdentifiers

All that is left to do is "announcing" the availability of these constants and functions to
class \b %Calculus in the constructor of the custom plug-in. As shown before, functions are
added to table \alib{expressions::plugins;Calculus::Functions}. The constant, parameterless
functions are put into a simplified version of this table found with field
\alib{expressions::plugins;Calculus::ConstantIdentifiers}.<br>
The entries of both tables expect an object of type \alib{strings::util;Token}. This object
is used by class \b Calculus to match identifiers and functions found in expression
strings against the names that are defined by a plug-in.
With the use of class \b Token, a flexible way of optional name abbreviation is provided,
taking "CamelCase" or "snake_case" token formats into account.
In our case, for example, we allow all constant identifiers to be shorted to just two letters.
For example, Identifier \b "OwnerExecute" can be abbreviated \b "OE", \b "oe", \b "ownR", etc.

Here is comes the sample snippet:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_MoreIdentifiersPlugin

After all this theory and discussion, this is surprisingly simple and short code!
Our file filter is already quite powerful. Here are some sample expressions and their output:
  \verbinclude "DOX_EXPR_TUT_FF_More-1.txt"
  \verbinclude "DOX_EXPR_TUT_FF_More-2.txt"
  \verbinclude "DOX_EXPR_TUT_FF_More-3.txt"
  \verbinclude "DOX_EXPR_TUT_FF_More-4.txt"
  \verbinclude "DOX_EXPR_TUT_FF_More-5.txt"

\note
  Looking at the last sample: If you are wondering why file \c expressionslib.cpp is so huge, the
  answer is: it contains this whole manual and tutorial that you are just reading, created with
  marvelous \https{Doxygen,www.doxygen.org}!


\I{################################################################################################}
## 6.3 Adding Functions ## {#alib_expressions_tut_ffext_func}
The latest sample expression was:

          size > 81920

It would be nicer to allow:

          size > kilobytes(80)

Ok, let us add three functions. Here are the callbacks:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_Functions

The functions unbox the first parameter. For this, due to the <em>type-safe compilation</em> of
\alib_expressions_nl, neither the availability nor the type of the given argument needs
to be checked.

Next we need to define the function "signature", which is defining the number and types of
arguments that the functions expect. Class \alib{expressions::plugins;Calculus} allows us to do
this in a very simple fashion. It is just about defining an array of pointers to
\ref alib_expressions_prereq_sb "sample boxes". As all three simple functions
have the same signature (they all just receive one argument of type integer), we need only one
signature object:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_FunctionSignature

This was all we needed to prepare: here is the new version of the plug-in:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_FunctionsPlugin

Macro \ref CALCULUS_SIGNATURE simply provides two arguments from the one given: The pointer to
the start of the array along with the array's length. Those two values will be assigned to

fields \doxlinkproblem{structalib_1_1expressions_1_1plugins_1_1Calculus_1_1FunctionEntry.html;a3f9769a430a930630a75d41bc7e4055c;FunctionEntry::Signature}
 and
fields \doxlinkproblem{structalib_1_1expressions_1_1plugins_1_1Calculus_1_1FunctionEntry.html;aca52a8f1496207b9bc51ec6227168fd7;FunctionEntry::SignatureLength}
 of function table records.

And here is a quick test using one of the functions:
  \snippet "DOX_EXPR_TUT_FF_Func-1.txt"     OUTPUT

This worked well!

\note
   We have sampled above the creation of an array of pointers to boxes to denote the
   function signature. The built-in plug-ins of the library also need a certain set of signatures.
   These are collected in static struct \alib{expressions;Signatures}. In the upcoming samples,
   object \c TakesOneInt is removed and replaced by the corresponding field of the library, because
   custom code is very well allowed to use the built-in arrays.<br>
   Only for signatures that are not found with built-in functions, custom arrays have to be
   created prepared.

A picky reader might now think: well it is more efficient to use expression:

          size > 81920

instead of:

          size > kilobytes(80)

because the latter introduces a function call and hence is less efficient. But this is not the case,
at least not in respect to evaluating the expression against a directory entry. The evaluation
time of both expressions is exactly the same, because both expressions result in exactly the
same expression program.

The only effort for the library is at compile-time. While later chapter
\ref alib_expressions_details_optimizations "11.5 Optimizations"
will discuss the details, here we only briefly note what is going on:
The definition entry of the function table for function \b Kilobytes states
\alib{expressions::plugins;Calculus::CTI} in the last column. This tells class \b %Calculus that the function might
be evaluated at compile-time in the case that \b all arguments are constant. Because the single argument
given is constant literal \c 80, this condition is met. Thus, the callback function is invoked at
compile-time and instead of the function's address, the result value is passed back to the compiler.
The compiler notes this, and replaces the original command that created the constant value \c 80
with the constant result value \c 81920. This is why both expressions lead to exactly the same
program.<br>
In contrast to this, the identifiers of the previous chapter are marked as
\alib{expressions::plugins;Calculus::ETI}, which means <em>"evaluation-time invokable only"</em>.
The obvious rationale is, that these functions access custom data in the \b %Scope object and such
custom data is available only when the expression is evaluated for a specific directory entry.


\I{################################################################################################}
## 6.4 Adding Operators ## {#alib_expressions_tut_ffext_ops}
Next, some binary operator definitions are to be showcased.

We had implemented identifier \b Permissions to return a value of \alib{expressions;Types::Integer}
instead of returning the C++17 filesystem library's internal type. The advantage of this was
that the built-in bitwise-boolean operators defined for integral values, could instantly be used
with expressions. This was demonstrated in the above sample expression:

       (permissions & OwnerExecute) != 0

The disadvantage is that the filter expressions are not really type-safe.
An end-user could pass the expression:

       (permissions & 42) != 0

without receiving an error. While this is a design decision when using \alib_expressions_nl,
in most cases, type-safeness has definite advantages.
To achieve type-safeness, we now change the definition of the callback function of identifier
\b Permission as follows:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PermType

In the previous version we had casted the enumeration elements of \c fs::perms to its underlying
integral type. Now we are boxing the un-casted enumeration element value.

To denote type \c fs::perms as being the return type of identifier \b %Permission, we need a
\ref alib_expressions_prereq_sb "sample box". This is an easy task, we just randomly
choose one enumeration element and assign it to a new variable of type \b Box.

A next small change needed, results from a requirement of class \b Box: Global (or static) objects
must not be initialized with custom types (in this case with elements of enum \c fs::perms).
Such initialization has to happen after module \alib_boxing is duly bootstrapped.
Therefore, the initializations of all constant boxes, as well as of the sample box, is now moved to
the constructor of the compiler plug-in.

\note
  It is possible to customize \alib_boxing_nl to support the initialization of global
  boxes with custom types, but we do not do this exercise here.<br>
  Details are explained in the Programmer's Manual of
  \alib_boxing_nl with chapter \ref alib_boxing_more_opt_staticvt.


The new code for the compiler plug-in's constructor now is:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PermTypePlugin

Apart from initializing the constant boxes, the only new line of code is the definition
of the sample box for the return value, which is then used in the function table to denote the
return type of function \b Permissions.

\note
  The constant identifiers we had added to the sample returned a constant value and hence no
  need for a <em>"sample box"</em> was needed with them.<br>
  The other way round, the definition of object \c TypePermission could have been omitted and
  just one of the constant values, e.g., \c constOwnRead could have been used as the return type
  sample box in the function table entry.<br>
  We therefore define one unnecessary object of type \c %Box, which resides in the compiled
  software occupying 24 bytes, in favor to better readable code.
  Real-life plug-ins could find other solutions, e.g., using a preprocessor macro, to save this small
  overhead.

Let's see what happens if we try to compile the previous expression:
  \snippet "DOX_EXPR_TUT_FF_Operators-1.txt"     OUTPUT

The compiler throws a run-time exception, noting that operator \c '&' is not defined. The first
thing we want to fix is the output information of this \alib{exceptions;Exception} itself.
While in general it is not necessary to announce custom types explicitly, the exception is
that the human-readable information collected in exceptions thrown by the library
benefits from it. For just this purpose, method \alib{expressions;Compiler::AddType} is available.
Consequently, we add statement
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PermTypeAddTypeDef

to the constructor of our plug-in.<br>
With this in place, the exception thrown looks as follows:
  \snippet "DOX_EXPR_TUT_FF_Operators-2.txt"     OUTPUT

This looks better, but still its an exception. What it tells us is to define the operator.
We do this for a bunch of operators at once. Firstly, we need the callbacks for the operators:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PermTypeOperatorCallbacks

This is the first time that two parameters are read in the callbacks. It is done using simple
iterator arithmetics.

Struct \alib{expressions::plugins;Calculus} organizes compilation information on unary and binary
operators in a hash map. For filling the map, a convenience function is available that accepts
a simple array of information entries.
This array usually is defined in the anonymous namespace of the compilation unit:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PermTypeOperatorTable

For information about the meaning of the values of the table, consult the documentation of
\alib{expressions::plugins;Calculus::OperatorTableEntry}. But looking at the code, and reflecting
what was already presented in this tutorial, the meaning should be is quite self-explanatory.
It just should be noted, that also for operators, flags \alib{expressions::plugins;Calculus::CTI}
or \alib{expressions::plugins;Calculus::ETI} may be given.
If, like in our case, \b %CTI is specified, then at the moment that both operands are constant, the
compiler will optimize and the callbacks are pruned from the compiled expression.
This means, that, for example, sub expression:

       ( OwnerRead | GroupRead | OwnerExecute | GroupExecute )

will be reduced to one single constant in the compiled expression program, because each of the
identifiers returns a constant value.

Finally, in the constructor of the plug-in we now add the following line of code:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_PermTypeFeedTable

With this in place, the expression now compiles in a type-safe way:
  \snippet "DOX_EXPR_TUT_FF_Func-3.txt"     OUTPUT

\note
  The expression got slightly changed from:

       (permissions & OwnerExecute) != 0

\note
  to

       (permissions & OwnerExecute) == OwnerExecute

\note
  which is logically the same. To allow the first version, a replacement identifier for integral value
  \c 0, e.g., \b NoPermission had to be inserted to the plug-in.



\I{################################################################################################}
## 6.5 Implementing Auto-Casts ## {#alib_expressions_tut_ffext_autocasts}
To finalize this tutorial part of the documentation, a last quite powerful feature of
\alib_expressions_nl is presented. We re-think again what we did in the previous
section:
- We changed identifier \b Permissions to return values of custom type \c fs::perms.
- Because of this, the built-in bitwise boolean operators are not applicable anymore and therefore,
  we implemented operators for the custom type.

For the latter, there is an alternative available, called "auto-casting". If no compiler
plug-in compiles an operator for a given argument or pair of arguments, then the compiler invokes
method \doxlinkproblem{structalib_1_1expressions_1_1CompilerPlugin.html;a28e497e20681d76d79b1913f6fb0f19d;CompilerPlugin::TryCompilation(CIAutoCast&)}
 for each plugin.
In the case that one of the plug-ins positively responds by providing one or two
<em>"cast functions"</em>, the compiler inserts the cast functions for one or both arguments
and performs the search for an operator of this now new type (respectively pair of types)
a second time.

We add such "auto-casts" for allowing the compiler to convert \c fs::perms to \b integer.
This approach obviously has the following consequences:
- "Permissions" still is a distinguishable type.
- Specific operators using the type can be defined and will be selected with preference.
- Existing binary operators for integers become available to the type in addition, but with
  a lower priority. (Because auto-casting is performed only if no direct match exists.)
- Expression terms using the type are "type-unsafe", the same as in the first implementation,
  when identifier \b Permission returned an integer type.
  For example, expression <c>{ Permissions == 0 }</c> is well compiled and evaluated.

\note
  It is a design decision of \alib_expressions_nl that auto-cats are performed only once
  per expression operator and not a second or third time:
  If after one plug-in performed auto-casts, still no matching operator is found, then compilation
  fails.<br>
  One rationale for this behavior is that repeated auto-casts would undermine the "type-safeness"
  of \alib_expressions_nl too much by leading to maybe unforeseen effects.<br>
  For example, consider a custom application supposed over two different enum types, which both are
  exposed by expression function result values. Now, the application programmer might want to allow
  built-in bitwise boolean operators that are defined on integral values to become compatible
  with both types. For both types she would implement an auto-cast to integer type.
  The effect is that the operators work, but also combinations of the custom enum types and integer
  literals. So far, so good. Now, if the library would do repetitive
  auto-casts, then even binary operators that contain different custom types for \e lhs and \e rhs
  would be compiled and evaluated. Hence, this would conclude in even less strict type-safeness!

\note
  If strict type-safeness is demanded, auto-casts are to be avoided! Instead, the implementation
  of all needed operators for the custom types is to be preferred. The decision for either
  approach is a matter of taste and field of application.

\I{################################################################################################}
### 6.5.1 Implementing Auto-Casts Using A Compiler Plug-in ### {#alib_expressions_tut_ffext_autocasts_using_compiler_plugin}

To implement this, we revert the most recent code changes (the operator callbacks, the binary
operator table and the single line of code that feeds the table to parent \b %Calculus).

As a replacement, we add the following callback function which casts a permission type to
\alib{expressions;Types::Integer}:
 \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_AutoCastCallback

A cast function takes one parameter of the originating type and returns the converted value.
In this sample, this is trivial. Sometimes more complex code is needed.
Casting one type to another might even include memory allocations to create a certain custom type
from a given value. Such allocations, have to be performed using the
provided, \alib{expressions;Scope} object, which, as explained \ref alib_expressions_scopes "later",
optionally is of custom type. Allocations done by auto-casting are then to be deleted when the
scope object is deleted or reset.

\note
  In the case that casting is done at compile-time (due to
  \ref alib_expressions_details_optimizations "expression optimization") such allocations will be
  then be performed using the compile-time scope which survives the expression's life cycle.

With this casting callback function in place, we add the following method to the custom plugin:
 \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_AutoCastCompilation

Likewise with previous solution, our sample expression compiles with the very same result:
  \snippet "DOX_EXPR_TUT_FF_Func-4.txt"     OUTPUT

However, unlike the recent implementation, compilation is not type-safe in respect to mixing
\c fs::perms with integer values:
  \snippet "DOX_EXPR_TUT_FF_Func-5.txt"     OUTPUT

\I{################################################################################################}
### 6.5.2 Implementing Auto-Casts Using Class Calculus ### {#alib_expressions_tut_ffext_autocasts_using_calculus}

This was a rather simple use case, but a very frequent one. Again class \alib{expressions;plugins::Calculus},
may be used to avoid the code from the previous section and replace it by just one line of
static table data.

To demonstrate this, we remove the code of the previous section. This is not only in consideration
of method \b TryCompilation, but we can also remove the custom callback function that
performed the cast!

Now, we add the following statement to the constructor of our custom compiler plug-in which
is already derived from class \b %Calculus
 \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_TUT_FF_AutoCastCalculus

A quick check confirms that our sample expression compiles and evaluates the same as before:
  \snippet "DOX_EXPR_TUT_FF_Func-6.txt"     OUTPUT

The various options and fields of table \alib{expressions::plugins;Calculus::AutoCasts} are not
explained here, but well documented with inner struct \alib{expressions::plugins;Calculus::AutoCastEntry}.<br>
Further documentation is found with method \doxlinkproblem{structalib_1_1expressions_1_1plugins_1_1Calculus.html;a465e9a2e6063dc79859507d264b88943;Calculus::TryCompilation(CIAutoCast&)}
,
including some hints about the use cases not covered by this helper-class, hence those
that demand the implementation of a custom \b %TryCompilation method.


\I{################################################################################################}
# 7. Built-In Expression Functionality # {#alib_expressions_builtin}

The types, identifiers, functions and operators presented in this manual section are to be named
"built-in" in that respect, that they are available by default. But the truth is, that
they are implemented using the very same technique of providing compiler plug-ins, that has
been explained in the previous section. This way, this built-in logic is fully optional and
can be easily switched off, partly or completely.

For doing so, class \b %Compiler offers a set of configurable flags, gathered in member
\alib{expressions;Compiler::CfgBuiltInPlugins}. The flags are declared with enumeration
\alib{expressions;Compiler::BuiltInPlugins} and are evaluated in method
\alib{expressions;Compiler::SetupDefaults}. Field \b %CfgBuiltInPlugins defaults to
\alib{expressions;Compiler::BuiltInPlugins::ALL}. With this information, it is easy to understand that
the following code of setting up a compiler:
 \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_EmptyCompiler

leads to a compiler that does not compile anything.

It should be very seldom that disabling of one or more of the built-in compiler plug-ins is needed.
Here are some rationals for this statement:
- While in a certain domain-specific use case scenario, e.g., string handling or math functions
  are not needed, it should not impose a negative effect if they are provided.

- The compiler plug-ins are invoked by the compiler in an ordered fashion. Custom plug-ins have
  highest priority. This way, selected functionality can be "overridden" by a custom plug-in by
  just compiling permutations operators/function names and their arguments, which otherwise
  would be compiled by a lower-prioritized, built-in plug-in.

- The availability of unused functionality provided by unnecessary plug-ins has no effect on the
  evaluation-time of expressions. There is only a very small downside in compile-time.


\I{################################################################################################}
## 7.1 Completeness Of Built-In Functionality ## {#alib_expressions_builtin_completeness}

In the default setup (all built-in plug-ins are active), \alib_expressions_nl is considered
to be \e "complete" in respect to providing all reasonable operators for permutations of arguments
of all built-in types.

This manual does not elaborate about implications in respect to such \b completeness in the case
that selected built-in plug-ins are omitted. It is up to the user of the library to think about
such implications and provide alternatives to the built-in functionality that is decided to
be left out.<br>
By the same token, there is no mechanism to disable the compilation of selected built-in compiler
plug-ins and with that, their inclusion in the library code. If such is to be achieved in favor
to code size, a custom build-process has to be set up.

\I{################################################################################################}
## 7.2 Types ## {#alib_expressions_builtin_types}

As explained in previous sections of this manual, the introduction of types to \alib_expressions_nl is
performed in an implicit fashion: New types are introduced at the moment a callback function
chooses to return one and consequently, the corresponding compiler plug-in announces this return
type of such callback to the compiler during the compilation process.

Therefore, the set of built-in types is resulting from the set of built-in compiler plug-ins.
Nevertheless, the library design opted to collect sample boxes for the set in struct
\alib{expressions;Types}, which is defined right in namespace \ref alib::expressions.

It is notable that no built-in support for unsigned integral values is provided. In the unlikely
event that this is needed for any reason, such support can quite easily by implemented by a
custom plug-in. As a jump-start, the source code of class \alib{expressions::plugins;Arithmetics}
might by used.

Furthermore, all possible sizes of C++ integral values are collectively casted to
\alib{integer}, which on a 64-bit platform to 64-bit signed integral value and to a 32-bit
signed integral on a 32-bit platform.

Finally, \alib{expressions;Types::Float} is internally implemented using C++ type \c double.
No (built-in!) support for C++ types \c float and <c>long double</c> is provided.

This reduction of used types simplifies the built-in plug-ins dramatically and reduce the libraries
footprint, as it reduces the number of type-permutations to a reasonable minimum.

Due to the type-safe compilation, adding custom types has no impact on evaluation performance of
operators and functions that use the built-in types (or other custom types).


\I{################################################################################################}
## 7.3 Arithmetics ## {#alib_expressions_builtin_arithmetics}
What is called \e "arithmetics" with this library comprises the implementation of unary and
binary operators for permutations of types \alib{expressions::Types;Boolean},
\alib{expressions::Types;Integer} and \alib{expressions::Types;Float}.

The operators and some few identifiers and functions are collectively implemented and documented
with plug-in \alib{expressions::plugins;Arithmetics}.


\I{################################################################################################}
## 7.4 Math Functions ## {#alib_expressions_builtin_math}
Fundamental mathematical functions like trigonometrical, logarithms, etc. are collectively
implemented and documented with plug-in \alib{expressions::plugins;Math}.

\I{################################################################################################}
## 7.5 String Expressions ## {#alib_expressions_builtin_string}
Plug-In \alib{expressions::plugins;Strings} provides quite powerful string operations.
The library here benefits tremendously from underling modules \alib_strings and \alib_boxing.

For example, operator \alib{expressions::DefaultBinaryOperators;Add} (<c>'+'</c>) allows concatenating
two strings, but also a string with "any" other built-in or custom type.
The latter - namely that there is no need to define an overloaded expression operator for strings
and custom types - is achieved by leveraging box-function \alib{boxing;FAppend}.
Consult the \ref alib::boxing "user manual of ALib Boxing" for details on how to implement this interface
for your custom types, to allow end-users to concatenate your types to strings within expressions.

All built-in string features, including:
 - wildcard matching (using \c '*' and <c>'?'</c>)
 - matching of regular expressions, as well as
 - a powerful <b>Format(String, ...)</b> function

is given with \alib{expressions;plugins::Strings;the plug-in's documentation}.

\I{################################################################################################}
## 7.6 Date And Time Expressions ## {#alib_expressions_builtin_datetime}

The built-in types \alib{expressions;Types::DateTime} and \alib{expressions;Types::Duration}
represent \alib classes \alib{time;DateTime} and \alib{time;TimePointBase::Duration} of the same
name.
The corresponding %Expression functionality is implemented and documented with plug-in
\alib{expressions::plugins;DateAndTime}.

If a user of \alib_expressions_nl prefers to use different, own or 3rd-party types, then
support for such type needs to be implemented by a custom plug-in. Such implementation may
be created by copying the source code of built-in plug-in \b %DateAndTime and replacing
all corresponding code lines to work with the desired date and time types. If wanted, some or all
identifiers might remain the same and even if the built-in plug-in may be kept active. In the
latter case, no clash of identifiers would occur. This is because the custom plug-in would
usually be inserted to the compiler with a higher priority than the priority of
the built-in plug-in.



\I{################################################################################################}
## 7.7 Conditional Operator ## {#alib_expressions_builtin_ternary}

The conditional operator <c>Q ? T : F</c> is the only ternary operator, and (for technical reasons)
not implemented as a plug-in.
In contrast, it is hard-coded in the details of this library's implementation.<br>
This is not considered a huge limitation, as there is no obvious use case, why this operator
should be overloaded: It's meaning is the same for any use of types.

The conditional argument \c 'Q', which of course could result in a value of any built-in or custom
type, is interpreted as a boolean value using box-function \alib{boxing;FIsTrue}.
While a default implementation for this box-function exists that evaluates any custom type,
a provision of this interface for a custom type may be used to override this default implementation.

For result arguments \c 'T' and \c 'F', the only requirement that needs to be fulfilled is that
both are of the same type or that a compilation plug-in for auto-casting them to a joint type exists.

This means:
- Support for custom types is given, if both arguments share the same custom type.
- To support a mix of at least one custom types with a different built-in or custom type,
  corresponding auto-cast mechanics have to be provided.

A variant of the conditional operator is the so-called "Elvis Operator", <c>A ?: B</c>.
This variant is duly supported by this library and compiled as binary operator
\alib{expressions;DefaultBinaryOperators::Elvis} just as any other operator is - including that the compiler
tries to perform an auto-cast, if needed.

Built-in compiler plug-in \alib{expressions::plugins;ElvisOperator} handles this operator
for built-in types as well as for custom-types, in the case that \c 'A' and \c 'B' share the
same type.

Similar to the conditional operator, the default
implementation invokes box-function \alib{boxing;FIsTrue} on argument \c 'A' and decides
whether \c 'A' or \c 'B' is chosen. This default behavior can be changed by just
implementing the elvis operator, likewise any other operator would be implemented.


\I{################################################################################################}
## 7.8 Auto-casts ## {#alib_expressions_builtin_autocast}

Built-in compiler plug-in \alib{expressions::plugins;AutoCast} offers casting proposals
to the compiler in respect to the built-in types.

For details on the casting facilities, consult the class's
\alib{expressions::plugins;AutoCast; documentation}.


\I{################################################################################################}
# 8 Scopes ## {#alib_expressions_scopes}
As it was demonstrated in
\ref alib_expressions_tut_ff_scope "4.4 Exposing The Directory Entry To ALib Expressions",
a customized (derived) version of struct \alib{expressions;Scope} is passed to method
\alib{expressions;ExpressionVal::Evaluate}, and the very same object is passed to the callback
functions, when the expression program is executed by the built-in virtual machine.
As a result, a custom callback function can rely on the fact that it is possible to
dynamically cast parameter \c scope back to the custom type and access <em>"scoped data"</em>
which exposes an interface into the application that uses \alib_expressions_nl.

This is the most obvious and also intuitively well-understandable role of struct \b %Scope.
But there are other important things that this class provides.

\note
  Struct \b %Scope is a good (or bad!) sample of this library's design principle discussed
  in chapter \ref alib_expressions_prereq_bauhaus "3.4 Bauhaus Code Style". Remember,
  that the software that uses \alib_expressions_nl is supposed to hide struct
  \b %Scope with all its publicly accessible members, the same as most other details of the
  entity find in module \alib_expressions_nl.<br>
  In other words: not all members that are accessible should be accessed. Some care has to
  be taken and do not make them accessible to the consumers of your code.

\I{################################################################################################}
## 8.1 Provision Of The Evaluation Stack ## {#alib_expressions_scopes_stack}
Struct \b %Scope incorporates field \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;af1f402e9cac81ef3ad0a982590344472;Scope::Stack;expressions::Scope::Stack}.
This vector is used by the built-in virtual stack-machine implementation during evaluation.
This way, it was possible to implement the machine's execution method without using any data
exposed by the machine (in fact, the machine is a pure static class).

The important consequence is:
\attention
  In multithreaded programs, it is allowed to evaluate one instance of an expression in parallel
  by different threads.<br>
  To do this, each thread must have its own instance of class \b %Scope.<br>
  In other words, \b %Scope instances must not be used in parallel execution threads,
  neither for evaluating the same expression, nor for evaluating two different expressions.
               
\I{################################################################################################}
## 8.2 Scope Allocations ## {#alib_expressions_scopes_allocations}

The next important role that struct \b %Scope fulfills is to provide fields that allow to
<b>allocate temporary data</b>.
With a simple arithmetic expression like this:

       1 * 2 + 3

no allocations are needed. The reason is that the intermediate results that the multiplication
of integral numbers produces, can be (and is) stored as a \e value in the \b %Box object that
operator \c '*' returned.
However, an expression with string operations like this:

       "Hello " + "beautiful " + "world!"

incorporates an intermediate result (in this case <c>"Hello beautiful "</c>) that cannot be
boxed by value.
Storage space for such intermediate results has to be allocated somewhere, because the \b %Box object
stores only a pointer to a character array, together with its length. Thus, the string buffer
itself needs to be allocated.

For this reason, struct \b %Scope incorporates some built-in "facilities" to allocate data.
Those are briefly:
- \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;a86cd560d324c9e4df047b254ea96ef36;Scope::Allocator}<br>
  This is a simple but powerful "monotonic memory allocator" of type \alib{monomem;TMonoAllocator}.
  As it name indicates, it allocates memory buffers and thus leads to high run-time performance,
  because it reduces heap allocation costs.
  And even better: the allocated buffers remain allocated and are reused when an expression is
  evaluated against a next scope. This usually reduces the need for memory allocations to
  zero, starting with the second evaluation.

- \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;a45b12fe152f8e6a90341987ef35c958e;Scope::NamedResources}
<br>
  A hash map of pointers to objects of type\alib{expressions;ScopeResource}. Its purpose and
  use will be discussed in a later section.
   
\par Here come the important rules:
\attention
  1. All instances of that are placed in one of the above-mentioned containers have to be allocated
     using the provided allocator.
     This is because resetting or deleting the scope does not free the objects in a normal way.
     It only destructs these objects. Their memory will be reused with resetting the allocator.
     In other words: if such an object would be created using keyword \c new and then added to the
     container, a memory leak would occur.
  2. Custom specializations of the class (which anyway have to be created for the purposes
     discussed before) may provide other fields that can be used to allocate memory resources,
     tailored to the type of objects needed.<br>
     With those, it has to be ensured, that method
     \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;a372de693ad40b3f42839c8ec6ac845f4;Scope::Reset;expressions::Scope::Reset}
     is overridden to free all resources.<br>
     And: when overriding this method, it has to be ensured that the original virtual function of the
     base class is invoked as well, because built-in plug-ins allocate resources by using the
     built-in features of struct \b %Scope.

\I{################################################################################################}
## 8.3 Compile-Time Scopes ## {#alib_expressions_scopes_ctscope}

So far, we talked only about the instance of struct \b %Scope that is provided to method
\alib{expressions;ExpressionVal::Evaluate}. But there is a second scope object created, that is
called the "compile-time scope".
If you reconsider the sample expression from the previous section:

       "Hello " + "beautiful " + "world!"
All three string-type arguments are constant string literals. The operator \c '+' is implemented
with built-in compiler plug-in \alib{expressions::plugins;Strings}, which defines the operator being
"compile-time-evaluable". As explained in the tutorial section, this means that at the moment
all arguments are constant, struct \alib{expressions::plugins;Calculus} (the parent of struct
\b %Strings), invoked the operator's callback function at compile-time. Callback functions
rely on a scope object, e.g., for memory allocation, as just discussed.

For this reason, a compile-time singleton of type \b %Scope is created and provided to the
callback functions during compilation of constant terms. Intermediate results may this way
be stored either in the compile-time scope instance or in the evaluation-time instance.
The latter is cleared upon each evaluation, while the data allocated in the compile-time scope is
cleared only with the deletion of the expression.


\I{################################################################################################}
## 8.4 Custom Compile-Time Scopes ## {#alib_expressions_scopes_custom_ctscope}

If at least one custom callback function that is compile-time invokable uses custom allocation
tools which are only provided by a corresponding custom version of the type, then - ooops!

To support this scenario, a derived version of class \b %Compiler has to be created, which
re-implements virtual method \alib{expressions::Compiler;createCompileTimeScope}. This method
is internally called with method \alib{expressions::Compiler;Compile} to allocate the
compile-time scope.

If the conditions described above are met, then this method has to be overwritten to return a
heap-allocated custom scope object. This object will internally be deleted with the deletion of
the expression.

Custom callback functions can then rely on the fact that the compile-time scope object can
be dynamically casted to the custom type and use its custom allocation facilities.


\I{################################################################################################}
## 8.5 Using Compile-Time Resources At Evaluation Time ## {#alib_expressions_scopes_ctresources}
So far, things had been still quite straight forward. Let us quickly recap what was said about
scopes:
- Scopes are used to provide evaluation time data from the application.
- Scopes are used to allocate data for intermediate and final expression result objects.
- Due to the fact that \alib_expressions_nl provides the feature of compile-time optimization,
  a compile-time scope is created with the compilation of the expression.
- Of course, any evaluation specific field of custom scopes which provide access to the application
  data are \e nulled in the compile-time scope and accessing them is unspecified behavior
  (usually the program crashes).
- The life-cycle of the compile-time scope is bound to the life-cycle of a compiled expression.
  Its method \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;a372de693ad40b3f42839c8ec6ac845f4;Scope::Reset;expressions::Scope::Reset} is only called with destruction.
- The life-cycle of the evaluation-time scope is user dependent. It is strongly recommended to
  create one object and reuse this object for each evaluation (as sampled in the tutorial).
  Its \b %Reset method is automatically (internally) called at the beginning of method
  \alib{expressions;ExpressionVal::Evaluate}. This also means, that the expression result object
  of the previous call to \b %Evaluate becomes invalid (if it is relies on evaluation-time
  allocated data).

This concept of having two separated scope objects in certain cases is extended.
In general terms, it could be phrased as follows:

<b>%Compiler plug-ins may choose to create resources at compile-time, which are not intermediate
constant results, but which are objects used at evaluation time.</b>

To support this, two further fields are found in class \b %Scope:
- \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;aed843e776cc359a842b37e5a4491e84a;CTScope;Scope::CTScope}<br>
  During evaluation, this pointer of the evaluation time scope, provides access to the compile-time
  scope. In contrast to this, at compilation-time this field equals \c nullptr, because the given
  scope object already is the compile-time scope.<br>
  (Consequently, simple inline method \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;ab71ec93e1c7127735d3b6e17dd393ffa;Scope::IsCompileTime;expressions::Scope::IsCompileTime} just checks this
  pointer for being \c nullptr.)
- \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;a45b12fe152f8e6a90341987ef35c958e;NamedResources;Scope::NamedResources}<br>
  This hash-map provides access to "named" resources. It is provided to allow creation and storage
  of resources at compile-time, which are then retrieved during evaluation using a specific resource
  name.

The following sample taken from the built-in compiler plug-in \alib{expressions::plugins;Strings}
nicely demonstrates what can be achieved with this concept.

\I{################################################################################################}
## 8.6 Sample For Using Compile-Time Resources At Evaluation Time ## {#alib_expressions_scopes_ctrressample}

Built-in compiler plug-in \alib{expressions::plugins;Strings} provides expression function
\b %WildcardMatch, which matches a pattern against a given string. For example, expression

           WildcardMatch( "MyPhoto.jpg", "*.jpg" )

evaluates to \c true.
\note
   The function is alternatively available through overloaded binary operator \c '*'.
   The sample expression of above can this way also be phrased:

           "MyPhoto.jpg" * "*.jpg"

To implement this function, internally helper-class \alib{strings::util;TWildcardMatcher;WildcardMatcher}
provided by underlying library module \alib_strings is used. For performance reasons,
this class implements a two-phased approach: First, the "pattern" string (here <c>"*.jpg"</c>)
is parsed and translated into a set of internal information. Then, for performing a
single match, this internal information is used, which is much faster than if the pattern
still had to be parsed.

In most cases, an expression string given by an end-user would contain a non-constant
string to match and a constant pattern string, like in the following expression:

           filename * "*.jpg"

In this case, it would be most efficient, if the pattern string was passed to an instance
of \alib class \b %WildcardMatcher at compile-time, while at evaluation time this exact
matcher would be used to perform the match.

This setup already explains it all:
- The instance of \b %WildcardMatch is to be created at compile-time and stored as a named
  resource in the compile-time scope object. For the name (storage key) of the resource,
  the pattern string is used.

- At evaluation time, the object is retrieved by accessing the named resources of the compile-time
  scope and the match is performed against the first given function argument, while the second
  argument is used to search a named resource. If one is found, the already set-up matcher
  is used.

You might not be interested in the details of the implementation and skip the rest of the
chapter. The code becomes a little more complex than usual plug-in code. The reason is that
helper-struct \alib{expressions::plugins;Calculus} does not provide a mechanism to support this.

We start with defining the resource type, derived from struct \alib{expressions;ScopeResource}.
This simply wraps a matcher object and its sole purpose is to have a virtual destructor that
later allows internal code to delete the matcher:
  \snippet "expressions/plugins/strings.cpp"   DOX_EXPR_CTRES_1

Next, method \b %TryCompilation needs to be overwritten to be able to fetch the function:
  \snippet "expressions/plugins/strings.cpp"   DOX_EXPR_CTRES_2

The methods starts by invoking the original implementation of parent \b %Calculus.
Because the wildcard function is compile-time invokable, in the (unlikely) case that both parameters are
constant, a constant value would be returned. Only if one of the parameters is non-constant, then
the callback is set to callback function \c wldcrd.

The following <em>if-statement</em> selects this case that we are interested in:
  \snippet "expressions/plugins/strings.cpp"   DOX_EXPR_CTRES_3


If the second parameter is not an empty string, obviously a constant value was given.
\note
  The reason for this is: if it was not a constant string, a sample box value was provided to only
  denote the argument type and this would be of zero length.<br>
  Furthermore, it might be mentioned that we do not need to check the parameter type(s) to
  be of string type, because if it was not, then the call to the original implementation that was
  done at the beginning had not returned <c>true</c>!

Now, we extract the pattern string and combine it with prefix <c>"_wc"</c> to a key string to
store the resource:
  \snippet "expressions/plugins/strings.cpp"   DOX_EXPR_CTRES_4

It may happen, that an expression uses the same pattern twice. In this case, the same matcher
object can be used. Therefore, it has to be checked, if a matcher with that same pattern already
exists. If not, it is created:
  \snippet "expressions/plugins/strings.cpp"   DOX_EXPR_CTRES_5

After that, \b %TryCompilation exits, signaling compilation success.
All that is left to do is the implementation of the callback function.
At the beginning the function checks if this is an evaluation-time invocation. In this case,
it searches a named resource according to the given pattern string.
If this is found, the function uses the resourced matcher and exits:
  \snippet "expressions/plugins/strings.cpp"   DOX_EXPR_CTRES_6

If it was not found, then two possibilities are left:
- Either, this is a compile-time invocation with both arguments being constant, or
- This is an evaluation-time invocation, with the right-hand side argument not being constant.
 
Consequently, all that is needed to be done now, is to perform the match operation by using a local,
one-time matcher object:
  \snippet "expressions/plugins/strings.cpp"   DOX_EXPR_CTRES_7


\I{################################################################################################}
# 9. Operators # {#alib_expressions_operators}

\I{################################################################################################}
## 9.1 Built-In And Custom Operators And ## {#alib_expressions_operators_default}

In its default configuration, module \alib_expressions_nl parses and compiles an
almost complete set of operators known from the C++ language. Not supported by default are
for example, assignment operators like e.g., <c>'+='</c> or increments <c>'++'</c>.
Operators included are:

<b>Unary Operators:</b><br>
- Positive, <c>'+'</c>
- Negative, <c>'-'</c>
- BoolNot, <c>'!'</c>
- BitNot, <c>'~'</c>
- Indirection, <c>'*'</c>

<b>Binary Operators:</b><br>
- Multiply       , <c>'*'</c>
- Divide         , <c>'/'</c>
- Modulo         , <c>'%'</c>
- Add            , <c>'+'</c>
- Subtract       , <c>'-'</c>
- ShiftLeft      , <c>'<<'</c>
- ShiftRight     , <c>'>>'</c>
- Smaller        , <c>'<'</c>
- SmallerOrEqual , <c>'<='</c>
- Greater        , <c>'>'</c>
- GreaterOrEqual , <c>'>='</c>
- Equal          , <c>'=='</c>
- NotEqual       , <c>'!='</c>
- BitAnd         , <c>'&'</c>
- BitXOr         , <c>'|'</c>
- BitOr          , <c>'^'</c>
- BoolAnd        , <c>'&&'</c>
- BoolOr         , <c>'||'</c>
- Assign         , <c>'='</c>

<b>Special Operators:</b><br>
- Ternary         , <c>'Q ? T : F'</c>
- Elvis           , <c>'A ?: B'</c>
- Subscript       , <c>'[]'</c>

Not only the operators were taken from C++, but in the case of binary operators, also the
definition of their precedence.


The built-in operators are set by method \alib{expressions;Compiler::SetupDefaults} if flag
\alib{expressions;Compilation::DefaultUnaryOperators}, respectively
\alib{expressions;Compilation::DefaultBinaryOperators} is set in bitfield
\alib{expressions;Compiler::CfgCompilation}.

Internally the following approach is taken:
- The \e resourced \ref alib_enums_records "ALib Enum Records" of enumeration class
  \alib{expressions;DefaultUnaryOperators} respectively
  \alib{expressions;DefaultBinaryOperators} is loaded.
- For each enumeration element, the information is passed to a call to
  \alib{expressions;Compiler::AddUnaryOperator}, respectively
  \alib{expressions;Compiler::AddBinaryOperator}.
- Dependent on flag \alib{expressions;Compilation::AliasEqualsOperatorWithAssignOperator}
  the precedence of the <em>assign-</em>operator is "patched" to the level of the <em>equal-</em>operator.


This is a rather simple process, and thus it is similar simple to intervene and customize
the operators. While removing what is built-in is seldom necessary, adding an operator
might be wanted. This is exercised in the next section.


\I{################################################################################################}
## 9.2 Tutorial: Adding A Custom Operator ## {#alib_expressions_operators_custom}

With what was described in the previous chapter, the following options of customizing the operators
parsed and compiled by module \alib_expressions_nl can be taken:
- Built-in operators can be completely disabled by just clearing the flags in field
  <b>%Compiler::CfgCompilation</b>.
- \alib{resources;ResourcePool} of singleton \alib{EXPRESSIONS}
  can be altered to change operator symbols or precedences.
- Methods \alib{expressions;Compiler::AddUnaryOperator}, respectively
  \alib{expressions;Compiler::AddBinaryOperator} may be called prior or after
  \b %SetupDefaults to define additional custom operators.
- Single built-in operators might be simply removed from public fields
  \alib{expressions;Compiler::UnaryOperators} and
  \alib{expressions;Compiler::BinaryOperators} after the invocation of \b %SetupDefaults.

As a sample, the goal is to have a new binary operator <c>'{}'</c> that allows
to format the right-hand side operand according to a format provided with the left-hand side
operand. Let's first check what happens if we just start and use the operand:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_OPS_1

This produces the following exception which indicates that parsing the expression fails due
to a syntax error:
  \snippet "DOX_EXPR_OPS_1.txt"     OUTPUT

Now we define the operator:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_OPS_2

We give the operator a high precedence, on the level of operator <c>'*'</c>. The operator
precedence values are documented with \alib{expressions;DefaultBinaryOperators}.<br>
The exception changes to:
  \snippet "DOX_EXPR_OPS_2.txt"     OUTPUT

Obviously, now the parser recognized the operator. This single line of code, was all we needed
to do to define the operator.

To get a working sample, a compiler plug-in that compiles the operator for left-hand side strings
and any right-hand side type is needed. Here it is:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_OPS_Plugin

With the plug-in attached:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_OPS_3

The expression compiles and results in:
  \verbinclude "DOX_EXPR_OPS_3.txt"


\attention
  While "verbal" operator names are allowed as aliases of operators (see next section),
  operator symbols must not contain alphanumerical characters and character <c>'_'</c>
  (underscore).

\note
  - Any changes in respect to operator setup has to be made before invoking method
    \alib{expressions;Compiler::Compile} for the first time, because with that, the internal
    parser is created (once) and configured according to these settings.
    Later changes will have no effect or result in undefined behavior.
  - The registration of custom compiler plug-ins may be done before or after modifying the operator
    setup of the compiler.


\I{################################################################################################}
## 9.3 Verbal Operator Aliases ## {#alib_expressions_operators_verbal}

End-users that are not too familiar with programming languages might find it easier to use
verbal operators. Instead of writing:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_OPS_verbal_2

they prefer:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_OPS_verbal_1


Such sort of "verbal" expressions are supported and enabled by default, with the concept
of <em>"Verbal Operator Aliases"</em>. As the term explains already, verbal operators can
not be defined with this library as being full featured "stand-alone" operators, but only
as aliases for existing symbolic operators.
\note
  The rationale behind this design decision is that usually, verbal operators just in fact are
  aliases. Now, with restricting verbal aliases to be aliases, the number of "real" operators
  does not increase by adding verbal aliases. In the case it did, the number of permutations of
  operators and types that had to be overloaded would drastically increase.

The default built-in (resourced) verbal operator aliases are:
 | Verbal Operator      | Is Alias For
 |----------------------|--------------------------------
 | \b Not               | Unary operator <c>'!'</c>
 | \b And               | Binary operator <c>'&&'</c>
 | \b Or                | Binary operator <c>'\|\|'</c>
 | \b Sm                | Binary operator <c>'<'</c>
 | \b Smaller           | Binary operator <c>'<'</c>
 | \b Smeq              | Binary operator <c>'<='</c>
 | \b Smaller_or_equal  | Binary operator <c>'<='</c>
 | \b Gt                | Binary operator <c>'>'</c>
 | \b Greater           | Binary operator <c>'>'</c>
 | \b Gteq              | Binary operator <c>'>='</c>
 | \b Greater_or_equal  | Binary operator <c>'>='</c>
 | \b Eq                | Binary operator <c>'=='</c>
 | \b Equals            | Binary operator <c>'=='</c>
 | \b Neq               | Binary operator <c>'!='</c>
 | \b Not_equals        | Binary operator <c>'!='</c>

Likewise the operators themselves, \alib_expressions_nl defines the names and alias
operators using \e resourced \ref alib_enums_records "ALib Enum Records" assigned to
enumeration class \alib{expressions;DefaultAlphabeticBinaryOperatorAliases}.<br>
The resource data is processed by method \alib{expressions::Compiler;SetupDefaults} dependent
on flag
\alib{expressions::Compilation;DefaultAlphabeticOperatorAliases} of bitfield
\alib{expressions::Compiler;CfgCompilation} (which is set by default).

Additional flag \alib{expressions::Compilation;AlphabeticOperatorsIgnoreCase} controls whether
the alias names are matched ignoring letter case (which is also set by default).

Class \b %Compiler simply stores the alias information in its public hash tables
\alib{expressions::Compiler;AlphabeticUnaryOperatorAliases} and
\alib{expressions::Compiler;AlphabeticBinaryOperatorAliases} which can be altered prior or
after the invocation of \b %SetupDefaults, but before a first expression is compiled.

<b>Some further notes:</b><br>
- Even with flag \alib{expressions::Compilation;AlphabeticOperatorsIgnoreCase} cleared, no two
  verbal operator aliases that only differ in letter case must be defined
  (e.g the definition of "or" in parallel to "OR" is forbidden).
- There are five possible configuration settings to normalize verbal operator names.
  See flag \alib{expressions::Normalization;ReplaceVerbalOperatorsToSymbolic} for more information.
- If the resources of built-in alias operators are changed (e.g., for translation/localization),
  it is allowed to set single names of the predefined enum element names to empty strings.
  These will be ignored with method \b %SetupDefaults. By the same token, when changing the
  resources, completely different values and meanings may be used, because the enum class
  \b %DefaultAlphabeticBinaryOperatorAliases is exclusively used for accessing its enum records.
- The concept of <em>verbal operator aliases</em> must not be confused with the concept
  of <em>operator aliases</em> performed with compiler plug-ins (explained in next section).
  Verbal aliases are defined globally and a compiler plug-in will never "see" the alias names as
  those get translated to the aliased operator internally before the compilation is performed.
- Identifiers/functions may not be equal to \b unary verbal operator aliases. By default the only
  unary verbal operator is \b 'not'.<br>
  In contrast, identifiers/functions may have the same name as verbal binary operator aliases.
  (The expression parser can distinguish both by the context.)



\I{################################################################################################}
## 9.4 Operator Aliases ## {#alib_expressions_operators_aliases}
This library does some effort to support operator aliases, which is about telling the system
that an operator used with certain argument types is to be considered equal to
another operator using the same argument types.

The only occasion where this is internally used, is with combinations of boolean, integer and
float types and bitwise operators <c>'~'</c>, <c>'&'</c> and <c>'|'</c>: Any use of these
operators a mix of these types - excluding those permutations that only consist of integers -
are optionally aliased to boolean operators <c>'!'</c>, <c>'&&'</c> and <c>'||'</c>.
It would have been less effort to just define the bitwise operators for the types to perform
boolean calculations! So, why does the library do the effort then?

The motivation for taking the effort comes from normalization. While the library should be
configurable to accept the expression:

       date = today & size > 1024

it should at the same time be configurable to "normalize" this expression to:

       date == today && size > 1024

Maybe, custom fields of application identify other operators where such aliasing
is reasonable as well.

The following parts of the library's API are involved in operator aliasing:
- Inner structs \alib{expressions::CompilerPlugin;CIUnaryOp} and
  \alib{expressions::CompilerPlugin;CIBinaryOp} of class \b %CompilerPlugin:<br>
  Both structs provide string field \b %Operator by reference and thus allow a compiler plug-in to change
  the operator string one aliased by the given one.
- Helper-class \b %Calculus with methods
  \alib{expressions::plugins::Calculus;AddOperatorAlias} and
  \alib{expressions::plugins::Calculus;AddOperatorAliases},
  which simplify feeding entries of operator alias definitions stored in hash map
  \alib{expressions::plugins::Calculus;OperatorAliases}.
- Normalization flag \alib{expressions::Normalization;ReplaceAliasOperators} which controls
  if alias operators are replaced in the normalized expression string.


\I{################################################################################################}
## 9.5 Array Subscript Operator ## {#alib_expressions_operators_subscript}

The "array subscript operator" <c>'[]'</c> is only in so far an exceptional binary operator, as it is
parsed differently than other binary operators.
While usually the operator is placed between the left-hand side (\e Lhs) and right-hand side
(\e Rhs) arguments, the subscript operator is expressed in the form

       Lhs[ Rhs ]

In any other respect it is completely the same! The only built-in use of the operator is
with lhs-type \b %String and rhs-type \b %Integers. With it, the substring at the given position
of length \c 1 is returned.

Its use and meaning is of course not bound to array access.
For example, with the right-hand side operands of type \b %String, a mapped access to
pairs of keys and values can be realized. To implement this, the left-hand side type would
be a custom type returned by an identifier, say \b %Properties. Now, if the subscript operator
was defined for this type and strings, expressions like

       Properties["HOME_PATH"]

are possible. The operator's callback function could catch certain key values and return
appropriate results from objects accessible through the custom scope object.


\I{################################################################################################}
## 9.6 Unary Operator For Nested Expressions ## {#alib_expressions_operators_nested}
It was not talked about <em>nested expressions</em> yet. This is concept is introduced
only with the next chapter, \ref alib_expressions_nested "10. Nested Expressions"

Here, we just quickly want to explain that this operator exists, that it has a special meaning and
how it can be changed.

The definition of the operator is made with field \alib{expressions;Compiler::CfgNestedExpressionOperator}.
Its default value is <c>'*'</c>, which in C/C++ is also called the <em>"indirection operator"</em>.
With this default definition, expression:

           date < today && *"myNested"

refers to nested expression \c "myNested".

Changing the operator needs to be done before invoking
\alib{expressions;Compiler::SetupDefaults}. Should operator definitions be changed as explained
in the previous chapters, it is important to know that the nested expression operator itself has to
be duly defined. In other words: Specifying an operator with field
\b %CfgNestedExpressionOperator does not define the operator.

The operator internally works on string arguments which name the nested expression that is addressed.
However, to overcome the need of quoting names of nested expressions, a built in mechanism is
provided that allows omitting the quotes. This feature is enabled by default and controlled
with compilation flag \alib{expressions::Compilation;AllowIdentifiersForNestedExpressions}.
For this reason, by default, the sample expression given above can be equally stated as:

            date < today && *myNested

Note that this does \b not introduce a conflict with defined identifiers or function names.
For example, if a nested expression was named \b "PI", just as math constant identifier \b PI, then
still the following works:

       5.0 * PI    // multiplies 5 with math constant PI
       5.0 * *PI   // multiplies 5 with the result of nested expression named "PI".


When changing the nested expression operator, some thinking about the consequences is advised.
Other candidates for nested expression operators may be <c>'$'</c>, <c>'%'</c> or
<c>'@'</c>, which are more commonly used to denote variables or other "scoped" or "nested"
entities. But exactly for this reason, module \alib_expressions_nl opted to default
the operator to <c>'*'</c>. Often, applications offer to provide expressions via a command
line interface, which in turn allows using <b>%bash CLI</b> and any scripting language.
The asterisk character <c>'*'</c> seems to be least clashing with such external environments.<br>
Therefore, we recommend to do some careful thinking about potential conflicts in the
desired field of application and use case environments, before changing this operator.




\I{################################################################################################}
\I{################################################################################################}
# 10. Nested Expressions # {#alib_expressions_nested}
Often certain "terms" of an expression are to be repeated in more than one expression.
Furthermore, it sometimes is valuable to be able to split an expression into two parts, for example
parts that have different levels of validity. This latter is often the case when it comes
to "filtering" records from a set. A first filter might be a more general, long living expression.
A second expression adds to this filter by applying more concrete demands. In the filter sample,
there are two ways of achieving this:
1. First apply a filter using the general term and then apply a second filter.
2. Apply one filter with both terms concatenated via boolean operator <c>&&</c>.

Besides being faster, the second has one huge advantage: it is up to the end user if the single
filter refers to a different term - or not. There is no need to hard-code two filters into a
software.

These thoughts bring us to the concept of "nested expressions", which is referring to expressions
from expressions!

The foundation to achieve such feature, is first to provide a way to store expressions and
retrieve them back using a key value.

\I{################################################################################################}
## 10.1 Named Expressions ## {#alib_expressions_nested_named}

Module \alib_expressions_nl provides a built-in container to store compiled expressions.
At the moment an expression is stored, a name has to be given and that is all that makes an
expression a named expression.

So far in this manual, we had compiled expressions using method \alib{expressions;Compiler::Compile}.
What is returned is an \e anonymous expression. It is not named.
To create a named expression, method \alib{expressions;Compiler::AddNamed} is used. This method
internally compiles the expression and stores it with the given name. The expression itself is
not returned, instead information about whether an expression with that name existed (and thus was
replaced).

For retrieval of named expressions, method \alib{expressions::Compiler;GetNamed} is offered and
for removal method \alib{expressions::Compiler;RemoveNamed}.

\note
  The optional internal storage of expressions are the reason why the library addresses
  expressions exclusively through type \alib{expressions;Expression} which evaluates
  to <c>std::shared_ptr<Expression></c>. With this, a named expression is automatically deleted
  if it is removed from the storage and not externally referred to.

By default, <b>letter case is ignored</b> when using the given name as a storage key.
Hence adding \c "MYEXPRESSION" after adding \c "MyExpression" replaces the previous instance.
This behavior can be changed using compilation flag
\alib{expressions::Compilation;CaseSensitiveNamedExpressions}. Changes of this flag
must be done only before adding a first named expression. Later changes leads to is undefined
behavior.

Named expressions is not too much of a great thing if viewed as a feature by itself.
But it is the important prerequisite for <em>nested expressions</em> explained in the following
sections.


\I{################################################################################################}
## 10.2 Nested Expressions Identified At Compile-Time ## {#alib_expressions_nested_operator}

The simplest form of addressing nested expressions is by using unary operator <c>'*'</c>,
which allows embedding a named expression into another expression.

While the operator defaults to being <c>'*'</c>, this can be changed as described in
\ref alib_expressions_operators_nested "9.6 Unary Operator For Nested Expressions".

The operator expects a string providing the name, but for convenience, this string does not
need to be quoted, but may be given like identifiers are.

\note
   This behavior is configurable with compilation flag
   \alib{expressions::Compilation;AllowIdentifiersForNestedExpressions} and enabled by default.
   If the name of an expression does not conform to the
   \ref alib_expressions_details_identifiers "11.3 Identifiers/Functions",
   for example, if it begins with a numeric character, then the otherwise optional quotes have to
   be provided.

With this operator, expressions:

       * *"MyNestedExpression"
       * *MyNestedExpression

both simply "invoke" the expression named \c "MyNestedExpression" and return its result.

Of course, this sample was just the shortest possible. Nested expressions can be used just
like any other identifier:

       GetDayOfWeek( Today ) == Monday && *MyNestedExpression

This expressions evaluates to \c true on mondays and if named expression \c  "MyNestedExpression"
evaluates to true in parallel.

One might think that this is all that has to be said about nested expressions. But unfortunately
this is not. An attentive reader might have noticed some important restriction with nesting
expressions like this: Because \alib_expressions_nl is a type-safe library, the compiler
can compile operator <c>&&</c> in the above sample only if it knows the result type of
\c "MyNestedExpression". As a consequence, we have to state the following rule:


\attention
  %Expressions addressed with the unary operator have to be existing at the time
  the referring expression is compiled.

Let us simply have a try. The following code:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_OP_1

Produces the following exception:
  \snippet "DOX_EXPR_NESTED_OP_1.txt"     OUTPUT

The exception tells us that this is a "compile-time defined nested expression". This indicates
that there will be a way out, what we will see in the next chapter, but for the time being
let us fix the sample by adding the named expression upfront:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_OP_2

Now this works:
  \verbinclude "DOX_EXPR_NESTED_OP_2.txt"

The compiler found the nested expression, identified its return type and is now even able to
use it in more complex terms like this:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_OP_3

This results in:
  \verbinclude "DOX_EXPR_NESTED_OP_3.txt"

But there is also another restriction that has to be kept in mind with the use of the
unary operator for nested expressions.<br>
While this sample still works well:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_OP_4
  \verbinclude "DOX_EXPR_NESTED_OP_4.txt"

This expression does not work:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_OP_5

as it throws:
  \verbinclude "DOX_EXPR_NESTED_OP_5.txt"

While - due to the compile-time optimization of \alib_expressions_nl - the constant
concatenation term <c>"MyNested" + "Expression"</c> is still accepted, the compiler complains
if we use the function \c random. The compiler does not have the information that
<c>random>=0</c> evaluates to constant true, and hence the term is not optimized and not
constant.<br>
The exception names the problem, which leads us to a second rule:

\attention
  %Expressions names addressed with the unary operator have to be constant at compile-time.

This is obvious, as the expression has to exist and be known. But still, it is a restriction.

There are many use cases, where still this simple operator notation for nested expressions is
all that is needed. For example, imagine a set of expressions is defined in an INI-file
of software. If the software loads and compiles these "predefined" expressions at start,
a user can use them, for example, with expressions given as command line parameters.
This way, a user can store "shortcuts" in the INI-file and use those as nested expressions
at the command line.

A final note to compile-time nested expressions: After an expression that refers to a named
nested expression is compiled, the named nested expression may be removed using
\alib{expressions;Compiler::RemoveNamed}. The program of the outer expression stores the
shared pointers to all compile-time nested expressions used. While after the removal from the
compiler the nested expression is not addressable for future nesting, the nested expression is only
deleted at the moment the last expression that refers to it is deleted!

\I{################################################################################################}
## 10.3 Nested Expressions Identified At Evaluation-Time ## {#alib_expressions_nested_function}

In the previous section we saw the first samples of nested expressions. The unary operator
<c>'*'</c> was used to address nested expressions.
These nested expressions suffer from two restrictions:
1. The nested expression has to be defined before using it.
2. The name of the nested expression must not be an expression itself, but rather a constant
   string term or an identifier.

Let us recall what the reason for this restriction was: The compiler needs to know the result
type of the nested exception to continue its type-safe compilation.

The way to overcome this restriction is to use function <b>Expression()</b> instead of
unary operator <c>'*'</c>.

\note
  Likewise the unary nested expression operator is configurable with member
  \alib{expressions::Compiler;CfgNestedExpressionOperator} of class \b %Compiler, the name, letter case
  sensitivity and abbreviation options of the nested expression function is configurable with
  member \alib{expressions::Compiler;CfgNestedExpressionFunction}.<br>
  It just defaults to <b>"Expression()"</b>

This function has three overloaded versions.
The first is using just one parameter of a string-type, and is 100% equivalent to the use of the
unary expression operator - including its restrictions.

The second overload takes a <em>"replacement expression"</em> as its second value.
This is how it may be used:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_FUNC_1

The output of this sample is:

  \verbinclude "DOX_EXPR_NESTED_FUNC_1.txt"

As you see, although the nested expression is was not defined, this sample now compiles. The compiler
uses the result type of the second parameter and assumes that the expression will return the same
type. And even more, the expression even evaluates! On evaluation it is noticed that the
expression does not exist, hence the result of the "replacement expression" is used.
While in this case the replacement is simply value <c>-1</c>, any expression might be
stated here. Even one that contains a next nested expression.


We extend the sample by adding the nested expression:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_FUNC_2
  \verbinclude "DOX_EXPR_NESTED_FUNC_2.txt"

As a "proof" that the nested expression is identified only at evaluation time, the following
sample might work:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_FUNC_3
  \verbinclude "DOX_EXPR_NESTED_FUNC_3.txt"

Above, we said that the compiler "assumes" that the named expression addressed, has the same
return type. The following code shows that this was the right verb:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_FUNC_4

No exception is thrown on compilation. The compiler does not check the return type at compile-time.
The simple reason is: At the time the expression becomes evaluated, the named expression might
have been changed to return the right type. This is why the return type is only checked at
evaluation time. Let's see what happens when we evaluate:
  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_FUNC_5
  \verbinclude "DOX_EXPR_NESTED_FUNC_5.txt"

This shows, that with using function <b>Expression()</b> we are a little leaving the
secure terrain of expression evaluation: While the only exceptions that can happen at
evaluation-time had been ones that occurred in callback functions
(for example, in expression <c>"5 / 0"</c>), with nested expressions that are identified only at
evaluation-time, we have a first exception that is thrown by the virtual machine that
executes the expression program.

At the beginning of this section, a third overload of <b>Expression()</b> was mentioned.
We postpone its documentation to the next manual section, and end this chapter with another quick
note:

Likewise with unary operator <c>'*'</c> for nested expressions, compilation flag
\alib{expressions::Compilation;AllowIdentifiersForNestedExpressions} allows omitting the quotes
and accept identifier syntax instead. Hence, this expression is compiling fine:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_FUNC_6

In addition, with this form of embedding nested expressions, also the restriction
of expression names being constants, fell. This way, the sample of a random name is now allowed:

   \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_FUNC_7


\I{################################################################################################}
## 10.4 Forcing The Existence Of Nested Expressions ## {#alib_expressions_nested_throw}

The previous two chapters explained the differences between nested expressions that are
identified at compile-time and those identified at evaluation time.

Compile-time nested expressions are usually expressed with unary operator <c>'*'</c>, but can
also be expressed when using only one parameter with nested expression function <b>Expression()</b>.

Evaluation-time nested expressions are expressed by giving a second parameter to
function <b>Expression()</b>, which provides the "replacement expression" for the case that
a nested expression of the name given with the first parameter does not exist.

In some situations however, a user might not want to provide a "replacement expression". It might
be rather in her interest, that an expression just fails to evaluate in the case that the
nested expression is not found. For example, if this indicates a configuration error of a
software.

Such behavior can be achieved by adding a third parameter to <b>Expression()</b>. This third
parameter is keyword \c "throw".

We take the sample of the previous section where the expression was not defined, which resulted
in default value <c>-1</c>. The only difference is the use of keyword <c>throw</c>:

  \snippet "docsamples/DOX_EXPR_TUTORIAL.cpp"   DOX_EXPR_NESTED_FUNC_41

The output of this sample is:

  \snippet "DOX_EXPR_NESTED_FUNC_41.txt"     OUTPUT

So, why do we need the second parameter which previously gave the "replacement expression" when
this is not evaluated? Well, the only purpose of the replacement expression is to determine the
nested expression's return type. Otherwise it is ignored. In fact, it is not optimized out
and its result will be calculated with each next evaluation of an expression against a scope.
Different to other areas, where the library puts some effort in optimization, here this was
omitted. An end-user simply should be noted to put a constant "sample value" for this parameter.
A user that uses this third version of the nested expression function, is supposed to be a "pro"
and understand the impacts.

\note "throw" is the only keyword of \alib_expressions_nl!


\I{################################################################################################}
## 10.5 Automated Named Expressions ## {#alib_expressions_nested_config}

As described in a previous section, a prerequisite for the nested expression feature is to have
\ref alib_expressions_nested_named "named expressions". Methods
\alib{expressions::Compiler;AddNamed},
\alib{expressions::Compiler;GetNamed} and
\alib{expressions::Compiler;RemoveNamed} of class \b %Compiler had been already described
briefly.

Nested expressions often can be seen as building blocks of other expressions. software might want
to provide a predefined and/or configurable set of expressions to be usable within end user's
expressions.<br>
To support such scenario, a mechanism is needed that allows retrieving (and compile) named
expression strings, right at the moment an unknown identifier of a nested expression occurs during
the compilation of the main expression.

Module \alib_expressions_nl offers abstract virtual class \alib{expressions;ExpressionRepository}
which offers a customizable implementation of such mechanism. This interface is used as follows:
- With field \alib{expressions;Compiler::Repository}, class \b Compiler exposes a public pointer to
  an object of type \alib{expressions;ExpressionRepository}.
- If the field is set, named expressions that are referred by other expressions (during compilation),
  and that have not been previously defined by a prior invocation of method
  \alib{expressions;Compiler::AddNamed} will be defined on the fly by method
  \alib{expressions;Compiler::GetNamed}. For this, \b GetNamed will use the expression repository
  to retrieve an expression string associated with the given identifier.

In other words, method \alib{expressions::Compiler;GetNamed} supports a "lazy" approach to compile
nested expressions "on the fly" as needed. The expression strings are received
using the abstract virtual method \alib{expressions;ExpressionRepository::Get}.

A built-in implementation of this interface class is provided with class \alib{expressions;StandardRepository}.
This implementation retrieves expression strings from
1. Static resource strings as provided with module \alib_camp.
2. External configuration data, like command line parameters, environment variables, INI-files or any
   other custom resource that is attached to the configuration object.

\note The second option is available only if the module \alib_variables_nl is included in the
      \alibbuild.

For the details of using the built-in implementation, consult the
\alib{expressions;ExpressionRepository;reference documentation} of class \b ExpressionRepository.

The creation of an own implementation that receives predefined expression strings in a custom way,
should be a straight-forward task.

\I{################################################################################################}
## 10.6 Summary and Final Notes On Nested Expressions {#alib_expressions_nested_summary}

Nested expressions is a powerful feature of \alib_expressions_nl, but also needs some
thoughtful and knowledgeable user because of the different approaches of compile-time and
evaluation-time defined nested expressions.<br>
If software offers an end-user to "express herself", a certain level of understanding is
anyhow required. Often software hides expression syntax behind a graphical user interface with
checkboxes and input fields per "attribute", e.g., to define an email filter and creates an
expression string unseen by the user in the background.
Then, only in a certain "expert mode" an end-user is allowed to freely expressions, which then
may be more complex and probably even allow to "address" nested expressions that such end-user
had defined in external configuration resources.

So, it is a bit of a task to define the user interface and user experience when it comes to
allowing expressions. This library tries to cover the broad spectrum of use cases and this can
probably be noticed in the area of nested expressions very well.


To end this chapter about nested expressions, some final hints and notes should be collected here:
- To disallow nested expressions, simply fields
  \alib{expressions::Compiler;CfgNestedExpressionOperator} and
  \alib{expressions::Compiler;CfgNestedExpressionFunction} of class \b %Compiler are to be cleared.
- It is undefined behavior if a nested expression that is successfully identified at compile-time
  is deleted and the referring expression still evaluated afterwards.
- To disallow compile-time nested expressions only, compilation flag
  \alib{expressions::Compilation;AllowCompileTimeNestedExpressions} is to be cleared.
- To disallow evaluation-time nested expressions only, field
  \alib{expressions::Compiler;CfgNestedExpressionFunction} is to be cleared. In this case
  nested expressions are available only using the unary operator.
- Evaluation-time nested expressions may be changed (replaced) before evaluating an expression
  that uses them.
- As long as only compile-time nested expressions are used, no circular nesting can occur.
  As soon as evaluation-time nested expressions are used, circular nesting might happen.
  The library detects such circular nesting an throws
  \alib{expressions;Exceptions::CircularNestedExpressions} during evaluation. The exception
  includes informational entries of type \alib{expressions;Exceptions::CircularNestedExpressionsInfo}
  that list the "call stack" of named expressions that caused the circle.


\I{################################################################################################}
# 11. Detail Topics # {#alib_expressions_details}
In the previous chapters of this manual, most features of module \alib_expressions_nl
have been touched, either as tutorial sample code or in a more theoretic fashion.
This chapter now provides a list of in-depth discussions on different dedicated topics.

\I{################################################################################################}
## 11.1 Types # {#alib_expressions_details_types}
It was a lot said about the intermediate or final result types of expressions in various
sections of this manual. The use of underlying run-time type information library \alib_boxing
with its very "seamless" nature, helped to implement \alib_expressions_nl tremendously.

But being so seamless, it is not so easy to understand all aspects of its use and meaning
in this library. Therefore, this quick chapter tries to review various aspects of the library
from the angle of types. For simplification of writing and reading this chapter, this is done
with a list of bullet points.

- A prerequisite to fully understand how type information is handled by this library, is to
  read and understand the documentation of \alib_boxing.
  <p>

- This library often uses "sample boxes" that just transport type information. This concept is
  explained in \ref alib_expressions_prereq_sb   "3.2 Type Definitions With \"Sample Boxes\"".
  <p>

- While custom types are to be registered with the compiler using \alib{expressions::Compiler;AddType},
  such registration is purely used for the generation of exception messages or other sorts of
  end-user information. It is not necessary otherwise.
  <p>

- This library follows a "type-safe paradigm". This means that during compilation of expressions
  each of its terms is determined in respect to what exact type it will result to during
  evaluation. The disadvantages of this approach are not easy to be named: A non-type-safe library
  would just look a lot different and it could name advantages along that ultimately different
  design. Hence we rather talk about the consequences of this library's type-safe approach:
  - Exceptions caused by malformed expression strings, are as far as possible happening at compile-time.
  - The choice of overloaded operators and functions happens at compile-time, which allows a very
    performant evaluation. In fact, at evaluation time, the run-time type information included
    in boxed intermediate result values can mostly be completely ignored in that respect that a
    callback function does not need to perform checks on its input values in respect to their type:
    Their implementation just unboxes values without doing type checking.<br>
    In this matter, it might be hinted to the fact that library \alib_boxing_nl is designed
    to not throw run-time exceptions. It rather \ref alib_mod_assert "raises an error" in
    debug-compilation, e.g., if inconsistent types are tried to be unboxed. In release-builds,
    software simply has undefined behavior (crashes).
  - Overloaded versions of one operator (or function) can be implemented in very separated
    software units, not "knowing" each other and not interfering with each other.
  <br><br><p>

- While it may seem to a user of this library, that for each possible permutation of input
  parameter types, a distinct callback function has to be provided, this is not the case.
  The concept of quite strict assignment of such permutations to corresponding callbacks is
  not hard-coded in the depth of this library, but rather "voluntarily suggested" with using
  high-level helper-struct \alib{expressions;plugins::Calculus}. By using the underlying type
  \alib{expressions;CompilerPlugin} directly, it is possible to provide just one callback
  function that is enabled to process various combinations of input parameters. The disadvantage
  of doing so is that this moves the effort of identifying the types to evaluation-time,
  which implies a drop of evaluation performance. This is why this library "suggests" to use
  struct \b %Calculus as the foundation for custom plug-ins.
  <p>

- Likewise to the previous note, the concept of "variadic function parameters" is not something
  that arises from the depth of the library, but again comes only with the use of helper
  struct \alib{expressions;plugins::Calculus}. It may need some time and thinking about the
  relationship of structs \b %CompilerPlugin and \b %Calculus to fully grasp the differences and
  benefits of each. And by the use of virtual functions, in some situations it makes very much
  sense to mix both concepts, by inheriting a custom plug-in from struct \b %Calculus but
  still overwriting parts of the underlying interface of \b %CompilerPlugin.<br>
  Such mixed approach is also used with some of the built-in compiler plug-ins.
  <p>

- The concept of auto-casting of types is located somewhere in the middle! Auto-casts can be fully
  prevented by providing either a dedicated callback function for each permutation of types or
  by doing "manual" casts just within a callback function that accepts multiple permutations.
  Still, this library takes the effort of supporting auto-cast in the details of the implementation
  of the compilation process which assembles the evaluation program.
  How it is done can be seen a little like  "a last call for help" before throwing a compilation
  exception.

  The compiler does these calls in two occasions: When a binary operator could not be compiled and when
  terms \c T and \c F of ternary conditional operator <c>Q ? T : F</c> are not of the same type.
  In this moment, the compiler just calls for help by asking each plug-in for an auto-cast
  of one or both of the types. It does this only once! After a first plug-in provided some
  conversion, it is retried to compile the actual operator. If this still fails, the exception
  is thrown, although it might have been possible that a next plug-in provided a different
  cast that would lead to success.<br>
  This of course is a design decision of the library. Complexity was traded against
  effectiveness. At the end of the day, the whole concept of auto-cast could be described as
  being not really necessary to do any sort of custom type processing. Therefore, auto-cast is
  being offered as an optional way of reducing the number of necessarily provided callback
  functions, that has two disadvantages: First, the auto-cast has to be implemented as a compiler
  plug-in functionality and second auto-casts increase the length of the evaluation program and
  hence constitute a penalty on an expression's evaluation performance.
  <p>

- The variety of built-in types has been reduced to a bare minimum needed.
  While module \alib_boxing (by default!) already drops the distinction of C++ integral type of
  different size (\c short, \c long, \c int, etc.), module \alib_expressions_nl in addition
  drops the distinction between \e signed and \e unsigned integral types. All integral types are
  \e signed. (Given that the "complete" JAVA programming language dropped unsigned integers, we
  thought it might not be too problematic).

  The good news for users of this library is that it is no problem to implement support for
  unsigned types, because "dropping" here just means, that just none of the built-in operators and
  functions "produces" a result value of unsigned integral type. In other words, unsigned integral
  types are considered just another custom type.

  If - unexpectedly - unsigned integer types and corresponding operations need to be supported,
  custom operators and function definitions have to be added.
  <p>

- For technical reasons, \alib_boxing_nl is not "aware" of type inheritance relations.
  It is not possible to detect if a type of one box inherits a different type in another box.
  Usually it is sufficient to expose only the base type to \alib_expressions_nl.
  It is important to note, that in this case callback functions have to perform a
  \c dynamic_cast before returning derived types. If they do not do that, the derived type
  becomes introduced to \alib_expressions_nl!<br>
  Finally, if the common base type is abstract, and therefore no sample value can be
  created, the trick to create a sample box is to use <c>reinterpret_cast<>()</c> like this:

       Box myTypeSample   = reinterpret_cast<MyAbstractType*>(0);

  <p>

- To make custom types compatible in full with all features of the library, it might be needed
  to do some side-implementations along the lines of underlying \alib features. For example,
  to allow nicely formatted string output of custom data using built-in expression function
  \alib{expressions;plugins::Strings;Format(formatString,...)}, box-function
  \alib{boxing;FAppend} and/or \alib{format;FFormat} have to be implemented for the custom type.
  <p>


\I{################################################################################################}
## 11.2 Literals ## {#alib_expressions_details_literals}
While it was in some places of this manual indicated that the built-in types listed with
\alib{expressions;Types} are all "inherently introduced" by the built-in compiler plug-ins just
as any custom type could be, this is not the full truth. In fact, types
\alib{expressions::Types;Integer},
\alib{expressions::Types;Float} and
\alib{expressions::Types;String} are in so far "hard-coded" as values of these types are also
created (and thus introduced) by expression "literals".

With the current version of the library it is not possible to change the internal "boxed" types
which result from "parsing" a literal. The term "parsing" denotes the first phase of the
compilation of an expression string. Changes on how literals are parsed and in what types such
parsing results can only be made by touching the library code, which is not further documented
here.

\I{#####################################################################################  }
### 11.2.1 Numerical Literals ### {#alib_expressions_details_literals_1}
The parsing of numerical constants found in expression strings is done with the help of
member \alib{format;Formatter::DefaultNumberFormat} which in turn is found in
member \alib{expressions;Compiler::CfgFormatter}.

The use of this helper-type, allows influencing how numerical literals are parsed. For example,
integral types can be parsed in decimal, hexadecimal, octal and binary formats. For this,
a set of prefix symbols, which default to <c>"0x"</c>, <c>"0o"</c> and <c>"0b"</c>, can
be customized. The support for one or some of the formats can also be dropped, if this is
wanted for whatever reason.

Likewise, the format of floating point numbers and its scientific variants can be defined.
In respect to the topic of localization (see also
\ref alib_expressions_details_localization "11.6 Localization"), this is especially of interest
if a different floating-point separation character than <c>'.'</c> is to be supported.
It is supported and tested in the unit tests of this library to allow the use of character
<c>','</c> as it is standard with many countries. In this case, an end-user has to be only aware
of the fact that the two expressions:

           MyFunc(1,2)
           MyFunc(1 , 2)

have a different meaning: The first is a call to an unary function providing floating point argument
\c 1.2, the second is a call to a binary function providing integral values \c 1 and \c 2.

Even worse:

           MyFunc(1,2,3)

is parsed as two arguments, the first being float value \c 1.2 and the second integral value
\c 3. This means, the end-user has to insert spaces to separate function parameters.

As this is a source of ambiguity, applications that address end-users with a high degree of
professionalism, should rather \b not localize number formats but instead document with their
software that english standards are to be used.

In general, all flags and options in respect to parsing and formatting (normalizing) number
literals that are available through class \alib{strings;TNumberFormat;NumberFormat} are compatible with
\alib_expressions_nl. This includes even to set character <c>' '</c> (space) as a grouping
character for any number format! This might be used to allow quite nicely readable numbers in
expression strings.

Finally, normalization flags
\alib{expressions::Normalization;KeepScientificFormat},
\alib{expressions::Normalization;ForceHexadecimal},
\alib{expressions::Normalization;ForceOctal} and
\alib{expressions::Normalization;ForceBinary} may be used to further tweak how numbers are
converted in normalized strings.

\I{#####################################################################################  }
### 11.2.2 String Literals ### {#alib_expressions_details_literals_2}
String literals are to be enclosed in quote characters <c>'"'</c>. If a string literal should
contain, the quote character itself, this needs to be "escaped" using backslash character
<c>'\\'</c>. Some further escape characters are supported, by the internal use of \alib string
feature documented with \alib{strings;TEscape;Escape}.

For the output of string literals in the normalized version of expression string, the
reverse functions of \b Escape are used.

\I{#####################################################################################  }
### 11.2.3 Box-Function FToLiteral ### {#alib_expressions_details_literals_3}
Ultimately, box-function \alib{expressions;FToLiteral} might be implemented for one of the
types (\alib{expressions::Types;Integer}, \alib{expressions::Types;Float} or
\alib{expressions::Types;String}) to do any imaginable custom conversions, other than possible
with the standards provided by the mechanics of the \alib types used.
But this should be seldom needed. The main purpose of this boxing-function is described
with \ref alib_expressions_details_optimizations "11.5 Optimizations".


\I{################################################################################################}
## 11.3 Identifiers/Functions ## {#alib_expressions_details_identifiers}

Identifier (parameterless functions) and function names are recognized (parsed) in expression
strings at appropriate places only if the following rules apply:
- The name starts with an letter, <c>'a'</c> to <c>'z'</c> or <c>'A'</c> to <c>'Z'</c>.
- The rest of the name consists of letters, underscore character <c>'_'</c> and numbers
  <c>'0'</c> to <c>'9'</c>.

In the current version of module \alib_expressions_nl, this is hard-coded and not
configurable.

\I{################################################################################################}
## 11.4 Localization ## {#alib_expressions_details_localization}

In the previous section, information about localizing number formats in respect to parsing
expression strings and their output as a normalized expression, was already given.

A second area, where localization may become an obvious requirement is the naming of built-in and
custom expression functions.
The built-in compiler plug-in use mechanics provided by \alib classes \alib{resources;ResourcePool}
and \alib{camp;Camp} to externalize the names, letter case sensitivity and optional
minimum abbreviation length of identifiers and functions. The matching of identifier and
function names found in expression strings is performed using class
 \alib{strings::util;Token}, which allows not only simple abbreviations, but also
"CamelCase" and "snake_case" specific abbreviations.

These mechanics allow to replace such resources using an arbitrary custom
"string/data resource backend": The one that your application uses!
With this, it is possible, for example, to translate certain identifiers (e.g., \b Minutes or \b True)
to different locales and languages.

While there is no detailed documentation or step-by-step sample on how to perform such
localization in detail is given, investigating the documentation and optionally the simple
source code of the entities named above, should enable a user of this library to quite quickly
succeed in integrating any custom localization mechanics used otherwise with her software.
For creating a custom plug-in, the way to go is of course to copy the setup code
from the built-in plug-ins of this library.

A third area where localization might become a need are callback functions
processing expression data. Again, for formatting and parsing, an instance of \alib class
\alib{format;Formatter}, which has (as was explained above) an instance of
\alib{strings;TNumberFormat;NumberFormat} attached.<br>
A compile-time scope (used with optimizations) is created with virtual method
\alib{expressions::Compiler;createCompileTimeScope} which in its default implementation attaches
the same formatter to the compile-time scope that is used with parsing, namely the one found in
\alib{expressions::Compiler;CfgFormatter}.<br>
The scope object used for evaluation should be constructed passing again the very same formatter.
This way, formatting and number formats remain the same throughout the whole chain of processing
an exception and can collectively tweaked through this one instance \b %CfgFormatter.

Finally a fourth area where localization might be applied is when it comes to exceptions during
compilation or evaluation of expressions. All exceptions used in this library provide human
readable information, which is built from resourced strings and hence can be localized.
See chapter \ref alib_expressions_details_exceptions "11.6 Exceptions" for details.

\I{################################################################################################}
## 11.5 Optimizations ## {#alib_expressions_details_optimizations}

\I{#####################################################################################  }
### 11.5.1 Goals ### {#alib_expressions_details_optimizations_1}
One very important design goal of this library was to favor evaluation-time performance of
expressions over compile-time performance. This way, library is optimized for use cases where a single
expression that is compiled only once, is evaluated against many different scopes. The higher
the ratio of number of evaluations per expression term is, the more increases the overall process
performance when this design principle is applied. This design goal caused a great deal
of effort and its implications were far-reaching.

The concept of "expression optimization", that was touched in this manual various times already,
is all about optimizing the evaluation-time performance. The library volunteers to have some efforts
at compile-time to shorten the compiled expression program that is run on the built-in virtual
machine as much as possible.

A simple sample for optimization might be an expression that calculates the circumference
for a given radius. In case the radius is received from the scope with a custom expression
identifier \b %Radius, then the expression would be:

           2 * PI * Radius

If no optimization was applied, each time this expression was evaluated, four callback functions
had to be invoked: Two for receiving the values \b %PI and \b %Radius and two for the
multiplications. Now, we know that \b %PI is constant and so is the term <b>"2 * PI"</b>.
The goal of optimization consequently is to reduce the expression program to just do two callback
invocations: one for retrieving the radius from the scope and a second for multiplying the radius
with the constant <b>2 * PI</b>.

To express this goal the other way round: An end-user should be allowed to provide
operations that introduce some redundancy, but are easier readable and understandable for
human beings, without impacting evaluation performance.

\I{#####################################################################################  }
### 11.5.2 Optimization Mechanics ### {#alib_expressions_details_optimizations_2}
The foundation of compile-time optimization of this library is implemented with the assembly
of the expression program: During the assembly, the compiler keeps track of previous results being
constant or not. Each time a compiler plug-in is asked to provide compilation information, this
information about whether the arguments are all or partly constant is provided. Then it is up
to the plug-ins to decide whether the expression term is a constant in turn or implies a
callback function call.

In the simple case of identifiers (parameterless functions), no arguments exist and hence
all arguments are constant. Nevertheless, custom identifiers are usually not constant, because
they return data received from the (custom) scope object. Therefore, the compiler does not
"know" if identifier \b "PI" is a constant, only the plug-in that compiles the identifier knows
that. While in the case of \b PI it is, in the custom case of \b %FileDate it is not: It depends
on the currently examined scope data.

This way, the compiler and its plug-ins have to work hand in hand: The compiler provides information
about arguments being constant and the plug-ins can return either a callback function or
leave the callback function \c nullptr and return a constant value instead. The compiler then
either assembles a callback function or the use of the constant value, which is an internal
program command of the virtual machine's "assembly language".

With binary operators, a further option is available: In the case that one operator is
constant, while the other is not, some operators might inform the compiler to either optimize out the
complete term or at least to optimize out the constant argument. Again, this information has to be
encoded in the result data provided by the compiler plug-ins. The compiler will then
modify the existing program and remove the program code for one or both arguments.
(Further samples of binary operator optimizations are given in documentation of struct
\alib{expressions;CompilerPlugin::CIBinaryOp}.)


\I{#####################################################################################  }
### 11.5.3 Optimizations Of The Built-In Compiler Plug-ins ### {#alib_expressions_details_optimizations_3}
As explained earlier, the built-in compiler plug-ins mostly rely on helper-struct
\alib{expressions;plugins::Calculus} instead of deriving directly from
\alib{expressions;CompilerPlugin}. \b %Calculus provides very convenient ways to
ensure that every operation that can be optimized at compile-time truly is optimized.

For example, callback functions can be denoted "compile-time invokable". If so, helper-struct
\b %Calculus automatically invokes them at compile-time if all arguments provided are constant
(or no arguments are given) and returns the calculated result to the compiler instead of the
callback function itself.

Furthermore, struct \b %Calculus provides a special sort of optimization applied to binary
operators that may be applied when only one of the two arguments is constant. For example,
an arithmetic multiplication with constant \c 0 results in \c 0 and with constant \c 1 it
results to the other argument's value. These kind of rules can be encoded using quite simple static
data tables.

Overall, the use of struct \b %Calculus makes the implementation of optimization features of custom
plug-ins as easy as possible.
Consult the struct's \alib{expressions;plugins::Calculus;documentation} for further details.


\I{#####################################################################################  }
### 11.5.4 Compile- And Evaluation-Time Optimization Of The Conditional Operator ### {#alib_expressions_details_optimizations_conditional}
In the current version of \alib_expressions_nl there is only one evaluation-time
optimization performed. This considers built-in ternary operator <c>Q ? T : F</c>
(conditional operator).

Likewise a C/C++ compiler, depending on the evaluation-time value of \c Q only the
program code to calculate \c T or \c F is executed by the virtual machine.

\note
   This is important to be known by end-users: The "side effect" of this optimization is that it does
   \b not produce "side effects" that the left-out term might be expected to produce in the case that
   this optimization was not performed and thus custom callbacks had been invoked for both terms
   \c T and \c F in parallel.

The optimization of the conditional operator is as well performed at compile-time: In the case
that \c Q is a compile-time constant, the code for either \c T or \c F is not even included
in the program.


\I{#####################################################################################  }
### 11.5.5 Limits Of Optimization ### {#alib_expressions_details_optimizations_5}
The current version of this library has an important limit in respect to optimizations.
While - as we saw - expression:

            2 * PI * Radius

is optimized to perform only two callbacks instead of four, the mathematically equivalent
expression:

            2 * Radius * PI

is only optimized by one callback, hence still includes three.

The reason is that there is no built-in mechanics to tell the compiler that
for the two multiplications, the associative and commutative laws apply, which would allow
to transform the latter expression back to the first one.

Instead, the compiler "sees" two multiplications that both are not performed on constant operands and
hence cannot be optimized. Only the callback of constant identifier \b %PI is removed.

Consequently, if performance is key, it might be documented to an end-user that she is good to write:

       HoursPassed * 60 * 60 * 1000

because this expression is optimized to:

       HoursPassed * 3600000

but that she should "sort" constants to constants, because expression

       60 * HoursPassed * 60 * 1000

is less effectively optimized to

       60 * HoursPassed * 60000


In a more abstract way, it could be stated that a C++ compiler does such optimization, because
it "knows" about the rules of the multiply operator of integral values.
The compiler of this library does not know about that and hence cannot perform these kind
of operations. If in the case of C++, the operands were custom types with overloaded operator
<c>'*'</c>, the C++ code would also not be optimized, because in this case, the compiler
does not know the mathematical "traits" of the operator.
The C++ language has no syntax to express operator traits.

From the "point of view" of the expression compiler provided with this library, the
built-in operators are just "built-in custom operators". This leads to the inability of optimizing
such rather simple mathematics.

Finally, evaluation-time optimization of operator \b && (as known from the C++ language) is not implemented
with this library. For example, with expression:

       IsDirectory && ( name == "..")

the right-hand side operand of \b && is evaluated even if \b %IsDirectory returned \c false already.

\I{#####################################################################################  }
### 11.5.6 Normalized, Optimized Expression Strings ### {#alib_expressions_details_optimizations_norm}

In the case that professional, experienced end-users are the addressees of software, it might
be wanted to tell such end-users about the result of optimizations. To stay with the sample
of the previous sections, this means to be able to show an end-user that the expression:

       2 * PI * Radius

was optimized to

       6.283185307179586 * Radius.

To be able to do this, a normalized expression string of the optimized expression has to be
generated. This way, the interface of class expressions allows access to three strings with
methods
- \alib{expressions;ExpressionVal::GetOriginalString}
- \alib{expressions;ExpressionVal::GetNormalizedString} and
- \alib{expressions;ExpressionVal::GetOptimizedString}

The generation of the normalized string during compilation cannot be disabled and hence is
available in constant (zero) time after the compilation of an expression.
However, the first invocation of method \b %GetOptimizedString is all but a constant task!
With this, the library <em>"decompiles"</em> the optimized expression program with the result being an
abstract syntax tree (AST).
This AST is then compiled into an new program and with this compilation a "normalized" expression
string is generated.

Consequently, this normalized string is the optimized version of the original expression string!
Once done, the AST and the compiled (second) program are disposed, while the optimized string
is stored.

It is questionable if the result is worth the effort! The decision if software using
library \alib_expressions_nl presents "optimized normalized expression strings" to the
end-user is highly use-case dependent. In case of doubt our recommendation is to \e not do it.
The feature may be helpful during development of custom compiler plug-ins.

In any case, to receive correct, compilable, optimized expression strings, a last hurdle might has to be
taken. In the sample above, the optimized term <c>2 * PI</c> results in floating point value
<c>6.283185307179586</c>. This value can easily be written out and - if wanted - later be
even parsed back to a correct expression. But this is only the case, because
the type \alib{expressions::Types;Float} is expressible as a literal. Imagine the following
sample:

       Seconds(1) * 60

Built-in identifier \b %Seconds returns an object of type \alib{expressions::Types;Duration}.
The multiplication operator is overloaded an in turn results in a value of type \b %Duration.
And yes, it is a constant value. The challenge now is to produce an expression string that
creates a constant time span value representing \c 60 seconds. The result needs to be

       Seconds(60)

or even better:

       Minutes(1)

To achieve this, this library introduces \alib a \ref alib_boxing_functions "box-function" which
is declared with \alib{expressions;FToLiteral}.
This function has to be implemented for all custom boxed types that might occur as results of
constant expression terms. Only if this is ensured, the "optimized normalized expression string"
is correct, meaningful and re-compilable.

For details and a source code sample consult the documentation of the box-function descriptor class
\alib{expressions;FToLiteral}.

Besides this box-function to create constant expression terms for custom types, a next
prerequisite might have to be met to receive compilable expression strings. This is in the
area of auto-cast functionality. If custom auto-casts are in place, such auto-casts,
if decompiled, have to be replaced by a function call which takes the original value
and returns the casted value. The names of the function has to be provided with members
\doxlinkproblem{structalib_1_1expressions_1_1CompilerPlugin_1_1CIAutoCast.html;a1b78ad7a906e7c8e0f3d5b471ecb8fbf;CIAutoCast::ReverseCastFunctionNameRhs} and
\doxlinkproblem{structalib_1_1expressions_1_1CompilerPlugin_1_1CIAutoCast.html;ae064a880bf0198fb372674e22ffe3221;CIAutoCast::ReverseCastFunctionNameRhs} of the
auto-cast information struct at the moment an auto-cast is compiled.
If optimized, normalized expression strings are not used, these fields are not necessary to be set
and much more, the corresponding expression functions that create the constant values
may not be needed (they might still be needed for the expression syntax a programmer wants to offer).

\note
  All built-in identifiers, functions, operators and auto-casts are fully compatible with optimized
  expression strings.
  Most unit tests provided with this \alibmod perform the "full circle" of compilation and
  evaluation, which is:
  - compile the given expression string and check the evaluation result.
  - compile the normalized version of the expression string and check the evaluation result.
  - create the optimized version of the expression string, recompile it and check the evaluation
    result.
  - switch off optimization (see next section), compile the original expression string and check
    the evaluation result.


\I{#####################################################################################  }
### 11.5.7 Disabling Optimization ### {#alib_expressions_details_optimizations_7}

While there is no reason to switch off optimization, the library offers compilation flag
\alib{expressions;Compilation::NoOptimization} for completeness.


\I{################################################################################################}
## 11.6 Shared Resources And Multithreaded Use # {#alib_expressions_details_multithreading}

This library does not make use of \e semaphores (aka "thread locks") to protect resources
against violating concurrent access.
Consequently, it is up to the user of the library to ensure some rules and potentially
implement semaphores if needed.
This goes well along with the design principles of most foundational, use-case agnostic libraries.

Therefore, this chapter lists the rules of allowed and denied parallel actions:

- Any use of the same expression compiler instance in multithreaded software has to be locked
  by the using software. This includes changes of compiler properties, addition and removal of
  plug-ins, addition and removal of named expressions and the compilation of expressions.
  <p>

- Instances of compiler plug-ins may be shared between different compiler instances.
  Parallel compilation using compilers that use shared instances of plug-ins is allowed.
  This is true for all built-in plug-ins.<br>
  It depends on the implementation of custom plug-ins, whether their parallel use by different
  compiler instances is allowed.
  <p>

- During evaluation of expressions originating from a certain compiler instance,
  no named expressions must be added or removed from that compiler.
  <p>

- During evaluation of expressions originating from a certain compiler instance,
  no further compiler-actions must be invoked.
  For example, it is forbidden to invoke \alib{expressions::ExpressionVal;GetOptimizedString}.
  <p>

- The same expression instance or a set of different expressions originating from the same compiler
  are allowed to be evaluated in parallel if the following conditions are met:
  1. Each thread must own and use a dedicated \b Scope instance that is passed to method
     \alib{expressions::ExpressionVal;Evaluate}.
  2. All callback functions are re-entrant, hence allow parallel invocation.<br>
     Note that the built-in callbacks are.
  3. What was said above about evaluation and compiler use.
                     

\I{################################################################################################}
## 11.7 Exceptions ## {#alib_expressions_details_exceptions}

\I{################################################################################################}
### 11.7.1 Catching Exceptions ### {#alib_expressions_details_exceptions_1}
At the moment end-users are allowed to provide expression strings, some error handling
to deal with malformed expression strings is unavoidable.
Module \alib_expressions_nl chooses to implement error handling using C++ exceptions.

One of the design goals of this library is to allow to recognize erroneous expressions at
compile-time if possible. The advantage of this is that compilation is often performed at a point
in time where the consequences of exceptions are usually less "harmful". Of course, software cannot
continue its tasks if exceptions occur, but the implicated effort of performing a "rollback"
should be much lower.

For this, the following general approach should be taken:
   1. Compile expressions
   2. On exceptions inform the end user (e.g., stop program)
   3. Allocate resources (scope) needed.
   4. Evaluate expressions
   5. On exceptions rollback resource allocations and inform the end user (e.g., stop program)

As evaluation-time exceptions anyhow can occur, in simple cases step 2 might be left and
step 1-4 be wrapped in one \c try statement.

The exception object thrown by any \alibmod is of type \ref alib::exceptions::Exception.
\note
  This class combines the advantages of two paradigms frequently discussed as alternative
  approaches to exception handling.
  For more information and to fully leverage its use with this \alibmod, please consult the
  class's \alib{exceptions;Exception;documentation}.


\I{################################################################################################}
### 11.7.2 Exceptions In Compiler Plug-Ins ### {#alib_expressions_details_exceptions_2}
A compiler plug-in may throw an exception during compilation. Helper-struct
\alib{expressions::plugins;Calculus} already throws exception
\alib{expressions::Exceptions;MissingFunctionParentheses} and
\alib{expressions::Exceptions;IdentifierWithFunctionParentheses}.
Furthermore, a callback function may throw an exception during the compile-time evaluation of a
constant expression term.

Exceptions of type \c std::exception as well as those of type \b %alib::Exception that are not
exposed by this \alibmod itself (hence using values of enum types different than
\ref alib::expressions::Exceptions "expressions::Exceptions"),
by default are "wrapped" by the compiler into an exception of enum type
\alib{expressions;Exceptions::ExceptionInPlugin}. Such wrapping can be disabled by setting flag
\alib{expressions;Compilation::PluginExceptionFallThrough}.

In addition, plug-in exceptions of type \b %alib::Exception are extended by an informational entry
of type \alib{expressions::Exceptions;ExpressionInfo}.

Exception objects of other types are never caught and wrapped and therefore have to be caught
in a custom way.<br>


\I{################################################################################################}
### 11.7.3 Evaluation-Time Exceptions In Callback Functions ### {#alib_expressions_details_exceptions_3}
In the case that a callback function throws an exception during the evaluation of an expression,
such exceptions by default are "wrapped" into \alib{expressions::Exceptions;ExceptionInCallback}.
Wrapping is performed with exceptions of type \c std::exception and \alib \b %Exception.
Other exception types are never caught and wrapped and therefore have to be caught
in a custom way.

The wrapping of evaluation-time exceptions can be disabled by setting flag
\alib{expressions;Compilation::CallbackExceptionFallThrough}. Note, that even while this flag
is tested at evaluation-time, it is still stored in member \alib{expressions;Compiler::CfgCompilation}.


\I{################################################################################################}
\I{################################################################################################}
# A. Appendix # {#alib_expressions_appendix}

\I{################################################################################################}
## A.1 Built-In Identifier, Function And Operator Reference {#alib_expressions_appendix_builtin}

The built-in expression functionality is provided via the built-in compiler plug-ins which
\alib{expressions;Compiler::CfgBuiltInPlugins;by default} are enabled and used.

Reference tables about identifiers, functions and operators is provided with the
each plug-in's class documentation. Those are:

- \alib{expressions::plugins;Arithmetics}
- \alib{expressions::plugins;Math}
- \alib{expressions::plugins;Strings}
- \alib{expressions::plugins;DateAndTime}

\note Sibling \alib module \alib_files, provides a compiler plug-in dedicated to file and
      directory trees.



\I{################################################################################################}
## A.2 Expression Based String Formatting {#alib_expressions_appendix_formatting}

The goal of using a library like this is to allow end-users to write expressions. One common
field of applications are filter expression, as sampled in this manual.

Another common requirement is to allow users to define output formats. To - once more - stay with
the file-system sample of this manual, software may want to allow a user to specify
how a line of output for a directory entry should look like.

With built-in plug-in \alib{expressions::plugins;Strings}, expressions that return a string
can be created quite easily. For example:

       String( Name ) + "  " + Size/1024 + "kB"

could be such an output expression.

However, there is a more comfortable and powerful way to do this! The key to that is the use
of format strings as processed by \ref #alib::format "ALib Formatters" in a
combination with expression strings that comprise the placeholder values found in the format
strings.

Utility class \alib{expressions::util;ExpressionFormatter} implements such combination.

For details and usage information, please consult the class's
\alib{expressions::util;ExpressionFormatter;documentation}.


\I{################################################################################################}
## A.3 The Built-In Virtual Machine ## {#alib_expressions_appendix_vm}
Talking about a virtual machine, most people today would consider the JAVA Virtual Machine
as good sample. While this is true and comparable, the machine that is included in this library
is a million times simpler. In fact, the current implementation that executes an expression
program consists of less than \c 300 lines of code:
A very simple "stack machine" that has only just <b>five commands</b>!

For people who are interested in how the machine works, besides investigating into its source
code a look at some sample programs for it, leads to a quick understanding.

With debug-builds of this library, static method
\alib{expressions::detail;VirtualMachine::DbgList} may be invoked to generate a listing of an
expression's program. Because the originating expression string itself is given with these listings,
in this chapter, we just sample the listing output, without sampling the expressions explicitly.

Let's have an easy start with a simple expression of a constant value:
  \verbinclude "DOX_EXPR_VM_-1.txt"

This shows the first command \c "Constant", which pushes a constant value that is a parameter of
the command to the stack.

Lets do some multiplication:
  \verbinclude "DOX_EXPR_VM_-2.txt"

Ooops, it is still one command, which includes the result. The reason for this is the optimizing compiler
that detected two constants, passed this information to the compiler plug-in and this in turn
did the calculation at compile-time. Consequently, we have still a constant expression program.<br>
We now have two options: Use non-constant functions like built-in math function \e Random, or
just \alib{expressions;Compilation::NoOptimization;switch off optimization}. The latter is what
we do:
  \verbinclude "DOX_EXPR_VM_-3.txt"

We now see two pushes of constant values and then virtual machine command \c "Function", which
invokes a C++ callback function as provided by the compiler plug-ins.
In this case it is a callback named "mul_II", which implements operator <c>'*'</c> for
two integer arguments. Those arguments will be taken from the current execution stack.
The result of the callback will be pushed to the stack.

\note
  A correct expression program leaves one value at the stack when finished. This value is the
  result value of the expression.

Command \c "Function" is used for expression terms of type unary operator, binary operator,
identifier and function.

In column "Description" the listing tells us that the callback "mul_II" in the third and final
program command was compiled by plug-in "ALib Arithmetics" with operator <c>'*'</c>. Such
information is debug-information and not available in release compilations of the library.

We now know two out of five virtual machine commands and already quite complex expressions can
be compiled:
  \verbinclude "DOX_EXPR_VM_-4.txt"

Note, that listing column "ArgNo" denotes for each argument the program code lines which are responsible for
calculating it on the stack. In other words: each segment of code <b>{x..y}</b> noted in this column
produces exactly one result value on the stack, just as the whole expression produces one.

The following sample uses a function that consumes three arguments:
  \verbinclude "DOX_EXPR_VM_-5.txt"

Now, as two VM-commands are understood, lets continue with two further ones.
For implementing the ternary conditional operator <c>Q ? T : B</c>, two types of jump commands are
needed, a conditional jump and an unconditioned one:
  \verbinclude "DOX_EXPR_VM_-6.txt"

Note that while the program listing for convenience presents the destination address using the
absolute program counter (first column "PC") number, internally relative addressing is used.
The insertion of the two jump commands explains what is said in
\ref alib_expressions_details_optimizations_conditional "11.5.4 Compile- And Evaluation-Time Optimization Of The Conditional Operator".<br>
Just for fun, we enable compile-time optimization and check the output:
  \verbinclude "DOX_EXPR_VM_-6opt.txt"


The fifth and final command \c "Subroutine" is needed to allow
\ref alib_expressions_nested "Nested Expressions". We add an expression named "nested" and
refer to it:
  \verbinclude "DOX_EXPR_VM_-7.txt"

Using the alternative version that locates nested expressions at evaluation-time only, the
program looks like this:
  \verbinclude "DOX_EXPR_VM_-8.txt"

With these few simple samples, all five commands of class \alib{expressions::detail;VirtualMachine}
are covered.

\I{################################################################################################}
## A.4 Notes On The Architecture Of The Library ## {#alib_expressions_appendix_architecture}

This quick chapter does not need to be read. We just felt to write it for those people
who want to take the source code and understand how module \alib_expressions_nl was implemented,
and maybe want to extend it or add internal features.

Often, there are two different perspectives needed when you think about the architecture of a
software library. The first is from the viewpoint of the user of the library. This may be called the
"API perspective". It basically asks: What types do I need to create and which methods do I need to invoke?
The second is from the implementer's perspective. Here, it is more about what types implement
which functionality and how do they interact internally.<br>
With the development of this \alibmod, these two perspectives had been in a constant
internal fight. The decision was taken to follow the needs of the API perspective.

A user of the library just needs to "see":
- Type \alib{expressions;Compiler}, which she extends with custom derivates of
- type \alib{expressions;CompilerPlugin}. Together, these create objects of
- type \alib{expressions;Expression} and its underlying \alib{expressions;ExpressionVal},
  which, under provision of an object of
- type \alib{expressions;Scope} become evaluated.
That's roughly it. Very simple.

From an implementation perspective there are some more things:
- %Expression strings need to be parsed into an abstract syntax tree (AST),
- ASTs need to be compiled into a program,
- Programs need to be executed by a virtual machine,
- Optimized programs need to be decompiled back into ASTs to create normalized, optimized expression
  strings.

To keep the types that are needed from the API-perspective clean and lean, responsibilities had
been moved into maybe "unnatural" places. Some more quick bullets and we have said
what this chapter aimed to say:
- The types necessary for the user may be abstract and show only a minimum set of interface
  methods. Corresponding implementations have been shifted to inner namespace \c detail.
  The differentiation between the abstract base and the implementation is a pure design decision.
  It even costs some nanoseconds of run-time overhead, by invoking virtual functions, where no such
  abstract concept is technically needed. (While it reduces compile time for a user's software)
- To keep class \alib{expressions;Compiler} clean, it just contains configuration options and
  holds the plug-ins, while
- the compilation itself is implemented in class \alib{expressions;detail::Program}.
  Maybe a class named "Program" should not compile and assemble itself. Well, but it does.
  If it didn't, the class would probably not exist: It would be just a \c std::vector of virtual
  machine commands residing in the expression. Therefore, it just was a nice empty thing that we put
  the assembly stuff in, while keeping class %Compiler free of that.
- Well, and we admit: to keep the program concentrating on just assembly, the virtual machine has,
  besides its duty to run programs, two other responsibilities: The first can be almost considered
  "OK": In debug-compilations of the library, it creates program listings. But then:
- The virtual machine decompiles programs back to ASTs!

This design and structure might be questionable. Probably, a virtual machine should not perform
de-compilation and should not "know" about ASTs, which otherwise constitute the intermediate data
layer between a parser and a compiler.
Please do not blame us. We do not foresee bigger feature updates of this \alibmod_nl. If such were
needed, this current code design may fail and need some refactoring. But as we did
it, its a compromise strongly towards simplicity of the API as well of internal code.

<br><br><br><br><br><br> */
