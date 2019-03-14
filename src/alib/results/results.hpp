// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#ifndef HPP_ALIB_RESULTS_RESULTS
#define HPP_ALIB_RESULTS_RESULTS 1

#if !defined(HPP_ALIB_LIB_MODULE)
#   include "alib/lib/module.hpp"
#endif

#if !defined(HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

ALIB_ASSERT_MODULE(RESULTS)

namespace aworx { namespace lib {  namespace results {

/** ************************************************************************************************
 * Exception codes of namespace \alibmod_results.
 **************************************************************************************************/
enum class Exceptions
{
    /** Error when writing a report. This typically indicates an erroneous format string in an
     *  \ref ALIB_ASSERT_ERROR or related macro. */
    ErrorWritingReport= 1,
};

/** ************************************************************************************************
 * The module class.
 *
 * This is a strict singleton class. The only instance is found with namespace variable
 * \ref aworx::lib::RESULTS.
 **************************************************************************************************/
class Results : public Module
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_modsingletons).
         ******************************************************************************************/
        Results();

    protected:
        /** ****************************************************************************************
         * Initializes this module.
         *
         * @param level  The initialization level to reach.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void        init( InitLevels level,
                                  int argc, const char** argv, const wchar_t** wargv )     override;

        /** ****************************************************************************************
         * Frees resources of this module.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

}; // class Results


} // namespace aworx::lib[::results]

/** The singleton of module-class \alib{results,Results}. */
extern ALIB_API results::Results RESULTS;

}} // namespace [aworx::lib]


ALIB_EXCEPTIONS( aworx::lib::results::Exceptions, aworx::lib::RESULTS, "Exceptions" )

#endif // HPP_ALIB_RESULTS_RESULTS
