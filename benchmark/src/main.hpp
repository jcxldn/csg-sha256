#ifndef BENCHMARK_MAIN_HPP_
#define BENCHMARK_MAIN_HPP_

#include <utility> // std::pair
#include <cstdint> // uint64_t

#define ITERATIONS 1'000'000 // 1 million

static std::pair<int, uint64_t> test(uint64_t nonce);

#endif /* BENCHMARK_MAIN_HPP_ */