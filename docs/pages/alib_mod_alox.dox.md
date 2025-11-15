// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_alox ALib Module ALox - Programmer's Manual

\tableofcontents

\attention
    This Programmer's Manual grew to <b>roughly 70 pages</b>.
    The good news is: \alox <b>is simple</b> - it only <b>can</b> get complex, when you want it
    all of it.
\attention
    <b>Therefore, for new users, please check out the</b><br>
    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\ref alib_alox_tut "!!! ALox TUTORIAL !!!"<br>
    <b>instead of reading here!</b>


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 1 About ALox # {#alib_mod_alox_intro}

\alox is a module of the general purpose C++ library \aliblong. Nowadays it is fully integrated
in \alib, and has the status of what we call an \alibcamp. Historically, since 2013, \alox was developed
in parallel for programming languages C# and Java likewise. While the JAVA and C# versions
are not maintained anymore since 2018, they are still available and found with this link:
[ALox for C# and Java](http://alexworx.github.io/ALox-Logging-Library/).
The C++ version is and will be continuously maintained.

Within \alib, module (or "Camp") \alox provides an ecosystem to support structured and well
organized log output. The library attempts to make <em>debug logging</em> as easy and lightweight
as possible. One of the goals we want to achieve is that developers, once using the library, just
stop to put any temporary debug output statements into their code, but do it all with nice, readable,
configurable, reusable logging statements that can be temporarily or long-term switched off and
that are furthermore automatically <em>pruned</em> out of release versions of the application.

On the other end of the list, we want to enable __release logging__ for applications in production
systems (release versions) to collect mission critical metrics and data from *the field*.
Basic support for release logging is here today already. The introduction of new features and concepts
in the area of release logging will also support debug logging aspects of \alox. Therefore , the natural path
of evolution of \alox is <em>"debug features first, release features next"</em>.

The rather simple class structure and architecture of the library allows users to easily extend \alox.
If you extended \alox by new \e Loggers or features, you are welcome to contribute your innovations
to this open source project.

\attention
  While the \ref #alib::lox "ALox class hierarchy" looks quite simple and while you might expect
  a very simple manual, because all you wanted is <em>"just logging"</em>, be a little warned:<br>
  The concepts of \alox, when investigated deeper, are maybe more complex than what you would
  expect from a typical logging library. Due to the "orthogonality" of the features, which means you
  can just omit the complex stuff and don't even notice that it exists, \alox can be used in a very
  simple fashion though. Therefore, when you are searching for a simple, quick logging library, you
  have found it. In this case, instead of reading this manual, use the \ref alib_alox_tut "tutorial"
  and just get started. <b>Read this manual only when you want to understand \alox in full!</b>

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 2 Features and Limitations # {#alib_mod_alox_features_and_limitations}

- <b>Free software</b>, published under Boost Software License.
- \ref alib_alox_tut "Jump-start Tutorial" available.
- Formatted, **colorful, configurable log output** with support for ANSI compatible consoles and Windows OS consoles.
- Log lines <b>'clickable' in IDE</b> to jump to the corresponding source code line (depending on IDE).
- Automatic collection of **meta-information** like time stamp, time-difference to previous log,
  executing thread or executing source scope information.
- Automatic removal, aka <b>'pruning'</b>, of *debug-log* statements from release code mostly without
  the need of pre-processor <em>\#if/\#endif</em> code cluttering.
- Minimal intrusiveness in respect to impact on your code and what you need to make it really work.
- Use of external configuration (INI-files, command line parameters, environment variables, etc.)
  which allows developer-specific local settings and greatly supports working in teams.
- **Hierarchical 'Log-Domains'**, (like, for example, "MYAPP/UI", "MYAPP/DATABASE" that allow the
  classification of different <em>Log Statements</em> into user-defined topics. Verbosity is
  controlled per domain to allow the programmer to concentrate on the \b current area of interest.
- Defaulting 'Log-Domains' for **source scopes**, like *file*, *class* or *method*, which allows
  to omit the log-domain parameter from the <em>Log Statements</em> (again: leads to short, clean
  and readable <em>Log Statements</em>).
- Defaulting 'Log-Domains' for **thread scopes**, which allows controlling the verbosity of the log
  output on a per thread basis.
- Very **fast** and minimal runtime overhead.
- Multiple **parallel log streams** with a different \e Verbosity, e.g., for logging to the console
  and in parallel into a file.
- **Extensible** with own loggers, which enables dedicated "data drains" (e.g., databases or
  Internet services) and custom output formatting. \alox provides a simple programming interface
  and internal architecture that is easy to learn.
- Logging of <b>arbitrary objects</b> which means the whole \alox architecture builds on logging '
  objects' instead of just string messages. Only the logger instances which are attached at
  runtime, decide how logged data might be converted to human-readable strings.
- Scope dependent **Log Data** to add debug variables that automatically get pruned like the
  debug-logging statements are.
- **Conditional logging** (similar to assertions).
- **Log Once** to restrict a log message or a set of log messages to appear only once
  (or up to n-times).
- Support for **Recursive logging**, which is the correct execution of log statements during
  the evaluation of parameters of another log statement.
- Mapping of thread IDs to **readable thread names**
- Extensible to log custom types in custom way.
- \alox is in a stable state since years and **continuously maintained**. You're invited to contribute!

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 3 Terminology and Key Concepts # {#alib_mod_alox_terminology}

\I{################################################################################################}
## 3.1. Terminology ## {#alib_mod_alox_tak_terminology}

\I{################################################################################################}
### 3.1.1 Loggers ### {#alib_mod_alox_tak_terminology_logger}

\alox is designed to support different log streams. A log stream is a destination of log messages
and can be the IDE console, the terminal console, a file, a web service, or anything that accepts
data streams. Abstract class \alib{lox;detail::Logger} represents such a log stream.
While some implementations of class \b %Logger are provided with the \alox package,
the system allows custom implementations of \e Loggers that log information to any 'data drain' in
any format you like.

When using \alox, there is not much interaction with classes of type \e %Logger, though. The
reason for this is, that the data that is logged, can be directed into multiple
\e Loggers at once. Therefore, the API interface for executing the logging itself, is residing somewhere
else and internally, \alox dispatches the log data to the list of \e Loggers in question.

The interaction with class \b %Logger (and its derived types) is therefore limited to the
following:
- Creation of a \e %Logger
- Optional configuration of a \e %Logger (e.g., details on the output format, etc.)
- Registration of the \e %Logger with \alox.

### 3.1.2. Logables ## {#alib_mod_alox_tak_terminology_logables}
What can be described as "the data that is logged", is called a \b 'Logable' or, as it may be more
than one, <em>the Logables</em> in \alox. While most logging APIs are restricted to logging
text messages, \alox is not. Still, the usual case is logging text messages, but the architecture
of \alox allows logging an arbitrary amount of objects of arbitrary type.

Because C++ is a very strictly typed language, a lot of prerequisites need to be in place
before it is possible to (safely) drop any sort of object into a logging interface.
These prerequisites are created in other \alibmods, that \alox builds upon. The most important
of them are:
- \alib_boxing,
- \alib_strings, and
- the \ref alib_format_intro "formatting tools" found with module \alib_format.

In short, \alib_boxing_nl is <c>"std::any on steroids"</c>, allowing to pass any sort of object
and "box" it together with runtime type information. Interested users might consult the
Programmer's Manuals, tutorials, and reference documentation of the \alibmods linked above.
For now it is enough to know: \alox accepts any sort of object to be dropped as <em>Logables</em>
into the logging system and internally can access the data in a type safe way, just as it is
true with higher-level programming languages like Java or Python. And, besides the ability of getting
type information from the "boxes" and to "unbox" their values, \alib_boxing_nl allows defining and
invoke some sort of "virtual methods" on boxed values (this is why this concept is much more than
what <c>std::any</c> provides.

In the case of textual logging (the standard case, including 'simple' debug logging), all \e Logables
are processed by instances of derived types of class \alib{format;Formatter}.
This is why \alox logging optionally allows passing format strings which include placeholders
and formatting options, like field width, alignment, tabulators, quotes, and much more.
If no format string is detected, string representations of the \e Logables will be just
concatenated and logged. This way, formatting the output is just an option.

And the flexibility goes even beyond that: By default, \b two built-in incarnations of this class
are used \alib{format;FormatterPythonStyle} and \alib{format;FormatterJavaStyle}.
The two types are used in parallel. This means, that the already optional and auto-detected format
strings may follow either the
[Python String Fromat Syntax](https://docs.python.org/3.5/library/string.html#format-string-syntax),
which is also similar to what C# offers, or the
[formatting standards of the Java language](http://docs.oracle.com/javase/8/docs/api/java/util/Formatter.html),
which in turn are a little like good old \c printf, but of course much more powerful and also type-safe.

It can be concluded that \alox logs just anything a user passes to it as \e Logables:
Textual or binary data and if textual, optionally accepting auto-detected formatting strings of
different syntactical type!


\I{################################################################################################}
### 3.1.3. Log Statements ## {#alib_mod_alox_tak_terminology_statements}

When this manual and the reference documentation of \alox are referring to a <b>'Log Statement'</b>,
this means a piece of (user) code that invokes an \alox interface method that potentially performs
log output.

Of course, each <em>Log Statement</em> incorporates the provision of one or more \e Logables to \alox.
Hence, those interface methods of the \alox API that carry a parameter named \p{logable} (or a list
of those, named \p{logables} can easily be identified as methods that comprise <em>Log Statement</em>.


\I{################################################################################################}
### 3.1.4. Log Domains ## {#alib_mod_alox_tak_terminology_domains}
The term <b>'Log Domain'</b> in \alox denotes a sort of 'key' that is used to group
<em>Log Statements</em> into different sets. Each <em>Log Statement</em> belongs to exactly one
specific set of <em>Log Statements</em> by having the same <em>Log Domain</em> associated.

Such association can be made by using an \b optional parameter named \p{domain} which
is available in each interface method of the \alox API which comprises a <em>Log Statement</em>.

\note
  To be precise: Most \alox <em>Log Statements</em> do not have an explicit \p{domain} parameter. Instead
  \alox uses a mechanism to auto-detect <em>Log Domains</em> at the first position in the list
  of \e Logables of the statement.

A typical sample for a <em>Log Domain</em> might using name \c "UI" with all <em>Log Statements</em>
that concern the user interface of an application. A developer may switch <em>Log Statements</em> of
<em>Log Domain</em> \c "UI" completely off at the moment he is not interested in UI development.
Alternatively, warning and error messages might get allowed. Later, when working on UI related
code again, that <em>Log Domain</em> may be re-enabled.
If a team of developers is working on a project, each team member may configure \alox to enable
those <em>Log Domains</em> that he is currently working on.

Although parameter \p{domain} is optional, still each <em>Log Statement</em> is associated with a <em>Log Domain</em>.
The way how \alox 'magically' associates <em>Log Statements</em> with 'the right' <em>Log Domain</em>,
without the need for the user to be explicit about it, is one of the features that makes \alox quite unique.
More about this will be explained in later chapters of this manual.


### 3.1.5. Verbosity# {#alib_mod_alox_terminology_verbosity}

It is very common for logging eco-systems, to implement the concept of 'verbosity levels',
and \alox is no exception to this. First of all, the term <b>%Verbosity</b> denotes an attribute
of <em>Log Statements</em>. The values of this attribute are defined in enum class
\alib{lox;Verbosity}.

Some of the interface methods that comprise <em>Log Statements</em> carry the \e %Verbosity in their
method name, e.g., \c %warning() or \c %verbose(). Other, specialized methods offer a parameter
named \p{verbosity} of type \b %Verbosity.

Besides <em>Log Statements</em>, \e Verbosity is also an attribute of <em>Log Domains</em>.
As every <em>Log Statement</em> has a <em>Log Domain</em> and a \e Verbosity
associated with it, \alox is able to match both values against each other to decide whether a
<em>Log Statement</em> is executed or not.

When you think about this for a second, what is gained by this concept becomes obvious: The
overall verbosity of the log output is not controlled globally but on a 'per <em>Log Domain</em> basis'.
This allows switching the topic that an application is logging about. When interested in
user interface things, a <em>Log Domain</em> called \c "UI" might be set to a 'more verbose' level, while
other <em>Log Domains</em> are switched off or are reduced to real error messages.

As described later in this manual, the possibilities of \alox to filter the log output
are even more granular and flexible.

### 3.1.6. Scopes ## {#alib_mod_alox_terminology_scopes}
The \b '%Scope' is a next attribute any <em>Log Statement</em> possesses. The different types of scopes are nested into
each other and are a little similar to what C++ names a "scope". For example, \e %Scope::Method is similar
to a local variable's scope from a C++ perspective: it is visible only in method it is declared.
But still, in \alox, \e Scopes are different and the main reason is that \alox is not able to detect scopes in the
same way as a compiler of a programming language is. Therefore, we are talking about the following
<b>language related scopes</b>:

- \alib{lox;Scope;Scope::Path}<br>
  The file path that a source file which contains the <em>Log Statement</em> in question is located
  in. As this can be also parent directories, this \e Scope type represents in fact on its own
  already a nested set of scopes!

- \alib{lox;Scope;Scope::Filename}<br>
  The name of the source file in which a <em>Log Statement</em> is located in.

- \alib{lox;Scope;Scope::Method}<br>
  The name of a classes' method (or of a function) that a <em>Log Statement</em> is located in.


Besides these <em>language-related Scopes</em>, \alox in addition defines <em>thread-related Scopes</em>.
Those are relating to the thread that is executing a <em>Log Statement</em>. There are two of them, one defined as
an 'outer' \e Scope of the language-related set of \e Scopes, the other is an 'inner' \e Scope
of those.

All information about this topic is found in chapter
\ref alib_mod_alox_scopes. The corresponding enum type in the reference documentation is \alib{lox;Scope}.

### 3.1.7. Scope Domains ## {#alib_mod_alox_terminology_scopedomains}
Attention: now it becomes tricky! Each <em>Log Statement</em> belongs to
a \e Scope, precisely, even to a set of nested \e Scopes. The grouping of the statements
into \e Scopes is done automatically by \alox. The \e Scope is detected from the position
of the <em>Log Statement</em> within the source code and from the thread that is executing it.
Now, a <b>Scope Domain</b> is a default domain set by the user of \alox for a specific \e %Scope.
Once such default value is set, \alox uses this domain for each <em>Log Statement</em> located within that \e %Scope.
This way, <em>Log Statements</em> get their <em>Log Domain</em> associated 'automatically'.

As a sample, a <em>Scope Domain</em> \c "DB" could be set as the default <em>Log Domain</em> for \e %Scope::Filename
that contains code that stores and retrieves data from a database management system.
Now, all <em>Log Statements</em> within this source file get this <em>Log Domain</em> automatically associated, without
explicitly specifying \c "DB" with every <em>Log Statement</em>.
Therefore - although each <em>Log Statement</em> needs to refer to a <em>Log Domain</em> - such domain is not needed to be
added to each statement into the source code. This has several advantages: less typing, less
code clutter by <em>Log Statements</em>, copied <em>Log Statements</em> magically switch their domain when
'pasted' into a different scope, and so forth.

As you see, there are two ways to assign a <em>Domain</em> to a <em>Log Statement</em>: Either
by providing optional parameter \p{domain} with a <em>Log Statement</em>, or by setting a <em>Scope Domain</em>
and omitting the parameter.

### 3.1.8. Tree of Log Domains and Domain Path ## {#alib_mod_alox_terminology_logdomaintree}
By having <em>Scope Domains</em> which associate a "default domain" with a <em>Log Statement</em> that resides in a certain \e Scope
and knowing that the \e Scopes are nested into each other, the question is what happens if multiple
<em>Scope Domains</em> apply to the same <em>Log Statement</em>? Or, a similar question: what happens if a <em>Scope Domain</em> is set for a \e Scope that a
<em>Log Statement</em> resides in and in addition, the <em>Log Statement</em> uses optional parameter \p{domain} to explicitly specify
a <em>Log Domain</em>?

The answer is: \alox concatenates all <em>Scope Domain</em> to a <b>Domain Path</b>, separated by character <c> '/'</c>.
This means that \alox organizes <em>Log Domains</em> hierarchically, hence this can be seen as a <b>tree of Log Domains</b>.
The concatenation starts from the most 'outer' \e Scope and ends at the most 'inner'. The value of optional
parameter \p{domain} is appended close to the end - but not completely at the end.

Besides 'mixing' <em>Scope Domains</em> and parameter \p{domain}, \alox also allows 'overriding'
<em>Scope Domains</em> with parameter \p{domain}.

Using the techniques in the right manner, is one of the keys to efficiently use \alox.
The details of how this is done is explained in a dedicated chapter: \ref alib_mod_alox_logdomains.


\I{################################################################################################}
## 3.2. Class Lox - Managing it all# {#alib_mod_alox_tak_lox}

The definitions and terminology that were introduced in this chapter so far should be
quickly summarized. We have:

- <b>Loggers and Logables:</b><br>
  \e Loggers are responsible for writing the contents of \e Logables to dedicated output 'drains' like the
  console, a text file, or a remote server. Multiple \e Loggers might exist even in quite simple
  applications.
- <b>Log Statements and associated attributes:</b><br>
  A <em>Log Statement</em> is the user code that invokes the interface API of \alox and pass a \e Logable, e.g., a
  text message to \alox.
  Each <em>Log Statement</em> has three attributes besides the \e Logable:
  1. A \e Verbosity, defining the 'importance' or 'severeness' of the statement.
  2. A <em>Log Domain</em> that makes the <em>Log Statement</em> belong to a certain set of <em>Log Statements</em>. <em>Log Domains</em> can be considered to specify
     the 'topic' that a <em>Log Statement</em> is 'talking' about.
  3. A \e Scope, which gives a different, automatic way of grouping <em>Log Statements</em>.
- <b>Scope Domains:</b>
  Those are acting as 'default domains' and are collected and concatenated by \alox to form,
  together with parameter \p{domain} of a <em>Log Statement</em>, a 'domain path' identifying the resulting, final
  <em>Log Domain</em> of a <em>Log Statement</em>.

Now, the most prominent class of \alox which acts almost like a "single point of contact" comes
into the game: Class \alib{lox;Lox}.

This class keeps it all together! This is what the class \b %Lox does:

- It provides the most important parts of the \alox API, especially those interface methods that
  comprise <em>Log Statements</em>.
- It manages a set of <em>Loggers</em> which write the \e Logables of <em>Log Statements</em>.
- It maintains a tree of hierarchically organized <em>Log Domains</em>.
- It stores a \e Verbosity value for <em>Log Domains</em>, which is the counter-value of a
  <em>Log Statements</em>' \e Verbosity and determines if a <em>Log Statement</em> is executed or not.
  This is done on a per-\e Logger basis.
- It automatically determines the \e Scope of a <em>Log Statement</em> and manages the associated
  nested <em>Scope Domains</em>.
- It provides other nice features related to \e Scopes, like \ref alib_mod_alox_log_once "Lox::Once",
  \ref alib_mod_alox_prefix_logables "Prefix Logables" or \ref alib_mod_alox_log_data "associated Log Data".
- It collects some meta-information like timestamps or counters.
- It provides a dictionary to translate thread IDs in human-readable (logable) thread names.

It becomes clear that this class is an \alox users' main interface into logging. After \alox was
set-up once (probably in the bootstrap section of software), and <em>Loggers</em> are created,
configured, and attached to a \b %Lox, this class is almost all that is needed across all other parts of
software. All main \alox functionality, especially the interface for the logging statements themselves
is comprised in this class.

One important detail of the internal management of class \b Lox is the fact that it
associates a separated <em>Verbosity</em> value for each combination of <em>Log Domain</em> and
<em>Logger</em>.<br>
The rationale behind that is easy to understand: An application that supports different \e Loggers
at once (which happens quite quickly when using \alox), might want to log different subsets of the
log messages with a different \e Verbosity to each of theses \e Loggers. For example, a \e Logger
dedicated for debug-logging that streams into an output window of an IDE, would be desired to
be generally more verbose and also switch \e Verbosity more frequently, than a
<em>Logger</em> that in parallel logs into a file which is storing also logs of earlier debug sessions.

### 3.2.1 Prefix Logables ## {#alib_mod_alox_tak_lox_prefixlogables}
While those interface methods in class \alib{lox;Lox} which comprise a <em>Log Statement</em> already accept an
arbitrary amount of \e Logables as parameters, this list can even be extended by further objects
which are then all together passed to method \alib{lox::detail;Logger::Log} of each \e Logger attached.

Such additional objects are called <b>Prefix Logables</b>.
In short, \alox allows associating \e Logables to \e Scopes. This way, all <em>Log Statements</em> 'collect'
such <em>Prefix Logables</em> which were defined to any of the nested \e Scopes in a list and passes
them to the \e %Logger. We could have named them <em>Scope Logables</em> or <em>Context Logables</em>,
however, the word 'prefix' makes perfect sense with the most important type of \e Logables, namely strings!
With logging text messages, <em>Prefix Logables</em> act like a prefix to a log message.
All about this topic is found in chapter \ref alib_mod_alox_prefix_logables.

## 2.2 Log Once ## {#alib_mod_alox_tak_lox_logonce}
Another feature of \alox which leverages the concept of \e Scopes, is found with overloaded methods
\alib{lox;Lox::Once}.

They are really 'heavily' overloaded, therefore the most simple version just accepts a \e Logable.
With this version \alox hides the use of \e Scopes and offers what you would expect from the methods'
name: logging a statement only the first time it is executed. The different parameters allow to
cover more complex uses cases than this. All about this <em>Log Statement</em> is found in chapter
\ref alib_mod_alox_log_once.

## 2.2 Log Data ## {#alib_mod_alox_tak_lox_logdata}
As being able to 'prune' \alox debug-<em>Log Statements</em> from release code, a low hanging fruit for the \alox
feature list is to offer a concept for storing and using data, that otherwise would be
temporary debug variables during the development process. Again, \alox \e Scopes are leveraged
which makes the otherwise simple methods \alib{lox;Lox::Store} and \alib{lox;Lox::Retrieve} quite powerful.

All about <em>Log Data</em> is found in chapter \ref alib_mod_alox_log_data.

\I{################################################################################################}
## 3.3. Using Multiple Lox Instances # {#alib_mod_alox_tak_multiple_loxes}

While \alox claims to be lean and easy, besides these concepts explained in this chapter, it was
decided that a next level of complexity is supported. The good news is: for simple use case
scenarios, you do not need to know about that.

So, this new 'level of complexity' is simply introduced by the fact that it is possible, and sometimes
very attractive, to create and use more than one instance of class \b Lox in software.
Each class is populated with \e Loggers and of course uses an own dedicated tree of domains.

The following paragraphs gives use cases and further background on using multiple \e Loxes.

### 3.3.1 A Dedicated Lox Singleton for Debug Logging ## {#alib_mod_alox_tak_multiple_loxes_dbg_rel_log}

There are two fundamental logging scenarios that we call
\ref alib_alox_faq_debug_vs_release_logging "debug logging and release logging".

For various reasons (performance, code size, security, etc), debug <em>Log Statements</em> should be disabled
and removed (pruned) from the release version of software.

To achieve all goals and provide a very simple interface into debug logging, the \alox ecosystem
provides a specific purely static class \alib{lox;Log} which provides access to one instance of
class \alib{lox;Lox} and to a few other static objects.

\dot
digraph example
{
    node [shape=record, fontname=Courier,   fontsize=12];
    edge [shape=record, fontname=Helvetica, fontsize=10];
    log  [ label="{Log|Holds \"the\"\n debug-Lox}"   URL="\ref alib::lox::Log"];
    lox  [ label="Lox"                               URL="\ref alib::lox::Lox"];
    logrs[ label="Loggers"                           URL="\ref alib::lox::detail::Logger"];
    doms [ label="Domains" ];
    cfg  [ label="Configuration Data"];
    log -> lox  [ label= "Contains 1 singleton" arrowhead="open", style="solid" ];
    lox -> logrs[ label= "manages"              arrowhead="open",               ];
    lox -> doms [ label= "manages"              arrowhead="open",               ];
    lox -> cfg  [ label= "manages"              arrowhead="open",               ];
}
\enddot

The assumption (restriction) that is taken here, is that debug logging is implemented by using
one dedicated \b Lox. This should be sufficient for most scenarios, because, as described above, within that Lox
instance various \e Loggers with own <em>Log Domain</em> settings will provide a lot of flexibility to log different
types of messages into different streams and manipulate the verbosity for
each stream accordingly.

To achieve efficient and complete pruning, \alox relies on a set of preprocessor macros.
These macros are doubly defined:
- One set starting with prefix <b>Log_</b>, and
- one set starting with prefix <b>Lox_</b>.

The first set performs <em>debug logging</em>, hence addresses the objects of static class
\alib{lox;Log} (through preprocessor symbol \ref LOG_LOX), while the second set performs
<em>release logging</em>, operating on the \alib{lox;Lox} (through the preprocessor symbol
\ref LOX_LOX).




### 3.3.2 Separating Mission Critical Log statements ## {#alib_mod_alox_tak_multiple_loxes_missioncritical}

Another motivation for using separated instances of class \b Lox may be understood when thinking
about use cases where things start getting critical from different point of views. For example:

- \alox is supposed to collect runtime data from <em>the field</em>, hence metrics, which are
  transferred using a tailored \e %Logger that contacts a metrics server at runtime. The team
  that implements such metrics collection, may, for good reason, not want to share 'their' \b Lox
  with other parts of software maintained by another team. Accidental mis-configuration of the
  Lox and its domain may lead to uncontrolled server communication.
- A Lox is supposed to collect critical runtime errors from deployed software. Such log information
  should be separated from other log streams, e.g., much more 'verbose' standard release-logging
  that goes to a rolling log file
- software wants to support writing messages to the \e Linux or <em> Windows OS</em> specific
  system journal. Also in this case, a mis-configured Lox might 'spam' into such system journals
  in an inappropriate manner, and it would be advised to use a separated Lox that is not touched
  outside its scope of activity.

### 3.3.3 Multiple Registration of a Logger ## {#alib_mod_alox_tak_multiple_logger_registration}

It is explicitly allowed to attach a \e %Logger to more than one \b Lox object.
class \b %Logger implements a mechanism to protect
against race conditions in multithreaded environments as soon as such double-registration occurs.
The obvious use case is again the mission-critical, separated \b Lox described in the previous
paragraphs. A \e %Logger that is responsible for 'standard' release logging, e.g., logging into a rolling
release log file, can be attached to the 'mission-critical' \b Lox which holds the corresponding
'mission-critical' \e %Logger. Now, the standard release log automatically receives a copy of the
'mission-critical' <em>Log Statements</em>.

## 3.4. Class ALox and Registration of Lox Instances # {#alib_mod_alox_tak_lox_registration}
The module keeps a list of all instances of class \b %Lox that were registered with it.
Registration is done by default when constructing a \b Lox.
The advantage of that registration is that a \b %Lox can be retrieved by its name using the static
method \alib{lox;Lox::Get}. This is convenient because this way, references of a \b %Lox do not have
to be passed around or otherwise made public, which would clutter your own software's codebase,
especially your header-files.

But there might also be situations, when this <em>'public availability'</em> of a \b %Lox
instance is not wanted. For this case, optional parameter \p{doRegister} may be set to \c false
when invoking constructor \alib{lox;Lox::Lox}.

\note
  Descriptions of other \e 'protection' mechanisms against unwanted manipulation of
  class \b %Lox are described in:
  - Chapter \ref alib_mod_alox_external_verbosity_configuration and
  - Chapter \ref alib_mod_alox_domain_substitution_config.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 4 Log Domains # {#alib_mod_alox_logdomains}
This chapter provides all details on <em>Log Domains</em>. Before reading it, be sure that
you have read and understood the chapter \ref alib_mod_alox_terminology.


\I{################################################################################################}
## 4.1 Why Log Domains? # {#alib_mod_alox_logdomains_intro}

Let us quickly recapture what was explained already on <em>Log Domains</em> in the previous chapter
and the tutorials.
It is very common to logging libraries to provide a system that allows controlling the "verboseness"
of the log output, by matching what \alox calls a \e Verbosity associated to <em>Log Statements</em>.
\alox allows dividing the overall set of <em>Log Statements</em> into subsets that belong to different
topics or 'areas of interest'. These topics are called <em>Log Domains</em> and each
<em>Log Statement</em> has one associated.
Furthermore, \alox allows having multiple different \e Loggers performing each <em>Log Statement</em>
in parallel.
Now, \alox stores the \e Verbosity per combination of <em>Log Domain</em> and \e Logger.

The result is, that the \e Verbosity can be different not only in respect to each <em>Log Domain</em>
but also, the same <em>Log Domain</em> may have a different \e Verbosity set for different \e Loggers.

The method to set such \e Verbosity which is found (in different, overloaded versions)
in class \b Lox is simply called \b SetVerbosity(). As you see in the reference documentation of
\alib{lox;Lox::SetVerbosity}, this method as a minimum requires a value for the \e Logger that is
affected and the \alib{lox;Verbosity} that is to be set. The third parameter,
the <em>Log Domain</em> defaults to "/" if not given. Omitting parameter \p{domain} with this method
tells \alox to set \e all <em>Log Domains</em> to the same \b Verbosity.

\note The fourth parameter of method \b SetVerbosity found in the reference documentation is not
      covered in this chapter. The impatient reader might refer to \ref alib_mod_alox_external_verbosity_configuration.

This probably sounds more complex than it is. If you think about it for a second, it should become obvious.
As a sample, this mechanism allows logging warning and error messages of an application
of just any <em>Log Domain</em> to a file to have that log available across different debug-sessions, while
the IDEs' console output is much more verbose in respect to those <em>Log Domains</em> a programmer
currently is interested in.<br>
Or, as another example, we could consider a \e Logger that sends alert Emails to system administrators.
Of course, Emails should be sent only for very special events, e.g., when something really severe happens.
You may collect these severe errors in a special domain "/ADMIN" (real-time administration errors)
and exclusively activate this domain for this Email-sending \e Logger.

\I{################################################################################################}
### 4.1.1 Allowed Characters ## {#alib_mod_alox_logdomains_chars_allowed}

Domain names may only consist of the following characters:
- Upper case letters \c 'A' to \c 'Z',
- Numbers \c '0' to \c '9', and
- Hyphen (\c '-') and underscore (\c '_').

If one of this characters is used, \alox replaces this character with \c # (which, as stated above
is otherwise equally not allowed) without further notice.
(Also, no \ref alib_mod_alox_internal_domains "internal log statement" is given in this case).
As a result, the explicit provision of illegal domain names \c "ABCa$)" or \c "ABC<=>" will each
result in the same domain name \c "ABC###".


\I{################################################################################################}
## 4.2 Hierarchical Log Domains # {#alib_mod_alox_logdomains_hierarchical_domains}

One of the major design goals of \alox is simplicity. By nature simplicity is sometimes in
competition with flexibility or feature richness of an API. One way to resolve such conflicting
goals is to provide features that - if not wanted or needed or not even known - are invisible to
the user. The concept of hierarchical <em>Log Domains</em> is a good sample of how this was achieved in the
\alox API design.

Consider an application with a user interface. Within the code that implements
the UI, a lot of <em>Log Statements</em> using different \e Verbosities may be embedded using
<em>Log Domain</em> 'UI'.
Depending on the complexity of an application, switching domain 'UI' to a more verbose level
(by setting the <em>Verbosity</em> of <em>Log Domain</em> 'UI'), might already lead to a huge
amount of <em>Log Statements</em>. Of course, the solution is to split that domain into several ones,
e.g., one for UI dialogs, one for manipulations of the main window, one for menu events and one
for mouse interactions, etc. Therefore, splitting domains in subtopics is a common use case
as soon as code becomes more complex.
To support this use case, \alox organizes <em>Log Domains</em> hierarchically. The sample
would lead to:


\dot
digraph logdomains
{
    node [shape=record, fontname=Courier,   fontsize=12];
    edge [shape=record, fontname=Helvetica, fontsize=10];
    ui [ label="UI"];
    dialogs[ label="DIALOGS"];
    window [ label="WINDOW"];
    menu   [ label="MENU"];
    mouse  [ label="MOUSE"];
    ui -> dialogs   [ arrowhead="open" label= "is parent of" ];
    ui -> window    [ arrowhead="open"];
    ui -> menu      [ arrowhead="open"];
    ui -> mouse     [ arrowhead="open"];
}
\enddot


With every method in \alox that accepts a <em>Log Domain</em> as a parameter, instead of just one
single domain name, a <b>domain path</b> can be used.
Domains in a path are separated by a slash \c '/'. As soon as domain parameters contain path separators,
the hierarchical domain system of \alox takes action.<br>
The <em>Log Domains'</em> paths in our sample would be named:
- '/UI'
- '/UI/DIALOGS'
- '/UI/WINDOW'
- '/UI/MENU'
- '/UI/MOUSE'

Setting the \e Verbosity of a <em>Log Domain</em> is always done recursively for all its Subdomains
(and sub-Subdomains). Let us look at a sample, how an \alox user would setup the
domain tree, when debugging.

First, the setting should allow only error messages "in general", which means over all domains.
Hence we provide the root domain <c>'/'</c>, which is anyhow the default value for
parameter \p{domain}:

\snippet "ut_alox_dox.cpp"        Man_DOMAINS___1

As the interest is currently in domain '/UI/DIALOGS', we first switch '/UI' to a <b>Verbosity</b> that
logs \e error, \e warning and \e info statements and then switch subdomain '/UI/DIALOGS/' to the most
verbose level:

\snippet "ut_alox_dox.cpp"        Man_DOMAINS___2

As such setting is always recursive, the order of setting domains is important. If the statements
above were performed in reverse order:

\snippet "ut_alox_dox.cpp"        Man_DOMAINS___2reverse

the setting for domain '/UI/DIALOGS' to \e All would be overwritten by the setting of parent domain '/UI'.

\note
  In some situations, it might be wanted to be able to protect a domain setting and make
  it 'immune' against subsequent settings of parent domains. This is especially important when
  configuration is done 'outside' the source code, hence using configuration files, command line
  parameters, and such, but could be also helpful otherwise. This protection mechanism is explained
  in chapter \ref alib_mod_alox_external_verbosity_configuration.


The advantages of hierarchical domains so far are:
- You can control a whole set of domains with one statement
- If parts of software that is out of your control (other team, code in a library, etc.) introduces
  new <em>Sub-Log Domains</em>, no changes of \e Verbosity settings in your code parts need to be done (as long as
  you are not interested in directly manipulating this new subdomain).

But the real reason, why hierarchical domains drive the flexibility of \alox really to a next level,
becomes only obvious in the next chapter!


\I{################################################################################################}
## 4.3 Scope Domains # {#alib_mod_alox_logdomains_default_domains}

People might argue that providing a <em>Log Domain</em> to each <em>Log Statement</em> is an overhead of
typing. That is true. For this reason - and many more - \alox provides the concept
of *Scope Domains*.

<em>Scope Domains</em> are set using method
\doxlinkproblem{classalib_1_1lox_1_1Lox.html;ab6f2a9bee025cf4a0e6ff4b84ef2d1a5;Lox::SetDomain(const NString&, Scope)}.
This method, along with parameter \p{scopeDomain}, requires a parameter of enumeration type
\alib{lox;Scope} that determines the \b 'Scope' that the given domain path should default to.


Such \e Scope is assembled of two parts: one part that is taken from the source code information
that \alox collects at compile-time with each <em>Log Statement</em>. The other part
is related to <b>the thread that is executing the <em>Log Statement</em></b>, hence is evaluated at runtime.

\attention
  For this chapter, explaining the benefit of being able to assign <em>Scope Domains</em> to \e Scopes, we are
  simplifying \alox \e Scope features to a bare minimum.
  We are deferring the full explanation of possible \e Scope settings to chapter \ref alib_mod_alox_scopes.

We start really simple and look at \alib{lox;Scope;Scope::Method} which denotes that the <em>Scope Domain</em>
given with method
\doxlinkproblem{classalib_1_1lox_1_1Lox.html;ab6f2a9bee025cf4a0e6ff4b84ef2d1a5;Lox::SetDomain(const NString&, Scope)}
should apply to all <em>Log Statements</em> of the current method. Here is a sample:

\snippet "ut_alox_dox.cpp"        Man_DOMAINS___3


This sets the domain "/MYDOM" as the <em>Scope Domain</em> for this method. As a result, a
<em>Log Statement</em> that does not provide a domain uses the <em>Scope Domain</em>. The output
could be:

\verbinclude    pages/generated/ALoxMan_Domains_3.txt

We have already gained some advantages from this most simple use of a <em>Scope Domain</em>
1. Less typing (we can omit the <em>Log Domain</em>)
2. Less \e 'clutter' in the code, so better readability (<em>Log Statements</em> serve as comment lines)
3. When the <em>Log Domain</em> should be changed, this has to be done only at one point in the code
4. When code including <em>Log Statements</em> is copied and pasted into a different \e Scope, the
   <em>Log Domain</em> adjusts to the destination \e Scopes' <em>Scope Domain</em>.


\I{################################################################################################}
## 4.4 Absolute and Relative Domain Paths# {#alib_mod_alox_logdomains_absolut_and_relative}

As we have learned above, the following two <em>Log Statements</em> are the same when a <em>Scope Domain</em>
was set for \b %Scope::Method:

\snippet "ut_alox_dox.cpp"        Man_DOMAINS___4

But we have to be careful! The following two <em>Log Statements</em> are \b not using the same <em>Log Domain</em>:

\snippet "ut_alox_dox.cpp"        Man_DOMAINS___5

As can bee seen in the output:

\verbinclude   pages/generated/ALoxMan_Domains_5.txt

The difference of the sample is that while we previously always used a '/' at the beginning of
domains, now for parameter \p{domain} of the last <em>Log Statement</em> this was omitted. This caused \alox to concatenate
the domain provided in the <em>Log Statement</em> with the <em>Scope Domain</em> set, to a new domain path.

This mechanism does not need too much further explanation, as this is similar to file system
path usage: A <em>Log Domain</em> path starting with \b '/' is called an <b>absolute path</b> and one that omits the
\b '/' character is called a <b>relative path</b>.<br>

Same as with file system paths, a relative path can also be expressed with a single dot.
As a Sample, domain paths \b "DOM" and \b "./DOM" are the same. The use of a leading extra
'dot'-domain in the path to signal relative domains is optional and probably a matter of taste.<br>
More important is that \alox <em>Log Domains</em> also support two dots '<b>..</b>' in
relative path names for addressing a parent domain. Later in this chapter, a sample for
addressing a parent domain is shown.

While the previous sample code looked more like an erroneous use of a domain (resulting
in domain '/MYDOM/MYDOM'), the following sample shows a use case of a relative domain path which
absolutely makes sense:

\snippet "ut_alox_dox.cpp"        Man_DOMAINS___RelativePaths

The output would be:

\verbinclude   pages/generated/ALoxMan_Domains_RelativePaths.txt


\I{################################################################################################}
## 4.5 Inner and Outer Scopes# {#alib_mod_alox_logdomains_inner_outer_scopes}

The previous samples used <b>%Scope::Method</b>. Another important \e Scope 'level' is
\alib{lox;Scope;Scope::Filename}.

As mentioned before, we are not going into the details of the extended \e Scope definition of
\alox (those are given in \ref alib_mod_alox_scopes), but what is very important to understand is that
scopes are nested into each other. We talk about <em>outer scopes</em> and
<em>inner scopes</em>. As you might guess, <b>%Scope::Filename</b> is an outer \e Scope of
<b>%Scope::Method</b>.

Now, <em>Scope Domains</em> which can be set per \e Scope level, are concatenated to a complete
domain path. \alox starts from the 'most inner' \e Scope and then prepends (!) the paths of outer
\e Scopes as long as relative domain paths are given. In other words, as soon as
an absolute path is found as a <em>Scope Domain</em> for a scope, the further concatenation of outer
\e Scopes' <em>Scope Domains</em> is stopped.

We are using the same sample as above, but this time, the public interface method internally
is split into different parts:

\snippet "ut_alox_dox.cpp"        Man_DOMAINS___nested_scope

In the constructor fo the class, <em>Scope Domain</em> "IO" is set for the entire
class by using <b>%Scope::Filename</b>.
\note If more than one objects of type IO is created, the statement is repeatedly executed.
      This is no problem. However, it is always good to find a place for setting <b>%Scope::Filename</b>
      that is executed a) early and b) seldom. The reasons should be obvious.

When sample method \e ReadChangeAndWriteBack() is executed, this output is generated:

\verbinclude   pages/generated/ALoxMan_DOMAINS___nested_scope.txt

With the takeaways from the features explained already in this chapter, the code is quite self-explanatory.
Some remarks:
- As explained and observable in the output, relative domains are concatenated from inner scopes
  to outer scopes. Samples are the domains '/IO/READ', '/IO/PROCESS' and '/IO/WRITE'
- Like in the previous sample, method \e write() volunteers to use prefix "./" to signal that
  the domain path is relative. This is a matter of taste and the result is the same as if this
  prefix was omitted like in methods \e read() and \e process().
- Method \e checkSetup() uses an \e absolute domain path for its \e %Scope. This tells \alox to
  stop prepending any <em>Scope Domains</em> of outer \e Scopes.
- Method \e writeStats() uses a \e relative domain path\e starting addressing the "../". parent
  domain. The effect is that the outer \e Scope domain "IO" is removed. The effective path is:
  '/IO/../STATS' which \alox of course shortens to just '/STATS'.<br>
  Hence 'STATS' is a subdomain of the root-domain, the same as domain 'CHECKS' is. Note that, if
  there were other <em>Scope Domains</em> in \e Scopes that are 'more outer' than <b>%Scope::Filename</b>,
  the 'depth' of both domains would be different.

\I{################################################################################################}

## 4.6 Further Information and Wrap-Up # {#alib_mod_alox_logdomains_further_wrapup}

\I{################################################################################################}
### 4.6.1 Where to set the Verbosity## {#alib_mod_alox_logdomains_bootstrap}

The usual way to use \alox is to separate the code that sets the \e Verbosity of <em>Log Domains</em>
into a single location, normally somewhere in the 'boot-strapping part of a process. This is
probably the same location where the \e Loggers used and also \e Lox objects are created and
configured.

It is of course tempting to quickly (temporily) increase \e the Verbosity of the <em>Log Domain(s)</em>
that a code unit uses while working on that code unit. Method \alib{lox;Lox::SetVerbosity}
properly uses the current \e Scope, so that a statement like this:

\snippet "ut_alox_dox.cpp"        Man_DOMAINS___RelativePaths_Set

properly sets the evaluated <em>Scope Domain</em>. However, if such code lines are inserted,
they should be duly marked as 'temporary' and 'to-be-removed'.

Especially when working in a team, it is even more convenient to not even set the \b
Verbosity of any domain from within the code, but by using external configuration data.
\note
  To keep this chapter as simple as possible, all samples herein exclusively
  used the method \e %Lox::SetVerbosity and you may not even have heard yet about other possibilities.
  Information about how \alox is configured externally, is completely separated into an own chapter
  of this manual: \ref alib_mod_alox_external_verbosity_configuration.

The separation has good reasons: A portion of the code that logs using a certain set of domains
should not modify these <em>%Log %Domain's %Verbosity</em>. One day such code could be moved into a library and then
such changes are merely irreversible (well, in-fact this misbehavior can be 'healed', but this is also only explained in
chapter \ref alib_mod_alox_external_verbosity_configuration).

\I{################################################################################################}
### 4.6.2 Why Does Verbosity Setting Always Work Recursively? ## {#alib_mod_alox_logdomains_recursive}

It was explained in section \ref alib_mod_alox_logdomains_hierarchical_domains "Hierarchical Log Domains",
that setting the \e Verbosity is always done recursively for all Subdomains of the
given <em>Log Domain</em>. The reason for this is to avoid 'random' settings as much as possible.

For convenience, <em>Log Domains</em> do not need to be 'registered' with \alox. As soon as
an unknown <em>Log Domain</em> is used in a <em>Log Statement</em>, \alox creates a reference to this
<em>Log Domain</em> and for each \e Logger in the corresponding \e Lox the \e Verbosity of
the parent <em>Log Domain</em> is inherited.

Now, if a <em>Log Domain</em> setting was allowed to be non-recursive, the setting of a subdomain
could have different values:
- If it was known to \alox already (because it was used in a <em>Log Statement</em>), it would keep the
  \e Verbosity that it previously received by implicitly inheriting from its parent.
- If it was implicitly registered with \alox after such non-recursive setting, it would still
  inherit its parents setting.

Therefore, the order of the use of a <em>Log Domain</em> would decide about its \e Verbosity.

One of ALoxs' design goals is to hide features that are not used and which are unnecessarily
increasing complexity of using \alox if they are not needed (Further explained in \ref alib_mod_alox_apdx_auto_config).
For this reason, within this chapter of the manual (which is all about understand what domains
<em>Log Domain</em> just are), a parameter of method \alib{lox;Lox::SetVerbosity} that can be omitted
because it has a default value, was not even named.
Using this parameter allows restricting the setting of the \e Verbosity of to a domain and
allows including arbitrary sets of Subdomains or not, by assigning a priority to
each \e Verbosity setting.

Apart from lifting the always recursive \e Verbosity setting, the main motivation for introducing
this concept resulted from the general need to prioritize conflicting settings which may come
from different sources of configuration data. Therefore, if non-recursive settings are needed for any reason,
refer to chapter \ref alib_mod_alox_external_verbosity_configuration for explanations about how to achieve this.

\I{################################################################################################}
### 4.6.3 Absolute or Relative Domain Paths? ## {#alib_mod_alox_logdomains_absorrel}

As we learned, \alox allows using absolute or relative <em>Log Domain</em> paths. This is true for <em>Scope Domains</em> as well
as for parameter \p{domain} in a <em>Log Statement</em>.

The general recommendation is to use relative paths. There are several advantages which
all arise from the fact that \alox assembles the <em>Scope Domain</em> path by looping through all \e Scopes from
the most inner to the most outer. As soon as an absolute path is found, the loop is aborted and
concatenation stops. The benefits of keeping all <em>Log Domain</em> paths relative are highlighted in the
next chapter (\ref alib_mod_alox_scopes) when all about \e Scopes is explained - including an important use
case that justifies absolute <em>Log Domain</em> paths!

\I{################################################################################################}
### 4.6.4 ALox Internal Log Domains ## {#alib_mod_alox_logdomains_internal}

\alox uses 'itself' to log information about its use. This is helpful e.g., to observe where
and when a <em>Log Domain</em> is used for the first time or where and when a \e Verbosity
setting is performed.

To keep the 'tree of <em>Log Domains</em>' clean and reserved exclusively for application specific
<em>Log Domains</em> and to avoid accidental activation of internal log messages, \alox
manages the internal <em>Log Domains</em> in a separated tree.

Information about how to 'address' this tree and when to use it, is explained in chapter
\ref alib_mod_alox_internal_domains.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 5 Scopes in ALox # {#alib_mod_alox_scopes}
\I{################################################################################################}
## 5.1 Introduction # {#alib_mod_alox_scopes_intro}

\alox uses the concept of \e Scopes in different ways. The most important and prominent use is to set
so-called <em>Scope Domains</em>. Those can be registered and then are used as a 'default' domain
path with each <em>Log Statement</em> placed within the according \e %Scope. The concept of
<em>Scope Domains</em> has been explained already in the previous chapter (see \ref alib_mod_alox_logdomains). But to keep it simple,
the full variety of \alox \e Scopes was not yet, explained there. This is what this chapter
aims to do.

So, be sure, that you have read and understood chapter \ref alib_mod_alox_logdomains, before working yourself
through this chapter. Using the more complex possibilities of scopes is optional when using \alox, so you can
consider this chapter as an advanced topic.

Furthermore, there are other features in \alox that use \e Scopes. Those are elaborated in chapters
- \ref alib_mod_alox_log_once,
- \ref alib_mod_alox_prefix_logables and
- \ref alib_mod_alox_log_data.


\alox \e Scopes are enumerated in enum-class \alib{lox;Scope}. The \e Scopes found here are separated in two sets:

- <b>Scopes related to the programming language:</b><br>
  These scopes are identified 'automatically' at compile time with each invocation to a method of class \b Lox
  using the preprocessor macros.
- <b>Scopes related to the execution thread:</b><br>
  These scopes are identified at runtime, by examining the thread that is executing a method of class \b Lox.



\I{################################################################################################}
## 5.2 Language-Related Scopes # {#alib_mod_alox_scopes_language}

\e Scopes are \b nested into each other. We talk about 'outer' and 'inner' scopes.
For example, the \e Scope of a method is nested into the \e Scope of the file name that the method
is implemented in.

The language-related \e Scopes that \alox supports are (from outer to inner):

- \b %Scope::Global,
- \b %Scope::Path,
- \b %Scope::Filename, and
- \b %Scope::Method.

Apart from \e %Scope::Global, to evaluate the actual \e Scope of an invocation of a member of
class \b Lox, \alox needs to 'automatically' collect information of the calling entity.
In \alox for C++ is this is achieved using preprocessor macros for all <em>Log Statements</em>.

As explained in detail in chapter \ref alib_mod_alox_debug_and_release_logging, for release-logging, such
automatic collection is not wanted. Without repeating the reasons for this here, let us emphasize
the consequence:

\attention
    The \e Scopes.Path, \e %Scope::Filename (in Java Scope.CLASS) and \e %Scope::Method can be
    used for debug-logging only! While it is possible to set <em>Scope Domains</em> (and use other
    features of \alox which rely on \e Scopes) in release-logging, scope information values will be
    empty and hence not distinguishable from each other.


The good news is: This is absolutely OK! The rationale for claiming this is:
- In respect to <em>Scope Domains</em>:<br>
  Release-logging statements are quite rare compared to those of debug logging. Furthermore, they
  should be very well thought, clear, and meaningful. It is advisable to not use <em>Scope Domains</em>
  in release logging anyhow (apart from thread-related and Scope::Global as explained below).
  Instead, <em>Log Statements</em> that comprise release-logging should specify the <em>Log Domains</em> they refer
  to \b explicitly.
- In respect to <em>Log Data</em>:<br>
  Well, the whole concept of <em>Log Data</em> provided by \alox is merely a tool to support the process
  of debugging, e.g., to explore the location of an exception in the log output. It is not deemed
  to be used to implement any functionality of an application, for example, to store thread-local
  information.
- In respect to method \alib{lox;Lox::Once}:<br>
  For release logging, the optional parameter \p{group} certainly does the job. It should be used
  because, this makes release logging statements to be more explicit and readable.
- In respect to <em>Prefix Logables</em>:<br>
  The most important use case for them are to make the output better readable, e.g., by adding
  recursive indentation. Again, something that should not be too important for release logging.
  If it still is, a thread-related \e Scope can be used.

The following sections describe each of the language-related \e Scopes in detail.

\I{################################################################################################}
### 5.2.1 Scope::Global ## {#alib_mod_alox_scopes_language_global}
As the name of this \e Scope indicates, this is the 'most outer' \e Scope of \alox. It is always
there and only one single 'instance' of it exists per \b Lox. In other words, all <em>Log Statements</em> or other
invocations of \alox are executed 'within' this singleton scope.

\note
  Because scopes are managed by class \b Lox, each instance of this class provides its own \e Global
  scope, the same as a \e Lox has different \e Loggers, <em>Log Domains</em>, <em>Scope Domains</em>,
  <em>Log Data</em>, etc. Well, and this makes perfect sense!

When setting <em>Scope Domains</em> for the \e Global \e Scope, all <em>Log Statements</em> have such domain
as their root-domain. (Of course, unless one inner <em>Scope Domain</em> or the statement itself
is using an absolute domain path!).


One use case of setting a <em>Scope Domain</em> for \e %Scope::Global could be described as follows:
An instance of class \b Lox is used for release-logging of special and sparse <em>Log Statements</em> (e.g., logging into the operating
systems' journal, aka event log). Now, in debug versions of the executable a debug-\e Logger is
attached to the release lox (in addition to attaching it to the debug-lox), within this debug log output, all
of these special <em>Log Statements</em> would be nicely sorted into the <em>Scope Domain</em> of \e %Scope::Global,
while non of the <em>Log Statements</em> to this release lox need to specify that domain path.

Another use case are special debug situations. Imagine a dubious behavior of software
is observed rather seldom. A programmer could register a debug \e Logger with \e Verbosity \e All
for domain '/DEBUG'. Now, when a certain first indication for the start of the dubious behavior
occurs, a setting of the <em>Scope Domain</em> '/DEBUG' for \e %Scope::Global can be activated.
From this point in time, all <em>Log Statements</em> would be activated, because all <em>Log Statements</em> of all
code would be gathered beyond that temporary <em>Log Domains</em> '/DEBUG' and all Subdomains inherit its \e Verbosity.


\I{################################################################################################}
### 5.2.2 Scope::Path  ## {#alib_mod_alox_scopes_language_path}

This scope adresses the directory structure that a C++ source code file is located in.
Unfortunately, \alox is not able to automatically gather information about the namespace that a
<em>Log Statement</em> resides in, but if the source codes are stored in accordance to the namespaces
they implement, a quite congruent behavior to the namespace scopes of C++ can be achieved with
\alox \e Scopes.

For example, a directory tree could look like this:

        /home/user/myproject/src/main.cpp
        /home/user/myproject/src/ui/menu/menu.hpp
        /home/user/myproject/src/ui/menu/menu.cpp
        /home/user/myproject/src/ui/dialogs/about.hpp
        /home/user/myproject/src/ui/dialogs/about.cpp
        /home/user/myproject/src/io/sockets/http.hpp
        /home/user/myproject/src/io/sockets/http.cpp
        /home/user/myproject/src/io/database/mysql.hpp
        /home/user/myproject/src/io/database/mysql.cpp

Using method
\doxlinkproblem{classalib_1_1lox_1_1Lox.html;ab6f2a9bee025cf4a0e6ff4b84ef2d1a5;Lox::SetDomain(const NString&, Scope)}
with \e Scope::Path instructs \alox to use the direct parent directory of a source file as the scope
for the given <em>Scope Domain</em>. Thus, this domain is activated for all <em>Log Statements</em>
residing in any source file of this directory.<br>
Now, method <b>%Lox::SetDomain</b> allows adding an integral value to the enumeration value
<c>Scope::Path</c> and call the method with argument <c>Scope::Path + N</c>. This tells \alox to
cut \b N directories from the path and hence to refer to an 'outer directory'.
This way, \e %Scope::Path can be used to denote <b>a whole set of nested scopes</b>!
When executing a <em>Log Statement</em> within a source file, all <em>Scope Domains</em> which are
set to any parent directory of the source file are applied!

\note
  In general, the organization of the source code in a well structured tree of directories
  corresponding to the tree of nested namespaces, is a good idea.<br>
  If a project using \alox is \e not organized this way, and there is no possibility to restructure
  the source tree for this purpose, then the \e %Scope::Path still can be used. It just will
  not match and reflect the namespace but (quite as its name indicates) the structure of the
  source tree.<br>
  However, if all sources of a project simply reside just in one single directory, then the use of \e %Scope::Path
  is not advisable. Its effect would be similar to \ref alib_mod_alox_scopes_language_global "Scope::Global", with the
  only difference that a thread-related \e Scope is applied after \e %Scope::Path but before \e %Scope::Global.
  (Thread-related \e Scopes are discussed later in this chapter).
\par

\note
  Because \alox is designed to tolerate errors as much as possible, adding values to enum value
  <c>Scope::Path</c> that are higher than the number of parent-directories available to be cut,
  does *not* result in an error.
  Instead, an empty path name is registered and consequently, a registration
  with, lets say <c>Scope::Path + 42</c> would simply be overwritten by a subsequent registration
  with level <c>Scope::Path + 43</c> and the effect would be nearly similar to using
  \e %Scope::Global.<br>
  On Windows OS, still the drive letter would remain and such setting would apply to
  all source files residing on the corresponding drive.


\I{################################################################################################}
### 5.2.3 Scope::Filename  ## {#alib_mod_alox_scopes_language_filename}

\e %Scope::Filename does not really need to be explained. All <em>Log Statement</em> within the same
source file share the same \e Scope::File. It is an inner scope of \e %Scope::Path.
Files with the same name, but residing in different directories have different scopes.

C++ source file names typically have an extension like .cpp, .hpp, .h, .hxx, etc. Such
extension is ignored by design. This way, by setting a <em>Scope Domain</em> of \e %Scope::Filename
e.g., from within file 'mydialog.cpp', such setting also applies to <em>Log Statements</em> occurring
in the corresponding header 'mydialog.hpp'.
Of course, this is only true if both files reside in the same directory.


\I{################################################################################################}
### 5.2.4 Scope::Method ## {#alib_mod_alox_scopes_language_method}

\e %Scope::Method comprises all <em>Log Statements</em> which are residing in a certain method
or function. This is the 'most inner' of the language-related set of \e Scopes
supported by \alox. But it is not the most inner of all \alox \e Scopes, as we will learn later
in this chapter.

As \e %Scope::Method is 'language-related', its behavior is like in the programming language in respect
to nested method calls: Same as a method variable is not 'visible' within other methods
that are invoked from this method, a <em>Scope Domain</em> set in a method is not 'active' within
nested, invoked methods. This of course makes a lot of sense, otherwise <em>Scope Domains</em> of methods
would be overwritten or at least redirected by <em>Scope Domains</em> of a calling method.

\note
  \alox also provides a feature of adding a <em>Scope Domain</em> for a method and all
  methods it invokes! The way how to achieve this is described later in this chapter.

\attention
  Unfortunately, \alox receives just the name of a class method or a function. While this
  name is associated with the file name, and thus two or functions or methods with the same
  name that reside in different files, do not share the scope (as expected), those that
  are implemented in the same header-file or compilation unit, do share the same scope.
  This is not what a user would expect but there is no technical way to avoid that.<br>
  Therefore, if two or more functions (in different namespaces) or class methods share the
  same name and reside in the same file, the behavior of \alox might be different to
  what a user expects!


\I{################################################################################################}
### 5.2.5 Anonymous Scopes ## {#alib_mod_alox_scopes_language_anonymous}
C++ allows opening and closing 'anonymous scopes' using curly braces '{' and '}'.
For example, a variable declared in such anonymous scope is not visible to the rest
of the method after the anonymous scope is closed.
Unfortunately, these anonymous scopes cannot be 'detected' by \alox automatically.
In C++, with the help of its concept of strict 'stack-unwinding', it would be possible to
extend \alox to support inner \e Scopes for nested blocks that automatically get
unset as soon as program execution leaves an anonymous scope. In favor of keeping the different
language versions compatible (and also in favor to not overcomplicate things!), this is not offered by \alox.

But there is an obvious way to reach the goal of setting Subdomains for <em>Log Statements</em> within a
block of code: Simply equip each <em>Log Statement</em> of an anonymous source scope with a relative path
using parameter \p{domain}. Such relative domain paths provided with
a <em>Log Statement</em> are placed by \alox within the evaluated, resulting domain path, as if they
resulted from a <em>Scope Domain</em> setting of an inner \e Scope of \e %Scope::Method.

\I{################################################################################################}
### 5.2.6 How To Set Scope Domains for Language-Related Scopes ## {#alib_mod_alox_scopes_language_setting}

When reading earlier chapter \ref alib_mod_alox_logdomains and the later chapters
(\ref alib_mod_alox_log_once, \ref alib_mod_alox_prefix_logables, and \ref alib_mod_alox_log_data),
you might be surprised, that the only way to manipulate a specific \e Scope is to do this with an
invocation <b>from within</b> that \e Scope itself.

Why does \alox not provide alternative interfaces that makes it possible to
explicitly address a \e Scope with an invocation from 'outside' of this scope? E.g., why is it not
possible to set the scope for a method by naming the path, file, and method?

The reason is to avoid ambiguities and misconfigurations. The source files' paths may be quite
volatile things. When working in a team, or already when one person is working in parallel on two
different machines (at work and at home) the paths may vary.
Furthermore any sort of code refactoring in any respect would enforce a 'manual' change of scope
specifications.<br>
The errors and hassle that would quickly occur when the explicit naming of \e Scopes was
supported by \alox would not justify the benefits.

But we do not consider this as a huge restriction. The responsibility for <em>Log Domains</em> names
is deemed to rely in 'the hands' of the code that is defining the <em>Log Statements</em> using these
<em>Log Domains</em>. The handler of a certain subset of a code within a project should know best
which domains and Subdomains are to be used. As an exception, the use of rather 'global' domains
that collect certain information, e.g., "/CONFIG_ERRORS", should be aligned across the team.
Usually those domains are addressed using an absolute path within a <em>Log Statement</em> - and
hence are not impacted by the \e Scope and potentially associated <em>Scope Domains</em> anyhow.

\note
  There is one obvious use case, that might let you think about changing <em>Scope Domains</em> set
  in scopes not reachable by your code: This is when for some reason you want to change
  the domains that a library (or part of the project you do not have access to) uses.
  To do this, \alox provides a concept called <em>Domain Substitution</em>,
  described in \ref alib_mod_alox_domain_substitution. The huge advantage of this approach is, that this
  feature substitutes \b all <em>Log Domains</em>, regardless whether they are evaluated by \alox from
  <em>Scope Domains</em> or if they are given as parameters of the <em>Log Statement</em>.

Having said this, it is agreed and understood that the definition of <em>Scope Domains</em> of
language-related \e Scopes has to appear in source code within the \e Scope itself - optionally
within an 'inner' \e Scope of the \e %Scope.
For example, within a method of a class, both \e %Scope::Method and \e %Scope::Filename
can be set.

What should be avoided are <b>Contrary settings</b>.
If the same \e Scope is set with different values, the second invocation just replaces the first
one. Therefore, some random behavior might appear when the settings of <em>Scope Domains</em> are
contrary. For example, a <em>Scope Domain</em> for a source folder could
be set from within different classes residing in or below this source folder.
As a rule of thumb (to avoid double definitions), it is advised to put the registration code to
the most central (important) class of such directory.

A snapshot of all current settings can be logged using \alib{lox;Lox::State} to investigate which
settings have been performed. Alternatively, if the snapshot is not enough to understand what is set,
overwritten, and used where, a live log of \alox' internal messages can be activated to identify
exactly which code lines are doing what in respect to <em>Scope Domains</em>.
See \ref alib_mod_alox_internal_domains for more information about how to enable internal log messages.


\I{################################################################################################}
## 5.3 Thread-Related Scopes # {#alib_mod_alox_scopes_thread}

This section adds two new \e Scope 'levels', named:

- \alib{lox;Scope;Scope::ThreadOuter} and
- \alib{lox;Scope;Scope::ThreadInner}

to the \alox \e Scope feature. As the name indicates, these \e Scopes create a reference to the thread that
is executing a statement that is using values associated with such \e %Scope.

\note
  Even if your application is single-threaded, you should continue reading!

Looking at <em>Scope Domains</em>, of course, they are used to add an additional component to the overall evaluated
<em>Log Domains</em> path of a <em>Log Statement</em>. For \e %Scope::ThreadOuter, such addition is performed at the
beginning of the evaluated <em>Log Domains</em> path, directly after \e %Scope::Global.
For \e %Scope::ThreadInner, the <em>Scope Domain</em> set is appended at the very end
of the evaluated <em>Log Domains</em> path. The term 'very end' is important: This is not only the most
'inner' of all \e Scopes, it is appended to the assembled <em>Log Domains</em> path even after the
optional parameter \p{domain} of a <em>Log Statement</em>.
In other words, it could be said, that \e %Scope::ThreadInner is even more 'inner' than the local,
optional parameter \p{domain} of a <em>Log Statement</em>.

The whole list of <em>Scope Domains</em>, together with the parameter \p{domain}, which are all concatenated
(as long as none of them is an absolute path) results to:

1. [L] Scope::Global
2. [T] Scope::ThreadOuter
3. [L] Scope::Path
4. [L] Scope::Filename
5. [L] Scope::Method
6. Parameter \p{domain} of a <em>Log Statement</em>
7. [T] Scope::ThreadInner

<em>Remark: [L] and [T] here indicate language-related and thread-related \e Scopes.</em>

\I{################################################################################################}
### 5.3.1 Use Cases for Scope::ThreadOuter ## {#alib_mod_alox_scopes_thread_outer}

An important use case for <em>Scope Domains</em> of \e %Scope::ThreadOuter is useful in single-threaded
applications, the same as in multithreaded.
If a <em>Scope Domain</em> is set for \e %Scope::ThreadOuter before invoking
a method (and removed right after the invocation), all subsequent <em>Log Statements</em> are 'redirected' to
the domain path specified, covering the whole call stack of nested method calls. This way,
a portion of the program execution can be controlled in respect to the \e Verbosity of <em>Log Statements</em>
easily. You can consider this use as being similar to \e %Scope::Method but lasting not only for
the method itself but for all statements of recursively invoked methods as well.


In multithreaded applications, \e %Scope::ThreadOuter is typically used in situations where the log
output of different threads should be separately controlled in respect to the \e Verbosity of their log output.
Imagine a background thread that causes trouble but uses the same code and libraries that the
rest of the application does. If you now would increase the \e Verbosity of such <em>Log Domains</em> where the
problems occurred, the log output would be 'cluttered' with a lot of <em>Log Statements</em> caused by
any thread of the process. Setting \e %Scope::ThreadOuter allows 'redirecting' all such log-output
of the thread in question to a dedicated root domain. Now, controlling the \e Verbosity of
the Subdomains of this thread-specific root domain allows investigating directly what is
happening there. This sample addresses debugging and probably a temporary 'redirect' of domains
that is removed when a problem is fixed.

But there are also samples where a permanent setting of a \e %Scope::ThreadOuter makes sense.
Most operating systems/programming environments are using a dedicated thread implementing the
user interface. Handlers of UI-events like mouse clicks are installed and executed on a per
event basis. If now, with the very first UI event firing into the user code, (e.g., signaling that
the application is now running, or the main window was created), a <em>Scope Domain</em> like 'UI' is
registered with \e %Scope::ThreadOuter, all UI related code magically logs into
this domain path. As a consequence, no UI-related code like classes for dialog boxes, menu handlers,
etc, need to set such domain themselves (e.g., using \e %Scope::Path in a constructor). <br>
Furthermore, it becomes very obvious from just looking at the Subdomains that get created, when
the UI thread is tasked with things that rather should be moved to a different thread to avoid
blocking the application for too long.

\note
  This last sample nicely shows, how the use of \alox for all debug-logging tasks, leads to new
  insights of software, that simple "debug log statements" do not provide!

\I{################################################################################################}
### 5.3.2 Use Cases for Scope::ThreadInner ## {#alib_mod_alox_scopes_thread_inner}

While technically \e %Scope::ThreadInner is very similar to \e %Scope::ThreadOuter, the effect and
use cases still differs slightly. Instead of 'redirecting' just all log output of a thread into
a new subtree of <em>Log Domains</em>, \e %Scope::ThreadInner splits all 'leafs' of the overall
Log Domain tree by adding a thread-dependent <em>Log Domain</em> to those leafs.

When we think about this for a minute, the obvious use case is to filter the log output of
specific Sub-<em>Log Domains</em> by thread. First, when a <em>Scope Domain</em> of \e %Scope::ThreadInner
is set, the \e Verbosity of the new Subdomains will not change. This is true, because all new domains that are
created by this thread are Subdomains of those <em>Log Domains</em> used before. And such Subdomains
just inherit the setting as long as they are not controlled explicitly (as explained in
\ref alib_mod_alox_logdomains_recursive). From here, specifically for this thread, the \e Verbosity of
certain domains can now be tweaked until the right set of <em>Log Statements</em> appear.

Imagine a very general class providing a very general feature, hence frequently used by different
parts of software. Increasing the \e Verbosity of a <em>Log Domains</em> of such class might increase the
overall log output too much. Now, by splitting such <em>Log Domains</em> using a <em>Scope Domain</em> for \e %Scope::ThreadInner
it becomes possible to either decrease the \e Verbosity for threads that are not of current
interest or by only increasing the \e Verbosity of the thread of interest.

Finally it is noteworthy to mention the impact of \e %Scope::ThreadInner being the most inner
<em>Scope Domain</em> that is evaluated:
1. A <em>Log Statement</em> that provides an absolute domain path directly in its statement using
   optional parameter \p{domain}, can still be split by <em>Scope Domains</em> of
   \e %Scope::ThreadInner.
2. If an absolute domain path is provided for a <em>Scope Domain</em> of \e %Scope::ThreadInner, then
   this 'flattens' all log output into exactly this single domain. (Even parameter \p{domain}
   of a <em>Log Statement</em> will not be used!)


\I{################################################################################################}
### 5.3.3 Multiple Use of Thread-Related Scopes ## {#alib_mod_alox_scopes_thread_multiple}

We learned in section \ref alib_mod_alox_scopes_language_path, that this \e Scope through the use of
different parent directories may be seen as whole set of nested \e Scopes itself.

The same is true for \e %Scope::ThreadOuter and \e %Scope::ThreadInner! If multiple <em>Scope Domains</em>
are set for one of both \e Scopes, instead of overwriting the previous setting (as done with
language-related scopes), such <em>Scope Domains</em> are added to the ones that were previously set.<br>
This is important for almost all use cases described in the previous sections.

Hereby, subsequent settings are 'inner' \e Scopes of the previous ones. This means, that
during program execution the first <em>Scope Domain</em> that is set, results in a higher level within
the domain tree. Subsequent <em>Scope Domains</em> set result in direct Subdomains of the former
ones.<br>
\alox, when passing a nulled string with parameter \p{scopeDomain} of method \b %Lox::SetDomain removes
the most recently set <em>Scope Domain</em> first. But also an out-of-order removal of thread-related \e Scopes is possible.
More details on setting and removing <em>Scope Domains</em> for thread-related \e Scopes is given
in the next section.


\I{################################################################################################}
### 5.3.4 How To Set Scope Domains for Thread-Related Scopes ## {#alib_mod_alox_scopes_thread_setting}

The same method,
\doxlinkproblem{classalib_1_1lox_1_1Lox.html;ab6f2a9bee025cf4a0e6ff4b84ef2d1a5;Lox::SetDomain(const NString&, Scope)}
which is used for language-related \e Scopes is used to set and remove thread-related \e Scopes.

If a domain path is given with parameter \p{scopeDomain} and either \e %Scope::ThreadOuter or
\e %Scope::ThreadInner for parameter \p{scope}, then this domain path is added to the list of
corresponding domains set. The list reflects a set of nested \e Scopes for itself.

To remove the most recently added <em>Scope Domain</em>, it is sufficient to do the same call, with
an empty or nulled parameter \p{scopeDomain}. Again, this is the same as with removing or 'un-setting'
<em>Scope Domains</em> of other \e Scope types.

For the case that the reverse order of adding and removing thread-related <em>Scope Domains</em>
cannot be guaranteed, class \b Lox offers method \alib{lox;Lox::RemoveThreadDomain} which accepts
the domain path to be removed explicitly as a parameter.

It was discussed in \ref alib_mod_alox_scopes_language_setting, that those types of \e Scopes can only be
set from 'within' the \e Scope to be set (the same or an inner \e Scope). This is different
with thread-related \e Scopes. Method
\doxlinkproblem{classalib_1_1lox_1_1Lox.html;ab6f2a9bee025cf4a0e6ff4b84ef2d1a5;Lox::SetDomain(const NString&, Scope)}
as well as \alib{lox;Lox::RemoveThreadDomain} accept an optional parameter \p{thread} which allows
explicitly providing the thread object to associate a thread-related <em>Scope Domain</em> to.
Of course, if this parameter is omitted, the 'actual \e Scope', hence the current thread, is used.

\note
  While \alib provides class \alib{threads;Thread}, usually, software using would use a different
  library to create threads. As long as such library is creating 'native' threads of the underlying
  operating system, this is no issue. To refer to a thread created with a different library, the
  approach is as follows:
  - from within the thread that is created, static method \alib{threads;Thread::GetCurrent}
    has to be invoked.
  - The pointer to the \alib{threads;Thread} object received has to
    be passed to the scope of the method that is supposed to set a thread-related \e Scope value
    for referencing the thread in question.


When things get more complicated, same as with language related scopes, a snapshot of all current
settings can be logged using \alib{lox;Lox::State} to investigate which settings have been performed.

Alternatively, if the snapshot is not enough to understand what is set, removed, and used where,
a live log of \alox' internal messages can be activated to identify exactly what the code is doing
in respect to <em>Scope Domains</em>.
See \ref alib_mod_alox_internal_domains for more information about how to enable internal log messages.


\I{################################################################################################}
## 5.4 Wrap up # {#alib_mod_alox_scopes_wrapup}

We want to summarize the takeaways of this chapter:

- \e Scopes are used for different features of \alox, as documented in
  \ref alib_mod_alox_logdomains, \ref alib_mod_alox_log_once, \ref alib_mod_alox_prefix_logables, and \ref alib_mod_alox_log_data.
- Most of the samples and explanations in this chapter are related to <em>Scope Domains</em>.
- \e Scopes are nested, we have inner and outer \e Scopes.
- Four (programming-)language-related \e Scopes were introduced. They work similar to scopes of
  the programming languages.
- Two thread-related \e Scopes were introduced. They are runtime \e Scopes and associated with
  the thread executing a <em>Log Statement</em>.
- The two thread-related \e Scopes differ only in the 'position' within the hierarchy of \e Scopes.
- It was explained, that language-related \e Scopes are set exclusively by statements that are placed
  within the according \e Scope itself...
- ...while thread-related \e Scopes can also be set from within other threads, by providing
  the associated object of type Thread explicitly.
- Repetitive settings of language-related \e Scopes are overwriting previous settings.
- Repetitive settings of \e thread-related \e Scopes are each acting as a nested, inner scope of
  the previous setting.
- Removing <em>Scope Domains</em> is performed by passing a nulled or empty domain path to
  method \b Lox::SetDomain. In the case of thread-related \e Scopes this removes the most
  recently added <em>Scope Domains</em>. Therefore, if out-of-order removals are needed, method
  \b Lox::RemoveThreadDomain is to be used for removal.
- To investigate into settings of a \b %Lox, two options exist. Either by creating a snapshot
  of the current setting (using method \b Lox::State) or by activating internal log messages
  and observing which setting is made at which position in the source code and which point in time
  when running a process.

Finally we want to express an important thought: The three concepts of \alox, namely
1. Hierarchically organized <em>Log Domains</em>,
2. Nested \e Scopes and
3. <em>Scope Domains</em>,

align very nicely. Clever use of them may lead to true "emergence": Suddenly, log output provides
more information than the single <em>Log Statements</em>' messages itself. (Similar to water, which
has different "emerged" properties than the sum of properties of its molecules.)


But it should not be forgotten what the main purpose of <em>Log Domains</em> is: It is controlling
the \e Verbosity of <em>Log Statements</em>. In other words, the main purpose of
<em>Log Domains</em> is not to understand and analyze the calling hierarchy (call stack)
a piece of code produces.
While \alox may be used to help here quite nicely, there are other software tools and techniques
available for accomplishing this.<br>
Therefore , our recommendation is: <b>Do not overuse the concept of <em>Scope Domains</em></b>.
With too many <em>Scope Domains</em> set, the original purpose of <em>Log Domains</em> may become
harder to achieve. Already the maintenance of \e Verbosities may start causing some unwanted effort.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 6 Lox::Once() # {#alib_mod_alox_log_once}
## 6.1 Introduction # {#alib_mod_alox_logonce_intro}

For a better understanding of what is explained in this chapter, it might be advisable to have read chapter
\ref alib_mod_alox_scopes before continuing with this one. In short, at that place it is explained how \alox defines
language-related \e Scopes (including the 'global' \e Scope) and thread-related \e Scopes.
Both types are 'interwoven' and form the complete set, as denoted in enum-class \alib{lox;Scope}.

The most important use of \e Scopes in \alox is for setting and using <em>Scope Domains</em>
which helps sorting and controlling the log output tremendously. Further information on
this topic is found in chapter \ref alib_mod_alox_logdomains.
This chapter provides details about a feature that we simple call \b %Lox::Once.

Using \alox, you have probably come across method \alib{lox;Lox::Once}, with its various parameters
and overloaded versions which omit and default the parameters.

As the name indicates, this method is used to execute a <em>Log Statement</em> only once. A first use case of such
a <em>Log Statement</em> is obvious: Whenever it is interesting to know whether a piece of code was executed
at least \e once, this <em>Log Statement</em>:

\snippet "ut_alox_dox.cpp"          Man_LogOnce___1

is the one to go. No further (debug) code needs to be added and pruned using \c \#if / \c \#endif
clutter. Its nice and readable, just as <em>Log Statements</em> should be.

The concept of method \b %Lox::Once goes quite beyond this simple sample. Before we go to the
real interesting things, lets first quickly introduce other parameters that are optional to all
other variations of this method:
- As with all <em>Log Statements</em>, a <b>domain path</b> can be given:

  \snippet "ut_alox_dox.cpp"        Man_LogOnce___2

  Note that there is no overload available that accepts a <em>Domain Path</em> and defaults
  the verbosity to <em>Verbosity::Info</em>. This has technical reasons: The overload resolution
  would become ambiguous.

  This sample above indicates a next use case besides the one asking for
  "was a piece of code reached once". This use case is: <em>"Log problems or facts, which will not
  change in the future only once"</em>.
  Here, the configuration file "config.ini" contains wrong information. It is what it is. Do not tell me this
  twice and do not clutter the log output in subsequent execution of software.


- At this point in time, it might be worth to mention, that the counter is increased with
  each \b Lox::Once statement independently from the \e Verbosity setting of the effective
  <em>Log Domain</em> such statement is associated with. In other words, even if the domain was
  switched off (using \b Verbosity.Off), the counter is still increased. By the same token, if
  more than one \e %Logger (or none) is attached to the \b Lox, still the counter is increased by
  exactly \c 1.

  Then, although the method is named 'Once', parameter \p{quantity} allows specifying a different
  number of occurrences:

  \snippet "ut_alox_dox.cpp"        Man_LogOnce___4

  Yes, yes, not really true: when the integral value of the internal counter overflows, you'll
  see an next 10 log statements. The counter is implemented as <c>int</c>.

- Finally setting the parameter to a negative value, lets \alox perform the log the first time
  it is invoked and after that, every n-th time (to be precise, every (-quantity)-th time).

  \snippet "ut_alox_dox.cpp"        Man_LogOnce___5


\I{################################################################################################}
## 6.2 Grouping # {#alib_mod_alox_logonce_grouping}

In the introduction we have seen use cases like:
- Is a certain piece of code reached?
- A log message results from a non-changeable fact, and hence it should only be logged once.

These and similar use cases are now extended from concerning <em>one</em> log message to
<em>a set of</em> log messages. Let us Stick to the sample with the mis-configured config.ini:

\snippet "ut_alox_dox.cpp"        Man_LogOnce___2

This might be observed in two methods: when data is written or when it is read:

\snippet "ut_alox_dox.cpp"        Man_LogOnce___11

While this works well and we could be happy as it is, the little drawback here, is that, if both methods
get executed, each <em>Log Statement</em> is executed once and hence, this sums up to two. For a debug-log
output on a IDE console, this is not really dramatic. But there are other usage scenarios
of logging (logging to a system journal, logging over the air, etc.) where \alox should be asked
to avoid this.

And it can be very easily avoided:

\snippet "ut_alox_dox.cpp"        Man_LogOnce___12

We provide optional parameter \p{group}, both valued <em>\"INI_DIR_ERROR\"</em>. Because both log
statements share the same \e Group <em>'key'</em>, \alox shares its internal counter for the number of
already performed logs between them.

\note
  Of course, such \e Group keys do not need to be registered and their use is not limited. As
  always, \alox just manages internally what the user feeds in. Even if parameter \p{quantity} is provided
  and it differs between \b Lox::Once statements that belong to the same group, \alox does what you
  might expect:
  - The counter is shared and increased with each executed (!) statement.
  - Each \b Lox::Once statement checks the counter against what is provided with \p{quantity}.
  In other words: While one statement of a group may be already disabled, others might continue
  logging. Please do not ask us for a use case of this!

In summary, parameter \p{group} is used to group a number of \b Lox::Once statements which are
located in arbitrary places of software together and count the number of overall executed logs.

For a short time, this is all to know about using \e Groups. But we will come back
to \e Groups after the next section.

\I{################################################################################################}
## 6.3 Lox::Once and Scopes # {#alib_mod_alox_logonce_scopes}

Instead of 'grouping' a set of \e Lox::Once statements by manually assigning them a
group name, \alox \e Scopes can 'automatically' group statements which in a 'natural' way belong
together. If the methods \b ReadData and \b WriteData from the previous sample reside in
the same source file, the very same that was achieved using \e Groups, can be achieved
using \e Scopes:

\snippet "ut_alox_dox.cpp"        Man_LogOnce___21

Comparing the samples, it can be observed, that parameter \p{group} of type \e String was replaced by
parameter \p{scope} of type \b %Scope, with value \b Scope::Filename. For other possible
values, see chapter \ref alib_mod_alox_scopes.

From a birds' perspective, the advantages of \e Scopes are:
- No need to 'invent' a group name and no risk of accidentally using the same name twice (e.g.
  in a library that a user does not even have access to).
- No need to double-check what \e Group key was used with other statements in the set.
- Copy and pasting of the <em>Log Statements</em> (into a different \e Scope), 'automatically' adjust the
  'grouping key'.

Their biggest disadvantage: There is only one \b Lox::Once counter per \e %Scope. The sample above
does not support two different sets of methods that independently from each other get counted.

On the other hand, the advantage of \e Groups is: Statements from completely different \e Scopes
can be grouped and there is an unlimited number of \e Groups possible.

There is one thing, that can only be achieved with \e Scopes, namely with
\b Scope::ThreadOuter or \b Scope::ThreadInner. This attaches the \b Lox::Once counter to
a thread that is executing one of the statements. This opens the new use case for \alox:
- Log one or one set of messages, up to n-times per execution thread.

As we see, using groups or using scopes have their proper use case and both have advantages and
disadvantages, so why not combining them?


\I{################################################################################################}
## 6.4 Combining Groups and Scopes # {#alib_mod_alox_logonce_gs_combined}

The parameter list of \alib{lox;Lox::Once} and some of its overloaded variants, allow to provide
both, a \e Group name and a \e %Scope.
To understand the consequence, its best to explain how \alox internally handles \e Groups.<br>
\e Groups of course are implemented with hash-tables. Their key is a \e String, the \e Group name,
and their value contains the counter. Now, \alox (to be precise, class \b Lox), creates a
hash-table for \e Group keys for each 'instance of a scope' where \b Lox::Once is used.<br>
When parameter \p{scope} is omitted with \b Lox::Once, it simply defaults to \e %Scope::Global, which
is, as explained in \ref alib_mod_alox_scopes, a singleton for each \b Lox. Consequently, each and every
statement belongs to \e %Scope::Global and this is why \e Groups of default \e %Scope::Global
seem to work independently from any scope.<br>
The other way round, if parameter \p{group} is omitted, then there are two options for \alox: If
parameter \p{scope} is given and is not equal to \e %Scope::Global, then \alox uses a default
\e Group key. Because this is the same for all such invocations, the statement is bound to the
given \e Scope and to nothing else. In the second case, when \e %Scope::Global is given (still with no
\e Group), \alox switches to \e %Scope::Filename and creates the \e Group key from the line number
of the invocation. This way, the 'parameterless' invocation of \b LogOnce,
results in binding the counter exclusively to this single \b Lox::Once statement.

The truth therefore is, that \alox always combines \e Groups and \e Scopes, we just have not noticed
that, yet.
After having understood this, the only question is: What new use cases do we get when using
\e Groups with \e Scopes other than \e %Scope::Global?
So far, we had:

- Is a certain piece of code reached?
- Don't spoil my log-output, so stop after n-messages of the same type.
- A log message results from a non-changeable fact, and hence it should only be logged once.
- A set of log messages result from a non-changeable fact, and hence only one of them should
  be logged once (or together n-times).
- Log one or a set of messages, up to n-times per execution thread.

With using \e Groups in \e Scopes other than \e %Scope::Global all of the above get combined and
a little more. Just for example:

- %Log only the first n statements which belong to the same group and are placed within
  any method of
  - a source file
  - a directory of source files
  - a parent directory of source files and all sources recursively
- %Log only the first n statements of a group of statements executed by a specific thread.
- Have an arbitrary number of different \e Groups of Lox::Once statements per Thread.

It is up to the reader of this manual and user of \alox to adopt his own use cases to this list.

\I{################################################################################################}
## 6.5 Passing Multiple Logables To Lox::Once# {#alib_mod_alox_logonce_multiple_logables}
Unlike other methods of class \b %Lox that comprise <em>Log Statements</em> which accept an
arbitrary amount of logables, method \b %Once and its overloaded variants accept only one logable.
This restriction is caused by the otherwise complicated set of parameters and overloads of this
method. There is an easy way out!

To supply more than one \e Logable, in C++ a container of type \alib{boxing;TBoxes} may be passed
with parameter \p{logables}:
\snippet "ut_alox_lox.cpp"      DOX_ALOX_LOX_ONCE

\I{################################################################################################}
## 6.6 Wrap-Up # {#alib_mod_alox_logonce_wrapup}

Some combinations of \e Scope 'levels' and using \e Groups probably do not make too much sense.
For example: Grouping different \b LogOnce statements together within \e %Scope::Method? Well, you rather
think about splitting a huge method into a bunch of smaller ones, as your method seems to have
become a little complex.

If this gets all to complicated for you being new to \alox, here are some hints:
- Just don't overuse the combination of \e Groups and \e Scopes with \b Lox::Once.
  You will probably get along without any of it! A simple:
  \snippet "ut_alox_dox.cpp"        Man_LogOnce___1_wrapup
  is mostly what is needed for debug-logging.

- Like with other features of \alox, if parameters are omitted, you don't even see the more complex
  options and you are not bothered too much.
  See \ref alib_mod_alox_apdx_auto_config for a summary of this.

- If, especially in decent release-logging scenarios, a more complex setup troubles you,
  switch on \alox internal logging and see exactly what is happening where, when, and why!
  Information on how to do this is found in \ref alib_mod_alox_internal_domains.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}

# 7 Prefix Logables # {#alib_mod_alox_prefix_logables}
\I{################################################################################################}
## 7.1 Introduction # {#alib_mod_alox_prefix_logables_intro}

The feature of \alox called <b>Prefix Logables</b> covered in this chapter, builds upon
the \alox concept of \e Scopes in a similar way as feature <em>Scope Domains</em> does.
Therefore , it is advisable to have read and understood chapters
- \ref alib_mod_alox_logdomains and
- \ref alib_mod_alox_scopes before continuing reading here.

This chapter will \e not repeat every detail covered already in the aforementioned chapters.

\e Logables in \alox are of type \alib{boxing;Box}. Class \b Lox forwards them to its attached loggers with
each <em>Log Statement</em>. In addition to those provided with the statement, one or more
boxes might be added to the beginning of the list. We call these addable objects <em>Prefix Logables</em>.

Before we explain the use cases for <em>Prefix Logables</em>, let us begin to elaborate how those are set by the
user of \alox and how \alox processes them.

\I{################################################################################################}
## 7.2 Setting Prefix Logables # {#alib_mod_alox_prefix_logables_setting}

### 7.2.1 Setting Prefix Logables According to the Scope ## {#alib_mod_alox_prefix_logables_setting_1}
With the first way of setting and removing <em>Prefix Logables</em> method
\doxlinkproblem{classalib_1_1lox_1_1Lox.html;a868d9d33d9456b6380abf3bf39cd8b16;SetPrefix(const Box&, Scope)}
 is used. The method and its overloaded versions
is very similar to method
\doxlinkproblem{classalib_1_1lox_1_1Lox.html;ab6f2a9bee025cf4a0e6ff4b84ef2d1a5;Lox::SetDomain(const NString&, Scope)}
used for setting <em>Scope Domains</em>. Besides the difference in the name, the only
difference is the first parameter, which is a \e Logable instead of a domain path string.

All that was said about setting <em>Scope Domains</em> in Chapter \ref alib_mod_alox_logdomains is true
for setting <em>Prefix Logables</em> and this should not be repeated here. The same rules for
\e %Scopes apply, including the fact that with \e %Scope::ThreadInner and \e %Scope::ThreadOuter,
a subsequent setting of a <em>Prefix Logable</em> is added to a list of <em>Prefix Logables</em>
for these \e Scopes, while for other \e Scopes, the current <em>Prefix Logable</em> is replaced.

Passing \c nullptr as parameter \p{logable}, removes the <em>Prefix Logable</em> from the given \e Scope,
respectively, in the case of thread-related \e Scopes, removes the <em>Prefix Logable</em> most recently set.

The only small difference to the interface for setting <em>Log Domains</em> is that there is no method available
corresponding to \alib{lox;Lox::RemoveThreadDomain}, which provides a little extra flexibility
of maintaining <em>Scope Domains</em> in contrast to maintaining <em>Prefix Logables</em>.

\I{################################################################################################}
### 7.2.2 Setting Prefix Logables According to the Log Domain## {#alib_mod_alox_prefix_logables_setting_2}
Besides binding <em>Prefix Logables</em> to a \e Scope, \alox provides an alternative
and this is binding <em>Prefix Logables</em> to a <em>Log Domain</em>.
The method for doing this is \doxlinkproblem{structalib_1_1expressions_1_1plugins_1_1Calculus_1_1FunctionEntry.html;a3f9769a430a930630a75d41bc7e4055c;FunctionEntry::Signature}.

The method accepts a <em>Log Domain</em> path which may be absolute or relative. If
relative the normal evaluation of a resulting domain path taking <em>Scope Domains</em> into account
applies.<br>
A third optional parameter allows making the setting exclusive in respect to <em>Prefix Logables</em>
which are set according to a \e Scope. By default, the exclusivity is not set.

\note
 Alternatively, <em>Prefix Logables</em> bound to a <em>Log Domain</em> can be set using
 configuration variable \ref alxcvALOX_LOXNAME_PREFIXES. Of course, this allows only
 <em>Prefix Logables</em> of string-type to be set. More on this is found in chapter
 \ref alib_mod_alox_apdx_config_vars.

\I{################################################################################################}
### 7.2.3 Setting More than one Prefix Logable at a Time## {#alib_mod_alox_prefix_logables_setting_3}
While the <em>Log Statements</em> accept an arbitrary amount of objects, the methods
to set <em>Prefix Logables</em> have only one parameter. If more than one <em>Prefix Logable</em>
is to be set, the first alternative is to set them one by one:

  \snippet "ut_alox_dox.cpp"      Man_PREFIXLOGABLES_1


The second alternative is to wrap them in an object of class \alib{boxing;TBoxes}. This is derived
from <c>std::vector<aworx::Box></c>. If this is done, \alox will "flatten" the given arrays when the
<em>Prefix Logables</em> are passed to the \e Loggers. This means, instead of adding the array
to the overall list of \e Logables, the single objects contained in the array are added:

  \snippet "ut_alox_dox.cpp"      Man_PREFIXLOGABLES_2

\attention
    We had seen in a previous chapter how a list of logables was created and passed to \b Lox::Once:
      \snippet "ut_alox_lox.cpp"      DOX_ALOX_LOX_ONCE
\attention
    These two code snippets must \b not be used with \b Lox::SetPrefix. The problem is that the
    boxes array, respectively vector, is a local variable and will be removed from the stack, as
    boxes as the compound of the statement is closed. (With the closing brace <c>'}'</c>).
\attention
    Please consider later chapter \ref alib_mod_alox_prefix_logables_lifecycle for more information.


\I{################################################################################################}
## 7.3 How ALox Processes Prefix Logables # {#alib_mod_alox_prefix_logables_processing}
With any sort of <em>Log Statement</em> in \alox, the <em>Prefix Logables</em> are collected according
to the \e Scope and the <em>Log Domain</em> of that <em>Log Statement</em>.
In the same fashion as <em>Scope Domains</em> are concatenated, \alox adds <em>Prefix Logables</em> to the list of \e Logables
that are passed to each \e Logger instance. Consequently, the list that a \e Logger receives
is filled as follows:

1. <em>Prefix Logable</em> of \e %Scope::Global
2. <em>Prefix Logables</em> of \e %Scope::ThreadOuter (can be more than one)
3. <em>Prefix Logable</em> of \e %Scope::Path
4. <em>Prefix Logable</em> of \e %Scope::Filename
5. <em>Prefix Logable</em> of \e %Scope::Method
6. The <em>Prefix Logables</em> of the parent <em>Log Domain</em> in the order of there setting (recursively prepended!)
7. The <em>Prefix Logables</em> of the <em>Log Domain</em> in the order of there setting
8. <b>The \e Logable of the <em>Log Statement</em> itself</b>
9. <em>Prefix Logables</em> of \e %Scope::ThreadInner (can be more than one)

If in \b 6. or \b 7. a <em>Prefix Logable</em> was passed with optional parameter \p{otherPLs} valued
\alib{lang;Inclusion::Exclude}, then after adding this \e Logable, the collection of further <em>Prefix Logables</em> is stopped.
Because all objects are collected in reverse order, starting with objects of \e %Scope::ThreadInner,
this means that objects otherwise collected in \b 1. to \b 5. (respectively \b 6.) are not added.
This allows having a setting of a <em>Prefix Logable</em>, which is bound to a domain, to 'override' those bound to
a \e Scope.

As with any 'normal' \e Logable that is passed to the \e Logger, it is completely up to the
\e Logger what to do with this data.

Those \e Logables passed with \e %Scope::ThreadInner are appended to the list \e after
the <em>Log Statements</em>' \e Logable and therefore should be considered a 'suffix', not a prefix.
You might wonder why this whole feature is named 'prefix', especially as
this term is not applicable to objects in an ordered array. The answer to this is given in the
next section.

\I{################################################################################################}
## 7.4 Use cases of Prefix Logables # {#alib_mod_alox_prefix_logables_usecases}

Talking about the use cases of feature <em>Prefix Logables</em>, we have to distinguish between logging arbitrary
objects, what \alox supports, and logging textual (string) messages, what is by far the most widely
application for \alox.

### 7.4.1 Textual Logging # {#alib_mod_alox_prefix_logables_usecases_textual}
When logging textual messages (more precisely: when using \e Loggers derived from abstract class
\alib{lox::textlogger;TextLogger}, just as all textual \e Loggers delivered with \alox are),
the impact of <em>Prefix Logable</em> is simple. Class \b %TextLogger
just passes all objects found in the list of \e Logables to its
\alib{lox::textlogger;ObjectConverter} which in turn (in its default implementation)
passes them to the formatters found in field \alib{lox::textlogger;StandardConverter::Formatters}.
This list by default, has two objects, namely of types
\alib{format;FormatterPythonStyle;FormatterPythonStyle} and
\alib{format;FormatterJavaStyle;FormatterJavaStyle} attached.
This way, \b %TextLogger is well prepared to assemble a nicely formatted log output, by default
accepting Python formatter strings as well as the corresponding Java syntax.

\note
  As already explained in this manual and in the documentation of class
  \alib{format;Formatter}, there is an important feature that supports
  the concept of <em>Prefix Logables</em> very nicely: While usually (in other libraries and languages)
  such formatting classes accept \b one format string and an arbitrary amount of objects, with
  this class the format string is included in the object list. The first object may or may not
  be a format string. If it is not, the object is just "applied" (appended in textual representation)
  to the log output.
  As soon as a format string with placeholders is detected, the formatting process starts. All
  arguments consumed by the format string are then skipped in the list and - if still arguments
  exist - the algorithm continues from the start.<br>
  As a consequence, prefix logables can contain a format string and arguments, while still the
  <em>Logables</em>, which are collected from the <em>Log Statement</em>, can themselves contain
  a format string and corresponding arguments.

\note
  For more information on \e Loggers and \e %TextLogger, see chapters \ref alib_mod_alox_colorful_loggers and
  \ref alib_mod_alox_ext_loggers.

This explains the term \e 'prefix': Apart from <em>Prefix Logables</em> of \e %Scope::ThreadInner, all
<em>Prefix Logables</em> are prefixes to the 'log message'. Those of \e %Scope::ThreadInner are suffixes. For the architects
of the \alox API it was just too teasing to name the whole concept <em>Prefix Logables</em> and this way being able to
have - for the most obvious use case - the user code look like this:

\snippet "ut_alox_dox.cpp"        Man_LogSetPrefix___1

The output will look similar to this:

\verbinclude pages/generated/ALoxMan_LogSetPrefix___1.txt


### 7.4.2 Recursive Logging and Indentation # {#alib_mod_alox_prefix_logables_usecases_recursiveindent}
A next use case is recursively increasing 'indentation' of the log messages, as demonstrated
here:

\snippet "ut_alox_dox.cpp"        Man_LogSetPrefix___2

Note that this sample is using \e Scope::ThreadOuter. If it was using \e %Scope::Method it would fail,
because only the thread-related \e Scopes allow to add multiple objects. With thread-related \e Scopes,
this works like a 'push and pull' mechanism. Luckily, with using the thread-related \e Scopes, the
whole indentation is automatically \e thread-safe!

Indentation can also be useful when adding prefixes for different language-related \e Scopes.
For example, classes whose source files are residing in nested directories (alike namespaces),
might be considered core, helper tools that usually have a low \b Verbosity setting.
It might be a good option to indent all their logging by setting a prefix for their namespace.
If they need to be debugged, and their \e Verbosity is increased, <em>Log Statement</em> of those are due to the
indentation still very easily distinguishable from the rest of the log output. Such structured
log output can help to increase the readability of a debug-log tremendously.

As an alternative to 'indentation', think about using the escape codes found in class
\ref alib::lox::ESC. Prefixing those instead of normal strings or spaces,
leads to nicely colorized or bold or italic log output, at least with text-loggers supporting such
styles. (\alox provides such \e Loggers e.g., for ANSI consoles or Windows OS command windows.)

### 7.4.3 Displaying Current Application States # {#alib_mod_alox_prefix_logables_usecases_appstate}
Use cases are depending on the application and situation. Let us touch
a last one here: Consider an application that causes errors in certain situations. Let's say, a
phone app seems to start logging errors 'randomly' which means, you do not know when. You suspect
it happens when the network connection drops. A first quick investigation could be to add
a <em>Prefix Logable</em> \b "Online:  ", respectively \b "Offline: " as soon as the devices' OS signals a change.
You simply set this using \e Scope::Global, or alternatively for the <em>Log Domain</em> where the error occurs.
In the next debug-runs, you have all messages prefixed
with the current state. You do not need to follow your log output \e 'backward' to find the most
recent log message giving you information about that status. Generally spoken: <em>Prefix Logables</em> allow
to add status information to log lines providing information collected elsewhere.

\I{################################################################################################}
### 7.4.4 Arbitrary Object Logging # {#alib_mod_alox_prefix_logables_usecases_binary}

The situation with \e Loggers designed to log arbitrary objects is different.
(How to create such custom, application specific \e Loggers is described in \ref alib_mod_alox_ext_loggers).

If only arbitrary objects were supported in \alox and the standard textual logging would not
exist as the predominant use-case, then the whole feature probably would have been named <b>Context Logables</b>.
Instead of providing the 'context' with each <em>Log Statement</em> to a custom \e Logger, or setting it explicitly
using a custom interface method of such custom \e Logger, arbitrary context data can be used
leveraging the various \e Scope options.

Imagine for example, a custom \e Logger that logs into a database. A 'context' in this case
could be the database table to use. <em>Log Statements</em> of different \e Scopes would then 'automatically' direct
their \e Logables to different tables in the database, if different <em>Prefix Logables</em>
had been set for the \e Scopes.

Another sample could be logging application metrics to an online metrics-server. The parameters
and information passed to the server are probably encoded in a URL. Now, the bigger parts of such
parameters do not change within a context (aka \e Scope). Those would be passed only once per
\e Scope to \alox using the feature of <em>Prefix Logables</em>. The metrics-<em>Log Statements</em> themselves would only carry the
rest of the detailed information specific to the metrics information that are supposed to be sent.

Use cases are endless and cannot be named here, they depend the field of application that
\alox is used to support.

\I{################################################################################################}
## 7.5 Lifecycle Management of Prefix Logables in ALox for C++# {#alib_mod_alox_prefix_logables_lifecycle}

One of the design goals of the \alox Logging Library is to avoid code clutter when using it.
In a perfect world, <em>Log Statements</em> would be as clear and easy to read as comment lines.
C++ does not provide life-cycle management for allocated data and this causes a potential problem
when using <em>Prefix Logables</em>.

When logging <b>arbitrary objects</b>, the use cases
touched in the previous section make it obvious that \alox cannot be responsible for life-cycle
management of <em>Prefix Logables</em>. Therefore, if data is used as <em>Prefix Logable</em> which is exclusively created for that
purpose (and are no general long-living objects), there is no way to avoid some extra
code that creates and deletes such objects, probably enclosed by

        #if defined(ALOX_DBG_LOG) // alternatively ALOX_REL_LOG, or both
        ...
        #endif

or embedded in macro

        Log_Prune( ... )    // alternatively Lox_Prune()

We think with release logging and binary object logging, both considered a 'heavy' use
of \alox anyhow, extra code should not be anything to be concerned about.

With <b>textual logging</b>, especially in the case of debug logging, this is different. Here,
the designers of \alox are concerned about extra code which increases the 'intrusiveness' of \alox!
Therefore, the following rule applies. For string-type \e Logables, precisely such of box types
\c nchar[], \c wchar[] and \c xchar[], \alox internally creates \b copy of the string provided.
Of course, when such <em>Prefix Logable</em> is removed, \alox deletes this internal buffer.
The benefit of this is huge: A user of \alox does not need to care about keeping string-type
<em>Prefix Logables</em> 'alive' after setting them. This means, any locally assembled,
short-living string can be passed to method \b %Lox::SetPrefix and right afterwards, it can be deleted
or removed by C++ from the stack if the corresponding C++ scope is left.

It is important to understand the impact:
- With string-type <em>Prefix Logables</em>, you do not need to worry about the life cycle of the string passed.
- With <em>Prefix Logables</em> of arbitrary type, it is the users' responsibility to keep objects intact
  as long as any <em>Log Statement</em> may be invoked that gets such <em>Prefix Logable</em> passed.
- Unlike, with \alox for \b C# and \b Java, setting an \b %AString as <em>Prefix Logable</em> and afterwards
  changing the instance, such change is \b not reflected in the prefix object! This is because
  the contents of the \b %AString is copied.

The latter is of course a disadvantage of this design: The <em>Prefix Logables</em> becomes a
static object that does not reflect changes of its origin object! But there is an easy way out.
Remember that only boxed objects of character array types are copied. The trick to have changes of an
\b %AString instantly reflected in the logging, is to pass it wrapped in an object of type
\c std::reference_wrapper. If this is done, the contents is \b not copied.
Instead, a reference to the \b %AString is boxed and any change of this object is reflected
in the <em>Prefix Logable</em>.

The following code and its output demonstrate what was just said:

  \snippet "ut_alox_dox.cpp"      Man_PREFIXLOGABLES_Lifecycle

  \verbinclude   pages/generated/ALoxMan_PREFIXLOGABLES_Lifecycle.txt



\note
  This approach is not only applicable to class \b %AString but to any custom string type
  that by default gets boxed to a character array. The only precondition is that along with the setup
  of module \alib_boxing in respect to the custom type, the type <b>%std::reference_wrapper\<CustomString\></b>
  has to be equipped with boxing interface \alib{boxing;FAppend}. <br>
  How to adopt custom string types to support %boxing, including this "trick" is described in the
  documentation of <b>%ALib %Boxing</b> in chapter
  \ref alib_boxing_strings "10. Boxing Character Strings".<br>
  Furthermore, in compatibility headers, the following functions are found which perform that
  task during bootstrap for 3rd-party libraries:
  - \ref alib::boxing::compatibility::std::BootstrapStdStringBoxing
  - \ref alib::boxing::compatibility::qt::BootstrapQTStringBoxing


\I{################################################################################################}
## 7.6 Wrap-Up # {#alib_mod_alox_prefix_logables_wrapup}

This is what this chapter has covered in respect to <em>Prefix Logables</em>:
- <em>Prefix Logables</em> are \e Logables that can be set according to the \e Scope mechanisms featured by \alox, or
  according to a <em>Log Domain</em>.
- With every <em>Log Statement</em> executed by \alox, all applicable <em>Prefix Logables</em> are collected in a list and passed to
  each \e Logger.
- The most prominent use case for <em>Prefix Logables</em> is adding a prefix, a color, or (optionally recursive)
  indentation to textual log messages.
- With custom \e Loggers using arbitrary types of \e Logables, the use cases are different but
  not less powerful. Consider the feature to be named <em>Context Logables</em> rather than
  <em>Prefix Logables</em>.
- In <b>%\alox for C++</b> a copy of any string-type <em>Prefix Logable</em> set is created. Therefore, a user must not worry about
  the life-cycle of such <em>Prefix Logables</em>. If arbitrary objects are used, the user of \alox has to ensure that
  <em>Prefix Logables</em> survive until the last corresponding <em>Log Statement</em> is executed.

As with other features using \alox \e Scopes, on the first sight, this seems to be
a little complicated. Especially when looking at the list given in chapter
\ref alib_mod_alox_prefix_logables_processing.
But when you look at the use cases, it becomes clear, that from the nine options of that list,
mostly one is used in parallel, seldom two.
Once the concept of <em>Scope Domains</em> is fully understood, the use of this feature
and of others that leverage \alox \e Scopes, should quickly become very intuitive.


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 8 Log Data (Debug Variables) # {#alib_mod_alox_log_data}

\I{################################################################################################}
## 8.1 Prerequisites # {#alib_mod_alox_log_data_prerequisites}

For a better understanding of what is explained in this chapter, it might be advisable to have read:
- Chapter \ref alib_mod_alox_scopes. In short, at that place it is explained how \alox defines
  language-related \e Scopes (including the 'global' \e Scope) and thread-related \e Scopes.
  Both types are 'interwoven' and form the complete set, as denoted in enum-class \alib{lox;Scope}

- Chapter \ref alib_mod_alox_log_once

\attention
  Both chapters are prerequisites for understanding what is explained here.
  Especially the use cases that arise from using and combining the different
  parameters of method \alib{lox;Lox::Once}, which are explained in a high degree of detail in
  the above mentioned chapter, is not repeated here. If all aspects of using \b %Lox::Once are
  understood, those can be quite easily adopted to what is described in this section!


\I{################################################################################################}
## 8.2 Introduction # {#alib_mod_alox_log_data_intro}

The concept of <em>Log Data</em> is a feature used for debugging. The goal that is aimed here
is similar to the goal of debug-<em>Log Statements</em> themselves. As a reminder, those are:
- Have a process log out debug messages, that help to understand software during
  the implementation phase.
- Avoid adding \b temporary 'debug output statements'. Instead, use \b permanent code, which
  can be disabled and preserved for later use, when needed.

Corresponding goals are aimed with <em>Log Data</em>. \alox provides this feature to enable
the insertion of variables and data into the code that otherwise would be temporarily
added for debug purposes and removed afterwards. The same as debug-logging is pruned from
release executables, such variables and their use gets pruned. Therefore, such variables and use
of them can stay in the code \e forever, and once a problem in the corresponding code section
appears, no need to reinvent temporary code is needed.

A typical example for using <em>Log Data</em> is debug log output written by an exception
handler. The code that may cause an exception, could store status information according to
\e %Scope::Method. When an exception occurs and the program execution abruptly continues within the
exception handler, such information can be retrieved and corresponding log output can be
generated giving more information about where and why the exception occurred.

\attention
  The whole concept of <em>Log Data</em> provided by \alox is merely a tool to support the process
  of debugging and debug code. Code using \alox <em>Log Data</em> should be pruned from release executables.
  In other words: It is not advised to use \alox Log Data to implement any functionality of
  an application, for example, storing thread-local data used outside the <em>Log Statements</em>.
  There are other, standardized, reliable, and efficient ways to store thread-local data.

\I{################################################################################################}
## 8.3 Using Log Data # {#alib_mod_alox_log_data_using}

The interface for setting and retrieving <em>Log Data</em> is provided with methods
\alib{lox;Lox::Store} and \alib{lox;Lox::Retrieve}.

The type of data stored is \alib{boxing;Box} in <b>%\alox for C++</b>. (What else did you expect?)

While in Chapter \ref alib_mod_alox_log_once of this manual, it was explained in detail how parameters
\p{group} and \p{scope} of method \alib{lox;Lox::Once} can be omitted, how one of them can be used, and what
happens when both are used, we do not want to repeat these details in this chapter.

Instead, we want to discuss the differences:
- Instead of storing a counter (in \b %Lox::Once), with \b %Lox::Store, the data object is stored.
- Parameter \p{group} of method \b %Lox::Once is named \p{key} in methods \b %Lox::Store / \b %Lox::Retrieve.
- If both parameters (\p{group} and \p{scope}) are omitted, \b %Lox::Once defaults to the 'most inner \e Scope possible'
  (by switching the \e %Scope::Filename and creating a unique key from the line number), which
  is the single log line itself. In contrast to this, \b %Lox::Store / \b %Lox::Retrieve in the
  parameterless version refers to the 'most outer \e Scope possible', hence a global singleton object
   (singleton in respect to the instance of class \b %Lox used).

As a result, the use of <em>Log Data</em> can be summarized as follows:
- An otherwise parameterless invocation the methods \b %Lox::Store / \b %Lox::Retrieve stores and retrieves
  a global anonymous <em>Log Data</em> object.
- If parameter \p{key} is provided while parameter \p{scope} is omitted (or set to \e %Scope::Global),
  a named <em>Log Data</em> object is stored and can be retrieved using the same key.
- If parameter \p{scope} is provided while parameter \p{key} is omitted (or nulled or empty),
  an anonymous <em>Log Data</em> object is stored and can be retrieved (only) from within the same
  \p{Scope} as they were set.
- If both parameters, \p{key} and \p{scope} are provided, then the key is bound to the provided
  \e Scope and for this scope a named <em>Log Data</em> object is stored. Consequently, it can be
  retrieved (only) from within the same \p{Scope} as it was set and with using the same \p{key}.

For clarification, this means, that different <em>Log Data</em> objects are stored in different scopes
even if parameters \p{scope} and \p{key} are the same. For example,
an object stored using \p{Scope}.Method cannot be retrieved from a different method and will also not be overwritten
within a different method. Again, the consequences and use cases of this design may become
clearer when reading chapter \ref alib_mod_alox_log_once "Lox::Once".


\I{################################################################################################}
## 8.4 Lifecycle Management of Log Data+# {#alib_mod_alox_log_data_lifecycle}
It is important to understand that in \alox for C++, <em>Log Data</em> objects have to
be kept in memory, if they are passed as pointers. The question if data is passed as a pointer
or as a value is answered in the Programmer's Manual of module \alib_boxing.

In short, it can be said, that typically all fundamental C++ types (\c int, \c char, \c double, etc.) as
well as pointers to those, are stored as values. The same is true for supported string types.
However, in the case of strings, the "value" passed is the pointer to the start of the string in
the memory, along with its length. Therefore, this memory has to be kept valid.
In the case of storing an \b %AString, the memory stored might become invalid, if the string
is extended after it was stored. Be sure to understand this constraint when using this feature.


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 9 Multithreaded Processes # {#alib_mod_alox_multi_threaded}

\I{################################################################################################}
## 9.1 Introduction # {#alib_mod_alox_multi_threaded_intro}

What does "logging" have to do with multithreaded applications? Quite a bit, as we will see
in this chapter.

\alox is using C++ <c>std::thread</c> library to identify threads through dependent module
\alib_threads. Basically, that module provides just a little wrapping code around what C++ offers
since language version C++ 11:
- classes \b %Thread and \b %Runnable which have a similar interface to the classes found with
  the Java language.
- Each \b %Thread instance created by the module is hashed in a static table for future reference.
- Static method \b %Threads::CurrentThread uses <b>std::this_thread::get_id()</b> to identify
  the current thread. If no corresponding \b %Thread instance is found in the aforementioned
  static table of threads, then a new object is created, added to the table, and returned.

As a consequence, \alox detects all those threads rightfully that are detected (detectable!) by
<b>std::this_thread::get_id()</b>. In general this works with all major combinations of modern
operating systems, C++ libraries and thread libraries. Should threads not be detected, check
out whether the thread library you are using is compatible with thread tools found with C++
library \c "std".
The other way round: if you are using a threading library that creates threads that are detected
by C++ \c "std", then you can use \alox thread features without any further action.
If not, you should consider either to switch your software to usign something that is compatible with
this modern standard, or use the thread classes provided with \alib (if you use \alox, you have
\alib available). But remember the latter is a quite simplified limited piece of art - enough for
simple applications, but not more!

\I{################################################################################################}
## 9.2 Mapping Thread Names # {#alib_mod_alox_multi_threaded_mapping}
\alox collects information about the thread that was executing a <em>Log Statement</em>. For textual
logging, struct \alib{lox::textlogger;FormatMetaInfo}, which is a
\ref alib_mod_variables "configuration variable" and this way configurable from outside,
contains information on how and where class \alib{lox::textlogger;TextLogger}, writes the
name of the executing thread by default. This default is defined with substring "%tN" in field
\alib{lox::textlogger::FormatMetaInfo;Format} of that variable. Now, if you want to change the name (without
changing your applications' true thread name), then method \alib{lox;Lox::MapThreadName} does the
job. With this method, you do not rename a thread, you just tell the \b %Lox to provide a different
name to the \e %Loggers in future <em>Log Statements</em>. Thread names can be mapped for the
current thread or for threads passed via optional parameter \p{thread}, respectively \p{id}.

A sample for using this method is given in the tutorial chapter
\ref alox_tutcpp_threadnames "14. Name Your Threads".

\I{################################################################################################}
## 9.3 Thread-Related Scopes # {#alib_mod_alox_multi_threaded_scopes}

With the fundamental concept of having \e %Scopes in \alox, and in particular with the fact
that \alox "interweaves" so-called \b %Scope::ThreadInner and \b %Scope::ThreadOuter with other
language-related scopes (e.g., \b %Scope::Method) and the global \e Scope, \alox supports much more
of multithreaded applications than just mapping new names to threads!

This is a complex topic and there is a whole bunch of chapters we have to refer you to:
- \ref alib_mod_alox_scopes
- \ref alib_mod_alox_log_once,
- \ref alib_mod_alox_prefix_logables and
- \ref alib_mod_alox_log_data.

To name just a few "applications" of the features described in the chapters above:

- Execute a <em>Log Statement</em> only if executed (or just not executed) by a certain thread.
- Separate <em>Log Statements</em> depending on the thread they are executed by, and control the \e Verbosity
  independently from each other. By mixing thread-related <em>Scope Domains</em> and language related <em>Scope Domains</em> with each other,
  a user of \alox is enabled to fine-tune the log output very granular, even for logging code that
  he has no access to.
- Execute <em>Log Statements</em> once per thread.
- Assign prefix strings (respectively arbitrary \e Logables) to each <em>Log Statement</em> executed by a certain thread.
- Store and retrieve named <em>Log Domain</em> objects (debug-information) associated with threads.


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 10 Differences of Debug- and Release-Logging # {#alib_mod_alox_debug_and_release_logging}

The \ref alib_alox_tut and most sample code in this manual, provide information about <em>debug logging</em>.
The main goal of this chapter is to explain how to implement <em>release logging</em>. This might be
easiest to explain when comparing both types of logging and listing the differences.
(For a general definition see \ref alib_alox_faq_debug_vs_release_logging).

As previous chapters described, any logging with \alox is roughly performed as follows:
- A dedicated instance of class \alib{lox;Lox} is created.
- This \b %Lox object is configured, mainly by populating it with one or more <em>Loggers</em> which get
  \e Verbosity set for different <em>Log Domains</em>.
- Then, anywhere in the source code, <em>Log Statements</em> which perform the logging exclusively
  through the interface of that dedicated object of class \b Lox are inserted.

## 10.1 Debug Logging ## {#alib_mod_alox_debug_and_release_logging_dbg}

\note
  A-Worx (the maker of \alox) is a consultant to the software industry and one area of
  competence is code style and code cleanness.
  We emphasize our clients to stop using *temporary debug print lines* in
  their sources. Instead, using \alox (or a similar tool) all debug output lines should
  be implemented tidy and clean using a nice language for the messages. With \alox, such debug output should
  never be temporary again and as such never be removed from the code! Programmers often remove
  debug output after they think a piece of code works. But if problems arise or if the code
  is further extended at a later stage, similar lines are inserted and re-written.
  This is obviously a waste of time. With \alox, you just disable certain <em>Log Domains</em> for
  which you think the code is fine and be able to reactivate them (with different verbose levels)
  at the moment your interest moves back to it!

Now, with <em>debug logging</em>, there are some specialties that do not apply in release logging:
1. A dedicated, pre-created, static object of class \b %Lox is provided for the purpose
   of <em>debug logging</em>, which is publicly accessible from virtually anywhere in the code (including
   external library components).

2. For being able to \b prune <em>debug logging</em> statements from the code, the interface
   to this dedicated \b %Lox singleton is slightly different than just using the interface of
   the \b Lox object itself. An own set of preprocessor macros for each type of logging exist
   (\ref GrpALibPreproMacros "documented here").
   The almost only difference between the two sets is that those macros used for <em>debug logging</em>
   (that log into the debug-Lox singleton and get pruned in release compilations),
   are prefixed <em>Log_</em>, while those that are used for release logging are prefixed <em>Lox_</em>.

3. For convenience, \alox does not only provide a pre-configured \b Lox singleton for <em>debug logging</em>,
   but it also creates an appropriate debug \e Logger (or even two of them for certain IDEs) in the case
   that no other \e Logger was explicitly created before invoking the first log call.<br>
   This way, \alox allows starting to use <em>debug logging</em> with no <em>'bootstrap efforts'</em>
   as shown in \ref alox_tutcpp_hello_alox "Tutorial: Hello ALox".


## 10.2 Release Logging ## {#alib_mod_alox_debug_and_release_logging_rel}
From the previous discussion about the specialties of <em>debug logging</em>, we can now quite easily
identify how release logging differs and derive a guideline on how to implement it:

1. An object of type \alib{lox;Lox} has to be created.
   (In more complex scenarios two or even more of such objects might be created
   and used for different use cases of <em>release logging</em>.)<br>
   It has to be ensured that each code entity that should be enabled perform
   <em>release logging</em> on this \b Lox object has proper access to it.

2. One or more objects derived from (abstract) type \alib{lox;detail::Logger} have to be created and attached to the
   release-Lox object(s), usually with setting the right <em>Verbosity</em> for different
   <em>Log Domains</em>.<br>
   (Both actions are usually performed at the beginning of the life-cycle of software process, what
   we call the <em>bootstrapping section</em> of software.)

3. The interface of the lox object itself is directly used for logging. The <em>Log Statements</em>
   work and look similar to <em>debug logging</em>, because as already explained above, an own set
   of preprocessor macros each type of logging exist that differ only in their prefixes <em>Log_</em>
   vs. <em>Lox_</em> (see \ref GrpALibPreproMacros).<br>
   Each code entity has to set the preprocessor symbol \ref LOX_LOX before using
   <em>release logging</em> macros, because this symbol is used inside all preprocessor macros.
   This can be done in a general header-file of the software, (e.g., the same that exposes the
   release-Lox object to that source), or, in more complex scenarios with more than one
   release-Lox object, at any other appropriate source location.

4. By default, the language-related \e Scopes are not usable with release logging.
   It is elaborated in \ref alib_mod_alox_scopes_language "Language-Related Scopes",
   why this is not considered as a big penalty.
   Release executables just should not contain information about source code directories and files.
   For security reasons and to protect a companies or programmers intellectual property.<br>
   When the inclusion of caller information with release logging is activated (see
   chapter \ref alib_mod_alox_debug_and_release_logging_ft_1), then the scopes are fully usable.
                                   

## 10.3 Further Thoughts ## {#alib_mod_alox_debug_and_release_logging_ft}
### 10.3.1 Pruning Release Logging ### {#alib_mod_alox_debug_and_release_logging_ft_1}
Probably just because it was easy to implement (!), the C++ version of \alox supports the optional
pruning of <em>release logging</em> statements as well (see \ref ALOX_REL_LOG).
This might sound confusing in the first moment, but it allows creating different
versions of release-executables, which is not completely uncommon. It is optional and just provides
a next level of flexibility to adopt \alox to complex use cases.

In contrast to this, \alox also allows including caller information with <em>release logging</em>
statements, by passing symbol \ref ALOX_REL_LOG_CI to the build system. With that,
information about source file names, their location, and function and method names will be
included in the release executable. As this is usually not wanted, such setting is not defaultet.
However, if done, language-related scopes can be used with <em>release logging</em>.



### 10.3.2 More Complex Use Cases ### {#alib_mod_alox_debug_and_release_logging_ft_2}
\alox allows usage in various fashions. By having the concept of different \e %Loggers aggregated
in one or more \b Lox objects, using an hierarchical structure of <em>Log Domains</em>, which by
default inherit their \e Verbosity from their parent domain
(and this way automatically adjust to probably unknown Subdomains), we think that a good
balance of "ease of use" and "unlimited extensibility" was found with the \alox architecture.

The simple and common scenarios incorporate:
- Debug logging with
    - An appropriate <b>Console Logger</b> and probably an IDE specific \e Logger when run in
      an debugger
    - Optionally a simple text \e Logger that allows reviewing the output of 'historic' debug sessions
    - A few root-level <em>Log Domains</em> like "UI", "TCPIP", "DB", "IO", etc. having a maximum of one
      subdomains
- Optionally release logging e.g., for collecting severe errors or collecting statistical summaries
  from 'the field'

These scenarios can be addressed with a few lines of \e bootstrap code and a little discipline
when inserting the <em>Log Statements</em>.

Various ways of matching use cases of complex scenarios with \alox exist.
By purpose (to keep things simple) \alox is not providing extended classes, for example, to
allow pruning release logging as explained in the 'recipes' of the previous paragraph.
However, once the basic concepts of \alox are understood, it is easy to build such extensions,
tailored to the complex case itself.


### 10.3.3 Using ALox and Libraries ### {#alib_mod_alox_debug_and_release_logging_ft_3}
Using \alox with libraries, is basically the same as using \alox in a single code entity. However,
we recommend the following principles:
- For <em>release  logging</em>, a library should provide an initialization method that receives a reference
  to the dedicated release-Lox instance it is supposed to log into. If such object is not
  provided (which means the library is not duly initialized), the library should use \alox
  <em>debug logging</em> to notify that.
- A library should not create and set any \e %Loggers by itself. The creation of \e %Loggers should
  generally be controlled by the main source entity of a process.
- A library should document which <em>Log Domains</em> it is using. Furthermore it is helpful to
  also disclose which <em>Verbosity</em> is used by <em>Log Statements</em> per domain and maybe
  roughly what log output might be expected. This is especially important for rather uncommon log
  messages, like severe warnings and errors.
- A library should expose a root-level <em>Log Domain</em> with optionally different Subdomains.
- A library should (by nature of all library code) make thoughtful and reasonable use of combinations
  <em>Verbosity</em> and <em>Log Domains</em>. This allows controlling the verbosity of the library
  in granular way.


### 10.3.4 Logging ALib Assertions ### {#alib_mod_alox_debug_and_release_logging_ft_4}
\alib, the general purpose C++ library that incorporates \alox as one of many modules, provides a
concept for raising assertions, warnings, and debug-messages.
This mechanism is used to report problems in debug-versions of \alib and may optionally used by
of software that uses \alib.
With function \alib{lox::ALoxAssertionPlugin}, \alox implements the provided plug-in mechanism to
direct the textual output to an \alox \e %Logger.
This is all transparently set up and destructed with methods \alib{lox;Log::AddDebugLogger} and
\alib{lox;Log::RemoveDebugLogger}.

Applications that do not use debug logging may want to use method
\alib{lox;Log::SetALibAssertionPlugin} to add and remove the plugin "manually" with
debug-builds.

It is important to set the verbosity of the domains used to a proper level, to enable the logging
of assertions, which are logged to a subdomain of the domain given with namespace variable
\alib{lox;ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX}. (This variable may be changed by the using code.)
                                                                                     
The following code samples a proper bootstrap using the release-logger.:

\snippet "ut_alox_dox.cpp"     DOX_ALOX_SET_ASSERTION_PLUGIN

\note
  <b>\alox for C++</b> does not provide a corresponding preprocessor macro.
  Instead, macro \b Log_Prune (not \b %Lox_Prune!) should be used to perform the invocations
  with debug-builds.
  
Upon termination, the plug-in is to be removed as sampled here:
  \snippet "ut_alox_dox.cpp"     DOX_ALOX_SET_ASSERTION_PLUGIN_2


### 10.3.5 Other differences of Debug and Release Logging ### {#alib_mod_alox_debug_and_release_logging_ft_5}

Appendix reference chapter \ref alib_mod_alox_apdx_auto_config might hint to other differences of
debug and release logging.


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 11 Internal Logging # {#alib_mod_alox_internal_domains}

Well, it seems like a miracle that C++ compilers are written in C++ and looking behind the
scenes it is really fine piece of <em>software art</em> that you discover when start
bootstrapping such a system from scratch!

Well, with \alox it was much more simple: We implemented \alox and when this was done, we just
afterwards added \alox Log lines into \alox code. Easy!

So, this chapter explains how these internal logs are organized and how an \alox user can
work with them. A first introduction to this is given in the language-specific tutorials,
chapter \ref alox_tutcpp_config "16. ALox Configuration Information and Internal Log Messages".

\I{################################################################################################}
## 11.1 The Internal Domain Tree # {#alib_mod_alox_internal_domains_tree}
<em>Log Domains</em> are hierarchically organized in \alox. All <em>Log Domains</em> together
constitute a tree of <em>Log Domains</em>. Class \alib{lox;Lox} maintains the tree, inserts
<em>Log Domains</em> 'on the fly' and manages the \e Verbosity on a per <em>Log Domain</em> and
\e %Logger basis.

For internal <em>Log Statements</em>, \alox uses a second, separate domain tree. The separation is made to keep the
main tree of <em>Log Domains</em> 'free' for custom domains. If this was different, then setting the root domain
as in the following sample:

\snippet "ut_alox_dox.cpp"        Man_InternalDomains___1

would affect internal log messages as well. Because of the separation, they are not.
To set or modify the \e Verbosity of internal <em>Log Domains</em>, static field
\alib{lox::Lox::InternalDomains} has to be used as follows:

\snippet "ut_alox_dox.cpp"        Man_InternalDomains___2

When you check out the reference documentation as linked above, you will see that this field
is a simple string \c "$/". Therefore, the code:

\snippet "ut_alox_dox.cpp"        Man_InternalDomains___3

is equivalent to the previous line.

\note Of course it is advised to use the static field instead of 'hard-coding' \c "$/" to
      be compatible with future versions of \alox (...or at least get a compiler error if this
      would be changed to something different than a string code.).

This piece of information is all you need to control \e Verbosity of \alox internal messages.

Please Note that method \alib{lox;Log::AddDebugLogger} sets the \e %Verbosity.Warning for
the internal domain. This way, when using \alox, your debug-logger will show you errors
and warnings if something goes wrong inside of \alox (e.g., when you pass wrong parameters
to \alox, etc.).

\I{################################################################################################}
## 11.2 Subdomains used by ALox # {#alib_mod_alox_internal_domains_sub}

For different topics, \alox uses different Subdomains for its internal logging. As usual,
this allows controlling the \e Verbosity of the internal log output more granular then by setting
the internal root domain \c "$/". For example, if software uses wrong <em>Log Domains</em> evaluated from a more
complex configuration of <em>Scope Domains</em>, then, to investigate into the problem,
subdomain <c>'DMN'</c> could be set to more verbose, while
suppressing other internal messages:

\snippet "ut_alox_dox.cpp"        Man_InternalDomains___4


The list of Subdomains used by \alox is given in the following table:

Subdomain | Description
- - - - - -| - - - - - - - - - - - - - - - - - - -
LGR        | Used when \e %Loggers are registered, retrieved or removed from a \b %Lox and when the \e Verbosity of a <em>Log Domain</em> for a \e %Logger is changed.<br>In addition used with method \b %Lox::SetStartTime.
DMN        | Used when <em>Log Domains</em> are registered (on first use), when <em>Scope Domains</em> are set or removed and when <em>Domain Substitution Rules</em> are set.
PFX        | Used when <em>Prefix Logables</em> are set or removed.
THR        | Used with method \b %Lox::MapThreadName.
LGD        | Used with storing and retrieving <em>Log Data</em> objects.
VAR        | Used when parsing and processing external variables.


\I{################################################################################################}
## 11.3 External Use of Internal Domains ## {#alib_mod_alox_internal_domains_external}
It is not 'forbidden' to have custom <em>Log Statements</em> using the internal domain tree:
\snippet "ut_alox_dox.cpp"        Man_InternalDomains_ext___3

Of course it is advisable to do so only if there are good reasons for that.
For example, a user might want to redirect \alib_assertions output to the internal domain tree.
(Note that such output is performed with function \alib{lox;ALoxAssertionPlugin} as it is
described in the previous chapter \ref alib_mod_alox_debug_and_release_logging_ft_4.)
                       
This could be done by changing namespace variable \alib{lox::ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX},
for example, to the internal domain <c>"$/ALIB"</c>.
This way, internal \alib_assertions and warnings are directed to the same domain tree as internal
\alox logging, hence all internal logging of \alibmods would then go to the internal tree.

\I{################################################################################################}
## 11.4 Final Remarks # {#alib_mod_alox_internal_domains_remarks}

It is absolutely OK, to register a \e %Logger exclusively for internal domains. Methods
\alib{lox;Lox::RemoveLogger} always removes the logger from both, internal and standard
<em>Log Domains</em>.

As an alternative to internal logging, method \alib{lox;Lox::State} provides an actual snapshot of
all (or selected) settings made for a \b %Lox and its \e %Loggers attached.
Depending on the situation, each way to disclose what is going on inside \alox might help
when something is irritating. As an alternative to (temporarily) adding an invocation of
<b>%Lox::State</b> to your code, \alox provides configuration variable
\ref alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT. This allows enabling an automatic dump of the state using a
command line parameter or other external configuration variable sources.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 12 External Configuration # {#alib_mod_alox_external_configuration}

\I{################################################################################################}
## 12.1 Introduction # {#alib_mod_alox_external_configuration_intro}

With the term <em>"external configuration"</em>, we are addressing any type of configuration
data that an application might have access to. The most common types of such configuration data
are:
- Command line parameters
- Environment variables and
- Configuration files, e.g., "INI-files"

For reading such configuration data, \alox relies on the facilities provided by underlying
module \alib_variables. It is advisable to read its Programmer's Manual.

<b>In short, the features of the module \alib_variables_nl are:</b>

- A public instance of class \alib{variables;Configuration} is found in field \alib{camp;Camp::config}
  and with that in singleton \ref alib::ALOX, which is derived from class \b Camp.
  \alox always tries to read configuration data from this object. If no data is found, it falls
  back to defaults.
- Class \b %Configuration supports \b 'plug-ins' that read configuration data from different sources
  at the moment a variable is requested. Default plug-ins are those for reading
  <b>command-line parameters</b> and <b>environment variables</b>.
- In addition, a class to import and export INI-files is provided with \alib{variables;IniFileFeeder}.<br>
  Custom configuration standards and sources can be supported using the API that module
  \alib_variables offers.
- Different configuration data sources have different \b priority and may thus overwrite each other.
  For example, command-line parameters have a higher priority than variables
  defined in an INI-files. This way, command-line parameters 'overwrite' INI-file entries.
- Variables can contain references to other variables. This concept named <b> variable substitution</b>,
  allows even more flexible configuration possibilities .

\I{################################################################################################}
## 12.2 Relationship of ALox, Applications and Configuration Data # {#alib_mod_alox_external_configuration_relationship}

It may be very obvious to the reader, but lets spend a minute and think about the relationship
of \alox and applications using it.

- \alox is embedded as a library in applications, they live in one process.
- \alox longs to read external configuration data
- The application itself probably does this as well and probably disposes of a system to maintain and read such
  data already
- \alox may want to write default values into the configuration. (If a variable was not found, the
  variable should be written, so that a user sees it and learns that is there and can be modified!)


In most use cases, the goal now is to enable \alox to access (read and write) the configuration that is
already in place with the application. The huge benefit of this approach is, that no second configuration file or
database is needed. And this is exactly what \alib - and therewith \alox - intends to achieve.

Then, other use cases and variants are:
- Having separated configuration systems for the application and \alox. For documentation on
  how to do this, see chapter \ref alib_mod_bs_customize_resconf of the
  \alib Programmer's Manual.
- Have \b no configuration system for \alox: The most obvious reason why this might be wanted is that a
  developer of an \alox enabled software might not want to allow the end user to reconfigure \alox (i.e. switching
  a \e Logger off that collects telemetry data).
- A mixed approach: Some \alox features should be configurable (from either configuration system) while
  other features should not be configurable from outside.
- Similar to this, some default values (that \alox creates on the fly) should automatically appear and be saved in
  the configuration. The benefit of this is that the user gets a fully filled configuration with maybe
  automatically commented entries that hints him/her to what is adjustable - all without reading manuals.
  Other of such \alox options probably should not automatically be saved. If not saved, these are regenerated as
  default values on the next run and again not be saved. A simple reason to disallow saving default values
  might be to avoid that the external configuration is being 'bloated' with \alox options that the user of an application is not
  interested in.

Finally, as \alox is a library, there might be more than one \alox enabled software running on the
same system. Now, for configuration sources like INI-files and command line parameters, this is
not problematic. In the case of environment variables, it might become a problem: A variable
setting here could influence more than one application if variables are not renamed on a per
application basis.


\I{################################################################################################}
## 12.3 How ALox Uses the ALib Variable System # {#alib_mod_alox_external_configuration_alox}

\note For details consult the \ref alib_mod_variables "Programmer's Manual of camp ALib Config".
All configuration variables are declared in one place with enumeration \alib{lox;Variables}.

Module \alib_variables_nl imposes a data "contract" that asks a programmer to respect priorities of
variables and write values only if permission is granted. Of course, as \alox is a sibling camp,
it uses the configuration system exactly as this contract expects.
In particular, when using interface method \alib{variables;Variable::Define} to write configuration
data, values are only written with standard priority.

Furthermore, \alox leverages the flexibility that the priorities of the \alib variable
system brings by exposing parameter \p{priority} in methods \alib{lox;Lox::SetVerbosity}
and \alib{lox;Lox::SetSourcePathTrimRule}. This way, settings made 'in code' are by default
overruled by any type of external configuration. If wanted, a programmer may specify a higher
priority and this way either allow only certain, higher prioritized external sources to overwrite
"hard-coded" values, or just completely disallow any external change.

<b>For the user of \alox, the benefits of this approach towards external configuration data are:</b>

- \alox does not impose new requirements to module \alib_variables_nl. In respect to the configuration
  sources installed, everything remains untouched.
- \alib Because all variables are using \ref alib_variables_declaration_decl_resourced "resourced declarations",
  a user of the library may modify variables in the following ways:
  - Variable names can be changed
  - Category names can be changed: either for a subset of the variables or for all.
  - Default values of \alox variables can be changed
  - Variable comments can be changed. Especially, those that may come from a pool of externalized strings.
- Single \alox variables (or all) can be excluded from the possibility to be modified from outside.
- \alox will not bloat an applications' external configuration system with own variables, unless
  explicitly demanded:
  - An application may also choose to expose only selected variables to the outside.
  - An application may store \alib and \alox configuration data in a configuration file which is
    separated from its own configuration. (See \ref alib_mod_bs_customize_resconf)
- A custom configuration plug-in will not only work in respect to prioritizing values of external
  variables, but also in respect to consistency with \alox verbosity settings.


\I{################################################################################################}
## 12.4 Concrete Steps on Integrating ALox Configuration in your Software# {#alib_mod_alox_external_configuration_concrete}

After the discussion of the previous sections it should be quite straightforward to understand how
\alox is to be integrated to your software in respect to external configuration data.
Here are some hints:

- Follow the steps provided in chapter \ref alib_variables_external to have your configuration
  system feeding its values into the configuration provided with module \alib_variables_nl.

- Alternatively: Use built-in mechanics, for example, class \alib{variables;IniFileFeeder}.

- During \ref alib_mod_bs "bootstrapping of ALib", patch the resources of
  module \alib_alox, to change variable names to match you application name or shortcut.
  This is needed to avoid clashing of environment variables when two \alib enabled applications exist.

- If wanted, change the resourced names and default values of the
  \alib{lox;Variables;various configuration variables}.

- Programatically set values that result from own application logic.
  A simple sample is command line parameter <em>\-\-verbose</em> which might lead to
  'programmatically' change verbosities of various domains at once.

- Protect values that must not be changed from outside by defining variables with priority
  \alib{variables;Priority;Priority::Protected}. Do this during or right after bootstrap, before
  variables are used.

- Apply the previous two steps to one variable to redefine configuration behavior.
  Let's stick to the sample of having the <em>\-\-verbose</em> switch. Imagine this option
  should get three possible values: 'silent', 'normal' and 'verbose'. The approach to take
  is the following:
  - programmatically add three new custom variables stored as protected values.
    Their names might be <c>MYAPP_VERBOSITY_1[2|3]</c> .
    These custom variables contain the verbosity settings for all <em>Log Domains</em>
    for each of the three switchable verbosities
  - Depending on the command line switch, set the value of variable \alib{lox::Variables;VERBOSITY}
    to <c>"${MYAPP_VERBOSITY_1[2|3]}"</c>, also in protected mode.
    With \c "${}", a portion of a variable value
    \ref alib_variables_values_substitution "can be substituted" with the contents of another
    named variable.

  What is achieved here, is that there are custom 'presets' defined for the verbosities
  and those are all that the end-user can set! <br>

  Furthermore, if the substitution would be set to
  <c>"${MYAPP_UNDOCUMENTED_SWITCH} ; ${MYAPP_VERBOSITY_1[2|3]}"</c> then, in addition, a
  hidden new configuration variable is exposed. This might be used in "the field" when
  something goes wrong with a deployed application and more logging is wanted.

- Finally, before the termination of a process, copy those auto-generated \alox variables
  that you are interested in to your write-enabled configuration plug-in.
  This way, the user sees variables with comments and learns how to leverage those.<br>
  Built-in type \alib{variables;IniFileFeeder} provides export methods that do exactly this.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 13 External Verbosity Configuration # {#alib_mod_alox_external_verbosity_configuration}

\I{################################################################################################}
## 13.1 Introduction # {#alib_mod_alox_external_verbosity_configuration_intro}

This Chapter brings us back to talking about <em>Log Domains</em> and their attribute <em>Verbosity</em>
which are associated with pairs of a <em>Log Domains</em> and a \e %Logger.
\note
  Please make sure that you have read and understood \ref alib_mod_alox_logdomains before continue reading.

Setting such \e Verbosity by invoking \alib{lox;Lox::SetVerbosity} is easy and straightforward.
However, when working in a team, different team members, working on different parts of software,
naturally are interested in different information. Now, when the \e Verbosity is defined in the source
(preferably the bootstrap section of software), then these would be randomly changed when
checking source files in and out of a source control system.
Therefore, \alox supports to read \e Verbosity settings from the command line, from environment
variables, from an INI-file or from any other custom data source. This way, all personal changes
are set outside the code - good for your team spirit!

But before we can explain how this is done, we first have to talk about <b>priorities of
Verbosity settings</b>. This is an important prerequisite to understanding how
external configuration works in \alox.


\I{################################################################################################}
## 13.2 Priority of Verbosity Settings # {#alib_mod_alox_external_verbosity_configuration_prios}

In chapter \ref alib_mod_alox_logdomains of the \alox tutorial and elsewhere in the \alox documentation,
we have just not mentioned optional parameter \p{priority} of the overloaded set of methods \alib{lox;Lox::SetVerbosity}.
It was silently omitted, because this parameter brings a new level of complexity to the table.
The good news is that (as explained in \ref alib_mod_alox_apdx_auto_config) when the parameter
is omitted, \alox behaves as if this feature was not existing.

So, what can we do with it? The parameter is of enum type \alib{variables::Priority}. From the
namespace you can already tell that we are using an enumeration of sibling module \alib_variables here!
The parameter defaults to its enum element \alib{variables;Priority;Priority::Standard}.
This tells us something: If we do not touch the parameter, the domain is set with only a
priority a little higher than the priority of "default values". That sounds like a quite low
priority, doesn't it?

When passing a higher value here by just adding <c>+ 1</c> to the constant, subsequent invocations
for the <em>Log Domain</em> in question will be ignored. Let's look at the following sample:

\snippet "ut_alox_dox.cpp"        MAN_EXT_VERBOSITY_VARIABLES_1

And its output:

\verbinclude   pages/generated/ALoxMan_ExtVerbosityConfig_1.txt

As you see, the fourth invocation of \b %Lox::SetVerbosity is ignored, because the third had a
higher priority. Also, if a setting was given on parent domain <c>"/"</c> with standard priority
now, all other Subdomains were changed, but <c>"/MYDOM"</c> and its Subdomains would keep their
setting.

With \alox internal logging (see \ref alib_mod_alox_internal_domains)
activated, \alox nicely reports what is going on. We add line:

\snippet "ut_alox_dox.cpp"        MAN_EXT_VERBOSITY_VARIABLES_2

to the top of the sample. Now the output is:

\verbinclude   pages/generated/ALoxMan_ExtVerbosityConfig_2.txt

\alox in its internal log messages tells us explicitly, that the fourth request was ignored due to the
higher priority level that the domain setting had before!

Consider the following <em>Log Domain</em> tree:

        /UI
        /UI/MOUSE
        /UI/DIALOGS
        /UI/DIALOGS/MOUSE
        /UI/DIALOGS/KEYS

Imagine you are currently frequently switching the \e Verbosity of Subdomains of <em>Log Domain</em> <c>'/UI'</c>
because you are currently working on the UI. Now you have a random problem with mouse event
handling. As long as this problem occurs, <em>Log Statements</em> related to mouse event should stay \c Verbose.
Therefore, you enable them 'permanently' by invoking

\snippet "ut_alox_dox.cpp"        MAN_EXT_VERBOSITY_VARIABLES_3

After that, any changes in parent domains, even for root-domain, will not disable <em>Log Statements</em> of
mouse events anymore. When the mouse event handling is fixed, these settings can be deleted
again.

\note
  Once a priority of a \e Verbosity setting was increased, for the rest of the life-cycle
  of a process, this priority cannot be changed back to a lower one. \alox does not provide
  an interface for that. The reason is simply, that the designers of \alox do not see any need
  in it. Honestly, if it was not so helpful for external configuration, the priority-feature
  of \e Verbosity settings would not even have been implemented with \alox.

\I{################################################################################################}
## 13.3 Priority of External Configuration # {#alib_mod_alox_external_verbosity_configuration_configprios}

We learned two fundamantel things from the previous chapter:
1. Using hidden parameter \p{priority} with \e Verbosity settings, allows \alox to ignore
   verbosity settings, in case a previous setting was made with a higher priority already.
2. The priority enumeration used, stems from sibling camp \alib_variables.

Next, we take a look at the other enumeration elements of \alib{variables::Priority}.
(Please do this now and return back to here.)

With this prioritization, module \alib_variables_nl provides a very natural behavior for choosing
\alox <em>Verbosities</em>, dependent on the source of information.
This should be quickly discussed on a sample:
- A team works together on different areas of one executable software.
- Each team member adds "hard-coded" verbosity settings to the code, during the day, just as
  he needs and wishes.
- Next morning, when everybody checks out the current codebase, all team members get theses
  hard coded domain settings. However: Their logging does not change, because they have either
  - a private INI-file, that is not managed by the source control system.
  - a second INI-file, which in contrast to the one which is updated with the source control system
    is private and attached to the \alib_variables_nl with a higher priority (e.g., 20000+1) than the
    normal INI-file
  - an environment variable set, which overrules the INI-File coming from the source control system.
  - command line parameters set, for example, in the IDE settings of the debug-configuration

  All of these options, prevent that other team members are bothered with hard-coded domain settings.
  Of course, it is recommendable that every team member anyhow uses a "private" method to change
  verbosity settings, instead of hard coding them. (But we all know how it is in practice.)
- What can also happen: Next morning everybody checks out the current codebase and every team member
  sees some new log statements in a new <em>Log Domain</em>, that not existed the day before.
  OK, here everybody is bothered with log entries that he is not interested in. But these new
  domains will automatically show up in the INI-File after the first run. (This is shown below.)
  This way, it is easy for all team members to quickly shut these domains off, without touching the
  code. All that they have to do is open the INI-file, search for the newly created domain name
  which automatically appeared in their INI-file, and change the setting down to "Off",
  "Error" or "Warning".

There are many other use cases. Also for <em>release logging</em>. Imagine a situation where
a remote customer has a problem. He sends you a log-file of the problemeatic run.
Unfortunately, the log file misses important verbose <em>Log Statements</em>. You send the customer
an email and ask him to run the software again, but providing a command line parameter that
enables verbose logging for a tree of domains...

This manual stops here with use cases. They are really huge. The "marriage" of sibling modules
\alib_variables_nl with \alib_alox_nl through hidden parameter \p{priority} of method
\alib{lox;Lox::SetVerbosity}, which is directly "lent" from the configuration system,
just heavily pays off!<br>

\note
  In case you want to disable the whole thing and just deny changes made to a certain
  <em>Log Domain</em> by any user of your software, you can do this easily:
  Just add a hard-coded call to \alib{lox;Lox::SetVerbosity} providing the hidden
  parameter \p{priority} with level \alib{variables;Priority;Protected}.
  From there on, from nowhere lese but from within the source code a future change can be
  made. Only a next statement with the same maximum priority can overrule this setting.<br>

\note
  While hard-coding things is generally bad, there are use cases for it: Logging may become a
  critical key feature of an application. For example, when collecting metrics from the field with
  deployed applications. In this case, a programmer may want to disallow to disable such logging.


\I{################################################################################################}
## 13.4 Variable LOXNAME_LOGGERNAME_VERBOSITY # {#alib_mod_alox_external_verbosity_configuration_variable}

As a reader may has already guessted, this camp does not only "lent the concept of priorities"
from sibling camp \alib_variables, but of course also imports and exports <em>Verbositiy</em>
information from and to configuration data systems.

A programmer needs nothing special to do. The same as \alox creates <em>Log Domains</em> on the fly as soon
as you are using them, the same it reads configuration data for these <em>Log Domains</em> without having
an explicit mandate for that. It just always does. If no external configuration is used, nothing
is read or exported and \alox will not complain. However, if it finds configuration data that influences
the \e Verbosity setting of <em>Log Domains</em> you are using, then you will get noted by \alox as soon as
you have activated \ref alib_mod_alox_internal_domains "Internal Logging".

The variable that is used to store <em>Verbosity</em> information is named
<b>ALOX/LOGGERNAME/VERBOSITY_WITH_LOXNAME</b>.Please read its
\ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME "quick reference documentation" now and return
back to here.

When a \e Verbosity setting is read by \alox from external configuration, the priority of the plug-in
that provided the data is used as the priority of the setting, just as if this value was
passed to method \alib{lox;Lox::SetVerbosity} using optional parameter \p{priority}.

\note
  Unlike most other variables, the value of this variable does is not "directly" used during the
  run of an application. Instead, when a \e Logger is attached to a \e Lox, the variable
  is read and for each domain listed, a corresponding invocation of \alib{lox;Lox::SetVerbosity}
  is performed. At the moment a \e Logger is removed, the variable is first cleared and
  completely rewritten.


\note
  For more general theory of configuraiton variables, please consult:
  - The \ref alib_mod_variables "Programmer's Manual" of module \alib_variables_nl.
  - A basic understanding of how \alib and its \alibcamps are "bootstrapped" is helpful.
    During bootstrapping it is decided whether your application shares the configuration files with
    \alib or if you application wants to divert \alib and \alox configuration to separated files,
    and may other things like that. \alib bootstrapping is discussed in full length in the
    \ref alib_mod_bs "Programmer's Manual" of module \alib_bootstrap_nl.


The formal syntax of variable \ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME is given with:<p>

&nbsp;&nbsp;&nbsp;<c> ALOX/\<LOGGERNAME\>/VERBOSITY_WITH_\<LOXNAME\> = [ExportAll ;]  [*]domainpath[*] = verbosity [ ; &hellip; ] </c>

This formal definition states that an indefinite number of arguments of the form:

        [*]domainpath[*] = verbosity

may be given, each separated by a semicolon <c>';'</c>.
The pairs of \p{domainpath} and \p{verbosity} have the following meaning:

- <c>domainpath</c> denotes the path of the <em>Log Domain</em> whose \e Verbosity
  is to be set. Optionally the value may contain wildcard character <c>'*'</c>
  at the beginning or end of the string or at both places.<br>

- <b>Parameter verbosity</b><br>
  The verbosity value as listed in enum class \alib{lox;Verbosity}. The names given here are
  treated case insensitive and can be shortened to their bare minimum, for example, "i" instead
  of "Info".

With having the ability to provide wildcards, the possibilities to define the \e Verbosity
is a little different than when using method \alib{lox;Lox::SetVerbosity}! The differences are:

- With using configuration variables, it is possible to fetch <em>Log Domains</em> which are residing in different
  subdomain trees. For example, setting <c>'*ERRORS*'</c>, affects any <em>Log Domain</em> with this substring
  in the path, like

        /ERRORS
        /UI/DIALOGS/ERRORS
        /COMM/ERRORS/SEVERE

  This is not possible with using method \b %Lox::SetVerbosity which always modifies exactly
  one <em>Log Domain</em> and its Subdomains.

- Using method \b %Lox::SetVerbosity, it is possible to use "relative path addressing", which
  means, a <em>Log Domain</em> or subdomain of the evaluated from the position in the code where
  the statement is placed (hence the <em>Scope Domains</em>), can be addressed. Although it is not
  often recommended to do so, sometimes this possibility is be very helpful. For example, to do a
  quick, temporary setting or for settings that are made explicitly "programmatically", meaning
  those who are depending on state conditions of software)

In general, as already described, controlling the \e Verbosity using configuration variables
is preferred over using method <b>%Lox::SetVerbosity</b>. This is the reason why \alox
does not provide an interface method to set "rules", similar to those coming from configuration
variables from within the source code.

\note
  If a programmer needs the same flexibility of setting the \e Verbosity for patterns of
  <em>Log Domain</em> paths from within the code, then the way to go is to add a rule to the
  corresponding configuration variable.

\note
  Source code samples of how this is concretely done in the programming language of your choice
  can be found in the unit tests of \alox.

<p>
\note
  To address the internal domain tree (see \ref alib_mod_alox_internal_domains), provide string
  \b "INTERNAL_DOMAINS" for parameter \c domainpath. This can also be used as a prefix e.g.

        INTERNAL_DOMAINS/ALIB = verbose ;
  However, a \e Logger that is not configured to log on internal <em>Log Domain</em>, cannot
  be enabled for doing so using this configuration variable. Instead, a \e Logger has to be added
  to the internal <em>Log Domain</em> tree once by the software itself. This behavior is wanted
  and similar to the fact that a <em>Logger</em> cannot be added to a different \b %Lox instance
  by just adding the verbosity setting variable on the command line!


\I{################################################################################################}
## 13.5 Receiving the Resulting Domains From A Variable  # {#alib_mod_alox_external_verbosity_configuration_variable_writeback}

If a configuration source is write-enabled, of course configuration variables can be written back,
for example, if an application closes. To leverage this feature, \alox writes all verbosities that
resulted from the patterns given in variable \ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME
during software run, back to the configuration variable. This is done at the moment a \e Logger
is removed from a \b %Lox object.

If now, an external configuration source is enabled to write configuration data back to a source,
then the changed variable can be presented back to the end-user. A simple "write-back-enabled" type
is found with class \alib{variables::IniFileFeeder}.
In the sample application provided with this library, this approach is demonstrated.
At the beginning of function <c>main()</c>, bootstrapping is halted after the first phase, an
INI-file is created, and all file contents is fed to the configuration system:

 \snippet "ALox/sample.cpp"                 DOXYGEN_CREATE_INIFILE

 At the end of function <c>main()</c>, we remove the INI-File and use method
 \alib{variables;IniFileFeeder::Export}, to fill the INI-file with all defaults.
 This of course saves an end-user of your software quite some work.

 \snippet "ALox/sample.cpp"         DOXYGEN_REMOVE_INIFILE

 After a first run of the application (with no INI-file existing before!), the resulting INI-File
 looks like this:

 \verbinclude   ALoxSample.ini

Well, this is quite a long INI-file, but this is due to the fact that the sample application
creates several \b Lox and \b Logger instances. Furthermore, each and every configuration aspect
of \alox is found now in this file. An end-user is now enabled to change the logging behavior
to adopt his preferences and needs.

\note Again: If you don't like this, or don't like it for some subsets of your log statements,
      a programmer has the full control over allowing such changes or not!
      Also, overloaded methods \alib{variables;IniFileFeeder::Export}, allow to just write back
      only certain variables.

Usually, only variables that did not exist yet in a configuration file, are written.
But type \alib{variables;IniFileFeeder} has a special feature: It allows putting the word \c "writeback"
(respectively what is resourced with to token <c>"CFGIniWB"</c>) in a line in the INI-file above a
variable. If this is detected, the new value of the variable is overwriting the previous value of
the INI-file.

In the case of variable \ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME this can be used for
two things:

1. A user can directly see the results for each log-domain with the patterns previously given.
   With repeated tries, a fine-grained setting can be achieved.
2. If the verbosity variable is passed with higher priority once, for example, by providing a
   command line parameter, then the resulting value is written into the INI-file. With that,
   subsequent calls without the parameter are performed with the same verbosities.

If you closely look at the generated INI-file above, you might notice the following:
Variable <b>"ALOX/DEBUG_LOGGER_VERBOSITY_WITH_LOG"</b> contains flag <b>"ExportAll"</b> and lists
much more domains than the other verbosity-variables found in the INI-file.
The reason is that method \alib{lox;Log::AddDebugLogger} sets field
\alib{lox;CVVerbosities::ExportAll} to \c true for this logger. If this is set, redundant
information is generated when the variable is exported: An entry for each subdomain appears,
even if a subdomain has the same verbosity setting as its parent.<br>
The rationale for doing so is that during development, when working with the debug-logger,
a programmer can quickly see an overview of all known domains and change the setting
very fine-grained.<br>
On the other hand, the reason why this is not done for other loggers is, that \alox by default
does not want to disclose the internal domain structure to end-users.

Both behaviors can be changed either
 - by directly manipulating the field in the variable, after a logger got attached, or
 - by adding tag <c>ExportAll</c> manually to an external configuration.

 Of course, in case of INI-files, \alib{variables;IniFileFeeder::SetWriteBackFlag;writing back of entries}
 has to be enabled, if the domain list should be adjusted by future runs of the same application.


\I{################################################################################################}
## 13.6 Writing Substitution Variables  # {#alib_mod_alox_external_verbosity_configuration_variable_writeback_subst}
What was explained in the previous chapter about writing back verbosity settings to INI-files,
can be extended a little further when combined with
\ref alib_variables_values_substitution "configuration variable substitution".

Let's imagine a developer of an application has some problems with interfacing the XWindow library.
Hence, she would focus on the relevant domains and change the \e Verbosities to this:

        [ALOX/DEBUG_LOGGER]
        VERBOSITY_WITH_LOG =         /                   =Warning;     \
                                     /ACTION             =Warning;     \
                                     /AWAX               =Info;        \
                                     /CMDLINE            =Off;         \
                                     /TILE               =Off;         \
                                     /WMI                =Info;        \
                                     /WMI/XLIB           =Verbose;     \
                                     /WMI/XLIB/RANDR     =Warning;     \
                                     /WMI/XLIB/X11       =Verbose;     \

Now, the X11 problems were fixed. However, the developer likes the detailing of the settings and
aims to preserve them for the future. All he needs to do, is to copy the variable to a different
one, like this:

        [MYSTUFF]
        X11_DEBUG_VERB=              /                   =Warning;     \
                                     /                   =Warning;     \
                                     /ACTION             =Warning;     \
                                     /AWAX               =Info;        \
                                     /CMDLINE            =Off;         \
                                     /TILE               =Off;         \
                                     /WMI                =Info;        \
                                     /WMI/XLIB           =Verbose;     \
                                     /WMI/XLIB/RANDR     =Warning;     \
                                     /WMI/XLIB/X11       =Verbose;     \


With that, this configuration can always be restored, by just setting the verbosity as follows:

        [ALOX/DEBUG_LOGGER]
        VERBOSITY_WITH_LOG =   ${MYSTUFF/X11_DEBUG_VERB}

        [MYSTUFF]
        ...
        ...

To conclude, lets take some further notes on what was said above:

- The procedure of fine-tuning a certain "verbosity preset" and storing this under a different
  variable name, can of course be repeated and different sets can be saved in different custom variables.
  Once this is done, switching the verbosities between different "debug use-cases", is a matter of
  changing the substitution variable in variable \ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME

- Of course, the configuration sets stored can be shortened easily to contain only non-redundant
  information. \alox, when writing the variable, includes all Subdomains in the list of settings,
  even those that have the same \e Verbosity as their parent domain. This is intentionally done to
  support the user and decrease typing effort.

  Once a setting is fixed, all redundant lines can be deleted easily - but again this is not
  necessary even, it might just increase readability and shorten the configuration file.

- When execution paths are changing between different runs, \alox will remove \e Verbosity
  information provided, if a <em>Log Domain</em> was not used.
  The advantage of this is, that when <em>Log Domains</em> are renamed in the source code,
  no "garbage" remains in the config file - at least for freshly written variables.

- With more complex use cases, there are even new options: For example, when having different
  configuration files in parallel attached to a \b Configuration instance, e.g., a system-wide and
  user specific INI-file (the latter with a higher priority value!), then substitution
  "preset-variables" might reside in the deployed application's system-wide configuration, while
  each user locally can address these variables and use them in his local INI-File.<br>
  Likewise, an environment variable, or a command line parameter could use a substitution variable
  that defines such preset, with the substitute residing in a configuration file...

- The whole concept can also be used 'in code'. As a sample think of a command line application that
  provides different "presets" for verbosities". The corresponding CLI parameter is usually
  specified as <c>-v, --verbosity \<val\></c>.<br>
  Now, the software could write a variable of e.g., \b ALOX_REL_CONSOLE_VERBOSITY
  (a release logger responsible for the software's standard output) with either
  \alib{variables;Priority;Priority::Standard} or \alib{variables;Priority;Priority::Protected}
  (use the latter to disallow the user to hack in).
  The value of the variable set 'in code' would either contain substituted system-wide external
  variables, or hard-coded strings (again, depending on the necessary protection level of the software).

- If module \alib_variables is wished to be used in software, besides the use of \alox,
  hence for the applications own config parameter, it is possible to separate application-specific
  configuration variables in different configuration file. When looking at the quite long config
  file above, the motivation for this is obvious: \alox should not clutter an application's
  config file with all its information. The way how to do this is described in detail
  in the Programmer's Manual of \alib in chapter \ref alib_mod_bs.


\I{################################################################################################}
## 13.6 Wrap-up # {#alib_mod_alox_external_verbosity_configuration_wrapup}

As a summary, these are the takeaways of this chapter:

- \alox has an optional \p{priority} with method \b %Lox::SetVerbosity which is fully orthogonal
  in its use. This means, when omitted \alox works as if this feature was not existing.
  (aka "...as if \alox was one of the hundreds of other ordinary and boring logging library out there.)
- The priority of \e Verbosity settings was invented to make external configuration as
  transparent and "natural" as possible. It sounds complex, but in the end it is the simplest-stupid
  approach, when juggling with hard-coded defaults, configuration files, environment variables and
  command line options...and even with system-wide and local configuration files!
- The recommended way to set \e Verbosities - especially when working in a team - is to use
  external configuration variables.
- It is possible to overwrite different data sources, e.g., use an environment variable to
  overwrite what is given in an iniFile. Or to have two INI-files, one local, user dependent
  and one general, public instance. The user-dependent INI-file may modify (overwrite) settings
  of the general one.
- In special situations it is recommended to protect certain <em>Log Domains</em> from being changed
  at runtime. This is done by explicitly providing
  \alib{variables;Priority;Priority::Protected} to method \b %Lox::SetVerbosity.
  A similar issue is described for <em>Domain Substitution</em>
  \ref alib_mod_alox_domain_substitution_config_prevent "in an upcoming chapter".
- External setting of \e Verbosity has different (partly more, partly less) possibilities
  in comparison to "programmatically" setting. A way to extend the programmatically setting
  to work the same as external configuration, is by using an in-memory configuration file,
  with higher priority than \alib{variables;Priority;Priority::Standard}.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 14 Trimming Source File Paths and Clickable IDE Output # {#alib_mod_alox_trim_source_path}
To collect the path information of a <em>Log Statement</em>, the C++ version of \alox
uses preprocessor macro \c \_\_FILE\_\_.

The source file name and path information received is used in two ways:
- To determine the \e Scope of a <em>Log Statement</em>: \alox uses this to implement a variety of
  nice features like <em>Scope Domains</em>, <em>Prefix Logables</em>, <em>Lox::Once</em> or
  <em>Log Data</em>.

- For logging out such information to provide the context of a <em>Log Statement</em>.
  If an IDE supports it this may make log output 'clickable'.

\note Let's be honest: While we are roughly on page 50 of this freaking Programmer's Manual, and
      we are not even very close to its end, and you might wonder:
      "Wow, what did these people motivate to write and publish all this fancy code and docs?",
      here comes the answer: The project started in 2013 for one single reason:
      We wanted to have <b>clickable log output in our IDEs!</b>. We just hated to search the
      spot of a debug-log message manually in the code! The rest is history...

Source file paths can become quite long and for various different reasons, it might be wanted remove
a redundant or irrelevant prefix from the it.

\I{################################################################################################}
## 14.1 Source Path Trim Rules # {#alib_mod_alox_trim_source_path_set}

\alox provides a mechanism to trim the path of source files by using a set of rules.

Internally, different lists of such <b>'Source Path Trim Rules'</b> are maintained.
One is a static table, which stores trim information used by all instances of class \b %Lox.
In addition, each \b %Lox has an own, private set of <em>Source Path Trim Rules</em>.
The advantage of having the static, global table is that a user can set 'hints' for trimming
the path only once (at bootstrap) and forget about it. All instances of \b Lox will use it.

\I{################################################################################################}
### 14.1.1 Automatic Detection of Trim Rules ## {#alib_mod_alox_trim_source_path_set_automatic}
\alox tries to detect trimable portions of a source path automatically.
For that, \alox compares the actual path name of the executable of the currently running
process with the path of a source file. The prefix both have in common is identified as trimable.

As an example, imagine a process' file name including the path is:

        /home/lisa/bin/myapp(.exe)

When \alox finds a source file residing in:

        /home/lisa/dev/myapp/src/ui/dialog.cpp

The trimable prefix of the path would be <em>"/home/lisa/"</em>.
Therefore, the trimmed file name would be:

        dev/myapp/src/ui/dialog.cpp

The result is a more narrow log output which is probably even better readable.

Of course, it would be even nicer to have

        ui/dialog.cpp

or alternatively:

        src/ui/dialog.cpp

Before we learn how we can configure \alox to trim the path like this, let us first better understand
when \alox performs the automatic detection described above:

- \alox tries to detect a rule for trimming only <b>once per Lox</b>:<br>
  The reason for this is simply performance. We rather want \alox to be fast instead of trying to detect
  trim rules with every <em>Log Statement</em>. However, if different subsets of the source code resides in
  different paths, the path detected becomes random: It depends on the fact which source logs first!
  And even worse: if no match was found with that first file, no second try is performed.

- More precisely, \alox does its one-time detection exactly at the moment a first source file is
  using a \b Lox and the path of this file was \e not processed by an explicitly provided
  <em>Source Path Trim Rule</em>.

- If \alox finds a rule, such rule is stored in the list dedicated to the \b Lox that found it,
  not in the global list applicable for all \b Lox instances. This is why the one-time detection
  is a one-time detection <em>per %Lox</em>.

As you see, this auto-detection works nicely on small projects with source files kept local
and the executable is residing in a 'related' place.

When things become more complex, it is advisable to add <em>Source Path Trim Rule</em> explicitly.


\I{################################################################################################}
### 14.1.2 Programmatical Provision of Trim Rules ## {#alib_mod_alox_trim_source_path_set_programmatical}
A user of \alox may use method \alib{lox;Lox::SetSourcePathTrimRule} to add
<em>Source Path Trim Rules</em> explicitly.
When you reconsider the sample above, explicit setting leads to better results even in simple
project. The sample used source file:

        /home/lisa/dev/myapp/src/ui/dialog.cpp

Now, for a programmatically setting, Lisa invokes

\snippet "ut_alox_dox.cpp"        Man_TRIM_SOURCE_PATH

as the very first statement before using \alox. The first parameter tells \alox which string
to search. A leading wildcard character <c>'*'</c>, indicates that the path is a substring
of the file paths identified (no trailing <c>'*'</c> is needed). The second, optional parameter
tells \alox to include the given substring in the trimming. In other words, the substring will
be excluded from the trimmed path. In again other words it means that the given portion of the path is
included in the trimming action!

Now the trimmed source path leads to

        ui/dialog.cpp

which is probably what is wanted. As we see, the huge advantages of explicitly setting <em>Source Path Trim Rules</em> is that they
do a better job, are independently applied regardless of 'where' the executable is located,
and that any random behavior of trimming is eliminated.

\note
  If the platform (compiler) specific path separator is <c>'/'</c>, then characters
  <c>'\'</c> found in parameters \p{path} and \p{trimReplacement} when setting rules are
  replaced by <c>'\'</c> and vice versa.
  This allows specifying paths and substrings thereof in a platform-independent way.
  The same is true when rules are defined in external configuration variables, as described
  later in this chapter.


It is advisable to use set <em>Source Path Trim Rules</em> at the very start of a process.
Even before attaching the debug \e Loggers or custom \e Loggers to any \b %Lox instances.
The reason for the latter is that already attaching a \e Logger lets class \b %Lox evaluate the
\e Scope and potentially create the internal automatic rule.
This auto-rule might be conflicting with a user-given rule but be prioritized
due to the fact that it was created before the users' rule.

Then, in a multithreaded application, from the moment on parallel access to two different
instances of class \b Lox are performed, it is not allowed to add 'global' rules, because,
for performance reasons, the access to the rules is not protected by a \e mutex.

\note
    Alternatively, all existing instances of class \b %Lox have to be locked 'manually'
    by invoking \b Acquire() before setting a global rule.

If multiple <em>Source Path Trim Rules</em> are set, those are applied in their order of creation. The private rules
of a \b Lox are applied first, the global rules afterwards.
Only one rule per source path is executed. In other words, as soon as a rule matches and modifies
a source path, no further attempts for trimming are performed.

\I{################################################################################################}
### 14.1.3 Trim Rules Provided by External Configuration Variables## {#alib_mod_alox_trim_source_path_set_config}

Setting <em>Source Path Trim Rules</em> <em>'from within the source code'</em> is easy and
straightforward. However, when working in a team such changes might not be appreciated by other
team members. In this and other situations it is advisable to retrieve <em>Source Path Trim Rule</em>
from external configuration data, command-line parameters, or other sources at runtime.

For <em>Source Path Trim Rules</em> and also other configuration data, \alox leverages the configuration
facility provided by sibling module \alib_variables.

\note
   - For general information on \alox configuration variables consult \ref alib_alox_cfgvars.<br>
   - For information on how to pass configuration data from custom sources to \alox, refer to
     the Programmer's Manual of module \alib_variables.
   - Furthermore, previous chapter \ref alib_mod_alox_external_verbosity_configuration
     gives a great introduction in external configuration techniques, including a source code
     sample that can be quickly copied to your own code.


When an instance of class \b %Lox is constructed, two a configuration variables are tried to
be read:
- \c ALOX_GLOBAL_SOURCE_PATH_TRIM_RULES.
- \c ALOX_LOXNAME_SOURCE_PATH_TRIM_RULES.
<p>
The first is for defining global rules. The second variable is specific to a named \b %Lox
and the substring <c>'LOXNAME'</c> needs to be replaced by the name of the \b %Lox in question.

Otherwise, the format of the variables are the same and the following description applies to both.

The variable may contain a list of <em>Source Path Trim Rules</em> separated by semicolons <c> ';'</c>.
Each rule consists of up to five values, separated by colons <c> ','</c>.
The five values correspond to the first five parameters of method
\alib{lox;Lox::SetSourcePathTrimRule}. The same as parameter \c 3 (\p{trimOffset}),
\c 4 (\p{sensitivity}) and \c 5 (\p{trimReplacement} are optional with method
<b>%Lox::SetSourcePathTrimRule</b>, providing them in the configuration variable is optional as well.

As an example, an INI-file of an application might have the following subsection:

\snippet "ut_alox_lox.cpp"        Man_SourcePathTrimming

\I{################################################################################################}
### 14.1.4 Priority of Trim Rules ## {#alib_mod_alox_trim_source_path_set_priorities}

Different trim rules might be redundant or even contradict each other. Therefore , the rules
are equipped with a priority. Rules with higher priority are applied first.
Once a rule matches, no further rule is applied.
The priority is defined with enumeration \alib{variables::Priority} and "lent" (for good reasons)
from sibling module \alib_variables. It is are set as follows:
- In the case that a rule is read from an external configuration
  variable, the priority value is set to the priority value of the configuration plug-in that
  provided the variable.
- In the case that the rule is set from within the source code, the priority is taken from
  optional parameter \p{priority} of method \alib{lox;Lox::SetSourcePathTrimRule}.
  The parameter defaults to \alib{variables;Priority;Priority::Standard} and thus "hard-coded" trim
  rules in source code have a lower priority than those in external configuration.
- Rules that are automatically detected have a priority of \alib{variables;Priority;Priority::AutoDetected}
  and thus are overruled by all others.
  To change this behavior, any other priority may be specified in the source code, especially
  the pre-defined value \alib{variables;Priority;Priority::Protected}, which
  allows a programmer to give the rule set in the source code the highest priority and this way
  to protect the rule from external manipulation.

\I{################################################################################################}
### 14.1.5 Verifying Trim Rules ## {#alib_mod_alox_trim_source_path_set_veryfy}

With most methods invoked on class \b Lox, internal log messages are provided (See
\ref alib_mod_alox_internal_domains). When setting <em>Source Path Trim Rules</em>, this is \b not the
case. The rationale here is, that in most cases no \e %Logger is attached to the \b %Lox attached,
yet. If something goes wrong with source path trimming, e.g., when you are not sure if a rule was
read from external configuration variables, you can use method \alib{lox;Lox::State} to dump out
the current state of a \b %Lox after a logger was added.<br>
Besides rules set programmatically or using configuration variables, if exists, also the auto-generated
rule is displayed.
\note
  As an alternative to (temporarily) adding an invocation of <b>%Lox::State</b> to your code,
  \alox provides configuration variable \ref alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT.
  This allows enabling an automatic dump of the state using a command line parameter or
  other external configuration sources.


\I{################################################################################################}
### 14.1.6 Removing Trim Rules ## {#alib_mod_alox_trim_source_path_set_remove}

In very special situations (e.g., \alox uses it for unit testing) method
\alib{lox;Lox::ClearSourcePathTrimRules} can be used to clear the rules of a \b %Lox instance,
optionally including all global rules.

\note
    This method can also be used \e before invoking \b %Lox::SetSourcePathTrimRule to suppress
    the creation of the automatic rule. Of course, this has to be done for every instance of
    class \b %Lox in question, because the automatic rule and its creation is local to the each
    \b %Lox.

## 14.2 Considerations in respect to Scope Functionality # {#alib_mod_alox_trim_source_path_scope}
The benefits of having \alox using reduced length source file paths in consideration to \e Scopes
is in respect to performance. All data associated with language-related \e Scopes is stored and
retrieved in hash-tables with keys containing the source path.<br>
Furthermore, the output of method \alib{lox;Lox::State} might be better readable with these shorter
paths.

It is also important to consider that when using \e Scope::Path, especially if increased by
an amount of addressed parent directories,
the source path must not be trimmed more than such use suggests. When the files are sorted
in nested directories according to the nested namespace structure implemented in the files,
then the directory before the tree of namespace directories should be included in the trimmed path.


\I{################################################################################################}
## 14.3 Considerations in respect to Logging Meta-Information # {#alib_mod_alox_trim_source_path_set_metainfo}

\note
  This section is about textual logging with the use of \e Loggers derived from class
  \alib{lox::textlogger;TextLogger}, provided with \alox. For other
  \e Loggers, other considerations might apply.

Due to the rich meta-information that \alox provides, the log output lines quickly become quite wide.
Therefore, trimming the path of source files in the log output to a level that still allows
distinguishing all files properly, can help a lot to reduce such width.

The \ref alib_variables_types_custom "registered configuration variable" type \alib{lox::textlogger;FormatMetaInfo},
with its instance found in every \alox \b TextLogger, provides a format string to structure the meta
info. For the source path, two format parameters exists:
- <b>%SP</b>: Provides the full path name
- <b>%Sp</b>: Provides the trimmed path name

Directly from the log-output you can observe how trimming
is performed by \alox and whether there is the need to add a trimming rule explicitly.

However, those IDEs which recognize certain patterns and allow to automatically link these patterns
with the source file in their editor, sometimes need to see the full path name or a certain part
of it.
To adjust \alox to your IDE, you might have to play around a little. If your IDE supports clickable
output lines, you might want to change the format of the meta info to match the pattern that your
IDE requires.

\note
  In documentation page \ref alib_mod_alox_ide_setup, detailed instructions for selected platforms and
  IDEs are given.

Certain build systems/compilers might not provide the absolute path information
in certain situations. In contrast, they might provide source directories in relative addressing
in respect to a project file. If then the IDE does not recognize the right origin of a relative path,
this path needs correction to be absolute.
\alox provides a feature to do such correction. An optional parameter of method
\alib{lox;Lox::SetSourcePathTrimRule} allows specifying a replacement string
for what is cut by a rule. (This replacement string can of course also be set using
the configuration variables described in previous chapter \ref alib_mod_alox_trim_source_path_set_config.)
Sometimes it might be needed to "fiddle around" with the options of the trim rules and the
format of the \b %TextLogger output, until finally your IDE recognizes the source information
correctly. But once succeeded, it proofs being worth the effort!


When you are sure, that your IDE does not support the links to the editor, then you're unfortunate
and your way to go is displaying a (best possible) trimmed path, to reduce overall log output width.

\note Honestly, in this case we would recommend to switch the IDE!

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 15 Log Domain Substitution # {#alib_mod_alox_domain_substitution}

\I{################################################################################################}
## 15.1 Introduction # {#alib_mod_alox_domain_substitution_intro}

<em>Domain Substitution</em> is a concept of \alox that allows manipulating the effective domain of
<em>Log Statements</em> without changing neither these statements nor the <em>Scope Domains</em>
that apply to them. Therefore, <em>Domain Substitution</em> is useful when those changes cannot
be performed, most likely because the corresponding code using \alox is residing in a library.
Sometimes, also a "temporary redirect" of certain domains may be wanted.

As with <em>Scope Domain</em> settings, the concept of <em>Domain Substitution</em> in general
should not be 'overused'. Otherwise, things might get complicated.

The rules are set and removed with method \alib{lox;Lox::SetDomainSubstitutionRule}, which accepts
two parameters:
- \p{domainPath}:<br>
  The domain path (substring) that is to be searched and replaced. For the search, a simple
  \e 'wildcard' mechanism is applied: It is allowed to use character <c> '*'</c> at the beginning and/or
  the end of the search string. As a result, four \e 'types' of rules can be created
  - Exact match (no \c * given)
  - Prefix match (\c * at the end of \p{domainPath})
  - Suffix match (\c * at the start of \p{domainPath})
  - Substring match (\c * at both, start and the end of \p{domainPath})

- \p{replacement}:<br>
  This is the string that replaces the string in \p{domainPath} when the rule matches.
  If this is empty (or nulled), the rule is removed from the \b %Lox.

If for any reason, all rules should be deleted at once, invoke method \b %Lox::SetDomainSubstitutionRule
with providing a nulled or empty string for \p{domainPath}.
There is no explicit other method for clearing the rules.

\I{################################################################################################}
## 15.2 How Class Lox Processes the Rules # {#alib_mod_alox_domain_substitution_processing}

Rules for <em>Domain Substitution</em> are applied after any other evaluation of the resulting domain is performed.
The rules are applied in the order of their definition. Therefore, rules that are passed from
configuration files (or command line, etc.) are applied first.
Rules might influence each other, hide each oder, or even be circular. When setting the rules,
\alox does not check for such (sometimes wanted) dependencies of the rules. It might happen,
that a rule that was defined earlier than a different one, would be effective only after
the second rule was applied. Therefore, \alox repeats the application of all rules
as long as at least one of the rules matched. Because of the possibility of having circular
dependencies, a maximum amount of ten loops is performed.
After that, \alox stops processing <em>Domain Substitution</em> and writes a <em>Log Statement</em> of \e %Verbosity.Error to the
internal domain. The error message is only issued once. However the flag is reset
when all rules are cleared (by providing a nulled string value for parameter \p{domainPath}
of method \alib{lox;Lox::SetDomainSubstitutionRule}.

This amount of loops should be more than enough for all major use cases. Even a third
loop is necessary very seldom.

\I{################################################################################################}
## 15.3 Substitution Rules and External Configuration # {#alib_mod_alox_domain_substitution_config}

### 15.3.1 Using External Configuration ## {#alib_mod_alox_domain_substitution_config_using}

Setting <em>Domain Substitution Rules</em> <em>'from within the source code'</em> is easy and straightforward. However, when
working in a team such changes might not be appreciated by other team members. In this and
other situations it is advisable to retrieve <em>Domain Substitution Rules</em> from external configuration data, command-line
parameters, or other sources at runtime.

For <em>Domain Substitution Rules</em> and also other configuration data, \alox leverages the
configuration facility provided by sibling \alib module \alib_variables

\note
   - For general information on \alox configuration variables consult \ref alib_alox_cfgvars.<br>
   - For information on how to pass configuration data from custom sources to \alox, refer to
     the Programmer's Manual of module \alib_variables.
   - Furthermore, previous chapter \ref alib_mod_alox_external_verbosity_configuration
     gives a great introduction in external configuration techniques, including a source code
     sample that can be quickly copied to your own code.


The constructor of class \b %Lox tries to read a configuration variable named
\c ALOX_LOXNAME_DOMAIN_SUBSTITUTION.
When setting the variable, the substring <c>'LOXNAME'</c> needs to be replaced by the name of
the \b %Lox in question.
The variable itself may contain a list of <em>Domain Substitution Rules</em> separated by semicolons <c> ';'</c>.
Each rule consists of two strings which correspond to the two parameters of method
\alib{lox;Lox::SetDomainSubstitutionRule}.
These two strings are to be separated by <c>'->'</c>.

As an example, an INI-file of an application might have the following subsection:

\snippet "ut_alox_log_domains.cpp"        Man_DomSubst_Config

With variable \c ALOX_MYLOX_DOMAIN_SUBSTITUTION set in the INI-file, a \b %Lox named <c>'MYLOX'</c>
would receive the two rules specified.


\I{################################################################################################}
### 15.3.2 Preventing The Use Of External Configuration ## {#alib_mod_alox_domain_substitution_config_prevent}

Sometimes it might be wanted that <em>Domain Substitution Rules</em> are not manipulable externally. A reason could be that
by the nature of these rules, unauthorized users are enabled to change the <em>Log Domains</em> and hence
also to change their \e Verbosity, which otherwise can be prevented for protection
(see \ref alib_mod_alox_external_verbosity_configuration).

To keep things simple, \alox does not provide an explicit interface to prevent reading the rules
from the configuration, because, the fast way out is to delete all rules after the creation of the
\b %lox:

\snippet "ut_alox_log_domains.cpp"        Man_DomSubst_Config_Prevent


\I{################################################################################################}
## 15.4 Final Remarks # {#alib_mod_alox_domain_substitution_wrapup}

<em>Domain Substitution</em> rules are always applied to all \e %Loggers within a \b %Lox.

As noted already, <em>Domain Substitution</em> should be used with care and in case of doubt, thought well.
Different alternatives to setting a <em>Domain Substitution</em> exist.
For example, to manipulate the domain of methods executed within a library, a thread-related
<em>Scope Domain</em> setting may be set before invoking library methods.

<em>Prefix Logables</em> that have been set for a domain that gets substituted, of course do \e not apply to the
substitution domain. In contrast, a <em>Log Statement</em> whose resulting domain gets substituted works
just as it was using the new domain within its invocation and <em>Scope Domain</em> setting.
Therefore, <em>Domain Substitution</em> settings - if at all - should be made at the beginning of a processes life-cycle
and then should not be altered. In this case, also <em>Prefix Logables</em> will be 'transferred' to the substitution
domain, providing that the commands that are setting them use relative domain paths.

<em>Domain Substitution</em> may not be used to substitute <em>Log Domains</em> of the standard
domain tree with those of the internal tree or vice versa.

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 16 Colorful Loggers # {#alib_mod_alox_colorful_loggers}
\I{################################################################################################}
## 16.1 Technical Prerequisites # {#alib_mod_alox_colorful_loggers_prereq}

To explain the use of colorful log output in \alox, it is best to elaborate roughly on some
details of the \alox software architecture.

While \alox in principle is designed to process \e Logables of arbitrary type, the most
widely spread use case is logging textual information. For this reason,
abstract class \alib{lox;detail::Logger} is specialized by class \alib{lox;textlogger::TextLogger}
(still abstract).
Besides providing basic features useful for textual log output, for example, generating
human-readable meta-information of <em>Log Statements</em>, class \b %TextLogger introduces a set of 'escape sequences'.
These sequences are gathered in class \alib{lox;ESC}. The ESC-sequences are quite similar to those known from
ANSI terminals. They provide settings of foreground and background colors and font styles.
The ESC-sequences may occur in the (string type) \e Logables and as well in fields
of \ref alib_variables_types_custom "registered configuration variable" type \alib{lox::textlogger;FormatMetaInfo}.

Here is a quick sample:

\snippet "ut_alox_dox.cpp"     DOX_ALOX_ESC

Now, if a output 'drain' does not support the modification of colors (e.g., when logging into
a text file), these codes need to be removed before the textual log message is written.
This task is done by class \alib{lox::textlogger;PlainTextLogger} which in turn
specializes \b %TextLogger and which also is still abstract.
 The following diagram visualizes what was said so far:

\dot
digraph TextLoggers
{
    node [shape=record, fontname=Helvetica, fontsize=10, color=blue, fontcolor=blue];
    edge [shape=record, fontname=Helvetica, fontsize=10];
    l    [ label="Logger\n(abstract, arbitrary Logables)"               ];
    tl   [ label="TextLogger\n(abstract, textual Logables)"             ];
    ptl  [ label="PlainTextLogger\n(abstract, removes ESC sequences)"   ];

    l -> tl   [ label= "" arrowhead="open", style="dashed"  ];
    tl-> ptl  [ label= "" arrowhead="open", style="dashed"  ];
}
\enddot

The result of this design should not need much explanation: \e %Logger types that do not support
colors are derived from \b %PlainTextLogger, while those that support colors are derived from
\b %TextLogger:

\dot
digraph TextLoggers
{
   node  [ shape= record , fontname=Helvetica, fontsize=10];
   edge  [ shape= record , fontname=Helvetica, fontsize=10];
   tl    [ label= "TextLogger"          color=blue fontcolor=blue];
   ptl   [ label= "PlainTextLogger"     color=blue fontcolor=blue];
   cfl1  [ label= "ColorfulLogger_A"   ];
   cfl2  [ label= "ColorfulLogger_B"   ];
   ncfl1 [ label= "PlainLogger_A"      ];
   ncfl2 [ label= "PlainLogger_B"      ];
   tl  -> ptl   [ label= "" arrowhead="open", style="dashed" ];
   tl  -> cfl1  [ label= "" arrowhead="open", style="dashed" ];
   tl  -> cfl2  [ label= "" arrowhead="open", style="dashed" ];
   ptl -> ncfl1 [ label= "" arrowhead="open", style="dashed" ];
   ptl -> ncfl2 [ label= "" arrowhead="open", style="dashed" ];
}
\enddot

\I{################################################################################################}
## 16.2 Colorful Logging with ALox # {#alib_mod_alox_colorful_loggers_wrapup}

The takeaways from this short technical spotlight are now fairly obvious:

- The \e Logables of <em>Log Statements</em> as well as <em>Prefix Logables</em> may contain ESC-sequences.
- A user of \alox does not need to care about whether \e %Loggers are used that support colorized output
  (or similar ESC-sequences) or not. They are automatically removed if not supported.
- With the introduction of own, <b>\alox</b>-specific escape codes, software that uses \alox becomes
  independent of the underlying, platform-specific sequences. For example, \alox is not relying
  on ANSI color codes, which are not supported by colorful Windows consoles. Instead, on each platform,
  dedicated \e Loggers will perform the translation of \alox codes to platform-specific ones.
- A developer of a custom \e %Logger that should not be enabled to support ESC-sequences, simply can
  derive from class \b %PlainTextLogger which finds and removes ESC-sequences before the
  messages are passed to the custom type.
  (For the creation of custom loggers, see also chapter \ref alib_mod_alox_ext_loggers).

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 17 Loggers and Implementing Custom Types # {#alib_mod_alox_ext_loggers}

\I{################################################################################################}
## 17.1 Class Logger # {#alib_mod_alox_loggers_logger}

\alox is designed to support different log streams. A log stream is a destination for the \e Logables
and can be the IDE console, the terminal console, a file, a web service, or anything that accepts
data streams. Abstract class \alib{lox;detail::Logger} represents such a log stream.
While some specializations of the class are provided with \alox,
custom specializations may log information to any 'data drain' in any format you like.

\dot
digraph logger
{
    node [shape=record, fontname=Courier];
    edge [shape=record, fontname=Helvetica];
    logger  [ label="Logger (Abstract)" URL="\ref alib::lox::detail::Logger"];
    cl      [ label="ConsoleLogger"     URL="\ref alib::lox::loggers::ConsoleLogger"];
    ml      [ label="MemoryLogger"      URL="\ref alib::lox::loggers::MemoryLogger"];
    tf      [ label="TextFileLogger"    URL="\ref alib::lox::loggers::TextFileLogger"];
    my      [ label="MyCustomLogger"  ];
    logger -> cl   [ arrowhead="open", style="dashed" ];
    logger -> ml   [ arrowhead="open", style="dashed" ];
    logger -> tf   [ arrowhead="open", style="dashed" ];
    logger -> my   [ arrowhead="open", style="dashed" ];
}
\enddot

Abstract class \alib{lox;detail::Logger} has a very lean interface, in fact it is basically just
one method, which in specialized classes needs to be implemented to execute a log.
The data to log is provided as an instance of class \alib{BoxesMA}, which is a (monotonically
allocated) vector holding elements of type \alib{boxing;Box}, hence arbitrary objects.
\note
  Programmers that seek to implement a custom logger for <b>ALox for C++</b> should be familiar
  with \alib module \alib_boxing. You could say that class \b Box is like "std::any on steroids".

This allows creating \e Loggers that take any type of (binary) data and use the data for writing
a log entry in any custom way.

While all \alox code tries to be short and simple, class \b %Logger is particularly simple!
So, let us quickly walk through the class by looking at its members.

\I{################################################################################################}
### 17.1.1 The fields of class Logger ## {#alib_mod_alox_loggers_logger_fields}
There are just a few fields in class \alib{lox;detail::Logger}.

\I{################################################################################################}
#### 17.1.1.1 Name and TypeName ### {#alib_mod_alox_loggers_logger_fields_name}
class \b %Logger has two members, \alib{lox::detail;Logger::Name} and \alib{lox::detail;Logger::TypeName}
that can be read using \alib{lox::detail;Logger::GetName} and \alib{lox::detail;Logger::GetTypeName}.

Field \b name is defined by the user and provided with the constructor. Field \b typeName is
'hard-coded' by each derived classes' constructor. If the user of a \e Logger does not provide a name
in the constructor, then field \b name defaults to the hard-coded \b typeName.
Both fields are not used internally but only provided to be used by classes managing multiple \e Loggers
(which generally is class \alib{lox;Lox}).

If multiple \e Loggers are attached to a \b Lox, they need to have unique \e Names.

\I{################################################################################################}
#### 17.1.1.2 TimeOfCreation and TimeOfLastLog ### {#alib_mod_alox_loggers_logger_fields_TimeOf}
These are two time stamps that contain the time of the creation of the
\alib{lox;detail::Logger} (or the time this timestamp was reset to) and the time of the last log.
These two members are normally used to calculate the elapsed time (the cumulated time an application is running)
and the time difference between two log calls. The latter is especially interesting for
log outputs on a debug console, as it allows getting a first quick impression about your
software's performance, lock states, bottlenecks, etc.

#### 17.1.1.3 CntLogs ### {#alib_mod_alox_loggers_logger_fields_CntLogs}
This is a simple counter of the number of logs processed so far. Feel free to
reset it as you like, it is not used anywhere internally, other than as an option
to output with each log line.


\I{################################################################################################}
### 17.1.2 Methods of class Logger ## {#alib_mod_alox_loggers_logger_methods}

Besides the protected constructor, which just initializes some default values for the \e Loggers'
fields, abstract method \alib{lox::detail;Logger::Log} is the most important.

Derived classes only need to implement this abstract method with code that is executing the
<em>Log Statement</em>.
When the method is invoked, class \b Lox already performed various checks, including that the
\b %Verbosity justifies the execution.

### 17.1.3 Implementing a Custom Logger ## {#alib_mod_alox_loggers_logger_customloggers}
As an experienced programmer after reading the previous sections, it is certainly fully obvious
to you which steps need to be taken to create your own variation of class
\alib{lox;detail::Logger} that directs your \e Logables to a certain data drain in a specific format.
Nevertheless, let us quickly name these steps explicitly:

- Create your own class derived from class \e Logger.
- Add a constructor that takes a user-defined name and pass this name along with a hard-coded
  string representing the type of your \e %Logger, to the base constructor.
- Implement the abstract method \b Log by
  converting and streaming the given objects into a custom data 'drain'.
- In the main code entity add an instance of your custom \e %Logger type to the \alib{lox;Lox} of
  your choice.

But before you now go ahead and implement your own \e Logger type class, you should first continue reading
through this chapter.


\I{################################################################################################}
## 17.2 Abstract class TextLogger # {#alib_mod_alox_loggers_textlogger}

When you think about debug log output you think about logging textual messages that
get displayed on your debug console. We think the use of a logging ecosystem for this
quite simple purpose is advisable as soon as software project bigger than
two pages of code!

While \alox wants to be a perfect tool for quick, easy, and comfortable debug logging,
the goals of \alox go beyond that. This is why \alox logging interface methods
are not restricted to string types, but accept any object to be passed to any derived
\e Logger type.

It is very important to understand this. The result is twofold:

- For textual (e.g., debug) logging, any object that is logged needs to get converted into a textual
  representation (a human-readable format).
- Custom \e Logger types are enabled to log data of an application in a very custom
  way, as these \e Loggers get the original object passed.

Class \alib{lox;textlogger::TextLogger},
which is described in this chapter, is exactly about the first thing: Log any object that
is passed to it as a textual representation, hence into a character stream.
All of the provided \alox \e Loggers that produce text output, derive from this base
class instead of deriving directly from \e Logger. Among these classes are
\alib{lox;loggers::ConsoleLogger}, \alib{lox;loggers::MemoryLogger} and \alib{lox;loggers::AnsiLogger}.
Hence, the class diagram above is a little simplified. It rather looks like this:

\dot
digraph TextLoggers
{
    node [shape=record, fontname=Helvetica];
    edge [shape=record, fontname=Helvetica];
    logger  [ label="Logger"            URL="\ref alib::lox::detail::Logger"];
    tlogger [ label="TextLogger"        URL="\ref alib::lox::textlogger.TextLogger"];
    cl      [ label="ConsoleLogger"     URL="\ref alib::lox::loggers::ConsoleLogger"];
    ml      [ label="MemoryLogger"      URL="\ref alib::lox::loggers::MemoryLogger"];
    my      [ label="MyCustomTextLogger"  ];
     logger   -> tlogger [ arrowhead="open", style="dashed" ];
     tlogger  -> cl      [ arrowhead="open", style="dashed" ];
     tlogger  -> ml      [ arrowhead="open", style="dashed" ];
     tlogger  -> my      [ arrowhead="open", style="dashed" ];
}
\enddot

\note
 - Even this diagram is simplified. Refer to the language specific reference documentation of class
   \alib{lox;textlogger::TextLogger} to get the full picture.
 - Of course you can also derive your own *Logger* types without using class \b %TextLogger
   and still do pure textual logging.

\I{################################################################################################}
### 17.2.1 Helper-classes for TextLogger ## {#alib_mod_alox_loggers_textlogger_helpers}
Class \alib{lox;textlogger::TextLogger} contains two helper-classes as public fields.
The advantage of using helpers is that they can be replaced at runtime by your own, tailored
versions of these helpers and this way you can change the default behavior of existing \e Logger
types, without deriving new ones.

The helper-classes are described in the following two paragraphs.

\I{################################################################################################}
#### 17.2.1.1 Class FormatMetaInfo ### {#alib_mod_alox_loggers_textlogger_MetaInfo}
The following \ref alib_variables_types_custom "registered configuration variable" types are used by
class \alib{lox;textlogger::TextLogger} to assemble the meta-information of each log line,
which incorporates things like date and time, thread information, \e Verbosity and
<em>Log Domain</em>:

- \alib{lox::textlogger;FormatMetaInfo},
- \alib{lox::textlogger;FormatDateTime},
- \alib{lox::textlogger;FormatTimeDiff},
- \alib{lox::textlogger;FormatMultiLine} and
- \alib{lox::textlogger;FormatOther}.

\b FormatMetaInfo provides a public format string that defines the start (prefix) of each
log line. A sample of such format string is:

    "%SF:%SL:%A3%SM %A3[%TC +%TL][%tN]%V[%D]%A1#%#: "

The format string contains variables, marked by a leading '\%' sign.
The set of these format variables available are:

<center>Variable</center>| Description
- - - - - - - - - - - - -| - - - - - - - -
 <center> SP </center>   | The full path of the source file.
 <center> Sp </center>   | The trimmed path of the source file.
 <center> SF </center>   | The callers' source file name.
 <center> Sf </center>   | The callers' source file name without extension.
 <center> SL </center>   | The line number in the callers' source file.
 <center> SM </center>   | The callers' method name.
 <center> TD </center>   | The date the log call was invoked.
 <center> TT </center>   | Time of day the log call was invoked.
 <center> TC </center>   | Time elapsed since the \e Logger was created or its timer was reset.
 <center> TL </center>   | Time elapsed since the last log call. Note: These time values do not sum up correctly. This is not only because of rounding errors, but also because the time between the "end" of the last log call and the "beginning" of this log call is measured. This has the advantage that a quite accurate value of "elapsed time since the last log call" is displayed and hence a very good performance indicator is provided.
 <center> tN </center>   | Thread name
 <center> tI </center>   | Thread ID.
 <center> V  </center>   | The \e Verbosity. For the display of the different values, \b %FormatMetaInfo exposes four public fields containing string definitions.
 <center> D  </center>   | The <em>Log Domains</em>' full path.
 <center> #  </center>   | The log call counter (like a line counter, but counting multi-line log output as one).
 <center> An </center>   | An auto-adjusted tabulator. This grows whenever it needs to grow, but never shrinks. The mandatory number n (a character between 0 and 9) specifies how much extra space is added when tab is adjusted. This is useful to achieve very clean column formatting.
 <center> LG </center>   | The name of the \e Logger. This might be useful if multiple loggers write to the same output stream (e.g. the console).
 <center> LX </center>   | The name of the \e Lox.
 <center> P  </center>   | The name of the process / application.

\note
  The available format variables vary slightly across programming languages and platforms supported by
  \alox. The details are documented in the corresponding class reference of
  \alib{lox::textlogger;FormatMetaInfo::Format}.

<p>
\note
  Class \b TextLogger prepends the meta-information to the decoded \e Logables
  and therefore, the message itself is not provided as a formatted string variable.
  This restriction (that the meta-information is always prepended) is a result of the otherwise
  smart and helpful multi-line features of TextLogger (see below).

Changing string \alib{lox::textlogger;FormatMetaInfo::Format} provides an easy way
to change the look of your log output. For example, if you are not interested
in thread information, just remove the "[%tN] " part from the original string.

If you want to modify the output of a certain variable or if you want to add your own variables,
you can derive your on implementation of *TextLogger* and override the virtual method
\alib{lox::textlogger;TextLogger::processVariable}.
Within the implementation, just fetch your own variables and/or modify existing and call
the original method for the rest that you do not want to modify.

Finally, if you really want to customize the logging of meta-information in the class *TextLogger*
completely and maybe do not want to even rely on a format string, then feel free to override the
virtual method \alib{lox::textlogger;TextLogger::writeMetaInfo}.

\I{################################################################################################}
#### 17.2.1.2 Class ObjectConverter ### {#alib_mod_alox_loggers_textlogger_ObjectConverter}
Class \alib{lox::textlogger;ObjectConverter} is used by class
\alib{lox;textlogger::TextLogger} to convert the \e Logables that get passed by the user
through the <em>Log Statements</em> to the \e %Logger into a string representation.
While \b %ObjectConverter is abstract and declares only one simple interface method, the standard
implementation used with the built-in loggers is provided with type
\alib{lox;textlogger::StandardConverter}.

This class is still is extremely simple, as it lazily transfers this responsibility
to instances of types which are derived from class \alib{format;Formatter}.
This abstract class and corresponding standard implementations are provided with
\alib module \alib_format.

Please consult the \alib documentation of classes
- \alib{format;Formatter},
- \alib{format;FormatterPythonStyle} and
- \alib{format;FormatterJavaStyle}

to learn more about how these classes work.

As you see, also here are different "levels" of possibilities of how to change and implement custom
functionality in respect to converting the \e Logables while using class \b %TextLogger:
- A straight forward approach is to implement interfaces of module \alib_boxing, namely
  \alib{boxing;FAppend} or \alib{format;FFormat}.
- Own formatters may be implemented and attached to the instance of \b %StandardConverter.
- Alternatively an own object converter might be implemented.

<p>
\note
  In <em>C# and Java versions</em> of \alox, the logging of arbitrary complex objects is additionally
  supported with class \alib{lox;LogTools}.<br>
  This class makes use of the reflection APIs of these languages and creates a nicely formatted output.
  We are still looking for someone to volunteer and contribute to the C++ version of \alib in
  respect to using the \e ABIs of specific compilers to perform similar reflection for debug logging
  with \alib.

\I{################################################################################################}
### 17.2.2 The Multi-Line Features of Class TextLogger ## {#alib_mod_alox_loggers_textlogger_multiline}
Class \alib{lox;textlogger::TextLogger} provides a feature to log a message into multiple lines.
This is useful, for example, if you want to log a string that contains XML text.
Instead of one vast log line, *TextLogger* is configured by default to separate the text
into multiple lines in a very controlled way.

Multi-line output behavior of class *TextLogger* is configured by the field
\alib{lox::textlogger;FormatMultiLine}.

The following modes are available:

- 0: Multi-line mode off<br>
  In this mode, the text is not split into multiple lines. However, certain log streams, for example
  a system console window, will split the msg autonomously if the message contains line end characters
  (CR, LF, or CRLF).
  Therefore, using this mode, it is recommended to set the fields
  \alib{lox::textlogger;FormatMultiLine::Delimiter} and
  \alib{lox::textlogger;FormatMultiLine::DelimiterReplacement} in a way that your log file
  does not get cluttered.

- 1: Multi-line mode with all meta-information repeated in each log line<br>

- 2: Multi-line mode with blanked meta-information starting with second log line.<br>
  This is the default, because this setting makes it easy to visually recognize, that a log
  is continued in the next line and hence it is the best for debug window purposes!

- 3: Multi-line mode which writes a (configurable) headline message in the first log line.<br>
  All text lines of the multi-line text is logged starting in column zero.

- 4: Like multi-line mode 3, but without writing the headline.


\I{################################################################################################}
### 17.2.3 Locking the Standard Output Streams ## {#alib_mod_alox_loggers_textlogger_stdio_lock}
While \alib since ever provides the global object \alib{threads;STD_IOSTREAMS_LOCK},
C++20 introduced an own mechanism for the synchronization of streamed output by
providing type <b>std::basic_osyncstream</b>.

While both approaches share the goal that the output of multithreaded applications is not
interleaved, they are fundamentally different:
- C++20 type <b>std::basic_osyncstream</b> can be used to synchronize any output stream instance
  but only one
- \alib instance \b STD_IOSTREAMS_LOCK can be used to synchronize the standard output streams
  only, both \b stdout and \b stderr at the same time.

For best results, just both approaches should be used in your application. This ensures that
- the output of \b stdout and \b stderr are not interleaved, and
- that a third party library that uses \b stdout or \b stderr does not interfere with the
  logging of your application - in case it uses the C++20 mechanics.

The implementation of the built-in logger types relies on class
\alib{strings::compatibility::std;OStreamWriter} of the module \alib_strings, which takes care
of both mechanisms under the hood. Those loggers are:

- \alib{lox::loggers;ConsoleLogger},
- \alib{lox::loggers;AnsiLogger}, and
- \alib{lox::loggers;TextFileLogger}

Type <b>OStreamWriter</b> switches synchronization off if the library is compiled in
\ref ALIB_SINGLE_THREADED "single-threaded mode".

\I{################################################################################################}
### 17.2.4 Recursive Log Statements ## {#alib_mod_alox_loggers_textlogger_recursive}

\alox supports recursive log calls. Recursion occurs when log statements are executed during the
evaluation of the logables that belong to another "outer" log statement.
A user might think that recursive log calls are seldom and exotic, but we had to learn that in
reality, recursive calls unfortunately might occur quite quickly.

To allow and properly treat recursion, each class and method involved in the execution of a
log statement has to be prepared for it. The execution must be free of dependencies to member
variables or such members need to be created per recursion.

Hence, not only class \alib{lox;Lox} needs to support recursion, but also the logger classes
themselves.

Class \alib{lox;textlogger::TextLogger} and its utility class
\alib{lox;textlogger::StandardConverter} are well prepared and as long
as custom loggers are built on those, recursion should not be a problem. This is because abstract
method \alib{lox::textlogger;TextLogger::logText} was "protected" of recursion and
therefore will \b not be invoked recursively.

When implementing own variants of class \b %ObjectConverter or otherwise a "deeper" use of provided
classes is done, the possibility of recursion of log calls has to be taken into account.
We recommend a look at the built-in classes' source code to have an idea what needs to be done
in respect to field of concern.

\I{################################################################################################}
## 17.3 Logger implementations provided by ALox # {#alib_mod_alox_loggers_implementations}

While the abstract classes \b %Logger, \b %TextLogger, and \b %PlainTextLogger are located in the
namespaces \ref alib::lox::detail, and \ref alib::lox::textlogger,
you can checkout which 'ready to use' \e %Logger implementations are available today for your
preferred language version of \alox, by referring to the reference documentation of namespace
\ref alib::lox::loggers.

For convenience, method \alib{lox;Lox::CreateConsoleLogger} is provided. This method chooses
an appropriate implementation of class \b %Logger suitable for human-readable log output.
The \e %Logger chosen depends on the platform and configuration settings.

For debug logging, method \alib{lox;Log::AddDebugLogger} is provided. This may even choose and attach
more than one \e %Logger, depending on the language, platform, and IDE.

A noteworthy, built-in specialization of \b %Logger is found with class \alib{lox::loggers;MemoryLogger}
It uses an internal character buffer of type \alib{strings;TAString;AString}
and just appends each log entry to this buffer, separated by a <em>new line</em> sequence.

As \b MemoryLogger does not log to the console or any other slow thing, it is extremely fast. The latest
record was over on million log entries per second in a single-thread!
(Achieved on Intel(R) Haswell Core(TM) i7 CPU @4.0GHz, using <b>ALox for C++</b>, doing release logging.)

This gives an indication that the \alox ecosystem, in combination with its \b MemoryLogger is extremely
useful in performance-critical sections of your code. If you would do 1000 log entries per second,
the performance loss for your application would only be around 0.1%.

\I{################################################################################################}
## 17.4 Summary and outlook # {#alib_mod_alox_loggers_summary}

The following summarizes the takeaways of this chapter:
- We learned about abstract class \b %Logger and its simple structure and few fields.
- Class \b %TextLogger was introduced and explained in detail. We learned that \b TextLogger aims
  to be the abstract parent class of all \e %Logger types that are supposed to convert \e %Logables
  into a human-readable format.
- We saw how the output format of class \b %TextLogger can be manipulated without introducing
  new code and how custom logging of custom types may be achieved, without extending the given
  loggers themselves.
- Furthermore, it was explained that custom loggers that share the console of a process, should
  use C++20's new <c>ostream</c>-synchronization mechanism (<b>std::osyncstream</b>),
  as well as \alib{threads;STD_IOSTREAMS_LOCK}, to protect console output against racing
  conditions.<br>
  Both mechanisms are easily implementable when using the type
  \alib{strings::compatibility::std::OStreamWriter}
- Then we briefly looked at the currently existing \e Logger types in the \alox ecosystem.

If you developed an interesting \e %Logger, like one that
- is supporting a specific IDE with 'clickable' log lines,
- is sending Emails,
- is reporting to an analytics/metrics server or
- is logging into the Linux journal or Windows OS event log,

then please do not hesitate to propose the code to us as an extension of the open source project \alox!

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# Appendix A: Configuration Variables # {#alib_mod_alox_apdx_config_vars}

\alox defines several configuration variables, which are read from configuration files
(e.g. INI-files), environment variables, or the command line interface.
Their use are documented in various places. A reader might consult:

- For general information on \alox configuration variables consult \ref alib_alox_cfgvars.
- For information on how to pass configuration data from custom sources to \alox, refer to
  the Programmer's Manual of module \alib_variables.
- Furthermore, previous chapter \ref alib_mod_alox_external_verbosity_configuration
  gives a great introduction to external configuration techniques, including a source code
  sample that can be quickly copied to your own code.

The configuration variables (and their documentation) are:

- \ref alxcvALOX_CODEPAGE
- \ref alxcvALOX_CONSOLE_TYPE
- \ref alxcvALOX_CONSOLE_LIGHT_COLORS
- \ref alxcvALOX_GLOBAL_SOURCE_PATH_TRIM_RULES
- \ref alxcvALOX_NO_IDE_LOGGER

- \ref alxcvALOX_LOXNAME_DOMAIN_SUBSTITUTION
- \ref alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT
- \ref alxcvALOX_LOXNAME_PREFIXES
- \ref alxcvALOX_LOXNAME_SOURCE_PATH_TRIM_RULES

- \ref alxcvALOX_LOGGERNAME_AUTO_SIZES
- \ref alxcvALOX_LOGGERNAME_FORMAT
- \ref alxcvALOX_LOGGERNAME_FORMAT_DATE_TIME
- \ref alxcvALOX_LOGGERNAME_FORMAT_TIME_DIFF
- \ref alxcvALOX_LOGGERNAME_FORMAT_MULTILINE
- \ref alxcvALOX_LOGGERNAME_FORMAT_OTHER
- \ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME
- \ref alxcvALOX_LOGGERNAME_REPLACEMENTS


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# Appendix B: Auto-Configuration and Orthogonality # {#alib_mod_alox_apdx_auto_config}

\I{################################################################################################}
## B.1 Introduction # {#alib_mod_alox_apdx_auto_config_intro}

\alox addresses two fundamental 'use cases', namely
\ref alib_alox_faq_debug_vs_release_logging "debug logging< and release logging".
While technically both are similar, from a conceptual perspective the two are very different things.

Looking into the head of software developer that thinks about debug logging, you will find
the following thoughts:
- "I will quickly write some output routines and if not needed comment them out or just delete them."
- "Don't bother me, I am not willing to read a user manual for generating simple debug output."
- "My company will not pay me for writing nice debug log output, so I do not care likewise."
- "I have a debugger and comment lines in my code, so I do not urgently need a logging library."
- "I don't care too much if logging misbehaves or even crashes, this happens only in debug-versions of my software".

&rArr; We understand that a logging library has to be \b easy!

Now release logging:
- "I have to obey legal rules, logging is a key component of my server"
- "Collecting usage metrics and detecting failures in the field is a key to success of my app"
- "Using any external library needs deep and thorough understanding of this. I am paid for making sure this works."
- "More features of a logging library is always better than less."

&rArr; We understand that a logging library has to be <b>feature rich</b> and \b reliable!

This discrepancy easily justifies the use of two different libraries for both use cases.
\alox takes a different approach: it tries to match the needs of both worlds. There are two
fundamental design rules in \alox that lead to a solution:

<b>1. Orthogonality of ALox and its API.</b><br>
   Orthogonality here means that features that are not used,
   are not visible when using \alox.
   This is reached by optional parameters with reasonable default values, many overloaded variants
   of methods, and by taking internal prerequisites when \alox 'detects' that a feature is not used.

<b>2. Auto-configuration.</b><br>
   This means not only that missing configuration is defaulted, but also that
   \alox is not pedantic but designed rather non-intrusive. For example, logging into a 'new'
   <em>Log Domain</em> does not need to anyhow 'register' or 'create' such domain. Many things in \alox just
   happen on the fly.

Most of \alox auto-configuration features apply to debug logging only and are implemented by
the dedicated, auto-created \b %Lox instance used for debug logging and also by the dedicated API
interface used for debug logging (which enables the automatic pruning of debug-logging from release code).

But it has a little drawback: Hiding features to make an API simple reduces the
understanding of the API - and this is what a user of release-logging requires.
And this is <b>the goal of this appendix chapter</b>: We try to collect, list, and name concepts of
\alox that otherwise might be a little hidden for good reasons.

\note
  The list might not be too well sorted and certain points may seem to be off-topic. Please excuse.
  Many of the things that happen 'automatically' or 'on the fly' can be observed by activating the
  <em>internal logging</em> as described in \ref alib_mod_alox_internal_domains.


\I{################################################################################################}
## B.2 Initialization, Lox and Logger Instances ## {#alib_mod_alox_apdx_auto_config_list_init}

\I{################################################################################################}
### B.2.1 ALib/ALox Bootstrapping ### {#alib_mod_alox_apdx_auto_config_list_init_init}
Bootstrapping \alox is a matter of bootstrapping the \aliblong, because \alox is just one out of several
modules which all needs to be duely initialized and shut down. All about this process is
documented in the \ref alib_mod_bs "Programmer's Manual" of module \alib_bootstrap_nl.

As long as no major configuration changes are planned to be done, the standard bootstrapping
and shutdown is just a matter of invoking one function. Thus, the minimum \alox "hello world"
sample looks like this:

\snippet ut_alox_dox.cpp        ALoxTut_Minimum_Hello

\note In previous versions, with debug-logging, \alox could start a missing bootstrapping itself,
      but this feature was removed.

With this standard bootstrapping, reading \alox variables given with command line options or
environment variables is already in place!

In chapter \ref alib_mod_alox_external_verbosity_configuration_variable_writeback, a quick sample
of how to slightly customize bootstrapping is given, which allows writing all defaulted \alox
variables into a custom INI-file.


\I{################################################################################################}
### B.2.2 The Lox Singleton for Debug-Logging ### {#alib_mod_alox_apdx_auto_config_list_init_log}
For debug logging, a pre-configured instance of class \b %Lox is created and used.
More information on this is given in chapter \ref alib_mod_alox_debug_and_release_logging_dbg.

\I{################################################################################################}
### B.2.3 Adding and Removing Loggers to a Lox ### {#alib_mod_alox_apdx_auto_config_list_init_addlogger}
While the method \alib{lox;Lox::RemoveLogger} is available for removing a \e %Logger from a
\b %Lox instance, there is no method for explicitly adding one. Instead, the method that allows
to control the \e Verbosity of a <em>Log Domain</em> and its Subdomains for a \e %Logger,
\alib{lox;Lox::SetVerbosity} adds a \e %Logger at the moment the one provided
is not known to the \b %Lox, yet. Consequently, the very first time a \e Verbosity of a \e %Logger
is set, the overloaded version of \b %Lox::SetVerbosity which takes a reference to the \e %Logger
as parameter has to be used. In subsequent invocations, alternatively, the name of the
\e %Logger can be provided.

\I{################################################################################################}
### B.2.4 Logger Names ### {#alib_mod_alox_apdx_auto_config_list_init_loggernames}
Each \e %Logger attached to a \b %Lox has to have a unique name. Nevertheless, \e %Loggers can be constructed
without explicitly providing a name. In this case, a default name which is different with each
derived type of \e %Logger is used. This way, many standard use cases do not require the user to
think about the name of \e %Loggers. For example, if a \e %Logger that logs to the console should be used
in parallel to one that logs into a text file, both \e %Loggers will have different names.

However, if, for example, two different loggers of type \alib{lox;loggers::TextFileLogger} are to be added to
the same \b %Lox, then with their creation, a dedicated, distinguishable name has to
be provided.

In addition, when reading \e Verbosity settings from external configuration (as described in
\ref alib_mod_alox_external_verbosity_configuration), it is advisable to explicitly name \e Loggers, so
that external configuration data stays "compatible" at the moment a different \e %Logger type is used
for a certain task. On the other hand: this is \e not necessary for the standard debug \e %Logger
created automatically or explicitly by calling \alib{lox;Log::AddDebugLogger}
(see more info on this below), because such \e %Loggers are always named \c "DEBUG_LOGGER".

\I{################################################################################################}
### B.2.5 Using a Logger with Multiple Lox Instances ### {#alib_mod_alox_apdx_auto_config_list_init_loginmultiplelox}
It is allowed to attach an instance of a \e %Logger to multiple \b %Lox instances.
Concurrent access will be locked in case multithreading
\ref alib_threads_intro_codeselector "is activated".

\I{################################################################################################}
### B.2.6 Console Loggers and Debug Logger ### {#alib_mod_alox_apdx_auto_config_list_init_consoleloggers}
Method \alib{lox;Lox::CreateConsoleLogger} creates an appropriate type of \e Logger
depending on the operating system, IDE used (if in IDE), implementation language, configuration
variable, etc.
The goal get a maximum logging experience, e.g. colorful logging if the console supports colors.

This method can be used to create a \e Logger whenever a console logger is needed (also for
release-logging of course).

Furthermore, method \alib{lox;Log::AddDebugLogger} uses this method to attach a \e Logger to
the \b %Lox singleton used for debug-logging.
In addition, if running in an IDE, \alox tries to detect the IDE and optionally (if available)
creates a second, IDE specific \e Logger. See reference documentation of the methods linked above
for more details.<br>
For both \e %Loggers, root domain <c> '/'</c> is set to \e %Verbosity.Verbose. In addition, the \e Verbosity
for internal \alox messages is set to \e %Verbosity.Warning.
(More information on internal \alox messages is found in \ref alib_mod_alox_internal_domains.)

Finally, method \b %Lox::AddDebugLogger is automatically invoked internally at the moment that
the \b %Lox singleton used for debug-logging is used and no other \e Logger was attached, yet.

\note The exception to this rule is when a \e Logger was added to the
      \ref alib_mod_alox_internal_domains "internal domain tree" only.
      In this case, the normal standard domain tree is still free of loggers and therefore,
      a debug logger is automatically attached.

\I{################################################################################################}
## B.3 Source Path Trimming ## {#alib_mod_alox_apdx_auto_config_list_spt}
If no <em>Source Path Trim Rules</em> are given a default rule is automatically detected and registered.<br>
If <em>Source Path Trim Rules</em> are given, path separators <c>'/'</c> and <c>'\'</c> are
corrected to suit the target platform of compilation.

For more information, see chapter \ref alib_mod_alox_trim_source_path.

\I{################################################################################################}
## B.4 Log Domains and Verbosity ## {#alib_mod_alox_apdx_auto_config_list_ldv}
<em>Log Domains</em> are created on the fly as needed (used). <em>Log Domain</em>, once created, are never deleted and a user does
not need to care about their management.
New, unknown <em>Log Domains</em> inherit their \b Verbosity from their parent.

Furthermore, when obtaining \e Verbosity settings from external configuration, a natural
prioritization is applied: First INI_file settings, which are overwritten by environment variable
settings, which are overwritten by command-line parameters. All of them are overwriting
settings made invocations of \b %Lox::SetVerbosity in the source code - as long as optional
parameter \p{priority} is not given.<br>
More information on this is found in \ref alib_mod_alox_external_verbosity_configuration.

\I{################################################################################################}
### B.5 Logging Text and Objects ## {#alib_mod_alox_apdx_auto_config_list_logables}

### B.5.1 Arbitrary Logables ### {#alib_mod_alox_apdx_auto_config_list_logables_arbitrary}
\alox API transparently hides the fact that it is made for logging arbitrary \e objects - not
just text messages. Features for logging text is just an application build on the concept
of logging abstract \e 'Logables'.
This is not only true for the \e Logables provided in <em>Log Statements</em>, but also for
\ref alib_mod_alox_prefix_logables "Prefix Logables". More information about logging arbitrary
objects is found in \ref alib_mod_alox_ext_loggers.

\I{################################################################################################}
### B.5.2 Namespace 'textlogger' ### {#alib_mod_alox_apdx_auto_config_list_logables_textlogger}
When logging text, classes found in namespace \ref alib::lox::textlogger are doing quite a
bit of things in the background that are not obvious to the user. For example:

- \alox \alib{lox;ESC}-codes are removed from textual \e Logables if a \e Logger
  does not support them. (For more information, see \ref alib_mod_alox_colorful_loggers.)
- Classes derived from \e %TextLogger by default recognize multi-line text in \e Logables
  (in standard line ending, Windows line ending, or mixed mode) and format multi-line text accordingly.
- With the help of utility class \alib{strings::util;AutoSizes}, class \b %TextLogger
  formats the log output into columns that grow over time.
  At the moment a write-enabled configuration plug-in is attached (e.g. an INI-file), such sizes
  are restored when a process is re-launched. Also automatic shrinking happens. This is done
  when next run of a process detects that the tabulator and field sizes of a previous session it had
  used, have been higher than needed after all! In the next run, the smaller sizes are restored.
  Reading and writing of the values from and to the configuration system is performed in method
  \alib{lox::textlogger;TextLogger::AcknowledgeLox} which is invoked when a \e Logger is
  added, respectively removed from a \b %Lox.


### B.5.3 C++ Strings ### {#alib_mod_alox_apdx_auto_config_list_logables_cppstrings}
In \alox for C++, logging strings leverages the string facilities of underlying ALib, which allows
to transparently pass just any sort of string object to <em>Log Statements</em>, including
references or pointers to almost any custom string type - without any sort of explicit prior
conversion!
This is achieved using \ref alib_manual_appendix_tca "type traits".
Information on how to adopt custom string types is found in the
\ref alib_mod_strings "Programmer's Manual" of module \alib_strings_nl.


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# Appendix C: Preprocessor Symbols and Macros # {#alib_mod_alox_apdx_prepro}

## C.1 ALox Compiler Symbols ## {#alib_mod_alox_apdx_prepro_sym}

Compiler symbols, including those that control features of \alox, are documented with the
\ref GrpALibPreproSymbols "reference documentation" of \aliblong.



## C.2 ALox Macros ## {#alib_mod_alox_apdx_prepro_macros}

\alox for C++ makes extensive use of preprocessor macros.
Those mainly achieve:

- **Pruning of log code**<br>
  Using the logging macros provided, allows removing all \e <em>debug logging</em> selectively, or
  also <em>release logging</em> invocations from the code. (see \ref alib_alox_faq_pruning).<br>

<p>

- **Adding scope information**<br>
  \alox uses built-in standard preprocessor symbols like  "__FILE__" and "__LINE__" to provide
  information about the position of logging statement within the source. Especially for release
  logging, one problematic fact is that such logging source information could disclose internals
  if the release software unit was reverse engineered. Therefore, the use of source information
  is configurable and disabled for release logging by default.<br>
  Also, hiding the use of the built-in symbols by using dedicated \alox macros, reliefs the
  source code from a lot of clutter and overhead.

The macros and their documentation are divided into two different groups:<br>

- <b>ALox Macros For Debug Logging</b>
- <b>ALox Macros For Release Logging</b>

Documentation for those are found in the corresponding subsections of the
\ref GrpALibPreproMacros "reference documentation" of \aliblong.

While all macros for debug logging start with the prefix <b>Log_</b>, those for release
logging are using the prefix <b>Lox_</b>. Otherwise, their naming and parameters are the
same.<br>
The rest of the macro names in most cases corresponds to methods found in class \alib{lox;Lox}.

Both macro sets include macro #Log_Prune, respectively #Lox_Prune.
All that these two macros do is pruning the code that is passed to them at the moment that
debug logging (respectively release logging) is supposed to be pruned. This allows placing
code for setting up \alox, for example, the creation of custom loggers, which is pruned in
the moment the according log-statements are.

\note
  The provision of two sets of macros, one for <em>debug-</em> and one for <em>release logging</em>,
  imposes one of the differences to other \alox language implementations (<em>%ALox for C#</em> and
  <em>%ALox for Java</em>), as this allows pruning <em>release logging</em> independent of <em>debug logging</em>.

\note
  It might be important to understand the options: When release logging is only \e disabled,
  the code for release logging still remains in the binaries,
  which has an impact on their size and an (probably marginal) impact on their speed.
  In addition it is possibility to reverse engineer such binaries and learn about internals of the
  software (should someone do so).

\note
  In this respect, \alox for C++ is the most flexible implementation of \alox as two different release
  builds are possible: such with release logging incorporated and such with release logging pruned.<p>

For an introduction of how to use these macros, work yourself through the \ref alib_alox_tut.


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# Appendix D: IDE Setup for ALox for C++ # {#alib_mod_alox_ide_setup}

\I{################################################################################################}
## D.1 Introduction # {#alib_mod_alox_ide_setup_1}

The C++ version of \alox is distributed as an \alibcamp of \aliblong.
\note This is contrast to the currently discontinued versions of <b>ALox for C#</b> and
      <b>ALox for Java</b>, which are located in a
      [separated repository](https://github.com/AlexWorx/ALox-Logging-Library).

Documentation for compiling \alib is given with chapter \ref alib_manual_build "6. Building The Library"
of the \alib Programmer's Manual.


\I{################################################################################################}
## D.2. Using ALox with Different IDEs# {#alib_mod_alox_ide_setup_ides}
In the following sections, the use of ALox with different IDEs is discussed. Besides specifics
in respect to project setup, an overview about how \alox clickable log output is supported - and
achieved! "Achieving" here means most of all to configure the output format in a way that the
IDE "understands" the source file and line number information and links them to the editor and source.

To change the format of the log output, field \alib{lox::textlogger::FormatMetaInfo;Format} of the
\ref alib_variables_types_custom "configuration variable" can be modified. This can be done either
programmatically, or, if available in the external configuration of an application.
If you are new to \alox, the easiest way to play around with different log output formats, is to
compile and run the sample project. This will generate an INI-file (in your personal OS-specific
configuration folder) and within this file you will find an entry for configuration variable
\ref alxcvALOX_LOGGERNAME_FORMAT. You can edit this, and re-run the sample. The log output will
change. From here, you can start your own investigations about how to convince your IDE to accept
the log statements.

In the configuration file, there are also other configuration variables which can get useful to
adjust the log output to a certain IDE. If your IDE is not listed in the following paragraphs,
then still reading these might help you in understanding how such support is possible.


\I{################################################################################################}
### D.2.1. QT Creator## {#alib_mod_alox_ide_setup_qtcreator}

[QT Creator](http://doc.qt.io/qtcreator/index.html) can be used in two different "modes", with
CMake files and with QMake files. Unfortunately, in respect to clickable log-output there is a
tremendous difference in the behavior of QT Creator.

\note When using \alib classes in your project or you want to debug into or extend \alox,
      please note the special support of \alox for QT Creator pretty printers.
      Update: 02/2024: These were completely outdated and have been removed.
      We completely rely on JetBrains IDEs now.


\I{################################################################################################}
#### D.2.1.1 Using QT Creator with QMake ### {#alib_mod_alox_ide_setup_qtcreator_qm}

In respect to log output file name recognition, we have made the following observations:

- When using QTCreator with QMake (!), the IDE recognizes source code information if the line starts
  with three spaces and the string \c "Loc:".
- The source information is then to be provided with square brackets.
- no other square brackets must occur in the output line
- Any color information (ANSI color codes) which otherwise are recognized by QT Creator
  are removed. You can see colors flashing up shortly sometimes and then they disappear. The
  reason is obviously that QT Creator parses the output window and colorizes all source links it
  detects in blue - and removing all other color code!
- The rules how source paths are recognized by QTCreator is not known to us. Often,
  trimmed source file paths are sufficient (see chapter \ref alib_mod_alox_trim_source_path)
  but sometimes the full path has to be given. Especially if libraries that use ALox are involved,
  the trimmed path is sometimes not enough. In this case, a source path trim rule might be created
  that turns relative source file paths (provided by QMake to the C++ compiler) into absolute ones.

From these observations, the following advice can be given:
- A plain (non-colorful!) logger of type \alib{lox::loggers;ConsoleLogger} is to be used.
- The string \alib{lox::textlogger;FormatMetaInfo::Format} needs to start with
  \c "   Loc: [..." .
- No square brackets must be used in the meta-information (and the log output itself).
- In addition, all square brackets are removed from Fields
  \alib{lox::textlogger::FormatMetaInfo;VerbosityVerbose},
  \alib{lox::textlogger::FormatMetaInfo;VerbosityInfo},
  \alib{lox::textlogger::FormatMetaInfo;VerbosityWarning} and
  \alib{lox::textlogger::FormatMetaInfo;VerbosityError}.
- Finally, using method
  \alib{lox::textlogger;TextLogger::SetReplacement},
  replacement rules may be added which will replace all square brackets within the contents
  of <em>Log Statements</em> by curly braces.

Using external configuration this means to set three variables. For example, an INI-file might
contain the following entries:

    CONSOLE_TYPE=                      plain
    DEBUG_LOGGER_FORMAT=              "   Loc: [%SP/%SF(%SL)]:%A3%SM %A3{%TC +%TL}{%tN}{%D}%A1#%#: ",    \
                                       {ERR},                                              \
                                       {WRN},                                              \
                                      "     ",                                             \
                                       {***}
    DEBUG_LOGGER_REPLACEMENTS=        "[", "{", "]", "}"




\I{################################################################################################}
#### D.2.1.2 Using QT Creator with CMake ### {#alib_mod_alox_ide_setup_qtcreator_cm}

<em>QT Creator</em> has a built in CMake support which works really well with the CMake files
provided with the project (as of the time of this writing).

To use the pre-build CMake files of ALox with <em>QT Creator</em>, open menu \e File of
<em>QT Creator</em> and choose <em>Open File or Project...</em> and
select the file:

        ALIB_LIB_PATH/build/cmake/samples/ALox/CMakeLists.txt

Follow the project setup as <em>QT Creator</em> documentation suggests.
When the CMake run dialog appears, for debug-builds, no arguments need to be passed. For release
builds, provide

        -DCMAKE_BUILD_TYPE=Release

as the CMake Arguments

The ALox sample project is now ready to be build in <em>QT Creator</em>

In respect to log output file name recognition, there is some bad news now: When used with CMake
projects, QT Creator does not support clickable log lines. At least, we have not found out how!
Therefore, we have no recommendation about how to change the log output in respect to make
such output clickable.

The output panes of QT Creator support colorful (ANSI) logging and have a light background.
Therefore, we recommend to set the following two configuration variables:

    CONSOLE_TYPE=            ansi
    CONSOLE_LIGHT_COLORS=    background


\I{################################################################################################}
### D.2.2. Using ALox with Visual Studio## {#alib_mod_alox_ide_setup_vstudio}

When you are new to \alib and you just want to play around with it a bit, the easiest thing
is to open the Visual studio solution found in

        ALIB_LIB_PATH/build/ide.vstudio/Solution/ALib.sln

You are set! For example, you could walk through the \ref alib_alox_tut by adding the tutorial
code to the \c main() function of the project *ALox-CPP-Samples*.

ALox provides a Visual Studio project that compiles ALox into a DLL.
The projects is located in:

        ALIB_LIB_PATH/build/ide.vstudio/ALib-DLL/

Add this project to your solution and set a build dependency from your project to the just added
*ALib-DLL* project. Furthermore, the *ALib-DLL project* has to be added as a reference to
your project.
Finally, the \alib include directory has to be added to your project.

\note
  When using ALox in a Microsoft Windows software, it may be important to
  include <em>windows.h</em> before any ALox header-file. The reason is that \alib
  includes <em>windows.h</em> unless it was included already. When the \alib
  library includes <em>windows.h</em>, it uses some defines like <em>WIN32_LEAN_AND_MEAN</em>
  or <em>NOGDI</em> to minimize the impact of that huge header-file. If <em>windows.h</em>
  is included before including ALox (AWorx library), it is up to the embedding application
  which level of windows functionality is needed.


<b>Clickable Log Output with Visual Studio:</b><br>
As far as we know, clickable log output is only possible to have when launching a project
in debug mode. In this case, \alox automatically adds a second logger of type
\ref alib::lox::loggers::VStudioLogger "VStudioLogger". (This is the default behavior by default,
implemented with method \ref alib::lox::Log::AddDebugLogger "Log::AddDebugLogger".)

This logger does everything right to have clickable log lines in Visual Studio. No
adjustments need to be done.


\I{################################################################################################}
### D.2.3 Using ALox with JetBrains CLion ## {#alib_mod_alox_ide_setup_clion}

[CLion](https://www.jetbrains.com/clion/)s' build system relies on \e CMake.
Pre-configured project folders are provides with

    ALIB_LIB_PATH/build/ide.clion/samples/ALox
    ALIB_LIB_PATH/build/ide.clion/unittests

The \e CMake files within these directories are just including the corresponding \e CMake files
found in

    ALIB_LIB_PATH/build/cmake/samples/ALox
    ALIB_LIB_PATH/build/cmake/unittests

The advantage of this approach is that the "generic" folders found in folder \e cmake are
kept free from \e CLion project files.

Therefore, \e CLion users should be ready to compile by just opening these project folders.

For debugging \alib and \alox code, you might want to install the
\ref alibtools_debug_helpers_gdb "Pretty Printers for GNU Debugger".


<b>Clickable Log Output with CLion:</b><br>

\e CLion has an intelligent output console that supports ANSI colors and detects links to files
automatically. The file name without path is sufficient and this way, the default format string
should do fine.

There is one specialty about the ANSI color support: CLion does not support light colors.
Therefore, the use of light colors has to be switched off.
If this is not done, colors are in most cases not effective.

As a summary, these or similar configuration variable settings are recommended:

    [ALOX]
    CONSOLE_TYPE=           ansi
    CONSOLE_LIGHT_COLORS=   foreground
    DEBUG_LOGGER_FORMAT=    %SF:%SL:%A3%SM %A3[%TC +%TL][%tN][%D]%A1#%#: %V,    \
                            \e[31m,                                             \
                            \e[34m,                                             \
                            ,                                                   \
                            \e[33m


<br><br><br><br><br><br> */
