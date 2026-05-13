// #################################################################################################
//  Documentation - ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_filetree   ALib Module FileTree - Programmer's Manual


\tableofcontents

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 1\. Introduction # {#alib_filetree_intro}

This \alibmod provides mechanics to scan directories and contained files.
The scan results are collected in an instance of #"FTree", which inherits
class #"StringTree".
Using classes #"StringTree::Cursor;*" and #"StringTreeIterator",
two very comfortable interfaces to accessing the results are available.

As of today, besides scanning files, no specific further functionality is given, and it is up
to the user of the module to do with the result lists whatever is intended.

While the #"alib::filetree;reference documentation" of the types found in this module is
quite verbose and thus should be all that is really needed, this Programmer's Manual
just provides some few step-by-step demo samples.


\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 2\. Tutorial # {#alib_filetree_tut}
\I{################################################################################################}
## 2.1 Scanning a File Tree  ## {#alib_filetree_tut_scan}
A simple application needs to include just header #"F;ALib.FileTree.H".
\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_HEADER

For scanning a path including its subdirectories, a few objects are needed:
\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_OBJECTS

Here are the links to the reference documentation of these objects:
- #"FTree",
- #"ScanParameters",
- #"CanonicalPathList" and
- #"TMonoAllocator" located in module \alib_monomem.

Note that the sample code above uses configuration macro <b>ALIB_BASE_DIR</b>, which is defined
with the \alib unit test project, that this documentation uses to generate the samples.

That is all we needed to start the scan, which is done using the namespace function 
#"ScanFiles(FTree&)".
Next we use namespace function #"filetree::DbgDump", which as its prefix <em>Dbg</em> indicates,
is only available in debug-compilations of the library:

\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_SCAN

The resulting output is:

\snippet "DOX_FILETREE_MAIN.txt"     OUTPUT

You might wonder about the <c>resultPath</c> vector given as an output parameter into function
#"ScanFiles(FTree&)".
Especially the question is: Why is it a vector? Wouldn't it be just the requested start path?

Let's quickly examine the result:
\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_RESULTS

This writes:

\snippet "DOX_FILETREE_RESULTS.txt"     OUTPUT

So, in this test, we get indeed only one result. Nevertheless, we have can ask the #"FTFile" instance
in the vector for two different representations: "symbolic" and "real" path.
The "symbolic path" is exactly the path we requested to be scanned. 
The real path, however, is different. How this relates and how multiple result paths may be
returned is explained in the next section. 

\I{################################################################################################}
## 2.3 Real-Paths ## {#alib_filetree_tut_scan_realpath}
### 2.3.1 The Symbolic Link Challenge ## {#alib_filetree_tut_scan_realpath_symlinks}
The invention of symbolic links in the Unix world allowed very flexible ways to organize
directories and files. The drawback is, that software might be confused by such links, and
many applications and tools exist do not handle symbolic links correctly. If done wrongly, 
circular references can easily occur and let a software, for example one that scans a file tree,
run into an endless loop.

This library avoids this problem in the only possible efficient way: It always uses the
real path of a file or directory. If a "real" directory is it twice, the recursion stops. 

For this, the function \b ScanFiles defines a <em>"data contract"</em>. 
Instead of repeating its definition here, we ask the reader to read 
#"ScanFiles(FTree&);about this here" right now.

With this information in mind, we can now answer the question of why the result when scanning
just one single path is a vector of paths! Here is a sample:

    Suppose we have the following directory structure:
        /a/a 
        /b/b 
        /b/link -> /a/a

    Scanning the directory /b leads to the following result paths:
        /a/a 
        /b/b 
             
So, one simple link into a sibling path of the start-path leads to two results already.

### 2.3.2 Reestablishing Symbolic Paths ## {#alib_filetree_tut_scan_realpath_sympaths}
The approach taken by the library to handle symbolic links leads to 100% correct scan results,
detects circular references, and is extremely efficient in respect to common approaches of other 
software. Also, software that uses the result trees will not be confused by symbolic links and 
will never need to care about them.

However, at the moment software displays the scanned file's paths back to the user or to other 
software, presenting real-paths can be a problem.
For example, if a tool processes source code files and lists some results to be "clickable" an
an IDE's output tool window, most IDEs (including CLion) do not detect that the presented file
is the as registered with the IDE using a symbolic path. With that, the IDE opesn the same 
source-file twice, once with the symbolic path (the one it knew before) and once with the real path! 
The IDE would maybe even ask: <em>"Do you want to edit files outside your project?"</em>

To avoid this, class #"%filetree::FTFile" offers a special parent node, accessible with  
#"FTFile::GetSymbolicParent;2". As its name suggests, this method returns the symbolic link that  
the scanner followed to hit that file or directory. Custom scanners (or other code that manipulates
a #"%FTree") can use the method #"FTFile::SetSymbolicParent(FTFile);2" to provide similar information.
In the case that no symbolic link was followed, the method returns the same object that the code

        FTFile ( myFile.AsCursor().Parent() )
                 
would receive.  

Consequently, class #"%FTFile" offers two methods:
- #"FTFile::AssembleSymbolicPath;2", and
- #"FTFile::AssembleRealPath;2".

\note It is also possible to directly use the #"%StringTree"'s functionality provided with the 
      method #"^Cursor::AssemblePath(const TCursor);2". This can be accessed with:
          
            myFile.AsCursor().AssemblePath(...);
\note            
      Here, of course, it is always the real path that is returned.
      If you can answer to yourself why this is, you have understood \alib's architecture in  
      respect to the classes #"%StringTree" / #"%^StringTree:Cursor" and #"%FTree" / 
      #"FTFile" very well!
                      
As a final remark: The function #"filetree::ScanFiles" sets the symbolic parent only when the
file or directory was not scanned already - either through the real path or a different symbolic 
link. This means that the symbolic parent is set only once and its setting depends on the order
of the scanned files, and, in case of multiple scans, the order of their start paths.  

\attention
  Symbolic parent information stores backlinks to other nodes in the tree. Therefore, if symbolic
  paths are needed, nodes that participate in those links must stay alive.
  In other words: avoid deleting nodes after scanning, and avoid scan filters/options that delete
  nodes while scanning (for example, post-recursion filtering or empty-directory removal), because
  this can invalidate symbolic path reconstruction.<br>
  The library does not check for this, and it is the responsibility of the user to ensure that
  symbolic parent information is not invalidated by deleting nodes or using filters that delete
  nodes during scanning. The reason that this is not implemented is that symbolic parent information
  is not needed in many cases, and in the case it is needed, usually no node deletion is needed.
  
\attention
  But stay warned: mixing both, symbolic path retrieval and arbitrary node deletion, can lead to
  hard crashes (undefined behavior) of your software!

\I{################################################################################################}
## 2.3 Using Filters With Scanning  ## {#alib_filetree_tut_scan_filter}
We left all fields of class #"ScanParameters" with their default values in the previous
sample. Now we want to look at fields:
- #"ScanParameters::DirectoryFilterPreRecursion"
- #"ScanParameters::DirectoryFilterPostRecursion"
- #"ScanParameters::FileFilter"

Again, please refer to the reference documentation of the fields linked above, to get a quick
understanding, why the scan function offers to set up to three different filter objects.

Class #"FFilter" is a very simple virtual abstract interface class, which only has
one single method #"FFilter::Includes" to implement. Derived filter types, need
return \c true, if a file or directory "passes" the filter, hence in this case is included in
the scan results.

It should be very straight forward to implement an own derived a filter class. The problem
with such class would be, that it would be more or less "hard-coded" in respect to what is filtered
and what not. This might be flexible enough for most applications.<br>
The next chapter introduces a filter which is runtime compiled!

\I{################################################################################################}
## 2.4 Class FileExpressions  ## {#alib_filetree_tut_fex}
In case module \alib_expressions is included in the \alibbuild, this module \alib_filetree_nl exposes
the #"FileExpressions" class, which implements a #"CompilerPlugin"
that allows articulate runtime expressions working with #"FTValue" objects.

The class exposes the public inner type #"FileExpressions::Filter;*" which implements the
\b %FFilter interface. With construction, the filter accepts a character string containing the
filter expression.

The full set of expression operators, functions and constants is documented with class
#"FileExpressions" and not to be repeated here. While expressions might return any
kind of type, those used with class \b %FileExpression::Filter, have to evaluate to a boolean
value. As documented with module \alib_expressions, due to the type-safe implementation of the
module, already at "compile time" of an expression (which is runtime of your software), the
result type of an expression can be checked.

We just look at some samples:

We have to add the header-file #"F;ALib.FileTree.Expressions.H":

\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_HEADER2

Now this code compiles:

\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_SCAN_FEX1

The resulting output is:

\verbinclude "DOX_FILETREE_SCAN_FEX1.txt"

Here are some more samples:
\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_SCAN_FEX2
\verbinclude "DOX_FILETREE_SCAN_FEX2.txt"

\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_SCAN_FEX3
\verbinclude "DOX_FILETREE_SCAN_FEX3.txt"

\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_SCAN_FEX4
\verbinclude "DOX_FILETREE_SCAN_FEX4.txt"

\snippet "DOX_FILETREE.cpp"     DOX_FILETREE_SCAN_FEX5
\verbinclude "DOX_FILETREE_SCAN_FEX5.txt"


\note Of course the expression compiler implemented with class #"FileExpressions" may
      be used not only with filtering files. The compiler has public access and it can be used
      to compile and evaluate any other sort of expressions, including such that do not return a
      boolean value, can be compiled and evaluated.

\I{################################################################################################}
# 3\. Monitoring Changes in an FTree # {#alib_filetree_monitoring}
Class #"FTree" provides several interface methods that allow instances of abstract
type #"FTreeListener" to be registered for monitoring changes.

Those are:
- #"FTree::MonitorDistinctFile;*",
- #"FTree::MonitorFilesByName;*",
- #"FTree::MonitorPath;*",
- #"FTree::MonitorPathPrefix;*",
- #"FTree::MonitorPathSubstring;*", and
- #"FTree::MonitorStop;*".
                                        
For various reasons it is not - with reasonable effort and efficiency - possible to
trigger the notification events from inside class \b FTree automatically.
Besides this class being just a rather thin layer on top of class #"StringTree",
the class itself can never be sure when, for example, a new file entry is really finally created
with all available information set.

For this reason, the notification events have to be triggered by the code entities that manipulate
the tree. Notification is performed by calling the method
       
- #"FTree::Notify;*"

with the according event type set.

Built-in scan functions duly perform such notifications. One warning has to be mentioned:
If filter #"ScanParameters::DirectoryFilterPostRecursion;*" is set, then notification
about the creation of files, which later are removed by this filter will occur. On removal, only the
node that is removed will be notified, but not the child nodes, which previously had been
notified to having been created.<br>
This is a design decision in favor of gaining efficiency.
                                      
\I{################################################################################################}
# 4\. Attaching Custom Data to Files # {#alib_filetree_customdata}
Class #"FTree" allows attaching a custom object to each node.
The memory of this custom object is allocated (and thus recycled with deletion of nodes) using the
internal pool allocator.
 
It is up to the using software to keep track about which data type is assigned to which
node of the tree. In the most common cases, where either all nodes receive the same data,
or leaf-nodes (files) receive a different type than directories, this is no burden.
With debug-compilations, type information is stored with every node and it is asserted that
the same type is received or deleted that was previously set.
                                                             
The interface provided to manage custom data is comprised by the following methods:

Once this is done, methods of class #"%FTFile":
- #"FTFile::AttachCustomData;*",
- #"FTFile::HasCustomData;*",
- #"FTFile::GetCustomData;*", and
- #"FTFile::DeleteCustomData;*".
                                             
A convenience method is furthermore provided with:
- #"FTree::DeleteAllCustomData;*".

However, this method is only applicable if all nodes have custom data of the same type set.

\I{################################################################################################}
# 5\. Iterating File-Trees # {#alib_filetree_iteration}
For iterating scanned #"%FTree"s, class  #"StringTreeIterator", provided with the module 
\alib_containers, is very convenient.
                                                           
Here is quick sample code taken from the debug-function #"DbgDump(FTree&)":
\snippet "ftree.cpp"     DOX_FILETREE_ITERATION

As you can see from this sample, to access the file information during iteration, the 
#"^StringTree::Cursor;2" received with the method #"StringTreeIterator::Node" has to be wrapped
in an instance of class #"FTFile". (For the optimizing compiler, this is a no-op.)

Further options are available, for example, to sort the files and directories with custom sorters
which could be implemented using runtime expressions.

For all information consult the
#"StringTreeIterator;reference documentation of class StringTreeIterator".

\I{################################################################################################}
\I{################################################################################################}
\I{################################################################################################}
# 6\. Outlook # {#alib_filetree_outlook}
This should be enough for the time being. Module \alib_filetree_nl is quite new and was introduced only
with \aliblong release Version 2402 and was extended and overhauled with Version 2412.
The future will show how this module expands.

Again, consult the extensive #"alib::filetree;Reference Documentation" for all details about the
currently existing functionality.



<br><br><br><br><br><br> */
