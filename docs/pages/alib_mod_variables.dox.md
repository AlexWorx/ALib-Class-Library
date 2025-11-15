// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_variables   ALib Module Variables - Programmer's Manual


\tableofcontents

\I{################################################################################################}
# 1. Introduction # {#alib_variables_intro}

\I{------------------------------------------------------------------------------------------------}
## 1.1 Hello World Sample ## {#alib_variables_intro_hello}
Let us start this manual with the simplest hello world sample demonstrating the use of an
\alib variable:

A simple application needs to include just header \implude{Variables}
\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_HEADER

\note OK, a second include is needed with \implude{Lang.CIFunctions}, because
      the code following is a global function, namely <c>main()</c>. Background information on this
      is given with chapter \ref alib_manual_appendix_callerinfo of the General ALib Manual.

The main function then looks as follows:

\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_HELLO_WORLD

and the output is of course:

\verbinclude "DOX_VARIABLES_HELLO_WORLD.txt"

The takeaways from this quick sample is the following:
- \alib provides an implementation of the concept of "run-time variables".
- Variables have a name, which is of string-type.
- Variables are associated with \alib camps, in this case the singleton #alib::BASECAMP.<br>
  Well this is not really true: In fact variables are associated with an instance of class
  \alib{variables;Configuration}. Now, because every \alibcamp provides a singleton that disposes over
  an own or a shared instance, in this sample we use an overload of the constructor of class
  \alib{variables;Variable} that accepts a \b Camp instance instead of a \b Configuration
- Variables have a type, which is specified as a string. Here, string <c>"S"</c> specifies
  that the variable is of string type. Other built-in types exist and users of the library
  can register their own (complex) types to be stored in variables.
- Variables have to be defined. Strangely definitions might "fail", so the code assigns the
  value only if the definition succeeds. More strangely, the code above accesses the
  variable, even if method "Define" failed.
- There is a (convenience) method \alib{variables;Variable::GetString} that receives a string value
  from a variable.

OK, so far so good. But you might wonder: What is this good for? C++ already has variables in
its language. Why should now run-time variables be used?

The answer becomes obvious when you compile and run this sample program passing the following
argument:

    --MY_VAR=Joe

Then the output will be:

    Hello Joe

Obviously, the command line parameter was recognized as an external definition of the variable.
And: This external definition is "stronger" than the "hard-coded" definition.<br>
Does this make sense? Of course it does: Hard-coded values placed in the source code of an
application are considered default values. If the user specifies some other value explicitly,
quite naturally this value is the one to be used!

With this litte introduction sample, a reader might already guess what the whole use and effort
of providing module \alib_variables is about. The rest of this manual explains the details.

\I{------------------------------------------------------------------------------------------------}
## 1.2 Feature List ## {#alib_variables_intro_features}

This module provides tools to read and write configuration data using different mechanisms.
In short, the features are:
- Central class \alib{variables;Configuration} used to store and retrieve internal and external
  configuration data using arbitrary sources.
- Abstract plug-in interface \alib{variables;ConfigurationPlugin} to read from custom external
  configuration storage.
- Predefined plug-ins for
  - command line parameters,
  - environment variables, and
  - simple INI-files.
- The possibility to set default values 'in code' (if no external plug-in finds a value)
  and doing this "out of scope" which means independent of the code that finally uses a
  variable's value.
- The possibility to protect variables 'in code' from being modified. (For example, to block
  modification of variables exposed by 3rd party code, in case such modification is not appropriate
  or even would impose a security risk.)
- Possible provision of variable declarations, including
  - the variable name,
  - the data type,
  - an optional default value, and
  - an optional end-user description text

  as \ref alib_enums_records "resourced C++ enumeration values".<br>
  With that in place, the source code exclusively refers to variables using an C++ enumeration element.
  This allows duly separating all external configuration data from the source code and for example
  to translateall variable names and description to a different locale.
- Automatic \ref alib_variables_values_substitution "variable substitution", which resolves the values of
  variables by referring to other variables.
- With the combination of variable substitution and protection of variable values, it is possible
  to hide, rename or even redefine the syntax and behavior of variables exposed by third party code
  (e.g., libraries).
- Creation, value change and deletion of either single variable, a tree of variables or a set of
  variables, can be monitored by registering callback entities.


As a result from this feature list, it may become obvious that this
\alibcamp_nl can used in \ref alib_variables_usecases "other use-cases" - apart from "just" configuration data.

\I{################################################################################################}
# 2. Variable Names # {#alib_variables_names}
\I{------------------------------------------------------------------------------------------------}
## 2.1 Building on Class StringTree ## {#alib_variables_names_string_tree}
Variables are stored in and managed by class \alib{variables;Configuration}. This type inherits
its core feature from class \b %StringTree provided by module \alib_containers.

If you are not familiar with class \b %StringTree, you might read its
\alib{containers;StringTree;reference documentation} now. In short, this type is a hash
table which organizes the entries in a linked tree structure (in addition to the key/value hashing).
The path through the tree is implemented by strings - just as the path of a file system is.
While in a file system, the nodes of the tree are files, in case of class \b %Configuration, the
nodes are variables.

Same as with filesystems, the \b StringTree defined with class \b %Configuration uses a slash <c>'/'</c>
as the path separator. The default interface into configuration variables treats all variable names
given with declarations as <b>absolute paths (!)</b> in the string tree. Hence to declare a variable
named

    MYAPP/MYVAR

is the same as declaring it

    /MYAPP/MYVAR

\note
    As just said, this is true for the "default interface". In a later chapter, it will be discussed
    what the existing other interface - which does not treat variable names as absolute paths - looks
    like and when it is to be used.

For simple use cases, the fact that variables are organized in a tree structure
<b>can just be ignored!</b> If things grow and become more complex, organizing variables along a
well thought hierarchy will bring some tremendous benefits.

\I{------------------------------------------------------------------------------------------------}
## 2.2 Case Sensitivity of Variable Names ## {#alib_variables_names_casesensitivity}
Variable names are case-sensitive. This has advantages and disadvantages. One advantage is
performance: When declaring variables (which is searching variables in the \b StringTree and
creating the path if not existent), no upper/lower case conversion has to be performed.

A problem arises if external configuration systems are case-insensitive. Then, two \alib variables
that only differ in letter case, are not externally distinguishable. This may already be true
for environment variables (depending on the operating system).

The most natural way out of this problem <b> - and this is what we recommend to do - </b> is to
just use capital letters and underscores <c>'_'</c> with \alib variables, as seen with the
sampled <c>"MY_VAR"</c> and as implemented with all \ref alib_cfgvars "built-in variables"
of \alib.

Built-in plug-ins \alib{variables;EnvironmentVariablesPlugin} and \alib{variables;CLIVariablesPlugin},
as well as class \alib{variables;IniFileFeeder} (all types are discussed in detail in later chapters)
operate in a case insensitive fashion.

\I{------------------------------------------------------------------------------------------------}
## 2.3 Translation of Variable Names ## {#alib_variables_names_external_translation}
As just learned, if variable names use the path separator, the variables become organized
hierarchically. For example, two \alibcamps are using variables today:
- Camp \ref alib_mod_camp "Camp": All variables are residing in path <c>"/ALIB/"</c>
- Camp \alib_alox: All variables are residing in path <c>"/ALOX/"</c>

Let us look at variable \ref alibcvALIB_LOCALE which is read by camp class
\alib{camp;Basecamp} during bootstrap and allows setting a different locale than defaulted
by the environment that an application is launched in. If an INI-file was used, naturally this
variable would reside in INI-file section "ALIB" and the entry would be named "LOCALE=...".
Here you see, that some sort of "translation" already has to take place.

When this variable is read from the command-line, it has to be addressed as:

    --ALIB_LOCALE=...
The path separator here is replaced from slash to an underscore <c>'_'</c> character.

The decision for such translations is not really a concept of this module. It is rather
defined by the configuration plug-ins (which are discussed later) and by user code that
imports or exports configuration data from custom sources as needed. It is just important to keep
this in mind, because using slashes is often not wanted for external representations of variables.

\I{------------------------------------------------------------------------------------------------}
## 2.4 Placeholders in Variable Names ## {#alib_variables_names_placeholders}
This \alibcamp_nl actively supports to use one declaration singleton to define a set of variables.
This feature is used in the case that a certain set of configuration data is applied to more
than one instance of a user type. As a sample, let us look at \alib_alox configuration variable
\ref alxcvALOX_LOGGERNAME_FORMAT. While the documentation here says "/LOGGERNAME/" the resourced
name of the declaration of the variable reads <c>"ALOX/%1/FORMAT"</c>.<br>
Classes \alib{variables;Variable} and \alib{variables;Declaration} both provide overloaded methods
that accept an additional parameter called \p{replacements}. With that, replacement strings for
placeholders <c>%1, %2, ...%N</c> can be passed. In the above sample, camp \alox passes the name
of a logger that a user of the camp can freely choose and where multiple instances may exist.
With that in place, each logger disposes of individual settings.

In addition to replacing placeholders in the variable name, those may also occur in a declaration's
default value and comment strings.

If placeholders are used, a new singleton of class declaration is allocated in the
\alib{MonoAllocator} of class \c Configuration. This means, that it is not allowed to
perform an indefinite number of replacement-declarations. In the above sample, this is of course
given: An application usually has two or three different \alox loggers in place, but does not
create and delete new loggers with different (run-time generated) names.

\note On this topic, please see also the reference documentation of method
      \alib{variables;Configuration::StoreDeclaration}, which is used by the overloaded methods which
      accept parameter \p{replacements}.

Parameter \p{replacements} of the various overloaded declaration methods is of type
\alib{boxing;Box}. As explained with the \ref alib_mod_boxing "Programmer's Manual of module Boxing",
a box may also contain arrays and even arrays of boxes. Because of the multiple overloads, it was
technically the easiest solution to offer the replacement values with such single parameter.
Internally, \ref alib_boxing_basics "type-guessing" is performed and the following types are
discovered:
- A single \b Box object,
- a C++ array af \b Box objects, and
- instances of \alib{boxing;TBoxes} (either version \alib{Boxes}, \alib{BoxesMA} and
  \alib{BoxesPA}).

The box or boxes found are converted to string values, by appending them to an internal
\alib{strings;TAString;AString} instance.

Given this information, it becomes obvious, that in the simple case of one replacement string,
such string can just be passed to parameter \p{replacements}. The following sample, taken from
module \alib_alox, shows how two replacements strings are passed. The resourced variable declaration
is:

\snippet "alox/aloxcamp.cpp"     DOX_VARIABLES_REPLACEMENTS1

The declaration of the individual variable for a certain \alib{lox::detail;Logger} attached to a certain
\alib{lox;Lox} is as simple as this:

\snippet "alox/detail/loxpimpl.cpp"     DOX_VARIABLES_REPLACEMENTS2

      

\I{################################################################################################}
# 3. Variable Declarations # {#alib_variables_declaration}
This camp differentiates between variable \e declarations and their \e definition. While these
terms were chosen along the lines of the C++ language, of course with this camp the effective
meaning of the terms are quite different.

This chapter is about declaring variables.

\I{------------------------------------------------------------------------------------------------}
## 3.1 Minimal Declaration with Name and Type ## {#alib_variables_declaration_minimum}
In the hello-world-sample above, three parameters had been given to the constructor of a variable:
\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_DECLARATION1
This constructor is a shortcut to the following two lines of code:
\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_DECLARATION2

This demonstrates the minimum information that is needed to declare a variable: The variable's
name and type. While we have talked about names already, variable types will be discussed later.
For now, it has to be enough to say that types are given as strings and that type <c>"S"</c> stands
for string variables.

With the declaration, the run-time memory allocation of the variable is in fact performed.
A subsequent repeated declaration has no effect, of course no matter if this is done in the same
code entity or in a different module or thread.

Internally the following things happen:
- The underlying \alib{containers;StringTree::TCursor;string tree cursor} is moved
  to position in the tree. Along the path of the variable, non-existing nodes are created.
- The custom data record, which is chosen by the variable type (given as a string), is also
  allocated and attached to this node.
- Different mechanisms, explained in later chapters, may be applied to already define the variable.
- If none of these mechanisms apply, the variable is considered "undefined" and the custom data record
  is considered invalid.

\I{------------------------------------------------------------------------------------------------}
## 3.2 Class Declaration ## {#alib_variables_declaration_decl}
A variable's declaration can contain two more values. Together four declaration values can be given:
1. The path name,
2. the type name,
3. a default value and
4. a description text.

These four elements can be specified by using an instance of class \alib{variables;Declaration}:

\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_DECLARATION3

The first advantage of using a declaration object lies in the fact that in case a variable should
be used from different places in the code, the declaration might be created only once and
shared between these places. This way, to change a variable's name, type, default value or
description, this has to be done only at one point in the code. All variable declarations should
be kept centralized somewhere.

\I{------------------------------------------------------------------------------------------------}
## 3.3 Declarations as Resourced Enums ## {#alib_variables_declaration_decl_resourced}
Variable declaration values can be defined using "externalized" resource strings.
For this, the concept of \ref alib_enums_records "C++ Enum Records", introduced with module
\alib_enumrecords, comes into play. With the extension of this concept by the possibility to
retrieve such enum records from resources, every aspect of a variable can be externally defined.
The using code exclusively uses a C++ enum element to refer to variables, not "knowing" even
what their name is.

As a sample, to declare aforementioned variable <c>"ALIB/LOCALE"</c>, the following single
line of code is needed:

\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_DECLARATION4

This works, because:
1. Type \alib{camp::Variables} is a C++ enum type and that type was
   equipped with enum resourced records:
   \snippet "camp/basecamp.mpp"     DOX_VARIABLES_DECLARATION5
2. The resources strings are defaulted during bootstrapping \alib as follows:
   \snippet "camp/basecamp.cpp"     DOX_VARIABLES_DECLARATION6
3. Likewise during bootstrap the records are read:
   \snippet "camp/basecamp.cpp"     DOX_VARIABLES_DECLARATION7

This is all that is needed to fully externalize variable declarations.<br>
If during the development of software, a new variable is needed, all that is to be done is:
1. Add an element to the enumeration, and
2. Add the resource strings for the variable.

## 3.4 Declarations using ALib Camps ## {#alib_variables_declaration_decl_camp}
The inclusion of the module \alib_camp with the header \implude{Camp}, injects various overloads
of the function \alib{variables;CampVariable} into the namespace #alib::variables of this module.

These functions are inline shortcuts used for creating variables associated with the
\alib{variables;Configuration} instance found in a \alib{camp;Camp}.

\I{################################################################################################}
# 4. Variable Definition # {#alib_variables_definition}
We chose the terms "declaration" and "definition" of variables, because they are so familiar
to C++ users. The one tells what name and type a variable has, the other tells where the memory
for a variable is found.

This implementation does not really go along these lines (and even technically could not).
Instead, with the declaration of an \alib variable, the memory for its value
is already allocated and accessible, namely in the nodes of the underlying
\alib{containers;StringTree}.

Let's again look at the code of the introductory hello-word sample. It read:

\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_HELLO_WORLD2

So why do we need to also "define" a variable? And even: does such a definition can go wrong or why
do we have this <em>if-statement</em>?

The truth is: We could leave the <em>if-statement</em> out, and even we do not need to
call \b Define() at all and could still write the string <em>"World"</em> into the variable, like this:
\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_HELLO_WORLD3

What would not work then, was to overwrite the value by passing a command line argument.
If passing

    --MY_VAR=Joe

Then the output would remain:

    Hello World

Conclusion: "defining" variables obviously means something different here.

\I{------------------------------------------------------------------------------------------------}
## 4.1 Variables and Priorities ## {#alib_variables_definition_prios}
A challenge that is inherent to working with <em>"externalized configuration variables"</em>, is
that contradicting values for a variable can exist. If such contradiction is resolved by ordering
all possible input sources along a <b>priority value</b>, the challenge can be resolved and
in fact turned into a feature that nicely resolves a variable value to what a programmer or
an end-user expects.

Let's first look at the priority values. Those are given with C++ enum type \b Priority.
Please read \alib{variables;Priority;this enum's reference documentation} now.

As you learn, a default value, that is usually residing in externalized resources along with
the variable name and type, is superseded by any value that is set with priority
\alib{variables;Priority;Standard}. Priority \b Standard is used (as default!) when common source code
assigns a value to a variable.

In turn, \b Standard is superseded by any of the external configuration data sources like
\alib{variables;Priority;ConfigFile}, \alib{variables;Priority;Environment} or \alib{variables;Priority;CLI}.
This make a lot of sense, as we saw in the introductory hello-word-sample. The hard-code value
"World" was overwritten by "Joe" if explicitly given as a command line argument.

Among the external sources superseding \b DefaultValues and \b Standard, priority \b CLI is the
highest, configuration files (like INI-files) are the lowest and between both are
environment variables and temporary session files.
If you think about this for a minute, this order becomes quite intuitive.

Enumeration type \b Priority is an \alib{enumops;ArithmeticalTraits;ALib arithmetical enum}.
Thus values can be easily compared and constants can be added or subtracted.
Imagine two configuration files:
1. One residing in a general configuration directory of the system, for example, <c>/etc</c> on
   GNU/Linux,
2. The other residing in a user-specific configuration directory, for example, <c>~/.config</c> on
   GNU/Linux.

Obviously, entries in the user-specific configuration file should supersede those in the general
configuration file. Hence, values read from the general configuration file would use
<b>Priority::ConfigFile</b> and values read from the user-specific file would use
<b>Priority::ConfigFile + 1</b>.

\I{------------------------------------------------------------------------------------------------}
## 4.2 Method Variable::Define ## {#alib_variables_definition_define}
With this background information, method \alib{variables;Variable::Define} can be explained.
It has a parameter that was not visible in the hello-world-sample, namely parameter \p{priority},
which is defaulted to \alib{variables;Priority;Standard}. Now, all that this method does is
to compare the given priority with the one that the variable already was defined with before.
If a previous definition was higher, \c false is returned! This indicates to the caller
that no value <b>should be</b> written. While a C++ compiler can forbid you to write to a \c const
value, the \alib variable system cannot forbid a programmer to still write a value
into the variable.

Therefore, the whole mechanism of "variable definition" has to be seen as <b>a contract between the
code entities!</b>
If all sources fulfill the contract, a well-organized management of external and internal
configuration data is achieved.

And as every rule has its exception, it can still make sense sometimes to
- provide a different value to method \alib{variables::Variable;Define} when setting variables in
  the code, or
- to just ignore the result of method \b Define,
- or to break the contract otherwise.

What is described here is the principle concept. Nothing is graved in stone.
One example is found in the next quick section.

\note To notify a programmer on the contract, method \b Define is annotated with <c>[[nodiscard]]</c>.
      Thus, a compiler warning might be raised if the value returned is not used. If such
      ignorance is intentional, the call has to be preceded by <c>(void)</c> to mitigate the
      warning.

\I{------------------------------------------------------------------------------------------------}
## 4.3 Protecting Variables ## {#alib_variables_definition_protected}
Enumeration type \alib{variables;Priority} defines element \b Protected. It has the maximum
possible priority value. If a piece of code uses this priority to assign a value, no other
entity will overwrite this value. At least no other code that obeys the contract, which is true
for all default plug-ins provided with \alib.

This way, it is possible to protect values against external modification.<br>
You might wonder: "If I do not want to allow external modification, I rather do not use an external
variable at all!" This of course is true for code under control. However, for 3rd party
code using \alib, this offers an easy way to disallow users of your software (which
incorporates that 3rd party code) to configure things that you do not want to be configurable.

An obvious sample is already found when software just uses \alib: Imagine you do not want to
allow \alib set a locale during bootstrap different from the one you designed your application
for. All you have to do is to modify bootstrapping to stop before the final phase, define
variable <b>ALIB_LOCALE</b> with priority \b Protected and assign the needed locale.<br>
This way, an end user cannot "hack" into your application and switch the locale.

Another sample comes with module \alib_alox. Here, the verbosity of log statements can be very
quite easily fine-tuned with external configuration data, like setting an environment variable.
Consider you use release logging which counts some transactions to an external server. Now you
want to forbid the default behavior which allows disabling such logging. Well, setting the
verbosity variable to \b Protected, stops any 3rd-party attempt to jump in.

In general, software security aspects must not be underestimated when using external configuration
data, which this module enables so conveniently. Always consider what someone with bad intentions
could do with your variables.

Protecting values can be performed in various ways, especially it is possible to set protected
values without declaring or defining a variable. This is important to understand, because
declaration and definition is only possible if the concrete variable type is known - and
accessible! Now, if a variable is defined in a non-accessible library code, still its value
can be protected. This will be shown in later chapters
\ref alib_variables_values_importexport and \ref alib_variables_values_presets.

\I{------------------------------------------------------------------------------------------------}
## 4.4 Testing on Variables ## {#alib_variables_definition_testing}
To recapture quickly what was said:
- Declaring a variable creates an entry in the \b StringTree including the variable's data storage.
  The "priority" of the variable is set to \alib{variables;Priority;Priority::NONE} which denotes
  this variable as "undefined", with no value assigned.
- Defining a variable changes the priority value, but otherwise does not do anything.
  technically a variable is perfectly established after declaration.
  (Precisely spoken, one more thing is done. This is described in later chapter
  \ref alib_variables_monitoring.)

Now, a code might want to just test whether some other code entity has defined a variable or
not, without creating an entry in the underlying \b StringTree. This can be achieved
with methods \alib{variables;Variable::IsDeclared} and \alib{variables;Variable::Try}.



\I{################################################################################################}
# 5. Variable Types # {#alib_variables_types}
So far, we have just seen variables of type <c>"S"</c> which are string types. With external
configuration, human-readable strings are a very common approach of exporting and importing
data and, for example, with command line parameters or environment variables, everything is received
as string types.

Nevertheless, \alib variables support arbitrary variable values. This means, not
only integers and floating points are supported, but complex custom structs and classes can
likewise be stored in variables.

\I{------------------------------------------------------------------------------------------------}
## 5.1 Built-In Variable Types ## {#alib_variables_types_builtin}
The following types are built-in and thus available for use without further efforts:

Declaration Type Name | C++ Variable type     | Description
----------------------|-----------------------|-----------------------------
 <c>B</c>             | \c bool               | A boolean value, that is parsed with method \alib{variables::Configuration,ParseBooleanToken} and optionally written back with \alib{variables::Configuration,WriteBooleanToken}.
 <c>I</c>             | \ref alib::integer    | An integral value which will be parsed when imported from external string values.
 <c>F</c>             | \c double             | A floating-point value which will be parsed when imported from external string values.
 <c>S</c>             | \alib{AStringPA}      | A string value. This is probably the most often used type.
 <c>SV,</c>           | \alib{StringVectorPA} | A list of strings, which is imported by parsing a comma-separated list.
 <c>SV;</c>           | \alib{StringVectorPA} | A list of strings, which is imported by parsing a list of strings separated by semicolons.
 <c>BOX</c>           | \alib{boxing;Box}     | A generic variable type using the power of \alib_boxing.

The value of a variable can be accessed with templated method \alib{variables::Variable;Get<T>}.
The line of code of the hello-world sample:
\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_BUILTIN_TYPES1
can therefore be generalized with:
\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_BUILTIN_TYPES2
(Note that, for this to compile, the header \implude{Strings.Monomem} has to be included.)

Method \alib{variables;Variable::GetString} is just an inlined shortcut to <b>Get<AStringPA>()</b>.
Similar methods for the other built-in types exists with
\alib{variables::Variable;GetBool},
\alib{variables::Variable;GetInt},
\alib{variables::Variable;GetFloat},
\alib{variables::Variable;GetDouble},
\alib{variables::Variable;GetString},
\alib{variables::Variable;GetBox},
\alib{variables::Variable;GetStrings()},
\alib{variables::Variable;Size} and
\alib{variables::Variable;GetString(int)}.

By the same token, corresponding assignment operators and implicit conversion operators have
been defined. This allows, for example, to write:

\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_BUILTIN_TYPES3

which uses the variable declared as type <c>"B"</c> just like a native boolean value.
But due to the various overloads, the compiler has to be fed with the precise type.
For example, stating

        varSwitch= 0;
        
in the sample above would not compile because \c 0 might fit to \c bool or other integral types.
Therefore, a precise cast hast to be given. In this case of type <b>"I"</b> it would be:

        varInt= alib::integer(0);
              


Having the <b>GetXYZ</b> methods and the assign- and conversion operators in place,
the built-in variable types have a very convenient and natural interface. However, it has to be
kept in mind, that no internal conversion takes place. Instead, accessing a variable using a
wrong type, \ref alib_mod_assert "raises an ALib error" with debug-compilations.
With release-compilations, such wrong access is <b>undefined behavior</b>.

A variable of type <c>"BOX"</c> may be used to have different parts of software store different
sorts of data. Of course all limitations and precautions in respect to type-guessing,
life-cycle considerations, etc. that are explained with the Programmer's Manual of module
\alib_boxing have to be respected. The \alib{variables;VMeta} implementation (explained only in the
next chapter) for this type imports the given string data as follows:
- If parsable as integral (without remains in the given source string), the integral is boxed.
- If parsable as a floating point value (without remains in the given source string), a \c double
  is boxed.
- Otherwise, just the given String is boxed. (Attention, no copy is generated, the source string
  has to survive the lifecycle of the variable in the tree!).

For exporting, class \b VMeta just appends the \b Box to an \b AString. Consequently, all
mechanics of serializing boxes as described with that module are in place.
 
\I{------------------------------------------------------------------------------------------------}
## 5.2 Custom Variable Types ## {#alib_variables_types_custom}
Enabling the \alib variable system to create and manage variables that contain custom
structs and classes, is considered an advanced use of the API.
While the effort is not very high, some basic understanding on how this camp internally manages
variable types and values is helpful.

\par Please Read Now:
To avoid redundancy, we would ask you to read the following pieces of documentation:
- Reference documentation of class \alib{variables;VMeta}.
- Preprocessor macros
  - \ref ALIB_VARIABLES_DEFINE_TYPE,
  - \ref ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR and
  - \ref ALIB_VARIABLES_REGISTER_TYPE.
- Method \alib{variables;Configuration::RegisterType}.
- Chapter \ref alib_contmono_poolallocator_metatrick of the Programmer's Manual of module
  \alib_monomem.

There are no technical restrictions in respect to the custom type that is to be stored in variables,
apart from one: The type's memory alignment must not be greater than '<c>alignof(uint64_t)</c>'.

\note This is due to the use of the \alib{monomem;TPoolAllocator;PoolAllocator} for allocations and
      recycling.
      This value might be changed by passing a custom value to compilation symbol
      \ref ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT.
      See chapter \ref alib_variables_internals_memoman for further information.

With this pack of theory in place, let's look at a step-by-step sample. Camp \alib_alox introduces
various own variable types, one of them defines the output meta-information and format of log
statements. Here are the steps that have been taken:
1. Declare an ordinary type that determines the variable's contents. The struct used is
   \alib{lox::textlogger;FormatMetaInfo} and it contains six members, all being of string type
   \alib{AStringPA}.<br>        <br>
2. In the header that declares the struct (outside any namespace), the following macro
   is used to create a custom \alib{variables;VMeta} derivate:
   \snippet "lox/textlogger/variables.inl"     DOX_VARIABLES_DEFINETYPE1
   Note that we are using the second version of the macro, because the string members of type
   \b FormatMetaInfo are of pool-allocated \alib{AStringPA} type.
   Those need a \alib{monomem;TPoolAllocator;PoolAllocator} with construction and here, with the
   declaration of a variable, the singleton instance of the associated \b Configuration is
   passed.<br><br>
3. The preprocessor macros are missing to define virtual abstract methods
   \alib{variables;VMeta::imPort} and \alib{variables;VMeta::exPort}. Those have to be defined in a
   compilation unit like this:
   \snippet "lox/textlogger/variables.cpp"     DOX_VARIABLES_DEFINETYPE2
   Note the name <em>"VMeta_FormatMetaInfo"</em> of the meta info type. This name is assembled
   by the macro used in the previous step.<br>
   To get a full understanding about what this code does, please refer to later chapter
   \ref alib_variables_values_importexport.<br><br>
4. In the bootstrap code of camp class \alib{lox;ALoxCamp}, the variable type is registered with
   the associated configuration:
   \snippet "lox/aloxcamp.cpp"            DOX_VARIABLES_DEFINETYPE3
   <br><br>
5. Finally, for easy access, class \b TextLogger received inline shortcut method
   \alib{lox::TextLogger;GetFormatMetaInfo}, which just returns the result of
   \alib{monomem;Variable::Get;Get<FormatMetaInfo>()} on the variable which is hold as a member
   of the logger type.



\I{------------------------------------------------------------------------------------------------}
## 5.3 Extending Class Variable ## {#alib_variables_types_custom_derive}

The convenience methods GetXYZ(), assignment operators and conversion operators that are existing
for the built-in types (see previous chapter \ref alib_variables_types_builtin, are of course
not existing for custom types. Hence, variable value read/write access is always to be made
using templated method \alib{monomem;Variable::Get<T>}. In the sample of the previous chapter,
the final step 5. added a convenience method to type \b TextLogger which hides the whole fact
that the meta-information struct resides in a configuration variable.

Should you use variables very intensively, it can be a good idea to derive an own custom variable
type from class \b Variable. In this case besides adding the convenience functions for the different
content types, a tip would be to then also override the constructors of this custom variable type
to omit the parameter that specifies the configuration.
This way, your custom type would always automatically bind to the configuration instance that
these variables should reside in.

The reason why we did not do this with camp \alib_alox is that we have the access methods in place
as just explained, and if we had an own variable type in addition, this would have added a next
type to the reference documentation, and thus just added complexity to the user of the API, which
is not necessary the code internally accesses the variables in the not so convenient fashion.

\I{################################################################################################}
# 6. Variable Values, Presets and Substitution # {#alib_variables_values}
\I{------------------------------------------------------------------------------------------------}
## 6.1 Im- and Export # {#alib_variables_values_importexport}
In the previous chapters, it was seen already that variables can be declared by providing a
default value. For this, either \alib{variables;Declaration::DefaultValue} is used or parameter
\p{defaultValue} of overloads of constructor of type \b %Variable or its declaration methods.
In all cases, independent of the variable type, this is a string value.

With method \alib{variables;Variable::Import}, string values can be used to set a variable value
even after its declaration and a variable can likewise be serialized into a string value with
method \alib{variables;Variable::Export}.

These interface methods can be used without "knowing" a variable's type. For example, the following
code does not use a variable's type:

\snippet "DOX_VARIABLES.cpp"     DOX_VARIABLES_IMPORT

Instead, this code only tests if the variable was declared and if it was, it passes a string
value to import.

\note In this sample, the priority is set to \alib{variables;Priority;Protected}
      which indicates that no other configuration source is allowed to set any other value.
      See previous chapter \ref alib_variables_definition_protected for more information on this.

From this, it becomes obvious that serializing and de-serializing variables from string values is
an inherent concept of this \alibcamp_nl. And as indicated with the abstract interface methods
\alib{variables;VMeta::imPort} and \alib{variables;VMeta::exPort}, this camp talks about importing and
exporting variable values (instead of using the word serialization).

This concept is very important to understand: Any value of any built-in or custom variable type
can be noted in a human-readable string. There are different rationals for this:

- First of all, most external configuration systems exclusively use string values to define
  data, even for integral or number values. This is true for command line parameters, environment
  variables and all sorts of text-based configuration files.
- By having such foundational common base type <em>'string'</em>, it is possible to provide
  values for variables, whose type is unknown. Only later, when a using code declares a variable
  such a provided value will be imported. In the context of this camp, we call this
  <em>"Variable Presets"</em>. This is covered in the next chapter.
- String values allow easy replacements of values. In this context we call this
  <em>"Variable Substitution"</em>, which is a next powerful concept, explained in chapter following
  the next.

Before the concepts of <em>Presets</em> and <em>Substitution</em> are detailed, a challenge arising
by working with strings has to be discussed. This challenge is that there is a difference
between strings that are human-readable (and, for example, editable in a text file), and those
used by a C++ program internally. The difference is that, all non-printable characters
like line-feeds, tabulators, leading and trailing spaces, quotation marks, etc have to be "escaped",
when externally stored, just like these characters are escaped in a sting definition in
the C++ source code.

Unfortunately, it is not sufficient to just unescape a given string and then store this C++ string
for later imports, because the escape mechanics depend on the final variable type. Take the
following two import strings:

        Greets, 100
        "Greets ", 100
        "Hello, my name is \"Joe\"", 200

The corresponding custom variable type's import/export format definition, obviously expects
a string type and an integral value. The import (deserialization) code obviously uses a comma to
separate the values. Now, when de-escaping the import string, it has to be ensured, that the comma in the third
sample is not seen as a separation character. Therefore, parsing the different tokens has to
take into account that this comma is enclosed in quotation marks. If the import string as a whole
was just converted to a C++ string representation, this information was lost.

For escaping strings, virtual utility type \alib{strings::util;StringEscaper} and its derivate
\alib{strings::util;StringEscaperStandard}, provided by module \alib_strings, is used.
Whenever values are im- or exported, an instance of one of both types (or a derived custom type)
has to be available and with the just given samples, it becomes clear that only with the declaration
of a variable, when the type and import mechanics are known, the escaping can take place.


\I{------------------------------------------------------------------------------------------------}
## 6.2 Preset Values # {#alib_variables_values_presets}
Let's recap the prerequisites made in the previous chapter:
- Each variable type (built-in or custom types), needs to provide import and export capabilities.
- A parameter of the import and export methods is an escaper instance that allows
  escaping the values from (and to) a proper externalized string format, provided (and usable) by the
  target configuration system.

With this in place, the concept of <em>Preset Values</em> was implemented with this camp.
Its use is very simple through overloaded methods \alib{variables;Configuration::PresetImportString}.

The methods store the given string along with an escaper and a priority in a hidden section
of the \b StringTree that the configuration instance is based upon. If the given string \p{value}
is \e nulled, a previously set value is removed.
Once the variable is declared, the preset value will be imported using the given priority.
(This is of course only performed if no different preset value was given with a higher priority
and if no configuration plug-in with a higher priority defines the variable.)

While this looks like "just another feature" of the camp, in fact presetting import values is
a very fundamental concept. Remember that otherwise, setting variable values is only possible
once a variable is declared. And for this, the using code needs to know the variable's type.

More on using this concept is described in later chapter \ref alib_variables_external.

\I{------------------------------------------------------------------------------------------------}
## 6.3 Substitution Values # {#alib_variables_values_substitution}
When values are imported, class \b Configuration by default substitutes references to other
configuration variables found in the import value of the requested variable.

For example, if two variables are defined as follows:

        MY_RESULT= 42
        MY_VARIABLE= The result is ${MY_RESULT}

then, with the declaration of variable \c MY_VARIABLE, variable \c MY_RESULT is read and the
substring "${MY_RESULT}" is substituted by "42".

Substitutions are performed repeatedly until all variables are resolved. Therefore, nested
substitutions may be defined as well.

If a variable that is referenced as a substitution value is not declared yet, then it is checked
if a preset value exists. If so, this preset value is imported (using the right escaper) to
a string variable and exported back using the escaper of the substituted variable.


\attention
  If a variable is not found during the (nested) substitution process, then an empty string is
  substituted. This is likewise \c true for the case that circular substitution occurs.
  No warning or error message is given. If it is needed to detect substitution errors, the
  resulting value of a variable has to be checked, according to the expected value or pattern,
   which is depending on the using code


By default, enclosing \c "${}" is used to recognize variables. This can be altered using members
\alib{variables::Configuration;SubstitutionVariableStart} and
\alib{variables::Configuration;SubstitutionVariableEnd} of class \b %Configuration.
With both, It is also possible to specify a prefix and a suffix for the identification of
substitutable variables in other variable's values. For example, the syntax can be adjusted to

        MY_VARIABLE= The result is %MY_RESULT

A third member is \alib{variables::Configuration;SubstitutionVariableDelimiters}. Please refer
to all three member's reference documentation for further details.

Variable substitution can be a powerful concept and should be explained to end-users of an
application. Consider an application that exposes a variable called <em>"OUTPUT_FORMAT"</em>.
A user may alter the format in a configuration file or with command line parameters. Whenever
the user changes it, the former value has to be overwritten. With the concept of variable
substitution, a user could create own variables, for example, in an INI-file, which are not
even specified by the application, for example

        LONG= ...a long format...
        SHORT= ...a short format...

and then just alter the original variable's value between:

        OUTPUT_FORMAT= ${LONG}
and

        OUTPUT_FORMAT= ${SHORT}

The user could even start its software by passing a command line parameter

        --OUTPUT_FORMAT=${SHORT}

or whatever else is defined in the INI-file.

Alternatively, an application could create a set of presets for a variable and depending
on a command line parameter like <em>"--verbose"</em> could set a variable to substitute one
of the presets.


\I{################################################################################################}
# 7. Attaching External Configuration Systems # {#alib_variables_external}
\I{------------------------------------------------------------------------------------------------}
## 7.1 General Aspects ## {#alib_variables_external_general}
In general, external configuration data can be made available to the \alib variable system
in two possible ways:
1. When a variable is declared for the first time, the external source is checked if a corresponding
   value exists.
2. All external data is read when an application starts, and all external variables found are
   set.

The decision about which one is the right way to go mostly depends on the type of the external
data. Here is one example for each implementation possibility:
1. Environment Variables are general sources targeting plenty of applications and aspects.
   Most of them are not related to the application running in this environment, but are addressing
   other applications or general information. Here it would be a huge waste, if just the whole
   environment was fed into the configuration system and therefor held doubly.<br>
   Thus, method 1. is chosen.
2. Configuration files are specific to an application and hold data that in its majority is used by
   a concrete application run (individual usage of some variables may still depend on the concrete
   execution path of a run of that application).<br>
   Thus, method 2. is chosen.

The following chapters explain both approaches and a little more.

\I{------------------------------------------------------------------------------------------------}
## 7.2 Built-In CLI and Environment Plug-Ins ## {#alib_variables_external_cli_env}
As just explained, some data sources may be read only on request, instead of feeding all
configuration data to the \alib variable system on bootstrap. For such sources, class
\alib{variables;Configuration} provides a plug-in mechanism. Simple virtual base class
\alib{variables;ConfigurationPlugin} describes the interface that is to be implemented.
When done, the plugin can be attached with method \alib{lang;PluginContainer::InsertPlugin}.

For two of such sources, built-in solutions exist with types
- \alib{variables;EnvironmentVariablesPlugin}, and
- \alib{variables;CLIVariablesPlugin}

Both plug-ins are created and attached with construction of a configuration instance (respectively
its method \alib{variables::Configuration;Reset}) in case parameter \p{createDefaults} is not
specified to suppress such creation.

\I{------------------------------------------------------------------------------------------------}
## 7.3 Built-in Class IniFileFeeder ## {#alib_variables_external_ini}
As explained in the \ref alib_variables_external_general "introduction of this chapter", configuration
data that is specific to an application is recommended to be read and fed into the configuration
system in one shot with application bootstrap.

A challenge arises from the fact, that external configuration systems usually do not dispose
about a variable's declaration information, especially about the type information.
All that, for example, an INI-file stores is externalized string data, which cannot be imported
(un-escaped and parsed) to a variable value, without this information.

There are three ways to cope with this problem:
1. The use of method \alib{variables;Configuration::PresetImportString}. This method stores
   import strings together with a \alib{strings::util;StringEscaper} needed for the later import,
   in an otherwise hidden branch of the \b StringTree of a \b Configuration object.<br>
   These values are automatically imported at the moment a variable is declared.
2. The use of method \alib{variables;Configuration::PreloadVariables} \b before feeding external
   data. Note that this method relies on variable declarations, that are duly externalized
   by using \ref alib_enums_records "resourced C++ enumeration values", as described in previous
   chapter \ref alib_variables_declaration_decl_resourced.
3. If 2. is not applicable (because the declarations have not been externalized in enum records),
   of course, "manual" declarations of variables can be made before an import.

\par Please Read Now:
    To avoid redundancy, we would ask you to read the reference documentation of the links in the
    three options listed above, to get a full understanding of the concepts.

Now, with all this knowledge in place, a step-by step recipe for using built-in type
\alib{variables;IniFileFeeder} can be given and this should be easily understood:

<b>With the start of software:</b>
- During or after bootstrapping of an application, use method
  \alib{variables;Configuration::PreloadVariables} to declare your custom variables.
  Alternatively, if no enum records are in place, do this in a manual way for each variable.
- Create a local instance of class \alib{variables::IniFileFeeder}.
- Use method \alib{variables::IniFileFeeder;ImportStart} to load an INI-file.
- Use methods named \b Import (or own specialized code) to declare variables in the configuration.
- Call \alib{variables::IniFileFeeder;ImportEnd} to free all memory allocated by the INI-file loaded.

Here is a sample for this approach, taken from the Programmer's Manual of module \alib_alox:
 \snippet "ALox/sample.cpp"            DOXYGEN_CREATE_INIFILE


<b>With termination of software:</b>
- At the end of an application, right before shutting down \alib, again create a local instance of
  class \b IniFileFeeder.
- Use method \alib{variables::IniFileFeeder;ExportStart} to open the same INI-file as after bootstrap.
- Use methods named \b Export (or own specialized code) to write the variables of interest
  into the INI-File. This populates empty INI-file with options, and only potentially
  overwrites existing values - just as needed and wanted by programmer and by the end-user.
- Optionally, method \alib{variables::IniFileFeeder;AddResourcedSectionComments}
  can be invoked to add (externalized) comments to the sections of an INI-file.<br>
  Furthermore, the internal INI-file can be retrieved with \alib{variables;IniFileFeeder::GetIniFile}
  and missing \alib{variables::IniFile;FileComments} may be set (or other tweaks performed).
- Invoke \alib{variables::IniFileFeeder;ExportEnd} to write the INI-file.

Following the above sample, this code could look like this:
 \snippet "samples/ALox/sample.cpp"            DOXYGEN_REMOVE_INIFILE


\I{------------------------------------------------------------------------------------------------}
## 7.4 Attaching a Custom Configuration System ## {#alib_variables_external_custom}
In case a custom configuration system is to be attached, all information given in the previous
chapters should be duly read and understood.

Next, the source code of class \alib{variables;IniFileFeeder} should be analysed. The good news
here is that most INI-file related code resides in separate class \alib{variables;IniFile} which
\b IniFileFeeder uses.

With that, the code is easily understandable and rationally short to be used as a jump start.
In addition, information given in chapter \ref alib_variables_stringtree might be helpful.

Another detail to be discussed is that export methods of variable types are allowed to insert
\alib{NEW_LINE} code into the export string. An external configuration system probably has to
check if such codes exist and properly handle them. While the easiest way to handle them is of course
to simply remove them, this feature allows external configuration systems to smoothly format longer
lists of values. Built-in class \alib{variables;IniFileFeeder}, respectively the internally used
helper-type \alib{variables;IniFile}, creates duly formatted lines and adds a continuation backslash
<c>'\\'</c> to the previous line. Duly formatted here means that the values are written in
a tabular way. See for example, the following INI-file entry, generated for an \alox variable:

    # Meta info format of logger "DEBUG_LOGGER", including signatures for verbosity strings and
    # astring added to the end of each log statement.
    # Format: MetaInfo,Error,Warning,Info,Verbose,MsgSuffix
    FORMAT=    "%SF:%SL:%A3%SM %A3[%TC +%TL][%tN][%D]%A1#%#: %V    ",   \
               \ec0,                                                    \
               \ec3,                                                    \
               ,                                                        \
               \ec8,                                                    \
               \e[0m



The built-in string vector types <c>SV,</c> and <c>SV;</c> insert a \alib{NEW_LINE} code after each
character.

\I{------------------------------------------------------------------------------------------------}
## 7.5 Writing Session Information ## {#alib_variables_external_session}
Session information data is a special sort of external configuration data. Its character is quite
volatile: Nothing bad happens if no data is given and storing the information might be a matter
of a background task that does this only in certain time intervals. A good example is the
recent position of an application window:
- If no data on the position is found, an application lets the window manager decide on the
  position of the window, maybe giving it a minimum or average size.
- If data exists, this data is used to open an application window just at the place it was recently
  closed.
- If a window is moved, this information might be lazily stored, together with similar configuration
  data.

Two ways to write session information are proposed in the next subchapters. Both approaches have
advantages and disadvantages which are rather obvious and thus are not further elaborated here.


### 7.5.1 Using A Dedicated Session File ### {#alib_variables_external_session_dedicated}
The right and preferred way to manage session information is to create a dedicated session file.
Often such data is stored in a temporary folder or in folder <c>~/.cache</c>, <c>/tmp</c> or similar.
Using the \alib variable system provides all necessary tools needed to quickly implement
a session file. Here are the steps to do so:
- Gather all session-related variables in an C++ enum type, for example, called
  <b>"MySessionVariables"</b>.
- The names of the variables should all include the same prefix path, for example
  <b>"/MYAPP/SESSION/"</b>. While this is not mandatory, it simplifies the maintenance of the
  software in case variables are added, renamed or removed.
- Preferably equip this type software with corresponding resourced enumeration data.
- Register a listener for monitoring the branch of variables (and/or specific session-related)
  variables. See upcoming chapter \ref alib_variables_monitoring for information how to do so.
- When notified on changes, create an instance of class \alib{variables;IniFileFeeder}, invoke method
  \alib{variables::IniFileFeeder;ExportStart} by providing a filename within a temporary folder.
- Then use method \alib{variables::IniFileFeeder;Export}
  to export the subtree of session variables. For example:

        myIniPlugin.Export( "/MYAPP/SESSION" )

  and then write the INI-file with \alib{variables::IniFileFeeder;ExportEnd}
- The former two steps could also be placed in a background task that defers the writing of the
  session file and asserts a maximum number of updates in a certain period. If done so, of course
  thread-safeness has to be asserted. (See later chapter \ref alib_variables_multithreading).

- Of course, when the application starts, the corresponding methods
  \alib{variables::IniFileFeeder;ImportStart}, \alib{variables::IniFileFeeder;ImportAll} and
  \alib{variables::IniFileFeeder;ImportEnd} have to be used with a similar \b IniFileFeeder instance,
  to try and read the most recent session information.

### 7.5.2 Store Session Information Along With Other Configuration Data ### {#alib_variables_external_session_mixed}
A different approach might be taken if software uses a write-enabled configuration data source.
Of course, session information can then be stored along with the read-only data.

This module provides, for example, built-in type \alib{variables;IniFileFeeder} which provides an
explicit interface to cope with session data. Here are the steps to implement mixed configuration
and session information using this type:
- Equip your software with code that populates INI-files with default values in the case
  they were empty. This is done with termination of a process and described step-by-step in chapter
  \ref alib_variables_external_ini.
- In the previous step, populate the INI-file with all variables, configuration data and session
  data.
- For those variables that resemble session values, invoke method
  \alib{variables;IniFileFeeder::SetWriteBackFlag}. As documented, this method sets the
  \ref config_IniFile_writebackflag "writeback flag" only for variables that did not exist
  before. This way, the end-user is enabled to remove the flag and prevent the change of a value
  with future sessions.

With the above in place, those entries that have the writeback-flag set, are overwritten by
future application runs, in contrast to those entries that are only written once when an empty
INI-file is populated for the first time.


### 7.5.3 Session-Variables in ALib ### {#alib_variables_external_session_builtin}
As of today, only one built-in variable that have a notion of session data exist with
\ref alxcvALOX_LOGGERNAME_AUTO_SIZES. This is exposed by module \alib_alox.


\I{################################################################################################}
# 8. The StringTree in Class Configuration # {#alib_variables_stringtree}

It has been mentioned already several times in this manual, that class \alib{variables;Configuration}
inherits class \alib{containers::StringTree}. A bigger portion of its logic builds
on this base type. Consequently, class \alib{variables;Variable} encapsulates a
\alib{containers;StringTree::TCursor} and with that it is just a lightweight pair of
pointers (which even fits into a \alib{boxing;Box}).

In some special situations, for example, if a custom plug-in or "feeder" of external configuration
data is to be written, it is necessary to use the two underlying interfaces.

This chapter of the manual should give some hints for doing that.

\I{------------------------------------------------------------------------------------------------}
## 8.1 Access to the StringTree # {#alib_variables_stringtree_access_tree}
Class \b Configuration inherits class \c StringTree directly and publicly.
Hence, all functionality is directly accessible.
When using the StringTree interface, often values of type \alib{containers;StringTree::Cursor}
are involved. This type provides method
\doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a75140d731555edad87151e9bec346413;StringTree::TCursor::Tree},
which, for convenience, allows providing an optional template type to statically cast to.

Thus, when casting a \b StringTree instance back to class \b Configuration, instead of using the
rather unhandy syntax:

       *static_cast<Configuration*>( myCursor.Tree() )
       
the better way to phrase this, is:

       myCursor.Tree<Configuration>()
  

\I{------------------------------------------------------------------------------------------------}
## 8.2 Access to the Cursor # {#alib_variables_stringtree_access_cursor}
Class \alib{variables;Variable} inherits class \alib{containers;StringTree::TCursor}
in a \c protected fashion. The rationale for this is not to completely hide the cursor-interface.
Instead, this is meant to duly hide the interface for the 99% of use-cases where a variable
is used in code.

Because class \c Variable does not add any field-members, a variable can be "converted" into
a cursor and vice versa. Conversion to a cursor is provided with method
\alib{variables;Variable::AsCursor}, which is just a static cast! The cursor can be manipulated
freely. Once done, it can be used as a constructor argument of class \b Variable.

Note that not any position in the tree that a cursor may represent, in fact is a variable.
Instead, <b>a cursor can point just be node in the path to a variable!</b>
For an example, let us look at some \alib variables:

    /ALIB/LOCALE
    /ALIB/WAIT_FOR_KEY_PRESS
    /ALIB/HAS_CONSOLE_WINDOW

If a cursor was generated from one of these variables and next method
\doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a2d1c92248627f9342f2b95ba80cb74c8;TCursor::GoToParent}.
was invoked, and then this cursor
was passed to the constructor of class \b Variable, then this variable was illegal.

This simple sample already demonstrates why the interfaces of class \b StringTree and \b Cursor
are hidden: It needs some core understanding of the internal mechanics to safely use them.

To probe a cursor for either being a variable or just a node of a path in the tree, a temporary
variable can be constructed from the cursor and method \alib{variables::Variable;IsDeclared}
invoked. If \c false is returned, the cursor represents a path node.

\I{------------------------------------------------------------------------------------------------}
## 8.3 Internal Sub-Tree $PRESETS # {#alib_variables_stringtree_presets}
When traversing the internal \b StringTree of variables, a first-level node named <c>"$PRESETS"</c>
will be found. Beyond this node, class \c Configuration stores information provided with method
\alib{variables::Configuration;PresetImportString}. This node and its children have to be skipped
explicitly when traversing the tree.

\I{################################################################################################}
# 9. Monitoring Changes of Variables # {#alib_variables_monitoring}
The \alib variable system provides a mechanism that enables software to monitor changes that
occur in the tree of data. For this, a custom implementation of abstract interface type
\alib{variables;ConfigurationListener} is to be created and registered with a configuration instance.

## 9.1. Event Types ## {#alib_variables_monitoring_events}
There are three types of events that can be monitored. They are defined with enumeration
\alib{variables;ConfigurationListener::Event}:

1. <b>Event::Creation</b><br>
This event is notified to listeners after a variable is <em>declared</em> for the first time.
The reason why this event is named <em>"Create"</em> and not <em>Declared</em> is that only
the first declaration in fact creates a variable. Future declarations (often made from a different
part of software) refer to the same variable data and are often only performed to read the value
which is not of interest for a monitoring entity.

2. <b>Event::Definition</b><br>
This event is notified when a variable is "successfully" defined. Precisely, it is fired by
method \alib{variables;Variable::Define} in case the method returns \c true. With that, this event
is used to monitor changes of a variable's value.<br>
As explained in chapter \ref alib_variables_definition_define, the whole concept of defining variables
is implemented as a "contract" and is not protected, unbreakable mechanism. This is important to
understand in respect to monitoring this event: The monitoring entity needs to duly follow the
contract to ensure that listeners become notified on changes.<br>
Usually, both software entities - the one that performs the changes and the one that monitors
those - are aware of each other or at least the changing part is aware that a certain variable
is a potential subject to monitoring, and thus should implement definition invocations along with
value changes accordingly.

3. <b>Event::Deletion</b><br>
Finally, this events notifies variable deletions. When the event is fired, the variable instance is
still intact and accessible.

## 9.2. Registration of Listeners ## {#alib_variables_monitoring_registration}
While there is only one interface type \alib{variables;ConfigurationListener}, which only has
one simple virtual notification callback function, \b five different ways of registering such
listener for monitoring one or more variables exist. Those are:

1. \alib{variables::Configuration;MonitorDistinctVariable}<br>
   Used for monitoring a distinct single variable. Note that this method cannot be used to monitor
   the creation of a variable, because a first declaration has to be performed for the provision of
   the parameter. In case the later creation of a distinct variable is to be monitored,
   method \b 4. is to be used.<br><br>

2. \alib{variables::Configuration;MonitorVariablesByName}<br>
   Used for monitoring all variables that share the same name (residing in any path).<br>
   As sample use-case, built-in configuration variable \ref alxcvALOX_LOGGERNAME_AUTO_SIZES
   can be considered: With it's placeholder name, different variables for different \alox loggers
   might be created. Each variable has the name <em>"AUTO_SIZES"</em>, but they reside in
   different paths, depending on the logger name. Now, if changes of the auto-sizes values should be
   stored in a temporary session file, all such variables would be fetched at once if string
   <em>"AUTO_SIZES"</em> was passed to this method.<br><br>

3. \alib{variables::Configuration;MonitorPath;MonitorPath}<br>
   Monitors all variables of a subtree of the configuration system. The subtree is specified
   with a cursor object.<br><br>

4. \alib{variables::Configuration;MonitorPathPrefix;MonitorPathPrefix}<br>
   Same as \b 3. but allows specifying the subtree by a prefix string. This approach imposes a minimum
   performance impact, but allows registering a listener before the creation of a path.
   By providing a full path string, including a variable name, this method can also be used
   as a replacement for method \b 1. and used in cases a variable is created only later by a
   software.<br><br>
   
5. \alib{variables::Configuration;MonitorPathSubstring;MonitorPathSubstring}<br>
   Monitors all variables whose full path (including the variable name) contains the given string.
   

Each method accepts four parameters:
- Enumeration \alib{lang;ContainerOp}, to specify if a listener should be added or removed
- The listener instance
- The \alib{variables::ConfigurationListener;Event} to get notified on.
- The forth parameter is specific to the type of listener registration.

To de-register a listener, parameters two to four have to be specified <b>exactly as with
registration</b>, along with the first parameter being \alib{lang;ContainerOp;ContainerOp::Remove}.
If this is not done, de-registration fails and the listener remains set which may cause undefined
behavior. Therefore, in debug-compilations, an \alib_warning is raised if the exact combination of
the parameter-set was not found internally.

An alternative, more convenient and in most cases sufficient method to de-register a listener
is offered with method \alib{variables::Configuration;MonitorStop}, which removes all registrations
for a given listener.

\I{################################################################################################}
# 10. Internal Memory Management # {#alib_variables_internals_memoman}
Class \b Configuration, acts as the container type for all variable data. With its inheritance of
class \alib{containers;StringTree} of camp \alib_containers, it fully implements this camp's
concept of \ref alib_contmono_intro_strictweak "weak monotonic allocations". The challenge of
extending the \ref alib_contmono_intro_recycling "recycling capabilities" of class \b StringTree
to the user-defined variable content types, was resolved by adding a
\alib{monomem;TPoolAllocator;PoolAllocator} which uses the \alib{MonoAllocator} of the
underlying \b StringTree.<br>
As a result, infinite insertions and removals of similar data will not cause a "memory drain".
Instead, all allocated memory is re-used and hence the processes' heap memory remains completely
un-fragmented, because all that class \b Configuration allocates are several bigger blocks of
memory, until the "weighted limit" of the necessary space is reached.

Because this introduces a very nice showcase for the mentioned concepts of module \alib_monomem_nl,
its Programmer's Manual provides chapter \ref alib_contmono_poolallocator_metatrick, which discusses
implementation details of class \b Configuration.

\I{################################################################################################}
# 11. Multi-Threading / Racing Conditions # {#alib_variables_multithreading}
This \alibcamp does not provide built-in facilities to avoid racing conditions in multithreaded
processes. It is up to the user of the library to make sure that no conflicting operations are made.
(Note that this is the common approach. For example, the container classes in namespace \c std do
not provide such mechanics).<br>

With the compiler-symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS set, the underlying \b StringTree receives
assertion mechanics to detect forbidden parallel access. More information on how this works is
provided with chapter \ref alib_threads_intro_assert_entry of the Programmer's Manual of module
\alib_threads.

To implement protection against race conditions efficiently, some understanding of the internal
implementation is necessary.

The general rule is:
\attention Parallel execution of insertions and/or removals of variables in a \b Configuration
           is not allowed, and using software has to take measures to protect such operations.

This is again very similar to container classes in namespace \b std. The good news is that
accessing a distinct \b existing variable is fully independent of parallel insertions or deletions
of other variables in the tree. Precisely, the following rules are guaranteed:

1. Class \alib{variables;Variable} is a set of two pointers, one to the internal
   \alib{containers;StringTree} and a second to the specific node in the tree that stores
   a variable's meta-information. <br>
   Once a variable is declared, a corresponding \alib{variables;Variable} object remains valid,
   independent of insertions, modifications or deletions of other variables in the tree.
   This is even true if siblings or children within the tree are concerned.
   Therefore, such variable may be copied and stored and accessed from different threads
   after declaration, as long as no deletion or definition is performed.
2. Once a variable is defined, an instance of its associated data record is allocated and attached.
   For this record, the same rules apply: As long as the variable is not deleted, the record
   remains valid.
   Consequently, parallel read/write operations on a variable's record are due to the same cautions
   as if the record was allocated in a common and there is nothing special to note.
3. Re-definitions of a variable with a higher priority impose, as was explained in this manual,
   only a change in the integral priority value stored with the variable.
   With that, on certain platforms such change with a parallel read of the value
   (For example, by calling method \alib{variables::Variable;IsDefined} may result in undefined values.
   However, the previously existing data record is not moved or otherwise touched.
4. To explicitly mention it: Points \b 1. and \b 2. are also true if a rehash of the underlying
   \alib{containers;HashTable} of the \b StringTree is re-hashed at the moment further
   variables are inserted into a \b Configuration instance.
5. When accessing the underlying <b>StringTree::Cursor</b> by calling \alib{variables;Variable::AsCursor}
   a little more caution has to be taken: Walking the string tree (as described in the reference
   documentation of the tree and cursor types) must be protected against parallel insertions or
   removals of tree nodes. However, such operations usually are only performed during
   bootstrap and shutdown of software, when external configuration and session data is read
   respectively written. In these phases, usually all working thread of an application are
   either not started yet, or have been stopped already.
6. Methods \alib{variables;Variable::IsDeclared} and \alib{variables;Variable::Try} are safe to be
   used along with other read operations and read-only cursor movements in the \b StringTree.
   They are specifically designed to not touch the tree. In case method \b Try returns \c true
   the variable can also be accessed.

 

\I{################################################################################################}
## 11.1 Internal ALib Variable Creations And Deletions ## {#alib_variables_multithreading_withalib}
As learned in the previous section, only insertions and deletions of variables interfere
with each other and with cursor operations in the same subtree of a configuration.

As described chapter \ref alib_mod_bs_customize of the general manual of \alib,
different \alibcamps and custom camps may share a single configuration object and it is allowed
and recommended to use this same configuration object to store custom configuration data.
The variables that are stored by \alibcamps_nl are listed in the corresponding
\ref alib_cfgvars "reference manual".

The only occurrence of insertions and deletions of variables built-in with any \alibcamp_nl
is when instances of class \alib{lox::textlogger;TextLogger} are inserted to or removed from a
\alib{lox;Lox}. Now, in common applications, loggers are inserted during bootstrapping and removed
with the shutdown phase of software. With that, such process can rely on the fact that \alib
will not perform insertions or deletions of variables while multi-threading is in place.
Consequently, only custom insertions and deletions have to be protected.<br>
The other way round: If a multithreaded software inserts or removes \b TextLogger instances during
its lifetime, such operations have to be protected against each other and against any other
insertion and removal operation of custom variables.

This camp \alib_variables_nl can be very well used for other purposes than storing "just"
configuration data. If so, it is recomendet to not rely on the built-in instance shared with
the \alibcamps_nl, but rather created a dedicated \b Configuration object. Such object is then
under complete control of the application, independent of other camps and thus not subject
to race conditions introduced by such.

\I{################################################################################################}
## 12. Use Cases Apart from Configuration Data## {#alib_variables_usecases}
While the motivation to introduce this \alibcamp_nl results from the need of
general configuration data of almost any software that is a little more complex than a
"Hello World" application, and thus perfectly solves the problem of prioritizing different
external data sources, this camp may be very well suited for other use cases.

Generally spoken, this camp offers named run-time variables, which in contrast to using simple
flat hash tables, are organized hierarchically and furthermore support different levels of
write-access rights.<br>
With that, many structured data models (e.g., all that is represented in widespread JSon or XML files)
can be easily implemented using this library.

For example, a CAD software could store all model data, along with "materials", "colors", etc.
in a configuration tree. From a performance perspective, repeated read-access to such data
can be implemented in a way that is just as performant as accessing a simple C++ pointer.
This is due to the fact that class \b Variable is a simple pair of pointers and when accessing
its value, only the second pointer is even used. Furthermore, values of variables can be
stored by software and as explained in chapter \ref alib_variables_multithreading, it is ensured
that these pointers remain valid, even if parallel insertions or deletions of other variables
occur.<br>
Next, by accessing the underlying \b StringTree cursor (with method \alib{variables;Variable::AsCursor})
the library allows traversing and inspect the data model without "knowing" about which variable
(names) are existing.

This is all that should be said in this final chaper. An experienced software engineer can
easily judge about the fields of application of the \alibcamp_nl.


<br><br><br><br><br><br> */
