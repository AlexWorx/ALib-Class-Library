//==================================================================================================
/// \file
/// This header-file is part of module \alib_files of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {

//==================================================================================================
/// This is the reference documentation of module \alib_files_nl of the \aliblong.<br>
/// @see
///  A user manual with tutorial-style sample code is found in the
///  \ref alib_mod_files "Programmer's Manual" of this module.
//==================================================================================================
namespace files {

//==================================================================================================
/// This is a strict singleton class representing module \alib_files.
///
/// The only instance is found with the namespace variable \ref alib::files.
//==================================================================================================
class FilesCamp : public camp::Camp
{
    public:
        /// Constructor.<br>
        /// While this is public, it must not be invoked as this is a strict singleton type.
        /// (See notes in \ref alib_camp_camp).
        ALIB_DLL
        FilesCamp();

    protected:
        /// Initializes this camp.
        ALIB_DLL
        virtual void    Bootstrap()                                 override;

        /// Terminates this camp. (Nothing to do.)
        virtual void    Shutdown( ShutdownPhases )                                       override {}

}; // class FilesCamp

} // namespace alib[::files]

/// The singleton instance of \alibcamp class \alib{files;FilesCamp}.
extern ALIB_DLL files::FilesCamp FILES;

} // namespace [alib]
