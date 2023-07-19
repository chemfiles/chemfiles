set(OLD_CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake/CheckCompilerFlags")
# import CheckCompilerFlag from cmake 3.19, and make sure it runs with older
# versions of cmake as well
include(CheckCompilerFlags/CheckCompilerFlag)
set(CMAKE_MODULE_PATH "${OLD_CMAKE_MODULE_PATH}")

set(CMAKE_REQUIRED_QUIET YES)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(CMAKE_SHARED_LINKER_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

if(${EMSCRIPTEN})
    if("${EMSCRIPTEN_VERSION}" VERSION_LESS "3")
        message(FATAL_ERROR "emscripten ${EMSCRIPTEN_VERSION} is not supported, chemfiles requires version 3 or later")
    endif()

    set(CMAKE_CXX_EXTENSIONS ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1")

    find_program(NODE_JS_EXECUTABLE NAMES nodejs node)
    if(NODE_JS_EXECUTABLE)
        set(CMAKE_CROSSCOMPILING_EMULATOR ${NODE_JS_EXECUTABLE})
    endif()
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "PGI")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 18)
        message(WARNING "Only PGI >= 18 was tested to be able to compile chemfiles")
    endif()
endif()

macro(add_warning_flag _flag_)
    string(REPLACE "++" "xx" _cleaned_flag_ ${_flag_})
    CHECK_COMPILER_FLAG(CXX "${_flag_}" CXX_SUPPORTS${_cleaned_flag_})
    CHECK_COMPILER_FLAG(C "${_flag_}" CC_SUPPORTS${_cleaned_flag_})
    if(CXX_SUPPORTS${_cleaned_flag_})
        set(CHEMFILES_CXX_WARNINGS "${CHEMFILES_CXX_WARNINGS} ${_flag_}")
    endif()

    if(CC_SUPPORTS${_cleaned_flag_})
        set(CHEMFILES_C_WARNINGS "${CHEMFILES_C_WARNINGS} ${_flag_}")
    endif()
endmacro()

set(CHEMFILES_CXX_WARNINGS "")
set(CHEMFILES_C_WARNINGS "")

macro(remove_msvc_warning _warn_)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${_warn_}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd${_warn_}")
endmacro()

macro(remove_intel_warning _warn_)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -diag-disable ${_warn_}")
endmacro()

if(MSVC)
    if(${CMAKE_CXX_FLAGS} MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/Wall" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
    endif()

    if(${CMAKE_C_FLAGS} MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/Wall" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall")
    endif()

    if (${MSVC_TOOLSET_VERSION} GREATER_EQUAL 141)
        # supported since MVSC 15.6
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:external /external:anglebrackets /external:W0")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /experimental:external /external:anglebrackets /external:W0")
    endif()

    # Disable other warnings
    remove_msvc_warning(4061) # enumerator in switch of enum is not explicitly handled by a case label
    remove_msvc_warning(4127) # conditional expression is constant
    remove_msvc_warning(4275) # non-dll-export interface as base class of dll-export class
    remove_msvc_warning(4251) # class <> needs to be dll-export
    remove_msvc_warning(4355) # 'this' used in based member initializer list
    remove_msvc_warning(4503) # mangled name too long was truncated
    remove_msvc_warning(4514) # unreferenced inline function has been removed
    remove_msvc_warning(4571) # catch(...) semantics changed since Visual C++ 7.1;
    remove_msvc_warning(4582) # constructor is not implicitly called
    remove_msvc_warning(4583) # destructor is not implicitly called
    remove_msvc_warning(4623) # default constructor was implicitly defined as deleted
    remove_msvc_warning(4625) # copy constructor was implicitly defined as deleted
    remove_msvc_warning(4626) # assignment operator was implicitly defined as deleted
    remove_msvc_warning(4628) # digraph <: not supported with -Ze (fires with template<::namespace>)
    remove_msvc_warning(4643) # forward declaring 'optional' in namespace std is not permitted by the C++ Standard
    remove_msvc_warning(4668) # not defined preprocessor macro, replacing with '0' for '#if/#elif'
    remove_msvc_warning(4627) # move assignment operator was implicitly defined as deleted
    remove_msvc_warning(4710) # function not inlined
    remove_msvc_warning(4711) # function selected for automatic inlining
    remove_msvc_warning(4820) # padding added
    remove_msvc_warning(4868) # compiler may not enforce left-to-right evaluation order in braced initializer list
    remove_msvc_warning(5026) # move constructor was implicitly defined as deleted
    remove_msvc_warning(5027) # move assignment operator was implicitly defined as deleted
    remove_msvc_warning(5039) # pointer or reference to potentially throwing function passed to extern C function under -EHc.
    remove_msvc_warning(5045) # Compiler will insert Spectre mitigation for memory load

    # Noisy warnings from external code that are checked by clang/gcc
    remove_msvc_warning(4018) # operator >=, signed/unsigned mismatch
    remove_msvc_warning(4100) # unreferenced formal parameter
    remove_msvc_warning(4365) # conversion from '...' to '...', signed/unsigned mismatch
    remove_msvc_warning(4774) # format string is not a string literal
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
    add_warning_flag("-Wsuggest-final-types")
    add_warning_flag("-Wsuggest-final-methods")
    # C and C++ standard conformance
    add_warning_flag("-Wpedantic")
    add_warning_flag("-pedantic")
    # The compiler is your friend
    add_warning_flag("-Wdocumentation")
    add_warning_flag("-Wdeprecated")
    add_warning_flag("-Wextra-semi")
    add_warning_flag("-Wextra-semi-stmt")
    add_warning_flag("-Wnon-virtual-dtor")
    add_warning_flag("-Wold-style-cast")
    add_warning_flag("-Wcast-align=strict")
    add_warning_flag("-Wcast-qual")
    add_warning_flag("-Wunused")
    add_warning_flag("-Woverloaded-virtual")
    add_warning_flag("-Wundefined-func-template")
    add_warning_flag("-Wmissing-prototypes")
    add_warning_flag("-Wmissing-variable-declarations")
    add_warning_flag("-Waggressive-loop-optimizations")
    add_warning_flag("-Wc99-c11-compat")
    add_warning_flag("-Wc++-compat")
    add_warning_flag("-Wduplicated-branches")
    add_warning_flag("-Wduplicated-cond")
    add_warning_flag("-Wjump-misses-init")
    add_warning_flag("-Wlogical-op")
    add_warning_flag("-Wundef")
    add_warning_flag("-Wuseless-cast")
    add_warning_flag("-Wmissing-prototypes")
    add_warning_flag("-Wmissing-declarations")
    add_warning_flag("-Wmultiple-inheritance")
    add_warning_flag("-Wconditionally-supported")
    add_warning_flag("-Wzero-as-null-pointer-constant")
    add_warning_flag("-Winconsistent-missing-destructor-override")

    add_warning_flag("-Wsuggest-attribute=pure")
    add_warning_flag("-Wsuggest-attribute=const")
    add_warning_flag("-Wsuggest-attribute=noreturn")

    # This list was last updated with GCC 8 warnings
    # see https://github.com/barro/compiler-warnings for a list of all warnings

    add_warning_flag("-Weverything")
    # Disable some warning implied by -Weverything
    add_warning_flag("-Wno-c++-compat")
    add_warning_flag("-Wno-c++98-compat")
    add_warning_flag("-Wno-c++98-compat-pedantic")
    add_warning_flag("-Wno-weak-vtables")
    add_warning_flag("-Wno-weak-template-vtables")
    add_warning_flag("-Wno-padded")
    add_warning_flag("-Wno-float-equal")
    add_warning_flag("-Wno-switch-enum")
    add_warning_flag("-Wno-documentation-unknown-command")
    add_warning_flag("-Wno-exit-time-destructors")
    add_warning_flag("-Wno-global-constructors")
    # This warning trigger when compiling for 64-bit,
    # but the code is relevant for 32-bit
    add_warning_flag("-Wno-tautological-type-limit-compare")
    add_warning_flag("-Wno-useless-cast")
    # Not everyone is as smart as clang for code reachability
    add_warning_flag("-Wno-covered-switch-default")
    add_warning_flag("-Wno-unreachable-code-break")

    # Warnings for PGI compiler
    add_warning_flag("-Minform=warn")

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
        # external function definition with no prior declaration
        remove_intel_warning(1418)
        # Intel compiler is too strict in errors about 'explicit' keyword
        remove_intel_warning(2304)
        remove_intel_warning(2305)
        # parameter "args" was never referenced in variadic templates
        remove_intel_warning(869)
        # exception specification for implicitly declared virtual function ...
        # This is an issue with compiler generated destructors and noexcept
        remove_intel_warning(811)
        remove_intel_warning(809)
    endif()
endif()

set(CHFL_SANITIZER "none" CACHE STRING "Sanitizer (clang and gcc only) to use in the build")
set_property(CACHE CHFL_SANITIZER PROPERTY STRINGS none address memory undefined thread)

set(CHEMFILES_SANITIZERS "")
set(CHEMFILES_SANITIZERS "")

if(${CHFL_SANITIZER} STREQUAL "none")
    # Nothing to do
elseif(${CHFL_SANITIZER} STREQUAL "undefined")
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -fsanitize=undefined")
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -fsanitize=integer")
else()
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -fsanitize=${CHFL_SANITIZER}")
endif()

if(NOT ${CHFL_SANITIZER} STREQUAL "none")
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -fno-omit-frame-pointer")
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -g")
endif()
