# - Check if the source code provided in the SOURCE argument compiles.
# CHECK_FORTRAN_SOURCE_COMPILES(SOURCE VAR)
# - macro which checks if the source code compiles
#  SOURCE   - source code to try to compile
#  VAR      - variable to store whether the source code compiled
#
# The following variables may be set before calling this macro to
# modify the way the check is run:
#
#  CMAKE_REQUIRED_FLAGS = string of compile command line flags
#  CMAKE_REQUIRED_DEFINITIONS = list of macros to define (-DFOO=bar)
#  CMAKE_REQUIRED_INCLUDES = list of include directories
#  CMAKE_REQUIRED_LIBRARIES = list of libraries to link

if(${CMAKE_VERSION} VERSION_GREATER 3.1.0)
    cmake_policy(SET CMP0054 OLD)
endif()

MACRO(CHECK_FORTRAN_SOURCE_COMPILES SOURCE VAR)
  IF("${VAR}" MATCHES "^${VAR}$")
    SET(MACRO_CHECK_FUNCTION_DEFINITIONS
      "-D${VAR} ${CMAKE_REQUIRED_FLAGS}")
    IF(CMAKE_REQUIRED_LIBRARIES)
      SET(CHECK_FORTRAN_SOURCE_COMPILES_ADD_LIBRARIES
        "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    ELSE(CMAKE_REQUIRED_LIBRARIES)
      SET(CHECK_FORTRAN_SOURCE_COMPILES_ADD_LIBRARIES)
    ENDIF(CMAKE_REQUIRED_LIBRARIES)
    IF(CMAKE_REQUIRED_INCLUDES)
      SET(CHECK_FORTRAN_SOURCE_COMPILES_ADD_INCLUDES
        "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_REQUIRED_INCLUDES}")
    ELSE(CMAKE_REQUIRED_INCLUDES)
      SET(CHECK_FORTRAN_SOURCE_COMPILES_ADD_INCLUDES)
    ENDIF(CMAKE_REQUIRED_INCLUDES)
    FILE(WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.f90"
      "${SOURCE}\n")

    MESSAGE(STATUS "Performing Test ${VAR}")
    TRY_COMPILE(${VAR}
      ${CMAKE_BINARY_DIR}
      ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.f90
      COMPILE_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
      CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FUNCTION_DEFINITIONS}
      "${CHECK_FORTRAN_SOURCE_COMPILES_ADD_LIBRARIES}"
      "${CHECK_FORTRAN_SOURCE_COMPILES_ADD_INCLUDES}"
      OUTPUT_VARIABLE OUTPUT)
    IF(${VAR})
      SET(${VAR} 1 CACHE INTERNAL "Test ${VAR}")
      MESSAGE(STATUS "Performing Test ${VAR} - Success")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
        "Performing Fortran SOURCE FILE Test ${VAR} succeded with the following output:\n"
        "${OUTPUT}\n"
        "Source file was:\n${SOURCE}\n")
    ELSE(${VAR})
      MESSAGE(STATUS "Performing Test ${VAR} - Failed")
      SET(${VAR} 0 CACHE INTERNAL "Test ${VAR}")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
        "Performing Fortran SOURCE FILE Test ${VAR} failed with the following output:\n"
        "${OUTPUT}\n"
        "Source file was:\n${SOURCE}\n")
    ENDIF(${VAR})
  ENDIF("${VAR}" MATCHES "^${VAR}$")
ENDMACRO(CHECK_FORTRAN_SOURCE_COMPILES)
