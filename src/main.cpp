#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <cstdlib>

#include "SHA256.h"

std::mutex output_mutex;
std::mutex nonce_mutex;
std::mutex max_mutex;

uint32_t nonce = 0;
uint32_t max = 0;
uint32_t max_nonce = 0;

std::string msg("This is IN2029 formative task");

std::pair<int, int> test()
{
    nonce_mutex.lock();
    int current_nonce = nonce;
    nonce++;
    nonce_mutex.unlock();

    SHA256 sha;

    std::string message(msg);
    message.append(std::to_string(current_nonce));
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

    return std::pair<int, int>(zeros, current_nonce);
}

void task(int min_zeros, int i)
{
    output_mutex.lock();
    std::wcout << "Thread " << i << " started.\n";
    output_mutex.unlock();

    while (max < min_zeros)
    {

        // pair[0] zeros
        // pair[1] nonce
        std::pair<int, int> res = test();
        if (res.first > max)
        {
            max_mutex.lock();
            max = res.first;
            max_nonce = res.second;
            printf("[new]: (zeros, nonce) (%d, %d)\n", res.first, res.second);
            max_mutex.unlock();
        }
    }
}

void dbg_task(int min_zeros)
{
    while (max < min_zeros)
    {

        std::this_thread::sleep_for(std::chrono::seconds(1));

        nonce_mutex.lock();
        max_mutex.lock();
        printf("Nonce: %d, Curr: %d (%d), Goal: %d\n", nonce, max, max_nonce, min_zeros);
        nonce_mutex.unlock();
        max_mutex.unlock();
    }

    std::wcout << "Done\n";

    nonce_mutex.lock();
    max_mutex.lock();
    printf("Nonce: %d, Curr: %d (%d), Goal: %d\n", nonce, max, max_nonce, min_zeros);
    nonce_mutex.unlock();
    max_mutex.unlock();
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