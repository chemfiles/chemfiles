option(RUN_TESTS "Run harp tests." OFF)

function(harp_cpp_test _name_)
    add_executable(${_name_} ${PROJECT_SOURCE_DIR}/tests/${_name_}.cpp)
    target_link_libraries(${_name_} harp)
    set_property(TARGET ${_name_} PROPERTY CXX_STANDARD 11)
    add_test(${_name_} ${_name_})
endfunction()

function(harp_c_test _name_)
    add_executable(${_name_} ${PROJECT_SOURCE_DIR}/tests/${_name_}.c)
    target_link_libraries(${_name_} harp)
    add_test(${_name_} ${_name_})
endfunction()

if(RUN_TESTS)
    include_directories(${CMAKE_SOURCE_DIR}/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})

    harp_cpp_test(logging)
    harp_cpp_test(format_factory)
    harp_c_test(capi)
endif()
