// #################################################################################################
//  AWorx ALib Unit Tests
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_SINGLETONS

#include "alib/alox.hpp"
#include "alib/singletons/singleton.hpp"

#include <iostream>

namespace dox_lang_singleton {

//! [DOX_ALIB_SINGLETON_1]
// Derive a class from singleton, providing its name as template parameter:
class MyClass : public alib::Singleton<MyClass>
{
    //... MyClass implementation
};
//! [DOX_ALIB_SINGLETON_1]

//! [DOX_ALIB_SINGLETON_STRICT_1]
// Derive a class from singleton, providing its name as template parameter:
class JustOne : public alib::Singleton<JustOne>
{
    friend alib::Singleton<JustOne>;
    private: JustOne() {}

    //... class JustOne implementation
};
//! [DOX_ALIB_SINGLETON_STRICT_1]

} // namespace dox_lang_singleton

namespace dox_lang_singleton {



extern void test(); // prototype, needed to avoid (strict) clang warnings
void test()
{
//! [DOX_ALIB_SINGLETON_2]
// Then, the singleton can be received as:
MyClass& myClassSingleton=  MyClass::GetSingleton();
std::cout << "The singleton of MyClass is: " << std::hex  << &myClassSingleton << std::endl;
//! [DOX_ALIB_SINGLETON_2]

//! [DOX_ALIB_SINGLETON_3]
MyClass instance2;
std::cout << "Another instance of MyClass is: " << std::hex  << &instance2     << std::endl;
//! [DOX_ALIB_SINGLETON_3]

#if defined(ALIB_DOX)
//! [DOX_ALIB_SINGLETON_STRICT_2]
JustOne& theOne= JustOne::GetSingleton();
JustOne  theSecond; // Compiler error, not allowed!
//! [DOX_ALIB_SINGLETON_STRICT_2]
#else
JustOne& theOne= JustOne::GetSingleton();
(void) theOne;
#endif

    (void) theOne;

#if !ALIB_FEAT_SINGLETON_MAPPED
    delete &myClassSingleton;
    delete &theOne;
#endif
}

} //namespace dox_lang_singleton


#endif // ALIB_UT_DOCS && ALIB_SINGLETONS
