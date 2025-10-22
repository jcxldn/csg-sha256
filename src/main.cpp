
#include <atomic>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <chrono>

#include "FileLockFactory.hpp"
#include "SHA256.h"

#include <inttypes.h>
#include <sys/stat.h>

#include "main.hpp"

std::atomic<int> max(0); // max zeros
std::atomic<uint64_t> max_nonce(0);

std::atomic<uint64_t> starting_base(0);
std::atomic<uint64_t> size(1000000); // 1 million
std::atomic<bool> done(false);
std::atomic<bool> local_base_lock(false);
std::atomic<bool> local_output_lock(false);

std::atomic<uint64_t> last_local_base(0);

std::string msg("This is IN2029 formative task");

std::string filename_results("output.csv");
std::string filename_base("base.txt");

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

    auto lock = file_lock::FileLockFactory::CreateLockContext(filename_base);
    if (lock)
    {
        if (does_base_exist())
        {

            std::ifstream base_file(filename_base.c_str());

            std::string line;
            std::string contents;
            while (std::getline(base_file, line))
            {
                // processing

                contents += line;
            }

            // atoi but uint64_t
            base = atoll(contents.c_str());
        }
        else
        {
            base = starting_base.load();
        }

        // append base
        uint64_t next_base = base + size.load();

        // write new file
        std::ofstream output;
        output.open(filename_base.c_str(), std::ios::out);
        output << std::to_string(next_base) << std::endl;
        output.close();
    }
    else
    {
        while (!lock)
        {
            // wait for lock
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    return base;
}

void log(int machine_id, int thread_id, int zeros, uint64_t nonce)
{
    // operations that can be done pre-writing (ie calc message)
    // TODO make func instead of duping code 3x.
    std::string message(msg);
    message.append(std::to_string(nonce));
    SHA256 sha;
    sha.update(message);
    std::string hash = SHA256::toString(sha.digest());

    auto lock = file_lock::FileLockFactory::CreateLockContext(filename_base);
    if (lock)
    {

        // open results file
        bool did_exist = does_output_exist();

        std::ofstream output_file;
        output_file.open(filename_results.c_str(), std::ios::out | std::ios::app);
        // check if output exists
        if (!did_exist)
        {
            output_file << "epoch,machine_id,thread_id,zeros,nonce,hash,message" << std::endl;
        }

        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

        output_file << ms.count() << "," << machine_id << "," << thread_id << "," << zeros << "," << nonce << "," << hash << "," << message << std::endl;

        // tidy up
        output_file.close();
    }
    else
    {
        while (!lock)
        {
            // wait for lock
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
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

void task(int min_zeros, int machine_id, int thread_no)
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
                log(machine_id, thread_no, res.first, res.second);
            }

            if (res.first > max)
            {
                max = res.first;
                max_nonce = res.second;
                // printf("[new]: (zeros, nonce) (%d, %d)\n", res.first, res.second);
                fprintf(stderr, "Thread %i found new nonce %" PRIu64 " with %d zeros.\n", thread_no, res.second, res.first);
                done = max >= min_zeros;
            }
        }
    }
}

void dbg_task(int min_zeros)
{
    while (max < min_zeros)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        fprintf(stderr, "Local nonce base: %" PRIu64 ", Curr: %i (%" PRIu64 "), Goal: %d\n", last_local_base.load(), max.load(), max_nonce.load(), min_zeros);
    }

    return;
}

int main(int argc, char **argv)
{
    int num_threads = argc >= 1 ? atoi(argv[1]) : 1;
    int min_zeros = argc >= 2 ? atoi(argv[2]) : 1;
    int machine_id = argc >= 3 ? atoi(argv[3]) : 0;

    printf("%i threads, %i min zeros\n", num_threads, min_zeros);

    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; i++)
    {
        threads.push_back(std::thread(task, min_zeros, machine_id, i));
    }

    // start debug thread
    std::thread dbg = std::thread(dbg_task, min_zeros);
    dbg.join();

    for (int i = 0; i < num_threads; i++)
    {
        threads.at(i).join();
    }

    std::string message(msg);
    message.append(std::to_string(max_nonce.load()));

    // get final hash

    SHA256 sha;
    sha.update(message);
    std::string hash = SHA256::toString(sha.digest());

    fprintf(stderr, "Final message '%s' ('%s') using nonce %" PRIu64 " with %i digits of leading zeros. Goodbye!\n", message.c_str(), hash.c_str(), max_nonce.load(), max.load());

    // sha.update("This is IN2029 formative task");
    // cout << SHA256::toString(sha.digest()) << endl;
    return 0;
}