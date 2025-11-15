// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_assert    ALib Module Assert - Programmer's Manual

The \alib assertion system serves as the backbone for runtime validation across various \alib modules.
It provides a flexible mechanism for enforcing conditions, outputting diagnostic information, and
handling unexpected states during execution of debug-builds of the library.<br>
While primarily intended for internal use, developers working with \alib can also utilize this
system in their own code, should they find it suitable for their requirements.

This system is designed to ensure consistency and reliability in debugging and validation across
\alib modules. Its modularity allows it to be extended and customized, but it remains lightweight
and easy to integrate into existing workflows.


<b>Custom Printability with Conversion Functions</b><br>
The assertion system includes functionality for converting values into human-readable strings,
even for complex or custom types.
This is achieved through the type alias \ref alib::assert::AnyConversionFunc, a function pointer
that enables conversion of <c>std::any</c> objects to <c>std::string</c>.
\alib modules rely on this to display meaningful debug information for internal types.
If required, custom conversion functions can be registered using
\ref alib::assert::RegisterPrintable, extending this feature to user-defined types.

<b>Assertion Output Format</b><br>
A predefined format string, \ref alib::assert::FORMAT, governs how assertion messages are
structured. The default format, compatible with IDEs such as JetBrains CLion, links messages
in the output window to source code locations. It can be customized if you need to adapt the
output for specific IDEs.

<b>Stream Management</b><br>
To segregate error, warning, and informational messages, the system provides dedicated streams:
\alib{assert;STREAM_ERRORS}, \alib{assert;STREAM_WARNINGS}, and \alib{assert;STREAM_MESSAGES}.
These streams default to <c>std::cerr</c> or <c>std::cout</c>, but can be redirected to custom
destinations.

<b>Raising Assertions and Warnings and sending Messages</b><br>
At the heart of the system lies the templated function \alib{assert;Raise}, which processes
events based on caller information, the message type, and variadic arguments.
This central function is responsible for formatting and routing messages to the appropriate
output streams.

<b>Custom Assertion Handling</b><br>
The system includes a mechanism for overriding its default behavior.
By setting the function pointer \ref alib::assert::PLUGIN, developers can replace the standard
assertion output mechanics with custom one.
For example, you could use this feature to integrate assertions with external logging systems or
monitoring tools.
Module \alib_alox provides such a plugin. Further information is given with
the chapter \ref alib_mod_alox_debug_and_release_logging_ft_4 of the Programmer's Manual of that
module.

<b>Preprocessor Macros</b><br>
A suite of macros, such as \ref ALIB_ERROR, \ref ALIB_WARNING, or \ref ALIB_ASSERT_ERROR, simplifies
the use of the assertion system. These macros encapsulate common assertion patterns, collect
the \alib{lang;CallerInfo} struct, prune the code away with release-compilations, etc.
Developers can use these macros directly in their custom code, if they choose to adopt the
assertion system.

<b>Conclusion</b><br>
While primarily intended for internal use, its flexibility allows developers to leverage it
for their own projects if desired.

Its features, such as custom printability, configurable output formats, and stream management,
make it adaptable to a variety of needs without imposing itself as a mandatory or universal solution.

The \alib assertion system ensures robust runtime validation and debugging capabilities.
While designed as an internal utility for \alib modules, the assertion system is flexible enough
to be employed in custom projects.
Developers can extend its functionality by:
- registering custom conversion functions for user-defined types,
- change the output format,
- redirecting output streams, or
- using a custom plug-in, for example, to attach custom monitoring

However, this is entirely optional; the primary role of the system is its use within \alib itself.
<br><br><br><br><br><br> */
