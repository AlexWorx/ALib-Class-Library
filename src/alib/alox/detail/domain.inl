/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALOX_DETAIL_DOMAIN
#define HPP_ALOX_DETAIL_DOMAIN 1

#if !defined(HPP_ALIB_LOX_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined (HPP_ALOX_DETAIL_LOGGER)
#   include "alib/alox/detail/logger.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_LIST)
#   include "alib/monomem/list.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_STDCONTAINERMA)
#   include "alib/monomem/stdcontainerma.hpp"
#endif

namespace alib::lox::detail {

// forward declarations
class ScopeInfo;

/** ************************************************************************************************
 * Used to store prefixes set. Those provided as
 * \ref alib::boxing::Box "boxes" of character arrays are copied into an internal AString
 * and will be deleted with the object. This ensures, that simple strings might get assembled
 * on the stack and still be used as a prefix logable.
 **************************************************************************************************/
class PrefixLogable : public Box
{
    protected:
        /** If set, it will be deleted. */
        AString* copy   = nullptr;

    public:
    /**
     * Constructor taking the originally provided box. If this is an array of characters, the
     * contents is copied into a heap allocated (new) AString and our vtable is replaced accordingly.
     * @param src The prefix object provided by the user
     */
    PrefixLogable( const Box& src )
    : Box( src )
    {
        // uses "placement new" to overwrite the box part of ourselves
        if( src.IsNotNull() )
        {
                 if ( IsArrayOf<nchar>() )  new (this) Box( copy= new AString( Unbox<NString>() ));
            else if ( IsArrayOf<wchar>() )  new (this) Box( copy= new AString( Unbox<WString>() ));
            else if ( IsArrayOf<xchar>() )  new (this) Box( copy= new AString( Unbox<XString>() ));
        }
    }

    /** Destructor. Deletes field #copy if set.  */
    ~PrefixLogable()
    {
        if(copy)
            delete copy;
    }

    /**
     * Move assignment operator.
     * Default implementation of compiler is used.
     * (Needs to be explicitly given due to custom destructor.)
     * @param move The object to be moved.
     * @return This object
     */
    PrefixLogable& operator= (PrefixLogable&& move ) = default;
};

/** ************************************************************************************************
 * Objects of this class represent a <em>Log Domain</em> of \alox. This class is internally used by
 * class \b Lox.
 **************************************************************************************************/
class Domain
{
    /** ********************************************************************************************
     * Internal class that holds data stored per Logger
     **********************************************************************************************/
    public:
    struct LoggerData
    {
        /** The logger. */
        detail::Logger*     Logger;

        /** The verbosity of the \e Logger for this domain. */
        Verbosity           LoggerVerbosity       = Verbosity::Off;

        /** The priority value that was used to set the priority. */
        Priorities          Priority              = Priorities::NONE;


        /** the number of log calls on this domain for this logger. */
        int                 LogCallsPerDomain     =0;

        /**
         * Constructor
         * @param logger The logger to add
         */
        LoggerData( detail::Logger* logger )
        {
            this->Logger= logger;
        }
    };

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:

        /** The name of the domain. For root domains, this is \e nulled. */
        NString                           Name;

        /** The full path of the domain (set in the constructor once) . */
        NString                           FullPath;

        /** The parent domain. For root domains, this is \c nullptr. */
        Domain*                           Parent;

        /** A list of sub domains, sorted by name. */
        List<Domain>                      SubDomains;

        /** Data stored per logger. The index is corresponding to the list of loggers in 'our'
         *  Lox. */
        std::vector<LoggerData,StdContMA<LoggerData>>    Data;

        /** <em>Prefix Logables</em> associated with this domain. */
        List<std::pair<PrefixLogable*, lang::Inclusion>> PrefixLogables;

       /**
         * A counter for the quantity of calls on this domain.
         * Counting does not include:
         * - logs when no \e Logger was set
         * - conditional logs that were suppressed
         * Otherwise, it includes all log calls, even when no \e Logger was enabled on this domain.
         */
        integer                             CntLogCalls                                          =0;

        /** Flag which is set when verbosity configuration data was read. */
        bool                                ConfigurationAlreadyRead                         =false;

    // #############################################################################################
    // Public interface
    // #############################################################################################
    public:
         /// @return Returns the domain path separation character.
         static constexpr
         nchar  Separator()
         {
            return '/';
         }

        /** ****************************************************************************************
         * Constructor used for the root domain.
         * @param allocator The monotonic allocator used allocation of permanent objects.
         * @param name      The name of this root domains
         ******************************************************************************************/
        ALIB_API
        Domain( MonoAllocator* allocator,  const NString& name  );

        /** ****************************************************************************************
         * Constructor
         * @param parent    The parent domain. For root domains, this is \c nullptr.
         * @param name      The name of the domain.
         ******************************************************************************************/
        ALIB_API
        Domain( Domain* parent,  const NString& name );

        /** ****************************************************************************************
         * Destroys the <em>Log Domain</em> object.
         ******************************************************************************************/
        ALIB_API
        ~Domain();

        /** ****************************************************************************************
         * Returns the root domain of this object.
         * @return The root domain of this object
         ******************************************************************************************/
        Domain* GetRoot()
        {
            Domain* rootDomain= this;
            while ( rootDomain->Parent != nullptr )
                rootDomain= rootDomain->Parent;
            return rootDomain;
        }

        /** ****************************************************************************************
         * Adds a new entry in field #Data and recursively demands the same from its sub-domains.
         * Checks if a logger with the same name exists.
         *
         * @param logger The logger to add.
         * @return The number of the \e Logger, -1 if a logger with the same name exists already.
         ******************************************************************************************/
        int    AddLogger( detail::Logger* logger)
        {
            // let our root do this
            if ( Parent != nullptr )
                return Parent->AddLogger( logger );

            // check for doubles
            if ( GetLoggerNo( logger->GetName() ) >= 0 )
                return -1;

            // now this and all children
            addLoggerRecursive( logger );
            return static_cast<int>(Data.size()) - 1;
        }

        /** ****************************************************************************************
         * Removes an new entry in field #Data and recursively demands the same from
         * its sub-domains.
         * @param loggerNo  The number of the \e Logger to be removed.
         ******************************************************************************************/
        void    RemoveLogger( int loggerNo )
        {
            // let our root do this
            if ( Parent != nullptr )
            {
                Parent->RemoveLogger( loggerNo );
                return;
            }

            // now this and all children
            removeLoggerRecursive( loggerNo );
        }

        /** ****************************************************************************************
         * Returns the number of loggers stored in this domain (the same for all domains within
         * a tree).
         * @return The number of loggers attached.
         ******************************************************************************************/
        int  CountLoggers()
        {
            return static_cast<int>( Data.size() );
        }

        /** ****************************************************************************************
         * Searches and returns the \e Logger given by name.
         * @param loggerName  The logger to search.
         * @return The \e Logger found corresponding to given name.
         *         If the \e Logger does not exist, nullptr is returned.
         ******************************************************************************************/
        detail::Logger*  GetLogger( const NString& loggerName )
        {
            for ( size_t i= 0; i < Data.size() ; ++i  )
                if ( loggerName.Equals<true, lang::Case::Ignore>( Data[i].Logger->GetName()) )
                    return Data[i].Logger;
            return nullptr;
        }

        /** ****************************************************************************************
         * Returns logger of given number.
         * @param no  The number of the \e Logger to return.
         * @return The \e Logger found with number \p{no}.
         ******************************************************************************************/
        detail::Logger*  GetLogger( int no )
        {
            ALIB_ASSERT_ERROR( no < static_cast<int>(Data.size()), "ALOX", "Internal error: Illegal Logger Number" )
            return Data[static_cast<size_t>(no)].Logger;
        }

        /** ****************************************************************************************
         * Returns the number of the \e Logger specified by name.
         * @param loggerName  The logger name to search.
         * @return The number of the \e Logger found corresponding to given name.
         *         If the \e Logger does not exist, -1 is returned.
         ******************************************************************************************/
        int  GetLoggerNo( const NString& loggerName )
        {
            for ( size_t i= 0; i < Data.size() ; ++i  )
                if ( loggerName.Equals<true, lang::Case::Ignore>( Data[i].Logger->GetName() ) )
                    return static_cast<int>( i );
            return -1;
        }

        /** ****************************************************************************************
         * Returns the number of the \e Logger.
         * @param logger  The logger to search.
         * @return The number of the \e Logger. If the \e Logger does not exist, -1 is returned.
         ******************************************************************************************/
        int  GetLoggerNo( detail::Logger* logger)
        {
            for ( size_t i= 0; i < Data.size() ; ++i  )
                if ( logger == Data[i].Logger )
                    return static_cast<int>( i );
            return -1;
        }

        /** ****************************************************************************************
         * Sets the verbosity for a logger of this domain of all its sub domains to the specified
         * value. If given priority is lower than those actually stored, nothing is set and
         * recursion is stopped.
         *
         * @param loggerNo    The number of the \e Logger to set the \e Verbosity for.
         * @param verbosity   The verbosity value to set.
         * @param priority    The priority of the setting.
         * @return The new \e Verbosity.
         ******************************************************************************************/
        ALIB_API
        Verbosity SetVerbosity( int loggerNo, Verbosity verbosity, Priorities priority );

        /** ****************************************************************************************
         * Returns the <em>%Log %Domain's %Verbosity</em> for the given logger number.
         * @param loggerNo  The number of the \e Logger whose \e Verbosity is requested.
         * @return The found/defined domain \e Verbosity.
         ******************************************************************************************/
        Verbosity GetVerbosity( int loggerNo )
        {
            return Data[static_cast<size_t>(loggerNo)].LoggerVerbosity;
        }

        /** ****************************************************************************************
         * Returns the priority of the \e Verbosity setting for the given logger number.
         * @param loggerNo  The number of the \e Logger whose \e Verbosity is requested.
         * @return The priority.
         ******************************************************************************************/
        Priorities GetPriority( int loggerNo )
        {
            return Data[static_cast<size_t>(loggerNo)].Priority;
        }


        /** ****************************************************************************************
         * Returns the number of log calls for this domain and logger.
         * @param loggerNo  The number of the \e Logger whose \e Verbosity is requested.
         * @return The number of calls executed by this logger on this domain.
         ******************************************************************************************/
        int       GetCount( int loggerNo )
        {
            return Data[static_cast<size_t>(loggerNo)].LogCallsPerDomain;
        }

        /** ****************************************************************************************
         * Determines if the domain is active in respect to the given Verbosity.
         *
         * @param loggerNo  The number of the \e Logger whose \e Verbosity is to be evaluated against
         *                  \p{statement}.
         * @param statement The \e Verbosity to check.
         * @return  \c true if domain is active (log should be performed)
         ******************************************************************************************/
        bool      IsActive( int loggerNo, Verbosity statement )
        {
            Verbosity domain= GetVerbosity( loggerNo );

            //    domain ^  / stmnt > |   Off   Error  Warning   Info  Verbose
            //  ---------------------------------------------------------------------
            //    Off                 |    -      -       -        -      -
            //    Errors              |    -      Y       -        -      -
            //    Warning             |    -      Y       Y        -      -
            //    Info                |    -      Y       Y        Y      -
            //    Verbose             |    -      Y       Y        Y      Y

            if(    statement != Verbosity::Off
                && (    ( domain == Verbosity::Error       &&        statement == Verbosity::Error   )
                     || ( domain == Verbosity::Warning     &&    (   statement == Verbosity::Warning || statement == Verbosity::Error ) )
                     || ( domain == Verbosity::Info        &&        statement != Verbosity::Verbose )
                     ||   domain == Verbosity::Verbose )
              )
            {
                ++Data[static_cast<size_t>(loggerNo)].LogCallsPerDomain;
                return true;
            }

            return false;
        }

        /** ****************************************************************************************
         * Searches a domain. If not found, the domain is (or path of domains are) created in
         * the domain tree.
         * If the path string starts with the character defined with #Separator, then
         * the search (and creation) is done starting from the root domain of this domain and not
         * from this domain.
         *
         * @param       domainPath  Path and domain to search.
         * @param       maxCreate   The maximum number of sub domains that are created if not
         *                          found at the end of the path.
         * @param[out]  wasCreated  Output parameter that is set \c true if domain was not found
         *                          and hence created. If \c nullptr, it is ignored.
         * @return The domain found or created.
         ******************************************************************************************/
        ALIB_API
        Domain*   Find( NSubstring domainPath, int maxCreate, bool* wasCreated );


        /** ****************************************************************************************
         * Creates a string representation of this object.
         * @param target The target string.
         ******************************************************************************************/
        void      ToString( NAString& target );

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Internal, recursive helper of #Find.
         *
         * @param       domainPath  Path to search.
         * @param       maxCreate   The maximum number of sub domains that are created if not
         *                          found at the end of the path.
         * @param[out]  wasCreated  Output parameter that is set \c true if domain was not found
         *                          and hence created. If \c nullptr, it is ignored.
         * @return The domain found or created.
         ******************************************************************************************/
        Domain*     findRecursive( NSubstring&  domainPath, int maxCreate, bool* wasCreated );

        /** ****************************************************************************************
         * Internal, recursive helper of #AddLogger.
         * @param logger The logger to add.
         ******************************************************************************************/
        ALIB_API
        void        addLoggerRecursive( detail::Logger* logger);

        /** ****************************************************************************************
         * Internal, recursive helper of #RemoveLogger.
         * @param loggerNo  The number of the \e Logger to be removed.
         ******************************************************************************************/
        ALIB_API
        void        removeLoggerRecursive( int loggerNo );

}; // Domain

}// namespace [alib::lox::detail]

#if !defined(ALIB_DOX)
    ALIB_STRINGS_APPENDABLE_TYPE_INLINE( alib::lox::detail::PrefixLogable,
                                         target.Append(static_cast<const Box&>(src) ); )
#endif

#endif // HPP_ALOX_DETAIL_DOMAIN
