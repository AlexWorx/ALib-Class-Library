// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_alox_tut   ALox - Tutorial

\tableofcontents

# 1. Hello ALox # {#alox_tutcpp_hello_alox}

Welcome to the <b>ALox for C++ Tutorial</b>! This tutorial aims to introduce you to \alib_alox_nl as quickly as
possible and put you into a position to decide whether \alib_alox_nl is the right logging library for you.

Let us start with the very quick "hello world" sample:

\snippet ut_alox_dox.cpp        ALoxTut_Minimum_Hello

When this program is run, the output should look similar to this

        myproject/mysourcefile.cpp:7:main [0.001 +003 µs][PROCESS][/] #001: Hello ALox!

So, with one line of code we get a <em>Log Statement</em> that incorporates:
- The source file, line number and method, which - depending on the IDE you use - is 'clickable' which
  means a click in the output window opens the source file and highlights the line where the
  <em>Log Statement</em> is placed.
- The time that passed since the process started (here 0.001 which means a millisecond after start)
- The time that passed since the last log line: 3 microseconds. This information is handy for
  a first impression on the execution speed of your methods.
- The name of the thread executing the log (automatically identified as the \b main thread named
  \b PROCESS in this case).
- The <em>log domain</em> <b>"/"</b>, which is to be explained later.
- The log counter <b>#001</b> , which, for example, is useful to quickly find the right output lines in your
  next debug-run.
- Finally the log message itself.

In the release build of your software, this line of code is completely pruned away.
But, as we will see later, also in debug compilations you can <b>selectively disable</b> log output.
And this is not just done using "log levels" like "Info", "Warning", "Error", etc.
Instead,  log statements become a "domain" assigned and these are used to disable or enable the 
statements.<br>
This means: with \alib_alox_nl you can keep your debug <em>Log Statements</em> in your code forever!

Let us quickly start a new chapter of this tutorial and go into more details.

\I{################################################################################################}
# 2. Class Lox and ALox C++ Macros # {#alox_tutcpp_adding_ALox}

The previous chapter showed the shortest possible sample program which just bootstrapped
everything to reasonable default options.

\alib_alox_nl has some concepts that need a little more time to understand. Before we see more code,
we have to take some prerequisites.

\alib_alox_nl is designed to support <b>debug logging</b> (log output that should appear only in the
debug version of your software) as well as <b>release logging</b>. In C++ such pruning is (still )
done best by using preprocessor macros. Therefore, you will see a lot of macros in the code samples
given here. We want you to understand a little of the class design of \alib_alox_nl, while you
are reading yourself through this tutorial. This is why a very brief explanation of the macros is
given here:

- The most important interface into \alib_alox_nl is class \alib{lox;Lox}, which is a container to hold one or more instances of abstract class
\alib{lox;detail::Logger}. Through the interface of class \b Lox, the log statements are invoked.
Log statements are forwarded to the \e Loggers attached.

- The most simple case is that you have just one \b Lox instance, holding exactly one \e %Logger which logs its output to the
standard output stream of your process (e.g., \alib{lox::loggers;AnsiConsoleLogger}).
\alib_alox_nl supports much more complex scenarios and the preprocessor macros must support this as well.<br>
In this tutorial we stick to the simple case, maybe adding a second \e Logger here and there.

- Most of the macros that we are using here have the form "Log_Xyz()". What these macros are doing is basically two things:
  - Invoke the method Xyz() on a predefined singleton of class *Lox*. This predefined singleton is the default (configurable)
    container for all debug logging output. Macros of this type will be pruned in release compilations of your software.
  - Pass scope information to the \alib_alox_nl, so that the file name, line number and method name of the occurrence of a log
    statement can be included in the log output.

  So, when you see the line

        Log_Info( "Hello ALox" )

  This means that method \alib{lox::Lox;Info} is invoked on the predefined singleton of class \b Lox which
  is by default used for debug logging.

- In addition to the macros that actually perform a <em>Log Statement</em> or otherwise interface with class \b Lox,
  macro \ref Log_Prune exists, which in the release version of your software, removes any code it gets passed.
  This macro is needed for things that do not represent single invocation of methods of class *Lox*, e.g.:
  - creation and manipulation of \e Loggers
  - changing log output format
  - code to assemble more complex log messages before logging them

In the tutorial language we do not further explain these macros. So do not be surprised when the presented sample code
shows a macro, while the explaining text refers to the method invocations that are represented by these macros.
For a information on \alib_alox_nl macros for logging, please refer to Programmer's Manual appendix
chapter \ref alib_mod_apdx_prepro_macros "C.2 ALox Macros".

\I{################################################################################################}
# 3. Create a tutorial project or equip your current project with ALox #   {#alox_tutcpp_createproject}

If you want to follow the tutorial samples and play around with it a little, you should
either create a tutorial project or right away equip your current project with \alib_alox_nl.
For information about how you do that see chapter \ref alib_manual_build of the Programmer's Manual
of \aliblong.

\note
  Of course, you can skip this step and continue \e reading the tutorial
  without setting up \alib_alox_nl now. For some IDEs, there might be a
  \ref alib_mod_ide_setup "ready to use solution" available that minimizes efforts when you want
  to follow the samples and play around a little.

To include all headers necessary for this tutorial, add the following statements to your source file:

\snippet ut_alox_dox.cpp        ALoxTut_include_statement

And add a statement to use namespaces \c std and \ref alib to your source file:

\snippet ut_alox_dox.cpp        ALoxTut_using_statement

\note Although \alib and \alib_alox_nl are organized in several inner namespaces (below #alib respectively
      alib::lox), it is enough in most cases to just <em>'use'</em> namespace \b %alib.
      The mechanics and design goal are explained in the documentation of namespace \ref alib.


\I{################################################################################################}
# 4. Class Logger ##  {#alox_tutcpp_logger}

While class \b %Lox is the main interface to \alib_alox_nl logging, abstract class
\ref alib::lox::detail::Logger  "Logger" is performing the log. 'Performing' here means that
derived instances of this abstract class are responsible for writing the \e 'Logables' (the message
or data that is logged) to a 'drain', for example the console or a text file.

Before we can start logging, we need to create one or more \e %Loggers and attach them to
our instance of class \b %Lox.

\note
  In the shortest sample shown above (\ref alox_tutcpp_hello_alox "Hello ALox") we have not done this. For debug
  logging \alib_alox_nl notices this and automatically creates a \e %Logger.


You need to identify the right place in your application where to do this.
A good place is the "main()" method or any other place of your project where some "bootstrapping"
of your application takes place.
Add the following line:

\snippet ut_alox_dox.cpp        ALoxTut_Logger_1

This tries to identify the best \e Logger type that fits to your console type.
For example, if the process runs on an ANSI enabled console or a console window on Windows OS, 
colorful log output is enabled. Sometimes, this method even
creates two \e Loggers: one for the console and one for your IDE output, when \alib_alox_nl is able to
detect and support such IDE.<br>
For information about what is currently supported and how (with environment variables,
configuration files or command line parameters) this behavior can be changed, see methods
\alib{lox;Log::AddDebugLogger} and \alib{lox;Lox::CreateConsoleLogger} which this macro invokes.

We never interface with a \c Logger directly. After it is created and added to the \c Lox, we
can mostly forget about it.
For almost all the rest of this introductory tutorial, all you
need to know is that \e Loggers created with this method, write
log messages in textual format e.g., to standard output stream (and/or to the application console).
As said above, other \e Loggers could be added in parallel that use other "drains" to write the
output to, e.g., files, a metrics server in the cloud, etc.

Together with bootstrapping the library and the first <em>Log Statement</em> of the introductory
sample, your projects code (e.g., in method \c main) should look like this:

\snippet ut_alox_dox.cpp ALoxTut_Logger_2


\I{################################################################################################}
# 5. Run the Application #  {#alox_tutcpp_run}

When you run your project, output similar to this:

\verbinclude pages/generated/ALoxTut_Logger.txt

should appear in your IDE or your console.

\note
  From the meta-information of the log output above you can see, that this tutorial has its
  sample code embedded in unit tests and the output is captured when running them and inserted
  here.

\I{################################################################################################}
## 5.1. Run your application within your IDE ##        {#alox_tutcpp_run_ide}

If you are using an IDE supported by \alib_alox_nl (e.g., Visual Studio in Windows), you can double-click the log line in the output
window. The IDE  opens the source and selects the line of code where the log call was invoked.
This means, each <em>Log Statement</em> is "clickable" in your IDE and links to its source code line in the
editor. This is a tremendous help for developers when debugging their code.

Besides the fact that you can click it, users of \alib_alox_nl are saving even more time. Developers, when
using standard "debug print lines", often phrase textual info like:

        "attn: dbman.savedata(): oops could not store data....ERROR!"

hence, putting some info about the place in the code, where they have put the debug statements. As you see from the
\alib_alox_nl output above, information about the *caller* of the <em>Log Statement</em> is all included automatically.
Advantages are:

- lines are clickable and link to the editor (as mentioned above)
- less to type when adding debug print lines
- no need to change the <em>Log Statement</em> when you copy/paste a debug print line to a different class/method
or when you rename your class/method

Furthermore, as we will see later, the log message type (which seems to be "ERROR!" in the phrase above) is standardized
in \alib_alox_nl and does not need to be phrased in your message text.

Besides scope information and message type, you automatically get:

- the relative time of the log execution (time since application started)
- the time elapsed since the last log
- the ID of the thread that executed the method

The output format and detail of the log line is configurable in \alib_alox_nl. The line above just shows
the default. Details you see can be removed or other details can be added (e.g., the absolute
date / time instead of only the relative).

\note Should your IDE not be supported by \alib_alox_nl in respect to generate "clickable log output lines" (aka lines that
link automatically to the right point in your source code) it might be the case that only by adjusting
the log output format a little, your IDE learns how to read and interpret \alib_alox_nl logging!
<p>

\note
You should adjust your IDE in a way that the output window spans over the complete
width of the main window. Also, depending on your screen size, you might decrease the font size of
the output window a little. The output gets wide, because a lot of valuable information is logged
besides the log message. In addition, the output is organized in columns that auto adjust their size.
This takes a wider output into account for the  benefit of largely improved readability.


\I{################################################################################################}
## 5.2. Build a release version ##                              {#alox_tutcpp_run_release}

Switch your project configuration to "Release" and run the application. The output should not appear!

Even better: The \alib_alox_nl code is not even compiled into the release target. So, whatever you
are logging out during developing and debugging your software, it is all automatically gone in the
release version.

This is achieved by providing the right set of \ref GrpALibPreproSymbols "compiler symbols" when
\ref alib_manual_build "setting up the project".
Only project configurations that have the symbol \ref ALOX_DBG_LOG set to \c 1 will contain \alib_alox_nl
debug logging statements.

This has a lot of benefits:
- your release code executes faster,
- your release executable gets a smaller footprint and
- you are not delivering your debug language to the end user (not even if your executable is reverse engineered).


\I{################################################################################################}
# 6. Controlling the 'Verbosity' #     {#alox_tutcpp_verbosity}

The code above uses the method \alib{lox;Lox::Info} (embedded in a preprocessor macro) to create the
log output. There are three other Versions of that method, together constituting the four <em>'Verbosities'</em>:

- \alib{lox;Lox::Error}
- \alib{lox;Lox::Warning}
- \alib{lox;Lox::Info}
- \alib{lox;Lox::Verbose}

Let us see what happens when extending our sample as follows:

\snippet ut_alox_dox.cpp        ALoxTut_Verbosity

If you run your application now (in "Debug" mode), the following output should appear:

\verbinclude pages/generated/ALoxTut_Verbosity.txt

The little space in the meta-information, right after the thread name [main] fills
with text-markers <b>[ERR]</b>, <b>[WRN]</b> and <b>[***]</b>, giving info about
which verbosity a <em>Log Statement</em> defines.<br>
In this sample, we are using just one source file, thread name, etc. When more variety
appears, \alib_alox_nl will automatically set tabulators in the meta-information of the log output,
so that this information is almost always at the same column. This makes it very easy for example
to identify those log messages of \e Verbosity 'Error'.

\note
  If on your machine, \alib_alox_nl detects a console that supports colorizing the log output,
  method \b %Log.AddDebugLogger will choose a colorful implementation of class \b %Logger and
  you will see the different output lines in different colors instead of these little text-markers.
  This helps 'visually filtering' log lines even better.

Now, we want to control the \e Verbosity of the log output. Let's say we just want to see
'Error' and 'Warning' messages, and suppress those of \e Verbosity 'Info' and 'Verbose'.
\alib_alox_nl allows controlling this on a 'per \e %Logger' basis using method
\ref alib::lox::Lox::SetVerbosity "Lox::SetVerbosity". We add the line:

\snippet ut_alox_dox.cpp        ALoxTut_Verbosity_SetVerbosity

Here, we are explicitly passing the \e Logger object that \alib_alox_nl creates as a static singleton
for debug-logging purposes, with method \b AddDebugLogger().
Instead of passing the reference to the object, we could as well use a \e Loggers' name.
The \e Logger that method \b AddDebugLogger() creates is named \c "DEBUG_LOGGER". Therefore, the
line of code shown above can alternatively stated as:

\snippet ut_alox_dox.cpp        ALoxTut_Verbosity_SetVerbosity_2

Our sample now looks like this:

\snippet ut_alox_dox.cpp        ALoxTut_Verbosity_2

and produces the following output:

\verbinclude pages/generated/ALoxTut_Verbosity_2.txt

As you see, only the <em>Log Statements</em> with \e Verbosity 'Error' and 'Warning' survived.

Obviously \alib_alox_nl uses the attribute \e Verbosity, defined in enum class
\ref alib::lox::Verbosity "Verbosity" two times:
- every <em>Log Statement</em> has a \e Verbosity assigned.
- A \e Logger attached to a \e Lox has a \e Verbosity assigned.

Then \alib_alox_nl matches both attributes to decide whether a <em>Log Statement</em> is executed with a \e Logger or not.

\note
  Some notes on method \b SetVerbosity:
  - The first time this method is called for a \e %Logger, this method internally 'registers'
    the given \e %Logger with the \b %Lox object. In other words, there is no other method
    (and no need!) to register a \e %Logger with a \b %Lox.
  - To switch off all log output, use \e Verbosity.Off.
  - To remove (for any reason) a \e %Logger from a \b %Lox, use method
    \ref alib::lox::Lox::RemoveLogger "Lox::RemoveLogger".
  - Method \b AddDebugLogger, by default sets \e %Verbosity.Verbose for the \e Logger created.


Now, we are able to control the \e Verbosity of the \b 'overall' log output of our application.
But probably, in bigger projects, we need more \b 'granularity' for such control. The next section tells how!


\I{################################################################################################}
# 7. Log Domains  #     {#alox_tutcpp_logdomains}

Controlling the \e Verbosity of the log output, including switching it completely off,
is of course a core feature of any logging library.
\alib_alox_nl allows controlling such \e Verbosity for different <b>Log Domains</b>. A <em>Log Domain</em> can be seen
like a key-word associated with just every <em>Log Statement</em>. This way, <em>Log Domains</em> are sorting each and every
<em>Log Statement</em> used in an application into a specific \b subset.

Until now, we have not used and set any <em>Log Domain</em> in our samples. We have just omitted the parameter
in our <em>Log Statements</em> and this way the parameter defaulted to an empty string. Look at the following
two <em>Log Statements</em>:

\snippet ut_alox_dox.cpp        ALoxTut_Domains

As you see, two parameters are given now. The first denotes the <em>Log Domain</em>. The string provided can
be arbitrarily chosen. Let us quickly look at the log output:

\verbinclude pages/generated/ALoxTut_Domains.txt

In the meta-information of the log output, just before the line number, formerly appeared
<c>[/]</c> (what we did not explain, yet). Now it says <c>[/HTTP]</c>, respectively <c>[/UI]</c>.
Obviously this field of the meta-information denotes the <em>Log Domain</em> associated with the <em>Log Statement</em>.

The <em>Log Domains</em> chosen in this sample obviously group the <em>Log Statements</em> of the application into two sets, one
for <em>Log Statement</em> concerning information about the 'user interface', the other about HTTP communication.
Now, we can control the \e Verbosity of the two sets independently. We are using another
default parameter that we previously omitted. Imagine, the UI of your application works well, but
you have some problems with HTTP connections:

\snippet ut_alox_dox.cpp        ALoxTut_Domains_2

Now the output is:

\verbinclude pages/generated/ALoxTut_Domains_2.txt

Although both <em>Log Statement</em> share the same \e Verbosity, only the one that belongs to <em>Log Domain</em> "HTTP" is shown.

The next section tells us more about <em>Log Domains</em>. You might already guess what this is
when looking at the meta-information of the log output, showing
<c>[/HTTP]</c>, <c>[/UI]</c> and <c>[/]</c> for the <em>Log Domains</em>!


\I{################################################################################################}
# 8. Hierarchical Log Domains  #     {#alox_tutcpp_logdomains_hierarchical}

\alib_alox_nl organizes <em>Log Domains</em> hierarchically. A first advantage of this is that it becomes easy to switch
\e Verbosity of a whole set of <em>Log Domains</em> by controlling the parent.

Look at the following sample:

\snippet ut_alox_dox.cpp        ALoxTut_DomainsHierarchical

and its output:

\verbinclude pages/generated/ALoxTut_DomainsHierarchical.txt

We can use a slash (<c> '/'</c>) to separate <em>Log Domains</em> and organize them in a tree structure, just as
we do with directories in a file system. In the sample above, <em>Log Domains</em> \b "DLG" and \b "MOUSE" are
Sub-<em>Log Domains</em> of <em>Log Domain</em> "UI".

With this information, it is important to understand that method
\ref alib::lox::Lox::SetVerbosity "Lox::SetVerbosity" always sets the given <em>Log Domain</em> and
<b>all its Subdomains</b> to the \e Verbosity value provided. Consequently, the following statement
switches <em>Log Domains</em> \b "UI", \b "UI/MOUSE" and \b "UI/DLG" to the same \e %Verbosity.Warning:

\snippet ut_alox_dox.cpp        ALoxTut_DomainsHierarchical_2

If we were interested how mouse events are processed in our application, we might do invoke:

\snippet ut_alox_dox.cpp        ALoxTut_DomainsHierarchical_3

The order of these two statements is important: If they had been written the other way round,
then the setting of <em>Log Domain</em> \b "UI" had overwritten that of <em>Log Domain</em> \b "UI/MOUSE".

\note
  You might wonder why there is no second version of the method available (or an optional parameter)
  that allows manipulating only the <em>Log Domain</em> given, without touching its Sub-<em>Log Domains</em>.
  There are good reasons for this and these are explained in the \ref alib_mod_alox "Programmer's Manual".
  It is also explained there, that there is a way to stop recursion and in which situations this
  might be useful.
  But for now and in standard situations: The setting of \e Verbosity is recursive!

Almost everywhere \alib_alox_nl accepts a <em>Log Domain</em> name as a parameter, a <b>domain path</b> can be given.
This even works with the dot and dot-dot syntax that we know from file paths. Also, \b relative
and \b absolute path names can be given. Here are some samples:

        "DOM"          // a relative path name
        "./DOM"        // same as "DOM"
        "/DOM"         // an absolute path name
        "/DOM/XYZ/.."  // same as "/DOM"
        "/DOM/.."      // same as "/"
        "DOM/XYZ/.."   // same as "DOM"
        "DOM/./XYZ"    // same as "DOM/XYZ"

Now, we are able control the \e Verbosity of a (sub-)tree of <em>Log Domains</em>.
This enables us to write very complex software and still manage log output far beyond the classical
"log level" way, that other logging libraries offer. Remember: The primary goal of \alox is that
you never again remove any log statement. You keep them forever, and should your code be perfect,
well, then you disable them forever. In case something goes wrong, even years after the initial
creation of the code, the log statements in the concrete area of concern can be simply activated
again.

But the concept of <b>hierarchical organization of <em>Log Domains</em></b> is even more powerful,
as you will learn in the next section.

\I{################################################################################################}
# 9. Scope Domains  #     {#alox_tutcpp_scope_domains}

As we saw, optional parameter \p{domain} of <em>Log Statements</em> allows us to group the log output into
different areas of interest and control the \e Verbosity per group. This is nice, but
- We have to type more
- We have to recall the 'actual' <em>Log Domains</em>' name when we insert a <em>Log Statement</em>
- The <em>Log Statement</em> become less easier to read (remember: <em>Log Statement</em> should fit to your code like comment lines)
- When copy/pasting code, all <em>Log Domain</em> might have to be changed to the destinations' domain name
- When we want to change a <em>Log Domain</em> name, we have to change it everywhere we use it.

To avoid all this (and gain even more cool features) \alib_alox_nl provides a feature called <b>Scope Domains</b>.

The term \e Scope is known from your programming language. For example, a variable declared
within a class method, 'automatically' carries the scope of that method. This means, it is not
visible outside of that method. \alib_alox_nl uses a similar approach: All <em>Log Statements</em> within a method might
carry the same <em>Log Domain</em>. In other words, we can set a 'default value' for a <em>Log Domain</em> to be used for
all <em>Log Statements</em> of a method.

The interface in \alib_alox_nl which provides this feature is found with the set of overloaded versions of
\ref alib::lox::Lox::SetDomain "Lox::SetDomain". Here is a sample:

\snippet ut_alox_dox.cpp        ALoxTut_ScopeDomains

and its output:

\verbinclude pages/generated/ALoxTut_ScopeDomains.txt

You see, all disadvantages we just listed are blown away using <em>Scope Domains</em>.

In the sample above, we have used \e %Scope::Method. Another type is \e %Scope::Filename which
"binds" a given <em>Log Domain</em> path to all <em>Log Statements</em> within a source file. As with scopes in your programming
language, \e Scopes in \alib_alox_nl are nested and of course \e %Scope::Filename is an <b>'outer Scope'</b>
of \e %Scope::Method. For a single <em>Log Statement</em>, both <em>Scope Domains</em> might apply at the same time. With having
hierarchically organized <em>Log Domains</em>, \alib_alox_nl concatenates all <em>Scope Domains</em> applicable. Look at the following
sample:

\snippet ut_alox_dox.cpp        ALoxTut_ScopeDomains_Zipper

and its output when both sample methods are executed:

\verbinclude pages/generated/ALoxTut_ScopeDomains_Zipper.txt

Imagine, the source file of class Zipper resides in a source directory, together with other
'utility classes'. Somewhere in these classes, probably at the place where this 'utility library'
is initialized (once, when bootstrapping a process), the following statement might be added:

\snippet ut_alox_dox.cpp        ALoxTut_ScopeDomains_Path

This sets <em>Log Domain</em> "UTIL" for \e %Scope.Path, which is again an <b>outer scope</b> of \e %Scope.Filename.
Without changing the code in class Zipper, invoking its methods then leads to the output:

\verbinclude pages/generated/ALoxTut_ScopeDomains_Zipper_Path.txt

What happens when <em>Scope Domains</em> are set and we still use optional parameter \p{domain} with a <em>Log Statement</em>? Let
us just try out:

\snippet ut_alox_dox.cpp        ALoxTut_ScopeDomains_ParamDom

\verbinclude pages/generated/ALoxTut_ScopeDomains_ParamDom.txt

As you see, the path provided with parameter \p{domain} gets appended to the path of <em>Scope Domains</em> evaluated.
You can consider this parameter being a 'local' \e Scope, or an <b>inner</b> scope of \e %Scope::Method!

Finally, imagine you want to 'overwrite' current applicable <em>Scope Domains</em> and direct a certain <em>Log Statement</em>
to a different <em>Log Domain</em>. All you need to do is to use an absolute domain path with parameter \p{domain}:

\snippet ut_alox_dox.cpp        ALoxTut_ScopeDomains_ParamDom_2


\verbinclude pages/generated/ALoxTut_ScopeDomains_ParamDom_2.txt

In this sample, the second <em>Log Statement</em> uses an absolute path. The reason is that the error dectected
in the code, obviously belongs to a different topic. The sample suggests, that it is not related
to reading a file, but it is related to a wrong configuration. So, the log output goes directly
to the corresponding domain.

This is enough about <em>Scope Domains</em> in the context of this tutorial. All details about
<em>Log Domain</em> and <em>Scope Domains</em> are found in the \ref alib_mod_alox "Programmer's Manual".
Among other things, you will find:
- Information on a \e %Scope.Global
- How to use "outer \e Scopes" of \e %Scope.Path, which allows setting <em>Log Domains</em> for a complete subtree
  of your source files at once
- Information on "thread-related" \e Scopes, which allow to change <em>Log Domain</em> on a per thread basis in different ways.
  For-example, this allows creating log output only if a certain thread runs a code.
- More samples and use cases, that make \alox the most powerful and unique logging library.


\I{################################################################################################}
# 10. Formatting #   {#alox_tutcpp_formatting}
While talking about rather complicated things like <em>Log Domains</em> and \e Scopes, we must not
forget to talk about the log output itself. \alox is designed to be able to pass any type of
data to one or more \e Loggers. In the default case, a textual output of these \e "Logables" is wanted.
We have seen in previous samples already, that if two objects are passed, their textual representation
is just concatenated:

\snippet ut_alox_dox.cpp        ALoxTut_Format1
\verbinclude pages/generated/ALoxTut_Format1.txt

But in fact, this concatenation is just the "fallback" strategy of a quite powerful formatting
system coming with \alox. In short, \alox here relies on underlying library \alib, which
provides abstract class \ref alib::lang::format::Formatter "Formatter"
allowing to format a set of arguments in accordance with a given format string that contains placeholder symbols.

Now there are two tricks implemented: First, if no format string is recognized, a simple
concatenation is performed. This is what we already saw. But secondly, if one formatter does not
recognize a format string, a next formatter can be asked.

Two formatters are provided by \alib and \alox (by default) installs both in parallel:
1. \ref alib::lang::format::FormatterPythonStyle "FormatterPythonStyle"<br>
   Implements an extended version of
   [Python String Fromat Syntax](https://docs.python.org/3.5/library/string.html#format-string-syntax),
   which is also similar to what C# offers.
2. \ref alib::lang::format::FormatterJavaStyle "FormatterJavaStyle"<br>
   Implements the
   [formatting standards of the Java language](http://docs.oracle.com/javase/8/docs/api/java/util/Formatter.html),
   which in turn are a little like good old \c printf, but of course much more powerful and also type-safe.

With this background information, it is no surprise that the line above can alternatively be
written like this:

\snippet ut_alox_dox.cpp        ALoxTut_FormatP

which is Python compatible syntax, or like this:

\snippet ut_alox_dox.cpp        ALoxTut_FormatJ

which is how Java formatters declare format strings!

If more arguments exists than a format string "consumes", the next remaining argument is treated
like the first one: if a format string is detected it is processed, otherwise the textual representation
of the argument is just appended to the log output. Because of this, the following statements
all produce the same log output:

\snippet ut_alox_dox.cpp        ALoxTut_Format_Multi

Of course it is even allowed to mix Python style and Java style format strings in one log statement:

\snippet ut_alox_dox.cpp        ALoxTut_Format_Mix

The output is:

\verbinclude pages/generated/ALoxTut_Format_Mix.txt

However, please note that it is \b not allowed to mix Python and Java styles within one format
string!

In general, we are recommending to use Python style syntax with \alib_alox_nl, because it is more powerful and
probably also better readable. The full documentation of the formats and how this is adopted
within the C++ version of <b>%ALib/%ALox</b> is found with documentation of classes
\ref alib::lang::format::FormatterPythonStyle "FormatterPythonStyle" and
\ref alib::lang::format::FormatterJavaStyle   "FormatterJavaStyle".

Here are a few more samples:

\snippet ut_alox_dox.cpp        ALoxTut_Format_Sample_1
\verbinclude pages/generated/ALoxTut_Format_Sample_1.txt

The formatting system found with \alib module \alib_basecamp provide a concept which is similar
to method <c>ToString()</c> in the Java language or similar concepts in other languages: It allows
to define a textual representation for just any C++ Type.
It needs a little effort, but once in place you can drop just any of your objects into a
log statement. This relieves you of getting the single attributes out of an object and write
complicated long format strings, again and again.

Next, it is even possible to define new placeholder "languages" for the formatters. As a sample, for
class \alib{time;DateTime} such custom extension exists with the inclusion of
\alibheader{lang/system/calendar.hpp}:

\snippet ut_alox_dox.cpp        ALoxTut_Format_Sample_2

The result is:

\verbinclude pages/generated/ALoxTut_Format_Sample_2.txt

Details on how to do this are given in chapter \ref alib_basecamp_format_custom_types
of the Programmer's Manual of module \alib_basecamp.


\I{################################################################################################}
# 11. Conditional logging  #   {#alox_tutcpp_conditional}

## 11.1. Lox::Assert  ##

Often log lines should only be displayed if a certain condition is met. Here is a
sample:

\snippet ut_alox_dox.cpp        ALoxTut_ConditionalLogging

The last two lines can be replaced by one using method \alib{lox::Lox::Assert} as follows:

\snippet ut_alox_dox.cpp        ALoxTut_ConditionalLogging2

Advantages are again less typing and better readability of the code. Furthermore, the C++ compiler
would probably not prune the *if statement* if it was a more complex evaluation.
As with using *Assert()* the evaluation of the expression is definitely pruned from your code.
(Be sure you do not put side effects into expressions of *Assert()* invocations).

\note
- In accordance with the concept of assertions, the condition has to be \c false to have \alib_alox_nl perform the log.
- The \e Verbosity of *Assert()* is *Verbosity::ERROR*.

## 11.2. Lox::If  ##

A similar method to \b %Lox::Assert this is \alib{lox;Lox::If}. The differences to \b Lox::Assert are:
- The given condition is interpreted the other way round: if \c true, the <em>Log Statement</em> is executed.
- The \e Verbosity of the <em>Log Statement</em> is not fixed but can (has to) be specified with parameter \p{verbosity}.

Hence, the very same effect as given in the previous sample can be achieved with:

\snippet ut_alox_dox.cpp        ALoxTut_ConditionalLogging3

## 11.3. Log.Once  ##

Another useful <em>Log Statement</em> provided by \alib_alox_nl is \alib{lox;Lox::Once}.
As the method name suggests, the statement

\snippet ut_alox_dox.cpp        ALoxTut_ConditionalLoggingOnce

will lead to a log output only the very first time that it is executed. This seems simple, and well
it is - as long as you omit all optional parameters. There are quite a bit, which makes
this statement extremely flexible.

All details are given in a dedicated chapter of the \ref alib_mod_alox "Programmer's Manual". Let us
therefore just summarize some facts:
- A \e Verbosity and <em>Log Domain</em> can be given (of course)
- A counter to increase the 'once' to 'n-times' can be given.
- The method can be used to log a message every n-th time.
- A key-word can be given, which combines different of such <em>Log Statement</em> to a set. Then the counter
  applies to this set.
- Instead of a keyword, a \e Scope can be given. This implements "Log once per this \e Scope" or
  "Log n-times per this Scope"
- A combination of a keyword and a \e Scope can be given. This implements
  "Log those statements of this group in this \e Scope n-times"


\I{################################################################################################}
# 12. Prefixes  #   {#alox_tutcpp_Log_Prefix}

Imagine, all log statements of a of a certain kind should be very visible and distinguishable
from other output. You could do this by
- giving them a separated color
- Indent them with spaces
- Start each log line with a clearly visible 'marker text'.

These things can be achieved with \alib_alox_nl using method \alib{lox;Lox::SetPrefix}.
With this method, additional \e Logables are passed to \e %Loggers attached to a \b Lox.
When logging text messages (we have not even talked in this tutorial about <b>logging arbitrary
objects</b>) these objects are simply prepended to the log message itself.

Of course, <em>Prefix Logables</em> can be defined according to \alib_alox_nl \e Scopes, language-related or thread-related ones.
This means, you can do things like this:
- Add a prefix to all statements of method, source file or source directory
- Add a prefix to all statements executed by a certain tread.

Often, it is simpler to assign a <em>Prefix Logable</em> to a <em>Log Domain</em>. Also this is possible with overloaded versions of
the method.

The real simple sample looks like this:

\snippet ut_alox_dox.cpp        ALoxTut_Prefix

\verbinclude pages/generated/ALoxTut_Prefix.txt

For colorizing (depending on the availability of a console that supports colors) the following
prefix would be suitable:

\snippet ut_alox_dox.cpp        ALoxTut_Prefix_2

We cannot easily show colorful sample output in this tutorial. Try this yourself. \alib_alox_nl supports
colorized output on ANSI consoles (GNU/Linux, etc.) and Windows OS command windows.

More and more complex use cases are elaborated in the \ref alib_mod_alox "Programmer's Manual", for
example it is explained how log output can be 'recursively indented' with very simple statements.
Recursive indentation is very helpful when logging in recursive algorithms or when
structured, composed data objects are logged.

The next chapter shows that \alib_alox_nl has already built-in mechanics for logging out
structured data!


\I{################################################################################################}
# 13. LogTools: Log Complex Data ##       {#alox_tutcpp_logtools}

Ooopps, unfortunately, we have to (almost) skip this chapter in the C++ Version of \alib_alox_nl.
While C# and Java provide runtime type information and class reflection (introspection),
C++ does not.
Therefore, we are missing the extremely handy LogTools class in C++, which in the other \alib_alox_nl incarnations recursively
log complex objects automatically.<p>
Therefore, if you want to *dump* more complex objects you have to extend your classes (and probably their subclasses) with
corresponding helper functions that assemble a log string.<p>
What helps to have a clear presentation of objects dumps within your log files are the concepts of <em>Prefix Logable</em>
providing recursive indentation and *multi-line* logging that \alib_alox_nl provides.<p>

Here is a sample output from the Java LogTools class that gives you and idea how nicely a dumped
object might look, if recursive indentation and tabulators are used:<p>

\attention This is Java code, taken from the \alib_alox_nl for Java tutorial!

\verbatim
// Log multi-lines without meta info
((TextLogger) Log.getLogger( "Console" )).multiLineMsgMode= 4;

// Log current thread instance
LogTools.instance( "MYDOM", Verbosity.INFO, Thread.currentThread(), 4, "Actual Thread: " );
\endverbatim


As you can see, we are just passing a quite complex object, namely the current thread, to the method. The parameter
that follows the object which is to be logged determines the level of recursively logging composite objects.
Here are the first lines of output of the above sample:

\verbatim
>> Actual Thread:                                                    {Thread}
>> <01>  name:                         "main"                        {[C}
>> <02>  priority:                     5                             {Integer}
>> <03>  threadQ:                      <null>
>> <04>  eetop:                        140565756819456               {Long}
>> <05>  single_step:                  false                         {Boolean}
>> <06>  daemon:                       false                         {Boolean}
>> <07>  stillborn:                    false                         {Boolean}
>> <08>  target:                       <null>
>> <09>  group:                                                      {ThreadGroup}
>> <10>    name:                       "main"                        {String}
>> <11>    maxPriority:                10                            {Integer}
>> <12>    destroyed:                  false                         {Boolean}
>> <13>    daemon:                     false                         {Boolean}
>> <14>    vmAllowSuspension:          false                         {Boolean}
>> <15>    nUnstartedThreads:          0                             {Integer}
>> <16>    nthreads:                   2                             {Integer}
>> <17>    threads:                    array[8]                      {[Ljava.lang.Thread;}
>> <18>      0: (Cyclic ref., see line <00>)                         {Thread}
>> <19>      1:                                                      {com.intellij.rt.execution.application.AppMainV2$1}
>> <20>      2: <null>
>> <21>      3: <null>
>> <22>      4: <null>
>> <23>      5: <null>
>> <24>      6: <null>
>> <25>      7: <null>
>> <26>    ngroups:                    0                             {Integer}
>> <27>    groups:                     <null>
>> <28>  contextClassLoader:                                         {sun.misc.Launcher$AppClassLoader}
>> <29>  inheritedAccessControlContext:                              {java.security.AccessControlContext}
>> <30>    context:                    <null>
>> <31>    isPrivileged:               false                         {Boolean}
...
...
...
\endverbatim

When implementing an object dump, you should not place a <em>Log Statement</em> for
every attribute. In contrast, you should collect the whole dump within an \alib{AString}
(or any string-type you like) and then log this String as a whole.

So, currently class \alox{LogTools} just provides one static method for logging an
\alib{lang;Exception}.

For future versions of \alib_alox_nl for C++, it is planned to provide:
- Working with debug information to implement log tools for user-defined classes
- Preprocessor macros that standardize the customized logging of complex objects
- Tools to log certain standard classes from STL, BOOST, QT or what have you.
You are invited to send us your proposals of log tool code for common libraries to be included in our codebase.


\I{################################################################################################}
# 14. Name Your Threads  #       {#alox_tutcpp_threadnames}

In multithreaded applications, \alib_alox_nl by default logs information about the thread id or
name that executed the code containing your log statements. (This can be configured for
\alib_alox_nl textual \e Loggers with class \alib{lox::textlogger;FormatMetaInfo}.)

Often threads do not have a name, but just an ID. It would be much nicer to see a
name instead. \alib_alox_nl provides a simple mechanism to overwrite thread names with method
\alib{lox;Lox::MapThreadName}.

Let us go back to the Hello ALox sample and add one line to rename the currently executed thread:

\snippet ut_alox_dox.cpp        ALoxTut_MapThreadName

The generated output is:

\verbinclude pages/generated/ALoxTut_ThreadName.txt

As you see, the currently executed thread is now named *BKGRND*. All <em>Log Statements</em> that will
be executed by this thread after the mapping takes place, will log this name.

\note Renaming threads in \alib_alox_nl will *not* rename the thread in your operating system. It is a
      purely \alib_alox_nl defined mapping of threads to \alib_alox_nl thread names.


The mechanism is simple, but it might not be simple to identify the right place in
the code to put it! Some code can be executed from different threads, and sometimes
(for example when using an UI framework) you might not even know exactly which thread will invoke
your code. However, the good news is that each thread ID has to be mapped only once during
the lifecycle of the program, but may be mapped multiple times. Therefore, the guideline is:

- For threads that your own code starts, the name mapping should occur in the start code of course.
- Otherwise: Identify a piece of code from which you are sure that only one certain thread invokes that code.
- Make sure that this code is not executed frequently (to avoid the overhead caused by the duplicated mapping ).
- Make sure that this code is executed early in the lifecycle of your application, so that all log lines are
equipped with the right thread name mapping.

For the sample of UI Frameworks, a good place to invoke \alib{lox;Lox::MapThreadName}
is the initial creation callback of your main application UI component. Normally, it is
enough to put this statement only in one component (the main one), because all other components
will be initialized by the same Framework thread.

But as this is a convenience concept anyhow, it is good practice to not care too much
about it at first. Later, when your application grows, you can check your log files periodically for new, unmapped
thread IDs. When you see one, scroll up in the log file and try to identify the very first appearance
of this new ID. A double-click on the log line will open the code that invoked the log.
If you have populated your code with a reasonable amount of log entries, you will end up at the
right place automatically! It also might be a good idea to restart your app with all domains set
to \e Verbosity.Verbose and then look for the first appearances of unknown threads.

\I{################################################################################################}
# 15. ALox Log Data #      {#alox_tutcpp_logdata}

We hope that you are fully aware when reading through this tutorial, that all debug-log statements
that are sampled here, are \b pruned from the release executables.
One of the great benefits of \alib_alox_nl is that this removal of the statements is done quite
automatically (depending on the language version you are using). Once, you have set-up your projects
rightfully, your next release built is silent and does not contain your \alib_alox_nl <em>Log Statements</em>.

The concept of <b>Log Data</b> leverages this feature of \alib_alox_nl to allow to create variables
that are existing exclusively with debug-builds of your software. You can consider this
feature as a "low hanging fruit" - not necessarily related with logging - but for \alib_alox_nl easy
to implement and for the user easy to use. As the access to <em>Log Data</em> is exclusively done through the
\alib_alox_nl API, the data is nicely 'encapsulated' and the probability of creating side effects is
reduced in comparison to other ways to introduce temporary variables used for debugging.

The methods to store and retrieve <em>Log Data</em> with \alib_alox_nl are
\ref alib::lox::Lox::Store "Lox::Store" and
\ref alib::lox::Lox::Retrieve "Lox::Retrieve". The objects stored are of type
\ref alib::boxing::Box "Box". In the C++ implementation of \alox it is notable, that in the case
that the data provided is not a value type (in respect to \alib_boxing, the
data has to be kept in memory as long it is stored and potentially retrieved.

Here is a sample code which sets a debug variable in \alib_alox_nl providing a version of a file that
was read by an application:

\snippet ut_alox_dox.cpp        ALoxTut_LogData

Now, whenever it might be interesting, this file version can be accessed:

\snippet ut_alox_dox.cpp        ALoxTut_LogData_2

The output will be:

\verbinclude pages/generated/ALoxTut_LogData.txt

If it was not sure, that <em>Log Data</em> was set, then using macro \ref Log_Retrieve as shown
above, allows doing checks on the object returned, because it is declared as a local variable.
When you are sure, that the retrieval returns an object of the type you expect, then the retrieval
of the data can also be inlined. In this case, macro \ref LOG_LOX should be used to name
the right debug \b %Lox:

\snippet ut_alox_dox.cpp        ALoxTut_LogData_3


\I{################################################################################################}
# 16. ALox Configuration Information and Internal Log Messages #      {#alox_tutcpp_config}

In complex projects it might get a little confusing to keep track about the
\e Loggers and their \e Verbosity, <em>Scope Domains</em>, <em>Prefix Logables</em> and <em>Log Data</em> set.

More importantly, when other software components of your project
are using \alib_alox_nl as well (defining their own domains) then you might not know exactly which
<em>Log Domains</em> get registered and used by those.
The same applies when working in a team.            


Let us create a sample, do some \alib_alox_nl configuration and then invoke method
\ref alib::lox::Lox::State "Lox::State":

\snippet ut_alox_dox.cpp        ALoxTut_LogState

\b %Lox::State gathers internal configuration information and logs it out.
The output is quite self explanatory:

\verbinclude pages/generated/ALoxTut_LogState.txt

Besides this, there is a <b>second option</b> to inspect what happens internally in class *Lox*.
\alib_alox_nl is equipped with an internal <em>Log Domain</em>. This domain is used by \alib_alox_nl to
log messages about itself! All we have to do, is setting the \e Verbosity of the internal
domain to verbose for our debug \e Logger:

\snippet ut_alox_dox.cpp        ALoxTut_LogInternalDomains

The output will be:

\verbinclude pages/generated/ALoxTut_LogInternalDomains.txt

This way, you can exactly observe what is going on inside \alib_alox_nl.

To summarize: We have to ways to look into \alib_alox_nl:

1. Method \alib{lox;Lox::State} logs a "snapshot" of the current states. The advantage of this is,
   that it is all logged sequentially in one place and does not clutter your log output.
2. By setting the *Verbosity* of the internal <em>Log Domain</em>
   \alib{lox;Lox::InternalDomains} to a more verbose level. While this clutters your log output and you
   might have to search the pieces in your overall log stream, the advantage here is that you see the scope information and therefore
   you see "where" a certain manipulation of the settings took place...and you can click on it!

\note
    The internal <em>Log Domain</em> found in static, constant field \alib{lox;Lox::InternalDomains}
    is not just a domain name string. In fact it specifies a completely different <b>domain tree</b>
    which is not related to those domains created when using \alib_alox_nl. Even when changing
    the \e Verbosity for a \e Logger on the root domain \c /, this domain is not affected, because
    it is just not a part of the general domain system. The advantage is that you cannot 'accidentally'
    switch this domain on or off.
    In other words, the general domain tree is completely kept free for the users of \alib_alox_nl.


\see
    \alox configuration variable \ref alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT provides
    a way to automatically log state information when a \b %Lox gets deleted.


# Further Reading #     {#alox_tutcpp_end}

This is the end of the tutorial of \alib_alox_nl for C++. You should be able to use the basic features of the \alib_alox_nl logging ecosystem and
have nice, formatted, configurable log output in your software projects. \alib_alox_nl puts you in the comfortable position to
keep your debug <em>Log Statements</em> in your code, because you can easily configure not only the verbosity as a global
parameter, but for different <em>Log Domains</em> separately. When you start editing code that you have not touched
for a while, you just can enable the corresponding <em>Log Domain</em> and restrict others to warnings and errors for a while.
Because of the efficient \alib_alox_nl implementation, disabled <em>Log Domains</em> do not have a relevant impact on the execution of your
debug code. As soon as you release your code, debug logging is pruned completely from your binaries.

In the \ref alib_mod_alox "Programmer's Manual" you will find all details of the \alib_alox_nl features along
with information on use cases and often explanations why a feature is designed as it is. Some
important things not covered in the tutorial are:

- <b>Release Logging</b><br>
  Release logging is very similar to debug logging. The differences and how to use release-logging is
  described in a \ref alib_mod_debug_and_release_logging "dedicated chapter" of the Programmer's Manual.
- <b>External Configuration</b><br>
  Instead of setting \e Verbosities and other changeable configuration "from within the source"
  (by adding \alib_alox_nl API invocations to your software), such data can also come from INI files,
  environment variables, command-line parameters or a any custom source that your application
  uses. This is especially important on larger projects, when working in teams.<br>
  A summary of all \alib_alox_nl configuration variables is found in reference documentation
  \ref alib_alox_cfgvars "ALox Configuration Variables".
- <b>Trimming Source File Paths</b><br>
  For C++ and C# users: \alib_alox_nl automatically trims paths of source code files. This can
  be optimized, as described in chapter \ref alib_mod_trim_source_path of the Programmer's Manual.
- <b>Log Domain Substitution</b><br>
  What this is, is described chapter \ref alib_mod_domain_substitution of the Programmer's Manual


If however, you just want to start using \alib_alox_nl and get to know more details over time, you should
only read \ref alib_mod_ide_setup and probably section <b>"ALox Macros For Debug Logging"</b>
in \ref GrpALibPreproMacros.

Finally, for the daily work, the \ref alib::lox "ALox class reference" gives a lot of help!



<br><br><br><br><br><br> */
