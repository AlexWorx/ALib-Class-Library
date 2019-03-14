// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALOX_CORE_DOMAIN
#define HPP_ALOX_CORE_DOMAIN 1

#if !defined(HPP_ALIB_LOX_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

namespace aworx { namespace lib { namespace lox {

class PrefixLogable;

namespace detail {


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
        detail::Logger*       Logger;

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
        NString32                           Name;

        /** The parent domain. For root domains, this is \c nullptr. */
        Domain*                             Parent;

        /** A list of sub domains. */
        std::vector<Domain*>                SubDomains;

        /** Data stored per logger. The index is corresponding to the list of loggers in 'our' Lox. */
        std::vector<LoggerData>             Data;

        /** The full path of the domain (set in the constructor once) . */
        NString64                           FullPath;

       /**
         * A counter for the quantity of calls on this domain.
         * The does not include:
         * - logs when no \e Logger was set
         * - conditional logs that were suppressed
         * Otherwise, it includes all log calls, even when no \e Logger was enabled on this domain.
         */
        int                                 CntLogCalls                                          =0;

        /** <em>Prefix Logables</em> associated with this domain. */
        std::vector<std::pair<PrefixLogable*, Inclusion>>   PrefixLogables;

        /** Flag to which is set when verbosity configuration data was read. */
        bool                                ConfigurationRead                                =false;

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
         * Constructor
         * @param parent    The parent domain. For root domains, this is \c nullptr.
         * @param name      The name of the domain. For root domains, this is \e nulled.
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
        inline
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
        inline
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
        inline
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
        inline
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
        inline
        detail::Logger*  GetLogger( const NString& loggerName )
        {
            for ( size_t i= 0; i < Data.size() ; i++  )
                if ( loggerName.Equals<Case::Ignore>( Data[i].Logger->GetName()) )
                    return Data[i].Logger;
            return nullptr;
        }

        /** ****************************************************************************************
         * Returns logger of given number.
         * @param no  The number of the \e Logger to return.
         * @return The \e Logger found with number \p{no}.
         ******************************************************************************************/
        inline
        detail::Logger*  GetLogger( int no )
        {
            ALIB_ASSERT_ERROR( no < static_cast<int>(Data.size()), "Internal error: Illegal Logger Number" )
            return Data[static_cast<size_t>(no)].Logger;
        }

        /** ****************************************************************************************
         * Returns the number of the \e Logger specified by name.
         * @param loggerName  The logger name to search.
         * @return The number of the \e Logger found corresponding to given name.
         *         If the \e Logger does not exist, -1 is returned.
         ******************************************************************************************/
        inline
        int  GetLoggerNo( const NString& loggerName )
        {
            for ( size_t i= 0; i < Data.size() ; i++  )
                if ( loggerName.Equals<Case::Ignore>( Data[i].Logger->GetName() ) )
                    return static_cast<int>( i );
            return -1;
        }

        /** ****************************************************************************************
         * Returns the number of the \e Logger.
         * @param logger  The logger to search.
         * @return The number of the \e Logger. If the \e Logger does not exist, -1 is returned.
         ******************************************************************************************/
        inline
        int  GetLoggerNo( detail::Logger* logger)
        {
            for ( size_t i= 0; i < Data.size() ; i++  )
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
        inline
        Verbosity SetVerbosity( int loggerNo, Verbosity verbosity, Priorities priority )
        {
            LoggerData& ld= Data[static_cast<size_t>(loggerNo)];
            if( priority >= ld.Priority )
            {
                ld.Priority=        priority;
                ld.LoggerVerbosity= verbosity;

                for( Domain* subDomain : SubDomains )
                    subDomain->SetVerbosity( loggerNo, verbosity, priority );
            }
            return ld.LoggerVerbosity;
        }

        /** ****************************************************************************************
         * Returns the <em>%Log %Domain's %Verbosity</em> for the given logger number.
         * @param loggerNo  The number of the \e Logger whose \e Verbosity is requested.
         * @return The found/defined domain \e Verbosity.
         ******************************************************************************************/
        inline
        Verbosity GetVerbosity( int loggerNo )
        {
            return Data[static_cast<size_t>(loggerNo)].LoggerVerbosity;
        }

        /** ****************************************************************************************
         * Returns the priority of the \e Verbosity setting for the given logger number.
         * @param loggerNo  The number of the \e Logger whose \e Verbosity is requested.
         * @return The priority.
         ******************************************************************************************/
        inline
        Priorities GetPriority( int loggerNo )
        {
            return Data[static_cast<size_t>(loggerNo)].Priority;
        }


        /** ****************************************************************************************
         * Returns the number of log calls for this domain and logger.
         * @param loggerNo  The number of the \e Logger whose \e Verbosity is requested.
         * @return The number of calls executed by this logger on this domain.
         ******************************************************************************************/
        inline
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
        inline
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
                Data[static_cast<size_t>(loggerNo)].LogCallsPerDomain++;
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
        inline
        void        addLoggerRecursive( detail::Logger* logger)
        {
            Data.emplace_back( LoggerData( logger ) );
            for( Domain* subDomain : SubDomains )
                subDomain->addLoggerRecursive( logger );
        }

        /** ****************************************************************************************
         * Internal, recursive helper of #RemoveLogger.
         * @param loggerNo  The number of the \e Logger to be removed.
         ******************************************************************************************/
        inline
        void        removeLoggerRecursive( int loggerNo )
        {
            Data.erase( Data.begin() + loggerNo );
            for( Domain* subDomain : SubDomains )
                subDomain->removeLoggerRecursive( loggerNo );
        }

}; // Domain

}}}}// namespace [aworx::lib::lox::detail]

#endif // HPP_ALOX_CORE_DOMAIN
