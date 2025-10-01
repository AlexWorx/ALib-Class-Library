//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace expressions {

//==================================================================================================
/// This is a default implementation of abstract interface \alib{expressions;ExpressionRepository}
/// which retrieves definitions of named expressions using core mechanics of \alib, namely:
/// 1. \ref alib_mod_resources "Static resource data", and
/// 2. \ref alib_mod_variables    "Configuration data".
//==================================================================================================
class StandardRepository : public  ExpressionRepository
{
    protected:
        /// The configuration object used to retrieve definitions of nested expressions.
        SharedConfiguration     config;

        /// The resource pool used to retrieve definitions of nested expressions, if not found
        /// with configuration data of field #config.
        ResourcePool*           resources;

        /// The category within #resources to use for searching expression strings.
        NString                 resourceCategory;

    public:
        /// May be filled with paths in field #config. A named expression's identifier will
        /// be first tried to be loaded using each path name added here.
        ///
        /// If this is left empty, with the first invocation of method #Get, an empty string is
        /// added to enable to search an expression in the root of the \b StringTree of #config.
        std::vector<AString>    ConfigPaths;

        /// Within this vector, all variables that have been loaded from #config are stored.
        ///
        /// The tuple elements provide:
        /// - the priority (configuration slot),
        /// - the variable name and
        /// - the name of the nested expression.
        ///
        /// The latter is needed, as it might differ from the combination of the "category" and
        /// variable name due to definitions in #ConfigPaths.
        ///
        /// This vector might be used for various things, e.g., logging out status information.
        /// It is also used by method #StoreLoadedExpressions.
        ///
        /// \note
        ///   This list is never cleared by this class. It might be cleared from outside
        ///   (e.g., to release memory) without causing any side effects, other than those occurring
        ///   to mechanics an application itself volunteers to implement using this information.
        std::vector<std::tuple<Priority,AString,String>>                            VariablesLoaded;

        //==========================================================================================
        /// Constructor.
        ///
        /// @param pConfig      The configuration data to use.
        ///                     May be set to \c nullptr to omit the use of configuration data.
        /// @param pResources   The resources to use.
        ///                     May be set to \c nullptr to omit the use of resource data.
        /// @param pResCategory The category within #resources to use for searching expression
        ///                     strings.
        //==========================================================================================
        inline
        StandardRepository( const SharedConfiguration& pConfig,
                            ResourcePool*              pResources,
                            NString                    pResCategory )
        : config          (pConfig)
        , resources       (pResources)
        , resourceCategory(pResCategory)
        {}

        //==========================================================================================
        /// Implementation of the central interface method. If invoked, the definition string is
        /// first searched in #config and if not found, secondly searched in field #resources.
        ///
        /// @param      identifier  The name of the required expression.
        /// @param[out] target      The target to write the requested expression string to.
        /// @return This \c true, if the expression string could be retrieved, \c false otherwise.
        ///         If \c true is returned and \p{target} is still empty, then the string is defined
        ///         to be empty, which throws an exception on compilation.
        //==========================================================================================
        virtual ALIB_DLL
        bool                Get( const String& identifier, AString& target )               override;

        //==========================================================================================
        /// Stores back all expression strings which had been automatically loaded from
        /// the plug-in of field #config, specified by parameter \p{slot}.
        /// For storing, the normalized expression string is used. Thus, this method may be used to
        /// write a "clean" expression string, that does not use abbreviations for identifiers, has
        /// no unnecessary whitespaces, etc.
        ///
        /// Note the importance of parameter \p{slot}: Usually, only such expression variables
        /// should be stored that had been loaded from a configuration source that is manually edited
        /// by end-users, like INI-files.
        ///
        /// @param compiler  The compiler that this interface is attached to.
        /// @param slot      The slot of the configuration plug-in of which loaded variables are to
        ///                  be written back.
        /// @return The number of variables written.
        //==========================================================================================
        ALIB_DLL
        int             StoreLoadedExpressions( Compiler*  compiler,
                                                Priority slot = Priority::Standard  );
};

}} // namespace [alib::expressions]

