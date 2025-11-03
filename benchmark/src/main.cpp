#include "main.hpp"

#include <benchmark/benchmark.h>

#include <SHA256.h>
#include <iostream>

static void BM_sha256(benchmark::State &state)
{
    // Perform setup here
    while (state.KeepRunning())
    {
        uint64_t iteration = state.iterations();
        test(iteration);
    }
}

std::string msg("This is IN2029 formative task");

static std::pair<int, uint64_t> test(uint64_t nonce)
{
    SHA256 sha;

    std::string message(msg);
    message.append(std::to_string(nonce));
    sha.update(message);

    std::string hash = SHA256::toString(sha.digest());

    int zeros = 0;
    for (char &c : hash)
    {
        if (c == '0')
        {

            zeros++;
        }
        else
        {
            break;
        }
    }
    // printf("HASH: %d (%s)\n", zeros, hash.c_str());

    return std::pair<int, uint64_t>(zeros, nonce);
}

// Register the function as a benchmark
BENCHMARK(BM_sha256)->Iterations(ITERATIONS);
BENCHMARK(BM_sha256)->Iterations(ITERATIONS)->Threads(2);
BENCHMARK(BM_sha256)->Iterations(ITERATIONS)->Threads(4);
BENCHMARK(BM_sha256)->Iterations(ITERATIONS)->Threads(8);
BENCHMARK(BM_sha256)->Iterations(ITERATIONS)->Threads(16);
// Run the benchmark
BENCHMARK_MAIN();