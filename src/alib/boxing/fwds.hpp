/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_boxing of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BOXING_FWDS
#define HPP_ALIB_BOXING_FWDS 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)

namespace alib {  namespace boxing {

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


} // namespace alib[::boxing]


/// Type alias in namespace \b alib.
using     Box           =   boxing::Box;

/// Type alias in namespace \b alib.
using     Boxes         =   boxing::Boxes;

/// Type alias in namespace \b alib.
using     Enum          =   boxing::Enum;

/// Type alias in namespace \b alib.
using     FHashcode     =   boxing::FHashcode;

/// Type alias in namespace \b alib.
using     FClone        =   boxing::FClone;

/// Type alias in namespace \b alib.
using     FEquals       =   boxing::FEquals;

/// Type alias in namespace \b alib.
using     FIsNotNull    =   boxing::FIsNotNull;

/// Type alias in namespace \b alib.
using     FIsLess       =   boxing::FIsLess;

/// Type alias in namespace \b alib.
using     FIsTrue       =   boxing::FIsTrue;

#if ALIB_STRINGS
/// Type alias in namespace \b alib.
template<typename TChar>
using     FAppend       =   boxing::FAppend<TChar>;
#endif
} // namespace [alib]

#endif // HPP_ALIB_BOXING_FWDS
