// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALIB_MONOMEM_HASHTABLE)
#   include "alib/monomem/hashtable.hpp"
#endif
#endif // !defined(ALIB_DOX)


namespace aworx { namespace lib { namespace monomem { namespace detail {

#if !defined(ALIB_DOX)

const uinteger primeNumbers[] =
{
                      11ul ,   // 0
                      23ul ,   // 1
                      47ul ,   // 2
                      97ul ,   // 3
                     199ul ,   // 4
                     409ul ,   // 5
                     823ul ,   // 6
                    1741ul ,   // 7
                    3469ul ,   // 8
                    6949ul ,   // 9
                   14033ul ,   // 10
                   28411ul ,   // 11
                   57557ul ,   // 12
                  116731ul ,   // 13
                  236897ul ,   // 14
                  480881ul ,   // 15
                  976369ul ,   // 16
                 1982627ul ,   // 17
                 4026031ul ,   // 18
                 8175383ul ,   // 19
                16601593ul ,   // 20
                33712729ul ,   // 21
                68460391ul ,   // 22
               139022417ul ,   // 23
               282312799ul ,   // 24
               573292817ul ,   // 25
              1164186217ul ,   // 26
              2364114217ul ,   // 27
              4294967291ul ,   // 28
#if ALIB_SIZEOF_INTEGER == 8
              8589934583ull,   // 29
             17179869143ull,   // 30
             34359738337ull,   // 31
             68719476731ull,   // 32
            137438953447ull,   // 33
            274877906899ull,   // 34
            549755813881ull,   // 35
           1099511627689ull,   // 36
           2199023255531ull,   // 37
           4398046511093ull,   // 38
           8796093022151ull,   // 39
          17592186044399ull,   // 40
          35184372088777ull,   // 41
          70368744177643ull,   // 42
         140737488355213ull,   // 43
         281474976710597ull,   // 44
         562949953421231ull,   // 45
        1125899906842597ull,   // 46
        2251799813685119ull,   // 47
        4503599627370449ull,   // 48
        9007199254740881ull,   // 49
       18014398509481951ull,   // 50
       36028797018963913ull,   // 51
       72057594037927931ull,   // 52
      144115188075855859ull,   // 53
      288230376151711717ull,   // 54
      576460752303423433ull,   // 55
     1152921504606846883ull,   // 56
     2305843009213693951ull,   // 57
     4611686018427387847ull,   // 58
     9223372036854775783ull,   // 59
    18446744073709551557ull,   // 60
#endif
};

void* dummyBucket= nullptr;

#endif

}}}}// namespace [aworx::lib::monomem::detail]

