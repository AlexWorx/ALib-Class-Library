//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::boxing {
#include "ALib.Lang.CIFunctions.H"

//==================================================================================================
/// This namespace function initializes the module \alib_boxing.
///
/// This function is automatically called by the
/// \ref alib_mod_bs "standard bootstrap" code of \alib, hence the (overloaded)
/// functions \ref alib::Bootstrap will call this function.
//==================================================================================================
ALIB_DLL
void        bootstrap();

//==================================================================================================
/// Frees resources and shuts down module \alib_boxing.
/// Multiple invocations of this method are forbidden.
/// The \ref alib_mod_bs "standard bootstrap" code of \alib, hence the (overloaded)
/// functions \ref alib::Shutdown will call this function.
///
/// This function is automatically called by the
/// \ref alib_mod_bs "standard bootstrap" code of \alib, hence the (overloaded)
/// functions \ref alib::Bootstrap will call this function.
//==================================================================================================
ALIB_DLL
void       shutdown();

#include "ALib.Lang.CIMethods.H"
} // namespace [alib::boxing]





