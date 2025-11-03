project(sha256_benchmark)

include(FetchContent)

FetchContent_Declare(
  google_benchmark
  GIT_REPOSITORY https://github.com/google/benchmark
  GIT_TAG        eddb0241389718a23a42db6af5f0164b6e0139af
)


set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_ASSEMBLY_TESTS OFF) # disable ASM tests (requires specific GCC versions)
FetchContent_MakeAvailable(google_benchmark)


add_executable(benchmark_runner
    src/main.cpp
)

target_link_libraries(benchmark_runner benchmark::benchmark sha256)