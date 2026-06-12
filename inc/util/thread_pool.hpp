#pragma once

#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>


namespace Aero::RT::Utils {
    class thread_pool {
    public:
        thread_pool(int threadNum);
        ~thread_pool();

        template <typename F>
        void Enqueue(F&& func) {

            {
                std::lock_guard<std::mutex> lock(mutex_);
                queue_.emplace(std::function<void()>(std::forward<F>(func)));
            }

            cv_.notify_one();
        }

    private:
        void workerLoop();
        void shutdownPool();

        std::vector<std::jthread> threads_;
        std::mutex mutex_;
        std::queue<std::function<void()>> queue_;
        std::condition_variable cv_;
        bool shutdown = false;
    };
}
