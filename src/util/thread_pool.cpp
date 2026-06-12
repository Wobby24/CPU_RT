#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <iostream>
#include <vector>
#include <functional>
#include "util/thread_pool.hpp"
#include <cassert>
#include <condition_variable>

namespace Aero::RT::Utils {
    // constructor shouild create threads in loop like this, but then emplace back a forever loop via a while(true) statement that makes the thread check for work then execute what its given.
    thread_pool::thread_pool(int threadNum) {
        assert(threadNum > 0 && "Thread pool must contain at least one thread!");
        assert(threadNum < std::thread::hardware_concurrency() && "Thread pool cannot have more threads allocated to it than the system has!");
        for (int i = 0; i < threadNum; i++) {
            threads_.emplace_back([this] {
                workerLoop();
                });
        }
    }

    thread_pool::~thread_pool() {
        shutdownPool();
    }

    // Inside your thread pool's worker thread setup:
    void thread_pool::workerLoop() {

        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(mutex_);

                cv_.wait(lock, [this]() {
                    return !queue_.empty() || shutdown;
                    });


                // pool actually break the loop when stopped?
                if (shutdown && queue_.empty()) {
                    break;
                }

                task = std::move(queue_.front());
                queue_.pop();
            }

            task();
        }

    }

    void thread_pool::shutdownPool() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            shutdown = true;
        }

        cv_.notify_all();
    }
}
