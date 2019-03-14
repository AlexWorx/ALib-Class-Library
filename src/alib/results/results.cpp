// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_RESULTS_RESULTS)
#   include "alib/results/results.hpp"
#endif

#if !defined (HPP_ALIB_RESULTS_ICLONEMESSAGEARGUMENT)
#   include "alib/results/iclonemessageargument.hpp"
#endif

#if !defined (HPP_ALIB_BOXING_ENUM)
#    include "alib/boxing/enum.hpp"
#endif

#if !defined (HPP_ALIB_LIB_ALIBMODULES)
#    include "alib/lib/alibmodules.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#    include "alib/resources/resources.hpp"
#endif

#if !defined (HPP_ALIB_STRINGFORMAT_STRINGFORMAT)
#   include "alib/stringformat/stringformat.hpp"
#endif

namespace aworx { namespace lib {

results::Results RESULTS;

/**
 * This \alibmod is very small and might undergo some changes in the future development of the
 * library.
 *
 * Please consult \ref alib_mod_results "ALib Module Results - Programmer's Manual"
 * for further information.
 */
namespace results {


// #################################################################################################
// Replacement method for ALib Essential Reports
// #################################################################################################

#if ALIB_DEBUG
#if !ALIB_DOCUMENTATION_PARSER
namespace {
#endif
    /**
     * This method is installed with \alib{DBG_SIMPLE_ALIB_MSG_PLUGIN} in method
     * \alib{Module::init}.
     *
     * The message strings are simply passed to the default \alib{results,Report}.
     * This way, the essential assert, error and message macros are using the \alib report system
     * in the moment that the complete %ALib library is in place (instead of only one of the
     * library's modules.
     *
     * @param file    Information about the scope of invocation.
     * @param line    Information about the scope of invocation.
     * @param method  Information about the scope of invocation.
     * @param type    The type of message. See \alib{results,Message}.
     * @param qtyMsgs The number of messages in \p{msgs}.
     * @param msgs    A list of strings (this is all that the essential versions of \alib reporting
     *                macros provide).
     */
    void debugReportPlugin(const char* file, int line, const char* method, int type, int qtyMsgs, const nchar** msgs)
    {
        Message message( file,line,method, Report::Types(type), msgs[0] );
        for (int i= 1; i< qtyMsgs; ++i )
            message.Args.Add( msgs[i] );
        Report::GetDefault().DoReport( message );
    }
#if !ALIB_DOCUMENTATION_PARSER
} // anonymous namespace
#endif
#endif // ALIB_DEBUG


// #################################################################################################
// Module class Messages
// #################################################################################################
Results::Results()
: Module( ALIB_VERSION, ALIB_REVISION, "ALIB_RESULTS" )
{
    ALIB_ASSERT_ERROR( this == &RESULTS,
        "Instances of class Results must not be created. Use singleton aworx::lib::RESULTS" )

    auto& moduleStringFormat= lib::STRINGFORMAT;
    Dependencies.emplace_back( &moduleStringFormat );
    moduleStringFormat.Dependencies.emplace_back( this );
}


void Results::init( InitLevels level, int, const char**, const wchar_t** )
{
    if( level == InitLevels::PrepareResources )
    {
        ALIB.CheckDistribution();

        #if ALIB_DEBUG
            lib::DBG_SIMPLE_ALIB_MSG_PLUGIN= debugReportPlugin;
        #endif



        Resources->AddBulk( ResourceCategory,

        // aworx::lib::results::Exceptions
        "ExceptionsPrefix"  , A_CHAR("") ,
        "ExceptionsPostfix" , A_CHAR("")       ,
        "Exceptions"        , A_CHAR("1,ErrorWritingReport,EX1") ,

        "EX1"  , A_CHAR("Error writing ALib report."),

        // end of AddBulk()
        nullptr );
    }

}

void Results::terminationCleanUp()
{
    Report::TerminationCleanUp();
}


// #################################################################################################
// class Message
// #################################################################################################
void   Message::CloneArguments( memory::MemoryBlocks& memoryBlocks )
{
    Args.CallAll<boxing::FClone>( memoryBlocks );
}


}}} // namespace [aworx::lib::results]





