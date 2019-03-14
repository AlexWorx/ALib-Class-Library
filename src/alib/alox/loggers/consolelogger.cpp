// #################################################################################################
//  aworx::lib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(HPP_ALOX_CONSOLE_LOGGER)
#   include "alib/alox/loggers/consolelogger.hpp"
#endif


using namespace aworx;

bool ConsoleLogger::notifyLogOp( Phase phase )
{
    if ( phase == Phase::End )
        std::cout << std::endl;
    return true;
}

aworx::integer ConsoleLogger::logSubstring( const String& buffer, integer start, integer length )
{
    return writer.WriteAndGetWideLength( buffer.Substring<false>( start, length ) );
}
