include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

set(CMAKE_REQUIRED_QUIET YES)

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

CHECK_C_COMPILER_FLAG("-std=c99" COMPILER_SUPPORTS_C99)
if(COMPILER_SUPPORTS_C99)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

if(MSVC)
    add_definitions("/D NOMINMAX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(CMAKE_SHARED_LINKER_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

macro(add_warning_flag _flag_)
    CHECK_CXX_COMPILER_FLAG("${_flag_}" CXX_SUPPORTS${_flag_})
    CHECK_C_COMPILER_FLAG("${_flag_}" CC_SUPPORTS${_flag_})
    if(CXX_SUPPORTS${_flag_})
        set(CHEMFILES_CXX_WARNINGS "${CHEMFILES_CXX_WARNINGS} ${_flag_}")
    endif()

    if(CC_SUPPORTS${_flag_})
        set(CHEMFILES_C_WARNINGS "${CHEMFILES_C_WARNINGS} ${_flag_}")
    endif()
endmacro()

set(CHEMFILES_CXX_WARNINGS "")
set(CHEMFILES_C_WARNINGS "")

macro(remove_msvc_warning _warn_)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${_warn_}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd${_warn_}")
endmacro()

if(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/Wall" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
    endif()

    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/Wall" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall")
    endif()

    # Disable other warnings
    remove_msvc_warning(4061) # enumerator in switch of enum is not explicitly handled by a case label
    remove_msvc_warning(4275) # non-dll-export interface as base class of dll-export class
    remove_msvc_warning(4251) # class <> needs to be dll-export
    remove_msvc_warning(4514) # unreferenced inline function has been removed
    remove_msvc_warning(4582) # constructor is not implicitly called
    remove_msvc_warning(4583) # destructor is not implicitly called
    remove_msvc_warning(4623) # default constructor was implicitly defined as deleted
    remove_msvc_warning(4625) # copy constructor was implicitly defined as deleted
    remove_msvc_warning(4626) # assignment operator was implicitly defined as deleted
    remove_msvc_warning(4668) # not defined preprocessor macro, replacing with '0' for '#if/#elif'
    remove_msvc_warning(4627) # move assignment operator was implicitly defined as deleted
    remove_msvc_warning(4710) # function not inlined
    remove_msvc_warning(4711) # function selected for automatic inlining
    remove_msvc_warning(4820) # padding added
    remove_msvc_warning(5026) # move constructor was implicitly defined as deleted
    remove_msvc_warning(5027) # move assignment operator was implicitly defined as deleted
else()
    # Add some warnings in debug mode
    # Basic set of warnings
    add_warning_flag("-Wall")
    add_warning_flag("-Wextra")
    # Initialization and convertion values
    add_warning_flag("-Wuninitialized")
    add_warning_flag("-Wconversion")
    add_warning_flag("-Wsign-conversion")
    add_warning_flag("-Wsign-promo")
    # C++11 functionalities
    add_warning_flag("-Wsuggest-override")
    add_warning_flag("-Wsuggest-final-types")
    # C++ standard conformance
    add_warning_flag("-Wpedantic")
    add_warning_flag("-pedantic")
    # The compiler is your friend
    add_warning_flag("-Wdocumentation")
    add_warning_flag("-Wdeprecated")
    add_warning_flag("-Wextra-semi")
    add_warning_flag("-Wnon-virtual-dtor")
    add_warning_flag("-Wold-style-cast")
    add_warning_flag("-Wcast-align")
    add_warning_flag("-Wunused")
    add_warning_flag("-Woverloaded-virtual")
    add_warning_flag("-Wundefined-func-template")
    add_warning_flag("-Wmissing-prototypes")
    add_warning_flag("-Wmissing-variable-declarations")

    # Disable some strong warning with clang that are OK here
    add_warning_flag("-Wno-unknown-pragmas")
    add_warning_flag("-Wno-weak-vtables")
    add_warning_flag("-Wno-weak-template-vtables")
    add_warning_flag("-Wno-switch-enum")
    # We are not doing C here
    add_warning_flag("-Wno-padded")
    # Sometime this OK
    add_warning_flag("-Wno-float-equal")
    add_warning_flag("-Wno-double-promotion")
    # Yes, chemfiles uses globals
    add_warning_flag("-Wno-exit-time-destructors")
    add_warning_flag("-Wno-global-constructors")
    # Not everyone is as smart as clang for code reachability
    add_warning_flag("-Wno-covered-switch-default")
    add_warning_flag("-Wno-unreachable-code-break")

    if(${CMAKE_C_COMPILER} MATCHES "icc.*$")
        # Intel compiler is too strict in errors about 'explicit' keyword
        set(CHEMFILES_CXX_WARNINGS "${CHEMFILES_CXX_WARNINGS} -diag-disable 2304")
        set(CHEMFILES_CXX_WARNINGS "${CHEMFILES_CXX_WARNINGS} -diag-disable 2305")
        # 'parameter "args" was never referenced' => yes it was
        set(CHEMFILES_CXX_WARNINGS "${CHEMFILES_CXX_WARNINGS} -diag-disable 869")
    endif()
endif()

set(CHEMFILES_CXX_SANITIZERS "")
set(CHEMFILES_C_SANITIZERS "")

macro(add_sanitizer _flag_)
    CHECK_CXX_COMPILER_FLAG("${_flag_}" CXX_SUPPORTS${_flag_})
    CHECK_C_COMPILER_FLAG("${_flag_}" CC_SUPPORTS${_flag_})
    if(CXX_SUPPORTS${_flag_})
        set(CHEMFILES_CXX_SANITIZERS "${CHEMFILES_CXX_SANITIZERS} ${_flag_}")
    endif()

    if(CC_SUPPORTS${_flag_})
        set(CHEMFILES_C_SANITIZERS "${CHEMFILES_C_SANITIZERS} ${_flag_}")
    endif()
endmacro()

set(CHFL_SANITIZER "none" CACHE STRING "Sanitizer (clang and gcc only) to use in the build")
set_property(CACHE CHFL_SANITIZER PROPERTY STRINGS none all address memory undefined)

set(CHEMFILES_CXX_SANITIZERS "")
set(CHEMFILES_C_SANITIZERS "")

if(${CHFL_SANITIZER} STREQUAL "none")
    # Nothing to do
elseif(${CHFL_SANITIZER} STREQUAL "all")
    add_sanitizer("-fsanitize=address")
    add_sanitizer("-fsanitize=memory")
    add_sanitizer("-fsanitize=undefined")
    add_sanitizer("-fsanitize=integer")
elseif(${CHFL_SANITIZER} STREQUAL "undefined")
    add_sanitizer("-fsanitize=undefined")
    add_sanitizer("-fsanitize=integer")
else()
    add_sanitizer("-fsanitize=${CHFL_SANITIZER}")
endif()

if(NOT ${CHFL_SANITIZER} STREQUAL "none")
    add_sanitizer("-fno-omit-frame-pointer")
    add_sanitizer("-g")
endif()
