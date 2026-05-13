//==================================================================================================
/// \file
/// This header-file is part of module \alib_variables of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================

ALIB_EXPORT namespace alib { namespace variables {
#if !ALIB_SINGLE_THREADED || DOXYGEN
/// This class provides an implementation of abstract interface class
/// #"ResourcePool", which does externalize resources by using the mechanics
/// provided with module \alib_variables.
///
/// A (shared) instance of this class can be attached to each \alibmod, by
/// #"alib_mod_bs;customizing the bootstrapping of ALib".
///
/// \see
///   Please consult the #"alib_mod_resources;Programmer's Manual" of module
///   \alib_resources_nl for details on the concept of resources. Details on this specific type are
///   given in chapter #"alib_resources_interface_config".
class ConfigResourcePool : public ResourcePool
                         , public TSharedConfiguration<SharedLock> {

#else
class ConfigResourcePool : public ResourcePool
                         , public TSharedConfiguration<void> {
#endif
  protected:
    /// Protected definition of inherited method.
    /// @param createDefaults Determines if default plug-ins are to be created.
    ///                       Defaults to \c Yes.
    void Reset(lang::CreateDefaults createDefaults = lang::CreateDefaults::Yes) {
        TSharedConfiguration::Reset(createDefaults);
    }

  public:
    /// Constructor. Creates a configuration object without any default plug-ins.
    /// After construction, the "resource data", in this case variable values, has to be added.
    /// @param initialBufferSizeInKB The initial size of memory buffers.
    ///                              Defaults to \c 16.
    ///                              Passed to the constructor parent #"Configuration".
    /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                              with each next buffer allocation.
    ///                              Should be set to \c 200, to double the size with each
    ///                              allocation.
    ///                              Defaults to \c 200.
    ///                              Passed to the constructor parent #"Configuration".
    ConfigResourcePool(size_t   initialBufferSizeInKB = 4,
                       unsigned bufferGrowthInPercent = 200)
    : TSharedConfiguration(initialBufferSizeInKB, bufferGrowthInPercent,
                           lang::CreateDefaults::No)                                              {}

    /// Default Destructor.
    virtual ~ConfigResourcePool()                                                 override =default;

    /// Implements abstract method #"ResourcePool::BootstrapAddOrReplace;*".
    ///
    /// @param category   Category string of the resource.
    /// @param name       Name string of the resource
    /// @param data       The resource string.
    /// @return \c true if the resource did exist and was replaced, \c false if it was an insertion.
    bool BootstrapAddOrReplace( const NString& category,
                                const NString& name,
                                const  String& data      )                                 override;

    /// Implements abstract method #"ResourcePool::BootstrapBulk;*".
    /// All variables are written using #"Priority::DefaultValues".
    ///
    /// @param category    Category string of the resource. For technical reasons, this has to be
    ///                    of type <c>const nchar*</c>.
    /// @param ...         A list of pairs of <b>const nchar*</b> and <b>const character*</b>
    ///                    keys and data.
    ALIB_DLL

    virtual
    void BootstrapBulk(const nchar *category, ...)                                         override;

#if DOXYGEN
    /// Implements abstract method #"ResourcePool::Get;*".
    ///
    /// @param category   Category string of the resource.
    /// @param name       Name string of the resource
    /// @param dbgAssert  This parameter is available (and to be passed) only in debug mode.
    ///                   If \c true, an #"alib_mod_assert;error is raised" if the resource was not found.
    /// @return The resource string, respectively a \e nulled string on failure.
    ALIB_DLL
    const String& Get(const NString& category, const NString& name, bool dbgAssert) override;
#else
    ALIB_DLL

    const String& Get(const NString& category, const NString& name
                      ALIB_DBG(, bool dbgAssert))                                          override;
#endif
}; // class ConfigResourcePool
}  // namespace alib[::config]

/// Type alias in namespace #"%alib".
using ConfigResourcePool = variables::ConfigResourcePool;
} // namespace [alib]
