cmake_minimum_required(VERSION 3.4...3.31 FATAL_ERROR) # USES_TERMINAL_BUILD introduced in 3.4 (third_party/px4.cmake)

project(sha256_hash_challenge)

include(ExternalProject)

ExternalProject_Add(libsha256
    GIT_REPOSITORY    https://github.com/System-Glitch/SHA256
    GIT_TAG           9418899d53798805b029d20cd76ca91d7b25e2a4
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD  TRUE
    USES_TERMINAL_INSTALL TRUE
    STEP_TARGETS   build
)

ExternalProject_Get_property(libsha256 BINARY_DIR)
ExternalProject_Get_property(libsha256 SOURCE_DIR)

add_library(sha256 STATIC IMPORTED)
set_target_properties(sha256 PROPERTIES IMPORTED_LOCATION ${BINARY_DIR}/libsha256.a)
target_include_directories(sha256 INTERFACE "${SOURCE_DIR}/include")


add_executable(runner
    src/main.cpp
)
#target_include_directories(sha256-example PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(runner sha256)