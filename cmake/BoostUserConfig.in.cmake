file(WRITE
    @source_dir@/tools/build/src/user-config.jam
    "using @B2_TOOLSET@ : cmake : @CMAKE_CXX_COMPILER@ ;\n"
)
