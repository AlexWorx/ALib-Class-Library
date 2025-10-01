//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace lox {

//==================================================================================================
/// Configuration variables uses by ALox.
/// \note
///  As required by module \alib_variables, this enumerations is equipped with
///  \ref alib_enums_records "ALib Enum Records" of type \alib{variables;Declaration}  and resourced
///  with this module \alib_alox_nl.<br>
///  Hence, all variable categories, names, default values and such can be modified
///  by modifying the resource data of the singleton of class \alib{camp;Basecamp}.
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
/// This struct is used with the \ref alib_mod_variables "configuration variable" named
/// \ref alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME.
///
/// Note that during the run of software, the verbosity values are stored differently and this
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
/// This is the camp class for \alox logging library residing in namespace \ref alib::lox.
///
/// This is a strict singleton class. The only instance is found with the namespace variable
/// \ref alib::ALOX.
//==================================================================================================
class ALoxCamp : public camp::Camp
{
    public:
        /// Constructor.<br>
        /// While this is public, it must not be invoked as this is a strict singleton type.
        /// (See notes in \ref alib_camp_camp).
        ALoxCamp();

    // Public fields
    public:

        #if defined(_WIN32)
            /// Attributes of corresponding configuration variable \ref alxcvALOX_CODEPAGE
            /// used by class \alox{loggers,WindowsConsoleLogger}.
            ALIB_DLL static  variables::Declaration CODEPAGE;
        #endif

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
        ALIB_DLL void   Reset();
    
    // Internals
    protected:
        /// Implementation of \alib{camp;Camp::Bootstrap}.
        virtual void    Bootstrap()                                                        override;

        /// Implementation of \alib{camp;Camp::Shutdown}.
        /// @param phase  The shutdown phase to perform.
        virtual void    Shutdown( ShutdownPhases phase )                                   override;

};// class ALoxCamp

} // namespace alib[::lox]

/// The singleton instance of \alibcamp class \alib{lox;ALoxCamp}.
extern ALIB_DLL lox::ALoxCamp ALOX;

}  // namespace [alib]

ALIB_ENUMS_ASSIGN_RECORD( alib::lox::Variables, alib::variables::Declaration )
ALIB_RESOURCED_IN_MODULE( alib::lox::Variables, alib::ALOX, "Var" )

// #################################################################################################
// AppendableTraits<Scope> / AppendableTraits<Verbosity>
// #################################################################################################
#if !DOXYGEN
ALIB_EXPORT namespace alib::strings {

template<> struct       AppendableTraits<Scope,nchar, lang::HeapAllocator>
{
    ALIB_DLL
    void operator()( TAString<nchar, lang::HeapAllocator>& target, const Scope src );
};

template<> struct       AppendableTraits<Pair<Verbosity, Priority>,nchar, lang::HeapAllocator>
{
    ALIB_DLL
    void operator()( TAString<nchar, lang::HeapAllocator>& target, const Pair<Verbosity, Priority>& src );
};

} // namespace [alib::strings]
#endif // !DOXYGEN

ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::lox::, CVVerbosities, A_CHAR("ALOXV") )


