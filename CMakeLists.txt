cmake_minimum_required(VERSION 3.14...3.31 FATAL_ERROR) # FetchContent

project(sha256_hash_challenge)

# file lock lib, otherwise C++11
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  sha256
  GIT_REPOSITORY https://github.com/System-Glitch/SHA256
  GIT_TAG        9418899d53798805b029d20cd76ca91d7b25e2a4
)

FetchContent_Declare(
  file_lock
  GIT_REPOSITORY https://github.com/KaganCanSit/Cross-Compatible-FileLock-Windows-and-Linux
  GIT_TAG        42ffdc24b06a85383e1b7a20e09931d7849533e7
)


FetchContent_MakeAvailable(sha256 file_lock)

add_library(file_lock_lib INTERFACE)
target_include_directories(file_lock_lib INTERFACE ${file_lock_SOURCE_DIR}/include)


find_package(MPI REQUIRED)

add_executable(runner
    src/main.cpp
)
#target_include_directories(sha256-example PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(runner MPI::MPI_CXX sha256 file_lock_lib -pthread)

target_compile_options(runner PRIVATE
  -Wall -Wextra -Wpedantic -Wstrict-aliasing
  -Wcast-align -Wmissing-declarations
  -Wpointer-arith -Wcast-qual
  -Wnon-virtual-dtor -Wold-style-cast
  -Wshadow -Werror
  -Wno-missing-field-initializers
)

#-Wextra-semi