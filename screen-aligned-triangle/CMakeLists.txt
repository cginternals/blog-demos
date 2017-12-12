cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

include(cmake/Custom.cmake)

set_policy(CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.


# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project("Screen-Aligned Triangle" C CXX)
set(target screen_aligned_triangle)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)

# Exit if using Mac OS
if (APPLE)
    message("${target} skipped: Mac OS does not support OpenGL 4.2")
    return()
endif()

# Exit if required dependencies are not met
if (NOT glfw3_FOUND)
    message("${target} skipped: glfw3 not found")
    return()
endif()

message(STATUS "${target}")

include_directories(
    ${TEMPLATE_INCLUDES}
    ${CMAKE_SOURCE_DIR}/source
)

set(libs
    ${TEMPLATE_LIBRARIES}
)

set(DATA "${CMAKE_SOURCE_DIR}/data")
set(SOURCE "${CMAKE_SOURCE_DIR}/source")
set(sources
    ${SOURCE}/main.cpp

    ${SOURCE}/cgutils.h
    ${SOURCE}/cgutils.cpp
    ${SOURCE}/screenalignedtriangle.h
    ${SOURCE}/screenalignedtriangle.cpp

    ${DATA}/benchmark-minimal.frag
    ${DATA}/record.frag
    ${DATA}/record.geom
    ${DATA}/record.vert
    ${DATA}/record-empty.vert
    ${DATA}/replay.frag
    ${DATA}/replay.vert
)


include(cmake/CompileOptions.cmake)

add_executable(${target} ${sources})

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/source
    SYSTEM
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    glfw
    glm
    glbinding::glbinding
)

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
    PUBLIC
    GLFW_INCLUDE_NONE
    GLM_FORCE_RADIANS
)

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)