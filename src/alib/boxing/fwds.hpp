/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_boxing of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BOXING_FWDS
#define HPP_ALIB_BOXING_FWDS 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
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
ALIB_IF_STRINGS(
template<typename TChar>
struct FAppend;             )


}}// namespace [aworx::lib::boxing]


/// Type alias in namespace #aworx.
using     Box           =   lib::boxing::Box;

/// Type alias in namespace #aworx.
using     Boxes         =   lib::boxing::Boxes;

/// Type alias in namespace #aworx.
using     Enum          =   lib::boxing::Enum;

/// Type alias in namespace #aworx.
using     FHashcode     =   lib::boxing::FHashcode;

/// Type alias in namespace #aworx.
using     FClone        =   lib::boxing::FClone;

/// Type alias in namespace #aworx.
using     FEquals       =   lib::boxing::FEquals;

/// Type alias in namespace #aworx.
using     FIsNotNull    =   lib::boxing::FIsNotNull;

/// Type alias in namespace #aworx.
using     FIsLess       =   lib::boxing::FIsLess;

/// Type alias in namespace #aworx.
using     FIsTrue       =   lib::boxing::FIsTrue;

#if ALIB_STRINGS
/// Type alias in namespace #aworx.
template<typename TChar>
using     FAppend       =   lib::boxing::FAppend<TChar>;
#endif
} // namespace aworx

#endif // HPP_ALIB_BOXING_FWDS

