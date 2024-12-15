
//==================================================================================================
/// \file
/// This header file is part of module \alib_bitbuffer of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BITBUFFER_ARRAY_COMPRESSOR
#define HPP_ALIB_BITBUFFER_ARRAY_COMPRESSOR
#pragma once
#include "alib/bitbuffer/ac_v1/ac.hpp"

namespace alib {

/// Type alias in namespace \b alib, referencing the sub-namespace within #alib::bitbuffer
/// which provides the current version of array compression.
/// \see Namespace #alib::bitbuffer::ac_v1 for more information.
using    ArrayCompressor = bitbuffer::ac_v1::ArrayCompressor;

/// Type alias in namespace \b alib, referencing the sub-namespace within #alib::bitbuffer
/// which provides the current version of array compression.
/// \see Namespace #alib::bitbuffer::ac_v1 for more information.
using    HuffmanEncoder =  bitbuffer::ac_v1::HuffmanEncoder;

/// Type alias in namespace \b alib, referencing the sub-namespace within #alib::bitbuffer
/// which provides the current version of array compression.
/// \see Namespace #alib::bitbuffer::ac_v1 for more information.
using    HuffmanDecoder =  bitbuffer::ac_v1::HuffmanDecoder;

} // namespace [alib]

#endif // HPP_ALIB_BITBUFFER_ARRAY_COMPRESSOR

