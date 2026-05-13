// #################################################################################################
//  Documentation - ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_camp   ALib Module Camp - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1\. Introduction # {#alib_camp_intro}

This module is the fundament for the subset of \alibmods that we call \alibcamps_nl.

\par Please Read Now:
    To avoid redundancy, we would ask you to read the short chapter
    #"alib_manual_modules_camps" of the Programmer's Manual of \alib, which gives an
    explanation to this statement.
                                       
The three key aspects mentioned in the link above — 1) managing externalized resources, 2) accessing
external configuration, and 3) defining a bootstrap process — are relevant
to most software, even to small command-line tools.
However, these areas are not well-supported by the C++ language and also the introduction of
C++20-Modules left these challenges unresolved.

This module addresses these issues by introducing the virtual interface #"Camp", which
is designed to handle these three concerns.
Other \alibmods_nl already integrate this interface, and custom code units can also derive one
or more types from it as needed.

In traditional application design, there is typically a centralized application class managing
these responsibilities. From a broader perspective, \alibcamps_nl offer an alternative by allowing
the delegation of these tasks to the specific code entities responsible for them.
Put simply, conventional software design often relies on each code unit being aware of a
central application class that provides the necessary interfaces.
This can lead to circular dependencies between the application class and the code units.
One of the primary goals of this module is to eliminate such design limitations.
   
 
\I{################################################################################################}
# 2\. Class Camp # {#alib_camp_camp}
To implement the proposed functionality, class #"Camp" relies on the aggregation
of functionality stemming from other \alibmods_nl.<br>
Let us look at them one by one:

\I{################################################################################################}
## 2.1 Resource Management ##
For resource management, the class provides a pointer to an instance of the virtual class
#"ResourcePool" defined in the module \alib_resources.
Derived types can use the inherited methods
- #"^Camp::GetResource;*",
- #"^Camp::TryResource;*", and
- #"^Camp::GetResourcePool;*"

to access externalized string resources conveniently.

In addition, this module extends the concepts imposed by the module \alib_resources 
by a resource compiler.<br>
All details are explained in the chapter #"alib_camp_rc".

\I{################################################################################################}
## 2.2 Configuration Data ##
For external configuration data, the class provides a pointer to an instance of
class #"Configuration", defined in the module \alib_variables.
Derived types can retrieve this instance with method #"Camp::GetConfig;*" and with that
get access to \alib runtime variables.

In this context, one core feature of \alib_variables_nl is that their values can transparently
emerge from
- command-line parameters,
- environment variables,
- configuration files, or
- any other external configuration system.

This gives access to such data at the place where it is needed.

Note that runtime variables can also be defined by other code entities to share information
between modules.

\I{################################################################################################}
## 2.3 Bootstrapping and Shutdown ##
Implementing a well-defined bootstrap and shutdown process is the responsibility of the
overarching module \alib_bootstrap. The namespace functions #"alib::Bootstrap(BootstrapPhases);2" 
and #"alib::Shutdown;2"
change their signature, depending on the inclusion of this module \alib_camp_nl in the \alibbuild.
Without the inclusion of any \alibcamp_nl, a heavily simplified bootstrap and shutdown process is
in place, which calls corresponding bootstrap- and shutdown-functions
of certain non-camp modules (among only a few other things).

The altered versions of the functions allow to either share or not share resources and configuration
data between the different \alibcamps_nl.
This is why the internal field members #"^Camp::resourcePool" (inherited) and #"Camp::config"
are implemented as pointer types.

Furthermore, the altered functions divide the processes into different phases. For each phase,
they invoke the abstract virtual methods #"Camp::Bootstrap", and
#"Camp::Shutdown" which have to be implemented by derived classes.<br>

Finally, the bootstrap functions also make sure that the built-in \alibcamps_nl are initialized
in the right order, namely from lower-level camps to higher ones. The same applies for the
shutdown process, but in reverse order.

All information about
- how default bootstrapping works,
- how an application can alter this default to adjust the distribution of different configuration
  and resource management instances, and
- how a using code may inject own \alibcamps_nl into these processes,

is documented with the #"alib_mod_bs;Programmer's Manual of module ALib Bootstrap".

When developing custom camps, the Programmer's Manuals of the modules \alib_resources and
\alib_variables should be consulted as well.
A good jump-start to copy from could be the straightforward
#"^F;filetreecamp.cpp;reference implementation" found in the \alib_filetree camp.
 
\I{################################################################################################}
# 3\. The Basecamp # {#alib_camp_basecamp}
In the introduction to this manual, it was explained that the \alib_camp_nl module introduces
the #"Camp" class.
Each module designed to be an \alibcamp must provide a singleton instance derived from this class.

Below is a summary of the currently available camps in \alib:

Module                         | Camp Class                              | Singleton Instance
-------------------------------|-----------------------------------------|--------------------------
\alib_alox                     | #"alib::lox::ALoxCamp;3"                | #"alib::ALOX;2" 
\alib_camp                     | #"alib::camp::Basecamp;3"               | #"alib::BASECAMP;2" 
\alib_app                      | #"alib::app::AppCamp;3"                 | #"alib::APP;2" 
\alib_expressions              | #"alib::expressions::ExpressionsCamp;3" | #"alib::EXPRESSIONS;2" 
\alib_filetree                 | #"alib::filetree::FilesCamp;3"          | #"alib::FILETREE;2" 
       
As shown in the table, this very module creates its own singleton instance with #"alib::BASECAMP;2".
Consequently, it qualifies as an \alibcamp_nl itself.

A natural question might arise: why would a module like this, which mainly provides the interface
class #"Camp", require any extensive setup, resources, or configuration?

The answer lies in the way this module subtly enhances functionality. When included in the
\alibbuild, it replaces some lower-level resources, primarily related to
#"alib_enums_records;ALib Enum Records".
Without this module, these records are usually hardcoded or just unavailable.
By incorporating this module, the build process dynamically constructs these records during the
module's bootstrap phase.
This approach takes advantage of the externalized resources managed by the \b Basecamp class,
doing away with the hard-coded implementation via preprocessor directives.<br>
This improvement applies not just to enum records but also to a variety of format strings and
exception-related messages (notably in the \alib_format and \alib_system modules).

Additionally, during bootstrap, the \b Basecamp class reads and processes external
#"alib_cfgvars".

For those curious about the full range of tasks this module performs,
#"^F;basecamp.cpp;the source code" of the \b Basecamp implementation serves as a useful reference.

For developers looking to create custom camp-modules, it is recommended to begin with the much
simpler and more straightforward #"^F;filetreecamp.cpp;reference implementation" found in
the \alib_filetree camp.

\I{################################################################################################}
# 4\. Resource Compilation # {#alib_camp_rc}
             
This \alibmod extends the concepts of resource management introduced in the module \alib_resources
by introducing a resource compiler for external resource description files of type `.alibrc`.

This allows resources to remain editable text files during development while still being converted
into C++ code for production builds. The concept is deliberately flexible:
- <b>Built-in development-time compilation</b>: class #"camp::DevtimeResourceCompiler" may parse
  a `.alibrc` file during program startup, register the resources immediately and, optionally,
  patch a C++ source file with generated bulk-load code.
- <b>Load-only development mode</b>: if the optional parameter `cppFileName` of
  #"DevtimeResourceCompiler::Do" is omitted, resources are always taken directly from the
  `.alibrc` file and no source file is modified. This is useful for fast test cycles because no
  recompilation is needed after changing resources.
- <b>External build-step compilation</b>: the standalone tool `ALibRC`, located in directory
  `tools/ResourceCompiler`, performs the same source generation outside the running application
  and can be integrated into custom workflows or build systems such as CMake.

With these choices, projects may switch freely between immediate in-process loading, automatic
source patching during development, and explicit pre-build compilation.


## 4.1 Class DevtimeResourceCompiler ## {#alib_camp_rc_rc}
The development-time compiler, implemented by class #"camp::DevtimeResourceCompiler", processes
`.alibrc` resource files and registers the contained resources in a camp's resource pool.
Optionally, it also generates C++ code that loads these resources efficiently at runtime.
This combines the benefits of:
- <b>Maintainability</b>: resources are defined in editable `.alibrc` text files..
- <b>Performance</b>: production builds can avoid runtime file I/O and parsing.
- <b>Flexible workflows</b>: the same resource description can be used in-process or in external
  build steps.
- <b>Type safety</b>: resources integrate with \alib's type-safe resource access.

## 4.2 The .alibrc File Format ## {#alib_camp_rc_rc_fileformat}
The authoritative description of the `.alibrc` syntax is given with function
#"LoadResourceFile". The following summary reflects that parser and serves as an overview.

Each resource starts on a non-empty, non-comment line and follows the general scheme:
\code
Key [=] Value
\endcode
with optional whitespace around the optional \c '=' sign.

<b>Keys</b>
- Keys consist of printable ASCII characters and end at whitespace or \c '='.
- The double quote character is not allowed in keys.
- The maximum key length is 511 characters.

<b>Comments</b>
- A line whose first non-whitespace characters are \c '#' or \c '//' is ignored.

<b>Value Forms</b>
- <b>Quoted values</b>: start with \c '"' and preserve leading and trailing whitespace.
- <b>Plain values</b>: consume the rest of the line, with trailing whitespace removed.
- <b>Block scalars</b>: start with \c '|' or \c '>' and continue on following indented lines.
- <b>Compact scalars</b>: start with \c '~' and ignore whitespace outside quoted portions, which is
  useful for machine-parsed token streams that should remain human-editable.

<b>Escapes</b>
- Backslash escapes such as <c>\\n</c>, <c>\\t</c>, <c>\\\"</c> and <c>\\\\</c> are processed.

<b>Block Scalars</b>
- <c>'|'</c> preserves line breaks.
- <c>'>'</c> folds line breaks to spaces, while empty lines create paragraph breaks.
- Block content ends when indentation falls back to the indentation level of the key line
- Trailing empty lines are ignored.

<b>Compact Scalars</b>
- <c>'Key ~'</c> defines a compact scalar on the same line.
- <c>'Key ~|'</c> starts a compact block that may span several indented lines.
- Outside quoted parts, whitespace is ignored completely.

<b>Example .alibrc file:</b>
The following example is taken from the resource-compiler tool itself:
\snippet{lineno} "tools/ResourceCompiler/alibrc.alibrc" DOX_ALIB_SAMPLE_ALIBRC

For precise parsing rules and edge cases, consult the API documentation (or implementation) of 
the function #"LoadResourceFile;3".

## 4.3 Workflow Options ##

The following workflow variants are supported:

<b>Variant 1: Built-in compilation and source patching</b>

During development, the configuration macro #"ALIB_CAMP_RESOURCE_COMPILATION" may be defined.
Then, at application startup, #"DevtimeResourceCompiler::Do" can

1. resolve the `.alibrc` file path, mostly given relative to the file calling the function
   (specified with preprocessor macro \c __FILE__)
2. If a cpp-file to patch is given, compare the file dates. If the cpp-file is newer than the 
   resource file, do nothing.
2. Parse the resource file,
3. register the resources in the camp's resource pool, and
4. patch a designated C++ source file with generated bulk-load code.

This keeps the generated code synchronized while the application is run during development.

<b>Variant 2: Built-in load-only mode</b>

If #"DevtimeResourceCompiler::Do;2" is invoked without a `cppFileName`, the `.alibrc` file is
always parsed and loaded into the resource pool, but no C++ file is touched.
This mode is especially useful while testing resource texts, because modifications to the
`.alibrc` file are picked up on the next start without requiring a recompilation and linking 
of the executable.

<b>Variant 3: External build-step compilation</b>

As an alternative, the dedicated executable `ALibRC` found in `tools/ResourceCompiler`
can be built and invoked separately. This allows resource compilation to become an explicit custom
build step, for example, in CMake-based projects. Compilation and installation details of that tool
are documented #"alibtools_rc".

<b>Typical built-in usage in camp bootstrap:</b>

\snippet  "tools/ResourceCompiler/alibrc.cpp" DOX_ALIB_SAMPLE_DEVTIMERC_INVOCATION

The method #"DevtimeResourceCompiler::Do;2" takes:
- `alibrcFileName`: path to the `.alibrc` resource file
- `callingFile`: reference file for path resolution, typically `__FILE__`
- `campInstance`: the camp whose resource pool receives the resources
- `resourceCategory`: category name for resource registration
- `allowReplacements`: determines whether duplicate keys may replace existing entries
- `cppFileName`: optional path to the C++ file to patch

If #"ALIB_CAMP_RESOURCE_COMPILATION" is not defined, #"DevtimeResourceCompiler::Do" becomes a
no-op and returns `false`. In such builds, resources are typically provided by generated code that
was produced earlier, either by the built-in development-time workflow or by the external tool.

<b>Example fallback method:</b>
\code{.cpp}
    void Sample::bulkloadResources() {
        APP.GetResourcePool().BootstrapBulk( APP.ResourceCategory,
        
        // ALIB-RESOURCE-COMPILER-REPLACEMENT-START
         
        // ALIB-RESOURCE-COMPILER-REPLACEMENT-END
        
        nullptr );
    }
\endcode

## 4.4 Target File Requirements ##

If a C++ file is to be patched, the file specified in `cppFileName` must contain two special
marker comments:

\code
// ALIB-RESOURCE-COMPILER-REPLACEMENT-START
... generated code replaces this section ...
// ALIB-RESOURCE-COMPILER-REPLACEMENT-END
\endcode

All content between these markers is replaced with generated resource loading code.
The markers themselves are preserved. The generated code uses #"ResourcePool::BootstrapBulk"
with properly escaped string literals.

This requirement applies both to #"DevtimeResourceCompiler::Do" when `cppFileName` is given and to
the external tool `ALibRC`.

For a complete example, see the resource compiler tool's #"F;App/sample.cpp;source code itself".
The tool defines resources to implement its command-line interface. 


## 4.5 Timestamp-Based Re-compilation ##

When source patching is enabled, the compiler uses file modification timestamps to avoid
unnecessary work:
- If the C++ file is newer than the `.alibrc` file (with 10ms tolerance), no action is taken
- This allows checking out or copying files without triggering recompilation
- Only when the `.alibrc` file is genuinely modified does regeneration occur

In load-only mode, this optimization is intentionally bypassed, because the `.alibrc` file is
always read and registered at startup.

When regeneration is needed, a message is logged:
\code
CAMP/RESCMP: Compiling resources:
   Source: /path/to/sample.alibrc:1
   Target: /path/to/sample.cpp:1
\endcode

## 4.6 Integration with Camps ##

The typical integration pattern in a camp's bootstrap method:

\code{.cpp}
void MyCamp::Bootstrap(BootstrapPhases phase) {
    if (phase == BootstrapPhases::PrepareResources) {
        camp::DevtimeResourceCompiler rc;
        if (!rc.Do("myresources.alibrc", __FILE__,
                   *this, this->ResourceCategory, false,
                   "mycamp.cpp")) {
            // Patching didn't occur:
            // production build, source already up to date, or an error.
            // In such cases, load the previously generated code instead.
            bulkloadResources();
        }

        // Bootstrap enum records and other resource-dependent components
        enumrecords::bootstrap::Bootstrap<MyEnums>();
    }
}
\endcode

For quick test cycles, the same code may omit `cppFileName`. In that case, the fallback method is
usually not needed, because resources are taken directly from the `.alibrc` file on each start.

The external tool `ALibRC` provides the equivalent patching step outside the application.
Its basic usage is:
\code
alibrc resourcefile cppfile
\endcode

It also supports validation-only runs:
\code
alibrc --validate resourcefile [cppfile]
\endcode

## 4.7 Error Handling ##

The resource compilers perform validation and report errors with file/line information:
- <b>Illegal resource name</b>: Non-ASCII or invalid characters
- <b>Name too long</b>: Exceeds 511 character limit
- <b>Line ended in quotes</b>: Missing closing quote
- <b>Duplicate resource name</b>: The same key appears more than once in the input
- <b>Missing markers</b>: Target C++ file lacks required markers
- <b>File not found</b>: `.alibrc` or target C++ file does not exist
- <b>Access problems</b>: input or target files are not readable or writable

Errors are reported using #"alib_mod_assert;ALib's error reporting system" with the 
category `"CAMP/RESCMP"` or, when using the external tool, by corresponding command line error
messages and exit codes.

## 4.8 Best Practices ##

1. <b>Choose the workflow deliberately</b>: use load-only mode for fastest text iteration, 
   built-in patching for convenient local synchronization, or `ALibRC` for explicit build integration
2. <b>Version control</b>: Commit both `.alibrc` files and generated C++ sections
3. <b>File organization</b>: Place `.alibrc` files alongside the C++ files they generate or the 
   camp sources that consume them
4. <b>Relative paths</b>: Use relative paths and `__FILE__` for portability
5. <b>Comments</b>: Use comments in `.alibrc` files to document resource purpose
6. <b>Testing</b>: Test both the direct `.alibrc` loading path and the generated-code path to ensure they stay synchronized

\I{################################################################################################}
# 5\. Extensions For ALib Variables # {#alib_camp_variables}
The inclusion of this module with the header #"F;ALib.Camp.H", injects various overloads
of the function #"CampVariable(camp::Camp&);2" into the namespace #"alib::variables;2"  
of the module \alib_variables.<br>
These functions are inline shortcuts used for creating variables associated with the
#"Configuration" instance found in a #"Camp".

*/
