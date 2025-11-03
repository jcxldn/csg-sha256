cmake_minimum_required(VERSION 3.14...3.31 FATAL_ERROR) # FetchContent

project(sha256_hash_challenge)

set(CMAKE_CXX_STANDARD 17) # google-benchmark (specified as we recreate it's static library target)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(USE_FOLDERS ON)  # Housekeeping for generators that support it

set(CMAKE_AR "gcc-ar") # required for our HPC env

set(OPTIMIZED_COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -march=native -fno-math-errno -fno-trapping-math -flto -ffat-lto-objects -fuse-linker-plugin")  # from release-optimized benchmark

include(FetchContent)

FetchContent_Declare(
  sha256
  GIT_REPOSITORY https://github.com/System-Glitch/SHA256
  GIT_TAG        9418899d53798805b029d20cd76ca91d7b25e2a4
)


FetchContent_MakeAvailable(sha256)


separate_arguments(SHA256_COMPILE_FLAGS_BASE UNIX_COMMAND "${CMAKE_CXX_FLAGS_RELEASE}")
target_compile_options(sha256 PUBLIC ${OPTIMIZED_COMPILE_FLAGS})

find_package(MPI REQUIRED)

add_executable(runner
    src/main.cpp
)
#target_include_directories(sha256-example PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(runner MPI::MPI_CXX sha256 -pthread)


target_compile_options(runner PRIVATE
  -Wall -Wextra -Wpedantic -Wstrict-aliasing
  -Wcast-align -Wmissing-declarations
  -Wpointer-arith -Wcast-qual
  -Wnon-virtual-dtor -Wold-style-cast
  -Wshadow -Werror
  -Wno-missing-field-initializers
)

#-Wextra-semi

add_subdirectory(benchmark)