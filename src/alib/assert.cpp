// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/strings/strings.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#if ALIB_DEBUG
#   include <cstdlib>
#   include "alib/platformincludes.hpp"
#   include <cassert>
#   include <cstring>
#   include <iostream>
#   include <iomanip>
#   include <string>
#   include <string>
#   include <stdexcept>
#   include <any>
#   if __has_include(<format>)
#      include <format>
#   else
#      include <fmt/format.h>
#   endif
#   include <codecvt>

#   if (ALIB_SINGLE_THREADED && ALIB_EXT_LIB_THREADS_AVAILABLE)
#       include <thread>
#   endif
#   if ALIB_DEBUG_ASSERTION_PRINTABLES
#       include <unordered_set>
#   endif

#   include <iostream>
#   include <iomanip>
#   include <typeindex>
#   include <functional>
#endif

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module   ALib.Lang;
#  if ALIB_DEBUG
        import   ALib.Threads;
        import   ALib.Bootstrap;
#      if ALIB_STRINGS
        import   ALib.Strings;
        import   ALib.Strings.Token;
#         if ALIB_MONOMEM
           import   ALib.Strings.Monomem;
#         endif
#      endif
#      if ALIB_BOXING
        import   ALib.Boxing;
#      endif
#       if ALIB_ENUMRECORDS
        import   ALib.EnumRecords;
#       endif
#       if ALIB_RESOURCES
        import   ALib.Resources;
#       endif
#       if ALIB_VARIABLES
        import   ALib.Variables;
#       endif
#       if ALIB_SYSTEM
        import   ALib.System;
#       endif
#       if ALIB_THREADMODEL
        import   ALib.ThreadModel;
#       endif
#       if ALIB_CAMP
        import   ALib.Camp.Base;
#       endif
#   endif
#else
#   include    "ALib.Lang.H"
#   if ALIB_DEBUG
#      include "ALib.Bootstrap.H"
#      include "ALib.Threads.H"
#      include "ALib.Strings.H"
#      include "ALib.Strings.Token.H"
#      include "ALib.Strings.Monomem.H"
#      include "ALib.Boxing.H"
#      include "ALib.EnumRecords.H"
#      include "ALib.Resources.H"
#      include "ALib.Variables.H"
#      include "ALib.System.H"
#      include "ALib.ThreadModel.H"
#      include "ALib.Camp.Base.H"
#   endif
#endif

#if ALIB_DEBUG
// ======================================   Implementation   =======================================
#include "ALib.Lang.CIFunctions.H"

namespace alib::assert {

// #################################################################################################
// Debug functions
// #################################################################################################
#if (ALIB_SINGLE_THREADED && ALIB_EXT_LIB_THREADS_AVAILABLE) || DOXYGEN

#if !DOXYGEN
    namespace  { std::thread::id  dbg_thread_seen;
                 bool             dbg_in_single_threaded_check= false; }
#endif


//==============================================================================================
/// This method stores the first thread that invokes it, and if in the future the method is
/// visited by a different thread, it asserts.
///
/// In release compilations, this function is inlined and empty, and therefore it is not
/// necessary to remove usages with preprocessor macro \ref ALIB_DBG or similar.
///
/// In debug-compilations, this is not empty if:
/// 1. Compiler symbol \ref ALIB_SINGLE_THREADED is set (what disables thread-safeness
///    throughout the library), and
/// 2. Compiler symbol \ref ALIB_EXT_LIB_THREADS_AVAILABLE was passed on library compilation,
///    which allows using the C++ standard library's threading types without causing linker
///    failures.
///
/// If given, this function is, for example, called by macros \ref ALIB_LOCK or
/// \ref ALIB_DCS, which otherwise are defined to do what they are supposed to do.
/// This exclamatory approach was made with \alib to motivate to
/// write \ref alib_threads_intro_agnostic "threading-agnostic software".
///
/// Besides several macros, some other prominent \alib entities, like \alib{lox::Lox},
/// \alib{format;Formatter} or \alib{monomem;TMonoAllocator} invoke this method with their
/// acquisition.
//==============================================================================================
void SingleThreaded()
{
    if( dbg_in_single_threaded_check ) // this would happen when the assertion below is raised
        return;
    dbg_in_single_threaded_check= true;

    // first time?
    if( lang::IsNull(dbg_thread_seen) )
    {
        dbg_thread_seen= std::this_thread::get_id();
        dbg_in_single_threaded_check= false;
        return;
    }

    if( dbg_thread_seen != std::this_thread::get_id() )
    {
        ALIB_ERROR(  "THREADS",
                     "A second thread was detected using a single-threaded compilation of ALib"
                     "(Symbol 'ALIB_SINGLE_THREADED' given with the ALib Build)."     )
    }
    dbg_in_single_threaded_check= false;
}
#endif  // ALIB_SINGLE_THREADED && ALIB_EXT_LIB_THREADS_AVAILABLE

} // namespace [alib]

// #################################################################################################
// Assert functions
// #################################################################################################
namespace alib::assert {

void (*PLUGIN)( const CallerInfo& ci, int type, std::string_view domain, std::string_view msg )     = nullptr;

namespace {  }
std::string_view                   FORMAT           = "{file}:{line} {type}:\n{message}";
std::ostream*                      STREAM_ERRORS    = &std::cerr;
std::ostream*                      STREAM_WARNINGS  = &std::cout;
std::ostream*                      STREAM_MESSAGES  = &std::cout;

#if !DOXYGEN
namespace {
    thread_local TLD HALT_FLAGS_AND_COUNTERS;
    bool           isInitialized= false;
    std::string    outBuffer;
#if !ALIB_SINGLE_THREADED
    RecursiveLock  lock;
#endif


static std::unordered_map<std::type_index, AnyConversionFunc> registeredAnys;

#if __has_include(<format>)
    namespace f=std;
#else
    namespace f=fmt;
#endif

void initializeDefaultPrintables() {
    isInitialized= true;
    RegisterPrintable(typeid( bool        ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast< bool        >(any)); });
    RegisterPrintable(typeid( char        ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast< char        >(any)); });
    RegisterPrintable(typeid( int8_t      ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast< int8_t      >(any)); });
    RegisterPrintable(typeid(uint8_t      ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast<uint8_t      >(any)); });
    RegisterPrintable(typeid( int16_t     ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast< int16_t     >(any)); });
    RegisterPrintable(typeid(uint16_t     ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast<uint16_t     >(any)); });
    RegisterPrintable(typeid( int32_t     ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast< int32_t     >(any)); });
    RegisterPrintable(typeid(uint32_t     ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast<uint32_t     >(any)); });
    RegisterPrintable(typeid( int64_t     ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast< int64_t     >(any)); });
    RegisterPrintable(typeid(uint64_t     ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast<uint64_t     >(any)); });
    RegisterPrintable(typeid(float        ), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast<float        >(any)); });
    RegisterPrintable(typeid(unsigned long), [](const std::any& any, std::string& s) { s+= f::format("{}", std::any_cast<unsigned long>(any)); });
DOX_MARKER( [DOX_ASSERT_REGISTER_PRINTABLE])
RegisterPrintable(typeid(double),
    [](const std::any& any, std::string& s) {
        s+= f::format("{}", std::any_cast<double     >(any));
    });
RegisterPrintable(typeid(const char*),
    [](const std::any& any, std::string& s) {
        auto* value= std::any_cast<const char*>(any);
        if (value)
            s+= value;
    });
RegisterPrintable(typeid(const char8_t*),
    [](const std::any& any, std::string& s) {
        auto* value= std::any_cast<const char*>(any);
        if (value)
            s+= value;
    });
RegisterPrintable(typeid(const wchar_t*),
    [](const std::any& any, std::string& s) {
        auto* value= std::any_cast<const wchar_t*>(any);
        if (value)
        {
          ALIB_WARNINGS_IGNORE_DEPRECATED
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            s+= converter.to_bytes(value);
          ALIB_WARNINGS_RESTORE
        }
    });
DOX_MARKER( [DOX_ASSERT_REGISTER_PRINTABLE])
        
        if constexpr (sizeof(wchar_t) == 2)
            RegisterPrintable(typeid(const char32_t*),
                [](const std::any& any, std::string& s) {
                    auto* value= std::any_cast<const char32_t*>(any);
                    if (value)
                    {
                      ALIB_WARNINGS_IGNORE_DEPRECATED
                        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                        s+= converter.to_bytes(value);
                      ALIB_WARNINGS_RESTORE
                    }
                });
        if constexpr (sizeof(wchar_t) == 4)
            RegisterPrintable(typeid(const char16_t*),
                [](const std::any& any, std::string& s) {
                    auto* value= std::any_cast<const char16_t*>(any);
                    if (value)
                    {
                      ALIB_WARNINGS_IGNORE_DEPRECATED
                        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
                        s+= converter.to_bytes(value);
                      ALIB_WARNINGS_RESTORE
                    }
                });

        RegisterPrintable(typeid(std::string          ), [](const std::any& any, std::string& s) { s+= *std::any_cast<std::string    >(&any); });
        RegisterPrintable(typeid(std::string_view     ), [](const std::any& any, std::string& s) { s+= std::any_cast<std::string_view>( any); });
        RegisterPrintable(typeid(const std::type_info*), [](const std::any& any, std::string& s) {
            auto* typeInfo= std::any_cast<const std::type_info*>(any);
            if (typeInfo)
                s+= lang::DbgTypeDemangler(*typeInfo).Get();
        });
        RegisterPrintable(typeid(std::thread::id)      , [](const std::any& any, std::string& s) {
            #if !ALIB_SINGLE_THREADED
                auto threadID= std::any_cast<std::thread::id>(any);
                if ( !lang::IsNull(threadID)) {
                    Thread* thread= Thread::Get(threadID);
                    if (thread) {
                        #if !ALIB_CHARACTERS_WIDE
                            s+= thread->GetName();
                        #else
                            std::wstring_convert<std::codecvt_utf8<character>, character> converter;
                            s+= converter.to_bytes(thread->GetName());
                        #endif
                        s += f::format("({})", thread->GetID());
                    }
                    else
                        s+= "<Unknown thread>";
                }
            #else
                (void) any;
                s+= "<SINGLE_THREADED>";
            #endif
        });

        RegisterPrintable(typeid(CallerInfo), [](const std::any& any, std::string& s) {
            CallerInfo callerInfo= std::any_cast<CallerInfo>(any);
            if ( callerInfo.File == nullptr ) {
                s+= "<nulled caller>";
                return;
            }
            s+=f::format("{{Caller: @ {}:{} ({}) ", callerInfo.File,callerInfo.Line, callerInfo.Func);
            if ( callerInfo.TypeInfo ) {
                s+= "<"; s+= lang::DbgTypeDemangler(*callerInfo.TypeInfo).Get();s+= "> ";
            }
            #if !ALIB_SINGLE_THREADED
                s+=f::format("thread::id= {}", 5 );
            #endif
            s+= '}';
        });

        RegisterPrintable(typeid(Thread*)      , [](const std::any& any, std::string& s) {
            #if !ALIB_SINGLE_THREADED
                Thread* thread= std::any_cast<Thread*>(any);
                    if (thread) {
                        #if !ALIB_CHARACTERS_WIDE
                            s+= thread->GetName();
                        #else
                            std::wstring_convert<std::codecvt_utf8<character>, character> converter;
                            s+= converter.to_bytes(thread->GetName());
                        #endif
                        s += f::format("({})", thread->GetID());
                    }
                    else
                        s+= "<Unknown thread>";
            #else
                (void) any;
                s+= "<SINGLE_THREADED>";
            #endif
        });

        #if !ALIB_SINGLE_THREADED
        RegisterPrintable(typeid(Thread::State)      , [](const std::any& any, std::string& s) {
            auto state= std::any_cast<Thread::State>(any);
                 if (state==Thread::State::Unstarted ) s+= "Unstarted";
            else if (state==Thread::State::Started   ) s+= "Started";
            else if (state==Thread::State::Running   ) s+= "Running";
            else if (state==Thread::State::Done      ) s+= "Done";
            else if (state==Thread::State::Terminated) s+= "Terminated";
            else         s+= "<Unknown thread state>";
        });
        #endif

        ALIB_WARNINGS_IGNORE_DEPRECATED
        #if ALIB_STRINGS
        RegisterPrintable(typeid(NString ), [](const std::any& any, std::string& s) { s+= std::string_view( std::any_cast<NString >(any).Buffer(), size_t(std::any_cast<NString >(any).Length()) ); });
        RegisterPrintable(typeid(NAString), [](const std::any& any, std::string& s) { s+= std::string_view( std::any_cast<NAString>(any).Buffer(), size_t(std::any_cast<NAString>(any).Length()) ); });
        RegisterPrintable(typeid(NCString), [](const std::any& any, std::string& s) { s+= std::string_view( std::any_cast<NCString>(any).Buffer(), size_t(std::any_cast<NCString>(any).Length()) ); });
        RegisterPrintable(typeid(WString ), [](const std::any& any, std::string& s) { std::wstring_convert<std::codecvt_utf8<wchar>, wchar> converter; auto src=std::any_cast<WString >(any); s+= converter.to_bytes(src.Buffer(), src.Buffer() + src.Length() ); });
        RegisterPrintable(typeid(WAString), [](const std::any& any, std::string& s) { std::wstring_convert<std::codecvt_utf8<wchar>, wchar> converter; auto src=std::any_cast<WAString>(any); s+= converter.to_bytes(src.Buffer(), src.Buffer() + src.Length() ); });
        RegisterPrintable(typeid(WCString), [](const std::any& any, std::string& s) { std::wstring_convert<std::codecvt_utf8<wchar>, wchar> converter; auto src=std::any_cast<WCString>(any); s+= converter.to_bytes(src.Buffer(), src.Buffer() + src.Length() ); });
        RegisterPrintable(typeid(XString ), [](const std::any& any, std::string& s) { std::wstring_convert<std::codecvt_utf8<xchar>, xchar> converter; auto src=std::any_cast<XString >(any); s+= converter.to_bytes(src.Buffer(), src.Buffer() + src.Length() ); });
        RegisterPrintable(typeid(XAString), [](const std::any& any, std::string& s) { std::wstring_convert<std::codecvt_utf8<xchar>, xchar> converter; auto src=std::any_cast<XAString>(any); s+= converter.to_bytes(src.Buffer(), src.Buffer() + src.Length() ); });
        RegisterPrintable(typeid(XCString), [](const std::any& any, std::string& s) { std::wstring_convert<std::codecvt_utf8<xchar>, xchar> converter; auto src=std::any_cast<XCString>(any); s+= converter.to_bytes(src.Buffer(), src.Buffer() + src.Length() ); });
        RegisterPrintable(typeid(Token   ), [](const std::any& any, std::string& s) { s+= NString256(String256(std::any_cast<Token>(any))); });
        #if ALIB_MONOMEM
        RegisterPrintable(typeid(NAStringMA), [](const std::any& any, std::string& s) { s+= std::string_view( std::any_cast<NAStringMA>(any).Buffer(), size_t(std::any_cast<NAStringMA>(any).Length()) ); });
        RegisterPrintable(typeid(WAStringMA), [](const std::any& any, std::string& s) { std::wstring_convert< std::codecvt_utf8<wchar>, wchar> converter; auto src=std::any_cast<WAStringMA>(any); s+= converter.to_bytes(src.Buffer(), src.Buffer() + src.Length() ); });
        RegisterPrintable(typeid(NAStringPA), [](const std::any& any, std::string& s) { s+= std::string_view( std::any_cast<NAStringPA>(any).Buffer(), size_t(std::any_cast<NAStringPA>(any).Length()) ); });
        RegisterPrintable(typeid(WAStringPA), [](const std::any& any, std::string& s) { std::wstring_convert< std::codecvt_utf8<wchar>, wchar> converter; auto src=std::any_cast<WAStringPA>(any); s+= converter.to_bytes(src.Buffer(), src.Buffer() + src.Length() ); });
        #endif
        #endif
        ALIB_WARNINGS_RESTORE

        #if ALIB_BOXING && ALIB_ENUMRECORDS
        RegisterPrintable(typeid(Box ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<Box >(any)); });
        RegisterPrintable(typeid(Enum), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<Enum>(any)); });
        #endif

        // CodeMarker_CommonEnums
        #if ALIB_ENUMRECORDS
        RegisterPrintable(typeid(lang::Alignment        ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Alignment        >(any)); });
        RegisterPrintable(typeid(lang::Bool             ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Bool             >(any)); });
        RegisterPrintable(typeid(lang::Caching          ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Caching          >(any)); });
        RegisterPrintable(typeid(lang::Case             ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Case             >(any)); });
        RegisterPrintable(typeid(lang::ContainerOp      ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::ContainerOp      >(any)); });
        RegisterPrintable(typeid(lang::CreateDefaults   ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::CreateDefaults   >(any)); });
        RegisterPrintable(typeid(lang::CreateIfNotExists), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::CreateIfNotExists>(any)); });
        RegisterPrintable(typeid(lang::CurrentData      ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::CurrentData      >(any)); });
        RegisterPrintable(typeid(lang::Inclusion        ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Inclusion        >(any)); });
        RegisterPrintable(typeid(lang::Initialization   ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Initialization   >(any)); });
        RegisterPrintable(typeid(lang::Phase            ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Phase            >(any)); });
        RegisterPrintable(typeid(lang::Propagation      ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Propagation      >(any)); });
        RegisterPrintable(typeid(lang::Reach            ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Reach            >(any)); });
        RegisterPrintable(typeid(lang::Recursive        ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Recursive        >(any)); });
        RegisterPrintable(typeid(lang::Responsibility   ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Responsibility   >(any)); });
        RegisterPrintable(typeid(lang::Safeness         ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Safeness         >(any)); });
        RegisterPrintable(typeid(lang::Side             ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Side             >(any)); });
        RegisterPrintable(typeid(lang::SortOrder        ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::SortOrder        >(any)); });
        RegisterPrintable(typeid(lang::SourceData       ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::SourceData       >(any)); });
        RegisterPrintable(typeid(lang::Switch           ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Switch           >(any)); });
        RegisterPrintable(typeid(lang::Timezone         ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Timezone         >(any)); });
        RegisterPrintable(typeid(lang::Timing           ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Timing           >(any)); });
        RegisterPrintable(typeid(lang::ValueReference   ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::ValueReference   >(any)); });
        RegisterPrintable(typeid(lang::Whitespaces      ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<lang::Whitespaces      >(any)); });
        #endif

        #if ALIB_VARIABLES
        RegisterPrintable(typeid(const variables::Variable*), [](const std::any& any, std::string& s) {
            auto* o= std::any_cast<const variables::Variable*>(any);
            String256 serialize;
            o->Name(serialize);
            ALIB_STRINGS_TO_NARROW(serialize, ns, 256)
            s+= ns;
        });
        RegisterPrintable(typeid(variables::Variable*), [](const std::any& any, std::string& s) {
            auto* o= std::any_cast<variables::Variable*>(any);
            String256 serialize;
            o->Name(serialize);
            ALIB_STRINGS_TO_NARROW(serialize, ns, 256)
            s+= ns;
        });
        RegisterPrintable(typeid(variables::Variable), [](const std::any& any, std::string& s) {
            auto o= std::any_cast<variables::Variable>(any);
            String256 serialize;
            o.Name(serialize);
            ALIB_STRINGS_TO_NARROW(serialize, ns, 256)
            s+= ns;
        });
        RegisterPrintable(typeid(variables::Priority), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<variables::Priority>(any)); });
        #endif
        #if ALIB_SYSTEM
        RegisterPrintable(typeid(system::Path        ), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<system::Path        >(any)); });
        #  if ALIB_CAMP
            RegisterPrintable(typeid(system::SystemErrors), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<system::SystemErrors>(any)); });
        #  endif
        #endif
        #if ALIB_THREADMODEL && ALIB_ENUMRECORDS
        RegisterPrintable(typeid(threadmodel::Priority), [](const std::any& any, std::string& s) {  s+= NString256(std::any_cast<threadmodel::Priority>(any)); });
        #endif

    }

    void printRegisteredAny(const std::any& any, const CallerInfo& ci) {
        if (!any.has_value())
            return;
        auto it = registeredAnys.find(std::type_index(any.type()));
        if (it != registeredAnys.end()) {
            // Call the associated function
            it->second(any, outBuffer);
        } else {
            std::cerr << "Internal Error using alib::assert::Assert(): No converter registered for type: <"
                      << lang::DbgTypeDemangler( any.type() ).Get()
                      << '>' << std::endl
                      << "at " << ci.File << ':' << ci.Line << std::endl;
            assert(false);
        }
    }

    const char* resolveMessageType(int msgType) {
        if (msgType == 0) return "error";
        if (msgType == 1) return "warning";
                          return "message";
    }

    void writeMessage( std::ostream& os, int msgType, const std::string_view& domain,
                        const char* file, int line,
                        const std::string_view& message                   ) {
        // Start with the global format string
        std::string_view format = FORMAT;

        // Loop and process the format string
        size_t start = 0;
        while (start < format.size()) {
            // Find the next placeholder starting from 'start'
            size_t openBrace = format.find('{', start);
            if (openBrace == std::string_view::npos) {
                // No more placeholders, print the remaining part
                os << format.substr(start);
                break;
            }

            // Print everything before the '{'
            os << format.substr(start, openBrace - start);

            // Find the closing '}'
            size_t closeBrace = format.find('}', openBrace + 1);
            if (closeBrace == std::string_view::npos) {
                // Invalid placeholder syntax (no closing '}'), just print raw text
                os << format.substr(openBrace);
                break;
            }

            // Extract the placeholder name
            std::string_view placeholder = format.substr(openBrace + 1, closeBrace - openBrace - 1);

            // Substitute the placeholder with the corresponding value
                 if ( placeholder == "type"    ) os << resolveMessageType(msgType);
            else if ( placeholder == "file"    ) os << file;
            else if ( placeholder == "line"    ) os << line;
            else if ( placeholder == "message" ) os << message;
            else if ( placeholder == "domain"  ) os << domain;
            else       os << "{" << placeholder << "}"; // unknown placeholder

            // Move past the closing '}'
            start = closeBrace + 1;
        }
        os << std::endl;
    } // writeMessage

    class RecursionBlocker {
    private:
        static inline std::atomic<bool> isRecursing{false};
        bool wasBlocked;

    public:
        RecursionBlocker() noexcept {
            wasBlocked = isRecursing.exchange(true);
        }

        ~RecursionBlocker() {
            if (!wasBlocked) {
                isRecursing = false;
            }
        }

        [[nodiscard]] bool blocked() const noexcept {
            return wasBlocked;
        }

        // Delete copy operations to ensure RAII semantics
        RecursionBlocker(const RecursionBlocker&) = delete;
        RecursionBlocker& operator=(const RecursionBlocker&) = delete;
    };

#if ALIB_DEBUG_ASSERTION_PRINTABLES
    struct Key { const char* str; // Pointer to the file name
                 int value;       // Line number
                bool operator==(const Key& other) const {
                        return str == other.str && value == other.value;
    }};

    struct KeyHash { std::size_t operator()(const Key& key) const {
                     return std::hash<const char*>()(key.str) ^ ( std::hash<int>()(key.value) << 1);
    }};

    std::unordered_set<Key, KeyHash> seenSourceLocations;
#endif // ALIB_DEBUG_ASSERTION_PRINTABLES

} // namespace [anonymous]

void RegisterPrintable(std::type_index typeIndex, AnyConversionFunc func) {
    registeredAnys[typeIndex] = func;
}

#if ALIB_DEBUG_ASSERTION_PRINTABLES
#   pragma message ("ALIB_DEBUG_ASSERTION_PRINTABLES set. ALib will print all assertions once, no matter if they are raised or not." )
    void CheckArgsImpl( const CallerInfo& ci, const std::span<std::any>& args ) {
        if ( !alib::NonCampModulesInitialized )     return;
        if( !isInitialized )                        initializeDefaultPrintables();

        if (!seenSourceLocations.insert(Key{ci.File, ci.Line}).second)
            return;


        for (size_t i = 0; i < args.size(); ++i) {
            auto it = registeredAnys.find(std::type_index(args[i].type()));
            if (it == registeredAnys.end()) {
                std::cerr << "Internal Error using alib::assert::Assert(): No converter registered for type: <"
                          << lang::DbgTypeDemangler( args[i].type() ).Get()
                          << '>'  << std::endl
                          << "    at: " << ci.File << ':' << ci.Line << std::endl;
                assert(false);
        }}

        raise(ci, -1, "ASSERTTEST", args );
    }
#endif

#endif // !DOXYGEN

TLD& GetHaltFlagAndCounters() { return HALT_FLAGS_AND_COUNTERS; }

void raise( const CallerInfo& ci, int type, std::string_view domain,
            const std::span<std::any>& args )  {

    // lock parallel assertions if necessary
    ALIB_LOCK_RECURSIVE_WITH(lock)
    RecursionBlocker blocker;
    if (blocker.blocked())
        return;
    
    // register printable types (once)
    if( !isInitialized )
        initializeDefaultPrintables();

    // assemble message
    outBuffer.clear();

    // With ALox replacing this method (the usual thing), the first string might auto-detected
    // as an ALox domain name. To keep this consistent, we do a similar effort here.
    {
        for( nchar c : domain ) {
            if (!    (    isdigit( c )
                       || ( c >= 'A' && c <= 'Z' )
                       || c == '-' || c == '_' || c == '/' || c == '.' ) )  {
                std::cerr << "Illegal alib::assert::Assert() domain given: " << domain << std::endl;
                assert(false);
    }}}

    size_t i = 0; // Index to track the current argument
    while (i < args.size()) {
        const std::any& arg = args[i];

        if (    arg.type() == typeid(const char*)
             || arg.type() == typeid(std::string)
             || arg.type() == typeid(std::string_view) ) {

            std::string_view str;

                 if (arg.type() == typeid(const char*)) str = std::string_view( std::any_cast<const char*     >( arg));
            else if (arg.type() == typeid(std::string)) str = std::string_view(*std::any_cast<std::string     >(&arg));
            else                                        str =                   std::any_cast<std::string_view>( arg) ;
            std::string_view origStr= str;

                                                     
            // Lambda function to parse and print the format string
            auto handle_format_string = [&]() {
                size_t pos = 0; // Position to find "{}"
                while ((pos = str.find("{}")) != std::string::npos) {
                    // Print the portion before the placeholder
                    outBuffer+= str.substr(0, pos);

                    // Move to the next argument for substitution
                    if (++i >= args.size())
                    {
                        std::cerr << "alib::assert: Not enough arguments for format placeholders!" << std::endl;
                        std::cerr << " Format string: <" << origStr << '>' << std::endl;
                        std::cerr << " @ : " << ci.File << ':' << ci.Line << std::endl;
                        assert(false);
                        return;
                    }

                    // Print the argument substituted in place of "{}"
                    const std::any& placeholderArg = args[i];
                    printRegisteredAny(placeholderArg, ci);

                    // Remove the processed portion of the string
                    str = str.substr(pos + 2);
                }

                // Print the remaining part of the string (after all `{}` are replaced)
                outBuffer+= str;

                // End the lambda function
            };

            // Process the format string with the lambda
            handle_format_string();
        } else
            printRegisteredAny(arg, ci);

        ++i;
    }

    // if plugin hook, use it.
    if( PLUGIN && type!= -1 ) // -1 = CheckArgs
        PLUGIN( ci, type, domain, outBuffer.c_str() );

    // Print the formatted message to the console.
    else {
        // check if io-streams is already locked by us. We can do this, because this is anyhow
        // debug-code. This way, we avoid recursive locking.
        // (If locked by somebody else, we do not care and still write to the ostream!)
        #if !ALIB_SINGLE_THREADED
        bool lockingIoStreams= !threads::STD_IOSTREAMS_LOCK.Dbg.IsOwnedByCurrentThread();
        if( lockingIoStreams )
            threads::STD_IOSTREAMS_LOCK.Acquire(ALIB_CALLER);
        #endif

        writeMessage(   type == 0 ? *STREAM_ERRORS
                      : type == 1 ? *STREAM_WARNINGS
                      :             *STREAM_MESSAGES,
                      type, domain, ci.File, ci.Line, outBuffer);
        #if !ALIB_SINGLE_THREADED
        if( lockingIoStreams )
            threads::STD_IOSTREAMS_LOCK.Release(ALIB_CALLER);
        #endif
    }

    // Halt?
    bool halt;
         if (type == 0) {HALT_FLAGS_AND_COUNTERS.CtdErrors++;   halt= HALT_FLAGS_AND_COUNTERS.HaltOnErrors; }
    else if (type == 1) {HALT_FLAGS_AND_COUNTERS.CtdWarnings++; halt= HALT_FLAGS_AND_COUNTERS.HaltOnWarnings; }
    else                {HALT_FLAGS_AND_COUNTERS.CtdMessages++; halt= false; }
    #if defined( _WIN32 )
        if( halt )
        {
            #if  ALIB_CAMP
            if ( BASECAMP.IsDebuggerPresent() )
                DebugBreak();
            else
            #endif
            assert( false );
        }
    #else
        #if defined(__GNUC__) || defined(__clang__)
            if (halt)  __builtin_trap();
        #elif defined ( _MSC_VER )
            if (halt)  __debugbreak();
        #else
            (void) halt; // for release compiles with ALIB_DEBUG set
            assert( !halt );
        #endif

    #endif
}
}// namespace [alib::assert]

#   include "ALib.Lang.CIMethods.H"
#endif // ALIB_DEBUG





