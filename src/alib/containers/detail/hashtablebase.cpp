// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/containers/hashtable.hpp"
#endif // !DOXYGEN


namespace alib::containers {
namespace detail {

#if !DOXYGEN

const uinteger PRIME_NUMBERS[] =
{
                      97ul ,   //  0
                     199ul ,   //  1
                     409ul ,   //  2
                     823ul ,   //  3
                    1741ul ,   //  4
                    3469ul ,   //  5
                    6949ul ,   //  6
                   14033ul ,   //  7
                   28411ul ,   //  8
                   57557ul ,   //  9
                  116731ul ,   // 10
                  236897ul ,   // 11
                  480881ul ,   // 12
                  976369ul ,   // 13
                 1982627ul ,   // 14
                 4026031ul ,   // 15
                 8175383ul ,   // 16
                16601593ul ,   // 17
                33712729ul ,   // 18
                68460391ul ,   // 19
               139022417ul ,   // 20
               282312799ul ,   // 21
               573292817ul ,   // 22
              1164186217ul ,   // 23
              2364114217ul ,   // 24
              4294967291ul ,   // 25
#if ALIB_SIZEOF_INTEGER == 8
              8589934583ull,   // 26
             17179869143ull,   // 27
             34359738337ull,   // 28
             68719476731ull,   // 29
            137438953447ull,   // 30
            274877906899ull,   // 31
            549755813881ull,   // 32
           1099511627689ull,   // 33
           2199023255531ull,   // 34
           4398046511093ull,   // 35
           8796093022151ull,   // 36
          17592186044399ull,   // 37
          35184372088777ull,   // 38
          70368744177643ull,   // 39
         140737488355213ull,   // 40
         281474976710597ull,   // 41
         562949953421231ull,   // 42
        1125899906842597ull,   // 43
        2251799813685119ull,   // 44
        4503599627370449ull,   // 45
        9007199254740881ull,   // 46
       18014398509481951ull,   // 47
       36028797018963913ull,   // 48
       72057594037927931ull,   // 49
      144115188075855859ull,   // 50
      288230376151711717ull,   // 51
      576460752303423433ull,   // 52
     1152921504606846883ull,   // 53
     2305843009213693951ull,   // 54
     4611686018427387847ull,   // 55
     9223372036854775783ull,   // 56
    18446744073709551557ull,   // 57
#endif
};

void* DUMMY_BUCKET= nullptr;

#endif


} // namespace alib::containers[::detail]
    
#if ALIB_DEBUG
    ALIB_API extern uinteger  DBG_STATS_STRINGTREE_NAMES;
    ALIB_API extern uinteger  DBG_STATS_STRINGTREE_NAME_OVERFLOWS;
    uinteger  DBG_STATS_STRINGTREE_NAMES        = 0;
    uinteger  DBG_STATS_STRINGTREE_NAME_OVERFLOWS= 0;
#endif

} // namespace [alib::containers]

