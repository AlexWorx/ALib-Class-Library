#if ALIB_DEBUG && !DOXYGEN
namespace alib::boxing::debug {

    // This is used by boxing::Bootstrap to do runtime-check for compatibility of boxing
    // and long double values.
    // It was put here to prevent the compiler to optimize and remove the code.
    extern  long double LONGDOUBLE_WRITE_TEST_MEM[2];
            long double LONGDOUBLE_WRITE_TEST_MEM[2];
extern  void LongDoubleTrueLengthSet();
void LongDoubleTrueLengthSet()
{ memset( LONGDOUBLE_WRITE_TEST_MEM, 0x3E, 2 * ALIB_SIZEOF_LONGDOUBLE_REPORTED); }
extern  bool LongDoubleTrueLengthTest();
bool LongDoubleTrueLengthTest() {
const char* mem= reinterpret_cast<const char*>( LONGDOUBLE_WRITE_TEST_MEM );
return    mem[ALIB_SIZEOF_LONGDOUBLE_WRITTEN - 1] != 0x3E
   && mem[ALIB_SIZEOF_LONGDOUBLE_WRITTEN    ] == 0x3E;
}
} // namespace [alib::boxing::detail]
#endif
