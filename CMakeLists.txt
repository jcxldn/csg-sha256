cmake_minimum_required(VERSION 3.14...3.31 FATAL_ERROR) # FetchContent

project(sha256_hash_challenge)

include(FetchContent)

FetchContent_Declare(
  sha256
  GIT_REPOSITORY https://github.com/System-Glitch/SHA256
  GIT_TAG        9418899d53798805b029d20cd76ca91d7b25e2a4
)

FetchContent_MakeAvailable(sha256)

add_executable(runner
    src/main.cpp
)
#target_include_directories(sha256-example PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(runner sha256 -pthread)