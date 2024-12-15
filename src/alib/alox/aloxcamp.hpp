//==================================================================================================
/// \file
/// This header file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_ALOX_CAMP
#define HPP_ALIB_ALOX_CAMP 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(ALOX)

#include "alib/alox/alox.hpp"
#include "alib/lang/basecamp/camp.hpp"
#include "alib/config/configcamp.hpp"
#include "alib/config/configuration.hpp"

namespace alib {  namespace lox {

//==================================================================================================
/// Configuration variables uses by ALox.
/// \note
///  As required by module \alib_config, this enumerations is equipped with
///  \ref alib_enums_records "ALib Enum Records" of type \alib{config;Declaration}  and resourced
///  with this module \alib_alox_nl.<br>
///  Hence, all variable categories, names, default values and such can be modified
///  by modifying the resource data of the singleton of class \alib{lang::basecamp;BaseCamp}.
//==================================================================================================
enum class Variables
{
    /// Denotes configuration variable \ref alxcvALOX_NO_IDE_LOGGER used by \alox{Log::AddDebugLogger}.
    NO_IDE_LOGGER        = 1,

    /// Denotes configuration variable \ref alxcvALOX_CONSOLE_TYPE used by \alox{Lox::CreateConsoleLogger}.
    CONSOLE_TYPE         = 2,

    /// Denotes configuration variable \ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME.
    VERBOSITY            = 3,

    /// Denotes configuration variable \ref alxcvALOX_GLOBAL_SOURCE_PATH_TRIM_RULES.
    SPTR_GLOBAL          = 4,

    /// Denotes configuration variable \ref alxcvALOX_LOXNAME_SOURCE_PATH_TRIM_RULES used by class \alox{Lox}.
    SPTR_LOX             = 5,

    /// Denotes configuration variable \ref alxcvALOX_LOXNAME_DOMAIN_SUBSTITUTION used by class \alox{Lox}.
    DOMAIN_SUBSTITUTION  = 6,

    /// Denotes configuration variable \ref alxcvALOX_LOXNAME_PREFIXES used by class \alox{Lox}.
    PREFIXES             = 7,

    /// Denotes configuration variable \ref alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT used by class \alox{Lox}.
    DUMP_STATE_ON_EXIT   = 8,

    /// Denotes configuration variable \ref alxcvALOX_LOGGERNAME_AUTO_SIZES used by class \alox{textlogger,TextLogger}.
    AUTO_SIZES           = 20,

    /// Denotes configuration variable \ref alxcvALOX_LOGGERNAME_FORMAT used by class \alox{textlogger,TextLogger}.
    FORMAT               = 21,

    /// Denotes configuration variable \ref alxcvALOX_LOGGERNAME_FORMAT_DATE_TIME used by class \alox{textlogger,TextLogger}.
    FORMAT_DATE_TIME     = 22,

    /// Denotes configuration variable \ref alxcvALOX_LOGGERNAME_FORMAT_MULTILINE used by class \alox{textlogger,TextLogger}.
    FORMAT_MULTILINE     = 23,

    /// Denotes configuration variable \ref alxcvALOX_LOGGERNAME_FORMAT_TIME_DIFF used by class \alox{textlogger,TextLogger}.
    FORMAT_TIME_DIFF     = 24,

    /// Denotes configuration variable \ref alxcvALOX_LOGGERNAME_FORMAT_OTHER used by class \alox{textlogger,TextLogger}.
    FORMAT_OTHER         = 25,

    /// Denotes configuration variable \ref alxcvALOX_LOGGERNAME_REPLACEMENTS used by class \alox{textlogger,TextLogger}.
    REPLACEMENTS         = 26,

    /// Denotes configuration variable \ref alxcvALOX_CONSOLE_LIGHT_COLORS used by colorful specializations of class \alox{textlogger,TextLogger}.
    CONSOLE_LIGHT_COLORS = 27,

    #if defined(_WIN32)
        /// Denotes configuration variable \ref alxcvALOX_CODEPAGE used by class \alox{loggers,WindowsConsoleLogger}.
        CODEPAGE         = 28,
    #endif
};

//==================================================================================================
/// Variable data struct, defining the verbosities of the domains of a \e Logger in a \e Lox.
/// This struct is used with the \ref alib_mod_config "configuration variable" named
/// \ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME.
///
/// Note that during the run of a software, the verbosity values are stored differently and this
/// variable type is only for importing and exporting such values to external systems.
//==================================================================================================
struct CVVerbosities : protected StringVectorPA
{
    bool        ExportAll= false;  ///< Determines whether only the minimum necessary domain tree
                                   ///< is written when verbosities are exported.
                                   ///< If \c true, redundant verbosity information is given,
                                   ///< even if Subdomains have the same value.
    /// Constructor.
    /// @param pAllocator The allocator to use.
    CVVerbosities(AllocatorType& pAllocator)
    : StringVectorPA(pAllocator)                                                                  {}

    /// Destructor. Frees all allocated strings.
    ~CVVerbosities()                                                                    { Clear(); }

    using StringVectorPA::Add;
    using StringVectorPA::begin;
    using StringVectorPA::end;
    using StringVectorPA::Size;

    /// @return \c true if this vector is empty, \c false otherwise.
    bool    IsEmpty()   { return empty(); }

    /// Frees all allocated strings and clears vector.
    void Clear()
    {
        for ( auto it = begin() ; it != end() ; ++it )
            it->Free( GetAllocator() );
        clear();
    }
};

//==================================================================================================
/// This is the module class for \alox logging library residing in namespace \ref alib::lox.
///
/// This is a strict singleton class. The only instance is found with namespace variable
/// \ref alib::ALOX.
//==================================================================================================
class ALoxCamp : public lang::Camp
{
    public:
        //==========================================================================================
        /// Constructor.<br>
        /// While this is public, it must not be invoked as this is a strict singleton type.
        /// (See notes in \ref alib_manual_camp_modules_campmodule).
        //==========================================================================================
        ALoxCamp();

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:

        #if defined(_WIN32)
            /// Attributes of corresponding configuration variable \ref alxcvALOX_CODEPAGE
            /// used by class \alox{loggers,WindowsConsoleLogger}.
            ALIB_API static  config::Declaration CODEPAGE;
        #endif

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:

    // #############################################################################################
    // Lox management
    // #############################################################################################
    public:
        //==========================================================================================
        /// Returns a \b %Lox with the given name.
        /// A \b %Lox is only found if it was created and registered with \alox using #Register.
        /// If not found, and parameter \p{create} is \c true (the default), a new \b Lox is created,
        /// registered and returned.
        ///
        /// @param name      The name of the \b %Lox to search and optionally to create.
        ///                  Comparison is case insensitive.
        /// @param create    Denotes whether a \b %Lox that was not found is created.
        ///                  Optional and defaults to \b %CreateIfNotExists::No.
        /// @return The \b Lox found, \c nullptr in case of failure.
        //==========================================================================================
        ALIB_API Lox*   Get( const NString&          name,
                             lang::CreateIfNotExists create= lang::CreateIfNotExists::No );

        //==========================================================================================
        /// Registers or un-registers a \b %Lox object statically with \alox.
        /// Once registered,  any code entity of the same process is enabled to retrieve
        /// the \b %Lox using #Get.<br>
        /// No two objects with the same name must be registered. If this is done, the latter
        /// will not be registered and not be found by #Get. In debug-compilations, an \alib
        /// error report is written (by default raises 'assert') if a name is registered twice.<br>
        /// Note that name comparison is performed case <b>in</b>-sensitive.
        ///
        /// If debug-logging is enabled (depends on optional compiler symbols) and used, the
        /// singleton of type \c %Lox provided for debug-logging is registered. This uses the name
        /// \c "Log".
        ///
        /// Registration is \e not mandatory but done by default by the constructor of class \b %Lox.
        /// Therefore, to keep a \b Lox private, an optional parameter is available.
        ///
        /// @param lox       The \b %Lox to register.
        /// @param operation If \b %ContainerOp::Remove, the given \p{Lox} is deregistered.
        ///                  Defaults to \b %ContainerOp::Insert.
        //==========================================================================================
        ALIB_API void   Register( Lox* lox, lang::ContainerOp operation );

        //==========================================================================================
        /// Resets this object. Concretely, the following steps are performed:
        /// - If the debug lox singleton exists, it is deleted.
        /// - It is asserted that no other lox object is registered.
        /// - The \alox path of the configuration is deleted.
        ///
        /// \attention
        ///   This method was introduced to support resetting \alox in the unit tests.
        ///   In real applications, this method should NOT be used.
        ///   Side effects might appear using this method and it is not tested otherwise than
        ///   used in tests!
        //==========================================================================================
        ALIB_API void   Reset();

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        //==========================================================================================
        /// Implementation of \alib{lang;Camp::bootstrap}.
        /// @param phase  The initialization phase to perform.
        //==========================================================================================
        virtual void    bootstrap( BootstrapPhases phase )                                 override;

        //==========================================================================================
        /// Implementation of \alib{lang;Camp::shutdown}.
        /// @param phase  The shutdown phase to perform.
        //==========================================================================================
        virtual void    shutdown( ShutdownPhases phase )                                   override;

};// class ALoxCamp

} // namespace alib[::lox]

/// The singleton instance of \alibcamp class \alib{lox;ALoxCamp}.
extern ALIB_API lox::ALoxCamp ALOX;

}  // namespace [alib]

ALIB_ENUMS_ASSIGN_RECORD( alib::lox::Variables, alib::config::Declaration )
ALIB_RESOURCED_IN_MODULE( alib::lox::Variables, alib::ALOX, "Var" )

// #################################################################################################
// T_Append<Scope> / T_Append<Verbosity>
// #################################################################################################
#if !DOXYGEN
namespace alib::strings {

template<> struct       T_Append<Scope,nchar, lang::HeapAllocator>
{
    ALIB_API
    void operator()( TAString<nchar, lang::HeapAllocator>& target, const Scope src );
};

template<> struct       T_Append<std::pair<Verbosity, Priority>,nchar, lang::HeapAllocator>
{
    ALIB_API
    void operator()( TAString<nchar, lang::HeapAllocator>& target, const std::pair<Verbosity, Priority>& src );
};

} // namespace [alib::strings]
#endif // !DOXYGEN

ALIB_CONFIG_VARIABLE_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::lox::, CVVerbosities, A_CHAR("ALOXV") )

#endif // HPP_ALIB_ALOX_CAMP

