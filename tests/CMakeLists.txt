option(BUILD_TESTS "Build harp tests." OFF)

function(harp_cpp_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)
    add_executable(${_name_} ${_file_} $<TARGET_OBJECTS:catch>)
    target_link_libraries(${_name_} harp)
    set_property(TARGET ${_name_} PROPERTY CXX_STANDARD 11)
    add_test(${_name_} ${_name_})
endfunction()

function(harp_c_test _name_)
    add_executable(${_name_} ${PROJECT_SOURCE_DIR}/tests/${_name_}.c)
    target_link_libraries(${_name_} harp)
    add_test(${_name_} ${_name_})
endfunction()

if(BUILD_TESTS)
    include_directories(${CMAKE_SOURCE_DIR}/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/catch)
    add_library(catch OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/catch/catch.cpp)

    file(GLOB all_test_files
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

    add_definitions("-DSRCDIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

    foreach(test_file IN LISTS all_test_files)
        harp_cpp_test(${test_file})
    endforeach(test_file)

    harp_c_test(capi)
endif()
