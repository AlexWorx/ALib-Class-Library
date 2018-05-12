// #################################################################################################
//  aworx - Unit Tests
//  Private, not published in git ( I hope! )
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"

#include <iostream>

namespace dox_lang_singleton {

//! [DOX_ALIB_LANG_SINGLETON_1]
// Derive a class from singleton, providing its name as template parameter:
class MyClass : public aworx::Singleton<MyClass>
{
    //... MyClass implementation
};
//! [DOX_ALIB_LANG_SINGLETON_1]

//! [DOX_ALIB_LANG_SINGLETON_STRICT_1]
// Derive a class from singleton, providing its name as template parameter:
class JustOne : public aworx::Singleton<JustOne>
{
    friend aworx::Singleton<JustOne>;
    private: JustOne() {}

    //... class JustOne implementation
};
//! [DOX_ALIB_LANG_SINGLETON_STRICT_1]

} // namespace dox_lang_singleton

//! [DOX_ALIB_LANG_SINGLETON_DEBUG_LIST_1]
#if ALIB_DEBUG && ALIB_FEAT_SINGLETON_MAPPED

    // import the map of singletons. This is not done in the header files!
    namespace aworx { namespace lib { namespace lang {
        extern ALIB_API TypeMap<void*>  singletonMap;
    }}}
//! [DOX_ALIB_LANG_SINGLETON_DEBUG_LIST_1]

#endif
namespace dox_lang_singleton {
#if ALIB_DEBUG && ALIB_FEAT_SINGLETON_MAPPED


    extern void  DumpSingletons();

//! [DOX_ALIB_LANG_SINGLETON_DEBUG_LIST_2]
    void  DumpSingletons()
    {
        std::cout << "Debug-Mode: Dumping Singletons: " << std::endl;

        for( auto& it : aworx::lib::lang::singletonMap )
            std::cout << "  "
                 << aworx::lib::debug::TypeDemangler(it.first.get()).Get()
                 << " = 0x" << std::hex
                 << reinterpret_cast<uint64_t>(it.second)
                 << std::endl;
    }
#endif
//! [DOX_ALIB_LANG_SINGLETON_DEBUG_LIST_2]


extern void test(); // prototype, needed to avoid (strict) clang warnings
void test()
{
//! [DOX_ALIB_LANG_SINGLETON_2]
// Then, the singleton can be received as:
MyClass* myClassSingleton= MyClass::GetSingleton();
std::cout << "The singleton of MyClass is: " << std::hex  << myClassSingleton << std::endl;
//! [DOX_ALIB_LANG_SINGLETON_2]

//! [DOX_ALIB_LANG_SINGLETON_3]
MyClass instance2;
std::cout << "Another instance of MyClass is: " << std::hex  << &instance2     << std::endl;
//! [DOX_ALIB_LANG_SINGLETON_3]

#if defined(DOX_PARSER)
//! [DOX_ALIB_LANG_SINGLETON_STRICT_2]
JustOne *theOne= JustOne::GetSingleton();
JustOne theSecond; // Compiler error, not allowed!
//! [DOX_ALIB_LANG_SINGLETON_STRICT_2]
#else
JustOne *theOne= JustOne::GetSingleton();
(void) theOne;
#if ALIB_DEBUG && ALIB_FEAT_SINGLETON_MAPPED
DumpSingletons();
#endif
#endif

    (void) theOne;

#if !ALIB_FEAT_SINGLETON_MAPPED
    delete myClassSingleton;
    delete theOne;
#endif
}

} //namespace dox_lang_singleton






