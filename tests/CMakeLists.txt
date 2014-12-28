option(RUN_TESTS "Run harp tests." OFF)

function(harp_test _name_)
    add_executable(${_name_} ${PROJECT_SOURCE_DIR}/tests/${_name_}.cpp)
    target_link_libraries(${_name_} gtestmain)
    target_link_libraries(${_name_} harp)
    add_test(${_name_} ${_name_})
endfunction()

if(RUN_TESTS)
    include(GoogleTest)
    include_directories(${CMAKE_SOURCE_DIR}/include)

    harp_test(logging)
endif()
