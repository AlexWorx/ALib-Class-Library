//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib::boxing {
#include "ALib.Lang.CIFunctions.H"

//==================================================================================================
/// This namespace function initializes the module \alib_boxing.
///
/// This function is automatically called by the #"alib_mod_bs;standard bootstrap" code of \alib,
/// hence the (overloaded) functions #"alib::Bootstrap(BootstrapPhases);2" will call this function.
//==================================================================================================
ALIB_DLL
void        bootstrap();

//==================================================================================================
/// Frees resources and shuts down module \alib_boxing.
/// Multiple invocations of this method are forbidden.
/// The #"alib_mod_bs;standard bootstrap" code of \alib, hence the (overloaded)
/// functions #"alib::Shutdown;2" will call this function.
///
/// This function is automatically called by the #"alib_mod_bs;standard bootstrap" code of \alib,
/// hence the (overloaded) functions #"alib::Bootstrap(BootstrapPhases);2" will call this function.
//==================================================================================================
ALIB_DLL
void       shutdown();

#include "ALib.Lang.CIMethods.H"
} // namespace [alib::boxing]
