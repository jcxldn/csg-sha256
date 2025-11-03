set(RUNNER_SOURCE_FILES src/main.cpp)
get_target_property(BENCHMARK_INCLUDES benchmark INCLUDE_DIRECTORIES)


macro(register_runner suffix flags )
    set(RUNNER_TARGET benchmark_runner_${suffix})


    separate_arguments(RUNNER_TARGET_FLAGS UNIX_COMMAND "${flags}")
    add_library(${RUNNER_TARGET}_flags INTERFACE)

    target_compile_options(${RUNNER_TARGET}_flags INTERFACE "${RUNNER_TARGET_FLAGS}")


    #region Google Benchmark trickery
    # Create a "clone" of the google benchmark target so that we can add our target flags directly
    # instead of using the CMake defaults when the actual benchmark target (a static library) is compiled
    # Note that we cannot use TARGET_OBJECTS here as that is post-compilation
    add_library(${RUNNER_TARGET}_benchmark STATIC
        $<TARGET_PROPERTY:benchmark,SOURCES>
    )
    target_include_directories(${RUNNER_TARGET}_benchmark PUBLIC ${BENCHMARK_INCLUDES})
    target_compile_options(${RUNNER_TARGET}_benchmark PRIVATE "${RUNNER_TARGET_FLAGS}")
    set_target_properties(${RUNNER_TARGET}_benchmark PROPERTIES FOLDER "Benchmarking/Internal")

    #endregion

    add_executable(${RUNNER_TARGET}
        ${RUNNER_SOURCE_FILES}
    )

    target_link_libraries(${RUNNER_TARGET} PUBLIC ${RUNNER_TARGET}_flags PRIVATE sha256 ${RUNNER_TARGET}_benchmark)
    set_target_properties(${RUNNER_TARGET} PROPERTIES FOLDER "Benchmarking")

endmacro()