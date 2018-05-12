// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION) || defined(HPP_ALIB_LANG_RESOURCES)
    #error "include alib/alib.hpp instead of this header"
#endif

// then, set include guard
#ifndef HPP_ALIB_LANG_RESOURCES
#define HPP_ALIB_LANG_RESOURCES 1


#if !defined (_GLIBCXX_MAP) && !defined(_MAP_)
    #include <map>
#endif

#include "alib/util/memoryblocks.hpp"
#include "alib/strings/util/stringmap.hpp"

namespace aworx  { namespace lib {

#if ALIB_MODULE_CONFIGURATION || ALIB_MODULE_CORE
        namespace threads { class ThreadLock;          }
#endif

#if ALIB_MODULE_CONFIGURATION
        namespace config  { class Configuration;       }
        namespace config  { class ConfigurationPlugin; }
        namespace config  { class Variable;            }
#endif
namespace lang {


/** ************************************************************************************************
 * This class provides string based resources in a two-level, nested hash map. The outer map
 * uses a category string key, while the inner uses another string key denoting the resource
 * string name and hashing the resource string.<br>
 *
 * In addition to this simple hashing, with \alib module \ref ALIB_MODULE_CONFIGURATION in place,
 * when loading resources, those are tried to be read from field #Config with priority.
 * With the prioritization mechanics of class \alib{config,Configuration} itself, this
 * allows for example to have language translations of resources in place, which - if only sparsely
 * defined - automatically fallback through the different configuration plug-ins and finally get
 * loaded from this classes internal simple nested hash map.
 *
 * This class is usually used with class \alib{lang,Library}, which also creates and distributes
 * instances of this class to dependent libraries. In other words, normally instances of this class
 * are never to be created by custom code.
 *
 * The documentation of class \alib{lang,Library} explains how a using software gets control over
 * - the assignment of either the same or different instances of this class to different C++
 *   libraries
 * - the initialization phases which allow to add custom resource "sources" on bootstrap.
 *
 * Due to the use of this class throughout \alib itself, almost all built-in names of
 * \alib{config,Variable,configuration variables},
 * \alib{lang,T_EnumMetaDataDecl,enum meta data},
 * \alib{lang,Exception,exception messages},
 * and so forth can be changed and customized.
 *
 *
 * The simplest way to change names is by changing the values programatically (hard-coded) in
 * the bootstrap section of a software.<br>
 * A more advanced approach ist to use class \alib{config,IniFile}, load a resource file and
 * add this to field \alib{lang,Resources::Config} as plug-in. Due to the "fallthrough" mechanism
 * two things are possible:
 * - An INI-file may be only "sparsely" filled. Only the values that are to be overwritten need
 *   to be set. For others, the default values apply.
 * - More than one INI-file may be attached (with different priorities). This allows even to
 *   sort the supported languages by their preference.
 *
 * A fresh and complete INI-file that contains all default values can be created using method
 * #FetchDefaults. Typically, a software would provide a special command line parameter to
 * let a user export the resources once to an INI-file. The user then might choose to edit
 * the file and copy it to a certain folder (and name scheme) that the application uses to search
 * for (or a different command line parameter allows to use the file).
 * If found, these files would be attached to the instance of this class with the next run of the
 * software.
 *
 * The most advanced way to manage resources, is to write a custom configuration plug-in, to load
 * resources from a place that the main software uses by default.
 * Of-course, alternatively, such external sources might be used directly to fill the defaults -
 * and this way leave field #Config untouched. There is a great freedom of choice.
 **************************************************************************************************/
class Resources
{
    // #############################################################################################
    // internal fields
    // #############################################################################################
    protected:
    #if ALIB_MODULE_CONFIGURATION || ALIB_MODULE_CORE
        /**
         * Lock used to perform thread-safe access to resources.
         * \note
         *   Instead of deriving this class from \b %ThreadLock, we use a pointer field here.
         *   This is to enable forward declaration of the type and include
         *   \alib resources features at an early stage of compilation.
         */
        threads::ThreadLock*                            lock;
    #endif

        /**
         * Two nested \c std::unordered_map objects to store static hard-coded default resources.
         * The outer map object assigns the inner map to the resource category.
         * The inner map object assigns resource names to the data.
         */
        UnorderedStringMap<UnorderedStringMap<String>>  defaults;

        /**
         * Two nested \c std::unordered_map objects to store copies of resources found in #Config.
         * On the initial read of a variable, the value is copied using #cache and
         * inserted here.
         *
         * The outer map object assigns the inner map to the resource category.
         * The inner map object assigns resource names to the data.
         *
         * This map is searched first (before #Config and #defaults).
         */
        UnorderedStringMap<UnorderedStringMap<String>>  cachedVariables;

        /**
         * Block-allocator to store copies of resources (and category/name strings).
         *
         * Used with #AddCopy and also all resources that are loaded from #Config get
         * cached.
         *
         */
        MemoryBlocks                                    cache;



    #if ALIB_MODULE_CONFIGURATION
        /**  An only locally used object. Declared as field member to be reusable (performance
         *   optimization preferred against memory consumption). */
        config::Variable*           variable                                              = nullptr;
    #endif

    // #############################################################################################
    // public fields
    // #############################################################################################
    public:
    #if ALIB_MODULE_CONFIGURATION
        /**  The configuration object for custom resource data.<br>
         *   This field is only available with \ref ALIB_MODULE_CONFIGURATION set.  */
        config::Configuration*      Config                                                = nullptr;
    #endif



    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    public:
    /// Constructor. Creates a configuration object pointed to by #Config without plug-ins.
    Resources();

    /// Destructor. Deletes the configuration object created in the constructor.
    ~Resources();


    // #############################################################################################
    // Resource Interface
    // #############################################################################################

#if defined(DOX_PARSER)
    /** ********************************************************************************************
     * Stores static, default resource data (in field #defaults).
     * This method is usually invoked by classes derived from
     * \alib{lang,Library} in initialization code of phase \alib{lang,Library::Phases::resourceset}.
     *
     * \attention
     *   All three string parameters must be kept available through the life-time of this class,
     *   as no copy is created. This condition is naturally met by C++ string literals which
     *   are usually passed. If a software uses custom resource mechanics to already
     *   populate the default values with otherwise resourced strings, and those are not surviving
     *   in memory, method #AddCopy is to be used instead.
     *
     * @param category   Category string of the resource.
     * @param name       Name string of the resource
     * @param data       String data
     * @param dbgAssert  This parameter is available only in debug mode. If \c true, an assertion
     *                   is raised if the resource was added before.
     ********************************************************************************************~*/
    ALIB_API
    void Add( const String& category, const String& name, const String& data, bool dbgAssert );
#else
    ALIB_API
    void Add( const String& category, const String& name, const String& data ALIB_DBG(, bool dbgAssert ) );
#endif

    /** ********************************************************************************************
     * Copies and stores default resource data (in field #defaults).
     * Creates copies of the data passed. Parameter \c category is only copied if the category
     * does not exist, yet. Parameter name usually is copied. It is not, if the resource variable
     * existed before. Unlike #Add, in debug compilations no \alib assertion is raised, if the
     * entry is overwritten.
     *
     * The use cases for this method, in contrast to using methods #Add and #AddBulk are:
     * - Add resources that are not string literals but local objects or other string data
     *   with a limited lifecycle.
     * - Replace resources that contain formatter string, with formatted data. For example
     *   application name strings, which at bootstrap get formatted with the actual version
     *   number of the software.
     *
     * \attention
     *   The string copies will not be freed unless this object is deleted. This means, resources
     *   must not be replaced on a regular basis during runtime.
     *
     * @param category          Category string of the resource.
     * @param name              Name string of the resource
     * @param data              String data
     ********************************************************************************************~*/
    ALIB_API
    void AddCopy( const String& category, const String& name, const String& data );

    /** ********************************************************************************************
     * Stores static, default resource data (in field #defaults).
     * This method is usually invoked by classes derived from
     * \alib{lang,Library} in initialization code of phase \alib{lang,Library::Phases::resourceset}.
     *
     * The variadic arguments must be of type <b>const char*</b> and consists of pairs
     * of resource name and values.<br>
     * <b>The list has to be finished with a final \c nullptr argument!</b>
     *
     * \note
     *   The use of variadic C-style arguments in general is absolutely not recommended.
     *   We still do it here, because this method is usually used with implementations
     *   of \alib{lang,Library::init} to load static default values.
     *   This approach saves a lot of otherwise needed single invocations (reduces code size) and
     *   makes the the init methods look very clean.
     *
     * \attention
     *   All string parameters must be kept available through the life-time of this class,
     *   as no copy is created as documented with method #Add.
     *   To create copies, use #AddCopy.
     *
     *
     * @param category    Category string of the resource. For technical reasons, this has to be
     *                    of type <c>const char*</c>.
     * @param ...         A list of pairs of <b>const char*</b> String data
     ********************************************************************************************~*/
    ALIB_API
    void AddBulk( const character* category, ... );

#if defined(DOX_PARSER)
    /** ********************************************************************************************
     * Returns a resource. First, field #Config is searched (which features a prioritized
     * search in different plug-ins) and if not found, the #defaults hash map is searched.
     * On failure, a \e nulled string is returned.
     *
     * \note
     *   Usually resources are associated with \alib{lang::Library} objects and should be loaded
     *   using its methods \alib{lang,Library::GetTry,GetTry} and \alib{lang,Library::Get,Get}.
     *   If used directly, a preprocessor switch has to be set about providing parameter
     *   \p{dbgAssert}.
     *
     * @param category   Category string of the resource.
     * @param name       Name string of the resource
     * @param dbgAssert  This parameter is available only in debug mode. If \c true, an assertion
     *                   is raised if the resource was not found.
     * @return The resource string, respectively a \e nulled string on failure.
     ********************************************************************************************~*/
    ALIB_API
    String Get( const String& category, const String& name, bool dbgAssert   );
#else
    ALIB_API
    String Get( const String& category, const String& name  ALIB_DBG(, bool dbgAssert)    );
#endif


    // #############################################################################################
    // Other interface methods
    // #############################################################################################
    #if ALIB_MODULE_CONFIGURATION
        /** ****************************************************************************************
         * This method fetches all values from the internal hash map which are not present in the
         * given configuration plug-in \p{dest} and stores them there.
         * This is useful to collect all static resource values of a library, generated in
         * initialization phase \alib{lang,Library::Phases::resourceset}
         * and store them in a users' configuration file.
         * Such file can then be added to #Config in the next run of the software and values
         * get read from there instead of from the internal hash map.
         *
         * @param dest   The destination plug-in.
         * @return The number of variables fetched.
         ******************************************************************************************/
        ALIB_API
        int     FetchDefaults( config::ConfigurationPlugin& dest );
    #endif


}; // class Resources



}}} // namespace [aworx::lib::lang]



#endif // HPP_ALIB_LANG_RESOURCES
