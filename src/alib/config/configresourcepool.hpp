//==================================================================================================
/// \file
/// This header file is part of inner namespace #alib::lang::resources of module \alib_basecamp of
/// the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LANG_RESOURCES_CONFIG_RESOURCE_POOL
#define HPP_ALIB_LANG_RESOURCES_CONFIG_RESOURCE_POOL 1
#pragma once

#include "alib/lang/resources/resources.hpp"
ALIB_ASSERT_MODULE(CONFIGURATION)

#include "alib/lang/resources/detail/resourcemap.hpp"
#include "alib/config/configuration.hpp"
#if ALIB_THREADS
#   include "alib/threads/recursivelock.hpp"
#endif

namespace alib { namespace config {

#if ALIB_THREADS || DOXYGEN
/// This class provides an implementation of abstract interface class
/// \alib{lang::resources;ResourcePool}, which does externalize resources by using the mechanics
/// provided with module \alib_config.
///
/// A (shared) instance of this class can be attached to each \alibmod, with
/// \ref alib_manual_bootstrapping "customization the bootstrapping of ALib".
///
/// \see
///  Please consult the \ref alib_basecamp_resources "Programmer's Manual" of module
///  \alib_basecamp_nl for details on the concept of resources. Details on this specific type are
///  given in chapter \ref alib_basecamp_resources_interface_config "3.4.2 Class ConfigResourcePool".
class ConfigResourcePool  : public ResourcePool
                          , public TSharedConfiguration<SharedLock>
{
#else
class ConfigResourcePool  : public ResourcePool
                          , public TSharedConfiguration<void>
{
#endif
  //========================================   Internals   =========================================
  protected:

    /// Protected definition of inherited method.
    /// @param createDefaults Determines if default plug-ins are to be created.
    ///                       Defaults to \c Yes.
    void            Reset(lang::CreateDefaults createDefaults= lang::CreateDefaults::Yes)
    {  TSharedConfiguration::Reset( createDefaults); }

  //=================================   Construction/Destruction   =================================
  public:
    /// Constructor. Creates a configuration object without any default plug-ins.
    /// After construction, the "resource data", in this case variable values, has to be added.
    /// @param initialBufferSizeInKB The initial size of memory buffers.
    ///                              Defaults to \c 16.
    ///                              Passed to the constructor parent \alib{config;Configuration}.
    /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                              with each next buffer allocation.
    ///                              Should be set to \c 200, to double the size with each
    ///                              allocation.
    ///                              Defaults to \c 200.
    ///                              Passed to the constructor parent \alib{config;Configuration}.
    ConfigResourcePool(size_t          initialBufferSizeInKB = 4,
                       unsigned int    bufferGrowthInPercent = 200  )
    : TSharedConfiguration(initialBufferSizeInKB, bufferGrowthInPercent, lang::CreateDefaults::No) {}

    /// Default Destructor.
    virtual ~ConfigResourcePool()                                               override  = default;

  //==================================   ResourcePool Interface   ==================================
    //==============================================================================================
    /// Implements abstract method \alib{lang::resources;ResourcePool::BootstrapAddOrReplace}.
    ///
    /// @param category   Category string of the resource.
    /// @param name       Name string of the resource
    /// @param data       The resource string.
    /// @return \c true if the resource did exist and was replaced, \c false if it was an insertion.
    //==============================================================================================
    bool BootstrapAddOrReplace( const NString& category,
                                const NString& name,
                                const  String& data        )                               override;


    //==============================================================================================
    /// Implements abstract method \alib{lang::resources;ResourcePool::BootstrapBulk}.
    /// All variables are written using \alib{config;Priority;Priority::DefaultValues}.
    ///
    /// @param category    Category string of the resource. For technical reasons, this has to be
    ///                    of type <c>const nchar*</c>.
    /// @param ...         A list of pairs of <b>const nchar*</b> and <b>const character*</b>
    ///                    keys and data.
    //==============================================================================================
    ALIB_API
    virtual
    void BootstrapBulk( const nchar* category, ... )                                       override;

#if DOXYGEN
    //==============================================================================================
    /// Implements abstract method \alib{lang::resources;ResourcePool::Get}.
    ///
    /// @param category   Category string of the resource.
    /// @param name       Name string of the resource
    /// @param dbgAssert  This parameter is available (and to be passed) only in debug mode.
    ///                   If \c true, an assertion is raised if the resource was not found.
    /// @return The resource string, respectively a \e nulled string on failure.
    //==============================================================================================
    NALIB_API
    const String& Get( const NString& category, const NString& name, bool dbgAssert   )    override;
#else
    ALIB_API
    const String& Get( const NString& category, const NString& name
                                                          ALIB_DBG(, bool dbgAssert)  )    override;
#endif

}; // class ConfigResourcePool

} // namespace alib[::config]


/// Type alias in namespace \b alib.
using     ConfigResourcePool=       config::ConfigResourcePool;


} // namespace [alib]


#endif // HPP_ALIB_LANG_RESOURCES_CONFIG_RESOURCE_POOL

