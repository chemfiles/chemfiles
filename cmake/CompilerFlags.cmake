include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(SEND_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

macro(set_debug_flag_if_possible _flag_)
    CHECK_CXX_COMPILER_FLAG("${_flag_}" COMPILER_SUPPORTS${_flag_})
    if(COMPILER_SUPPORTS${_flag_})
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${_flag_}")
    endif()
endmacro()

# Add some warnings in debug mode
set_debug_flag_if_possible("-Wall")
set_debug_flag_if_possible("-Wextra")
set_debug_flag_if_possible("-Wconversion")
set_debug_flag_if_possible("-Wsign-conversion")
set_debug_flag_if_possible("-Wsign-promo")


include(CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG("-std=c99" COMPILER_SUPPORTS_C99)

if(COMPILER_SUPPORTS_C99)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()
