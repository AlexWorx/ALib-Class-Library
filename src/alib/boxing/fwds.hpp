// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_BOXING_FWDS
#define HPP_ALIB_BOXING_FWDS 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)

namespace aworx { namespace lib { namespace boxing {

namespace detail {  struct  VTable;  }
class  Box;
class  Boxes;
struct Enum;

struct FHashcode;
struct FClone;
struct FIsNotNull;
struct FIsEmpty;
struct FEquals;
struct FIsLess;
struct FIsTrue;
#if ALIB_MODULE_STRINGS
template<typename TChar>
struct FAppend;
#endif


}}// namespace [aworx::lib::boxing]


/// Type alias in namespace #aworx.
using     Box=         aworx::lib::boxing::Box;

/// Type alias in namespace #aworx.
using     Boxes=       aworx::lib::boxing::Boxes;

/// Type alias in namespace #aworx.
using     Enum=        aworx::lib::boxing::Enum;

/// Type alias in namespace #aworx.
using     FHashcode             =   aworx::lib::boxing::FHashcode;

/// Type alias in namespace #aworx.
using     FClone                =   aworx::lib::boxing::FClone;

/// Type alias in namespace #aworx.
using     FEquals               =   aworx::lib::boxing::FEquals;

/// Type alias in namespace #aworx.
using     FIsNotNull            =   aworx::lib::boxing::FIsNotNull;

/// Type alias in namespace #aworx.
using     FIsLess               =   aworx::lib::boxing::FIsLess;

/// Type alias in namespace #aworx.
using     FIsTrue               =   aworx::lib::boxing::FIsTrue;

#if ALIB_MODULE_STRINGS
/// Type alias in namespace #aworx.
template<typename TChar>
using     FAppend   =   aworx::lib::boxing::FAppend<TChar>;
#endif
} // namespace aworx

#endif // HPP_ALIB_BOXING_FWDS

