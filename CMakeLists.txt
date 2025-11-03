cmake_minimum_required(VERSION 3.14...3.31 FATAL_ERROR) # FetchContent

project(sha256_hash_challenge)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(USE_FOLDERS ON)  # Housekeeping for generators that support it

include(FetchContent)

FetchContent_Declare(
  sha256
  GIT_REPOSITORY https://github.com/System-Glitch/SHA256
  GIT_TAG        9418899d53798805b029d20cd76ca91d7b25e2a4
)


FetchContent_MakeAvailable(sha256)

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