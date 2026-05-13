//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace lox {

//==================================================================================================
/// Configuration variables uses by ALox.
/// \note
///   As required by module \alib_variables, this enumerations is equipped with
///   #"alib_enums_records;ALib Enum Records" of type #"var Declaration"  and resourced
///   with this module \alib_alox_nl.<br>
///   Hence, all variable categories, names, default values and such can be modified
///   by modifying the resource data of the singleton of class #"BASECAMP;2".
//==================================================================================================
enum class Variables {
    /// Denotes configuration variable #"alxcvALOX_NO_IDE_LOGGER" used by #"Log::AddDebugLogger;2".
    NO_IDE_LOGGER        = 1,

    /// Denotes configuration variable #"alxcvALOX_CONSOLE_TYPE" used by #"Lox::CreateConsoleLogger;2".
    CONSOLE_TYPE         = 2,

    /// Denotes configuration variable #"alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME".
    VERBOSITY            = 3,

    /// Denotes configuration variable #"alxcvALOX_GLOBAL_SOURCE_PATH_TRIM_RULES".
    SPTR_GLOBAL          = 4,

    /// Denotes configuration variable #"alxcvALOX_LOXNAME_SOURCE_PATH_TRIM_RULES" used by class #"Lox".
    SPTR_LOX             = 5,

    /// Denotes configuration variable #"alxcvALOX_LOXNAME_DOMAIN_SUBSTITUTION" used by class #"Lox".
    DOMAIN_SUBSTITUTION  = 6,

    /// Denotes configuration variable #"alxcvALOX_LOXNAME_PREFIXES" used by class #"Lox".
    PREFIXES             = 7,

    /// Denotes configuration variable #"alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT" used by class #"Lox".
    DUMP_STATE_ON_EXIT   = 8,

    /// Denotes configuration variable #"alxcvALOX_LOGGERNAME_AUTO_SIZES" used by class #"TextLogger".
    AUTO_SIZES           = 20,

    /// Denotes configuration variable #"alxcvALOX_LOGGERNAME_FORMAT" used by class #"TextLogger".
    FORMAT               = 21,

    /// Denotes configuration variable #"alxcvALOX_LOGGERNAME_FORMAT_DATE_TIME" used by class #"TextLogger".
    FORMAT_DATE_TIME     = 22,

    /// Denotes configuration variable #"alxcvALOX_LOGGERNAME_FORMAT_MULTILINE" used by class #"TextLogger".
    FORMAT_MULTILINE     = 23,

    /// Denotes configuration variable #"alxcvALOX_LOGGERNAME_FORMAT_TIME_DIFF" used by class #"TextLogger".
    FORMAT_TIME_DIFF     = 24,

    /// Denotes configuration variable #"alxcvALOX_LOGGERNAME_FORMAT_OTHER" used by class #"TextLogger".
    FORMAT_OTHER         = 25,

    /// Denotes configuration variable #"alxcvALOX_LOGGERNAME_REPLACEMENTS" used by class #"TextLogger".
    REPLACEMENTS         = 26,

    /// Denotes configuration variable #"alxcvALOX_CONSOLE_LIGHT_COLORS" used by colorful specializations of class #"TextLogger".
    CONSOLE_LIGHT_COLORS = 27,

    #if defined(_WIN32)
        /// Denotes configuration variable #"alxcvALOX_CODEPAGE" used by the class #"WindowsConsoleLogger".
        CODEPAGE         = 28,
    #endif
};

//==================================================================================================
/// Variable data struct, defining the verbosities of the domains of a \e Logger in a \e Lox.
/// This struct is used with the #"alib_mod_variables;configuration variable" named
/// #"alxcvALOX_LOGGERNAME_VERBOSITY_WITH_LOXNAME".
///
/// Note that during the run of software, the verbosity values are stored differently and this
/// variable type is only for importing and exporting such values to external systems.
//==================================================================================================
struct CVVerbosities : protected StringVectorPA {
    /// Determines whether only the minimum necessary domain tree is written when the verbosities
    /// are exported. If \c true, redundant verbosity information is given, even if Subdomains
    /// have the same value.
    bool        ExportAll                                                                    =false;

    /// Determines whether changes should be written back to configuration sources, if possible.
    /// @see Methods #"Lox::SetVerbosityExport(detail::Logger*);2", #"Variable::IsWriteBack;2",
    /// and the virtual method used to implement this feature: #"VMeta::isWriteBack;2".
    bool        WriteBack                                                                    =false;

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
    bool    IsEmpty()                                                            { return empty(); }

    /// Frees all allocated strings and clears vector.
    void Clear() {
        for ( auto it = begin() ; it != end() ; ++it )
            it->Free( GetAllocator() );
        clear();
    }
};

//==================================================================================================
/// This is the camp class for \alox logging library residing in namespace #"alib::lox".
///
/// This is a strict singleton class. The only instance is found with the namespace variable
/// #"alib::ALOX".
//==================================================================================================
class ALoxCamp : public camp::Camp {
  public:
    /// Constructor.<br>
    /// While this is public, it must not be invoked as this is a strict singleton type.
    /// (See notes in #"alib_camp_camp").
    ALoxCamp();

    // Public fields
  public:

        #if defined(_WIN32)
            /// Attributes of corresponding configuration variable #"alxcvALOX_CODEPAGE"
            /// used by class #"WindowsConsoleLogger".
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
    /// Implementation of #"Camp::Bootstrap;*".
    void    Bootstrap()                                                                    override;

    /// Implementation of #"Camp::Shutdown;*".
    /// @param phase  The shutdown phase to perform.
    void    Shutdown( ShutdownPhases phase )                                               override;

};// class ALoxCamp

} // namespace alib[::lox]

/// The singleton instance of \alibcamp class #"ALoxCamp".
extern ALIB_DLL lox::ALoxCamp ALOX;

}  // namespace [alib]

ALIB_ENUMS_ASSIGN_RECORD( alib::lox::Variables, alib::variables::Declaration )
ALIB_RESOURCED_IN_CAMP( alib::lox::Variables, alib::ALOX, "Var" )

//##################################################################################################
// AppendableTraits<Scope> / AppendableTraits<Verbosity>
//##################################################################################################
#if !DOXYGEN
ALIB_EXPORT namespace alib::strings {

template<> struct       AppendableTraits<Scope,nchar, lang::HeapAllocator> {
    ALIB_DLL
    void operator()( TAString<nchar, lang::HeapAllocator>& target, const Scope src );
};

template<> struct       AppendableTraits<Pair<Verbosity, Priority>,nchar, lang::HeapAllocator> {
    ALIB_DLL
    void operator()( TAString<nchar, lang::HeapAllocator>& target, const Pair<Verbosity, Priority>& src );
};

} // namespace [alib::strings]

ALIB_EXPORT namespace alib::variables::detail {
struct VMeta_CVVerbosities : public VMeta {
         ALIB_DLL String                typeName ()       const override { return A_CHAR("ALOXV"); }
ALIB_DBG(ALIB_DLL const std::type_info& dbgTypeID()                                                            override { return typeid(alib::lox::CVVerbosities); }  )
         ALIB_DLL void                  construct(VDATA* obj, PoolAllocator& pool)                             override { new (obj) alib::lox::CVVerbosities(pool); }
         ALIB_DLL void                  destruct (VDATA* obj, PoolAllocator&)                                  override { reinterpret_cast<alib::lox::CVVerbosities*>(obj)->~CVVerbosities(); }
         ALIB_DLL size_t                size     ()                                                            override { static_assert(alignof(alib::lox::CVVerbosities) <= alib::PoolAllocator::MAX_ALIGNMENT); return (std::max)( sizeof(alib::lox::CVVerbosities), sizeof(void*) ); }
         ALIB_DLL void                  imPort   (VDATA*, Configuration&, const StringEscaper&, const String&) override;
         ALIB_DLL void                  exPort   (VDATA*, Configuration&, const StringEscaper&,      AString&) override;
         ALIB_DLL bool                isWriteBack(VDATA* obj)                                            const override { return reinterpret_cast<alib::lox::CVVerbosities*>(obj)->WriteBack; }
};}
#endif // !DOXYGEN
