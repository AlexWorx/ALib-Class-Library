// #################################################################################################
//  alib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALOX_CONSOLE_LOGGER)
#   include "alib/alox/loggers/consolelogger.hpp"
#endif
#endif // !defined(ALIB_DOX)


using namespace alib;

bool ConsoleLogger::notifyLogOp( lang::Phase phase )
{
    if ( phase == lang::Phase::End )
        std::cout << std::endl;
    return true;
}

alib::integer ConsoleLogger::logSubstring( const String& buffer, integer start, integer length )
{
    return writer.WriteAndGetWideLength( buffer.Substring<false>( start, length ) );
}
