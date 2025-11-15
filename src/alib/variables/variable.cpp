//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/variables/variables.prepro.hpp"
#include "ALib.Strings.StdFunctors.H"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Variables;
#else
#   include "ALib.Variables.H"
#endif
//========================================== Implementation ========================================
namespace alib::variables {

const String&    Variable::substitute( const String&  orig,
                                       AString&       buf , const StringEscaper* escaper ) {
    Configuration& cfg= GetConfiguration();
    String substitutionVariableStart= cfg.SubstitutionVariableStart;
    if( substitutionVariableStart.IsEmpty() )
        return orig;

    String substitutionVariableEnd       = cfg.SubstitutionVariableEnd;
    String substitutionVariableDelimiters= cfg.SubstitutionVariableDelimiters;

    integer searchStartIdx  =  0;
    int     maxReplacements = 50;
    do
    {
        // search start
        integer repStart= orig.IndexOf( substitutionVariableStart, searchStartIdx );
        if ( repStart < 0 )
            break;
        buf << orig.Substring( searchStartIdx, repStart - searchStartIdx );
        searchStartIdx  = repStart;
        integer varStart= repStart + substitutionVariableStart.Length();

        integer varLen;

        // search end in two different ways depending on setting of public field "SubstitutionVariableEnd"
        if ( substitutionVariableEnd.IsEmpty() ) {
            integer idx=   orig.IndexOfAny<lang::Inclusion::Include>( substitutionVariableDelimiters, varStart );
            if ( idx < 0 )
                idx= orig.Length();

            varLen= idx - varStart;
            searchStartIdx= idx;
        } else {
            integer idx=   orig.IndexOf( substitutionVariableEnd, varStart );
            if (idx < 0 ) {
                ALIB_DBG( String256 namebuf; )
                ALIB_WARNING( "VARIABLES", "End of substitution variable not found "
                                         "(while start was found). Variable name: ", Name(namebuf) )
                break;
            }

            varLen= idx - varStart;
            searchStartIdx= idx + substitutionVariableEnd.Length();
        }

        // get variable name string
        Substring    replVarName= orig.Substring( varStart, varLen );
        if ( replVarName.IsEmpty() ) {
            buf << substitutionVariableStart;
            continue;
        }

        // load replacement variable:
        // - First we do a try. If this succeeds (the variable is already declared), then we're happy
        // - If not, we declare it as string. This might read it from a plug-in or from a Preset string!
        //   While we are not sure about the type, we declare it as "S" and then delete it, for the case that
        //   it is not a string! This is the best we can do! For example, the escaper given with the preset
        //   is used.
        Variable replVar(cfg);
        if( replVar.Try(replVarName) )
            replVar.Export( buf, escaper );
        else {
            replVar.Declare(replVarName, A_CHAR("S") );
            if( !replVar.IsDefined() ) {
                replVar.Delete();
                continue;
            }
            replVar.Export( buf, escaper );
            replVar.Delete();
        }


    }
    while( --maxReplacements );

    if( maxReplacements < 50) {
        buf << orig.Substring( searchStartIdx );
        return buf;

    }
    return orig;
}


void           Variable::create( const String& typeName, const String& defaultValue ) {
    auto it= Tree<Configuration>().types.Find(typeName);
    ALIB_ASSERT_ERROR( it != Tree<Configuration>().types.end(), "VARIABLES",
          "No Meta-Handler found for given variable type \"{}\".\n"
          "Probably the type was not registered during bootstrap.\n"
          "Use macro ALIB_VARIABLES_REGISTER_TYPE in bootstrap phase "
                    "'PrepareConfig' to register your custom types.", typeName)
    auto*  meta= Cursor::Value().meta= *it;

  //-------------------------------------------- declare -------------------------------------------
    Cursor::Value().data    =  reinterpret_cast<detail::VDATA*>(Tree<Configuration>().Pool().Alloc( meta->size(), alignof(detail::VDATA)));
    meta->construct(Cursor::Value().data, Tree<Configuration>().Pool );
    Cursor::Value().priority= Priority::NONE;

    String256 varName(*this);

    // invoke listeners
    Tree<Configuration>().notifyListeners( int(ConfigurationListener::Event::Creation),
                                           *this,
                                           varName,
                                           Priority::NONE    );


  //------------------- check for definitions (plugins, preset or default values) ------------------
  // Plugins?
    {
        String256 buf;
        for (int i = 0; i < Tree<Configuration>().CountPlugins(); ++i) {
            auto& plugin= *Tree<Configuration>().GetPlugin(i);
            auto  plPrio= plugin.GetPriority();
            if( Cursor::Value().priority <= plPrio  && plugin.Get(varName, buf) ) {
                String512 substBuf;
                Cursor::Value().priority= plPrio;
                Cursor::Value().meta->imPort( Cursor::Value().data, GetConfiguration(), plugin.GetEscaper(),
                                              substitute(buf, substBuf, &plugin.GetEscaper())  );
    }   }   }

    // Preset value?
    auto cursor= Tree<Configuration>().Root();
    if(     cursor.GoToChild(A_CHAR("$PRESETS"))
        &&  cursor.GoTo(varName).IsEmpty()
        &&  cursor->meta != nullptr              )
    {
        ALIB_ASSERT_ERROR( Variable(cursor).GetString().IsNotNull(), "VARIABLES",
                   "Internal error. This must never happen. ")
        ALIB_ASSERT_ERROR( Cursor::Value().priority == Priority::NONE, "VARIABLES",
                   "Internal error. This must never happen. ")
        StringEscaper voidEscaper;
        auto* escaper= cursor->declaration ? reinterpret_cast<const StringEscaper*>( cursor->declaration )
                                                  : &voidEscaper;
        Cursor::Value().priority= cursor->priority;
        String512 substBuf;
        Cursor::Value().meta->imPort( Cursor::Value().data,
                                      GetConfiguration(),
                                      *escaper,
                                      substitute(Variable(cursor), substBuf, escaper) );

    }

    // default value?
    if( Cursor::Value().priority <= Priority::DefaultValues  &&  defaultValue.IsNotEmpty() ) {
        StringEscaper escaper;
        String512 substBuf;
        Cursor::Value().priority= Priority::DefaultValues;
        Cursor::Value().meta->imPort( Cursor::Value().data,GetConfiguration(), escaper,
                                      substitute( defaultValue, substBuf, &escaper)  );
    }

    // invoke listeners
    if( IsDefined() )
        Tree<Configuration>().notifyListeners( int(ConfigurationListener::Event::Definition),
                                               *this,
                                               varName,
                                               Priority::NONE    );

} // Variable::initialize()

Variable&      Variable::Declare( const String& name        , const String& typeName,
                                  const String& defaultValue                            ) {
    ALIB_ASSERT_ERROR(cmCursor::tree != nullptr, "STRINGTREE",
      "Invalid Variable. Not associated with a Configuration. Probably a default constructed "
      "instance.\nCopy or move a valid Variable object before usage.")

        ALIB_ASSERT_ERROR( name.IndexOf('%') <0, "VARIABLES",
                                                 "Variable name with placeholder(s) given: ", name )

    // Variable existed?
    GoToRoot();
    if(    GoToCreatedPathIfNotExistent(name) == 0
        && Cursor::Value().meta != nullptr              )
    {
        #if ALIB_DEBUG
            auto it=  Tree<Configuration>().types.Find(typeName);
            if( it == Tree<Configuration>().types.end() ) {
              ALIB_ERROR( "VARIABLES",
                "No Meta-Handler found for given variable type \"{}\".\n"
                "Probably the type was not registered during bootstrap.\n"
                "Use macro ALIB_VARIABLES_REGISTER_TYPE in bootstrap phase "
                "'PrepareConfig' to register your custom types.", typeName )
            }
            if( *it != Cursor::Value().meta ) {
              ALIB_ERROR( "VARIABLES",
                 "Variable \"{}\" redeclared with a different typename.\n"
                 "Previous typename: ", Cursor::Value().meta->typeName(), typeName )
            }
        #endif
        return *this;
    }

    // new variable
    Cursor::Value().declaration= nullptr;
    create( typeName, defaultValue );
    return *this;
}

Variable&      Variable::Declare( const Declaration* decl ) {
    ALIB_ASSERT_ERROR(cmCursor::tree != nullptr, "STRINGTREE",
      "Invalid Variable. Not associated with a Configuration.\n"
      "Probably a default constructed instance.\n"
      "Copy or move a valid Variable object before usage.")

    ALIB_ASSERT_ERROR(decl->Name().IndexOf('%') <0, "VARIABLES",
                      "Variable descriptor \"{}\" with unset placeholders given.", decl->Name() )

    // Variable existed?
    GoToRoot();
    if( 0 == GoToCreatedPathIfNotExistent( decl->Name() )) {
        #if ALIB_DEBUG
            ALIB_ASSERT_WARNING( GetDeclaration() == nullptr || GetDeclaration() == decl, "CONFIG/VARDECL",
             "Variable \"{}\" redeclared with different declaration record pointer.\n"
             "Declaration records should be singletons and their life-time needs to survive\n"
             "that of the variable. New record will be ignored.", decl->Name() )

            auto it= Tree<Configuration>().types.Find(decl->typeName);
            if( it == Tree<Configuration>().types.end() ) {
              ALIB_ERROR( "VARIABLES",
              "No Meta-Handler found for given variable type \"{}\".\n"
              "Probably the type was not registered during bootstrap.\n"
              "Use macro ALIB_VARIABLES_REGISTER_TYPE in bootstrap phase "
                    "'PrepareConfig' to register your custom types.",  GetDeclaration()->typeName)
            }
        #endif
        return *this;
    }

    // new variable
    Cursor::Value().declaration= decl;
    create(decl->typeName, decl->DefaultValue() );
    return *this;
}

bool Variable::Define(Priority requestedPriority) {
    if( Cursor::Value().priority > requestedPriority )
        return false;
    auto prevPriority= Cursor::Value().priority;
    Cursor::Value().priority= requestedPriority ;
    Tree<Configuration>().notifyListeners( int(ConfigurationListener::Event::Definition),
                                           *this,
                                           nullptr,
                                           prevPriority  );
    return true;
}

void      Variable::Delete() {
    Tree<Configuration>().notifyListeners( int(ConfigurationListener::Event::Deletion),
                                           *this,
                                           nullptr,
                                           GetPriority()  );
    Cursor::Delete();
    Cursor::node= nullptr;
}

bool Variable::Try(const String& name, const String& typeName) {
    ALIB_ASSERT_ERROR(cmCursor::tree != nullptr, "STRINGTREE",
      "Invalid Variable. Not associated with a Configuration. Probably a default constructed "
      "instance.\nCopy or move a valid Variable object before usage.")

    // check if this variable is already declared
    if(    Cursor::GoToRoot().GoTo( name ).IsEmpty()
           && IsDeclared() )
        return true;

    // check if a preset exists
    auto cursor= Tree<Configuration>().Root();
    if( cursor.GoToChild(A_CHAR("$PRESETS")) && cursor.GoTo(name).IsEmpty() ) {
        ALIB_ASSERT_ERROR( Variable(cursor).GetString().IsNotNull(), "VARIABLES",
                           "Internal error. This must never happen.")
        Declare( name, typeName );
        if( Cursor::Value().priority < cursor->priority) {
            StringEscaper voidEscaper;
            auto* escaper=  cursor->declaration ? reinterpret_cast<const StringEscaper*>( cursor->declaration )
                                                : &voidEscaper;
            String512 substBuf;
            Cursor::Value().priority= cursor->priority;
            Cursor::Value().meta->imPort( Cursor::Value().data,
                                          GetConfiguration(),
                                          *escaper,
                                          substitute(Variable(cursor), substBuf, escaper) );
        }
        return true;
    }
    return false;
}

bool Variable::Try(const Declaration* decl) {
    if( Try(decl->Name(), decl->TypeName()) ) {
        // already declared, add the declaration struct, if not set, yet.
        ALIB_ASSERT_WARNING(   Cursor::Value().declaration== nullptr
                            || Cursor::Value().declaration == decl,          "CONFIG/VARDECL",
             "Variable \"{}\" redeclared with different declaration record pointer.\n"
             "Declaration records should be singletons and their life-time needs to survive\n"
             "that of the variable. New record will be ignored.", decl->Name() )

        if( Cursor::Value().declaration== nullptr)
            Cursor::Value().declaration= decl;
        return true;
    }

    return false;
}

void Variable::Import( const String& src, Priority priority, const StringEscaper* escaper ) {
    ALIB_ASSERT_ERROR( src.IsNotNull(), "VARIABLES",
       "Tried to import nulled string for variable \"{}\"", this )

     StringEscaper exVoid;
     if( !escaper )
         escaper= &exVoid;
     if(Define(priority)) {
        String512 substBuf;
        getMeta()->imPort( Cursor::Value().data, GetConfiguration(), *escaper,
                           substitute(src, substBuf, escaper) );
}   }


} // namespace [alib::variables]
