#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>

#include "SHA256.h"
#include <atomic>

std::atomic<uint64_t> max(0);
std::atomic<uint64_t> max_nonce(0);

std::atomic<uint64_t> base(0);
std::atomic<uint64_t> size(1000000); // 1 million
std::atomic<bool> done(false);

std::string msg("This is IN2029 formative task");

std::pair<int, uint64_t> test(int nonce)
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

        int current_base = base;
        base.fetch_add(size.load());

        // std::wcout << "BASE: " << base.load() << "\n";

        int local_best_zeros = 0;
        int local_best_nonce = 0;

        for (uint64_t i = current_base; i <= current_base + size; i++)
        {
            // pair[0] zeros
            // pair[1] nonce
            std::pair<int, uint64_t> res = test(i);
            if (res.first > max)
            {
                max = res.first;
                max_nonce = res.second;
                // printf("[new]: (zeros, nonce) (%d, %d)\n", res.first, res.second);
                fprintf(stderr, "Thread %i found new nonce %d with %d zeros.\n", thread_no, res.second, res.first);
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
        fprintf(stderr, "Nonce base: %d, Curr: %d (%d), Goal: %d\n", base.load(), max.load(), max_nonce.load(), min_zeros);
    }

    std::string message(msg);
    message.append(std::to_string(base.load()));
    fprintf(stderr, "Final message '%s' using nonce %d with %d digits of leading zeros. Goodbye!\n", message.c_str(), base.load(), min_zeros);
    return;
}

int main(int argc, char **argv)
{
    SHA256 sha;

    int num_threads = argc >= 1 ? atoi(argv[1]) : 1;
    int min_zeros = argc >= 2 ? atoi(argv[2]) : 1;

    printf("%i threads, %i min zeros\n", num_threads, min_zeros);

    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; i++)
    {
        threads.push_back(std::thread(task, min_zeros, i));
    }

    // start debug thread
    std::thread dbg = std::thread(dbg_task, min_zeros);
    dbg.join();

    for (int i = 0; i < num_threads; i++)
    {
        threads.at(i).join();
    }

    // sha.update("This is IN2029 formative task");
    // cout << SHA256::toString(sha.digest()) << endl;
    return 0;
}