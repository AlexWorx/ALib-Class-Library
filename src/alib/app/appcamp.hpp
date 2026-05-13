//##################################################################################################
//  ALib C++ Framework
//  Configuration Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
//##################################################################################################
ALIB_EXPORT namespace alib::app {

/// This camp holds resources for the predefined CLI functionality.
/// It is recommended common practice to inject further (and missing) resources into this
/// camp in an overloaded version of the method #"App::onBsPrepareConfig;*".
class AppCamp  : public alib::camp::Camp {
  public:
    /// Constructor. Passes version number and resource name to the module class.
    AppCamp() : Camp("APP")                                                                       {}

  protected:
    /// Initialization of the module.
    ALIB_DLL
    virtual void    Bootstrap()                                                            override;

    /// Terminate this module. (Nothing to do.)
    ALIB_DLL
    virtual void    Shutdown( alib::ShutdownPhases )                                     override {}
};

} // namespace [alib::app]

ALIB_EXPORT namespace alib {

/// The singleton instance of the camp class used by class #"App".
extern app::AppCamp APP;

} // namespace [alib]
