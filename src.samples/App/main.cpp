// #################################################################################################
//  ALib C++ Framework
//  App Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
/// \file
#include "ALib.App.H"
#if !DOXYGEN // otherwise this sample would be seen in the ALib dox

#include "sample.hpp"


//#define TEST_MINIMAL_APP
#define TEST_MINIMAL_APP_WITH_CUSTOM_METHOD

//------ A minimal pure App used with dox. Should be tested after changes ------
#if defined(TEST_MINIMAL_APP)
DOX_MARKER( [DOX_APP_MINIMAL_APP])
struct MyApp :  alib::app::App {

    void onRun()  override    { cOut->Add("Hello ALib App!"); };
};

int main( int argc, const char** argv) {
    return MyApp().Main(argc, argv);
}
DOX_MARKER( [DOX_APP_MINIMAL_APP])

//------- A minimal pure App with custom method. Should be tested after changes --------

#elif defined(TEST_MINIMAL_APP_WITH_CUSTOM_METHOD)
DOX_MARKER( [DOX_APP_MINIMAL_APP_WITH_CUSTOM_METHOD])
// My derived app
struct MyApp : public alib::app::App {

    // Define an own states enumeration with one entry. The value of the state element
    // defines the execution order. Here, we use RunStart as the reference. Thus the state
    // will be executed after RunStart and before Run.
    enum class MyStates { WarmUp = int(States::RunStart) + 1 };

    // Constructor: Inserts the custom state
    MyApp() {
        machine.Program.Add<MyApp, &MyApp::WarmUpStep>(( MyStates::WarmUp )  );
    }

    // The method associated with the custom state
    virtual void WarmUpStep() {
        cOut->Add("I'm still warming up");
    }

    // Overriding the original run method
    void onRun()        override {
        cOut->Add("Hello ALib App!");
    }
};

// C++ main() calls App::Main and returns its exit code
int main( int argc, const char** argv) {
    return MyApp().Main(argc, argv);
}
DOX_MARKER( [DOX_APP_MINIMAL_APP_WITH_CUSTOM_METHOD])


///---------------- The real sample ----------------
#else
int main( int argc, const char** argv) {

    return Sample().Main(argc, argv);
}

#endif //TEST_MINIMAL_APP


#endif // !DOXYGEN

