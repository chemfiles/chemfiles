
enable_language(Fortran)
add_library(chemharpf SHARED chemharp.f90)
target_link_libraries(chemharpf chemharp)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/chemharp.mod DESTINATION include)
install(TARGETS chemharpf DESTINATION lib)
