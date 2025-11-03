set(RUNNER_SOURCE_FILES src/main.cpp)
get_target_property(BENCHMARK_INCLUDES benchmark INCLUDE_DIRECTORIES)
get_target_property(SHA256_INCLUDES sha256 INCLUDE_DIRECTORIES)
get_target_property(SHA256_SOURCE_DIR sha256 SOURCE_DIR)


add_custom_target(benchmark_runner_all)


macro(register_runner suffix flags )
    set(RUNNER_TARGET benchmark_runner_${suffix})
    separate_arguments(RUNNER_TARGET_FLAGS UNIX_COMMAND "${flags}")


    #region Google Benchmark / SHA256 trickery
    # Create a "clone" of the google benchmark target so that we can add our target flags directly
    # instead of using the CMake defaults when the actual benchmark target (a static library) is compiled
    # Note that we cannot use TARGET_OBJECTS here as that is post-compilation
    add_library(${RUNNER_TARGET}_benchmark STATIC
        $<TARGET_PROPERTY:benchmark,SOURCES>
    )
    target_include_directories(${RUNNER_TARGET}_benchmark PUBLIC ${BENCHMARK_INCLUDES})
    target_compile_options(${RUNNER_TARGET}_benchmark PRIVATE "${RUNNER_TARGET_FLAGS}")
    set_target_properties(${RUNNER_TARGET}_benchmark PROPERTIES FOLDER "Benchmarking/Internal")


    add_library(${RUNNER_TARGET}_sha256 STATIC
        ${SHA256_SOURCE_DIR}/src/SHA256.cpp
    )
    target_include_directories(${RUNNER_TARGET}_sha256 PUBLIC ${SHA256_INCLUDES})
    target_compile_options(${RUNNER_TARGET}_sha256 PRIVATE "${RUNNER_TARGET_FLAGS}")
    set_target_properties(${RUNNER_TARGET}_sha256 PROPERTIES FOLDER "Benchmarking/Internal")

    #endregion

    add_executable(${RUNNER_TARGET}
        ${RUNNER_SOURCE_FILES}
    )

    target_link_libraries(${RUNNER_TARGET} PRIVATE ${RUNNER_TARGET}_sha256 ${RUNNER_TARGET}_benchmark -pthread)
    target_compile_options(${RUNNER_TARGET} PUBLIC "${RUNNER_TARGET_FLAGS}")
    set_target_properties(${RUNNER_TARGET} PROPERTIES FOLDER "Benchmarking")


    add_dependencies(benchmark_runner_all ${RUNNER_TARGET})

endmacro()