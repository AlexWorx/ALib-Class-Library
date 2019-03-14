// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALOX_CORE_LOGGER
#define HPP_ALOX_CORE_LOGGER 1

#if !defined(HPP_ALIB_LOX_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined (HPP_ALIB_LIB_ALIBMODULES)
#   include "alib/lib/alibmodules.hpp"
#endif



namespace aworx { namespace lib { namespace lox {
/** ************************************************************************************************
 * This is the C++ namespace for internal main classes and other things belonging to
 * the <em>%ALox Logging Library</em>.<p>
 * Types and interfaces found in this namespace are not designed for access by users of the
 * library. However, if extending \alox, e.g. by implementing new custom <em>loggers</em>, things
 * found here provide an important foundation.
 *
 * Developed by A-Worx GmbH and published under Boost Software License.
 **************************************************************************************************/
namespace detail
{

// forward declarations
class Domain;


/** ************************************************************************************************
 * This is central class of the \alox logging implementation. It is **not** recommended to use this
 * class directly for logging. Instead, use the simple and convenient static interface class
 * Log or, for release logging and other more complex operations use a Lox instance.
 * The class is abstract. To implement an own log stream, derive a new Logger class and implement
 * the abstract method #Log.
 *
 * ## Friends ##
 * class \alib{lox,Lox}
 **************************************************************************************************/
class Logger : public threads::SmartLock
{

    #if !ALIB_DOCUMENTATION_PARSER
    friend class aworx::lib::lox::Lox;
    #endif

    // #############################################################################################
    // Internal fields
    // #############################################################################################
    /**
     * An utility value. Has to be first in the class. Will be initialized prior to any other
     * field. Usually, \ref alib_manual_bootstrapping "library initialization" is performed
     * with bootstrapping of the process. However, if this is not done this field's initialization
     * code will do the job. Note that it is always recommended to bootstrap the library
     * as \ref alib_manual_bootstrapping "documented in the Programmer's Manual" of \alib.
     */
    private:    bool _apiInitializer;

    protected:
        /**
         * The name of the \e Logger. Used as a reference to a logger. All loggers attached to a
         * \b %Lox have to differ in their names.
         * If no name is specified with the constructor, the name will by the same as #TypeName.
         */
        NString32                   Name;

        /**
         *  The type name of the \e Logger. This is set by the derived class similar to the class
         *  name.
         */
        NString32                   TypeName;

    // #############################################################################################
    // public fields
    // #############################################################################################
    public:
        /**
         * The number of logs actually performed so far. In a text logger this is similar to the line
         * number, despite the fact that a single log call can produce more than one line.
         */
        int                         CntLogs                                                      =0;

        /** The creation time of the \e Logger. */
        lib::time::Ticks            TimeOfCreation;

        /** Timestamp of the last log operation. */
        lib::time::Ticks            TimeOfLastLog;

    // #############################################################################################
    // Abstract methods
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * This is the central method that derived logger classes have to implement to log a
         * message. When it is invoked the <em>%Log %Domain's %Verbosity</em> was already checked
         * against parameter \p{verbosity}. The only action to take is to perform the log
         * itself.<br>
         * Parameter \p{logables} contains at least one object, the one provided with the log
         * statement. Other objects that might be included in the list, are <em>Prefix Objects</em>
         * corresponding to the \p{scope}.
         *
         * @param dom       The <em>Log Domain</em>.
         * @param verbosity The verbosity of the message.
         * @param logables  The list of objects to log.
         * @param scope     Information about the scope of the <em>Log Statement</em>..
         ******************************************************************************************/
        virtual void   Log( Domain& dom, Verbosity verbosity, Boxes& logables, ScopeInfo& scope) =0;

    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Constructs a logger. This constructor is protected because this class is abstract.
         *
         * Note: This constructor is implemented in the header and annotated as inline.
         * This way, the Log::InitALox call receives the size of classes from the compilation unit
         * that invokes the constructor. If different compile options are set, we have a chance to
         * detect them here.
         * @param name     The name of the \e Logger. If empty, it defaults to the type name.
         *                 Will be converted to upper case.
         * @param typeName The type of the \e Logger.
         *                 Will be converted to upper case.
         ******************************************************************************************/
        inline
        Logger( const NString& name, const NString& typeName )
        : _apiInitializer( ALIB.Init() )
        , Name(name)
        , TypeName(typeName)
        , TimeOfCreation ()
        , TimeOfLastLog  ()
        {
            if ( this->Name.IsEmpty() )
                this->Name << typeName;
            this->Name    .ToUpper();
            this->TypeName.ToUpper();
        }

        /** ****************************************************************************************
         *  Destructs a logger
         ******************************************************************************************/
    public:
        virtual  ~Logger()
        {}

    // #############################################################################################
    // Registration with class Lox
    // #############################################################################################
    protected:
        /**
         * This method is invoked by class \alib{lox,Lox} when a logger is added or removed from it.
         * Note, that a logger might be added to multiple \b %Lox objects in parallel.
         *
         * The default implementation of this method is empty.
         *
         * @param lox The \b %Lox to acknowledge insertion or removal
         * @param op  The operation. Either \b ContainerOp::Insert or \b ContainerOp::Remove.
         */
        virtual void AcknowledgeLox( Lox& lox, ContainerOp op= ContainerOp ::Insert)
        {(void) lox; (void) op;}
    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Returns the name of this logger. The name has to be unique for all \e %Loggers attached
         * to a \b %Lox.
         * @return The loggers name.
         ******************************************************************************************/
        inline
        const NString&      GetName()      const  { return Name;      }

        /** ****************************************************************************************
         * Returns the constant type name of this logger. The type name is defined by the class
         * and hence provides a sort of run-time type information.
         * @return The loggers type name.
         ******************************************************************************************/
        inline
        const NString&      GetTypeName()  const  { return TypeName;  }


}; // class Logger

}}} // namespace aworx[::lib::lox::detail]


/// Type alias in namespace #aworx.
using     Logger=       aworx::lib::lox::detail::Logger;

}  // namespace [aworx]



namespace aworx { namespace lib { namespace strings {

// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if ALIB_DOCUMENTATION_PARSER
namespace APPENDABLES {
#endif

    /** ********************************************************************************************
     * Specialization of functor \alib{strings,T_Append} for type \b %Logger.
     *
     * @tparam TChar Character type of the \b AString to append info about this logger to.
     *               (Templated inline implementation for all possible character types.)
     **********************************************************************************************/
    template<typename TChar> struct T_Append<lox::detail::Logger,TChar>
    {

        /** ****************************************************************************************
         * Writes the name of the logger. In case the type name is different, it will be appended
         * in braces.
         *
         * @param  target The AString to append \p{src} to.
         * @param  logger The logger to append information for.
         ******************************************************************************************/
        inline void operator()( TAString<TChar>& target, const lox::detail::Logger& logger )
        {
            target << logger.GetName();
            if ( !logger.GetName().Equals( logger.GetTypeName() ) )
                target << " (" << logger.GetTypeName() << ")";
        }
    };

#if ALIB_DOCUMENTATION_PARSER
}
#endif

}}}// namespace [aworx::lib::lox::strings]

ALIB_BOXING_VTABLE_DECLARE( aworx::lib::lox::detail::Logger* , vt_lox_logger    )

#endif // HPP_ALOX_CORE_LOGGER
