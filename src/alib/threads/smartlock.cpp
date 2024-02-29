// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALIB_THREADS_SMARTLOCK)
#   include "alib/threads/smartlock.hpp"
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif
#endif // !defined(ALIB_DOX)

namespace alib {  namespace threads {

SmartLock     SmartLock::StdOutputStreams;

int SmartLock::CntAcquirers()
{
    ALIB_LOCK_WITH( lock )
    return static_cast<int>( acquirers.size() );
}

int   SmartLock::AddAcquirer( ThreadLock* newAcquirer )
{
    size_t count;
    ALIB_DBG(
    bool errAlreadyAdded=       true;
    int  errWasAcquired=        0;        )
    {
        ALIB_LOCK_WITH( lock )

        count= acquirers.size();

        // check doubly added
        if (     newAcquirer == nullptr
             ||  std::find( acquirers.begin(), acquirers.end(), newAcquirer ) == acquirers.end() )
        {
            ALIB_DBG(
            errAlreadyAdded= false;
            errWasAcquired=   ThreadLock::CountAcquirements() != 0 ? 1 : 0;        )

            // switch on?
            if( acquirers.size() == 1 )
            {
                ThreadLock* firstAcquirer= acquirers[0];

                // non-anonymous acquirer?
                if ( firstAcquirer != nullptr )
                {
                    firstAcquirer->Acquire( ALIB_CALLER_PRUNED);
                        SetSafeness( lang::Safeness::Safe );
                        acquirers.emplace_back( newAcquirer );
                        ++count;
                    firstAcquirer->Release();
                }

                // critical section: our first acquirer is anonymous. As documented in class,
                // this must happen only in situations, when we are sure, that we are safe, e.g. still
                // single threaded execution of process bootstrap.
                else
                {
                    // If this assert happens, its only good luck: we detected a misuse. But it should
                    // be very seldom to be detected this way :-/
                    ALIB_DBG(
                    if ( errWasAcquired == 1 )
                        errWasAcquired= 2;            )

                    SetSafeness( lang::Safeness::Safe );
                    acquirers.emplace_back( newAcquirer );
                    ++count;
                }
            }
            else
                acquirers.emplace_back( newAcquirer );
        }
    }// ALIB_LOCK_WITH


    ALIB_ASSERT_ERROR( !errAlreadyAdded , "THREADS", "Acquirer already registered." )
    ALIB_ASSERT_ERROR( errWasAcquired!=1, "THREADS", "Already acquired. Hint: Acquirer[0] must not acquire this before adding itself!" )
    ALIB_ASSERT_ERROR( errWasAcquired!=2, "THREADS", "Acquired and acquirer[0] anonymous. Misuse of SmartLock!" )

    return static_cast<int>( count );
}

int   SmartLock::RemoveAcquirer( ThreadLock* acquirerToRemove )
{
    size_t count;
    ALIB_DBG(
    bool errWasAcquired;
    bool errNotFound=    true;      )
    {
        ALIB_LOCK_WITH( lock )

        ALIB_DBG(
        errWasAcquired=   ThreadLock::CountAcquirements() != 0;    )

        // search acquirer
        auto it= std::find( acquirers.begin(), acquirers.end(), acquirerToRemove );
        if( it != acquirers.end() )
        {
            ALIB_DBG(
            errNotFound= false;     )

            // switch off?
            if( acquirers.size() == 2 )
            {
                ThreadLock* acquirer1=  acquirers[0];
                ThreadLock* acquirer2=  acquirers[1];
                if( acquirer1== acquirerToRemove ) acquirer1= nullptr;
                if( acquirer2== acquirerToRemove ) acquirer2= nullptr;

                // Acquire acquirers in their order of appearance
                if ( acquirer1 != nullptr ) acquirer1->Acquire(ALIB_CALLER_PRUNED);
                  if ( acquirer2 != nullptr ) acquirer2->Acquire(ALIB_CALLER_PRUNED);
                      SetSafeness( lang::Safeness::Unsafe );
                      acquirers.erase( it );
                  if ( acquirer2 != nullptr ) acquirer2->Release();
                if ( acquirer1 != nullptr ) acquirer1->Release();
            }

            // just remove acquirer, keep mode
            else
                acquirers.erase( it );
        }
        count= acquirers.size();
    }

    ALIB_ASSERT_ERROR( !errNotFound,    "THREADS",  "Acquirer not found." )
    ALIB_ASSERT_ERROR( !errWasAcquired, "THREADS",  "Acquired on release. Hint: Acquirers must acquire only when acquired themselves!" )
    return static_cast<int>( count );
}

}} // namespace [alib::threads]
