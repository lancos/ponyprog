# Disallow in-source build
STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" PONYPROG_IN_SOURCE)
IF(PONYPROG_IN_SOURCE)
    MESSAGE(FATAL_ERROR "Ponyprog requires an out of source build. Please create a separate build directory and run 'cmake path_to_ponyprog [options]' there.")
ENDIF(PONYPROG_IN_SOURCE)

