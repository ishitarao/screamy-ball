get_filename_component(CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../" ABSOLUTE)
include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

file(GLOB RESOURCE_LIST CONFIGURE_DEPENDS
        "${CINDER_PATH}/blocks/PretzelGui/resources/**"
        )

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS
        "${FinalProject_SOURCE_DIR}/apps/*.h"
        "${FinalProject_SOURCE_DIR}/apps/*.hpp"
        "${FinalProject_SOURCE_DIR}/apps/*.cc"
        "${FinalProject_SOURCE_DIR}/apps/*.cpp")


ci_make_app(
    APP_NAME    cinder-screamy-ball
    CINDER_PATH ${CINDER_PATH}
    SOURCES     ${SOURCE_LIST}
    LIBRARIES   screamy-ball gflags
    BLOCKS      ciSpeech PretzelGui
    RESOURCES   ${RESOURCE_LIST}
)

target_compile_features(cinder-screamy-ball PRIVATE cxx_std_14)

# Cross-platform compiler lints
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
        OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(cinder-screamy-ball PRIVATE
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
    target_compile_options(cinder-screamy-ball PRIVATE
            /W3)
endif ()
