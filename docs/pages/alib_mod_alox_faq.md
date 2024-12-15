// #################################################################################################
//  Documentation - ALox Logging Library
//
//  Copyright 2015-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_alox_faq              FAQ - Frequently asked Questions for ALox
\tableofcontents
# Why the name "ALox" and how is it pronounced? # {#alib_alox_faq_the_name_alex}

The English term "log" is known. Now "logs" is the plural. As English pronunciation is always
a little unspecific and volatile, you could write the word "logs" as "lox" easily without
changing the pronunciation.  Now, as we were looking for some artificial new word for the project,
we loved to turn "logs" into "lox".

But then we wanted to add the "A". This tells you: **This is *class A* logging!**
Got it? So, please pronounce it in two syllables, with a separately spoken "a" like in "a car" or "a cat".

<em>Note: This explanation also proves that the name has nothing to do with this strange guy
named Alex who started this project.</em>

# What is the performance impact on my application when using ALox # {#alib_alox_faq_performance}

\alox is fast (see previous question on Bauhaus). Using a MemoryLogger, in C# we
measured more than 200.000 log entries per second on a Notebook (equipped with an Intel(R) Core(TM) i7 CPU @2.8GHz)
while running an application (and \alox) in debug mode! Hence, if doing as much as 100 log entries
per second, the performance loss for your application would still be a maximum of 0.05%.

Of course, logging into a console is slower. But the good news is: As the internal management of \alox
is so fast, there will not be a huge difference between hacked in debug log output using <c>std::cout</c>
versus using \alox logging. And: as for debug logging, this gets automatically pruned in the release
code. Hence, the release version of your software is free of any performance issue caused by
debug logging. 

# What do the terms "debug logging" and "release logging" mean?  # {#alib_alox_faq_debug_vs_release_logging}

The goals of <em>debug logging</em> and <em>release logging</em> are quite different.
<em>Debug logging</em> is about adding <em>Log Statements</em> to your code that enables you to see on the
terminal console, your IDEs output window or in a log-file,
what your software currently does or recently did, what errors and warnings it produces, how long certain
methods take to execute and whatever you want your software to tell you while testing it.
Debug <em>Log Statements</em> should ultimately be \e pruned (see next answer) from release binaries.

<em>Release logging</em> is about generating a log file, a log database, an online-report to a server or whatever
you choose during execution of your \b deployed software. Often, <em>release logging</em> is used for dealing with
problems that arise when your software is used by the target audience. It is also useful to
collect statistical data on the use of a software, identify problems that end-users might face, and so forth.
Sometimes, <em>release logging</em> is a subset of the debug log, sometimes it is something completely different.

Technically however, both logging use cases require very similar software. Therefore, \alox aims to
solve both in one library. Basic support for release logging is here today already. The introduction of new features and concepts
in the area of release logging might also support debug logging aspects of \alox and vice versa.
Therefore the natural path of evolution for the \alox library is "debug logging features first, release logging features next".

# What is Pruning? #  {#alib_alox_faq_pruning}

What we mean with the term "pruning" is the automatic removal of the (debug) logging code from
your release binaries when they are ready for deployment. The goals that motivate
for automatic pruning are:

- Maximize execution speed of the deployed code
- Minimize the footprint of the deployed code.
- Keep the console (or other standard output) clean in the release version of a software.
- Hide technical details and secrets, by not only stopping the output but by also removing all
  textual strings from your executable or library that include debug messages.

Automatic pruning ensures furthermore, that the above
stays true for all  <em>debug logging</em> statements while all <em>release logging</em> statements
remain in the code. Such remaining <em>release logging</em> might be deactivated initially and
activated in case of runtime errors, external triggers, or other special events.

# Why are ALox log formats all US/English? # {#alib_alox_faq_internationalization}

Of course the log content itself is unicode and hence supports your locale within
your application. But the following is true: All configurable output of meta-information is done in US/English locale.
There is no config option to change this. The reason is that log output is really something that is not for a different audience
than the software developers and here, US/English rules (just look at the keywords of your programming language).
And we think that a maximum of compatibility (e.g., with other tools or within international teams)
is guaranteed this way.<br>
If you absolutely want localized meta-information, like logging the name of months in Finnish,
you can extend \alox for that.
Here is \ref alib_mod_loggers_textlogger "how you do that"!

<br>
# Why is ALox introducing a huge set of own "utility classes"? # {#alib_alox_faq_aworx_util}
We would not say that \alox is "introducing" an own set of utility classes. \alox
only builds upon such a set called \aliblink.<br>
So, it is rather the other way round: \alox is just one \alibmod of many others found in
\alib. The library can be compiled to a subset of those modules, so, if you are you exclusively
interested in logging, other \alibmods_nl can be removed in a library compilation. Of course, still,
these \alibmods_nl that \alox depends on, need to be included.

All about what is offered and how an customized \alibdist is built is found in the
\ref alib_manual "ALib Programmer's Manual".

<br><br><br><br><br><br> */
