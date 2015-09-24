/*
 * C dynamic library for testing the Dynlib class
 */

#if defined( _WIN32 ) || defined( _WIN64 )
     #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

EXPORT int foo(int a){
    return 3*a + 4;
}

EXPORT double bar(){
    return 42;
}
