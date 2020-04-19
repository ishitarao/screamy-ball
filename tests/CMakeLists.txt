get_filename_component(CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../" ABSOLUTE)
include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS
        "${FinalProject_SOURCE_DIR}/tests/*.h"
        "${FinalProject_SOURCE_DIR}/tests/*.hpp"
        "${FinalProject_SOURCE_DIR}/tests/*.cc"
        "${FinalProject_SOURCE_DIR}/tests/*.cpp")


ci_make_app(
        APP_NAME    test
        CINDER_PATH ${CINDER_PATH}
        SOURCES     ${SOURCE_LIST}
        LIBRARIES   mylibrary catch2
        BLOCKS
)

target_compile_features(test PRIVATE cxx_std_14)

# Cross-platform compiler lints
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
        OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test PRIVATE
            -Wall
            -Wextra
            -Wswitch
            -Wconversion
            -Wparentheses
            -Wfloat-equal
            -Wzero-as-null-pointer-constant
            -Wpedantic
            -pedantic
            -pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    cmake_policy(SET CMP0015 NEW)
    set_property(TARGET test APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:CONSOLE")
    target_compile_options(test PRIVATE
            /W3)
endif ()
