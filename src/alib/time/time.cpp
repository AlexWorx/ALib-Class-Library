using namespace std::chrono;

namespace alib {

/// This namespace provides types for calendrical date and time processing as well as
/// for non-calendrical steady and monotonic time measurement.
///
/// This namespace and the types provided, are always included in any \alibbuild.
///
/// Besides this reference documentation, further information is provided with
/// #"alib_mod_time;Programmer's Manual" of \alib_time_nl.
namespace time {

//##################################################################################################
// Module Bootstrap/Termination
//##################################################################################################
#if !DOXYGEN
namespace { Ticks   creationTime; }
#endif

#   include "ALib.Lang.CIFunctions.H"

Ticks  CreationTime()                                                       { return creationTime; }

}} // namespace [alib::time]
