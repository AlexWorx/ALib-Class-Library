//==================================================================================================
/// \file
/// This header file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LOX_MACROS
#define HPP_ALIB_LOX_MACROS 1
#pragma once
#if !defined(HPP_ALIB_ALOX)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

// #################################################################################################
// Macros to access the Lox instance
// #################################################################################################
#if !defined(LOG_LOX)
#   define LOG_LOX       (*alib::lox::Log::Get())
#endif

// #################################################################################################
// Macros for setting the source code info before executing a log
// #################################################################################################
#ifdef  ALOX_DBG_LOG_CI
#   define LOG_CI      ALIB_CALLER
#else
  #if ALIB_EXT_LIB_THREADS_AVAILABLE
    #define LOG_CI          {nullptr,0,nullptr,::std::thread::id(), nullptr}
  #else
    #define LOG_CI          {nullptr,0,nullptr,nullptr}
  #endif
#endif

#ifdef  ALOX_REL_LOG_CI
#   define LOX_CI      ALIB_CALLER
#else
  #if ALIB_EXT_LIB_THREADS_AVAILABLE
    #define LOG_CI          {nullptr,0,nullptr,::std::thread::id(), nullptr}
  #else
    #define LOG_CI          {nullptr,0,nullptr,nullptr}
  #endif
#endif

#define LOG_ACQUIRE     { alib::lox::Lox& _log= LOG_LOX; _log.Acquire( LOG_CI );
#define LOG_RELEASE         _log.Release(); }
#define LOX_ACQUIRE     { alib::lox::Lox& _lox= LOX_LOX; _lox.Acquire( LOX_CI );
#define LOX_RELEASE     _lox.Release(); }


// #################################################################################################
// Debug logging macros
// #################################################################################################
#if ALOX_DBG_LOG
#   define Log_Prune( ... ) __VA_ARGS__
#else
#   define Log_Prune( ... )
#endif

#define Log_SetSourcePathTrimRule(...)          { Log_Prune( LOG_ACQUIRE _log.SetSourcePathTrimRule( __VA_ARGS__ );    LOG_RELEASE ) }
#define Log_ClearSourcePathTrimRules(...)       { Log_Prune( LOG_ACQUIRE _log.ClearSourcePathTrimRules( __VA_ARGS__ ); LOG_RELEASE ) }
#define Log_AddDebugLogger()                    { Log_Prune( LOG_ACQUIRE alib::Log::AddDebugLogger   ( &_log );        LOG_RELEASE ) }
#define Log_RemoveDebugLogger()                 { Log_Prune( LOG_ACQUIRE alib::Log::RemoveDebugLogger( &_log );        LOG_RELEASE ) }
#define Log_GetLogger( identifier, name )         Log_Prune( alib::lox::detail::Logger* identifier;                                  \
                                                             LOG_ACQUIRE  identifier= _log.GetLogger( name );          LOG_RELEASE )
#define Log_RemoveLogger( logger)               { Log_Prune( LOG_ACQUIRE _log.RemoveLogger ( logger );                 LOG_RELEASE ) }
#define Log_SetDomain(...)                      { Log_Prune( LOG_ACQUIRE _log.SetDomain    ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_RemoveThreadDomain(...)             { Log_Prune( LOG_ACQUIRE _log.RemoveThreadDomain( __VA_ARGS__ );       LOG_RELEASE ) }
#define Log_SetDomainSubstitutionRule(...)      { Log_Prune( LOG_ACQUIRE _log.SetDomainSubstitutionRule( __VA_ARGS__); LOG_RELEASE ) }
#define Log_SetVerbosity(...)                   { Log_Prune( LOG_ACQUIRE _log.SetVerbosity ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_SetStartTime( ... )                 { Log_Prune( LOG_ACQUIRE _log.SetStartTime ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_MapThreadName(...)                  { Log_Prune( LOG_ACQUIRE _log.MapThreadName( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_LogState(...)                       { Log_Prune( LOG_ACQUIRE _log.State        ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_Verbose(...)                        { Log_Prune( LOG_ACQUIRE _log.Verbose      ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_Info(...)                           { Log_Prune( LOG_ACQUIRE _log.Info         ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_Warning(...)                        { Log_Prune( LOG_ACQUIRE _log.Warning      ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_Error(...)                          { Log_Prune( LOG_ACQUIRE _log.Error        ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_Assert(...)                         { Log_Prune( LOG_ACQUIRE _log.Assert       ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_If(...)                             { Log_Prune( LOG_ACQUIRE _log.If           ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_Once(...)                           { Log_Prune( LOG_ACQUIRE _log.Once         ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_Entry(...)                          { Log_Prune( LOG_ACQUIRE _log.Entry        ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_SetPrefix(...)                      { Log_Prune( LOG_ACQUIRE _log.SetPrefix    ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_Store(...)                          { Log_Prune( LOG_ACQUIRE _log.Store        ( __VA_ARGS__ );            LOG_RELEASE ) }
#define Log_Retrieve(data,...)                    Log_Prune( Box data; LOG_ACQUIRE data= _log.Retrieve( __VA_ARGS__ ); LOG_RELEASE )
#define Log_IsActive(result, ...)                 Log_Prune( LOG_ACQUIRE result= _log.IsActive( __VA_ARGS__ ); LOG_RELEASE )
#define Log_Exception(...)                      { Log_Prune( alib::lox::LogTools::Exception( LOG_LOX, __VA_ARGS__ );   )             }



// #################################################################################################
// Release logging macros
// #################################################################################################
#if ALOX_REL_LOG
#    define Lox_Prune( ... ) __VA_ARGS__
#else
#    define Lox_Prune( ... )
#endif

#define Lox_SetSourcePathTrimRule(...)      { Lox_Prune( LOX_ACQUIRE _lox.SetSourcePathTrimRule( __VA_ARGS__ );    LOX_RELEASE )  }
#define Lox_ClearSourcePathTrimRules(...)   { Lox_Prune( LOX_ACQUIRE _lox.ClearSourcePathTrimRules( __VA_ARGS__ ); LOX_RELEASE )  }
#define Lox_GetLogger( identifier, name )     Lox_Prune( alib::lox::detail::Logger* identifier; LOX_ACQUIRE identifier= _lox.GetLogger ( name ); LOX_RELEASE )
#define Lox_RemoveLogger( logger )          { Lox_Prune( LOX_ACQUIRE _lox.RemoveLogger ( logger );                 LOX_RELEASE )  }
#define Lox_SetDomain(...)                  { Lox_Prune( LOX_ACQUIRE _lox.SetDomain    (__VA_ARGS__);              LOX_RELEASE )  }
#define Lox_RemoveThreadDomain(...)         { Lox_Prune( LOX_ACQUIRE _lox.RemoveThreadDomain( __VA_ARGS__ );       LOX_RELEASE )  }
#define Lox_SetDomainSubstitutionRule(...)  { Lox_Prune( LOX_ACQUIRE _lox.SetDomainSubstitutionRule( __VA_ARGS__ );LOX_RELEASE )  }
#define Lox_SetVerbosity(...)               { Lox_Prune( LOX_ACQUIRE _lox.SetVerbosity ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_SetStartTime( ... )             { Lox_Prune( LOX_ACQUIRE _lox.SetStartTime ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_MapThreadName(...)              { Lox_Prune( LOX_ACQUIRE _lox.MapThreadName( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_LogState(...)                   { Lox_Prune( LOX_ACQUIRE _lox.State        ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_Verbose(...)                    { Lox_Prune( LOX_ACQUIRE _lox.Verbose      ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_Info(...)                       { Lox_Prune( LOX_ACQUIRE _lox.Info         ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_Warning(...)                    { Lox_Prune( LOX_ACQUIRE _lox.Warning      ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_Error(...)                      { Lox_Prune( LOX_ACQUIRE _lox.Error        ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_Assert(...)                     { Lox_Prune( LOX_ACQUIRE _lox.Assert       ( __VA_ARGS__);             LOX_RELEASE )  }
#define Lox_If(...)                         { Lox_Prune( LOX_ACQUIRE _lox.If           ( __VA_ARGS__);             LOX_RELEASE )  }
#define Lox_Once(...)                       { Lox_Prune( LOX_ACQUIRE _lox.Once         ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_Entry(...)                      { Lox_Prune( LOX_ACQUIRE _lox.Entry        ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_Store(...)                      { Lox_Prune( LOX_ACQUIRE _lox.Store        ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_Retrieve(data,...)              { Lox_Prune( LOX_ACQUIRE LogData* data= _lox.Retrieve( __VA_ARGS__ );  LOX_RELEASE )  }
#define Lox_SetPrefix(...)                  { Lox_Prune( LOX_ACQUIRE _lox.SetPrefix    ( __VA_ARGS__ );            LOX_RELEASE )  }
#define Lox_IsActive(result, ...)           { Lox_Prune( LOX_ACQUIRE result= _lox.IsActive( __VA_ARGS__ );         LOX_RELEASE )  }
#define Lox_Exception(...)                  { Lox_Prune( alib::lox::LogTools::Exception( LOX_LOX, __VA_ARGS__ );   )              }

#endif // HPP_ALIB_LOX_MACROS

