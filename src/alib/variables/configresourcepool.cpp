#if ALIB_RESOURCES

namespace alib::variables {

bool ConfigResourcePool::BootstrapAddOrReplace( const NString& category,
                                                const NString& name,
                                                const  String& resource     ) {
    Variable var( Self(), String(String256(category) << '/' <<  name), A_CHAR("S") );
    bool existed=  var.IsDefined();
    if(var.Define())
        var.GetString().Reset(resource);
    return existed;
}


void ConfigResourcePool::BootstrapBulk( const nchar* category, ... ) {
    va_list args;
    va_start(args, category);
    auto cursor= Self().Root();
#if ALIB_CHARACTERS_WIDE
    String256 wCategory(category);
    cursor.GoToCreateChildIfNotExistent( wCategory );
#else
    cursor.GoToCreateChildIfNotExistent( category  );
#endif
    for(;;) {
        NString name=  va_arg( args, const nchar* );

        if( name.IsNull() )
            break;

        #if ALIB_CHARACTERS_WIDE
        String256 wName(name);
          auto child= cursor.CreatePathIfNotExistent(wName).first;
        #else
          auto child= cursor.CreatePathIfNotExistent(name).first;
        #endif
        if( !child.IsValid() ) {
            ALIB_WARNING("RESOURCES", "ConfigResourcePool: Resource value existed already. Name=",
                                      name)
            continue;
        }

        Variable var(child, A_CHAR("S") );
        (void) var.Define(Priority::DefaultValues);
        var.GetString().Reset( String(va_arg( args, const character* )) );

        // \checkpromise: when typed ALib assertions and warnings are available, then
        // raise a warning if the result of above EmplaceOrAssign is an assign, aka the
        // bulk data existed already.
    }
    va_end(args);
}


const String& ConfigResourcePool::Get( const NString& category,
                                       const NString& name       ALIB_DBG(, bool dbgAssert ) ) {

    String256 path(category); path << '/' << name;
    Variable var(Self());
    if( var.Try(path) )
        return var.GetString();

    ALIB_ASSERT_ERROR( !dbgAssert, "RESOURCES",
               "Missing resource \"{}\" in category: \"{}\"", name, category )
    return NULL_STRING;
}

} // namespace [alib::variables]
#endif // ALIB_RESOURCES
