// #################################################################################################
//  ALib C++ Library
//  Singleton Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "ALib.Singletons.H"
#include "ALib.Bootstrap.H"

#include <iostream>


// Derive a class from singleton, providing its name as the template parameter:
class MyClass : public alib::Singleton<MyClass>
{
    //... MyClass implementation
};

// Sample for a strict singleton: only one instance may exist
class JustOne : public alib::Singleton<JustOne>
{
    friend alib::Singleton<JustOne>;
    private: JustOne() {}

    //... class JustOne implementation
};


#if ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG

// A simple debug dump function
extern
void  DumpSingletons();
void  DumpSingletons()
{
    std::cout << std::endl
              << "Debug-compilation and mapped mode: Dumping Singletons: " << std::endl;

    for( auto& mapPair : alib::singletons::DbgGetSingletons() )
        std::cout << "  "
             << mapPair.first->name()
             << " = 0x" << std::hex
             << reinterpret_cast<uint64_t>(mapPair.second)
             << std::endl;
}
#endif

int main()
{
    alib::Bootstrap();

    MyClass& myClassSingleton= MyClass::GetSingleton();
    std::cout << "The singleton of MyClass is: " << std::hex  << &myClassSingleton << std::endl;

    // It is still allowed to create other instances of MyClass (non-strict implementation)
    // but this will not appear in the singleton list below and is not considered a singleton.
    MyClass instance2;
    std::cout << "Another instance of MyClass is: " << std::hex  << &instance2     << std::endl;

    JustOne&theOne= JustOne::GetSingleton();
    (void) theOne;
    // We can't create a second one, constructor is private


    // The dump function is only available if symbol ALIB_FEAT_SINGLETON_MAPPED is true.
    // On GNU/Linux and mac this defaults to false. (On Windows OS to true.)
    // The sample's cmake file however overwrites the default value for demonstration purpose!
    #if ALIB_DEBUG && ALIB_FEAT_SINGLETON_MAPPED
        DumpSingletons();
    #endif


    alib::Shutdown();
    return 0;
}
