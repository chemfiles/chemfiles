if(${CMAKE_CXX_COMPILER_ID} MATCHES "PGI")
    # Remove IPA optimization, as it fails with the error:
    #           'unknown variable reference: &2&2821'
    #
    # The default value of both variables are "-fast -O3 -Mipa=fast -DNDEBUG"
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "-fast -O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE_INIT "-fast -O3 -DNDEBUG")
endif()
