include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

option(USE_WARNINGS "Compile the code with warnings (default in debug mode)" OFF)

# C++11 support.
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXXOX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    if(MSVC)
        if(MSVC_VERSION LESS 1900)
            message(SEND_ERROR "MSVC < 14.0 is not supported. Please update your compiler or use mingw")
        endif()
    else()
        message(SEND_ERROR "The ${CMAKE_CXX_COMPILER} compiler lacks C++11 support. Use another compiler.")
    endif()
endif()

macro(add_warning _flag_)
    CHECK_CXX_COMPILER_FLAG("${_flag_}" CXX_SUPPORTS${_flag_})
    CHECK_C_COMPILER_FLAG("${_flag_}" CC_SUPPORTS${_flag_})
    if(CXX_SUPPORTS${_flag_})
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${_flag_}")
        if(USE_WARNINGS)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_flag_}")
        endif()
    endif()

    if(C_SUPPORTS${_flag_})
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${_flag_}")
        if(USE_WARNINGS)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_flag_}")
        endif()
    endif()
endmacro()

# Add some warnings in debug mode
if(MSVC)
    add_warning("/W2")
else()
    # Basic set of warnings
    add_warning("-Wall")
    add_warning("-Wextra")
    # Initialization and convertion values
    add_warning("-Wuninitialized")
    add_warning("-Wconversion")
    add_warning("-Wsign-conversion")
    add_warning("-Wsign-promo")
    # C++11 functionalities
    add_warning("-Wsuggest-override")
    add_warning("-Wsuggest-final-types")
    # C++ standard conformance
    add_warning("-Wpedantic")
    add_warning("-pedantic")
    # The compiler is your friend
    add_warning("-Wdocumentation")
    add_warning("-Wdeprecated")
    add_warning("-Wextra-semi")
    add_warning("-Wshadow")
    add_warning("-Wnon-virtual-dtor")

    add_warning("-Wold-style-cast")
    add_warning("-Wcast-align")
    add_warning("-Wunused")
    add_warning("-Woverloaded-virtual")

    # Remove some warnings
    add_warning("-Wno-potentially-evaluated-expression")
endif()

CHECK_C_COMPILER_FLAG("-std=c99" COMPILER_SUPPORTS_C99)
if(COMPILER_SUPPORTS_C99)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

if(MSVC)
    add_definitions("/D COMPILER_IS_MSVC")
    add_definitions("/D NOMINMAX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
else()
    if(USE_WARNINGS)
        # Make sure to have asserts compiled
        add_definitions("-UNDEBUG")
    endif()
endif()
