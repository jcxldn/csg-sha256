project(sha256_benchmark)

include(FetchContent)

FetchContent_Declare(
  google_benchmark
  GIT_REPOSITORY https://github.com/google/benchmark
  GIT_TAG        eddb0241389718a23a42db6af5f0164b6e0139af
)


set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_ASSEMBLY_TESTS OFF) # disable ASM tests (requires specific GCC versions)

set(CMAKE_FOLDER "Benchmarking/Dependencies")
FetchContent_MakeAvailable(google_benchmark)
unset(CMAKE_FOLDER)

# Include macro definition
include(register_runner.cmake)


register_runner(debug ${CMAKE_CXX_FLAGS_DEBUG})

register_runner(release ${CMAKE_CXX_FLAGS_RELEASE})
register_runner(release_native "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
register_runner(release_optimized ${OPTIMIZED_COMPILE_FLAGS})

register_runner(o2 "-DNDEBUG -O2")
register_runner(os "-DNDEBUG -Os")