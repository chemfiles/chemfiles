
option(BUILD_DOCUMENTATION "Build the documentation." OFF)

if(BUILD_DOCUMENTATION)

  find_package(Sphinx REQUIRED)
  find_package(Doxygen REQUIRED)

  configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
  configure_file(${CMAKE_SOURCE_DIR}/doc/conf.in.py ${CMAKE_BINARY_DIR}/doc/conf.py @ONLY)

  add_custom_target( doxygendoc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
  )

  set(SPHINX_CONF_DIR ${CMAKE_BINARY_DIR}/doc)
  set(SPHINX_INPUT_DIR ${CMAKE_SOURCE_DIR}/doc)
  set(SPHINX_DESTINATION ${CMAKE_BINARY_DIR}/doc)
  set(SPHINX_DEPENDENCIES doxygendoc)

  set(sphinx_target_base_name doc)
  Sphinx_add_targets(${sphinx_target_base_name} ${SPHINX_CONF_DIR} ${SPHINX_INPUT_DIR} ${SPHINX_DESTINATION} ${SPHINX_DEPENDENCIES})

  if( ${SPHINX_HTML_OUTPUT} )
    install( DIRECTORY ${SPHINX_DESTINATION}/html
      DESTINATION share/chemharp/doc
      COMPONENT Doc
      PATTERN "${SPHINX_DESTINATION}/html/*"
      )
  endif()

  if( ${SPHINX_LATEX_OUTPUT} )
    set( DOC_WITH_LOCAL_DOXYGEN OFF )

    # Build the PDF with pdflatex
    find_package(LATEX)
    if( NOT PDFLATEX_COMPILER )
      message("pdflatex compiler was not found. Please pass to advanced mode and provide its full path")
    else()
      # Needs to be executed twice to get table of contents.
      add_custom_command( TARGET ${sphinx_target_base_name}_latex
        POST_BUILD
        COMMAND ${PDFLATEX_COMPILER}
          ${SPHINX_DESTINATION}/latex/Chemharp.tex
          -output-directory ${SPHINX_DESTINATION}/latex
        COMMAND ${MAKEINDEX_COMPILER}
            Chemharp.idx
        COMMAND ${PDFLATEX_COMPILER}
          ${SPHINX_DESTINATION}/latex/Chemharp.tex
          -output-directory ${SPHINX_DESTINATION}/latex
        WORKING_DIRECTORY ${SPHINX_DESTINATION}/latex
        COMMENT "Building PDF"
        )
    endif()
  endif()

endif()
