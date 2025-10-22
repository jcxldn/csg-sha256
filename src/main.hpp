#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <utility>

#include "FileLockFactory.hpp"

bool does_base_exist();
bool does_output_exist();

uint64_t handle_base(file_lock::FileLockContext *lock);
uint64_t get_base();

void handle_log(int machine_id, int thread_id, int zeros, uint64_t nonce, std::string message, std::string hash, file_lock::FileLockContext *lock);
void log(int machine_id, int thread_id, int zeros, uint64_t nonce);

std::pair<int, uint64_t> test(uint64_t nonce);

void task(int min_zeros, int machine_id, int thread_no);

void dbg_task(int min_zeros, int machine_id);

#endif /* MAIN_HPP_ */