//==================================================================================================
/// \file
/// This header-file is part of module \alib_camp of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {

//==================================================================================================
/// This is the reference documentation of module \alib_camp_nl of the \aliblong.<br>
/// Besides the few entities found and documented here (namely in namespace
/// <em>alib::camp</em>), this module <em>"injects"</em> several overloaded versions of
/// the function #"CampVariable(camp::Camp&);CampVariable" into namespace <em>alib::variables</em>.
/// @see
///   Please also consult the #"alib_mod_camp;Programmer's Manual" of this module.
//==================================================================================================
namespace camp {


//##################################################################################################
// Variables
//##################################################################################################

/// Configuration variables of \alib.
/// \note
///   As required by module \alib_variables, this enumeration is equipped with
///   #"alib_enums_records;ALib Enum Records" of type #"var Declaration"
///   and resourced with this module.<br>
///   Hence, all variable categories, names, default values and such can be modified
///   by modifying the resource data of the singleton of class #"Basecamp".
enum class Variables {
    LOCALE               = 1   , ///< See #"alibcvALIB_LOCALE".
    WAIT_FOR_KEY_PRESS   = 2   , ///< See #"alibcvALIB_WAIT_FOR_KEY_PRESS".
    HAS_CONSOLE_WINDOW   = 3   , ///< See #"alibcvALIB_HAS_CONSOLE_WINDOW".
};


//==================================================================================================
/// This is a strict singleton class representing module \alib_camp.
/// The only instance is found with the namespace variable #"alib::BASECAMP;2".
///
/// @see
///   For detailed descriptions, please consult the chapter #"alib_camp_basecamp" of the
///   Programmer's Manual of the module \alib_camp_nl.
//==================================================================================================
class Basecamp : public Camp {
  public:
    /// If \c true, within #".Shutdown", it is waited for a key press in the console
    /// window.<br>
    /// By default, this flag is enabled when debugging a console application under Visual
    /// Studio.<br>
    /// This default behavior can be overruled by setting configuration variable
    /// #"alibcvALIB_WAIT_FOR_KEY_PRESS".<br>
    /// In addition, this public flag may be modified at runtime (after method #"Bootstrap" was
    /// invoked).
    bool                    WaitForKeyPressOnTermination                                     =false;

    /// This flag indicates if the current process has console output window attached.
    /// While this is not detectable in Unix like systems (or just with huge efforts), in
    /// windows it is. The determination is performed in #"Bootstrap" and done as follows
    /// - If configuration variable \b ALIB_HAS_CONSOLE_WINDOW
    ///   is set, its value is returned.
    /// - Otherwise, on Unix like OS\c true is returned.
    /// - On Windows OS it is determined if a console window is attached to the process
    ///   using system call \e GetConsoleWindow() and compare its result to \e NULL.
    ///
    /// In addition, this public flag may be modified at runtime (after method #"Bootstrap" was
    /// invoked).
    ///
    /// \note
    ///   The evaluation of configuration variable \b ALIB_HAS_CONSOLE_WINDOW is done
    ///   only if module \alib_variables is included in the \alibbuild.
    bool                    HasConsoleWindow;

    /// This string is set during bootstrapping in the case that a locale information was
    /// found (via standard environment variables or \alib variable #"Variables::LOCALE")
    /// and successfully set.<br>
    /// Otherwise this string is \e nulled.
    String                  LocaleFound;

    /// Constructor.<br>
    /// While this is public, it must not be invoked as this is a strict singleton type.
    /// (See notes in #"alib_camp_camp").
    ALIB_DLL
    Basecamp();

    /// Implementation of #"Camp::Bootstrap;*".
    ///
    /// In phase #"BootstrapPhases::Final;2" performs:
    /// - In glibc versions of \alib (GNU/unix) probably invoke glibc method
    ///   <em>setlocale()</em>, depending on the setting of the environment variables
    ///   <em>LANG</em> and <em>LANGUAGE</em> and depending on \alib configuration variable
    ///   #"alibcvALIB_LOCALE".
    ///
    ///   In addition, invokes
    ///   #"TNumberFormat::SetFromLocale" on static singleton
    ///   #"TNumberFormat::Global;NumberFormat::Global".
    ///
    /// - Config variable #"WAIT_FOR_KEY_PRESS" is read and the field
    ///   #"WaitForKeyPressOnTermination" set accordingly.
    ALIB_DLL
    virtual void    Bootstrap()                                                            override;

    /// Terminates this camp.
    /// @param phase  The shutdown phase to perform.
    ALIB_DLL
    virtual void    Shutdown( ShutdownPhases phase)                                        override;

  //################################################################################################
  // Environment definition/detection
  //################################################################################################
    /// If \c true, the current process is under the hood of a debugger.
    ///
    /// @return \c true if the application is run in a debugger.
    ALIB_DLL
    bool        IsDebuggerPresent();

}; // class Basecamp

} // namespace alib[::camp]

/// The singleton instance of \alibcamp class #"Basecamp".
extern ALIB_DLL camp::Basecamp BASECAMP;

} // namespace [alib]



ALIB_RESOURCED_IN_CAMP( alib::format::ByteSizeUnits , alib::BASECAMP, "BS" )
ALIB_RESOURCED_IN_CAMP( alib::format::FMTExceptions , alib::BASECAMP, "FE" )

ALIB_RESOURCED_IN_CAMP( alib::variables::Exceptions , alib::BASECAMP, "VARE" )

DOX_MARKER([DOX_VARIABLES_DECLARATION5])
ALIB_CAMP_ENUM( alib::camp::Variables, alib::variables::Declaration, alib::BASECAMP, "Var" )
DOX_MARKER([DOX_VARIABLES_DECLARATION5])
ALIB_RESOURCED_IN_CAMP( alib::variables::Priority   , alib::BASECAMP, "VarPrio" )
