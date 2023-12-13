/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_results of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_RESULTS_RESULTS
#define HPP_ALIB_RESULTS_RESULTS 1

#if !defined(HPP_ALIB_FS_MODULES_MODULE)
#   include "alib/lib/fs_modules/module.hpp"
#endif

#if !defined(HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

ALIB_ASSERT_MODULE(RESULTS)

namespace aworx { namespace lib {  namespace results {

/** ************************************************************************************************
 * Exception codes of namespace \alib_results.
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
         * (See notes in \ref alib_manual_bootstrapping_class_module_singletons).
         ******************************************************************************************/
        Results();

    protected:
        /** ****************************************************************************************
         * Initializes this module.
         *
         * @param phase  The initialization phase to perform.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void    bootstrap( BootstrapPhases phase,
                                   int argc, const char** argv, const wchar_t** wargv )    override;

        /** ****************************************************************************************
         * Terminates this module.
         * @param phase  The shutdown phase to perform.
         ******************************************************************************************/
        virtual void    shutdown( ShutdownPhases phase )                                override;

}; // class Results


} // namespace aworx::lib[::results]

/** The singleton of module-class \alib{results,Results}. */
extern ALIB_API results::Results RESULTS;

}} // namespace [aworx::lib]


ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::results::Exceptions, aworx::lib::results::ERException  )
ALIB_RESOURCED_IN_MODULE( aworx::lib::results::Exceptions, aworx::lib::RESULTS, "E" )

#endif // HPP_ALIB_RESULTS_RESULTS
