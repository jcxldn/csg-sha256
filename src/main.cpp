
#include <atomic>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <chrono>

#include "SHA256.h"

#include <inttypes.h>
#include <sys/stat.h>

#include <mpi.h>

#include "main.hpp"

std::atomic<int> max(0); // max zeros
std::atomic<uint64_t> max_nonce(0);

std::atomic<uint64_t> starting_base(0);
std::atomic<uint64_t> size(1000000); // 1 million
std::atomic<bool> done(false);

std::atomic<uint64_t> last_local_base(0);

std::string msg("This is IN2029 formative task");

std::string filename_results("output.csv");
std::string filename_base("base.txt");

MPI_Win win_base, win_log;
uint64_t base_buffer;

bool does_base_exist()
{
    struct stat buffer;
    return (stat(filename_base.c_str(), &buffer) == 0);
}

bool does_output_exist()
{
    struct stat buffer;
    return (stat(filename_results.c_str(), &buffer) == 0);
}

uint64_t get_base()
{

    uint64_t base;
    MPI_Get(&base, 1, MPI_UINT64_T, 0, 0, 1, MPI_UINT64_T, win_base);
    MPI_Win_fence(0, win_base);

    // append base
    uint64_t next_base = base + size.load();

    // write new file
    std::ofstream output;
    output.open(filename_base.c_str(), std::ios::out);
    output << std::to_string(next_base) << std::endl;
    output.close();

    MPI_Put(&next_base, 1, MPI_UINT64_T, 0, 0, 1, MPI_UINT64_T, win_base);
    MPI_Win_fence(0, win_base);

    return base;
}
void log(int thread_id, int zeros, uint64_t nonce)
{
    // operations that can be done pre-writing (ie calc message)
    // TODO make func instead of duping code 3x.
    std::string message(msg);
    message.append(std::to_string(nonce));
    SHA256 sha;
    sha.update(message);
    std::string hash = SHA256::toString(sha.digest());

    // lock
    MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win_log);

    // open results file
    bool did_exist = does_output_exist();

    std::ofstream output_file;
    output_file.open(filename_results.c_str(), std::ios::out | std::ios::app);
    // check if output exists
    if (!did_exist)
    {
        output_file << "epoch,thread_id,zeros,nonce,hash,message" << std::endl;
    }

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    output_file << ms.count() << "," << thread_id << "," << zeros << "," << nonce << "," << hash << "," << message << std::endl;

    // tidy up

    output_file.close();

    MPI_Win_unlock(1, win_log);
}

std::pair<int, uint64_t> test(uint64_t nonce)
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

void task(int min_zeros, int thread_no)
{
    // std::wcout << "Thread " << thread_no << " started.\n";

    while (!done.load())
    {

        uint64_t current_base = get_base();
        last_local_base = current_base;

        for (uint64_t i = current_base; i <= current_base + size; i++)
        {
            // pair[0] zeros
            // pair[1] nonce
            std::pair<int, uint64_t> res = test(i);

            if (res.first >= max)
            {
                // found higher or equal
                log(thread_no, res.first, res.second);

                max = res.first;
                max_nonce = res.second;
                // printf("[new]: (zeros, nonce) (%d, %d)\n", res.first, res.second);
                fprintf(stderr, "Thread %i found new nonce %" PRIu64 " with %d zeros.\n", thread_no, res.second, res.first);
                done = max >= min_zeros;
            }
        }
    }
}

void dbg_task(int min_zeros, int machine_id)
{
    while (max < min_zeros)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        fprintf(stderr, "[#%d] Local nonce base: %" PRIu64 ", Curr: %i (%" PRIu64 "), Goal: %d\n", machine_id, last_local_base.load(), max.load(), max_nonce.load(), min_zeros);
    }

    return;
}

int main(int argc, char **argv)
{
    // Init OpenMPI

    MPI_Init(NULL, NULL);
    int world_size; // number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank; // the rank of the process (aka uuid for process?)
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME]; // gets the name of the processor
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    int min_zeros = argc >= 2 ? atoi(argv[1]) : 1;

    printf("processor %s, rank %i, %i min zeros\n", processor_name, world_rank, min_zeros);

    // create base window
    MPI_Win_create(&base_buffer, sizeof(uint64_t), sizeof(uint64_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win_base);
    MPI_Win_fence(0, win_base); // wait for completion
    if (world_rank == 0)
    {

        uint64_t starting = 0;
        MPI_Put(&starting, 1, MPI_UINT64_T, 0, 0, 1, MPI_UINT64_T, win_base);
    }

    // create log window
    MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win_log);

    std::vector<std::thread> threads;

    std::thread work_thread = std::thread(task, min_zeros, world_rank);

    if (world_rank == 0)
    {
        std::thread dbg_thread = std::thread(dbg_task, min_zeros, world_rank);
        dbg_thread.join();
    }

    work_thread.join();

    // target reached

    std::string message(msg);
    message.append(std::to_string(max_nonce.load()));

    // get final hash

    SHA256 sha;
    sha.update(message);
    std::string hash = SHA256::toString(sha.digest());

    fprintf(stderr, "Final message '%s' ('%s') using nonce %" PRIu64 " with %i digits of leading zeros. Goodbye!\n", message.c_str(), hash.c_str(), max_nonce.load(), max.load());

    MPI_Win_free(&win_base);
    MPI_Win_free(&win_log);

    MPI_Finalize(); // finish MPI environment

    return 0;
}