//==================================================================================================
/// \file
/// This header-file is part of module \alib_filetree of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {

//==================================================================================================
/// This is the reference documentation of module \alib_filetree_nl of the \aliblong.<br>
/// @see A user manual with tutorial-style sample code is found in the
///      #"alib_mod_filetree;Programmer's Manual" of this module.
//==================================================================================================
namespace filetree {

//==================================================================================================
/// This is a strict singleton class representing module \alib_filetree.
///
/// The only instance is found with the namespace variable #"alib::filetree".
//==================================================================================================
class FilesCamp : public camp::Camp {
  public:
    /// Constructor.<br>
    /// While this is public, it must not be invoked as this is a strict singleton type.
    /// (See notes in #"alib_camp_camp").
    ALIB_DLL
    FilesCamp();

  protected:
    /// Initializes this camp.
    ALIB_DLL
    virtual void    Bootstrap()                                                            override;

    /// Terminates this camp. (Nothing to do.)
    virtual void    Shutdown( ShutdownPhases )                                           override {}

}; // class FilesCamp

} // namespace alib[::filetree]

/// The singleton instance of \alibcamp class #"FilesCamp".
extern ALIB_DLL filetree::FilesCamp FILETREE;

} // namespace [alib]
