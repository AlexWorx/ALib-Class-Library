
//##################################################################################################
// Type De-mangling
//##################################################################################################
#if ALIB_DEBUG && !DOXYGEN
namespace alib::lang {

    #if defined(__GNUC__) || defined(__clang__)
DbgTypeDemangler::DbgTypeDemangler( const std::type_info& typeInfo ) {
    int status;
    name= abi::__cxa_demangle( typeInfo.name(), nullptr, nullptr, &status);
    assert( status==0 || !name );
    if ((failed= (name==nullptr)))
        name= "<DbgTypeDemangler Error>";
}

DbgTypeDemangler::~DbgTypeDemangler()
{ if (!failed) std::free(static_cast<void*>( const_cast<char*>(name) ) ); }

    #else
        lang::DbgTypeDemangler::DbgTypeDemangler( const type_info& typeInfo )
        {
            name= typeInfo.name();
            if (std::strncmp( name, "class ", 6) == 0)
                name+= 6;
        }
    #endif

const char* DbgTypeDemangler::Get()                                                 { return name; }

} // namespace [alib::lang}

#endif // ALIB_DEBUG && !DOXYGEN
