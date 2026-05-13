#include "alib_precompile.hpp"
//DOX_MARKER([ALIB_MANUAL_T_HPP_FILE_USAGE_IMPL] )
// ================================================================================================#
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// ================================================================================================#
#include "ALib.Monomem.H"

// Under windows, we cannot do the following within the unit tests project, which imports the
// ALib as a DLL. With Windows OS, it has to be done in an external lib.
// Also, with C++20-Module compilation, things become a little more complicated.
// We do not provide further testing here.
#if !defined(_WIN32) && !ALIB_C20_MODULES


#include "alib/monomem/monoallocator.t.hpp"
namespace alib::monomem {  template class TMonoAllocator<alib::PoolAllocatorHA>;  }

#endif
//DOX_MARKER([ALIB_MANUAL_T_HPP_FILE_USAGE_IMPL])

