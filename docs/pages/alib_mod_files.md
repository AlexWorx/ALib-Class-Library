// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_files   ALib Module Files - Programmer's Manual


\tableofcontents

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 1. Introduction # {#alib_files_intro}

This \alibmod provides mechanics to scan directories and contained files.
The scan results are collected in an instance of \alib{files;FTree}, which inherits
class \alib{containers;StringTree}.
Using classes \alib{containers;StringTree::Cursor} and \alib{containers;StringTree::RecursiveIterator},
two very comfortable interfaces to accessing the results are available.

As of today, besides scanning files, no specific further functionality is given and it is up
to the user of the module to do with the result lists whatever is intended.

While the \ref alib::files "reference documentation" of the types found in this module is
quite verbose and thus should be all that is really needed, this Programmer's Manual
just provides some few step-by-step demo samples.


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 2. Tutorial # {#alib_files_tut}
\I{################################################################################################}
## 2.1 Scanning a File Tree  ## {#alib_files_tut_scan}
A simple application needs to include just headers \alibheader{files/ftree.hpp} and
\alibheader{files/fscanner.hpp}:
\snippet "DOX_FILES.cpp"     DOX_FILES_HEADER

For scanning a path including its subdirectories, a few objects are needed:
\snippet "DOX_FILES.cpp"     DOX_FILES_OBJECTS

Here are the links to the reference documentation of these objects:
- \alib{files;FTree},
- \alib{files;ScanParameters},
- \alib{files;ResultsPaths} and
- \alib{monomem;TMonoAllocator} located in module \alib_monomem.

Note that the sample code above uses preprocessor symbol <b>ALIB_BASE_DIR</b>, which is defined
with the \alib unit test project, that this documentation uses to generate the samples.

That is all we needed to start the scan, which is done using namespace function \alib{files;ScanFiles}.
Next we use namespace function \alib{files;DbgDump}, which as its prefix <em>Dbg</em> indicates,
is only available in debug-compilations of the library:

\snippet "DOX_FILES.cpp"     DOX_FILES_SCAN

The resulting output is:

\snippet "DOX_FILES_MAIN.txt"     OUTPUT

You might wonder about the <c>resultPath</c> vector given as an output parameter into function
\alib{files;ScanFiles}.
Especially the question is: Why is it a vector? Wouldn't it be just the requested start path?

Let's quickly examine the result:
\snippet "DOX_FILES.cpp"     DOX_FILES_RESULTS

This writes:

\snippet "DOX_FILES_RESULTS.txt"     OUTPUT

So, in fact, it is only one result and it is here called "real path" is exactly the path that
was requested to be scanned.
But this can be quite different, when symbolic links come into place.
Details on this topic is given in the with reference documentation of function \b ScanFiles
and instead to repeat this here, we ask the reader for
\alib{files;ScanFiles;a brief digression to this explanation} now.

\I{################################################################################################}
## 2.2 Using Filters With Scanning  ## {#alib_files_tut_scan_filter}
We left all fields of class \alib{files;ScanParameters} with their default values in the previous
sample. Now we want to look at fields:
- \alib{files::ScanParameters;DirectoryFilterPreRecursion}
- \alib{files::ScanParameters;DirectoryFilterPostRecursion}
- \alib{files::ScanParameters;FileFilter}

Again, please refer to the reference documentation of the fields linked above, to get a quick
understanding, why the scan function offers to set up to three different filter objects.

Class \alib{files;FFilter} is a very simple virtual abstract interface class, which only has
one single method \alib{files::FFilter;Includes} to implement. Derived filter types, need
return \c true, if a file or directory "passes" the filter, hence in this case is included in
the scan results.

It should be very straight forward to implement an own derived a filter class. The problem
with such class would be, that it would be more or less "hard-coded" in respect to what is filtered
and what not. This might be flexible enough for most applications.<br>
The next chapter introduces a filter which is run-time compiled!

\I{################################################################################################}
## 2.3 Class FileExpressions  ## {#alib_files_tut_fex}
In case module \alib_expressions is included in the \alibdist, this module exposes class
\alib{files;FileExpressions}. This class implements a \alib{expressions;CompilerPlugin} dedicated
to articulate run-time expressions working with \alib{files;FInfo} objects.

The class exposes public inner type \alib{files;FileExpressions::Filter} which implements the
\b %FFilter interface. With construction, the filter accepts a character string containing the
filter expression.

The full set of expression operators, functions and constants is documented with class
\alib{files;FileExpressions} and not to be repeated here. While expressions might return any
kind of type, those used with class \b %FileExpression::Filter, have to evaluate to a boolean
value. As documented with module \alib_expressions, due to the type-safe implementation of the
module, already at "compile time" of an expression (which is run-time of your software), the
result type of an expression can be checked.

We just look at some samples:

We have to add header file \alibheader{files/fileexpressions.hpp}:

\snippet "DOX_FILES.cpp"     DOX_FILES_HEADER2

Now this code compiles:

\snippet "DOX_FILES.cpp"     DOX_FILES_SCAN_FEX1

The resulting output is:

\verbinclude "DOX_FILES_SCAN_FEX1.txt"

Here are some more samples:
\snippet "DOX_FILES.cpp"     DOX_FILES_SCAN_FEX2
\verbinclude "DOX_FILES_SCAN_FEX2.txt"

\snippet "DOX_FILES.cpp"     DOX_FILES_SCAN_FEX3
\verbinclude "DOX_FILES_SCAN_FEX3.txt"

\snippet "DOX_FILES.cpp"     DOX_FILES_SCAN_FEX4
\verbinclude "DOX_FILES_SCAN_FEX4.txt"

\snippet "DOX_FILES.cpp"     DOX_FILES_SCAN_FEX5
\verbinclude "DOX_FILES_SCAN_FEX5.txt"


\note Of course the expression compiler implemented with class \alib{files;FileExpressions} may
      be used not only with filtering files. The compiler has public access and it can be used
      to compile and evaluate any other sort of expressions, including such that do not return a
      boolean value, can be compiled and  evaluated.

\I{################################################################################################}
# 3. Monitoring Changes in an FTree # {#alib_files_monitoring}
Class \alib{files;FTree} provides several interface methods that allow instances of abstract
type \alib{files;FTreeListener} to be registered for monitoring changes.

Those are:
- \alib{files;FTree::MonitorDistinctFile},
- \alib{files;FTree::MonitorFilesByName},
- \alib{files;FTree::MonitorPath},
- \alib{files;FTree::MonitorPathPrefix},
- \alib{files;FTree::MonitorPathSubstring}, and
- \alib{files;FTree::MonitorStop}.
                                        
For various reasons it is not - with reasonable effort and efficiency - possible to  
trigger the notification events from inside class \b FTree automatically. 
Besides this class being just a rather thin layer on top of class \alib{containers;StringTree},
the class itself can never be sure when, for example, a new file entry is really finally created
with all available information set.

For this reason, the notification events have to be triggered by the code entities that manipulate 
the tree. Notification is performed by callng method
       
- \alib{files;FTree::Notify}

with the according event type set.

Built-in scan functions duly perform such notifications. One warning has to be mentioned: 
If filter \alib{files;ScanParameters::DirectoryFilterPostRecursion} is set, then notification
about the creation of files, which later are removed by this filter will occur. On removal, only the
node that is removed will be notified, but not the child nodes, which previously had been
notified to having been created.<br>
This is a design decision in favor to efficiency.
                                      
\I{################################################################################################}
# 4. Attaching Custom Data to Files # {#alib_files_customdata}
Class \alib{files;FTree} allows attaching a custom object to each node. 
The memory of this custom object is allocated (and thus recycled with deletion of nodes) using the
internal pool allocator.
 
It is up to the using software to keep track about which data type is assigned to which 
node of the tree. In the most common cases, where either all nodes receive the same data, 
or leaf-nodes (files) receive a different type than directories, this is no burden.   
With debug-compilations, type information is stored with every node and it is asserted that 
the same type is received or deleted that was previously set.
                                                             
The interface provided to manage custom data is comprised by the following methods:

Once this is done, methods of class \b %File:
- \alib{files;File::AttachCustomData},                                       
- \alib{files;File::HasCustomData},                                       
- \alib{files;File::GetCustomData}, and                                       
- \alib{files;File::DeleteCustomData}.
                                             
A convenience method is furthermore provided with:
- \alib{files;FTree::DeleteAllCustomData}. 

However, this method is only applicable if all nodes have custom data of the same type set. 


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 5. Outlook # {#alib_files_outlook}
This should be enough for the time being. Module \alib_files_nl is quite new and was introduced only
with \aliblong release Version 2402 and was extended and overhauled with Version 2412. 
The future will show how this module expands.

Again, consult the extensive \ref alib::files "Reference Documentation" for all details about the
currently existing functionality.



<br><br><br><br><br><br> */
