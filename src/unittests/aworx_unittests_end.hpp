//
// this file has to be included at the end of an unit test file (after an unit test class)
//

#if defined(TESTCLASSNAME)
    UT_CLASS_END
#   undef TESTCLASSNAME
#endif




// Restore ALIB_CALLER
#undef ALIB_CALLER
#if defined( __GNUC__ )
#   define ALIB_CALLER    __FILE__, __LINE__, __func__
#elif defined ( _MSC_VER )
#   define ALIB_CALLER     __FILE__, __LINE__, __FUNCTION__
#else
#   pragma message ("Unknown Platform in file: " __FILE__ )
#endif
